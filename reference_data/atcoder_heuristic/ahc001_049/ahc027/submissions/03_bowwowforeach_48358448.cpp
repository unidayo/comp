
#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0


#define TIME_LIMIT (1950)

#define NOT_SUBMIT 0
#define VALIDATION 0

#define IO_FILE 0

#define OUTPUT_INFO 0
#define OUTPUT_FINAL_INFO 0
#define OUTPUT_LOG 0
#define OUTPUT_VISUAL 0
#define OUTPUT_PERF 0

#define FIX_RESULT 0


#define TIME_LIMIT_US (TIME_LIMIT * 1000)


#define USE_DIST3 0

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
	cerr << fixed;
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

	void RemoveInsert(int start, int end, const T* p, int size) {
		int newEnd = start + size;
		if (size_ - end > 0 && newEnd != end) {
			memmove(data() + newEnd, data() + end, sizeof(T) * (size_ - end));
		}

		memcpy(data() + start, p, sizeof(T) * size);

		size_ -= end - start;
		size_ += size;
	}

	void stable_sort() {
		::stable_sort(begin(), end());
	}
	template <class LESS>
	void stable_sort(LESS&& less) {
		::stable_sort(begin(), end(), less);
	}

	void reverse() {
		::reverse(begin(), end());
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
	using Key = conditional<CAP <= 256, u8, u32>::type;
	CapArr<Key, CAP> elemens;		
	array<Key, CAP> indexs;		


public:

	bool operator == (const CapSet<CAP>& r) const {
		if (elemens.size() != r.elemens.size()) {
			return false;
		}
		for (Key i : elemens) {
			if (!r.IsContain(i)) {
				return false;
			}
		}
		return true;
	}

	constexpr int capacity() {
		return CAP;
	}

	void fill() {
		elemens.resize(CAP);
		iota(ALL(elemens), 0);
		iota(ALL(indexs), 0);
	}
	void Fill() {
		fill();
	}

	void clear() {
		elemens.clear();
	}
	void Clear() {
		clear();
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
	void Add(Key i) {
		add(i);
	}

	void fadd(Key i) {
		if (contains(i)) {
			return;
		}
		indexs[i] = elemens.size();
		elemens.push(i);
	}
	void ForceAdd(Key i) {
		fadd(i);
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
	void Remove(Key i) {
		remove(i);
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
	void ForceRemove(Key i) {
		fremove(i);
	}

	bool contains(Key i) const {
		return indexs[i] < (Key)elemens.size() && elemens[indexs[i]] == i;
	}
	bool IsContain(Key i) const {
		return contains(i);
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
};





template <class T, int CAP>
struct CapMap {
private:
	using Key = conditional<CAP <= 256, u8, u32>::type;
	CapArr<pr<Key, T>, CAP> elemens;		
	array<Key, CAP> indexs;		


public:


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

	T& ref(Key i) {
		return elemens[indexs[i]].b;
	}

	void AddValue(Key i, const T& addValue, const T& defaultValue) {
		if (!contains(i)) {
			indexs[i] = elemens.size();
			auto& e = elemens.push();
			e.a = i;
			e.b = defaultValue;
		}
		auto& e = elemens[indexs[i]];
		e.b += addValue;
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

#include <cstdint>


struct Xor64 {
	using result_type = uint64_t;
	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return UINT64_MAX; }

private:
	Xor64(const Xor64& r) = delete;
	Xor64& operator =(const Xor64& r) = delete;
public:

	uint64_t x;
	inline Xor64(uint64_t seed = 0) {
		x = 88172645463325252ULL + seed;
	}

	inline uint64_t operator()() {
		x = x ^ (x << 7);
		return x = x ^ (x >> 9);
	}

	template <class T>
	inline T operator()(T l, T r) {
		return ((*this)() % (r - l)) + l;
	}

	template <class T>
	inline T operator()(T r) {
		return (*this)() % r;
	}
	inline double GetDouble() {
		return (*this)() / (double)UINT64_MAX;
	}
	inline bool GetProb(double E) {
		return GetDouble() <= E;
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

	inline Dir CalcDir1(int from, int to) const {
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
		else {
			VABORT();
		}
		return Dir::Invalid;
	}
	inline Dir CalcDirM(int from, int to) const {
		return CalcDir(ToPos(from), ToPos(to));
	}
};
GridSystemD gs;


constexpr int N_min = 20;
constexpr int N_max = 40;
constexpr int NN_min = N_min * N_min;
constexpr int NN_max = N_max * N_max;
constexpr int T_max = 100000;

#define DEF_CONTAINERS(name, size) \
	template <class TYPE> using name##Arr = CapArr<TYPE, size>;\
	template <class TYPE> using name##Que = CapQue<TYPE, size>;\
	using name##Set = CapSet<size>;\
	template <class TYPE> using name##Map = CapMap<TYPE, size>;

DEF_CONTAINERS(N, N_max)
DEF_CONTAINERS(NN, NN_max)
DEF_CONTAINERS(T, T_max)
DEF_CONTAINERS(T1, T_max + 1)


int N;
int NN;
NArr<NArr<bool>> WallH;		
NArr<NArr<bool>> WallV;		
NNArr<int> Dirties;			
int DirtySum;				

double ScoreNormalize = 1;		

bool CanMove(int x, int y, Dir dir) {
	if (dir == Dir::R) {
		return !WallV[y][x];
	}
	else if (dir == Dir::D) {
		return !WallH[y][x];
	}
	else if (dir == Dir::L) {
		return !WallV[y][x - 1];
	}
	else {
		VASSERT(dir == Dir::U);
		return !WallH[y - 1][x];
	}
}


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

struct AroundMap {
    using CA = CCA<int, 4>;
    NNArr<CA> table_;

    void Init() {
        table_.resize(NN);

        REP(i, NN) {
            cauto& p = gs.ToPos(i);
            REP(di, 4) {
                const pint& a = Around4[di];
                pint n = p + a;
                if (n.a >= 0 && n.a < N &&
                    n.b >= 0 && n.b < N) {
                    if (CanMove(p.x, p.y, Dir(di))) {
                        table_[i].push(n.a + n.b * N);
                    }
                }
            }
        }
    }
    inline const CA& operator[](int i) const {
        return table_[i];
    }
};

struct DirMap {
    using CA = CCA<int, 4>;
    NNArr<CA> table_;

    void Init() {
        table_.resize(NN);

        REP(i, NN) {
            cauto& p = gs.ToPos(i);
            REP(di, 4) {
                const pint& a = Around4[di];
                pint n = p + a;
                int aroundId = -1;
                if (n.a >= 0 && n.a < N &&
                    n.b >= 0 && n.b < N) {
                    if (CanMove(p.x, p.y, Dir(di))) {
                        aroundId = gs.ToId(n);
                    }
                }
                table_[i].push(aroundId);
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

AroundMap aroundMap;
DirMap dirMap;


struct AroundDist2Map {
    using CA = CCA<int, 8>;
    NNArr<CA> table_;

    void Init() {
        table_.resize(NN);

        REP(i, NN) {
            NNQue<int> que;
            NNArr<int> dists;
            dists.assign(NN, -1);
            que.push(i);
            dists[i] = 0;

            while (que.exist()) {
                int p = que.pop();
                for (int n : aroundMap[p]) {
                    if (dists[n] >= 0) {
                        continue;
                    }
                    dists[n] = dists[p] + 1;
                    if (dists[n] == 2) {
                        table_[i].push(n);
                    }
                    else {
                        que.push(n);
                    }
                }
            }
        }
    }
    inline const CA& operator[](int i) const {
        return table_[i];
    }
};


struct AroundDistMap {
    using CA = CapArr<pint, 60>;     
    NNArr<CA> table_;

    void Init(int maxDist) {
        table_.resize(NN);

        REP(i, NN) {
            NNQue<int> que;
            NNArr<int> dists;
            dists.assign(NN, -1);
            que.push(i);
            dists[i] = 0;

            while (que.exist()) {
                int p = que.pop();
                for (int n : aroundMap[p]) {
                    if (dists[n] >= 0) {
                        continue;
                    }
                    dists[n] = dists[p] + 1;
                    if (dists[n] <= maxDist) {
                        auto& t = table_[i].push();
                        t.a = n;
                        t.b = dists[n];
                    }
                    if (dists[n] < maxDist) {
                        que.push(n);
                    }
                }
            }
        }
    }
    inline const CA& operator[](int i) const {
        return table_[i];
    }
};
AroundDist2Map aroundDist2Map;


struct DistMap {
	vector<NNArr<int>> dists_;
	vector<NNArr<int>> froms_;

	void Init() {
		dists_.resize(NN);
		froms_.resize(NN);
		REP(a, NN) {
			auto& dists = dists_[a];
			auto& froms = froms_[a];
			dists.assign(NN, -1);
			froms.assign(NN, -1);

			NNQue<int> que;
			que.push(a);
			dists[a] = 0;
			froms[a] = -1;
			while (que.exist()) {
				int p = que.pop();
				int nd = dists[p] + 1;
				for (int n : aroundMap[p]) {
					if (dists[n] >= 0) {
						if (dists[n] == nd) {
							if (Dirties[p] > Dirties[froms[n]]) {
								froms[n] = p;
							}
						}
						continue;
					}
					dists[n] = nd;
					froms[n] = p;
					que.push(n);
				}
			}
		}
	}

	int GetDist(int a, int b) const {
		return dists_[a][b];
	}

};
DistMap distMap;


constexpr int MaxRouteLength = 4;
constexpr int OneCellRouteCount = 7;					
constexpr int RelArrWidth = MaxRouteLength * 2 + 1;		
constexpr int RelArrSize = RelArrWidth * RelArrWidth;	

using Route = CapArr<int, MaxRouteLength>;

template <class T>
using RelArr = CapArr<T, RelArrSize>;		

struct RouteTbl {

	vector<Route> routeList_;				
	vector<RelArr<CapArr<int, OneCellRouteCount>>> routeTbl_;			
	vector<RelArr<CapArr<int, MaxRouteLength + 2>>> routeStarts_;		

	void Init() {
		routeList_.reserve(NN_max * RelArrSize * OneCellRouteCount);
		routeTbl_.resize(NN);
		routeStarts_.resize(NN);
		REP(p, NN) {
			routeTbl_[p].resize(RelArrSize);
			routeStarts_[p].resize(RelArrSize);
		}

		Route route;
		NNArr<bool> checked(NN, false);

		REP(startP, NN) {
			auto dfs = [&](auto&& dfs, int p) -> void {
				checked[p] = true;

				int routeIdx = (int)routeList_.size();
				routeList_.emplace_back();
				routeList_.back().MemCopy(route);
				int len = route.size() + 1;

				for (int n : aroundMap[p]) {
					if (checked[n]) {
						continue;
					}

					int rp = ToRelPos(startP, n);
					VASSERT(rp >= 0);
					routeTbl_[startP][rp].push(routeIdx);

					if (len < MaxRouteLength) {
						route.push(n);
						dfs(dfs, n);
						route.pop();
					}
				}

				checked[p] = false;
			};
			dfs(dfs, startP);
		}

		int maxLen = 0;
		REP(from, NN) {
			REP(rp, RelArrSize) {
				auto& routes = routeTbl_[from][rp];
				auto& starts = routeStarts_[from][rp];
				starts.assign(MaxRouteLength + 2, 0);

				if (routes.empty()) {
					continue;
				}


				chmax(maxLen, routes.size());
				sort(ALL(routes), [&](int a, int b) {
					return routeList_[a].size() < routeList_[b].size();
					});

				REP(i, routes.size()) {
					cauto& route = routeList_[routes[i]];
					int length = route.size() + 1;
					VASSERT(length <= MaxRouteLength);
					starts[length + 1] = i + 1;
				}
				FOR(length, 1, starts.size()) {
					chmax(starts[length], starts[length - 1]);
				}
			}
		}

	}

	int ToRelPos(int base, int to) const {
		cauto& b = gs.ToPos(base);
		cauto& t = gs.ToPos(to);
		int rx = t.x - b.x;
		int ry = t.y - b.y;
		if (rx < -MaxRouteLength || rx > MaxRouteLength ||
			ry < -MaxRouteLength || ry > MaxRouteLength) {
			return -1;
		}
		rx += MaxRouteLength;
		ry += MaxRouteLength;
		int rp = ry * RelArrWidth + rx;
		return rp;
	}


	int GetRandom(int from, int to, int minLength, int maxLength, Xor64& rand_) const {
		VASSERT(minLength <= MaxRouteLength);
		VASSERT(maxLength <= MaxRouteLength);

		int rp = ToRelPos(from, to);
		if (rp < 0) {
			return -1;
		}
		cauto& routes = routeTbl_[from][rp];
		cauto& starts = routeStarts_[from][rp];
		int start = starts[minLength];
		int end = starts[maxLength + 1];
		int count = end - start;
		if (count == 0) {
			return -1;
		}
		int idx = start + rand_(count);
		return routes[idx];
	}

	bool GetRandom2(int from, int to, int minLength, int maxLength, Xor64& rand_, CapArr<int, 2>& ris) const {
		ris.clear();
		VASSERT(minLength <= MaxRouteLength);
		VASSERT(maxLength <= MaxRouteLength);

		int rp = ToRelPos(from, to);
		if (rp < 0) {
			return false;
		}
		cauto& routes = routeTbl_[from][rp];
		cauto& starts = routeStarts_[from][rp];
		int start = starts[minLength];
		int end = starts[maxLength + 1];
		int count = end - start;
		if (count == 0) {
			return false;
		}
		else if (count == 1) {
			ris.push(routes[start]);
		}
		else {
			int i0 = rand_(count);
			int i1 = rand_(count - 1);
			if (i1 >= i0) {
				++i1;
			}
			ris.push(routes[start + i0]);
			ris.push(routes[start + i1]);
		}
		return true;
	}

	bool GetAll(int from, int to, int minLength, int maxLength, CapArr<int, 100>& ris) const {
		ris.clear();
		VASSERT(minLength <= MaxRouteLength);
		VASSERT(maxLength <= MaxRouteLength);

		int rp = ToRelPos(from, to);
		if (rp < 0) {
			return false;
		}
		cauto& routes = routeTbl_[from][rp];
		cauto& starts = routeStarts_[from][rp];
		int start = starts[minLength];
		int end = starts[maxLength + 1];
		int count = end - start;
		if (count == 0) {
			return false;
		}
		REP(i, count) {
			ris.push(routes[start + i]);
		}
		return true;
	}
	const Route& GetRoute(int idx) const {
		return routeList_[idx];
	}
};
constexpr int RouteTblSize = sizeof(RouteTbl);
constexpr int RouteTblSizeM = sizeof(RouteTbl) / 1024 / 1024;
RouteTbl routeTbl_;


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



InputParam<int> ipNN(NN_min, NN_max);
InputParam<int> ipDirtySum(5000, 100000);	

constexpr
struct {


START_TUNING;





PARAM_GROUP(optuna);
PARAM_DOUBLE(EmergencyTime, 0.6376950416239445, 0.6, 0.7);PARAM_UPPER(0.9);
PARAM_DOUBLE(EmergencyTime_NN, -0.0641010133140648, -0.5, 0.5);
PARAM_DOUBLE(EmergencyTime_DirtySum, 0.05066617206256563, -0.5, 0.5);
PARAM_INT(BeamWidth, 146, 100, 150);PARAM_LOWER(1);
PARAM_DOUBLE(BeamWidth_NN, -0.0887848776398901, -0.1, 0.0);
PARAM_DOUBLE(BeamWidth_DirtySum, 0.5665545059000293, 0.5, 0.6);
PARAM_DOUBLE(NotReached, 427.3032575195445, 400.0, 500.0);
PARAM_DOUBLE(NotReached_NN, 1.0372037423924276, 0.8, 1.2);
PARAM_DOUBLE(NotReached_DirtySum, 0.5377211821286004, 0.4, 0.8);
PARAM_DOUBLE(STotal, 1.1534374192261285, 1.0, 1.4);
PARAM_DOUBLE(BTotal, 0.9617849780856226, 0.9, 1.1);
PARAM_DOUBLE(ReachedTotalDirty, 1.1083988573281942, 0.9, 1.1);
PARAM_DOUBLE(NotReachedTotalDirty, 17.30722000279734, 17.0, 19.0);
PARAM_DOUBLE(BSum, 0.988491662283239, 1.0, 1.1);
PARAM_DOUBLE(ReachedDirtySum, 0.999660879758338, 1.2, 1.32);
PARAM_DOUBLE(AppendTotalDirty, 1.9982656476728469, 1.9, 2.4);
PARAM_DOUBLE(NrDirtySum, -1.0634075644575667, -1.3, -1.0);
PARAM_DOUBLE(NrAppendTotalDirty, 1.0718606905850785, 0.9, 1.3);
PARAM_DOUBLE(AroundA, 11.299346523987696, 8.0, 12.0);
PARAM_DOUBLE(AroundA_NN, -0.06247372868809758, 0.0, 0.1);
PARAM_DOUBLE(AroundA_DirtySum, -0.30923761282519974, -0.5, -0.2);
PARAM_DOUBLE(Around2A, 2.268938737270897, 4.0, 8.0);
PARAM_DOUBLE(Around2A_NN, -0.010688767243212004, -0.2, 0.0);
PARAM_DOUBLE(Around2A_DirtySum, -1.2484814723609101, -1.5, -1.0);
PARAM_DOUBLE(AroundS, 0.8465939917433404, 0.8, 0.95);
PARAM_DOUBLE(AroundS_NN, 0.2210256162049022, 0.1, 0.2);
PARAM_DOUBLE(AroundS_DirtySum, -0.05388652238588701, -0.1, 0.0);
PARAM_DOUBLE(Around2S, 0.8661680274379177, 0.6, 0.8);
PARAM_DOUBLE(Around2S_NN, -0.02766204124687891, -0.1, 0.1);
PARAM_DOUBLE(Around2S_DirtySum, -0.8406330163938944, -1.0, -0.8);
PARAM_DOUBLE(BeamTimeWeight, 0.023189655065058386, 0.0, 0.1);PARAM_LOWER(0.0);
PARAM_DOUBLE(BeamTimeWeight_NN, -0.017167446674773535, -0.2, 0.0);
PARAM_DOUBLE(BeamTimeWeight_DirtySum, -0.4192163669832423, -0.6, -0.4);
PARAM_DOUBLE(SATimeWeight, 3.8287140383008644, 3.5, 3.9);PARAM_LOWER(0.0);
PARAM_DOUBLE(StartTemp, 56.264388325053574, 50.0, 60.0);PARAM_LOWER(0.0);
PARAM_DOUBLE(StartTemp_NN, -0.46734814481480774, -0.6, -0.2);
PARAM_DOUBLE(EndTemp_NN, -0.8613060795554134, -1.1, -0.8);
PARAM_DOUBLE(EndTemp, 1.6896001590102687, 1.0, 3.0);PARAM_LOWER(0.0);
PARAM_DOUBLE(StartTemp_DirtySum, -0.21704524801203512, -0.5, -0.1);
PARAM_DOUBLE(EndTemp_DirtySum, 0.19588510311125829, 0.1, 0.3);
PARAM_INT(Trans1, 550, 480, 580);PARAM_LOWER(0);PARAM_UPPER(1000);
PARAM_INT(Trans2, 351, 240, 340);PARAM_LOWER(0);PARAM_UPPER(1000);
PARAM_INT(Trans3, 39, 0, 70);PARAM_LOWER(0);PARAM_UPPER(1000);
PARAM_INT(Trans4, 258, 160, 260);PARAM_LOWER(0);PARAM_UPPER(1000);
PARAM_INT(Trans5, 209, 170, 270);PARAM_LOWER(0);PARAM_UPPER(1000);
PARAM_DOUBLE(STotal_NN, -0.11782832266443241, -0.5, 0.5);
PARAM_DOUBLE(STotal_DirtySum, 0.10328416721330251, -0.5, 0.5);
PARAM_DOUBLE(BTotal_NN, -0.06920648139879215, -0.5, 0.5);
PARAM_DOUBLE(BTotal_DirtySum, -0.43107241437820776, -0.5, 0.5);
PARAM_DOUBLE(ReachedTotalDirty_NN, -0.019391880862605695, -0.5, 0.5);
PARAM_DOUBLE(ReachedTotalDirty_DirtySum, 0.08132199301427355, -0.5, 0.5);
PARAM_DOUBLE(NotReachedTotalDirty_NN, -0.039255444791598634, -0.5, 0.5);
PARAM_DOUBLE(NotReachedTotalDirty_DirtySum, 0.5813188026076788, -0.5, 0.5);
PARAM_DOUBLE(BSum_NN, 0.04078814004146618, -0.5, 0.5);
PARAM_DOUBLE(BSum_DirtySum, 0.01144381356656634, -0.5, 0.5);
PARAM_DOUBLE(ReachedDirtySum_NN, -0.05649043385241236, -0.5, 0.5);
PARAM_DOUBLE(ReachedDirtySum_DirtySum, 0.3608789131487613, -0.5, 0.5);
PARAM_DOUBLE(AppendTotalDirty_NN, -0.06265539012827151, -0.5, 0.5);
PARAM_DOUBLE(AppendTotalDirty_DirtySum, 0.15477620038953702, -0.5, 0.5);
PARAM_DOUBLE(NrDirtySum_NN, -0.016802539655144742, -0.5, 0.5);
PARAM_DOUBLE(NrDirtySum_DirtySum, -0.21251237440175652, -0.5, 0.5);
PARAM_DOUBLE(NrAppendTotalDirty_NN, -0.12793821418397675, -0.5, 0.5);
PARAM_DOUBLE(NrAppendTotalDirty_DirtySum, 0.2651459785800466, -0.5, 0.5);
PARAM_GROUP_END;
	END_TUNING;




	PARAM_INT(MinRollbackCount, 10, 10, 900);PARAM_LOWER(1);

	PARAM_DOUBLE(RollbackStartRate, 0.054038528974589856, 0.0, 0.3);PARAM_LOWER(0.0);
	PARAM_INT(RollbackCount, 1289455, 240000, 327481);PARAM_LOWER(1);
	PARAM_DOUBLE(RollbackNextMulti, 1.3472323418707621, 1.1, 1.3);PARAM_LOWER(0.1);

	PARAM_INT(Trans1_RandSize, 1, 1, 2);PARAM_LOWER(1);
	PARAM_INT(Trans2_LowerSize, 1, 1, 2);PARAM_LOWER(1);
	PARAM_INT(Trans3_LowerSize, 2, 1, 2);PARAM_LOWER(1);
	PARAM_INT(Trans3_RandSize, 1, 1, 2);PARAM_LOWER(1);
	PARAM_INT(Trans4_RandSize, 1, 1, 2);PARAM_LOWER(1);
	PARAM_INT(Trans2_AppendableTurn, 2, 2, 4);PARAM_LOWER(2);    
	PARAM_INT(Trans1_LowerSize, 3, 2, 4);PARAM_LOWER(2);PARAM_UPPER(8);    
	PARAM_INT(Trans2_RandSize, 2, 2, 4);PARAM_LOWER(1);PARAM_UPPER(4);

	PARAM_INT(Trans4_LowerSize, 6, 6, 8);PARAM_LOWER(3);PARAM_UPPER(8);    

	PARAM_INT(MaxSkipLength, 18, 16, 26);PARAM_LOWER(1);PARAM_UPPER(180);    

	PARAM_CATEGORY(TempType, 0, 0, 2);
	PARAM_DOUBLE(TempPow, 1.4519182725814792, 2.0, 3.0);PARAM_LOWER(0.1);






} HP;




double CalcScoreVerify(const T1Arr<int>& ans) {
	NNArr<int> a(NN, 0);
	FOR(i, 1, ans.size()) {
		int p = ans[i];
		REP(n, NN) {
			if (p == n) {
				a[n] = 0;
			}
			else {
				a[n] += Dirties[n];
			}
		}
	}
	s64 s = 0;
	FOR(i, 1, ans.size()) {
		int p = ans[i];
		int st = 0;
		REP(n, NN) {
			if (p == n) {
				a[n] = 0;
			}
			else {
				a[n] += Dirties[n];
				st += a[n];
			}
		}
		s += st;
	}
	double mean = s / (double)(ans.size() - 1);
	return mean;
}

double CalcScore(const T1Arr<int>& ans) {
	int L = ans.size() - 1;

	NNArr<int> lastTurn(NN, -1);
	int count = 0;
	FOR(i, 1, ans.size()) {
		int t = i - 1;
		int p = ans[i];
		if (lastTurn[p] < 0) {
			++count;
		}
		lastTurn[p] = t;
	}
	if (count != NN) {
		return -1;
	}

	s64 s = 0;
	FOR(i, 1, ans.size()) {
		int t = L + i - 1;
		int p = ans[i];
		s64 et = t - lastTurn[p];
		s += et * (et - 1) / 2 * Dirties[p];
		lastTurn[p] = t;
	}

	double mean = s / (double)L;
	return mean;
}

double CalcScoreJudge(const T1Arr<int>& ans) {
	int L = ans.size() - 1;

	NNArr<int> lastTurn(NN, -1);
	FOR(i, 1, ans.size()) {
		int t = i - 1;
		int p = ans[i];
		lastTurn[p] = t;
	}

	s64 s = 0;		
	s64 sum_d = 0;
	REP(i, NN) {
		s += (L - lastTurn[i]) * Dirties[i];
		sum_d += Dirties[i];
	}

	s64 S = 0;
	FOR(i, 1, ans.size()) {
		int t = ans.size() - 1 + i - 1;
		int p = ans[i];
		int dt = t - lastTurn[p];
		int a = dt * Dirties[p];
		s -= a;
		S += s;
		s += sum_d;
		lastTurn[p] = t;
	}

	double mean = S / (double)L;

	return mean;
}

void PointsToDirs(T1Arr<int>& ans, string& output) {
	output.resize(ans.size() - 1);
	REP(i, ans.size() - 1) {
		output[i] = DirToChar(gs.CalcDir1(ans[i], ans[i + 1]));
	}
}


double CalcScoreTest(const T1Arr<int>& ans) {
	int L = ans.size() - 1;

	NNArr<int> lastTurn(NN, -1);
	int count = 0;
	FOR(i, 1, ans.size()) {
		int t = i - 1;
		int p = ans[i];
		if (p < 0) {
			continue;
		}
		if (lastTurn[p] < 0) {
			++count;
		}
		lastTurn[p] = t;
	}

	s64 s = 0;
	FOR(i, 1, ans.size()) {
		int t = L + i - 1;
		int p = ans[i];
		if (p < 0) {
			continue;
		}
		s64 et = t - lastTurn[p];
		s += et * (et - 1) / 2 * Dirties[p];
		lastTurn[p] = t;
	}

	double mean = s / (double)L;
	return mean;
}

void ConvertConnected(const T1Arr<int>& src, T1Arr<int>& ans) {
	ans.clear();
	ans.push(src[0]);
	FOR(i, 1, src.size()) {
		if (ans.back() == src[i]) {
			continue;
		}
		int n = src[i];
		cauto& froms = distMap.froms_[n];
		int c = ans.back();
		while (true) {
			c = froms[c];
			ans.push(c);
			if (c == n) {
				break;
			}
		}
	}
}

void Rotate(const T1Arr<int>& src, int newStart, T1Arr<int>& dst) {
	int length = (src.size() - 1);
	dst.resize(src.size());
	REP(i, length) {
		dst[1 + i] = src[1 + (newStart + i) % length];
	}
	dst[0] = src[newStart];
}



struct IOServer {

	void InitInput(ChronoTimer& timer) {
		istream& is = cin;
		is >> N;
		NN = N * N;

		string s;
		WallH.resize(N - 1);
		REP(y, N - 1) {
			is >> s;
			WallH[y].resize(N);
			REP(x, N) {
				WallH[y][x] = (s[x] != '0');
			}
		}
		WallV.resize(N);
		REP(y, N) {
			is >> s;
			WallV[y].resize(N - 1);
			REP(x, N - 1) {
				WallV[y][x] = (s[x] != '0');
			}
		}
		Dirties.resize(NN);
		DirtySum = 0;
		REP(i, NN) {
			is >> Dirties[i];
			DirtySum += Dirties[i];
		}

		gs.Init(N, N);
		aroundMap.Init();
		dirMap.Init();
		distMap.Init();
		aroundDist2Map.Init();
		routeTbl_.Init();

		ScoreNormalize = 1000000.0 / (NN * DirtySum);

		ipNN.SetValue(NN);
		ipDirtySum.SetValue(DirtySum);
	}

	void Input() {
		istream& is = cin;
	}

	void Output(const string& ans) {
		ostream& os = cout;
		os << ans << endl;
	}

	void Finalize() {
	}
};
IOServer server;


Xor64 rand_;


s64 CalcBlock(int prevTurn, int nextTurn, int L) {
	s64 et = nextTurn - prevTurn;
	if (et <= 0) {
		et += L;
	}
	return ((et * (et - 1)) >> 1);
}
s64 CalcBlockAppend(int prevTurn, int nextTurn, int L, int appendTurn) {
	s64 et = nextTurn - prevTurn;
	if (et <= 0) {
		et += L;
	}
	et += appendTurn;
	return ((et * (et - 1)) >> 1);
}

struct Scorer {
	int turn_;					
	s64 reachedDirtySum_;		
	s64 appendTotalDirty_;		
	s64 totalDirty_;			

	void Init() {
		turn_ = 0;
		reachedDirtySum_ = 0;
		appendTotalDirty_ = 0;
		totalDirty_ = 0;
	}

	void InitWithAns(const T1Arr<int>& ans) {
		turn_ = 0;
		reachedDirtySum_ = 0;
		appendTotalDirty_ = 0;
		totalDirty_ = 0;

		NNArr<int> firstTurns(NN, -1);
		NNArr<int> prevTurns(NN, -1);
		FOR(t, 1, ans.size()) {
			AddTurn();
			int p = ans[t];
			if (firstTurns[p] < 0) {
				SetFirst(p, t);
				firstTurns[p] = t;
			}
			else {
				Set(p, prevTurns[p], t, firstTurns[p]);
			}
			prevTurns[p] = t;
		}
	}


	void AddTurn() {
		++turn_;
		appendTotalDirty_ += reachedDirtySum_;
		totalDirty_ += appendTotalDirty_;
	}

	void SetFirst(int p, int turn) {
		VASSERT(turn <= turn_);
		reachedDirtySum_ += Dirties[p];
		appendTotalDirty_ += (turn_ - 1) * Dirties[p];
		totalDirty_ += CalcBlock(turn, turn, turn_) * Dirties[p];
	}

	void Set(int p, int prevTurn, int turn, int nextTurn) {
		s64 block = 0;






		if (prevTurn < nextTurn) {
			s64 T = turn;
			s64 N = nextTurn;
			s64 P = prevTurn;
			block = (T - N) * (T - P);
		}
		else {
			if (turn >= nextTurn) {
				s64 T = turn;
				s64 N = nextTurn + turn_;
				s64 P = prevTurn;
				block = (T - N) * (T - P);

				int oldRange = nextTurn + turn_ - prevTurn;
				int newRange = nextTurn + turn_ - turn;
				int diffTurn = oldRange - newRange;
				appendTotalDirty_ -= diffTurn * Dirties[p];
			}
			else {
				s64 T = turn;
				s64 N = nextTurn;
				s64 P = prevTurn;
				s64 L = turn_;
				block = (T - N) * (T + L - P);

				int oldRange = nextTurn + turn_ - prevTurn;
				int newRange = turn + turn_ - prevTurn;
				int diffTurn = oldRange - newRange;
				appendTotalDirty_ -= diffTurn * Dirties[p];
			}
		}
		totalDirty_ += block * Dirties[p];
	}

	void RemoveLast(int p, int turn) {
		VASSERT(turn <= turn_);
		reachedDirtySum_ -= Dirties[p];
		appendTotalDirty_ -= (turn_ - 1) * Dirties[p];
		totalDirty_ -= CalcBlock(turn, turn, turn_) * Dirties[p];
	}

	void Remove(int p, int prevTurn, int turn, int nextTurn) {
		s64 block = 0;

		if (prevTurn < nextTurn) {
			s64 T = turn;
			s64 N = nextTurn;
			s64 P = prevTurn;
			block = -(T - N) * (T - P);
		}
		else {
			if (turn >= nextTurn) {
				s64 T = turn;
				s64 N = nextTurn + turn_;
				s64 P = prevTurn;
				block = -(T - N) * (T - P);

				int oldRange = nextTurn + turn_ - prevTurn;
				int newRange = nextTurn + turn_ - turn;
				int diffTurn = oldRange - newRange;
				appendTotalDirty_ += diffTurn * Dirties[p];
			}
			else {
				s64 T = turn;
				s64 N = nextTurn;
				s64 P = prevTurn;
				s64 L = turn_;
				block = -(T - N) * (T + L - P);

				int oldRange = nextTurn + turn_ - prevTurn;
				int newRange = turn + turn_ - prevTurn;
				int diffTurn = oldRange - newRange;
				appendTotalDirty_ += diffTurn * Dirties[p];
			}
		}


		totalDirty_ += block * Dirties[p];
	}

	void Insert(int p, int prevTurn, int nextTurn, int appendTurn) {
		s64 block = 0;
		block -= CalcBlock(prevTurn, nextTurn, turn_);
		block += CalcBlockAppend(prevTurn, nextTurn, turn_, appendTurn);

		if (prevTurn >= nextTurn) {
		}
		totalDirty_ += block * Dirties[p];
	}

	void FinishInsert(int appendTurn) {
		turn_ += appendTurn;
	}

	double GetScore() const {
		return totalDirty_ / (double)turn_;
	}

};


NNArr<int> areaPathMap_;		

#define USE_HASH 0


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
			free(pool_);
		}
		if (reusable_) {
			free(reusable_);
		}
	}


	void Init(int capacity) {
		if (capacity != capacity_) {
			if (pool_) {
				free(pool_);
			}
			if (reusable_) {
				free(reusable_);
			}
			pool_ = (OBJECT*)malloc(capacity * sizeof(OBJECT));
			reusable_ = (OBJECT**)malloc(capacity * sizeof(OBJECT*));
			capacity_ = capacity;
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
		ss << maxTotalCount_ << " / " << capacity_ << " (" << (int)floor(maxTotalCount_ * 100 / (double)capacity_ + 0.5) << " %)";
		return ss.str();
	}
	double GetMemoryRate() const {
		return usedTotalCount_ / (double)capacity_;
	}
};

#define USE_HISTOGRAM 0			
#define USE_HISTOGRAM_RANGE 0	


#define MULTI_BEAM_LOG 0
#define SINGLE_BEAM_LOG 0


template <class NODE, class STATE, class BACKUP>
struct DiffBeam {
public:
	struct NodeEx : public NODE {
		NodeEx* parent;			
		NodeEx* child;			
		NodeEx* prev;			
		NodeEx* next;			
		double evalScore;
	};

private:
	using NodePool = ObjectPool<NodeEx>;

	NodePool nodePool_;
	vector<NodeEx*> targets_;
	vector<NodeEx*> dels_;

public:

public:
	NODE* New() {
		NodeEx* node = nodePool_.New();
		return node;
	}
	void Delete(NODE* node) {
		nodePool_.Delete((NodeEx*)node);
	}

	void GetBest(const NODE* bestNode, vector<const NODE*>& nodes) {
		nodes.clear();
		const NodeEx* node = (NodeEx*)bestNode;
		while (node->parent) {
			nodes.push_back(node);
			node = node->parent;
		}
		reverse(ALL(nodes));
	}

	void ReleaseNode(NodeEx* node) {
		VASSERT(node->child == nullptr);

		while (true) {
			NodeEx* next = nullptr;
			if (node->parent) {
				if (node->parent->child == node) {
					VASSERT(node->prev == nullptr);
					if (node->next) {
						node->parent->child = node->next;
						node->next->prev = nullptr;
					}
					else {
						node->parent->child = nullptr;
						next = node->parent;
					}
				}
				else {
					NodeEx* prev = node->prev;
					NodeEx* next = node->next;
					VASSERT(prev);
					prev->next = next;
					if (next) {
						next->prev = prev;
					}
				}
			}
			nodePool_.Delete(node);

			if (next) {
				node = next;
			}
			else {
				break;
			}
		}
	}

	void Initialize(int widthLimit, int maxExpandCount, int turnMax) {
		targets_.reserve(widthLimit * maxExpandCount);
		dels_.reserve(widthLimit * maxExpandCount);
		nodePool_.Init(widthLimit * turnMax * 2);
	}

	template <class INIT_ROOT, class MAKE_NEXT_NODES, class UPDATE_STATE, class REVERT_STATE, class GET_WIDTH, class IS_END>
	void Run(int turnMax, INIT_ROOT&& initRoot, MAKE_NEXT_NODES&& makeNextNodes, UPDATE_STATE&& updateState, REVERT_STATE&& revertState, GET_WIDTH&& getWidth, IS_END&& isEnd, int realTurn) {
		nodePool_.Clear();
		targets_.clear();
		dels_.clear();

		STATE state;

		NodeEx* rootNode = nodePool_.New();
		{
			rootNode->parent = nullptr;
			rootNode->child = nullptr;
			rootNode->prev = nullptr;
			rootNode->next = nullptr;
			rootNode->evalScore = 0;
		}

		initRoot(state, rootNode);

		int startDepth = 0;
		NodeEx* startNode = rootNode;

		REP(turn, turnMax) {
			auto dfs = [&](auto& dfs, int depth, NodeEx* node) -> void {
				if (depth >= turn) {
					static CapArr<pr<NODE*, double>, 4> nexts;
					nexts.clear();
					makeNextNodes(depth, node, state, nexts);


					NodeEx* prev = nullptr;
					for (auto&& [nextTmp, evalScore] : nexts) {
						NodeEx* next = (NodeEx*)nextTmp;
						if (node->child == nullptr) {
							node->child = next;
							next->parent = node;
							next->child = nullptr;
							next->prev = nullptr;
							next->next = nullptr;
						}
						else {
							prev->next = next;
							next->parent = node;
							next->child = nullptr;
							next->prev = prev;
							next->next = nullptr;
						}
						next->evalScore = evalScore;
						prev = next;
					}

					if (node->child) {
						NodeEx* child = node->child;
						while (child) {
							targets_.push_back(child);
							child = child->next;
						}
					}
					else {
						dels_.push_back(node);
					}
				}
				else {
					BACKUP backup;
					NodeEx* child = node->child;
					while (child) {
						updateState(state, child, backup);
						dfs(dfs, depth + 1, child);
						revertState(state, child, backup);

						child = child->next;
					}
				}
			};

			dfs(dfs, startDepth, startNode);


			if (isEnd()) {
				break;
			}

			int TargetWidth = getWidth(turn);
			int width = 0;

				NNMap<NodeEx*> tbl;
				for (NodeEx* target : targets_) {
					int key = target->to;
					if (tbl.contains(key)) {
						NodeEx* old = tbl.get(key);
						if (target->evalScore > old->evalScore) {
							ReleaseNode(old);
							tbl.set(key, target);
						}
						else {
							ReleaseNode(target);
						}
					}
					else {
						tbl.set(key, target);
						++width;
					}
				}

				if (tbl.size() > TargetWidth) {
					targets_.clear();
					for (auto&& [k, v] : tbl) {
						targets_.push_back(v);
					}

					nth_element(targets_.begin(), targets_.begin() + TargetWidth, targets_.end(), [&](NodeEx* a, NodeEx* b) {
						return a->evalScore > b->evalScore;
						});
					FOR(i, TargetWidth, targets_.size()) {
						if (areaPathMap_[targets_[i]->to] == 2) {
							continue;
						}
						ReleaseNode(targets_[i]);
						--width;
					}
				}

			for (NodeEx* d : dels_) {
				ReleaseNode(d);
			}
			dels_.clear();
			targets_.clear();

			BACKUP backup;
			while (startNode->child && startNode->child->next == nullptr) {
				updateState(state, startNode->child, backup);
				startNode = startNode->child;
				++startDepth;
			}
		}


	}
};

struct ScorerBeam {
	int turn_;					
	int reachedDirtySum_;		
	int nrDirtySum_;			
	s64 appendTotalDirty_;		
	s64 totalDirty_;			

	s64 nrAppendTotalDirty_;	
	s64 nrTotalDirty_;			

	void Init() {
		turn_ = 0;
		reachedDirtySum_ = 0;
		appendTotalDirty_ = 0;
		totalDirty_ = 0;

		nrDirtySum_ = DirtySum;
		nrAppendTotalDirty_ = 0;
		nrTotalDirty_ = 0;
	}

	void InitWithAns(const T1Arr<int>& ans) {
		turn_ = 0;
		reachedDirtySum_ = 0;
		appendTotalDirty_ = 0;
		totalDirty_ = 0;

		NNArr<int> firstTurns(NN, -1);
		NNArr<int> prevTurns(NN, -1);
		FOR(t, 1, ans.size()) {
			AddTurn();
			int p = ans[t];
			if (firstTurns[p] < 0) {
				SetFirst(p, t);
				firstTurns[p] = t;
			}
			else {
				Set(p, prevTurns[p], t, firstTurns[p]);
			}
			prevTurns[p] = t;
		}
	}


	void AddTurn() {
		++turn_;
		appendTotalDirty_ += reachedDirtySum_;
		totalDirty_ += appendTotalDirty_;

		nrAppendTotalDirty_ += nrDirtySum_;
		nrTotalDirty_ += nrAppendTotalDirty_;
	}

	void SetFirst(int p, int turn) {
		VASSERT(turn <= turn_);
		reachedDirtySum_ += Dirties[p];
		appendTotalDirty_ += (turn_ - 1) * Dirties[p];
		totalDirty_ += CalcBlock(turn, turn, turn_) * Dirties[p];

		nrDirtySum_ -= Dirties[p];
		nrAppendTotalDirty_ -= (turn_) * Dirties[p];
		nrTotalDirty_ -= CalcBlock(turn_, turn_, turn_ + 1) * Dirties[p];
	}

	void Set(int p, int prevTurn, int turn, int nextTurn) {
		s64 block = 0;






		if (prevTurn < nextTurn) {
			s64 T = turn;
			s64 N = nextTurn;
			s64 P = prevTurn;
			block = (T - N) * (T - P);
		}
		else {
			if (turn >= nextTurn) {
				s64 T = turn;
				s64 N = nextTurn + turn_;
				s64 P = prevTurn;
				block = (T - N) * (T - P);

				int oldRange = nextTurn + turn_ - prevTurn;
				int newRange = nextTurn + turn_ - turn;
				int diffTurn = oldRange - newRange;
				appendTotalDirty_ -= diffTurn * Dirties[p];
			}
			else {
				s64 T = turn;
				s64 N = nextTurn;
				s64 P = prevTurn;
				s64 L = turn_;
				block = (T - N) * (T + L - P);

				int oldRange = nextTurn + turn_ - prevTurn;
				int newRange = turn + turn_ - prevTurn;
				int diffTurn = oldRange - newRange;
				appendTotalDirty_ -= diffTurn * Dirties[p];
			}
		}
		totalDirty_ += block * Dirties[p];
	}

	void RemoveLast(int p, int turn) {
		VASSERT(turn <= turn_);
		reachedDirtySum_ -= Dirties[p];
		appendTotalDirty_ -= (turn_ - 1) * Dirties[p];
		totalDirty_ -= CalcBlock(turn, turn, turn_) * Dirties[p];
	}

	void Remove(int p, int prevTurn, int turn, int nextTurn) {
		s64 block = 0;

		if (prevTurn < nextTurn) {
			s64 T = turn;
			s64 N = nextTurn;
			s64 P = prevTurn;
			block = -(T - N) * (T - P);
		}
		else {
			if (turn >= nextTurn) {
				s64 T = turn;
				s64 N = nextTurn + turn_;
				s64 P = prevTurn;
				block = -(T - N) * (T - P);

				int oldRange = nextTurn + turn_ - prevTurn;
				int newRange = nextTurn + turn_ - turn;
				int diffTurn = oldRange - newRange;
				appendTotalDirty_ += diffTurn * Dirties[p];
			}
			else {
				s64 T = turn;
				s64 N = nextTurn;
				s64 P = prevTurn;
				s64 L = turn_;
				block = -(T - N) * (T + L - P);

				int oldRange = nextTurn + turn_ - prevTurn;
				int newRange = turn + turn_ - prevTurn;
				int diffTurn = oldRange - newRange;
				appendTotalDirty_ += diffTurn * Dirties[p];
			}
		}


		totalDirty_ += block * Dirties[p];
	}

	void Insert(int p, int prevTurn, int nextTurn, int appendTurn) {
		s64 block = 0;
		block -= CalcBlock(prevTurn, nextTurn, turn_);
		block += CalcBlockAppend(prevTurn, nextTurn, turn_, appendTurn);

		if (prevTurn >= nextTurn) {
		}
		totalDirty_ += block * Dirties[p];
	}

	void FinishInsert(int appendTurn) {
		turn_ += appendTurn;
	}

	double GetScore() const {
		return totalDirty_ / (double)turn_;
	}

};

struct Block {
	int p_;						
	int notReached_;			

	s64 bSum_;					
	s64 bTotal_;				
	s64 sTotal_;				

	ScorerBeam scorer_;
};

struct Backup {
	int to;

	int firstTurn_;
	int lastTurn_;

	union {
		struct {
			int p_;						
			int notReached_;			

			s64 bSum_;					
			s64 bTotal_;				
			s64 sTotal_;				

			ScorerBeam scorer_;
		};
		Block block;
	};
};

struct State {
	NNArr<int> firstTurns_;		
	NNArr<int> lastTurns_;		

	union {
		struct {
			int p_;						
			int notReached_;			

			s64 bSum_;					
			s64 bTotal_;				
			s64 sTotal_;				

			ScorerBeam scorer_;
		};
		Block block;
	};

	void Init() {
		firstTurns_.assign(NN, -1);
		lastTurns_.assign(NN, 0);

		p_ = 0;
		notReached_ = NN;

		bSum_ = 0;
		bTotal_ = 0;
		sTotal_ = 0;

		scorer_.Init();
	}

	void Move(int to, Backup& backup) {
		backup.to = to;

		backup.firstTurn_ = firstTurns_[to];
		backup.lastTurn_ = lastTurns_[to];
		backup.block = block;

		p_ = to;
		scorer_.AddTurn();

		s64 et = scorer_.turn_ - lastTurns_[to];

		if (firstTurns_[to] < 0) {
			--notReached_;
			firstTurns_[to] = scorer_.turn_;

			scorer_.SetFirst(to, scorer_.turn_);
		}
		else {
			VASSERT(lastTurns_[to] >= 0);
			scorer_.Set(to, lastTurns_[to], scorer_.turn_, firstTurns_[to]);
		}

		lastTurns_[to] = scorer_.turn_;

		bSum_ -= et * Dirties[to];
		bSum_ += DirtySum;
		bTotal_ += bSum_;

		sTotal_ -= ((et * (et - 1)) >> 1) * Dirties[to];
		sTotal_ += bSum_;
	}

	void Revert(const Backup& backup) {
		int to = backup.to;

		firstTurns_[to] = backup.firstTurn_;
		lastTurns_[to] = backup.lastTurn_;
		block = backup.block;
	}

	void MakeMoveBackup(int to, Backup& backup) const {
		backup.to = to;

		backup.firstTurn_ = firstTurns_[to];
		backup.lastTurn_ = lastTurns_[to];
		backup.block = block;
	}
	void Apply(const Backup& moveBackup, Backup& restoreBackup) {
		int to = moveBackup.to;

		restoreBackup.to = to;
		restoreBackup.firstTurn_ = firstTurns_[to];
		restoreBackup.lastTurn_ = lastTurns_[to];
		restoreBackup.block = block;

		firstTurns_[to] = moveBackup.firstTurn_;
		lastTurns_[to] = moveBackup.lastTurn_;
		block = moveBackup.block;
	}


	s64 GetA(int p) const {
		s64 et = scorer_.turn_ - lastTurns_[p];
		return et * Dirties[p];
	}
	s64 GetS(int p) const {
		s64 et = scorer_.turn_ - lastTurns_[p];
		return ((et * (et + 1)) >> 1) * Dirties[p];
	}

	s64 CalcTotal() const {
		return scorer_.totalDirty_;
	}
	double CalcScore() const {
		return scorer_.GetScore();
	}
};
constexpr int StateSize = sizeof(State);


struct Node {
	int from;
	int to;
	Backup backup;		
};
constexpr int NodeSize = sizeof(Node);

void MakePathMap(NNArr<int>& pathMap) {
	constexpr int DirtyThreashold = 10;		

	pathMap.assign(NN, 0);

	REP(from, NN) {
		if (Dirties[from] <= DirtyThreashold) {
			continue;
		}

		NNArr<int> dists(NN, -1);
		NNArr<int> froms(NN, -1);
		NNQue<int> que;
		que.push(from);
		dists[from] = 0;
		while (que.exist()) {
			int p = que.pop();
			for (int n : aroundMap[p]) {
				if (dists[n] >= 0) {
					continue;
				}
				dists[n] = dists[p] + 1;
				froms[n] = p;
				que.push(n);
			}
		}
		REP(to, NN) {
			if (Dirties[to] <= DirtyThreashold) {
				continue;
			}
			if (from == to) {
				continue;
			}


			int p = froms[to];
			while (p >= 0) {
				if (p == from) {
					break;
				}
				if (Dirties[p] <= DirtyThreashold) {
					++pathMap[p];
				}
				p = froms[p];
			}
		}
	}
}

void MakeWallDistMap(NNArr<int>& wallDistMap) {
	wallDistMap.assign(NN, -1);

	auto& dists = wallDistMap;
	NNQue<int> que;
	REP(i, NN) {
		if (aroundMap[i].size() < 4) {
			que.push(i);
			dists[i] = 0;
		}
	}
	while (que.exist()) {
		int p = que.pop();
		for (int n : aroundMap[p]) {
			if (dists[n] >= 0) {
				continue;
			}
			dists[n] = dists[p] + 1;
			que.push(n);
		}
	}
}


void CalcDirtyRoute(const NNArr<int>& starts, const NNArr<int>& groupMap, int startGi, NNArr<int>& route) {
	struct Node {
		int p;
		double cost;		
	};
	auto Comp = [&](const Node& a, const Node& b) {
		return a.cost > b.cost;
	};

	NNArr<double> costs(NN, 1e100);
	NNArr<int> froms(NN, -1);
	CapPriorityQueue<Node, NN_max * 4> que;
	que.clear();

	for (int p : starts) {
		que.push(Node{ p, 0 }, Comp);
		costs[p] = 0;
		froms[p] = -1;
	}

	int goal = -1;
	while (!que.empty()) {
		Node node = que.pop(Comp);

		if (groupMap[node.p] >= 0 && groupMap[node.p] != startGi) {
			goal = node.p;
			break;
		}

		if (costs[node.p] != node.cost) {
			continue;
		}

		int p = node.p;
		for (int n : aroundMap[p]) {
			double nextCost = node.cost + 1;
			nextCost -= min(0.99, Dirties[n] * 0.001);		

			if (nextCost < costs[n]) {
				costs[n] = nextCost;
				froms[n] = p;

				Node next;
				next.p = n;
				next.cost = nextCost;
				que.push(next, Comp);
			}
		}
	}
	VASSERT(goal >= 0);

	int p = goal;
	while (p >= 0) {
		route.push(p);
		p = froms[p];
	}
	route.reverse();
}

void MakeAreaPathMap(NNArr<int>& areaPathMap) {
	constexpr int DirtyThreashold = 10;		

	auto IsEasy = [&](int p) {
		return Dirties[p] <= DirtyThreashold;
	};

	static NNArr<NNArr<int>> groups;

	NNArr<int> groupMap(NN, -1);
	REP(start, NN) {
		if (groupMap[start] >= 0 || IsEasy(start)) {
			continue;
		}

		int gi = groups.size();
		auto& group = groups.push();

		NNQue<int> que;
		que.push(start);
		groupMap[start] = gi;
		while (que.exist()) {
			int p = que.pop();
			group.push(p);
			for (int n : aroundMap[p]) {
				if (groupMap[n] >= 0 || IsEasy(n)) {
					continue;
				}
				groupMap[n] = gi;
				que.push(n);
			}
		}
	}

	areaPathMap.assign(NN, 0);
	if (groups.empty()) {
		return;
	}

	REP(mergeLoop, groups.size() - 1) {
		int startGi = -1;
		int minSize = INT_MAX;
		REP(gi, groups.size()) {
			if (groups[gi].size()) {
				if (groups[gi].size() < minSize) {
					minSize = groups[gi].size();
					startGi = gi;
				}
			}
		}
		VASSERT(startGi >= 0);

		NNArr<int> route;
		CalcDirtyRoute(groups[startGi], groupMap, startGi, route);

		int goalGi = groupMap[route.back()];
		VASSERT(goalGi >= 0 && goalGi != startGi);

		auto& start = groups[startGi];
		auto& goal = groups[goalGi];
		REP(i, start.size()) {
			VASSERT(groupMap[start[i]] == startGi);
			groupMap[start[i]] = goalGi;
		}
		goal.append(start);
		start.clear();

		FOR(i, 1, route.size() - 1) {
			int p = route[i];
			VASSERT(groupMap[p] < 0);
			groupMap[p] = goalGi;
			goal.push(p);

			if (route.size() >= 8) {
				areaPathMap[p] = 2;
			}
		}
	}

	int startGi = -1;
	REP(gi, groups.size()) {
		if (groups[gi].size()) {
			startGi = gi;
			break;
		}
	}

	if (startGi >= 0) {
		for (int p : groups[startGi]) {
			if (areaPathMap[p] == 0) {
				areaPathMap[p] = 1;
			}
		}
	}
}

void CalcDirtyRoute2(const NNArr<int>& starts, const NNArr<int>& groupMap, int startGi, NNArr<double>& costs, NNArr<int>& froms) {
	struct Node {
		int p;
		double cost;		
	};
	auto Comp = [&](const Node& a, const Node& b) {
		return a.cost > b.cost;
	};

	costs.assign(NN, 1e100);
	froms.assign(NN, -1);
	CapPriorityQueue<Node, NN_max * 4> que;
	que.clear();

	for (int p : starts) {
		que.push(Node{ p, 0 }, Comp);
		costs[p] = 0;
		froms[p] = -1;
	}

	while (!que.empty()) {
		Node node = que.pop(Comp);

		if (groupMap[node.p] >= 0 && groupMap[node.p] != startGi) {
			continue;
		}

		if (costs[node.p] != node.cost) {
			continue;
		}

		int p = node.p;
		for (int n : aroundMap[p]) {
			double nextCost = node.cost + 1;
			nextCost -= min(0.99, Dirties[n] * 0.001);		

			if (nextCost < costs[n]) {
				costs[n] = nextCost;
				froms[n] = p;

				Node next;
				next.p = n;
				next.cost = nextCost;
				que.push(next, Comp);
			}
		}
	}
}
void MakeAreaPathMap2(NNArr<int>& areaPathMap) {
	constexpr int DirtyThreashold = 10;		

	auto IsEasy = [&](int p) {
		return Dirties[p] <= DirtyThreashold;
	};

	static NNArr<NNArr<int>> groups;

	NNArr<int> groupMap(NN, -1);
	REP(start, NN) {
		if (groupMap[start] >= 0 || IsEasy(start)) {
			continue;
		}

		int gi = groups.size();
		auto& group = groups.push();

		NNQue<int> que;
		que.push(start);
		groupMap[start] = gi;
		while (que.exist()) {
			int p = que.pop();
			group.push(p);
			for (int n : aroundMap[p]) {
				if (groupMap[n] >= 0 || IsEasy(n)) {
					continue;
				}
				groupMap[n] = gi;
				que.push(n);
			}
		}
	}

	areaPathMap.assign(NN, 0);

	REP(startGi, groups.size()) {
		NNArr<double> costs;
		NNArr<int> froms;
		CalcDirtyRoute2(groups[startGi], groupMap, startGi, costs, froms);

		FOR(endGi, startGi + 1, groups.size()) {
			double minCost = 1e100;
			int endP = -1;
			for (int p : groups[endGi]) {
				if (froms[p] >= 0) {
					if (costs[p] < minCost) {
						minCost = costs[p];
						endP = p;
					}
				}
			}
			if (endP >= 0) {
				NNArr<int> route;
				int p = endP;
				while (p >= 0) {
					route.push(p);
					p = froms[p];
				}
				route.reverse();

				if (route.size() >= 8) {
					FOR(i, 1, route.size() - 1) {
						int p = route[i];
						areaPathMap[p] = 2;
					}
				}
			}
		}
	}
}

void MakeDiffuseDirtyMap(NNArr<double>& dstMap) {
	constexpr int DirtyThreashold = 10;		

	auto IsEasy = [&](int p) {
		return Dirties[p] <= DirtyThreashold;
	};

	dstMap.assign(NN, 0);
	REP(i, NN) {
		REP(j, NN) {
			if (IsEasy(j)) {
				continue;
			}
			int d = distMap.GetDist(i, j);
			dstMap[j] += Dirties[i] * pow(0.3, d);
		}
	}
}

void MakeEasyDirtyDistMap(NNArr<int>& dstMap) {
	constexpr int DirtyThreashold = 10;		

	auto IsEasy = [&](int p) {
		return Dirties[p] <= DirtyThreashold;
	};

	dstMap.assign(NN, -1);

	auto& dists = dstMap;
	NNQue<int> que;
	REP(i, NN) {
		if (IsEasy(i)) {
			que.push(i);
			dists[i] = 0;
		}
	}
	while (que.exist()) {
		int p = que.pop();
		for (int n : aroundMap[p]) {
			if (dists[n] >= 0) {
				continue;
			}
			dists[n] = dists[p] + 1;
			que.push(n);
		}
	}
}

struct SolverBeam {
	DiffBeam<Node, State, Backup> beam_;

	void Run(ChronoTimer& timer, T1Arr<int>& ans, double& rawScore) {
		MakePosScoreMap();
		beam_.Initialize(800, 4, 200);

		Find(timer, ans, rawScore);
	}

	void MakePosScoreMap() {
		MakeAreaPathMap(areaPathMap_);







	}

	void Find(ChronoTimer& timer, T1Arr<int>& bestAns, double& bestScore) {
		bestAns.clear();
		bestScore = 1e100;

		bool emergency = false;
		Node* failBestNode = nullptr;
		double failBestEvalScore = -1e100;

		auto initRoot = [&](State& state, Node* node) {
			state.Init();
			node->from = -1;
			node->to = 0;
		};

		auto makeNextNodes = [&](int depth, Node* node, State& state, CapArr<pr<Node*, double>, 4>& nexts) {
			Backup backup;

			Dir failDir = Dir::N;
			if (node->from >= 0) {
				failDir = Back(gs.CalcDir1(node->from, node->to));
			}

			for (int n : aroundMap[state.p_]) {
				if (aroundMap[state.p_].size() >= 3) {
					if (failDir == gs.CalcDir1(state.p_, n)) {
						continue;
					}
				}

				Node* nextNode = beam_.New();
				nextNode->from = state.p_;
				nextNode->to = n;

				state.Move(n, backup);

				double evalScore = Eval(state);
				bool isEnd = state.notReached_ == 0 && state.p_ == 0;
				double rawScore = 1e100;
				if (isEnd) {
					rawScore = state.CalcScore();
				}
				state.MakeMoveBackup(n, nextNode->backup);

				state.Revert(backup);

				{
					auto& back = nexts.push();
					back.a = nextNode;
					back.b = evalScore;
				}

				if (isEnd) {
					if (rawScore < bestScore) {
						bestScore = rawScore;

						static vector<const Node*> nodes;
						beam_.GetBest(node, nodes);			

						bestAns.clear();
						bestAns.push(0);
						for (const Node* n : nodes) {
							bestAns.push(n->to);
						}
						bestAns.push(n);

					}
				}
				else {
					if (emergency) {
						if (failBestNode == nullptr || evalScore > failBestEvalScore) {
							failBestNode = nextNode;
							failBestEvalScore = evalScore;
						}
					}
				}
			}
		};

		auto updateState = [&](State& state, const Node* nextNode, Backup& backup) {
			state.Apply(nextNode->backup, backup);
		};

		auto revertState = [&](State& state, const Node* nextNode, const Backup& backup) {
			state.Revert(backup);
		};

		auto GetWidth = [&](int turn) {
			static int BeamWidth = BlendIntParam(HP.BeamWidth, 0, 10000, {
				ipNN.GetRate(HP.BeamWidth_NN),
				ipDirtySum.GetRate(HP.BeamWidth_DirtySum),
				});
			return BeamWidth;
		};

		static double EmergencyTime = BlendDoubleParam(HP.EmergencyTime, 0, 0.9, {
			ipNN.GetRate(HP.EmergencyTime_NN),
			ipDirtySum.GetRate(HP.EmergencyTime_DirtySum),
			});

		auto IsEnd = [&]() {
			if (!emergency) {
				if (timer.ElapseTimeMs() > TIME_LIMIT * EmergencyTime) {
					emergency = true;
				}
			}
			else {
				if (failBestNode) {
					return true;
				}
			}

			return !bestAns.empty();
		};

		beam_.Run(T_max, initRoot, makeNextNodes, updateState, revertState, GetWidth, IsEnd, 0);

		if (bestAns.empty()) {
			VASSERT(failBestNode);

			static vector<const Node*> nodes;
			beam_.GetBest(failBestNode, nodes);

			bestAns.push(0);

			State state;
			Backup backup;
			state.Init();
			for (const Node* n : nodes) {
				state.Move(n->to, backup);
				bestAns.push(n->to);
			}

			RREP(p, NN) {
				if (state.firstTurns_[p] < 0) {
					cauto& froms = distMap.froms_[p];
					int n = froms[state.p_];
					while (n >= 0) {
						state.Move(n, backup);
						bestAns.push(n);
						n = froms[n];
					}
				}
			}

			if (state.p_ != 0) {
				cauto& froms = distMap.froms_[0];
				int n = froms[state.p_];
				while (n >= 0) {
					state.Move(n, backup);
					bestAns.push(n);
					n = froms[n];
				}
				state = state;
			}
			bestScore = state.CalcScore();
		}
	}

	double Eval(const State& state) {
		double e = 0;

		static double NotReached = BlendDoubleParam(HP.NotReached, -1e10, 1e10, {
			ipNN.GetRate(HP.NotReached_NN),
			ipDirtySum.GetRate(HP.NotReached_DirtySum),
			});
		static double MeanDirty = 1000 * (double)DirtySum / (double)NN;		
		e -= state.notReached_ * MeanDirty * NotReached;

		static double STotal = BlendDoubleParam(HP.STotal, -1e10, 1e10, {
			ipNN.GetRate(HP.STotal_NN),
			ipDirtySum.GetRate(HP.STotal_DirtySum),
			});
		static double BTotal = BlendDoubleParam(HP.BTotal, -1e10, 1e10, {
			ipNN.GetRate(HP.BTotal_NN),
			ipDirtySum.GetRate(HP.BTotal_DirtySum),
			});
		static double ReachedTotalDirty = BlendDoubleParam(HP.ReachedTotalDirty, -1e10, 1e10, {
			ipNN.GetRate(HP.ReachedTotalDirty_NN),
			ipDirtySum.GetRate(HP.ReachedTotalDirty_DirtySum),
			});
		static double NotReachedTotalDirty = BlendDoubleParam(HP.NotReachedTotalDirty, -1e10, 1e10, {
			ipNN.GetRate(HP.NotReachedTotalDirty_NN),
			ipDirtySum.GetRate(HP.NotReachedTotalDirty_DirtySum),
			});

		e -= state.sTotal_ * STotal;
		e -= state.bTotal_ * BTotal;
		e -= state.scorer_.totalDirty_ * ReachedTotalDirty;
		e -= state.scorer_.nrTotalDirty_ * NotReachedTotalDirty;


		static double BSum = BlendDoubleParam(HP.BSum, -1e10, 1e10, {
			ipNN.GetRate(HP.BSum_NN),
			ipDirtySum.GetRate(HP.BSum_DirtySum),
			});
		static double ReachedDirtySum = BlendDoubleParam(HP.ReachedDirtySum, -1e10, 1e10, {
			ipNN.GetRate(HP.ReachedDirtySum_NN),
			ipDirtySum.GetRate(HP.ReachedDirtySum_DirtySum),
			});
		static double AppendTotalDirty = BlendDoubleParam(HP.AppendTotalDirty, -1e10, 1e10, {
			ipNN.GetRate(HP.AppendTotalDirty_NN),
			ipDirtySum.GetRate(HP.AppendTotalDirty_DirtySum),
			});
		static double NrDirtySum = BlendDoubleParam(HP.NrDirtySum, -1e10, 1e10, {
			ipNN.GetRate(HP.NrDirtySum_NN),
			ipDirtySum.GetRate(HP.NrDirtySum_DirtySum),
			});
		static double NrAppendTotalDirty = BlendDoubleParam(HP.NrAppendTotalDirty, -1e10, 1e10, {
			ipNN.GetRate(HP.NrAppendTotalDirty_NN),
			ipDirtySum.GetRate(HP.NrAppendTotalDirty_DirtySum),
			});


		e -= state.bSum_ * BSum;
		e -= state.scorer_.reachedDirtySum_ * ReachedDirtySum;
		e -= state.scorer_.appendTotalDirty_ * AppendTotalDirty;
		e -= state.scorer_.nrDirtySum_ * NrDirtySum;
		e -= state.scorer_.nrAppendTotalDirty_ * NrAppendTotalDirty;



		{
			static double AroundA = BlendDoubleParam(HP.AroundA, -1e10, 1e10, {
				ipNN.GetRate(HP.AroundA_NN),
				ipDirtySum.GetRate(HP.AroundA_DirtySum),
				});
			static double AroundS = BlendDoubleParam(HP.AroundS, -1e10, 1e10, {
				ipNN.GetRate(HP.AroundS_NN),
				ipDirtySum.GetRate(HP.AroundS_DirtySum),
				});


			s64 aroundS = 0;
			s64 aroundA = 0;
			for (int n : aroundMap[state.p_]) {
				s64 a = state.GetA(n);
				s64 s = state.GetS(n);
				aroundA += a;
				aroundS += s;
			}
			e += aroundA * AroundA;
			e += aroundS * AroundS;
		}

		{
			static double Around2A = BlendDoubleParam(HP.Around2A, -1e10, 1e10, {
				ipNN.GetRate(HP.Around2A_NN),
				ipDirtySum.GetRate(HP.Around2A_DirtySum),
				});
			static double Around2S = BlendDoubleParam(HP.Around2S, -1e10, 1e10, {
				ipNN.GetRate(HP.Around2S_NN),
				ipDirtySum.GetRate(HP.Around2S_DirtySum),
				});


			s64 aroundS = 0;
			s64 aroundA = 0;
			for (int n : aroundDist2Map[state.p_]) {
				s64 a = state.GetA(n);
				s64 s = state.GetS(n);
				aroundA += a;
				aroundS += s;
			}
			e += aroundA * Around2A;
			e += aroundS * Around2S;
		}

		return e;
	}

	void CutUpdate(ChronoTimer& timer, T1Arr<int>& bestAns, double& bestScore) {
		Backup backup;

		int bestStart = -1;
		int bestEnd = -1;

		RREP(i, bestAns.size()) {
			int p = bestAns[i];
			if (p == 0) {
				State state;
				state.Init();
				FOR(j, i + 1, bestAns.size()) {
					int p = bestAns[j];
					state.Move(p, backup);

					if (p == 0 && state.notReached_ == 0) {
						double score = state.CalcScore();
						if (score < bestScore) {
							bestScore = score;
							bestStart = i;
							bestEnd = j + 1;
						}
						if (timer.IsOver()) {
							break;
						}
					}
				}
				if (timer.IsOver()) {
					break;
				}
			}
		}

		if (bestStart >= 0) {
			T1Arr<int> bestCutAns;
			bestCutAns.resize(bestEnd - bestStart);
			memcpy(bestCutAns.data(), bestAns.data() + bestStart, sizeof(bestCutAns[0]) * bestCutAns.size());
			bestAns.MemCopy(bestCutAns);
		}


	}
};
#define USE_SA_POINT_FILTER 0
#define USE_SA_ROLLBACK 0
#define USE_ACCEPT_SCORE 0

constexpr int InitialSAStateCount = 1;
constexpr int MaxReachTurnCount = 10000;		



struct RandomTable {
	vector<int> table_;

	void push(int value, int count) {
		table_.reserve(table_.size() + count);
		REP(i, count) {
			table_.emplace_back(value);
		}
	}

	template <class ENGINE>
	int operator()(ENGINE& engine) {
		return table_[engine() % table_.size()];
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
#define MAKE_TRANS(func, weight) MakeTransition(#func, [&](SAChecker& sac, SAState& state) { func(sa, sac, state); }, weight)

struct SimulatedAnnealing {
	vector<SAChecker> checkers;

	TimePoint startTime_;
	TimePoint endTime_;
	double subTimeCountDiv_;

	double totalMaxScore = 0;				
	double timeRate = 0;				
	double temp = 0;					
	double divTemp = 0;					


	Xor64 rand_;

	double startTemp_ = 200;					
	double endTemp_ = 1;						
	int stepLoopCount = 1000;					

	double rollbackStartRate_ = 999.0;			
	int rollbackCount_ = INT_MAX;				
	double rollbackNextMulti_ = 1.1;			
	int minRollbackCount_ = 1;					


private:
	void UpdateTemp(double timeRate) {
		if (HP.TempType == 0) {
			temp = startTemp_ + (endTemp_ - startTemp_) * timeRate;	
		}
		else if (HP.TempType == 1) {
			temp = startTemp_ * std::pow(endTemp_ / startTemp_, timeRate);		
		}
		else if (HP.TempType == 2) {
			temp = startTemp_ * std::pow(endTemp_ / startTemp_, pow(timeRate, HP.TempPow));
		}
		else {
			VASSERT(HP.TempType == 3);
			temp = startTemp_ + (endTemp_ - startTemp_) * square(timeRate);
		}

		divTemp = 1.0 / temp;
		for (auto& checker : checkers) {
			checker.temp = temp;
			checker.divTemp = divTemp;
		}
	}

public:
	void UpdateTemp(ChronoTimer& timer) {
		timeRate = (timer.Now() - startTime_).count() * subTimeCountDiv_;
		UpdateTemp(timeRate);
	}

	template <class STATE, class ROLLBACK, class... TRANSITION>
	void Run2(ChronoTimer& timer, vector<STATE>& states, ROLLBACK&& rollback, tuple<SATransition<TRANSITION>...>& transitions) {

		checkers.resize(states.size());
		totalMaxScore = states[0].EvalScore();
		REP(pi, checkers.size()) {
			auto& checker = checkers[pi];
			checker.rand_ = &rand_;
			checker.totalMaxScore_ = &totalMaxScore;

			checker.temp = 0;
			checker.divTemp = 0;
			checker.currentScore = states[pi].EvalScore();
			checker.maxScore = checker.currentScore;
			checker.noMaxUpdateCount = 0;
			checker.nextRollbackCheckCount = rollbackCount_;

			chmax(totalMaxScore, checker.maxScore);
		}

		RandomTable randTable;
		TupleLoop(transitions, [&](auto&& e, size_t i) {
			randTable.push((int)i, e.weight);
			});

		startTime_ = timer.Now();
		endTime_ = timer.EndTime();
		subTimeCountDiv_ = 1.0 / (double)(endTime_ - startTime_).count();

		vector<int> pis(states.size());
		iota(ALL(pis), 0);

		bool forceEnd = false;
		while (!timer.IsOver()) {
			timeRate = (timer.Now() - startTime_).count() * subTimeCountDiv_;
			UpdateTemp(timeRate);


			for (int rp = 0; rp < stepLoopCount; ++rp) {
				int ti = (int)randTable(rand_);

				auto exec = [&](auto&& e, size_t i) {
					for (int pi : pis) {
						auto& checker = checkers[pi];
						e.func(checker, states[pi]);

					}
				};

				TupleAccess(transitions, ti, exec);
			}
			if (forceEnd) {
				break;
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


struct ScorerSA {
	int turn_;					
	s64 totalDirty_;			


	void InitWithAns(const T1Arr<int>& ans) {
		turn_ = ans.size() - 1;
		totalDirty_ = 0;

		NNArr<int> firstTurns(NN, -1);
		NNArr<int> lastTurns(NN, -1);
		int L = ans.size() - 1;
		FOR(t, 1, ans.size()) {
			int p = ans[t];
			if (firstTurns[p] < 0) {
				firstTurns[p] = t;
			}
			else {
				totalDirty_ += CalcBlock(lastTurns[p], t, L) * Dirties[p];
			}
			lastTurns[p] = t;
		}

		REP(p, NN) {
			totalDirty_ += CalcBlock(lastTurns[p], firstTurns[p], L) * Dirties[p];
		}

	}



	void SetFirst(int p, int turn) {
		VASSERT(turn <= turn_);
		totalDirty_ += CalcBlock(turn, turn, turn_) * Dirties[p];
	}

	void Set(int p, int prevTurn, int turn, int nextTurn) {
		s64 block = 0;






		if (prevTurn < nextTurn) {
			s64 T = turn;
			s64 N = nextTurn;
			s64 P = prevTurn;
			block = (T - N) * (T - P);
		}
		else {
			if (turn >= nextTurn) {
				s64 T = turn;
				s64 N = nextTurn + turn_;
				s64 P = prevTurn;
				block = (T - N) * (T - P);

			}
			else {
				s64 T = turn;
				s64 N = nextTurn;
				s64 P = prevTurn;
				s64 L = turn_;
				block = (T - N) * (T + L - P);

			}
		}
		totalDirty_ += block * Dirties[p];
	}

	void RemoveLast(int p, int turn) {
		VASSERT(turn <= turn_);
		totalDirty_ -= CalcBlock(turn, turn, turn_) * Dirties[p];
	}

	void Remove(int p, int prevTurn, int turn, int nextTurn) {
		s64 block = 0;

		if (prevTurn < nextTurn) {
			s64 T = turn;
			s64 N = nextTurn;
			s64 P = prevTurn;
			block = -(T - N) * (T - P);
		}
		else {
			if (turn >= nextTurn) {
				s64 T = turn;
				s64 N = nextTurn + turn_;
				s64 P = prevTurn;
				block = -(T - N) * (T - P);

			}
			else {
				s64 T = turn;
				s64 N = nextTurn;
				s64 P = prevTurn;
				s64 L = turn_;
				block = -(T - N) * (T + L - P);

			}
		}


		totalDirty_ += block * Dirties[p];
	}

	void Insert(int p, int prevTurn, int nextTurn, int appendTurn) {
		s64 block = 0;
		block -= CalcBlock(prevTurn, nextTurn, turn_);
		block += CalcBlockAppend(prevTurn, nextTurn, turn_, appendTurn);
		totalDirty_ += block * Dirties[p];
	}

	void FinishInsert(int appendTurn) {
		turn_ += appendTurn;
	}

	double GetScore() const {
		return totalDirty_ / (double)turn_;
	}

};

struct TmpReachCounts {
	NNMap<int> tmpReachCounts_;		
	int notReachCount_;				

	void Init() {
		tmpReachCounts_.clear();
		notReachCount_ = 0;
	}

	void Remove(const NNArr<int>& srcReachCounts, int p) {
		tmpReachCounts_.AddValue(p, -1, srcReachCounts[p]);
		if (tmpReachCounts_.get(p) == 0) {
			++notReachCount_;
		}
	}
	void Add(const NNArr<int>& srcReachCounts, int p) {
		tmpReachCounts_.AddValue(p, 1, srcReachCounts[p]);
		if (tmpReachCounts_.get(p) == 1) {
			--notReachCount_;
		}
	}

	int Get(const NNArr<int>& srcReachCounts, int p) const {
		if (tmpReachCounts_.contains(p)) {
			return tmpReachCounts_.get(p);
		}
		return srcReachCounts[p];
	}
};

struct SABackup {
	int start_;
	int end_;
	T1Arr<int> ans_;		
	double rawScore_;
};

struct SAState {
	T1Arr<int> ans_;				
	NNArr<int> reachCounts_;		
	int notReachCount_;				
	double rawScore_;

	NNArr<CapArr<int, MaxReachTurnCount>> reachTurns_;	
	ScorerSA scorer_;

	void Init(const T1Arr<int>& ans, double rawScore, bool isRollback) {
		rawScore_ = rawScore;
		ans_.MemCopy(ans);

		notReachCount_ = NN;
		reachCounts_.assign(NN, 0);
		FOR(i, 1, ans_.size()) {
			if (reachCounts_[ans_[i]] == 0) {
				--notReachCount_;
			}
			++reachCounts_[ans_[i]];
		}
		VASSERT(notReachCount_ == 0);

		reachTurns_.resize(NN);
		REP(i, NN) {
			reachTurns_[i].clear();
		}

		scorer_.InitWithAns(ans);

		int L = ans.size() - 1;
		FOR(i, 1, ans.size()) {
			int t = i;
			int p = ans[i];
			reachTurns_[p].push(t);
		}
		VASSERT(scorer_.GetScore() == rawScore);
	}

	int RawScore() const {
		return -(int)round(rawScore_);
	}
	double EvalScore() const {
		return -rawScore_ * ScoreNormalize;
	}

	bool CalcRouteSameTurn(int start, int end, NNArr<int>& route) const {
		VASSERT(start < end);
		VASSERT(end - start >= 2);
		VASSERT(end < ans_.size());


		TmpReachCounts tmpReachCounts;
		tmpReachCounts.Init();
		FOR(i, start + 1, end) {
			int p = ans_[i];
			tmpReachCounts.Remove(reachCounts_, p);
			if (tmpReachCounts.notReachCount_ != 0) {
				return false;
			}
		}

		CapArr<int, 2> ris;
		if (!routeTbl_.GetRandom2(ans_[start], ans_[end], end - start, end - start, rand_, ris)) {
			return false;
		}

		auto IsSame = [&](const Route& tblRoute) {
			REP(i, tblRoute.size()) {
				if (tblRoute[i] != ans_[start + 1 + i]) {
					return false;
				}
			}
			return true;
		};

		VASSERT(ris.size() >= 1);
		if (!IsSame(routeTbl_.GetRoute(ris[0]))) {
			route.MemCopy(routeTbl_.GetRoute(ris[0]));
		}
		else {
			if (ris.size() == 2) {
				route.MemCopy(routeTbl_.GetRoute(ris[1]));
			}
			else {
				return false;
			}
		}
		VASSERT(route.size() == end - start - 1);

		return true;
	}

	bool CalcRouteAddTurn(int start, int end, NNArr<int>& route) const {
		VASSERT(start < end);
		VASSERT(end - start >= 1);
		VASSERT(end < ans_.size());

		TmpReachCounts tmpReachCounts;
		tmpReachCounts.Init();
		FOR(i, start + 1, end) {
			int p = ans_[i];
			tmpReachCounts.Remove(reachCounts_, p);
			if (tmpReachCounts.notReachCount_ != 0) {
				return false;
			}
		}

		int idx = routeTbl_.GetRandom(ans_[start], ans_[end], end - start + 1, end - start + HP.Trans2_AppendableTurn, rand_);
		if (idx < 0) {
			return false;
		}
		route.MemCopy(routeTbl_.GetRoute(idx));

		if (ans_.size() - (end - start - 1) + route.size() > ans_.capacity()) {
			return false;
		}

		return true;
	}

	bool CalcRouteRemoveTurn(int start, int end, NNArr<int>& route) const {
		VASSERT(start < end);
		VASSERT(end - start >= 1);
		VASSERT(end < ans_.size());

		if (distMap.GetDist(ans_[start], ans_[end]) == end - start) {
			return false;
		}
		TmpReachCounts tmpReachCounts;
		tmpReachCounts.Init();
		FOR(i, start + 1, end) {
			int p = ans_[i];
			tmpReachCounts.Remove(reachCounts_, p);
			if (tmpReachCounts.notReachCount_ != 0) {
				return false;
			}
		}

		int idx = routeTbl_.GetRandom(ans_[start], ans_[end], 0, end - start - 2, rand_);
		if (idx < 0) {
			return false;
		}
		route.MemCopy(routeTbl_.GetRoute(idx));
		return true;
	}

	bool CalcRouteSameTotalTurn(int& start, int& end, NNArr<int>& route) const {
		VASSERT(start < end);
		VASSERT(end - start >= 1);
		VASSERT(end < ans_.size());

		TmpReachCounts tmpReachCounts;
		tmpReachCounts.Init();
		FOR(i, start + 1, end) {
			int p = ans_[i];
			tmpReachCounts.Remove(reachCounts_, p);
			if (tmpReachCounts.notReachCount_ != 0) {
				return false;
			}
		}

		int idx = routeTbl_.GetRandom(ans_[start], ans_[end], end - start + 1, end - start + 2, rand_);
		if (idx < 0) {
			return false;
		}
		route.MemCopy(routeTbl_.GetRoute(idx));

		const int srcRoute = end - start - 1;

		int appendTurn = route.size() - srcRoute;
		VASSERT(appendTurn > 0);

		if (appendTurn > 0) {
			REP(i, route.size()) {
				int p = route[i];
				tmpReachCounts.Add(reachCounts_, p);
			}

			VASSERT(appendTurn == 2);	

			Dir hdir = Dir::N;
			Dir vdir = Dir::N;
			int lastHMoveT = -1;
			int lastVMoveT = -1;
			int skipStart = -1;
			int skipEnd = -1;

			auto FindBack = [&]() {
				hdir = Dir::N;
				vdir = Dir::N;
				lastHMoveT = -1;
				lastVMoveT = -1;
				skipStart = -1;
				skipEnd = -1;

				FOR(i, end, min(ans_.size() - 1, end + HP.MaxSkipLength)) {
					if (tmpReachCounts.Get(reachCounts_, ans_[i]) == 1) {
						hdir = Dir::N;
						vdir = Dir::N;
						lastHMoveT = -1;
						lastVMoveT = -1;
						skipStart = -1;
						skipEnd = -1;
					}

					Dir dir = gs.CalcDir1(ans_[i], ans_[i + 1]);
					if (IsHorizontal(dir)) {
						if (hdir != Dir::N && hdir != dir) {
							int t = i + 1 - lastHMoveT;
							int d = distMap.GetDist(ans_[lastHMoveT], ans_[i + 1]);
							if (d < t) {
								if (d + 2 == t) {
									skipStart = lastHMoveT;
									skipEnd = i + 1;
									break;
								}
								else {
									VABORT();
								}
							}
						}
						lastHMoveT = i;
						hdir = dir;
					}
					else {
						if (vdir != Dir::N && vdir != dir) {
							int t = i + 1 - lastVMoveT;
							int d = distMap.GetDist(ans_[lastVMoveT], ans_[i + 1]);
							if (d < t) {
								if (d + 2 == t) {
									skipStart = lastVMoveT;
									skipEnd = i + 1;
									break;
								}
								else {
									VABORT();
								}
							}
						}
						lastVMoveT = i;
						vdir = dir;
					}
				}
			};
			auto FindFront = [&]() {
				hdir = Dir::N;
				vdir = Dir::N;
				lastHMoveT = -1;
				lastVMoveT = -1;
				skipStart = -1;
				skipEnd = -1;

				RFOR(i, max(1, start - HP.MaxSkipLength), start) {
					if (tmpReachCounts.Get(reachCounts_, ans_[i]) == 1) {
						hdir = Dir::N;
						vdir = Dir::N;
						lastHMoveT = -1;
						lastVMoveT = -1;
						skipStart = -1;
						skipEnd = -1;
					}

					Dir dir = gs.CalcDir1(ans_[i], ans_[i - 1]);
					if (IsHorizontal(dir)) {
						if (hdir != Dir::N && hdir != dir) {
							int t = lastHMoveT - (i - 1);
							int d = distMap.GetDist(ans_[lastHMoveT], ans_[i - 1]);
							if (d < t) {
								if (d + 2 == t) {
									skipStart = i - 1;
									skipEnd = lastHMoveT;
									break;
								}
								else {
								}
							}
						}
						lastHMoveT = i;
						hdir = dir;
					}
					else {
						if (vdir != Dir::N && vdir != dir) {
							int t = lastVMoveT - (i - 1);
							int d = distMap.GetDist(ans_[lastVMoveT], ans_[i - 1]);
							if (d < t) {
								if (d + 2 == t) {
									skipStart = i - 1;
									skipEnd = lastVMoveT;
									break;
								}
								else {
								}
							}
						}
						lastVMoveT = i;
						vdir = dir;
					}
				}
			};

			if (rand_(2)) {
				FindFront();
				if (skipStart < 0) {
					FindBack();
				}
			}
			else {
				FindBack();
				if (skipStart < 0) {
					FindFront();
				}
			}

			if (skipStart < 0) {
				return false;
			}

			if (skipStart < end) {
				NNArr<int> tmp;
				tmp.MemCopy(route);
				route.clear();

				cauto& froms = distMap.froms_[ans_[skipEnd]];
				int p = froms[ans_[skipStart]];		
				while (p >= 0) {
					route.push(p);
					p = froms[p];
				}
				FOR(i, skipEnd + 1, start + 1) {
					route.push(ans_[i]);
				}
				route.append(tmp);
				start = skipStart;
			}
			else {
				FOR(i, end, skipStart) {
					route.push(ans_[i]);
				}
				cauto& froms = distMap.froms_[ans_[skipEnd]];
				int p = ans_[skipStart];
				while (froms[p] >= 0) {
					route.push(p);
					p = froms[p];
				}
				end = skipEnd;
			}

			VASSERT(route.size() == end - start - 1);
		}

		return true;
	}

	struct Inserter {
		s64 appendDirtySum_;

		void Init() {
			appendDirtySum_ = 0;
		}

		int PrevIdx(const CapArr<int, MaxReachTurnCount>& turns, int idx) const {
			VASSERT(!turns.empty());
			if (idx > 0) {
				return idx - 1;
			}
			return turns.size() - 1;
		}
		int NextIdx(const CapArr<int, MaxReachTurnCount>& turns, int idx) const {
			VASSERT(!turns.empty());
			if (idx + 1 < turns.size()) {
				return idx + 1;
			}
			return 0;
		}

		void AddRange(const CapArr<int, MaxReachTurnCount>& turns, int L, int p, int nextIdx) {
			if (nextIdx >= turns.size()) {
				nextIdx = 0;
			}
			int prevIdx = PrevIdx(turns, nextIdx);
			AddRange(turns, L, p, prevIdx, nextIdx);
		}
		void AddRange(const CapArr<int, MaxReachTurnCount>& turns, int L, int p, int prevIdx, int nextIdx) {
			int et = turns[nextIdx] - turns[prevIdx];
			if (et <= 0) {
				et += L;
			}
			appendDirtySum_ += et * Dirties[p];
		}

		void RemoveRange(const CapArr<int, MaxReachTurnCount>& turns, int L, int p, int nextIdx) {
			if (nextIdx >= turns.size()) {
				nextIdx = 0;
			}
			int prevIdx = PrevIdx(turns, nextIdx);
			RemoveRange(turns, L, p, prevIdx, nextIdx);
		}
		void RemoveRange(const CapArr<int, MaxReachTurnCount>& turns, int L, int p, int prevIdx, int nextIdx) {
			int et = turns[nextIdx] - turns[prevIdx];
			if (et <= 0) {
				et += L;
			}
			appendDirtySum_ -= et * Dirties[p];
		}

		int GetRangeBlock(const CapArr<int, MaxReachTurnCount>& turns, int L, int p, int nextIdx) {
			if (nextIdx >= turns.size()) {
				nextIdx = 0;
			}
			int prevIdx = PrevIdx(turns, nextIdx);
			int et = turns[nextIdx] - turns[prevIdx];
			if (et <= 0) {
				et += L;
			}
			return et;
		}
		void AddRangeWithRemoveBlock(const CapArr<int, MaxReachTurnCount>& turns, int L, int p, int nextIdx, int removeBlock) {
			if (nextIdx >= turns.size()) {
				nextIdx = 0;
			}
			int prevIdx = PrevIdx(turns, nextIdx);
			int et = turns[nextIdx] - turns[prevIdx];
			if (et <= 0) {
				et += L;
			}
			appendDirtySum_ += (et - removeBlock) * Dirties[p];
		}

	};

	bool CalcUpdatedScore(int start, int end, const NNArr<int>& route, ScorerSA& scorer, const NNArr<int>& rtIdxs, const Inserter& inserter) const {
		if (end - start - 1 != route.size()) {
			if (!CalcUpdatedScore1(start, end, route, scorer, rtIdxs, inserter)) {
				return false;
			}
		}
		else {
			if (!CalcUpdatedScore2(start, end, route, scorer, rtIdxs)) {
				return false;
			}
		}
		return true;
	}

	int LinearFind(const CapArr<int, MaxReachTurnCount>& turns, int idx, int t) const {
		VASSERT(idx == 0 || turns[idx - 1] < t);
		int size = turns.size();
		while (idx < size) {
			if (turns[idx] >= t) {
				break;
			}
			++idx;
		}
		return idx;
	}

	bool CalcUpdatedScore1(int start, int end, const NNArr<int>& route, ScorerSA& scorer, const NNArr<int>& rtIdxs, const Inserter& inserter) const {
		static CapSet<NN_max* MaxReachTurnCount> removedTurns;		
		removedTurns.clear();

		auto FindPrev = [&](const CapArr<int, MaxReachTurnCount>& turns, int removedTurnsOffset, int startIdx) {

			int idx = startIdx;
			while (true) {
				--idx;
				if (idx < 0) {
					idx = turns.size() - 1;
				}
				int ri = removedTurnsOffset + idx;
				if (removedTurns.contains(ri)) {
					continue;
				}
				break;
			}
			return idx;
		};
		auto FindNext = [&](const CapArr<int, MaxReachTurnCount>& turns, int removedTurnsOffset, int startIdx) {

			int idx = startIdx;
			while (true) {
				++idx;
				if (idx >= turns.size()) {
					idx = 0;
				}
				int ri = removedTurnsOffset + idx;
				if (removedTurns.contains(ri)) {
					continue;
				}
				break;
			}
			return idx;
		};
		auto FindExist = [&](const CapArr<int, MaxReachTurnCount>& turns, int removedTurnsOffset, int startIdx) {
			int idx = startIdx;
			REP(loop, turns.size()) {
				int ri = removedTurnsOffset + idx;
				if (!removedTurns.contains(ri)) {
					return idx;
				}
				++idx;
				if (idx >= turns.size()) {
					idx = 0;
				}
			}
			return -1;
		};

		scorer = scorer_;
		Inserter tmpInserter = inserter;
		NNSet removedP;

		FOR(t, start + 1, end) {
			int p = ans_[t];
			int removedTurnsOffset = p * NN;

			auto& turns = reachTurns_[p];
			int idx = LinearFind(turns, rtIdxs[p], t);
			VASSERT(turns[idx] == t);
			VASSERT(!removedTurns.contains(removedTurnsOffset + idx));		
			if (removedTurns.contains(removedTurnsOffset + idx)) {
				VABORT();
				return false;
			}

			int prevIdx = FindPrev(turns, removedTurnsOffset, idx);
			int nextIdx = FindNext(turns, removedTurnsOffset, idx);
			if (prevIdx == idx && idx == nextIdx) {
				scorer.RemoveLast(p, t);
			}
			else {
				scorer.Remove(p, turns[prevIdx], t, turns[nextIdx]);
			}

			if (!removedP.contains(p)) {
				tmpInserter.RemoveRange(turns, ans_.size() - 1, p, rtIdxs[p]);
				removedP.add(p);
			}

			removedTurns.Add(removedTurnsOffset + idx);
		}

		for (int p : removedP) {
			auto& turns = reachTurns_[p];
			int removedTurnsOffset = p * NN;
			int idx = rtIdxs[p];
			int prevIdx = FindPrev(turns, removedTurnsOffset, idx);
			if (removedTurns.contains(removedTurnsOffset + idx)) {
				idx = FindNext(turns, removedTurnsOffset, idx);
			}
			tmpInserter.AddRange(turns, ans_.size() - 1, p, prevIdx, idx);
		}

		int oldRouteSize = end - start - 1;
		int appendTurn = route.size() - oldRouteSize;
		if (appendTurn != 0) {
			s64 diffDirty = 0;
			if (appendTurn > 0) {
				REP(i, appendTurn) {
					diffDirty += tmpInserter.appendDirtySum_;
					tmpInserter.appendDirtySum_ += DirtySum;
				}
			}
			else {
				REP(i, -appendTurn) {
					tmpInserter.appendDirtySum_ -= DirtySum;
					diffDirty -= tmpInserter.appendDirtySum_;
				}
			}
			scorer.totalDirty_ += diffDirty;
		}
		scorer.FinishInsert(appendTurn);



		NNMap<int> addedTurns;
		REP(i, route.size()) {
			int t = start + 1 + i;
			int p = route[i];
			int removedTurnsOffset = p * NN;

			auto& turns = reachTurns_[p];
			int nextIdx = LinearFind(turns, rtIdxs[p], end);
			if (nextIdx >= turns.size()) {
				nextIdx = 0;
			}
			nextIdx = FindExist(turns, removedTurnsOffset, nextIdx);
			if (nextIdx < 0) {
				VABORT();
				scorer.SetFirst(p, t);
			}
			else {
				int nextTurn = turns[nextIdx];

				int prevTurn = -1;
				if (addedTurns.contains(p)) {
					prevTurn = addedTurns.get(p);

				}
				else {
					int prevIdx = FindPrev(turns, removedTurnsOffset, nextIdx);
					prevTurn = turns[prevIdx];

					if (prevTurn >= end) {
						prevTurn += appendTurn;
					}
				}

				if (nextTurn >= end) {
					nextTurn += appendTurn;
				}

				scorer.Set(p, prevTurn, t, nextTurn);
			}

			addedTurns.set(p, t);
		}

		return true;
	}

	bool CalcUpdatedScore2(int start, int end, const NNArr<int>& route, ScorerSA& scorer, const NNArr<int>& rtIdxs) const {

		static NNArr<CapArr<int, MaxReachTurnCount>> reachTurns;		
		reachTurns.resize(NN);
		NNSet copiedReachTurns;

		auto GetReachTurns = [&](int p) -> CapArr<int, MaxReachTurnCount>&{
			if (!copiedReachTurns.contains(p)) {
				auto& dstTurns = reachTurns[p];
				dstTurns.clear();

				cauto& srcTurns = reachTurns_[p];
				if (srcTurns.empty()) {
				}
				else {
					dstTurns.MemCopy(srcTurns);
				}

				copiedReachTurns.add(p);
			}
			return reachTurns[p];
		};

		auto FindPrev = [&](const CapArr<int, MaxReachTurnCount>& turns, int startIdx) {
			if (startIdx > 0) {
				return startIdx - 1;
			}
			return turns.size() - 1;
		};
		auto FindNext = [&](const CapArr<int, MaxReachTurnCount>& turns, int startIdx) {
			if (startIdx + 1 >= turns.size()) {
				return 0;
			}
			return startIdx + 1;
		};

		scorer = scorer_;

		FOR(t, start + 1, end) {
			int p = ans_[t];

			auto& turns = GetReachTurns(p);
			if (turns.size() == 1) {
				scorer.RemoveLast(p, t);
				turns.clear();
			}
			else {
				int idx = LinearFind(turns, rtIdxs[p], t);
				VASSERT(turns[idx] == t);

				int prev = 0;
				if (idx == 0) {
					prev = turns.back();
				}
				else {
					prev = turns[idx - 1];
				}
				int next = 0;
				if (idx == turns.size() - 1) {
					next = turns.front();
				}
				else {
					next = turns[idx + 1];
				}
				turns.remove(idx);
				scorer.Remove(p, prev, t, next);
			}
		}

		int oldRouteSize = end - start - 1;
		int appendTurn = route.size() - oldRouteSize;
		if (appendTurn != 0) {
			VABORT();

			REP(p, NN) {
				int removedTurnsOffset = p * NN;

				auto& turns = GetReachTurns(p);
				if (turns.empty()) {
					VABORT();
				}
				else {
					int nextIdx = LinearFind(turns, rtIdxs[p], end);
					VASSERT(rtIdxs[p] == nextIdx);
					if (nextIdx >= turns.size()) {
						nextIdx = 0;
					}

					int prevIdx = FindPrev(turns, nextIdx);
					scorer.Insert(p, turns[prevIdx], turns[nextIdx], appendTurn);

					if (turns[nextIdx] >= end) {
						FOR(idx, nextIdx, turns.size()) {
							turns[idx] += appendTurn;
						}
					}
				}
			}
			scorer.FinishInsert(appendTurn);
		}



		REP(i, route.size()) {
			int t = start + 1 + i;
			int p = route[i];

			auto& turns = GetReachTurns(p);
			if (turns.empty()) {
				scorer.SetFirst(p, t);
				turns.push(t);
			}
			else {
				int idx = LinearFind(turns, rtIdxs[p], t);
				int prev = 0;
				if (idx == 0) {
					prev = turns.back();
				}
				else {
					prev = turns[idx - 1];
				}
				int next = 0;
				if (idx == turns.size()) {
					next = turns.front();
				}
				else {
					next = turns[idx];
				}
				scorer.Set(p, prev, t, next);
				turns.insert(idx, t);
			}
		}

		return true;
	}

	void ApplyChangeTurn(int start, int end, const NNArr<int>& route, const ScorerSA& scorer, const NNArr<int>& rtIdxs, Inserter& inserter) {
		VASSERT(start < end);

		NNSet removedP;		
		int oldL = ans_.size() - 1;
		FOR(t, start + 1, end) {
			int p = ans_[t];
			--reachCounts_[p];
			VASSERT(reachCounts_[p] >= 0);
			if (reachCounts_[p] == 0) {
				++notReachCount_;
			}

			auto& turns = reachTurns_[p];
			int idx = LinearFind(turns, rtIdxs[p], t);
			VASSERT(turns[idx] == t);
			if (!removedP.contains(p)) {
				inserter.RemoveRange(turns, oldL, p, rtIdxs[p]);
				removedP.add(p);
			}
			turns.remove(idx);
		}


		int oldRouteSize = end - start - 1;
		int appendTurn = route.size() - oldRouteSize;
		int newL = ans_.size() - 1 + appendTurn;
		if (appendTurn != 0) {
			REP(p, NN) {
				auto& turns = reachTurns_[p];
				int startIdx = -1;
				if (removedP.contains(p)) {
					startIdx = LinearFind(turns, rtIdxs[p], end);
				}
				else {
					startIdx = rtIdxs[p];
					VASSERT(startIdx >= turns.size() || turns[startIdx] >= end);

					if (rtIdxs[p] >= startIdx) {
						inserter.RemoveRange(turns, oldL, p, rtIdxs[p]);
						removedP.add(p);
					}
				}

				FOR(i, startIdx, turns.size()) {
					turns[i] += appendTurn;
				}
			}
		}

		REP(i, route.size()) {
			int t = start + 1 + i;
			int p = route[i];
			if (reachCounts_[p] == 0) {
				--notReachCount_;
				VASSERT(notReachCount_ >= 0);
			}
			++reachCounts_[p];

			auto& turns = reachTurns_[p];
			int idx = LinearFind(turns, rtIdxs[p], t);
			if (!removedP.contains(p)) {
				inserter.RemoveRange(turns, oldL, p, rtIdxs[p]);
				removedP.add(p);
			}

			turns.insert(idx, t);
		}

		for (int p : removedP) {
			inserter.AddRange(reachTurns_[p], newL, p, rtIdxs[p]);
		}

		ans_.RemoveInsert(start + 1, end, route.data(), route.size());
		scorer_ = scorer;
		rawScore_ = scorer.GetScore();

		VASSERT(notReachCount_ == 0);
	}


};
static constexpr int SAStateSize = sizeof(SAState);

struct SolverSA {
	Xor64 rand_;
	NNArr<int> closedMap_;		

	double bestScore_;
	T1Arr<int> bestAns_;

	void CheckMaxScore(const SAState& state) {
		if (state.rawScore_ < bestScore_) {
			bestScore_ = state.rawScore_;
			bestAns_.MemCopy(state.ans_);
		}
	}

	void Run(ChronoTimer& timer, T1Arr<int>& ans, double& rawScore) {
		{
			closedMap_.assign(NN, 0);
			REP(i, NN) {
				if ((dirMap[i][0] < 0 && dirMap[i][2] < 0) || (dirMap[i][1] < 0 && dirMap[i][3] < 0)) {
					closedMap_[i] = 1;
				}
			}

			REP(i, NN) {
				if (closedMap_[i] == 1) {
					continue;
				}
				bool aroundClose = false;
				for (int n : aroundMap[i]) {
					if (closedMap_[n] == 1) {
						aroundClose = true;
						break;
					}
				}
				if (aroundClose) {
					closedMap_[i] = 2;
				}
			}

		}

		vector<SAState> states;
		InitSAState(states, ans, rawScore);
		bestScore_ = states[0].rawScore_;
		bestAns_.MemCopy(states[0].ans_);


		static double StartTemp = BlendDoubleParam(HP.StartTemp, 0.0, 1e10, {
			ipNN.GetRate(HP.StartTemp_NN),
			ipDirtySum.GetRate(HP.StartTemp_DirtySum),
			});
		static double EndTemp = BlendDoubleParam(HP.EndTemp, 0.0, 1e10, {
			ipNN.GetRate(HP.EndTemp_NN),
			ipDirtySum.GetRate(HP.EndTemp_DirtySum),
			});

		SimulatedAnnealing sa;
		sa.startTemp_ = StartTemp;
		sa.endTemp_ = EndTemp;
		sa.stepLoopCount = 1;

		sa.rollbackStartRate_ = HP.RollbackStartRate;
		sa.rollbackCount_ = HP.RollbackCount;		
		sa.rollbackNextMulti_ = HP.RollbackNextMulti;
		sa.minRollbackCount_ = HP.MinRollbackCount;

		auto transitions = make_tuple(
			MAKE_TRANS(Transition_Update5, 1)
		);

		auto rollback = [&]() {
			states[0].Init(bestAns_, bestScore_, true);
		};

		sa.Run2(timer, states, rollback, transitions);


		ans.MemCopy(bestAns_);
		rawScore = bestScore_;
	}

	void InitSAState(vector<SAState>& states, T1Arr<int>& ans, double& rawScore) {
		states.resize(InitialSAStateCount);

		{
			SAState& state = states[0];
			state.Init(ans, rawScore, false);
		}

		FOR(i, 1, InitialSAStateCount) {
			states[i] = states[0];
		}
	}

	void Transition_Update5(SimulatedAnnealing& sa, SAChecker& checker, SAState& state) {
		auto& ans_ = state.ans_;

		static RandomTableS<5000> randTable;
		if (randTable.table_.empty()) {
			randTable.push(1, HP.Trans1);
			randTable.push(2, HP.Trans2);
			randTable.push(3, HP.Trans3);
			randTable.push(4, HP.Trans4);
		}
		static RandomTableS<5000> randTableAroundClose;
		if (randTableAroundClose.table_.empty()) {
			randTableAroundClose.push(1, HP.Trans1);
			randTableAroundClose.push(2, HP.Trans2);
			randTableAroundClose.push(3, HP.Trans3);
			randTableAroundClose.push(4, HP.Trans4);
			randTableAroundClose.push(5, HP.Trans5);
		}

		NNArr<int> rtIdxs;		
		rtIdxs.assign(NN, 0);

		SAState::Inserter inserter;
		inserter.Init();
		{
			int L = ans_.size() - 1;
			REP(p, NN) {
				cauto& turns = state.reachTurns_[p];
				inserter.AddRange(turns, L, p, 0);
			}
		}

		int startT = 0;
		int timerCounter = 0;
		NNArr<int> route;
		while (startT < state.ans_.size() - 1) {
			++timerCounter;
			timerCounter &= 0xFF;
			if (timerCounter == 0) {
				if (timer_.IsOver()) {
					break;
				}
				sa.UpdateTemp(timer_);
			}

			int start = startT;
			if (startT > 0) {
				int p = state.ans_[startT];
				cauto& turns = state.reachTurns_[p];
				VASSERT(turns[rtIdxs[p]] == startT);

				int L = ans_.size() - 1;
				inserter.RemoveRange(turns, L, p, rtIdxs[p]);

				++rtIdxs[p];

				inserter.AddRange(turns, L, p, rtIdxs[p]);

			}

			bool restoreTurn = false;
			[&]() {
				int startP = ans_[startT];
				CapArr<int, 4> closedCandidate;
				if (closedMap_[startP] > 0) {
					int end = start + 1;
					for (int n : aroundMap[ans_[start]]) {
						if (closedMap_[n] != 1) {
							continue;
						}
						if (n == ans_[end]) {
							continue;
						}
						if (start - 1 >= 0 && n == ans_[start - 1]) {
							continue;
						}
						closedCandidate.push(n);
					}
				}

				int pt = -1;
				if (closedCandidate.empty()) {
					pt = randTable(rand_);
				}
				else {
					pt = randTableAroundClose(rand_);
				}

				route.clear();
				int end = -1;

				if (pt == 1) {
					int size = HP.Trans1_LowerSize + rand_((int)HP.Trans1_RandSize);
					end = start + size;
					if (end >= ans_.size()) {
						return;
					}
					if (!state.CalcRouteSameTurn(start, end, route)) {
						return;
					}
				}
				else if (pt == 2) {
					int size = HP.Trans2_LowerSize + rand_((int)HP.Trans2_RandSize);
					end = start + size;
					if (end >= ans_.size()) {
						return;
					}
					if (!state.CalcRouteAddTurn(start, end, route)) {
						return;
					}
				}
				else if (pt == 3) {
					int size = HP.Trans3_LowerSize + rand_((int)HP.Trans3_RandSize);
					end = start + size;
					if (end >= ans_.size()) {
						return;
					}

					if (!state.CalcRouteSameTotalTurn(start, end, route)) {
						return;
					}

					if (start < startT) {
						RFOR(t, start + 1, startT + 1) {
							int p = ans_[t];
							cauto& turns = state.reachTurns_[p];
							int L = ans_.size() - 1;
							inserter.RemoveRange(turns, L, p, rtIdxs[p]);

							--rtIdxs[p];
							VASSERT(rtIdxs[p] >= 0);

							inserter.AddRange(turns, L, p, rtIdxs[p]);

						}
						restoreTurn = true;
					}
				}
				else if (pt == 4) {
					int size = HP.Trans4_LowerSize + rand_((int)HP.Trans4_RandSize);
					end = start + size;
					if (end >= ans_.size()) {
						return;
					}
					if (!state.CalcRouteRemoveTurn(start, end, route)) {
						return;
					}
				}
				else if (pt == 5) {
					end = start + 1;
					VASSERT(end < ans_.size());
					VASSERT(closedCandidate.empty() == false);
					int n = closedCandidate[rand_(closedCandidate.size())];
					route.clear();
					route.push(n);

					int startP = ans_[start];

					if (rand_(2)) {
						int next = -1;
						for (int nn : aroundMap[n]) {
							if (nn == startP) {
								continue;
							}
							if (closedMap_[nn] == 1) {
								next = nn;
								break;
							}
						}
						if (next >= 0) {
							route.push(next);
							route.push(n);
						}
					}
					route.push(startP);
				}
				else {
					VABORT();
				}

				ScorerSA scorer;
				if (!state.CalcUpdatedScore(start, end, route, scorer, rtIdxs, inserter)) {
					return;
				}

				double evalScore = -scorer.GetScore() * ScoreNormalize;
				if (checker(evalScore)) {
					state.ApplyChangeTurn(start, end, route, scorer, rtIdxs, inserter);


					CheckMaxScore(state);
				}
			}();

			if (restoreTurn) {
				FOR(t, start + 1, startT + 1) {
					int p = state.ans_[t];
					VASSERT(state.reachTurns_[p][rtIdxs[p]] == t);

					cauto& turns = state.reachTurns_[p];
					int L = ans_.size() - 1;
					inserter.RemoveRange(turns, L, p, rtIdxs[p]);

					++rtIdxs[p];

					inserter.AddRange(turns, L, p, rtIdxs[p]);

				}
			}

			++startT;
		}
	}






};

struct Solver {
	SolverBeam beam_;
	SolverSA sa_;

	void Run(ChronoTimer& timer) {
		T1Arr<int> ans;
		double rawScore = 1e100;

		static double BeamTimeWeight = BlendDoubleParam(HP.BeamTimeWeight, 0, 1e10, {
			ipNN.GetRate(HP.BeamTimeWeight_NN),
			ipDirtySum.GetRate(HP.BeamTimeWeight_DirtySum),
			});

		double totalWeight = BeamTimeWeight + HP.SATimeWeight;
		int beamTime = int(round(TIME_LIMIT * BeamTimeWeight / totalWeight));

		timer.Start(beamTime);
		beam_.Run(timer, ans, rawScore);
		VASSERT(rawScore == CalcScore(ans));

		timer.Start(TIME_LIMIT);
		sa_.Run(timer, ans, rawScore);
		VASSERT(rawScore == CalcScore(ans));

		string output;
		VASSERT(ans.size());
		PointsToDirs(ans, output);

		server.Output(output);

	}
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
