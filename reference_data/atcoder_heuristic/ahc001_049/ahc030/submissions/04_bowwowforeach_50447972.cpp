#define FULL_LIB 1

#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0


#define TIME_LIMIT (2800)

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



#define USE_TIMER 1
#define USE_LH_CACHE 1
#define USE_SILHOUET_HASH 1

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

template <class T, class U> inline void chmin(T& a, U&& b) { if (b < a) { a = b; } }
template <class T, class U> inline void chmax(T& a, U&& b) { if (a < b) { a = b; } }
template <class T, class U, class V> inline void clip(T& v, U&& lower, V&& upper) {
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
	template <class LESS>
	void sort(LESS&& less) {
		::sort(begin(), end(), less);
	}
	void stable_sort() {
		::stable_sort(begin(), end());
	}
	template <class LESS>
	void stable_sort(LESS&& less) {
		::stable_sort(begin(), end(), less);
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


template <class T, int CAP>
struct CapPriorityQueue {
    CapArr<T, CAP> buf_;

    constexpr int capacity() const {
        return CAP;
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

    template <class CMP>
    void push(const T& v, CMP&& cmp) {
        buf_.push(v);
        push_heap(ALL(buf_), cmp);
    }

    template <class CMP>
    T pop(CMP&& cmp) {
        pop_heap(ALL(buf_), cmp);
        T ret = buf_.back();
        buf_.pop();
        return ret;
    }

    struct Less {
        bool operator()(const T& a, const T& b) const {
            return a < b;
        }
    };


    void PushNoHeap(const T& v) {
        buf_.push(v);
    }
    void MakeHeap() {
        make_heap(ALL(buf_));
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
};


constexpr int N_min = 10;
constexpr int N_max = 20;
constexpr int NN_min = N_min * N_min;
constexpr int NN_max = N_max * N_max;
constexpr int M_min = 2;
constexpr int M_max = 20;
constexpr double Eps_min = 0.01;
constexpr double Eps_max = 0.2;
constexpr int D_min = 4;
constexpr int D_max = NN_max / M_min;		

constexpr int T_max = 2 * NN_max;

#define DEF_CONTAINERS(name, size) \
	template <class TYPE> using name##Arr = CapArr<TYPE, size>;\
	template <class TYPE> using name##Que = CapQue<TYPE, size>;\
	using name##Set = CapSet<size>;\
	template <class TYPE> using name##Map = CapMap<TYPE, size>;\
	template <class TYPE> using name##Deque = CapacityRingDeque<TYPE, size>;

DEF_CONTAINERS(N, N_max)
DEF_CONTAINERS(NN, NN_max)
DEF_CONTAINERS(M, M_max)
DEF_CONTAINERS(D, D_max)
DEF_CONTAINERS(T, T_max)

#define USE_REL_CELLS 0


struct OilField {
	NNSet cells;		
	DArr<pint> poss;		
	int w;		
	int h;		
	pint cog;
	int shapeId;		
};
int N;
int NN;
int M;
int MM;
double eps;
int E;
int EE;
double Deps;		
MArr<OilField> oils;
constexpr int OilsSize = sizeof(oils);
int totalValue;		

int T;


double neps;			
double normM;			
double meanOilRate;		




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

struct GridSystemD {
	int W;
	int H;
	int WH;	

private:
	vector<pint> toPos_;

public:
	void Init(int w, int h) {
		W = w;
		H = h;
		WH = W * H;

		toPos_.resize(WH);
		REP(i, WH) {
			toPos_[i].x = i % W;
			toPos_[i].y = i / W;
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


	inline bool IsOut(int x, int y) const {
		if (x < 0 || x >= W ||
			y < 0 || y >= H) {
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
		if (x == 0 || x == W - 1 ||
			y == 0 || y == H - 1) {
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

template <int AROUND_COUNT>
struct AroundMapD {
    using CA = CCA<int, AROUND_COUNT>;
    vector<CA> table_;
    int width_ = 1;

    void Init(int width, int height, const array<pint, AROUND_COUNT>& aroundDirs) {
        width_ = width;
        int count = width * height;
        table_.clear();
        table_.resize(count);

        REP(i, count) {
            pint p = { i % width, i / width };
            for (const pint& a : aroundDirs) {
                pint n = p + a;
                if (n.a >= 0 && n.a < width &&
                    n.b >= 0 && n.b < height) {
                    table_[i].push(n.a + n.b * width);
                }
            }
        }
    }

    inline const CA& operator[](int i) const {
        return table_[i];
    }
    inline const CA& operator[](const pint& p) const {
        return table_[p.x + p.y * width_];
    }
};

template <int AROUND_COUNT>
struct DirMapD {
    using CA = CCA<int, AROUND_COUNT>;
    vector<CA> table_;
    int width_ = 1;

    void Init(int W, int H, const array<pint, AROUND_COUNT>& aroundDirs) {
        width_ = W;
        int count = W * H;
        table_.clear();
        table_.resize(count);

        REP(cellId, count) {
            pint p = { cellId % W, cellId / W };
            for (const pint& a : aroundDirs) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                int n = -1;
                if (x >= 0 && x < W &&
                    y >= 0 && y < H) {
                    n = x + y * W;
                }
                table_[cellId].push(n);
            }
        }
    }

    inline const CA& operator ()(int id) const {
        return table_[id];
    }
    inline const CA& operator [](int id) const {
        return table_[id];
    }
};
GridSystemD gs;
AroundMapD<4> aroundMap;
DirMapD<4> dirMap;


template <int SIDE_SIZE>
struct NearMap {
    static const int SIZE = SIDE_SIZE * 2 + 1;
    using CA = CCA<int, SIZE * SIZE>;
    NNArr<CA> table_;

    void Init() {
        table_.resize(NN);
        REP(cell, NN) {
            cauto& baseP = gs.ToPos(cell);
            FOR(oy, -SIDE_SIZE, SIDE_SIZE + 1) {
                FOR(ox, -SIDE_SIZE, SIDE_SIZE + 1) {
                    int ny = baseP.y + oy;
                    int nx = baseP.x + ox;
                    if (gs.IsIn(nx, ny)) {
                        table_[cell].push(gs.ToId(nx, ny));
                    }
                }
            }
        }
    }

    inline const CA& operator[](int i) const {
        return table_[i];
    }
};
NearMap<1> nearMap1;



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




InputParam<int> ipN(N_min, N_max);
InputParam<int> ipNN(NN_min, NN_max);
InputParam<int> ipEE(1, 400);
InputParam<int> ipM(M_min, M_max);
InputParam<int> ipMM(M_min* M_min, M_max * M_max);
InputParam<double> ipEps(Eps_min, Eps_max);
InputParam<double> ipNeps(0.315434215, 0.632455532);
InputParam<double> ipDeps(5, 100);
InputParam<double> ipMOR(0.01, 0.4);


constexpr
struct {
	PARAM_CATEGORY(TempType, 2, 0, 1, 2);



	PARAM_DOUBLE(TempPow, 1.4360905822448569, 1.4, 1.6);PARAM_LOWER(0.0);
	PARAM_DOUBLE(StartTemp, 4.89215843640372, 0.0, 6.0);
	PARAM_DOUBLE(EndTemp, 0.9952760276678883, 0.0, 3.0);
	PARAM_DOUBLE(TransMaxStep, 44.32448091265865, 35.0, 55.0);

	PARAM_DOUBLE(StartTemp_Deps, 0.3319876755103619, -0.2, 0.2);
	PARAM_DOUBLE(StartTemp_MOR, 0.3580311651343593, -0.2, 0.2);
	PARAM_DOUBLE(EndTemp_MM, 0.16205416649386617, -0.2, 0.2);
	PARAM_DOUBLE(EndTemp_MOR, -0.1876024704120534, -0.2, 0.2);
	PARAM_DOUBLE(StartTemp_EE, -0.09448520383103379, -0.2, 0.2);
	PARAM_DOUBLE(StartTemp_MM, -0.06273318883372975, -0.2, 0.2);
	PARAM_DOUBLE(EndTemp_EE, 0.06866398020926828, -0.2, 0.2);
	PARAM_DOUBLE(EndTemp_Deps, -0.0509535646582233, -0.2, 0.2);
	PARAM_DOUBLE(TransMaxStep_EE, 0.05360556452318998, -0.2, 0.2);
	PARAM_DOUBLE(TransMaxStep_MM, -0.09171814261500573, -0.2, 0.2);
	PARAM_DOUBLE(TransMaxStep_Deps, 0.04133290296930658, -0.2, 0.2);
	PARAM_DOUBLE(TransMaxStep_MOR, -0.08925518220819344, -0.2, 0.2);


	PARAM_DOUBLE(TempScaleBase, -4.631981333560073, -6.0, -4.0);
	PARAM_DOUBLE(TempScaleMul, 0.25367461735000374, 0.0, 0.5);



	PARAM_DOUBLE(TempScaleBase_EE, 0, -0.2, 0.2);
	PARAM_DOUBLE(TempScaleBase_MM, 0, -0.2, 0.2);
	PARAM_DOUBLE(TempScaleBase_Deps, 0, -0.2, 0.2);
	PARAM_DOUBLE(TempScaleBase_MOR, 0, -0.2, 0.2);

	PARAM_DOUBLE(TempScaleMul_EE, 0, -0.2, 0.2);
	PARAM_DOUBLE(TempScaleMul_MM, 0, -0.2, 0.2);
	PARAM_DOUBLE(TempScaleMul_Deps, 0, -0.2, 0.2);
	PARAM_DOUBLE(TempScaleMul_MOR, 0, -0.2, 0.2);


	PARAM_INT(HighScoreCount, 11, 6, 16);PARAM_LOWER(0);PARAM_UPPER(128);
	PARAM_DOUBLE(InvalidProb, 0.3, 0, 0.5);PARAM_LOWER(0);


	PARAM_INT(FirstPatternThresholdN, 10, 10, 21);PARAM_LOWER(10);PARAM_UPPER(21);
	PARAM_INT(FirstPatternThresholdM, 6, 2, 21);PARAM_LOWER(2);PARAM_UPPER(21);


	PARAM_DOUBLE(MaximumProb, 12.952406128641899, 12.0, 18.0);PARAM_LOWER(0.0);

	PARAM_DOUBLE(WrongDown, 3.1008664549003315, 2.5, 3.5);PARAM_LOWER(1.0);

	PARAM_DOUBLE(SurveyRequireCountRate, 0.4982027428031262, 0.4, 0.6);PARAM_LOWER(0.0);
	PARAM_DOUBLE(SurveyRequireCountRate_Eps, -0.47541336821927355, -0.6, -0.2);
	PARAM_DOUBLE(SurveyRequireCountRate_Neps, 0.6179098873299621, 0.4, 0.8);
	PARAM_DOUBLE(SurveyRequireCountRate_EE, 0, -0.2, 0.2);
	PARAM_DOUBLE(SurveyRequireCountRate_Deps, 0, -0.2, 0.2);
	PARAM_DOUBLE(SurveyRequireCountRate_MOR, 0, -0.2, 0.2);




	PARAM_DOUBLE(SurveyRequireCountRate_M, 0.05722119160002112, 0, 0.1);
	PARAM_DOUBLE(SurveyRequireCountRate_MM, 0, -0.2, 0.2);


	PARAM_DOUBLE(MaximumProb_M, -0.22583411540350173, -0.3, 0.0);
	PARAM_DOUBLE(MaximumProb_Eps, -0.24309101767447555, -0.5, 0.0);
	PARAM_DOUBLE(MaximumProb_Neps, -0.33449251099450883, -1.0, 1.0);
	PARAM_DOUBLE(MaximumProb_EE, 0, -0.2, 0.2);
	PARAM_DOUBLE(MaximumProb_MM, 0, -0.2, 0.2);
	PARAM_DOUBLE(MaximumProb_Deps, 0, -0.2, 0.2);
	PARAM_DOUBLE(MaximumProb_MOR, 0, -0.2, 0.2);
	PARAM_DOUBLE(WrongDown_M, -0.054653640487070325, -0.5, 0.0);
	PARAM_DOUBLE(WrongDown_Eps, -0.15288737659176008, -0.2, 0.0);
	PARAM_DOUBLE(WrongDown_Neps, 0.3056020552979927, -1.0, 1.0);
	PARAM_DOUBLE(WrongDown_EE, 0, -0.2, 0.2);
	PARAM_DOUBLE(WrongDown_MM, 0, -0.2, 0.2);
	PARAM_DOUBLE(WrongDown_Deps, 0, -0.2, 0.2);
	PARAM_DOUBLE(WrongDown_MOR, 0, -0.2, 0.2);


	PARAM_INT(Trans_SingleMove, 95, 0, 100);PARAM_LOWER(0);PARAM_UPPER(100);

	PARAM_INT(Trans_SingleJump, 89, 0, 100);PARAM_LOWER(0);PARAM_UPPER(100);
	PARAM_INT(Trans_SwapMove, 11, 0, 100);PARAM_LOWER(0);PARAM_UPPER(100);
	START_TUNING;
	PARAM_DOUBLE(Trans_SwapMoveRect1, 31, 11, 51);
	PARAM_DOUBLE(Trans_SwapMoveRect1_M, 0, -0.5, 0.5);
	END_TUNING;




	PARAM_INT(E1MiningTurn, 220, 200, 300);


	PARAM_DOUBLE(StartMineTurn, 298.7642536497108, 200.0, 300.0);
	PARAM_DOUBLE(StartMineTurn_M, 0.32443075715893266, -1.0, 1.0);
	PARAM_DOUBLE(MineStep, 0.5863376548433226, 0.5, 0.9);
	PARAM_DOUBLE(MineStep_M, -0.8131044334367039, -1.0, 1.0);

	PARAM_DOUBLE(StartMineTurn_Eps, 0.07330029408114547, -1.0, 1.0);
	PARAM_DOUBLE(StartMineTurn_Neps, -0.2645595213163313, -1.0, 1.0);
	PARAM_DOUBLE(StartMineTurn_EE, 0, -0.2, 0.2);

	PARAM_DOUBLE(MineStep_Eps, 0.023274586984449314, -1.0, 1.0);
	PARAM_DOUBLE(MineStep_Neps, 0.09715818040456706, -1.0, 1.0);
	PARAM_DOUBLE(MineStep_EE, 0, -0.2, 0.2);


	PARAM_DOUBLE(PredictMargin, 1.5, 1, 2);PARAM_LOWER(1);		
	PARAM_INT(PredictFutureTurn, 11, 1, 15);PARAM_LOWER(1);


	PARAM_DOUBLE(MaximumCutNormLh, -8, -10, -6);		

} HP;


#define DefDoubleParam(name, minValue, maxValue)	\
	static const double name = BlendDoubleParam(HP.name, minValue, maxValue, {\
	ipEE.GetRate(HP.name ## _EE),\
	ipMM.GetRate(HP.name ## _MM),\
	ipDeps.GetRate(HP.name ## _Deps),\
	ipMOR.GetRate(HP.name ## _MOR),\
	});
#define DefIntParam(name, minValue, maxValue)	\
	static const int name = BlendIntParam(HP.name, minValue, maxValue, {\
	ipEE.GetRate(HP.name ## _EE),\
	ipMM.GetRate(HP.name ## _MM),\
	ipDeps.GetRate(HP.name ## _Deps),\
	ipMOR.GetRate(HP.name ## _MOR),\
	});

#define DefDoubleParamFull(name, minValue, maxValue)	\
	static const double name = BlendDoubleParam(HP.name, minValue, maxValue, {\
	ipM.GetRate(HP.name ## _M),\
	ipEps.GetRate(HP.name ## _Eps),\
	ipNeps.GetRate(HP.name ## _Neps),\
	ipEE.GetRate(HP.name ## _EE),\
	ipMM.GetRate(HP.name ## _MM),\
	ipDeps.GetRate(HP.name ## _Deps),\
	ipMOR.GetRate(HP.name ## _MOR),\
	});



struct CostCounter {
	double cost_;
	int mineCost_;
	double surveyCost_;
	int surveyCount_;
	int ansCost_;

	void Init() {
		cost_ = 0;
		mineCost_ = 0;
		surveyCost_ = 0;
		surveyCount_ = 0;
		ansCost_ = 0;
	}

	void Mining() {
		cost_ += 1;
		++mineCost_;
	}

	void Survey(int k) {
		cost_ += 1.0 / sqrt(k);
		surveyCost_ += 1.0 / sqrt(k);
		++surveyCount_;
	}

	void Answer() {
		cost_ += 1;
		++ansCost_;
	}

};



struct SurveyResult {
	NNArr<int> cells;
	int v;
};

struct IOServer {

	int turn_;
	CostCounter counter_;
	NNArr<s8> minedValues_;		
	TArr<SurveyResult> surveyResults_;


	void InitInput(ChronoTimer& timer) {
		istream& is = cin;
		is >> N;
		timer.Init();		
		is >> M;
		is >> eps;
		E = (int)round(eps * 100);
		EE = E * E;
		NN = N * N;
		MM = M * M;
		Deps = 1.0 / eps;
		T = 2 * NN;
		totalValue = 0;
		neps = pow(eps * (1.0 - eps), 0.25);

		int max_m = N * N / 20;
		normM = (M - 2) / (double)(max_m - 2);

		gs.Init(N, N);
		aroundMap.Init(N, N, Around4);
		dirMap.Init(N, N, Around4);
		nearMap1.Init();

		oils.resize(M);

		int d;
		int y, x;
		int shapeId = 0;
		REP(mi, M) {
			auto& oilField = oils[mi];
			is >> d;
			int x_max = 0;
			int y_max = 0;
			int x_sum = 0;
			int y_sum = 0;
			REP(di, d) {
				is >> y >> x;
				int cell = gs.ToId(x, y);
				chmax(x_max, x);
				chmax(y_max, y);
				x_sum += x;
				y_sum += y;

				oilField.cells.add(cell);
				auto& p = oilField.poss.push();
				p.x = x;
				p.y = y;
			}
			oilField.w = x_max + 1;
			oilField.h = y_max + 1;
			oilField.cog.x = x_sum / d;
			oilField.cog.y = y_sum / d;
			int sameShapeId = -1;
			REP(mj, mi) {
				if (oilField.cells == oils[mj].cells) {
					sameShapeId = oils[mj].shapeId;
					break;
				}
			}
			if (sameShapeId < 0) {
				oilField.shapeId = shapeId;
				++shapeId;
			}
			else {
				oilField.shapeId = sameShapeId;
			}

			totalValue += d;
		}

		meanOilRate = totalValue / (double)M / (double)NN;

		turn_ = 0;
		counter_.Init();
		minedValues_.assign(NN, -1);
		surveyResults_.clear();

		ipN.SetValue(N);
		ipNN.SetValue(NN);
		ipEE.SetValue(EE);
		ipM.SetValue(M);
		ipMM.SetValue(MM);
		ipEps.SetValue(eps);
		ipNeps.SetValue(neps);
		ipDeps.SetValue(Deps);
		ipMOR.SetValue(meanOilRate);

	}

	int Mining(int cell) {
		ostream& os = cout;
		cauto& p = gs.ToPos(cell);
		os << "q 1 " << p.y << " " << p.x << endl;

		int v;
		istream& is = cin;
		is >> v;
		minedValues_[cell] = v;
		counter_.Mining();
		++turn_;

		return v;
	}

	int Survey(const NNArr<int>& cells) {
		ostream& os = cout;
		os << "q " << cells.size();
		REP(i, cells.size()) {
			cauto& p = gs.ToPos(cells[i]);
			os << " " << p.y << " " << p.x;
		}
		os << endl;

		int v;
		istream& is = cin;
		is >> v;
		auto& sr = surveyResults_.push();
		sr.cells.MemCopy(cells);
		sr.v = v;

		counter_.Survey(cells.size());
		++turn_;
		return v;
	}

	int Answer(const NNArr<int>& cells) {
		ostream& os = cout;
		os << "a " << cells.size();
		REP(i, cells.size()) {
			cauto& p = gs.ToPos(cells[i]);
			os << " " << p.y << " " << p.x;
		}
		os << endl;

		int v;
		istream& is = cin;
		is >> v;
		if (v == 0) {
			counter_.Answer();
		}
		++turn_;

		return v;
	}

	void Comment(const char* comment) {
		ostream& os = cout;
		os << "#" << comment << endl;
	}

	void Finalize() {
	}
};
IOServer server;



struct ZobristHashD {
private:
	vector<uint64_t> hashMap_;
	int place_;
	int kind_;

public:
	inline void Init(int place, int kind, Xor64& rand) {
		place_ = place;
		kind_ = kind;

		hashMap_.resize(place_ * kind_);
		for (uint64_t& v : hashMap_) {
			v = rand.Get64();
		}
	}

	inline uint64_t Hash(int i, int k) const {
		return hashMap_[k + i * kind_];
	}
};


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
	}

	int GetCount() const {
		return m_count;
	}

};


template <class K, class V>
struct CapHashMap {
	size_t m_capacity = 0;
	K* m_keys = nullptr;
	V* m_values = nullptr;
	uint8_t* m_states = nullptr;
	size_t m_mask = 0;

	int m_count = 0;


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
	inline V* try_get(K key) {
		size_t i = key & m_mask;
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



	V& operator[](K key) {
		auto* p = try_get(key);
		if (p) {
			return *p;
		}
		set(key, {});
		return *try_get(key);
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
		return m_count  / (double)m_capacity;
	}
};

constexpr double DOUBLE_MIN = -numeric_limits<double>::max();

Xor64 rand_;

#define USE_SA_POINT_FILTER 0
#define USE_SA_ROLLBACK 0
#define USE_ACCEPT_SCORE 0

#define USE_LOWER_PROCESS 0




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

template <int CAP>
struct RandomTableS {
	CapArr<int, CAP> table_;

	void push(int value, int count) {
		REP(i, count) {
			table_.push(value);
		}
	}

	template <class ENGINE>
	int operator()(ENGINE& engine) {
		return table_[engine() % table_.size()];
	}
};




struct SAChecker {
	Xor64* rand_ = nullptr;
	double* totalMaxScore_ = nullptr;

	double temp = 0;		
	double divTemp = 0;

	double currentScore = 0;
	double maxScore = 0;

	int noMaxUpdateCount = 0;				
	int nextRollbackCheckCount = INT_MAX;	

	inline bool operator()(double newScore) {
		++noMaxUpdateCount;

		if (newScore > currentScore) {
			currentScore = newScore;
			if (newScore > maxScore) {
				maxScore = newScore;
				noMaxUpdateCount = 0;

				if (newScore > *totalMaxScore_) {
					*totalMaxScore_ = newScore;
				}
			}

			return true;
		}

		else if (newScore == currentScore) {
			return true;
		}

		else {
			if (exp((newScore - currentScore) * divTemp) * UINT32_MAX > (*rand_)(UINT32_MAX)) {
				currentScore = newScore;
				return true;
			}
			else {
				return false;
			}
		}
	}

};

template <class F>
struct SATransition {
	const char* name;
	F func;
	int weight;
};
template <class F>
auto MakeTransition(const char* name, F && func, int weight) {
	return SATransition<F>{ name, func, weight };
}
#define MAKE_TRANS(func, weight) MakeTransition(#func, [&](SAChecker& sac) { func(sa, sac, state_); }, weight)

struct SimulatedAnnealing {
	vector<SAChecker> checkers;

	double totalMaxScore = 0;				
	double timeRate = 0;				
	double temp = 0;					
	double divTemp = 0;					


	Xor64 rand_;

	double startTemp_ = 200;					
	double endTemp_ = 1;						
	int stepLoopCount = 1000;					

	int tempType_ = 0;		
	double tempPow_ = 1.5;

	double rollbackStartRate_ = 999.0;			
	int rollbackCount_ = INT_MAX;				
	double rollbackNextMulti_ = 1.1;			
	int minRollbackCount_ = 1;					


public:
	template <class... TRANSITION>
	void Run2(ChronoTimer& timer, double curEvalScore, tuple<SATransition<TRANSITION>...>& transitions) {

		checkers.resize(1);
		totalMaxScore = curEvalScore;
		REP(pi, checkers.size()) {
			auto& checker = checkers[pi];
			checker.rand_ = &rand_;
			checker.totalMaxScore_ = &totalMaxScore;

			checker.temp = 0;
			checker.divTemp = 0;
			checker.currentScore = curEvalScore;
			checker.maxScore = checker.currentScore;
			checker.noMaxUpdateCount = 0;
			checker.nextRollbackCheckCount = rollbackCount_;

			chmax(totalMaxScore, checker.maxScore);
		}

		RandomTable randTable;
		TupleLoop(transitions, [&](auto&& e, size_t i) {
			randTable.push((int)i, e.weight);
			});

		vector<int> pis(1);
		iota(ALL(pis), 0);

		auto startTime = timer.Now();
		auto endTime = timer.EndTime();
		chmin(startTime, endTime);
		const double subTimeCountDiv = 1.0 / (double)max<long long>((endTime - startTime).count(), 1);

		const double EndDivStart = endTemp_ / startTemp_;

		auto UpdateTemp = [&]() {
			if (tempType_ == 0) {
				temp = startTemp_ + (endTemp_ - startTemp_) * timeRate;	
			}
			else if (tempType_ == 1) {
				temp = startTemp_ * std::pow(EndDivStart, timeRate);		
			}
			else {
				temp = startTemp_ * std::pow(EndDivStart, pow(timeRate, tempPow_));
			}


			divTemp = 1.0 / temp;
			for (auto& checker : checkers) {
				checker.temp = temp;
				checker.divTemp = divTemp;
			}
		};


		timeRate = 0;
		if (startTime == endTime) {
			timeRate = 1;
		}
		UpdateTemp();

		for (u32 loopCount = 1;; ++loopCount) {
			if ((loopCount & 0b111) == 0) {
				auto now = timer.Now();
				if (now >= endTime) {
					break;
				}
				timeRate = (now - startTime).count() * subTimeCountDiv;
				chmin(timeRate, 1);
				UpdateTemp();

			}

			{
				int ti = (int)randTable(rand_);

				auto exec = [&](auto&& e, size_t i) {
					for (int pi : pis) {
						auto& checker = checkers[pi];
						e.func(checker);

					}
				};

				TupleAccess(transitions, ti, exec);
			}

		}

		}

private:
	template <class Tuple, class Func>
	void TupleLoop(Tuple & t, Func && f) {
		TupleLoop2(t, forward<Func>(f), make_index_sequence<tuple_size<Tuple>::value>{});
	}
	template <class Tuple, class Func, size_t... Indics>
	void TupleLoop2(Tuple & t, Func && f, index_sequence<Indics...>) {
		using swallow = int[];
		(void)swallow {
			(TupleLoop3<Tuple, Func, Indics>(t, f), 0)...
		};
	}
	template <class Tuple, class Func, size_t Index>
	void TupleLoop3(Tuple & t, Func & f) {
		f(get<Index>(t), Index);
	}

	template <class Tuple, class Func>
	void TupleAccess(Tuple & t, int i, Func && f) {
		TupleAccess2(t, i, forward<Func>(f), make_index_sequence<tuple_size<Tuple>::value>{});
	}
	template <class Tuple, class Func, size_t... Indics>
	void TupleAccess2(Tuple & t, int i, Func && f, index_sequence<Indics...>) {
		using swallow = int[];
		(void)swallow {
			(TupleAccess3<Tuple, Func, Indics>(t, i, f), 0)...
		};
	}
	template <class Tuple, class Func, size_t Index>
	void TupleAccess3(Tuple & t, int i, Func & f) {
		if (i == Index) {
			f(get<Index>(t), Index);
		}
	}

	};


constexpr double ProbEps = 1e-100;

struct Prober {
	vector<double> logLhs_;		
	int expectSize_;
	int surveySize_;
	int cellSize_;
	int stepK_;

	void Init() {
		expectSize_ = totalValue + 1;
		surveySize_ = min(totalValue * 2, 400);
		cellSize_ = NN + 1;
		stepK_ = surveySize_ * expectSize_;

		logLhs_.assign(cellSize_ * surveySize_ * expectSize_, 1);
	}
	double GetLogLh(int k, int survey, int expect) {
		VASSERT(k >= 0 && k < cellSize_);
		VASSERT(survey >= 0);
		VASSERT(expect >= 0 && expect < expectSize_);

		if (survey >= surveySize_) {
			return Calc(k, survey, expect);
		}

		int i = k * stepK_ + survey * expectSize_ + expect;
		if (logLhs_[i] > 0) {
			logLhs_[i] = Calc(k, survey, expect);
			VASSERT(logLhs_[i] <= 0);
		}
		return logLhs_[i];
	}

private:
	double Calc(int k, int survey, int expect) {
		double mu = (k - expect) * eps + expect * (1.0 - eps);
		double var = k * eps * (1.0 - eps);

		double minV = (survey == 0) ? (-numeric_limits<double>::infinity()) : (survey - 0.5);
		double maxV = survey + 0.5;
		double div = 1.0 / sqrt(2 * var);
		double prob = 0.5 * (erf((maxV - mu) * div) - erf((minV - mu) * div));
		chmax(prob, ProbEps);
		return log(prob);
	}
};


Prober prober_;

struct Patterner {
	MArr<NNSet> oilsCandidates_;			
	MArr<NNArr<int>> oilsCellsOilCount_;	
	NNArr<s8> isOil_;
	NNSet unknownCells_;

	void Init() {
		oilsCandidates_.resize(M);
		oilsCellsOilCount_.resize(M);
		isOil_.assign(NN, -1);
		unknownCells_.fill(NN);

		REP(mi, M) {
			auto& cellsOilCount = oilsCellsOilCount_[mi];
			cellsOilCount.assign(NN, 0);

			auto& candidates = oilsCandidates_[mi];
			cauto& oilField = oils[mi];
			REP(y, N - oilField.h + 1) {
				REP(x, N - oilField.w + 1) {
					candidates.add(gs.ToId(x, y));

					for (int offsetCell : oilField.cells) {
						pint p = gs.ToPos(offsetCell);
						p.x += x;
						p.y += y;
						VASSERT(gs.IsIn(p));
						int cell = gs.ToId(p);
						++cellsOilCount[cell];
					}
				}
			}
		}
		REP(cell, NN) {
			UpdateFixValue(cell);
		}
	}

	void UpdateFixValue(int cell) {
		VASSERT(isOil_[cell] < 0);

		int noOil = 0;
		REP(mi, M) {
			int patternCount = oilsCandidates_[mi].size();
			int oilCount = oilsCellsOilCount_[mi][cell];
			if (patternCount == oilCount) {
				isOil_[cell] = 1;
				unknownCells_.remove(cell);
				return;
			}
			if (oilCount == 0) {
				++noOil;
			}
		}

		if (noOil == M) {
			isOil_[cell] = 0;
			unknownCells_.remove(cell);
			return;
		}
	}

	int CalcMinePos(const NNArr<s8>& predictValues) {
		NNSet aroundOil;
		REP(cell, NN) {
			if (predictValues[cell] == 1) {
				for (int n : aroundMap[cell]) {
					if (predictValues[n] == 0) {
						aroundOil.fadd(n);
					}
				}
			}
		}


		double bestScore = -1e100;
		NNArr<int> bestCells;
		REP(cell, NN) {
			if (isOil_[cell] >= 0) {
				continue;
			}

			double totalPattern = 1;
			double blankPattern = 1;
			REP(mi, M) {
				int patternCount = oilsCandidates_[mi].size();
				int oilCount = oilsCellsOilCount_[mi][cell];
				totalPattern *= patternCount;
				blankPattern *= patternCount - oilCount;
			}

			double blankRate = blankPattern / totalPattern;
			double score = -fabs(blankRate - 0.5);
			if (aroundOil.contains(cell)) {
				score += 1;
			}
			if (score > bestScore) {
				bestScore = score;
				bestCells.clear();
				bestCells.push(cell);
			}
			else if (score == bestScore) {
				bestCells.push(cell);
			}
		}
		VASSERT(bestCells.exist());
		int bestCell = bestCells[rand_(bestCells.size())];
		return bestCell;
	}

	void UpdateMined(int minedCell, int v) {
		VASSERT(isOil_[minedCell] < 0);

		isOil_[minedCell] = v;
		unknownCells_.remove(minedCell);

		if (isOil_[minedCell] > 0) {
			return;
		}

		REP(mi, M) {
			NNSet removes;
			cauto& oilField = oils[mi];
			for (int baseCell : oilsCandidates_[mi]) {
				pint offset = gs.ToPos(minedCell) - gs.ToPos(baseCell);
				if (gs.IsIn(offset)) {
					int checkCell = gs.ToId(offset);
					if (oilField.cells.contains(checkCell)) {
						removes.add(baseCell);
					}
				}
			}

			auto& cellsOilCount = oilsCellsOilCount_[mi];
			for (int baseCell : removes) {
				oilsCandidates_[mi].remove(baseCell);

				cauto& baseP = gs.ToPos(baseCell);
				for (int offsetCell : oilField.cells) {
					pint p = baseP + gs.ToPos(offsetCell);
					VASSERT(gs.IsIn(p));
					int cell = gs.ToId(p);
					--cellsOilCount[cell];

				}
			}
		}

		REP(cell, NN) {
			if (isOil_[cell] >= 0) {
				continue;
			}
			UpdateFixValue(cell);
		}
	}

	bool IsEnd() const {
		return unknownCells_.empty();
	}

	void MakeAns(NNArr<int>& cells) const {
		REP(cell, NN) {
			VASSERT(isOil_[cell] >= 0);
			if (isOil_[cell] > 0) {
				cells.push(cell);
			}
		}
	}

};

Patterner patterner_;
template <int CAPACITY>
struct RandomChoice {
private:
	CapArr<int, CAPACITY> table_;
	int n_ = 0;
	int choicedCount_ = 0;

public:
	bool NotInited() const {
		return n_ == 0;
	}
	void Init(int n) {
		VASSERT(n > 0);
		table_.resize(n);
		iota(table_.begin(), table_.end(), 0);
		n_ = n;
		choicedCount_ = 0;
	}

	int Choice(Xor64& rand) {
		VASSERT(choicedCount_ < n_);
		int i = (int)rand(n_ - choicedCount_);
		int ret = table_[i];
		swap(table_[i], table_[n_ - choicedCount_ - 1]);
		++choicedCount_;
		ResetIfEnd();
		return ret;
	}

	void ResetIfEnd() {
		if (choicedCount_ == n_) {
			choicedCount_ = 0;
		}
	}

	void Reset() {
		choicedCount_ = 0;
	}
};

#define CUT_SURVEY 0			
#define CACHE_SURVEY_VALUE 1	
#define USE_SILHOUETTE_SIZE 0	
#define USE_INVALID_COUNT 1		

struct Likelihooder {
	struct Survey {
		double lh;			
		int v;				
		int prevCommitValue;		
	};
	struct Backup {
		TArr<Survey> surveys_;				
		NNArr<s8> values_;					
		double totalLh_;					
	};

	NNArr<TArr<int>> cellsSurveys_;		
	TArr<Survey> surveys_;				
	NNArr<s8> values_;					
	double totalLh_;					
	NNMap<int> changedCellValues_;		
	int forceInvalidCount_;				
	int easyInvalidCount_;				

	void Init() {
		cellsSurveys_.resize(NN);
		surveys_.clear();
		values_.assign(NN, 0);
		totalLh_ = 0;
		changedCellValues_.clear();
		forceInvalidCount_ = 0;
		easyInvalidCount_ = 0;

	}

	void SyncServer() {
		VASSERT(changedCellValues_.empty());

		FOR(i, surveys_.size(), server.surveyResults_.size()) {
			cauto& sr = server.surveyResults_[i];
			auto& s = surveys_.push();
			s.v = 0;

			for (int cell : sr.cells) {
				cellsSurveys_[cell].push(i);
				s.v += values_[cell];
			}

			s.lh = prober_.GetLogLh(sr.cells.size(), sr.v, s.v);
			s.prevCommitValue = s.v;
		}

		totalLh_ = 0;
		REP(i, server.surveyResults_.size()) {
			cauto& sr = server.surveyResults_[i];
			cauto& s = surveys_[i];
			totalLh_ += s.lh;
		}

		forceInvalidCount_ = 0;
		easyInvalidCount_ = 0;
		REP(cell, NN) {
			IncInvalidCount(cell, values_[cell]);
		}
	}

	void IncInvalidCount(int cell, s8 value) {
		int v = server.minedValues_[cell];
		if (v >= 0) {
			if (value != v) {
				++forceInvalidCount_;

				if (v == 0) {
					++easyInvalidCount_;
				}
			}
		}
		else {
			v = patterner_.isOil_[cell];
			if (v >= 0) {
				if (v == 0) {
					if (value > 0) {
						++forceInvalidCount_;
						++easyInvalidCount_;
					}
				}
				else {
					if (value == 0) {
						++forceInvalidCount_;
					}
				}
			}
		}
		VASSERT(forceInvalidCount_ <= NN);
		VASSERT(easyInvalidCount_ <= NN);
	}
	void DecInvalidCount(int cell, s8 value) {
		int v = server.minedValues_[cell];
		if (v >= 0) {
			if (value != v) {
				--forceInvalidCount_;

				if (v == 0) {
					--easyInvalidCount_;
				}
			}
		}
		else {
			v = patterner_.isOil_[cell];
			if (v >= 0) {
				if (v == 0) {
					if (value > 0) {
						--forceInvalidCount_;
						--easyInvalidCount_;
					}
				}
				else {
					if (value == 0) {
						--forceInvalidCount_;
					}
				}

			}
		}
		VASSERT(forceInvalidCount_ >= 0);
		VASSERT(easyInvalidCount_ >= 0);
	}

	void RemoveOil(int mi, int from) {
		cauto& oilField = oils[mi];
		cauto& baseP = gs.ToPos(from);
		for (cauto& offset : oilField.poss) {
			int cell = gs.ToId(baseP + offset);
			DecInvalidCount(cell, values_[cell]);
			--values_[cell];
			IncInvalidCount(cell, values_[cell]);
			VASSERT(values_[cell] >= 0);
			changedCellValues_.AddValue(cell, -1, 0);
		}
	}

	void SetOil(int mi, int to) {
		cauto& oilField = oils[mi];
		cauto& baseP = gs.ToPos(to);
		for (cauto& offset : oilField.poss) {
			int cell = gs.ToId(baseP + offset);
			++values_[cell];
			IncInvalidCount(cell, values_[cell]);
			changedCellValues_.AddValue(cell, 1, 0);
		}
	}

	void AddOil(int mi, int to) {
		cauto& oilField = oils[mi];
		cauto& baseP = gs.ToPos(to);
		for (cauto& offset : oilField.poss) {
			int cell = gs.ToId(baseP + offset);
			DecInvalidCount(cell, values_[cell]);
			++values_[cell];
			IncInvalidCount(cell, values_[cell]);
			changedCellValues_.AddValue(cell, 1, 0);
		}
	}


	void Commit() {
		static TArr<bool> updated(T, false);
		for (auto [cell, value] : changedCellValues_) {
			if (value) {
				for (int si : cellsSurveys_[cell]) {
					surveys_[si].v += value;
					updated[si] = true;
				}
			}
		}
		REP(si, surveys_.size()) {
			if (updated[si]) {
				updated[si] = false;
				auto& s = surveys_[si];
				if (s.v == s.prevCommitValue) {
					continue;
				}
				s.prevCommitValue = s.v;
				cauto& sr = server.surveyResults_[si];
				double lh = prober_.GetLogLh(sr.cells.size(), sr.v, s.v);

				totalLh_ -= s.lh;
				totalLh_ += lh;
				s.lh = lh;
			}
		}
		changedCellValues_.clear();
	}
};

struct Rect {
	int l, t, r, b;
	void Extend(int size) {
		l -= size;
		t -= size;
		r += size;
		b += size;
	}
	static Rect Make(const OilField& oilField, const pint& cog) {
		int l = cog.x - oilField.cog.x;
		int r = l + oilField.w;
		int t = cog.y - oilField.cog.y;
		int b = t + oilField.h;
		return Rect{ l, t, r, b };
	}
	static bool Intersect(const Rect& a, const Rect& b) {
		if (a.r <= b.l || b.r <= a.l) {
			return false;
		}
		if (a.b <= b.t || b.b <= a.t) {
			return false;
		}
		return true;
	}
	static bool ExtendIntersect(Rect a, Rect b, int size) {
		a.Extend(size);
		b.Extend(size);
		return Intersect(a, b);
	}
};

void Slide(const OilField& oilField, pint& cog) {
	int l = cog.x - oilField.cog.x;
	if (l < 0) {
		cog.x = oilField.cog.x;
	}
	else if (l + oilField.w > N) {
		cog.x = N - (oilField.w - oilField.cog.x);
	}
	int t = cog.y - oilField.cog.y;
	if (t < 0) {
		cog.y = oilField.cog.y;
	}
	else if (t + oilField.h > N) {
		cog.y = N - (oilField.h - oilField.cog.y);
	}
}

ZobristHashD zobrist_;
ZobristHashD silhouetZobrist_;
CapHashSetR<u64> ansedHash_;

CapHashMap<u64, double> lhCache_;


#define USE_NORMALIZE_LH 0

struct State {
private:
	MArr<int> cells_;		
	Likelihooder lher_;
	u64 hash_;

public:
	State() = default;
	State(const State&) = delete;

	const MArr<int>& RefCells() const {
		return cells_;
	}
	const NNArr<s8>& RefValues() const {
		return lher_.values_;
	}
	u64 GetHash() const {
		return hash_;
	}

	bool CheckValidAndSilhouet(u64& silhouetHash) const {
		cauto& values = lher_.values_;
		silhouetHash = 0;
		REP(cell, NN) {
			int v = server.minedValues_[cell];
			if (v >= 0) {
				if (values[cell] != v) {
					VASSERT(!IsForceValid());
					return false;
				}
			}
			else {
				v = patterner_.isOil_[cell];
				if (v >= 0) {
					if ((values[cell] > 0) != (v > 0)) {
						VASSERT(!IsForceValid());
						return false;
					}
				}
			}

			if (values[cell] > 0) {
				silhouetHash ^= silhouetZobrist_.Hash(cell, 0);
			}
		}
		VASSERT(IsForceValid());
		return true;
	}

public:
	void Init() {
		lher_.Init();
		cells_.assign(M, -1);
		hash_ = 0;

		REP(mi, M) {
			cauto& oilField = oils[mi];
			int x = (N - oilField.w) / 2;
			int y = (N - oilField.h) / 2;
			SetInitialCell(mi, gs.ToId(x, y));
		}
	}

	void SetInitialCell(int mi, int cell) {
		cells_[mi] = cell;
		lher_.SetOil(mi, cells_[mi]);
		hash_ ^= zobrist_.Hash(mi, cells_[mi]);
	}

	void SyncServer() {
		lher_.Commit();
		lher_.SyncServer();
		lhCache_.init(1024 * 1024);
	}

	void SetCell(int mi, int cell) {
		if (cells_[mi] != cell) {
			if (cells_[mi] >= 0) {
				lher_.RemoveOil(mi, cells_[mi]);
				hash_ ^= zobrist_.Hash(mi, cells_[mi]);
			}
			cells_[mi] = cell;
			if (cells_[mi] >= 0) {
				lher_.AddOil(mi, cells_[mi]);
				hash_ ^= zobrist_.Hash(mi, cells_[mi]);
			}
		}
	}
	void SetCell(const MArr<int>& cells) {
		REP(mi, M) {
			SetCell(mi, cells[mi]);
		}
	}

	static double NormalizeLh(double lh) {
		return lh / (double)(server.surveyResults_.size() * neps);
	}

	double CalcLikelihood() {
		{
			double* lh = lhCache_.try_get(hash_);
			if (lh) {
				return *lh;
			}
		}

		lher_.Commit();

		double logLh = lher_.totalLh_;

		static const double InvalidProbLog = log(HP.InvalidProb);
		logLh += InvalidProbLog * (lher_.easyInvalidCount_ + lher_.forceInvalidCount_);


		if (lhCache_.size() + 1 < lhCache_.capacity()) {
			lhCache_.set(hash_, logLh);
		}
		return logLh;
	}

	bool IsForceValid() const {
		return lher_.forceInvalidCount_ == 0;
	}

	bool IsEasyValid() const {
		return lher_.easyInvalidCount_ == 0;
	}

	int GetForceInvalidCount() const {
		return lher_.forceInvalidCount_;
	}

	static double CalcLikelihoodOld(const MArr<int>& cells) {
		NNArr<s8> values;
		MakeGrid(cells, values);

		double logLh = 0;
		for (cauto& sr : server.surveyResults_) {
			int sum = 0;
			for (int cell : sr.cells) {
				sum += values[cell];
			}
			logLh += prober_.GetLogLh(sr.cells.size(), sr.v, sum);
		}



		return logLh;
	}

	static int CalcInvalidCount(const NNArr<s8>& values) {
		int invalidCount = 0;
		REP(cell, NN) {
			int v = server.minedValues_[cell];
			if (v >= 0) {
				if (values[cell] != v) {
					if (v == 0) {
						invalidCount += abs(values[cell] - v) + 1;
					}
					else {
						invalidCount += abs(values[cell] - v);
					}
				}
			}
			else {
				v = patterner_.isOil_[cell];
				if (v >= 0) {
					if (v == 0) {
						if (values[cell] > 0) {
							invalidCount += 2;
						}
					}
					else {
						if (values[cell] == 0) {
							++invalidCount;
						}
					}

				}
			}
		}
		return invalidCount;
	}

	static void MakeGrid(const MArr<int>& cells, NNArr<s8>& values) {
		values.assign(NN, 0);
		REP(mi, M) {
			if (cells[mi] < 0) {
				continue;
			}
			cauto& oilField = oils[mi];
			cauto& basePos = gs.ToPos(cells[mi]);
			for (cauto& offset : oilField.poss) {
				int cell = gs.ToId(basePos + offset);
				++values[cell];
			}
		}
	}

};


static constexpr int HighScoreCountMax = 128;
struct HighScores {
	double lh;
	MArr<int> cells;
	u64 hash;

	bool operator < (const HighScores& r) const {
		return lh > r.lh;
	}
};
using MinQue = CapPriorityQueue<HighScores, HighScoreCountMax>;

struct Backup {
	MinQue minQue_;
	CapHashSetR<u64> usedHash_;

	void Init() {
		minQue_.clear();
		usedHash_.init(HP.HighScoreCount * 3);
	}

	void Add(const State& state, double newLh) {
		static auto cmp = MinQue::Less();

		VASSERT(state.IsEasyValid());
		u64 hash = state.GetHash();
		if (usedHash_.contains(hash)) {
			return;
		}

		if (minQue_.size() < HP.HighScoreCount) {
			HighScores highScore;
			highScore.lh = newLh;
			highScore.cells = state.RefCells();
			highScore.hash = hash;

			minQue_.push(highScore, cmp);
			usedHash_.add(hash);
		}
		else if (newLh > minQue_.top().lh) {
			auto old = minQue_.pop(cmp);
			usedHash_.remove(old.hash);
			VASSERT(minQue_.size() == usedHash_.GetCount());

			HighScores highScore;
			highScore.lh = newLh;
			highScore.cells = state.RefCells();
			highScore.hash = hash;

			minQue_.push(highScore, cmp);
			usedHash_.add(hash);
		}

		VASSERT(minQue_.size() == usedHash_.GetCount());
	}

};

Backup easyBackup_;

void InitBackup() {
	easyBackup_.Init();
}

void BackupState(const State& state, double newLh) {


	easyBackup_.Add(state, newLh);
};

MArr<int> bestCells_;
double bestEvalScore_;

void CheckBest(const State& state, double lh) {
	if (lh > bestEvalScore_) {
		bestEvalScore_ = lh;
		bestCells_ = state.RefCells();
	}
}

pint GetCog(int mi, int cell) {
	return gs.ToPos(cell) + oils[mi].cog;
}

int CalcTargetCogCell(int mi, pint cog) {
	Slide(oils[mi], cog);
	return gs.ToId(cog - oils[mi].cog);
}

enum class SwapType {
	Stay,
	Jump,		
	Swap,		
	Rotate,		
};

enum class MoveType {
	None,		
	Neighbor,	
	Rect1,		
	Rect2,		
	Rect3,		
	Circle,		
	All,		
};

enum class ChoiceType {
	Random,		
	Greedy,		
	Comb,		
};

void Transition_Common(SimulatedAnnealing& sa, SAChecker& checker, State& state_, int targetCount, SwapType swapType, MoveType moveType, ChoiceType choiceType) {
	cauto& cells = state_.RefCells();

	if (targetCount > M) {
		return;
	}

	static RandomChoice<M_max> choiceTbl;
	choiceTbl.Init(M);
	CapArr<int, 3> mis;
	REP(i, targetCount) {
		mis.push(choiceTbl.Choice(rand_));
	}

	CapArr<int, 3> backups(mis.size());		
	REP(i, mis.size()) {
		backups[i] = cells[mis[i]];
	}

	auto Revert = [&]() {
		REP(i, mis.size()) {
			state_.SetCell(mis[i], backups[i]);
		}
	};

	if (swapType == SwapType::Jump) {
		VASSERT(targetCount == 1);

		int miA = mis[0];
		int miB = choiceTbl.Choice(rand_);
		VASSERT(miA != miB);
		cauto& oilFieldA = oils[miA];
		cauto& oilFieldB = oils[miB];

		pint cogA = gs.ToPos(cells[miA]) + oilFieldA.cog;
		pint cogB = gs.ToPos(cells[miB]) + oilFieldB.cog;
		cogA = cogB;
		Slide(oilFieldA, cogA);

		int newCellA = gs.ToId(cogA - oilFieldA.cog);
		state_.SetCell(miA, newCellA);
	}
	else if (swapType == SwapType::Swap) {
		VASSERT(targetCount == 2);

		int miA = mis[0];
		int miB = mis[1];
		cauto& oilFieldA = oils[miA];
		cauto& oilFieldB = oils[miB];

		if (oilFieldA.shapeId == oilFieldB.shapeId) {
			return;
		}

		pint cogA = gs.ToPos(cells[miA]) + oilFieldA.cog;
		pint cogB = gs.ToPos(cells[miB]) + oilFieldB.cog;
		swap(cogA, cogB);
		Slide(oilFieldA, cogA);
		Slide(oilFieldB, cogB);

		int newCellA = gs.ToId(cogA - oilFieldA.cog);
		int newCellB = gs.ToId(cogB - oilFieldB.cog);
		state_.SetCell(miA, newCellA);
		state_.SetCell(miB, newCellB);
	}
	else if (swapType == SwapType::Rotate) {
		VASSERT(targetCount == 3);


		int miA = mis[0];
		int miB = mis[1];
		int miC = mis[2];
		cauto& oilFieldA = oils[miA];
		cauto& oilFieldB = oils[miB];
		cauto& oilFieldC = oils[miC];

		if (oilFieldA.shapeId == oilFieldB.shapeId || oilFieldB.shapeId == oilFieldC.shapeId || oilFieldC.shapeId == oilFieldA.shapeId) {
			return;
		}

		pint cogA = gs.ToPos(cells[miA]) + oilFieldA.cog;
		pint cogB = gs.ToPos(cells[miB]) + oilFieldB.cog;
		pint cogC = gs.ToPos(cells[miC]) + oilFieldC.cog;
		auto tmp = cogA;
		cogA = cogB;
		cogB = cogC;
		cogC = tmp;

		Slide(oilFieldA, cogA);
		Slide(oilFieldB, cogB);
		Slide(oilFieldC, cogC);

		int newCellA = gs.ToId(cogA - oilFieldA.cog);
		int newCellB = gs.ToId(cogB - oilFieldB.cog);
		int newCellC = gs.ToId(cogC - oilFieldC.cog);
		state_.SetCell(miA, newCellA);
		state_.SetCell(miB, newCellB);
		state_.SetCell(miC, newCellC);
	}

	CapArr<NNArr<int>, 3> candidateCells(mis.size());
	REP(i, mis.size()) {
		int mi = mis[i];
		auto& cands = candidateCells[i];
		cauto& enables = patterner_.oilsCandidates_[mi];

		if (moveType == MoveType::None) {
			VASSERT(swapType == SwapType::Swap || swapType == SwapType::Rotate);
			cands.push(cells[mi]);
		}
		else if (moveType == MoveType::Neighbor) {
			for (int n : aroundMap[cells[mi]]) {
				if (enables.contains(n)) {
					cands.push(n);
				}
			}
			if (cands.empty()) {
				Revert();
				return;
			}
		}
		else if (moveType == MoveType::Rect1 || moveType == MoveType::Rect2 || moveType == MoveType::Rect3) {
			int size = 0;
			if (moveType == MoveType::Rect1) {
				size = 1;
			}
			else if (moveType == MoveType::Rect2) {
				size = 2;
			}
			else if (moveType == MoveType::Rect3) {
				size = 3;
			}
			cauto& center = gs.ToPos(cells[mi]);
			FOR(oy, -size, size + 1) {
				int y = center.y + oy;
				FOR(ox, -size, size + 1) {
					int x = center.x + ox;

					if (targetCount == 1 && oy == 0 && ox == 0) {
						continue;
					}

					if (gs.IsIn(x, y)) {
						int n = gs.ToId(x, y);
						if (enables.contains(n)) {
							cands.push(n);
						}
					}
				}
			}
			if (cands.empty()) {
				Revert();
				return;
			}
		}
		else if (moveType == MoveType::Circle) {
			cauto& center = gs.ToPos(cells[mi]);
			FOR(oy, -2, 3) {
				int y = center.y + oy;
				FOR(ox, -2, 3) {
					int x = center.x + ox;

					if (targetCount == 1 && oy == 0 && ox == 0) {
						continue;
					}

					if (abs(oy) + abs(ox) > 2) {
						continue;
					}

					if (gs.IsIn(x, y)) {
						int n = gs.ToId(x, y);
						if (enables.contains(n)) {
							cands.push(n);
						}
					}
				}
			}
			if (cands.empty()) {
				Revert();
				return;
			}
		}
		else if (moveType == MoveType::All) {
			VASSERT(swapType == SwapType::Stay);		
			for (int n : enables) {
				cands.push(n);
			}
		}
	}


	if (choiceType == ChoiceType::Random) {
		REP(i, mis.size()) {
			int mi = mis[i];
			int n = candidateCells[i].choice(rand_);
			state_.SetCell(mi, n);
		}

		double lh = state_.CalcLikelihood();
		BackupState(state_, lh);

		if (checker(lh)) {
			CheckBest(state_, lh);
		}
		else {
			Revert();
		}
	}
	else if (choiceType == ChoiceType::Greedy) {
		VASSERT(moveType != MoveType::None);

		REP(i, mis.size()) {
			int mi = mis[i];
			state_.SetCell(mi, -1);
		}

		REP(i, mis.size()) {
			int mi = mis[i];

			int bestCell = -1;
			double bestLh = DOUBLE_MIN;
			for (int cell : candidateCells[i]) {
				state_.SetCell(mi, cell);
				double lh = state_.CalcLikelihood();
				if (lh > bestLh) {
					bestLh = lh;
					bestCell = cell;
				}

				if (i == mis.size() - 1) {
					BackupState(state_, lh);
				}
			}
			VASSERT(bestCell >= 0);
			state_.SetCell(mi, bestCell);
		}

		double lh = state_.CalcLikelihood();

		if (checker(lh)) {
			CheckBest(state_, lh);
		}
		else {
			Revert();
		}
	}
	else if (choiceType == ChoiceType::Comb) {
		VASSERT(moveType != MoveType::None);

		VASSERT(targetCount >= 2);

		MArr<int> bestCells;
		double bestLh = DOUBLE_MIN;
		auto dfs = [&](auto&& dfs, int i) -> void {
			if (i >= mis.size()) {
				double lh = state_.CalcLikelihood();
				BackupState(state_, lh);

				if (lh > bestLh) {
					bestLh = lh;
					bestCells = cells;
				}
			}
			else {
				int mi = mis[i];
				for (int cell : candidateCells[i]) {
					state_.SetCell(mi, cell);
					dfs(dfs, i + 1);
				}
			}
		};
		dfs(dfs, 0);

		VASSERT(bestCells.size());
		state_.SetCell(bestCells);
		double lh = state_.CalcLikelihood();

		if (checker(lh)) {
			CheckBest(state_, lh);
		}
		else {
			Revert();
		}
	}
}

void Trans_OneOil_Stay_Neighbor_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Neighbor, ChoiceType::Random);
}
void Trans_OneOil_Stay_Rect1_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Rect1, ChoiceType::Random);
}
void Trans_OneOil_Stay_Rect2_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Rect2, ChoiceType::Random);
}
void Trans_OneOil_Stay_Rect3_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Rect3, ChoiceType::Random);
}
void Trans_OneOil_Stay_Circle_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Circle, ChoiceType::Random);
}
void Trans_OneOil_Stay_All_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::All, ChoiceType::Random);
}

void Trans_OneOil_Stay_Neighbor_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Neighbor, ChoiceType::Greedy);
}
void Trans_OneOil_Stay_Rect1_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Rect1, ChoiceType::Greedy);
}
void Trans_OneOil_Stay_Rect2_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Rect2, ChoiceType::Greedy);
}
void Trans_OneOil_Stay_Circle_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::Circle, ChoiceType::Greedy);
}
void Trans_OneOil_Stay_All_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Stay, MoveType::All, ChoiceType::Greedy);
}

void Trans_OneOil_Jump_Neighbor_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Neighbor, ChoiceType::Random);
}
void Trans_OneOil_Jump_Rect1_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Rect1, ChoiceType::Random);
}
void Trans_OneOil_Jump_Rect2_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Rect2, ChoiceType::Random);
}
void Trans_OneOil_Jump_Rect3_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Rect3, ChoiceType::Random);
}
void Trans_OneOil_Jump_Circle_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Circle, ChoiceType::Random);
}
void Trans_OneOil_Jump_All_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::All, ChoiceType::Random);
}

void Trans_OneOil_Jump_Neighbor_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Neighbor, ChoiceType::Greedy);
}
void Trans_OneOil_Jump_Rect1_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Rect1, ChoiceType::Greedy);
}
void Trans_OneOil_Jump_Rect2_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Rect2, ChoiceType::Greedy);
}
void Trans_OneOil_Jump_Circle_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::Circle, ChoiceType::Greedy);
}
void Trans_OneOil_Jump_All_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 1, SwapType::Jump, MoveType::All, ChoiceType::Greedy);
}



void Trans_TwoOil_Stay_Neighbor_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Neighbor, ChoiceType::Random);
}
void Trans_TwoOil_Stay_Rect1_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Rect1, ChoiceType::Random);
}
void Trans_TwoOil_Stay_Rect2_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Rect2, ChoiceType::Random);
}
void Trans_TwoOil_Stay_Rect3_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Rect3, ChoiceType::Random);
}
void Trans_TwoOil_Stay_Circle_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Circle, ChoiceType::Random);
}
void Trans_TwoOil_Stay_All_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::All, ChoiceType::Random);
}

void Trans_TwoOil_Stay_Neighbor_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Neighbor, ChoiceType::Greedy);
}
void Trans_TwoOil_Stay_Rect1_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Rect1, ChoiceType::Greedy);
}
void Trans_TwoOil_Stay_Rect2_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Rect2, ChoiceType::Greedy);
}
void Trans_TwoOil_Stay_Circle_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Circle, ChoiceType::Greedy);
}
void Trans_TwoOil_Stay_All_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::All, ChoiceType::Greedy);
}

void Trans_TwoOil_Stay_Neighbor_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Neighbor, ChoiceType::Comb);
}
void Trans_TwoOil_Stay_Rect1_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Rect1, ChoiceType::Comb);
}
void Trans_TwoOil_Stay_Rect2_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Rect2, ChoiceType::Comb);
}
void Trans_TwoOil_Stay_Circle_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::Circle, ChoiceType::Comb);
}
void Trans_TwoOil_Stay_All_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Stay, MoveType::All, ChoiceType::Comb);
}

void Trans_TwoOil_Swap_None_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::None, ChoiceType::Random);
}
void Trans_TwoOil_Swap_Neighbor_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Neighbor, ChoiceType::Random);
}
void Trans_TwoOil_Swap_Rect1_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Rect1, ChoiceType::Random);
}
void Trans_TwoOil_Swap_Rect2_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Rect2, ChoiceType::Random);
}
void Trans_TwoOil_Swap_Rect3_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Rect3, ChoiceType::Random);
}
void Trans_TwoOil_Swap_Circle_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Circle, ChoiceType::Random);
}

void Trans_TwoOil_Swap_Neighbor_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Neighbor, ChoiceType::Greedy);
}
void Trans_TwoOil_Swap_Rect1_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Rect1, ChoiceType::Greedy);
}
void Trans_TwoOil_Swap_Rect2_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Rect2, ChoiceType::Greedy);
}
void Trans_TwoOil_Swap_Circle_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Circle, ChoiceType::Greedy);
}

void Trans_TwoOil_Swap_Neighbor_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Neighbor, ChoiceType::Comb);
}
void Trans_TwoOil_Swap_Rect1_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Rect1, ChoiceType::Comb);
}
void Trans_TwoOil_Swap_Rect2_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Rect2, ChoiceType::Comb);
}
void Trans_TwoOil_Swap_Circle_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 2, SwapType::Swap, MoveType::Circle, ChoiceType::Comb);
}

void Trans_ThreeOil_Stay_Neighbor_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Neighbor, ChoiceType::Random);
}
void Trans_ThreeOil_Stay_Rect1_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Rect1, ChoiceType::Random);
}
void Trans_ThreeOil_Stay_Rect2_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Rect2, ChoiceType::Random);
}
void Trans_ThreeOil_Stay_Circle_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Circle, ChoiceType::Random);
}
void Trans_ThreeOil_Stay_All_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::All, ChoiceType::Random);
}

void Trans_ThreeOil_Stay_Neighbor_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Neighbor, ChoiceType::Greedy);
}
void Trans_ThreeOil_Stay_Rect1_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Rect1, ChoiceType::Greedy);
}
void Trans_ThreeOil_Stay_Rect2_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Rect2, ChoiceType::Greedy);
}
void Trans_ThreeOil_Stay_Circle_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Circle, ChoiceType::Greedy);
}
void Trans_ThreeOil_Stay_All_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::All, ChoiceType::Greedy);
}

void Trans_ThreeOil_Stay_Neighbor_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Neighbor, ChoiceType::Comb);
}
void Trans_ThreeOil_Stay_Rect1_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Rect1, ChoiceType::Comb);
}
void Trans_ThreeOil_Stay_Rect2_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Rect2, ChoiceType::Comb);
}
void Trans_ThreeOil_Stay_Circle_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::Circle, ChoiceType::Comb);
}
void Trans_ThreeOil_Stay_All_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Stay, MoveType::All, ChoiceType::Comb);
}

void Trans_ThreeOil_Rotate_None_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::None, ChoiceType::Random);
}
void Trans_ThreeOil_Rotate_Neighbor_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Neighbor, ChoiceType::Random);
}
void Trans_ThreeOil_Rotate_Rect1_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Rect1, ChoiceType::Random);
}
void Trans_ThreeOil_Rotate_Rect2_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Rect2, ChoiceType::Random);
}
void Trans_ThreeOil_Rotate_Circle_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Circle, ChoiceType::Random);
}

void Trans_ThreeOil_Rotate_Neighbor_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Neighbor, ChoiceType::Greedy);
}
void Trans_ThreeOil_Rotate_Rect1_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Rect1, ChoiceType::Greedy);
}
void Trans_ThreeOil_Rotate_Rect2_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Rect2, ChoiceType::Greedy);
}
void Trans_ThreeOil_Rotate_Circle_Greedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Circle, ChoiceType::Greedy);
}

void Trans_ThreeOil_Rotate_Neighbor_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Neighbor, ChoiceType::Comb);
}
void Trans_ThreeOil_Rotate_Rect1_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Rect1, ChoiceType::Comb);
}
void Trans_ThreeOil_Rotate_Rect2_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Rect2, ChoiceType::Comb);
}
void Trans_ThreeOil_Rotate_Circle_Comb(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
	Transition_Common(sa, checker, state, 3, SwapType::Rotate, MoveType::Circle, ChoiceType::Comb);
}



template <class Tuple, class Func>
void TupleLoop(Tuple& t, Func&& f) {
	TupleLoop2(t, forward<Func>(f), make_index_sequence<tuple_size<Tuple>::value>{});
}
template <class Tuple, class Func, size_t... Indics>
void TupleLoop2(Tuple& t, Func&& f, index_sequence<Indics...>) {
	using swallow = int[];
	(void)swallow {
		(TupleLoop3<Tuple, Func, Indics>(t, f), 0)...
	};
}
template <class Tuple, class Func, size_t Index>
void TupleLoop3(Tuple& t, Func& f) {
	f(get<Index>(t), Index);
}

template <class Tuple, class Func>
void TupleAccess(Tuple& t, int i, Func&& f) {
	TupleAccess2(t, i, forward<Func>(f), make_index_sequence<tuple_size<Tuple>::value>{});
}
template <class Tuple, class Func, size_t... Indics>
void TupleAccess2(Tuple& t, int i, Func&& f, index_sequence<Indics...>) {
	using swallow = int[];
	(void)swallow {
		(TupleAccess3<Tuple, Func, Indics>(t, i, f), 0)...
	};
}
template <class Tuple, class Func, size_t Index>
void TupleAccess3(Tuple& t, int i, Func& f) {
	if (i == Index) {
		f(get<Index>(t), Index);
	}
}

struct Transer {
	double startTemp_;
	double endTemp_;
	int tempType_;
	double tempPow_;
	double endDivStart_;

	RandomTable randTable_;

	int nowStep_;			
	int maxStep_;			
	SAChecker checker_;

	vector<double> tempTbl_;



	void Init() {
		DefDoubleParam(StartTemp, 0.0, 1e10);
		DefDoubleParam(EndTemp, 0.0, 1e10);
		DefIntParam(TransMaxStep, 0, INT_MAX);



		startTemp_ = StartTemp;
		endTemp_ = EndTemp;
		tempType_ = HP.TempType;
		tempPow_ = HP.TempPow;
		endDivStart_ = endTemp_ / startTemp_;

		randTable_.clear();

		nowStep_ = TransMaxStep;
		maxStep_ = TransMaxStep;


		tempTbl_.resize(TransMaxStep + 1);
		REP(step, TransMaxStep + 1) {
			double progress = step / (double)maxStep_;
			tempTbl_[step] = CalcTemp(progress);
		}
	}

	template <class... TRANSITION>
	void MakeRandTable(tuple<SATransition<TRANSITION>...>& transitions) {
		randTable_.clear();
		TupleLoop(transitions, [&](auto&& e, size_t i) {
			randTable_.push((int)i, e.weight);
			});
	}

	void ResetScore(double curScore) {
		{
			auto& checker = checker_;
			checker.currentScore = curScore;
			checker.maxScore = checker.currentScore;
		}
	}

	void Ready(double curScore) {
		nowStep_ = 0;

		{
			auto& checker = checker_;
			checker.rand_ = &rand_;
			static double dummy;
			checker.totalMaxScore_ = &dummy;

			checker.temp = 0;
			checker.divTemp = 0;
			checker.currentScore = curScore;
			checker.maxScore = checker.currentScore;
			checker.noMaxUpdateCount = 0;
			checker.nextRollbackCheckCount = 0;
		}
	}

	template <class... TRANSITION>
	void UpdateStep(tuple<SATransition<TRANSITION>...>& transitions) {
		int ti = (int)randTable_(rand_);

		auto exec = [&](auto&& e, size_t i) {
			auto& checker = checker_;
			e.func(checker);
			if (checker.noMaxUpdateCount >= checker.nextRollbackCheckCount) {
			}
		};

		TupleAccess(transitions, ti, exec);
		++nowStep_;
		VASSERT(nowStep_ <= maxStep_);
	}

	bool IsEndLoop() const {
		return nowStep_ >= maxStep_;
	}

	void UpdateTemp(double tempScale) {
		double temp = tempTbl_[nowStep_] * tempScale;

		double divTemp = 1.0 / temp;
		{
			auto& checker = checker_;
			checker.temp = temp;
			checker.divTemp = divTemp;
		}
	}

	double CalcTemp(double progress) const {
		if (tempType_ == 0) {
			return startTemp_ + (endTemp_ - startTemp_) * progress;	
		}
		else if (tempType_ == 1) {
			return startTemp_ * std::pow(endDivStart_, progress);		
		}
		else {
			return startTemp_ * std::pow(endDivStart_, pow(progress, tempPow_));
		}
	}
};

struct Worker {
	State state_;

	void Init() {
		state_.Init();
	}

	void SyncServer() {
		state_.SyncServer();
	}

	int CalcMineTarget() {
		cauto& buf = easyBackup_.minQue_.buf_;
		CapArr<NNArr<s8>, HighScoreCountMax> highScoresValues;
		REP(i, buf.size()) {
			cauto& highScore = buf[i];

			State::MakeGrid(highScore.cells, highScoresValues.push());
		}

		cauto& cells = state_.RefCells();
		cauto& values = state_.RefValues();

		NNSet aroundOilCells;		
		NNSet aroundBlankCells;		
		REP(cell, NN) {
			if (values[cell] == 1) {
				bool hasBlank = false;
				for (int n : aroundMap[cell]) {
					if (values[n] == 0) {
						aroundOilCells.fadd(n);
						hasBlank = true;
					}
				}
				if (hasBlank) {
					aroundBlankCells.add(cell);
				}
			}
			else if (values[cell] > 0) {
				bool hasBlank = false;
				for (int n : aroundMap[cell]) {
					if (values[n] == 0) {
						hasBlank = true;
						break;
					}
				}
				if (hasBlank) {
					aroundBlankCells.add(cell);
				}
			}
		}

		NNSet noFixOilFieldCells;
		REP(mi, M) {
			cauto& oilField = oils[mi];
			cauto& baseP = gs.ToPos(cells[mi]);
			bool hasFix = false;
			for (cauto& offset : oilField.poss) {
				int cell = gs.ToId(baseP + offset);
				if (patterner_.isOil_[cell] >= 0) {
					hasFix = true;
					break;
				}
			}
			if (!hasFix) {
				for (cauto& offset : oilField.poss) {
					int cell = gs.ToId(baseP + offset);
					noFixOilFieldCells.fadd(cell);
				}
			}
		}

		double bestVar = DOUBLE_MIN;
		NNArr<int> targetCells;
		NNArr<double> scores(NN);
		for (int cell : patterner_.unknownCells_) {
			bool aroundOil = aroundOilCells.contains(cell);
			bool aroundBlank = aroundBlankCells.contains(cell);

			double score = 0;

			if (highScoresValues.size()) {
				int sum = 0;
				int sum2 = 0;
				for (cauto& values : highScoresValues) {
					int v = values[cell];
					sum += v;
					sum2 += v * v;
				}
				double count = highScoresValues.size();
				double mean = sum / count;
				double var = sum2 / count - square(mean);

				score += var;
			}

			double totalPattern = 1;
			double blankPattern = 1;
			REP(mi, M) {
				int patternCount = patterner_.oilsCandidates_[mi].size();
				int oilCount = patterner_.oilsCellsOilCount_[mi][cell];
				totalPattern *= patternCount;
				blankPattern *= patternCount - oilCount;
			}
			double blankRate = blankPattern / totalPattern;
			VASSERT(blankRate > 0 && blankRate < 1);

			if (aroundOil) {
				score += 10000;
			}
			if (aroundBlank) {
				score += 10000;
			}
			score -= fabs(0.5 - blankRate) * 0.5;





			if (noFixOilFieldCells.contains(cell)) {
				score += 0.0001;
			}

			scores[cell] = score;
			targetCells.push(cell);
		}
		targetCells.sort([&](int a, int b) {
			return scores[a] > scores[b];
			});

		return targetCells.front();
	}

	void CalcSurveyTarget(NNArr<int>& targetCells, bool timeOver) {
		targetCells.clear();

		DefDoubleParamFull(SurveyRequireCountRate, 0, 1);
		static const int RequireCount = int(NN * SurveyRequireCountRate);

		NNSet underOilCells;		
		if (easyBackup_.minQue_.buf_.empty()) {
			VABORT();
			cauto& values = state_.RefValues();
			REP(cell, NN) {
				if (values[cell] > 0) {
					targetCells.push(cell);
				}
			}
		}
		else {
			static CapArr<HighScores, HighScoreCountMax> highScores;
			highScores.MemCopy(easyBackup_.minQue_.buf_);
			highScores.sort(); 

			double bestLh = highScores[0].lh;
			CapArr<double, HighScoreCountMax> probs;
			REP(i, min(highScores.size(), 2)) {
				double lh = highScores[i].lh;
				double prob = exp(lh - bestLh);
				probs.push(prob);
			}
			discrete_distribution<int> dist(ALL(probs));
			int baseIdx = dist(rand_);

			NNArr<s8> values;
			State::MakeGrid(highScores[baseIdx].cells, values);
			const u64 baseHash = highScores[baseIdx].hash;



			NNArr<s8> otherValues;

			REP(i, highScores.size()) {
				cauto& highScore = highScores[i];
				if (highScore.hash == baseHash) {
					continue;
				}


				State::MakeGrid(highScore.cells, otherValues);

				bool diffShilhouet = false;
				REP(cell, NN) {
					if ((values[cell] > 0) != (otherValues[cell] > 0)) {
						diffShilhouet = true;
						break;
					}
				}
				if (!diffShilhouet) {
					continue;
				}

				REP(cell, NN) {
					if (patterner_.isOil_[cell] >= 0) {
						continue;
					}
					if (values[cell] > otherValues[cell]) {
						underOilCells.fadd(cell);
					}
				}

				if (underOilCells.size() >= RequireCount) {
					break;
				}
			}
		}

		if (underOilCells.size() > 0) {
			for (int cell : underOilCells) {
				targetCells.push(cell);
			}
			if (targetCells.size() > RequireCount) {
				targetCells.shuffle(rand_);
				targetCells.resize(RequireCount);
			}
			if (targetCells.size() < RequireCount) {
				REP(cell, NN) {
					if (patterner_.isOil_[cell] > 0) {
						targetCells.push(cell);
						if (targetCells.size() >= RequireCount) {
							break;
						}
					}
				}
			}
		}
	}


	double GetMaximumThreshold() const {
		DefDoubleParamFull(MaximumProb, 0.0, 1e10);
		DefDoubleParamFull(WrongDown, 0, 1e10);


		static const double MaximumProbLog = log(MaximumProb);
		static const double WrongDownLog = log(WrongDown);
		return MaximumProbLog + WrongDownLog * ansedHash_.size();
	}

	bool IsMaximum() {
		if (!state_.IsForceValid()) {
			return false;
		}

		if (server.surveyResults_.size() == 0) {
			return false;
		}
		double baseLh = state_.CalcLikelihood();
		double normLh = State::NormalizeLh(baseLh);
		if (normLh < HP.MaximumCutNormLh) {	
			return false;
		}

		const double threshold = GetMaximumThreshold();
		cauto& values = state_.RefValues();

		u64 baseHash = state_.GetHash();
		static CapArr<HighScores, HighScoreCountMax> highScores;
		highScores.MemCopy(easyBackup_.minQue_.buf_);
		highScores.sort();
		for (cauto& highScore : highScores) {
			if (highScore.hash == baseHash) {
				continue;
			}

			if (baseLh >= highScore.lh + threshold) {
				break;
			}

			static NNArr<s8> otherValues;
			State::MakeGrid(highScore.cells, otherValues);

			bool same = true;
			REP(cell, NN) {
				if ((values[cell] > 0) != (otherValues[cell] > 0)) {
					same = false;
					break;
				}
			}
			if (same) {
				continue;
			}

			return false;
		}
		return true;
	}



	void Transition_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& cells = state_.RefCells();

		auto backupCells = state_.RefCells();
		REP(mi, M) {
			cauto& cand = patterner_.oilsCandidates_[mi];
			state_.SetCell(mi, cand.choice(rand_));
		}

		double lh = state_.CalcLikelihood();
		BackupState(state_, lh);

		if (checker(lh)) {
			CheckBest(state_, lh);
		}
		else {
			state_.SetCell(backupCells);
		}
	}

	void Transition_SingleMove(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& cells = state_.RefCells();
		int mi = rand_(M);

		static RandomChoice<4> choice;
		choice.Init(4);

		int di = -1;
		{
			cauto& oilField = oils[mi];
			cauto& baseP = gs.ToPos(cells[mi]);
			cauto& candidates = patterner_.oilsCandidates_[mi];

			REP(i, 4) {
				int tmpDi = choice.Choice(rand_);
				pint p = baseP + Around4[tmpDi];
				if (gs.IsOut(p)) {
					continue;
				}
				int cell = gs.ToId(p);
				if (!candidates.contains(cell)) {
					continue;
				}
				di = tmpDi;
				break;
			}
			if (di < 0) {
				return;
			}
		}

		int backupCell = cells[mi];
		int nextCell = dirMap[cells[mi]][di];

		state_.SetCell(mi, nextCell);

		double lh = state_.CalcLikelihood();

		BackupState(state_, lh);

		if (checker(lh)) {
			CheckBest(state_, lh);
		}
		else {
			state_.SetCell(mi, backupCell);
		}
	}

	void Transition_SingleJump(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& cells = state_.RefCells();

		int mi = rand_(M);
		cauto& candidates = patterner_.oilsCandidates_[mi];
		int nextCell = candidates.choice(rand_);
		if (cells[mi] == nextCell) {
			nextCell = candidates[rand_(candidates.size())];
			if (cells[mi] == nextCell) {
				return;
			}
		}

		int backupCell = cells[mi];

		state_.SetCell(mi, nextCell);

		double lh = state_.CalcLikelihood();

		BackupState(state_, lh);

		if (checker(lh)) {
			CheckBest(state_, lh);
		}
		else {
			state_.SetCell(mi, backupCell);
		}
	}

	void Transition_MultiJump(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& cells = state_.RefCells();
		int miA = rand_(M);
		int miB = rand_(M - 1);
		if (miB >= miA) {
			++miB;
		}

		int backupA = cells[miA];
		int backupB = cells[miB];
		cauto& candidatesA = patterner_.oilsCandidates_[miA];
		cauto& candidatesB = patterner_.oilsCandidates_[miB];

		state_.SetCell(miA, candidatesA[rand_(candidatesA.size())]);
		state_.SetCell(miB, candidatesB[rand_(candidatesB.size())]);

		double lh = state_.CalcLikelihood();

		BackupState(state_, lh);

		if (checker(lh)) {
			CheckBest(state_, lh);
		}
		else {
			state_.SetCell(miA, backupA);
			state_.SetCell(miB, backupB);
		}
	}

	void Transition_SingleGreedy(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& cells = state_.RefCells();
		int mi = rand_(M);
		cauto& candidates = patterner_.oilsCandidates_[mi];
		int backupCell = cells[mi];

		double bestLh = DOUBLE_MIN;
		int bestCell = -1;
		for (int newCell : candidates) {
			state_.SetCell(mi, newCell);
			double newLh = state_.CalcLikelihood();


			if (newLh > bestLh) {
				bestLh = newLh;
				bestCell = newCell;
			}
		}
		state_.SetCell(mi, bestCell);

		BackupState(state_, bestLh);

		if (checker(bestLh)) {
			CheckBest(state_, bestLh);
		}
		else {
			state_.SetCell(mi, backupCell);
		}
	}

	void Transition_SimpleSwap(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& cells = state_.RefCells();
		int miA = rand_(M);
		int miB = rand_(M - 1);
		if (miB >= miA) {
			++miB;
		}

		cauto& oilFieldA = oils[miA];
		cauto& oilFieldB = oils[miB];

		if (oilFieldA.shapeId == oilFieldB.shapeId) {
			return;
		}

		int backupA = cells[miA];
		int backupB = cells[miB];

		auto Revert = [&]() {
			state_.SetCell(miA, backupA);
			state_.SetCell(miB, backupB);
		};

		cauto& candidatesA = patterner_.oilsCandidates_[miA];
		cauto& candidatesB = patterner_.oilsCandidates_[miB];


		pint cogA = gs.ToPos(cells[miA]) + oilFieldA.cog;
		pint cogB = gs.ToPos(cells[miB]) + oilFieldB.cog;

		swap(cogA, cogB);

		Slide(oilFieldA, cogA);
		Slide(oilFieldB, cogB);

		state_.SetCell(miA, gs.ToId(cogA - oilFieldA.cog));
		state_.SetCell(miB, gs.ToId(cogB - oilFieldB.cog));

		double lh = state_.CalcLikelihood();

		BackupState(state_, lh);

		if (checker(lh)) {
			CheckBest(state_, lh);
		}
		else {
			Revert();
		}
	}

	void Transition_SwapMove(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& cells = state_.RefCells();
		int miA = rand_(M);
		int miB = rand_(M - 1);
		if (miB >= miA) {
			++miB;
		}

		cauto& oilFieldA = oils[miA];
		cauto& oilFieldB = oils[miB];

		if (oilFieldA.shapeId == oilFieldB.shapeId) {
			return;
		}

		int backupA = cells[miA];
		int backupB = cells[miB];

		auto Revert = [&]() {
			state_.SetCell(miA, backupA);
			state_.SetCell(miB, backupB);
		};

		cauto& candidatesA = patterner_.oilsCandidates_[miA];
		cauto& candidatesB = patterner_.oilsCandidates_[miB];


		pint cogA = gs.ToPos(cells[miA]) + oilFieldA.cog;
		pint cogB = gs.ToPos(cells[miB]) + oilFieldB.cog;

		swap(cogA, cogB);

		Slide(oilFieldA, cogA);
		Slide(oilFieldB, cogB);

		state_.SetCell(miA, -1);
		state_.SetCell(miB, -1);

		Rect rectA = Rect::Make(oilFieldA, cogA);
		Rect rectB = Rect::Make(oilFieldB, cogB);

		pint basePointA = cogA - oilFieldA.cog;
		pint basePointB = cogB - oilFieldB.cog;

		bool combination = false;
		int moveSize = 1;
			if (Rect::ExtendIntersect(rectA, rectB, 1)) {
				combination = true;
				moveSize = 1;
			}
			else {
				combination = false;
				moveSize = 1;
			}

		if (combination) {
			double bestLh = DOUBLE_MIN;
			int bestCellA = -1;
			int bestCellB = -1;
			FOR(oyA, -moveSize, moveSize + 1) {
				int yA = basePointA.y + oyA;
				FOR(oxA, -moveSize, moveSize + 1) {
					int xA = basePointA.x + oxA;
					if (gs.IsOut(xA, yA)) {
						continue;
					}
					int cellA = gs.ToId(xA, yA);
					if (!candidatesA.contains(cellA)) {
						continue;
					}
					state_.SetCell(miA, cellA);

					FOR(oyB, -moveSize, moveSize + 1) {
						int yB = basePointB.y + oyB;
						FOR(oxB, -moveSize, moveSize + 1) {
							int xB = basePointB.x + oxB;
							if (gs.IsOut(xB, yB)) {
								continue;
							}
							int cellB = gs.ToId(xB, yB);
							if (!candidatesB.contains(cellB)) {
								continue;
							}
							state_.SetCell(miB, cellB);

							double lh = state_.CalcLikelihood();

							BackupState(state_, lh);

							if (lh > bestLh) {
								bestLh = lh;
								bestCellA = cellA;
								bestCellB = cellB;
							}
						}
					}
				}
			}

			if (bestCellA < 0) {
				Revert();
				return;
			}

			if (checker(bestLh)) {
				state_.SetCell(miA, bestCellA);
				state_.SetCell(miB, bestCellB);
				CheckBest(state_, bestLh);
			}
			else {
				Revert();
			}
		}
		else {
			auto SetBest = [&](int mi, const OilField& oilField, const NNSet& candidates, cauto& basePoint, double& bestLh, bool enableBackup) -> bool {
				bestLh = DOUBLE_MIN;
				int bestCell = -1;
				FOR(oy, -moveSize, moveSize + 1) {
					int y = basePoint.y + oy;
					FOR(ox, -moveSize, moveSize + 1) {
						int x = basePoint.x + ox;
						if (gs.IsOut(x, y)) {
							continue;
						}
						int cell = gs.ToId(x, y);
						if (!candidates.contains(cell)) {
							continue;
						}

						state_.SetCell(mi, cell);

						double lh = state_.CalcLikelihood();

						if (enableBackup) {
							BackupState(state_, lh);
						}

						if (lh > bestLh) {
							bestLh = lh;
							bestCell = cell;
						}
					}
				}
				if (bestCell < 0) {
					return false;
				}

				state_.SetCell(mi, bestCell);

				return true;
			};

			double bestLh = 0;
			if (!SetBest(miA, oilFieldA, candidatesA, basePointA, bestLh, false)) {
				Revert();
				return;
			}
			if (!SetBest(miB, oilFieldB, candidatesB, basePointB, bestLh, true)) {
				state_.SetCell(miB, backupB);
				bestLh = state_.CalcLikelihood();
				BackupState(state_, bestLh);
			}

			if (checker(bestLh)) {
				CheckBest(state_, bestLh);
			}
			else {
				Revert();
			}
		}
	}

	void Transition_SwapMoveRect1(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& cells = state_.RefCells();
		int miA = rand_(M);
		int miB = rand_(M - 1);
		if (miB >= miA) {
			++miB;
		}

		cauto& oilFieldA = oils[miA];
		cauto& oilFieldB = oils[miB];

		if (oilFieldA.shapeId == oilFieldB.shapeId) {
			return;
		}

		int backupA = cells[miA];
		int backupB = cells[miB];

		auto Revert = [&]() {
			state_.SetCell(miA, backupA);
			state_.SetCell(miB, backupB);
		};

		cauto& candidatesA = patterner_.oilsCandidates_[miA];
		cauto& candidatesB = patterner_.oilsCandidates_[miB];


		pint cogA = GetCog(miA, cells[miA]);
		pint cogB = GetCog(miB, cells[miB]);
		swap(cogA, cogB);
		int baseCellA = CalcTargetCogCell(miA, cogA);
		int baseCellB = CalcTargetCogCell(miB, cogB);

		state_.SetCell(miA, -1);
		state_.SetCell(miB, -1);

		auto SetBest = [&](int mi, const OilField& oilField, const NNSet& candidates, int baseCell, double& bestLh, bool enableBackup) -> bool {
			bestLh = DOUBLE_MIN;
			int bestCell = -1;
			for (int cell : nearMap1[baseCell]) {
				if (!candidates.contains(cell)) {
					continue;
				}

				state_.SetCell(mi, cell);

				double lh = state_.CalcLikelihood();

				if (enableBackup) {
					BackupState(state_, lh);
				}

				if (lh > bestLh) {
					bestLh = lh;
					bestCell = cell;
				}
			}
			if (bestCell < 0) {
				return false;
			}

			state_.SetCell(mi, bestCell);

			return true;
		};

		double bestLh = -1e100;
		if (!SetBest(miA, oilFieldA, candidatesA, baseCellA, bestLh, false)) {
			Revert();
			return;
		}
		if (!SetBest(miB, oilFieldB, candidatesB, baseCellB, bestLh, true)) {
			state_.SetCell(miB, backupB);
			bestLh = state_.CalcLikelihood();
			BackupState(state_, bestLh);
		}

		if (checker(bestLh)) {
			CheckBest(state_, bestLh);
		}
		else {
			Revert();
		}
	}



	void FixInvalidCells() {
		cauto& cells = state_.RefCells();
		REP(mi, M) {
			cauto& candidates = patterner_.oilsCandidates_[mi];
			if (!candidates.contains(cells[mi])) {
				state_.SetCell(mi, candidates[rand_(candidates.size())]);
			}
		}
	}

	void FixGreedyInvalidCells() {
		cauto& cells = state_.RefCells();
		REP(mi, M) {
			cauto& candidates = patterner_.oilsCandidates_[mi];
			if (!candidates.contains(cells[mi])) {
				int bestCell = candidates[0];
				double bestLh = -1e100;
				for (int cell : candidates) {
					state_.SetCell(mi, cell);
					double lh = state_.CalcLikelihood();
					if (lh > bestLh) {
						bestLh = lh;
						bestCell = cell;
					}
				}
				if (bestCell >= 0) {
					state_.SetCell(mi, bestCell);
				}
			}
		}
	}

	SimulatedAnnealing sa;

	bool initedTranser_ = false;
	Transer transer_;
	MArr<int> resumeCells_;

	void UpdatePos2(bool timeOver) {
		static const int Trans_SwapMoveRect1 = BlendIntParam(HP.Trans_SwapMoveRect1, 0, 200, {
				ipM.GetRate(HP.Trans_SwapMoveRect1_M),
			});

		static auto transitions = make_tuple(
			MAKE_TRANS(Transition_SingleMove, HP.Trans_SingleMove)
			, MAKE_TRANS(Transition_SingleJump, HP.Trans_SingleJump)
			, MAKE_TRANS(Transition_SwapMove, HP.Trans_SwapMove)			
			, MAKE_TRANS(Transition_SwapMoveRect1, Trans_SwapMoveRect1)		

		);

		FixInvalidCells();

		bestEvalScore_ = state_.CalcLikelihood();
		bestCells_ = state_.RefCells();

		{
			auto easyBuf = easyBackup_.minQue_.buf_;

			InitBackup();
			for (cauto& highScore : easyBuf) {
				state_.SetCell(highScore.cells);
				if (state_.IsEasyValid()) {
					double lh = state_.CalcLikelihood();
					BackupState(state_, lh);
				}
			}
		}

		static bool canComb = false;
		static bool neverComb = false;
		bool oneTimeComb = false;
		if (!canComb && !neverComb) {
			int CombMax = server.turn_ <= 1 ? 200000 : 20000;		
			int combCount = 1;
			REP(mi, M) {
				combCount *= patterner_.oilsCandidates_[mi].size();
				if (combCount >= CombMax) {
					break;
				}
			}
			if (combCount < CombMax) {
				if (server.turn_ >= 2) {
					oneTimeComb = true;
					neverComb = true;
				}
				else {
					canComb = true;
				}

			}
		}
		if (!timeOver && (oneTimeComb || canComb)) {
			auto dfs = [&](auto&& dfs, int mi) -> void {
				if (mi >= M) {
					double lh = state_.CalcLikelihood();
					BackupState(state_, lh);
					CheckBest(state_, lh);
				}
				else {
					for (int cell : patterner_.oilsCandidates_[mi]) {
						state_.SetCell(mi, cell);
						dfs(dfs, mi + 1);
					}
				}
			};
			dfs(dfs, 0);
		}
		else {
			VASSERT(M != 2);
			if (!initedTranser_) {
				transer_.Init();
				{
					transer_.MakeRandTable(transitions);
				}

				initedTranser_ = true;
			}

			{
				constexpr int divE = 9;
				constexpr double base[2] = { 6.94706904268242, 4.11840397770514 };
				constexpr double scale[2] = { 0.173712682113199, 39.4915670750594 };
				constexpr double expM[2] = { 2.45056405928244, 1.67315641862777 };
				constexpr double expStd[2] = { -0.00183583186080626, 1.50051991314182 };
				constexpr double mulMeanOil[2] = { -0.387245962975972, 0.217420034037147 };
				constexpr double dsum[2] = { 0.349706695278369, -0.0385107935859979 };

				int idx = E < divE ? 0 : 1;
				static const double BasePredictTurn = (
					base[idx]
					+ scale[idx]
						* pow(M, expM[idx])
						* pow(eps * (1 - eps), expStd[idx])
						/ pow(meanOilRate, mulMeanOil[idx])
						* pow(totalValue, dsum[idx])
					) * HP.PredictMargin;

				int predictTurn = max(int(BasePredictTurn), server.turn_ + HP.PredictFutureTurn);
				chmin(predictTurn, T);

				int curTime = timer_.ElapseTimeUs();
				int timeLeft = TIME_LIMIT_US - curTime;
				int turnLeft = predictTurn - server.turn_;
				int turnTime = int(timeLeft / (double)turnLeft);
				chmax(turnTime, 0);
				timer_.StartUs(curTime + turnTime);
			}

			if (!resumeCells_.empty()) {
				state_.SetCell(resumeCells_);
				FixInvalidCells();
			}

			transer_.ResetScore(state_.CalcLikelihood());

			auto GetTempScale = [&]() {
				DefDoubleParam(TempScaleBase, -1e10, 0);
				DefDoubleParam(TempScaleMul, 0.0, 1e10);


				double lh = state_.CalcLikelihood();
				double normLh = State::NormalizeLh(lh);
				if (normLh < TempScaleBase) {
					return 1 + -(normLh - TempScaleBase) * TempScaleMul;
				}
				return 1.0;
			};

			for (u32 loopCount = 1;; ++loopCount) {
				if (transer_.IsEndLoop()) {
					double curScore = state_.CalcLikelihood();
					transer_.Ready(curScore);
				}

				if ((loopCount & 0b111) == 0) {
					if (timer_.IsOver()) {
						break;
					}
				}

				{
					transer_.UpdateTemp(GetTempScale());
					transer_.UpdateStep(transitions);
				}
			}

			resumeCells_.MemCopy(state_.RefCells());
		}

		REP(mi, M) {
			state_.SetCell(mi, bestCells_[mi]);
		}

	}


	void MakeAns(NNArr<int>& cells) const {
		cauto& values = state_.RefValues();
		REP(cell, NN) {
			if (values[cell] > 0) {
				cells.push(cell);
			}
		}
	}

};

#define USE_NO_SURVEY 0

struct FirstPatternSurveyer {
	NNArr<NNArr<int>> groups[2];
	NNArr<NNArr<int>>* curGroups;
	NNArr<NNArr<int>>* nextGroups;
	NNArr<Rect> groupsRect;
	int axis;
	int phase;

	void Init() {
		axis = 0;
		phase = 0;
		PhaesInit();

	}

	void PhaesInit() {
		curGroups = &groups[0];
		nextGroups = &groups[1];
		curGroups->resize(1);
		(*curGroups)[0].clear();

		REP(cell, NN) {
			if (patterner_.unknownCells_.contains(cell)) {
				(*curGroups)[0].push(cell);
			}
		}
		groupsRect.clear();
		auto& rect = groupsRect.push();
		rect.l = 0;
		rect.t = 0;
		rect.r = N;
		rect.b = N;
	}

	bool Survey(const Worker& worker) {

		if (N >= HP.FirstPatternThresholdN && M >= HP.FirstPatternThresholdM) {
			if (Survey0(worker)) {
				return true;
			}
		}
		else {
			if (Survey4(worker)) {
				return true;
			}
		}

		return false;
	}

	bool Survey0(const Worker& worker) {
		NNArr<int> cells;
		bool hasDiv = false;

		{
			if (axis >= 4) {
				return false;
			}
		}

		if (axis % 2 == 0) {
			nextGroups->clear();
			for (auto& group : *curGroups) {
				if (group.size() == 1) {
					nextGroups->push(group);
					cells.append(group);
				}
				else {
					hasDiv = true;
					group.stable_sort([&](int a, int b) {
						return gs.ToPos(a).y < gs.ToPos(b).y;
						});

					{
						auto& next = nextGroups->push();
						next.clear();
						REP(i, group.size() / 2) {
							next.push(group[i]);
						}
						cells.append(next);
					}
					{
						auto& next = nextGroups->push();
						next.clear();
						FOR(i, group.size() / 2, group.size()) {
							next.push(group[i]);
						}
					}
				}
			}
		}
		else {
			nextGroups->clear();
			for (auto& group : *curGroups) {
				if (group.size() == 1) {
					nextGroups->push(group);
					cells.append(group);
				}
				else {
					hasDiv = true;
					group.stable_sort([&](int a, int b) {
						return gs.ToPos(a).x < gs.ToPos(b).x;
						});

					{
						auto& next = nextGroups->push();
						next.clear();
						REP(i, group.size() / 2) {
							next.push(group[i]);
						}
						cells.append(next);
					}
					{
						auto& next = nextGroups->push();
						next.clear();
						FOR(i, group.size() / 2, group.size()) {
							next.push(group[i]);
						}
					}
				}
			}
		}

		++axis;
		swap(curGroups, nextGroups);

		if (!hasDiv) {
			return false;
		}

		int v = server.Survey(cells);

		return true;
	}
	bool Survey0_turn2(const Worker& worker) {
		NNArr<int> cells;
		bool hasDiv = false;

		{
			if (axis >= 2) {
				return false;
			}
		}

		if (axis % 2 == 0) {
			nextGroups->clear();
			for (auto& group : *curGroups) {
				if (group.size() == 1) {
					nextGroups->push(group);
					cells.append(group);
				}
				else {
					hasDiv = true;
					group.stable_sort([&](int a, int b) {
						return gs.ToPos(a).y < gs.ToPos(b).y;
						});

					{
						auto& next = nextGroups->push();
						next.clear();
						REP(i, group.size() / 2) {
							next.push(group[i]);
						}
						cells.append(next);
					}
					{
						auto& next = nextGroups->push();
						next.clear();
						FOR(i, group.size() / 2, group.size()) {
							next.push(group[i]);
						}
					}
				}
			}
		}
		else {
			nextGroups->clear();
			for (auto& group : *curGroups) {
				if (group.size() == 1) {
					nextGroups->push(group);
					cells.append(group);
				}
				else {
					hasDiv = true;
					group.stable_sort([&](int a, int b) {
						return gs.ToPos(a).x < gs.ToPos(b).x;
						});

					{
						auto& next = nextGroups->push();
						next.clear();
						REP(i, group.size() / 2) {
							next.push(group[i]);
						}
						cells.append(next);
					}
					{
						auto& next = nextGroups->push();
						next.clear();
						FOR(i, group.size() / 2, group.size()) {
							next.push(group[i]);
						}
					}
				}
			}
		}

		++axis;
		swap(curGroups, nextGroups);

		if (!hasDiv) {
			return false;
		}

		int v = server.Survey(cells);

		return true;
	}

	bool Survey1(const Worker& worker) {
		NNArr<int> cells;
		bool hasDiv = false;

		nextGroups->clear();
		for (auto& group : *curGroups) {
			if (group.size() == 1) {
				nextGroups->push(group);
				cells.append(group);
			}
			else {
				hasDiv = true;
				auto Dist = [&](int cell) {
					pint p = gs.ToPos(cell);
					static pdouble center = pdouble{ (N - 1) / 2.0, (N - 1) / 2.0 };
					double d = norm(ToDouble(p) - center);
					return d;
				};
				group.stable_sort([&](int a, int b) {
					return Dist(a) < Dist(b);
					});

				{
					auto& next = nextGroups->push();
					next.clear();
					REP(i, group.size() / 2) {
						next.push(group[i]);
					}
					cells.append(next);
				}
				{
					auto& next = nextGroups->push();
					next.clear();
					FOR(i, group.size() / 2, group.size()) {
						next.push(group[i]);
					}
				}
			}
		}
		swap(curGroups, nextGroups);
		nextGroups->clear();

		if (!hasDiv) {
			return false;
		}

		int v = server.Survey(cells);

		return true;
	}

	bool Survey2(const Worker& worker) {
		NNArr<int> cells;
		bool hasDiv = false;
		bool hasNoDiv = false;

		static NNArr<Rect> nextGroupsRect;
		nextGroupsRect.clear();

		REP(gi, groupsRect.size()) {
			cauto& rect = groupsRect[gi];
			int w = rect.r - rect.l;
			int h = rect.b - rect.t;
			int cx = rect.l + w / 2;
			int cy = rect.t + h / 2;

			if (w <= 1 || h <= 1) {
				hasNoDiv = true;
				cells.push(gs.ToId(rect.l, rect.t));
			}
			else {
				hasDiv = true;

				cauto& rect = groupsRect[gi];

				{
					auto& nextRect = nextGroupsRect.push();
					nextRect.l = rect.l;
					nextRect.t = rect.t;
					nextRect.r = cx;
					nextRect.b = cy;
				}

				{
					auto& nextRect = nextGroupsRect.push();
					nextRect.l = cx;
					nextRect.t = rect.t;
					nextRect.r = rect.r;
					nextRect.b = cy;
					FOR(y, nextRect.t, nextRect.b) {
						FOR(x, nextRect.l, nextRect.r) {
							cells.push(gs.ToId(x, y));
						}
					}
				}

				{
					auto& nextRect = nextGroupsRect.push();
					nextRect.l = rect.l;
					nextRect.t = cy;
					nextRect.r = cx;
					nextRect.b = rect.b;
					FOR(y, nextRect.t, nextRect.b) {
						FOR(x, nextRect.l, nextRect.r) {
							cells.push(gs.ToId(x, y));
						}
					}
				}

				{
					auto& nextRect = nextGroupsRect.push();
					nextRect.l = cx;
					nextRect.t = cy;
					nextRect.r = rect.r;
					nextRect.b = rect.b;
				}
			}
		}
		groupsRect.MemCopy(nextGroupsRect);

		if (hasNoDiv) {
			return false;
		}
		if (!hasDiv) {
			return false;
		}

		int v = server.Survey(cells);

		return true;
	}

	bool Survey3(const Worker& worker) {
		NNArr<int> cells;

		int size = 1 << axis;
		if (size * 2 > N) {
			return false;
		}
		REP(y, N) {
			int yb = (y + axis) / (size) % 2;
			REP(x, N) {
				int xb = (x + N - axis) / (size) % 2;
				if ((xb + yb) % 2 == 0) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		++axis;
		int v = server.Survey(cells);

		return true;
	}

	bool Survey4(const Worker& worker) {
		NNArr<int> cells;
		if (axis == 0) {
			REP(y, N) {
				REP(x, N / 2) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 1) {
			REP(y, N / 2) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else {
			return false;
		}
		++axis;
		int v = server.Survey(cells);

		return true;
	}

	bool Survey4B(const Worker& worker) {
		NNArr<int> cells;
		if (axis == 0) {
			REP(y, N) {
				REP(x, N / 2) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 1) {
			REP(y, N / 2) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 2) {
			REP(y, N) {
				REP(x, N / 4) {
					cells.push(gs.ToId(x, y));
				}
				FOR(x, N / 2, N * 3 / 4) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 3) {
			REP(y, N / 4) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
			FOR(y, N / 2, N * 3 / 4) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else {
			return false;
		}
		++axis;
		int v = server.Survey(cells);

		return true;
	}

	bool Survey4C(const Worker& worker) {
		NNArr<int> cells;
		if (axis == 0) {
			REP(y, N) {
				REP(x, N / 2) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 1) {
			REP(y, N / 2) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 2) {
			REP(y, N) {
				REP(x, N / 4) {
					cells.push(gs.ToId(x, y));
				}
				FOR(x, N / 2, N * 3 / 4) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 3) {
			REP(y, N / 4) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
			FOR(y, N / 2, N * 3 / 4) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 4) {
			REP(y, N) {
				FOR(x, N * 0 / 8, N * 1 / 8) {
					cells.push(gs.ToId(x, y));
				}
				FOR(x, N * 2 / 8, N * 3 / 8) {
					cells.push(gs.ToId(x, y));
				}
				FOR(x, N * 4 / 8, N * 5 / 8) {
					cells.push(gs.ToId(x, y));
				}
				FOR(x, N * 6 / 8, N * 7 / 8) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 5) {
			FOR(y, N * 0 / 8, N * 1 / 8) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
			FOR(y, N * 2 / 8, N * 3 / 8) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
			FOR(y, N * 4 / 8, N * 5 / 8) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
			FOR(y, N * 6 / 8, N * 7 / 8) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else {
			return false;
		}
		++axis;
		int v = server.Survey(cells);

		return true;
	}

	bool Survey5(const Worker& worker) {
		NNArr<int> cells;
		if (axis == 0) {
			REP(y, N) {
				REP(x, N / 3) {
					cells.push(gs.ToId(x, y));
				}
				FOR(x, (N * 2 + 2) / 3, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 1) {
			REP(y, N / 3) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
			FOR(y, (N * 2 + 2) / 3, N) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else {
			return false;
		}
		++axis;
		int v = server.Survey(cells);

		return true;
	}

	bool Survey6(const Worker& worker) {
		NNArr<int> cells;
		if (axis == 0) {
			REP(y, N) {
				REP(x, N / 3) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 1) {
			REP(y, N) {
				FOR(x, (N * 2 + 2) / 3, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 2) {
			REP(y, N / 3) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else if (axis == 3) {
			FOR(y, (N * 2 + 2) / 3, N) {
				REP(x, N) {
					cells.push(gs.ToId(x, y));
				}
			}
		}
		else {
			return false;
		}
		++axis;
		int v = server.Survey(cells);

		return true;
	}

	bool Survey7(const Worker& worker) {
		int sumW = 0;
		int sumH = 0;
		int maxW = 0;
		int maxH = 0;
		REP(mi, M) {
			cauto& oil = oils[mi];
			sumW += oil.w;
			sumH += oil.h;
			chmax(maxW, oil.w);
			chmax(maxH, oil.h);
		}
		double meanW = sumW / (double)M;
		double meanH = sumH / (double)M;
		int oilW = max(1, (int)floor(meanW));
		int oilH = max(1, (int)floor(meanH));

		NNArr<int> cells;
		if (axis == 0) {
			if (oilW * 4 > N) {
				++axis;
			}
			else
			{
				REP(y, N) {
					REP(x, N) {
						if ((x / (oilW * 2)) % 2 == 0) {
							cells.push(gs.ToId(x, y));
						}
					}
				}
			}
		}
		if (axis == 1) {
			if (oilH * 4 > N) {
				++axis;
			}
			else
			{
				REP(y, N) {
					if ((y / (oilH * 2)) % 2 == 0) {
						REP(x, N) {
							cells.push(gs.ToId(x, y));
						}
					}
				}
			}
		}
		if (axis == 2) {
			if (oilW * 2 > N) {
				++axis;
			}
			else
			{
				REP(y, N) {
					REP(x, N) {
						if ((x / oilW) % 2 == 0) {
							cells.push(gs.ToId(x, y));
						}
					}
				}
			}
		}
		if (axis == 3) {
			if (oilH * 2 > N) {
				++axis;
			}
			else
			{
				REP(y, N) {
					if ((y / oilH) % 2 == 0) {
						REP(x, N) {
							cells.push(gs.ToId(x, y));
						}
					}
				}
			}
		}

		if (axis == 4) {
			return false;
		}
		++axis;
		int v = server.Survey(cells);

		return true;
	}
};

struct Solver {

	void Run(ChronoTimer& timer) {
		zobrist_.Init(M, NN, rand_);
		silhouetZobrist_.Init(NN, 1, rand_);
		prober_.Init();
		patterner_.Init();

		static Worker worker;
		worker.Init();
		worker.SyncServer();


		ansedHash_.init(T);


		bool timeOver = false;


		static FirstPatternSurveyer firstSurveyer;
		firstSurveyer.Init();
		bool completeFirstSurvey = false;


		static const int StartMineTurn = BlendIntParam(HP.StartMineTurn, 0, T_max, {
			ipM.GetRate(HP.StartMineTurn_M),
			ipEps.GetRate(HP.StartMineTurn_Eps),
			ipNeps.GetRate(HP.StartMineTurn_Neps),
			ipEE.GetRate(HP.StartMineTurn_EE),
			});
		static const double MineStep = BlendDoubleParam(HP.MineStep, 0, 1, {
			ipM.GetRate(HP.MineStep_M),
			ipEps.GetRate(HP.MineStep_Eps),
			ipNeps.GetRate(HP.MineStep_Neps),
			ipEE.GetRate(HP.MineStep_EE),
			});
		double step = 1;

		while (server.turn_ < T) {
			if (patterner_.IsEnd() || server.turn_ + 1 == T) {
				int v = Answer(worker);
				break;
			}

			if (server.turn_ > 0) {
				u64 hash = 0;
				bool valid = worker.state_.CheckValidAndSilhouet(hash);
				if (valid && !ansedHash_.contains(hash) && worker.IsMaximum()) {
					int v = Answer(worker);
					if (v == 1) {
						break;
					}
					else {
						ansedHash_.add(hash);
						continue;
					}
				}
			}

			if (!timeOver && timer.ElapseTimeMs() >= TIME_LIMIT) {
				timeOver = true;
			}
			if (!completeFirstSurvey) {
				if (!firstSurveyer.Survey(worker)) {
					completeFirstSurvey = true;
				}
			}

			if (completeFirstSurvey) {


				{
					bool mining = false;
					if ((T - server.turn_) - patterner_.unknownCells_.size() < 1) {
						mining = true;
					}
					else if (E == 1 && server.turn_ >= HP.E1MiningTurn) {
						mining = true;
					}
					else if (server.turn_ >= StartMineTurn) {
						step += MineStep;
						if (step >= 1) {
							step -= 1;
							mining = true;
						}
					}
					if (mining) {
						TargetMining(worker);
					}
					else {
						Survey(worker, timeOver);

					}
				}
			}

			worker.SyncServer();
			worker.UpdatePos2(timeOver);


		}


	}

	void PatternFilterMining(const Worker& worker) {
		cauto& predictValues = worker.state_.RefValues();
		int cell = patterner_.CalcMinePos(predictValues);
		int v = server.Mining(cell);
		patterner_.UpdateMined(cell, v);

	};

	void TargetMining(Worker& worker) {
		int cell = worker.CalcMineTarget();
		int v = server.Mining(cell);
		patterner_.UpdateMined(cell, v);

	};

	void Survey(Worker& worker, bool timeOver) {
		NNArr<int> cells;

		worker.CalcSurveyTarget(cells, timeOver);
		if (cells.size() < 2) {
			RandomSurvey(worker);
			return;
		}

		VASSERT(cells.size() >= 2);
		int v = server.Survey(cells);

	};

	void RandomSurvey(Worker& worker) {
		NNArr<int> cells;

		auto unknownCells = patterner_.unknownCells_;
		if (unknownCells.size() < 2) {
			TargetMining(worker);
			return;
		}

		int count = max(unknownCells.size() / 3, 2);
		REP(i, count) {
			int cell = unknownCells[rand_(unknownCells.size())];
			cells.push(cell);
			unknownCells.remove(cell);
		}
		VASSERT(cells.size() >= 2);

		int v = server.Survey(cells);

		};

	int Answer(const Worker& worker) {
		NNArr<int> cells;
		if (patterner_.IsEnd()) {
			patterner_.MakeAns(cells);
		}
		else {
			worker.MakeAns(cells);
		}
		int v = server.Answer(cells);

		return v;
	};
};


struct Main {
    void Run(int argc, const char* argv[]) {
        server.InitInput(timer_);

        static Solver solver;
        timer_.StartMs(TIME_LIMIT);

        solver.Run(timer_);

        server.Finalize();

    }
};
