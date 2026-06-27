#define FULL_LIB 1

#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0


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

#define FIX_RESULT 0


#define TIME_LIMIT_US (TIME_LIMIT * 1000)

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


#define ALL(x) (x).begin(),(x).end()

template <class T, class U> inline void chmin(T& a, const U& b) { if (b < a) { a = b; } }
template <class T, class U> inline void chmax(T& a, const U& b) { if (a < b) { a = b; } }
template <class T, class U, class V> inline T clip(const T& v, U&& lower, V&& upper) {
	if (v < lower) { return lower; }
	else if (v > upper) { return upper; }
	else { return v; }
}
template <class T, class U, class V> inline void chclip(T& v, const U& lower, const V& upper) {
	if (v < lower) { v = lower; }
	else if (v > upper) { v = upper; }
}
template <class T> inline constexpr T square(T v) { return v * v; }

template <class T, int SIZE>
constexpr int len(const T(&)[SIZE]) { return SIZE; }

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

	bool operator == (pr const& r) const { return a == r.a && b == r.b; }
	bool operator != (pr const& r) const { return !((*this) == r); }
	bool operator < (pr const& r) const {
		if (a == r.a) {
			return b < r.b;
		}
		return a < r.a;
	}
	bool operator > (pr const& r) const {
		return r < (*this);
	}


	pr& operator += (pr const& v) {
		a += v.a;
		b += v.b;
		return *this;
	}
	pr& operator -= (pr const& v) {
		a -= v.a;
		b -= v.b;
		return *this;
	}

	template <class C, class D>
	auto operator + (pr<C, D> const& v) const {
		return pr<decltype(a + v.a), decltype(b + v.b)>{ a + v.a, b + v.b };
	}

	template <class C, class D>
	auto operator - (pr<C, D> const& v) const {
		return pr<decltype(a - v.a), decltype(b - v.b)>{ a - v.a, b - v.b };
	}

	template <class C, class D>
	explicit operator pr<C, D>() const {
		return { C(a), D(b) };
	}

	template <class T>
	auto operator * (T const& v) const -> pr<decltype(x * v), decltype(y * v)> {
		return { x * v, y * v };
	}
	template <class T>
	auto operator / (T const& v) const -> pr<decltype(x / v), decltype(y / v)> {
		return { x / v, y / v };
	}

	template <class T>
	pr& operator *= (T const& v) {
		x *= v;
		y *= v;
		return *this;
	}
	template <class T>
	pr& operator /= (T const& v) {
		x /= v;
		y /= v;
		return *this;
	}

	pr operator -() const {
		return pr{ -x, -y };
	}

	void flip() { swap(x, y); }

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
struct tuple_element<0, pr<A, B>> { using type = A; };
template <class A, class B>
struct tuple_element<1, pr<A, B>> { using type = B; };

inline pdouble ToDouble(const pint& p) {
	return pdouble{ double(p.x), double(p.y) };
}
inline pint round(const pdouble& d) {
	return pint{ (int)round(d.x), (int)round(d.y) };
}
inline double norm(const pdouble& d) {
	return sqrt((d.x * d.x) + (d.y * d.y));
}
inline double norm(const pint& d) {
	return norm(ToDouble(d));
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

	bool operator == (const CapArr<T, CAP>& r) const {
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
	bool operator != (const CapArr<U, U_CAP>& r) const {
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
				memset(data(), underlying_type<T>::type(e), size);
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
		idxs.sort([&](int a, int b) {
			return array_[a] < array_[b];
			});
	}
	void idx_rsort(CapArr<int, CAP>& idxs) {
		idxs.Iota(size());
		idxs.sort([&](int a, int b) {
			return array_[b] < array_[a];
			});
	}
	void idx_stable_sort(CapArr<int, CAP>& idxs) {
		idxs.Iota(size());
		idxs.stable_sort([&](int a, int b) {
			return array_[a] < array_[b];
			});
	}
	void idx_stable_rsort(CapArr<int, CAP>& idxs) {
		idxs.Iota(size());
		idxs.stable_sort([&](int a, int b) {
			return array_[b] < array_[a];
			});
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

    dvec() : ptr_(nullptr), size_(0), cap_(0) {}

    explicit dvec(int size) : ptr_(mem_alloc<T>(size)), size_(size), cap_(size) {
        for (int i = 0; i < size; ++i) {
            new(&ptr_[i]) T();
        }
    }

    explicit dvec(const std::vector<T>& vec) : ptr_(mem_alloc<T>((int)vec.size())), size_((int)vec.size()), cap_((int)vec.size()) {
        for (int i = 0; i < size_; ++i) {
            new(&ptr_[i]) T(vec[i]);
        }
    }

    dvec(int size, const T& v) : ptr_(mem_alloc<T>(size)), size_(size), cap_(size) {
        for (int i = 0; i < size; ++i) {
            new(&ptr_[i]) T(v);
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

    bool operator == (const dvec& r) const {
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
    bool operator != (const dvec& r) const {
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
    inline T* data() { return ptr_; }
    inline const T* data() const { return ptr_; }

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
        reserve(size);
        size_ = size;
    }

    void assign(int size, const T& v) {
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
        VASSERT(idx < size_);
        if (idx + 1 < size_) {
            memmove(ptr_ + idx, ptr_ + idx + 1, sizeof(T) * (size_ - 1 - idx));
        }
        --size_;
    }

    [[nodiscard]] T& insertR(int idx) {
        VASSERT(idx <= size_);

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

    dvec() : ptr_(nullptr), size_(0), consted_(0), cap_(0) {}

    explicit dvec(int size) : ptr_(mem_alloc<T>(size)), size_(size), consted_(size), cap_(size) {
        for (int i = 0; i < size; ++i) {
            new(&ptr_[i]) T();
        }
    }

    explicit dvec(const std::vector<T>& vec) {
        int size = (int)vec.size();
        ptr_ = mem_alloc<T>(size);
        size_ = size;
        consted_ = size;
        cap_ = size;;
        for (int i = 0; i < size; ++i) {
            new(&ptr_[i]) T(vec[i]);
        }
    }

    dvec(int size, const T& v) : ptr_(mem_alloc<T>(size)), size_(size), consted_(size), cap_(size) {
        for (int i = 0; i < size; ++i) {
            new(&ptr_[i]) T(v);
        }
    }

    dvec(const dvec& r) {
        if (r.ptr_) {
            ptr_ = mem_alloc<T>(r.size_);
            size_ = r.size_;
            consted_ = size_;       
            cap_ = r.size_;

            for (int i = 0; i < size_; ++i) {
                new(&ptr_[i]) T(r.ptr_[i]);
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
                    new(&ptr_[i]) T(r.ptr_[i]);
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
                new(&ptr_[i]) T(r.ptr_[i]);
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

    bool operator == (const dvec& r) const {
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
    bool operator != (const dvec& r) const {
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
    inline T* data() { return ptr_; }
    inline const T* data() const { return ptr_; }

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
        if (size > cap_) {
            int newCap = size;
            T* newPtr = mem_alloc<T>(newCap);

            if (ptr_) {
                for (int i = 0; i < consted_; ++i) {
                    new(&newPtr[i]) T(std::move(ptr_[i]));
                }
                mem_free(ptr_);
            }

            ptr_ = newPtr;
            cap_ = newCap;
        }
    }

    void resizeR(int size) {
        if (size <= cap_) {
            if (size > consted_) {
                for (int i = consted_; i < size; ++i) {
                    new(&ptr_[i]) T();
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
                    new(&newPtr[i]) T(std::move(ptr_[i]));
                }
                mem_free(ptr_);
            }

            ptr_ = newPtr;
            cap_ = newCap;

            VASSERT(size > consted_);
            for (int i = consted_; i < size; ++i) {
                new(&ptr_[i]) T();
            }
            consted_ = size;
            size_ = size;
        }
    }

    void assign(int size, const T& v) {
        if (size <= cap_) {
            for (int i = 0; i < size; ++i) {
                if (i < consted_) {
                    ptr_[i] = v;
                }
                else {
                    new(&ptr_[i]) T(v);
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
                new(&ptr_[i]) T(v);
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
        new(&ptr_[size_]) T();
        ++size_;
        VASSERT(consted_ < size_);
        consted_ = size_;

        return ptr_[size_ - 1];
    }

    void pop() {
        --size_;
    }

    void remove(int idx) {
        VASSERT(idx < size_);
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
        VASSERT(idx <= size_);

        if (size_ == cap_) {
            int newCap = std::max(DefaultMallocSize, cap_ * 2);
            T* newPtr = mem_alloc<T>(newCap);

            if (ptr_) {
                for (int i = 0; i < idx; ++i) {
                    new(&newPtr[i]) T(std::move(ptr_[i]));
                }
                if (idx < consted_) {
                    for (int i = idx; i < consted_; ++i) {
                        new(&newPtr[i + 1]) T(std::move(ptr_[i]));
                    }
                }
                ++consted_;

                new(&newPtr[idx]) T();

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
                            new(&ptr_[i + 1]) T(std::move(ptr_[i]));
                        }
                        else {
                            ptr_[i + 1] = std::move(ptr_[i]);
                        }
                    }
                    new(&ptr_[idx]) T();
                    ++consted_;
                }
            }
            else {
                if (size_ < consted_) {
                }
                else {
                    new(&ptr_[idx]) T();
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


constexpr int N = 5;
constexpr int NN = N * N;

constexpr s8 FrameCap = 127;

#define DEF_CONTAINERS(name, size) \
	template <class TYPE> using name##Arr = CapArr<TYPE, size>;

DEF_CONTAINERS(N, N);
DEF_CONTAINERS(NN, NN);

#define PARAM_CATEGORY(NAME, VALUE, ...) int NAME = VALUE;
#define PARAM_INT(NAME, VALUE, LOWER_VALUE, UPPER_VALUE) int NAME = VALUE;
#define PARAM_DOUBLE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE) double NAME = VALUE;


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




constexpr
struct {


	PARAM_DOUBLE(GetTurn, 0.17940921150033606, 0.1, 0.2);
	PARAM_DOUBLE(CarryTurn, -0.09148400715176176, -0.1, 0.0);
	PARAM_DOUBLE(TotalTurnA, 0.5576148555677509, 0.5, 0.7);
	PARAM_DOUBLE(TotalTurnB, 0.001479007130499895, 0.0, 0.01);
	START_TUNING;
	PARAM_DOUBLE(InCount, 7.132286691318617, 6.0, 8.0);
	PARAM_DOUBLE(OutCount, 4.925234226807601, 4.0, 6.0);
	PARAM_DOUBLE(TmpPutCount, 2.293697154345731, 1.7, 2.7);
	PARAM_DOUBLE(TmpCellScale, 0.9859426281501161, 0.5, 1.5);
	END_TUNING;

	PARAM_DOUBLE(BigGotLoss, 0.02055269009838609, 0.0, 0.2);
	PARAM_DOUBLE(SmallGotLoss, 0.09421558465997858, 0.0, 0.1);
	PARAM_DOUBLE(BigCarryLoss, 0.4555097752304989, 0.4, 0.6);
	PARAM_DOUBLE(SmallCarryLoss, 0.6057257733423642, 0.5, 0.6);

	PARAM_DOUBLE(BigArmFreeTurnA, 0.8288421762873127, 0.7, 0.9);
	PARAM_DOUBLE(BigArmFreeTurnB, 0.0011563068867952336, 0.0, 0.003);
	PARAM_DOUBLE(SmallArmFreeTurnA, 0.7597046111170974, 0.63, 0.83);
	PARAM_DOUBLE(SmallArmFreeTurnB, 0.0009498814034489072, 0.0, 0.01);



	PARAM_DOUBLE(InStepA, 0.09095505053700187, 0.0, 0.2);
	PARAM_DOUBLE(InStepB, 0.16911657101391836, 0.1, 0.2);
	PARAM_DOUBLE(TreeBadLevelA, 0.2940666055646174, 0.12, 0.32);
	PARAM_DOUBLE(TreeBadLevelB, 0.0030658433714359865, 0.0, 0.005);
	PARAM_DOUBLE(OutStep1, 0.47213328181245656, 0.4, 0.6);
	PARAM_DOUBLE(OutStep2, 0.6740012025505017, 0.5, 0.7);
	PARAM_DOUBLE(OutStep3, 1.14933261905151, 0.8, 1.2);
	PARAM_DOUBLE(OutStep4, 1.7662248964369736, 1.5, 1.9);
	PARAM_DOUBLE(OutStep5, 2.4485375797072386, 2.3, 2.5);

	PARAM_DOUBLE(TmpCell_01, 3.498873865788109, 3.0, 3.6);
	PARAM_DOUBLE(TmpCell_02, 0.979060640881774, 0.5, 0.9);
	PARAM_DOUBLE(TmpCell_03, -0.9824869147881773, -1.4, -1.0);
	PARAM_DOUBLE(TmpCell_11, 3.2780919203816157, 3.1, 3.5);
	PARAM_DOUBLE(TmpCell_12, 1.0348006636691158, 0.8, 1.2);
	PARAM_DOUBLE(TmpCell_13, -0.8478129462695861, -1.0, -0.8);
	PARAM_DOUBLE(TmpCell_21, 4.408370620819495, 4.2, 5.0);
	PARAM_DOUBLE(TmpCell_22, 1.1137113569814692, 1.1, 1.7);
	PARAM_DOUBLE(TmpCell_23, -0.8134289635123152, -1.1, -0.8);



	PARAM_INT(AppendTurn, 3, 2, 4); PARAM_LOWER(0);				
	PARAM_INT(MeasureProcessTurn, 7, 4, 10); PARAM_LOWER(0);		
	PARAM_INT(InitialBeamWidth, 7000, 6000, 14000); PARAM_LOWER(0);	
	PARAM_INT(SkipOverTurn, 5, 3, 7); PARAM_LOWER(0);		
	PARAM_INT(ArmCutTurn, 8, 6, 12); PARAM_LOWER(0);		
	PARAM_INT(EnableBadLevel, 11, 1, 16); PARAM_LOWER(0); PARAM_UPPER(23);
	PARAM_INT(WidthOverTurn, 3, 1, 5); PARAM_LOWER(0);
	PARAM_DOUBLE(WidthProcessRate, 1.5, 1, 2); PARAM_LOWER(0);





} HP;





using pt = pr<s8, s8>;


bool CanOut(s8 box, s8 outs) {
    return box / N * N + outs == box;
}
bool CanOut(s8 box, const array<s8, N>& outs) {
    return box / N * N + outs[box / N] == box;
}

enum class IOAction : s8 {
	L = 0,
	U = 1,
	R = 2,
	D = 3,
	Stay = 4,
	Catch = 5,
	Release = 6,
	Bomb = 7,
};


struct IOResult {
	array<CapArr<IOAction, FrameCap>, N> armsActions_;

	void clear() {
		REP(ai, N) {
			armsActions_[ai].clear();
		}
	}

	void push(const array<IOAction, N>& actions) {
		REP(ai, N) {
			armsActions_[ai].push(actions[ai]);
		}
	}

	void Output(ostream& os) const {
		static array<char, 8> ToChar = {
			'L',
			'U',
			'R',
			'D',
			'.',
			'P',
			'Q',
			'B',
		};
		REP(ai, N) {
			for (auto action : armsActions_[ai]) {
				os << ToChar[(int)action];
			}
			os << endl;
		}
	}

	int GetMaxTurn() const {
		int turn = 0;
		REP(ai, N) {
			chmax(turn, armsActions_[ai].size());
		}
		return turn;
	}
};


template <class T, int CAP>
struct CCA {
private:
    T ar[CAP];      
    int s;

public:
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

    inline constexpr const T& operator ()(int i) const {
        VASSERT(i >= 0 && i < CAP);
        return ar[i];
    }
    inline constexpr const T& operator [](int i) const {
        VASSERT(i >= 0 && i < CAP);
        return ar[i];
    }
    inline constexpr int size() const {
        return s;
    }
};

constexpr array<pt, 4> Around4 = { pt{-1, 0}, pt{0, -1}, pt{1, 0}, pt{0, 1} };
constexpr array<pt, 5> Around5 = { pt{-1, 0}, pt{0, -1}, pt{1, 0}, pt{0, 1}, pt{0, 0} };

constexpr s8 cabs(s8 a) {
	if (a < 0) {
		return -a;
	}
	return a;
}

struct Grid {
	array<pt, NN> cell2pt_;
	using CA = CCA<s8, 5>;
	CA arounds4_[NN];
	CA arounds5_[NN];

	static constexpr int RAND_COUNT = 512;
	CA randArounds5s_[RAND_COUNT][NN];

	array<s8, 4> dirs_[NN];		
	s8 dists_[NN][NN];		

	constexpr Grid() : arounds4_{}, arounds5_{}, randArounds5s_{}, dirs_{}, dists_{} {
		REP(y, N) {
			REP(x, N) {
				int cell = y * N + x;
				cell2pt_[cell] = pt{ (s8)x, (s8)y };

				REP(dir, 4) {
					dirs_[cell][dir] = -1;
				}


				if (x > 0) {
					s8 nc = (y)*N + (x - 1);
					arounds4_[cell].push(nc);
					dirs_[cell][0] = nc;
				}

				if (x + 1 < N) {
					s8 nc = (y)*N + (x + 1);
					arounds4_[cell].push(nc);
					dirs_[cell][2] = nc;
				}

				if (y > 0) {
					s8 nc = (y - 1) * N + (x);
					arounds4_[cell].push(nc);
					dirs_[cell][1] = nc;
				}

				if (y + 1 < N) {
					s8 nc = (y + 1) * N + (x);
					arounds4_[cell].push(nc);
					dirs_[cell][3] = nc;
				}

				arounds5_[cell].push(cell);
				REP(i, arounds4_[cell].size()) {
					arounds5_[cell].push(arounds4_[cell][i]);
				}

				Xor64 r;
				REP(i, RAND_COUNT) {
					randArounds5s_[i][cell] = arounds5_[cell];
					Shuffle(randArounds5s_[i][cell].begin() + 1, randArounds5s_[i][cell].end(), r);
				}
			}
		}
		REP(cellA, NN) {
			cauto& a = cell2pt_[cellA];
			FOR(cellB, cellA + 1, NN) {
				cauto& b = cell2pt_[cellB];
				dists_[cellA][cellB] = dists_[cellB][cellA] = cabs(a.a - b.a) + cabs(a.b - b.b);
			}
		}
	}

	constexpr s8 ToId(s8 x, s8 y) const {
		return y * N + x;
	}
	constexpr s8 ToId(const pt& p) const {
		return p.y * N + p.x;
	}
	constexpr const pt& ToPos(s8 cell) const {
		return cell2pt_[cell];
	}
	constexpr const CA& Around5(s8 cell) const {
		return arounds5_[cell];
	}
	constexpr const array<s8, 4>& Dir(s8 cell) const {
		return dirs_[cell];
	}

	inline s8 CalcL1Dist(s8 a, s8 b) const {
		return dists_[a][b];
	}

	s8 CalcMovedCell(s8 cell, s8 dir) const {
		if (dir == 4) {
			return cell;
		}
		return dirs_[cell][dir];
	}






	inline IOAction CalcDir(s8 from, s8 to) const {
		static array<IOAction, 11> tbl = { IOAction::U, IOAction::Stay, IOAction::Stay, IOAction::Stay, IOAction::L, IOAction::Stay, IOAction::R, IOAction::Stay, IOAction::Stay, IOAction::Stay, IOAction::D };
		return tbl[to - from + 5];
	}
};
constexpr Grid gs;




struct Grid2 {
	array<pt, 25> five2pt_;
	array<pt, 29> six2pt_;
	array<array<s8, N>, N> pt2five_;
	array<array<s8, N>, N> pt2six_;
	array<s8, 25> five2six_;
	array<s8, 29> six2five_;


	constexpr Grid2() : five2pt_{}, six2pt_{}, pt2five_{}, pt2six_{}, five2six_{}, six2five_{} {
		REP(y, N) {
			REP(x, N) {
				s8 five = y * 5 + x;
				s8 six = y * 6 + x;		
				five2pt_[five] = pt{ (s8)x, (s8)y };
				six2pt_[six] = pt{ (s8)x, (s8)y };
				pt2five_[y][x] = five;
				pt2six_[y][x] = six;
				five2six_[five] = six;
				six2five_[six] = five;
			}
		}






	}

	constexpr s8 ToFive(s8 x, s8 y) const {
		return pt2five_[y][x];
	}
	constexpr s8 ToSix(s8 x, s8 y) const {
		return pt2six_[y][x];
	}
	constexpr const pt& FiveToPos(s8 five) const {
		return five2pt_[five];
	}
	constexpr const pt& SixToPos(s8 six) const {
		return six2pt_[six];
	}
	constexpr s8 FiveToSix(s8 five) const {
		return five2six_[five];
	}
	constexpr s8 SixToFive(s8 six) const {
		return six2five_[six];
	}
};
constexpr Grid2 gs2;

struct IOServer {

	array<array<s8, N>, N> ques_;		
	array<s8, NN> boxY_;					

	void InitInput(ChronoTimer& timer) {
		istream& is = cin;
		int a;
		is >> a;
		timer.Init();
		REP(y, N) {
			REP(i, N) {
				is >> a;
				ques_[y][i] = a;
				boxY_[a] = y;
			}
		}
	}

	void Output(const IOResult& result) {
		ostream& os = cout;
		result.Output(os);
	}

	void Finalize() {
	}
};
IOServer server;


Xor64 rand_;


#define USE_INPUT_GUARD 1		



struct TaskTreeLink {
	NArr<pint> nexts_;		
	NArr<pint> prevs_;		

	void Init() {
		nexts_.clear();
		prevs_.clear();
	}
};
struct TaskTreeState {
	NArr<int> fromsNos_;	
	NArr<s8> ys_;			
	NArr<s8> boxs_;			
	s8 badLevel_;			

	array<s8, N> ins_;		
	array<s8, N> outs_;		
	NNArr<s8> tmpBoxs_;		
	s8 inCount_;			
	s8 outCount_;			
	s8 tmpCount_;			
	s8 maxTmpCount_;		

	void Init() {
		fromsNos_.clear();
		ys_.clear();
		boxs_.clear();
		badLevel_ = -1;

		ins_.fill(0);
		outs_.fill(0);
		tmpBoxs_.clear();
		inCount_ = 0;
		outCount_ = 0;
		tmpCount_ = 0;
		maxTmpCount_ = 0;
	}
	void InitBlank() {
		fromsNos_.clear();

		tmpCount_ = NN;		
		maxTmpCount_ = 0;
	}

	bool Pop(s8 y, NNArr<s8>& outTmpBoxs) {
		outTmpBoxs.clear();

		s8 box = server.ques_[y][ins_[y]];
		++ins_[y];
		++inCount_;

		if (IsOutable(box)) {
			++outs_[box / N];
			++outCount_;


			NNArr<s8> nextTmpBoxs;
			for (s8 box : tmpBoxs_) {
				if (IsOutable(box)) {
					++outs_[box / N];
					++outCount_;
					outTmpBoxs.push(box);
				}
				else {
					nextTmpBoxs.push(box);
				}
			}
			tmpBoxs_ = nextTmpBoxs;
			return true;
		}
		else {
			int idx = tmpBoxs_.size();
			REP(i, tmpBoxs_.size()) {
				if (tmpBoxs_[i] / N == box / N && box < tmpBoxs_[i]) {
					idx = i;
					break;
				}
			}
			tmpBoxs_.insert(idx, box);

			++tmpCount_;

			chmax(maxTmpCount_, tmpBoxs_.size());
			return false;
		}
	}

	void Pop(s8 y) {
		NNArr<s8> outTmpBoxs;
		Pop(y, outTmpBoxs);
	}

	bool IsPopOut(s8 y) const {
		s8 box = server.ques_[y][ins_[y]];
		return IsOutable(box);
	}

	void Apply(s8 y) {
		Pop(y);
	}

	bool IsOutable(s8 box) const {
		s8 y = box / N;
		return y * N + outs_[y] == box;
	}

	static int GetStateNo(const array<s8, N>& ins) {
		int no = 0;
		RREP(i, N) {
			no *= 6;
			no += ins[i];
		}
		return no;
	}

	int GetStateNo() const {
		return TaskTreeState::GetStateNo(ins_);
	}

	int EvalScore() const {
		return -tmpCount_;
	}
};


struct TaskTree {
	static constexpr int MaxPattern = 6 * 6 * 6 * 6 * 6;
	array<TaskTreeLink, MaxPattern> links_;
	array<TaskTreeState, MaxPattern> states_;

	void Init() {
		states_[0].Init();
		FOR(i, 1, MaxPattern) {
			states_[i].InitBlank();
		}

		REP(i, MaxPattern) {
			auto& state = states_[i];

			REP(y, N) {
				if (state.ins_[y] < N) {
					auto nextState = state;
					nextState.Pop(y);

					int no = nextState.GetStateNo();
					links_[i].nexts_.push(pint{ y, no });
					links_[no].prevs_.push(pint{ y, i });

					if (nextState.EvalScore() > states_[no].EvalScore()) {
						states_[no] = nextState;
						states_[no].fromsNos_.clear();
						states_[no].fromsNos_.push(i);
					}
					else if (nextState.EvalScore() == states_[no].EvalScore()) {
						states_[no].fromsNos_.push(i);
					}
				}
			}
		}

		RREP(i, MaxPattern) {
			auto& state = states_[i];

			if (i + 1 != MaxPattern && state.ys_.empty()) {
				continue;
			}

			state.badLevel_ = 0;

			for (int fromNo : state.fromsNos_) {
				auto& fromState = states_[fromNo];

				REP(y, N) {
					if (state.ins_[y] != fromState.ins_[y]) {
						fromState.ys_.push(y);
						fromState.boxs_.push(server.ques_[y][fromState.ins_[y]]);
						break;
					}
				}
			}
		}

		REP(level, HP.EnableBadLevel) {
			REP(i, MaxPattern) {
				auto& state = states_[i];
				if (state.badLevel_ >= 0) {
					continue;
				}

				bool hasPrev = false;
				for (auto [y, no] : links_[i].prevs_) {
					if (states_[no].badLevel_ >= 0) {
						hasPrev = true;
					}
				}
				if (!hasPrev) {
					continue;
				}

				bool hasNext = false;
				for (auto [y, no] : links_[i].nexts_) {
					if (states_[no].badLevel_ >= 0) {
						hasNext = true;
					}
				}
				if (!hasNext) {
					continue;
				}

				for (auto [y, no] : links_[i].prevs_) {
					if (states_[no].badLevel_ >= 0) {
						states_[no].ys_.push(y);
						states_[no].boxs_.push(server.ques_[y][states_[no].ins_[y]]);
					}
				}
				for (auto [y, no] : links_[i].nexts_) {
					if (states_[no].badLevel_ >= 0) {
						state.ys_.push(y);
						state.boxs_.push(server.ques_[y][state.ins_[y]]);
					}
				}

				state.badLevel_ = level + 1;
			}
		}
	}

	int GetTmpCount() const {
		return states_.back().tmpCount_;
	}

	const NArr<s8>& GetRecommend(const array<s8, N>& ins) const {
		int no = TaskTreeState::GetStateNo(ins);
		return states_[no].ys_;
	}
	const NArr<s8>& GetRecommendBoxs(const array<s8, N>& ins) const {
		int no = TaskTreeState::GetStateNo(ins);
		return states_[no].boxs_;
	}

	int GetTmpCount(const array<s8, N>& ins) const {
		int no = TaskTreeState::GetStateNo(ins);
		cauto& state = states_[no];
		return state.tmpCount_;
	}
	const TaskTreeState& GetTreeState(const array<s8, N>& ins) const {
		int no = TaskTreeState::GetStateNo(ins);
		cauto& state = states_[no];
		return state;
	}
};
TaskTree taskTree_;


inline int popcount(uint64_t bit) {
#if _MSC_VER
	return (int)__popcnt64(bit);
#else
	return __builtin_popcountll(bit);
#endif
}


int msb(uint64_t v) {
	if (v == 0) return false;
	v |= (v >> 1);
	v |= (v >> 2);
	v |= (v >> 4);
	v |= (v >> 8);
	v |= (v >> 16);
	v |= (v >> 32);
	return popcount(v) - 1;
}

inline uint64_t RotateLeft(uint64_t bit, uint64_t shift) {
	return (bit << shift) | (bit >> (64ULL - shift));
}
inline uint64_t RotateRight(uint64_t bit, uint64_t shift) {
	return (bit >> shift) | (bit << (64ULL - shift));
}


#if _MSC_VER
#include <intrin.h>
#endif

inline int FindBitRL(uint64_t bit) {
	if (bit == 0) {
		return -1;
	}
#if _MSC_VER
	unsigned long index = 0;
	_BitScanForward64(&index, bit);
	return (int)index;
#else
	return __builtin_ctzll(bit);
#endif
}

inline int FindBitLR(uint64_t bit) {
	if (bit == 0) {
		return -1;
	}
#if _MSC_VER
	unsigned long index = 0;
	_BitScanReverse64(&index, bit);
	return 63 - (int)index;			
#else
	return __builtin_clzll(bit);
#endif
}



template <class K>
struct CapHashSetR {
	size_t m_capacity = 0;
	K* m_keys = nullptr;
	uint8_t* m_states = nullptr;		
	size_t m_mask = 0;
	int m_count = 0;


	void init(size_t capacity) {
		if (capacity == m_capacity) {
			clear();
			return;
		}
		m_capacity = 1ULL << (msb(capacity) + 1);
		m_mask = m_capacity - 1;
		m_keys = (K*)malloc(m_capacity * sizeof(K));
		m_states = (uint8_t*)calloc(m_capacity, sizeof(uint8_t));
		m_count = 0;
	}

	void clear() {
		memset(m_states, 0, m_capacity * sizeof(uint8_t));
		m_count = 0;
	}

	int capacity() const {
		return m_capacity;
	}
	int size() const {
		return m_count;
	}

	inline void add(K key) {
		size_t i = key & m_mask;
		REP(loop, m_capacity) {
			if (m_states[i] == 1) {
				if (key == m_keys[i]) {
					return;
				}
			}
			else {
				VASSERT(m_count < m_capacity);

				m_states[i] = 1;
				m_keys[i] = key;
				++m_count;
				return;
			}
			(++i) &= m_mask;
		}
		VABORT();
	}

	inline bool contains(K key) const {
		size_t i = key & m_mask;
		REP(loop, m_capacity) {
			if (m_states[i] == 1) {
				if (key == m_keys[i]) {
					return true;
				}
			}
			else if (m_states[i] == 0) {
				return false;
			}
			(++i) &= m_mask;
		}
		return false;
	}

	inline void remove(K key) {
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

	bool safe_enter(K key) {
		if (m_count == m_capacity) {
			return false;
		}
		size_t i = key & m_mask;
		REP(loop, m_capacity) {
			if (m_states[i] == 1) {
				if (key == m_keys[i]) {
					return false;
				}
			}
			else {
				VASSERT(m_count < m_capacity);

				m_states[i] = 1;
				m_keys[i] = key;
				++m_count;
				return true;
			}
			(++i) &= m_mask;
		}
		VABORT();
		return false;
	}

	int GetCount() const {
		return m_count;
	}

};

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

template <class K, class V>
struct dhmap {
	size_t m_capacity = 0;
	K* m_keys = nullptr;
	V* m_values = nullptr;
	uint8_t* m_states = nullptr;		
	size_t m_mask = 0;

	int m_count = 0;

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

		m_capacity = 1ULL << (msb(capacity - 1) + 1);
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
		VASSERT(m_count < m_capacity);		

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
					m_states[i] = 0;
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

template <class T>
struct Node {
    double evalScore_;
    u64 hash_;
    T data_;
};

struct SegTree {
    struct Data {
        double evalScore;
        int index;
    };
    dvec<Data> buf_;
    int depth_;
    int size_;

    void Init(int size) {
        depth_ = msb(size) + 1;
        size_ = 1 << depth_;
        buf_.assign(size_ * 2, Data{ numeric_limits<double>::max(), 1 });
    }

    void InitSet(double evalScore, int index) {
        buf_[size_ + index].evalScore = evalScore;
        buf_[size_ + index].index = index;
    }
    void InitUpdate(int size) {
        RFOR(k, 1, ((size_ + size - 1) >> 1) + 1) {
            buf_[k] = SegTree::Min(buf_[k << 1], buf_[(k << 1) ^ 1]);
        }
    }
    void Set(double evalScore, int index) {
        int p = index + size_;
        buf_[p].evalScore = evalScore;
        REP(i, depth_) {
            int s = p >> i;
            int k = s >> 1;
            buf_[k] = SegTree::Min(buf_[s], buf_[s ^ 1]);
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
    int width_;
    SegTree seg_;
    dvec<Node<T>> nodePool_;
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



template <class State>
struct BeamState {
	int ai_;
	State state_;
};

template <class State, class Action>
struct BeamSearch {
	struct Candidate {
		double evalScore_;	
		u64 hash_;			
		bool end_;			
		int rawScore_;		

		Action action_;
	};
	struct StateAction {
		int si_;
		Action action_;
	};

	ActionTree<Action> actionTree_;
	dvec<BeamState<State>> curStates_;
	dvec<BeamState<State>> nextStates_;

	BeamFilter<StateAction> filter_;

	int bestAi_;
	int processWidth_;

	void Init(int actionCap, int stateCap) {
		actionTree_.Init(actionCap);
		curStates_.reserve(stateCap);
		nextStates_.reserve(stateCap);

		filter_.Reserve(stateCap);
	}

	template <class EnumCandidate, class CalcWidth, class Apply>
	void Run(EnumCandidate&& enumCandidate, CalcWidth&& calcWidth, Apply&& apply) {
		{
			auto& state = curStates_.pushR();
			state.ai_ = -1;
			state.state_.Init();
		}

		int bestRawScore = INT_MIN;
		bestAi_ = -1;
		processWidth_ = 0;
		int turn = 0;

		[&] {
			while (true) {
				int width = calcWidth(turn, processWidth_);
				if (width <= 0) {
					break;
				}
				chmin(width, curStates_.capacity());
				filter_.Init(width);

				REP(si, curStates_.size()) {
					auto& state = curStates_[si];

					++processWidth_;

					enumCandidate(turn, state.state_, [&](
						double evalScore,
						u64 hash,
						bool end,
						int rawScore,
						Action action
						) {
						if (end) {
							if (rawScore > bestRawScore) {
								bestRawScore = rawScore;
								bestAi_ = actionTree_.Push(state.ai_, action);
							}
							return;
						}
						auto* data = filter_.TryPush(evalScore, hash);
						if (data) {
							data->si_ = si;
							data->action_ = action;
						}
						});
				}

				nextStates_.clear();

				for (cauto& node : filter_.nodePool_) {
					auto& state = nextStates_.pushR();
					if constexpr (is_trivially_copyable_v<State>) {
						memcpy(&state.state_, &curStates_[node.data_.si_].state_, sizeof(state.state_));
					}
					else {
						state.state_ = curStates_[node.data_.si_].state_;
					}
					state.ai_ = actionTree_.Push(curStates_[node.data_.si_].ai_, node.data_.action_);
					apply(state.state_, node.data_.action_);
				}
				swap(curStates_, nextStates_);
				++turn;

				if (curStates_.empty()) {
					break;
				}
			}
			}();

	}

	bool GetBest(dvec<Action>& actions) const {
		if (bestAi_ < 0) {
			return false;
		}
		actionTree_.GetActionList(bestAi_, actions);
		return true;
	}
};


struct BitGrid {
	u32 bits_ = 0;

	void Clear() {
		bits_ = 0;
	}
	void SetFive(s8 five) {
		bits_ |= 1 << gs2.FiveToSix(five);
	}
	void ResetFive(s8 five) {
		bits_ &= ~(1 << gs2.FiveToSix(five));
	}
	bool GetFive(s8 five) const {
		return (bits_ >> gs2.FiveToSix(five)) & 1;
	}

	void SetSix(s8 six) {
		bits_ |= 1 << six;
	}
	void ResetSix(s8 six) {
		bits_ &= ~(1 << six);
	}
	bool GetSix(s8 six) const {
		return (bits_ >> six) & 1;
	}


};
constexpr BitGrid BitGridMask = { 0b11111011111011111011111011111 };


struct Zobrist {
	array<u64, NN> bigArmHash_;
	array<u64, NN> smallArmHash_;
	array<array<u64, NN>, NN> boxHash_;

	void Init() {
		REP(i, NN) {
			bigArmHash_[i] = rand_.Get64();
			smallArmHash_[i] = rand_.Get64();
		}
		REP(box, NN) {
			REP(cell, NN) {
				boxHash_[box][cell] = rand_.Get64();
			}
		}
	}

	const array<u64, NN>& GetArmHash(s8 ai) const {
		if (ai == 0) {
			return bigArmHash_;
		}
		else {
			return smallArmHash_;
		}
	}

};
Zobrist zobrist_;

struct Frame {
	BitGrid noBoxGrid_;					
	BitGrid noArmGrid_;					

	array<BitGrid, 4> armCrossMask_;	

	void InitFirst() {
		noBoxGrid_ = BitGridMask;
		noArmGrid_ = BitGridMask;
		REP(i, 4) {
			armCrossMask_[i] = BitGridMask;
		}

		REP(y, N) {
			noBoxGrid_.ResetFive(y * N);
			noArmGrid_.ResetFive(y * N);
		}
	}
	void InitZero() {
		noBoxGrid_ = BitGridMask;
		noArmGrid_ = BitGridMask;
		REP(i, 4) {
			armCrossMask_[i] = BitGridMask;
		}

		REP(y, N) {
			noBoxGrid_.ResetFive(y * N);
		}
	}
};

template <class T>
using FArr = CapArr<T, FrameCap>;

struct Finder {
	array<BitGrid, FrameCap> movables_;

	void CalcBoxsGetTurn(
		const array<Frame, FrameCap>& frames,
		const array<s8, NN>& boxGrid,
		const array<s8, NN>& boxTurn,
		const array<s8, NN>& boxPos,
		const NNArr<s8>& boxs,
		array<s8, NN>& boxsGetTurn,
		s8 startTurn, 
		s8 from,
		s8 limitTurn
	) {
		NNArr<s8> boxLeft;
		for (s8 box : boxs) {
			if (startTurn > 0 && boxPos[box] == from && startTurn == boxTurn[box]) {
				boxsGetTurn[box] = -1;
			}
			else {
				boxsGetTurn[box] = -2;
				boxLeft.push(box);
			}
		}

		BitGrid gettables;

		BitGrid curMovable;
		curMovable.SetFive(from);

		BitGrid nextMovable;
		for (s8 turn = startTurn + 1; turn < limitTurn; ++turn) {
			cauto& nextFrame = frames[turn];

			nextMovable .bits_ = curMovable.bits_;

			nextMovable.bits_ |= (curMovable.bits_ >> 1) & nextFrame.armCrossMask_[0].bits_;
			nextMovable.bits_ |= (curMovable.bits_ >> 6) & nextFrame.armCrossMask_[1].bits_;
			nextMovable.bits_ |= (curMovable.bits_ << 1) & nextFrame.armCrossMask_[2].bits_;
			nextMovable.bits_ |= (curMovable.bits_ << 6) & nextFrame.armCrossMask_[3].bits_;
			nextMovable.bits_ &= nextFrame.noArmGrid_.bits_;

			RREP(bi, boxLeft.size()) {
				s8 box = boxLeft[bi];
				if (turn > boxTurn[box]) {
					gettables.SetFive(boxPos[box]);
					boxLeft.RemoveSwap(bi);
				}
			}
			u32 bit = gettables.bits_ & curMovable.bits_ & nextMovable.bits_;
			if (bit) {
				gettables.bits_ &= ~bit;
				do {
					s8 six = std::countr_zero(bit);
					s8 box = boxGrid[gs2.SixToFive(six)];
					boxsGetTurn[box] = turn - startTurn;

					bit &= ~(1 << six);
				} while (bit);

				if (gettables.bits_ == 0 && boxLeft.empty()) {
					break;
				}
			}

			curMovable = nextMovable;
		}
	}

	void CalcBoxsPutTurn(
		const array<Frame, FrameCap>& frames,
		const array<s8, NN>& puttableTurn,
		const NNArr<s8>& tos,
		array<s8, NN>& tosPutTurn,
		s8 startTurn,
		s8 from,
		bool isBig,
		s8 limitTurn
	) {
		NNArr<s8> toLeft;
		for (s8 to : tos) {
			tosPutTurn[to] = -1;
			toLeft.push(to);
		}

		BitGrid puttables;

		BitGrid curMovable;
		curMovable.SetFive(from);

		BitGrid nextMovable;
		for (s8 turn = startTurn + 1; turn < limitTurn; ++turn) {
			cauto& nextFrame = frames[turn];

			nextMovable.bits_ = curMovable.bits_;

			nextMovable.bits_ |= (curMovable.bits_ >> 1) & nextFrame.armCrossMask_[0].bits_;
			nextMovable.bits_ |= (curMovable.bits_ >> 6) & nextFrame.armCrossMask_[1].bits_;
			nextMovable.bits_ |= (curMovable.bits_ << 1) & nextFrame.armCrossMask_[2].bits_;
			nextMovable.bits_ |= (curMovable.bits_ << 6) & nextFrame.armCrossMask_[3].bits_;
			nextMovable.bits_ &= nextFrame.noArmGrid_.bits_;
			if (!isBig) {
				nextMovable.bits_ &= nextFrame.noBoxGrid_.bits_;
			}

			RREP(ti, toLeft.size()) {
				s8 to = toLeft[ti];
				if (turn > puttableTurn[to]) {
					puttables.SetFive(to);
					toLeft.RemoveSwap(ti);
				}
			}
			u32 bit = puttables.bits_ & curMovable.bits_ & nextMovable.bits_;
			if (bit) {
				puttables.bits_ &= ~bit;
				do {
					s8 six = std::countr_zero(bit);
					s8 to = gs2.SixToFive(six);
					tosPutTurn[to] = turn - startTurn;

					bit &= ~(1 << six);
				} while (bit);

				if (puttables.bits_ == 0 && toLeft.empty()) {
					break;
				}
			}
			curMovable = nextMovable;
		}
	}

	bool MakeFindBoxMap(const array<Frame, FrameCap>& frames, s8 gottableTurn, int startTurn, s8 from, s8 boxPos, int limitTurn, int& endTurn) {
		movables_[startTurn].Clear();
		movables_[startTurn].SetFive(from);

		s8 sixBoxPos = gs2.FiveToSix(boxPos);

		FOR(turn, startTurn, limitTurn - 1) {
			cauto& nextFrame = frames[turn + 1];

			cauto& curMovable = movables_[turn];
			auto& nextMovable = movables_[turn + 1];

			nextMovable.bits_ = curMovable.bits_;
			nextMovable.bits_ |= (curMovable.bits_ >> 1) & nextFrame.armCrossMask_[0].bits_;
			nextMovable.bits_ |= (curMovable.bits_ >> 6) & nextFrame.armCrossMask_[1].bits_;
			nextMovable.bits_ |= (curMovable.bits_ << 1) & nextFrame.armCrossMask_[2].bits_;
			nextMovable.bits_ |= (curMovable.bits_ << 6) & nextFrame.armCrossMask_[3].bits_;
			nextMovable.bits_ &= nextFrame.noArmGrid_.bits_;

			if (turn >= gottableTurn && curMovable.GetSix(sixBoxPos) && nextMovable.GetSix(sixBoxPos)) {
				endTurn = turn;
				return true;
			}
		}
		return false;
	}


	bool FindBoxRoute2(const array<Frame, FrameCap>& frames, s8 gottableTurn, int startTurn, s8 from, s8 boxPos, int limitTurn, FArr<s8>& route) {
		int endTurn;
		if (MakeFindBoxMap(frames, gottableTurn, startTurn, from, boxPos, limitTurn, endTurn)) {
			GetRandRoute(frames, startTurn, endTurn, from, boxPos, route);
			return true;
		}
		return false;
	}

	bool MakeFindCarryMap(const array<Frame, FrameCap>& frames, s8 puttableTurn, int startTurn, s8 from, s8 box, s8 to, bool isBig, int limitTurn, int& endTurn) {
		movables_[startTurn].Clear();
		movables_[startTurn].SetFive(from);

		s8 sixTo = gs2.FiveToSix(to);
		BitGrid curMovable = movables_[startTurn];
		BitGrid nextMovable;
		FOR(turn, startTurn + 1, limitTurn) {
			cauto& nextFrame = frames[turn];

			nextMovable.bits_ = curMovable.bits_;
			nextMovable.bits_ |= (curMovable.bits_ >> 1) & nextFrame.armCrossMask_[0].bits_;
			nextMovable.bits_ |= (curMovable.bits_ >> 6) & nextFrame.armCrossMask_[1].bits_;
			nextMovable.bits_ |= (curMovable.bits_ << 1) & nextFrame.armCrossMask_[2].bits_;
			nextMovable.bits_ |= (curMovable.bits_ << 6) & nextFrame.armCrossMask_[3].bits_;
			nextMovable.bits_ &= nextFrame.noArmGrid_.bits_;
			if (!isBig) {
				nextMovable.bits_ &= nextFrame.noBoxGrid_.bits_;
			}
			movables_[turn] = nextMovable;


			if (turn > puttableTurn && curMovable.GetSix(sixTo) && nextMovable.GetSix(sixTo)) {
				endTurn = turn - 1;
				return true;
			}

			curMovable = nextMovable;
		}
		return false;
	}


	bool FindCarryRoute(const array<Frame, FrameCap>& frames, s8 puttableTurn, int startTurn, s8 from, s8 box, s8 to, bool isBig, int limitTurn, s8& dist) {
		int endTurn;
		if (MakeFindCarryMap(frames, puttableTurn, startTurn, from, box, to, isBig, limitTurn, endTurn)) {
			dist = endTurn - startTurn + 1;
			return true;
		}
		return false;
	}
	bool FindCarryRoute(const array<Frame, FrameCap>& frames, s8 puttableTurn, int startTurn, s8 from, s8 box, s8 to, bool isBig, int limitTurn, FArr<s8>& route) {
		int endTurn;
		if (MakeFindCarryMap(frames, puttableTurn, startTurn, from, box, to, isBig, limitTurn, endTurn)) {
			GetRandRoute(frames, startTurn, endTurn, from, to, route);
			return true;
		}
		return false;
	}


	void GetRandRoute(const array<Frame, FrameCap>& frames, int startTurn, int endTurn, s8 from, s8 to, FArr<s8>& route) const {
		cauto& arounds = gs.randArounds5s_[rand_(Grid::RAND_COUNT)];

		int turn = endTurn;
		s8 p = to;
		while (true) {
			route.push(p);

			if (turn == startTurn) {
				break;
			}

			cauto& nextMovable = movables_[turn - 1];
			s8 nextPos = -1;
			for (s8 n : arounds[p]) {
				if (nextMovable.GetFive(n)) {
					s8 d = (s8)gs.CalcDir(n, p);
					if (d < 4) {
						if (!frames[turn].armCrossMask_[d].GetFive(p)) {
							continue;
						}
					}
					nextPos = n;
					break;
				}
			}
			--turn;
			VASSERT(nextPos >= 0);
			p = nextPos;
		}
		route.reverse();
	}

};
Finder finder_;

constexpr array<BitGrid, N> OpenCellBits = {
	BitGrid{ 0b00000000000000000000000001110},
	BitGrid{ 0b00000000000000000001110000000},
	BitGrid{ 0b00000000000001110000000000000},
	BitGrid{ 0b00000001110000000000000000000},
	BitGrid{ 0b01110000000000000000000000000},
};


array<double, FrameCap> totalTurnScore = {};
array<double, FrameCap> bigArmTurnScore = {};
array<double, FrameCap> smallArmTurnScore = {};

array<double, 6> inStepScore = {};
array<double, 6> outStepScore = {};


array<double, NN> tmpCellScore = {};
array<double, 24> trerBadLevelScore = {};

void InitEvalTbl() {
	REP(i, FrameCap) {
		totalTurnScore[i] = i * HP.TotalTurnA * exp(i * HP.TotalTurnB);
		bigArmTurnScore[i] = i * HP.BigArmFreeTurnA * exp(i * HP.BigArmFreeTurnB);
		smallArmTurnScore[i] = i * HP.SmallArmFreeTurnA * exp(i * HP.SmallArmFreeTurnB);
	}

	REP(k, 6) {
		inStepScore[k] = HP.InStepA * exp(k * HP.InStepB);
	}


	outStepScore[0] = 0;
	outStepScore[1] = HP.OutStep1;
	outStepScore[2] = HP.OutStep2;
	outStepScore[3] = HP.OutStep3;
	outStepScore[4] = HP.OutStep4;
	outStepScore[5] = HP.OutStep5;


	tmpCellScore[gs.ToId(1, 0)] = HP.TmpCell_01 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(2, 0)] = HP.TmpCell_02 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(3, 0)] = HP.TmpCell_03 * HP.TmpCellScale;

	tmpCellScore[gs.ToId(1, 1)] = HP.TmpCell_11 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(2, 1)] = HP.TmpCell_12 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(3, 1)] = HP.TmpCell_13 * HP.TmpCellScale;

	tmpCellScore[gs.ToId(1, 2)] = HP.TmpCell_21 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(2, 2)] = HP.TmpCell_22 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(3, 2)] = HP.TmpCell_23 * HP.TmpCellScale;

	tmpCellScore[gs.ToId(1, 3)] = HP.TmpCell_11 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(2, 3)] = HP.TmpCell_12 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(3, 3)] = HP.TmpCell_13 * HP.TmpCellScale;

	tmpCellScore[gs.ToId(1, 4)] = HP.TmpCell_01 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(2, 4)] = HP.TmpCell_02 * HP.TmpCellScale;
	tmpCellScore[gs.ToId(3, 4)] = HP.TmpCell_03 * HP.TmpCellScale;

	FOR(level, 1, HP.EnableBadLevel + 1) {
		trerBadLevelScore[level] = level * HP.TreeBadLevelA * exp(level * HP.TreeBadLevelB);
	}
}

struct CommanderState {
	array<Frame, FrameCap> frames_;
	array<s8, N> ins_;			
	array<s8, NN> boxTurn_;		
	array<s8, NN> puttableTurn_;		

	struct EvalState {
		array<s8, N> armFreeTurn_;			
		array<s8, N> armPos_;				
		array<s8, NN> boxGrid_;		
		array<s8, NN> boxPos_;		
		BitGrid tmpBitGrid_;		
		array<s8, N> outs_;
		array<s8, N> taskIns_;		
		s8 outCount_;				

		int gotLoss_;		
		int carryLoss_;		
		s8 tmpPutCount_;	
		int getTurn_;		
		int carryTurn_;		
		s8 totalTurn_;		

		u64 hash_;

		double armEval_;
		double inEval_;
		double outEval_;
		double boxEval_;
	};
	EvalState es_;

	void Init() {
		frames_[0].InitFirst();
		FOR(fi, 1, FrameCap) {
			frames_[fi].InitZero();
		}

		ins_.fill(1);
		boxTurn_.fill(-1);
		REP(y, N) {
			s8 box = server.ques_[y][0];
			boxTurn_[box] = 0;
		}
		puttableTurn_.fill(0);

		es_.armFreeTurn_.fill(0);
		REP(y, N) {
			es_.armPos_[y] = N * y;
		}

		es_.boxGrid_.fill(-1);
		es_.boxPos_.fill(-1);
		es_.tmpBitGrid_.Clear();
		REP(y, N) {
			s8 box = server.ques_[y][0];
			es_.boxGrid_[y * N] = box;
			es_.boxPos_[box] = y * N;
		}
		es_.outs_.fill(0);
		es_.taskIns_.fill(0);
		es_.outCount_ = 0;

		es_.gotLoss_ = 0;
		es_.carryLoss_ = 0;
		es_.tmpPutCount_ = 0;
		es_.getTurn_ = 0;
		es_.carryTurn_ = 0;
		es_.totalTurn_ = 0;

		es_.hash_ = 0;
		REP(y, N) {
			s8 box = server.ques_[y][0];
			es_.hash_ ^= zobrist_.boxHash_[box][y * N];

			cauto& armHash = zobrist_.GetArmHash(y);
			es_.hash_ ^= armHash[y * N];
		}

		es_.armEval_ = 0;
		es_.inEval_ = 0;
		es_.outEval_ = 0;
		es_.boxEval_ = 0;
	}

	bool CalcRoute(s8 ai, s8 box, s8 to, int limitTurn, FArr<s8>& gotRoute, FArr<s8>& carryRoute, int& startTurn, int& endTurn) const {
		int turn = es_.armFreeTurn_[ai];

		s8 pos = es_.armPos_[ai];

		gotRoute.clear();
		int gotedTurn;
		s8 gotedPos;
		if (turn > 0 && es_.boxPos_[box] == pos && turn == boxTurn_[box]) {
			gotedTurn = turn - 1;
			gotedPos = pos;
		}
		else {
			if (!finder_.FindBoxRoute2(frames_, boxTurn_[box], turn, pos, es_.boxPos_[box], limitTurn, gotRoute)) {
				return false;
			}
			gotedTurn = turn + gotRoute.size();		
			gotedPos = gotRoute.back();

			VASSERT(gotedTurn >= boxTurn_[box] + 1);
		}

		carryRoute.clear();
		if (!finder_.FindCarryRoute(frames_, puttableTurn_[to], gotedTurn, gotedPos, box, to, ai == 0, limitTurn, carryRoute)) {
			return false;
		}

		startTurn = turn;		
		endTurn = gotedTurn + carryRoute.size();

		return true;
	}

	bool PreCalcRouteWithPreCalc(s8 ai, s8 box, s8 to, int limitTurn, s8 boxGetTurn, bool calcPutTurn, s8 putTurn, s8& gotDist, s8& carryDist, int& startTurn, int& endTurn) const {
		int turn = es_.armFreeTurn_[ai];

		gotDist = -1;
		int gotedTurn;
		s8 gotedPos = es_.boxPos_[box];

		if (boxGetTurn == -2) {
			return false;
		}
		else if (boxGetTurn == -1) {
			gotedTurn = turn - 1;
		}
		else {
			VASSERT(boxGetTurn >= 0);
			gotDist = boxGetTurn;
			gotedTurn = turn + gotDist;		

			VASSERT(gotedTurn >= boxTurn_[box] + 1);
		}

		carryDist = -1;
		if (calcPutTurn) {
			if (!finder_.FindCarryRoute(frames_, puttableTurn_[to], gotedTurn, gotedPos, box, to, ai == 0, limitTurn, carryDist)) {
				return false;
			}
		}
		else {
			if (putTurn < 0) {
				return false;
			}
			carryDist = putTurn;
		}

		startTurn = turn;		
		endTurn = gotedTurn + carryDist;

		return true;
	}

	bool EvalNextState(s8 ai, s8 box, s8 to, s8 gotDist, s8 carryDist, int startTurn, s8 bestScore, double& eval, u64& hash, s8& totalTurn, bool& end) const {
		EvalState rv = es_;
		CommanderState::UpdateEvalState(ai, box, to, gotDist, carryDist, startTurn, rv);

		if (rv.totalTurn_ >= bestScore) {
			return false;
		}

		if ((rv.tmpBitGrid_.bits_ & OpenCellBits[2].bits_) == 0 ||
			(rv.tmpBitGrid_.bits_ & OpenCellBits[1].bits_) == 0 ||
			(rv.tmpBitGrid_.bits_ & OpenCellBits[3].bits_) == 0 ||
			(rv.tmpBitGrid_.bits_ & OpenCellBits[0].bits_) == 0 ||
			(rv.tmpBitGrid_.bits_ & OpenCellBits[4].bits_) == 0) {
		}
		else {
			return false;
		}
		eval = 0;


		eval -= totalTurnScore[rv.totalTurn_];
		eval -= rv.gotLoss_ * (ai == 0 ? HP.BigGotLoss : HP.SmallGotLoss);
		eval -= rv.carryLoss_ * (ai == 0 ? HP.BigCarryLoss : HP.SmallCarryLoss);
		eval -= rv.tmpPutCount_ * HP.TmpPutCount;

		s8 badLevel = taskTree_.GetTreeState(rv.taskIns_).badLevel_;
		VASSERT(badLevel >= 0 && badLevel <= HP.EnableBadLevel);
		eval -= trerBadLevelScore[badLevel];

		eval -= rv.getTurn_ * HP.GetTurn;
		eval -= rv.carryTurn_ * HP.CarryTurn;

		eval += rv.armEval_;
		eval += rv.inEval_;
		eval += rv.outEval_;
		eval += rv.boxEval_;




		hash = rv.hash_;
		end = rv.outCount_ == NN;
		totalTurn = rv.totalTurn_;

		return true;
	}

	static void UpdateEvalState(s8 ai, s8 box, s8 to, s8 gotDist, s8 carryDist, int startTurn, EvalState& es) {
		s8 gotedTurn;		
		if (gotDist < 0) {
			gotedTurn = startTurn - 1;
		}
		else {
			gotedTurn = startTurn + gotDist;
		}
		s8 putedTurn = gotedTurn + carryDist;		
		s8 gotedPos = es.boxPos_[box];
		s8 puttedPos = to;
		s8 startPos = es.armPos_[ai];
		cauto& gotedP = gs.ToPos(gotedPos);
		cauto& puttedP = gs.ToPos(puttedPos);

		{
			if (gotedP.x > 0) {
				es.tmpBitGrid_.ResetFive(gotedPos);
				es.boxEval_ += tmpCellScore[gotedPos];
			}
			VASSERT(es.boxGrid_[gotedPos] == box);
			es.boxGrid_[gotedPos] = -1;
			es.hash_ ^= zobrist_.boxHash_[box][gotedPos];
		}

		if (gotedP.x == 0 && es.taskIns_[gotedP.y] < N) {
			++es.taskIns_[gotedP.y];
			es.inEval_ += HP.InCount;
			es.inEval_ -= inStepScore[es.taskIns_[gotedP.y]];

			if (es.taskIns_[gotedP.y] < N) {
				s8 inBox = server.ques_[gotedP.y][es.taskIns_[gotedP.y]];
				es.boxPos_[inBox] = gotedPos;
				es.boxGrid_[gotedPos] = inBox;
				es.hash_ ^= zobrist_.boxHash_[inBox][gotedPos];
			}
		}

		if (puttedP.x == 4) {
			++es.outs_[puttedP.y];
			es.outEval_ += HP.OutCount;
			es.outEval_ -= outStepScore[es.outs_[puttedP.y]];

			es.boxPos_[box] = -1;
			++es.outCount_;
		}
		else {
			es.boxPos_[box] = puttedPos;
			es.boxGrid_[puttedPos] = box;
			es.hash_ ^= zobrist_.boxHash_[box][puttedPos];

			VASSERT(puttedP.x > 0);
			es.tmpBitGrid_.SetFive(puttedPos);
			es.boxEval_ -= tmpCellScore[puttedPos];

			++es.tmpPutCount_;
		}

		cauto& armHash = zobrist_.GetArmHash(ai);
		es.hash_ ^= armHash[startPos];
		es.hash_ ^= armHash[puttedPos];

		if (gotDist >= 0) {
			es.gotLoss_ += gotDist - 1 - gs.CalcL1Dist(startPos, gotedPos);
			es.getTurn_ += gotDist - 1;
		}
		es.carryLoss_ += carryDist - 1 - gs.CalcL1Dist(gotedPos, puttedPos);
		es.carryTurn_ += carryDist - 1;

		chmax(es.totalTurn_, putedTurn);

		cauto& armTurnScore = ai == 0 ? bigArmTurnScore : smallArmTurnScore;
		es.armEval_ += armTurnScore[es.armFreeTurn_[ai]];
		es.armFreeTurn_[ai] = putedTurn;
		es.armEval_ -= armTurnScore[es.armFreeTurn_[ai]];

		es.armPos_[ai] = puttedPos;
	}

	void Regist(s8 ai, s8 box, int limitTurn, const FArr<s8>& gotRoute, const FArr<s8>& carryRoute, int startTurn) {
		s8 gotedTurn;		
		if (gotRoute.empty()) {
			gotedTurn = startTurn - 1;
		}
		else {
			gotedTurn = startTurn + gotRoute.size();
		}
		s8 putedTurn = gotedTurn + carryRoute.size();		
		s8 gotedPos = es_.boxPos_[box];
		s8 puttedPos = carryRoute.back();
		s8 startPos = es_.armPos_[ai];
		cauto& gotedP = gs.ToPos(gotedPos);
		cauto& puttedP = gs.ToPos(puttedPos);

		{
			int t = startTurn;
			if (gotRoute.empty()) {
				VASSERT(!frames_[startTurn].noArmGrid_.GetFive(gotedPos));
				frames_[startTurn].noArmGrid_.SetFive(gotedPos);

				FOR(t2, startTurn, limitTurn) {
					VASSERT(!frames_[t2].noBoxGrid_.GetFive(gotedPos));
					frames_[t2].noBoxGrid_.SetFive(gotedPos);
				}

				--t;
			}
			else {
				FOR(i, 1, gotRoute.size()) {
					++t;
					s8 d = (s8)gs.CalcDir(gotRoute[i], gotRoute[i - 1]);
					if (d < 4) {
						frames_[t].armCrossMask_[d].ResetFive(gotRoute[i - 1]);
					}
					frames_[t].noArmGrid_.ResetFive(gotRoute[i]);
				}

				{
					++t;
					frames_[t].noArmGrid_.ResetFive(gotedPos);
					VASSERT(!frames_[t].noBoxGrid_.GetFive(gotedPos));

					chmax(puttableTurn_[gotedPos], t);
				}
			}

			bool moved = false;
			FOR(i, 1, carryRoute.size()) {
				++t;
				frames_[t].noArmGrid_.ResetFive(carryRoute[i]);
				s8 d = (s8)gs.CalcDir(carryRoute[i], carryRoute[i - 1]);
				if (d < 4) {
					frames_[t].armCrossMask_[d].ResetFive(carryRoute[i - 1]);
				}

				chmax(puttableTurn_[carryRoute[i]], t);

				if (!moved && carryRoute[i - 1] != carryRoute[i]) {
					moved = true;

					if (gotedP.x == 0 && ins_[gotedP.y] < N) {
						s8 inBox = server.ques_[gotedP.y][ins_[gotedP.y]];
						++ins_[gotedP.y];
						boxTurn_[inBox] = t;		

						frames_[t - 1].noBoxGrid_.SetFive(gotedPos);
					}
					else {
						FOR(t2, t, limitTurn) {
							frames_[t2].noBoxGrid_.SetFive(gotedPos);
						}
					}
				}

				s8 prev = carryRoute[i - 1];
				cauto& p = gs.ToPos(prev);
				if (p.x == 0 && ins_[p.y] < N) {
					if (frames_[t - 1].noBoxGrid_.GetFive(prev)) {
						VASSERT(!frames_[t].noBoxGrid_.GetFive(prev));
						frames_[t].noBoxGrid_.SetFive(prev);
					}
				}

			}

			{
				++t;
				frames_[t].noArmGrid_.ResetFive(puttedPos);
				VASSERT(frames_[t].noBoxGrid_.GetFive(puttedPos));


				if (puttedP.x == 4) {
					boxTurn_[box] = -1;
				}
				else {
					boxTurn_[box] = t;

					s8 puttedSix = gs2.FiveToSix(puttedPos);
					FOR(t2, t, limitTurn) {
						frames_[t2].noBoxGrid_.ResetSix(puttedSix);
					}
				}

				chmax(puttableTurn_[puttedPos], t);
			}
		}

		UpdateEvalState(ai, box, puttedPos, gotRoute.empty() ? -1 : gotRoute.size(), carryRoute.size(), startTurn, es_);
	}

	static void RegistToResult(s8 ai, const FArr<s8>& gotRoute, const FArr<s8>& carryRoute, IOResult& result) {
		auto& actions = result.armsActions_[ai];
		if (gotRoute.empty()) {
			VASSERT(actions.back() == IOAction::Release);
			actions.pop();
		}
		else {
			FOR(i, 1, gotRoute.size()) {
				actions.push(gs.CalcDir(gotRoute[i - 1], gotRoute[i]));
			}
			{
				actions.push(IOAction::Catch);
			}
		}
		FOR(i, 1, carryRoute.size()) {
			actions.push(gs.CalcDir(carryRoute[i - 1], carryRoute[i]));
		}
		{
			actions.push(IOAction::Release);
		}
	}
};
#define VALIDATE_DIST 0


struct CommanderAction3 {
	s8 ai_;			
	s8 box_;		
	s8 to_;			
	u64 seed_;

};

bool CommanderBeam3(IOResult& result) {
	taskTree_;
	taskTree_.Init();

	using BS = BeamSearch<CommanderState, CommanderAction3>;
	static BS bs;
	bs.Init(800000, 40000);

	FArr<s8> gotRoute;
	FArr<s8> carryRoute;
	CommanderAction3 action;

	InitEvalTbl();

	constexpr int limitTurn = FrameCap;
	s8 bestScore = FrameCap;

	zobrist_.Init();

	int maxTurn = min(50, 25 + taskTree_.GetTmpCount() + HP.AppendTurn);		
	auto startTime = timer_.Now();
	auto endTime = timer_.EndTime();

	auto CalcWidth = [&](int turn, int processWidth) -> int {
		if (turn < HP.MeasureProcessTurn) {
			return HP.InitialBeamWidth;
		}
		else {
			if (timer_.IsOver() && bs.bestAi_ >= 0) {
				return -1;
			}

			auto now = timer_.Now();
			double elapsed = chrono::duration_cast<chrono::microseconds>(now - startTime).count() / 1000000.0;
			double timeLeft = max(0.0, chrono::duration_cast<chrono::microseconds>(endTime - now).count() / 1000000.0);
			int turnLeft = max(maxTurn - turn, (int)HP.WidthOverTurn);

			double processPerTime = processWidth / (double)elapsed;

			double executableProcess = processPerTime * timeLeft;


			double turnProcess = executableProcess / (double)turnLeft;

			turnProcess *= HP.WidthProcessRate;		


			return max(100, (int)turnProcess);
		}
		};

	auto EnumCandidate = [&](int turn, const CommanderState& state, auto&& callback) {
		auto Add = [&](s8 ai, s8 box, s8 to, s8 getTurn, bool calcPutTurn, s8 putTurn) {
			int startTurn = -1;
			int endTurn = -1;
			u64 seed = rand_.x++;
			s8 gotDist = -1;
			s8 carryDist = -1;

			s8 boxPos = state.es_.boxPos_[box];
			s8 minD = gs.CalcL1Dist(state.es_.armPos_[ai], boxPos) + gs.CalcL1Dist(boxPos, to) + 2;
			int limit = state.es_.armFreeTurn_[ai] + minD + HP.SkipOverTurn + 1;

			if (state.PreCalcRouteWithPreCalc(ai, box, to, min(limit, limitTurn), getTurn, calcPutTurn, putTurn, gotDist, carryDist, startTurn, endTurn)) {
				if (endTurn >= limit) {
					return;
				}

				double eval = 0;
				s8 totalTurn = FrameCap;
				bool end = false;
				u64 hash = 0;
				if (!state.EvalNextState(ai, box, to, gotDist, carryDist, startTurn, bestScore, eval, hash, totalTurn, end)) {
					return;
				}

				if (end) {
					chmin(bestScore, totalTurn);
				}

				action.ai_ = ai;
				action.box_ = box;
				action.to_ = to;
				action.seed_ = seed;
				callback(eval, hash, end, -totalTurn, action);
			}
			};

		NNArr<s8> boxs;
		NNArr<s8> tmpPoss;
		{
			cauto& recommendYs = taskTree_.GetRecommend(state.es_.taskIns_);
			for (s8 y : recommendYs) {
				s8 box = server.ques_[y][state.es_.taskIns_[y]];
				VASSERT(state.es_.boxPos_[box] == gs.ToId(0, y));
				boxs.push(box);
			}

			FOR(x, 1, 4) {
				REP(y, N) {
					s8 pos = gs.ToId(x, y);
					s8 box = state.es_.boxGrid_[pos];
					if (box > 0) {
						VASSERT(boxs.find(box) < 0);
						boxs.push(box);
					}
					else {
						tmpPoss.push(pos);
					}
				}
			}
		}

		static array<NNArr<s8>, NN> boxsTmpPoss;	
		for (s8 box : boxs) {
			s8 boxPos = state.es_.boxPos_[box];
			VASSERT(boxPos >= 0);
			pt boxP = gs.ToPos(boxPos);

			s8 outPos = box / N * N + 4;
			if (box / N * N + state.es_.outs_[box / N] == box) {

				continue;
			}

			if (boxP.x == 0 && state.es_.taskIns_[boxP.y] >= 4) {
				continue;
			}

			if (boxP.x == 0) {
				auto& tmpPoss2 = boxsTmpPoss[box];
				tmpPoss2.clear();

				for (s8 to : tmpPoss) {
					if (gs.CalcL1Dist(boxPos, outPos) <= gs.CalcL1Dist(to, outPos)) {
						continue;
					}
					if (boxPos == to) {
						continue;
					}
					tmpPoss2.push(to);
				}
			}
		}

		auto TryAdd = [&](s8 ai, const array<s8, NN>& boxGetTurn) {
			for (s8 box : boxs) {
				s8 boxPos = state.es_.boxPos_[box];
				VASSERT(boxPos >= 0);
				pt boxP = gs.ToPos(boxPos);

				s8 getTurn = boxGetTurn[box];
				if (getTurn == -2) {
					continue;
				}
				if (state.es_.armFreeTurn_[ai] == 0) {
					if (boxPos == state.es_.armPos_[ai]) {
					}
					else if (boxP.x == 0) {
						if (server.ques_[boxP.y][0] == box) {
							continue;
						}
						else {
						}
					}
					else {
					}
				}

				{
					s8 minD = gs.CalcL1Dist(state.es_.armPos_[ai], boxPos) + 1;
					if (getTurn > minD + HP.SkipOverTurn) {
						continue;
					}
				}

				s8 outPos = box / N * N + 4;
				if (box / N * N + state.es_.outs_[box / N] == box) {
					Add(ai, box, outPos, getTurn, true, -1);
					continue;
				}

				if (boxP.x == 0 && state.es_.taskIns_[boxP.y] >= 4) {
					continue;
				}

				if (boxP.x == 0) {
					cauto& tmpPoss2 = boxsTmpPoss[box];

					array<s8, NN> tosPutTurn;
					finder_.CalcBoxsPutTurn(
						state.frames_,
						state.puttableTurn_,
						tmpPoss2,
						tosPutTurn,
						state.es_.armFreeTurn_[ai] + getTurn,		
						state.es_.boxPos_[box],
						ai == 0,
						limitTurn
						);

					for (s8 to : tmpPoss2) {
						if (tosPutTurn[to] < 0) {
							continue;
						}
						Add(ai, box, to, getTurn, false, tosPutTurn[to]);
					}
				}
			}
			};

		s8 minTurn = FrameCap;
		s8 maxTurn = 0;
		REP(ai, N) {
			chmin(minTurn, state.es_.armFreeTurn_[ai]);
			chmax(maxTurn, state.es_.armFreeTurn_[ai]);
		}

		array<s8, NN> boxGetTurn;
		REP(ai, N) {
			if (minTurn != maxTurn) {
				if (maxTurn == state.es_.armFreeTurn_[ai]) {
					continue;
				}
			}

				if (state.es_.armFreeTurn_[ai] > minTurn + HP.ArmCutTurn) {
					continue;
				}

			finder_.CalcBoxsGetTurn(
				state.frames_,
				state.es_.boxGrid_,
				state.boxTurn_, 
				state.es_.boxPos_,
				boxs,
				boxGetTurn,
				state.es_.armFreeTurn_[ai],
				state.es_.armPos_[ai],
				limitTurn
				);

			TryAdd(ai, boxGetTurn);
		}


		};

	auto Apply = [&](CommanderState& state, const CommanderAction3& action) {
		int startTurn = -1;
		int endTurn = -1;
		rand_.x = action.seed_;
		if (state.CalcRoute(action.ai_, action.box_, action.to_, FrameCap, gotRoute, carryRoute, startTurn, endTurn)) {
			state.Regist(action.ai_, action.box_, FrameCap, gotRoute, carryRoute, startTurn);
		}
		else {
			VABORT();
		}
		};


	bs.Run(EnumCandidate, CalcWidth, Apply);

	dvec<CommanderAction3> actions;
	if (!bs.GetBest(actions)) {
		return false;
	}

	CommanderState state;
	state.Init();
	result.clear();

	REP(i, actions.size()) {
		cauto& action = actions[i];

		int startTurn = -1;
		int endTurn = -1;
		rand_.x = action.seed_;
		bool ret = state.CalcRoute(action.ai_, action.box_, action.to_, FrameCap, gotRoute, carryRoute, startTurn, endTurn);
		VASSERT(ret);
		state.Regist(action.ai_, action.box_, FrameCap, gotRoute, carryRoute, startTurn);
		CommanderState::RegistToResult(action.ai_, gotRoute, carryRoute, result);
	}


	int totalTurn = result.GetMaxTurn();
	REP(ai, N) {
		if (result.armsActions_[ai].size() < totalTurn) {
			result.armsActions_[ai].push(IOAction::Bomb);
		}
	}

	return true;
}

struct Solver {
	void Run() {

		IOResult result;
		if (!CommanderBeam3(result)) {
			VABORT();
		}
		server.Output(result);

	}
};

struct Main {
    void Run(int argc, const char* argv[]) {
        server.InitInput(timer_);

        timer_.StartMs(TIME_LIMIT);

        static Solver solver;
        solver.Run();

        server.Finalize();

    }
};
