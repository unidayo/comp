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

#ifndef __ACCUMULATOR_H__
#define __ACCUMULATOR_H__

/* updated: 2020-08-02 */

#include <cassert>
#include <vector>
#include <algorithm>

template <class T>
class Accumulator2D {
	int height_ = 0;
	int width_ = 0;
	std::vector<std::vector<T>> sum_;

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
	height_ = height;
	width_ = width;
	sum_.assign(height_ + 1, std::vector<T>(width_ + 1, T()));

	is_calculated = false;
}

template<class T>
void Accumulator2D<T>::initialize(const std::vector<std::vector<T>>& arr) {
	int height = arr.size();
	int width = height == 0 ? 0 : arr.front().size();
	initialize(height, width);

	/* 累積和をとるとき in_-place にできるように1つ添字をずらしておく */
	for (int i = 0; i < height_; ++i) {
		for (int j = 0; j < width_; ++j) {
			sum_[i + 1][j + 1] = arr[i][j];
		}
	}

	is_calculated = false;
}

/* 要素 (i, j) の値を increment だけ増加させる (累積和の計算前) O(1) */
template<class T>
void Accumulator2D<T>::add(int i, int j, const T& increment) {
	assert(i >= 0 && i < height_);
	assert(j >= 0 && j < width_);
	sum_[i + 1][j + 1] += increment;
}

/* 累積和を計算する O(H * W) */
template<class T>
void Accumulator2D<T>::execute() {
	for (int i = 0; i < height_; ++i) {
		for (int j = 0; j < width_; ++j) {
			sum_[i + 1][j + 1] += sum_[i][j + 1] + sum_[i + 1][j] - sum_[i][j];
		}
	}

	is_calculated = true;
}

/* 領域 [0, i) * [0, j) 内の要素の和を得る O(1) */
template <class T>
T Accumulator2D<T>::get(int i, int j) const {
	assert(is_calculated);

	int new_i = std::max(0, std::min(height_, i));
	int new_j = std::max(0, std::min(width_, j));
	return sum_[new_i][new_j];
}

/* 領域 [ai, bi) * [aj, bj) 内の要素の和を得る O(1) */
template <class T>
T Accumulator2D<T>::get(int ai, int aj, int bi, int bj) const {
	assert(is_calculated);

	return get(ai, aj) - get(ai, bj) - get(bi, aj) + get(bi, bj);
}

#endif

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

std::ifstream input_stream("ain.txt");
std::ofstream output_stream("aout.txt");
#define cin input_stream
#define cout output_stream
#endif

using Pos = pair<int, int>;
using Line = pair<Pos, Pos>;

constexpr int rad = 10000;

int n, kmax;
vector<int> a;
vector<int> amod;
vector<Pos> ps;
vector<Line> ans;

Line hLine(int x) {
    return { { x - 1, -rad }, { x, rad } };
}
Line vLine(int y) {
    return { { -rad, y - 1 }, { rad, y } };
}

Accumulator2D<short> acc;

class Helper {
    Random rnd_;
    double score_ = 0;
    double pscore_ = 0;
    vector<int> xs_[2];
    vector<int> histo_;
    vector<pair<int, int>> delta_;

public:
    Helper() {
        int need = 0;
        for (int cnt : a) {
            need += cnt;
        }

        int d[2];
        REP(k, 2, 100) {
            d[0] = k;
            d[1] = need * 1.2 / k;
            if (d[0] + d[1] - 2 <= 100) {
                --d[0];
                --d[1];
                break;
            }
        }
        REP(k, 0, 2) {
            xs_[k].push_back(0);
            REP(i, 0, d[k]) {
                xs_[k].push_back(rad * (2 * i + 1) / d[k]);
            }
            xs_[k].push_back(2 * rad);
        }

        initHisto();
        score_ = calc();
        delta_.reserve(6);
    }

    vector<Line> get() {
        vector<Line> ret;
        REP(i, 1, xs_[0].size() - 1) {
            ret.push_back(hLine(xs_[0][i] - rad));
        }
        REP(i, 1, xs_[1].size() - 1) {
            ret.push_back(vLine(xs_[1][i] - rad));
        }
        return ret;
    }

    void solve() {
        const int iterations = 2e6;
        const int limit = 2000000;
        const double tmp_start = 100;
        const double tmp_finish = 1;
        double tmp = tmp_start;

        Timer timer;
        timer.set(limit);

        int z = 0;
#if ONLY_MY_ENVIR
        while (z < iterations) {
#else
        while (z % 256 != 0 || !timer.elapsed()) {
#endif
            ++z;
            if (z % 256 == 0) {
#if ONLY_MY_ENVIR
                double ratio = double(z) / iterations;
#else
                double ratio = double(timer.getMicrosec()) / limit;
#endif
                tmp = tmp_start * std::pow(tmp_finish / tmp_start, ratio);
                if (z % 16384 == 0) {
                    cerr << tmp << ' ' << score_ << endl;
                }
            }
            double delta = step();
            if (!isAccepted(delta, tmp)) {
                undo();
            }
        }
        cerr << z << endl;
        return;
    }

    bool isAccepted(double del, double tmp) {
        return del <= 0 || (del <= 8 * tmp && rnd_.rnd() < std::exp(-del / tmp) * (1ll << 32));
    }

    int mt_ = 0;
    int mi_ = 0;
    int mval_ = 0;
    int mni_ = 0;
    bool gap_ = false;
    double step() {
        pscore_ = score_;
        delta_.clear();

        int t;
        int i;
        int xlen = xs_[0].size() - 2;
        int ylen = xs_[1].size() - 2;
        int num = rnd_.rnd(xlen + ylen);
        if (num < xlen) {
            t = 0;
            i = num + 1;
        } else {
            t = 1;
            i = num - xlen + 1;
        }

        mt_ = t;
        auto&& xs = xs_[t];

        int nx;
        if (t && rnd_.rnd(2)) {
            nx = 1 + rnd_.rnd(2 * rad - 1);
        } else {
            int d = (rnd_.rnd(2) ? 1 : -1) * (rnd_.rnd(100) + 1);
            nx = min(max(xs[i] + d, 1), 2 * rad - 1);
        }

        mi_ = i;
        mval_ = xs[i];
        if (nx >= xs[i - 1] && nx <= xs[i + 1]) {
            gap_ = false;
            move(t, i, nx);
        } else {
            gap_ = true;
            erase(t, i);
            int ni = lower_bound(xs.begin(), xs.end(), nx) - xs.begin();
            insert(t, ni, nx);
            mni_ = ni;
        }
        
        score_ = calc();
        return score_ - pscore_;
    }

    void move(int k, int i, int nbx) {
        if (k == 0) {
            int ax = xs_[k][i - 1];
            int bx = xs_[k][i];
            int cx = xs_[k][i + 1];
            REP(j, 1, xs_[!k].size()) {
                int ay = xs_[!k][j - 1];
                int by = xs_[!k][j];
                int cntl = acc.get(ax, ay, bx, by);
                int cntr = acc.get(bx, ay, cx, by);
                int cntnl = acc.get(ax, ay, nbx, by);
                int cntnr = acc.get(nbx, ay, cx, by);
                histo_[cntl] -= cntl;
                histo_[cntr] -= cntr;
                histo_[cntnl] += cntnl;
                histo_[cntnr] += cntnr;
                delta_.emplace_back(cntl, -cntl);
                delta_.emplace_back(cntr, -cntr);
                delta_.emplace_back(cntnl, +cntnl);
                delta_.emplace_back(cntnr, +cntnr);
            }
        } else {
            int ay = xs_[k][i - 1];
            int by = xs_[k][i];
            int cy = xs_[k][i + 1];
            REP(j, 1, xs_[!k].size()) {
                int ax = xs_[!k][j - 1];
                int bx = xs_[!k][j];
                int cntl = acc.get(ax, ay, bx, by);
                int cntr = acc.get(ax, by, bx, cy);
                int cntnl = acc.get(ax, ay, bx, nbx);
                int cntnr = acc.get(ax, nbx, bx, cy);
                histo_[cntl] -= cntl;
                histo_[cntr] -= cntr;
                histo_[cntnl] += cntnl;
                histo_[cntnr] += cntnr;
                delta_.emplace_back(cntl, -cntl);
                delta_.emplace_back(cntr, -cntr);
                delta_.emplace_back(cntnl, +cntnl);
                delta_.emplace_back(cntnr, +cntnr);
            }
        }
        xs_[k][i] = nbx;
    }

    void insert(int k, int i, int bx) {
        if (k == 0) {
            int ax = xs_[k][i - 1];
            int cx = xs_[k][i];
            REP(j, 1, xs_[!k].size()) {
                int ay = xs_[!k][j - 1];
                int by = xs_[!k][j];
                int cntl = acc.get(ax, ay, bx, by);
                int cntr = acc.get(bx, ay, cx, by);
                int cntc = cntl + cntr;
                histo_[cntl] += cntl;
                histo_[cntr] += cntr;
                histo_[cntc] -= cntc;
                delta_.emplace_back(cntl, cntl);
                delta_.emplace_back(cntr, cntr);
                delta_.emplace_back(cntc, -cntc);
            }
        } else {
            int by = bx;
            int ay = xs_[k][i - 1];
            int cy = xs_[k][i];
            REP(j, 1, xs_[!k].size()) {
                int ax = xs_[!k][j - 1];
                int bx = xs_[!k][j];
                int cntl = acc.get(ax, ay, bx, by);
                int cntr = acc.get(ax, by, bx, cy);
                int cntc = cntl + cntr;
                histo_[cntl] += cntl;
                histo_[cntr] += cntr;
                histo_[cntc] -= cntc;
                delta_.emplace_back(cntl, cntl);
                delta_.emplace_back(cntr, cntr);
                delta_.emplace_back(cntc, -cntc);
            }
        }

        xs_[k].insert(xs_[k].begin() + i, bx);
    }

    void erase(int k, int i) {
        if (k == 0) {
            int ax = xs_[k][i - 1];
            int bx = xs_[k][i];
            int cx = xs_[k][i + 1];
            REP(j, 1, xs_[!k].size()) {
                int ay = xs_[!k][j - 1];
                int by = xs_[!k][j];
                int cntl = acc.get(ax, ay, bx, by);
                int cntr = acc.get(bx, ay, cx, by);
                int cntc = cntl + cntr;
                histo_[cntl] -= cntl;
                histo_[cntr] -= cntr;
                histo_[cntc] += cntc;
                delta_.emplace_back(cntl, -cntl);
                delta_.emplace_back(cntr, -cntr);
                delta_.emplace_back(cntc, +cntc);
            }
        } else {
            int ay = xs_[k][i - 1];
            int by = xs_[k][i];
            int cy = xs_[k][i + 1];
            REP(j, 1, xs_[!k].size()) {
                int ax = xs_[!k][j - 1];
                int bx = xs_[!k][j];
                int cntl = acc.get(ax, ay, bx, by);
                int cntr = acc.get(ax, by, bx, cy);
                int cntc = cntl + cntr;
                histo_[cntl] -= cntl;
                histo_[cntr] -= cntr;
                histo_[cntc] += cntc;
                delta_.emplace_back(cntl, -cntl);
                delta_.emplace_back(cntr, -cntr);
                delta_.emplace_back(cntc, +cntc);
            }
        }

        xs_[k].erase(xs_[k].begin() + i);
    }

    void undo() {
        if (gap_) {
            xs_[mt_].erase(xs_[mt_].begin() + mni_);
            xs_[mt_].insert(xs_[mt_].begin() + mi_, mval_);
        } else {
            xs_[mt_][mi_] = mval_;
        }

        for (auto&& p : delta_) {
            histo_[p.first] -= p.second;
        }
        swap(score_, pscore_);
    }

    double calc() {
        int sum = 0;
        REP(i, 1, 11) {
            int d = (amod[i] - histo_[i]) / i;
            sum += d * d;
        }
        return sum + dist(amod, histo_);
    }

    int dist(const vector<int>& a, const vector<int>& b) {
        int i = 0; 
        int j = 0;
        int x = a[i];
        int y = b[j];
        int sum = 0;
        while (true) {
            int mn = min(x, y);
            x -= mn;
            y -= mn;
            sum += mn * (j - i) * (j - i);
            if (x == 0) {
                ++i;
                if (i >= a.size()) break;
                x = a[i];
            }
            if (y == 0) {
                ++j;
                if (j >= b.size()) break;
                y = b[j];
            }
        }
        return sum;
    }

    void initHisto() {
        histo_.resize(n + 1);
        auto&& xs = xs_[0];
        auto&& ys = xs_[1];
        REP(i, 1, xs.size()) {
            int ax = xs[i - 1];
            int bx = xs[i];
            REP(j, 1, ys.size()) {
                int ay = ys[j - 1];
                int by = ys[j];
                int cnt = acc.get(ax, ay, bx, by);

                if (cnt == 0) continue;
                histo_[cnt] += cnt;
            }
        }
    }
};

void solve() {
    Helper helper;
    helper.solve();
    ans = helper.get();
}

int main() {
    cin >> n >> kmax;
    a.resize(10);
    amod.resize(11);
    REP(i, 0, 10) cin >> a[i];
    REP(i, 0, 10) amod[i + 1] = a[i] * (i + 1);
    REP(i, 0, n) {
        int x, y;
        cin >> x >> y;
        ps.emplace_back(x, y);
    }

    acc = Accumulator2D<short>(2 * rad + 1, 2 * rad + 1);
    REP(i, 0, n) {
        int x = ps[i].first + rad;
        int y = ps[i].second + rad;
        acc.add(x, y, 1);
    }
    acc.execute();

    solve();
        
    cout << ans.size() << endl;
    for (auto&& l : ans) {
        auto&& a = l.first;
        auto&& b = l.second;
        cout <<
            a.first << ' ' <<
            a.second << ' ' <<
            b.first << ' ' <<
            b.second << endl;
    }
    return 0;
}

// Included Files: 
//   * Timer.h
//   * Timer.cpp
//   * Random.h
//   * Random.cpp
//   * Accumulator.h
//   * Main.h
//   * Main.cpp
