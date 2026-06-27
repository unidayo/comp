#define FULL_LIB 1

#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0


#define TIME_LIMIT (2950)

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

#define ROBOT_SA 0

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

	inline T* push() {
		T* ptr = &ar_[end_];
		++end_;
		return ptr;
	}

	inline const T& get() const  {
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

	const T& operator[](int i) const{
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


#define NO_SWAP 0

template <class T, int CAP, class CMP>
struct CapIntervalHeap {
	CMP cmp;
	CapArr<T, CAP> buf_;		

	bool empty() const { return buf_.empty(); }
	int size() const { return buf_.size(); }
	constexpr int capacity() const { return CAP; }

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



template <class K>
struct CapHashSetR {
    size_t m_capacity = 0;
    K* m_keys = nullptr;
    uint8_t* m_states = nullptr;  
    size_t m_mask = 0;
    int m_count = 0;


    CapHashSetR(const CapHashSetR&) = delete;
    void operator=(const CapHashSetR&) = delete;

    CapHashSetR() = default;

    void init(size_t capacity) {
        if (capacity == m_capacity) {
            clear();
            return;
        }
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
                VASSERT(m_count < (int)m_capacity);

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
                VASSERT(m_count < (int)m_capacity);

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

enum class Dir : int8_t {
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
constexpr array<pint, 5> Around5 = {pint{-1, 0}, pint{0, -1}, pint{1, 0}, pint{0, 1}, pint{0, 0}};
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

inline Dir CalcDir(const pint& from, const pint& to) {
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
        'L', 'U', 'R', 'D', '.', '*',
    };
    return chars[(int)dir];
}


constexpr int N_min = 15;
constexpr int N_max = 30;
constexpr int NN_min = N_min * N_min;
constexpr int NN_max = N_max * N_max;
constexpr int M_min = N_min * N_min / 10;
constexpr int M_max = N_max * N_max / 2;
constexpr int V_min = 5;
constexpr int V_max = 15;

#define DEF_CONTAINERS(name, size)        \
    template <class TYPE>                 \
    using name##Arr = CapArr<TYPE, size>; \
    template <class TYPE>                 \
    using name##Que = CapQue<TYPE, size>;


DEF_CONTAINERS(N, N_max);
DEF_CONTAINERS(NN, NN_max);
DEF_CONTAINERS(M, M_max);
DEF_CONTAINERS(V, V_max);

int N;
int NN;
int M;
int V;

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



    PD(CandidateTargetScore, 11.010207800587064, 0.0, 20.0);
    PD(CandidateTakoScore, 26.126299995354085, 0.0, 20.0);
    PD(CandidateTargetPosCost, 3.6528921110494172, 0.0, 2.0);  
    PD(CandidateTakoPosCost, 2.972173939717964, 0.0, 2.0);     
    PD(CandidateRootPosCost, 0.5626839685982833, 0.0, 2.0);


    PI(CandidateHoldCount, 7, 1, 24, 1);
    PI(CandidateBranchHoldCount, 7, 1, 24, 1);
    PI(CandidateBranchHoldMin, 1, 1, 4, 1);


    START_TUNING;
    PD(TargetCountCost, 2.9052046085064287, 2.7, 3.1);
    PD(TakoCountCost, 1.650046325115058, 1.3, 2.0);
    PD(RootPosCost, 0.7504315206118098, 0.5, 1.0);
    PD(TakoPosCost, 0.8594379621546989, 0.6, 1.0);
    PD(TargetPosCost, 0.7753412924248925, 0.5, 1.0);
    PD(DimCostRate, 1.0, 0.8, 1.2);
    END_TUNING;

    PD(FreeMoveCostRate, 1.1093150360062987, 0.0, 1.0);
    PD(FreeMoveTurnRate, 2.4105060425438167, 0.0, 2.0);



} HP;


#undef PC
#undef PI
#undef PD





int CalcRotTurn(int curRot, int targetRot) {
    if (curRot == targetRot) {
        return 0;
    }
    else if (((curRot + 2) & 0b11) == targetRot) {
        return 2;
    }
    return 1;
}


struct GridSystem {
    NNArr<CapArr<int, 4>> aroundMap_;
    NNArr<CapArr<int, 4>> aroundDir_;
    NNArr<NNArr<int>> manhattanDists_;
    NNArr<NNArr<double>> euclidDists_;
    NNArr<pint> idToPt_;
    NArr<NArr<int>> ptToId_;

    void Init() {
        idToPt_.resize(NN);
        ptToId_.resize(N);
        REP(y, N) {
            ptToId_[y].resize(N);
            REP(x, N) {
                int cell = x + y * N;
                idToPt_[cell] = pint{x, y};
                ptToId_[y][x] = cell;
            }
        }

        aroundMap_.resize(NN);
        aroundDir_.resize(NN);
        REP(y, N) {
            REP(x, N) {
                int cell = ToId(x, y);
                aroundDir_[cell].assign(4, -1);
                if (x - 1 >= 0) {
                    int nc = ToId(x - 1, y);
                    aroundMap_[cell].push(nc);
                    aroundDir_[cell][(int)Dir::L] = nc;
                }
                if (y - 1 >= 0) {
                    int nc = ToId(x, y - 1);
                    aroundMap_[cell].push(nc);
                    aroundDir_[cell][(int)Dir::U] = nc;
                }
                if (x + 1 < N) {
                    int nc = ToId(x + 1, y);
                    aroundMap_[cell].push(nc);
                    aroundDir_[cell][(int)Dir::R] = nc;
                }
                if (y + 1 < N) {
                    int nc = ToId(x, y + 1);
                    aroundMap_[cell].push(nc);
                    aroundDir_[cell][(int)Dir::D] = nc;
                }
            }
        }

        euclidDists_.resize(NN);
        manhattanDists_.resize(NN);
        REP(i, NN) {
            euclidDists_[i].resize(NN);
            manhattanDists_[i].resize(NN);
        }

        REP(i, NN) {
            pint pa = ToPt(i);

            euclidDists_[i][i] = 0;
            manhattanDists_[i][i] = 0;

            FOR(j, i + 1, NN) {
                pint pb = ToPt(j);
                manhattanDists_[i][j] = manhattanDists_[j][i] = abs(pa.x - pb.x) + abs(pa.y - pb.y);
                euclidDists_[i][j] = euclidDists_[j][i] = sqrt(norm2(pa - pb));
            }
        }
    }
    int ToId(int x, int y) const {
        VASSERT(x >= 0 && x < N);
        VASSERT(y >= 0 && y < N);
        return ptToId_[y][x];
    }
    int ToId(const pint& p) const {
        return ToId(p.x, p.y);
    }
    const pint& ToPt(int id) const {
        return idToPt_[id];
    }

    bool IsIn(int x, int y) const {
        return x >= 0 && x < N && y >= 0 && y < N;
    }
    bool IsIn(const pint& p) const {
        return IsIn(p.x, p.y);
    }
    bool IsOut(int x, int y) const {
        return !IsIn(x, y);
    }
    bool IsOut(const pint& p) const {
        return IsOut(p.x, p.y);
    }
    int CalcManhattanDist(int a, int b) const {
        return manhattanDists_[a][b];
    }
    double CalcEuclidDist(int a, int b) const {
        return euclidDists_[a][b];
    }
};
GridSystem gs;

struct ArmShape {
    s8 parent_;
    s8 len_;               
    s8 depth_;             
    VArr<s8> children_;    
    VArr<s8> influences_;  
    int maxLeafLen_;  

    bool IsLeaf() const {
        return children_.empty();
    }
};

struct RobotShape {
    VArr<ArmShape> arms_;
    VArr<int> branchs_;
    VArr<int> leafs_;

    void Init() {
        arms_.resize(1);
        auto& a = arms_[0];
        a.parent_ = -1;
        a.len_ = -1;
        a.depth_ = 0;
        a.children_.clear();
        a.influences_.clear();
        a.maxLeafLen_ = 0;

        branchs_.clear();
        leafs_.clear();
    }

    int AddArm(s8 parent, s8 len) {
        int ai = arms_.size();
        auto& a = arms_.push();
        a.parent_ = parent;
        a.len_ = len;
        a.depth_ = arms_[parent].depth_ + 1;
        a.children_.clear();
        a.influences_.clear();
        a.maxLeafLen_ = 0;
        arms_[parent].children_.push(arms_.size() - 1);
        return ai;
    }

    void Finalize() {
        auto dfs = [&](auto&& dfs, int ai) -> void {
            arms_[ai].influences_.clear();
            arms_[ai].influences_.push(ai);

            for (int child : arms_[ai].children_) {
                dfs(dfs, child);
                arms_[ai].influences_.append(arms_[child].influences_);
                chmax(arms_[ai].maxLeafLen_, arms_[child].maxLeafLen_);
            }
            arms_[ai].maxLeafLen_ += arms_[ai].len_;
        };
        dfs(dfs, 0);

        branchs_.clear();
        leafs_.clear();
        FOR(ai, 1, V) {
            if (arms_[ai].children_.empty()) {
                leafs_.push(ai);
            }
            else {
                branchs_.push(ai);
            }
        }
    }

    void Disp(ostream& os) const {
        auto dfs = [&](auto&& dfs, int ai, int indent) -> void {
            REP(i, indent) {
                os << "  ";
            }
            if (ai == 0) {
                os << ai << endl;
            }
            else {
                os << ai << "(" << (int)arms_[ai].len_ << ") " << "leaflen: " << arms_[ai].maxLeafLen_ << endl;
            }
            for (int child : arms_[ai].children_) {
                dfs(dfs, child, indent + 1);
            }
        };
        dfs(dfs, 0, 0);
    }
};

struct IOArm {
    i8 parent_;
    i8 len_;

    void SetRoot() {
        parent_ = -1;
        len_ = -1;
    }

    void Output(ostream& os) const {
        os << (int)parent_ << " " << (int)len_ << endl;
    }
};

struct IOCommand {
    VArr<Dir> moves_;
    VArr<bool> puts_;

    void Output(ostream& os) const {
        REP(ai, moves_.size()) {
            os << DirToChar(moves_[ai]);
        }
        REP(ai, puts_.size()) {
            os << (puts_[ai] ? 'P' : '.');
        }
        os << endl;
    }
};

struct IOResult {
    int startCell_;
    VArr<IOArm> arms_;
    dvec<IOCommand> cs_;

    void Init(const RobotShape& shape, int rootCell) {
        startCell_ = rootCell;
        arms_.resize(shape.arms_.size());
        REP(ai, shape.arms_.size()) {
            arms_[ai].parent_ = shape.arms_[ai].parent_;
            arms_[ai].len_ = shape.arms_[ai].len_;
        }
        cs_.clear();
    }

    IOCommand& Push() {
        return cs_.pushR();
    }

    void Output(ostream& os) const {
        os << arms_.size() << endl;
        FOR(ai, 1, arms_.size()) {
            arms_[ai].Output(os);
        }
        auto p = gs.ToPt(startCell_);
        os << (int)p.y << " " << (int)p.x << endl;

        for (cauto& c : cs_) {
            c.Output(os);
        }
    }
};


struct IOServer {
    NNArr<bool> isTako_;
    NNArr<bool> isTarget_;

    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        is >> N;
        timer.Init();  
        is >> M;
        is >> V;
        NN = N * N;

        isTako_.resize(NN);
        isTarget_.resize(NN);

        char c;
        REP(i, NN) {
            is >> c;
            isTako_[i] = (c != '0');
        }
        REP(i, NN) {
            is >> c;
            isTarget_[i] = (c != '0');
        }

        gs.Init();
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
};

constexpr s8 tobotTbl[11][16][14][2] = {
{
{{ 0, 6 }, { 1, 4 }, { 1, 2 }, { 1, 3 }, },    
{{ 0, 5 }, { 1, 2 }, { 1, 4 }, { 1, 7 }, },    
{{ 0, 6 }, { 1, 4 }, { 1, 2 }, { 1, 3 }, },    
{{ 0, 7 }, { 1, 2 }, { 1, 5 }, { 1, 4 }, },    
{{ 0, 7 }, { 1, 3 }, { 1, 5 }, { 1, 4 }, },    
{{ 0, 7 }, { 1, 3 }, { 1, 5 }, { 1, 4 }, },    
{{ 0, 8 }, { 1, 6 }, { 1, 5 }, { 1, 4 }, },    
{{ 0, 8 }, { 1, 3 }, { 1, 6 }, { 1, 4 }, },    
{{ 0, 9 }, { 1, 5 }, { 1, 3 }, { 1, 6 }, },    
{{ 0, 9 }, { 1, 3 }, { 1, 5 }, { 1, 7 }, },    
{{ 0, 9 }, { 1, 3 }, { 1, 5 }, { 1, 7 }, },    
{{ 0, 10 }, { 1, 7 }, { 1, 6 }, { 1, 4 }, },    
{{ 0, 10 }, { 1, 7 }, { 1, 6 }, { 1, 4 }, },    
{{ 0, 10 }, { 1, 6 }, { 1, 3 }, { 1, 8 }, },    
{{ 0, 11 }, { 1, 7 }, { 1, 6 }, { 1, 5 }, },    
{{ 0, 11 }, { 1, 6 }, { 1, 3 }, { 1, 8 }, },    
},
{
{{ 0, 4 }, { 1, 6 }, { 2, 2 }, { 2, 4 }, { 2, 1 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 1 }, { 2, 4 }, { 2, 2 }, },    
{{ 0, 3 }, { 1, 5 }, { 2, 1 }, { 2, 4 }, { 2, 2 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 1 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 2 }, { 2, 4 }, { 2, 1 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 1 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 1 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 5 }, { 1, 7 }, { 2, 4 }, { 2, 1 }, { 2, 4 }, },    
{{ 0, 5 }, { 1, 7 }, { 2, 4 }, { 2, 1 }, { 2, 4 }, },    
{{ 0, 5 }, { 1, 7 }, { 2, 4 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 9 }, { 1, 4 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 9 }, { 1, 5 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 10 }, { 1, 6 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 9 }, { 1, 6 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 6 }, { 1, 10 }, { 2, 2 }, { 2, 5 }, { 2, 3 }, },    
{{ 0, 11 }, { 1, 6 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
},
{
{{ 0, 3 }, { 1, 4 }, { 2, 3 }, { 2, 1 }, { 2, 4 }, { 2, 2 }, },    
{{ 0, 3 }, { 1, 5 }, { 2, 3 }, { 2, 1 }, { 2, 4 }, { 2, 2 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 2 }, { 2, 1 }, { 2, 5 }, { 2, 3 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 3 }, { 2, 1 }, { 2, 2 }, { 2, 5 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 2 }, { 2, 1 }, { 2, 5 }, { 2, 3 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 1 }, { 2, 2 }, { 2, 5 }, { 2, 3 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 5 }, { 2, 2 }, { 2, 1 }, { 2, 3 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 5 }, { 2, 2 }, { 2, 1 }, { 2, 3 }, },    
{{ 0, 9 }, { 1, 5 }, { 2, 1 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 5 }, { 1, 9 }, { 2, 1 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 9 }, { 1, 6 }, { 2, 1 }, { 2, 2 }, { 2, 5 }, { 2, 3 }, },    
{{ 0, 5 }, { 1, 9 }, { 2, 1 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 9 }, { 1, 6 }, { 2, 1 }, { 2, 2 }, { 2, 5 }, { 2, 3 }, },    
{{ 0, 11 }, { 1, 6 }, { 2, 1 }, { 2, 2 }, { 2, 4 }, { 2, 3 }, },    
{{ 0, 11 }, { 1, 6 }, { 2, 1 }, { 2, 3 }, { 2, 2 }, { 2, 4 }, },    
{{ 0, 11 }, { 1, 6 }, { 2, 1 }, { 2, 7 }, { 2, 2 }, { 2, 3 }, },    
},
{
{{ 0, 2 }, { 1, 6 }, { 2, 4 }, { 2, 6 }, { 2, 3 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 2 }, { 1, 6 }, { 2, 4 }, { 2, 6 }, { 2, 3 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 5 }, { 1, 3 }, { 2, 6 }, { 2, 3 }, { 2, 4 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 4 }, { 2, 3 }, { 2, 5 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 4 }, { 2, 3 }, { 2, 6 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 4 }, { 2, 3 }, { 2, 5 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 4 }, { 2, 3 }, { 2, 5 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 4 }, { 1, 8 }, { 2, 4 }, { 2, 3 }, { 2, 5 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 7 }, { 1, 5 }, { 2, 4 }, { 2, 3 }, { 2, 5 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 8 }, { 3, 4 }, { 3, 3 }, { 3, 1 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 5 }, { 2, 4 }, { 2, 3 }, { 2, 5 }, { 2, 2 }, { 2, 1 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 8 }, { 3, 4 }, { 3, 3 }, { 3, 1 }, { 3, 2 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 8 }, { 3, 4 }, { 3, 3 }, { 3, 1 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 3, 4 }, { 3, 3 }, { 3, 1 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 3, 4 }, { 3, 3 }, { 3, 1 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 3, 4 }, { 3, 3 }, { 3, 1 }, { 3, 2 }, },    
},
{
{{ 0, 2 }, { 1, 5 }, { 2, 3 }, { 3, 5 }, { 3, 3 }, { 3, 2 }, { 3, 1 }, { 2, 3 }, },    
{{ 0, 2 }, { 1, 5 }, { 2, 3 }, { 3, 5 }, { 3, 3 }, { 3, 2 }, { 3, 1 }, { 2, 3 }, },    
{{ 0, 2 }, { 1, 6 }, { 2, 3 }, { 3, 5 }, { 3, 3 }, { 3, 2 }, { 3, 1 }, { 3, 4 }, },    
{{ 0, 2 }, { 1, 6 }, { 2, 3 }, { 3, 5 }, { 3, 3 }, { 3, 2 }, { 3, 1 }, { 3, 4 }, },    
{{ 0, 4 }, { 1, 3 }, { 2, 5 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 3, 5 }, { 3, 1 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 3 }, { 3, 5 }, { 3, 3 }, { 3, 2 }, { 3, 1 }, { 3, 4 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 3 }, { 3, 5 }, { 3, 3 }, { 3, 2 }, { 3, 1 }, { 3, 4 }, },    
{{ 0, 6 }, { 1, 3 }, { 2, 5 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 3, 5 }, { 3, 1 }, },    
{{ 0, 6 }, { 1, 3 }, { 2, 5 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 2, 2 }, { 3, 1 }, },    
{{ 0, 6 }, { 1, 3 }, { 2, 7 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 3, 5 }, { 3, 1 }, },    
{{ 0, 6 }, { 1, 3 }, { 2, 7 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 3, 5 }, { 3, 1 }, },    
{{ 0, 6 }, { 1, 3 }, { 2, 8 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 3, 5 }, { 3, 1 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 8 }, { 3, 4 }, { 3, 3 }, { 3, 1 }, { 3, 2 }, { 3, 5 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 3 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 3 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 3, 1 }, { 3, 6 }, { 3, 3 }, { 3, 2 }, { 3, 4 }, },    
},
{
{{ 0, 2 }, { 1, 4 }, { 2, 5 }, { 3, 1 }, { 2, 2 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 5 }, },    
{{ 0, 3 }, { 1, 5 }, { 2, 4 }, { 3, 1 }, { 2, 2 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 4 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 4 }, { 3, 1 }, { 2, 2 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 4 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 4 }, { 3, 1 }, { 2, 2 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 4 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 4 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 4 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 4 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 4 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 4 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 4 }, },    
{{ 0, 5 }, { 1, 7 }, { 2, 4 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 2, 3 }, { 3, 3 }, { 3, 2 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 4 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 4 }, },    
{{ 0, 5 }, { 1, 7 }, { 2, 4 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 3 }, { 2, 2 }, },    
{{ 0, 5 }, { 1, 7 }, { 2, 4 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 2, 3 }, { 3, 3 }, { 3, 2 }, },    
{{ 0, 4 }, { 1, 8 }, { 2, 6 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 6 }, { 3, 3 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 8 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 6 }, { 3, 3 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 8 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 6 }, { 3, 3 }, },    
{{ 0, 4 }, { 1, 9 }, { 2, 7 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 6 }, { 3, 3 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 9 }, { 3, 1 }, { 3, 5 }, { 3, 4 }, { 3, 2 }, { 3, 7 }, { 3, 3 }, },    
},
{
{{ 0, 3 }, { 1, 5 }, { 2, 1 }, { 3, 4 }, { 4, 3 }, { 4, 4 }, { 2, 1 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 3 }, { 1, 5 }, { 2, 1 }, { 3, 4 }, { 4, 3 }, { 4, 4 }, { 2, 2 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 3 }, { 1, 5 }, { 2, 1 }, { 3, 4 }, { 4, 3 }, { 4, 4 }, { 2, 1 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 5 }, { 1, 4 }, { 2, 1 }, { 3, 4 }, { 4, 3 }, { 4, 4 }, { 2, 2 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 3 }, { 1, 5 }, { 2, 1 }, { 3, 4 }, { 4, 3 }, { 4, 4 }, { 2, 1 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 5 }, { 1, 3 }, { 2, 1 }, { 3, 4 }, { 4, 3 }, { 4, 4 }, { 2, 2 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 4 }, { 1, 2 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 3, 4 }, { 4, 5 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 6 }, { 1, 2 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 3, 4 }, { 4, 4 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 6 }, { 1, 2 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 3, 4 }, { 4, 4 }, { 4, 1 }, { 2, 3 }, { 4, 2 }, },    
{{ 0, 7 }, { 1, 3 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 3, 4 }, { 4, 4 }, { 4, 1 }, { 2, 6 }, { 4, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 3, 4 }, { 4, 4 }, { 4, 1 }, { 2, 6 }, { 4, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 3, 4 }, { 4, 4 }, { 4, 1 }, { 2, 6 }, { 4, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 3, 4 }, { 4, 4 }, { 4, 1 }, { 2, 4 }, { 4, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 3, 4 }, { 4, 4 }, { 4, 1 }, { 2, 6 }, { 4, 2 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 7 }, { 3, 8 }, { 4, 5 }, { 4, 4 }, { 4, 3 }, { 4, 1 }, { 4, 2 }, { 4, 6 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 7 }, { 2, 9 }, { 3, 8 }, { 5, 5 }, { 5, 4 }, { 5, 3 }, { 5, 1 }, { 5, 2 }, },    
},
{
{{ 0, 5 }, { 1, 2 }, { 2, 3 }, { 3, 3 }, { 4, 2 }, { 3, 2 }, { 4, 3 }, { 4, 1 }, { 3, 3 }, { 4, 4 }, { 2, 2 }, },    
{{ 0, 4 }, { 1, 2 }, { 2, 3 }, { 3, 3 }, { 4, 2 }, { 3, 2 }, { 4, 3 }, { 4, 1 }, { 3, 3 }, { 4, 4 }, { 1, 4 }, },    
{{ 0, 3 }, { 1, 2 }, { 2, 5 }, { 3, 3 }, { 4, 2 }, { 3, 2 }, { 4, 3 }, { 2, 3 }, { 3, 3 }, { 4, 4 }, { 4, 1 }, },    
{{ 0, 3 }, { 1, 2 }, { 2, 5 }, { 3, 3 }, { 4, 2 }, { 3, 2 }, { 4, 3 }, { 2, 3 }, { 3, 3 }, { 4, 4 }, { 4, 1 }, },    
{{ 0, 3 }, { 1, 2 }, { 2, 5 }, { 3, 3 }, { 4, 2 }, { 3, 2 }, { 4, 3 }, { 3, 1 }, { 3, 3 }, { 4, 4 }, { 4, 1 }, },    
{{ 0, 6 }, { 1, 2 }, { 2, 5 }, { 3, 3 }, { 4, 2 }, { 3, 2 }, { 4, 3 }, { 2, 3 }, { 3, 3 }, { 4, 4 }, { 4, 1 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 2 }, { 3, 4 }, { 3, 4 }, { 4, 3 }, { 4, 1 }, { 4, 4 }, { 4, 2 }, { 3, 2 }, { 4, 5 }, },    
{{ 0, 2 }, { 1, 6 }, { 2, 5 }, { 3, 4 }, { 3, 3 }, { 5, 2 }, { 5, 1 }, { 5, 3 }, { 5, 5 }, { 3, 3 }, { 5, 4 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 2 }, { 3, 4 }, { 3, 4 }, { 4, 3 }, { 4, 1 }, { 4, 4 }, { 4, 2 }, { 3, 2 }, { 4, 5 }, },    
{{ 0, 4 }, { 1, 8 }, { 2, 6 }, { 3, 4 }, { 3, 3 }, { 5, 2 }, { 3, 2 }, { 5, 3 }, { 5, 5 }, { 3, 3 }, { 5, 4 }, },    
{{ 0, 4 }, { 1, 8 }, { 2, 5 }, { 3, 4 }, { 3, 3 }, { 5, 2 }, { 3, 2 }, { 5, 3 }, { 5, 5 }, { 3, 3 }, { 5, 4 }, },    
{{ 0, 4 }, { 1, 6 }, { 2, 5 }, { 3, 4 }, { 3, 3 }, { 5, 2 }, { 5, 1 }, { 5, 3 }, { 5, 5 }, { 3, 3 }, { 5, 4 }, },    
{{ 0, 4 }, { 1, 8 }, { 2, 6 }, { 3, 4 }, { 3, 3 }, { 5, 2 }, { 3, 2 }, { 5, 3 }, { 5, 5 }, { 3, 3 }, { 5, 4 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 7 }, { 3, 4 }, { 3, 4 }, { 4, 3 }, { 4, 1 }, { 4, 4 }, { 4, 2 }, { 3, 2 }, { 4, 5 }, },    
{{ 0, 6 }, { 1, 9 }, { 2, 5 }, { 3, 4 }, { 3, 3 }, { 5, 2 }, { 3, 2 }, { 5, 3 }, { 5, 5 }, { 3, 3 }, { 5, 4 }, },    
{{ 0, 6 }, { 1, 9 }, { 2, 5 }, { 3, 4 }, { 3, 3 }, { 5, 2 }, { 3, 2 }, { 5, 3 }, { 5, 5 }, { 3, 3 }, { 5, 4 }, },    
},
{
{{ 0, 2 }, { 1, 4 }, { 2, 2 }, { 3, 3 }, { 4, 3 }, { 2, 5 }, { 3, 4 }, { 4, 2 }, { 6, 1 }, { 4, 1 }, { 6, 2 }, { 6, 4 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 2 }, { 3, 3 }, { 4, 3 }, { 2, 3 }, { 3, 2 }, { 4, 2 }, { 6, 1 }, { 4, 1 }, { 6, 3 }, { 6, 4 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 5 }, { 3, 3 }, { 4, 3 }, { 2, 3 }, { 3, 2 }, { 4, 2 }, { 6, 1 }, { 4, 1 }, { 6, 2 }, { 6, 4 }, },    
{{ 0, 3 }, { 1, 4 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 2, 7 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 3, 6 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 4 }, { 1, 7 }, { 2, 2 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 3 }, { 1, 6 }, { 2, 8 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 3, 2 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 10 }, { 1, 5 }, { 2, 3 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 6, 6 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 10 }, { 1, 5 }, { 2, 3 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 6, 6 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
{{ 0, 10 }, { 1, 5 }, { 2, 3 }, { 3, 4 }, { 3, 3 }, { 3, 4 }, { 6, 6 }, { 6, 4 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 6, 5 }, },    
},
{
{{ 0, 2 }, { 1, 5 }, { 2, 2 }, { 3, 2 }, { 2, 7 }, { 3, 1 }, { 6, 3 }, { 4, 3 }, { 4, 4 }, { 6, 1 }, { 6, 4 }, { 6, 2 }, { 4, 2 }, },    
{{ 0, 2 }, { 1, 5 }, { 2, 2 }, { 3, 2 }, { 2, 3 }, { 3, 1 }, { 6, 3 }, { 4, 3 }, { 4, 4 }, { 4, 1 }, { 6, 4 }, { 6, 2 }, { 4, 2 }, },    
{{ 0, 2 }, { 1, 5 }, { 2, 2 }, { 3, 2 }, { 2, 4 }, { 3, 1 }, { 6, 3 }, { 4, 3 }, { 4, 4 }, { 4, 1 }, { 6, 4 }, { 6, 2 }, { 4, 2 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 5 }, { 2, 2 }, { 3, 2 }, { 3, 1 }, { 6, 3 }, { 5, 3 }, { 5, 2 }, { 5, 1 }, { 6, 4 }, { 6, 5 }, { 6, 1 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 5 }, { 3, 3 }, { 4, 5 }, { 3, 2 }, { 6, 3 }, { 4, 2 }, { 4, 1 }, { 6, 1 }, { 2, 3 }, { 6, 2 }, { 4, 3 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 5 }, { 3, 2 }, { 3, 4 }, { 5, 5 }, { 3, 1 }, { 7, 3 }, { 5, 3 }, { 5, 4 }, { 5, 1 }, { 7, 4 }, { 7, 2 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 5 }, { 3, 1 }, { 4, 2 }, { 3, 6 }, { 6, 3 }, { 6, 1 }, { 4, 3 }, { 4, 1 }, { 6, 5 }, { 6, 2 }, { 4, 5 }, },    
{{ 0, 2 }, { 1, 8 }, { 2, 5 }, { 3, 3 }, { 3, 3 }, { 5, 2 }, { 3, 4 }, { 7, 3 }, { 5, 3 }, { 5, 1 }, { 7, 1 }, { 7, 5 }, { 7, 2 }, },    
{{ 0, 2 }, { 1, 8 }, { 2, 5 }, { 3, 3 }, { 3, 3 }, { 5, 2 }, { 3, 4 }, { 7, 3 }, { 5, 3 }, { 5, 1 }, { 7, 1 }, { 7, 5 }, { 7, 2 }, },    
{{ 0, 2 }, { 1, 8 }, { 2, 5 }, { 3, 3 }, { 3, 3 }, { 5, 5 }, { 3, 4 }, { 7, 3 }, { 5, 2 }, { 5, 1 }, { 5, 3 }, { 7, 5 }, { 7, 2 }, },    
{{ 0, 2 }, { 1, 8 }, { 2, 5 }, { 3, 3 }, { 3, 3 }, { 5, 5 }, { 3, 4 }, { 7, 3 }, { 5, 2 }, { 5, 1 }, { 5, 3 }, { 7, 5 }, { 7, 2 }, },    
{{ 0, 2 }, { 1, 8 }, { 2, 5 }, { 3, 3 }, { 3, 3 }, { 5, 5 }, { 3, 4 }, { 7, 3 }, { 5, 2 }, { 5, 1 }, { 5, 3 }, { 7, 5 }, { 7, 2 }, },    
{{ 0, 2 }, { 1, 5 }, { 2, 9 }, { 3, 3 }, { 3, 6 }, { 5, 5 }, { 5, 1 }, { 4, 3 }, { 5, 3 }, { 5, 2 }, { 5, 4 }, { 4, 2 }, { 4, 1 }, },    
{{ 0, 2 }, { 1, 5 }, { 2, 9 }, { 3, 3 }, { 3, 6 }, { 5, 5 }, { 5, 1 }, { 4, 3 }, { 5, 3 }, { 5, 2 }, { 5, 4 }, { 4, 2 }, { 4, 1 }, },    
{{ 0, 2 }, { 1, 5 }, { 2, 9 }, { 3, 3 }, { 3, 6 }, { 5, 5 }, { 5, 1 }, { 4, 3 }, { 5, 3 }, { 5, 2 }, { 5, 4 }, { 4, 2 }, { 4, 1 }, },    
{{ 0, 2 }, { 1, 5 }, { 2, 10 }, { 3, 3 }, { 3, 6 }, { 5, 5 }, { 5, 1 }, { 4, 3 }, { 5, 3 }, { 5, 2 }, { 5, 4 }, { 4, 2 }, { 4, 1 }, },    
},
{
{{ 0, 2 }, { 1, 4 }, { 2, 3 }, { 3, 1 }, { 4, 1 }, { 2, 1 }, { 6, 5 }, { 4, 5 }, { 4, 3 }, { 4, 2 }, { 4, 4 }, { 6, 2 }, { 6, 3 }, { 3, 4 }, },    
{{ 0, 2 }, { 1, 4 }, { 2, 3 }, { 3, 1 }, { 4, 1 }, { 2, 1 }, { 6, 7 }, { 4, 5 }, { 4, 3 }, { 4, 2 }, { 4, 4 }, { 6, 2 }, { 6, 4 }, { 6, 5 }, },    
{{ 0, 5 }, { 1, 3 }, { 2, 3 }, { 3, 1 }, { 4, 1 }, { 2, 1 }, { 6, 3 }, { 4, 5 }, { 4, 3 }, { 4, 2 }, { 4, 4 }, { 6, 2 }, { 6, 4 }, { 6, 5 }, },    
{{ 0, 5 }, { 1, 3 }, { 2, 3 }, { 3, 1 }, { 4, 1 }, { 2, 1 }, { 6, 3 }, { 4, 5 }, { 4, 3 }, { 4, 2 }, { 4, 4 }, { 6, 2 }, { 6, 4 }, { 6, 5 }, },    
{{ 0, 5 }, { 1, 3 }, { 2, 3 }, { 3, 1 }, { 4, 1 }, { 2, 1 }, { 6, 3 }, { 4, 5 }, { 4, 3 }, { 4, 2 }, { 4, 4 }, { 6, 2 }, { 6, 4 }, { 6, 5 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 3 }, { 3, 1 }, { 4, 1 }, { 2, 1 }, { 6, 3 }, { 4, 5 }, { 4, 3 }, { 4, 2 }, { 4, 4 }, { 6, 2 }, { 6, 1 }, { 6, 5 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 3 }, { 3, 1 }, { 4, 6 }, { 2, 1 }, { 6, 3 }, { 4, 5 }, { 4, 3 }, { 4, 2 }, { 4, 4 }, { 6, 2 }, { 6, 5 }, { 6, 4 }, },    
{{ 0, 6 }, { 1, 4 }, { 2, 6 }, { 3, 1 }, { 4, 1 }, { 2, 1 }, { 6, 3 }, { 4, 5 }, { 4, 3 }, { 4, 2 }, { 4, 4 }, { 6, 2 }, { 6, 4 }, { 6, 5 }, },    
{{ 0, 6 }, { 1, 5 }, { 2, 3 }, { 3, 1 }, { 4, 6 }, { 2, 2 }, { 6, 3 }, { 4, 5 }, { 4, 3 }, { 6, 5 }, { 4, 4 }, { 6, 2 }, { 6, 1 }, { 6, 4 }, },    
{{ 0, 6 }, { 1, 5 }, { 2, 3 }, { 3, 1 }, { 4, 6 }, { 2, 2 }, { 6, 3 }, { 4, 5 }, { 4, 3 }, { 6, 5 }, { 4, 4 }, { 6, 2 }, { 6, 1 }, { 6, 4 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 0, 7 }, { 4, 4 }, { 5, 6 }, { 3, 4 }, { 3, 1 }, { 6, 4 }, { 3, 3 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 0, 3 }, { 4, 4 }, { 5, 8 }, { 3, 4 }, { 3, 1 }, { 6, 4 }, { 3, 3 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 0, 8 }, { 4, 4 }, { 5, 6 }, { 3, 4 }, { 3, 1 }, { 6, 4 }, { 3, 3 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 0, 7 }, { 4, 4 }, { 5, 6 }, { 3, 4 }, { 3, 5 }, { 6, 4 }, { 3, 3 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 4 }, { 2, 9 }, { 0, 7 }, { 4, 4 }, { 5, 6 }, { 3, 4 }, { 3, 5 }, { 6, 4 }, { 3, 3 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 3, 2 }, },    
{{ 0, 7 }, { 1, 5 }, { 2, 9 }, { 0, 6 }, { 4, 4 }, { 5, 7 }, { 3, 4 }, { 3, 6 }, { 6, 4 }, { 3, 3 }, { 6, 3 }, { 6, 1 }, { 6, 2 }, { 3, 1 }, },    
},
};
void GetTblRobot(RobotShape& shape) {
    shape.Init();

    cauto& tbl = tobotTbl[V - 5][N - 15];
    REP(i, V - 1) {
        shape.AddArm(tbl[i][0], tbl[i][1]);
    }

    shape.Finalize();
}

struct Zobrist {
    NNArr<u64> takoHash_;
    NNArr<u64> rootHash_;
    VArr<array<u64, 4>> armsRotHash_;
    VArr<u64> armsHasHash_;

    void Init() {
        takoHash_.resize(NN);
        rootHash_.resize(NN);
        REP(i, NN) {
            takoHash_[i] = rand_.Get64();
            rootHash_[i] = rand_.Get64();
        }
        armsRotHash_.resize(V);
        armsHasHash_.resize(V);
        FOR(ai, 1, V) {
            REP(rot, 4) {
                armsRotHash_[ai][rot] = rand_.Get64();
            }
            armsHasHash_[ai] = rand_.Get64();
        }
    }
} zobrist_;

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


template <class OBJECT>
struct ObjectPool {
    static_assert(is_trivially_copyable<OBJECT>::value, "not trivially copyable");  

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

    inline OBJECT* New() {
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
    double GetMemoryRate() const {
        return usedTotalCount_ / (double)capacity_;
    }
};

constexpr array<double, 5> TakoDimCostTbl = {4, 3, 2, 1, 0};
constexpr double TargetCostScale = 1;
constexpr array<double, 5> TargetDimCostTbl = {4, 3, 2, 1, 0};


CapArr<RobotShape, 32> shapes_;

constexpr array<pint, 4> absRotToVec = {
    pint{1, 0},
    pint{0, 1},
    pint{-1, 0},
    pint{0, -1},
};

enum class RotId : u8 {
    Invalid = 0,
    N = 1,
    L = 2,
    R = 3,
};

struct MoveBit {
    u32 bit_;

    void Init() {
        bit_ = 0;
    }
    void SetRootDir(Dir dir) {
        bit_ &= ~0b111;
        bit_ |= (u32)dir;
    }
    Dir GetRootDir() const {
        return Dir(bit_ & 0b111);
    }
    void SetRotId(int ai, RotId rotId) {
        VASSERT(ai > 0);
        int shift = ((ai << 1) + 1);
        bit_ &= ~(0b11 << shift);
        bit_ |= ((u32)rotId) << shift;
    }
    RotId GetRotId(int ai) const {
        VASSERT(ai > 0);
        int shift = ((ai << 1) + 1);
        return RotId((bit_ >> shift) & 0b11);
    }
    static u32 GetRotMask(int ai) {
        VASSERT(ai > 0);
        return 0b11 << ((ai << 1) + 1);
    }
};
static_assert(sizeof(MoveBit) == 4);

struct TargetCell {
    i16 cell_;
    i16 ai_;
};
constexpr int TargetCellSize = sizeof(TargetCell);

template <int NNCap>
struct BeamState {
    bitset<NNCap> isTako_;  
    bitset<NNCap> isTarget_;
    int startCell_;
    int rootCell_;
    u32 rotBit_;          
    u32 hasBit_;          
    int takoLeft_;        
    int targetLeft_;      
    u64 hash_;            
    double takoScore_;    
    double targetScore_;  
    int ai_;                  
    int shapeId_;             
    CapArr<s8, NNCap> dims_;  
    double dimCost_;

    u32 GetRot(int ai) const {
        return (rotBit_ >> (ai << 1)) & 0b11;
    }
    void SetRot(int ai, u32 rot) {
        rotBit_ &= ~(0b11 << (ai << 1));
        rotBit_ |= (rot << (ai << 1));
    }
    u32 GetHas(int ai) const {
        return (hasBit_ >> ai) & 1;
    }
    void SetHas(int ai, u32 has) {
        hasBit_ &= ~(1 << ai);
        hasBit_ |= (has << ai);
    }

    void Init(int shapeId, int rootCell) {
        isTako_.reset();
        isTarget_.reset();
        takoLeft_ = 0;
        targetLeft_ = 0;
        startCell_ = rootCell;
        rootCell_ = rootCell;
        rotBit_ = 0;
        hasBit_ = 0;
        hash_ = 0;
        takoScore_ = 0;
        targetScore_ = 0;
        ai_ = -1;
        shapeId_ = shapeId;
        dimCost_ = 0;


        hash_ ^= zobrist_.rootHash_[rootCell_];

        FOR(ai, 1, V) {
            hash_ ^= zobrist_.armsRotHash_[ai][0];  
        }

        REP(i, NN) {
            if (server.isTako_[i] && server.isTarget_[i]) {
            }
            else if (server.isTako_[i]) {
                isTako_.set(i);
                hash_ ^= zobrist_.takoHash_[i];
                ++takoLeft_;
            }
            else if (server.isTarget_[i]) {
                isTarget_.set(i);
                hash_ ^= zobrist_.takoHash_[i];  
                ++targetLeft_;
            }
        }

        dims_.assign(NN, 0);
        REP(i, NN) {
            if (isTako_[i]) {
                for (int n : gs.aroundMap_[i]) {
                    if (isTako_[n]) {
                        ++dims_[i];
                    }
                }
                dimCost_ += TakoDimCostTbl[dims_[i]];
            }
            else if (isTarget_[i]) {
                for (int n : gs.aroundMap_[i]) {
                    if (isTarget_[n]) {
                        ++dims_[i];
                    }
                }
                dimCost_ += TargetDimCostTbl[dims_[i]];
            }
        }
    }

    double CalcEval(double appendEvalScore) const {
        double eval = 0;
        eval -= targetLeft_ * HP.TargetCountCost;  
        eval -= takoLeft_ * HP.TakoCountCost;      

        static int centerCell = gs.ToId(N / 2, N / 2);

        eval -= gs.CalcManhattanDist(rootCell_, centerCell) * HP.RootPosCost;

        eval += takoScore_ * HP.TakoPosCost;
        eval += targetScore_ * HP.TargetPosCost;

        eval += appendEvalScore;

        eval -= dimCost_ * HP.DimCostRate;

        return eval;
    }

    void CalcMovedEval(const MoveBit& moveBit, const VArr<TargetCell>& targetCells, double appendEvalScore,
                       double& eval, u64& nextHash, bool& end) const {
        static int centerCell = gs.ToId(N / 2, N / 2);

        int nextTargetLeft = targetLeft_;
        int nextTakoLeft = takoLeft_;
        int nextRootCell = rootCell_;
        double nextTakoScore = takoScore_;
        double nextTargetScore = targetScore_;
        double nextDimCost = dimCost_;
        nextHash = hash_;
        static NNArr<s8> dimsDiff;  
        dimsDiff.assign(NN, 0);
        static bitset<NNCap> removed;
        removed.reset();

        {
            cauto& shape = shapes_[shapeId_];

            {
                Dir dir = moveBit.GetRootDir();
                if (dir != Dir::N) {
                    nextHash ^= zobrist_.rootHash_[rootCell_];
                    nextRootCell = gs.aroundDir_[rootCell_][(int)dir];
                    nextHash ^= zobrist_.rootHash_[nextRootCell];
                    VASSERT(nextRootCell >= 0);
                }
            }
            FOR(ai, 1, V) {
                RotId rotId = moveBit.GetRotId(ai);
                if (rotId == RotId::N) {
                }
                else if (rotId == RotId::L) {
                    int rot = GetRot(ai);
                    nextHash ^= zobrist_.armsRotHash_[ai][rot];
                    int nextRot = (rot + 3) & 0b11;
                    nextHash ^= zobrist_.armsRotHash_[ai][nextRot];
                }
                else if (rotId == RotId::R) {
                    int rot = GetRot(ai);
                    nextHash ^= zobrist_.armsRotHash_[ai][rot];
                    int nextRot = (rot + 1) & 0b11;
                    nextHash ^= zobrist_.armsRotHash_[ai][nextRot];
                }
                else {
                    VABORT();
                }
            }
            for (cauto& tc : targetCells) {
                int ai = tc.ai_;
                VASSERT(shape.arms_[ai].IsLeaf());

                int cell = tc.cell_;
                if (GetHas(ai)) {
                    VASSERT(!isTako_[cell]);
                    VASSERT(isTarget_[cell]);

                    removed.set(cell);
                    nextHash ^= zobrist_.takoHash_[cell];
                    --nextTargetLeft;

                    nextHash ^= zobrist_.armsHasHash_[ai];
                    nextTargetScore += gs.CalcEuclidDist(cell, centerCell);

                    nextDimCost -= TargetDimCostTbl[dims_[cell] + dimsDiff[cell]];
                    for (int n : gs.aroundMap_[cell]) {
                        if (isTarget_[n] && !removed.test(n)) {
                            nextDimCost -= TargetDimCostTbl[dims_[n] + dimsDiff[n]];
                            --dimsDiff[n];
                            nextDimCost += TargetDimCostTbl[dims_[n] + dimsDiff[n]];
                        }
                    }
                }
                else {
                    VASSERT(isTako_[cell]);
                    VASSERT(!isTarget_[cell]);

                    removed.set(cell);
                    nextHash ^= zobrist_.takoHash_[cell];
                    --nextTakoLeft;

                    nextHash ^= zobrist_.armsHasHash_[ai];
                    nextTakoScore += gs.CalcEuclidDist(cell, centerCell);

                    nextDimCost -= TakoDimCostTbl[dims_[cell] + dimsDiff[cell]];
                    for (int n : gs.aroundMap_[cell]) {
                        if (isTako_[n] && !removed.test(n)) {  
                            nextDimCost -= TakoDimCostTbl[dims_[n] + dimsDiff[n]];
                            --dimsDiff[n];
                            nextDimCost += TakoDimCostTbl[dims_[n] + dimsDiff[n]];
                        }
                    }
                }
            }
        }

        eval = 0;
        eval -= nextTargetLeft * HP.TargetCountCost;  
        eval -= nextTakoLeft * HP.TakoCountCost;      

        eval -= gs.CalcManhattanDist(nextRootCell, centerCell) * HP.RootPosCost;

        eval += nextTakoScore * HP.TakoPosCost;
        eval += nextTargetScore * HP.TargetPosCost;

        eval += appendEvalScore;

        eval -= nextDimCost * HP.DimCostRate;

        end = nextTargetLeft == 0;
    }

    void Move(const MoveBit& moveBit, const VArr<TargetCell>& targetCells) {
        cauto& shape = shapes_[shapeId_];
        {
            Dir dir = moveBit.GetRootDir();
            if (dir != Dir::N) {
                hash_ ^= zobrist_.rootHash_[rootCell_];
                rootCell_ = gs.aroundDir_[rootCell_][(int)dir];
                hash_ ^= zobrist_.rootHash_[rootCell_];
                VASSERT(rootCell_ >= 0);
            }
        }
        FOR(ai, 1, V) {
            RotId rotId = moveBit.GetRotId(ai);
            if (rotId == RotId::N) {
            }
            else if (rotId == RotId::L) {
                int rot = GetRot(ai);
                hash_ ^= zobrist_.armsRotHash_[ai][rot];
                rot = (rot + 3) & 0b11;
                hash_ ^= zobrist_.armsRotHash_[ai][rot];
                SetRot(ai, rot);
            }
            else if (rotId == RotId::R) {
                int rot = GetRot(ai);
                hash_ ^= zobrist_.armsRotHash_[ai][rot];
                rot = (rot + 1) & 0b11;
                hash_ ^= zobrist_.armsRotHash_[ai][rot];
                SetRot(ai, rot);
            }
            else {
                VABORT();
            }
        }

        static int centerCell = gs.ToId(N / 2, N / 2);

        for (cauto& tc : targetCells) {
            int ai = tc.ai_;
            VASSERT(shape.arms_[ai].IsLeaf());

            int cell = tc.cell_;
            if (GetHas(ai)) {
                VASSERT(!isTako_[cell]);
                VASSERT(isTarget_[cell]);

                isTarget_.reset(cell);
                hash_ ^= zobrist_.takoHash_[cell];
                --targetLeft_;

                SetHas(ai, 0);
                hash_ ^= zobrist_.armsHasHash_[ai];
                targetScore_ += gs.CalcEuclidDist(cell, centerCell);

                dimCost_ -= TargetDimCostTbl[dims_[cell]];
                for (int n : gs.aroundMap_[cell]) {
                    if (isTarget_[n]) {
                        dimCost_ -= TargetDimCostTbl[dims_[n]];
                        --dims_[n];
                        VASSERT(dims_[n] >= 0);
                        dimCost_ += TargetDimCostTbl[dims_[n]];
                    }
                }
            }
            else {
                VASSERT(isTako_[cell]);
                VASSERT(!isTarget_[cell]);

                isTako_.reset(cell);
                hash_ ^= zobrist_.takoHash_[cell];
                --takoLeft_;

                SetHas(ai, 1);
                hash_ ^= zobrist_.armsHasHash_[ai];
                takoScore_ += gs.CalcEuclidDist(cell, centerCell);

                dimCost_ -= TakoDimCostTbl[dims_[cell]];
                for (int n : gs.aroundMap_[cell]) {
                    if (isTako_[n]) {
                        dimCost_ -= TakoDimCostTbl[dims_[n]];
                        --dims_[n];
                        VASSERT(dims_[n] >= 0);
                        dimCost_ += TakoDimCostTbl[dims_[n]];
                    }
                }
            }
        }
    }

    void CalcPos(const RobotShape& shape, VArr<pint>& poss) const {
        poss.resize(shape.arms_.size());

        auto dfs = [&](auto&& dfs, int ai, const pint& parentPos, int parentAbsRot) -> void {
            int absRot = (parentAbsRot + GetRot(ai)) & 0b11;
            pint pos = parentPos + absRotToVec[absRot] * shape.arms_[ai].len_;
            poss[ai] = pos;

            for (int ni : shape.arms_[ai].children_) {
                dfs(dfs, ni, pos, absRot);
            }
        };

        cauto& rootPos = gs.ToPt(rootCell_);
        poss[0] = rootPos;

        for (int ni : shape.arms_[0].children_) {
            dfs(dfs, ni, rootPos, 0);
        }
    }

    bool IsTargetCell(bool has, int cell) const {
        if (has) {
            return isTarget_[cell];
        }
        else {
            return isTako_[cell];
        }
    }
};
constexpr int BeamStateSize15 = sizeof(BeamState<15 * 15>);
constexpr int BeamStateSize30 = sizeof(BeamState<30 * 30>);


struct Candidate {
    MoveBit moveBit_;               
    VArr<TargetCell> targetCells_;  
    double score_;                  
};
struct CandidateCmp {
    bool operator()(const Candidate& a, const Candidate& b) const {
        return a.score_ < b.score_;
    }
};
struct CandidateHolder {
    SizeFilter<Candidate> candidates_;

    void Init(int candidateCount) {
        candidates_.Init(candidateCount);
    }

    template <int NNCap>
    void Push(const BeamState<NNCap>& startState, const MoveBit& moveBit, int targetAi, int targetCell,
              double rootPosScore) {
        static CandidateCmp cmp;
        static int centerCell = gs.ToId(N / 2, N / 2);

        double score = 0;
        if (startState.GetHas(targetAi)) {
            score += HP.CandidateTargetScore;
            score += gs.CalcEuclidDist(targetCell, centerCell) * HP.CandidateTargetPosCost;
        }
        else {
            score += HP.CandidateTakoScore;
            score += gs.CalcEuclidDist(targetCell, centerCell) * HP.CandidateTakoPosCost;
        }

        score += rootPosScore;

        Candidate* ptr = candidates_.TryPush(score);
        if (ptr) {
            auto& c = *ptr;
            c.moveBit_ = moveBit;
            c.targetCells_.clear();
            auto& tc = c.targetCells_.push();
            tc.ai_ = targetAi;
            tc.cell_ = targetCell;
            c.score_ = score;
        }
    }

    template <int NNCap>
    void Push(const BeamState<NNCap>& startState, const MoveBit& moveBit, const VArr<TargetCell>& targetCells,
              double rootPosScore) {
        static CandidateCmp cmp;
        static int centerCell = gs.ToId(N / 2, N / 2);

        double score = 0;
        for (cauto& tc : targetCells) {
            if (startState.GetHas(tc.ai_)) {
                score += HP.CandidateTargetScore;
                score += gs.CalcEuclidDist(tc.cell_, centerCell) * HP.CandidateTargetPosCost;
            }
            else {
                score += HP.CandidateTakoScore;
                score += gs.CalcEuclidDist(tc.cell_, centerCell) * HP.CandidateTakoPosCost;
            }
        }

        score += rootPosScore;

        Candidate* ptr = candidates_.TryPush(score);
        if (ptr) {
            auto& c = *ptr;
            c.moveBit_ = moveBit;
            c.targetCells_ = targetCells;
            c.score_ = score;
        }
    }

};

template <int NNCap>
void MakeCandidateFewBranch(const BeamState<NNCap>& startState, CandidateHolder& holder) {
    constexpr array<pr<RotId, int>, 3> rotIdRotTbl = {
        pr<RotId, int>{RotId::N, 0},  
        pr<RotId, int>{RotId::L, 3},  
        pr<RotId, int>{RotId::R, 1},  
    };

    cauto& shape = shapes_[startState.shapeId_];
    cauto& branchs = shape.branchs_;
    cauto& leafs = shape.leafs_;

    MoveBit moveBit;
    moveBit.Init();

    VArr<u8> armsRot(V);
    VArr<bool> armsHas(V);
    FOR(ai, 1, V) {
        armsRot[ai] = (u8)startState.GetRot(ai);
        armsHas[ai] = startState.GetHas(ai);
    }
    double rootPosScore = 0;

    VArr<pint> poss(V);
    VArr<int> absRots(V);

    holder.Init(HP.CandidateHoldCount);

    static VArr<bitset<NNCap>> armsTargets(V);
    FOR(ai, 1, V) {
        if (shape.arms_[ai].IsLeaf()) {
            if (armsHas[ai]) {
                armsTargets[ai] = startState.isTarget_;
            }
            else {
                armsTargets[ai] = startState.isTako_;
            }
        }
    }

    int bestTargetCount = 0;

    auto MakeHold = [&]() {
        struct LeafTarget {
            RotId rotId_;
            int cell_;
        };
        static VArr<CapArr<LeafTarget, 3>> leafsTargets(leafs.size());

        int targetCount = 0;
        REP(li, leafs.size()) {
            leafsTargets[li].clear();

            int ai = leafs[li];
            int len = shape.arms_[ai].len_;
            int pi = shape.arms_[ai].parent_;

            int parentAbsRot = absRots[pi];
            const pint& parentPos = poss[pi];

            cauto& targetBit = armsTargets[ai];
            for (auto [rotId, rot] : rotIdRotTbl) {
                int absRot = (parentAbsRot + armsRot[ai] + rot) & 0b11;
                pint pos = parentPos + absRotToVec[absRot] * len;

                if (gs.IsIn(pos)) {
                    int cell = gs.ToId(pos);
                    if (targetBit.test(cell)) {
                        auto& t = leafsTargets[li].push();
                        t.rotId_ = rotId;
                        t.cell_ = cell;
                    }
                }
            }

            if (!leafsTargets[li].empty()) {
                ++targetCount;
            }
        }

        if (targetCount == 0) {
            return;
        }
        if (targetCount < bestTargetCount - 1) {
            return;
        }


        static VArr<TargetCell> targetCells;
        targetCells.clear();
        static CheckMapD used;
        used.Init(NN);
        auto leafDfs = [&](auto&& leafDfs, int li) -> void {
            if (li >= leafs.size()) {
                if (!targetCells.empty()) {
                    holder.Push(startState, moveBit, targetCells, rootPosScore);
                    chmax(bestTargetCount, targetCells.size());
                }
                return;
            }

            int ai = leafs[li];
            cauto& targets = leafsTargets[li];
            bool added = false;
            for (cauto& target : targets) {
                if (used.IsChecked(target.cell_)) {
                    continue;
                }
                used.Check(target.cell_);

                moveBit.SetRotId(ai, target.rotId_);
                auto& tc = targetCells.push();
                tc.ai_ = ai;
                tc.cell_ = target.cell_;

                leafDfs(leafDfs, li + 1);
                added = true;

                targetCells.pop();
                used.Reset(target.cell_);
            }
            if (!added) {
                moveBit.SetRotId(ai, RotId::Invalid);
                leafDfs(leafDfs, li + 1);
            }
        };
        leafDfs(leafDfs, 0);
    };

    auto branchDfs = [&](auto&& branchDfs, int bi) -> void {
        if (bi >= branchs.size()) {
            MakeHold();
            return;
        }

        int ai = branchs[bi];
        int len = shape.arms_[ai].len_;
        int pi = shape.arms_[ai].parent_;
        VASSERT(pi < ai);

        int parentAbsRot = absRots[pi];
        const pint& parentPos = poss[pi];

        int overX = 0;
        int overY = 0;
        if (parentPos.x < 0) {
            overX = -parentPos.x;
        }
        else if (parentPos.x >= N) {
            overX = parentPos.x - N + 1;
        }
        if (parentPos.y < 0) {
            overY = -parentPos.y;
        }
        else if (parentPos.y >= N) {
            overY = parentPos.y - N + 1;
        }
        if (overX + overY > shape.arms_[ai].maxLeafLen_) {
            moveBit.SetRotId(ai, RotId::Invalid);
            absRots[ai] = (parentAbsRot + armsRot[ai]) & 0b11;
            poss[ai] = parentPos + absRotToVec[absRots[ai]] * len;
            branchDfs(branchDfs, bi + 1);
            return;
        }

        for (auto [rotId, rot] : rotIdRotTbl) {
            moveBit.SetRotId(ai, rotId);
            absRots[ai] = (parentAbsRot + armsRot[ai] + rot) & 0b11;
            poss[ai] = parentPos + absRotToVec[absRots[ai]] * len;
            branchDfs(branchDfs, bi + 1);
        }
    };
    static int centerCell = gs.ToId(N / 2, N / 2);
    const int startRootCell = startState.rootCell_;

    moveBit.SetRootDir(Dir::N);
    poss[0] = gs.ToPt(startRootCell);
    absRots[0] = 0;
    rootPosScore = -gs.CalcManhattanDist(startRootCell, centerCell) * HP.CandidateRootPosCost;
    branchDfs(branchDfs, 0);


    REP(di, 4) {
        int rootCell = gs.aroundDir_[startState.rootCell_][di];
        if (rootCell >= 0) {
            moveBit.SetRootDir(Dir(di));
            poss[0] = gs.ToPt(rootCell);
            absRots[0] = 0;
            rootPosScore = -gs.CalcManhattanDist(rootCell, centerCell) * HP.CandidateRootPosCost;
            branchDfs(branchDfs, 0);
        }
    }
}

template <int NNCap>
void MakeCandidateManyBranch(const BeamState<NNCap>& startState, CandidateHolder& holder) {
    static dvec<dvec<CandidateHolder>> childrensHolders(V);
    static dvec<CheckMapD> armsCellUsed(V);

    constexpr array<pr<RotId, int>, 3> rotIdRotTbl = {
        pr<RotId, int>{RotId::N, 0},  
        pr<RotId, int>{RotId::L, 3},  
        pr<RotId, int>{RotId::R, 1},  
    };

    const RobotShape& shape = shapes_[startState.shapeId_];

    MoveBit moveBit;
    moveBit.Init();

    static VArr<u32> armsInfluenceMask;
    if (armsInfluenceMask.empty()) {
        armsInfluenceMask.resize(V);

        VArr<u32> rotMask(V);
        FOR(ai, 1, V) {
            rotMask[ai] = MoveBit::GetRotMask(ai);
        }
        REP(ai, V) {
            u32 mask = 0;
            for (int ni : shape.arms_[ai].influences_) {
                mask |= rotMask[ni];
            }
            armsInfluenceMask[ai] = mask;
        }
    }

    VArr<u8> armsRot(V);
    VArr<bool> armsHas(V);
    FOR(ai, 1, V) {
        armsRot[ai] = (u8)startState.GetRot(ai);
        armsHas[ai] = startState.GetHas(ai);
    }
    double rootPosScore = 0;

    auto dfs = [&](auto&& dfs, int parentAbsRot, int ai, const pint& p, CandidateHolder& holder) -> void {
        cauto& shapeArm = shape.arms_[ai];

        if (shapeArm.children_.empty()) {
            if (gs.IsIn(p)) {
                int cell = gs.ToId(p);

                bool enable = startState.IsTargetCell(armsHas[ai], cell);
                if (enable) {
                    holder.Push(startState, moveBit, ai, cell, rootPosScore);
                }
            }
        }
        else if (shapeArm.children_.size() == 1) {
            for (int ni : shapeArm.children_) {
                int len = shape.arms_[ni].len_;

                for (auto [rotId, rot] : rotIdRotTbl) {
                    moveBit.SetRotId(ni, rotId);
                    int nextAbsRot = (parentAbsRot + armsRot[ni] + rot) & 0b11;
                    pint nextPos = p + absRotToVec[nextAbsRot] * len;
                    dfs(dfs, nextAbsRot, ni, nextPos, holder);
                }
                moveBit.SetRotId(ni, RotId::Invalid);
            }
        }
        else {
            auto& childrensHolder = childrensHolders[ai];
            childrensHolder.resizeR(shapeArm.children_.size());

            int holdCount = HP.CandidateBranchHoldCount / shapeArm.children_.size();
            chmax(holdCount, HP.CandidateBranchHoldMin);
            for (auto& h : childrensHolder) {
                h.Init(holdCount);
            }

            REP(ci, shapeArm.children_.size()) {
                int ni = shapeArm.children_[ci];
                int len = shape.arms_[ni].len_;
                auto& childHolder = childrensHolder[ci];

                for (auto [rotId, rot] : rotIdRotTbl) {
                    moveBit.SetRotId(ni, rotId);
                    int nextAbsRot = (parentAbsRot + armsRot[ni] + rot) & 0b11;
                    pint nextPos = p + absRotToVec[nextAbsRot] * len;
                    dfs(dfs, nextAbsRot, ni, nextPos, childHolder);
                }
                moveBit.SetRotId(ni, RotId::Invalid);
            }

            CheckMapD& cellUsed = armsCellUsed[ai];
            cellUsed.Init(NN);
            VArr<TargetCell> targetCells;
            auto mergeDfs = [&](auto&& mergeDfs, int ci) -> void {
                if (ci >= shapeArm.children_.size()) {
                    if (!targetCells.empty()) {
                        holder.Push(startState, moveBit, targetCells, rootPosScore);
                    }
                    return;
                }
                else {
                    bool added = false;
                    int targetChild = shapeArm.children_[ci];
                    for (cauto& node : childrensHolder[ci].candidates_.nodePool_) {
                        VArr<int> appendCells;
                        for (cauto& tc : node.data_.targetCells_) {
                            if (!cellUsed[tc.cell_]) {
                                appendCells.push(tc.cell_);
                                cellUsed.Check(tc.cell_);
                                targetCells.push(tc);
                            }
                        }
                        if (appendCells.empty()) {
                            continue;
                        }

                        auto backupMoveBit = moveBit;
                        u32 mask = armsInfluenceMask[targetChild];
                        moveBit.bit_ &= ~mask;
                        moveBit.bit_ |= node.data_.moveBit_.bit_ & mask;

                        mergeDfs(mergeDfs, ci + 1);
                        added = true;

                        moveBit = backupMoveBit;

                        for (int cell : appendCells) {
                            cellUsed.Reset(cell);
                        }
                        targetCells.resize(targetCells.size() - appendCells.size());
                    }
                    if (!added) {
                        mergeDfs(mergeDfs, ci + 1);
                    }
                }
            };
            mergeDfs(mergeDfs, 0);
        }
    };

    static int centerCell = gs.ToId(N / 2, N / 2);
    const int startRootCell = startState.rootCell_;

    holder.Init(HP.CandidateHoldCount);

    moveBit.SetRootDir(Dir::N);
    rootPosScore = -gs.CalcManhattanDist(startRootCell, centerCell) * HP.CandidateRootPosCost;
    dfs(dfs, 0, 0, gs.ToPt(startRootCell), holder);

    REP(di, 4) {
        int rootCell = gs.aroundDir_[startState.rootCell_][di];
        if (rootCell >= 0) {
            moveBit.SetRootDir(Dir(di));
            rootPosScore = -gs.CalcManhattanDist(rootCell, centerCell) * HP.CandidateRootPosCost;
            dfs(dfs, 0, 0, gs.ToPt(rootCell), holder);
        }
    }
}

template <int NNCap>
void CalcFreeMove(const BeamState<NNCap>& startState, CapArr<Candidate, 4>& candidates) {
    candidates.clear();


    constexpr array<pr<RotId, int>, 4> rotIdRotTbl = {
        pr<RotId, int>{RotId::N, 0},        
        pr<RotId, int>{RotId::L, 3},        
        pr<RotId, int>{RotId::R, 1},        
        pr<RotId, int>{RotId::Invalid, 2},  
    };

    cauto& shape = shapes_[startState.shapeId_];
    cauto& branchs = shape.branchs_;
    cauto& leafs = shape.leafs_;

    MoveBit moveBit;
    moveBit.Init();

    VArr<u8> armsRot(V);
    VArr<bool> armsHas(V);
    FOR(ai, 1, V) {
        armsRot[ai] = (u8)startState.GetRot(ai);
        armsHas[ai] = startState.GetHas(ai);
    }
    const pint rootPos = gs.ToPt(startState.rootCell_);

    NNArr<int> targets;
    NNArr<int> takos;
    REP(i, NN) {
        if (startState.isTarget_[i]) {
            targets.push(i);
        }
        else if (startState.isTako_[i]) {
            takos.push(i);
        }
    }

    VArr<pint> poss(V);
    VArr<int> absRots(V);

    int bestTakoTurn = INT_MAX;
    pint bestTakoVec = {};
    MoveBit bestTakoMoveBit;

    int bestTargetTurn = INT_MAX;
    pint bestTargetVec = {};
    MoveBit bestTargetMoveBit;

    auto MakeHold = [&]() {
        struct LeafTarget {
            RotId rotId_;
            int cell_;
        };
        VArr<CapArr<LeafTarget, 3>> leafsTargets(leafs.size());

        REP(li, leafs.size()) {
            int ai = leafs[li];
            int len = shape.arms_[ai].len_;
            int pi = shape.arms_[ai].parent_;

            int parentAbsRot = absRots[pi];
            const pint& parentPos = poss[pi];

            cauto& cells = armsHas[ai] ? targets : takos;
            auto& bestTurn = armsHas[ai] ? bestTargetTurn : bestTakoTurn;
            auto& bestVec = armsHas[ai] ? bestTargetVec : bestTakoVec;
            auto& bestMoveBit = armsHas[ai] ? bestTargetMoveBit : bestTakoMoveBit;

            for (auto [rotId, rot] : rotIdRotTbl) {
                int absRot = (parentAbsRot + armsRot[ai] + rot) & 0b11;
                pint pos = parentPos + absRotToVec[absRot] * len;

                for (int cell : cells) {
                    pint vec = gs.ToPt(cell) - pos;
                    int turn = abs(vec.x) + abs(vec.y);
                    if (turn < bestTurn) {
                        pint r = rootPos + vec;
                        if (gs.IsIn(r)) {
                            bestTurn = turn;
                            bestVec = vec;
                            bestMoveBit = moveBit;
                        }
                    }
                }
            }
        }
    };

    auto branchDfs = [&](auto&& branchDfs, int bi) -> void {
        if (bi >= branchs.size()) {
            MakeHold();
            return;
        }

        int ai = branchs[bi];
        int len = shape.arms_[ai].len_;
        int pi = shape.arms_[ai].parent_;
        VASSERT(pi < ai);

        int parentAbsRot = absRots[pi];
        const pint& parentPos = poss[pi];

        for (auto [rotId, rot] : rotIdRotTbl) {
            moveBit.SetRotId(ai, rotId);
            absRots[ai] = (parentAbsRot + armsRot[ai] + rot) & 0b11;
            poss[ai] = parentPos + absRotToVec[absRots[ai]] * len;
            branchDfs(branchDfs, bi + 1);
        }
    };
    static int centerCell = gs.ToId(N / 2, N / 2);
    const int startRootCell = startState.rootCell_;

    moveBit.SetRootDir(Dir::N);
    poss[0] = gs.ToPt(startRootCell);
    absRots[0] = 0;
    branchDfs(branchDfs, 0);

    VASSERT(bestTakoTurn < INT_MAX || bestTargetTurn < INT_MAX);

    for (bool isTarget : {true, false}) {
        auto bestTurn = isTarget ? bestTargetTurn : bestTakoTurn;
        auto bestVec = isTarget ? bestTargetVec : bestTakoVec;
        auto bestMoveBit = isTarget ? bestTargetMoveBit : bestTakoMoveBit;
        auto cost = isTarget ? HP.CandidateTargetPosCost : HP.CandidateTakoPosCost;

        double score = -bestTurn * HP.FreeMoveTurnRate + cost * HP.FreeMoveCostRate;

        if (bestTurn < INT_MAX) {
            if (bestVec.x != 0) {
                if (bestVec.x > 0) {
                    bestMoveBit.SetRootDir(Dir::R);
                }
                else {
                    bestMoveBit.SetRootDir(Dir::L);
                }
                auto& c = candidates.push();
                c.moveBit_ = bestMoveBit;
                c.targetCells_.clear();
                c.score_ = score;
            }
            if (bestVec.y != 0) {
                if (bestVec.y > 0) {
                    bestMoveBit.SetRootDir(Dir::D);
                }
                else {
                    bestMoveBit.SetRootDir(Dir::U);
                }
                auto& c = candidates.push();
                c.moveBit_ = bestMoveBit;
                c.targetCells_.clear();
                c.score_ = score;
            }
            if (bestVec.x == 0 && bestVec.y == 0) {
                bestMoveBit.SetRootDir(Dir::N);
                auto& c = candidates.push();
                c.moveBit_ = bestMoveBit;
                c.targetCells_.clear();
                c.score_ = score;
            }
        }
    }
    VASSERT(candidates.size() > 0);
}

struct BeamAction {
    MoveBit moveBit_;
    u32 putBit_;  

    void Set(const MoveBit& moveBit, const VArr<TargetCell>& targetCells) {
        moveBit_ = moveBit;
        SetPut(targetCells);
    }

    void SetPut(const VArr<TargetCell>& targetCells) {
        putBit_ = 0;
        for (cauto& tc : targetCells) {
            putBit_ |= 1 << tc.ai_;
        }
    }
    void ToCommand(IOCommand& c) const {
        c.moves_.resize(V);
        c.puts_.resize(V);
        REP(ai, V) {
            if (ai == 0) {
                c.moves_[ai] = moveBit_.GetRootDir();
            }
            else {
                RotId rotId = moveBit_.GetRotId(ai);
                if (rotId == RotId::N) {
                    c.moves_[ai] = Dir::N;
                }
                else if (rotId == RotId::L) {
                    c.moves_[ai] = Dir::L;
                }
                else if (rotId == RotId::R) {
                    c.moves_[ai] = Dir::R;
                }
                else {
                    VABORT();
                }
            }
            c.puts_[ai] = ((putBit_ >> ai) & 1);
        }
    }
};

template <int NNCap>
using HeapPair = pr<double, BeamState<NNCap>*>;  

struct HeapCmp {
    template <int NNCap>
    bool operator()(const HeapPair<NNCap>& a, const HeapPair<NNCap>& b) const {
        return a.a < b.a;
    }
};

template <int NNCap>
struct BeamSolver {
    static constexpr int QueCap = 192;
    static constexpr int TurnCap = 350;
    ObjectPool<BeamState<NNCap>> pool_;
    CapArr<CapIntervalHeap<HeapPair<NNCap>, QueCap, HeapCmp>, TurnCap> ques_;
    ActionTree<BeamAction> actionTree_;
    dhmap<u64, int> hashSet_;

    bool Run(const RobotShape& baseShape, int limitTurn, IOResult& result) {
        pool_.Init(120000);         
        actionTree_.Init(4000000);  
        hashSet_.init(5000000);     

        bool useManyCandidate = false;
        {
            constexpr array<int, 7> borderMs = {0, 500, 300, 200, 150, 100, 0};
            int b = baseShape.branchs_.size();
            if (b > (int)borderMs.size() || M >= borderMs[b]) {
                useManyCandidate = true;
            }
        }
        bool useAppendShape = false;
        {
            int depth = 0;
            REP(ai, V) {
                chmax(depth, baseShape.arms_[ai].depth_);
            }

            if (depth <= 2) {
                double Mr = M / double(N * N);
                useAppendShape = Mr < 0.4;
            }
            else if (depth == 3) {
                useAppendShape = N < 27;
            }
            else if (depth == 4) {
                useAppendShape = N < 28;
            }
            else {
                double Mr = M / double(N * N);
                useAppendShape = Mr < 0.35;
            }
        }

        CapArr<int, 49> startCandidates;
        {
            FOR(oy, -3, 4) {
                FOR(ox, -3, 4) {
                    if (abs(oy) + abs(ox) <= 4) {
                        int cell = gs.ToId(N / 2 + ox, N / 2 + oy);
                        startCandidates.push(cell);
                    }
                }
            }
        }

        {
            auto& que = ques_.push();

            shapes_.clear();
            auto& shape = shapes_.push();
            shape = baseShape;
            for (int rootCell : startCandidates) {
                auto* statePtr = pool_.New();
                auto& state = *statePtr;
                state.Init(0, rootCell);
                state.ai_ = -1;
                que.push(HeapPair<NNCap>{0, statePtr});
            }
        }

        if (useAppendShape) {
            CapArr<pint, V_max * 2> aiAddTbl;
            FOR(ai, 1, V) {
                if (baseShape.arms_[ai].len_ < N - 1) {
                    aiAddTbl.push(pint{ai, 1});
                }
                if (baseShape.arms_[ai].len_ > 1) {
                    aiAddTbl.push(pint{ai, -1});
                }
            }
            Shuffle(ALL(aiAddTbl), rand_);

            auto& que = ques_[0];
            int queLeft = QueCap - que.size();
            int addShapeCount = (queLeft + startCandidates.size() - 1) / startCandidates.size();
            for (auto [ai, add] : aiAddTbl) {
                if (que.size() >= QueCap) {
                    break;
                }

                int si = shapes_.size();
                auto& shape = shapes_.push();
                shape = baseShape;
                shape.arms_[ai].len_ += add;
                VASSERT(shape.arms_[ai].len_ >= 1);
                VASSERT(shape.arms_[ai].len_ < N);
                chclip(shape.arms_[ai].len_, 1, N - 1);

                for (int rootCell : startCandidates) {
                    if (que.size() >= QueCap) {
                        break;
                    }
                    auto* statePtr = pool_.New();
                    auto& state = *statePtr;
                    state.Init(si, rootCell);
                    state.ai_ = -1;
                    que.push(HeapPair<NNCap>{0, statePtr});
                }
            }
        }

        int bestTurn = TurnCap;
        int bestAi = -1;
        int bestStartCell = -1;
        int bestSi = -1;

        auto MakeNext = [&](int turn, const BeamState<NNCap>& state) {
            auto Push = [&](const MoveBit& moveBit, const VArr<TargetCell>& targetCells, double appendEvalScore) {
                if (turn + 1 >= TurnCap) {
                    return;
                }

                if (turn + 1 >= ques_.size()) {
                    ques_.resize(turn + 2);
                }
                auto& que = ques_[turn + 1];

                if (que.size() < QueCap) {
                    BeamAction action;
                    action.Set(moveBit, targetCells);

                    auto* nextStatePtr = pool_.New();
                    auto& nextState = *nextStatePtr;
                    nextState = state;
                    nextState.Move(moveBit, targetCells);
                    nextState.ai_ = actionTree_.Push(state.ai_, action);

                    bool nextEnd = nextState.targetLeft_ == 0;
                    double nextEval = nextState.CalcEval(appendEvalScore);

                    if (nextEnd) {
                        VASSERT(turn + 1 < bestTurn);
                        bestTurn = turn + 1;
                        bestAi = nextState.ai_;
                        bestStartCell = state.startCell_;
                        bestSi = state.shapeId_;

                        pool_.Delete(nextStatePtr);
                    }
                    else {
                        int* ptr = hashSet_.try_get(nextState.hash_);
                        if (ptr) {
                            if (turn < *ptr) {
                                *ptr = turn;
                            }
                            else {
                                pool_.Delete(nextStatePtr);
                                return;
                            }
                        }
                        else {
                            hashSet_.set(nextState.hash_, turn);
                        }
                        que.push(HeapPair<NNCap>{nextEval, nextStatePtr});
                    }
                }
                else {
                    u64 nextHash = 0;
                    bool nextEnd = false;
                    double nextEval = 0;
                    state.CalcMovedEval(moveBit, targetCells, appendEvalScore, nextEval, nextHash, nextEnd);

                    if (nextEnd) {
                        BeamAction action;
                        action.Set(moveBit, targetCells);

                        VASSERT(turn + 1 < bestTurn);
                        bestTurn = turn + 1;
                        bestAi = actionTree_.Push(state.ai_, action);
                        bestStartCell = state.startCell_;
                        bestSi = state.shapeId_;
                    }
                    else if (nextEval > que.GetMin().a) {
                        int* ptr = hashSet_.try_get(nextHash);
                        if (ptr) {
                            if (turn < *ptr) {
                                *ptr = turn;
                            }
                            else {
                                return;
                            }
                        }
                        else {
                            hashSet_.set(nextHash, turn);
                        }

                        BeamAction action;
                        action.Set(moveBit, targetCells);

                        auto* nextStatePtr = pool_.New();
                        auto& nextState = *nextStatePtr;
                        nextState = state;
                        nextState.Move(moveBit, targetCells);
                        nextState.ai_ = actionTree_.Push(state.ai_, action);

                        VASSERT(nextHash == nextState.hash_);

                        auto* oldStatePtr = que.ReplaceMin(HeapPair<NNCap>{nextEval, nextStatePtr}).b;
                        pool_.Delete(oldStatePtr);
                    }
                }
            };

            static CandidateHolder holder;
            if (useManyCandidate) {
                MakeCandidateManyBranch(state, holder);
            }
            else {
                MakeCandidateFewBranch(state, holder);
            }

            if (holder.candidates_.nodePool_.empty()) {
                CapArr<Candidate, 4> candidates;
                CalcFreeMove(state, candidates);

                for (auto& c : candidates) {
                    FOR(ai, 1, V) {
                        if (c.moveBit_.GetRotId(ai) == RotId::Invalid) {
                            static array<RotId, 2> moveTbl = {RotId::L, RotId::R};
                            c.moveBit_.SetRotId(ai, moveTbl[rand_(2)]);
                        }
                    }
                    Push(c.moveBit_, c.targetCells_, c.score_);
                }
            }
            else {
                for (auto& node : holder.candidates_.nodePool_) {
                    auto& targetData = node.data_;

                    FOR(ai, 1, V) {
                        if (targetData.moveBit_.GetRotId(ai) == RotId::Invalid) {
                            static array<RotId, 2> moveTbl = {RotId::L, RotId::R};
                            targetData.moveBit_.SetRotId(ai, moveTbl[rand_(2)]);
                        }
                    }
                    Push(targetData.moveBit_, targetData.targetCells_, 0);
                    if (turn + 1 >= bestTurn) {
                        break;
                    }
                }
            }
        };

        int beamWidth = 0;
        int startTurn = 0;
        while (!timer_.IsOver())
        {
            FOR(turn, startTurn, bestTurn - 1) {
                if (turn >= ques_.size()) {
                    break;
                }
                auto& que = ques_[turn];
                if (que.empty()) {
                    if (turn == startTurn) {
                        ++startTurn;
                    }
                    continue;
                }

                auto* statePtr = que.GetMax().b;
                auto& state = *statePtr;
                MakeNext(turn, state);
                que.PopMax();
                pool_.Delete(statePtr);

                if (turn + 1 >= bestTurn) {
                    break;
                }
                if (timer_.IsOver()) {
                    break;
                }
            }
            ++beamWidth;
            if (startTurn >= bestTurn - 1) {
                break;
            }
        }

        int firstAi = bestAi;
        while (!timer_.IsOver())
        {
            {
                auto& que = ques_[0];
                VASSERT(que.empty());  
                REP(si, shapes_.size()) {
                    if (que.size() >= QueCap) {
                        break;
                    }
                    for (int rootCell : startCandidates) {
                        if (que.size() >= QueCap) {
                            break;
                        }
                        auto* statePtr = pool_.New();
                        auto& state = *statePtr;
                        state.Init(si, rootCell);
                        state.ai_ = -1;
                        que.push(HeapPair<NNCap>{0, statePtr});
                    }
                }
            }
            hashSet_.clear();
            startTurn = 0;

            while (!timer_.IsOver())
            {
                FOR(turn, startTurn, bestTurn - 1) {
                    if (turn >= ques_.size()) {
                        break;
                    }
                    auto& que = ques_[turn];
                    if (que.empty()) {
                        if (turn == startTurn) {
                            ++startTurn;
                        }
                        continue;
                    }

                    auto* statePtr = que.GetMax().b;
                    auto& state = *statePtr;
                    MakeNext(turn, state);
                    que.PopMax();
                    pool_.Delete(statePtr);

                    if (turn + 1 >= bestTurn) {
                        break;
                    }
                    if (timer_.IsOver()) {
                        break;
                    }
                }
                ++beamWidth;
                if (startTurn >= bestTurn - 1) {
                    break;
                }
            }
        }


        VASSERT(bestAi >= 0);

        dvec<BeamAction> actions;
        actionTree_.GetActionList(bestAi, actions);

        result.Init(shapes_[bestSi], bestStartCell);
        REP(i, actions.size()) {
            cauto& action = actions[i];
            auto& c = result.cs_.pushR();
            action.ToCommand(c);
        }


        return true;
    }
};

template <int NNCap>
struct Solver {
    void Run() {
        zobrist_.Init();

        RobotShape shape;
        GetTblRobot(shape);


        IOResult result;
        static BeamSolver<NNCap> bs;
        bs.Run(shape, result.cs_.size(), result);

        server.Output(result);
    }
};


struct Main {
    void Run(int argc, const char* argv[]) {
        server.InitInput(timer_);

        timer_.StartMs(TIME_LIMIT);

        VASSERT(argc == 1);
        if (N == 15 || N == 16) {
            static Solver<square(16)> solver;
            solver.Run();
        }
        else if (N == 17) {
            static Solver<square(17)> solver;
            solver.Run();
        }
        else if (N == 18 || N == 19) {
            static Solver<square(19)> solver;
            solver.Run();
        }
        else if (N == 20 || N == 21) {
            static Solver<square(21)> solver;
            solver.Run();
        }
        else if (N == 22) {
            static Solver<square(22)> solver;
            solver.Run();
        }
        else if (N == 23 || N == 24) {
            static Solver<square(24)> solver;
            solver.Run();
        }
        else if (N == 25) {
            static Solver<square(25)> solver;
            solver.Run();
        }
        else if (N == 26) {
            static Solver<square(26)> solver;
            solver.Run();
        }
        else if (N == 27) {
            static Solver<square(27)> solver;
            solver.Run();
        }
        else if (N == 28) {
            static Solver<square(28)> solver;
            solver.Run();
        }
        else if (N == 29) {
            static Solver<square(29)> solver;
            solver.Run();
        }
        else if (N == 30) {
            static Solver<square(30)> solver;
            solver.Run();
        }
        else {
            VABORT();
        }

        server.Finalize();

    }
};
