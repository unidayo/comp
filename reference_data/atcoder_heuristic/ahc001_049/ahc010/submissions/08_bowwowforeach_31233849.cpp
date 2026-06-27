
#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0

#define TIME_LIMIT (1950)

#define NOT_SUBMIT 0
#define VALIDATION 0

#define IO_FILE 0

#define OUTPUT_INFO 0
#define OUTPUT_FINAL_INFO 1
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


using TimePoint = chrono::steady_clock::time_point;

struct Timer {
private:
	TimePoint startTime_;
	TimePoint endTime_;

public:
	inline void Init() {
		startTime_ = chrono::steady_clock::now();
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
		return chrono::steady_clock::now() >= endTime_;
	}

	inline int ElapseTimeMs() const {
		return (int)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startTime_).count();
	}
	inline int ElapseTimeUs() const {
		return (int)chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - startTime_).count();
	}
	inline int LeftToUS(const TimePoint& limit) const {
		return (int)chrono::duration_cast<chrono::microseconds>(limit - chrono::steady_clock::now()).count();
	}

	inline double NowRate() const {
		return (chrono::steady_clock::now() - startTime_).count() / (double)(endTime_ - startTime_).count();
	}

	inline TimePoint Now() const {
		return chrono::steady_clock::now();
	}
	inline TimePoint EndTime() const {
		return endTime_;
	}

	TimePoint GetLimitTimePointUs(int limit) const {
		return startTime_ + chrono::microseconds(limit);
	}
};

Timer timer;

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

struct ValidateException {
	stringstream ss;
	ValidateException(const char* function, int line, const char* msg) {
		ss << "**** ABORT **** : " << function << " " << line << " " << msg;
	}
};
struct MemoryException {};


#define VALIDATE_ABORT()
#define VALIDATE_ASSERT(exp)


#define VABORT() VALIDATE_ABORT()
#define VASSERT(exp) VALIDATE_ASSERT(exp)



#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;


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
	operator pr<C, D>() const {
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
};
using pint = pr<int, int>;
using pdouble = pr<double, double>;

static_assert(is_pod<pint>::value, "not pod");

template <class A, class B, class C>
struct tr {
	union {
		A a;
		A first;
		A x;
	};
	union {
		B b;
		B second;
		B y;
	};
	union {
		C c;
		C third;
		C z;
	};

	bool operator == (tr const& r) const { return a == r.a && b == r.b && c == r.c; }
	bool operator != (tr const& r) const { return !((*this) == r); }
	bool operator < (tr const& r) const {
		if (a == r.a) {
			if (b == r.b) {
				return c < r.c;
			}
			return b < r.b;
		}
		return a < r.a;
	}

	tr operator + (tr v) const { return tr(x, y, z) += v; }
	tr operator - (tr v) const { return tr(x, y, z) -= v; }
	tr operator - () const { return tr{ -x, -y, -z }; }

	tr& operator += (tr v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	tr& operator -= (tr v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	friend istream& operator>>(istream& is, tr& p) {
		is >> p.a >> p.b >> p.c;
		return is;
	}
	friend ostream& operator<<(ostream& os, tr const& p) {
		os << p.a << " " << p.b << " " << p.c;
		return os;
	}
};

using tint = tr<int, int, int>;
static_assert(is_pod<tint>::value, "not pod");

template <class T>
struct arr : public vector<T> {
	static arr<arr<T>> D2(int y, int x, T value) {
		return arr<arr<T>>(y, arr<T>(x, value));
	}
	static arr<arr<arr<T>>> D3(int z, int y, int x, T value) {
		return arr<arr<arr<T>>>(z, arr<arr<T>>(y, arr<T>(x, value)));
	}
	static arr<arr<arr<arr<T>>>> D4(int w, int z, int y, int x, T value) {
		return arr<arr<arr<arr<T>>>>(w, arr<arr<arr<T>>>(z, arr<arr<T>>(y, arr<T>(x, value))));
	}

	static arr<T> Iota(int N, int value = 0) {
		auto r = arr<T>(N);
		r.iota(value);
		return r;
	}
	arr() {}
	arr(initializer_list<T> il) : vector<T>(il) {}
	explicit arr(int n) : vector<T>(n) {}
	arr(int n, T v) : vector<T>(n, v) {}
	arr(const arr& r) : vector<T>(r) {}
	arr(arr&& r) : vector<T>(move(r)) {}

	arr& operator = (const arr& r) {
		vector<T>::operator = (r);
		return *this;
	}
	arr& operator = (arr&& r) {
		vector<T>::operator = (move(r));
		return *this;
	}


	T& operator()(int i) { return (*this)[i]; }
	T const& operator()(int i) const { return (*this)[i]; }
	void init(int n, T v = T()) {
		this->assign(n, v);
	}
	int sz() const { return (int)this->size(); }
	void pb(T v) { this->push_back(v); }
	void sort() { std::sort(this->begin(), this->end()); }
	template <class F> void sort(F&& f) { std::sort(this->begin(), this->end(), f); }
	void rsort() { std::sort(this->begin(), this->end(), greater<T>()); }
	void reverse() { std::reverse(this->begin(), this->end()); }
	void unique_erase() { this->erase(std::unique(this->begin(), this->end()), this->end()); }
	bool next_permutation() { return std::next_permutation(this->begin(), this->end()); }
	int lower_bound(T const& v, function<bool(T, T)> p) { return std::lower_bound(this->begin(), this->end(), v, p) - this->begin(); }
	int lower_bound(T const& v) { return std::lower_bound(this->begin(), this->end(), v) - this->begin(); }
	int upper_bound(T const& v, function<bool(T, T)> p) { return std::upper_bound(this->begin(), this->end(), v, p) - this->begin(); }
	int upper_bound(T const& v) { return std::upper_bound(this->begin(), this->end(), v) - this->begin(); }
	void iota(T startValue = 0) { ::iota(this->begin(), this->end(), startValue); }
	void fill(T v) { ::fill(this->begin(), this->end(), v); }

	int find_sorted_nearest(T const& v) {
		int i = this->lower_bound(v);
		if (i >= sz()) {
			--i;
		}
		else if ((*this)[i] != v) {
			int p = i - 1;
			if (p >= 0) {
				int id = abs((*this)[i] - v);
				int pd = abs((*this)[p] - v);
				if (pd < id) {
					i = p;
				}
			}
		}
		return i;
	}

	int find_sorted(T const& v) {
		int i = this->lower_bound(v);
		if (i >= sz()) {
			return -1;
		}
		if ((*this)[i] != v) {
			return -1;
		}
		return i;
	}

	int find(T const& v) const {
		auto it = std::find(this->begin(), this->end(), v);
		if (it == this->end()) {
			return -1;
		}
		return (int)(it - this->begin());
	}
	bool is_contains(T const& v) const {
		auto it = std::find(this->begin(), this->end(), v);
		if (it == this->end()) {
			return false;
		}
		return true;
	}

	template <class P>
	void remove_if_erase(P&& predicate) { this->erase(std::remove_if(this->begin(), this->end(), predicate), this->end()); }

	void slide_remove(int i) {
		(*this)[i] = (*this)[sz() - 1];
		this->pop_back();
	}
	void remove_idx(int i) {
		this->erase(this->begin() + i);
	}
	void insert_idx(int i, const T& v) {
		this->insert(this->begin() + i, v);
	}

	int findGE(const T& v) {
		return lower_bound(v);
	}
	int findG(const T& v) {
		return upper_bound(v);
	}
	int findLE(const T& v) {
		return upper_bound(v) - 1;
	}
	int findL(const T& v) {
		return lower_bound(v) - 1;
	}


	friend ostream& operator<<(ostream& os, const arr<T>& p) {
		if (p.empty()) {
			return os;
		}
		os << p[0];
		FOR(i, 1, p.size()) {
			os << " " << p[i];
		}
		return os;
	}
};
using ints = arr<int>;

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
		ar[end++] = v;
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


template <class T, int CAPACITY>
class CapacityArray {
private:
	array<T, CAPACITY> array_ = {};
	int count_ = 0;

public:
	CapacityArray() {
	}

	CapacityArray(int count) {
		resize(count);
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

	inline bool IsContain(const T& value) const {
		for (const auto& v : *this) {
			if (v == value) {
				return true;
			}
		}
		return false;
	}


	inline void MemOverride(int dstIndex, int dstCount, const T* srcPtr, int srcCount) {
		memmove(&array_[dstIndex + srcCount], &array_[dstIndex + dstCount], sizeof(T) * (count_ - (dstIndex + dstCount)));
		memcpy(&array_[dstIndex], srcPtr, sizeof(T) * srcCount);
		count_ += srcCount - dstCount;
	}

};

template <class T, int CAPACITY>
using CapArr = CapacityArray<T, CAPACITY>;





template <int CAPACITY>
struct CapacitySet {
private:
	CapacityArray<int, CAPACITY> elemens;
	array<int, CAPACITY> indexTable;

public:
	CapacitySet() {
		fill(indexTable.begin(), indexTable.end(), -1);
	}

	void Clear() {
		elemens.Clear();
		fill(indexTable.begin(), indexTable.end(), -1);
	}

	inline void Add(int ai) {
		indexTable[ai] = elemens.GetCount();
		elemens.PushBack(ai);
	}

	inline void ForceAdd(int ai) {
		if (indexTable[ai] >= 0) {
			return;
		}
		indexTable[ai] = elemens.GetCount();
		elemens.PushBack(ai);
	}

	inline void Remove(int ai) {
		int removeIndex = indexTable[ai];
		int lastIndex = elemens.GetCount() - 1;

		if (removeIndex != lastIndex) {
			elemens[removeIndex] = elemens[lastIndex];
			indexTable[elemens[lastIndex]] = removeIndex;
		}
		elemens.PopBack();
		indexTable[ai] = -1;
	}

	inline void ForceRemove(int ai) {
		if (indexTable[ai] < 0) {
			return;
		}
		int removeIndex = indexTable[ai];
		int lastIndex = elemens.GetCount() - 1;

		if (removeIndex != lastIndex) {
			elemens[removeIndex] = elemens[lastIndex];
			indexTable[elemens[lastIndex]] = removeIndex;
		}
		elemens.PopBack();
		indexTable[ai] = -1;
	}

	inline bool IsContain(int ai) const {
		return indexTable[ai] >= 0;
	}

	inline int GetCount() const {
		return elemens.GetCount();
	}
	inline int size() const {
		return elemens.GetCount();
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
    bool IsChecked(int i) const {
        return checked_[i] == mark_;
    }
    void Check(int i) {
        checked_[i] = mark_;
    }
    void Reset(int i) {
        checked_[i] = mark_ - 1;
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

template <class T, int CAP>
struct CCA {
    T ar[CAP];
    int s;
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

    void Init(int width, int height, const pint (&aroundDirs)[AROUND_COUNT]) {
        width_ = width;
        int count = width * height;
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

#include <cstdint>

struct XorShift {
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;

	inline XorShift(uint32_t seed = 0) {
		x = 123456789;
		y = 362436069;
		z = 521288629;
		w = 88675123 + seed;
	}

	inline uint32_t operator()() {
		uint32_t t = x ^ (x << 11);
		x = y;
		y = z;
		z = w;
		return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
	}

	inline uint32_t operator()(uint32_t l, uint32_t r) {
		return ((*this)() % (r - l)) + l;
	}

	inline uint32_t operator()(uint32_t r) {
		return (*this)() % r;
	}
};

struct Xor64 {
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
};

struct Xor32 {
	using result_type = uint32_t;
	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return UINT32_MAX; }

	uint32_t x;
	inline Xor32(uint32_t seed = 0) {
		x = 2463534242 + seed;
	}

	inline uint32_t operator ()(void) {
		x = x ^ (x << 13);
		x = x ^ (x >> 17);
		return x = x ^ (x << 5);
	}

	inline uint32_t operator()(uint32_t l, uint32_t r) {
		return ((*this)() % (r - l)) + l;
	}

	inline uint32_t operator()(uint32_t r) {
		return (*this)() % r;
	}
};

struct Random {
	static inline Xor32& Instnce() {
		static Xor32 x;
		return x;
	}

	static inline uint32_t Get() {
		return Instnce()();
	}
	static inline uint32_t Get(uint32_t l, uint32_t r) {
		return Instnce()(l, r);
	}
	static inline uint32_t Get(uint32_t r) {
		return Instnce()(r);
	}

	static inline void Get2(uint32_t N, uint32_t& a, uint32_t& b) {
		VASSERT(N >= 2);
		a = Get(N);
		b = Get(N - 1);
		if (b >= a) {
			++b;
		}
	}

	static inline double GetDouble() {
		return Get() / (double)UINT32_MAX;
	}

	static inline double GetDouble(double l, double r) {
		return l + GetDouble() * (r - l);
	}
};


#define REGIST_PARAM(name, type, defaultValue) type name = defaultValue;




constexpr
struct {

} HYPER_PARAM;
auto& HP = HYPER_PARAM;

double ParamValue(double x, double a, double e) {
	return a * pow(x, e);
}

constexpr int W = 30;
constexpr int H = 30;
constexpr int WH = W * H;


AroundMapS<W, H, 4> AroundMap(Around4);
DirMapS<W, H, 4> DirMap(Around4);


template <int W, int H>
struct GridSystemS {
	inline constexpr int ToId(int x, int y) const {
		return x + W * y;
	}
	inline constexpr int ToId(const pint& p) const {
		return p.x + W * p.y;
	}
	inline constexpr pint ToPos(int id) const {
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
	inline bool IsIn(const pint& p) const {
		return !IsOut(p);
	}

	inline int RotateRight90(int id) const {
		pint p = ToPos(id);
		return ToId(W - 1 - p.y, p.x);
	}
};

template <class T>
using Grid = array<T, WH>;

GridSystemS<W, H> gs;

struct Action {
};

constexpr Dir TileDirs[8][4] = {
	{ Dir::N, Dir::N, Dir::U, Dir::L },
	{ Dir::N, Dir::L, Dir::D, Dir::N },
	{ Dir::D, Dir::R, Dir::N, Dir::N },
	{ Dir::U, Dir::N, Dir::N, Dir::R },

	{ Dir::D, Dir::R, Dir::U, Dir::L },
	{ Dir::U, Dir::L, Dir::D, Dir::R },

	{ Dir::L, Dir::N, Dir::R, Dir::N },
	{ Dir::N, Dir::U, Dir::N, Dir::D },
};

constexpr int TileRotate[8][4] = {
	{0, 1, 2, 3},
	{1, 2, 3, 0},
	{2, 3, 0, 1},
	{3, 0, 1, 2},

	{4, 5, 4, 5},
	{5, 4, 5, 4},
	{6, 7, 6, 7},
	{7, 6, 7, 6},
};

struct IOServer {
	Grid<u8> tiles;

	void Input() {
		auto& is = cin;
		char c;
		REP(cell, WH) {
			is >> c;
			tiles[cell] = c - '0';
		}
	}

	void Output(const Grid<u8>& ans) {
        {
            auto& os = cout;
			int cnt = 0;
			REP(cell, WH) {
				os << (int)ans[cell];
			}
        }

	}

};
IOServer server;




constexpr int PROGRESS_COUNT = 10;			


struct SimulatedAnnealing {
	double currentScore = 0;
	double maxScore = 0;
	double timeRate = 0;				
	double temp = 1;					
	double divTemp = 1;					
	int noMaxUpdateCount = 0;			
	int resetCount = 0;					


	double startTemp_ = 200;					
	double endTemp_ = 1;						
	int stepLoopCount = 1000;					


	template <class CALC_NEXT_STATE>
	void Run(Timer& timer, double currentScore_, CALC_NEXT_STATE&& calcNextState) {
		currentScore = currentScore_;
		maxScore = currentScore_;

		const auto startTime = timer.Now();
		const auto endTime = timer.EndTime();
		const double subTimeCountDiv = 1.0 / (double)(endTime - startTime).count();


		bool forceEnd = false;
		while (!timer.IsOver()) {
			timeRate = (timer.Now() - startTime).count() * subTimeCountDiv;
			temp = startTemp_ * std::pow(endTemp_ / startTemp_, timeRate);		
			divTemp = 1.0 / temp;

			for (int rp = 0; rp < stepLoopCount; ++rp) {
				if (calcNextState(*this)) {
					forceEnd = true;
					break;
				}
			}
			if (forceEnd) {
				break;
			}

		}

	}

	inline pair<bool, bool> operator()(double newScore) {
		++noMaxUpdateCount;

		if (newScore > currentScore) {
			currentScore = newScore;

			if (newScore > maxScore) {
				maxScore = newScore;
				noMaxUpdateCount = 0;
				return { true, true };
			}

			return { true, false };
		}

		else if (newScore == currentScore) {
			return { true, false };
		}

		else {
			if (exp((newScore - currentScore) * divTemp) * UINT32_MAX > Random::Get(UINT32_MAX)) {
				currentScore = newScore;
				return { true, false };
			}
			else {
				return { false, false };
			}
		}
	}
};



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

template <class... Updater>
struct SAExecuter {
    static constexpr int UpdaterCount = sizeof...(Updater);
    SimulatedAnnealing sim_;
    tuple<Updater...> updaters_;
    RandomTable ranTbl_;

    template <class TargetUpdater>
    void SetSelectRate(int count) {
        UpdaterLoop([&](auto&& updater, size_t i) {
            using UpdaterType = typename remove_reference<decltype(updater)>::type;
            if constexpr (is_same<UpdaterType, TargetUpdater>::value) {
                ranTbl_.push((int)i, count);
            }
        });
    }

    template <class... ARGS>
    void Run(double initialScore, ARGS&&... args) {
        UpdaterLoop([&](auto&& updater, size_t i) {
            updater.sim_ = &sim_;
        });

        sim_.Run(timer, initialScore, [&](SimulatedAnnealing& sa) {
            if constexpr (UpdaterCount == 1) {
                auto& updater = get<0>(updaters_);
                updater.UpdateBase(forward<ARGS>(args)...);
            }
            else {
                int pattern = ranTbl_(Random::Instnce());
                UpdaterLoop([&](auto&& updater, size_t i) {
                    if (i == pattern) {
                        updater.UpdateBase(forward<ARGS>(args)...);
                    }
                });
            }
            return false;
        });

    }

private:
    template <class Func>
    void UpdaterLoop(Func&& f) {
        UpdaterLoop2(forward<Func>(f), make_index_sequence<UpdaterCount>{});
    }
    template <class Func, size_t... Indics>
    void UpdaterLoop2(Func&& f, index_sequence<Indics...>) {
        using swallow = int[];
        (void)swallow {
            (UpdaterLoop3<Func, Indics>(forward<Func>(f)), 0)...
        };
    }
    template <class Func, size_t Index>
    void UpdaterLoop3(Func&& f) {
        f(get<Index>(updaters_), Index);
    }
};

template <class SUB>
struct SAUpdaterBase {
    SimulatedAnnealing* sim_ = nullptr;


    template <class... ARGS>
    void UpdateBase(ARGS&&... args) {
        static_cast<SUB&>(*this).Update(forward<ARGS>(args)...);
    }

    pair<bool, bool> Check(double newScore) {

        auto r = (*sim_)(newScore);
        if (r.first) {
            if (r.second) {
            }
        }
        else {
        }

        return r;
    }

};


using Line = CapArr<int, WH * 2>;

Dir CalcDir(int from, int to) {
	VASSERT(from != to);
	VASSERT(1 == abs(from - to) || W == abs(from - to));

	if (from + 1 == to) {
		return Dir::R;
	}
	if (from - 1 == to) {
		return Dir::L;
	}
	if (from < to) {
		return Dir::D;
	}
	return Dir::U;
}

constexpr int LIMIT_LOOP = 1000;

struct Solver {
	struct State {
		double score;
		array<Grid<s8>, 2> uses;
		array<Line, 2> lines;
	};

	bool randomMode = false;
	bool useDouble = false;

	void Run() {
		arr<Grid<s8>> patterns(2);
		{
			Grid<s8>& pattern = patterns[0];
			REP(y, H) {
				REP(x, W) {
					if (x < W / 2) {
						pattern[gs.ToId(x, y)] = 2;
					}
				}
			}
		}
		{
			Grid<s8>& pattern = patterns[1];
			REP(y, H) {
				REP(x, W) {
					if (y < H / 2) {
						pattern[gs.ToId(x, y)] = 2;
					}
				}
			}
		}

		{
			timer.StartMs(200);

			Line maxLine = {};

			randomMode = true;
			useDouble = true;

			[&]() {
				REP(loop, 999999999) {
					REP(cell, WH) {
						REP(d, 4) {
							if (timer.IsOver()) {
								return;
							}

							Line line = {};
							Grid<s8> use = patterns[Random::Get(patterns.size())];
							if (FindResume(cell, (Dir)d, line, use)) {
								if (line.size() > maxLine.size()) {
									maxLine = line;
								}
							}
						}
					}
				}
			}();

			Grid<s8> dummySecond = {};
			for (int cell : maxLine) {
				dummySecond[cell] = 2;
			}

			timer.StartMs(400);

			Line maxLine2 = {};

			randomMode = true;
			useDouble = true;

			[&]() {
				REP(loop, 999999999) {
					RREP(cell, WH) {
						RREP(d, 4) {
							if (timer.IsOver()) {
								return;
							}
							Line line = {};
							Grid<s8> use = dummySecond;
							if (FindResume(cell, (Dir)d, line, use)) {
								if (line.size() > maxLine2.size()) {
									maxLine2 = line;
								}
							}
						}
					}
				}
			}();


			Grid<s8> use = {};
			Grid<s8> use2 = {};
			for (int cell : maxLine) {
				use[cell] = 2;
			}
			for (int cell : maxLine2) {
				use2[cell] = 2;
			}

			timer.StartMs(1970);
			while (!timer.IsOver()) {
				if (maxLine.size() < maxLine2.size()) {
					if (UpdateLine(maxLine, use2)) {
						use = {};
						for (int cell : maxLine) {
							use[cell] = 2;
						}
					}
				}
				else {
					if (UpdateLine(maxLine2, use)) {
						use2 = {};
						for (int cell : maxLine2) {
							use2[cell] = 2;
						}
					}
				}
			}

			Grid<u8> rot = {};
			UpdateRot(rot, maxLine);
			UpdateRot(rot, maxLine2);

			cerr << maxLine.size() * maxLine2.size() << endl;
			server.Output(rot);

			return;
		}

		State curState = {};
		InitState(curState);

		State maxState = curState;

		SAExecuter<
			Updater1
		> executer;

		executer.SetSelectRate<Updater1>(100);

		executer.sim_.startTemp_ = 200.0;
		executer.sim_.endTemp_ = 1;
		executer.sim_.stepLoopCount = 200;

		executer.Run(curState.score, curState, maxState);

		Grid<u8> rot = {};
		server.Output(rot);
	}

	bool UpdateLine(Line& srcLine, const Grid<s8>& use) {
		int maxLen = min(srcLine.size(), 100);
		int minLen = 10;
		int len = Random::Get(maxLen - minLen) + minLen;
		int start = Random::Get(srcLine.size() - len);
		int end = start + len;
		if (start == 0) {
			start++;
		}
		if (end == srcLine.size()) {
			--end;
		}

		Grid<s8> use2 = use;
		FOR(i, 0, start) {
			use2[srcLine[i]] = 2;
		}
		FOR(i, end, srcLine.size()) {
			use2[srcLine[i]] = 2;
		}

		Dir startDir = CalcDir(srcLine[start - 1], srcLine[start]);
		Dir endDir = CalcDir(srcLine[end - 1], srcLine[end]);
		Line line;
		int loop = 0;
		if (FindResume2(srcLine[start], startDir, srcLine[end], endDir, line, use2)) {
			int size = end - start;
			if (line.size() > size) {
				Line resultLine = {};
				FOR(i, 0, start) {
					resultLine.push_back(srcLine[i]);
				}
				for (int cell : line) {
					resultLine.push_back(cell);
				}
				FOR(i, end, srcLine.size()) {
					resultLine.push_back(srcLine[i]);
				}
				srcLine = resultLine;
				return true;
			}
		}
		return false;
	}

	bool FindResume2(int cell, Dir dir, int endCell, Dir endDir, Line& line, Grid<s8>& use) {
		line.clear();
		int loop = 0;
		Grid<int> useDir = {};
		return DFS(cell, dir, line, use, useDir, endCell, endDir, loop);
	}

	void UpdateRot(Grid<u8>& rot, const Line& line) {
		const auto& tiles = server.tiles;

		int ls = line.size();
		REP(li, line.size()) {
			Dir from = CalcDir((line[(li + ls - 1) % ls]), line[li]);
			Dir to = CalcDir(line[li], line[(li + 1) % ls]);

			int cell = line[li];
			int srcTile = tiles[cell];
			REP(r, 4) {
				int t = TileRotate[srcTile][r];
				if (TileDirs[t][(int)from] == to) {
					rot[cell] = r;
					break;
				}
			}
		}
	}

	void Output(const Line& line) {
		const auto& tiles = server.tiles;
		Grid<u8> rot = {};

		int ls = line.size();
		REP(li, line.size()) {
			Dir from = CalcDir((line[(li + ls - 1) % ls]), line[li]);
			Dir to = CalcDir(line[li], line[(li + 1) % ls]);

			int cell = line[li];
			int srcTile = tiles[cell];
			REP(r, 4) {
				int t = TileRotate[srcTile][r];
				if (TileDirs[t][(int)from] == to) {
					rot[cell] = r;
					break;
				}
			}
		}

		server.Output(rot);
	}

	void InitState(State& state) {
		const auto& tiles = server.tiles;

	}

	struct Updater1 : public SAUpdaterBase<Updater1> {
		void Update(State& curState, State& maxState) {
			double newScore = curState.score;

			auto r = Check(newScore);
			if (r.first) {
				curState.score = newScore;

				if (r.second) {
					maxState = curState;
				}
			}
			else {
			}
		}
	};


	bool Find(int cell, Dir dir, Line& line, Grid<s8>& use) {
		line.clear();
		use = {};
		int loop = 0;
		Grid<int> useDir = {};
		return DFS(cell, dir, line, use, useDir, cell, dir, loop);
	}

	bool FindResume(int cell, Dir dir, Line& line, Grid<s8>& use) {
		line.clear();
		int loop = 0;
		Grid<int> useDir = {};
		return DFS(cell, dir, line, use, useDir, cell, dir, loop);
	}

	bool DFS(int cell, Dir dir, Line& line, Grid<s8>& use, Grid<int>& useDir, int startCell, Dir endDir, int& loop) {
		if (use[cell] > 0 && cell == startCell && dir == endDir) {
			return true;
		}

		++loop;
		if (loop >= LIMIT_LOOP) {
			return false;
		}

		int tile = server.tiles[cell];
		if (tile >= 4 && tile <= 5) {
			if (use[cell] == 0) {
			}
			else if (use[cell] == 1) {
				if (!useDouble) {
					return false;
				}
			}
			else {
				return false;
			}
		}
		else {
			if (use[cell]) {
				return false;
			}
		}

		line.push_back(cell);
		++use[cell];

		auto Next = [&](Dir nextDir) {
			int nextCell = DirMap[cell][(int)nextDir];
			if (nextCell >= 0) {
				int bit = (1 << int(nextDir)) | (1 << int(Back(dir)));

				if (use[cell] == 2) {
					if (useDir[cell] & bit) {
						return false;
					}
				}

				int backup = useDir[cell];
				useDir[cell] |= bit;
				if (DFS(nextCell, nextDir, line, use, useDir, startCell, endDir, loop)) {
					return true;
				}
				useDir[cell] = backup;
			}
			return false;
		};

		if (tile < 4) {
			if (randomMode && Random::Get(2)) {
				if (Next(RotateRight(dir))) {
					return true;
				}
				if (Next(RotateLeft(dir))) {
					return true;
				}
			}
			else {
				if (Next(RotateLeft(dir))) {
					return true;
				}
				if (Next(RotateRight(dir))) {
					return true;
				}
			}
		}
		else if (tile < 6) {
			if (randomMode && Random::Get(2)) {
				if (Next(RotateRight(dir))) {
					return true;
				}
				if (Next(RotateLeft(dir))) {
					return true;
				}
			}
			else {
				if (Next(RotateLeft(dir))) {
					return true;
				}
				if (Next(RotateRight(dir))) {
					return true;
				}
			}
		}
		else {
			if (Next(dir)) {
				return true;
			}
		}

		line.pop_back();
		--use[cell];

		return false;
	}
};

struct Main {
	void Run(int argc, const char* argv[]) {
		try {
			timer.StartMs(TIME_LIMIT);
			server.Input();

			Solver* solver = new Solver;
			solver->Run();
		}
		catch (ValidateException& e) {
			(void)e;
		}

		cerr << "time " << timer.ElapseTimeMs() << endl;
	}
};
