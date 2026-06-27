#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

#ifdef ONLY_MY_ENVIR

#include <chrono>
class Timer {
	const std::chrono::time_point<std::chrono::system_clock> start;
	int64_t limit_microsec = 0;

public:
	Timer();
	void set(int64_t time_microsec);
	bool elapsed() const;
	int64_t getMicrosec() const;
};

#else

class Timer {
	static const uint64_t ClocksPerMicrosec;
	const uint64_t start_clocks;
	uint64_t limit_clocks;

	uint64_t getClocks() const;
public:
	Timer();
	void set(int64_t time_microsec);
	bool elapsed() const;
	int64_t getMicrosec() const;
};

#endif

#endif

#ifdef ONLY_MY_ENVIR
#include "Timer.h"
#endif

#ifdef ONLY_MY_ENVIR

using namespace std::chrono;

Timer::Timer()
	: start(system_clock::now()) {
}

void Timer::set(int64_t time_microsec) {
	limit_microsec += time_microsec;
}

bool Timer::elapsed() const {
	return getMicrosec() >= limit_microsec;
}

int64_t Timer::getMicrosec() const {
	return duration_cast<microseconds>(system_clock::now() - start).count();
}

#else

const uint64_t Timer::ClocksPerMicrosec = 2987;

uint64_t Timer::getClocks() const {
	unsigned int lo, hi;
	__asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32) | lo;
}

Timer::Timer() 
	: start_clocks(getClocks()), limit_clocks(start_clocks) {
}

void Timer::set(int64_t time_microsec) {
	limit_clocks += time_microsec * ClocksPerMicrosec;
}

bool Timer::elapsed() const {
	return getClocks() >= limit_clocks;
}

int64_t Timer::getMicrosec() const {
	return (getClocks() - start_clocks) / ClocksPerMicrosec;
}

#endif

#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>
#include <utility>

uint32_t rnd();
uint32_t rnd(uint32_t max);

template <class T>
void shuffle(T& arr) {
	for (int i = arr.size(); i > 1; --i) {
		std::swap(arr[i - 1], arr[rnd(i)]);
	}
}

#endif
#ifdef ONLY_MY_ENVIR
#include "Random.h"
#endif

#include <vector>

uint32_t rnd() {
	static uint32_t val = 3141592653ul;
	val ^= val << 13;
	val ^= val >> 17;
	val ^= val << 15;
	return val;
}

uint32_t rnd(uint32_t max) {
	return rnd() % max;
}

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <vector>
#include <algorithm>

struct InputData {
	int size;
	std::vector<int> x;
	std::vector<int> y;
	std::vector<int> r;
};

struct OutputData {
	std::vector<int> a;
	std::vector<int> b;
	std::vector<int> c;
	std::vector<int> d;
};


struct Region {
	int a;
	int b;
	int c;
	int d;

	Region() = default;
	Region(int a, int b) :
		a(a), b(b), c(a + 1), d(b + 1) {
	}
	Region(int a, int b, int c, int d) :
		a(a), b(b), c(c), d(d) {
	}

	int area() const {
		return (c - a) * (d - b);
	}

	bool contains(int x, int y) const {
		return x >= a && x < c&& y >= b && y < d;
	}

	bool operator==(const Region& r) {
		return a == r.a && b == r.b && c == r.c && d == r.d;
	}
};

inline Region hull(const Region& s, const Region& t) {
	return Region(
		std::min(s.a, t.a),
		std::min(s.b, t.b),
		std::max(s.c, t.c),
		std::max(s.d, t.d)
	);
}

inline bool intersects(const Region& s, const Region& t) {
	return s.a < t.c && s.c > t.a && s.b < t.d && s.d > t.b;
}


#endif
#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <iostream>
#include <cstdint>

struct InputData;
struct OutputData;
InputData generate(uint32_t seed, int size = -1);
InputData readInput(std::istream& ist);
void writeInput(std::ostream& ost, const InputData& in);
void writeOutput(std::ostream& ost, const OutputData& out);
int64_t judge(const InputData& in, const OutputData& out);
void generateSamples(int count);

#endif


#ifdef ONLY_MY_ENVIR
#include "Generator.h"
#include "Global.h"
#endif

#include <cassert>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <random>
#include <string>
#include <unordered_set>

InputData generate(uint32_t seed, int size) {
	std::mt19937 mt(seed);
	std::uniform_int_distribution<int> dint(0, 9999);
	std::uniform_real_distribution<double> dreal(0.0, 1.0);

	if (size == -1) {
		size = std::round(50 * std::pow(4.0, dreal(mt)));
		if (size < 50) size = 50;
		if (size > 200) size = 200;
	}

	InputData ret;
	ret.size = size;
	ret.x.resize(size);
	ret.y.resize(size);
	ret.r.resize(size);

	// x, y の決定
	// 点 (x, y) を 整数 (x << 16) | y に対応させている
	{
		std::unordered_set<int> used;
		for (int i = 0; i < size; ++i) {
			int x;
			int y;
			int p;
			do {
				x = dint(mt);
				y = dint(mt);
				p = (x << 16) | y;
			} while (used.count(p));
			ret.x[i] = x;
			ret.y[i] = y;
			used.insert(p);
		}
	}

	// r の決定
	{
		std::unordered_set<int> used;
		std::vector<int> q(size + 1);
		for (int i = 1; i < size; ++i) {
			int x;
			int y;
			int p;
			do {
				x = dint(mt);
				y = dint(mt);
				p = x * 10000 + y;
			} while (used.count(p));
			q[i] = p;
			used.insert(p);
		}

		q[0] = 0;
		q[size] = 100000000;
		std::sort(q.begin(), q.end());

		for (int i = 0; i < size; ++i) {
			ret.r[i] = q[i + 1] - q[i];
		}
	}
	return ret;
}

InputData readInput(std::istream& ist) {
	InputData ret;
	ist >> ret.size;
	ret.x.resize(ret.size);
	ret.y.resize(ret.size);
	ret.r.resize(ret.size);
	
	for (int i = 0; i < ret.size; ++i) {
		ist >> ret.x[i] >> ret.y[i] >> ret.r[i];
	}
	return ret;
}

void writeInput(std::ostream& ost, const InputData& in) {
	ost << in.size << std::endl;
	for (int i = 0; i < in.size; ++i) {
		ost << in.x[i] << ' ' << in.y[i] << ' ' << in.r[i] << std::endl;
	}
}

void writeOutput(std::ostream& ost, const OutputData& out) {
	int size = out.a.size();
	for (int i = 0; i < size; ++i) {
		ost <<
			out.a[i] << ' ' <<
			out.b[i] << ' ' <<
			out.c[i] << ' ' <<
			out.d[i] << ' ' <<
			std::endl;
	}
}

int64_t judge(const InputData& in, const OutputData& out) {
	const int size = in.size;
	const auto& x = in.x;
	const auto& y = in.y;
	const auto& r = in.r;	
	const auto& a = out.a;
	const auto& b = out.b;
	const auto& c = out.c;
	const auto& d = out.d;

	assert(a.size() == size);
	assert(b.size() == size);
	assert(c.size() == size);
	assert(d.size() == size);

	// 領域の座標が範囲内か検証
	const int max = 10000;
	for (int i = 0; i < size; ++i) {
		if (a[i] < 0 || a[i] > max) return -1;
		if (b[i] < 0 || b[i] > max) return -1;
		if (c[i] < 0 || c[i] > max) return -1;
		if (d[i] < 0 || d[i] > max) return -1;
	}
	
	// 領域が正の長さを持つか検証
	for (int i = 0; i < size; ++i) {
		if (a[i] >= c[i]) return -2;
		if (b[i] >= d[i]) return -2;
	}

	// 領域が重なっていないか検証
	for (int i = 0; i < size; ++i) {
		for (int j = i + 1; j < size; ++j) {
			if (c[i] > a[j] && c[j] > a[i] && d[i] > b[j] && d[j] > b[i]) return -3;
		}
	}

	// 得点計算
	double sum = 0;
	for (int i = 0; i < size; ++i) {
		if (x[i] < a[i] || x[i] >= c[i]) continue;
		if (y[i] < b[i] || y[i] >= d[i]) continue;
		int s = (c[i] - a[i]) * (d[i] - b[i]);
		int t = r[i];
		if (s > t) std::swap(s, t);
		double ratio = 1 - double(s) / t;
		sum += 1 - ratio * ratio;
	}
	double score = 1e9 * sum / size;
	return std::round(score);
}

void generateSamples(int count) {
	std::mt19937 mt;
	for (int i = 0; i < count; ++i) {
		int size = std::round(50 * std::pow(4.0, (double(i) + 0.5) / count));
		if (size < 50) size = 50;
		if (size > 200) size = 200;
		auto in = generate(mt(), size);
		std::ofstream ofs("samples/" + std::to_string(i) + ".txt");
		writeInput(ofs, in);
	}
}


#ifndef __MAIN_H__
#define __MAIN_H__

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>
#include <functional>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <iomanip>
#include <sstream>
#include <bitset>
#include <limits>
#include <numeric>
#include <valarray>
#include <fstream>
#include <array>
#include <random>
#include <unordered_set>
#include <unordered_map>

using namespace std;
using uint = uint32_t;
using LL = int64_t;
using ULL = uint64_t;
using PP = pair<LL, LL>;
template <typename T> using PriorityQ = priority_queue<T, vector<T>, greater<T> >;
#define REP(i, a, n) for(LL i = (a), i##_max_ = (n); i < i##_max_; ++i)
#define REM(i, a, n) for(LL i = (LL)(n) - 1, i##_min_ = (a); i >= i##_min_; --i)
#define FLOAT fixed << setprecision(16)
#define SPEEDUP { cin.tie(NULL); ios::sync_with_stdio(false); }

#ifdef __GNUC__
typedef __int128 LLL;
istream& operator>>(istream& ist, __int128& val) { LL tmp; ist >> tmp; val = tmp; return ist; }
ostream& operator<<(ostream& ost, __int128 val) { LL tmp = val; ost << tmp; return ost; }
#endif

#ifdef ONLY_MY_ENVIR
extern ifstream input_stream;
extern ofstream output_stream;
#define cin input_stream
#define cout output_stream
#endif

#endif
#ifndef __ACCUMULATOR_H__
#define __ACCUMULATOR_H__

/* updated: 2020-08-02 */

#include <cassert>
#include <vector>
#include <algorithm>

template <class T>
class Accumulator2D {
	int height_m = 0;
	int width_m = 0;
	std::vector<std::vector<T>> sum_m;

	bool is_calculated = false;

public:
	Accumulator2D();
	explicit Accumulator2D(int height, int width);
	explicit Accumulator2D(const std::vector<std::vector<T>>& arr);
	void initialize(int height, int width);
	void initialize(const std::vector<std::vector<T>>& arr);
	void add(int i, int j, const T& increment);
	void execute();
	T get(int i, int j) const;
	T get(int ai, int aj, int bi, int bj) const;
};

template<class T>
Accumulator2D<T>::Accumulator2D() 
	: Accumulator2D(0, 0) {
}

template<class T>
Accumulator2D<T>::Accumulator2D(int height, int width) {
	initialize(height, width);
}

template <class T>
Accumulator2D<T>::Accumulator2D(const std::vector<std::vector<T>>& arr)	{
	initialize(arr);
}

template<class T>
void Accumulator2D<T>::initialize(int height, int width) {
	height_m = height;
	width_m = width;
	sum_m.assign(height_m + 1, std::vector<T>(width_m + 1, T()));

	is_calculated = false;
}

template<class T>
void Accumulator2D<T>::initialize(const std::vector<std::vector<T>>& arr) {
	int height = arr.size();
	int width = height == 0 ? 0 : arr.front().size();
	initialize(height, width);

	/* 累積和をとるとき in-place にできるように1つ添字をずらしておく */
	for (int i = 0; i < height_m; ++i) {
		for (int j = 0; j < width_m; ++j) {
			sum_m[i + 1][j + 1] = arr[i][j];
		}
	}

	is_calculated = false;
}

/* 要素 (i, j) の値を increment だけ増加させる (累積和の計算前) O(1) */
template<class T>
void Accumulator2D<T>::add(int i, int j, const T& increment) {
	assert(i >= 0 && i < height_m);
	assert(j >= 0 && j < width_m);
	sum_m[i + 1][j + 1] += increment;
}

/* 累積和を計算する O(H * W) */
template<class T>
void Accumulator2D<T>::execute() {
	for (int i = 0; i < height_m; ++i) {
		for (int j = 0; j < width_m; ++j) {
			sum_m[i + 1][j + 1] += sum_m[i][j + 1] + sum_m[i + 1][j] - sum_m[i][j];
		}
	}

	is_calculated = true;
}

/* 領域 [0, i) * [0, j) 内の要素の和を得る O(1) */
template <class T>
T Accumulator2D<T>::get(int i, int j) const {
	assert(is_calculated);

	int new_i = std::max(0, std::min(height_m, i));
	int new_j = std::max(0, std::min(width_m, j));
	return sum_m[new_i][new_j];
}

/* 領域 [ai, bi) * [aj, bj) 内の要素の和を得る O(1) */
template <class T>
T Accumulator2D<T>::get(int ai, int aj, int bi, int bj) const {
	assert(is_calculated);

	return get(ai, aj) - get(ai, bj) - get(bi, aj) + get(bi, bj);
}

#endif

#ifndef __RANGE_H__
#define __RANGE_H__

#ifdef ONLY_MY_ENVIR
#include "Accumulator.h"
#endif

#include <vector>
#include <utility>
class CompAccumulator {
	Accumulator2D<int> acc;

	int h;
	int w;
	std::vector<std::pair<int, int>> pos;

	// 座標圧縮用 (row[i] : 座標 i が圧縮された先の座標)
	std::vector<int> row;
	std::vector<int> col;

public:
	CompAccumulator() = default;
	CompAccumulator(CompAccumulator&&) noexcept = default;
	CompAccumulator& operator=(CompAccumulator&&) noexcept = default;

	CompAccumulator(int h, int w) 
		: h(h), w(w), row(h + 1), col(w + 1) {
	}

	void add(int i, int j) {
		pos.push_back(std::make_pair(i, j));
		row[i + 1] = 1;
		col[j + 1] = 1;
	}

	void execute() {
		for (int i = 0; i < h; ++i) row[i + 1] += row[i];
		for (int j = 0; j < w; ++j) col[j + 1] += col[j];
		
		int hcomp = row.back();
		int wcomp = col.back();
		acc.initialize(hcomp, wcomp);

		for (auto&& p : pos) {
			int i = p.first;
			int j = p.second;
			int icomp = row[i];
			int jcomp = col[j];
			acc.add(icomp, jcomp, 1);
		}

		acc.execute();
	}

	int get(int ai, int aj, int bi, int bj) const {
		return acc.get(row[ai], col[aj], row[bi], col[bj]);
	}
};

#endif



#ifndef __FINAL_H__
#define __FINAL_H__


#ifdef ONLY_MY_ENVIR
#include "Main.h"
#include "Global.h"
#include "Random.h"
#include "Timer.h"
#include "Range.h"
#endif

constexpr int fsize = 10000;

void init(const InputData& in);

template <class T>
typename T::value_type getSample(const T& arr) {
	return arr[rnd(size(arr))];
}

bool isAccepted(double del, double tmp);
vector<int> getIntersecting(const Region& ng);

bool isValidPos(int i, const Region& ng);
bool canMoveTo(int i, const Region& ng);
bool canForciblyMoveTo(int i, const Region& ng);
bool canForciblyMoveToDouble(int i1, int i2, const Region& ng);
void setRegion(int i, const Region& ng);
void loadRegions();
bool execShrink();
bool execExpandSimple();
bool execExpand();

int calcUpMax(int i, const Region& ng);
int calcLeftMax(int i, const Region& ng);
int calcDownMax(int i, const Region& ng);
int calcRightMax(int i, const Region& ng);
bool execFix();
int calcUpMaxF(int i, const Region& g);
int calcLeftMaxF(int i, const Region& g);
int calcDownMaxF(int i, const Region& g);
int calcRightMaxF(int i, const Region& g);
bool execFlip();
bool execFlipF();

bool execMove();
Region calcSafePlace(int j, const Region& ng);

bool execForciblyMoveTo(int i, const Region& ng);
void maximize(int i, Region& ng);
bool execForciblyMoveAndExpand(int i, const Region& ng);
bool execForciblyMoveToDouble(int i1, int i2, const Region& ng, const Region& ng1, const Region& ng2, bool needs_check);
bool execForciblyMoveAndExpandDouble(int i1, int i2, const Region& ng, const Region& ng1, const Region& ng2, bool needs_check);
bool execUnyo();
bool execUnyo2();
bool execExchange();

pair<int, int> calcHeightMaxEx(int i, const Region& ng);
pair<int, int> calcWidthMaxEx(int i, const Region& ng);
pair<int, int> calcHeightMaxAscend(int i, int nx, int ny, vector<int>& list);
pair<int, int> calcHeightMaxDescend(int i, int nx, int ny, vector<int>& list);
pair<int, int> calcWidthMaxAscend(int i, int nx, int ny, vector<int>& list);
pair<int, int> calcWidthMaxDescend(int i, int nx, int ny, vector<int>& list);
bool execDestroy();
bool execNobashi();

double exec();
void undo();
void step(double tmp);
double updateScore(int i);
double calc(int i);
OutputData get();

template <class Func>
int findBoundary(Func func);

template <class Func>
int findBoundary(Func func) {
	int ret = 0;
	int del = 1;
	while (func(del)) {
		del *= 2;
	}

	del /= 2;
	while (del > 0) {
		if (func(ret + del)) {
			ret += del;
		}
		del /= 2;
	}
	return ret;
}




#endif

#ifdef ONLY_MY_ENVIR
#include "Final.h"
#endif

int n;
vector<int> x;
vector<int> y;
vector<int> r;
vector<int> mis;
vector<Region> mg;
vector<Region> g;

//double psc = 0;
//double sc = 0;
vector<double> pscs;
vector<double> scs;
CompAccumulator acc;

vector<int> is_mb;
vector<int> buffer_mb;
vector<int> list_mb;

void init(const InputData& in) {
	n = in.size;
	x = in.x;
	y = in.y;
	r = in.r;
	g.resize(n + 1);
	pscs.resize(n);
	scs.resize(n);
	acc = CompAccumulator(fsize, fsize);

	for (int i = 0; i < n; ++i) {
		g[i] = Region(x[i], y[i]);
		scs[i] = calc(i);
		//sc += scs[i];
		acc.add(x[i], y[i]);
	}
	g[n] = Region(fsize, fsize, 0, 0);		// execUnyo2 で利用している (変更しない)
	mg = g;
	pscs = scs;
	acc.execute();
}

bool isAccepted(double del, double tmp) {
	return del >= 0 || (del >= -8 * tmp && rnd() < std::exp(del / tmp) * (1ll << 32));
}

/* 要求されている座標を含むかどうかのみ判定する (他との重なりは考慮しない) */
bool isValidPos(int i, const Region& ng) {
	if (ng.a < 0 || ng.b < 0 || ng.c > fsize || ng.d > fsize) return false;
	if (x[i] < ng.a || x[i] >= ng.c) return false;
	if (y[i] < ng.b || y[i] >= ng.d) return false;
	return true;
}

/* 面積大きいほうから比較したりすると意味あるかな */
bool canMoveTo(int i, const Region& ng) {
	if (!canForciblyMoveTo(i, ng)) return false;

	auto intersects_mb = getIntersecting(ng);
	intersects_mb[i] = false;
	for (int j = 0; j < n; ++j) {
		if (intersects_mb[j]) return false;
	}
	return true;
}

bool canForciblyMoveTo(int i, const Region& ng) {
	if (!isValidPos(i, ng)) return false;
	return acc.get(ng.a, ng.b, ng.c, ng.d) == 1;
}

bool canForciblyMoveToDouble(int i1, int i2, const Region& ng) {
	if (!isValidPos(i1, ng)) return false;
	if (!isValidPos(i2, ng)) return false;
	return acc.get(ng.a, ng.b, ng.c, ng.d) == 2;
}

void setRegion(int i, const Region& ng) {
	mis.push_back(i);
	mg[i] = g[i];
	g[i] = ng;
}

void loadRegions() {
	for (int i : mis) {
		g[i] = mg[i];
	}
}

bool execShrink() {
	const int range = 5;
	int i = rnd(n);
	int k = rnd(4);
	int ch = rnd(range) + 1;

	Region ng(g[i]);
	switch (k) {
	case 0: ng.a += ch; break;
	case 1: ng.b += ch; break;
	case 2: ng.c -= ch; break;
	case 3: ng.d -= ch; break;
	}

	if (!isValidPos(i, ng)) return false;
	setRegion(i, ng);
	return true;
}

bool execExpandSimple() {
	const int range = 5;

	int i = rnd(n);
	int k = rnd(4);
	int ch = rnd(range) + 1;

	Region ng(g[i]);
	switch (k) {
	case 0: ng.a -= ch; break;
	case 1: ng.b -= ch; break;
	case 2: ng.c += ch; break;
	case 3: ng.d += ch; break;
	}

	if (!canMoveTo(i, ng)) return false;
	setRegion(i, ng);
	return true;
}

bool execExpand() {
	const int range = 20;

	int i = rnd(n);
	int k = rnd(4);
	int ch = rnd(range) + 1;

	Region ng(g[i]);
	switch (k) {
	case 0: ng.a -= ch; break;
	case 1: ng.b -= ch; break;
	case 2: ng.c += ch; break;
	case 3: ng.d += ch; break;
	}

	return execForciblyMoveTo(i, ng);
}

/* 上方向にどれだけ伸ばせるか */
int calcUpMax(int i, const Region& ng) {
	int mx = 0;
	for (int j = 0; j < n; ++j) {
		if (j == i) continue;
		if (g[j].d <= ng.b || g[j].b >= ng.d || g[j].a >= ng.c) continue;
		mx = max(mx, g[j].c);
	}
	return max(-1, ng.a - mx);
}

int calcLeftMax(int i, const Region& ng) {
	int mx = 0;
	for (int j = 0; j < n; ++j) {
		if (j == i) continue;
		if (g[j].c <= ng.a || g[j].a >= ng.c || g[j].b >= ng.d) continue;
		mx = max(mx, g[j].d);
	}
	return max(-1, ng.b - mx);
}

int calcDownMax(int i, const Region& ng) {
	int mn = fsize;
	for (int j = 0; j < n; ++j) {
		if (j == i) continue;
		if (g[j].d <= ng.b || g[j].b >= ng.d || g[j].c <= ng.a) continue;
		mn = min(mn, g[j].a);
	}
	return max(-1, mn - ng.c);
}

int calcRightMax(int i, const Region& ng) {
	int mn = fsize;
	for (int j = 0; j < n; ++j) {
		if (j == i) continue;
		if (g[j].c <= ng.a || g[j].a >= ng.c || g[j].d <= ng.b) continue;
		mn = min(mn, g[j].b);
	}
	return max(-1, mn - ng.d);
}

bool execFix() {
	int i = rnd(n);
	int k = rnd(4);

	Region ng(g[i]);
	switch (k) {
	case 0:
	{
		int del = calcDownMax(i, ng);
		if (del <= 0) return false;
		del = min(del, x[i] - ng.a);
		ng.a += del;
		ng.c += del;
	}
	break;
	case 1:
	{
		int del = calcRightMax(i, ng);
		if (del <= 0) return false;
		del = min(del, y[i] - ng.b);
		ng.b += del;
		ng.d += del;
	}
	break;
	case 2:
	{
		int del = calcUpMax(i, ng);
		if (del <= 0) return false;
		del = min(del, ng.c - x[i] - 1);
		ng.a -= del;
		ng.c -= del;
	}
	break;
	case 3:
	{
		int del = calcLeftMax(i, ng);
		if (del <= 0) return false;
		del = min(del, ng.d - y[i] - 1);
		ng.b -= del;
		ng.d -= del;
	}
	break;
	}

	setRegion(i, ng);
	return true;
}

/* 上方向にどれだけ伸ばせるか */
int calcUpMaxF(int i, const Region& g) {
	if (!canForciblyMoveTo(i, g)) return -1;
	auto check = [i, &g](int x) {
		return canForciblyMoveTo(i, Region(g.a - x, g.b, g.c, g.d));
	};
	return findBoundary(check);
}

int calcLeftMaxF(int i, const Region& g) {
	if (!canForciblyMoveTo(i, g)) return -1;
	auto check = [i, &g](int x) {
		return canForciblyMoveTo(i, Region(g.a, g.b - x, g.c, g.d));
	};
	return findBoundary(check);
}

int calcDownMaxF(int i, const Region& g) {
	if (!canForciblyMoveTo(i, g)) return -1;
	auto check = [i, &g](int x) {
		return canForciblyMoveTo(i, Region(g.a, g.b, g.c + x, g.d));
	};
	return findBoundary(check);
}

int calcRightMaxF(int i, const Region& g) {
	if (!canForciblyMoveTo(i, g)) return -1;
	auto check = [i, &g](int x) {
		return canForciblyMoveTo(i, Region(g.a, g.b, g.c, g.d + x));
	};
	return findBoundary(check);
}

vector<int> getIntersecting(const Region& ng) {
	vector<int> ret(n);
	for (int i = 0; i < n; ++i) {
		ret[i] = intersects(ng, g[i]);
	}
	return ret;
}

bool execFlip() {
	int i = rnd(n);
	int k = rnd(8);
	bool ok = false;
	Region ng(g[i]);
	switch (k) {
	case 0:
	{
		int ex = fsize;
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].a == g[i].c && g[j].b > g[i].b) {
				if (g[j].b < ex) {
					ex = g[j].b;
					exj = j;
				}
			}
		}

		if (exj == -1) return false;
		int w = g[exj].b - g[i].b;
		ng.d = ng.b + w;
		int h = r[i] / w + 1;
		int dh = calcDownMax(i, ng);
		if (dh <= 0) return false;
		h = min(h, g[i].c - g[i].a + dh);
		ng.c = g[i].a + h;
		if (canMoveTo(i, ng)) ok = true;
	}
	break;
	case 1:
	{
		int ex = 0;
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].a == g[i].c && g[j].d < g[i].d) {
				if (g[j].d > ex) {
					ex = g[j].d;
					exj = j;
				}
			}
		}

		if (exj == -1) return false;
		int w = g[i].d - g[exj].d;
		ng.b = g[i].d - w;
		int h = r[i] / w + 1;
		int dh = calcDownMax(i, ng);
		if (dh <= 0) return false;
		h = min(h, g[i].c - g[i].a + dh);
		ng.c = g[i].a + h;
		if (canMoveTo(i, ng)) ok = true;
	}
	break;
	case 2:
	{
		int ex = fsize;
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].b == g[i].d && g[j].a > g[i].a) {
				if (g[j].a < ex) {
					ex = g[j].a;
					exj = j;
				}
			}
		}

		if (exj == -1) return false;
		int h = g[exj].a - g[i].a;
		ng.c = g[i].a + h;
		int w = r[i] / h + 1;
		int dw = calcRightMax(i, ng);
		if (dw <= 0) return false;
		w = min(w, g[i].d - g[i].b + dw);
		ng.d = g[i].b + w;
		if (canMoveTo(i, ng)) ok = true;
	}
	break;
	case 3:
	{
		int ex = 0;
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].b == g[i].d && g[j].c < g[i].c) {
				if (g[j].c > ex) {
					ex = g[j].c;
					exj = j;
				}
			}
		}

		if (exj == -1) return false;
		int h = g[i].c - g[exj].c;
		ng.a = g[i].c - h;
		int w = r[i] / h + 1;
		int dw = calcRightMax(i, ng);
		if (dw <= 0) return false;
		w = min(w, g[i].d - g[i].b + dw);
		ng.d = g[i].b + w;
		if (canMoveTo(i, ng)) ok = true;
	}
	break;
	case 4:
	{
		int ex = fsize;
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].c == g[i].a && g[j].b > g[i].b) {
				if (g[j].b < ex) {
					ex = g[j].b;
					exj = j;
				}
			}
		}

		if (exj == -1) return false;
		int w = g[exj].b - g[i].b;
		ng.d = g[i].b + w;
		int h = r[i] / w + 1;
		int dh = calcUpMax(i, ng);
		if (dh <= 0) return false;
		h = min(h, g[i].c - g[i].a + dh);
		ng.a = g[i].c - h;
		if (canMoveTo(i, ng)) ok = true;
	}
	break;
	case 5:
	{
		int ex = 0;
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].c == g[i].a && g[j].d < g[i].d) {
				if (g[j].d > ex) {
					ex = g[j].d;
					exj = j;
				}
			}
		}

		if (exj == -1) return false;
		int w = g[i].d - g[exj].d;
		ng.b = g[i].d - w;
		int h = r[i] / w + 1;
		int dh = calcUpMax(i, ng);
		if (dh <= 0) return false;
		h = min(h, g[i].c - g[i].a + dh);
		ng.a = g[i].c - h;
		if (canMoveTo(i, ng)) ok = true;
	}
	break;
	case 6:
	{
		int ex = fsize;
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].d == g[i].b && g[j].a > g[i].a) {
				if (g[j].a < ex) {
					ex = g[j].a;
					exj = j;
				}
			}
		}

		if (exj == -1) return false;
		int h = g[exj].a - g[i].a;
		ng.c = g[i].a + h;
		int w = r[i] / h + 1;
		int dw = calcLeftMax(i, ng);
		if (dw <= 0) return false;
		w = min(w, g[i].d - g[i].b + dw);
		ng.b = g[i].d - w;
		if (canMoveTo(i, ng)) ok = true;
	}
	break;
	case 7:
	{
		int ex = 0;
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].d == g[i].b && g[j].c < g[i].c) {
				if (g[j].c > ex) {
					ex = g[j].c;
					exj = j;
				}
			}
		}

		if (exj == -1) return false;
		int h = g[i].c - g[exj].c;
		ng.a = g[i].c - h;
		int w = r[i] / h + 1;
		int dw = calcLeftMax(i, ng);
		if (dw <= 0) return false;
		w = min(w, g[i].d - g[i].b + dw);
		ng.b = g[i].d - w;
		if (canMoveTo(i, ng)) ok = true;
	}
	break;
	}

	if (ok) setRegion(i, ng);
	return ok;
}

bool execFlipF() {
	int i = rnd(n);
	int k = rnd(8);
	Region ng(g[i]);
	buffer_mb.clear();
	switch (k) {
	case 0:
	{
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].a == g[i].c && g[j].b > g[i].b) {
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;
		exj = getSample(buffer_mb);

		int w = g[exj].b - g[i].b;
		ng.d = ng.b + w;
		int h = r[i] / w + 1;
		int dh = calcDownMaxF(i, ng);
		if (dh <= 0) return false;
		h = min(h, g[i].c - g[i].a + dh);
		ng.c = g[i].a + h;
	}
	break;
	case 1:
	{
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].a == g[i].c && g[j].d < g[i].d) {
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;
		exj = getSample(buffer_mb);

		int w = g[i].d - g[exj].d;
		ng.b = g[i].d - w;
		int h = r[i] / w + 1;
		int dh = calcDownMaxF(i, ng);
		if (dh <= 0) return false;
		h = min(h, g[i].c - g[i].a + dh);
		ng.c = g[i].a + h;
	}
	break;
	case 2:
	{
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].b == g[i].d && g[j].a > g[i].a) {
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;
		exj = getSample(buffer_mb);

		int h = g[exj].a - g[i].a;
		ng.c = g[i].a + h;
		int w = r[i] / h + 1;
		int dw = calcRightMaxF(i, ng);
		if (dw <= 0) return false;
		w = min(w, g[i].d - g[i].b + dw);
		ng.d = g[i].b + w;
	}
	break;
	case 3:
	{
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].b == g[i].d && g[j].c < g[i].c) {
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;
		exj = getSample(buffer_mb);

		int h = g[i].c - g[exj].c;
		ng.a = g[i].c - h;
		int w = r[i] / h + 1;
		int dw = calcRightMaxF(i, ng);
		if (dw <= 0) return false;
		w = min(w, g[i].d - g[i].b + dw);
		ng.d = g[i].b + w;
	}
	break;
	case 4:
	{
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].c == g[i].a && g[j].b > g[i].b) {
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;
		exj = getSample(buffer_mb);

		int w = g[exj].b - g[i].b;
		ng.d = g[i].b + w;
		int h = r[i] / w;
		int dh = calcUpMaxF(i, ng);
		if (dh <= 0) return false;
		h = min(h, g[i].c - g[i].a + dh);
		ng.a = g[i].c - h;
	}
	break;
	case 5:
	{
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].c == g[i].a && g[j].d < g[i].d) {
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;
		exj = getSample(buffer_mb);

		int w = g[i].d - g[exj].d;
		ng.b = g[i].d - w;
		int h = r[i] / w + 1;
		int dh = calcUpMaxF(i, ng);
		if (dh <= 0) return false;
		h = min(h, g[i].c - g[i].a + dh);
		ng.a = g[i].c - h;
	}
	break;
	case 6:
	{
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].d == g[i].b && g[j].a > g[i].a) {
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;
		exj = getSample(buffer_mb);

		int h = g[exj].a - g[i].a;
		ng.c = g[i].a + h;
		int w = r[i] / h + 1;
		int dw = calcLeftMaxF(i, ng);
		if (dw <= 0) return false;
		w = min(w, g[i].d - g[i].b + dw);
		ng.b = g[i].d - w;
	}
	break;
	case 7:
	{
		int exj = -1;
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[j].d == g[i].b && g[j].c < g[i].c) {
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;
		exj = getSample(buffer_mb);

		int h = g[i].c - g[exj].c;
		ng.a = g[i].c - h;
		int w = r[i] / h + 1;
		int dw = calcLeftMaxF(i, ng);
		if (dw <= 0) return false;
		w = min(w, g[i].d - g[i].b + dw);
		ng.b = g[i].d - w;
	}
	break;
	}

	return execForciblyMoveAndExpand(i, ng);
}

bool execMove() {
	const int range = 5;
	int i = rnd(n);
	int k = rnd(2);
	int ch = rnd(2 * range) - range;
	if (ch >= 0) ++ch;

	Region ng(g[i]);
	switch (k) {
	case 0: ng.a += ch; ng.c += ch; break;
	case 1: ng.b += ch; ng.d += ch; break;
	}

	if (!canMoveTo(i, ng)) return false;
	setRegion(i, ng);
	return true;
}

/* 重なりを回避するためにどう移動すればいいか */
Region calcSafePlace(int j, const Region& ng) {
	assert(!ng.contains(x[j], y[j]));

	int best = -1;
	Region bestg;

	if (x[j] >= ng.c) {
		Region nextg(ng.c, g[j].b, g[j].c, g[j].d);
		int area = nextg.area();
		if (area > best) {
			best = area;
			bestg = nextg;
		}
	}

	if (y[j] >= ng.d) {
		Region nextg(g[j].a, ng.d, g[j].c, g[j].d);
		int area = nextg.area();
		if (area > best) {
			best = area;
			bestg = nextg;
		}
	}

	if (x[j] < ng.a) {
		Region nextg(g[j].a, g[j].b, ng.a, g[j].d);
		int area = nextg.area();
		if (area > best) {
			best = area;
			bestg = nextg;
		}
	}

	if (y[j] < ng.b) {
		Region nextg(g[j].a, g[j].b, g[j].c, ng.b);
		int area = nextg.area();
		if (area > best) {
			best = area;
			bestg = nextg;
		}
	}

	return bestg;
}

bool execForciblyMoveTo(int i, const Region& ng) {
	if (!canForciblyMoveTo(i, ng)) return false;

	setRegion(i, ng);
	auto intersects_mb = getIntersecting(ng);
	intersects_mb[i] = false;		// 自分自身は処理しない
	for (int j = 0; j < n; ++j) {
		if (!intersects_mb[j]) continue;
		setRegion(j, calcSafePlace(j, ng));
	}
	return true;
}

void maximize(int i, Region& ng) {
	auto expHol = [i](Region& ng) {
		int h = r[i] / (ng.d - ng.b) + 1;
		tie(ng.a, ng.c) = calcHeightMaxEx(i, ng);
		if (ng.c - ng.a > h) {
			ng.a = max(ng.a, x[i] - h + 1);
			ng.c = min(ng.c, x[i] + h);
			if (rnd(2)) {
				ng.c = ng.a + h;
			} else {
				ng.a = ng.c - h;
			}
		}
	};

	auto expVer = [i](Region& ng) {
		int w = r[i] / (ng.c - ng.a) + 1;
		tie(ng.b, ng.d) = calcWidthMaxEx(i, ng);
		if (ng.d - ng.b > w) {
			ng.b = max(ng.b, y[i] - w + 1);
			ng.d = min(ng.d, y[i] + w);
			if (rnd(2)) {
				ng.d = ng.b + w;
			} else {
				ng.b = ng.d - w;
			}
		}
	};

	switch (rnd(2)) {
	case 0:
		expHol(ng);
		expVer(ng);
		break;
	case 1:
		expVer(ng);
		expHol(ng);
		break;
	}
}

bool execForciblyMoveAndExpand(int i, const Region& ng) {
	if (!canForciblyMoveTo(i, ng)) return false;

	setRegion(i, ng);
	auto intersects_mb = getIntersecting(ng);
	intersects_mb[i] = false;
	for (int j = 0; j < n; ++j) {
		if (!intersects_mb[j]) continue;
		Region place = calcSafePlace(j, ng);
		maximize(j, place);
		setRegion(j, place);
	}
	return true;
}

bool execForciblyMoveToDouble(int i1, int i2, const Region& ng, const Region& ng1, const Region& ng2, bool needs_check) {
	if (needs_check && !canForciblyMoveToDouble(i1, i2, ng)) return false;
	setRegion(i1, ng1);
	setRegion(i2, ng2);

	auto intersects_mb = getIntersecting(ng);
	intersects_mb[i1] = false;
	intersects_mb[i2] = false;
	for (int j = 0; j < n; ++j) {
		if (!intersects_mb[j]) continue;
		setRegion(j, calcSafePlace(j, ng));
	}
	return true;
}

bool execForciblyMoveAndExpandDouble(int i1, int i2, const Region& ng, const Region& ng1, const Region& ng2, bool needs_check) {
	if (needs_check && !canForciblyMoveToDouble(i1, i2, ng)) return false;
	setRegion(i1, ng1);
	setRegion(i2, ng2);

	auto intersects_mb = getIntersecting(ng);
	intersects_mb[i1] = false;
	intersects_mb[i2] = false;
	for (int j = 0; j < n; ++j) {
		if (!intersects_mb[j]) continue;
		Region place = calcSafePlace(j, ng);
		maximize(j, place);
		setRegion(j, place);
	}
	return true;
}

bool execUnyo() {
	const double factor = 0.95;
	int i = rnd(n);
	int k = rnd(4);
	int s = factor * r[i];
	int h = sqrt(s) * (rnd(60000) + 10000) / 10000 + 1;
	int w = s / h + 1;
	if (rnd(2)) swap(h, w);
	switch (k) {
	case 0: return execForciblyMoveAndExpand(i, Region(g[i].a, g[i].b, g[i].a + h, g[i].b + w));
	case 1: return execForciblyMoveAndExpand(i, Region(g[i].c - h, g[i].b, g[i].c, g[i].b + w));
	case 2: return execForciblyMoveAndExpand(i, Region(g[i].c - h, g[i].d - w, g[i].c, g[i].d));
	case 3: return execForciblyMoveAndExpand(i, Region(g[i].a, g[i].d - w, g[i].a + h, g[i].d));
	}
}

bool execUnyo2() {
	const double factor = 0.95;
	int i = rnd(n);
	int k = rnd(8);
	int s = factor * r[i];
	switch (k) {
	case 0:
	{
		int h = g[rnd(n + 1)].a - g[i].a;
		if (h <= 0) return false;
		int w = s / h + 1;
		return execForciblyMoveAndExpand(i, Region(g[i].a, g[i].b, g[i].a + h, g[i].b + w));
	}
	case 1:
	{
		int w = g[rnd(n + 1)].b - g[i].b;
		if (w <= 0) return false;
		int h = s / w + 1;
		return execForciblyMoveAndExpand(i, Region(g[i].a, g[i].b, g[i].a + h, g[i].b + w));
	}
	case 2:
	{
		int h = g[i].c - g[rnd(n + 1)].c;
		if (h <= 0) return false;
		int w = s / h + 1;
		return execForciblyMoveAndExpand(i, Region(g[i].c - h, g[i].b, g[i].c, g[i].b + w));
	}
	case 3:
	{
		int w = g[rnd(n + 1)].b - g[i].b;
		if (w <= 0) return false;
		int h = s / w + 1;
		return execForciblyMoveAndExpand(i, Region(g[i].c - h, g[i].b, g[i].c, g[i].b + w));
	}
	case 4:
	{
		int h = g[i].c - g[rnd(n + 1)].c;
		if (h <= 0) return false;
		int w = s / h + 1;
		return execForciblyMoveAndExpand(i, Region(g[i].c - h, g[i].d - w, g[i].c, g[i].d));
	}
	case 5:
	{
		int w = g[i].d - g[rnd(n + 1)].d;
		if (w <= 0) return false;
		int h = s / w + 1;
		return execForciblyMoveAndExpand(i, Region(g[i].c - h, g[i].d - w, g[i].c, g[i].d));
	}
	case 6:
	{
		int h = g[rnd(n + 1)].a - g[i].a;
		if (h <= 0) return false;
		int w = s / h + 1;
		return execForciblyMoveAndExpand(i, Region(g[i].a, g[i].d - w, g[i].a + h, g[i].d));
	}
	case 7:
	{
		int w = g[i].d - g[rnd(n + 1)].d;
		if (w <= 0) return false;
		int h = s / w + 1;
		return execForciblyMoveAndExpand(i, Region(g[i].a, g[i].d - w, g[i].a + h, g[i].d));
	}
	}
}

bool execExchange() {
	buffer_mb.clear();
	int i = rnd(n);
	int k = rnd(2);
	if (k == 0) {
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[i].c == g[j].a || g[i].a == g[j].c) {
				int nb = max(g[i].b, g[j].b);
				int nd = min(g[i].d, g[j].d);
				if (nb >= nd) continue;
				if (y[i] < nb || y[j] < nb) continue;
				if (y[i] >= nd || y[j] >= nd) continue;
				if (y[i] == y[j]) continue;
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;

		int exj = getSample(buffer_mb);
		int exi = i;
		if (y[exj] < y[exi]) swap(exi, exj);

		/* 以下 i, j ではなく exi, exj を使う */
		/* 広げすぎるパターンに注意 */
		Region nghull(hull(g[exi], g[exj]));
		if (canForciblyMoveToDouble(exi, exj, nghull)) {
			Region ng1(nghull);
			Region ng2(nghull);
			int mid = nghull.b + int64_t(nghull.d - nghull.b) * r[exi] / (r[exi] + r[exj]) + 1;
			mid = max(mid, y[exi] + 1);
			mid = min(mid, y[exj]);
			ng1.d = mid;
			ng2.b = mid;
			return execForciblyMoveAndExpandDouble(exi, exj, nghull, ng1, ng2, false);
		} else {
			// min と max に注意する
			int na = nghull.a;
			int nc = nghull.c;
			int nb = max(g[exi].b, g[exj].b);
			int nd = min(g[exi].d, g[exj].d);
			int mid = nb + int64_t(nd - nb) * r[exi] / (r[exi] + r[exj]) + 1;
			mid = max(mid, y[exi] + 1);
			mid = min(mid, y[exj]);
			setRegion(exi, Region(na, nb, nc, mid));
			setRegion(exj, Region(na, mid, nc, nd));
			return true;
		}
	} else {
		for (int j = 0; j < n; ++j) {
			if (j == i) continue;
			if (g[i].d == g[j].b || g[i].b == g[j].d) {
				int na = max(g[i].a, g[j].a);
				int nc = min(g[i].c, g[j].c);
				if (na >= nc) continue;
				if (x[i] < na || x[j] < na) continue;
				if (x[i] >= nc || x[j] >= nc) continue;
				if (x[i] == x[j]) continue;
				buffer_mb.push_back(j);
			}
		}
		if (buffer_mb.empty()) return false;

		int exj = getSample(buffer_mb);
		int exi = i;
		if (x[exj] < x[exi]) swap(exi, exj);

		Region nghull(hull(g[exi], g[exj]));
		if (canForciblyMoveToDouble(exi, exj, nghull)) {
			Region ng1(nghull);
			Region ng2(nghull);
			int mid = nghull.a + int64_t(nghull.c - nghull.a) * r[exi] / (r[exi] + r[exj]) + 1;
			mid = max(mid, x[exi] + 1);
			mid = min(mid, x[exj]);
			ng1.c = mid;
			ng2.a = mid;
			return execForciblyMoveAndExpandDouble(exi, exj, nghull, ng1, ng2, false);
		} else {
			int nb = nghull.b;
			int nd = nghull.d;
			int na = max(g[exi].a, g[exj].a);
			int nc = min(g[exi].c, g[exj].c);
			int mid = na + int64_t(nc - na) * r[exi] / (r[exi] + r[exj]) + 1;
			mid = max(mid, x[exi] + 1);
			mid = min(mid, x[exj]);
			setRegion(exi, Region(na, nb, mid, nd));
			setRegion(exj, Region(mid, nb, nc, nd));
			return true;
		}
	}
}

pair<int, int> calcHeightMaxEx(int i, const Region& ng) {
	int lower = 0;
	int upper = fsize;
	for (int j = 0; j < n; ++j) {
		if (j == i) continue;
		if (g[j].d <= ng.b || g[j].b >= ng.d) continue;
		if (g[j].c <= ng.a) {
			lower = max(lower, g[j].c);
		} else {
			upper = min(upper, g[j].a);
		}
	}
	return make_pair(lower, upper);
}

pair<int, int> calcWidthMaxEx(int i, const Region& ng) {
	int lower = 0;
	int upper = fsize;
	for (int j = 0; j < n; ++j) {
		if (j == i) continue;
		if (g[j].c <= ng.a || g[j].a >= ng.c) continue;
		if (g[j].d <= ng.b) {
			lower = max(lower, g[j].d);
		} else {
			upper = min(upper, g[j].b);
		}
	}
	return make_pair(lower, upper);
}

pair<int, int> calcHeightMaxAscend(int i, int nx, int ny, vector<int>& list) {
	int lower = 0;
	int upper = fsize;
	auto lit = remove_if(list.begin(), list.end(), [ny](int j) {return g[j].d <= ny; });
	list.erase(lit, list.end());
	for (int j : list) {
		if (g[j].b > ny) continue;
		if (g[j].c <= nx) {
			lower = max(lower, g[j].c);
		} else {
			upper = min(upper, g[j].a);
		}
	}
	return make_pair(lower, upper);
}

pair<int, int> calcHeightMaxDescend(int i, int nx, int ny, vector<int>& list) {
	int lower = 0;
	int upper = fsize;
	auto lit = remove_if(list.begin(), list.end(), [ny](int j) {return g[j].b > ny; });
	list.erase(lit, list.end());
	for (int j : list) {
		if (g[j].d <= ny) continue;
		if (g[j].c <= nx) {
			lower = max(lower, g[j].c);
		} else {
			upper = min(upper, g[j].a);
		}
	}
	return make_pair(lower, upper);
}

pair<int, int> calcWidthMaxAscend(int i, int nx, int ny, vector<int>& list) {
	int lower = 0;
	int upper = fsize;
	auto lit = remove_if(list.begin(), list.end(), [nx](int j) {return g[j].c <= nx; });
	list.erase(lit, list.end());
	for (int j : list) {
		if (g[j].a > nx) continue;
		if (g[j].d <= ny) {
			lower = max(lower, g[j].d);
		} else {
			upper = min(upper, g[j].b);
		}
	}
	return make_pair(lower, upper);
}

pair<int, int> calcWidthMaxDescend(int i, int nx, int ny, vector<int>& list) {
	int lower = 0;
	int upper = fsize;
	auto lit = remove_if(list.begin(), list.end(), [nx](int j) {return g[j].a > nx; });
	list.erase(lit, list.end());
	for (int j : list) {
		if (g[j].c <= nx) continue;
		if (g[j].d <= ny) {
			lower = max(lower, g[j].d);
		} else {
			upper = min(upper, g[j].b);
		}
	}
	return make_pair(lower, upper);
}

bool execDestroy() {
	const int len_mn = 300;
	const int len_mx = 1500;

	Region gdestroy;
	{
		int h = rnd(len_mx - len_mn) + len_mn;
		int w = rnd(len_mx - len_mn) + len_mn;
		int a = rnd(fsize - h + 1);
		int b = rnd(fsize - w + 1);
		gdestroy = Region(a, b, a + h, b + w);
	}

	auto intersects_mb = getIntersecting(gdestroy);
	int cnt = count(intersects_mb.begin(), intersects_mb.end(), true);
	if (cnt <= 1) return false;

	is_mb.clear();
	list_mb.clear();
	for (int i = 0; i < n; ++i) {
		if (intersects_mb[i]) {
			is_mb.push_back(i);
		} else {
			list_mb.push_back(i);
		}
	}

	const double factor = 0.95;
	int k = rnd(4);
	switch (k) {
	case 0:
	{
		sort(is_mb.begin(), is_mb.end(), [](int i, int j) { return x[i] < x[j]; });
		for (int p = 1; p < is_mb.size(); ++p) if (x[is_mb[p - 1]] == x[is_mb[p]]) return false;

		/* いったん領域を点にする (ひどい復元法なので注意する) */
		for (int i : is_mb) {
			swap(mg[i], g[i]);
			g[i] = Region(x[i], y[i]);
		}

		for (int i : is_mb) {
			Region ng(x[i], y[i]);
			tie(ng.b, ng.d) = calcWidthMaxAscend(i, x[i], y[i], list_mb);
			tie(ng.a, ng.c) = calcHeightMaxEx(i, ng);
			int h = int(factor * r[i] / (ng.d - ng.b)) + 1;
			if (ng.a <= x[i] - h) ng.a = x[i] - h + 1;
			if (ng.c - ng.a > h) ng.c = ng.a + h;
			swap(mg[i], g[i]);			// 注意
			list_mb.push_back(i);
			setRegion(i, ng);
		}
	}
	break;
	case 1:
	{
		sort(is_mb.begin(), is_mb.end(), [](int i, int j) { return y[i] < y[j]; });
		for (int p = 1; p < is_mb.size(); ++p) if (y[is_mb[p - 1]] == y[is_mb[p]]) return false;

		for (int i : is_mb) {
			swap(mg[i], g[i]);
			g[i] = Region(x[i], y[i]);
		}

		for (int i : is_mb) {
			Region ng(x[i], y[i]);
			tie(ng.a, ng.c) = calcHeightMaxAscend(i, x[i], y[i], list_mb);
			tie(ng.b, ng.d) = calcWidthMaxEx(i, ng);
			int w = int(factor * r[i] / (ng.c - ng.a)) + 1;
			if (ng.b <= y[i] - w) ng.b = y[i] - w + 1;
			if (ng.d - ng.b > w) ng.d = ng.b + w;
			swap(mg[i], g[i]);
			list_mb.push_back(i);
			setRegion(i, ng);
		}
	}
	break;
	case 2:
	{
		sort(is_mb.begin(), is_mb.end(), [](int i, int j) { return x[i] > x[j]; });
		for (int p = 1; p < is_mb.size(); ++p) if (x[is_mb[p - 1]] == x[is_mb[p]]) return false;

		for (int i : is_mb) {
			swap(mg[i], g[i]);
			g[i] = Region(x[i], y[i]);
		}

		for (int i : is_mb) {
			Region ng(x[i], y[i]);
			tie(ng.b, ng.d) = calcWidthMaxDescend(i, x[i], y[i], list_mb);
			tie(ng.a, ng.c) = calcHeightMaxEx(i, ng);
			int h = int(factor * r[i] / (ng.d - ng.b)) + 1;
			if (ng.c > x[i] + h) ng.c = x[i] + h;
			if (ng.c - ng.a > h) ng.a = ng.c - h;
			swap(mg[i], g[i]);
			list_mb.push_back(i);
			setRegion(i, ng);
		}
	}
	break;
	case 3:
	{
		sort(is_mb.begin(), is_mb.end(), [](int i, int j) { return y[i] > y[j]; });
		for (int p = 1; p < is_mb.size(); ++p) if (y[is_mb[p - 1]] == y[is_mb[p]]) return false;

		for (int i : is_mb) {
			swap(mg[i], g[i]);
			g[i] = Region(x[i], y[i]);
		}

		for (int i : is_mb) {
			Region ng(x[i], y[i]);
			tie(ng.a, ng.c) = calcHeightMaxDescend(i, x[i], y[i], list_mb);
			tie(ng.b, ng.d) = calcWidthMaxEx(i, ng);
			int w = int(factor * r[i] / (ng.c - ng.a)) + 1;
			if (ng.d > y[i] + w) ng.d = y[i] + w;
			if (ng.d - ng.b > w) ng.b = ng.d - w;
			swap(mg[i], g[i]);
			list_mb.push_back(i);
			setRegion(i, ng);
		}
	}
	break;
	}

	return true;
}

bool execNobashi() {
	int i = rnd(n);
	int k = rnd(2);

	Region ng(g[i]);
	switch (k) {
	case 0:
	{
		int h = r[i] / (ng.d - ng.b) + 1;
		tie(ng.a, ng.c) = calcHeightMaxEx(i, ng);
		if (ng.c - ng.a > h) {
			ng.a = max(ng.a, x[i] - h + 1);
			ng.c = min(ng.c, x[i] + h);
			if (rnd(2)) {
				ng.c = ng.a + h;
			} else {
				ng.a = ng.c - h;
			}
		}
	}
	break;
	case 1:
	{
		int w = r[i] / (ng.c - ng.a) + 1;
		tie(ng.b, ng.d) = calcWidthMaxEx(i, ng);
		if (ng.d - ng.b > w) {
			ng.b = max(ng.b, y[i] - w + 1);
			ng.d = min(ng.d, y[i] + w);
			if (rnd(2)) {
				ng.d = ng.b + w;
			} else {
				ng.b = ng.d - w;
			}
		}
	}
	break;
	}
	if (ng == g[i]) return false;
	setRegion(i, ng);
	return true;
}

double exec() {
	const int type_num = 9;
	int type = rnd(type_num);
	const int iteration_max = 5;
	for (int z = 0; z < iteration_max; ++z) {
		switch (type) {
		case 0:
			if (execShrink()) goto end;
			break;
		case 1:
			if (execExpand()) goto end;
			break;
		case 2:
			if (execFix()) goto end;
			break;
		case 3:
			if (execFlipF()) goto end;
			break;
		case 4:
			if (execFlip()) goto end;
			break;
		case 5:
			if (execUnyo()) goto end;
			break;
		case 6:
			if (execExchange()) goto end;
			break;
		case 7:
			if (execDestroy()) goto end;
			break;
		case 8:
			if (execUnyo2()) goto end;
			break;
		}
	}

	// 最後まで成功しなかった
	return 0.0;

end:

	//psc = sc;
	double sum = 0.0;
	for (int i : mis) {
		sum += updateScore(i);
	}
	return sum;
}

void undo() {
	//sc = psc;
	for (int i : mis) {
		scs[i] = pscs[i];
	}
	loadRegions();
}

void step(double tmp) {
	mis.clear();
	double del = exec();
	if (!isAccepted(del, tmp)) {
		undo();
	}
}

double updateScore(int i) {
	pscs[i] = scs[i];
	scs[i] = calc(i);
	double delta = scs[i] - pscs[i];
	//sc += delta;
	return delta;
}

double calc(int i) {
	int s = g[i].area();
	int t = r[i];
	double ratio = double(t - s) / t;
	double ret = (1 - ratio * ratio) / n;
	return ret;
}

//double getScore() {
//	return sc;
//}

OutputData get() {
	OutputData ret;
	ret.a.resize(n);
	ret.b.resize(n);
	ret.c.resize(n);
	ret.d.resize(n);
	for (int i = 0; i < n; ++i) {
		ret.a[i] = g[i].a;
		ret.b[i] = g[i].b;
		ret.c[i] = g[i].c;
		ret.d[i] = g[i].d;
	}
	return ret;
}
#ifdef ONLY_MY_ENVIR
#include "Main.h"
#include "Generator.h"
#include "Timer.h"
#include "Random.h"
#include "Global.h"
#include "Solver.h"
#include "Final.h"
#endif

#include <chrono>

#ifdef ONLY_MY_ENVIR
ifstream input_stream("in.txt");
ofstream output_stream("out.txt");
#endif

OutputData solveGreedyNaive(const InputData& in) {
	const int n = in.size;
	
	OutputData out;
	const auto& x = in.x;
	const auto& y = in.y;
	const auto& r = in.r;
	auto& a = out.a;
	auto& b = out.b;
	auto& c = out.c;
	auto& d = out.d;
	a.resize(n);
	b.resize(n);
	c.resize(n);
	d.resize(n);
	
	vector<int> ord(n);
	iota(ord.begin(), ord.end(), 0);
	sort(ord.begin(), ord.end(), [&](int i, int j) {
		return r[i] < r[j];
	});

	const int mx = 10000;
	vector<int> rem;
	vector<vector<bool>> used(mx, std::vector<bool>(mx));
	for (int z = 0; z < n; ++z) {
		int i = ord[z];
		int h = sqrt(r[i]);
		int w = r[i] / h;

		bool ok = true;
		if (x[i] + h > mx || y[i] + w > mx) {
			ok = false;
		} else {
			for (int p = 0; p < h; ++p) {
				for (int q = 0; q < w; ++q) {
					if (used[x[i] + p][y[i] + q]) {
						ok = false;
						goto end;
					}
				}
			}
		}
	end:

		if (ok) {
			a[i] = x[i];
			b[i] = y[i];
			c[i] = x[i] + h;
			d[i] = y[i] + w;

			REP(p, a[i], c[i]) {
				REP(q, b[i], d[i]) {
					used[p][q] = true;
				}
			}
		} else {
			rem.push_back(i);
		}
	}

	for (int i : rem) {
		int p;
		int q;
		do {
			p = rnd(mx);
			q = rnd(mx);
		} while (used[p][q]);
		a[i] = p;
		b[i] = q;
		c[i] = p + 1;
		d[i] = q + 1;
		used[p][q] = true;
	}
	return out;
}

#ifdef ONLY_MY_ENVIR
#define EVALUATE 1
#define EVALPARA 0
#define VERBOSE !EVALUATE
#endif

#ifdef ONLY_MY_ENVIR
OutputData solveAnneal(const InputData& in, double tmp_start, double tmp_finish) {
	const int iterations = int(5e8) / in.size * 2 / 3;
	const int limit = 4990000;
	//const double tmp_start = 2e-4;
	//const double tmp_finish = 5e-7;
	double tmp = tmp_start;

	Timer timer;
	timer.set(limit);

	int z = 0;
	Annealer annealer(in);
#if ONLY_MY_ENVIR
	while (z < iterations) {
#else
	while (z % 4096 != 0 || !timer.elapsed()) {
#endif
		++z;
		if (z % 4096 == 0) {
#if ONLY_MY_ENVIR
			double ratio = double(z) / iterations;
#else
			double ratio = double(timer.getMicrosec()) / limit;
#endif
			tmp = tmp_start * std::pow(tmp_finish / tmp_start, ratio);
#if VERBOSE
			if (z % 16384 == 0) {
				cerr << tmp << ' ' << annealer.getScore() << endl;
			}
#endif
		}
		annealer.step(tmp);
	}

#if !EVALUATE
	cerr << z << endl;
	cerr << annealer.getScore() << endl;
#endif

	auto out = annealer.get();
	return out;
}
#endif

OutputData solveAnnealFinal(const InputData& in, double tmp_start, double tmp_finish) {
	const int iterations = int(5e8) / in.size / 2;
	const int limit = 4997000;
	double tmp = tmp_start;

	Timer timer;
	timer.set(limit);

	int z = 0;
	init(in);
#if ONLY_MY_ENVIR
	while (z < iterations) {
#else
	while (z % 4096 != 0 || !timer.elapsed()) {
#endif
		++z;
		if (z % 4096 == 0) {
#if ONLY_MY_ENVIR
			double ratio = double(z) / iterations;
#else
			double ratio = double(timer.getMicrosec()) / limit;
#endif
			tmp = tmp_start * std::pow(tmp_finish / tmp_start, ratio);
#if VERBOSE
			if (z % 16384 == 0) {
				cerr << tmp << ' ' << getScore() << endl;
			}
#endif
		}
		step(tmp);
	}

#if !EVALUATE
	cerr << z << endl;
#endif

	auto out = get();
	return out;
}

#ifdef ONLY_MY_ENVIR
void evaluate(function<OutputData(const InputData&)> func, int start, int end) {
	using namespace chrono;
	auto time_start = system_clock::now();

	ofstream ofs("result.txt");
	ofs << endl;
	if (!ofs) throw std::runtime_error("output failed");

	for (int i = start; i < end; ++i) {
		stringstream sst;
		sst << "samples/" << i << ".txt";
		ifstream ifs(sst.str());
		auto in = readInput(ifs);
		auto out = func(in);

		int res = judge(in, out);
		cerr << res << endl;
		ofs << res << endl;
	}

	auto time_finish = system_clock::now();
	auto duration = duration_cast<milliseconds>(time_finish - time_start).count();
	cerr << "avg " << duration / (end - start) << " ms" << endl;
}

#include <future>
int64_t evaluateParallel(function<OutputData(const InputData&)> func, int start, int end) {
	using namespace chrono;
	auto time_start = system_clock::now();

	ofstream ofs("result.txt");
	ofs << endl;
	if (!ofs) throw std::runtime_error("output failed");

	const int capacity = 4;
	vector<InputData> ins;
	for (int i = start; i < end; ++i) {
		stringstream sst;
		sst << "samples/" << i << ".txt";
		ifstream ifs(sst.str());
		ins.push_back(readInput(ifs));
	}

	vector<int> ord;
	for (int i = start; i < end; ++i) {
		ord.push_back(i);
	}

	sort(ord.begin(), ord.end(), [&ins](int i, int j) -> bool {
		return ins[i].size > ins[j].size;
		});

	vector<int> ress(end - start);
	for (int s = 0; s < end - start; s += capacity) {
		vector<future<OutputData>> outs;
		for (int i = s; i < min(end, s + capacity); ++i) {
			outs.emplace_back(async(
				launch::async,
				[i, start, &ins, func, &ord]() -> OutputData {
					return func(ins[ord[i - start]]);
				})
			);
		}

		for (int i = s; i < min(end, s + capacity); ++i) {
			int res = judge(ins[ord[i - start]], outs[i - s].get());
			ress[ord[i - start]] = res;
			cerr << res << endl;
		}
	}

	int64_t sum = 0;
	for (int res : ress) {
		sum += res;
		ofs << res << endl;
	}
	return sum;
}
#endif

int main() {
	//double sts[] = { 1e-3, 1.1e-3, 1.2e-3, 1.3e-3, 1.5e-3 };
	//double ens[] = { 2e-7 };

	//ofstream ofs("res_temp.txt");
	//for (double st : sts) {
	//	for (double en : ens) {
	//		auto func = [st, en](const InputData& in) -> OutputData {
	//			return solveAnneal(in, st, en);
	//		};
	//		
	//		int64_t res = evaluateParallel(func, 0, 200);
	//		ofs << st << ' ' << en << ' ' << res << endl;
	//		cerr << res << ' ';
	//	}
	//	cerr << endl;
	//}
	//return 0;


#ifdef ONLY_MY_ENVIR
	auto func = [](const InputData& in) -> OutputData {
		const double st = 1e-3;
		const double en = 2e-7;
		return solveAnneal(in, st, en);
	};
#endif

	auto funcFinal = [](const InputData& in) -> OutputData {
		const double st = 1e-3;
		const double en = 2e-7;
		return solveAnnealFinal(in, st, en);
	};

#if EVALPARA
	evaluateParallel(func, 0, 200);
#elif EVALUATE
	evaluate(funcFinal, 0, 10);
#else
	auto in = readInput(cin);
	auto out = funcFinal(in);
	writeOutput(cout, out);
	cerr << judge(in, out) << endl;
#endif
 	return 0;
}
