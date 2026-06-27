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

#define USE_COMP_AREA_RATE 1		

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

#else
#include <bits/stdc++.h>
#endif

using namespace std;


#define FOR(i, s, e) for (int i = int(s); i < int(e); ++i)
#define RFOR(i, s, e) for (int i = int(e) - 1; i >= int(s); --i)
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
	inline Xor64(uint64_t seed = 0) {
		x = 88172645463325252ULL + seed;
	}

	inline uint64_t Get64() {
		x ^= (x << 7);
		return x ^= (x >> 9);
	}

	inline result_type operator()() {
		return Get64() & 0xFFFFFFFF;
	}


	template <class T>
	inline T operator()(T r) {
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
void Shuffle(IT&& begin, IT&& end, Xor64& rand) {
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





constexpr int W = 1000;
constexpr int WW = W*W;
int D;
int N;



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
	PARAM_DOUBLE(StartTemp, 111.83343790391366, 50.0, 150);PARAM_LOWER(0.0);
	PARAM_DOUBLE(EndTemp, 0.1955542982979398, 0.0, 1.0);PARAM_LOWER(0.0);




	START_TUNING;
	PARAM_INT(Trans0, 16, 10, 25);PARAM_LOWER(0);PARAM_UPPER(100);
	PARAM_INT(Trans1, 79, 58, 98);PARAM_LOWER(0);PARAM_UPPER(100);
	PARAM_INT(Trans2, 99, 70, 120);PARAM_LOWER(0);PARAM_UPPER(200);
	PARAM_INT(Trans3, 17, 10, 30);PARAM_LOWER(0);PARAM_UPPER(100);
	PARAM_INT(Trans6, 1, 0, 6);PARAM_LOWER(0);PARAM_UPPER(100);
	PARAM_INT(Trans7, 5, 0, 5);PARAM_LOWER(0);PARAM_UPPER(100);
	PARAM_INT(Trans4, 0, 0, 5);PARAM_LOWER(0);PARAM_UPPER(100);
	PARAM_INT(Trans5, 87, 60, 100);PARAM_LOWER(0);PARAM_UPPER(200);
	PARAM_INT(Trans9, 45, 30, 60);PARAM_LOWER(0);PARAM_UPPER(100);

	END_TUNING;

	PARAM_DOUBLE(CompressRate, 0.92, 0.0, 1.0);PARAM_LOWER(0.0);PARAM_UPPER(1.0);


	PARAM_INT(PullMaxCount, 1, 0, 5);PARAM_LOWER(0);PARAM_UPPER(50);
	PARAM_INT(PullFitMaxCount, 1, 0, 5);PARAM_LOWER(0);PARAM_UPPER(50);
	PARAM_CATEGORY(TempType, 0, 0, 1, 2);
	PARAM_DOUBLE(TempPow, 3.136385651073013, 1.5, 2.5);PARAM_LOWER(0.0);

} HP;



struct Rect {
	int l;
	int t;
	int r;
	int b;

	bool operator == (const Rect& o) const {
		return
			l == o.l &&
			t == o.t &&
			r == o.r &&
			b == o.b;
	}
	bool operator != (const Rect& o) const {
		return !(*this == o);
	}

	int Size() const {
		return (r - l) * (b - t);
	}

	void Output(ostream& os) const {
		os << t << " " << l << " " << b << " " << r;
	}
};

struct IOResult {
	vector<vector<Rect>> rects_;

	void Init() {
		rects_.resize(D);
		REP(d, D) {
			rects_[d].resize(N);
		}
	}

	void Sort() {
		REP(d, rects_.size()) {
			sort(ALL(rects_[d]), [&](const Rect& a, const Rect& b) {
				return a.Size() < b.Size();
				});
		}
	}

	void Output(ostream& os) const {
		REP(d, rects_.size()) {
			REP(k, rects_[d].size()) {
				rects_[d][k].Output(os);
				os << endl;
			}
		}
	}
};




template <class T>
struct Mat {
private:
	vector<T> vec_;
	int rows_;
	int cols_;

public:
	void clear() {
		vec_.clear();
		rows_ = 0;
		cols_ = 0;
	}

	int rows() const {
		return rows_;
	}
	int cols() const {
		return cols_;
	}

	void resize(int rows, int cols) {
		vec_.resize(rows * cols);
		rows_ = rows;
		cols_ = cols;
	}
	const T& operator()(int r, int c) const {
		VASSERT(r >= 0);
		VASSERT(c >= 0);
		VASSERT(r < rows_);
		VASSERT(c < cols_);
		return vec_[r * cols_ + c];
	}
	T& operator()(int r, int c) {
		VASSERT(r >= 0);
		VASSERT(c >= 0);
		VASSERT(r < rows_);
		VASSERT(c < cols_);
		return vec_[r * cols_ + c];
	}

};

template <class T>
void Remove(vector<T>& vec, int i) {
    vec.erase(vec.begin() + i);
}

template <class T>
void RemoveSwap(vector<T>& vec, int i) {
    if (i + 1 < vec.size()) {
        vec[i] = vec.back();
    }
    vec.pop_back();
}

template <class T>
void FindRemoveSwap(vector<T>& vec, const T& value) {
    int i = int(find(ALL(vec), value) - vec.begin());
    if (i + 1 < vec.size()) {
        vec[i] = vec.back();
    }
    vec.pop_back();
}

template <class T>
const T& Choice(const vector<T>& vec, Xor64& r) {
    return vec[r((int)vec.size())];
}

template <class T>
T ChoicePopSwap(vector<T>& vec, Xor64& r) {
    int i = r((int)vec.size());
    T dst = vec[i];
    RemoveSwap(vec, i);
    return dst;
}

template <class T>
void Insert(vector<T>& vec, int i, const T& value) {
    vec.insert(vec.begin() + i, value);
}



s64 CalcScore(const vector<vector<int>>& A, const vector<vector<Rect>>& rects, s64& areaCost, s64& changeCost) {
    areaCost = 0;
    changeCost = 0;

    struct Change {
        int t0;
        int t1;
        int t2;
        int t3;
        bool t4;
        bool operator ==(const Change& r) const {
            return t0 == r.t0 &&
                t1 == r.t1 &&
                t2 == r.t2 &&
                t3 == r.t3 &&
                t4 == r.t4;
        }
    };
    vector<Change> change;
    set<pair<int, int>> hs;
    set<pair<int, int>> vs;
    REP(d, A.size()) {
        REP(p, A[d].size()) {
            REP(q, p) {
                if (min(rects[d][p].b, rects[d][q].b) > max(rects[d][p].t, rects[d][q].t)
                    && min(rects[d][p].r, rects[d][q].r) > max(rects[d][p].l, rects[d][q].l))
                {
                    cauto& rectP = rects[d][p];
                    cauto& rectQ = rects[d][q];

                    VABORT();
                }
            }
        }
        set<pair<int, int>> hs2;
        set<pair<int, int>> vs2;
        REP(k, N) {
            cauto& rect = rects[d][k];
            int i0 = rect.t;
            int j0 = rect.l;
            int i1 = rect.b;
            int j1 = rect.r;
            int b = (i1 - i0) * (j1 - j0);
            if (A[d][k] > b) {
                areaCost += 100 * s64(A[d][k] - b);
            }
            FOR(j, j0, j1) {
                if (i0 > 0) {
                    hs2.insert(make_pair(i0, j));
                }
                if (i1 < W) {
                    hs2.insert(make_pair(i1, j));
                }
            }
            FOR(i, i0, i1) {
                if (j0 > 0) {
                    vs2.insert(make_pair(j0, i));
                }
                if (j1 < W) {
                    vs2.insert(make_pair(j1, i));
                }
            }
        }
        if (d > 0) {
            for (auto [i, j] : hs) {
                if (!hs2.contains(make_pair(i, j))) {
                    changeCost += 1;
                    if (d + 1 == rects.size()) {
                        if (change.size() > 0 && change[change.size() - 1] == Change{ i, change[change.size() - 1].t1, i, j, false }) {
                            change.back().t3 += 1;
                        }
                        else {
                            change.emplace_back(i, j, i, j + 1, false);
                        }
                    }
                }
            }
            for (auto[j, i] : vs) {
                if (!vs2.contains(make_pair(j, i))) {
                    changeCost += 1;
                    if (d + 1 == rects.size()) {
                        if (change.size() > 0 && change[change.size() - 1] == Change{ change[change.size() - 1].t0, j, i, j, false }) {
                            change.back().t2 += 1;
                        }
                        else {
                            change.emplace_back(i, j, i + 1, j, false);
                        }
                    }
                }
            }
            for (auto [i, j] : hs2) {
                if (!hs.contains(make_pair(i, j))) {
                    changeCost += 1;
                    if (d + 1 == rects.size()) {
                        if (change.size() > 0 && change[change.size() - 1] == Change{ i, change[change.size() - 1].t1, i, j, true }) {
                            change.back().t3 += 1;
                        }
                        else {
                            change.emplace_back(i, j, i, j + 1, true);
                        }
                    }
                }
            }
            for (auto [j, i] : vs2) {
                if (!vs.contains(make_pair(j, i))) {
                    changeCost += 1;
                    if (d + 1 == rects.size()) {
                        if (change.size() > 0 && change[change.size() - 1] == Change{ change[change.size() - 1].t0, j, i, j, true }) {
                            change.back().t2 += 1;
                        }
                        else {
                            change.emplace_back(i, j, i + 1, j, true);
                        }
                    }
                }
            }
        }
        hs = hs2;
        vs = vs2;
    }
    return areaCost + changeCost + 1;
}

s64 CalcScore(const vector<vector<int>>& A, const vector<vector<Rect>>& rects) {
    s64 areaCost;
    s64 changeCost;
    return CalcScore(A, rects, areaCost, changeCost);
}



s64 CalcAreaCost(const vector<int>& A, const vector<Rect>& rects) {
    s64 cost = 0;
    REP(k, N) {
        int loss = A[k] - rects[k].Size();
        if (loss > 0) {
            cost += loss;
        }
    }
    return cost * 100;
}
s64 CalcAreaCost(const vector<vector<int>>& A, const vector<vector<Rect>>& rects) {
    s64 cost = 0;
    REP(d, D) {
        REP(k, N) {
            int loss = A[d][k] - rects[d][k].Size();
            if (loss > 0) {
                cost += loss;
            }
        }
    }
    return cost * 100;
}

struct Line {
    int pos_;					
    array<int, 2> edges_;		

    bool operator <(const Line& r) const {
        return pos_ < r.pos_;
    }
};
struct DayLine {
    vector<Line> hLines_;		
    vector<Line> vLines_;		
};
struct ResultLines {
    vector<DayLine> lines_;		
};

s64 CalcTotalSize(const vector<Line>& lines) {
    s64 size = 0;
    for (cauto& line : lines) {
        size += line.edges_[1] - line.edges_[0];
    }
    return size;
}
s64 CalcTotalSize(const DayLine& dayLine) {
    return CalcTotalSize(dayLine.hLines_) + CalcTotalSize(dayLine.vLines_);
}

s64 CalcCommonSize(const vector<Line>& prev, const vector<Line>& cur) {
    int pi = 0;
    s64 commonSize = 0;
    REP(ci, cur.size()) {
        cauto& curLine = cur[ci];
        int curPos = curLine.pos_;

        int under = pi - 1;
        int equalOver = (int)prev.size();
        while (equalOver - under > 1) {
            int mid = (under + equalOver) / 2;
            if (prev[mid].pos_ < curPos) {
                under = mid;
            }
            else {
                equalOver = mid;
            }
        }

        pi = equalOver;

        while (pi < prev.size() && prev[pi].pos_ == curPos) {
            cauto& prevLine = prev[pi];
            int common = min(curLine.edges_[1], prevLine.edges_[1]) - max(curLine.edges_[0], prevLine.edges_[0]);
            if (common > 0) {
                commonSize += common;
            }
            ++pi;
        }

        pi = equalOver; 
    }
    return commonSize;
}
s64 CalcCommonSize(const DayLine& prev, const DayLine& cur) {
    return CalcCommonSize(prev.hLines_, cur.hLines_) + CalcCommonSize(prev.vLines_, cur.vLines_);
}

s64 CalcChangeCost(const DayLine& lineA, const DayLine& lineB) {

    s64 totalSize = 0;
    s64 commonSize = 0;
    totalSize += CalcTotalSize(lineA.hLines_);
    totalSize += CalcTotalSize(lineA.vLines_);
    totalSize += CalcTotalSize(lineB.hLines_);
    totalSize += CalcTotalSize(lineB.vLines_);
    commonSize += CalcCommonSize(lineA.hLines_, lineB.hLines_);
    commonSize += CalcCommonSize(lineA.vLines_, lineB.vLines_);

    s64 cost = totalSize - commonSize * 2;
    return cost;
}

s64 CalcChangeCost(const vector<DayLine>& lines) {

    s64 totalSize = 0;
    s64 commonSize = 0;
    totalSize += CalcTotalSize(lines[0].hLines_);
    totalSize += CalcTotalSize(lines[0].vLines_);

    FOR(d, 1, D) {

        totalSize += CalcTotalSize(lines[d].hLines_) * ((d + 1 < D) ? 2 : 1);
        totalSize += CalcTotalSize(lines[d].vLines_) * ((d + 1 < D) ? 2 : 1);
        commonSize += CalcCommonSize(lines[d - 1].hLines_, lines[d].hLines_);
        commonSize += CalcCommonSize(lines[d - 1].vLines_, lines[d].vLines_);

    }

    s64 cost = totalSize - commonSize * 2;
    return cost;
}

s64 CalcScore(const vector<vector<int>>& A, const vector<vector<Rect>>& rects, const vector<DayLine>& lines) {
    return CalcAreaCost(A, rects) + CalcChangeCost(lines) + 1;
}

bool IsValid(const vector<int>& A, const array<Rect, 2>& rects, const array<vector<int>, 2>& dividedKs) {
    REP(ci, 2) {
        int areaSize = rects[ci].Size();
        int requireSize = 0;
        for (int k : dividedKs[ci]) {
            requireSize += A[k];
        }
        if (areaSize < requireSize) {
            return false;
        }
    }
    return true;
}


void DivideSum(const vector<int>& values, const vector<int>& idxs, const array<double, 2>& weights, array<vector<int>, 2>& dividedIdxs, array<int, 2>& sums) {
    dividedIdxs[0].clear();
    dividedIdxs[1].clear();

    static vector<int> is;
    is = idxs;
    sort(ALL(is), [&](int a, int b) {
        return values[b] < values[a];
        });

    double targetRate = weights[0] / (weights[0] + weights[1]);

    sums = {};
    int total = 0;
    int size = (int)is.size();
    REP(idx, size) {
        int i = is[idx];
        int v = values[i];

        if (idx + 1 == size) {
            if (dividedIdxs[0].empty()) {
                sums[0] += v;
                dividedIdxs[0].emplace_back(i);
                break;
            }
            if (dividedIdxs[1].empty()) {
                sums[1] += v;
                dividedIdxs[1].emplace_back(i);
                break;
            }
        }

        double div = 1.0 / (double)(total + v);
        double rate0 = (sums[0] + v) * div;
        double rate1 = (sums[0]) * div;
        if (fabs(rate0 - targetRate) < fabs(rate1 - targetRate)) {
            sums[0] += v;
            dividedIdxs[0].emplace_back(i);
        }
        else {
            sums[1] += v;
            dividedIdxs[1].emplace_back(i);
        }
        total += v;
    }


    VASSERT(!dividedIdxs[0].empty());
    VASSERT(!dividedIdxs[1].empty());
}

bool CanDivRect(const Rect& rect, bool vert, int mid) {
    if (vert) {
        if (rect.l >= mid || rect.r <= mid) {
            return false;
        }
    }
    else {
        if (rect.t >= mid || rect.b <= mid) {
            return false;
        }
    }
    return true;
}

void DivRect(const Rect& rect, bool vert, int mid, array<Rect, 2>& dividedRects) {
    if (vert) {
        VASSERT(mid > rect.l || mid < rect.r);
        dividedRects = {
            Rect{rect.l, rect.t, mid, rect.b},
            Rect{mid, rect.t, rect.r, rect.b},
        };
    }
    else {
        VASSERT(mid > rect.t || mid < rect.b);
        dividedRects = {
            Rect{rect.l, rect.t, rect.r, mid},
            Rect{rect.l, mid, rect.r, rect.b},
        };
    }
}

bool CalcVert(const Rect& rect) {
    return (rect.r - rect.l > rect.b - rect.t);
}


bool CalcVertRange(const Rect& rect, const array<int, 2>& sums, bool& vert, int& lower, int& upper) {
    vert = CalcVert(rect);
    if (vert) {
        int h = rect.b - rect.t;
        lower = rect.l + (sums[0] + h - 1) / h;
        upper = rect.r - (sums[1] + h - 1) / h;
    }
    else {
        int w = rect.r - rect.l;
        lower = rect.t + (sums[0] + w - 1) / w;
        upper = rect.b - (sums[1] + w - 1) / w;
    }
    if (lower > upper) {
        return false;
    }
    return true;
}

bool CalcDivRect(const Rect& rect, const array<int, 2>& sums, bool& vert, int& mid, array<Rect, 2>& dividedRects) {
    double rate0 = sums[0] / (double)(sums[0] + sums[1]);

    int l = rect.l;
    int t = rect.t;
    int r = rect.r;
    int b = rect.b;

    vert = CalcVert(rect);

    if (vert) {
        mid = l + (int)round((r - l) * rate0);
        chmax(mid, l + 1);
        chmin(mid, r - 1);
        if (mid <= l || mid >= r) {
            return false;
        }
        dividedRects = {
            Rect{l, t, mid, b},
            Rect{mid, t, r, b},
        };
    }
    else {
        mid = t + (int)round((b - t) * rate0);
        chmax(mid, t + 1);
        chmin(mid, b - 1);
        if (mid <= t || mid >= b) {
            return false;
        }
        dividedRects = {
            Rect{l, t, r, mid},
            Rect{l, mid, r, b},
        };
    }
    return true;
}

void MakeMaxA(const vector<vector<int>>& A, vector<int>& maxA) {
    maxA = A[0];
    FOR(d, 1, A.size()) {
        REP(k, A[d].size()) {
            chmax(maxA[k], A[d][k]);
        }
    }
}

bool CalcAssign(const vector<int>& A, const vector<Rect>& rects, vector<vector<int>>& kss) {
    kss.resize(rects.size());
    for (auto& ks : kss) {
        ks.clear();
    }

    static vector<int> rectSizes;
    rectSizes.resize(rects.size());
    REP(ri, rects.size()) {
        rectSizes[ri] = rects[ri].Size();
    }

    static vector<int> ris;
    ris.resize(rects.size());
    iota(ALL(ris), 0);
    sort(ALL(ris), [&](int a, int b) {
        return rectSizes[a] > rectSizes[b];
        });

    static vector<int> sums;
    sums.assign(rects.size(), 0);
    RREP(k, N) {
        int a = A[k];
        int bestOver = INT_MAX;
        int bestRi = -1;
        for (int ri : ris) {
            int over = sums[ri] + a - rectSizes[ri];
            if (over <= 0) {
                bestRi = ri;
                break;
            }
            if (over < bestOver) {
                bestOver = over;
                bestRi = ri;
            }
        }
        VASSERT(bestRi >= 0);
        sums[bestRi] += a;
        kss[bestRi].emplace_back(k);
    }

    REP(to, kss.size()) {
        auto& toKs = kss[to];
        if (!toKs.empty()) {
            continue;
        }

        int bestOver = INT_MAX;
        int bestFrom = -1;
        int bestI = -1;
        REP(from, kss.size()) {
            if (from == to) {
                continue;
            }
            auto& fromKs = kss[from];
            if (fromKs.size() <= 1) {
                continue;
            }

            REP(i, fromKs.size()) {
                int k = fromKs[i];
                int over = A[k] - rectSizes[to];
                if (over <= 0) {
                    bestFrom = from;
                    bestI = i;
                    break;
                }
                if (over < bestOver) {
                    bestOver = over;
                    bestFrom = from;
                    bestI = i;
                }
            }
        }
        if (bestFrom >= 0) {
            auto& fromKs = kss[bestFrom];
            int k = fromKs[bestI];
            sums[bestFrom] -= A[k];
            sums[to] += A[k];
            toKs.emplace_back(k);
            RemoveSwap(fromKs, bestI);
        }
        else {
            return false;
        }
    }



    return true;
}

void DivSize(const Rect& rect, const vector<int>& sizes, const vector<int>& idxs, vector<Rect>& result) {
    array<vector<int>, 2> dividedIdxs;
    array<int, 2> sums;
    DivideSum(sizes, idxs, { 0.5, 0.5 }, dividedIdxs, sums);

    bool vert;
    int mid;
    array<Rect, 2> dividedRect;
    CalcDivRect(rect, sums, vert, mid, dividedRect);

    REP(i, 2) {
        if (dividedIdxs[i].size() == 1) {
            int idx = dividedIdxs[i][0];
            result[idx] = dividedRect[i];
        }
        else {
            DivSize(dividedRect[i], sizes, dividedIdxs[i], result);
        }
    }
}



struct Item {
    int di;
    int k;
    int size;

    bool operator < (const Item& r) const {
        return size > r.size;
    }
};

void CalcDivM(vector<vector<int>>& A, const vector<vector<int>>& srcDaysKs, int beginD, int areaCount, vector<vector<vector<int>>>& dividedDayKs, vector<int>& areasMaxDaySize) {
    dividedDayKs.resize(areaCount);
    for (auto& daysKs : dividedDayKs) {
        daysKs.resize(srcDaysKs.size());
        for (auto& ks : daysKs) {
            ks.clear();
        }
    }

    static vector<Item> items;
    items.reserve(N * D);
    items.clear();
    REP(di, srcDaysKs.size()) {
        int d = beginD + di;
        VASSERT(srcDaysKs[di].size() >= 2);		
        for (int k : srcDaysKs[di]) {
            items.emplace_back();
            auto& item = items.back();
            item.di = di;
            item.k = k;
            item.size = A[d][k];
        }
    }
    sort(ALL(items));

    static vector<vector<int>> areasSums;
    items.reserve(N);
    areasSums.resize(areaCount);		
    REP(ai, areaCount) {
        areasSums[ai].reserve(D);
        areasSums[ai].assign(srcDaysKs.size(), 0);
    }
    areasMaxDaySize.assign(areaCount, 0);

    for (cauto& item : items) {
        int bestAi = -1;
        double bestEval = 1e100;		
        REP(ai, areaCount) {
            int newAreaSum = areasSums[ai][item.di] + item.size;
            int appendSize = newAreaSum - areasMaxDaySize[ai];
            chmax(appendSize, 0);
            int spaceSize = areasMaxDaySize[ai] - newAreaSum;
            chmax(spaceSize, 0);
            bool appendOver = areasSums[ai][item.di] > 0 && appendSize > 0;

            double eval = (appendOver ? ((double)WW * WW * WW) : 0) + (double)appendSize * WW * WW + (double)spaceSize * WW + newAreaSum;
            if (eval < bestEval) {
                bestEval = eval;
                bestAi = ai;
            }
        }
        VASSERT(bestAi >= 0);

        areasSums[bestAi][item.di] += item.size;
        chmax(areasMaxDaySize[bestAi], areasSums[bestAi][item.di]);
        dividedDayKs[bestAi][item.di].emplace_back(item.k);
    }

    bool updated = false;
    REP(di, srcDaysKs.size()) {
        int d = beginD + di;

        REP(toAi, areaCount) {
            if (areasSums[toAi][di] == 0) {
                int bestFromAi = -1;
                int bestKi = -1;
                double bestEval = 1e100;		
                REP(fromAi, areaCount) {
                    if (fromAi == toAi) {
                        continue;
                    }
                    cauto& ks = dividedDayKs[fromAi][di];
                    if (ks.size() <= 1) {
                        continue;
                    }
                    REP(ki, ks.size()) {
                        int k = ks[ki];
                        int appendSize = A[d][k] - areasMaxDaySize[toAi];
                        chmax(appendSize, 0);

                        double eval = (double)appendSize * WW * WW - (double)dividedDayKs[fromAi][di].size() * WW - A[d][k];
                        if (eval < bestEval) {
                            bestEval = eval;
                            bestFromAi = fromAi;
                            bestKi = ki;
                        }
                    }
                }
                VASSERT(bestFromAi >= 0);
                int k = dividedDayKs[bestFromAi][di][bestKi];
                RemoveSwap(dividedDayKs[bestFromAi][di], bestKi);
                dividedDayKs[toAi][di].emplace_back(k);

                areasSums[bestFromAi][di] -= A[d][k];
                areasSums[toAi][di] += A[d][k];
                updated = true;
            }
        }
    }


    REP(di, srcDaysKs.size()) {
        int d = beginD + di;

        REP(fromAi, areaCount) {
            auto& fromKs = dividedDayKs[fromAi][di];
            if (fromKs.size() < 3) {
                continue;
            }
            REP(toAi, areaCount) {
                auto& toKs = dividedDayKs[toAi][di];
                if (toKs.size() + 1 >= fromKs.size()) {
                    continue;
                }

                REP(ki, fromKs.size()) {
                    int k = fromKs[ki];
                    int appendSize = areasSums[toAi][di] + A[d][k] - areasMaxDaySize[toAi];
                    if (appendSize <= 0) {
                        RemoveSwap(fromKs, ki);
                        toKs.emplace_back(k);

                        areasSums[fromAi][di] -= A[d][k];
                        areasSums[toAi][di] += A[d][k];
                        updated = true;
                        break;
                    }
                }
            }
        }
    }


    if (updated) {
        areasMaxDaySize.assign(areaCount, 0);
        REP(ai, areaCount) {
            int maxSum = 0;
            REP(di, srcDaysKs.size()) {
                int d = beginD + di;
                int sum = 0;
                for (int k : dividedDayKs[ai][di]) {
                    sum += A[d][k];
                }
                chmax(maxSum, sum);
            }
            areasMaxDaySize[ai] = maxSum;
        }
    }

}

#define USE_COMPRESS 1

struct IOServer {
	bool compress_;
	vector<vector<int>> orgA_;
	vector<vector<int>> A_;
	vector<vector<int>> compDays_;
	int orgD_;
	vector<int> areaCostScale_;		



	void InitInput(ChronoTimer& timer) {
		istream& is = cin;
		int dummy;
		is >> dummy;
		timer.Init();

		is >> D >> N;
		A_.resize(D, vector<int>(N));
		REP(d, D) {
			REP(k, N) {
				is >> A_[d][k];
			}
		}

		vector<vector<int>> compA;
		vector<vector<int>> compDays;

		vector<int> A;
		vector<int> ds;
		int d = 0;
		while (d < D) {
			ds.clear();
			ds.emplace_back(d);

			A = A_[d];

			int d2 = d + 1;
			while (d2 < D) {
				int total = 0;
				REP(k, N) {
					total += max(A[k], A_[d2][k]);
				}
				if (total <= WW * HP.CompressRate) {		
					REP(k, N) {
						chmax(A[k], A_[d2][k]);
					}
					ds.emplace_back(d2);
					++d2;
				}
				else {
					break;
				}
			}
			d = d2;

			compA.emplace_back(move(A));
			compDays.emplace_back(move(ds));
		}

		if (compA.size() >= 3) {
			compress_ = true;
			orgA_ = move(A_);
			A_ = move(compA);
			compDays_ = move(compDays);
			orgD_ = D;
			D = (int)A_.size();

			areaCostScale_.resize(D);
			REP(d, D) {
				areaCostScale_[d] = (int)compDays_[d].size();
			}
		}
		else 
		{
			compress_ = false;
			areaCostScale_.assign(D, 1);
		}


	}

	void Output(IOResult& result) {
		ostream& os = cout;

		IOResult trueResult;
		if (compress_) {
			trueResult.rects_.resize(orgD_);
			for (auto& r : trueResult.rects_) {
				r.resize(N);
			}
			REP(d, D) {
				cauto& ds = compDays_[d];
				for (int trueD : ds) {
					trueResult.rects_[trueD] = result.rects_[d];
				}
			}
			trueResult.Output(os);
		}
		else {
			result.Output(os);
		}

	}

	void Finalize() {
	}
};
IOServer server;

Xor64 rand_;



template <class State, class SnapShot, class StateBackup>
struct SABackup {
private:
	State state_;
	SnapShot bestShot_;
	StateBackup backup_;

	double eval_;

	double startTemp_;
	double endTemp_;
	int tempType_;
	double tempPow_;

	double divTemp_;

	int stayBestCount_;				
	int rollbackStayBestCount_;
	int rollbackCount_;


public:
	State& GetState() {
		return state_;
	}
	SnapShot& GetBestShot() {
		return bestShot_;
	}
	StateBackup& GetStateBackup() {
		return backup_;
	}

	void Init(double startTemp, double endTemp, int tempType = 0, double tempPow = 2) {
		state_.Init();
		state_.Shot(bestShot_);
		eval_ = state_.EvalScore();

		startTemp_ = startTemp;
		endTemp_ = endTemp;
		tempType_ = 0;
		tempPow_ = 2;

		divTemp_ = 0;

		stayBestCount_ = 0;
		rollbackStayBestCount_ = INT_MAX;
		rollbackCount_ = 0;

	}

	void UpdateTemp(double timeRate) {
		double temp;
		if (tempType_ == 0) {
			temp = startTemp_ + (endTemp_ - startTemp_) * timeRate;
		}
		else if (tempType_ == 1) {
			temp = startTemp_ * pow(endTemp_ / startTemp_, timeRate);
		}
		else {
			temp = startTemp_ * pow(endTemp_ / startTemp_, pow(timeRate, tempPow_));
		}
		divTemp_ = 1.0 / temp;
	}

	void Check(int transIdx, bool success, StateBackup&& backup) {
		++stayBestCount_;

		if (success) {
			double e = state_.EvalScore();
			if (e >= eval_) {
				state_.Accepted(forward<StateBackup>(backup));
				eval_ = e;

				if (state_.RawScore() > bestShot_.RawScore()) {
					state_.Shot(bestShot_);
					stayBestCount_ = 0;
				}
			}
			else if (exp((e - eval_) * divTemp_) * UINT64_MAX > rand_.Get64()) {
				state_.Accepted(forward<StateBackup>(backup));
				eval_ = e;
			}
			else {
				state_.Revert(forward<StateBackup>(backup));
			}
		}
		else {
		}

		if (stayBestCount_ >= rollbackStayBestCount_) {
			++rollbackCount_;
			state_.Apply(bestShot_);
			stayBestCount_ = 0;
		}
	}


	template <class TransFunc>
	void Run(ChronoTimer& timer, TransFunc&& transFunc, int timerCheckBit = 0b111) {
		const auto startTime = timer.Now();
		int checkCount = 0;
		int ti = -1;
		double timeRate = 0;

		while (true) {
			if ((checkCount & timerCheckBit) == 0) {
				if (timer.IsOver()) {
					break;
				}
				timeRate = timer.CalcRate(startTime);
				UpdateTemp(timeRate);
			}
			++checkCount;

			bool success = transFunc(state_, timeRate, ti, backup_);

			Check(ti, success, move(backup_));

		}

	}
};



struct RandomTable {
	vector<int> table_;

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


vector<vector<int>> DifferencingMethod(const vector<int>& values, int M) {
	struct Elem {
		vector<vector<int>> part;		
		vector<int> sums;		
		int label;

		bool operator < (const Elem& e) const {
			return label < e.label;
		}
	};

	priority_queue<Elem> que;
	REP(i, values.size()) {
		int v = values[i];
		Elem e;
		e.part.resize(M);
		e.sums.resize(M);
		e.part.back().push_back(i);
		e.sums.back() = v;
		e.label = v;

		que.push(e);
	}

	while (que.size() > 1) {
		Elem e1 = que.top();
		que.pop();
		Elem e2 = que.top();
		que.pop();

		vector<pair<int, vector<int>>> tmp;
		REP(i, M) {
			vector<int> part;
			int sum = 0;
			{
				part = e1.part[i];
				sum = e1.sums[i];

				int ri = M - 1 - i;
				part.insert(part.end(), ALL(e2.part[ri]));
				sum += e2.sums[ri];
			}
			tmp.push_back(make_pair(sum, part));
		}
		sort(ALL(tmp));

		vector<vector<int>> part(M);
		vector<int> sums(M);
		REP(i, M) {
			part[i] = tmp[i].second;
			sums[i] = tmp[i].first;
		}
		int label = sums.back() - sums.front();

		Elem e;
		e.part = move(part);
		e.sums = move(sums);
		e.label = label;
		que.push(e);
	}

	Elem e = que.top();
	que.pop();
	return e.part;
}

void DifferencingMethod(const vector<int>& values, int M, vector<vector<int>>& dividedIdxs, vector<int>& sums) {
	struct Elem {
		vector<vector<int>> part;		
		vector<int> sums;		
		int label;

		bool operator < (const Elem& e) const {
			return label < e.label;
		}
	};

	priority_queue<Elem> que;
	REP(i, values.size()) {
		int v = values[i];
		Elem e;
		e.part.resize(M);
		e.sums.resize(M);
		e.part.back().push_back(i);
		e.sums.back() = v;
		e.label = v;

		que.push(e);
	}

	while (que.size() > 1) {
		Elem e1 = que.top();
		que.pop();
		Elem e2 = que.top();
		que.pop();

		vector<pair<int, vector<int>>> tmp;
		REP(i, M) {
			vector<int> part;
			int sum = 0;
			{
				part = e1.part[i];
				sum = e1.sums[i];

				int ri = M - 1 - i;
				part.insert(part.end(), ALL(e2.part[ri]));
				sum += e2.sums[ri];
			}
			tmp.push_back(make_pair(sum, part));
		}
		sort(ALL(tmp));

		vector<vector<int>> part(M);
		vector<int> sums(M);
		REP(i, M) {
			part[i] = tmp[i].second;
			sums[i] = tmp[i].first;
		}
		int label = sums.back() - sums.front();

		Elem e;
		e.part = move(part);
		e.sums = move(sums);
		e.label = label;
		que.push(e);
	}

	Elem e = que.top();
	que.pop();

	dividedIdxs = move(e.part);
	sums = move(e.sums);
}

vector<vector<int>> DifferencingMethod(const vector<int>& values, const vector<double>& weights) {
	int valueSum = 0;
	for (int v : values) {
		valueSum += v;
	}
	double weightSum = 0;
	for (double w : weights) {
		weightSum += w;
	}

	vector<double> normedWeights(weights.size());
	REP(i, weights.size()) {
		normedWeights[i] = weights[i] / weightSum;
	}

	vector<int> extendValues = values;
	REP(i, normedWeights.size()) {
		extendValues.emplace_back(valueSum + int(round(valueSum * (1.0 - normedWeights[i]))));
	}

	auto df = DifferencingMethod(extendValues, (int)weights.size());

	vector<vector<int>> dst(weights.size());
	REP(i, weights.size()) {
		int gi = -1;
		for (int idx : df[i]) {
			if (idx >= values.size()) {
				gi = idx - (int)values.size();
				break;
			}
		}
		VASSERT(gi >= 0);

		for (int idx : df[i]) {
			if (idx < values.size()) {
				dst[gi].emplace_back(idx);
			}
		}
	}
	return dst;
}

void DifferencingMethod(const vector<int>& values, const vector<double>& weights, vector<vector<int>>& dividedIdxs, vector<int>& sums) {
	int valueSum = 0;
	for (int v : values) {
		valueSum += v;
	}
	double weightSum = 0;
	for (double w : weights) {
		weightSum += w;
	}

	vector<double> normedWeights(weights.size());
	REP(i, weights.size()) {
		normedWeights[i] = weights[i] / weightSum;
	}

	vector<int> extendValues = values;
	REP(i, normedWeights.size()) {
		extendValues.emplace_back(valueSum + int(round(valueSum * (1.0 - normedWeights[i]))));
	}

	auto df = DifferencingMethod(extendValues, (int)weights.size());

	vector<vector<int>> dst(weights.size());
	sums.assign(weights.size(), 0);
	REP(i, weights.size()) {
		int gi = -1;
		for (int idx : df[i]) {
			if (idx >= values.size()) {
				gi = idx - (int)values.size();
				break;
			}
		}
		VASSERT(gi >= 0);

		for (int idx : df[i]) {
			if (idx < values.size()) {
				dst[gi].emplace_back(idx);
				sums[gi] += values[idx];
			}
		}
	}
	dividedIdxs = move(dst);
}


#define USE_AREA_SIZE_CHANGE 1

namespace bar {

	struct Div {
		int ai_;	
		int pos_;	
		int size_;	

		int prevDivIdx_;		
		int nextDivIdx_;		
	};

	struct Table {
		Mat<vector<int>> divs_;					
		vector<int> areaHeights_;				

		vector<vector<Div>> daysDivs_;			
		Mat<vector<s8>> dayAreaDivIdxs_;		
		vector<vector<s8>> daysDivIdxs;		

		vector<s64> daysAreaCost_;				
		s64 areaCost_;
		s64 changeCost_;

		void Init(const vector<vector<vector<int>>>& tbl, const vector<int>& areaHeights) {
			int areaCount = (int)tbl.size();

			areaHeights_ = areaHeights;

			cauto& A = server.A_;

			int bottom = 0;
			for (int h : areaHeights_) {
				bottom += h;
			}

			if (bottom < W) {
			}
			else if (bottom > W) {
				int over = bottom - W;
				while (over > 0) {
					int step = (over + areaCount - 1) / areaCount;
					REP(ai, areaCount) {
						int s = min(areaHeights_[ai] - 1, min(step, over));
						areaHeights_[ai] -= s;
						over -= s;
						if (over <= 0) {
							break;
						}
					}
				}
			}

			cauto& at = tbl;
			divs_.clear();
			divs_.resize(areaCount, D);

			daysDivs_.resize(D);
			dayAreaDivIdxs_.resize(D, areaCount);
			daysDivIdxs.resize(D);
			REP(d, D) {
				daysDivs_[d].reserve(N);
				daysDivs_[d].clear();
				daysDivIdxs[d].reserve(N);
				daysDivIdxs[d].clear();
			}

			auto AddDiv = [&](int ai, int d, int pos) {
				int prevPos = divs_(ai, d).empty() ? 0 : divs_(ai, d).back();
				divs_(ai, d).emplace_back(pos);

				int divIdx = (int)daysDivs_[d].size();
				dayAreaDivIdxs_(d, ai).emplace_back(divIdx);
				daysDivs_[d].emplace_back();
				auto& div = daysDivs_[d].back();
				div.ai_ = ai;
				div.pos_ = pos;
				div.size_ = (pos - prevPos) * areaHeights_[ai];

				if (pos != 0) {
					daysDivIdxs[d].emplace_back(divIdx);
				}
			};

			REP(ai, areaCount) {
				REP(d, D) {
					cauto& ks = at[ai][d];
					int x = 0;
					divs_(ai, d).reserve(N);
					AddDiv(ai, d, 0);
					REP(i, (int)ks.size() - 1) {
						int k = ks[i];
						int a = A[d][k];
						int h = areaHeights_[ai];
						int w = (a + h - 1) / h;
						int div = x + w;
						chmin(div, W - ((int)ks.size() - 1 - i));
						AddDiv(ai, d, div);
						x = div;
					}
					AddDiv(ai, d, W);
				}
			}

			REP(d, D) {
				cauto& divs = daysDivs_[d];
				auto& divIdxs = daysDivIdxs[d];
				sort(ALL(divIdxs), [&](int a, int b) {
					return divs[a].size_ < divs[b].size_;
					});
			}

			REP(d, D) {
				for (auto& div : daysDivs_[d]) {
					div.prevDivIdx_ = -1;
					div.nextDivIdx_ = -1;
				}
			}

			areaCost_ = CalcAreaCost(daysAreaCost_);
			changeCost_ = CalcChangeCost();

		}

		void FitK(int d) {
			cauto& divs = daysDivs_[d];
			auto& divIdxs = daysDivIdxs[d];

			RREP(i, N - 1) {
				if (divs[divIdxs[i]].size_ > divs[divIdxs[i + 1]].size_) {
					swap(divIdxs[i], divIdxs[i + 1]);

					FOR(j, i + 1, N - 1) {
						if (divs[divIdxs[j]].size_ > divs[divIdxs[j + 1]].size_) {
							swap(divIdxs[j], divIdxs[j + 1]);
						}
						else {
							break;
						}
					}
				}
			}
			s64 lossSize = 0;
			REP(k, N) {
				int size = divs[divIdxs[k]].size_;
				int a = server.A_[d][k];
				if (a > size) {
					lossSize += a - size;
				}
			}

			lossSize *= server.areaCostScale_[d];
			areaCost_ -= daysAreaCost_[d];
			daysAreaCost_[d] = lossSize * 100;
			areaCost_ += daysAreaCost_[d];
		}

		void FitK(int d, const vector<s8>& sortedIdxs, s64 dayAreaCost) {
			daysDivIdxs[d] = sortedIdxs;
			areaCost_ -= daysAreaCost_[d];
			daysAreaCost_[d] = dayAreaCost;
			areaCost_ += daysAreaCost_[d];
		}

		void MoveDiv(int ai, int d, int di, int newDiv) {
			VASSERT(di > 0);
			VASSERT(di + 1 < divs_(ai, d).size());

			auto& poss = divs_(ai, d);
			auto& divs = daysDivs_[d];
			auto& divIdxs = dayAreaDivIdxs_(d, ai);
			auto& div = divs[divIdxs[di]];

			changeCost_ += CalcRemovedChangeCost(ai, d, poss[di], div);
			changeCost_ += CalcAddedChangeCost(ai, d, newDiv, divIdxs[di], div);
			poss[di] = newDiv;

			VASSERT(poss[di - 1] < poss[di]);
			VASSERT(poss[di] < poss[di + 1]);

			{
				cauto& leftDiv = divs[divIdxs[di - 1]];		
				auto& targetDiv = divs[divIdxs[di]];		
				auto& rightDiv = divs[divIdxs[di + 1]];		
				targetDiv.pos_ = newDiv;
				targetDiv.size_ = (targetDiv.pos_ - leftDiv.pos_) * areaHeights_[ai];
				rightDiv.size_ = (rightDiv.pos_ - targetDiv.pos_) * areaHeights_[ai];

				VASSERT(leftDiv.pos_ < targetDiv.pos_);
				VASSERT(targetDiv.pos_ < rightDiv.pos_);
			}
		}

		void MoveArea(int d, int fromAi, int fromDi, int toAi, int toDi, int newDiv) {

			int divIdx = dayAreaDivIdxs_(d, fromAi)[fromDi];
			RemoveDiv(fromAi, d, fromDi);

			int toKi = toDi;
			AddDiv(toAi, d, divIdx, toDi, newDiv);
		}

		void RemoveDiv(int ai, int d, int di) {
			VASSERT(di > 0);
			VASSERT(di + 1 < divs_(ai, d).size());

			auto& poss = divs_(ai, d);
			auto& divs = daysDivs_[d];
			auto& divIdxs = dayAreaDivIdxs_(d, ai);

			changeCost_ += CalcRemovedChangeCost(ai, d, poss[di], divs[divIdxs[di]]);
			Remove(poss, di);
			Remove(divIdxs, di);

			cauto& leftDiv = divs[divIdxs[di - 1]];
			auto& rightDiv = divs[divIdxs[di]];
			rightDiv.size_ = (rightDiv.pos_ - leftDiv.pos_) * areaHeights_[ai];

		}

		void AddDiv(int ai, int d, int divIdx, int di, int newDiv) {
			VASSERT(di > 0);
			VASSERT(di < divs_(ai, d).size());

			auto& poss = divs_(ai, d);
			auto& divs = daysDivs_[d];
			auto& divIdxs = dayAreaDivIdxs_(d, ai);
			Insert(divs_(ai, d), di, newDiv);
			Insert(divIdxs, di, (s8)divIdx);

			auto& div = divs[divIdxs[di]];
			changeCost_ += CalcAddedChangeCost(ai, d, newDiv, divIdxs[di], div);

			cauto& leftDiv = divs[divIdxs[di - 1]];		
			auto& targetDiv = divs[divIdxs[di]];		
			auto& rightDiv = divs[divIdxs[di + 1]];		
			targetDiv.ai_ = ai;
			targetDiv.pos_ = newDiv;
			targetDiv.size_ = (targetDiv.pos_ - leftDiv.pos_) * areaHeights_[ai];
			rightDiv.size_ = (rightDiv.pos_ - targetDiv.pos_) * areaHeights_[ai];

			VASSERT(poss[di - 1] < poss[di]);
			VASSERT(poss[di] < poss[di + 1]);
			VASSERT(leftDiv.pos_ < targetDiv.pos_);
			VASSERT(targetDiv.pos_ < rightDiv.pos_);
		}

		void Move(int d, int divIdx, int ai, int pos) {
			RemoveIdx(d, divIdx);
			AddIdx(d, divIdx, ai, pos);
		}

		void RemoveIdx(int d, int divIdx) {
			cauto& div = daysDivs_[d][divIdx];
			VASSERT(div.pos_ > 0 && div.pos_ < W);
			RemoveDiv(div.ai_, d, FindDiv(div.ai_, d, div.pos_));
		}

		void AddIdx(int d, int divIdx, int ai, int pos) {
			VASSERT(pos > 0 && pos < W);
			cauto& div = daysDivs_[d][divIdx];
			cauto& ds = divs_(ai, d);
			auto it = lower_bound(ALL(ds), pos);
			int di = int(it - ds.begin());
			AddDiv(ai, d, divIdx, di, pos);
		}

		void ChangeAreaSize(int ai, int newH) {
			VASSERT(areaHeights_[ai] != newH);

			REP(d, D) {
				auto& divs = daysDivs_[d];
				cauto& idxs = dayAreaDivIdxs_(d, ai);
				FOR(i, 1, idxs.size()) {
					int idx = idxs[i];
					auto& div = divs[idx];
					VASSERT(div.ai_ == ai);
					if (div.pos_ < W) {
						changeCost_ += CalcAreaSizeChangeCost(ai, d, div.pos_, newH, div);
					}

					cauto& prevDiv = divs[idxs[i - 1]];
					div.size_ = (div.pos_ - prevDiv.pos_) * newH;
				}
			}
			areaHeights_[ai] = newH;
		}

		int CalcRemovedChangeCost(int ai, int d, int pos, Div& div) {
			const int h = areaHeights_[ai];
			int cost = 0;

			if (d > 0) {
				if (div.prevDivIdx_ >= 0) {
					cost += h;

					int nd = d - 1;
					auto& ndiv = daysDivs_[nd][div.prevDivIdx_];
					ndiv.nextDivIdx_ = -1;
					div.prevDivIdx_ = -1;
				}
				else {
					cost -= h;
				}
			}
			if (d + 1 < D) {
				if (div.nextDivIdx_ >= 0) {
					cost += h;

					int nd = d + 1;
					auto& ndiv = daysDivs_[nd][div.nextDivIdx_];
					ndiv.prevDivIdx_ = -1;
					div.nextDivIdx_ = -1;
				}
				else {
					cost -= h;
				}
			}

			return cost;
		}

		int CalcAddedChangeCost(int ai, int d, int pos, int divIdx, Div& div) {
			const int h = areaHeights_[ai];
			int cost = 0;

			auto Check = [&](int nd) {
				cauto& divs = divs_(ai, nd);
				int i = FindDiv(ai, nd, pos);
				if (i >= 0) {
					auto& nDivIdxs = dayAreaDivIdxs_(nd, ai);
					if (nd < d) {
						VASSERT(div.prevDivIdx_ == -1);
						daysDivs_[nd][nDivIdxs[i]].nextDivIdx_ = divIdx;
						div.prevDivIdx_ = nDivIdxs[i];
					}
					else {
						VASSERT(div.nextDivIdx_ == -1);
						daysDivs_[nd][nDivIdxs[i]].prevDivIdx_ = divIdx;
						div.nextDivIdx_ = nDivIdxs[i];
					}
					cost -= h;
				}
				else {
					cost += h;
				}
			};

			if (d > 0) {
				Check(d - 1);
			}
			if (d + 1 < D) {
				Check(d + 1);
			}

			return cost;
		}

		int CalcAreaSizeChangeCost(int ai, int d, int pos, int newH, Div& div) const {
			const int oldH = areaHeights_[ai];
			const int appendCost = newH - oldH;
			int cost = 0;

			if (d > 0) {
				if (div.prevDivIdx_ < 0) {
					cost += appendCost;
				}
			}
			if (d + 1 < D) {
				if (div.nextDivIdx_ < 0) {
					cost += appendCost;
				}
			}

			return cost;
		}

		int FindDiv(int ai, int d, int pos) const {
			cauto& divs = divs_(ai, d);
			FOR(i, 1, divs.size() - 1) {
				if (divs[i] == pos) {
					return i;
				}
			}
			return -1;
		}

		s64 CalcCost() const {
			s64 cost = areaCost_ + changeCost_ + 1;
			return cost;
		}

		s64 CalcAreaCost(vector<s64>& daysAreaCost) const {
			cauto& A = server.A_;

			daysAreaCost.resize(D);

			s64 areaCost = 0;
			REP(d, D) {
				s64 lossSize = 0;
				{
					cauto& divs = daysDivs_[d];
					cauto& divIdxs = daysDivIdxs[d];
					REP(k, N) {
						int size = divs[divIdxs[k]].size_;
						int a = server.A_[d][k];
						if (a > size) {
							lossSize += a - size;
						}
					}
				}
				lossSize *= server.areaCostScale_[d];
				daysAreaCost[d] = lossSize * 100;
				areaCost += lossSize * 100;
			}
			return areaCost;
		}

		s64 CalcChangeCost() {
			s64 totalSize = 0;
			s64 commonSize = 0;
			REP(ai, areaHeights_.size()) {
				int h = areaHeights_[ai];

				int count = 0;
				REP(d, D) {
					cauto& div = divs_(ai, d);
					if (d == 0 || d + 1 == D) {
						count += (int)div.size() - 2;		
					}
					else {
						count += (int)(div.size() - 2) * 2;
					}
				}
				totalSize += count * h;

				count = 0;
				REP(d, D - 1) {
					cauto& divA = divs_(ai, d);
					cauto& divB = divs_(ai, d + 1);
					if (divA.size() > 2 && divB.size() > 2) {
						int a = 1;
						int b = 1;
						while (true) {
							if (divA[a] == divB[b]) {
								int divIdxA = dayAreaDivIdxs_(d, ai)[a];
								int divIdxB = dayAreaDivIdxs_(d + 1, ai)[b];
								daysDivs_[d][divIdxA].nextDivIdx_ = divIdxB;
								daysDivs_[d + 1][divIdxB].prevDivIdx_ = divIdxA;
								++count;
								++a;
								if (a + 1 == divA.size()) {
									break;
								}
							}
							else if (divA[a] < divB[b]) {
								++a;
								if (a + 1 == divA.size()) {
									break;
								}
							}
							else {
								++b;
								if (b + 1 == divB.size()) {
									break;
								}
							}
						}
					}
				}
				commonSize += count * h;
			}
			return totalSize - commonSize * 2;
		}

		void MakeAns(IOResult& result) const {
			result.Init();

			vector<int> tops(areaHeights_.size());
			FOR(i, 1, tops.size()) {
				tops[i] = tops[i - 1] + areaHeights_[i - 1];
			}
			REP(d, D) {
				cauto& divs = daysDivs_[d];
				cauto& idxs = daysDivIdxs[d];

				REP(k, N) {
					cauto& div = divs[idxs[k]];
					int ai = div.ai_;
					int h = areaHeights_[ai];
					int w = div.size_ / h;
					VASSERT(w * h == div.size_);
					result.rects_[d][k] = Rect{ div.pos_ - w, tops[ai], div.pos_, tops[ai] + h};
				}
			}

			result.Sort();
		}
	};
} 
namespace bar {

int CalcAreaHeights(const vector<vector<vector<int>>>& tbl, const vector<int>& areasMaxDaySize, vector<int>& areaHeights, bool canOver) {
	areaHeights.resize(tbl.size());

	cauto& A = server.A_;
	int top = 0;
	REP(ai, tbl.size()) {
		int maxSize = areasMaxDaySize[ai];

		int h = (maxSize + W - 1) / W;

		while (true) {
			bool ok = true;
			REP(d, D) {
				int sumW = 0;
				for (int k : tbl[ai][d]) {
					int a = A[d][k];
					int w = (a + h - 1) / h;
					sumW += w;
				}
				if (sumW > W) {
					ok = false;
					break;
				}
			}
			if (ok) {
				break;
			}
			++h;

			if (!canOver) {
				if (top + h > W) {
					return -1;
				}
			}
		}
		areaHeights[ai] = h;
		top += h;
	}
	return top;
}

bool MakeAssignTable(int areaCount, vector<vector<vector<int>>>& tbl, vector<int>& areaHeights, bool canOver) {
	static vector<vector<int>> daysKs(D);
	REP(d, D) {
		daysKs[d].resize(N);
		iota(ALL(daysKs[d]), 0);
	}
	static vector<int> areasMaxDaySize;
	CalcDivM(server.A_, daysKs, 0, areaCount, tbl, areasMaxDaySize);

	cauto& A = server.A_;


	int maxSum = 0;
	REP(ai, areaCount) {
		maxSum += areasMaxDaySize[ai];
	}

	if (!canOver) {
		if (maxSum > WW) {
			return false;
		}
	}

	int bottom = CalcAreaHeights(tbl, areasMaxDaySize, areaHeights, canOver);
	if (!canOver) {
		if (bottom < 0 || bottom > W) {
			return false;
		}
	}
	return true;
}

bool MakeAssignTable(vector<vector<vector<int>>>& tbl, vector<int>& areaHeights) {
	int okAreaCount = 0;
	int ngAreaCount = N + 1;
	vector<vector<vector<int>>> tmpTbl;
	vector<int> tmpAreaHeights;
	while (okAreaCount + 1 < ngAreaCount) {
		int ac = okAreaCount + (ngAreaCount - okAreaCount) / 2;

		if (MakeAssignTable(ac, tmpTbl, tmpAreaHeights, false)) {
			okAreaCount = ac;
			swap(tbl, tmpTbl);
			swap(tmpAreaHeights, areaHeights);
		}
		else {
			ngAreaCount = ac;
		}
	}
	if (okAreaCount <= 0) {
		return false;
	}
	return true;
}
} 

namespace bar {

struct SnapShot {
	Table table_;
	s64 rawScore_;

	s64 RawScore() const {
		return -rawScore_;
	}
};

struct StateBackup {
	struct MoveDivAction {
		int ai;
		int d;
		int di;
		int newDiv;

	};

	struct MoveItemAction {
		int d;
		int fromAi;
		int fromDi;
		int toAi;
		int toDi;
		int newDiv;

		void clear() {
			d = -1;
		}
		bool exist() const {
			return d >= 0;
		}
	};

	struct MoveAction {
		int d;
		int divIdx;
		int ai;
		int pos;

	};

	struct AreaSizeChangeAction {
		vector<pair<int, int>> aiHs;		

		void clear() {
			aiHs.clear();
		}
		bool exist() const {
			return !aiHs.empty();
		}
	};

	struct To {
		int divIdx;
		int ai;
		int pos;
	};
	struct MoveDivIdx {
		int d;
		vector<To> tos_;		

		void clear() {
			d = -1;
		}
		bool exist() const {
			return d >= 0;
		}
	};

	bool exist_;

	vector<MoveDivAction> moveDivs_;
	MoveItemAction moveItem_;
	vector<MoveAction> moves_;
	AreaSizeChangeAction areaSizeChange_;
	MoveDivIdx moveDivIdx_;

	vector<vector<s8>> daysDivIdxs_;		
	vector<s64> daysAreaCost_;

	void Clear() {
		exist_ = false;
		moveDivs_.clear();
		moveDivs_.reserve(D);
		moveItem_.clear();
		moves_.clear();
		moves_.reserve(D);
		areaSizeChange_.clear();
		moveDivIdx_.clear();

		daysDivIdxs_.resize(D);
		daysAreaCost_.resize(D);
	}

	void MoveDiv(int ai, int d, int di, int newDiv, const vector<s8>& divIdxs, s64 areaCost) {
		exist_ = true;

		moveDivs_.emplace_back();
		auto& m = moveDivs_.back();;
		m.ai = ai;
		m.d = d;
		m.di = di;
		m.newDiv = newDiv;

		daysDivIdxs_[d] = divIdxs;
		daysAreaCost_[d] = areaCost;
	}

	void MoveArea(int d, int fromAi, int fromDi, int toAi, int toDi, int newDiv, const vector<s8>& divIdxs, s64 areaCost) {
		VASSERT(!exist_);
		exist_ = true;

		auto& m = moveItem_;
		m.d = d;
		m.fromAi = fromAi;
		m.fromDi = fromDi;
		m.toAi = toAi;
		m.toDi = toDi;
		m.newDiv = newDiv;

		daysDivIdxs_[d] = divIdxs;
		daysAreaCost_[d] = areaCost;
	}

	void Move(int d, int divIdx, int ai, int pos, const vector<s8>& divIdxs, s64 areaCost) {
		exist_ = true;

		moves_.emplace_back();
		auto& m = moves_.back();
		m.d = d;
		m.divIdx = divIdx;
		m.ai = ai;
		m.pos = pos;

		daysDivIdxs_[d] = divIdxs;
		daysAreaCost_[d] = areaCost;
	}

	void ChangeAreaSize(const vector<pair<int, int>>& aiHs, const vector<vector<s8>>& daysDivIdxs, const vector<s64>& daysAreaCost) {
		VASSERT(!exist_);
		exist_ = true;

		auto& m = areaSizeChange_;
		m.aiHs = aiHs;

		daysDivIdxs_ = daysDivIdxs;
		daysAreaCost_ = daysAreaCost;
	}

	void MoveDivIdx(int d, const vector<To>& tos, const vector<s8>& divIdxs, s64 areaCost) {
		VASSERT(!exist_);
		exist_ = true;

		auto& m = moveDivIdx_;
		m.d = d;
		m.tos_ = tos;

		daysDivIdxs_[d] = divIdxs;
		daysAreaCost_[d] = areaCost;
	}

};

vector<vector<vector<int>>> baseTbl_;
vector<int> baseAreaHeights_;

struct State {
	Table table_;
	s64 rawScore_;

	void Init() {
		table_.Init(baseTbl_, baseAreaHeights_);
		rawScore_ = table_.CalcCost();
	}

	void Accepted(StateBackup&& backup) {
	}

	void Revert(StateBackup&& backup) {
		if (!backup.exist_) {
			return;
		}

		RREP(i, backup.moveDivs_.size()) {
			cauto& m = backup.moveDivs_[i];
			table_.MoveDiv(m.ai, m.d, m.di, m.newDiv);
			table_.FitK(m.d, backup.daysDivIdxs_[m.d], backup.daysAreaCost_[m.d]);
		}
		if (backup.moveItem_.exist()) {
			cauto& m = backup.moveItem_;
			table_.MoveArea(m.d, m.fromAi, m.fromDi, m.toAi, m.toDi, m.newDiv);
			table_.FitK(m.d, backup.daysDivIdxs_[m.d], backup.daysAreaCost_[m.d]);
		}
		RREP(i, backup.moves_.size()) {
			cauto& m = backup.moves_[i];
			table_.Move(m.d, m.divIdx, m.ai, m.pos);
			table_.FitK(m.d, backup.daysDivIdxs_[m.d], backup.daysAreaCost_[m.d]);
		}
		if (backup.areaSizeChange_.exist()) {
			cauto& m = backup.areaSizeChange_;
			for (auto [ai, newH] : m.aiHs) {
				table_.ChangeAreaSize(ai, newH);
			}
			REP(d, D) {
				table_.FitK(d, backup.daysDivIdxs_[d], backup.daysAreaCost_[d]);
			}
		}
		if (backup.moveDivIdx_.exist()) {
			cauto& m = backup.moveDivIdx_;
			for (cauto& to : m.tos_) {
				table_.RemoveIdx(m.d, to.divIdx);
			}
			for (cauto& to : m.tos_) {
				table_.AddIdx(m.d, to.divIdx, to.ai, to.pos);
			}
			table_.FitK(m.d, backup.daysDivIdxs_[m.d], backup.daysAreaCost_[m.d]);
		}

		rawScore_ = table_.CalcCost();
	}

	void Apply(const SnapShot& shot) {
		VABORT();
	}

	double EvalScore() const {
		return (double)-rawScore_;
	}

	s64 RawScore() const {
		return -rawScore_;
	}

	void Shot(SnapShot& shot) const {
		shot.table_ = table_;
		shot.rawScore_ = rawScore_;
	}


	void EnumSameDiv(int ai, int baseD, int baseDi, vector<pair<int, int>>& daysDis, int& lower, int& upper, int& beginD, int& endD) const {
		cauto& tbl = table_;

		daysDis.clear();
		daysDis.reserve(D);
		int pos;
		{
			int di = baseDi;
			auto& divs = tbl.divs_(ai, baseD);
			pos = divs[di];
			lower = divs[di - 1] + 1;
			upper = divs[di + 1];
			daysDis.emplace_back(make_pair(baseD, di));
		}

		beginD = baseD;
		endD = baseD + 1;
		{
			auto Check = [&](int d) {
				int di = tbl.FindDiv(ai, d, pos);
				if (di < 0) {
					return false;
				}
				auto& divs = tbl.divs_(ai, d);
				int newLower = max(lower, divs[di - 1] + 1);
				int newUpper = min(upper, divs[di + 1]);
				if (newLower >= newUpper) {
					return false;
				}
				daysDis.emplace_back(make_pair(d, di));
				lower = newLower;
				upper = newUpper;

				chmin(beginD, d);
				chmax(endD, d + 1);
				return true;
			};

			bool canPrev = true;
			bool canNext = true;
			int prevDivIdx = tbl.dayAreaDivIdxs_(baseD, ai)[baseDi];
			int nextDivIdx = prevDivIdx;
			int od = 1;
			while (canPrev || canNext) {
				if (canPrev) {
					int d = baseD - od;
					if (d < 0) {
						canPrev = false;
					}
					else {
						if (tbl.daysDivs_[d + 1][prevDivIdx].prevDivIdx_ < 0) {
							canPrev = false;
							prevDivIdx = -1;
						}
						else {
							prevDivIdx = tbl.daysDivs_[d + 1][prevDivIdx].prevDivIdx_;
							if (!Check(d)) {
								canPrev = false;
							}
						}
					}
				}

				if (canNext) {
					int d = baseD + od;
					if (d >= D) {
						canNext = false;
					}
					else {
						if (tbl.daysDivs_[d - 1][nextDivIdx].nextDivIdx_ < 0) {
							canNext = false;
							nextDivIdx = -1;
						}
						else {
							nextDivIdx = tbl.daysDivs_[d - 1][nextDivIdx].nextDivIdx_;
							if (!Check(d)) {
								canNext = false;
							}
						}
					}
				}
				++od;
			}
		}
	}

	s64 CutAreaCost() const {
		return 1000 + table_.areaCost_ * 2;
	}

	bool Trans_MoveDiv(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int ai = rand_((int)tbl.areaHeights_.size());
		int d = rand_(D);
		auto& divs = tbl.divs_(ai, d);
		if (divs.size() <= 2) {
			return false;
		}
		int di = 1 + rand_((int)divs.size() - 2);
		int lower = divs[di - 1] + 1;
		int upper = divs[di + 1];
		int newDiv = lower + rand_(upper - lower);

		backup.MoveDiv(ai, d, di, divs[di], tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

		tbl.MoveDiv(ai, d, di, newDiv);
		tbl.FitK(d);

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_MoveDivMD(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int ai = rand_((int)tbl.areaHeights_.size());

		int baseD = rand_(D);
		int baseDi;
		{
			auto& divs = tbl.divs_(ai, baseD);
			if (divs.size() <= 2) {
				return false;
			}
			baseDi = 1 + rand_((int)divs.size() - 2);
		}
		static vector<pair<int, int>> daysDis;
		int lower;
		int upper;
		int beginD;
		int endD;
		EnumSameDiv(ai, baseD, baseDi, daysDis, lower, upper, beginD, endD);

		s64 cutAreaCost = CutAreaCost();

		int newDiv = lower + rand_(upper - lower);
		for (auto [d, di] : daysDis) {
			auto& divs = tbl.divs_(ai, d);
			backup.MoveDiv(ai, d, di, divs[di], tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

			tbl.MoveDiv(ai, d, di, newDiv);
			tbl.FitK(d);

			if (tbl.areaCost_ > cutAreaCost) {
				break;
			}
		}

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_MoveDivFit(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int ai = rand_((int)tbl.areaHeights_.size());
		int d = rand_(D);
		auto& divs = tbl.divs_(ai, d);
		if (divs.size() <= 2) {
			return false;
		}
		int di = 1 + rand_((int)divs.size() - 2);

		int lower = divs[di - 1] + 1;
		int upper = divs[di + 1];

		int td;
		if (d == 0) {
			td = 1;
		}
		else if (d + 1 == D) {
			td = d - 1;
		}
		else {
			td = d + (rand_(2) ? 1 : -1);
		}

		static vector<int> cand;
		cand.clear();
		cauto& tdivs = tbl.divs_(ai, td);
		auto lit = lower_bound(ALL(tdivs), lower);
		auto rit = lower_bound(ALL(tdivs), upper);
		for (auto it = lit; it != rit; ++it) {
			cand.emplace_back(*it);
		}

		int newDiv;
		if (cand.empty()) {
			newDiv = lower + rand_(upper - lower);
		}
		else {
			newDiv = Choice(cand, rand_);
		}

		backup.MoveDiv(ai, d, di, divs[di], tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

		tbl.MoveDiv(ai, d, di, newDiv);
		tbl.FitK(d);

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_MoveDivFitMD(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int ai = rand_((int)tbl.areaHeights_.size());

		int baseD = rand_(D);
		int baseDi;
		{
			auto& divs = tbl.divs_(ai, baseD);
			if (divs.size() <= 2) {
				return false;
			}
			baseDi = 1 + rand_((int)divs.size() - 2);
		}
		static vector<pair<int, int>> daysDis;
		int lower;
		int upper;
		int beginD;
		int endD;
		EnumSameDiv(ai, baseD, baseDi, daysDis, lower, upper, beginD, endD);

		static vector<int> cand;
		cand.clear();
		if (beginD > 0) {
			cauto& tdivs = tbl.divs_(ai, beginD - 1);
			auto lit = lower_bound(ALL(tdivs), lower);
			auto rit = lower_bound(ALL(tdivs), upper);
			for (auto it = lit; it != rit; ++it) {
				cand.emplace_back(*it);
			}
		}
		if (endD + 1 < D) {
			cauto& tdivs = tbl.divs_(ai, endD);
			auto lit = lower_bound(ALL(tdivs), lower);
			auto rit = lower_bound(ALL(tdivs), upper);
			for (auto it = lit; it != rit; ++it) {
				cand.emplace_back(*it);
			}
		}

		int newDiv;
		if (cand.empty()) {
			newDiv = lower + rand_(upper - lower);
		}
		else {
			newDiv = Choice(cand, rand_);
		}

		s64 cutAreaCost = CutAreaCost();

		for (auto [d, di] : daysDis) {
			auto& divs = tbl.divs_(ai, d);
			backup.MoveDiv(ai, d, di, divs[di], tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

			tbl.MoveDiv(ai, d, di, newDiv);
			tbl.FitK(d);

			if (tbl.areaCost_ > cutAreaCost) {
				break;
			}
		}

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_SwapHeight(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;

		int fromAi = rand_((int)tbl.areaHeights_.size());
		int toAi = rand_((int)tbl.areaHeights_.size() - 1);
		if (toAi >= fromAi) {
			++toAi;
		}


		int size = 1;

		static vector<pair<int, int>> aiHs;
		aiHs.clear();
		aiHs.emplace_back(make_pair(fromAi, tbl.areaHeights_[fromAi]));
		aiHs.emplace_back(make_pair(toAi, tbl.areaHeights_[toAi]));
		backup.ChangeAreaSize(aiHs, tbl.daysDivIdxs, tbl.daysAreaCost_);

		s64 cutAreaCost = CutAreaCost();

		tbl.ChangeAreaSize(fromAi, tbl.areaHeights_[fromAi] - size);
		tbl.ChangeAreaSize(toAi, tbl.areaHeights_[toAi] + size);

		REP(d, D) {
			tbl.FitK(d);

			if (tbl.areaCost_ > cutAreaCost) {
				{
					cauto& m = backup.areaSizeChange_;
					for (auto [ai, newH] : m.aiHs) {
						table_.ChangeAreaSize(ai, newH);
					}
					REP(d2, d + 1) {
						table_.FitK(d2, backup.daysDivIdxs_[d2], backup.daysAreaCost_[d2]);
					}
				}
				backup.Clear();
				return false;
			}
		}

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_ChangeHeight(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;

		int sumH = 0;
		for (int h : tbl.areaHeights_) {
			sumH += h;
		}

		int size = -1;
		if (sumH < W) {
			size = rand_(2) ? 1 : -1;
		}

		int ai = rand_((int)tbl.areaHeights_.size());

		static vector<pair<int, int>> aiHs;
		aiHs.clear();
		aiHs.emplace_back(make_pair(ai, tbl.areaHeights_[ai]));
		backup.ChangeAreaSize(aiHs, tbl.daysDivIdxs, tbl.daysAreaCost_);

		s64 cutAreaCost = CutAreaCost();

		tbl.ChangeAreaSize(ai, tbl.areaHeights_[ai] + size);

		REP(d, D) {
			tbl.FitK(d);

			if (tbl.areaCost_ > cutAreaCost) {
				{
					cauto& m = backup.areaSizeChange_;
					for (auto [ai, newH] : m.aiHs) {
						table_.ChangeAreaSize(ai, newH);
					}
					REP(d2, d + 1) {
						table_.FitK(d2, backup.daysDivIdxs_[d2], backup.daysAreaCost_[d2]);
					}
				}
				backup.Clear();
				return false;
			}
		}

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_MoveItem(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;

		int d = rand_(D);
		int aiA = -1;
		REP(tryIdx, 10) {
			int ai = rand_((int)tbl.areaHeights_.size());
			if (tbl.divs_(ai, d).size() <= 2) {
				continue;
			}
			aiA = ai;
			break;
		}
		if (aiA < 0) {
			return false;
		}
		auto& divsA = tbl.divs_(aiA, d);
		int diA = 1 + rand_((int)divsA.size() - 2);

		int aiB = -1;
		int diB = -1;
		int pos = -1;
		REP(tryIdx, 4) {
			int ai = rand_((int)tbl.areaHeights_.size() - 1);
			if (ai >= aiA) {
				++ai;
			}
			auto& poss = tbl.divs_(ai, d);
			REP(tryIdx2, 4) {
				int di = 1 + rand_((int)poss.size() - 1);

				int lower = poss[di - 1] + 1;
				int upper = poss[di];
				if (upper <= lower) {
					continue;
				}
				pos = lower + rand_(upper - lower);
				diB = di;
				break;
			}
			if (diB < 0) {
				continue;
			}
			aiB = ai;
			break;
		}
		if (aiB < 0) {
			return false;
		}

		backup.MoveArea(d, aiB, diB, aiA, diA, divsA[diA], tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

		tbl.MoveArea(d, aiA, diA, aiB, diB, pos);
		tbl.FitK(d);

		rawScore_ = tbl.CalcCost();

		return true;
	}


	bool Trans_Pull(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int areaCount = (int)tbl.areaHeights_.size();

		int d = rand_(D);
		int ai = rand_(areaCount);
		int pos = 1 + rand_(W - 1);
		int f = tbl.FindDiv(ai, d, pos);
		if (f >= 0) {
			return false;
		}

		bool ok = false;
		int divIdx;
		REP(i, 10) {
			divIdx = tbl.daysDivIdxs[d][rand_(N)];
			cauto& div = tbl.daysDivs_[d][divIdx];
			if (div.pos_ == W) {
				continue;
			}
			ok = true;
			break;
		}
		if (!ok) {
			return false;
		}

		cauto& div = tbl.daysDivs_[d][divIdx];
		backup.Move(d, divIdx, div.ai_, div.pos_, tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

		tbl.Move(d, divIdx, ai, pos);
		tbl.FitK(d);

		rawScore_ = tbl.CalcCost();

		return true;
	}


	bool Trans_Pull2(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int areaCount = (int)tbl.areaHeights_.size();

		s64 cutAreaCost = CutAreaCost();

		REP(tryIdx, 2) {
			int d = rand_(D);
			int ai = rand_(areaCount);
			int pos = 1 + rand_(W - 1);

			int f = tbl.FindDiv(ai, d, pos);
			if (f >= 0) {
				continue;
			}

			bool ok = false;
			int divIdx;
			REP(i, 10) {
				divIdx = tbl.daysDivIdxs[d][rand_(N)];
				cauto& div = tbl.daysDivs_[d][divIdx];
				if (div.pos_ == W) {
					continue;
				}
				ok = true;
				break;
			}
			if (!ok) {
				continue;
			}

			cauto& div = tbl.daysDivs_[d][divIdx];
			backup.Move(d, divIdx, div.ai_, div.pos_, tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

			tbl.Move(d, divIdx, ai, pos);
			tbl.FitK(d);

			if (tbl.areaCost_ > cutAreaCost) {
				break;
			}
		}

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_PullFit(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int areaCount = (int)tbl.areaHeights_.size();

		int baseD = rand_(D);

		int baseDivIdx = -1;
		{
			cauto& divs = tbl.daysDivs_[baseD];
			cauto& divIdxs = tbl.daysDivIdxs[baseD];
			REP(tryIdx, 8) {
				int i = rand_((int)divIdxs.size());
				cauto& div = divs[divIdxs[i]];
				if (div.pos_ == W) {
					continue;
				}
				baseDivIdx = divIdxs[i];
				break;
			}
			if (baseDivIdx < 0) {
				return false;
			}
		}

		int dir = 0;
		{
			if (baseD == 0) {
				dir = 1;
			}
			else if (baseD + 1 == D) {
				dir = -1;
			}
			dir = rand_(2) ? 1 : -1;
		}

		if (dir > 0) {
			while (true) {
				cauto& div = tbl.daysDivs_[baseD][baseDivIdx];
				if (div.nextDivIdx_ >= 0) {
					++baseD;
					baseDivIdx = div.nextDivIdx_;
				}
				else {
					break;
				}
			}
		}
		else {
			while (true) {
				cauto& div = tbl.daysDivs_[baseD][baseDivIdx];
				if (div.prevDivIdx_ >= 0) {
					--baseD;
					baseDivIdx = div.prevDivIdx_;
				}
				else {
					break;
				}
			}
		}

		int pos = tbl.daysDivs_[baseD][baseDivIdx].pos_;
		int ai = tbl.daysDivs_[baseD][baseDivIdx].ai_;

		int d = baseD + dir;
		if (d < 0 || d >= D) {
			return false;
		}

		int f = tbl.FindDiv(ai, d, pos);
		if (f >= 0) {
			return false;
		}

		bool ok = false;
		int divIdx;
		REP(i, 10) {
			divIdx = tbl.daysDivIdxs[d][rand_(N)];
			cauto& div = tbl.daysDivs_[d][divIdx];
			if (div.pos_ == W) {
				continue;
			}
			ok = true;
			break;
		}
		if (!ok) {
			return false;
		}

		cauto& div = tbl.daysDivs_[d][divIdx];
		backup.Move(d, divIdx, div.ai_, div.pos_, tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

		tbl.Move(d, divIdx, ai, pos);
		tbl.FitK(d);

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_PullFit2(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int areaCount = (int)tbl.areaHeights_.size();

		REP(tryIdx, 2) {
			int baseD = rand_(D);

			int baseDivIdx = -1;
			{
				cauto& divs = tbl.daysDivs_[baseD];
				cauto& divIdxs = tbl.daysDivIdxs[baseD];
				REP(tryIdx, 8) {
					int i = rand_((int)divIdxs.size());
					cauto& div = divs[divIdxs[i]];
					if (div.pos_ == W) {
						continue;
					}
					baseDivIdx = divIdxs[i];
					break;
				}
				if (baseDivIdx < 0) {
					return false;
				}
			}

			int dir = 0;
			{
				if (baseD == 0) {
					dir = 1;
				}
				else if (baseD + 1 == D) {
					dir = -1;
				}
				dir = rand_(2) ? 1 : -1;
			}

			if (dir > 0) {
				while (true) {
					cauto& div = tbl.daysDivs_[baseD][baseDivIdx];
					if (div.nextDivIdx_ >= 0) {
						++baseD;
						baseDivIdx = div.nextDivIdx_;
					}
					else {
						break;
					}
				}
			}
			else {
				while (true) {
					cauto& div = tbl.daysDivs_[baseD][baseDivIdx];
					if (div.prevDivIdx_ >= 0) {
						--baseD;
						baseDivIdx = div.prevDivIdx_;
					}
					else {
						break;
					}
				}
			}

			int pos = tbl.daysDivs_[baseD][baseDivIdx].pos_;
			int ai = tbl.daysDivs_[baseD][baseDivIdx].ai_;

			s64 cutAreaCost = CutAreaCost();

			int d = baseD + dir;
			if (d < 0 || d >= D) {
				break;
			}

			int f = tbl.FindDiv(ai, d, pos);
			if (f >= 0) {
				continue;
			}

			bool ok = false;
			int divIdx;
			REP(i, 10) {
				divIdx = tbl.daysDivIdxs[d][rand_(N)];
				cauto& div = tbl.daysDivs_[d][divIdx];
				if (div.pos_ == W) {
					continue;
				}
				ok = true;
				break;
			}
			if (!ok) {
				continue;
			}

			cauto& div = tbl.daysDivs_[d][divIdx];
			backup.Move(d, divIdx, div.ai_, div.pos_, tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

			tbl.Move(d, divIdx, ai, pos);
			tbl.FitK(d);

			if (tbl.areaCost_ > cutAreaCost) {
				break;
			}
		}

		rawScore_ = tbl.CalcCost();

		return true;
	}

	bool Trans_Redivide(double timeRate, StateBackup& backup) {
		backup.Clear();

		auto& tbl = table_;
		int areaCount = (int)tbl.areaHeights_.size();

		int d = rand_(D);
		int ai = rand_(areaCount);

		static vector<int> ws;
		static vector<int> divIdxs;
		ws.clear();
		divIdxs.clear();
		int sumW = 0;
		REP(k, N) {
			int idx = tbl.daysDivIdxs[d][k];
			cauto& div = tbl.daysDivs_[d][idx];
			if (div.ai_ == ai) {
				int h = tbl.areaHeights_[ai];
				int w = (server.A_[d][k] + h - 1) / h;
				ws.emplace_back(w);
				if (div.pos_ < W) {
					divIdxs.emplace_back(idx);		
				}
				sumW += w;
			}
		}
		if (ws.size() <= 1) {
			return false;
		}

		static vector<int> targetDivs;
		targetDivs.clear();
		if (d > 0) {
			cauto& poss = tbl.divs_(ai, d - 1);
			FOR(i, 1, poss.size() - 1) {
				targetDivs.emplace_back(poss[i]);
			}
		}
		if (d + 1 < D) {
			cauto& poss = tbl.divs_(ai, d + 1);
			FOR(i, 1, poss.size() - 1) {
				targetDivs.emplace_back(poss[i]);
			}
		}
		sort(ALL(targetDivs));



		const int usableSpace = W - sumW;
		if (usableSpace < 0) {
			return false;
		}

		static vector<bool> used;
		used.assign(ws.size(), false);

		static vector<int> widthStack;
		widthStack.clear();

		static vector<int> bestWidthStack;
		bestWidthStack.clear();
		int bestUsedTargetCount = -1;

		Shuffle(ALL(ws), rand_);

		auto dfs = [&](auto&& dfs, int x, int usedSpace, int i, int ti, int usedTargetCount) -> bool {
			if (i >= ws.size()) {
				if (usedTargetCount > bestUsedTargetCount) {
					bestUsedTargetCount = usedTargetCount;
					bestWidthStack = widthStack;
				}
				return true;
			}

			REP(j, ws.size()) {
				if (used[j]) {
					continue;
				}
				used[j] = true;

				int w = ws[j];
				int nextX = x + w;

				int nti = ti;
				while (nti < targetDivs.size() && targetDivs[nti] < nextX) {
					++nti;
				}

				if (nti < targetDivs.size()) {
					int targetX = targetDivs[nti];
					int addSpace = targetX - x;
					if (usedSpace + addSpace <= usableSpace) {
						int nextUsedTargetCount = usedTargetCount + 1;
						int nti2 = nti;
						if (nti + 1 < targetDivs.size() && targetX == targetDivs[nti + 1]) {
							++nti2;
							++nextUsedTargetCount;
						}

						widthStack.emplace_back(targetX - x);
						if (dfs(dfs, targetX, usedSpace + addSpace, i + 1, nti2 + 1, nextUsedTargetCount)) {
						}
						widthStack.pop_back();
					}
				}

				widthStack.emplace_back(w);
				if (dfs(dfs, nextX, usedSpace, i + 1, nti, usedTargetCount)) {
				}
				widthStack.pop_back();

				used[j] = false;

				break;
			}
			return false;
		};
		bool ret = dfs(dfs, 0, 0, 0, 0, 0);

		if (bestWidthStack.empty()) {
			return false;
		}

		static vector<StateBackup::To> tos;
		tos.clear();
		for (int idx : divIdxs) {
			cauto& div = tbl.daysDivs_[d][idx];
			tos.emplace_back();
			auto& to = tos.back();
			to.ai = ai;
			to.divIdx = idx;
			to.pos = div.pos_;
		}
		backup.MoveDivIdx(d, tos, tbl.daysDivIdxs[d], tbl.daysAreaCost_[d]);

		for (int idx : divIdxs) {
			tbl.RemoveIdx(d, idx);
		}
		VASSERT(divIdxs.size() == bestWidthStack.size() - 1);
		int x = 0;
		REP(i, bestWidthStack.size() - 1) {
			int w = bestWidthStack[i];
			x += w;
			tbl.AddIdx(d, divIdxs[i], ai, x);
		}
		tbl.FitK(d);		

		rawScore_ = tbl.CalcCost();
		return true;
	}
};

struct Solver {
	void Run(const vector<vector<vector<int>>>& assignTbl, const vector<int>& areaHeights, IOResult& result, s64& rawScore) {
		baseTbl_ = assignTbl;
		baseAreaHeights_ = areaHeights;

		SABackup<State, SnapShot, StateBackup> sa;
		sa.Init(HP.StartTemp, HP.EndTemp, HP.TempType, HP.TempPow);

		RandomTable tbl;
		tbl.push(0, HP.Trans0);
		tbl.push(1, HP.Trans1);
		tbl.push(2, HP.Trans2);
		tbl.push(3, HP.Trans3);
		if (sa.GetState().table_.areaHeights_.size() >= 2) {
			tbl.push(4, HP.Trans4);
		}
		tbl.push(5, HP.Trans5);
		tbl.push(6, HP.Trans6);
		tbl.push(7, HP.Trans7);
		tbl.push(9, HP.Trans9);



		sa.Run(timer_, [&](State& state, double timeRate, int& ti, StateBackup& backup) -> bool {
			ti = tbl(rand_);
			if (ti == 0) {
				return state.Trans_MoveDiv(timeRate, backup);
			}
			else if (ti == 1) {
				return state.Trans_MoveDivMD(timeRate, backup);
			}
			else if (ti == 2) {
				return state.Trans_MoveDivFit(timeRate, backup);
			}
			else if (ti == 3) {
				return state.Trans_MoveDivFitMD(timeRate, backup);
			}
			else if (ti == 4) {
				return state.Trans_MoveItem(timeRate, backup);
			}
			else if (ti == 5) {
				return state.Trans_Pull(timeRate, backup);
			}
			else if (ti == 6) {
				return state.Trans_SwapHeight(timeRate, backup);
			}
			else if (ti == 7) {
				return state.Trans_ChangeHeight(timeRate, backup);
			}
			else if (ti == 8) {
				return state.Trans_Redivide(timeRate, backup);
			}
			else if (ti == 9) {
				return state.Trans_PullFit(timeRate, backup);
			}
			else if (ti == 10) {
				return state.Trans_Pull2(timeRate, backup);
			}
			else if (ti == 11) {
				return state.Trans_PullFit2(timeRate, backup);
			}
			else {
				VABORT();
			}
			return false;
			});

		auto& best = sa.GetBestShot();
		auto& table = best.table_;
		s64 bestScore = best.rawScore_;

		int areaCount = (int)table.areaHeights_.size();
		vector<int> areasDivCount(areaCount);
		REP(d, D) {
			REP(ai, areaCount) {
				areasDivCount[ai] += (int)table.dayAreaDivIdxs_(d, ai).size();
			}
		}
		vector<int> ais(areaCount);
		iota(ALL(ais), 0);
		sort(ALL(ais), [&](int a, int b) {
			return areasDivCount[a] > areasDivCount[b];
			});

		auto backupDaysDivIdxs = table.daysDivIdxs;
		auto backupDaysAreaCost = table.daysAreaCost_;
		for (int ai : ais) {
			while (true) {
				if (table.areaHeights_[ai] <= 1) {
					break;
				}
				table.ChangeAreaSize(ai, table.areaHeights_[ai] - 1);
				REP(d, D) {
					table.FitK(d);
				}
				s64 score = table.CalcCost();
				if (score < bestScore) {
					bestScore = score;
					backupDaysDivIdxs = table.daysDivIdxs;
					backupDaysAreaCost = table.daysAreaCost_;
				}
				else {
					table.ChangeAreaSize(ai, table.areaHeights_[ai] + 1);
					REP(d, D) {
						table.FitK(d, backupDaysDivIdxs[d], backupDaysAreaCost[d]);
					}
					break;
				}
			}
		}

		table.MakeAns(result);
		rawScore = bestScore;
	}
};

} 

#define LEAK_CHECK 1
#define HAS_CUT 0


namespace mix {
struct Node {
	Rect rect_;						
	int beginD_;					
	vector<vector<int>> daysKs_;	
	vector<int> nexts_;				


	void Delete() {
		rect_ = { -1,-1,-1,-1 };
		beginD_ = -1;
		daysKs_.clear();
		nexts_.clear();

	}

	void Init(const Rect& rect, int beginD, int endD) {
		rect_ = rect;
		beginD_ = beginD;
		daysKs_.resize(endD - beginD);
		for (auto& ks : daysKs_) {
			ks.clear();
		}
		nexts_.clear();

	}

	int GetEndD() const {
		return beginD_ + (int)daysKs_.size();
	}

	void Disp(ostream& os) const {
		os << "----------" << endl;
		os << "rect: " << rect_.l << " " << rect_.t << " " << rect_.r << " " << rect_.b << endl;
		os << "D: " << beginD_ << " ~ " << beginD_ + daysKs_.size() << endl;
		os << "Ks:" << endl;
		REP(di, daysKs_.size()) {
			int d = beginD_ + di;
			os << "  " << d << ":";
			for (int k : daysKs_[di]) {
				os << " " << k;
			}
			os << endl;
		}
		os << "nexts:";
		for (int ni : nexts_) {
			os << " " << ni;
		}
		os << endl;
		os << "----------" << endl;
	}
};

struct NodePool {
	vector<Node> pool_;
	vector<int> freeIdxs_;

	void Init() {
		pool_.resize(N * D * 2 * 10);		
		freeIdxs_.resize(pool_.size());
		iota(ALL(freeIdxs_), 0);
		reverse(ALL(freeIdxs_));
	}

	int New() {
		int idx = freeIdxs_.back();
		freeIdxs_.pop_back();
		auto& n = GetNode(idx);
		n.nexts_.clear();

		return idx;
	}

	int CloneTree(int srcNi) {
		const Node& src = GetNode(srcNi);
		int ni = New();
		Node& n = GetNode(ni);
		n.rect_ = src.rect_;
		n.beginD_ = src.beginD_;
		n.daysKs_ = src.daysKs_;
		n.nexts_.resize(src.nexts_.size());
		REP(i, src.nexts_.size()) {
			int ci = CloneTree(src.nexts_[i]);
			n.nexts_[i] = ci;
		}
		return ni;
	}
	int CloneSingle(int srcNi) {
		const Node& src = GetNode(srcNi);
		int ni = New();
		Node& n = GetNode(ni);
		n.rect_ = src.rect_;
		n.beginD_ = src.beginD_;
		n.daysKs_ = src.daysKs_;
		n.nexts_.clear();
		return ni;
	}

	void Delete(int idx) {
		auto& n = GetNode(idx);
		for (int ni : n.nexts_) {
			VASSERT(idx != ni);
			Delete(ni);
		}
		n.Delete();
		freeIdxs_.emplace_back(idx);
	}

	void DeleteNext(int idx) {
		auto& n = GetNode(idx);
		for (int ni : n.nexts_) {
			VASSERT(idx != ni);
			Delete(ni);
		}
		n.nexts_.clear();
	}

	const Node& GetNode(int idx) const {
		return pool_[idx];
	}
	Node& GetNode(int idx) {
		return pool_[idx];
	}

};
} 
namespace mix {
	NodePool pool_;
}


namespace mix {
struct Tree {
	int rootNi_ = -1;
	double divRate_ = 1;		

	Tree() = default;
	Tree(const Tree&) = delete;
	void operator =(const Tree&) = delete;

	void SwapTree(Tree& tree) {
		swap(rootNi_, tree.rootNi_);
		VASSERT(divRate_ == tree.divRate_);
	}
	void CloneTree(const Tree& tree) {
		if (rootNi_ >= 0) {
			pool_.Delete(rootNi_);
		}
		rootNi_ = pool_.CloneTree(tree.rootNi_);
		divRate_ = tree.divRate_;
	}

	int MoveNextToDummy(int ni) {
		int dummyNi = pool_.New();
		auto& n = pool_.GetNode(ni);
		auto& dummy = pool_.GetNode(dummyNi);
		swap(dummy.nexts_, n.nexts_);
		n.nexts_.clear();
		return dummyNi;
	}

	void RevertDummy(int ni, int dummyNi) {
		auto& n = pool_.GetNode(ni);
		auto& dummy = pool_.GetNode(dummyNi);
		swap(dummy.nexts_, n.nexts_);
		pool_.Delete(dummyNi);
	}

	void Init(double baseDivRate = 1) {
		if (rootNi_ >= 0) {
			pool_.Delete(rootNi_);
		}

		rootNi_ = pool_.New();
		auto& n = pool_.GetNode(rootNi_);
		n.Init(Rect{ 0, 0, W, W }, 0, D);
		for (auto& ks : n.daysKs_) {
			ks.resize(N);
			iota(ALL(ks), 0);
		}

		divRate_ = baseDivRate;
	}

	bool TryDivRoot() {
		int okAreaCount = 0;
		int ngAreaCount = N + 1;
		int okNi = -1;
		while (okAreaCount + 1 < ngAreaCount) {
			int ac = okAreaCount + (ngAreaCount - okAreaCount) / 2;
			int ni = pool_.CloneSingle(rootNi_);
			if (TryDivRootArea(ni, ac)) {
				okAreaCount = ac;

				if (okNi >= 0) {
					pool_.Delete(okNi);
				}
				okNi = ni;
			}
			else {
				ngAreaCount = ac;
				pool_.Delete(ni);
			}
		}
		if (okNi < 0) {
			return TryDivRootOver();
		}

		pool_.Delete(rootNi_);
		rootNi_ = okNi;


		return true;
	}

	bool TryDivRootOver() {
		double ngRate = 1.0;
		while (true) {
			divRate_ *= 0.995;

			int ni = pool_.CloneSingle(rootNi_);

			if (TryDivRootArea(ni, 1)) {
				pool_.Delete(rootNi_);
				rootNi_ = ni;
				break;
			}
			pool_.Delete(ni);
			ngRate = divRate_;
		}

		double okRate = divRate_;
		REP(tryIdx, 4) {
			if (timer_.ElapseTimeMs() >= TIME_LIMIT * 0.8) {
				break;
			}
			divRate_ = (okRate + ngRate) * 0.5;

			int ni = pool_.CloneSingle(rootNi_);
			if (TryDivRootArea(ni, 1)) {
				pool_.Delete(rootNi_);
				rootNi_ = ni;
				okRate = divRate_;
			}
			else {
				pool_.Delete(ni);
				ngRate = divRate_;
			}
		}

		return true;
	}

	bool TryDivRootArea(int ni, int areaCount) {
		auto& node = pool_.GetNode(ni);
		VASSERT(node.nexts_.empty());

		vector<vector<vector<int>>> assignTbl(areaCount);
		vector<int> areasMaxDaySize;
		CalcDivM(ni, areaCount, assignTbl, areasMaxDaySize);

		int maxSum = 0;
		REP(ai, areaCount) {
			maxSum += areasMaxDaySize[ai];
		}

		if (maxSum <= WW) {
			if (TryDivRootAreaWithAssign(ni, assignTbl, areasMaxDaySize)) {
				return true;
			}
			pool_.DeleteNext(ni);
		}
		return false;
	}

	bool TryDivRootAreaWithAssign(int ni, const vector<vector<vector<int>>>& assignTbl, const vector<int>& maxSizes) {
		auto& node = pool_.GetNode(ni);
		VASSERT(node.nexts_.empty());

		VASSERT(node.rect_.l == 0);
		VASSERT(node.rect_.t == 0);
		VASSERT(node.rect_.r == W);
		VASSERT(node.rect_.b == W);

		cauto& A = server.A_;

		int top = node.rect_.t;
		REP(ai, assignTbl.size()) {
			int maxSize = maxSizes[ai];


			bool ok = false;
			Rect rect = Rect{ 0, top, W, top + (maxSize + W - 1) / W };

			int ci = pool_.New();
			auto& c = pool_.GetNode(ci);
			c.Init(rect, node.beginD_, node.GetEndD());
			c.daysKs_ = assignTbl[ai];

			while (rect.b <= W) {
				if (TryDivAuto(ci)) {
					node.nexts_.emplace_back(ci);
					ok = true;
					break;
				}
				pool_.DeleteNext(ci);

				++rect.b;
				c.rect_.b = rect.b;
			}
			if (!ok) {
				pool_.Delete(ci);
				return false;
			}

			top = rect.b;
		}
		return true;
	}

	bool TryDivTable(const vector<vector<vector<int>>>& tbl) {
		int areaCount = (int)tbl.size();
		vector<int> maxSizes(areaCount);
		cauto& A = server.A_;
		REP(ai, tbl.size()) {
			int maxSize = 0;
			REP(d, tbl[ai].size()) {
				int size = 0;
				for (int k : tbl[ai][d]) {
					size += A[d][k];
				}
				chmax(maxSize, size);
			}
			maxSizes[ai] = maxSize;
		}
		return TryDivRootAreaWithAssign(rootNi_, tbl, maxSizes);
	}

	void GetRootDivTable(vector<vector<vector<int>>>& tbl) const {
		cauto& n = pool_.GetNode(rootNi_);
		tbl.resize(n.nexts_.size());
		REP(ai, n.nexts_.size()) {
			cauto& c = pool_.GetNode(n.nexts_[ai]);
			tbl[ai] = c.daysKs_;
		}
	}

	void EnumDivBinaryAreaNode(vector<int>& nis) const {
		nis.clear();

		auto dfs = [&](auto&& dfs, int ni) -> void {
			cauto& n = pool_.GetNode(ni);

			if (n.nexts_.size() == 2) {
				cauto& c = pool_.GetNode(n.nexts_.front());
				if (n.daysKs_.size() == c.daysKs_.size()) {
					VASSERT(n.rect_ != c.rect_);
					nis.emplace_back(ni);
				}
			}
			for (int ci : n.nexts_) {
				dfs(dfs, ci);
			}
		};
		dfs(dfs, rootNi_);
	}

	void FitRootRect() {
		auto dfs = [&](auto&& dfs, int ni, int offset, int expand, int parentBottom) -> void {
			auto& n = pool_.GetNode(ni);
			for (int ci : n.nexts_) {
				dfs(dfs, ci, offset, expand, parentBottom);
			}

			if (n.rect_.b == parentBottom) {
				n.rect_.b += expand;
			}
			n.rect_.t += offset;
			n.rect_.b += offset;
		};

		cauto& rootNode = pool_.GetNode(rootNi_);
		int h = pool_.GetNode(rootNode.nexts_.back()).rect_.b;

		int space = W - h;
		if (space > 0) {
			int cnt = (int)rootNode.nexts_.size();
			int step = (space + cnt - 1) / cnt;

			int y = 0;
			for (int ci : rootNode.nexts_) {
				auto& n = pool_.GetNode(ci);
				int size = n.rect_.b - n.rect_.t;
				int offset = y - n.rect_.t;
				int expand = min(space, step);
				space -= step;
				chmax(space, 0);

				dfs(dfs, ci, offset, expand, n.rect_.b);

				y += size + expand;
			}
		}
	}

	bool TryDivAuto(int ni) {
		auto& node = pool_.GetNode(ni);
		VASSERT(node.nexts_.empty());

		cauto& A = server.A_;

		VASSERT(!node.daysKs_.empty());
		if (node.daysKs_.size() == 1) {
			return TryDivSingleDayBinary(ni);
		}


		auto TryMake = [&](int beginDi, int endDi) -> int {
			int ci = pool_.New();
			auto& c = pool_.GetNode(ci);
			c.Init(node.rect_, node.beginD_ + beginDi, node.beginD_ + endDi);
			REP(i, c.daysKs_.size()) {
				c.daysKs_[i] = node.daysKs_[beginDi + i];
			}

			if (c.daysKs_.size() == 1) {
				if (!TryDivSingleDayBinary(ci)) {
					pool_.Delete(ci);
					return -1;
				}
			}
			else {
				if (!TryDivMultiDayBinary(ci)) {
					pool_.Delete(ci);
					return -1;
				}
			}
			return ci;
		};

		auto Try = [&](int beginDi, int endDi) -> bool {
			if (endDi - beginDi <= 1) {
				int ci = TryMake(beginDi, endDi);
				if (ci < 0) {
					return false;
				}
				node.nexts_.emplace_back(ci);
				return true;
			}

			int startDi = beginDi;
			while (startDi < endDi) {
				int okDi = startDi;
				int ngDi = endDi + 1;
				int okCi = -1;
				while (okDi + 1 < ngDi) {
					int midDi = okDi + (ngDi - okDi) / 2;
					int ci = TryMake(startDi, midDi);
					if (ci >= 0) {
						okDi = midDi;

						if (okCi >= 0) {
							pool_.Delete(okCi);
						}
						okCi = ci;
					}
					else {
						ngDi = midDi;
					}
				}
				if (okDi == startDi) {
					return false;
				}
				VASSERT(okCi >= 0);
				node.nexts_.emplace_back(okCi);
				startDi = okDi;
			}
			return true;
		};

		int beginDi = -1;
		REP(di, node.daysKs_.size()) {
			cauto& ks = node.daysKs_[di];

			if (ks.size() <= 1) {
				if (beginDi >= 0) {
					if (!Try(beginDi, di)) {
						return false;
					}
					beginDi = -1;
				}

				if (!Try(di, di + 1)) {
					return false;
				}
			}
			else {
				if (beginDi < 0) {
					beginDi = di;
				}
			}
		}
		if (beginDi >= 0) {
			if (!Try(beginDi, (int)node.daysKs_.size())) {
				return false;
			}
		}
		return true;
	}

	bool TryDivSingleDayBinary(int ni) {
		auto& node = pool_.GetNode(ni);
		VASSERT(node.nexts_.empty());
		VASSERT(node.daysKs_.size() == 1);

		cauto& A = server.A_;
		cauto& ks = node.daysKs_[0];
		VASSERT(!ks.empty());
		if (ks.size() == 1) {
			return true;
		}

		int d = node.beginD_;

		array<vector<int>, 2> tmpDividedKs;
		array<int, 2> sums;
		DivideSum(A[d], ks, { 0.5, 0.5 }, tmpDividedKs, sums);

		array<vector<vector<int>>, 2> dividedKs;
		REP(i, 2) {
			dividedKs[i].resize(1);
			dividedKs[i][0] = move(tmpDividedKs[i]);
		}

		if (!TryDivBinary(ni, dividedKs, sums)) {
			return false;
		}
		return true;
	}

	bool TryDivMultiDayBinary(int ni) {
		auto& node = pool_.GetNode(ni);
		VASSERT(node.nexts_.empty());

		if (node.daysKs_.size() == 1) {
			return TryDivSingleDayBinary(ni);
		}

		int areaCount = 2;

		vector<vector<vector<int>>> assignTbl(areaCount);
		vector<int> areasMaxDaySize;
		CalcDivM(ni, areaCount, assignTbl, areasMaxDaySize);

		array<vector<vector<int>>, 2> dividedKs;
		array<int, 2> sums;
		REP(i, 2) {
			dividedKs[i] = move(assignTbl[i]);
			sums[i] = areasMaxDaySize[i];
		}

		if (!TryDivBinary(ni, dividedKs, sums)) {
			return false;
		}
		return true;
	}

	bool TryDivBinary(int ni, const array<vector<vector<int>>, 2>& dividedKs, const array<int, 2>& sums) {
		auto& node = pool_.GetNode(ni);
		VASSERT(node.nexts_.empty());

		bool vert;
		int lower;
		int upper;
		cauto& rect = node.rect_;
		vert = CalcVert(rect);
		if (vert) {
			int h = rect.b - rect.t;
			lower = rect.l + int(sums[0] * divRate_ + h - 1) / h;
			upper = rect.r - int(sums[1] * divRate_ + h - 1) / h;


			chmax(lower, rect.l + 1);
			chmin(upper, rect.r - 1);
		}
		else {
			int w = rect.r - rect.l;
			lower = rect.t + int(sums[0] * divRate_ + w - 1) / w;
			upper = rect.b - int(sums[1] * divRate_ + w - 1) / w;


			chmax(lower, rect.t + 1);
			chmin(upper, rect.b - 1);
		}

		if (lower > upper) {
			return false;
		}

		auto Div = [&](int mid) {
			array<Rect, 2> dividedRect;
			DivRect(node.rect_, vert, mid, dividedRect);

			REP(i, 2) {
				int ci = pool_.New();
				auto& c = pool_.GetNode(ci);
				c.Init(dividedRect[i], node.beginD_, node.GetEndD());
				c.daysKs_ = dividedKs[i];

				if (!TryDivAuto(ci)) {
					pool_.Delete(ci);
					pool_.DeleteNext(ni);
					return false;
				}
				node.nexts_.emplace_back(ci);
			}

			return true;
		};

		int center = (lower + upper) / 2;
		int offset = 0;
		while (center - offset >= lower || center + offset < upper) {
			if (center - offset >= lower) {
				int mid = center - offset;
				if (Div(mid)) {
					return true;
				}
			}
			if (center + offset < upper) {
				int mid = center + offset;
				if (Div(mid)) {
					return true;
				}
			}
			++offset;
		}
		return false;
	}

	void CalcDivM(int ni, int areaCount, vector<vector<vector<int>>>& dividedDayKs, vector<int>& areasMaxDaySize) const {
		cauto& node = pool_.GetNode(ni);
		::CalcDivM(server.A_, node.daysKs_, node.beginD_, areaCount, dividedDayKs, areasMaxDaySize);
	}

	void KSort() {
		static vector<vector<int>> rectSizes;
		rectSizes.resize(D);
		for (auto& r : rectSizes) {
			r.resize(N);
		}
		{
			auto dfs = [&](auto&& dfs, int ni) -> void {
				cauto& n = pool_.GetNode(ni);
				REP(di, n.daysKs_.size()) {
					int d = n.beginD_ + di;
					cauto& ks = n.daysKs_[di];
					if (ks.size() == 1) {
						int k = ks[0];
						rectSizes[d][k] = n.rect_.Size();
					}
				}
				for (int ci : n.nexts_) {
					dfs(dfs, ci);
				}
			};
			dfs(dfs, rootNi_);
		}

		static vector<vector<int>> transTbl;		
		transTbl.resize(D);
		REP(d, D) {
			auto& ks = transTbl[d];
			ks.resize(N);
			iota(ALL(ks), 0);
			sort(ALL(ks), [&](int a, int b) {
				return rectSizes[d][a] < rectSizes[d][b];
				});
		}

		{
			auto dfs = [&](auto&& dfs, int ni) -> void {
				auto& n = pool_.GetNode(ni);
				REP(di, n.daysKs_.size()) {
					int d = n.beginD_ + di;
					auto& ks = n.daysKs_[di];
					for (int& k : ks) {
						k = transTbl[d][k];
					}
				}
				for (int ci : n.nexts_) {
					dfs(dfs, ci);
				}
			};
			dfs(dfs, rootNi_);
		}
	}

	s64 CalcScore() const {
		static IOResult result;
		static vector<DayLine> lines;
		result.Init();
		auto& rects = result.rects_;
		lines.resize(D);
		for (auto& l : lines) {
			l.hLines_.clear();
			l.vLines_.clear();
		}


		auto dfs = [&](auto&& dfs, int ni) -> void {
			cauto& n = pool_.GetNode(ni);

			REP(di, n.daysKs_.size()) {
				int d = n.beginD_ + di;
				cauto& ks = n.daysKs_[di];
				if (ks.size() == 1) {
					int k = ks[0];
					rects[d][k] = n.rect_;
				}
			}

			if (ni != rootNi_ || n.daysKs_.size() != D) {
				for (int ci : n.nexts_) {
					cauto& c = pool_.GetNode(ci);

					if (c.daysKs_.size() != n.daysKs_.size()) {
						VASSERT(n.rect_ == c.rect_);
					}
					else {
						VASSERT(n.nexts_.size() <= 2);

						bool diff = false;
						bool vert = false;
						Line line;
						if (n.rect_.l != c.rect_.l) {
							diff = true;
							vert = true;
							line.pos_ = c.rect_.l;
							line.edges_[0] = c.rect_.t;
							line.edges_[1] = c.rect_.b;
						}
						else if (n.rect_.t != c.rect_.t) {
							diff = true;
							vert = false;
							line.pos_ = c.rect_.t;
							line.edges_[0] = c.rect_.l;
							line.edges_[1] = c.rect_.r;
						}
						else if (n.rect_.r != c.rect_.r) {
							diff = true;
							vert = true;
							line.pos_ = c.rect_.r;
							line.edges_[0] = c.rect_.t;
							line.edges_[1] = c.rect_.b;
						}
						else if (n.rect_.b != c.rect_.b) {
							diff = true;
							vert = false;
							line.pos_ = c.rect_.b;
							line.edges_[0] = c.rect_.l;
							line.edges_[1] = c.rect_.r;
						}

						if (diff) {
							REP(di, n.daysKs_.size()) {
								int d = n.beginD_ + di;
								if (vert) {
									lines[d].vLines_.emplace_back(line);
								}
								else {
									lines[d].hLines_.emplace_back(line);
								}
							}
							break;
						}
					}
				}
			}

			for (int ci : n.nexts_) {
				dfs(dfs, ci);
			}
		};
		dfs(dfs, rootNi_);

		for (auto& l : lines) {
			sort(ALL(l.hLines_));
			sort(ALL(l.vLines_));
		}

		result.Sort();
		s64 score = ::CalcScore(server.A_, rects, lines);
		return score;
	}

	void MakeResult(IOResult& result) const {
		result.Init();
		MakeResult(rootNi_, result);
		result.Sort();
	}
	void MakeResult(int ni, IOResult& result) const {
		cauto& n = pool_.GetNode(ni);
		REP(di, n.daysKs_.size()) {
			int d = n.beginD_ + di;
			cauto& ks = n.daysKs_[di];
			if (ks.size() == 0) {
				VABORT();
				continue;
			}
			if (ks.size() == 1) {
				int k = ks[0];
				result.rects_[d][k] = n.rect_;
			}
			else {
				VASSERT(!n.nexts_.empty());
			}
		}
		for (int ci : n.nexts_) {
			MakeResult(ci, result);
		}
	}
};
} 

namespace mix {

struct SnapShot {
	Tree tree_;
	s64 rawScore_;

	s64 RawScore() const {
		return -rawScore_;
	}
};

struct StateBackup {
	struct Action {
		int targetNi_ = -1;		
		int writeNi_ = -1;		
	};
	vector<Action> actions_;
	s64 rawScore_;

	void Clear() {
		for (cauto& a : actions_) {
			if (a.writeNi_ >= 0) {
				pool_.Delete(a.writeNi_);
			}
		}
		actions_.clear();
		rawScore_ = -1;
	}

};

struct State {
	Tree tree_;
	s64 rawScore_;

	mutable vector<vector<vector<int>>> tbl_;

	vector<int> areaDivNis_;

	void Init() {
		tree_.Init();
		bool ret = tree_.TryDivRoot();
		tree_.KSort();
		VASSERT(ret);
		rawScore_ = tree_.CalcScore();

		tree_.GetRootDivTable(tbl_);
		tree_.EnumDivBinaryAreaNode(areaDivNis_);
	}

	void Accepted(StateBackup&& backup) {
		tree_.KSort();

		tree_.GetRootDivTable(tbl_);
		tree_.EnumDivBinaryAreaNode(areaDivNis_);
	}

	void Revert(StateBackup&& backup) {
		for (cauto& a : backup.actions_) {
			tree_.RevertDummy(a.targetNi_, a.writeNi_);
		}
		backup.actions_.clear();
		rawScore_ = backup.rawScore_;
	}

	void Apply(const SnapShot& shot) {
		VABORT();
	}

	double EvalScore() const {
		return (double)-rawScore_;
	}

	s64 RawScore() const {
		return -rawScore_;
	}

	void Shot(SnapShot& shot) const {
		shot.tree_.CloneTree(tree_);
		shot.rawScore_ = rawScore_;
	}

	bool Trans_Remake(double timeRate, StateBackup& backup) {
		backup.Clear();

		int areaCount = (int)tbl_.size();
		if (areaCount == 1) {
			return false;
		}

		int d = rand_(D);
		int aiA = rand_(areaCount);
		int aiB = rand_(areaCount - 1);
		if (aiB >= aiA) {
			++aiB;
		}

		auto& ksA = tbl_[aiA][d];
		auto& ksB = tbl_[aiB][d];
		int kA = -1;
		int kB = -1;

		bool swap = rand_(2) == 0;
		if (swap) {
			kA = ChoicePopSwap(ksA, rand_);
			kB = ChoicePopSwap(ksB, rand_);
			ksA.emplace_back(kB);
			ksB.emplace_back(kA);
		}
		else {
			if (ksA.size() <= 1) {
				return false;
			}
			kA = ChoicePopSwap(ksA, rand_);
			ksB.emplace_back(kA);
		}

		int dummyNi = tree_.MoveNextToDummy(tree_.rootNi_);

		if (!tree_.TryDivTable(tbl_)) {
			if (swap) {
				ksA.pop_back();
				ksB.pop_back();
				ksA.emplace_back(kA);
				ksB.emplace_back(kB);
			}
			else {
				ksB.pop_back();
				ksA.emplace_back(kA);
			}

			tree_.RevertDummy(tree_.rootNi_, dummyNi);
			return false;
		}

		rawScore_ = tree_.CalcScore();

		backup.actions_.emplace_back();
		auto& a = backup.actions_.back();
		a.targetNi_ = tree_.rootNi_;
		a.writeNi_ = dummyNi;

		return true;
	}

	bool Trans_RemakeRootDiv(double timeRate, StateBackup& backup) {
		backup.Clear();

		int areaCount = (int)tbl_.size();
		if (areaCount == 1) {
			return false;
		}

		int d = rand_(D);
		int aiA = rand_(areaCount);
		int aiB = rand_(areaCount - 1);
		if (aiB >= aiA) {
			++aiB;
		}

		auto& ksA = tbl_[aiA][d];
		auto& ksB = tbl_[aiB][d];
		int kA = -1;
		int kB = -1;

		bool swap = rand_(2) == 0;
		if (swap) {
			kA = ChoicePopSwap(ksA, rand_);
			kB = ChoicePopSwap(ksB, rand_);
			ksA.emplace_back(kB);
			ksB.emplace_back(kA);
		}
		else {
			if (ksA.size() <= 1) {
				return false;
			}
			kA = ChoicePopSwap(ksA, rand_);
			ksB.emplace_back(kA);
		}

		cauto& rn = pool_.GetNode(tree_.rootNi_);
		int niA = rn.nexts_[aiA];
		int niB = rn.nexts_[aiB];


		int backupNi = tree_.rootNi_;
		tree_.rootNi_ = -1;

		tree_.Init(tree_.divRate_);
		if (!tree_.TryDivTable(tbl_)) {
			if (swap) {
				ksA.pop_back();
				ksB.pop_back();
				ksA.emplace_back(kA);
				ksB.emplace_back(kB);
			}
			else {
				ksB.pop_back();
				ksA.emplace_back(kA);
			}

			pool_.Delete(tree_.rootNi_);
			tree_.rootNi_ = backupNi;
			return false;
		}

		rawScore_ = tree_.CalcScore();

		backup.actions_.emplace_back();
		auto& a = backup.actions_.back();
		a.targetNi_ = tree_.rootNi_;
		a.writeNi_ = backupNi;

		return true;
	}

	bool Trans_RemakeBinaryDiv(double timeRate, StateBackup& backup) {
		backup.Clear();

		int ni = Choice(areaDivNis_, rand_);
		auto& n = pool_.GetNode(ni);

		array<int, 2> cis;
		{
			int a = rand_((int)n.nexts_.size());
			int b = rand_((int)n.nexts_.size() - 1);
			if (b >= a) {
				++b;
			}
			cis[0] = n.nexts_[a];
			cis[1] = n.nexts_[b];
		}

		static array<vector<vector<int>>, 2> dividedDaysKs;
		REP(i, 2) {
			int ci = cis[i];
			cauto& c = pool_.GetNode(ci);
			dividedDaysKs[i] = c.daysKs_;
		}

		cauto& A = server.A_;

		int di = rand_((int)n.daysKs_.size());
		int d = n.beginD_ + di;
		auto& ksA = dividedDaysKs[0][di];
		auto& ksB = dividedDaysKs[1][di];

		array<int, 2> maxSizes = {};

		bool doSwap = rand_(2) == 0;
		if (doSwap) {
			int kA = ChoicePopSwap(ksA, rand_);
			int kB = ChoicePopSwap(ksB, rand_);
			ksA.emplace_back(kB);
			ksB.emplace_back(kA);
		}
		else {
			if (ksA.size() <= 1) {
				return false;
			}
			int kA = ChoicePopSwap(ksA, rand_);
			ksB.emplace_back(kA);
		}

		REP(ai, 2) {
			int maxSize = 0;
			REP(di, dividedDaysKs[ai].size()) {
				int d = n.beginD_ + di;
				int sum = 0;
				for (int k : dividedDaysKs[ai][di]) {
					sum += A[d][k];
				}
				chmax(maxSize, sum);
			}
			maxSizes[ai] = maxSize;
		}

		int dummyNi = tree_.MoveNextToDummy(ni);

		if (!tree_.TryDivBinary(ni, dividedDaysKs, maxSizes)) {
			tree_.RevertDummy(ni, dummyNi);
			return false;
		}

		rawScore_ = tree_.CalcScore();

		backup.actions_.emplace_back();
		auto& a = backup.actions_.back();
		a.targetNi_ = ni;
		a.writeNi_ = dummyNi;

		return true;
	}
};

struct Solver {
	void Run(IOResult& result, s64& rawScore) {
		pool_.Init();

		SABackup<State, SnapShot, StateBackup> sa;
		sa.Init(1000, 0);

		RandomTable tbl;
		{
			cauto& state = sa.GetState();
			cauto& n = pool_.GetNode(state.tree_.rootNi_);
			if (n.nexts_.size() >= 2) {
				tbl.push(0, 1);
			}
		}
		tbl.push(1, 10);


		sa.Run(timer_, [&](State& state, double timeRate, int& ti, StateBackup& backup) -> bool {
			ti = tbl(rand_);
			if (ti == 0) {
				return state.Trans_Remake(timeRate, backup);
			}
			else if (ti == 1) {
				return state.Trans_RemakeBinaryDiv(timeRate, backup);
			}
			else {
				VABORT();
			}
			return false;
			}, 0b1);

		cauto& best = sa.GetBestShot();
		best.tree_.MakeResult(result);
		rawScore = best.rawScore_;

	}

	void Solve1(IOResult& result) {
		result.Init();

		Tree tree;
		tree.Init();


		bool ret = tree.TryDivRoot();
		VASSERT(ret);

		tree.MakeResult(result);


		vector<vector<vector<int>>> assignTbl;
		tree.GetRootDivTable(assignTbl);


	}

};

} 

namespace greedy {

struct Solver {
	vector<int> sizes_;
	vector<Rect> rects_;

	bool Run(IOResult& result, s64& rawScore) {
		sizes_.resize(N);
		int sum = 0;
		REP(k, N) {
			int size = 0;
			REP(d, D) {
				chmax(size, server.A_[d][k]);
			}
			sizes_[k] = size;
			sum += size;
		}
		if (sum > WW) {
			return false;
		}
		rects_.resize(N);

		vector<int> ks(N);
		iota(ALL(ks), 0);

		if (!DivRecursive(Rect{ 0, 0, W, W }, ks)) {
			return false;
		}
		result.Init();
		REP(d, D) {
			result.rects_[d] = rects_;
		}
		rawScore = 1;
		return true;
	}

	bool DivRecursive(const Rect& rect, const vector<int>& ks) {
		if (ks.size() == 1) {
			rects_[ks[0]] = rect;
			return true;
		}

		array<vector<int>, 2> dividedKs;
		array<int, 2> sums;
		DivideSum(sizes_, ks, { 0.5, 0.5 }, dividedKs, sums);

		bool vert;
		int lower;
		int upper;
		if (!CalcVertRange(rect, sums, vert, lower, upper)) {
			return false;
		}

		auto Div = [&](int mid) {
			array<Rect, 2> dividedRect;
			DivRect(rect, vert, mid, dividedRect);

			REP(i, 2) {
				if (!DivRecursive(dividedRect[i], dividedKs[i])) {
					return false;
				}
			}
			return true;
		};

		int center = (lower + upper) / 2;
		int offset = 0;
		while (center - offset >= lower || center + offset < upper) {
			if (center - offset >= lower) {
				int mid = center - offset;
				if (Div(mid)) {
					return true;
				}
			}
			if (center + offset < upper) {
				int mid = center + offset;
				if (Div(mid)) {
					return true;
				}
			}
			++offset;
		}
		return false;
	}
};


} 

struct Solver {
	void Run() {
		IOResult greedyResult;
		s64 greedyScore = INT64_MAX;
		{
			static greedy::Solver sovler;
			if (sovler.Run(greedyResult, greedyScore)) {
				server.Output(greedyResult);
				return;
			}
		}

		IOResult result;
		s64 rawScore = INT64_MAX;

		vector<vector<vector<int>>> tbl;
		vector<int> areaHeights;
		if (bar::MakeAssignTable(tbl, areaHeights) && areaHeights.size() >= 2) {
			static bar::Solver solver;
			solver.Run(tbl, areaHeights, result, rawScore);
		}
		else {
			static mix::Solver solver;
			solver.Run(result, rawScore);
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
