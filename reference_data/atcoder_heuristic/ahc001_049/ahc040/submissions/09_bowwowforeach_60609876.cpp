#define FULL_LIB 1

#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0
#define USE_LOCAL_SA 0


#define TIME_LIMIT (2900)

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

#define OUT_INFO 0
#define OUT_LOG 0
#define OUT_VIS 0
#define OUT_PERF 0

#define FIX_RESULT 0


#define TIME_LIMIT_US (TIME_LIMIT * 1000)

#define OUT_INFO OUTPUT_INFO
#define OUT_LOG OUTPUT_LOG
#define OUT_VIS OUTPUT_VISUAL
#define OUT_PERF OUTPUT_PERF

#define USE_ENCODE 0

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
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;


using TimePoint = chrono::high_resolution_clock::time_point;

struct ChronoTimer {
private:
	TimePoint startTime_;
	TimePoint endTime_;

public:
	inline void Init() {
		startTime_ = chrono::high_resolution_clock::now();
		endTime_ = startTime_;
	}

	inline void Start(int limit) {
		endTime_ = startTime_ + chrono::milliseconds(limit);
	}
	inline void StartMs(int limit) {
		endTime_ = startTime_ + chrono::milliseconds(limit);
	}
	inline void StartUs(int limit) {
		endTime_ = startTime_ + chrono::microseconds(limit);
	}

	inline void Join() {
	}

	inline bool IsOver() const {
		return chrono::high_resolution_clock::now() >= endTime_;
	}

	inline int ElapseTimeMs() const {
		return (int)chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime_).count();
	}
	inline int ElapseTimeUs() const {
		return (int)chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - startTime_).count();
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


#define VALIDATE_ABORT()
#define VALIDATE_ASSERT(exp)


#define VABORT() VALIDATE_ABORT()
#define VASSERT(exp) VALIDATE_ASSERT(exp)



#define EIGEN_NO_DEBUG  
#include <Eigen/Dense>



#include <cstdint>


struct Xor64 {
	using result_type = uint32_t;
	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return UINT32_MAX; }

private:
	Xor64(const Xor64& r) = delete;
	Xor64& operator =(const Xor64& r) = delete;
public:

	uint64_t x;
	inline constexpr Xor64(uint64_t seed = 0) {
		x = 88172645463325252ULL + seed;
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
    friend class CapArr;

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


constexpr int N_min = 30;
constexpr int N_max = 100;

constexpr int T_min = N_min / 2;
constexpr int T_max = N_max * 4;

constexpr int S_min = 1000;
constexpr int S_max = 10000;

constexpr int BoxSizeMax = 100000;
constexpr int BoxSizeMin = 10000;

#define DEF_CONTAINERS(name, size) \
    template <class TYPE>          \
    using name##Arr = CapArr<TYPE, size>;


DEF_CONTAINERS(N, N_max);
DEF_CONTAINERS(T, T_max);

struct Size {
    int w;
    int h;
};

int N;
int T;
int SIGMA;
NArr<Size> boxSizes_;

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




InputParam<int> ipN(N_min, N_max);
InputParam<int> ipS(S_min, S_max);
InputParam<double> ipTe(-1, 2);

constexpr
struct {

    PI(MeanShift, 0, -10000, 10000);

    PD(StdScale, 0.78, 0.4, 0.8, 0.0);
    PD(RandomScoreRate, 0.005285445150543031, 0.0, 0.01);

    PD(GridScoreRate, 91.58406650212751, 80.0, 100.0);
    PD(GridScoreStartRate, 5.340707441276768, 4.0, 6.0);
    PD(GridScoreEndRate, 0.41718641891804853, 0.0, 1.0);


    PD(SkipCountRate, 0.10711713075878243, 0.0, 0.5, 0.0);
    PD(ValidationScoreRate, 0.1655119182261405, 0.0, 1.0, 0.0, 1.0);

    PD(EstimateCountRate, 0.7821734904906724, 0.0, 1.0, 0.0, 1.0);
    PD(EstimateCountRate_N, 0, -0.1, 0.1);
    PD(EstimateCountRate_S, 0, -0.1, 0.1);
    PD(EstimateCountRate_Te, 0, -0.1, 0.1);


    START_TUNING;
    PD(CellHashCellSizeRate, 0.45, 0.3, 0.6, 0.1);
    END_TUNING;

    PI(GridHashCellSize, 42000, 10000, 50000, 1);

    PI(StartGreedyCount, 149, 1, 100, 0);
    PI(StartBeamCount, 65, 1, 200, 0);
    PI(FirstBeamWidth, 17, 1, 16, 1);  
    PI(SecondBeamWidth, 16, 1, 16, 1);
    PD(BeamProb, 0.17680233225249797, 0.0, 1.0, 0.0, 1.0);

    PI(TopCount, 2000, 100, 4000, 1);
    PI(BestResultCount, 450, 100, 4000, 1);
    PD(OutputBestRate, 0.5, 0.0, 1.0, 0.0, 1.0);

    PD(FindTimeRate, 0.9, 0.0, 1.0, 0.0, 1.0);



} HP;


#define DEF_DOUBLE_PARAM(name)                                                  \
    static const double name = BlendDoubleParam(HP.name, -1e10, 1e10,           \
                                                {                               \
                                                    ipN.GetRate(HP.name##_N),   \
                                                    ipS.GetRate(HP.name##_S),   \
                                                    ipTe.GetRate(HP.name##_Te), \
                                                });
#define DEF_DOUBLE_PARAM_RANGE(name, minValue, maxValue)                        \
    static const double name = BlendDoubleParam(HP.name, minValue, maxValue,    \
                                                {                               \
                                                    ipN.GetRate(HP.name##_N),   \
                                                    ipS.GetRate(HP.name##_S),   \
                                                    ipTe.GetRate(HP.name##_Te), \
                                                });

#undef PC
#undef PI
#undef PD




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
struct Zobrist {
    NArr<array<u64, 2>> rHash_;
    NArr<array<u64, 2>> dHash_;
    NArr<NArr<u64>> bHash_;  

    void Init(Xor64& ran) {
        rHash_.resize(N);
        dHash_.resize(N);
        bHash_.resize(N);
        REP(p, N) {
            REP(j, 2) {
                rHash_[p][j] = ran.Get64();
                dHash_[p][j] = ran.Get64();
            }
            bHash_[p].resize(p + 1);
            FOR(b, -1, p) {
                bHash_[p][b + 1] = ran.Get64();
            }
        }
    }

    u64 GetHashR(int p, bool r) const {
        return rHash_[p][r ? 1 : 0];
    }
    u64 GetHashD(int p, char d) const {
        return dHash_[p][d == 'U' ? 0 : 1];
    }
    u64 GetHashB(int p, int b) const {
        return bHash_[p][b + 1];  
    }
} zobrist_;

#define USE_SORTED_IDX 1
#define USE_BIN_IDX 0  

#define USE_ACTION_HASH 0
#define USE_COLISION_HASH 0  
#define USE_CELL_HASH 0      
#define USE_DENSITY 0        
#define USE_TRY_HASH 0
#define USE_EARTH_SIZE 0




struct SimPos {
    int x1;
    int x2;
    int y1;
    int y2;

    bool operator==(const SimPos& sp) const {
        return x1 == sp.x1 && x2 == sp.x2 && y1 == sp.y1 && y2 == sp.y2;
    }

    void SetInvalid() {
        x1 = -1;
        x2 = -1;
        y1 = -1;
        y2 = -1;
    }

    bool IsValid() const {
        return x1 >= 0;
    }

    int GetW() const {
        return x2 - x1;
    }
    int GetH() const {
        return y2 - y1;
    }
};

struct Sim {
    struct Backup {
        int W_;
        int H_;
        int t_;
        int rightInserted_;
        int bottomInserted_;
    };

    NArr<SimPos> pos_;
    int W_;
    int H_;
    int t_;

    NArr<s8> rightSortedIdxs_;
    NArr<s8> bottomSortedIdxs_;


    void Init() {
        pos_.assign(N, SimPos{
                           -1,
                           -1,
                           -1,
                           -1,
                       });
        W_ = 0;
        H_ = 0;
        t_ = 0;

        rightSortedIdxs_.clear();
        bottomSortedIdxs_.clear();
    }

    int Put(const Size& size, int p, bool r, char d, int b) {
        int putL;
        int putT;
        int putR;
        int putB;
        int W;
        int H;
        int collisionBox;
        CalcPutPos(size, p, r, d, b, putL, putT, putR, putB, W, H, collisionBox);

        pos_[p] = SimPos{
            putL,
            putR,
            putT,
            putB,
        };
        W_ = W;
        H_ = H;

        ++t_;

        {
            auto it = lower_bound(ALL(rightSortedIdxs_), p, [&](s8 a, s8 b) { return pos_[a].x2 < pos_[b].x2; });
            rightSortedIdxs_.insert(int(it - rightSortedIdxs_.begin()), p);
        }
        {
            auto it = lower_bound(ALL(bottomSortedIdxs_), p, [&](s8 a, s8 b) { return pos_[a].y2 < pos_[b].y2; });
            bottomSortedIdxs_.insert(int(it - bottomSortedIdxs_.begin()), p);
        }
        return collisionBox;
    }


    void TryPut(const Size& size, int p, bool r, char d, int b, int& W, int& H, int& putL, int& putT, int& putR,
                int& putB
    ) const {
        int collisionBox;
        CalcPutPos(size, p, r, d, b, putL, putT, putR, putB, W, H, collisionBox);


    }


    void CalcPutPos(const Size& size, int p, bool r, char d, int b, int& putL, int& putT, int& putR, int& putB, int& W,
                    int& H, int& collisionBox) const {

        int w = size.w;
        int h = size.h;
        if (r) {
            swap(w, h);
        }

        collisionBox = -1;
        if (d == 'U') {
            int x1 = (b < 0) ? 0 : pos_[b].x2;
            int x2 = x1 + w;
            int y1 = 0;

            RREP(ii, bottomSortedIdxs_.size()) {
                int i = bottomSortedIdxs_[ii];
                cauto& q = pos_[i];
                if (max(x1, q.x1) < min(x2, q.x2)) {
                    y1 = q.y2;
                    collisionBox = i;
                    break;
                }
            }
            int y2 = y1 + h;
            putL = x1;
            putT = y1;
            putR = x2;
            putB = y2;
        }
        else {
            int y1 = (b < 0) ? 0 : pos_[b].y2;
            int y2 = y1 + h;
            int x1 = 0;
            RREP(ii, rightSortedIdxs_.size()) {
                int i = rightSortedIdxs_[ii];
                cauto& q = pos_[i];
                if (max(y1, q.y1) < min(y2, q.y2)) {
                    x1 = q.x2;
                    collisionBox = i;
                    break;
                }
            }
            int x2 = x1 + w;
            putL = x1;
            putT = y1;
            putR = x2;
            putB = y2;
        }

        W = max(W_, putR);
        H = max(H_, putB);
    }

    bool CalcSafePutPos(const Size& size, int p, bool r, char d, int b, int& putL, int& putT, int& putR, int& putB,
                        int& W, int& H) const {
        int w = size.w;
        int h = size.h;
        if (r) {
            swap(w, h);
        }

        int collisionBox = -1;
        if (d == 'U') {
            int x1 = (b < 0) ? 0 : pos_[b].x2;
            int x2 = x1 + w;
            int y1 = 0;

            RREP(ii, bottomSortedIdxs_.size()) {
                int i = bottomSortedIdxs_[ii];
                cauto& q = pos_[i];
                if (max(x1, q.x1) < min(x2, q.x2)) {
                    if (i > p) {
                        return false;
                    }
                    if (collisionBox < 0) {
                        y1 = q.y2;
                        collisionBox = i;
                    }
                }
            }
            int y2 = y1 + h;
            putL = x1;
            putT = y1;
            putR = x2;
            putB = y2;
        }
        else {
            int y1 = (b < 0) ? 0 : pos_[b].y2;
            int y2 = y1 + h;
            int x1 = 0;
            RREP(ii, rightSortedIdxs_.size()) {
                int i = rightSortedIdxs_[ii];
                cauto& q = pos_[i];
                if (max(y1, q.y1) < min(y2, q.y2)) {
                    if (i > p) {
                        return false;
                    }
                    if (collisionBox < 0) {
                        x1 = q.x2;
                        collisionBox = i;
                    }
                }
            }
            int x2 = x1 + w;
            putL = x1;
            putT = y1;
            putR = x2;
            putB = y2;
        }

        W = max(W_, putR);
        H = max(H_, putB);

        return true;
    }

    void Revert(const Backup& backup, int p) {
        pos_[p] = SimPos{-1, -1, -1, -1};
        W_ = backup.W_;
        H_ = backup.H_;
        t_ = backup.t_;
        rightSortedIdxs_.remove(backup.rightInserted_);
        bottomSortedIdxs_.remove(backup.bottomInserted_);
    }


    int GetScore() const {
        return W_ + H_;
    }


};

constexpr int SimSize = sizeof(Sim);

struct Candidater {
    NArr<s8> upCandidates_;
    NArr<s8> leftCandidates_;
    NArr<bool> isUpCandidate_;  
    NArr<bool> isLeftCandidate_;

    void Init() {
        upCandidates_.clear();
        leftCandidates_.clear();
        upCandidates_.push(-1);
        leftCandidates_.push(-1);
        isUpCandidate_.assign(N, false);
        isLeftCandidate_.assign(N, false);
    }

    void Add(const Sim& sim, int p, char d) {
        auto IsUpGuard = [&](int lineX, int lineBottom, int boxT, int boxL, int boxR) {
            return boxT >= lineBottom && lineX >= boxL && lineX < boxR;
        };
        auto IsLeftGuard = [&](int lineY, int lineRight, int boxL, int boxT, int boxB) {
            return boxL >= lineRight && lineY >= boxT && lineY < boxB;
        };

        int l = sim.pos_[p].x1;
        int t = sim.pos_[p].y1;
        int r = sim.pos_[p].x2;
        int b = sim.pos_[p].y2;

        RFOR(i, 1, upCandidates_.size()) {
            int q = upCandidates_[i];
            cauto& pos = sim.pos_[q];
            if (IsUpGuard(pos.x2, pos.y2, t, l, r)) {
                upCandidates_.RemoveSwap(i);
                isUpCandidate_[q] = false;
            }
        }
        RFOR(i, 1, leftCandidates_.size()) {
            int q = leftCandidates_[i];
            cauto& pos = sim.pos_[q];
            if (IsLeftGuard(pos.y2, pos.x2, l, t, b)) {
                leftCandidates_.RemoveSwap(i);
                isLeftCandidate_[q] = false;
            }
        }

        if (d == 'U') {
            upCandidates_.push(p);
            isUpCandidate_[p] = true;

            bool ng = false;
            REP(i, p) {
                cauto& pos = sim.pos_[i];
                if (pos.IsValid()) {
                    if (IsLeftGuard(b, r, pos.x1, pos.y1, pos.y2)) {
                        ng = true;
                        continue;
                    }
                }
            }
            if (!ng) {
                leftCandidates_.push(p);
                isLeftCandidate_[p] = true;
            }
        }
        else {
            leftCandidates_.push(p);
            isLeftCandidate_[p] = true;

            bool ng = false;
            REP(i, p) {
                cauto& pos = sim.pos_[i];
                if (pos.IsValid()) {
                    if (IsUpGuard(r, b, pos.y1, pos.x1, pos.x2)) {
                        ng = true;
                        continue;
                    }
                }
            }
            if (!ng) {
                upCandidates_.push(p);
                isUpCandidate_[p] = true;
            }
        }
    }

    const NArr<s8>& GetCandidates(char d) const {
        if (d == 'U') {
            return upCandidates_;
        }
        else {
            return leftCandidates_;
        }
    }

    bool IsCandidate(char d, int b) const {
        if (b < 0) {
            return true;
        }
        if (d == 'U') {
            return isUpCandidate_[b];
        }
        else {
            return isLeftCandidate_[b];
        }
    }
};


struct NormalDistribution {
    double mean;      
    double variance;  
};



struct IOCommand {
    s8 p;
    bool r;
    char d;
    s8 b;

    bool operator==(const IOCommand& c) const {
        return p == c.p && r == c.r && d == c.d && b == c.b;
    }
    bool operator!=(const IOCommand& c) const {
        return p != c.p || r != c.r || d != c.d || b != c.b;
    }
    void Output(ostream& os) const {
        os << (int)p << " " << (r ? 1 : 0) << " " << d << " " << (int)b << endl;
    }
};
constexpr int IOCommandSize = sizeof(IOCommand);

struct IOResult {
    TArr<IOCommand> cs_;

    bool operator==(const IOResult& r) const {
        return cs_ == r.cs_;
    }
    bool operator!=(const IOResult& r) const {
        return cs_ != r.cs_;
    }

    void Clear() {
        cs_.clear();
    }

    void Push(const IOCommand& c) {
        return cs_.push(c);
    }
    void Push(int p, bool r, char d, int b) {
        auto& c = cs_.push();
        c.p = p;
        c.r = r;
        c.d = d;
        c.b = b;
    }

    void Output(ostream& os) const {
        os << cs_.size() << endl;
        for (const auto& c : cs_) {
            c.Output(os);
        }
    }
};

struct Pos {
    int x1;
    int x2;
    int y1;
    int y2;
    bool r;
    int t;
};

constexpr Pos P0 = {
    -1, -1, -1, -1, false, -1,
};

struct IOServer {
    int turn_ = 0;
    s64 minSizeSum_ = 0;

    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        is >> N;
        timer.Init();  

        is >> T;
        is >> SIGMA;
        boxSizes_.resize(N);
        REP(i, N) {
            is >> boxSizes_[i].w;
            is >> boxSizes_[i].h;

            int minSize = min(boxSizes_[i].w, boxSizes_[i].h);
            minSizeSum_ += minSize;
        }

        double Tr = T / (double)N;
        double Te = log2(Tr);

        ipN.SetValue(N);
        ipS.SetValue(SIGMA);
        ipTe.SetValue(Te);

    }

    void Output(const IOResult& result) {
        Size response;
        Output(result, response);
    }
    void Output(const IOResult& result, Size& response) {
        ostream& os = cout;
        result.Output(os);

        istream& is = cin;
        is >> response.w >> response.h;
        ++turn_;
    }

    void Finalize() {
    }

};
IOServer server;

template <class T, int W, int H>
struct Imos2 {
   private:
    array<array<T, W + 1>, H + 1> sums;

   public:
    void Init() {
        REP(y, H + 1) {
            sums[y].fill(0);
        }
    }

    void Add(int x, int y, T const& value) {
        sums[y + 1][x + 1] += value;
    }

    void Calc() {
        REP(y, H) {
            REP(x, W) {
                T s = 0;
                s += sums[y][x + 1];
                s += sums[y + 1][x];
                s -= sums[y][x];
                sums[y + 1][x + 1] += s;
            }
        }
    }

    T Get(int l, int t, int r, int b) const {
        VASSERT(l <= r);
        VASSERT(t <= b);
        T s = sums[b][r];
        s -= sums[b][l];
        s -= sums[t][r];
        s += sums[t][l];
        return s;
    }
};


struct GridScore {
    static constexpr int CellSizeBit = 12;
    static constexpr int CellSizeMask = (1 << CellSizeBit) - 1;
    static constexpr int CellSize = 1 << CellSizeBit;  
    static constexpr int CellSize2 = CellSize * CellSize;
    static constexpr double DivCellSize = 1.0 / (double)CellSize;
    static constexpr double DivCellSize2 = 1.0 / (double)CellSize2;
    static constexpr int GridSize = 2000000;
    static constexpr int CellCount = GridSize / CellSize;

    array<array<double, CellCount>, CellCount> cellsScore_;
    Imos2<double, CellCount, CellCount> imos_;

    void Init() {
        imos_.Init();

        double a = 0.174613419;
        double b = 492223.861;

        double predictScore = a * server.minSizeSum_ + b;

        auto CalcValue = [&](double x, double y) {
            double v = sqrt(square(x) + square(y));  

            return v;
        };

        double maxValue = 0;
        {
            double y = CellSize * (CellCount - 1) + CellSize / 2.0;
            double x = CellSize * (CellCount - 1) + CellSize / 2.0;
            double v = CalcValue(x, y);
            double s = v * square(double(CellSize));  
            s /= 1000000.0;                           
            s /= predictScore;  

            maxValue = s;
        }
        REP(i, CellCount) {
            REP(j, CellCount) {
                double y = CellSize * i + CellSize / 2.0;
                double x = CellSize * j + CellSize / 2.0;
                double v = CalcValue(x, y);
                double s = v * square(double(CellSize));  
                s /= 1000000.0;     
                s /= predictScore;  

                cellsScore_[i][j] = maxValue - s;
                imos_.Add(j, i, cellsScore_[i][j]);
            }
        }
        imos_.Calc();
    }


    double CalcScore(int l, int t, int r, int b) const {
        double score = 0;

        int begX = l >> CellSizeBit;
        int endX = ((r - 1) >> CellSizeBit) + 1;
        int begY = t >> CellSizeBit;
        int endY = ((b - 1) >> CellSizeBit) + 1;

        int begU = begX + 1;
        int endU = endX - 1;
        int begV = begY + 1;
        int endV = endY - 1;

        if (begX + 1 < endX && begY + 1 < endY) {
            if (begU < endU && begV < endV) {
                score += imos_.Get(begU, begV, endU, endV);
            }

            double rateL = double((begU << CellSizeBit) - l) * DivCellSize;
            double rateT = double((begV << CellSizeBit) - t) * DivCellSize;
            double rateR = double(r - (endU << CellSizeBit)) * DivCellSize;
            double rateB = double(b - (endV << CellSizeBit)) * DivCellSize;

            if (begV < endV) {
                score += imos_.Get(begX, begV, begX + 1, endV) * rateL;
            }
            if (begU < endU) {
                score += imos_.Get(begU, begY, endU, begY + 1) * rateT;
            }
            if (begV < endV) {
                score += imos_.Get(endX - 1, begV, endX, endV) * rateR;
            }
            if (begU < endU) {
                score += imos_.Get(begU, endY - 1, endU, endY) * rateB;
            }

            { score += cellsScore_[begY][begX] * rateL * rateT; }
            { score += cellsScore_[begY][endX - 1] * rateR * rateT; }
            { score += cellsScore_[endY - 1][begX] * rateL * rateB; }
            { score += cellsScore_[endY - 1][endX - 1] * rateR * rateB; }
        }

        else if (begY + 1 < endY) {
            {
                double rate = double(r - l) * double(begV * CellSize - t) * DivCellSize2;
                score += cellsScore_[begY][begX] * rate;
            }
            {
                double rate = double(r - l) * DivCellSize;
                score += imos_.Get(begX, begV, begX + 1, endV) * rate;
            }
            {
                double rate = double(r - l) * double(b - (endV * CellSize)) * DivCellSize2;
                score += cellsScore_[endV][begX] * rate;
            }
        }

        else if (begX + 1 < endX) {
            {
                double rate = double((begU * CellSize) - l) * double(b - t) * DivCellSize2;
                score += cellsScore_[begY][begX] * rate;
            }
            {
                double rate = double(b - t) * DivCellSize;
                score += imos_.Get(begU, begY, endU, begY + 1) * rate;
            }
            {
                double rate = double(r - (endU * CellSize)) * double(b - t) * DivCellSize2;
                score += cellsScore_[begY][endU] * rate;
            }
        }

        else {
            VASSERT(endX - begX == 1 && endY - begY == 1);
            double rate = double(r - l) * double(b - t) * DivCellSize2;
            score += cellsScore_[begY][begX] * rate;
        }

        return score;
    }

} gridScore_;

const std::string base64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

struct Embed {
    int N;
    int estimateCount;
    string encoded;
};

const Embed EMBED_TBL[] = {
Embed{30,96,"IIIITIIIIYAAIJLYaBaaAQYabKTaJaIaRDBTIaITBaBaLaTBIDRLYbRSIIaSIBaQDZbTCCZYCJYaIRDIYbCRKLISRKITDaRYBKTBTIZKAaBTKSDIRJAbbYSRKKJRQDTIZRQbDTAYQDRLLSDDBJCYTLabaCRKKAKBAJDBZKRCbKQJBYSICLbYaKAbBBACJCRTJLTIDLCAKTbRQIBRZRaIDRZaJSbYKaCYDZabSKSBIYAZYJRKCZTQJIDZZBCSQILJAQSbCDDaTTLIgbBLDAYZLQKDQCbQKQbAJYTZAZDDkAJQCLYECTQZaLJKgLAQZRQbCITZMRAbEDCRLMZKQUALUZBjSETbBcAcKEASLKUbIBZjIhBEKZCgcSRiMBSTTRREgAJcZiQkacbhSJJQTciCKaacaiMbBDTBQajESCLTRgZSZbbBJiKUJEJDiDSiRIYYEJCQQiJbLaCibDDDhJbLYLTTSAULEQSQbhDRBiJLIKRESRSQIbKSUJTgaRKTUZIYYRZSYLSBIQLSQaJQAIJaMAbTZKaRTIZZTKaDaSbDgIAIbhTAJDagYhRbAYCLKCZBBATJaDaKICDZJQSKIQRjQQbLAAaAIITQLSLLZZTCaSLEZTLQBYZKRDaLALIADLYDBDBCZZZILYCQaBSDabKBDaIBCZRBDjQATKJbSLQKhaRLRSbAZYYCJbZATDSYTJQYIAaDITCabDKIbDQIYhQJLJSQYCKIRIaSJRCaaSZLBbILBQLJKYCQJABDJJSALYTIBRaDYbQAaYDKaSJKAZZYaSJSLQjQZLTbDYAAQIKabRIQbYbRCYbbYCATBSLbDCbATLCLRAKbQZaCJLhaYKTLQQDIIBDaSZJSQabDIRQaIJYKJLLCCRaAacLKSTABRIJYSBARaQabLYIBbTaJTCSIBAZCLaSDKQTLDLabZIZbCRRQCQRDIIYbSAYSCCJBZILKIaCSKZIIRLLSQYaSARZSbYDYLAYKQZAJLCIQYTBBDBJIAKRITAYKJabIDaIQSBaTJSKaRBRDSRZABTJYaDQKLZRQJTJJZJKICYBaCYAaTJbQBDYZQRbBIDRAJCSYbYCLYRTCBLJCKbLRZZaSDTABYSaaaZBSRTBJLAYABKJKYICbBQIQAYJLSAYIaAbSLZKaRYDSQATLTRLRYDILILQTSZKBCbCZJaDDIQTBIBKQADTJYbKZCZTaTCCJJAaaSbZaDJBbBSAIKDDBaYZBDDASYZDJbKCQYJIDbLISSQBIRKCbSSLCDDLZABKQDZbYKITARCCbLRLRTLBSIKZKISKYbKbSISIRZaSJKbSTRAZKKCDQZJZTAaDZQBRJZQRJRIADAYRTLCaIAKYZaICRabDKbYTYaZDaJQTBRKQBCAJBLSKKCJAKJJBICLZaLTLRDAaSSYJTYKBBAJZaQTQKKCaIDRLCLYAZYIZS"},
Embed{32,102,"IIAITLQIJAIIIJTKSRYbSaZKIZSaabCKIbJDBTAZITaBITAQZTBLTZDbTZISBSSAIBDSbYSQYLaaRBKYaKSBLDRYSLKYbaSRYRKDIIICTaYJTJYCKDIRBIaDSBTYDRBJbKTQRKCZKaRDQZIDKRaYIQCbAAJDKCZbaYAAIbbJLSBSQBCZQLKQaZBALbDQLKAbIaTZAKBDTYaSJAThKSYSZBAJQbAJTYbRRKZKYJQDICZaALQZKAZCQaZYTQBLDLIIaKKRKSULZRQIZDQYKLCKCYIJCJZCbgLAIIRAKATBRbQaCTZLIALTYLTKCKCDabRbCZcATJCSAKLgZLJJYJTiBTTiRCBZBYCKCbDJcQYYBcLTQRQMbKJRBYIIUKDCgCaKQRRJjcjjCATKcRYRYabTAEbIABLISMMDaRJaCLMCZDgTgEIhAKZgLYJjSUIBUcBiUgIJBARRBIaEEbKbKaKcLLbCgaALESMYKQhLIRcUihUQZZcaQQKZZagShBhBCAJUIQZYDCIBjKjZJRCDSQUDQJQhiLbbMCcMAbbLCMCYRQScTEZDSDRRbgKZLSQYKIKZQCLSRTLbLIJCATbCDISSTIJaQBaBZTabgJALSDMTKaQBRQILJLbAJSjUSCagJCIhKJTILSMQQLRLYDTCSKDEcJaTTBEDaJIATbKSDhYTBLSJSaALCJCDDSDZagCTDZJaDLYaYLRbSJDYSDKQAYLbTQABYBBQLIYIBShYRBKcBZCYLTTbLBQYKJIJDAICjbSIAYZQabiJCYLLbASaYTCRLZDTBSKZYQbYAABaaKbLYaabCKRTSJLBIBaIJaYABBBTaIbZJZKaCTiSCKAbYDKKbCSIKaARYLaTQRLQQQRLTRbBAKTZTDBDJaQKSTTTRTTZDLRZCDYRQQYIIZhYCZJKIYTRBBQRLZDQYTaDZTJDAaRSSZTIabCJSRSRTJZbCbCBJLIbDaRJDAAYSSDCSgSYZDDIDSLKRZYTAZAYJaYZTLASaAbJaRTAbIZBaTbQDYCDQZICRBJZaBbJaBRAZQQKDbaTISYTRDDLIaQLYBKLSDZDTSaaYbZKDRBSLTTABSJIADIJJJLSSQBaLSQDSTJaTJDLSZKQQRKaCLKQYKIaJIJKRILACSYLZaDCJIbDKBAIYKaBDKITbDKQLBSRaJRaSaQIYIBBBJQJSJTDRZJQYYILQAaLYYSaRKCAJZLYQaDYYRACABLCBBZAILZJICBBRLRJRaDQJIBACSDLLTSKbBBKACJbKIbQRZYDBSABaaJSQTDKDQLILJAbJYAaIaSLALJYKTQCKZLDQbDRbbACYZSBDDDCBIQBSIbQAJDJbRRbLIBJICbSaQbKQTDICCbbCaTDITaaTZbKAQRQZSZKRBYbTRLSaSZYALDKZLSJIQSRbDRDAaCIBbZKaBYQAQAJQZaZZSTLBbIRabTaKAaTZICYLRZAaRJLDAYBSYDaAbTRQaaYLLYABTaYBLCAIbYQSYBRZBYKCTIZKKATZKQbDARJKTRIYRZBKDAYIRYZJSTLLATAYJQBCJAIBRbLQCJRQYKRTLKTQKRLIYDbZSRSAKIRYLIRKAYKAAYaQbLRJBZabJTKKYCCQBLLKaKSbQLRKJSQLbACQLZC"},
Embed{34,108,"IYQYLIKLYKIYYAJIIaAaaZZDDaaCbaCCaZIaRbBTYKBTSBIAATbaTBIDRZIDZAaALSKQIBDKaYSSJJYDaLQCTSITZIDRaKBQZDYKBaITaBQKLYaRAIKbYbCaBIDSLLRRabaAYBRCabLKRAQbALDARTaRBILBYQaYBTKTCbZaAJaKaRaKBJIDACbIZQTQQIaJJIQTABLZDKRTKKYLADKTIZCSaBbBKICDSRBJZZYQRBBbLALRIYTABTCYSBLIYZQKSRKKRJQSLRYTabYbRTCJDJSBQITZaALTDSDADYRCLYALAJZAJbBAaCABRIRJDDLjZQICKBJRKQLRYYiZbAaYQZYLCKBKQchLCZKLgRBKYJRAaabKScZZSCJIYaTCbCCBLQhYTSjKLLSDJiALDUDIchaAAbAZZRKBJRBUCYKYiaLZQMBiiQSYBjjSLKQZjjLbCAEZTIZSUMQEIEJKbbYAKBBKLDcACZCBRjagaMabBKZETCbSIYQbDkjYLLBDkEQJJAhCJLKCBJaAkCMiRLbQaIYMCkUjShKLQAZDTQbRIbJTIkMgUAKbgCDBbhSMYYgMMRajTDbSRARgJQYRZTMCLMKLZJQbIYSSgYUbSQJRbCYbBcSLSaETjJYYLaSKaLIRaZUSAAYSJbELSRiSMLRLDQCYjaLJAQSLAQbAZLiZLZSJKBTLTCITSCgAQYIQQEUQYbbZJJaQAMSRaYbIDZjRDABMTDYKUYDLYYYBjbBEAYaCYKSZDiDILKRCRERZRABcJBDJYATKaJDRAJbABJBibAIQbcZiaTSbRDRbJbZQKSQRBKCIIRCBaCABTJaJLQJJYJAQTRYDDbTSKKLKSYKDBKQbAaaYJgQAJDYaSQKLBIbQBLJCLJhTKaaTbDTCDSZYhLYYTLCZZITCbDIDIaajCKYCKSZbKKQLLQIKAZKQQJJSRTDYJLBQTQaIIbSTAZZbCYDTDLTSJSALQCQZATKIRZYLZbaTTISLDKQRBRaSbIDATIJQDYJJYQQJKKIBSJASRTbSbaTJbRADaIIbAaZDKILQKSTLSRYaaaQZZBRBKDCRSRbaZLCAAYDSQTZKJLYYQYTTIZLTBDiCJahDCCCRYYYZLbTJLRDYSSKSDZLQRJCabRIZSADQbLZBaRZTQYKDSDaAaDLRIDCTIJRKDASDDKYaYaLQbAQKRDKaDRaBbKaSLZbQBBKITRQIBYKKIRQBDTJRABYJATAQRaCJRJJaIZBBRLBCAJDTbKQBSbbQJBJLSAQSJZQLKbaYCIBYTQZbaBAKYKBZLDQCTbZSLADIKKLTCaBSYYLaTRKRLCRTDJCIRYKAQCTbALLSYbJRRDKLRAYBBKTaRbaRIYQYILQZYTZSCQLIKRBJQAYBILATBSZAIZKLLKZYCLRRCBLTTTTYBKSBRTbAZTCYBQLbICBLCDSbZDbIRLTBYSYCKSZDCBJTDIbJCSaSYLJAILACRYJLbQCRAbbSaIYDZAaSKRJLbaKYYABCKbJaJaRIJKCSDACATJYJCYIBBQIJLJLZDbCCIQSJCTRZTBSKbAJTTBITSaKQaIRLYJaADRRCbKITKALDQYaRBSACIZRRQaLZZZaKDDSDTRBYZDBDJSYKDJDJRaZACSYBbDYISZBRBaaZaBbQLQYCDQTBTQBQKIQZZJSDKJSJJLKCADJKJaSLaRSIKIRbSCJaLAbTICTLSBZbTDSTDJCTKTCITTbQCYTSbZKCBRRZaTBKYRJLITISQSKDDQbQTYKSQLJZRKQDZRIIQIYYTDZZIDBJQRDBAKBSIBQZRIIQLRRaaZYCaYKJaTaLaSZSSJIKZBYLIIKJDSASDALY"},
Embed{36,115,"ITLJAJQJSAAIYICAZJKKaaBDaaaDSaSaaRZbQaYDBTIKIaRDJTBaYKaTZIYQaILRSQbQZZLCIBZRDYKLDKAZKATSJASIJTKDAZJRDZTCBIBCITKZTDBQCQZDLZSKLSaIDBRaKbZAITYJBSDQIISDaZSQISLKBbLALBbYYTTBRLYDSBAAQLTZYYDaYYBJSKRbSTYJBRSJAJJQRRKKYKCDCZTaQBYYRaTJTRTITKKLbAKaRQRIZYQIDLDSBTIJLaYSIYYQJRYTQLaKKZIBTDaAZTJBRRbaADaKaCaTIILaCRZIIZABARJYJYABLDBSDSSTCBbYCLICQcbKURIBKDRDgbYZMJKQYbbQCCRbYTQQKBIbCYSAYZYCQRJZDKDTRQLaKYaKAARaYJbJYQKSBKTaZABBCbbcKCIaBSaSTaAEcBZIMYhYYbSQDUYhRbhYDDIQDaaQTBDRLUILDAagaLRTQZYCbBSgChBThIjBRTCIZLAKJgiJjhBYYSiYDCDRQLBMhaKQhkcRTiRJRhAIkYkDBCYhUKJTQcLTLMcBAAahAUKjZRQCJabMiULDkaJDQjTMSDDITTQKchbZALSBMEKbCLLbRMbSDEMQKSAcSASZLZYbCZQUgbhccYaYJQBLUDEJICccAJaUAIRZDCCARRcLigLbYLYJCIQJIEZYkJKYALTQASagIARbbYjTYJMSLLkaQIQKTDYDAjJEjCLRcQALZaAIDLcBhASUJLIBLDJSDCZZQURDSKYDCbbDKZDARLCcYRICMKKBJgLJALSDJRCCZYQICDCBCTKbEDQJRDKJSbYSDIJjLSDRCTEbTIBJAJCaaZJZaLcSSQUQKSCLbEYIQTDYZADYRJYbMISLKSSRRbCEKJLRIRIYIJQSJYBKKaDSBaRLSSJISBKLKaaDLBIYgBaQQSILRDaTabRRRTZYIKRZbDTYDZKggBaTKASBQAZRbIRAaaKgbIbDRIKSYJZIbLCLCKSYASCbRSQACKbJJSMZCYZLQBSZDLDDJMSBDZBDBDSTJCSCLRYZKbSIZSTZBCQJKbICTbJBSSaCbBCLKaJKIQZCRDYCaTRZAIbKbgYTCLDaZZJAQBYRTYKRAYAYTZQJTLAZaKQCBLQbBRCaLKKaIKCbTLYbJSACQbDaZKKQSKTTZaJLRDLASJSZTCQCDJSJaBbUBAIQLZSbAbLcBTRYJTYQSaLQAJILQAAbaKDbbTZBbRBSaRATDCIDZIJbQCLbKSDKLTSTbQCIBSQYJDLKAYKbTYbCQYYBLKQLBAaAJCabTSJYBKZCRZQDYLCBIRZTKZDAQCAabDKZDAQZDIKISDTLKDCDaCIbCIIIKLBICJQIJYbYDaKCIIKTaZbCRBbCQaQDYQAZLIQJaYabLbQJBTRaTQaDSKZTBJYZJZDQbYSCSbSDKaTYDYaKbbTLCSLAKITICALRCTaRIBKaTQQCBJTbZKQSZTYJSAZLLZIDYSDIbRBBQYTYDBBZSYKYTBISbaCIJRBYIBBSIYYJKBCJCBaARJbTZISRQZICBALDZAYCLaKKKCaaaRIRJYRZQLTSIQZACDJSaBQCTQRbYYLIZLCKYZYKSJBQTaQYaDJLZSKYBZYSLCCQLQLDLIBBSDAKRILAITbJDbYZYQIAaTCLDJKBYSAaLRbDCaZJTCDLAbBLRCIBZKJIRSJLQCJKARTQLBJYYQCBbKZKLYaYAbTCSYYTTQQbbTQTKSJbaaRYCCCYIZDbDbLCDYDKLKARIJYJTaKCJDJAABKaKTJDKKIbAKaaJYQRBRCQQIbDLYZZSKbQIaDRYZISLSKARZTKRLDSJDKLQLSQDRbBRZKKTSCYabLZLRACYLYRCIKKSSIATZTaIJIJRABRZQYKQaLKQZIDbRLYAYBbCDSQbKZDLDTKCTILBQbCAKDBITIJQZILLSALIAaaSRRDQTTSZLDJYaAZIQbIKaCKRCDJQZaJDYCJACBabZJSbSJSJKBKaRRITSLYTTYBARBBTJIDTSYADDLIbIQSQLJJZSICaQZBYBJDDLKBDQaKbIBYCYIaJSSTTZSBIIKAbIZDQRSLQADJQJ"},
Embed{38,121,"QYLARIJDQKIKATLILYIaaaCaaZZZKaaabaQCaSIaRDBTIaITaBJTBaQCBaSBJDJJYCZIaAaTAYCbIRaRDYKTLKYIaQTZAJCaBITJTYDbSBYSKBAbILITDYaZZBKBKQKJAbTSIbITBRCLBBQTYZIRDJSCKKBTYBDLQIQCSYZLTLYYARaKLaQTIDKJBCDIZYKIIIISbAJBBCDTTTQZDITbZQLaLSSBbQLKSRSaCbQLIITJaQKZQRCYKJAALaBZSDYKIAARRIBbRTYDBIbCTQDSZLTTJbCCIaLDARZSSTZCACLYYIZZYaYZAAbSBBKKSDZBbTBKQTCLIRbBaJDRAZaTRRJBUaZBBhCaDTCTRQIRbAaAbabLCIaDbYATIBRJRQDTbJaCBIaBQTERZJSQjKZIUSBATbRCYbLbBQKKTSKBKJaabLASgBbYCgBQbJZbaAQYQBaKRCbUSaRSSLLBBcAALMKhDRYLRSQSKLaTQLZKTcCbDJgcZDCaADLhbDQCSjKBjaJCMBKYLDCURjZjIZjQSMRBbQZgZCCJgICMiDbSELbZKQajYTKEJDZUAEQLDKhIAQaLhIIkbSkjCARZCbcJJkBRUCBhBBbKRZBZCLBibUbiEQAajaiJSLjKaQEYBCkbTICBjcikbAAhDCCCJkbLEcYLRIAKYTZEahjCcIgKLJMQihLckgMSRJQThIBSIIKQTUhABBDhZQJLBjShYTkLLIjRgaTEIYYAQgTRQDBJQaZbAAbRMBAaMciEKZShBAhEAbJLTZADDJaCSbLRSJRJgjSKBaIcBiCJADLLChRJhabQaTAQJaBTBASKaYbjIDLJKKRIJCYBJJTUbRSbRhQIIDSRjaYCTKbbCIiTgbDKTaQIIRJZIkBALQgDYagKLaZbSKTBaBQARYSLZaScBCZaRiYZiTQbTABcAZJCCTUaICJTCaYJbLTLKZKIaILbSBaQIKBLSSZRLSRBIKAIZDJRKKLQJILRAQMASDTaBLbLRLDQBbYSSBARLjRITJZQbLBQSDSSIKBYZKYaZBZASJLiIIBJbZSJRAIJCLQIQSbTbAACZQADSBbabKaaLZYZTITCTTICRDIKDCJRZRCDYSKBYSSKKZJaJYRBBCQCcabLLTLaATQRaZABZSAYCQaRbIQLDRaJDIDgABTaIKKZRTRBIIATJTaARaBBQAZQKDaSYRJLJKDKQZKIZCJQBYTADZAYCYbSbZZTQDTLJSaCKTQBKKacJSAISRDILJTbaCKaYIJSRITbLTIIASRKZKIZZIQICSacbRCYLaYKRKILaTALBLKbaRJCQBIQCSLRTDZKaQJDTZjKKCYRZSYTTDZbYBAbSCQISIaaAAJAJZIRSTSTJRZKZRbDKYCYaYKQLABAbRAKRKBYaRSLAAZjabBIZKKZRIZDSKJBDaTBZKDCCAJCALCaYLTKLZBRKJYYabQLSJKTCDJBZQRJSIaAKZZARSLAACJbbAZLDDaACTIDZIYJTAIZbZYDKYIQIaKRZaJQIRYKcIAZZJASTLKAKYISRLaZRATaSIRBbCYTRLRDJQLATSSRaKQYJYBQRbLRIYAKbIbADBDIbJaRDRYISYaACbLQSRJLICIIDBQTDBJaIDaCaTbIKbYZJDCCLQAbJLTYABBKDDZbCCTCaaDZZRTBZYTASJQLADKIISJQBTCTaAbALZBIaIZaLIARIZTADCbCKbKTZBIKZLSYCYQZQBSBZDKKRIRDITITTTRYTJCKTbDALaKaKTZBTSRADBAaLTCaBJLSaDDJAJIAIKaJADCQKJDRYKLZZQACaATLaZJSCQZYJCDbQbDLQIaTIaTKbLYaBAQIIaRBCbDACCZTSSBRbDYYTRbZZTKDSBabQTLaQLaDDIRQJaZTSDACJILRZSASYbLJbLaRJITKSAJYSTRJQIYbQaATDCJbaJbaQaTAEDSaDDaQZYbDLbKTaTRASIKaTCLTCALBKbSZQbALbATbYIRZQCIRYRKKAbZQBKCCKaZKLbTKKBJARDJQRTISBbRDRabaALBKQICLZIAILaRKABRARJCbZSALZCYDSABJQALRTRKbYJKKSISbbaRQLRIDSQIRRJIRaAKKIZZQQYTZAYAAYQLJYJABBCZBAJBLZDJaDLZLQbCaKRCQKSCATQKaJSDbDLaRQBRYJYQRQYLATYYZJCIKbaZSRCKQJBDLDZTSAKbabAZYYISZJaIIITCLbISDYQRaQbDYBLLQQTTIIBabYAaLbKIQBRLIRCZBRTAARYZAbCBDSDKKLIZSRLKLYSYAbTIAYbTJZDIZCSY"},
Embed{40,128,"YQLYICIIQAYLIKLIJIIIZSSSaaYbaDYDSYaaSZZJIaRDBRIaITaBIaDRYKDRCTBICZKJDQJaBaITSJYTIBDSaYTQIIZZRBCQLbJSaITBICRZCRLQKBaTYYLDbRKDQIKDRTBbSRJAZTDTKDIBCLaLQCTAQZZBLIKCIBYRTBKSaLDACKYLJCYZaDRaYTAIJZDaYLQACabBIKTIATaRQbZSKBJKCDaZaaaRYYbLSISRIIIRYDYLKTIAZSIRbARBaTAbATLKYTQLDAbJJCAKLAYTJTBBYIIJJZADLRSISBAKSbYBKKRIbAZTYRQbTKSBZKICSIYQBTLITaZLSZSZRCBTYKCIaJAJCKbTDTLYLSbCZTCDQDbYLKaDRDRLaCIaKJbTQYQKDSYKALZiCZDaSCBLJTBLYCBZCDbYKYAbaQZDaKCSbZSaAAKiLSJZITAbRMYBAJTTLYSKYYRTCCbIQKJDJJECYcCbULYYAZaaTSBJBTCZiYJLEAJCKECiBCbDTTLYYTLiISRMZJJMabTgQRBAJaaLSUTKaQIZDMRUabKChJMYELTUaLQATJBQEihBYYBAJRQcUIQRBIScRcTcJSBbaYMKKZELMQIQTEKTZQZkREZACTLUCBDRDYQBiETZMZUBQRSDYhBBhQcIIQUcDKcMALQTiCLAaZTLUkLEQRKkSJKgTQQbIJKQbjMChMkhiCjZTDCjhKUTUacSRBLIkbLAUJMDaYAcJUiESgjYAjjDZYjZUbECEJTQRgcLQRKLLcaikcZUSShBKASgAQJRkYTBMALDZbiaKSKBZLCZQiCRIaJZicQRRaScgkcJCLTICBbRDQSBjKKZaSaCIDSZAJIcZEBRSRCACgAaYMKCZEADLZJRjYUZZIaIaSDBbKjSBiRUiMMjKbRSSaRbLYDgBgbJDASTCJTaELiAYSIaCSDJIDZLAICBADYZZTBCZJiKLJCYIKJbACZaSKSYDREaDjUJSbbBYUYTaRZKTYBYKATDYbRAbTQURKIjDiKbgLQbCbMaZYJaaKLBKCIKBZCSSLaSCRBKLDKjbBYSaYYIIYMZKTLDBbaASgAQIYCKITZKCYALZRIRKBRDTCAIAJbILBCbIDLQQYSUbRRaSQLCTCAQSLBJRTbRZMBJYiBCTJbKjDLQRATARbjZcZZRJSCgCQAALBTLRZKTJAbTDbADaBZbYALQDbKQIBYYQABYJARLJKBIILaBaaQbAKQIJDbbRDQJaRSIZRJSRTJThSTITZTSYaBYJDSSJQSaABIJRbCKbDIbKYaYIbTCSATYCbTZZDbQaAQTKTBTITLJDSTJDMDIBIKZBCQADQbbbCACTAbbKbKKLZKJQACZDCRAASJLICbAJJBBBCZZLJKRCCCZCAJJQRYCAYbbTLaDAZJSYRCZZRTIKIDIYbBSATQICDCDTaBRCJYYJaDaKRZaaaKUBbaQJIYKKISCZIJTQRILaBCCTRABJILCaZTLSLRRKJZCTRQLYJQQRTRCIJRTTACSBZbBAZQJQIbQLbTZbBBTLAYSRLLCAQKSaRTITJTYTLCCTQKKLaaAIRDRIQabRaJaZCIBZDTSAJaSZYYAQbKKRbbbTYQaATbLTKZYKSKTBLQRAAbDbZYYQRbYCBIZCBYaTKRTKbCJBATBICaAQRKDJJBbZJbJCSKRSbbBTIDYZQABZIZDJKaKICBSJAIaIKBIabDIJaKCBRBTCDBBKJQRRRKCbQbJTRZCQQABTLKTCCYCbZSLRaaTDJAIIbRQKQKZbKZCbDIKDaZQIBASbZJASJARIZALbSYBIDQAYKISIKRabKZDQabDAIBASZZDbLAZLLTbQCKICSARQKaASBJTbRDBDDaKBDLKaKQDBRQZaSSZBLZDTCaSKAJQKDQaJZaAZaBbTQRLYYJSBQBbSDBJCKJaQZTaACJbRSIZRKIBSBLQbTIYCZRZIIADDCBRJJYYLQCLYSLKDYZJARAbLTAbCSSIKTBBbZLDSYCTRCDKBKbAIICQTbIJBSLIRIYADLQbCbITKKJJaCYIITBKTaQBaSbRTTJATCQJYAJbQLaZKbYLCICbDbCITDQTAJZaaDKaCLaAKSJSLRbKQRLBQKCaRKTTQZaYaaZLYKITJBKLbJTQSRTABBYDCYCZLRJKQKABZQaLAAaLZITKaLbCYDRSCJbLKQYQISYLTIIQbRCQRQZKLBKZbRYALYZBALTQDCQRSLTJbRSRITbYJSQBICDIKKQZBbaKIZbQAQbSaSSCDZLYaKKIZCADCZbCTBJIQIYJRAIIJIAKLYYTJIYBARDSABCJATCQIJQLaBASQLLZYDZTJYQKTJDZCKTSIRSYKALaIAYJJBIIZJAKCSYbbSBCSLBbJbDISSIBSQaJALRaLRCIBTBSYYCZKbIITDTDYZKRRLDYKBaBZSAAZTYSZDZIDCQYBRJaILDaYYCSAaBIDbCRJBRJKTDBDIbCSSTJSITIJTZJQaQYSBKDYKYbJQTTBDLCbQLTSCTTaCTQZIBIDYIQKZYCZCJZCKJaZRYaZYRYSTTRKTKQIAaLYBabQIJJYDQLKYSD"},
Embed{42,134,"IYQYAIAIQLITbLIRIIJIKaQaYaYILaKaZaaaDaaaaSIaZDBTIaISaBKYTBADDRJaTBITRKYDRIaCRIaYKRDZJBCSaZCaBRQLRAKRDAbbKaATBLDQKCZbTITJaCYAZAIDKAZCZITZABRTaAZRQKLaBILITbBRIKBQDaaDLbAQCSaTDITRbDIAJIJKZQDYRYDAYRKTSBCbaJBQJTbBKTaKYKRBbCIbILbSAAaATZTIKSZbaSBAALRKYCCbJKBDKBRKDILDaBaKDQRQYbLKCYRTDaARRIJSITJQRZYRBYBKDKZAIABLCQZbIaBbLSZaaZbaCZQYaTCRZLaQCLLCIIYaCTTbCaLYRTBABTZBaYKTQDZJIKQRCRSZJCLKJQQJCSYKISSbCCKRZLLYbBISJTBQKbQaYbZSAIBBKDSaKJSCJSBYZSZSLDKLaQBDRbCYRaKIbJAIJKDLYIQSKYCaShTTTLRbBJaBQRDSIjBKQKRABBDAQCbYJLIRbTJhQZIKbAcKZQBbKCSKJSDZaIYSJYTLCLiTLBIcLAASCTYaDLMbADSiRYCDBRaiTILaTARLRCAZLSBhaDEIRZiAJJZRQISIiKRThhSQELUYJCcIACbhQBKcURYRYKYiRbjaSDRAKTUJZZBScILEhAgDMCbCBRIZRkBKRBiCKRSSYRDaMKDkUhhRbabQRAATYQTRDYbKSTIKichCgCSJZTQBJMSAKBkUTJJBJBahgEUCZEZRbUKKCQMEZKYbDgcgQSJZIRQbjEjRDCgbbMRaMELIkJIQZTgAbDQbcCcTUTCKDQgYLZRRjjEbUQchBZSkYCAIZYTcJECZQgKgJATKKLDACCEjZjYjiQBbhUkIBYLLDaRQChhbaagKQQjKkQBbQcKChjCAIAMKbTgLAbcAQYbaaZYiTJMQSSbADjAkCAALCUicLIBQkQbAbbMLJaLYBECYcjBcKDjDBBYQYgAZaSbDCBKDjTSjYSCYZaZSgJcbaCSZaKbDTZRDRCTASZhLYZiZCbQYLALShASQDIhaLDYSDBZQSAKabSUBhKKYZDJiDMRCSKbSaSaJZCaaaKCSLgKbbJQQacTZRTSDAbbSjZJRDIKTaRIDbESgKSRCTJDYZYCbDZJSLSBMQYQCBSBSBJBbLZIYjKAbQaIbAZZChaARABDJLATKYDDCcbJJAQBJKTaAZCgZDCKhIDBYSACRaSahaZIIZaTZIDYBLYTaAYbIAIKLTKCDaEKDJTYQBALJIahTLKCQBTZDADLAZTaLSCTIJIDaTKRTSRLabBBAASTZKLbZIcCAaKQYbRISYLCTIZTJAKaQYJRIcbQKDRYRYaRAITJJaIYCRaIQYJCTLZIKJBAZAZQLCIJDJQbaJTBCYKKaYAZAYBaRhSajSCKYZbDRAaILAQJSSKIJZTZJTBYRLIDQKIKYSBJZBJDbAJCTSDYIIbTTQATCbSbLQQRIjRALJSRKZZCLJYIQQQDBTbKQQbaDLJYZSbACDARDCSJJBZLAILCbCaZIZSSgIRSZKLKLTJCCDAKDDDgBAJSILJSaCJIYLIaLILBSYBRIRBTCSJQaaJbSYRaSJaBJCCBYJDaRQZILQZYQSYLAbDRbbZbCAKJDACDZJIaDLLKLQZSBQQabCTAIJBQAJaabAKbDCLQZBIbaKQRbSRTRSYYQZLBIBLQLITYaTDKTJKTAIDYJbTISSKSKKIBKSRRYDZaRSDLRIIQBIDbLRKCRSBbTDDSDQYZYICDLKSZYKCbRCTYDKbYCKATTSaABKTbDLDIYJBRDDQCbJBSCSZZYACBLTaIZbBSbCJQQaJbKKITbRZKLABYLLQLYBDKbDQDRBTIKbaSSaRIDSZKLZBSaCYSSZYYTbJLYaKQSRITBJCCLSSTBbIKATLARJDbbDJLIKRCSAAKYKbZaIBRDTBQRTRaYQCYBLKBKZLJJJYKCLAQQSQTDYCRCYYRDRKIAJYaBZYYSTBIQQaAICIQLLZaJJYIKLQZCQDRKDQaDIRaJaZBZDBJaIRCYYCKbZCJRBRbDTACJBKIBCDBaTCLTIJATDRLaaTbACBaQbQZQQIALYIBJCbJYKaJSJCKAQYKbTaTRBJRSQYbYLRAQYCJCDCKILZYAbAbRCBLaTZAAJDYJTIDLKCAJQTCJYDIaTSYTTJbRDaIYBDJJCQQIDKRRIBbDSLTJSCALaaaIJJBLQCCKRDLCZaaTYaRZZCTKQCZaZCDbIQYZRDKCRBbZZYIDaQQQIKJJZTbZaYSTQbIYSAbSYTICJbZIAZRSaSDaSLAALATabZaZQAaSaBRDTDZBLYRIDIAZCBCZSTBYJDRKYaCbRYYTSTaCBBbbKKbbAAKZCKIYbbAIBAIJAaQDJYaRZYTYTRZAZaTQCBBaZLIQAbLYRTKYQCIbRLYCASbSIbaYbBLIBaZABLICACIISbSIRbLbaZBICIbYTAIBJBDQBZCSZYBBSATQJLTJaQTCLLaZbAKDCSZKTRAaBLZZSIDBIZCQLZRCaDbTSDBAYYBTLaDRaISJRYBQZBRRYTBCKZQLQBTDJbLCKYSLTQYYSAKZKKZRJZSQKaKaQSbaIYJKIaCSLRRZSKYbITaRJbQYAKATKbbSaLQYJJQAISJKTSALTJJBRSRbKCaRbCQJbYISCYITADZRBYKDAAZbCBJJSAYSKSKKbBCDYYaSYbLJKSbKBQTYaaYKaYCBRTKBDLIbTLJKQYCaAbQbQaAJABZLbAaZIQBBCJYILLQSSQaJSTJKSLDRIIILQaDJIAJITQZACbIaASbJLRBCDILQTZRKKAIbLLBLIDaAJCZQATSBLA"},
Embed{44,140,"IIIIIIKIJYIKZIAIIIQIIIJaYBaKaSaaaaYAbaaCZCKJAaBTBTQSLTaJaQRBYLDRIbCTJKaTBIaBYZIaQDKRDYaBaDIbaAITYDJKTSJaQRYTDTIBbaTTIIBBaTbBCZTQKCYbYRTLAYbDKJATZSbCKDYaQaSITAYBQTTbSBLDZBKQJIZKbKaQKDTQBLAYDCDBRKSZTJCZSQJYbBZSLBIQCbBSKKLYabQYDZAKBYbYYLBTISYBRCYaLBDAJJTATLICTYYKCaBKKKKBQZDTJRaRIbDQbBAQDTaIBZRbKQYSRbQbbCQLKKQCICSSLDCZaITJBTDCIJZABbYYBTKJSZCbQQIRLQDRIaaDIBCAYJJYTCLQSYDaJTbRbCYTaRYaAaIKBLAZSYLQTJJLCLASIAbKbAKTBZZYaZSBYDJCZCZSTYJQDCLTZYITQbSCQaJKQCaABKZITSLTabAJZYaKBIDKLQCYIZSBaSDQDjBZILYQSZTCbCBJZMLZAADYZSQBLTTbQbZQIAAIISSDJZJJBRiJLbKaDLSCKaBaYACLcBTaQabZDZYBCITQaAUIAKbBYYIEbTaCTRIYabKBRRSIECCSKCiZIhQbIbLIabADhBkaaQLQRaCMLEaAJBSCZZYTbaKJZaiYRBDLLgjTSSbARSIBaDLhSRbBZKSiDYBADKTjMYQAYLahLZAiacIRRCZbCEJSKQTBLCjIgDRIYQYRbBRRLkIJCCBQUbgJZZUKbUhAiDKIaCYMBTRACMUDLIKKMkIgLDQBJjLiBSUUULJLDQCMaBaUKZCJcLQTECASJjgJkcBDBaDZbgJMIQaZRgCLUgcDSCjaALiaLhBgIcZSYEkITbcbAIMbJCMZIaCDgBkCMQbccjUQQJcCCJMLKQDKZkaiZUKRThSRhDELZKRJaaTRMMIMQgRkUMLLbaSaMIQkjAjZBUSSBiCIYLTLZKBTRjBUEikBjjIKQCDbKLiakELQZYJScCEiULAbEAhKILEiCgbIRhLLhYSRMbRTBBbaQKZSJJQCQUBIjRiakYQgJaKJESaZLDSITRLRYSbAbJYDIYbRBEAJiaQJKRZRIYcJgITIBDIRYABLBSKLSiYRJbJDhaBISaaTISbCAQIMLaZDBJQQiCCIhLaTYKQZYRSJLRJUKTYQTBgDYAJKRQQZKbAQYabaUBRTQDLTaZIBDJYTKCTKCUJRLYgBgYKUCaBTaEDDaRKAMAKTbRbKDIYKSQJRZAYIbbbTaIbYDYLCTCZSaREKLYADYSBLRTARaYZIKBEYJSbJJYCLALIRQCCbQICZaDTSQLSZDDCDZaaZDKLASDQBJIRTCYZDZDTDUAQLjZAAKKLKLBTDDCUZLCLKYCKZIASLSZRKYbJQSAKbZKSZCKMLSRZRaaRBDRQAJTKYQAbJLTYYBaaBTSaTaQJAIKJbBDaRDSIYaSAjBaJYaDDLAabTSJIQDRICYDRIIbAIiZLZCZRIBIaBKRAUZTTLAYLSAZbKRbZCYZJBbJTQYSCBQbAARbCZBMRSDAAYIIQYYbLZKAJJKLCbZCCBJbaBjaQSAJRCZaCaLBaICCRDJRAJYBZIKbTAAQQJCQbJBZDaaDAKALKJZTKAZbIaQbSSbYQBBZAYLYYZCRDBYDTKLKIaCTKQTaaYZaIJRZaYLYYTQDYTCJIKDDbQYKZJTSaZbQRCbaaDRAYTBRCjYKSCKZJTIJSDTJRTDYTRQYJQILbSKIBDSAJJTBaIZBSACDBaIQKTSJLJQJJYYIKbbbZTLDaDSDaIKDIaQQCDKARbYZQDLQQYATIKCSZQIJBLSLBaBJCILBDYJCTCASabLbADLALARAcBCARJbJDQBRBbJDbSBIQRYLSSSaTTKYaJbSZbJTYKBYTDSbRQQISabSTJSBaIYIAbARBJKaZQLBSIBYCJLBLIIiSJbSTQBQRCRbCBSYaKZYZICKSSZBKDCBLDbYKaAAaRZLBTCQSKRYBYZLaaaCZaIBaJaaQBIIRZAKSBTbKLSRCbACRCQQLJbLDZAJQJbbTSSCYLZJLAKbYCaCAYCZARBKZZBCBJQJYDRaZAYTAKRRTSAbZbCYBYTbTBBaJQQAaZbZSZbRQDTCBQTbJQRQaYaDIQBJJLYQILDAbRBIACaBITbLaQKCIZJAaRBDRJYSSKKBSAISbSALJabSJKaKZKTCCIDTYJLDbQJCKTLIKISBALaCASbbIJLZZDTSALYTITCRLDTBQJaSDSCJABZLSTISJaDZQIaZAIKDJTBJZSAYIQJSJKADIQZTIBSbBbRALDIBBBZRSIKAIQZQDKIQLRbBLDJBQLAQAKRTRLLDYIJbQSYBZDSJKTaRCLDICKBRIbbSIKCDIZKTaYJQbYABQbZCCTRLKLJaKQTIBaKLaIITDYLQKAbLbIAZbKAIbIJQQLTJRADaRAabTKbIRRSSaQCKAabbLaDLYAYaSLQAILDBDLIYaKZARJJIaKIYTIDBabCTYBYYaJSDYKLSJDSRACJZJRRQRAbaJTAYYRYCAJIKAYbYbSLQbTIIQZADRbKaaRYZSQIADILRTYaLbDADRIZYBRKACRLbZCaQIJKBLDQDCILICICBKLATLZZTYAZaBJCBTCATIRIRIZKIATBQSLbbQYKbICDDIDTZRKYbQKITKbTTDJYLQTCCKKIKSJIKQICJYAZTLDKKSLDLSaABaJARCQZDZaBDBBIAJaDLKQZQBDIKDLJQDbLKSDaQTLIRaLTTTTZBSAbAbYBaQYJAASIYDYbRTbQIKLJRTaTQAJDJAbSAKZAaCQQRbJaaYAbbYbCCDYKDJYSYTRKaJbSKbQbACQAAZaSSRDZAKJLYBLRZTRBTQYSJZDRLCKaaBSZYYJKDbaaCSARaSLRQLQRLLbYBQSDAIKKABSYDbbATLJZIJTKLLIARCKKBQYQLaZKaJTZAaCQBAZCaAJKDbIJBTBSZYbQLTKDCYDCRaBCDZLbCRRKRIAITJJCRbZSTKBATaCBbTBQBYBSaBLSDbaIZQLBLSDJRKbAbLaRaaALbJSLDaBCQQTRJAQJRQYILQCYRRZRLLZATCTLTZCKICQTbDZRQYYQZBL"},
Embed{46,147,"JIJJIIRQYQKSILLKLIQICIIZCbYKaaaJSaaSaCKKaYaZSKIaBTITBaITaBbISbYCDBIYRaTBIabAZaATCIbZCCRDYYKZKITAYCTBIZYBBJaRTIKbYSDYZTaKLBIaaILBQBRBDATKQYLBITaAaDYDBYZTQKJQDAaRbYIKDRYDaLKQKKDQRZTRBDCLLTZaYJCATBLQTBCAZaRbQCIbKKIDRRDKBQZSZLZSAbCKAYYaRZZQJCTYJBQDAaTBaQaZDKTbLDYBIQaKDKYbIaSaBSRDaZaSLaDSTALZLKSDJALKCTDIZIKAZTbTYJAaYQJCIQaIbSaaADAJDAJCRTDLIKYBLTBRaIaDSRCSQKLLSZZJYKBSAJKLaDKTAJaISZABaTaaLbKQIAJIIYZKTKYBBBbYKaRKSSSDIbKTQZTRSZYQZJKSIQBCYIJALISLYYaZJBIAQDTICTSJCLbYLJYSAILKLTTJATBLDKaKKCZbBRCbSIYZSIQYBILbTICIIDbbaBRITTZDaAJBaACaZQLQSbLRAYYQRKCBLaJTKJCCKTbRJbTLAADYRYUJQSKbRZADbKSYBKYSEAhhTLJSbYAYIaTCLAKCCRLQDRbaDYABZYaYAhQbYZQBZhaTICTTLCACJcQJIKJSCJCIhCLYaJajTBISJaTCSZJYRDBSJLACIRYQZTDLKhhYQMJIZSABgTbIabTaAYbTEgRQjSQYSLLSbMJKKAJRSTEhEYJQaLgDYCJJBJLQSkEZKBBQSiUIDRIDSbgYDZggIBBQjMQTjJajZIEjKCDcKKRTMhIQAJJYBKZJRYQIbIbURTiTQKkIajgABcbLTSiLZCLDELCiTQUJIgZIbIcLEACAMhIDIcKEZZKUCCCZbbiBRQKRQYYCSgRRackLkQLRTYcaQbSYLCcaTCgMCMZMQTbEJgRhDDihUMYJbcCiJBaaIjRDBYZSCkKbDBkEKgSAhiMcCaQQajAgIDIkBMbTJZcMIkcBMSKYLTjSahaaaBAYTSEjIicJSaKhKSMDSCkbBZUhhETiabCMKLZAcbJaRQKCjLkDgiAAjLIiQEZLijJRTJgKLQgCkTZDiJbgRiaBSgQALKIZUiKLUIJSgSQKRZJgaAZCZcZUZTcSKCJcEhbQSRYhJDSYLIAJARQIMabBMIYYTEKJibbSabTBSRTYRKSEMSIJYDDabgCKYDLLAQiBAIZBKZLaLKaKZiYETaATKDZCYaMIbcagBKUIBDKQTCSBZBCiSKbQDYLcLJTYJDSICSRThZEZSRJSLKbAYDATKZSAQLjQTIJDJSRbJSICYDaYgaYLkKjLaICSZBDLZQRATCTJIYAZCLbKCDLLSQCAZBZAIIBIKIQATZDIIUCICRQTYRIZLTYYCbQIQZLKjBZTDDJKJQCSbQQSLYSZJYCKDIbQAbKgKTBbJBQAaZDQJaLaQJBaRYaIBIaTbaIAIJYKDJbTKbIYYTJSRYTKKLQiQIQYYRACBBAJIZLJJLDAYRDTZBbRTbQQRQKSISLhRbgaLQBQbTSACYALKTZZTLKKQKSJZQTIDRBbQRSLLTUDASJJCJYbBYATCUbRCKRZJJIAYAYARLYbATJQREYTBKYADIRAQJZCLCBLSaaZTDTYiCZYISBbRASQaCKAJDZSbJLCbTJCaRKLaTZYbTYTIKRBYTKZQYRQBDAIAIYDaRTBKDZbSBQICTDLIBITJJRCKRJBQTALRSKhIbASUaAICaYLYSDLTYKbEQYDbBCLRQIAKZZAbZBCQALbCRIKaYTYKaSZTCaDKCTLBZJSSYLKRKSEbCDBbJIDSAJaALJSQTaaYCTbKQABCLBBBKRCQRLbRLJKSSRILICbRDJZbDTbJSZRRAQLLACJLbLTSSRAZhQDSbQRIBICCSLbEIJbBbSSYKLLTYQQBABJRIaQbQCQBRKZYITDJKbSCabZZKYQIATSRTbTJDCCLRDCASJaZJRQQIDbBYDJbYKSKaQIJDCBAJIbJJAUAALQLBKIKIBDBaTTTZBKJIAcRIRQLRDIZITCZCCZTTQZRQJTaLBCAJYaYTTTbSBABAgQYaZbQbYLDbaJCASCZLKJaALSTDLIYIZRCKSbDTbYSTTKbbRDBCBSIYbSIbIbIJaaBDAaIRRALYQASbQaTCBBbQIJYYYCJRIQDISZaYKBQTLaKYIJZZCbSDBSADCRaIDZTRBBaCRDCBRbLDRBaBSQaCaZIKDRCJSaQCLQBRaRJZZSLJSKLTSBLTCDYQTYJAQLDYIIYbZIAKZRZQbASAQLbJaaDCJJBBbLbbIBCYABSTCCZSQbIAIJaRTZJACQTZABZDZSATBQDCAAACLSbRRLbTRBZTaaZbIIZKSYZJaCSLTDDbAYQATLYCDKCLDKDaAKJBDSJBLaDRKRbQDSKhZTARBTaCRKbbaBKTAaabISTIARLCRCLAIaAZZAbJQJbZKDQbTaADaALCbaCQZKRIRLYDJSCIIaDACBYbJTQDJaDJYQaARYSTYKILDabTQTTbBYSQKZAYKLRZCSYLRbARLLIKYJTDYRaSQQDZBaDCQYKJDKLAQbQTSAARKbJYZbTLLAICYBSLSALQIKBQSDYKbYZJABITLQbTCSaaSZZIYYZLbCAJTSAZKJIRRKZTQTKSZYKIQLRCSYZAaZSYLIZCRbLIYKIBDTIbSAQTBZZKDACZYBbISKTACaQLIaTJIISZJKKAIZQCJaQTLaaDTSIYSDbaaIIRZYZQITKISRIIRQKBJSBSILJLBIYaAQZKRRQRBKDLZZCZaSSJaTIaYRAIDICRQaYTALCaKJTZIBKaaYTDaYZaLRBIABaaQbTAKZSaKCZASLTZJCLLIISIKALSbBAJTTCYYSKTDBJbLDKRBZACIaQKZYbDQJIJRBKJARIBQJRLCCBSARADKSZBbAbBDDCIaZDIZAJbATBRQDZYLRTTLbYRKSQIISAISbATRSJRRCDaJLBbSbCZDCYKJaDZQCKZbSJKAAIBQKLTSRDaQRJYTSRKLJIaKRbAZaaLTaACZBLTCZZCQYZKTSDYJDaBCIJBLLbbAKaaaACLRaTYQaJbSYaSIaYBRRBIABBDbCYYQaBQRJJaKARJCQQBTJJISbLSALAbKbLZTBQCLRYYDCLRJSSKADIZTRYSCRIbJbKKJRQKSIIYBQSaBAYRIabaTTTLZJTQIaKCZBBQZADRJaSbTCYSbZKLKSTLRKKKaLAKTCKYZCJKIaYARTALIIAbBJJABTaDaALKJALKaRLACYCZIJaDZZBBRQKQQSaASLJQYLQbBTZCJCBJbbTLSDYILDKKALAZZaTBKSBZCTaRLTSQSIZKJbAaKJKKaaTaAJKRaCBCRbLTZKTBIRRDSRCDYIDYbCIDYRLYYSabKRCSCLJ"},
Embed{48,153,"ILRQJIYLKAIQJICIKKKAIJIIaaaSaKSKaYaaBabaCaaaZKKZIaZbLLBaYTSTaYRDQKCJTQRCaSBICTIIaRLIZBTIbARKLQTRaISBIaTBICTDLSBYTDJJYQJbIBTaaRBIKBRLBBAbSQZTaBaDLDKTYALQKTJBTQZAZQaJBKDSYZKAYBCbDSLYJaJSBIBQCbRIIBBYTaTTQIYJbCbAJRRYSaJBaYALBYZQQCJSbJKTBbKbBDSaaTTBDRAaJJIAYaYLDRKDZKBCIQSDSLKZJQLZaDLBDCYbABIKaBYCZIbbaQQQaKBCbBbLSBYRIaIBRQJLbIaSTTBLDQRAJQCbCDZARDJZABaaIAQaZBaTJTJQRDQYSYAaJbaDRKJQICBLJDSZYQYQIZLYBYBLKSTLRDaaKZCAaTbSRBBRZADBCLZQKTBZDQQQLBaBAaYaDQbSbZQYITYLLASIYILaDYRCBTCaTCSTbIbBISIJAZDDIZLSZBaABJKZRCTBSCLQaYITZTBAIZaILRLCIRTIYCQIKbaQQRDKZbDSLBCBTZJTSLAJIBCIDBJATbKDKLBKATLCZRIJQbICAJLJKKZbYIQACLRKTTYJKDATIZaRTRSIQbLSaSJcIQZBbTIIKARaAabIDIARTSQZDTLjBDILabCDZYBJMZAYDKSTKSRSaDSaJDISbJALbQJcCIYYQTSIQJJbCIKTBBSQKQbBURJLhkUTaTESCITDgILjRbIRAZbSLIBQSIcjZCgCCLLhUEKLQTBLKTLSkLYJTRTTCCKICJbbIEaRQhRKIbAZJSSIhQCZbIgakcDMZcDgBLRCbUJYDQBSQbLYQShIUJMhBJSBSJRgBbQiZcCEjUCaaTaYCRKZIbMbMDZUACCEgbTYkJRaQZQCjTiCDiZLaEBBcgLZJIaASKLAaIkIUIQhDbjgYjLDETTCQLiKYbRBYSMCJjiJcTYYhcABZJQZCCbJYUAZIhcSQgIBLJiSgbTEAEZEIgTaUaRJhJQccETaADZTDiSbKKCDhBikhgMKDcMaaLbiKQMDUjYRYZSjKACkChhZDQhEbEjTJQYakhUUSbMZIjjBCRRDJYBKCMjcaUJiciQhIcAJASQKcibJhjEKLZTTiTiBDKcYikBbYQBJkbZiYYUYLMJaUUiCEaCRRiZUZUZLRaAiLSMkELSDQJYaRAJMJcRDLKYgIaYKKAZBaYgBCLDRRMBAZBDgIKQkRhLQEJCjZRabSaYRDCRQDLYJgSCTAUiDUQQaQBEZRJRbcJSDCIIDUQLLSTLACKCRhaYSCaJLaYYKARLJaCAQbgbDgBTCZJCAMTaUSELTLSUhETjLZIEESAYAJBQJYYBbiBQbbcDSCLiCcUYTYITbjLSTiCTYbDYMaAYIabbETCJjajLJTIKAjaLaJiBYQADBQIgRKQShLTKDaYBALYYDaZRBbBKBaBJJcjcKTAAbSCYJLRiSEQIZQBaBISLSIaAaabIQLIDDJbUbBADCACZZLCIRbQbZIIQRKLiLiSJAQRCTDDKLTLaKTbAJTKcZBbZSQAahSABJZTJAARDBYAZaYIKaDIbLDBQDBbYaBDSBYbCQgSLRADDKSaAhJYKZaCLYTRBTCYaYEDYaSTYRILDDJKJCbaTBZYDBMBIZALJDJjASaZEbSQCjSQBKIMbYDQYYJTaATaKLBQZISLDDjLYCcCTbTADSATSCRBBbCJSbQCIhBISYBDTZKZZJDBAaAAcZSaCKYLbIQbZICLMKSBbTIBILBJDZRAbSDDKKCKCZYJRBJCIbLQRKJCKBJTSQaRbSQbZCSYQLLLQLQADRMYJKJADRRSYKDDYYIRZDCRDCDQJIYBKCTIKcCDAJSAEIDShBRIQDBRBZIKLLRaLYIQTCJQRYYbaLbTQIIKSTSaQQLCaYYbASCRYBJDQaTLJJAYBJKbBSRIaSTQDIBBATLbKSIDRQQKJKCKZTQJZRYLCRIRRACKKIBZZQYIJZYAQcKSITQIRZCYbYITQLLKLKASKSISJZZBKAaIQDIIJhRJbJCSSaSJCQZCbRYBTCLYSASRBRYTQKQKJJDbCLBJaQLYTILJRCJDLRSYQTbSIBIbbTRZIISDTaDQATIYTIbYaSSYLSSSIQLBbCZaTKJJaJABBRSbSRYIaTbbAZbLSRYAaKCYTRKJLJQATSaBYBAbRCBYIIKaKYQgTSZCJJRZLIIBYQBSDLTaQYCBaLDSZBLLTQSIQJYYBLQCZYYZJbaCCDRTZBJKIDAASSTDYDSTZBaKBYJZJJDLDRJCLCASCKALAYIJRZYDYBRBLLQSDIYLASZLYaQKaAKJAJLQaKZQRbTRQCITIYLCYJKaKQYZTJDSQTDAbTYCSKTAIBbbRZDILATZLQIRDCKLZTKAbBDaSTARIYZILADJADSTDLADJYKLSKbYJSRbASCbQDCSAbTSTDCARTbBbRZKaYJDbRIYZIbCTYZCSJSaRSYZSbRZDKIQaTaQILZTCDYADJSCDbKLJYDKJZaBLaQYDDbJBSQQZATaJCZISBAAbAKAZICYJTaSTTBLaRBCJLJabDQBYaKKBZCKDabTSAASKAJZaSZCIRDZIBILBKJaIQKSaCIDSCQbLCRaDYbIYRLbbYbQRYISZTRaDCBABBJYLIJATKaLSIbbICRSQZQLCQZCQKIBZbAKIJIKaaBRLAICLCKDbSYIDbbCABITLIBKIBIKbZaIQJACAJLCDbQYIAbaBQLDQaIAAabJbabJTAbQbSALJSZaYKCDSYaQLKZYJRQJJIIABRQZIaRCYaLAQDBZRQaRZKYQSTbSSKbYRRYBLCaSBYSJTBbBDBYKADCRTRBbTZAaRBTRBRaJSbQYTALACLbLZJKJRIKBCZKCBITIbZKLIZSYaCQaCbaQYSKKTZKJaZTZZSQLKKQRKaZTLQaTBKBRZaAaDBZCBLaBJZISKYQLSaDDCBARIJJJTSKaZaBKbZAbZZSQKRbSCCaYIQAJZZRRKSLAbCKTYQaYIbCTLQCALDaJLDICaIJIKTTIKDIKDZQKCTRDJbRBZbDZaKKCSQLTDKaQJBLaRLaRRCITYLbIBCSRBaQZTKKILTaTITTCaTYJYDRSYYSbJARADAJYATaSZLCRaKbCDaJJBRbDAJaYYCTAKDQSZKSDDbCDDCZIIKDRBATKJbbAIITLRCKKKSCTZYTBQaJRLDTJAIRRJBAKbQAYaZaJQARJSKBQbZJQSJSZQQQDDLCZLaKJDLAAJZIabAaTBbCSaKCAJBKRRIIZSYJaQbZJQCJSJCLAYbQDTJTaZBRYRSYSTIaLbAIbaCCBbQLCBJQZCbJRKQJRbRQTBQLIQQJRYKaJAKaDRJBbLZISASRZDaaTBQRKbQYRACabBZTYSJKaaSIbbbbQRBKaYaKBTRSKaTBTTJLDTaCQRLIYCTbaDAAZAYbLLYIBIRKCKDYSDATBaQJQaaLbSZYRbQYBIZYJbKAKASSACIbSLRaYBQLJBLYIALLBbKLRBLBQATCIabQKQDYBbBaQDBYCTJQBbLCAKIaTIJKRbRbBYTJKDCASKbRZABLKaSCLQZALDYQRaAATIYJLZILAIKSCCBBaZCSDTRDCKJDTLbSZZLRaBaJYKLQCDCAZDDAbZTKRAJSQCKZQRSYQLYZLSZJDBLZICBZACIaRCQKJBDKJJRSQaDJRbI"},
Embed{50,160,"ATQIIAQBJIAbKDIbJIIbAKAALIYbSDCCaRbDaYDYaaCZaCYaJZLbBTISBbIQaBaYRJYDKQDDCTDaKBISTYBaRDKZDSIbTBKZASBLABaASaTCLaIBLSBYbBaIaIZbDaBITaBbIJSRJZJaaBCRDSCADRZRTaCYBASLTLBYCLRRLABaRLASYRBYCIRQLBRKZCADbabRZKaQYKKBAJTTCAbZIAaIKZTYLSYRTKCCaYRDTTJYaTbASJQJIRRDYZYDYaQTaYIDZTLCSITYRBDIDKBLZIQJaYBADbZAKQTDKKAZSSYJYSDRKTBLIaAYYaDDDYRKIKKbSKTSCRILTBTBLAKTCLDJJAYYTLYTBDSKaYbTIaSZZQCJCJBKYQYKBRIDYCbICDDRRTZKZBYRCLAIYQAZBYLZLSLSaYKBILZASQZIKKTZRTaTaQBCJSJKLIQbKKJQRBYaTYbIZQRRSDIZZQLAACaRKSLKLKALKCZAIJSLZYDZARCYSbDAYDRKTSLCSaLYKLRCBbARTSJbBQQaYTDKRLIRYRKaKQJLQbCKZDJIKaaKJYJBCYCBLYbbIABKBASKIZQCbTZRYYTRaIABcAQATYabCaTRQbKDaKaTIQbRRLZCBKBhaALbbILASTbYIAKRZJcaCRAABICJAiZDJBKBaKAbAAKhQbLCZRBaJRaCQYbjYASISLQYKKDKbLRBZIaETQITQKYTbJIJLKQLBCaQCCjiTLQDARSbLDDLASIiSZbJZUaZIJSYASKLSLiCZSacSLDTJaabbALIaKKgJQZAQAaTJBSSZZADbKQRUABCAjKSRJJYZJZZQRQcRQcbSISbSMDDCDAJRhbLTQKbSabKDQKYIUaChILKjYKCMUDJUaYIaMLaSRTCBScJMIEIKaRYIDLUYYIQDMbLZQQCCbUJUSYZUJUZQKUYJKDZiCabAEBELghIBERKBcSbCcEijQaZBCgJIbjiTQhjALIkJCJJDUaZaTTSLEUciLKDIISAKIRbjiYAKYabTYTjEQbbgAjSTaiSCZJQgIZjhjaJTYbQRSKShhgMQRBAQJTECIZhBLTEgQjMLSZEbjEcZaATAAbRhSRIbTcCTELgAajbjYhhLgJYRQgEbUCjEJQSEaLhILhDADSUQggYbAjBjBMcYEcJZBZAkTZhKASSSciITikiULcCZCaZIZLUMQMbQZMCacMkUCEKgSRghRZCYDiLKSibZKagAhLEigKDDkQSKBgDjAQaKJLILcEBZYUEIKBCYZCSIJUYhJZBBiMbgMaIZBBQDMBDLTjQbLZIUCEYITCiIMLABUBQbYaZaJQCgLhBbjSAiYLbADDQLaRbYIRZRTRMUATiIADKcISCSRRLZJaIBjQYTKSRhBSAALTJgbbAZAJCkDIISTBBQbYDYTZhJBEDCYBEZKbIRRSaSLBQKAZRSARTYaKcaTKQZKYKZaJJALTJiaCLETgARZQhEKZQABCIZLhJBJLCQKCbACKbCgDJRQaUTaaRDLiLRBLKADADbgIAcAKcRCTZYChjJIITJZTaRLJUKCTQgiaIaiCQbaaASIaQIBAbLSjKBYDJRBTDKCRLCAYbJEbLaSSRRCDhTJTKSJYjYSZYULDUKKDDKTDajaSYJBBbLTJAZQSCAIZCAJbJTYIBChSRSEIZCBhEYAaYJYbCbRZRKKCQYDJBLAADRZSLaDKLIQbbZCDEYSITQZYJBZSBDTBRKIIATbKTQYLLZCKUBIKDTTBiSUCbBLBaDJQIBKKaaLTCJRARaZTBKJLQRIRTRJSIKLEMKTgIbiYIISKQDTKYRJTJTRYLKLYCBCKaQAEIALabSSJaRAIaSCBIYZKKRATLaRbLATIKaZbbICSQARZTBaADUBYZTbILLBCJbRKSISKZCBRJKBTJIQKTQKQBRKThZDQQLjBZCZRCBBBaTYBCQZYSQjJaQEBAJYDBDTZbZKTRJQDQRTJRCBTIcZDISBTQKRbKTKRYARJQELKICQaDIDLBLRRICZCIRSKbDaIbbBBYSJJDQJQhYJaACLhiBJCDBSKZDSQCbRZDDBKRDRDZBKRKKJLRLCQSIaDCBKaSaJaaZQYAQSCaLTIQKJLTAILJIRbCJQIKKSRTTaAIYLaYJTAbSCAaJTJSLaJRZCSTDbAKDYBDaBIbLSJbATZJQRKAQQZRbSCCTBRQLaDRDCDKSKZDTQTLbCQSZKKZaJITDhKAbYBCBbBKDYaIaQRKbTbAAYLbZQaBaRQTCDLRYARIZSSRRDLaLTYQbIBRZSbKTCQbRDBDRIIJbQDIbQKIDJCSTKYJJQSTITSKYICZYZaDZLYBSJAZCASJDRJYCRYDAKBbbKBDLTKaZIAYACIJKQSLLKLYaIQLbTRQRTKBBLSDTRBZTKQTBZBIRbJaSZACBiaTSSRQTbbKSZZDaYIaRZQSTbAAJAYSLJLRSbSDZAKILCbbKTTRQCIBAbKLQIRRQISYIZDJSaIBLbbCLJaQBJbRRDaJYCRYiCCDZBYQZaKabTAKYSYJSYZaJTIAQaKaQKBJbZIIZBJJYQQSLhCSZRKSQSYZYSLJJDQTKRBAaAJSJJRDKAIYRZKBCZRDCADAYTRBZTbaZAKCQTDJDYJAIYQJYKKKJAiTTCIZQAYRQbTKCITIbaRRDBCQYbIYbYKCZKLaJSATDYLDTZSQaQARaRBQKKLbBJYJLZSQBTBLBRJCQSBQSYbQaDRRIZQDDKLKaRRJIaZQQCRRBaBIbSbQKTLYbTDbDbDCCCQbLZJRbRCaKYSCbJADCIBCDTbILCLbCYRKDJCSaTISaISITbKQAZQIaTbICYLBZJSRZSAIZRbSCKAaDRRbDIISQIRSbZJYKBbJJSBLCAJYJKaaCSIYCRCSIZAbZBJCJDAARYKYJRLaAJbITQRSRTJTQZZbSSSYYCBJRbLDCSJSBaYBKKZQbSBDTbRLDYQABAQaLLIAZQASIAbKALDABaLCJDLBAJTabZQKDKaQAYILDSBZKDDLRRYbRBBIbbBARQRbQTRCATZQBDIKLBTDAYbSTQZKJISRYIKRYaQDbJCCCZJDACLBYZZQCJLBIJCITSLARCCaKAKLSTbIYBDSQKJbQRCbLKabACKBKCLabJCAZSbSTYLYZBYIBCZQRDAaSBKTSTZLDZJKSLYCLDQZLYJDCYTbJQYKKYQIbASaZCDSDRITZTJaDASBCZTYLYZaCSYKaYKRaYZYLTLCTQaLSJCQYaSBJYIZaRKSIJZLSZTQYbTQIIbAQACQYAIIZLALaLRZSICDaIKIBZARDaSZRQJbALLKITSKZaCDTATbQbATRKKZSDaDTYbaLIBYAITJSKICaaTLDDBSJTCRBRKbIQbCIZJARDJaAQabRZAbaCYaRIDZCKYSQLCKCCCIRJKDIbZZQYBJRLJKQAAZRABZQLbYbZJTaQSLADCYLIDQSCJKABIJZZYJLADJbabKAQaaDRKaDDLCRYZbCRLZKYCCIBAZZaKQRRTBJCIAJJKbLCATAYCSLaRKTAbaJSYYTKCAYBLRBbQRRbBbTbQQQaKICbaRBTQbITATaJKZRARBSIDBIZTTQIBbSZDSZKTICAILSJSYRIaKTYSLbbDKYDCIADQYIABJZAYRTDBYLIJJRBaSbYSDaDZKbYZYKQKCDAaSAJDbRLYBKQSLCRYTDSBJYJSaILDbDaRKbbQACTAbIbJCAZYKaAYbDDYRKBDBZTTKSTBTBTRDQJCTJKbSTbLIYZCQCAJQCJIJZIaCRZaSDCYAAbSBJRLYIBLbLITCACRKQbJBDbKRSIQRAJQSJaTBaDLSZQZLKLAbbRCIKAAIbYaKRCaZaJCQTbTSYKBIAQTZTTIbSYBKKQYJKBaAZTJQaLBLTJZIDCaYYTSQYDBCZabKaRQQAaZICYZYZQTLDQKDYTbLILYCJBACLTJQaABTTTCLAAIKJBJQKLLabTbCTaabKTSTASCLRILLBSDIDaRQYQTRDAJIYJKYCZKabKKSCbbCBKCbIYaZYCLC"},
Embed{52,166,"IIIIAIZTAQALQLIIIIYLIIJRBIbYKaCKSaaZabSaLYaCYbaBaaaaIaBTITBaITaJaIRDZDCSaTBLQYaTBAZbIJaBAQCCRIbQBKaIZBKTIBbIaCIDQKZBDRIYaJaQBTaIKRbBIbTACTaLLSBIQTZRDJICCSJLDYTJYBDJBSDDYSKZBSDIaaRIaKRSTaKYbBYIRLCDaDARJZADZaSIIDZaTYaJLABQJAKaCDQYSRLIRbbCaBBaRZDLSQYZSLLTaDBQTSAJKZSaBBZBaYAZIQQJTBbBCLALRQYITYRKQLJLCaCTKCTRJIQIALYZKSKQBADZZCJADbDDRCYIAYLTLaYKaaIBIQbALKbCLYLRSTQbBCBaaaQBIKaSZJDBAZCYJAJQTDLKJAITBZCBDLCIaDZbSCbYSDTDKALIaAJTCZYQIDRLDZaRKKDBSJAQKYaKLCaATZCQLIKRAKabBbITAJKBITICTbJYTKSbYaLBBABJITCQZRIIYDATKZRZbTKACTTJKLABDASDaBIJKCLIJSYaSBSBLBYZKSJDADIKDBDYIAaSLZBIaRQDYCZKRaLJbADKARJCZYRLBSQTDbRAYSLBZCaKTRRRYbDQaBaCbQZJQZCCKRbTITLBaTSSYRBIZJTbCTLBAKIbESCabSIbTDLbITKIJIIQbCQSKQLCaBAZgLTBbAaJJBABBaaIRaSiaQQZLaZKBIDRYYDSDSILKaCCDUJIZZZCQCJCQBJJcZSAQSRKkYIbbLCIYJDCjBAJCYTaRSBLbSYLMYRbARDLYaTIaDSJaMQUCRQLbBLTRBcBDJSSQYIJIAIKaJTICiQbbaZCZDKbKLARRYJZaQBCAQchaIbKTCBLYQUJbaaTZBCTRJAjKKAkQCZKCJCLAYCKDYRIbQKIZUIZYESbISQUZhLQZLYYKRCLDgTaTBiZTYYBCLBLBTDSjCITAakbYRaUjQYYZZJBbJDRUYEAEaSjZQRSBjDUDiMYKIIJZQRDcDCDJQKjgAUaDDTEbjaLLSUiTMQaEDRaZBZDKUhJTBEAiakIbDEYLIBCCISkSjSZhJaiYIQbbEQBahZYSaEJYjYCkYackSBRSRYZSTLbbRhDkiYQKDkAIYcLigicSaScCcBDZhAbJbQZRCkMRBJjEZICJDQSEbjRKRaUjYYQIUZhKZaDEhbSbJEbZMUSMbUASDQkISScCJJZaYIhhaihEaThDYAcUQCbUYQZkQCJBCMkJcRIIaLMBbEgbIARACJhQJhUScZiCQMMhAIcTbUjhLSSZahQKiADJbabYcjDMaJiggUCiUUajBgIMMTJTZRIKggMgARYEjcbCMiAJaaKBgagjTIMbQTgQQiJADCQgCSDLQTZKCjbbJkkLZZgMIKZUTRZLQAYKAZBRMQRDYLYbYiILJBMDATRZhZCYEJCaYbbLTRUSSDTDhbcgDAaZbaYCMKRLIQLURgaAaLbDaAbIALLIRRRbAkSRDKjCUAZTKaKcDTDZCJZDRTEATaRgCYbRBJTbSZDJIcaYJKCgjQSgaBBKQYAaSJDIbBTYKDIKRASLLbDYcKSZATZRAZCBDTTZIRaYMaMSZajiDACcCBQYiZZaDZRBDKSBLZIRJbYaBQDYBhRbJLQQjLCUMDIJBQZDaYQZbLIDACaTSITbBcZRYaCYiTICBJBARIjaCSCLKhaQbJJZAbBIZSbRBJiUYLSKTRDIIBQZTaZCABKSYJZRbbBKhBIKCAaBKKUbCRkbiLaZZICCQLMSjQADJTDAIbYIBKTEbCBKQJBQZDbDTJCZDAJTIZAcCYbBabJaSSQKQQaUTabbJTaBLISbZYDJSADSQRZBYbLUKRKMTLTCiIZYQLIbQbSjbaYCiBKTDBAaZSZIKSbQQIJTbLaSMQZiQDiBRZDaKQYCILZURaKbDZbLZSQCBKCbDKDRLRAaSYTBQRKYITRJQaALhbYZDCbaJRJQSRRCMaLTTgZQYCDJaISQCYLJaYJQCDRJYSAILDYbLABZSSDbYCQCSBKYJbCaMSLbRRBLCYbISALaCbCRCCTSJbKbTJCIDDbbLaCBSKLARZKZbLJQQDBQaLJBQCJDYSIZBKJICKLYJYSBbZIDQMSAYYJKTJRDYScRLDCQaADbARZBCJDSQCBaCDQIbbaaaZTALJbBSRLZRAQZTRIJSLIbJTKhYRBaaMDARYAKCTQLSRJTBBZBIBJIZITIAQaZSQSSBZbSCLCZYACCBYYRRIbIQbZKBKKAKbRILIRbSBJaRRIbJCAQBaZZLQCCDTTDTARBQAYJZcIDSCJACRYaBZaBKBIJYZQYCaKBLaBRBIRJTYaachDAZJALSSbCCLIKIYaIRQaLTCDQZQIaJJIBJaAZbQDaYaQJYZaRaBbbRCKICZKRSJYLCCQKDJIbBSCCRYZRAECLAZYABJDDJQZDSRDBLKZTJQaABaSKaKJIKIISbIJYLJKaALABDRQDKICJTQKYTSTILJRQCIIRTIIISICCLBITbKJQJJYTBDCJBACJLYLTQRJTRSKSCADQbTDbDDRaLBDLZKYCAbaDJARQBSRbSYYCJDRYRKJDaIbYKaTDAKSCDIbbQQTSBbQIDJaIKCTbCKJIDQDBYRRCABRCRZKLRCZDZaKJaSYbTaTKRBLSaCDSLJZBKJAASRaAYDZCbKISJRBbETCQRZTLBKRCKAZJIabLRARUCAaaDBBCTAbRJZRBQTIBALQSRJYIDDITCTDZbZALSCTaaKYRLSTBbLAaLbADCDCAZAYaIYIbSJSIYCSbDAJAICKKJbJZKBKaZLRBRZTSRRBaSCRQDTLYDLKJJSJTLaDAaZSTaaQLDRIaSIYCCQIQLKCIABbKKJIBaZCRbQKISILYJDDJYCBILKIRQaTISKTbQZBbQSJCSbYIYDLTbTRBISJCSQTLJDSCJaYAAAKYICJLbJCTKJQIaAQLBSCJRTQaJSaYLIDLTaRRQASbZZJLbBZZKLRJDDZIYYICCCQaSAQTLKITJITRJQSbSLJZRSSbKQYZSTBQTTIQQQTJDJTDCITTLZbSLYZTQbDQIIAaZKBKBLaBaDRSJRADKBKATSaSTDbKYDABZAKJbTQaQaRJJRBYSABaSTTZTSYJJBBZCJQJRLICZQCLCLIRIITJIABKZJBZYRIRaSaAABRYJKTAYaCZAJDDZJZIaBTIbYAAZDLRZRCaDQQaDJRQabDYAabRTDSaEZTTQLbYJaBbaCDTSYCSCIBIYDLaRKIQaJKQZBICJJQaQIaJSCDRTJIJTZCZKYaQIDSTbTCCJDSRIJCJASSAIJbbJTTDZLDAZYIBAYTZIDZKTQDaIQTALbRTIIDKbKTQAIKSSJKaLaaLLCRKJQBTILLLYKDYQKaDDQRJRKZTBCDQQbQYTIJbTCQLDaAYZCaKBJSbRaaBTbALCbTYKYbRABYbSbLaAQaBaQJKaaZZCCRSCJATALbRJKRYKKIIRKDQKLIACTJRIRBbBBTTKRCSTLRaSbILJKZDKRQKKBbQaLDRSCAbIZYaSKQBTTaASZZJAQCZBaBKLAIYQaJCbKDbSDZSLTSbQZTBKTKTSYTCbBCZKLJKaYSbQAYLSALCJKaDJZDTAJRQRBIBSQQBaBaRSJIZDDJIBJAaIbCRBKSbbSKZYDDCBQTALbBRbSQBIYCabZbBbQaAIZYBQTRDCZDJSTQQJLDTIJQJJCKQCaIASYKRTLAKTbKKBLRSTKLLYIQCbbYDJIQaIYACKRBCYLabBDKLDKCaCDKTLCSSBSZLRKKaaBBIQRZBDYJAJCTKRQbYbSYDICKTZSZDBTbQDabKJZYSALQTSDRJSJYYRRbTYZLbCIIaACQbQIKQKSASbTZIJbJIRIaCKBKbCRbIbBTRaCbbCbAKSYATTQBLTRAQbQbKASJKIDCabLbCICCbSQTaDZBTLRCZbZbDRATKYKKbDaZYaIRAQDJbZAASbTAaBSYaLJQaQTDYJSDIARRJKIYKKKZCJYILBDRDCAQDJLSBYTTTTKAZJIITRRTSKSZYTASKAbaSZbJIRDYJIYLCQYKSLJAJIJTJRQBBACYSaJbACDTQIDARaDYJZCJDYZLALAaQJSTDSJBCJbbZKQDSJIKCSRBbLQLaBBDLJZJJIaRSTDKRDZDQSKCCRIZDaICbAYKZaCBJYaCIYZCTabYALYRJQZZBSQbBZKCYQTLITRAIRQYBZJCZSbRbAYYSSLJBZQAABBCZaCbZDBYAQRBSZCLIbDYSKaYDDYAaCZZZDYDKKZSKZBIbISAYAbQCQZYBCKbICTBaTSZDBaZSQRbYJIKBYLIIBKDSTaJRQRIICIBJQTK"},
Embed{54,172,"QQRLYILLQIJKKRILKILIAIIIIQBSBaaaKaKYKYaZabaaSLaCKRaaRYIaBTIaDRIbaBYCLBQZKRKIBTKIAbTAIaTBBKYbTYCATRIZDBKTBTRaaBIRbIRaKSJBRSYDSZAbYRKDBKDIJKaaaTAbRDIKAaRKCZTTSZBIYDISaBLBSbCJbQbRKIAKYLDZCTYSIaATBIQKTDTBRLIYBZKbZRCZQSDaADYDABIQYBZKAKSZDbRJYaLCTKbKKLRBTZAYDDbCRJaJQKBaRSQICZKTDaDZJSTQBbbSbSABIQSDJJTSJYDRQTSTaYDLIbKLTLAQIZDBYQDYKRRRKDIIbaCADTAQKbSTBLDYSJJYYKbCRYaQTaYIRDDJSADJbIBaJSaTaDRYZQTCTAJDQRDKLLaIQDZDIIbTYZZCDCCBCITLbATYaRRQbSZDJZYKbTADAaZQKLQBCBISaYaZTTRaIYaIJJKCYaRZLKTJTSSbQABJaQYQKIIbCRYKRZSSKQbDaCZZKADbKLYaSJDbYDAKRZCYLDAYJQabSCZZaTBAIAZDIJKICBaZaTALSCBZLZRIKDKCYKbKBbAJaIabRDTZASQBJRZYTZYSLQKZTDCQZaTTIRDbLCYTQLaSYDaTDAIYDRJYBCLDSBJIAJCaSALYZRJQZIIAYIKRABCbJbSTaTZKBDLATBLDLQIaSQSBBDBDTASZYbQbJUZLITCZQZaZAADQTDSDZDJIUADQQBIbZaZBRLDJSaBCjCYJCaYaKCLaRKASKDBRCJCbSaAIBITJZbKbTRBBMIRbLYihBCKTBIabSIRABDIBJSYJYAKQYLTTLKTLTBQgYRTDYbQLRKIDgAaBLIIjJSBRKUBASZRRLgYRaRQZIAQYEDgKYaLIRDJiJYAgESJZaZDSYcLDISCLYbCMADaCaLIgZLYYUYbSITYJRSZbQLCCMZAigcCDIbcICTJJRAbjIKCUQLRgBKJYLjcbAQICRaCJkaaARJcLIDSKIjbZSELLbcYLDBbKKSEgTCaQUjDIUYKaSLSMJEZSQMbhBbaTbgYZBCDQgaIZAjSUhJYDKaaDgKTDRjIYEQgjcLBbLAaIKEaiKZhRCYZQgDYUCaKSZbjTZDAMIiQTIMCZQagTkLSTJCYgDAjLAaRZKbUiQZhKIEBcRaEADJUQShiAIUIQYCLJMjUhbkMJTAKiILhRSBEYcKjDScYIYCDaMacJaQSAKCJZCDihZCcYEYKMbUaUkKcRRMagQUDJbLZCaSCKcURAJREZiZZEkiTAJbjDUjDiZhZTgCQTJhcgggbSYTaIUZhIAbAUiKUaKYLZkTcjEDBgJDgKYJZCCTKEJUADZigJKhUkRkJMSCbLbAaJJCgkBAZZjITiiIkgMECCiYhRAJKDiJZJkBKAUcDLKaZjSZCCUAYLbYjLASRZkckIBAETjMSYSLYJBkLLJAAUgBjTEjIKSikbiCCIBiUZCYAbJbUDAQCkbLLSLahhaYKYBSMMZhbbkChJRAkhAARRKQbZCcABhbSJIQIEJQCZhDBQIhRRKTMSKIISQCAbhZKDTQURDbcSLjjaCRaZJCaYZBTDaAiYkIULAiJjJCYCMARQLLRQSBAJASZZYLhITRCDEbgJYJYMBCaBgIhabYUZBaCDQRbIUASiJQQYKSSKTgjjITJAgKZTQKKLMLZKBLJjaCCCIKRCJUkBTkBYJbIYbSQahAAJEKJSDUZLbBDZSRQcbAScLLYJbIaKJLSQiTLTQgEDZCZiJRDEIAMQLIBIaCIMSbgSIQJbRDBCAJYLZKRRCDLBSIBKbTIZJQAagEaJUcbCbaLUahLTKTYaMSQQUSISbDCiQbDSMQjCRaZaRQCZYUhBbTYLZhYgCADZQYYYRQhYIKQSJLSJJbJDZBSbgZLKZRYBYbQLALBRQQECaICLbKIEZCCALCZMBDIDUYYACKCLTLDZSZTKQBbiIbTRJLALYaRLBBSBgCDaBAaCDIZKSYLSbTTYTIJUJIIZSDQJaTbBKTSRBDBShTKBRYhbTJBAAKAabRLAUIARSBCbJbLBELQYaaSBKaSDDJBBQQBaQbaAIYRJAABDbRDKLBYbRTZbiCBQTKZBSRYaQLCSCJJYAAKCKSbbRQJRICLSDaCJYZbKLZAIYJQLJJBJYTJIAYDJBRATRQCQSISKJSLaQLRQSaTKBJaZYbEBSbDIabKDARRKQSUZRBKQCKZLYDYBKZaZIQDRLgRASLIUKKabDCcSTLIZASTCbRSaDRCLDIJYKRAQJbjZSLKAaBKJSARBTZIZKAZAZKSYDKYJRBTKASQKBYaRBkJJbZZSbaAbSKQBQRIQQbTTJCZcLSKQDLKJTaSAJQBbQAYZDZCKabIJbRQRCCDJJBiCCIDRKABSZYSCZbAKIRYLZZKRIZTaJACICCKIZLZRDTQDQBABARJKZaaIJCADbKJbLSYDIAAbZRaaDaYLCQTCTYJADabTQQZaLZJaSTBZLaAaKSTSAQYYIKADJDQBJBSIIRRTABZLTRZLDKaLCRYKZaJTCBITSDSBRTQJQBQARKBBKbIYCIZjKDRRSZDSZLbDICBJbaSAaAbDSSTbBTTLJYBTIRKIKTDaCSDSSILbZBaTDbRgLJBAAbDYbKRQaYYJYTCYZCTBRKaRDQALBZKCIDJCDBCaSITJBTaQTBLKJRQJJKZQIBYSITJDTDBKbKLQRSbYSQTLYaAQbQZTZYAZRDLBLKQKLDQZATCYaRJJKYSZKJLaAIYDQBJKISLRaBCSbAYAYZLYJIDRZIBaaCSDKDBLDSJLZLSQBSDIIZAKQJLKKCRKCCKaSRTZCJJabaCYBBSBQaYSJaaTTICARBLTQTJCRCQJLKJBAbATZSBLTRYIZQDTDITRbaRLYCCLSYRJDKTIhACZQDLSKKcTbLbABJQYKAYLTKaDTbgYZBIbKRDbKRDSJYQSIICQbBZTaBZYTCiTSZTCZBBZCAZTRJDRBSCTAJJSRIQAbTYDBCAJRCjaKZaAZQKQZYDDLZDDIDaIbASSYYIYAJBYSYKARLRJaaJbKYZCQaLDSDYATZYKSLRZTIBDJbBbZTBQCBSAQTabJCIDCBaTQDLaRUJLRaTbQAQbCJKaaQZIaALYKBJKCISLYTZBITYLTYSaTTbCAaabJDRbaKaRTJTBTKQYCISIBASAIQLZDZbSAbTBSLaaTDBIIJISbBZIbIASRKKADbbCSDBJYJTSITISRRZDCZRIIbTDBBJABCTARKRCJTaLbYKAaZYLRSCZAYCaDZBDYZSCIQaZIAYTaADbKZDbIJbTQSLLYRYIJIKDLCaSTTQAQTRILADDJLIIKTRaCBRbALLLTJQIDaQKQLASBBQJQYaIabSTTJLIaLRJDIJSZCRRASYCJTSITCAJYKRLIRTBKTRaZZYYSTCLaZLbQBbQTJARYDQCABJDIKYATBbbTKKJRDKDQRIbQBLQIQQbDJABSLLDRaQYLDaRQDLQBQbADYBbBBARCJSZKAZZRZQZCCCSLRIJDbYQDTQbJbaJbLCATBRSZKSKaRCSRZCBRaLSCAAKSYTLYZLZYQSLSYLLbRDSAYZZAYDCJCYQZTKYaaLCSIDSbLLQQIRDCIBBIALSaaBYZYQaLZbKSJLJLILAQaYCTCBZYCBaRRISCJKTaILQDbIbJLaLQCYSSKAYDLJTIaQCYDDCDQDDBJQLDRRASIZbICIBZCIQDYTTTDaaLBSZYDILLISZSKYIZQKQIaIbYSCbBbZDRYZBbDLaBASQDITCSKLaRbSSTCbIALIBKBLRQRLLCZYAbBZJRaSRaBAbDDbaQRBYCDICYDRLJKDKJTYICLACabYZKQSRRIDaSIDSITbQZJCTCLLaJQTKaSKBATDTBBbCAJYbKCKTYKCRKDBKRCYZLCBZLBLKJIBYKJAQAIKAAAZKCSTQRbYZQQSbACJKKTaaJabaLJSLDCCIZCaTbSKTaKKJCSLYAYJCAbJQbLBDDRJKSCBITaRJbaKaZRQYYISaJbKLSSYBZJCBbLASaCKaZRSLQYYZJSARYARIaDJJSJSBDCaBaZKJSIbbJRYQSZCARLJLQLKRDIZYCCDCaZbaRDDQSBLBAIQRRbICRSKJIBLZAJSTLIIYQKSKZRDBbSSAKSLAbbJaZCJJCaYbQAaZYDISJbRRSDDRKYTCSAIBYRDYaYbAbJSYDDZaQaLYZKRYKKaDbBSJYIRRBKAYKDQZKKDYQIbaTaaIBKZLTTRQIJALYRSaRIIIaTTKBSLbBYTISCaZRIQbQIZTYRJICAYKRSDDRLBKTTRBYQDRCLZJbKbZCZKKLAbSSbCACYYRSTbKSJSDZQITACRbZSKDKBTYDaSLJBRYJKTYJZASLTZbIYRJJBIQQCDTKQQSZLKKIARILZRRAYCISLTaQJDYKaTAJaDTbABYZSCYaRSRRLSZISbQJSRLSaARJaLDADZRZZCQRCaLCJLRDADLILSQSYLIYIQYbbDQTQLQKJRATbICKLYKQDILKARJDBJZDZCIAYDAKYJADaRBQaZZaTBSBIJIbTRRQYCRbaYbBKYQTCDQLRBaSQLIZLaASLRbKQZBBQKYYSLLIKJRaBIRTTCATDTaCbAIDaYDQIYDTSAbBSLLRJRCJLaAaDLQZCIYSCQACCJJbZABLbJD"},
Embed{56,179,"IKYYIQZKIAYIZAIIJIAAKbIKIILIQaaaZaKaaKSKbKCaYYaabbAaaaaBIaBbTILQITDBYaARICRKYIRDIYRbbQBIKDIQaZAIbARKAJBTZIDRTTKBSBITaYaILBLKIDTZSBKQDYbCKTAZIAaaDTRTBRISTITDTTbIYBTSJLQDLDTBYQaIYaaZDSQBLSAZRQJIbDaDaKBBTLYCBDSbDYLKRaISLCbQYZAaKYIKBIIRTKZYATKbbZSJAKDIIJSQZYaBADKKSYBTJQZRLCTLDTYZSIAaAYTBKaRARCDLDDATDKZZADYSLRLYaZaTZARIJYCaQLAKRRYBIRBRQaBRIRRBBbTaKRbDBACLKTAbRYDCQZYRaZKZSSATZAZIBaCAbBJDJYATKAYLKTTKDBSKIARJCZQTAZabIZaTDIQZYDaITIAZKBCDLRIBCQaRBbKJIaQLSITASAbaBKJBRLTSKBZDSLDJbYQAIILTTDKbQYYSCaZCDAZRTaBKYBLRKKKRIKTJCJALCTbBaaYDaABaBQSJYTILRaZAYLILRRaIJSaIZSCLaIAAZIRKYCYYbBYAaCYDKaZKKRZCJaASKCRaZKZYZYDAIRJSTYDTTKQALIIJaILQJTCBJSYAKJCaISQIabZADDYZKRDIYYSQJJBUTYKCZALLaSQKAbLaYDDCIZKJYYaASbDbKCKADADTRcJIYSaCSDKARYLZYBaBYTAbKCLLJCTZIKTQZLQQDDRSZRTRLBIaBBCZJCIQKDRAQTDaYbIKITYSZRRJLDDaCSBbYJADCZQJLSAaSSJQTgZTaTLbLBQjKKBbCRJLSKJSTBYDDLKCBTJADYYYIBabYBIBRTaLCBSRQARCLRbCbQbCJbIaKTSAJLTZBQZDSAUCDZLIQaUDBKcBLJYabMbQAZCJZDQISRSRDSQBCRZCJLITAJLacbDQBSBRChcIaKcCiLSZBDALKSQhYLJJaKbYRSTiQSjYATJRYQLiCREYCKSbTjZKBaTbTKDISDQTLRKYRDBYJICZCYRbZQRkcgKSYcCiYRRKTBgDIZBbYLgRbJSIYTjCILQJYEBBAZBCILSCZLDYTcJRahSakZBUKRQiZQaMQRbJDQbYCKJZibiSCZbYSSZbSjCZKARRJbKISUDggEMkCbBBjABDYYRTQAIcSaUkchDJMkIBTKcCTCSSbTgCZZKTMLBAESDbkLajSKJJALcSYiCcKEMBSBbjkJSTZDZQIJhQDAIDZMjLYSUcBQEDhTSQBLjiKBZUSaQILYQTjabYgJRMALRCZkQIaLKSQbZMZRUBkDQYjahiUakRBZbUAbCZJgKYaAjjTLZAhhUQMEcYRBRSKQZbQAULjRDaCbMKIEQaDTibkjhESbZQiZTMjaZLMKMEcKSQZAYEgJEDaSIaLIkYEQZRaiicEbLEEEjLSDiURgJMcbCbLkTRSCQgRRCkJAhYMIMICiTJAYQZRcgZZZSLThTkIaiMSYkRAAALYhCCMTZZIIkIJIKBQkEcBYkDURZMTDAKDKEATUBbcghRikZDZhkJQCTTLTahLkAkASgQYLZSCkIMQMhhQhLZiDJRJCaEbRTACKQZBRTbChEkiQaDTiTIbLKaYCUBjaKKjMbMBhjIKCLCCJBDakECDLSJJYhDJJLKgQESgSSBYSALgjRTbhJcDEJbKBhAAJhTShIIAKaQCTgBAIbjBMUcJLYBIKbDAQEIhCbZLbSLCYgZLQTDghZBSRaYKYESMaCbaTJaRQBBQLBZURQDKTTZALIYahDYabEALaYLQECZQScKLThKCaLaJSQJQBIZTEISJACLZIURTkBCRYYAITTCKbUZQJSYYZJTRQMEKbISQbJSYIchjIASbQaQITYTbiJREQLAJBbZbBTaSUSQYCLabhRBSAYRiYDQaAbBDBIYbJZRjkJAUJKaKYQUIEaaQbLZSISKAbJSCSBKaiDUIRIZTLaIcakYDaBiDADIIMDACTJLBCZKTIZCJZjJAZbYEBTKDJIRAgCQSDDLCJLIaZEUbCbSTcaBhaJICABIRDKYRQIbRLQaDZIYDTCMTBAaJRbAEMLYYQTbCLAbIMIATSSQCZJQLJAZBZTSRLCJQAKAIRLLbCDaDCLZKTDJLiAIEaIaiTcAbbhRACTbbSahZRaCQLRCTbaKacbDCSaDQQSJJbQbYTTRJICaJJSbIJZLLZYaSYTASDTDLiJaaIKISRYDZAAaSbJJDIRaIIYLSSZYJJSAUDDTCabICBSCIZLDRSYZjKLSBBRTBSJRaBaDaBaDBbIQTiQRKZbbSaabTJRCIQBaKLEQTIYbBTCaSLbaLQCZCZDLgZhaQAZDLDRIRLZZDIYJQYICDCCUASbCBCYRaBBISRZTiZDYJjZKCbTTAJDcJbRbBbYKaAKDYSLITLgTYISSSICJSJJBZDKBAQRcYDCJaIZiCIJLYQJLZQRATKSIRTLJaaSQSKJTIabBKCRZQKQTKYSRKIaZZTCSULbLJRbCSLCIjJYSJYZKTRTJIIRRSDBaIDDgbbAcLACSBLDCCBILRRKTZbACaZZYaIAIhDbcIMDYDaBRBTQQZaIZZLBQQYIhDbBQIZKYZJLTTDTCaIYADaQaQZQCDRJLRJAYKQYCMBSCSCSJZJaJaTbZJRbKbCSRKQCTTZTDAQAYbMTBIDAAQTCJZaRLZZTSJbDKAKBAaIZQKTTZbQCTBMLZIaTLIbCAZRYRSCCITJRAJBAACSKLQIKBQZIRLJKYSIbIIaZSYLTLADZTAJKKSIDQCKYKZABZIYMBZZYRQZAbQaBISJIJQDLDaYaDSCCKZBLSCCLiZKIYJCYLCKbDaTTLASZJIBAMBLSLALACZQSJDZQCLbILQabaYCBIZSLTaaBRBCaRRYJLYRCaCBTKATbQaKTACRRaYQIYYBbYLaBZKTQCZYLSRRRRQZbBbSBSLQAbDAJTYbRJSELQYRaAbDaJJDJRABaSSTIBQJKBSBDCTbBSbADKLYaAJLSQJbRRACRTRJZIRJDBRBaAaDDBJbLABDBACRaKJCCbSAZBYTbKIDbKLCIQbJKAQYLTIbICZQADLTQJDDBKJBYTYQCLJCITbQRQIAYJTbBTISbCSZQSZJZISZAbbTDTCIARbBAbBaCIIaTJBDIDaAKDDZDAJAJYAAbYZJDQDZSZYDKYCLbBYYDKKQJKTaICaTTCIJTQJSYTIAJILTYJRDJCbbKSAKISKTBZTLQQJIbZbBJQYLRbJRDAYQaDYKCYQRASJJCSKCabQRSaJABIZBRKaZBJDCDaSSAZZAIZbTQABTaQaZJTTJKIZCZaCQBCAbaIADIKJBBLJYQTbRKTYZJJaJYSRTQQbJRKKDCISYjZRYbTTSKIaITbUCAYBZbIbITbKRYCDQIaDCbTYRKaKJaSIJDJZIQCQRRQSJZhABKLKDDSIYRCQLYYaLDCTZADabJYJSIAYaSJKDaKARbJQKLZbIBDTYBSZaQACDJJaTCDDQDYJaZALYaKQbCZCQbZIJISDTBaDQKbBITTaSCbIDYDSDJSSTRITRJSQAYJABJIBDJYDLYZLRQSYaTIbbTBaQCKBCbKbZSZRSKADJCLSbJAZIKCKLSSaYbDaILbSCSCZDDLIbQDCKYYCQJbaRYCQIILBJbRDZJBCISJLZRLIKKQJZQZAAZLRDAYSaJCCKSLICLBCbLBLRbQQTTYQYRbKZYJIDIKTaTCACTLICSTLALRLBTIBbAZQRCbTZBBaSbbZbJYQSTRBCRRLQDQZLBQTSJLIDTDDIQALaABRZabYAATZJQRIJSRIYLQBAITCbISLCDBbLRDCBKYKBLYAIQDCLTTJLbCZQRDIQaZYDLTRBJBQQbSJSSZBJALYTTCICZIKIQAZBCYTbaRZTJbBCABQJDTRIBbbRLZIabABaIBRabaSbQJRJCIADSQKQQRKJbBbDQQaKZbKQLBaDQLIJYAbLRCYSbBJKLYYRCbBAZSSISDKYRYIKbRZQKaKbTYQJIITDCKDTRQQTTJYCCCBIbJaLYTTSRCbLIIRLADTQbaQaTRBCKTLIYDYYDSJKRALaLZQJDIKZTbZKDKTBKQBSCIQSTabaBBaDZaYZDAAZSQLBTTCTLIYDKLKITbJCYSQQICYTLbSaJTQAYIIaJBbbaBSSTLKJYTTYQICaASbJaZILKDIYKDAIDaSRSCSQRaLJLBRaYJDJaKaSaKaLYBKbKYLaIBbCDBRLAaBYAAATIDRJLAATZQLKTLDTRTJCDJARIQRJLSSaTDATbYASKZILCKTJLJCCYRbaQBBTIaYIZSSLCBBISDBKYLLBLADADaLSJaZQABLaBTTaQZDRKSZILJRQJSKSLRaSBBKLJCKABCTYRTBQKbLKJJCKKKSIbKQCLSTYBbDAJZSZZYCBBRAIZaTYaRJSBSQSRSCRTZRJTJLKaLDTbDLSJYAZILRQLaIQALBKRbSYQKBSaDRBYISJZTLDKQSbYATBCbTSSTQabZLbQbAQbRTTLCBaDaaCZRDQLQTIAJKKaLALITZZJRaCTSRRYBIRKSZCRSQBATYaQBLKJQLTJDJCSYBLaabICRIYaQCCaICKQQLbIYDSbKLRLZKYaTaCDDBRBCZTKJLDYCSbJQYAKJRaCZCLbBRACbABbTADDaRQLDCJRYCIKLYTQCLZIRAJZDSYYBDYTaYKKIIABKQSBDLIaAQBZYKIIKZRZDKQATbJRZALCKDSQCZaaCIIbTBLDBJLTJACYJTBIRTDISKBQCbILKAKIYTLBIJSRYKCaZLAZQbZKYQAQLADITCZRZLQBTJIbSAAbCbBaTKZKSaaKabaTAQRJDLBBTBRJCBYBBSZJJISTSSZCSCKJRKTZLYRCAIBaBSSaDZTDIBYZDBYTQYKCRYDbDQZBRYLCTaJCLBRAJLKYIaCITYCJZbSCJKTJYTRIARSYYCDLKQJaLKIJIJBDbBDCDZKLQbTCASSCaKbALTbabQCITQKRRabZSCKaSRDaLAZLSLCbTaDLKDbZDaBSAQAZSaAJD"},
Embed{58,185,"IbbILRKLLAJAQAIIIIIIASIIJIIATKaaaaaaSKaSJbZaaQaASabaYYYKaTIaATZLARIQBaBQDRYCZLYLBTKQYKIaTBDKDSRYIaDaBLQKYTDKYBCZLIRKKJQRDRLRYaTYIaLAKBbYSTLBCbSIAYBKDDRQIDaBKIBSKbaZRIBaCTRLQJaTTZIBYQRJDBDbQLCSZQBLYTBKaDSYBTCRKTKRYIbaDZBBJCLQYbQaKTbQIBIYQYRLSKaSBLYSYZLASJLaJABLCbAABQABLLCZYSSBaTLDSDLYBabZRTCAbBRTIQAIYLBYBaLQYDSJTJIBKYRYTaRYaKAYbLQCJYLSTCLLRBCIQZYQKJTZBLCKIJYKCCTLQLQRLaIbbCAQAYIZaDDBbKYSRKIRRKABSIKLSBAaYLQYZRSJDRTBQLQLRTYKQDZRKCZabJSCYRDYYLKYCCJRIKTKLbBAKaTQJTbAJKRDJRLYKQTQBSJIbaSDBZaSSDBbTKDBIDKBAYSZBTBZTbCABYTTJAKBDBaaIQJbTDaSABIZDbSIYaZZDSLCRRTIaBJQTCYCDZaKLTICBYQJYRQQZTbDLSLTAYIYbAAQbDBTIRBIKIKYTaKRBYQTKYZQBTQTILKKZZJQSKbTAITYDBKQJYKSbJCBbSaBBJBTYaQKSCKaYaZYaQIKSLDAYaTRaBTRZIQLAYbTaaLICLabbITBALRQAARaABTaJYYTKaJDBaJYSJYbZDQZSbJSaTQSAAMJADJQLSbCZTIQJKQITCLAbIaJAKbCbDbRCKabaBAYZBSBSZAaYQJIYSbRBLDQKKKBITaIBBZSQaQTbIBgTLZYbZSTIRARUaCLCDZJDTBIRYSQQbSbTbaBbIYSbJYSJLZAbCJKSAELSTDSYBTDLYKbLQYJZCTSbJCRTTLCKBQaSbaQcIZTIbAbaZbTACIJKTSDYaDZDBKBYgAjhTJbQbbaRZCBKKBLTQACCaRTRRbTMjBIBIARBRDaabKIICTSTZaDKKCBakgKLJZALIgDBJZJUAMADjSJQKSaabDQJZaQYaCZgaaYRRQIRDJDTILcRiQRLhKYKgYIbTZQaSYbbgQRLCJRECTBIRLCaZILSjSbYbSCUEKQaaZQaBhiZLCaaRESRRALAZZSCkJAYcIQRBLiEQYcBZhhQBQMaSLAbSIZiKDCAZUjhKZTJkIAZSCDIRcQcjZRTIBCYIkLBAbcTYjaIgKICJSijRbQLgYDaZKEZQjAkLbBBKRSILSUKakKjABCRIMDaBITRjDRRYLDCZJLJDhChQLYKUaCBjjkcRYJAQKJiJIDUEcbSEKTZEEAiTLJCZSJKcALEIaTJiSLTaYDDaBaECSYBkJAEJMZkMckQSBMYRAZTRRZUTaJbYjITCkbcggCIQZRZZADkihBULagDbMMiYYDDDUiJKQIjRgKTBRZJJEQQKZCcTagiCUZkkAbDTIATDKZkaLTCUjKiiEijKMESKRUMIScZjIRKMRjRBUYhYQTQQBiDMkjQYTDkKDIbMZgkYLQRSiIZJkMERcUaYLLQCACDjiEjjCkbCRYLUJZZUbBkcKSiTaDEIbLUhAkQhjhUYKCbkaAQJZgKJagkMbIMLLaREihAUJSBihLUTBCAgQAUDZaQTJCLkIBkcURMLJLjgLSjYcBAhAYjBJTgCbBSiJZBMURMEhUhRMiMITEjABLiaBQaMKDCLaMARjbaRLaKBTjUAYTDYTZIbSkEJicTLiMkECRQZRZAiLJTEkTBhUKiYAIIbjZiUSUIQZiAQRQTcDUTTTkMRDjRDCMiYRIcZCICJgTZLRKBDASYALBAJDbUiLKkYTSMiKYDIQbRiCIaBMbaBKgIjYYEBJKaBDALZaEcACLMjJaIRSSTLCZBATKIbigJabLDCBCAaDbAgRSTARMJgRDELDKQbDJcQbABSaaghCLSCQaRZZhZbSQSaSaJbbkkLYZQLJAJJTDaDaSaKASQJbRLQCjBYDCYcbTLcDACZQTgYbAbMITZRDBbKTMRTZcaIJBYACEAAKbQKCSAADRQJKSRLaZaJYQAYZMIZKJQDaCBJbCZaBQTRRaABRJMDIBAbSDZTQTQLECCLZQKaLQRbQBbRUBaaAAhbESJJCSSYBCRBaISjTTIRSTLKILJLLjIAQTBLLLRLASCIZZSLBDDJBCRbUAIiALBURJRLbDLShCSCBAITAZJAYATLKBSAYTYhjJLJIQJiaaQKgUCBRTBYBbbKBSCbSBAAjDDZRJbYLYiCSYITRZMYAgLbCCRATIDSSLbZIYJZDISaCQAZDILBghYTQCBaMAQYJJaLSbLSCBJJYaKaabTbRRLbTBRaZSRQCTBISASBLSCcZCYSKAZZIQSbIQIZLDbDQALQSbCJKbALKaaYRIDhJQCZQCRLISMSIDRTKIAaaKBAaYYIJABABbJKaALJLSBZRCITYBDDLZBCZJDbLTCKQJAQCRYAKLDKYCCJQKSTZKaSJLZbTRAAEBhTBQTDACAKjQIKabBTLAbAbITbJQbBRYLZBbASTDIbRQLabIJSKLYTJUIQKQbCDiAIbCLBbTJAYChCDZYLjZJDIKCTAAAaaSQAJRcbJJJbAbDAATaURCYAbYRQQYMEZCSKCQRbhZAbJQRSRQBLaKDQRYAJDCMYDZaJJDCZYYABCRILQTSTLaICTCZLJZZZDQQIAbJaCAaQKZgBSBICSBZJaBJaATKDZDBUiIDTKbCTTCTJKBQZhCbBZYSgJAQTaAKKBBRAZLABBRDabRYabbSLDCIbLbaQSJSTRSLAKSTACIBBIJLQDaZLDALRURRDQAbaSLYABZCSbbSJLBDJRKQSBTabKRZYBZQDLYZQTDITAaTSbRJBCaQQDAbbYLjBaYAIJRSDCaLCIBKLJZSjRQaKbYKRSLaDRaZLAKCSIARKDCYCBKDCKKbbZZTIUbZbIJQLSAZITQaRATASADKQJZJbCZaTQLRCCCAARBAbJJSIILTYAZIbJKSTZaCSRRAJSDRRBQbBJZRRbCZRaIKbKAbJaYBRAIQRbCCbAZBLbAaaCQZLaJabSaDYSLbZSQCCKLZaaQBACALLAIChLaSBDJYJZCYSZTCCbSIYKLSLLAbSJBTJCZAIDZTAJQKBARLaBLQDCbZTYIYQDLRbALIRDRKSSDZBKLARQIRKAKBITSZaSARIIKBDDKYLZSJDIRQZRbRDZBDTIZSYbTAAAbCTIIQZTKZLBaIQDIJKKJYaAaJBCKKIRZSDBQQJBbKYZDDSTYDRYCLISDIDQZCYbBZTbaSYSCILbaKKRLbTCRDRbLQCJQaYQBaBKCbLAQZDaRbaJLAAZLRQbRaSKLKCAACYLbZZJBAYDAJKCaaJbIAAIJZDQAQbDKbTAQZLDRCISCKLKLTaBYQYAJQCIJKYIBZAKTLSSTYYLaBCQRBbLJSYZZTJDBIJABAIBRaJLRQYJSRQDAZCQQJIBaCCZCRQBZbJYJYBBbZLaAAaBASCBJBIYCTAKQITbLKbKDBZQLIAJTKQCSILaIRQLLKSLAbCQQZYZZSAZbabYZcRTabYAYRaQDTAbSSLZaSZbICYaaDIBLJATQThSISaRRJBRSTKBLKAZIiZLKIDaaAKKYbYIRIQDTTLASSBBJKCBCTITZRDTbLDTUDbYaSIYJTCQBBLYRaZRDZQCBTILKBIQIIJYTTIRITTYaIDSBCaJTZDAJaSRZaSIQbCKJCLIbSAbYKBLaDJBSCBTIBBTCKZAMTIDJSIbbbCJDRCTKQaQLATSKYSTAYCTILLbLJZIIARRDAaTAYCYaISIZDBYZTBCIBRQQYRSbBSZYZJKAJKJRDITSbBaRAQAaRRBDRBQaCaKYIBJYLQJRICLbaRRBZASLKAZbRICKbYQJTLLDCIaQSLQTTLYZQASZbZbKDabCSIBIAbbBTCATQJbTIYRBZSDYKLBKRTKaLZKRSSAIIRaQTaSKJLKYaYSbQZYABITJDKABIAIIBZIKJYbZaLTRKQBKYLDISAKbASSYILJIQIIBRQaBQTaKZLZIaQADTLDJRIJDJQaDAZTRbQADDISbTbaQCDYLTSBJbZRICYACLBISBIbaDbRRBCIZISDSCAIKTCIDKKaIQDKbDLJbIbTbaTSJZbRDCSTKKZJCbZBKYbYSKaDaSaACKSLTAABaJLRQDRKYKQQTBDbLCKbRZIQTSTaZQRCBDbYKCZIBBIRKaLQKbKaYABIRLQLTDRTYZQQbKJbAbRQRZQYaIaYYDSRATbbaKJbDADZSAQBQYZbCbQJJSDYLSTRJJJQARaSKICKQJIaaJICYLQDAQAaLaYTbDaBKTZDCALJQYZDSaTZRBZKRCJDDJbBLDaKDAaSRAQITCKTKbLQSCBZATYKZCBLRCBRBTCZKIDJQSLKZCTZAQYITZDLAbZYDCQDRSTQQaTDbbCBYTTJbKILabAAKLBCALYDRIZYIaJRZQbCbYDKTAIYRDCQYIZYKBIZLIBCLbDaZYTQBLRQAYBBZSSBIRKDKSISIaDTBSIRLJRZRDTRYKKLQKISZSLbYAIIIbJCQZLbIKDDBYSAKASaJYLYTLbBaSBYCSDSaZRTZJbSCCJYLQaATaIZDBBaBCBDQAbCBTTLbRYaCSKKBBAIBDbIbQIKTJCZaaDZSLJRTKQRTZDBZRYIDKAQIRRQJbIaQISDbaZJaKASDBIIbQAJKCRLRYBIKSTIRaBCZDIDaSLDISDDSRIDLJQKBKSaZRTbaQaLQTSYbZQARZCYCSRDbZYBBJaZIKJQYILJRADYCTSRbbTBDRaZJSAQLCaBSbCARLSZJAAaYBJbKaSbYZABYIaYSDQISQDLTILRDYQZKJLCZDIRaDQYLKAbTYQAKKICCTJTTYAZIYCLKZDBLJIYaIBTCYbDTZIYYaaIRSCIKaSbAKISTbYBZbKbSKSYKLAYBSaQTZCaRYRaTDKKaILAALDSbJSYSDaaJIZKIIBICJKTaJbQJZISKbLIZDTaARATCCRSIbbSRbQIDBCZRCKQLBSZBaDQaZCZJCQQSIDAZaITACYRLSbDLTZTJaBbbDIIDARbYDaTBQQaRZaBAJLSKLCJaaSTaKLQTRJSBYaSQDJaBYZRJISTbRIACYZCbbBKIIBKSAJIYYRYKbaCALTSDDCYDBAAIbSZCLLJDZBZLJBSCDQIABKTJIDJSJSCSZSZACKaSLICKabCZTZJQaATBBTQSTZTTaLRCDRDaBLTLRDBKBLaIZSKZBDQKJYJDCbTTSLCaIRRDQTABKQRYCSTRALITaBYKaZJaJZZCLCJJRQTCRKRJTKYJDAIaBCbBBIKICaaSLDAaDICTJJCQYCBLLIJJARCSKZDLALASTZLBaaKRJQADAZRKZYIIQbaII"},
Embed{60,192,"IYLLIIQLTTIDAIIIBQLQQIJAQLLLIICZabQaCaTZYaCabaaaKBSKaKYaRJAaAaBTILAZILBYYCZRBLQJaIADKRSYZSaTBYDTABbDIBKRDYKRCKCTZTaLRQbIIASISaSBJaTJYbLRJDAZITDaAaATLQaIBTTaaTIBKIBBRDbaRKIJTQLbDCIAIbSBaBYTIIZTbYbKCJZAKRZQZADQDCaBQZJZIaTBTTYLBAQYQaKaKCJAICbLTKQBBJTJZRKTDRbaRACSZaAQDZSLYSYDLDaACJKSbQSDIDTaaSZISJaJYQbZICQYTRBRKbJDBaQDYYaIQbLSaBKbBRaaaTbBRYRSLBTTRBSJKIaZDCATaLTAYRIRSIJLRYZBQTDLZCSJJCBQKCZARKKRRLYJCIAITIJJAabAbRbbACSaJRDAYTbQIYQARCRIDJYCKLDKJCLLbRJKCSSJDbKQZSJSTLDQJbIBbSSAYSIIBYJTBBBJAbDSQTTALbLTSBBJTJKITBCTSKKIYaIRYBDSZSKDKBDCZJaRZYLCKJaQIBDCDRKRAJIZLYKBTITCJKDKIKTASYaKAKRQRZSIAITaCSYDBCZZJAYLBRJLSaKBDZDRTKTLQBaIKARBYTTJKTZaATIQQRADaaCRCAbJJJaCQKYTJZQYRbDZaDKYBIbTBKDYAaRZRSDJCbYAbLIACZASaLISDIQZKJIYBCRSZKLQYJBDBDZbQTQbQJDQYKYKKJDRSbIQDBLYIJYaIYCaJICBRQYQbCDCZYLbAKJDJCRCLYYZLAbKQCAYbaRCRRZbBLBCDKQALIaDDJIKCaYBITCQTYCZbLLIIbDSKSCTBDZBaZLJZRaQLRQQQIAZCjCLDQDbcATDYTQAJCCJbYRARaBaUDIbaDaQYTTTRTLIIJSScRRaYDLaQIJjaKBRISYbabaSQDCZQCYYaKZbMYYSYACZBLYDTSDIZRJJILBQLZLIMQaQbAZZQAABTDQaYAaLBIYLMKSTbCSZJTCbRQTLUZDTACCIJKCibaTDCLQKaCSYLQBTJSZZRbLaZMTBAgKCIRZaZBaQbTIQBBZLbAAAKhQZRRCKQLYAYSYZAbRRDITJJcaUTLREACUScLLRJTCKQREgSJbBaDQZCZAYbCiAZLabARbRJKSJaiTBaabZYAcaCJSYTLLYIAahaSSjKhIaQbbRQIRZISQUIZbTKhKiDSbLjbJjBChSJKYUSBSCIJIRYKZCcCKDaZAajbaBRZTEkSKQTYBBARMaIjahYKUjEKLCDYLCRMaQDaiKRBSbhMChjDYajJBgKSkUIbLBASZIZBARaDESKYbJaQbYCMLIKMBUITIKMKERSKRQgZDbRDSbjiCYRZkZkQJLCZjSRYCiZhSRCSLCIMKZTCgIREBacEDcDZBSSZTJCkkEKSULLUQTSDThhIIERKaIIYTbYcgZcRRRQIhLjAMCaYRhkRQhQQAhhibIhbMSRaQcIaJKSQSRZbZDKicBikBjcUMjADEYiTgLJLQBakJSRQhMTIZEKEjTUUEjSZSSJEJSIKjLAETjbiTjaciBbQYZiECRJZTYRAcICKjLCUjSCQgMYgJjkSUBiUIbRZaCjTEbSCJSDAJhEDMkIMjUYcQDQMKiBgIjYAKMSLbUEhbYMcihKAcbTUCDcTJBRZbQgcihMUZSQTTLLUkiScDEcMTiIZhDbaCZUZkCkCaSCZELUEjQEZkkCBRDgIhIcKijIJLRSaiIYTDUbjQcKADJicKZQhLMUAEYARaUBihcRgiIgTaICSBIDAkbUCciJTUCYEJIbTgEMiZEiMgMRkCLLgiEbLiEgcIJhAQDDMACCTaAUIaAJcLcELYkaZSKaYLLIDEDEMAYCRBhSSRLERBMMIccDLYRARTELEkRBKUJBYKZDCQSSDYkAjJjIAaMBTZaiLSJIabQThDBQRCMTRYLCTcMKgQKBKISjcQDDJRMbYKCKUAQKIJLLJcZbYQESSYEDKRZZLMbSJSSABJCMabKDQjBYgKQQYKCJBIAbCYKKajIZIaAEZBIRRChYhKKACDCbKATJKJATLBagKATgLIMBYAZaYQRQIBBYLQiBQRSLAhSgbQaDbRZZMbKAYAALKbbZCjTJaUaRccZDQbKSKbDQRASQKYQJBBAYRhBbbgYDRSYDbJRLBADIhCSLURSTJSUSDYBKJLRRBKDRaBDIiYcRTDScRhCECaLJaIBcjbaTCJBSIKJSATBTDRAjcTRZCbSbMhLZiCIaQbLaICUTSZBAbBaKbEADJSZDZRSRCQZQJcBDRaKbQLTbJhYbTjDbQaCaCRYCITJYhCZBEQLaTLTBBCQABTRDCbZJJCJKbDbSIILZLYQZJAZRYYQIKCKAIQKJSSRTKQhJYTMKUCKADIYRJKIBSJCLRTAYJbAADTZLTQAIcAJDZKbRSCDYBSRJZhTSTDKaQSDIITLLThaYBJQbTEbTJIYCAKbBQbJBCabQTRLaIZDKaSAhDZIBRaDKCSSDAAJaaJJKRYjTBZTABTZKBJTbQgBYScKiJDSZIAKJQLaCKIaLJBTSYKCITAQDTBDDiMbDcAbSYBDJCSChZLSYBaYSIJTjTZbAaLaLTbELbRAQIKbBYQQBTIJJITSCZJZbSZEDQSCTQbTACTRRRAbaSMaLTQIKCBBBJhDADTRTKZJDSCKQhQSiZSBZLJjQQDBSJAKZDLbZZJKQIBSYJbBATTAEIZRAZDACYbbYYLAJLADbCTCIShZKCKDTaKLDYRSLSRLbAaiDJLBBbILCBIJaaQCjbCBIRLRITgQSDYRYJScZZZbRDSYKAaLKRCKDCABDaDBRAZJCBSYBcIaDLZaTRCZKDJADISIBCJAZSDbaKUBJCLBSYQQZSBYaTYIJCTJZQBABaIYICTJILSZJTRYQYTZKBKbICBAYacZSSYbaITIABRRJIbTCALQbaZbQBKCSbLIaADRSDYDYRjSQDKBZJTJbYZDQCYJLaRbBZZQQKBbTCRJCTSQSISDLaRCAALKQbLICbIbKQBZYBKIYCACZJCKAbLZQLQTbaDATJIDYRAaAABRRRbDDaYYQcRKIDSJJQZKLISTZYJaQgTQJYKIIaAZaDKRKDJbQIJZJAATTaRYRTQTCZCYDKYJAaCBIILADQabSRALDaZDbYJRTASLCRRIJUAIDCLTSCCbCLJICZYZBbIZLaQRKQRIYZQSCbRKKLCCAbBIZKJLbDaCZJSRIJDKCAYAJAQQJIQBCJJKDJRBYKSZaDQDCZCRLQKZRDKRDbIIYhTKLQITKDbYDLRJAYZSCTBLYIRTZQJABQADAJJJARZaKIJDIBKKDBAYSaIIQSTRaTYCYYbDLZISTAYCAKJZSBLYBIJbSDTIbQRBKDLBLRbZKCSCIKSTRabCDabRIRQQCJaYSKbRTDSDBQSRJbKLTjABLSRbCDSQQZTJKSQDZLbJYKZIDYCTbQJBJTSRTRaSLDJISZCICjZARIJLYaDYJYRKABbBKRZDADbJRTQCJCAZKLSAbQSJLDIIDYLLJJKZASQQYRAjRbCJaSCAYZaASSAbKLJTTCKIaZTLRBBQZQKQIaIRKZIJYDTYACZQCLQKJELRRDTRRYbSKTbZAZLSBKZIYZBZZBYTQSJTAQJAabILQSCKRaIILJYDLYSJaDKJIQbbRaIQTDQSaJLDCYSAYBDASBLJTYSbIDZRRYRKIJQYYbCTBaQKYbRKTBTRBKLjDDBADbKSaIaYZRAYYAATJDCDZSAYZKLaLKZKaYAbRLZaQCRaJKBATZYQSAILZZISABRJYaBZSAIAZRYDZJQJKLDYYKCbCaaQCADIbCKRaSRaDbDRBRBIRITBCIKRJTLIbKALJRRSSbZaaKCKLRQLQARJTLIQaCRBCbAQLZKSYYBbYCTISbRTbKaJISbTQYIRbDIKCLBIKJDAIYCDBZYDQILISLSLJSbLaaAAIKYaQIQQZZJbRTbJCQaQIAJSIZJKZBKADYQCRAQQCALbZDbDJLCbIBLALSDASDZAKbJZQSDJCBRLYRZCQKSABTSZRDRZRIIIAJQKYRKYZZbYSLLLQSASLbLSbbJKBSSTSRAbKRJCSKKKSaDJILbBDDDCBBDJaSIZTCLCDQDCRYKIADQTaYKKIYZTZYCSITaJBKDDaAJZbRYAaLJCYTBKBABRYDZKAJYJTZQDCTATIIaLASRBYYbTKSbIDTbaaKCTAYLRabLZARQbJDRYCQSSYDYBLSARaaLBJYLRKJDLSbbYCDCSQLKYKJCZIaaQCTKBabQCBDBbRDICYRZbLQaYSRQLQYYJLICRKTRAJCLJKATKDTYQYBbRCKLBbKYQIBBYZAaYSAKJTJBRTCKLQaABIIZZKZbCaDaLYLIAJZATIaIQBQTTCARJZCaKQBbARBTQbKJSCTaILaZKDLbJYDRSKYZBJQLQZBCZTABbIIBTaTLZYDSSYAIYDKQBISBZBYaIIDQTLTaKTKJSRBKZSLDBbAKLIbRaISLCJQYBCTaDLDBKJQRAaYKTCbJRTAaJDLBLDIQRYKJRCKRTAITTJBJSRaTASSJRYZSSDJYSZKCIQDJbaLQQKZZAQRJDbKaQKBRZQKDYSJDaDZKBRTYRDKBbbbSSLTQaDSDIJLQKaSLCIYbASADJDRJLJYILaQSBJCYISTSRaBDAJSSaYBKaaLCRCTCJCBTTTLTYCYZbQQQIIKbbARCSSRLIbLTTQJDRZTAJZYYCLbDSLATAIYKQLJQTbDSbAaaDJQAbbTDTDQbKSTCIDDSIZJTSYbTIAAZJCBTZKLYLbAJRBSIKCbAICSIRDBCRJbIDAALZDLTDLYBTbbZQbRKQADBaAYCCIDYSCCDYDQILLJZTSYDATLDRDBTQILRQKTYbCAYAbbKSZJISabJSSAbCTZIZTIALDTbAILLCYZQZAIIDaIBBLbSBBbDCKabAZKJKIRCZKZJJDLCYbAQaTCASTTQBQKTDSTYTaJDCbCATAaLYZaSTbDZLLSLRQTYQKBYBRKKDLZabaRBTATQRRaIATIbZYaTYZABDIQSIKaZTSDQJTLIBJAICIQYTJLSYSbAZQBRIYaTZIbBIbSQAaJDSYKDBLBYLaDDTDJIbKSKYRaRCYTQZSJKZBDRZDKaKRAbAbAbbDACLCAQCTABJCbDLAYKLaTKYATKbBZRSLBAKAZbQQaLYRTCRSJCaAaLLKLKITZaBYTALAZLabAaLQbQDLTTAISKCYYBJRSKaIZbBIZTZAKIDCRCCJSRCYCZBIBbaCTYRaKaZBICTaKTZZYKJAAZBaSIaQYJaaDaCTKIBQJLKALJBKZYaAKLIBSJLIDJSJDRIaCSDaBJABIIKISLQIbKCJZZSBaQSSIQYCQbCDLSRQbJCJRbKLLLKZRYBaQKQSKJabKYCQLTKJDabCaBSDZbYSSJQYAZKRITJBSaSZTIaYBJZYZIRYaKYKJQKKAQDQKbAQJZaCLBbLLQBIYIJJKJaCKTKLCALDJYALCYCQYABRBTIZCAaQTRSYTIZZIbSYJJSTYaSIBbCZALBAZYYTQZaJCCZaAKCKIJIZaRTSKDKRJZaBKJAaZTIQBYSBQLYDAIZYSQTSDLTCSZIbRCDBRQZJRCJTbJBAQBQDBZKJBKKRQSDSCJBBYKTbBJBJTJDbAKKCbaSaCCRbZDYICCYZYQCYZZbICJQICZTYYYIZBBDDDSRASAaaJACRADLIDJbTRLZIRLBSaYJJDBSJaRQRTYYQJDSZLDAKSYAIKKSCRKACCIJLSbCRJbTKJbTLTSB"},
Embed{62,198,"ILLJAAILKIRYIIQbaTYIIAQKYAIIbQbTBQaDDaaaYKaBCaaaabYKCZaaLaaCaLIaBTYTZBITBaITRaQKBSaQBaLJDBIKZaTBDTDBITQbIaBIKYRLCYKBQYYKCZJDIBbITbTBTBaIIaBLDKRaQabIKSYIRSIaRARKIDJZDZSaQCJRAYIbSbbACIbIaRLTCBYBTbAbKDaSJADJTIBBALaZQSIDKQKTZTZbZAAAYJJKBKQKQDQLQBZTaTJQKARTATIbZSaAaYTYIAJLLJLBSLaBSRQZRTLBYISKDADaDbQIaDaZTTIBTIQDICYbaZZSKRaBKRRYIQQRZATIZIKDLCLCDKYaaKQAKQYbQBSTBbSbJZYLIKRDRALTaKCbYDTDSKabCYaJKKADRYRRBRSLKZDIJIYTJRaKRLCaQSSKTRLSbYALJKTYIQYZIBIDBQQDSRBJTbKICaJRYBaYATJLAKaYBBSTTBQTbQJZACCJbaLIQAbAbaabYYDabDSTJYQIKRIRTDKKJDaDBTKRSSJKBLYBCDLAJYKZLDQSZKTTCRYCQSBIIJQIIILYADDbSZCTIYCRDLbZJSJCSDBaKJKRSBQbbRLQBaIALZaSSBaYDIDTCBCTYJTCKJbKYaKCYZBaARIYBbJRDQBBDSbKDSLTJQALLDRRZaJaQDaDDSBYDDBISJICabCLILLQIRSBKLLBQTYBDbCKBaDIAKRZDRYSCbYRbBYKRAYSSJIbaCaBDZAJaCbISBBbaKTDYCQYbIZbAQKbBCDSZKRSDIDLTaBAIDDKARRJZQDZISLIQYZBCYLRILYaZCZAICRIIZDKKBYTCLSDSTCRYYTiKBLDDSZBaBDLABAICCaQJRLaZZKQJKSICRQAYJQKKbTSBILAYJYYZCKbTDDaATAKKZZCBYLJQKDYaALYZACaTZLLCZKASaQIMBDYabbLAaIDKBCRYLJaYQIYZbSLJBRCQDcCBQSaTZLLDAKbSLJaTIQZLbQYDCTKbCBJISYaLcKALDKKISBDJAQYQZaDJaIBCBBZLDZCKRLZABIYCaKQgSJBARbYCTagLCAJbBYLDQQJBAJCBJJUALYQAIDYDIUDCDLIERZRaAZjIAJLjCYBbbBILMiYQQKaSaJDKbKaYRZUTCDAaARIKTLJBCREUJJDIZRbSAITRSZTbMCAYgYSbUaTISBbRKJJgKIYTDjBSQSKhgYRTSZbQAMYTaIIZbQYRBATYbSCTYJEAZjCYLUARSTUiAcSKaLZSbDDAiKZaJJYAcaATBZCJAhZAEKUITQgaTKCULTLSSJbbLaZkTJZYiCKBjCIBLTUJAIBIQKJUQTKJMYQDTbCcSjghTKTaTAbCZYRjhQRIaTZCMKZUTaJbbRQccbhAbRZMSASRTCYKgAgCUKZLIIgKLJLSIYQbUcjKIcEAQiSLYBZDZYJkQBRTcECUbYARDRCDYZEZECAhCJAMRBhYacAbDiDLChQRYLjjMaLJASSgSBhbQRKKbaBEMKZREJZSTSKjQZBDMTABRkTYKJSjRbgZIEDKbDKEjhYgURCMDKjKSJTDLLCCcjiCShcbhhTEiTbEKTUhKZTIIAQYTYTBkBLQEkSLQAZZkJMILSggJBTALRjCcZSRDYMJcbJkYaYUkQCUCcLTBjZCJURMSKJYkibRjYigBEBaUZUYCIjRcAiZBchjDjTRBZjBQIjLSCQkaShUSTLCaZALZJQUTKZkMigQKMUIbhgCibAQSQAZiLBbaZjSjkRkcMYcAjjZIRQgTEaIhaJacgajcACihYbIQYCUEMcYAbUBhQTMZaQkAaSbbIJQYTYiKccACjLUiighgLiERCKYBiKiQRSjJgRgScEMBEkDLMKQgBZbMTQMMkkbhCAhkjTKJRJSBAgLgaKYCjKKkcYhAKAjLRcaJEjYiDIIkCiIKEkJgIYDLEYKRSbCKYZRCckSJShZZQiESERKCBLSRUTACIakQLZgLZCbbMUJZMKQIEcaKCJRMbBZCBbCMJAMADQDcLRDiIYEACATZAZLLZQIjCJLKJhSjQEBZURZaYcTRkaIRDYSICCCKBLacDJBIIThKgcQKbYjBIbADKUSbCaASKMKMEABLSJAUJDTDAaJbRLACSQSbEiDkSbbISCCMZAMZLAZaKbLcILQQDYAjjaAbSSaEaSITbIZcQTCAEYcSCjRJYQZLIRJLAMEbAZSARTTSIQRRbEShCaAbMJIZRQBbCYYZTTRKATACYACaBKIYBYZkLYJEBCDJBDZBIYCEbSJZgZTSBCBLCMBYacEKAKjRDIDDLQbMQZJJCDbZJSYZBRZCTbYLCKSDaCITSQBABDYLCKbRLYCTTBKCKLUDLYakhRIZABiQZjDgCgIQTYRUBbTJMYRaYRBUZULBCEbDAhQLDYKQRBgKIbIZRBRTQDIaLYQYCaSLcKZbRiAaYaYKTLDiSTLISjaRLDRMTDZaQZZIABJCiaKCQTRaaagRRbTSTKKRYJKSSISTCLJLaCJZLaCKLZbIRISCISDCTBJjZRZiZATQAILaYIAaKUJjZADbMBBSSYCTJSABLZSjSLIZRIAUJLAbQLRYJASbASKLRAYDbCAILBDYCAbQRaCZJLYCCJDCILaCUSJABDjDQSYbBKLZCTBbLKULIQbISCCYLCUjYBJDSJQYTTbZbaLLCZBbSAAAbJaaaQcDRRCEZJIIQYBKIJbSbSTDSaIABLTKIaKBIABUJZTBKYYDaJATSRYKSDYaQaLSZQIARLjCcJIcBLRKBKJSIAYaRIZTTCZCCLCIJDSBAYZRJcTabDCIbBAZDIJYYbKIRICJKIETKYRDDQAIRMTZTBJRIAQADADQCCBTRJJbbbIRThBBQICQbTQDYYDSAbbRTJLIRTTKTCLAbQDTDaTYIDZbJaaABTTJSAQaBZTCLbaCQAJQbATaARCSYJJJbAYZKQLQJRJKaaaAbaCSZQDQQJZaIATLabhYDJabDJDKRKKLTSCBJKAYgZIDZDTQTaLIDaJSaRYBTbTKSYRITJaCLZBbZLKSAALaADQTSYMKYRQbYYJQTTMjbALQBYaDKcKSLRAaARBBRDIQQhJDKTBRaTSLSAIIbYLZDBQQIZSACLKBDSKYKBbJQRIIJQbajAQDaYBYJCRRYLRABbASRKMTAJDLZSZCRCCLSaJTCQIQIDJLbRLIabJDCIbABRBLSRTDAQLaDZCLSZRbDMaAZSYZTQCLaIaZZSIASSZARDZbDYaSKZRDARBRRbQEZYLDTIaKYaJSQIbLYZBRYTIajTBLQBIQSZYbQSYYKaLCTLLQJbSKKABCQIATIYRTTbRSMYTCDJbKLTJKIBCQIbCZbQRTDbCaQQATZIZbbaQSSaLYKTALZJTaTQAARATZAJBJDSCLZjAQKSbIYbaETbYDKDLaQAKLBQLYYYJQAYRbDDaDBKbSaTRaKKCaDIJLDTLCaYLLAbQZaYKKbTSaCZLQIQIbIDABBKAQBJALBaTKIQQIIALJBgJLRAYJLbaSSCYTJKTRSZRSDbTYYAaIaAYZLKcCAbRYbbJJRaDBAZAAIQIZQLLRYKDZDBDAABYaBADQLYKYLQALJYLBSabSaSaRZLRRDKZCabSSJQTcQATDZaTDTIYSKQaDLZbLIcCKDARCZKZAZCYZYABIILZDKTSDYILSYBRaYIBCRKRCKZBRZBbQbSZLTIYDYDQQTIIJZJTYbDQBaSLRBRIYRJCRIAQaRBAQbSRBYQKTKTAZKSBDSTbAbJLSLTTZZZSTBTQQJZCLBDADIJQZAYSTBZaSDKBKKLRSaRaCRaaQDCTJCbcZYZTaBATDJIaBQLDbIaDQABCICYRQJYIBZQYJaSZaJJKQDCBLCTLJIbKEJZTRbRSDaCIKBYRAIZDJKBQAATBZZLITKSQYIDDRAKLbSBBYSZLCZLRCQQQKDYLJLRKIYQAJCRAJDQJAQQZCKQYYTSLABDLZLKKTKRYKbBTAKTLBZRJZKILZBAaQYQZSSKQYTTCSZbagbZJYaabIZCAADLYSCDDBCRZbLALCBZQCSCAaJQYZBRCCSCLZJYRaDYRBLDYILTbaAbJLRLSABTbQSCJKDDZSbZbAQSZCDBaZaAZIQRLTIYQCAKYaJIZLIbZCYRAaALIbDCJaaBDAaARSSJYLJLLCJSIQIZCISbaKACBKZTSIcAQRTKRbDBLRZYDZaDaJJABCDZTRCQCbJZLaLRISTJJISBQCZZIKSZLBDZDDTJZSSKbZQQQSQATQJbRTZaaTaDBJLIAZSQTKTYSZCZKKTIABKTbaJASLTJJBRTIaaKDBTCCBBIDaaCCTDJaZIIZYaABZJISYACACTTZaQJBCIbKRIJISAaSYCCaaZaaZAJAZBQKLTTaRTKLRJCJIRDabCTIbRSbQYDARITJDYTCCaSYLaBCKJJQJSSLTYTZBKLTLJTSQBaRIBCZLDKZYTRQACCCCKYDBYZTLDAbZaZaTIDYIYIZQZaKbCBLabQTJbSLSCBQCRLCaCYDIJCDYBARJCDDbKLCJDBbYSTARbbCTaYBaZJaLQBJARbDSTISRITYbYaICKRZQbaJSIJZAZKZQZRRaRAYCBCDJSKCJKKIKTQKSCTAQCbJbYYZQRJaBRYJQDbbbQKTaYIZaYTCAaKTKIaTaBLRBCbBSBDZZBLSLZRSTaAYYQBDLBSaKZCABARJYaDLZRDbRRYbQZKKbIQDAZBZaSCTLBAbCCQATITBbSYLBQBKYRZbKBLYaKQAYSSbbBCBIRTIDJALYYJRJSBLZSYDDDCAZQYZSbLSSBQAIBKTYBJIBbSALJYZDSSQYAJJYSKSDBZRIJYDRSABaaIKJRJQaJQDZaLKTYIaSTbJbLCRSILATKBCQDIYIaYaZRYDaDTCaBKSSRbRJaQRCSbYJbZIYaKALaQaAbJLCDAbABLYDTLYKCaJBLTDCbATILJRICIaICLYTCTABZLTRQRDKDCADTJaRDRIabLJSQJAKLBSCKbYJbAQDKabCYKLDLDLSDYKAQBIQaKbAYBQLbTBJADZCTbBbQTDIJDbDICbSQBCKZRTYJTBABQRCAZYSDCBLBBJDCIBBQBYKLRZAJKARaRALbBZALDSBCCaRDbLQQDKJZAYLZTBQSSTQZRCZAbAbAJaJCYSIBTDSbTTLJZKCADQYIDIAZbABBTCTIIYIILDLJQJJSBIaaYBSRTTbRbCJZaLJJCaJCDSTaaSASYAJbbAJBRCARDJRSCDKKbAJRSICLZDYbaYQTRQKaBDRSLDQTZZITaTZaBBBaKLAbBTDZKTabTCTZDQZSBBQRTAQYSQDJbbYLQbBDJbTKBDSRTQQDBbDATBCbDJQCIIDLaaJbQAJYDIaKQYCSACILBSZRQKLIbKJbADSJDbKQZaZDIAKTAbADbTCSYCZZKAKZbBQYAKBYQRIbbaITLBSIBYYIISQSLLaSCILAJYCTBaJJCBaIDJLJDCKaYabbLRIbQaKIKCBYDJISSABSYSLQJSDBQYAJbIaRLCYRLIYZBSQbQIADZKBRYBTKSRIACIQCDASQYJLLZabSBDDSYbDJaJLaKbKYYJDCKYRSTLQKaSQCZYDKTLBITaQIbJQDDBaTLQYJDKaTSJKBYKCbRbSBICZYDIDZaCTCYBJLCIALTSYLRDBDaYYaCKZLaRYSKDJDASKTSKZaJCDJJBQDbQAZDCIDRCCZZaTCLYSbCbLYZAQaAJZbCYYCRaaASZZDbQYLLYZCRATSRbbKZAYDQaIADRIKbKaZAJbRARQLCJQIABbYZDBIbBCQZLCLaQIbABJZQDBBRCaKSLIaaTSKBZLCaDDZSDaBKAbJZZDSYABRQQBLTIQDRaaRbDZJDIRTDBAYAZZSLRabQCDBCQJBJBYaaKKAJQJaLCDYaTYYKZBZQSDYCYQQISDDJZQYKBKTSLZLQDZITSZRbBKRCBYDCYAbKJIDYJQKQTbRLZQQaTAILQTSSTKBLbZSSTTBbDbBCSZIDQTBZBJaJDLSLRCBCQIZZDDRYBJCQKTRSbJBAYBQIZLTSbIDSQCBbAZADbIbbbBIaITQQBYLAIKCCaRCYbaTaZRCLKTDIKSZaCZZaAJIDSZQIRRaRTAaaKYZRTKZTZRLDSTSCLZDACDQQRLDJLSJSbQaYISLCDRLLKT"},
Embed{64,204,"ILJYAIIIYCIILIKIQIIIIKIYAIITJaIYYaYYKCSaSaaaJCaZSaZZSaaZaKaaYaKZIQJbJaBTCJDAITBaITaBaITYIaBRYKRDaTIBaTCYQRSBaBJTIBSTIbILDAZbZASbaBbLITSBJYRLIZZQaIASaBQCKaDLBSYaIBaDaSIDRRJYBKbDTTJBATJCSLBISJQIbKZDbIRCDJZQbJCYKDSKLRaRQRTCDAQBKYDRCAZQSLYTZabDAALKRYAaLaTYLRLDQYKBRRKKLCDLSRYBCbQRIICLQJbJLSDKKCSTYBbZASLTSbJKYQLbSLaBYRABTBCZSYYYTZQDBTBRBbKBALTQDCIIKRCLbbSbIQTaYCTIaIbRRQLAJTRDYBATJILKQAbZKTaJSaAZTKJRAIYRKZKabCCbYZKQBCbQbKKKBYbbaQBbQZKSQDLRBDCSBKLaaaaAYaLaaZaBDCRRLITDTCCQCIbSLKIRZBJSBBAIRSbYBQZRLSabaTaLLIJSBYYBDSDYabCIDLaKBJIAQDZBBTTAaKZDBKRbDAJQIKaTTRLTBQQCLBLAIZbCRKYTTbabSCLCKaZICJaDIRaQJRRKBZICIDSQJIaBQTYDbABDRYIRYLaRYDZIQSDKALSZDLaKADDKIBTQLYaALILZQJIaYCSARbZISAaIAbLbaIYRLSKZKCbSATbaRAQKBKLKZAJTKBADIDIDbbBBKSATRSaRBIZKTbTBBDSRYYLSKQDKBRSDIabbaKaQIIDDDQJJTBKbRCJRaTYCZYLQDaDJBDAKCKIBZZICJZaaTQJYLSABBRJBbZQRLYALRTQbTYCAaLDTCIKbbADDDDBLAASIILCDaZLTSBJZZIaQRZDDYTLaRBLARAKYKbRRTQSILaZDARCaKRbDIRBZZKCLJAQSZKBaYBDYBBSDRTTCbRRYYaRADRIbBJKbaSDLaAZLbAQTKCKJKDRYKKTRQKLbAAIZKRaZRKBSYRaALBUQbKTIILASKLRbATZIQQQLKIbYQYZLRJDKBDCACJSBTZMDTJYKgaZCSARAIYRRJYISZKiZYBBQJRRIQBCDAIaKDZDKaYLIIJRCSCbZaaIKYILKLCaAIYSbIJTTZbQZSjIICTURIZRbASDbbZkJcASBDKKSBKTJBTBYLQCbDaBAaBZghSbMYAaISLSCcAUDZTBcZbbCAZDZCYJLCabASZQSYSAKbDbTiIZTMYUJCDaTaSZTZKSbUIRbKYUYcJEQEKKaKYQTAZKjjALQIEREYKBUCSZbAaaYKBJjbTYTQATahERBLZKAkLYLghZSJJCCDSaQJCaYCbQbEYCDYCJKcYbabaUTKSAJAZbkQMJAaaDREaAADTSaTLJbKSIbYkRJJThQKCbMYSChQRTSZbJJACJZQAYTJRDYkTIBAhMACBZKbMDRibKLYRIASZiZCcAZCEQBKLRkkEBhRDbKSTBMjIYLbCgCaDIRSQLZcIYTKZCYZAYRRRkJkbCULMBATDEaYkbBgADiSYbZJSKQAbJZgAiCbZbLMcSRYTkTBcigRMCCKKMDRaaTiUCBJCIRUaKbhRRhKKYbTLQBIiQAbkaJbDYhABbYaTLSikkJSSBDYLhQQJLCaEQZRYESKRBBkJhJcchRYjCAEScDBaTaTCcDLEDMhgbADhSKCgRBBZZJYRUbYBICCaUQILYMZBcijCDKEJQkLLakQJSjBJAQUaLjLTUjLLSbBjgbZgUaRhASSLDZikQbRBUjhRCIRTkjkaKCAgKIgZJUiMcSEABUMIAJaDTbbUhRgQLjibICZhaaCCZDIZaDgUYiRDSDkUhZZIgKggbajCcCALLCLjAZiIIRiagJkIaDkkMAjLDicDDRigTJZKcaaUCLagDTMLakLSbCEiCkEjMKRMZJZLRUDEIjSiKgZUIaCSBccaZcAQgMQQTAIIhcUKQMMZhSBjIkjDUMSbcLEESECTbCAcaKZBDUIjMhKkjJQSEEbRkiAiEBgKYYURbbiKjJMcCEYRiaKZRahEkaUkbcZAATSZDiMKhaIJAQhjbkYYMSTUZhgRgIQSILZSgaZTbRaJAATgZgaUkbJIEggciCgKbMKJbALYRCCAKLTgZLQDMJQKhETcDbAUIZCIZLUYCbDSYIJkBkKUaaZTATiDhBASITQIKRCTRgKYQKBbKRRZJDciTcJChcRLLLChAaSQacALAbSAEUDbUAZZKZUYhcCgRYTCSZJSSgDCgQkCJZMbbIbZRLBJALCYbBTDJKRMJDZSEKghTjSBjIDhQSaQKQaDgaSjZAKjILBTDYIbIQBAMEIBJIcaAJAMITJCbJIZSAhZKaMQQDEQDjaSibDRZKISLACCTDbBKiAYjahRgZAkIIZKAZZITShJDMZiJTKCTCDZTJCZJBYQaJLCQLSACKCDMYYQThAKDbKAIZQBTITIKBLCbTQLYKUQZZADLQaKacTLahZaUDRQIDTaDQaDRYaACbLKTITDYDDhQJbACZQURBLZYbJZYAAIALZZQJZIabRcRYRTcSQYcIgTZCBITAAbSAbJLZYBhaCARTaDAaDRKaLLSQISJLTSTLBaJYLJgKaQKRaKSSAJMSaJJcIDgSCIaEDbCIacRQbIhRJSLQLLhSYLALKYDYIRTTZLBJZEcYLALADCQTJAJAYJKCLAALQBBRLCaaIIJYTaSTTJKISSQDJJYhLBBhITYSZJKITbKKZYSQYAJBaSQUDCLDQZBRSZIJTEhgBZCQIITCTaCZETKKTDZKTbZYQCaKaAKBKDKZbBQJKQUIJCZKRLRBRYRITRDMTRaLAaaBTQUaQALULBJZSZBJYBICAKUTKRAYZbDDbJSSCIJKBTKDCcYRajbJIaJDJaCIbIKRSJjLAJCRTAbBZgTJQYaTSTJASQJLZAKCbTaTDDYACZBJIEbSCbKKQZAbITBSISAbZJLQLQIRbZKRbKSCijBQJgSZKDaYZYYKYaAJDTTZDJQLICCDYCYCSSRKIJJaAIRCZJLAAIDLQKTYaTZYaIBQZJTRaQIJZLRYBQYSITCATKSZTbCZCaYDAJJbaQBCCLYSAbSQbLTRIaBCTTKJaRTLabShBBDBLIcCaDKYSKILQRRQIYTLacRCaKRLJDITICKRMCYRaDILCJcARDYAIKBBZELTYZZBRLKTTAACCQASLRYLYSIIKJBYYLDCTbBShQZBYKKKJJBaJagaKIbbQJQKbJRACSSbaLbTQZRDJYALKZKSSBCDjYZCJICcDQaCZLSDKCKIBbDCcLZZabASLiBTJJCBIAjbIAKTDSRCYZDRIITQCKDYbbLJahJIRSBSDAJJDZAaZDZIQLQTSYBIJDKTgRBLAbKIJTCBADACLaJZDKDRIQASRSIbKZbTTaaaQYDZbCLLZTRBBBBCDSZQRIQBLRJDTJQSKKQACCSJLgLQKAKaZQbaZQJASRCQJZRZaRLDZIYZRSARIBABQYaIhRDDYARDDYaBSYEJBISZDbYLYYZTDISYADAJSCZSCDQbLYBQJYCaLDCIYJbZJRJLYSSSaLZBSYRYaZAZKIYACDJARTaIKTCbTBTQKaYZDEBJILJKBTARBARabaJbSSAJAIDBaTSLSCKSbRbILRSDCQDBLBYbZTDACTLJSTTRBILCaKbQRCDJIbTSAbTZYICDRJbAALBaJCbZRLAILIbRJIDRQLIYKALSDSQAaJDQZBZAaLCSSRQbDaaYbLSbDJJTBKQSQbDISYZCLBDDSKBbKCbBhaJTBDbDKERCKKCKYbRUBQAKDSBZQbQLbRaaCCbJZRZZSbQQCYJRZICBabZEZRJKaDIYBBAaIZYBSgbJADKJTDYCAKIbaRIQLLQCKDRKTLZRbIQQSQKRCaabDLLBTZZaTRTBYQbbbQDTIJYIBYQZQYQTQTbDSDSZJLQRILDAaRIILbKIQTJbDITQJBYJLAaBAAQaATCZYCRLKYRRYRBJSZRSDZKJSBBDbCLSabZCUBYSACJSJDQYDZTZDDJaJaYCLYCaDADAIJCRIADTIaBJTDJDabADIYTAQLaaBYRBaQaKQhBbCLRZSSTKYBRZRBJCSLYLABIYAYbLIISaKbLKIJRBbIBaBCQIKTRBbACaRYQKCCDQbKLDLaBLIRSRDDTbKCAAIARJJYbQLLaSZKQTCDBYICaYZYRITbTQJSYaCDZTRbLCIBRBJTQZLQQYYAJJABBBCLDDTSbQAQJIbCbaSSILaKRYZCDTBQKJYLALCKaRKKYADLaaJJbYTbKaYBaJQQJaSCQAKTLKKaCSLQaaYbQCbaQDJbYCACaTYZZaALLYQBSZDJJYaYKLTLKYSCaRQZaQLSDZBSaRDKYLCZTAaTQJJaIBaaAIaYRLISRSABKDQZbTATISIbIIASDbBJDJTBBKRaaZACCLCIBBbQRBTAZIIITLSIaSKTRJbQCbIKKZQYDABLDBASKADCYCbICbbTaDZBbBLYIbaTLJLBIDASbDbYQDRZRSaLRQCDTZSCCaKQbbJTbDZYBYABBTYSCKSYCRabjITBRTITQbRSLbQTCSDYLICLRLJACJRAJBIKbQCRAIIbCTbJICLAZJabBICAISbTBbaaIRYYDbbIaCbAQQRDaAJCDJJDTKTBIYbIKSQRQRaDKAQAZYKTbaJDZTZYCIZZQLbDQAKZJCSICZRSAYJLCLAYDJZKCQLZCBCLCDITZCZBSIDKCICRIDKRTaCBZDAQiBLJKaRDZAaaLYbQSQQZYbTYJAJSJRYAQAYTTTJTYTYDbKDaIYaaCSScKJYbbSLDBQQYJQaBIaQaTBCZZCbSKTLKYRBbZTKYSKISaSQJKBABZbLAZBQQJQLbbaLQKBKQaYCICAaCIJTaDYJBIaSCLCTTbIKDABBZBDCADZSQABQCYJYRQZKRCYLJCaZTJYAKJbSRKDKQQBBLbTABDLQKILLAJKYCQKSbaKZISJATKSSJATZYISaJAQRDDRLKRZBYbQTYLZTDDQTQBIIRJJRZZTTDCJBICKaSQISaJaBYTRaZYKKCaKbZLIZaCaRaYBSDBJKSBYRDbZZSACYSCCYYJRSDYZLTJbQaBaZACRCSACaTaLRRTTQaaLAabTBJTbDKBaIZTIKDTaCBIKBBZRKRTSaYSRSDaKYKZCZbTRbLYJAQRaZSJRRQQIDSDYAaCAbAZILBSJSRZQAaZCCBCbbBLIQLQIAILJBDYYbYACQILTTSSRbRQKIQQKIZBDbZYRICSYbZDKKQCIaKBKIZLISQbQBJZCDLBSSRTZZJRBIQbSYQATLRbBDLZIBKKDQZKZYBKDDQIDAbADIaAQSKRJTAAbTRQTILCZKBZBKLJRKLRJQQTQTSTLYbKZbBYKRAbBZKJRAKRCYDYDCSALZQYALaBJYaaSRCBYITLJRLAKILDbZYYJDLCaYSJIDAITAaBCCSDLaDYaSJKYJTCQCDJLKZDKAKbYLRDAaRJAISYICYDLKRAYBaCTTTZKbLJSABRLLaQISQJZSTSTBCZQRYTSRILYKYBaKJZCYJAJbAZbQCKBZIJKBSIRLTZRKabIYARISSJTTAbCSITCbCDAKDZAZDJJITKSZKTDTSbRBDJLAASJIYDbJBKYSbSJCbBZCbRTACYKLTIAKLYYLZbIIRSKDRCQKbaQTRAIbYCRLQAbIAYJBJRIYSSDYDBbZICDCZTBSZYTIaALYbZJSBYRKKJTIAaYSYKZYQASBTSSBZYbKZARLCTJRKYZQZQQZDIQZITJKTICZACZTaYKIRZRDaSKCbZLbQDSKbKTSQQBTJAKCYZRSIJLQLbKRCBQBYYIJBbDQaAABZKSLCBZSZYCTRCSJYSCIRbDRLRAKQaLBYbLYJQTZDTaCZASJDaZQKbDKSbRSYTIRYSBbZKSBKYCKRaQATAKYCZQaLbZYBTKAKZaTYbYBAaLARLKYTCaSTDSSABBLLaaJTKDQRDKAIAKLbRTZCBACAZKbYIRKKIZITCYAaJLaLDIATYBASaTADAZKbLKbDALBRTIKAaQBRSJYaBLDYDaYQbRZIKTDQBLZQLQIKJAKKRSbJaLTaYSJARTRQZAALJIQTCbAZKDCJbbbJZDYaSbQRRADAASQaTDbSJBTITbbJTSTKLATbTAAYJAZaKYRJYKQKQRZJRASJaKJQRYCAIRCIYJKbSbCLILTSKRILbTYIKQABLBZJAJaQYYITSTQSDKYKDDQAQabaKLQICBYDQKTCYYTJKSTLLYDRZZJaZRICDYaQTAITIIaTRLLaaYQKTYKIYYaCLYIKQILQDZKIbaQLACbAJTCbASbaZLKbACQLAYADKZLZJSDCBAYJYZTISJLZaQKYQAZbZDZAJRZSTACZBBKaJZKSCAARCbLSLBCDDSQbBbLDRTBZDQQSSTZDDYYKLZYYKAaDYQTLLQLSKIDBRKBLKLCJZQJADSYSDIQLbLKIKIZRLZYTJALIRCYBBCTICSLaCYAQJYTaTYYBbQSaQLbBTbbATJBTDBYJRaTRRLbCCSJDQbDaQSJDBYRRAIaDbAAZCZTLLRJBATIJYSQbSSQaKDAJRbCLQR"},
Embed{66,211,"YbADZSISICISYQTJJITIIKJLJABIBYKYQbKabaYaabDaabaaCaYaYCSZaSaZaCaCaJITJZAaBTKRDYISBZbTCRKQRBIaZDZKZCQCSIBbTBIRbDIaKQDZZIDYIKRQDJSKYRLKIBaSKCQIJDZDQbBLAKBCJRaDCBRSZLSJLaABSYJKRAbbKIBKRTZAASYDTRBSYbKAZbIaTBJYDATISKaRCbSKAaTKRZDJCBYQabbaBTIIKDCKBJZDJTTAZYKBSaJITCYSJBDAIIIBTBaaaRQBKLIIBDabRAKbSSSJAJIZYSRSaDBQBQRAYJJZYCCbQSKBIZRDKKbbILaTKATIIRDAILBTSYYSBaKBSJTYIZJKTZIIICLTTCRJYDbDaAZALTYSaTBaBAZSRILCKYYYaDCDIBCLQLQDLSbTDRRRBIaZDZARRYKRILAaTITDRRCJTBIBBbKYSRYRZQYTAYKKDKAJJCBSRABTRbbYTSaLLCKIaKYQQbJYRBRDDJYCCZRaCDDKDYZCCCTZILaZTYbAaRICCYaJKITTTLYZKYLaZSADLDAQYaYAZICYTYIIDCLDRLIDSCTRSYAJKTJATCLDLZKBITRJYDbILSQaaDLBJRRCbBZSRIRARaSYIKCYRARZKbZRBRDSLKKLBRISTKSTBBARKYTYIYQLZTbaJLAAIQIJAIYATDKZRQKaIYTDKSDQDZLQbCKJSRALbBQQJRZaAaTRaTabBbCbDIQAILKJBLCTSAbAbKaQQIZALCJIAIYDBLIQbBCCRaBJIKaKDYZBKYTSKLTRKCCZZCbZBSBZSIYACJATbZQLKaKABCTbDCZLYSZJTAZAAbYBTSJJYACKDKaSZTSDIRYQRRaYSKIKKARbKATJTLYYKZbQDASLTaJARTCSTYJDTLSAYRaATYLLLJSaSaBRKITbCaARQJTZKTaDaaJaIaSBZKRJBYCLTKKAMYaCZRLSCBZYZQYZLZSKQBbCQbKJQQIiEaCITDbaSILZBRBMDKTBCQBBBaJBIDaASQIYKSIZZZJRTSKLDTKKBRZLYCBAJYIJCQMQKbaTbIBZDKACABBKZBaZYSYTJTDZAIQCRaZQTZCARabSYJAIbDAibBDbQKLYALSbLKSLZQKbYaJJZKZKYRSARaKLSSYCACjIBDSMAJBQDbKSQZCRAIJaRZADTRKLZBaLSUKQQDaRTaCATDhBIaQILYLIYDYKcJiZLSBAbCLQJLQJALTITQMaRQLaJiYSCbLcIbSgJTQcZKBgYSCRJDKDQCTKKBBiJDLBbYCAJTLaRBBCESSbTiEIQaDBBJaYZICTBSLZZMDCgiKIIDRABAaIBQbYTDZEZYZbgDRRDSDSBAZbBZkIMLDTATBhYAQKQiaQLcUZLIBCZCaARDBLLUBBTMDaICKIbBJhKAYTEbAUaaTJSRabIbSZLYAKSDZjbIELUIIBCAhURAIKKTkaMSSZQZhgZCCSIBUjJaIgbZhSABJcZAJZJYQChBbgCALIDcbUSRTAIRSRJhSLgDSZDATAJDbaZaQDYQaMSLbQJkAhgUYJDiaaiILLTYgKIRYYDjAEcBhaRCYLkaiLRKSbJDJQajYaZRRbCgCbMZSMQaTYiLjbgMQQKEcLJBBQhRgKTSQDAMhCaMkbSQYDgIZAJDQJJbMZDAAQThTQkCZYQKJEQDhBbiBhQhKTBCSJcJTaDACUaADJTkkbYBaICKYKLaBjAjaLCCMTJgTKbhKMRRMETKIYUcjABjCDISAbMRggbYJKBjkEJcZcgcKQSCUcDZTgCCQSbcTaZhKgCJQEEbkAiJSjaIBDBiJKEbhSRaRYZKbhkYbRREBBagDRkMgZhYDKcYKQkBccZSRJiCJaiYJTRikYcABADSgjSgQZMYDbgLCjQkiKDLbhQaTIThLDUKcCjKYighKZQESKJcQkibaSMhjhcMaJACiEbjSBTMbCBjALLCIcCARYTcbijDBEgJSkZYAEUkkjgQJCISLJTjAZLkgLbCQikSkRbKkkUDKUBAQSLIjJjiTQjaBSYicMYRRhUbQMUgbRckYBQJBJZIMbYAhQQUDASkEMRUShMhDLBChIjUYMTijTKADLiaEIIiTkBJKgkJUckSBSIKKALcDbhZkTgUICESLgEKKiggccLJYiMCACLYiYQUibEMAZZKCiMhKMhQQIbchhiRaMDCcLbDKgcgSKEYhaMCABQSQJjMEjLRckkjLYEhZbTKRIjSBJCZCKhiZIRgECSAIYagYaAIJZcAQSYKjRgKAIRbbBjDLMKhcUICDQKihDKbYEhIYSaBIkCbAkYDgZZicbJABABRLZIMTICcURDRQEbIJUDEIbhLBSjjaSZJIbaQaSMgKAcKZSZJDIDRBQYMKgDaYiBKAKAKIIaiRkYSCAZRLAjjkJcTTDTIYZRIbgTJUZKRRCLRCBQYZLSEjLbTAAALDbiaRISABRQLigKKbIUDijKSZTaDhaZbcLDEjQCIiaDjCIJIbDAZCYURIjTIRARLLiQabbSaJkQSBYBLaQbCCbRQDLBRDcjQYISMQBhAgiacZUbDUMQTQaMbLJJQQCgIECBKBZDLQbJbaJKRYYZjiQSIQiLTZIYZiaQiKSITTCLCiiTbQCbJZSbYhCABLYMbLLaDBiTCaiBLbaaQJKAKCZASIYTLRSRKaCaJachZRaYQURRLgYbASLYIJJEbTBKDZYjCbaSQcaRSCCSABYDLLTbbKDbQRbTgCJJZIKDJEESTCQYZKSJDDLARARSLTCbZMiCKLTaIgEYKIbiKaaJTLcbQTDJZCaTBAQaRbDAZSCSJLYTQacQAQbKYLIIShDQLYbjbaBAZJSAcJBbQbaLSIBbRhCYhIYSiSZBaCLAaZCJTYIZAKSSZAZJQSKQCDbaaSZKaZULbZYSaSbQDaECQiZaSTJTaTDbRTDgQZJDDDaCCTYAIBQYAhDDAEbbaKQbjAAaBJYaIbajDYSIaSRSLSJDQZCLCLIRLJJLJYLjDSAaSTSJTRJgCRSLQEIYaQASRKTYBRTDYDRJAaAKjLZbJIZSSLbKEARJSCSCBbRIQQAYbCLcJIZQbbZJQDIKbZSQagaaTJTKKTAZCJLAjIQYACTBYaRJSRDCbAYQJLSTJSYKRALDZISRBUaKZLKLBJQCLaKgaRTaTAZCTRQSZLCBMDIChDJKDDZTRQaZTAKRTaCSbTJDBYBaIRTbbCSMQQBYJBJZaJLAAKTcQSQaADJaJLTTJaQiBSCTaYIaTRYaRJKJABQZaYQRTaLRQLJKSYKSDAZYRAZJLBYAQCYEZARYDLaYBCBZLRJLRYbIZYKRZIAKALYCbYTCBDBDbSDCDABDLZRKISCIJIDJCAUKJSaYQJZBJCCIgRbIRDaAYRTYLRJIZCYRRLUADCCYZITAKbYbBYYSQZKCTZCaISRbAIJaZQSILaYbILBLQQADbaMDbDBIIbEKCSDaAACRbaBYTaTjDQIbDRIaDKJaJCJLLABBRaBTTYSRJIARASJCaALDIRbaaRQaYbDITLSYSLATYJAMDAZSIDABRBSDZBLKBAbLSSJZCTYTDSBAYLjBYZKYILYSYQaDCTYKaCBCBLBaZJTZTaLIKBRYIJBCCJSgKbLRYBbYIJAjTDQYYLYbSYDKSAQaRTaDIbRYKZQJBQKKBKRbaKJSYaRIJBKbJQDDDLRCAAaIDCbIBYRISKRLUARTCbZYTYATAQRRJJaKKTKBLDiDIRTYCLIYKYIRYYJRBaITJBKMAKZaKASRQAbDLAYSBJBIcaCbSIJRLSCJYYaYKRQCTLBKJSSDbZBYAKRbSRKQBRSAbBZRbSTDJCQBIRLYaBaKADLAJQRILQJbBRaLRLBJKBAACDDCKDTJTTDDLBQbbLDLbSIBBaALACBQITIARSbQAaZJRDDDDbBBSDTTIIYDLKKJTSJiLCDYCQJTCLTABQaTCaZaQQKZZcDCKSJSCDIYLbLSDbZALAALABLaBZTBSaKSRYUKSDZBLZITQBRKRDIABbJZRbRJQTZQRASCQRLIZDKYRRJADLTZbCJDTSLSRaDYCQTbSCabASQKTLIJZaYSKLDTTKYaDLIIKAIBaIISQQbZQJZYSQJADCYAYBKTKbTTaLaJBQJBZQASaCSTLYLIYZYKAQJDLZCaJaLYIDKALBaaZALCaQIRCQRRZLTKLIaKaaDaKaaJRBBRSQDCbCILLJaTBZCRaYLBIIQBASBLKCJabhDJbSRKYAZQYLLRQSCSDbJZZAYRYYIIZCSSbDTDAbaTQRaDABKShIYZKJARYBDJKAbYKIZbQDKCJCYbBKLTQKZQJJCaQbBDRIZSCTTLKSZgQDKDaJRILSBIaCZLbCAKIDQLCRTRZYLZTQSRJALYbQBRRRZKCQKCKaLLQYCLZQKTLCBCCJSQKZTBLZaSaDAJLTKLCDITCLBQaaKYaaQYDKTBYIDZCQSRISSbJTYLQJIaRIDbYCQCTTAbSKQbTDRIKRIZACDLZLRLbCBKJYSSbKaKBIYbTQDaKbTZTYSLLTTKAAKIQTaIRZIbQATcYDDaRQbIDKQZaATJIQKABYYIaITZTZRTDTQJZDAYBDRQARSBCbLQJKKbCKBSLDQBZADZLJLLCYYQSCDYSBBQCYKDBJZCBZbZIYBQBQBCYDDRRTbTRDYDCBBZBATJSALAYQJAZSIZJAIJRLCDAAIYaDbZLYRLDTCCYZDCYCDaZbRTAZLRTKbAbBBAbbKKICaDbJZDICAJbQJSCKLSDYJTCbSIaLAIZDAICQKCBYZTQLRYQYLIDLZYKBaBRLAYDDAZaCaKLbQSAYIZDBTbbJYQYCKaZYaQRTZRKZJICDDSCJKLBDBZBRCLLbYZSASAaZYSTDZZICDRCQQRKALZABSSQZISQJaZJTSJLIYSRICJTRTKCIKJYCZbbLKDKDbRJIRTCCKAKaCLQbJJIKLTDRSTBISDQKAIIYZRISCJZZBZSaSAZATLLBKYDaAYRCYKYDYQLbCITBATCQIBbYCJSTBbJbKKSYIaZQJRbZBAYTISZbDKJCaZILYSITQKTKKSSCKDSBBAIJLAKKYTCASJDSaTCZRZBLZZTDSBADaaRAJCZAJSQbaTaTaICQSQQAYbDIZDBBIIZJYZabCJQYATSQBRZCCKYLLaBSSRZZTbSDJBKJKLRAIKKQTCRLIRRACJZSYZDBSJQZaIRSYbILQTYZBBAIaJYIZSTIYCbDBbYDDBQBSbSIbCCaILYQZAaRTCZSTZRTQSJZBQKDbQbLbTISKIJAbATZCBRQaSLbARBaSZbTTSIYaABTSTKCbaZZLKTLZDSLAKLYaAQKKADCaaKbQBYTYDBZTSYDRBZAQJaZSabIASBaBKRIBBCAIDSZDSZBLQLLQKJKbTBQbTBYDZQTYZaYLZaKATRRZIDSTYZSQAZSAbbIKaAaBTRCBSISBZJQKbQRDSADIbSaJBZaZYCLYKCJKZJIBIRCCLSJIJKAQYDYIKSBKLYKRBRJISBTabSCJbbQYLLJRBSZSQCTLICYYIaDZTRZbQISRbKTTSJQLLabSQJBCSIRCCSbJZRJDQYKbJAaBBCQbILTQYZCLIJCICJAADZDLKRYRJRJQRIZSSBTSRKABKTLAJYTTbQKRTITJDSLJKIAJKQDBBIQJKLaaYYTAAQBKYCZBaCYaKZJJCCRYIRYSRJQBaQBJDTDQaZZQQSKLSIQabYLQCSSYJLLBBLaRTQSRbQCIJbCYDLLJIBaCbSRICTbYASIDbDLSYLITTJKTTISYBJYaBDIYADJSIZAABKLIAQbQAQQAZIIDAKJZaDITZTZRCYSJQKRbLQLRYZSISSAAbbRAbAZYaZASQQCYIRYJbZZBDJLLKZDLaADISLSJJCCAIARYQQTACbRaAJSbYRDLBZBTACTBKYIJDQZKZCCADRKaBIZIKJBYYJTTRILAQbbCYIIaCZTCCYTDbZIAJSCTJbJCKBBSbIIbDRQZLRZSYAAQQCJZDYRaYJaYABSDTKRaLTYCRDTaBbRRRTQDIYALRbQAbaKDIZIBBBTRTSATJCDaRKTRZLTABYbbARACQRQaZJTQbKIQKTZZKAAYJaDCJDSbDZIIISDKSCLYDQYKAYCZDBZLKTKSTaIBYbTRSbYTRRADBSBSBZJYJRAKLCTaYALIIDRLQSYYKDZQLLRDSBJYJABZBYQJATKaSbDCBBCSBRaDRRKJTaQYLIBLBKDLbQZIIIJDLZKKZbQSCbQQRIDCbQASYaLQbBLTYSTKaIbCDYLbSQYDYQbBDaKTBLDLYDQRDRZBTCbCQaILCDaLQJaTTABCKBLCbDJSBKDSZCRZCZIZTJTbSSASDRKZDRDLDJJBDISaJASbKCRQLLCbIAbRQKLSKLAQJLDTRCCBLaDbYACTLKBIQDDbTZYTYTRAAabRYCYJZSKaQCTLJDTRaDLDDASJYKTbIbTKRYYAbDSRAaKSTCKbZSRQCYLALIIDBRZDYbILZSYYCTIQZSaJSbCbAQBTDZCJaJKRKYTDILKRBKDKQCAZQIJISCJbaYYBDJZYYDaQJIRTSYKIAKbKKCIAQLQYYaJARSaDTJaSCbZZBbaSZRJTQQQCBIZATSZADZBLJJIYTbTKKQDCJZRbYDaLaSQDLLAIQSLDADaBZKYaLbSACCQDLBQBLBLJZBIZYYADBAAARZaBIQSYbbDATIIbaRbBaLCRLCaDAYLQTBBDCRJYICBDKaJIbCbAbIYCQbBJRRZQbaRYaLRKDDAQRYLBKYAARYbSAbYbBLQTQTbTZZaJaAaCBDKRKRYCQSTDSQIRSaZIbbDIYZJTLQZBIJQaTDADbBCLAIRCITDCbLRBLJJBDLYQDSYCRLbaIYKDKSIbQAYZSJBZSLIbJRSLAaYQYIBAZKaLQbLCIZabCJbKIDDZSTYSICQTQALKRYJQQKRSDTKKKDJTbCLQIBTRAbQYJbSbIbBLIZaJRJADDJYZICbQSTSKSDACCSa"},
Embed{68,217,"IYAAKIILYIIKIIZIJLYQQIYIIIIIIZIYQIBZYYKKZaaaQKaSaZaCaDaCCbaaYCZSZaRbYKBaJSDSLRCYLYBTYDZRKSYJYDAaZJRDSSaTIBaSBIIaDRRKDYbARKAISBZTaJTDKYKLLBTJSabATBYJQJAKaTDQYSYLDJTBACTSLbYLBbTBKAKJbQATRIBRKCbTKbCQKRYTbARKKYSCBZIJDCbDLRbYDZIQKSCaRaCBKIBAaZTRKIRJaSQLTRbaITCRYKABAaJIIbTYCDJQDCaTYRYTZZIbKCLBZbLSDQIJCSaaLQQQBAZKTTJDaRBaBKRASCJBDJQIaZSYYTCCDDbJYRbQTYLABaBILaDbIKATTJSTBKTZJTYCCQKKIYYDBQRIIKAZZbbIDYLATaRBaCSRTBJSJDLALQTTCALTaYDQBZZBICabTATKBTQYJBaABTCDTCDLQTDKaRBYCTZZZYZLYKIBDQCTIBQDbaRASSbTBAKLJLITCIDRaADDQJBLBDaaALJCbKKSIDZLATTYAaTKKIaICQBYYRZYRTaRCCLBSIaZaLDbZAaZaQIBRYSLKZABQAKbRYYKaCBLaBTQTABbAZBIAAZBTaZSYYbKKKaBKBJQCbTaBTYDQZRSDZaLYBRbIaDaTAaZZKZYIDTAIQRQaKYBDaBCRbTCYbRZKQTIYSaKKRCDBKYQJYQISKJLbYZRbZaKaIaSIZIZSLQLDZCBASTSZTZaKKTBQBIYBJaDJRLCYCYCSZaSTaCBCaZCCZCZZJTKQTLLaLIRRZKDabSAaJYaKaQYRRLBaDZTBBZCBBQYQLKIaRYRBIaQbTLbISRaBKJRAaAQLDSACDbRRKYIDJIaRBZIKZDSZBQBKbAICZTIJADTRSaTKDQIJJZDAQCZbRJSITTbTBITCTaJJIDKSTYTYIYTQCBBZaBJYaJBaCTSDDILSSTKBbCYSSJKaJIAbYBBaKRRJSTALYBZaADSIDSDDYABRBIBDIZYQAbSAIZbILIbDLRCQTDIAJAKaTKYYZaJSDZLAYBDAQZTSIBLTSALLCRRDLIRBDaLTQIJSRKAQZTJBbSCQLTaCTLLbDYJZBBaASJbQKTYZZSSTaIBQIDKDLahISJDLASALjQJCKRQTaSLIILDQRSTSTbYabZTBRTbbaJQQSZYJRaRZbKALKKASZYaRBBLBABQQIKaLADSCDLZbKZKRTBTKRBRTbSbBQTcYCIBIaKATCAaaQRZDaTKKYZaYaTQCKcZbJTAbMIRTaaZYLISRYBDAaZQaABZQZaLLICTAIcCZKRhDIaTSBSCZabISDaSLARQBCKRLDDJKZDCCbTSYLTaJZYSYIYJBBLKIDJRUQIBBDITIbJJbRbYBABCSaTDCIIBCYIBSaKgQYLbUbKAJjYYLZJbCAbCSIQQbaiKIhLAZBSMAChZCJTUDTCZTIJRBBACLTZDaCKYYiEaYcKZSYAZBSLYYAaZTjQZBYAZCTTAcBKBcbSZCRcUYQLZBAYYCKBhBAaBibTUITbKbcCRRJIiEiQaCbSEQDZBDLZBDQUSBbBbASBCIKULLjRcUhLICSDTATiKBTLgQYbRbKAcJQDTYJaQELAJKZAQLSjQJbYEJRjAESMhAjQIbMYKaKaJAbbQRZbKRJDhKDYQSgZhbQDhCMBKAEDIKKCDSDLgDQTEiJJDLZQCYEIIhLLhZRkAQATSQaZUKaRbgghbTKIJBSRUkCcMRQIhaQcLKCCaLMBjDgEaYbKYEKSDaaADgJMRELISLMATbQZaDJEDIBCZgJiLDhSJMISMJaZKAiRMQSKTIKETTJZYCTBTbLSjhUUUZUUAbgTCZBkggIYRQLJLCcSDjbBMcAUBhAALbUgQDKjgERLbaCjCIUJBiEhiYAhAAbYUMLbKSDIUkBMTYJjDURZRLiQJYcicDKhYUiZBIcQDbUScBjBkQbhaADZjCBCLcjLMcCAkRLEhAJCIcaShTgMDYcSTQCgEbRBQhjkQacTBKZhZTSLDaISJaZBaTMSRRkcIbgJjBLhcakAZKASQSkRgUEgbLDJCcTAcbQCIIRgAUghiTLDYikMjkTaMILCJggIUhSJhShaLchQhJkDAkCcRAMaTCLbQQKLTaCcAQCkbhBkiLLkkkjCKTYjjJTITZhhQiZBDRcYDBjMJicBUjKIQMIggUCKZCBbDMjaTIkRgDcZTkICIiggTRDDQMYjQhMQQijcKbEJkhSTbCCihJYLTCgMgkUMAckDBTkQDDZbBcQcCiIacRghDkTjaMRZcRRkSIaUTAYUYEiKaYiLCDDDcBCRQMMUMhIghDLTLQCBCCRMcRRMMZYBBhbSjAakUcSiDCTJaLgSJCibAIYQkCLMJajRhbKaciBTSZiBBbSCBSQTDZSIaAZYAKLZkIShkUJiZRcMEYKhJbACKYIZSZZLDgEURDJTZQDibQRRLgRLRQMRaChYZbLARRJCYIIQQkaSQYLJYkQkBjZRaIAKZDMYTShMZLQYKaZKEEBIIQiijBbCQAaDBALkCJJBLaAYbgciDaDUCDZCQDbJRahYEiKAYAYBTLAAZUAgZLAbTLDCYLIJKRMTCQTcZCCZbZJIAISgLBCkYLaJBTDaSAJLRAaCBIBBbMgAAKUDQADDIELILJJRTSYIJZBaDBYgaBQTaSAZRSLLjJaIacagECZTRSAZCYYLSkZSSIbISZZaBZSMRgUBRaJQSJICJLjCLRAAkRbbLBJKIYiIATcLaTRDMYKDTQQSKahBAUUKRLRDaREYLYaZbYCYJQhaDAiISYaKDEDbZTAcKLKZRTJKSBDCbjSJSAZCCASSYkTAbYLBBJCLDDEaIJTTLZQSRABRMLAYaADCSTKKZAMRBaCaJAUSJZKLKYJSibTbaDIBKQSBBRJAALJMYASCADSLZabDTIZbaYaEUBDJBKKJIQRZASDaBaRKRTRMSIKICIKAJQbKYRBIJKBSQKYTQhJLQQCaZIaaRSTBIabQCEJEbbMbSbLJbIaSBDKDRIbYCIDKSSZKThRAQAZQRjaSTTRKIZSJCbJCJJICbhJCZRYDQCSKaQbYQTYLDBIgCgKZYbBQBaYabDKZDTAYJBQSZQJQDSRBISbZLSLRYIgDAIMQEKZCBLSJKTZbLDLDaSZCKjDcCjIaSBIZRTYIBZScRTaICZBQCDJLCQTaIKBaDSCBYLSRBTBJIbbJARaRRTZIRJALZTJDACiDaTLgAbaAIKCZRZSTRQTRbIZQBDYQZJJJYSIQBIQBKSKQbAaJbCCZKCAQKAaJTLZTQCIKTIJYYJCKUTYabDKTcaKQbJITYBQYALRaQDDJQQKkILaLRKBLaDRABDDTYbSKKJgKgJZJJSYUIZCSIIDSZQhTRCbAZIJIIMKQDQJBZQCRRJTLJJJJASYSRbCgJQLSLZRgESYIZSAaKYYCZKZZIKYTTRTbAKaYBJDSaYLDLICBJQTYBBACYaCKILAJIJSQSAIJRjBYBISCKKAYTDDLTSKRAbTKJAIbCbUAbSSAKcTAYZYZLCaTaTKCRbMiZbQDjAJRTITYAYIYIbJRJbAaIDAThZbLDAaCZgAJTaCTJTYbCIKZaABDRSLMCSTBSbQTCJIYLQIJDCICbIYQbBSDTbLQZAIDBATTbIDQZbCUQKLBIZbKQDIDbaLTSKARLCYSCBECQZLIBYJRCKbYDSSADTCSDJBZJSSATADKIZQJKZaBZbaCCRACQRbSTABQAZbCTbIbJZDKKQYBLADLDYTDZASLZISDbQJYQaCLIDALISBBDbTYbQJSYQYTTJRaRBJRKCZCZTKSYDbZSTKIIaSIYIBAaTLbbBYSZJKURbRZKabCASDCTYSSSZCSYbZaIBRbaQLRLBCaKbRISQAAKBZTbDQTASSZALJTLDJbaCCDZICYKQDZYZASaABLSBZQDIBJTJLLCKQAKATQYaZDbDLBJgYbBBQRaCTLQJBRbDBQJbaSBBYbAJTIQCQJDQZaLALALSQaDbLZKbKcRSACRASKASRZRRZBLRDJBYYZCJDJYSbZRYSYCbSLLTTASLADSkJTKIIDRZLBSSIRCSKTIBCDQTAQbZbLDZbaLTYaaCLJaQbQZKJCaZAJKRJDBDCZCRLSDZYSIACQaTZQbIbDTYRaYTAQCIJaAZTYbYTSDCLaQQALIZBLRSaTRLRJYDLaAATIDbBYCbDYDASKBKbRCZTBCYLIbKaTTSJJCTCZYIaDCIYABCTBDRIRCJAZSLKSYIZLQZBJSKiaLaSJQZQAJYSSaSKSbKAKTCJIbLYQIIABBDJYTLTKDYKKbJLLhITJBTKCIQYATYRSBTBZYQQQbDYIbRZKQaQbbDAJYbZBAbACTCSTaQLSBBaIBTbATYRATSRLCJaTDYRTLRBDZDaZBAZIQATARCRZSZaASbCBYJTJRBaKCITDBLabABKYaSZBYaCDDBJRCAZSIZDLZIRBSDDbYKYZDCISLATgJJQRLSDZBDSATaSLQLCTLYTAbKKLYCBKLABDYLYBDYDBaZZAIBCCbCTJAQBJCIDATDJTSILQbbRDCBZaIQTKYRaARAIBKZJLSaDDIKQTJABJDKDIRLLSCbbBDAZKBKbQCLLYYYQBQARILARDbSaJDYKRbbJTTQBKSZIZDSKbCSTYaBSJbTQCbbRaYZZIITRbBTATbTTQJLYQAIARKKaAJbTRJTIQDYRYRSRBQKAbLKaRTTKbaCbSIQYQQJKDCBRTLBRaCBbKZaDYCALCBJKLRYIIBBbKDARQJbCSCbRJCCYKJIIBICCCIKaabbaIbQRbQLbTJYSLACALZSaSYKKLCAYLTTILTZAaZRAaCRJZbAQYSAKLJKIRJBCSTBLLKSBCJBSaIJIYTAJBbbSCBKIAJbCDaDJTJYBJRJQZCSaRIRJbCILSACYAYYSbLbYSZRKJDaYbASSQIILSKBZBaIIaYSTYZaDJRSKDKBaCLLZQKKCJSDaTSJDBQYTbQJJBARKYKQIARKRBDCbKbYbBTAbZCTKRQDbaCLBDDaCAJKRYTQbCDRCQCCLCKZJQJLLZaZbbbKKaZQQTLSBSbRbAaJTYRJSAQQLTQYCSCAQRaCJTaRTKaKTIZBRDSbBLLRTRRbbDaCQQCQLKCJLLJLLDJJQQCLaBBbAbLDLDAQAbALQaSQKaZLJKaDIQJTZaRCQKTILDCZIbALIbSaZJADQSRKaDILbCQTQRLAATbZYYRZBYIICLISaRRITADKLbKLLaATBSMLTKZQCDRQZTQLDCaADaYaJAaaIBIZJKRTYCALYbYJSJLCYBQDIbAJBCZCSAJDAALYZZYabALCLARKbDRDRIKJAQKbAYALKQZYSCBJTTbKaITSbYCQYTYTZDILKTBBATbJLLDQbDITDCYDJKALIATTAZZCbBYISJIJCRTKZKZAQAbTZSQALCQabbADIQLaTJKRJbCabBRAAaLABCIDTJJITLLQTSJKJADSKTQLZDSJaYAbaLaSSCRBJYACBAZDCQaJZRJRbAaICJAaDBBSYaZJYTJCTBALAYSTTDbSICRTDYTLAQZSYYIZTJLYbJBJaZLRSCRbDbBSDKaDTZAKZRTITBQZASIbIKAYbbbRQSSIQTBCZTRJKYSCTYYIQDbYbKBSCZaCZKbKbASDCSTIDRBZZYRCJbDASIYYbZaRaZRJRTQZQTKACQYKRbDCKTSCBJZDQaJYaYSKBQIDKYAQQCKZQZZAYbZKTZZRLCBQKZDQSLSRDJDBCIaKZZASYRRILRCJbBIaAJJKTBRQBbYYLZaaLaTTCDYQABJAQSDTKYLYbSLbZQSTQLZIKaQZCLBQabQSSKaTLDIRCJSZIIBYYYDQQaYZaAZKKaALDDSDJRLBaSIKYDQIILBSSKICLLaaQQDDYKAJbbZQTZRCaZZKSYALbIbbZTLABKQSDIDJQTIRQKASBBRaTaDJRIDALQCJJJAQADbRKRRKQBBYTSbKRJbTTaAKabBRRKBRAYCbKTZRAZJCIJSLRQSCZCbQDSAaKITZAaCIBJbLLATRSKIbRCBbYKKJbBJDLKCZSCJaADABAbADSQZQLLJLSYDaJQYKTBBQYaQJBDTbKKSYAJLISYaIQbJYSBDJJCDbZCIQYCSRTaAZTIBTJJBQbSBYYZQaARBJARIQSSCZLQLbZbZLYSaYRRKTDSaBTAZZAATRSCKSQIZSbTSQRIBJaZKBQLLRKLbCITIBJZYCCLbbARQKIKZTAKKZaBYKRRAbRaRDQTBYTYTBRSBIbKCZbaCYYZDaZIAQaLQKDbQBbbRTZLYLQAJADRIISTSKTaIBKCRSZQQRICKIDaZBJYLADJKCbIKTAISaBaZbKBKQDZTaZaaZIaCLbILCKCBKZYLCbAJSRQRASKKLRAQBTbLBaKSKRQIIZDDITIBLSCJBQJSIDaASYKQZTYIJBKAbbCTITCYDBDYDJZabRSYQYbZSILJBKCCJbYDSBILITLKaQZDIISDIDRKCQJaIIJSRQbaTDBRBQJKBbbRYJTABQIAbBTLQIIAQaJTBQITbDKBbbSJKAYYbDaIQbILZYBAaJQZSSIBIIISbabaBTSJCYRbIDJQIKKSQQJKDBDITCKASZIDaZSLKJTbLRIJaYKSDCLSaCAbBZTBTJbIIJJBCZYLQRACLCJKRDaIIKSIDQbKaBbTQIRCALJJKYIaSYKSCZRAKJRSIYTJaSRJTRBLCbIIQZRZSQKKCZDaDTSBTbLRKbBDRTABQZbSQaIBSBLLRJBCKCTSKZAbRSYDYJJRRCBACZLYTSDYaJRTJDQaIRRAaBISZTKQJRbLBRJDCSaCAbYbaZRLJITZTRZCYLIDCYIRDRDTaTBDBDRAZQLTQCYTTLSbDRCTAJZYJbJIRQSTCKRZISbTQTSYBJRATaLLJKISQYZZIKZBTJQZABYQQIJCQSCaYKAKQACTbQaSILBbDZTJJZSbRIYATZDKSAbDQbaLbYZBSCSSTITIJKJbRLCCBDBaRZYKAKaIZYLIQDBBKbBSTDTSTQKaAbDAJALIIQRbYQKRBCKZLSCbSDLBZSbbBaSJaYaSTBSLKZQbYKAZKRAZaCAYILQRKYYCLbbALACDSaYICCaKaDbBRIDZBCLBbQKLKBZARKSRaRZAKDIKLSKRYaaCCICSBCJbSbYAZTJKKbLaJBTaQSAKDDRAbSbbQSKRRbBQTRabTRYZDRYYJKICITKYJZIRZLYYAJAARaBYKABRCTYYKRBAYKDBbAaDCQJIbJIRaJaKaSRTDCJCKKAaZZAIRBYDBALbbDIDRaIYIaCQADDDYDJBaCJLbIYICIaJTRTRSBSSCZQSJaTBbJZZQTbZITRYSbaQTBYKRJDBKAACJRDDBQLBbCCaQaDQYbJJYQbZbaYQSAaISQCBYRKRYDJJbL"},
Embed{70,224,"IQIYIIJQQLKQIKIIBILIIIIYIYIJIIIIIILCKYbaKZaZaKYRCaYaaCaZZYTSaaCaaJSKYaQTAaAabSbRTYKZBTQTZKKSZbLTAaBKRTITBaTIBbTIBaDBJSRLDKYRLYBTKABaSQQDRITDCIJTKIBCDARCITBQLTYZaJLISQLTIRbDCRZIBTTYaSACDIRJBbRSJBSbDbARKIbTAZAbKBYCSBKBDKLQIbQTaKIYAIbTAbaDQJDYDZRJBSDTJBYJYIYSDDIaBJQDbILBIASbZRCSSTbRaRTKQCaIZDSDbaQLYKTABSLJbAKIKTBBSaZJYLBaRSaBRRDbAIbbDTQKAaTBIQBTKbLQBSIDZIRRJCTDaBDDBSABZYQaDTabaRSKZJZQDbBaRIYLabIAAZKTCSDRZCYILBSCLQbaIKLCITZRDTTBJSKCLaabaAaRASLZRYYTBQSaDKKaKBCbITYKTDCTbIYQJJKRJALQLBRRTYCLACYaDTTLCKYKLTKJRAbKBYSZBYBDaRJSTIYaSKKJRBbDABRIICaBJSAJYZbQKLQBISSLLRaQbDaTCZQLRLRQSbQZBAbKbRBQBAZKaJaCCbBQQZCKQDLYRJRCCKRJLTDRZTAaLCYRIKDaLaIYbYDaQZbTZbLTAJRKBBKQACQTSTaAKYSZbSJCIIDIDDSRLbRaSDKLJYbQRBaASZIJAJBSKJILTZZJCILRDBbSIDKRKRADYAJICIAQAbSQaaKBIaLCCKYLRBDKZLRDQRICYTLTbALKATIaQbQILDbRTLTATACCQDZLBDJRYLLKCRDAYLJaLIAKIJSDZQABLZLQLSKBSTIabQAbSJSTZQBbSQKJLaQJBZRBQYDSIaIaSLTLYIbBAAQaTAYaARKCZbZYDQCRAKBTLbJZSYTBLLKSSTCSCTBbabIRJJRDTSDRAIAAKZQKaZSLIJIZYYaBZBCRDJIRTIRbKBDZSYZZKYJCAQLRTSQKIIIJZKYLYZAaKDKKDYYCCaBZZKQCQSYCTLRSCDTbaYSJJBaYIIRIAQJBbZAaZTDKIaRYCTSRZbSZBDKTAbYaJDDaISBLKQBQIRiCQbaTDaRKYRSDBbJKQbTaYLBTSaIBYTSaYEDZaYATLBDDZTKDRabaACQTKRLCCZTLIQCaJCDaQJJIBZSaKaQKKaDQbCDBBSbaRQZITLQLLLCJTQYBaZiLRDIBLAbKQJQRALLaTDAhKQbBYTBBRaIJQRYSJRSADaAIDALJTbYTYKabAAZURLhBIDCRLQKJDQKbcIBcRZaCRRCAIKJCKTYJTRaIbaKZRAZTICCAaYCLKKLTYbYTRIKRabQACYSabTLARJRZgKACCILJSYbSCDDDAThLJRALLIQCCRKZIDjRIYCDZYaBJQZBBDSTBgTTaZLIKICbLJUSaKCRaZSDgTTTRTCJiQYZYDaBRIIIADjYibLaQZSBZTIZBKAIISaCYShaZRDBSDDRYBCSYDTSCBIUUZIjZjKDYABKZhJbTBRYTbiRIiAZRSRBYJZaSbEBLZZkACAaaLDQKQIgZKaTQaZCCbZBCKCDkabKYIhSZZKZBYAERBcYJCABIIaJCBRIYiQJLcaLCiSZSibLaCZajQCcRBSDaTBgbQZRIcCCKbUKJiDRKbcYIBULAjLZDBbJQUQScTSLAZSDRLiScZBSEBjYBIKBgLJZgSIZQLZARIkEATCITjDiLKAhCBTDKMSgbhCBCbREJgTSDTRhYIiMgCTCcgbbRbIZZaAhJCLLJabBgMUJQDcEaTAjYAcCBajYLSEQJYKEKATZabaCiUSZZTRYQbDLhEBkJYAMgMETaBZAJaaaiKRKRBBYRgMUQgLTMJYiJEgTRLQZLBAcRaRBEUMZaigTbSiKBRIaRUjCMQQZaLiAiRCLiJTZRBDMZSigJCRMahUUCEjDTQJSLJbZgAEEiKCRSYhZZSCUiDcCMaiYBJIBEIbJiKQDSELciIYJJYLJUSjMiCDQMjBETBMCahLcjSMRKiKZBIYAULcUTjYULIkBbgQDESQCZYLSDKJLjhTkAChDEJJYCSicQgJgaTELikMRiKbQkAZjBCDRZQLiLMLEECaJIiEDULcchMMSYMBZaACDbbgCUbTcMhjZjgKEKQMERKQcZahSZkhZhRcEBJSCCBhUaDhYEicJLMiMDCagQBCSICLabEDYDZBkIJDDDCRiAcEkMhbaTciiMJLTkMLIRIaMJSAAKCkbJAJaDCCkcERhbaQhakTRAjQYQjSaMkZkJbQZgMQRaMZKaYBiQSICJRccaChQBgjQgRaYQJCjERcDZhhYgIIkRKAKagbRgCaTkcjgEkhEhSJJLhKEJbBLBCAhCjZQCaIJTJLMCMYLUhIDAAUQDQkZjQkjhBkEAbDAULiUDIbJSZkMYZRjbcMccYCRQhigCTEaThjYjbQkSJSIjkCTgTShKMCDbDAUMCkRjbghLgIJAJkgRYaSLTaBbaaKQQMhkDETYMRYcaBZKESUEYabahQLBLIZgUBgLjCSJKJgRbQMQcbTSTJgKbEIcTSYYLJUEDiaSJIIBikacRkgcgajASLQATgKJiRKSQQaJaDZJKMCkLJZBJKQDYJBgCRbZEKKhIibCDaJCLTBZYRcIRSZUJIRQaKBDkATLjKTBabYIRTAZJJaSAkZiQbTDCZjEYbbiQKCCLcYALBRSDURbTIZkhZiTEMIJYYRBiUgSEQBCTCZbDaBRbYkIkDZILYTTjaCaILbLbQKYjIiRLcUScDRISUJCcASDCTBCUYJCZRZIIJjJAJcTSRTYIQLQBSYAiZAaDSJbcDcUSEcjAJaiIIQDSUBkQYYiKJKRDZAbLQbKSQLBTIhZKKBYaUYjbDiZKiDJKDISARKhRYYYbKICSDRCRRIaLBSKJQhSYBZCgkZDJEbcAZUQIRLZKCAECbDjBQBBZbBDTYSSCIAbjILTJKDUQDJCbSZbBUTBTCKCcILBBRaMgCbTSBRgLAiBabhDKJKKAiJBBKDLAbUDIjaEAQJTQAZbAacRLThcBZLBaRabLSTbJCCQIQASRcBIRRTbRDDKAKBKgZQZBLbCZUTDQhIjSbTSbISYbbhJUDQRAUJbJbBSICCAABBIYCZZRSKiDAICLaQBISBUacAbQCbIIIgDThRJZCDDiDKYJQSICDDCCZaRZbbRYQIAJJaSQZaTLARDQTLZQQQQDLThJQYbLQDDDYTaCDZQYLKaJZYEBIQaQKiaBZRLQZSRRSBDLLZDDKDLSTTRABRACiBIDLLSQSSRJDBTTbLaIbBaIDAREZAaCYhjRSaKBaCYYTCJSjYjLjLBAQJaRYRYSDRMYbYDQASLagBBTKDbCJIIZRBQCSTBbKLbaQAAaDbILBSSZYQIIcKbCKIJARaRABRRLbIAbDJQDBIZIZjEJQISTBJAAQaZbQBbKSTTTSaLSAYZQCbCKYZBYRZZLKQQCCLKSgaDSSDIZiDRZTZTQABJYJLhCSSCZTSTAJiYLIcIbDZYSSDKaRIDZAKZSBbSISJJBCbQAJYAJQCUAbDRaYTIJbITZJQSbMCaDLZICKSKTJaDabYbZDCaaZAKESJbISRLbBRCDSRIAQARIRKKBYJDJIRTKAKTCKbLIERRJLKIQTARSbBTSRZIJaBCYYDIJZAQTQIJQYaSjbCTaYbRDbLZASKBTCSaQZJJMIKLAQMQUTABaYTJjKaUTiYYbCbRJRAZBELbaSTgbYQAYbZQATBaTZSZJTBBSKDZDAZKbBRQATSLZDKTZLDQaIBRLQaQSgATSbJTLaBaIIaIBRbbDZaCTYTSQaZYIDaCYAbJSRAbJSTDDDYaQJSLYARBYIaTBDJZTRTCYLCRBBSJKSDJZRBABRDLSQUBYULbATZJDDKALJLIbYSIDSaJaCaYYTCQSYSJCATYbAJbZQbDYLAYaDiJLJSKJAEYCJJTSLYaDJZAAajRQSIADaYJJCCLCaCTIICYBKJQZRRRAZIaQAKJDbBJDYSRYYISKCYAKADbKTZRBKTLbMRQICRTKbKJTZSaLDYCJZCRJLaIBLTAIBAZDAQBJDIIIZAJBBATYALaATBRiDBZBDDTCSCZTRZATISZbIIKhYgLZLabLTDQbDQJQYaMSISYYDLRLTBLSBTDBLKAARLCZAYASRIYbRbIDJDBYabaTQaSQRDBKQTLCBKICCQRBLTKRabICJABTLAKSLLZTLSJBTJCDbAKYQbIBBJCSaRTBYYJCaJDYJLDRCQRaQAIJJCJJICAAYAaQJRZJTShYCQCKLLJBbLBaCAATQRRLKTYKJIADJQZZJaSJYaIYTIJISABBbJKASLQbLJZAZBZJSYCBbSYaKIAAbLSQZQARSAARTIYSITRCIJTbKJCIZLKJZBKDDQCbbSbKaRIBRiALCabIYZQiKIBYJKTCbIaBSBKDAaYTQBKBYaDTAbYZLYYCQIIgZBZYSTJLLKACZBaQLaBJZISbQaJRLIbCJACbIABYLAAIKRSIRJYSKKIAKRYZASaBbZECTTTZSJbTISAJbQTDIDSZQJZCSCJLJbbSJJaYRkAYaRAYDEBCILSDbAbBIYKCCSBbDJSKLbbSCKTLILKKgZRJbTYaZBSIJSbZSAaLRCREYCQYLBITSTBZKYiKBaKbIJILCCDQDbKQLRaBYAZDYDJRRQCTDKQLBSSTIDaZDKaDDDTIBLZCRJDBDZYAKITDQbQRKKACKKabiBRKKaZQJKIKLYaTZDCRbCRaJSbSLTYKaTCDJBQbYSgDAQCLCTALbZJZJLKYaAaTbaabQDQYLTaaaBYCIJTKbaCZCCaCYIAaZZIIYSDKLBJbKTDQBBBTQRBLAIIRIAYRIRRCAYRZBKRTQQBLJBTJDKTZTZIaBKTKaKALRACCZBZATACaJbSIKYTTZJRAbSAaZbaTZTLaQbBYLJRRJRASQQQQCZRCLJaYZBCQTDLRJCEBbLKZbDISTRaTSJaLQCAZKCTDIBTZSTABSKbIJaYATITbRICBIQISbKjJAIKQLLAYbJjSLLRbDAITLARZQRQDCDRQbRZLAaYIZZCJYZaSDQCAabQIJLADaTKBaSBbaYLBQLQAaZDIbYZTJRAAILTLbCYZbYZJKISLDCCDYKIbQQSIALBDJJBaDIZIDDIDaKQRRDKDBIZKYQRTKKDARBJKZBTSbJZICKDDYBDLSiKTRSBAJZAAIKCKaBTaYRQDRIKaZJbCLDSYTDJIRLSCZCZRTDZCTAQaaLTDhaSIQDYJTYbQZIRJQDCDIDLaQBDbQLJKRDZTBCJIKQYQLbbQKSTRTbYaKCIIRCTBBCLZDaJTJQQbSILaKYILIABYJRKbASbQSLQRQASLRZKRILKLKaQRZBJQSITLIQZAJabIADbabRaSBBLQZRKBABIJTKTKQDKZQCTKKKRQDaLQJbDRJBbCBSabCSaAQRTKBbDbSKRJJYRYJZRZRTRRISaKTJATCASAaZaaAKIBQZKKAKYTCTCRJDDKJZBSBCILKLSYTYbBbRYJRYTJQCSLaQLDKbQDYLCBAYRaKSaBZbQCQZZJQADADZKQZYRDZAIaSRZbbRQbRKCCKaZabDbILYTSJSLZLCKQDZDCDQCaILbATRBJALCbbKYLTYZKaASSQKATTLAQZLALCJbJTbabLYCCKaJBTDYSRAJKYZKIBYZAQaRSRaCbSZZLbDSbQJYbBQTCKYZTaYSBJDSDALbZDbTQCRRDRAaCRDLJBAJYQDLIIDaBDCITaIBaRZCDBAaKCJSJDSKaLDaSSJQaIaLRARIDJLLQYDSISLTbKIaRCKDSaaJRYDZAADYaYSLbYAYZZAKaaRTBKYBbSDLaDCQKJRQRbSbaLJRBQQLCAAARDYaAKJRaQIRAbZTLRZYQIDKDaAbAQYaKLDTRDILYAKSCRDbIBITaaJDaBbDBTLaBADKRAIRCBDIYRTJBDQbTQDCTJDZaIaABbbbbCTYSSQKLIhQZJDBABKCBZabTQBLYSBSYaZYJQDRAbRKaLZLQTBCbQKaSCLKSQDTCSaIDTLKBKYKJTLaCQZbSLLBIBKASJCKJLLKYaTBYJQSZAABIQQTKbabALJZaYLZRKYRJAQDSLaKQYBIADCaTAZaAKAJQCJQJLDDJaLCbJAKZJTJQKSIaAATYQKQLTRDJARbaLAIQCbBbAaBDSZBLTLDTRQKTLTaZbTSCDYTAbSQKKZCZTaCQSbbBbJASIQaaaSRCbQJLBaYACISBJKYTSKJDJLIabSAKAYILZBKYYTTCJYKJCJAIDJIQLYCbBARKDIQQRbAQIDaTTRaRLRBYLZZQQYQIBYCYDRRYbJICaQIKQQLIIBARJaBJZTZZTTaQJbCSRbIaLSCCBJARDLBCJIYQbSaYIBZDIQIDIZZZDYLZASRDLZSATBACLBabLRBAYabJKZJaYTAZQDAKBbQQQaYCZLKKDACBJDQRCJYZYBIJZTCCBIYJILYLIRDAZZAZQDDKBJKSAaSbQARSIZaYRDJITLARYZBSQCLJSJSbKZTQaQJCbRJQBSRLaBaSJBLaIBRQLJSJYARTaKBDSSSLCbQILYZaJCYTJDTQDARDbCIYbDLTTSIAaJDRQDCKDYAbAQKaLSRDKabYJCLDbKTJBAZBTaZSIQLQSLabIAaAbSabSIabTTAJIZDYDRQJYYTSCYQZYaLCALRKAKYLLIJTDBaTDaAQDICBRJaDADRCZZKDLQIRQTaQSQKJbCaaYSITBKbaYLDJCBIbKRDSARKTDZTSAJLSJQRRIRbDDJQYIBIbAAIABBBSKSaKZRKBKTLLTJRZaISCYZTAZCIYYIKLKaZZaTAZaCbYJDAbDQBbIASSIZRDaSBICbBBaSSYbKJaBIJLQRaDLaSQJZDbaCQLSQKBJJaYSCSJDIBLYbQBYZYAZSKISKCKTCLJYKCQbYJASTQCKZYBRLSCaJDLYBSYZSZTISZbaQYbBYbCaJKaYCCADJIQaYSDIDaJBTbYIDZSbKBQBaALJLYBRDRZTSIDCabSYKDSBaTLQKYaRZSTZDTbQZTbKCQDQSbLQRRTBYKBSaKJRaLLBLSCLKICAZRTBJKQJbKCIJSTKYASYQBISSYTDSKJBaLaBLKYaACLAbRILDLBBTCAaJASLAaBJJLLRZYCDbATYBAYBASLIKIAZQZARYAaBSZTTDZYbaSBBTISTLTQBRKYRJBCJAQYLQITDALAJJJCQBICbCJZBBATZRRBLDZQILSSaQARJZbKIaJAQKaTRabKQTCJYLRSKaTLZDTIYCITCDKIBDZRRRBQRBRIbQYRLaQbBJQRSbaDJIBZaRALQASbBQRBDbSZYBTaDTZACBTabAAJaIAKRSBRSRaJAbYSBZRJLIZZDDALYbCLaZCBIKQITQAQIJCCQZQYATZDJbYbbKaLYIKKbSQDBIbTaZQIKLaYTYBBAYaYADAYCKIKDbTDCBQIJRKYSKAQZZYCTaQJIKRDSYDLBbTDLCTTTSSbZTLLBYYTLAaSLZLJAQKaIbSQDTRRSZQLRCSTYIRSTTQQLDbBIBRDDIKabRbJYBSYLbBTTYJaQSAIDaaIDaYZDDCCbIIQRBABaLTQTRbRBZSJTZYZJaTRTJSQAQSCbTCbLJYCACbQSZQIBZJIBbKYCYZTCbLbaBTSBCYDBKSJSCYLBABJQaCAbCLCCLBYbSIBZaTLTKLRJbCYKZaLIBSaLZaYIQSADLaBYLLSCICTCKJBZYQSbbKBbTTSIDIJAabLIaYDSIRQTZDBISTAbDRSaRJYKARCLAZSBSSSZbZaaYQJKRLZQIZBQRJJLITJRZRDCSaQLKZCDLIYaIITKTIKSYCRaZQRAZQJRaAKKRLTSSZABJCBDJCALbaaYQ"},
Embed{72,230,"IbaTYIYKIISIYISAIIIIILIILAIIIYIIIZIIaKCbSCCaAKaaaKbDZAKQbaDSBZaDYaaKZZaZITBaISDRLQDYKYBLYARSLRYDISBRaKBDLZRDaTIBSRICaLRKSRYbLYRbYCQLTBIaSRZDYDJSaAITRKaBDRAbZDTZSaBKBbITZDAbIASYLCTDIAaDKTBAJTZbBBJCLDDYQbaASSaCJIIZaSZaCYQBAICYRAZJKDTYJCKJKBZAYLTRSDLTYLCbYRTIKYaAYSCJRbBTQRTLDBKSBKTKDBbZQZZIKBRYLTYTaLKRKaDYBDYbYISRaRDAJbQQLBCAYKTBZLQKaYIDBYJJSbSBTRDQDQRYabKDKCKZYZJbDABRaSRKDBIIJQDKDDDZZQJKQJCTKAABKaKCCJTQCSJZYJCTKZbAaBQBCABLRbRZKbTYKBLBRDSRJKZQbYIDaZAJRCJQYaYRCaYbTAIIaYLaIQbaQQTTLQaCDTZKCCDRKKbbRaJAaSKLIBIQAZQbIQQJaCIKTTIQRBKDDBDDDKKLJIbRLIAYZBIbKCLSIIKbKBTIDYLAbCRCLCTCYQBDYIZKRBaSJDBDAKDKKRYSICQYJZTATYBbJTaLYaYZRTTTQbQRQSRTLALJBJSAaZQSYJZKCBTLADCJBAJCYALBbIARDDILKTbTRBaKQbQJDARDaTaYDJAaBCLTTZRBTTDaJQSIZKCRKQaLDbaBJYRRTIYAaBTZJKALDTTARbRaAbKTYabDZSQAaRYKIIZbKZDAaTKDRBKQbTaCaDSaRIZTSDIAKLYLDCJZYRJCQbbSbZRBARCYYQCQRCIIZRAIRDBCIQTCSLSZLDIRSZCRLYIYbSCYIZJJBTAZTDBTaBIYIRYBBRKZaDCKZAQaKTTTCRYYIZIbYDQRDZDDLaYIQSLDCLZJSbAKRCRZRaDZBRCAJITaaZaSQDIKTCbARbTJJTYbCDISbKRYDYIaaSSSYRJQJCAZIbYKbCZKLSBJbZKDJLYaACAYIYBISbRYBbCTJIQBBYBKbRJLRAaQRZZIIICSaJTJbCQZBBSZbQTSLSaYKZIKISBCZJDADAJaZTBCRDAaTKgQCJYKIaYIBBaabLbIJMTYASRBKDDYBJRCYaAQDDSILJZCYKJBBTbScBDBLRYYLJbTSCSaTaTJRAZDSAYCKQSDYZaaKZDTKaDYYRaJRSIiCDIIJBaQaJJUBJKTIDJTRbASTQaZIQCBYATTDaRKZSbISCTbTITaRTAJJAZIQTZbILaIZKgRSbbKDQCSRYCDZaIBDJKbBCIbAaSDSRSYJCSIaYRZEBCBbZbSAbCLTDTJaaRABJYTSUaBZZKZQAAbYJADZIbZTLYaaJDYABKBKQCCaCBRLTQbbBJAYBBAQTJRLDJBTaQKQBhYYaaCbABTLBBKSKbUQTEKZDbLBJILAAZQIAJTTBbSKSBQbaIDQJbLICSibZERaTUQBKYICIBRLAbZKbZIYRaLCYgbDRLDAQCbARRDAYTKAjTgbRDZTQbbDQbDRaicIQSYSbIYYRBbZRRESCKbBaZKIQLQbBDiDYZahbQTRjJcIYgQKLTMbSBIiCQJICJJIaiaRJiSgaCcTKDLJDDDQBBCaQLZKTAZiLDDbRCiLJCjQQagARLDbJLIaTCZULCaIIQTAIRbJJDBIKDIJZiSQRAciDTALRgUjMaAbLDSALbSTZCjQZJZLJCYcgQIQQbjBCBEJRMiRCKLZZZYiKUDLAIYjSEJjAAJaJCQkBDjLEJhCSYSTLBCSZbJBTiBKBaLRCjkYZTgQIkKcaKATEYESkTSYSIYbaLCSCALLIEaRhMBJSIATcCZRLkjYLDBCbISKZcYYDTJDRbiQKhbIMCICCUiSLEKCJJCkCkCkbQUJaZZKhaRCjbJAILSjSKZYhDCYZDEJTLgSQDKUKUcBahRaTZEMYQjMUgKJQJASaZTbYRQUDYSQUMZDcAgjTiiIBbZLCDCjbZUBDLZKcSKEhbLIILSAARQCJBihRLSEUUckDgARKCLLjYgQSMLUUchIIgQIMZRgLAKBibLJDCSRJYjYRRachQaccRchJELSUQBBLCAUaRQTigBKASMKLCkLRRgLSYbBMMTgTAabIZCkBgMRiTckaYLKBiTUKcTJZQKDMKRRkCaASQKMcAZkMLEJbijJbYAjIBSacbTLDCECDJbSRjgiRciUgSLBBBJDJZgkERYYkbCbIADDYUUKAUgJiicMaZjMJTLASIUTjThYRKEEbJbEQYQRkETbMShgBSgSaLiQDbbIkTLAcBZAUajZcaMjTckiCAkQAaZChAZYJbEDbMZUYQcIDcATCKUAkMJggJiJEgaQcIYZYhcTSKIgTULSRCRBMQZRciEkbMcKAJiaISgDTbMTBgbJiiibhADSkYULhLaYUkDSJgQELCSLACRDjDMAbJJDZMMLKjCkAJSEjhIhKIiSYZEiIkbJcibUMICJDhDbSURMSEQiBEITBRUEZhSYTaDBUMgTLJQcMLiEMUSRBChiiIRRcBgTZigBEhiJLbKZAMiCKbaZRgSAKEcYkghLEMMkkYBBDKAkITELEYYDgQDbLATjajDDKjSScUIijILQgcgYgLBAChgYLUiZgJYQJRaLMKEUiDAUJBZIBATbUYJYgRARZiCJcALYYIMDUKBSRjLRCjYTEKIUMQIbTBYJDTSKCkAEQZZRLaYTETiRDJYQjQQBSbbESihJBKZRLjbbiSYbAbALcSgCTZZLbMJSAUCKibIaJYacjSEQSIjbRRLAUQjQZQEabCYABTQKjiQMTCbLJCBJZBJJQELZCBICYLcRSYSjRgAaYKBcCBbKiijDTULBZLRhILgAScJABaTgJJKCbJARkEAKhLKJaDTaZhRIIQERLQTRTKLCDYIDiQjACZDLQhJaCbiLKJhAMBbSJUMCZJQJKIDMBBQQCZZLIIQJgbASSbaZQjTASkRCUILJSTajaQUgZSKkJCYCZZKbBZaJbKRgBAKAbCgTbbZELMUaJDJIbEhYTZQAaYCYAZQBZSJCSRBSKKTgIcMZTRMYDKDbaiYbQCYSLIZBIBKRKQJZAaKDZaKDbSIEMaLRIABSYZUKETYZhSDQEbKUbATDZCTZRQSJbDSjAQBDBTDMABJIDJJZTRLSDKJCbYZQARKiYgiAbYARAABTaDMSDCARijJTIJTbQBaQbTgBBAJMjSSLhbRZaQaCbkKDBAJKIARJDTQZYBTRTRKIRaJjARbKEJYSRRbQLBDSQbALYabRIBASIKKDDTYQcTIYZJZKaDICCKabUZLQJYcSIYRCLMCaRKJZTJQDajJBLYRYBCLRAiSZCCJALDDYbLCbRKICLaCZRRKQDKDAJgCADCYLLBaSbcZDSbAgIKCZZDDCDbTCZQRbLMIDLhagCaTRQQKACaZZSKTIADBIBbBKDZJSRZMIDELSAARKbAZRQBRTIRRKQgCabLCiMKaaDTYRJSASKTITZSIDYJQKQjLAJQBbYLaDJKZbSDYIhZDYTAjSaRKSSCQDCLgKaZILIDIRTJDLQLKMiJjRKLTDIaRLBIDBDBRDACACRLYIaSRaSTJBQZYRBRBcLgAZARIZRbQBILKQATQYRgaJSZIBRLLZAYaILAbJhDaCAAbQSRQSZTSQbDbTIJAZaSgIBJRMbLLJCQaCabaZQQQYYSZBIBLAYDIaLhKbYCbIBhaRJSQaZQDBTAJSSZCLQTjAQSLQZDTLLZLLYKZaCKJSCSZAKISJAERSSCDDZAZJTRRDLZTKQkLZIDDSCbITYUBRJQaCbTQKRTQIBKRbYbSLYJCALAAZDJbQYbbQYSBADZUCKJJZRLRSSBDLLSRQiYECTQLIJDYAQBKAYJQTZZQCJTIBQLLabKYKZCSLSCSALJCCaYbJSbSZIBCKDIKZABDbQaMASRIbAAKbTYDZYRRCKBQaLJCTCDCCTDDQbTjSbRLYAjDJZaTQDLZTabKZDCRAIDbYSZJJJZQLTSSRYKAIRIAQTBaCZZJaAAILQYKJRAZKhDDYYZZSACCYIDbBYAbQDTKZCLQTDDITJTIJQKTTDBQDJCYRZKUIJYLAQbbKJYARYJJCASJJCTBDQYKLYZZTZSIDCAKQQbIBSDITQKZYLbbDTQBRJTELAIaaKDJYTaAASIaYZTRZRCLTTYbbcICaTJQUZQSLCSRYCJLCKYAATLaYAZILQZLRaSYJbQbCKBQaJQbLRAQSBaQIYQLSDTQBKIQBDLZaaJIbYIBKRRYJLDYaUBaARLbCLbCbRTBaSQTbJaYTTJBIACTKITIJRTYBDDYIRRYKRRRDKAYARbIAZIBCbbaCBSbZJbTTJTiBABTADQYaDSJYLQRBbZKYaIaCLTDDKbCSCDbRRaIQAbTILbLTAaDTBYLLBISJDADSbIQDSSQLZLIYbARCSTgbDJaRMTIbbKQCBZaTRRSLbaDbYKQDTABJSRLaCDAIBKTYJQKKLbCQKJBSZLLDYRKYTISDTaKRQbSATRYQQbLDJJYQQABIAZRaZYLaBILYQSQCDQTKIbDLBLLBYKAQjLLSTZBLZZCBQQYZTIQbJAYSaILCAJAbSBDbZJAaLZLDKQJDICAKiQRRDKISKJQRYBSZbCDARRLRZLaTJYZBSJIDKJLAARCIaRSaYILSIBSAASQKDBaaLKbISTIITTJaQKaLbRLBKbRURRbaLDCDaRAASJZaZKAYITTZQYLIZSCADTbDTLRKJRLgbSAQTKCDbDZaYIKBDTRJJCZISBQDRSIQZBAZSabQbBDBCLbLBDKQJLKTZBLBAQbAAIQIaJBLKDYZLLRZTAKQaaTQISYZaCRDRUAQDaASSJJaZDYSaDDDCbKCBTRaSYJaSYbZbIQaJKQCJDBRaRRAKIbSLABRTAZIJRCSKZBTKIDLJRJLLCJJBTDbCTbRAQZZKIJCKKBQQTDBDaLYDCYQKKSTRKKIKZbbALZJCQJDaJQSTaARDRBBZRBDYCLaTLBRCTLYbYASICJbCRYDBbBYJTDRSKQIBDJbBZIZICIDMbBBZQAQSKDLbTYITKDBQTDaDRUaAYDSZJaYTaIRaYSRKBTSZTQbbaYaLRRZaTBSDSRLDJLZJDLRJDIbSSDYQJaRAQQaYaDaKRbDDCbDALYSQaYABJBABQIRTaIbDAJKYLLLJKSCaIAJYIbJLZIBTCLKTRKQDLJIDCaTCADSLQIJIAbJJbKSBbaYCQaJQSTDAJJQZABaTCJITLDQTDBYAYRQRZCLaQIbZZCbYBbDDBKTYbaKJbDKYbRaQZDDRCIBCbbRBKCZDIACKSBBbSaQYDBbCaAKRaTRYSABRKaBbaZDQZBaLbICBBTaBZTbTKYCKSLSABJCKJYJIRADQaTDSbTZSITAaJYZJQRabTTSBaCCKKaLBJLKDSYZCYIRDDBLSTRBJJJIbBaSYKaKQBSZLBABSQabKQaBDSISLCaQLAKKCRJBTTKRILDABKSBCTbbQbabQbRKZCQTJBRSRAJaIZZTRSBBQAIKBbJCBaQBIZYILRBYaASQQJDQAIZbKBQLaaYRKTKKZaTBabTabAaSbKZRKZRJQSTYLCaAKTCBbAYJLCUALYBYJKSJDSDJASRLTJbKDBCTRabCLQDYILSTLaSDRbQIZRJISbYKKQSTAJRRaTbQRBZLDbCCQAKDZTLCISJSKDCCTDbbRaDZQYQbLTSLYZaTCICYTBDBYaKbSQRbKLYRCAaZYCRTBaIJBLSITTabJRAIKKaCaJLTZTaaRaYAaCbYCQYQKKLYSJaLJLaBDICCDBKJSYCQDLRZYSALQDRQDJJALQCQJJTSQIQBbbTaLLCLSDSCTQZRTACZRaBbSKLACTKKRYaAKSALLKZDIbZLbALDKITTTCCBIBYZbbLKBJSQQYBKDLLQRYIKQbIBCCRZDSZKbCSbDQCSSYLaQJQCAIJSJIZITBSabaQIbLTbQKJYaTIRabZKCKKKTbQJbJCKYLaBabAASCQaBLCIIZCRAaICRAAAZCLJYIZYAQYATRZLYaZbRZAJIaCYBbZTKYCZLbCZSLZaTSTBISaCabAAYbAaIJKIKRSIJRARRZASSTLTIABQJJYATKJJAKDaaQbKYIZJbCbAYZKZJDDQaQQASCDIBZYAbBLSLIBLCZDQKTIRJBaAZBRYJbQKBTCLSbTYYACaLYbALDYDJIQDBabRTZSabBIQARATTYSRDKRLYZDCTCKaaQQaTTaDLRRSaaRLZKJbaSDQDCJIbAZKBTSKbTYYKRLYAABSZBZDAYRBAIITKLBSZRbTTJJCCKIZZKIICAbKZKCSIKJIZaISAIQIRKYDQLRAJKBSTQbYLDRRCJbZCZaYTbYKKQIIZaJTIbSSCaABIIKIJRaLSTLQTTRDaRKKCBZQJDQYTBKZYbDYKYYaLZYBQaQALYRDRJLYKASIIQTRSRIIaIbBQCALZYAbDTbQYIbKBQJDLYaKRLDaSCRDRBAbQJRYRKbCLJJCaJDKCBZSSaBCLLAZIAbYKRZLaTDLJLQaQDBYQIBbYCSSYRTQZDCKQTLYDALZAZaCIDJaYRLZJTCQYCKbBIIARAYCCZZTATRDZKbQaZQaKRbJQKLYSKbRBBZaYRYbAbCJYbLCBTTTQKKaYKJQLQCLKTaJIQIbBKSBBTMDRZCTYRKRSBAIDIRAYLbbRaLDbCSLaTQZLZLBRQBaDTQCYTCbJZBABASSDYLTKCbIABYAKSQBYLIaaDaKJJaAbLAaBLBADICRbTYQbQZZLIRDCIQSKBZBALQQYALCZaZCJTLKBLSRSISBcZJAYTRJCDZYDbYDaCaTbRCabCILQRLaKCbIIITbRDAbLTJQLQSZCaSLYZIYDQaBRBSYRTRTaDLSYYALBLRSJKRQBSbTRKDBLBYbaCQKCbACLQRLLIYRLYIYSSAIaCJbDICZQJTLTRARLJCQZRALRSRAaTKIJKBbZRLRLYAKLALCCRQIKJIbIAAZCaDaKJDCaYDAKDSDYJJJKbDTDQKAYSCJDTSBKKaDSIBQBZDZZZTBJBZKSSAbDCRIYAYQZJTDCSQZAbRZSCRLIIZYASTbIIBYKQJCCbBLRQaZRISJLaLJRYTBCQKTBTQAaIJAaAJJDZCIaJaTCbIYDKbbBTADRRSZSIZDIAKIRSDZbbSRQBDZYAQLZYZQQICRbaSKCDIaSCLAKbLKSKTTYLLJSDZQIaRSQbIAJKLLKABAIaAQIIRZIBaQRYZYJILDSDRASbATTbBSTLRQJIYQBbTKCLJJCTZAYYSIIQRJIRbZaBKKTQISTLBQBaTQILZLbJaADLTLRCQKRSSQCLRYZYJaYaCDQZYQKBJAALDCRQLBCIJJSTaQRBbZbLIZQBJAJAAZAbQJJSDCaLBDAJLQKCSKDQCJLZBSQZaYTDbaIRSABZRZLRSIAaZDbILIARBbCAZQYbZSACSCTJCTRYZRQCbKBAADLSAAICbJaJSTTIKbZKJKLRAIRLYKZCSQYYRCTZQSRQaaZZaZBQRLYBaBRDRZaICZISQYSDKYQBCbBLLCYRZQAZRZYaIaYZSabJBIQZKBKRaaYYTKSYJTABaCTASTJaLJKDTDSRBTTRRbJQKABRDQARaQbSDTJTAZZLDSYJTCBZIJAKYLBSJaKJRbBCRaCTTaDBAKTIKCTLIIAIDCAAaZYDSIDJLDBCJQaYIDYYbLDDLYBQTILBaRTRKZIABbSIBKCZDSDZCZKKaKTYYJSbSJLTQSKAJAJKSZRQIAYLBCAKLaDRJaCILZTTSYYbLTLQTaLSYIaJbSRAYBKbDYAQRCTAZTRRACAbASZLQJKILTLADbKITSYYSBQbAQDbLRDLZSCBABaCZRILYRKYATSKaYBCLIYTCJDKACTBRRLRbSDKICLDBCRYSZSQTBKQbbbJZTCTDSQBCDJITLIKSTYLTQbTLKRZCYbSTbCKIaSabLLZJaKCJRZKJIaSTCDISLBYJDDRSBSQRAQLQKSCQDTJQBIZTKABIJQaRRJTJbSTSDZRKBDbDYTLKBbQKIBBLKIQLYSZLKLIRKCCYQRZSbZRCTDaaZIJSaTbTIbBJCRLAIZCAQRJIICTaQYKJAJZCCaJKbBAaCRRZCCCYLDLKJBYCBZJACIabLZKYLBSbZBYBaSQaTDLLIADDRSICYABSZCJbYAbDADABAZJJaaDRDZCbbLSKCRLbYISQZKYIDCJDQBbDZbJRBJBBaITAKbYIACaaTBCJYRQRQSYaKaaJaLJSAbIaTQaDTbBTYYBRZbASYaQTSBZIQAQbJKDLJTSKaaRSYYDDAbBLaKTKAIRB"},
Embed{74,236,"ADBQIIJKKIZIIAKALYILIZIIJCIZAIAYKLYSIKSYaaLYbSYaCaZKZaYaKYTaSaDRaaaaaaKBaKISDaLYKQAJDYKYCTYDLRaIRDICBTaKRDLRDZQTTIBabBSKTKYYSSQRKYDRIBRAbSIZKDZJbTATLAbIYTbBAQYJKDYQJZSLDIBSTIRJDCQTTQCJZYBCTKaYTRYRTLCQDYAJIDTTJDbSIDSZBIDZLCaIBTRIaBaDDSIbSSALYbJaKKQZLIYZZCTASaBYSTAQBCLQTCLDLILCaZYaJRQbQLCAJDKZDIZDYYTJIBDALDRRZYRLTBCbIDaSLACTaYCZYRQDaLBRKDSQYTCJCYJAJKaLZSaDJYZQTAYCAJQLIZSDaaIBBZBDBKTbTYIIDIRILBKAQLISJSZKQRRQAQTRTIYBTBARTbCbZDSKIaQZZRZaAJLKDKTDbZTCQQJCRKTQaLBYJCAIAZaDabQYTBLZLBCaRCTTTYaDKbYAaaDbKDCLSDaBJAKbYaRBaYRBRJITATBaaCLKbJTDRTaSCZQITBaTLBBbLaSRSDJLBADABRSBRBRZKCabLABDCRaLBLaBQaSbLCIIKBTDJSaCDALJTSbICLYLYBaLARCaKKQAJJALDYbaCLCYKJBIBCJaQQRZZBbQQYYbaSIJbQISJAZBLISYYBbZADYKDYTbaQIITCBKJASQJaJIYAaIDTKaJBRCTQTRBabQSISIBACSbbDLbJALAILLaTKIJTQQTJYQJYKIZTDYTADLYRaILASaRDIaIIaACZQJBIAQSTbRIbAZCSbJLQbJCLQSLZIbQabJAAAIDKRTZCSYTLYbSQZDBRBLYRKbTALJKCAJZSIDLCKLbCKQSCQRIDJCTCIRDDIBBRIbLKbaaKaQYTKSDJRCABLbTJSJbZRDQISKaYKICDTLRJIQISQbRRACKbKLDQRDKAaQDIZaBZKDRRaZaIaTYRTDTBDIRabKCAYKZJKBbYCQJaJAADLQJCRKLQCLJLATQZBADYaQAZZQAbABaZSDJbSZbSSSIZRSAbIBRbYbQSLKIRQZRRZSQBLCICZZARYCQaBZBCCZSQDTQRJbCDKSJKIBJZCBDTDACRZRaKALQYAZbIaCARBCaQZbSJSaJbCSKbSaJRBDLabaZaTAITAKUabJLABSADDYAYTQTRBSTJBZLLJJJaCZSKRaYKILAKYLbKBSYQLSAAAJbKaZTBKSZYDaaTRQCKbSKIRaJDDKQLCZTZTMRBCbbRiULQKKLACRIAJaSbJASISbYKZaTJCADZbTIDYaKTAKLBaSbCKZCLAIBDCaRRCLJKSDSRCBZQRZJILYYYbQRZBaSKYJMLDbQALKRbCBRRBSJaYKQZMADTCIAYKERbIJBQhASbDDKIaYRbTDQKLYJQAYZQLaSBQCQDAZaJgcaCYQKYLZCDCaBbZbSIBLaghBRTBbQSUZCAKZTIBLLRAAIJSSCbQLYYDIbbbBSKLJZKDZRRbADKSALDJYYaLaSjACZBBYCTSLDSKLZTIRKYQZRbaJTSQCSTLBLZBjYBgJAAQLSUIaTbBADaRYgZYBJBBjbDKDTBJQKIISgRSTaISQSBbYCLQTSSCRIhRBQIJJLICAJTSbCbKJIKBKLBILYASaLSRLSYLAQBJhLQaaIcKMDDIaBZjYTKjAhAUSLQMQBZQTQJIIacbZCLiZJQhBDSaAITQZIaQJIZiYBKBQQhUSbDbYRMDCSCIMIYRRKjJYDMDDaaLUYJSgQJLJBDCZYIgSQBjTIESDDBDhiRTbSgbhAJYCQhQTLSYBRCMBLYAJRAgQIkLZDIZITbKiBcRDAKUbTKCQIJRJEcAIYKSDIUbhIaCCLLQhcIYCaSkTgZbKbbLRRBgBcbhLRRDSAACMJSZhDDCZIBbBQDZbaIDhKEIibTDRKSKAgiaSEQBLYLADJEIDKAhaYCDJaERjREUZUahYDaRbjaCTbIZgSSSEJCUJgaYZAjaIjMbAgcDDDTYREIYgRZTZLTLEiQCbQjiYURgYkgDhccLQQaSKbJTEJIbSIBQTJTIYTKIAABZcJkQjJCBbUiAYYAEkJbBIEYATARRCJjIKDKaAALLKYSSkZLMJTJhESDZSQKaLbLUQICIZBQKicJYhckREKhgSTMACbCTbTKCEahASJYjaKIJcBhJQgBEMZUYUbgSYULCAJZQEBcCCibZSIgSESUiaQjSMJYIjIULIiZgZSRURjUEJQiUjKZUQgZCKLETbAbCECYkJDZihKSUkRCQRBjBbMAgCLhbJTDkAYcEiSjaJTSMbMaaYRLLiADiKBASKYEUhEjEMBDkjLMSJYjMaMAYQLDTYDYJZLSCYaMTaiMIASAgMRgEkhiThLAiTBCCkBLhkjTTjSKIiJUaibLARgQMRUQYBiMRabQDQLCjJaIjjDMIMhQLTcAkZjiIhQJRCMUgajECYkZYiZiTYCcIJYYIZAARDTajSUUkLDkkgRgDCJAgLSLTCBgjKSRUhjBjjhCgaDjaYEiihcJRBicYgJBSZjCkTKaYLDIgTaMcJcKEEhBDkIBaCkgMTgYcQIjZJZBhABEJAgiUajgiJChijATYEUJKUDgjALLZhiJZSSiSYTAZjZBcJUkjZYMEUEgIAYiiRiCMMbZhJjLUMKIDSEjQAhUkLkaCJkTZIYijaECUJIJDAYRZigiiLbQSckjhQIIjDIJaEjKScRMDcIhBJbcCikjSCASZZaUhKgKcKkbiIbScDgkbIIIEDBabITIbgQJcCiURAkCRjKhCCcUJkRDKRQEhIDCZSLaDZBRJjaMZBCSaLJMcagQAkjSAKYSLkDbhBLZgLCgBUZaKSSBaZUZUjAbRUCIMIcEMcTBDjRKkZRTILQBSMZQCbjAkIZIIAZJUCjAjaAQiYEBaKRYBiELREZKKKMTJJUDSLILkSEAKTKDAcbKIBYZcgZaYjKLiTZEgQTQUAZZahKCQSBbBhJaYAZKMSDMSBJRSQZCCUUhhiZYDTRZQKiChjUhCUcKcLRMTMDCTbZabBaSICQSbgTZCBSIcBYDbScKILDURiYSYRRYRbLjZUcBCbQAhYAMCiQBLBCDRCQAJKRaLSQIUBJLbZTCbBhDDURLMKZUbSUJSRYIRSYabAKJaJDAKMjcbQSRcTiAJZCiISTJJBJJZbaJMcSSCCCZcakEYARDSKDDAaJcDcBRThKYRQEbYAQcYgAaYaJKCZYTJbRRCAhAcJDTRICLYACJYQciaCCiRjETJRTZKTTYMaCAiiDITACYKDDBLTKLDDZaCcLKYCZQZbZJQiCLBEbKZRBgZaCKQZSBJDYKTQLCJKCBJKDiERhRLQCJBKIShiMZQZihTaQYRMSKZBJCIQQTbBERTKIQDYYAYAQiIaJDaJRkbYZTKaRaLTCLIYEDRYYQIbQCIZYIIQRKUKIbSRCJABBCIaZZDRILSZJbRJYRAZCITKUAAgDMJZKDBBKKRQRKRbJYaBTLIBJIZZTRKSDKbMUJYTDDSbAgAYAbQCMLKZCYTYaSRaCTSALLKIAQhCMBaRSbKJIDaSIJJhQRgbQbKhRATTLIAZCcKYJTTIIJJTTBDACYJQLJBABSQIJKKQaRRIRbTIYaJSRaTSZKZDRSRgJMDLgSLTQTJBTYZKTSQJMJJQAIaSBaSaRaIbZBRKbhgKQZgCKLIbCTbBIJSbITYUCAQJLRJKJLYYTLICLJAJTKZbSAkZLDAYDJJKKSCAYIQYJQbQCJTEYYQJIKQRLZQRjQDBSJYARZRbAICYARRKKJBZcDATbSSIQKTSCATCcLEYBbMJZYRZZJJkIRSQBZbbJLDZRBbYaYCRIYACKQRDZIYbLjBiCITBDAhCYDZZSRKYRLYZKCBCRCBRKaRKaYAScjZLDYbaCbQLCbYIRKbTbQQAZRKRCbRLJaZSJAABCaJKaCZgCZaTaSIRYKBZCKSBbIJAbahIQBRKaCIjZBSaaEKKDTAaDRaJCYQZCUQIbQbIJbKTITKTJCBRaBicJJBbZJSbTZDSRACbDCaZCiZRYIRSKUBYIQYDIILJARMYgDJRARJJSRRDJCUJJQbCcTZABYbRaBZJLaQBCIhCBDZBSDCRLRZKBDYJYCTKaADYLaQYTDQgSYLLKLZLZZSLIKLKCTQJRCQKSSaQQIbLDaCAjQCYKSARJJaKDbYDRYaCTIaYZTcZSJTZJDgRQBLaSSaTICKiDYAKDYSRYRKbZaCabDZZSJZYbKSKSDbQYaCTaQRiBDbDKRKbSKaKAZBJabTKDDCKBSDSRRJZILJCYKADRKSCTLAAbhCZABABDbLBbUaBYRQICRIIRLYYKCAJaLIADCKbaJSbKIYBATILYJTKTTRIaAQaLDgYbaCZSBKKQBbKBBKJaRLQIQZIAbIKBjJCYDKKJQCDJIQbYQLQSaSbAZYKJACaQLZRYBaICAZAbbZZRIALDaRbaJaKJaCaDQCRZASKDaTZDjIBRTQbSYDDRYaRTQbZZaTQbAYTRCLADDbQCTARAKBYaKLDCKbJBIKbbRSabTYIBKABRaaLQCYTAJKaKRZCSYBYbDQILLAbZEDDDQaTBKSKjACJKSYbTSLBJCZQaYAZaaCQASTZBZbKaSaAYKZDQKZZSjaDCTbSYaLDAISBRCJDZCRZiDAJITZbICZZBICTITAQDaYaSIKLRCKIJYYZCTQaSLDCYaKKDTJZCAASaRDLQLJYaYJIIKBBaabAIKRZYQISDRYbICTRDLCKaSDIKTKJYQJjLYbYRTYJCYZLTbSSKIKQQRaBTUKAbbDJBDbQSBCKILJJTRYKaYCQBRYQDIKSIYLRBYIZSIaSAJJDKAATRYBQaCDADLLKRZbYYDIRJCBABTaIJCCYbJJKaTKJIhIBKQALTJbIBaSKCZACQQAZCSTDaBbDLabZJSIZZSDLBQIIAZTbZYaDAICabZaCQCRQaAaYaIILRQJJTJKYKYbJRCRKJKaJDRQJCSJJRBRYaEKDLBSTSJSKQLJQYIJCCSZDhYICAQCJTbDTRCBYaSYBZLTIKZIZaCCCZDRYEIKSKbBQRCJCKKaCCZIbbaTCaALBLCBjaRabJYLCZbACRRKDTRaASRJbAIbDBDRBJYRKLAIbSRCJUDLSYISTJbCLaJDYTgURSZACZbCSYbBKZbTSDSDQQZDRKBYaBSJRbKRSTIbLTRALJRICZKKDABZbcQjaZaSQTaQaLTJYZRaSYYJDDKTIDAICJaTKDKKSTKZaZbJDDDZSKaILAQLIJATAIbSAKDIbRCbCJIJRYJLYYBbjARbJABTISIITRCACZZYaAITDLbYLDbAYYAAbKJQDASDDbQRLKKTRZAKYQYIacZISAZDSDbICRYKbLDYTCTACBTZTbKZIDYSKJILIKaTDZKIDbSLYaQZaYDSCDbQDJQbRRCYKYZARKQYSCBQCKYDRDAaIIZbLYDQZCaYJBIbKBCQALZCIICBDZKBLZZTbSZCYZDIABAbCRJDCSSSCBLAbTIQLQIDRAJSTIZKJLYIYDLaCbIBDLQbaBSYbaABTJhRCLKKDQbBAbBYLSSBAbKCQaZSBJBIQIBCLQaZYbLDSDDBZZABDDRBIaYRRTQYgARALRbIZJKBLTAKQJSYYSJLRDAQDALYSKZCZRKIaTSZCbKZabSDQAQTKQBJAbaRYATCbLDDZbKTAaIBRJSIRaAIASZRIYZQIbRJbAKAIZAATLYIYKITCIQKQLYJbTIZRIRRSJCDZaSBJaQaAaRLCbDLTDCLACYDbYBJMISYTKaZYLCSYSKZZBaAJSZZYTLYDbTJRaSIZBDCLSKDDQITCTDTRaTbSDZZICYaJJZTIQRBQBDZLLQKSbQRhQTICZaCKASDIITQbTTLLQRRQDKZYZbQSBYJIACaaQYTaZJQKQQSJBKDTJKSCCBZLaAaTLSQAIYTJLQLabLYZQLBIbZQQaYICCBBTYLLLYBbBbYSRJTQSDYSABCKRbLYJRSRCbSRbQYIRJbQLICYSDZYCRYSISLAJLJCDbICbKIKbQSJSCbQJKYQKTJSDBBAJKZKKCRRBTTRZRJLYTQATCRJLDLTbRaDKRQbTQSBaAYQbbJDJLJRIIbKRCaTRAabSaaJARAIYAZSYAaaabbTSIQZTbATTDYZaKCKQCBZTQTaRDRCbTZYYKbAbDDSKJRBBBBDCRIJZCKRZSLIQbCSDYbTQbQCQDaBaCSYLITDAbBbADSRbJabBYJCaQLaBaLAIQLBQLBIICIbZbSSCiBLaYDQAYLDbSATKBQabRRISJDQIaTCRJDBYTbSRQCJSbQDZJJYAKaKbJZASTIbTQQYCbTKZTYRAQSCTZYbDaQKISLSCBIKKCbTDRJTKTBBKLJCIaBSBJCYSYLBDTCKIKKbDDAbZKBKARLCKSbAKIAbKRAbLYMRRRBBISQALIDYKJRRSZCJYYbCZDCCKJJTRIKSSQBbCAJICISKACQYJaTDTbRCBTTRSIZZCRZLaTLYYZYaIKaBSZKaTJQSKLZSaYBCBQbLTBKBQSRJBTIDDCJLRLLSaSSbSBDABLQCKaADJLBJADLKLAJCZCSBCQBZJLQaLTKBYYJQTIAaaCZJKZTKBaIaRTSITbCRARYCTJDAJbaCLbbDTTYQYQDDbKIQLABSQaCCICTZQDYBLSQQJYIRAIKaaZRDLbZYTTDLDIKASZIIDKSRQIbBYJJRDDIBaLQBCZTDCBSQbZARCaBQYCYaQLBRLCaYYbCAbIQTKQKYbLLZCZbKBYYaYCZDTADbZSDISTbKABIDYLBYQQKQaZJbaATaLAaaLLLaTCZDJICQIYZDaTQBCaAACIKbJCKBbaCQYZbbKTJQYRBBDBDaQBZQYZAQZKKAIRYBDZJbLYBAZSSQLAABaQDYJaBIZSCBSIJBJIRKbIQRBYSDLbTAYSDRDZQYAYIKAJTabLSZSBBJSJYbJTbAJCDKIRYTIaKbJSCRLSDTIbIaZbDSQaaJARTbTTZCYBZDDDQLYDSSBKSIaRDCZARRLTYITIaDZSBKQDYBQbDISZbIKLBYTIQLYTRSTQQDBLQQJTASATYAZAYCZQCQJbTCTRTIZIbYIZLJYLTYbITBQILTZIKCSaBACRKDBKIAbYSRbDTTYbQJbRSATQLRSLSYDbbYJDDaJTZRSAZDaACTCaATAICaZbARKaaYCRTTDZJRTSJBTQBYAILZbRRCALYbaZCIZTRLKCZYCYSDAKYISBaRDKJJIIBDZDbSYSAbbARJCaKBSSBKTQZZBKSRLDRbZKARRJLZJbRZTADKZQaBaaKSTQCBTRDZYDSDYKTQTDCLaTJCBBJQKBYbLbbYaQKDYSRSSKDCSLTKJbIASZDYTKTbAaTbIbQYSbCJRBLSJTQSIbSCJJZBaRQLBRABBbQCRbIJYJDKRYALaSSDDRBQLDYJYAYRCBRRCBDDLLACJZaLKITJYDKJJRKTSYDDYTQQAAbTJZbDDCJJBBCAKYSbRRJYCJSRbLaCCTKYIDKCBBZbYBLITZYQTTTSCSDSBCRDLYZBDZKaIAYTLTBKYaBASJQbSQBBLLRYQLJIbaASbYYLYCSKDYIZSRDLJAIZKRTAbaYTTDZKKJbDIDAICQJCZIYBbJSQDSTKLDBZRKCRTYTCSCKIaYbZKYDbRbQZTIbIKLYZCAKYKJAaYQYDBSaKDTIZQZCIYCRZQZaYLSRDDLCQbKaQDJDTSABabJDKbABKCaIRDKKJRYQSaTSQBLTRAAaBIDALTJBZRRZZBISRRICQSCIRZKabTaAbQQbYKLRYJLQIZIAaIJTKZbaDRBSaIKYILITLYYTCZDYaZABCIRJCAaRTKbTbILbZRCBCTSZJSIDRTAYQYbAKQIRQASBBZbTSZQRKCCAJALLTCLATDIKAaJSIKLZIbDIAQZARKAAABTLbJCTBaYDKARJbAaDQZTaSZbbRRSLQTRTIaDCDYaZRLBYDKJJQISJbDDCAbRRKaAbYaQIDTDSZIQQJbDKIRJLALCQQIaZQCbTBILYTBbQbRQKTJLbAZaRQSZDAYLDBBYCDabIJQaDSQbQDLJQSLSLTADDZbCQRIQYLaYJbKBTABDLRJQAIJLYTSRRQCJBYSTRBSZZJIYBSCbBCQRaLaIKaLaJCYDAZITZYAQJKbDJbKZCJaDKAQDCRLCTRDQTaSJJBSYRBYZaaCBZDKBACSSbAaYZTBTJLAJIDYZJaRTLAQQLAAaAJCJCJRJaBIJICYKQLTDDbCAJLIQAbBDCADQDRIZLKRAYRTDLBbBLSBIALACKYRbJJJKZZSaTLIZRCITCZQBIRDbZDQQSSQSRZRKbTTaLLAbJKRSAaCASKJbQaKBLAQQDYKJCSBCTZCCDDSSaJQLSbRRJKDDbJKaKDaaRBJBIBTaaCbCBDSIKQBDTBIbACBKJYbbSLDTKDRaLALRTAKLQIDSYILZabbDaRRQaYCCIaBQLDDDbDASYJCKLQTTYKQQYDCJbTLQCZLIYBLQKAbJLISSYLKICYSbADKIRILASAIKZLZLYDBaRJKDCaTQYaKaYJKbAAQTKZBBKaCCLbBSLKJZDaQBTAIZYKSCZaCJIRZIYQaQabCLYaKKACKZRYJIBbDaAbZaRZIQZYBIZDBTATQDLSDKALCCLaYTRSTYZDDSISabbaDJSACIaDRBTJQARSYIKLIZRZLLJSBaCaBbTZQAaAZISKaBQTRJRSRQTZaZSJLZDCaRaDZLTbJYAAKDbDRKKITYRYKSRaKbBBRSbYBASLY"},
Embed{76,243,"IJATBIYICYLISIIILIIaIIAIIAILIQIDBLYJIIaRaCaCBaCZYIaSaZCaZCZKZCYbSbbQSSYKZKbbITBaIaCTQDaCbIbQZDKRbKRDLaDRBSRDDIDTYKSSIBbTJITaAIJDJaZDQLDBDAICQSRDKYKIbRRDIYaTaCZBTDBTDYQCBSZYIAbaLBIBBCTJRYKKSZaBTIITbRBDKYAbaSYRISTTQbbJAbDAZSKTaZBYIYTAJBabTIYaBLLLSYbKRQDBCIaQTTBKSQAIKLaLJQAIJBYaTaRRDIDDTITaRRYZSBLCDQJZQQaYKKKQTYDRBRABDQJbTICZJbDYSSaJbBZAbKCCDbYDYDTRaKBASaZKRBAZCBDYKbQTICYZLYZSITJKYKTIYaRSbQDIabDBKJSYaQTCSLQJYbRQCZbDQJSIYaDTQTZAYJBRAAZCbbCCJTRZSLYTbCDTKJCJKIDJZRZCRIDSKJSaQIZDDSIDRCbZJSCAYDRDZYISSBLYAaYQSLDaJSJaTQDJJJBAKaLYBZAbaTCYaaKJKDTLKIKKYCQTLDRZTIIYKaaCDQTACZBIJRIDLKDTDaKTYaAAJKaQZYbCTKYaKKYIILJQRAYRaYIYaYaaSbaTKDBQLRATJaBLSYYDLLKSLYKDRSIJBRZTQTJaBbYDRCCLYZRaBJALJYRSTTAaCRDSaKbSQbLLTBQRLIIKKCJSRYRZSALaSTAQZCLZYRKDRRCKSbCSKCJZaBIAIAIBbDRIaYbRLLSJLRAZIBaYCZLQTKQZSCRRDKaKDRRLKbCBTJaBRBTZAJbZIDQSITJTTSCAKISYTDZBCTKLABCQTJbDbQDZASBQTaRIKBJYBbKZbTCYRCYLRJLBLQCBTSRQRLaDbBJYQCDCQAQZATRIKIBYILLYBLKaTKJAZSTTISZCKQJIaTDRTaZDTBbaaRQTYTYTKSLDaCKBKTbKRZAKIYDJaTDDQTaJCaDCaCJKJQTKbQZTICaIIYZCAAALJYYKIDZLIJBTTbaATRQBKZSJSBaSYIaRCRDRIDALKSAJbYLJTYCQTAaZIaRZRaTSSBbCIBAbIYLCSaDBBTQYKTQBaaLbaJCaSLbJKKBKaICKRDTBAbQaLZQYBTCYIDaKYBLBYBZbYAZSBZACSQQJRYaIRRRZDQZaKZDISIbBIZDJYKTDKDTKBQaSKLIRSCZTABKDQIYLKKSJZKYIIJKBYcQQJICaIYCRIDYKYRTQZSCSbBALCAaRYSCbZJaJRQDbLbaDKbDJYLYIYLLbSDCRBSaAALSaBZYCCBKIbRKCQKIKSLaTDaZYAZaZBATSabIILIabJJSYYATCKKLTIKDSQZQBBKRTbSjIIKQQKbKRKDKDZTYabAZBIRTTSbBaLAaLZRRDTALIDCAYaJBZSBKSbBYIAQBQYDCDcKKTbIDATYTDKQLYIRJLZITDBIASTRYaaADDRbBBbaTaRTaCgZCZJAJKJYaRSIRBALKBYCKLTTSbKKSKYBQLbBaRBSCRaJLBaQAATaDaQTLZaIYaSaRjZiILCLDabcTAbRQZARZTCJALBIASBAQbhRCRRAYQZZSADKICBBTjDDKiRTKIZKbSJJRYCTTaILADZLBSCaBJAACLQcAZJDKKQZUjDIYLZTaDRSJQKMTYJTYTTbECSSIDJbCRIJZgBDbTRaQRISbILZbMISRDARTJSSLTALYQMKQTYUTZIIIBYAQJaJLDCKQLYSRjIRSEAAYDChTbRhJRJIBLSKDACJIQLSDBAYZQJCkUIRZULAZbKLRJTaIbAAQDCjCBabJCTbRSBYbbbAZSBRKiChkQAKKSMQTBQhbIQcLAAKDcYBBEKILBhJARARYbAYZaRDDARTRiEcUBRRSILJALBIbDTLiYbDQKLMDMCTCbAABQiDRbQZDgTbIZBQcSKcRITLBacZKSEJSCJigCiIJbBDaYCYSJRSTLiIRgTDYCLkQaaTAjDKZgJhMLhSLBiZQbZJTZADCLASIAShgZjJBDDTaTcBAcRETcaRahKSAJZAbQbgLigMRUSRRSaaDbbUIUbbgYaZATQbaQhZkiKLAibgAYIQQaCJLZZSjQDiQTZEYhAKZbLAKCSEIUiZbDIiJZaEAaQSRBahJMQCiMQZaBQJZMQSYYZiSDBSJgATIaMSbYDUEgkMBCThhBMTABIYTDICDJJUachZMBLLTCDKBQZTMERRCMEcEYCcRYRCLcSgDQAAKDcbJgEZJcQaZZCcZKAZaLjCaTAZASQjMBTLUIUESTRIETZYjIcMJIIgaLCkkLELDbYACKEZMjJUYZRaTMAYQMRigiDCZLSSjBAEQEKhQIJbUSYBiLhRRBIZRIkRhEkZBQCLaRKESBbZQKMIMYMRIKUECTRUcMJUJiQEDZURTYMQMBhKRLABJcJRJDIAgUbTCTkghiUMSCgUZbMDgUJKMYQRbQEUTIRjTMIRQUcJRShSCEJCAIjhRZbYgcikkQMRIcYBCaQICRRJMZEEaRBiTUbckQJhbShbaBALTSISYabTCJAMgRLgUkcIhkJZRSKDaBijQkhCabcZCLQSYbRkRjZhbMKRLQhQSJCQESgMcEgIkQURLLcaaCLbMaRQIUhTJbcjYBAkKYabCjkkUMDACLkkcJSILACQTACZTLZTQQDRZkIcUMKUiJLCkkijaScLKZAiTJIAcSIAcLZBQSUhjhBcJEiSkgcaaMELTAMUABScMSBZjiYkKRCjJMhSgaSigJcYYgAYDiahIjAhQhhBbARMiCDIYiiMbKiKMjhcJEQMaSbiDYIDKTZZcUIDCMUcgiDjhDkIEEKKAUjiBkQDRiLIgUkLAbjIhkYahCJAaEciYZkBYKEbbSaTQiAjTCiKZJIAkhQELgaUUcESgUhhTKccYYRYYJAUAjYEkAhKLRZCkSZLaSijhMTYhEcBQTLBISZBCLJLIjaQCBbaCESjLajERBLIgQDCkhETCJCQKDITAJZRCBYYDAQUKJbhjiJMaTSEBAALJQMhMDSZACLBchDAUEILIEKLDQQYYQZRAjkDCYjbILAUbMDQTLTMkBgISLagYZRjIQhCLBYAShLSJTBCIILjCYCLBIICICMAcERhaahaLbCJbTiLBTbEKcURcTQAKSLIUDLZbTDRiUSZcQTBSYKRkSYSAKUhBbbaCTYIaQbRiCITDhkbYYDJQSiQIabCJLEbbYcJTIRQCIaYSbYJSCYIgaEaJMBAcLUbEDIEJYZJCJgQgJSARBYAKBRLZbALATbJbLICRgQBSIZMBBRCcDaTRjEILQJYCKYLKQBCbBQLaKKYLTbJLkkSTbEkAIbbhiQZSJACbKQKREkkhTSgYbaJQLDRgRZaRaTJQAADSbTQUaZUKLZCYbMRMJYCjLLaAKcaJTZQRRUISbiLKYAQJDBDZKTkZYTgbRJDMQRYSTKaDbBLAKQJQaSILIbZJSAZKbCbBTKCcSiUEgKcKYSbSRYRQCRiIbIETbDYCYSLJBaZCahJbacSSBIcaRSCDBRJKaJbCZSSYIKhJJaQSZEKRBTRAYLbYAQbSLCURABRJJYBKAICZaKjCTbABLIaKQZQJgYAKIKCAAbRJZKbIDUYSKRaQRQQRKLSTIKJTAcBTQDTIZRKTARQAQgJSZSKBJcTaYZIYUMQBDJbMDbLJgABBLZZKJbSaIRIAKYJYDIJiYKDacZARKcCAAAJCaCbKCASQQDKKLILUUJJRDDKaRBYbLACjQISYEaALIEBJYjbSQbTYSSIDbJLMRjRBaLDIZDKaQSKYQbIJSKRLDLYbaZAISQBCMZaCiCTTSYYBJLiMCKKLaLKSSMQIQKQLaSCYKIKIYZKbCQJYZZBSLhZCgSKYTJKSQUDTMLRJKCYJbYYBbSTQJQRLSBLYbIBQKMLDYRDRcKQAYRbTASDCQhjJBBaLQDSKRIcYYCIRQSbbDCZCBLZSDKDZTYIbADYSCjDRIQZBLAbKTaYBLZbRbQZLIKSaKAaIBYQZDQTJaQRaQDTKQSJAZLTRRbLLDYZTYaDaKCaYQQALYDiRDCDSSIQIUKLAJMQZSRJAcCKJCbLRRDabSTbZBRiEYLAYJLZCDBZSYDgDIDJJZaYSDSIBATCaUYaiYTBDISaZZiSJBTZaaYRUCQLRKKBTjBCLTIYbbCAZTbjAKSbCIbAYYLBLRZIIAYABQSLTTIKRaRIZILYjBCKJYSbCZJCJTQLICDIDBBSUYYYRTBaKbIJLTKJSBBaSSaBZYJJLgSZRSAYRJSBIgYLABLJBbBCKaYARLSIQTBDLTZCZIDJDJbRIRJCTLATSiKaSKIRSAIJgAIIIaJaAIYLASYRbZTSCZLcSCIQRbaBRbZQBKBKDJTbTBQASZbQALYLTJCbDCJBTaCAZjYbTKJZTQLbLTQIKJQYIKCRLQLJbDaAaKZSIaESQaSZDAZbgQARJAYJBaRBCALTLJKBjCITKIQDYaYZbJATIaZbQAQUKBZYABZDSIQaBZJIYBaJBDBCKLLKIZBQZDCACCJYCRCICQBJTKKJLBZTCMLLQJYDRLRbIDLbLCbaAASCRaSSYaaQCITIDTSZZKKibZDTZBSbBJQTLDCBQSQSLJKLbQQTIZTaKSRLTTZIYJDaZaLJaKZARBIBTSRDBAJSbALRYECSLSbJCRAKYAIaJTQIJLIQCCYLbbbLYKZICKKRZQQQKBTRYLZSZIbKQSaSBDCIZaKJBYCDZQDbYYIYALQaJBYTYITaaZYRRJITDSTYaCQDDBDaQITLSIITCJLLCbTSQQjLLJBDJaAbDRICZJBQRCCZYLRbaUATBaAARRLBTTCLZKSQTCSQcLBaSIDBYbhbDZIZLKQLCTAACIZLTRDYYCBLYbAbbAIQASJILSLJLZDLSCJbbLLZKQSDBRDRRJYCSQBSCSDaKbZAYCSCADTJIZKDJSTbLTKChTDZaKQYgbKSIDBATCSDATbbSQJLLLRbDIAYZRYaaAiYgITEaSSCSYSYDBbTBSCDYARDSJIBKJCIKZKaJSSILbAQSZBIZaBITCIZJDQYSYCZRaZDLQYTDTZQYRaDRCJJaaYCCJaRARJDJRJYYBIDRBIQCQCZaTaBBbYTCRKJAJRAhDcTQTRLDZCLRAIBYITKJIQKCRCbLYDCAZTBLRQQCZLSSDbJbKRbJKDRZKTRRBYCJIIKSJQABYZaAQYYbQBYTRYIaZRTKITTRaDRbSBRZTQISCKahDDIDaCLKKDRbYSZSADbZaQSYQJRJBITKbLSLZTZKTSCLQSSTZbKZLKDJSaJSbbbZKQBbaCDATSSgJJaIABSaLKZBBCYTRTQQRKAYDCCLKTQZYQZaBDbDRYYQAQIDQITDAKJSKRTRDLLTRIJQJQYIbAQIaJTBIQDITCLSLBRTCJSBYIQIbRQZIDRQRaAIZAIaTbBRJCLLTBIICRJJLRaUDaZITAbSCYZTAITSRRKJTaLSKcBIJKSKbRRaAJSBIZAKQKAKBBJYDRIRIDbCIDJAJLKRCTZKLKLCJDABaDYJDQKaICAQZRSKBAYIZQZBDZRKBQAYJRRLKbDYaADQaTKSaDDSaIKSJILQaTLJRaSYbYCIbCJCSQDQRLbAIZIiYTZZQACALKLbYTZZLQaDLSKATISaSZABBTTRJYKAaRRQBKRQDYKSYBLbJCabADSBTTZDbDRRZQIBYaAbbICIaIUQYSAKZbAAIRbJDJZCITDDAAQJaKJLBaZAaIYZbTTSZTCLabCaJbDRCCKBaTJbLALJQRACICDQLJZYCKLZQETSZKZKLQbSCRRQJCaQAIAbASbIbCTZBRUZCIDQLabYBBDDAbZRAZQaATZQCKIZaDRabBbIARYcJTQCCabCaLIZBAZIKISZJKYQQQRSKbIARSZbYBRZSIKQBKYIDJCJLbZQDAJKYQaRCBQBQSCJALIDDLSZIJJALSDSJQTIaBTBKZKBLYBaRCDbBYaBCTCKKbAQbIIJAaTALbCQSRTDCZZKbZaJQTABQJabSaRALJabTJJaaaIIDTIDLAQCRaSAYaRDZARYJDJQBKQJYZQRQRbIZIBATJQYRQJQDDJKDYSKSITZaJIKTKQJYaICJbbbBJSBCbCKSTSaAIKDBAZRRZJBSbTYTABACALJLaQRbSIaDIKCaKbKbCALJDATaSLTKaZKSRJSRLCSLITTCBIaKAKBJJJSaICKYZCLZLCDaKLLSbYQLCACBKaCaTQSKJJaCTLbQBDYRbJKKJTKQSKKbBTJSJBQICSDKTATYKRbSaLSIDDSAKbCRQYTJaaSKTRSIaIDLTJJKZaQYQTaLAbYDKBZRaBCRBYQYbYYaQTaZTCYBABLcBZQRDZLDbCRCCbbCJCJKTLCSDKCCbBLQLIBQISSYIaDZaDSbTaBZIgBaSLYKKLZRSKDKKDYQaRDbYTSRbbKBLYQALaDDZYDbDBLQaDbLCQYCDTCALaTbbaDIZTBYSSSQDBKDRZabZZaYTbJKTaSCDCKCAaILKhLBJQBDSAZLbYQSACLLRCIYLKDKCLKYaIChJTRBTDCSSQbQLZDZKRYCBJDJBRQAQJJTJCLKSTKLTaRAITYSIBaQbKLZBJIZSLTAaLTRCADZYLYRJaJDQABJLBLLRAQABDaCLRYIBAbbbSLYQABDJATLKCbDKZIKJaLKAADaDQBSLTRYIBLYKJSDCaKKRZZSDSCCTKBDBSRBQJZbIZDbIIISSCZJYABKRZQBAASBIJBRYbRKIITQZiaKbLYBQQRYZRbBaRIbDJIAZYaKDTLDKKaASbARTAQDCDbJBDBYZSLSCZSYADLRARLbRIaBQYLaQQaJADJZJLSJSCYYLCSTRAZbKTSDDJTTDLaQKYJDbaTYBACbYYabJKSabZaTZKKLIJDYKQbCBSRDCQALSaIAQJAJYbSQBRYbIbDKLIAMKBZICaJABTaBISDQKTCBJCIDJYaYKQAbRAbbTZaCQDDTTRJRSRICZBCCIYIbBCLIZZQBLLCCICTRSBDCYKTKZLIbRISbbQLYJYYJBCSDDIZKJRJQDBBaJbBAZSZCCBBIYBKYTISbTBCBQQQaRKAJaKBBTQbAbYASLLBaBTJbKABTYSAAbLZCaJCCQCYJJAaIBSTSBCRIaBLJRKCDRbTJTJbDADCbDDZDaIKCKBLICQTbKKBJTbYJQIISBaAYJTSBbITRBSCQLSJBbQDTCKRJIQSaABZbRJLSKQYJAbABARRRRbCBJJRRDJKSBBZQJbKTAKZRaDCLKTKBSIbLARYaaCCJaJBDAAKbbIbTQSbRZSCIDDBKYJILbSDAbARbYSKZLIQBYYYTKLSIabCaCCaJKLIQCaZIDZDJYZKITCJSCLLQKZTAQTbYIIbCZCKCaSCRbJKaAJYCIaKILSaZAJLDKCLIITBYCLZACZSABLCKJJYBRDaTZTLBIASRbJSKCDQBIILJYCSRAAQSSaaYDIJISZBKJAaYaBbTDIJaYbbYQDSSJQZbIIaZIbaBDRJKQITIZTAZTQCaSSRRbARSaTLSBDbIQCZTLLYKRDaYQQYTDYYLBbTKACLAaLaAYaYTBQYAIYLaBSBYLbKLSCRJYDBAaZQYSDZBCDTYQJSIIQBYASZRQIIADJSZTCBabKSBSZDDZLJYTBLYIKBQACaBYLIZAZYRLZRRJDBRYSSYDCbZCDZJDQZaAIbDBQJZCIAaYbJAARLCaDAZYSKbASSbLBYDZRKISbaYKZTSZCSRKRbSADZZYRISbYTCDLLaAYSIQYKRQLADCQJZTIJBJaKAQBaYJDBDBDYYZTYIKCKDTJZJbIJRJQQLTKKJZCYAAaaAYQSTCYBZDAAQRDbTbaLZYQAZbSCIaRIADJCICRJRRRJBRZSaTbQbZaYYRSBCQDICAKBQJIDBKQZLLQQTBLJbDQSJDCRaLYZbTKBSaTLABCQYAQDJbDZCDTIBSLRJKILZJIDZBZQLDCSDbQZLQSSJKSCYSBBIDSRZaAJLaTaQAZBZARAIZJTILCSbJSZKBIZKCRIAISACYBAYbQSKSLJSTDLDAYYZRJJIBCQIBDBaLbACYLALQTKCZRLbSRQaSRQBSZZCALZDLQLbaBLRAaaQDKLTDQCRZaSAbTaDISaKBDQDLTTBDbRbJDKDBRTJZIbDLKYSYZCASLCSDDRITZZCZCCaRLKKYAKICaKIJQJADKCJZIYSYJTKRSLRDATaCKQRbRDLKZZCBKJaAZICRRAJYLYZZACBILQBTLRBITYSbaSCSCYBZQJJCBRbCBAAIbITDDRKSKJTAbJBRKIJAZRLKSLZTaKKLKaCbBDKCKaTCTIAbTQRZBACbCSbACIKZbQLBKIRYBKKDTSDBTKTBLYZTJAISZTJQIJRLDZIIQIDRSLYIDZRITBATQDQaJJJYZCCCQaTYAYaKDZCIRAZKCLDQZJTCZaBBLaTTRTbYTQDCBbaJRKAKKAbZYSSYBbKCYbYQaabAJRYACaYDJaKaJTKBSCaQbZDaQDIKJAJCKbQIbJDLDTbCSSaIRJbKRRYYCbBbTRSKQSDDBLDCbDYRITQCYSCZTLJCZQCaDabLCKKAIZAKQIDCCaSKTRIYKbZKIBZbLaCLTSbKALSCSDBDaLIAZKbJDaCKJabaLZCQRASLKIDaIIQCBIJAbQRYaIYJRKDBJbAJTLBASQALYZYARbJTQSSYTSYJQTJbCYALCZRBTZILaBbCJSRSZbAQIDbJRbTaJRQZYCQBISBCDSQbbIAZbCCIbJbRQIKbbQJSYaRTTQIQRSRJYLIBJKbSQAbYTJAJSDZTbQbZJQDSADAICKYZLTCAALKBRSJaZQZZDLILKSQQTIYJbLYAaRAKRRbZbSRRTaAQBAJQbQZCQTYZQLAbYLASQTJZKJALIJBTTKQQaZRQBbRbBAAYATIYbYBbSCYQZaaBJCKBTCLbbaaSaTIaZZSaJSSARbKJDTRQLQaCbaJIZQBISZZAYDRQZSBZZAISCLLQCbaYBaSIRbBITIaJKISKaJBIADbICJJYbbAaKZDaKICBZCIYYbZIYAITTSRTJSLQIDAAbKaKDJACJTYTZKZBbCCISIYBTDDQTYRLIIKJJKSYYTCLDAZSTDJLAaSZaTZbBIYAQZCJAKKRRLTCKaQISABTIRJJDBZYJYSICBbQZJYLIKQTIbQBSTBaIaDLaJIKASDTBLZQDAIRIZSIDZCRYQaKBZDDYDRJQLBZSDJSDDLLAbaTSDbYSQRLTaBCYCaKZJYBBQAZZaaZLAZYDAYZBJKKQaYRDLSAIRSRQCRKB"},
Embed{78,249,"IQJTBIILKYSIIIQLAKTZQDIJIIRIIILIIAAYLJIRJCaaaaaSZKaYKYKCKSaaSZKaYaCZbBKaabKIYaJJBDYDKJaBYTaIDRZDRIIRQDICCRSBRDKbAaYKRaTABQAIbBITTaaJASYALYDZKaRIBLJDbKZSBTaaLBaRKDYBTSBJKTBISaDQTQZYKbBKTRaYbIDQaKYaASIbKDRBaJTICZRBIZCKALbRBJSQDAZZBYQKKDYaTACZBTSLRCbTTDIIAYKRKYZbQSBCbDJKaaZRYaLBTbZRZIKKJQBDTYRbQALbSKDKaCaRYQKLDTQAJTYBASaCZaLQDRbbTSJKBLKRZLSIAZCRTaAaBSLCSTSJYaIAJZJBQKZDQDLSQDBKZSYTbQLJYZBAALITDIaaRIDCTbRAIIKJBSZSaZAATIaZSbRLSSLKJaBBBDYTCBSaKRIZIAIKBbJAQRTDQTaLYKaBKZDRISSZIaAaASYZbIJDICRRKDKSCZDbSQZBLSLALLTZCYTCKbbaAbYSJLAALLSIIBYbCBaDADTRJTAALKSQIYQDSZaKDKLQbKAJIKJDTADRLKBTTCILJJAKYKKKKAKJRYQSZBZYSQADTbTILYSBbAZbaRTZKaCAADLZYIKIbDaYDLDCIYbYSIbSRYQBCTAaYZBSZKSbLDKCABRRCBZTJALTDIKSBZQDJRBIDQIKTQDIBBIBTIDRZDAJAbKSbYBDSaQIKCLAbLALaaKZCDTDAaaQRaTBDRBISAILIaJTICaIJQRZDDJQBZAJDZKAaBbLRRIKSCLBbBQZQSBKLbDKQDaDTDLCYRCILAZIQKTTYDaZTZBAKCJCZAKZKISJCATBZTLbRADDZYIaJBLCRaICDQQLLZZbZATRRKATAKZbDKBACDIADZTKaBDZIYAJKCLZAJZSACCYTbDaaQRLRIRaTITIKRKCJaCYZACDbBDLDaTbQYCbRSBBaLSAJKKJLTBaaIaCYTZKRIRaDSYBKLBSSSSJMZLCBSbAbARKTaIDICbbCDYZIJDaAaBJBDLJRAAZSRbCaYQCKASZKaIDBBRCaTSIaKKaRQZZZQSJZKATRCRYTADKJABLLYYYDIQQQLLbDTBIBDLSASTJQLQZSZBDKSJLbYLYBYDKLZICaabCLATSYKTBBDCRRKLKYZBBYIIARYIQBbRTZAIDSTLSQQbLZJaBDQaSZaCDYbbCDRBAZZDKRJSTRKbQaJSDKQDRDJCCBCaQATMBZAZbDbRZKDDLaCbaIISIcbSJaDQaLQSQRBSaLZIBDAJJSLRYYCLKbAQDQICJYDKZSIYYLaYRRibbLBKTBACYCLLIAAJSLDAADIaDbBDSIRKKbAKZATYJQaYQZTJRJYRIYKQSDTICTDZICTRRAbQLTJABSRYKJYbQBbAIKKZAYTIYLJBaRYBDKCJRJYIaDSSSDbDRIYITJASRZYLQaYYTTSJTSDBDKaAbUBRYBKIASQZJYSDbJDKRLYKJQATDJIYaSKTARIIJAKZQLQLBBaRTjDCTTKahKKSbBbaKCYIBYDRYQLAbBbIQjIAjbDRKJbCSCLURZJYICZCKbDJQTbYSaLIQAICTBDCLBLSLQTYZCJKAYZQZCTKIKUSCCJYbZLILICBSICBTIBkRbbYbRbQATSaBBhYDBAQSBbZILIYQLaYbUUCCIbCMJSIbSQZbBBbaTTRRKYYRSKYZIBEILSLSbCAJaIUKKRSDSaZSQTJZCDIKBRZgQbiAAbLRKLDUgaRZZDQRbMAcBMSDbTBCIYIbKTCJQTUKTSbZAYTIREBRTDaJaYTUTKRBUIBAITTCBYYDTaRcAZBJbYLATCAJSIJYbTYMBIKYCaDaSZaTKRaYZbRQiRQLJhbQAiDDaQAZZLcDJSRREhCaLaCZKRBaUgSKYIBTKDRIjUAKaTJBZbCCcDCDKEiRELJRDhbSCaiiKRKcJQRaJiARZbELCCZCEMbRYQcAARgSIbMZYJLaATAJYjbSQIYKJaBSABghYRBJCZIEaUSDBaKMLAbJQEZDhRZCQIKYBCBbBAKLYABbQZKMJZLYghScEaaMiZTcaQTJJQZZCibZaQDRQjJcgSRTKbkTLbRQEbJRSBhCIKSJQLCJBTYMSAJZjMKaZMgCDChAULTaSSMiaRQbTDUYRgbJSaJgacAUbTICSaJcJhDDiiaIQCRSRMYBCQYhcAKRTbLZaTDRQJSKchCLjQackACITIaShjQSbbKhKgCaRTLSMDRBMRiAccCKZhQJSJRbDIZhQiAaCCJZIaKiBJhUYYijaYEZZLZADJIUUhcDcEhMRDKJTSSCDgiKLDgLIhiAZQcLBRAiSZkhbDRZQMTkcKjkUKSRCCQbCkScLbKJZDiBJbYaDTChiCkMTYCIhYTJAijLLbgBCKTiAkIDTZiUKcYkCSZiRKKMDBSJIBgbRSDKYMJTABkBKjIghQIKcicjJBJTCkhMiDbgATEIUbQcDZbUaATjEKTIaKAIIikgkbaZhIhAbJMMbQbRigcMDCgjYKaACDUIkSDBLTiCTZYZjiIUMbDagDMIQDTIQccjQMaLUajEaKBAgIIJELUDDKaAkKCUAkJDILMAKLbDaDDKJLSUCQgQDicKEkUhcSZgUEUJAABZTLKTCYUQTTRcbULUIBhUkQhjLTZcALZIMTYBQQJihJaJiYUDIIUEEcJaCiDbkUQhDJjADLQBYZjTYQBcYBZkCcihikRQgZZhBjTCghUAbJghbLYATaAMBShiCkSbDTAJMBLEgZYRgEYkDYYDQYjakQbKIkCEQAATbKBihgQJUaUMBDYMKDjjYZRhgTSZDDTbELBZScgKbAISSkTbIKSiCDgjjEgkZghKbcETKITShTRJiYAIcaKLiSjLkikaaZAkkjTIQcCYBZgJKJTEYAEEDhBLkSDDggkKjCIZkBIUbAQhcacKSQUZIhIjbTTDMcKAgkDQhIYbEhQCcYkEIggjAYaMhhbUjKChEgJMSDBJEgQijZgSKEjkgKRkZcLkIQDDMkZLQLMgJSEAgUDBjZRDiYhcTcibaURcQZQhQBQgkhUITIjaAhZJDZBiaYZECLSADZCEhKMIbQTTiaQYLZIYUURQSRiaJYAiLkbgIEhZQTTThLjTYMJQhKSLbJJCZLaSRDCERkTIaBJAQIQUkCTSbKgYLSIYLAAZjRBagLTAhLCSZBhDkAKhMaLSUBaMUaSjBACALQbSSKACZTchTQRhBRJZIjRbBQJkiAMjMZMZZKYJDBbTBQQaAgTCDQJYgCiCMDDJiYgaLYKBQiijMALhbRRYTRAATQJQiCKEbTcBZMJLQKaYTMZMQJChSCQARSaQBLZCAaTYLabhSMjJAMjIjTSaMbAIIbCQIACMLSScAjLDEEcchabbBTQLMZSKJQYRCILiBSMbLCRQSRIRRBRBEMajbRIaIbUKQcJcKRATaMLhTRSMSYCBaQCCBabQCIaSDLZUJaAQLZJaaJhRbSiKaTQKgbaaSUCZZkJMTJJTCJTkBYJIIQYTSTSLCSJSCbRZLYgATgaQLBbjEQbTgKKLgUZRCMgbQDaJBaJCDaYJjRiiDUTCSLAYYQTYJJLSIMCEhZTbQbJTYIbYRQILJbJYBEALhRBibjDRBSUSLCIcSSaSZCjLDQQZbUYAZKbBYgcLBYCTBBaSDSASLbAZCLARjRRDBQMTKACSYSiSQCLLhBZBBZIDBLbIbLLYiABBASCabRhZSKRaZJZTRYDACRSCZIiJKREJbZYICSUDJSRALaZJBYTTRaICKaKJYaYbQSMCLbRbQCcQTAaaRJTCCbDLIKLBYbCDLcRYQIATKBDQgYUaLQBQaCIIBJBCRCbMQAbQaIEITaSBLIQDJJKLIkJbICDYIYBEjCJSgCTSDiYJaRjSbKYDTZYTJbSZTjCJQCYaLYDTUaaSKLYLLQRSSBZQjAaQDREAaLRbBKcSQLALjaTCRBLKaLQjJABTAcKRcIBQYTBBIAcIYhCZKZDCKLAaZYQCSibDBYKaZREBDIbaEDjAAZMCaSQBcRDDMKZLZLZRASUTAQQAjJBTJbCCJSQBSbBCYLBCYBJEIAJYKSTbTSCZCCQUQDUjbgQbZLLQCYKbSAZLIZAbLDKJUQZYcDCZYTDKbJRiRBKQDIaKAESKaAISLRZBALMbCJKRaCALADgDRBQcZTLJQSSSLYRZCLLTDCTJATCJZKbIIYAZTILCQKRASTaSICTkCJLJITCKYAbCaZRIaTYUJYkBKKAQSZaIaAKCRYbACaTaBZJYDRERTCUDRSZabZJLJRSRhACBRRbYZQQIACQLRKiYhLAJZIaABaZKAESgcCMRYLTJALYZhaCDRAAITYDhJbAZJJAIADSBKDajTZhKRYBTYbQQYZYBZCBacLSJYKLCbRIBaASaRTLQYiRSYQBQAAbJQYAgSJYSDIRZZCSIQZZRCbDDZcKLCRYLACZjKQRaQZbQKCCAQDTTKTKBbgIZQKJRYSICQIbbLIjbKaICgYQTaAjaUTSRKLbQCTbIiLaJLJSYQCKYRLMZARaaDKYCKJDACbRDLTKLRSLQkSbBDSCIJBbYIJLKJLAMRaQbYCBQgSbSTLYaRDITZLUASZLCaYDKYDZbDaJbSZgRALSbSjQQYaaaRjDQjbTBDCDaSIJLYCbTTZYSIYTACQLJJDCSSBCbIaDRJaKZZIbCLCZQKARTbZITBQMZILQQJCSAbajBZaTaZBQIKKcLABbCKTbLJJQZRYYCCDYQhBSAQRZDJaAjQYaSARJLJLIAIbSbIJJaCARaJQcCIBIJCbYDJDKBQIaAYbSTICRBDJYYCUIRTCRRKKBLBbBIJABTADDaCCRCBSJYKDJhYKCBSDBAIRAbABIBSKTZbBaYJRbRRbZCTCRSAYLbJRYTCDDaSbLJQJbIDQQZIRKLRLbEaSSISIaYZSDgaCZICQZBgKRLJCIKZCQTQZRRLJJSYIDKUaKLaJYbZDTAYJBDDJCQKRJDTCAAQJJIBQKjDIYAILQLTTaKDaTaYTSYLLaRTQILBCCDJBRBJaTDSTSYBkMICJIKSQQAaYbJLIRaIDYAaIJRSCTLSBJaBaabaTSZBAARAJIZIBLILLTYCJYRQAIDaKILbZIKiSTIaCCQRLBAZSIBYJYbRKBbKIBCCZLTaDKIaDDBCKIabBbTJAaaQgIRDBLLTIYYKDJZLBKCSCQQDQaARRZjCJJQTITBTRACKLbLKRKTYCZIJBJSRIDCSbIKDKIaARbTZbBAACYLZLRYAKJTSZTSbgILCbCRKSIKJQCCAKJJTLLIBYKSQaTJUSYURbRLCCbSTKLJQBTYTSTDbSLSDZDRALiJYKAJCJBJZDKTYYYCYJCCBaSbACCaCQIJIKaYRBKABDLbKabKbIbaiIBaRZRSJSITTYQKCbaBTQDbBiLbbJCTSBYRRTaZDKBRLSSDQKZZSKaIDISJBLZABBKRYLITaZZDaIZaCbTCJCbZIRTUBJQKCTZZZTAbChLaQAQbICaLRAIZYbcIILRbABSBJZSQLBILKBbYTRLARYJSaSaAJbSTTKYASaYLaSQZAQQZbCQIYILJIBJZDaYISBKQRZYTJKBQQYbYbaSLJRRZTbQbaKSJCISaBJALSQCDRaZTIbZaJTZCSIbRZIBBbAYSCZCZbCRCRIKKSgDLDKJRRKBDbJaTBJRIDaQBDIaZAAZEASZBKYaYSIaKhJaSAIBJKJCRKAZJbbaRDQZCaKAJTJaAKYBMDDRQBSBQCBKDKJSDBKDaRILQQIZLBTLIILYSZQBbRCaJJKQDZDQQTSSQbLDAAbJJaIBRIaRYLITaTRAYIYYLJRACZJARbAIZTKbbSKQLYYJTaZgaSDbAASADbKaDTSRZTBCYKYDTCQbQRTLRTabIKRAKKBJbCCIYbCMbBAYSTCaDCRDJTCZTLaQTTDQJCIRZYbQTRDAJSATSYBLbTTTaDJYZYBbCIQITRTbCQKaYJZaLCQASLbAbaQDIAQALRJYRJYABbSZKaaRTBRbQRRJCIDCaDYADBbbJDTaDIaCAKBbZDMYcIRDQSCRbYAbBjYCEIBAKAZKaJKJRDALYYTQQCABKQZJabBTaYZbLYCASbZKIKARIbIBSRKQRAbTDDDADARJIAYIBbZZAZCRRSQDBDCbYZbRJTZaZTLZbIJJTQTAQQKRDDSaYbQQIJCLLYYKIITCYYZKDLBCLBZJaAQSZRTJIDAYRBZAbAKBabZRAKaYQIAYJYaRLATAZbZKSCYDRSLZZKaKSBRJaMCTIQYbCBZIAQQJKAQIRLBZBbBaKZILZDIRKRZLQTIaSaAJZaITBKISQJILKAaCTKTLaYADDCaBRaQTAATJBABJBYQaYZCbbBJILKRaTLACCJaQQKAIJIaBabDSDTKZAbQLZaDYbJaaCbQLBAYZbCJYbJQSJCQTMLDDQZISDCaAbIaDSaZKCAbTQDBBDCTKBLYTRKYYDJaISILbZYQCYDJSTKZAJYYZYIabYTITaYJDRSLKRQDYQZZZCRSSILAQBbZAbBCaSCIJJYJLSRQZLJACCTQBKKbYQYTBAJLRQKRbBKRaCCZQIbKLTIRQaIRLKAZLCAJBDIKQbYITITDADSZQABQKRKDbCLTKBaaQICRIJSJLTJIZQaJbKARYIaBYBDRQSYbZaaRCDTSbRTIRYYYaQQJRJCZRZSLSZKTYIYAAAZQbbaYJRZaLaYLJKCZCCRSRKRTaSIJQIDbDTMYTISJBCBBQaALaKCIDaRCbBZbZJJQLKDQBAKYDSZZKCIQjKRKYTZAIabLJCJQYDRQRYaRQYLSZTLZCYDKIAaAKSZQaKSSaaRLaTZCJBJKLZBIaTTJKYLACQJCTaaYJQAKTLTKQbaDJSSAITaJhJIADaYDASRLAbLJZQCTRRBaZRALabBJIQbDTQQCCKALZSBTIYYJIJRLLCKaBDSKQJJCYRARIYSIaKaQQZbJaIJLRCbIDLJSQTSZSZQTIBITaSTLCKBaKQBYaLAIAIKYZJDAZYQbbKKTRabQYbIYSbYDYJCCKDCSaRZBBRDQTQKZTYZLJbRRIABbbDCabQQDQKbLRKaZYSCAJDbBKbJLTJLDKAaBSaKYCYYaZACCIBRLRaaIJYSLSKRCJKRKbCYLbLMBCLKZaCaBLKCAaCSaQKRZZTSBbQBTYRZYSQJbICAYAYJTQDTCDLbJSDAKIKBZLCALTLYaDLQIQAJRbZICbJIbZQbSCaRQITSKLSZKBRbaIAaBYICLSATRLILASbSJTKBLSQKRQYKLDYKQQaJBSSbRYTYJRYDITaBDDLKDTbYKRDDKKASSAYASCZDRJQLRDIDaIATLTAJbYYZYDJCRbbLADZZKBQKDDQBbSCBZbSZDaCYALDLLYDKKaSBYQQCZQBLCRZBLJCSSBZYLDAKIDDYIZbLRaZQDJSaCIKIRKYSZTBDbbSRBAbaQKSAabbCDQaaSZSbTYbIALaIaILQTbITIDDAKZBBACSYZTYLYDQALaSTaYLaJBaDDSTTQDRBYADbKSRRbJCbTBKaRCLDKZSSLTKCCDDQZJJabbADLYTQATbaBaKQBAaSaAbTSRZISLZbBbZTbBKIQaaISQYACCQIYLZAZTLYbATSDAARSQBaZbKZCYYKQDQRDZILZSRZITDTBYTLDKRBJRQITLIbCSZIDaBYJTTbSASbTADTbTYATDTKBRaKAKYaQaIJAYLRZSKQIYTQDYJCCLLBYBCBJDDSZJRbAQZKAKLDKCQKZBLKbQKLJDSZbCRDDDRABCBaQbAIaDIARTZIQQaaJDTSYbaDCBaJCYaaQJLLCDZAZBSYBTCKRaKTBTQSDRJaQCaRIJZZJSZaQLaIJKJBCYCCJbaKLDKJQbYYCQaSYbQAaRSLCBCDYZQLBBDLIYCKBCDKJKTICbQJKTbJbQTLSaBbKSAABQLYJSYJKJCIZaJZZBBQTIQACIDDDQZbQYJYLAKQaZJLADTIaCJDDABLAKCCBLRIaZBDCZDQAKRaaYJSKRZbYaKTbLYSDaSACZTCLCJAQKQSLbSLZRRYDYJTaIRLZBZBKCLASYDLbSaJALaSDIIQKCbZASKLbDBIZKAIQDIJTZYaDZCRZQJABBYAKJJASTTZLQYLDZAKDIQbQKKJBLCZQbCTbKYIZAYAABSDYZbAaRBbICCQaRABYZIbBBKSBBZABJBLSCLSIIYLRAYKBDIYRbTCBKLaCCLTBJAYKDRZabISaJDSYSZQKILYSTYJSQRaSIJJBZYaIaTILZQBJARAYKaaQLKYARAAQKbSbCaTCIbaLQYYTKIJDASZKIIDLTKTAIAKaYYbRZTRSRRRLRDTYARDSTbJILAJATRaSSQSLCabbBKILLYBAQaJaRSDJCDYSCILIQCKYZQJCDDSYIZJYYbDZLJDDIBBTASLKBQKLDQIIQASSRCALZZbLKYJBZADRKYAZAbTSRISTRYYKSILZTRLKLAIKQDSbTIRLRIaKQAQYBSJBDTBCYARYQLZAbZLBTDJRLBTKBYSYbZYTKTJJQDRYKASRSbCLTLbDaYLbJbYAIQICRQCTLRDIZJZQbYAACZYIYTaATRRIDBBTYTRQAAKLLIDZICSASLRbRTIbDSbbbbSDCQCbLJRBKTbZDITAKJQAIATTIIZCTIIYJBbIKLTLJKCKIACAIJIZQYJKQIYKKSIDTKAZYLYSRYZILISBQKQJQALbLRSaJIKYBZTCSbbLQZbTTLaILaaKaJSIYaaQICSJIQTYQAZSDbDKTZAJIJDADBbCKKaKIBQBDBYDSaBaJAJYCRZDJIKCBJKbLJbCBSZSZBIKSIBTRBKKDaYLDALYaYSSQRAaQYRDDLKDYbTBBACBALJDKSZYSZbSZSbATZZLBSKYRbLAZCCYRQKSbQTBAQKALJZJTTKTSbCLQbYRbSJLDSLZRIZQLSIBaTSAbQISDYYCTDRBKCIJZKaZJCRBLQbYDRTCDYaDaKZYaABTDBALZLDRCAaZbJABCRQRYJSLJLDYKBKaIYTZRDJCDDSTICBCKJCAJaCIBaYKZQKDSZCQIbBYYbCbQZLBZbYKLTCCCAYbSQYTBbTZTLRZSQTZYCCJCCQJKSIBBaIDIZRLSZBaLLLTRQCIYQICZRLaQQaZSLaIaBCJYaZTQBDISKATCIZAATDJLYSKTRZaDSJABbDYJBJJRIJaASZCIYKRTTIZISBaSKBSRBYTbRLYbaAJKAaJTJBaQSYSZZSaSaTLbLSLZSDRaADASDTCJBJbDARZARLCZCITbSbJBDSIADAKAbLSCDBYBJTKDbABZLRLIIBDQBRBALSYZYDYLIAbbZTABQBIBCaabaQDITTaZIYYRCbZRCQBYIZLbYIILKAJTYTaCSTACYKJabZAQRSTCIBBYIbBIYJIaTCTALaRSZbTKSQJSDbJSYDKZQTZQDJTbQTTQQCaJaLaRbABKKTbDQSYJKYbCLKKCAQbITTbZbATKKRDLbRIYAaRQSDJQKJLKaITJSBYRICQBLTIKQATKIaZaZQBJSTCRTSDJKYBJSZLBaQCBCTCITTYQaJbSRTIDYDYLITZTRQKDQZZbRSbJJSRYAIZBSZKLRAJJRZDYZTTCIZYYQBIKYLCKKCSSCbTYBQAbSYRRSCYSAYaBDYZbIbIaZKTBZTLKYKIABZLBLRaIATQSYSISZBKRBKRZJDKZDRYRZAARLJLIbIRSCZTTLaBKRBCSRCADSabRRbCKACJZaBZa"},
Embed{80,256,"YbJYCIIIQILJQQJAAQIAJYLCIBAILJJIQJILIIIaBbaSaaaLaaaAaaYBKYbbCZaAYSaTKaaSSZKaaZYaASZaYDCTaRALIDZaILRZSSJCIZDRZKRDISRZASBJTQIBJKZYIRTaSbIBbDCYKCTRaCLAKRSCbbRBIbaBIACLJKZRYADYbKCTabICbBRDKQYCJSBDIJTIbQRSaBTISAKBLYQaKIbSLBBKRZRYbDTTLCBZQDACJKZQaIDTSRJSaDBKLZLAQLbbSIISRCACTYJSBbDSLBLIIabBCRAaBDZTJKKSZDJRIaSRAZQTZATRCZaDYIbCKKQKYYTAJCDYAYYZSRTKZKKSKJKJYTbQbDIRbKLCbJBYJbRLLSRJZJaCIRSAALBBSaZRQBLACBaQaTCQYDYKDTabDBQZJJACZSDJIaICIDaKSTKbZYAIDYQILSZBKaKABRAJLSaLQTbQQBLITIYbKRALBTQSaKYLQYTZBYSZYRKaACZDJALbLLIZBIATCKBZBBCRDTSCKRKaaBTLJRYBBTISBbDATLBSLSYaDQBZabAIQRKaZRbYQLCDSLDaKSaBRbIKYKLKDSSZSYCAQYLLRTaBKJBBDJaaaKBRLbASARQZZbSIDDKICLBaIRSQYBaRYBCBZYLZSTSJYLJbSaaBYJCbZRARKIQSYAQTQZbIKKZIRKIQBLaYCbIJIQYJLAZYBbYSTSRSJAJbATCQYTIBTbYRZKYbKBBZDSaCaDJBBCCabABISDbSYaRaKRaDDaBBTZSYATLCKDQDSaIZDAIZZBKLaRJbKBTQbQYTKYDRBLJSSQDCaRLaLYADDYZRAKRTbTQYRaaZYDLSRZBAIRZTTTZaYbZIDaTALTBJITASSCKaTDRAaYIDYQSZLDaCJSZKAJKDaQbIIQaJRRBAJTRDbZAAYIZZQKRBDaACBTbTYaDCDQbTJDITaDRCDaZISIbbLSLYJSaaSaYQSZLABCDDZRaSKYAJbKATCCIAJDZLRLLIaJaZBCKBTAbLILQSKaIbCSDZYQTCIYAAaDRabaaLKBIaDSJIQaaRaQCJCKZZLAKbQKDDAKDJQZRAKCKALDKBCDbKQSYZBKKKBTADSALDJLIaaZaaATKKYTAAZJACaCbaAbLbJJKQSIJKaTZQCSDLSIZYCJYLKKBaICLZaAbLSITaLZCRIYbTDIAaaZKYQbKSTYSTKTRRbAKABiJQZACQLKKIZJTBAbABBRRACYLZLZYIYaDYISbIBRBDDYCbDRDISLTDaQSDZAaTJRCbJBCYLDLaCYZJUTITQLQQIJaJBARKCQSTKYbDZLZKCCbQIJSRaDSQaDRSRZSLbJQAKCKZLATLKYZKaBLYBIBDKYKAbbISQJDIZITSZRDLATYCBTDZKaRDKKASKIDBILIKDALCYaUbKBRLYZJDKJDQSYKDSIQJLYQMKQSAbLYCATRaRaKCYTKDJKAIYDTATKDSLiQZThBLbaJRIZYLaSZBKSaLaSJSQBSIEDILbcJAbIYQJaDIDLYSLRTTSQIMJKCbBATCJabZAKTbSQLMSLAKRJRQYYBaaaISTLTTQSRYBTSTLTZaKRYLQIIIbIbZQISADDLIiDbQCSDbISTKLCIJKRBTRTEbBaCJJRRIYLbYDAAZIYBRJBCbIaKbRQYTTKaTQSIQZbRKTaLDYDILaBKZKQDKILKJYbJDCCIISBTAAYZBZDKRYZAcaIBBYAMLYRZSDKgIYBTgACJSJYaKYYaQaaBbBhbCbTDARKBIaQAELIJYTRYBICiAALALJKCJZZTBSacaSLKYbKTbAjTSLQDRTJIIBaZLCbDBcgYYTDZAaTaBCJbLICaSaDbKJSKQEgaZYQEQRBYYaQCLSSjTaBJJDALJQhTBAICLCLIbCZLLCRCQMZbTBSCJLAZKZUQYTBYBJSZAYZTDZBLDQKUCYQSZIakRRYKBASLDAJCTJCcALSKbTEYCZKZRLhBSSKSILaabRKJgjRIJDDTYDRALgAEKLLaAKiSBbMDjTEAZDbkQKDSJYjYBQKJARiBRTCLIAJaZgDJTAMJATCRYRDCiThSAahIhSBDRTLIUBZgUAYTCgLZYLYLYSRhKBDASABhiDKbRRIZkZCTakBAIQDQiShRCDRLAaLCLTALBUYQIJYDLbJhDTJMQgLCkSDLbSkRJQSDLYMETcTATYYIRJIbBTTDaKSaZhaYRaZESJQZKCLDRSTYbMQgATJMMgLMKZYREcYhQJKQCaBLTZIhYZbRQDcBCTkSRhSbDZJaYQKQJUaMCCUbiIaJSiaQRDARDbbcARZRcREZcRUDUBkAIbIIkLcSACRJDbAbCYgKZMKCEZKTADgKaBaMiJCDAhkSjADJYQZUAKMkSMQISbUDSiAaJAgKZKcCZYSgZZYabMSCLSjbRYRCYTRSYabjjkhaEBJZECbKCYJMgETQYTAhEABikTZIaLZaiLQQELjBASJbLIgCUAkLSLhLAiCMTSaYRTSEESLLcbLAKKiBSbkLLaKBbQckcADDMIhKKILEYRTAEhALQIUTACIBgMRJRBkLMJJUUjLSDShbAAJJaUQYDjRRCRIDZicIbAhUBLKhKESjIgShQYkMDRbIbLKKiEgCiYDIIhkRMAIUTjjUIhMDTCDJaUESiULaRDRhLZASMCYZKSEbgjUhLkAAICccBJKihZSMZigbQMJaMTQhYRhDYYIRCSkQaZiSBDZSbMUcLTaMiQIMgEDMLKMSALccaKSaQJKUhJIjBZjcTCMgMECUkTckAZBAjSMBZaSJUQKTKLaTUcQaLIQbcJMZQkkEgUjSbbCUhDaDAjBIiJDYISQiIRMCEKbCgLEUYaJjIQLDDKaDBBjYbbiKiJQCIZhEQKQMbTcQkgikQEUkjSiZjhbBjZAYSZEQLUBkLTIUAMCjkIaUYQQDjkIgSCAZgYbDSAQEjcQgYJSaDMkhZJZcYCaKEcRjEJhcgSabIaATcjMSKIBCMDcLKjhKJCUCgUhQUMDkiiJjRKjYZMJZZEYQUASDLcUjacEhKkbDURQKYBkMUIBQUkCJDgQJLBKZgcBKhiIJSKjLCkDTYCgMjMhaDjUDTCRKMbCQgJYJZBTUhQiQhgkbjUUjDhCKBBJgQjQahAEBYDJjcCKghEaIaALcMigcBCjabcKTYDDDaLZUEDbZZIhhEkZUcSYYQSIJRaLDaQDBBhjBIKUkiUcAcUhaISThYkAYkRKbTMbZKEgkDBhaDiZRbIQJBMUAMAQQAITLYYLCTZZJTaEIJJBJEAATJUhLiDQhcCiMYARAjSaKYbRCSBQaajCTRLTLAUCZDagJYkIKLhZZRAYYMRYbjaLcEcALjTgTgQSDaCBQbgTacaCQKbKZRIAbZJLQILZATYLIMYISagTCAAaUKikEkEKLhjEYYTZTDDUcSITYEQTYTaJRkKTEBJhCZDBKBRIQYaIkSQJICJYSZCAAMTLSUgDIREZBCRbDLATYRTcTDRMjbSJRKBCLiJBhjMibBaCCCYQQiADIKEYAbRBbZSbRJCRJBKLSDSYbRiIbMbkBLYTRKYgaQDLLRgbCbRQRjIjCciJKiIbBIbTZLTLRLJCBhSaJKEAYTARCDQYAJCaYZSkYTJCTRSiaTTLYQZZcYicbKiZSBJDbKQCbjgQjLBRRLYBLaLAAYjAQaabRIDTKLkigSDSIaIaTcUCKAYJSBJJLLAiUBYIBTLMSURBQQJJaRDARgJbADKRaDaYbjAAcgTJZSBiDZIJAJQLRkIJbYTQDZKUaBgAQbBIAQTKKAJaLhLJKSBKRUTECZLKaKZYCRScYZQTbKKZLaRYLCKAacYZLbRBIThDTaSEYhCCRTSKEKACZLbRKBLCZUAJhALDbSbAASRRTLQKMKabRTZAIcCYAjBDbURZBZRAYEEaSZaDJILYCTKRcjCjRARTKBYaibQCCKhgRcKaDgaYZjKRAaZRZbYTLARgLAaDKJgMBDIYgJgSJRKJQZBQZKZSLQTZIBABaJIKDCKBRCCZbTCKiUYhYCQaSBaZQaSRDJTJbIIbQDKTaTLJQKQZhcbKTIBKJYTQcDDRJBMhSibSTQIDQAaIRKDSBYQALZabZARQADTcTASabQKLaLDiJJTQKCQZZbbISUabLbJUBSIjQDCBiCIAbRYZRYRSQAYSLYMZgBAJEbQSRQJJKLJYEDAIhTBiBDCaTLQhBbKCRSZZAaDJIQaTLASBYTZaEKAJIYjLKBbSgDbIQRQQgAZJCaILBLUZZUDYJbjSDAQRCAZJIbbQbIYARUbKZRgAARYbabKZSLKLAKQTCISCBZKZQTLAEUAbQLTDQZUJTacRLbIDTRBTQTTIZRQJgBADRJAbbYKKIcJAKgQTRKJSDZKZCJQJDBSJZJQZYShIYDCRjQSCBQYSBTbQIhZKTKSDAJaISDSaSJTJaICBcYIJDSBaQbDaLDJSJAAYZAQSYJIDYQJQUIjRIBCbATJYBIYLAQYbBKDKaQZZRaKIKBBSLZTbSMYCQcUBZCEARCSJBKQZJBAKabDbBYDLZRYIRQIBbKKaIAABbRQaSbSITLTQSbbYKQQBJTQIiQIQbDTZaRIDDBbTILYMTYRBKLZAACYJbCRQMKSDSaZDALbLJYQIaQYRbYICLLQBKBTLQIUaaQBKSLJTKLLJDbcRTCIYZSBKIRDDQaKLKBbYIJQbRDQYQAIQJIBKBASYIIKhCbTZSIYJIbIBBRALRLBJSAaAEaKYQCQbZDLSIJBbIBICLYYQRDYJhbRZCShbQDCSDDASAYZCRSBBZZABBSKILJDDRbYaQaSbRAaKYYKYIDLiYKZbaYASKTYYKCTbRBaSTYDIICTDJhhCMALIKbaISKYZEZZJCQILSDCZCaTaQYBIhYBRLICYQZKgDJSTBIISABTZTQLbKAQaJgaIDBZYYKKYbZRAAabRQRIbaYRKBQbKLLILaYBCSJRaDBQQCSCjKCKBDDZbJLITSDTZBKBaDRJKBBQbDKICJbKLQADJAaaTYJALaSKJcTLDQaISZbDISaCYKaLKbaYCBSJACbBRSaCIjTQbKQZRJTCTRLYYTQbbRSTLZQLaUaACCaTDbjaBBJRAbTQCbKSaJKALBRUbACRLCAIJaLZAbYaaQBDcRTJMLLaDRSSZQIQKADTQRZTQRRYDbZaKYKLQJIYAKILRSZQRaBBQbIZDKAYaEYDKKYAJAhIQIZKTLRRYIaQTICQYAbQBRDRBJDQSRJBLKLQRbATaUTZbLJaIRbISZYASTTYIaQTYRRYRJTYbJKIYhBSLKZCALCIbLKKJDaLZITSBIibBDDAAcSTCDIbQcDBLLACDRBbYZRgERAKBJILZKaTCAYQKKILYAKSIYIYCBLIaTLKIZQLRIICSZDTbbDbaCJCAKcQSaSBZTQABTKKLQSJZJbMKIbaCYIIZbCZICYKLLTAZQbCBAiAYaIDQDDDTBBCKLDDbSCLDQJAcJLDaLYCaQAYYICBCILYIbSZbBKSCbZJBQDCTZCIBJIYAARKIBZCLZSTBZILbASSAbIASIhIQLKaaTTLbZRLYKYTASJTQaTTJKDQTYIYTDjaaJSDCaJQRICTJcJBZTLLYRBQaIQYIQCRCLACKAJcDZDIDTYDDbZBYTDJCQTBCJiTKSLBLbDJCACYCCSaRCJALTIQBYQaIDSDaLCZLJZZbTTYBAQKaBIZDAZIITRaKBLZLDKIRTDJYJQaYaTQTSCLAbDSQTSAZKDQJISRISSDbabLKIIKYYRJDKUYTJRYaYYYZQCTAARSBARaQbIDSbbQTTLSKDLICTCRQZRCKDJCZLBbTbKKDCLTCaQZDQRQBIZbLZRTLaJASZLRBYbQSSTTcLSACZQZRSabbADKDQRSRbAhKYBYKCTLLYYJLLTSTTBYBTTAbZbCCKbKbKJKTTADQQbKTQKCSJITJDITbYLSbbYCCIRbbZJKZJBRKASLSCbJZRDCQUDQSRYbCYYBIRTSbICaJTAYJTICRIMAZJTbSQAaBLZJBIbDAIYCZaZRaDIKLLRZBASSLRSYKKSDLLTZaKIDSAYCKKIKDTJjYYJJAZIKbQaQgRLQBQRTYaTKABCbQYLCDTbBKQJYJaDTALBaZBDIBabLLLRSICaKCSIaSSYAbZKSaAATRCYBIbIbAKBRZLBQDDJJDIJCSLKJDKKCYRKQZLLAZAKDDTjCBJabJaaQZTaSZIBIIZSCZSLYSJRIAKAYJLKZDICRRKTKCaZaJBQJZJDAACYIBaKBDZDQQJZJAQYAJITBBDAaZCIRBbBKBSLBABJbDRQTLAJaCYTZCCKgYTBKbbBATYbbRSTDARZYDRZRbBJRaCQATIJCZaSaSIYbASBDKJDIAZTSJSJZLbYaQZJJSbQahJZADYYBLAKbaRTYQKICIDLJCZSYZSbbaSDRRKRAZTRbCBZSLYQCIaQbIIKSBKTJKaISTCIRJbBIYZDSTaTYILSJLKBbJaYbATQCAYKaYSZabTSTZKRCLCYDbaILTAIIKAKTZTJDAbIZQLZTJQESCDQATITTaZILTQTKBCJTLIaMDZQDZKSIQICYCbICZKCbCISLDQAbKYBJQQDKKIbSBSQLRJJSIKIJbYBYTTCCSCbZbCRQCKBTJTJLCYAKaDYQLQaTYZJLCLZSALTBQYCRTZCbJIQRRLUDAAACZaAAITQJZTLBZaLJZBZISTRCCCLYbIDYSILJRZbAbQYQDYQATLBaLTQLTDZSAbQKQaSIAJKALSYIIDZDBASCKIRQQSbbZILKRaTQbBSAZZJYaLKYLDRbJaKRDRSTaADZRQRCCYbAbbISTSbSLICLaSTYKYQSZILLCCaZYDADILDTYZQATLBYLRKAKTTQKDbLJJTQACZRACBBJKZACIBTRYSJbDZTZIARCCLCZaASDJKRZbSDbRYKRRBYSaDSAIYLBBaTIRKbDbZIIbDJTYADJQCCYSKQBBQbDQLRBBCZKYDIZaKKLZIYLTYLASIDRKBaIBRBIADCIaQSbTbILBDLRaBDAJKISIJBDRLQIDRICIZbSbTCYQDLbbABYDCJQSbKTbQQYSLCQabJQASYbCKTQQBKaZSIRLBBLTbaLBIQJBbaBDKaaTAaIKCLZTCJJbaaAQCIDaaJTSRaRATBRZZaYQCYbBAJaDBIQJQaAKCRCbJQZJTaITJRSTLLDBKKbRRIZBSLYBDACKbRCDZZLCCJCCRRJTaDAbAQILJTBBACRQRSKQZZYTYbLQbBRjTCbJTTQaYSYYBCJbYZZCbZIYZDLDYBRZZCUJCCaSTYaDLRSaaQDaaKQCJJCALDBJCJJIDTYJASBJLJBYSQJbAQQDQILbCQCSZAKAKYaaaAZAZTJICTASSLbabLDTCTRZKJIYBBTDKIZITYZJDCJRSRALKSYCbaYZJbISTRAQQbKQRYYSKCRSYLDbSAbSaSZaARTBbAYLTbJDRbQSRIDTTYbLDYSRBQCJAQJQbaLaIDDYTKSJZBRabTKABCQCYRYJZaIYQTaIJBKTbBCZQYbAJCKQSTZRaBBbLYKJAYRLDaSCCCZDLZAKIQDBabQIQCbaBTbARKYaZLLaaYIaRSLIDCRAYIKaZCDAYZDRZZaAbZIbZIKCTaDLRQCQALBKRQYSQZbKKCAbTATCLTKLRZbaCJIZaQBAALTTYKDZQaDZCYKLSZBYRDAYBQJbJCDDKRSABBKbZZASDBJbbSbRLabJADLADAKCTBQDIAZABSSQZZbZASJTSIRCQBJKZKLTIBaQTJAQILDAaCKYBZBaRJAKITDBaCbQRSTLRLDITbQLRTTAZaZADYTQSSTZTAASLKLBLILLbaTKAQCAYITbBKSLLTBYSZJYACBKIDRAaCJKAYSQJSYDbDabKYaRaJJDJSSBKYKAAbBYATZJTACCIYBaYIICTJTQbIZAIZZSbLDQTLRAYBKSIBIKRLCRTaBTaaQIDLITSDCKLZLQBbQRZLTSAYYLLbTYCaDACBBQQTLAACbbYbTABYRYCJYBCRCJDBbDTTIYBIbIZYKZSbJYQTIIKIaDTaDSBQTCCaQYTRYDJIBbJbKYDARKaKJZJSBCKZQSZATYKZRQKIZJaLSRBZRBCYbSLTJBLTZLTbKAASCTSIQIRISQDKYRKDIATBLLaBbTSAJATYLTIbYQIISKLaYBJATJZABbbbDRCKYACQYKBIJBYAACKaBbRaSRDADbZaQZDbSYJTaZBIKIKCZCIaLJQZIDCRCKTBDLYRSZDQSRLYJJCQBZbLQaSTZTRSJQIJTRKJQYIBJLaYJIKCbALRTCQDLLCaSTKTaTCTJISLYTbIDSJLIKCbSbRIABRZSAQYCBLCRZaKRIaYCaLaZCRAIaDBLZJKSTSKLDDBQLKKZDBCTYQYZRTQZAAICYRYKITAIDDBaJISbRaDYTYbITabRCJQDIbQCLAIbYJZZTYaQYCRTJDBTCZQIJKCbBaCCJQBKJKCaJBJaaQJSTIZDZBJRTIaDJYaDDTAbAbQLSCIATICRDRZJaJTRJJTCBRaALCRSBTbTBBZIKSDSDQSRJRQRSRbSCDaZQCJJBbISRZBSDKbZSKILbDIYZaLQJQSQRJTYSQDJYaCIIBKJYAbTRJYKAZAAbZBIAJIRSbALYJARCSRSJAJKIIRSIbTYaZADKbDAaBAJJCKRQLaSLDYaTKKQSaQYASYSRKLKbASTRJTAbALBKJJLIJALSQSALYQTZaKBYTTYTKDBBRCYJCJILYaLKCJZBCJSKZSRbDJLSBALTBBIZKQRIJKACRDZZQYKLAIKARSKJaJRZAbZJLaSCKKKAaKDKATIBbLSLKQAaQJJDAabKIKCSBZCSBBaSTZRCQABRBDbbRDYYQBZTbJaKaIYQKISRbSBTQDaLZbTSLbSKYCCDJKYJKSRZTSRJTQBLILIJKJZBDKATJJRCSIKLIZYYQKQYSIQKBSKQbaISYAICJIYIBbQbYKIYRTbTIKIABTJACLTTAJLKYRTCSCZYSTRRTCYAJbaLaaAALLDbTQTZbaDTDDLKYSJBDCKYYCCCCZbJQbLaTLAQQbLZDKDJSJSLIKIRZJCaQTbYQIRDSYSZYRCSIAbRQJRYJLTLDBabCQRbRCJSAJJZaCIaTYYYRCZZTZYIIICIRQIZTDICQZCLQLJLKQBDYSTCRCBSJJDJDLZTKDDRZCTASAbYabRIQIbQZSLYAYIRZRQCSYJDADJaQaJKAQaRSaaJZTDRTKAbbRLIJRTCQTJCDTADZSAJAKIZYIKaKSIDaRbKAQYBDRDLRaZATTLZZZBICQKQKAbaRbCRARKKRJRaCSaYSJaJCTKCJJYRbBIYTBKDQICIJJLSISAIYZBQRbZZRRDDZBYSIIQYDaBYYIYBQTJTISbZQaLDDCQCKKSAQbBbJKaBCYRYAYKCBBDLTCLDbAJKLARIRDRCbIJaBDTJCaTLZQaLbRLBQBQRCBQBbQRCYZTBDBIQTKaAbSRTKISKZKTZRbQaIRQCJbSSYYAYLZYIAZQbbaJDCTKKTSYRAKDRKBCZIZIBZKRLLQAZaYZLLKCCCAKSLCYbARDTbRTSLDRaJRLLABISJQbKDBCZRCTYaQRJTKRATaYaBBAaaRTZJZKRTQCAAZDIJIITbDDaSQZYIKKCaILBCYKbaRTLaSQLQQLCRSaDbBZQIZALBaCZSQSZLDRAbbQKLQKTJZYIRZYCDTABRJDKJRCLRQCTIRJQbIRZaKbZLCCBBaTKQDBSSTbSYLJRbYJQSKRKbLYbTISZCSaYLZCLSTaYRIaSYKRJBSaAZSKRaaaAQITBJBJCZKaBQLRRQKJBKITSTQSIZBSZZQbBCYIDABaTBaDaaCYQZKSQLSJbKbTDYBaJKSIRTBATZLRDABRBCZRSIJCDIBDARSDbbIYQRYZTCALAZJRJLaIBCKCJYJTbIbKZSLQZBTLTTTaIaaQSAabTTQQQIQQALRRRCLKCIBRCSJYJTBKRAaALQTQJJJBBRJKaIKLBLLLALRBQCATRDSLLTBTaSTAKTQBQSaRCTJDTJ"
"ICDRQRAIKbSDTKLLLaYKAJCTKKLDZLSbAQBRZaQYbaSaIZaQYJKAIDaLbJBZCSAaJIRTBCKARbDKTBSJLIAQaDSLDASSJBDaCJbSbDbSbQRAJQBbDYKDSZSLBDDSAZZIDKIBQIBKLRZRTRaYBZSALZZLBDQSKISAASACBQCbZIZTCaKRBRIRDBLTZIJLbBAQbCTZIbRTZTAZICIaSaIYSQRJLRKBYbTTbaQLTaRKIKDABJTS"},
Embed{81,259,"IATILDKIKIQIIKLIKbJCQIQLIJIICLYYIKIIAJIYQRTRSTRLZRQTTTLTaRTTBDADbTTRTTLbTTTTSLQARQTZaAbKRYBITITDbIQaBKSbbARaBYTRKKBYCITCJRaDJTJADLKJIAIYLQQBZSJaATQDZCaSJaTDJSIaaTABSIbCCTQSAbSAbCZBTBJaKITLIZbASJZRSDJBJaTDAZZCLbCZLSQTDQbDBBYKZZKTBKZDJQSAIADCYaRKYQQYaRCBLaBYRJbYDKYCaRLDYZCCIKRRTDLKbQABSRZLCATbITLbYRBBYKKDbaRBCZSBATIBYSDbaKQRYYDJBRABaZLLTBRBbYTSCIIJBZTSIAJDaBSSQLRQQRJbAATQRaSALJCKKLIYKaBKbTZBKZSaBAIBQbSbKJTaKKbRSSYbKACYLRaBDabQTKYJKbACCbRYZADDSTTSCQZBKSYYbaJbTBIIRTZIaASDLCBDDQYLZIQJaZaDDBIaQIYAZSQLSYJKSBYCJaLbRRJISRYIATbKKLQLSZSaCITaCJBbQDDBLBKCYTIaDZJIaSJDSYBARDIIRJBTCTBRaTYKDKQRYKaKRSZBCZbDQbTCIQYKDJLAZARIbAZAKYQZZSJLIAZYCCJYDTQSLKLLaRDIQYTAYRYDASaTZITRKIaTCZZQYJDZLJBaCAQaJbCTBTTRSJaQQAIQDSLbZSYTDAaJCTDLKCZDLbBDSaLIabCLDIbKTCYbIYISLBRTTTRQZKKRARDAIZIDYJDaRCIRDQDRRDZICBDaDLACZAARRLKbTTTYCITIRLKQCJIJKbIBDbJRRQDLCLISCCTIDLQZDYDQJQIJYJKRCYCYBJQbISJDaDRCAaRIRRZSbRKbSLBDJbSCRSZSYQICbYJQCSZbJTKAZASbCYQKAATbSTBDaDJRKTaJZKSJbIIRJCDbQDSIJCZZQDLQaaCDDKZLTRAIJSCbQQbBRbZDYRLKaaCbCJTKJIRYICbbQTCQATTRTaCYDZCJYBRDRCRYYBSLAIbSBTZKAKBCRBSLQbLTZQDKTRaaQbTJbAAYJCTTRSKDTDIZAJTKJKZICbKSJRZITQSCZSZCBRZIIaSZTTKYbJDSKAJCKTKbYCRBJTQIRRLZZZTCACYaBYSRbQTbLACSTbJDSLISIDSIIDZKYaDIAKZLRSRDJAYJRRZBZIaAQQSLbJRaCRLaSYYYRIQIDTTBYDIKBAIJJZaDSRCIYabBTRaJKSBCbSLITJaABbSBAKTBBDLKLAIJCSTKBBbDIJABLQZIYabQDLKRQDRTCCRDKLTaRZYTAIaZaAaJRSKIJYQRBRABQaQbIAZLKJYKATCTbSBBTADbCRYSZLBYIBMBYZQCRIQZKCgSZbaKJCLQDBLCRYaRYASaYbbCZTABDLQaTRACZLZTKQRYDCZDZBKIJAQSJDRQCBRZLbTCDCRJAZBARIQbZIDRBQYDaATQZZZTIKQRJCZSCSbhBSRKDSLbbZACYaZRQBDbBISATRJBAZbBaTLTTYBCZSQQJKBAbSYbaCISILTKCDbacTBKSJSDBITJZSABZaIAQCZDJIZIBBAZTTbCTJZALYBTQIbQBIDJCZCDKRaSLTRIKRITbAQYRBJQBKLCSDSDJQYALYBTLCQBIJBRJIRDYYTJaRJKBYLABRBTbICCjYaSKCgIaBRLTDLIRCDbZQRbZTJLIEAICaaJSBbCTDZCKTEAYRJQKRRZLACJaBDISQTYaBZIKQBKBbSbDKDYQaaZDBDBaCbBDSAYTAZABJDJEJKTjQQJiaQLSCICAJBJaDcKAYLLQbSRLRCZZjSAQRZYIUQLRIIDTBSabajLbAaZJJcLDTSIZAQCaJbSSSCRaSCYYQSIITDTDQDSjCLITTSbIABESZITZCBhRILDRZbCYTCKIYUSJaAbbTgBSLCCBYabZaCbKZLSSMIJJQKISaBQLRAhKbQALRIABQJJbLQYKCSbagQDBRbJjYAZBBJIKCKJCKSSgYAATQKbQJLTLbTECQRDMLbRZMLjaQZLEKKcYEAJYTSgaYDhIaRcQbRahDDjDIKKTTYhSJIRjDTaIRADChZTILjQITSQETSJQDcLhJjDYDaBEUSAZKTEILUASZaSEDbDaAEDTAAbDbJZKaCJLRgcJRUjMbBbIjaEDRaUDLbCDQDRDQaBCKSKSIKgKjYLJBiBLDIDbRIBDTiUCLUjCbbYkbDjBCQSYJKCQQjDRaYRYLZjAZILBDjQLASTbYZRhDQRIYkSRSkBJRBLQAJYRBKQQJIJLKLgZiQaTYRBSbJAEKcUQaCbUiBDBTKaTaKKAgghEYRTLQLYRTJCTDTAIKLIIbgjLCCDLaYIDIZSLCJDiSCQSKYBRSLIhBTLSMThCgMgLAKckTZbZDaJQDREJITESaDIEESKYLYYJcTEJZRJhYgSDAQahggDTJQbLLajEESaciDBCMCDKaJYbIDjUbBTiJLgiSCaZTKhZAiJjTUZIbCMbgSULTCUBYZhTCSCQRMKjkCBYYBAhJACYgISRKCEgRRgUaKDhIZKbiaTRhMaULLTIBYbJBMYSYCRcYaLaATUMkQcKZCIkkCKBMTJkCARiBJJMRCSYSSAJKMcZJJRYAhhBSkkSZMAYETkcSBAjKLBKbZIQATaahhagDUEUEJhZEDRKiYTBgcAkDaLUcJYTLZLaJDAYQaRkKgkiJEKBQAQjahQkSLTTiLkCSYSQkKkBKAZIYIjKbTcDMSSUgTDDRBMiDjEkbJLijAacQBKgKSTMTaJbAKiiJhBSbRQUSbUMgMBjZBjZgACcLUBgikhZKBEaLLRcSIhYSjDUgQkZBBaZLLUSRkThLDJYMDUARjDiQhALiUDgULDMAUcIYkDEgKjZShDaCbZijMZAZTDiMAikKBEABZUBAjQaJLYDYkKhLUkSibccJahRTgYQDaakLSSSckUJUbYIIIIEcCBiKDcBYARCRYLcMUBIiYbcbMIMKEMDacCiCQEUDLkBUhYjSQDkKLRKjULZcRDYUMiTbLIJBkiKSAUcDKBkEZkUCigEISJaTgKgKSJJLibbkJaSRkkjTbkIYUgASjZDKSEYIIbbgKURcCjQRECZMAjhRRLDLEScDbkkgLkYbUAabYKkQgjThDUTchASbJYRkCkDDiMLhiTRKgiiaJIjMiIjkURZIARkUCCJMCMDEUYhSaLcJLjjTkSAaKBkjMLBTYIRgJQIAcYUKLUSQYIhcMcaYghChcUIkQcCRibEkbKJDbSLTUhBEMSZgAaJhbEBYiSkhKkigciAahaajUUAKJJKEJEJAgTYSJcDCaYTLQkjEDhRQIjaiQkUaLJJBaQbLEMDCAajMCZRihJBZMKLaCRbDCBSCSBBRRKYKYJZJABSiiUcIAQjKZLbCchQibChMJCcQLJaQTTBgIIRBbEhYIRAYCjCMJZDRihgKbSLBgDhKiZJRZTgZTRZLjMDYIcScaIYgIKQTBRTiiCBEaSSDLCZbYQERIbBBZJJgEScEKZaKQEIUCAbQBZLZaACDcTUBLBEJbAZQIZUIBQIMZQQTSbEIhKDIMAEZQcibUhIIZIRQZSDAZKMZDSERDTDKBZSciRDSTbRhCbiMEISKZaCZSDJDgMMEAJTZAYgMQbACDCDiabCRAITbMZZgYRaTgJaBJUIZZaJKLZCCDIgTSSjYCBhKTUBZaYJkLDabSJYaERTJbaJADAKAbQajbRELKCaQgRQYQZQLASQZZSbZYKJiUTAkYZYZZKTDgJJEYRDgATBZCBJQaabkYQJCDDCJZbjQIYLBQJCkASkZMSCZJbQREYSCTKZYBSBTJLhBDALIKZADTAgTEYTaCbCMYCShSiZDIQgYJZDaUYCUYJLKCJjjARaIjSJACIhZIYQbbaYADcCMTSEDDCBQLMYIIUKBYUKJJYiDbMIhBBRZZCAYSaITYBKLZKKRBUTAaARIAibTYgDRZKCYIZLBQkCDKbbISTDQJbZTSTbZZZDLKRjAIZLICBUgDQQjLKiKAIKDIjBSACSBDTCCYKLBCEhZITbYbLYZJQMZIZETTRSZSaMRTLEIRKAMQYKaTJKDRYAJELACLSTAIIcIbTRCMIDQBCjIDaBJbKMDZKBLSIKKiTAJAKSRLRRAJBQJCZZLIJjagQgLABRZQSQRYBALCaJZYYCUTKJDKKDRBYaRDRSJRSKCKETQDLJDBBTZbRhbYIbZaCIRSSIJYDCBJQTLLUCDKYBbLZKaYDDaQKZTJbSaIhERaTQAKTQcbLKDTRYRJQIBSjKRCRiDSCEabhALCRYMSabaBaJKLASBDKbDaADATTBYZRALZKIKQBRBZBQbKCYDTgTbZSLLLQiJaAaITKKTAUiSLRDDJCJYSibAaYkBTTBTbRZZALCLQiRJQQZSYbCAaSQJCCRIIKBRRSJbcKEBJKSJJAKgKJaBCTSYQSLaLQRKCIgKRbTLJCTLbDhARASCTZRZIMaKhhbAZMSLAADJSCSJSDQJZDMJSaLCBELLTAJKIbbQQBQaaaIiRASDBQKRJBAcCYQLJLJabSAaLDTJCKTKJRKRCBBBDBJbaBLCbZYabCRSAQIZAhQRRCMYbbBLTLICCcJaZCCYCQZLRYCLbZYIbaDjAQDATBYYJBbRYYCJKSSIaTLDRJZBLiCcBYSLELRCKARAYZTYJIZBKAZaSAKZbSBDbJSJTKJDZCQTBjICSiCbCcYQZCZbYIDTKSJbaZCBTbbRCCLDTDhQaUaDJYBKSAQJKLJAAbaabJISSBCADIAaBbRbDCMAMiADJYhLbaDKLaISIjhjQCASDLTZjLQBIJIIBQSLDCjZCSbKJiTKSCTbYagMIYJQRYaRYCDLTQJaaTIKTDTZLTYRbUTQgCBbTJKJSaKALBZLTaLScBJRYCQSARLLYABIBTJIaIZSLIJAaaIJCbRIBRbCaDbBjZaTTbTCChSaCAKCTZCbRaTJTLZSLaRALQZCSQQTaRTBbbQYZBQALaKTSLLAIJLQDabASaJZSIkIbIESKDSaShLZaBCRSLbTARSICbabJKhIabSITRACaIIJKIITJYIRJbQKQIZQRIKTTTbCDaRZCEDbCTSSaaIYTLYARDBZIBJATLYTSagbcSSSJSbLAYbJBDTADIQLKDTbABDDbTELJRAQYARaJSIKDYZDQZbKYLIaLJTKZJLCaBCQTBbaCILBAASBiRYCTTTCbaYADaSLLJATCLRSYIKJTLYTLKYbTTQYcQJaBJTZJDMRSQSYLLCRQADDIQbQZQZAYDQJZBQQTDYaDBZJSSIJbDKbLSbJTBSZIgIASCSSZQLIbRKQLBLQCaBSSBBICJCIaTQbYLJBKQbaCKTRJJLAJLTJIKAIDQTKLhBLSaBbDaSRTACQYZbZSSbJKDJaAKTDDRAYKaQaDCYTYLRLZKQKZQgQRICRbZJBKDRCQKcJJIYBJCKATJDRYBCKJCSIALJJCSZKASLASJSbAYDSaTTLKJJaJBbCKYAJCCIAYCZRaIDBSTBQLCZDZQbSYJAIRLSaKTTSAKILJRaITKAhTSKDYJJSZKDARJITBbSSRRKRJaJCIRaQMKJBJARRDZLKZCBISBJKICYSaRZLZCCYZLLaSaYSKBaaDYTBQcJaBADRAaRBQDZBBQLDADbDZbaQTZCIbLRBKICaCAAMSLZTbCTRQLAYDTYCYTCBTKCLKACADZLASQZSRIZTYIRBIBZLTZCBLLbAiRBTLILBaRCBCLJBTZaLSKJSAALQJAaYQRCbRRCLaSTQTbISCZDQIaabaDQRBCRIIaBRTbZYKKIbDKBRDSDARYDDbLBCIaCRaRCIKTRbLZYDDDKBKgCJSDaTYCSTKaRTAAbbZZSaJgKDKbQbZaLRSabSAKBTTSaQajBTRQBDKJSaJRBJSJjAICJZJBZAQaQTYAZDDRALQJSQbRJBBJYJTYQYDLABZAbTAaLTabCAaSCQIRRAAJBJZZTRIBJDIQSSLBRJCaKIaTIbKZaTYaLCQBRaaJCgaJAAKaZATLQSTQYQbaLDRQCIKLIaDYDLRYLCLIKLTBQACIIRKTYJKKaRAbZQJSbabbaJLRBKZBBRYIQCRabIaTbDDKQCRIABaTITLZTaaQTJaaIZAbTSRZBCQDJCTSbDRACLJCKDaQQbIZIBBLTDBQQZSRaLZJZQALaYQALaRbYZLbCLaSBabYQKZaAJQRIIYSQSILYYLARDabCbIYJLQbSLTAAiDbDBYCTITbTJAYbaCQKJYTYARTbSBCAKbAbSQRJJJTTCCJJCaDZaSALCaZTTBYgRQZRTBSLTTYKCKRQbBaBZQQKTRAAIADLJLKLaYaLDaCRCBaLSbJTZaITZAQSSRZRKRCKaLDSbDaQBRLLAYJaJKBLSSJSgQYZKaJKbBaSRSCLZLRBQaRaSLQKRTRRbTRQLTZZIRDaKSZCQKSEIBCRLQKQRDBZCbYZIBAaIBZLTKJKBbKaKRRSCABDRDCaQAJCYYTJZCKaaaSIDZRBaQADKIDIRSLDYKYSQIBbATaABQTbJKRZaDLRATbQYbLJaLQZTZIYSYRbABTDDYQZRCKCTAIZQQDZBJJbbCTISICQCbJZABJZCYRbaSKSAbTCRCSCBZKLaQYSTDRTbJAbYTYJbIBTBKSALYJYbRYRKSSRbbTbASJRJYQITYbIRaIACBbBYZIRQIDYKLTIQCKRIZQATTCILARTDCLRAZKLJBaTaZBTITZKLaSbRQIDhSQaQIQSLIaITCaYAbDBCaQJBSbJDDALbCbCBBLYBBIQIIRRKCRSDaaTTZbQILIZIDabSQDLbBCYZQRADKKYILSSBbLIYZYDRaCAYJTRYISZLDDDSCZDIBIZRTBALSADIbISYLYaaZRBIJCjbRKZJYCAJbCQTTYALQCYDBbQYJBDAARRLLBSCBYZabCaSTaZgLAZIIJJLDDTTACBALDJLQTKKAQLZCbKIQJQCCYITTRYLYTCAKZTCBDKKaTAZKJKKaKCKDBDDBAYAABZLTDLKaZCAaSbaSIaZCJDUZZBQDLLRYZYKSQRZDaYKSBZQKbCITaCaCITbBCKLCDDAKSabCbaaDYLDABSBQBbJZSIQADLYRZJBLCIQDYASJCTRBJbQZaaZZZYaDQCBSTaRJQRaJbKKDaSSKbLbLRRYBDCRYSLYKRaCJJRbSaTDBaCZKZbCSAAJYAbKIbCJLYaDZACDLRRLQIaaDbQAIRKSKDSQIZTYJYLLCCSaBZaLCYKIBASCZLaBBYSCaYJDZQIbIZTBJBSCLCAIBTJKILSJaQSARaYITIZZDJJDTIZZSLSKaLZAKITbbJTBZACZKKLRCKIRRSRYRaJaQKASKABZJaDIBAIKbDbLAKLLALCBZZALBYRSBAATDQTTTBLDZJaCLQQYJRYLJSDDLKKBLATBILQZbCQIARCABBKbARYLRYDIJKQIIYYIDLaIKAaaKCJLISRQaKCIBQbZCLBYaZaBQCbBYQRLQabRKQBJITJbBbAJTILQQDCYbCQQAAbQDJbbIAaQQQSKbZJJIIRBIZSRYSDLaaZRJKLKRaBZQTbBDZYJTJILabAJDCDLAQBYIAAQSSJQTAZQKLQQBYQCBIbSTaSSDKQZBaRJDZKDaILLDRQBYBLIADbSJTQIDCKKYTBRQZTTSIDLLZSAbQbZZKQZJAJbaLKKYZbJbJDRBSKRAKIQJCZCKaLDSISbBKKTASBAJQBDYbLZDKTbCaKBLBSaQSaZYIQZISJTbATKZZQBKQZYQJAaaDBDSIRBLAaKZRQLZDJbIYQLQSZQCJYBSYaKZbAKQAKRLRITLJDbLJAQaTaTKLaDBRADDKQTKSBTJAabQKCDCbLCBZLBQJJTaYRBYQZDKAaSCTYBAIRSCLJRILbKSDKRBQAKAQKYIZAaMJRYbQLRKLLKSRTQSRDJTLQDEBCQKZYYKbCQLaRbJQIRRaJQYZLAKaLCBJDRDKJAaBICBCZLAIRBLAIabaYIJBDIbCAJCaAYSQbIbbCBJYRRDRSZARaRAMAJDRZJJKSDaaJCaCKCATZAAZLIBZSAAAJaDIaCLaLCSZBLabQSKQDbKDbLSCSTTRYLSYJIKTBBBCYISABIZKbaBCSDBLJCIYZaBRLLSRIJDIbaTTKaKLDTDQZBTISYDCJYRTSRBIYSJQSRTYbIRYYQDIKJITCBQZLQbBIRZAABDDYYbJDASRDQIKaJZbKKaIJbJKDJJQRSSCCIQCAIbaYLADaTaSDDRJSIbYCZTaDDaIQaCCLTYQBbYaYKbYSIRbbYCJYKBJASCCYIZZDIJDBATQSIaZSJYIaYaJZYTTKDIBIRBKZISCDIJLBZSSSCDBLTYYbSJZRYBZLLQRABJIJCRJbZYRTRaRaSRLDSLQYaSbSIYKLaRZCAIYQKbLQBRaTAIRDZDKCRZbDTBAQSKQCCLRaCIDJbKDQabKYaLbaJTJYRTaTSSZBTRTILZATKCCTCICLZTZBRAYKBBQDATaLBICTRYaZSIJTQRJYZZaAaSBJADZabTbRaCTAaLTbbLBQCRZQQJCDIDRaARDCJIQYQCJKYIaDZDJCDLASBAKTKKIbYLQaJZYKLBBCBLIQJaRTAKDYTSKbTRIIAIRLDQTYJJSBTbAKRQLZaQIZQICBBQJBTBLTTSYCaDSbCRQQJRbSTLYSQZJDRaDLLYQKDQKYRZIYbbbbZTCRJBaaLLQaRSQJIBKRSabATRQLRYSLTabRYIQKRCRaSBbbQCIKJBIABQRCaKaKJLIZJYYBDALTaABLZJTYbQaSYDaZRYCbRIKIaTKYDALSABSDARDYDBaDKbaIJAJKISaIJbSSKBSYLbbTYTaQQJCJASILSRbaAKQTSbZLYDAQKBIJYJZJYSAbTCSCSRCbBKLbQbRRZKZDLTBZBTIYBLYBDQICZSCQZYCKbQRATbJYIQDSBLYbALQKIAZbCaCTaaKSSZSRZJJJSKZDCKBZaSDZQTbbZRLCTZDaCDATRaLLCaSJZTSKbTRQDSaAZRCALRKRKTYKKKKbRDIYBBKTSTZJYALQYTYbCBLSKQLYTaYCSILBKKbTJDCRLBKZRSAQRAbLbBYSQaaQYbKKQIDYJIaRCLDJYYYIJARQAIDJKRbTIbLTQIDYCTAaBZBBYTLLYBRaSKaISZDDYaDDTTQLSRTCTKLABJYLCBLIRZBSYKISBCBIIYSLZLILQYbATbJRIQbYBZaASBABTZJCJCDbAQKBJRTKYJQAAJAbJIQKKZAJYJKKLLTAQDKJRDJZZQBZTRYaAQAAQSLJLIJTaRYSaaAKaSBDDRaTQISKZJKZIRKAbBKSQKAZLIbaKRZaYCYKSaBQZLTRIRDTbJYSISQBbJLQJCIKbISTIBZaICADAAYDYbDJCZBTaLDSDAYQBLRDBLCKCRJLbAABZbKZCLICSCKQILJAbKaDIBQTYQJaCSZRSZSDTJQZIZJSSRRYLLIbbJSKQZTaRCQaYSTIbTZYYZYLADTBYDQRYIIaDaRQADaBKJCIARYZLbAJYYRaRBJKQZZRADAZDCbabTSSaCBaQDbJZZSLaJIKQYTDSQQSSBBYRDCTKLRSJLaBZBBCZbbQCKZLaBTSJYYCRKIaZCaCTJaCACIJCJJbIQaLSQQAaBIJTYbJBZTBaQTBBRCYBBCaIRBZZDDBBbYIbQbQAaLKKKSaLTBACJZTYJJYZDSYCZAKSLaCKbTIYKCQTIRBaLBLBYRSCBKKSaIJRRIDIACYZIbQbKLRSDQYCbSQZTKQZLAJKJTKADABJBBDBYBYQQJLYbKBSZbZTSaJBIbIAYCaYZRJZDJAAaQIZATDDaTKZRBQZITBJYaQQSaCaTbZQbILbCJQYRDAQIKZSAaTLADTCIBYIBLJIQAJLAbRaRCaZQRCDZSLTILbSJDTDTKSQISKATKRaLKKKAbQYYBKYLIBIQCTbKJZSRJCZRbQaZCbCQQDCKJLRDaBATZIbbKBLTRRSIYZZIYYLQTSYBKLQJDLDDCLDSbATYDZAIBJTYSIRTTDLZBIARC"
"ATZJZYZSKCYIJIRJQZZTKBSKAaTbCACJQSTIKaZTaaJKJZBBZJJBLIaTAKCaDQTQYRDRQSZTBRAJRbQBSBKTDbDSaRZQKZaKSYRbYRBDLRQTBbSDCLSCLTJaSISQZDZKYKLRQbKbQLJBTSDJSLAJSBSaBRLAQKYKTATQbaZQBJLRKIbIDSAAJADZKYYDSAAYLSAZZADJKbBAIKSRZSBBSDDABAIZIRYJaSCZBKYKYIZbaZIRaRAKIbBbCRaLKBaZLYZJCRZQSDZISJIJQKbSAZACQLTZBCKQCDDDBDACaaQJQbDbAJQDLACTRBTaICYJbBIIJZJRTLKTBCLIRTISJZLBSDQTabTQSAQSIYSYJQLaJTaLKDIQKIbDBKLTaRLLaDSAaTSbQIZCTKbIbQbSIaDCLJYLLSQBZIYSLKRDQaQaaZSDRSKQTTaJLDbQaIQCJQLSBJCCIRbTSIKLKKBIbDICSDKbLJBSYaDAZZTDDA"},
Embed{82,262,"JKIBQIIIALLaIKIQAIKJQIIAIIILYIIYIYIIZQLIJKKBSIZZAbaabaaaaKCDSKYRaaCYaaabSYaaaZRaKZQTZaYDLbaBJTKYbQITaRKBaDCaDRIKSBKYCJAaZAKaTIBKDYQRKRACBDZLRIKRbIaDBbRCAKbQYBYSLITbJBaTLSYRCJQBILaDSaIQJTABIZCTKTYRDAbbDRKBKaBSAQJDbBQIRDbYLQQRKKTSJTBJBaZSLJKITZIALCIbTBaSZDISLYaAYATBRSCJAaRYQbYJILCTbCLKRZTaICTbZLCTBTIRRAYALBCbTaDYbTBARJKaDSAbRLQYYQDYDBDBSYJKQIJKTbTRYILYSbZKDTAJQAYZBLQSBLZZCBTaaATTKZSDLZJTIYCTZADLIYCaRSCIKDCaTYbKRRKTSLQLIQQCbDYYAAaAaKRDBYBaBJYYZTbbJAQIbSYTQDIJKQATQaSZTbDAAZRSCbZbZKRCBLYSTLYJIKKaDDTRDSJZYQQBYKARRDbBKARYaBTDTRZBaQbTDaaTKKYIAbLAaBTZATADJITYDCSLbTTCDJCBJDJYZQIYITaBQYBYBRLQISbQKYSRQKYLDJZTBTKZZCTBZSBYDIKTaaDQRQRIaDCaTbKZKIbKSJLRYTTYRTABZBQCaRARTQJaBaaQAAbRRaIZDRSZCYRaRaLYRTIJDBbIIICbIaJTTZTCIIQBRTYLaAKaBZaKSCZYTYaKYIaTDYSJRRLRQJZATLKZAATKaZYIbISBDaZATTJSTYTCABLIRTDAbKRZQKQDRCbZDIITQIIZaQTbKDCZSZRRTCQbaDSCJDRBABbSZJCTaaQDALZJYSRJALKDIYLJKBSCBIIaRIARDZbQJSDSRQLTITaZCDTJBLIJSTSARJJRALaRDDaCAKAbQJIRLKJBbJJAZDbRSaaLIBaRaTQATITADBSIQASQAZLTIJBaQRCIJTTBTTTbCILbKBLRRLLLKaBBDYSJKZRAbZLBITJICLYAASKCQBZJBSJTJSYSCAJaDAJDZLDYDYYCBbBBSCRabIRIITQKQRYbKRDIYAaCaSJJKRIYTZbAaLDAKRBCDLRCbSBRCYbRAJJAJQaQJBLQTIJIKRJDDRYABQYIKCZQKCZKbKDYTSKJbYSAaSYSAJTLLaAABITYaJKKbJBYQQDYIJbTICZLQYSQJIaQDbILCbRTLQLbRaaCIZaCIZIZYaSCYCaTLSLYCKKIZJQQSTLAYTLBbLBBCCbDLCbYYTSDBTSBTJCSTaBaKCJYKaZYQJCaKITRLBYBbZTaQDTZbYBSLCIRbARAQRCQaATRKCKSaKCJBDTLLBIIIQDYKTZbZaQAbJJbSABKJBZSCCYIDCBhLAbCKQYSSJLLZIRLBASbICBbJSKRSbLTKYIbKbAITYBQJTJLASDDKIRRICSKTYYbZCDaLYCCBJSJBLLTAITASRIAQLTLaBZSYYSDYRbDYaZRIDYZYKBZTQaAJJRDAKDJIAQCDQBJBQiKZDYbKCKaYYZIDRBZJYBKCJUTBRBABKKTSSLZRAZZQLKTQAILaaaQZQQbbCSCbDKRBCRIAQACAaAaYILbKaJRJTZYZBDIYQLTaaLCabMSIBTKBZAjRbIDBQSIQbDMBKRZYIbACJJATKJDBYTSbBaAaJLQKbRQJKSICDcKJBQhQbLAQaDCCTbQZSSBLaYabQJLbQADALTaIJJBQCbZSAMDKIYKRaYZQKCaCLZaCDBSKTLaKRRIKDQMALTBIQRLaYDIDJDCZLQCDSZEKLLRICAaYAbRRSALYZDSDDZLLQIKZKLQBKDIYSbcDaCZQZSUKSKCARTRZIBBBJUbLZZSRChAbICRSCRLJYTBYiJULDLECSabQbaDJIZBQKDKIEMRMKQDbAYLTTJQYLDabKKZKQASASjRLIDDhQabAEJCJTBTIRELKSJJTQSETAQQBKjBTZQZLYbIAJBYCBbaaQaIJSIZbYJQhKZIKCaMAQaRJTcIYYYbTRDSbSiUCCDLiYTCbICDCQDAJUSKIaCZACbSjITDaIJcaZjYcKQSAAcYKIAILBbLQhbBYaEYBEDbKEhKCRZATYbJTDTaJcISLRIQACZYJKQLkLSbIKKIIDEbSiBELLSAIaUKZYBZkTDaJDSDZDSRKbLSQgQDcbAYSMTDKjDTJJIRRSJYKZgLRScSQCjYRTbRCYKhbDBgbJEAAMZJTYEaQZQbbBDSLSYKCAKUhILiajbbgRIDgZZiQEbYYDJMDCALbQJgDJIjaLDaSiEBYQKYTTDZRKCITUbbJQkEJSQKZaaTQSZDBDZbcbRkkUIBQQJLCAiDKRQJAkjAYDTjQgLCRJaCARSTaLYUZDYIBKRiIbcAMLaYRUJCLaKiYUIJgAgLKLShSQIDBJgIBTURRYaTjRhKjZCRRcgIAbUUbgMIJacYbBcESKJCLAbBgIgCMUCKbgLLibDCiIgASbLiLZUIARCAJLMAJRSYTQkbBkhKKkjYDECMcTDCRQLUTSCSbjRRLCicZJCZKUMSRAMbLThAiYEcZcIRBhgZLLagaRTBbAYISMAELjkaCTTLLagMRDhgMDSDABMEJYTDDjkIAAUTUZBIckUBbAYbQYLALCRZDCijEgZLKYQMhDDjSUIKEBccgjaRJaRDYKYKkAYZRIZCLcEcicUhJBCTMKLJgKBQYaSDBSCSYhYjDcDikjZKZDRADUMZZQBCYJBLSBkgAgQYCCBbMCDiYickQMcLZMRJhJiCCJRJTLILkCBgYSbaMRcgUJQkKkkDCgbaUMCkgAIhYajcCkaCASYTBZIaaMgMBRMCRLBcBDagaKQcMUCDKQCJCIAaLESLbTAjgJLRZhKgiaTDQQRgUkkMTbBEcKCIIgSCbYihYKhbAkhYbIKZAkZEBSYLCjiSBZBEMikBQRjDTbRBcUKMKbJTkkKBETiSQjBiIICJZTEcRkiUCabCBTCDDJZgSbMTLEEhbESkcgACJZMMchQgiZKLjhciYZBIShEKhiCjDKRIYIEEKgkUDbLaIRbaZjETkBBbLiMUhiTIIYJkLTCCZjDCCKgJITcBMUICBMRIUgYUBBJCaYiJBIKLBaUYaEMjkhMEjkSjRgTABMAhMbQRQibkbCRKCAMaMAMIQbcacTkLjAKBTjbgiSkjghKbkCAbaTgMcUDQbjBkjDEQBSjhaMBTLkabJLIYgEJiCcCbBiRIDbkLcBBiBYUiYckSDYcMAEEQkJSYiDLEMRbABYLTcQIhaILcjMBBcRULiaEiKQZAKDaichEJUgEUiCERQDJjEkkaZSAEKUjZciMiiJYhMYBcBLCLiSJDSRYKAIJhJgiJKJTUBSghKcTaEEacaEEcTShYTSTRjJDQCSDbgkZRajcCMShiZYRKKUQjiJaQcZMjBJaJAIcaIJkbMCQICYRTgQhSCTJSZDLgbkAIYQkaDbUgjkAgBbEBbijRibYbTgKMZQIaIAYCIAbiAckkIhMaCICBQZIYLLZJjTiBUgSKgZSULIJigAgSQELQBUKDJZTJLbAATBLJSShZaAcTbaQkbQiIJjKSTSKDSURLiiBYScZgZaDKTQiUMIKSZSIZcZQChDQCSKjCcDDDDYRCZDRYaBZZIYAZAKQYEQCRIaIkahLJaYaacSiKJkREEZDMBAgDZcMTTAZJDBEESCYLgJDRYKBIZDKSDDAjIaiTIaQIaRAEbaYhQCYMLASDkJKCYMJJUAQSAhbhJIKZBTZSRAZChaSTERBRQIAjaZTKCZcbYJUKAZZKSbQjjRCgUCLSRLZaZTTQCAKDEQISLJbYkjbbhZSRaDgDiBgLAZZibaZJLBSCaLCJLCBbABZCAaBhKLAQaYQISIhgDTTaaDLLADCQMAJUSaKSMJkZZZBRDCRjBcYiLaBKYKRYaSKZSUZZCJBcLJbZITQSYYbUhQBkIaCIACSCLBcBRaICSKERbYiJYSTIbZZhaETaDZRhSbQZBURSZRYAKKSDILBCBQJYYbAKZJJCgQhaIbDUBSASCMDjDRYbhKSKRLSIjESIDRZARDKgCjBCYUQDRJKLDKBKIaAbQLcDMTQKbbLTSIBQTbLCIJjQJMDLAaRaQaQTRLYgSJQCBiiZYjRKTZjSQYBYcCQJTYIZDCSJZRbKQRTDTaTBaAcBaYSYRbQEIKLQbCCaSSIgMIZLKEbJaCLJTbaLILSILDJDaJKSQBbKcKBKTSQQAZZYJZLhLJDaSDSYYIbCjCQJAYIDLaKDSRDISSSCbYbMKcRJTRBJQLIYQQiIACaQKCBBYJTDDRLDBJbBQDjJSIKSbIRCCJYcDKYDhSQIZERAQJLZZJRcLCTCEaJbaDACIDIKQZSKTTLTUaAaQZJILbYRAIaCTaTQRSTQLSLCCShJJJLAbbRTcTSiIJJDZbSaaMaCDSbbJLSTQCTUQRIRASJYJDIASBICAIDRBZKbKCYKISBRZILRYTSCjKLCaLKYACDTLZQRZQAKIZYbJTBCLLRKJMbIRaDQQRBYDKAJZAaScRADQDALbAIbQBZbTQJKbZaaQBLaUAUCJDRQYDKLKQDDTLCCbSIQRKQLBLSKTQShJICRCIZTTajaSTRZSKJMbKKRBZSIQYAaKRZDgAjITBQRYbSKJQJBAAbAZbDBZCBAJSiJbQAQgTDYCSCaZSLJDbQQbSZZSaLaDbLDKKIbDjSJIYJDYZJYQYZKRCAAQYBTTbDYDTYACSAJbZZZDjARRbbjZYZDJLRRRBRDEJSAaTiISTCJBAbSQCBKZZaIQRcAZaBYLLZKSabACLQkCCCRYDgZSTSbbJDDBLCYbSIKZabBTTJKJDKZSRaIUAbRAgLTQAZDCZDaITECLSQLRSAJIATLIDcKRTQbJZJIDZYACRDDRIQYQRASaZBSDZYATSKbSDaDIBRCIbSLJaIJRCBCbTaRYbaDDRAUJTQQYYKKbSSbRDLbJaTCQBJYKcQKAJCTDaABIZKTCMZZKTLQBQBbaATLgaAJaQbISDZBKLIDTTZQAbTQIZaLJTbTSIDQbRZTBCTbYAKCJIQQLSRKYKYJUDBSQRKIIBCLaJAUTQTBYBJTQZLEBKYRISZYBSBILaCDBLSJJTBbhSJZDKYZKRISAQIZRKAYBSJUQCZBCjBSRbIbZaTLSbKKZJDLJCUKaIASTbDQBaDSRBILZSBYIIaIBJIDSCAbBZTbLSKRhKRSCCJRbTLIRYbbCSAQRKLCYALKYDAbJbaIbZRSSYZAZZBTZKJRJALQaBgSSRjaDATQCASbAJIJYIKYTYbIBYQQUZLTALKLJCJLabCBACIBLIbASBRCbKJabLTSIAKJKDaIJiELAgKaZJLQbQBJCDSBQRRLTJDTDRJKTIIDRCTDAUCJYACAIADLbZEZaaZaQZRKYQJaTbKjRJADKaaKARZTRDLYCLARYjCSLJbQIAZZTRZaJCTLSJCaLbKbQZSiICJBZQCSbgZTYTIbQKLZKaKBaRKIQYQLZbQaLTKDZSJKADTLDZIABaaSSQCLIDDIbIAIJTYbaLbKTYZCIJCBZSQDRQSgaQLKTYTaDAYLZZSYLQRAbDCYaJBaSYYDLDAJbbCbQASCSTZDAKDYUAJLTjBbKTQZSAIYabbJYJRTBCIYDBAQRACKSCRIJaLJDaTYZRYQZIQaDgLRZKTYaKRASQBSTBRBALIZTLRbDSbABYALYTCCaZCBDBKTITRbIJIQCBAbbTJBbbBCLaJSYBJCYjBCLALQTILJCDADDLBZASIaLLChQbJYBaIQZYJSYJQJKIRJQRZKSRaaYSDCaDDYRaJZYaJLIDRCRDLCBSCBABIBJQRBTRZARaSZLYYITZaSYTTCCKbLZYDIRcARTLSCbQITSRCQBMQIbTZaJBRgDZRSaKISSBKQBRLRJbLICITDSJCbAQKZDRAJATcRZSLDIQAZBZAZERAhaAJJaYTYaABLEQYjABTKJJRILAKBLCDTTLRLYRRZYQKRTYZAKRBJBRbYCTZChRZTSRAbCACBjTIJBabQKBbSADKAbBDYbQDATLITbDTbSLYYbSSABKbDCbLTZBaLgQaSaZTTAJKDDAbTbaQaaASYJIRZIIYJaTICBJbLLKQCSBDCIBKASAKLZTZZDSDCIaCIJSQabaBDDIYDZYBbDICAILQKLRTIRaaCJCAIBARZaKKCICBIDRTAIbaSDYCZYIKgaAKJZBbBAQKBALbbLZDYBLBaaIYCYRRRDTSBSDTBIaZYBaACaRRTZQZLADLKYSSASRLTLKBaZDZQYRRbIDQZLDIiLBDCYLBbiYQSSSLRLZIBCBYSSJATaaJKaYKIBZAQTLaSKBabSZaJDIaYACCBZKTTCCZbZKCaIZLJZSSDQCYQRZBBZRaLDJYYJAKQRbCZQDCTQJYIbaQSSLIbSJASJTbQaYIJSbbbSJSCZBYDaCbBBBRQJAKJZAJAKRJbDRKACbJRSIKQTAQDIaYKKRJSRYZCAARaTYBLRBRbaQJKQQLbbbTSZbiCTDBSaBLbZQbaTDDIaAJJSBZSJaYITQLDCAAIcThKQSBBYaIYDJYLRYRaDaJIKQLTaBBBTTUIZYCYZCJIYZTYJLBLZRQJDSITBZALJTYaQSSIaDAICKAYKQRJLSRYRJLbBBIDYJaaIRTBRCKKZQLRCAQRTCTBKSTRRTLZYTSaRCKKLIZaDTKIYAIbJDRZTQSCKSKZbDKQAKbIKJSCJZIQLSKRLACZCSCTRBIJRZRKLYISLDCKQZBRKDTACATCYJBZYaJBBALbCYSAaBDZRQZLYSLJZAZDYCYaCRZaaZKYLKDZCQZDDBLTTBJSZRIADKKYCRIJQIbCLbbYZQIBJLKQZaASZYDRDaTQRZCQCJRbLLQaQCDSaKIDTKJKLKQBIKbSQbLaAaabaIQBIQRQLRBITZKIKaYYQKJRZZYRTTAZRKCZYICTLTAKTCabBbaKTBQBDSaSDYLIZSTDDALISYJACLKBKbaTJQQabbDAZbKRITCDaZJKDIKJBCZQCLBTAKZJTAJYbJKKTBAQARbQKTDQRISYDTYZZKbBZTKCLKBCRALTKLKJADJTDICJLKRQDaDTKDZaSKKbbQZZYDRJKSZTIRBLCITQATABCZIAbAaaabDQKBTLYQAAabYAbaZQALRQAIZJSCTLZIKaIBZJDRaDabZDKabLAISISAbCCSRaSKDTRTaIDTBCLbBYTaQAKLKEBaaDBKaDTTbbYZDTaDQaKAILARBBaLJJYCRTTRKYLZYKYbLLQSQQYDbAYKBDICBIBKKaIJRARKALTJALIJSRTRCSQKYDaLYTLIKSSTDDBUQJABJJbbQbBJAaYJbZYDaCYaILQKQBCYCbBJBZYSTSJDCIRTLSYbTSIALBRLZCZJKRCTAYJaZTCbLQLBaAJYTSRbSAIJYJJAYKLQKRCKJRQTaaLaKIaBSYKJJIaDCZDSiBSQbSCbCZRQIaZQISTbTAQIaBBTIAZBTDZRZJYKRITaQaILZKACTDKaZDZALQgZRRRIQKCaRIZADQZIYDbKLaJBYaLTTcRJCSYRTRRATTQQIbZZRQCCQZBZACJISZAYZDCbADLIRBQAKLaRKBZCBTaKRabaJBTaTDZBSZCKbDZaQILTSbIDZYaBbbRCCDTaQCBKCASLYLJYTDQBARZYSQaQCAIBKJBJIKBaBDITIKTaAZZZKDTBKTBTZKAKRIRYLaRSSaDBLbbaDQKADabDbADYIBDRYRSBIZaKAAKIYYRZDZKASDZZRKRSBCRaLAJDLSLbKJCRTCKIDLIKZbJSLbDZCCQJBKIATADDQQaCKJKKaBCIJLYQaYJJYRbbAYSKABQTSBRbSZKaJIZABSLASZYJBYQAbCKALQDBaaBRQLaTADRJIAbTbKZCYbQCaSJQLCLTbYYbSBKICSbbBYSRCJLILBaAIaRICJQCQLSTAZLbYYaKCJLYbZaZKICKaDSbTTAaAbSJbYBDYAZISSIADCJTQAAKKbDTLILBLbIALYbYTBQCJCBDZaLLAbRCDDQCYYBbZYQLDYDATIAJSKJICCZZRTCBSbTbSCLCBbTaKSCTYbQLZDQBSKRIKDbJIaCQYSZRKbACZDQRKDbabZRaBRLBbaQSQRbJLLCRbSZbZATDCbaCJSRRQaYaCSaCDLZRLDBQARZRbCaKAYTQLSYRZJJYRQYKQCZaYDCBSAKLARZRRKBLaQZRKYAaYSKaLCYaRQRLTZICJbRCDIYYYaTZSTJIJbTLDSYTTCRCCRKbDYITICCYYQLILTKAAATBQLZTYLbDRRRBBSTBKYYZBSaIDYJYLLTAQCbJRZCSJRIQTDKZaCYAZJYTDbabLAZCTAAZCYATRJQbJaZDQYKbRSIZIIABaCLICbDaKYKIbKTBAYYTSTLCJKLABQDZTADKYJJISZTZCbAAJJZYDISRbBCADJAJDICaRJDBQADbRSZCIAaZBbIIRLBZbSQCQZaLLYSCAIIYCRaDBISaaYILSQaQSDLbQTLADZLIaBRKBYRIbZBTBJAZTSAJBDDaCTYAQZbJLJQLLASLZJbQKCAabJYSQACSLSaCSRDaDBIaILADLADZKYLJBIBBSJJKaBIIbaDCYaTYYSCLbCJYSTIQYRLSaDJYAZDaJCYKDJCKDILRLKCRbCLJRQDBTKBbDSbZABATRCbLTJBSKQCSZYBYIZIYaBYKATDTSbCDLbDABSZYRAKYYJSDLQYJJAIJAQYTCKBYJbAaJiBAISIKZCZaQAIbLTATTIYSKbRBTKCKRARBTQRQZJBJbLbKRDJSKQARKSBJJQYSaDbTQaLTQaBBQYZJJJCBRLKYZAYSBbaZQLLAaQDIKaTIAaKYBYZKIbTIAAKDJIZYIJQYKAIZIJJQDCbQQTQRTaaIDQDbZAZCYTJbBADIZCQBDYAIIQSLZIZbDYRBDRIBQYTACAbZTJCYKaSBaaKDBKKZBSbAZATJBbTTZIQJALYRYKbDbbCSTZSAAZIDTJSZAbLBITKZADARYaRSLCJSaAYDKASLbBQTZJYbRbaJLIbTCAbJISAQBJQDCLAKBRSLBRDbYTBKaQQDDbQBIYTYDSTATQYAZRIRYbLLaDLSDDJBBRQCLQRDQKYLQTDKLYSKaZZKBaSJDATRJYLYbJYBQbYIaALSbYIKABITRDRBRSADCBaZBQLTDZKSLJbSBACBTDaTSCSbaIATLaZBLbJIKSSLRBZSAQRZBBQTQbRABLYIACIJDZBaSBTRJDIYJQRYSTAJBLbJKITaaJaRCTbBKKJDKYbAJISRKLJDaDZIJKDABRIQCSYIbbKBRaSYAZDDRIBDDbaKbDYJKYARaYYAaKZBRDATBZbaQaJRZIbIRKBYCDSIYZJYSAAACTBTBbBbaAQJADKYKSLALCaTbBZbaZCDTTDRRSYTKSaaLTKISYJYQaaKIbQBBCYICCDLIDKaRDTZbYYRQKJRCIQbDRaZBLbQYYbLSBQBYYSJBSSKQCAaLBSRLSLRTQQbKLTZKKTRJbIbbDAAZDLIYZASDJCLCbKCLJTbCZRCDBSQCbDIQbIbaZTIYSSRCZDBTbCSZALSTYQbaaDDIJYbRCBLRASBJbCaARabaZJDDBbLTBaJJBSQYaBSQKZQACQbKDKBYKSYLYCTaIYBRCbDaZRTYLYJCaBBLQJITQDBAaZRbQIQbCJLKCBZTAQDZRCZAABZRbLBBZBQDYAabQCJZaYYQYLSCKRZALLYASLRAbZQSTCKZQAbBLYbKbQbYDKIZZAZDRKCYTYaSIZZDBSBQRBYCCLLQBaJDBDZQSCbaKCZKBTQTYITIYJbRDaCIQTISBbKAQIabYQZKDLLIaLRDBYZJYTJJIaCQYTCJQbRKYCQIbaALDCAZSDbaZIDaTLaQLIDYATATLJaCKSLJSCBRbBBSZbJQSKbSbRJDCbabASJCbKbYCAQKTRDLaZQJDRDCLKYBSJbSaDAaCDDYaDICAJRDTbQALYSQbAQYZaSRbbbALLZZJYAbaDIaSBYaDAaSQJKZbRBAILCKCATTZZTICbbQY"
"ZSCZISQLLZCZZQDSAbLCSTTbaCJJLIYKaAIbaALDKRQADLRCYSJKJDZJDLSKIaASBYIJYKYKBBYZBKDYALQDAaJKRCIJIKBbAJTRJQSTZIRRQRKDDbRCTCQRJTbTaJCKbRYKbKBTaJARSRKYZbbYRKCLDAQSLQITDSKRIaYJbCJAbTIRIKaTbSTDLYIDLRSBQKKIIaQaaSKLaDBZbCBKDQDaBRCYJLAbBDSZJZQaQaLYALIARBaDBJSQKYYSACITaaQbbSRZAZIYbZAYYaQYZLIYKBDZaCCCKZRYbTRSTQBaLaDYLIYbYBLDLRIDRYCIDBaQCIRDBRbSIZDDbZBaTABbCaZRJAaTSCISTIbSbKKYDRIATKRRCACBQQBBTLTTIYSQJRRLZbbCLCbQTCaJKAALaTJJKTBbBKDKQYYSRSLZYBSDIKKTCbTLJaLRBYQDABSRKLaSJDKbIIJIZQQRabDbQTQICRJIQBAQKLJAQLKZKQJCKYQICbKKIYBZSARZTIIQbKLTQTLaBBZSBIZRQCBYbAISTQRKQAJTIJCZLKLIZRTAZZTBSTJYaLYYQbBCCDaTAJTTDIZZbZIDBRYAIAaYSRDBCaCbCCSDSKaaDLDIJKATATZDACQbJBADQJBbYCCbbRRSZbBTaSTSbBTLCZJKIAJKYbaKZKZTaZDZASCADCIZBCYCaIIJQSITBRZQALbZSAAYaIAQYDBTaKYQZJBaKabRLJIZBDAQTC"},
Embed{83,265,"IQIAIYJLTbTIIJAIYILJAKIAIIIICaILLRJIaIaQQYRQQLTaSTCTSDTLLTTSZKSTJYDbbSTbICKQTbTZSLSITBaYDSTaBITJSZDITSBKRYDILaIZQRKKTYZQRYCKRaCLQZYKDBQLbCYRaRTSYBSALQDYCSZIZJaJJQRbSLaBSIaDDRIaJAILKBYLQZQbDaYDKAQCJaACRRZCJLRDKQbaZATaDDARLYCKBQbYBBKbDCLZARRADIYaILQDZaIATSRBKATITSTYKKYIbISZRDKLLCARaTDLCJTLZbAQbKQDbIZQBRBaYYCDCIJCIaQJLRaCBKKDaTZJAKZRaaaTIIRAADSJKZBBTDRYSJTDRDRTIYbZaDbDaZDQAAIBJJIKAIDYSSBbbSJARabYZSDTCDRaLBRLBCYKBYRZDABDaaIYaZBQRQSQBSTZZYDTLLCKYKJDSaADTRLYRJaYYQYQKKAbBRIBTJbABDJKKaYTBJRJISCIRbaBRSYYIZASDRYbDTLIRKJSBLYBDAZBYTTACTaCQZKIBCbZRBTLaQIQaJCRIKBCZaZTSDAKIBQDZJbRSbRaRAYBZYDTKJYKaYTKIbAIILRTQDbaRCBLRaAaBYTRRQCbAJQRbAZYADJZDJbQDKBBDCJYRCZARCKRYBSBbQTRaaAZYRSTBLCKCZCBZSQTDYRSYYBTIJZRAZAYKBIJbLQQLTJCLAJAbQaBQLBDQTaKbZYLTDKRYaDQIAIITBYAZCTKIKKBYbQKSIJIRbaDCZCBYaBCRaTAQIDRaLDZBJBQBbLYQIbLAZJSAZQCTRaRIRaZDYaKYIbaSJBSbIYBAYaSRBJTILIRZKISKJYDaKBBARbYQZbZYAbCLaRDCCCAZQZSYJCICaJaQZZTbKaLAAQJCaQaTASZLCKYaTJBBKBAaCIKBDBCJYLAIDYILKbbLbQAATTDBLLTSSRJKSBDRDJTSLYQYTKKJaRLLYIAJDCZbIASaaAbIZTSCYDAYSZRYIYYLYDLTZKACaLQQBIQDZbCTSCaJbZCDaIBRZKABaQBJZLBaRSbLCIDALRZSAYJabLCYAKCKISQYKJaBLARBCKAQYYYITDJaDZCBIaYJTSaSBJTIbbBZSABDBBTICACBTJKQbDaQZRDJaZRSBSZBKAILCJJbCbTRLRabAbJSKbAZYaSZCYLBSTBTYAQYBbALDDbLJQILIQSRTCCTIJITSKKQICSBbTSLbLZQLJQDDYBIJTJBTJQZCTTKKKYZZISTbIRCBLJYIaRTTQBKCYJSLCJbRDQBARBKJaAaabaCDQbYSZaKKSLACCTKJLKKCCIKZSZYZJJZYSTZSKZQYbCYQbAYJYAKLaSJRCbJIICKQbaCbKQTLbRAaDIJCABaLADZZSSTCQYJJDJQYBKBIJCZbDIISQLDQBLQAQaQTSZCZIDKQJTRJLIADDLIaYSQKZZRIbAbLDaTaCRJRQYLYYCAbaTbTCTYJaSCBaAALLBTLKDJLIQQBaBKJZIKICLTbDCDQLRQBRBaAJZTbQAaCaDbbaYDZYZaKahDRQBKDACLLKaJTJJBTQBYTKTIAIABQRYIYbJQZbRaQKaBRTBaASJLLJRabKDAAKKbRYaRQEBAJSJBSaSDKDCaLbZTJIDYQAJTbDBAIDLSCDTCRAJJKATaKYaIYCKbSCZhLJLQTLBJDSSTQZDBLYQIRADBSJSDADYBBTKRbjYITRAhYQQbJDQbbRKTRILDUQaSbRYbCUKLCaIQJALIIQBRaABKAKZYQYZbDZCBLcAYYIRRTSLZDALbIYaLTRACKSMbaaAJDRDBRaJbAIQbRZJSTLCQQTBAYZCbgSCBTIISIDISaJBIKSZJJQITQaZbBUDBIMIhbJKBQTEKDTKRYQIAARbRbAaZTRKLaQCTRJYBLRDAZIQajDLQDbZBAaLCSABJTYRJCZbLaiDAKMRQADaRETTYCaKJLQEbYJYMTTaAIgYJBSTIMKIjCZDDDKBKDAKYZDRSaZCCRJAAAJRDBLjbaLCYIiLRAhYcaAbAaaJKRDQJYKaRhSbaYSjIbYZTKhLbIbDSKaRRSYKaaKCjSBIbTaTZAAgTARYRKTCJbQiITiJaYQjaBQbRBRhDBTQAACBKJZCIjaZBCkAITCcCITCaZJaYYJCIKJgjbbiUbADRMACaKDQkaTTBJShSKTiQBZTAbAbhSRJbRgIKJjThULBKTCbBJAKSbTDDTCQSYYQTJZQZRZYTJIUZgEUMIIKJbDYRCTRjAKISAbKIbESbIRBZKUSRhLiQZajYTADTZCKJaAQQLLTJDMYBCYhiBBaDRTgILIQKEZZIUgbYLYYRYMURCBILBYBUYRJLUAMZaRUBaMUUDKKQbbDRjATjLTRYBcAabacCLKhUMLbIRDYhZRjCiQDSDjCBSDILBCBChQRZALRKcAcBCaTBbYgSSbBKggMERCbIQgEQMjZbACEBLLAbDihLDiLhCahaCDMAKcSJYUDSDUKihQYgTaBMakcCLIRSkBBTDRikLaDETaSaKIJDiRQSJJBgYUZaKZUQUTUJbbBTbRRRcCcaIcICCDSgRDYiTcBTiLkhbITKUITLKADLIbYCYcacDBhgEDBYcDQICCCZgTgLDQTCbSTLJAjhjEAaSCQQDkDbcjZbicSgKCLCBbZScAJgjMAKIDZCkQbhBTZMakQAcIaaKCQTTZbScLACSajiMTiTLbQQEYbTbBJMCaSAEKjCZbADZMSkZAKUDMkBbhJQJZCJRTTBBTYMcTRDUiCDaRYkcCjBYCchTCiTKSbbBEbTShbQQUTZbaIEEaZahIAiQccAgcgZELTYciDILLUTgjLMZKDKhSIhBZCZgTESgaYATMKKkIQQcUhRJcDSbRRLCkQhRBRUCKkZhgBbUQQjZEIkYBaABLTjAYTISgDZTLjIRaIIQTkDMQMhbLKjgUCaCckLBbEICUaEUAaajDZKRhMJADYhQiZMjIhJDkTcDMBiKQCjKciAaTBZjgSBkASjLKikEARccaBUCMggRLDEhIZkciUKTASIKCgZcRiajjiUjLhLISbajMREgkIiJjhIQJBCIDSAZYDgSSIgJUBUhiDIkLLMbSikRkbTkbIRTgbjaMBCKDYIBKKcDQDiUaiDcakDkDDRjhhCcBKgZhCaJZMCKcLbCMZDBZUSRSgCKBZcJkSQBkcgEYEiikALgYKgMjBQaIccjRMYcBKbLCaaERkQIkLIZYgkTChQYiDRkhAMCLYJUbLDjkYDcAUSKKcITZIkaAMRMchaCCkJTgCiECTMcLBbMUDCaLSRkgjRKEQgTMJBcJZchDUDiCgaZMiSSJQEbIAMbECUULjMLjhCZaJZZCbigEAMSKAcAjIkMBIMBLTaKIYiTgMRjhKSLLMRMBTESMECJAcccYaghKKUIhBcZjYYQAjRbBCCEJQhkjEiakaTgAMbTbkSgYbAiQELhJREYKIYSBcSZKURhUIRABJIaKKMEabMjAbbICQUbSBCbTTDTSiYBBCaKTLLYRJZUkgKDkBYgiaMckBJDgabDIRJYgQKZIISJaaiDRZgKaUYIjKEZjMcRjKSEYAbcZZCIEaAUaKkLJJCRDRCZTLISjCkTTcSKMSjYTTABLBATYZBMAicJJhQECJKDTYBSCgTBbMRkTJBLZCbaZaJbbYDkjYSDTcCQiKQTDShDQLYQRZIRCLZhgQJUTgYbibMKLJbJLLSYiaQQSSKDDhADAIkAKCTKAUhZDTJLLCATCCEbDiJQYJLBATJSBUIZjZDSAIkJaJBACBMBZYTEBDCYMTRCZMTgkYATDJTSCSaMADZaKJaDiSKZKQDBYbbMbCBTbhYJLRBAMaDSCKBZbQhhSkSggACKQDJYbMUCCISADZgBYKAKBIJJiCLEZTDbhJLASJCajaIIBAKAIADBYJAICAKJBiBbUBKJBMLJiZbRjcELQAChKabYIYCbcQJMTRKbICSZSbEQSDLCSaQYTkMMATZLUAZjIaaTbIBQaakSRSRJTBCJbEAMTbURYQZCASLLZbMSaaTkaBTDQIaCAJBRBgiIAJEZJQRLYBURKDRTbYJKACKCSTgDbBbBQJQaEZAKYhJIRQUBAZKAQTJCLkRjJZDCTJYIDJIAIBZCQaBTSbUMZEDQZBRcYUCRQZYMBRKIATYZgKDIRKaBCRJgZAIYSDKLKZAjZBJRiLKRaggDCUYLTYRYBJZKTjQBYKMAaTRKRKTCDQQQaQLTRKJCSTTaIRAILQACaTISZZQjJaaZLCDAKMBICjCKJAJITSZZKCBYBJQTISEZSBYZabJYBIRTIDCZAMJbajRCIgAJaTCQQJMIURCJAaKQCZSZBIBTBAhIjaCSJcKbUCbbZaabRULJQBLRASBhcBTKZYjJaABbibbAABTBBSaRZCQRSYLSIDZKYQCgQKYRaDEiRaSKCSYaKLYSZZIRSJKTLDDSSQRIYDRLDgDAKIYbBZADaQBBYLALRJTLQhDKibLIQLLLaDSaaZDLbYCSQBIQCKARLZCRBSbbDQSTKJaKEARcDJJYYACZICZaAaISAbRYLYQZCKZCTZbSDBAZMQZYRBYDSJbbARJQDgQTAJbDLRCScQAJJJCAIKLZbCKLQbYZDTZJaYQCAIQYbZDhYSTBiQTaARSYRUTcMabJKDYJYLSCIMYJJUTDbYBZSKAZScCQaIBKBJCRDYDLRjSSYJJaQUKaDThTKJQaDSATADLJaIJQRKAZaQQjaBDAKaTSLZJSCJhCJQDAQATBZKTLaZJZZLBQTSCRbaKSBaAAYbKQLAJRRAaDBDCaLIALJSZCUICZLESBKTTMLMJRiCQRbLAYDRKZUaaIQZJYQAYJaCaATLbYYkaIQaDiZTQLJjYSKKRCaARDSBYCKDaTRYgCILQDZLLRLDbIaLYIZLLLDLTQCSgCSJDSgYYATaTTTbDDZJLBSRAQIQKZESKabRjbLATJSQIaTSLabLCIYRBYDCYAbYKSJhKagCSIDZaLRQgJIYLKDQZDkTTLbLRSDaQaQCQKKDRIBTTRSZBTKSBbITCTkQTJRJADBQRQZJQYBLSTDCZYDSJSRILSEZZQbbSbSYZkZRLCbQRBbRLIRCKSiQaKAYCZQJSCJCbZBKBSSaaaYLLKAZiQCBDZSRTJDbCBYIMYTDDJIaLCCQCLiYbDSAiBLILJBSJZBTDYZLTbKRZRSIBSYYBYbaSITYBSAQZKLQYRhDASYBJbCJSEYQYDMLbJQCLKiRLZCAbTBRYDYBQIaaZKKCYaALDZAYaRZLEADDBIARgZbLJLKQJTLCABYEhCSEbMYRSJZTRACYZbQISIbIKZSITJDaLKIaCKKIDcbZDBADBCRJJASJZBIBCBYKQDIADTLAJZaCTKJjTCQaZYKJIiRaRQbIQJBDaDDJCCZKZZJQQSDjYYJAICZKIYRbBDRSShJAIZQAIATZaAbaZIZIISQCYYLKESJTbQbTQIYAbBaJDQSICDYbZCaLLDQZSEJZBTZbLbYBLICIISLKKSARDSSRiJYCTCabARZSaKJDRKASAZQJIARLJTbAJDSRLKBQIbYLRMSTQIYZDIQCTLSIYCBCBZLLQaQAZBaDBBBTaDAZbIYCRTJaTDCQCcKBQJRBYRTIIDRBKRTDZJTCTKRSQTIbAIQQaaZLALRJbRaQRSDALJDSaKTYBTbICKKIYJBbQRYDZCBJCCLKJQKaRCIJSYAIbaCJKYZBJZaYBBRCZYbLAKRECSQYKZCZSaIBZRDaQALZRTCLTSbJKLTCTYDTaCCCbCLQRbTLBDZILIYSTAKJCQaABARKDIRKcYTKDBJTICKBTbSYAJBSDCYaZgBZCLQSRaaDAJTJCQbZaBLaTLbIaTMCIRJCRICDaTZCZRZSSTIIABRITZCYbTTSaLYTRaRBIKYbbIKYAbKKRZYCDaAYCTIQKBLZYQRSKZTYLIABYRCADCCJAaaYTbYaKRYbbISbYKCaBYJaDTLcDJbQYSTCbKDLIACaBSBbYRZJTaQCCbbKbaaSZAQCRCSQTZAIbbAAZJaRDQTQbZbDbbRCKDLJaIIaaCTCSTTZTaACQRYYQTYQIJTKRZAAIDaCbBASLIbbBRSYLBRLaKJDQQJYQYZBaECbKKSCDIQBQbQaaLLILCbbKAaIZBabRBSDYbKaKSRQZRRRBJRBTJZaQBSTSSTaZJKaZaCIRJASJSYaQLDSIBYARQaLQQSZBTQbSLMKZDDDIQLSJSRLQZJDRYYJTQYLZKDAaYBKTTQCYQBDKISZYILYJDbZBQDSBTAaTbCCTaZSJQBQSSILIRCIKLYYbDZLYDCTbREJSaaaBILDBLKCZTbCBLQDjYaZLSYBYDKBDYZBKACCIQBTSLLZDYLLYRcaQCQCQDDYAIbQKZbCCSSbBRLIIYRDLbICagCCCZISCDZJAZaTbZLRJALbLIaLSSIQQAJbAADRZQRaLDDRACBYRQKICDZaaZKJRBKCLQKQJQTLRJDDBRJaIJJbBLJARCIaZIBAYBaZDCKLDSTLRCRZAKRASQSCQKaKJTDCCbLSQQCJYaJRSDQBSIDaRZLRaBJKJSBTJSSKYBBCYDRjaZBCDQRADcCKIBBLLRJJCLjKJDJQZACbDMJaJbLIAJKKSAYRSTASLDbBYSZTBIICKQLTATJRABDQYDBQATCbDZZJZQZaZZYACTAKjLDCAAJKYLKaTRLAbKZAQCZYZJaCIBbDAAYCZQDbATBDaKIBZaBILKSSBTLCILJCYQBRbLAZJAYZRSKYKbSTSKTJSYSIaLKATRZIBKITDRIRJSQIKIZISBITTaAYQIaIaSjIbbYQJYQKYCaKQDKbaSDDLLTZLYDAZSLYRCCQJCBCZBbRSYBRSYSbTIABRCDDaTKSDLTARZaJTZLJSQLBLJCYLQRRACBCZSaCQTATYAbLLRTCQTaAaLDDLaTbIZSZBLaTSKZDYTJZTSZSDAaBTBbDTYICBIBALATSJYYTIAAbCJQLTYDZTLQLSBSBDTILAIZDSBbDCLRTYSJYAaDIIbLZQaCJSJSaRaLYKKCQLYbaCAIZjCLbSYCLTZKYRCSZCLTKbDBBIIDQSTZabICbZTLIQASaCRLabQSZBBIZKSbKaKbAbBbCSBISbRaKTaYSRDJCAJKBbBRTLBSDTRQJQbSLZYIJaSKKLQSZLBICbbKBJLSaTIDIaCCSTKSJTCJbaSYaSTYCJRQRCLcKTZCaSDDKLBLKQZSYRRYBDYLLBQYZIDTJZSbJLYRaTRbRSTBQTSRYCDCAAZCKZTRJTaBZRYIhLTLCBSTBTabZRCDZZZAIATQaaBRSZIbACRDCAZYKJIKDJBbASQbbTJADZaTbDSYCIZAYCDIKabKITAAYTBLJILJBIQTYADTIKBDBBRYbZDBYDaCQBIRKTKAYTRZRRAAaTYLKRDbYaTTBaDKQYKDTTKTaJbYAKSSTRKIbKAZQbbQYSAbJDIKLaabbQRBTTSZJRCDDIICIADRYKDLSDbYRDAKLTBJALDKYCaBSSaYCJTZQIITAIbSbJIIJTIaYLYbLAARQSQaRKQBKaLRDKCDSZYKSBQZRaLbCCKJJKTRTTCDRQUaAJCZRRKRCbSCIRbRaLYQCCDZKZBSJabQQDSJaBLDAICCSBTLbKLBaQRQKBLJZCYCbaJBRLTKRZIQAJKYISQBTRABBajbKJCIKbKJaCKDLBCKKDTIRKDAKLbRaYKICDBBTISSLADCABBJLQLBaTZRQaAJYLZYaTBQSSYZZKAaSLCRLARZRTQLIALLLZTDQCAAQaaBSKYJCZRCRQCBZYLRBLZBLbBYJCIaYLBBADBIZSCQJLQRAaLLDBBYDKbRYSbJaBTTBDABCYQSQYbBAYbAaTSYRbRJJSYTZaCJLaRADJTKBKLCIJLCAaABCQTZRZaYDLRQLTJDYCAbBCKIYATIIBBIDZaKKBBIYATCSZLAYZTYISDRDbSACBAJLARbDLISRRDbLBSLIQDYRLKRSKaAaDQZbCQYDCTLDLbTQDDCJIKKDRCTcaKJRCTCZAQKDLZBAKITJaRACCKZDBIQYZbTRYQbYBaDQCJbASCLKBQRLTLSbIaBLYLZbDSCbDJSDBIISIaQZTaCbCRaQQBJQaRQSQLBQaJBbAbQITaTDbTRADLLbaRZZCIZSDRBBJTTQZBCLKJbSQTbIALZJSYAIYbJCQbSBSCLABIDRZCKKDKYQJDYaQLTRQJZBRAIRLBSZBDIQBAZTZYAbCJCSZCQYSACJJIJQKIZIbLABKDRJJKJSIKIDRBJbYSJaRKJJLAQILKKbYRbISLQDYDSKCRQaKKLQAZYAIbAYBaQARLACZBaRaIBBSSaBTBKaLIKIRbQARbZYZTRRaIbaSJRLBDYCJYICCJQJSDaIDDAJQabDCJDZLYCIDQQCLaBbaTYZQZLYBBaQBCaJIQCCabRYbQYKZTaCZRDbDSCbaaCZDSLDCISSALRIRJYBQCTRCRJRLLTbTaaTAaRbDbCZZIBYTTAYKabbCSIbbYAYAbDIIRCJQICbIDILJJJCACYAALIALQLYSQCaKDLILKSYaJKIaaICJYIQaCSBTZKZZAKITKBKRSILQISDBLKbIaRARbbLAYBSYKIBQYYKZaLKBIASaIbaLbCZYIAZIYJASCIKKLbKKJDLCaSKSJJCaZBYTQKDADTQbYDZALbCILIaKaYDaRQLYbIaYIQBSKbKZTLTRSJLaLLCKQbRTaYKKCTKDAAbaADSTYbYCIQAKDARZRbSYKLDbJaBZbYDBSDaCYaYZBZICTBQDZYKZKKaKDSIBBDabTZTRBKCSDRKSYLCTDJKCQZTJCLLaCDTCaCBSaIZISKaYLbRRIaSZKSBYKSCCLSLaYRYaSLBAJDZBaABRLAYSJDTLbKQYQIaSbYIYBaILaCaBALSKZASLBbTBBJSISaTaZLKZYSCBBIIBRIbbKADTBIZBYZSCTaBJRTTDCKQAABYSbbCBJZRaBaTIZKbYAYLLRBABJRaYbSRbYaQSJBDYKKSSRDTIJDaYKTRTZBBCaSRZYTaaaRIAYJBDDJDQKKALBbYKSbYaZbbZBDCAQKBDYQbKYLbRaKISJKBbDBRaATAKLLBCSDIBSYQbJYbLZDLLTJCIIQbQYAJZAQaRAAKRCCADLJaaDYAbDBaLSaJIbQSSaCKYBJQCASITTJJLaIAKLBCRYDbRbSaKDCIDZQAaYQJJRBaaYTJKJaYYDKBBCSKIaRBYCCQKaKIBaRbIKRJRDACSLJbCRbSKQaTLBZRLACYZYTJJJJILIATCCYKSBbIJBKYRCKBCALBYADKRQBICYJZYRISZIKLBDIARaSBBLDKaYTKJLSAYRYCZSbSAbbBYTQQBZDbDaAbJCLZKCJJJBCCTYZaTRJTYATDQDSBRIQYITLSaLJLAbIJQTJIYaQDBJKABAIKDDCKQaTSLZbQQIDDQZTYbLILRbRBQbRCKBIABJAKZZSTDSTCKCAKBDSaQDRKZIYLRTbSBIaCBYZSJaZAabQBIKLaBTCLISaDTDCRKSCJZDSIKZCDRKRaDCaCSTJbYKbKRTKZLTILCIIbZJaDCQYBbDJZCbBQJZSbTRQQCRZJDYKbQICAAKQBLLYLBJSYQRDbYQIRIQCYJCabSDICRATJYaaJBKbQKADDIIBTQDZSYYIBDSIKBIZLRKIDJDKSTRQZSJJIBSJIZTSRCTBDSDCSDQZAILJDRSbJJCbTIAAAYZaTJYDYILJbRSQIYBZKZTaSLCYIZRAZBQARATRBDIDRJISJBTDQaLKKACaKZTCCbTZbbQCRSJZZCQRDQCBJACDJZZbSQZAQaZQIAJLJJBJLQSAARCAKITIZbZJDJIKTARDRRCbLITbbZZSCDaLCQbCISZIDIKbJSKaQAKJBSaRQJAKCYTACBLDIKJDICaRZZJbYRDIBQSTSbLBaBLYQIKTIbSbCCIDDJLSJQQCRBKbAJDbLCTSAKDDTYbJbYaKaYRaCKJITIJCLaRaJQKabYLbSDCLYTZBSLSCBQBDD"
"DCQJLSATbSRBbbYLLKSJJQDRJQJTJCCaaCaATSALJSCYDYZDARCIbLRCIBJDTbQKJKaDaabBQYaDCDaIQSZACBKABTSYKJaJRBIYbARRISAZSLaTYLIJbTbQKTTAYKSbAJKAKBCABIQLSaCCLAIBLJILbbJKJDLDYQATLQbCZaaYbSaZTAZCQYZJCIaYZKRaTTYYBRICBDLJBaRDCaZRIJKYJITCDKaIDSRJSaRQZLJRTQJBKASCALQRKRBBQbCACBRCaTbRRaBTYYLYICaJRLTaKIJISQbTYBCZKYCLKbRIBZKBTRbBCIYJbRJIJAKQYQJbYRSDQCCRKJTATALCQQSZbKITBTYQDSbKCTRITALYaDTYbLaZZZBYKCYYQTaaSDIDYZACKJTDKDDZbQIbJALJQDYRBRbCaBIQKCKDTQBRTZbBAIbTYbRTJaaCbLIASSbJQRIRYRYDLQADJbIICKSQLRbQKBaTDYRDbBKDSYLRTQBYCaJRDaYQZSKISbZCZIKSKYQLRCAKILKTJBCTSJTaIZYKYKARKIaaJAKaBDTIKIDKADaLaYbJbBCQIDIKabKCbKAYZAYSRRYTJTYRLbDSYLBTDCBaaZSBCKLTDIQTaLDQKYKSbCSAaBCQDYJTAZIbJQbZARaLKaTSJQIBZaQRLZaBJZRBTJRDbCbSYDSYBZYSIRIDJALDARJCaZCBSTTIJLSCBISZQSLYQLAQBACLQLLLIIQYTJbRLJbYTKZCCCLKaLBBTZaTQYRTCKRLYABCTRbDSZBbCYJSBDCYYbSbQAAAZDRCABbAARSLYCBbDJRbZIRbLBYIDDYBQJJYLYTTDISaDLJIDSRSDbaaKDYLSQbAKbSYBRSZLCRCTYZRIYRIYYJKTQbQZZQAICbATLQILJaYRQCbCCIbLIZJJAJSZbSIbBZYaCBaRIABKYRADTaaSYYCSAbDbRaLATaZZDaQIRRCbTRAITISABJDLKJKIAQAJaTLBAJRCD"},
Embed{84,268,"IQLbIJIAIYJIJIIIIAIIQDJQIIQIIIIIAQLIIIIJKIabCaSaCKbCaYbCbKKbAQYDDAYYbaTCRZZaaYKKKJYZITZaYbJRKBISIbBaATaZKRZCIaDRAKRDSYDQAaZSZabTJAKCbZAaTBASYBZTQTKRDTRKYLIKDRDRQKLaaDDRIAbTSTRaYTACbRKYQDKKZZQZbTIKKCbRKLRQQZCLYAbIBQYJRDZKYTDSaLKIZKBTaYTAKDJYRKTCTCYSYQbDIZSAYISKbYCBSJQbaRTaKQJbCIbTbYIBLSJTCASZZDaJQIALDYKBTTQBIZSTDCQDQKbJQBZQKbRJbSTLICALYDYIDRKKTSRIATDYCILJSCaBBSCYTRDLJBBITQTbLbLKKKCZQZBBKTbASDQBRRCBLLCbAZTSaaYDJLYAJQLQKALCDTBLSJAIBQTZAaQBAbSTYKLLRYRaaRISaBbBJBbKbZDaCZQTRKYJLJSCZYCJIQZADYBZQBTSQRKYBQBLQLaYRLDIZIQJCZbQYQJBDBDDSBJaCKKLQSIAYDRLYJYSQDRDYJSaBCLDbYYKBYaCKIKBSJZSTSbaLaAZLLCDJLSRBSRDLYTSDYIKRABJaAJJQCSaJZTDKAabTDaBaRKKaBIDYDZSRZTLBJITaRAaALBIBabARSKLTARAIDKSKYRRDYYTaJSACIKSZCBSKTABJQDJIJIQZbZCYLTDbLYBbRaYBARLKRRCISYbSDKSLRDDZZZRbISSLISSDBaYCZRKSTKDJaTDLKYAAYCKAZLRBALQARLRbBDSZBJSRYYSKTBbSQZCbCZJJRDIQBASbALQTTLJDaCZITRbCJATJBASJSLAaLDQCCbJQQTaZBJQaTDSIDBLBYTLaYLbDCaBQJDZRDADbbIQQSIRJRTaJbTQaLCISLBaCYCZJTBBKQCRYQDKbKZTYRQQRKYAbZLBCCRJDKJbDJTJLAQSJSQBCDLKQTSCRLbbQbZbKYBJDQATYZKRBARASSZbQBJDKTKBAJIaRTbaDZCRRQSYZDSRRYZQZCCIbaKCBKbaJILaYaKITCQJBSAYBBJSLZQIDBbAIKbSDJbYYTbRQRSbYZIaLAKBYAQLTCCQTYTZLRYIQaRLRBTbILLSKJQBbQZSaBKaLLKbIaTIQYCIJCJCADbaBAIDJZDSKDTTRRTAYaDYYJbILQSaAYSBBaBRBRRYBCIRaZRQALKLSYLLQCKKDIRaTZJIZCLCDTDTLCSDARJSZTSbIaKYAKIYIZaAYaYDRRBSRZYZQTaCKTbQDJBJaYQDIZIYDABKSDDaJJKTRaJCBSZTBTRLaQAAILCRIbZZQRaTSaJQSBTIbJRDYIKKZCKIQZCTZKBLBDBbDCCiZZKIaILLCCRaBYSSYYLRCIaCYBAbQYZYDaACaCDIbJBRKTIaJKJaLSSIAKDJYRBABQbBIDJJaRJDRZCBDTRBaIKYTTQZbQTDARbDYAbaBKQQQAbJLCaYJQCJKbSUCSQLAbBTJLbLSJSBKJBRaLaLZJAAaLTAISDDZYBLJQZKZTTJTCKRKSSRQJBRKCYAAKKKKBDKRaIRKKDLTQRZIaKBbQLICYaSJBYBbBSJiCCLbabYQSYZBIYaRIDJYRYDaTYABQIKQjbKBBaJZSAQTDKZQYYZRZYaZSSaIbQbZZRJDRJDLbSAJCCABgTCBTQKKRKAJRSJaZDaKSQDBEKRLCbDaSICYLbZIbSDbDSTTILBKIIJCRITSRTIaQbaDQAaCBLbYJLAYTQCJLZLKLTbJIaLKQbTZTACSJLDCaCRTTgSYBRQYaJRIQZjAKCIRBYBTBTZQDZBbBCQLRJbKKCSbYALTILIQgiTCJQIKRLDbSDAbaLCLZREICYSTSQAaZYSSYbAYBBJaSRDDLKCJJSECJaILRSZCZLYJKSJRSCAAcJIcZCIaaBSCYDcYBYbSjAQBJQTbYAIRRAKbTaLJQbKSLQBcLILTAjQCbKDKYBIIDDLZhaCBYZKYQCjSLajTDCZDJQIBQRZKUTJJYIIZSDbCgSZIaKJYISCILKDQYAMCRRbaiDRDYCIaaSQICJbaKDYTBUDLJRaJATQBZTRAIgTUCDLBIQKZRZbQbQRJaTAcQKLSTJbACRIZSDTLaLLRabRZgbKEEAQQJARbQZaSDJCCZSILCLRBYJZSUATQIAYLKaZQALIcKLDKSAZCCQYTYAQiLCDaZhaYbbATKUgKYBKMTDZaJhTgZAZbhBKIYCKJBRBSKAAiJaYScRAgDQAIcZLQjibAUTZSabQRRLZDTCDbbSSKaSjRDMCIIhIIAcDRYJjRQcBRZILBBQgSKAEIJggBUIITSSJaRZEaJIDTQATJLULIIhbiIEAJLAbjBKSSaYaDBcgIYIJcEDJQaEIBUAJBRKDbabABbaDJLiRjJJJATbQQYRAaBAgQaDjYSRMZMKcZSQBSYKQaYJQRUbJaKQCAILcZETKaKKZkUQSKhAZgCbZDLIIAZaRSBCQBKZkcQaBEUZgREIhUDSbLZTTTgZCYbEYYbSAibjaSJBaRLhaAZTTThaQUQUiELbhUEChYIaALKaZBRMRJCgaAakRTCRSKjSTTLEjEbJcCaAaZhJiKAYbbZbLASYKbEBDRSLgBkZSJIZAKKQgaEQKQigiADKTbZADKckDRDYKaCECKhCKbKRgJChJcJISQcJIJjLakiJUKaaihSEjBQbSKjYYRUiRLYaQbgaZZTJRAiZAacUILZcMcSiZDjaDYDYRRIbCibAKjSSQUSaTUiLRKKJDBMacIhYSJBLSkTRQKIYTMZZaCgYhQaJASakiUbccIjEIDYKIjLaSQASabbUTMZLQiACEkigJJJChDRkckDATBIbCYLTMKULDDhYiDLUTMMkaAahJIhgLhTiDERQAkYCTAKSDIZISLbZijQJUJSIBjUETCkDgUhhCQEBEgTBhhILIAbIhSARTKCiIIKRJjKgEMDYZDbISckKMDbUIAEjDhhCZYIYTAJALbJUILUCMCgISEYYCkiLcigIiEMZcjhBUAKLYDCJAgBZKUZUUhZgSjDJjTRTEJQRULSkMgBKKUjMSMKSRUYKCMjJTIQbYUJIYCjaRcQJZkShIQhEiSUMjiaJZZYbShbAMMBcjSjCKgIiTSACkaIhBEJKZjMMBDQAiBILTgBEEbScUBYYghSYRDBUMIiZhJMChYRkcEgiBEkhCgJCLBBDIBEZSZihiAMaEhbCTAJiRaKhJCEgRASaDJSRggcAZALMbgMaCbAJQjKjSIUjBMkQbSkbBMSiEIDLSITYShgaMJaaLDTLDacjEkkSgEEkBKiTKhLgkBhUhZKSccQQgTEMMEKkMRAjKQbMKAJULRgBQjYbjUccSSicDICIYZTjagBghYZRjUSkMJTKECJBZIgKQciMUZhaKUaKYSMMJEkBBKUYMikhZLLhMYYESASEDjcRkSYhTMQkJgLLJSQQRCQZSLcIaDAhJbMhAkMbkagCjckIYhjgYaZCJZaIAkQCLkMKEIgckYiQDJbTLiaaCgYUKcUjEAhBATbJIBIJccCShaDicLChUbDDacjSIIMQSEhUiBiQJKYSCTJTbTYZSIZJMLLbkTBiQDRESkAYRhEEIMUARYSLLJRBJjjLjiIBASgSSKKRiBJBQCTCBTIIbaJggYjJJLRAZURbaSMiMAYBbcTQAUTMBKSTDCLJEbLSaCcRCCJITAQZYBRYBYicabAYKTLEUTJQcDLTSRAbBYYiJRQCQiLRMhQMbAKDJYIbDTBSkBICaaKRDZKLKTRKDYADKCLcjaZILIbZYAAYQATEEDJYRKLcDhQBARLCTCSChgDYQcgQZITLMiLTRTDQBEJbAiRALcJCBTaCAcZMJAMBMAgEEAKZDKJTDbYIjZSbEMKCbIaRQkSBLSTMJkaRYKbSCJhKkTaKTZDTaDaBLBQhSZTQSahjZCJYBTEKLRbSZKYJTIIiTTAUSDgbbIKARTZDTAZhULIIkLYCbgZTSAYQSASDMKaAKIASKJkKTKhRLIJcCQBaRRRLKYMaQBEkiaQDZRDJARBBjAaLZDMJjbCYYIQYcYaKALiZRJCAITAQDJMIKjDCcZQQLbZSZAJZacaJCRaQBYKDbJCACSkYTZIgSTKZSCZIBLkbACZDKBbLCDbIhDKAgIIIQDYgDbBKKRYZBMQYASCjALYbTKYLbSgRCJRhDAbSDJJDQASUREQKCASbBaQDZEiCDQLKQYKJSbTICBZLABaJZBLBTIcgKYKYEjYZIgJLCBaRBLDSZJAILZaKZgQZDYAEKCCQUJBQcTKLZZREbQZaCSKYKRATYKDiJZbLSaTQLaRCSiaCLYTKKaTARSSbADBQTJhIAIACSLgJLaBZTTSBJKILBCLTJSZbaBgKIJYABESSKQKQKILRCbCTJjYRgIBDRbRRJSLIDJYAbiRQZIQjabRYBQSbIgDCbRiaaUcSMjCIRQYDJTQSbJLTaQAZCbRIJDQJYQYKSDTBiBaLiDYDRbIASQbkgBBLbUBRSQJaAQbjZACJJiBjZBBYZUIADKYLaQIAACALMRTJZTKZIJQDJCIMQDRILIAaZiYjJQSRYMRiTLYYCSQSAKBYJbAACZIDgbLYThCcTbcBZKbSZYAKSZbBKTbASQUbJTKaBIJYDJTCQEBBSYKRBZDLACCSDIbCLCajBChCKIATZbIRZIEJQRRZICIZKAASZRRIjZLRiZTDDZbTRQQASIIaJEDRSDDBbQLRDBBIAaKUJJSSRLLZcYaLIRabaLSKahBaACLbaSjQhaTQhbARYLSUTZQQDKJRQAaJaKBLSSASIiYaBDIgKCYDaaZIBAZRLZKbLSRIDYAbCKaYbIJbiYZaJUZADSaRCDLDJaSDgCbIBQbSZKAZKQIRZADRYJBaAJQBZbCCKDJLBaYSCZDYMLAJhhaakJZRRYbTJABCKQYQRDZRKDAUDJCBaBCDQTKQLLZKTKALYCbBCTiSbAATajZZiJCIRRbQTLDbSTIQRbbJaYRBLLabQDgLETYDRCYYLDRaIQBYTTKLJQJAIKaKQbKTbSjZZCQaRBLZYYTKLSCTBKDLIDZCSQJLLSSYKZDAjSDZTZTUABYDYSLBZRSIRBgTIDaagbDRTSLKbJRbaaLSAbKQLSSIbIACRZbZLCTJTLQLTBjRBSYCQCJaQYSDaBJIbaJLbZbDRRBRRSiZTIQQDZCKBCQCCbYbIIZbSTIABYKLQJIYZKYAYIcACLbQIBQYQZMCZaKSLJJDYBBDIQTKaSTQRaTYJTJSZBcLLRLaLLDbZJCYRIYBBQUKCISYjAgTTaRBKaQRgJaBIKJDZJDDJYDAYAJBabSaRDbTCLRYBTQKKhLSIYYADTTbTaTYLKYLUSKRILCYQbDTBbZTYJbKLQaCSLDEZhCTDQECKaDBZLSSYaJBaRJDRaSBZTSIKSUIAJSUaBJQAKbICaRBICSJTEQRBaYJTYYZbZRJKIJZSRbLBIKkLLKJCKCTCYYZYTBaAhJCDCJCBQQbZbQDJJRQBZZTSDTICYYbTQYBSIDDKTJYBZZIAIBCRKDTKYACaCYYYAACARBQBLIBTBaRZLbDKEbZDLZASCTAJJTKRSBBMbLRgDSICRTYZabKRIJAMYQbbQbDJbDIQQKKLYcQSADabRbBCLJJCQAcYYBBTIAJRSRCTAIRbZBCLLBACRYTKATLLITKbJaIIBDDCaRJLQQJaAZLQDLbKATjShBYEQQYBKQaRTDZBZASJRZQJLQZYBKKZTQJJCIJKABCZQaQARKCZRZJIRbLDKKRCLBKBBTTaRKQLBDRSYLYLaBBTYKRTDDCLDKJQTbLYACSIBYTZQSbZTbYQAYDRKZLRDYCIbAAaDJaBZKDJiKKDCBAKaRbiaCIDSYCLDDLQSDCIaLDZKBDRKLTBLBZKZYQQRbDYLRRYYCZIQDABUCYJSJDRDDabKaBRbBTJSARaYLQSAAYBJTKDYZDaLaLZabDAYBQIbDDRbQKaRaSDJacbbKYCBIJKaYSQbLRCQITTUISbCBbZZCTaCKKQJIBAgDCJLYTbTATJaRiZTTIaARbLRKQKaRCCDZabigIDKYRBCbLbIJDIIBBbZCLSISaJIQBTQTSCTIBbSJASbITJJZcIZDbZbKbSIITSRbASCBZYLLTBIQTCDETIRCRDCbLSbYSTALKSTTJbCaQJJICSbZaLaLKCQaKSaDKCZRbAKRaCYZBBaJLACRLYAZAZZRQaEJKCLLaQITbZJYTSKKTQATKLCTLaJLKQBATbSKbDSbTCJLZSCiQKTaKAJDYJYKQJZbIQAKQaSJBZBZAIKTYCSBQJZbJBCITIYYQSbTSbbDSZYYCTLDKaJTYSYKSJJDSaAaYKCSLTRTDIDZDTaCIBLbbBZSSKAYJARARZabIASICbJRKYLRCKBJKZZIRCCRQDSKKLACYLILQDZQQYDZTJLDBYADSbAJAbCCYDBSZRbbTQJRABYRTIRRQTiTBSJaCKRTQBZJYZLaaJQRcSIaKDQBSTBRKKAJLaQSZDACKBERQaLDKBKASJRJQLCQIZRZIbaKIJDDTYTKCCYDBaLAYZRDITQCRLaYLIQYSbaSJCJSZQaAYTKYZYbTLZTBCLYABTTBIaDDYILBBYSCIKCLRaYKBZIZDRYCZCDQYJaYBSbZBQTbCAYKSDRbSIBAJBYBIYCQDTDJaZaJJRCTaTBbRISYYZKDRYSDCYaBaBJJbgTaRBSKYLKLTLDYMDLZSRCbDUaBCYTKYQISQLTZbaSSKSTKKJYQYbLabAIDbaSZThBAUabQTSKSaaRSIbYKZbKBQDLQLSDADBAbYbYSDRQYIRSbaADabbBBQATRCTJTQRZJaSZSTLYAYRSRZTQQKLJJBBCCbBZSbbYCYZKJAARBQKAjZAZBKRQTZKBSJLLJDKIAaLAaJQRKDTLCDbaQACaZLLSDAKIAIRaJYZRbBSCLZCbbCYDLBLQRSYSCKIKTCIDBRRRTBDKJbCCQRLIRICaZYJIZAQCCDYRRZIYYQJQIbRZbBBLSILaATRZKYaLBDYARTCDQaSKBATJDaSZZbTDDYZASRaIaRYIIJRbCYDAZBTDSQaKIYbYTADSJABSCRKLRTbKKCAKSDLTQIYBDIKZDBLYTCYYJCKBbKAZRASaQZZbLYCbSLCaKTLDJJRQJQbbSCRDDLKAATAJRDTaZQCAYQLBQCQAJRIJBIKJRDaABIJaYYJJbaKSQCDYKKZKIIICDTBaIYLLJaCILZbKCKILaBLCTSZAQbCTARAQZJLDBCaBJaLKLBYLRZBKAKbISYIJLcIbRbaACTCBSAQSCaYIQKYQBQZZLEJIQYZLaDIQCCRCQDYLKLJITBYZQSZbQYQDDaQTSCCARCTZbQKARbCbbaJDDSRBZZDKAJIBYDKJTRIIKbCDaTbCKBLARDSAISJJYDaAJDSQCSKZRZTRbKYAJQZCBJYSbLBJbSbZDQSRCZDQBCKSIJLbAIKYITaKaSIZbRRZAYZIYZACKDbbBAJDCRCRIYAIRSaCTSTRDDIZIYRLSQYJAZLTKJbbYRSBQYJSaBTCZTATaZCSDKSBIaBDaZSBAKIaDKDCRDZSbZICbQIYIBRDLDaSDDZICTRAYIbKCLRAQCJCAaKYLLSQZZaCLJLSBTTAaYZYYaKhZQDZAABTIDIbZKKIRALDCbYTQaRDKRICJKAJDRCZCJZARACKAQLZBYAZZaTYLASZZQYRAIYahYIDLILATAIZAILCLARIYCZTKbQZJBKZYJBLAEbQQQBaCDZRTTJSYbCAAYYJYbCZJLKBDaJBSaBBCQQQJKCIJLKJYRZaACSQDJCRKSKLLRLQTRQIBCSAQALRJRDTQKILCZZSDCYIJDZYJSLbQJBRRLTSYSBDBaIYSKSaQTbRSBYbIYRSLBYCAZTBSKaJCYIRSJKaYLCTTCaCYBZDSRbRbKQbRTJAADCKZCJTACTTDTCKSSKCaRKRbJLAKJbaaCJZYCRLDaCaJDZJBZLLDCBZIBBYYCRSJLBbLAKDQbIBCALIQAKCAZILSaCaRIBZLaTKQTaJJALJITSSKCJYTTRLRIRBaATaSCLZCJRTTBYQCbKLCDCIKRbKDJjJJDBQQLBSBAKRQDYbKbaAbSKJCBSLTRSLJTZAABIQBRaLSAbTbYSCaRaBaLAKABIIaaCSZLYCJBDSQYDRZLbTbKLTCQBaLIBJDRSDZZLZbQCTQQTJBYRBKKbSRIaKIKYITKCbAbSDSLTRJTBYQKRLQKCRTYDIDTJRBYZhRCBSBACYYabaBKRCDYJIZZZbYYYDQSKBARBZJKKaaARbTBSKYCDBAKYbJYBYCYLDKBRaBQCLRDKQZQJIbBaAYRDbCCBSQADYKJAbZbIZZbKKAQKCDDQYKYJaBZDJQbBKaaaBKIDBaIDZLQLTSbDaCJLBAALADLQIKLRBQSSDTQSbDKBQCbILJZBJKaDJKbLbBJQCaJSQJCYZLRIYIJZJILQbRKAKYAKAAAQIRKKIZCbLABCRRaBZQCbZQBSQbJaDLBaLRARBBRAbbADDYADBDQAQaTJSTJSARSBYTaYBLbIaTLJRCLaaZBaAKaJZbbZSbKKJYTRQTaRDSQZDKLTaRSILQQQABDTCYACDBTRILQZSYABQCSRZTLRDDKJZQbSaCTaRbBZRbLDYRTQSRDBAKKRYZADaKRaZTRLCabSIABIZQZTaJAIKSbaLJIDTJTCBRCKZaITTDTLDLbaSDBYLDKZTTSIILYKCCCSCJaRKADIAKDbSIJJAAQaSbKKYbIYCaSDJDKJKaRLaIYSAKIAAKTIZZABAKIDTJLKbKTKRDDJLQQDaRLRZTLACDbCbZKATILSDJJTaARIAYKRDJACZKSYQbAIZTKJIBQJQbYQaYaTbQAaSSBJBLQbZTSZLISYDCLbaSTQYbaCCCDITBbaDbYIZZLLIJSCJJYRQQKKKBYSBQCJDSARbaASKKaKCYLCTBATYBaJJaQSSZLbJIICKDCaQZJYIZBIaZSADJZaRYBAALDYZTaRRBRDbabAATaCaYBKbKCQKCaBSBJLLSDDKKBQJSaJaZaYTCLSbbZYBYCTZSaBRYBSYIAYCRRIbQZILIRZDCaCaLBQJYDQbCYTRLYZYTLaCCAYABRBASbQTbaLQJLQaJSQIKDRYZSZbDKIBDSJIZbAQZZZADaSQIaSBZIAZaaCZQJKKLRLDKaDZYQTQbBYIaCaJJLITaCbSDYKIYTLJCILKBDRJAbLAaKKYTSTLABAYYaRaZaQJYCRISRQaYCIZZTAZRJQaJDRJBCLTCLTBaKCQDCADBIBDTDbbKaZYTLKKDDDBbDSRIIbQLQCQZTbYTaKAJQZRATBaaSLCbAAbYKARJQJRBYaRbDZACZYaJZIRbCbSYKIYLaCJaKSbCIaSCTSRbIBCDYRLaBKYDSbAZZKAQQDJLASSQLLLBKbTTRaAZZDaAJARTAKIJbYRBaTRJQabSbSTABSaJQCKTTBAAYTDZBLaJRSIYTDYbJDbBDCIbDCYTQaKLISBYLRbKRJCJLAaCCZBZbaRCSTLZKKDYbbSbCJIZDIKJaRQbaCQTbaRYAALYRCSDQDDLKBLZKCBKSCKAZbRKQQIbJBTJASTQBRJILQJCRJYYQJYRAZRRBJCJAAJRaBLJaQLCBJBLCBLQBARSSDLDbRTKISYBBSZCJAbAbDYBaKLSDCZAJRJbZYRYRIBSDYSaLTCRJBaLAAIJIKTZRJYAZIaCCaaJTbbSQLYZSRaQQBLSQRSKYDTaIBTDTSCTTQJCTYIZBTSYLLJBILaCZBCLbTSRQRLbZRQCBCLbCAbRLYaTBQIQaQJAIRQZaZRKITRDaLSIRaBYYRBQQBJJLQCJTJRLKQDCZCIAJZLbBKZDBCAALKZTITQaTTbASJTYCKKLaCQJCCZLBTSLBLDAbYIBITCTRZALIICBBbADZAZCQaIQDYRbZYKJIIIRDaYISKbALaDZYSJQDTQYLQDRYYbDZJKZIDCYYRKAIBATIaaQKQILaZ"
"TYTDYIYKaDCaaBIYCIATYCKCbLIZbJRSTQaKbKLbDQTYSISIRSJYRQAJbSCSJAaLZaYaLBaTKaAbKJDbBKCLJQKBCbSJKSAaCaCBASJDALDZbKZKCYbZbQJJKAIIQBKATZZaICZSTATIZaSRBIZaKYBQKYIACJBZTRLbKKISIbaDYRDLJQTJJbAARSbaZaJJbTACQQYKKACLQSYLDADaBSKRTLSAQDRTKYYBTJJBKbLKbLSbSKDBbQQYTJCICZBTTLJBbbLRBYaJKSCCKQZBTJQAQCbAJALAZbILBbbRbQDCJCIKYDIbDaTaBKDASLYKKaSSJLCQKaTYYYLDSDJaSJRAaAKDaTRABQbDQIYRSTJLZBLIKALBaQLRBDRaSbYACBTALLAIAbIYDJCDSSARaAbKbKaDZRbCbRJSaIaTDLJaBQSaTaKaLYaTTIJZSSaSJaBTLTICQYJLSZbQQQQZBZJZRTSTBYTIQQKZZJCTCJTCYRAJbSJIIAbTQQIQIaCbKaJRDLJRQBTTTCTKBBQYDCTbbLZIAYIALLBJSTDYRaRCAIRDCRQJILQaYbZaCRYRZYSYbTaYJRbLBSaCRaSZbJRRaSbTaQSYBKKYDQZTCTZQTKLJQKYTZDDRCAIZYRTYSCCAYbRILBTSAaQbKaKLYZLLDTDTKIZRZKTRBAJLTbIaCDZDSAZbIIKKQRQSDZAZARKYRCBRDZAZDLAbYZSYDQSDZaBDabTTaSTQbZYSZIaZQbSCKYTJaDCRLZRIABLZCbLYYDTSTTaCAaYBLABYAKDaQTbRYZbSILJCRLaLQCKIQYJBSSQaYBDKRAbbIYJbACLCaaDTJLbILCKBSDBLCKQQZJbAQaRBTaCBJYCSKbYKRBaBYTCbLLLZLQCBSLRbDBSDCYYRRYKRRASYLKAITQLKZaJYIRILCISRRDDDbAZSJbRBAAAZIQTIBKRaCBZZLQYTbKRDSbbCCTQCTSZJJBAZaDJLabYbCTRQKAIBSCILCRKLDZDRJDTBLRRZbKBaCSJLQABRCIQIJDICRJaTAaIDLbKaZCITTRYYDARTQaZLRLIYRaABRSAABSZTIZaTDSZZCIBSBIDDDYAZbIBIJDTTIZCTZASZCQZLYQCJTJQbBBSKQJIbBIbaDBJKQITZaCbKKCDIAbCKDZQRSDKSbKBJQBLBQARZZZIbYQILQRBKZKAKKaRabKCSIDQDCKZSCYTaAQRabQKAILDKLJKaZaQbJKaQTbbQSBLSQYIT"},
Embed{85,272,"JILKJIIBAYQQQIKIIDYJAAIIaIJKBIJYDLIQIIIZQLYLRbTSTbQTLSCTYSLTTTTQbBTTYQQTTRDTTYTDRTKbRIRZaYAKaSBAQITBDLRaRITBZITBZYDRKJTBaYQaBKRRbCAQZICDJQSbCKRYbILSLSBbIaDJCQBDBbTRLbACBbCBTJSKATQKZCTTBQbZCYIIaLTBBTKIDKLIYLLCYLCTAaIBDbALIYKIBBYaaDQBRTJTDSbJIJaDAaZSISCYYYBaLJTBYJLaITAIYYTKRACaJKJABYYLSBLaJKYDaCARSLRbCIISDTZYRYQRDKTLRCTLabBCaaLLCaSBJYDCAbKLYbbKKTbRYQKKCZBBJJKLCbQLALSDBTQSIYQYBKSKRBTJKIbKaQYYbISbBCBQIbBKRaASSDKTQLZLTCJYBaSJYYYICTSIZCZRCJDCZAQRbSTLLDQLBLKRCCQKLZDBbaZARSRKSBDYBIDbQbKDKQaZJTLADALTKLCDBCRJYZLaQaKSLDILLbbAZSABaSRaBBAAZKIaTJbCDZaARSRDKLJAQLQRJJTADARJQbYTSYZRCBCRDJZBYKSSJIYQJABLBQRJTIKYRYTKRYDKYKCTLTbAKJAYDDSbYJJTaSBCBBLAQTTTLDCLATZKRTKDaYBaYSIIDKLCYQbBBbISRKQCZDbIKTYDDBIKIKJLAKRAaIILDTIZRBTBKKSbTTBATLQYISYDTLCLYAKQJSJYBRJKADCKZLJaYbZILSDKaAALQIZCQIQATLLCJbBJKSIJTTYCDJKAIbRQCSZCCZZTAAYZZSJBDJaBJBKDYQLbTBQDJSQIYRQQaaCbbRCLSTIaYTYYYDKIaLaYQBQJASCJRRaaRBLZRKCbKSZBQCQRLJAbDIYTDTKaSDYbCJaaAaJCTQKJARKDILSRLZKJIDAYYJRIbQQKDIRAYZZaIbaZDBYQAaSRQQBKDCKJbSDbbQSJSBQLLBLQCYQIYbYSCBBZJRKJRBDSLBYKISLBZISSaCJbaKZJKBAYLSZSDYIIBBQbZKCZISaYSbYCLLQQKJLZIRSBIYDbQYCbbQKZIYLDRBQLDSTQQSSRJLIIRIBCCJLBILTJaCaLaKZbbBSZQLLIRaAaJDRKCBYACLbDJRCAYTYBCaABLRQSDZCaIZRARSKZDaDJATLYLZIBaRRSTZCbaKCBLSaYDSDbJADRaRYSBIaSCISLYbYSYaaATTTTJbaCJJDIDJDKKQKJALAaQRaLYJAIDYbRLQYbIKAIYTYQbRabaaQYaIIaBCLLYATbSCbTKYBJbaTLKZSKRSbDYCYCaDbIDQZRTDaBaSJYZDaCQRQAJATRaBBbbACDaIZYBISJSKbDbIDYTKRLZJCYCADKZRITBSJAIZKSCbBZDSIJQSATLBCDbQQQSbZRgbbJBRQBYBICZJTZSRKRCDSLJQBYALJbCKaDBabDDBZCAZZDbQJJQDBIIbATKQKYDQbBKLaSBSRYLICJYSCIYbZLALLZTCLIZYBBKQIYDTTYAJQKBbSSACIBDASZQRKJILZZIIBQbAYAATAARZKTTJQaJKbYbTRISBKTRYAJZQYIYTbJaQDaSKADLbYZZZCIJUBKCgaRIJTQDLZLQBCQJbSbKASBZYBKTQYYALZSJaISCaBQJTLZQRTJKYJIRYBJLQBLaIYTDSBTaScbbMKTZISJABRIDSAALLTbQCbQLLaRIZLJIDQBCbATbJTSLCCCSAbDJQQBIIBaCKKaCZRaRIBJIJTRYDKBITRBbARATAaQaYbQRKLDbCRbBITCjDRRZAKAZIIKBKCRZCICRRYBLYZchbUJTAaCMZBCaJCYbDaZbIAQbBYaaJQJBaIKSaRCKABCKTKAILIZRZCLKaBISDTRQYRbSTDBACABJTBBZQYDKbQSZLKKAIYZJLbSILSTAQBLTAAaCAZDiCCaCBQZDZZYBSbQLJLaaIZABSBbTbKUBKAYTKIIYTSbLYICKAKJSJBYcRDJSCgabLABSIQIJBKIJTaTASITKYZQDCbETbAZUZJJYDCICQbIQJYKLRSABTBTSQcCYbJSKCJLTKKQZhADIIZTTEDUAZbSRCQYTALRLCKBkZLLQbDhZZZDAIRJDAYDYIBQIaQJRTLbQDJIQRAIELIhaLRQTaDJRaUBaYCZUjCCabbADRDLYaARRDTCBjaSLKiDSKQYKMKMYDIBYbDUUJMIABIYUJDJaaRQAZSYDaZCTcSLSaZYBJKiJLaiLCMLiYRiSabbRBILSaLZUAQBTMIaUJABYZIaCSYCZQZAUbYjahYbIUZJCbQARRJbDaCLSSKbQRJIDUcIIKBDcIgCSbEQRBaSbZiQhbCaCaLBTIKiASDEZIbJSIAbiYJbhKELaCLSKDIkKIigJKSRAJaZEEBYZKTYBhIbYbbTgUCSMAQDcZDYJDbLLBEITJbIYJAQagLQQgZQBLJQTgLjhaAjQAJkKaJaAbAbLUTZSBCMYBTKjUaCchAaJcgYQLCaSBbJhSZSYRUJYIJYgTSjKLibYIRiRbKQUaAYBiKTCkTQiKjZLYQLEbQDLjAKbLCigJgKbKcKBYbTJkbDhCIAQMSaLaQASJRRjJRADcaYRZCZJbQgEKCZTUkCLhjAQSTcCCYRbSicBIDRhKQRBjAIBEASCLZabUQCJRakbDgDbcJQBcCSDBYTZRciijbABMQKKJRDBDCAKJcgBjUYZLIaJhaCISQkIbLBJMIIYiDIRChKMTbCbkbSbIQDbTAJkcSbaRUgCjUQaIcLRbQZUMhELDJCCSDSMMcZajZIcgCYgbTgRCgbgRjQhJiATjUCTJCSDAEMDjBhMaRgZJhEKJMIKKYSKbISgKSJajDYZZDRIASAMDhgMQajIhYLhYbCQkiTDZMkbBSLaSjYDhjiYhYLbCLaaSAkJQARjbgAZSThYZcTjajSSShLASjAZSkgYSijZLZZBAiABgCTcZJjKSZEgAhRcjaAcaQbTiQAQbDKhRaLkbAZMMQgTihjiEDCUMRkacBKMBBMYBBScYUTRgKahKMcLTbKgITibUDRYiIBabiMQaQTYCDQZLjMCIRUKBAZibIbhYRUcCTAJigibhLSaEUijiMLAaaIgaQRcTCKIbhDYCKYAiBEMjgcMJhMTgQgccYjKJCIjZQJLgYAbJBhUaSiQcZCBQUUSULbMaSjhkMjKEDcMKKATCcLjDBiCYSEQcAZLiJgiEgLASUMiYTiTJMjKiaYDKhCADJIRacAZgiKEEDiUkCEUaTTJcMTDDMZMKKYLMJgIAbYTRAkiLQhTgTZRZZUIYcbMUhaBRJiQCSKDgkSLDTIRUiSbgIaZkRAIQZkMMkKjcCbhZLbgYihjTUjLQEajSaKcQDAQcMSkJYgUJbUETLBKAhTDJIckbDkhRRRkRhbDKBRYSCjRZSBMgbKkZhbMaSiEghSTQUiEBZDMaiUEYUjjTKRJBRThZRKKJcbKQRSAgcCiRZRcZkSggEMRiUAaKiZTADjikKZckKTRjLQRgJaZMKkMTEBYggChhKSkEaDILJhShAAiUBiLDakTZZUDSMDICBUQhgaLETAIkgQRZACLILEKTKjLZZMKgbkkIAAcMUMDMEjiAILjKiJKJIbRaLjiiAEDjkKckESIgaYISLLCgciIUDTQLBBBUZgISKIhJQKYBRjSbAQYIYiBbhUbCQYbcBKJJUEbcbELBTQDQCKYbYZJZYRUgaQEhIbhhRMQhKKMMhLLTSEEgQZQEAAJiacZZECIZAMMcaiAMAKbZIRRATcBYBRDMJkDAbKIDDRaTKBQIDiQUUELBKaBThTgYZJRKYLYbZYbISgSaULaLCRcMAjBQaSQTRLARUDRUhbCYijIICTbLKJSUTJMbSQBbTJiYIJIIaaCMcDCcaTLKiACgIDKEKaQbaDEDCgSBSILRDCIbaAYRBhJBhKIZTYUjcYKIkTIjcMDRgSjSbbIbSLKLEaaYaSCJbSQSYbRALjDLDBhDgCKAYQQTjISKLLQZTBRKaLRAICaZDBccJhTgATBcLAiKQcAIAiQUhYIZiIARSAaZAaaCKThgQDTIDDJLELRQILLLAYAjLKRiSZLYIMYBTiTbJTAJYYRbYAQCBTZDLTAIbUgBQJIKULjCYBQBCDSDZADKkADISJJSECLBRYAbZIJcIDBhhJQIaJCTDBRLZhIYScTRDDYiLAiKbCIZITRIDSRbMaaYUACAbIRYiTTZLbgChZDhaTaJIQjLgZAaUIaSEBDEKKaJCcBJQQCIYTDacZLaSbRJRAJRKKIRIDRiKRRCYaBCQDASKaRLQYaEbAZjDYSZKZgSRDhIabEATaKSKRDJSBbIJQbRRDZYbSbaSAQIBRSMSSbZRYhISRLIJSDYCJDcSAcLCBIJkJCICCbAKCaQAQLJDQjCZYZJBiQDaIKYbBEIMZCKbjDSIQhjAaCbIaDZbRLRiTTATCBaLRJhQLBAKaDITIBAZBBaJEYYaTaSICKQRbZYZAAKKYkZbDkEbbCKYRaRYZSjKJAKTBjAJDTTaRAjKYaTShSDTSaEJaaJRhCjKIRQSCQAbAaRJDMSYKJbAQZgCKRALbAbLDTZABKLQjKJQZLBCSTZLQcAacTYQSCBbZQIKZZRaLahQILLCgCaAMZRDSZUjLYKaLMUUZDYBBCLSRRIZKQDAQRbCDYICAYaISLCYTDIjJCJRZZCKbBQJkAYKIRCSQJhBYRiIcRDYTBLSbRZaYaSSaQTbRSbgbQjYILbZQJLKQAZhMbQCKggIZUbSIBBLbZJBLQaBSRYZDADSAUAKIaMRiacbgYAKTALJBSaDLaQRDRBDhcCZQCRQADRJAJKRTaQaLRCbRLRDDYIKJjZAQIQTYRQbYIUIaKTjRYaZCDQKSaKYQYKRbRbBRSSCYLZYARCCcbJJRTYLSKaaAJDcCJaRbLBITKIZQKCUYIJAYZIIZLQLCQKBADBTSBaALLARYhRLIBJJZDBQBRbLZIIBLSAQKaQZYaYRbIAKkaJEATAYYSDKYCaZMCBcSCIICSBCAUJDACbIaDIaLbBTBBTIBRCLCbiIDBJbILQYYYbTQSijaBjbKIKQITZIYSIjJRBRDKABRDaQBLcQBQZAbaILgJQYKLYCQIJhAKbLZTIICJIBBZRCRQUJKJaSZDCDCAQaDKCgaZbJSBCITabATZCbRbDZDIDBIiEbAbITAUDDDCZbRRRBIDAJBRBYLbQbSZAAQJBKYbQIaaDDAbBCLLDMBSIDRQCLSbLAaaQbTZaLCQKLIaTZYBKIKEYIIDJTIASYYJEYTKRLCJTYahaBZBaKRRZCAARiATJRKTBKKJAbaJZDYcIDKabgDBBCMCQBIaLTQIYLZSKTTSCgQIaDLhDRJDLBYDSKJTSaJRaZbCKCEAYSgLDBIYYIaBaTDbLIKbbKTaQAJSSSYAQBARaLDYAKICaBRTcLDKJbRKZSLZRLLTDRIbTCKTbCIZCDQQhKJJSSMSRAIBLRDBQaQBaBAZYTIIZCajTYDJhSMCAQTbLbYCaCJKMRDZAhACAKRQQLYYYSRbaiKITBabYBDBALJBQKQSDZbSIZAKZQRJhbSRCDLSLZTbQaaTDRaYTAAQBBQbIJJjIZCLTIIcDQSKMJBgYKbaDBYCLSZBJBYTbQSDKKLYCaiLTLREDaIAIDCCTATALDKIYRQIIiaJIBIRbTAKLLADIZYIRZRIRTLJLLACRaTSZIJCiKQYLZSiKRQSQLTJLSALDZQaIRLaLhBSQKQaCaJbKAbKMKQTZAZLKQJZYJaJbbCSCQRQRRJRTBKCSTIbaLaMISDSBYaTTTILYDADTQIABURZDZTBQBQRUaTMTIMZCLCCKDRTSSDbAbJaaATZYJJbQRQDYBBbITQZJYBZbISTaCZILCRSRbZQTUZjCYTYJJTBBDAQSSACZBYQLYISDRRSBBIRRZJEYRQQKZbZgCITLBZRJaTRKIQBQaaTcRDRCaKRaZITRbYYDRZbgTIRSYSKAaaYCZARibBLaaDSYbRIBJJADQAIDRJQAKLSYDKDJIaRaEJYRSIJDKBUDZAQYaRBLQQCAZZIAITKbaYZJhQJZQIRZQYBSRSDTQTKAbBbCBIJYSjJJQAYJESYYSJYSKKLLJAKLaASJCJaRRAQYSZIZKJBYabLRYabSSJCLLbZCYRBSCSKDDLLDBRSYJTRbSYRLDTICCBSKjBYZLDQJSQBLbKTRCZKKZRiAQRZaQTRTbABZhLaJaaSCDIaQAQZLZADaDAZaITAaBLTbSDYbLLZYDbKbCSJKTRTDZQiIaACDBYYRDQRBKJZYZKLScBYIRJZABAagSCbhAZBBBSZKBSCRaYCYQJbQTJQJKSJBRJLAILQCDLALRbBTKRYDbCBQRAALJZIKaQYIDQKQIDDIJKaBBCJARaCbARDaKZSJZTZTYDCaSJDSSZYICCIBLRIBSSYLQSSDLAZKKJaZaBLJRKSAaZJAaBaCBbAKbDKYLJSSBZQTDKTQKIALLbTRbYIaYRAQRCbCRSRaSKQTLbQTRLaCAbKBaRaYcTLbDaLTDZRbYIQaDCYSKIQYQbYTKDbQZZTQTQaBAJTLDLAKYIBYQKZaYKTRaSIaTDDCZZZbAIBTDTDLTTbBTIZRADQATDaRRACKSZSbQBCaBLZRYALDCbAYTIQUKDZRBQbILbaaRIQbYaQRbRAIJISLQYYSaAQKIbbYCAQAKJbCTRJKZSDKLZJZZIRaLKQRRLSBaSacbIaBCBTBTaAAJZaaQDAIILTYLSADbbbAIQCbSZKRKRASLSDYZAZYSRQbBRabBBbbCYQDLiDLRDLARIKZTIbCBBCDATICaJKTLRTTCBDaZLKCJADKDLYaCSISLbCRbQBBAYIaLZbRTSZbTSSCRKjKZBKLKCLCYZKLJDRZIYDbYZBaLTSCDLaJSCYRSCRLIDARBaaQIYDKATTDRDbARARQKTBAARBhbZBDZKSTLbLAIYSAIbZDDaQZYYBKbAaLRJLYYBJTAYQZLLbDAaQIAKZYSaQZRICDaQSZBLQIQDKABSaYLDRRRZTKQQaSBKYASbDRBZaTbbYBCRYibIIQDBIAYJYbABDQRQYDJDRaZQYACAaTBBLbZLbDRKJTSRaKIKKISTRQDDABaaIZCJSRZQZYZBTTbLSATLabYLLJDJLbACAaSDCSYYbRDZQQILQaTCDIQKYQDYQIAACTZRKZTQAJKRaKBYBQAIJaCQRYbTRKDTaTKRDbBIYYBQLTAIADQbZBKaJYQJbASLCbAJARbDISDAYJZaaaLQBICAZbIBKQBJYbRDaABRbLCbQKRYCLaTDaKZCCRQYZQbKSJKSTLAJYLDbbQRLLAQSTabRJBDJSZKBBaQAAaYYSBITaaBSJaCSTbZYZLDaaCSBJCBbQZQLDRBbaDBSbDRSYCBZCaRALKQYDCLBDLbZZKILiLJYKJCCAZKCQRQbSbTIKCIQISAbaAZQTZIJILabBALQZIaIYDbSQYSKJIQbKKTJQYSKbDbBJbDJaSIDRKQDYaZISSQTDDRQSJTYDLZZYZbBAJaACTSAZDKbTSITIZIISbZTIAJSQRTRYJLSIJJYDZCSYaTLLIbSAYJSYJCTJBKKTaIaASRIJSSIKYBYLKALRTTYZTITUJKSISZBQDAAIALTQQYTaBTIZZBKLCBKELbDTBSSBLCJCJZRaLBACRITEIZZKDJSQTISRDbTJRAQRJJKYZIDCYRZSYYaaKBQBSAYLBYRYJLARJCTQKSKRYMTSBIQYSbQabYYTLBSBSBBDRZYTYcbaKYKbSDQQIYBQSLBZYRYBbBDCDYbJQDaZiARDbJCTBBTQSZKBaSQIILCaZIZSAZbbRbTTTZaISISCBYABYYRDCaLaTIDQQRAIYJTLBCBBYJbSDTQDDIYBZZACbYERSKIBDIJIKRZCYRDZACJJBRTSTRKSIYLBZKKQTDbDbTaCRYTBaZILZCTZBCITJICZbLbIKRQTRQYaSbahIJaTAZBCaCZARRKSYJYCZLRQZZQaAZCAAbSSRTJAAbJKJLLQLTACTBbZRIJICSaDTBZTDLDKJSabQISQRLgTYKbSSYDJRBBZYSAbTSTJTKDRTKJaARSDLQbTIYZbCICSaLKATJZYDKTQaLACBAABbRBJJZASAaTYYSJLLAKLTASDLIJbKYaSLSJAQIIZAaaZBLSbBYSYDBaJZaQKBRbABDKAZQDTbQRbLCbRSYBaBaLZbZQJIAIDaQCRaaJZLYTKKBQRRSITRSQaIDaaITALRJCbSZSJbADRTJSQbaYRKSbaKJQSbTJaLDSYTSaYLKTSQTJIbaLYJLJQJACYJYaSJAbRDALAABBTRbASITaQAYKQSTCSbaAJQDIKJRZJLZZRIQLKRaJLLYRYTbQTAKTKYAIbCQCLATCKDIJLJZIBYbKYQCKbCSSZBAZZJaYYRSICIDRILaLDIDYDKQKKRSQRTKaYKIaDJaTZbCaRAZQKADACZRZbRQQCZJLDYYTRDSbIJABLBRYJIRZCDKQbRaIIQSCLLRRZTQIBTTZLYSJAIJSZbRJCSCKaLbBTTRSAJKBCZJLLIZKKLIZZQKZSaCbTDDSASRBLbQCJCKLACYZbJRTSQDLBAQTJSLRZLQADTBRTSJCKLaTKLaDRYaJCLbZDYbJSSJZCKSCSCaBaAKAQBIJaYBCZbQDYRZZCTJDATZbIICCATYDJZaQLJSSLJTBYSRSIIaQRRZaZZQBaIRKSJbQBaCTCSCaYJLJYaRJQLRCQRLQQaSBIYDLZbLYLTbTQCYRTYICTCDBKKSIaAIIAKZDZLSKYKQLQJJRDCYILZSRLAQRBCTLYAYBLIZCSJKLYKCDaSDJTaDJDTAIbLILaBAaCYAIIKJDJIALYLLJCSBBLCSITbCAZTTQYQYDLZKCJALRKTBZTDKDLRDKATQADCCSTSbQJADLLTQQYSBLRZJaSSRDTTJYCRCJTKQSaCIRbIbBTICJDTRJTISZJLaLAbLbCbLCADYbAYYLRSYZYYCZaSaDKIbQaQATBRKDaRABIJLKCbCaZLQYQaaQASDKJACZDDQYQLbRDabRSTbJQSYKRQCJYbZbDYQTYTbDBDTKBKYZCYCQSbTBAJSYSbbaSCKCBaCTZKIbaALIICTJJAbSITbaJSJSTBKBQKQKKCaSBILADATJRbSQCSJDJJDIJJIQKJJJZDBbRBYSBLaBYSARaKLaKLIBKSYIbJYZLSYLCbCaRSBSJDQBRIQLYJTLZAKYZSBTADBQYSQZYKDZSDZQBJICZYSaLRCDTaaTLDDIAYRQTSJSACbRSLAbSYJDKCDaZBYZIZDARbCRZJDBYbSCDRRYKLbZSLRbCJKCBCSJICQQJTBIQbJARACbbSBYQDaQDDADLDDZRLIDSIbDbZYJBSRYCaQbZaTDaRaLaDLASRDLZTCISRSZaZJKbDAAQIaTLDAbBTaTKTQCAQKJRLaJbJTYIKRaaCZQKRRZYIKaDCAAabRTBJYRQBTYITQJSTQZIDRDCaZTTBQAIJKZRZLKBaYRTYBCTaIICLSaRbSRDYDRLKSLSIaQKRKSKYIQQbZJJbAZCQJLTIJLTDRTDDKCKDYQRQIKCIIbDTabARCZIZYRBRJRbATAKZbLKCYQQYRabRTSJAQDABJDSBDJLYaDDLLBRZRKBDQRLaILZCIQSKYBTaATYKBLCZTbZCACCZABaLZTSBDIaSYJZLDaaJLRTSKSJKDQSRATTZKCZRQJJCJCabRAKCJDARAIRIYBaCaRDLZaIYBDJYCSCDaCbQYCZTaZJKSJJbabSBZKJCIbBKLTYLBYABKLDSKQBJTDbaIAJKQAaRbTBIKbAICKARTJDBLaBbIAIKCZSICJDDIAJaaSSTCZYLTYaZBQJSaSKJILJCYTJLTJKJYTAATKTAZDRZCJIACCKRRTCbYaDRIAbAZKaRCLaCYILLKSDSDIDYSDTAJKRRDbKBARaIJAKAZLaQLCKJbLSIBDQbQJTaQYSYSDbJLYYJTDabYZAaSSLaR"
"JDBYRCJaTbIRRIDJTLACSRSaJYaCDYaJKJRCDKYTJDBKRAYYbSSQTaZBCIbZDTAaLTIICTDJYKRYIKKCDDbbQYARCYBCRTYTDZCITRKbBTaQLZbTaAbBLTbYLCQCBJbTIALZBSIRbTYSCTQbKRYTSRbYbCKJDbLQaaJTYaIaDTZJILBCJLCDIRYAZBRALSDSRSKTTSCaADTJKJbAKZDbTJBaAIYLYaYQBLZDCIBLBLRLABQQCABKCbIYZDCKCLJYSbBLIJIADbQQKALaaJCKJSYAJAAaLYBTDLSKLTabCIIbDIbCLRCJKRIBSTCbBTACDYJAQQLaLTZITDCDYDJQAAYbCLSBDBKbRBbZAJKZYRICSIBITSLTJYRSKbRDJaDBQYYZYSYSYTCSATbSQaIBSBSJZZCZAZSJABbBaRJYKCbZLQaAIAAADSBZKKTBbJARbISLYYDZZAbAKQJRYQDRQIYRDKKTLZDKRaZCBATKZaLRCISCLIZTBAKCSIJBBKZKBZbIIJTJKaIBaIKSaICCBBaZCSQAIATRTQJBbZQbTAQRZbLIDQCZJBLCaQYYAJSAKADTRJRKCBbTAZRYJJQabYCKaZZSTITabJKICDYbbABCQIBIKKCLKIbCRYBTBaQQbTYZDDKIAIICACQBALSTLJbSDTLJDALRLKKTZQAAKDDJBJAYDaKDAbISSYbTICKBSRaBZKILTSKRCIDCKLSbbIYaCZJDDCDZCbBIASTLaKRALARbZZaCLJaAaDaIBDKATIJSbQLYADSJAaYSLQLRCRIIRJQbJKaJTRBKDJbDabRaQZZTCLQLQRKCKaKIRYSRQBKQCCTSIRDQBSJSZJTZCSIQTKAKLLBaKCAaKLDZbTCbJKTbAJDIIBBSCRKAJDTARABLICILTZYCLTLKaRKBDTISIJaLKIJJZRRCRIRAIAJZACATKYRZRDKbZRSRCARAbCaSTDTZZJQaSTDCSKALKTKLIBTQRDRTIaRDLLQbbbDBCCRZbSKTaSbTJYIbTRZAQYaBJTLZSDaLYYBAAaCIRALLSQCAAKbDaQJaLLLCYaDQDKTSKIDAbLYTSRIbDSQKYYLbCKSbBTBTbBJaIDTKYbBIRbIBBCJSJBaSCTJBSaICTJSbBZADBAKaIQLKTIDTBAZSQYIBTRSLYIIJTIbDaCbBBBBSCbYCYaaDIKCIQAZRKKDLRTITaCbLRBZLJLKDBJSJQbIKBTDJTSQZJLKQSBBQBCRDLKDLbKYYYCZTQZISKQRZSZTbLAZCKBKDAISRDBTSQbBTTDBITYbbSKRSaJDALABTBAATbaATZJDDbDIIDDBDLbIQAYSRRRZIaRLLaYSBDCRQYDRADYSTaTJSCYKDRbYZKAKDLZbaLQbTYJYSbBLKDCBIABQDLAbYJICDQDBASYTYJDLCAJaJDSRDRLbKRIJDaRZCLJAKQLYLRSSDbRRSLDYYDbIBLSLCTYADRYDYKbLTCQYYBBIKCCCaSTYZbYDDSTTTZZAKBDTDALKBLTZJKIIACJCYQKQICZRaIBBbRKTZRBBAADYaABbLJSZZ"},
Embed{86,275,"KJbIKKYYITYAIQKILIIJRJCJIAIYYALYYIIIYBLACYAaaZDbaaCaaYKaAaaaJCYaaCCaaaZZaaaSQaYaaSabZZIRBaYDKRbCITATBaADRKATRKIaTQYKJDYTJaIabRKJJaDIJKCQRJBaKTTIAJYTIYYZQTRCbKIIKDYSBIaRTTBbZBTQYJDZQQDaYASZDYaaAIbRaDJKAKATLbCRaaIJATZACCSBTSaYIBRSIZaTDIbRYBKDZYKKJbCALQRRJJLZCDSIAbaRBDIKDQBaLDYbKQDLBaTQZCbbIQTSRKZBBbLTCIaTBaABLKIaCILSDQbRBSZaAYLKAKbTBRCYSLCYJZaKaRKKBYBDTZQYCYIYRIDCCDQZAAAILZYRBJLKDZTKRKRBSYRARaALbCJTLRADaRTKKLTACbSZZSIKALLIAKZBITKBYDJKCSDRSLKISRabQaJLITRZLQJKQBARZCbYZDDQCILaYYTAaTCZBBTaQKLTDbJCSCYBaDRZCTZYTLQKRTTRYIAYBKDJCLYRTARLQQRIbAKRaRYQJLabQKBLDAZSKDZZTSYTDKaAJZIKTBQQKSYIDDZLLSSbIQSJCAJbKbaIYDKaKSTBTYaSbTDYQTIZDZaTLQKYCKJTBIACJJKJAQaYBbYTYITRSaDTaQIDKRJBTSbTTYIDaTJIKTTJDbBRAYAaIYSRCAIDQTRTbTJYDKBYTRJRZSRJLCJBTIKSaLBaDAabIDRJJCYBbKCIIIASDbYTSQKbYQBbYaIBRJZKKaRQQYIBLCLTbQYKIKLALAQZICLSaRYSRYCRYSaBRADbBZJQLYTYBIDIaQDLYbYYLDSKJKbSRRQYTQZBABCRYSaaLLaBKQRRbZKTCDJASBJJAZDDZbSBTDLACKYIBYYTYRCSJDBYYICYbYABaJDaYSIDDCQBJbALBTBaDQCLJBbLBRISaDLJZKTKIIbAARaLIBSJSSSDbTLCZDAbCDRIAKQTRJbQDQDJZKJZCQKBADITaCBZbQCYRRBCDbbSZaaIDDSLDaAJaLDIQZLBTIRCJYYIIaAYABKIZSYSSaKDIRIDYTALZJQIaYLQLCBQTTJZCCCIYLbQBJYIYQBDYDbBbRTTYSQJIbSKRBIbTCBBCYJSRYSYITbRTIBTbIADBZQTAICZDRbCbJIKQYRJAJLabaTZRZAKaQDJKQKLYRZCaAZQZDCCKCSKKYZYJBabRBBBQaTBRCKCLRAQYKAJRaLLCbCIKAZbSZBQQRaSLKLLBabCYIILZIbIYIDaJJCbQbCSIRZKACBCBQYSRTTRTAbDDSADCZaTKDZZLZTRAYbAKBYBKCJbaKSbDKKYSRZQRQaRCTTJbSTaJIYTCYKJLDQTYQaBBbBYaDKTYIbAabRALQKIDJIaJaBaZYBSJBRQBbQLSTDQTLYaUbACRBJQBZZYJCZBKQSCQJBRTKSLDJISAZSTIZDLDYaIRYYBAbKKSLCbRQZDRSRaaAAaTJIABAJRBCaYZLRLDLYZSBLSDTLRJaCCaZRIZBARIDBEDaAQIZKZAKaRALRSDBYSCIDbTQIbJbSQZAZbBZQDCIQQJbSLYITKbJAQTaaaaQKIbIaRSJSDYCAJKBBbALIaJCBYQEbBbZDDTCRaKACCJTKYCCTKJBIIIiQZaSZSJCRaLISASTIRbALZDLcCDRDDbYYLASLaSLTDKCTTZICALTLSKaTBLIQCQZabaYIRibJSLQZDBTBKSLSDIQASRbQTYIaISCRLgIKbLILZBUAbJZBYCKBYKJbaQDITKYgQTLQLZQLSJCBSKSbRJDKYZKYCRIYQaIZSLQRTRbAKUBSYZhbCAAJIKCaaaAIYTAJaJSbKLYABBRIKBLSZAYUDTSIISCQaTLbKKDBCJDjTJbDKDJZIIYCZZIZKKBRITZSLaRaDQTJKAcRKaKJQBQDgAbQJCYTCjCQLaKYYDSZTQDIabZLBMiADQKBQTKRSZSKTLQBYJBaTZREJRbaaAbLSCRJSBIUAQSIZbBIETSDZQAaKBBJbTYATKZjAYKIDLaJJaCKDKKZKYQTaLRAMRYRRLJDgRQQRSbCLCZLCKUBaZCBgKLRCZICEYBTDTYACZCTJSZZRCaYLLEBaTbTKaQcCDiQZILYJQjbZBYKQIiICKASJTgQIKRabLDCBYQJLYDTKAKSLAiACDbRUJJJLbSibEUAYKBISCQSAjJjJQCQcIRQSYJCSSTbIDTbIJcZBYSMIBaTBJQLKTBDbZLJaUAIZTgbQEQCYZBLCgjCZDQDAiEbRIZTQZRCDEDRbbICTRhKRKCDLLAYAcZRCiaAJQJYDZBIMRbiKKILAIiBTbbbZgCbSUKhYAAQLgSSLYDTJIAYSCbLLLIYjcaDYRjZBQMEMCZiibTRahKTBIDBSYLhKbJCSTLQBCcBQYJiDDSaDaBjSYajiLgaMYbIMhaJbCDTQKKILTKDTRQIMMbCUBKJSBEIIbaAiBThBTbUQZaBZhYTYAiRRBBSUKKULJBbICjjTDARZCKTLMKIKJAMLARICYAUAQhBkBZSMSSZZBCTKIRDIMjDQSYJaKBAQbEbDQSbIgMMhRZBIIBjZACbjBRDTDLIMgQBLiRJbchUSMICAUaBUDKALgDTjTISAjYSZDTLhBSEQAJQIiAIKDZLZEaZCiDhTDLIDSMMYbYQjbjhIaBKgRZgQIUSAABYCRRbLjThRhRMRSacRZQCkBDZAbZZLBTJABbTQYTcRhajEJQZkRCBCUCbRQZgUgTEgEbaALSKRTbLZEIgcSiSgRkYKQDZaDAEbjcKDKJiDiakDLDREbLgQajAAUISJILRhbIggMBIZiUDDESLQYcIbSTYLTCZYYicSQbBKRAhTbjYjQZSUUSLiDZbCQEMESLjhYEhRBJRaZEQZRKQKLJKjEKULKaSjEjbYiLKEEAciCDMcAZABSgbKEgJKhKJTBbCZjEhKIYMbgiYZggYYbciTQDjaTZYBDgBCQKCZcjUcAhBBaJbAMMMaaCcRJUYkbMUgBCAcTIDkhkMRZQZCKSBRDcIiTATTZiUEShEbbaigJMbIhQLZASiSbQJZDBQBhMQcCibDTEhkJiIKiKTLaMLAkjSLATIAJILichcSkbhajMkTDAjESMbRAYZhTRJBgCcQRiCEJiIaBADbLZaYMkQDTiCRkKRjMiJhYhbIkDECgASbhTSTDAYiZQTCBhIBETcUREIagicbKJDkLLMIBZiZkaLQCAIcKMARKZYSiEECMRbZYTTJLEiMTRkAShiikgjaARCJaBRSDiBBUZbZERYCDLILkhkkZiDjLgQaJTCgATITihiCjibDLQSLhJbIMSDUQLkjJgIaAaRERgaQUISAkUMgUZAEYjLCLbTUBBKbgkUUDbUakJjSDaIIDgSEBckThEUTbYcEbLZAUTgSjgkbhjkCJhEETULRgAUKiJCcbQLiYKMLEaKhgLcIQZaSCgKRCLRgCMRJjMMcQUgjIDRDQLjcEbScSKLMLKBgDUhTBSUZhEkcAQaKCcMYhABJUAcgBIgjAYAKMRZahCjkbLKDjAbRBigTBBaiYIgkQBMKkBbMYBaUUKAUkbTaYJaTaREQgbSghUZjDZahCQkcjkEDiEibTBEahAcYbJYEUQLKMQADRMbUUERhLBibUIJAQCccTaMhBBUAYQCibZbJSYZckAhiSYMIaaRigjjcgaajhYjRKMgbEURSBCYZBQYZLDBLgTZQADLUCLjUQTkKRhQkCjbZREEIJCQbAaYbZYgSEbjSDZMQaZJUaDIAiDAaYJjLBEgkRckjQQBMYKQjJacgIKECKIRZbTJBACTJgJKYKcREJIgLEKCQZKQcTSYCCIKkZiLJgIDaARkjSQTbQSTAZJMYRULaSKSQSTbCAQIZQDjaYLLaZUADkbKLBZDMAAaLcahRbSbLTaURZCSaKbgbZDkQBRZaALZRSCTYJYKkBSZJhRQDAEIiAbJaETgEKaBJKUIbBhagZBKZCYTCKcQkSSjJIILSIbIiiJLDQTbAAicCLTgLBAUAJKJJTJDDZIigBSjKYMiLYQKKRSLRTJJQJLhAbUAIUACYAZRKEbYDkTSAZRMAhIgaTSEDQhRkJCTSRAbYKYbIADBUUIQLZKBALbQDUbIjMRSJSJcJLJEQKAMTEhEQCagLSZIRBCYKDaaBbbDaZaDIUhIDCBCIgQABUCbZBIICDbDZjbLKccbBLKABRQTYSDZabAQaJYZRcKIDUKLhYhATUSZabMZEIQLRhaELIbZYAQBhgLZRJYSLDMCaRSSSLDiCLDSAcAcaDZCUQDTTbDLYZjSBIDCBKTBZbLTSSTIDkJZZSQYDTAbTBaBaaJTbIYBKCSEYJTbTbBgRJQJCLKLIIDATKTCBaSbCUTiYQSZBCMEZZYQRUgJSJDIbIiZMTRSBDLDJBgMZDcLCIhQKDLQSKILUTSaLBTQAYLMjYbTZJJARjLDQRISMSZCaYJLDRUCTLgBRSDLgSZCAETQQQBbBDaBQbLKabLZhbLJQUCTAhTIbAabCKKKhDBCABLBJTYCSKaIZITBibSBabbbLbQaKECZMQSaTIbCIDaIDJCCADaYCUYJCZJSYTjZDSDQLCCIRCZbJIKTZRkYJcRIDRLIaaaCSTTIbZLLIQTJSIDAZKgiQCACIaIRiJgjEABQbaSLRYJiJSIJYCDQhBJRSgIbSSJbbRRLQJQAALSTCYLJLbTaJbRaIRYLBLYcYQCIDDSYQEZaDRQSKRQIIYJSLgCaRTKDULQLAiQTaJbJZTQjhLiMhCAKLDaJBbbASAScAYDSAZcETaKCZKKKYCCbLZLCbJJDRJAQULBECDbTIIbSLaSLgIiaTYRRQTaKhKbIARbZDSbaASBKYhCQCIBChAAYTCbJBJQDRRZTbRCCKAbaaBSJJaYYTLZQRIBAYYJaRIDSRZbRLiZTYbAICCIUbKTURDDKJZQDhQICiTQJkcTYaYaJBLhhASbKJbAQQREabTDIICTQJQYALCCcIJLYLZKBYURcTZAhLCSgLUBQaRTIATTaIRaCbZIaSQRbAiLaBbLaIZIICAAKSLTASCJKJTZZRCQZLJJiRLLIbSMDLIYgbSTiYATUabJbIBACRYILYhIKBaQAaRZTbKKQibRTbAQQYTYRaaLRZSATYBBIbBIQULSDDTLZCjYKZKCDDRQARbSRASIIKBIZIhRBBAZIUYLBRYibbAkgaIBIYBjJITRSIiADKDRZBbIiRDQQRJQBhTacTIbYQKIRBLAADJRRILTaDTDZjUQICRCBBBCSAJDQIbIQRabJQaLBCTIQZJYZJMABJZYSLZJLJQSKBLQbZQLAhAKBASDQQIAQDCbALaTUjbQQIYSLDATADILbBKKSTIJCLIDaJRTUBRaDABKaDbBDLSYCJZTQSIQZBCSIKaJZJKRQLIKRSDBCAIZBRMQJAaLQKTRYCTTYLCZTSTcSRBjYhJaBTTRIBCUAbIaaSCARKKLRCCIaLAZaBTECRZZZATRQRaDQCBJbTjJTRSDYCRMQKKKZJQCSRDBTSRLDbJQDBRaZLaYYTRZSKMTQaZBKIQZYDJJbTDBZTQCBaSJCjAbZbIQDCiSJBSSQBTIRJSaTABTTQJIIAQKZDaALYRYIDYJRRZAJTZILQaDZSYCIbDRZLDaLaaCZQjQYCDIABDRaCaTRLDZJYZIIAJQQZSIQcQJDQBRTRALKCDRQZCQDjKJKbDaYSRJIITLCTLZJQYBRSTDBABaILIZBTSRZLKAYTaZRTTIBZACaLBQSZaSCbTKTCbKSADIAJCYBYYYYJCLIJDQSLRYSbJBTBcILEAARLcAKDYJZBLbAZALZZIAYCQbRQaaRBDDRYJbDSKQCBQaZYDbRLZbTAbSAADKKDCYZKSIgBaSaJaKDSYJaBRAAZaKbSYLIYYDLQCIITQIZDZZQDZTLBKCKLSDQJADRIKTIQDaCAZTRZZJJBQJDKbbBbBTISSDDBQaQDICZZRBCSDKJbRJQTRDaCKQYDAAKJRQbBaaBTRJQCQSLQbKAJQabJYIQZMaSJAKKZZAYCDRaTZBSITBZABKJLAYCZbbbJAKIbQYaTRALQCAYJKRIYJJYDQjCZjbIZSKLLZIYCSCgDSLARDIAIKSZACQLIKLYIKSKSQYRKBZLaMaSbDTLTTJJCZaSbZaDUCKbLQaIQTZQKIbCbTaBLZCSaQBEKTRTbaBDaCJQTBRAbZALJSBDaiaLSAKBKTjKRBKIZBSbQYBDCDahARTabJYACLKaDbJBKIKLQRbQCBDTRLBJJILAJICAIYQaSCACJbMRDISBQSDCDJZSDKBBRTJZRLIZTYZCDLTYCQSICZZJSbSDBaYhDJZIQJBRbCJRCYKJQBCTJBYAIBIQLBKJRDLSBTKSCCCBISSaaZZYLJThZbbZbCQjYTaSTSUBQQaZYaYRaSYaSQLJUCbaaLDaZAKTaCSKbTZRDAaLTYIIRASZJQQbZBLbCKKSCaCBaaZIIALgRLLRaAZQAYDLIaIIYTILDILJZaBTTRAYYSYBIaYBYCIbLKJTCCQJRAJSSaDKDYBbJTYAKCBZSKQCTBQSTabYbMAYZCBZKJDDZZCAJQCLLSaQJKAQbDbIQBKZLKBTCQYBbIDTTTKDYLIILAQaTAjTTIIBKbDDbLTRBDaJYKYZbDDSBKSKbTCaaQaCCLKaYJDRZJDSbYaDKICaRTLAYQBQDJQYJCgRTIBRIBQJScATbLIRhSbYRDiaCaYaRLLDLZCILKCJCTSDDASCSZaRDARJKCCIYIKCZBSKYYSJKLcJbZBBSCbTIBYIYDJATaZAKKRCSZQRAQBLLJDIAIAQbJDQDAAJCLRZYRDABKRIJaaDYKYIRZhJSTIZIJTBLCIJRBCZBKCAaCQCYbKZADYCKRTJBZJZKTQaRJaTKBQJJQZLALIQAaKIKQJSiJLAIbYaSQSZBBRaBYCDQTQZYZLJZCAJITZLKSIbDahZBKRIKKYSCQALDbRZZIDICaYYIBSTZLKJJDSJIQBJYKIRYKbCCSQSSJZRKLRRQITbCBAKCCLJCYbaLYZDCbbJCTQbATYBBBQIaYTbDAKBbAaYSIZSKKDJATYQTaAJYBIKKDZLCDDDaKBSQbDLIRARDKTLaSRIKITLDADDKaaZRSaDZKbRSKAALLZLCBLDTTRBSJQZCYSbSZABaBQLRaSYJIRgaLYKYABRLLQZCLLAKYbICZQSbTBQbbCKSJSCQSRQUbbaZYATTBAYQbSTAQAZbbRZbCKbBSQCYaJLTKZZAbSaQLITRSJYQbCZLCTabTYbKJCaSSIYIAJbQRRbbBTIYQbQZAYKbDRbKLbSIZCYCSDYbDTRCBDQSbZLQYJKTYYCaZSQbbLQCTaIJQYLQIEKYTLRAQUAKbRLBCJIbQTCRBZQJDRTLaTBZCaYLAAaYSJKJKaTKaKZBRTYKYRQTbRJRSaSBaSYbJBDZZQZaLQRSJDALJQZIRaTSaSSbQQJJZaCYDYSIDaIKJDCSSACZbbICTIJQCSSbYLIBDLLKbDZDbRAKLAKQSYSJJKDYKSSCRaLCJZTAIRQDRKZZYRLIYACZbZSRCaJZbaIJISLLBTJTILaTJKaKBTYBKQKZYDBSCIAJDBJSTSSBAJTCJBCYRbJgbBQKJDKIBDSJTbBKSaBDSTCLJDADYTRQaICSIaKZKYTYDTCaBbBITALAJIJYKQQRDKKDYLaLKSJJSAYLKARIRDYLDBCAJKRDJTDRbDDKYJLAQaQTRJCAbTaaQLICACJBSRSYYRQbBDaZBKLZDIIaQIJSLbZLYARaJaBIBSTIbaaCCZQaZYSbAJbKTTYCTKaJbaJYJAZTCJCSaDAbSIZZYRYYYYSTSSbCbDAIITJbBIYZZBDAADDLKCSCJjZbYYAhQCBTCaCabSaRZbYKbRSJSISbJBRDBRYAAaRLILZTSBaDQZJbTBIYCTSAAbIYIaKTTSaQARbDBaJZIAaLJKJBBCYBLCIRCYLCADZQJDTALbBYASALIbSZbaTRRbQIZQSRKTDaJIBSKYTaTJKSbAKDQZKZBBTBZAQRbKYRARLSaBILYAJLCTQJJDJZSaRYLSaIISQCRBIZAKAJRBbBKDBISARQCKJZKZBSCbLLYCYRRLCIJCAAZLDTYLSLBaAZLKRKKQDKILAIYaLAaDDbILSCLLARQLbAbUCDATIJIAaJQYJBLSBQCRLaBEJKbBJZITQCJBIQKTZRYaTRBZQTQCDJZJLSSJYRSKSJSYRDbAbaYDQYQbTTARZaQKCbaCRLZYDAZRCaaTQYJZZATQDaDDabQDDbaAZCAbYJAQKaBKCQKIRZIKIAADZBITSTaRZAIIJDKSbBAbDLCLKJIIaaRaaQJKJaALIBZQKLBIZbCBbBYQAAATRYBKJABJBbQIaBZaBBKAKRYRSSYRRIQLTRRLYbJIaKBLJAbYQKAQaaYLKRTLbIDAQaISYADBYBCLLQTZJRRRbbRCQCBKaaTBQBKYbJbCKaICZbAILBKLaYKQBTCQBZDYCQTSLRJbTRZKKCIBSLDAZSZYTJaACLaIQCaITDRJDbCRIRaLJCSTSDTbBLbCbBDaDBDTCaTBYZTSBbIRabKSSbYRIDbJBASLbaAKYZKRKAYTRLQSCTBKaZAIRYYJaRIBSJDLLaZKTIYQZIRALIQLAbARZZYLIAYQZLTJbASIDbAJDDSJDIQASSbJDIbATTbTCQQaTBAaKIAJZJCDIDCKRYTbZTbRRDAABbZKQaICRDTARSLaQbBTATYTKSJbTLJCYSQZbJQbZAKRBCTKSLQKQICYKZJBTBQYaTSQKRaDaCJTbDaLKAbBIBbaQabIZIaJYCaRLAIDKSJZDKKYaRDTaBSLRAJYCDYSYJTKbaAASIJDISRSILTQRBITbKIJQZQTYLYbSDSLaDQLLIKTATLCbJDSBKaBRIAaQRAYJZDKZRDbCACCBaYDIDIZIYCbIKJJIQLQLZZDBCRSTBIDYLJKYIYJSSTAYCSYIDDTIJAKJYKTJZaBKTJLIQCDYKDTSRQTDSIRYSIJYZJSAIQLKDCLQLKLBTBAaSTCDJaKDbCTJQRQYQLaDbSIBAYBaYJLaKKSBSbLZaSYJSJBADZTZQCSBJRCbLTTBYYDTCZCSIIQZBICYQTaabAYSDBTBAaJabJJLBQYBJKTQKSRASLALSJJaJbCRAILQYbCQSRABAJDIbBLIJQRRCRZDJLJJbCKAYBSSaJDBIaBLbQZJZaRAbJKLQKTTYCAYKCCYQCYSRZIaQbSJQKAIaQTLbYLSZCaRRLQRDISBJTCaIYABZJALZQKAbYJRLYCAYYIQJaQKRQRRRTYabLYAaRbCILbKDDRDTRDaaKJLaLYBJQATTDLaSLbJITDITLQaSRbDIKCRCIRQZTAICJBIAADBBYSbCSQTZTDTSIaAJaLaLaBKLZbLZSKjLYIKAJCAIYbCJCbBDLIQZaAZQBRZCIQJDYTKKYBIRYSZSaSIDaAQaRaBaBLbIAKJQSSbIZYDCbYJQCTZZDICBTbDYaYaJSKaIRSBRAIYYDDAYRDIbTKLLaCTZbDYJTbCKQAJTLCCJZQbRTaCbCaCCJZZJQKCKabSJDCYYaRDRYSAYDILTATLAADaDRJTALITQDIQbZABBAKaJITKIIJbYDDDBZbLLAIIDCRBYRACLIAISRSCDKKBDZTIZLZLJSLLCRCSYDSSDYABDJYADZKBKaZRYTbJLIIIDCbKQBbCZRKJQTIKDSbaIQDBTYJCIbLYDQLaZBTIIbJSRZSaYQYQCAJKDabZDTZLKTDQRaCCKQbTJaISRSQTYTKDISRIaDbIRLIBRKKLJDaQBBSSZTKRCbRSYRTKLSJIaYBRaZBDDBKDAAYBCARaLYKTQCZKJJIbQCRTLDKSALTaADaTCLaQBIbDJaLZJSAJKZDZQJQbRQLSZTISTKKbabJJSCSSBQDRZYQJSSZTRaRRISaRDbCILLaYKbDIJaLBCBACSbKRCYbSRIQaCABJLIILbDZQIbbLaDDQIDDAC"
"BRaCZSTIbCYCLQCRRRCKRLZKDIYAYIaQJbTRZKSaCTJKDQLYSRDIIQTZZYCSKLYRRQKCBCbKbZbISTZCSIBLTAbLRLYLITDYZQAYAQSTRaBZaCbBAbTLACDbTTRaBQLIaKZTALAaJBAJIaSDSLAZKRTZTQIbYDTbKDCACQDaQYTDYaLKZaLJRBYTAJKLaIAAZYBQSbQAKAIYIJaLIILLDCZIbZKJKIBRTaJaKQQSAZaJYYbSARKabAabRTBQSTAJBLCQRCaTALDKCCZJIaYZaaTTQBKDJQBILIBYAKaAZJKKLBDaCZTCATYRJYIbRILRSBZQKJAaCADAJIIZKaZAAaTIQJZYaKCaZTKBCCLDLJRaZBDZSbLQKQCTIAbZRJBaYZCBRJaDDDSQTJbQJSKRIbYLaCabaTaTBJLSBQAbZQTSALRDRaZRQLICKYTaIJRbTCACTIQLBTIZbQCYIJLbbDLIIACDZIBKJZaKDCRYKSISCIQIKJQTYTabiYaBZBIaRQYRYKRSIBTACSJKbLZQZZIQTTbTLBYIDaCSCLDISJALDaYbJTZCITRbCBBabSLRTCKLLRbZSYJSBbISbJbaCRSTIQDCBJKSDbYLJDIRSRIAKBQKbSQALbDRYbaCSKBBZKJTJQTTAQISTYRDbaRQRJBZAYLIYLDJBSAaaDCLAZaQDLbbCbIRAYRJSbaRIKBKaSRAZJKLLYaBIJRBLQYbYDaTICbJSTTCQYDCACJADCRJYLSYBYZYDYLIBBJYZICBKJaIQbaALYBRICKYBYbZJAQRSLJQbQRSJJCBCJZICKZaKDSaAAYbLQRJJJABIJKIQQYBaLQBTBbbYQRZZbYbLDLRQBAIKCCAaLRBSTDCCAYaZDZYSAaTBKDbaAIbDJJYQYIbILSQCIQBTIDaBTDQLZSDLJKZKYIJDbLYaZTBAIBaKZIQCCTSRASIQBRbYZSSbbQZaIAabBKQSRQbRYJCDDJaYaCRLbJaBRDJBLJaASBYTBYJAZYbRLRYaaaLQRCIISBTRJQCIRaCaSLKKLKYDLCQaLDSCARZaSLTKQAJTSbJBYLQCJYbARaAAATZKaZLKDZRCDAZYRbAABQDabAbRKCDYJYYLSbaLTKLDYBZCbaLQDDQTSbbaRAZRCCaLRDIBZYCTIaLLBIDRDJJKISaDTAASTRAYbALIYbKQRTCYTDBSITIJALYaLADYRQZQSDbRTBbAbZaIaKbDbKbKSaJSSRTJQLDATSLZZDCLbKaaRSYQYRYYACZDRYJSLJZBQAZBTbLbbRCCBQbKZDaKTCaLABAYADTAITKDDbKKLKKbIaDJALQaQYbYCSaIAbKRDAIbZDTAZKQaaDTTZRSBBZaBIKSAbSRQJKZIQJLASQSTJRJSKYLRJQQBKIDDRTSLDCLQJTKDDaQRSRDCKTAZBZSLYZDZDaDbBILSRYTJDKKIQaYBYKTSRIIQLJZTTACYCJZYRZSZDILBSLRIAaTQICTLZJTYATJAKIbQbBILIYILYYDJSAQSAJYabKTbQKKBbJSZCLBCCIDZTLDKaYaSKQBZCIISYCIIZDCabQTKKACTCKYYASbRbJAAZSabDKTIKZbLDSDDZZCLRYATDKTYZJbALZBRDDQCYKQYIQTJZRRCYRKRaADQKLKbLCRLDRATQRKaZaTSTYbAKbbAJAZAQSIYKaITYZbAaRbQDTKJQJQJTaIDTCSIJKCRKbaLAKIaIZQDTQCBJTLQZTYSZADYZLZRCZCRJYJJQRCCRAILLQBABDYaYLDDCQTZKCCYbbKTACBYBJDZLZDDYSRABLaAJCLIQTQRTBCJAIKD"},
Embed{87,278,"IYTLSIKJIYBaAICILIKQIJIYIIIIJRbILAIQQJIQIYIQJTDLTTTATTTTDQIRSLQDQTTbbLTTITJTSTTSSARTTbRITZaDTKRYDLRISBaYDTKRTBaITBaADRILRZDITaAIKCSbCBQZAaTRISDLCZZAKLDAbKBBTKILZBKBKSTZQbQQJCDBSIYCBbCIBZQSZDYRaDTLBICLRaTJaaZLKDZSQZKQaALQZSZALaISARSLCSYCRZTRaBKTYbCCJYJAIbLDJITDTZaSARQbATBTLKIaRaBTbSBRYCSLIZLZaJADJCTCDbYZSDJYSDIYIQbDALARLJYAZJSaTJYCLQDYIbRDBJSSKaZSLLBTIYYKQYBLKSaLRDYBYBAARDSCLbJDYSJbAACKbBABbJAJBTBSaBKRZYCCZKAaYaQYLBIRSZTbaLKZDICKAYJYbYbYBJCTYIRALCTaSYDKQbDYBBIZTLQSJSbASLBDRTCRRIaSLSZQYQDAJKIbILRQKRbKYISJYIADRKYBbIYBKBIAQIbaDTITTKZCSDLbSCbAJJaRAIBDBBJTTTSIKZBBQLTYSYIDSDTKYLTIDSJQBRZRISbbTQZbDaIbKDRACYKZaIZRIIJDbbRLaSRQSBBISSIDSbBRYKIJIaAKKLLRKQBAQJaRARKaCDaDRCARCRJbKTBCLDIIZbaYZISLJACDbYLDaCLRAQCDBYJKTZARDRACARJRaTIZIDJZDCBLYYbAKbTQLQKTALLTKILLBZaRQRRKKCRLLTaDASYSRZaBTKYBABCLTKRDAaaRRTYATIATCSLLZKAKKaISDJbCBKRbAJZAKKQLaYIKZYbaBRATZBLTBIZLDQSbSDKKKSAJLQaTCAbRICIJJKaaDSRZRKKbJCbILYCTQTSZaCYBYaSAYTIRYKDRBTDDLbBDZCQJQSbSJIRYBSIJDIABYaTISSYYbbRaITSDRIYDaYTBaCDRLDQLYKabBABTLbaQDCTQZICKJJSQAJBIRLYSZCQDJTJKaZDbTRDLJBAZYKKQJKBBACYCYZCSTbTAbCKBbIAIaYaTaTLDZbDYTLQSYCIYIKDQIRSRRIYJZCBCRRZIIIaASZYIaIYYCDRLAYKZZBRKBDaaIQbZDSKCDIBIDZQRJSaJIZISRbDYCIIDCabARLDASRRTYLCaKaRAbRBDRBBYZADTBBDBBQAZKTbJITSZTbYZCICYACRIIIQbLbYDJBZBYAIATRTLIIIBaCATJCSbZQbaRJLSYSBbAKRQIQBRRbRRJKCYaYIRDAIBTYTTAJDIbCRbDLJYaaRQKQabIIaDaSBILRbYAKAYABaKTTQJIJZYCZAbBbSBBSaRIaJZSZYIDTTBZbCIDKLCDIQKBILKQDBZBDDbSCSDDSRYIbITaaQaJDIRZJabYQJJbSYRbCIKSJQIIYRKCKCIaAaCTRaTDYYSYYDIZSLLaTTaDDBbLDTbQKAYabLRTSZIRYAQAIIISJbSabIYLJYbTCJCBAbTIQDKCSRADDKRLbZZCQIKTYRLJQDaCABaRAKKSKBBSYLQSBDbaBRDBADLYZTJLLLACaZIYIBCbZQbYKQZIQRDEIIaSBZLTCYKDIQKDDZTDBYRIABZYYCCLQTSSKIaJCbBZaRZYYIJTZbBISQLYCSTaSRaDbKIAYaIaRJLABQRJQIBITBbDBTBDBILIDCSRILAbKBCAbCYZYCYYILCDCZKSSTCKbYYMRbTCSJDaDZbJCBRTbKISaKBIIZQETBaIZDYJQQLTTZSDYJCBaTDRKSTaRYbARILJaAIRIKCIQBIaRMSZQJaZLQCaDRIIBZDaAKCBabRaQKcSbabDBIABKKBAYbRTIbLBDIQQZJKCZSRaaQYKBKAaIaRKYaJSBbIDKBLYKRDLRQRIRZZABYAcQKICTIQCcKSDYYaAKbKLIAQLSDZbKYKDLQYQKSaCKTYKTjDZJaabRYMSRZQIAZIRaQLDSZYCaKJQDBIcKDbCbZAQJBbJaTTDDDSQIZJRTBCRQLJbbJcTbTRLQZaRBACJCIYSDSMQRRZCaQDAKTDCYLbBLTAiAJTQRTJZSIbQBZCBbJTIJKQCbaDKSYIZJSLJBBKIAZDQZSJYYQTYSRCCIRCKCBDCBkRLLJAJLbSBKCRbZYBQBJSgAIDCDZSaTRYSZYAZDRbJbhIATcYMJCQSaJKSiZISSDKabKAaAAhZLTMDQZZZhSYYCRJZBCDgSDZABRMZSJaiYAbKLZZUSCTaSICKDbjCbCRJAJRYcShDYKEbTZCbaSSYbJJSaKBBJZKDLhKgYYCRaDDRgCZAigSRDYYULDBZDaYYRYBISAZALTKbRIbMRaTICLSRILQYBabiCLjQDALAAJBDAaTQJcYYDDUYUDSZYAJICbUCRbIbZJTajDaLQAJQLITSQjYcZAYQSbCaBCbbCBIKJMDALYEbIJccABRYAJaYbTSLQaRDSaSEMAZjKcTJgSCbYRQJDKZhkZBCbTKUYZZTcTIjDCRCIKYIAQCIbZDaaQKQKbMJIKJDcQDbARaIBBbBMLYRAKLIibRCCUTLDjaaZhQTjZSiCIMgKAIbiCQACTgLjUbLjAaJiKDTRcEYJIgICDZJbBZBIiKYTJDSCRSgQQJAYTQcgaYTBiRDRYZRhLajLKjZDiZLSijEQbbJQUCSBbASEkYcjKRZhiQMJIYAcKKLRaAAJJMKAcaYQLAZRCTKLIJKZCQUAbIjYRhQMgYMgaKZjMCcLRYbUiCSYDaRDBbcCQCMBKDBZIZBBJTjjiJUBQCiiEcKDiBZbhgEZRKBahCRjSSSCYbEMacLbhCACCYIcTRkMIZTbATKIhTbZCjkaYbISbCSSJiZhQjcCLIBTJJILcKCCCQEjKcChJaZLCDcCAjIJIKDAMjCiDJjRbJIRREAiJaSDUDMDThQQRUjLbZjUgUQAJaSRJLjCKCQASLCZYaUDaMJBkaicETiDRBQLAAcSLhKgLhSgJabQQZTISEjRMhUgbSbQcgSBAiLLZSRiBZRiCRDaLbZCEChJQMMkDcSAiiBCjEBEiUgbhBZSSJkRITAkaQDCagUbQSJaLCJJhTbLIZhkUbKMYKYCUYUkDKILaKRJRkgZBQgYSDTMaEBjaYSYkZQCIcKgBjiMTbcAMbkCAZQQgAYMDAJKhBRAZBEUCQKMIbjYLjjTiLjYgJgiZJAMQQZMDgcKTRIIZQTAEMbBChEjhaTQKgUjYgcbEATbEMEDLTITTTCTLJBUQMYaSKaBkkkMhKCDRQRQcgcZEiIKZCajZQKJQiAhMEiSBkYZDcQYYEZThJTYRSghcRYcCKZRAcJZIhgMIEiTYSjSKCTgCQZUScLMchIUYcUhbLCAMQBJYQhhLBiLMDScaJLCaTIgAaBLAYYSbTcCUEkMjhkQRQbZibjMcSZAijkRUhARRIRhAZScRCkBBkJaJjDMaUSAKAjJEjchZUKTZLMLTABBggbQRcEcZMQMAhhgIZAgSUSLJAEQAahkacTMTjDiJgiRkaKKCMLJQAUJThRaSikTYEUhgUZEQSLbjjSZTjSgkkRZKaMkCBIjaJZcQYAgcBRZRQTLTYLaBZZjLAaagIMhZCEhkYhgAKgYQCMETMcUhgKghQJCMcYkSRSkUBJaDMBcUQBZQZJciRikYMgRiDYJTDDKAQKLbCiJBDhaZBjEkYYJajQQgUUBLLAccBSgUjjKYILUQTRAERgJMjJKijiQbgiKkQKjkRUKbgkChDDZgCDDQLiCEBLAZMRgCIcDTZURZChkhDcTcAYRSRgkCjURDcZRchTKSiAbDTBKkkQBBacjkJhRkYAhkLbSTQbCiLbCiMghZEKAAiCcZSSLMjCULcTZURcgAZiRLBgLMESDAbTJaKUIIBJAcQbbEckSCAabYSiMbQZELLYATRUbahZLECKaAgDTDAASQLUbSIQLIDCBbbIZKRLcAjbKSATbgYSZYjKBSBZkSLBIagbITbQYiMLjZAbAKIJYAMZaUcakLgBQBSCgDYRgbZZiSQbSJCTTZUBRRCcjbiBCYZaBDYCcEbbQBMYCQcQaIBCScYLBEICBJTLRLISMbCLaDSkEBCTRMMjaCDJBYKhISQCCCQRLBhRRRaYCCbaIkkAMaJaTSJCQZLKDJbbJjIRKAaMKhTZAYLLBgZQAaAISTiMZCEhZZTAYCBCJhbUKBJYjYQKQTJZaCRLQSgJJQbKJTcBBKQQcEAJiaZCbBjShLBKkjRJSDSaYUITgYaMbYAJRhRLJECCZZQCbShDDYZRICDQLAQbMSTRSDCRIZabaCSZSMJLAkQZEBBYCLYLIKYDDCKUASBAMQYCJaaBLJRQaRjSAKJAbKZKZLDBZaLBcIjAACQRIILjYLZBQLMKDBSTBBLJaRLbJTSKJRTADDjaAgCTUAZZYTTQYYSLUBUJiALLCZZJAcaRDCCbihYJRLEjUCQDQLRcASTAbRAZYbAbKBjijKCLDKIELLJQbCARjgZgTKaCSCbgAaDZSLZBLRLbaDBCSSQBbJbJACZBCBJgYcAMKiYaJaDTJTQKRcSDigaQAASKZJJDMKZLTaYCLJBKQCajDDEAQQMIQTaDRICYcSRaaKcbJSLTQaILEbcCYRSRaIZKaARAYBaSEKJgAaSbaZLLYEZDhgRJCLSIBIABbSYBKSLITJRZJBbKLYBRSbQbZAEIQBBQCYKZKLRQEDBIijQRaJijKBSaZDRbbBbhCSQgIYcLIBaRRBDYaKBgDKTTKCYYCRAYJKRbLTACZbZaRaYRMRCICABLTZDTTKCgMTKiZYTYDaBQAZLTLDIKKTJKAIaSRIQaTZDaJRTScKACLbQZASLRjLaUCLZBICTAAbSiMaaLBRTILRKSRYLRQLRShTaQEKYDJkMYCbADZBCLDCSJYAKLTAThCYTIYASJaQYKTILZaaLMZbKhQDBAacBIKRhBLRhAALQJJbaJaTQBLYAKARJKAZQQKQRDZCBaDTIKCYBJRBYIDTIRIAbRaYbAYCJQAKbDCEIJKRIAbTSTCQQaAaTDhJbYZbZCRcIZZQLJQMbSTKSLiAZDYASIJJbbZiBYIShRSIBbjQDRZLCJTRJSSaiRZQEKSSDQSSZBQTIIDKbgJZKRCaTCLgJDDSADaRZYCDZDABJBYJDQaBaTcQLBZMBCCAaLYLRBRTAJKAACabRZQYLCDERUaBSDSIYRCTQJLKCagZZALTTbLLLRbKSISADRQKbTIIbZQhTIBBBcQMaSKIKbCiCICLCZbSDQLaZaCbTJCTgTRZJITIYYIKDbLgKSDDKDJJZBQCBILaBEIBDYJIZYIaYMQIKKTRKLYSCMaJRAaKAAYLaJUcCbLQQLbBBYCJZDTIbASYBBYSIQSYYKJbaYRDZJZCRQDARBJYiAaAIDLJITLCITSbTTISLAYbTCCSaBAABBSCRIZSZDRZRaZBDDaYaQcbZKLAYaDIDTDRaBTRCLRgZJbAYBSbIMQDACQSRIbDaaABDBRDLZIaKYSAYbJYBDRbKAbICBKBDJDbBDABKTbbQiigaRTiaILIBRIISZUIKASIDYDLCaRaDJAAZRKTABAKIDIAjLLSBYCabITQATJLJSbZTSZZLRZACISBRICiBBRCAIDaJLRQDDIbKDRYDaKZSCbaZQaLDQSAiQSRYIbDRjLAAAJJQKaLDaRTBLQLBKQIDbCYAbJTLQTBhbCCALRaKAiIKKKbDKIIaTZbbbKJBACSRZLCSTAKZASZRbiMYYBIaADATLDTRYBSIaJZDCYDRaLZSabaQCADKTCZZjATQTLiYICIDaSKbRQRTKCKIDLLARSLKYSCTKAbCDRYTKRLRSCBiJbbDZAKbbCCbKbIICBTAYJDBZALQIYBACRbBQaRKhZLJRDRJaTDJSLaabCYIKBDYTAaQjIYBTSZTKABZKQaLAZQZDYQLBJKgBQLYIQaYRCaISSBQTDARDRKTDJTQDZbCRTZITCbTZJCTICTKSBbJbjZDDYQILTDQbRYYQLTLBCCbCQbCTIbaRAZTcDQZSiKLRaSDbbJTBDBRRKbaaZbKAZUYTKZCQSCSgITQICKDYbQbDaQTBZUaCTSRIDIRJRCKAYJKKSbhbTYTSZSBbZQbTZRJDQADTaIDBRRJIAADBSLIaTTaQCBKICBIJRZTTZBCbALDBbSRLZaLRIABBSCKAbADAAATIZTBbaabCYYbDRBJJCaSaRABZbCDZAbAQZaDIADATQTYTQbDYDYQYQLAQaAJaaabDRQLDSSbJYaIBAIZZJECAZSSaQbQDMbaAbIAQaBYCbCZjZBISQZSIYTIaDZZZJDRSCJADAZJJASRCADZKKSTbBKSJRJaQIZbIQMaYTTZRbKLBYZDQaSZKRSAJQbTLJDAASZbRICBRQTDBIDIJRZQBQLYCDCCRaBbRbJULTQJaBKIKYDRBSDYTbTAEBITSKLKZTaLbbSgQLCaQBabATAJabLSSBbCJCTTbJBATKRRKQQSLJTKaBDCILSbIQQbLMBbTbKTJAIKDCZSRQBYJCTRIBYKYbbCQKEKITIJSQRTCDJaBSaKIIJJbDACbJabKCaASKDZKZAgKSBLDJDQaaTLbYKcDYQAKCQQLaTJDDRLDYQKaSaTZYaCBTTATLSSAISCaKZZJJTIRBYRJRLTaKJQRTTYCRSALZYESCZIaTRSCBMRCYSKCQbbRbIYZLJaQbYDDRTQbLRSJITQLBKLbQSYTKRKSTBSZCZJRbKaYCRTAJZRAZhSEYaRDIgAYTTDbZQRKSYDQILQCAALRQJDLBAYbbLbYRBBQJDIBAARabDZIJAZJJRSCQQLICYRKZBhJKCQTLLZBAJZagSZaDATCDCbaBLLICLCLYCZZKCJSIYQKCIARZCZTLJJARYBRTDQQRbRKKLIQhDaISaTQRKBJBRaSTCRDQSQBIJaTJRILZBBYRBJTASYTKSSBDTDhjLBLbCTKKSDLQIQbDTZbaZQBYJIDSYQYbRDZAJILAbAJRbYQLLTCZCTJRaBYKCKaIAICAJDRYSSJBTZLRIaKLaIbTYBCABaLDIRICAYSSQYZITQabYaYCRaTRaaSZQLBRLCYaCZARLbYSTbDERKQRTBaJJALBZKabTaYRJbbaIBZLZSIbSCDLBSiaSaSZKCLCRaaLCCaYDAYLQRBALIBLBQRKIDBBBYLRLaTBaKRbJaTbRJSYBZBSCZAADRBJJZRQTYcBKQKLAQbZJCZRSARSDCbTZJAYbQBJCTQAZCSbYTQLZKaDBRYZCCabCaCbTIRLZKRIBDDKYISCMSACbZKJAKaaAAIDQkQBKDJIBLQQRKTYTbJRZbbADLSRSYJJbLAQARKTbBABSDaDIRTYKKZZJjIbQALQgTCRbQJCSCaKDjQAJLRTZKbLIARQSRaRAZJSKQSbaTbSALTRTKbYbSYRZLCJJSRZTDQAQDQZbJgQCZIbYKBKJDQBQaDQTBBbRCIjTZCLSbKCDbaBYQaCQBQJTaRLYbLgAaRZABLYJZKTCJLDCbYQKDILTTQTYaLATTRRBQQaJCZBQbJIZBQDLJKSBACDCDaLZDDISISJBIZLDSKDBZKbSZTDTBRCbYIaDABYYCLBABKJCQLDTbLAQBAQaaJQBBJbYLAZRDRDDIIYBITaTKCJAaDJIZTACYDLZIBJAcDQDAATYIbLbaASaLDIQaSCZbDgSQICLbIYJLIBQZLaLQSDTLYRQbKYYYIIQTCTKBBIBaIbLQCSLRBBDZJYBAKCKLIbIJSTCAIKAIZZbRSTAKYKBQCJTLLCJCKBBCbTbJbRTSZYQTBKYAKRSRaZSSLaSaSZYZRSYLLCCCDRDYQYSDZCbKTbDIbRALaYRLaSKTLKIQISabLSaLJDbAZTBbTTYJJSDaQLJDbIIRSAAJKJQQQRIJRTYLLBQaRQabJLbZYITQQTLRBJQYBIQIBbATAaSIYaKCALbAYKAZJbCSQabRTSKaLIBCYRBabBAaDDYTaJJJBDBZKLZRTYDAbSCIATAaSDTTDbYcYRLCbASTbKCbBARLZLaRAaYTLSAKYTbaCQIZYRCYIQDKTZDMLACbSLJZLCRJaAIaRRTRTZKACCDCBILZIAbJDbbaATBQLDDJZTTAZIRSTADBQhAQRTDLLJCJKKbQbISCKDDCYKKIRJIQYbKJZJYQZJLDRSSbJRSRJCQTJKIRCYLCaJYCZbDTCIaQTBbIACbTLSDTDSLQIIaIKATKARSIJKYZQaAaaTbaIQLCRDbDIRbCYDQTTaSJCbRDLQCbSTACLAITBKZJYZJRQDDbBaSKBRSBTIZJTYAaDRbCbBaZAZSBCYBZJCIAQLAAKYAZaaJaZJJIZJATLBCDQAbRKIBDTTCaSAYLYaIaISKBQKaBaYBSCRZQSSKZMCJAJSaaaLbLABbBKCDbbJLBAAZBZKQLDSIKYZLQYIRBTZYBRSCJQbAaBZLARKQiIQJDbSZDDaLSAbaALJjQaDDBIIQISZYAZLBLEaKTSALaQQDZaJDIDYCSKLDCDTCZIYCRaSQZKaTbAATADZbTaDTEJZDAaSTQaIZZZBBaTTISZbILRYCDaaCaSISSBJCJKJLaYYZZZAKCAbJYQTBZKRCDQAQYSLAYJRSBIJZIKKDQTLDRRZICIQDJLKLTICbBSZaQITSRaSISJZYIBRICJDaTTbBAKSSTSYJKJLIJDQbZBDCDbDSIQZaBKDQbRSbCAYTTTACTbIbJbJJBAKKaKAbaTZZbZRAYYIYTaJCDASRSLSKBaSIaIZSJQbAJZLQYLaADKAISIDSZJbbKcDbRSAATbbYDTaLDDJYCTQDSQLJYDKIKDSYBBISYJCCAbQbLKBDBLbKbTCKbBSYRIAIIBITQUSYbQaRCDKBLTDRDBKJZSRaDRLACZCaCJaLLYCTYQZARIZZCbABQDITYCbLaCISSbLJYbZQBbCLIQYTSbRRSDKTAKQLDZJKSISSIZYKJQBRRLaJKIBDDSTDYBILZQKYIRRZaAKBSBBSKJDQLaaZaQTISBCbLTaKYIYICbTBJDYSTSJLITTIaCAQCSBALaIYDZKIJLbCIYaBDSDBSYIaBJBZKCAbRQQSKQJAJDIJJBLZTYQALbTIZSBKLJLSSJILSISYCQQLQYDabTKbJISQKKZTJBDTKaDRBCJTTZSaCaBYIDTZaSbLbSKTSBZJZbZYQJJTaSJIYAQSbJKKQDTYTBCSYaLYQATDTIIbBRZYDDaRSCZDKRILLAQRLIDCKSJaBQLKILCYYCATSBTJTbIISTJQJZQIDQRAbJCSTLSDAYYJLaZBQTBKIaDZaQJDBILbQJSIIIQTIQCSZSCabDCDZAKbZIbIbDaDJBbYSJCIYbDSCBTRSYJALLAaJaZKIRAKbJSYKYTIDDTDTBDQCCSTBCKCZQLLQCQRQSSAJJaZJZYJRRRLbbDDLCIDSBTJLCAKTQJLCSCDbaZTJQTZAIbaBKIZTbIZRIKSQSZCKRYRYQTCbAZCCKbLJKSSZbYDBKZaDLYRBAbYDCaATBKQSICIQBQRJCTSIaLISSSKTIIKbDRLJaDZKDZJBLDBJKaLCZJRSSYSJQaJYTQRaASDKYJbbSABSCJJRAJCJaRKbDIQISaIRYZZLAAJZCIaAJRTQRLIZTQBLaIDbIJRASCKBZbIAYaKZYJTADaBZIQLQYLQKBCLDbDQQRJLKBRZBaQSISCRTSDAIZCLDIbCBBBRbDSRJYLbRaDbQDaBKIRYLCJbZICLYbTIZYSLJRATSDASCbDZQSbBQYaLaIQLDRCCBAZDLaZACYYTBIIDJTYTYYLaQIJbRTBIDDZALSBSBBCQYJaIIZAADJQARTaIIBYZIZCSLaQYTZTBSYbCZBSLLQARISSZTAaTICKaSaZICYJLYYaZBKJYCKDJILITRTCKTJDRSRSSYRJbbSQYRIaZSQKICaASTIbIIbbRYYaJaCTCBJJaTAJKDRYRQLaJZIYaSZCJRQIYKYKTZQCCLJRSKIRSTaBBJJAIaCRYIKYALTJYCRISSAYbLCRKTKQLJTbCS"
"JbbDCCLBaKZBSJRCBBaaCDBLDQDaJKAZSASTILbBTJKSTLQATLKRZLYTBRLYabICDRRbACQIaIJDLaaIQDBaQIZRQRQDABbTTQIADJCCTDILQSbILIJLLKRAYTRADQRbIYKYLbQBJCKSKRTSBTZCZQSAbBbYQRZKBLJAKBbIbKKaBRaRCJaADDZKJTBSDZYAbJTKYICDIBDTCaIABJAYAYZZBbKYADDTCAJTJIYTbIZBBSIIBJJLTIACTYJYRSCALKADADYbCIYZZQIKZIQSILBBbQBAQYJLbbbYaDCJQDIBASIJCAZSbCRLDBSTKBaAYDQTJLRJICJLYIZBIaCKIQbKBRASbAKJKaQKBJaABYCYBYILCSYaSTTDaaTBBITZbbKZICbAIaJARRYLQBZZYCLSTIBCJKJYYQTQBAJKQJaaQLQIbZaDYITLKISTbLAJbLQLSJIAQabZDCaAbCSZZRBZTQYATKCDZCRSZBDATKSDTSTTSYZYZTaJQATILKRYYQCAZKLILIKYRDLQbQSZRAYSSZBZRKYQLRZJATALDDJJCZRKbSRABZILSDRKBKQTJBRIbDSRJbLJCTDJCIaYRSCTabaYCaKADbKbbTaDbaRBRKDDJRYRRDCKZLaTZQCYTbDABAbBAKICSQZIZAKJbARZLBJASabDZQKQIISBaKAbQaAbJBSRTDZASJASJADaaLCZLYZYARDYYDaSRDTZQATIbKbKZCCBTDJJRTDRDSYKJJIASJRBZSSCRbBATJLATYRIKIICLARbZaZQYDLaDCCSBJRBIaRZCCbQRZSbAQRbZIBabIKRJQKIIJIRSDJTISKBAITYbabKQYJabRYKYbDbITIYTZIQCQCTBYJDADAQADQCZJQJJbQCBIRSYaTRILZaYAILTALAZDLBDJbZRIYLAITSLQQYBLSARDRAKDRSDSKJIbIDQRRbYCbLLZZAKbCZQBARaJaQLIZYQRKDZQAABKRBBLBYDbLBTZILKAKTKJIaRTLTBZDQSJKZZLaKBDCIBIQDKYZAaDDADKACLTDCYBRBJCILQCbaJTISDQDCJaTYBZQSBSYBBDaaCQRJLIZLbYaKaBSJIbAZaBKKJTRKDQQLSZSQZJCCAbKTSaKCLBKbKZTCLTJAbIYBTabQKJYCJLKIQQSAbAYZZRSaaTYLKDZKIDTJCRYCJBaRBYYKYQQKILCZCIRBaYKCLLZISCYJRRDAISDTaZKBKDZKbYQIJSYLLKJSATaJARCabTIDQRQaaIBTKbSbKAJJTCTDbIRRYSJDbRSALIBRRZTDQYSRYaQJaQSTBABBZBSLAKSBTCJILKYKLAaBDBYbZQbLLBCQaYRBYLbLZRKSYKTJAJKRBaRDSRKBSARTbDbaJAaQQYaSCBZQRLTBIKYIZRLTRIKAKALDITKbKQLSaCYJDQQLTaLJaaIYCRSBDDJDaASZJQSJTCZJTYaabSKZBRYBIbabDYaCYDQBKSDbZSCJDSaCSRDKZAAQCTJLLYRTRALaIYBSDbJJCZaJZCQaSYRDYAYQYTaLLbSJYDaTCRCJYBZZbAYYKLYQYDaLRAQRLRRBIZCRTZbSQRASBaTDDALARLbLKJIKTKTRZIQbQSKSBbZabAIQLZAZADYZYSATBJZIADTDTZTCKCbSTDKDRLBKKLDKaZLQLYbAADSIRCBDACRaKDJRDbJSCKTDQSTSICQBITBBBaICYbRZYDLJCLATRDIBAQLLQDIAZTLYDQJbQRDYCJADbSIKaSTBJDJbCbQaaRALKZCDSTSYKBbZAIAIIZZSbaRCRTJQTTDbSKbBKJLRJBBIaAITYbSCCZAQATCAYDIAARYYRZZIbSbDCICJBIDJRDaSQaYZSIQYRJJDLLSYAJKKbCSaBaBZZLDTIZYADBaBBTCRYCSALZQCKRYRZaJTTDZCCYCJBYRaaJJYKZaTYCSSSKbCBYLRQRSTAYBIBDZQLRbaSZTBCSaSYTYTQARITbBTBLbBRAJYaQZLIZSRJaZYQJJbbaCTaSJYaZaTAKLRRQCLKIJATYSCQbKIDCSBLJaAJYQJLCTTJLRYBTBaaSQCIBCbKQDQRYJTaLRIJ"},
Embed{88,281,"JYQQLIKKQIJLCKKIYKILAYQLLIIIKQbQYIIJDIIAIYIILZaaSaSaaaJCaaaJaaZCaaaaaYTAaaaKaZaCbSYKaKaaYQBaYLISQLKRSRYbYLZJIILKAaDRZKQBKYBTIaDSCYJTbSIBKDbSSZbJYJCIJYDTLRaIBTICYKATBRaIIKSDYQSLICaTKJZAKYRQDSYZKYRQTIBKBIRbSBLQTTZBQDDbQbLBaBSAICDRKCQYbQCKDLKZYRYaBbTLJRKRLKTRbYIRDKQRYKRLQRICIDTLBIRCbAaRbCRAQRALCbRKZLIDJKQIaaRZKICRaRBQDbLSICISJQYTYCSRaCBJIJaLIKaaBZLRJITDYbDARaRJAJCJQKTYKACLAKBYbATaRJBCTaLYSQZLBIBRKKJDIYTBZQYTJYIYQSLIDSCZaDTRCaIZABIbaJTQDDLKYJCSYKaDCRBCBBKCQYJIZQJAZRISZRCJZILSRTZSIDbLSSZaDCDIabbDSJIaADIBRDIYJAZYaaYLTAbBRQSQTIRJaaKRDRRZQaAQTAILKDASLbIAZZIaKSTYIbKTZYBKbRKSYIQADSDIaQBZaTQbZIDaLACCAJZQBQbKICKaLDTDZLaRJDQRIKTbJLKaSKBYDbbQZaKDTSJQABRbQRCDSYbTYSYRKBDQBLYKaDDSTYSTbZKYKYARaQLBAKRJTLQJIARaZADJIDRBaALIISJATaKTTRCKbAbZJTabCKbBYQJQIRYTaRRDKBIKaKYRYIYbaATDLQZbQDKAJQSALYISYJBQLbDLRABICQYCSaTCDJCQBJLTDDaJLICTYLADLRYIQRTRSLIYZRKbIZYTIAZICLITZTJAZBCCIKIZQbYIbBAZJTSKCCATTAZaaBSRRBaYTICKJbYbDaKSaDLJKKBSRbYYDBAYTKABbARTIIDBKbSLaaQLZBRRAKYDRJLQZRDTISLaDSIACAAZYaZTabYAYRSbSSJTAKSDBBbRQBJKbTbRSDKATDDZLBKKaQbIITKYRDKTLRbRYDIQRbIIaLSIYQaIKYQJLCRTTCILAZYBCKCaJABDJaYIQSTDbKRYbIKbQDYYQQZbIBDALQRKKYLLYILIIYCbQJRDLDJSCJRKKTCbRTRAQLBIQJZZQaBQZYSQaYbAaZYKDIQCbQQZbDCYZZKYRCAIaACDBQJbQJLCJZYIIYLZZRDRCLRASYBLRSQRBIAYKBQLBDALRZCQbDYBaaLJKTJRDYaCJJSTJaSLRTRCjZBAAKbCRJZDDKCSZQIQQaZYAKYRRYQTSYTIQBaKbBBJLTRSKIIIBZJBAaAATZLTbCYKILaLBQLaZJCaCQCSATBQTLJJBAKJRIDKAZTJCJQKKIYTaLZbAaJIIKZaZQDQKYQBJQTBLYIRbALDDKAQSCJAaYTCRYbJYDZRBQZbBIKQYDSaaJZQCIRZZTaBIBaDTYCaaQDRKaRIJCIDTJBDaDCCbTSbRiZRYILbYYADBTYaZDDYDSKTILIKYJRITQALTBTaZKRYJiTLDCCDaBaRDbbTYAQJYRSYCAJZQIaLAACZSBJDKBADbYaYTITRSaLTCZTZBDSRLScZYYKYCIbARBKJIZKAbCYYQLTALaRIDRILTTIQIJSaSKSSISRRADRRBaaCLTRaCLbBAYSJQISLZbKIAALIIDBDICaLaTDJQTBDASLSaabQZbBJTDIaCSBATQDZBILZILLZQKAaAAIBDZYDKLAIDILACSbZKKQBJSTZYabBCACKbLaYIIYDbYAKYJITZaCAJCCQLZARJRBZQDaCKCaaLYALTLBbRKSZICbbaCTLIRZJLKbBCJBQBbAQSCDAJLaAJLQSBKTAbRIbCLIZbYSQbKJATSLZbAQASJZIZLAUAkBaJISKIbAQDAAZaKQYBbbJBDIAALYSRbISQQbbJIBAKDJAabaIaSTLIBATQZAbaaASRCRLBbLAKLDjbAKLKBLSKLBAADSRSAbaRKQJSYJQJZTZKbRSCCjASBLAaDLRDJCiBBcRbRJIJSTBBYSbKJLCLSKaBYDZQRbSSLTKYagAKLZZDKMQBTQAQLBJBaQAMZRILLDKJKYSQDaTAARRbKRDbBRDbLAQKQBaQBQULJDaQDTDTJLCLSTTSRCBbMYiMSaYJCLZJTIZShLcTCBjgACYKEbLJKZTDAJCRbQYACJhMRYRDZaTKIRARRLCBaLJSAaBJjiYBgDbAYBKRQKIDYjYIBiaIEBBbSQAbbKbJRCAIKJZYTQSLaBQKBLSBASIZRYSCcLSQiYTJYDACZZEAJRYDhRJaAKRZCDJLSYDBcaUIZcbCSRLQJDZTgZKRZaQKBCSCaYgQDDiSJhaUQRaLSaSaSJLJJQITTQCLbJLcSCLaESYBQZTKYQaCYJKYZDBTQbUAiDRECTSSZRaALDZbBbTLaMkQTYbCYjZTRQLJYRRZKBbTTCIISQKIbLTgJCKaJZIAITATLDUQbKThJaLYbKCTKRSMISLbQYgIKZLJQiICUScKiSMTTJKIaYYTSJBMYMCRhZEDMIATRBLCSTiYKRTgaRYLbaZBhDgQJSIKRJJjZDQkLMjLLRKgRLQTYDCSAQJCAgKCjaTLaJBScDLMEYZKTSJTDjLiaIKjBbCYAQYYCaYQahSChQQCMYZLEaKbERQgASIaLhJQgSKRaISIRBiKkRBDJDYKJUbhYajCbRQACZUIgTggRDagBRDKLBjKaDSKiKaSUcTaJaLIDkLYQREZMJLTBAYgbZJITaALRkBYAKAATMSZiaZQYaISQhKSLZCKBaQYcaMRcAJTbbIEIBCSZSEMJbjKRTZZjCgIgACRZagIJCjbBgRCIiYcRQEbbDKMDTibLLKEKLCIgEBiDIDRKJUaDLbKEDEQbgDcIQZiaQchcaQbBDYYJECihLJASDTSJSBSKQTAQIDDMDRIjMCKMDhhZhgYMATDaBbjhUhaKMJAbLSQTChchScSAMIYJaSDQabMKkBiLUTbCgggLhSCLZjcRBCZYDjZURRihRJKYARgTUZTShbYhgBcTUAhMAYJCICEDSZIbBbShUDBQcBDgLCJLEDgKYJiIkkcMYEITRbTYCQSBLaJgSUADERbUcDIEjTEKajEAKicRJKREIDhJQRYkBCBZJJiKgcSBbQJRYYTQAiUiRcUiMBUibgbCjIUYaJbaILQhaRITRkRbKaJRCBYgAZQhiDIETQBghUZhjDbChgcZEjQEIQiAIRBZJbQgbLAYKEJKLaUhhDBCgkjkbIZBLILkSQSYgZZaSaJRhKaQIcUcYkgEEEULZaLcCBDLgJKQajYBhiCEhISMhQUKSBbMMZBAYjCBJJCTZgTQiJgjQiQZRMITkbEQcMJRhZbSJbZDSgTUZRIYDcJQEQiQAjEMUYhIcgEjkYTTTKYQjBiIagQjQEJIMUaMUkiTLMkbbUbbiLQIMSEjiCCYERhIJgKZKTcbkIgkTYYYMKkUSRMjSIJaABUDikZhgQADEjbCaLcajBIcAgaiLQhSQTMZAaRTEDIcJUgcAMcgkhCbkIRBbQELbJZASBSbkiZSEjRRkTkBMAkIiDKgLEUSajQAIahbMZCQIEYCDabLBgkBkDajJJYjRacURiBhkChiEbTBjAgKJDTjKkaiIBiJhbjUTKKJaMaAEgEZKKjYQMhcDjaTAbQjEDYIRZULYMakSMCahRhBIZEgakhRiiKkYhJATYCRCLZbZaicEikjRASZacEkTKgjMECDCCgEMAZJBUbIJQBaSKIhLgbikBLAbKThZjckiEZSIUBcZCTkJgkiCMJkbUAbYQTkZhjQiCYcMcRZQRhcDQBQQjcaEkMkiRMghRQYTcCCbkhJEQLKcZhjLQRiBSjibEUYJikaSKJILhcCaSZEMBSCAAgDLbhhJKhckhRCAYYAZjcbkUMZDcTaSREbQhCbKIiSJIQakMJRQIDbaiJaBRCZaTKRiBUkAgAIJLjSjbiibECUTYcLSCCjScJMSCAbDQIESIbiSbjKjcMbLgBbMjiTJKABcJYQAKMKgRMbJSCQIiMkBakYaTbRRTAUaJJUjcBBbLbiLaKKJTSQDRjKASCKgBIjJQYCiMDZELKSEJagDRbJaKaBIMAbTRJBTRKBTYUgYMJkcMKJSIDSZCKUCbBTaScSjQAbKbCYTJRMALCCQZEbSJKaYREUcSiJhLZQZgTZAaCkLiaLBRaLLBSCZJQIcZBgBASQCBMMgaLAhMCkDDRgASDjhDbaTLURRZTLSCKYICMaLEiYbKSTKDLSJBQSTUDMiALDCJgKDDAMAKbKbEIIIYgKSJaLLTSAKcKDLQRbRgJjiRQLiRJDLRKQTaBIRUbTLaLJMgDQREbRBZiCBIKaDaZIjcCQTKaRZUbAIBBDKCKQbibKELabBQhbcAiLchibbgQAgbcKcIQJbZLaBAKjgYIYaUjLKZaCRgBaTKCgTKDQRTDRDMJQIYCJYKCBJSiUSSEKJLYQbLZIZRLAZbQAQAbLhLBaADECCKMScbaCiYARSSSRRKDYBbaJKZLMUaAAbSIaITccRaTDTbbajABTBIQBKEhCRaIYKDJhUEJcZCRYCRKBBRRLgJAAIDQaQJTYiSCTDCKIBRJKBKAbRLbDCRZYaTbBLZSYbQEQZBTbQAJDhAQKTaKJbSLQaYTJYRTJBRCbbaUKESTSDQgRLKRJjCLSLJKiaTTUKABYCCJAIUDbAISLaCBKgcDbbYSaRLQbRBQRJQbTCTTRaghEkDSDYYTIJCYSBSIbDAKTLiZcIKKBJbYZKaQkEIAJBKZZIBjQBTRBMQTKCaILYYTDCCTbYRQZICDYIbAKZIJLAIQSZTKISgARbIAAKBKaLRATDhUAhDCaLSILbKAQZaMBDDBLJTSZJJIZKKSJQRQKaCJQgYYRQZIZAKZTIYIEIZDUILSDSBYLSLaJARTBTJARLYBAZCASSKASBRJKbJABAZKSJIbAjSDMLIaRbUYZakSDRJjCJBSbQAaSTKSZITQKaYaSKhTcISbbCJTBbcZBDCUBYCYRYIDQDYQRDSCJQYRJbLBIBILKCSRZiZcLTIKSDTJAaCJZSDJCAIICJCTbTYCCDZQIQCRcjjZMJhSaQBYDCZLYbKSKYLRJLAihDbYACJUaQKBbARJAjbJYAIbKJKADIaLBBaMKaACJSbjJYbSIZBKIARSACJKRZQgDQDILBDaBDZRYYYTSKJTEQTYZLIKjDYZCCYCZJSISQQIDRACZYIMKYYRYBBMYJALSDQCLBDCQKLhQLABhSLAIZbRaJAQDSaZbhKQLaiDKMDBCLAEYQTKIRCYTZCUjQSZDYKZbSLabZAjZRSRYZIbIAELYZACKAJLJLIDSiTAABKiYZaDRCKQZJDBaLbbQAQCIYbabBbSKLCgARYRgIJCTSRKCaQaRDKLSIDRYRZKLSBLZZZTKbZQabLDAbTEJTYBRIYLJbBcCSCbZCQQSJRDJQaIaCSLRJQAJTDZBCBIIbRKBZaRTCDMAZTIBRSRCDTbZDBCISYKcZAAALbYDBCYJJaBCSYAaRKTBTbbZbYIZKKaKQbTSBLZaaKDRbRbDLbKKTDCSYBTaKLDaBLACRQLbQQSEITJYJiJCCBbYZZDAKKUACERaDaZTbSIibLTTBYZCQKBSSCZYJLbMbAQQYaJTKZbRDILSCiaAgiBYJhjJJSZQIJSbBDYKYTTSTJCICQLQTASbBBbYCIKLYEJZZbRgSIQLKYCDTIYTIKBZDBgIBaaAIAZYDTCaLCZZbLSZQBDhLLBQSAZaibAAKRiTKjQKaILSDCDCIBLKDhaZJhRALSaSJJIBDSZZYLQDYALALEILDKTZTBaAKTZLUJYCAaKSKTDLKCDZgQCgISLaKJTLITaLKaCCCbTTLDKJSLLabTbDDRCLSZZBRCCARYQYTIIZQLYRQaIBhKRLZTSQTKYRQJDSCSLLCCYTKZTZRJBTZKkBaCYLKALQQSSRTYjDZbQAUTCBALQAJTJALZQKaJZYCiZLDBAJIRJKYCLCKJKUYSCbSDBbABbDDZARYaaDSCLaJBhBZTKYQLKTCbaAYSIJIaCKDRBibbbIcTBZCZBKRDLJIYZASSaZTJJQAQDbCYDKADBKYQTTIRKgBRDDMRKMZDBCKJTbYIaDTTDSKaLAckaKZDIAbJRTaZARSIaZAbbKDQATBKbDITYaSbbQKBBTRZDKJJRSKDZCKSTYICJJTYcCTZIRRDQSQSJAKhRSYRSKLQQRQQYMKYSLDBITISQJQaZTLDBDQDJLTYDBALTDQKACZARBTDbgSTYZjSJSJYJRBbAAYLYZTQCaIIQCQbDIJYBYQhRBKILLSTCQaCRBDLLbYASSZbaIYIILYJKCKDBTDALTBIJQJRYJLDKKZaLRCQDbDASCYKDYZJJJSaAZbIgYabIRZSTDSJLTaDASIJCSCJBIaYZBATTUbDJLCQYTDKLbQLJbbaRJKBaTZADbAASbLTSSASTCALJLUJSBjCSJAJQALaSRAIYCQYSRCLZRbbCLTLLKQQLhYKLKDLKBLaKQIZSSQZabYBBZbKZSRcZBRJDEDKKDTDKLQQTTZQIjBSALIZIRbbBaDSZYJZYbRCaQTJIQUKTKLBJQbCbSQYCCKQQCCCYTYJZbUDaJQaRAJAALABBBDaTKJYDYaJRZDKbKAYBZYBAbIRRDbZQCKKIKDCCRIBCSLIQBQQQJTKKRKYbIJbaRYQCSYJBZRDaAADaCKCIiKbZZJZYbBaAKDDKLCDIhJSaJKRKZZDTYDZTZDAQCSCBYYYTTZaJaQSYYKaAZYYSTCYaZbSEUSTaSCKbJYaJLCYARLCLBIbIbTJgBaLRTSgICSDKARLQDaADCLZSACbSZaMIITADALIARZTRaSJBLDLJBRZLIYLQbCIKSRQYTQTRDLADCKRRLTIIbIDKaJSSbSZCaJIIDbKTYZAbIRDAQRQQYDDaBbRYDbQRDLIYBZKLLAAYBQCIDSIbbSSSKYbLAIBJSbZBTLaIgDYAKRIChLRSIIBaQYJSLZbCbQIQASDIaKQDIZCDKSRYQYALaaBRJKbaZDTKJQRCQKQSBIKIIJICZYYLRTABBKYYCLTbTbSZTDbBYbLAaIBQYBRAKABSZJLBDJRBZQYALYKJSZKDaaaLIBICYTZICDBZZLTbLTICZRjaBQRARZKaQJTSDTAZQDabACbYDIYQCSZKCASSQBLaCTJKaaZTTaCbKBQLTKJKQIYaIbIaKJZbYKaADZbLKLSQTYSabQZAKQAZbSaQJTBCDYYJiJIAZZaaQCCYBCRRTRRbQRLARSCbLTTYKTbCZSCQYIAZRKBBRLQDDbJaaRZYJbaaJKDAbaDYLCYRJACQTCSIARDABLTbIYTDCTaKbDBBBJBQJaBYRBAYQJaCBRaRCYRQRbYaYKZYYSbBRKSTCTQZBaQTSSTRKLZKTBKZBQDDKKSbYaaYZIIjBIbYZTIAAJAKCTKLbBaQQaLJITaSIJTRDJRLhQZZCIADITaQCZIYYYSAQbAJDABJLSRDJRJDACZLKLSJRCABTbRDJCBSYDJJYbTKLCQLZAKCADJBYKBaRQYATIKaLYCADLLSZgKTBZSYSbTSIKTRYLAILRJBBDIZBLCLQBRTJICYaQTaSDCRLAZbQYCMZaAQDCBBLYbCaDbQSBbKDaISLJBZaBYaSRTRSLZKTbbJQKYKRIbZQDaLDQKQIgSKCQARBKCYQYZIJbZKDJLLJCDRJAYBTZCTQLaSRDZTKRKCRYIARIYYJADLaAKQKCbQKaDRDAJAIKLJISDLaAbDTRTBKbIZIDaISDJaSaKBDRSRaBALaYBhQBKCDRQZDBSRSRIYQDbLRTYZSJTIJIJbDASAZQBKYIDbKCAaTCYACLZDZBSJISKKYDJJTQLSaJaIJRJIAZATZADBCBTIBbDYIICDDKLTYAJTZTCLbCbLKZQaAabSJJKBTABRYRRAIAKITILRULZQKYAabbSTKSRaaBBYQbYSADJLKQQTJIaQZYLYDTAIADZDRCIZYRYKLYZZSCZAKJDaKYCATKCaRIYSTJbYBLaLCYBCQbZRLaLbBJACILCLZAALKDYSKZQRYIIDBCaQQIIBaZZaKKCRTDBYARLJATSbYZAbLRYTSaTKDIISQYRCQBSLJJZJRJKLCZQTTIMSKBACTIIIYCRIaJZLBTYJLAaYLITJLKSYAZDBYBAaKQaZTQSSRCYYKRbZDSATIAQLRSIIBLYYJYEaCbJITAIDIbSJRJTAQDDZZbaYRRSZDLDSDDbRBbYRLSZbSQZLISCCCAKTSQDIQJCKbRIATbDQRQAaZLISYDASISSLZJbKLRIDRSTbCbCRbZKYRYDYJCTCAbaCIZQRBKISZRDQKZDYCZIaRJITJbSSJJZaTLSaQQRJSQQRbTICbaBDYLKBKbSLDbLbCaCQSSLJQKZZIYTKBQABRaBABCBLQRRRIISLbBTYYJbbAIJLSBTLITADSZJaSTQYQDYKDabKIaQCRbLJKQYbTSYbJTZabCQSLAIIBQZZCRJLRCYSKARQJDASTZKbKYLaJITKAQbYJICLaDDJKKTbYSRZSAZCIICIICZJBAJJRRBRTCLSaCRBRDLAbSbaCRAYYBQYDZTCTQILIRSLbbTQSRLBJbbAJQLCZKZRSJQIKbKLKYJICRKUSRZDQZbQLABRQaRIJRILDSSYBYLaIISQaJQaLSLJTIALRARKaKIKABKQTbYQSLDJRDKZKbaILISSbZTZBaSRCLbBJCCQLbSYZYJZDCDYATZLCDQILaCTaaKKZYQLJLBYKSLJZIJJSBRKLDJSIJKADQRYCADCKBDaKDQSKDDBAZbRSQTJAbTZATRQbKJSRQDKKISZZZJRTQSRYRSbJLSKCbIKCJIYaCCKATZYSQKTQBaCJBYSbbbTaSALBbbIBKKCCbAJKZSSZRBababbbKLSBTSaKJbAISYKCLTAbBSAKbBRDBabTbIAYJKCIJQIRaKAbTKbQRCCTZDABbZaIRKJQICYYTaDKTSDJBDRTACbJAaKCKYbZJSBZTRTQZKKQJTLaQDBaKQaaLDYJTJDbDBCQZYCaYBIQRLTbBYBKRaCTBIbYBCKRQaCKIbRYBAQDTBCDZRILLaDYQARZSLARJLQLZQKCZSALSZLYaIDCYaDTZJBDAIJZKbTDTbBYLaACASRRTRYQKTKSabLJaLARTZSIDaKCRTSAAZbACSTSJZIISRZQLJALaAbSSKDaQSCLZCRKbaIYaASJQZCZBDKTILZIALBTCZQbQRCaTIRDSKaARLSZJbRbDDAAaaTYYLSYbbITZQCAQIQTJDICZCZASZYSLLDTIDAJRbIJbACQRDLYITSSIDbJITKbKBLIAaTAbAZCZbKYCIZADYZaSSIIISTQYLBZLTTSaTZJDDSQKKbZTJTBYKDaAYLDTJTKCQAaRZBBZRaBbAQAaASIAYJZBJBAZABBYLASCBYTSCRSLCbbALTRKLYQTZJDDZILTRJYRCAJKQCKIZJZJRTCITLCICDSQaTCRQbaDYBJbDaRBAJTJRQaDAbCSJaZAZSaQDYJDKZaLDSLRJSKAKIYAaLJJAQYTRTRDZBaDDADTIYDJSRLQTIACbALRAZKYTZAATZaAQTAAKRJATTZKbKLJBSbBAbBAAZAbRaJAQAZRDJTYBIKIZABDRICCbDTIAJAZAIaDaYbTSZLYYRYTIYQYYSQYLLYbQBCTJADaZRBaICZAJJaBRYSRRIKJaLADBQaALYTBIIIBbIaICbbbIIaKTSBCJIaaJYaCAKBZaKBCZCQIbBYQYKCIRZJYJDQRSaDIRQBDLRKLZCYDKBQBJCbSYLISbRLYQYQKDTJLIKaKCBbQQKDJICRKTbJSaDCaRLBDRIALBJBSaYKBIDKRCJSbSACDbTRTAZIQLJbbAAYaJZQABaJSKYBRLLBLDABCSZLIQbAYSYYBDJISQBIaTRJSaLZIaaIIRYIAZYTCAKbbISIYBSSbQRJKLKRIaZLSDJIbaRbABaJQACYLCYRAbKAISKARB"
"JSaDYLJabZLYAbYbQCDSDCRZDCCZILBTADLLJRTaSBDTATYZBARZaTBCKRZJKaZSAaCCYRKRRbCLTRCLQZDRLSCDSaBaTaSaKYKBJaAJBCTKJBbJCIRTTZYTIRIaQZaKRBTYZZAbZIDZCAbBbTADDRZIQCIDQIJIAIRSLbSZYDIIDSZIIYLaAAICJDRYIBTTSLASKDTSaIRJLYBaJRDSTDIKZTQAAbZDAKCRbJaRIIDYISKIZIDYbAAaDQbBaYbaJRLKRBaISJRTbJaRCaRaBIQJQaBKAJZKCSTSQbSDALSTDRLKaQAZZBDJTILKBbSKJAaQAJbARJSATSAJTKTYAKSKTSALITJBYJZTZaRZZJAbYKIDRQQZAIKBSQYaLCaaaaaaCTBSILJZKZZTCQQJYDQTDRaJSCbQJCIIILDZAAYLYBZBDbDbDbKaQTCBRQIaZCZQbTISaSSLBZabAQCLabYAaTCLZKDaBRIJLCCAbAZQaCbbQaLJJKJTLJQSSDDKDACZKJRASQJYQCZCDbbCbCaBBJDDDADJbTaRJRQaBTCBDIIIbYLQQCRJTLDDJIKaDQaQRQIbKbQKASbIITaTIAaBQabZKaIRbBICAJTRQaAbQSaJKJZIDJJDTKaDSTZBQQJQSaLLIYTSaBLRCSISRIATBQRYLDZLIaCYQTJCRSYYJZZILbCZAYDYBLICZICaaJAKBACCZKLLIQADYIbBAbLabDTRaRIJSIRCJYCYYRQTKbBADTKLJKTZRRJSBBIIIJDIbaSQIYAJQZLAYRabQQLbIIQLbZRKbJCIZZCKTaCTYDAQaDQIRQbRaAQAbYBTbLJLTaDCJAbJCLTCaISIaaAADCbAbABYDbSYbbCYaAQbbSQZCRZZaRAQCSJRaaaSDZKBQQbIBbaZRIKaZYDBCKAaJQBBZTQTKALZYTIAaKRTSARBKTJIYaTIKKZYaZACICALLbACKCSCTCBZAZbIRRTSTRJQZaJCZDIDYTJYaDTKTCACSQbLZSLKBKIaSKYRRLCSbQDBaaDZJSLaKLDDSTZLYQQBCZDSDZDSIIZBbRDRZYIQSKITIBZITKIZCDDISCAbKSSbQLJJIBTRQLYCaaaBYKRYITLBBbISIaaTDJTJYLRJSZALRLJTCSaISTAbIYBYJCBbCLSBYJJSTBSaATQSKZZLIDbIabCDSITAISQQCDaYaSKKbITJaaICRKZKJQQbJSBAYZBLQbAJYTbbAZJSaSDJJIDQCIDKaSSJTAbAYATIQYTBIDQDbYZRbDTLKaZQIDJDZQZBCCaYCLQRLIASQDKBASZRLBQYRBRZLaKQDRBDKQSQKICaZTLZDDRYRbRRYDZYLaTRYYYZBQKaTCDLLKIKaQSTCJRLRAbKDYJTBRKDCSAZbBSbTCSbCAQaQbSYSDKLTTSLRYZRYZZCZLbRbIQYbaCTKSALDLRaDYZTKIADCbSATRTAKLQRTDZLbKTTRCKYLKYYQYQbAbLRLDLBaTDSaTYBTCDQJBYaZCLCITbDARDQZKBTRQRKDRBZZRRKYJRIIAKbYbLACTCRAZTCBCAbQZaCBZLbLZRRZADTKbSQZTJYbBCSaDCBRDKbATJRCQRZICTbAIKYQISIaQYaIaKZBBKSTbbTJbJQSLSZYBIIaKabQRTYJAbBYQCDQAZKCTTILDJRTCSCRSTJaZKZabSTIbCZbZLIARYDKITbRRTTZKSaISDDDLJbZSYQCTRYLZCRaDALTKaBSQaCLabJQCJLDRaRbQKTDDbaAZCDDaLAAZTQYKDYBLDTCAKRQBATRQDRDZaATbZJCQZLYaSLKQQDRBCBbYASRJJYIKTJACJTBaYIICYTQCBZKCYTJBRCZJDCILBTYSJbJLYBQSBDKRAJCCIRADaTLbCQRZTSZRaaRRBKCYbZTQaDbLTSCLLCLSSDLDSTLBKCDaQYBZCaCILLITRCSIDQLJKYKbZKZSTQbabBTRIRAJRYCSLCIDaYZDTZYQCQLDZTRQISZRQKKaDRQTLBASRQTaDZSTALZCaRZaYIAJaaQTSKDKLRLbTLJYSQYLbYJZYTBSCRAYKIDYKDDAQKKRKLDSaDCaDLbIbADSLKQRTQADALALLZBCSJITRDBRAJITSRZJBIJQBKCKBTRRKbCRLCbYIJZRaCJQBYTbRDYCYLYYaBYDAAKKITKARRIIAYQJJKBZCKYJTRBKCBQaaYDKSbKIBIRDYABbQBALTCbLSATbBZAISQDZSBbTSDBSQLKDRJZbDZLSACQBAABQbJZBTbaaYZLADLTKLLACTQBaCIRJKTbQYZSLLLSCAIDARDKJDLJSBLbKbITILACIZKZQJZADZQKbBKQKIbSASCKIDBISKR"},
Embed{89,284,"JQIKIIIILSKLYISIIQJYYIQIIIIDIIILYAYIKIIIKKIYQYTTLRDDTQTLIRQbRRTbITTTTQSTQZTTTTSTTLTTRTIBSQLBaYDCaaAILKRYDbBRKIRBTIQaBbAKKKZDYITaBKTBaRbCJaRICDIQTLLCJSQBAZAbISBLQaTRKCDLSBJBZbALSZKYDRbYBTYTBTADKJYaJTJICBBKDYIRTZSbKIQSaQIQaJBLQZTSBAYbQKCbKQDLBYZaITaKALDZCLQTIJCDRZKaZTKKIYKDTaSJBRYKJBTaSaRTDYBZRaABBIADKaTKYAJBIaJDTJKYCZbRTbIYCBIARZBbCaTTaRRZCTBCIAZbaCSKDTIIKDTIJZaAYTQJTaTbTISSBSLDYCQaRDJTBAKIJDIRbRTSDKbKJDaQIIARbZSCLJaLQQIAZSLBbaLaBQLaQaZTASBRRaabLaDBYQQAJSaDaCTLbTTIJSJCQTZIKaIRBRQZLKaRQAJKCZQJCbDIAJZQIDSYTQJAYCbTQCJJDDIJDKTYBRbDAaaRDZAIDRKZJICSBYBYBICCbSYLbKZQDJZYaRJRZRaCSCCBRJSDSLTIaSRDbTLSLYYDKJAZaQSBDabKIZaRDRQaYaBaCaQBCJCZRbLATLQZDCZQKYLAYaaZLTAaBLDTaIIYCBDBZKYILSBAJSLIYBISDIYYbYDQIZBYDIKLJYQJaaCRKBaSKDLJYTDQTIZSZDTTLCAZAJTRRRaJCIbKQBZYSDQTYALRCTDBbYDAJIACYARLBYTQAALaQKJDAJbTJBZYJYRRCYYKYKARYYKLaSJaITITKIJACJDKBbIZbTIQZYLSCTYKZCTCBCLQTZIYJCaRaJRDCYTKaaATAYKTbJRLAAKDQZTSLTRYJaKQYDKbaDCIKaQaSBLBRLQZaKSLSYYRDIbJJYbKABCJQCDDJCARLIbQBBSaJASLbZCDbZKIIKbDDCSaKTRQLJKTBRCKTRKSBRLJBCBSZQbRZZbAATbAaIDCKYISbKBYaRaQLZBCTLCSJCLZLKDTSDLbDAADDBAYRKaILaYDTSLDabYIBYAKaLTLCYDBYSLALJSBRYZARLKQRQaTCASBbRYTBKLbARZSBJRbDCACCLADZKIKYZYJYTYSIZKKLJSbRYCKQTBDbBKaAaDALYYYRAQBCSQLIKJSDLBDLJATTAaZIZDDTLJSKRYDQBRDTLJRICTZYaaCRKSYTJTQKDRBLCDKSBBKRKBLYJQbIYbRaaZABJRIKQBJJaTAZLRQDJZCCIACKbICSaCLISaLDDBBDaYQYJADKJKLTQLZCIBQaZBQKCRYbLDBBIZBTSJBRQDYYaKKRRTYYaCYIBLAIBRISJSLIbZQBZSLRSZLCTCYKSIbJQSYAYYLDbJDRRaRSaQYJJQSIAbIDYYLbKRJCIYACJaASKKDKCRSJKQaLBbYQIBZQDCDaKTLbaZaADIRZQUTLCIKRISZYLKIZDQRbCDDQZZZTQCKYTbTQaBLIJaDCJSICSKYBLRbAIAKRICbbDQRaBJAbSRSDBRSRLYLYJKCIYBDCQaZRJLKISLQZIaYIJTRLaDZIKQSaJBaRDbDQRQBTKITaSCBDYJLaTaKBCKYJbBIYYLZTSIDRCCZKQAZARYQIZKTIaTQLTDTJYBQTBBCJACDBCLZSJRCLIQRYTSZQZTTbBLDMKYSTKRKBAaDQJBRDQZaACKJZYaLLICKaDJYQCQSJQbZLRJYSAKCLYICYDQLDCYYTYRRADLRLSDaJLRIQBBDJYJTTTQLQCaYLKKJLBSAADLAQCJYYbZKJZYQDRZQTYALQDaYDTSKDZBCQRLKKCSCAaDZQIJLLbBYSDbZCDDDJSJBZLCASDRYQQYYRJSLSZIaKDYKRTCCRbUBJKYLCTYTRBaZaCILTRJJQJTBRbTZDTBbDSZQJKDQTSZJDCRCQBLAJJJBEAaLQKibKAYKSJITZASTIZDDTQLCTZSJRALajRDIKSIDCLLAbBSQRLZSKLRLBQKaQDJYbLLYKQAKSBBJSZIbYZKIIIBJDCRSTZBJEJQKKBCIaYCDBKACaQLZKDZBRCJARAhQZhRIZQTaJZbbbSTQDbRCQCJSBCJAKZaCKZLSJiDSZLDCbRIYQMQAbLhaBYIDISLZKLJKYKTCYRTSbRbAbAKcYCSgSZAYZZTJiBaQgJDKQJAZSbDYLZTABYTIQQJLSLLaBTKKaEAYKjjYgAASLTbIRZIAACBZARTTaLbTBBKQDKaZKIAkRLKIZCSaaRSBSMJDbTJBDKCaSBaIYCJbhBERJQRaQbYZKQAJQADZbJZDYSYhDCaSRYKaCYBJLLbSQDZQKRKTYSDKDIgCQSTDBJMLDICYLbADSIJgDCMCYQIIDSZLKBDRRcbaDIbBgJbBRQKJDUYECABLSLKEBBaSKJRRRZAgZACcaZYRKQbAbITMZijaSTLSAQbDKCUDRRKRZABIAcbZTBhaQJBAAQiababBKYhYCbgDCDTYIKQCLhTQSYRKBEKJDRLDgLYUYSLBTTAAJCbBZMJUUTiBRYDEMbZZIRgTAQARcAiSTELJYbAaDJTKUYLTDDKSRAZcIALbQAZAcDJiRJUJKJiYiQAgLKLSDTCAhDJYQjJUUIIRhAbIBcjBSiQSATBagaaLbKbTETZECLIULABCDZiYIQjhLTRZJaShbUACIDbKJjBBbbILBRabDiCgDECcCLTAAYREAUREZEKbEEIDBTRjbJQLEDIAKCiYRRRAabUZEQYLLBIiTjbARRSYDZQccQQRSiYQbQRLMUKaICLaYTBjULUKMEijbZaYiUTKgUaIUKTaaBaRbBZZDASkZIQicbCJMQKkBSKSLIELSbDSTSJMTQabBDBLBEhSDAKcJRYLKSgKMDZjDUAAjZUKDLRLSAJTZShbRQgaCgLEIKaDAUjMAahaJCTEIADgJEcDLSAbUTAbDbKRQkaKgDKaZZBbQLIIELUYQKYEIBMUEjDLEAYgRQaaDZcZTYQSRahLkKhBaiYYjKTRhKAUckDYcSYBCICRAJRcJLTSYMKRIiabTEMjJKaRMTTDLiQiIRKDQAEkUBURDCBTcKbSRYaEYZgKcKjbDAYICEIJiKMYjgUZBUiLBCcEgZJbMQiTbbJBZQSCaADgJERMDKiYUhDACgZkiBkhLBaCAjAhTUZDJDSLIKgBbYjLKcCkcDUgQEgUScjBkAaEZKYbALDKhQAchYSjEjZSBMDYTQASSAJJMZTbbkicLUTYEQhRaahaBSaBaCRQhUELCbJLAUCZZQhQkBKIjMMMBBIZMkhIgELQRMKBIAUBZSKRCjjEhhKCUbZiUKEZIcikkABTIaDgJBQCMbAjDRBYcSgMYZSbCShQKJCJScCYYSKIMMjUKkbZhLDJkcMSaDhgTRSRQZDcQDUiCUIMMkRJgiILKkTjYbScAZQiSacDQYKZjJCDcMQLMBAgEMLhBYMaTCURkTbRcAZcSikAKZaCYDRRDJbjQijSYAMbgTLCZjjBBggMkbiaUIkUMAJCERQBKQiYBIcJSYEQUKaaZbJBkMSTCZLkRbhjIJcAgkkSMZYQKQCIJQhLcBAkaMUIEchZhKcKjRUABiAQbBBaSjUESaJJAQAcUKiSiCcASBgRRbhSCUUhjgbRQhTIMJUEKYZccASKTgZkTECCbUjMLKTiSJkBRRQcLJcSjQIiJkchEaZEjCZSaLKSRAAJZERJQhADJLkkYkgkUbhESgiRZCCZDKcabjTSZThRRjhAUYTSDcIIRkiIhcYEjkCASUYbBIMYbIZScSkKhDKLLYAKZcAjRSCbgghYUkJDLSihCZDKibkkbZYEIkiYQKLIAJaTjQTSkSjRhaijgQIcDJYTEjBbJcacEhiSQcATQIDTDYgQZEKBRCSTEbBRBLjJjgCAkhibkScYgRbcgDJcahhTMEAjjYKAUKRYkkCEIUcaiQZhKQZgAKjabEkJQhiZZALaabKhKQMJSZIAaBBRAabQQUDYkTQbcDhIQhSUUIUciYZLZUgaRhZbZjJcJaTDkhaSCbCCAAiISUaBCLAkEbhITSSAIQLBDDJIBbZaYhaJIgTEAZcEEBCcBIiRkbEJJZEcISBYgJkDCLYBMIijSETTATZTYLAaYkiaSbaLYQLDDSbQIERSJYTcigUCJTCbTjhMRQJjQKALDZLbkLICIYLQCThTDLYSaLbSbUIBTZBCCiaAjLBREBCkASgLIBiLScIkZIbkSQkDREKLRiTjjADKAKJChAKkcLgZLDbCZSAbcRcJQIJKRhbDACADASCAZLkTJKgBRbLkRLbZbCKhYaTbATBTMBIhKciSAKYbZBCBEQYiKaDbQcJQRYYETMaIURCAYLERQSJTDRjEbUECIgKMSDbhLQJJCCAZLaQRCSKgJTLYZDhBLhiRBjBISBbLLjibhQQDCEIbKCREaTDAYbTCbaAZcMAJZJYZDDCSCbiJKQBYQCCLhKQRLbJSTZTRLYLaLUaaIRTLiaSEDgERYaDRJJYSLBLLRIEDbTjYQaQhjaBJZJRYQKhSASCKaLTAZSKaTJbaSSLCCTbJBaITQKBSYMahcKBULTTTTQaAALUZJCQJiYISKRTiIDZRJcCDSKDZUJcbYKYQZBTCbAQZZLUSAZYMQjTYiCTQbZJaAbYATIiALLiaRJTUCbJYTaZQSYiCJQTZTKZLJTTSYBkBTBQKAIiRDRbbTCIDRRTLICUKSkDIUbjZaLKTCKKaTBTaAJJICQBYYTbhJjUbCRQZTBJcZJSagYQLYKaLBLYJLUbKSJSaJiYJKDYTQIgKJBSUJDTKSiTDCSTDATIAjbbYBDRRRATSLBRTTALCabCbYBZYbCCBQbYYSDRKJaQKKCCMLjTAYhAcCDARKLYSBLAbbJDQKkTRQKiZYKbZaBDDALQSCDQaAbaMYZhABBJDiIaIaaRTBTJCcYJaLCKSRgTACASTIIYCAZbRAYLRMQbECQAZIaTALTZaLDSKKgZbYiLDQaKZAbJBbTCMjJaSDJSCQKbILRASRhDYiTSYKJDLRZKDgYARRLZCIRaaKDbADJQYIAajRgabaTUgbaJAhTSAYTSIIATLSLDKKiKBbADSZZMbYKJhMTDYYSREKATDJbQYBDSAgARaQIaBaaJbRTBSDIabIQAaKJAEYIKAKDIKCJaQDLZgDLAEIATYShSUkJZKRSLaLcYQBRbQZLQJJBZZLASQjaJaRSYCaJRILhIYTRQZaLJBaASbKiISbRKJaCIRBRcDCSBZJTCAjYQQIRbABaCZCURDCDaKBQbCCJYhIYIjAQILIZEhRYLiQSSDZZRYaYAYLIYSYbLRRIbQUTZQRYZCLDCZKBLSCKaaQRBgLaDbLjaDIbBRTTSLJaZTJBSQSYSBYgIRJSKQQCYYBkBDTTCZaTBbbLLIbRLJabTIBCbDMSISSTKAUSKKiCQDYCTAYSbRabDRDRJbKICbLAbKQKKDbKZRQBQCaRTaADYSLZbbCbYRZLCYTRQZRSSTbZJaALTTAIZbCSAQKITBaARSMCCaDYLSiKjQBSYIKKZCBTAJJaURJIBIARaLQQaALLIDTbTaZbcSIBIRSbZACTLQabaTLaSQkKAQIBIYKRLITIRbSATYYDbLRTZCRYLIRDaKLBKSLAJIQRBaZZTBTKKCISCSDcDDDKDhQSRDbRRIbABAQYKQESbLJYYJbIZiBZJYIEaITQbQSKCIRYKTaaYQJDQaRZBJBKYQDIhZTLCUbaDJQCQTTKLTDBRDKRbbbaYBRZZjBaaRSIRQKSASLDBTZYbaCYKAJSSSQJIZaLTRDaIDYBLDTZAaCRSBIDaBDKLYZRSIQISSJBjQASDJKYbJQiaCZaZLBBQLaTBCBZCQRTLQALURbCBQYDBDAQCQLBJSbZCLRYaRKZKZIJISKUQRAZTKBLLAQAJYRIITJRRaBLCJQJYKbYIDSJDJKCDCKDJLTIZIbZQBIbSQgTQIYSTBCbcbCQCDRaJCZILQBLCAabAcIaJSCQKDCLLbCLRBIbDSgKSRIBKABKJZILLQISYLJRSDDbAZhTADZaZAYCRSCJRAaQQIRRbDCLbTDDCIjBTYcDRQaRZSRaYECQREYJDZKIILBRgRLKTDYSQaTLTTIDbSJKATCaLaSUjZJZYIDRYTCBDLTCaKLDRYACTbARIBABKZYIaaRJaQRBLDKRLLSIBMAYMbARAIZKSQIaCCMIZaZYDRTZLBABLBRERBYJSJaZCTJTCQBKaLSAITRQZSUJSLaLJDBZYRYZRDSCTQabRYQKBCILCaZJAKbTQCKSAaLIbCBYBBTjQKTZSJADbBCZTZJKBMRSaBZDKQAIaQIbBSYIJQDIABLITARYbJaZYYAbJCQYIRCZYIbDLTIRRaJJSiZJRCTCYLKTBcaQSDRiIYCZCLLDIDQJJIDBYAZAJaACSjJLZKJJRKZJRYBCaaCBLEQRCCSjbJSZRaDCDIRYLCKBATIQLaYLLSSKYKCSJQBJJAIIQBJKQTJZTZBTLDYaZAKIDZCKJJSJbTJIIKIChBbSLRBQCQbBCTbYDDAaRYSISRQZQbQDQABbRDQaZRbRZKCDaCJZTYAJAITAIQZJQabhSSbIBTLKLYADDJSYIYURDJChJQKCbaAJRZICbZbQRBTKSCTTALJARCQiQJKaLbZAbRJCILJQLbgaCQKCKKLCZRZJQQSIBaRLbRTLRSKSJSSYTbBQYDJBJCRKLCabaIbYBDCZCZbbRASILaaLCaZQQBbIaLbabADBARYJKZISRDLjaISJQbTBJSJBaZABZQYLDaBcSCITKaAYYSLSaTBQZiBZYTJIQRSaYSJDKZSICAbhLTABSCAJYKaIQKDMBSaTQZZILRLRTIIAZJACLaaQIQKDJZLBQLSYLbKQQATLZZBCZKQIBKYCQaZaCRJRDRaZbZIbaSAJRDICaZbQDRISaTTIDQIZYRDDDCDAIKTYDBaLTSSIATCbQKDICSJabZLLLIiaDSAbQJCbaTZQBJDbLLIZJTLKKRKSSIRKBTSBAaACQDaJCIUbaYSTLZRZSCZIKSRCJZaJKYYQQYDbSJAMLZLQaJaZSbaIKCIIKQZCQCBYISTbKBJSKJDRaSbTBbISLaEbJIRbTaSICQbYZDDBYYILIZQJCCJRACSJKISTAKIKaKDILIQaSQaBLSBRRBJRKLLJZKRSJLJbYQQaIASRYaDCTAhBBSZZQJcYaABRbJLYIYAQJBDaDJYLDIQDKTQbhIKZAaIYSbCAJLKTQTJBSTIKBbRbKCaZBJLSARRLLZaTQAQCBBJSbBBQLbaRSZACIJYbTYATTAbLQbDYBZYKBYQbbSRbLKACKQAaSKIDZRADCDKLTaaBZQJRZJbDSJTKQRBAaCaabYYJabZTAbZARhSQSADSJASbBKYQJKCJKBCZZTISbAaKKTBTKZAIKZhBBKDRSTSBACAaJSYRaLKTKRbBZIRTSRDaSQIbCbaDTaIaJJbQaISTICDZIJSaJbRLYKhbabJTATTZKbQAKSDIDCaYQbSYKJZTBBSSBKbaRCIALTRZCRTIQQTLRLbTaLLJRRSDATKZUCRDaDIADRKYaQTIZKBabCILRYIDaILLaQiCZILSAaaBJSCBYIYRBRbMKDBbDAZaILJSSaYDCCLLDRZBaQJbbSALSaaILYRDCDYZRJLSCYKKaJDAKJAYBRBTaZAAZBLZaaTbSQJbLSKJCLSCAaLJZKBbTDKLZCISDBYYZSCbQSYYIQaYAQZAITiTJAbBDbRAICTKLSRKTYbLDIILZQKSQCCASRADZIAIKCJJJBJAZRTZQKBZbKYSKJTIAbbCDSTRYLSDLCDJIKZQJQRCIaKYbQAKBLADBJDZQbaTIZbCCJbBaYCAJRATBSBDZQIKJSJbSQQAYbICTLJBISLLbEBaQAYbYYAAbKbYADDbSQCIKbCIDZaAIJKJSQKBJLAIaJBLIQIRLIYaIRTITbCSbRCZRAbTJBTCCTKLBTBbRZKDBLbRAYDDAQIKZYLBQRYaABDTJRCQJhJQaDCZRaZYZJRSITJDjbJTYJTZaAZCTJaKIIKYCCBCDATQSSYZSTLBSbbYRCbUKRbQDDZBLLIAaBITZRDLAYKZSCSRLADQBZZQRRTaQTISbZQJRQBabYLRLbTQTYAabYDSISBSLBKDQALDRLAbZTJJRYDSaQAYQATRRIBRSJbbLAQQKLbTDbARJJYALQIQbKJgKJASALSLKADYQBJaaRZRaJTIRIDbTKITRZYYYbCRQbSJaJZTQBQKEJTbaSCRjRYJIQIJSJLSaRILbSCSYRABQJTZTACYITDBbIIbAADQIYTKYJQbLLRIBbDDKbLCbabYRJBLaLSTJbBSQBSYAYRCZYAQBCCLDDZAADaJKDLQDJBIYJLIJYKJYMIZAABLISTKabCSCBCJTSYJQRLBAAQRIDbYaLZIIKYQDQSTJIKTSQYTJKbLDaBLCAbJTJJAJKTZbIbYTABZBABAKAZYDALDYLAaJaSTBTbTKCDSIICIbQJTDBQBZRAbIKZRLYAAQKBSLZLISDSaBLbSADDZYCSaJRIbJYZJZTTSbIaQCACCKJaaDAILZbTATYLTKSLBKQTDASYYLbBQRDIbaZCSIJLLTACIKIaJTYTAQTLCaSSCZRJSSYbASRaYDaBZQBDKQRJAIZBLYTbZQLIKKbICAAZCRbYbSRBLSbYSKBKQRSSKQQKDDTZAbRJIYQLKIRBZYDRJKaKRCJTTARLBAaDACCLRbLDIbRJJDLJZBJAQKTQabCAARJRBaDQBCYbCTDLZDaLRSaDBDYJSBaIICaRDCRYCAKBLIJbbJQKDQaTDKAaBSJabCSKDZbbYBIYaSDLbRZTYBLAAQbIaYKaZABbBKKICBRBTKYRBJLaYbTLbLQQAYBZCbYaAQCKZaBCAAYSQYBRLKbTaKSRYDJBDQZSIBZJJKDTIBBRDJSaYZZAQSSDISITQJJSYRYTAJKCYJTTbLTTACKLATCbBaQJbAaZaIDDYCaQQZLKIQLYQTLCBARABaTLCJYKAJRDRSALZLJSIaaLQICRabDKKKDDCBLDSRbaaSQbAIQTRYBLLBJSbaKAJbSBAKILDBZRCJbASIAZBKSYZRTRCBTIDIYTKaSbaITZZYaDbCRLCCbDBRaLTJIIBSBQLbYJLDabABSDYIDaRCQBIbJBSKCDaKYKZZbQKDSBYBLIYbKCLBDTIQBDaDDJRBSJCRAZADQSKTBaIYRLIbKDZTTaaZQJaBAZSCZZQQICRBBRJCQbIDZCYKJIBCZABQKCLCSDZZBRITTABDDbIKQIBZJJKDbSBAaZKZCTLDBCDAIZJKaYbRCTCaCQJYCbYDIKZbTLDDYSBQaQQITRAQgZbCZRISJITKLCaAaKSZBZbaTZaJSICBYRDYAYBCRAYKYYDJIIJTICaCZACbDTLTDDTaAYZRYKTbLDDIJQYYASBTZQRLIbBZAATLJBQKKLSIBRSIJSKBAICLKYIDZKJSIBKLYbIDaLZTRbYALSAKTRKYRLaaSCjQaCKZIbJSRQCDaBJCJBSYaSIBSTATCaSYITCBTgQJRTLJJICLaZQZTBIaDaTYZZTAKTaSDDRbCRDKLbKZTSTSRRaKAAZKLRLZCQaAIDBBCLJRaaTDJIZSDQBJTSQAKRbJDQIJADYRCQJSKSaRLBaAARZaQBYCQYTILLQYBJAZLZYLKJAARTATTITQJbCTZCIRRRBQADKLaDCSKQDAaTBaRaITQBJZbDBYDLKaJTBJaJSSACRIQbIIYJbKTbbLADSZaQTbCZQYKJBTRaKTKCZRZCaCZDDQRLQbJaYYBQJBASLJQDQTbCIbJYTKSDAZRRIARDYASYRbSCCAJRQTDaBBZKbBJICJDTSIQIZLZTbYJbSRQAZIJQKCCDLbZaAYKKJLbJZKQLYDAaSIDSJCDSJTKKQTYKRZCDTBSYJTYDSLQTLaKbAaZSLCBDQITCaAYaRZbYAALZZSJQSRIBYbSICRMaJILKJTSZYCDJTCRRTJZ"
"ATbCDQCZATAJTJRRDAQDLYCCaDIIAJDQDYDKTKJBLJJbAYATYYICKITDLSTYAQZKAIJZZCRJTQbTKRaITSKJRYDIQbbKDCDLJaSBQYJQJZKbDabRRLKKRDTLTSZSIBISITBSIRaALQaaAQTTIaKTRJSLRABYTJLALIRSaQAIIDDRKSJBBDITTZIZQRJABSDZLRZIKJBYABADaaZaALCIaLICAKCKTCRbZIIACZSZLRLYRbSZJSTDCKJAbRKTZCaaZKQbZAKAaIDCJCAKCRbZbRKAJALCTTDRSTBCTQIJQKQYYSCJTYZaYBJbaSJJJSbaTIYKbZLbRTTCQCJBSKaBLKZSbADLLaIABYCDDCACYCRLLJTQBKKaTSAaTBYDTYLYKBCBbKLBaISLDbLSTKDTKbRYKRaaaRbRabZAKRYCbCQDJaJTTCBIJABQRbbLDTSSYSCZCRSLQTZJLAQbKbQZBYJSLQaCDZQBRDbQCZQbYJBDIaRYQLASYYLbTaYBQbbLRDDbbCBBJaLBQQZKKJRIATZSQQTDaSCSbKSRJSaQJKDIbRLQaDbQLAZKIaZCSADRSBYRZTLZQSRSTaZYCIYRKbbIYQQRZLbaICbSLYIZZCBZJQAIASaKCICJDAICCBbQASAaALKQLJCYCTBTLAYZaYYKJTSKLKJbYbCZQKaBaIKKBJACJQaKRTBaRQJabTSRKbKTaALQDaTZDaZTTDSASJIYILRACAZDSLbLZZZTaCADQRAbbALBSCIRBabCCCDDBKJLBKLLJIBCRAbLIZSQATZJJCALDZCQKAIATQaSSABZTDZYRYTZTYDIATRRBTbCSZJDaRCIbQaDILSYALKBaTRRTJYLTDaBYCaYIbJAAAATAQRJJbRILaBIKYAZJYaKJYRQARBYSSAJQDZSSCZaZTATITSCIBIBTbCbBQbbJKKZZILCZJbAZZTTKJRBTQRCaLDLRZARTCCCbJIJBIDZBIIBKYaIQLSQJCIJSKCYASRRJBBSAZZLKJZATabbKIbJaCAADCTKSJCLSLaRaCIRTDBKZQbDDYRbZTYKRLKAQDZQQTQLISKZKDYZRCZDRILaSQJDSQSLCTQCRLZLJTDJTZQbBKIZbIRQQCZZTaaRTKASLbYYZaCbSIIBJISBbabKRSRQDCYCKDSTTRQBZTJbSSZLaAQTabCJbIaBZDQQSLBJTRZSJSCabSCZLZYBCSIbYQJADRZJQICTSZQQLTYZZKIZLSJIbCaIbIQDALJCABACbSTSKAJZAJDKYRaKRKKQTJRaSbBQZLZBQLDJBaBTASBYAIBZTJKaBJRDBADYIBLCKSaaTabZZaYaRDTJaBTCQKCTYZRJSKZQBQCIATTJTDbKSBATJbCRCCKZbLbSYIKQBLbACaLALbZbRCBJJZQBJaBBJRJQRSQZSZJRLIbRDSQCBSCRAJTDBbaaICBTAJDSTDbAIDAIQaBKTLTQbBTZAaLLRLKKZABQbLTbaLZKSRQBCZDAYZRYaLSbKQCJAZAaITaCLSLBYQDabLZBJSCYBDRCQAKZLbYYRRIJIQTSJLZSaYIAaCaZQCSBBTBDYaRKTLSAALDbRZRIAJYaKJaDQSaZTCTYQCbJaJaLDLIDDADZaCDRYQbRQJQQARRDRaJJIKaZaBTTKITaaZRISbZbBZKBIAaTBSSDLITbDKTKZARSBAAaLYIKRTaAIIDRQZAbTaBCbbbCaBKSALDDDBJAQTaCSRJCbaBRDQQJKTJaQLaAbCQQSTQJTDYIJbCbJYZaDQRDZSTDKCQIBRBRZIRBTLBLLCabJCKCbQCZKbIACAKBAAKLTBDZISJJDIIbbDALTICKbaKBRLCDCbBBCbDbABJBZQKIRbQRTKKCBSLIaCLLaSJAaCaCIBaKIQSRTbLSbRBZKSCDDAKZLbBKTDDYDIbKbAYbLZDaIbQbSZYRBCaaKCQIQIDIBLATZYSaZJQSIQALbBDCbIIIZCRQSBKLJYDALAZSbDKLaJSSKIDTRZDSZDIKYLDYbALJATBSAYKSBZDRBRRYQYTZABKIKDYRRARBbQCCLbTDALIJbLRSaKTKDCDIaSRRTBASQSQabRRCTRQDCaZLTZaZKYJQJYIALDLDBZCDAQLTIQJbBSIIDCAAKAZARIKbDZBYabbLACCCSJQCRRBIYIYTZJRYDZBQCLDICBZJTKTQYDRCLDLQDDDLbYIAQYLSAQJBJLaCJBCKbLBISRIYBKTCZLYbQICCbRRZCDSKZLbKSIZDLaRIQQabRZJCBTYAZTJQCYIRQbYKIDIAAKZJJLBKLBCbTTCbDABLQLaYJIQYQabKDBIYbQKKRZBSBaJBBTaYLLAKJKSJaCBSabSABSAZaADJTBBZRKIbBBBKITDRCITABaIIATZYZQZBJDCRAbSaIJBbCDYaKJKJZLBYZRSbTAbJARSYYTDQDSTTQLYaKIQBSCKYLLSLTAZBRDCDACLKLCIDbYaaRYKIICJIJICDIKIbQZbYAYQJQRKQSITLQSJLTTAYQYaBQLAJIYCLaJRYYQRIQQCbbaKbYKRKKSDJYBQCJQLaJJKZQZBTDYCQLRJLRLYSLYSTSaIDAbBCCTKaJBJYQKJSbAKZAaCBQBRCIDIIAYZJaZKZRIBaZYaBYQ"},
Embed{90,288,"JJbSQIAIAILILBICIIIQbIJYIIIIAISYIJAIQIJIQILJIKbZSaaSKbKaSaYCCaaKaKJSaaBDZCbSbZaIBbKZYJCSSYISBaYAKRaBJTJADRDTaBITBSYaLTZKTBKYBTIYRCIbBTKaTACKDZbQKIBKYRDIZITITaBCTSJZLBQDLaQISTRBTaILSYLRCRKYKbDYDaBBTLQKRJYSIRabRIALTTaJKYRCZRTAQYBLTJKITJRYCRaYRCZILbSDITAKTTRALYZCSCJYRKACTIKBaKRTTDYaDRICYRQKDZDYCDZQZBKSIIZJRDLYDaaSAZKYbRASITLKYSDYLaSJIYLQJBSbSJSIYIQLDRCZKKQTZZKTaBTJJaBYbDRSRSLAYIKRRLQAALQCLbTTKCCTZbDKDJYSRIKJIDZRAAZRIaAaTaTTIKSRBLKQBbBbDQRaYTKLLaDAITKSKRRDYRRQLTBITKYISLSbDQZKSaRTKaSJbAZIBTKDTIZYAIYYaDCRKDISBbSRIQJIAaQRKDLTZRCDKKBLYSJDaLTbLYYZRIZTBDYCKTTYTBYKKQCYIJBCbIJYaTJaRaaKYCQQIYTTaDTAJRTBCKDSYYbDDZYRSJJIZRaCJLICAaaaZYLICAKKDATLATDKCBZCbYCRCbCZaTQBbQbJTIDSaKZTBIKZJARYKaAQBTYJBITSZQDKDSZQYbbbBaIabIbIYaCaZTIAIQDCKDITISTRCbaIaSRaBJCATLZbYQDSLBCZAIbYYZaSBLYZKYYIaaDYLDBKKYbIZQKSZDaSQLTRTDLYCAADLYRQYRbQIQaYaRAYLCRBYLYAJaKabAKZLCRYSRJIYTCKCJJDCTIKYBILQJBDKaYAJATTLaaRYJRCKBLJTLRKDBDCQAALBBAYIRTJLTSDabKSLYRbKJIDQYZYCSYQIIBTLZBKIabKJKbDASCCKZbBZKbBSQJbYIKZIbKRZaCDaDBIATQYIAIZYIDJSAARSSKZKDbYaBYAZBCRBZISQDZCbZRIYaSKCAIDRSLJDYDJRLQIIAbSRBQBCTaAbLSZYaYZQbLaDLZbKCQQCKDCLDDYRTJLbRaTaDCYCCLJSKSYaTYYAKCITLYbBRLTTARYbAaJJKBBLZTaATKBBCTSYYYTbACBJLKCCABaLQDZJISBTTJSDLSTJJIRaZQKSYRCJaYSZKBbQTCIQbDLaTIIbBRYLRISDbSDKCaYIADICbBYRTabSCBaZJJDbTSIJbAIQCZDaRKBAbQYZAIbYRARbYTaBaAQbBDJJQbbbLAKQBSCZTZaYAQTRaJQZTJCAKYQLBYQYaJKabYQYbQbLADBLDAJIRZRBRLKKQYBQDSSJCCKLRLSDKIACbLZCZAIICIQAZDLTZJRRSIQJSaTKbIYQYKYLBJRRKKSLLTLCQKSRYCLbYDDZRbSCaSDBAaTJbYDBCQJYKKRaISRYbDDQJJYSAJbRDRDARJIDSKDRaARADKZBTZIYKaYYBACITbBIRTAJAZZJabbADADKITBLLABCbCJBSTYRTIBTSLRCSZAYYYLSDLDTbAbQZJAJTACBYKSaZbABRTcbQLKCLQLYYDSTTLaZZLQbZSAQJaZDRKBDAAbAITAIYIABBCiDKhLbTCZZQaIKaBSTAaIYBKTaLRAIDZLLZCSYCQQaaDKAAaaLLQTQJYSDJKCDbZASBBRBSJTLTLYaCBIAIZRaLSZRbbYDDRZABIYLQQQbYCQbbDAQZLaDJKLARKAYbKiTSJCLbYYTLLKYZDTQBTDaQaBJZYACLDQBJZEATbSRRBKSRIDbIRYIJBIJKLILRAAAQSbRTaDaAbAJIQQCTRQbaZZMbQbJRTKCaICTZCSbTQILLDbDDYDhBISaJRYCRRRIRBBaSLICQZaZcKZABaABDJZYQLbSRSDBSRSRTQYbaAbbSTIDCBLDJLCTaKZTRYBDDSKRKaSRBZRbJYRRQARKZISJLYDQQTCMBQbTabIKZTYbTJADAJDTCAaLAKKbCSKYKaKTTCISIYhhBbZZZCZbRYDRTAZAbAaJSKKCRRDJKgBKbBIIIZDCDZTTMSZbCJbQKSLbAaRSAJTBRQLaZZSAITTZYTLAhRAZaaKCRIKZQIYaYBYIZCLIaCZaLaDTQQLICYLRJBZCabYTTJBTTQIDYKYCaCUKALbaDDKKQQTZAAITAIJBLMJSZCQYCZBbBbQKTbJaIYLSALKKLKBBAKIASLIBRLaIAZAKCZDSRKZSZTYKAKDZiLTbQSRZaYADZIbBCTcDKIDALMaILJDQQLBQDSKTKSKYaLBBjaYTbKSKJAaQDZbTJDaYZKAUaJRaLAKJISYKjQYZJCRBATAAZRSCTURaTbYBKYTIDIMCQZJbRBBQURSiYhTRJCIBIMBZABQJabBaYRAbKDLZBUCQYaBRbRaIKIBJSCARcIcKKCSETSjBZbACiaZRBAaIcYZKRAiIYLCCDcjRARTYARcKRKJLZAaQhACDYKAaCKJMIJLRaBQCRbhIYZQIJRiZKBSBLiLRDSiagDZQiQZLSSJSCYZJCRjITIYaYbJTTaTCTBIjLAaZQLCDQbKDQhLKCLZTCDCBAKLDCEChZbYIbRBYRcCMAjAIMCjIiAKZbTAaSbhSjBIUIZKTDAKKBAhbCDJCKMICjLaLJhACcIaJbcDYDSUScBQASTRcZKRbSjLTaZjYSRgZQKJYQASDhcBYcUKgLABRCSjDJRRSSLTSBIYLUKbLEAabQTbQEhKaQQZhSLQYZaCAgYQJIDQYDbgbKLLKUKIBJEiaIhQDRRQKhUTDZIbTQBELJJETQJQJbYYCaIbJjSRYTUYRiEDTZDASBYQhYYSLSZTLQkbIEIBYbAULaBjARKRZEJckIbASDSJAZRTBAMSKUDcAJhBcYabDMaIAbYZYITCiTEaiaZaITSYQcUcKYEABALMDIUgLISZZQQLbLAYgEJCAKQQRkhDLTMMDRZjbQETTSSJMgCAZRjJIQIkJCbjJAAAAUYjibSRKcIBhJZDbgbSKDUiLYDBbBjJEUaRgSCECBBYLhTZcRTBTMCkbYJEUJKQLCBIMEZBChRZDCQcIkAkIRICjBbSLaZARRicRCShKBEZACJbBShchSEKBSDQLMRiEJIaJJQQDUBUILADkYgYYhghcJacbAADJLiAgabhTJaQjAAThbchgkQThZSUgAagLASJDRYcZYRESLjIJSiTLIBLIigSiBgYTEjjKCIIMCjhDTTDYEiZbDjQETUADZiaZAKBbDZEhhLIkKTgUEEYjCiDUYJCLhUaaQYLMQJCRDYcJDDRTKYYkjgQYRTCZRcUgAZZMkiJjSSISaMERTAkBTSIDLYZICZRBYQiRYBCkaaJkhjUkUJLgEYgBDShCRDEQZRcLQDSTJQjgKjTIgDIDYKaBLaiZYACkMkJkMjIEgJELQJJBaBRIkjBJLMDbDCSacUISSkLMREIhaEkjTggDLQKACBLKZhIjScgcQagahCTCCJTcYZUIcITigYRCRLUEkUYajURhbhQciAAbQYJcRAAAJCRZCMDkccjKTZkAYUDUJjgIDKKCAJTLaYQDaCQhETILkZQMchAZDRQRQjSLRjkjZkikJZCCKBjThjChJDZSLRTCCYAZIUIKEbESkkhkZLgECBaJMMkhQCLZDbCJTYgTCiRhMhcaMEjSMDaUiibRjIMLELkgTaUgJAaTLZYZcASMIhbAAajEDigigMQbRDgcMjAbRaMkAMQcjEYTKcBQCJijKbDJiQAAjghijKEhjiMIRiAJcZJEMQbEhCiCQLaZJhJjjkZaRCiZEKTTSKYEUYZYjKkAEBMEcTDSgRiYQDIKRLYQMUIgJYhZiAbBJUYaYKJihDkTTEQAiUKgCIbkhIYciLjKgaSSCYZcMChBQLRSUhbYicDhiKJLcUaEUBgkRDQAECKibbDBhZRKBkTSkKagEDUcYTgcagLEgYLbZEEiLKSILLBZUQaCRSgREhAbUZZiSZMQCDbMcYMkMEMkSUYQjTahUCZSEABYZEJEIBibCKIRiEhJgkTERijJTQcLBbcSUBUZDSADAcaSAQSRTMcBCJLiJbKBMLMDKUhjIUjEJTBgBUQAAQCSRLCiRaEBZSAZKbYBbbLJJDCIkMhBUaBhLQLikiUYEkYaRAQBYBYRJZLTKZYZBiKBDYkMbZAMAAZcbEUQjSJkgCYUKADBUQILLMLaiYRTBZkLSYgBiaAYLQMiASQDBQYcUISJRRBaDLSJRSKgkhTbKCDKcYLJQLLIbCcQUQQZDBiLiAYIKAhAIZMaTThUaYSAabYiKCBKZRhRLJMSgQJKTaTCMCcDcSRQQQgZIZLJBQIcibIaLLYcDCbMQIMCKZhYICALRQKSbjZaaASYYCaTYLkCjiCBcQMCKKiIRbhYZEaLgRbRSZCbScZYMQYTEIThICgaBSILAgbURhcbgBIJDQRbZYCIYTYSiYBKcICKYSDbZLgSJQYITBBYJLJUZYUkADIYCiRQiAIbhJLcaSjaQSLSCbQAQJBbDagRRUQZRIcbBQUUhLJIIUCBAJbjiTaJCjaBITEQTARAjRgIZbZMKYgabKLhaIUBbhYgATCAbCBJSZjTKSBYDbBCThbDADMSKZhDZKKIjMTKRBSZRTKQLSCZRSISRRJDTLjZILCTYSZKDSaRIETLZTCgSYQabgRSCchRDTCSSaZjDJDYRYSLAJSTKgYCZSKDakZQQBDbQbiiJKLDQTCbaQKgLZYKIBYRQSKSITTIMgjTYDbDJbJKZQjTAAKgRRAZBCETBJAaBDKRIaRKZSAbSJJDJABBTgTIIDbiJaYQIMiALhABbTAMaAIhKQAZEgLCaUcLQRTScZJJbYiaTaAYgJAJYMbaBabAJQbaSbIJbAYJaBgCATAAhYUKBAQYDYLDCYiKLLIZYRRUQhJRLZAaQAAaALBTBTaKRTAYTLbDJRLMjBJYCaCIYaSRCZZJIaAkAiIIQDRTAbaIYTURabRCDDLDbCKLCYaJjBQbQBDSQDICcLSJIIbKJSYiKTIcSAQAMZBSRLYZRRcbiSLaLZDYaCbaDSbUIbJRLAhJCCLaaSDCJSIbLjQBABQSQaAZbTbaIIcQZJSSKZbTbCDAJYbQbLCCSQQkTAIKZURRJDQbcKJbJIBjDQcLKYJZADQLSIYJjTQbABbRDZDABhRCCKAACEaZZLbRbbCYQSTALcibTQKAYQDSTLIkJaSaULScbRSKDAQQIZIYIULMQbQSDBIYJZCTbBYRAZALbZSCZaABQjJbSEYJiTbSZQDCBcZJDYIDLKJDZKBQiJAQcKYDYAZZASYUBChDZAQJBJbSgYKBILTTDBBYaDALBZDKSTLIabSRbDJARKIMUYSLRaAaJbCYKBKSSJaQDKBaLDLJBcUZDDcCZDaDYRQQBRKYKKYILJaIYbLThSLbRZDZRCJIbBDQAgABiAbJKCRASIEQBAbbRSEbZKBTYhChIZCYYQaKBKCADbBBKgDbUTRIYRaMRECCKKQAaCYZCJKaKBYLQLYDBICabRDZQKKbJZRRKaJBAAaSAJbRbLKIBYKIaIKSZZUQbKIYKZTKTYBgYKbMAIBAaTQigZLIYITUSJaRbKSCDQJbTAECbCYaCBZTIIZZhSSQTBaKALKBRZbZbZRCCCaLJbASabZYiaJKJCLCIQDbaCQDLaiQKYaBYjZBTCCDKJRKgRiITSJYRaACbRMYJQaDCQQaDZZJDaSKBZaJYCRaCIKQDDgaCISKAbIbLICASCZTBBLBJLDhBbjIbBCBCJAQQbKkaBLRSQATQEIDQDTJaSZJSJIDDTLITbRADDYaJCTYLiQCALJQIRaRZBCBbcbITSCbADbjDTbQIJAUaCBIQQAZZKIQQTSIIDaAZISTYLLZCZJaLRaQchQBDBLRIDARJbBaLQaAIJTBjSKZbBTMbZYYCTCZBKRCbJQCCZaaTKTAKKCJCRCbBJCJJDLAMgjCSIQJYZKKabbSLIbhQIaAQZKTQRBbJLZLLSaLDTSRAICITaZbASTYZQaDJYKLRQCCDKIQYCBAZKZAIKLLDRZRbDYTKIRBSJQBBAKQJCSBZSRbITKbaIIAJKTTBAQJaZCBbTaIADCDDYQJSBabSJDbTZRTLCLIBiIRDCQbLDRCTQJTbYDZCKZRIbDTYZCSLBRCTUbaYTbSBYSaQBYIQJAAIZZBLQTjZiSJZQLIZDTJASZKTDBQZCQAYZbDKAZJQSZYCJIKAJLaJbRbbRYaAChRBaYBjSQBQTbIbZbUBYDIYSBTbRLSJbIRDYZZQBaKDCQBBbTBRTZSISSYDSBcTagAQQZRBTDaLLSYbQKaRSaKKCaIYCDIbDQbaSJIDJCQBLDbRQbDLbLCaDJYDDQKIYDTQAYIKJZLRSJLZCBCDJTKSJKQSabRIJCKJTbJAATDAbaAaEQBRCIKJaRbBBCCIaaJIaITDZCIAKZLKTZLKZBBIaKDYKSQCRDSYARDAZJBJAbQDBbYLbASSZBTCLYTbKYJJLYaAaJDDTCJSAABCcQLLRJIiDLJRKBLSDaYaTZKJaLDYaZCSQBDSTTZSSDKQbSJZJCIBaabQBKTJRbbbZBJJSbZAZBaTBCaDASAYCTKUTJKBgULTLAASQJCYLIAACTYTSILQMTJBZQRCQaYIaCQKLAJKBZYaLJKYTcZAJBSDDQbaQaJBbbZDaAQBaQBaAAZKRbbgLIBjQYKBRTDATLATIbaKIDZKZKTYQYZRDBZBIISTYAZJQQIQZKRDCRJCKBShQTAAJaYTCbCZIaCILYYbZDIASTbYIQIBRcYRDbZMIaQDDAABKBBSDDaIDDCbbDATDDSZLaYZbTITAYJQQbaaRLJIISYCIBKbZZBLDBQRADBAaBgaKCbKSKSAZSBJaQDRJCKTbZLCLTaTYQJCCTaDJLJLRCCQRRCQJTYYTbYcbbSbBAaCaaZLTYKaRaQDRDZIIIAUQTZSIJLZRRaCRKTaRgAQZYbYRKbIICbZAAZDKDTaJZKILAbTaLRQJAKKaLATQDCZaBaDCQTQbaQCBALLBDZITTITTJLTLCCJDQCbILJBCBSLRaaSSYTTSaQKKCKKIAZbTCAKZBKLAJQASbCbhALYKDJDBLbRJSBTbQDJZbKaDLJYLZRLJKAASSDKDKaSIQLJiTBCScJBCbSSJbKYCIDbILDSSASCQBJZZQCRaaIYZIKALLabBRZBQSBDSTCKKbSYCJaLbDSRBDBRIQbBRCjLCKJQBKIJJDLQSTLRYKLgSSaBASJBaSBYYaABKjDJKBDYJYDLRYZZAJSbYKCCTCbYDCBCCLAJDZbCQKQCaJAYDbaTTDJTSYYTbCaIJIJTZLaAAARYKATDJaILQRLLKILAbQSQLYZTbZaZZIZKYTSYAQBQAZJKASaYabDAIRBbABYaLTBQKQbYYaBZbRCDLLJASYQSBKQLaIYQYSLYJaRTCYYLCJITRgJSaZCJbAIYBSQKaBLAKTQLQaJDKiYYbRTZBJJSSZLaRLDcARLJJTJAaSaQKQQSTDRRRaCRaIAYRBBBSTaTaTACZbTTaDSYDJAQCaYKLZDbAKaCKTLRYZAjLLTSCTCDBDKADRZBLLIKQUBQZIDICgJZbIaTJRRKQCBCRCZZKKATTBRLQZJIRRDQCJAISbDIDJRQBYaYKBTSKBLISTZKSBaJbCSbRTRbSCLIaLKKTLYSYbKISTICaTISYRTDIKQaBaQLSaICaCTDARRRKaBDYhJRZRYDQSZRbbbILKIKbEQIYSZZaZQKTSCQCAQJhLbSTZCKKQSJTLIYLKSRRbbLaBBJDSDAJZaJCZQbLLZKLSSAQIBDZZABLACTcKCRZIQLYDADYSTIBaDZJYZDZRKSTaSDIJATTbLSSAQTJRYLSZITKSIILSKTIQQIIJIJJAAbZTDZRLCYUQRKaIZKSJIbZSSaIIALRIITaKIDSZTTaYQLaIZLbRRJKTaCTKQLSSLATZKDTIYKBITabAJABCQZLJLCTQBScIALbLCIDTISRLTabLQLQQDQTRQDJZSJLYJIYSJLCKQLTDQZBRDAZCYLBYSDaSAKARJAKaAYbScLYSYCZBKZbLbDQCTDCRTYYbCYBJSJTCIDbYhbTbZAQTILRRZRTCQRYDLQASaZbiaSaACZYaDbYJKIIBDBATQbILTQQAYKTaaBbbITQZTKaARbBKaDZKTJZRYKRJBaJCACAQIZDZASRaaaaDaZIDbSDRZKADJIKZAJTDYCKCaIKDYBLTBaZCDSTgLTLRQaaKBRRCASBYCJJQCKRbQDbaDDBRCLJJDRbTASYLAAgJIAICITILJLAAZQYbKSJIZDQJABKJLJZRRBYaaKCTCBLLSKBRQQJbIDYICZZKLbRaaABRJYZZIYEBJTSTTTIBYbTQaZLKCKbRKDbARARBTQKLKKTZDYBIaAQSJBAaBDSCaIZSbBKATQYaaJRLKZKbKASaaZADRTBaJSSRCgLYZLAYaTZTZbCLKbSIIaRRabSCBRaAbaaQQCJQQSKKLALYLYJJCABTQZSIKSKbbKbTaKSBZJYKSZZLSbKbRASJQJCBIYSSKCAJAKYSIRDBDQZLRCLZDZDCILDICJBDaRZITaTYLSKJJQSQISQDLCKLLRCLBYTSADKYZBDYLALRKKDAALKTbbLCCbCZaBYTZLQDDZIJYSRTTDDRTASZJJRADaSRSabAYDQaaDLAbKZJRJaAARIaQaDKYKBSJKRCLKLabIRKBITKLCIRBALbCQYIALTDJaBKBbLIZDIBaQKTDbLSCaaSACaSBRBYSZBJBCAaLCCLSBLYQbKBYbBSSZBabYbKIRKDCKLCCKDIYJaLIQDTIKLRSJZRJIKRBKZKIACRbKTDTADRDQJaAQQLITIASRJbBBILDLDSDbRbCLZCaKQSRbBYCQQbYYaQSaCBIJSaTaTRDLKKIKZZISbAZZaZDJbZARAbCZQAQaAKYTQbABCCBTYBKbCSRBJTDBZZRaJAJYaBKJaJCCBJARTaJICQDaSDaKbDTKIRSLYTCZBIAaBSBDDJIQLCSACRRBLZACQLbIYKLKQBaaaKZQaCbBBRIKRCRIJTISIBTLQAYCbYIZYIQDRBBIITCLYbBZCYBSTKZbQQDYYbACLASbZTITLJTTCLLDQKABZQTSBTAZJTDCYTYZJQRRKRIACKADAQLRZBZBbLSQQITKZQRaRIDLZQYTLbQLIYBDDAaIICTBSBCIZRQILKBaKLCTRKbLCBbSBZDCQKQBQcSZLbICBRKRCJTRKZTRKQZLBAKIDCBLLDAQZDCBIASDLaJZBaaQQZYRaJDYbCKKARQISKLJaBASaLYTJRLCSIYBJLQZaQZCTIbJILSSKQKaYSQJYSSTICYbBCQbZaDbIbDLQDZDBQDCKAAYaLBQBaILLQBLATSRSRTJDBIZZZCLTLCaISTBRaZSBCbAbBbCTCTLaQSaaJBLaCRabIYDbIKBZaRIaCQDIRCSIBJIaCYCYSBQJJbJDKLCaYYbZIKTQLTDKJRaICZICJTTSATIbZSZAKLAYYYITJDSYKbDABTAKbJaAYTBRJRLBZYCKQLIQRLbAICaTRaRaTJBSCTQTbaLCIQCAYJJTJZQZYKCYZBYaJJDYaaARIQYbYCZJLTDTCRCSTSBIDITAJBRYSQBSISLSaJRRbaBDKYaKBICTaDLDZRAIDBZTYTCYLSbaRJSIJRILZZCKTBKBJJYRKQDaRCZRDCARDKSAYZDSADRaLRbZCAbITIbBYKRJQYTTbYBAQAKBSZSKKKDYBSZCaYJKKJLQCIIbTRJZDSbYTZZQILLQQLYRTTJZTASTQaARDZYISSTSQbLbbTSaCTDAYaCQCTJSQJAIBaQBSbaLBKZaZLQbbDRIQSJQKJLQRYCJCYQLDaBCCDQSYYbIZLTLKAAQKLJBQLJKLKDIABKJbZDADYQYYAaISYRTAYCDbYZAbDQbTLSbKJTJIaIIKZZCDCKZZbJKLRQYIbTDRRQKKbYTCRAASYKSYaBZQbBAIKDLBAILC"
"CRZaDQbAIABIJQbTRKLZISYaCbRZBIBQZIQTJQTKZTJDIBQEBRQCBYRTITILaJbCbYKCDZSQAaTBbSJCJJCbRIYCBKLAQKRCYALBaZSJaASCJLLBDCJabZIBQYZICSZDBSASRBJTZZYKKJbDBYYKYYbITKJCYZCSRADTZDCYYCBTYJJSaIBATBJRKbAITZBZCIDCbAAbYAAYSLDZCLTbTSRABKZDLIBDYZJbLQKATKKZCZIAQCQTIQLDRKBQQILALCZZCaKLaKbQLbAQDKYDITLYCJKYSbKTLKaSDBBTTARSKBQiTATCbDDDBbYBbKCaCIJJSTBCKBTJJaYJZQbCJTDDSCTLRDQARYIbYAABYabSSbKYBCYbBQTESJAZKaaJaYKYTJLbCCBSRSaARLADBbTRDZJRDaYJRSDAILCTRLYLAaIASLQZbJCCRSYaBKDJaQaLTaBAZDTJSSKAAKJKKTIbLKRIIIAbKBSQbRISKJKASCATbAIKZabQABADTRbDCLJJLBTBRSAZDZQIQZACKbTRQTRDQBALRAaSZLQZDbbKIbQRTJYKIRRZKCbASAbQTSBTIIKTJbADCALDbRQISbJbQZAQDbQABQRaYTaYDITbIKKALQKYKaCZbYRIBYTaLYbZQKBBSBIDJDZQDRQQDBDYTJCaLSJQJQCSAZJCRbBbRZDZIRLDbKRAaaQKaYCDQRRLTTSADDTRbJIZJDIZQTQLZYCYQDTaQRbBCbALDRYCCTbKbSQCIRZSADBQDCSRDRbSJJZDTJQbSSQYIJDAYZCAJRRYSbYDYJSYSSCDIBILaYILCASbSCZKBTKIBDbKBTbLJBCJbZKKCQTaQJJBKDTJSZaKCDJaDZBAQITYBYQTCJZJDITaabRYaATRTDSRJDCCBSTKRAIZBLTRYBJTTRRDITaDAbZSJZDDDaYaLTBaQbBbSSBLCKSZQBDCZbJLACYJJQSLTJASQDQLYICLLJbbIRCBKALKTRLILDDQaCRSSAIRADZDTDaCTbZYLTYQTSSLZAKRbLCTIYQTbJQbDKaATCTBYQQbZKBRAQbZQABBJabDSKIBDTLSRbYSCKAIYbYCZZaJZCARZQAbBaIILRASICYIbYBIRaLSZDBSLAJJQTRDJDCYBaCbDLCJTBDQYIBASQRBKCZDRLAAbIDaQYZKBLALJLCCTSJJRTaCBYZSSBDBJIARKCZARRbSSJaaKKTCJLKJRAJbYQITABZbTQJBYBRZKJAAQSKRQZSbIDbTRBSbJQaYAaRaLLSZADBYZAKKbQKKDaARYTTILQZCITIabKYRTCSQQQBLYaRKTCTJBYYSYSQDLJLbLKIIKQDYCISBCKIBKKTJIZIQBKRLLLZTTSIALYCBSIaIaLRSbYZILJTIZIQRRLYbLCJJSCLQLYSATCSYaJbSYJTRDaQISaBSKCLCBDQJDbTLSaTBAARZALQBIBBbQIABaAJJTLKAZTYKTIYTCLQJAYAJbCDbQDTTYQSBZKDSSTACbKLYQbbRIZaJTIYYCDIbCKQJZaCRIaSJaKLIRCTYKRJJRaYDYabDLQICARBJTDYTJSYBBJbbIQKCAZSBRAYSaQbISZJRTYDRaSYRCTTRYASIaDYJBYDAIbCSCKTJYJSLaKYJDRYaQZABaSZQZIBQZaYTCYSSSTTQRaTYYBRbSaDIBBLLKCDbbJCZRaLKLSbTRJRbbIDBQbQIaKRZbYQSJBYabQARQYAbKbLKIIYABISARSDJaZDQJIKaCaSSQZSaKTDaBLZIDTLJbBAQCTYCBSJbRAILBSBbTTRALBDKLBRIBQTKZaKbBIYDTLSSaaTTDTaJACCCILBIZIKRBIbTCDYYCKbSDKJSbDRCYTSDZJRATbBRKSRCIZDBYAYKIQQYDZTRATYDKRBDBJJDIKJYaRCDbRYRZDBQBQALLSQJRaRAIIbaCTbTBLZRbKIQCACTSRAbbBbBDCbSRBCbSTQITKKRTJBBBJZZCbAbCKIBYaRARbTAJbIKSLRTDQKSTQSITZBKaSIDZRRAZLQKIBTITYaJLIACKJKLBBCaJYZKKTIASBBSTbAbbJIQJDRDIDRKKCBTRLbTJaTQaIQQKQDJASaAJIKQYZRDSJaKKSBBIBKDaSRRRSLRRQIaDZIKCIKJLRIKaIASJRBTaADCQTIIaaZZZCaCLTTbaISLZLABCCBKLTKQLSKYbSRaYYDRDLCACBbbJAbBAbQKRAYTYBKJDQZTAZRDDLLLTSSaaDDaACBJQIADBZDYLbTKCLDaLCSSAQAJIZJZYAKJQaITTRADZQRaTDLRbKKSZbJADZCIKCISKIaaSLabLDSRDRbKRBJDQYSKATCRSZbDJJACLbZBRRLIaZDABDACJCLAaDCRZbCKbJAZAZZZJDZZSTJZCQIBTQKSCQBTAKQBSQJLIJJJYSBTDZLaQBRIAaJBbQQQTJQIRBQRLRSDKISTZSaRSCIYYIJRAZJSCLZIaQIaTSKbBbKQBTYKZbCKaaAILCTbSRTZYRQSZAJKDbJYQQZaYCRSKbDAALISLTQLaKAbSaJZQBYQAJIASbKbJQYSbJTJBDLAZaKDCDAAZYTbRCKLTJaAIKbabCJSIICRbDJDDICLKKYSQCAIJYSSJALRbaaBLRDBBbJbSQTKZJBYaKCLCQJbASZSaLaZKaYTALQSZIJAaYIBYJYTKaYRBbLZLKQIAKJCYZKIKKQaRQBBCCLBQBKCTSbLRALBLTZTJZJRJJCaTSTIABCRRQAYKJBDZKQLKIbBYCZJKTCDTSDSKACTIZIBaDRJZRBCDKDCATbACQKBLJJATBQSSZJLBKRZZAZZaBJTZJDDaRKQCSYZSKBIJRBYSQZCAAZTTaZLTCZQYYBTZTaabIRRabSIbSIQbSJIDKKAIICBaZRLTCCKTRbTZAZBbBYIZSKIKbDSaSDBLRBaIJCSKQQZLYCYTIZATZKCRJRY"},
Embed{91,291,"BAKJQILLBALIQILIIIALIJJAYALYJIJITLKYICAKAIIKIYbKTSTSRLTTDDTTTLTIQRDSLBTTDTQbDAbQTTTTIRKJSRRISBaYbKRaZITKRbYaRYAKRBaITBaYLRLLRDAASaCITZaSRSCJQRICAJQSbZJCIZYCTDbKQTbCBbZIaJCbIRBbCTIZCDYILTLBIYbTQKBRZKIABRYSJDLBbSTABRICIQbISKaDSZSRBYAIaTDbTCTIKBYZZbRKZQDBbLZQRKSaDCbRQaATBZISBYKRDBYRSIKLCYaBYIDbbBRLQJBACRYCaJTaKQBZIIaTIDSJbCCDIRYQZCTBAIaDabRQZYTQLKSKRCIaIZDDKJbTSIDbKDQTDIRKYQBRSDKKaZTIQCYTaJCJLTZbIJABaBCRJQSKZDDTCRZbKYIAIQQaSbTDITbJSSIBbBJaRRDTCaILbTCKJBYIRAYJYILSLZZZKbLYCQYKCYAAQLQSaLSLaCBZTZJDACYYSDaRDYBDDJaDDYbBATKTLRBaCYBKYSDaITCBKRYLIaRRYIICTZKaAZBTLALACIRCaIRbSIIBAJIZbATKAIKTSJDaZRRRIRTLQZRAIAZSZRICYbAYTRJQaCTRSLQLRZJRbLQQQJLQBZBBRKTLLaZTQDTBRLBJCBSRRSIZCJDDYASSAZTDZLbDSAQLQbATbTRKTIZQRKJBBTaKSZBKDYZCRaSbTASCbLCLYTCLIDIKRSbZKCDBKDZbQTbJAARBSATbTJSaYYAZYKLKSDbSBSYaIKSDaITLIBBDBSbADaDJSYKZYIDKLDYQbKYKRTQRTYLaDDZCRDADYIbBIbKQaRKCYbAIRRSDILZBBBbJYJQISbKCCLSRKKRaDDIBYJCZASZRBKKDKSTLSKbYRIKYZDKbSQaZAKITIDIBYATZbRLBDSILAaTAKZCLABCYbYDRBaQZKLKQQZLQDLLKYSJSIJTIBKJIARARDQbSKYBbRSbIQaQCJZZABTKDRIARIbSTbKRYQAZIRRTYaZYBDLbJQISLbKJICSDJKYCTAQTTRJITCYDBTaIaRDLTBQZYCZbaZKRJQSbYDBYYBTbBSCRSIJRKATIbRYTSLQRLZbJZLADbQBCITBJJRAZICaAQRSKAAQDCDRDIaaBQAaBJbBCJIJQJSBaRRAaILaZbQRIKJQbLCaZKaYaLLKYSbIIAKBaALJIACCaRaQQYQDYaZKJBJIYZJSKTBaSIITKQZIaQBCDYJAbaRRICbRbARCbJDDZCYCabIaBSbSKSZZBQBZLJQJZCYCLIZSTZYSaAYIYAJKTJJCbSLBCCICDDbQLCBRYYDRaRBCKDbQaLIQSYbDZbQLKIDKKRZALZJAaQaKTaKaLDCaIJaTZBSKKaTIbBCAAaJTCCBLTAZBbbYBBQJITaYSbTIRSIJCQZRRTBLSbJICJYSJIDAJKITTbQZUZIbJACIKSRTaZAZJSbDBQDSJBLYKCQADaCaZLLZbCaKLACIACabSbQALbQDaaRCTCIZJKRBTDCJaILIJaJQBKRJDBJATCDDYaYYRDQQbILTaTKICBaKKTKTRCSZTQZZaQIRCZbDLAJTJBaQBbQJRBTbCLQYDYTLKYaASSJQLBDCRDJJKaaBaaQAbaATbLRTTSZSaSSabILJZASSJSJYZRBYSJAJaDKQYCabCYRATbDLCBaSJaCABTZDQRRYLLQYbJDDAZTRaCSKYCbaADTZJTYCZKATBCJaATBRAATJDSDSIBAKTQRbbaLaTTaSKbKYZRZTQDBRbQKBBSRaaKRTZZQIaYYLIRLSRIYCTALDZCBCbbQCTDaaBYQITZKZCZKBQaAJaDAMaAAJCYBJJZYZYIKAYKATABDRDZTKSLCZaRITRbYSBDRRKLBRKSKbBQaDKRDLCCaQCbUSJACSTJJRKRIDLJDYKbJQIJYZCACLCYELBSJSQYTbRRJKDaaDYCIRKLbDIKCLQBKbASRICaKYBAILKCDJJaIbIDKDDRCTJaLAJBQKKbTYBKJCYTbaKYaDbZKbBSbITSASQIBYKaTYUIBTbTCYLSBITJAbDQTCZJQRLRAKDZbILQDAJZCZBKZCABDBaBRYThQJBSBQBRRaaLDTLILRTIIICLDDbQCBQaTCZCjITBZJBYYILZCIZAQbITKTAbTLLLDaIbJISQbCaKUKIATKSQCIIaSRLbCJDRbIDCSbIQgRYZbKUSBKLTRSIaCDZRIIJjYDRbDRYIDYQZYYCBARMKLLLBCCbTATRYKSZTQZQYYZaLZIBDCZARILAEKTbbaaBAbKbIARJJDDSSABJbTRLSTQQiQYbDMAIKBUZLSKaZACYRBYCRaSQSYJZZabcYBhJKLRBYRTAIkRQJLbLKKZDCbZQAgSBABKJTCbSIDCTQLTaYALCQSbQiYJLKQTZCARILBSbbEcSZAATTJDIZKDTRCTRLZBaAKbDYTKEARaKLBCLTDAQBIQZKIICDILIJETIIARcSZQQgYaaacbbTLgZaJKZhEDBSaYQRSCDDQaQZTJUQhIYRQQJZCQZbJBIYZZYcCQZSACaCLLDJQSYcIbThKZBDRSYYJbIQbCARakIScABBZbYRbZLaACQYQZbBUAcSDZYTYIaEiZKSBIbYKiQARLRbRSJYQEZbUBBJDZBRYhJAhaLISZDLADjKBQADaYYZQDCYKLCQKDacDSkYZBTRbKCbUCQjJTSaShJBARDDSaJSRRbRQiTbcbIKQUKThAhRZaKCMSRZYEBAQJhDQBhbYjKKAbbYRaRjERDLCQDIKaZChYLSQZabCMETJZTIADRRJUcRAZIbYChJCCDIkKaZIJSBJRbJKCAaLbSLDgRADDEkQjZSZRUKkAgDSBBRDLBCQZQCBRRLIicbkKTQYYDiajgIQJJLAQSjZRCDIYYUTZZjSCBcYZaLaJhLMZTQaQUAaaTSZDDSTbTYRiQJYJQiAMagIcQAJDMDQcJbaIBjRiURIMLaBLSSiYUBhZBJiajSRCLaCIEQabYDAgSJBLRiRKgERSIbTLBMBLZQgIBIgKbKZkaSKhcaRYLbYbJLCkTUECEACaAjBDaDJAhaBDgSLJDAChTMRMIKijBARJcDDMISggJQARLRSTCgcMUSaKBYBaUJibUCSZIMZMLLRLYjIgAbQkgBAYKLYZLZQJDADcYKCSkTBJhCMgjIhgYELLQIbTYUURCUCSRBjZZJaYQkAicCCRabKZMYcbkcJZaJECgQRYbiDTaaKYLgCcYQYDkgRCbcUTTLYShgcZDJUiCMKakSMYLCDSAYJYbIKbTgIjAgZECikDQQYcEUjiLJTSZBAEkhhJTbMgBCZIUcILbYiLBDZaYUTIcJZgUJjgSaCTBgicEKEYQBTbbYQaAQLZkSBDABZTLKQKJMREMKjIcSkhSEgjYcQkhgAEYJLJDjChAiLDbkhIZhMKASUikKicgJCKQUDZJEcQiKSDMCbMCSKLTgJaCAYZgjSTRJbikhRkIEbDZAjEgcUhLJQAUDZJcTTSKaBZkiTZbiJATcEJCTIUhMgCihgjACBaBYgBiLjUcaCLCAjABCcJUJIgjhjcKAIMUEgbaQkQIQhMDUjJZDcDUTSYZBaDRQBJSKRAkBSJcJbCJaAJQkjckRBDEiYKIEMIkkLkbBLDUgkIYBKaJDRQcASCiQbMcKjgJRMQbAiZAikLIhhgbRLDkTBQZRbUUChJBJDSkaLIKkCcTgaAJkKEShLiRkScIJZDZjLgjScACUEMLRTAaILUULDAIAYLUMSchbShgMUgKbDYRQggZCcECBaKYBQLYKiUJhcgSikKkLbgcSRbLYiAaDIhIYQIZYAgCAMchCEgTckjRRUkbZkKZJUJLUACbbcjQEIaaLSbBBJCQCEkUcKDbSShSBCIZjMbLSJLEEhDMkjCigZIbKCaYRTKaRJaYEBRkKMBbkChEcaATUkMggCbSbkRJSSicZajjUgcZUIaiiYIDjBTIBLQQScghTkbjBIEKhkQhjagKiiARJcMiZCBZZSicaMRZkUbgBJkBkBJaRJQDbADaQkTDgSAZcaMEUMMbEbLgDEBMhKCcIRCgSZBZijMcSMDETAJYQTiaakMgkSCYJQLBZkgCJjEEkcRKYJiBchBbUAQIgETDaKaDUEjhLTEIYYTRQZZkQQbajKCgJiDbBMRJQCKkCCZgYLAMUkTacDZaIKLZKjjRKQDYQcACSTDSYBMIkJLghbLCCKhhSYEMaIgTjAILTZJQAAQcELYYYbEEQcbcTDKDAjhCSDbhiEJARScCcKTQbCJJZaRkaQEKgcTKCBjIaYIJKkRbDAjbTLYRjQUKEMCjiahQaBKMIBQSAITKTBjRICQLEMMBbABBZESBZESbYLRJMJCITQKIQYKZQEEAcETZQSLKbKiADRChjTLIRLQCaLEZjKLTLMIEbRAYcCRgabRSaKKZgaSRQIhELRDBZYEQbLQaTBDCYhMMbAIcDSBCSbbIRJaBaARLALMAEDhRSkTQLcQRYMhQbhTKbJSYiDLSCKUKQBBUCgEKIDKZBBBYJIMZIhLICbJQMAaRIJKZRQDiZKZBATaZARKBKMJYCIAAQUgbDcgMCgCbgcJDATSSBhbSCRLTERiKbiKYJbIcbZZCAjRjDagQIKIYKjhSJRDTLIDZbDZQLkaJZMDaACUYYRQARkJKMbcBDSLACDkKDLIEMEgMTQUAaQSabbJLQACJbJIbESSIDKaLRTSRgBAALTYibBCZbYSIYAakADQBBhBAaJSRTZgKbDZabLDbbCLARTjCSQRIJRBAZTMZDSThBAKYIJQbTaSSZiSZSCYBKhTbaITJTDjBQQKKZaABSJEMAICbZBQLCgIITETZTKCTCZDBhbDgShaYhbZLBAJiYSRBCRBAADSkTQaaEahKbAYaZJYbYbIJLIDbjRTRQZkCSCaZUQQYDIISIjILKEbJITKDCDJZBMQbIjMTUJiRAJKaaKYZLaJAQBAaQIYIBLgIDLIBaTJaIYbBbZDDLCYCDRhRJAJTRSKbbbTRDSCZQDLJSkBIDajSRRSKLEJjARbYRKATSCIaRCZYbQBjCjCbKhYCCJAKLYQJTaBQDKRbaKTbALjKJSATIjAYYkDaICKLRChSbQbDaULCDCKTJDAkTYIZJZjZMIiAARAZbbKaAIZcaATKCSMQYSILBDSKSIYJLTCARRLJJZLaRKJiITUTKRDAILRRDKSRJJBIaYIDbBQaZUEARZBIiCIRKMRCbZMJZJEDSJDIaDIDLCJJaYYSBQBQQRBCYIRBYjbBjUCJBLZQCZCBbLAILICYQZZRYRKDBTBCIJKQLgCEbCILYIBATTKbSQAJKAJLZLSaaLCBIMKDZQDaUggKLDDSbTJBQRbQJYYTaJQJCAbbTDJQJZaACJQMAKYaISCTJZQSDRUgLZCjBYZLASCcaCMbITTADLjTbYBYZRAQTIhTKZUALBaaIBICJIKZSSCaSbcLKJYCBIabgKRDSRRKaDAbTZCIYBjZBaZLYgQSLTBQTbgaaQZaJbKZaLJTQkUgKSSJBALBCBZACRbZDYBSCLaBjASLAbZASRaIaDISJYSJYTQACBQLbRbJDAJbSaTQbMTbJITZbaKDRCiCIYbTJDYJJSQKYaACKQjhEYCTMRRbLaQQBZZSUJJbaJZYSASRSYYaaQcbDIAYBAJCZKLhgSARQDJJSSLBJZSQIcQJDBLQBIAYbARLLAAhLTYLIZIZZASSDUQZJJTRYBSQJILEQiBQJCYQSbKILRKZjRTICZDRbaAKJKKRUKCJCZbKLDILaRBbZIABQKACRRQMgQaYRSKADhKQLYLILCQZQaELBZJRKBbJDDTCUaRQRSRaMJKDADJjCBAaKCSLZDADCCDJRSYCJKaAgBCDbTARSYYQBgaQDYRLASLBCCBKLLBZJJCLBZjSLSRAbaBEBJAYRaSaZCUDDTjLTKaTJLiTSCLQQKJYgRLSZiDJDTBQDIaLAKIcTbaYACTQTBaCLAILDULKQDJCLTbbRhhLaIAAaZLQBLAKbDLbCabAcJbJcATACLSYaRQKRRRRDTKTISJTgDRASbTbDCIbJaATbIIYTAIKTTLTTSACbICBJSRZhaZJLZRLQAbBZYSKaYbCIDjBLaijbEDRTAJMSCCIaTADIZRATKQaZDATRbRaYRZSTYhISaZAILCRDKaCTQaDaZZbDILAKDKQDJAacKSDAMSbBYZZabBLITBDICBREQAbBgaAKSZYTZAaATSDJaABTKQbCDCQIYAbKSZDCaLabIQZYKDRCbZKILQULIbLJYKQKQCKZTCSITEZDJECICLSJZLZYYBSCLSBCQbSDIcbbISTUITTARBRISQCRSbZcIjbbSKBZSIKDbBAcabCITZIRbRbQSBAaQDALYZSaICCZCZbBCQLBaBYbYDaJRcCaQMbYLYIRZCLRaZATMDDLbISYQaKaSjYSSAIBbCJLaYZQKRCSaJACCSKQCDUbKBaBTKTBBRLYJJRTQALICZYKYcYgDZJKTZZBCLICKYCBCDbbQSJTaMAhEKDYLKLDBBAYRbQAAJRLaCJaDLCYJDZRabbYQDCJKYDSAZRibQKRCSJCJbIILYBbICSABBQCKQAQCRSALJSDLCACISbbRCCbLYiaCDJiLJIZbbSbKaKKKKRJLKIUbLCJBcIRQKJYKYCLSCLbLbBCDDTTACAIAaTSZCJIZIQRKbSDAKaLYCAaTKBBKYLaQYTaKJCRKcCRbJAJbTDAJTbLLBZZKSTDbTIDSaRbCJAKCSDTAQQQZaaabTTCTABKBKbJCCKRKZCDJRZRJKAJKSRTYCQKJZCRLICKSIKLJaIADSAAbTaEAKaJLTTYECbJCJYLaTSILBTITJRCCJDRCAbLIQSaTZTaYIBLQLQQTBQaaCKbZCaKYISBKJLSMACRQIbIaDgabQJRCRbRQCCaRZAJIIZYZDSCLDZZAQSjCSLQbLKDYTCIIJTLTaQKcSCBDBRJRRbZATDBBQRRJSYKTRRTZQZBCaQjRIZiJBRJUbICJRQUYYALYIQKLJIYbQCTIICYaBRJYIIDKZKQQSQabLIBCZRICBTCaRbTIRSIjRTaAKbQALaBSQbbLbDbLYSaCDRbbTJSTACZYJRCJLLbTKaKACYRADTBALbBCaZISaDBBKRLBRabQjYaJATYRYbBSZCTKILSARIYDQIBZRIKTTYQBKLaCYQaCBAJjYIIJaJKRRTQaYKYLiYBSLKTKRRYabBIAQJSBLKEbTYTLRbabQIBRYQTaIYIAbCKKQBaCaJRQTJQaJDbQIQTQDRZRMRIJZLTIBCSTQAYJYJZTBKRLKaRKBDLZQaLUAYaCKaADJIRbTaYUATSTaBRbLBETSRQTSBISDaaYRLDTZLbSDaTDLUCQZTLYLIIDCZCZRLaSJJIDaLARKQYQJSYZARACRZbALBKbAKSEYKSCALAKJbRTZQIQbKLaKKJaQbDCJZRRLRQhZCLJLbBDIZKYQRTRSQRJbLASaaJBSRICCKLLaAYARITYaSaTYCATKCBJLDIAIbJISQaKTbDBJSJaLTDBKQRBKSIKQSSJLSTaJBbACJYIAYbICCDLaYIRTSabZaLKSaYDabDDbbABAZJRLZRDbBRaJAKaZYIaKDZQaLSQBRLKCRATAaALQDabKDKCbDYAYbDZbAKSCTSRJDbBJBLQbbIKabCQACSBaYYLICRRbLISJQJASDAZgQTALSBJKbbTRLBDDZSSIRRKYIICCJDQZBbDAZDJLbaSAKCZCBZTZRCTIRCKLBSDKBKSaYZYQQRIBILCILRRAYQZZJbZSJSARCLBKKTAQCJTBbaDASAbLYTIQALTQIALJTIJQbRaCSITZIaSKLITTIKQgJCADTAZADTCTDLbZLDTYaIbLTaTYZaLJJCaYSIACQSSQYDIZAAYaCKRABZJQaDZbCZBTDTTgCRbBIBCIDQYBKJKIJYaTAQJKAQjIbLJAbZLJKABYKIIQKDAbCBZKRDaKLYTbLSLBYYQBIKKYQIRTYACAKIZIKJQJbQTZITDYRBTLAALLYAbSKaSBICACYJQYYaCSMDIJjCCKKYIBJTQQYYaTDITDLJSLIQTCBbCYSaJZDQARRZKSDDQTTLKaJaJDbZTDSAIJTQTTQbabRLBAKCIAKABKKYKSZRZCKbTLKKZKgZYIYQSRIDBDRahSaSIaaJAYSaILSKJLRACCQZIJAYTbSLSDJZILYDAbQZTSQDbYRDbQjJACYCIRTZADZiRQSDICLTZaTYKKQDaLKQSDBBZaYLTBCIZQBRbRBLKSbRTBKLaSDLRBCKKaRJAAZZTJBbZQSQSIRDARZAKKQhTbbQDJQJCBAbbbSLCbRBAZQDAbRZaLQRDSZQhJQTTAbaJQRTAITBKTJCTQQJRQKIBZTbDbJIBaBQDCbIAbKJQBAJIBDaIRAKKSBTRDZJLaTLSQLSRRQbQCZRQRKBJIAACJTKYLIDKBZYAALIKSTCLJRCJDQZKQaJJCbYTBbCCBTAQBJSJaZCDTDZRBRDTbKaAJIKAaTBSbRQDKQbIKJLYQBJSaTabQbDQYSTbaLARIaCKIbDJJJLbZSBbAJQTAIaSRbbQDSLYLTCSLAKLQbADTAaaBYQTbbBDaBbKTLIZITKKLKYDQQRALIKTCIAZQKYbaATCBDZBRZBBaRKCBSQBSTTDIQKYQYJKaJCKBIRBLSLbZSBBaLDSQRATCATDAZbKJYJaJIJDRAZQLKAYJLYIbAQYYZDRSaIRbIZZASbISaRJKBSaYJSZLTZQADbTaCCQYbbKCLIYARbaDBCLIBBYKYTbCaQYQTBKIbKIbABJbCbQKKKBBLDBIISQTYBBRSCJCaTQZTaSIQbBRSKJDLLJZKbLQRKBYCZAAAaYDLAZQDIaJLYZTDCQRSLRZQZJLQSCIKYjDBJaKQbDCZQASTIBKKJAbQTYaJLaDYLCIKTKRDJIJKAASDLDKSIYBDKQZBCAYbLYCCTbZaBDDaZSRaZQTSaSaRTSRaabTRIQCDCDJLbJRYLZRIRbDIJDCLTCQQZSbAKIDTaQLbQKCZKSTJJZZCiYCLQKbbJbQACbTTAYBCRCYDDaDaTTSTTSDYJbBbYRaRaAbATKCQTbTKbBJLaSQSJYQTaQCbYbRTYLAZZRLBIJTIQBJTQJAaCRYRDbTbBTTITJJAAQRLLTJLSRbaAKaTRTDQYQaYChAaBKYQaYLTQRIYKKJADCaCZaBLDSZCCZIRbTJSTTIALaBDbQBAJDCQDARJQQRISJZIBbaLbCDRLJTAILZIKYRDLIQBTYIQILIBTYCCCAbYRLjZbBLRaJYLRCQQbKDRJTbSZaLTQITSBAKIIbDLCQIBSRJRDYTDAATaLIbCbYKAIbaJAQIYYYKBTJYSCJZTbZIKKAZaQRCTYIJDIaAQaTYKbTIIDCYDDRaTJBJTDDaCLCZALbIaBTCLSKCYDZQLaBJRjaCQLADTbBRKKaaAAbQSaDKJYKRRaCBQJKDLBZKCJBZISAIbIYaBDLRKAQbILCBTYBaLZTQDCCCKBCIbBLCIIRYDbBRDBBTDSAZIQYIKDAbDTAKZbbCQRTKLLRDaRaJQLBSDKSDTRQKSDaQILZJSRBYBIBQKCYaRYbYASKAbYYLCKLDbIDCaJJJCKDLbSZSICZCADBbADZQJARSbDbYDbSYBJAAZJIKKZBTBACZDATSBJQSBIZLSADRQDDZRBYRSJRCBDABLbQaYZIKKaBIaKTCAITLaJDDRJLQLSQLRRKRJLIAYISRLZTIKRBASbLBZQRBYZDaQKDSRIaYKZLQLBBBSCAJQBLJIDKYYJaLYCIYKSCYLBRJaBARJYbKRQRYYQCRSJbRZAZRCYaYYABbRQDQCaRTaAIYRbhTBYZbKZAaARTAAIRJaDYSYKRLBRSBbSTDYYaCbRaTZbLbbKIaKbYTCCALSQRKIKTZQADTDQaZZbTbTALBSRLZRZJAASZDaQCDYTLRBZIJLbZZLQAbRRQYKaKaSDSKYIZYQAZRABD"
"bSaDBBQKJCYYSBbZYabaAZIaBBDbYSARBJKTCACKBBDZKJKRKaDDQCRBZYYBJJTaCTCBLATCDZJCbTIJCaQSJAJaBKAKZJDLSTaCIaJZZDSLSYaQTTQSCZZSAIZZRDIDSZALDIRJbQQKTTQDLZZSZTZZCICIAZSZLLBCTAJbaQaSASBbZZbYRTRYaYCQSIYRCIZQALbDZIQIRJbKBYTKRZaBCBbJRJQbASaBSYZCSJaAZDCCQIbCILDRCBALbYZCaLJJbJBQIaLaTCAIaCZKRARKAJRaJbaKISBJAYSBTRDbaTSIbDIJLJQTRQCbACaSTQZJBTCbaJaBKCTTJYRBbKSIQAACLYZLSaDRBbLDQAZaLILRQTDKIIYLaJbJSLKCACDTbSBQaYJDaSaDJRCTCAQSZRIZYYLQDbACKZSCQbabBRSSCKIaTSTAZKZJLLKTTTLTJbSIIJSRIKCTTQDTRCbZAZQLADZKbLbBBDTASLIZAaYAbQbSTaJKARABKYYRKQZDJQDQZSYDbTRbQSRICCZBBJLKRRBTaIYCSKRZILLICCLIDSLLAYCBLJAabDRDhZIaZYCSIRZCRQZJAKQKBKKKQQAQQRKAJDACRTJbKSIJTaQTYLCZaAYRTJALSbaZZaRbCLbYQLZYBbYBTCQIDJZZARAQJSAbBDKBITaKKRIbBQLIICCJBDIRIZQDRTRCTQBLLZITDBZTDIBKKKSJBaDBJKRLSQSaAbQSTBJAYBQCZQZJYAAKZDTRJaQDZaJIKDKbBDIQaBZJRATBLRaJaaDCIaSKCLLLRCabbYbLQKZATZAbCSCDaBTQKLSSKKRISKCTaLTDDCaJQJZaRDZaABZYSIKKJCILZQDQBTbKZSSJaTILIIAbBDJDKYDDIAaQQYSQIJSbIQTaAAJBDYZIYBZYDBBQRJQDaaYbLSKDDaLLAJQbDaZaRILJRQLSaDQSZQLRSabICATJRTYAYaZBZTCDLJBZDTCJKQbAISRSLBaACBASTJLZZaIAYYZKRCJDAbSKASQbJDQaJCYRIKJJBTQDYISRSTKTDLQBYALaDYCAKLDRQCJZDLTSSABZCDZKaYAQaRJKKbRaJRIIZTBYBQRTJTIRRSILTBZKAYIZRbYSYZQBZSQRYBAJDLABCCISZJJTabYAaSKABYBRIabKTIQZQKAJLTTACLbKTQKJYRbCIADZJDBIILSKLQBbTYRZIDRITBYCASYZLJACCAJDZASaYYCLCCILZKISZbCAIZYJRIDJbSbIISAZZIBYBASJTabAYbCLQSCZCRKSIKBKZRBYISYRSYJIQDTBRbIBYDJJKARJYTJRTQLbZZYCKBbQLDYSSYAASbYbbaKYaaLCKDAbRYQZaQZabYTIDaDQLZYYZIAAQADQKbIbYZaTRZAAAYZBbCKRTZTCSJSRaQIQTCZKaLBaIaJCLBJAJQKAJQBYZRDaRBYIZJDSBTYaBLBJBaRaZTKYBYKTZSSZDQASKQLAJSARRDBbSQLJCYYQaIDJJBTSTTBADIITKDKYRAQKRabJTZbSKbIYSDLJLBYQAAaIYSbDZYYTBCKCbCIaLYKARLDZJKRCBLLLYBSaAIYbZYCSRZSSIZSQLBZDCKSBSTKbDabKDSBLAYBaZJRDCRJZSQYDAYBCYAQBJRRSbZTBYbTbKLYDRKIICAJKIJAZZCaYTKJLKZRIJRbDaCaICCQYaaSTYKSTJBQBZKZZaILYBLZATTTSABASLRaZRabZJLQDQQaALADAZBbCZCZCDaDBDRSQDRbZISRSbZbCZDYISSRaJCIRYSIDLQQTRDZQTTYRLARQZTQDIDbDIDCZDaADLaTaZYZRDQSSRIZLYaZIbBKaTKARLYYTZLAQQACARLYABDJJBDZITTBSSaYIAYYARCJaYaJbZKZbBYQRaDCALbDCZJTLDJaATTKTJbSDDCIQAaIAKCLBLCJDDSSJCALDbTQQKYBJIKTZLLZKbRQKDRZTAaSbRQKLAQQKQaDLBZSIQbARaJRQYQKRZQYZbIADLKYLaSSLYSYILZACaaJAAYKRAISLZBKIRAKaICLAIDJYARTYYLIbaRZJbYQSLTKYDDRKBRRRQaBYRTIAZbBBAQQKAKCCaTKZKJTYZKZQBKIbbYRaYIZDaDJCCCALJTBKbKCYbCQZJCTSDTZZQCaBTRTTQISDRDaZJKLaSRKIIYDKSBaLTSZRJaSCYCSRKRTKBBTLYBTZTLYbJSAaYLTQBQSRaiQILSbYCRICSIIZLYQbYRSLCaLITaBaJJRbITDZDaCCSaQZAKCSQLDKJDJLQJSLZKJbJIIZQDTQaTDLISTYKQDQLYLTBYALKLJYRCBKYSaTLZBBaKQSJARDbJKaQSISSaBLDBSLbDJKIDCSTTCSQJaCSLIQIBZADLLaITTKBYIbSJJYZCJTCLTDLRabSQRACALLKALAQQZaTLaLDDBQbRZZBCYKRaDaBDICAKACAAbBQTbKBAYSBQaQTTDLSKSbZKTZDDbYLJLTTBADBbKYbTKJYBSCCLDDaZBTRCICCJBbbKBbYRbCQRCBaLJTRRKIBZSJIaQbCZCJYTJYIARITKTKaQYDLCYJQYAaYALRaaLDYCJTaLJCSCLLZLRTBSQRSSRASCKLYaALYTRbYQLIRbTbTBDQJZQSBLbKAKBDBYQYRRLQZSaASLDLYSYbBKQJSTQZSYAJDZTBRARbKCJLCbbALDTCDTQQbKYBJDSRSBaLDJBDYDCBRAKDTJBLYLZTAJaaTJCSJBILIRDIaBZAYaAKbQARbJBKAQTIQCAbCZQIAaYCYSJZTILTBIRaYIaCbLYbJaBTYbJIZbSTBZJCSBIBCaDSaDAKAKbZTTQTLZJKBKCQRKQLBKbSLLAIaICADSQRYBITADTbRSLKZBKRIbCYRLTJZIZKQYDaQZLbYKKKLRKaICILaTKTACTbIbKTaaKQQCKTSKZRBBTISTTCLQJQJTSLBZBYKYDDIAABbSBDTLIbTIKKCTTIAaTBIJKYRJKBbSTSYCLKJTDCIbKSTDQKKZRKRaRJJIJIRKQZTBIJASKRJYTISYaQZaTJADKQYDLLBCYDQASaJbSZDCTKZYTYJZSLZIIbDJJBTIZRCAILAbLZDIBAJKYCJAaSBBKCJSaTSYSYBZKBaBBQAaQAaJIIZRbDRIBRaSYSTJYLSJSYTaKJbbbCCSQIZTTKSTKZJRKIKJRRCRbCIYaIDABLJSACZIQbaTZRCQSBC"},
Embed{92,294,"IBIISYLIJQIISIQIILIYTIIIYIYYLIIQLIAJJKIJIIYAKKJKaACaKSYQKQaSbbRDYaaYYCaCaaaZaaSZbaYCbaaLaabZISBaRbKQSBYTJYCZKTYDISBaCYDRYDLZKABTIaRDIaRbYLZQYBKDYaRLQKKYJDTYKRATaJQbYJQKBYKRaKQaDRBQKbKIaJCSKJQZYKbKRDIBJRTCIBYKITIBbRaDKISZCBQbKKQJBQQRCTKDRLLTBYJJDbRASRSLAbDKQZLbJLQSYTaTCaYCBJQCKaQBIBCASbCBQLYLTLTACYLbKRDKYbYDJCQKKZBIRZbRCaaRbAYKDYKBbRLCYTBYYTJLTQQBaDIAQJRaKBLATJCbALQaaDDYRRRKKKRLDYSYDDDKbYSZSKILIRCJYJBYSIbCSYDRAJCKQaRYRLAYDSYaDJaYBJQDDIJQCbKKaaRTLARZJZTADKaIYTSbKADKbJaCTLbRCJIYZCCKJTbIKQAYQCZTIbZCaYISLQKYTTBTbIYQQJaZCTBZKQbYYKaRBBTDbIQLKDCKRAQBKDRYYIBDIYbTKYKLSBBaCaDYQbSSJKLTJaaQSaQAKLATJKSbIbKBKBBSBDZIAZDKLRaaDBIaCBAZYATAbBTTBaSBDKYKRYKbaYRKaKBRBaLAJTTYKCKBYYZaIZZRADSTASISJKbYDbDYIQAITLZbYCTSbKBKJIDAYaQSIZSABbbAYBDSQDBbDKbbKJIRZKZBBQJYDaZQRJaIbRCSDDQLRZaSbCIAZBRRIIAaYSDQZYCSDRDYAZTQLLIRQJQRDQYKDQSCbJQLbIARZQZDYJaKbDBTbDKCBAIYBDbSaQSBZDRYZTBYZSSRRYKDDKLLCaDIZILRQbISKCTDACDaTILZDabZILCJKIQRDYZZSRALTILCTaQKSBLCYYaKAYRYDRRaZaKBQTLKSIKJbSZQAaYbDTYCLAaDKZQLSbaIaZTIKRKSASTTDTbDBSCbRJAIJQSbDIBKZLRRRIKLKLKSLRTRAZISbJCIaILABJBKSaZQBYCbRTITZZSSKDLABaALKAIJAIDKbZCKCaRSSQQJSJbTDDQQADaaIZKbLIaKZDZaJCIACBBDCBKKCTTYDSBDIaAITDZICRbCZDBYbARabbaLDJTJQQYBBRRaYAZLDaSaYabIKYZJSBKSKSaDCKBLaSBLTDARZIaSaTIRDbSJCCabaJJCKKLKRYABaRTQKYRQITKbQADZYTbLYSaCTbaTITTaaYZCZRCDSQaabLTSSQYKRILAABSJLZBJRDKBIbRQLYSaKaaLYIaTYKJJADYYIaRCTJTKDaASABbLAAKJbAKIDaIDLTJTBCZBSKTBQBRBbASJCARbZQTLBRJYbTATCSbLKaTBZLaSLZJDDKQQKQaJSbYIZDYQRSCbKKaQBQBDYLRBRZYaJLIZCbAaBBCDBYRBaLBCaJCYQTQYbRJIJCZDIJaQJCQIKTDIRQaBYbTaLLaKSbSISKbTYAJRTARSJZQbDbCSKIAaALYTShCRCDIALaICKITRZaTZTYbbBKYKDYYKLbKTRQAARIIbKQZTIYaJDQSRDYIYbIRQZTDCRIZKaQKTIJQZTBaZAbSQSDZZDTYbKQbSCQLTCKDILRLKJYZbIDAYBCDRIYYRRRQIQSaQAaCJZZJQILQJATQaCZYTZRLDALaSaSJSTSQTSAZYJILLDYRRIJLBZLYZKSBASIDbAZKARLACLSRZTACIICSZbBQaJKRRKBaaaQIZZADbCJQaRbLSaBTJDYZDJZASCCACAYJTQjLBIAJBBZAKQYIAaiADRRbAILCLYabJYZABYTYDTTTbBBZAaaKRCRITTabQAQJBRCaBLaJSDabYYabLTZAMJCbTRaKaTQSLADCZCJDCTSRaLTTYZQCTRLKbDYDbAZLDZSQJYBICYQZRKBaSaDLKYAZKZYaAQZTCaLCCATZDSZQLaBBacbYBZSRIICYJQDYCLZMBDRYRKKBQCbLSSLYLQTZBaDKLDRLCTLaACbSRIBaTaRAIYDUSJLLMYRCaZBJSTbKCTIDIAAbBKYZSDZAYSZZAaBKKYKBJDARbATLRRBAIJiKCKTZaQSJDaIYiTbBQabACZKJIARaIRSKJAZaZZRQbAIjDSCLZBTLJQQCYTJTCZYDICSiUJaIKISaZLbIADYDIBTYCMAKZJDQLJJRLQCJTbAAQSACYKQYRYSBCRbLTCAMaSRbADBZRBARAKAYBLSRDIRDKTRKTICbJJZaDQYCbYTAQKTbTCSBYRMAJLbRJZSZTIZAQZigIQBZAYYBKLKTABKAJDMJZaIjaDYAaRLBaDbACSbbaLaAQUSCJKZTKJKJQabJLQYCLKZaZRIYjSAIZQCZLCSTBCZRRJCIQKLTISKCCYaSRJcTABECbLTQbJTBIDJCTTgbQQULCSbIBRaJSDbAJLRKTCLLaZADSLaIgDQKCbTCgbBBJUCLaDIaQSYEaJJJZASbcJJQKbgjLQDQLSaKZCAEDEMCbaDaKYDCTZYJQiBKJYjCDbSYJKCKLCILQTCaJaTIbDbYQJRBQYTYDbDZJSQSQYiTUhajBYIJEELLMTZSYSAAIacYDSZbbJYTBARCDAaZZQAQKBBLKQBRLYDSQaYbbMgDATZCRJIAZDBSIDAIJCTTKSibACBLDZZZASSbIaaajRMIgBZALJCbMRCSYRSCUCBESJLgZaTZChcabDQYCYQICQCgJbBiBbIIiaSLLDJCIJYhDDTDbLYSgRRaCDiBTKAYDJSKTSCRDYKKccSgRTBJMcBgTMRjLQJaAYbaYJYIIJTZYQCDRBIAjJaDTcMKYZDaTQKCKiLBIcDCjihZjgESbAcKCSEBCJgRQBYaAaAKYTYTIaBKRCLbjTKJIJiJLITUgMjcECRKYBRLQJCBLYJbYQRTYYYDATQYQJbKCjTcTcCaQIMSSRaZUSZDRbDEKgSakIEbJYQCSCTMTLAbTEbCRMIDJgKLCMJkYiaMQcQZBSIUJIjRBAQaBbaiBZBIEaZQcJJbSEBjLTUbBUQYAaJZYRRITKSUSKMTEgIkLbRAUBETITMaTbCYCicQSiZcLRDUJKgQAMDTaKLYTJQJSDQLjijaESJaRjbhSKTDYbEQYMkAMQJiBChcbRAIRZbYiCCagcAIUJYYCBhBJEJLQCbkLKDDjaEJRLKIBZiQiBTcjTTURccLbQYSYbBCUEYhQQJChSgLkaiYhQLbRQiCIQTbRRkjEUbABBMcJJDRBJSJQRUkICSJUScYAYBaJAKUgCDjBaQABcjBbaJMhSgcbhUSbkJkTSaBIIYjESTSBYRbkRQRRjUCMSIhUbDYCgMJgUTLIDKaBicQTcTZjRBhaAgDLSCjRYkDEChUbCLCRihEZhSLKYYJEjgLEaZRETbUaJYSjADJATDUBcZbUbjCAQgaQALRhbbCcAcYCaLRiUJcZjbKhjEEbaLJDKShYLbkiIDDACBUIhUiiEUSJKEgSZLbIRUMigSUCkLbaSIbCCRZZMcULZiQMYBZUMRCIQgbkigScQDIQYYBRKUAiMibAIJgKEkTQKLDbbAaLaQJhQiTQTkaURgJCEhZUTjcRgJKcAZCUJcacQITZDQTkZRhLYQBkEgTKCZUUIiSZZkEgEDhYKMDYSiZKaYabSkSgKCjLbSRBKhMbUDEgDaUJIEkRaZALSYZgCRBYbhABiQJIJYkEBcbCjAMSkkAJiUKECbUjLIKBbTacIYZKQATBRIDiEgRcETjAjSMMbcDTQcLIiEYbDkICLZciCIITBkLbabjgIhSCDkRbiMKKEaZYYACZkbCBEEMTcZLSkjAjgDaIZSQZSJaTRacRaiiQEIUkEEkbMJbREbMMADAjIYDTQQbhhSiKZCAYThLCUSZJIjBaaMUKQjikJkcMEIgLCiEkjJLJLCJSjSgBMiZMTYkiDCKLKKUQEEYcTAIThShbjkhALATkbbATQEALYbAhbhEYbCbDcQkgkhjAJCgabQbJkQcSLEiMMUIbKLQALihSZUiAJaSaIhQhSEIUJRETJKIEDkkUjhTbUBhMBIkEIAUTAEZIIcRcALbZMahBARKUEEgjYYjjRIJbKAcDgSkLbbKBUUSYAIJiYBcKgYAbBcUIIZREUIUUSkjiIUIhbETJSjEZZiALDkMQgjQALKKaJZbjaYgAMkUEaRhEIBQAUjCLhMIICicEYYYTKkbKMSLhkTJACJhkiLgbQAEkhaDDABLhEaQECaAMQbMcEDUEjbLCJSbIaJACATajDacIcIUKKaRKUAJgRDakcQLYabYbRKkcJiASQYDCLYLbiiSTZDCQjJJSTkUKZRikCMJQjcABZjaBaKRSiBJDkjIbESMbCRkYDEAbJZakCITkZgKbbCYJKKTIaiQQIaQQQLaYCTDjRcjAZJUBhjRZKAUAcMkIYSQMAIQKaaKDJKTbDaCEiBhQYYiCghiDBDiJEbKQgDLDDLRjLYCcCKALUIQKAQBgJAIRAgbkLUaLZURJJRTMhYLKDYJhEELJaiigDjACZLMLQkDTRcAYKLKjZJJZYYTEBQAQaLJQJAgjhZBCIYSbBAgJBLYARKLIRTQjDJEZKDSSgJAELLIQQYLcKkIRQIRBQLAJEMZJgDQJYRLABKShIiEAYCbSbZTAKhALDiSbJSbJSJBjRDaQDLJZKSSSQKYbCIYARgAajUCIgTJiYDijQZbjTLRYAgMQYLAbBTBDJCDJERiYRDULYAIgjhCUBIiYYLDIYIULYQbLSZIUiBaEQISTDIQbLDLhBRYUIgKLZcaDEQYZIaMZgSQRMRZMJaKbYDZIBSagCbYDKgaITDKJQJURBIhiDCCQYADAaQMKbMRLLMZTCZgIgDaZbDCITYLKJQURaTDjYjkYbBaZZJDTALKAjLJMTCIikTZAQiTjYTIKjcaYRCCUQSgBYbSACBIKUAYSRLDbYECbQEZZIYSATZSYBBbiJZZgYjZYcZJLULRZaYKTLLZbbiRSaDQQaESZKgaQJIbhIbYKDCQKBCgjgKSBJSaRLgbKSLIZDKEDBbBTDKIBJTjYIYSDRIkCYYKDSZCCATJCbLIjLBbSQZRZTKBSIKYKQKaSSCZLhgLYIcZALRQiLKCCZbTTZIQajKYIBCIDURZZaCLIbIKDLCBaCIabBBATATRQQbUCLQYCAIiIRjLDLDJTTYJCBQYSZZYIAbbDiYSKbbjQKJAIKSIQBTaAKaDYBaJJKIRKDgBAJLDTZZZBaKAATbZjCQJYRiYCRKQaYAYIcJbTAZLZEMBLKLECZQZkKKYBaZkQTRQCSTSaSbLDJabDYLCEbAYATSYKDILZTBCQLjJZSYhEgSaIZIYKCQSAIZZJRRYLLQJLQDJbbJYMEDQABZSSZBKSADhQICLSSbRjABAJbBLaCiLKYYILLgTTUSLKATQTZbBADADCJKaQYCTASbCRBDYTCIbTSIDTSJAIRhaaRJKRgYAhEZYhJTABCABSbRCaBTSSjRQJKAKJCRRTTCDUQTIcAJDIRTBZhIRIRZTbMSIKYZRYRaabaRQLIEbLDRIASZBRhiAbBKRDbQKTSIaKCSCERQLZQAaaRKBIASJZZQbTATRKjhIKBTSQCSSARYabAbAUJJZJQLYZILKAARIaISAKKJSibIKKQZAbRTKQLJDDQRCZBBRTJJARITJbCDZJTADAQTABERZcCCJLTJcQYIbDgBgaJLIThaSISLTKAYAQCSQIjRKJJCKDZLRJbcBgRCJbbZAAMDTLLSZSDAIAQaTKJhbaAgKEBTDQYQTAQZRBZZbDZQJKKYjCSDLYTTDESRCSRTAZCJCAIaJIZAJIbYhJKYKDZYbSIAiRZQZLRaYkTZJZIBDIQLKLbZJRASRAaLQCIaSSgKBLMbLCSYKCZAJTaQQDYIQRZSKQaJLZKbbTUaTTJSCbTYICQBKKYITSbTLbTQYbCRLLRJRbBQIYBDJAZYSDTKQQbKUBKaCCTRJBbcAICLTbbcRCKZUbQBSABBZKRTRICZZQADUZLCILIQQSEIZLaAJDbLSIDbQCADZYiISBBaZSBBRLbKLTJDJZJYCRBCIDKBRJgiIjKCCYKRZbCLSSYBbbQDICLDRKBSQTLJSBBbAYTAYaZYLIJZRDZTSKTbZKZRILRSCLYZKTZQAZJaASQCDALCQCLJAYbRAQCRaBIJCRCZZCAIZScJSJKYZDCKCAbBLYCJRQZKCBLakSZSJRCKaLKDbCDaQKAARQSZBjRQSKRaaDCLJSjZLIgKTLJSABDcBBRTQSZYJAISBBTbTITAJYACATDTARKTRbQbETIbAZCJIQAZKAYYbbZSKADCabYbBSAYbCBabgTLLKAEDTBRBABTZRTICiIYAaKbSJbLLTSIRBQCbJZITQKCAKjBBaTDTjUjDaDZaATCQZDQTZRTRRIaALKBADaaYABKRLJRKJSJBYRZADTKDQYDZUSZYJLIYJLDASLDZSYZZaBZCABCCZgMTRSYCQIICYbYDaaIKTTTQDTCKTBQSDCLYSLZZSLZRYCaMJJbRKKCTIaKaRILajDKSaKRTIKcJRIKIRRSKICYLCBLRBITJQbLZYDZZLYISYQQiDSSCAZZABZbBJYDbQCbQbSSSYQIbTRSCSIbTJLJBLJaBYTDSYbBJZQQLbIYRbJZCJaASLZaICACKTTSSIZIRICTIIZDJCZQAbbJBZBIDYbJSSRSRATbCBbCSYICYZCQRLTKjDKKRJACJJSDCbiKBZJQQCiIbRYKBIDKCRZASAZYJATRTYbDYBZbSRbYaCJDCYhDRRLDALITBQYQLQBYSZbDAIKKCTBARRUIAKACaJAILKSZJAYDRQDLIDAIRJJAYQTIMZLBRBAJBKSJBZLSLADITZKYYYLCBCJJSThATRKQDKLTbYRLYRQQCCbYAZcbDSYCQaTTJKBIBKbBCICaTCTLaRaTSBQJAJRCYbRBbTZRaYZRBaJCSYZDaYaJBBkTSYCKJaZJLbQYYIBLQQBCYaSKAYQYBBAAbTDDLDYIaSZYIJRBLTiTRJKLTaSDSURAKTaTIcZYDYLaSYbKRDSBQRYCTTJaBITDRaIYTJYAIJYCLBKIAaIbaBTaQBCbTbRKbKZDQTJaTZLJZZSAKDQbIJaIbDTCRCARTDZCSRChCZRCKIKAKaSSJYSJaaIBTSABDKBKLLQRKRJJJRbSSJLSbTAZZZZCDhBQSaYDZJQBTbQCaCYaBAZTQBZLaBKaRZacKDDaQbLZSZIZJaJIZBaTJYAYSSRCCDIIKBIYaCLSRDBYAZAAaYQSRLCIRbRJbTQIIIZJRBUQaSDCZbJbBYZQDCKYQTQQaaIBCTRBLJTKIJATBJLaQCCIZQJaZBaQTLBAAKKbZaQYTABAYYKTRBTaJLUbCCKDIDBZaBIbYIAYRTYRaCLRAIZIAZbJCABQQKLIbBRYSDZKJbYQZaSKKTDCKKYSbKBTDBYKLSaADaJDRKaRJjBQBALDIbJABLRIICBDBaSKSQBZaBYTRbCZKRBKYIKaQYRSCLhcDCBBLBIiABQaLJRbZSIYLRIQLKSKYZLRBCKLICYZIDQBJBaRQAJYTQDQBKJJYKYAbCIBTDIBDDbLbIbSQRYaQDILJbCIRYaZaLZJRSLRDbYSADaQCQCDSaBabKLSRCRbITBAYLTCICZQBEQKKJCaZIaIaaJAATBRTRKAJaKcQLQZIKLISIYRTZTKbYTLSLCLbIaULACBTCBACIAKLTALYLDIaYABUZaKbCEBYKZZaLDZBIQTIRQSCAYSJBLCLRDSLCIZSJBbbQQTKATRKCYZCCYZSIiQCDJaCRRRDZZYARIJTQLIZaCBLCAZTJbLQBRTLRTShTADLDaIJQBQDJaIaIYRRRBBRSZRBAIaaSSLMAJSSJcQTLLIZgJSBKBbAaDTDCALKBDBAaQJDJKLKASQDSQKQAASbKLJIBASaKQaLJBaJJQIKJKbQbaTILKSSIbJDLSKYQbTcTjLaQLQLSZbaQYaTIAJbKQZKRIaCZTaSYYQTYRDBALDKSRKRbcRKSLQBIASAQbCJCZLTTBBRJRTQKZLTBZTBBQJIbbaIabSCZTJACCDDRbCAKYBZMISZLBaCRCILDIaBAIKSYIALQCCIDTIQRLTRZLJaaIQDKLTCBQTDCLbYAZAZKTLZJSZaLYAaLDACQYYbAYaAJZRQBCZTBTSaKZDBTLQJLTLQKaDgIYBAAZRZYBYKRQZRRbADaQJLYBCCYSILSDRbCADKbLTcDLJaKKbSRKbQBAYbbQJDCaRRQZDRRSIBCZICZDJaSIbIZYZaSKIYBBaQJbRaRZASAJRQbDIKYDZjJDYQRCaAYCLZaISYIBKSQAAJJDDbBBKCQbRCICZSTaRabYLBBIADTEZYSJKTLbTYTKQKYJabCIDBALJLSKjARBYTSDTQCYZSJbJLTKBKSBQKDRLBbKTJKYAKTJTBRaITQECIBALbQZbICaQYDCYRRRKbCaBLRCAbaSbTDIRIBBaTJbZBAQbbTYCTCSSKJIJKaJSDBLDbAKQQDKBSJIaaKBbQDbbRAZTKAJCJJBDIaLBBYATASIBTKbQJSLbZTLRCDZCRCDKDRBbJbYIJTLQCSbLaSMSLQTKQQaIZLASYbKZBTBYSAQRCJJaQTIQajTQScIZRISYKYIaLjJYbKQSDRKMBLBAKIIbSYDDLbKaZaDTIJRADDJCKKRAZYLBJYSQRASKQLBSKSBLaJTIYJZQTJKQYTKbRRDCSSALICTBSCJBYJJYCLIQJaTJDDbYSZLbYAKCDBCKQLJYaJCJJIaCKBIaDBARJSARALJLALbABDaISZSDYDDLTBIaZbBZSZDSIZTQBSBQQKDYTTBCISRZYRCAaZIYJKLLQRaLIZTCSYJDDIRIRRBQYSbbRCAKCLYSCBJJJaBJaALKJDDAbQKDKQAKJZSDbDIDLaCTTQSZJTABSYDJALAbaSRaIAICYAbYSJQJTAQBbBbBASbKKQKLIJKZZYSYRQQZTLDABIAaRRAJDLYBZKZASZLLYALBDADKRaQAALKaYRbDJRSZQSICbJKaDZCbTYhTbCTACLDKSJAaAbSKYBQLZajIbBabISYDLDaDSLSSAKSCSDBAJLbACDSJbQTYLQLILTbCaLIajbDSTRabAKCQZaCTKbQaRCLSDLYZaJAbRQRIDARRABQQILIRRLJBBLBDbYQaAKCBKZJBBKSAbbLDKaIDYaCKRDabCAAAAIRATJbaDRBSUBJRLKDBTRYJJSZaRZISQAAbTCLZYTDJLSKYTBITbIaABQaLIYbJISTYKLaDRLTCKCBLCAYCARARYSDCSSLLRYKDAbIRSLCbbTLBKKLTIYZRSQCTTDbYQYLJKLLITDTCAbSDICZTRQKaZBaDDKIDJQBDTIZRKTDSILQZJAZDTYLIAbRaYYIBSJIJCEZIJYKSbZJRQRQRZKQJAIbIKRSCYJBCLYIYRLRbCCKIZLTZKIQIRLTASTRICBbZBKSSZDCDLAAKRQbLISAADYRYDSbJbAYDKaYBSLKbKZCCKaIJBJKTaYRZaKSLQSYQDaRCLZLbDZSIISQCbZLaZLCZQQCAQRKSJJZBILaRITJZCZQaBQASSRYZIASSTTZKIDAbZDQLRbYBRTDbBAZZKSBLYTQCaYJRTSLJTLQLTbTZSSCTBRTTCRCTLaBZbQaQaBYAKaYSABILJQKCbKYLZILQLQCZZaKKJCSQBDabIIABZYSUSaYLbKTALICBIYBAJDIbIRTJZBgCaZTQAJRJRTILKBAQSBKaCQJRaJZRKQSZEDSLTDIDbaDaDKZQRbLbKIbQLRCDQKaCBZaAZBDaLSRILTIRZDYIBBQbAaJCKDBbJSDSSKIZRABSKSABAKZBKTbDABSRJKZJaJDARbaRTJaKcbaQACTDQCALQBLKCASKILDYZKQCaSYbZTaKQDCLTTaKDJTSDYTabBCTaDLYLKDZaKKCJYKJCYDIaBSJBCBYQYYaCDDZIYCSRQLTYRIbBbABYJLQJSbbICZDYYIYCB"
"QTTRbBIZCTCKCKbKBaTZSYJJaKKASBQYTRYKJARaZQLQQRCBCSLKaaZYQSZKSbbJTTTZKIJbBDTaLBLAALCKbDTRTaZDAKBQIQBYRJARCaRJSDLRRLCaIJKRAIaIaZSRAIADbBaQCYBbbTYSSKJIAZRSIYBIQIRJbLCTKbQbLAAJYQDUZCYLDDaRJBLLTTCIaBTSAZaSBZZDBJSADDQKDRADDaTBLbIYJZIZAbYCZSYDDJYRZSTCSKQTDCSLRDSaTTYBLCKIBKRAIBTTDLCJBQbQaLAZIaTZKQAIaTDAbbSLCSLIKbaYJQTTDQCbBBALQIKKKILaLQIbCYRaZSQRbJCBSCZLJAQICLSCCbZAiYAAbbZaCDYaYYSRKRTLbJTLBBJZJQLCASTASCBQbCZCabCaZSDBBSLSKIQQJZSYLaQLJabKaJDYLaCSYTKDQZQAbBDLbTTZYQZRYCKAZYDTCYRLCLKBRYQILbIDLTRQaZSTJLLRYabaASTICRIbZIAAQCBJDJZQaBKCCRYJTSQaaZKYQTCaZIIRZYIbRYRISYCADDKKTKBBbSaYLSYaDYaDTTJDAbQIQQSRaDTTZIaYaLbRSRbBJQRDDDYITJKJaJaaQCRQKJYYCCQaKILIYDDSRTIBaJCRBIKIJKJAAAJJRTbYIQRRKAZJIDZTaaQBSbDRILRLSBACAZAKRaLKJYIRJTIbBRSaIDLDKACbBBbTTbBLJaLALJaQCQYZJRIABKYZDYKIAaaTQSKTaQRLSSTDQSRLLIYQYBTCTLbKaBABbDDQDDKDZCYLRDQTDYLSBRaabBAJCRLRAZQZbQRKAQQYRTBKBbYZSYQZZDBZCARIJaZaZaYAAAZBQbTDAbKRZIKJbCKJbKKRDRKSIbYIKRITZCbAYQQBYQCCQQbKTLTJKJKaAYIBLQCbAAbaLLCZJaZKBRbKKJCSICSJbRaAaKYJLQARbBRgbAYBYYKbAQYLaZLRBZSIDRLCTaSLBALQaZIKLZLbSIQQbJCDTQYJKbaJaRTbABQIDSIDTKSILQTaBRBCLKbCAbSBIKASRTKIYARQbKbZTaKTKJKJLACBJKYDBIDSDJKRCBBLDLJZTLaCCSRAZAYDDAQRbYQSDLRBBJDIYJQbBIZCaKDRKTRSZSRaZZZLSBZTbTCaBAJSbQSRSZZQSCaBbBASZQbDbZKSKaABDYIDSYJSaAATZRbIZDIRJaYAIaCRJASQTZSCSZKLTBLaJZDIKRZJIKCSLLKYaZTBSDRBKITaYQKJKJTbJJZALADSCZhIQaKLIRCJRSKSIAAIDQaBCKTRJLQTYQTDTJYZDQRLJRRTSKSJZDSDYTDCTYTYRLTaLbKaaBAaYSCYDaLSSRTQZJRKTaACZARBALBJDBRbADKbZRARZDDTYbCYbbDTTaIKKAIaLKYYaQBBLALRBAIKQYDCIDKRKASDLDJRaSQQADITDaRCaDLIQKJLBaTIZbZKJDbCBbaQSKSQbaJSSYZTabLYaBRYYQZLKJKCbaBRCSDaBALKDaSKZZATCCCJJRAaQLYaJZQACaLYKACLaTZCDZaDITKRADJSRBaLQCDLZZKIBTabJTYaDTLLSISJRaAASZLKDSYLSDRIDTCRYKBBBBKKbYBQKSKLIaLbBZDKJJbaYBTZZCDDRSJDSZbAZCQCaCSYRACRARYRLQQBZDDQBKQCRZKSRJBRKBDTZCLZRLbCZaDLCKZCBYIYSAIAIDZISbJBRSLDQAYJRLAAKKATSBZaBSLCZJZTAKTYAbYDaDCDRBTJQJLQaADSBYZBRRCQTDKDaLILILKSLAaKaIJLLTARKYbbICTBYTYRYLCaCJSSITTQJBLDYBKBLYbIIBADaKSbYJZbBYABRKQYALLCRILITAZIIYSaQCTLDRTLIQYCbTKaQZSBJCCYSCKaQRDYDYCbKSBZIZASbbDRJYBDDKSKSIZSLLbLIaAIAAJbaSKaSLTQIACQJKYLLLKaaLLBBKabRKRKQCDZIBKSKKKIbIQJJaRLJBSQbZJRJBbARBTTATQATCRRAYJCKCJYSALBRCBCLZZKLBaJJRLbYKYLCYTAAYSKALSYDKSAAYbLIZJKDKBBAabRLDJTTZYbAAaaIbZLBbCCCSaBLKIAKRIaIQIYYSLARQaICTRRbQgCIBYKKYIDJbZKZKLRSBCRJabBZABZTCSZIJCSYaALLKCCDZSKCJDQRLbaTQYLRQJBZZQYbBbAQLQTQIIABDKbJbZBRLCZQZJIIbBBTBbTAZTJKZBLQDCAQSKBKAaTYaYJIZQIYATTZaBSSYADYbZAYRCYQIZRaSaJLSSYBLbYIRBSZKRRIAKZCBQRRRBTBLZZDbKQJYCCRBTaIDZSIaLDRJKbaSSZATJbZCLISKLATRBAAZaDRaTQJTSSBZJZSDRbKYBALRDSIAYZLDSQYTICSYDQaJSZQBQJASCaAJJBaKLLTRLbJZJDRRaZSBADSbYaSBQTAZIAabaAbDIJQJCaZSRCZTDJBQKJKTJKbCZBDSaKISAKJYYLZBbSBKbSQKYYaCKbJRaYIKSAbBaBRAYBIYIDaBDYJKaIKBBTaQKLAKjBDaLKQYDLaBDKbDSBQCLZARCLIBSZAKKLKKRaBLASbRJJIISBYRDRaSJLCbYSbRKYZQTQLSSIYSaTaaYJCbJZSAYZAIYAZLLDLADAZQRZaIJBQITAYCLBZZQbTDJBaQSBTTRKDBLQCQaRaQJBTbYTZSDIbBLJCSSRSARabJaDIbYKIIBbKIZCTZCYSDIJBYbYCYJDTBATLIKJLCIISLZZCQSDDKCAbRBLTTbYbabJQKAAKIQbDIZAYbaKRZKBARCRCJYLAIKLZaLADZYTAQRRaAaJYQaJASZZCZRRADDJSBLaAJCaZZACSbAZYLIJDYSRBAbbSLALIbSQTJYSYSIZTDDLLLLKCLAQLIAbCZLKQSBQSABICbIaBKRJLDYbQATLabTKBZRIBRDCAIZRLbALARLaQZQLDJQTSBaQRLQaQCbaJTYaBSbbAKYaRSZJKLAITSYQZbSYATBLYbJCBDBQKbYASTDBSJaRLRQKIQQSJZRQZaYYTaRBTAISRKKKBYbQRbDZDBYDKALSTZYAQJYSDTaSTBaITDQBRZLKYYRLJJYDDYZDQARYSJDaLLaZJQCQDZBTTCZDYYCDBIABAZCCITTBZYYLCZDCKDLAbBRZSCDCBAAbYbbQDAKIBIASJBTTZJRJDDTJTQDJJRCJSDLQTJLAJTQQKaSKQBRCRSDZKQCZbJZIbbDAITYKZYaYATZCJDRLSbCaYTIKBTJYITTJBSILKTJIZKCCQDRbICQDIbSQCILBQKaDRZRIACCLRRSaJIIRIJaYaAaJLRTBADJBCbIYIbJYQILBITATBBRABKBRZYSAaDYDZQLbZQJTKITBRQSLCJRDAQJJAJAYYaCDAabIBYBLISQJKASBTAIbRSBZaKJICAYKLIbKRBZLLbISTZAIBZZAZKLCRSKYQIAYTRbJYQIKTBTATCCCIAJK"},
Embed{93,297,"QIIITRIIIIIIRIIJIIARYALDIIAAILAJbLZIIKaYIYILIQQTQDTTTTRTTTTTAQISTJDTTTLDRYDIbTTbTLTTRQDITTDTTITAaYDKBaBITLTaDJRYDITBbQSaTbLRKaBCZITaBCQBbaIQbCJYBILbAZSSTJITRbKQBbKATDbCDCAAJLaRIaKDCYIbSJKBTIYJCTbZAbYJKZYaQDCKCBCRIQbIbZTILSJABSYKQRDaARRZSYAKCZKABTbQJKBaRABLSJZTKIbARRTBYLKIQTaBLBaKIRTBYBTJJbJAaKRaRLSQTIYRKZDALYRARCDbCILIIYCDIKRaCILbLRZaAQJaTSDQJZSIDYTaJQQTRIQKCBaKTTZACZbBYYTKKDCCRaRCIYQRDLCaIQJbCCIDSZDbZJRTRYDJSKCKJDTLKZCZDTLTYKbAASZZALSCYaZISKZaBaLYIRbAISQbbDAKSDLQLYTBSTSbYRaZALABDaQZLRIRZCIIaLZQCAbCZLTAQLaCbJaKIASTIDZRIICBARLbYDLACIAJAZaILYQRLaTaSCQYLbbCDDZRIRDRQKDLTSbIJAIJSbTTIACRaQKDaTDDDAbYabLBCZIKYQSBJCIQLZKLSTICTCBILCSDZKRCZJaTQTJYTDbQBbKYKDRLIKKITaRTYaQAaZRRIRRJSaRLCbDZARDKaCARKQbDZSJSJSJKAZDCDaBbCIYQSbaTaaSBLBBTKZIBAJYZQaQZJDDDYZaabSBaCLBTDDDRYCYIaSaQILLJDKARDZRKYYbABaaaRIQBDbYIaTIIKTYTRTDYBZJQIKBCDSJSRQTbCIZJQaRRbSbQDYYKJaKLTAQTJIDSCZLCbSRDIZITQTILBYTLTASSaLQSZCJTQbSTBaBDTaYBLCDBLKLSbQRBTRDBSLBaTRTabSSJTYIaBIYTYIRIIRRSbDIBLKCSRDYCATRZLLIBSBBbJSRJKIASKYbSRJABKRAJDaCaYIQBaZCATRJLLQLIDSAQYTDLLTCYSBTJKJZbIADIBTbbCBZJaQTBBaYbKQTJAJDYQQIRaaJISAIIaTQDLICbCZRJbRIaRCSaaZILBIaZTTJKKBTJADKYTRCAYTACBDDYKYKaRDYbAZBDRSJABaZQZRRKSRCBCbQBJQZQBILaLIbLDKAKYSKCbLDJIaKTIYbBQbDYQaLZZDTRBZBaJYJSRYSLQAbQCKJDZSSSaBJTLBQbYKbQKAKLLRaQDLZZAQSSJYYTQTTQIYRDBDLSSTATZSJTKRYaBLBDJbSAQZSJLQCZLTRBALRRTRYYQRabIYAZJKLYQTRYBKRDLQIKJCSDaRSJZYAbIIRRJIZSASBLaTJQbQYIaBYCJaAabbKYYQLSLKRIZTTAADJKAJYRBSJJADCACLTATQTKKRBSKbILaSZTABKCTZDTACZQbCZYLLIBAJSbbAZYACTZQDCKTTaKSRaJJKbSIbQIAAbDTZYSZRIYCKQJBRSBbYCDRbCJYabQRDLBJQSZJSIJIRAITYJIDCTLSYQDKCZSZaCBLIZDCYbBSbSYATJLCTTJDRTZTRQaaTLaKSRaKYCYYJAIDaLTLQaIDQAKITLRaQaIbZbQILaYZBYaJQCJCQbQJUYCJZDARDaYTJJSCbIQBTCDbaLLZQbCJAKTLCZDQAIYDZQAALaZQKQBYDDKBZaKYbYYbLQSCaLISJSSTCJKYBaKZaQCRZLCZLaTCRSYJRaJRKDCAZJYRaTLRLZCBYRRARBCKKBJQLTJTaCLKQAaABDaSTTZSKSLAULTYDQYAARKaaSBZJBDABJBILbaLATJJAJICDQJCZIaAJQBLZIRTLTYIYTSJLaCBSDDaJARJBDSTTbSZaQYRKTYJAQCABBBbaLbTDQLCARULTbSTYSCLKZBLCLaJAQQJDAIBLZAAISDZjAbSTDbDZKITbYZIAaZaSTITJTTRCIRZaRYSBLDLIZZISAQZDZYCKBCTCYbRBSSYLTKBQbaSJYQKaCRJLDQZLZAiBQISYIZDRZQaBSKRTCTCbSJbZIYBbBLYYaQDSQELIRIABZLYbRKQYaRZYCKbIbAaKATLaAZDRAZLCKaRLJESDbAIZICiaCBTRZDCKAYAKSJJbRKLbAbCaaLRSZRQSYQJaKKKgCTKRJBbJbTICLLCLSCZCLZSTAQbSaJATLRDBMASZRJEZBTTQKZSQUCCRCJZIKiJQQYIDaLLRJDJICKQTRIAYETYIDLaQaKQbBZADZKDYJKIbBYRSKYaQSibDKSBJTBKTBYYbAYbSTDbLCBbTSLBAZRKKRTSYJJbLIAUSCTSIRBQSLCJAJZKRYBCIIbJAJZaYaDERBZDTKRBAbZiJDYSTTSRDRQhDSASKBYbRDBbCRYDTaCLICJJZDAZBTkQDSQYTADZLRUIbSbAIYMCBBZaQJYLbQgRKTBIDMSCabRJSSYKTRZZYJSKJaaJjCQZCRLYSgaTRKBDKTQCbLhRbabbAYYTBYCcCIRTRJDKKAbcCbgDBQhAKLIJYQCLTLSKSiLCJAIBBSLabLbRYYBJDQBDZIbUDJTJRBCBgKTJTZRIKYRAZLMYLLhQabASMaCgLSCUbARYDKSBTSDabaKKJKaCADRaAIZBKEJCQJUIBEZZBSIDQabhJgQaYQRBgLYTLTIIITCYBaJKRiIDIQCKUgLLLRgIcYRLhJYYRbcaQABTSLSSIKDKIAbkDaYKRTTASZAaQaaLgRjaTIBjKIgDRiZaZbSDRIBaDjaJgTKJQIAaJibIMIQSTBcTUYbEJTZQQAMTJJcZQTDQaKaSMRDDgKCDbQLaYkKKQLIAiJMgLICiLLUAZTDhZZRSYSZCYDZAhbQSYbZTaYBUTJTDabISJJDEcCjIagCKZCKTMLQRECYCSJQbbBjLiAaQcIDABUDcgZbLRRQLQIgkKQbAaThkSLIcAYLKIkZTDTBBKQbRiajCEgCQYaCKJTaRLRgZBZcLDiUjKCcEKZbZZQMJSJRJLbUZaASShbbECBSaSaIAcBZjhUiDUAhCgaSEIZQMSIZhUDBKSaEjbCRjCYIUhkCKJabKJbbiISAICbDBCZAKJaQRAgLYYYBDMbKDTCahTCMiLBCiaijSkLbLZcYBQMgiTBbSJkaDSIhbIBCYRSScJKDjbagMhgMUAjSYhDCcJQMJQaKBCiJLgBKiZAbLAJhDJaaTUJUESacUcYaBCgMYTaAjKAcQIJCJAgYSQQTYkcSbiIKMKJcibZBiJCRgDRBbjICTBIgEYTQILCZbJDDAJcSRCLijQLcMLAUhITUSMDBikJciBDbJDYMASaYYbMIaQTUQkJcAhMbSJhIBaUZiSaTERbYcRRRMcQjDIUJahAADSYahhccLbETYJBZTDDKJZiEiSTMCgIYQDTEZhAAiKDZTJbbSCBiBSTEACEAiLBaKAcLIYKLLEkUJYMaAcCjkMcBUScZQLEAabaQQYEKUJALUZhgKjaMibbULSaLkagSKcgUYERRCMZYLYMQaMAZUITScMZaRBaRcYakEMMjgRSZiRRCUUMSkcKQTILLDiAUATLMQDKSJZCKUSQJERREYkJbicBjUkULRAgRDYRDDIKMJcSEUkgZKERcZIiaaCKbRQIZBLiCkEIEDabbaIgSYAIaZiSjREiRCSJMQQaTcQZIEbJacBSbjMhYkIhbjYkDjJURKaYTjQLkQAIZUBQYEDBgLIgLRCELMEIhJTYCThgZIAkiAILLUkaMaBSKQgJRgDKAIkEUKIThUiQcCYDRJRRBRMjgQhkaDgAYZYLYiALJCAcSIJAgjjDSDaQRgJkicYgbjkjDUZUYkQLjRQRBZLacAMbRaIiKEiTDaJRTcgkhAYYgaaKhUSMYMYhMIbYQIRLUCcDLjRUMYcERTDgjjMhaRUYagZCJCihIMihZAgCSKMZJJUQJTTRZkEBAciLAULJKIgZUDMAIjgaEBSkTbibLjcBKIBhQaQQabZgJDAbBhbKBRZUMgSTCEZhZkhcBQcCkERjJcJkKKCRJLCATjggkMhcASKLJQEZLSUUaRLIbjAjjUEgiIEEAhBUDKbQKCRIbgCIKCiIgEkiUDMDjcLLQaLhREKcgIMbhSLQEaCIJZhREScBTaEYMjYEMjMCSBRKThEcDBAIgEkgiIYDJJTBSACQAKcEbjckUYEcSScCkBiQUiCkYCZjZSZkbEZbDSCjEcEhcDbMiJDYCCiKJLBBIRAMJMikCQDiZbUQRjURccikETZBIiAJaSTYhhZiMkgEaQIjCScjZJILccDUQACYEYKkkiEYTZZBJASbRJQgkUAIhIQgkbMJUCkKMaLLaccaBDIkEkRRABDIAYJTQLLYgaBIQgaRiRbhUjLbALEASTYCUgDQkZhMbjLjCSbZIYBJRCAZRYBUDigLMShBMcUJiZjIJRAaaUcUJKAjEAhMUSBJDZKhLYTCKRIgYDIEKQCTjAKDRJSLhIBMbaSkRhIjYLDIbbLLCRADYSKbIbkEIYhMLYKcKKShTZQMkgALTCKgAgLCJbJDEBDBMYQZZDTJCQKaJkikKKTUiagYIDYBjbLhJLQCRbKCQTSCEDDKUBBAJjChaDSZiQRQDKUEZjDILIKDYcZZaMMASQQTRjCSjBZRYJLkJIahjaEKYcbYISTZQjYkCBaZTJSgiQScSSBKZKKIAYhTETZYZSSbCKaTIJDiRJjKZTaJbRhKQLLQCBQSQQBETgIDTYjbBbTRJCCbDAEaciDESJaZZjCLaaRMMCbiSRRgchTIUTCbBaRYbQiCgCLCEQISLJTTQAKRjBSLSbJcbaBJYaTZgLBRaRTCSJKRJTMSDDYaBQgRbKSCETTbBDASKgLaLQSYLDhZQRcQcDBYDLhcIBQiBJBRSLCKEUBagBbTMRAQZbaSIRbCATBLbaCIThaZMLcbRSBRYaQBDhQaRKAKYRCAYRgJbTKQQJQZSJSIaRDBQRDbYRQMbMcLIaYYaCagACDSZJjcTKLBTJYCaCLSabkBQgZRakRDJbYLBcZaYQYgKTbIESRQJLKZBCZAJIbUaDIZbhSTTMSETSZBYSgCSIQaBIIKLbiEQIDChRDbJUZAcLbabLbQZDgbJDCYMYQCUgLYBbJAAhDaiDABSJEZCKBASAKSCChQMBbZgBTQbSLJZJcbTQYBJIDCSLBhJAgLELAQMDJDCTCTRAbSDMbEBRgDQYYZBRTJLBaSTBjTaiJLDDDaCBJaaabKZhaRBKCIQZKaCYZbBJaUAZALASaTbUDYBCCjIKBMKaQDSjcYhTaZLAjLLgZLKKchBMYEQDYDYIaCDDDSacYcKAQLAaYYYAiSBDaDDBDZMRRYBIcTYCDBgcIJaZKgMBCBaTJBTRYCLSSTDSbZJKaCJZDRBBbLTaRTLQbBIYaabDQQRSCcIAIJagQIYSCTUcZDKUaMbabBIQAZILTZQJbZIMQCTSCThSQBLJACIBKSCIZJDCDUDDTSQITZTDTJBRKYSBYAZhAIaTJAbbEiIYJaQELRUISbCDAUSbACZJDURIYDKQULCJaTKIYZIbQJajZMTJQCSJkIEKJSQTJBabRBbBQDTRCIKJBRaTRTYbcAABKZKQLLcLKDISThLZQbQDcQIYCaLbYBACaRYTLCaSUJbZajRYRAJRIZKbJSKcRZaALaTcciQRbDRQADLRACJRBAIicAZaAZDJRQaLQARYCIbKCbKZIJRCLLAEJQSSDaDJQSBYRLRSYSjSIRLZJLaCLBgThbIBbRZISbjQQTDAKZaYRQSMJSJhDIADISJaQZJKBARScCKUBKKAADQbJRRYZRLbaMKCYKDBDTBJTBhCAILTATajBhJLDQbKYbDgAZYYJLRBCaIAQBTTEDBCgYRZJRDaLIBKKAAAbBSTaSaATYbLRBACYCBJZbIabLATLCJaLDTIYIECLAaKYDBaRDZCAZTQBLLIJTSAQARABKQbLZDSARCCRSTCJLZbTQIIRJDISJbacaSBJSAaASDhAbISbDDJRRUjRZTZRahJCYSaBScJaCIaJSIZKaBCTaTaSDDaSEDYDbcCTLRZLQJLTKaCTjSSQLUaLZYQiIBDcYiJAJBRKLAbTLZcTBJSKMDJCTTCJAISZIAUBYJKBYJaYBITJALRKSILSSIBTKaLJYIJBcLRQTIALQCJCAIJKaBJJLJQbbSQQQZTChDBTSbAYSAKSRSRaSCIJZYRKIKSDQbAgLTLCJZIZBCbbRBYIJKSIARAYLRKQJbUZRaYBADACIQZSQYZSbDDbSKDBASSJSSSJLZLaKTADbgQaZRZZLIQbTIYYbLDDDTZQAbZaUaaDSBEgABBZAaIYSABaRRDDYILCLZQbQbCCZbLSQJCLLYSiIEARQbaZKaEBIKBITQTbSbDZDLbLDIARQBLQYILKJDBIBIZCLABbQSCbDRSDAQAKLhbBLbDBYKTQCbSLaYMIYLJMSYAZRCASJbISTICAQDDbLAJDITAbBDICAYAIZBQBaQBRaZbSYCASDDBZbcQaYbKZRIBTBLIBBCCZTTKLTYBKJRYLRKYbaKQYBaBQLQDLCQJZCJZTAAJKBQiRKgJMCIUKCDTJRIaTBJIiCBRACKbBQQLMKgAJAYIaALRYJDITiLZJaRKQJZZLSDZISBLICSBaYKCKKJTSKCIYIYLKbJICbTQIEIIcACSTbZTKRMQRaZSIbCLbSCAaQZAZYLCAAKLBTSDIJbaQTBYabSYCKSLDDDBRbLDbCSBaQMKYBYTDDCYULSLBJILISaSTTQICRKLYbKRbYIMSLSYYAYALYRRRQYJTAbDLLLQACJBKAJYaCLJbLJQRUbaSDAZAUALaLRARARZaIIBBbZIDYDbDYCITRCTbYYDYJLKbCQbSTIBDJUBBTIRKZYKTAbQRSYSbDIYKZSYSaQaYISJDSbTEAKbATTSSIaDTYRRDYYaDJCBYQSBgAZSZBJZCbbDASBbCSRCACYIaKIZJJMLTQYJLIBYbDaLRCYbLYLDKBCRYTaDJRZbYKZQBIRJKDTQJTIIKSSYRCRRaAQMMAKCYJQbTLKYAITARCJTAYQACBDJjBIaTCZbYYDSAQLZJRLSZhARZSZDTJLYYBaBLCMRYIDISTRQQDKRIRYSITbZTRJRAaRbRQLBbKJQACCBIRZZTIRKJKaaERZQaZCQIQgLjLAaDSBbDIYEDDBJbDaSDSQTRSTSDYDJTRQaSSTBIRjABIDYbJTICBZZRLJbbCQJAbTAYBKRBaZBZTZDYJLTbIbJLARQCZJSDbKSACSbSRSCCbADTABALZBZZAYQJLLIaYYZISCIZCaCLZDSBAZCILCaSYBJQabJYjSARbIRRDDQKBJYZTCBTJaKRJjJIBQYQCcbaABYAKaZTISTLJjRQQbaTRaRSQTYRYTKaZLJRBQLQLBLRCQZaTRIDDBABIJbbBZAACSbTJTaIaAbLAATDCSDACRCQAaJbAJJICKbZRJaTUKQSZBKBDKaCaADIIZKZIjSKQRbBJYCCbITJLIRbbTTgKQBKBbSADbQSLZahYSRYKYTbKQBTALCbbagIBTIJZASDKAaIEARYISCTJJLCZbDCZZTJQLYbbTLSBSKTKbaZCIaAABCCBIITLAJZbQKCbSbISZQBAaSbRMKTbbMTBJABKaQDQQQYKBaBaKILYBYQYQYTaZbZBbJCAZBbCISKDSTKKSRRCabRQZKQATRCKaQKLBbADABYTbKCYQbKADRILYbAZTDZZZZbICSQbBZQACYSQaBBKRhKSBJAZYUDLiRCKRTZRgDQTBAbZAJBRAJJZaZQLTaIaJRRJaLEbCYAKQaKKaJSIQCLaDKAJTZCCQZIYBZLJQIaZbaAJaLSSYASRBKKTKKYZJLDLIbCaaCJATQDaBIKITYRQbZDCCKZIBKbLbYLYDRRSSCCDbLACTZYSCDKRYCLbQJIbQZSaKZDYbYTbJAZDCaYDCAIAYZISDYJbLBSQIZYYJYQJTADAASJLYDIRSTJAQIAAIBBDYKCSLARZZZShZTjDRBaCLZBKRJLRIbZALLZACZBAZQZjSQQZICJDLQYaDDRCBTQJbZISACYKKQCYYJLLJDbQZbRADDSSSRKYAJAIbSKTAZTZSKbZZYYLCIAJCbBbEISRLDTBRJKYCYSCKBZQYZZJbAIQSTCIJaBbBSYRZRbJTYabaKCYILZQSbaTTQDaLTaSLbRAbaQARDTIZSCbRCBBSYRIZBTJDJJYKIAQJJDRTATKQbKDCDBCARQQaJZTAaaaKAJLYCaJADTJQKLBRCTaCZDbaIaYaAYDDDAaTCJJTLSDLZJQTASaQCTCKQYAbKZaJIDRaYCQRCZTbAJRJYQQRabbYCTLIZKhDLRQKLRDaDQCKQATRLZbIZJTZQaQRTBKASBaaZLbLQKAJKDYSJJJCRBDDYRKTYKRQaCbILZBTZJBiAbQBDAYQQYbaIKZCAaZBYbZIiRIRACBQIKJbJBAJJCbBCIZSCZSCSRbRaLTJYIAhabIATABYBQZBBCaYbBaLKjSDZDQKYIQLDLbRKQBQYbKCYSTbRIQZIQSSYbTSZIATYKYQDQLJJLZJARLAKRbZLCBZAQKQCASaYQaZBAJRAKLQZAQRLYIBKbCYBLKbLIIBTLDDRKaAYITILIJKJaQCICYYaBQRZLCDTICKKIAYaARQRKLQRCDZDTbILaLYYSTQbCZZaAJARQQKAIbJbZBZRJIKJIITKaQZDQDKYQKSDCICaZYQJDCKLCaSAYRRDYabLSCBKYJYbBCDLZKYBRZRDZQIDCYIIBSZIKKbISDBJSBRSZQKBabKIIDbKTCJJZYBQYbbabIITJQKBKZYALLJDBKDLaTZQaKASJbTDDCYQaRSLSBSQaCCJTRDRRJADRbZaDKJTDJILKDAASDLKJaBTbAAYYYaSKbSCQSJSRBLQILaITTDbCRBCIILKRRYTDKJaRbQBTIbBQSYRRIbIQLaZKIYSBZCBCJaBDDbQZjDSJJATACZYKaIQZSKDACLCaBCZDSbaDIYTbSCKLACRbTJALITKaQBbDIQTZILAaQQAbRLKKBbRJATaZYCKJJbCZRIAJBbAQLbaLDTYZQCCQCTBATaaLCKCIKTIACTICbZABQIbZZbAJCRTJYABaaZADCICYJLDDZITSABbaQRICIbbLRKATaYBIBLQTITZZDaSKQBKRJCDYTQBDIYTTCIaQABCKCbYSaZYaZCQLDDBYLBTIJaYYLaDBRCRKYbIbRSQbJDLBbTAASCKKLbbaZARRITSbADJZJUZKaSSAbjRaAILAAKIQSYJKbKBBCQJDYKDZCRTJLAbSDaSYLJJTQLaLQSTCaLCLJDICDbTBDYQKbaCaDBDTRJIAJaYBRCQYBJLAaRIYQZSYBaYISATbRaRaJbKZKISajTKJLSbRSDCQBDIaIIIBQLLLTCbBLYYDCIJLJaYTJCQDKARTCTZBRTITBbDSJRBCbAYaRZCACRbbKBTCCBDKTATSaLDDTADQJIIaYTDISIZIDYKCZIZYCZYBaaYbBBaABCRCARKaLRaJDKAAaALZCaZQLDBTCBJCBAJLKIBKCAYbCJLCaTCbBaQAaALTLQSBJQABLLbLIIBAITJLLDRYYIZTIaBCJBQKYZTKRLTLDbTIRYLbJARKRIJSKDZAaTBSRAABZBCZYbZLRCSQbZJQLBSBKSRLaYZBCBCDSYbIBbKJRTCCDLaYCaJKYLYbCJCIDaLCSIbTATDYBLQIbDQQDbARAZCDARabBQBAZZLQRIJJLIYDJTJBYDADYLBLaILQAQIZQCAIIRLBKSTaBbSbbRQCSSBRaBLCADKDbKDZJAYYYLQQKJSQDLYYDKRZZRYADBLJaJQCQDaAKBITRTBaYSaRLDRKabCJZAZDQYRKJYKAYaYTQDaLRKabaCJCYQDaJJBYBKaDSRACSRKaTYbAJDAbRDAQZYSACjLJAJTaYQbAbbTBJBSBAaLaRKCTJQRKICTBLCIZaASKbTAQaLBLLLRZBaCABQJQRbISRQLIabQRTJBIabRKYDZSJR"
"KLCICabDLLJQBISDILDSIRbTTKCLTbSDJaIKCCbQAQZCQZRaBDCZKQAJJQTAJAZYQTBZBLKKZKBSZIJAYaSLSSTKBSLDZJIJSJQSKJLbJSAQKADKATSQCRRIIKLSCBZJQITJRTKDRaSRbYQBTSYaKRBaIQLRLQaRBQZLbIJCRJCDDDAaQaCYCbLbJTJADDSLZLaYBDKSCJJTQaDQKBZRbJYRSaaAIKTbSQaBLRILQbILQARYCBbCYaRCIIRZJRbCRQBQJSTDTZISQILKBJLDTKRDaAbYBTJJYLaLabaCDAZRCBSKLTAZBZRaICCSCAATKabAbYBKJbAaZQJQAIILRDAKISIALJCbACDaRLZBRCLTBIQBACADaRTbDKCbYLRBIADCKKIZRZKJQKTBAZAbDBaDISBTDKIaILZLIaaBDRCRDLbRTTbCaDSSZKTADYBRRYDTLIZDbQTCaASJLLBYQRYDbTBJYIRbQbQQKaISLKQYAbaQCZBQJYKIYJILIaAAbYTTRKDCBKSbDYbRCBSbZSQAADIbbIJZIJQJaCRZZACSASgYAYSDRKIRBJKDRRBLZZBAIKQaCKLaKBaaQJbADLKSYJaTBISZbDRBbRaCRQJaALZJbCTTKYJTSQLYADaDbaTBbSBDCbSIDabSbYQSbLbKLQJJRAYRRTbLLCBbKLSLTZDAQaAKKSBISbaDaIRYBCCLbIbbKRLLDLIITCBKTLABaJKDCYQTaSKbQQCBRTJSKYJCKAaJRBjAJDBADBKbaLLCQZDQZRCYARbaaTYLLDAKCQbZJCIRDZZKSSDADQKLQSYCSYCIBZBLYZQBJDLLIRDaYTLTbCYKRAJbKAaCLLJCRASAQZRYAYbJKAbaJbBSCRBCYKKZLLCRbIYTLaYISAIRDbCSQbSZKSQQSZYZBLTBYALKTSSLaDZQQSYCQAbLJbJRBDRaZKDbRISaabaKJKTaKISZSDZaYDACbTSSbTTaRSZaKbZKKQSDabYLCJBaAJbSRbAATZIDTZKSaBSbKQTZRQQQTLBCSTKLQCDIKZBZbCZRIKDCTKYZCKbLKALQDCDbQDRbbSCBLCRbCLDBbJYCYIYKaBAJIIKRTCaLaLACLBBCLQaCYJBaCRJSQbZRALYKBaJTSaLDISAbLaaZRCJATBDbKDSTKZaBYKJLTJCYDTQYRDQbLRYBIKZDbCSASSbLQCSbTBQCRJJRRDQBCTJKYaSDIBIQZZBIYZIbDBSZLKQLJSKLSICCCAJBTZaACBKYZQCKJIJQCBTbBCZIQQQLaaTAYJIYKCbKbZITJDALJQARKbDSKDBKbLQSYYBaRDBSRSDJJLCKYJaBRJKLaTIbLZAaLQQQSKQbYAaCQZbZZAbTQTQZSJKDYLSYCCAZJbbAADACCRJbabYBbabCabJQYYDbAAJZKCYKZICLATKBAaZQRKZKKRAAJLYaJDZJYSbSYbKAAaaTILRSZaCLbASDaAYDAbaAADYZSKZDZJCDTZKTLSaDZDQRAZLQCRbJDLKRbBSSBKABRSSRKDDBQKQQLKaRIRBQIITJTSBAYKJaYKLbIIDKaaSDLbRQSaSDYZZDaDDLQTYAaaSQCRZCCCZRTLKLZJRBSADIRBDTYBJZISQYAARLIYLbBDLJbLTLBRIJRbAKLALCISAIIIaJJCRSBRSTKTRSaKJbQTBACDZYQLabLRCbITSKYZZATYQYQTIQbaJaCZIKYLJIIZQLRKQSRSBKIYDSJRKARYIJLSTSYKBITBRQbDATQKAQQYDYAKKIDICJTabAaSbSZSDYJJZSLSSYCbKYbKLbZRaQARYaJIYZTRDKBTLDaRTYJSDKKTIaZDYAZAYBSDAITKRLCCSYYbaJDSACZAYbQKBTbSaKSKaYTLJbZZLCaaSAZBLRIBAaabaSBQJLTCTRRCTCIaabBYLJDKbDISDaYTLJTRRRQSSAJASBTaJYCSSSZRJDBYZARJZBRbZADDbZACTQbaRRCAIQJIZQLYDLLBaBTZIDRKRQSARZTYbAJDAARJJZIAKSZDYaaSADDaIYaJIBSYCBSZTaCKZbLKDLJaJSYDDSKBZJaDQASIDTSZBBTLQCKQBCJSIaKbbJIKSbKIALJJJLaAaQaTTCCbZbYCKJbTAAAaLYKQQLRaaYaZCSJYZQITYbIIJTKYBLbRCRTBCDaTCKAaIaTZAKCAICCZbLTQYZaBTQQaKCRDIKKaLSDZBZKLCBaJBJTbQCQQAYJYZbQbSYSIaSTKZSAaKJBBSbZBRbZZSQTIKTJLaAIRYbaAbKBTYKLKQISYZALJAYJQLYCDZJSISTKITTKTSIBYLJRRSbLQBAIRRaKBBACAJYbQDbZSZaDAbQLAbLSZLBQSAYCYLDIAIKLYZZRJKCKLBYARZCaKZQZQRDIaBRCITSAYJaTaAbZZCQCCCIKBSaJbBJbDRJRDKKIIZCBLKCQRSbLAJTbDBYSaQRYbabQKKALBDCKDCJKSbLDbASJZLaaBYATKTZATAZTSKTBJIYTBKTKKKQYAZTDKBCTbSCLQSaZLBZLYIRQYARZaJLKABLLYQTQSAKRKZTQKDJDQTSZADBQKZaDLbTJbRSZYAKAJZaKQCDIABAJSALQbaaYRJbbBIQKYYKYIYaYDZDSJZTZBCKBSJQCLTJTLJCTBbYBLSTYRZILCbAbSCYYCAZZIRSRSYRSDATYBIbKZKADDZJTADKQbTLRBBDTaSRRDLYYCZYYRaDBITSQYBaaRJaTKYJTATIRKLQRRJYJLKKQSRSQJaBIJTBaaYAIJDSCKaBYZBBCSRBRRAQKRDDAKbCJZDASABJAbZBDAJbbaZaDIRLQSAbSZDCJJYKLCYYYIRACSSaaYIKJLYYSBBbQaBYJZSQCZJBSTbYSJCIBRCCDLbYaaQTaBLAALaBSYSDbQKJKJARDYaDLbZCJYRJAAAJQSTJaALTCDaKKYJLKLbTSbKDCAQTLaZTbICIZQRbJBCDCKYbJCRLbYZIACBIABaCZaDbRYYADRJQTYRIaCYJIKRDAbTBQRYATZILbSYZZLQLSaAJIAaIQQaRbZKTTSYBSRJQZbLLTITbJACKbCDDbBLSBbbATSICRaLaQIKIAQCJSQTRTSRDaJATRBBQTSJJZJSaIQaTLYKTRCZKDQSLAIJBSSbBJSZIKCZTYRKIbBTYYaIADBASJTTaZBZIQRTAAILRQJZBADZACTCJKRKZQBDCLQIRYJTYLLBDJSRQYRAbYKSZbaADCBCJBKCTSSRaTQJBDDSBKJRLIbbATCBaILTDRJYICaKLRbBIZAbSDDSDYCZTKYDCBBBIBATILKZIBKLIIKKQBaRCQJAYDLRaIKKYDDTRBBCAJJARIRYCYJbIKJbARDbKRCRIKbSaQCKJCZYCbRCRaQbaYLLRZaQYLDYIIIbCITLTRSSKKICAARQSKIaZaKbLKbCQDTKYZAZKRBJLaSTABYAKAaDZQDADLQBZATISAbZIZQZbbDRZBZITIASDaCbDATDLCaRTJZRCRaQBaKCTLbSDQTYbbAQbJJLTKIDTZYLBIZZbIaJQQSBYRDZACYaATQZCDCCDBZTCIBDLTAYSTSaSKbbATSYBJbbYJQLJYCDZYaARAAZARTTEbZaRCAQBSBaaBKJRCZKDDDYBZCSTISZRbYIDQLLJYSbTaABYQTBYSCQSLQQaDTKRJCBKRBbbRJIZIJZCDJZBTDYBTKKaSTBTSRLDbAQbQaJALCARASbCIDABZKaJQTILDITTQaZKKBCKJJBBABZSADbDSLKbIJSDBRIYKDDBTSLaRZIASbDJZRRaKIAQbZD"},
Embed{94,300,"IASSQYQIIIaJISAKYAAILQJICRRYIIILDLZIJIILJILIJAJabaYYSaDYbSZaRYCaKCKCaKaKCaYaaaabKaSDbaaZaaJaCaITRaIDKDaBAaIYDRbZTIJTBSYaCJZKQBKYBTQYRCJSBTbIBSTIBCDAYLBCTBKRILKDRCZYRLRDaSYQIRaLDSKAZJTabIaBaBbIRDaaACJbTDIJRYSSQIZTTIBKYBLbTaIQZQKRRBZKYQCIDDCKKRYaSYBAKZRJSRZZaKLKIRLQbJQRCRaDAaLSJATDLSCYDRTBKBKRKBKRDYBKYTYIRIRabKDCTbABJBAKbYaSQBLTRYKZDKQITBTIaQJCDKYIRLJKBRCCQAIbTBAbJSSSJDBKDLSbaBBaLQAITZRJaISTRaTLbCSIADBSLZBIKZLQBZAYDCTYRIbIYBARRbSQCTCbLaTDJBLQDTaYRQICTLBRZSKTKIKbQIZYYIaKTJDIRDDJTDKQATbQZLAJDLLSRaSQaJKISYaADIDZYSbSBIJaJALKIaRRRQLQCTJQZADYLQLaTSIQDYJbKbZBQSDYBLAILIIbJSYaKIRYCSCDALYaDRLTYYRYTLRbQYDQTTDaJBRZDRSYaYIRBTDLYYRCaSBLbCRbBaRZIYaBbRCbbQQYJQQCIDQbCJKSbaTaaKBRYYBALZKIbbCBTKCQLLBYYQYYbLAKIKSATKaBDYTKaBSLCCCaCKTKYRBLYSJKDSLbaaADJQIJIKbQQYJDbRBZKAJBARBbDaZQIDBZaCAJIZCJCLKBDJTCSQAbBIQaaZIJQSLYILQaTQJSSDYaSIYSYRRbTAbJRKJDKCDLKCDbKZYCJbbSYCaRKDDRTLRIYZRLCJARCIBSbQZYaTCBALJDIDLYaIRQYZSTDILYSKATRALRATYLASKKIDCZKJJLCSaTIBDBAbITSLYZBAJbbRKaIBACRZIDLKQKCBSBSbTLbRaITYZCTDZYYDSADAILIRSJYKQCAaSZaKbAYBLBTSAaKASKDJbBJbJYRRICKBTBCYYLaDZIIBRbCZAZZKCZYRIBRAKSRbATITQaRLJYZBCQbIZQCCZCKKaKBLQaICLASbTRSCZRDLZbTJKTCBBZYDaCJKYZKIABAJARYLJTBKaRbASQKBbSZZYQRaYIbKSRLAbQZaTIIIbDBQTKQaKKBYbTTKBLYCYbCYQJSLIRBDDCZKYTRTQATZBbRaLADAIbYBAKQICTDDBZSKABRKQSJZRYQSRIJILRIRbALJBLTIJQRZbIaZaRYYbIaKICARbRTAAbABKSJSLAKLDRSbCBDLaLKLZYSIAKJaIIYLJCJSbbYZCSRabQSbLJBRTDaZADBSLARTCCTKQBBCJaaaZADBRBAZBSaDKQSZBZJbaAaDIAaJBQKTCIaRYRDJICbLSRYDLabQQaTLIDbbQATYKIKRJKQKBaSTBLIaQBRaBCYLTYIIILDBBTCbIQZSbQSBDDZLIAYSRBBJZaYDJLTDSTJbDSLBDAaaYRbIKZbBACQRRYYLIQDSLDYRbJJJKKRRSaARKSKBRLSQYJQITYKQAKBLKRAJQaJCTKLYZAKQCLYTJabQaDaaIQIJIbTaKYLBQaaaSBSSBJbJYQBKDJLABbCBbKAAbDSIaBTQDLYDBLaYCaTKLCSJaIDTDbRDQATCRSIJRYZSKRAKIITLDLbICRAZLLaRQZCaTCJQBZZRbRRLAbIaQDKDYRYKLBBLCZaLRJTCBaAQJJIaZJYADBKJIDQQSZQDQBESZLIbTYCJYIRaKQKSSLJTRQbRQRDDaYBCKZZDaACKYaLSYQRTKbYDRDbaYbTKKIIaDbJDaACIDYLSYLBTIKSSRBLKBTBAYCKADRRabAQQbAZIDZYbCZCYbCCDJTSKYSKLQSKIRaJAJIRLRTaDbARZJDCKIDYICQLLDJaKIQKSRIAZBJaQSEDIIJBIDaDLQBIAQaKBRYZCDBQYJLbYLCRJaCSLLLQSSTbCAZYLIKSbJLQJIYSBKCSYDZRLZRQAZJKYQBDaBRAZYZTaKKMSJKDIIZcSCYLERLbTAQBQLCTBBSERDBTSLRISTLaKILQDQJCCAJBLZQICDZBLDJCBRKCSaRCDLQRbLbYZQIYjJZKaZZaAJLILbQKDBJYSDYSSaCRQRLTLSKYAALIKSCRDKZDQbCCaLDJYQSDKbBBBRDIZIAZSTYDRCRSTJCTKJgDCJBDKAaQaDDCIBRDKITDIbYLLbBgTKaTARLITSALSbYaTYAahZCaIQabAbQKSIILcDKbLLKYaKRRLKIQJZaSJbDaCIDYIRiQDgLDBLaaTJBZaAaASgBIaIYIITDJaQgLaCTYKTYSBJJQYABDbYBALADaLKZIAIYaCDYhBbJKCBQLDYAKSRjBbKKJIEaCJRRQaQQCYZCcBKRJIRZQDYTaRTLSBBYQCBbIEaQCbMBRabbKLJTjQCZBTaQDhJTaIJJaDcZRADBRAYhQZQCBKRCjSJDLaLQRUaILYIDZYbCBjRKAZAYEKbTLSbZbTbASCiCERDBDALQDJCKYTIQJQBYcJKDRDBDcMCSAJZCcJDcbQILaRJKAZgTCAYDSTbCDZJCASJYjIEYQZLTaSjbJKQILaBDIaSRaIYQJBTBgBbASTTbaTSSZTTJaLSRBKZCKTTQATTKZaBiZMDBDQIKZSKcEZMCDaQYRBTQjAaiiDIaMRAcSSKCYACQJTbBLhJDBKBIAbAbkTITRJCDRLZaQZRCYBLYQYaSDCUBZRKiQEaJICYZBADTbMDDBajhICLKBgbIKBcKRKALTjKJBQSJBYYhDAjaZQAQTTRLQAKQMgAZLbBRZJSSJZTaSjAiKSahAZZEBTASDEIaTJDbThRDYKDicYTSLKcRSQEKaLCkLjTbKaAMAQbQUDQJJIijcZRKAcCSbbbEKTQZaCLTCTAQKJcjJIiJbEbZQJEjSJJUKhScaAYUDSTaESjTgaJBDQELJDTZbSQJBZUBQKLYEiaYIITAYcLZckDECIYUYKQQCZCJLChJBACYSIIaKbDMKMSZZBDBUDLMcEAgIQjbSQKjcIECbCbIYKETYABjJRSRBCLIBAEBIJLDiILZKkgRSISYRLCijCQJTgJjKLRASjabDZLDkCBbAYgjLCQAbckLkkBJKCKCaQDTcKIhRhbIAUbaZYcIILQEbjIcKYEbMBRTSMgcaJACZQaBTACKQbCBAKTBCiCIQLjLEcEcEbJcLSLQjQBiIQhBYJKRjBIKZgiCbMKYgQKAIhCKDSLaJATRjBChJDYRabSKhMJDhhTjIQUEjAKYBbiCDYIKIQDTaKYZJQEYBKDYJLCaiLScckkZBBSTgYjCbhDLYkAQZRJDUgZaIQTbDAgagKIQBQhRbiIIcaSkRTcKkbYRDIRZEICTcSSQRAKcTBiaYJLaKbBhBbIiRbMDjRagBYCUMMhIQJIibjTJjRIibJaaYCTgiMDYQBcgAZQiDZQhSZSKRAAZbcSjTKYLMQbIcSQKJYQQCCbUDbTciaKUbYJIUZYCiMkSCZJBbQZJiATZaRZAbTSgUYSYKIggJkEkUkSjiJbIDUKJBUJRCLJQALLhaRYSETZEhREJRZUYCSkkZgIYMbSQURAYSACMkLiIhcJCTQREEMKbaRKbAAbihJAjMIIjQaTQMhSjYhIDEEQkEhTaZkKDjBACZBJBKYJaADQjSDDTULkcQYhEaMgiMCcKbTKaiDUMLQMZJcLRIIQIaLIaDCBKJRgkUJEjkLKDEiIUiiUiiaiYYIkUYTkbbIESDSTThhIacjJCbUYCABJYZjTjkCDjEbbhaIYRhQQiYbJhCUCKJZbiSTkUTjciTKCEBQJMAZgJEgjjJiQkUaSASbSiJYJZSjMATCjgBTaTRJKRMcBAbkgBUZQEJcZKDBjikcEAKgYAiQZCgBLMEJgEZCMhaacikSIgCccDJhMiSLRahITARAiCCChJIThJSKJkRgTgJLibRaJhaTAcBgkbYIRhiEIkjgABRAIQgBchKJabBIcSLEACKSiJbCEIJcQhYEaSgihLAkUDJYUiMMTUKjYEAchBccIbYULCMYbiRLhATaEkAhLjCkbZIUbUQCQciEAaDZbjbkTECQKYZDbEKUKScEZUcDKYCYhiYRcZhDYjiMSRAbKKgITLKUYjkUYMZDhZAMDkAiMUDURIkkcARSTcLQJjTYjKJSYjMTRERYgRChTAciUbYjAREUDQkTDkJiIjiAhcMAbcCIYcSAMhLkLbciJSaaRCgkSkUDSCjDjLMLckBRSLEERYaAibKQKiZbYZjCRZihjUSJUCRACaacTiZcJjJRgcRZcRkUkQMhREQYRQSZiBQLhcDMIAJRMgZZJDkSSLELUgQUkajLLYiiKhgBBgKACcSgkbabMcaKJhkgKgYbSaJUSUCCBjSaMkJbbgDTkMkiaQTJjDgMYQZUBZRkSQbDaCRBYSMUUYKKbAYChjSjhgbLLjQQSJSLIYBkAbQSKSSQJLjQakggYJMYMMYJjiBbEUZYZgCIKZSSBYDRASZLYjYRkEKLAcTRYZBbRCCIjjLRDAgjRKLcKMCISKCSbKZDIQUKQaBLDSTYacZbYLQkSMZYLMDJTAKgcQZhTIMcJICDaJLZcBRTjDSJKhTYDCiBSCTkbCJDMQZEcAIDRCjbcJRSZAKiJhSCBSIZZCDBAQEJiJZCaYZUaIcaDibYUgZLkLaDIEgEYQCKSJLJcKBTURTCZbIZAYLZAiBSIDSDDAQUaDcibJRBajQJJARKBYSSDCKahUKTAQAajLAQARRbJkZbLRBUMIMbSSBciAZiaRBIAaCcJjDSTDiQATSDYaRTDBAUhJBMYDcbjJShaTLAIUDRBBJKjDbKKKUScYSShIMaLhIaZQQKCCZgcQTbSbjDBECRbZSCBJKLTDJJSMDTgCJZEMgEQkgDAZbQBSRCRJAIDKKbZgSULaRMRQJAIRLRLbCIBCDbbSLJLYSbkRJKQkLDRAQiUSKRaaCYRTScaTDYIgAbJEBDKSbRbJMDZQIQAYYEckQZQaKRKLYABALcTaTBAQZRBKTYDBbagRTMLDgTIEYhQBJIaESQMDMCJcTASIaRaRLDjCcQbBQDBQDIZJIJhDTQCSLbYBBQUTChMaRCCBLZaZJYRJMSAaTgRCSBLZLYSSQhLCIDIQBQSaBKZRMTDLSYCQZTDbaJTIAQYQjCbZQUYIhhSMYYMABbRSbYDBSLRaZAjZQgIQTICAcbRCbZYIDQTQTSKRSAQMIYDLBCZTRZDJiBLjRRaLAIMIJQJILLJQCYIRQCTIabLZDTCDhEIZCQLSYbcJbBQKALTSAQQJQAJaQgBiAQDDacbDbZhSBKbbbTcZAKbSUZCARKiibcSSACCAIZRKLDRbYBIZKaahKEbZLZjALQiJITMQBKJKUYLAYSgBiSSITQBTJADZAQbZDQKDRZbIEIZLbLTTQLhKZhDQQURTARCbTZICIKKbQEUaSRTTYZZcaBKAABYAbCSKSiLBJJJYDKJbADCaJbZSQMgYSTSKSYDKJYSLbLCCbQYcJZILEJSDAYJBIjaDaCkRYKZSDSSRSIJAaJLaJaZKIDJRBRQbcCAZJaZRcQASBYJCBBSSITILAaZBQSYRZSKDZQYAZMLRAaRYLCbMRiIKBiKBCQYYSbLaZDDKgTSCIRZCJCDKMQEKTTRaJJKLITCLUYaBSTJYCYICCbTaKDZLaRbLiZYSZDLDCaSJYDMSBSZDALThJCAZSQJTCKBLIabIDZBRLJDcKRQJaZBCSLTKaaATERYICKDQCDBZKgBZCZJSTbBDhBAbCDjKCaYDJRQRACbbAaAZLJBUIZURLRICQaRYQYZZZKLIScbaDRSLTRCBLDTAIiQaRaYYQbYaDKbTBSSbJSZQTABKCbIJTLEIATYLAYCALLKTbYIaCIKCBRZBRCRaBRIBSTTDbABTEiLQaLAKaCKTaSLDBRDYLbMSZQDDaIYLJCBAJRLRbSTAbJCIKSCLKDKYIaBBIYSAjJaJABBBKbDTSIaaCJQECbgDaZKEQaBYgQKJabaJBBALKaDRYbCJDBbRagSJaRISJLISjDaILABJbRIRbCTQJAACIJZDCZZBJSDBIjaQYIIAbDASTASTLSTTSCCZSQiCKRJTbKbKaJLSbQKaKBiRARRbbDDSKYaBZKKJZSJQaBAJRhIKQAYYAKDKMLLKbDQCJTjAbUCIDZARUZKaAASKRcTbTCSAJJMBRCKDRQaJibCRSIZEDTLaTaDaBJUZJTBMTKSJJBZRQTLDaCIMLBDaIDjSYaaJCYSTCRRQZRIYEIJLQJALKaRZACbUSaAaRbZAIbCbCUbbUKYDKLZLShJAJKSYKbQBQbJBYSADSRSCTiQhRIJYDRKJZTJSAbSQKKSiCZJaYaRBJJRTATQJbRLICJLCEaZAbJADZiCISBBQDRKTKQYYLLJSRTgQIBYaLBMDbRBIBIBDQDJSDYYRSSCQRCATZDLRbaQAbZJQZSbQYZIbDSbDSASADTTBRBBaJIRZALaDZBYCCZYaLKARZLLYYTUKbQJYJKBabAIAZCTAZQQaTRAYTSZJSJLJRKYBYJIIDbaSaUaRIBSTBKJIRJDDQZLTSjLSLDRSYDYYbKYLCZZYJCAYSLQRCDSIQbaITKKZbKATDSIYZaYiKYDIBAMYTKTaBKbbZQYCDMCBISYDJDLRDDYZLTBAABDIJBRaJCCLDRJQQKMJBaICAZSSLAaTaCKJbZBIBZaBLDCBSKYRaZIJIAZICQYIRQbRJQRaQAKLBSbTZSTaCZJDAJZRQQQQDRQQBaQjbARJLIJRDbQBCRbBIIKJBZCKSQKLAbBTbALCRhYDBCKJDTbIIADbbIQQQbbZCJBCAjBQSaKbCKZQZYQZAQCAAZZYQDKJTYLDIIIJBDbDTBIRETBZRJjTCaLDBKJLMZQZLSLaIUBJLDRZQQTaCCaKSLTJRSTDJBaRKACQADZDLCJcJAJJYQBRSASADZLjQSLJYSRIADYLTIRbYLARYSaKZbQZZYbSSIDIYKTJRCAQADCRBASbYKIAITZKLBJKTLRBQBAYDAIQKbTJRDLZbCYQYLbQcYcTSTZZZbAASDQBQTQQbYYCJCESJaABaLTLYQLJLBSDiDYQDBJajCCBbIZBLLRQISaJQbKaBYJRTIbIIZQZRCRQKaDABUaYLbITQTYCSKZSKBTJKKaYTKCZLLYACRQLRSRLLIJIIQTZJRBKbJQKLTYQQaLDAZaASLSQTKJSLTLbRLbaDSYBCbBBLbaCIYLZQIbYQIACJQIIKQTLSCZCASSaIYaCQBZLKRIbZRhbTDYKIKaIaZTLYBQBBCRjIBRLDQSTEaRZISAKYYTTRIYCLRbBbICRZDIJKKAQUDKIbgZUKDZZBAJZJYCYCSJQSQSAATaTZAKJQbJAAKJBQYYJTbZJBAKDJKQbJJQaQAbJTLJhSDQTJaZQTAQJZADYQLJZCKTaAKZIAQZSQSQaJLCTbabCITZQZCabIIJSYQZKKRZZTJQJBIBKCQQCZiIQZCDZSJZMKZJQLSbRARIDaKDYSLCQCAZQjRZCRSaAaabTBIJBJDBaaDRLQDAZADCYLLKATKAYEabBDZaZTCQCbRUTLaRRSIZLZCIJBIARBAZIDCAAYYKZRBDSICDZQJSYgbRQaJSQLBbbDKLLYCTZCaKbQjKCaZIAZiJZYLBJCTTTbICSDbCAZZSDSIAKDLDBKICaKIaTJJZJJBKZRJQSRKCITJbSTDCaQDTITATCSRaZbJYYIJTQCIIRRTSTSCbRDDYLLCLDKRbSbJaZDDBKLKBQSDURQICCbSBIQCBDaYbDZCTJCZZDJQRBRLLZDaabaSQYTDZZbRYQSSaLZQLDcIbDCYaZZLDZTQQIJSCJATCbbLRQSYDBAaAbRQJCYLbICILYgaDILSaSBTABZbJDQjYRBDMDIZSARAQYJDSSRJBLSRJbCKSaACLbDKZLSaRBJSaAIYbTRKbBYSAJZCJZKIBcIALJJCKIgYaLQbICbDSBbBaBBBQbCZSDBRTRKZDaChBRbaCCYaaSKBCYZYZZJYLCLZTYZYTKQTQBDKQSACCLaISJIaIAaKMLIcbabCARbLRIALLDZQKAITRSKaLZAYICIaYDSZAAQLaaLDKbCBCMbLaYaQaBbIYBKDSLTDJJYJCIISLCDSCSZRLQCBSKaDAQKZYKCBADbRYZQZZZBQRLLQJDCLQDIRARYRTYKaISBADYTLSaKJAQYJCRAKTZIJKZKAKRAKAaLRSQJJgAYIATJISYLBZQYbIJQYYCTTKTDYSADZZZLSZYaILSRCADDCCJbLCBDKSYCIDYCDbDJAIKaKKQTaRJZcAbQTBLQaaYRbJaSRZDQACDIIBLYbaRTCAIARaYLDDIKaCAAbQLRZZKCRDRIJaCJLLZBTaaCbTRSaaIBIJJaIQQLYYKKTQKbBISabYQYYbLCKJYKAQBSLRbYKTAAKJKSLBSASIZJDBZIIRTLJIQYCQBZaRiQQJbLLCRQDZCJZCYRLJYQTabTKiYDIDQDBKKBCKaJKRaJLaDKRaSRLQIBQDZZTYSIDKISBYKRALSZbIaCBLZCTbTZIbaAYLIbCBRLbTICaAQTAJTADYIQbRbSBDAQLTSQSRQJTaILDZCTLCJKZKRaLCbKaRTIQAZJbbTZBAQbaQQSACBDLbRaAQRLSBLSBYZITYCJCbKKAbKTQJSaYLbASbQbAAKKJTZTTLRhTRbDAAJTSQCZAZYQLQbDKSLKTZbCYKJYTDJZaSaIKJaYRYQSZATDYIKSCTKRTACLJZTQTISBAZCCZKYTYTCATYBDKDZLTDDKIZbBYACYKBbaSRCZIJYQBLbTKZaaIBDLBTaIYQIAATARKDTSIQaJTRBJKbaBaITLCbaSZYYaBQRTSJBSYSbCLbIAbRYDDIDJSaIRTIaKJZaaKSDZYbbIYbZBQCIbDBTKYLQDSLCBTYZRAAQZABBLRACIKLCYbaKSKTLRJSQATJLZLLLYJBAQJKLaCbDQASRSLgSTYBaCJZCaJYCKDSTCTaTLTJKZQCaJIYYbDSBaYSKIAaaSDQKJZbJLDDBAYCaIYADDRIJAKKbJQDgJYTAYBTSZDBALaQIACCZBSLZKBLaaBSBYADbITKSRYaITITTDTJISCbbKTQYBRJSBQRbaRaDSBYJbQZZYZBTZARIIJZAKSYDbTRSJTQbLDTaKBTCIRBaAaLaYBSiLZQRRTQRICRCZBQaLRBKRaJTRbRKYBLDSQDLQKCLSYICRSYIBLJCZYTBQLIILRBYCASDBZKBBIYJLJJICSSZQLMASKYSIYYKiCBJLTRIIbaADYLCBQDDKBKSbDIJQbJZSbAaKITTQabTRYbBTCACbYSbYZBBTYRDCRKZJJACACQKYDJaDaJTCaZTDYDYQAJSABQTYTIJLRKIaSRJCCZbIbKDbSCDZRDTKQZKTRBRbAbKLZZJIALSDYYDQCRTALSSbAALLAYLKbBTQADRLJYRaYQZLKTJJBaRIACZRYBRDYTTYZTaDYLBKCYLAaCAISJSYaCYTIbbBIRJYRCCKSbCKaJKYBLZTBRYBTAQSDaTCZJJZTDLbSBQbCYZQaZLaTLIRSJRbRALCSIRYLTZaaTIJAYQZLJBRYDSKaAAAJZAaBbLbCSCBBLIKSZQaCYYSDAJILDbIZAQLaaaaALZBCaBZIYSZDKAJASIYATbTTJYDJCQZCLCRBLDaCLAJaDTSIJYLBRBaKAKbDKTRSZTZaDJKCBKKaTATTCLZRQZAbASILRIJRRbASTDQKZQQKDLLDZbDRSbLISaDDDKKDKJQQJQSaBSYBZBTLIKCBZKCIZKRKBbTCTJKZKbSDSYRAQRBSBQTTbaYJZBZSCZZQLKTSYZQCKZbSLAabSTTRYBJJbQSTDIALJQbZYAbAQYJAAbYIBaBKITIIZTJDIISbRTALLKaAJRRLRYTSATBbaDSSICLBBTCQaCAQABYbIbTLJILDKLbYLSTBSYAASAJITgZQIL"
"IaSDZBKJTDDYYIaZJADTYACSIAJIJIZCDJbAYIQQBKYCIKISbJBRSRTZZaSRKCYCBRKaZRBaZbKSKCZTBZLTBIZJJDDJaYKRSBYCQSJKBbZQAIIQLABRJALBZDaALaRaBASTYSTARSLRDKTLLYbJDaBBYaIJSZTKZLDTZQLDKZRIBAAaJDAIbBSCCbDYRSQTYDCTYTJALSbJCbaYRLRBAbYYSAJQQYLRSYZaYLSYSJTQaYbbSCBQbaYZCLaSTRJbIIYCRBBILYKSZbZYTDDKBTRIJSRAZZKQYKIQAQLSKDYRbKKJYbTTBAKZRYSYSZKaaTBSQKRLbCRaRZDRKSKKZLTIbDaCQQBKBSRbTBLRARKIJRBDSBbQaLQaLCYBQRYRLSRJQJDbQbJTQZbKQLCBYQbBSCTRDSSBBKTDZKJCSIASYZBbZJKYJYLQJTaZDKIDRJQKSBACZIaLCICADaZDCYAIBLIIRIDbBIbRQITZJAaJZAYSYYDBAaaKLRAALDaDYCKAABRCKRKJZKKZTBKSDTZaYTALAbBBJJDaALaDYLQSDKJIbDRAAAYBBAAaRDbQIaLZRTKQaaRJIDQRQBSaRIIDLSSLIKbKBBRbYTZALJDbCSCTTCJJTaTbAJYDALLSYRBCAbZZJbRabbCCBZJBACKbBADCISZARDBJSAaLKZaaCYCYLSRDCLbRbLATKTYRZCDSaCTJJDZLKKCEBbDBALTKRAbISYZIDKBDSRSKSJZCYJLAJbKCYKIAZLbJLRDDBDQDLCSLYJBbQCRIYLbJCIJLDQQQTaQQRTSTZQTBADKQRCDBaZTJaAAJLBCDBBAKRTKJZIJIKJSTRITABBbTZAKbSYSBbZQSYKSSbTYTCaBaCTbYAZaLISDTBBDbaaDDYATZCaKZYRRSQKYZLJTRZbASAZbaCLACaIJZbaZTITaSQSQKAJKLLaQSLTSZRbDaAKRJSQRbZTLbDAYQaaYSKabZYIAITQbKBIACBKIQSKDLLYQABIBaAZLSQIZKaTBRICZDZaLYALQCBYIJQIKaQBBbRRTADIIiLDTJbQJILILIITDATQJaKSRBJCAQYJRYSAYASJTYYBYYLDBAbYJKRaJAJZBQRRTBaSaKASRYCAJLRZRYASZAJIAbIYCaLZYaKbLDDKAAaYJYISTJRIARARACZCbILbQJAYabKZQDBBIRAISBbQRYQCYbCaZCJYLBQBYQQBLACZJbTBBabSDRZKYDRSYRRQAYTJLQLTZbQQbQbZRZIJTDSBCSAbABZKCQIRaSRTCZBaIQDALbACAQZRTDQLLTaDDBaRYSCTCJKIbbDBDJZBQLYCCKTTRJBBLDCSIbDKRJLAAYSKRJQYZDBRSBQRQYZTKDiaSIRKIBZYbKZKLLTDCRbYCKDKILabLabBSCYSASJSBCDYaZRJbBQJaZICBbaTLLRCDBRKKSQZQCbATYDZDbAAJCYQICCTTTKIJDBbLADQTIbYCZKAQRDbJCSRKIDRCCDYDLCRaKJYJKLZLYbJZQbCCaBKBLRRIIbJRASKCIaACCDJAaCQSaAaKDJIQDYBQTTBRRCYDASICLbTLTabSTYLYQIBTbLYbATQBIDJIaICILQZKSDDSaIKaaAAJbaaJQbBaYAKSLKLBbYJCKaQATLBLJLRRCaAQYbKIKKCZJSRCTRDbAbCbJASaYKKDTYaKBDDIbQbIKZLTAZQJZKBKDYAQBaSDISISKbZBJBbaBIJTbYIAQSaICRCaSTabBSZIDaATRBbTSJLZIbaSDAIJABTBKCSBZIKAARZRLILDJIJJQDDaaKLaBKCZQaTQCRZLQLKbAIYCKDYRDZZQQRKRKaJDJBaaZQBKZaQIIQaLJaYJBbZRSTLTaDCbAbZTQLQQBQABYCIbTaZSbQJaDYBCBSYLBLLJIbJCBKASZSYSKSTDBTKCZDKACSZDKYIaLSCLCRRRJSZYACJARKLYCRKTKDDaBCaCKbZbJRSZTDRLTADTSDCKALZDYSaKJQaQAaBQSZZaaLARJZaRDBRJTbJKCLZYKIbLYSKQRICKLIJBSRDQKCSYLJTZSbQARSRDRJKYJZRaDIJBRQTCLICaKKRKaaBIYSCTYLbDJRTCJTDACRDKAbYYKZICbKLYIQDQAbLRIQCJCLaRLBZDSTQLDKKKDLQYBbRSYLYTKKbLSbZISbJQTYCDYLQIBaDQRAaIJJQICLDCSTKaJSCAJIICBZaIAAICBZBZSILDDZTSKIKDbBTLKSJCTTTDbDAAIRabAAZYYDILDSDbRaRTJDIJaBKSSaDKIJDATSBaTRYDKIQJbYADTbbBQQbYLZSCbTKDIDLLBTTJKAQDYbTQYQIDSSBSRJJBQZRQBBAZISKSIbKASbJbSCYRDbaLRSaSTQaIYSTJQYZLJKBJaQRQYQZaYbBZQaTAQaLQLZYJYJaSQCRBDIbJZRJTbTIYCBZQQAbJTBQQITIKISYaYbTRCYIBTIDBJLYTBYTSABTRAZDIbZJaTaQaJaYCTLJKZDaLLaCYQTKQZSDLZARSLDIaKLDRKCDQJLQKbJBQCSRLZZQKQABSTRTSRKDaCJYRDKYLZYQTaJZTaQIbLLCCSIRLBJJZTYYIJSYbSSTKZTJKYCLBSZIRLYKDCCAbQCIBYSIZLbZDaBAZYLCCCSKaLKTaTZaJSaYQaJQDDSLaYLbKQCAZTLZYLRCYbIbCJBDAYDIBSbQDIaaYALSBTBQJLBYTCARDRRbQTJbRbCCYJYSTYKDZAAZLZKaADZSKBCTSbDRKZRCTRTSBTKTZBIJAQYCDARZQZBQLSaQKDRACTDIRKKZDCJbQDITJaABLKIaDDRSIDBZDACSaARbbBDCAZBZBaJDLJTCACaRCJJBCRJaSbIBITbbLAZJCCJRDKCTQIRCDTLDaITBBBIYLJRaTTKTSIIIJJDDYSRYCaRaaCTbIaQRICbSRTbQJaYDTQJTKARRbKSTZRJaLRaRCaBRTYJAIDLIQZBIYTAIBTQaYTaKSYRQZabSZDTKSLZSZaCQDabaLDDCAIDQZSbTTTAKAZYSaJJIaTbYTSJBTTaDSASJLISSTIBZJQQRQLZQIabBLZYbQIYTKQKKCbJAQZYRRRZYBDBKDBLYbZDLRSabbJDSQIYIJLSCCLSLAQIBTIJYKQQYTQKSQQSLLQADbDYSDBRJJSDbBCDYZRBLICKIaRJCaQLSJJbILIYRRDIIaRAKbbYLJKIITJADKRQKCBaABYTDZKAKJTQAKZRCIbATIBKARCSACLZZCSTKAATJLZQJQZJTTaaYLaYbCKBBZSDCaSASTARLaIDJbaQDJQJJSbRISYARDLQTZDBZCZTZJKQDLaaBAbJDKCKJALRQYDYISZSRRKKQSRSZaJJSDAaLJQSDIKJSJSKZaCSTATBbTKDTJYBICRQLLaLCaaaIZRRRSJAQLSBaJKCYZBTKJBaJRRJSQABASJAKSASQYDQYbSICDJKRKYIbLTbKLSDKbSYAQIQbIQZIbaaDYZTCYYaYTYQIIYIIBYZTIIbKAbbaKRARDaLCSALQRDDSKSbaBZARZJTJSYLJBCaLSDJQSQTLLbKbDZQJDQSbDSDALBLQCTJARBYRZZBTDISZJZaJbCTQTASSJDIASCBTaZDYYaCSCIRbIJAJDKBIAbbATLQBbCLLRRARKZZaIQJCCLSDCLYDZALJIRIYKRAQBKRSQSIDJbQTAYRRLIJDZZCTLYASQDARRLSTSaKTDLZaaDaAZATYAJKbJKbBISKBBBTaZLaKSKQDIRKDSIBYIBILTTaRYQCKIIaJTaYALDTQbJJJRKITLSaYYJJBaLRALRRaKAQCASRaaLTLLTJKRTTLRQQJCLITZQCLaAKQADCLDAaZJYbYTRLJZSIbaAZZCTCTTYKLbKAJIRCCSSRYCLIBZYCLbASQJRKCKTRZLJZSQBZQYbCbIRCBaQYLKJCYBKIDCILDLSRZIKYCLZLTYCTAYKCZZIARZSSgZCTQKbSCATRTYJbTKCbRTIbDTKTAYZCCICQJKaIaLIRKKQCBYJZSZaaDRZLTSIJRabJYIYRYJSYbBDSZBZTQSQZABCCCLLDQTaLZSBLbRTLCRa"},
Embed{95,304,"AYYKBbbIBLIIQQIIIILJSIJKAIIIAYAIKILAaLIILIIILSYQZTTQQTLQbTCTYTTTZTTTDTRSKTTTBTTbATbTTTTQTLLRRTQIbBaQDIQaBITKRYDbLRKALCbKTBaYDRKSRDZJDaBDQBSbJYIbDJSYCTbKZLQJSALBaAbRRLAYCKZTQRaDIaBbASDbQZSIIAaIDZTBIaISBTaYKbKLARJBZSASCYaRDDLKRZSZBLAaLIQTYCLQZaIYJSDTTBbZCICSYJRDAaSJBDYSCLQBYCbTILJJKQLaRKAYZYRRDAKTbADILBBYaIKTLaLYLCBSJJSYJKYQYTBTCQYLATJIQDabBBLYZSBLBTKLbLSIbAQBAaIJLDTARAaQZbRSCARTKDTIZBDYYAZZBDDQTQJKaYZJZQSCKKQIZDKaAIBbYTBQRCbZLTSBQARQZaJACITSTZDIbLDBDYbLCDYKTCSBAQLLBaSDZJYYDRBRKAbTTSDYTIRTADTDbBSJIaYDCLKDCLCaabISRaTYLTRTRQJSKSDJTQYSaBKbYTBbZaCLCRYYCDbBBIAZRSKYKbJZTKIILIICaYQLIICKTTRCaYRZKbJTZYZIRAICQDSJTaYIKaabSRJCLTYSCRJSSDTTTILYZIRTaBBSAaTJDAbbTKDbYQZRQKZCDIRKITbBCLKQLKKaZQYJKZAQLAIbaQRQRbLIRSCCIabaDbABQSbCTbZKKYCbIIJRJabQLSYYIIATJQZQLLCCTaIQKZCTSbSTYRTaRYSSbZJLKDJQABAbbSTaRTTQaBYSBTDQYJBYLYBKKKBSRRZCDDBZQLKQLZDATYBSZaSIRSbBRCSRRTZYJLQIDAaZCJbYaSTKBQABAZYSKaBbLIbQKLKZLRBTRCaACBKKZTJCbAZIAZRARTbDBaLCbLaKIDISRaZDARJASYbDQRYKBQLKACSLZSZJbTYAKYBQTTIRDYIYTBKLQJDSLDKIQDKZQZZAJbTCJCaQSYZLISBCJKAYRLKAbYKTKCIZSITRLaCZLADTYQJbbAKJIYaCLCCbSIDYaJSLSBaTSZKLSTYYAKRDDSQZBQYaCbTBJSILRAKITKJbTLIYCSZDJKBbJaLTbaCBTaIaZBYaSbTbZACIARRYSDQBCYDBAIaDTTKTBAKDTAbKCDaSbDZJIRBRAJSRTbTLCbJaKYIQbAKBTbQKYSRIRBIDbJSBCSSRZISbJbLaQIQDRILJAISRTKAbKbQKaICbIBSLBJTBIIYZKYQDaKIDASRAQTJICZbQLaLCCBBJJKAbCTSBLLDQKZYQaTSKRKJJICCZYSZJYCJDTQIZBJALAIABaQKTDAbTDQBQJIIaTBBZBYKLJSYBIbbASKCbRDAAaIBJZaQDCDLAbDDDBATRBbJQYJDDQaaDLKYAJCKbDQKBSCQZTRTCTLYbTKKYQLSITTTZJQDQJKDDaSKaYLaCKQAYRBJAZKKTBIKBYJTBbTZTRTRIYLaJTDCLbIAaQBBbASJKISQCJIDKbKRQJbBTaKCJIKBDBIbCKYJSDDABKYLIICRDQLDTJBQaCRYSSbbbQbLYBJSCDLDBZBYQILIKRRCKIJbKCTAAZLZSQbTYBCZBRLZaYZDaCKSKATTAYCTaAbSKaZLQCBbZYARQIQJDDBLLIZCCKLCBCLKZJADTRJaJYAAaRRLJYbDIRAAbKYIITYRYZCbBaDYCYKTRBbBJAADTBaZBAJIZKAKaLaCYbbRKBJIZABABJKIKIYQRDBSCSYDCJMLaSBaKJKCLTbIQLQAbDKaDRTbRSBRIAZCALbCKSbAhZDBCBaTCIAKSKTRaabYKLRIIZbYBADBZbIBSZCDLQBIQBBQLYAZZLLKKaDTTSYcCbCBQZYKCBKZQCAQYIRKCYZRZJLRaCCKDTZJBIRARKKAKUbLALJBBaDKRRQYaSBJICQTLZQJSAYCSQZJCLZLRbBaQbRYbZBRKCbYBZTZCLbDSTCTSaITBLAYQLIIZCaKAJYBYLZIIRQCQAQJLYIKaSDLARTSRRLSZYDIAbZZQaAAIBQRZBJABKCABIILLRRYDCSJJCTRDQTRRCbbZbAKYKQbKbSJRTYAYRZKDbRZSCLJBQYBSBYaJTJYbTaDLBYAKCQbRCCQALBBYKJDTJCLbQbQDbCASJKJYIDUCZZLBCabTTLRRbBAaKRabCICRYISBCcaJSYaRICKbIaARRaYEJZCBRaYBSTJaIIBbbbAASLZKAYDSbQCCBKIbYYQTDBTDYTDSZjKaaZQBRCQTKJIDRKLRCKIZbYBTAJSLZZCRbaCZRSLARLTQZKKBKaCKbYDZCbSRYiAIQaKaYJLYJbABTAaLbCCaKjRYADKCZbhACTECbaLKBQZZZbIjbAQALQYIKJSTDSKSRTYAERbIYSAcYRUJTQMTYJbBLSLCLBJBZUTBabAIZRQZSKBRAbSKTAbAITYTaDiAZZcJJQJABbigTJCBQbbKZICLYKLJASIabYQRSSQZMZZIbETCCZCBBQAUKRJDZLLRRTaIQUiaJSIDJbabQaRCBLYTahbiaZDAQDKZAhQKZZRJRUChITKQKAAZRTCZRbKTRCJCUZCaZCQRDYAASMSZbAZJAQYRJKTLZYEjabIBQQBBJbiSJZYMBQJTZRBZLTiCLAKIACTDAZIRCKbBYBAQJDDDAUBRSbITSAZTJMISZJbCbBBASbbaQKaDRaRIcJCcLSBDQKKZAZKACCBbaKQaTAYTZBLTTbgBJDMbKIcQLBJSIhaaaSQaBRIZCbcTjCASCAagSQAYCKLCDbJCJRaTJRTJYDILBJIZEQDaBKDSLIZILRhIaaBRMAJIThYCaLAkRQbRMSJBQCSabAZaaLZCKLIUBKbATQJRCRALAbJCBIgBaibIIJQSMJiRKKKIBJDTbSJZRZILYRQAKcZCbAMABgiYJCUKDYabhSKKACTKKKLIBDUQDiSZTAShQIEKTKhQYQbAIJIJcCkZQhCBYLbTaMUIQbUQbAaJDILAJIKTMYYSTiBLLZcCaRCQMTSbBbQjATCaDQZBQRaMYUCKCZJTjASZKSDAMBJJbRIABZSgQbkKCECAaDaIKTaIaMZZjUMTKbaZJJBDKYQEZMCUBCJaBSaBhiYgDIULKZTBKURLECLADKLSKSaADaZBihAKBRjBKYbERZBbaDAUCjADIMTYSLjJRcSEJKQRQJUBSJKSARZBABYBaaZgkJQLZkTDKbDZiQBkKbRhUTBSCKCJIRAaChJgBIRYhUTTUSQSBRYDZbiakbUKTYRRLcbjBDLhSABCigZJDhBCaKJIRSRcIQTBJIbELQijEcgahMMSaBEZCjDURQAJMaiQIZAKZQLbKLLMIMRcQiLUDQALDiKLUBSQckjhLCDgKiJbTKBbhbIEiYaLcQSMUUhJRMIJTZCEgbcLSQZAKQSaBcKSkCghEbKQDJCQCLLDaJbhaMSCYYTkRLEEEDAYATDQYRZERjJDTKbRQbQASbMAgLBBTMKEiJbDIDDLgMjQQUcMbbjiEkMCMgbJMLBRAZZDSQYUYZASYMKjIiYBUQJBBbKiShJgiETLbcMAhIEDDJUMjQSIBAKjZkcaaQjMBRJEiKgRQLMIRTikghbBQabgQaSbIiAYJYZcQZcABDcBTkUYDKkjKUUBRbTjahiBDjYKkUSTEBYBZTZMTQYYYIaEAZBDcMSbQBMTSjgUKikLgISKIMbjQEaDYREZQJLKaARaSDSZDhUDYSAMgMiZUQRiJIIjMRcjacchhSMMQiKRSTYJKKQICUiajEcJSJTMKKQggJCjMTKBSciTEDbciJLTKZTgRjjjiDkbSSQYCcAbIUYBaUJMEQIjgkKYLCUbgMYJBLLRKKbUccEhDLaYBDKCgSCEZZhKRZADgZLjDAAJIEgIhUjkZhCARjJLgKZCJaEYLKDSihKKEJkDjIRjQADQQChciijEaUBJUcELTIBBaJRhjLAEKZTYjSTCBiiAhCbTMkUKKhSgjDLZIiMCIkhYTCikRkahYibMISSSKRUkbBLLAZKRREAbUMaIMQcLhEUjDibZDTKBLYQUBACiMMjCgiTMgjCDMAEJaUjTiibAiAkZMJDTJSjkASCJgDhhgkJKCKTELjbZTCaRJRZLcITKEUiiYDMDjhYkCBAYjjADRIiZRTKUMEQRiELTaicUZcgjBaDBZUYUiYcLbjgSJUbJCBEBZTDhDMjQaUgIBASSEYibUDMcijUkLMILCIaAMcSULRCiZLjYhBgUiYcKAKZbbaTMMCAhkjZAMbCbRRhgiUkQASkSELBbTjCZBckMCMUagicURaKCigMJYKCUEaThkTBkDLkUMBTZKcZbicJATYTbiBEijLYCSZgZgaKkTaAbREUYBjkhaaEMEgCbYTKkRTRTZQiATQIkjiYjAbJIcUjBjBijTZUjEjYQbcgMiCDIADiBZMcLAZTaIkZMRghbcBCUibQREUiLaRgUIBhkISEbgYgLYbDgEaQcQhbiCIMcAQKhhjEDABkMbZELAJcQgLKhEUkERYUiMKgKUjSJMbJcjDYMUALIRCIYCSaAUCAaJhhEBbjZRAhCAALTaZLBKRRahBbkTabZQAZAQEDkYiJRggBDgDCiQUCIbijTgCAIJDaZZJYcgSbARQSbRBQQaMYIhJcUUhJDjYLMLCLEgSbRKRBaSYRCZDLLRhESIQgbYjIYKbRKhLQgkEQUMgUiAJRBRcTAbYDDARYBRTZgYaSKAbYQbKiBijLgCMaDZjZDjbQIUZJSiDIiAJBEiQcDZLhDSgTSJjQBiiaYKBScJhKBZBRZYULcKZiQJTIQDTSAISbBJLIDJSTRYAjADZRAiJMACLJJJUgiBiaUEhAgbIKhJJcRTLBbAILESSKQSAIBYJCLCkJaUgKaKjTIaAZSgSZQASYBAJYbDBZBQbLDDaahYYDSETQIQcBMTcagMLIDUKBMhMJQACDTBMAJTCTIMcbUbKYYSaIUCRaCBDTTJRAhRAaZMISjTcDUibbJSaAYaJRLjjYKScQIbSRCbbQUaCEBAJhhJEcMRISUCLUIbRRYRRcbMRCDBaYkUShUbYTcbJSTSQSIZDTDRJDIahQJIIRQARbCajMBbQSJaThbMYgjLTMbLhaDaKSDCCLQRIaYQDQbIDKiDDRRaLaaIcTiITbMLgBgBcRMZJRiYBTYJTTRASaCTAgThUIBYTQbZSTQCRTRLRQSALCKBbbBCACDRahiJYcJYcIJJALLKKTTSZQKQAbRCLbZIDZCRCCIZRJiITJCMKKjiaEALMTKjDDRSAbCBYIZDJJZJBJQAAcgSLCAKIjYbQUYaMEQSRLJUZIcQRUZQRJbRhKCiZJkBABJQRIBCMLQbLaLIQLKJRYYYQASYZQCEJCRJARALBJbbaaBCbQBJKAIaKbAaRUIbCDASATEaDEKIhCZQJZDZQQaQADQLSBKDYTIJZKBhLZCQLZgQYLhZITBLTRkLECCBICaTDaIKJcQKZbbRkSKCZTjBKTaRRAYbTZTRSMIMDCCYQLaaaQCCCYZDJABDSSgCRCjZTbJJcBbIcUASgILDZIZEJYiKIBRDgScKBKSIbSRJLJAQTZJiTTbCTCCQLaQhQBbDBZJbbjSDYZhaRIRZaTiSTBIAhAJBLEaKLTAQQIICbaQLhCQALIZjMKAYYDjASZbQBQQLaTYTBKTTTTbZKLYMgTLQZDYMgJSLcQSZKKhIAECSZSZLSACLSLBAaIBTIDZJYQbLTKQSLJUTDBQZYSSbSabTBKQAQKTCRTBDaLDDDURADTCZQJJaLTbbbRTbRYAZIijIMASYiZETBRgJLQaKkKYRgERIKEBhaYARITDiILZTiYKJTUJBCJJgQSKCRBaiCTDabYbaKKSbZAShCTIZSIZRTbRLaLUbYRSJJaQTTSKDQJZAKJTCSYDBBBJAIQbCZAjUJQbcZIciiDYaQIbQSIIZTLbADQLQaQaYaBAETDIDQCLAcJJYRZSBYRIKKYIREZQIjMCRJaQLbSMKaJCJQQKJESTLDAbIahBBDLjYBkRgKBcDYIQBTaAQEbRYZQTBYQUbMTZRIIaIRJCREbDAJAZgKSKZCCBBaCYaJRMLCZYTJCDKLRSQJQCCCLaDTZQBDaQIjSLLCRSRSBbIDYRTTJBCQRCScDZbQbYaJZYTQQbYZQAZSbZYBQQbTJhBCIZCTiYJYJaALaCBSISKBBDIBZAYTIEBYJLCbSiAbSDjLabLaZRZAZbQQBbJLBKZBBYRRAQgTCKYIBUAQJIRIQCaZKDTaRSSJJKIDbaZbRbcRDAARCTZSYSCYDJZMLQDSZRYCCbRDTBIhaLAIabQIbIYTQaAaLLDIYBbCIbQLTZJYCYYSCJBTTbShZQIBTaJLCLYjZQCARRSQRaYCIDTBKLbJZAiIYZYTKIBCIIZDTZULBbAITKiSKjALLIBLKbCTSQYDLSKJbYBIJBSCKKRbAaCSRBJcKQKTbbCTbYSIKTSMRUAbQDYKRKDKQAcJJaAAIYLRJjASDLDQbBIJbALSLKJIKZBDJAaDIJRLKIQSDDDLISQSSCCbIQDbKYLaBJcYAcYTLSTILLaYDIBAATRSJaYZYKRAYZMJTZQMaaBRLDZcIaRTQICAAaCQKaCAYRTSaCZIDSCLKQKCKbIKbZbZKJQKJJTTcYaDRLabYCAbQARbRRAJDDATDbbZJSbITRiRIaBIYBSQbDgABKZSJJRURTSJYBLLIJTITAASEQKIIZaLQCbIcLIJcDbQSATDYLYBTaDabKABJLJQYbKSBaSDaRiSRBIQaSbJbDBLTJIbJRgBISSRCLKKJRTCbSDaRKDhDQaKRJDARTKBbYYASIRJSCCYLTBQbaBDSBSSKIZIRbCYYAYbKZYLaUKLCQLTRZaQbLTaRSQAQLAACKbIRBIIZYTTJUSBZaBRaabYKABSRTZQLbaDSbTQJbCJZIIKBRDQUSILKBIIDIIQTAAZCRTBDJYYBTDLbUBaYaRZBLTRKSAZCQYbDKbJaYRUZDEYYDSQCLBBLUQbYTDaSLCCRCZSTRZQYYSKLbbLUBZbLcZIJSSSTYaYCLSCDJDBaaKBLCDgbQDTKZSaKLBCDCDRaDQZDIDRQJAZZBAaLCRJDTZYIZAJiKJKQIbLIaRRCBShTICAJYBBBaZTLYSZabKAAYIDYYAKDQAbSQZahZTBIQJZCbSBQTAKACAbDYQCaRJDSbbASQJDKLJBTZKbaTgTTIAEKIaBbZMBLCQZYADBbTAZATLTLKBQSDIaSQCUbRJCDTAYbAILBZjZQISbYJDAbRbALBJQKCZBAKCBCASaIZbhKLaAIZYIYQDYAaBbTJBSJEBYZITDSLbDQYDZbQQTLKCZDTQaYRZAJIbCLQJACSLjZAiSRZQYZcUKbCjjCIZbAIKBSRbRRADZIJRBSKZBTBKRSSDCSbBSSAZCSYCALJaKSDaKBIRQJIQJIRTAKIQJJSRIABRTbLTbaJTDKCZKARbTJZLBSDhLYCLALbIIQaQaDDRRCCRJaQbKURJbBAbKDbCLcBBYDZZIQBZJLIDADIJZBYDRSEDRYaTZDbADAaSYBBKZTaKSLYaTRDYBKAYJTaYCIbZaBQYBYgTbBbJCLCSQCaZSSaKJYDASQIYJYLQbDBbYCJCQTDaaIZAKLSCIDaTTRaAjbTKQBBRLJTaJLKCSQTKZaiDRaSBYJIIIaYLJTKBCLLTDLRhISLCKCSCALKRKCSQTKBTDYAJRLDbCRTSYDLaYIDKKLRaTIZCZAIbKBBRIcCbITIACabJCJAaCJbYbTZJQJRDJSTURKBcQBISLDCCTaRTJACTCBTIQSIALUCaTZSSLSZaZLBAZQTaYDRZTYZCKTaJTAaAYSDSCbDCIbTDKYIJbJLZKRRJbAJDICIAJAAaLDbRZSCJIRaKAQJZbTAIbSJbbKZQDSLZAaDJRALZTBCaZISYbCAYSTLCaZLSCCACIaSbYKRQZaZATKTDDRRbbJQYKbZSCKTZIAQLQJDKSIRBbDKSYaJRRKahICTDZbDKLKQBTaTAZSZRYJAaQBKCbBTagKaYYaDbJZSSSJSDIBBCLJYJLJDJIKaTRQaKDRaRBAYLQTIKSbSBAbDZRRYDSJRLJYDASBDaSIbLCbbDYZTRSJDCRLKSARQbJJTaaYAaDKRBLRAZJDIbRAKKaZCRBLYDIaTYQAAKagARJCAQKIJYRRJQLRaCAIMaRSLaIYJZSLBRDBIaYbbCaSDLJBLSSbJQSZbbIRQLBLJTBLSRZQBBERTDCcDDJAYaaKALTQCBRCYLYRLDLMSaaABCQIYQKSCYZKaAbRLaIaDJMbIYITJYKALUSBKQaLaBaBJATZACYRRLRLiaLQQRZKYAQBZBaDIYCBRYcZBZaISQQQIaKLaiaISQaAJYLRbYAISbbSSZZCRRYAjLQZIZbIDAJLRRKaAYLDLJATIYCRYKLbTYABAYYYAIYLIQbLBIYCAJBJLBADbJaDIBJQZZCARACYDaBBKKITbQZYBTKbJASCTSbKBahSBTZAQBIYDKQDIQIRQRbAKLiCYDYRZCYLBKDRDaYKTSTTJbaRbYJJYYTAJRSKDRTJaIBLSKLYYCJLATKbQAQJRSbLCQYSZKICaDSYBASCRZDRIZITDZRSZaTDKZSLRZSZRYTIADRTRiTaTTKbSYaQAZRJJLaRRBCABKDKZLRKQCKSYJRLACRQRTTYKTITAZCbSQbQQJRSTQCQaKLCLZDAaIaSTKDBCTITbTDBRZZQKSLbAbIJDQTQAZTSIJbJLYLCSJaBbYLTLACZJaKJZaCQBKLRSaBIaDaQRDRBZKCTaaKRIBJBIaJTDUADIBZDSRJBZRRQSaKDKYTCQQYKKbYZJZKQRAAbaJSBZDZBABbbQDQKTTDJTSSQQJIaJLKDSJYDISaRZJQQLYZBKITQYYSbJQaIRIKaCKKIQKRbDZQSAJaKIaaIZCSCSKJICIIZRCaRJbCKLBCLBIQYRTBJJDIJCYZQJJKACSbZZBRAIaIBYSSKTaJaaKhaBDRCbbIaDKACJABBZSZaYSJTaAYSaZaDZRQLJRBACBLQSYJLTKQJLDDTbQSQKACSSYTTIQZRYBCaIJQbJBSKKBRYBYLTAaQDCbDILTJBRBDDTBQIQbBbTKISICZbLaCbKbYQTQSKATSYJSbZJJRCSKaCbTSbJACJLABYAJJRDZZQABKLQSbAQabKYABQDCQITBTTbZCBIJDBQJTaCDDLLTYbIBaBaZALTYLQYJLQTLDBDAALYJbKKACASASbZaZJYLDQBKaQRDQCaYTaDBCBRQbJaDZBLATDRbJIBJSKZARSBCKCZaaaJTTYBLaQaYQJBZTSRKQIJQKCCRSKRIZZRJJIKQTZITLSSITDQIYYJDBYAIRYDTZQaSCLRKIYQLADKZZJBJZSYQSbTaBCKbBKDCCBbbYIaYBASBaKBQRYRKYJIYYJBUDRLTZbLLQYAQTaALSJCAKQRSLZRKJDIITDITKAAJQbLJaCLJJJBKSABCIbZZAZQDCSSaaLQbJSAKaKTDQZLJLYJLTCYDAAILBaCYQLTCKaaDZASRTQDAbKDRbSLJLBQRCIbICDSAaSCCZBJDLbTQJJTaRAYBJDZaYQJAKbALTKTCCILRDKSTDZIbCQICKLSSbaSRYTRBRbZbIaJRBQaRITRSKaMCLYTLDBLTZQaQYILaJYaRKCBSJARKCLRBRTRJQSIKZZCCIaKBKJCbAISbYaTCJRbILBZRLCAQLLYhDABCJLDDZZDKCIDQSCSIAbYTBTBKTDLDTKIbITLJLBZABSRBCALLQBKJIbQDJKLZKQYRaAQACDQZRRRKAYJbRSaLLZSbDTaLRTQaZbCADDbJCDRDbYDaIbbJJRCQaRSTSBQKIICaIQSaIJSKJYbbDSCQYRZYJCJIDSZSJYDDCDBLBBAYKASJDCaZbAQDbLKDQKYKYDSBQYbADIRIBZIaILQaSaBYSZZSIRAQQBDDQYRDbJZBCbRQbLAIbKaCbLZIKRgTQRATaYbJBLC"
"SSTCCRCIZaYJYLAKYJbCDZAaTJJRJTADBQRbKLCCLRaCZDSTITDLQQKAQDTKbaAaQTJJDZLQIAQYRDITASbSIBQaLZRBTJLIBYALTCCKbLQJYKLQRJSZLTaRTCbDIZTQCTIDKLBYRDCACaTKLLILCBbJDJQLKQKBQAYLZKIZQAZKCDRZBIQTDAQDBDbLKDZSabZITaTBQTIRYaTRabZKSBaITaDAIADARRQLJJIISYJCIAaIBbBSKLTbQZJZDTSCIAIJIJTJCbAaKQBLTJTBCbaBJISYLbYLSaADKCRDCTQDJSAbSAYRbbbYaZZaISIZIDJCADBKbCSKCYSKARCLSBILbZTBQDTbIQYIKKaLIQRZJRCDZBYDAKJZZBKabThYRaZTYbIYBBAJSZACQaTCRDSZYIDbKSJZbYabIIKCIRYYCTQTabJaaKCBbBYCbaYaDZABJTTCAYZBCaIADBTJJRLLSZRbJQBDQKRKIBbKSZYbYCSKKSLKBLKSSCKZQSJASbJATLYaZDJLKaRAAQDRYQRbbIabQDZCRZTJJLYYBbbTIYQIDYIAJCISTQTIBaSaCZbTCTQBLLZCZSSQbDbBISYQJRZbLTTLKAKICSSaALDZaTTBaQBARbaATSaQDJJBYLJYZDRCQRSQCCZRaISaCaAZDJaBYITLCZZJJJaRBTaIQIZCTSaKTQLZbbKDbIYjITLTQRDSbaaYaZCQRYYJJJBYCQTYQBQKCLLCQKBAKKCRLCbCBbZTICZYSLJZYLTAaQaDZBJQaSBIBZQJYRRLZLQbLDLDAaCQLBSbQADBSRaIDJSIaKTbQRKDITATKRQTDSCASRJYTYTCLKCYTCRLTYTIARbDJLLDQZRBLYRbKRQbLCZLZQDZRIaCSAISTKTJbSDaaZRRIYQAZCaCRKRIYZBKICIAIKDYaKLDLQKaBSQRZJSILIRSASLbIIaTTZBbSDSDYYYZBTIATRZDTDDCbAZKbBCBLRLSLQaQBbIRYSIKLACBIIBLSCRbTAJIQbSYBYBQQLRBLRQbQTZRQKCQABDCJbLaTbRZIQTBaKLBQDDBbBaQSBCIIZaZCRCQAJBTKCIQKIBRbCLSZbBRAbZLDRAcbZTQCBLILSRJJYJLCSKIQBSAKCYaZAIQKaZbJLKJKCRDYQKYaYRYZabYLaYJQIKIYaLLTQAJITBJBBDCRTSCQTDSDZRbIICTbBYYbLIKTYIbBbDSRaYAYDLSIKQKRRIDBJYCYTKATRCLRYKSSTQLYYJLSBYKSTDCQJBTTJJDLSCbRTLBLAKASLTbAbSAZQRJbaIITISCRYIaKITIAZRJQaZQRDJCDYbICLDKbALABTbbbASDCaJaJDKKJIJRCZDTABaYIKaTDRCZRYbCKBaLIbAADIZLIATAJZLKLDYKCSCQACaIQTZTYQTZABIKQQIaaaBaBBRTRYLDDJTCaKaLJDZCQabLSRKbJJIJbbaSDYCICIaCJYARSAaYYIYZSIBLBRAKILLACTZBDZADTYTAKAITLLYZLSBATYJRLQYYDTbYTIQDaSADYTSJRIYJbZZbKRIRaITZICQCRSCJRYKILARKQIJaBYQKQISbISSDJaIYJhLCKCIRYTARAJBCKababLLBYbQTCBZTKSZRIZbYZIJZALCRTKLQQQZbAbQIRIJAATSbZBQSACATJDbBAKSRBJTbQbRTTIaTTKSJBYDYITYSLKCTSTYQKCbSbRIJIYCTDDLAKaBRJTBTTaaaZTaaLJDYCbATDZKCRCSZaBCAaRYSBTaKIDbSQBSZBZDBKAYDDYaLKZaaaQBTSZZKAbQABRYbCBTbQBAZJRCBBDDTaQbIBBDADZLLBTSAQTKZSBCaSQLTTbRYJaSCKZKYLKIIQDKTCRRDDCJBSLSBbZDYKTbCBLQaKSZASaBZCIJZYSRbJLCSZLDKIDBZJTTRQYTKZbaZIAaCLSbKARTCQIIBAQSZBBAaRaQSBZYDYYDKJCLARZSKILDBQSRKITYLSBQZYZYIbaZBCZZSDZQYAZCRJTRZCTSCJQZAQTBCKQTSLCKQbbKBQZRZJYaARaSARSJRbBAKAIaBDKSbCTYBJJAJSDAKYZKJJaRLAZTTJLSAQZQCAQAKJCJSSAJJQZDLYLSISIZQKIRLKIKZaKCKYCLBILZZIITTSBYBYYCBbCBCYLZKKSbTITabZCAKARSKbRASbJIJRYJYLCCTYSbbZIAATSBCDQBBaZaTDYBBRLZICCIRBabKKLaJALLYRJBTQACDCabaCLRBRBQTRYKAQZRRKBBZAbYJCbKZQJRbCTBBaSLAZLIKAYDDTAAKKYbJLDYIZTSACKDCaKJIJQQJLYKTQLaZSSRbBDCARaTKCbQCaSIZLZYQAZYaSZaQTATLDLLYbKbZBDKbLJSSLRZQYYCAIISJKaAADIaYBDDLRCYYbKJYIKCDaZbJAKZAaLKDJabICCYTKLYDAbKRaTCRTSJDaCTAIQLLLRCQRBQbKLKTZKLKLRBRYbSbSDJQAIBIKSZAZBYICBKaBZYKCCABIaQIIaASbCKbZSRZQRbRQKSBYZZRQabJQQQRBTJBQaZaBJCZQBKAICBDABbQKJADJZDKTIJBIaQaCBRSKKCBLKQBRBBZTBBBJLYZTLYRBRLAZAALTATYKDQYYACKbIRBaKbJAaQAYSaJISTZZaTBKCDCIQaTLLIYTRYDDZYZZAIQSLbCaJbKLAARDLJJbCaTRSKLRBSQYbJLIKZRQaSaQRIKabBYTZQASSKaDAbIaAZTATLBRJQBSYKJRCZbKDDZLSaCYSaLYTTZBCYDaRITBaTDJZRDYYQaaSDAIYJaSbSZITTKZCSZJIQZZBYBDBQAbAAYALDZZLCZLTKYQTSJYKILRSYRbSaRALaJZAYDTKaaTYCbLZABDbLQSKaADaLIARJKLBRZTRSJJZZAaYSbYABKATZYZDQARZQRTRDDISSKZSRQQJQTIRDRZATaLTYYTCIbbCJTQILAbKIILRZTaSaRTTLCIZCaDZTKLCTLTLZKCBQQSCKRDRRDLRZBYIYQCZASRKTAZLTKTaLDKLaRRaAKDIRAYZbCARDDJZATTRIKaJaRAaRJSCYTJIZYJQSAZAaTAKIbBICYaJAZaaaYDYSTRKQCYSTTZKbIJJaSLZZBCCRQAbLYaIZSbBbBTDIYBSABDICAQZLZZATRDIQCaZYaISKJZCSaQbQYYaRCYZRCBKQQLLKDAICDQJbYTLIBDBLaLZbbQLALQDAABTJZBYAaaJKJTKITIAAaYABJALSLJabCLRBbTIYRYBICTZDKQbBJQZZZYaaLJTbAKSZDRQRZCBKDBQDTTYSCRLIKSCLAbDYJKIDLabRSQSRCJSIIBIQYDTAICZTSbbAZYbBATTQBQRJBQCSRKDZBTBDJBIKbDDKYTYbABaBDbAQCCLKCLbRbZRACDQADaDZSRbDJCBRIDCAYBBCZbbBbAaLSbbJYDLaSZRAYABYQZAKBZCbALKQTbKTYSLQaLRTZDCLBaTbYaRaIAKLSQSaLaLSQQYCZbJAabJBACLDTLbBKBTYIQLIJDLRBQKQBCARTBaAZYCICDATCCJbYKLICCTKKTDCYYCCIKLaDbDCYITRDbRYQYZLBDLIRbBRKCLJBbaJQIZTJIKCSIQRSQLCYDSIRAYASQZLaBbRBZIKZYCKSaTRZQJALDQaYLKCDTTKAJaCZaYQILJLLAYbQTRJBDRZAaTLTIJQRICZbKabKABDLYJKSYQASJSCCBbLZKRCJabaCTTZRARSRbTYKDQDSJQYKLaLSSDCDIbTTBIQZTIbaSBKBaaZLaIBAIACLbJaAaASIQAQRbZIDRJJIKBKBZTYTYBaZBALQBIDbIbAYTSDQBTTbaKJJbRAbCLQCSZbCLaZSDBYDYKBaYDBTLCQICJabISDYAIYYTDYKYQYADDbYCIbbITSbYKDRCZJDKJIYSIYaACYBQLAaRSLDLaSTJTKLbSSTYZKKQBAbZKKbBAJTbCATQBBLLBYIRbYTTLRCTLbBYKSKJRIIQKBaZbJJCQYYTCALLQCQYADYSTJZDAILRQZDDDAIJDCSBCLDaASALQQJBTYTbDIIbBQBLRZBbAaZYATYYYKLQDIJbJabAaIaZbaALAZYDIDQYARDRDKCYICLJQCKSLJbTYKAAAbKDZZaZSCYJBCZAbLYQLBSaZIKBSQIaBSSLAbaaIRCDYCDYZSCQCTSBQKJLaJbKKQSIRAJKTaLJJbKbBKLBKJYQTZDDDbAJDQBTbLQIDaZAIaQICBJaYTaZAIALTIDQDTKJTJAbIAYbYDCTJCSJQRRKYJYTBKbAQZSRaJTBRQIDIBbCTYKIAKADZBRKSAKSJRIDRCTZSbZLbAbIKARITQBabBKTabTAKaQAQJJAZQQLAKBLJaCQbQICBDZLAIAaRLBYZaJRAJaIQRL"},
Embed{96,307,"IJQQIYIIIAKIbKIKAIIBJLIAYIZKLIIYCYIYLIIIYQYTIIIIZKYbaaaQLaaZaCCaKSaaaaSAaZaabaDYKaabaaDaaKSaBaCZQSBaYDKTCBILKYLRYDaKLTAaIaDZSCLDKYBRICRDIaBbaYBbbTABLDQAIBSaYRDKLAJbSIBTaITYRADLZARKDTCZJaZBSBaIKITIKKTTbaDJYBZTRIBIDQRaDYDaKDZaZBQKRRQLaQDAIaDCDIBDaQITRZYaBDBKQbBQJRQLTbBIDTRJAKTYJaARICIJCRTQKYTBYYLAZDYDaIbJCaSTRCLIBTITYJTTJCAIYSaDbBCATJYZYSKRDQCCTDaRJTbJSTCZZIKRAZSBIRZJbYSTTIZIIATDbTACaQDTJDYYBaJICTYRRQYLCJaBAKYJASaTRSLCZAKRTLAaZLTDIKKDTJYIaZATARYZCBSAYSbbSCJKZQIJTDDJCITSKZbYJKRSKDaYILICKYZSKCLbLABSaRKQBKJQSSRQLJAJaYIILIRLKDaTYBRIRCDLSaIQKYDYSZQIRISBbJJYaYBRRYaIZKQTQbZJTAKBKTRBZRZbLQCJYYTSKLCCDBbCQJZAKKaISLTIaQRaRCLaTBITYIZIJRIaZTQbaQSaQLTbARBZBAKLIaBYAKBSKQbbITabYASKKaRDJSBLTJBYZKYZYAZRCLBCJBSaSQTJDLCQRCKaDRSDSbTJbQTSabAKDSDLJJKDBTSTSJQSIIJbAZQJSZCbAJDZTQLYKBCRLQaSAKbTZISbLTbIDKAJJSBASJIZBTbSLZTQLDTSDIKYCAKKYRTIZISDISYADLYZTCDaLDZabIYLQQJKKJZDCKBSYJBCARaSIYYZYCQZaaDaLaSZAaBZBJCaaKbTbQALQCJQJRLARDLSLTBAARBBKKDCYRKJKKIQLZbYKZCIQRYSJJTJQJISBSYRKLTTJAAaaBYTbBDTaBBZBbQaaCITbCKRQLDKIJLCYSKSSZIDLKJBIAKRIYYRTSbDABKYbAQLJDaITZTQLCAILaSKTCQTQLTLATRKBKZJYCaALKBKZADKDabSZKRbZDDLQbTALCQRLKZCZKSZQQBQBKKYaaZKQbaJbRCKTSBJRZIQZQRYIKCTIIRDDDTCCbaCQIZaQBDbYADRAKKBSZYCbbKCJJLRaLbDRBLBSaaCSDSbBYIDKYKQQZRIKZTYTCYBDRYJTRQabLCJIAAABSLLYRCRJIaDBCIJBSaZbSDQaRIKaTDBBKIaaRbYICJQLYDKSYJIAZaILYJAQYICRBTQAaTbITRaKZIITDICQCSZKIJbJYSRSDQZCLKQYRbTJKLCAQDJRZZSaAaRKQaJAKBTKSRaKYZLTDYTITYQCAbBJBbYLaZLaSbSLQSKICRDKDQRBYCZLZCITTKTYbJYADYATYaBBYDLRRZCbZCLYQaBKDIQZbQCQJAARYCQDRDbRSDbaZCCIbQbJJTQBCJISaBbJKTJYTbDQJRbRSDIKBAKDBIZLJDRAQKAaKLAITSZJJaLCAYYCbbCabBaDITKCbCLCKZJLSSBRaCDBADKCabaIIIBDRCaAbJIYRLRJRKAaLSaITIbATBJCJYTYLQBDKLDJKDZTSYQRZCBZQSKTYBTaTBIDYBQDRCQDLSKbIZKYRJKCBJAZYDTBCZSIKAKaALTKLBTIBCLTATILKLQYDKbJaITBIAbABSRZSSbKLTYABZaKSRAYAZTYRCZbYJSbYTCRKRaTCATQTKCLDZRRIZLLQCJARTTSSQaZaLKJDTbRSSJBYKKBSQbaDaZbRKLDIKQJDIDIaBQDDAQSITATZQJZAYCSJKQTZTLBYQbAYCRKbBBABZTKbLLZITBaDDJYZBSTCYDaBJAIUaQCZBCJDTDSLIBSJDYQbSbZTDYKILRYLSTZRAQIRRKKJKBBDILSSbZQTIALSCLSRZbJAIQKALIBaIRbLDDYSIRBbAbIAQDBQJSKLTJIQAYaJIAZBaKLbTLAabLCZACDbbACbAZKJZQJTRBZKLSKAZQBCSKhbJQSabaISAJJBDRZMZBLIDYTaSYQYTCYZAJZJCKCRCaSQLASILKaYYLRSbbLIQTJaSYSBDICKDIALbBaISaZAZCASLbAJBCQbICBSCTRJSjZIaZbZZLZTJLILSQSKYDKIRBALBYIIJITAZIYaTbQZSLRZaYQCJDYAaRiADLLRZCKRBSaZLARTJDLCSBBZaTIaQYRRYRSCbDTBSZLSCILbBACJJKIYBKIBbCBYAZDYQLKDTRCUDACZJTCYCLDATKabaKQAKbIaZKBIDIaKADTbDbQaBBbSTTDKbBJSQbBLRkRJbTSZZRIYLZABRIDDSAILJCBRRCTBcCLZJaCSbbJJQATSCRLRbbTBAaaAAibZYRaaKDaZYSQBKYYJLKQbKKSTbQLCIaDLLAaDQZYYEKTCRZDYTSAbYaLCRQCJLaDCRKgQBCJYCAYJDDaKTSYbRBIaZjIDASYBTjYLLKKcTKKZCAKYUbBZLaJAaZITTJJASEYJRTQcYYTDRCQaISJQTADIMbYTSbYZbKRIDZJIbQRKBCADYRMZKQRLaKDBQIgCYAESCSAIZJDRTJIAbBZTSAaQYSKTTjALLaSQARYQLIBRbKALkYLKDLYTYQBSaIATZaEAJTTSCRLDQMaAaZJBBUBbaIhRaBKZIYgCJUSbaLSLbRKDbIDIDQaKBLZLUCYAMYBLYJcEDZAShjIYSEATRBIMKbYRKIRIBgDIDADCJJDYYKCKQTYKBJAjULUQAREaJabaCLQTDKbSIUIBRIThDTcZRBKCKTELaQIagIbBZYTjDCbDCIZABZSjIBJKTagBTKTJZcSYaZQRKYADRKCgBCCcTAYYITRaKYDUYZYMKYCYSTLDTTbTCSIBJYKYYiRbgKhDUQRACEZDabTEQKbTIRTLKSCTZIbZEATiALZUCQQLbJTicLMKCYKZIAJEibgaCDLDaCCQIJahYZbSQADScLZRYTiTaaQBQQhDDKKYMKSLRJMTaJkcLAALASYRZBUKSUAISBRICJjKBIYLLIRBJbKBiaZbCQTTSLDgDTLbDCAcJbRDLaLIAbUCLCaQajIATaiRLbTaLAAChIJQiaZgiQQZcBKSARbIJCARgMDKgAJbaTZAjRKYDSEaRKcERhTCUDKRRhaTYaDIhbRYaaagahJbjDKJUKMTRLYSYSbSEIATBEacQJKQAKAjbLZiILUSSjTAZKKRQASJhiZSTBBbMDcZMRkEYZhADSRASUSgJAbbQBBBZTEKMYTCKIYJEShhYMaCAbYTJKTLjbiAgUTEKkSLSQCaSSUJJjKBBhSYJTbTZJiRbATTagSKkTCJEcQLkZhAhJKCgTQDJBTDQDBYJCbEcQDMSbEQRLcQgYTMaZhaDiTLCChDMMTbbBQaJZSCCYBDMKEIaCEUChUbgMIkYLJJCKCIkDgTRhaQJLgTLLAZbTjaDLQaYSILJBQSURbQYhTUBBRgJgQJLkDchDEJEUBTbZaAEcjBDaYaSERYSIJILTTYMYhSLRiKCRTYMikhgSKgbkJELIQAAiDDJEaKDIYijgSkkIijbRIUiLQRSBLbRjYIaYDLLRSTJRRJCRZJMgJKagacERDghJZcQUCTEISCYhMUQLaUUIaiaQCDCLCJZIkKJZTJCbSSLciBDQTIBjKgTERcRZDgagkEELRQJiSMLAAQJIJSKbkRjBbDBZAKcUJbBSLjjjSgLSEKMcaKAiQiKRTBRcBaZkgJTZLCbKTBahhYRQScbUQjUgUShIJIBkYiCUZkSTUCbcBBLAAgYYYJJZiYQggYYacYEIjcLiDJEShRiZQUKiLLCKSZjLUZcCYSRUKiAjLCURjZMKRLCZckaZTIZgEAUUkQMjAMIhRijcjIaYbSgggLDAADYTRJEJCRTMKBhSRBKBBkbgCgUUkEZYSUJbIIZiQhMhJRCUgUQLjAMhMTJjUkAUEbaiijEaJRjCgbggQaakRaSMQYQkYYTMbbcMSEIjgiYLiLSLkQhThhAiLiQBCcjSCgTAbYbLaJCiikCYkCTMKiSZYiiBEUMKAcSLgbMJTYUEcikbSCShLjaZBAbDJkYIBMDAMgTTkDIYbJjcgiCQjjQgkaZicUbhAgATAKKMjhbCaMUaCiAcQMcKDYTCRLQIJjKCMhhChbMjikgLgjgIbJBjJbiZICAQBAKUAbKTQLEEMjkRhbEjciDTcTaAiRhKaAbKYKBLiRJjUTTbjKiRUgMMMMiIhZkcSSUjZEjKUbkjDgSagkDcCTCaMZQYZRYkDaTiDDYSkEEYJUgDIkcBJLbBhgSThgAhSKLSjAIgZLjDZBSgjMcKEJQJKIkSUcMAMMQLSASALEJjTcIEiAjjKABYYJKhbaBUEjhhaIcUZCMKYEKkbZjkaDQBQCJJSUjkQAcBLILDhjEQCRiDCULTSARALKcMgbhDIIIBZDDiiUakcEijQZALgcUgBZKSiMAgZjQUYDCCEYALADbYESkkLBbYhhbAEMUhYIZJbbBgCKCgDQURKbkQbKMEkEMUCDTgkJQUjKBRJciDkCZaEakYCDTRDDhQASKikBccMRZTaLcjbjYKUjKjiUkSZgBYUUEQDQKQDLQgTMZYhkBbJbKLCCZgkULUZAJKgaIYcQbTTTBCCDTRDRJAaDLZIIQacAgSKaaDJSIIDSUZajkkLTihDUUIIbbIBDkjcBIcUZhUZSBIZMEJEDTKgigbJUIBSRYESTIabSSIYBBhRSRJARACcIbKEZQgZDIDYCaKBYUIRcISKUYjDiZLQjbZDKabQRTCADjBaKKIcYDJTZTYZALKbQBBMURZZDiYBSAAISLRcBgIQhEJAZAQZBZSjSMLbABZDJhEJiBZUKigSAgRRgIZiTLSiicSJbShAbQhKZTEDLBLLjTkYhCAAjQUYSILJBMQJEJKBIkiCICIaLYICbJRRQUSBQgLJgDBRAbTKEAMbLLUJMUIjEDbZYRaBIJDkIDERBChBbILbZaREDTCQYJBRLDgKCIiYSkkULAgZbRMLjRYacAAIAYCRETYKBaJRaACTYICEbTJbLIBRRRajTJcEIAMJAQKYLRZKSLKCZRELaLCAADBCZKKCDCLIRjITMYMBKhJTAaEZKLAihiTCSRDDbCMSURBABCJJJBUjbAQZZJaITCMLbTJgSYgTJYSbMAiYBSSbbIjIjJKZgZSZQQaUagRRRZLZDBaQKTECZYCiTKDbIEAQTiKjAgBAMRZDQYLAZTMAJLBQjDIaJDBQDaLYSBaBiQbAZMbCCSCAaRAbSgaJCbcYTLDTSZDaaZhJgjBYBDLTiDDhhILbSQjMQJLLhZQQbQSIbJITIDAMJaTJaQUZTQAAAYbDaZDCRhZAIQTQAcaJKCQTaJZCQSSZBDBDJbYQCkZiLYbLCAICcaZBDZgDDZiQCDcaZbRALcKCBBBZRIAhZLjCTKSDCRLbASAbQLIEQLbTSaQaAAAJRJZRbRDRRbSKZQBJYaJYYLZJKBAURDRQQBcIgcRSIcJUYCSBLEDaCTYaTAIQLYhKIYKTAhDjBBYLJZJbaKCICbYTJYRKDBTADTYbLAaSTLDAbBIZZJYKJAZSMAaJCBbYZQYSTaAJAgUCCKLjCSJYZKYMJZRQiRQIaDRQQSbIbJbaJIbaCBDBIIJJbiSQLQbSQCTLZBSLbJSRIKjiBUZKaLSiJAJbZQSAicMcLYKDIKagIbLBLSCJLZKIbKIJUCDRAAbQQbaLYjZCTJTIbQITQCJQQCMQZbSIDAjMADCCJLYhBQBkCTSgDYRBQIBTZJDLAAbKLTZYZDSDgZaghLLYKZRJIYKADRRZLCJCQAAbYCKZgACAYTCRJAjhhTQAQRSSLATYJIaCaDLISDEASiLaZQjZKaBBIQUYLbILCKSLaLZSQDCaCBacLZDTgYhMUCYKKgZCQBDALZALYDabJLTaRRZAaCLJCMRQZAJRDKMLDCCBaBaRSaSQKZJBaBADLcIDTRZYRARLZjhDSIZSYJBabYbTDYaRQRDLJISaRSTSASLSaZhJBYAhQbIUajZQaTZBBLCTTRaSYaZDKJKRKTSTIISLDaSiaciaZZLMaLIRCTISTARhbYJJZJZYQJbAjKYaQSSaDCDTaahIBIBDQCJDhZTYQLaYKaTAIAAYIKbRKJATbKSBbJkjACYKBYTQRDCYTIcZiBZbJJQCDMbIYRTQgaYSSYYCICKZZBJRgRBDCSbRDbTDTJJLCaaabYKAAZbKZbZIDaSJDQZIbaRCCjDhYTDBRcZRbEAaRCBCaBQJKIBCbKQRRaLYICgKZQDDTZYZICQCDZJAJRDAKEBSTDRZDBARLIIDSbBbDBLTJYDBCDIBYJbABSYRAJBaDZiaQaDABiaJIQLIIKYASQZUTTIJiLjSYTJDJYJKYJQCQDKBTALTZAESbTLQSYTIJSJIJDZSCaBCQYLAYDcCSaLYgLLKZJBLTBQIRDZADIZBDBTaBQCDSAACSaDJDBQYSTYQRQbBLRbDDEJCbSQAYZIBMTJBaJTLIDRQDBKQTKSZiCRDQLYTRZRDaKTjQJYTZTKJJLQYDZbADICSIhAATICACbZDBRRaLDIJabBYakYJCaYRcCCaZBTBJDQbQDaaBUASMLSRZCDCIJAJIDCIJRLLjKARYQKJKSbLIRYSZAJabAYKARbQRBaTRUYSYTbbRCCYQCQZSZYZBYJjZTKDQKZaZZDJYDZkKKaCKbaTZIYDaSiLERSCKaAbaZBLDBBaSKZKYBCTLAYLJKBRBARSSCIabDZQUALhZQLSDBSRKaLbRBSZJCUYQRiZYCZYJKYKbBKJKbQJaZBRKJKQZDTgAbTSYTCDRTkRKQKKTYSYJbQKabbbLQCRZYDYDESBaaaIIaJYJAaSQJbKRTIKgRQZTBATCZBjgLKLaJCCJLRgQTaKbRYYIKISRZCDDZBSJTLRRDTZIAbbLSCIQbREUBaCIRaSARLSLAIBQRYjRQTaYbSSKCaIRbKCTDaLaDRLJRQLLbYYJZQCTZICALCbBYCRbRSAQRICTYRKBLMKADTBZbKSBJJKZKLLCYIJILaKKKgLIRZJSaYYRCDETIaQhACRBRRbZRbjBbQIaARQKBBCYYTJLTBRKSTKQLULKCbSIagaSTKJBZDBTSRSAbBgLaJSICKaQbARRDaZBCQBJhCILKTCBTjIYJZTDbiIJQbTJTCYCSCJLQjLIBbTSRCRYJaYRJSZCJLACbbQCZKIYAJZRIBZACLQISZIZgZaLbQCZADJAZbYDBbaQDBAIDBJaSCQJhaZJIQZTIBULKAIJDAbgDTZAbIRCBbSIBCAYYCCCIaALSbSDBhRLZDaZDRRELIYaRRJbJaIaIAaBRKIQRKaZLKIADQKJDSITaQBDLLRJASCSBTIaKZBTIbbTKIJJAAbaSBYaRLYBBcQQQAJAALLIaBSLTASQLACZZYSYAaRRLaDALKYIQLRJbQYJSSCKBAZKIbTLRQDZBaKMSIKZBLJKTSYDTgCQADTZCTDKAAKBRDLAKQYZSQTYTZbbYJRJSDRaQYbRSKKCISRDAQKSbJcaRALAYTbKJRSZTZLSIaRTZKKQYIBAIJbCJIJRSAKCKDAASBbBSCaTbYZZBaTATbTRaJIaBJRbBQSTIZSKQYJCSERbDIKZQCbIYIRbZYICLRATKLYRLSSLaTBaRQSKZTBCLASYQSbTDZZASaLaCLYUZDABABRLIaSKTbZAaIYTLYAaTIDIKZQCDRQYaIKIJbQIZIiISDQbIIDQTaLAQJaIQZJSbRCJQJDJJAQBZbbSQKETBACYQbIZIAAQJCLJRQIBDRCUbLTDbDRcCZIJTBDLSBYSQKRSDLCKIZAbabKCAbDKIDSLIRIZTKISQQAQAZLBbSbAaaUbRJIBTJTDALKKIBibTIALQTDJSKYZAIQLITagSDEKAZJKIQTAhZTDRCIJLQIbbZZbIYBSaCTSaQQKJbYBAbZJCJSCbbYbJACbRQCDCLaSDQbQYCSILLLILISIZaDAISIBBKbRKZTZaABTLTKCYTIQYCDBJIYIRYQYRZZBCAZaTBKBKSYCTAaSDJQBDRKRRQLCJDagDbaLJKTRJZYQYKBKQTJQAaRCAYDaLIDaZQjRAIKbSJRAKaCDRTKbDIZRJITQZTKRKADJJRaCSACaIBQCKDbYDJQDJKITTBTTQbCTJaBabAYRSAKJbaISQQAbKBCaZRQBJQIRLaRJJARaLSRJKJYCIDaaBYDQLIDSKKBBTaaBSYTDYKKKAaLSSJJJCQaZaaaLbQDTTQYKCSaRQiRaJBATRBYDKaLRLBJaaZYACCDITaAZYIIYSDaTQTZIYLRRaSBLTCSIDbSDbbRYScCYRIYJZSQAIBZbALScKbZBbSKSKKiQAYZJKJTBAZBCRBbQRbYIABKaIIAQJaJCICBDDbbaRJTCTIRDBJDTJJbYBAYYZCJQITaaJCISKAJJRbSQTbZYbCJQJSYCIAABAbYLJLCIYRQJKIJJaTaQLbCRKBCISRBLQLBYTCRCTCCaRaLIZCTaLbBKbbJYADaYZRLCTbYTAYKSBZZbCDRRiILAYTKAALSKBYJCbQIQBKZYKLBDLQQYYAKLRLZSLTSbKYbYCICAKBCZSZKbZLQbbQDICABCaYSTDARSaYQBZQaIBaRTYaLKJbCCASbJTYaBKKSSCLKQJaabaRYRDbBCaSbKKBaACabKZDAZIIAaKIaYJRaQaZBIYKZbbIABZhCJCCYALJSLCIDDSICRRQQRbZRIQZZRbQCJIKRbZaAYhAJRQLaCIJISaCCYTTCRCCYLBJZLJZDBbBbTQZIIBLQSYaDDTJAKbICAYbbRZTRYIAJCBJSYSRSCBJKDbJDZQIZBaATbILTBDBTTQJQRaYSSDTTKBaRLCKDZIAIZaaIQRCYgIbLYCZIIARZCJAbRTSbDJTKCCITTJRYTRYRaQYKTSaBTRJSYIaBKbTZRIaCBIALJILBZAAKIKZDDZCSCKJSJLAZZJQELIaCAJZRJASaYJAJARSAABBJCYKLAKTKCRCLCIJQQKDbZbBYITJRLYCCKLYbISBaRRLjaaDAQTIZaDTCSTDAabISCIKLBIabSYCDbKjILSRZBITZTCaKRabKYSDKDZDbTCRLaISQYASADKIIKZKJTQaURhCAZLIaJAaYLLQDSTbKKZbASIbTCRZYabBQBBAKDABJIaCCLCbSRAQbLIBLIBAKSZcABLDYCaCIbATIDBYbZTKCRSJAALaYKJJLCbBLLCaIRBYTRQbCTSaQSbZDLYKSSCTBAKZTDIZJBAaJaIaKLIaLQbBJAZQSJSRKaEIBBAKLJYAaIQAJCTBJTaBQaTTTDRZKBYKJLbaDaAZABLaAQSAQLYABQbIQLbAaCZSCZJBbQBSYRDDjKSLRJRKJLZLDYRBQBYYBAbYCYZTAKKLSbRLLZCQQbLBSSDYBbjRRDYAKMKQZKLCSRITYJKLCDQKaKRLLaSKRJLBBTKTTTbKJDBDSSLaaIQCKTSbKAIIIILZRYDJQRTDIJTaIZQIACAJLAbRKSQYCJCJbSIRLaYDaRbRDLRSaQbBBRRQZYQZIKRTbKDKQbaLCbCAITaRbQBbRaACIJYCKZRaRZaKDbgBKRTYQTJQJTRAAZaRZZDLTbYJQCCTBTSQCICTKAKIZbYRLKbbQbYJQKbTDZYbACQRJSKQAaYaLDTLCAZTaKZDbKbQaZCJAKYYSJYZRTYLCTDCTDJYRKCLYRaIBYCSKbTQYBRLYSZbLCabBJSAbJRTSQaaDbBLBAQDSRSZDYCIbaLKAQCCQIKQZZBBbDRZCAJKRABKJSICbaTJQSISSQTDbZDZBbTTLTJSaSLSKALTSJJSTYQJQCaQRTLLBYZJYYABYTTBJRJKaTLaSSYCbAaKLSQQDRbRLBLKITSSSKSaLBDALQIYSLbbBDISbALaJDRYISDLZaYaAAaAAQYIDAJCSCTRbZZJZLLYaARYSLSRTDYCZbKIIDSJYDYYbaRLLTCK"
"SQaKQTTABDRSTSaZbTYRZJTSSYBBAbZbAKRKIYSZYBRKSDYZbbKSIZCbTLTCRKBYBRCLSQLbCBBJDaIAZKJDCSDIDTQRZTYJbATISbKTaTQZBSILIZRJaDTSYbATQKZCSDZAYASBSSKLKbJYRQJDbKQbYTDJKQQQLBQSKbZSQCCSLZZDISLKSAJTbaTYbDKAaCQbTbYDbDYYbQRQBLDCCaIRAYBRZBSBJISYYCIZSbbbCRRabCQZBaZRITJKJBQZDYBSABSAaRBTCDJYBZcLYaBSbKLCZTTAKRZaRZBJBaKZTLICQCCSDCDDQCKKaLJCALYRaQDKZAaRCZCJYSKCZJTBZaDDKACbRSJTIYSTaJYDZBIYabTRAJaDLSKRBCYbCJQJCZCDYbQKKBASYQDZIJJQJIYKCDaKSRKLbCCBQLDRZaIBAYYZaabBJCSRRZLQRQDRRLQQTTaQQCbDaJCAIZRZCATSJQZQZTIQRRAKaZaKbDSTRbYDaLKbZaIRTCbKIabIQYRaILbBCaRSBTIIQZRSBYTTZCJAbLRJKKZCSCDIKADLSTYYDKRRJKIZJALBJQIQSSTRKDYISSBLAbBaJYSKSDQZKLITYbKaAaLKKSBSAbbSTDCYKIZZQRKIRIKBYIYDIDKZKbKYYLQbCCBaZTRbJYJSYQJLBbSJAARJRASTJaQBTBSbLZIQYYSRbBQKbLTaJaABaZLARLATDJZaBTBUJBZbaADATDQAIDDSLIBSIIIQIJZIYRDDCbQBBRZLCCYBYLBICZLZSLRBSaAYbQTKQIQKRRSZSYQbRTLRaKQKBDZTTAKZCLaALbDACYbALAaDAZABYCbYJIZYQKQDILQDTSATLQQBIaSbJDQSBTBBSLLRCKIYIDZAIDRKaZbaAYKZZIYARZRSDbIBDQAQbQBJbJQJbKBQKDCIYTaYISDLJRTKRZIaZZYKAQQSCCTAZbBBITLTTbAaAQAbTZQRbCZSbDLTYZCLAIDYDBQKJDLLbabQACLRKRbIDRQJaYaALbBSCSRTYJJSAbAQaJYATILBLJbKYRSKCDCRTaJTJaKDaICSCCZZYCRaLZSRTYBISbRKDaQbaAaDbDASZaYbRDLJAKDDCSIKADbDCZZJLSKTRQULYLYLaARbKRSRITbCSSRTCTCQICSTbZbbZQSCRSLCLBLDTJRRYBJRACKJTYIIBRISKIBKYASSCJIDJYDaRZaCIZJJBLJLRSJATZRTLCRADCZKTTTLSBTDIRJRKIaKZSZYQAaRaILZQIZAIZLCCBDQTbYIbTZCYTYZIILAZJKCYbaYJZKTTTDCZBTZDCSQCICYaZBARYaZZQQBLBKbJSbLAYQRBSbQDYZDZTIQRQAYKLLAIBJaAAZQLaIJTRDbDLBLKAIYDIAQTSCaSRTKLaBDbIICJaLLICQIAZCSKbKITZAbKAYRJaQSAZDIbLLJDaaKQCbSTASSbaYLLaRBSbJZKIaDYRbQZCDSCCJYASYaDJKYDaBJSaTZTBZYQIADbZZQCBADAZQbIZTBBJZRQBZbDDRBQRTYbSSICBLbRaBLADRZIaACQZALSSAJRALARRJJDYbADRCbbQaaQBZAbIALIaYKZZSCTDABaTSbSCbSaBAZCALRaIaTYJCDRQaJYDKICDJILZJQKCRJbTLBKCaaaJRKDLaBTbJCCaZDSSKSTbYITRQSbDYbQQRTJSZBRJLTICTAZCJJASBBJLbYYIDAbLLYJJIAKaZZRBIaQbBSKQCBQDTSILAAAbQISIZBRBRZbZaYKQAZLbIBYKAKZYSabJQBCYbRAJRADaDATaDDLIACTJLCQIQKZRTZRTJSBYAYITTDZSDDRZAJBRZBAQRBBZKSKKBASLRKDSSBKLTYSLALDDKJZaTABaIYZDZSSLJBAYZZZDBJQATYIbIBaDbZQaQBaZZQIDYaYCDYZDbIICCIQaJQaRbYJDaYJDIQDKBKZTYYRZTYQTCARAaKAAQKKRCBTBbbZTKBQaKSDCBabbAAQCTZJSZLJADaDDSIaILRBBZJKLSAZADZBYBCYaIYRDDaLabJZARbQJCADIIZZTQaYDRJBCSDRSZRbTIICaKZaaYLZYYRRYQYJJCKRJYJYBKKKKAaaJIDKISRJKRRaSQQZZAAKJJZLQKLCIDCJABYQBDDYTSRLJRZTaISZJZAaRSIDKCKYADTDCIJJCSQCTbaBBKYCaCaQZDYSTBbCLQTKRJKaZABILSZQJDJBRJbQJjCTYAQKbLaRJYBSYAKLBLbQSDbLCLSCRJRBITZQRAaQKIKJLLQSYaTQRaSYTJJBIaYJIaZZSbLaBYQLKSDQbQIIDKRCRSSICRTbYYLQLSADabAIJILICSQALZSRZJAJRYaYLZJaBaIDJBCZBQALRACSIDCKYCJBbQRaLabKIbCJbAZDKaSCSIBbabLRabRCKSKACbJDKKBJDABBTQIJDCbbKDCSCZJRBRKJbZQDTIbDZCTKBLARQTIZLKZZZJBSIKJRaSCAABZaDDYSJCARARSZaYRaQYDZBCDbDbAADQTLaRRaIbaaJRCDCRDbQYJYCLCCCKJKQITQRKDJaDQZAQLBSKITTLKZDbDIYTLCJSaYJYYSCbIIQLASCTBKSLLKbTbZRDZTQQQTILCIABbKYZRKQSKLaYTBaCABbBQRZSYLbJCYAAQICaQbKKRaQIDQYTbQYJYbbKRJSQJCLTBbITLKTQTDJSRLQBbBSaBIaRLZSQaIDASRJQaLQTCJIDKTBDbJbSYRJCTIICAISaSIDJCaDbTJRRYBIjDYLBTSRBKSbaRRRCTJITKBbDRQQbBJBCYKQZbKQbaKIADDCZRRBYAZIAAaZYBKZCKBKTYaJZDYDIAaSQDALbTYCAaKJYSKbCZbILIKBZTKbIAQCAITZBTJKLKAJbSBYICDYZZZIQTLIJLaRKBQBDALCTRCBTTAQKCCZKaTbTJSBJaYKBYZTTCSYCBBDQbaJIYbCLCJDCZKCaCCKRILDJBbIbACJDIJQYCSJQbQbCYJISYSLRKbCabZTTZBJAbaARYAKJDCBJRSCYaSTCCKCBKbaCZDbLJbBRDBJaZDTJaaZCCBbRABKKZRSBIYZQbLYRaBZJCTAaQIIQRaCYZSRaLABBaRCLaaaBZQKbQaBAbYbJbKZADQYZIbaJbYKSBSQDQIDDSIaaaTZaRKKCaIBIZBaaCabLSRBJJIYBSYSSRTKKCCSLLJSAbSTDASKLIRJLQLCCbKZLABBaRYBYZJKLSDYBCLTCDCCYYRKLRYJaLAZCDYAbBIIACSAZTTDaBTIbYSZTABCZZZKbKSbYBaIYCLLKCDBSAAIJCLbDLBDYQQaZIJQLDJQaAJaaTIRBSKTITQSJZJJYDAYLTQbBSAALRCKRbCZabbBSTIIBLQRLLCLQaKBKIDSZZKaRKQBQShYILZRTTCQTLIDZCCZcRaBYILSJQTJAYJZAQZKASJAbZaDBbCAADSTDLAZTbaJRTLJJSRbJLYCBADBIYLCATDDRAaLaYZTQBTaTSTAaaKSTaCZJRaJTCYIZTQDBbIQbSTbCRQBIRRCYDSICIJCBDYbJBbAALKSSDSRKZIBYQRIaJLSDLaKSLTYLKSCSASBBaJTJZTJIAZZRaQDCRCQKCJYBZZZTDabQKRAJaLZCBKDIYLQIQJQLaQbDDLYQIITZZDCSABaDQKDaZLZIIZKRTDKSCZaSTQKbZDCDRJaBCQZaIIKCQIKaYTCBQJBQZAYZLDCaKKQDbRYARbbSDZCaLTQZLSJLLYIIDADTYBKaaTACIYJLbRKBQLBZLBZCYIDQBQYLaYTSJLJDDaYLCYSYKDSbZKYbTJRbLZRBCIDKZAQJBTSaYAabDTTATSKYSCLKITSCTLDKZBICaZLRQTKbRLIbBRKbRJaTCBYJTKBBIYCQSCLJDTQLKYTbYaZQJQLDBaCBQCDBKaJYSJAaYRAQDSIbZbBKDAbBLQYLTTCKBSSJCQZDZTSJYajTJISRBSJQSSTDAJCZDLBZJSIbbIBLCRAKCbKTbBQCCZYBYCZKQbRbQARbAbYCJbLZJAaDQAbLABARTCZSZLKABLDDBCYIaAAaKRIQILbabIBbRRRaKQDDIAYaBbDaDQRIKRDTRRQLQRDKTBJLLQbKLRSbJQRQQBbKRKJQCBAQLbYKaZLAIZSDRbJTTJTACZLLCQQATAYQIJJSZKKJbQBLACCbLIQQASZTRICaQbaBZBQLQIAJAKIZaKIRIbTZTBLYJaIIBSbSAKDJIBZYKZTJISaQYYTDJLQZBZRADIaRLITDDBAaYDCAQaAQRTZBTRATSYKBYYQZDCJYTZQRbJBTZKKYZBQCRSJDLRTCARaLQYQZRbQCCRSRSYIYBAbZDbTQBLJCKYDJLRbIBQJKCQYIKTDJAZJLJBaLBYKQKDCTSDQCDbBaBAJITRSDZKQIbSSRJRIbbQSSCDQbRYaLDBYIBRZLDSDBQBRJDLSRJKYCAbaQDLSYLZbDbSTTDJKQKZKSYBJLRYTBCCaCIbLIBIRBRZRJBZTDQDAQZIbSbIBZJQIKbISabADZbDCaBZDKZRDYSbTCJIKAJBSSBQBQJICTQQIZTQJBYQZIDAJAYDTSBZBKSLKaZSASSLTTIQLACYaJJAZKYSYRTZYIQbIISQBYTBLCDRQDLTSaYTBYDBLCaDJRaCJAKTCJaJYSBbaJbSDZbTYCRCKCJDZSKDYAQDJCTQaCSSLbCBbDZYYSZ"},
Embed{97,310,"IIDRQJYIIIIIKIIQAKDIITKARYYDYYIILIIaIILQYLKIYILYQSRDTTTTTSILTLTTTTTQTTTKTDCLDTDQTbRTTTDYLTTTRTTTTITBKYDKBZBITJQYbYZRaJLBaLSCKYDRKIRDIQSAaITBaaRATQbDJZRASBQQDZaCaRLJKZBbITCbISDZISACbIBTSRJDbIaARAZDLTDZIYRbKTRbDJAQDKCRIDKSYIaBTTYKKQCZSZBCTYSDZIQBCIaJAQbIaQRLRTRCRbDKJLYTBaLRZJBSbCYKQJQKbADDDISbIRDYSDKRJaIYCDJBBQZaQKCAKQZIRaIDDLBRKACKBSJLRbLLJKaYRbQbYKJBYJBYSKQJbJCRIRQbQLDKSBbCQBDDDIYYBQCIbATIBYSRLSYaDJQaADDYBBRbALLACASSJYJDLRQLaLBZILaSCRRbDCIKZLTbCQTJTQDDZaQTKCRYQQKRYKKCSbQLZADbLZKBZQTIYATbbRZbLQKQQJaTLLJTCZTTYSDAIRJCCRJaLZSADDAbQSZSTLQZDQBCDLSBZZKQRaTTTBKTaQLZYRRbCALLLKTIKCJSCBIaDIQLDTaBaYYZDaQIJIIIIIYJbSSJBaZKIJLaSBDSDIBSSSQAaZQbIDaSbRALRaAYJLBRSLBZTIDCIYbQCLSKIBKZJSQJISaBZSCSBQaJJDaQAIKQbCLSYAaRBYABJQYBabLQLKbJTIaJCKLDIYCTRBZJBaDSJQSYQDLLZaQJTTRbRaLSbRQADTCSYaZTZJaCaBDBBKBZQLYTIDIIaLZDZBKIYTARBTJaARKYBaaQZRYZQaSTCLbbIYIBTJIJTbQAQITIaYRKCYICabBLaALBYKSJADTQRCaZAaSRKBBBTTAZDCYLTBaRBSbAAAaRaKJKLaSQKJRLbYJIbSaaQZLYaAbRQaDKDJADDQBYZIYKTTbDDAJbIACaSSBRLJRLRQQCRJaQIITKTYLRDYJICRKZTaCIJRARYBLQJDbSDbBSRTTQaCBJJYJJDZAZZDKQQCbCTTBTTLIRLaIIJIQaSBZCRCDRIBRSKKSaTLZICIDRDBTCSIRKSZSZCSaCSLYIZaRAIQAAbDbTJaJbJTYLbLALbQaQRJZIaASRLBYDDCaBCbCDARCZRbZIKaBbDIRaTSBJJADIKYBaATQSCIZDKLaBQLAJDZZbCQDCCRYDbLDBTbLKYSSAZKBTBAJaYSRDQLaIQRJSJbaJICRATTLYTDYALCTBZYSRBTQbDIKLRZKZAQbQZQKDJBCSYbYJaLTILQITSJBSbSYQABJIJYTKQCDBZDSTIICRLJALBBbbCBaDSLKTKBRAbKBLaKKbTSCabZSBaCaZYCaaZabAJIDLRbJBQKZQbAKJDKBJJSYaQQQJbIDAQRBJSCISKQRLQBDTRJCTRRTbJYJSLZSYBbbJYSTCaBBaJbKKbAaAATBBCQbIbBACYAITAQbYIRCKKJbCCIZRbRLaZRZbZZQQIQCTDKBDbZDSSJaDIYJTRSABDZCDbCYIIZJKKDKBSZCQTJIJbaTSBJABaIZQILDbQaDBCLSCIYAJKKBTJRAZACZCDCCRKDTJKRKTbZRDKADZSTBBJBBbaLBYCDCDbJCYLLTKAJYRRTQLbQRQCYQIARRaLBDSbIIJQJAKKaaYJLJTIbbYTaCCbCKZAbJZTISREIJBAJSYAAATDBZALCRQJcBZQBbLQIKYCKDRaLaLaRIISKDBSLQIJKJbCQSBabKBBBRQJKKACaSSbZJRRIJRCTbIbaTSLaAYbYYTRYRKADCDLIDLQLIILBKIaTSQIaCDYYKSRLgABRCLJQZRQLTYbRYACYYZBLIaCaRCLDbBYBLRZTIRKAYbIgbARCYYAAZLARZADbaSTLZCQKBAKaSTaTSbKQKaRTDKSIYCDLYJQDQSbYbIILKKCYCaJZYBLZTZILIBbLAZIQaEbbIRJbAQSIRKbQRIQSaALaYICLQgQJLSIDZSRLKDaRCLQJTAAILRAQJSSRbJbAADTbQbADIAabZJBiKIDRaSILRSRTALQQTaZZIQYZAQZTbQRKQbLCTZSCBCBIAjDRBJaaSJBTRBaZKJTIaaLaLQIQJBZDTBaIRbBYaIBCDYbCabQJDRJBQQACYIIQAaJhSTKaTQLARIbAaKAZYYDDIKaJDJbSSQZKBDBLZZSTBBYTAZLSIZRcBKaKbTQLQaBCSZQbKTYbJDbcIYZbSCQDIRDTCYTSaLTYKDSQZLBKBAQQRZIATTQaDRJCZIjLZBKCBbDRLSjJSSRSRIIRLYbTATgKYCbKLTLZCZSZIAKaTajQaACQaDbbBZAYZRaQCQbcQTQZLAIaJSZTCIZbDSJLDCbERQTbTJKZBRSAQJabJCQbRIKQbAIUTSCYSabBBIICKJLJJBCZABIaCQITBIIaYbRjBBJJaDYZRLTTbIJbIJBTIKQLSCaCCIZCDDAaYDYRCRTKAYcADYZRTBZDCSCDBZJTQIbZCJKYTZLLTRDJQIBKTahSRTDSkRQbIZTAADjIZTRaCKbKAIZJLQbZYZSRjaKIgQBDhLRAYYJZDDKSQiKBILZCgDRaREYYSDJCTSKDAZLYYIKZQBJAAbALQaAbJDaCSLcbCKTQAIBZDAKDaZjBIRaAaRTJSRacQBEDbbBJgKLUICCiLRhQQRZTQCLTJZDbbBDATSYSJESaYUISDSBiLKATaLRbBYETIDLISLCBJKCSDRIDBbZILYZLSRaTSDTYIDTDDRRTAYAZICKYDiLLCSRbhJAKKIZJAJgIRgBjZBYADTRKaJbSAaSbTShCRBTCKaCDDCYRYYJBUKCQjDLTYQCLKKLBIbDYaiCAaZaCTIQMJCbQQRcIZLDBbDaLAARDKZYaIZJEgQcDZaJRAYJBRCBLhCaBBKTiTaiMABSRCBSLYUIZQYYQJbBLLiaCDACUCZbbALJZTCQQBIIDcLDRZAYUCDbKZALBKSAADJZcKQDBaLajcSSTiRBEIIRiQYDLUhMLAJDIDbYBLECCbiDbgRLMADCJZRiDcTBiDQBZKbZTaCBQSDbaTUCDYDcQJQYTCTKIKDDZZACATJZTTTYjZKCaDUSKUZhZaLKhMJEYIRZcJSZaBCQacRYCSEIYLIDTTYRCMDiILbJKRjYCRiSZDRQQJTUTiMTbKQDZIACjAiaYRKJiAZYYJbRDTbgBjcRSiYhQYCbJQIRAaQQTJDBJALjLDYcDSKIcZYZDDDcYBDSSSMTYgRDAZAAgbbkRKJIiIAaYUSKIQJAKDTJBBIKhZCjLQhhaJKZLiLISQTkShSZhChJDTLhJgSgibBJALRiMSRDDASScJDSLZMYUKiUjTTZEbSILCDMTaKaJDiCQSCMMRIAbRBLBjbaRMjLgRKhUQQABaDbTjSRTYKAJRYiTDMDLkIAjSIYBLbEDLKCUjBhAgKBJQbATbcQUScbcKRIBCLDLRCZgMhAaYYADgjULDMCCYaDkKBEjBbZBbIaLiSEIkZcLKLiTTCBbAKKBJCAhJBgbJLiEQYicRCKbQcYUSABYKRZTkRTJTQTJZAJJJEIQbCaLSBSkMJATYMkgbAIRMDYUSRLIkkjRMijRIZRbMQQTYhLJUcjaCQRADUKbIbJAAkkIKJjUTIjDQcLRUTgTYJDBRKKggaiUiZMSaBEcIAIAchbaLIBhRTYcEEgCJhBDLChYCZSTbDkTKBCMLJQRQChiLBCkcKBAZKSbJjRTkagCSKgaUchIBYJKbRZBEBDKbIKAShTAhMTDQbRBTKJcUkaKTSgkRbAchMjQjaJCCSTbhDBZKkbBDIkERAEkghLaMZSKaEIDEYDEMIBSUSLhJQISZJJjACbDjZkBCEIhTDSiQKKIRLTgUTLIQaicbkLZakIUhkaRBJQCYRJccRJUgAUMCQcKLQAZjjbQbcRbZBYiIjBgkQTEkZAKYEiJkiSYUJhaDQRaBZIDZZBQhJkSjSYQEUEIMYIcSZcULATYLLMCIaSTbcCEDbaMikYDkEQbchjJaSEiASRBiJAkCJgURELRCjTQLCRIZSUMDbZLCJcbAgiCaYSckaBQkCZEjUMJbEbcLZCCSYKZbQAjTDiZCbECJShEJkaDjTcCIARMTajgIkLEZULKBkQjUTDaQTYjSKUcjQRkbaQiAkajajTTYckLiJRLkCcKcbhLSYgYiIKRUJIIbBRTZRBIEUZRJAkKiAcbEUkDCkhSCjMZgRZBiZUaLSBIaZJCBUSchRCDibJkEAaEiLKEYkIEZjggJBETiaUjkJLbIkSjTJIbDLEZgYBCZRBDUITKEMhkcBMSMEQUagIBEZYSkikKhDRYKaKCUcZIZUTTEcbERjATIEQiIACKLijkJIagKgTgJBMLDgiBAJTcgJYTEKQAIcgRjaSQckLZgbUbQBMjTUjLKCKgDSEhSjMkUYTKgMaTIBZBjRIiYEgDUJIacbjEKhKDjgBEECjEEEbgZCKhIJcQbCUYhbMQMkgUhQYKRhTJgYYIDDSZCkiJTTMIjgRkEKckQUQZkhJgZBbIMALMTQIcaAISQEBJkLBSjIbkBLhQUKATaMgKgAahBCjITaYRZELYTJDJKcahSkChjJjkEUERckUSAhBRJhLKUBMLTbBLLYSYBihCSbgikBATbiBaaigDBMSKQTSTcEBcJkMgcQUZRSjSUggIaJJchAgCMhDaYgbikCLSYicSSYRSScbKjCJTUEYDJKTJTiELLSIbJYUQSSgTMMZLYSjCEMBSEgADJaZLQgbbCZigSTajRSMBZUKYSCgIiiQBKabKTRbZTJQLjgKKZZKgbUEUTkgDUQiETZBRZRLiKYDQDaJRaZJhIKbMigahCEigKSIQSRbBMUbcDaSKJhbaAcBbaQZKAjRAjYTYgLhbDEjUDQREhLQCDTLCKJgICIQIaKBSIKMbaUcZLZYjbTEJKgCESQDbCbBQgJQRUZgQMIABhRRIJhhSBIEgJKBbMhgYLgJJUIbbBbDQDbiciCSRYLCABaQZBMSYLgBYIIgZDbCJACRUEDZTTZSIDUYBJiCDghcBQiEDUTLMhTISLEbRYSICZKDAAZSQQIaEYjZBISTBhhhBCIJDaCiBMDbEZLSAUEZkLYQCTSiYTBcCbaTbCBTSKJZabTBgZZZEADTBSIYLaZBgQZIKaaMQBCJbbCjRRZZjBJaIAKAARYQhAbbcTIhUEAQUjaEEDTbRBgScAQRjTcaQJaLQRIRaEYDLLYcJYQkDYcLDSCCTBKgMiYCRZacbABLCEQAIAAkSbjaThcCaaBIBZhbZjhCIjQZADgMUDUCYbUaLiKLKRjbDRLAbcCCKZJQSCJbaSBhhTUBABYKYKRJZbJiLbAIiTQJARbSDTiBILCJSLRbYKDLAKBKBahaMbABcAaLDQQARaSLAaJkKKKRYYRbbQBIBaMDIBCKcRjCbRYTJBaRKJABgSLCZLJBQZAbJBSIYaYDBDKIRTYCJRBUibZYCLYSMbiUDIIYjZCTSKZBIBCIZJKJcKSbaSSaBITYAacBSRJJIJaRBRDZBUJQCbjaQbbJYaSRUTBbaQKLDUDYQYIKSJKITLQMZTAAkBBICaUbAYLJBQihYSbYJIBDLaCLjSBZJYcaIITCJAbDcRAAjCBSZRQSRDcKEKBJYBAKYBARTbTbICbJZSIYSLDABDUCRJaMRYQTJaibYYcLibSDhSKQbUaLBRDiZITYbSYLLEAjAKSKJDZbIBUDKSLYjCUUBTKYQEQSSDCKKIZiSDEZbRKZSAZTgZLaQTDCSLbZQKBRBbEBaZTaBKIaaBBAIBAgSSRKYZLDKSZTCZDZYUTDJYJZTMkDTSMjDIcQRSRaQRAYhBJDDIZaCcKSKEcSETcgLLTLZMRZjKDIQQhSJTbCABaKLTRbQRbSYhSLTcYCLUQbYDKKaRTIZTABDAYjCISLKgAbILZZCaYDBSbRQZSZASaCBLTQLSDaTLBTAIJaIEDRKTASjaMMRScDCCcRRTaASaBDQDSYaJhRCKYBUTLSKbYBYSjaBJYLhaIYJSEDRSYLaKMJLbQSBDaZAACLbIJaaJDSQQaUjiAJaKAMJJAMSRBZUIKBSAiKARKbDcLZDaQbCQSURBKQKZKIbIDiLIRBARbICBQAbJJDLTCYQEKaIAYQbaBYZBDJgLjRaRJKLTDLiYJBYQiAgYQLZBSKSTIaAcZSDLaDYAaKSZZLKaTCJYKIYLbLDaDRTRLBRKAQRLaLCAbKAJEZKbTYJQcQTaISMBAMbYCBYJRaAaBDTSZSCADbZBRBaZKASJJBKBAaaKiJSQKbCDCYDZRAbCLbQSJQCiJDEADBSLQSbgRUJCTLDaZaCJbbJSJDCCDIDIKgCYRYCYBTbBZYJTbQASBQIYTbIbJRbRJbTKKYYaJbLaRZBASbRaSRYAUYQQASaRIbgiJTCJCBKRIQKZZQDgDJKYbBDIZTQAAYKIRDKabIDACBbRaLbRYbAADSZYSQDZKKQLbASaBhJSKAKLJbBDRJKAaQBZIDTBRLRRSZbQDRDbLbRAiBASQBKBaZQaKYaLDIJbSbIBKRDJLBBUaDaCBZCYLJSbTALIYASTIDZRAKIARCZSIRLAJDiMAaaaYRYgcJaJEYAYCRIQKaDQRJcAYDATAKBDBJaALRJhgjICBJKaLAIaRJSSbBAQSASSYbYSISZaJaZRRKMKJJLZBCbRLQTIRDKbLQDLZSLiTbAJKALTQIKbBSAKDbTKTZBJEQYbARIZQSAThbCTRAYKbhKICbSSaQRbATCaCBCJKZRRbRIEJIQYDEaSbBKIRbDIaZAIgRJAKJZQZJBaJJDSJcCZJLYQZAbaQQBJMQCaSBQQYTbbAADTajDDcZCJQbMKDCIKDSIAcCYKIRAIaZJMQBaAIYcBYRKRRLZbTSAbaQLSCbYbRJRLaQILICLQAQQKAQCKLJLaCZZLQDSbATJJiBZQUIDKCSRaLSQAITBDJaRDCDaBZBDJKIRaabaCDITbIJKMCZYbURCQKIabKKYAAADIBBKBCSbIRDRQKARDIIjRYIDYQcIJYKJDRRZJaAQCYRZDLLSAIBAZBAKZJKLSASRJJCAQIDRAbIRQRZQDRBTRbJQZJJaaDJQQTKZJYJLITDIZBIDTARYZaCQCCBRLRJQAQJDQKYAZKILZSbIBSLBCIQBZQDDLbKACTbTBTDBISZJILKRAILIgKCZaCTQKCJBbKCCbYIIKCYJTKKYZYBZTZKAYZAbQKLRBKSRDYYKSDAJAQLRaTJSaTbTaKRRIZbSAaRDTabRDZZTKZCSYLKBYCKcQaYTKKJaYaSTJQbLBKBQAQCYRIKAJDLLKBTRQTBQKZATTaYIBDRbRJMSIQSBaYADKTTCIKRLDBaDBTDYRRCTJCSiTbALDQLZaLZIRDbYILYBBQTaKBTYIDIKISTZMLCQTTCQaJCSYbaRKBgIIITQZKBQKDTbATJLJRZAIZLIbSIDaCKYDabECIaAYDbKLJQZYCTIaAQYCTSbZYDIJaKLRTZAKZTSQIARYBJSCZJabCaJTLSSIYJDSLCYZISaaaYYSLZBITgSbISZZKgJTJMbbCQJRIJLAAbBBCTDKBQKZYQJhLRaCJabJbYSZAacLBIBRbCabACTMQIQYBaSQDBQJLLYSJQAASTQbJILKAZYLYDIAJKbABhTSBRQCKQTaZSIabLAZKDJDhAAJKLJDDDJQAQbRLISTIJbCIbLYCRRZbAQIQbQAKRaYQKabIJZEYDDLABZJbYTKKKDZLQIBYRZSZIYIQASYBYKARASRLCQBaJLBBKbJMRSAARCCJLATRCQJRTLRCbIKCaLRaZaRKQLQYDLRBQBDYQQbDSBhLYIIDaDZBQRSRYILLYBBBaSaLBKTZLKBLCKCYJaLAIQBLATDTITTBLQbKYKKBYLSRBYJBLTILDZDCIQTTZRLZISDZaBSRKJAKRBJCLJIYTYRQYQTaILCDRRYIaTbBCZTCASYcTQAZYKbDQICSaYbZaKTDZJJabaAIAaIKQbBDBcTTQZRRTASYAAZIDSaCTRMbQBTIQaQJbTRTLBQJSCYQIDZIaRCBiTIZDZBAbSbKZSSaLSYQQaKBYbJYTKLQADabDJRDKbKiCYJQKLQADSBRYbBJLIIJZbLDRKSJBABgBJBYCBATCSQCRALYDQDiJCYJbRYBQDCTAKCKAZIbRaBDaJQYJITYLLTSRQYRaJTRCJJcKYLCbRKJZAIYQTbIDTbKbiSZZLYIQARAIQYDJRIAQLRZaTbJSQLbTBLYABRbRAQaJAJKbIaQICSQJAJZARSBLDaDILQijbRBRLAIDCZKCDAKbbYDIbBgCZTITTDbQaIZKCTSQQBaDbYEIBCBbITDZDIDYcCSZbZZLDDRYLBgSSBBDRSZZZJBDRBaICSQLBKKLCQRLTBRARARYYZARAYTZYBSZaTCLSCSJSRRIZILQLCCBbLRQCJJIabBIIAaTRLSCTTYCbbQBabTAIDKBQZKJQUCCYKTbYIQIBZJIbASKZbCKbUYTBJCILSDKYaYDIJAaaYSYSZTABgDIbACRZRBJBZLDKQbSYRZALLZDbACTAZTBQAaKILbIYSTaJTJJBaDYZKTSKACSABAQTDLbJAKZJRTSSRbSaIDICCAADKaRZZISSZIAYbaLCILSIJDQYTYSDDBaYaAaJARDTBcbDBAKQSIZiaLRRZaJLYZgASbbACYKQQYaQYBRSIbbZZSYRZLRJaYDYQZLTLTTaaKDRSabKKbIARBRIIBLSLaYIbaZLASBTDagLaQLBaTDTCSRCSbIJDRIgDCCSaJQRRaDDAAYZaJZKJKKBBQBACYIYZRCZSZaTSLAZSIaTbITSjYTZRYbbTKbAQBQDKZSSSJSKYZJKbaTIhRRILcQTTYSQZbIDJISBQTBJZaaQJCbbCRIaSILZKbTIDLbSCIYZbAQCTKAbKCQIIQRbJBAKCRbAbBIIQbSbBBZJKZSbCIaKQCaQRQLQDZLRKbZBJRLLRSbaJTSSQCQYALQIAZBSAaYRLYCLJLTKaILBZbRATJSIaKBLaZKRADYKSTSZDLSDZTQZKCSAZIYDKKRKAbZYYKaBASUQabRZIbSRQTbJRCBLDSBYQIaBDAIJbYABKDZICTAYBZQKQbRLbUIDDLbaRRZQSZYKYRKBiZCTLBIabLAZTSSBbLJKSAQYKAYKBCIYLLTbCJbYALILTRSIKSACYaZRZQLabCMLRKYKYYAAaQZICbATJRbJYaCJRKbTZBRBYYLKAISQDDCTIJALBYRIIaSZSRZaCBBZYRDLaDLBbYQRKRCIAabZIaDTLDQIIYKLSLYYQRCCZZKLDaCbQZCRDDQYLSZIbZbZCTJBKTJYRTTYSCQJQKLSQLAALaJDBRDYTCDBTTSbQJKLaJbACaKLaYASKYYDCJbSLgICQIYIJBRJCbZRAJRTJCTaYDBMLCCIbLRDLIZZSSLaSCSLZABQYITaJQCLLbQTRIaSRbBCYCBQJQbDIBAaIZRQTTSTKDLDQbZYIACaaRbLDLQTDRDQKYLaSKaJILRKDACDCLSLBDLQKTbDYbaARDjTabKKTSBCDBQBLbTaQCZATCDZbSKLTZYLCRKALQKBbIYLQAKZCKCAJbKAbQKLYZQIbIDCIRbJRYTRLBJQRSIAIBLMAaCCKAKSbQDSLCZKQRLaDYSLbDRRJTAQYZTIRbCRBaJRKYIZaDBbJZLZYaZbARJSAAQaDTTACRZRJMQSYRJKTTDIAYRDbDCIAKaIIBBYKLLTLKALRDALAKZDAZZTZYRYbTBBbAKRbSDSCLSZJYTKSYYKCQCYKBAaTbBSbLLbCRKKJZJYLKIZRUCSTbIACZZYCQABIIIDAIAIASJKJKBQJYRIZSRCYIQLYCCaDTDIYRTYRSDBASaYBJbZTbQCCYRIDYTDLCKbTILLQJbbTSACARTQZDDBBDbSbSabRYLbISCITLSRRBTZBSKKLDIQSBJZZRTQSSDSCIKZIaSDbKJaLIAIJbTQQbARbbQZIZSRQITSKaAKSIAbQQ"
"DJDAaZJIBJIaCRZBKJbJTAbQDYBSTQQYIZbQDQbQIIQDYbBSAZQRADaJJaYaQKIBTJYbECABTDKbRDYbYZTSILAZCDJBbIISIIILSacSADaDBZILQITSRBQJKCaLIDDQTZCZJCYTTQSDQZLSTRBbIaaAbQbDQCaALbQLDYJYIACKDJAKaRJaLCYbJARBTZQaCTJJaBSIRAJLCSAbbbLDaQaLYZAJaRYALJAABAbCJCIRDMIbbBZCSBLTbLbARQQDBKALIQRIRSbaSSLTSLBRQTTaLRLKABYDZCaRLLbTZTTSYSRBQYKBTTJACaYSTZYICRIQAAbbYbDJZbCaYCDTbJaDSZDBaSQZDZaSQSAATSLBSDYJbAITJZaJSQLaQZQCRbIRYTbQLLDCQIYaJBDSDTaLYLBTCCKZKCLCKAKTRbQDZCABTYSbTZKABbIaTIaZbYbKbbIAZDaBASDBQCJLaTQDQISSIYhaBDJKDLKSCaDCbYQYDICQSBLYJCYZDTKIIaTRLRBQaLKaCYLTZDSCTJKLLTQDALDZKJITQQTIJCIaaKSLQZDJCYKCYAbTTYZCKTRLRACJQRQZaBIZLRJRCAJZTQRaLaCZRZCYIRQKYRIDCRLQYSCDDIZRRCbKIQBBKSRLAQAJKAIZKQLDSJBKIDaBBBDbCDADLZADAQRSSTITBbbZKLJaBYYBYQbSRJYaaAYLAKJaQBDSIDSYDZaaSJSbYLYTTQBRKQQIYRQYARaJITLJBCbSAKRLSLBKaKKYQKRbTaLSYJCJBaYLBQQACJLQTZQAIQIKTQbaAJJSbZKbYBTYCTbCJRRIRICIJBaDQCZLBZBJRJbKTTDLIAQJZYDDbLASQSCAQCYZQZJIIYCaBDABYUKAbBCaBTDQDYZbILZbCCSYYLSYKLYARCSKCLZAYSJJJJAQaaZaAIRTaKbJLbIQRJBAAbDDBARYQJKJRIDAIDZDBCRCDLSLYIbBTQTJbbZQbLaLZCCCQKTRAaBSCKbbQTJTJRRCBLZDATBBBIJTSBYZabIQTSJSATIRJKLBCRYACCZCbBQaJAQIZZTIQYLQCaRRRJRCABYKTQKbYBIjKLYBYbaRAQLQLabLJbQIJZYBQJRJLJLKJDDbKTSYDaBBSSYDBbSSBQBKYQQSAQLISRCRLRIRCYaaZaKBKABRTaIAYLDBbKJQLARZRTTKaICDJTDRBbbLTABSQDYTLIJZSLQJCBTRDLQJaLAJbTASAALCCJBJbSTDKSYbbIRTYQZKSBITYZRbaaSLCCYbQCIbLRSSDCYbIDASRLIBIAJRLZZDCKKQYBQYQRJRaLBaJRDCIABQaTTJZAKTJabATRAJISLDTRCRQJDBSTTBKKTMTALYaQbZaCJYBDAADaZIYDbLKAZIBAYLZDRLTZYRbQaIAYKZYQSLJaBQKAYTBBBLCaTJZADIITABDSCACCIIaLLbTRRKSJSQLLBbCJDDaCACJabKIDKSYBKATaJZZKCBbCKJDYKTaTAbLYJABJCCIYYAYIKCAYLJJBDJSTTYLKAKCBDaQbYYDSbQDSLRSAJALaTAYTLBKSAbbaaSLDQZBCMRRLKQaDJTLBAIZSYaRTaSDaTKCAaITZTZICAbRZAJJCbZDJBQRDJAAYDIYbTZabBBIQbASRTbaaLBRJaRTKJQZbBATRBJCSQSBQQCbBDKSJDTBQLDQCLYaKSLCbCIYYSAIDJBAKYCZLRJRDRJZLYKITCaYDYIATBRLJYYKTLbDIbBRZKZYJKAIBaSIaQKSQSQRBQQbITRJTTJCQAbQKTLZJbKCQDQAJBQYBATSZSKLRDKYJIKbJLCLQRTbDAJZIaTYSJCILAbYRLQDRKLSYZBaIBRICRBTDSQZIZADADJbAIYLTSCTLITTCZQYLBJRRDJCZaCSZYQIYBYJQZSBBaSaQJKJRASIRTDZZaSSQDCKYZLCSTabbZSQZZYQbDIYRRBKLQCJQATaCJDYaYSTbaKCAAKSDDCbLLbDCbSbIYDCRTJbbJLZSSSQSDYBZJQQDYSYIDZJBYRCARBBRaKQZZAIJKZTaYQaTJTRAKRbJJCYBCbTACLSSADYLaYLJbTJDbLTTSYDASTTbBSCDKTBRLZCbQCILKAaKLZTRKQDQbBaSYCaQaaSYJCabLDDbAQRZBRTCaBBbTKDLKLQIbZDJLJDZJSCJCRaSRSKDQKBRLRQCaKYZALQCBaAAbRJIaQRRbLaZJATSBJKaZAbJTCILCRBCASBSAIJJZZKDRRTQaADiKCJRZaZZTIabCDLKQDTACCDZTQYZSLLTIRDKCJBIKBQZaBQTTYTaZYALaQSJRRQLJASaRRZDZYbYCBRZRLADACQBRLRJbULLRKAbbCCLaIICIYJQAJbTaSIaaDZAIQRJZYIZCbTCBAYJaSZJBbSRIZSJAJSQLJBYZDJKABRCYSQZKLaIAbCSCSASaKZTQCaZJLQJLJQYDRQYTaaQYbLDZKYZCKBZBCBKLIRYIBbIBKQbTIYaTITZDAKJCbRRIbCaSKKRILABITCITDZKTJYbQSKIZCJCBYCbQQJCKbJSYDIaBYQDCKBBZZIRSDCLAaRKbARLYAAZZIRQRKaDKBBSLbYIYKSJSRQLILYILATJYYDAIbbDaAJJBAZSaAQaZYYaIDTSRCKQBIbZYLaCSJaAASCTQCBSLLQARbSYDLIbKDJDDLaDKaYTZbAYBbbRYZaTACbILJIYQKRZCCYQAYTaRBaLCYJCJCZZaIaSCQDRQCZKRQTIKABJAIJTBRAKBZIRTLBAZSCKIRQZDICLBDYCaDYRaJKbDQDTDLBBaaTYJZbTLYQIaKaQSAbDSCKLSLKIbTAITLLAKDSAAYTSJQQLQZTTQTLZBLBDJIDJBZSASILaKSbDaJZQAbDKQRSRbTLCKTbYCKQYSAZZSZQZTTKAQbQSZTIbSTYJLZDDQCQaKJRDQBTKQLCaRRKYYJSCKIBYaJISTDbaZCTJRYQJYbaRBbDCbYQBSRSKDKKaLQZCZSLDSQKLTICYbSIJLbQRbLQTQKCaRSbQRLaBbYDZJMKLYIbBSYaaRbRCYIKaDTaLYQCSICZaCaQbQIDZIAaDLAYIIZADaLSbRCRabSQaZCRbZSZBbLRKaaaSaDYTaSSTRKYJQBCQQRDSJTDaBaYIIYRbJbbKBDAQISTBTRJDYAIZYaJCaTaATJbQRARTJLIJSTaQYQJKYDLbaTISLbQRYQbZACKTSbRDQRIBaSADYbYaJQaCJaYbDbKJLTZBbCYSBIQIaQICTIDTDAADKRSDLYKZRJbKBSYJZRYSJDBaDDLRTJBQAABYBCLYRQBAAZCZKLQICLTAaJDSLYDTbYTZDAKJaTYZSJKLaTbQBbCaRTSRaCCRRKbYTDQQKDIYaZYYAISSIRAKBRLibZICIZZIYQIJZCAKSKSQCAJJTbZbJZTACYCSBJBaLILDTSISaLSZKIJLYQTBCJLTaJAYDAaaQADLCaZDIBIBYQRLKDZRQbbYRaYbKRQZQQDKTCKYIQDSDYCbIARZbTZQKQYKAaJCCBbCJIRbDAYbRJQZYYZbKRDSJAKQLBTYLLYJCALRTBIDbKRIbBBJYaSZDCIZBYAaISBLZIBKQISBZCDRYQYbKAYZaKKDZDKJSAYBSYaIZYIIQDBCQTJbaDQKSRICTIZBBBTSJDZaIRKQDSBbJIBQBQQJQJZCBbabLKIISBYJIRSKBCILKYITYYTDbYaJZTBJRIZbRbQKIAZYYDYJbTCACYaQJAAIKbJSSSCKACSaYTKQIIJRCCTLSCRLaAKSLAaaYJLLbJZCISICLJZAIQJIYSZSCBBJSSaSLYZYZCYALLQDLBRbaQICCTRAaLZYICaQBYDLCYZILJSRYSJLAYSaBDDLRSBYabZZDYTQYCDSYDJLRbBDQQbKabQRDCTRAQJAJACKbbAAAYYICTJZDKQZTbJBKBBRTLaBLaSbBKbSDSCJRLTaTJJBQbBLCDQKTKRaSASZLYIIAJCLLZQbKSCDIJCCbSDTJYSIIabTJRSQTYTbRCJbAaCQDTZbZATIAbCTIJACIKBaBIaSATZYZaBDLIYLabAQAIBaAKAKAbCIZBQBAKLDLALQLLZIbLCaCSARYKaJKDDZIaJbKKJKYAaKYKKAbCYKCSRRCLBZbSBBbQJQQRbZDBKaAKDaTBQKJLKQKTJQaBSBKTLaaTbJbQaJSQKbaBZCARACDDLZYaISYaTTZLBAJZCaQaRQLQLKKRSKaaRKDTDbaLSLbKRJQCRRCCbYJLaCRBIIAJSTKLYaRbCKIRIZBDaKAAZCKLZIKAIZRDBKAQLTRKbbATaCQDbJKLQZKRJCZSKaDBSCILBTCKRRIASLCaZZJYJIKDIISKKYBCKRSJDSRJAICDLTICDLKQTTbKJCDTBBRJJQaZAaABZZRTSYQJYaIaZADZTSDZQQTSATSJKABLaLKLKCQDBJSZLKJKRIJBaSDBQbALIYTRSbRSZDLCRLLKYQSQabSCKbbAKRTKABYQITaRCKSSIaBRJKAAJKBZCDDAYRaZaZKKRbRLJYKTZbSIDJRJSRLaZAZASbDDbKCZILDDBCbSTJAAAQRRRbCRCCCCDAKRaRaATIDTaADAJLYYbCaZBLDYIaBaBabSJLQZJRAaCLDLCYbSARCZDTYKJBCLDDJaZDQbITbRSDCRbSYBBKQKRTZBBZRAZYYYDAZDZRZLaCJbRCIYDaKALLaJTSKaAZDbAKRaALASaaICZaTAIQbBBQYYbBATYAZAQLRQQDDBbQLRZDYQaQIbRTZDYbYJBaZTCBRATDSKBKYTSRZASYKaKRJaILTDIQBLRZbQSYRbZaRbKJYJbKQRKaQQQBaSJJbSBQIJAbRJYAYSRLSZbIaBJSaSYKLYLYAZQJYaJLTLYQZYZIJCQADSKLbJTQaLJBRDbaSTYSaSbSIAZQbSZBbZTabCSZSSbIQYIKYabSAYRSCBIAKaLCQYKbZZKDJDbBDDKZBSDRRKbTYbKTQZJCAbBTCTRYbSRYLSBRDBLIRKQQQRZab"},
Embed{98,313,"AIYYQSYIBYIDTTLQKIYILILLYAJLAYQIRITIIDIJJICYIaQILKLZAYaaaaaaaSaSaKIaaaYaaaaaCbSaYaaZaSSYaKaabYZBZYQQBbYDKTbBITJYDRYDRKYKRDIKBBbKSJKABTIaBTQaBDaIBLKTTIBSLRAIRaQDIRIKaaLYDJRIaBTRAIabZTBBaaKDaTITCCBLaKDTKIBTKbJZQaQDIYTBADRaTBDJBIQLQKBQTTSBSJIKZKSRZABSTLBaIaIRYZIRKYbKKDRDYRDTILQCZTYaLCBIBTaIQLAZCRaJTKJbKQBQZCAKLISRDYRQbAKSZbJIYDBQCKaJTYQDRKCZBTJIaTBDIJKBDCIYZaQITBZaSaJBYDSLbKSILKTYRAKJQRQYbSSLYbADCZKZTLDBCAJKSIRSDBIAbYaYSLJRIKBAJBQQLIDDBRaLTKRKZYYYDQLaLCDBTRCIYAKBaCARZQTDLKSQRaBCbZIbRLDDRZaIIRDBDIIabTKaLQRTJLARaQYBSaLSTIbDaJLQaQRQaKRJaAKIDCKYJQQRaKDaKCJAZKbIRBTALCIKLIZCJaYLKYbaIQKaTQbDYaSYBRSaYRYKYARRLBIDAYYYDRBAaDSYBJSJTRKLCAJbZILIJKbKDKSTKCZDSbYDZBZAaITLKYTZRQJAYIDBISbDIRSLRaRRARaDBBRIaKTKIITLYYRCLRaRJAYBZCYLYCZKLDCTbTCbAQKIZQCIaBSJTYDIabKBLIaCICTbTSCYJTBIJKAQQALAaKZDQZLSDKKbaIQCDDQLSJSKTYBbKZRIJRQBTLISCCILSBQbBTDbTAYYbZaLYIAADTJDCJRZDACaBZJYBaSTbaSTYYSITLAKRRLLbDZBAJBSJQaSRTbBbSIDKDDRaIIAaCZZLZIJYAIRQSSSBabAJYJALATTTDJIJTIQIDbCBLCKDBCLSKJZSLCQDBLbJTSCTZBZJSZICAbQLaATYaBKSaBaTARTJbbIIbKQIQKJaIALaTDbSBKBTRLQDBbJACBSaKYaTSLKJaQJDIABYBKLTKbCZSYCbKaSKRTBZACbDLLKCSbRQAITJYKBRTTKJbDBKACDAbZCaYTDYTAZCBJRSLRKYQTJYZQIYRIaBYLLYBTYbAJbIQSBQCTaKLDRKYQTQBISZSZZZYLAIQCCJJIAIYRSKZISITCRLCRRKCaRLZRDDCIRIKbabCIAaLQBSTaZDJRCLaTYYZSLIaQDRSDaKQZBAYTRTRDBADZDKAKIJIIIZDRbADYCZBITRDZDLDBIYQaKDKLYYSRAJIZZDYQaTLBTYTCQaIaRQbDKRKASBTSRRZCKKABLabASJTSAaRJbbCRLACRRTTaJZKaYQDIILJJCIBARDAJALICaRDRBDBJaZbJSKQbJSbRCZbZKQTKYQLaIBRAKKaRKKRLYIJAISYKRDaLZaKZKaADYQCZTTYCKZAaRRAIRTZAJJQIJaRSCJRZKKSKJIJRQYKCRTIQIQJZBTSBIQbTbDCDYbDBaIRZaYTZIJDTYQAQBbJCAaYZSYDTZYJZBaBQASbDRSZSTYLIaTKDSIIITJDSbLCQALRaACSJBCITaJKRYTbAbYaYYLTSIDCKLKZDRDBKLRCSLQBDIJZCLCbKYIbSSYZLJDSDIZaaKRDCLaQIaRSLBDBaKaJDTBABJQDCbQZAKQaTLZaRCCDTRLSJAQCbTYRUBLLLJTTAabCYaTRaQYIIaRBAaQDBTTLLaYaBLQSTLBYQDaTZYbYYBbbCKIZYQDRYaDQZZLIAATASJBSCTAabTSCLDQKLKKYRTBJILbbRTRaBISbZQRBBLIIDDTIYYRAjZSJQYaDaaRBYBLDbQQDKBYADBBAIaTLRBYYBaIBQTBKRBBLAYaKKRITTYKLbCQDRCbBaTTAaBiBZIIbKDaSALDKQZSIILLTKQKJCSAZZIaaQLJCKSATaDZIDbQKYDcZYcDLKQCbTCARDDTIKRZZITIADLYQCZCCSQISbCBSJRLJYZYaIBDBIARLYSAbRRBSDAKAKaLRZBBASTBAbjZaDIaDJSZSDYbaCCLbKCaBbQIADLbBKJSRDRQKAAIbRZbADZZSTYAbJCSKYJbIaISSCKYLYZADCabSRSTbTJBRSSYZDACKDRZSZJTJRJaZKKSbARaTBCYJCKDBDRJTJZDASACJbRAQSJKYKjLYSBDAaKRBQICYJJZIBJRZTTZRAKRDYRTTYCbSITaMYKSIKAKDKKTbZLASBBJRBYYCQQLAbKJbKaTbYDZQAQSKTJRSaJAYaYLBBaTbCJDZADKRbcQAKCYSRYBDJLaBCQZZDJQTLICbILIbTRLBaKYCACaYZEBabLARKiaLTYBBbRIAYbQIiZDaRSKSLCDISbDYABLDIYCAiSKaZJDDQgbLTZSTTYBADLAQgMSCZZALQaQTKQJQTSDBabBZRAaRbJCZIRbSYRKSaZDIaTQjSRCRKRBJaLAQBZIDKIAbZLDSTBRSbTYTIECIaQLJRSDRSCYJMTZARQaZTTZBDKSiDATTUIbKKBJDKMASbMAKLZaQLQCbLYDJAJbKKRJJKCQAYBILTZSLCKTkbMZCBBKZAZYbCSSKLDMZYJYTRbYRRSSSJQLURKZhLIYJCCKiALCQbaCBCCSJSTLMQLZADCAQZRAaAbJIBJJBAISJQTbYIKDbSbACBYJSQLCSDLbRLQLaLSITYAERSSQJDYACISSRbLJAZLaJAZRAIDTIDJYDSYQZcTaZJBAcSjRRLRbgALALBYJCQSbKTbLRRSLaLSDDKLATKDIaYCRIaKKJIZhTJRLASAaDZbKQcMDYCSSDKRBAaSQbChJJaaIAIEYJQJSbRZQCKBJbBJSjRIYSDKZRRYRKEbIYiLcAQCTBATbSacbaSQRRJDRSZaLZKRSbTQJQBKbKAAbhQRRDTJQYbLCbJUZgCYCUQDbbAQJIYAMQTQJaELRQiajIMACbIDCTDSZJRQTKLaJZBaKYRBLJAbLAJYYSZDKjYBAIJYQSJjZCKaDTcTBRIKQSgTAZRcQSKKELCLgRKAICjbJRLRiaYJAaRgICTRBajARaiRiAKJJRbbhabTKUaIEDDISTABScSARQYcRhQgJYDbLQLaibEiThMJBQLSJZhJaZEIAajBbLAgbQYBKCQbBLQQJDjLAgYKEICYaibJBhCjbUADQQYTJCBBUKLgKZMTKTjYCJBagQAKADYBILKIJIBIMDAUggSbChjgbYZIITAAMbYQEJRRTbiILIKKBQjEaSYhKTBDYCQZEcCZTaaTARJSBJATcECRCQZRADJKQjbCTjRDBULULaQKiajIKJAZcKELKTYcEZDALQZLCRKDDaLDaacjSjbQiIggchRBaDUUMQADDLgjKBgaLDRIRYkaQJDLYBcRhASJBQSIJakKSYMMIRBUcaBgBgDIbEaCjRTjbSAZLLjKbUCSELACJCjKKKCihDRiAiAJabJTJBDiUQcjhaMQDcQJZRJQgTYSijkZbDJbKQJKICYQBbgZQEUIEQZAMLIRQMDZIUYQZJCBAcYAZLJZYgTQgCCRZCRQChRYJBJahZjBiCiQSEkTbCgaJZYQZECYZDZSJKUaREAMgIYiJSQUBETEJgLMaKThTcIKjBCLQbMCYYTabbiEZbbjaBKLShBaEZTTQQDCTCZLBYkkIjCSiIQgZCZgLjTQSbBKLcAMhJEDKIBTTKUESYiTYgLIbZYgZIRiAAScAMATIRbbQDAabTUbSZBbbCbhCSEiJUjjZJUhEQMSCMDBScTbQULaLccbQZCCYLAabkARQgBaKLEjgkCUZRQbBREDbELZMikIIjCYZLJZQQSThJMLZBUbLKaBAYiILTickUAhIAJUICYiUYRaUggkCJhSRKcaRBIIMgbLSDiBAcTCkJZYZQicCgjLJcKAYYghRjbgITZjTSAbMRETiZTDZCEELTMghARiLDYjAUTKCgbCYIgJgMKRhChbbRShRLkAaKZCYIAIcLbASZIChaihbjRiiMMkBhUQkSAUKhQQbgaTiDRBDBhMEQUgMLIRMLBjjAkQhAZRDcURTcacTUBRbchASZbgKZCSDcAZQIJhTZbbkQShakSRJQRUEgMSKiSZhAMJhZkkCRTDkEScAJAbaAKScQLhEBgkSSKDghhYSEkKUhZLBhjKgKCbESIBSaASgYLhYARBMbgZiKShMkCIESTMhZiLMbaKQhLhRacaKECCMbLIJZRIZKKCTCCiICKQTUUTkTkkkMMMDSkAciURLckRIJjQhYKLAYDaZCRkECbMhJDTUZJLbJLYjLUMaMhiSjgQABKiCkUggRAbRTLThJTkZRIMYkkQKYAISiKZYAZgiacJRigicTQMZTkAhTCiYKLLaZBiJSahIUkDAakkYLiUJQTkUhLMMAkjDRCQjihYBcCEhITJbLZacSDEgEbIEYQhAEAkIaETicMZkQYTkIcQLMEcRBUIaIEbhKKAhgQTAKRQiLaLIjIUjghjEJJgTRjjaUUIYQhAgMicgSRgCIZZATIjcbELECkQMcAYIjLYkiZghhaSjMEYDcbAgIYMQQYKUKaSZTLIkDKkiDCDkZZkDRBkcLDAjECkZhQKckBjEKCUEgAJRJYkQZCACEBcSBBMbSaaZSiDAEkEIiBaSUBZURbkKijLUhUQRQJEYgMABUQSZYbDLgjBkhMZBiEUEQkiZJaZkjcCKhLBZiQIgjkkYEDbZDScKScLDYYKTISDYBbjTTYUgkMkahAUYTkgLKcYMMBSkkIITckYLQTKCLiTcMDILIjUCDEEUkbAkbkAASRiJJgjIjZBbhDSRKYjKChhZISRbAkBCbRcbiUiCSBKiUCMURKKZJaBCZMSKARZITQDjCTSjTILbYkRCiDgbQBJLBbRihbSJSKkcIcIRMcgagbaQDgEBTRaDAZLZQDBRcJKSZUQJCCcgbciihYTKEZRQQKhiDJLIKDKMTLCAQTcTRUiQaDDhJAAAEJYQQjJRgKMMQJIkcQLDISjaCUTCRQUMKiIYabTSBKjhYbjLhBUJJUUiBKBDUaTKYMBjZLSQYZCbDJbbbQcCRISBRbEcbCCbTiAgMASAJSjjgQQJcTAMLZbBLbRBYITCKSjQBLAQUETYIbBCJZKCUgjTTKKTaiTMCJcLCjAUKACZSAAIAYSbSSgUZKZijJgaBLbYZIiSRjLBaYZkDDJZSEhbZUTLJALKCJKYCKZLJbiDbjQRYLLYAAQTTAhThQBCBREMiQbQaTUbCADCaZTRBTgESDYhbcSDRAYMZjUUgKTTYbDCgkaBLRQbLjiKZJbRKAIbaRBZQaKAKBRREbgbQTRiULYRjTRgSSYKaAAJUDTKabZbbTJSAkQThKDYbSLJiACaCSjcEDYDCcBYbRKJQBaCUAIjMKTLZZDcKaiTZAATYUjaIDaTRLQLKJBSAIZADhjQELJSJKaSEAhjcAgJIbZDKaBbASjDZQBgTYZZTRaAQScCCYJaYLBLTYQSBgbSLQYLaiMILbCJIQaMCjKRLiLcQDCSZCRJIYRTZYTZQLZRLgAiQIQIAihAAbhSQbLZCAbJCbIaDIMaYQEKZLZLYKRbYRiiLSBbDRTDIQBDJBDKMIAIZTIJMTTRJTabCKIbaDQRYDDYaLSKCKQCRKabLKIAJIKSKAkDRJZJQRbKYDkaTCQgJCIQbLRRBCBTQKBCTJSSQAZBZZTSQSaJKLSJRQRLRiCbCChIMTIYJQYBIKDKZJkaQEZSQLDCTbRYZLKbJQUZUaaDYCAScTIZJYTRiYCKIIDJJChTBgLRSIbLLSjRBjADbQYJJIRKQMDQhARLSYEjhKIUQaYCacMTQKZJACCbCYAhaBZZISRRTQEDTCZQEhabYQhbZKDTIRZjBaRaCJLCSBCAACIRALSaabSDaZRQgZYJAIbZRSLZIYBCKiSDZCRZJMCQRbDABDRQaACATBBQKIBCKKTRLKDaabkKbRSCYYQJcQBRbLJCZZALLTbTZbLcaLTAJCQYYKYZLJRaICTBbSKRATLSJJZBACQagYBhBAKTKKZajbRbCRDKLaQjaRTYAISSYTZgISYDLTSBShBIabSgTJBSJQKQKITDAAIbSAQSbaAYCLIRKLCKLaDIDBRZJZRJDYIRaQZaCjLCCIZCIKULILUBZLLLJYLQRCIDKBKRQSYTbAIjgSJDaSBDJaRbYgRISbbCgIYUQAbbREJYbMCBMBSLRihQTTLcaaDbDIDQSTCTJJbjbASgRATSCKZITBQAQTRYbbYDLJDYaJELCRCITTQMDSYIbTRgSTTKRAIZCYQYDLYRTbDIaQcDLaYBZTBKYLBhSRDDTLRDAZYKbZDCDISKIbISIShAMZDDAZRDJZACQBBDQBAYBbCRDhaRRLSLaLaQRaSRALQJaTYkcDBLaAZABRAKKhTaSLABJQLCjIDAZSYTCJDSKYBTbbbISSKSTYYaTZDSAjTJJLABDLhKTaTSJIQSYCITAJCRZLRAQDBJRSbIRLaBQKBZQbSZATQAbBDBacLRQYMZKARDQAbaaRCUDhgZaBbBScRKRaJQULABbBIISIYgQLLaJLLIRCBAUUAbTQbZKQRaCaLCgAYBSQbgQAZJJDjZbaAQTEYIbRiaaRDibCCDZKTCMUJTSYaLKDQCILKCaDJZKISQSILABQRJRLTJRbKLKCZYCiAAILJLBUYEKbZKKSZDJBRJQLTbTAYBSSJTUiLITShDAKTQAIKDbCJTLbLYSQBBLLDQSbZCRLATKAcAYRITIbTKLCCYKbYRRYaBSTbJgKbRLZRMYDBZLSLLKbaCjBRYaQSDRSLCiKITCQaAbRSabaYQRaZKDBDIYBTDaSDRZEBZZYYBBLBDDhDKASSMKATbCSbLKZEJDKYRBcKIYYZaDITQaLDCDaBDIBSTJZSbSIDTITRJBQTYCZKTQTaDRaBSYTaKLKKaEJKbaKMRKYATKJLJIIQBEIJaASgQRAABRbYBbJJKCKAYKCJKMZAbJDSKJABARDTQAgKDJQDYRIDZbjAZZACRDiITbYDRQaQTQMYTTaQSCQKbDJDYLRKRBDIZJaCTZLAhLRUSYASbULLYKZCBRbabTDRDJaZTADQKYCLQRLRCCDbARALgSaRJZBIBRTQTgISTCQEYSLSMLBBJaTZCBZLYQABLbKYgBASZISDbDAKIIAYZDYAaJABYTJIKYjbQRZJZQZbAaBABSACBKLcaBhACIKAAYLjQZbJJRYBQZBCTIDJRSBARADLIJABQQZjARhbTRYbKabQDQDRRJDYRJBJIThBLaTbQBRYBIIYZRQaUaAADQbLaIDASDQJTDKCaSCQSCSRYRDJBJDZYbIRDZChLSZIIIbCZLTTKBAKQYABQCaRRLBCZIbBRZRIbKaLaJTJJbZTJTDBASaKaDBaCSIJQUaSIDSDISSKATJAZCKJARARZAYLSYTDbQILTZTZLbKTKYSYTLaRBaDIZJQbSIIQYKCZSTbDAJLDbCLIKDKIaIABKDSCTbaZQTLISRbZBJQZASAaILRJLSbBiBJCYBQKYCZKKSQRLJATLaBDLLLSCADIaDTZQKRZAIZbBCDIDAZQKSQSYIZSBCTQbJLKACYhJJLTBKbYILKBcYZRJYQRDRbZSAKTBBbDaQRbTaBSLCLRYZQQZJAJJAQDBBTZCZaICcSKDCSIbTLZISRQTJSbACEAbSBBYILJbRaIRZYTSCCTaBBJLSBBQAaZaLIZAZYBATAAiQKbJAZDBTjZZaMBbbCLQZCBQAKIaLSBKLSKZADTCbTbIJASTQYYTJDDKcAJIKJbZTYJCTBCJbZQbLBARDYYKBSBZAKIDRYbUCCCIIARKKZKKCIRDCBLBJQZIaLCaKYTAZTZJCKJLSRQYTBIaAKIRSCRATYIABJJTJTYJRRJKAZYLCTALKRCQbYKKaAYEZRbSKTAbBYKTaCLYaZJISRQSZIAIaQKKSBLJRZQaBRQMCbbSZQZbLCZaKCRKCCaASCUJaLSBZaIZSYSDLBZIQbbBJCCKKTZaJQYSTbASCTZDLBRAZYSKJAZSYZAbQYQCYJLRRSLYbTLYJSJZbSARSRRILJATKaRcLSBSSSbJZKTSRBRIaaaAYQIgSJZYYSJZAaTbJARDbaaaabTCJbSDabJIaSIScAYaaAABZCYQBYZYaIAJYRLYbSJACaYDTbBYZbJBMZJICBQbcbjTSaYTCTQZaDBQBZCCLJSRYKSaZbIZTALZZCAAJJTIRJSSIBRDTIaYCBLCCaTTSZIRRaQJbJbBYDRYBZDKQRQaAjYDRABLScCRZLaDAaaCZZZYQbaDAbbSbAJaRSADBDhSRCIAbbJKBYDBJDBASTTLKRBALLTAILBbJTjbJAKbKIRAQYaBCYACCCRYTBISJLBJBYKbRYTaDDISIKDDRQCaTKLBSZaLbUbBIYIRARTLIYRZbZAIaLSIDTCKBITaTSaCEaJRLLaaRTYLYZCBYbLaSYBQIgULQZbDAADDTKaSSJBbJKhKQQSaDBDBDKLAACYLaZYJDKIJLJTbITCLQSYAjRCQBTZSIBYJabZQbbBjKaLBAATLDQaTTCaTQCACaCDbQAIKZJDQCDTbZABAISSIKDJLKQcLYYaZZKSIRQbIbKLKAQRLCLSAIRCJJLIKbZTaZZAZBDALAbQRCQQRLCTTTRAETIBJRCBALZBIIKLbCYaAJBSgSBbDKbKDAbBKRDQQKSbTDRZbZRSKSbbILQQSBCKIYYKTDQZTTUZLYKCKKaRLBIKBRBQBLSTAISSBZMLDAZIAKDZIDCabbaYJZAbLABYYCYAQbYZRCaaRKAJRKZQKZKSDKCBKSDbQbDJKAKTQQJKADCYSJZbRBYSKbACaLYLLAbZBCbCbBSbYCSJAbRLCLQDCADZJJKLSDZZZCYTAaQLQRJDKQZJCKIQKYQJCbSSKCbDabYYCARCBQZRLAbKRAZTSLZKIJKTBCTADSDaCDRDKCJBTKZILDQaCIBYITCMbYIBQTJKZJKYaKbaQZCRKDCBBbKZDTKYcQQZcKaJSaCCSSBILQZLDKKJbQRZLDCSbLATIICATAIABYLAJKaQYYbAbBaTDaCIYBZBCTRJKSCaYBCCQLRbKIbJAYKTbRaDbJaQRbCQJKKQZKSaKKaLKAICRICDACJZbJTSAKQKZKLLDRSSRYBbbLRRQbJTbCTQCLDDaSDaSBKZbDaAKIIbTIbRAaDbIQKIASbbTRAZLYICALJaJTKbRLTIDDKCQBTaKDILBJQQJKSYBLDaTRAKCSRJSbbACZRKZCQQSLSYYQBADBRIKBSYBJJRJjJaJBYRZKRaQTBYCAaKbJbJTLSaCaKJDDZJALCDZIIIJRLIIARKaQCJSbbIKISISbQYQSRLIRQSJTQRBYbLahBCKRRYIAbZLYBCYJBbZQbRJRDBbQgQRDQJTAaYIDBJYKaZSLBQTTTDbIJQYTYCLJbUBYTYAIAKLLLSLbSDADDDYLQLBSbbATZCTBICBSRYaBQYLYLACCZJLDQCKZDIbbBaQCALKRSBQIQKSSKLLBRQZbaCKKCATQRJKCLRJAQLRLYDYIYLZLYBLQIARBQTYUMALDTTYKbJZTJBKCIBZDZYKSbRCMQYIQQLYSLBBDKDSKJbYKaIJIBYSbKbKYAZJCTRiJRYSAJJCZaTJAQQSABaDQRZBAZQBTYDARKaYCIZTQBQCbJQSLaZBDSKJAJZSABbIbYIBRIBDbKQYTJbCCCLLCZbaTJbBZaQADKRSDKICZTbABaTDJQiCZZIAALISYBIBYZIKSKQJKaSYQAQQKLDKLAaIKAbBRQLRZZIJRBTabQRBYKARJYJDTSYRLKLAZLIQLTaLDaDYRQRaZDIZJaCCTCDDBBCAKAICRAaKZDZRbBBAJYQTIaCYbZBLSJYSCbSDbZSAaZYRRQSLLSQBAbLZIIRJLITTBQTYCDCbaIRbbITYYAJYAAZBQBKRKQRIKbCSaCRTQBZAQITCJLYCDLTRIDAQBZRRbKaIIBJZbJLaKaCZDAJSJaB"
"BAbaaBZSKQZRCDBJIKLBILLABJCDIRJARQKKYDKYARDRDZARIDaRAbbJYQLQCACCbIKKLQIQJSbSICLLYibJTSYgDTZTQIaASTALJbbBQBLJaDRbbRLDRAARKKJCLILTRSKBKRDAJSDIACYALbYbTQBJZRJKJJQJRaKACQQDaALCJSSZCDBbDJDYCUKQARTIaSBACCQKLSKDRYJRDLBLTSIQJSJQBIBRQLDDIYTbCaLYTYLBQQTJQLITQZLACJDJAJZDDRIJSYAKCZDRLCRJYbQBBBJKBQSKTKKKQbRTSSCLQAYQbZbAZLYRKLALbRYBACDCZQCDSBRZDJDYZZBSCKDQSJARLKJQKJYTbJRAAbRTYABBbCZKBYZTbRDDTCQZJDIJDKZLSTBIIIAaBQBSTCLbRILbbITIILYSSTBKRRTQbaSTbYSLYAYYYLaDQACabIKCIZKQLBaZQIJJRCZbCRDILLDZSTRRACZbBAZYLaDCDaIbYRIJZTLKAaQBISLKCYKAaRQSTaZKAJBICSKCbCDIABbBRIKKTRJALBKSIQaDZZTZDSARDBJTQZKaLaADDQZKKAbLYRYCKRSYJLRDYRTARYJYbLRaaLRRAQZKYQZDTYKDaAaQaZJQACKLQLJRRKLIKSSQLRIIDIIYJZTBRRaTCSKTTBQLLCDJQSKaKbSBbaTILJATCRATDZATSZSATaaSLZDCCDLAYZMbLBKYSBBYaaYCQKJRCYBRbZJQTRSQJbTbRJQaBTaLaIaTZDQASZDQIATICJZRQZKRCCJaLATTbTYDTJQDSaYSbSADAaYYCIaMaCYDRbbYIZaZYLIIYISJDITSSSZKSAJLLCRaSIJaRJSYLDYaBaYCSYZLTRaQICITSSQaJCQaQKbJZRZYKLTLLYQRDaKBBTJIYDTARYKSDYIbQLYaKJCSSBLTAYCALSKRZbKYJDRaZDLQRRYTAKYILDLaLTJYSYBIbCbJIBBaCAIZCQJTYZQKJTbCSBTSCLAQYBBJZbaACYLJSLBKJQAQTITTCJRLSTLCYTRKYIbRDSCLbCYJQQLTYBIAKKDTJRDBZBKYBKJACZbBKaYQYQQSbZLYSRJACCYQLYLKaLbARQRJAITZBKTALJaSKZQJDSRTYBIaYDBYBaBKTaSYbSSDLADLCIIZSYZRZZYKIDJZSAAAQZSKILQLDBTZYbbbBKKKDILDAILUDZCCLYJYIQCKACRBAJSbIAbTSRaKTRADICDJBAIZLACLJYISZKSBADDTZKIZKQQSQYBIBbBQTCIKDTaZbDSTBLRYKaKCaaISbKZaLaSCJRTJKLTYQDRJaYCICbYCZDLTbZbDBDJRLaTYbZAASYSJCYRabITZTSKKaRTBaQLBCaJLZbabYZBYJJILKCaKSTAKCRBJSAARIRSQRBZIYIZRDALCaDYKQJCCCAKLTTBQBLRZRQYaLBRLbTIYKAKRJaaRKIIIAbaJLQJKRKDYQTYTBRLIRKbIJCSBYSTCQYbLCRRIbSYDRIIabAICLaAbQbaCAKTZbKATDBZZKQJBRQIDbIYLSSZAbCLSIbbBSIBJIDIJASLAJZJQTTQBQLJQSTIRQbRaCaCJLATLaQaCKaDZTLAaKLYKZLSaKZCAQIaDKCTCTDTLZYKTJQbLSSDKCaDBCKDJTKYDbIKAYKJIQQYLTQIAaLIKDaDAYKRRTbRDYbaQYJDCRRBRDJBaQbbRaQJIaaRJAACKBJCIABCIJYYaDBabKKbBZLChLRQLaAbTASaaYDBCYSLTBKRZISbQCbKKaJBITZbbLRTQYALSRTabJQCTQKKRSSYLQBJKaaTJbCCLACbQQRDBILYRAIBIZKJYBLRTTQbRDTQIZZRTbYBILLCaYBRSKZYRSYaCISZAJaKDADRbIYDLQaDBATITZSIKKAYKRJYLKJBaTJASaKYICZaQAbBRASSJLaDIIQaRaaJZCYTZIbISJKRDRBAJTDTIYZQQSIDKZSgLJBKKZSRYCKSDAJIaLCbSbDLRBQCLAbAKIKIaLAIZQBRbCDDIZYTYYJBDDIabTSSCSJJbBaAQTLKYbKRDBKCDYTSACQQRSBaBbJQZRADDLZLbSBBLBKQBDLZKITYRQDLYBKKBYRaQYKbIDZTZDSYDKQCBLTQQCZbZATLTSCLCQJSIZYZLTJITJALIDQbBaCABbKRBBRKKKDCKKSLBJCKJAbYbIRQaDaBLTCBaBRAbSIKbTBSTBSCBJbCSbCZYIYSaADDZJCSIKKbBJSQCKKIZYLKBaIJQQaZDRBDDaLBIRaITISAKQBLLbJSaTJYLJSbYACRZRbBQTLbZZBITJTADLSCRITaSCDALTDaBKBQaKCbITDTADTBYaAQCLLDACSDKZbaQYBZRbLRYSQZTIAKaYJDbbYbIKZSKYaJAYBaCLAJQTDICAYLDbZLRKIQRZAYYbIKBZACAbZJRbKCZbYIaCCSaIYbbLSZLSDQRTBKSaCKJYYYJTTQBQZSSLJBYZZQYRKLRRYJBQCSKZJZARKIJISYAABaaLaCYLBSABLZaTSCRCDBIIQICYaIRYaSBRJRJRTYZSQSDSJJCbBZYQLKDDaBDRYSBLITYZIYKKJRKQQBCBbRCQBBJRZQBTZZCZaQDQZRRLbARKTBTACDbDRTZTbCTaDDbBQLBbYTDSJAYKCRRTDKQITIDKTAICYIYRJZSJRQTBDaCKTQKBQQARaKLCTTJCRSaJSIDYTQIZZDLDDBabIYbIJBJTKSDDCYLTZDKRKSLKbTBbLbAJCZRBZCSaAKTQRLKSTYITQDZAJLCQTSYSABZJabIIIZZAbQCJKSQbITTZSDaTJJaKLYQKTDCLTAZKDIYIaDRZZYABJQCaSZAbZJJJKAASAIKZCADYKAKJTTSCTbabbaBRaaTLQTDIJBJKaADISIbRZIBCJDSCISJQQLaALQbCIDCKTJYaaYbLADQKKATaBbYTaRRJABTDYLKRRTZYRYJDCDCDaIRLJADJJTBQCTbLaALDDSYaAITRSQZZTZAaIaJbYTZIIBSTYSBYTRCKZLAQLKADLLRIAZKKDQZaKBbaZTSAYITAKKBBCAYRYIALIaQDLLLLIabYIaBQJSJZZZJYDAIQDLABQQaQbAYaBSSDCYRYSZLLKbCKRLJYTQYCYaBRJSBDaYAjSATaDSKQZBbBCAZYaDbAJBQYJBBQTZCZaBRDBLaaIRCAbCLBLLbZSQKKLCZKQQBYRADYSAAaDDITLaJADQZQRYQCKLbYJDZDaYLYCTbaZJSLRIRQaaJQZSbbRYZBKBSASRRAKDDCabbYTbIZBaaKADKDISaTLJRAIKYRBDAaAaQJLbRCSBYJCRSTDSDKTaBKYDbIDRQARJQIbAYTTLQYbDJIaTRBABZDITDCKZDbaJBZAILaaCQTaYBSBRILCRbIZaACILLATIDDLBTBKLCAYTSSBBaQQBAIKCZaBZDQKJDCLKYRbLILBZQZRTCZIQaaACaQKZbDRKTTDJJDaZYBQCAabZYbAKBCLKbLTRTSITABbLBLSKZRBCaTCDZZAaaQJYSSJSQQIZITLLbDBCRTADJBCabQBYSbBaKSaSAJCDLCYZSBYbDZKALBATJBZTZDaYTRZCBDQaaIDQLALLTaJKJaBKZYKRKDAQQDCabQKIBYIQDQQRLJRRLbbSYBKYZJKRbIZJSAIbYKKALTYKAaQQDIRTZIJQBTAARILYRJKTLSRJAKQIQLSBJTILYBBDIZRTSSCbLQSQaZQIRZJJRaJLTBSAICABBYKLaDSALLCTLaBZCCbaabTbARDDBKKICRDARLIYJSSYbARSbSLYAKTQQYESISLKQTLCTbASLQTJLTBKCTbRQDTZKRQLaYbJTbRLDLIaSIbZKRYQZDTJQaCCQQBIRaZRIKLaZBaCaQCJDSLYLIbIIRRTQZKTLaAIKCTaQDJSSYJJAZIDRITLTTDQIDBQBSaSAZTJDKCJJJYATQQDRBRQDAIaZAbLJJDZQZKRBIJLaCJYYLRKRABYTSQQCRQKZLZKbYBIDCIYCILCDRJKLJACTKLSSYZQISYLKaIBQIZSDDLAQBQISLZKaQYRTQBTQbSDQJIZDDSZBDDaIBJZRaIIKZKKCYTTBaCRCbLQADCICSLIbSTCDKbbIDZIBJIIIZbabbRQCaCJSbDRLTBRaQYRYJKQBCBDCLRSTARIbJaQIaJaJYbAYLKJbRSDARAASLaTQIQQDLITJLTAbSCJCARIJJKbKZQJIIKQRJKDKZISTSIBDTKRZBRIDSSTCTaZQAQDaTJBDKCAJIZSSKYbALJRaIBbTLZZJZTaAATBBRCRZJJATRCYBTBbCCILZTZSCTJSBCTADbJCaDBIIDIDZIaaSTQTKDBKJILRYCIQRRQQZDRDaIBDLRRTQKaRbDZBICZIKaLQZBbKCBRDSZACTTYLTKARDYJaLITRQZACRYLBALbQBKLQAAAZJTBYaZSaJLZKDBJQAJRJJSaCSJBBLbaDYBIAQRbRISLCAZRKaDbIIaCZCabYLTSSRATIJLbaZRQLbDASTLaKILTABCRRSTCZCDbSZbZSJRAKLaJCDDAZCKKTYRZKbASQRaZDSabaTTKbTYBIQSZTBYYAQIYJYJCbYDIAKLSQYTLCJYbKLSSTJYLZTBSRLYIJLRAKTYLKSRTZDIAASATZIQCDKICRJJabaDbZJSDbYLaKSIJRCYDBCCbYRSbBSRTAKJZLSKaLCbIaCDbbARSBCJbQZCKDJTJDTZbLQCTSQaTSZJRYLYALbRKADZBDBKQbJYCSYYaDSaLJaDDaQDYbJKbKCbAYTYaRJSZJYKDIDIIACCCbRKbTSbLDbTAABaSSbSDSAZKBZDbCKJJTBSZACLLJCATbTbITLSDARbRLKKDQSYJCDISYSLBTRJSKTCIJAQYBSSIJCTDaIIKBLBRKAIDSZCQbQJAJBTJbJIYDSaTRJTTbKAJBabIDABTCZYCYBRILDRLYBCLKSSRRKDaCabQADADYJLRISCAADbQTKDbIKSKACRbJZBRbaDJIBAILbBbKRCZQYLTAIABCZCDZCbSCYDLCQQKAbbLCRILZaaLRSRILSZLTaZYaISaISDaYTYYaaAYaDCDZCCLRZCQBIJbBbRIBRRQBLbZLKQCZCaYaLTTJbaTBTCJARDJQAIYYCbJSbIRRTATCbSAILRKJCCJZZICaRALBKLDSDZYaBIRSATIaLJQYSbKACRKSJJLZIKZbYaRBSSCSDKYZaTTJSSICSTQBZJbYRaSDLKZBTLBJSbKZASSAJRAQAbABLAIABCKZTJYZaZICLDLQZaQYbTDDSRQCLKQRAISZDJQBZbZQRC"},
Embed{99,316,"AIKILYQIAIIIIIKLRIZIILJQaCLDIJIIYBbAIIYIIQJYIIIAIIbRDQASTBTbSKDTDTLTLIRTQRTRTRTQSTTTTTTTLbLTTDRCZTTLSBaYCKSaBIbLRYbaBTYYDSLAbCbYDRKKRTYITBSITBaSBYTZQaCRQZIABLRTbZDJSaSYKJAITJbKCbSJbAISABYKAabKTLbARYaATIZDKYDaKTDKTIBZRZSISBaRCZATLYRKYRbbBaAITIAbSKCBBQbSQZCSTCJBKBJbZIYQQDaDKCBYTKTJABYIIDYYKZRbZaRIRaKBZZYATDRQKYLIIDYQKRRCaJSYICYbaABDYTBKAZKBIBJbQRDQbCQLJAZLZCRQSTIZBYSDLKLCAaTJIaYYKRaJDDQTAbJaACaLKRITATYDBTBRYAKLIbDabITKAJQAAZIITSLKbCbTCJaCSJBKbADDbLKSaKQRLRaJTDSYAZRKJQQJZSDQIKTDBBbQZTZDIRaTLCQITDYRKaIRTDDTTIIQJSLQIAILDBAKDYJJCaKSZABDbSLKISTLbLARbIaCYIYSTCDZQICSbBLZTBYJKaATAaBCZRJAbKJbRBKKTCIKISRYBAZaDRDZBTAAJJbQCYTbbAQATZZRDALYKLAAaTbTQJaSTTBSRaRJaJIZSSTaQBCLLDbYbKIADLQDbQTALJBCaBBISKRRaATBCLRTYaQTRBLRIZbTQDTYSLaABaRZAaITYTYaZBDIDSaZKKRbQSYaTKIBKaZRSYITQYbSbDLQZYBLbSKCZYCKbZYBTAQBDYLKILBRKDbSBJDKKQTAIRKYCQLaICJZDLTTZQICbQZDZTAYYQLDCRLSKKAQDJDCDTKLYJYDKLYLBASLSLKQLIBAZTBRRCQZRLCZYaIKRTIKCIDaaaKTAAaJJbQDIabCKDCRQbZALJQaKQBZQBbTKaLRCaaZSRIYAAbJLQIbJACBISbKaJRaSZYCRZRILKAZSDJITASZQaaKLbDLRaaDaZDJDQYRAAQZRaKCYTQBDLbBAKZSJBABTCBIYbICRZZDCbQZBQKACKTbCbQbCQLLbaQQJSBIZASIbLLKBYQDLaIbYKYYaKYJATTYACYSIZRBAJDDIBZIZBRQKZCTDZRCTbYYTRBaJQDbQKCIBDZaDRYQDTTTKZaaaRTIZQRIIYaJKDYbRaKaQRZAJSAZKaRaATaLQCDaBTbBbQZaDaIJZaTDDRbYaKAIKTAJSJCQTJQIABLKABZKZaCQJRaTAaLaQSQTCbbLSBRBbJKIADBYTbJTLZQYSQSCBDZTYBaILDTIaAYQKbSBKQbaBZbDZRKQQQZTYITRBLKTZKRQCDJSBIJYAbRQCSTYBRLLTZSLCQKKaaDQQZbSJKAIAJDaZQYBLJZDDIYbBARaLZYBZZKKCYIKaCIZKSSKZAYAIDTITBKYIARRQRJQZYDIRYKDCAADTBbCDLIRDaZDDBQKZLbSTQCKJRDRAbQZLbDTRKYDBYAZDAbZBLLRSRaQbARTDBYQACLSSbACLIaQaZRJZLTQZKDYJBZQZZRIDaKRKSSTIIYIIIJRJCKaLRJSQBQBaabDYBJYQTKAaJKaYKYRYISDBTbIKKIAKILDbbaABBIBDKLTTAZbTYBSbSYSADJARBRILRICZAbSQCLDSZaCZRKbRCTaCQaLRRKLBKRaBJbBBDTKKbbbDJLSSSDYZCJaDKQSaYTSARICLKZTLaALJCTJTARIIbRaBJSTJQZaaKYRaJKQZbJYJBYYACRLTICCCCRABTZJCREKAQTZCbYbIJJSaDIRTZABDCKLDRYbKCZBAJBYLKCAYICYAQATSbLabBZTKADCYTARLICDDKKDKJSabYJYAITABYKRITYICLLIbJBZQTAQJbRYDTYSTQaTCRTYTAJYYYJSSBISZDLBBRBRDBZYBKCLYYLZKQLBRYZKQQDTASbbRSaDCYLTDJJITDIBBBIaCRaASBBJLZRATZJaCBBBbSJQZaSBZYLKbJRKAaaQRCLQRZQQCSJBAbaTQDBBDZSbLDDaQaIaRDCaRIBYZDSBYaKQZSKLABTZLZYCLKTTARDjDILYbSIKRCaIJRYSYBaAbLSKYKDZLSILAILSYDSRbYSKIZJLIIZAbaADAEIbaSJbIBRKbbQTTCSaJYZSDDDJJBBBDJRKZDTQYSQCKaEjDIKBKRLBBJLIaASLICIJJYBCBCZTSDKTYaSTYbZSLYCDaQDbYDJRTALABBKLZAUSJAZbTTIYAJLKajJQBZaYTBRDCCIabJAZAIRQbIZQQTaQCaKJAAIbDLRTRLBARLYSJQLbaLKZSYAaDJKDKJSUZZTASaLKLAiIIBAYCTRIaILISbZLZJDSTYYDRZQSbARTSQYZZZQBYBSLSbIJbIDDJLZQBTTQRCCDADQIRYTLZbIIBKATCLRbSBAYKCBBIKJAYDSbaZQADYJQSZaKABKIIITTRYKQaLRIKDJZIAKERDLYbRRbIQYYZBCaDKbSDTLIYSLDLTRISCIBDITRTLQBBASaiaJSZBRLLZSRKJbQSTZATZDALIKBaAZQYLKDZQRQIBbaaRLaKkACiCITBAQZIZTCgRQbTDTbYJaJJTJBATUASZAJJbYLJTCZSQMUADQQRJDRIBiLIIQIhAIQUZCTiLCgJbaJDJCLBaSQYRJTDZLaTbSiRLgJDbAQKSbKITDaUbbTRLQDAZCCTBKTJKQKRJABRSKaLJYLIEAZAYTTYKJhIBaTKIACADCaRSDcZLQKTABQLhLQQjjKTLJZgDMaZKaCYZZaDKbZLCAYTARQCiBLBLbRAYYSKKbTCSZDCCIJQIDMaBDbbIQRJAZZBALaBKAaDIAERTBKSKASQCIaaEBbLjLBJQADaLDQJCIKYSDiDIaQQIICSLLZZSAUYbQAJiQSIaZZTRbDbIIaDTJIBSETLYALiAbbACEaUZAJCQLLjEKbMBRCKabDjbDSIAUSIIIBRZbCTRJSEKSKYAIiDZYLbJAZDYJZDQCbSBJbQRDEZYLUSgSiASKIBKITaYIaYBDKBJhTRcKQTTBUBYSQRKJCKJYLACBCjCSJCJRbgYZQAZZYKLLALTaLhAAAJaDQCBKJgaQZITLTRCibJRUCaAQITKchZZEIgBYLDBDJRDSAhTaYbjhQggTJYALaRbCQaTAcbKYMKSCSCRYDYTJDRBhCjYZLLMYLTSIbJDCLLiCiEZEQSQiaCATIQQbJLCLJbQJcgSaMBRZMYZQCKAMRjTcCQIYMBTKDRQUTSQiTASLTIaLISJhZADKaRZbCILAJgUgKEZiTZSaUJAbURCJALQZDiKLihKgCSUYLAMTLSQiATTSTZIZZDQIQahLMjSYBZibRiJJCDKgKTDghDgSZZZDUIZRZIZSCBAScCgbBALcbjQCiUbhjjBLRYCQYkaIKIiJBYJKQiCRbELZRJKYCDZCLESgQDCLTCEJcSIgIKAaZMIaKALcBBYAbLQBUCILTIBRTIaMYJcATLDRMKUickgCJYIMiLRKTDSMDEhRabIDUJDBTCDJYiZUITLZTbTSYiKCEBChKDCEUbbQSILiUbJMCZLCDDcRRZLRcCQSTJIjbLhgJihJTCTJjkJCZSbCaCDUZDCAQESJDgAQIMASDJcKhLTahACjaAbiYLbKiZBERRTakUZbYBSAbCLiITbJSJSgcaCJkIIRDQATQCERBThaBRECcaiZZEJKYUigLahUcQcLRBAKIaSEjZBACLZDRgZSSJbRSBbJIKcEjICDChKZiaEkIjEIkcILEYbaAUSYIRiAKSDhKUJTkKhkJhDMjBDCZjbQRSEURRhTgEBhTIaKACRAZYZajYUSZiBKEBbibBDkaIiMcKJkAISDLkQaSLYBLbYCCaTcECUJLLUBiQZhLUALaTbIkUUCbTKZkgZBSRbCkIjSZDTDLAUYDbYDbYEJgZLDKAEUUDYKhTKEKEiILSKcUCjkgZRakSLALERDBahICMELSLYCJAKkQaMZLjkbhKDkDMLEjciaiQSCMIAbRCLDKRAKAKMkBgMSKbbQYTacCJDhSIjcbJKBkMQJcgLLRUkTKIjCJAZaLbAYCTJKICjYQkBRcKBMESgREcBgRaUIihJiIkRkbjKbRMSQDUILDKMgEjTAURISLIaEZUZZRcgDaZhhZQIBbjDYQISAYZSLRABbQQChaKRMULYgRKLgkcYEARZTkKZgahEMCMJUkUcMjjhKhCgYLLahRDEYRbSjQCCLYhEiITAZAQSMATjjcccDTKiABkKiachZZCEQhQcAbBZSSabJAkkiCIbCATKQZZMkMahERJQiLCRCUaKbJkLKRYAYKiYbREMKTYUURMjiMUUcbQhQIiBigcBBLcSAUScQbDRbKMbiAMSYRbJMCQjTcUbYSIiTCgUDLSKSAMMQMachBAUQIZJTDRgYRSbhhYaRLUIUDDcKhIUkIcKkTkLTQiJUgRSbgcUJKRIhJYCCShTYjaECZabEiTkjMgcAIQIJJKLkEjDCaQMUARkJhTAQbZLYRJiKhTKSkMUjJAJKaERZSZIcEQgEEIkgbIbgkgUCgZbAaZARBkARDcbZibSKZMTZSQCCjTEBbaUKMRkSMMSchcYkBiQERJAZIIYLTkRkJUQgLiYZkRTDZECbgQkUihQUcYYgcRILIJUQSjQkjJLiRhTYDDYMAgiRAcLbhJRgCISAMjKcDkAQBhicMYbkTgTkBLaSkIajaabLcDEAcLkIJRhKSZhJZASkcYbcBMkEQiCRUEQKiBAJaACJJcjjYCaITjAQZUgQKEMiTkEKkJKZgTYMULTUCgZjjDUSiIQbDkTKRLLEcERhIkZikgZgjhaciITKCBgQTRgZBkIhMIZQAUSCTajBLUhMLkRIkZgCiLMjTZUUDiKKITKCMjchIRAicbTacURahBLRBgZBTACgIIccaSYKhCCDBDjRJEhACbKgbjKLkRcERZiCJYQYiCZZhTTTLQJKhRbQBYaSgBJUabUahLRJScAKcRTCTiBQYDCCbbaYhIDITJYZgKZJYaBDhKSQLiADZTJSYJkBgihBgAiMTUQaAQDTKRBhEaQLbZgLEBCACbcLYSMLAUIhZZAkDLYDLTLIIIQUDEATCZTMQbSLJDRDJKbYAiBKjUCSaLZbaKKDKgTgQTKAjhUhkaDRIDJQZgJZDAZTKbIagbYiYkMJYMgEJYiUAiCBAAUhbaDjcQDCSbbUbLTcbLhAUKAIbQBUCDAkTDRbYAKIDJQhCCbcTYcIZQMaSLaRTRTKTZQgiBCbaKBQQaMiDYCBEaAaIcbMZZhcDQTcDTULDkTTKaQQYJQBgbTaaMMTYgjDJQLIBDLQAjkKALQMccQAYSLITIhACETCiIbLJBBaZSBZRICYRSIbiZEYZYBQYCQjJLJCgUcRahTiBcTYTRTCiCKJIQKaASJcTSMYJQBCLbhMDbJSABJSKYbjSYbkUTCQRZhQLbbKhaAAYaQIRBgBIYADIEbLDJUYDgKTLJaZbQKCQRKTSUABUCSiJJLYRAJEADgKMKDZTSaaCCZIIYDjKJhBKSbZjSLAbCKJKaZUhJabCRZaRAZjZhILIDRJKgLgBkSQkbgSSBDDaKQaCcADCCCTLMKbQJSBLDSLRhBTQJJTIcJBJRbTDaJLYSTKIZATRAabSAJYBQQRLJLZaMRDYQJLCZEiCDDiAALAYTIUCRhZhKZhjCZajBgRJBjThCRaUMRRRBBDSBLTICaKaILYaARJgYQKUbQDaLcRKTAaQbBSTbUaCATASYJCahCRbCDBKYbiMKhaRSQYQLYRKSbZQSYLBSDSAjbLKjDgUEaaEBYbIJbCEMSiLTbbQATjbZZTCLgYBRJaRaiIRRQbYSLBILEZCBQJcbDaCLSIiSJLjMCAJLRLILKQaBSAhCYAKBQASYChJiLYbRaLRZSbcbQIjJRICaIaKTDLbIAQSDIAZCBKISDbYZYQCEKYUZRaDKbgjDBKBYLbaTEIUIBASCDIJTTITAjRTCRIKZTQKSCRCaALYKcTIaJZJbYCKRaLKCZJaZKTBSDbDDDiZARaQASKADKTSbSTIcYYbTRbKJbRKLbRQhZZCgSBiaIiRBZjCTLTZERUDJCaADBbJJQKbZJBJSYbDSCcZRCLADCKRZQaUJagKJcZYRCSIZBBQJYIaaCKTTQBDZYIQIKRZBbLhLUIYJKDYLSRISJTTYSISDaAQYQiYLZCYUgSZaITDJJgQaDSJRDBYbBATZYLJaQRMLJLKCYLDQTIJRYTBKJKbBYDBKhJJDJBCgYciDCaLABDSCjScBCgQRQCIbJBALBJAYSIEKRIbZYUbBDDQSUaTSRbcDZDSgBDBKgbBIYCJITJQhLZCBjKBCRALBLKCCZTDDQZRLaZjDYRiAQSJIjaLQZYZBATSYMLSADEYbSZDBgAJJLEYJAbTDYIJBZILSLSQIYBASLBLYIKaJBaSDITAJZRaJISTZBRQjLaIYiQYCBALQSBjbcLQUZQQAAbDTDTJCZRCDQCYaRaYCaCaSbRDMbLhRJALCgRgBLCYQbZJYKgYbEQbaDZbZCKZLTKDIbTRaLKIITbSDSCjQBRSDYKBJKKSJQTZJcQCKKYDDYRbKMQBATDSRYbCZDIYhLDbZRTIbaYTTLYASKYaYDIQDCTaSLDRbZBCjDBZbBcbQKSEIaSIATSLAJDRTYKCZhJQQJYTTCbYEYbASTKAQDISTKZBZjKZRLLYIcRBZRSQSSIRJZbLSSSZCUKITbRLJaDCSRSbKabZDbJYSCbZCYJbaCaERRaYKAYBAYIAJASKSAIBTRaLSTQRYEbRbbRDbTaIRLKSCJbSbBDKbSCYAYKQTLSTJJaIAIJabZbaaIIZJAYKKBIaBAbJCBIJICAJJLaUIYRDKaCIAJLAAjZKbJRIZAKRCZILCZIDIaYAYAYRYZKSQIhTaaBKDSISLKKJJQAYYJYAYYDTDRRDAZZRaIJaZbACAYQTbSLIZBEDKiAhIacbYALAaCDDRRLbaLDbLRYBYIIUARTSSQbIKcYbZSALCbChBDRDSAKMISYLSbDZQRIRTZLEBTYZBZDiCDbZTRSBATSIKSbDADZDKSKZDIIYZJCaQYLRIbQBZDbRQIAYDYYSTJBASjCaLSKJDAAYKbaURhQYCbQLbDJAIALKaRaLIBQARSBaJbMATACZKKSUYZJMaQQLbAZCJTYATbbRCBaZRbBRAIDIRbCDaAQKARTBbDCLZRbDbBLAAKZaAIEQQQDLbIJgSbZbDLJcIYKKSIZDITZCAKYJABZbITAYZbRDQKRDSbLALJRZRZTQCSSKbUSRaLATKKDjZACBJbQgJSZTDDbTQaJARDSKTBZKBRCETBIaCIRRKTZAaQaJQhMQDbQCJKBKZZTLbABYABDTJCIJYbCaTRILJQSBbCBUYBLABTKQIhCQLLLCZICCYLDSRKDIQCZDRDKCTKQZKZbBAaIJQAIJTQRYLYDaSBRRRaDTSJIKYIRbLYAJahUIQQZbQYZLQDYIKJaIRSZAIZLYAZAQDMACJARDDBCbCLZZQIRASSSJAIZbYEBZJLCZRASQjBbBDKbBRLALISaATQKRaTBKLYbRhQKSBJBDICTZKJbbCIRKaBaaaTaBYBYSLBaQIDRJBARZLCMAZLBIDBBLZSQBAYADICDCAKAIcLYQYSLZIaQRbJSLJKBRJbKBaZBCQbRSSSIYTUQCTDTQaKTJDZQRRKKSaBKZRDZDRRIIUaLTBKiDSZSQDTRLDQABaRBABKYAQRRBIQJBZJQJbBbLZBbBJBLAaZABBITBBZBRTZCQKLAKKTYZSaYDLbZSADDCIJILYbSDZLQKDJSLLKDDAZCAQJBZRTTaQbACAKbZJYBRYCaALSLRZLQaITQTabaSJIAJaYTATQJBRBLREaRCISJBCBRBBTZLLSLAJSBgKATKCBYSBRDbhDJRYaLTYCDJSIDZaSYZDQjQYDBSaaTYbTBCQLBcARIJSRaLbTaaJaiTTSIDbZCLJQTIQDELBJJRjLCSCAYKQaCQLSbLaJJYASLCRZQcDBZIIIBQSZICTBZZZZZbASDZDAKDSZJCJATZKKTYIBSCCLSZAIYAhAQDITTQTCULDDJTLTbADAIbQAaZJJCJZZZaQQYBRbaIKaDSALJKTDSYZbUDKQCJTLJaaCaKTbKCSRJQTQaJIBDbBRLRLRaBbDRQaRaYbIBCZCIYCKSQKKLABLDShZSRZBBLQbLYZBACaaaKAQIDDTaIQabJaJAQKQBaYCLSDTBLTBARaYYBLATLZKKcQJSYAbJSTZZJSaSbaLZCBBYDIQRLTYCYRQYCZJJDTQCJSYTYCALSACCbKRZZZLCYQZDDaLaKTbZSbISaCSSKSSSTSTaJIIjLJjYZaCATCKJIQDIATIYZbLSYDLARIDCATKZJKKCTKALYTaQLJKQZILbYaTKSAbbBRaaJCIDDbIYbiRQTACTCAAZaLTSaKDbLBRATbZCITRJLYTQCJCSSZJIKDbaLDJAiBbCZRSTYZDZYQCDJIbKbLAAATISQbRQBKRQDDAaJJbRQQCJLYAZQJYKbDYKbbRICTTTDbKIbCKaCDTbQZYJSDLbYaTaDTZZZaIKRJQRBLQbSCATBQJYQBaDKTQCJSKJQRLYLLBJTbQZIaDiARZcSSQDDILQQBaYRRIBQcKIISaYaIJSYLYQKJCJAYCZgBRAZARRLIZSQJbCQaKSRaQZQDQaSJDCLSQJLQJCLJRLTJSbYZabDCSBAYbTLRTBRSQLSbCALTaaTTTIQZKbYTQABCKDJDaCTQTZACATBIYRJYDAbZLLICDKDbaSSILIabQLLbbSIbTbCZQaDSYRCaTZZaSBIQARLABBbaDYSBSIAJDIJbKaZbABSALQRQAQZRaIBbKYSBCTZLCQbbALZRJYBALCaaYQTIRAbZRbaQBYDZYUCbCKSBYaUSCJZaJRjAQBRbRITSbZaBJbRCTaLJKBQBRAARLKIbiDRRLCKDRaSKDSBAYbBLSJZAIDAZbbQTCaaTTRDYBJIaKCTabbKAbBEbjbDbCICcIDbCTRLDKZJRSQaIZACYaSKCDQZaaRDTKDSKCQIIIZKKbhTYJCBKbEAJQZBRbLQbYbAJbABTBBKLBAKITJQAQACZabaabDCACYSKbbIKTTDYJYaZZKDSJCCbATTRbKAZLZbRJYaLTBABcDTZDYRASDKQYQaCJKJTKLLKQAITSaaLSbSaQKaQRYLCKYCbYTKIKBIQYaZTSDTQQJIaJSJJKbBbBTBYQTICQJbRYBAIQQATZSCBKJDIIBSYALDBYZTRIJJQLZRLIZaTAQAZAYTRAbJJbLLIbaKCaBDARJABhSSRQaADKIRTSAaLZKaDQUAIbBDBBaDLASYaITBYbSRZYKbAIYAIZIRbYSLjCIKJaCZACaBCLLLJSDYZaaBJITAQabSZTaRZABaARISCJLBTDZQSLIZAbKAYKCZbaaLDDAQRTZYQQJRIZDIARSDYDAQaDYZZILTLKBJRRTIQZAJbRSAQYLBQJRJAARBCYAaLTIaQbICJCbJRCYaCabITCTZaZCJSYCJBSBLbCYbRTZCAJLCLBLCADaCZJTBZKIDAbBLASRCbbBYKDDIKRTBCDYbQaAIaKAJaIKYSTAALZATAaRLALQbaaRKQZJJaSCSLLKZZaRRhSIZRLSAaILCRBKKBLDQaQJDJYYYBaKJDDIKRZabYCQTYBBYTIQACbQbRJSBYQBIDIDBTCbDJKYDQDIYKaIJZRRSTLBAbRQSBTITDTSZRASKLLTBDLQZKiZaRbYYjSCbIISaKYCBSRRKbDTKCaDRJSRZAZYADAABIQZJTILbABRCJCZRALSCCTYYKLTBQLBaDTIYAIDSLZYiLBYJKTaDYZaJALZKJICDATTSZTTBIBYIYAbZECABQDSCKaBSCZTDTBabCSSLYTTDZSbQIBBIaRKCTCLBBYIZYLRBLLILJCZDQCbZabJCaTBLRKRZTKAKbSADLZbAYJBAIASSBZCSSILYSSbZ"
"SaDDaTJYZQRZAAAIDiAJaTBQbDLRTaZSTACQbBIaIaKRYTZLLBaQIaLRZaZBYARICDDZCQJLJaRDYIDCRIDKQRDZCCCbLERDBACSRSTbATCIIYJZCSRRAIITZbDAaSBLCaaQCKRKbYLbAJZRKTTabIBLDKIKKCQYbLJDSaYYCaYYSLJJQRSTAILbLaRICZYDCQQaBIZbTKQaJCZIZCICRZIQRLJZDaZLARaIKKSDTTbTCaBACCQbDQRYBLaDSSYDSDLADKZIRJJYKIDZICaiIDRZTAbbDZACLaCJIKBDYCBSBSIASSBLLIYIIYRQLYLIKYQCTDSBbZIZTJLaaTLBLTDBBAYbLYZAYCbCDSCKQCDbaKSTAZSSbabLbbLaBDRITBCZDSABZKLbLZbRbZSIJIJDARQAJCDJDaYSaCTQYDDDaSDARRDDDBaLDaIZBJLTJbLRCQKLCSYLYaIQaBBSRQJDTYLZRbAaKSJaCQRKATKDaaATLCQTKZRKSJKCISBRLILKDCQZISKDZABDBYBQRSbQLbJYYZbUJCbaCJDBJIRQSbADJYZDRKSKYCRbDLCaDSIBZSDACYAAaIJbQBQaAKABJTLQLBTTDLRTSCAZLJKDYYbRCIBBLaYaIbDRAaQJRTADYBRBQYJRKKISYRDLCLAYAaQBCbIaYISaYLLCIJBADIIaaZBRbBaRAQYSLQYbIIZRLRTbSCIKLaILKQTCQYRARJDJAYYKYLRbSQbTRRILJRTIQLBbYaTSSQKQTYbRTIKRaZaKJbZCKKCSCSBDCYUaDRDaKYZDSTBCISDAZDIRLLCLIALRgZDIKCZACTRIBYJJSJSYCSCJDQRDAaAIJATJYaLbSKKZYLbSCIBZCbDLBQLbJQZQQQDBLAKAaBJIRATLJRZYIJaLACCICSaAbKbTIIZJKIRJRBaCBKCKAKISabZIJBTLJIabRACRKbQCQAJLCSCAKRDSIBYabJQZQZJRCYbKBDDZLDZbaRJBJILTCASSDTBaTYKQSJBIKaKSabIYAISCRaILJSbCALIKDaLQTTBAKBZLSYQYZJLYRBRYbRLQbTBaRKTSYJQQQKQBQAbbRABRbAYLLYQQTKSRIITRYYaBaSZTbKJQKLYDACLZJIRSJCAISaLZKRZYTJCYSbSZQTKDSSCDDbaQJASIbTbLZaKbADIRTTYIRQZbRSTDDRIaKTDJEaLaACYCaABCYKRJBYDAITBSbTTZIJJKAbLTaDJaKSTITTbLSQQJZaBLZIAADSZICKRBTSIZRQKDBYaYKAaKbbQLDEQDbZIKQBDbYYaAALILRZRQRaCaZLIAKaDAIIBSCRABAQBLJaIABSIJRRCJbDYbajSKKYRBSbQZIZKaBDRQbTRCRDBaZCAZKKIYSKCJITZjRLTSZLILZRABJJCASJbCBaBDbSLABBIISSBCTBQLBASIQRAbISQKbLSRbQIRTITBARJLJKbKTCRKCTDJbIZIYDaJRZaDDRYaTTADKZSCQYADaTKZRQBDZCSTBYZYJDYJAKSKLTBSIbBRILDQbIBKBQIYJQRDLDKISKKYKDRIDaJLDARKbQbABRaAKJQBbIZBBQSDKJYLQYKTIYYLBTJYRCISCSLTLRBSAKYbDJYTDIKKCBaRQQZBaQKKbbSDSZYKRbDLLZIaQIBBIbYADKRTUYbCaRZLaTTRSSBBLbSajIQbSKKRQSRBJaLTQYIYJACCAKRZAbbQLbYCDCJaJaTaTaRZKRBYCILIJCRZBJSCYZLQaaKRYCAIbQbTZBKCCbBBCTZCAIbAaJJaLZaIRICBYbaDSLALLQQYBSBSQTJbRIBICICTKJZYICKJZRAYAKJSZCJCRLQQTYSTIJZYbLJCBTLBCbQJABDDLLIaZCTDZIBSRSQQQRTaaIJIbJZDJQYYKDKTCZYQSSLKSQbIBTLSTJZaBAYSaLTKCDAaIaDLJSbQLZTZJAaQQaZYLSISSZYQJTDQBaTJRaTbIJJAYCZISIYbSYYQaIJJJILLRTSKYIIBKRKLCCCATTZDaBZYBAARBbAaLRIRaYYJAYKbTaDRARRZQKRTBYTbYRDSDQKTZAYZABKLZQRLIZZJRBLSITBYAbAJAQYDJAAQKDASYSDDDZLRbbbJCAZDaZAJTbSRCaLTAKLZSDJDSKKRYbTQQSCBLQICTRQbaLKLYRbSDQaJTRASZaILTDbLKDBQCKCBBaYBDDJCCDZBIaYjRaQCRAbCQJZQKTRDRbLYRaSaYKRSJIJBTCRQaJYaKDLQKCZLBbJDJYAQLIJBJDCICCCJKDTTDbITbaSBSDZabDIaaBQBTAIabaZDQADAYSYbTYQKaBZJYRAKRSaaCIIYbSAaATbYZTbbBKAZYBAYJJaSTQITRTQTaAADSBTbbKbBSaTIIaYaTbAJaTbJRbSCRLLJCYZTBBDZBBDQSZTKjDLbZYJTTLTQDCLKBbLIATCQYIDTCBCaBCRAITCbKCSaaSDKaCSJJYYQBKaYBRIZTBQbZaLYQBIKYILaAIBCaJLCQBQJAbbJSDKRKbaCbBYYJBKLRaJATRZCTYRSIaRABDJTKAIRDJjCZSaZBaSIRYZJSDaCSBJZKAQAbQLSBSAYIZYYACbLRaCLLIbZbDCTIaZKSDYDDSZISDASbBTTKRaDZKYTCQDaAYLTRYBbQLRKYBCBZYQJJZKRSJAaDbaBCJZIQRYBJAZCZBZSLSBRRRQDLATQTaQbCKKIZCDTZSLBaQJLISBBRDYTCaAaADbZALbIBQDBaIjJSDDYQLSRASLbRSLQCLBaaKJJZbYKbaBRLCSAYBAYKYSBaJSSRSaTJZJCBKATCZSRCATDBAAKLYaJTLYbLQCaaJbYRZLSaBSbQQLJIRYLDCIRYTQZIZJADIbDSLaDSKDSYSRRYYIZaZBLYTZCaCaBAaKQZRRRSAITJLSJaZICRbBQbZaDQATDCITDSRCZJYJTARTQbRJKALZCRTDRJSKTITYYIADLRRKCATLRDSSJQYYQbbLaIILRYBRIRIRBCTRIJTJCJbDCJRJbATbTQLSCQaYAIRSTKZYKIAIbJTLYLCQAbDTLBRJTCaKbDQKTSSbaSbDTBYTCbbDQLAZSRCaZAIaSDYYDIDTDADbIYIBZDBQISYALRBTbZKZZAJSRDZARQaQICCAZYDZDaBRaIbbAAJQALRRAabAIJRTQaYYDAbSSBICRTZBJQKaRZILKZKCaTBJDKRRLaaCSRYSSJaARRCJSbZIBSbJIaQJDKSKZCSZCIJaaQZCDBaTKSJbbLbZAaJSZBAKRaIbTLLaCZSBCCBYTQYQICbSJJAADKKALDbTZLQSKSLbLCSQSSIBIYTSJYITAJbDBSDSTJJRTbTYBLQADTIIBQRYCSZZQQbYDABDIaQZDBJZbIRCBTYBLKKLCYYQRbJTbKaTLIQZBbABBIYLbZACDJIbIDTCBBYaDQSLJRSLbACCKQaTSIYRDBLYZJQaICBLKaBaDJbBQaIZAbQLTQLCBKYbCSbCYYSCYZQLSRJQJDCbRbKRYJKYABaIAJbRYaLBRDIAaQTJBIBRYAIDCSJbQZTZKaaIRaQIbCJJJBRZBJJaICTQDQabBRKLYAaQCBQTDAKZASaRKRJLDYITCSZSJBYJJAaDCCZRDZTDTBaQSRYTQZAbLabaSQaYaJaDZCaCRTTZZYYKLZJBTBJKLADZJACDQBQaZLZSDJJICaQSSbTIYYZIIQLRYRCZKBbaRCLSCTYaQJTSCLCQRRIIIDLIDZZSKIIZTLDKRATAIbRLDaLTRLDLAICJCYAaSAAZBQZBDQJKbKLaSBAZTCZKQaIBYTTSRITZTZQRRAbCJKDCSJaSJRbBbZKAaCLKLDLCaJATSSLZLIRSCZQBaAKJLJRQbLTBYSRLSIRJQLSCYbZSJDACIBQCBbbKbRJBDaCJQIAQQSJADCYZDKbZYLSIIYBSCbYYSICIDJKBKTSCZZLBZTATASLCKKaIQTDKSTDLBQLLDKYDbBYabSAJKJBBSZRBCYTLDLKIbBDRTaDYQJCDKKTbTBKYADQAYIBAIRaIbSBKCbTJZCCISISDZIILZbSAbLYTbYBAJSCKLDDCCJRKKTaDKQSQZBKKARZCAKYBTJCQbbLJRBBaSTTTCIZZLYYSKQRSSYabLRZAKCYLaCRDDQTALDZKDDQASabBYBbARYQIDYKJDJDRaLDaTKRYYKRaIKCYaRKCQbaCbbIDSQTbZLYYYKYYLAYZaJBKSLCQCaBSDJKCDBTTAQBIaBDIDLYSICbYaQZDIYBaBbBBZZbSZQQTDbaCbaDYBSDDSQDaLYYYCBDSBICDaLIYaTaZSZIZSJJaQYaTTSKbARICDDQBRZJDaLbCcSTbCLKIbKZBCRSDBaRBLDARIBILTgTLDATQLZDaTSRQAIZaCSTKRJTZDBJAaaSYZQQSDLJQZBaQDBSbTaLLLTRQSBIYTCRIaALDKaLKYYDDBSABSSBYQDSZBCJIRBQCTaBIbJaLaJDJSZTCCRLCYAQTaDBaLCQZbRRDQBRLCaSSbBDRLLATJBaaaZTZBAaYLZJDACLDaaACBAAABDLJRIZJZAaQTRLCRKABZYSbJbLJTZZCQSLYILQJYQQTaCZQQTKCLKTDQAQAbLZTKbZLIITDJRQQJaQYbZbZCaJQDBDCQZADTbIBKSTAZUBbQYSQIILIJKaAQJJJSLLQJRbBYTQJYYZSaSQRBIBYKRRaLBTQbaJSIIJJaABBRABCZKIbYCSLKRaLSJZQKJJYZYYIIIBYDIIZSTQTYaDSRCRBDBKCAKbRaaKZKQYCTTKTJJJaQLZCLQBARYDBJAKKZJIIaCKAIbDLYLAaQTKTYYSAILTAQJRYKaLYbBLDCIKQQSACLZTCKRaQSBJTCTYZJZADBJILYTJTCCQCLbCJAJbbbTZRADQSICaYTLRBQZBRAYLRCITbAZDATAYJCLLDLSDYTKBLYRbIZYIJYSQQSAAaJRJaKCQLDRZQKDYaIQBaDTBCTDSAALIQKJTTBKYQASQYYSZTQTaZLaLJJKKZTTYRKDaCabJYbDbKaDBJTQTDRLDDKTLLCabRaYbAQTBLQKCDBKCDBKAKKBBLBISLZIJRYJaZQTSLICSaIJABSbQKZBTbCSbBSLTKKZCYTJTARKITLDZILbCRBaSAYZJaZISAKCTTTLSBAQYZKJDAbDbJBARASbLCbACBYRaKYDLZBJKLJRRSAbSYBYKYAKbYJBDATKbbKRACBBTCIISJJLTSQKjbBTCLLKAZRZAZBDRRBLYTYZQDBBICJbQADASSZTSCBZSRbCKCRLQJBBSKbDCLZbJSYRJCQZCALQDTCTLBLISLDYJQCJYAKATLKQaJbCYZILZBDaBaJbbJaIRCQDKBYABRYCAJTBTLCTCDCKRaaZYJQJJaZLYAaLSYTIIISCCDQQaLQTDQBDIaKRCBLaJDaIZRQQRbaYaBRJDKQZRDbQLQZSBJDbIJKKaaDAAKbRRLaQKTaAIILTTKZYaLbKDABQYKARKBCJSSbLSLKRJaRDCbALKbYIaYCYSCKSBJRRYRDJCSbADYbYZKZDILBYJRYBYbCYTaQABQKKTJIIBDSIDABbKbDSSYBLQbQZKJDYKSBALKLQDAIbKRQSCbBYSIZAYbYZI"},
Embed{100,320,"IILRQSIIAIQIYYLQAKIKAYKIBLAICIZAaLbLYBKIIJZIIDLQIABaaaaaabACaaaaCZIaTaaaLaaaIYBZbCZZaKaaaCbKCCKaYSZaITBaZALSaBITDYDRDZTIYJRDITSBYKTBKJDZKaDRCSBaaJCQKYbbIBKDYTRKTBKQRDTIBbYKBRBDIYbKBTDYSKIaRTaaSIZQTADIQBZTKDRIITJLQJITaaDRaBRKaBaDYQATaBLQYLRKaIDaRDKBBSbBTAYKDaKaSTBDYLISaKbRSZLDaLKLAQZSAIDaaQJBSZKYAJDKIbTBJYYITTCbBaBLQJYBKbCQIYYaBRSIADCZBIQLRQbaaASLbBbIBTBBaIaBDYZLTQTRICJCbCaAJQZKbTBSIITBAZACLKRbQKQBKIAJSRDTRYARTKYDZKaZIKAYYZCLTTDSbDYSZADJQbJZCLZSSabaSLCYAJKQYILBbAKIBQLRYDaRbaCYABBRRbKQIbCYYJIQJSYaIBYIKRZYCJaKJaSSaTTYLKAIJDDaIALbZQYCLKDBKQILaAIJYAaDaRAIDRYKZCAILQYLTBZaCBTYCZSYLRZAJLBSACLbAYZTSDKJIaBaYDIRYYaCRDQbABKBDTQKZDRSDbTRabRCDDYbYKBRaQLbARaaRRRBQYKKKKbSAZBBCRDBDBTLIDSaZDICZSRbCJBTabZQIQbDLICDYKSLSJIBKbYYSBBBabDKTSDKIBBAaJADJCaaZYLaBRRQRQYQSYTYRDZISDKKaBSRLCJBSBBZSAKSYLabIaQTYDRJJLKRZKLQYIIDABKBIDDJaaAZDJbTYQDZBRYRKLISATAYCLbCbYAQDbKLYYKRYDDKTSZAbaTBBBaJbCDQDKSAKICJSDRYYQYJZbKQZYSKKQLLDbYLRYDZADSRYDYJTDIBRIDKBLSaQLQJZRLSRKRICKJAIISSQDYBaaRTITTBLIRRTKRKYZCQIDBbAJRaYbYaDYBSBaKYTRSKRKZSYaDDbIRCDKIaYDSKZYITSIKCYZAALICRDSAKIBLLDKIZZJKIaSLBKRCYRZCDTBLRACDbIAYBISTbZIBRTTILBJABZQSDIRDRITCCYDIDTCCDZbaAIbSAJaCJLbATKTTLSQSDKYBZBBJBbKRCRbABaBLDRRaRSDDYAJaRQRQbZCTRIBYCQaLSSDLQIbbIKaCJDYYZbDKZYAQAYZIaBbBLIYBICYJDAabLSaRQRLDbSRZCTICKSZQaZIZaKKKRaTbCYBQLAAJJYKKSaDCSJYQTKYYCZZATJabZbIBaRbBACBIaLaKYaCSQLbDBACLJYQAIJYZRIJSIRZJDYRRASCDbQTBYbCbJBTbZSIabCKRKDaQZTaBCIJYBZCZQKbDKICBIYBZISQTbRDSRaLIAYKaSZIaSBRQBbADTSYBJRaAbARDbCBITQLIQbYaLICSJTJIZbBIKZQLLAbICSYaaTBLIJDSLQJSYCLYaCCRDBBLABABQSQJJbJQZSLQaJYAIbLTKaLIYYTZQRSZTDbDIDKTBSCYATAZSZSZCABaBbaZTRaDbCDbRKBBZJKQQQTQRDbSBADRKBBZLZTALJIADIKCYLTDQKAabSKAKDbJTaAbSTBCTKRKZCDLYIbRADKbJKLBBTTABSQZYTBASDZCIKLaCbBSIKKIaCBDQLJCIJbQCDQTRDZTITTRYJJIBILaLaALBKIhASBTbbYCYYQBSTTbaYbSSYaJKaZKbDJSBALRRTSSRJKQIKaAaQaTaLbDLSIILBRbbAYaBTZbKZbLAQSSLbQKBBTZCKSSBKCTTAaKIQBLTLRRCKRLJZICZAIRCDRCSaaJCSaBRLaQKYLbRYbYTRZaKDICIZDKBQJbLQACaaZYTRDSCCaBDaAJRTYaBBDTLTYZCJRYYIJADIRbLSTaDRKISRaDbQLLCAILIIQAZSaITYTDaRSLbKCSTQLaYBCIICLBSBQTKABaCLQCRDTZbLQLaaBTZjbJIQQLTBRLRIDKKRIJIIQCaCBZAZaRRIIJCTCLRIbDDZCRaBbbUQBRIAaLAJAKYSYRJIAYTRQRbCQbARYjRBSDIbbTCaTbTILBZLQaICCLLZIKSaZAQIJQLBDChbIRDIYSJaSDCBQCCaZYJYKaDKBLaSBZDYDZTBZYIYSZZTLaSZRbTQIJBYALIJASRaZZAJQSbaJCgRCASLDaSLZJKDBJDKaYDhSRKabMIJJTQABaaDSDaSTTQIIJCRbDbAIBJaJCKRLARJbYSSaaaSLTIJSSbJDAKDSDZabKRJKBaTKBCKZAQBSAKjLbCTTSBbCKTCRSKSTBSLgZaJZYAaJKAQYaRCaDSLLbKLDYDgLSLQIKBQARBRZSTKCDbTTDDbKSYYZDZJLAKBKTZaZJQIcIKAJYCDSRTQTCBSbJAbaIIaLYRZAaLYKDbSCaaBBSaQKKYLaATDbZTIAJRhRKLSTUIZbTCBTTAIKZAQSBCQRBKQbbICBKYAJRKYJAAYLJaBbBYIZSBDRDjZCDLZZZJSQCZCbYSQJRBTYQQZMLJITCRISCABRKabAKTZKISJYZCICaBbYAYYJZBbRYDDBYRBMTTZQCYaASIKcQQBKaIIADZJabTJAACRLRZCYQTRaJYDARLYTKAIiTTQIZajaYAbQIIECbDAQDagSBaKKLKbbTARjLQIKILAQbQDIaKKRRQbQCLEaDYTbZDYJgIQQKZTKBQCYBLaBJKLQISQBCBDSjMgRLDbZRaZDISQQIIRZJbZAEaJAQMBSSjZDJZADgSQADaERAIRbBJMTYKSZBCBhbbYJaSaLCSIIBZZZRKRRSCQBbLbIJYaSkMSQRKIRSJRDSbKRCKLIaQbQYTTLCZYRScIZbBbALDRASbQiCCILDQDZYSDScDKBYSJALCSJJIIISUBbCRJSLJaJCLcRQZDCaJYaDSITbZCZDZAALSCjCDIaDQDUQbCBZTjaLCRIMabKSZTZRLTDTBISTQZCDEgBShZhgZJBQDDLCajYAaSQIbaKTZLCbSCYYAJRCYQCZDIbAZAIbZRRjLcJUAZCjBCTCcKBaaYQBhQSaiLDbSYBbJDajTYRTiIYRERZLLJaJSYShaZESQIQJYZBBJDEAIKSMTLTEbDgbTBJDTaRCTYgQRZhQLDbSECCaUKJYZAciDZIJBAaCTKCQDbQATCLSMSLBbJYLRMJKZbgBbSEEJTacRTLBCACBKELYcZDLJKaTYLAYQBIIDIJKKITDaEYRCAcMZRUZSDRZKbCSLRQYICQLLZaDDZaBZLICRSAiZckUADEiILRZQDKYZSaCiQBBQRKbIikaTQQQhSIZLLBhkBZBCBZRYDIQiiTLBbLaQJZATUigbJAQYRDQBSCBYjaMZYIEEJRiDjDTDYYBCIMKAAKajLgBLJTgLCJRbIJjChDJQIDDCQQJKQCYAUKUDKTbTbDTTkSEThYZECcMTRDTLZBagCMhAgQTEaaEQKZijAbDZhRJAkBCZYaISLiQESaLKKLJBAbJMAiUQMRTQZJQRQYKgbYRZMaEAJbEaCJCBZgJBKcQLESIKJJCBBITUEQRiSJEELYRZkSbIZKSZZaMCYBCjEbaaZBaZCEcDgZAhAbLCBZALicTBRBAAECLbTKMBIIYJMSIgiIZAIZcZBLDaiDUghkYQRSabBYDUZSBaBhJYcEaDEQZKDYKIJiKiLkZEJUZhZIQggiAMARIUbZJILKDEZEkMjMSIRTZYjiIYigALYZLiKAIhjASSBbLQSBaaZTBCBbjMZJCShaQCiggJQZQQZTJKYMcJiCTAbkKjZSgjYgMKaLMkKhDTYCMLjgQJaSLTYRMAhSLSBRacRhcTLgJRgBiKiDEiMDQZAIiITTRRBchUJKJTBhIiSJLAJRCaZAkLkgEUKMDIAYIRTgQcCSZSDQaACSDZZcabUTcSBcaAZQchTcRTLUATISZbRjUYMIJREkbJQKbCBASajgiRigIYRMQbUKBIbkZkbUAUcLQTRJjUjSKiBkacMMTabQRTALZbIhZARYKCBQTULBMKTDbiiEJaKgjkUaiTEbiiRIIBbZCTAabIMhaLBajAjALkUDDcRUDDMiIhQhLjCLSCigAMUBAMgLEgaBCKEQLbEMIBJhQMiBcYSbKSKBRcUQRDRKYbMaIUcLMCcgABbDZQSDJZZgLCKUZALRkDagUCBgYZhCMhECCMQUaDkDMZLKiiCLKICMZUYjYJYZBgAkQREDaIjhaAJCECZaigUiUZbDhbbKkaUZgcjKbBBJCiQQYbJKacKCISghYSgKQkLLSKQATbRRLjiibMhjMEUYDBDMTZEKQJAJRRMijBcBZbJUkBBCjjiUcUSSIDkATjCaTckjCYKZJADSAJLEkURJBLYLjUicUaTkCAEhLCSZiLYkAhiKSBhJjYUbBQQLMRTZMQKZcTCLQULLJAicZJAJYhZcAiUEjLCEQACUhKckYaSbbBKAgCjESRMCiZhjKiJKkDjjAIIYKcYUZkMQERIccaYCihQIhQZKYTaKDKQMUMRQBYaQEZLZMaMZibLEhhELScMUIkAkjRKjSZhAMELCKYULMCUCLRSRkjDjAaRYAUMMjLDjgAbiMZjcAaKghAKjADTQAbbSkCZQDLicTAihjccbbhkTJZJakCckQQTZTKIhacUASADhhbbMCZTaIjAZKAAacjQQUDEYKhbgRZDhhgcIRhEaUkYcMjASLbgUTULkcYhBEDCSTZgYcRQhLcUMTLCAYbBhhKBAMkhZQbQCMIaTcJEAcJgMDjYBSYQQkAaRBhhDDUEJRJEIkMBSjLcQgIkgCLCAQQYSbLSILjBJLCjchkTLKMbTgCkJcTjkSRkZUABQhAKMYKLRQQIiTJYhUicAbEcUYZLaIEBMbLhCAIAQkUDLhYkbQhhSkjSMjERiRSMCAJZDhRKZCAcJUjEEgYURSLDcYhUkigZKMjhLagUARMKCKSTYMDTiLRCAKcYaRQTEZQackKaRBMjchgjkAYMMiRbTkMiTbhcTRYTaTcKgEhICKDAMKLgjaCKgDSDaZJKaCajJRYKiThDiMMSbYaJSQCLahDMbTRahRgRCQkDSKERQQSDChYBIkIJIJDIEMABRMhQJEJAZCQaMQTgKTbMgbaBISAiTbSacBTIYjcUggYUbcTEYCIISLbhKBSaacLBULECbMgYRCcKLZBaCCAMDbCIDUcIBCgiiiRCEELaEjCADQBYbDMAQLMSJbJDJSiiMJcLAgDBJaEUMcDBCijjKRQhCKLLJbKSZZbMThQKTBbBKYijYECQShKYaTALTDEhSRISbSgKZgUSLTQZRSiQCZaUIhcASDALKYLhBQACDCRZJLMZDcEBkjRJibKgaCCQaLIKMIbTYAJJbhhQcQREUSKQRiRKghJIIjSLBabIJZSQEQaJLALaCMjQULJJaJYRSbaAbhLAKUZaAiDRgSYCQTbBRShAQcCCRaIhicQLRLKaZRLQQaDKgIchSDLbCAZQTLZZCIIiAELBkRIUbCcQcRLAbLBBJZKZgKcRKBcBRaBAbKKahAACBMTLbJAIRjaYKELUSYKBZLbSCSAZKTEaQbaZgDbLZDCiSLjRbbJaaUQQKLCCIQRQJUBQKQYDTBcJYbQaYTBLSASCJMYZARcjDULAkRISZKaCbgaThCTIcaRLIYaaKjSMSLMkBhCcDRSAIChLJTcIDBJCLSTCCTTTZcDRCBMAJRZQLKZITSDTDJKbQARbbQahbTKKKRJQUIiYQEDhLSCSJbAZYCaQAAAJIIbYaQBaZbMZCAkhEABTZJAYLgIZRDASTSZABMJDIUbLZLCLbKSaIYbhCiSUSEaSCRaZJCJShTZACSBjKLSSJcQDRJIQZLZIaAESISYRabTQBCUTRaBJLAKBQKKaALbUibiDRRSbATYTJKKKTKMaDahTJCLERCZaIQgTbbYZAADAZZUCYJZQRCUaiLZDjEKRDCACiKbSahaSJJTKZDCaIAKgAJZLjCgCZBJLTCbTDLCIIRbicJZQCAECQCTZRKLgERZTaKYJRJJjiAIYCJLTbLQRJCIQTTQbAJRbaYYgaaKaDKAJKgRMZSTSRLccTUjaLJCBJQabULLQRicBMThRSASYDaBSMQZCRIISaKYTLSIQICgYCZEbhLTIDTQZDKBRJKcRZDJDIJMDTADQJJQIYICTSLbEBSiJSLJAZRcAIJbYcQbZRJBDaQAYLUgAZkISJYAYihaMDaRRjLbKTIbcTQZcBaRSBZZQYIJLDZARSARRIKBDKQaChIaDaAaSQUTabZLbITUQjCUAZcTQIYICiDILBYLRbTRKRIQbAQSAAaRJJbEZIaIbJMBBCTaZYDALBASQJIJRZLSbSQYhTaIbLiYTAKSCZSZUUBZbJBATbJYKaRBBgbIDgbDjUABYCbQBZaASbDYDJQIZRYDhSAaKJajgbYQDQKYQQTBJESjccDJgJALDiDiIMKTLDJKYkDYAATJQBDBBTBbSTYaYTKYbTELMIRaRTIZYAQCJRAaKJKJBbJSBbKaSaIYIYJSaJAbILTTURIQTgUbaJQBCAUYLAbCQKAZKjJCAJRQYZSYDSBTKaTRTaaRZTSbBaaSLTCYDETCBZLSREaKJYJLYLbbJaYQDKAKTJJaBIaARAAQAASbEJbaDKLLRADCjDYATIabbIBgMjBSTDSLaBDSDBQbbALRBYZLaYRScADTASAKaJbYbTDcCJDJKCCZCZaSUZZTRTCDCAJBbIACZTQBQbQhBJZQYTTLJbcgKLAabILKRDICYZLTTBDLCaaBCIEbCDTIZDUQLaJLaRSAMIAAAAZRabLSaJaIJLAYQiZBYbQYZCIbQZLabAESJTSAARjYSSTBSQCBQTSIKCgDYRLiCBIBLLRaZSSbQLAQBScBTJZSYLJJDRTIYZQDIDIAQKARKTTIEgRSRaiYRbYZLZQBCRZaSJYacIBIjJKRAYTaEaYRJBCBSJTaaLbZYIRDDgZRRTDaCRZRCgIBJKZjabYTCIAahSBYaJCbYQLZbTLLSjBKaDRZLSRLJCQQYRZKRASBJIIZSaYQSLLSBSZaUjYbQBLAICKcKDAZKTILSaBAYKLSJjJaKaDJBBaYBLADbKAJYBQSKIZJLbCLLTJYCAZBQCLIDDSQbCCbQaLSAKAiIYMZCJTRabQBKAQDKgbTAKILITJASKSBCDDIbCIaCJZTbQTKbYJTIATAZSbJbCYLBLTbhScDhYJIIJAISJAYBKBACARbZKBQaTRbLaISTTQTILJRRDTLIaCRJCiEaJSgDTKDKTRDaaIQQISKKECZYKRDTIRaQSIBabRgQRQSKaZbQKKIYSYCZCYbJAZBKCBYRMLhbZRDYKbZAQChDQCYAaSTJJIQSBKTTKLLAbRZCCILCZJMRbjYQKaaZQQaDICCKCKBbYiIQCIBLZYTJIJRDZJCQbYCaIYSaIIBKZBabQKSjSaKZESBLECbCYKYITRACIaYITJLZaSITLQBRAYYcIRaILKTgTKMQKbABRCSDbDLBbLQabQJiaZQCZCJRQJQYZZQAbQZQaBIIiiDKTbLQKIDTYaBAZLbbaTTCQDAaLIbYKbTBJJABDAQIbARDKTaJISTKcaMQbaTYMSSDLBDjbaAYYBBaSBCZDTKAQbLITBJCaQLDaQBTRJQQYJJZQDSZIDbaLLAYIaAIYBbARDaAAaIYjSIBKQSbUbSYTSIKDZRKLBIZLRQCQKIBJAYTJiIITTQYIAYAbRCYTKRYAQaTJIRDKbJDYZYYLbJDAISZYDYYRJJJLQKDbYDbbQCKDDAIKJYYKJQSKKIJTMCJaTRbSCRZKSTQIYCASIDYRJLYTCIYLIDZZJbCTLAKIIaTKZKSQTARZjLCKQYDIZAKDTQCQYTCQALbLDZCKAJIAbULIbaDIRSRaKTJLTDZbJJQRbDQZbIBARLSBYYSZSKbaRJJJSQDYKTDSKJDALYDRTJEJZCQISJaAQYRUCALQZSBZQDIQTYDJZSYJbCQAJRbYBZATaJKbCbAbCSSZJaKSbTbIJDDDIILLDZCRbLKYaABAKSaIABQTSaRJKKDYIKaCDEJjBQILDIRKDRZbYTaKKADaCKQbBBbbhZCTSbTSDaDTRDDAYCSIIbLCbKZBTAQSaAJQKBYZLJaaYDYJSZKTIYJCJDQCDQbTDLDACbDSJQBZbBLRARbIKCJaCSCaILTICaBDDIAKLcABZabABSZbCRAAKLKjCIAIbBTJRDKCDKQTLJTTASBIJKBBDITCYDSRQbZZJLAJIRSbDRZbQIQTQIDbTLJIYADQJIRZIDSCBRaAQTJBQTBYKCJZaRBLYZRJLaBAETKRSQBIDAbDYBQRAJIDbJaLSCLYCYbRLKJDSYARYLbDATRJaJCSISCAKRAISJbBZKLQBTbJDQbJaIbbKLSSKQiZBBKbTZAaIITAZCYJbCRCaITSBIJJaSaCcRLKACIRSQIAAYLJQbLTaJaBRDIbIZRSCBDBKISDJYaYhTQALDDYSYQLBQIbDRQYYRSMbKALQDgTZZZRbbbZITDQaTLARCCbTTCAKQDKRgJZKIRSRJYKARCYRILRBbAADLCITCYBQLYJICLKDCRZICJbaKZbJTADaAKCIKKYBKAYLAhAZZIILbJIIAIYbLJLSCTSQBAbRBAIYDZSDQQEYaRIRLITaJCQZJYBCYJIhBbIgKaBZJTDbKDQSSQIRALaaZBQADYDZBSTBRDTCZQQDQQZZbaaCSYJLDEaTLbJYTZaTRSKTTDbYaQSiBQQLgCBADaDbLaCSTbSYSIbADZTaBTQIDYKAbYbaQLbASBJDBQTIIcIJTKABSCTBDZYLAKBBJLJYSLQBbDJYaaJLACQKZbQRcbQLaLCRYRYYaIZACZAZDASRCLIDBTbKTjAZKIIQaDbUZQETCYADSbbSLKBRZSIRCBRDDYbAYZJZCDZRLaabSaDAIaLQCYZTaCITARBKJTRRDaTRZIYaBhLCSBDJKZCYbBTAACDIbRQbBSbaCaJAYaTTILALJQCTSIIbaITYaCZSDKRTIBaZCKCaYRYKDZISDJbITSQaRYYRBLKCTCLSLCLSRSTYbBAJSQJbYRaTBaILabRRRBYSSBKIKZIDSBLBQJbTIKZRZKQTRACTZaAITYAIKSaDDSQbaIYBIYbKIBLbCZDTSJLQKALQKbSQYIRTbCRJKSaJILQKTLRKJZLQQRSbKKJDKKRBCATCZTCbTZJTQCQaYRTJQDLQLDaSLSYZJQSaTQbTLJSLSYTaBDZJQYBBZKDSaaQbKSYAbKLAZDYDCDTTRYJSKJZYLSZAYSLbbTLSKTIATSIRYYJBRLBZYTZYRLQBAKRCAJDJLCYIJSYaaZQBCALQAQIaCTJTRTaKZaCCYZBaTgKbbSAKJJabSQRKbSBYITYBSDSSDKLbYDRBEbYTbCDIRaAQDDQIZSAUbLBJTZKJAQYRTKJQLZTIZQKKbITaZCRAaYLCBTIKJIaTDATZZBRIBbSaKTRLSbTTYCDIARIIQTCDbRKDQYJhCbTICSCDRTZRLYJBSACCaRBIaDbZLABSLLCYaQATRDBaIJYIKIbDZRSZbSLSbIAaIJTBKbZbQAMCQLDBDLRJAYITKQAYBDAALKCBYBjJKRRBTDQaATaCAQSRQYaCJDAJKZIaJTLLBRKBLBBTBBRAIZaLKRASCQCLDDRAZTCLABCZYTQIYAaCDKLBCZJZJbJIJRBJKTKTQBTDCaYLLLAKJYARCDQBJIIIJZgZQQQYYSTSLBBYJCAZDJJKhQRTabTKLaCKTaBAJYSKSaYTZaTBCBRLIYYLbKBCZACTBZYaaJYDCDITbRCSTTZQJZBCbaaJTDSATJDJZAbbIRBIRYSQRRTTCbLCTQSSLLTADBBRZSRADZQDTQDYQCLJCLCSILTQCZYKKKQLabbLSbKYRIBBZBIACSJZRZYbSLaTYDLJSSZJCBTKbKJaARACjTILDRSYDCQYaLLBISILaJRIaITAZRCJaYRJQaDCSJYAhSIAZBILaKBCKBKJZ"
"QIaRAKTaLKKCRZJRASKaThRTQIRIbLBDRKIDAaSKAaKRLRKZJQAZDILaDTQDQYIRLZaJQCZbKDDQKQSLSJbZCTJBiQJSSRYALZCKaRKRBTSQJQBTYbaaIJZZQaKaJYTBSLaRTZQLLYQCaLJCTARRYDAACZBSKbILSTLYYSSSQYZAZLYAQJTJQTJAaJZTJRJRYBAaSIRJaLYaBAAIIZZbbSJQBRISIYCRLBCYYDCQBDRRLKIAYZQSSDZAZKAbTQYZKADKbBCSYCBaQaSZQDbcRLbZKCSBbKSRKbZaDKQQJJYbYSBABCLJBBaBCBKJAbDabLYKRKAKZKIaYCJbIJYTTCAACTRZKYDCbhALJDCaYQJDDQICCZSAYTLZaTaZKYaRSAJLZRCRARaTZCLAYQTSKBABaTRARAKIALRCIALbSRChLJJDUZSKaJAbQZYZLbaZRDSBZQTCaaRSTBLLRQQIJJCBZDBRIJRRTCCTTZRRQQaLbBKKYaQIBTAYDDYaZJaKALSBSLBCRDTBcAZCAbAAIbYKbRDJKQKZRTJAbCAKYSTBRRSLTbCYYRQCBBBKZDBbaQQAJBJJBBATTLaYLISRBbbIYaBDCTBCRTRbCLbTEIKTTKabbQCZBSZbabDBBSJBTQaRBDKSAbSCCDYYCZbCDYZBCQTKAYaAQALTKQLJJZJLbbSbTBSKSYYLTABJaZRDRbKRTaQbSISaACIYJDZLTBBCaYTAJADTBaBZLaCIJBaSSDSJKJbJTBKDaBKJAJAYQJKQSRCCYYYSKbBSDRADAbIRDZJLTBYbQYZCSYBSTIZKJLJICBJASYbaMATDKSYJKYSJIBZbTTYKaIDDEaCDZKCBAaKCBZTKZZYKBLRSSQKKRIaQLbLDQKJTZaYYCYKbBICQABJbLLCZZLCBZKZIJASRLYbIAbKCTSSAQJAIYZALaDaQKDDISBIRKQSRITbZIaYQDIQaYQCZRRLKRaRLJJaKAABJKQLALbILCCQCaabZYRQIQBIKBYQAILYKcTKJJbDIaRALSKKZQRTKbBZLQbBRAAIbKCDITYLARLRCTTIRCACQKLALTaDZLCYRLSYKIDbARbJIBTIAaRbYIDJQTLTbLZASTBCbYZAKQADaSbZTSBYZSbRYAYATKbADDKRZSRZaZALJZAKYTZSJZSYDYSBJaQLADLQbaKCLLCSIAKaQDJSTaTDLIKQASYbLABDLYQJZIDYBabCJADLBTLZQCCbQbSKKDRAALCSJRBQSaJAQaIZSaTTYTDJJaKTILIDZZYADIAAAbDIYbJbYbAZDDLYBJCASKSSSJDLbLSLYKJCRTACARBYCIIJJALJDSaBbTTJQSZDITYRZTDSJbQDTDDCLCDIJbZKaCIYZTSTIQLQYCBKbBDCIQYQLSDAJZAZCYKBbJbBbDDaYJJILAKaTTSTZQaSAaShLZZTCSJLSDAKZLLKaZKIKQAQSDLQIBQTahKIBKIQJQRIBZJRRYaBDIDBZTCaCYRQKRISZLKDQAYACIZaTKTALQIJSIBaAaaLRKaYICRIJDbaYCLTAZCDSaTSYbDCKDIZDAAIBBDRRTQSQDBQbYaCbKLJTaBbLZBJZLTYCKZSbDTLYTYYRaDQCCJKQaRIaJTLDRaQDTRZTTLCCDTTQSDALZLYCQAJQCYYbTKKDDbBTRYTLZIYLaSSRCQbJITTZQABaCABJQQYYZYRbRaKQZDaRQBRQIaJYTZRZRCSBBADaBBaaJZDKZDRRDQDLAaQbaRTQSRTBDBKJLJDcbCQJZAaSRDADZQCZJCBBISQKDRIbQQbKBZKSQTbQATJCTJKZJSQQaabLBYBKYbaBbJYIRRJYJAAQQZAbACJJRYJZAJDLIYbIDQSSIIbKCJQTDQKDaCSJDJIRTBYRITaKSLabRBKZTYIZDDKKbJYSbSKTQSZQBZaQCJRKRAbSYKaQaSSTLbALaAQASaDRbbSAADZJBaILDYBICQJCATbRTbbQRKRJbDZCACbBQARICTLIZAIKKYLTJRKbAZIIQBaLaKJTKCaJAAbSKKQYbTAZbJIRRSKQbQYSbTADaACRTTLSbAbQJbLTBbKKbJLATAZaCDKBBDIACRCaZDLBRIbSLKYTTQTSRDSZKQaZKJDIRLIBYZIKZIQIDKJRAAYKJQDabJQSaJKTARJAaaYSBIIYKBLTZTCZAaTZAZBLSaQSZBQSRYSbZDJASCJLTZSTAKISLKDSTbILQTYBKTQQIJYCRYYbTDYaJLLbSAbTTSTKAADbZYbaSBbSRZCACIjARKIDTbTRZRJQYKQLBZDRTDLbRDBQICJQABCYCAJbaIKIaJbaLbSALJLSCRSKKIIYYQLBTZYDAZTZJSCBIQDaTJZQbSSCaBRCJDbBLSZSAZBaLIRaJCJYTDbRAbCJYSBRJLKAMiCAbYKSBTLCJaSZAYaDIRKLRIBDbbJIaaLaQZRITSJKISaJKJBQDSbLKBQYCYRRJJCAZIJRTaICBSDLQZQBRSDZLZCBbKIKYZALSJKTTBbLQQKBLIRAaYTCSSbIKDCSRLYRJAQaDaQZTBDKRTaLDJTLAJTRTQKLQQARYDbLYITBCbRKaAYAbaLRCKAAQTSICYCISZYBZBCYTTLZCaSbDZAAQLKIZYKTRbLbBbYLaRQZCAYKAYQaZQSCSIZICLJZABQTbSYRRaTISZDIRbaRIILbJYKRIYYbKLBJTDbaALJRabKZYaCLKCTaDRCaIRCRLTaYSCKJTAATTYRYSaJACbaJBLAbATbbIDCbZYSbBbQCKITAaLSRQZALYQYASYJCBQYbKQRBABJJIIZDQDTbTKRBQLIDADaaAKIZJSQYDZDYbRCQTQYTBJQBTDTDSLICaCRLaICZDSKZCbYYKJYABBRJJCbBLBQTJQRKKaDLQYQJSZKADAKbDQBSKCLZZZaCBICKLbDLSARYYbSRaQKBBLZKaSaaQBKRAATBZCIBAJTLDbZJTQQQJTYaRSJBSCABLDZLJRbRJIQKKJCQKSKRZIbTRDIDYQLLaAAKSDJILQQRYaQDZDaLTQJAKDbJLYAJSDIYKKTABQbYTaAZKbQKLTJDSLKILTQLACKIQLKSYLJARBRQSaJaISQDBaRTKbZDJbKIBYIRAKbBKBbRRQLBDBYKACYbKQZADBbLYIJLJBYQaRaASYCaAATCQSIJIACZDJJbRLKCRDCCYCJJIRbKRRQaTZTQASbJIYbLTJKQZAbYLATQTIYQDIJADKSBAKYSRbbLTILIQSQYTbQTZYRRZIQDYLBCILKCSSSTSJQDTTTJBSJaRYZabLZZKZBRDZZLSRRBKCQALBBSTKAQBCBbBbSKSKTACJJJaRaYTZBZDBIIQYYCLbBZbQTYbKKKDbLYCZBRbBSYQYCZTSQKBZCJKAIYQAIITSTCJKaCRZaaLSLBCbAJTUBDaTAaTLZTLRIKUYLaASbARRIabZJTTQJQCSJTKJSKaaIJAKIIYDbLKQbLAJCQABSLaIZYQIJYKQJaYTZDaaDTLBAaLLbQAaIBYRYSDKDBBCQITDIBZCIZaJIDTSDaJaaDSSbbITaTBSbBISLQaAAZbLZYbYYKRJBDBQTQaaTCbYaRZAKRIbaQDIBBABLZTJYaBIBbCYLJRTLQBZJDQQKSYCbaaTBCSBYKTBASICZZQIQSSLARAILbTIBDBRRDAZLaQLIKAYZCZACYQBKIbDLLCIJaRaDYaQALbaQZQYSZIRYDTYLKZASRaAbCJJRabIADYJTKABKICJbIaRCACLSAbJCRCQYZBZKZbLBTaQKCBBKSTLaIbTIISYIBLCCKKbATBYRBJRSTCSSRTLIIRaISIbIADKbaKKLaLKJBAKCBIJaTDCABYISaIITaYbbASADRTKDLbTITIZBYCJaKCCKSLRZTTSTbaCDaRRCbBAYbIRJBaYbDAKTJRKIbQKAYIZBZCKIYQZAIBKYaDASZLcSYYTCRCYIZQKYKKALaLbCQSbJLTCCKIaKBZCCLZISSATZZLBZaQSQBKKKRBRTBTKbSJQZLYBQJIKaISZYDKCRKQYTQJLBKaZADbSaaJCbLDZQCCJIKLAQKKJSTCLCTCZZYDYAbIRIKTTKaJYQTQRaJRTQRCaDKDALCBaALDbbKbQADDIYRBTKCCTBRJLAZLKDDDDKbTaBQAQAITCKJYRKQDLaTKSJJSLAQZITQDKbLRbAAKZIRYTBZIQZJIRCQBKIKCLSQbTLTKZDTSKQQBBQbbBBQKZDZBSDKKKTQRLYbCAJSDKSIACAAZKSAIDZYADAabRaKTTRKaYQLLKDLBbRbLBLLKQYAJLDLaYKQKLBJLRIBBLAYJTRLbLBBATQQCRTYQCATQKAKISQJARZQYbQZYRCbRCASJSSRZAaLTCJaIJYBRKAaLRIZQbSbYLDRYATDRaDBIIBaZYQCJIKJQBCbaKITYbILLBRYQRYQSKbTaTaTJaYCBSQbbQLbaDYIALBLTKZaaCaKbYICKBARTBKSCSSJRLLJIbTSSbBAbTDDASQaDCQRBBTJJZKLabRBTBLZKIQQTYQKTbKSJYBZKIJTYAYbCTQZBLSAJZLaCZJBaQTSKLCJCTaSBSTYACKDJJQJDbBaSISDRZBSYbLSSQRCYABKBLILQbIBTKBCLLLDJTKAAZTRKDCSZZIZJRRZTCBIaYZBJbaRSaSaZSSTJSQYQISLaKBSQBQLDTQZJAabAJABIZQBDQKDQITAQQAIIIKabRZBQbabZCAabLISBITJQRTbaJDLBBKYIKYaLAbZDKZLTTDCASSaSBBJaTYKLBAQQaJTAKKTZaIKCKABCDIQKYCJBLZSKLSQKQRTRJYYTZJJTYLYRBBZBJRKTaYaYSQTRCZLYbaATSQCBTCbJaBTbBDBZQCLZSaBCZKRBLDZZYCRDQBIQbTYCJZTIRQTZSJaITCYSJDCRQQRIBbDJJRTZBTRIKDZBACLIaSRLCCLTICbbZbQBICSQKYATDCIZbIIbZQCLTYQLYTAISbCJLSLASJTJBKDQCJBbYJJQBLDTAIbKJQAASAQIQbDZYbKYbRJIRBAQBQCKbYABJCISbDSABYLJZRQbQbIbJCABQTKTTCKaBRKDaKLaZRKKSYYKZLTJKKLJBDaSQRBIQQZIIADZCbBIRCaBSSTZZaaIbKICbCZIDSSTARbYQKTaCRbLCTDLbJQYICTJDZYCAaQaYKbCABbDLYZKAYRbDJCDACRZTBQIKTbBJZbZQRDKBASDSKIBIBISTTYZDDaCIYKIZbSaRZBJADKDaDRKIACIYSIbKALAKbIQITDTaIYCZRaZKDILSZDJaCDKBbAAQSDLKQDbKRRTbTISSbRDCKSRDTRDYKaaIKZYRQIBTIJaBCKTbRaYKRTQJBDJLLADYYCDKZZAJaZSCADTQTQYBIDbQQLJDBaILJRCBDKDbIYSBQSDTTYbSLACARCbKIJYLbIJKZSaaaARbQYRLaRALCDbCAKDSRZJYSRBJSBIBQbKKLYKKDACJKQbTTaCAQBSJLZKSIDITSYJDBLLaLKRAAQBALRBDITIKLaIaLLSIIABSYbCLKBLTYRJZYJaRTZCaQQZTAZDTKbDCaBIZLKAbJQCSCYADbBQDbCIBaZaTZKabTTQQTKDCQJZaJILQTQDLRBKYRRZQSZTDCIbLBRTRRSQAbRbbJDTZSSIBaJRILCDBCBDJAQQLSBYZSCBKIZBCJJILTAZQSADRQbLTRAJaKAJBaTLTKKJSaCLKTLYTDCZTATbKKAbABYJQQLAbRBDDLYYDZSbYLSbCIJAQbbUSLaBYBRILBQDaIYKCaYSaYYZIDAQbKJRaYbLSRYARAaaSKSCKLQCLbbLJCZQBIILIDaTTCCBbRLQSIJQAbKaTJCZZYILZYRLIZJALYKTSQSRBaBbBZSBBQKQAabCCSZARAQJJLZZBQKYaIbTLRCQabLYLBbIQAKZSZCQZLTCCYIJSKJRbAJSCSZDQKRaRJLBCJJIbSJYSYaaDSAaJKIISLDLQDT"},
};


bool Decode(int requresN, int requresEstimateCount, int& gotN, int& gotEstimateCount, Eigen::MatrixXd& X) {
    int targetIndex = -1;
    gotN = -1;
    gotEstimateCount = -1;
    REP(i, sizeof(EMBED_TBL) / sizeof(EMBED_TBL[0])) {
        auto& e = EMBED_TBL[i];
        if (e.N <= requresN) {
            if (e.N >= gotN) {
                targetIndex = i;
                gotN = e.N;
                gotEstimateCount = e.estimateCount;
            }
        }
    }
    if (targetIndex == -1) {
        return false;
    }


    if (requresEstimateCount < gotEstimateCount) {
        gotEstimateCount = requresEstimateCount;
    }

    array<int, 256> base64Map;
    base64Map.fill(-1);
    for (int i = 0; i < (int)base64Chars.size(); ++i) {
        base64Map[base64Chars[i]] = i;
    }

    int requireByteSize = gotN * requresEstimateCount * 3 / 8 + 1;

    stringstream ss(EMBED_TBL[targetIndex].encoded);
    dvec<u8> bytes;
    bytes.reserve((int)EMBED_TBL[targetIndex].encoded.size());
    u8 buf = 0;
    u8 bufSize = 0;
    while (true) {
        u8 c;
        ss >> c;
        if (!ss) {
            break;
        }
        u8 v = base64Map[c];  
        if (bufSize + 6 >= 8) {
            int left = 8 - bufSize;
            int over = (bufSize + 6) - 8;
            buf |= (v << bufSize) & 0xFF;
            bytes.pushR() = buf;
            buf = 0;
            buf |= (v >> left) & 0xFF;
            bufSize = over;
        }
        else {
            buf |= (v << bufSize) & 0xFF;
            bufSize += 6;
        }
        if (bytes.size() > requireByteSize) {
            break;
        }
    }
    if (bufSize) {
        bytes.pushR() = buf;
    }

    int byteIndex = 0;
    int bitIndex = 0;
    auto Get = [&] {
        u8 b = (bytes[byteIndex] >> bitIndex) & 1;
        ++bitIndex;
        if (bitIndex >= 8) {
            bitIndex = 0;
            ++byteIndex;
        }
        return b;
    };

    X = Eigen::MatrixXd::Zero(gotEstimateCount * 2, gotN * 2);
    REP(ei, gotEstimateCount) {
        REP(p, gotN) {
            u8 s = 0;
            REP(i, 3) {
                s |= Get() << i;
            }

            if (s == 0) {
                X(ei * 2 + 0, p * 2 + 0) = 1;
            }
            else if (s == 1) {
                X(ei * 2 + 1, p * 2 + 0) = 1;
            }
            else if (s == 2) {
                X(ei * 2 + 0, p * 2 + 1) = 1;
            }
            else if (s == 3) {
                X(ei * 2 + 1, p * 2 + 1) = 1;
            }
        }
    }
    if (bitIndex) {
        ++byteIndex;
    }

    return true;
}


Xor64 rand_;







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

#define USE_MAX_FILTER 0

template <class T>
struct DoubleHashMeanFilter {
    struct Node {
        u64 hash_;
        CapArr<u64, 4> subHashs_;
        int count_;
        double evalSum_;
        double evalScore_;
        T data_;

        double GetMean() const {
            return evalSum_ / (double)count_;
        }
    };

    int width_;
    SizeFilterSegTree seg_;
    dvec<Node> nodePool_;
    dhmap<u64, int> hashMap_;
    dhmap<u64, int> subHashMap_;

    void Reserve(int capacity) {
        nodePool_.reserve(capacity);
        seg_.buf_.reserve(capacity);
        hashMap_.init(capacity * 4);
        subHashMap_.init(capacity * 4);
    }

    void Init(int width) {
        width_ = width;
        nodePool_.clear();
        nodePool_.reserve(width);
        seg_.Init(width);
        hashMap_.init(width * 4);
        subHashMap_.init(width * 4);
    }

    int GetSize() const {
        return nodePool_.size();
    }
    double GetMinEvalScore() const {
        return seg_.GetMin().evalScore;
    }

    T* TryPush(double evalScore, u64 hash, u64 subHash) {
        if (nodePool_.size() == width_) {
            if (int* indexPtr = hashMap_.try_get(hash)) {
                int index = *indexPtr;

                auto& node = nodePool_[index];
                if (node.subHashs_.size() < node.subHashs_.capacity()) {
                    if (node.subHashs_.find(subHash) < 0) {
                        node.subHashs_.push(subHash);

                        if (int* refCount = subHashMap_.try_get(subHash)) {
                            ++(*refCount);
                        }
                        else {
                            subHashMap_.set(subHash, 1);
                        }
                    }
                }
                ++node.count_;
                node.evalSum_ += evalScore;
                node.evalScore_ = node.evalSum_ / (double)node.count_;
                seg_.Set(node.evalScore_, index);
                return &node.data_;
            }
            else {
                cauto& minData = seg_.GetMin();
                if (evalScore <= minData.evalScore) {
                    return nullptr;
                }
                if (subHashMap_.contains(subHash)) {
                    return nullptr;
                }

                int index = minData.index;
                auto& node = nodePool_[index];

                hashMap_.remove(node.hash_);
                for (u64 subHash : node.subHashs_) {
                    int* refCount = subHashMap_.try_get(subHash);
                    VASSERT(refCount);
                    VASSERT(*refCount > 0);
                    if (--(*refCount) <= 0) {
                        subHashMap_.remove(subHash);
                    }
                }
                node.subHashs_.clear();

                node.hash_ = hash;
                node.subHashs_.push(subHash);
                node.count_ = 1;
                node.evalSum_ = evalScore;
                node.evalScore_ = evalScore;
                hashMap_.set(hash, index);
                subHashMap_.set(subHash, 1);
                seg_.Set(evalScore, index);
                return &node.data_;
            }
        }
        else {
            if (int* indexPtr = hashMap_.try_get(hash)) {
                int index = *indexPtr;

                auto& node = nodePool_[index];
                if (node.subHashs_.size() < node.subHashs_.capacity()) {
                    if (node.subHashs_.find(subHash) < 0) {
                        node.subHashs_.push(subHash);

                        if (int* refCount = subHashMap_.try_get(subHash)) {
                            ++(*refCount);
                        }
                        else {
                            subHashMap_.set(subHash, 1);
                        }
                    }
                }
                ++node.count_;
                node.evalSum_ += evalScore;
                node.evalScore_ = node.evalSum_ / (double)node.count_;

                return &node.data_;
            }
            else {
                if (subHashMap_.contains(subHash)) {
                    return nullptr;
                }

                int index = nodePool_.size();
                auto& node = nodePool_.pushR();
                node.subHashs_.clear();
                node.hash_ = hash;
                node.subHashs_.push(subHash);
                node.count_ = 1;
                node.evalSum_ = evalScore;
                node.evalScore_ = evalScore;
                hashMap_.set(hash, index);
                subHashMap_.set(subHash, 1);

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


#define USE_SPARCE 0

double compute_log_determinant(const Eigen::MatrixXd& M) {
    Eigen::LLT<Eigen::MatrixXd> llt(M);  
    VASSERT(llt.info() == Eigen::Success);
    Eigen::MatrixXd L = llt.matrixL();
    return 2.0 * L.diagonal().array().log().sum();
}



struct Estimater {
    using Queries = CapArr<CapArr<s8, N_max * 2>, T_max * 2 + N_max * 2>;
    using Responses = CapArr<int, T_max * 2 + N_max * 2>;



    Queries queries_;
    Responses responses_;
    NArr<double> estimateMeanW_;
    NArr<double> estimateMeanH_;
    NArr<double> estimateStdW_;
    NArr<double> estimateStdH_;

    array<normal_distribution<double>, N_max> distributW_;
    array<normal_distribution<double>, N_max> distributH_;
    int predMinSize_;

    void Estimate(int estimateCount) {
        ReadyMinSize();
        ReadyInitialData();

        {
            static Queries bestQueries;
            static TArr<IOResult> bestResults;
            int gotEstimateCount;
            auto X = FillGreedy(estimateCount, gotEstimateCount);

            int tryCount = 3;
            dvec<bool> usedEstimate(estimateCount, false);
            REP(i, tryCount) {
                int count = (estimateCount * (i + 1) / tryCount) - (estimateCount * i / tryCount);
                TestMakePlanWithMat(estimateCount, count, X, usedEstimate, bestQueries, bestResults);
                DoPlan(bestQueries, bestResults);
                UpdateEstimate(i + 1 == tryCount);
            }
            MakeDistoribute();

            return;
        }

    }

    void ReadyMinSize() {
        int minSize = INT_MAX;
        REP(p, N) {
            chmin(minSize, boxSizes_[p].w);
            chmin(minSize, boxSizes_[p].h);
        }
        predMinSize_ = max(minSize - SIGMA * 2, BoxSizeMin);

    }

    double ClipSize(double s) const {
        return min(max((double)predMinSize_, s), (double)BoxSizeMax);
    }
    int ClipSize(int s) const {
        return min(max(predMinSize_, s), BoxSizeMax);
    }

    double ClipMultiBoxSize(int boxCount, double s) const {
        return min(max((double)predMinSize_ * boxCount, s), (double)BoxSizeMax * boxCount);
    }
    int ClipMultiBoxSize(int boxCount, int s) const {
        return min(max(predMinSize_ * boxCount, s), BoxSizeMax * boxCount);
    }

    void ReadyInitialData() {
        estimateMeanW_.resize(N);
        estimateMeanH_.resize(N);
        estimateStdW_.resize(N);
        estimateStdH_.resize(N);
        REP(p, N) {
            estimateMeanW_[p] = ClipSize(boxSizes_[p].w);
            estimateMeanH_[p] = ClipSize(boxSizes_[p].h);

            {
                auto& query = queries_.push();
                query.assign(N * 2, false);
                query[p * 2] = 1;
                responses_.push(ClipSize(boxSizes_[p].w));
            }

            {
                auto& query = queries_.push();
                query.assign(N * 2, false);
                query[p * 2 + 1] = 1;
                responses_.push(ClipSize(boxSizes_[p].h));
            }
        }
    }



    Eigen::MatrixXd FillGreedy(int estimateCount, int& gotEstimateCount) const {
        Eigen::MatrixXd X = Eigen::MatrixXd::Zero(queries_.size() + estimateCount * 2, 2 * N);

        Eigen::MatrixXd embX;
        int gotN;
        if (!Decode(N, estimateCount, gotN, gotEstimateCount, embX)) {
            gotN = 0;
            gotEstimateCount = 0;
        }

        REP(i, queries_.size()) {
            cauto& q = queries_[i];
            REP(j, 2 * N) {
                X(i, j) = q[j];
            }
        }

        Eigen::MatrixXd XtX = X.transpose() * X;
        auto CalcScore = [&](const Eigen::MatrixXd& tmpX) {
            return compute_log_determinant(XtX + tmpX.transpose() * tmpX);
        };

        REP(ei, estimateCount) {
            Eigen::MatrixXd tmpX = Eigen::MatrixXd::Zero(2, 2 * N);
            REP(p, N) {
                int measureW2 = queries_.size() + 2 * ei + 0;
                int measureH2 = queries_.size() + 2 * ei + 1;
                int measureW = 0;
                int measureH = 1;
                int boxW = p * 2 + 0;
                int boxH = p * 2 + 1;

                if (ei < gotEstimateCount && p < gotN) {
                    X(measureW2, boxW) = embX(ei * 2 + 0, p * 2 + 0);
                    X(measureH2, boxW) = embX(ei * 2 + 1, p * 2 + 0);
                    X(measureW2, boxH) = embX(ei * 2 + 0, p * 2 + 1);
                    X(measureH2, boxH) = embX(ei * 2 + 1, p * 2 + 1);
                    tmpX(measureW, boxW) = embX(ei * 2 + 0, p * 2 + 0);
                    tmpX(measureH, boxW) = embX(ei * 2 + 1, p * 2 + 0);
                    tmpX(measureW, boxH) = embX(ei * 2 + 0, p * 2 + 1);
                    tmpX(measureH, boxH) = embX(ei * 2 + 1, p * 2 + 1);
                }
                else {
                    int bestIndex = -1;
                    if (false) {
                        if (rand_(8) != 0) {
                            bestIndex = rand_(4);
                        }
                    }
                    else {
                        double bestScore = CalcScore(tmpX);

                        tmpX(measureW, boxW) = 1;
                        double score = CalcScore(tmpX);
                        tmpX(measureW, boxW) = 0;
                        if (score > bestScore) {
                            bestScore = score;
                            bestIndex = 0;
                        }

                        tmpX(measureH, boxW) = 1;
                        score = CalcScore(tmpX);
                        tmpX(measureH, boxW) = 0;
                        if (score > bestScore) {
                            bestScore = score;
                            bestIndex = 1;
                        }

                        tmpX(measureW, boxH) = 1;
                        score = CalcScore(tmpX);
                        tmpX(measureW, boxH) = 0;
                        if (score > bestScore) {
                            bestScore = score;
                            bestIndex = 2;
                        }

                        tmpX(measureH, boxH) = 1;
                        score = CalcScore(tmpX);
                        tmpX(measureH, boxH) = 0;
                        if (score > bestScore) {
                            bestScore = score;
                            bestIndex = 3;
                        }
                    }

                    if (bestIndex == 0) {
                        X(measureW2, boxW) = 1;
                        tmpX(measureW, boxW) = 1;
                    }
                    else if (bestIndex == 1) {
                        X(measureH2, boxW) = 1;
                        tmpX(measureH, boxW) = 1;
                    }
                    else if (bestIndex == 2) {
                        X(measureW2, boxH) = 1;
                        tmpX(measureW, boxH) = 1;
                    }
                    else if (bestIndex == 3) {
                        X(measureH2, boxH) = 1;
                        tmpX(measureH, boxH) = 1;
                    }
                }
            }
            XtX += tmpX.transpose() * tmpX;
        }
        return X;
    }
    void MakePlan(int estimateCount, Queries& queries, TArr<IOResult>& results) const {
        queries.clear();
        results.clear();

        REP(ei, estimateCount) {
            NArr<bool> isTargetWs(N);
            REP(p, N) {
                isTargetWs[p] = rand_(2) != 0;
            }

            NArr<char> ds(N);
            REP(i, N) {
                ds[i] = i < N / 2 ? 'U' : 'L';  

                if (rand_(8) == 0) {
                    ds[i] = 'N';
                }
            }
            Shuffle(ALL(ds), rand_);

            IOResult& result = results.push();
            result.Clear();
            Sim sim;
            sim.Init();

            NArr<int> cols(N, -1);
            NArr<bool> rs(N);
            int lastUp = -1;
            int lastLp = -1;
            REP(p, N) {
                bool isTargetW = isTargetWs[p];
                char d = ds[p];
                bool r = false;

                if (isTargetW) {
                    if (d == 'U') {
                        r = true;
                    }
                }
                else {
                    if (d == 'L') {
                        r = true;
                    }
                }

                ds[p] = d;
                rs[p] = r;

                if (d != 'N') {
                    result.Push(p, r, d, -1);

                    Size size;
                    size.w = int(ceil(estimateMeanW_[p]));
                    size.h = int(ceil(estimateMeanH_[p]));

                    int col = sim.Put(size, p, r, d, -1);
                    cols[p] = col;

                    if (lastUp < 0) {
                        lastUp = p;
                    }
                    if (lastLp < 0) {
                        lastLp = p;
                    }
                    if (d == 'U') {
                        lastUp = p;
                    }
                    else {
                        lastLp = p;
                    }
                }
            }

            queries.resize(queries.size() + 2);
            auto& queryW = queries[queries.size() - 2];
            auto& queryH = queries[queries.size() - 1];
            queryW.assign(N * 2, 0);
            queryH.assign(N * 2, 0);
            int boxCountW = 0;
            int boxCountH = 0;

            {
                int p = lastLp;
                while (p >= 0) {
                    bool r = rs[p];
                    int qi = 2 * p + (r ? 1 : 0);  
                    if (ds[p] != 'L') {
                        queryW[qi] = 1;
                        ++boxCountW;
                        break;
                    }
                    queryW[qi] = 1;
                    ++boxCountW;
                    p = cols[p];
                }
            }
            {
                int p = lastUp;
                while (p >= 0) {
                    bool r = rs[p];
                    int qi = 2 * p + (r ? 0 : 1);  
                    if (ds[p] != 'U') {
                        queryH[qi] = 1;
                        ++boxCountH;
                        break;
                    }
                    queryH[qi] = 1;
                    ++boxCountH;
                    p = cols[p];
                }
            }
        }
    }

    void MakePlanWithMat(int estimateCount, const Eigen::MatrixXd& X, Queries& queries, TArr<IOResult>& results) const {
        queries.clear();
        results.clear();

        REP(ei, estimateCount) {
            NArr<char> ds(N);

            IOResult& result = results.push();
            result.Clear();
            Sim sim;
            sim.Init();

            NArr<int> cols(N, -1);
            NArr<bool> rs(N);
            int lastUp = -1;
            int lastLp = -1;
            REP(p, N) {
                int measureW = queries_.size() + 2 * ei + 0;
                int measureH = queries_.size() + 2 * ei + 1;
                int boxW = p * 2 + 0;
                int boxH = p * 2 + 1;

                bool r = false;
                char d = 'N';
                if (X(measureW, boxW) > 0) {
                    r = false;
                    d = 'L';
                }
                else if (X(measureH, boxW) > 0) {
                    r = true;
                    d = 'U';
                }
                else if (X(measureW, boxH) > 0) {
                    r = true;
                    d = 'L';
                }
                else if (X(measureH, boxH) > 0) {
                    r = false;
                    d = 'U';
                }

                ds[p] = d;
                rs[p] = r;

                if (d != 'N') {
                    result.Push(p, r, d, -1);

                    Size size;
                    size.w = int(ceil(estimateMeanW_[p]));
                    size.h = int(ceil(estimateMeanH_[p]));

                    int col = sim.Put(size, p, r, d, -1);
                    cols[p] = col;

                    if (lastUp < 0) {
                        lastUp = p;
                    }
                    if (lastLp < 0) {
                        lastLp = p;
                    }
                    if (d == 'U') {
                        lastUp = p;
                    }
                    else {
                        lastLp = p;
                    }
                }
            }

            queries.resize(queries.size() + 2);
            auto& queryW = queries[queries.size() - 2];
            auto& queryH = queries[queries.size() - 1];
            queryW.assign(N * 2, 0);
            queryH.assign(N * 2, 0);
            int boxCountW = 0;
            int boxCountH = 0;

            {
                int p = lastLp;
                while (p >= 0) {
                    bool r = rs[p];
                    int qi = 2 * p + (r ? 1 : 0);  
                    if (ds[p] != 'L') {
                        queryW[qi] = 1;
                        ++boxCountW;
                        break;
                    }
                    queryW[qi] = 1;
                    ++boxCountW;
                    p = cols[p];
                }
            }
            {
                int p = lastUp;
                while (p >= 0) {
                    bool r = rs[p];
                    int qi = 2 * p + (r ? 0 : 1);  
                    if (ds[p] != 'U') {
                        queryH[qi] = 1;
                        ++boxCountH;
                        break;
                    }
                    queryH[qi] = 1;
                    ++boxCountH;
                    p = cols[p];
                }
            }
        }
    }

    void TestMakePlanWithMat(int allEstimateCount, int useEstimateCount, const Eigen::MatrixXd& X,
                             dvec<bool>& usedEstimate, Queries& queries, TArr<IOResult>& results) const {
        queries.resize(allEstimateCount * 2);
        results.resize(allEstimateCount);
        dvec<double> evals(allEstimateCount, 0);
        dvec<int> idxs;

        REP(ei, allEstimateCount) {
            if (usedEstimate[ei]) {
                continue;
            }
            idxs.pushR() = ei;
            NArr<char> ds(N);

            IOResult& result = results[ei];
            result.Clear();
            Sim sim;
            sim.Init();

            NArr<int> cols(N, -1);
            NArr<bool> rs(N);

            auto GetRotatedMeanSize = [&](int p) {
                Size size;
                size.w = (int)ceil(estimateMeanW_[p]);
                size.h = (int)ceil(estimateMeanH_[p]);
                if (rs[p]) {
                    swap(size.w, size.h);
                }
                return size;
            };

            int firstBox = -1;
            int secondBoxU = -1;
            int secondBoxL = -1;
            int lastUp = -1;
            int lastLp = -1;
            REP(p, N) {
                int measureW = 2 * N + 2 * ei + 0;
                int measureH = 2 * N + 2 * ei + 1;
                int boxW = p * 2 + 0;
                int boxH = p * 2 + 1;

                bool r = false;
                char d = 'N';
                if (X(measureW, boxW) > 0) {
                    r = false;
                    d = 'L';
                }
                else if (X(measureH, boxW) > 0) {
                    r = true;
                    d = 'U';
                }
                else if (X(measureW, boxH) > 0) {
                    r = true;
                    d = 'L';
                }
                else if (X(measureH, boxH) > 0) {
                    r = false;
                    d = 'U';
                }

                ds[p] = d;
                rs[p] = r;

                if (d != 'N') {
                    result.Push(p, r, d, -1);

                    Size size;
                    size.w = int(ceil(estimateMeanW_[p]));
                    size.h = int(ceil(estimateMeanH_[p]));

                    int col = sim.Put(size, p, r, d, -1);
                    cols[p] = col;

                    if (firstBox < 0) {
                        firstBox = p;
                    }
                    else {
                        if (d == 'U') {
                            if (secondBoxU < 0) {
                                secondBoxU = p;
                            }
                        }
                        else {
                            if (secondBoxL < 0) {
                                secondBoxL = p;
                            }
                        }
                    }

                    if (lastUp < 0) {
                        lastUp = p;
                    }
                    if (lastLp < 0) {
                        lastLp = p;
                    }

                    if (d == 'U') {
                        lastUp = p;
                    }
                    else {
                        lastLp = p;
                    }
                }
            }

            auto& queryW = queries[ei * 2];
            auto& queryH = queries[ei * 2 + 1];
            queryW.assign(N * 2, 0);
            queryH.assign(N * 2, 0);
            int boxCountW = 0;
            int boxCountH = 0;

            int enableBoxCount = 0;

            {
                int p = lastLp;
                while (p >= 0) {
                    bool r = rs[p];
                    int qi = 2 * p + (r ? 1 : 0);  
                    if (ds[p] != 'L') {
                        queryW[qi] = 1;
                        ++boxCountW;
                        if (p == firstBox) {
                            ++enableBoxCount;
                        }
                        break;
                    }
                    queryW[qi] = 1;
                    ++boxCountW;
                    ++enableBoxCount;
                    p = cols[p];
                }
            }
            {
                int p = lastUp;
                while (p >= 0) {
                    bool r = rs[p];
                    int qi = 2 * p + (r ? 0 : 1);  
                    if (ds[p] != 'U') {
                        queryH[qi] = 1;
                        ++boxCountH;
                        if (p == firstBox) {
                            ++enableBoxCount;
                        }
                        break;
                    }
                    queryH[qi] = 1;
                    ++boxCountH;
                    ++enableBoxCount;
                    p = cols[p];
                }
            }

            double eval = (boxCountW + boxCountH) - (result.cs_.size() + 1);


            evals[ei] = eval;
        }

        stable_sort(ALL(idxs), [&](int a, int b) { return evals[a] > evals[b]; });
        idxs.resizeR(useEstimateCount);

        static Queries tmpQueires;
        static TArr<IOResult> tmpResults;
        tmpQueires = queries;
        tmpResults = results;
        queries.clear();
        results.clear();

        for (int idx : idxs) {
            queries.push(tmpQueires[idx * 2]);
            queries.push(tmpQueires[idx * 2 + 1]);
            results.push(tmpResults[idx]);
        }
        for (int idx : idxs) {
            usedEstimate[idx] = true;
        }
    }

    double EvalPlan(const Queries& newQueries) const {
        Eigen::MatrixXd X(queries_.size() + newQueries.size(), 2 * N);

        REP(i, queries_.size()) {
            cauto& q = queries_[i];
            REP(j, 2 * N) {
                X(i, j) = q[j];
            }
        }
        REP(i, newQueries.size()) {
            cauto& q = newQueries[i];
            REP(j, 2 * N) {
                X(i + queries_.size(), j) = q[j];
            }
        }
        Eigen::MatrixXd XtX = X.transpose() * X;

        return compute_log_determinant(XtX);
    }

    void DoPlan(const Queries& newQueries, const TArr<IOResult>& results) {
        REP(i, results.size()) {
            int boxCountW = 0;
            int boxCountH = 0;
            cauto& queryW = newQueries[i * 2];
            cauto& queryH = newQueries[i * 2 + 1];
            REP(p, N) {
                if (queryW[p * 2] || queryW[p * 2 + 1]) {
                    ++boxCountW;
                }
                if (queryH[p * 2] || queryH[p * 2 + 1]) {
                    ++boxCountH;
                }
            }

            cauto& result = results[i];
            Size response;
            server.Output(result, response);

            responses_.push(ClipMultiBoxSize(boxCountW, response.w));
            responses_.push(ClipMultiBoxSize(boxCountH, response.h));
        }
        queries_.append(newQueries);
    }

    void UpdateEstimate(bool updateStd) {
        Eigen::MatrixXd A = Eigen::MatrixXd::Zero(queries_.size(), 2 * N);
        Eigen::VectorXd w(queries_.size());

        REP(i, queries_.size()) {
            cauto& q = queries_[i];
            REP(j, 2 * N) {
                A(i, j) = q[j];
            }
            w(i) = responses_[i];
        }

        auto AT = A.transpose();
        Eigen::MatrixXd ATA = AT * A;

        Eigen::VectorXd x_estimated = ATA.ldlt().solve(AT * w);  
        REP(p, N) {
            estimateMeanW_[p] = x_estimated(p * 2);
            estimateMeanH_[p] = x_estimated(p * 2 + 1);
        }

        if (updateStd) {
            Eigen::MatrixXd ATA_inv = ATA.inverse();  
            Eigen::VectorXd standard_deviations = SIGMA * ATA_inv.diagonal().array().sqrt();






            REP(p, N) {
                estimateStdW_[p] = standard_deviations(p * 2);
                estimateStdH_[p] = standard_deviations(p * 2 + 1);
            }
        }
    }

    void MakeDistoribute() {

        auto FixStd = [&](double s) { return s * HP.StdScale; };

        REP(p, N) {
            distributW_[p] =
                normal_distribution<double>(ClipSize(estimateMeanW_[p] + HP.MeanShift), FixStd(estimateStdW_[p]));
            distributH_[p] =
                normal_distribution<double>(ClipSize(estimateMeanH_[p] + HP.MeanShift), FixStd(estimateStdH_[p]));
        }
    }

    void MakeSize(int p, Size& size) {
        size.w = int(round(distributW_[p](rand_)));
        size.h = int(round(distributH_[p](rand_)));
        size.w = ClipSize(size.w);
        size.h = ClipSize(size.h);

    }

    void MakeSizes(NArr<Size>& sizes) {
        sizes.resize(N);
        REP(p, N) {
            MakeSize(p, sizes[p]);
        }
    }

};

#define USE_FIRST_HASH 0

struct State {
    Sim sim_;
    Candidater candidater_;
    IOResult result_;
    void Init() {
        sim_.Init();
        candidater_.Init();
        result_.Clear();
    }

    void Put(const Size& size, int p, bool r, char d, int b) {
        sim_.Put(size, p, r, d, b);
        candidater_.Add(sim_, p, d);
        result_.Push({(s8)p, r, d, (s8)b});
        UpdateOther();
    }
    void Put(const Size& size, const IOCommand& c) {
        sim_.Put(size, c.p, c.r, c.d, c.b);
        candidater_.Add(sim_, c.p, c.d);
        result_.Push(c);
        UpdateOther();
    }
    void UpdateOther() {
    }
};

bool Greedy(const NArr<Size>& sizes, State& state, const NArr<bool>& isRandom, double minScore) {
    FOR(p, state.result_.cs_.size(), N) {
        if (-state.sim_.GetScore() < minScore) {
            return false;
        }

        cauto& size = sizes[p];
        IOCommand bestCommand;
        double bestScore = 1e100;
        if (isRandom[p]) {
            REP(ti, 100) {
                bool r = rand_(2) != 0;
                char d = rand_(2) == 0 ? 'U' : 'L';
                int b = rand_(p + 1) - 1;
                bestCommand = IOCommand{(s8)p, r, d, (s8)b};

                if (!state.candidater_.IsCandidate(d, b)) {
                    continue;
                }

                break;
            }
        }
        else {
            for (bool r : {false, true}) {
                if (p == 0 && r) {
                    continue;
                }

                int rw = size.w;
                int rh = size.h;
                if (r) {
                    swap(rw, rh);
                }

                for (char d : {'U', 'L'}) {
                    if (p == 0 && d == 'L') {
                        continue;
                    }

                    if (p != 0) {
                        if (rw < rh) {
                            if (d == 'U') {
                                continue;
                            }
                        }
                        else {
                            if (d == 'L') {
                                continue;
                            }
                        }
                    }
                    for (int b : state.candidater_.GetCandidates(d)) {
                        if (b >= 0) {
                            if (d == 'U' && state.sim_.pos_[b].y1 == 0) {
                                continue;
                            }
                            else if (d == 'L' && state.sim_.pos_[b].x1 == 0) {
                                continue;
                            }
                        }
                        int W;
                        int H;
                        int putL;
                        int putT;
                        int putR;
                        int putB;
                        state.sim_.TryPut(size, p, r, d, b, W, H, putL, putT, putR, putB);

                        char d2 = d;
                        if (b >= 0) {
                            if (d == 'U' && putT == 0) {
                                d2 = 'L';
                                b = -1;
                            }
                            else if (d == 'L' && putL == 0) {
                                d2 = 'U';
                                b = -1;
                            }
                        }

                        double score = 0;



                        if (N - p > 1) {
                            score -= gridScore_.CalcScore(putL, putT, putR, putB) * HP.GridScoreRate;  
                        }
                        double rate = ((p + 1) / (double)N);
                        score *= HP.GridScoreStartRate + (HP.GridScoreEndRate - HP.GridScoreStartRate) * rate;


                        score += W;
                        score += H;





                        score += rand_.GetDouble() * HP.RandomScoreRate;

                        if (score < bestScore) {
                            bestScore = score;
                            bestCommand = {(s8)p, r, d2, (s8)b};
                        }
                    }
                }
            }
        }

        state.Put(size, bestCommand);
    }
    return true;
}

void EvalOutput(Estimater& estimater_, const DoubleHashMeanFilter<IOResult>& topResults_)
{
    int sampleCount = topResults_.nodePool_.size();
    dvec<IOResult> samples(sampleCount);
    dvec<double> samplesScoreSum(sampleCount);
    dvec<double> samplesScoreBest(sampleCount);
    dvec<int> samplesScoreCount(sampleCount);
    REP(i, sampleCount) {
        cauto& node = topResults_.nodePool_[i];
        samples[i] = node.data_;
        samplesScoreSum[i] = -node.evalSum_;  
        samplesScoreBest[i] = -node.evalSum_ / (double)node.count_;
        samplesScoreCount[i] = node.count_;
        VASSERT(samplesScoreSum[i] >= 0);
    }

    int evalCount = 0;
    while (!timer_.IsOver()) {
        NArr<Size> sizes;
        estimater_.MakeSizes(sizes);

        REP(si, samples.size()) {
            if (timer_.IsOver()) {
                break;
            }
            cauto& cs = samples[si].cs_;

            Sim sim;
            sim.Init();

            REP(p, N) {
                cauto& c = cs[p];
                sim.Put(sizes[p], p, c.r, c.d, c.b);
            }
            int score = sim.GetScore();
            samplesScoreSum[si] += score;
            chmin(samplesScoreBest[si], score);
            ++samplesScoreCount[si];
        }
        ++evalCount;
    }

    dvec<double> samplesEval(samples.size());
    REP(i, samples.size()) {
        VASSERT(samplesScoreCount[i] > 0);

        double mean = samplesScoreSum[i] / (double)samplesScoreCount[i];
        samplesEval[i] = mean;
    }

    dvec<int> idxs(samples.size());
    iota(ALL(idxs), 0);

    int turnLeft = T - server.turn_;
    int bestCount = min(idxs.size(), (int)round(turnLeft * HP.OutputBestRate));
    if (idxs.size() > bestCount) {
        nth_element(idxs.begin(), idxs.begin() + bestCount, idxs.end(),
                    [&](int a, int b) { return samplesScoreBest[a] < samplesScoreBest[b]; });
    }
    REP(i, bestCount) {
        cauto& result = samples[idxs[i]];
        Size response;
        server.Output(result, response);
    }
    idxs.resizeArea(0, bestCount, 0);

    turnLeft = T - server.turn_;
    int meanCount = min(idxs.size(), turnLeft);
    if (idxs.size() > turnLeft) {
        nth_element(idxs.begin(), idxs.begin() + turnLeft, idxs.end(),
                    [&](int a, int b) { return samplesEval[a] < samplesEval[b]; });
    }
    REP(i, meanCount) {
        cauto& result = samples[idxs[i]];
        Size response;
        server.Output(result, response);
    }

    turnLeft = T - server.turn_;
    REP(i, turnLeft) {
        IOResult result;
        Size response;
        server.Output(result, response);
    }
}


template <class ACTION>
struct ActionTree {
    dvec<ACTION> actions_;
    dvec<int> parents_;

    void Init(int reserveSize) {
        actions_.clear();
        parents_.clear();
        actions_.reserve(reserveSize);
        parents_.reserve(reserveSize);
    }

    void Clear() {
        actions_.clear();
        parents_.clear();
    }

    int Push(int parent, const ACTION& action) {
        int newIdx = actions_.size();
        actions_.pushR() = action;
        parents_.pushR() = parent;
        return newIdx;
    }

    void GetActionList(int actionIdx, dvec<ACTION>& dst) const {
        dst.clear();

        int ai = actionIdx;
        while (ai >= 0) {
            dst.pushR() = actions_[ai];
            ai = parents_[ai];
        }
        reverse(ALL(dst));
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
};


#define USE_BEAM_FILTER 1

template <class State, class Action>
struct BeamSearch {
    struct BeamState {
        int ai_;
        State state_;
    };

    struct StateAction {
        int si_;
        Action action_;
    };

    ActionTree<Action> actionTree_;
    dvec<BeamState> curStates_;
    dvec<BeamState> nextStates_;
    BeamFilter<StateAction> filter_;
    dvec<typename BeamFilter<StateAction>::Node> stateActions_;
    dvec<typename BeamFilter<StateAction>::Node> nextStateActions_;
    int turn_;  


    void Init(int actionCap, int stateCap) {
        actionTree_.Init(actionCap);
        curStates_.clear();
        curStates_.reserve(stateCap);
        nextStates_.clear();
        nextStates_.reserve(stateCap);
        filter_.Reserve(stateCap);
        stateActions_.clear();
        stateActions_.reserve(stateCap);
        nextStateActions_.clear();
        nextStateActions_.reserve(stateCap);
        turn_ = 0;

    }

    void Clear() {
        actionTree_.Clear();
        curStates_.clear();
        nextStates_.clear();
        stateActions_.clear();
        nextStateActions_.clear();
        turn_ = 0;
    }

    State& PushRoot() {
        int si = nextStates_.size();
        auto& state = nextStates_.pushR();
        state.ai_ = -1;

        auto& node = stateActions_.pushR();
        node.evalScore_ = 0;
        node.data_.si_ = si;
        node.data_.action_ = {};

        return state.state_;
    }

    bool Run(int beamWidth, int maxTurn, auto&& makeNext, auto&& apply) {
        turn_ = 0;

        int totalWidth = 0;
        bool ok = true;

        while (turn_ < maxTurn) {
            int turnWidth = beamWidth;

            auto Expand = [&](int si, BeamState& state) {
                makeNext(turn_, state.state_, [&](double evalScore, u64 hash, const Action& action) {
                    if (turnWidth >= 0) {
                        auto* data = filter_.TryPush(evalScore, hash);
                        if (data) {
                            data->si_ = si;
                            data->action_ = action;
                        }
                    }
                    else {
                        auto& node = nextStateActions_.pushR();
                        node.evalScore_ = evalScore;
                        node.data_.si_ = si;
                        node.data_.action_ = action;
                    }
                });
            };

            if (turn_ > 0) {
                nextStates_.clear();  
            }

            filter_.Init(turnWidth);

            if (stateActions_.empty()) {
                ok = false;
                break;
            }

            int expandWidth = 0;

            REP(sai, stateActions_.size()) {
                cauto& node = stateActions_[sai];

                if (turn_ == 0) {
                    int si = node.data_.si_;
                    auto& state = nextStates_[si];
                    Expand(si, state);
                }
                else {
                    int si = nextStates_.size();
                    auto& state = nextStates_.pushR();
                    if constexpr (is_trivially_copyable_v<State>) {
                        memcpy(&state.state_, &curStates_[node.data_.si_].state_, sizeof(state.state_));
                    }
                    else {
                        state.state_ = curStates_[node.data_.si_].state_;
                    }
                    state.ai_ = actionTree_.Push(curStates_[node.data_.si_].ai_, node.data_.action_);
                    apply(state.state_, node.data_.action_);

                    Expand(si, state);
                }
                ++expandWidth;
            }
            ++turn_;

            swap(curStates_, nextStates_);
            stateActions_ = filter_.nodePool_;


            totalWidth += expandWidth;

        }

        return ok;
    }


    void GetActions(int ai, dvec<Action>& actions) const {
        actionTree_.GetActionList(ai, actions);
    }

    void MakeFinalState(auto&& apply, dvec<BeamState>& states) {
        states.clear();
        REP(sai, stateActions_.size()) {
            cauto& node = stateActions_[sai];

            VASSERT(turn_ > 0);

            auto& state = states.pushR();
            if constexpr (is_trivially_copyable_v<State>) {
                memcpy(&state.state_, &curStates_[node.data_.si_].state_, sizeof(state.state_));
            }
            else {
                state.state_ = curStates_[node.data_.si_].state_;
            }
            state.ai_ = actionTree_.Push(curStates_[node.data_.si_].ai_, node.data_.action_);
            apply(state.state_, node.data_.action_);
        }
    }

};

struct GridHash {
    array<dvec<u64>, N_max> cellsHashW_;
    array<dvec<u64>, N_max> cellsHashH_;
    int cellSize_;
    int cellCount_;

    void Init(Xor64& ran) {
        cellSize_ = HP.GridHashCellSize;
        int GridSize = 1500000;
        cellCount_ = (GridSize + cellSize_ - 1) / cellSize_;

        REP(p, N) {
            cellsHashW_[p].resizeR(cellCount_);
            cellsHashH_[p].resizeR(cellCount_);
            REP(i, cellCount_) {
                cellsHashW_[p][i] = ran.Get64();
                cellsHashH_[p][i] = ran.Get64();
            }
        }
    }

    u64 GetHash(int p, int x, int y) const {
        int i = x / cellSize_;
        int j = y / cellSize_;
        if (i >= cellCount_ || j >= cellCount_) {
            return 0;
        }
        chmin(i, cellCount_ - 1);
        chmin(j, cellCount_ - 1);
        return cellsHashW_[p][j] & cellsHashH_[p][i];
    }
};
GridHash gridHashTbl_;

struct CellHash {
    array<dvec<u64>, N_max> cellsHashW_;
    array<dvec<u64>, N_max> cellsHashH_;
    int cellSize_;
    int cellCount_;

    void Init(Xor64& ran) {

        double a = 0.174613419;
        double b = 492223.861;
        double predictScore = a * server.minSizeSum_ + b;
        double predictW = predictScore / 2.0;

        cellCount_ = 1;
        cellSize_ = int(predictW * HP.CellHashCellSizeRate);

        REP(p, N) {
            cellsHashW_[p].resizeR(cellCount_);
            cellsHashH_[p].resizeR(cellCount_);
            REP(i, cellCount_) {
                cellsHashW_[p][i] = ran.Get64();
                cellsHashH_[p][i] = ran.Get64();
            }
        }
    }

    u64 GetHash(int p, int x, int y) const {
        int i = x / cellSize_;
        int j = y / cellSize_;
        chmin(i, cellCount_ - 1);
        chmin(j, cellCount_ - 1);
        return cellsHashW_[p][j] & cellsHashH_[p][i];
    }

    bool GetSafeHash(int p, int x, int y, u64& h) const {
        int i = x / cellSize_;
        int j = y / cellSize_;

        if (i >= cellCount_ || j >= cellCount_) {
            return false;
        }
        chmin(i, cellCount_ - 1);
        chmin(j, cellCount_ - 1);
        h = cellsHashW_[p][j] & cellsHashH_[p][i];
        return true;
    }
};
CellHash cellHashTbl_;

struct BeamState {
    Sim sim_;
    Candidater candidater_;
    u64 gridHash_;
    double totalScore_;

    void Init(const State& srcState) {
        sim_ = srcState.sim_;
        candidater_ = srcState.candidater_;
        totalScore_ = 0;
        gridHash_ = 0;
    }
    void Move(const Size& size, const IOCommand& c) {
        sim_.Put(size, c.p, c.r, c.d, c.b);
        candidater_.Add(sim_, c.p, c.d);
        gridHash_ ^= gridHashTbl_.GetHash(c.p, sim_.pos_[c.p].x2, sim_.pos_[c.p].y2);
        cauto& pos = sim_.pos_[c.p];
        totalScore_ += gridScore_.CalcScore(pos.x1, pos.y1, pos.x2, pos.y2);
    }
    u64 GetNextGridHash(int p, bool r, int putL, int putT, int putR, int putB) const {
        u64 h = gridHash_;
        h ^= gridHashTbl_.GetHash(p, putR, putB);
        return h;
    }
};
constexpr int BeamStateSize = sizeof(BeamState);

using BeamAction = IOCommand;
using BS = BeamSearch<BeamState, BeamAction>;

struct Solver {
    Estimater estimater_;
    DoubleHashMeanFilter<IOResult> topResults_;
    BS bs_;

    void Run() {
        zobrist_.Init(rand_);
        cellHashTbl_.Init(rand_);

        gridScore_.Init();
        gridHashTbl_.Init(rand_);

        DEF_DOUBLE_PARAM_RANGE(EstimateCountRate, 0, 1e10);
        int estimateCount = int(round(T * EstimateCountRate));

        topResults_.Init(HP.TopCount);

        constexpr int ActinCap = 8000;
        constexpr int StateCap = 32;  
        bs_.Init(ActinCap, StateCap);

        estimater_.Estimate(estimateCount);


        timer_.StartMs(int(round(TIME_LIMIT * HP.FindTimeRate)));
        Calc();

        timer_.StartMs(TIME_LIMIT);
        EvalOutput(estimater_, topResults_);


    }


    void Calc() {
        int makeCount = 0;
        static DoubleHashMeanFilter<IOResult> baseResults;
        baseResults.Init(HP.BestResultCount);

        auto PushSim = [&](const Sim& sim, const IOResult& result) {
            double eval = -sim.GetScore();

            static NArr<Size> validateSizes;
            estimater_.MakeSizes(validateSizes);

            {
                static Sim sim;
                sim.Init();
                REP(p, N) {
                    cauto& c = result.cs_[p];
                    sim.Put(validateSizes[p], c.p, c.r, c.d, c.b);
                }
                eval = (eval * (1.0 - HP.ValidationScoreRate) - sim.GetScore() * HP.ValidationScoreRate);

            }

            u64 actionHash = 0;
            u64 cellHash = 0;
            REP(p, N) {
                cauto& c = result.cs_[p];
                cauto& pos = sim.pos_[p];

                actionHash ^= zobrist_.GetHashR(p, c.r);
                actionHash ^= zobrist_.GetHashD(p, c.d);
                actionHash ^= zobrist_.GetHashB(p, c.b);

                u64 h;
                if (cellHashTbl_.GetSafeHash(p, pos.x1, pos.y1, h)) {
                    cellHash ^= h;
                }
                cellHash ^= zobrist_.GetHashR(p, c.r);
            }

            if (IOResult* ptr = topResults_.TryPush(eval, actionHash, cellHash)) {
                ptr->cs_.MemCopy(result.cs_);
            }
            if (IOResult* ptr = baseResults.TryPush(eval, actionHash, cellHash)) {
                ptr->cs_.MemCopy(result.cs_);
            }
        };
        auto Push = [&](const State& state) {
            cauto& sim = state.sim_;
            cauto& result = state.result_;
            PushSim(sim, result);
        };

        static NArr<Size> sizes;
        static State state;
        static dvec<Sim> dstSims;
        static dvec<IOResult> dstResults;

        auto GetLimitScore = [&]() {
            return baseResults.GetSize() == HP.BestResultCount ? baseResults.GetMinEvalScore() : -1e100;
        };

        REP(makeI, HP.StartGreedyCount) {
            estimater_.MakeSizes(sizes);

            state.Init();

            NArr<bool> isRandom(N, false);
            if (Greedy(sizes, state, isRandom, GetLimitScore())) {
                Push(state);
            }
            ++makeCount;
        }

        {
            state.Init();
            REP(makeI, HP.StartBeamCount) {
                estimater_.MakeSizes(sizes);

                Beam(HP.FirstBeamWidth, sizes, state, GetLimitScore(), dstSims, dstResults);
                REP(i, dstSims.size()) {
                    PushSim(dstSims[i], dstResults[i]);
                }
                ++makeCount;
            }
        }

        static dvec<IOResult> results;


        static dvec<int> skipCountTable;
        if (skipCountTable.empty()) {
            REP(p, N) {
                REP(i, int(round(p * HP.SkipCountRate)) + 1) {
                    skipCountTable.pushR() = p;
                }
            }
        }

        while (!timer_.IsOver()) {
            results.clear();
            results.reserve(baseResults.nodePool_.size());
            for (cauto& br : baseResults.nodePool_) {
                results.pushR().cs_.MemCopy(br.data_.cs_);
            }

            estimater_.MakeSizes(sizes);

            REP(tryIndex, results.size()) {
                if (timer_.IsOver()) {
                    break;
                }

                cauto& baseResult = results[tryIndex];

                state.Init();

                {
                    int skipCount = skipCountTable[rand_(skipCountTable.size())];
                    REP(p, skipCount) {
                        cauto& c = baseResult.cs_[p];
                        state.Put(sizes[p], c);
                    }

                    double minScore = GetLimitScore();
                    if (rand_.GetProb(HP.BeamProb)) {
                        Beam(HP.SecondBeamWidth, sizes, state, minScore, dstSims, dstResults);
                        REP(i, dstSims.size()) {
                            PushSim(dstSims[i], dstResults[i]);
                        }
                    }
                    else {
                        NArr<bool> isRandom(N, false);
                        {
                            int c = max(skipCount, 1);
                            isRandom[c] = true;
                        }

                        if (Greedy(sizes, state, isRandom, minScore)) {
                            Push(state);
                        }
                    }
                    ++makeCount;
                }
            }
        }
    }

    void Beam(int beamWidth, const NArr<Size>& sizes, const State& srcState, double minScore, dvec<Sim>& dstSims,
              dvec<IOResult>& dstResults) {
        bs_.Clear();

        {
            auto& state = bs_.PushRoot();
            state.Init(srcState);
        }

        auto MakeNext = [&](int turn, BeamState& state, auto&& callback) {
            if (-state.sim_.GetScore() < minScore) {
                return;
            }

            auto& sim = state.sim_;
            cauto& candidater = state.candidater_;

            int p = sim.t_;
            cauto& size = sizes[p];

            for (bool r : {false, true}) {
                if (p == 0 && r) {
                    continue;
                }

                int rw = size.w;
                int rh = size.h;
                if (r) {
                    swap(rw, rh);
                }

                for (char d : {'U', 'L'}) {
                    if (p == 0 && d == 'L') {
                        continue;
                    }

                    if (p != 0) {
                        if (rw < rh) {
                            if (d == 'U') {
                                continue;
                            }
                        }
                        else {
                            if (d == 'L') {
                                continue;
                            }
                        }
                    }
                    for (int b : candidater.GetCandidates(d)) {
                        if (b >= 0) {
                            if (d == 'U' && sim.pos_[b].y1 == 0) {
                                continue;
                            }
                            else if (d == 'L' && sim.pos_[b].x1 == 0) {
                                continue;
                            }
                        }
                        int W;
                        int H;
                        int putL;
                        int putT;
                        int putR;
                        int putB;
                        sim.TryPut(size, p, r, d, b, W, H, putL, putT, putR, putB);

                        char d2 = d;
                        if (b >= 0) {
                            if (d == 'U' && putT == 0) {
                                d2 = 'L';
                                b = -1;
                            }
                            else if (d == 'L' && putL == 0) {
                                d2 = 'U';
                                b = -1;
                            }
                        }

                        double score = 0;




                        if (N - p > 1) {
                            score -= state.totalScore_;
                            score -= gridScore_.CalcScore(putL, putT, putR, putB);
                            score *= HP.GridScoreRate;

                            double rate = ((p + 1) / (double)N);
                            score *= HP.GridScoreStartRate + (HP.GridScoreEndRate - HP.GridScoreStartRate) * rate;
                        }


                        score += W;
                        score += H;






                        u64 nextHash = state.GetNextGridHash(p, r, putL, putT, putR, putB);
                        double nextEval = -score;

                        BeamAction action;
                        action.p = p;
                        action.r = r;
                        action.d = d;
                        action.b = b;
                        callback(nextEval, nextHash, action);
                    }
                }
            }
        };

        auto Apply = [&](BeamState& state, const BeamAction& action) { state.Move(sizes[action.p], action); };

        bs_.Run(beamWidth, N - srcState.sim_.t_, MakeNext, Apply);

        static dvec<BS::BeamState> states;
        bs_.MakeFinalState(Apply, states);

        static dvec<BeamAction> actions;

        dstSims.clear();
        dstResults.clear();
        for (cauto& state : states) {
            bs_.GetActions(state.ai_, actions);

            dstSims.pushR() = state.state_.sim_;

            auto& result = dstResults.pushR();
            result.cs_.MemCopy(srcState.result_.cs_);
            result.cs_.insert(result.cs_.size(), actions.data(), actions.size());
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
