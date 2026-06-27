
#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0

#define TIME_LIMIT (5900)

#define NOT_SUBMIT 0
#define VALIDATION 0

#define IO_FILE 0

#define OUTPUT_INFO 0
#define OUTPUT_FINAL_INFO 0
#define OUTPUT_LOG 0
#define OUTPUT_VISUAL 0

#define FIX_RESULT 0
#define FIX_LOOP_COUNT 1000


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

#else
#include <bits/stdc++.h>
#endif

using namespace std;


#define FOR(i, s, e) for (int i = int(s); i < int(e); ++i)
#define RFOR(i, s, e) for (int i = int(e) - 1; i >= int(s); --i)
#define REP(i, n) for (int i = 0, i##_size = int(n); i < i##_size; ++i)
#define RREP(i, n) for (int i = int(n) - 1; i >= 0; --i)


#define ALL(x) (x).begin(),(x).end()

template <class T, class U> inline void chmin(T& a, U b) { if (b < a) { a = b; } }
template <class T, class U> inline void chmax(T& a, U b) { if (a < b) { a = b; } }
template <class T> inline constexpr T square(T v) { return v * v; }

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


#define VALIDATE_ABORT()
#define VALIDATE_ASSERT(exp)


#define VABORT() VALIDATE_ABORT()
#define VASSERT(exp) VALIDATE_ASSERT(exp)






template <class A, class B>
struct pr {
	union {
		A a;
		A key;
		A first;
		A x;
	};
	union {
		B b;
		B value;
		B second;
		B y;
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
inline pdouble normalized(const pdouble& d) {
	return d / norm(d);
}
inline double dot(const pdouble& a, const pdouble& b) {
	return a.x * b.x + a.y * b.y;
}
inline double cross(const pdouble& a, const pdouble& b) {
	return a.x * b.y - a.y * b.x;
}

template <class T, int CAPACITY>
class CapacityArray {
	static_assert(is_trivially_copyable<T>::value);

private:
	array<T, CAPACITY> array_ = {};
	int count_ = 0;

public:
	CapacityArray() {
	}

	explicit CapacityArray(int count) {
		resize(count);
	}

	bool operator == (const CapacityArray<T, CAPACITY>& r) const {
		if (count_ != r.count_) {
			return false;
		}
		REP(i, count_) {
			if (!(array_[i] == r.array_[i])) {
				return false;
			}
		}
		return true;
	}
	bool operator != (const CapacityArray<T, CAPACITY>& r) const {
		return !(*this == r);
	}

	void CopyFrom(const CapacityArray<T, CAPACITY>& r) {
		memcpy(array_.data(), r.array_.data(), sizeof(T) * r.count_);
		count_ = r.count_;
	}

	constexpr int capacity() const {
		return CAPACITY;
	}

	inline void clear() {
		count_ = 0;
	}

	inline void resize(int count) {
		count_ = count;
	}

	inline void assign(int count, const T& e) {
		count_ = count;
		for (int i = 0; i < count; ++i) {
			array_[i] = e;
		}
	}

	const T* data() const {
		return array_.data();
	}
	T* data() {
		return array_.data();
	}

	inline T* PushBack() {
		return &array_[count_++];
	}

	inline void push_back(const T& e) {
		array_[count_++] = e;
	}
	inline void PushBack(const T& e) {
		push_back(e);
	}

	inline void pop_back() {
		--count_;
	}

	T& front() {
		return array_[0];
	}
	const T& front() const {
		return array_[0];
	}

	T& back() {
		return array_[count_ - 1];
	}
	const T& back() const {
		return array_[count_ - 1];
	}

	inline void RemoveSwap(int i) {
		array_[i] = array_[count_ - 1];
		--count_;
	}

	inline int size() const {
		return count_;
	}

	inline bool empty() const {
		return count_ == 0;
	}

	inline T& operator[](int index) {
		return array_[index];
	}

	inline const T& operator[](int index) const {
		return array_[index];
	}

	inline auto begin() -> decltype(array_.begin()) {
		return array_.begin();
	}

	inline auto end() -> decltype(array_.begin()) {

		return array_.begin() + count_;
	}

	inline auto begin() const -> decltype(array_.begin()) {
		return array_.begin();
	}

	inline auto end() const -> decltype(array_.begin()) {

		return array_.begin() + count_;
	}

	inline int Find(const T& value) const {
		REP(i, count_) {
			if (array_[i] == value) {
				return i;
			}
		}
		return -1;
	}
	inline bool IsContain(const T& value) const {
		for (const auto& v : *this) {
			if (v == value) {
				return true;
			}
		}
		return false;
	}

	inline void MemInsert(int index, const T* mem, int count) {
		if (index == count_) {
			memcpy(array_.data() + index, mem, sizeof(T) * count);
			count_ += count;
		}
		else {
			memmove(array_.data() + index + count, array_.data() + index, sizeof(T) * (count_ - index));
			memcpy(array_.data() + index, mem, sizeof(T) * count);
			count_ += count;
		}
	}

	void insert(int index, const T& value) {
		MemInsert(index, &value, 1);
	}

	inline void Insert(int start, int end, const T* p, int size) {
		int newEnd = start + size;
		if (count_ - end > 0 && newEnd != end) {
			memmove(array_.data() + newEnd, array_.data() + end, sizeof(T) * (count_ - end));
		}

		memcpy(array_.data() + start, p, sizeof(T) * size);

		count_ -= end - start;
		count_ += size;
	}

	inline void Remove(int start, int end) {
		int size = end - start;
		memmove(array_.data() + start, array_.data() + end, sizeof(T) * (count_ - end));
		count_ -= size;
	}


};

template <class T, int CAPACITY>
using CapArr = CapacityArray<T, CAPACITY>;



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

template <int CAPACITY>
struct CapacitySet {
private:
	CapacityArray<int, CAPACITY> elemens;
	CheckMapDataS<int, CAPACITY> indexTable;

public:
	CapacitySet() {
	}

	void Clear() {
		elemens.clear();
		indexTable.Clear();
	}

	inline void Add(int ai) {
		indexTable.Set(ai, elemens.size());
		elemens.PushBack(ai);
	}

	inline void ForceAdd(int ai) {
		if (indexTable.IsChecked(ai)) {
			return;
		}
		indexTable.Set(ai, elemens.GetCount());
		elemens.PushBack(ai);
	}

	inline void Remove(int ai) {
		int removeIndex = indexTable[ai];
		int lastIndex = elemens.GetCount() - 1;

		if (removeIndex != lastIndex) {
			elemens[removeIndex] = elemens[lastIndex];
			indexTable.Set(elemens[lastIndex], removeIndex);
		}
		elemens.PopBack();
		indexTable.Reset(ai);
	}

	inline void ForceRemove(int ai) {
		if (!indexTable.IsChecked(ai)) {
			return;
		}
		int removeIndex = indexTable[ai];
		int lastIndex = elemens.GetCount() - 1;

		if (removeIndex != lastIndex) {
			elemens[removeIndex] = elemens[lastIndex];
			indexTable.Set(elemens[lastIndex], removeIndex);
		}
		elemens.PopBack();
		indexTable.Reset(ai);
	}

	inline bool IsContain(int ai) const {
		return indexTable.IsChecked(ai);
	}

	inline int GetCount() const {
		return elemens.GetCount();
	}
	inline int size() const {
		return elemens.GetCount();
	}
	inline bool empty() const {
		return elemens.empty();
	}

	inline int At(int index) const {
		return elemens[index];
	}

	inline int operator[](int index) const {
		return elemens[index];
	}

	inline auto begin() -> decltype(elemens.begin()) {
		return elemens.begin();
	}
	inline auto end() -> decltype(elemens.begin()) {
		return elemens.end();
	}
	inline auto begin() const -> decltype(elemens.begin()) {
		return elemens.begin();
	}
	inline auto end() const -> decltype(elemens.begin()) {
		return elemens.end();
	}
};
template <int CAPACITY>
using CapSet = CapacitySet<CAPACITY>;



struct SizeSet {
private:
	vector<int> elemens;
	vector<int> indexTable;

public:
	void InitSize(int size) {
		elemens.clear();
		elemens.reserve(size);
		indexTable.assign(size, -1);
	}

	void Clear() {
		elemens.clear();
		indexTable.assign(indexTable.size(), -1);
	}

	inline void Add(int ai) {
		indexTable[ai] = (int)elemens.size();
		elemens.emplace_back(ai);
	}

	inline void ForceAdd(int ai) {
		if (indexTable[ai] >= 0) {
			return;
		}
		indexTable[ai] = (int)elemens.size();
		elemens.emplace_back(ai);
	}

	inline void Remove(int ai) {
		int removeIndex = indexTable[ai];
		int lastIndex = (int)elemens.size() - 1;

		if (removeIndex != lastIndex) {
			elemens[removeIndex] = elemens[lastIndex];
			indexTable[elemens[lastIndex]] = removeIndex;
		}
		elemens.pop_back();
		indexTable[ai] = -1;
	}

	inline void ForceRemove(int ai) {
		if (indexTable[ai] < 0) {
			return;
		}
		int removeIndex = indexTable[ai];
		int lastIndex = (int)elemens.size() - 1;

		if (removeIndex != lastIndex) {
			elemens[removeIndex] = elemens[lastIndex];
			indexTable[elemens[lastIndex]] = removeIndex;
		}
		elemens.pop_back();
		indexTable[ai] = -1;
	}

	inline bool IsContain(int ai) const {
		return indexTable[ai] >= 0;
	}

	inline int GetCount() const {
		return (int)elemens.size();
	}

	inline int At(int index) const {
		return elemens[index];
	}

	inline int operator[](int index) const {
		return elemens[index];
	}

	inline auto begin() -> decltype(elemens.begin()) {
		return elemens.begin();
	}
	inline auto end() -> decltype(elemens.begin()) {
		return elemens.end();
	}
	inline auto begin() const -> decltype(elemens.begin()) {
		return elemens.begin();
	}
	inline auto end() const -> decltype(elemens.begin()) {
		return elemens.end();
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

template <class IT, class RAND>
void Shuffle(IT&& begin, IT&& end, RAND&& rand) {
	int size = int(end - begin);
	if (size <= 1) {
		return;
	}
	REP(i, size - 1) {
		int j = i + rand() % (size - i);
		swap(*(begin + i), *(begin + j));
	}
}

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

	inline uint64_t operator()(uint64_t l, uint64_t r) {
		return ((*this)() % (r - l)) + l;
	}

	inline uint64_t operator()(uint64_t r) {
		return (*this)() % r;
	}
	inline double GetDouble() {
		return (*this)() / (double)UINT64_MAX;
	}
	inline bool GetProb(double E) {
		return (*this)() < ((double)UINT64_MAX + 1) * E;
	}
};



#define PARAM_CATEGORY(NAME, VALUE, VALUE_LIST) const char* NAME = VALUE;
#define PARAM_INT(NAME, VALUE, LOWER_VALUE, UPPER_VALUE) int NAME = VALUE;
#define PARAM_DOUBLE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE) double NAME = VALUE;


#define PARAM_INT_RANGE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, LOWER_LIMIT, UPPER_LIMIT) int NAME = VALUE;
#define PARAM_DOUBLE_RANGE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, LOWER_LIMIT, UPPER_LIMIT) double NAME = VALUE;


#define PARAM_LOWER(v)
#define PARAM_UPPER(v)
#define START_TUNING
#define END_TUNING




constexpr
struct {
	PARAM_INT(Trans2_Rate, 16, 0, 100);PARAM_LOWER(0);
	PARAM_INT(Trans1_Rate, 16, 0, 100);PARAM_LOWER(0);		

	PARAM_INT(VisRadius_Initial, 118, 80, 200);PARAM_LOWER(80);PARAM_UPPER(500);    
	PARAM_INT(VisRadius_Route, 98, 9, 300);PARAM_LOWER(9);PARAM_UPPER(500);
	PARAM_INT(VisRadius_Trans1, 207, 9, 300);PARAM_LOWER(9);PARAM_UPPER(500);
	PARAM_INT(VisRadius_Trans2, 191, 9, 300);PARAM_LOWER(9);PARAM_UPPER(500);


	PARAM_DOUBLE(DayWeight, 6.388281491864555, 0.01, 10.0);PARAM_LOWER(0.0);		

	PARAM_INT(VisSetCount, 2, 1, 3);PARAM_LOWER(1);PARAM_UPPER(4);

	START_TUNING;
	PARAM_INT(CutCount, 32, 24, 40);PARAM_LOWER(24);PARAM_UPPER(40);
	END_TUNING;

} HP;

constexpr int N_Min = 500;
constexpr int N_Max = 1000;

constexpr int M_Min = 500;
constexpr int M_Max = 3000;

constexpr int D_Min = 5;
constexpr int D_Max = 30;

constexpr int K_Min = ((M_Min + D_Max - 1) / D_Max) + 1;
constexpr int K_Max = 2 * M_Max / D_Min;

int N = 0;
int M = 0;
int D = 0;
int K = 0;




template<class T, class COMPARERE>
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


template <class T> using NArr = CapArr<T, N_Max>;
template <class T> using MArr = CapArr<T, M_Max>;
template <class T> using DArr = CapArr<T, D_Max>;
template <class T> using KArr = CapArr<T, K_Max>;

constexpr int INF = 1000000000;
constexpr s64 TOTAL_DIST_MAX = (s64)INF * N_Max * N_Max;		

constexpr pint Center = { 500, 500 };


using Result = MArr<s8>;

struct Edge {
	union {
		array<int, 2> vs;		
		struct {
			int a, b;
		};
	};
	int w;					
};
using Edges = MArr<Edge>;
using Points = NArr<pint>;

struct AroundEdge {
	int ei;		
	int vi;		
	int w;		
};
using AroundEdges = NArr<CapArr<AroundEdge, 32>>;		




void ComputeDist(const AroundEdges& aroundEdges, const Result& rs, int day, int s, NArr<int>& dist, NArr<int>& fromEis) {
	dist.assign(N, INF);
	fromEis.assign(N, -1);

	priority_queue<pint> que;
	que.push(pint{ 0, s });
	dist[s] = 0;
	fromEis[s] = -1;
	while (!que.empty()) {
		pint p = que.top();
		que.pop();

		int d = -p.a;
		int u = p.b;
		if (dist[u] != d) {
			continue;
		}
		for (cauto& ae : aroundEdges[u]) {
			if (rs[ae.ei] == day) {
				continue;
			}
			int d2 = d + ae.w;
			if (d2 < dist[ae.vi]) {
				dist[ae.vi] = d2;
				fromEis[ae.vi] = ae.ei;
				que.push(pint{ -d2, ae.vi });
			}
		}
	}
}

void ComputeDistMatrix(const AroundEdges& aroundEdges, const Result& rs, int day, NArr<NArr<int>>& distMat, NArr<NArr<int>>& fromEiMat) {
	distMat.resize(N);
	fromEiMat.resize(N);
	REP(s, N) {
		ComputeDist(aroundEdges, rs, day, s, distMat[s], fromEiMat[s]);
	}
}
void ComputeBaseDistMatrix(const AroundEdges& aroundEdges, NArr<NArr<int>>& distMat, NArr<NArr<int>>& fromEiMat) {
	Result rs;
	rs.assign(M, -1);

	distMat.resize(N);
	fromEiMat.resize(N);
	REP(s, N) {
		ComputeDist(aroundEdges, rs, 0, s, distMat[s], fromEiMat[s]);
	}
}

double ComputeScore(const AroundEdges& aroundEdges, const Result& rs, DArr<double>& daysScore) {
	DArr<int> counts;
	counts.assign(D, 0);
	REP(ei, M) {
		VASSERT(rs[ei] >= 0 && rs[ei] < D);
		++counts[rs[ei]];
	}
	REP(day, D) {
		VASSERT(counts[day] <= K);
	}

	s64 num = 0;
	static NArr<NArr<int>> dist0;
	static NArr<NArr<int>> fromEis;
	ComputeBaseDistMatrix(aroundEdges, dist0, fromEis);

	daysScore.clear();
	REP(d, D) {
		static NArr<NArr<int>> dist;
		ComputeDistMatrix(aroundEdges, rs, d, dist, fromEis);

		s64 tmp = 0;
		REP(i, N) {
			FOR(j, i + 1, N) {
				tmp += dist[i][j] - dist0[i][j];
			}
		}
		num += tmp;
		daysScore.push_back(tmp / (double)(N * (N - 1) / 2));
	}
	int den = D * N * (N - 1) / 2;
	double avg = num / (double)den * 1000.0;
	return avg;
}




struct IOServer {
	Edges edges_;
	Points points_;
	AroundEdges aroundEdges_;
	NArr<NArr<int>> baseDistmat_;
	NArr<NArr<int>> fromEiMat_;

	void InitInput(ChronoTimer& timer) {
		istream& is = cin;
		is >> N >> M >> D >> K;
		timer.Init();

		edges_.resize(M);
		REP(ei, M) {
			auto& e = edges_[ei];
			is >> e.vs[0] >> e.vs[1] >> e.w;
			--e.vs[0];
			--e.vs[1];
		}

		points_.resize(N);
		REP(vi, N) {
			auto& p = points_[vi];
			is >> p.x >> p.y;
		}

		aroundEdges_.resize(N);
		REP(ei, M) {
			cauto& e = edges_[ei];
			aroundEdges_[e.a].push_back(AroundEdge{ ei, e.b, e.w });
			aroundEdges_[e.b].push_back(AroundEdge{ ei, e.a, e.w });
		}
		ComputeBaseDistMatrix(aroundEdges_, baseDistmat_, fromEiMat_);


	}

	void Output(const Result& rs) {
		ostream& os = cout;
		VASSERT(rs.size() == M);
		os << (int)rs[0] + 1;
		FOR(ei, 1, M) {
			os << " " << (int)rs[ei] + 1;
		}
		os << endl;

	}


	void Finalize() {
	}
};
IOServer server;

#define USE_SA_POINT_FILTER 0
#define USE_SA_ROLLBACK 0



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




struct SAChecker {
	Xor64* rand_ = nullptr;
	double* totalMaxScore_ = nullptr;


	double currentScore = 0;
	double maxScore = 0;


	inline bool operator()(double newScore) {

		if (newScore > currentScore) {
			currentScore = newScore;
			if (newScore > maxScore) {
				maxScore = newScore;

				if (newScore > *totalMaxScore_) {
					*totalMaxScore_ = newScore;
				}
			}

			return true;
		}

		else if (newScore == currentScore) {
			return true;
		}

		return false;
	}

};

template <class F>
struct SATransition {
	const char* name;
	F func;
	int weight;
};
template <class F>
auto MakeTransition(const char* name, F&& func, int weight) {
	return SATransition<F>{ name, func, weight };
}
#define MAKE_TRANS(func, weight) MakeTransition(#func, [&](SAChecker& sac, State& state) { func(sa, sac, state); }, weight)

struct SimulatedAnnealing {
	SAChecker checker;

	double totalMaxScore = 0;				
	double timeRate = 0;				


	Xor64 rand_;

	int stepLoopCount = 1000;					



public:
	template <class STATE, class... TRANSITION>
	void Run2(ChronoTimer& timer, STATE& state, tuple<SATransition<TRANSITION>...>& transitions) {

		totalMaxScore = state.EvalScore();
		{
			checker.rand_ = &rand_;
			checker.totalMaxScore_ = &totalMaxScore;

			checker.currentScore = state.EvalScore();
			checker.maxScore = checker.currentScore;

			chmax(totalMaxScore, checker.maxScore);
		}

		RandomTable randTable;
		TupleLoop(transitions, [&](auto&& e, size_t i) {
			randTable.push((int)i, e.weight);
		});

		const auto startTime = timer.Now();
		const auto endTime = timer.EndTime();
		const double subTimeCountDiv = 1.0 / (double)(endTime - startTime).count();


		bool forceEnd = false;
		while (!timer.IsOver()) {
			timeRate = (timer.Now() - startTime).count() * subTimeCountDiv;


			for (int rp = 0; rp < stepLoopCount; ++rp) {
				int ti = (int)randTable(rand_);

				auto exec = [&](auto&& e, size_t i) {
						e.func(checker, state);
				};

				TupleAccess(transitions, ti, exec);
			}
			if (forceEnd) {
				break;
			}
		}

	}

	void ForceUpdate() {
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

#define USE_UPDATE2_CUT 1
#define USE_DUMMY_BACKUP 1
#define USE_SKIP_VIS 1

#define USE_STABLE 1


#define SKIP_CHECK 0


template <int CAPACITY>
struct RandomChoice {
private:
	CapArr<int, CAPACITY> table_;
	int n_ = 0;
	int choicedCount_ = 0;
	bool autoReset_ = false;

public:
	void Init(int n, bool autoReset) {
		table_.resize(n);
		iota(table_.begin(), table_.end(), 0);
		n_ = n;
		choicedCount_ = 0;
		autoReset_ = autoReset;
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
};

#define USE_BASE_DAY 0

struct DistQueComparer {
	bool operator()(const pint& a, const pint& b) {
		return a.first > b.first;
	}
};
using DistQue = PriorityQueue<pint, DistQueComparer>;

struct VisSet {
	NArr<int> testVis;
	NArr<int> otherVis;
};

struct VertexData {
	s64 totalDist_;					
	NArr<int> dists_;				
	NArr<int> fromEis_;				
	int constructCount_;			

	bool operator == (const VertexData& r) const {
		if (totalDist_ != r.totalDist_) {
			return false;
		}
		if (dists_ != r.dists_) {
			return false;
		}
		if (fromEis_ != r.fromEis_) {
			return false;
		}
		if (constructCount_ != r.constructCount_) {
			return false;
		}
		return true;
	}

	void Init() {
		totalDist_ = 0;
		dists_.assign(N, -1);
		fromEis_.assign(N, -1);
		constructCount_ = 0;
	}
};

struct DayData {
	int constructCount_;					
	s64 totalDist_;							
	NArr<VertexData> vertexsData_;			

	bool operator == (const DayData& r) const {
		if (constructCount_ != r.constructCount_) {
			return false;
		}
		if (vertexsData_ != r.vertexsData_) {
			return false;
		}
		if (totalDist_ != r.totalDist_) {
			return false;
		}
		return true;
	}

	void Init() {
		constructCount_ = 0;
		totalDist_ = 0;
		vertexsData_.resize(N);
		REP(vi, N) {
			vertexsData_[vi].Init();
		}
	}
};

struct Backup {
	Backup() = default;
	Backup(const Backup&) = delete;

	struct EiDay {
		int ei;
		int day;
	};

	struct KV {
		int key;
		int dist;
		int fromEis;
	};
	struct DistFrom {
		int day;
		int vi0;
		int vi1;
		int dist;
		int fromEis;
	};

	static constexpr int DFKeySize = D_Max * 1024 * 1024;		

	CheckMapD rsCheckd;
	MArr<pint> rsKvs;
	CheckMapD dfChecked;
	vector<KV> dfKvs;

	void Init() {
		rsCheckd.Init(M);
		rsKvs.clear();
		dfChecked.Init(DFKeySize);
		dfKvs.clear();
		dfKvs.reserve(DFKeySize);
	}

	void Clear() {
		rsCheckd.Clear();
		rsKvs.clear();
		dfChecked.Clear();
		dfKvs.clear();
	}

	void RegistRS(int ei, int day) {
		if (rsCheckd[ei]) {
			return;
		}
		rsCheckd.Check(ei);
		rsKvs.push_back(pint{ ei, day });
	}
	void RegistDF(int day, int vi0, int vi1, int dist, int fromEi) {
		int key = (day << 20) | (vi0 << 10) | vi1;
		if (dfChecked[key]) {
			return;
		}
		dfChecked.Check(key);
		dfKvs.emplace_back(KV{ key, dist, fromEi });
	}
	int RSCount() const {
		return (int)rsKvs.size();
	}
	int DFCount() const {
		return (int)dfKvs.size();
	}
	void GetRS(int ki, EiDay& ed) const {
		cauto& kv = rsKvs[ki];
		ed.ei = kv.a;
		ed.day = kv.b;
	}
	void GetDF(int ki, DistFrom& df) const {
		cauto& kv = dfKvs[ki];
		df.day = (kv.key >> 20);
		df.vi0 = ((kv.key >> 10) & 1023);
		df.vi1 = (kv.key & 1023);
		df.dist = kv.dist;
		df.fromEis = kv.fromEis;
	}
};
static constexpr int BackupSize = sizeof(Backup);
static constexpr int BackupSizeM = BackupSize / 1024 / 1024;

struct DummyBackup {
	DummyBackup() = default;
	DummyBackup(const DummyBackup&) = delete;

	void RegistRS(int ei, int day) {
	}
	void RegistDF(int day, int vi0, int vi1, int dist, int fromEi) {
	}
} dummyBackup_;


DistQue commonDistQue_;

struct Aggregate {
	Aggregate() = default;
	Aggregate(const Aggregate&) = delete;

	Result rs_;
	int constructCount_;				
	s64 baseTotalDist_;					
	s64 totalDist_;						
	DArr<DayData> daysData_;			

	bool operator == (const Aggregate& r) const {
		if (rs_ != r.rs_) {
			REP(ei, M) {
				if (rs_[ei] != r.rs_[ei]) {
					return false;
				}
			}
			return false;
		}
		if (baseTotalDist_ != r.baseTotalDist_) {
			return false;
		}
		if (daysData_ != r.daysData_) {
			return false;
		}
		if (totalDist_ != r.totalDist_) {
			return false;
		}
		return true;
	}

	double GetScore() const {
		return CalcScore(baseTotalDist_, totalDist_);
	}

	void Init(const NArr<NArr<int>>& baseDistmat, const NArr<NArr<int>>& baseFromEiMat) {
		constructCount_ = 0;
		baseTotalDist_ = 0;
		totalDist_ = 0;
		daysData_.resize(D);
		REP(day, D) {
			daysData_[day].Init();
		}

		REP(i, N) {
			FOR(j, i + 1, N) {
				baseTotalDist_ += baseDistmat[i][j];
			}
		}
		baseTotalDist_ *= 2;

		{
			rs_.assign(M, -1);

			{
				int day = 0;
				auto& dd = daysData_[day];
				REP(vi, N) {
					auto& vd = dd.vertexsData_[vi];
					vd.dists_ = baseDistmat[vi];
					vd.fromEis_ = baseFromEiMat[vi];
					vd.totalDist_ = CalcVertexDist(vd.dists_);
					dd.totalDist_ += vd.totalDist_;
				}
				totalDist_ += dd.totalDist_;
			}
			FOR(day, 1, D) {
				auto& dd = daysData_[day];
				dd = daysData_[0];
				totalDist_ += dd.totalDist_;
			}
		}
	}

	void InitOld(const NArr<NArr<int>>& baseDistmat, const NArr<NArr<int>>& baseFromEiMat, const Edges& edges, const AroundEdges& aroundEdges, const Result& rs) {
		constructCount_ = 0;
		baseTotalDist_ = 0;
		totalDist_ = 0;
		daysData_.resize(D);
		REP(day, D) {
			daysData_[day].Init();
		}

		REP(i, N) {
			FOR(j, i + 1, N) {
				baseTotalDist_ += baseDistmat[i][j];
			}
		}
		baseTotalDist_ *= 2;

		if (rs.empty()) {
			rs_.assign(M, -1);

			{
				int day = 0;
				auto& dd = daysData_[day];
				REP(vi, N) {
					auto& vd = dd.vertexsData_[vi];
					vd.dists_ = baseDistmat[vi];
					vd.fromEis_ = baseFromEiMat[vi];
					vd.totalDist_ = CalcVertexDist(vd.dists_);
					dd.totalDist_ += vd.totalDist_;
				}
				totalDist_ += dd.totalDist_;
			}
			FOR(day, 1, D) {
				auto& dd = daysData_[day];
				dd = daysData_[0];
				totalDist_ += dd.totalDist_;
			}
		}
		else {
			rs_ = rs;

			REP(day, D) {
				auto& dd = daysData_[day];
				REP(vi, N) {
					auto& vd = dd.vertexsData_[vi];
					commonDistQue_.Clear();
					ComputeDist(aroundEdges, rs_, day, vi, vd.dists_, vd.fromEis_, commonDistQue_);
					vd.totalDist_ = CalcVertexDist(vd.dists_);
					dd.totalDist_ += vd.totalDist_;
				}
				totalDist_ += dd.totalDist_;
			}

			REP(ei, M) {
				int day = rs_[ei];
				if (day >= 0) {
					++constructCount_;

					auto& dd = daysData_[day];
					++dd.constructCount_;

					cauto& e = edges[ei];
					++dd.vertexsData_[e.a].constructCount_;
					++dd.vertexsData_[e.b].constructCount_;
				}
			}
		}

	}


	bool CalcConstructTotalDist(const Edges& edges, const AroundEdges& aroundEdges, int day, int ei, const NArr<int>& simpleVis, s64 bestTotalDist, s64& resultTotalDist) const {
		VASSERT(rs_[ei] == -1);
		VASSERT(daysData_[day].constructCount_ < K);

		auto& mrs = const_cast<Result&>(rs_);		
		mrs[ei] = day;

		resultTotalDist = totalDist_;
		cauto& edge = edges[ei];
		auto& vertexsData = daysData_[day].vertexsData_;
		for (int vi : simpleVis) {
			cauto& vd = vertexsData[vi];
			if (vd.fromEis_[edge.a] != ei && vd.fromEis_[edge.b] != ei) {
				continue;
			}

			int farVi = edge.a;
			if (vd.fromEis_[edge.b] == ei) {
				farVi = edge.b;
			}
			auto& mvd = const_cast<VertexData&>(vd);
			commonDistQue_.Clear();
			UpdateRouteRevert(aroundEdges, mrs, day, farVi, mvd.dists_, mvd.fromEis_, resultTotalDist, commonDistQue_);

			if (resultTotalDist >= bestTotalDist) {
				mrs[ei] = -1;
				return false;
			}
		}
		mrs[ei] = -1;
		return true;
	}

	template <class BACKUP>
	void ConstructSimple(const Edges& edges, const AroundEdges& aroundEdges, int day, int ei, const NArr<int>& simpleVis, BACKUP& backup) {
		VASSERT(rs_[ei] == -1);
		VASSERT(daysData_[day].constructCount_ < K);

		backup.RegistRS(ei, rs_[ei]);
		rs_[ei] = day;

		auto& dd = daysData_[day];
		cauto& edge = edges[ei];

		++constructCount_;
		++dd.constructCount_;
		++dd.vertexsData_[edge.a].constructCount_;
		++dd.vertexsData_[edge.b].constructCount_;

		auto& vertexsData = dd.vertexsData_;
		for (int vi : simpleVis) {
			auto& vd = vertexsData[vi];
			if (vd.fromEis_[edge.a] != ei && vd.fromEis_[edge.b] != ei) {
				continue;
			}
			int farVi = edge.a;
			if (vd.fromEis_[edge.b] == ei) {
				farVi = edge.b;
			}
			s64 appendDist = 0;
			commonDistQue_.Clear();
			UpdateRoute(aroundEdges, rs_, day, vi, farVi, vd.dists_, vd.fromEis_, appendDist, commonDistQue_, backup);

			vd.totalDist_ += appendDist;
			dd.totalDist_ += appendDist;
			totalDist_ += appendDist;
		}
	}

	void CancelSimple(const Edges& edges, const AroundEdges& aroundEdges, const MArr<int>& cancelEis, const NArr<int>& simpleVis, Backup& backup) {
		static DArr<MArr<int>> daysCancelEis;
		daysCancelEis.resize(D);
		REP(day, D) {
			daysCancelEis[day].clear();
		}
		for (int ei : cancelEis) {
			int day = rs_[ei];
			if (day >= 0) {
				daysCancelEis[day].push_back(ei);
			}
		}

		for (int ei : cancelEis) {
			backup.RegistRS(ei, rs_[ei]);

			int day = rs_[ei];
			if (day >= 0) {
				rs_[ei] = -1;

				--constructCount_;

				auto& dd = daysData_[day];
				--dd.constructCount_;

				cauto& edge = edges[ei];
				--dd.vertexsData_[edge.a].constructCount_;
				--dd.vertexsData_[edge.b].constructCount_;
			}
		}

		REP(day, D) {
			auto& dd = daysData_[day];
			auto& vertexsData = dd.vertexsData_;

			for (int vi : simpleVis) {
				auto& vd = vertexsData[vi];
				auto& dist = vd.dists_;

				commonDistQue_.Clear();
				s64 appendDist = 0;
				for (int ei : daysCancelEis[day]) {
					cauto& edge = edges[ei];
					int diff = abs(dist[edge.a] - dist[edge.b]);
					if (diff <= edge.w) {
						continue;
					}

					int farVi = edge.a;
					if (dist[edge.a] < dist[edge.b]) {
						farVi = edge.b;
					}
					AddStartForCancel(aroundEdges, rs_, day, vi, farVi, vd.dists_, vd.fromEis_, commonDistQue_, appendDist, backup);
				}

				ComputeDistQue(aroundEdges, rs_, day, vi, vd.dists_, vd.fromEis_, commonDistQue_, appendDist, backup);

				vd.totalDist_ += appendDist;
				dd.totalDist_ += appendDist;
				totalDist_ += appendDist;
			}
		}
	}

	template <class BACKUP>
	void CancelConstruct(const Edges& edges, const AroundEdges& aroundEdges, const MArr<int>& cancelEis, const MArr<s8>& newDays, const NArr<int>& simpleVis, BACKUP& backup) {
		static DArr<MArr<int>> daysCancelEis;
		static DArr<MArr<int>> daysConstructEis;
		daysCancelEis.resize(D);
		daysConstructEis.resize(D);
		REP(day, D) {
			daysCancelEis[day].clear();
			daysConstructEis[day].clear();
		}
		REP(ci, cancelEis.size()) {
			int ei = cancelEis[ci];
			int oldDay = rs_[ei];
			int newDay = newDays[ci];
			if (oldDay == newDay) {
				continue;
			}
			if (oldDay >= 0) {
				daysCancelEis[oldDay].push_back(ei);
			}
			daysConstructEis[newDay].push_back(ei);

			backup.RegistRS(ei, rs_[ei]);
			rs_[ei] = newDay;
			if (oldDay >= 0) {
				--constructCount_;
				auto& dd = daysData_[oldDay];
				--dd.constructCount_;
				cauto& edge = edges[ei];
				--dd.vertexsData_[edge.a].constructCount_;
				--dd.vertexsData_[edge.b].constructCount_;
			}
			{
				++constructCount_;
				auto& dd = daysData_[newDay];
				++dd.constructCount_;
				cauto& edge = edges[ei];
				++dd.vertexsData_[edge.a].constructCount_;
				++dd.vertexsData_[edge.b].constructCount_;
			}
		}

		REP(day, D) {
			auto& dd = daysData_[day];
			auto& vertexsData = dd.vertexsData_;

			for (int vi : simpleVis) {
				auto& vd = vertexsData[vi];
				auto& dist = vd.dists_;
				cauto& cancelEis = daysCancelEis[day];
				cauto& constructEis = daysConstructEis[day];

				s64 appendDist = 0;
				commonDistQue_.Clear();
				for (int ei : cancelEis) {
					cauto& edge = edges[ei];
					int diff = abs(dist[edge.a] - dist[edge.b]);
					if (diff <= edge.w) {
						continue;
					}
					int farVi = edge.a;
					if (dist[edge.a] < dist[edge.b]) {
						farVi = edge.b;
					}
					AddStartForCancel(aroundEdges, rs_, day, vi, farVi, vd.dists_, vd.fromEis_, commonDistQue_, appendDist, backup);
				}
				for (int ei : constructEis) {
					cauto& edge = edges[ei];
					if (vd.fromEis_[edge.a] != ei && vd.fromEis_[edge.b] != ei) {
						continue;
					}
					int farVi = edge.a;
					if (vd.fromEis_[edge.b] == ei) {
						farVi = edge.b;
					}
					AddStartForConstruct2(aroundEdges, rs_, day, vi, farVi, vd.dists_, vd.fromEis_, appendDist, commonDistQue_, backup);
				}

				ComputeDistQue(aroundEdges, rs_, day, vi, vd.dists_, vd.fromEis_, commonDistQue_, appendDist, backup);

				vd.totalDist_ += appendDist;
				dd.totalDist_ += appendDist;
				totalDist_ += appendDist;
			}
		}
	}

	template <class BACKUP>
	void CancelConstructDiff(const Edges& edges, const AroundEdges& aroundEdges, const MArr<int>& cancelEis, const MArr<s8>& oldDays, const MArr<s8>& newDays, const NArr<int>& simpleVis, BACKUP& backup) {
		VASSERT(cancelEis.size() == oldDays.size());
		VASSERT(cancelEis.size() == newDays.size());

		static DArr<MArr<int>> daysCancelEis;
		static DArr<MArr<int>> daysConstructEis;
		daysCancelEis.resize(D);
		daysConstructEis.resize(D);
		REP(day, D) {
			daysCancelEis[day].clear();
			daysConstructEis[day].clear();
		}
		REP(ci, cancelEis.size()) {
			int ei = cancelEis[ci];
			int oldDay = oldDays[ci];
			int newDay = newDays[ci];
			if (oldDay == newDay) {
				continue;
			}
			if (oldDay >= 0) {
				daysCancelEis[oldDay].push_back(ei);
			}
			VASSERT(newDay >= 0);
			daysConstructEis[newDay].push_back(ei);

		}

		REP(day, D) {
			auto& dd = daysData_[day];
			auto& vertexsData = dd.vertexsData_;

			for (int vi : simpleVis) {
				auto& vd = vertexsData[vi];
				auto& dist = vd.dists_;
				cauto& cancelEis = daysCancelEis[day];
				cauto& constructEis = daysConstructEis[day];

				s64 appendDist = 0;
				commonDistQue_.Clear();
				for (int ei : cancelEis) {
					cauto& edge = edges[ei];
					int diff = abs(dist[edge.a] - dist[edge.b]);
					if (diff <= edge.w) {
						continue;
					}
					int farVi = edge.a;
					if (dist[edge.a] < dist[edge.b]) {
						farVi = edge.b;
					}
					AddStartForCancel(aroundEdges, rs_, day, vi, farVi, vd.dists_, vd.fromEis_, commonDistQue_, appendDist, backup);
				}
				for (int ei : constructEis) {
					cauto& edge = edges[ei];
					if (vd.fromEis_[edge.a] != ei && vd.fromEis_[edge.b] != ei) {
						continue;
					}
					int farVi = edge.a;
					if (vd.fromEis_[edge.b] == ei) {
						farVi = edge.b;
					}
					AddStartForConstruct2(aroundEdges, rs_, day, vi, farVi, vd.dists_, vd.fromEis_, appendDist, commonDistQue_, backup);
				}

				ComputeDistQue(aroundEdges, rs_, day, vi, vd.dists_, vd.fromEis_, commonDistQue_, appendDist, backup);

				vd.totalDist_ += appendDist;
				dd.totalDist_ += appendDist;
				totalDist_ += appendDist;
			}
		}
	}


	void Restore(const Edges& edges, const Backup& backup) {
		Backup::EiDay ed;
		REP(ki, backup.RSCount()) {
			backup.GetRS(ki, ed);
			int ei = ed.ei;
			if (rs_[ei] == ed.day) {
				continue;
			}
			{
				int day = rs_[ei];
				if (day >= 0) {
					--constructCount_;

					auto& dd = daysData_[day];
					--dd.constructCount_;

					cauto& edge = edges[ei];
					--dd.vertexsData_[edge.a].constructCount_;
					--dd.vertexsData_[edge.b].constructCount_;
				}
			}
			{
				int day = ed.day;
				if (day >= 0) {
					++constructCount_;

					auto& dd = daysData_[day];
					++dd.constructCount_;

					cauto& edge = edges[ei];
					++dd.vertexsData_[edge.a].constructCount_;
					++dd.vertexsData_[edge.b].constructCount_;
				}
			}
			rs_[ei] = ed.day;
		}

		Backup::DistFrom df;
		REP(ki, backup.DFCount()) {
			backup.GetDF(ki, df);
			auto& dd = daysData_[df.day];
			auto& vd = dd.vertexsData_[df.vi0];
			s64 diff = df.dist - vd.dists_[df.vi1];
			totalDist_ += diff;
			dd.totalDist_ += diff;
			vd.totalDist_ += diff;
			vd.dists_[df.vi1] = df.dist;
			vd.fromEis_[df.vi1] = df.fromEis;
		}
	}

private:
	static double CalcScore(s64 baseTotalDist, s64 totalDist) {
		int den = D * N * (N - 1);
		double score = (totalDist - baseTotalDist * D) / (double)den * 1000.0;
		return score;
	}

	static void ComputeDistQue(const AroundEdges& aroundEdges, const Result& rs, int day, NArr<int>& dists, NArr<int>& fromEis, DistQue& que) {
		while (!que.empty()) {
			pint p = que.top();
			que.pop();

			int d = p.a;
			int u = p.b;
			if (dists[u] != d) {
				continue;
			}
			for (cauto& ae : aroundEdges[u]) {
				if (rs[ae.ei] == day) {
					continue;
				}
				int d2 = d + ae.w;
				if (d2 < dists[ae.vi]) {
					dists[ae.vi] = d2;
					fromEis[ae.vi] = ae.ei;
					que.push(pint{ d2, ae.vi });
				}
			}
		}
	}

	template <class BACKUP>
	static void ComputeDistQue(const AroundEdges& aroundEdges, const Result& rs, int day, int vi0, NArr<int>& dists, NArr<int>& fromEis, DistQue& que, s64& appendDist, BACKUP& backup) {
		while (!que.empty()) {
			pint p = que.top();
			que.pop();

			int d = p.a;
			int u = p.b;
			if (dists[u] != d) {
				continue;
			}
			for (cauto& ae : aroundEdges[u]) {
				if (rs[ae.ei] == day) {
					continue;
				}
				int d2 = d + ae.w;
				if (d2 < dists[ae.vi]) {
					backup.RegistDF(day, vi0, ae.vi, dists[ae.vi], fromEis[ae.vi]);
					appendDist += d2 - dists[ae.vi];
					dists[ae.vi] = d2;
					fromEis[ae.vi] = ae.ei;
					que.push(pint{ d2, ae.vi });
				}
			}
		}
	}

	static void ComputeDist(const AroundEdges& aroundEdges, const Result& rs, int day, int vi, NArr<int>& dist, NArr<int>& fromEis, DistQue& que) {
		dist.assign(N, INF);
		fromEis.assign(N, -1);

		que.push(pint{ 0, vi });
		dist[vi] = 0;
		fromEis[vi] = -1;
		ComputeDistQue(aroundEdges, rs, day, dist, fromEis, que);
	}

	static s64 CalcVertexDist(const NArr<int>& dist) {
		s64 d = 0;
		REP(j, N) {
			d += dist[j];
		}
		return d;
	}

	static void UpdateRouteRevert(const AroundEdges& aroundEdges, const Result& rs, s8 day, int startVi, NArr<int>& dists, NArr<int>& fromEis, s64& totalDist, DistQue& priorityQue) {
		static NArr<int> beforeDists;
		static NArr<int> beforeFromEis;
		static CheckMapS<N_Max> checked;
		beforeDists.clear();
		beforeFromEis.clear();
		checked.Clear();

		static CapQue<int, N_Max> que;
		que.clear();

		{
			auto Add = [&](int vi) {
				que.push(vi);
				checked.Check(vi);
			};
			Add(startVi);

			while (!que.empty()) {
				int vi = que.pop();
				for (cauto& ae : aroundEdges[vi]) {
					if (rs[ae.ei] == day) {
						continue;
					}
					if (fromEis[ae.vi] == ae.ei) {
						Add(ae.vi);
					}
				}
			}
		}

		beforeDists.resize(que.end_size());
		beforeFromEis.resize(que.end_size());

		REP(qi, que.end_size()) {
			int vi = que[qi];

			totalDist -= dists[vi];
			beforeDists[qi] = dists[vi];
			beforeFromEis[qi] = fromEis[vi];
			dists[vi] = INF;
			fromEis[vi] = -1;

			int fromVi = -1;
			int fromEi = -1;
			int neadDist = INF;
			int wantDist = 0;
			for (cauto& ae : aroundEdges[vi]) {
				if (rs[ae.ei] == day) {
					continue;
				}
				if (checked[ae.vi]) {
					continue;
				}
				int d = dists[ae.vi] + ae.w;
				if (d < neadDist) {
					neadDist = d;
					fromVi = ae.vi;
					fromEi = ae.ei;
				}
				chmax(wantDist, dists[ae.vi] - ae.w);
			}
			if (fromVi >= 0) {
				dists[vi] = neadDist;
				fromEis[vi] = fromEi;
				if (neadDist < wantDist) {
					priorityQue.push(pint{ neadDist, vi });
				}
			}
			checked.Reset(vi);
		}

		ComputeDistQue(aroundEdges, rs, day, dists, fromEis, priorityQue);

		REP(qi, que.end_size()) {
			int vi = que[qi];
			totalDist += dists[vi];
			dists[vi] = beforeDists[qi];
			fromEis[vi] = beforeFromEis[qi];
		}
	}












	template <class BACKUP>
	static void UpdateRoute(const AroundEdges& aroundEdges, const Result& rs, int day, int vi0, int startVi, NArr<int>& dists, NArr<int>& fromEis, s64& totalDist, DistQue& priorityQue, BACKUP& backup) {
		AddStartForConstruct2(aroundEdges, rs, day, vi0, startVi, dists, fromEis, totalDist, priorityQue, backup);
		ComputeDistQue(aroundEdges, rs, day, vi0, dists, fromEis, priorityQue, totalDist, backup);
	}

	template <class BACKUP>
	static void AddStartForConstruct2(const AroundEdges& aroundEdges, const Result& rs, s8 day, int vi0, int startVi, NArr<int>& dists, NArr<int>& fromEis, s64& totalDist, DistQue& priorityQue, BACKUP& backup) {
		static CheckMapS<N_Max> checked;
		checked.Clear();
		static CapQue<int, N_Max> que;
		que.clear();

		{
			auto Add = [&](int vi) {
				checked.Check(vi);
				que.push(vi);
			};

			Add(startVi);

			while (!que.empty()) {
				int vi = que.pop();

				for (cauto& ae : aroundEdges[vi]) {
					if (rs[ae.ei] == day) {
						continue;
					}
					if (fromEis[ae.vi] == ae.ei) {
						Add(ae.vi);
					}
				}
			}
		}

		REP(qi, que.end_size()) {
			int vi = que[qi];

			backup.RegistDF(day, vi0, vi, dists[vi], fromEis[vi]);
			totalDist += INF - dists[vi];
			dists[vi] = INF;
			fromEis[vi] = -1;

			int fromVi = -1;
			int fromEi = -1;
			int neadDist = INF;
			int wantDist = 0;
			for (cauto& ae : aroundEdges[vi]) {
				if (rs[ae.ei] == day) {
					continue;
				}
				if (checked[ae.vi]) {
					continue;
				}
				int d = dists[ae.vi] + ae.w;
				if (d < neadDist) {
					neadDist = d;
					fromVi = ae.vi;
					fromEi = ae.ei;
				}
				chmax(wantDist, dists[ae.vi] - ae.w);
			}
			if (fromVi >= 0) {
				totalDist += neadDist - dists[vi];
				dists[vi] = neadDist;
				fromEis[vi] = fromEi;
				if (neadDist < wantDist) {
					priorityQue.push(pint{ neadDist, vi });
				}
			}
			checked.Reset(vi);
		}
	}









	template <class BACKUP>
	static void AddStartForCancel(const AroundEdges& aroundEdges, const Result& rs, int day, int vi0, int vi1, NArr<int>& dists, NArr<int>& fromEis, DistQue& priorityQue, s64& appendDist, BACKUP& backup) {
		auto& que = priorityQue;

		int fromVi = -1;
		int fromEi = -1;
		int neadDist = INF;
		for (cauto& ae : aroundEdges[vi1]) {
			if (rs[ae.ei] == day) {
				continue;
			}
			if (dists[ae.vi] == INF) {
				continue;
			}
			int d = dists[ae.vi] + ae.w;
			if (d < neadDist) {
				neadDist = d;
				fromVi = ae.vi;
				fromEi = ae.ei;
			}
		}
		if (fromVi >= 0) {
			backup.RegistDF(day, vi0, vi1, dists[vi1], fromEis[vi1]);
			appendDist += neadDist - dists[vi1];
			dists[vi1] = neadDist;
			fromEis[vi1] = fromEi;
			que.push(pint{ neadDist, vi1 });
		}
	}

public:
	static void CalcSimpleVis(int baseRadius, VisSet& visSet) {
		constexpr pint center = { 500, 500 };

		int baseRadius2 = square(baseRadius);
		visSet.testVis.clear();
		visSet.otherVis.clear();

		cauto& points = server.points_;
		NArr<int> radiuss(N);
		REP(vi, N) {
			radiuss[vi] = norm2(points[vi] - center);
		}
		NArr<int> order(N);
		iota(ALL(order), 0);
		stable_sort(ALL(order), [&](int a, int b) { return radiuss[a] < radiuss[b]; });
		for (int vi : order) {
			bool fail = false;
			for (int svi : visSet.testVis) {
				int d = norm2(points[vi] - points[svi]);
				if (d < baseRadius2) {
					fail = true;
					break;
				}
			}
			if (!fail) {
				visSet.testVis.push_back(vi);
			}
			else {
				visSet.otherVis.push_back(vi);
			}
		}
	}

	static void MakeRadiuses(const NArr<int>& allVis, NArr<pint>& radiuses) {
		cauto& points = server.points_;
		radiuses.clear();
		for (int vi : allVis) {
			radiuses.push_back(pint{ norm2(points[vi] - Center), vi });
		}
		stable_sort(ALL(radiuses));
	}

	static void MakeSimpleVis(const NArr<pint>& radiuses, int baseRadius, VisSet& visSet) {
		int baseRadius2 = square(baseRadius);
		visSet.testVis.clear();
		visSet.otherVis.clear();

		cauto& points = server.points_;
		for (auto&& [r, vi] : radiuses) {
			bool fail = false;
			for (int svi : visSet.testVis) {
				int d = norm2(points[vi] - points[svi]);
				if (d < baseRadius2) {
					fail = true;
					break;
				}
			}
			if (!fail) {
				visSet.testVis.push_back(vi);
			}
			else {
				visSet.otherVis.push_back(vi);
			}
		}
	}

	static void CalcSimpleVis(const NArr<int>& allVis, int baseRadius, VisSet& visSet) {
		int baseRadius2 = square(baseRadius);
		visSet.testVis.clear();
		visSet.otherVis.clear();

		cauto& points = server.points_;
		static NArr<pint> radiuss;
		radiuss.clear();
		for (int vi : allVis) {
			radiuss.push_back(pint{ norm2(points[vi] - Center), vi });
		}
		stable_sort(ALL(radiuss));
		for (auto&& [r, vi] : radiuss) {
			bool fail = false;
			for (int svi : visSet.testVis) {
				int d = norm2(points[vi] - points[svi]);
				if (d < baseRadius2) {
					fail = true;
					break;
				}
			}
			if (!fail) {
				visSet.testVis.push_back(vi);
			}
			else {
				visSet.otherVis.push_back(vi);
			}
		}
	}


	static void CalcPolarVis(const NArr<int>& allVis, int dirCount, bool shift, bool inner, bool innerHalf, VisSet& visSet) {
		cauto& points = server.points_;

		vector<NArr<pr<int, int>>> divs(dirCount);		
		for (int vi : allVis) {
			pint diff = points[vi] - Center;
			double deg = atan2(diff.y, diff.x) * 180.0 / M_PI;
			if (deg < 0) {
				deg += 360;
			}
			int dir = (int)floor(deg / (360.0 / (double)dirCount));
			chmin(dir, dirCount - 1);
			divs[dir].push_back(pr<int, int>{norm2(diff), vi});
		}
		REP(dir, dirCount) {
			stable_sort(ALL(divs[dir]), [&](const pint& a, const pint& b) {
				return a.x < b.x;
				});
		}

		NArr<bool> used;
		used.assign(N, false);
		REP(i, dirCount) {
			cauto& ds = divs[i];
			if (ds.empty()) {
				continue;
			}
			int size = ds.size();
			if (i % 4 == (shift ? 2 : 0)) {
				int vi = ds.back().b;
				used[vi] = true;
				visSet.testVis.push_back(vi);		
			}
			else if (inner && i % (innerHalf ? 8 : 4) == (shift ? 0 : 2)) {
				int vi = ds[size / 5].b;
				used[vi] = true;
				visSet.testVis.push_back(vi);
			}
		}

		for (int vi : allVis) {
			if (used[vi]) {
			}
			else {
				visSet.otherVis.push_back(vi);
			}
		}
	}
};
constexpr int AggregateSize = sizeof(Aggregate);
constexpr int AggregateSizeM = AggregateSize / 1024 / 1024;

struct State {
	Aggregate agg;
	int rawScore;
	double score;

	vector<VisSet> trans1VisSets_;
	vector<VisSet> trans2VisSets_;


	double EvalScore() const {
		return score;
	}
	int RawScore() const {
		return rawScore;
	}

	void SetScore(double score_) {
		score = -score_;
		rawScore = -(int)round(score_);
	}
};
static constexpr int StateSize = sizeof(State);

struct Solver {
	Xor64 rand_;
	NArr<int> allVis_;
	vector<NArr<int>> cutViss_;		
	vector<NArr<int>> totalViss_;		
	VisSet initialVisSet_;
	VisSet routeVisSet_;

	RandomChoice<N_Max> vertexRandChoise_;
	RandomChoice<M_Max> edgeRandChoise_;


	Backup backup_;



	void Run(ChronoTimer& timer) {
		Ready();

		static State state;
		InitState(state);





		SimulatedAnnealing sa;
		sa.stepLoopCount = 10;
		auto transitions = make_tuple(
			MAKE_TRANS(Transition_Update1, HP.Trans1_Rate)
			, MAKE_TRANS(Transition_Update2, HP.Trans2_Rate)
		);

		MArr<int> cancelEis(M);
		MArr<s8> oldDays;
		iota(ALL(cancelEis), 0);
		oldDays.assign(M, -1);
		NArr<pint> radiuses;

		{
			auto rs = state.agg.rs_;
			state.agg.Init(server.baseDistmat_, server.fromEiMat_);
			state.agg.CancelConstruct(server.edges_, server.aroundEdges_, cancelEis, rs, cutViss_[0], dummyBackup_);

			state.SetScore(state.agg.GetScore());
		}

		const auto startTime = timer.ElapseTimeMs();

		const int VisSetCount = HP.VisSetCount;
		state.trans1VisSets_.resize(VisSetCount);
		state.trans2VisSets_.resize(VisSetCount);
		REP(ci, HP.CutCount) {
			cauto& vis = totalViss_[ci];
			Aggregate::MakeRadiuses(vis, radiuses);
			REP(si, VisSetCount) {
				Aggregate::MakeSimpleVis(radiuses, HP.VisRadius_Trans1 / (si + 1), state.trans1VisSets_[si]);
				Aggregate::MakeSimpleVis(radiuses, HP.VisRadius_Trans2 / (si + 1), state.trans2VisSets_[si]);
			}

			timer.Start(startTime + (TIME_LIMIT - startTime) * (ci + 1) / HP.CutCount);
			sa.Run2(timer, state, transitions);

			if (ci + 1 < HP.CutCount) {
				state.agg.CancelConstructDiff(server.edges_, server.aroundEdges_, cancelEis, oldDays, state.agg.rs_, cutViss_[ci + 1], dummyBackup_);

				state.SetScore(state.agg.GetScore());
			}
		}

		server.Output(state.agg.rs_);


	}

	void Ready() {
		vertexRandChoise_.Init(N, true);
		edgeRandChoise_.Init(M, true);

		{
			allVis_.resize(N);
			iota(ALL(allVis_), 0);

			cutViss_.resize(HP.CutCount);
			totalViss_.resize(HP.CutCount);

			REP(vi, N) {
				cutViss_[vi % HP.CutCount].push_back(vi);
			}
			totalViss_[0] = cutViss_[0];
			FOR(ci, 1, HP.CutCount) {
				totalViss_[ci] = totalViss_[ci - 1];
				totalViss_[ci].MemInsert(totalViss_[ci].size(), cutViss_[ci].data(), cutViss_[ci].size());
			}

			NArr<int> vis;
			REP(vi, N) {
				if (vi % 2 == 0) {
					vis.push_back(vi);
				}
			}
			Aggregate::CalcSimpleVis(vis, HP.VisRadius_Initial, initialVisSet_);		
			Aggregate::CalcSimpleVis(vis, HP.VisRadius_Route, routeVisSet_);
		}

		backup_.Init();
	}


	double Eval(const Aggregate& agg, double progress) const {
		double score = agg.GetScore();



		return score;
	}

	void InitState(State& state) {
		InitAgg(state.agg);
		state.SetScore(state.agg.GetScore());
	}

	void InitAgg(Aggregate& agg) {
		cauto& edges = server.edges_;
		cauto& aroundEdges = server.aroundEdges_;
		cauto& points = server.points_;

		agg.Init(server.baseDistmat_, server.fromEiMat_);

		MArr<int> orderEis;
		{
			MArr<bool> used;
			used.assign(M, false);

			cauto& vis = routeVisSet_.testVis;

			static vector<pint> viPairs;
			viPairs.reserve(vis.size() * vis.size());

			REP(i, vis.size()) {
				int viA = vis[i];
				cauto& fromEis = server.fromEiMat_[viA];

				FOR(j, i + 1, vis.size()) {
					int viB = vis[j];
					viPairs.push_back(pint{ viA, viB });
				}
			}
			stable_sort(ALL(viPairs), [&](pint a, pint b) {
				return server.baseDistmat_[a.a][a.b] > server.baseDistmat_[b.a][b.b];
				});

			for (cauto& pair : viPairs) {
				int viA = pair.a;
				int viB = pair.b;
				cauto& fromEis = server.fromEiMat_[viA];
				int vi = viB;
				while (true) {
					int ei = fromEis[vi];
					int nextVi = (edges[ei].a == vi) ? edges[ei].b : edges[ei].a;

					if (!used[ei]) {
						used[ei] = true;
						orderEis.push_back(ei);
					}
					vi = nextVi;
					if (vi == viA) {
						break;
					}
				}
			}
			REP(ei, M) {
				if (used[ei]) {
					continue;
				}
				orderEis.push_back(ei);
			}
		}

		cauto& visSet = initialVisSet_;

		double dayScoreMul = D * N * (N - 1) / 1000.0;

		MArr<int> cancelEis;
		MArr<s8> newDays;
		for (int ei : orderEis) {

			int bestDay = -1;
			s64 bestDistScore = TOTAL_DIST_MAX;
			REP(day, D) {
				if (agg.daysData_[day].constructCount_ >= K) {
					continue;
				}

				cauto& edge = edges[ei];
				auto& dd = agg.daysData_[day];
				bool invalid = false;
				REP(i, 2) {
					int vi = edge.vs[i];
					cauto& vd = dd.vertexsData_[vi];
					if (vd.constructCount_ + 1 == aroundEdges[vi].size()) {
						invalid = true;
						break;
					}
				}
				if (invalid) {
					continue;
				}

				s64 evalScore = (s64)(agg.daysData_[day].constructCount_ * dayScoreMul * HP.DayWeight);

				s64 totalDist = 0;
				if (agg.CalcConstructTotalDist(edges, aroundEdges, day, ei, visSet.testVis, bestDistScore - evalScore, totalDist)) {
					s64 distScore = totalDist + evalScore;


					if (distScore < bestDistScore) {
						bestDistScore = distScore;
						bestDay = day;
					}
				}

				if (agg.daysData_[day].constructCount_ == 0) {
					break;
				}
			}
			if (bestDay >= 0) {
				agg.ConstructSimple(edges, aroundEdges, bestDay, ei, visSet.testVis, dummyBackup_);
				cancelEis.push_back(ei);
				newDays.push_back(bestDay);
			}
		}

		if (agg.constructCount_ < M) {
			int fillCount = 0;
			REP(ei, M) {
				if (agg.rs_[ei] >= 0) {
					continue;
				}

				int bestDay = 0;
				s64 bestDistScore = TOTAL_DIST_MAX;
				REP(day, D) {
					if (agg.daysData_[day].constructCount_ < K) {
						s64 totalDist = 0;
						if (agg.CalcConstructTotalDist(edges, aroundEdges, day, ei, visSet.testVis, bestDistScore, totalDist)) {
							if (totalDist < bestDistScore) {
								bestDistScore = totalDist;
								bestDay = day;
							}
						}
					}
				}
				agg.ConstructSimple(edges, aroundEdges, bestDay, ei, visSet.testVis, dummyBackup_);
				cancelEis.push_back(ei);
				newDays.push_back(bestDay);
				++fillCount;
			}
		}

	}


	static int TimeToStep(double timeRate, size_t count) {
		int step = (int)floor(timeRate * count);
		chmin(step, (int)count - 1);
		return step;
	}

	void Transition_Update1(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& edges = server.edges_;
		cauto& aroundEdges = server.aroundEdges_;

		backup_.Clear();

		auto& agg = state.agg;
		int ei = edgeRandChoise_.Choice(rand_);
		edgeRandChoise_.ResetIfEnd();
		int oldDay = agg.rs_[ei];
		s64 oldTotalDist = agg.totalDist_;
		VASSERT(oldDay >= 0);

		cauto& visSet = state.trans1VisSets_[TimeToStep(sa.timeRate, state.trans1VisSets_.size())];

		static MArr<int> cancelEis;
		cancelEis.clear();
		{
			cancelEis.push_back(ei);
			agg.CancelSimple(edges, aroundEdges, cancelEis, visSet.testVis, backup_);
		}

		int bestDay = oldDay;
		s64 bestTotalDist = oldTotalDist;
		REP(day, D) {
			if (day == oldDay) {
				continue;
			}
			if (agg.daysData_[day].constructCount_ < K) {
				s64 totalDist = 0;
				if (agg.CalcConstructTotalDist(edges, aroundEdges, day, ei, visSet.testVis, bestTotalDist, totalDist)) {
					if (totalDist < bestTotalDist) {
						bestTotalDist = totalDist;
						bestDay = day;
					}
				}
			}
		}

		if (bestDay == oldDay) {
			agg.Restore(edges, backup_);
			return;
		}

		static MArr<s8> oldDays;
		static MArr<s8> newDays;
		oldDays.clear();
		newDays.clear();
		oldDays.push_back(oldDay);
		newDays.push_back(bestDay);

		agg.ConstructSimple(edges, aroundEdges, bestDay, ei, visSet.testVis, backup_);
		agg.CancelConstructDiff(edges, aroundEdges, cancelEis, oldDays, newDays, visSet.otherVis, backup_);
		double bestScore = Eval(agg, sa.timeRate);

		if (checker(-bestScore)) {
			state.SetScore(bestScore);
		}
		else {
			agg.Restore(edges, backup_);
		}
	}

	void Transition_Update2(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		cauto& edges = server.edges_;
		cauto& aroundEdges = server.aroundEdges_;

		backup_.Clear();

		auto& agg = state.agg;
		cauto& visSet = state.trans2VisSets_[TimeToStep(sa.timeRate, state.trans2VisSets_.size())];
		const s64 oldTotalDist = agg.totalDist_;

		static MArr<int> cancelEis;
		cancelEis.clear();

		for (cauto& ae : aroundEdges[vertexRandChoise_.Choice(rand_)]) {
			cancelEis.push_back(ae.ei);
		}
		vertexRandChoise_.ResetIfEnd();
		Shuffle(ALL(cancelEis), rand_);
		static MArr<s8> oldDays;
		oldDays.clear();
		REP(ci, cancelEis.size()) {
			int ei = cancelEis[ci];
			oldDays.push_back(agg.rs_[ei]);
		}

		agg.CancelSimple(edges, aroundEdges, cancelEis, visSet.testVis, backup_);

		static MArr<s8> newDays;
		newDays.clear();
		REP(ci, cancelEis.size()) {
			int ei = cancelEis[ci];
			int bestDay = -1;
			s64 bestTotalDist = sa.timeRate < 0.5 ? oldTotalDist : TOTAL_DIST_MAX;		
			REP(day, D) {
				if (agg.daysData_[day].constructCount_ < K) {
					s64 totalDist = 0;
					if (agg.CalcConstructTotalDist(edges, aroundEdges, day, ei, visSet.testVis, bestTotalDist, totalDist)) {
						if (totalDist < bestTotalDist) {
							bestTotalDist = totalDist;
							bestDay = day;
						}
					}
				}
			}
			if (bestDay < 0) {
				agg.Restore(edges, backup_);
				return;
			}
			agg.ConstructSimple(edges, aroundEdges, bestDay, ei, visSet.testVis, backup_);
			newDays.push_back(bestDay);
		}

		if (newDays == oldDays) {
			agg.Restore(edges, backup_);
			return;
		}
		agg.CancelConstructDiff(edges, aroundEdges, cancelEis, oldDays, newDays, visSet.otherVis, backup_);
		double newScore = Eval(agg, sa.timeRate);

		if (checker(-newScore)) {
			state.SetScore(newScore);
		}
		else {
			agg.Restore(edges, backup_);
		}
	}



	void Test(Aggregate& agg) {
		cauto& edges = server.edges_;
		cauto& aroundEdges = server.aroundEdges_;

	}
};
static constexpr int SolverSize = sizeof(Solver);

struct Main {
    void Run(int argc, const char* argv[]) {
        ChronoTimer timer;
        server.InitInput(timer);

        timer.StartMs(TIME_LIMIT);

        static Solver solver;
        solver.Run(timer);
        server.Finalize();

    }
};
