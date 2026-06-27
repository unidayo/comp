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
#define OUTPUT_FINAL_INFO 1
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





template <int CAP>
struct CapSet {
private:
	using Key = typename conditional<CAP <= 256, u8, u32>::type;
	CapArr<Key, CAP> elemens;		
	array<Key, CAP> indexs;		


public:

	bool operator == (const CapSet<CAP>& r) const {
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
		indexs[i] =  elemens.size();
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

	const CapArr<Key, CAP>& RefArray() {
		return elemens;
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
    bool operator[](int i) const {
        return checked_[i] == mark_;
    }

    bool operator == (const CheckMapS<S>& r) const {
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


template <class T, class CMP>
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

constexpr array<pint, 4> Around4 = { pint{-1, 0}, pint{0, -1}, pint{1, 0}, pint{0, 1} };
constexpr array<pint, 5> Around5 = { pint{-1, 0}, pint{0, -1}, pint{1, 0}, pint{0, 1}, pint{0, 0} };
constexpr array<pint, 8> Around8 = { pint{-1, 0}, pint{-1, -1}, pint{0, -1}, pint{1, -1}, pint{1, 0}, pint{1, 1}, pint{0, 1}, pint{-1, 1} };

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


inline const string& DirString(Dir dir) {
	static const string strs[6] = {
		"LEFT",
		"UP",
		"RIGHT",
		"DOWN",
		"WAIT",
		"INVALID",
	};
	return strs[(int)dir];
}

inline char DirToChar(Dir dir) {
	static const char chars[6] = {
		'L',
		'U',
		'R',
		'D',
		'N',
		'*',
	};
	return chars[(int)dir];
}

inline Dir CharToDir(char c) {
	if (c == 'L') {
		return Dir::L;
	}
	else if (c == 'U') {
		return Dir::U;
	}
	else if (c == 'R') {
		return Dir::R;
	}
	else if (c == 'D') {
		return Dir::D;
	}
	else if (c == 'N') {
		return Dir::N;
	}
	VABORT();
	return Dir::Invalid;
}


constexpr int N = 600;
int M;  
constexpr int T = 600;
int LA;  
int LB;  

#define DEF_CONTAINERS(name, size)        \
    template <class TYPE>                 \
    using name##Arr = CapArr<TYPE, size>; \
    template <class TYPE>                 \
    using name##Que = CapQue<TYPE, size>; \
    using name##Set = CapSet<size>;


DEF_CONTAINERS(N, N);
DEF_CONTAINERS(T, T);
DEF_CONTAINERS(A, 1200);
DEF_CONTAINERS(B, 24);

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

    PI(EnablerLoopCount, 2, 0, 3, 0, 3)

    PD(EnablerBaseCost, 0.516386569416615, 0.0, 4.0, 0.0);
    PD(EnablerBaseCost_LA, -0.1288869234213828, -0.1, 0.1);
    PD(EnablerBaseCost_LB, 0.02503891534423216, -0.1, 0.1);
    PD(EnablerBaseCost_ApB, -0.06444692503231499, -0.1, 0.1);
    PD(EnablerUseRate, 0.006167597735074262, -0.2, 0.4);
    PD(EnablerUseRate_LA, -0.05167395246500098, -0.1, 0.1);
    PD(EnablerUseRate_LB, -0.07155510535391019, -0.1, 0.1);
    PD(EnablerUseRate_ApB, -0.01621720393699004, -0.1, 0.1);
    PD(EnablerTerminalRate, 0.20786352955054954, -0.2, 0.4);
    PD(EnablerTerminalRate_LA, 0.004425354624500787, -0.1, 0.1);
    PD(EnablerTerminalRate_LB, 0.11706276637878504, -0.1, 0.1);
    PD(EnablerTerminalRate_ApB, -0.035602086908833944, -0.1, 0.1);



    START_TUNING;

    PD(EvalCandidateTisBase, 2.357371369444421, 0.0, 2.0);
    PD(EvalCandidateTisBase_LA, 0.03690871077619523, -0.1, 0.1);
    PD(EvalCandidateTisBase_LB, 0.04929637955239749, -0.1, 0.1);
    PD(EvalCandidateTisBase_ApB, -0.023464270917062525, -0.1, 0.1);
    PD(EvalCandidateTisExp, 1.1071827358139381, 0.0, 2.0, 0.0);
    PD(EvalCandidateTisExp_LA, 0.10280699891233151, -0.1, 0.1);
    PD(EvalCandidateTisExp_LB, 0.4019552491075202, -0.1, 0.1);
    PD(EvalCandidateTisExp_ApB, 0.08051636204201283, -0.1, 0.1);
    END_TUNING;



    PD(EvalCandidateProspectSize, 6.376645237438615, 4.0, 7.0);
    PD(EvalCandidateProspectSize_LA, -0.18760139116970145, -0.1, 0.1);
    PD(EvalCandidateProspectSize_LB, 0.1889042442398586, -0.1, 0.1);
    PD(EvalCandidateProspectSize_ApB, -0.5731108490947392, -0.1, 0.1);
    PD(EvalCandidateCommonSize, 8.09538678975907, 4.0, 8.0);
    PD(EvalCandidateCommonSize_LA, 0.16595131428853904, -0.1, 0.1);
    PD(EvalCandidateCommonSize_LB, -0.1346370160055713, -0.1, 0.1);
    PD(EvalCandidateCommonSize_ApB, -0.393340185192709, -0.1, 0.1);
    PD(EvalCandidateCoverVertex, 0.3550366731186142, 0.0, 3.0);
    PD(EvalCandidateCoverVertex_LA, 0.04320182839205379, -0.1, 0.1);
    PD(EvalCandidateCoverVertex_LB, 0.06037827984010012, -0.1, 0.1);
    PD(EvalCandidateCoverVertex_ApB, 0.024171294462584737, -0.1, 0.1);
    PD(EvalCandidateTerminalLeft, -0.5554627057217593, -2.0, 2.0);
    PD(EvalCandidateTerminalLeft_LA, -0.06829344591271037, -0.1, 0.1);
    PD(EvalCandidateTerminalLeft_LB, -0.18099747875129757, -0.1, 0.1);
    PD(EvalCandidateTerminalLeft_ApB, -0.0902237499654279, -0.1, 0.1);



    PD(PreSwitchLimitCostPer, 0.7904552615353643, 0.0, 2.0, 0.0);
    PD(PreSwitchLimitCostPer_LA, -0.03590980953564775, -0.1, 0.1);
    PD(PreSwitchLimitCostPer_LB, 0.22653808926847419, -0.1, 0.1);
    PD(PreSwitchLimitCostPer_ApB, -0.1159464946219055, -0.1, 0.1);


    PD(PreSwitchLimitCostPer2, 0.8362863880278553, 0.0, 2.0, 0.0);
    PD(PreSwitchLimitCostPer2_LA, 0.009145032105403041, -0.1, 0.1);
    PD(PreSwitchSecondScore, 0.5, 0, 1.0);





} HP;


#define DEF_DOUBLE_PARAM(name)                                                    \
    static const double name = BlendDoubleParam(HP.name, -1e10, 1e10,             \
                                                {                                 \
                                                    ipLA.GetRate(HP.name##_LA),   \
                                                    ipLB.GetRate(HP.name##_LB),   \
                                                    ipApB.GetRate(HP.name##_ApB), \
                                                });

#undef PC
#undef PI
#undef PD




struct IOCommand {
    bool isMove_;
    int lenOrV_;
    int startA_;
    int startB_;

    void SetSwitch(int len, int startA, int startB) {
        isMove_ = false;
        lenOrV_ = len;
        startA_ = startA;
        startB_ = startB;
    }

    void SetMove(int v) {
        isMove_ = true;
        lenOrV_ = v;
    }

    void Output(ostream& os) const {
        if (isMove_) {
            os << "m " << lenOrV_ << endl;
        }
        else {
            os << "s " << lenOrV_ << " " << startA_ << " " << startB_ << endl;
        }
    }
};

struct IOResult {
    AArr<int> a_;
    dvec<IOCommand> cs_;

    void SetA(const AArr<int>& a) {
        a_ = a;
    }

    IOCommand& Push() {
        return cs_.pushR();
    }

    void Output(ostream& os) const {
        VASSERT(a_.size() == LA);
        REP(i, LA) {
            if (i != 0) {
                os << " ";
            }
            os << a_[i];
        }
        os << endl;
        for (cauto& c : cs_) {
            c.Output(os);
        }
    }
};

InputParam<int> ipLA(600, 1200);
InputParam<int> ipLB(4, 24);
InputParam<double> ipApB(25, 300);

struct IOServer {
    TArr<s32> ts_;
    NArr<NArr<int>> arounds_;  
    NArr<NArr<int>> vToTis_;   
    NArr<pint> points_;        

    TArr<s32> allTerminals_;  

    bool IsTerminal(int v) const {
        return !vToTis_[v].empty();
    }

    int FindTerminal(const pint& p) {
        int bestD = INT_MAX;
        int bestT = -1;
        for (int t : allTerminals_) {
            int d = norm2(points_[t] - p);
            if (d < bestD) {
                bestD = d;
                bestT = t;
            }
        }
        return bestT;
    };

    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        int dummy;
        is >> dummy;
        timer.Init();  

        is >> M >> dummy >> LA >> LB;

        arounds_.resize(T);
        int u, v;
        REP(i, M) {
            is >> u >> v;
            arounds_[u].push(v);
            arounds_[v].push(u);
        }

        vToTis_.resize(N);

        ts_.resize(T);
        NArr<bool> used(N, false);
        int t;
        REP(i, T) {
            is >> t;
            ts_[i] = t;
            vToTis_[t].push(i);
            if (!used[t]) {
                allTerminals_.push(t);
                used[t] = true;
            }
        }

        points_.resize(N);
        int x, y;
        REP(i, N) {
            is >> x >> y;
            points_[i].x = x;
            points_[i].y = y;
        }

        ipLA.SetValue(LA);
        ipLB.SetValue(LB);
        ipApB.SetValue(LA / (double)LB);
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


static CapQue<int, 10000> finderQueue_;

struct FinderBackup {
    int start;
    int v;
    int dist;
};

struct Finder {
    int start_;
    dvec<int> dists_;

   public:
    void Clear() {
        start_ = -1;
        dists_.clear();
    }

    void Init(int vertexCount, int start, const dvec<dvec<int>>& arounds) {
        start_ = start;

        auto& dists = dists_;
        dists.assign(vertexCount, -1);

        auto& que = finderQueue_;

        dists[start] = 0;
        finderQueue_.clear();
        que.push(start);

        while (!que.empty()) {
            int p = que.front();
            que.pop();
            for (int n : arounds[p]) {
                if (dists[n] >= 0) {
                    continue;
                }
                dists[n] = dists[p] + 1;
                que.push(n);
            }
        }
    }



    void AddVertex(const dvec<dvec<int>>& arounds, const dvec<int>& froms, dvec<FinderBackup>& backup) {
        VASSERT(arounds.size() == dists_.size() + 1);

        auto& dists = dists_;
        auto& que = finderQueue_;
        que.clear();

        int addVi = arounds.size() - 1;

        {
            int d = INT_MAX;
            for (int n : froms) {
                if (dists[n] < d) {
                    d = dists[n];
                }
            }
            VASSERT(d != INT_MAX);
            dists.pushR() = d + 1;
        }

        que.push(addVi);
        while (!que.empty()) {
            int p = que.front();
            que.pop();

            int nd = dists_[p] + 1;
            for (int n : arounds[p]) {
                if (nd < dists[n]) {
                    auto& b = backup.pushR();
                    b.start = start_;
                    b.v = n;
                    b.dist = dists[n];
                    dists[n] = nd;
                    que.push(n);
                }
            }
        }
    }
    void AddVertex(const dvec<dvec<int>>& arounds, dvec<FinderBackup>& backup) {
        AddVertex(arounds, arounds[arounds.size() - 1], backup);
    }
    void AddVertex(const dvec<dvec<int>>& arounds) {
        static dvec<FinderBackup> backup;
        backup.clear();
        AddVertex(arounds, backup);
    }

    void Restore(const dvec<pint>& backup) {
        auto& dists = dists_;
        RREP(i, backup.size()) {
            cauto& b = backup[i];
            dists[b.a] = b.b;
        }
        dists.pop();
    }








    void AddVertexEx(int newVi, const dvec<dvec<int>>& arounds) {
        VASSERT(newVi != start_);
        VASSERT(arounds.size() >= dists_.size());

        REP(i, arounds.size() - dists_.size()) {
            dists_.pushR() = -1;
        }

        auto& dists = dists_;
        auto& que = finderQueue_;
        while (!que.empty()) {
            que.pop();
        }

        {
            int d = INT_MAX;
            for (int n : arounds[newVi]) {
                if (dists[n] >= 0 && dists[n] < d) {
                    d = dists[n];
                }
            }
            if (d == INT_MAX) {
                dists[newVi] = -1;
            }
            else {
                dists[newVi] = d + 1;
                que.push(newVi);
            }
        }

        while (!que.empty()) {
            int p = que.front();
            que.pop();

            int nd = dists_[p] + 1;
            for (int n : arounds[p]) {
                if (dists[n] < 0 || nd < dists[n]) {
                    dists[n] = nd;
                    que.push(n);
                }
            }
        }
    }

    void RemoveVertexEx(int removeVi, const dvec<dvec<int>>& arounds) {
        VASSERT(removeVi != start_);

        auto& dists = dists_;
        auto& que = finderQueue_;
        que.clear();

        static dvec<bool> checked;
        checked.assign(dists.size(), false);

        {
            auto dfs = [&](auto&& dfs, int p) -> void {
                int d = dists[p];
                dists[p] = -1;

                for (int n : arounds[p]) {
                    if (dists[n] < 0) {
                        continue;
                    }
                    if (dists[n] == d + 1) {
                        dfs(dfs, n);
                    }
                    else {
                        if (p != removeVi) {
                            if (!checked[n]) {
                                checked[n] = true;
                                que.push(n);
                            }
                        }
                    }
                }
            };
            dfs(dfs, removeVi);
        }

        {
            while (!que.empty()) {
                int p = que.front();
                que.pop();

                if (dists[p] < 0) {
                    continue;
                }

                for (int n : arounds[p]) {
                    if (n == removeVi) {
                        continue;
                    }
                    if (dists[n] < 0 || dists[p] + 1 < dists[n]) {
                        dists[n] = dists[p] + 1;
                        que.push(n);
                    }
                }
            }
        }
    }

    void Remove(const dvec<int>& removeVis, const dvec<dvec<int>>& arounds) {


        auto& dists = dists_;
        auto& que = finderQueue_;
        que.clear();

        static dvec<bool> checked;
        checked.assign(dists.size(), false);

        static dvec<bool> removeFlag;
        removeFlag.assign(dists.size(), false);
        for (int v : removeVis) {
            VASSERT(v != start_);
            removeFlag[v] = true;
        }

        for (int v : removeVis) {
            auto dfs = [&](auto&& dfs, int p) -> void {
                if (dists[p] < 0) {
                    return;
                }
                int d = dists[p];
                dists[p] = -1;

                for (int n : arounds[p]) {
                    if (dists[n] < 0) {
                        continue;
                    }
                    if (dists[n] == d + 1) {
                        dfs(dfs, n);
                    }
                    else {
                        if (!removeFlag[p]) {
                            checked[n] = true;
                        }
                    }
                }
            };
            dfs(dfs, v);
        }

        {
            REP(v, dists.size()) {
                if (checked[v] && dists[v] >= 0) {
                    que.push(v);
                }
            }

            while (!que.empty()) {
                int p = que.front();
                que.pop();

                if (dists[p] < 0) {
                    continue;
                }

                for (int n : arounds[p]) {
                    if (removeFlag[n]) {
                        continue;
                    }
                    if (dists[n] < 0 || dists[p] + 1 < dists[n]) {
                        dists[n] = dists[p] + 1;
                        que.push(n);
                    }
                }
            }
        }
    }
};

struct MinSwitch {
    int ci;          
    BArr<int> vs;    
    bitset<N> bits;  

    bool HasV(int v) const {
        return bits.test(v);
    }
};

struct Switch {
    int len;
    int startA;
    int startB;
    BArr<int> vs;     
    BArr<int> total;  
    bitset<N> bits;   

    bool HasV(int v) const {
        return bits.test(v);
    }
};

struct Enabler {
    NArr<bool> enableVs_;
    NArr<NArr<int>> vertexDists_;
    NArr<NArr<int>> vertexFroms_;
    NArr<int> routeVisitCounts_;

    void Init() {
        {
            NQue<int> que;

            vertexDists_.resize(N);
            vertexFroms_.resize(N);
            REP(from, N) {
                auto& dists = vertexDists_[from];
                auto& froms = vertexFroms_[from];
                dists.assign(N, -1);
                froms.assign(N, -1);

                dists[from] = 0;
                que.clear();
                que.push(from);

                while (!que.empty()) {
                    int p = que.pop();
                    for (int n : server.arounds_[p]) {
                        if (dists[n] >= 0) {
                            continue;
                        }
                        dists[n] = dists[p] + 1;
                        froms[n] = p;
                        que.push(n);
                    }
                }
            }

            enableVs_.assign(N, false);

            int from = 0;
            REP(i, T) {
                int to = server.ts_[i];
                enableVs_[from] = true;
                enableVs_[to] = true;
                REP(via, N) {
                    int nearD = vertexDists_[from][to];
                    int viaD = vertexDists_[from][via] + vertexDists_[via][to];
                    if (viaD == nearD) {
                        enableVs_[via] = true;
                    }
                }
                from = to;
            }

            int enableCount = 0;
            REP(from, N) {
                auto& dists = vertexDists_[from];
                auto& froms = vertexFroms_[from];
                dists.clear();
                froms.clear();
                if (!enableVs_[from]) {
                    continue;
                }
                ++enableCount;

                dists.assign(N, -1);
                froms.assign(N, -1);
                dists[from] = 0;
                que.clear();
                que.push(from);

                NArr<int> reacheCells;
                while (!que.empty()) {
                    int p = que.pop();
                    reacheCells.push(p);
                    for (int n : server.arounds_[p]) {
                        if (!enableVs_[n]) {
                            continue;
                        }
                        if (dists[n] >= 0) {
                            continue;
                        }
                        dists[n] = dists[p] + 1;
                        froms[n] = p;
                        que.push(n);
                    }
                }
            }
        }

        struct Node {
            int point;
            double totalCost;

            bool operator<(Node const& n) const {
                return totalCost > n.totalCost;
            }
        };

        static dvec<int> route;
        static dvec<bool> isTerminal;

        static dvec<int> froms;
        static dvec<double> minCosts;
        static priority_queue<Node> que;
        static dvec<int> tmp;

        DEF_DOUBLE_PARAM(EnablerBaseCost);
        DEF_DOUBLE_PARAM(EnablerUseRate);
        DEF_DOUBLE_PARAM(EnablerTerminalRate);

        REP(loop, HP.EnablerLoopCount) {
            NArr<int> useCount(N, 0);
            for (int si : route) {
                useCount[si] = true;
            }

            route.clear();
            isTerminal.clear();

            REP(ti, T) {
                int from = ti > 0 ? server.ts_[ti - 1] : 0;
                int to = server.ts_[ti];

                minCosts.assign(N, 1e100);
                froms.assign(N, -1);
                VASSERT(que.empty());

                minCosts[from] = 0;
                que.push(Node{from, 0});

                cauto& dists = vertexDists_[from];

                while (!que.empty()) {
                    Node node = que.top();
                    que.pop();

                    if (minCosts[node.point] != node.totalCost) {
                        continue;
                    }

                    for (int n : server.arounds_[node.point]) {
                        if (!enableVs_[n]) {
                            continue;
                        }
                        if (dists[n] != dists[node.point] + 1) {
                            continue;
                        }
                        int next = n;
                        double cost = EnablerBaseCost - useCount[n] * EnablerUseRate -
                                      server.vToTis_[n].size() * EnablerTerminalRate;
                        chmax(cost, 0);

                        Node newNode;
                        newNode.point = next;
                        newNode.totalCost = node.totalCost + cost;

                        if (newNode.totalCost < minCosts[newNode.point]) {
                            minCosts[newNode.point] = newNode.totalCost;
                            froms[newNode.point] = node.point;
                            que.push(newNode);
                        }
                    }
                }

                int w = to;
                tmp.clear();
                while (froms[w] >= 0) {
                    tmp.pushR() = w;
                    w = froms[w];
                }
                RREP(i, tmp.size()) {
                    route.pushR() = tmp[i];
                    isTerminal.pushR() = (i == 0);
                }
            }
        }

        NArr<int> useCount(N, 0);
        for (int si : route) {
            useCount[si] = true;
        }

        REP(from, N) {
            auto& froms = vertexFroms_[from];
            froms.clear();
            if (!enableVs_[from]) {
                continue;
            }

            cauto& dists = vertexDists_[from];

            minCosts.assign(N, 1e100);
            froms.assign(N, -1);
            VASSERT(que.empty());

            minCosts[from] = 0;
            que.push(Node{from, 0});

            while (!que.empty()) {
                Node node = que.top();
                que.pop();

                if (minCosts[node.point] != node.totalCost) {
                    continue;
                }

                for (int n : server.arounds_[node.point]) {
                    if (!enableVs_[n]) {
                        continue;
                    }
                    if (dists[n] != dists[node.point] + 1) {
                        continue;
                    }

                    int next = n;
                    double cost =
                        EnablerBaseCost - useCount[n] * EnablerUseRate - server.vToTis_[n].size() * EnablerTerminalRate;
                    chmax(cost, 0);

                    Node newNode;
                    newNode.point = next;
                    newNode.totalCost = node.totalCost + cost;

                    if (newNode.totalCost < minCosts[newNode.point]) {
                        minCosts[newNode.point] = newNode.totalCost;
                        froms[newNode.point] = node.point;
                        que.push(newNode);
                    }
                }
            }
        }

        routeVisitCounts_.assign(N, 0);
        static dvec<int> vs;
        REP(ti, T) {
            int from = ti > 0 ? server.ts_[ti - 1] : 0;
            int to = server.ts_[ti];

            {
                vs.clear();
                {
                    cauto& froms = vertexFroms_[from];
                    int w = to;
                    while (froms[w] >= 0) {
                        vs.pushR() = w;
                        w = froms[w];
                    }
                }

                for (int v : vs) {
                    ++routeVisitCounts_[v];
                }
            }
        }
    }
};

void MakeSwitch(const AArr<int>& as, dvec<Switch>& switchs) {
    switchs.clear();

    {
        int idx = as.find(0);
        auto& s = switchs.pushR();
        s.len = 1;
        s.startA = idx;
        s.startB = 0;
        s.vs.clear();
        s.vs.push(0);
        s.total.assign(LB, -1);
        s.total[0] = 0;
    }

    NArr<bool> usedSingle(N, false);
    REP(startA, LA - LB + 1) {
        int len = min(LB, as.size() - startA);
        VASSERT(len == LB);

        NArr<bool> area(N, false);
        REP(i, len) {
            int v = as[startA + i];
            area[v] = true;
        }

        NArr<bool> checked(N, false);
        REP(baseCell, N) {
            if (!area[baseCell] || checked[baseCell]) {
                continue;
            }
            BArr<int> vs;
            NQue<int> que;
            checked[baseCell] = true;
            que.push(baseCell);
            while (que.exist()) {
                int p = que.pop();
                vs.push(p);
                for (int n : server.arounds_[p]) {
                    if (!area[n]) {
                        continue;
                    }
                    if (checked[n]) {
                        continue;
                    }
                    checked[n] = true;
                    que.push(n);
                }
            }

            if (vs.size() == 1) {
                int v = vs[0];
                if (usedSingle[v]) {
                    continue;
                }
                usedSingle[v] = true;
            }

            auto& s = switchs.pushR();
            s.len = len;
            s.startA = startA;
            s.startB = 0;
            s.vs = vs;
            s.total.resize(LB);
            REP(i, LB) {
                s.total[i] = as[startA + i];
            }
        }
    }

    dvec<int> sis;
    sis.resizeR(switchs.size() - 1);
    iota(ALL(sis), 1);
    stable_sort(ALL(sis), [&](int a, int b) { return switchs[a].vs.size() > switchs[b].vs.size(); });

    dvec<int> newSis;
    for (int siA : sis) {
        bool ng = false;
        for (int siB : newSis) {
            bool unique = false;
            for (int v : switchs[siA].vs) {
                if (switchs[siB].vs.find(v) < 0) {
                    unique = true;
                    break;
                }
            }
            if (!unique) {
                ng = true;
                break;
            }
        }
        if (!ng) {
            newSis.pushR() = siA;
        }
    }
    dvec<Switch> newSwitchs;
    newSwitchs.pushR() = switchs[0];
    for (int si : newSis) {
        newSwitchs.pushR() = switchs[si];
    }
    switchs = move(newSwitchs);


    for (auto& s : switchs) {
        s.bits.reset();
        for (int v : s.vs) {
            s.bits.set(v);
        }
    }
}

NArr<u64> vertexHashTbl_;

void InitVertexHash() {
    vertexHashTbl_.resize(N);
    REP(v, N) {
        vertexHashTbl_[v] = rand_.Get64();
    }
}

u64 MakeVertexHash(const BArr<int>& vs) {
    u64 h = 0;
    for (int v : vs) {
        if (v >= 0) {
            h ^= vertexHashTbl_[v];
        }
    }
    return h;
}
u64 MakeVertexHash(const dvec<int>& vs) {
    u64 h = 0;
    for (int v : vs) {
        h ^= vertexHashTbl_[v];
    }
    return h;
}

void SolveDP(const NArr<NArr<int>>& vertexDists, dvec<Switch>& switchs, IOResult& result) {
    const int MaxTmpD = LB - 1;


    switchs.reserve(switchs.size() * 2);

    static dvec<dvec<int>> arounds(switchs.size());
    REP(si, switchs.size()) {
        cauto& switchA = switchs[si];
        FOR(sj, si + 1, switchs.size()) {
            cauto& switchB = switchs[sj];

            bool join = false;
            for (int va : switchA.vs) {
                for (int vb : switchB.vs) {
                    if (vertexDists[va][vb] == 1) {
                        join = true;
                        break;
                    }
                }
                if (join) {
                    break;
                }
            }
            if (join) {
                arounds[si].pushR() = sj;
                arounds[sj].pushR() = si;
            }
        }
    }

    static TArr<CapArr<AArr<int>, 25>> tisStartATable(T);
    REP(ti, T) {
        auto& startATable = tisStartATable[ti];
        startATable.resize(MaxTmpD + 1);

        int target = server.ts_[ti];
        FOR(minD, 1, MaxTmpD + 1) {
            auto& tbl = startATable[minD];
            tbl.clear();

            static dvec<int> usedStarts;
            usedStarts.clear();

            cauto& as = result.a_;
            REP(startA, as.size() - minD + 1) {
                {
                    bool hasTarget = false;
                    REP(i, minD) {
                        if (as[startA + i] == target) {
                            hasTarget = true;
                            break;
                        }
                    }
                    if (!hasTarget) {
                        continue;
                    }
                }

                {
                    bool exist = false;
                    for (int usedStart : usedStarts) {
                        bool diff = false;
                        REP(i, minD) {
                            if (as[startA + i] != as[usedStart + i]) {
                                diff = true;
                                break;
                            }
                        }
                        if (!diff) {
                            exist = true;
                            break;
                        }
                    }
                    if (exist) {
                        continue;
                    }
                }

                usedStarts.pushR() = startA;
                tbl.push(startA);
            }
        }
    }

    static dvec<dvec<int>> framesFroms(1);
    static array<dvec<int>, 2> framesComps;  
    int curCompFrame = 0;
    framesFroms.reserve(3000);
    framesFroms[0].assign(switchs.size(), -1);
    framesComps[0].assign(switchs.size(), 0);
    framesFroms[0][0] = 0;  

    static dhmap<u64, int> switchsHashMap;
    switchsHashMap.init(100000);  

    bool enableCut = false;

    int finalSi = -1;
    int maxCmp = 0;
    int skip = 0;
    while (true) {
        if (!enableCut && timer_.ElapseTimeMs() >= TIME_LIMIT - 600) {
            enableCut = true;
        }
        framesFroms.resizeR(framesFroms.size() + 1);
        auto& curFroms = framesFroms[framesFroms.size() - 2];
        auto& nextFroms = framesFroms[framesFroms.size() - 1];
        auto& curComps = framesComps[curCompFrame];
        auto& nextComps = framesComps[1 - curCompFrame];

        nextFroms.assign(switchs.size(), -1);
        nextComps.assign(switchs.size(), -1);

        int active = 0;
        REP(p, switchs.size()) {
            if (curFroms[p] < 0) {
                continue;
            }
            ++active;

            int curComp = curComps[p];

            if (enableCut && curComp + 1 < maxCmp) {
                ++skip;
                continue;
            }

            for (int n : arounds[p]) {
                if (n == 0) {
                    continue;
                }
                if (switchs[n].len < LB) {
                    continue;
                }

                int nextComp = curComp;

                while (nextComp < T && switchs[n].HasV(server.ts_[nextComp])) {
                    ++nextComp;
                }

                if (nextComp > nextComps[n]) {
                    nextComps[n] = nextComp;
                    nextFroms[n] = p;

                    if (nextComp == T) {
                        finalSi = n;
                        break;
                    }

                    chmax(maxCmp, nextComp);
                }
            }
            if (finalSi >= 0) {
                break;
            }

            if (switchs[p].vs.size() == LB) {
                int target = server.ts_[curComp];

                int minD = INT_MAX;
                for (int v : switchs[p].vs) {
                    for (int n : server.arounds_[v]) {
                        int d = vertexDists[v][target];
                        chmin(minD, d);
                    }
                }
                VASSERT(minD > 0);

                if (minD <= MaxTmpD) {

                    cauto& as = result.a_;
                    for (int startA : tisStartATable[curComp][minD]) {
                        REP(startB, LB - minD + 1) {
                            u64 totalHash = (((u64)p) << 32) | (startA << 10) | (startB << 5) | minD;
                            if (int* psi = switchsHashMap.try_get(totalHash)) {
                                int sameSi = *psi;

                                int nextComp = curComp;
                                while (nextComp < T && switchs[sameSi].HasV(server.ts_[nextComp])) {
                                    ++nextComp;
                                }
                                VASSERT(nextComp > curComp);
                                VASSERT(nextComp > nextComps[sameSi]);

                                if (nextComp > nextComps[sameSi]) {
                                    nextComps[sameSi] = nextComp;
                                    nextFroms[sameSi] = p;
                                }

                                continue;  
                            }

                            static CheckMapS<N> area;
                            area.Clear();
                            {
                                cauto& orgTotal = switchs[p].total;
                                REP(i, LB) {
                                    if (i >= startB && i < startB + minD) {
                                        area.Check(as[startA + i - startB]);
                                    }
                                    else {
                                        area.Check(orgTotal[i]);
                                    }
                                }
                            }

                            BArr<int> vs;
                            static CheckMapS<N> checked;
                            checked.Clear();
                            {
                                auto dfs = [&](auto&& dfs, int v) -> void {
                                    vs.push(v);
                                    checked.Check(v);
                                    for (int n : server.arounds_[v]) {
                                        if (!area[n]) {
                                            continue;
                                        }
                                        if (checked[n]) {
                                            continue;
                                        }
                                        dfs(dfs, n);
                                    }
                                };
                                dfs(dfs, target);
                            }
                            if (vs.size() != LB) {
                                continue;
                            }

                            {
                                bool canMove = false;
                                for (int v : switchs[p].vs) {
                                    if (checked[v]) {
                                        canMove = true;
                                        break;
                                    }
                                    for (int n : server.arounds_[v]) {
                                        if (checked[n]) {
                                            canMove = true;
                                            break;
                                        }
                                    }
                                    if (canMove) {
                                        break;
                                    }
                                }
                                if (!canMove) {
                                    continue;
                                }
                            }

                            auto& newSwitch = switchs.pushR();
                            newSwitch.len = minD;
                            newSwitch.startA = startA;
                            newSwitch.startB = startB;
                            newSwitch.vs = vs;
                            newSwitch.total = switchs[p].total;
                            REP(i, minD) {
                                newSwitch.total[startB + i] = as[startA + i];
                            }
                            newSwitch.bits.reset();

                            int newSi = switchs.size() - 1;

                            for (int v : vs) {
                                newSwitch.bits.set(v);
                                for (int n : server.arounds_[v]) {
                                    checked.Check(n);
                                }
                            }

                            {
                                auto& newAround = arounds.pushR();

                                arounds[p].pushR() = newSi;

                                FOR(sj, 1, newSi) {
                                    cauto& switchB = switchs[sj];
                                    if (switchB.len < LB) {
                                        break;
                                    }

                                    bool join = false;
                                    for (int vb : switchB.vs) {
                                        if (checked[vb]) {
                                            join = true;
                                            break;
                                        }
                                    }
                                    if (join) {
                                        arounds[newSi].pushR() = sj;
                                    }
                                }
                            }

                            int nextComp = curComp;
                            while (nextComp < T && switchs[newSi].HasV(server.ts_[nextComp])) {
                                ++nextComp;
                            }
                            VASSERT(nextComp > curComp);

                            nextFroms.pushR() = p;
                            nextComps.pushR() = nextComp;

                            switchsHashMap.set(totalHash, newSi);

                            chmax(maxCmp, nextComp);

                            break;
                        }
                    }
                }
            }
        }

        if (finalSi >= 0) {
            VASSERT(nextComps[finalSi] == T);
            break;
        }
        curCompFrame = 1 - curCompFrame;
    }

    static dvec<int> switchRoute;
    switchRoute.reserve(framesFroms.size());
    {
        int w = finalSi;
        RREP(fi, framesFroms.size()) {
            switchRoute.pushR() = w;
            w = framesFroms[fi][w];
            VASSERT(w >= 0);
        }
        reverse(ALL(switchRoute));
    }

    {
        int curP = 0;
        int comp = 0;
        REP(fi, framesFroms.size() - 1) {
            int curSi = switchRoute[fi];
            int nextSi = switchRoute[fi + 1];
            cauto& curSwitch = switchs[curSi];
            cauto& nextSwitch = switchs[nextSi];

            NArr<bool> curArea(N, false);
            for (int v : curSwitch.vs) {
                curArea[v] = true;
            }
            NArr<bool> nextArea(N, false);
            for (int v : nextSwitch.vs) {
                nextArea[v] = true;
            }

            {
                NArr<int> dists(N, -1);
                NArr<int> froms(N, -1);
                NQue<int> que;
                que.push(curP);
                dists[curP] = 0;
                int endP = -1;
                while (que.exist()) {
                    int p = que.pop();
                    for (int n : server.arounds_[p]) {
                        if (nextArea[n]) {
                            endP = p;
                            break;
                        }
                        if (!curArea[n]) {
                            continue;
                        }
                        if (dists[n] >= 0) {
                            continue;
                        }
                        dists[n] = dists[p] + 1;
                        froms[n] = p;
                        que.push(n);
                    }
                    if (endP >= 0) {
                        break;
                    }
                }
                VASSERT(endP >= 0);
                int w = endP;
                static NArr<int> route;
                route.clear();
                while (froms[w] >= 0) {
                    route.push(w);
                    w = froms[w];
                }
                VASSERT(w == curP);
                reverse(ALL(route));
                REP(i, route.size()) {
                    result.Push().SetMove(route[i]);
                }
                curP = endP;
            }

            result.Push().SetSwitch(nextSwitch.len, nextSwitch.startA, nextSwitch.startB);

            while (comp < T && switchs[nextSi].HasV(server.ts_[comp])) {
                NArr<int> dists(N, -1);
                NArr<int> froms(N, -1);
                NQue<int> que;
                que.push(curP);
                dists[curP] = 0;
                int endP = -1;
                while (que.exist()) {
                    int p = que.pop();
                    for (int n : server.arounds_[p]) {
                        if (!nextArea[n]) {
                            continue;
                        }
                        if (dists[n] >= 0) {
                            continue;
                        }
                        dists[n] = dists[p] + 1;
                        froms[n] = p;
                        que.push(n);

                        if (n == server.ts_[comp]) {
                            endP = n;
                            break;
                        }
                    }
                    if (endP >= 0) {
                        break;
                    }
                }
                VASSERT(endP >= 0);
                int w = endP;
                static NArr<int> route;
                route.clear();
                while (froms[w] >= 0) {
                    route.push(w);
                    w = froms[w];
                }
                VASSERT(w == curP);
                reverse(ALL(route));
                REP(i, route.size()) {
                    result.Push().SetMove(route[i]);
                }
                curP = endP;

                ++comp;
            }
        }
    }
}


void ToBArr(const dvec<int>& vec, BArr<int>& vs) {
    vs.clear();
    VASSERT(vec.size() <= LB);
    for (int v : vec) {
        vs.push(v);
    }
}

struct Candidate {
    BArr<int> vs;

    bool frontIsTerminal;
    bool backIsTerminal;
    bool isSteiner;
    bool isOffetRoute;
    bool isMultiPrim;
    bool isMultiSteiner;

    bitset<N> bits;
    BArr<int> tis;  

    u64 hash;
    double eval;

    void Init(const dvec<int>& vs_, int startTi) {
        vs.clear();
        for (int v : vs_) {
            vs.push(v);
        }
        frontIsTerminal = false;
        backIsTerminal = false;
        isSteiner = false;
        isOffetRoute = false;
        bits.reset();
        for (int v : vs) {
            bits.set(v);
        }
        SetTis(startTi);

        hash = 0;
        eval = 0;
    }
    void Init(const BArr<int>& vs_, int startTi) {
        vs = vs_;
        frontIsTerminal = false;
        backIsTerminal = false;
        isSteiner = false;
        isOffetRoute = false;
        bits.reset();
        for (int v : vs) {
            bits.set(v);
        }
        SetTis(startTi);

        hash = 0;
        eval = 0;
    }

    void SetTis(int startTi) {
        if (startTi > 0 && bits.test(server.ts_[startTi])) {
            while (startTi > 0 && bits.test(server.ts_[startTi - 1])) {
                --startTi;
            }
        }

        tis.clear();
        FOR(ti, startTi, T) {
            if (bits.test(server.ts_[ti])) {
                tis.push(ti);
            }
            else {
                if (!tis.empty()) {
                    break;
                }
            }
        }
    }
};


void EnumCandidates(const NArr<bool>& enableVs, const NArr<NArr<int>>& vertexDists, const NArr<NArr<int>>& vertexFroms,
                    const dvec<dvec<int>>& edgeRoutes, dvec<Candidate>& candidates) {
    auto AddNextRoute = [&](int ti, const dvec<int>& curVs) {
        int nextTi = ti + 1;
        if (nextTi >= T) {
            return;
        }
        int to = server.ts_[nextTi];

        int from = -1;
        int minD = INT_MAX;
        for (int v : curVs) {  
            int d = vertexDists[v][to];
            if (d < minD) {
                minD = d;
                from = v;
            }
        }
        if (from == server.ts_[ti]) {
            return;
        }

        dvec<int> vs;

        {
            cauto& froms = vertexFroms[to];
            int w = froms[from];
            while (w >= 0) {
                vs.pushR() = w;
                w = froms[w];
            }
        }


        if (vs.size() >= LB) {
            vs.resizeR(LB);
            auto& c = candidates.pushR();
            c.Init(vs, nextTi);
            c.isSteiner = true;
        }
    };

    auto AddRoute = [&](int ti) {
        int from = ti > 0 ? server.ts_[ti - 1] : 0;
        int to = server.ts_[ti];

        {
            dvec<int> vs;

            {
                cauto& froms = vertexFroms[from];
                int w = to;
                while (froms[w] >= 0) {
                    vs.pushR() = w;
                    w = froms[w];
                }
            }

            int startTi = ti;

            if (vs.size() < LB) {
                RREP(ti2, ti) {
                    startTi = ti2;
                    int from = ti2 > 0 ? server.ts_[ti2 - 1] : 0;
                    int to = server.ts_[ti2];

                    cauto& froms = vertexFroms[from];
                    int w = to;
                    while (froms[w] >= 0) {
                        if (vs.find(w) < 0) {  
                            vs.pushR() = w;
                        }
                        w = froms[w];
                    }
                    if (vs.size() >= LB) {
                        break;
                    }
                }
            }
            if (vs.size() >= LB) {
                vs.resizeR(LB);

                reverse(ALL(vs));

                auto& c = candidates.pushR();
                c.Init(vs, startTi);
                c.backIsTerminal = true;

                AddNextRoute(ti, vs);
            }
        }

        {
            dvec<int> vs;

            {
                cauto& froms = vertexFroms[to];
                int w = from;
                while (froms[w] >= 0) {
                    vs.pushR() = w;
                    w = froms[w];
                }
            }

            if (vs.size() < LB) {
                FOR(ti2, ti + 1, T) {
                    int from = ti2 > 0 ? server.ts_[ti2 - 1] : 0;
                    int to = server.ts_[ti2];

                    cauto& froms = vertexFroms[to];
                    int w = from;
                    while (froms[w] >= 0) {
                        if (vs.find(w) < 0) {  
                            vs.pushR() = w;
                        }
                        w = froms[w];
                    }
                    if (vs.size() >= LB) {
                        break;
                    }
                }
            }
            if (vs.size() >= LB) {
                vs.resizeR(LB);

                auto& c = candidates.pushR();
                c.Init(vs, ti);
                c.frontIsTerminal = true;
            }
        }
    };

    REP(ti, T) {
        AddRoute(ti);
    }

    {
        auto Flush = [&](const dvec<int>& vs, int ti) {
            auto& c = candidates.pushR();
            c.Init(vs, ti);
            c.isOffetRoute = true;
        };

        dvec<int> baseRoute;
        dvec<int> baseStartTi;
        REP(ti, T) {
            int from = ti > 0 ? server.ts_[ti - 1] : 0;
            int to = server.ts_[ti];

            cauto& froms = vertexFroms[to];
            int v = froms[from];
            while (v >= 0) {
                baseRoute.pushR() = v;
                baseStartTi.pushR() = ti;
                if (v == to) {
                    baseStartTi.back() += 1;
                }
                v = froms[v];
            }
            if (baseRoute.size() >= LB) {
                break;
            }
        }
        array<int, 2> startIs = {0, LB / 2};
        REP(startI, LB) {
            dvec<int> buf;
            int startTi = baseStartTi[startI];
            FOR(ti, startTi, T) {
                int to = server.ts_[ti];

                int bestD = INT_MAX;
                int from = -1;
                if (buf.empty()) {
                    from = baseRoute[startI];
                }
                else {
                    for (int v : buf) {
                        int d = vertexDists[to][v];
                        if (d < bestD) {
                            bestD = d;
                            from = v;
                        }
                    }
                }

                cauto& froms = vertexFroms[to];
                int v = froms[from];
                while (v >= 0) {
                    if (buf.size() == LB) {
                        Flush(buf, startTi);
                        buf.clear();

                        startTi = ti;  
                    }
                    buf.pushR() = v;
                    v = froms[v];
                }
            }
        }
    }

    {
        auto Flush = [&](const dvec<int>& vs, int ti) {
            auto& c = candidates.pushR();
            c.Init(vs, ti);
            c.isMultiPrim = true;
        };

        dvec<int> baseRoute;
        dvec<int> baseStartTi;
        REP(startTi, T) {
            dvec<int> buf;
            buf.pushR() = server.ts_[startTi];

            FOR(ti, startTi, T) {
                int to = server.ts_[ti];

                int bestD = INT_MAX;
                int from = -1;
                for (int v : buf) {
                    int d = vertexDists[to][v];
                    if (d < bestD) {
                        bestD = d;
                        from = v;
                    }
                }

                cauto& froms = vertexFroms[to];
                int v = froms[from];
                while (v >= 0) {
                    if (buf.size() == LB) {
                        break;
                    }
                    buf.pushR() = v;
                    v = froms[v];
                }
                if (buf.size() == LB) {
                    Flush(buf, startTi);
                    break;
                }
            }
        }
    }

    {
        auto Flush = [&](const BArr<int>& buf, int ti) {
            auto& c = candidates.pushR();
            c.Init(buf, ti);
            c.isMultiSteiner = true;
        };

        auto PushRoute = [&](BArr<int>& buf, int from, int to) {
            cauto& froms = vertexFroms[to];
            int v = froms[from];
            while (v >= 0) {
                if (buf.size() == LB) {
                    break;
                }
                if (buf.find(v) < 0) {
                    buf.push() = v;
                }
                v = froms[v];
            }
        };

        REP(startTi, T - 2) {
            int a = server.ts_[startTi];
            int b = server.ts_[startTi + 1];
            int c = server.ts_[startTi + 2];
            int bestJ = -1;
            int minD = INT_MAX;
            REP(j, N) {
                if (enableVs[j]) {
                    int dist = vertexDists[a][j] + vertexDists[b][j] + vertexDists[c][j];
                    if (dist < minD) {
                        minD = dist;
                        bestJ = j;
                    }
                }
            }
            if (minD > LB) {
                continue;
            }

            BArr<int> buf;
            buf.push(a);
            PushRoute(buf, a, bestJ);
            PushRoute(buf, bestJ, b);
            PushRoute(buf, bestJ, c);
            if (buf.size() == LB) {
                Flush(buf, startTi);
            }
            else {
                FOR(ti, startTi + 3, T) {
                    int to = server.ts_[ti];

                    int bestD = INT_MAX;
                    int from = -1;
                    for (int v : buf) {
                        int d = vertexDists[to][v];
                        if (d < bestD) {
                            bestD = d;
                            from = v;
                        }
                    }
                    PushRoute(buf, from, to);
                    if (buf.size() == LB) {
                        Flush(buf, startTi);
                        break;
                    }
                }
            }
        }
    }

    for (auto& c : candidates) {
        c.hash = MakeVertexHash(c.vs);



    }

    dvec<Candidate> newCandidate;
    newCandidate.reserve(candidates.size());

    dhmap<u64, bool> hashMap;
    hashMap.init(candidates.size());
    REP(ci, candidates.size()) {
        cauto& c = candidates[ci];
        if (hashMap.contains(c.hash)) {
            int baseCi = *hashMap.try_get(c.hash);
            auto& bc = candidates[baseCi];
            if (c.frontIsTerminal) {
                bc.frontIsTerminal = true;
            }
            if (c.backIsTerminal) {
                bc.backIsTerminal = true;
            }
            if (c.isSteiner) {
                bc.isSteiner = true;
            }
            if (c.isOffetRoute) {
                bc.isOffetRoute = true;
            }
            if (c.isMultiPrim) {
                bc.isMultiPrim = true;
            }
            if (c.isMultiSteiner) {
                bc.isMultiSteiner = true;
            }
            VASSERT(c.vs.size() == LB);
            continue;
        }
        hashMap.set(c.hash, ci);
        newCandidate.pushR() = candidates[ci];
    }

    candidates = move(newCandidate);

}


struct ArraySwicth {
    int si;     
    int start;  
    int size;   

    int GetEnd() const {
        return start + size;
    }
};

struct ArrayA {
    AArr<int> as_;
    dvec<ArraySwicth> switchs_;  

    void Init() {
        as_.clear();
        switchs_.clear();
    }

    void Push(int si, const BArr<int>& vs, bool autoJoin) {
        VASSERT(as_.size() < LA);

        if (!autoJoin) {
            auto& sw = switchs_.pushR();
            sw.si = si;
            sw.start = as_.size();
            for (int v : vs) {
                as_.push(v);
                if (as_.size() == LA) {
                    break;
                }
            }
            sw.size = as_.size() - sw.start;
            return;
        }

        int fixSize = 0;
        if (switchs_.size() >= 2) {
            cauto& sw = switchs_[switchs_.size() - 2];
            fixSize = sw.start + sw.size;
        }

        NSet commons;
        BArr<int> vsDiffs;
        for (int v : vs) {
            bool exist = false;
            FOR(i, fixSize, as_.size()) {
                if (as_[i] == v) {
                    exist = true;
                    break;
                }
            }
            if (exist) {
                commons.add(v);
            }
            else {
                vsDiffs.push(v);
            }
        }

        int fixCommon = 0;
        if (commons.size() == as_.size() - fixSize) {
            RREP(i, fixSize) {
                int idx = vsDiffs.find(as_[i]);
                if (idx >= 0) {
                    ++fixCommon;
                    vsDiffs.remove(idx);  
                }
                else {
                    break;
                }
            }
        }

        BArr<int> asDiffs;
        FOR(i, fixSize, as_.size()) {
            int v = as_[i];
            if (!commons.contains(v)) {
                asDiffs.push(v);
            }
        }

        as_.resize(fixSize);

        for (int v : asDiffs) {
            as_.push(v);
        }

        auto& sw = switchs_.pushR();
        sw.si = si;
        sw.start = as_.size() - fixCommon;

        for (int v : commons) {
            as_.push(v);
        }
        VASSERT(as_.size() < LA);

        for (int v : vsDiffs) {
            as_.push(v);
            if (as_.size() == LA) {
                break;
            }
        }

        sw.size = as_.size() - sw.start;
    }

    void CalcPushedSize(const BArr<int>& vs, int& pushedArraySize, int& commonSize) const {
        int fixSize = 0;
        if (switchs_.size() >= 2) {
            cauto& sw = switchs_[switchs_.size() - 2];
            fixSize = sw.start + sw.size;
        }

        int common = 0;
        BArr<int> vsDiffs;
        for (int v : vs) {
            bool exist = false;
            FOR(i, fixSize, as_.size()) {
                if (as_[i] == v) {
                    exist = true;
                    break;
                }
            }
            if (exist) {
                ++common;
            }
            else {
                vsDiffs.push(v);
            }
        }

        int fixCommon = 0;
        if (common == as_.size() - fixSize) {
            RREP(i, fixSize) {
                int idx = vsDiffs.find(as_[i]);
                if (idx >= 0) {
                    ++fixCommon;
                    vsDiffs.RemoveSwap(idx);
                }
                else {
                    break;
                }
            }
        }

        int newSizeA = min(as_.size() + vsDiffs.size(), LA);

        pushedArraySize = newSizeA;
        commonSize = common + fixCommon;
    }

    void Pop() {
        switchs_.pop();

        if (switchs_.empty()) {
            as_.clear();
        }
        else {
            cauto& sw = switchs_.back();
            as_.resize(sw.start + sw.size);
        }
    }

    void Remove(int si) {
        int hitI = -1;
        REP(i, switchs_.size()) {
            if (switchs_[i].si == si) {
                hitI = i;
                break;
            }
        }
        VASSERT(hitI >= 0);
        if (hitI == switchs_.size() - 1) {
            Pop();
            return;
        }

        int prevEnd = 0;
        if (hitI > 0) {
            prevEnd = switchs_[hitI - 1].GetEnd();
        }
        int nextStart = switchs_[hitI + 1].start;
        if (prevEnd < nextStart) {
            int moveSize = nextStart - prevEnd;
            as_.remove(prevEnd, nextStart);
            FOR(i, hitI + 1, switchs_.size()) {
                switchs_[i].start -= moveSize;
            }
        }
        else {
        }
        switchs_.remove(hitI);
    }

    int FindSwitch(int si) const {
        int hitI = -1;
        REP(i, switchs_.size()) {
            if (switchs_[i].si == si) {
                hitI = i;
                break;
            }
        }
        return hitI;
    }
};

struct State {
    int singleSwitchCount_;         
    dvec<MinSwitch> switchs_;       
    dvec<dvec<int>> arounds_;       
    dvec<Finder> finders_;          
    dvec<dvec<int>> tisTargetSis_;  
    NArr<int> vertexToSingleSi_;    
    dvec<int> finderSis_;           

    NArr<int> vertexsUseCount_;  
    int coverVertexCount_;       
    int terminalLeft_;           

    ArrayA arrA_;
    dhmap<u64, int> switchHashMap_;

    void Init(const NArr<bool>& enableVs, const NArr<NArr<int>>& vertexDists, const NArr<int>& routeVisitCounts) {
        vertexToSingleSi_.assign(N, -1);

        REP(i, N) {
            if (i != 0) {
                if (!enableVs[i]) {
                    continue;
                }

                if (routeVisitCounts[i] == 0) {
                    continue;
                }
            }

            vertexToSingleSi_[i] = switchs_.size();

            auto& s = switchs_.pushR();
            s.ci = -1;
            s.vs.clear();
            s.vs.push(i);
            s.bits.reset();
            s.bits.set(i);
        }
        singleSwitchCount_ = switchs_.size();

        arounds_.resizeR(switchs_.size());
        for (auto& a : arounds_) {
            a.clear();
        }
        REP(si, switchs_.size()) {
            cauto& switchA = switchs_[si];
            FOR(sj, si + 1, switchs_.size()) {
                cauto& switchB = switchs_[sj];

                bool join = false;
                for (int va : switchA.vs) {
                    for (int vb : switchB.vs) {
                        if (vertexDists[va][vb] == 1) {
                            join = true;
                            break;
                        }
                    }
                    if (join) {
                        break;
                    }
                }
                if (join) {
                    arounds_[si].pushR() = sj;
                    arounds_[sj].pushR() = si;
                }
            }
        }

        tisTargetSis_.resizeR(T);
        for (auto& t : tisTargetSis_) {
            t.clear();
        }
        REP(si, switchs_.size()) {
            for (int v : switchs_[si].vs) {
                for (int ti : server.vToTis_[v]) {
                    tisTargetSis_[ti].pushR() = si;
                }
            }
        }

        finderSis_.clear();
        finderSis_.pushR() = 0;
        FOR(si, 1, switchs_.size()) {
            bool require = false;
            for (int v : switchs_[si].vs) {
                if (!server.vToTis_[v].empty()) {
                    require = true;
                    break;
                }
            }
            if (require) {
                finderSis_.pushR() = si;
            }
        }

        finders_.resizeR(switchs_.size());
        for (int si : finderSis_) {
            finders_[si].Init(switchs_.size(), si, arounds_);
        }

        vertexsUseCount_.assign(N, 0);
        coverVertexCount_ = 0;
        terminalLeft_ = server.allTerminals_.size();


        arrA_.Init();
        switchHashMap_.init(10000);
    }

    bool ContainsSwitch(u64 hash) const {
        return switchHashMap_.contains(hash);
    }

    void PushSwitch(int ci, const NArr<NArr<int>>& vertexDists, const BArr<int>& vs, dvec<FinderBackup>& backups,
                    bool autoJoin) {
        int prevSize = arrA_.as_.size();
        arrA_.Push(switchs_.size(), vs, autoJoin);
        int pushedSize = arrA_.as_.size() - prevSize;

        cauto& asw = arrA_.switchs_.back();
        auto& s = switchs_.pushR();
        s.vs.clear();
        FOR(i, asw.start, asw.start + asw.size) {
            s.vs.push(arrA_.as_[i]);
        }
        s.bits.reset();
        for (int v : s.vs) {
            s.bits.set(v);
        }
        s.ci = ci;

        auto& newAround = arounds_.pushR();
        newAround.clear();
        int newSi = switchs_.size() - 1;
        static CheckMapS<N> check;
        check.Clear();
        for (int v : s.vs) {
            check.Check(v);
            for (int n : server.arounds_[v]) {
                check.Check(n);
            }
        }
        REP(sj, newSi) {
            cauto& switchB = switchs_[sj];
            bool join = false;
            for (int vb : switchB.vs) {
                if (check[vb]) {
                    join = true;
                    break;
                }
            }
            if (join) {
                arounds_[newSi].pushR() = sj;
                arounds_[sj].pushR() = newSi;
            }
        }
        backups.clear();
        for (int si : finderSis_) {
            finders_[si].AddVertex(arounds_, backups);
        }

        bool require = false;
        for (int v : switchs_[newSi].vs) {
            if (!server.vToTis_[v].empty()) {
                require = true;
                break;
            }
        }
        if (require) {
            finderSis_.pushR() = newSi;
        }

        finders_.resizeR(finders_.size() + 1);

        if (require) {
            finders_.back().Init(switchs_.size(), switchs_.size() - 1, arounds_);
        }

        for (int v : s.vs) {
            for (int ti : server.vToTis_[v]) {
                tisTargetSis_[ti].pushR() = newSi;
            }
        }

        for (int v : s.vs) {
            if (vertexsUseCount_[v] == 0) {
                if (server.IsTerminal(v)) {
                    --terminalLeft_;
                }
                ++coverVertexCount_;
            }
            ++vertexsUseCount_[v];
        }

    }

    void PopSwitch(const dvec<FinderBackup>& backups) {
        arrA_.Pop();

        int si = switchs_.size() - 1;
        auto& s = switchs_.back();

        for (int v : s.vs) {
            --vertexsUseCount_[v];
            if (vertexsUseCount_[v] == 0) {
                if (server.IsTerminal(v)) {
                    ++terminalLeft_;
                }
                --coverVertexCount_;
            }
        }

        for (int n : arounds_[si]) {
            if (arounds_[n].back() == si) {
                arounds_[n].pop();
            }
        }
        for (int v : s.vs) {
            for (int ti : server.vToTis_[v]) {
                VASSERT(tisTargetSis_[ti].back() == si);
                tisTargetSis_[ti].pop();
            }
        }

        switchs_.pop();
        arounds_.pop();
        finders_.pop();

        for (cauto& b : backups) {
            finders_[b.start].dists_[b.v] = b.dist;
        }
        for (int si : finderSis_) {
            finders_[si].dists_.pop();
        }
        if (finderSis_.back() == si) {
            finderSis_.pop();
        }
    }

    void FixPushSwitch(int ci, const NArr<NArr<int>>& vertexDists, const BArr<int>& orgVs, dvec<FinderBackup>& backups,
                       bool autoJoin) {

        PushSwitch(ci, vertexDists, orgVs, backups, autoJoin);

        auto& s = switchs_.back();

        u64 hash = MakeVertexHash(s.vs);
        if (int* cnt = switchHashMap_.try_get(hash)) {
            ++(*cnt);
        }
        else {
            switchHashMap_.set(hash, 1);
        }
    }

    struct SwitchAction {
        int si_;
        BArr<int> vs_;  
        int dist_;
    };

    int EvalGreedy(const NArr<NArr<int>>& vertexDists, const dvec<Candidate>& candidates) const {
        static dvec<SwitchAction> dummy;
        TArr<int> targetsDist;
        TArr<int> targetsArriveSi;
        return EvalGreedy(vertexDists, candidates, false, targetsDist, targetsArriveSi, dummy);
    }
    int EvalGreedy(const NArr<NArr<int>>& vertexDists, const dvec<Candidate>& candidates,
                   dvec<SwitchAction>& actions) const {
        TArr<int> targetsDist;
        TArr<int> targetsArriveSi;
        return EvalGreedy(vertexDists, candidates, true, targetsDist, targetsArriveSi, actions);
    }

    int EvalGreedy(const NArr<NArr<int>>& vertexDists, const dvec<Candidate>& candidates, bool calcExtra,
                   TArr<int>& targetsDist, TArr<int>& targetsArriveSi, dvec<SwitchAction>& actions) const {
        actions.clear();

        int totalD = 0;

        cauto& tisTargetSis = tisTargetSis_;
        cauto& switchs = switchs_;
        cauto& finders = finders_;
        cauto& arounds = arounds_;


        struct Cur {
            int si;
            int totalD;
            int fromI;  
        };

        static dvec<dvec<Cur>> tisCurs;
        tisCurs.resizeR(T + 1);
        for (auto& curs : tisCurs) {
            curs.clear();
        }
        {
            auto& curs = tisCurs[0];
            auto& cur = curs.pushR();
            cur.si = 0;
            cur.totalD = 0;
            cur.fromI = -1;
        }

        REP(ti, T) {
            cauto& targetSis = tisTargetSis[ti];

            cauto& curs = tisCurs[ti];
            auto& nexts = tisCurs[ti + 1];
            nexts.resizeR(targetSis.size());
            REP(i, nexts.size()) {
                auto& next = nexts[i];
                next.si = targetSis[i];
                next.totalD = INT_MAX;
                next.fromI = -1;
            }

            REP(fromI, curs.size()) {
                cauto& cur = curs[fromI];
                REP(i, nexts.size()) {
                    auto& next = nexts[i];
                    int d = finders[cur.si].dists_[next.si];
                    int nextD = cur.totalD + d;
                    if (nextD < next.totalD) {
                        next.totalD = nextD;
                        next.fromI = fromI;
                    }
                }
            }
        }

        cauto& lastCurs = tisCurs[T];
        int bestTotalD = INT_MAX;
        int bestI = -1;
        REP(i, lastCurs.size()) {
            if (lastCurs[i].totalD < bestTotalD) {
                bestTotalD = lastCurs[i].totalD;
                bestI = i;
            }
        }
        VASSERT(bestI >= 0);
        totalD = bestTotalD;

        if (calcExtra) {
            static dvec<int> sis;
            sis.clear();
            {
                int i = bestI;
                RREP(ti, T) {
                    sis.pushR() = tisCurs[ti + 1][i].si;
                    i = tisCurs[ti + 1][i].fromI;
                }
                reverse(ALL(sis));
            }
            VASSERT(sis.size() == T);

            targetsDist.resize(T);
            targetsArriveSi.resize(T);
            actions.resizeR(T);

            int curSi = 0;
            int verifyD = 0;
            REP(i, sis.size()) {
                int nextSi = sis[i];

                auto& action = actions[i];
                action.si_ = nextSi;
                action.dist_ = finders[curSi].dists_[nextSi];
                action.vs_.clear();

                targetsDist[i] = finders[curSi].dists_[nextSi];
                targetsArriveSi[i] = nextSi;

                verifyD += finders[curSi].dists_[nextSi];

                curSi = nextSi;
            }
            VASSERT(totalD == verifyD);
        }
        return totalD;
    }

    void FixActions(const dvec<SwitchAction>& baseActions, dvec<SwitchAction>& actions) const {
        actions.clear();

        {
            auto& action = actions.pushR();
            action.si_ = 0;
            action.dist_ = 0;
            action.vs_.clear();
        }

        int curSi = 0;
        BArr<int> curTmpVs;
        for (cauto& baseAction : baseActions) {
            if (baseAction.si_ >= 0) {
                static dvec<int> dists;
                static dvec<int> froms;
                dists.assign(switchs_.size(), -1);
                froms.assign(switchs_.size(), -1);
                queue<int> que;
                if (curSi >= 0) {
                    que.push(curSi);
                    dists[curSi] = 0;
                }
                else {
                    for (int v : curTmpVs) {
                        int si = vertexToSingleSi_[v];
                        que.push(si);
                        dists[si] = 0;
                    }
                }
                while (!que.empty()) {
                    int p = que.front();
                    que.pop();
                    for (int n : arounds_[p]) {
                        if (dists[n] >= 0) {
                            continue;
                        }
                        dists[n] = dists[p] + 1;
                        froms[n] = p;
                        que.push(n);
                    }
                }

                VASSERT(dists[baseAction.si_] == baseAction.dist_);

                int startI = actions.size();
                int w = baseAction.si_;
                while (froms[w] >= 0) {
                    auto& action = actions.pushR();
                    action.si_ = w;
                    action.dist_ = 1;
                    action.vs_.clear();
                    w = froms[w];
                }
                reverse(actions.begin() + startI, actions.end());

                curSi = baseAction.si_;
                curTmpVs.clear();
            }
            else {
                auto& action = actions.pushR();
                action = baseAction;
                curSi = -1;
                curTmpVs = action.vs_;
            }
        }
    }


    void CalcSwitchRoute(const NArr<NArr<int>>& vertexDists, const dvec<Candidate>& candidates,
                         dvec<int>& switchRoute) const {
        switchRoute.clear();

        static dvec<SwitchAction> baseActions;
        EvalGreedy(vertexDists, candidates, baseActions);

        static dvec<SwitchAction> actions;
        FixActions(baseActions, actions);

        for (cauto& action : actions) {
            if (action.si_ >= 0) {
                switchRoute.pushR() = action.si_;
            }
        }
    }

    void RemoveUnuseSwitch(const NArr<NArr<int>>& vertexDists, const dvec<Candidate>& candidates) {
        static dvec<int> switchRoute;
        CalcSwitchRoute(vertexDists, candidates, switchRoute);

        static dvec<int> switchUseCount;
        switchUseCount.assign(switchs_.size(), 0);
        for (int si : switchRoute) {
            ++switchUseCount[si];
        }
        static dvec<int> removeSis;
        static dvec<bool> removeSwitchFlag;
        removeSis.clear();
        removeSwitchFlag.assign(switchs_.size(), false);
        FOR(si, singleSwitchCount_, switchs_.size()) {
            if (switchUseCount[si] == 0 && !switchs_[si].vs.empty()) {
                removeSis.pushR() = si;
                removeSwitchFlag[si] = true;
            }
        }

        for (int si : finderSis_) {
            if (removeSwitchFlag[si]) {
                continue;
            }
            finders_[si].Remove(removeSis, arounds_);  
        }

        for (int removeSi : removeSis) {
            u64 hash = MakeVertexHash(switchs_[removeSi].vs);
            if (int* cnt = switchHashMap_.try_get(hash)) {
                --(*cnt);
                if (*cnt == 0) {
                    switchHashMap_.remove(hash);
                }
            }
            else {
                VABORT();
            }

            {
                for (int v : switchs_[removeSi].vs) {
                    --vertexsUseCount_[v];
                    if (vertexsUseCount_[v] == 0) {
                        if (server.IsTerminal(v)) {
                            ++terminalLeft_;
                        }
                        --coverVertexCount_;
                    }
                }
            }

            for (int v : switchs_[removeSi].vs) {
                for (int ti : server.vToTis_[v]) {
                    int idx = tisTargetSis_[ti].find(removeSi);
                    VASSERT(idx);
                    tisTargetSis_[ti].removeSwap(idx);
                }
            }

            int idx = finderSis_.find(removeSi);
            if (idx >= 0) {
                finderSis_.removeSwap(idx);
            }

            finders_[removeSi].Clear();

            for (int n : arounds_[removeSi]) {
                int idx = arounds_[n].find(removeSi);
                VASSERT(idx >= 0);
                arounds_[n].removeSwap(idx);
            }
            arounds_[removeSi].clear();

            arrA_.Remove(removeSi);
            switchs_[removeSi].vs.clear();  
        }

    }

    int ProspectArraySize() const {
        return arrA_.as_.size() + terminalLeft_;
    }

    void CalcPushedProspectArraySize(const BArr<int>& vs, int& prospectSize, int& commonSize, int& terminalLeft,
                                     int& coverVertexCount) const {
        int pushedArraySize = 0;
        arrA_.CalcPushedSize(vs, pushedArraySize, commonSize);
        prospectSize = pushedArraySize + terminalLeft_;

        int appendVSize = commonSize + (pushedArraySize - arrA_.as_.size());

        coverVertexCount = coverVertexCount_;
        terminalLeft = terminalLeft_;
        REP(i, appendVSize) {
            int v = vs[i];
            if (vertexsUseCount_[v] == 0) {
                if (server.IsTerminal(v)) {
                    --prospectSize;
                    --terminalLeft;
                }
                ++coverVertexCount;
            }
        }
    }

    void FinalizeA(const NArr<NArr<int>>& vertexDists, const dvec<Candidate>& candidates) {

        dvec<int> switchRoute;
        CalcSwitchRoute(vertexDists, candidates, switchRoute);


        {
            dvec<int> switchUseCount(switchs_.size(), 0);
            for (int si : switchRoute) {
                ++switchUseCount[si];
            }
            int unused = 0;
            dvec<int> removeSis;
            FOR(si, singleSwitchCount_, switchs_.size()) {
                if (switchUseCount[si] == 0 && !switchs_[si].vs.empty()) {
                    ++unused;
                    removeSis.pushR() = si;
                }
            }
            for (int si : removeSis) {
                arrA_.Remove(si);
            }
        }

        NSet outerSingleVertexSet;
        for (int si : switchRoute) {
            if (si < singleSwitchCount_) {
                VASSERT(switchs_[si].vs.size() == 1);
                outerSingleVertexSet.fadd(switchs_[si].vs.front());
            }
        }
        for (int v : server.allTerminals_) {
            outerSingleVertexSet.fadd(v);
        }

        auto& as_ = arrA_.as_;

        NArr<int> useCounts(N, 0);
        REP(i, as_.size()) {
            ++useCounts[as_[i]];
        }
        int overWrite = false;
        for (int v : outerSingleVertexSet) {
            if (useCounts[v] == 0) {
                if (as_.size() < LA) {
                    as_.push(v);
                    ++useCounts[v];
                }
                else {
                    RREP(ai, as_.size()) {
                        int a = as_[ai];
                        if (useCounts[a] > 1) {
                            --useCounts[a];
                            as_[ai] = v;
                            ++useCounts[v];
                            ++overWrite;
                            break;
                        }
                    }
                }
            }
        }
        int maxA = as_.size();
        while (as_.size() < LA) {
            as_.push(0);
        }


    }
};


struct Edge {
    int a;
    int b;
    int dist;
    dvec<int> vs;
    TArr<int> tisDist;      
    TArr<int> tisChange;    
    TArr<int> tisFromDist;  
    TArr<int> tisToDist;    
};
void EnumPairs(const NArr<bool>& enableVs, const NArr<NArr<int>>& vertexDists, dvec<Edge>& edges) {
    struct BaseEdge {
        int a;
        int b;
        int dist;
    };
    dvec<BaseEdge> baseEdges;
    baseEdges.reserve(N * N);
    REP(a, N) {
        if (!enableVs[a]) {
            continue;
        }
        FOR(b, a + 1, N) {
            if (!enableVs[b]) {
                continue;
            }
            if (vertexDists[a][b] <= LB) {
                continue;
            }

            auto& edge = baseEdges.pushR();
            edge.a = a;
            edge.b = b;
            edge.dist = vertexDists[a][b];

        }
    }

    constexpr int MaxEdgeCount = 1600;

    dvec<int> eis(baseEdges.size());
    iota(ALL(eis), 0);

    auto Comp = [&](int a, int b) { return baseEdges[a].dist > baseEdges[b].dist; };

    if (eis.size() > MaxEdgeCount) {
        nth_element(eis.begin(), eis.begin() + MaxEdgeCount, eis.end(), Comp);
        eis.resizeR(MaxEdgeCount);
    }

    stable_sort(ALL(eis), [&](int a, int b) { return baseEdges[a].dist > baseEdges[b].dist; });

    edges.resizeR(eis.size());
    REP(ii, eis.size()) {
        cauto& src = baseEdges[eis[ii]];
        auto& dst = edges[ii];
        dst.a = src.a;
        dst.b = src.b;
        dst.dist = src.dist;
    }
}

void AddPreSwitch(const NArr<bool>& enableVs, const NArr<NArr<int>>& vertexDists, const NArr<NArr<int>>& vertexFroms,
                  State& state, dvec<dvec<int>>& routes) {
    dvec<dvec<FinderBackup>> backupsStack;

    auto AddSwitch = [&](const dvec<int>& route) {
        int prevSize = state.arrA_.as_.size();

        bool first = true;
        REP(start, route.size() - LB + 1) {
            if (state.arrA_.as_.size() >= LA) {
                break;
            }
            BArr<int> vs;
            int i = start;
            while (i < route.size()) {
                if (vs.find(route[i]) < 0) {
                    vs.push(route[i]);
                    if (vs.size() == LB) {
                        break;
                    }
                }
                ++i;
            }
            if (vs.size() < LB) {
                break;
            }
            state.FixPushSwitch(-1, vertexDists, vs, backupsStack.pushR(), !first);
            backupsStack.clear();
            first = false;
        }
        int afterSize = state.arrA_.as_.size();
        int appendSize = afterSize - prevSize;
        int expectSize = route.size();
        VASSERT(appendSize <= expectSize);  
    };


    struct TargetArea {
        int dist;         
        int change;       
        bool fromOnEdge;  
        bool toOnEdge;    
        double gotScore;
    };

    TArr<TargetArea> areas(T);
    REP(ti, T) {
        int from = ti > 0 ? server.ts_[ti - 1] : 0;
        int to = server.ts_[ti];
        auto& area = areas[ti];
        area.dist = vertexDists[from][to];
        area.change = vertexDists[from][to];  
        area.fromOnEdge = false;
        area.toOnEdge = false;
        area.gotScore = 0;
    }

    dvec<Edge> edges;
    EnumPairs(enableVs, vertexDists, edges);

    dvec<int> eis(edges.size());
    iota(ALL(eis), 0);

    for (int ei : eis) {
        auto& edge = edges[ei];

        cauto& froms = vertexFroms[edge.b];
        edge.vs.clear();
        {
            int v = edge.a;
            while (v >= 0) {
                edge.vs.pushR() = v;
                v = froms[v];
            }
            VASSERT(vertexDists[edge.a][edge.b] == edge.vs.size() - 1);
        }

        NArr<int> dists(N, -1);
        NQue<int> que;
        for (int v : edge.vs) {
            dists[v] = 0;
            que.push(v);
        }
        while (que.exist()) {
            int p = que.pop();
            for (int n : server.arounds_[p]) {
                if (!enableVs[n]) {
                    continue;
                }
                if (dists[n] < 0) {
                    dists[n] = dists[p] + 1;
                    que.push(n);
                }
            }
        }

        edge.tisDist.resize(T);
        edge.tisChange.resize(T);
        edge.tisFromDist.resize(T);
        edge.tisToDist.resize(T);
        REP(ti, T) {
            int from = ti > 0 ? server.ts_[ti - 1] : 0;
            int to = server.ts_[ti];

            int toEdgeD = dists[from];
            int toGoalD = dists[to];
            int directDist = vertexDists[from][to];
            int directSwitch = (directDist + LB - 1) / LB;

            int viaDist = toEdgeD + toGoalD;  
            int viaSwitch = 0;
            viaSwitch += (toEdgeD + LB - 1) / LB;
            viaSwitch += (toGoalD + LB - 1) / LB;

            {
                edge.tisDist[ti] = viaDist;
                edge.tisChange[ti] = viaSwitch;
            }

            edge.tisFromDist[ti] = toEdgeD;
            edge.tisToDist[ti] = toGoalD;
        }
    }

    bool manyLB = LB >= 20;

    REP(loop, 16) {
        int bestEii = -1;
        double bestEval = 0;  

        auto EdgeScore = [&](const Edge& edge, int ti, const TargetArea& area) {
            double score = 0;
            int fromD = edge.tisFromDist[ti];
            int toD = edge.tisToDist[ti];
            if (fromD == 0 && toD == 0) {
                score = 2;
            }
            else if (fromD == 0) {
                score = HP.PreSwitchSecondScore;
            }
            else if (toD == 0) {
                score = HP.PreSwitchSecondScore;
            }
            return score;
        };

        REP(eii, eis.size()) {
            int ei = eis[eii];
            cauto& edge = edges[ei];

            double eval = 0;

            REP(ti, T) {
                cauto& area = areas[ti];
                int dist = edge.tisDist[ti];
                int change = edge.tisChange[ti];
                if (dist < 0) {
                    continue;
                }
                if (dist < area.dist) {
                }
                if (!manyLB) {
                    if (change < area.change) {
                        eval += area.change - change;
                    }

                }
                else {
                    double score = EdgeScore(edge, ti, area);
                    if (score > area.gotScore) {
                        eval += score - area.gotScore;
                    }
                }
            }



            if (eval > bestEval) {
                bestEval = eval;
                bestEii = eii;
            }
        }
        if (bestEii < 0) {
            break;
        }

        double evalPerSize = bestEval / (double)edges[eis[bestEii]].vs.size();
        if (!manyLB) {
            DEF_DOUBLE_PARAM(PreSwitchLimitCostPer);
            if (evalPerSize < PreSwitchLimitCostPer) {
                break;
            }
        }
        else {
            if (evalPerSize < HP.PreSwitchLimitCostPer2) {
                break;
            }
        }

        int ei = eis[bestEii];
        cauto& edge = edges[ei];
        REP(ti, T) {
            auto& area = areas[ti];
            int dist = edge.tisDist[ti];
            int change = edge.tisChange[ti];
            if (dist < 0) {
                continue;
            }
            if (dist < area.dist) {
                area.dist = dist;
            }
            if (change < area.change) {
                area.change = change;
            }

            if (area.fromOnEdge == false) {
                int d = edge.tisFromDist[ti];
                if (d == 0) {
                    area.fromOnEdge = true;
                }
            }
            if (area.toOnEdge == false) {
                int d = edge.tisToDist[ti];
                if (d == 0) {
                    area.toOnEdge = true;
                }
            }
            double score = EdgeScore(edge, ti, area);
            if (score > area.gotScore) {
                area.gotScore = score;
            }
        }

        AddSwitch(edge.vs);
        routes.pushR() = move(edge.vs);
        eis.removeSwap(bestEii);
    }
}

struct Solver {
    Enabler enabler_;

    void Run() {
        enabler_.Init();
        InitVertexHash();

        static State state;
        state.Init(enabler_.enableVs_, enabler_.vertexDists_, enabler_.routeVisitCounts_);

        static dvec<dvec<int>> edgeRoutes;
        AddPreSwitch(enabler_.enableVs_, enabler_.vertexDists_, enabler_.vertexFroms_, state, edgeRoutes);


        static dvec<Candidate> candidates;
        EnumCandidates(enabler_.enableVs_, enabler_.vertexDists_, enabler_.vertexFroms_, edgeRoutes, candidates);

        MakeSwitch3(enabler_.enableVs_, enabler_.vertexDists_, enabler_.vertexFroms_, candidates,
                    enabler_.routeVisitCounts_, state);


        static dvec<Switch> switchs;
        MakeSwitch(state.arrA_.as_, switchs);


        static IOResult result;
        result.SetA(state.arrA_.as_);
        SolveDP(enabler_.vertexDists_, switchs, result);



        int score = 0;
        REP(i, result.cs_.size()) {
            if (!result.cs_[i].isMove_) {
                score += 1;
            }
        }
        cerr << "Score: " << score << endl;
        server.Output(result);
    }

    void MakeSwitch3(const NArr<bool>& enableVs, const NArr<NArr<int>>& vertexDists, const NArr<NArr<int>>& vertexFroms,
                     dvec<Candidate>& candidates, const NArr<int>& routeVisitCounts, State& state) {
        dvec<dvec<FinderBackup>> backupsStack;

        dvec<int> cis;
        cis.reserve(candidates.size());

        dhmap<u64, bool> hashMap;
        hashMap.init(candidates.size());
        REP(ci, candidates.size()) {
            cauto& c = candidates[ci];
            if (!state.ContainsSwitch(c.hash)) {
                if (hashMap.contains(c.hash)) {
                    continue;
                }
                hashMap.set(c.hash, true);
                cis.pushR() = ci;
            }
        }

        if (state.arrA_.as_.size() == LA) {
            cis.clear();
        }

        double predictLoopCount = LA / (double)LB;
        int CheckCandidateCount = 1 + int(2000 / predictLoopCount) + LB;

        int prevTotalCost = 10000;

        int removeSwitchCount = 0;
        TArr<int> tisUseCandidateCount(T, 0);
        int loopCount = 0;

        dvec<double> candidatesTargetCountScore(candidates.size());
        dvec<double> candidatesBaseScore(candidates.size());
        for (int ci : cis) {
            auto& c = candidates[ci];

            DEF_DOUBLE_PARAM(EvalCandidateTisBase);
            DEF_DOUBLE_PARAM(EvalCandidateTisExp);
            candidatesTargetCountScore[ci] = EvalCandidateTisBase * pow(c.tis.size(), EvalCandidateTisExp);

            double e = 0;

            candidatesBaseScore[ci] = e;
        }

        bool updateEval = true;
        while (!cis.empty()) {
            ++loopCount;

            if (updateEval) {
                for (int ci : cis) {
                    auto& c = candidates[ci];

                    DEF_DOUBLE_PARAM(EvalCandidateProspectSize);
                    DEF_DOUBLE_PARAM(EvalCandidateCommonSize);
                    DEF_DOUBLE_PARAM(EvalCandidateTerminalLeft);
                    DEF_DOUBLE_PARAM(EvalCandidateCoverVertex);

                    double e = candidatesBaseScore[ci];

                    int usedCount = 0;
                    for (int ti : c.tis) {
                        if (tisUseCandidateCount[ti] > 0) {
                            ++usedCount;
                        }
                    }
                    e += candidatesTargetCountScore[ci] * c.tis.size() / double(usedCount + 1);


                    BArr<int> vs;
                    REP(i, min(LB, c.vs.size())) {
                        vs.push(c.vs[i]);
                    }
                    int prospectSize;
                    int commonSize;
                    int terminalLeft;
                    int coverVertexCount;
                    state.CalcPushedProspectArraySize(vs, prospectSize, commonSize, terminalLeft, coverVertexCount);
                    e -= prospectSize * EvalCandidateProspectSize;
                    e += commonSize * EvalCandidateCommonSize;
                    e -= terminalLeft * EvalCandidateTerminalLeft;
                    e += coverVertexCount * EvalCandidateCoverVertex;


                    c.eval = e;
                }
            }
            else {
                updateEval = true;
            }

            if (cis.size() > CheckCandidateCount) {
                nth_element(cis.begin(), cis.begin() + CheckCandidateCount, cis.end(),
                            [&](int a, int b) { return candidates[a].eval > candidates[b].eval; });
            }
            sort(cis.begin(), cis.begin() + min(cis.size(), CheckCandidateCount),
                 [&](int a, int b) { return candidates[a].eval > candidates[b].eval; });

            int bestCandidateD = INT_MAX;
            double bestCandidateEval = 0;  
            int bestCi = -1;
            int bestCii = -1;
            REP(cii, min(cis.size(), CheckCandidateCount)) {
                int ci = cis[cii];

                int prevArrSize = state.arrA_.as_.size();
                int prevTerminalLeft = state.terminalLeft_;

                backupsStack.clear();
                cauto& cand = candidates[ci];
                VASSERT(cand.vs.size() == LB);
                state.PushSwitch(ci, vertexDists, cand.vs, backupsStack.pushR(), true);

                int afterArrSize = state.arrA_.as_.size();
                int afterTerminalLeft = state.terminalLeft_;

                int pushedSize = afterArrSize - prevArrSize;

                int totalD = state.EvalGreedy(vertexDists, candidates);

                double eval = 0;
                eval = (prevTotalCost - totalD) / (double)(pushedSize);
                if (eval > bestCandidateEval) {
                    bestCandidateEval = eval;
                    bestCandidateD = totalD;
                    bestCi = ci;
                    bestCii = cii;
                }
                while (!backupsStack.empty()) {
                    state.PopSwitch(backupsStack.back());
                    backupsStack.pop();
                }

            }

            if (bestCi < 0) {
                RREP(cii, min(cis.size(), CheckCandidateCount)) {
                    cis.removeSwap(cii);
                }

                updateEval = false;
            }
            else {
                cauto& cand = candidates[bestCi];


                VASSERT(cand.vs.size() == LB);
                state.FixPushSwitch(bestCi, vertexDists, cand.vs, backupsStack.pushR(), true);
                backupsStack.clear();


                if (removeSwitchCount == 0 && state.ProspectArraySize() > LA / 2) {
                    state.RemoveUnuseSwitch(vertexDists, candidates);
                    ++removeSwitchCount;
                }
                else if (removeSwitchCount < 8 && state.ProspectArraySize() >= LA) {
                    state.RemoveUnuseSwitch(vertexDists, candidates);
                    ++removeSwitchCount;
                }

                prevTotalCost = bestCandidateD;
                cauto& c = candidates[bestCi];
                for (int ti : c.tis) {
                    ++tisUseCandidateCount[ti];
                }

                if (state.ProspectArraySize() >= LA) {
                    break;
                }
                cis.removeSwap(cis.find(bestCi));
            }
        }



        state.FinalizeA(vertexDists, candidates);
    }
};


struct Main {
    void Run(int argc, const char* argv[]) {
        server.InitInput(timer_);

        timer_.StartMs(TIME_LIMIT);

        static Solver solver;
        solver.Run();

        cerr << "time " << timer_.ElapseTimeMs() << endl;
        server.Finalize();

    }
};
