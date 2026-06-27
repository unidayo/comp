#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <mutex>
#include <thread>

#ifdef ONLINE_JUDGE
#define NDEBUG
#else
#undef NDEBUG
#endif
#include <cassert>

using namespace std;
using uc = unsigned char;
using us = unsigned short;
using ull = unsigned long long;
using ll = long long;
using pii = pair<int, int>;
using pdi = pair<double, int>;
#define len(c) (int) (c.size())
#define array_len(a) (int) (sizeof(a) / sizeof(decltype(a[0])))
#define rep(i, from, until) for (int i = (from); i < (until); i++)
#define repr(i, from, until) for (int i = (until) -1; i >= (from); i--)
#define rep0(i, until) rep(i, 0, until)
#define rep0r(i, until) repr(i, 0, until)

template <class T>
T mix(T a, T b, double ratio) {
	return a + ratio * (b - a);
}

#ifdef _MSC_VER
#include <intrin.h>
#define popcount32(a) ((int) __popcnt(a))
#define popcount64(a) ((int) __popcnt64(a))
#define clz32(a) ((int) __lzcnt(a))
#define clz64(a) ((int) __lzcnt64(a))
inline int ctz32(int a) {
	int i;
	_BitScanForward((unsigned long*) &i, a);
	return i;
}
inline int ctz64(ull a) {
	int i;
	_BitScanForward64((unsigned long*) &i, a);
	return i;
}
#else
#define popcount32(a) ((int) __builtin_popcount(a))
#define popcount64(a) ((int) __builtin_popcountll(a))
#define clz32(a) ((int) __builtin_clz(a))
#define clz64(a) ((int) __builtin_clzll(a))
#define ctz32(a) ((int) __builtin_ctz(a))
#define ctz64(a) ((int) __builtin_ctzll(a))
#endif

class Rnd {
public:
	Rnd() : y(135272729) {
	}

	int getSeed() {
		return y;
	}

	void setSeed(int y) {
		this->y = y;
	}

	int nextInt() {
		return next();
	}

	int nextInt(int n) {
		return next() % n;
	}

	int nextInt(int min, int max) {
		return min + nextInt(max - min + 1);
	}

	double nextFloat() {
		return (double) next() / 0x80000000;
	}

	double nextNormal() {
		return sqrt(-2 * log(nextFloat())) * cos(6.283185307179586 * nextFloat());
	}

	double nextFloat(double min, double max) {
		return min + nextFloat() * (max - min);
	}

private:
	int y;

	int next() {
		y = y ^ (y << 13);
		y = y ^ (y >> 17);
		return (y = y ^ (y << 5)) & 0x7fffffff;
	}
};

double timeScale = 1.0;

// return in ms
int timer(bool reset = false) {
	static auto st = chrono::system_clock::now();
	if (reset) {
		st = chrono::system_clock::now();
		return 0;
	} else {
		auto en = chrono::system_clock::now();
		int elapsedTotal = (int) chrono::duration_cast<chrono::milliseconds>(en - st).count();
		return (int) round(elapsedTotal / timeScale);
	}
}

bool debug = true;

template <class T>
auto tracen(T&& t) -> decltype(cerr << t, void()) {
	if (!debug)
		return;
	cerr << t;
}

template <class T>
auto tracen(T&& t) -> decltype(t.str(), void()) {
	if (!debug)
		return;
	cerr << t.str();
}

template <class T, typename U = enable_if_t<!is_same_v<T, string> && !is_same_v<T, string&>>>
auto tracen(T&& t) -> decltype(t.begin(), t.end(), void()) {
	if (!debug)
		return;
	bool first = true;
	auto from = t.begin();
	auto until = t.end();
	while (from != until) {
		if (first) {
			first = false;
		} else {
			cerr << ", ";
		}
		tracen(*from);
		from++;
	}
}

template <class T1, class T2, class... Rest>
void tracen(T1&& t1, T2&& t2, Rest&&... rest) {
	if (!debug)
		return;
	tracen(forward<T1>(t1));
	tracen(forward<T2>(t2), forward<Rest>(rest)...);
}

void trace() {
	cerr << endl;
}

template <class T, class... Rest>
void trace(T&& t, Rest&&... rest) {
	if (!debug)
		return;
	tracen(forward<T>(t), forward<Rest>(rest)...);
	cerr << endl;
}

template <class Iter>
void tracein(Iter from, Iter until) {
	if (!debug)
		return;
	bool first = true;
	while (from != until) {
		if (first) {
			first = false;
		} else {
			cerr << ", ";
		}
		tracen(*from);
		from++;
	}
}

template <class T, class Iter>
void tracein(T&& header, Iter from, Iter until) {
	if (!debug)
		return;
	tracen(header, ": ");
	tracein(from, until);
}

template <class Iter>
void tracei(Iter from, Iter until) {
	if (!debug)
		return;
	tracein(from, until);
	cerr << endl;
}

template <class T, class Iter>
void tracei(T&& header, Iter from, Iter until) {
	if (!debug)
		return;
	tracein(header, from, until);
	cerr << endl;
}

void sleepUntil(int until) {
	volatile unsigned int a = 0;
	while (timer() < until)
		a++;
}

void waitForever() {
	while (true)
		getchar();
}

bool writeText(string fileName, string text, bool append = false) {
	ofstream fout;
	fout.open(fileName, append ? ios::out | ios::app : ios::out);
	if (!fout) {
		return false;
	}
	fout << text;
	fout.close();
	return true;
}

pair<string, bool> readText(string fileName) {
	ifstream fin;
	fin.open(fileName, ios::in);
	if (!fin) {
		return make_pair("", false);
	}
	string res;
	string line;
	while (getline(fin, line)) {
		res += line;
		res += "\n";
	}
	return make_pair(res, true);
}

class Movie {
private:
	bool isOpen = false;
	ofstream out;

	void writeInstruction(string it, vector<double> argf, vector<string> args) {
		writeString(it);
		for (double f : argf) {
			writeFloat(f);
		}
		for (auto& s : args) {
			writeString(s);
		}
	}

	void writeFloat(double a) {
		float f = (float) a;
		out.write((char*) &f, 4);
	}

	void writeString(string str) {
		out.write(str.c_str(), str.length() + 1);
	}

public:
	static constexpr int LEFT = 0;
	static constexpr int CENTER = 1;
	static constexpr int RIGHT = 2;

	Movie() {
	}

	void beginMovie(string file) {
		assert(!isOpen);
		isOpen = true;
		out = ofstream(file, ios_base::out | ios_base::binary);
		out.write("smv", 3);
	}

	void endMovie() {
		assert(isOpen);
		isOpen = false;
		out.close();
	}

	void fill(double rgb, double a = 1.0) {
		fill(rgb, rgb, rgb, a);
	}

	void fill(double r, double g, double b, double a = 1.0) {
		writeInstruction("f", {r, g, b, a}, {});
	}

	void stroke(double rgb, double a = 1.0) {
		stroke(rgb, rgb, rgb, a);
	}

	void stroke(double r, double g, double b, double a = 1.0) {
		writeInstruction("s", {r, g, b, a}, {});
	}

	void noFill() {
		writeInstruction("nf", {}, {});
	}

	void noStroke() {
		writeInstruction("ns", {}, {});
	}

	void tooltip(string text) {
		writeInstruction("tt", {}, {text});
	}

	void noTooltip() {
		writeInstruction("nt", {}, {});
	}

	void strokeWeight(double weight) {
		writeInstruction("sw", {weight}, {});
	}

	void textSize(double size) {
		writeInstruction("ts", {size}, {});
	}

	void textAlign(int align) {
		writeInstruction("ta", {(double) align}, {});
	}

	void rect(double x, double y, double w, double h) {
		writeInstruction("r", {x, y, w, h}, {});
	}

	void circle(double x, double y, double r) {
		writeInstruction("c", {x, y, r}, {});
	}

	void ellipse(double x, double y, double rx, double ry) {
		writeInstruction("e", {x, y, rx, ry}, {});
	}

	void line(double x1, double y1, double x2, double y2) {
		writeInstruction("l", {x1, y1, x2, y2}, {});
	}

	void text(string str, double x, double y) {
		writeInstruction("t", {x, y}, {str});
	}

	void translate(double tx, double ty) {
		writeInstruction("tr", {tx, ty}, {});
	}

	void rotate(double ang) {
		writeInstruction("ro", {ang}, {});
	}

	void scale(double s) {
		scale(s, s);
	}

	void scale(double sx, double sy) {
		writeInstruction("sc", {sx, sy}, {});
	}

	void push() {
		writeInstruction("pu", {}, {});
	}

	void pop() {
		writeInstruction("po", {}, {});
	}

	void clearFrame() {
		writeInstruction("cl", {}, {});
	}

	void endFrame() {
		writeInstruction("n", {}, {});
		out.flush(); // flush at the end of each frame
	}

	void target(string name) {
		writeInstruction("tg", {}, {name});
	}
};

Movie mov;

bool isLocal = false;
constexpr bool USE_TL = true;
constexpr int ITER = USE_TL ? 10000 : 1;
constexpr int TL1 = USE_TL ? 300 : 10000000;
constexpr int TL2 = USE_TL ? 1950 : 10000000;

// define N and stuff here
constexpr int T = 100;
constexpr int T1 = T + 1;
constexpr int L = 20;
constexpr int L2 = L * L;
constexpr int MAX_N = 40000;
constexpr int DIRS[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
constexpr int DIR_U = 0;
constexpr int DIR_D = 1;
constexpr int DIR_L = 2;
constexpr int DIR_R = 3;
#define moveIJ(i2, j2, i, j, dir) int i2 = i + DIRS[dir][0], j2 = j + DIRS[dir][1]

#define repn() rep0(k, n)
#define rep2() rep0(i, L) rep0(j, L)

constexpr double linearstep(double a, double e0, double e1) {
	return (a - e0) / (e1 - e0);
}

constexpr double saturate(double a) {
	return clamp(a, 0.0, 1.0);
}

constexpr bool inBounds(int i, int j) {
	return i >= 0 && i < L && j >= 0 && j < L;
}

static constexpr int TRANSITION_MAP[256 * 4] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 244, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 240, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 233, 0, 0, 4, 232, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 6, 224, 0, 0, 5, 225, 228, 0, 4, 224, 0, 0, 3, 224, 0, 0, 2, 224, 0, 0,
    1, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 212, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 208, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    7, 192, 0, 0, 6, 192, 0, 0, 5, 193, 196, 0, 4, 192, 0, 0, 3, 192, 0, 0, 2, 192, 0, 0, 1, 192, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 148, 0, 0, 40, 148, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 144, 0, 0, 40,
    144, 0, 0, 32, 151, 0, 0, 32, 150, 0, 0, 33, 149, 180, 0, 32, 148, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, 145,
    176, 0, 32, 144, 0, 0, 47, 128, 0, 0, 0, 0, 0, 0, 45, 132, 169, 0, 44, 132, 168, 0, 43, 128, 0, 0, 0, 0,
    0, 0, 41, 128, 0, 0, 40, 128, 0, 0, 39, 135, 160, 0, 38, 134, 160, 0, 37, 133, 161, 164, 36, 132, 160, 0,
    35, 131, 160, 0, 34, 130, 160, 0, 33, 129, 160, 0, 32, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 148, 0, 0, 8,
    148, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 144, 0, 0, 8, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 148, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 144, 0, 0, 0, 0, 0, 0, 15, 128, 0, 0, 0, 0, 0, 0, 13, 132, 137, 0, 12,
    132, 136, 0, 11, 128, 0, 0, 0, 0, 0, 0, 9, 128, 0, 0, 8, 128, 0, 0, 7, 128, 0, 0, 6, 128, 0, 0, 5, 129,
    132, 0, 4, 128, 0, 0, 3, 128, 0, 0, 2, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 105, 0, 0, 4, 104, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 96, 0, 0, 6, 96, 0, 0, 5, 97, 100, 0, 4, 96, 0, 0,
    3, 96, 0, 0, 2, 96, 0, 0, 1, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 7, 64, 0, 0, 6, 64, 0, 0, 5, 65, 68, 0, 4, 64, 0, 0, 3, 64, 0, 0, 2, 64, 0, 0, 1,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 41, 0, 0, 20, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 41,
    0, 0, 16, 40, 0, 0, 23, 32, 0, 0, 22, 32, 0, 0, 21, 33, 52, 0, 20, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17,
    33, 48, 0, 16, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 41, 0, 0, 4, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 7, 32, 0, 0, 6, 32, 0, 0, 5, 33, 36, 0, 4, 32, 0, 0, 3, 32, 0, 0, 2, 32, 0, 0, 1, 32, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 20, 0, 0, 8, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 16, 0, 0, 8,
    16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 16, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 9, 0, 0, 4, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

class Problem {
private:
	bool wallVs[L][L - 1] = {};
	bool wallHs[L - 1][L] = {};
	bool walls[L2 * 4] = {};

public:
	int n = 0;
	int i0 = 0;
	int sd[MAX_N][2] = {};
	int scores[MAX_N] = {};

	int neighbors[L2 * 4] = {};
	int numN[L2] = {};

	Problem() {
	}

	void load(istream& in) {
		int t, h, w;
		in >> t >> h >> w >> i0;
		assert(t == T);
		assert(h == L);
		assert(w == L);
		assert(i0 >= 0 && i0 < L);
		rep0(i, L - 1) {
			string l;
			in >> l;
			rep0(j, L) {
				wallHs[i][j] = l[j] != '0';
			}
		}
		rep0(i, L) {
			string l;
			in >> l;
			rep0(j, L - 1) {
				wallVs[i][j] = l[j] != '0';
			}
		}
		in >> n;
		repn() {
			in >> sd[k][0] >> sd[k][1];
			sd[k][0]--;
			scores[k] = sd[k][1] - sd[k][0];
		}
		rep2() {
			rep0(dir, 4) {
				int idx = i * L + j;
				walls[idx * 4 + dir] = wallSlow(i, j, dir);
				if (!walls[idx * 4 + dir]) {
					moveIJ(i2, j2, i, j, dir);
					neighbors[idx * 4 + numN[idx]++] = i2 * L + j2;
				}
			}
		}
	}

	bool wallH(int i, int j) const {
		assert(i >= 0 && i < L - 1);
		assert(j >= 0 && j < L);
		return wallHs[i][j];
	}

	bool wallV(int i, int j) const {
		assert(i >= 0 && i < L);
		assert(j >= 0 && j < L - 1);
		return wallVs[i][j];
	}

	bool wall(int i, int j, int dir) const {
		assert(inBounds(i, j));
		return walls[(i * L + j) * 4 + dir];
	}

	bool wallSlow(int i, int j, int dir) const {
		assert(inBounds(i, j));
		switch (dir) {
		case DIR_U:
			return i == 0 || wallH(i - 1, j);
		case DIR_D:
			return i == L - 1 || wallH(i, j);
		case DIR_L:
			return j == 0 || wallV(i, j - 1);
		case DIR_R:
			return j == L - 1 || wallV(i, j);
		default:
			assert(false);
			return true;
		}
	}
};

struct SolverResult {
	int score = 0;
};

class PlantSet {
public:
	int num = 0;
	pii data[T1] = {};
};

class LenSet {
public:
	ull bits = 0;
	int counts[64] = {};

	LenSet() {
	}

	void inc(int l) {
		assert(l > 0);
		l = min(l, 64);
		bits |= 1ull << (64 - l);
		counts[l - 1]++;
	}

	void dec(int l) {
		assert(l > 0);
		l = min(l, 64);
		assert(counts[l - 1] > 0);
		bits &= ~((ull) (--counts[l - 1] == 0) << (64 - l));
	}

	void dump() const {
		tracen("[");
		ull left = bits;
		while (left != 0) {
			ull bit = left & -left;
			left ^= bit;
			int i = 64 - ctz64(bit);
			int num = counts[i - 1];
			tracen(i);
			if (num > 1)
				tracen(":", num);
			if (left != 0)
				tracen(" ");
		}
		trace("]");
	}

	// packs b into a
	static int pack(const LenSet& a, int numB, const pii* dataB) {
		ull la = a.bits;
		int res = 0;
		pii na;
		{
			if (la == 0)
				return res;
			ull bit = la & -la;
			la ^= bit;
			int i = 64 - ctz64(bit);
			na = pii(i, a.counts[i - 1]);
		}
		pii nb;
		{
			if (numB-- == 0)
				return res;
			nb = *dataB++;
		}
		while (true) {
			while (na.second == 0) {
				if (la == 0)
					return res;
				ull bit = la & -la;
				la ^= bit;
				int i = 64 - ctz64(bit);
				na = pii(i, a.counts[i - 1]);
			}
			while (nb.second == 0 || na.first < nb.first) {
				if (numB-- == 0)
					return res;
				nb = *dataB++;
			}
			int num = min(na.second, nb.second);
			na.second -= num;
			nb.second -= num;
			res += num * nb.first;
		}
	}
};

class Room {
public:
	LenSet lens[T1] = {};
	int scores[T1] = {};
	int rawScore = 0;
	int totalArea = 0;
	bool dirty[T1] = {};
	int ds[T1] = {};
	int numD = 0;

	Room() {
	}

	void inc(int d, int l) {
		totalArea++;
		lens[d].inc(l);
		if (!dirty[d]) {
			dirty[d] = true;
			ds[numD++] = d;
		}
	}

	void dec(int d, int l) {
		totalArea--;
		lens[d].dec(l);
		if (!dirty[d]) {
			dirty[d] = true;
			ds[numD++] = d;
		}
	}

	double score() {
		return rawScore;
	}
};

class Cell {
public:
	bitset<T1> bits = 0;
	int ds[T] = {};
	int numD = 0;
	bool aisle = false;

	Cell() {
	}

	bool has(int d) {
		return bits[d];
	}

	int floor(int d) {
		rep0r(i, numD) {
			if (ds[i] <= d)
				return ds[i];
		}
		return 0;
	}

	int ceil(int d) {
		rep0(i, numD) {
			if (ds[i] >= d)
				return ds[i];
		}
		return T;
	}

	void clear() {
		numD = 0;
	}

	void insert(int d) {
		assert(!has(d));
		bits[d] = true;
		int i = numD++;
		while (i > 0) {
			if (ds[i - 1] > d) {
				ds[i] = ds[i - 1];
				i--;
			} else {
				break;
			}
		}
		ds[i] = d;
	}

	void erase(int d) {
		bits[d] = false;
		rep0(i, numD) {
			if (ds[i] == d) {
				while (++i < numD) {
					ds[i - 1] = ds[i];
				}
				numD--;
				return;
			}
		}
		assert(false);
	}

	template <typename IntIntVoid>
	void forEachRange(const IntIntVoid& f) const {
		int d = 0;
		rep0(i, numD) {
			f(d, ds[i]);
			d = ds[i];
		}
		f(d, T);
	}
};

class Move {
public:
	static constexpr int AIS = 0;
	static constexpr int INS = 1;
	static constexpr int ERS = 2;
	static constexpr int INC = 3;
	static constexpr int DEC = 4;
	static constexpr int UPD = 5;

	int kind;
	array<int, 4> data;

	Move(int kind = 0, int d0 = 0, int d1 = 0, int d2 = 0, int d3 = 0) : kind(kind), data({d0, d1, d2, d3}) {
	}

	static Move aisle(int i, int j) {
		return Move(AIS, i, j);
	}

	static Move insert(int i, int j, int d) {
		return Move(INS, i, j, d);
	}

	static Move erase(int i, int j, int d) {
		return Move(ERS, i, j, d);
	}

	static Move inc(int d, int l) {
		return Move(INC, d, l);
	}

	static Move dec(int d, int l) {
		return Move(DEC, d, l);
	}

	static Move update(int d, int dscore) {
		return Move(UPD, d, dscore);
	}
};

class Diff {
public:
	static constexpr int MAX_MOVES = 10000;
	array<Move, MAX_MOVES> moves;
	int size = 0;

	Diff() {
	}

	void clear() {
		size = 0;
	}
};

namespace dfs {
	int targetD = 0;
	int dtime = 1;
	int dcount = 0;
	int vis[L2] = {};
	int ord[L2] = {};
	int low[L2] = {};
	bool ap[L2] = {};
	bool touchingAisle[L2] = {};
	int aisleTouchCount = 0;
	int numNonApCands = 0;
	int nonApCands[L2] = {};
	vector<pii> possibleAps;
	vector<pii> nonAps;
} // namespace dfs

void beginDfs(int d) {
	using namespace dfs;
	targetD = d;
	dtime++;
	dcount = 0;
	aisleTouchCount = 0;
	numNonApCands = 0;
	possibleAps.clear();
}

void endDfs() {
	using namespace dfs;
	assert(aisleTouchCount > 0);
	nonAps.clear();
	rep0(i, numNonApCands) {
		int cand = nonApCands[i];
		if (dcount == 1 || aisleTouchCount > 1 || !touchingAisle[cand]) {
			// non-ap confirmed
			nonAps.emplace_back(cand / L, cand % L);
		} else {
			// no, it's ap
			possibleAps.emplace_back(cand / L, cand % L);
			ap[cand] = true;
		}
	}
}

class Params {
private:
public:
	static const int ADD = 0;
	static const int REM = 1;
	static const int SRK = 2;
	static const int GRW = 3;
	static const int SWP = 4;
	static const int SFT = 5;
	static const int BRG = 6;

	array<double, 4> temps;
	array<double, 4> add;
	array<double, 4> rem;
	array<double, 4> srk;
	array<double, 4> grw;
	array<double, 4> swp;
	array<double, 4> sft;
	array<double, 4> brg;

	int blurIter = 0;
	int aisleDist = 0;
	double minImportance = 0;

	Params() {
		setDefault();
	}

	void setDefault() {
		temps = {5.0, 2.0, 5.0, 0.06};
		add = {0.10, 0.10, 0.00, 0.00};
		rem = {0.10, 0.10, 0.00, 0.00};
		srk = {0.50, 0.50, 0.50, 0.50};
		grw = {0.50, 0.50, 0.50, 0.50};
		swp = {0.10, 0.05, 0.00, 0.00};
		sft = {0.10, 0.05, 0.00, 0.00};
		brg = {0.10, 0.10, 0.10, 0.10};
		blurIter = 4;
		aisleDist = 14;
		minImportance = 0.1;
	}
};

class Farm {
public:
	int i0 = 0;
	Room room;
	Cell cells[L2];
	const Problem& p;
	const PlantSet (&plants)[T1];

	vector<pair<int, pii>> lengthList[T1];

	bool importanceSampling = false;
	double importanceProb = 0.0;
	double importance[L2] = {};
	int occupation[L2] = {};
	bool isActuallyUsed[T1][L2] = {};

	vector<int> changedDs;
	vector<pii> nonApsCache[T1];
	vector<pii> possibleApsCache[T1];
	bool apDataCached[L2] = {};

	static constexpr int HISTORY_CAPACITY = 100000;
	array<Move, HISTORY_CAPACITY> history;
	int historySize = 0;
	Diff diff;

	Farm(const Problem& p, const PlantSet (&plants)[T1]) : p(p), plants(plants) {
	}

	void disposeHistory() {
		historySize = 0;
	}

	int historyPoint() {
		updateScore();
		return historySize;
	}

	void setDiff(Diff& diff, int base) {
		updateScore();
		diff.size = historySize - base;
		memcpy(diff.moves.data(), history.data() + base, sizeof(Move) * diff.size);
	}

	void applyDiff(const Diff& diff) {
		rep0(i, diff.size) {
			Move m = diff.moves[i];
			redoAtomic(m);
			assert(historySize < HISTORY_CAPACITY);
			pushMove(m);
		}
		// clean up
		rep0(i, room.numD) {
			int d = room.ds[i];
			room.dirty[d] = false;
		}
		room.numD = 0;
	}

	void undoTo(int hp) {
		while (historySize > hp) {
			undoAtomic(history[--historySize]);
		}
		// clean up
		rep0(i, room.numD) {
			int d = room.ds[i];
			room.dirty[d] = false;
		}
		room.numD = 0;
	}

	void init(int i0) {
		this->i0 = i0;
		rep0(i, L2 - 1) {
			room.inc(T, T);
		}
		updateScore();
		cell(i0, 0).aisle = true;
		trace("initial score: ", room.rawScore);
	}

	void updateScore() {
		rep0(i, room.numD) {
			int d = room.ds[i];
			int pscore = room.scores[d];
			int score = LenSet::pack(room.lens[d], plants[d].num, plants[d].data);
			int delta = score - pscore;
			room.rawScore += delta;
			room.scores[d] = score;
			room.dirty[d] = false;
			pushMove(Move::update(d, delta));
		}
		room.numD = 0;
	}

	void updateLengthList() {
		rep0(t, T1) {
			lengthList[t].clear();
		}
		rep2() {
			auto& c = cell(i, j);
			if (c.aisle)
				continue;
			c.forEachRange([&](int d1, int d2) {
				lengthList[d2].emplace_back(make_pair(d2 - d1, pii(i, j)));
			});
		}
		rep0(t, T1) {
			sort(lengthList[t].begin(), lengthList[t].end(), std::greater<pair<int, pii>>());
		}
	}

	void updateOccupation(const vector<pii> plantsForD[T1]) {
		updateLengthList();
		memset(isActuallyUsed, 0, sizeof(isActuallyUsed));
		rep2() {
			occupation[i * L + j] = 0;
		}
		rep0(t, T1) {
			int i1 = 0;
			int i2 = 0;
			while (true) {
				if (i1 >= lengthList[t].size())
					break;
				if (i2 >= plantsForD[t].size())
					break;
				auto l = lengthList[t][i1];
				auto p = plantsForD[t][i2];
				int roomLength = l.first;
				int plantLength = p.first;
				if (roomLength < plantLength) {
					i2++;
					continue;
				}
				int i = l.second.first;
				int j = l.second.second;
				isActuallyUsed[t][i * L + j] = true;
				occupation[i * L + j] += plantLength;
				i1++;
				i2++;
			}
		}
	}

	void updateImportance(const vector<pii> plantsForD[T1], const Params& params) {
		updateOccupation(plantsForD);

		// update importance
		rep2() {
			int idx = i * L + j;
			int leftRoom = T - occupation[idx];
			importance[idx] = max(params.minImportance, (double) leftRoom / T);
		}

		// blur importance
		rep0(iter, params.blurIter) {
			double tmp[L2];
			rep0(i, L2) {
				int nn = p.numN[i];
				double sum = importance[i];
				rep0(idx, nn) {
					int j = p.neighbors[i << 2 | idx];
					sum += importance[j];
				}
				tmp[i] = max(importance[i], sum / (nn + 1));
			}
			memcpy(importance, tmp, sizeof(importance));
		}

		// trace("importance:");
		// rep2() {
		// 	string s = to_string((int) (importance[i * L + j] * 99.99));
		// 	if (s.length() < 2)
		// 		s = " " + s;
		// 	tracen(s);
		// 	if (j == L - 1)
		// 		trace();
		// 	else
		// 		tracen(" ");
		// }
	}

	void setAisle(int i, int j) {
		auto& c = cell(i, j);
		switchAisle(i, j, true);
		c.forEachRange([&](int a, int b) {
			decRoom(b, b - a);
		});
		while (c.numD > 0) {
			eraseD(i, j, c.ds[0]);
		}
	}

	void unsetAisle(int i, int j) {
		auto& c = cell(i, j);
		switchAisle(i, j, false);
		assert(c.numD == 0);
		incRoom(T, T);
	}

	int aisleDeg(int i, int j) const {
		int res = 0;
		rep0(dir, 4) {
			if (!p.wall(i, j, dir)) {
				moveIJ(i2, j2, i, j, dir);
				if (cell(i2, j2).aisle)
					res++;
			}
		}
		return res;
	}

	void swapRandomHeights(Rnd& rnd, double sigma) {
		int d1, d2;
		do {
			d1 = rnd.nextInt(1, T - 1);
			double rand = rnd.nextNormal() * sigma;
			d2 = d1 + (int) round(rand);
		} while (d1 == d2 || d2 <= 0 || d2 >= T);

		vector<pii> ps1;
		vector<pii> ps2;
		rep2() {
			if (cell(i, j).has(d1)) {
				ps1.emplace_back(i, j);
			}
			if (cell(i, j).has(d2)) {
				ps2.emplace_back(i, j);
			}
		}
		if (ps1.empty() && ps2.empty())
			return;
		removePoints(ps1, d1);
		removePoints(ps2, d2);
		addPoints(ps1, d2);
		addPoints(ps2, d1);
		changedDs.push_back(d1);
		changedDs.push_back(d2);
	}

	void shrinkWhole(const vector<pii> plantsForD[T1]) {
		int removed = 0;
		updateOccupation(plantsForD);
		rep(d, 1, T) {
			bool end = false;
			while (!end) {
				end = true;
				if (!apDataCached[d]) {
					int id = -1, jd = -1;
					rep2() {
						if (cell(i, j).has(d)) {
							id = i;
							jd = j;
						}
					}
					if (id == -1)
						break;
					computeApsAt(id, jd, d);
					for (pii p : nonApsCache[d]) {
						assert(cell(p.first, p.second).has(d));
					}
				}
				for (pii p : nonApsCache[d]) {
					int i = p.first;
					int j = p.second;
					if (!isActuallyUsed[d][i * L + j]) {
						removePoint(p, d);
						removed++;
						apDataCached[d] = false;
						end = false;
						break;
					}
				}
			}
		}
		trace("removed ", removed, " points");
	}

	void addRandomLeaf(Rnd& rnd) {
		int i, j, d;
		int trial = 0;
		do {
			i = rnd.nextInt(L);
			j = rnd.nextInt(L);
			d = rnd.nextInt(2, T - 2);
			if (++trial >= L2)
				return;
		} while (cell(i, j).aisle || aisleDeg(i, j) == 0 || cell(i, j).has(d) ||
		    cell(i, j).floor(d) > d - 2 || cell(i, j).ceil(d) < d + 2);

		int maxSize = rnd.nextInt(1, 20);
		bool vis[L][L] = {};
		priority_queue<pair<double, pii>> q;
		q.emplace(0, pii(i, j));
		vis[i][j] = true;
		vector<pii> ps;
		int size = 0;
		while (!q.empty()) {
			auto top = q.top();
			q.pop();
			int i = top.second.first;
			int j = top.second.second;
			ps.emplace_back(i, j);
			if (++size >= maxSize) {
				break;
			}
			rep0(dir, 4) {
				if (p.wall(i, j, dir))
					continue;
				moveIJ(i2, j2, i, j, dir);
				if (vis[i2][j2])
					continue;
				auto& c2 = cell(i2, j2);
				if (c2.aisle)
					continue;
				if (c2.has(d))
					continue;
				if (c2.floor(d) > d - 2)
					continue;
				if (c2.ceil(d) < d + 2)
					continue;
				vis[i2][j2] = true;
				q.emplace(pow(rnd.nextFloat(), 1.0 / (c2.ceil(d) - c2.floor(d))), pii(i2, j2));
			}
		}
		addPoints(ps, d);
		changedDs.push_back(d);
	}

	void removeRandomLeaf(Rnd& rnd) {
		int trial = 0;
		int i, j;
		do {
			if (++trial >= L2)
				return;
			i = rnd.nextInt(L);
			j = rnd.nextInt(L);
		} while (cell(i, j).aisle || cell(i, j).numD == 0);
		int d = cell(i, j).ds[rnd.nextInt(cell(i, j).numD)];
		bool vis[L][L] = {};
		queue<pii> q;
		q.emplace(i, j);
		vis[i][j] = true;
		vector<pii> ps;
		while (!q.empty()) {
			pii top = q.front();
			q.pop();
			int i = top.first;
			int j = top.second;
			ps.emplace_back(i, j);
			rep0(dir, 4) {
				if (p.wall(i, j, dir))
					continue;
				moveIJ(i2, j2, i, j, dir);
				if (vis[i2][j2])
					continue;
				auto& c2 = cell(i2, j2);
				if (!c2.has(d))
					continue;
				vis[i2][j2] = true;
				q.emplace(i2, j2);
			}
		}
		removePoints(ps, d);
		changedDs.push_back(d);
	}

	void bridgeRandomLeaf(Rnd& rnd) {
		int trial = 0;
		int i, j;
		do {
			if (++trial >= L2)
				return;
			i = rnd.nextInt(L);
			j = rnd.nextInt(L);
		} while (cell(i, j).aisle || cell(i, j).numD == 0);
		int d = cell(i, j).ds[rnd.nextInt(cell(i, j).numD)];

		if (!apDataCached[d]) {
			computeApsAt(i, j, d);
		}

		const vector<pii>& poss = possibleApsCache[d];

		if (poss.empty())
			return;

		pii pos;
		do {
			pos = poss[rnd.nextInt(len(poss))];
			i = pos.first;
			j = pos.second;
		} while (importanceSampling && rnd.nextFloat() < importanceProb &&
		    importance[i * L + j] < rnd.nextFloat());

		int hp = historyPoint();
		double maxScore = 0;

		if (i == 0 || i == L - 1 || j == 0 || j == L - 1)
			return;
		assert(cell(i, j).has(d));
		int bits = 0;
		bits |= (int) (cell(i - 1, j - 1).has(d) || cell(i - 1, j - 1).aisle);
		bits |= (int) (cell(i - 1, j).has(d) || cell(i - 1, j).aisle) << 1;
		bits |= (int) (cell(i - 1, j + 1).has(d) || cell(i - 1, j + 1).aisle) << 2;
		bits |= (int) (cell(i, j - 1).has(d) || cell(i, j - 1).aisle) << 3;
		bits |= (int) (cell(i, j + 1).has(d) || cell(i, j + 1).aisle) << 4;
		bits |= (int) (cell(i + 1, j - 1).has(d) || cell(i + 1, j - 1).aisle) << 5;
		bits |= (int) (cell(i + 1, j).has(d) || cell(i + 1, j).aisle) << 6;
		bits |= (int) (cell(i + 1, j + 1).has(d) || cell(i + 1, j + 1).aisle) << 7;

		int wallMask = 0;

		wallMask |= (int) (p.wall(i - 1, j, DIR_L) || p.wall(i, j - 1, DIR_U)) << 0;
		wallMask |= (int) (p.wall(i - 1, j, DIR_R) || p.wall(i, j + 1, DIR_U)) << 2;
		wallMask |= (int) (p.wall(i + 1, j, DIR_L) || p.wall(i, j - 1, DIR_D)) << 5;
		wallMask |= (int) (p.wall(i + 1, j, DIR_R) || p.wall(i, j + 1, DIR_D)) << 7;
		wallMask |= (int) p.wall(i, j, DIR_U) << 1;
		wallMask |= (int) p.wall(i, j, DIR_L) << 3;
		wallMask |= (int) p.wall(i, j, DIR_R) << 4;
		wallMask |= (int) p.wall(i, j, DIR_D) << 6;

		int idx = (bits & ~wallMask) << 2;
		diff.clear();

		rep0(t, 4) {
			int bits2 = TRANSITION_MAP[idx];
			if (!bits2)
				break;
			if (bits2 & wallMask)
				continue;

			if (bits2 & 1 << 0)
				addPoint(pii(i - 1, j - 1), d);
			if (bits2 & 1 << 1)
				addPoint(pii(i - 1, j), d);
			if (bits2 & 1 << 2)
				addPoint(pii(i - 1, j + 1), d);
			if (bits2 & 1 << 3)
				addPoint(pii(i, j - 1), d);
			if (bits2 & 1 << 4)
				addPoint(pii(i, j + 1), d);
			if (bits2 & 1 << 5)
				addPoint(pii(i + 1, j - 1), d);
			if (bits2 & 1 << 6)
				addPoint(pii(i + 1, j), d);
			if (bits2 & 1 << 7)
				addPoint(pii(i + 1, j + 1), d);
			removePoint(pii(i, j), d);

			updateScore();
			double score = room.score();
			if (score > maxScore) {
				maxScore = score;
				setDiff(diff, hp);
				changedDs.push_back(d);
			}
			undoTo(hp);

			idx++;
		}

		applyDiff(diff);
	}

	void shrinkRandomLeaf(Rnd& rnd) {
		int trial = 0;
		int i, j;
		do {
			if (++trial >= L2)
				return;
			i = rnd.nextInt(L);
			j = rnd.nextInt(L);
		} while (cell(i, j).aisle || cell(i, j).numD == 0);
		int d = cell(i, j).ds[rnd.nextInt(cell(i, j).numD)];
		changedDs.push_back(d);

		if (!apDataCached[d]) {
			computeApsAt(i, j, d);
		}

		const vector<pii>& poss = nonApsCache[d];
		assert(len(poss) > 0);

		int hp = historyPoint();
		double maxScore = 0;
		diff.clear();

		pii pos;
		do {
			pos = poss[rnd.nextInt(len(poss))];
			i = pos.first;
			j = pos.second;
		} while (importanceSampling && rnd.nextFloat() < importanceProb &&
		    importance[i * L + j] < rnd.nextFloat());

		removePoint(pos, d);

		updateScore();
		double score = room.score();
		if (score > maxScore) {
			maxScore = score;
			setDiff(diff, hp);
		}

		int hp2 = historyPoint();
		int low = cell(i, j).floor(d) + 1;
		int high = cell(i, j).ceil(d) - 1;
		static int d2s[T1];
		int numD2s = 0;
		rep0(dir, 4) {
			if (p.wall(i, j, dir))
				continue;
			moveIJ(i2, j2, i, j, dir);
			rep(d2, low, high + 1) {
				if (d2 == d)
					continue;
				d2s[numD2s] = d2;
				numD2s += cell(i2, j2).has(d2);
			}
		}
		if (numD2s > 0) {
			int maxTrials = 10;
			rep(i, 1, numD2s) {
				int j = rnd.nextInt(i + 1);
				swap(d2s[i], d2s[j]);
			}
			int trials = min(maxTrials, numD2s);

			rep0(idx, trials) {
				int d2 = d2s[idx];
				addPoint(pos, d2);

				updateScore();
				double score = room.score();
				if (score > maxScore) {
					maxScore = score;
					setDiff(diff, hp);
					changedDs.push_back(d2);
				}
				undoTo(hp2);
			}
		}
		undoTo(hp);

		applyDiff(diff);
	}

	void growRandomLeaf(Rnd& rnd) {
		int trial = 0;
		int i, j, d;
		while (true) {
			if (++trial >= L2)
				return;
			i = rnd.nextInt(L);
			j = rnd.nextInt(L);
			if (importanceSampling && rnd.nextFloat() < importanceProb &&
			    importance[i * L + j] < rnd.nextFloat())
				continue;
			if (cell(i, j).aisle || cell(i, j).numD == 0)
				continue;
			d = cell(i, j).ds[rnd.nextInt(cell(i, j).numD)];
			int dir;
			do {
				dir = rnd.nextInt(4);
			} while (p.wall(i, j, dir));
			moveIJ(i2, j2, i, j, dir);
			auto& c2 = cell(i2, j2);
			if (!c2.has(d) && !c2.aisle) {
				i = i2;
				j = j2;
				break;
			}
		}
		pii pos = pii(i, j);
		addPoint(pos, d);
		changedDs.push_back(d);
	}

	void destroyAisle() {
		bool vis[L2] = {};
		set<int> dss[L2] = {};
		vector<pii> order;
		dfsAisle(i0, 0, vis, dss, order);
		order.pop_back();
		for (pii pos : order) {
			disposeHistory();
			int i = pos.first;
			int j = pos.second;
			unsetAisle(i, j);
			for (int d : dss[i * L + j])
				addPoint(pos, d);
		}
	}

	void shiftRandomLeaf(Rnd& rnd, double sigma) {
		int trial = 0;
		int i, j;
		do {
			if (++trial >= L2)
				return;
			i = rnd.nextInt(L);
			j = rnd.nextInt(L);
		} while (cell(i, j).aisle || cell(i, j).numD == 0);
		int d = cell(i, j).ds[rnd.nextInt(cell(i, j).numD)];
		bool vis[L][L] = {};
		queue<pii> q;
		q.emplace(i, j);
		vis[i][j] = true;
		vector<pii> ps;
		int maxBot = 0;
		int minTop = T;
		while (!q.empty()) {
			pii top = q.front();
			q.pop();
			int i = top.first;
			int j = top.second;
			ps.emplace_back(i, j);
			maxBot = max(maxBot, cell(i, j).floor(d - 1));
			minTop = min(minTop, cell(i, j).ceil(d + 1));
			rep0(dir, 4) {
				if (p.wall(i, j, dir))
					continue;
				moveIJ(i2, j2, i, j, dir);
				if (vis[i2][j2])
					continue;
				auto& c2 = cell(i2, j2);
				if (!c2.has(d))
					continue;
				vis[i2][j2] = true;
				q.emplace(i2, j2);
			}
		}
		if (minTop - maxBot <= 2)
			return;

		int delta;
		do {
			double rand = rnd.nextNormal() * sigma;
			delta = (int) round(rand);
		} while (delta == 0 || d + delta <= maxBot || d + delta >= minTop);
		shiftPoints(ps, d, delta);
		changedDs.push_back(d);
		changedDs.push_back(d + delta);
	}

	void addPoint(pii pos, int d) {
		int i = pos.first;
		int j = pos.second;
		auto& c = cell(i, j);
		assert(c.aisle == false);
		assert(!c.has(d));
		int bot = c.floor(d);
		int top = c.ceil(d);
		insertD(i, j, d);
		decRoom(top, top - bot);
		incRoom(top, top - d);
		incRoom(d, d - bot);
	}

	void removePoint(pii pos, int d) {
		int i = pos.first;
		int j = pos.second;
		auto& c = cell(i, j);
		assert(c.aisle == false);
		assert(c.has(d));
		int bot = c.floor(d - 1);
		int top = c.ceil(d + 1);
		eraseD(i, j, d);
		decRoom(top, top - d);
		decRoom(d, d - bot);
		incRoom(top, top - bot);
	}

	void addPoints(const vector<pii>& ps, int d) {
		for (pii pos : ps) {
			addPoint(pos, d);
		}
	}

	void removePoints(const vector<pii>& ps, int d) {
		for (pii pos : ps) {
			removePoint(pos, d);
		}
	}

	void shiftPoints(const vector<pii>& ps, int d, int delta) {
		assert(delta != 0);
		int d2 = d + delta;
		for (pii pos : ps) {
			removePoint(pos, d);
			addPoint(pos, d2);
		}
	}

	Cell& cell(int i, int j) {
		assert(inBounds(i, j));
		return cells[i * L + j];
	}

	const Cell& cell(int i, int j) const {
		assert(inBounds(i, j));
		return cells[i * L + j];
	}

	Cell& cell(int idx) {
		return cells[idx];
	}

	const Cell& cell(int idx) const {
		return cells[idx];
	}

	void draw(double progress, double temp, int singleD = -1) {
		double PI = 3.141592653589793;
		double hs = 0.5;
		double hshift = 0.4;
		double hdelta = hshift * hs;
		if (singleD == -1) {
			mov.target("all");
			mov.clearFrame();
			mov.fill(1);
			mov.textSize(0.8);
			mov.text("t=" + to_string(progress) + " T=" + to_string(temp), -20, 5);
			rep(d, 1, T) {
				mov.push();
				mov.translate(0, -hdelta * d);
				mov.scale(1, hs);
				mov.rotate(45 * PI / 180);
				double h = 1.0 * d / T;
				drawLayer(d, hdelta, false);
				mov.pop();
			}

			mov.endFrame();
		}

		rep(d, 1, T) {
			if (singleD != -1 && singleD != d)
				continue;
			mov.target("d=" + to_string(d));
			mov.clearFrame();
			mov.translate(0, -hdelta * d);
			mov.scale(1, hs);
			mov.rotate(45 * PI / 180);
			double h = 1.0 * d / T;
			drawLayer(d, hdelta, true);
			mov.endFrame();
		}
	}

	void drawLayer(int d, double hdelta, bool single) {
		if (single) {
			mov.noStroke();
			rep0(s, d) {
				double h = 1.0 * s / T;
				mov.fill(1 - h, 0, h, 0.6);
				rep2() {
					auto& c = cell(i, j);
					if (c.has(d)) {
						if (c.floor(d - 1) != s)
							continue;
						mov.tooltip("d=" + to_string(s) + " i=" + to_string(i) + " j=" + to_string(j));
						mov.rect(j + (d - s) * hdelta, i + (d - s) * hdelta, 1, 1);
					}
				}
				mov.noTooltip();
			}
			mov.strokeWeight(0.1);
			mov.stroke(1, 1, 1);
			rep2() {
				auto& c = cell(i, j);
				if (c.has(d)) {
					int s = c.floor(d - 1);
					mov.line(j + 0.5, i + 0.5, j + (d - s) * hdelta + 0.5, i + (d - s) * hdelta + 0.5);
				}
			}
		}
		double h = 1.0 * d / T;
		mov.noStroke();
		mov.fill(1 - h, 0, h, single ? 1 : 0.4);
		rep2() {
			auto& c = cell(i, j);
			if (c.has(d)) {
				mov.tooltip("d=" + to_string(d) + " i=" + to_string(i) + " j=" + to_string(j));
				mov.rect(j, i, 1, 1);
			}
		}
		mov.fill(1, 1, 1, single ? 1 : 0.4);
		rep2() {
			auto& c = cell(i, j);
			if (c.aisle) {
				mov.tooltip("d=" + to_string(d) + " i=" + to_string(i) + " j=" + to_string(j));
				mov.rect(j, i, 1, 1);
			}
		}
		mov.noTooltip();
		if (single) {
			mov.stroke(0, 1, 0);
			rep2() {
				auto& c = cell(i, j);
				rep0(dir, 4) {
					if (p.wall(i, j, dir)) {
						switch (dir) {
						case DIR_U:
							mov.line(j, i, j + 1, i);
							break;
						case DIR_D:
							mov.line(j, i + 1, j + 1, i + 1);
							break;
						case DIR_L:
							mov.line(j, i, j, i + 1);
							break;
						case DIR_R:
							mov.line(j + 1, i, j + 1, i + 1);
							break;
						}
					}
				}
			}
		}
	}

private:
	void computeApsAt(int i, int j, int d) {
		using namespace dfs;
		beginDfs(d);
		dfs(i * L + j, -1);
		endDfs();
		possibleApsCache[d] = possibleAps;
		nonApsCache[d] = nonAps;
		apDataCached[d] = true;
	}

	void pushMove(const Move& m) {
		assert(historySize < HISTORY_CAPACITY);
		history[historySize++] = m;
	}

	void dfs(int i, int prev) {
		using namespace dfs;
		vis[i] = dtime;
		ord[i] = dcount;
		low[i] = dcount;
		ap[i] = false;
		dcount++;
		int ni = i << 2;
		int niTo = ni + p.numN[i];
		bool aisleTouch = false;
		int numChildren = 0;
		while (ni < niTo) {
			int i2 = p.neighbors[ni++];
			auto& c2 = cell(i2);
			aisleTouch |= c2.aisle;
			if (c2.has(targetD)) {
				if (vis[i2] != dtime) {
					numChildren++;
					dfs(i2, i);
					ap[i] |= (prev != -1) & (ord[i] <= low[i2]);
					low[i] = min(low[i], low[i2]);
				} else {
					int mask = -(i2 != prev);
					int a = low[i];
					int b = ord[i2];
					low[i] = a & ~mask | min(a, b) & mask;
				}
			}
		}
		ap[i] |= (prev == -1) & (numChildren > 1);
		nonApCands[numNonApCands] = i;
		numNonApCands += !ap[i];
		touchingAisle[i] = aisleTouch;
		aisleTouchCount += aisleTouch;
		if (ap[i])
			possibleAps.emplace_back(i / L, i % L);
	}

	void dfsAisle(int i, int j, bool vis[L2], set<int> dss[L2], vector<pii>& order) {
		int idx = i * L + j;
		vis[idx] = true;
		auto& ds = dss[idx];
		rep0(dir, 4) {
			if (!p.wall(i, j, dir)) {
				moveIJ(i2, j2, i, j, dir);
				auto& c2 = cell(i2, j2);
				if (c2.aisle) {
					int idx2 = i2 * L + j2;
					if (!vis[idx2]) {
						dfsAisle(i2, j2, vis, dss, order);
						auto& ds2 = dss[idx2];
						ds.insert(ds2.begin(), ds2.end());
					}
				} else {
					rep0(idx, c2.numD) {
						ds.insert(c2.ds[idx]);
					}
				}
			}
		}
		order.emplace_back(i, j);
	}

	void switchAisle(int i, int j, bool to) {
		pushMove(Move::aisle(i, j));
		change(cell(i, j).aisle, !to, to);
	}

	void insertD(int i, int j, int d) {
		pushMove(Move::insert(i, j, d));
		cell(i, j).insert(d);
	}

	void eraseD(int i, int j, int d) {
		pushMove(Move::erase(i, j, d));
		cell(i, j).erase(d);
	}

	void incRoom(int d, int l) {
		pushMove(Move::inc(d, l));
		room.inc(d, l);
	}

	void decRoom(int d, int l) {
		pushMove(Move::dec(d, l));
		room.dec(d, l);
	}

	void redoAtomic(const Move& move) {
		auto& data = move.data;
		switch (move.kind) {
		case Move::AIS:
			cell(data[0], data[1]).aisle ^= true;
			break;
		case Move::INS:
			cell(data[0], data[1]).insert(data[2]);
			break;
		case Move::ERS:
			cell(data[0], data[1]).erase(data[2]);
			break;
		case Move::INC:
			room.inc(data[0], data[1]);
			break;
		case Move::DEC:
			room.dec(data[0], data[1]);
			break;
		default: // UPD
			room.scores[data[0]] += data[1];
			room.rawScore += data[1];
			break;
		}
	}

	void undoAtomic(const Move& move) {
		auto& data = move.data;
		switch (move.kind) {
		case Move::AIS:
			cell(data[0], data[1]).aisle ^= true;
			break;
		case Move::INS:
			cell(data[0], data[1]).erase(data[2]);
			break;
		case Move::ERS:
			cell(data[0], data[1]).insert(data[2]);
			break;
		case Move::INC:
			room.dec(data[0], data[1]);
			break;
		case Move::DEC:
			room.inc(data[0], data[1]);
			break;
		default: // UPD
			room.scores[data[0]] -= data[1];
			room.rawScore -= data[1];
			break;
		}
	}

	template <typename T>
	void change(T& lhs, T from, T to) const {
		if (lhs != from) {
			trace("expected: ", from, " -> ", to);
			trace("got: ", lhs);
			assert(lhs == from);
		}
		lhs = to;
	}
};

class Selector {
private:
	vector<int> opts;
	vector<double> ws;
	double wsum = 0;

public:
	Selector() {
	}

	void clear() {
		opts.clear();
		ws.clear();
		wsum = 0;
	}

	void add(int option, double weight) {
		opts.push_back(option);
		ws.push_back(weight);
		wsum += weight;
	}

	void update(int index, double weight) {
		wsum += weight - ws[index];
		ws[index] = weight;
	}

	int choose(Rnd& rnd) {
		double f = rnd.nextFloat(0, wsum);
		int n = len(opts);
		rep0(i, n) {
			if ((f -= ws[i]) <= 0) {
				return opts[i];
			}
		}
		return opts.back();
	}

	int size() const {
		return len(opts);
	}
};

class Solver {
public:
	SolverResult res;
	Problem p;
	Rnd rnd;
	int seed = -1;
	bool render = false;
	bool write = false;
	int n = 0;
	int i0 = 0;
	bool pass = false;
	int saIter = 0;
	double plantAmount = 0;

	Params params;
	PlantSet plants[T1];
	vector<pii> plantsForD[T1];
	Farm f;
	vector<array<int, 4>> solution;

	Solver() : f(p, plants) {
	}

	void load(istream& in) {
		p.load(in);
		n = p.n;
		i0 = p.i0;

		render &= isLocal;
		write &= isLocal;

		init();
	}

	void load(int seed) {
		this->seed = seed;
		string data = readText("in/"s +
		    (seed < 10            ? "000"
		            : seed < 100  ? "00"
		            : seed < 1000 ? "0"
		                          : "") +
		    to_string(seed) + ".txt")
		                  .first;
		istringstream in(data);
		isLocal = true;
		load(in);
	}

	void solve() {
		if (render) {
			mov.beginMovie(to_string(seed) + "_m.smv");
		}

		setupAisle();

		if (pass) {
			res.score = -1;
			return;
		}

		sa(0, 0, TL1);

		f.destroyAisle();
		f.updateScore();
		f.importanceSampling = true;
		f.importanceProb = 0.75;

		sa(1, TL1, TL2);
		f.shrinkWhole(plantsForD);

		plantAll();

		int rawScore = 0;
		for (auto& a : solution) {
			rawScore += p.scores[a[0]];
		}
		res.score = (int) round(1e6 * rawScore / (T * L2));
		trace("score: ", res.score);

		if (render) {
			mov.endMovie();
		}

		if (isLocal) {
			ostringstream out;
			print(out);
			writeText(to_string(seed) + "_o.txt", out.str());
		} else {
			print(cout);
		}
	}

private:
	void init() {
		int plantCounts[T1][T1] = {};
		repn() {
			int s = p.sd[k][0];
			int d = p.sd[k][1];
			if (d - s > 64) {
				trace("too big: ", d - s);
			} else {
				plantCounts[d][d - s]++;
			}
		}
		rep0(t, T1) {
			rep0r(l, T1) {
				int count = plantCounts[t][l];
				if (count > 0) {
					plants[t].data[plants[t].num++] = pii(l, count);
				}
			}
		}
		rep0(t, T1) {
			plantsForD[t].clear();
		}
		repn() {
			int s = p.sd[k][0];
			int d = p.sd[k][1];
			plantsForD[d].emplace_back(d - s, k);
		}
		rep0(t, T1) {
			sort(plantsForD[t].begin(), plantsForD[t].end(), std::greater<pii>());
		}
		f.init(i0);
		int scoreSum = 0;
		repn() {
			scoreSum += p.scores[k];
		}
		plantAmount = (double) scoreSum / (T * L2) - 1;
		// computeTransitionMap();
	}

	void computeTransitionMap() {
		int transitionMap[256 * 4] = {};
		auto isConnected = [](const bool tiles[3][3]) {
			int si = -1;
			int sj = -1;
			int count = 0;
			rep0(i, 3) {
				rep0(j, 3) {
					if (tiles[i][j]) {
						si = i;
						sj = j;
						count++;
					}
				}
			}
			assert(count > 0);
			bool vis[3][3] = {};
			int visCount = 0;
			queue<pii> q;
			vis[si][sj] = true;
			visCount++;
			q.emplace(si, sj);
			while (!q.empty()) {
				int i = q.front().first;
				int j = q.front().second;
				q.pop();
				rep0(dir, 4) {
					moveIJ(i2, j2, i, j, dir);
					if (i2 >= 0 && i2 < 3 && j2 >= 0 && j2 < 3 && tiles[i2][j2] && !vis[i2][j2]) {
						vis[i2][j2] = true;
						visCount++;
						q.emplace(i2, j2);
					}
				}
			}
			return count == visCount;
		};
		auto set9 = [](int bits, bool tiles[3][3]) {
			rep0(i, 3) {
				rep0(j, 3) {
					tiles[i][j] = (bits & 1 << (i * 3 + j)) != 0;
				}
			}
		};
		rep0(bits, 512) {
			if (bits & 16) {
				int holed = bits & ~16;
				if (holed == 0)
					continue;
				bool tiles[3][3];
				set9(bits, tiles);
				if (!isConnected(tiles))
					continue;
				set9(holed, tiles);
				if (isConnected(tiles))
					continue;
				set<int> cands;
				rep0(patched, 512) {
					if ((patched & 16) || (patched & holed) != holed)
						continue;
					int diff = holed ^ patched;
					set9(patched, tiles);
					if (isConnected(tiles)) {

						bool ng = false;
						rep0(t, 9) {
							if ((diff & (1 << t)) == 0)
								continue;
							int patched2 = patched ^ (1 << t);
							set9(patched2, tiles);
							if (isConnected(tiles)) {
								ng = true;
								break;
							}
						}
						if (!ng)
							cands.insert(diff);
					}
				}
				if (cands.empty())
					continue;
				assert(cands.size() <= 4);
				trace(cands.size(), " transition(s) found!");
				int num = 0;
				for (int cand : cands) {
					bool tiles1[3][3];
					bool tiles2[3][3];
					set9(bits, tiles1);
					set9(bits ^ cand ^ 16, tiles2);
					rep0(i, 3) {
						rep0(j, 3) {
							tracen(tiles1[i][j]);
							if (j == 2)
								tracen(i == 1 ? " -> " : "    ");
						}
						rep0(j, 3) {
							tracen(tiles2[i][j]);
							if (j == 2)
								trace();
						}
					}
					trace();
					int idx1 = (bits & 0b000'001'111) | (bits & 0b111'100'000) >> 1;
					int idx2 = (cand & 0b000'001'111) | (cand & 0b111'100'000) >> 1;
					transitionMap[idx1 << 2 | num++] = idx2;
				}
			}
		}
		rep0(i, 256 * 4) {
			tracen(transitionMap[i], ", ");
		}
		trace();
	}

	void setupAisle() {
		f.disposeHistory();

		trace("plant amount: ", plantAmount);
		// if (plantAmount <= 0.9) {
		// 	pass = true;
		// 	return;
		// }

		int targetDist = -1;
		vector<pii> targets;
		vector<pii> targetCands;
		bool first = true;
		while (true) {
			bool vis[L][L] = {};
			int dists[L][L] = {};
			int dist = 0;
			queue<pii> q;
			rep2() {
				if (f.cell(i, j).aisle) {
					q.emplace(i, j);
					vis[i][j] = true;
				}
			}
			while (!q.empty()) {
				int left = len(q);
				rep0(t, left) {
					pii top = q.front();
					q.pop();
					int i = top.first;
					int j = top.second;
					dists[i][j] = dist;
					rep0(dir, 4) {
						if (p.wall(i, j, dir))
							continue;
						moveIJ(i2, j2, i, j, dir);
						if (vis[i2][j2])
							continue;
						vis[i2][j2] = true;
						q.emplace(i2, j2);
					}
				}
				dist++;
			}
			pii pos = pii(i0, 0);
			if (targets.empty()) {
				int maxDist = 0;
				rep2() {
					int dist = dists[i][j];
					if (dist > maxDist) {
						maxDist = dist;
						pos = pii(i, j);
					}
				}
				if (targetDist == -1) {
					targetDist = params.aisleDist;
				}
			} else {
				int minDist = 10000;
				for (pii p : targets) {
					int i = p.first;
					int j = p.second;
					int dist = dists[i][j];
					if (dist == 0)
						continue;
					if (dist < minDist) {
						minDist = dist;
						pos = pii(i, j);
					}
				}
			}
			vector<pii> path;
			{
				int i = pos.first;
				int j = pos.second;
				while (dists[i][j] > 0) {
					path.emplace_back(i, j);
					int d1 = dists[i][j];
					int minWallDeg = 4;
					int ni = -1;
					int nj = -1;
					rep0(dir, 4) {
						if (p.wall(i, j, dir))
							continue;
						moveIJ(i2, j2, i, j, dir);
						int d2 = dists[i2][j2];
						if (d2 == d1 - 1) {
							int wallDeg = 0;
							rep0(dir2, 4) {
								wallDeg += p.wall(i2, j2, dir2);
							}
							if (wallDeg < minWallDeg) {
								minWallDeg = wallDeg;
								ni = i2;
								nj = j2;
							}
						}
					}
					assert(ni != -1 && nj != -1);
					i = ni;
					j = nj;
				}
			}
			reverse(path.begin(), path.end());
			if (targets.empty()) {
				assert(targetDist != -1);
				path.resize(max(0, len(path) - targetDist));
				// trace(len(path));
			}
			if (path.empty()) {
				if (targets.empty()) {
					targets = targetCands;
					f.undoTo(0);
					continue;
				}
				break;
			}
			for (pii p : path) {
				f.setAisle(p.first, p.second);
			}
			if (targets.empty()) {
				targetCands.push_back(path.back());
			}
		}
		// trace("---");
		// rep2() {
		// 	tracen(f.cell(i, j).aisle ? "*" : " ");
		// 	if (j == L - 1)
		// 		trace();
		// }
		// trace("---");
	}

	void sa(int phase, int timeStart, int timeFinish) {
		constexpr int ITER = 100000000;
		double progress = 0;
		double tempFrom = params.temps[phase * 2];
		double tempTo = params.temps[phase * 2 + 1];
		double temp = tempFrom;
		int bestScore = 0;

		Selector sel;
		sel.add(Params::ADD, params.add[phase * 2]);
		sel.add(Params::REM, params.rem[phase * 2]);
		sel.add(Params::SRK, params.srk[phase * 2]);
		sel.add(Params::GRW, params.grw[phase * 2]);
		sel.add(Params::SWP, params.swp[phase * 2]);
		sel.add(Params::SFT, params.sft[phase * 2]);
		sel.add(Params::BRG, params.brg[phase * 2]);

		int step = 0;
		if (render) {
			timeScale = 3.8;
		}

		for (int t = 0; t < ITER; t++) {
			saIter++;
			if ((t & 0xff) == 0) {
				progress = 1.0 * (timer() - timeStart) / (timeFinish - timeStart);
				if (progress >= 1)
					break;

				if (render && progress * 200 > step) {
					f.draw(progress, temp);
					step++;
				}
				sel.update(Params::ADD, mix(params.add[phase * 2], params.add[phase * 2 + 1], progress));
				sel.update(Params::REM, mix(params.rem[phase * 2], params.rem[phase * 2 + 1], progress));
				sel.update(Params::SRK, mix(params.srk[phase * 2], params.srk[phase * 2 + 1], progress));
				sel.update(Params::GRW, mix(params.grw[phase * 2], params.grw[phase * 2 + 1], progress));
				sel.update(Params::SWP, mix(params.swp[phase * 2], params.swp[phase * 2 + 1], progress));
				sel.update(Params::SFT, mix(params.sft[phase * 2], params.sft[phase * 2 + 1], progress));
				sel.update(Params::BRG, mix(params.brg[phase * 2], params.brg[phase * 2 + 1], progress));
				temp = tempFrom * pow(tempTo / tempFrom, progress);
			}
			if ((t & 0xfff) == 0 && f.importanceSampling) {
				f.updateImportance(plantsForD, params);
			}
			if ((t & 0x7fff) == 0 && phase == 1) {
				f.shrinkWhole(plantsForD);
			}

			f.disposeHistory();
			f.changedDs.clear();
			int hp = f.historyPoint();
			double score = f.room.score();
			double sigma = max(1.0, temp);

			switch (sel.choose(rnd)) {
			case Params::ADD:
				f.addRandomLeaf(rnd);
				break;
			case Params::REM:
				f.removeRandomLeaf(rnd);
				break;
			case Params::SRK:
				f.shrinkRandomLeaf(rnd);
				break;
			case Params::GRW:
				f.growRandomLeaf(rnd);
				break;
			case Params::SWP:
				f.swapRandomHeights(rnd, sigma);
				break;
			case Params::SFT:
				f.shiftRandomLeaf(rnd, sigma);
				break;
			case Params::BRG:
				f.bridgeRandomLeaf(rnd);
				break;
			default:
				assert(false);
			}
			f.updateScore();
			double tolerance = -log2(rnd.nextFloat()) * temp;
			if (f.room.score() < score - tolerance) {
				f.undoTo(hp);
			} else {
				for (int d : f.changedDs) {
					f.apDataCached[d] = false;
				}
			}
			if (t % 10000 == 0)
				trace((int) round(1e6 * f.room.rawScore / (L2 * T)), " ", temp, " ", t);
		}
		trace("total iter (phase ", phase, "): ", saIter);
	}

	void plantAll() {
		bool used[MAX_N] = {};
		f.updateLengthList();
		solution.clear();
		rep0(t, T1) {
			int i1 = 0;
			int i2 = 0;
			while (true) {
				if (i1 >= f.lengthList[t].size())
					break;
				if (i2 >= plantsForD[t].size())
					break;
				auto l = f.lengthList[t][i1];
				auto p = plantsForD[t][i2];
				if (l.first < p.first) {
					i2++;
					continue;
				}
				solution.push_back({p.second, l.second.first, l.second.second, t - l.first});
				used[p.second] = true;
				i1++;
				i2++;
			}
		}
	}

	void print(ostream& out) const {
		out << len(solution) << endl;
		for (auto a : solution) {
			out << a[0] + 1 << " " << a[1] << " " << a[2] << " " << a[3] + 1 << endl;
		}
	}
};

int main() {
#if 1
	isLocal = false;
	timer(true);
	Solver sol;
	sol.load(cin);
	sol.solve();
#elif 0
	tracen("[");
	rep0(i, 1000) {
		Solver s;
		debug = false;
		s.load(i);
		debug = true;
		if (s.plantAmount >= 0.5) {
			tracen(i, ", ");
		}
	}
	trace("]");
#elif 1
	int seed;
	cin >> seed;
	cin >> timeScale;
	Params params;

	rep0(i, 4) cin >> params.temps[i];
	rep0(i, 4) cin >> params.add[i];
	rep0(i, 4) cin >> params.rem[i];
	rep0(i, 4) cin >> params.srk[i];
	rep0(i, 4) cin >> params.grw[i];
	rep0(i, 4) cin >> params.swp[i];
	rep0(i, 4) cin >> params.sft[i];
	rep0(i, 4) cin >> params.brg[i];
	cin >> params.blurIter;
	cin >> params.aisleDist;
	cin >> params.minImportance;

	timer(true);
	Solver s;
	s.params = params;
	s.load(seed);
	s.solve();
	cout << s.res.score << " " << s.saIter << " " << timer() << endl;
#elif 1
	timeScale = 1.3;
	int num = 50;
	int from = 0;
	int stride = 1;
	int single = 0;

	int seeds[1000] = {};
	int times[1000] = {};
	int scores[1000] = {};

	int scoreSum = 0;
	int numInvalid = 0;

	vector<int> seedList = {};
	bool useList = false;

	if (!seedList.empty()) {
		useList = true;
		num = len(seedList);
		single = -1;
	}

	rep0(t, num) {
		timer(true);
		int seed = useList ? seedList[t] : single != -1 ? single : from + t * stride;
		trace("\n------------ SOLVING SEED ", seed, " ------------");
		Solver s;
		s.load(seed);
		s.solve();
		if (single == -1)
			trace("score: ", s.res.score);
		if (s.res.score == -1) {
			numInvalid++;
		} else {
			scoreSum += s.res.score;
		}
		seeds[t] = seed;
		scores[t] = s.res.score;
		times[t] = timer();
		if (single != -1)
			break;
	}
	if (single == -1) {
		trace("--- summary ---");
		pii timesRank[1000] = {};
		pii scoresRank[1000] = {};
		rep0(t, num) {
			timesRank[t] = pii(times[t], t);
			scoresRank[t] = pii(scores[t], t);
		}
		sort(timesRank, timesRank + num);
		sort(scoresRank, scoresRank + num);
		trace("seed:");
		rep0r(i, num) {
			if (scores[i] == -1)
				continue;
			int s = seeds[i];
			string space = s < 10 ? "   " : s < 100 ? "  " : s < 1000 ? " " : "";
			trace("  seed ", space, s, ": ", scores[i], " (time ", times[i], " ms)");
		}
		trace("score:");
		rep0r(t, num) {
			int i = scoresRank[t].second;
			if (scores[i] == -1)
				continue;
			int s = seeds[i];
			string space = s < 10 ? "   " : s < 100 ? "  " : s < 1000 ? " " : "";
			trace("  seed ", space, s, ": ", scores[i], " (time ", times[i], " ms)");
		}
		trace("total score: ", scoreSum,
		    ", mean: ", (int) (scoreSum / (double) (num - numInvalid) * 100) / 100.0);
		if (numInvalid > 0) {
			trace(numInvalid, " invalid cases");
		}
	} else {
		trace("total score: ", scoreSum, " time = ", timer());
	}
#endif
}
