#pragma GCC target ("avx2")
#pragma GCC optimize ("O3,omit-frame-pointer,inline")
#pragma GCC optimize ("unroll-loops")

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

	template <class Container>
	void shuffle(Container& arr);

	template <class T>
	std::vector<T> generateVector(int size, std::uint32_t max);
	template <class T>
	std::vector<T> generateVector(int size, std::int32_t min, std::int32_t max);
};

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

void YesNo(bool b) { cout << (b ? "Yes" : "No") << endl; }
void YESNO(bool b) { cout << (b ? "YES" : "NO") << endl; }
template <class T, class U> istream& operator>>(istream& ist, pair<T, U>& right) { return ist >> right.first >> right.second; }
template <class T, class U> ostream& operator<<(ostream& ost, const pair<T, U>& right) { return ost << right.first << ' ' << right.second; }
template <class T, class TCompatible, size_t N> void Fill(T(&dest)[N], const TCompatible& val) { fill(dest, dest + N, val); }
template <class T, class TCompatible, size_t M, size_t N> void Fill(T(&dest)[M][N], const TCompatible& val) { for (int i = 0; i < M; ++i) Fill(dest[i], val); }
template <class T> T Next() { T buf; cin >> buf; return buf; }
istream& Ignore(istream& ist) { string s; ist >> s; return ist; }
bool Inside(int i, int j, int h, int w) { return i >= 0 && i < h&& j >= 0 && j < w; }
void pL() { std::cout << std::endl; }
template <class Head> void pL(Head&& head) { std::cout << head << std::endl; }
template <class Head, class... Tail> void pL(Head&& head, Tail&&... tail) { std::cout << head << ' '; pL(std::forward<Tail>(tail)...); }

#ifdef __GNUC__
using LLL = __int128;
istream& operator>>(istream& ist, __int128& val) { LL tmp; ist >> tmp; val = tmp; return ist; }
ostream& operator<<(ostream& ost, __int128 val) { LL tmp = val; ost << tmp; return ost; }
#else
using LLL = LL;
#endif

#endif
#ifdef ONLY_MY_ENVIR
#include "Main.h"
#endif

#ifdef ONLY_MY_ENVIR
std::ifstream input_stream("ain.txt");
std::ofstream output_stream("aout.txt");
#define cin input_stream
#define cout output_stream
#endif

using Pos = pair<int, int>;
constexpr int n = 1000;
constexpr int m = 50;
Pos p[2 * n + 1];
int d[2 * n + 1][2 * n + 1];

int dist(const Pos& a, const Pos& b) {
	return abs(b.first - a.first) + abs(b.second - a.second);
}

void input() {
	REP(i, 0, n) cin >> p[2 * i] >> p[2 * i + 1];
	p[2 * n] = Pos(400, 400);
	REP(i, 0, 2 * n + 1) {
		REP(j, 0, 2 * n + 1) {
			d[i][j] = dist(p[i], p[j]);
		}
	}
}

void outputOrders(const vector<int>& v) {
	cout << v.size();
	REP(i, 0, v.size()) {
		cout << ' ' << v[i] + 1;
	}
	cout << endl;
}

void outputPoints(const vector<int>& is) {
	cout << is.size();
	for (int i : is) {
		cout << ' ' << p[i];
	}
	cout << endl;
}

constexpr int len = 2 * m;
class Helper {
	vector<uint> xs;
	vector<uint> xtoi;
	vector<bool> tmp;
	int scr;
	int pscr;

	Random rnd;

public:
	void init() {
		xs.push_back(2 * n);
		REP(s, 0, m) xs.push_back(2 * s);
		REP(s, 0, m) xs.push_back(2 * s + 1);
		xs.push_back(2 * n);

		xtoi.resize(2 * n, -1);
		REP(i, 1, len + 1) {
			xtoi[xs[i]] = i;
		}

		tmp.resize(n);
		scr = calc();
	}

	bool canReverse(int i, int j) {
		tmp.assign(n, false);
		REP(k, i, j) {
			uint s = xs[k] / 2;
			if (tmp[s]) return false;
			tmp[s] = true;
		}
		return true;
	}

	int estChange(int i, int j, uint t) {
		assert(i < j);
		
		uint x = 2 * t;
		uint y = 2 * t + 1;
		int sum = 0;
		if (i + 1 == j) {
			sum -= d[xs[i - 1]][xs[i]];
			sum -= d[xs[i]][xs[j]];
			sum -= d[xs[j]][xs[j + 1]];
			sum += d[xs[i - 1]][x];
			sum += d[x][y];
			sum += d[y][xs[j + 1]];
		} else {
			sum -= d[xs[i - 1]][xs[i]];
			sum -= d[xs[i]][xs[i + 1]];
			sum -= d[xs[j - 1]][xs[j]];
			sum -= d[xs[j]][xs[j + 1]];
			sum += d[xs[i - 1]][x];
			sum += d[x][xs[i + 1]];
			sum += d[xs[j - 1]][y];
			sum += d[y][xs[j + 1]];
		}
		return sum;
	}

	int estReverse(int i, int j) {
		assert(i < j);

		int w = xs[i - 1];
		int x = xs[i];
		int y = xs[j - 1];
		int z = xs[j];

		int sum = 0;
		sum -= d[w][x];
		sum -= d[y][z];
		sum += d[w][y];
		sum += d[x][z];
		return sum;
	}

	int estMoveRight(int i, int j) {
		assert(j > i);
		int sum = 0;
		sum -= d[xs[i - 1]][xs[i]];
		sum -= d[xs[i]][xs[i + 1]];
		sum -= d[xs[j]][xs[j + 1]];
		sum += d[xs[j]][xs[i]];
		sum += d[xs[i]][xs[j + 1]];
		sum += d[xs[i - 1]][xs[i + 1]];
		return sum;
	}

	int estMoveLeft(int i, int j) {
		assert(j < i);
		int sum = 0;
		sum -= d[xs[i - 1]][xs[i]];
		sum -= d[xs[i]][xs[i + 1]];
		sum -= d[xs[j - 1]][xs[j]];
		sum += d[xs[j - 1]][xs[i]];
		sum += d[xs[i]][xs[j]];
		sum += d[xs[i - 1]][xs[i + 1]];
		return sum;
	}

	int estMove(int i, int j) {
		if (i == j) return 0;
		if (i < j) {
			return estMoveRight(i, j);
		} else if (i > j) {
			return estMoveLeft(i, j);
		}
	}

	void stepMove(int i, int j) {
		if (i == j) return;
		if (i < j) {
			rotate(xs.begin() + i, xs.begin() + i + 1, xs.begin() + j + 1);
			REP(k, i, j + 1) {
				xtoi[xs[k]] = k;
			}
		} else {
			rotate(xs.begin() + j, xs.begin() + i, xs.begin() + i + 1);
			REP(k, j, i + 1) {
				xtoi[xs[k]] = k;
			}
		}
	}

	vector<uint> pxs;
	int estImprove(int ai, int aj, uint t, int& ni, int& nj) {
		assert(ai < aj);
		int sum = 0;
		if (ai + 1 == aj) {
			sum -= d[xs[ai - 1]][xs[ai]];
			sum -= d[xs[ai]][xs[aj]];
			sum -= d[xs[aj]][xs[aj + 1]];
			sum += d[xs[ai - 1]][xs[aj + 1]];
		} else {
			sum -= d[xs[ai - 1]][xs[ai]];
			sum -= d[xs[ai]][xs[ai + 1]];
			sum -= d[xs[aj - 1]][xs[aj]];
			sum -= d[xs[aj]][xs[aj + 1]];
			sum += d[xs[ai - 1]][xs[ai + 1]];
			sum += d[xs[aj - 1]][xs[aj + 1]];
		}

		pxs = xs;
		pxs.erase(pxs.begin() + aj);
		pxs.erase(pxs.begin() + ai);

		int mn = INF;
		uint nx = 2 * t;
		uint ny = 2 * t + 1;
		
		int locmn = INF;
		int locmni = -1;
		REP(i, 1, len - 2 + 2) {
			int ysum = 0;
			ysum -= d[pxs[i - 1]][pxs[i]];
			ysum += d[pxs[i - 1]][ny];
			ysum += d[ny][pxs[i]];

			int sum = ysum + locmn;
			if (sum < mn) {
				mn = sum;
				ni = locmni;
				nj = i;
			}

			int xsum = 0;
			xsum -= d[pxs[i - 1]][pxs[i]];
			xsum += d[pxs[i - 1]][nx];
			xsum += d[nx][pxs[i]];
			
			if (xsum < locmn) {
				locmn = xsum;
				locmni = i;
			}
		}
		return sum + mn;
	}

	void step(double tmp) {
		int type = rnd.rnd(4);
		int delta = 0;
		if (type == 0) {
			int i = rnd.rnd(len) + 1;
			int j = xtoi[xs[i] ^ 1];
			if (i > j) swap(i, j);
			uint s = xs[i] / 2;

			uint t = 0;
			do {
				t = rnd.rnd(n);
			} while (xtoi[2 * t] != -1);

			delta = estChange(i, j, t);
			if (!accepts(delta, tmp)) return;

			xtoi[2 * s] = -1;
			xtoi[2 * s + 1] = -1;
			xs[i] = 2 * t;
			xs[j] = 2 * t + 1;
			xtoi[2 * t] = i;
			xtoi[2 * t + 1] = j;
		} else if (type == 1) {
			int i = rnd.rnd(len) + 1;
			int j = rnd.rnd(len) + 1;
			if (i > j) swap(i, j);
			++j;

			delta = estReverse(i, j);
			if (!accepts(delta, tmp)) return;
			if (!canReverse(i, j)) return;
			reverse(xs.begin() + i, xs.begin() + j);
			REP(k, i, j) {
				xtoi[xs[k]] = k;
			}
		} else if (type == 2) {
			int i = rnd.rnd(len) + 1;
			int j = xtoi[xs[i] ^ 1];
			if (i > j) swap(i, j);
			//if (j == len) return;
			
			
			if (rnd.rnd(2)) {
				// move i
				int k = rnd.rnd(len) + 1;
				if (k >= j) return;
				delta = estMove(i, k);
				if (!accepts(delta, tmp)) return;
				stepMove(i, k);
			} else {
				// move j
				int k = rnd.rnd(len) + 1;
				if (k <= i) return;
				delta = estMove(j, k);
				if (!accepts(delta, tmp)) return;
				stepMove(j, k);
			}

			//int nj = j + rnd.rnd(len - j) + 2;

			//delta = estMoveRight(j, nj);
			//if (!accepts(delta, tmp)) return;

			//rotate(xs.begin() + j, xs.begin() + j + 1, xs.begin() + nj);
			//REP(k, j, nj) {
			//	xtoi[xs[k]] = k;
			//}
		} else {
			int i = rnd.rnd(len) + 1;
			int j = xtoi[xs[i] ^ 1];
			if (i > j) swap(i, j);
			uint s = xs[i] / 2;

			uint t = 0;
			do {
				t = rnd.rnd(n);
			} while (xtoi[2 * t] != -1);

			int ni = -1;
			int nj = -1;
			delta = estImprove(i, j, t, ni, nj);
			if (!accepts(delta, tmp)) return;

			xtoi[2 * s] = -1;
			xtoi[2 * s + 1] = -1;
			xs.erase(xs.begin() + j);
			xs.erase(xs.begin() + i);
			xs.insert(xs.begin() + nj, 2 * t + 1);
			xs.insert(xs.begin() + ni, 2 * t);
			REP(k, 1, len + 1) {
				xtoi[xs[k]] = k;
			}
		}

		scr += delta;
	}

	bool accepts(int delta, double tmp) {
		return delta <= 0 || delta <= 8 * tmp && rnd.rnd() < std::exp(-delta / tmp) * (1ll << 32);
	}

	int calc() {
		int sum = 0;
		REP(i, 1, len + 2) {
			sum += d[xs[i - 1]][xs[i]];
		}
		return sum;
	}

	const vector<uint>& get() const {
		return xs;
	}
	
	int getScore() const {
		return scr;
	}
};

int main() {
	const double time_limit = 1990000;
	Timer timer;
	timer.set(time_limit);

	input();

	Helper helper;
	helper.init();

	const double tmp_start = 70;
	const double tmp_finish = 2;
	double tmp = tmp_start;

	const int iterations = 1e7;
	int z = 0;
	Random rnd;
#if ONLY_MY_ENVIR
	while (z < iterations) {
#else
	while (z % 256 != 0 || !timer.elapsed()) {
#endif
		++z;
		if (z % 32768 == 0) {
#if ONLY_MY_ENVIR
			double ratio = double(z) / iterations;
#else
			double ratio = double(timer.getMicrosec()) / time_limit;
#endif
			tmp = tmp_start * std::pow(tmp_finish / tmp_start, ratio);
#if ONLY_MY_ENVIR
			cerr << tmp << ' ' << helper.getScore() << endl;
#endif
		}

		helper.step(tmp);
	}
	cerr << z << endl;
	
	auto xs = helper.get();
	cerr << helper.getScore() << endl;
	set<int> ss;
	for (int x : xs) {
		ss.insert(x / 2);
	}
	ss.erase(1000);
	outputOrders(vector<int>(ss.begin(), ss.end()));
	xs.insert(xs.begin(), 2 * n);
	xs.push_back(2 * n);
	outputPoints(vector<int>(xs.begin(), xs.end()));
	return 0;
}

// Included Files: 
//   * Timer.h
//   * Timer.cpp
//   * Random.h
//   * Random.cpp
//   * Main.h
//   * Main.cpp
