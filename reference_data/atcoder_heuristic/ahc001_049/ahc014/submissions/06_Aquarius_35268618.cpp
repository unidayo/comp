#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

#ifdef ONLY_MY_ENVIR

#include <chrono>
class Timer {
	const std::chrono::time_point<std::chrono::system_clock> start;
	std::int64_t limit_microsec = 0;

public:
	Timer();
	void set(std::int64_t time_microsec);
	bool elapsed() const;
	std::int64_t getMicrosec() const;
};

#else

class Timer {
	static const std::uint64_t ClocksPerMicrosec;
	const std::uint64_t start_clocks;
	std::uint64_t limit_clocks;

	std::uint64_t getClocks() const;

public:
	Timer();
	void set(std::int64_t time_microsec);
	bool elapsed() const;
	std::int64_t getMicrosec() const;
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

void Timer::set(std::int64_t time_microsec) {
	limit_microsec += time_microsec;
}

bool Timer::elapsed() const {
	return getMicrosec() >= limit_microsec;
}

std::int64_t Timer::getMicrosec() const {
	return duration_cast<microseconds>(system_clock::now() - start).count();
}

#else

const std::uint64_t Timer::ClocksPerMicrosec = 2987;

std::uint64_t Timer::getClocks() const {
	unsigned int lo, hi;
	__asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((std::uint64_t)hi << 32) | lo;
}

Timer::Timer() 
	: start_clocks(getClocks()), limit_clocks(start_clocks) {
}

void Timer::set(std::int64_t time_microsec) {
	limit_clocks += time_microsec * ClocksPerMicrosec;
}

bool Timer::elapsed() const {
	return getClocks() >= limit_clocks;
}

std::int64_t Timer::getMicrosec() const {
	return (getClocks() - start_clocks) / ClocksPerMicrosec;
}

#endif

#ifndef RANDOM_H
#define RANDOM_H

#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>

class Random {
	std::uint32_t seed_;

public:
	Random(std::uint32_t seed = 3141592653ul);

	std::uint32_t rnd();
	std::uint32_t rnd(std::uint32_t max);

	// [min, max) の一様乱数
	std::uint32_t rnd(std::uint32_t min, std::uint32_t max);

	template <class Container>
	auto sample(const Container& arr) -> decltype(arr[0]);
	template <class Container>
	void shuffle(Container& arr);

	template <class T>
	std::vector<T> generateVector(int size, std::uint32_t max);
	template <class T>
	std::vector<T> generateVector(int size, std::int32_t min, std::int32_t max);
};

template <class Container>
auto Random::sample(const Container& arr) -> decltype(arr[0]) {
	assert(!arr.empty());
	return arr[rnd(arr.size())];
}

template <class Container>
void Random::shuffle(Container& arr) {
	for (int i = std::size(arr); i > 1; --i) {
		std::swap(arr[i - 1], arr[rnd(i)]);
	}
}

template <class T>
std::vector<T> Random::generateVector(int size, std::uint32_t max) {
	std::vector<T> ret(size);
	for (int i = 0; i < size; ++i) {
		ret[i] = rnd(max);
	}
	return ret;
}

template <class T>
std::vector<T> Random::generateVector(int size, std::int32_t min, std::int32_t max) {
	assert(min < max);
	std::vector<T> ret(size);
	for (int i = 0; i < size; ++i) {
		ret[i] = min + (std::int64_t)rnd(std::int64_t(max) - min);
	}
	return ret;
}

#endif
#ifdef ONLY_MY_ENVIR
#include "Random.h"
#endif

Random::Random(std::uint32_t seed) :
	seed_(seed) {
	assert(seed != 0);
}

std::uint32_t Random::rnd() {
	seed_ ^= seed_ << 13;
	seed_ ^= seed_ >> 17;
	seed_ ^= seed_ << 15;
	return seed_;
}

std::uint32_t Random::rnd(std::uint32_t max) {
	return rnd() % max;
}

std::uint32_t Random::rnd(std::uint32_t min, std::uint32_t max) {
	assert(min < max);
	return min + rnd(max - min);
}

#ifndef __MAIN_H__
#define __MAIN_H__

#ifndef ONLY_MY_ENVIR
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
#endif

#ifdef ONLY_MY_ENVIR
#include "Includes.h"
#endif

using namespace std;
using uint = std::uint32_t;
using LL = std::int64_t;
using ULL = std::uint64_t;
using PP = pair<LL, LL>;
template <typename T> using PriorityQ = priority_queue<T, vector<T>, greater<T> >;
#define REP(i, a, n) for(LL i = (a), i##_max_ = (n); i < i##_max_; ++i)
#define REM(i, a, n) for(LL i = (LL)(n) - 1, i##_min_ = (a); i >= i##_min_; --i)
#define FLOAT fixed << setprecision(16)
#define SPEEDUP { cin.tie(NULL); ios::sync_with_stdio(false); }
const int INF = 0x3FFFFFFF;
const LL INFLL = 0x3FFFFFFF3FFFFFFF;
const double INFD = 1.0e+308;
const double EPS = 1.0e-9;

inline void YesNo(bool b) { cout << (b ? "Yes" : "No") << endl; }
inline void YESNO(bool b) { cout << (b ? "YES" : "NO") << endl; }
template <class T, class U> istream& operator>>(istream& ist, pair<T, U>& right) { return ist >> right.first >> right.second; }
template <class T, class U> ostream& operator<<(ostream& ost, const pair<T, U>& right) { return ost << right.first << ' ' << right.second; }
template <class T, class TCompatible, size_t N> void Fill(T(&dest)[N], const TCompatible& val) { fill(dest, dest + N, val); }
template <class T, class TCompatible, size_t M, size_t N> void Fill(T(&dest)[M][N], const TCompatible& val) { for (int i = 0; i < M; ++i) Fill(dest[i], val); }
template <class T> T Next() { T buf; cin >> buf; return buf; }
inline istream& Ignore(istream& ist) { string s; ist >> s; return ist; }
inline bool Inside(int i, int j, int h, int w) { return i >= 0 && i < h&& j >= 0 && j < w; }
inline void pL() { std::cout << std::endl; }
template <class Head> void pL(Head&& head) { std::cout << head << std::endl; }
template <class Head, class... Tail> void pL(Head&& head, Tail&&... tail) { std::cout << head << ' '; pL(std::forward<Tail>(tail)...); }

#ifdef __GNUC__
using LLL = __int128;
istream& operator>>(istream& ist, __int128& val) { LL tmp; ist >> tmp; val = tmp; return ist; }
ostream& operator<<(ostream& ost, __int128 val) { LL tmp = val; ost << tmp; return ost; }
#else
using LLL = LL;
#endif

constexpr int di[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
constexpr int dj[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
constexpr int dsgn[] = { 1, 1, 1, -1, -1, -1, -1, 1 };

using Pos = pair<short, short>;

struct Input
{
	int n;
	vector<Pos> ps;
};

struct Segment
{
	Pos a, b;

	Segment(const Pos& a, const Pos& b)
		: a(a), b(b)
	{
	}

	bool containsAsLine(const Pos& p) const
	{
		int abi = b.first - a.first;
		int abj = b.second - a.second;
		int api = p.first - a.first;
		int apj = p.second - a.second;
		return abi * apj == abj * api;
	}

	bool contains(const Pos& p) const
	{
		if (!containsAsLine(p)) return false;
		int abi = b.first - a.first;
		int abj = b.second - a.second;
		int api = p.first - a.first;
		int apj = p.second - a.second;
		int bpi = p.first - b.first;
		int bpj = p.second - b.second;
		return abi * api + abj * apj >= 0
			&& abi * bpi + abj * bpj <= 0;
	}

	bool containsExceptEndpoints(const Pos& p) const
	{
		if (!containsAsLine(p)) return false;
		int abi = b.first - a.first;
		int abj = b.second - a.second;
		int api = p.first - a.first;
		int apj = p.second - a.second;
		int bpi = p.first - b.first;
		int bpj = p.second - b.second;
		return abi * api + abj * apj > 0
			&& abi * bpi + abj * bpj < 0;
	}

	int getDir() const
	{
		auto i = b.first - a.first;
		auto j = b.second - a.second;
		if (i == 0 && j == 0) return -1;
		REP(k, 0, 8)
		{
			if (i * dj[k] == j * di[k] && i * di[k] + j * dj[k] > 0) {
				return k;
			}
		}
		return -1;
	}
};

struct Rect
{
	Pos a, b, c, d;

	Rect(const Pos& a, const Pos& b, const Pos& c, const Pos& d)
		: a(a), b(b), c(c), d(d)
	{
	}

	array<Segment, 4> toSegments() const
	{
		return array<Segment, 4>{
			Segment(a, b),
			Segment(b, c),
			Segment(c, d),
			Segment(d, a)
		};
	}

	bool containsExceptVertices(const Pos& p) const
	{
		for (const auto& s : toSegments()) {
			if (s.containsExceptEndpoints(p)) return true;
		}
		return false;
	}

	bool isValid() const
	{
		vector<int> ks;
		for (const auto& s : toSegments()) {
			int k = s.getDir();
			if (k == -1) return false;
			ks.push_back(k);
		}
		int k = (ks[1] - ks[0] + 8) % 8;
		if (k != 2 && k != 6) return false;
		REP(x, 1, 3)
		{
			if ((ks[x + 1] - ks[x] + 8) % 8 != k) {
				return false;
			}
		}
		return true;
	}
};

using Output = std::vector<Rect>;

Input input(std::istream& ist);
void output(std::ostream& ost, const Output& out);
Output solve(const Input& in);

inline bool overlaps(const Segment& s, const Segment& t)
{
	if (!s.containsAsLine(t.a)) return false;
	if (!s.containsAsLine(t.b)) return false;

	auto getPos = [](const Segment& s, const Pos& p)
	{
		const auto& a = s.a;
		const auto& b = s.b;

		int abi = b.first - a.first;
		int abj = b.second - a.second;
		int api = p.first - a.first;
		int apj = p.second - a.second;
		int bpi = p.first - b.first;
		int bpj = p.second - b.second;
		if (abi * api + abj * apj <= 0) return -1;
		if (abi * bpi + abj * bpj >= 0) return 1;
		return 0;
	};

	return getPos(s, t.a) * getPos(s, t.b) != 1;
}

inline bool overlaps(const Rect& a, const Rect& b)
{
	for (auto&& s : a.toSegments()) {
		for (auto&& t : b.toSegments()) {
			if (overlaps(s, t)) return true;
		}
	}
	return false;
}

#endif
#ifdef ONLY_MY_ENVIR
#include "Main.h"
#endif

#define SREP(i, a, n) for(short i = (a), i##_max_ = (n); i < i##_max_; ++i)
#define SREM(i, a, n) for(short i = (short)(n) - 1, i##_min_ = (a); i >= i##_min_; --i)

Input input(std::istream& ist)
{
	Input in;
	int m;
	ist >> in.n >> m;
	REP(z, 0, m)
	{
		int i, j;
		ist >> i >> j;
		in.ps.emplace_back(i, j);
	}
	return in;
}

void output(std::ostream& ost, const Output& out)
{
	ost << out.size() << endl;
	for (auto&& p : out) {
		ost << p.a << ' '
			<< p.b << ' '
			<< p.c << ' '
			<< p.d << endl;
	}
}

int calcScore(int n, const Pos& p)
{
	int c = n / 2;
	int ai = p.first - c;
	int aj = p.second - c;
	return ai * ai + aj * aj + 1;
}

void findErase(array<short, 4>& v, short i)
{
	for (auto&& x : v) {
		if (x == i) {
			x = -1;
			break;
		}
	}
}

void addElement(array<short, 4>& v, short i)
{
	for (auto&& x : v) {
		if (x != -1) continue;
		x = i;
		break;
	}
}

void printScore(const Input& in, int score)
{
	cerr << score << endl;
	double den = 0;
	SREP(i, 0, in.n)
	{
		SREP(j, 0, in.n)
		{
			den += calcScore(in.n, Pos(i, j));
		}
	}
	int real = 1e6 * in.n * in.n / in.ps.size() * score / den;
	cerr << real << endl;
}

struct Op
{
	short pa;
	short b;
	short c;
	short d;
	short k;
};

using Ty = pair<int, Op>;
struct GComp
{
	bool operator()(const Ty& a, const Ty& b) const
	{
		return a.first > b.first;
	}
};

bool getBit(uint8_t x, int k)
{
	return (x >> k) & 1;
}

void setBit(uint8_t& x, int k)
{
	x |= (1 << k);
}

void resetBit(uint8_t& x, int k)
{
	x &= ~(1 << k);
}

double calcAlpha(const Input& in)
{
	double ret = 1.0;
	ret *= in.ps.size();
	ret /= in.n * in.n;

	double den = 0;
	SREP(i, 0, in.n)
	{
		SREP(j, 0, in.n)
		{
			den += calcScore(in.n, Pos(i, j));
		}
	}
	ret *= den;

	double m_min = in.n;
	double m_max = in.n * in.n / 12.0;
	double rate = (in.ps.size() - m_min) / (m_max - m_min);
	ret *= 1.0 + 1.0 * rate;
	ret /= 7e4;
	return ret;
}

struct Helper
{
	const short n;				// const
	const short sent_size;		// const
	const short orig_size;		// const
	short dp[8];			    // const

	vector<short> f;
	vector<Op> ops;		// 前半 const
	vector<array<short, 4>> h;
	vector<array<short, 8>> adj;
	vector<uint8_t> used;

	int scr = 0;
	vector<int> stable;

	// memo
	int mscr = 0;
	vector<short> mf;
	vector<Op> mops;
	vector<array<short, 4>> mh;
	vector<array<short, 8>> madj;
	vector<uint8_t> mused;

	// best
	int bscr = 0;
	vector<Op> bops;

	double rate;

	Helper(const Input& in)
		: n(in.n + 2)
		, sent_size(4 * (n - 1))
		, orig_size(in.ps.size())
		, f(n * n, -1)
		, stable(n * n)
		, mf(n * n)
		, alpha_(calcAlpha(in))
	{
		double m_min = in.n;
		double m_max = in.n * in.n / 12.0;
		rate = (in.ps.size() - m_min) / (m_max - m_min);

		SREP(k, 0, 8) dp[k] = ver(di[k], dj[k]);

		vector<short> ps;
		SREP(j, 0, n) ps.push_back(ver(0, j));
		SREP(j, 0, n) ps.push_back(ver(n - 1, j));
		SREP(i, 1, n - 1) ps.push_back(ver(i, 0));
		SREP(i, 1, n - 1) ps.push_back(ver(i, n - 1));
		for (auto&& p : in.ps) ps.push_back(ver(p.first + 1, p.second + 1));

		SREP(x, 0, ps.size())
		{
			f[ps[x]] = x;
			ops.push_back({ ps[x], -1, -1, -1 });
			h.push_back(array<short, 4>{ -1, -1, -1, -1 });
			adj.push_back(array<short, 8>{ -1, -1, -1, -1, -1, -1, -1, -1 });
			used.push_back(0);
		}
		mops = ops;

		SREP(x, 0, ps.size())
		{
			SREP(k, 0, 8)
			{
				short i = ps[x] / n;
				short j = ps[x] % n;
				while (true) {
					i += di[k];
					j += dj[k];
					if (!Inside(i, j, n, n)) break;
					if (f[ver(i, j)] != -1) {
						adj[x][k] = f[ver(i, j)];
						break;
					}
				}
			}
		}

		SREP(i, 0, n)
		{
			SREP(j, 0, n)
			{
				stable[ver(i, j)] = calcScore(n - 2, Pos(i - 1, j - 1));
			}
		}

		toMaximal();
	}

	void add(const Op& op)
	{
		auto nearest = [this](short pa, short k) -> short
		{
			while (true) {
				pa += dp[k];
				if (f[pa] != -1) return f[pa];
			}
		};

		short a = ops.size();
		f[op.pa] = a;
		ops.push_back(op);

		addElement(h[op.b], a);
		addElement(h[op.c], a);
		addElement(h[op.d], a);
		h.push_back(array<short, 4>{ -1, -1, -1, -1 });
		scr += stable[op.pa];

		adj.push_back(array<short, 8>{ -1, -1, -1, -1, -1, -1, -1, -1 });
		used.push_back(0);

		{
			short k = op.k;
			short b = op.b;
			short c = adj[b][k ^ 4];
			adj[a][k] = b;
			adj[a][k ^ 4] = c;
			adj[b][k ^ 4] = a;
			adj[c][k] = a;

			if (getBit(used[c], k)) {
				setBit(used[a], k);
				setBit(used[a], k ^ 4);
			}
		}
		{
			short k = (op.k + 2) % 8;
			short b = op.d;
			short c = adj[b][k ^ 4];
			adj[a][k] = b;
			adj[a][k ^ 4] = c;
			adj[b][k ^ 4] = a;
			adj[c][k] = a;

			if (getBit(used[c], k)) {
				setBit(used[a], k);
				setBit(used[a], k ^ 4);
			}
		}

		SREP(k, 0, 4)
		{
			if (k % 2 == op.k % 2) continue;
			short b = nearest(op.pa, k);
			short c = adj[b][k ^ 4];
			adj[a][k] = b;
			adj[a][k ^ 4] = c;
			adj[b][k ^ 4] = a;
			adj[c][k] = a;

			if (getBit(used[c], k)) {
				setBit(used[a], k);
				setBit(used[a], k ^ 4);
			}
		}

		setBit(used[a], op.k);
		setBit(used[a], (op.k + 2) % 8);
		setBit(used[op.b], (op.k + 4) % 8);
		setBit(used[op.b], (op.k + 2) % 8);
		setBit(used[op.c], (op.k + 4) % 8);
		setBit(used[op.c], (op.k + 6) % 8);
		setBit(used[op.d], op.k);
		setBit(used[op.d], (op.k + 6) % 8);
	}

	vector<char> del;
	vector<short> stack;
	vector<short> mapping;
	void eraseAll(const vector<short>& xs)
	{
		auto erase = [this](short a) -> void
		{
			auto eraseLine = [this](short a, short b, short k) -> void
			{
				resetBit(used[a], k);
				a = adj[a][k];
				while (a != b) {
					resetBit(used[a], k);
					resetBit(used[a], k ^ 4);
					a = adj[a][k];
				}
				resetBit(used[b], k ^ 4);
			};

			short p = ops[a].pa;
			f[p] = -1;

			short b = ops[a].b;
			short c = ops[a].c;
			short d = ops[a].d;
			short k = ops[a].k;
			eraseLine(a, b, k);
			eraseLine(b, c, (k + 2) % 8);
			eraseLine(c, d, (k + 4) % 8);
			eraseLine(d, a, (k + 6) % 8);

			SREP(k, 0, 4)
			{
				short u = adj[a][k];
				short l = adj[a][k ^ 4];
				adj[u][k ^ 4] = l;
				adj[l][k] = u;
			}

			findErase(h[b], a);
			findErase(h[c], a);
			findErase(h[d], a);
			scr -= stable[p];
		};

		del.assign(ops.size(), false);
		stack.clear();
		for (short rx : xs) {
			stack.push_back(rx);
		}

		while (!stack.empty()) {
			short x = stack.back();
			stack.pop_back();
			if (del[x]) continue;
			del[x] = true;
			for (short y : h[x]) {
				if (y == -1) continue;
				stack.push_back(y);
			}
		}

		SREM(x, offset(), ops.size())
		{
			if (del[x]) {
				erase(x);
			}
		}

		// normalize
		mapping.resize(ops.size());
		std::iota(mapping.begin(), mapping.begin() + offset(), 0);
		{
			short id = offset();
			SREP(x, offset(), ops.size())
			{
				if (del[x]) continue;
				mapping[x] = id;
				if (x != id) {
					swap(ops[x], ops[id]);
					swap(h[x], h[id]);
					swap(adj[x], adj[id]);
					swap(used[x], used[id]);
				}
				++id;
			}
			ops.resize(id);
			h.resize(id);
			adj.resize(id);
			used.resize(id);
		}

		for (auto&& x : f) {
			if (x != -1) {
				x = mapping[x];
			}
		}

		SREP(x, offset(), ops.size())
		{
			auto&& op = ops[x];
			op.b = mapping[op.b];
			op.c = mapping[op.c];
			op.d = mapping[op.d];
		}

		for (auto&& row : h) {
			for (auto&& x : row) {
				if (x == -1) continue;
				x = mapping[x];
			}
		}

		SREP(x, 0, ops.size())
		{
			auto&& row = adj[x];
			for (auto&& y : row) {
				if (y == -1) continue;
				y = mapping[y];
			}
		}
	}

	//void findCandidatesByB(short b, vector<Op>& ret)
	//{
	//	SREP(k, 0, 8)
	//	{
	//		short l = (k + 2) % 8;
	//		if (getBit(used[b], k)) continue;
	//		if (getBit(used[b], l)) continue;
	//		short c = adj[b][k];
	//		if (c < sent_size) continue;
	//		if (getBit(used[c], l)) continue;
	//		short d = adj[c][l];
	//		if (d < sent_size) continue;
	//		if (getBit(used[d], k ^ 4)) continue;
	//		short pa = ops[b].pa + ops[d].pa - ops[c].pa;

	//		if ((pa - ops[adj[b][l]].pa) * dsgn[l] >= 0) continue;
	//		if ((pa - ops[adj[d][k ^ 4]].pa) * dsgn[k ^ 4] >= 0) continue;
	//		ret.push_back({ pa, b, c, d, short(l ^ 4) });
	//	}
	//}

	void findCandidatesByC(short c, vector<Op>& ret)
	{
		SREP(k, 0, 8)
		{
			short l = (k + 2) % 8;
			if (getBit(used[c], k)) continue;
			if (getBit(used[c], l)) continue;
			short d = adj[c][k];
			if (d < sent_size) continue;
			if (getBit(used[d], l)) continue;
			short b = adj[c][l];
			if (b < sent_size) continue;
			if (getBit(used[b], k)) continue;
			short pa = ops[b].pa + ops[d].pa - ops[c].pa;

			if ((pa - ops[adj[b][k]].pa) * dsgn[k] >= 0) continue;
			if ((pa - ops[adj[d][l]].pa) * dsgn[l] >= 0) continue;
			ret.push_back({ pa, b, c, d, short(k ^ 4) });
		}
	}

	void findCandidates(short x, vector<Op>& ret)
	{
		SREP(k, 0, 8)
		{
			short l = (k + 2) % 8;
			if (getBit(used[x], k)) continue;
			if (getBit(used[x], l)) continue;

			short y = adj[x][k];
			short w = adj[x][l];

			{
				if (y < sent_size) goto endC;
				if (getBit(used[y], l)) goto endC;
				short z = adj[y][l];
				if (z < sent_size) goto endB;
				if (getBit(used[z], k ^ 4)) goto endB;
				short pa = ops[x].pa + ops[z].pa - ops[y].pa;

				if ((pa - ops[adj[x][l]].pa) * dsgn[l] >= 0) goto endB;
				if ((pa - ops[adj[z][k ^ 4]].pa) * dsgn[k ^ 4] >= 0) goto endB;
				ret.push_back({ pa, x, y, z, short(l ^ 4) });
			}
		endB:
			{
				if (w < sent_size) continue;
				if (getBit(used[w], k)) continue;
				short pa = ops[w].pa + ops[y].pa - ops[x].pa;

				if ((pa - ops[adj[w][k]].pa) * dsgn[k] >= 0) goto endC;
				if ((pa - ops[adj[y][l]].pa) * dsgn[l] >= 0) goto endC;
				ret.push_back({ pa, w, x, y, short(k ^ 4) });
			}
		endC:
			{
				if (w < sent_size) continue;
				if (getBit(used[w], k)) continue;
				short v = adj[w][k];
				if (v < sent_size) continue;
				if (getBit(used[v], l ^ 4)) continue;
				short pa = ops[v].pa + ops[x].pa - ops[w].pa;

				if ((pa - ops[adj[v][l ^ 4]].pa) * dsgn[l ^ 4] >= 0) continue;
				if ((pa - ops[adj[x][k]].pa) * dsgn[k] >= 0) continue;
				ret.push_back({ pa, v, w, x, l });
			}
		}
	}

	bool canAdd(const Op& op)
	{
		if (f[op.pa] != -1) return false;
		short k = op.k ^ 4;
		short l = (k + 2) % 8;
		if (getBit(used[op.b], k)) return false;
		if (getBit(used[op.b], l ^ 4)) return false;
		if (getBit(used[op.d], k ^ 4)) return false;
		if (getBit(used[op.d], l)) return false;
		if (op.d != adj[op.c][k]) return false;
		if (op.b != adj[op.c][l]) return false;
		short pa = op.pa;

		short bx = adj[op.b][k];
		short dx = adj[op.d][l];
		if ((pa - ops[bx].pa) * dsgn[k] >= 0) return false;
		if ((pa - ops[dx].pa) * dsgn[l] >= 0) return false;
		return true;
	}

	short offset() const
	{
		return sent_size + orig_size;
	}

	short ver(short i, short j)
	{
		return i * n + j;
	}

	bool isSentinel(short x)
	{
		return x < sent_size;
	}

	bool isOriginal(short x)
	{
		return x >= sent_size && x < sent_size + orig_size;
	}

	////////////////////////////////////////////////////////////////////////////////////

	Random rnd_;
	const double alpha_;

	bool isAccepted(double del, double tmp)
	{
		return del >= 0 || (del >= -8 * tmp && rnd_.rnd() < std::exp(del / tmp) * (1ll << 32));
	}

	vector<pair<int, Op>> cans;
	vector<Op> buffer;
	short aai = -1;
	short aaj = -1;
	void toMaximal()
	{
		auto getLen = [&](const Op& op) -> int
		{
			int length = (ops[op.b].pa - op.pa) / dp[op.k] + (ops[op.d].pa - op.pa) / dp[(op.k + 2) % 8];
			int mn = std::min({ op.pa, ops[op.b].pa, ops[op.c].pa, ops[op.d].pa });
			return rate < 0.3 ? rnd_.rnd(65536) :
				65536 * (length == 2 ? length + mn % 2 : length + 1)
				+ 512 * stable[op.pa]
				+ rnd_.rnd(65536);
		};

		cans.clear();

		if (aai == -1) {
			SREP(x, sent_size, ops.size())
			{
				findCandidatesByC(x, buffer);
			}
		} else {
			int sz = 15;
			SREP(ei, -sz, sz + 1)
			{
				int i = aai + ei;
				if (i < 0 || i >= n) continue;
				int w = min(sz - ei, sz + ei);
				REP(ej, -w, w + 1)
				{
					int j = aaj + ej;
					if (j < 0 || j >= n) continue;
					int x = f[ver(i, j)];
					if (x < sent_size) continue;
					findCandidatesByC(x, buffer);
				}
			}
		}

		if (buffer.empty()) return;
		int special = rnd_.rnd(buffer.size());

		REP(i, 0, buffer.size())
		{
			cans.emplace_back(i == special ? 0 : getLen(buffer[i]), buffer[i]);
		}
		buffer.clear();
		make_heap(cans.begin(), cans.end(), GComp());

		while (!cans.empty()) {
			pop_heap(cans.begin(), cans.end(), GComp());
			auto op = cans.back().second;
			cans.pop_back();

			if (canAdd(op)) {
				add(op);
				short x = ops.size() - 1;
				findCandidates(x, buffer);
				for (auto&& nop : buffer) {
					cans.emplace_back(getLen(nop), nop);
					push_heap(cans.begin(), cans.end(), GComp());
				}
				buffer.clear();
			}
		}
	}

	vector<short> xs;
	void step(double tmp)
	{
		mscr = scr;
		memcpy(mf.data(), f.data(), f.size() * sizeof(f[0]));
		mops.resize(ops.size());
		memcpy(mops.data() + offset(), ops.data() + offset(), (ops.size() - offset()) * sizeof(ops[0]));
		mh.resize(h.size());
		memcpy(mh.data(), h.data(), h.size() * sizeof(h[0]));
		madj.resize(adj.size());
		memcpy(madj.data(), adj.data(), adj.size() * sizeof(adj[0]));
		mused.resize(used.size());
		memcpy(mused.data(), used.data(), used.size() * sizeof(used[0]));

		xs.clear();
		if (rnd_.rnd(5) == 0) {
			aai = -1;
			aaj = -1;
			int iterations = rnd_.rnd(3) + 1;
			REP(z, 0, iterations)
			{
				if (ops.size() == offset()) break;
				short x = rnd_.rnd(ops.size() - offset()) + offset();
				xs.push_back(x);
				//aai = ops[x].pa / n;
				//aaj = ops[x].pa % n;
			}
		} else {
			if (ops.size() > offset()) {
				short x = rnd_.rnd(ops.size() - offset()) + offset();

				aai = ops[x].pa / n;
				aaj = ops[x].pa % n;

				if (rnd_.rnd(2)) {
					REP(i, -3, 4)
					{
						REP(j, -3, 4)
						{
							short p = ops[x].pa + i * n + j;
							if (p < 0 || p >= n * n) continue;
							if (f[p] >= offset()) {
								xs.push_back(f[p]);
							}
						}
					}
				} else {
					REP(i, -3, 4)
					{
						REP(j, -3, 4)
						{
							if (abs(i) + abs(j) > 3) continue;
							short p = ops[x].pa + i * n + j;
							if (p < 0 || p >= n * n) continue;
							if (f[p] >= offset()) {
								xs.push_back(f[p]);
							}
						}
					}
				}
			}
		}
		eraseAll(xs);
		toMaximal();

		int delta = scr - mscr;
		if (!isAccepted(delta, tmp)) {
			scr = mscr;
			swap(f, mf);
			swap(ops, mops);
			swap(h, mh);
			swap(adj, madj);
			swap(used, mused);
		}
	}

	void solve()
	{
		const int iterations = 1e5;
		const int limit = 4990000;
		const double tmp_start = 1000.0 * alpha_;
		const double tmp_finish = 50.0 * alpha_;
		double tmp = tmp_start;
		double ratio = 0.0;

		Timer timer;
		timer.set(limit);

		int z = 0;
#if ONLY_MY_ENVIR
		while (z < iterations) {
#else
		while (z % 16 != 0 || !timer.elapsed()) {
#endif
			++z;
			if (z % 16 == 0) {
#if ONLY_MY_ENVIR
				ratio = double(z) / iterations;
#else
				ratio = double(timer.getMicrosec()) / limit;
#endif
				tmp = tmp_start * std::pow(tmp_finish / tmp_start, ratio);
				if (z % 256 == 0) {
#ifdef _DEBUG
					cerr << tmp << ' ' << scr << endl;
#endif
				}
			}

			step(tmp);

			if (scr > bscr) {
				bscr = scr;
				bops = ops;
			}
		}
		cerr << z << endl;
		return;
		}

	vector<Rect> get() const
	{
		auto toPos = [this](int p) -> Pos
		{
			return Pos(p / n - 1, p % n - 1);
		};

		vector<Rect> ret;
		REP(x, offset(), bops.size())
		{
			ret.push_back(Rect(
				toPos(bops[x].pa),
				toPos(bops[bops[x].b].pa),
				toPos(bops[bops[x].c].pa),
				toPos(bops[bops[x].d].pa)
			));
		}
		return ret;
	}
};

Output solve(const Input& in)
{
	Helper helper(in);
	helper.solve();
	//printScore(in, best.scr);
	return helper.get();
}

#ifndef ONLY_MY_ENVIR_LOCAL
int main()
{
	auto in = input(cin);
	auto out = solve(in);
	output(cout, out);
	return 0;
}
#endif

// Included Files: 
//   * Timer.h
//   * Timer.cpp
//   * Random.h
//   * Random.cpp
//   * Main.h
//   * Main.cpp
