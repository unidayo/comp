
#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0

#define TIME_LIMIT (2950)

#define NOT_SUBMIT 0
#define VALIDATION 0

#define IO_FILE 0

#define OUTPUT_INFO 0
#define OUTPUT_FINAL_INFO 0
#define OUTPUT_LOG 0
#define OUTPUT_VISUAL 0

#define FIX_RESULT 0


#ifndef _MSC_VER 
#pragma GCC target ("avx2")
#pragma GCC optimize "O3,omit-frame-pointer,inline"
#pragma GCC optimize ("unroll-loops")



#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"

#endif

#define _USE_MATH_DEFINES
#include <bits/stdc++.h>
using namespace std;


#define FOR(i, s, e) for (int i = int(s); i < int(e); ++i)
#define RFOR(i, s, e) for (int i = int(e) - 1; i >= int(s); --i)
#define REP(i, n) for (int i = 0, i##_size = int(n); i < i##_size; ++i)
#define RREP(i, n) for (int i = int(n) - 1; i >= 0; --i)
#define ALL(x) (x).begin(),(x).end()

#define rep(i, n) for (int i = 0, i##_size = int(n); i < i##_size; ++i)
#define rrep(i, n) for (int i = int(n) - 1; i >= 0; --i)
#define all(x) (x).begin(),(x).end()

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

static_assert(is_pod<pint>::value, "not pod");

template <class A, class B>
struct tuple_size<pr<A, B>> : integral_constant<size_t, 2> {};

template <class A, class B>
struct tuple_element<0, pr<A, B>> { using type = A; };
template <class A, class B>
struct tuple_element<1, pr<A, B>> { using type = B; };

static pint round(const pdouble& d) {
	return pint{ (int)round(d.x), (int)round(d.y) };
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



template <class T, int CAPACITY>
struct CapacityQueue {
private:
	array<T, CAPACITY> ar = {};
	int start = 0;
	int end = 0;

public:
	inline void clear() {
		start = 0;
		end = 0;
	}

	inline void push(const T& v) {
		ar[end] = v;
		++end;
	}

	inline T* push() {
		T* ptr = &ar[end];
		++end;
		return ptr;
	}

	inline const T& get() const  {
		return ar[start];
	}

	inline T pop() {
		return ar[start++];
	}

	inline bool empty() const {
		return start == end;
	}

	inline bool exist() const {
		return start != end;
	}

	inline int size() const {
		return end - start;
	}

	inline int total_push_count() const {
		return end;
	}

	const T& operator[](int i) const{
		return ar[i];
	}
	int end_size() const {
		return end;
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
		elemens.Clear();
		indexTable.Clear();
	}

	inline void Add(int ai) {
		indexTable.Set(ai, elemens.GetCount());
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


#define REGIST_PARAM(name, type, defaultValue) type name = defaultValue;




constexpr
struct {

	REGIST_PARAM(TEMP_START_K2_MIN, double, 18.54130977119276);
	REGIST_PARAM(TEMP_START_K2_MAX, double, 18.54130977119276);
	REGIST_PARAM(TEMP_END_K2_MIN, double, 1.36935898918667);
	REGIST_PARAM(TEMP_END_K2_MAX, double, 1.36935898918667);

	REGIST_PARAM(TEMP_START_K3_MIN, double, 10.0);
	REGIST_PARAM(TEMP_START_K3_MAX, double, 10.0);
	REGIST_PARAM(TEMP_END_K3_MIN, double, 0.1);
	REGIST_PARAM(TEMP_END_K3_MAX, double, 0.1);

	REGIST_PARAM(TEMP_START_K4_MIN, double, 10.0);
	REGIST_PARAM(TEMP_START_K4_MAX, double, 10.0);
	REGIST_PARAM(TEMP_END_K4_MIN, double, 0.1);
	REGIST_PARAM(TEMP_END_K4_MAX, double, 0.1);

	REGIST_PARAM(TEMP_START_K5_MIN, double, 10.0);
	REGIST_PARAM(TEMP_START_K5_MAX, double, 10.0);
	REGIST_PARAM(TEMP_END_K5_MIN, double, 0.1);
	REGIST_PARAM(TEMP_END_K5_MAX, double, 0.1);


	REGIST_PARAM(Transition_MoveMulti, int, 100);
	REGIST_PARAM(Transition_RevertMulti, int, 220);
	REGIST_PARAM(Transition_RevertMoveMulti, int, 100);
	REGIST_PARAM(Transition_MoveOut, int, 140);

	REGIST_PARAM(Transition_PriorityShuffle, int, 1);
	REGIST_PARAM(Transition_StartPc, int, 1);

} HP;

constexpr int N_MAX = 48;
constexpr int NN_MAX = N_MAX * N_MAX;

constexpr int K_MIN = 2;
constexpr int K_MAX = 5;

constexpr int PC_MIN = K_MIN * 100;
constexpr int PC_MAX = K_MAX * 100;

constexpr int N_MIN_K2 = 15;
constexpr int N_MIN_K3 = 18;
constexpr int N_MIN_K4 = 21;
constexpr int N_MIN_K5 = 24;

constexpr int N_MAX_K2 = 39;
constexpr int N_MAX_K3 = 42;
constexpr int N_MAX_K4 = 45;
constexpr int N_MAX_K5 = 48;

struct pcell {
	int a;
	int b;
};
struct Action {
	vector<pcell> moves;
	vector<pcell> connects;
};
template <class T>
using Grid = CapArr<T, NN_MAX>;




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

inline Dir RotateRight(Dir d) {
	return Dir(((int)d + 1) % 4);
}
inline Dir RotateLeft(Dir d) {
	return Dir(((int)d + 3) % 4);
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

inline const Dir CharToDir(char c) {
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
	void Init(int w, int h) {
		W = w;
		H = h;
		WH= W * H;
	}

	inline int ToId(int x, int y) const {
		return x + W * y;
	}
	inline int ToId(const pint& p) const {
		return p.x + W * p.y;
	}
	inline pint ToPos(int id) const {
		return pint{ id % W, id / W };
	}

	inline int CalcL1Dist(const pint& a, const pint& b) const {
		return abs(a.x - b.x) + abs(a.y - b.y);
	}
	inline int CalcL1Dist(int a, int b) const {
		return CalcL1Dist(ToPos(a), ToPos(b));
	}

	inline int CalcL2Dist2(const pint& a, const pint& b) const {
		return square(a.x - b.x) + square(a.y - b.y);
	}
	inline int CalcL2Dist2(int a, int b) const {
		return CalcL2Dist2(ToPos(a), ToPos(b));
	}

	inline double CalcL2Dist(const pint& a, const pint& b) const {
		return sqrt(CalcL2Dist2(a, b));
	}
	inline double CalcL2Dist(int a, int b) const {
		return CalcL2Dist(ToPos(a), ToPos(b));
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

template <class T, int CAP>
struct CCA {
private:
    T ar[CAP];
    int s;

public:
    inline constexpr void push(const T& v) {
        ar[s++] = v;
    }
    inline constexpr const T* begin() const {
        return &ar[0];
    }
    inline constexpr const T* end() const {
        return &ar[s];
    }

    inline constexpr const T& operator ()(int i) const {
        return ar[i];
    }
    inline constexpr const T& operator [](int i) const {
        return ar[i];
    }
    inline constexpr int size() const {
        return s;
    }
};

template <int W, int H, int AROUND_COUNT>
struct AroundMapS {
    using CA = CCA<int, AROUND_COUNT>;
    CA table[W * H];

    constexpr AroundMapS(const array<pint, AROUND_COUNT>& aroundDirs) : table{} {
        REP(cellId, W * H) {
            pint p = { cellId % W, cellId / W };
            for (const pint& a : aroundDirs) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                if (x >= 0 && x < W &&
                    y >= 0 && y < H) {
                    table[cellId].push(x + y * W);
                }
            }
        }
    }

    inline constexpr const CA& operator ()(int id) const {
        return table[id];
    }
    inline constexpr const CA& operator [](int id) const {
        return table[id];
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

template <int W, int H, int AROUND_COUNT>
struct DirMapS {
    using CA = CCA<int, AROUND_COUNT>;
    CA table[W * H];

    constexpr DirMapS(const array<pint, AROUND_COUNT>& aroundDirs) : table{} {
        REP(cellId, W * H) {
            pint p = { cellId % W, cellId / W };
            for (const pint& a : aroundDirs) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                int n = -1;
                if (x >= 0 && x < W &&
                    y >= 0 && y < H) {
                    n = x + y * W;
                }
                table[cellId].push(n);
            }
        }
    }

    inline constexpr const CA& operator ()(int id) const {
        return table[id];
    }
    inline constexpr const CA& operator [](int id) const {
        return table[id];
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

struct IOServer {
	int N = 0;
	int NN = 0;
	int K = 0;
	double D = 0;		
	Grid<u8> grid;
	GridSystemD gs;
	AroundMapD<4> aroundMap;
	DirMapD<4> dirMap;



	void InitInput() {
		istream& is = cin;
		is >> N >> K;
		timer.Init();			
		NN = N * N;
		grid.resize(NN);
		char c;
		REP(i, NN) {
			is >> c;
			grid[i] = c - '0';
		}
		D = (K * 100) / (double)(NN);

		gs.Init(N, N);
		aroundMap.Init(N, N, Around4);
		dirMap.Init(N, N, Around4);

	}

	void Output(const Action& action) {
		ostream& os = cout;
		auto& res = action;

		os << res.moves.size() << endl;
		for (const auto& m : res.moves) {
			pint a = gs.ToPos(m.a);
			pint b = gs.ToPos(m.b);
			os << a.y << " " << a.x << " " << b.y<< " " << b.x << endl;
		}

		os << res.connects.size() << endl;
		for (const auto& m : res.connects) {
			pint a = gs.ToPos(m.a);
			pint b = gs.ToPos(m.b);
			os << a.y << " " << a.x << " " << b.y << " " << b.x << endl;
		}

	}

	void Finalize() {
	}
};
IOServer server;

#define USE_SA_POINT_FILTER 1
#define USE_SA_ROLLBACK 1



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

constexpr int PROGRESS_COUNT = 10;			


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
	void Run2(ChronoTimer& timer, vector<STATE>& states, tuple<pair<TRANSITION, int>...>& transitions) {

		vector<STATE> maxStates = states;
		checkers.resize(states.size());
		totalMaxScore = states[0].score;
		REP(pi, checkers.size()) {
			auto& checker = checkers[pi];
			checker.totalMaxScore_ = &totalMaxScore;

			checker.divTemp = 0;
			checker.currentScore = states[pi].score;
			checker.maxScore = checker.currentScore;
			checker.noMaxUpdateCount = 0;
			checker.nextRollbackCheckCount = rollbackCount_;

			chmax(totalMaxScore, checker.maxScore);
		}

		RandomTable randTable;
		TupleLoop(transitions, [&](auto&& e, size_t i) {
			randTable.push((int)i, e.second);
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
						e.first(checker, states[pi]);

						if (states[pi].rawScore > maxStates[pi].rawScore) {
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

struct Histogram {
	int total_ = {};
	array<int, K_MAX> counts_ = {};

	void Clear() {
		total_ = 0;
		counts_ = {};
	}
	void Add(s8 k) {
		++total_;
		++counts_[k];
	}

	void Add(const Histogram& r) {
		total_ += r.total_;
		REP(k, server.K) {
			counts_[k] += r.counts_[k];
		}
	}

	int Score() const {
		int total = total_;
		int score = 0;
		REP(k, server.K) {
			int c = counts_[k];
			score += c * (c - 1) / 2;
			total -= c;
			score -= total * c;
		}
		return score;
	}
};

template <int CAPACITY>
struct RandomChoice {
private:
	CapArr<int, CAPACITY> table_;
	int n_ = 0;
	int choicedCount_ = 0;

public:
	void Init(int n) {
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
		return ret;
	}
};

constexpr double LinearParam(double ax, double ay, double bx, double by, double cx) {
	double r = (cx - ax) / (bx - ax);
	double cy = ay + (by - ay) * r;
	return cy;
}
int LinearParamInt(double ax, double ay, double bx, double by, double cx) {
	return (int)round(LinearParam(ax, ay, bx, by, cx));
}


#define VEFIRY_SCORE 0
#define VERIFY_STATE 0
#define SKIP_CHECK 0
#define OUTPUT_ANIMATION 0


#define USE_K_PC 0					
#define USE_PARAM_MOVE_COUNT 0		
#define USE_CENTER_PCED 0			
#define USE_OUTLINE 0				
#define USE_LINEMAP 0				
#define USE_TOP2_NO_HUB 0			
#define USE_MULTI_START 1			

using PcSet = CapSet<PC_MAX>;



constexpr int PcMaxMove = 3;		
constexpr int CellMaxMove = 10;		

using MoveStack = CapArr<s32, PcMaxMove>;
using PcStack = CapArr<s32, CellMaxMove>;
template <class T> using PcCapArr = CapArr<T, PC_MAX>;
using CellSet = CapSet<NN_MAX>;

#define LINE_REP(CELL, START, DIR, DIR_MAP) for (int CELL = DIR_MAP[START][(int)DIR]; CELL >= 0; CELL = DIR_MAP[CELL][(int)DIR])

struct State {
	Grid<s8> grid;							
	Grid<s32> pcGrid;						
	Grid<PcStack> pcStackGrid;				

	PcCapArr<int> pcCells;					
	PcCapArr<MoveStack> cellStacks;			
	PcSet movablePcSet;						
	PcSet revertablePcSet;					

	array<int, K_MAX> startPcs;			

	CapArr<u8, K_MAX> priority;				
	int moveCount;							

	int rawScore;
	double score;

	void Init() {
		const int N = server.N;
		const int NN = server.NN;
		const int K = server.K;
		const auto& gs = server.gs;
		const auto& serverGrid = server.grid;
		const auto& aroundMap = server.aroundMap;

		grid.assign(NN, -1);
		pcGrid.assign(NN, -1);
		pcStackGrid.resize(NN);

		pcCells.resize(K * 100);
		cellStacks.resize(K * 100);
		movablePcSet.Clear();
		revertablePcSet.Clear();

		array<int, K_MAX> pckIndexs = {};
		REP(cell, NN) {
			s8 k = (s8)serverGrid[cell] - 1;
			if (k < 0) {
				grid[cell] = -1;
				pcGrid[cell] = -1;
			}
			else {
				int pckIndex = pckIndexs[k]++;
				int pc = k * 100 + pckIndex;
				grid[cell] = k;
				pcGrid[cell] = pc;
				pcCells[pc] = cell;
				cellStacks[pc].push_back(cell);
				pcStackGrid[cell].push_back(pc);

				bool movable = false;
				for (int a : aroundMap[cell]) {
					if (serverGrid[a] == 0) {
						movable = true;
						break;
					}
				}
				if (movable) {
					movablePcSet.Add(pc);
				}
			}
		}
		REP(k, K) {
			startPcs[k] = k * 100;
		}

		priority.resize(K);
		iota(ALL(priority), 0);
		moveCount = 0;

		rawScore = 0;
		score = 0;
	}

	int GetCell(int pc) const {
		return pcCells[pc];
	}

	int GetPc(int cell) const {
		return pcGrid[cell];
	}

	bool CanMove(int from, int to) const {
		int pc = pcGrid[from];
		auto& cellStack = cellStacks[pc];

		{
			if (cellStack.size() >= 2 && to == cellStack[cellStack.size() - 2] && pcStackGrid[to].back() == pc) {
				return true;
			}
		}

		if (cellStack.size() == cellStack.capacity()) {
			return false;
		}

		const auto& pcStack = pcStackGrid[to];
		if (pcStack.size() == pcStack.capacity()) {
			return false;
		}

		if (moveCount >= server.K * 100) {
			return false;
		}

		return true;
	}

private:
	bool HasAroundBlank(int cell) const {
		for (int a : server.aroundMap[cell]) {
			if (pcGrid[a] < 0) {
				return true;
			}
		}
		return false;
	}

	bool CanMoveRevert(int from, int to) const {
		int pc = pcGrid[from];
		auto& cellStack = cellStacks[pc];
		if (cellStack.size() >= 2 && to == cellStack[cellStack.size() - 2] && pcStackGrid[to].back() == pc) {
			return true;
		}
		return false;
	}






	void UpdateSetWithPcIn(int cell) {
		int pc = pcGrid[cell];
		if (pc < 0) {
			return;
		}

		auto& cellStack = cellStacks[pc];

		if (!movablePcSet.IsContain(pc)) {
			return;
		}

		{
			bool revertable = false;
			if (cellStack.size() >= 2 && grid[cellStack[cellStack.size() - 2]]) {
				int prevCell = cellStack[cellStack.size() - 2];
				if (pcStackGrid[prevCell].back() == pc) {
					revertable = true;
				}
			}
			if (revertable) {

				return;
			}
			else {
				revertablePcSet.ForceRemove(pc);
			}
		}

		{
			bool movable = false;
			if (cellStack.size() < PcMaxMove) {
				if (HasAroundBlank(cell)) {
					movable = true;
				}
			}
			if (movable) {
			}
			else {
				movablePcSet.ForceRemove(pc);
			}
		}
	}

	void UpdateSetWithPcOut(int cell) {
		int pc = pcGrid[cell];
		if (pc < 0) {
			return;
		}

		auto& cellStack = cellStacks[pc];

		if (revertablePcSet.IsContain(pc)) {
			return;
		}

		{
			bool revertable = false;
			if (cellStack.size() >= 2 && grid[cellStack[cellStack.size() - 2]]) {
				int prevCell = cellStack[cellStack.size() - 2];
				if (pcStackGrid[prevCell].back() == pc) {
					revertable = true;
				}
			}
			if (revertable) {
				revertablePcSet.ForceAdd(pc);

				movablePcSet.ForceAdd(pc);
				return;
			}
			else {
			}
		}

		if (!movablePcSet.IsContain(pc)) {
			bool movable = false;
			if (cellStack.size() < PcMaxMove) {
				if (HasAroundBlank(cell)) {
					movable = true;
				}
			}
			if (movable) {
				movablePcSet.ForceAdd(pc);
			}
			else {
			}
		}
	}
public:

	void Move(int cell, int next) {
		int pc = pcGrid[cell];
		VASSERT(movablePcSet.IsContain(pc));

		auto& cellStack = cellStacks[pc];
		{
			if (cellStack.size() >= 2 && next == cellStack[cellStack.size() - 2] && pcStackGrid[next].back() == pc) {
				Revert(pc);
				return;
			}
		}

		swap(grid[cell], grid[next]);
		swap(pcGrid[cell], pcGrid[next]);
		pcCells[pc] = next;
		cellStacks[pc].push_back(next);
		pcStackGrid[next].push_back(pc);
		++moveCount;

		{
			revertablePcSet.ForceAdd(pc);			
			VASSERT(movablePcSet.IsContain(pc));	
		}

		for (int aroundCell : server.aroundMap[next]) {
			UpdateSetWithPcIn(aroundCell);
		}
		for (int aroundCell : server.aroundMap[cell]) {
			UpdateSetWithPcOut(aroundCell);
		}

		VASSERT(moveCount <= server.K * 100);
	}

	void Revert(int pc) {
		VASSERT(revertablePcSet.IsContain(pc));

		auto& cellStack = cellStacks[pc];
		VASSERT(cellStack.size() >= 2);
		int cur = cellStack.back();
		cellStack.pop_back();
		int prev = cellStack.back();
		VASSERT(pcStackGrid[prev].back() == pc);

		swap(grid[cur], grid[prev]);
		swap(pcGrid[cur], pcGrid[prev]);
		pcCells[pc] = prev;
		pcStackGrid[cur].pop_back();
		--moveCount;

		{
			VASSERT(movablePcSet.IsContain(pc));  

			bool revertable = false;
			if (cellStack.size() >= 2 && grid[cellStack[cellStack.size() - 2]]) {
				int prevCell = cellStack[cellStack.size() - 2];
				if (pcStackGrid[prevCell].back() == pc) {
					revertable = true;
				}
			}
			if (!revertable) {
				revertablePcSet.ForceRemove(pc);
			}
		}

		for (int aroundCell : server.aroundMap[prev]) {
			UpdateSetWithPcIn(aroundCell);
		}
		for (int aroundCell : server.aroundMap[cur]) {
			UpdateSetWithPcOut(aroundCell);
		}

		VASSERT(moveCount >= 0);
	}

};
static constexpr int StateSize = sizeof(State);

struct Solver {
	Xor64 rand_;


	State maxState_;
	double maxStateProgress_ = 0;

	void CheckMaxScore(const State& state, double progress) {
		if (state.rawScore > maxState_.rawScore) {
			maxState_ = state;
			maxStateProgress_ = progress;

		}
	}

	void Run() {
		const auto& gs = server.gs;
		const int N = server.N;
		const int NN = server.NN;
		const int K = server.K;

		{
		}

		vector<State> states = {};
		InitState(states);
		maxState_ = states[0];
		maxStateProgress_ = 0;

		SimulatedAnnealing sa;
		if (K == 2) {
			sa.startTemp_ = LinearParam(N_MIN_K2, HP.TEMP_START_K2_MIN, N_MAX_K2, HP.TEMP_START_K2_MAX, N);
			sa.endTemp_ = LinearParam(N_MIN_K2, HP.TEMP_END_K2_MIN, N_MAX_K2, HP.TEMP_END_K2_MAX, N);
		}
		else if (K == 3) {
			sa.startTemp_ = LinearParam(N_MIN_K3, HP.TEMP_START_K3_MIN, N_MAX_K3, HP.TEMP_START_K3_MAX, N);
			sa.endTemp_   = LinearParam(N_MIN_K3, HP.TEMP_END_K3_MIN,   N_MAX_K3, HP.TEMP_END_K3_MAX,   N);
		}
		else if (K == 4) {
			sa.startTemp_ = LinearParam(N_MIN_K4, HP.TEMP_START_K4_MIN, N_MAX_K4, HP.TEMP_START_K4_MAX, N);
			sa.endTemp_ = LinearParam(N_MIN_K4, HP.TEMP_END_K4_MIN, N_MAX_K4, HP.TEMP_END_K4_MAX, N);
		}
		else if (K == 5) {
			sa.startTemp_ = LinearParam(N_MIN_K5, HP.TEMP_START_K5_MIN, N_MAX_K5, HP.TEMP_START_K5_MAX, N);
			sa.endTemp_ = LinearParam(N_MIN_K5, HP.TEMP_END_K5_MIN, N_MAX_K5, HP.TEMP_END_K5_MAX, N);
		}
		else {
			VABORT();
		}
		sa.stepLoopCount = 100;

		sa.rollbackStartRate_ = 0.1;
		sa.rollbackCount_ = 10000;		
		sa.rollbackNextMulti_ = 1.1;

#define MAKE_TRANS(func, weight) make_pair([&](SAChecker& sac, State& state) { func(sa, sac, state); }, weight)


		auto transitions = make_tuple(
			MAKE_TRANS(Transition_MoveMulti, (int)HP.Transition_MoveMulti)			
			, MAKE_TRANS(Transition_RevertMulti, (int)HP.Transition_RevertMulti)				
			, MAKE_TRANS(Transition_RevertMoveMulti, (int)HP.Transition_RevertMoveMulti)		
			, MAKE_TRANS(Transition_MoveOut, (int)HP.Transition_MoveOut)				
			, MAKE_TRANS(Transition_PriorityShuffle, (int)HP.Transition_PriorityShuffle)		
			, MAKE_TRANS(Transition_StartPc, (int)HP.Transition_StartPc)				
		);

		sa.Run2(timer, states, transitions);

		Action action;
		{
			int rawScore = StateAction(maxState_, action);

		}

		server.Output(action);
	}

	int StateAction(const State& state, Action& action) {
		int NN = server.NN;
		PcCapArr<u8> cellStackIndexs(server.K * 100);		
		Grid<u8> pcStackIndexGrid(NN);		
		REP(pc, server.K * 100) {
			MoveDFS(maxState_, pc, cellStackIndexs, pcStackIndexGrid, action);
		}
		VASSERT(action.moves.size() == maxState_.moveCount);

		int rawScore = 0;
		int margeCount = 0;
		double score = Eval(maxState_, maxStateProgress_, rawScore, &action, &margeCount);
		VASSERT(rawScore >= maxState_.rawScore);		
		return rawScore;
	}

	void MoveDFS(const State& state, int pc, PcCapArr<u8>& cellStackIndexs, Grid<u8>& pcStackIndexGrid, Action& action) {
		const auto& cellStack = state.cellStacks[pc];
		const auto& pcStackGrid = state.pcStackGrid;
		u8& cellStackIndex = cellStackIndexs[pc];		
		while (true) {
			if (cellStackIndex + 1 >= cellStack.size()) {
				break;
			}

			int cur = cellStack[cellStackIndex];
			int next = cellStack[cellStackIndex + 1];
			int targetPc = pcStackGrid[next][pcStackIndexGrid[next]];
			if (targetPc != pc) {
				MoveDFS(state, targetPc, cellStackIndexs, pcStackIndexGrid, action);
			}
			else {
				action.moves.push_back(pcell{ cur, next });
				++cellStackIndex;
				++pcStackIndexGrid[cur];
			}
		}
	}

	using ClusterId = s8;		
	struct ClusterManager {
	private:
		int totalScore = 0;
		ClusterId generatingClusterId = -1;
		CheckMapDataD<ClusterId> clusterGrid;				
		CheckMapD hubGrid;							
		CapArr<Histogram, 128> histograms;		
		CapArr<bool, 128> enableClusters;		
		mutable CapArr<ClusterId, 128> parentClusters;	

		inline ClusterId RootCluster(ClusterId clusterId) const {
			if (parentClusters[clusterId] < 0) {
				return clusterId;
			}
			return parentClusters[clusterId] = RootCluster(parentClusters[clusterId]);
		}

	public:
		void Init() {
			int NN = server.NN;
			totalScore = 0;
			generatingClusterId = -1;
			clusterGrid.SetSize(NN);
			clusterGrid.Clear();
			hubGrid.SetSize(NN);
			hubGrid.Clear();
			histograms.clear();
			enableClusters.clear();
			parentClusters.clear();
		}

		int NewCluster() {
			VASSERT(generatingClusterId < 0);
			generatingClusterId = histograms.size();
			histograms.PushBack()->Clear();
			enableClusters.push_back(true);
			parentClusters.push_back(-1);
			return generatingClusterId;
		}

		void AddPc(int cell, s8 k) {
			VASSERT(generatingClusterId >= 0);
			histograms.back().Add(k);
			clusterGrid.Set(cell, generatingClusterId);
		}

		void PopCluster(int cell) {
			VASSERT(generatingClusterId >= 0);

			histograms.pop_back();
			enableClusters.pop_back();
			parentClusters.pop_back();
			clusterGrid.Reset(cell);
			generatingClusterId = -1;
		}

		void FinalizeCluster() {
			VASSERT(generatingClusterId >= 0);

			totalScore += histograms.back().Score();
			generatingClusterId = -1;
		}

		const Histogram& RefHistogram(int clusterId) const {
			return histograms[clusterId];
		}

		ClusterId GetCluster(int cell) const {
			if (!clusterGrid.IsChecked(cell)) {
				return -1;
			}
			return RootCluster(clusterGrid.Get(cell));
		}

		bool IsHub(int cell) const {
			return hubGrid.IsChecked(cell);
		}

		void AddHub(int hubCell) {
			hubGrid.Check(hubCell);
		}

		void Marge(int mainClusterId, int subClusterId, const Histogram& margedHistogram) {
			totalScore -= histograms[mainClusterId].Score();
			totalScore -= histograms[subClusterId].Score();
			totalScore += margedHistogram.Score();

			histograms[mainClusterId] = margedHistogram;
			histograms[subClusterId].Clear();
			enableClusters[subClusterId] = false;

			VASSERT(parentClusters[mainClusterId] < 0);
			parentClusters[RootCluster(subClusterId)] = mainClusterId;
		}


		int Score() const {
			return totalScore;

		}
	};

	using UsedGrid = Grid<u8>;		

	double Eval(const State& state, double progress, int& rawScore, Action* action = nullptr, int* margeCount = nullptr) {
		const auto& gs = server.gs;
		const auto& grid = state.grid;
		const auto& pcGrid = state.pcGrid;
		const auto& pcCells = state.pcCells;
		const auto& priority = state.priority;
		const int K = server.K;

		rawScore = 0;

		int maxK = 2;		
		if (progress >= 0.9 || action) {
			maxK = K;
		}
		const int actionLimit = server.K * 100;

		int NN = server.NN;
		static UsedGrid used;
		used.assign(NN, 0);
		int actionCount = state.moveCount;
		int length = 0;

		static array<ClusterManager, K_MAX> managers;		
		REP(pi, maxK) {
			const int k = priority[pi];
			managers[k].Init();
		}

		REP(pi, 2) {
			if (actionCount >= actionLimit) {
				break;
			}
			const int k = priority[pi];
			auto& manager = managers[k];


			int pc = state.startPcs[k];
			int cell = pcCells[pc];
			if (used[cell]) {
				continue;
			}

			ConnectBFS(
				grid,
				used, 
				cell, 
				actionCount, 
				length, 
				manager, 
				true,
				actionLimit,
				action,
				margeCount);
		}

		{
			array<int, 2> indexs = {};
			array<int, 2> scores = {
				managers[priority[0]].Score(),
				managers[priority[1]].Score()
			};
			while (true) {
				if (actionCount >= actionLimit) {
					break;
				}

				int pi = 0;
				if (indexs[0] == 100 && indexs[1] == 100) {
					break;
				}
				if (indexs[0] == 100) {
					pi = 1;
				}
				else if (indexs[1] == 100) {
					pi = 0;
				}
				else {
					if (K == 2) {
						pi = 1;		
					}
					else {
						pi = scores[0] < scores[1] ? 0 : 1;
					}
				}

				const int k = priority[pi];
				int basePc = k * 100;
				auto& manager = managers[k];


				int& index = indexs[pi];
				while (index < 100) {
					int pc = basePc + index++;
					int cell = pcCells[pc];
					if (used[cell]) {
						continue;
					}

					ConnectBFS(
						grid,
						used,
						cell,
						actionCount,
						length,
						manager,
						true,
						actionLimit,
						action,
						margeCount);

					if (actionCount >= actionLimit) {
						break;
					}

					if (K != 2) {		
						int newScore = manager.Score();
						if (newScore != scores[pi]) {
							scores[pi] = newScore;
							break;
						}
					}
				}
			}
		}

		FOR(pi, 2, maxK) {
			if (actionCount >= actionLimit) {
				break;
			}
			const int k = priority[pi];
			int basePc = k * 100;

			auto& manager = managers[k];
			bool enableMarge = false;


			REP(offset, 100) {
				int pc = basePc + offset;
				int cell = pcCells[pc];
				if (used[cell]) {
					continue;
				}

				ConnectBFS(
					grid,
					used, 
					cell, 
					actionCount, 
					length, 
					manager, 
					enableMarge,
					actionLimit,
					action,
					margeCount);
				if (actionCount >= actionLimit) {
					break;
				}
			}
		}

		REP(pi, maxK) {
			const int k = priority[pi];
			rawScore += managers[k].Score();
		}

		double rate = progress / 0.8;
		chmin(rate, 1.0);

		double score = 0;
		score = rawScore;

		double pena = 0;
		pena += actionCount * 0.1;
		pena += state.moveCount * 1;
		if (K == 5) {
			pena += length / (double)NN;		
		}

		score -= pena * (1.0 - rate);
		return score;
	}

	struct FromHub {
		int from;
		int hub;
		bool canConnect;		
	};

	void ConnectBFS(
		const Grid<s8>&grid, 
		UsedGrid &used,
		int start, 
		int& actionCount,
		int& length, 
		ClusterManager& manager,
		bool enableMarge,
		int actionLimit,
		Action * action,
		int* margeCount) {
		const int N = server.N;
		const int NN = server.NN;
		const auto& gs = server.gs;
		const auto& dirMap = server.dirMap;
		const s8 k = grid[start];

		if (actionCount >= actionLimit) {
			return;
		}

		static CapQue<int, NN_MAX> que;
		que.clear();

		int clusterId = manager.NewCluster();
		static CapArr<FromHub, NN_MAX> fromHubs;		
		fromHubs.clear();

		que.push(start);
		used[start] = 1;
		manager.AddPc(start, k);

		while (!que.empty()) {
			int center = que.pop();

			[&] {
				REP(dir, 4) {
					int to = -1;
					LINE_REP(next, center, dir, dirMap) {
						if (used[next]) {
							break;
						}
						s8 nk = grid[next];
						if (nk >= 0) {
							if (nk != k) {
									auto ptr = fromHubs.PushBack();
									ptr->from = center;
									ptr->hub = next;
									ptr->canConnect = true;
							}
							else {
								to = next;
							}
							break;
						}
					}
					if (to >= 0) {
						int cur = center;
						while (true) {
							int next = dirMap[cur][dir];
							used[next] = 1;
							if (next == to) {
								break;
							}
							++length;
							cur = next;
						}

						que.push(to);
						manager.AddPc(to, k);
						++actionCount;
						if (action) {
							action->connects.push_back(pcell{ center, to });
						}

						if (actionCount >= actionLimit) {
							return;
						}
					}
				}
			}();
			if (actionCount >= actionLimit) {
				break;
			}
		}

		if (manager.RefHistogram(clusterId).total_ <= 1) {
			manager.PopCluster(start);
			used[start] = 0;		
			return;
		}
		manager.FinalizeCluster();

		if (actionCount >= actionLimit) {
			return;
		}

		if (enableMarge) {
			MergeCluster(grid, used, actionCount, length, manager, clusterId, fromHubs, action, margeCount);

			for (auto& fromHub : fromHubs) {
				if (!fromHub.canConnect) {
					continue;
				}

				manager.AddHub(fromHub.hub);
			}
		}
	}

	void MergeCluster(const Grid<s8>& grid, UsedGrid& used, int& actionCount, int& length, ClusterManager& manager, int clusterId, CapArr<FromHub, NN_MAX>& fromHubs, Action* action, int* margeCount) {
		const auto& gs = server.gs;
		const auto& dirMap = server.dirMap;
		const int actionLimit = server.K * 100;

		if (actionCount + 2 > actionLimit) {
			return;
		}

		auto CanConnectWithDir = [&](int from, int to, Dir dir) {
			LINE_REP(cell, from, dir, dirMap) {
				if (cell == to) {
					break;
				}
				if (used[cell]) {
					return false;
				}
			}
			return true;
		};

		auto CanConnect = [&](int from, int to) {
			Dir dir = gs.CalcDirM(from, to);
			return CanConnectWithDir(from, to, dir);
		};

		auto SetConnect = [&](int from, int to) {
			Dir dir = gs.CalcDirM(from, to);
			LINE_REP(cell, from, dir, dirMap) {
				used[cell] = 1;
				if (cell == to) {
					break;
				}
			}
		};

		struct Candidate {
			bool enable;
			int from;
			int hub;
			int otherId;
			int otherCell;
			int length;			
		};
		static array<Candidate, NN_MAX> candidates;
		static CapSet<NN_MAX> enableClusters;
		enableClusters.Clear();

		REP(i, fromHubs.size()) {
			auto& fromHub = fromHubs[i];
			int from = fromHub.from;
			int hub = fromHub.hub;
			if (!manager.IsHub(hub)) {
				continue;
			}

			Dir toHubDir = gs.CalcDirM(from, hub);
			if (!CanConnectWithDir(from, hub, toHubDir)) {
				fromHub.canConnect = false;
				continue;
			}

			int myLength = gs.CalcL1Dist(fromHub.from, fromHub.hub) - 1;

			Dir fromHubToSelfDir = Back(toHubDir);
			REP(dir, 4) {
				if ((Dir)dir == fromHubToSelfDir) {
					continue;
				}
				int toOtherLength = 0;
				LINE_REP(cell, hub, dir, dirMap) {
					auto otherCluster = manager.GetCluster(cell);
					if (otherCluster >= 0) {
						if (otherCluster == clusterId) {
							break;
						}

						int length = myLength + toOtherLength;
						if (!enableClusters.IsContain(otherCluster)) {
							enableClusters.Add(otherCluster);
							auto& candidate = candidates[otherCluster];
							candidate = {};
							candidate.length = INT_MAX;
						}

						auto& candidate = candidates[otherCluster];
						if (length < candidate.length) {
							candidate.enable = true;
							candidate.from = fromHub.from;
							candidate.hub = fromHub.hub;
							candidate.otherId = otherCluster;
							candidate.otherCell = cell;
							candidate.length = length;
						}
						break;
					}
					if (used[cell]) {
						break;
					}
					if (grid[cell] >= 0) {
						break;
					}
					++toOtherLength;
				}
			}
		}

		const auto& histogram = manager.RefHistogram(clusterId);
		while (actionCount + 2 <= actionLimit) {
			int selfScore = histogram.Score();		

			double bestEvalScore = 0;
			int bestClusterId = -1;
			Histogram bestMargedHistogram;
			for (int otherClusterId : enableClusters) {
				auto& candidate = candidates[otherClusterId];
				if (!candidate.enable) {
					continue;
				}
				int from = candidate.from;
				int hub = candidate.hub;

				if (used[hub]) {
					candidate.enable = false;
					continue;
				}

				if (!CanConnect(from, hub)) {
					candidate.enable = false;
					continue;
				}

				if (!CanConnect(hub, candidate.otherCell)) {
					candidate.enable = false;
					continue;
				}

				const auto& otherHistogram = manager.RefHistogram(candidate.otherId);
				int nowScore = selfScore + otherHistogram.Score();

				Histogram marged = histogram;
				marged.Add(otherHistogram);
				marged.Add(grid[hub]);
				int margedScore = marged.Score();

				int incScore = margedScore - nowScore;
				if (incScore <= 0) {		
					candidate.enable = false;
					continue;
				}

				double evalScore = incScore - candidate.length * 0.001;
				if (evalScore > bestEvalScore) {
					bestEvalScore = evalScore;
					bestClusterId = otherClusterId;
					bestMargedHistogram = marged;
				}
			}
			if (bestClusterId < 0) {
				break;
			}

			auto& candidate = candidates[bestClusterId];
			manager.Marge(clusterId, candidate.otherId, bestMargedHistogram);
			SetConnect(candidate.from, candidate.hub);
			SetConnect(candidate.hub, candidate.otherCell);
			actionCount += 2;
			if (action) {
				action->connects.push_back(pcell{ candidate.from, candidate.hub });
				action->connects.push_back(pcell{ candidate.hub, candidate.otherCell });
			}
			if (margeCount) {
				++(*margeCount);
			}
			length += candidate.length;
			candidate.enable = false;
		}

	}

	void InitState(vector<State>&states) {
		State state;
		state.Init();

		constexpr int tbl[11][4] = {
{6, 0, 0, 0},
{6, 6, 0, 0},
{4, 4, 6, 0},
{2, 4, 3, 2},
{2, 4, 3, 2},
{2, 3, 2, 2},
{2, 2, 2, 2},
{2, 2, 2, 2},
{0, 2, 2, 2},
{0, 0, 2, 2},
{0, 0, 0, 2},

		};

		int ti = [&] {
			const int N = server.N;
			if (N < 36) {
				return (N - 15) / 3;
			}
			else if (N < 40) {
				return 7;
			}
			else {
				return 8 + (N - 40) / 3;
			}
		}();
		int tj = server.K - 2;
		int startCount = tbl[ti][tj];

		REP(i, startCount) {
			if (i != 0) {
				shuffle(ALL(state.priority), rand_);		
			}
			state.rawScore = 0;
			state.score = Eval(state, 0, state.rawScore);
			states.push_back(state);
		}
	}


	void Transition_Move(SimulatedAnnealing & sa, SAChecker & checker, State & state) {
		int pc = state.movablePcSet[(int)rand_(state.movablePcSet.size())];

		if (state.cellStacks[pc].size() >= PcMaxMove) {
			return;
		}
		int cell = state.pcCells[pc];

		int next = -1;
		auto& around = server.aroundMap[cell];
		int offset = (int)rand_(around.size());
		REP(i, around.size()) {
			int ai = offset + i;
			if (ai >= around.size()) {
				ai -= around.size();
			}
			int n = around[ai];
			if (state.grid[n] >= 0) {
				continue;
			}
			if (state.pcStackGrid[n].size() >= CellMaxMove) {
				continue;
			}
			next = n;
			break;
		}
		if (next < 0) {
			return;
		}

		state.Move(cell, next);

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			state.Move(next, cell);
		}
	}

	static constexpr int MultiMoveCapacity = 4;
	const int MultiMoveCountMax = [&] {
		const int K = server.K;
		const int N = server.N;
		if (K == 2) {
			return LinearParamInt(N_MIN_K2, 2, N_MAX_K2, 3, N);
		}
		else if (K == 3) {
			return LinearParamInt(N_MIN_K3, 2, N_MAX_K3, 3, N);
		}
		else if (K == 4) {
			return LinearParamInt(N_MIN_K4, 3, N_MAX_K4, 3, N);
		}
		else { 
			return LinearParamInt(N_MIN_K5, 3, N_MAX_K5, 3, N);
		}
	}();

	void Transition_MoveMulti(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		VASSERT(!state.movablePcSet.empty());

		int pc = state.movablePcSet[(int)rand_(state.movablePcSet.size())];

		CapArr<pcell, MultiMoveCapacity> history;
		int startMoveCount = state.moveCount;
		int moveCount = 1 + (int)rand_(MultiMoveCountMax);

		RandomChoice<4> rc;

		int lastCell = -1;
		REP(i, moveCount) {
			int from = state.pcCells[pc];
			int next = -1;
			auto& around = server.aroundMap[from];
			rc.Init(around.size());
			REP(i, around.size()) {
				int to = around[rc.Choice(rand_)];
				if (to == lastCell) {
					continue;
				}
				if (state.grid[to] >= 0) {
					continue;
				}
				if (!state.CanMove(from, to)) {
					continue;
				}
				next = to;
				break;
			}
			if (next < 0) {
				break;
			}
			state.Move(from, next);
			history.push_back(pcell{ from, next });
			lastCell = from;
		}

		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			RREP(hi, history.size()) {
				const auto& p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_Revert(SimulatedAnnealing & sa, SAChecker & checker, State & state) {
		if (state.revertablePcSet.empty()) {
			return;
		}

		int pc = state.revertablePcSet[(int)rand_(state.revertablePcSet.size())];
		const auto& cellStakcs = state.cellStacks[pc];
		VASSERT(cellStakcs.size() >= 2);
		int cell = cellStakcs[cellStakcs.size() - 1];
		int prev = cellStakcs[cellStakcs.size() - 2];

		state.Revert(pc);

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			state.Move(prev, cell);
		}
	}

	void Transition_RevertMulti(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.revertablePcSet.empty()) {
			return;
		}

		int pc = state.revertablePcSet[(int)rand_(state.revertablePcSet.size())];

		CapArr<pcell, MultiMoveCapacity> history;
		int startMoveCount = state.moveCount;
		int moveCount = 1 + (int)rand_(MultiMoveCountMax);

		const auto& cellStakcs = state.cellStacks[pc];

		REP(i, moveCount) {
			if (cellStakcs.size() <= 1) {
				continue;
			}
			int cell = cellStakcs[cellStakcs.size() - 1];
			int prev = cellStakcs[cellStakcs.size() - 2];

			if (state.pcStackGrid[prev].back() != pc) {
				break;
			}

			state.Revert(pc);
			history.push_back(pcell{ cell, prev });
		}

		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			RREP(hi, history.size()) {
				const auto& p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_RevertMoveMulti(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.revertablePcSet.empty()) {
			return;
		}

		CapArr<pcell, MultiMoveCapacity * 2> history;
		int startMoveCount = state.moveCount;

		int pc = state.revertablePcSet[(int)rand_(state.revertablePcSet.size())];

		{
			const auto& cellStakcs = state.cellStacks[pc];

			int moveCount = 1 + (int)rand_(MultiMoveCountMax);
			REP(i, moveCount) {
				if (cellStakcs.size() <= 1) {
					continue;
				}
				int cell = cellStakcs[cellStakcs.size() - 1];
				int prev = cellStakcs[cellStakcs.size() - 2];

				if (state.pcStackGrid[prev].back() != pc) {
					break;
				}

				state.Revert(pc);
				history.push_back(pcell{ cell, prev });
			}
		}

		if (state.cellStacks[pc].size() < PcMaxMove) {
			RandomChoice<4> rc;

			int moveCount = 1 + (int)rand_(MultiMoveCountMax);
			int lastCell = -1;
			REP(i, moveCount) {
				int from = state.pcCells[pc];
				int next = -1;
				auto& around = server.aroundMap[from];
				rc.Init(around.size());
				REP(i, around.size()) {
					int to = around[rc.Choice(rand_)];
					if (to == lastCell) {
						continue;
					}
					if (state.grid[to] >= 0) {
						continue;
					}
					if (!state.CanMove(from, to)) {
						continue;
					}
					next = to;
					break;
				}
				if (next < 0) {
					break;
				}
				state.Move(from, next);
				history.push_back(pcell{ from, next });
				lastCell = from;
			}
		}

		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			RREP(hi, history.size()) {
				const auto& p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_RevertMoveMulti2(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		if (state.revertablePcSet.empty()) {
			return;
		}

		CapArr<pcell, MultiMoveCapacity * 4> history;
		int startMoveCount = state.moveCount;

		REP(loop, 2) {
			if (state.revertablePcSet.empty()) {
				break;
			}

			int pc = state.revertablePcSet[(int)rand_(state.revertablePcSet.size())];

			{
				const auto& cellStakcs = state.cellStacks[pc];

				int moveCount = 1 + (int)rand_(MultiMoveCountMax);
				REP(i, moveCount) {
					if (cellStakcs.size() <= 1) {
						continue;
					}
					int cell = cellStakcs[cellStakcs.size() - 1];
					int prev = cellStakcs[cellStakcs.size() - 2];

					if (state.pcStackGrid[prev].back() != pc) {
						break;
					}

					state.Revert(pc);
					history.push_back(pcell{ cell, prev });
				}
			}

			if (state.cellStacks[pc].size() >= PcMaxMove) {
				continue;
			}

			{
				RandomChoice<4> rc;

				int moveCount = 1 + (int)rand_(MultiMoveCountMax);
				int lastCell = -1;
				REP(i, moveCount) {
					int from = state.pcCells[pc];
					int next = -1;
					auto& around = server.aroundMap[from];
					rc.Init(around.size());
					REP(i, around.size()) {
						int to = around[rc.Choice(rand_)];
						if (to == lastCell) {
							continue;
						}
						if (state.grid[to] >= 0) {
							continue;
						}
						if (!state.CanMove(from, to)) {
							continue;
						}
						next = to;
						break;
					}
					if (next < 0) {
						break;
					}
					state.Move(from, next);
					history.push_back(pcell{ from, next });
					lastCell = from;
				}
			}
		}

		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			RREP(hi, history.size()) {
				const auto& p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_RevertMultiAny(SimulatedAnnealing & sa, SAChecker & checker, State & state) {
		constexpr int MaxRevertCount = 5;
		int revertCount = 1 + (int)rand_(MaxRevertCount);

		CapArr<pcell, MaxRevertCount> backups;
		backups.clear();

		REP(rci, revertCount) {

			int pc = (int)rand_(state.pcCells.size());
			const auto& cellStakcs = state.cellStacks[pc];
			if (cellStakcs.size() <= 1) {
				continue;
			}
			int cell = cellStakcs[cellStakcs.size() - 1];
			int prev = cellStakcs[cellStakcs.size() - 2];

			if (state.pcStackGrid[prev].back() != pc) {
				continue;
			}

			state.Revert(pc);
			backups.push_back(pcell{ prev, cell });
		}

		if (backups.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			RREP(i, backups.size()) {
				const auto& p = backups[i];
				state.Move(p.a, p.b);
			}
		}
	}


	static constexpr int MoveOutCapacity = 4;
	const int MoveOutCountMax = [&] {
		const int K = server.K;
		const int N = server.N;
		const double D = server.D;
		if (K == 2) {
			return 1;
		}
		if (D >= 0.5) {
			return 1;
		}
		return 2;


	}();

	void Transition_MoveOut(SimulatedAnnealing & sa, SAChecker & checker, State & state) {
		int pc = (int)rand_(state.pcCells.size());

		auto MoveOutCell = [&](int cell) -> int {
			const auto& around = server.aroundMap[cell];
			int offset = (int)rand_(around.size());
			REP(i, around.size()) {
				int ai = offset + i;
				if (ai >= around.size()) {
					ai -= around.size();
				}
				int nextCell = around[ai];
				int nextPc = state.GetPc(nextCell);
				if (nextPc < 0) {
					if (state.CanMove(cell, nextCell)) {
						return nextCell;
					}
				}
			}
			return -1;
		};

		CapArr<pcell, MoveOutCapacity * 2> history;		
		int startMoveCount = state.moveCount;

		int curCell = state.GetCell(pc);

		const int moveCount = 1 + (int)rand_(MoveOutCountMax);

		int lastCell = -1;
		REP(mi, moveCount) {
			const auto& around = server.aroundMap[curCell];
			int nextCell = around[(int)rand_(around.size())];
			if (nextCell == lastCell) {
				continue;
			}
			int nextPc = state.GetPc(nextCell);
			if (nextPc >= 0) {
				if (!state.movablePcSet.IsContain(nextPc)) {
					break;
				}
				int moveOutCell = MoveOutCell(nextCell);
				if (moveOutCell >= 0) {
					state.Move(nextCell, moveOutCell);
					history.push_back(pcell{ nextCell, moveOutCell });
				}
				else {
					break;
				}
			}
			if (!state.CanMove(curCell, nextCell)) {
				break;
			}
			state.Move(curCell, nextCell);
			history.push_back(pcell{ curCell, nextCell });

			lastCell = curCell;
			curCell = nextCell;
		}
		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);
		}
		else {
			RREP(hi, history.size()) {
				pcell p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}


	void Transition_MoveOutEx(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		int pc = (int)rand_(state.pcCells.size());

		auto MoveOutCell = [&](int cell) -> int {
			const auto& around = server.aroundMap[cell];
			int offset = (int)rand_(around.size());
			REP(i, around.size()) {
				int ai = offset + i;
				if (ai >= around.size()) {
					ai -= around.size();
				}
				int nextCell = around[ai];
				int nextPc = state.GetPc(nextCell);
				if (nextPc < 0) {
					if (state.CanMove(cell, nextCell)) {
						return nextCell;
					}
				}
			}
			return -1;
		};

		CapArr<pcell, MoveOutCapacity * 2> history;		
		const int moveCount = 1 + (int)rand_(MoveOutCountMax);
		int startMoveCount = state.moveCount;

		int curCell = state.GetCell(pc);

		RandomChoice<4> randomDir;
		int lastCell = -1;

		REP(mi, moveCount) {
			const auto& around = server.aroundMap[curCell];
			randomDir.Init(around.size());

			bool moved = false;
			REP(i, around.size()) {
				int ai = randomDir.Choice(rand_);
				int nextCell = around[ai];
				if (nextCell == lastCell) {
					continue;
				}

				int nextPc = state.GetPc(nextCell);
				bool moveOut = false;
				if (nextPc >= 0) {
					if (!state.movablePcSet.IsContain(nextPc)) {
						continue;
					}
					int moveOutCell = MoveOutCell(nextCell);
					if (moveOutCell >= 0) {
						state.Move(nextCell, moveOutCell);
						history.push_back(pcell{ nextCell, moveOutCell });
						moveOut = true;
					}
					else {
						continue;
					}
				}

				if (!state.CanMove(curCell, nextCell)) {
					if (moveOut) {
						pcell p = history.back();
						history.pop_back();
						state.Move(p.b, p.a);
					}
					continue;
				}

				state.Move(curCell, nextCell);
				history.push_back(pcell{ curCell, nextCell });
				lastCell = curCell;
				curCell = nextCell;
				moved = true;
				break;
			}
			if (!moved) {
				break;
			}
		}

		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);
		}
		else {
			RREP(hi, history.size()) {
				pcell p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_MoveOutDir(SimulatedAnnealing & sa, SAChecker & checker, State & state) {
		int pc = (int)rand_(state.pcCells.size());

		auto MoveOutCell = [&](int cell) -> int {
			const auto& around = server.aroundMap[cell];
			int offset = (int)rand_(around.size());
			REP(i, around.size()) {
				int ai = offset + i;
				if (ai >= around.size()) {
					ai -= around.size();
				}
				int nextCell = around[ai];
				int nextPc = state.GetPc(nextCell);
				if (nextPc < 0) {
					if (state.CanMove(cell, nextCell)) {
						return nextCell;
					}
				}
			}
			return -1;
		};

		CapArr<pcell, 6> history;
		int startMoveCount = state.moveCount;

		int curCell = state.GetCell(pc);

		int moveCount = 1 + (int)rand_(2);
		const auto& dirMap = server.dirMap;
		const int dir = (int)rand_(4);
		REP(mi, moveCount) {
			int nextCell = dirMap[curCell][dir];
			if (nextCell < 0) {
				break;
			}
			int nextPc = state.GetPc(nextCell);
			if (nextPc >= 0) {
				int moveOutCell = MoveOutCell(nextCell);
				if (moveOutCell >= 0) {
					state.Move(nextCell, moveOutCell);
					history.push_back(pcell{ nextCell, moveOutCell });
				}
				else {
					break;
				}
			}
			if (!state.CanMove(curCell, nextCell)) {
				break;
			}
			state.Move(curCell, nextCell);
			history.push_back(pcell{ curCell, nextCell });
			curCell = nextCell;
		}
		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);
		}
		else {
			RREP(hi, history.size()) {
				pcell p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_MoveOut2(SimulatedAnnealing & sa, SAChecker & checker, State & state) {
		int pc = (int)rand_(state.pcCells.size());

		auto MoveOutCell = [&](int cell) -> int {
			const auto& around = server.aroundMap[cell];
			int offset = (int)rand_(around.size());
			REP(i, around.size()) {
				int ai = offset + i;
				if (ai >= around.size()) {
					ai -= around.size();
				}
				int nextCell = around[ai];
				int nextPc = state.GetPc(nextCell);
				if (nextPc < 0) {
					if (state.CanMove(cell, nextCell)) {
						return nextCell;
					}
				}
			}
			return -1;
		};

		CapArr<pcell, 6> history;
		int startMoveCount = state.moveCount;

		int curCell = state.GetCell(pc);

		int moveCount = 1 + (int)rand_(2);
		if (rand_(3) == 0) {
			REP(mi, moveCount) {
				const auto& around = server.aroundMap[curCell];
				int nextCell = around[(int)rand_(around.size())];
				int nextPc = state.GetPc(nextCell);
				if (nextPc >= 0) {
					if (!state.movablePcSet.IsContain(nextPc)) {
						break;
					}
					int moveOutCell = MoveOutCell(nextCell);
					if (moveOutCell >= 0) {
						state.Move(nextCell, moveOutCell);
						history.push_back(pcell{ nextCell, moveOutCell });
					}
					else {
						break;
					}
				}
				if (!state.CanMove(curCell, nextCell)) {
					break;
				}
				state.Move(curCell, nextCell);
				history.push_back(pcell{ curCell, nextCell });

				curCell = nextCell;
			}
		}
		else {
			const auto& dirMap = server.dirMap;
			const int dir = (int)rand_(4);
			REP(mi, moveCount) {
				int nextCell = dirMap[curCell][dir];
				if (nextCell < 0) {
					break;
				}

				int nextPc = state.GetPc(nextCell);
				if (nextPc >= 0) {
					int moveOutCell = MoveOutCell(nextCell);
					if (moveOutCell >= 0) {
						state.Move(nextCell, moveOutCell);
						history.push_back(pcell{ nextCell, moveOutCell });
					}
					else {
						break;
					}
				}
				if (!state.CanMove(curCell, nextCell)) {
					break;
				}
				state.Move(curCell, nextCell);
				history.push_back(pcell{ curCell, nextCell });

				curCell = nextCell;
			}
		}

		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);
		}
		else {
			RREP(hi, history.size()) {
				pcell p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_PrioritySwap(SimulatedAnnealing & sa, SAChecker & checker, State & state) {
		int a = (int)rand_(server.K);
		int b = (int)rand_(server.K - 1);
		if (b >= a) {
			++b;
		}

		swap(state.priority[a], state.priority[b]);

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			swap(state.priority[a], state.priority[b]);
		}
	}

	void Transition_PriorityShuffle(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		auto backup = state.priority;
		shuffle(ALL(state.priority), rand_);

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			state.priority = backup;
		}
	}

	void Transition_RandMoveRevert(SimulatedAnnealing & sa, SAChecker & checker, State & state) {
		constexpr int MaxCount = 5;
		CapArr<pcell, MaxCount> history;
		int startMoveCount = state.moveCount;

		int moveCount = 1 + (int)rand_(MaxCount);
		REP(i, moveCount) {
			if (rand_(3) == 0) {
				if (state.movablePcSet.empty()) {
					continue;
				}
				int pc = state.movablePcSet[(int)rand_(state.movablePcSet.size())];
				if (state.cellStacks[pc].size() >= PcMaxMove) {
					continue;
				}
				int cell = state.pcCells[pc];
				int next = -1;
				auto& around = server.aroundMap[cell];
				int offset = (int)rand_(around.size());
				REP(i, around.size()) {
					int ai = offset + i;
					if (ai >= around.size()) {
						ai -= around.size();
					}
					int n = around[ai];
					if (state.grid[n] >= 0) {
						continue;
					}
					if (state.pcStackGrid[n].size() >= CellMaxMove) {
						continue;
					}
					next = n;
					break;
				}
				if (next < 0) {
					continue;
				}
				state.Move(cell, next);
				history.push_back(pcell{ cell, next });
			}
			else {
				if (state.revertablePcSet.empty()) {
					continue;
				}

				int pc = state.revertablePcSet[(int)rand_(state.revertablePcSet.size())];
				const auto& cellStakcs = state.cellStacks[pc];
				VASSERT(cellStakcs.size() >= 2);
				int cell = cellStakcs[cellStakcs.size() - 1];
				int prev = cellStakcs[cellStakcs.size() - 2];
				state.Revert(pc);
				history.push_back(pcell{ cell, prev });
			}
		}

		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			RREP(hi, history.size()) {
				const auto& p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_Pull(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		VASSERT(!state.movablePcSet.empty());

		constexpr int MaxCount = 2;
		CapArr<pcell, MaxCount + 2> history;
		int startMoveCount = state.moveCount;

		int moveCount = 2;

		RandomChoice<4> rc;

		int pullToCell = -1;
		int lastPulledCell = -1;
		REP(i, moveCount) {
			if (pullToCell < 0) {
				int pc = state.movablePcSet[(int)rand_(state.movablePcSet.size())];
				if (state.cellStacks[pc].size() >= PcMaxMove) {
					continue;
				}

				int cell = state.pcCells[pc];
				int next = -1;
				auto& around = server.aroundMap[cell];
				rc.Init(around.size());
				REP(i, around.size()) {
					int n = around[rc.Choice(rand_)];
					if (state.grid[n] >= 0) {
						continue;
					}
					if (state.pcStackGrid[n].size() >= CellMaxMove) {
						continue;
					}
					next = n;
					break;
				}
				if (next < 0) {
					continue;
				}
				state.Move(cell, next);
				history.push_back(pcell{ cell, next });
				lastPulledCell = next;
				pullToCell = cell;
			}
			else {
				if (state.pcStackGrid[pullToCell].size() >= CellMaxMove) {
					break;
				}

				int target = -1;		
				auto& around = server.aroundMap[pullToCell];
				rc.Init(around.size());
				REP(i, around.size()) {
					int n = around[rc.Choice(rand_)];
					if (n == lastPulledCell) {
						continue;
					}
					int pc = state.pcGrid[n];
					if (pc < 0) {
						continue;
					}
					if (state.cellStacks[pc].size() >= PcMaxMove) {
						continue;
					}
					target = n;
					break;
				}
				if (target < 0) {
					break;
				}
				state.Move(target, pullToCell);
				history.push_back(pcell{ target, pullToCell });
				lastPulledCell = pullToCell;
				pullToCell = target;
			}
		}

		if (history.empty()) {
			return;
		}

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			RREP(hi, history.size()) {
				const auto& p = history[hi];
				state.Move(p.b, p.a);
			}
			VASSERT(startMoveCount == state.moveCount);
		}
	}

	void Transition_StartPc(SimulatedAnnealing& sa, SAChecker& checker, State& state) {
		int k = (int)rand_(server.K);
		int pc = k * 100 + (int)rand_(100);

		int backup = state.startPcs[k];
		state.startPcs[k] = pc;

		int newRawScore = 0;
		double newScore = Eval(state, sa.timeRate, newRawScore);

		if (checker(newScore)) {
			state.score = newScore;
			state.rawScore = newRawScore;

			CheckMaxScore(state, sa.timeRate);

		}
		else {
			state.startPcs[k] = backup;
		}
	}
};

struct Main {
	void Run(int argc, const char* argv[]) {
		server.InitInput();
		timer.StartMs(TIME_LIMIT);

		Solver* solver = new Solver();
        solver->Run();

		server.Finalize();
	}
};
