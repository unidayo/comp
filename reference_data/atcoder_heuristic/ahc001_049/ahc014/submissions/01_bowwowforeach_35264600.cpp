
#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0

#define TIME_LIMIT (4900)

#define BENCH 0

#define NOT_SUBMIT 0
#define VALIDATION 0

#define IO_FILE 0

#define OUTPUT_INFO 0
#define OUTPUT_FINAL_INFO 0
#define OUTPUT_LOG 0
#define OUTPUT_VISUAL 0

#define FIX_RESULT 0


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

template <class T> inline void chmin(T& a, T b) { if (b < a) { a = b; } }
template <class T> inline void chmax(T& a, T b) { if (a < b) { a = b; } }
template <class T> inline constexpr T square(T v) { return v * v; }

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
	inline int LeftToUS(const TimePoint& limit) const {
		return (int)chrono::duration_cast<chrono::microseconds>(limit - chrono::high_resolution_clock::now()).count();
	}

	inline double NowRate() const {
		return (chrono::high_resolution_clock::now() - startTime_).count() / (double)(endTime_ - startTime_).count();
	}

	inline TimePoint Now() const {
		return chrono::high_resolution_clock::now();
	}
	inline TimePoint EndTime() const {
		return endTime_;
	}

	TimePoint GetLimitTimePointUs(int limit) const {
		return startTime_ + chrono::microseconds(limit);
	}
};

ChronoTimer timer;

template <class T>
void InstanceRun(int argc, const char* argv[]) {
	timer.Init();
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

inline pdouble ToDouble(const pint& p) {
	return pdouble{ double(p.x), double(p.y) };
}

static_assert(is_trivially_copyable<pint>::value, "not trivially_copyable");

template <class A, class B>
struct tuple_size<pr<A, B>> : integral_constant<size_t, 2> {};

template <class A, class B>
struct tuple_element<0, pr<A, B>> { using type = A; };
template <class A, class B>
struct tuple_element<1, pr<A, B>> { using type = B; };

inline pint round(const pdouble& d) {
	return pint{ (int)round(d.x), (int)round(d.y) };
}
inline double norm(const pdouble& d) {
	return sqrt((d.x * d.x) + (d.y * d.y));
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
		return memcmp(array_.data(), r.array_.data(), sizeof(T) * count_) == 0;
	}

	constexpr int capacity() const {
		return CAPACITY;
	}

	inline void clear() {
		count_ = 0;
	}
	inline void Clear() {
		clear();
	}

	inline void resize(int count) {
		count_ = count;
	}
	inline void Resize(int count) {
		resize(count);
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
	inline void PopBack() {
		pop_back();
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

	inline void RemoveSlide(int i) {
		array_[i] = array_[count_ - 1];
		--count_;
	}

	inline int GetCount() const {
		return count_;
	}
	inline int size() const {
		return count_;
	}
	inline int sz() const {
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
};


struct Random {
	static inline Xor64& Instnce() {
		static Xor64 x;
		return x;
	}

	static inline uint64_t Get() {
		return Instnce()();
	}
	static inline uint64_t Get(uint64_t l, uint64_t r) {
		return Instnce()(l, r);
	}
	static inline uint64_t Get(uint64_t r) {
		return Instnce()(r);
	}

	static inline void Get2(uint64_t N, uint64_t& a, uint64_t& b) {
		VASSERT(N >= 2);
		a = Get(N);
		b = Get(N - 1);
		if (b >= a) {
			++b;
		}
	}

	static inline double GetDouble() {
		return Get() / (double)UINT64_MAX;
	}

	static inline double GetDouble(double l, double r) {
		return l + GetDouble() * (r - l);
	}
};


#define PARAM_CATEGORY(NAME, VALUE, VALUE_LIST, TUNING) const char* NAME = VALUE;
#define PARAM_INT(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, TUNING) int NAME = VALUE;
#define PARAM_DOUBLE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, TUNING) double NAME = VALUE;

#define PARAM_INT_RANGE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, LOWER_LIMIT, UPPER_LIMIT, TUNING) int NAME = VALUE;
#define PARAM_DOUBLE_RANGE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, LOWER_LIMIT, UPPER_LIMIT, TUNING) double NAME = VALUE;





constexpr
struct {
	PARAM_DOUBLE(SparceLengthRate, 1358.590767698597, 1000.0, 1500.0, true);
	PARAM_DOUBLE(SparceWeightRate, 0.15896722387390944, 0.2, 0.3, true);
	PARAM_DOUBLE_RANGE(SparseRawRandGeta, 0.3714884419687901, 0.35, 0.5, 0.0, 1.0, true);
	PARAM_DOUBLE_RANGE(SparseUpperDeleteRate, 0.16673124030605707, 0.1, 0.25, 0.0, 1.0, true);
	PARAM_DOUBLE(SparseStartTemp, 32556.408291168456, 30000.0, 40000.0, true);
	PARAM_DOUBLE(SparseEndTemp, 8296.998284767324, 3000.0, 7000.0, true);
	PARAM_DOUBLE_RANGE(SparceRollbackStart, 0.3330005032652228, 0.0, 0.2, 0.0, 1.0, true);
	PARAM_INT_RANGE(SparseRollbackCount, 14000, 9000, 14000, 100, 1000000000, true);
	PARAM_DOUBLE_RANGE(SparseRollbackNext, 1.1214822474641735, 1.1, 1.25, 1.0, 100.0, true);
	PARAM_INT_RANGE(SparseTransRand, 74, 50, 100, 0, 10000, true);
	PARAM_INT_RANGE(SparseTransEvalGready, 31, 30, 100, 0, 10000, true);
	PARAM_INT_RANGE(SparseTransRawRand, 28, 20, 70, 0, 10000, true);



	PARAM_DOUBLE(DenseLengthRate_L, 2055.1635043689894, 1500.0, 2500.0, false);
	PARAM_DOUBLE(DenseLengthRate_R, 2270.618091088909, 1500.0, 2500.0, false);
	PARAM_DOUBLE(DenseWeightRate_L, -1.6309294843867599, -3.0, -1.0, false);
	PARAM_DOUBLE(DenseWeightRate_R, 1.1592919376086872, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseRawRandGeta_L, 0.0006829929056991601, 0.0, 0.3, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseRawRandGeta_R, 0.000709726006222135, 0.0, 0.3, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseUpperDeleteRate1_L, 0.230462690691009, 0.2, 0.4, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseUpperDeleteRate1_R, 0.04083384751279569, 0.0, 0.1, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseUpperDeleteRate2_L, 0.20737558248166468, 0.0, 0.2, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseUpperDeleteRate2_R, 0.011767620570048737, 0.0, 0.1, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseFirstSATime_L, 0.45954922502766204, 0.3, 0.6, 0.1, 0.9, false);
	PARAM_DOUBLE_RANGE(DenseFirstSATime_R, 0.6055384677403888, 0.6, 0.8, 0.1, 0.9, false);
	PARAM_DOUBLE_RANGE(DenseStartTemp1_L, 19341.596236743808, 15000.0, 20000.0, 0.0, 1000000000.0, false);
	PARAM_DOUBLE_RANGE(DenseStartTemp1_R, 9658.52993343047, 5000.0, 10000.0, 0.0, 1000000000.0, false);
	PARAM_DOUBLE_RANGE(DenseEndTemp1_L, 254.01324994019112, 200.0, 300.0, 0.0, 1000000000.0, false);
	PARAM_DOUBLE_RANGE(DenseEndTemp1_R, 304.3748950766912, 250.0, 400.0, 0.0, 1000000000.0, false);
	PARAM_DOUBLE_RANGE(DenseRollbackStart1_L, 0.7707660311179564, 0.5, 0.9, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseRollbackStart1_R, 0.8022327437942074, 0.3, 0.7, 0.0, 1.0, false);
	PARAM_INT_RANGE(DenseRollbackCount1_L, 4038, 1000, 5000, 100, 1000000000, false);
	PARAM_INT_RANGE(DenseRollbackCount1_R, 21986, 18000, 22000, 100, 1000000000, false);
	PARAM_DOUBLE_RANGE(DenseRollbackNext1_L, 1.835015118082963, 1.2, 1.8, 1.0, 100.0, false);
	PARAM_DOUBLE_RANGE(DenseRollbackNext1_R, 1.8414177366907856, 1.2, 1.8, 1.0, 100.0, false);
	PARAM_INT_RANGE(DenseTrans1_Rand_L, 47, 50, 150, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans1_Rand_R, 46, 0, 50, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans1_PointScore_L, 125, 50, 150, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans1_PointScore_R, 268, 150, 250, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans1_RawRand_L, 18, 0, 50, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans1_RawRand_R, 67, 40, 90, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans1_EvalGready_L, 26, 0, 100, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans1_EvalGready_R, 65, 50, 100, 0, 10000, false);
	PARAM_DOUBLE_RANGE(DenseStartTemp2_L, 31735.008209379233, 30000.0, 35000.0, 0.0, 1000000000.0, false);
	PARAM_DOUBLE_RANGE(DenseStartTemp2_R, 716.4608296541028, 0.0, 2000.0, 0.0, 1000000000.0, false);
	PARAM_DOUBLE_RANGE(DenseEndTemp2_L, 692.3549298016644, 550.0, 650.0, 0.0, 1000000000.0, false);
	PARAM_DOUBLE_RANGE(DenseEndTemp2_R, 357.0574019167821, 350.0, 450.0, 0.0, 1000000000.0, false);
	PARAM_DOUBLE_RANGE(DenseRollbackStart2_L, 0.04746777525635565, 0.0, 0.3, 0.0, 1.0, false);
	PARAM_DOUBLE_RANGE(DenseRollbackStart2_R, 0.011704995513039766, 0.0, 0.3, 0.0, 1.0, false);
	PARAM_INT_RANGE(DenseRollbackCount2_L, 2083, 1000, 5000, 1000, 1000000000, false);
	PARAM_INT_RANGE(DenseRollbackCount2_R, 9278, 9000, 11000, 1000, 1000000000, false);
	PARAM_DOUBLE_RANGE(DenseRollbackNext2_L, 1.620943699566913, 1.5, 2.0, 1.0, 100.0, false);
	PARAM_DOUBLE_RANGE(DenseRollbackNext2_R, 1.7766357515862299, 1.4, 1.8, 1.0, 100.0, false);
	PARAM_INT_RANGE(DenseTrans2_Rand_L, 125, 0, 150, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans2_Rand_R, 19, 0, 30, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans2_PointScore_L, 103, 50, 100, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans2_PointScore_R, 18, 0, 40, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans2_RawRand_L, 164, 140, 200, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans2_RawRand_R, 16, 0, 50, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans2_EvalGready_L, 0, 0, 20, 0, 10000, false);
	PARAM_INT_RANGE(DenseTrans2_EvalGready_R, 106, 50, 100, 0, 10000, false);
	PARAM_DOUBLE(PointScoreP2_L, 14.691297508438229, 0.0, 10.0, false);
	PARAM_DOUBLE(PointScoreP2_R, 9.59260598792893, 8.0, 16.0, false);
	PARAM_DOUBLE(PointScoreP3_L, 23.2269553707886, 20.0, 32.0, false);
	PARAM_DOUBLE(PointScoreP3_R, 31.680528388337542, 20.0, 32.0, false);
	PARAM_DOUBLE(PointScoreComplete_L, 14.75501623674338, 12.0, 20.0, false);
	PARAM_DOUBLE(PointScoreComplete_R, 16.37658545738926, 12.0, 30.0, false);
	PARAM_DOUBLE(PointScorePenalty_L, 25.303363668715708, 10.0, 60.0, false);
	PARAM_DOUBLE(PointScorePenalty_R, 49.70669883891135, 20.0, 80.0, false);















} HP;

constexpr int N_MIN = 31;
constexpr int N_MAX = 61;
constexpr int NN_MAX = N_MAX * N_MAX;

constexpr int M_MIN = 31;
constexpr int M_MAX = 310 + 1;		

constexpr double InitAreaMRateMin = 0.061394381;
constexpr double InitAreaMRateMax = 0.322580645;

enum class Dir : s8 {
	Invalid = -1,
	L = 0,
	LU,
	U,
	RU,
	R,
	RD,
	D,
	LD,
};
constexpr array<pint, 8> Around8 = {
	pint{-1, 0},
	pint{-1, -1},
	pint{0, -1},
	pint{1, -1},
	pint{1, 0},
	pint{1, 1},
	pint{0, 1},
	pint{-1, 1},
};
inline Dir Back(Dir dir) {
	return Dir((s8)dir ^ 0b100);
}
inline Dir Rotate90L(Dir dir) {
	return Dir(((s8)dir + 6) & 0b111);
}
inline Dir Rotate90R(Dir dir) {
	return Dir(((s8)dir + 2) & 0b111);
}
inline Dir Rotate45R(Dir dir) {
	return Dir(((s8)dir + 1) & 0b111);
}
inline Dir Rotate135R(Dir dir) {
	return Dir(((s8)dir + 3) & 0b111);
}
inline bool IsDiagonal(Dir dir) {
	return (s8)dir & 1;
}


struct Rect {
	array<s16, 4> ps;

	void Set(s16 p0, s16 p1, s16 p2, s16 p3) {
		ps[0] = p0;
		ps[1] = p1;
		ps[2] = p2;
		ps[3] = p3;
	}

	s16 DrawPoint() const {
		return ps[0];
	}

	bool operator == (const Rect& r) const {
		return 
			ps[0] == r.ps[0] &&
			ps[1] == r.ps[1] &&
			ps[2] == r.ps[2] &&
			ps[3] == r.ps[3];
	}
	bool operator != (const Rect& r) const {
		return !(*this == r);
	}
};
constexpr int RectSize = sizeof(Rect);

using DrawRects = CapArr<Rect, NN_MAX>;

struct Result {
	DrawRects rects;
};


struct GridSystem {
private:
	int N_ = 0;
	CapArr<pint, NN_MAX> to2d_;
public:

	void Init(int N) {
		N_ = N;

		to2d_.resize(N*N);
		REP(i, N*N) {
			to2d_[i] = pint{ i % N_, i / N_ };
		}
	}

	s32 To1D(int x, int y) const {
		return y * N_ + x;
	}
	s32 To1D(const pint& p) const {
		return p.y * N_ + p.x;
	}
	const pint& To2D(s32 i) const {
		return to2d_[i];
	}

	bool IsOut(int x, int y) const {
		return x < 0 || x >= N_ || y < 0 || y >= N_;
	}
	bool IsOut(const pint& p) const {
		return p.x < 0 || p.x >= N_ || p.y < 0 || p.y >= N_;
	}
	bool IsIn(const pint& p) const {
		return !IsOut(p);
	}

	s32 CounterCell(s32 baseCell, s32 sideCell1, s32 sideCell2) const {
		pint p0 = To2D(baseCell);
		pint p1 = To2D(sideCell1);
		pint p2 = To2D(sideCell2);
		pint p3 = p2 + (p1 - p0);
		if (IsOut(p3)) {
			return -1;
		}
		return To1D(p3);
	}

	Dir CalcDir(s32 from, s32 to) const {
		pint s = To2D(to) - To2D(from);
		if (s.x == 0) {
			if (s.y == 0) {
				return Dir(-1);
			}
			else if (s.y < 0) {
				return Dir::U;
			}
			else {
				return Dir::D;
			}
		}
		else if (s.y == 0) {
			if (s.x < 0) {
				return Dir::L;
			}
			else {
				return Dir::R;
			}
		}
		else {
			if (abs(s.x) != abs(s.y)) {
				return Dir::Invalid;
			}
			if (s.x < 0) {
				if (s.y < 0) {
					return Dir::LU;
				}
				else {
					return Dir::LD;
				}
			}
			else {
				if (s.y < 0) {
					return Dir::RU;
				}
				else {
					return Dir::RD;
				}
			}
		}
	}
	Dir CalcDirAndDist(s32 from, s32 to, int& dist) const {
		pint s = To2D(to) - To2D(from);
		if (s.x == 0) {
			if (s.y == 0) {
				dist = 0;
				return Dir(-1);
			}
			else if (s.y < 0) {
				dist = abs(s.y);
				return Dir::U;
			}
			else {
				dist = abs(s.y);
				return Dir::D;
			}
		}
		else if (s.y == 0) {
			if (s.x < 0) {
				dist = abs(s.x);
				return Dir::L;
			}
			else {
				dist = abs(s.x);
				return Dir::R;
			}
		}
		else {
			if (abs(s.x) != abs(s.y)) {
				dist = -1;
				return Dir::Invalid;
			}
			dist = abs(s.x);
			if (s.x < 0) {
				if (s.y < 0) {
					return Dir::LU;
				}
				else {
					return Dir::LD;
				}
			}
			else {
				if (s.y < 0) {
					return Dir::RU;
				}
				else {
					return Dir::RD;
				}
			}
		}
	}

	bool IsDiagonal(const Rect& rect) const {
		return ::IsDiagonal(CalcDir(rect.ps[0], rect.ps[1]));
	}

	pint Rotate45(const pint p) const {
		return pint{ p.x - p.y, p.x + p.y };
	};
};
GridSystem gs;

struct AroundMap {
private:
	CapArr<array<s32, 8>, NN_MAX> aroundCells_;		
	CapArr<bool, NN_MAX> isIn_;

public:
	void Init(int N) {
		const int NN = N * N;
		aroundCells_.resize(N * N);
		isIn_.assign(NN, true);
		REP(i, N * N) {
			pint p = gs.To2D(i);
			REP(dir, 8) {
				pint aroundP = p + Around8[dir];
				s32 aroundCell = -1;
				if (gs.IsIn(aroundP)) {
					aroundCell = gs.To1D(aroundP);
				}
				aroundCells_[i][dir] = aroundCell;
			}
		}
	}

	void Init(int N, int left, int top, int right, int bottom) {
		const int NN = N * N;
		aroundCells_.resize(N * N);
		isIn_.assign(NN, false);
		REP(i, N * N) {
			pint p = gs.To2D(i);
			REP(dir, 8) {
				pint aroundP = p + Around8[dir];
				s32 aroundCell = -1;
				if (aroundP.x >= left && aroundP.x <= right && aroundP.y >= top && aroundP.y <= bottom) {
					aroundCell = gs.To1D(aroundP);
				}
				aroundCells_[i][dir] = aroundCell;
			}
			if (p.x >= left && p.x <= right && p.y >= top && p.y <= bottom) {
				isIn_[i] = true;
			}
		}
	}

	bool IsIn(int cell) const {
		return isIn_[cell];
	}

	const array<s32, 8>& operator[](int i) const {
		return aroundCells_[i];
	}


};
AroundMap aroundMap;
AroundMap limitAroundMap;

int Length(const Rect& rect) {
	array<pint, 3> ps = {};
	REP(i, 3) {
		ps[i] = gs.To2D(rect.ps[i]);
	}
	int len = 0;
	REP(i, 2) {
		pint diff = ps[i] - ps[i + 1];
		len += max(abs(diff.x), abs(diff.y));
	}
	return len;
}



struct Scorer {
	int N_ = 0;
	int M_ = 0;
	int S_ = 0;
	int Q_ = 0;
	int C_ = 0;

	double baseScore_ = 0;		

	void Init(int N, int M) {
		N_ = N;
		M_ = M;
		S_ = 0;
		Q_ = 0;

		C_ = (N_ - 1) / 2;
		REP(i, N*N) {
			S_ += Weight(i);
		}

		baseScore_ = (1000000.0 * N_ * N_) / (double)(M_ * S_);
	}

	void AddMark(int i) {
		Q_ += Weight(i);
	}

private:
	int Weight(int i) const {
		auto p = gs.To2D(i);
		int w = square(p.x - C_) + square(p.y - C_) + 1;
		return w;
	}
public:

	double Score() const {
		return baseScore_ * Q_;
	}

	double Score(int totalWeight) const {
		return baseScore_ * totalWeight;
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

    inline bool operator[](int i) {
        return checked_[i] == mark_;
    }

    inline void Check(int i) {
        checked_[i] = mark_;
    }

    inline void Reset(int i) {
        checked_[i] = mark_ - 1;
    }
};

struct CheckMapLightD {
private:
    vector<u8> checked_;
    u8 mark_ = 1;

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

    inline bool operator[](int i) {
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
struct CheckMapDataD {
private:
    vector<T> data_;
    vector<u32> checked_;
    u32 mark_ = 1;

public:
    void Init(int size) {
        SetSize(size);
        Clear();
    }

    void SetSize(int size) {
        checked_.resize(size, mark_);
        data_.resize(size);
    }

    void Clear() {
        ++mark_;

        if (mark_ == 0) {
            checked_.assign(checked_.size(), 0);
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
    const T& operator[](int i) const {
        VASSERT(checked_[i] == mark_);
        return data_[i];
    }
    T& operator[](int i) {
        VASSERT(checked_[i] == mark_);
        return data_[i];
    }
};


struct EdgeCheckMap {
	CheckMapLightD checkMap_;

	void Init(int N) {
		checkMap_.SetSize(N*N * 4);
		checkMap_.Clear();
	}

	bool TestEdge(s32 cell, Dir dir) const {
		if ((s8)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		return checkMap_.IsChecked((cell << 2) | (s32)dir);
	}

	void SetEdge(s32 cell, Dir dir) {
		if ((s8)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		checkMap_.Check((cell << 2) | (s32)dir);
	}
};


template <int CAP_NN>
struct GridState {
private:
	int N_ = 0;
	int NN_ = 0;
	bitset<CAP_NN> points_;			
	bitset<CAP_NN * 4> edges_;		

public:
	bool operator ==(const GridState& r) const{
		return points_ == r.points_ && edges_ == r.edges_;
	}
	bool operator !=(const GridState& r) const {
		return !(*this == r);
	}

public:
	void SetEdge(s32 cell, Dir dir) {
		VASSERT(aroundMap[cell][(int)dir] >= 0);
		if ((int)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		edges_.set((cell << 2) | (s32)dir);
	}
	void ResetEdge(s32 cell, Dir dir) {
		VASSERT(aroundMap[cell][(int)dir] >= 0);
		if ((int)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		edges_.reset((cell << 2) | (s32)dir);
	}

	void SetPoint(s32 cell) {
		points_.set(cell);
	}
	void ResetPoint(s32 cell) {
		points_.reset(cell);
	}


	bool TestEdge(s32 cell, Dir dir) const {
		VASSERT(aroundMap[cell][(int)dir] >= 0);
		if ((int)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		return edges_.test((cell << 2) | (s32)dir);
	}

	bool TestPoint(s32 cell) const {
		return points_.test(cell);
	}

public:

	void Init(int N, const CapArr<s32, M_MAX>& marks) {
		N_ = N;
		NN_ = N * N;
		points_.reset();
		edges_.reset();
		REP(i, marks.size()) {
			points_.set(marks[i]);
		}
	}

	bool CanConnect(s32 startCell, s32 endCell, Dir dir, const AroundMap& aroundMap) const {
		s32 curCell = startCell;
		if (TestEdge(curCell, (Dir)dir)) {
			return false;
		}
		while (true) {
			int nextCell = aroundMap[curCell][(int)dir];
			VASSERT(nextCell >= 0);
			if (nextCell == endCell) {
				break;
			}
			if (points_.test(nextCell)) {
				return false;
			}
			curCell = nextCell;
		}
		return true;
	}
	bool CanConnect(s32 startCell, s32 endCell, const AroundMap& aroundMap) const {
		Dir dir = gs.CalcDir(startCell, endCell);
		return CanConnect(startCell, endCell, dir, aroundMap);
	}

	template <int CAP_VALID>
	void EnumValidRectForInit(CapArr<Rect, CAP_VALID>& rects, const AroundMap& aroundMap = ::aroundMap) const {
		rects.clear();

		int const NN = NN_;
		array<s32, 8> marks;

		REP(i, NN) {
			if (!aroundMap.IsIn(i)) {
				continue;
			}
			if (points_.test(i)) {
				continue;
			}
			marks.fill(-1);
			REP(dir, 8) {
				int curCell = i;
				while (true) {
					int nextCell = aroundMap[curCell][dir];
					if (nextCell < 0) {
						break;
					}
					if (points_.test(nextCell)) {
						marks[dir] = nextCell;
						break;
					}
					curCell = nextCell;
				}
			}

			REP(dir, 8) {
				if (marks[dir] < 0) {
					continue;
				}
				int dir90 = (int)Rotate90R((Dir)dir);
				if (marks[dir90] < 0) {
					continue;
				}

				s32 counterCell = gs.CounterCell(i, marks[dir], marks[dir90]);
				if (counterCell < 0) {
					continue;
				}
				if (!aroundMap.IsIn(counterCell)) {
					continue;
				}
				if (!points_.test(counterCell)) {
					continue;
				}

				if (CanConnect(marks[dir], counterCell, Dir(dir90), aroundMap) &&
					CanConnect(marks[dir90], counterCell, Dir(dir), aroundMap)) {
					Rect* rect = rects.PushBack();
					rect->ps[0] = i;
					rect->ps[1] = marks[dir];
					rect->ps[2] = counterCell;
					rect->ps[3] = marks[dir90];
				}
			}
		}
	}

	template <int CAP_VALID>
	void EnumValidRect(CapArr<Rect, CAP_VALID>& rects, const AroundMap& aroundMap = ::aroundMap) const {
		rects.clear();

		int const NN = NN_;
		array<s32, 8> marks;

		REP(i, NN) {
			if (!aroundMap.IsIn(i)) {
				continue;
			}
			if (points_.test(i)) {
				continue;
			}

			marks.fill(-1);
			REP(dir, 8) {
				int curCell = i;
				while (true) {
					int nextCell = aroundMap[curCell][dir];
					if (nextCell < 0) {
						break;
					}
					if (TestEdge(curCell, (Dir)dir)) {
						break;
					}
					if (points_.test(nextCell)) {
						marks[dir] = nextCell;
						break;
					}
					curCell = nextCell;
				}
			}

			REP(dir, 8) {
				if (marks[dir] < 0) {
					continue;
				}
				int dir90 = (int)Rotate90R((Dir)dir);
				if (marks[dir90] < 0) {
					continue;
				}

				s32 counterCell = gs.CounterCell(i, marks[dir], marks[dir90]);
				if (counterCell < 0) {
					continue;
				}
				if (!aroundMap.IsIn(counterCell)) {
					continue;
				}
				if (!points_.test(counterCell)) {
					continue;
				}

				if (CanConnect(marks[dir], counterCell, Dir(dir90), aroundMap) &&
					CanConnect(marks[dir90], counterCell, (Dir)dir, aroundMap)) {
					Rect* rect = rects.PushBack();
					rect->ps[0] = i;
					rect->ps[1] = marks[dir];
					rect->ps[2] = counterCell;
					rect->ps[3] = marks[dir90];
				}
			}
		}
	}

	void DrawRect(const Rect& rect) {
		VASSERT(!points_.test(rect.ps[0]));
		VASSERT(points_.test(rect.ps[1]));
		VASSERT(points_.test(rect.ps[2]));
		VASSERT(points_.test(rect.ps[3]));

		auto DrawLine = [&](s32 startCell, s32 endCell) {
			Dir dir = gs.CalcDir(startCell, endCell);

			s32 curCell = startCell;
			while (true) {
				VASSERT(!TestEdge(curCell, (Dir)dir));
				SetEdge(curCell, (Dir)dir);

				int nextCell = aroundMap[curCell][(int)dir];
				VASSERT(nextCell >= 0);
				if (nextCell == endCell) {
					break;
				}

				VASSERT(!points_.test(nextCell));
				curCell = nextCell;
			}
		};

		points_.set(rect.ps[0]);
		DrawLine(rect.ps[0], rect.ps[1]);
		DrawLine(rect.ps[1], rect.ps[2]);
		DrawLine(rect.ps[2], rect.ps[3]);
		DrawLine(rect.ps[3], rect.ps[0]);
	}

	void RemoveRect(const Rect& rect) {
		VASSERT(points_.test(rect.ps[0]));

		auto RemoveLine = [&](s32 startCell, s32 endCell) {
			Dir dir = gs.CalcDir(startCell, endCell);

			s32 curCell = startCell;
			while (true) {
				VASSERT(TestEdge(curCell, (Dir)dir));
				ResetEdge(curCell, (Dir)dir);

				int nextCell = aroundMap[curCell][(int)dir];
				VASSERT(nextCell >= 0);
				if (nextCell == endCell) {
					break;
				}

				curCell = nextCell;
			}
		};

		points_.reset(rect.ps[0]);
		RemoveLine(rect.ps[0], rect.ps[1]);
		RemoveLine(rect.ps[1], rect.ps[2]);
		RemoveLine(rect.ps[2], rect.ps[3]);
		RemoveLine(rect.ps[3], rect.ps[0]);
	}

	struct Bound {
		int l;
		int t;
		int r;
		int b;
	};

	void ToBound(const Rect& rect, bool diagonal, Bound& bound) const {
		if (diagonal) {
			const pint& p0 = gs.To2D(rect.ps[0]);
			const pint& p1 = gs.To2D(rect.ps[2]);
			int x0 = p0.x - p0.y;
			int y0 = p0.x + p0.y;
			int x1 = p1.x - p1.y;
			int y1 = p1.x + p1.y;
			if (x0 < x1) {
				bound.l = x0;
				bound.r = x1;
			}
			else {
				bound.l = x1;
				bound.r = x0;
			}
			if (y0 < y1) {
				bound.t = y0;
				bound.b = y1;
			}
			else {
				bound.t = y1;
				bound.b = y0;
			}
		}
		else {
			const pint& p0 = gs.To2D(rect.ps[0]);
			const pint& p1 = gs.To2D(rect.ps[2]);
			if (p0.x < p1.x) {
				bound.l = p0.x;
				bound.r = p1.x;
			}
			else {
				bound.l = p1.x;
				bound.r = p0.x;
			}
			if (p0.y < p1.y) {
				bound.t = p0.y;
				bound.b = p1.y;
			}
			else {
				bound.t = p1.y;
				bound.b = p0.y;
			}
		}
	}

	bool IsValidAffterDraw(const Rect& newRect, const Bound& newBound, const pint& newPoint45, bool newDiagonal, const Rect& testRect) const {
		if (newRect.ps[0] == testRect.ps[0]) {
			return false;
		}

		bool testDiagonal = gs.IsDiagonal(testRect);
		Bound testBound;
		ToBound(testRect, testDiagonal, testBound);

		if (newDiagonal == testDiagonal) {
			if (newBound.t == testBound.t || newBound.t == testBound.b || newBound.b == testBound.t || newBound.b == testBound.b) {
				if (newBound.r <= testBound.l || newBound.l >= testBound.r) {
				}
				else {
					return false;
				}
			}
			if (newBound.l == testBound.l || newBound.l == testBound.r || newBound.r == testBound.l || newBound.r == testBound.r) {
				if (newBound.b <= testBound.t || newBound.t >= testBound.b) {
				}
				else {
					return false;
				}
			}
		}
		else {
			const pint& newPoint = testDiagonal ? newPoint45 : gs.To2D(newRect.ps[0]);

			if (newPoint.y == testBound.t || newPoint.y == testBound.b) {
				if (newPoint.x >= testBound.l && newPoint.x <= testBound.r) {
					return false;
				}
			}
			else if (newPoint.x == testBound.l || newPoint.x == testBound.r) {
				if (newPoint.y >= testBound.t && newPoint.y <= testBound.b) {
					return false;
				}
			}
		}

		return true;
	}

	template <int CAP_VALID>
	void UpdateValidRect(CapArr<Rect, CAP_VALID>& rects, const Rect& drawRect, const AroundMap& aroundMap = ::aroundMap, int* newRectStart = nullptr) {
		const int drawPoint = drawRect.DrawPoint();

		{
			static CapArr<s32, CAP_VALID> removeRis;
			removeRis.clear();

			bool drawDiagonal = gs.IsDiagonal(drawRect);
			Bound drawBound;
			ToBound(drawRect, drawDiagonal, drawBound);
			pint drawPoint45 = gs.Rotate45(gs.To2D(drawPoint));

			REP(ri, rects.size()) {
				const auto& rect = rects[ri];
				if (rect.ps[0] == drawPoint) {
					removeRis.push_back(ri);
					continue;
				}
				if (IsValidAffterDraw(drawRect, drawBound, drawPoint45, drawDiagonal, rect)) {
				}
				else {
					removeRis.push_back(ri);
				}
			}
			RREP(i, removeRis.size()) {
				rects.RemoveSlide(removeRis[i]);
			}
		}

		if (newRectStart) {
			*newRectStart = rects.size();
		}

		{
			auto FindLineTo = [&](int cell, Dir dir) {
				int idir = (int)dir;
				int next = aroundMap[cell][idir];
				if (next < 0) {
					return -1;
				}
				if (TestEdge(next, (Dir)dir)) {
					return -1;
				}
				cell = next;
				while (cell >= 0) {
					if (points_.test(cell)) {
						return cell;
					}
					cell = aroundMap[cell][idir];
				}
				return -1;
			};



			array<s32, 8> marks;
			array<s32, 8> dists;
			marks.fill(-1);
			dists.fill(INT_MAX);
			REP(dir, 8) {
				int curCell = drawPoint;
				int nextCell = aroundMap[curCell][dir];
				if (nextCell < 0) {
					continue;
				}
				if (TestEdge(curCell, (Dir)dir)) {
					continue;
				}
				curCell = nextCell;
				int d = 1;
				while (true) {
					if (points_.test(curCell)) {
						marks[dir] = curCell;
						dists[dir] = d;
						break;
					}
					int nextCell = aroundMap[curCell][dir];
					if (nextCell < 0) {
						break;
					}
					curCell = nextCell;
					++d;
				}
			}

			REP(dir, 8) {
				if (marks[dir] < 0) {
					continue;
				}
				int dir90 = (int)Rotate90R((Dir)dir);
				if (marks[dir90] < 0) {
					continue;
				}
				s32 counterCell = gs.CounterCell(drawPoint, marks[dir], marks[dir90]);
				if (counterCell < 0) {
					continue;
				}
				if (!aroundMap.IsIn(counterCell)) {
					continue;
				}
				if (points_.test(counterCell)) {
					continue;
				}
				if (CanConnect(marks[dir], counterCell, Dir(dir90), aroundMap) &&
					CanConnect(marks[dir90], counterCell, (Dir)dir, aroundMap)) {
					Rect* rect = rects.PushBack();
					rect->ps[0] = counterCell;
					rect->ps[1] = marks[dir90];
					rect->ps[2] = drawPoint;
					rect->ps[3] = marks[dir];
				}
			}

			REP(baseDir, 8) {
				if (marks[baseDir] < 0) {
					continue;
				}
				int p1 = marks[baseDir];
				for (Dir dir90 : {Rotate90R((Dir)baseDir), Rotate90L((Dir)baseDir)}) {
					int idir90 = (int)dir90;
					{
						int next = aroundMap[drawPoint][idir90];
						if (next < 0) {
							continue;
						}
						if (TestEdge(drawPoint, dir90)) {
							continue;
						}
					}

					int cell = p1;
					int next = aroundMap[cell][idir90];
					if (next < 0) {
						continue;
					}
					if (TestEdge(cell, dir90)) {
						continue;
					}
					cell = next;
					int p2 = -1;
					int maxLen = dists[idir90];
					REP(i, maxLen - 1) {
						if (points_.test(cell)) {
							p2 = cell;
							break;
						}
						cell = aroundMap[cell][idir90];
						if (cell < 0) {
							break;
						}
					}
					if (p2 < 0) {
						continue;
					}

					s32 counterCell = gs.CounterCell(p1, drawPoint, p2);
					if (counterCell < 0) {
						continue;
					}
					if (!aroundMap.IsIn(counterCell)) {
						continue;
					}

					if (CanConnect(counterCell, p2, (Dir)baseDir, aroundMap)) {
						Rect* rect = rects.PushBack();
						if (dir90 == Rotate90R((Dir)baseDir)) {
							rect->ps[0] = counterCell;
							rect->ps[1] = drawPoint;
							rect->ps[2] = p1;
							rect->ps[3] = p2;
						}
						else {
							rect->ps[0] = counterCell;
							rect->ps[1] = p2;
							rect->ps[2] = p1;
							rect->ps[3] = drawPoint;
						}
					}
				}

			}
		}
	}

	bool CanDrawPoint3(const Rect& rect) const {
		return points_.test(rect.ps[1]) && points_.test(rect.ps[2]) && points_.test(rect.ps[3]);
	}

	template <int CAP_VALID, int CAP_DRAW>
	void RemovedUpdateValidateRects(CapArr<Rect, CAP_VALID>& rects, const CapArr<Rect, CAP_DRAW>& removed, const AroundMap& aroundMap = ::aroundMap, int* newRectStart = nullptr) const {
		{
			static CapArr<s32, CAP_VALID> removeRis;
			removeRis.clear();

			REP(ri, rects.size()) {
				const auto& rect = rects[ri];
				if (!CanDrawPoint3(rect)) {
					removeRis.push_back(ri);
				}
			}
			RREP(i, removeRis.size()) {
				rects.RemoveSlide(removeRis[i]);
			}
		}

		if (newRectStart) {
			*newRectStart = rects.size();
		}

		{
			auto Push = [&](int p0, int p1, int p2, int p3) {
				for (const auto& rect : rects) {
					if (p0 == rect.ps[0] && p1 == rect.ps[1]) {
						return;
					}
				}
				Rect* r = rects.PushBack();
				r->ps[0] = p0;
				r->ps[1] = p1;
				r->ps[2] = p2;
				r->ps[3] = p3;
			};

			{
				static EdgeCheckMap edgeCheckMap;
				edgeCheckMap.Init(N_);

				auto FindPointIndex = [&](int cur, int dir, int& index, int indexDir) {
					while (true) {
						cur = aroundMap[cur][dir];
						if (cur < 0) {
							break;
						}
						index += indexDir;
						if (points_.test(cur)) {
							break;
						}
					}
					return cur;
				};

				auto FindEdgePointIndex = [&](int cur, int dir, int& index, int indexDir) {
					Dir back = Back((Dir)dir);
					while (true) {
						cur = aroundMap[cur][dir];
						if (cur < 0) {
							break;
						}
						edgeCheckMap.SetEdge(cur, back);
						index += indexDir;
						if (points_.test(cur)) {
							break;
						}
					}
					return cur;
				};

				auto CheckBackRect = [&](int p0, int p1, int dir12, Dir dir23, int limitLength, bool rotateR, int limitlowerLength) {
					int p2 = -1;
					int cur = p1;
					Dir back12 = Back((Dir)dir12);
					REP(i, limitLength) {
						cur = aroundMap[cur][dir12];
						if (cur < 0) {
							return;
						}
						if (TestEdge(cur, back12)) {
							return;
						}
						if (points_.test(cur)) {
							if (i + 1 < limitlowerLength) {
								return;
							}
							p2 = cur;
							break;
						}
					}
					if (p2 < 0) {
						return;
					}
					int p3 = gs.CounterCell(p1, p0, p2);
					if (p3 < 0) {
						return;
					}
					if (!aroundMap.IsIn(p3)) {
						return;
					}
					if (points_.test(p3)) {
						return;
					}
					if (CanConnect(p2, p3, dir23, aroundMap)) {
						if (rotateR) {
							Push(p3, p0, p1, p2);
						}
						else {
							Push(p3, p2, p1, p0);
						}
					}
				};

				auto CheckOnEdgeRect = [&](int p0, int p1, int p2, Dir dir03, Dir dir23) {
					int p3 = gs.CounterCell(p1, p0, p2);
					if (p3 < 0) {
						return;
					}
					if (!aroundMap.IsIn(p3)) {
						return;
					}
					if (points_.test(p3)) {
						return;
					}
					if (CanConnect(p0, p3, dir03, aroundMap) &&
						CanConnect(p2, p3, dir23, aroundMap)) {
						Push(p3, p0, p1, p2);
					}
				};

				auto TryLine = [&](
					int backCell, int backIndex, int backLeftCell, int backLeftIndex, int backRightCell, int backRightIndex,
					int nextCell, int nextIndex, int nextLeftCell, int nextLeftIndex, int nextRightCell, int nextRightIndex,
					Dir front, Dir back, Dir left, Dir right) {
					if (backCell >= 0) {
						int limitLength = (nextCell >= 0) ? (nextIndex - backIndex - 1) : 999;
						if (backLeftCell >= 0) {
							CheckBackRect(backCell, backLeftCell, (int)front, right, limitLength, true, 0);
						}
						if (backRightCell >= 0) {
							CheckBackRect(backCell, backRightCell, (int)front, left, limitLength, false, 0);
						}
					}

					if (nextCell >= 0) {
						int limitLength = (backCell >= 0) ? (nextIndex - backIndex - 1) : 999;
						if (nextLeftCell >= 0) {
							CheckBackRect(nextCell, nextLeftCell, (int)back, right, limitLength, false, 0);
						}
						if (nextRightCell >= 0) {
							CheckBackRect(nextCell, nextRightCell, (int)back, left, limitLength, true, 0);
						}
					}

					if (backCell >= 0 && nextCell >= 0) {
						auto Judge = [&](int backCell, int backIndex, int nextCell, int nextIndex) {
							if (backCell >= 0) {
								if (nextCell >= 0) {
									if (backIndex <= nextIndex) {
										return -1;
									}
									else {
										return +1;
									}
								}
								else {
									return -1;
								}
							}
							else {
								if (nextCell >= 0) {
									return +1;
								}
								else {
									return 0;
								}
							}
						};
						{
							int judge = Judge(backLeftCell, backLeftIndex, nextLeftCell, nextLeftIndex);
							if (judge == -1) {
								CheckOnEdgeRect(nextCell, backCell, backLeftCell, left, front);
							}
							else if (judge == 1) {
								CheckOnEdgeRect(nextLeftCell, nextCell, backCell, back, left);
							}
						}

						{
							int judge = Judge(backRightCell, backRightIndex, nextRightCell, nextRightIndex);
							if (judge == -1) {
								CheckOnEdgeRect(backRightCell, backCell, nextCell, front, right);
							}
							else if (judge == 1) {
								CheckOnEdgeRect(backCell, nextCell, nextRightCell, right, back);
							}
						}
					}
				};

				for (const auto& r : removed) {
					REP(i, 4) {

						int j = (i + 1) & 3;
						int from = r.ps[i];
						int to = r.ps[j];
						int endIndex = 0;
						Dir front = gs.CalcDirAndDist(from, to, endIndex);
						Dir back = Back(front);
						Dir left = Rotate90L(front);
						Dir right = Rotate90R(front);
						int ifront = (int)front;
						int iback = (int)back;
						int ileft = (int)left;
						int iright = (int)right;


						int backCell = -1;
						int backIndex = 0;
						int backLeftCell = -1;
						int backLeftIndex = 0;
						int backRightCell = -1;
						int backRightIndex = 0;
						bool nextTry = !edgeCheckMap.TestEdge(from, front);
						if (nextTry) {
							if (points_.test(from)) {
								backCell = from;
							}
							else {
								backCell = FindEdgePointIndex(from, iback, backIndex, -1);
							}
							if (backCell >= 0) {
								if (aroundMap[backCell][ileft] >= 0 && !TestEdge(backCell, left)) {
									backLeftCell = FindPointIndex(backCell, ileft, backLeftIndex, 1);
								}
								if (aroundMap[backCell][iright] >= 0 && !TestEdge(backCell, right)) {
									backRightCell = FindPointIndex(backCell, iright, backRightIndex, 1);
								}
							}
						}

						int curCell = from;
						int curIndex = 0;
						int nextCell = -1;
						int nextIndex = 0;
						int nextLeftCell = -1;
						int nextLeftIndex = 0;
						int nextRightCell = -1;
						int nextRightIndex = 0;
						while (true) {
							bool curTry = nextTry;
							nextIndex = curIndex;
							if (curTry) {
								nextCell = FindEdgePointIndex(curCell, ifront, nextIndex, 1);
							}
							else {
								nextCell = FindPointIndex(curCell, ifront, nextIndex, 1);
							}
							nextTry = (nextCell >= 0 && nextIndex < endIndex && !edgeCheckMap.TestEdge(nextCell, front));

							if (nextCell >= 0) {
								if (curTry || nextTry) {
									nextLeftCell = -1;
									nextRightCell = -1;
									nextLeftIndex = 0;
									nextRightIndex = 0;
									if (aroundMap[nextCell][ileft] >= 0 && !TestEdge(nextCell, left)) {
										nextLeftCell = FindPointIndex(nextCell, ileft, nextLeftIndex, 1);
									}
									if (aroundMap[nextCell][iright] >= 0 && !TestEdge(nextCell, right)) {
										nextRightCell = FindPointIndex(nextCell, iright, nextRightIndex, 1);
									}
								}
							}

							if (curTry) {
								TryLine(
									backCell, backIndex, backLeftCell, backLeftIndex, backRightCell, backRightIndex,
									nextCell, nextIndex, nextLeftCell, nextLeftIndex, nextRightCell, nextRightIndex,
									front, back, left, right);
							}
							if (nextCell < 0 || nextIndex >= endIndex) {
								break;
							}

							curCell = nextCell;
							curIndex = nextIndex;
							backCell = nextCell;
							backIndex = nextIndex;
							backLeftCell = nextLeftCell;
							backLeftIndex = nextLeftIndex;
							backRightCell = nextRightCell;
							backRightIndex = nextRightIndex;
						}
					}

					Dir baseDir = gs.CalcDir(r.ps[0], r.ps[1]);
					const Dir dir45 = Rotate45R(baseDir);
					const int dp = r.DrawPoint();

					{


						REP(loop, 2) {
							Dir front = (loop == 0) ? dir45 : Rotate90R(dir45);
							Dir back = Back(front);

							if (aroundMap[dp][(int)front] >= 0) {
								if (TestEdge(dp, front)) {
									continue;
								}
								if (edgeCheckMap.TestEdge(dp, front)) {
									continue;
								}
							}
							else if (aroundMap[dp][(int)back] >= 0) {
								if (TestEdge(dp, back)) {
									continue;
								}
								if (edgeCheckMap.TestEdge(dp, back)) {
									continue;
								}
							}

							Dir left = Rotate90L(front);
							Dir right = Rotate90R(front);
							int ifront = (int)front;
							int iback = (int)back;
							int ileft = (int)left;
							int iright = (int)right;

							int backIndex = 0;
							int backCell = FindEdgePointIndex(dp, iback, backIndex, -1);

							int nextIndex = 0;
							int nextCell = FindEdgePointIndex(dp, ifront, nextIndex, 1);

							int backLeftCell = -1;
							int backLeftIndex = 0;
							int backRightCell = -1;
							int backRightIndex = 0;
							if (backCell >= 0) {
								if (aroundMap[backCell][ileft] >= 0 && !TestEdge(backCell, left)) {
									backLeftCell = FindPointIndex(backCell, ileft, backLeftIndex, 1);
								}
								if (aroundMap[backCell][iright] >= 0 && !TestEdge(backCell, right)) {
									backRightCell = FindPointIndex(backCell, iright, backRightIndex, 1);
								}
							}
							int nextLeftCell = -1;
							int nextRightCell = -1;
							int nextLeftIndex = 0;
							int nextRightIndex = 0;
							if (nextCell >= 0) {
								if (aroundMap[nextCell][ileft] >= 0 && !TestEdge(nextCell, left)) {
									nextLeftCell = FindPointIndex(nextCell, ileft, nextLeftIndex, 1);
								}
								if (aroundMap[nextCell][iright] >= 0 && !TestEdge(nextCell, right)) {
									nextRightCell = FindPointIndex(nextCell, iright, nextRightIndex, 1);
								}
							}

							TryLine(
								backCell, backIndex, backLeftCell, backLeftIndex, backRightCell, backRightIndex,
								nextCell, nextIndex, nextLeftCell, nextLeftIndex, nextRightCell, nextRightIndex,
								front, back, left, right);
						}
					}
				}
			}
		}
	}

	int AroundEdgeCount(int cell) const {
		int c = 0;
		REP(dir, 8) {
			int next = aroundMap[cell][dir];
			if (next < 0) {
				continue;
			}
			if (TestEdge(cell, (Dir)dir)) {
				++c;
			}
		}
		return c;
	}
};



struct IOServer {
	int N;
	int NN;
	int M;
	CapArr<s32, M_MAX> marks;
	Scorer baseScorer_;		
	CapArr<int, NN_MAX> weights_;

	int initL;
	int initR;
	int initT;
	int initB;

	int initAreaSize;		
	double initAreaMRate;	


	void InitInput() {
		istream& is = cin;
		is >> N >> M;
		timer.Init();
		NN = N * N;
		gs.Init(N);
		aroundMap.Init(N);
		baseScorer_.Init(N, M);

		initL = N / 4;
		initR = N * 3 / 4;
		initT = initL;
		initB = initR;
		limitAroundMap.Init(N, initL, initT, initR, initB);

		initAreaSize = initR - initL + 1;
		initAreaMRate = M / (double)square(initAreaSize);

		marks.resize(M);
		pint p;
		REP(i, M) {
			is >> p.x >> p.y;
			p.y = N - 1 - p.y;
			VASSERT(p.x >= initL && p.x <= initR && p.y >= initT && p.y <= initB);
			marks[i] = gs.To1D(p);
			baseScorer_.AddMark(marks[i]);
		}

		weights_.resize(NN);
		int C = (N - 1) / 2;
		REP(i, NN) {
			auto p = gs.To2D(i);
			weights_[i] = square(p.x - C) + square(p.y - C) + 1;
		}

	}

	void Output(const Result& result) {
		{
			ostream& os = cout;
			os << result.rects.size() << endl;
			pint p;
			REP(i, result.rects.size()) {
				const auto& rect = result.rects[i];
				REP(j, 4) {
					p = gs.To2D(rect.ps[j]);
					p.y = N - 1 - p.y;
					os << p.x << " " << p.y << " ";
				}
				os << endl;
			}
		}


	}

	void Finalize() {
	}


	int InitialWeight() const {
		return baseScorer_.Q_;
	}
	int Weight(int i) const {
		return weights_[i];
	}
	double Score(int totalWeight) const {
		return baseScorer_.Score(totalWeight);
	}
};
IOServer server;


constexpr double LinearParam(double ax, double ay, double bx, double by, double cx) {
	double r = (cx - ax) / (bx - ax);
	double cy = ay + (by - ay) * r;
	return cy;
}

int LinearParamInt(double ax, double ay, double bx, double by, double cx) {
	return (int)round(LinearParam(ax, ay, bx, by, cx));
}


double LinearParamM(double ay, double by) {
	return LinearParam(M_MIN, ay, M_MAX, by, server.M);
}

int LinearParamIntM(double ay, double by) {
	return LinearParamInt(M_MIN, ay, M_MAX, by, server.M);
}

#define CAT_NAME(a, b) a ## b
#define LINEAR_M_DOUBLE(name) static const double name = LinearParamM(HP.CAT_NAME(name,_L), HP.CAT_NAME(name,_R));
#define LINEAR_M_INT(name) static const int name = LinearParamIntM(HP.CAT_NAME(name,_L), HP.CAT_NAME(name,_R));

#define USE_SA_POINT_FILTER 1
#define USE_SA_ROLLBACK 1		

#define VIS_MAX_STATE 0
#define VIS_TRANSITION 0

#define SKIP_CHECK 0
#define DISP_DELETE_COUNT 0
#define DISP_VALID_COUNT 0




template <int CAP_NN, int CAP_VALID>
void EnumValidRectForInit(GridState<CAP_NN>& grid, CapArr<Rect, CAP_VALID>& validRects, const AroundMap& aroundMap = ::aroundMap) {
	grid.EnumValidRectForInit(validRects, aroundMap);
}

template <int CAP_NN, int CAP_VALID>
void EnumValidRect(GridState<CAP_NN>& grid, CapArr<Rect, CAP_VALID>& validRects) {
	grid.EnumValidRect(validRects);
}

template <int CAP_NN, int CAP_VALID>
void UpdateValidRect(GridState<CAP_NN>& grid, CapArr<Rect, CAP_VALID>& validRects, const Rect& rect, const AroundMap& aroundMap = ::aroundMap, int* newRectStart = nullptr) {
	grid.UpdateValidRect(validRects, rect, aroundMap, newRectStart);
}

template <int CAP_NN, int CAP_VALID, int CAP_DRAW>
void RemovedUpdateValidateRects(GridState<CAP_NN>& grid, CapArr<Rect, CAP_VALID>& validRects, const CapArr<Rect, CAP_DRAW>& removeRects, const AroundMap& aroundMap = ::aroundMap, int* newRectStart = nullptr) {
	grid.RemovedUpdateValidateRects(validRects, removeRects, aroundMap, newRectStart);
}



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
	double* totalMaxScore_ = nullptr;

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
			if (exp((newScore - currentScore) * divTemp) * UINT32_MAX > Random::Get(UINT32_MAX)) {
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
auto MakeTransition(const char* name, F&& func, int weight) {
	return SATransition<F>{ name, func, weight };
}
#define MAKE_TRANS(func, weight) MakeTransition(#func, [&](SAChecker& sac, State& state) { func(sa, sac, state); }, weight)

struct SimulatedAnnealing {
	vector<SAChecker> checkers;

	double totalMaxScore = 0;				
	double timeRate = 0;				
	double temp = 1;					
	double divTemp = 1;					


	Xor64 rand_;

	double startTemp_ = 200;					
	double endTemp_ = 1;						
	int stepLoopCount = 1000;					

	double rollbackStartRate_ = 999.0;			
	int rollbackCount_ = INT_MAX;				
	double rollbackNextMulti_ = 1.1;			


public:
	template <class STATE, class... TRANSITION>
	void Run2(ChronoTimer& timer, vector<STATE>& states, tuple<SATransition<TRANSITION>...>& transitions) {

		vector<STATE> maxStates = states;
		checkers.resize(states.size());
		totalMaxScore = states[0].EvalScore();
		REP(pi, checkers.size()) {
			auto& checker = checkers[pi];
			checker.totalMaxScore_ = &totalMaxScore;

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

		const auto startTime = timer.Now();
		const auto endTime = timer.EndTime();
		const double subTimeCountDiv = 1.0 / (double)(endTime - startTime).count();

		vector<int> pis(states.size());
		iota(ALL(pis), 0);

		bool forceEnd = false;
		while (!timer.IsOver()) {
			timeRate = (timer.Now() - startTime).count() * subTimeCountDiv;
			temp = startTemp_ * std::pow(endTemp_ / startTemp_, timeRate);		
			divTemp = 1.0 / temp;
			for (auto& checker : checkers) {
				checker.divTemp = divTemp;
			}


			for (int rp = 0; rp < stepLoopCount; ++rp) {
				int ti = (int)randTable(rand_);

				auto exec = [&](auto&& e, size_t i) {
					for (int pi : pis) {
						auto& checker = checkers[pi];
						e.func(checker, states[pi]);

						if (states[pi].RawScore() > maxStates[pi].RawScore()) {
							maxStates[pi] = states[pi];
						}
						else {
							if (timeRate >= rollbackStartRate_) {
								if (checker.noMaxUpdateCount >= checker.nextRollbackCheckCount) {
									states[pi] = maxStates[pi];
									checker.noMaxUpdateCount = 0;
									checker.nextRollbackCheckCount = (int)round(checker.nextRollbackCheckCount * rollbackNextMulti_);
								}
							}
						}
					}
				};

				TupleAccess(transitions, ti, exec);
			}
			if (forceEnd) {
				break;
			}

			{
				constexpr double start = 0.2;
				constexpr double end = 1.0;
				int targetPointCount = (int)states.size();
				if (timeRate >= end) {
					targetPointCount = 1;
				}
				else if (timeRate >= start) {
					double r = 1.0 - (timeRate - start) / (end - start);		
					targetPointCount = 1 + (int)floor(states.size() * r);
				}
				if ((int)pis.size() > targetPointCount) {
					sort(ALL(pis), [&](int a, int b) {
						return checkers[a].maxScore > checkers[b].maxScore;
					});
					pis.resize(targetPointCount);
				}
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



#define USE_CALLOC 0

template <class PointData, class EdgeData>
struct GridMap {
private:
	static_assert(is_trivially_copyable<PointData>::value);
	static_assert(is_trivially_copyable<EdgeData>::value);

	vector<PointData> points_;
	vector<array<EdgeData, 4>> edges_;

private:
	void ConvertEdgeIndex(s32& cell, Dir& dir) const {
		if ((s8)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
	}

public:

public:
	void Init(int size) {
		points_.resize(size);
		edges_.resize(size);
	}

	PointData& Point(s32 cell) {
		return points_[cell];
	}
	const PointData& Point(s32 cell) const {
		return points_[cell];
	}
	EdgeData& Edge(s32 cell, Dir dir) {
		if ((s8)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		return edges_[cell][(int)dir];
	}
	const EdgeData& Edge(s32 cell, Dir dir) const {
		if ((s8)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		return edges_[cell][(int)dir];
	}


};


struct EdgeBitset {
	bitset<NN_MAX * 4> edges_;

	bool TestEdge(s32 cell, Dir dir) const {
		VASSERT(aroundMap[cell][(int)dir] >= 0);
		if ((s8)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		return edges_.test((cell << 2) | (s32)dir);
	}

	void SetEdge(s32 cell, Dir dir) {
		VASSERT(aroundMap[cell][(int)dir] >= 0);
		if ((s8)dir >= 4) {
			cell = aroundMap[cell][(int)dir];
			dir = Back(dir);
		}
		edges_.set((cell << 2) | (s32)dir);
	}
};


#define VIS_DEPEND_MAP 0
#define VIS_NG_EDGE_MAP 0
#define VIS_REQUIRE_POINT_MAP 0

constexpr int DependRectMax = 3600;		

constexpr array<int, 62> DependRectCountTbl = {
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	-1,    
	899,    
	960,    
	1023,    
	1088,    
	1155,    
	1224,    
	1295,    
	1368,    
	1443,    
	1520,    
	1599,    
	1680,    
	1763,    
	1848,    
	1935,    
	2024,    
	2115,    
	2208,    
	2303,    
	2400,    
	2499,    
	2600,    
	2703,    
	2808,    
	2915,    
	3024,    
	3135,    
	3248,    
	3363,    
	3480,    
	3599,    
};

struct DependMap {
private:
	using PointData = CapArr<u16, 6>;
	using EdgeData = CapArr<u16, 2>;
	static constexpr int PointDataSize = sizeof(PointData);
	static constexpr int EdgeDataSize = sizeof(EdgeData);

	GridMap<PointData, EdgeData> dependMap_;
	CapArr<Rect, DependRectMax> rects_;
	EdgeBitset ngEdgeMap_;
	bitset<NN_MAX> requirePointMap_;

public:
	int RectCount() const {
		return rects_.size();
	}
	const CapArr<Rect, DependRectMax>& Rects() const {
		return rects_;
	}
	PointData& Point(s32 cell) {
		return dependMap_.Point(cell);
	}
	const PointData& Point(s32 cell) const {
		return dependMap_.Point(cell);
	}
	EdgeData& Edge(s32 cell, Dir dir) {
		return dependMap_.Edge(cell, dir);
	}
	const EdgeData& Edge(s32 cell, Dir dir) const {
		return dependMap_.Edge(cell, dir);
	}

	bool IsNgEdge(s32 cell, Dir dir) const {
		return ngEdgeMap_.TestEdge(cell, dir);
	}
	bool IsRequirePoint(s32 cell) const {
		return requirePointMap_.test(cell);
	}

	void Init(bool flagH, bool flagX, bool flagY) {
		dependMap_.Init(server.NN);

		const int N = server.N;
		const int C = N / 2;

		int baseX = server.initL;
		int baseY = server.initT;

		auto AddRect = [&](int x, int y, Dir edgeDir) -> bool {
			if (gs.IsOut(x, y)) {
				return false;
			}
			Rect rect = {};
			array<Dir, 4> es = {};
			rect.ps[0] = gs.To1D(x, y);
			es[0] = edgeDir;
			FOR(i, 1, 4) {
				rect.ps[i] = aroundMap[rect.ps[i - 1]][(int)es[i - 1]];
				if (rect.ps[i] < 0) {
					return false;
				}
				es[i] = Rotate90R(es[i - 1]);
			}
			REP(i, 4) {
				dependMap_.Point(rect.ps[i]).push_back(rects_.size());
				dependMap_.Edge(rect.ps[i], es[i]).push_back(rects_.size());
			}
			rects_.push_back(rect);

			return true;
		};

		REP(y, N) {
			REP(x, N) {
				if (((x & 1) != (y & 1)) == flagH) {
					if (AddRect(x, y, Dir::R)) {
						if (y >= server.initT && y + 1 <= server.initB) {
							if (x + 1 == server.initL) {
								ngEdgeMap_.SetEdge(gs.To1D(x + 1, y), Dir::D);
							}
							else if (x == server.initR) {
								ngEdgeMap_.SetEdge(gs.To1D(x, y), Dir::D);
							}
						}
						if (x >= server.initL && x + 1 <= server.initR) {
							if (y + 1 == server.initT) {
								ngEdgeMap_.SetEdge(gs.To1D(x, y + 1), Dir::R);
							}
							else if (y == server.initB) {
								ngEdgeMap_.SetEdge(gs.To1D(x, y), Dir::R);
							}
						}
					}
				}

				int baseRx = abs(x - baseX);
				int baseRy = abs(y - baseY);

				int baseCx = abs(x - C);
				int baseCy = abs(y - C);

				bool add = false;
				if (baseCx >= baseCy) {
					if ((baseRy & 1) == flagX) {
						add = true;
					}
				}
				if (baseCx <= baseCy) {
					if ((baseRx & 1) == flagY) {
						add = true;
					}
				}

				if (add) {
					if (AddRect(x, y - 1, Dir::RD)) {
						if (y - 1 >= server.initT && y + 1 <= server.initB) {
							if (x == server.initL) {
								ngEdgeMap_.SetEdge(gs.To1D(x, y - 1), Dir::RD);
								ngEdgeMap_.SetEdge(gs.To1D(x, y + 1), Dir::RU);
								requirePointMap_.set(gs.To1D(x + 1, y));
							}
							else if (x == server.initR) {
								ngEdgeMap_.SetEdge(gs.To1D(x, y - 1), Dir::LD);
								ngEdgeMap_.SetEdge(gs.To1D(x, y + 1), Dir::LU);
								requirePointMap_.set(gs.To1D(x - 1, y));
							}
						}

						if (x - 1 >= server.initL && x + 1 <= server.initR) {
							if (y == server.initT) {
								ngEdgeMap_.SetEdge(gs.To1D(x - 1, y), Dir::RD);
								ngEdgeMap_.SetEdge(gs.To1D(x + 1, y), Dir::LD);
								requirePointMap_.set(gs.To1D(x, y + 1));
							}
							else if (y == server.initB) {
								ngEdgeMap_.SetEdge(gs.To1D(x - 1, y), Dir::RU);
								ngEdgeMap_.SetEdge(gs.To1D(x + 1, y), Dir::LU);
								requirePointMap_.set(gs.To1D(x, y - 1));
							}
						}
					}
				}
			}
		}


	}
};
constexpr int DependMapSize = sizeof(DependMap);

struct DependMaps {
public:
	static DependMap& Instance(int index) {
		static array<DependMap, 8> instance;
		return instance[index];
	}

	static void Init() {
		REP(i, 8) {
			bool flagH = (i & 1);
			bool flagX = (i & 2);
			bool flagY = (i & 4);
			Instance(i).Init(flagH, flagX, flagY);
		}
	}

};


struct ScoreGridParam {
	array<double, 4> scoreTbl;
	double complete;
	double penalty;

	void Init() {
		LINEAR_M_DOUBLE(PointScoreComplete);
		LINEAR_M_DOUBLE(PointScorePenalty);
		LINEAR_M_DOUBLE(PointScoreP2);
		LINEAR_M_DOUBLE(PointScoreP3);

		complete = PointScoreComplete;
		penalty = PointScorePenalty;
		scoreTbl = {
			0,
			2,		
			PointScoreP2,
			PointScoreP3,
		};
	}
};
ScoreGridParam g_scoreGridParam;

template <int CAP_NN, int CAP_DEPEND_RECT>
struct ScoreGrid {
private:
	struct PointState {
		u16 weightCell;		
		u8 edgeCount;		
		u8 pointCount;		

		void Init(u16 bestWeightCell) {
			weightCell = bestWeightCell;
			edgeCount = 0;
			pointCount = 0;
		}

		double Score() const {
			if (pointCount == 4) {
				return g_scoreGridParam.complete * server.Weight(weightCell);
			}
			return (g_scoreGridParam.scoreTbl[pointCount] - (edgeCount ? g_scoreGridParam.penalty : 0.0)) * server.Weight(weightCell);
		}

		double AddEdgeDiff(int add) {
			VASSERT(add == -1 || add == 1);

			int oldEdgeCount = edgeCount;
			edgeCount += add;

			if (pointCount == 4) {
				return 0;
			}
			VASSERT(edgeCount >= 0 && edgeCount <= 4);

			if (oldEdgeCount == 0 && edgeCount > 0) {
				return -g_scoreGridParam.penalty * server.Weight(weightCell);
			}
			if (oldEdgeCount > 0 && edgeCount == 0) {
				return g_scoreGridParam.penalty * server.Weight(weightCell);
			}
			return 0;
		}
		double AddPointDiff(int add) {
			VASSERT(add == -1 || add == 1);

			int oldPointCount = pointCount;
			pointCount += add;
			VASSERT(pointCount >= 0 && pointCount <= 4);

			double weight = server.Weight(weightCell);

			if (oldPointCount == 4 && pointCount < 4) {
				double before = g_scoreGridParam.complete * weight;
				double after = (g_scoreGridParam.scoreTbl[pointCount] - (edgeCount ? g_scoreGridParam.penalty : 0.0)) * weight;
				return after - before;
			}
			if (oldPointCount < 4 && pointCount == 4) {
				double before = (g_scoreGridParam.scoreTbl[oldPointCount] - (edgeCount ? g_scoreGridParam.penalty : 0.0)) * weight;
				double after = g_scoreGridParam.complete * weight;
				return after - before;
			}

			return (g_scoreGridParam.scoreTbl[pointCount] - g_scoreGridParam.scoreTbl[oldPointCount]) * weight;
		}
	};

	int pattern_ = -1;
	CapArr<PointState, CAP_DEPEND_RECT> pointStates_;
	double totalPointScore_ = 0;

public:
	double Score() const {
		return totalPointScore_;
	}

public:
	void InitNoRect(int pattern) {
		pattern_ = pattern;
		const auto& dependMap = DependMaps::Instance(pattern_);
		const auto& rects = dependMap.Rects();

		const int C = server.N / 2;
		auto Weight = [&](int cell) -> double {
			return server.Weight(cell);		
		};

		pointStates_.resize(rects.size());
		REP(i, pointStates_.size()) {
			const auto& rect = rects[i];
			double maxWeight = 0;
			u16 bestCell = 0;
			REP(j, 4) {
				double w = Weight(rect.ps[j]);
				if (w > maxWeight) {
					maxWeight = w;
					bestCell = rect.ps[j];
				}
			}
			pointStates_[i].Init(bestCell);
		}
		totalPointScore_ = 0;

		for (int cell : server.marks) {
			for (int depend : dependMap.Point(cell)) {
				totalPointScore_ += pointStates_[depend].AddPointDiff(1);
			}
		}
	}

	void DrawRect(const Rect& rect) {
		const auto& dependMap = DependMaps::Instance(pattern_);

		auto DrawLine = [&](s32 startCell, s32 endCell) {
			Dir dir = gs.CalcDir(startCell, endCell);
			s32 curCell = startCell;
			while (true) {
				for (int depend : dependMap.Edge(curCell, dir)) {
					totalPointScore_ += pointStates_[depend].AddEdgeDiff(1);
				}
				int nextCell = aroundMap[curCell][(int)dir];
				VASSERT(nextCell >= 0);
				if (nextCell == endCell) {
					break;
				}
				curCell = nextCell;
			}
		};

		int dp = rect.DrawPoint();
		for (int depend : dependMap.Point(dp)) {
			totalPointScore_ += pointStates_[depend].AddPointDiff(1);
		}
		DrawLine(rect.ps[0], rect.ps[1]);
		DrawLine(rect.ps[1], rect.ps[2]);
		DrawLine(rect.ps[2], rect.ps[3]);
		DrawLine(rect.ps[3], rect.ps[0]);
	}

	void RemoveRect(const Rect& rect) {
		const auto& dependMap = DependMaps::Instance(pattern_);

		auto RemoveLine = [&](s32 startCell, s32 endCell) {
			Dir dir = gs.CalcDir(startCell, endCell);
			s32 curCell = startCell;
			while (true) {
				for (int depend : dependMap.Edge(curCell, dir)) {
					totalPointScore_ += pointStates_[depend].AddEdgeDiff(-1);
				}
				int nextCell = aroundMap[curCell][(int)dir];
				VASSERT(nextCell >= 0);
				if (nextCell == endCell) {
					break;
				}
				curCell = nextCell;
			}
		};

		int dp = rect.DrawPoint();
		RemoveLine(rect.ps[0], rect.ps[1]);
		RemoveLine(rect.ps[1], rect.ps[2]);
		RemoveLine(rect.ps[2], rect.ps[3]);
		RemoveLine(rect.ps[3], rect.ps[0]);
		for (int depend : dependMap.Point(dp)) {
			totalPointScore_ += pointStates_[depend].AddPointDiff(-1);
		}
	}

	double CalcDrawedScore(const Rect& rect) {
		const auto& dependMap = DependMaps::Instance(pattern_);

		static CapArr<pint, CAP_NN> lastPointCounts;		
		static CapArr<pint, CAP_NN> lastEdgeCounts;			
		lastPointCounts.clear();
		lastEdgeCounts.clear();

		double diffScore = 0;
		auto DrawLine = [&](s32 curCell, s32 endCell, Dir dir) {
			while (true) {
				for (int depend : dependMap.Edge(curCell, dir)) {
					auto* p = lastEdgeCounts.PushBack();
					p->a = depend;
					p->b = pointStates_[depend].edgeCount;
					diffScore += pointStates_[depend].AddEdgeDiff(1);
				}
				int nextCell = aroundMap[curCell][(int)dir];
				VASSERT(nextCell >= 0);
				if (nextCell == endCell) {
					break;
				}
				curCell = nextCell;
			}
		};

		int dp = rect.DrawPoint();
		for (int depend : dependMap.Point(dp)) {
			auto* p = lastPointCounts.PushBack();
			p->a = depend;
			p->b = pointStates_[depend].pointCount;
			diffScore += pointStates_[depend].AddPointDiff(1);
		}
		Dir dir = gs.CalcDir(rect.ps[0], rect.ps[1]);
		DrawLine(rect.ps[0], rect.ps[1], dir);
		dir = Rotate90R(dir);
		DrawLine(rect.ps[1], rect.ps[2], dir);
		dir = Rotate90R(dir);
		DrawLine(rect.ps[2], rect.ps[3], dir);
		dir = Rotate90R(dir);
		DrawLine(rect.ps[3], rect.ps[0], dir);

		RREP(i, lastEdgeCounts.size()) {
			const auto& p = lastEdgeCounts[i];
			pointStates_[p.a].edgeCount = p.b;
		}
		RREP(i, lastPointCounts.size()) {
			const auto& p = lastPointCounts[i];
			pointStates_[p.a].pointCount = p.b;
		}


		return diffScore;		
	}

};

constexpr int RangeWeightTable[16][31][31] = {
    {    
        {0, 0, 131, 249, 493, 717, 1072, 1406, 1886, 2350, 2985, 3615, 4451, 5299, 6398, 7532, 8715},
        {0, 0, 0, 118, 225, 449, 657, 991, 1310, 1774, 2230, 2860, 3495, 4343, 5215, 6349, 7532},
        {0, 0, 0, 0, 107, 205, 413, 609, 928, 1238, 1694, 2150, 2785, 3435, 4307, 5215, 6398},
        {0, 0, 0, 0, 0, 98, 189, 385, 573, 883, 1190, 1646, 2110, 2760, 3435, 4343, 5299},
        {0, 0, 0, 0, 0, 0, 91, 177, 365, 549, 856, 1166, 1630, 2110, 2785, 3495, 4451},
        {0, 0, 0, 0, 0, 0, 0, 86, 169, 353, 537, 847, 1166, 1646, 2150, 2860, 3615},
        {0, 0, 0, 0, 0, 0, 0, 0, 83, 165, 349, 537, 856, 1190, 1694, 2230, 2985},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 82, 165, 353, 549, 883, 1238, 1774, 2350},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 169, 365, 573, 928, 1310, 1886},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 86, 177, 385, 609, 991, 1406},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 91, 189, 413, 657, 1072},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 98, 205, 449, 717},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 107, 225, 493},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 118, 249},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 131, 249, 493, 717, 1072, 1406, 1886, 2350, 2985, 3615, 4451, 5299, 6398, 7532, 8972},
        {0, 0, 0, 118, 225, 449, 657, 991, 1310, 1774, 2230, 2860, 3495, 4343, 5215, 6349, 7532},
        {0, 0, 0, 0, 107, 205, 413, 609, 928, 1238, 1694, 2150, 2785, 3435, 4307, 5215, 6398},
        {0, 0, 0, 0, 0, 98, 189, 385, 573, 883, 1190, 1646, 2110, 2760, 3435, 4343, 5299},
        {0, 0, 0, 0, 0, 0, 91, 177, 365, 549, 856, 1166, 1630, 2110, 2785, 3495, 4451},
        {0, 0, 0, 0, 0, 0, 0, 86, 169, 353, 537, 847, 1166, 1646, 2150, 2860, 3615},
        {0, 0, 0, 0, 0, 0, 0, 0, 83, 165, 349, 537, 856, 1190, 1694, 2230, 2985},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 82, 165, 353, 549, 883, 1238, 1774, 2350},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 169, 365, 573, 928, 1310, 1886},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 86, 177, 385, 609, 991, 1406},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 91, 189, 413, 657, 1072},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 98, 205, 449, 717},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 107, 225, 493},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 118, 249},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 165, 315, 623, 907, 1352, 1770, 2362, 2930, 3695, 4445, 5425, 6405, 7658, 8932, 10532, 12180, 13892},
        {0, 0, 0, 150, 287, 571, 835, 1253, 1650, 2218, 2770, 3520, 4265, 5245, 6237, 7511, 8820, 10468, 12180},
        {0, 0, 0, 0, 137, 263, 527, 775, 1172, 1554, 2106, 2650, 3395, 4145, 5137, 6153, 7462, 8820, 10532},
        {0, 0, 0, 0, 0, 126, 243, 491, 727, 1109, 1482, 2026, 2570, 3320, 4085, 5101, 6153, 7511, 8932},
        {0, 0, 0, 0, 0, 0, 117, 227, 463, 691, 1064, 1434, 1978, 2530, 3295, 4085, 5137, 6237, 7658},
        {0, 0, 0, 0, 0, 0, 0, 110, 215, 443, 667, 1037, 1410, 1962, 2530, 3320, 4145, 5245, 6405},
        {0, 0, 0, 0, 0, 0, 0, 0, 105, 207, 431, 655, 1028, 1410, 1978, 2570, 3395, 4265, 5425},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 203, 427, 655, 1037, 1434, 2026, 2650, 3520, 4445},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 101, 203, 431, 667, 1064, 1482, 2106, 2770, 3695},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 207, 443, 691, 1109, 1554, 2218, 2930},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105, 215, 463, 727, 1172, 1650, 2362},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 227, 491, 775, 1253, 1770},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 243, 527, 835, 1352},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 263, 571, 907},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 137, 287, 623},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 150, 315},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 165},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 165, 315, 623, 907, 1352, 1770, 2362, 2930, 3695, 4445, 5425, 6405, 7658, 8932, 10532, 12180, 14217},
        {0, 0, 0, 150, 287, 571, 835, 1253, 1650, 2218, 2770, 3520, 4265, 5245, 6237, 7511, 8820, 10468, 12180},
        {0, 0, 0, 0, 137, 263, 527, 775, 1172, 1554, 2106, 2650, 3395, 4145, 5137, 6153, 7462, 8820, 10532},
        {0, 0, 0, 0, 0, 126, 243, 491, 727, 1109, 1482, 2026, 2570, 3320, 4085, 5101, 6153, 7511, 8932},
        {0, 0, 0, 0, 0, 0, 117, 227, 463, 691, 1064, 1434, 1978, 2530, 3295, 4085, 5137, 6237, 7658},
        {0, 0, 0, 0, 0, 0, 0, 110, 215, 443, 667, 1037, 1410, 1962, 2530, 3320, 4145, 5245, 6405},
        {0, 0, 0, 0, 0, 0, 0, 0, 105, 207, 431, 655, 1028, 1410, 1978, 2570, 3395, 4265, 5425},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 203, 427, 655, 1037, 1434, 2026, 2650, 3520, 4445},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 101, 203, 431, 667, 1064, 1482, 2106, 2770, 3695},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 207, 443, 691, 1109, 1554, 2218, 2930},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105, 215, 463, 727, 1172, 1650, 2362},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 227, 491, 775, 1253, 1770},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 243, 527, 835, 1352},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 263, 571, 907},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 137, 287, 623},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 150, 315},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 165},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 203, 389, 769, 1121, 1668, 2182, 2902, 3590, 4505, 5395, 6543, 7679, 9114, 10556, 12348, 14172, 16407, 18705, 21084},
        {0, 0, 0, 186, 357, 709, 1037, 1551, 2038, 2726, 3390, 4280, 5155, 6291, 7427, 8869, 10332, 12156, 14028, 16326, 18705},
        {0, 0, 0, 0, 171, 329, 657, 965, 1452, 1918, 2582, 3230, 4105, 4975, 6111, 7259, 8722, 10220, 12092, 14028, 16407},
        {0, 0, 0, 0, 0, 158, 305, 613, 905, 1371, 1822, 2470, 3110, 3980, 4855, 6003, 7175, 8673, 10220, 12156, 14172},
        {0, 0, 0, 0, 0, 0, 147, 285, 577, 857, 1308, 1750, 2390, 3030, 3905, 4795, 5967, 7175, 8722, 10332, 12348},
        {0, 0, 0, 0, 0, 0, 0, 138, 269, 549, 821, 1263, 1702, 2342, 2990, 3880, 4795, 6003, 7259, 8869, 10556},
        {0, 0, 0, 0, 0, 0, 0, 0, 131, 257, 529, 797, 1236, 1678, 2326, 2990, 3905, 4855, 6111, 7427, 9114},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 249, 517, 785, 1227, 1678, 2342, 3030, 3980, 4975, 6291, 7679},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 123, 245, 513, 785, 1236, 1702, 2390, 3110, 4105, 5155, 6543},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 122, 245, 517, 797, 1263, 1750, 2470, 3230, 4280, 5395},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 123, 249, 529, 821, 1308, 1822, 2582, 3390, 4505},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 257, 549, 857, 1371, 1918, 2726, 3590},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 269, 577, 905, 1452, 2038, 2902},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 138, 285, 613, 965, 1551, 2182},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 147, 305, 657, 1037, 1668},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 158, 329, 709, 1121},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 171, 357, 769},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 186, 389},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 203},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 203, 389, 769, 1121, 1668, 2182, 2902, 3590, 4505, 5395, 6543, 7679, 9114, 10556, 12348, 14172, 16407, 18705, 21485},
        {0, 0, 0, 186, 357, 709, 1037, 1551, 2038, 2726, 3390, 4280, 5155, 6291, 7427, 8869, 10332, 12156, 14028, 16326, 18705},
        {0, 0, 0, 0, 171, 329, 657, 965, 1452, 1918, 2582, 3230, 4105, 4975, 6111, 7259, 8722, 10220, 12092, 14028, 16407},
        {0, 0, 0, 0, 0, 158, 305, 613, 905, 1371, 1822, 2470, 3110, 3980, 4855, 6003, 7175, 8673, 10220, 12156, 14172},
        {0, 0, 0, 0, 0, 0, 147, 285, 577, 857, 1308, 1750, 2390, 3030, 3905, 4795, 5967, 7175, 8722, 10332, 12348},
        {0, 0, 0, 0, 0, 0, 0, 138, 269, 549, 821, 1263, 1702, 2342, 2990, 3880, 4795, 6003, 7259, 8869, 10556},
        {0, 0, 0, 0, 0, 0, 0, 0, 131, 257, 529, 797, 1236, 1678, 2326, 2990, 3905, 4855, 6111, 7427, 9114},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 249, 517, 785, 1227, 1678, 2342, 3030, 3980, 4975, 6291, 7679},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 123, 245, 513, 785, 1236, 1702, 2390, 3110, 4105, 5155, 6543},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 122, 245, 517, 797, 1263, 1750, 2470, 3230, 4280, 5395},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 123, 249, 529, 821, 1308, 1822, 2582, 3390, 4505},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 257, 549, 857, 1371, 1918, 2726, 3590},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 269, 577, 905, 1452, 2038, 2902},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 138, 285, 613, 965, 1551, 2182},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 147, 305, 657, 1037, 1668},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 158, 329, 709, 1121},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 171, 357, 769},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 186, 389},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 203},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 245, 471, 931, 1359, 2020, 2642, 3506, 4330, 5415, 6465, 7805, 9121, 10766, 12404, 14420, 16452, 18921, 21435, 24455, 27555, 30755},
        {0, 0, 0, 226, 435, 863, 1263, 1885, 2474, 3298, 4090, 5140, 6165, 7481, 8785, 10423, 12068, 14100, 16164, 18678, 21255, 24355, 27555},
        {0, 0, 0, 0, 209, 403, 803, 1179, 1768, 2330, 3122, 3890, 4915, 5925, 7229, 8533, 10178, 11844, 13908, 16020, 18597, 21255, 24455},
        {0, 0, 0, 0, 0, 194, 375, 751, 1107, 1669, 2210, 2978, 3730, 4740, 5745, 7049, 8365, 10031, 11732, 13844, 16020, 18678, 21435},
        {0, 0, 0, 0, 0, 0, 181, 351, 707, 1047, 1588, 2114, 2866, 3610, 4615, 5625, 6941, 8281, 9982, 11732, 13908, 16164, 18921},
        {0, 0, 0, 0, 0, 0, 0, 170, 331, 671, 999, 1525, 2042, 2786, 3530, 4540, 5565, 6905, 8281, 10031, 11844, 14100, 16452},
        {0, 0, 0, 0, 0, 0, 0, 0, 161, 315, 643, 963, 1480, 1994, 2738, 3490, 4515, 5565, 6941, 8365, 10178, 12068, 14420},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 303, 623, 939, 1453, 1970, 2722, 3490, 4540, 5625, 7049, 8533, 10423, 12404},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 149, 295, 611, 927, 1444, 1970, 2738, 3530, 4615, 5745, 7229, 8785, 10766},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 146, 291, 607, 927, 1453, 1994, 2786, 3610, 4740, 5925, 7481, 9121},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 145, 291, 611, 939, 1480, 2042, 2866, 3730, 4915, 6165, 7805},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 146, 295, 623, 963, 1525, 2114, 2978, 3890, 5140, 6465},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 149, 303, 643, 999, 1588, 2210, 3122, 4090, 5415},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 315, 671, 1047, 1669, 2330, 3298, 4330},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 161, 331, 707, 1107, 1768, 2474, 3506},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 170, 351, 751, 1179, 1885, 2642},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 181, 375, 803, 1263, 2020},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 194, 403, 863, 1359},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 209, 435, 931},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 226, 471},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 245},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 245, 471, 931, 1359, 2020, 2642, 3506, 4330, 5415, 6465, 7805, 9121, 10766, 12404, 14420, 16452, 18921, 21435, 24455, 27555, 31240},
        {0, 0, 0, 226, 435, 863, 1263, 1885, 2474, 3298, 4090, 5140, 6165, 7481, 8785, 10423, 12068, 14100, 16164, 18678, 21255, 24355, 27555},
        {0, 0, 0, 0, 209, 403, 803, 1179, 1768, 2330, 3122, 3890, 4915, 5925, 7229, 8533, 10178, 11844, 13908, 16020, 18597, 21255, 24455},
        {0, 0, 0, 0, 0, 194, 375, 751, 1107, 1669, 2210, 2978, 3730, 4740, 5745, 7049, 8365, 10031, 11732, 13844, 16020, 18678, 21435},
        {0, 0, 0, 0, 0, 0, 181, 351, 707, 1047, 1588, 2114, 2866, 3610, 4615, 5625, 6941, 8281, 9982, 11732, 13908, 16164, 18921},
        {0, 0, 0, 0, 0, 0, 0, 170, 331, 671, 999, 1525, 2042, 2786, 3530, 4540, 5565, 6905, 8281, 10031, 11844, 14100, 16452},
        {0, 0, 0, 0, 0, 0, 0, 0, 161, 315, 643, 963, 1480, 1994, 2738, 3490, 4515, 5565, 6941, 8365, 10178, 12068, 14420},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 303, 623, 939, 1453, 1970, 2722, 3490, 4540, 5625, 7049, 8533, 10423, 12404},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 149, 295, 611, 927, 1444, 1970, 2738, 3530, 4615, 5745, 7229, 8785, 10766},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 146, 291, 607, 927, 1453, 1994, 2786, 3610, 4740, 5925, 7481, 9121},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 145, 291, 611, 939, 1480, 2042, 2866, 3730, 4915, 6165, 7805},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 146, 295, 623, 963, 1525, 2114, 2978, 3890, 5140, 6465},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 149, 303, 643, 999, 1588, 2210, 3122, 4090, 5415},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 315, 671, 1047, 1669, 2330, 3298, 4330},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 161, 331, 707, 1107, 1768, 2474, 3506},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 170, 351, 751, 1179, 1885, 2642},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 181, 375, 803, 1263, 2020},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 194, 403, 863, 1359},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 209, 435, 931},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 226, 471},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 245},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 291, 561, 1109, 1621, 2408, 3150, 4174, 5150, 6425, 7655, 9211, 10731, 12614, 14476, 16748, 19020, 21759, 24525, 27825, 31185, 35156, 39226, 43417},
        {0, 0, 0, 270, 521, 1033, 1513, 2255, 2958, 3934, 4870, 6100, 7295, 8815, 10311, 12173, 14028, 16300, 18588, 21354, 24165, 27525, 30965, 35035, 39226},
        {0, 0, 0, 0, 251, 485, 965, 1417, 2120, 2790, 3726, 4630, 5825, 6995, 8491, 9975, 11830, 13692, 15980, 18300, 21111, 23985, 27425, 30965, 35156},
        {0, 0, 0, 0, 0, 234, 453, 905, 1333, 2003, 2646, 3550, 4430, 5600, 6755, 8239, 9723, 11585, 13468, 15788, 18156, 21030, 23985, 27525, 31185},
        {0, 0, 0, 0, 0, 0, 219, 425, 853, 1261, 1904, 2526, 3406, 4270, 5425, 6575, 8059, 9555, 11438, 13356, 15724, 18156, 21111, 24165, 27825},
        {0, 0, 0, 0, 0, 0, 0, 206, 401, 809, 1201, 1823, 2430, 3294, 4150, 5300, 6455, 7951, 9471, 11389, 13356, 15788, 18300, 21354, 24525},
        {0, 0, 0, 0, 0, 0, 0, 0, 195, 381, 773, 1153, 1760, 2358, 3214, 4070, 5225, 6395, 7915, 9471, 11438, 13468, 15980, 18588, 21759},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 186, 365, 745, 1117, 1715, 2310, 3166, 4030, 5200, 6395, 7951, 9555, 11585, 13692, 16300, 19020},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 179, 353, 725, 1093, 1688, 2286, 3150, 4030, 5225, 6455, 8059, 9723, 11830, 14028, 16748},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 174, 345, 713, 1081, 1679, 2286, 3166, 4070, 5300, 6575, 8239, 9975, 12173, 14476},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 171, 341, 709, 1081, 1688, 2310, 3214, 4150, 5425, 6755, 8491, 10311, 12614},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 170, 341, 713, 1093, 1715, 2358, 3294, 4270, 5600, 6995, 8815, 10731},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 171, 345, 725, 1117, 1760, 2430, 3406, 4430, 5825, 7295, 9211},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 174, 353, 745, 1153, 1823, 2526, 3550, 4630, 6100, 7655},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 179, 365, 773, 1201, 1904, 2646, 3726, 4870, 6425},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 186, 381, 809, 1261, 2003, 2790, 3934, 5150},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 195, 401, 853, 1333, 2120, 2958, 4174},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 206, 425, 905, 1417, 2255, 3150},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 219, 453, 965, 1513, 2408},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 485, 1033, 1621},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 251, 521, 1109},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 270, 561},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 291},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 291, 561, 1109, 1621, 2408, 3150, 4174, 5150, 6425, 7655, 9211, 10731, 12614, 14476, 16748, 19020, 21759, 24525, 27825, 31185, 35156, 39226, 43994},
        {0, 0, 0, 270, 521, 1033, 1513, 2255, 2958, 3934, 4870, 6100, 7295, 8815, 10311, 12173, 14028, 16300, 18588, 21354, 24165, 27525, 30965, 35035, 39226},
        {0, 0, 0, 0, 251, 485, 965, 1417, 2120, 2790, 3726, 4630, 5825, 6995, 8491, 9975, 11830, 13692, 15980, 18300, 21111, 23985, 27425, 30965, 35156},
        {0, 0, 0, 0, 0, 234, 453, 905, 1333, 2003, 2646, 3550, 4430, 5600, 6755, 8239, 9723, 11585, 13468, 15788, 18156, 21030, 23985, 27525, 31185},
        {0, 0, 0, 0, 0, 0, 219, 425, 853, 1261, 1904, 2526, 3406, 4270, 5425, 6575, 8059, 9555, 11438, 13356, 15724, 18156, 21111, 24165, 27825},
        {0, 0, 0, 0, 0, 0, 0, 206, 401, 809, 1201, 1823, 2430, 3294, 4150, 5300, 6455, 7951, 9471, 11389, 13356, 15788, 18300, 21354, 24525},
        {0, 0, 0, 0, 0, 0, 0, 0, 195, 381, 773, 1153, 1760, 2358, 3214, 4070, 5225, 6395, 7915, 9471, 11438, 13468, 15980, 18588, 21759},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 186, 365, 745, 1117, 1715, 2310, 3166, 4030, 5200, 6395, 7951, 9555, 11585, 13692, 16300, 19020},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 179, 353, 725, 1093, 1688, 2286, 3150, 4030, 5225, 6455, 8059, 9723, 11830, 14028, 16748},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 174, 345, 713, 1081, 1679, 2286, 3166, 4070, 5300, 6575, 8239, 9975, 12173, 14476},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 171, 341, 709, 1081, 1688, 2310, 3214, 4150, 5425, 6755, 8491, 10311, 12614},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 170, 341, 713, 1093, 1715, 2358, 3294, 4270, 5600, 6995, 8815, 10731},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 171, 345, 725, 1117, 1760, 2430, 3406, 4430, 5825, 7295, 9211},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 174, 353, 745, 1153, 1823, 2526, 3550, 4630, 6100, 7655},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 179, 365, 773, 1201, 1904, 2646, 3726, 4870, 6425},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 186, 381, 809, 1261, 2003, 2790, 3934, 5150},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 195, 401, 853, 1333, 2120, 2958, 4174},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 206, 425, 905, 1417, 2255, 3150},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 219, 453, 965, 1513, 2408},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 485, 1033, 1621},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 251, 521, 1109},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 270, 561},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 291},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 341, 659, 1303, 1907, 2832, 3706, 4906, 6050, 7535, 8965, 10761, 12509, 14658, 16772, 19332, 21876, 24921, 27975, 31595, 35255, 39556, 43934, 49038, 54262, 59630},
        {0, 0, 0, 318, 615, 1219, 1787, 2661, 3490, 4634, 5730, 7160, 8545, 10293, 12005, 14119, 16212, 18756, 21300, 24354, 27435, 31095, 34815, 39193, 43670, 48894, 54262},
        {0, 0, 0, 0, 297, 575, 1143, 1679, 2508, 3298, 4394, 5450, 6835, 8185, 9897, 11585, 13678, 15764, 18308, 20868, 23949, 27075, 30795, 34595, 39072, 43670, 49038},
        {0, 0, 0, 0, 0, 278, 539, 1075, 1583, 2373, 3130, 4186, 5210, 6560, 7885, 9573, 11249, 13335, 15428, 17988, 20580, 23706, 26895, 30695, 34595, 39193, 43934},
        {0, 0, 0, 0, 0, 0, 261, 507, 1015, 1499, 2256, 2986, 4010, 5010, 6335, 7645, 9321, 10997, 13090, 15204, 17796, 20436, 23625, 26895, 30795, 34815, 39556},
        {0, 0, 0, 0, 0, 0, 0, 246, 479, 963, 1427, 2157, 2866, 3866, 4850, 6160, 7465, 9141, 10829, 12943, 15092, 17732, 20436, 23706, 27075, 31095, 35255},
        {0, 0, 0, 0, 0, 0, 0, 0, 233, 455, 919, 1367, 2076, 2770, 3754, 4730, 6035, 7345, 9033, 10745, 12894, 15092, 17796, 20580, 23949, 27435, 31595},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 222, 435, 883, 1319, 2013, 2698, 3674, 4650, 5960, 7285, 8997, 10745, 12943, 15204, 17988, 20868, 24354, 27975},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 213, 419, 855, 1283, 1968, 2650, 3626, 4610, 5935, 7285, 9033, 10829, 13090, 15428, 18308, 21300, 24921},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 206, 407, 835, 1259, 1941, 2626, 3610, 4610, 5960, 7345, 9141, 10997, 13335, 15764, 18756, 21876},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 201, 399, 823, 1247, 1932, 2626, 3626, 4650, 6035, 7465, 9321, 11249, 13678, 16212, 19332},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 198, 395, 819, 1247, 1941, 2650, 3674, 4730, 6160, 7645, 9573, 11585, 14119, 16772},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 197, 395, 823, 1259, 1968, 2698, 3754, 4850, 6335, 7885, 9897, 12005, 14658},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 198, 399, 835, 1283, 2013, 2770, 3866, 5010, 6560, 8185, 10293, 12509},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 201, 407, 855, 1319, 2076, 2866, 4010, 5210, 6835, 8545, 10761},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 206, 419, 883, 1367, 2157, 2986, 4186, 5450, 7160, 8965},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 213, 435, 919, 1427, 2256, 3130, 4394, 5730, 7535},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 222, 455, 963, 1499, 2373, 3298, 4634, 6050},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 479, 1015, 1583, 2508, 3490, 4906},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 246, 507, 1075, 1679, 2661, 3706},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261, 539, 1143, 1787, 2832},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 278, 575, 1219, 1907},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 297, 615, 1303},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 318, 659},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 341},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 341, 659, 1303, 1907, 2832, 3706, 4906, 6050, 7535, 8965, 10761, 12509, 14658, 16772, 19332, 21876, 24921, 27975, 31595, 35255, 39556, 43934, 49038, 54262, 60307},
        {0, 0, 0, 318, 615, 1219, 1787, 2661, 3490, 4634, 5730, 7160, 8545, 10293, 12005, 14119, 16212, 18756, 21300, 24354, 27435, 31095, 34815, 39193, 43670, 48894, 54262},
        {0, 0, 0, 0, 297, 575, 1143, 1679, 2508, 3298, 4394, 5450, 6835, 8185, 9897, 11585, 13678, 15764, 18308, 20868, 23949, 27075, 30795, 34595, 39072, 43670, 49038},
        {0, 0, 0, 0, 0, 278, 539, 1075, 1583, 2373, 3130, 4186, 5210, 6560, 7885, 9573, 11249, 13335, 15428, 17988, 20580, 23706, 26895, 30695, 34595, 39193, 43934},
        {0, 0, 0, 0, 0, 0, 261, 507, 1015, 1499, 2256, 2986, 4010, 5010, 6335, 7645, 9321, 10997, 13090, 15204, 17796, 20436, 23625, 26895, 30795, 34815, 39556},
        {0, 0, 0, 0, 0, 0, 0, 246, 479, 963, 1427, 2157, 2866, 3866, 4850, 6160, 7465, 9141, 10829, 12943, 15092, 17732, 20436, 23706, 27075, 31095, 35255},
        {0, 0, 0, 0, 0, 0, 0, 0, 233, 455, 919, 1367, 2076, 2770, 3754, 4730, 6035, 7345, 9033, 10745, 12894, 15092, 17796, 20580, 23949, 27435, 31595},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 222, 435, 883, 1319, 2013, 2698, 3674, 4650, 5960, 7285, 8997, 10745, 12943, 15204, 17988, 20868, 24354, 27975},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 213, 419, 855, 1283, 1968, 2650, 3626, 4610, 5935, 7285, 9033, 10829, 13090, 15428, 18308, 21300, 24921},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 206, 407, 835, 1259, 1941, 2626, 3610, 4610, 5960, 7345, 9141, 10997, 13335, 15764, 18756, 21876},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 201, 399, 823, 1247, 1932, 2626, 3626, 4650, 6035, 7465, 9321, 11249, 13678, 16212, 19332},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 198, 395, 819, 1247, 1941, 2650, 3674, 4730, 6160, 7645, 9573, 11585, 14119, 16772},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 197, 395, 823, 1259, 1968, 2698, 3754, 4850, 6335, 7885, 9897, 12005, 14658},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 198, 399, 835, 1283, 2013, 2770, 3866, 5010, 6560, 8185, 10293, 12509},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 201, 407, 855, 1319, 2076, 2866, 4010, 5210, 6835, 8545, 10761},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 206, 419, 883, 1367, 2157, 2986, 4186, 5450, 7160, 8965},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 213, 435, 919, 1427, 2256, 3130, 4394, 5730, 7535},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 222, 455, 963, 1499, 2373, 3298, 4634, 6050},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 479, 1015, 1583, 2508, 3490, 4906},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 246, 507, 1075, 1679, 2661, 3706},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261, 539, 1143, 1787, 2832},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 278, 575, 1219, 1907},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 297, 615, 1303},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 318, 659},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 341},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 395, 765, 1513, 2217, 3292, 4310, 5702, 7030, 8745, 10395, 12455, 14455, 16898, 19292, 22172, 25020, 28407, 31785, 35765, 39765, 44440, 49170, 54658, 60242, 66677, 73255, 80002},
        {0, 0, 0, 370, 717, 1421, 2085, 3103, 4070, 5398, 6670, 8320, 9915, 11915, 13867, 16261, 18620, 21468, 24300, 27678, 31065, 35065, 39105, 43835, 48642, 54226, 59930, 66508, 73255},
        {0, 0, 0, 0, 347, 673, 1337, 1965, 2932, 3854, 5126, 6350, 7945, 9495, 11447, 13363, 15722, 18060, 20892, 23724, 27111, 30525, 34565, 38665, 43472, 48378, 54082, 59930, 66677},
        {0, 0, 0, 0, 0, 326, 633, 1261, 1857, 2779, 3662, 4886, 6070, 7620, 9135, 11051, 12943, 15281, 17612, 20444, 23292, 26706, 30165, 34265, 38445, 43351, 48378, 54226, 60242},
        {0, 0, 0, 0, 0, 0, 307, 597, 1193, 1761, 2644, 3494, 4678, 5830, 7345, 8835, 10727, 12607, 14938, 17276, 20124, 23004, 26463, 29985, 34165, 38445, 43472, 48642, 54658},
        {0, 0, 0, 0, 0, 0, 0, 290, 565, 1133, 1677, 2527, 3350, 4502, 5630, 7120, 8595, 10475, 12355, 14693, 17052, 19932, 22860, 26382, 29985, 34265, 38665, 43835, 49170},
        {0, 0, 0, 0, 0, 0, 0, 0, 275, 537, 1081, 1605, 2428, 3230, 4358, 5470, 6945, 8415, 10295, 12187, 14546, 16940, 19868, 22860, 26463, 30165, 34565, 39105, 44440},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 262, 513, 1037, 1545, 2347, 3134, 4246, 5350, 6820, 8295, 10187, 12103, 14497, 16940, 19932, 23004, 26706, 30525, 35065, 39765},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 251, 493, 1001, 1497, 2284, 3062, 4166, 5270, 6745, 8235, 10151, 12103, 14546, 17052, 20124, 23292, 27111, 31065, 35765},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 242, 477, 973, 1461, 2239, 3014, 4118, 5230, 6720, 8235, 10187, 12187, 14693, 17276, 20444, 23724, 27678, 31785},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 235, 465, 953, 1437, 2212, 2990, 4102, 5230, 6745, 8295, 10295, 12355, 14938, 17612, 20892, 24300, 28407},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 230, 457, 941, 1425, 2203, 2990, 4118, 5270, 6820, 8415, 10475, 12607, 15281, 18060, 21468, 25020},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 227, 453, 937, 1425, 2212, 3014, 4166, 5350, 6945, 8595, 10727, 12943, 15722, 18620, 22172},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 226, 453, 941, 1437, 2239, 3062, 4246, 5470, 7120, 8835, 11051, 13363, 16261, 19292},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 227, 457, 953, 1461, 2284, 3134, 4358, 5630, 7345, 9135, 11447, 13867, 16898},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 230, 465, 973, 1497, 2347, 3230, 4502, 5830, 7620, 9495, 11915, 14455},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 235, 477, 1001, 1545, 2428, 3350, 4678, 6070, 7945, 9915, 12455},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 242, 493, 1037, 1605, 2527, 3494, 4886, 6350, 8320, 10395},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 251, 513, 1081, 1677, 2644, 3662, 5126, 6670, 8745},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 262, 537, 1133, 1761, 2779, 3854, 5398, 7030},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 275, 565, 1193, 1857, 2932, 4070, 5702},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 290, 597, 1261, 1965, 3103, 4310},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 307, 633, 1337, 2085, 3292},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 326, 673, 1421, 2217},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 347, 717, 1513},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 370, 765},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 395},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 395, 765, 1513, 2217, 3292, 4310, 5702, 7030, 8745, 10395, 12455, 14455, 16898, 19292, 22172, 25020, 28407, 31785, 35765, 39765, 44440, 49170, 54658, 60242, 66677, 73255, 80787},
        {0, 0, 0, 370, 717, 1421, 2085, 3103, 4070, 5398, 6670, 8320, 9915, 11915, 13867, 16261, 18620, 21468, 24300, 27678, 31065, 35065, 39105, 43835, 48642, 54226, 59930, 66508, 73255},
        {0, 0, 0, 0, 347, 673, 1337, 1965, 2932, 3854, 5126, 6350, 7945, 9495, 11447, 13363, 15722, 18060, 20892, 23724, 27111, 30525, 34565, 38665, 43472, 48378, 54082, 59930, 66677},
        {0, 0, 0, 0, 0, 326, 633, 1261, 1857, 2779, 3662, 4886, 6070, 7620, 9135, 11051, 12943, 15281, 17612, 20444, 23292, 26706, 30165, 34265, 38445, 43351, 48378, 54226, 60242},
        {0, 0, 0, 0, 0, 0, 307, 597, 1193, 1761, 2644, 3494, 4678, 5830, 7345, 8835, 10727, 12607, 14938, 17276, 20124, 23004, 26463, 29985, 34165, 38445, 43472, 48642, 54658},
        {0, 0, 0, 0, 0, 0, 0, 290, 565, 1133, 1677, 2527, 3350, 4502, 5630, 7120, 8595, 10475, 12355, 14693, 17052, 19932, 22860, 26382, 29985, 34265, 38665, 43835, 49170},
        {0, 0, 0, 0, 0, 0, 0, 0, 275, 537, 1081, 1605, 2428, 3230, 4358, 5470, 6945, 8415, 10295, 12187, 14546, 16940, 19868, 22860, 26463, 30165, 34565, 39105, 44440},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 262, 513, 1037, 1545, 2347, 3134, 4246, 5350, 6820, 8295, 10187, 12103, 14497, 16940, 19932, 23004, 26706, 30525, 35065, 39765},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 251, 493, 1001, 1497, 2284, 3062, 4166, 5270, 6745, 8235, 10151, 12103, 14546, 17052, 20124, 23292, 27111, 31065, 35765},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 242, 477, 973, 1461, 2239, 3014, 4118, 5230, 6720, 8235, 10187, 12187, 14693, 17276, 20444, 23724, 27678, 31785},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 235, 465, 953, 1437, 2212, 2990, 4102, 5230, 6745, 8295, 10295, 12355, 14938, 17612, 20892, 24300, 28407},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 230, 457, 941, 1425, 2203, 2990, 4118, 5270, 6820, 8415, 10475, 12607, 15281, 18060, 21468, 25020},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 227, 453, 937, 1425, 2212, 3014, 4166, 5350, 6945, 8595, 10727, 12943, 15722, 18620, 22172},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 226, 453, 941, 1437, 2239, 3062, 4246, 5470, 7120, 8835, 11051, 13363, 16261, 19292},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 227, 457, 953, 1461, 2284, 3134, 4358, 5630, 7345, 9135, 11447, 13867, 16898},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 230, 465, 973, 1497, 2347, 3230, 4502, 5830, 7620, 9495, 11915, 14455},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 235, 477, 1001, 1545, 2428, 3350, 4678, 6070, 7945, 9915, 12455},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 242, 493, 1037, 1605, 2527, 3494, 4886, 6350, 8320, 10395},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 251, 513, 1081, 1677, 2644, 3662, 5126, 6670, 8745},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 262, 537, 1133, 1761, 2779, 3854, 5398, 7030},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 275, 565, 1193, 1857, 2932, 4070, 5702},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 290, 597, 1261, 1965, 3103, 4310},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 307, 633, 1337, 2085, 3292},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 326, 673, 1421, 2217},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 347, 717, 1513},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 370, 765},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 395},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 453, 879, 1739, 2551, 3788, 4962, 6562, 8090, 10055, 11945, 14293, 16569, 19334, 22036, 25268, 28452, 32217, 35955, 40335, 44715, 49808, 54934, 60854, 66846, 73723, 80717, 88697, 96845, 105189},
        {0, 0, 0, 426, 827, 1639, 2407, 3581, 4698, 6226, 7690, 9580, 11405, 13681, 15897, 18599, 21252, 24436, 27588, 31326, 35055, 39435, 43835, 48961, 54142, 60134, 66222, 73216, 80353, 88501, 96845},
        {0, 0, 0, 0, 401, 779, 1547, 2275, 3392, 4458, 5922, 7330, 9155, 10925, 13141, 15309, 17962, 20580, 23732, 26868, 30597, 34335, 38735, 43175, 48356, 53614, 59702, 65910, 73047, 80353, 88697},
        {0, 0, 0, 0, 0, 378, 735, 1463, 2155, 3221, 4242, 5650, 7010, 8780, 10505, 12673, 14805, 17423, 20020, 23156, 26292, 30030, 33795, 38235, 42735, 47993, 53350, 59558, 65910, 73216, 80717},
        {0, 0, 0, 0, 0, 0, 357, 695, 1387, 2047, 3068, 4050, 5410, 6730, 8455, 10145, 12277, 14385, 16982, 19572, 22708, 25860, 29625, 33435, 37935, 42515, 47872, 53350, 59702, 66222, 73723},
        {0, 0, 0, 0, 0, 0, 0, 338, 659, 1319, 1951, 2933, 3882, 5202, 6490, 8180, 9845, 11953, 14049, 16639, 19236, 22388, 25572, 29382, 33255, 37835, 42515, 47993, 53614, 60134, 66846},
        {0, 0, 0, 0, 0, 0, 0, 0, 321, 627, 1259, 1867, 2816, 3738, 5026, 6290, 7955, 9605, 11701, 13797, 16394, 19012, 22196, 25428, 29301, 33255, 37935, 42735, 48356, 54142, 60854},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 599, 1207, 1795, 2717, 3618, 4882, 6130, 7780, 9425, 11521, 13629, 16247, 18900, 22132, 25428, 29382, 33435, 38235, 43175, 48961, 54934},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 293, 575, 1163, 1735, 2636, 3522, 4770, 6010, 7655, 9305, 11413, 13545, 16198, 18900, 22196, 25572, 29625, 33795, 38735, 43835, 49808},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 282, 555, 1127, 1687, 2573, 3450, 4690, 5930, 7580, 9245, 11377, 13545, 16247, 19012, 22388, 25860, 30030, 34335, 39435, 44715},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 273, 539, 1099, 1651, 2528, 3402, 4642, 5890, 7555, 9245, 11413, 13629, 16394, 19236, 22708, 26292, 30597, 35055, 40335},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 266, 527, 1079, 1627, 2501, 3378, 4626, 5890, 7580, 9305, 11521, 13797, 16639, 19572, 23156, 26868, 31326, 35955},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261, 519, 1067, 1615, 2492, 3378, 4642, 5930, 7655, 9425, 11701, 14049, 16982, 20020, 23732, 27588, 32217},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 258, 515, 1063, 1615, 2501, 3402, 4690, 6010, 7780, 9605, 11953, 14385, 17423, 20580, 24436, 28452},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 257, 515, 1067, 1627, 2528, 3450, 4770, 6130, 7955, 9845, 12277, 14805, 17962, 21252, 25268},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 258, 519, 1079, 1651, 2573, 3522, 4882, 6290, 8180, 10145, 12673, 15309, 18599, 22036},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261, 527, 1099, 1687, 2636, 3618, 5026, 6490, 8455, 10505, 13141, 15897, 19334},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 266, 539, 1127, 1735, 2717, 3738, 5202, 6730, 8780, 10925, 13681, 16569},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 273, 555, 1163, 1795, 2816, 3882, 5410, 7010, 9155, 11405, 14293},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 282, 575, 1207, 1867, 2933, 4050, 5650, 7330, 9580, 11945},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 293, 599, 1259, 1951, 3068, 4242, 5922, 7690, 10055},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 627, 1319, 2047, 3221, 4458, 6226, 8090},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 321, 659, 1387, 2155, 3392, 4698, 6562},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 338, 695, 1463, 2275, 3581, 4962},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 357, 735, 1547, 2407, 3788},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 378, 779, 1639, 2551},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 401, 827, 1739},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 426, 879},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 453},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {    
        {0, 0, 453, 879, 1739, 2551, 3788, 4962, 6562, 8090, 10055, 11945, 14293, 16569, 19334, 22036, 25268, 28452, 32217, 35955, 40335, 44715, 49808, 54934, 60854, 66846, 73723, 80717, 88697, 96845, 106090},
        {0, 0, 0, 426, 827, 1639, 2407, 3581, 4698, 6226, 7690, 9580, 11405, 13681, 15897, 18599, 21252, 24436, 27588, 31326, 35055, 39435, 43835, 48961, 54142, 60134, 66222, 73216, 80353, 88501, 96845},
        {0, 0, 0, 0, 401, 779, 1547, 2275, 3392, 4458, 5922, 7330, 9155, 10925, 13141, 15309, 17962, 20580, 23732, 26868, 30597, 34335, 38735, 43175, 48356, 53614, 59702, 65910, 73047, 80353, 88697},
        {0, 0, 0, 0, 0, 378, 735, 1463, 2155, 3221, 4242, 5650, 7010, 8780, 10505, 12673, 14805, 17423, 20020, 23156, 26292, 30030, 33795, 38235, 42735, 47993, 53350, 59558, 65910, 73216, 80717},
        {0, 0, 0, 0, 0, 0, 357, 695, 1387, 2047, 3068, 4050, 5410, 6730, 8455, 10145, 12277, 14385, 16982, 19572, 22708, 25860, 29625, 33435, 37935, 42515, 47872, 53350, 59702, 66222, 73723},
        {0, 0, 0, 0, 0, 0, 0, 338, 659, 1319, 1951, 2933, 3882, 5202, 6490, 8180, 9845, 11953, 14049, 16639, 19236, 22388, 25572, 29382, 33255, 37835, 42515, 47993, 53614, 60134, 66846},
        {0, 0, 0, 0, 0, 0, 0, 0, 321, 627, 1259, 1867, 2816, 3738, 5026, 6290, 7955, 9605, 11701, 13797, 16394, 19012, 22196, 25428, 29301, 33255, 37935, 42735, 48356, 54142, 60854},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 599, 1207, 1795, 2717, 3618, 4882, 6130, 7780, 9425, 11521, 13629, 16247, 18900, 22132, 25428, 29382, 33435, 38235, 43175, 48961, 54934},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 293, 575, 1163, 1735, 2636, 3522, 4770, 6010, 7655, 9305, 11413, 13545, 16198, 18900, 22196, 25572, 29625, 33795, 38735, 43835, 49808},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 282, 555, 1127, 1687, 2573, 3450, 4690, 5930, 7580, 9245, 11377, 13545, 16247, 19012, 22388, 25860, 30030, 34335, 39435, 44715},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 273, 539, 1099, 1651, 2528, 3402, 4642, 5890, 7555, 9245, 11413, 13629, 16394, 19236, 22708, 26292, 30597, 35055, 40335},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 266, 527, 1079, 1627, 2501, 3378, 4626, 5890, 7580, 9305, 11521, 13797, 16639, 19572, 23156, 26868, 31326, 35955},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261, 519, 1067, 1615, 2492, 3378, 4642, 5930, 7655, 9425, 11701, 14049, 16982, 20020, 23732, 27588, 32217},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 258, 515, 1063, 1615, 2501, 3402, 4690, 6010, 7780, 9605, 11953, 14385, 17423, 20580, 24436, 28452},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 257, 515, 1067, 1627, 2528, 3450, 4770, 6130, 7955, 9845, 12277, 14805, 17962, 21252, 25268},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 258, 519, 1079, 1651, 2573, 3522, 4882, 6290, 8180, 10145, 12673, 15309, 18599, 22036},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261, 527, 1099, 1687, 2636, 3618, 5026, 6490, 8455, 10505, 13141, 15897, 19334},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 266, 539, 1127, 1735, 2717, 3738, 5202, 6730, 8780, 10925, 13681, 16569},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 273, 555, 1163, 1795, 2816, 3882, 5410, 7010, 9155, 11405, 14293},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 282, 575, 1207, 1867, 2933, 4050, 5650, 7330, 9580, 11945},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 293, 599, 1259, 1951, 3068, 4242, 5922, 7690, 10055},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 627, 1319, 2047, 3221, 4458, 6226, 8090},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 321, 659, 1387, 2155, 3392, 4698, 6562},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 338, 695, 1463, 2275, 3581, 4962},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 357, 735, 1547, 2407, 3788},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 378, 779, 1639, 2551},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 401, 827, 1739},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 426, 879},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 453},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
};

constexpr int CalcRangeWeightNi(int N) {
    return (N / 2) - 15;
}

struct RangeWeightAccessor {
    const int ni_;
    const int lower_;
    RangeWeightAccessor(int N) : ni_(N / 2 - 15), lower_(N / 4) {
        VASSERT(ni_ < 16);
    }
    int Weight(int top, int bottom) const {
        VASSERT(top <= bottom);
        int t = top - lower_;
        int b = bottom - lower_;
        VASSERT(t >= 0 && t < 31);
        VASSERT(b >= 0 && b < 31);
        return RangeWeightTable[ni_][t][b];
    }
    int WeightFromBase(int top, int bottom) const {
        VASSERT(top <= bottom);
        VASSERT(top >= 0 && top < 31);
        VASSERT(bottom >= 0 && bottom < 31);
        return RangeWeightTable[ni_][top][bottom];
    }
};



bool g_prospectScore = true;

#define STATE_VALIDATION_DRAW 0
#define STATE_VALIDATION_DELETE 0
#define STATE_VALID_RECT_CHECK 0

template <int CAP_NN, int CAP_VALID, int CAP_DRAW, int CAP_DEPEND_RECT>
struct dense {
	using ValidRects = CapArr<Rect, CAP_VALID>;
	using DrawRects = CapArr<Rect, CAP_DRAW>;

struct State {
private:
	GridState<CAP_NN> grid_;
	ScoreGrid<CAP_NN, CAP_DEPEND_RECT> scoreGrid_;
	int pattern_ = -1;
	int totalWeight_;
	double rawScore_;
	double evalScore_;

	ValidRects validRects_;	
	DrawRects drawRects_;	


public:
	inline double RawScore() const {
		return rawScore_;
	}
	inline double EvalScore() const {
		return evalScore_;
	}
	inline const ValidRects& RefValidRects() const {
		return validRects_;
	}
	inline const DrawRects& RefDrawRects() const {
		return drawRects_;
	}
	inline int Pattern() const {
		return pattern_;
	}

	static void Copy(const State& src, State& dst) {
		int size = int(((u64)&dst.validRects_) - ((u64)&dst));
		memcpy(&dst, &src, size);

		dst.validRects_.resize(src.validRects_.size());
		memcpy(dst.validRects_.data(), src.validRects_.data(), sizeof(Rect) * src.validRects_.size());

		dst.drawRects_.resize(src.drawRects_.size());
		memcpy(dst.drawRects_.data(), src.drawRects_.data(), sizeof(Rect) * src.drawRects_.size());
	}

private:
	void DrawRect(const Rect& rect) {
		int dp = rect.DrawPoint();
		grid_.DrawRect(rect);
		scoreGrid_.DrawRect(rect);
		drawRects_.push_back(rect);
		totalWeight_ += server.Weight(dp);

		int newRectStart = 0;
		::UpdateValidRect(grid_, validRects_, rect, g_prospectScore ? limitAroundMap : aroundMap, &newRectStart);
		CutValidRects(newRectStart);
	}

	void RemoveRects(int deleteRi, const CapArr<int, NN_MAX>& removeRis, const CheckMapD& removeMap) {
		static DrawRects removeRects;
		removeRects.clear();
		RREP(i, removeRis.size()) {
			int ri = removeRis[i];
			const auto& rect = drawRects_[ri];
			int dp = rect.DrawPoint();

			grid_.RemoveRect(rect);
			scoreGrid_.RemoveRect(rect);
			removeRects.push_back(rect);
			totalWeight_ -= server.Weight(dp);
		}

		int newRectStart = 0;
		RemovedUpdateValidateRects(grid_, validRects_, removeRects, g_prospectScore ? limitAroundMap : aroundMap, & newRectStart);
		CutValidRects(newRectStart);

		int removedCount = 0;
		FOR(ri, deleteRi, drawRects_.size()) {
			const auto& rect = drawRects_[ri];
			int dp = rect.DrawPoint();
			if (removeMap.IsChecked(dp)) {
				++removedCount;
				continue;
			}
			int newIndex = ri - removedCount;
			drawRects_[newIndex] = drawRects_[ri];
		}
		drawRects_.resize(drawRects_.size() - removedCount);
	}

	void CutValidRects(int startRi) {
		if (g_prospectScore) {
			RFOR(ri, startRi, validRects_.size()) {
				if (!IsValidRect(validRects_[ri])) {
					validRects_.RemoveSlide(ri);
				}
			}
		}
	}

public:
	void Init(int pattern) {
		pattern_ = pattern;

		grid_.Init(server.N, server.marks);
		EnumValidRectForInit(grid_, validRects_, g_prospectScore ? limitAroundMap : aroundMap);
		CutValidRects(0);
		drawRects_.clear();
		totalWeight_ = server.InitialWeight();
		scoreGrid_.InitNoRect(pattern_);

		rawScore_ = 0;
		evalScore_ = 0;
	}

	void UpdateValidRect() {
		EnumValidRect(grid_, validRects_);
	}

	double Eval(double progress) const {
		if (!g_prospectScore) {
			double s = server.Score(totalWeight_);
			return s;
		}
		else {
			const auto& dependMap = DependMaps::Instance(pattern_);
			RangeWeightAccessor rangeWeightAccessor(server.N);


			auto RangeWeight = [&](int a, int b) {
				return rangeWeightAccessor.WeightFromBase(a, b);
			};
			const int borderLength = server.initR - server.initL + 1;

			int weight = 0;
			REP(dirIndex, 4) {
				int baseCell;
				Dir front;

				if (dirIndex == 0) {
					baseCell = gs.To1D(server.initL, server.initT);
					front = Dir::R;
				}
				else if (dirIndex == 1) {
					baseCell = gs.To1D(server.initR, server.initT);
					front = Dir::D;
				}
				else if (dirIndex == 2) {
					baseCell = gs.To1D(server.initR, server.initB);
					front = Dir::L;
				}
				else {
					baseCell = gs.To1D(server.initL, server.initB);
					front = Dir::U;
				}
				Dir right = Rotate90R(front);
				Dir back = Back(front);

				int startPoint = -1;
				auto Flush = [&](int lastPoint, int lastCell) {
					if (!grid_.TestEdge(lastCell, back)) {
						--lastPoint;
					}
					if (startPoint < lastPoint) {
						weight += RangeWeight(startPoint, lastPoint);
					}
					startPoint = -1;
				};

				int cur = baseCell;
				int prev = cur;
				REP(i, borderLength) {
					bool ok = false;
					if (grid_.TestPoint(cur)) {
						int inner = aroundMap[cur][(int)right];
						if (dependMap.IsRequirePoint(inner)) {
							if (grid_.TestPoint(inner)) {
								ok = true;
							}
						}
						else {
							ok = true;
						}
					}
					if (ok) {
						if (startPoint < 0) {
							if (grid_.TestEdge(cur, front)) {
								startPoint = i;
							}
						}
					}
					else {
						if (startPoint >= 0) {
							Flush(i - 1, prev);
						}
					}
					prev = cur;
					cur = aroundMap[cur][(int)front];
				}
				if (startPoint >= 0) {
					Flush(borderLength - 1, prev);
				}
			}

			int totalWeight = totalWeight_ + weight;
			double s = server.Score(totalWeight);
			return s;
		}
	}

	bool IsValidRect(const Rect& rect) {
		if (!g_prospectScore) {
			return true;
		}
		const auto& p = gs.To2D(rect.DrawPoint());
		if (p.x < server.initL || p.x > server.initR ||
			p.y < server.initT || p.y > server.initB) {
			return false;
		}


		const auto& dependMap = DependMaps::Instance(pattern_);
		auto IsValidEdge = [&](int a, int b) {
			Dir dir = gs.CalcDir(a, b);
			while (a != b) {
				if (dependMap.IsNgEdge(a, dir)) {
					return false;
				}
				a = aroundMap[a][(int)dir];
			}
			return true;
		};
		REP(i, 4) {
			if (!IsValidEdge(rect.ps[i], rect.ps[(i + 1) & 3])) {
				return false;
			}
		}

		return true;
	}



	void PlayRandom(double progress, Xor64& rand) {
		while (!validRects_.empty()) {
			int ri = (int)rand(validRects_.size());
			DrawRect(validRects_[ri]);

		}
		rawScore_ = server.Score(totalWeight_);
		evalScore_ = Eval(progress);
	}

	void PlayPointScoreGready(double progress, Xor64& rand) {
		while (!validRects_.empty()) {
			int ri = 0;
			double bestScore = -1e10;
			REP(i, validRects_.size()) {
				const auto& rect = validRects_[i];

				double score = scoreGrid_.CalcDrawedScore(rect);

				if (score > bestScore) {
					bestScore = score;
					ri = i;
				}
			}

			DrawRect(validRects_[ri]);

		}
		rawScore_ = server.Score(totalWeight_);
		evalScore_ = Eval(progress);
	}

	void PlayEvalGready(double progress, Xor64& rand) {
		while (!validRects_.empty()) {
			int ri = 0;
			double bestScore = -1e10;
			REP(i, validRects_.size()) {
				const auto& rect = validRects_[i];

				int dp = rect.DrawPoint();
				int weight = server.Weight(dp);

				double score = 0;
				{
					LINEAR_M_DOUBLE(DenseWeightRate);
					score += weight * DenseWeightRate;
				}
				{
					LINEAR_M_DOUBLE(DenseLengthRate);
					int len = Length(rect);
					score -= len * DenseLengthRate;
				}

				if (score > bestScore) {
					bestScore = score;
					ri = i;
				}
			}

			DrawRect(validRects_[ri]);

		}
		rawScore_ = server.Score(totalWeight_);
		evalScore_ = Eval(progress);
	}

	void PlayRawScoreRandGready(double progress, Xor64& rand) {
		while (!validRects_.empty()) {
			int ri = 0;
			double bestScore = -1e10;
			REP(i, validRects_.size()) {
				const auto& rect = validRects_[i];

				int dp = rect.DrawPoint();
				int weight = server.Weight(dp);

				LINEAR_M_DOUBLE(DenseRawRandGeta);
				double score = server.Score(weight) * (DenseRawRandGeta + rand.GetDouble() * (1.0 - DenseRawRandGeta));
				if (score > bestScore) {
					bestScore = score;
					ri = i;
				}
			}

			DrawRect(validRects_[ri]);

		}
		rawScore_ = server.Score(totalWeight_);
		evalScore_ = Eval(progress);
	}


	bool DeleteRectRandomInt(int lowerCount, int upperCount, Xor64& rand) {
		VASSERT(!drawRects_.empty());

		static CapArr<int, NN_MAX> removeRis;
		static CheckMapD removeMap;

		int deleteRi = 0;
		REP(loop, 999999999){
			deleteRi = (int)rand(drawRects_.size());
			removeRis.clear();
			removeMap.Init(server.NN);

			{
				const auto& rect = drawRects_[deleteRi];
				int dp = rect.DrawPoint();
				removeRis.push_back(deleteRi);
				removeMap.Check(dp);
			}

			FOR(ri, deleteRi + 1, drawRects_.size()) {
				const auto& rect = drawRects_[ri];
				if (removeMap.IsChecked(rect.ps[1]) || removeMap.IsChecked(rect.ps[2]) || removeMap.IsChecked(rect.ps[3])) {
					int dp = rect.DrawPoint();
					removeRis.push_back(ri);
					removeMap.Check(dp);
				}
			}

			if (removeRis.size() <= upperCount) {
				break;
			}
			if (loop >= 1000) {
				return false;
			}
		}

		RemoveRects(deleteRi, removeRis, removeMap);

		rawScore_ = -1;
		evalScore_ = -1;

		return true;
	}
};
static constexpr int StateSize = sizeof(State);
static_assert(is_trivially_copyable<State>::value);

struct Solver {
	Xor64 rand_;


	State maxState_;
	void CheckMaxScore(const State& state) {
		if (state.EvalScore() > maxState_.EvalScore()) {
			State::Copy(state, maxState_);
		}
	}

	void Run(Result& result) {
		{
			DependMaps::Init();
			g_scoreGridParam.Init();
		}

		vector<State> states;
		InitState(states);
		State::Copy(states[0], maxState_);

		SimulatedAnnealing sa;
		sa.stepLoopCount = 100;

		LINEAR_M_DOUBLE(DenseFirstSATime);
		timer.StartMs(int(TIME_LIMIT * DenseFirstSATime));
		{
			LINEAR_M_DOUBLE(DenseStartTemp1);
			LINEAR_M_DOUBLE(DenseEndTemp1);
			LINEAR_M_DOUBLE(DenseRollbackStart1);
			LINEAR_M_INT(DenseRollbackCount1);
			LINEAR_M_DOUBLE(DenseRollbackNext1);

			sa.startTemp_ = DenseStartTemp1;
			sa.endTemp_ = DenseEndTemp1;
			sa.rollbackStartRate_ = DenseRollbackStart1;
			sa.rollbackCount_ = DenseRollbackCount1;
			sa.rollbackNextMulti_ = DenseRollbackNext1;

			LINEAR_M_INT(DenseTrans1_Rand);
			LINEAR_M_INT(DenseTrans1_RawRand);
			LINEAR_M_INT(DenseTrans1_PointScore);
			LINEAR_M_INT(DenseTrans1_EvalGready);

			auto transitions = make_tuple(
				MAKE_TRANS(Transition_Random, DenseTrans1_Rand)
				, MAKE_TRANS(Transition_RawScoreRandGready, DenseTrans1_RawRand)
				, MAKE_TRANS(Transition_PointScoreGready, DenseTrans1_PointScore)
				, MAKE_TRANS(Transition_EvalGready, DenseTrans1_EvalGready)
			);
			sa.Run2(timer, states, transitions);
		}


		g_prospectScore = false;
		maxState_.UpdateValidRect();
		maxState_.PlayPointScoreGready(1, rand_);		
		states.resize(1);
		State::Copy(maxState_, states[0]);

		timer.StartMs(TIME_LIMIT);
		{
			LINEAR_M_DOUBLE(DenseStartTemp2);
			LINEAR_M_DOUBLE(DenseEndTemp2);
			LINEAR_M_DOUBLE(DenseRollbackStart2);
			LINEAR_M_INT(DenseRollbackCount2);
			LINEAR_M_DOUBLE(DenseRollbackNext2);

			sa.startTemp_ = DenseStartTemp2;
			sa.endTemp_ = DenseEndTemp2;
			sa.rollbackStartRate_ = DenseRollbackStart2;
			sa.rollbackCount_ = DenseRollbackCount2;
			sa.rollbackNextMulti_ = DenseRollbackNext2;

			LINEAR_M_INT(DenseTrans2_Rand);
			LINEAR_M_INT(DenseTrans2_RawRand);
			LINEAR_M_INT(DenseTrans2_PointScore);
			LINEAR_M_INT(DenseTrans2_EvalGready);

			auto transitions = make_tuple(
				MAKE_TRANS(Transition_Random, DenseTrans2_Rand)
				, MAKE_TRANS(Transition_RawScoreRandGready, DenseTrans2_RawRand)
				, MAKE_TRANS(Transition_PointScoreGready, DenseTrans2_PointScore)
				, MAKE_TRANS(Transition_EvalGready, DenseTrans2_EvalGready)
			);
			sa.Run2(timer, states, transitions);
		}


		for (const auto& rect : maxState_.RefDrawRects()) {
			result.rects.push_back(rect);
		}

	}

	void InitState(vector<State>& states) {
		states.resize(8);
		REP(pattern, 8) {
			auto& state = states[pattern];
			state.Init(pattern);
			state.PlayPointScoreGready(0, rand_);
		}
	}

	bool DeleteRectRandom(State& state) {
		const auto& drawRects = state.RefDrawRects();
		int upperCount = 0;
		if (g_prospectScore) {
			LINEAR_M_DOUBLE(DenseUpperDeleteRate1);
			upperCount = int(drawRects.size() * DenseUpperDeleteRate1);
		}
		else {
			LINEAR_M_DOUBLE(DenseUpperDeleteRate2);
			upperCount = int(drawRects.size() * DenseUpperDeleteRate2);
		}
		if (upperCount == 0) {
			upperCount = 1;
		}
		return state.DeleteRectRandomInt(0, upperCount, rand_);
	}

	State tmpState_;

	void Transition_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.RefDrawRects().empty()) {
			return;
		}
		State::Copy(state, tmpState_);

		if (!DeleteRectRandom(tmpState_)) {
			return;
		}
		tmpState_.PlayRandom(sa.timeRate, rand_);

		if (checker(tmpState_.EvalScore())) {
			State::Copy(tmpState_, state);
			CheckMaxScore(state);
		}
	}

	void Transition_PointScoreGready(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.RefDrawRects().empty()) {
			return;
		}
		State::Copy(state, tmpState_);


		if (!DeleteRectRandom(tmpState_)) {
			return;
		}
		tmpState_.PlayPointScoreGready(sa.timeRate, rand_);

		if (checker(tmpState_.EvalScore())) {
			State::Copy(tmpState_, state);
			CheckMaxScore(state);
		}
	}

	void Transition_RawScoreRandGready(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.RefDrawRects().empty()) {
			return;
		}
		State::Copy(state, tmpState_);

		if (!DeleteRectRandom(tmpState_)) {
			return;
		}
		tmpState_.PlayRawScoreRandGready(sa.timeRate, rand_);

		if (checker(tmpState_.EvalScore())) {
			State::Copy(tmpState_, state);
			CheckMaxScore(state);
		}
	}

	void Transition_EvalGready(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.RefDrawRects().empty()) {
			return;
		}
		State::Copy(state, tmpState_);

		if (!DeleteRectRandom(tmpState_)) {
			return;
		}
		tmpState_.PlayEvalGready(sa.timeRate, rand_);

		if (checker(tmpState_.EvalScore())) {
			State::Copy(tmpState_, state);
			CheckMaxScore(state);
		}
	}

};
}; 


template <int CAP_NN, int CAP_VALID, int CAP_DRAW>
struct sparse {
	using ValidRects = CapArr<Rect, CAP_VALID>;
	using DrawRects = CapArr<Rect, CAP_DRAW>;

struct State {
private:
	GridState<CAP_NN> grid_;
	int totalWeight_;
	double rawScore_;
	double evalScore_;

	ValidRects validRects_;	
	DrawRects drawRects_;	


public:
	inline double RawScore() const {
		return rawScore_;
	}
	inline double EvalScore() const {
		return evalScore_;
	}
	inline const ValidRects& RefValidRects() const {
		return validRects_;
	}
	inline const DrawRects& RefDrawRects() const {
		return drawRects_;
	}

	static void Copy(const State& src, State& dst) {
		int size = int(((u64)&dst.validRects_) - ((u64)&dst));
		memcpy(&dst, &src, size);

		dst.validRects_.resize(src.validRects_.size());
		memcpy(dst.validRects_.data(), src.validRects_.data(), sizeof(Rect) * src.validRects_.size());

		dst.drawRects_.resize(src.drawRects_.size());
		memcpy(dst.drawRects_.data(), src.drawRects_.data(), sizeof(Rect) * src.drawRects_.size());
	}

public:
	void Init() {
		grid_.Init(server.N, server.marks);
		EnumValidRectForInit(grid_, validRects_);
		drawRects_.clear();
		totalWeight_ = server.InitialWeight();

		rawScore_ = 0;
		evalScore_ = 0;
	}

	double Eval() const {
		double s = 0;
		s += rawScore_;
		return s;
	}

	void PlayRandom(Xor64& rand) {
		while (!validRects_.empty()) {
			int ri = (int)rand(validRects_.size());
			const auto& rect = validRects_[ri];
			grid_.DrawRect(rect);
			drawRects_.push_back(rect);
			totalWeight_ += server.Weight(rect.DrawPoint());
			UpdateValidRect(grid_, validRects_, rect);			

		}
		rawScore_ = server.Score(totalWeight_);
		evalScore_ = Eval();
	}

	void PlayEvalGready(Xor64& rand) {

		while (!validRects_.empty()) {
			int ri = 0;
			double bestScore = -1e10;
			REP(i, validRects_.size()) {
				const auto& rect = validRects_[i];
				int dp = rect.DrawPoint();
				int weight = server.Weight(dp);

				double score = 0;
				{
					score += weight * HP.SparceWeightRate;
				}
				{
					int len = Length(rect);
					score -= len * HP.SparceLengthRate;
				}
				if (score > bestScore) {
					bestScore = score;
					ri = i;
				}
			}

			const auto& rect = validRects_[ri];
			grid_.DrawRect(rect);
			drawRects_.push_back(rect);
			totalWeight_ += server.Weight(rect.DrawPoint());
			UpdateValidRect(grid_, validRects_, rect);			

		}
		rawScore_ = server.Score(totalWeight_);
		evalScore_ = Eval();
	}

	void PlayRawScoreRandGready(Xor64& rand) {
		while (!validRects_.empty()) {
			int ri = 0;
			double bestScore = -1e10;
			REP(i, validRects_.size()) {
				const auto& rect = validRects_[i];
				int dp = rect.DrawPoint();
				int weight = server.Weight(dp);

				double score = server.Score(weight) * (HP.SparseRawRandGeta + rand.GetDouble() * (1.0 - HP.SparseRawRandGeta));
				if (score > bestScore) {
					bestScore = score;
					ri = i;
				}
			}

			const auto& rect = validRects_[ri];
			grid_.DrawRect(rect);
			drawRects_.push_back(rect);
			totalWeight_ += server.Weight(rect.DrawPoint());
			UpdateValidRect(grid_, validRects_, rect);			

		}
		rawScore_ = server.Score(totalWeight_);
		evalScore_ = Eval();
	}


	bool DeleteRectRandomInt(int lowerCount, int upperCount, Xor64& rand) {
		VASSERT(!drawRects_.empty());

		static CapArr<int, NN_MAX> removeRis;
		static CheckMapD removeMap;

		int deleteRi = 0;
		REP(loop, 999999999) {
			deleteRi = (int)rand(drawRects_.size());
			removeRis.clear();
			removeMap.Init(server.NN);

			{
				const auto& rect = drawRects_[deleteRi];
				int dp = rect.DrawPoint();
				removeRis.push_back(deleteRi);
				removeMap.Check(dp);
			}

			FOR(ri, deleteRi + 1, drawRects_.size()) {
				const auto& rect = drawRects_[ri];
				if (removeMap.IsChecked(rect.ps[1]) || removeMap.IsChecked(rect.ps[2]) || removeMap.IsChecked(rect.ps[3])) {
					int dp = rect.DrawPoint();
					removeRis.push_back(ri);
					removeMap.Check(dp);
				}
			}

			if (removeRis.size() <= upperCount) {
				break;
			}
			if (loop >= 1000) {
				return false;
			}
		}


		static DrawRects removeRects;
		removeRects.clear();
		RREP(i, removeRis.size()) {
			int ri = removeRis[i];
			const auto& rect = drawRects_[ri];
			int dp = rect.DrawPoint();

			grid_.RemoveRect(rect);
			removeRects.push_back(rect);
			totalWeight_ -= server.Weight(dp);
		}
		RemovedUpdateValidateRects(grid_, validRects_, removeRects);

		int removedCount = 0;
		FOR(ri, deleteRi, drawRects_.size()) {
			const auto& rect = drawRects_[ri];
			int dp = rect.DrawPoint();
			if (removeMap.IsChecked(dp)) {
				++removedCount;
				continue;
			}
			int newIndex = ri - removedCount;
			drawRects_[newIndex] = drawRects_[ri];
		}
		drawRects_.resize(drawRects_.size() - removedCount);

		rawScore_ = -1;
		evalScore_ = -1;

		return true;
	}
};
static constexpr int StateSize = sizeof(State);
static_assert(is_trivially_copyable<State>::value);

struct Solver {
	Xor64 rand_;

	State maxState_;
	void CheckMaxScore(const State& state) {
		if (state.RawScore() > maxState_.RawScore()) {
			State::Copy(state, maxState_);
		}
	}

	void Run(Result& result) {
		{
		}

		vector<State> states;
		InitState(states);
		State::Copy(states[0], maxState_);

		SimulatedAnnealing sa;
		sa.startTemp_ = HP.SparseStartTemp;
		sa.endTemp_ = HP.SparseEndTemp;
		sa.stepLoopCount = 100;

		sa.rollbackStartRate_ = HP.SparceRollbackStart;
		sa.rollbackCount_ = HP.SparseRollbackCount;		
		sa.rollbackNextMulti_ = HP.SparseRollbackNext;

		auto transitions = make_tuple(
			MAKE_TRANS(Transition_Random, HP.SparseTransRand)
			, MAKE_TRANS(Transition_EvalGready, HP.SparseTransEvalGready)
			, MAKE_TRANS(Transition_RawScoreRandGready, HP.SparseTransRawRand)		
		);
		sa.Run2(timer, states, transitions);


		for (const auto& rect : maxState_.RefDrawRects()) {
			result.rects.push_back(rect);
		}

	}

	void InitState(vector<State>& states) {
		states.resize(2);
		REP(i, 2) {
			auto& state = states[i];
			state.Init();
			if (i == 0) {
				state.PlayEvalGready(rand_);
			}
			else {
				state.PlayRawScoreRandGready(rand_);		
			}
		}

	}

	bool DeleteRectRandom(State& state) {
		const auto& drawRects = state.RefDrawRects();
		int upperCount = int(drawRects.size() * HP.SparseUpperDeleteRate);
		return state.DeleteRectRandomInt(0, upperCount, rand_);
	}

	State tmpState_;

	void Transition_Random(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.RefDrawRects().empty()) {
			return;
		}
		State::Copy(state, tmpState_);

		if (!DeleteRectRandom(tmpState_)) {
			return;
		}
		tmpState_.PlayRandom(rand_);

		if (checker(tmpState_.EvalScore())) {
			State::Copy(tmpState_, state);
			CheckMaxScore(state);
		}
	}

	void Transition_EvalGready(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.RefDrawRects().empty()) {
			return;
		}
		State::Copy(state, tmpState_);

		if (!DeleteRectRandom(tmpState_)) {
			return;
		}
		tmpState_.PlayEvalGready(rand_);

		if (checker(tmpState_.EvalScore())) {
			State::Copy(tmpState_, state);
			CheckMaxScore(state);
		}
	}

	void Transition_RawScoreRandGready(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.RefDrawRects().empty()) {
			return;
		}
		State::Copy(state, tmpState_);

		if (!DeleteRectRandom(tmpState_)) {
			return;
		}
		tmpState_.PlayRawScoreRandGready(rand_);

		if (checker(tmpState_.EvalScore())) {
			State::Copy(tmpState_, state);
			CheckMaxScore(state);
		}
	}
};
}; 

template <int CAP_N, int CAP_NN, int CAP_VALID, int CAP_DRAW>
struct Solver {
	void Run() {
		Result result;
		if (server.M >= 72) {
			constexpr int CAP_DEPEND_RECT = DependRectCountTbl[CAP_N];
			using TSolver = typename dense<CAP_NN, CAP_VALID, CAP_DRAW, CAP_DEPEND_RECT>::Solver;
			auto* solver = new TSolver();
			solver->Run(result);
		}
		else {
			using TSolver = typename sparse<CAP_NN, CAP_VALID, CAP_DRAW>::Solver;
			auto* solver = new TSolver();
			solver->Run(result);
		}

		server.Output(result);


	}
};

constexpr int CalcCapValid(int M) {
    int cap = int(1.681268289 * M - 33.12129963) + 300;
    return cap;
}
constexpr int CalcCapDraw(int M) {
    int cap = int(5.061583368 * M - 190.3946477) + 800;
    return cap;
}

template <int N, int M>
bool SolverRun() {
    if (server.N <= N && server.M <= M) {
        constexpr int Valid = CalcCapValid(M);
        constexpr int Draw = CalcCapDraw(M);
        auto* solver = new Solver<N, N * N, Valid, Draw>();
        solver->Run();
        return true;
    }
    return false;
}

struct Main {
    void Run(int argc, const char* argv[]) {
        server.InitInput();
        timer.StartMs(TIME_LIMIT);

#define REGIST(N, M) if (SolverRun<N, M>()) { return; }
#if defined(__clang_version__) || defined(_MSC_VER)
        [&] {
            REGIST(31, 80);
            REGIST(33, 90);
            REGIST(35, 102);
            REGIST(37, 114);
            REGIST(39, 126);
            REGIST(41, 140);
            REGIST(43, 154);
            REGIST(45, 168);
            REGIST(47, 184);
            REGIST(49, 200);
            REGIST(51, 216);
            REGIST(53, 234);
            REGIST(55, 252);
            REGIST(57, 270);
            REGIST(59, 290);
            REGIST(61, 310);
        }();
#elif defined(__GNUC__)
        [&] {
        }();
#endif

        server.Finalize();
    }
};
