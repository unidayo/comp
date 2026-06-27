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

#define NDEBUG
//#undef NDEBUG
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

// return in ms
int timer(bool reset = false) {
	static auto st = chrono::system_clock::now();
	if (reset) {
		st = chrono::system_clock::now();
		return 0;
	} else {
		auto en = chrono::system_clock::now();
		int elapsedTotal = (int) chrono::duration_cast<chrono::milliseconds>(en - st).count();
		return elapsedTotal;
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

template <class T>
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

void segv() {
	volatile int* a = nullptr;
	*a = 0;
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

	void clear() {
		writeInstruction("cl", {}, {});
	}

	void nextFrame() {
		writeInstruction("n", {}, {});
	}

	void target(string name) {
		writeInstruction("tg", {}, {name});
	}
};
Movie mov;

struct Result {
	ull score = 0;
};

constexpr int MAX_D = 14;
constexpr double INF = 1e18;

#define repd(i) rep0(i, d)
#define repd2(i, j) rep0(i, d) rep0(j, d)
#define repd3(i, j, k) rep0(i, d) rep0(j, d) rep0(k, d)
#define inBounds(coord) \
	((coord).i >= 0 && (coord).i < d && (coord).j >= 0 && (coord).j < d && (coord).k >= 0 && (coord).k < d)

// (z, y, x) -> (i, j, k)

class Silhouette {
public:
	int d = 0;

	// i-k plane
	bool ik[MAX_D][MAX_D] = {};

	// i-j plane
	bool ij[MAX_D][MAX_D] = {};

	Silhouette() {
	}
};

class Coord {
private:
public:
	char i;
	char j;
	char k;
	char __unused__;
	static constexpr int MIN = -128;
	static constexpr int MAX = 127;

	constexpr Coord(int i = 0, int j = 0, int k = 0) : i(i), j(j), k(k), __unused__(0) {
	}

	Coord operator+(const Coord& c) const {
		return Coord(i + c.i, j + c.j, k + c.k);
	}

	Coord operator-(const Coord& c) const {
		return Coord(i - c.i, j - c.j, k - c.k);
	}

	Coord operator*(int n) const {
		return Coord(i * n, j * n, k * n);
	}

	Coord operator-() const {
		return Coord(-i, -j, -k);
	}

	Coord min(const Coord& c) const {
		return Coord(std::min(i, c.i), std::min(j, c.j), std::min(k, c.k));
	}

	Coord max(const Coord& c) const {
		return Coord(std::max(i, c.i), std::max(j, c.j), std::max(k, c.k));
	}

	int rawData() const {
		return *((int*) this);
	}

	string str() const {
		return "C[" + to_string(i) + " " + to_string(j) + " " + to_string(k) + "]";
	}

	char operator[](int index) const {
		return ((char*) this)[index];
	}

	bool operator==(const Coord& c) const {
		return rawData() == c.rawData();
	}

	bool operator!=(const Coord& c) const {
		return rawData() != c.rawData();
	}

	bool operator<(const Coord& c) const {
		return rawData() < c.rawData();
	}
};

constexpr Coord STEPS[6] = {
    Coord(-1, 0, 0), //
    Coord(1, 0, 0),  //
    Coord(0, -1, 0), //
    Coord(0, 1, 0),  //
    Coord(0, 0, -1), //
    Coord(0, 0, 1)   //
};

constexpr int ROTS[24][2][6] = {
    {{0, 0, 1, 0, 2, 0}, {0, 0, 1, 0, 2, 0}},
    {{0, 0, 1, -1, 2, -1}, {0, 0, 1, -1, 2, -1}},
    {{0, 0, 2, 0, 1, -1}, {0, 0, 2, -1, 1, 0}},
    {{0, 0, 2, -1, 1, 0}, {0, 0, 2, 0, 1, -1}},
    {{0, -1, 1, 0, 2, -1}, {0, -1, 1, 0, 2, -1}},
    {{0, -1, 1, -1, 2, 0}, {0, -1, 1, -1, 2, 0}},
    {{0, -1, 2, 0, 1, 0}, {0, -1, 2, 0, 1, 0}},
    {{0, -1, 2, -1, 1, -1}, {0, -1, 2, -1, 1, -1}},
    {{1, 0, 0, 0, 2, -1}, {1, 0, 0, 0, 2, -1}},
    {{1, 0, 0, -1, 2, 0}, {1, -1, 0, 0, 2, 0}},
    {{1, 0, 2, 0, 0, 0}, {2, 0, 0, 0, 1, 0}},
    {{1, 0, 2, -1, 0, -1}, {2, -1, 0, 0, 1, -1}},
    {{1, -1, 0, 0, 2, 0}, {1, 0, 0, -1, 2, 0}},
    {{1, -1, 0, -1, 2, -1}, {1, -1, 0, -1, 2, -1}},
    {{1, -1, 2, 0, 0, -1}, {2, -1, 0, -1, 1, 0}},
    {{1, -1, 2, -1, 0, 0}, {2, 0, 0, -1, 1, -1}},
    {{2, 0, 0, 0, 1, 0}, {1, 0, 2, 0, 0, 0}},
    {{2, 0, 0, -1, 1, -1}, {1, -1, 2, -1, 0, 0}},
    {{2, 0, 1, 0, 0, -1}, {2, -1, 1, 0, 0, 0}},
    {{2, 0, 1, -1, 0, 0}, {2, 0, 1, -1, 0, 0}},
    {{2, -1, 0, 0, 1, -1}, {1, 0, 2, -1, 0, -1}},
    {{2, -1, 0, -1, 1, 0}, {1, -1, 2, 0, 0, -1}},
    {{2, -1, 1, 0, 0, 0}, {2, 0, 1, 0, 0, -1}},
    {{2, -1, 1, -1, 0, -1}, {2, -1, 1, -1, 0, -1}},
};

Coord rotate(int rindex, const Coord& c) {
	const int* r = ROTS[rindex][0];
	return Coord((c[r[0]] ^ r[1]) - r[1], (c[r[2]] ^ r[3]) - r[3], (c[r[4]] ^ r[5]) - r[5]);
}

Coord invRotate(int rindex, const Coord& c) {
	const int* r = ROTS[rindex][1];
	return Coord((c[r[0]] ^ r[1]) - r[1], (c[r[2]] ^ r[3]) - r[3], (c[r[4]] ^ r[5]) - r[5]);
}

class Transform {
public:
	int rot = 0;
	Coord off;

	Transform() {
	}

	Transform(int rot, const Coord& off) : rot(rot), off(off) {
	}

	Coord operator*(const Coord& c) const {
		return rotate(rot, c) + off;
	}

	Coord imul(const Coord& c) const {
		return invRotate(rot, c - off);
	}

	string str() const {
		return "TF[R[" + to_string(rot) + "] " + off.str() + "]";
	}
};

class BlockPos {
public:
	int index;
	Coord lpos;

	BlockPos(int index, Coord lpos) : index(index), lpos(lpos) {
	}

	BlockPos() : index(-1), lpos({}) {
	}
};

class Block {
private:
	int indices[64][64][64];

public:
	vector<Coord> poss;
	vector<array<int, 6>> adjs;

	Block() {
		memset(indices, -1, sizeof(indices));
	}

	bool needsNormalization() const {
		auto p = poss[0];
		return min(p.i, min(p.j, p.k)) < -16 || max(p.i, max(p.j, p.k)) >= 16;
	}

	void normalize() {
		memset(indices, -1, sizeof(indices));
		auto diff = -poss[0];
		int n = len(poss);
		rep0(i, n) {
			auto& p = poss[i];
			p = p + diff;
			iat(p) = i;
		}
	}

	void add(const Coord& pos) {
		assert(!has(pos));
		int i = iat(pos) = len(poss);
		poss.push_back(pos);
		adjs.push_back({-1, -1, -1, -1, -1, -1});
		connect(i);
	}

	void clear() {
		for (auto& p : poss) {
			iat(p) = -1;
		}
		poss.clear();
		adjs.clear();
	}

	int remove(const Coord& pos) {
		assert(has(pos));
		int i = iat(pos);
		int bi = len(poss) - 1;
		disconnect(i);
		if (i != bi)
			disconnect(bi);
		auto b = poss[bi];
		iat(b) = i;
		poss[i] = b;
		iat(pos) = -1;
		poss.pop_back();
		if (i != bi)
			connect(i);
		return i;
	}

	bool has(const Coord& pos) const {
		return iat(pos) != -1;
	}

	array<Coord, 2> getAABB() const {
		array<Coord, 2> res = {
		    Coord(Coord::MAX, Coord::MAX, Coord::MAX), Coord(Coord::MIN, Coord::MIN, Coord::MIN)};
		for (auto& p : poss) {
			res[0].i = min(res[0].i, p.i);
			res[0].j = min(res[0].j, p.j);
			res[0].k = min(res[0].k, p.k);
			res[1].i = max(res[1].i, p.i);
			res[1].j = max(res[1].j, p.j);
			res[1].k = max(res[1].k, p.k);
		}
		return res;
	}

	string str() const {
		ostringstream os;
		os << "B[";
		bool first = true;
		for (auto p : poss) {
			if (!first)
				os << " ";
			first = false;
			os << p.str();
		}
		os << "]";
		return os.str();
	}

	bool canBeEaten(BlockPos eat1, BlockPos eat2) const {
		assert(eat1.index != -1);
		assert(has(eat1.lpos));
		assert(eat2.index == -1 || has(eat2.lpos));

		if (eat2.index == -1) {
			if (canBeEatenFast(eat1.lpos))
				return true;
		}
		int n = len(poss);

		bool visited[1000] = {};

		visited[iat(eat1.lpos)] = true;
		if (eat2.index != -1)
			visited[iat(eat2.lpos)] = true;

		static int stack[1000];
		rep0(first, n) {
			if (visited[first])
				continue;
			int stackCount = 0;
			int visitCount = 1;
			visited[first] = true;
			stack[stackCount++] = first;
			while (stackCount > 0) {
				auto i = stack[--stackCount];
				rep0(t, 6) {
					int j = adjs[i][t];
					if (j == -1 || visited[j])
						continue;
					visited[j] = true;
					visitCount++;
					stack[stackCount++] = j;
				}
			}
			return visitCount == n - (eat2.index == -1 ? 1 : 2);
		}
		return false;
	}

	bool canBeEatenFast(const Coord& eat) const {
		int bits = 0;
		for (auto& p : poss) {
			if (p != eat) {
				auto diff = p - eat;
				if (abs(diff.i) <= 1 && abs(diff.j) <= 1 && abs(diff.k) <= 1) {
					diff = diff + Coord(1, 1, 1);
					bits |= 1 << (diff.i * 9 + diff.j * 3 + diff.k);
				}
			}
		}
		if (bits == 0)
			return false;
		if ((bits & (bits - 1)) == 0)
			return true;
		int fill = bits & ~(bits - 1);
		constexpr int ML1 = 0b011011011011011011011011011;
		constexpr int MR1 = 0b110110110110110110110110110;
		constexpr int ML3 = 0b000111111000111111000111111;
		constexpr int MR3 = 0b111111000111111000111111000;
		rep0(iter, 10) {
			fill |= ((fill & ML1) << 1 | (fill & MR1) >> 1 | (fill & ML3) << 3 | (fill & MR3) >> 3 |
			            fill << 9 | fill >> 9) &
			    bits;
			if (fill == bits)
				return true;
		}
		return false;
	}

private:
	void connect(int i) {
		auto base = poss[i];
		rep0(t, 6) {
			auto pos = base + STEPS[t];
			int j = iat(pos);
			if (j == -1)
				continue;
			assert(adjs[i][t] == -1);
			assert(adjs[j][t ^ 1] == -1);
			adjs[i][t] = j;
			adjs[j][t ^ 1] = i;
		}
	}

	void disconnect(int i) {
		rep0(t, 6) {
			int j = adjs[i][t];
			if (j == -1)
				continue;
			assert(adjs[i][t] == j);
			assert(adjs[j][t ^ 1] == i);
			adjs[i][t] = -1;
			adjs[j][t ^ 1] = -1;
		}
	}

	int& iat(const Coord& c) {
		return indices[c.i + 32][c.j + 32][c.k + 32];
	}

	const int& iat(const Coord& c) const {
		return indices[c.i + 32][c.j + 32][c.k + 32];
	}
};

class Placement {
private:
public:
	Block b;
	Transform tf;
	bool placed = false;
	vector<Coord> placedPoss;

	Placement() {
	}
};

class CellInfo {
private:
public:
	int index;
	Coord pos;
	int scoreForFlip;

	CellInfo(int index, Coord pos, int scoreForFlip) : index(index), pos(pos), scoreForFlip(scoreForFlip) {
	}
};

class Grow {
private:
public:
	BlockPos pos;
	BlockPos eat1;
	BlockPos eat2;

	Grow(BlockPos pos, BlockPos eat1, BlockPos eat2) : pos(pos), eat1(eat1), eat2(eat2) {
	}

	Grow() {
	}
};

class Teleport {
private:
public:
	int index;
	Transform tf;
	BlockPos eat;

	Teleport(int index, Transform tf, BlockPos eat) : index(index), tf(tf), eat(eat) {
	}

	Teleport() : index(-1) {
	}
};

constexpr int WALL_INDEX = 0xffff;

class Solution {
public:
	int n = 0;
	int indices[2][MAX_D][MAX_D][MAX_D] = {};

	Solution() {
	}

	void print(ostream& out, int d) {
		out << n << endl;
		rep0(slot, 2) {
			int count = 0;
			repd3(i, j, k) {
				auto c = Coord(k, j, i);
				int index = indices[slot][c.i][c.j][c.k];
				if (count++ > 0) {
					out << " ";
				}
				if (index != -1 && index != WALL_INDEX) {
					out << index + 1;
				} else {
					out << 0;
				}
			}
			out << endl;
		}
	}
};

class Snapshot {
private:
public:
	int iter = 0;
	int maxIter = 0;
	double score = INF;
	int numBlocks = 0;
	int numCubes[100] = {};
	Coord cubePoss[100][500];
	Transform blockTransforms[2][100];

	Snapshot() {
	}
};

class SAParams {
private:
public:
	int weight1;
	int weight2;
	int weight3;
	int weight4;
	int weightVol1;
	int weightVol2;
	int weightVol3;
	int singleIterations;
	int minCutLevel;
	double pruneFrom;
	double pruneTo;
	double prunePow;
	double tFrom;
	double tTo;
	double growFrom;
	double growTo;
	double shrinkFrom;
	double shrinkTo;
	double teleFrom;
	double teleTo;
	double allowEatGrowFrom;
	double allowEatGrowTo;
	double allowEatTeleFrom;
	double allowEatTeleTo;

	SAParams() {
		weight1 = 39;
		weight2 = 21;
		weight3 = 6;
		weight4 = 3;
		weightVol1 = 30;
		weightVol2 = 0;
		weightVol3 = 0;
		singleIterations = 300000;
		minCutLevel = 1;
		pruneFrom = 0.04;
		pruneTo = 0.4;
		prunePow = 2.0;
		tFrom = 7e10;
		tTo = 100000;
		growFrom = 2.5;
		growTo = 4;
		shrinkFrom = 3;
		shrinkTo = 7;
		teleFrom = 9;
		teleTo = 4;
		allowEatGrowFrom = 0.9;
		allowEatGrowTo = 0.2;
		allowEatTeleFrom = 0.2;
		allowEatTeleTo = 0;
	}
	
	void set(bool dhigh, bool phigh) {
		if (!dhigh) {
			if(!phigh) {
				// D0P0
				weight1 = 39;
				weight2 = 21;
				weight3 = 7;
				weight4 = 3;
				weightVol1 = 29;
				weightVol2 = 5;
				weightVol3 = 0;
				singleIterations = 350000;
				minCutLevel = 1;
				pruneFrom = 0.08;
				pruneTo = 0.4;
				prunePow = 2.8;
				tFrom = 7e10;
				tTo = 100000;
				growFrom = 2.4;
				growTo = 6;
				shrinkFrom = 3;
				shrinkTo = 7;
				teleFrom = 10;
				teleTo = 4;
				allowEatGrowFrom = 0.9;
				allowEatGrowTo = 0.2;
				allowEatTeleFrom = 0.2;
				allowEatTeleTo = 0;
			} else {
				// D0P1
				weight1 = 39;
				weight2 = 21;
				weight3 = 7;
				weight4 = 3;
				weightVol1 = 29;
				weightVol2 = 5;
				weightVol3 = 0;
				singleIterations = 350000;
				minCutLevel = 1;
				pruneFrom = 0.04;
				pruneTo = 0.6;
				prunePow = 2.0;
				tFrom = 7e10;
				tTo = 100000;
				growFrom = 2.4;
				growTo = 6;
				shrinkFrom = 3;
				shrinkTo = 7;
				teleFrom = 10;
				teleTo = 4;
				allowEatGrowFrom = 0.9;
				allowEatGrowTo = 0.2;
				allowEatTeleFrom = 0.2;
				allowEatTeleTo = 0;
			}
		} else {
			if(!phigh) {
				// D1P0
				weight1 = 39;
				weight2 = 21;
				weight3 = 7;
				weight4 = 3;
				weightVol1 = 29;
				weightVol2 = 5;
				weightVol3 = 0;
				singleIterations = 320000;
				minCutLevel = 1;
				pruneFrom = 0.08;
				pruneTo = 0.5;
				prunePow = 3.0;
				tFrom = 7e10;
				tTo = 100000;
				growFrom = 2.4;
				growTo = 6;
				shrinkFrom = 3;
				shrinkTo = 7;
				teleFrom = 10;
				teleTo = 4;
				allowEatGrowFrom = 0.9;
				allowEatGrowTo = 0.2;
				allowEatTeleFrom = 0.2;
				allowEatTeleTo = 0;
			} else {
				// D1P1
				weight1 = 39;
				weight2 = 21;
				weight3 = 7;
				weight4 = 3;
				weightVol1 = 29;
				weightVol2 = 5;
				weightVol3 = 0;
				singleIterations = 350000;
				minCutLevel = 1;
				pruneFrom = 0.04;
				pruneTo = 0.6;
				prunePow = 2.5;
				tFrom = 7e10;
				tTo = 100000;
				growFrom = 2.4;
				growTo = 6;
				shrinkFrom = 3;
				shrinkTo = 7;
				teleFrom = 10;
				teleTo = 4;
				allowEatGrowFrom = 0.9;
				allowEatGrowTo = 0.2;
				allowEatTeleFrom = 0.2;
				allowEatTeleTo = 0;
			}
		}
	}
};

class Field {
private:
	static constexpr int LO = 25;

public:
	int d = 0;
	int indices[64][64][64] = {};
	int penalties[MAX_D][MAX_D][MAX_D] = {};
	int ijCounts[MAX_D][MAX_D] = {};
	int ikCounts[MAX_D][MAX_D] = {};
	int ijWeights[MAX_D][MAX_D] = {};
	int ikWeights[MAX_D][MAX_D] = {};
	int uncoveredRaw = 0;
	int totalPenalty = 0;
	SAParams params;
	vector<Placement> bs;

	int emptyIndices[MAX_D][MAX_D][MAX_D] = {};
	vector<Coord> empties;

	Silhouette sil;

	Field() {
	}

	void normalizeBlocks() {
		for (auto& b : bs) {
			if (b.b.needsNormalization()) {
				b.tf.off = b.placedPoss[0];
				b.b.normalize();
			}
		}
	}

	void init(int d, Silhouette& sil, const SAParams& params) {
		this->d = d;
		this->sil = sil;
		this->params = params;
		clear();
	}

	void clear() {
		empties.clear();

		memset(emptyIndices, -1, sizeof(emptyIndices));
		rep0(i, 64) rep0(j, 64) rep0(k, 64) {
			indices[i][j][k] = WALL_INDEX;
		}

		repd3(i, j, k) {
			if (sil.ij[i][j] && sil.ik[i][k]) {
				iat(i, j, k) = -1;
				emptyIndices[i][j][k] = len(empties);
				empties.emplace_back(i, j, k);
			}
		}

		totalPenalty = 0;

		int ijSum[MAX_D] = {};
		int ikSum[MAX_D] = {};
		repd(i) {
			repd(jk) {
				if (sil.ij[i][jk])
					ijSum[i]++;
				if (sil.ik[i][jk])
					ikSum[i]++;
			}
		}

		uncoveredRaw = 0;
		repd2(i, jk) {
			int& wij = ijWeights[i][jk];
			int& wik = ikWeights[i][jk];
			int ij = (int) sil.ij[i][jk];
			int ik = (int) sil.ik[i][jk];
			wij = shadowWeight(numConnections(sil.ij, i, jk));
			wik = shadowWeight(numConnections(sil.ik, i, jk));
			switch (ij * ikSum[i]) {
			case 0:
				assert(ij == 0);
				break;
			case 1:
				wij += params.weightVol1;
				break;
			case 2:
				wij += params.weightVol2;
				break;
			case 3:
				wij += params.weightVol3;
				break;
			default:
				break;
			}
			switch (ik * ijSum[i]) {
			case 0:
				assert(ik == 0);
				break;
			case 1:
				wik += params.weightVol1;
				break;
			case 2:
				wik += params.weightVol2;
				break;
			case 3:
				wik += params.weightVol3;
				break;
			default:
				break;
			}
			uncoveredRaw += ij + ik;
			totalPenalty += wij + wik;
			assert((wij == 0) == (ij == 0));
			assert((wik == 0) == (ik == 0));
		}
	}

	int shadowWeight(int numConnections) const {
		switch (numConnections) {
		case 0:
			return 0;
		case 1:
			return params.weight1;
		case 2:
			return params.weight2;
		case 3:
			return params.weight3;
		case 4:
			return params.weight4;
		default:
			assert(false);
			return -1;
		}
	}

	static int numConnections(const bool s[MAX_D][MAX_D], int i, int j) {
		if (!s[i][j])
			return 0;
		int c = 0;
		if (i > 0 && s[i - 1][j])
			c++;
		if (i < MAX_D - 1 && s[i + 1][j])
			c++;
		if (j > 0 && s[i][j - 1])
			c++;
		if (j < MAX_D - 1 && s[i][j + 1])
			c++;
		return c;
	}

	static void saveSnapshot(const Field& f1, const Field& f2, Snapshot& to) {
		int n = len(f1.bs);
		to.numBlocks = n;
		rep0(i, n) {
			auto& b = f1.bs[i].b;
			int numCubes = 0;
			for (auto& lpos : b.poss) {
				to.cubePoss[i][numCubes++] = lpos;
			}
			to.numCubes[i] = numCubes;
			to.blockTransforms[0][i] = f1.bs[i].tf;
			to.blockTransforms[1][i] = f2.bs[i].tf;
		}
	}

	static void loadSnapshot(Field& f1, Field& f2, const Snapshot& from) {
		f1.unplaceAll();
		f2.unplaceAll();
		int n = from.numBlocks;
		f1.pushPopBlocks(n);
		f2.pushPopBlocks(n);
		rep0(i, n) {
			auto& b1 = f1.bs[i].b;
			auto& b2 = f2.bs[i].b;
			b1.clear();
			b2.clear();
			int numCubes = from.numCubes[i];
			rep0(j, numCubes) {
				b1.add(from.cubePoss[i][j]);
				b2.add(from.cubePoss[i][j]);
			}
			f1.bs[i].tf = from.blockTransforms[0][i];
			f2.bs[i].tf = from.blockTransforms[1][i];
			f1.placeBlock(i);
			f2.placeBlock(i);
		}
	}

	void pushBlock() {
		bs.emplace_back();
	}

	void popBlock() {
		assert(!bs.back().placed);
		bs.pop_back();
	}

	void addCube(int index, const Coord& c) {
		auto& b = bat(index);
		b.b.add(c);
		if (b.placed) {
			auto p = b.tf * c;
			setCube(index, p);
			b.placedPoss.push_back(p);
		}
	}

	void removeCube(int index, const Coord& c) {
		auto& b = bat(index);
		int i = b.b.remove(c);
		if (b.placed) {
			auto p = b.placedPoss[i];
			unsetCube(index, p);
			b.placedPoss[i] = b.placedPoss.back();
			b.placedPoss.pop_back();
		}
	}

	void moveBlock(int index, const Transform& tf) {
		auto& b = bat(index);
		assert(!b.placed);
		b.tf = tf;
	}

	void placeBlock(int index) {
		auto& b = bat(index);
		assert(!b.placed);
		b.placed = true;
		for (auto& lpos : b.b.poss) {
			auto p = b.tf * lpos;
			setCube(index, p);
			b.placedPoss.push_back(p);
		}
	}

	void pushPopBlocks(int n) {
		while (len(bs) < n) {
			pushBlock();
		}
		while (len(bs) > n) {
			popBlock();
		}
	}

	void unplaceAll() {
		int n = len(bs);
		rep0(i, n) {
			if (bat(i).placed) {
				unplaceBlock(i);
			}
		}
	}

	void unplaceBlock(int index) {
		auto& b = bat(index);
		assert(b.placed);
		b.placed = false;
		for (auto& p : b.placedPoss) {
			unsetCube(index, p);
		}
		b.placedPoss.clear();
	}

	Grow randomShrink(int index, Rnd& rnd) {
		auto& b = bat(index).b;
		int numBlocks = len(b.poss);
		rep0(iter, 30) {
			auto i = rnd.nextInt(numBlocks);
			auto bpos = BlockPos(index, b.poss[i]);
			if (b.canBeEaten(bpos, {})) {
				return Grow(bpos, {}, {});
			}
		}
		return Grow();
	}

	static Grow randomGrow(Field& f1, Field& f2, int index, Rnd& rnd, bool allowEat) {
		int d = f1.d;
		auto& b1 = f1.bat(index);
		auto& b2 = f2.bat(index);
		auto& b = b1.b;
		assert(b1.placed && b2.placed);
		int numBlocks = len(b.poss);
		rep0(iter, 30) {
			auto i = rnd.nextInt(numBlocks);
			auto dir = rnd.nextInt(6);
			auto lpos = b.poss[i] + STEPS[dir];
			auto pos1 = b1.tf * lpos;
			auto pos2 = b2.tf * lpos;
			auto i1 = f1.iat(pos1);
			auto i2 = f2.iat(pos2);
			assert((i1 == index) == (i2 == index));
			if (i1 == index || i1 == WALL_INDEX || i2 == WALL_INDEX)
				continue;
			if (i1 == -1 && i2 == -1) {
				return Grow(BlockPos(index, lpos), {}, {});
			}
			if (!allowEat)
				continue;
			Coord lpos1;
			Coord lpos2;
			BlockPos eat1;
			BlockPos eat2;
			if (i1 != -1)
				lpos1 = f1.bs[i1].tf.imul(pos1);
			if (i2 != -1)
				lpos2 = f2.bs[i2].tf.imul(pos2);
			if (i1 == i2) {
				auto& b = f1.bs[i1].b;
				if (lpos1 == lpos2) {
					eat1.index = i1;
					eat1.lpos = lpos1;
					if (!b.canBeEaten(eat1, {}))
						continue;
				} else {
					eat1.index = i1;
					eat1.lpos = lpos1;
					eat2.index = i2;
					eat2.lpos = lpos2;
					if (!b.canBeEaten(eat1, eat2))
						continue;
				}
			} else {
				if (i1 != -1) {
					eat1.index = i1;
					eat1.lpos = lpos1;
					if (!f1.bs[i1].b.canBeEaten(eat1, {}))
						continue;
				}
				if (i2 != -1) {
					auto& eat = i1 == -1 ? eat1 : eat2;
					eat.index = i2;
					eat.lpos = lpos2;
					if (!f2.bs[i2].b.canBeEaten(eat, {}))
						continue;
				}
			}
			return Grow(BlockPos(index, lpos), eat1, eat2);
		}
		return Grow();
	}

	Teleport randomTeleport(int index, Rnd& rnd, bool allowEat) const {
		auto& b = bat(index);
		int numEmpties = len(empties);
		int numCubes = len(b.b.poss);

		rep0(iter, 30) {
			auto pivot = b.b.poss[rnd.nextInt(numCubes)];
			int moveToIndex = rnd.nextInt(numEmpties + numCubes);
			auto moveTo =
			    moveToIndex < numCubes ? b.placedPoss[moveToIndex] : empties[moveToIndex - numCubes];
			auto rot = rnd.nextInt(24);
			auto off = moveTo - rotate(rot, pivot);
			int leftNg = (int) allowEat;
			for (auto& lpos : b.b.poss) {
				auto pos = rotate(rot, lpos) + off;
				int i = iat(pos);
				leftNg -= i != index && i != -1;
				if (leftNg < 0)
					break;
			}
			if (leftNg < 0)
				continue;
			if (b.tf.rot == rot && b.tf.off == off)
				continue;
			leftNg = (int) allowEat;
			BlockPos eat;
			// can be possible, check again
			for (auto& lpos : b.b.poss) {
				auto pos = rotate(rot, lpos) + off;
				int i = iat(pos);
				if (i != index && i != -1) {
					if (--leftNg < 0)
						break;
					int checkIndex = rnd.nextInt(2);
					bool ok = false;
					rep0(iter, 2) {
						if ((iter ^ checkIndex) == 0) {
							// eat self
							eat = BlockPos(index, lpos);
							if (bs[index].b.canBeEaten(eat, {})) {
								ok = true;
								break;
							}
						} else if (i != WALL_INDEX) {
							// eat the other if possible
							eat = BlockPos(i, bs[i].tf.imul(pos));
							if (bs[i].b.canBeEaten(eat, {})) {
								ok = true;
								break;
							}
						}
					}
					if (!ok) {
						leftNg = -1;
						break;
					}
				}
			}
			if (leftNg < 0)
				continue;
			return Teleport(index, Transform(rot, off), eat);
		}
		return Teleport();
	}

	// teleport in f1, f2 may be affected by eating a cube
	static pdi teleportScoreDelta(Field& f1, Field& f2, const Teleport& tp) {
		int index = tp.index;
		int eatIndex = tp.eat.index;
		auto& b = f1.bs[index].b;
		int n = len(b.poss);
		int prevPena = f1.totalPenalty + f2.totalPenalty;
		bool eatSelf = eatIndex == index;

		rep0(i, n) {
			f1.unsafeUncover(f1.bs[index].placedPoss[i]);
			if (!eatSelf || tp.eat.lpos != b.poss[i])
				f1.unsafeCover(tp.tf * b.poss[i]);
		}
		if (eatIndex != -1) {
			if (!eatSelf)
				f1.unsafeUncover(f1.bs[eatIndex].tf * tp.eat.lpos);
			f2.unsafeUncover(f2.bs[eatIndex].tf * tp.eat.lpos);
		}

		int deltaPena = f1.totalPenalty + f2.totalPenalty - prevPena;

		rep0(i, n) {
			f1.unsafeCover(f1.bs[index].placedPoss[i]);
			if (!eatSelf || tp.eat.lpos != b.poss[i])
				f1.unsafeUncover(tp.tf * b.poss[i]);
		}
		if (eatIndex != -1) {
			if (!eatSelf)
				f1.unsafeCover(f1.bs[eatIndex].tf * tp.eat.lpos);
			f2.unsafeCover(f2.bs[eatIndex].tf * tp.eat.lpos);
		}

		double deltaInvSizeSum = 0;
		if (eatIndex != -1) {
			int size = len(f1.bs[eatIndex].b.poss);
			deltaInvSizeSum += 1.0 / (size - 1) - 1.0 / size;
		}
		return pdi(deltaInvSizeSum, deltaPena);
	}

	static pdi growScoreDelta(Field& f1, Field& f2, const Grow& grow) {
		int prevPena = f1.totalPenalty + f2.totalPenalty;
		unsafeGrowCover(f1, f2, grow);
		int deltaPena = f1.totalPenalty + f2.totalPenalty - prevPena;
		unsafeUngrowCover(f1, f2, grow);
		double deltaInvSizeSum = 0;
		int size = len(f1.bs[grow.pos.index].b.poss);
		deltaInvSizeSum += 1.0 / (size + 1) - 1.0 / size;
		int i1 = grow.eat1.index;
		int i2 = grow.eat2.index;
		if (i1 != -1) {
			if (i2 != -1) {
				if (i1 == i2) {
					int size12 = len(f1.bs[i1].b.poss);
					deltaInvSizeSum += 1.0 / (size12 - 2) - 1.0 / size12;
				} else {
					int size1 = len(f1.bs[i1].b.poss);
					deltaInvSizeSum += 1.0 / (size1 - 1) - 1.0 / size1;
					int size2 = len(f2.bs[i2].b.poss);
					deltaInvSizeSum += 1.0 / (size2 - 1) - 1.0 / size2;
				}
			} else {
				int size1 = len(f1.bs[i1].b.poss);
				deltaInvSizeSum += 1.0 / (size1 - 1) - 1.0 / size1;
			}
		}
		return pdi(deltaInvSizeSum, deltaPena);
	}

	static pdi ungrowScoreDelta(Field& f1, Field& f2, const Grow& grow) {
		int prevPena = f1.totalPenalty + f2.totalPenalty;
		unsafeUngrowCover(f1, f2, grow);
		int deltaPena = f1.totalPenalty + f2.totalPenalty - prevPena;
		unsafeGrowCover(f1, f2, grow);
		double deltaInvSizeSum = 0;
		int size = len(f1.bs[grow.pos.index].b.poss);
		deltaInvSizeSum += 1.0 / (size - 1) - 1.0 / size;
		int i1 = grow.eat1.index;
		int i2 = grow.eat2.index;
		if (i1 != -1) {
			if (i2 != -1) {
				if (i1 == i2) {
					int size12 = len(f1.bs[i1].b.poss);
					deltaInvSizeSum += 1.0 / (size12 + 2) - 1.0 / size12;
				} else {
					int size1 = len(f1.bs[i1].b.poss);
					deltaInvSizeSum += 1.0 / (size1 + 1) - 1.0 / size1;
					int size2 = len(f2.bs[i2].b.poss);
					deltaInvSizeSum += 1.0 / (size2 + 1) - 1.0 / size2;
				}
			} else {
				int size1 = len(f1.bs[i1].b.poss);
				deltaInvSizeSum += 1.0 / (size1 + 1) - 1.0 / size1;
			}
		}
		return pdi(deltaInvSizeSum, deltaPena);
	}

	static void teleport(Field& f1, Field& f2, const Teleport& tp) {
		int index = tp.index;
		int eatIndex = tp.eat.index;
		if (eatIndex != -1) {
			f1.removeCube(eatIndex, tp.eat.lpos);
			f2.removeCube(eatIndex, tp.eat.lpos);
		}
		f1.unplaceBlock(index);
		f1.moveBlock(index, tp.tf);
		f1.placeBlock(index);
	}

	static void grow(Field& f1, Field& f2, const Grow& grow) {
		if (grow.eat1.index != -1) {
			f1.removeCube(grow.eat1.index, grow.eat1.lpos);
			f2.removeCube(grow.eat1.index, grow.eat1.lpos);
			if (grow.eat2.index != -1) {
				f1.removeCube(grow.eat2.index, grow.eat2.lpos);
				f2.removeCube(grow.eat2.index, grow.eat2.lpos);
			}
		}
		f1.addCube(grow.pos.index, grow.pos.lpos);
		f2.addCube(grow.pos.index, grow.pos.lpos);
	}

	static void ungrow(Field& f1, Field& f2, const Grow& grow) {
		f1.removeCube(grow.pos.index, grow.pos.lpos);
		f2.removeCube(grow.pos.index, grow.pos.lpos);
		if (grow.eat1.index != -1) {
			f1.addCube(grow.eat1.index, grow.eat1.lpos);
			f2.addCube(grow.eat1.index, grow.eat1.lpos);
			if (grow.eat2.index != -1) {
				f1.addCube(grow.eat2.index, grow.eat2.lpos);
				f2.addCube(grow.eat2.index, grow.eat2.lpos);
			}
		}
	}

	static void saveSolution(const Field& f1, const Field& f2, Solution& to) {
		to.n = len(f1.bs);
		rep0(i, MAX_D) rep0(j, MAX_D) rep0(k, MAX_D) {
			to.indices[0][i][j][k] = f1.indices[i + LO][j + LO][k + LO];
			to.indices[1][i][j][k] = f2.indices[i + LO][j + LO][k + LO];
		}
	}

	void draw() const {
		mov.noStroke();
		mov.push();
		mov.translate(-LO, -LO);
		rep0(i, 64) rep0(jk, 64) {
			int numIJ = ijCounts[i][jk];
			int numIK = ikCounts[i][jk];
			int inD = i - LO >= 0 && i - LO < d && jk - LO >= 0 && jk - LO < d;
			bool silIJ = inD ? sil.ij[i - LO][jk - LO] : false;
			bool silIK = inD ? sil.ik[i - LO][jk - LO] : false;
			if (numIJ + numIK == 0 && !silIJ && !silIK)
				continue;
			mov.tooltip(to_string(numIJ));
			if (silIJ) {
				if (numIJ == 0) {
					mov.fill(0.5);
				} else {
					mov.fill(0, 1, 0);
				}
				mov.rect(jk, i, 1, 1);
			} else if (numIJ > 0) {
				mov.fill(1, 0, 0);
				mov.rect(jk, i, 1, 1);
			}
			mov.push();
			mov.translate(d + 4, 0);
			mov.tooltip(to_string(numIK));
			if (silIK) {
				if (numIK == 0) {
					mov.fill(0.5);
				} else {
					mov.fill(0, 1, 0);
				}
				mov.rect(jk, i, 1, 1);
			} else if (numIK > 0) {
				mov.fill(1, 0, 0);
				mov.rect(jk, i, 1, 1);
			}
			mov.noTooltip();
			mov.pop();
		}
		mov.pop();
		mov.textAlign(Movie::CENTER);
		mov.textSize(1);
		mov.fill(1);
		mov.text("uncov: " + to_string(uncoveredRaw) + ", penalty: " + to_string(totalPenalty), d + 2, d + 3);
	}

private:
	void setCube(int index, const Coord& c) {
		assert(inBounds(c));

		assert(iat(c) == -1);
		iat(c) = index;
		unsafeCover(c);

		int& i = emptyIndices[c.i][c.j][c.k];
		assert(i != -1);
		auto& pi = empties[i];
		auto& pb = empties.back();
		int& b = emptyIndices[pb.i][pb.j][pb.k];
		pi = pb;
		b = i;
		i = -1;
		empties.pop_back();
	}

	void unsetCube(int index, const Coord& c) {
		assert(inBounds(c));
		assert(iat(c) == index);
		iat(c) = -1;
		unsafeUncover(c);

		int& i = emptyIndices[c.i][c.j][c.k];
		assert(i == -1);
		i = len(empties);
		empties.push_back(c);
	}

	void unsafeCover(const Coord& c) {
		assert(inBounds(c));
		int i = c.i;
		int j = c.j;
		int k = c.k;
		int fij = (int) (ijCounts[i][j]++ == 0);
		int fik = (int) (ikCounts[i][k]++ == 0);
		uncoveredRaw -= fij + fik;
		totalPenalty -= fij * ijWeights[i][j] + fik * ikWeights[i][k] + penalties[i][j][k];
	}

	void unsafeUncover(const Coord& c) {
		assert(inBounds(c));
		int i = c.i;
		int j = c.j;
		int k = c.k;
		int fij = (int) (--ijCounts[i][j] == 0);
		int fik = (int) (--ikCounts[i][k] == 0);
		uncoveredRaw += fij + fik;
		totalPenalty += fij * ijWeights[i][j] + fik * ikWeights[i][k] + penalties[i][j][k];
	}

	static void unsafeGrowCover(Field& f1, Field& f2, const Grow& grow) {
		int i1 = grow.eat1.index;
		int i2 = grow.eat2.index;
		if (i1 != -1) {
			f1.unsafeUncover(f1.bs[i1].tf * grow.eat1.lpos);
			f2.unsafeUncover(f2.bs[i1].tf * grow.eat1.lpos);
			if (i2 != -1) {
				f1.unsafeUncover(f1.bs[i2].tf * grow.eat2.lpos);
				f2.unsafeUncover(f2.bs[i2].tf * grow.eat2.lpos);
			}
		}
		int i = grow.pos.index;
		f1.unsafeCover(f1.bs[i].tf * grow.pos.lpos);
		f2.unsafeCover(f2.bs[i].tf * grow.pos.lpos);
	}

	static void unsafeUngrowCover(Field& f1, Field& f2, const Grow& grow) {
		int i = grow.pos.index;
		f1.unsafeUncover(f1.bs[i].tf * grow.pos.lpos);
		f2.unsafeUncover(f2.bs[i].tf * grow.pos.lpos);
		int i1 = grow.eat1.index;
		int i2 = grow.eat2.index;
		if (i1 != -1) {
			f1.unsafeCover(f1.bs[i1].tf * grow.eat1.lpos);
			f2.unsafeCover(f2.bs[i1].tf * grow.eat1.lpos);
			if (i2 != -1) {
				f1.unsafeCover(f1.bs[i2].tf * grow.eat2.lpos);
				f2.unsafeCover(f2.bs[i2].tf * grow.eat2.lpos);
			}
		}
	}

	Placement& bat(int index) {
		assert(index >= 0 && index < bs.size());
		return bs[index];
	}

	const Placement& bat(int index) const {
		assert(index >= 0 && index < bs.size());
		return bs[index];
	}

	int& iat(const Coord& c) {
		return iat(c.i, c.j, c.k);
	}

	const int& iat(const Coord& c) const {
		return iat(c.i, c.j, c.k);
	}

	int& iat(int i, int j, int k) {
		return indices[i + LO][j + LO][k + LO];
	}

	const int& iat(int i, int j, int k) const {
		return indices[i + LO][j + LO][k + LO];
	}
};

class Problem {
public:
	int d = 0;
	Silhouette s1;
	Silhouette s2;

	Problem() {
	}

	void load(istream& in) {
		in >> d;
		s1.d = d;
		s2.d = d;
		repd2(i, j) {
			char c;
			in >> c;
			s1.ik[i][j] = c == '1';
		}
		repd2(i, j) {
			char c;
			in >> c;
			s1.ij[i][j] = c == '1';
		}
		repd2(i, j) {
			char c;
			in >> c;
			s2.ik[i][j] = c == '1';
		}
		repd2(i, j) {
			char c;
			in >> c;
			s2.ij[i][j] = c == '1';
		}
	}

	double perimeterRatio() const {
		int perimeter = 0;
		int volume = 0;
		constexpr array<Coord, 4> d4 = {
		    Coord(1, 0, 0),
		    Coord(-1, 0, 0),
		    Coord(0, 1, 0),
		    Coord(0, -1, 0),
		};
		rep0(t, 2) {
			auto& s = t == 0 ? s1 : s2;
			repd2(i, j) {
				if (s.ij[i][j]) {
					volume++;
					rep0(t, 4) {
						auto c = Coord(i, j, 0) + d4[t];
						if (!inBounds(c) || !s.ij[c.i][c.j]) {
							perimeter++;
						}
					}
				}
				if (s.ik[i][j]) {
					volume++;
					rep0(t, 4) {
						auto c = Coord(i, j, 0) + d4[t];
						if (!inBounds(c) || !s.ik[c.i][c.j]) {
							perimeter++;
						}
					}
				}
			}
		}
		double periStd = pow(volume, 2.0 / 3);
		return perimeter / periStd;
	}
};

class Selection {
private:
	vector<int> opts;
	vector<double> ws;
	double wsum = 0;

public:
	Selection() {
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

class SAStat {
public:
	int seed;
	int d;
	int n;
	vector<double> progs;
	vector<int> lefts;
	bool successful;

	SAStat(int seed, int d, int n) : seed(seed), d(d), n(n), successful(false) {
	}

	void addProgress(double prog, int left) {
		progs.push_back(prog);
		lefts.push_back(left);
		if (left == 0)
			successful = true;
	}
};

// #include "tables.h"
constexpr int USED_BLOCKS[5000] = {4, 7, 4, 9, 5, 5, 3, 4, 2, 6, 3, 4, 3, 4, 11, 9, 3, 2, 3, 7, 3, 3, 5, 3, 4, 6, 8, 7, 4, 8, 6, 3, 3, 6, 4, 4, 3, 8, 3, 3, 3, 3, 7, 4, 7, 3, 3, 4, 4, 3, 5, 3, 3, 6, 6, 5, 6, 5, 2, 3, 9, 4, 4, 15, 3, 3, 5, 5, 8, 4, 5, 7, 3, 4, 3, 4, 5, 2, 3, 4, 6, 6, 10, 5, 8, 5, 2, 3, 3, 4, 8, 4, 8, 3, 2, 4, 6, 7, 3, 5, 3, 3, 4, 5, 5, 3, 4, 5, 4, 2, 3, 4, 3, 5, 5, 4, 6, 10, 3, 14, 7, 3, 6, 4, 2, 3, 6, 7, 5, 4, 18, 5, 3, 4, 9, 16, 5, 4, 10, 3, 4, 4, 7, 9, 3, 4, 4, 7, 2, 2, 9, 3, 11, 3, 2, 6, 3, 6, 6, 3, 4, 5, 9, 4, 3, 5, 3, 5, 4, 6, 3, 4, 3, 5, 9, 6, 3, 3, 4, 3, 4, 3, 3, 4, 12, 3, 6, 2, 2, 8, 6, 5, 4, 3, 3, 4, 2, 5, 4, 5, 7, 6, 4, 3, 7, 3, 7, 10, 3, 4, 7, 4, 4, 3, 3, 10, 4, 4, 5, 3, 6, 6, 5, 6, 3, 4, 3, 2, 9, 4, 8, 3, 8, 6, 4, 3, 3, 5, 9, 4, 3, 4, 7, 2, 2, 3, 5, 3, 8, 3, 5, 4, 6, 5, 5, 15, 3, 6, 4, 4, 3, 6, 6, 5, 3, 4, 11, 4, 3, 4, 4, 6, 5, 4, 3, 7, 2, 3, 10, 3, 4, 5, 9, 2, 5, 5, 9, 8, 4, 5, 3, 5, 2, 10, 5, 6, 4, 3, 3, 3, 4, 3, 7, 5, 4, 9, 5, 3, 3, 4, 3, 3, 6, 4, 3, 4, 3, 4, 5, 5, 4, 4, 3, 3, 3, 3, 5, 3, 3, 9, 3, 4, 4, 3, 4, 5, 2, 5, 8, 3, 3, 4, 5, 3, 5, 3, 4, 3, 8, 5, 5, 5, 4, 3, 6, 4, 3, 4, 5, 4, 7, 5, 3, 3, 4, 3, 5, 6, 10, 3, 3, 8, 3, 3, 7, 3, 18, 12, 3, 3, 6, 4, 3, 3, 5, 7, 11, 3, 3, 3, 4, 6, 3, 10, 6, 5, 3, 8, 4, 3, 4, 10, 3, 3, 4, 10, 9, 4, 6, 5, 6, 3, 3, 9, 3, 5, 6, 11, 4, 3, 7, 4, 3, 5, 3, 3, 4, 3, 7, 3, 3, 6, 3, 3, 3, 5, 7, 6, 3, 3, 7, 9, 3, 3, 5, 7, 6, 3, 12, 9, 5, 4, 2, 3, 6, 4, 12, 6, 4, 4, 4, 3, 3, 5, 3, 4, 6, 3, 5, 4, 8, 12, 4, 5, 5, 3, 6, 3, 4, 3, 11, 8, 3, 3, 8, 3, 3, 9, 3, 3, 3, 4, 9, 2, 4, 2, 6, 3, 6, 3, 7, 5, 5, 6, 7, 5, 2, 4, 4, 10, 8, 4, 4, 6, 3, 4, 2, 4, 3, 4, 3, 6, 3, 4, 3, 3, 4, 9, 4, 8, 8, 4, 4, 9, 4, 3, 8, 4, 4, 9, 3, 3, 5, 5, 3, 3, 6, 5, 4, 6, 5, 4, 4, 4, 3, 3, 3, 8, 4, 2, 4, 3, 4, 3, 3, 3, 17, 3, 7, 3, 5, 8, 4, 3, 3, 4, 9, 18, 4, 3, 5, 7, 3, 7, 3, 8, 5, 3, 3, 3, 4, 3, 4, 3, 8, 10, 4, 4, 3, 4, 3, 3, 6, 6, 14, 6, 3, 4, 3, 4, 3, 4, 4, 5, 4, 4, 3, 6, 4, 3, 3, 5, 3, 6, 6, 3, 5, 4, 3, 5, 3, 4, 3, 3, 6, 7, 6, 2, 3, 5, 6, 14, 3, 5, 4, 3, 5, 4, 6, 4, 4, 3, 4, 11, 4, 6, 9, 4, 3, 3, 4, 7, 3, 9, 5, 3, 11, 7, 9, 4, 5, 6, 8, 5, 4, 3, 4, 2, 3, 4, 4, 9, 4, 3, 3, 5, 4, 4, 4, 6, 4, 3, 4, 5, 6, 4, 4, 13, 4, 3, 5, 6, 5, 3, 4, 3, 3, 5, 3, 4, 10, 6, 4, 5, 3, 13, 2, 3, 8, 3, 4, 4, 5, 5, 7, 5, 6, 5, 3, 5, 5, 6, 6, 2, 5, 10, 3, 3, 3, 5, 4, 5, 4, 2, 3, 3, 4, 3, 4, 4, 4, 3, 5, 3, 3, 6, 5, 2, 4, 7, 6, 4, 7, 4, 6, 3, 4, 4, 4, 3, 9, 17, 3, 5, 3, 7, 3, 3, 4, 4, 4, 3, 6, 6, 3, 2, 2, 3, 5, 3, 6, 8, 10, 4, 6, 4, 3, 3, 6, 13, 5, 2, 3, 4, 4, 6, 3, 5, 3, 4, 4, 4, 7, 10, 10, 4, 4, 5, 4, 7, 4, 8, 3, 2, 3, 7, 3, 5, 5, 11, 7, 14, 4, 5, 4, 6, 6, 4, 11, 6, 7, 9, 4, 4, 4, 20, 3, 5, 6, 4, 8, 3, 3, 4, 3, 4, 3, 4, 3, 9, 4, 9, 5, 3, 3, 3, 6, 7, 5, 5, 5, 3, 4, 11, 3, 3, 4, 10, 5, 3, 2, 3, 3, 2, 7, 4, 6, 5, 4, 4, 2, 4, 14, 3, 5, 4, 3, 7, 5, 5, 7, 8, 5, 3, 4, 6, 5, 3, 3, 5, 8, 2, 5, 3, 5, 3, 3, 3, 6, 6, 3, 5, 4, 5, 8, 4, 10, 6, 4, 8, 3, 5, 4, 2, 4, 2, 3, 4, 6, 7, 4, 4, 6, 3, 3, 5, 3, 3, 8, 3, 5, 3, 10, 4, 3, 5, 12, 4, 3, 4, 7, 3, 5, 3, 3, 12, 4, 5, 15, 8, 5, 4, 4, 4, 4, 3, 4, 10, 5, 6, 3, 3, 5, 6, 8, 9, 3, 3, 3, 4, 12, 6, 11, 6, 4, 6, 4, 4, 3, 14, 4, 3, 5, 3, 3, 2, 5, 4, 4, 4, 4, 4, 3, 3, 7, 4, 3, 3, 3, 3, 4, 3, 10, 8, 2, 3, 3, 4, 3, 6, 3, 3, 5, 5, 3, 3, 3, 4, 2, 6, 4, 5, 13, 6, 4, 6, 16, 3, 3, 7, 7, 3, 4, 3, 5, 2, 5, 9, 4, 3, 4, 3, 5, 4, 8, 3, 5, 3, 3, 4, 3, 3, 4, 3, 2, 3, 3, 2, 4, 4, 18, 5, 14, 5, 3, 4, 5, 4, 6, 2, 3, 5, 3, 2, 3, 10, 4, 7, 3, 4, 5, 4, 3, 4, 4, 4, 3, 6, 15, 12, 5, 3, 16, 5, 5, 13, 5, 2, 6, 3, 5, 7, 3, 3, 3, 3, 4, 5, 4, 11, 4, 3, 4, 5, 5, 4, 5, 3, 9, 5, 4, 3, 3, 8, 5, 4, 5, 3, 4, 4, 4, 3, 8, 6, 4, 3, 4, 7, 12, 5, 2, 5, 4, 5, 3, 6, 11, 6, 3, 15, 5, 3, 3, 5, 3, 5, 5, 6, 2, 3, 2, 4, 5, 5, 3, 4, 4, 5, 4, 6, 4, 6, 4, 5, 3, 4, 3, 3, 9, 3, 6, 3, 5, 4, 9, 3, 8, 9, 6, 13, 3, 4, 3, 3, 4, 10, 3, 3, 3, 3, 6, 3, 3, 2, 3, 4, 5, 4, 5, 6, 4, 3, 4, 3, 3, 9, 3, 3, 3, 6, 3, 10, 7, 9, 8, 3, 7, 4, 5, 4, 3, 2, 4, 5, 6, 3, 4, 3, 4, 3, 14, 4, 8, 5, 5, 6, 5, 3, 3, 4, 3, 3, 6, 4, 3, 3, 3, 3, 13, 4, 3, 4, 5, 3, 5, 7, 8, 4, 4, 3, 3, 3, 7, 3, 3, 5, 7, 11, 4, 4, 7, 8, 12, 5, 5, 5, 13, 2, 4, 3, 8, 4, 4, 4, 3, 7, 3, 4, 3, 3, 20, 2, 3, 5, 7, 9, 7, 4, 2, 3, 3, 4, 6, 2, 5, 4, 6, 4, 2, 4, 4, 6, 3, 7, 3, 12, 7, 3, 6, 3, 3, 3, 4, 10, 7, 2, 3, 7, 4, 21, 4, 9, 15, 3, 3, 100, 7, 7, 3, 4, 4, 3, 3, 6, 3, 2, 6, 4, 3, 3, 4, 3, 4, 5, 7, 10, 6, 9, 4, 5, 3, 6, 3, 3, 3, 5, 8, 4, 5, 3, 4, 3, 3, 3, 4, 6, 3, 4, 8, 3, 16, 3, 3, 5, 6, 3, 4, 4, 4, 3, 4, 7, 3, 3, 8, 5, 3, 4, 9, 12, 3, 4, 5, 3, 3, 5, 7, 8, 6, 2, 5, 5, 4, 3, 2, 4, 5, 3, 5, 5, 3, 4, 3, 5, 4, 3, 5, 8, 4, 4, 3, 5, 3, 7, 11, 5, 3, 3, 5, 8, 4, 14, 5, 4, 3, 3, 3, 3, 2, 5, 4, 5, 5, 4, 5, 3, 4, 3, 11, 3, 3, 3, 5, 4, 4, 3, 3, 3, 4, 3, 3, 6, 3, 5, 5, 4, 12, 6, 8, 3, 6, 3, 3, 3, 5, 3, 11, 2, 5, 13, 6, 4, 4, 4, 6, 4, 3, 4, 3, 3, 3, 7, 4, 5, 6, 9, 5, 6, 3, 3, 5, 2, 7, 3, 4, 10, 8, 4, 6, 3, 4, 3, 2, 4, 3, 3, 3, 4, 7, 3, 3, 4, 2, 8, 3, 4, 7, 3, 5, 12, 3, 3, 3, 3, 4, 5, 4, 6, 6, 4, 8, 3, 4, 4, 4, 3, 3, 7, 6, 4, 7, 6, 5, 5, 3, 4, 8, 4, 4, 7, 3, 4, 11, 3, 6, 5, 4, 4, 4, 4, 4, 8, 3, 18, 5, 3, 6, 6, 4, 2, 3, 4, 4, 3, 3, 4, 4, 5, 3, 4, 4, 6, 6, 10, 5, 7, 3, 4, 5, 8, 6, 3, 5, 6, 3, 5, 5, 4, 6, 6, 8, 5, 4, 5, 3, 4, 3, 3, 7, 4, 3, 3, 4, 3, 4, 4, 3, 8, 4, 6, 6, 5, 3, 8, 5, 3, 4, 3, 12, 5, 9, 6, 10, 4, 3, 3, 5, 8, 4, 12, 10, 5, 11, 4, 2, 4, 2, 4, 4, 5, 3, 5, 3, 5, 5, 5, 7, 4, 7, 4, 3, 3, 9, 6, 5, 9, 4, 5, 6, 3, 6, 4, 9, 3, 4, 4, 3, 4, 5, 4, 6, 5, 3, 2, 7, 4, 4, 4, 3, 4, 5, 5, 4, 7, 9, 6, 5, 7, 4, 4, 4, 3, 3, 3, 4, 2, 7, 4, 3, 2, 5, 3, 4, 4, 3, 6, 5, 13, 3, 4, 3, 4, 4, 5, 3, 4, 3, 7, 3, 3, 4, 6, 3, 7, 8, 3, 4, 3, 7, 5, 6, 2, 10, 7, 3, 5, 3, 3, 10, 5, 4, 5, 12, 3, 3, 5, 2, 6, 3, 5, 2, 4, 4, 3, 8, 8, 4, 5, 4, 3, 4, 5, 3, 6, 12, 5, 4, 5, 6, 3, 8, 3, 6, 6, 7, 5, 4, 17, 4, 5, 4, 4, 6, 3, 2, 4, 3, 3, 5, 15, 4, 5, 2, 3, 3, 3, 3, 4, 2, 3, 3, 3, 13, 3, 3, 12, 4, 6, 5, 3, 4, 4, 4, 11, 4, 5, 3, 4, 5, 5, 3, 3, 4, 5, 2, 5, 8, 3, 5, 3, 3, 3, 7, 5, 3, 6, 3, 4, 6, 3, 4, 2, 4, 3, 8, 3, 3, 3, 4, 3, 3, 6, 4, 9, 3, 5, 3, 8, 3, 5, 3, 3, 3, 2, 3, 6, 3, 3, 4, 4, 3, 3, 4, 3, 3, 3, 4, 6, 9, 3, 7, 4, 5, 3, 4, 3, 4, 5, 9, 4, 5, 4, 6, 9, 5, 4, 3, 5, 5, 5, 5, 5, 7, 3, 6, 3, 6, 3, 4, 7, 3, 4, 10, 3, 4, 2, 5, 5, 3, 2, 4, 5, 8, 9, 6, 6, 3, 5, 4, 4, 3, 4, 5, 3, 8, 5, 4, 3, 2, 6, 3, 8, 5, 4, 2, 4, 5, 4, 2, 10, 6, 3, 4, 3, 3, 2, 8, 3, 8, 4, 4, 5, 5, 4, 4, 4, 9, 14, 29, 6, 3, 10, 5, 5, 5, 2, 3, 8, 4, 5, 12, 7, 4, 11, 4, 3, 2, 4, 5, 9, 7, 6, 4, 12, 6, 6, 5, 4, 3, 4, 5, 8, 4, 3, 3, 5, 12, 2, 3, 5, 3, 6, 3, 7, 7, 6, 4, 2, 6, 4, 3, 4, 5, 4, 3, 15, 100, 2, 4, 5, 3, 4, 4, 7, 4, 3, 3, 4, 2, 6, 4, 3, 3, 12, 7, 3, 5, 3, 8, 3, 3, 9, 3, 3, 4, 3, 4, 3, 3, 7, 3, 5, 4, 5, 6, 5, 8, 4, 5, 7, 3, 6, 4, 3, 5, 4, 2, 5, 3, 4, 6, 7, 10, 3, 4, 5, 4, 3, 2, 4, 4, 5, 4, 4, 4, 3, 2, 4, 4, 4, 4, 10, 5, 3, 5, 4, 5, 5, 8, 4, 3, 4, 3, 3, 4, 2, 4, 3, 7, 3, 3, 6, 5, 5, 6, 3, 4, 4, 5, 2, 4, 4, 5, 3, 3, 2, 4, 6, 4, 4, 6, 3, 2, 3, 9, 5, 4, 9, 4, 4, 4, 4, 4, 4, 3, 8, 5, 3, 3, 13, 3, 3, 4, 5, 5, 5, 7, 4, 3, 3, 3, 4, 4, 4, 3, 3, 11, 3, 3, 5, 5, 3, 4, 5, 3, 2, 5, 3, 10, 3, 8, 8, 4, 3, 5, 5, 4, 10, 5, 3, 3, 6, 3, 8, 3, 3, 3, 6, 3, 8, 6, 2, 4, 10, 3, 4, 4, 5, 3, 4, 4, 4, 5, 7, 3, 5, 5, 4, 11, 6, 2, 5, 3, 3, 5, 3, 3, 4, 4, 4, 4, 2, 4, 7, 3, 3, 4, 6, 7, 10, 7, 5, 3, 4, 3, 4, 19, 3, 6, 11, 4, 3, 13, 5, 5, 3, 6, 3, 7, 8, 4, 4, 5, 3, 4, 4, 3, 5, 5, 8, 3, 6, 4, 6, 4, 3, 3, 4, 4, 6, 3, 5, 3, 4, 3, 3, 3, 6, 4, 4, 9, 3, 3, 11, 6, 5, 3, 4, 3, 3, 8, 4, 6, 6, 3, 2, 4, 9, 8, 8, 3, 5, 6, 17, 9, 4, 7, 10, 8, 4, 3, 3, 3, 3, 4, 4, 4, 6, 7, 2, 5, 9, 8, 4, 3, 2, 6, 14, 5, 3, 7, 3, 3, 4, 3, 4, 11, 3, 2, 3, 3, 3, 5, 3, 4, 6, 4, 6, 14, 6, 5, 6, 3, 8, 3, 5, 4, 3, 4, 6, 7, 8, 3, 3, 3, 3, 5, 11, 5, 3, 6, 13, 9, 8, 4, 2, 3, 7, 4, 7, 4, 3, 5, 4, 7, 5, 6, 6, 4, 2, 3, 4, 3, 11, 3, 6, 4, 9, 4, 4, 3, 3, 6, 8, 3, 5, 7, 7, 6, 4, 2, 6, 4, 6, 3, 3, 6, 8, 5, 3, 11, 4, 5, 8, 12, 3, 4, 3, 4, 7, 4, 6, 4, 5, 11, 5, 4, 4, 4, 9, 3, 8, 3, 3, 10, 17, 2, 4, 4, 9, 2, 4, 4, 3, 5, 3, 3, 7, 4, 5, 6, 4, 6, 4, 7, 3, 3, 3, 3, 5, 6, 8, 4, 6, 4, 4, 5, 4, 6, 8, 4, 4, 6, 3, 14, 9, 5, 5, 4, 6, 4, 7, 3, 2, 5, 5, 7, 3, 3, 3, 5, 3, 4, 4, 8, 3, 2, 3, 4, 3, 5, 9, 6, 4, 2, 2, 5, 9, 4, 3, 3, 3, 4, 6, 5, 6, 6, 3, 3, 3, 6, 4, 5, 3, 3, 6, 4, 3, 4, 4, 4, 5, 9, 5, 4, 4, 3, 4, 4, 3, 10, 8, 3, 6, 7, 3, 4, 6, 5, 4, 2, 3, 4, 5, 8, 3, 3, 4, 4, 3, 5, 3, 15, 5, 5, 3, 5, 4, 7, 6, 9, 5, 6, 6, 5, 2, 5, 4, 6, 3, 3, 5, 3, 3, 4, 3, 8, 5, 3, 4, 3, 4, 3, 4, 5, 4, 3, 5, 13, 3, 3, 4, 3, 4, 4, 11, 4, 4, 9, 4, 5, 4, 4, 8, 2, 4, 7, 2, 10, 6, 4, 4, 6, 3, 3, 4, 4, 5, 4, 3, 9, 4, 6, 11, 5, 5, 5, 4, 2, 4, 4, 4, 4, 5, 3, 7, 5, 3, 4, 4, 11, 4, 2, 4, 4, 3, 4, 4, 9, 3, 3, 3, 5, 19, 7, 4, 6, 3, 4, 4, 3, 4, 3, 7, 12, 6, 3, 3, 22, 3, 3, 5, 3, 3, 2, 4, 4, 16, 3, 7, 10, 7, 3, 3, 3, 2, 3, 8, 3, 6, 5, 5, 6, 3, 3, 3, 7, 5, 4, 3, 5, 7, 4, 5, 9, 4, 2, 4, 4, 13, 4, 3, 4, 9, 4, 4, 3, 8, 16, 4, 4, 7, 6, 5, 5, 3, 4, 10, 5, 3, 3, 7, 5, 3, 5, 4, 4, 3, 3, 4, 4, 4, 3, 7, 2, 5, 5, 4, 8, 5, 5, 3, 6, 4, 4, 4, 6, 4, 7, 6, 6, 4, 3, 3, 3, 4, 3, 6, 4, 2, 3, 2, 3, 6, 7, 4, 11, 7, 3, 3, 3, 6, 9, 3, 2, 8, 3, 4, 3, 8, 4, 5, 5, 4, 5, 5, 3, 3, 3, 3, 4, 5, 10, 3, 9, 5, 5, 4, 3, 4, 15, 3, 4, 6, 3, 14, 5, 11, 3, 3, 9, 4, 3, 7, 4, 2, 5, 6, 3, 4, 7, 3, 5, 6, 3, 4, 3, 3, 3, 2, 8, 4, 3, 6, 3, 4, 3, 3, 4, 3, 3, 3, 3, 4, 9, 4, 3, 3, 4, 5, 4, 8, 2, 3, 4, 4, 3, 5, 4, 3, 8, 3, 5, 17, 3, 3, 6, 5, 5, 100, 9, 3, 4, 3, 3, 6, 4, 4, 4, 3, 7, 9, 3, 7, 5, 3, 3, 3, 3, 6, 4, 4, 10, 8, 3, 3, 5, 5, 16, 7, 3, 3, 4, 4, 4, 3, 3, 4, 12, 6, 3, 3, 2, 7, 8, 8, 3, 5, 4, 16, 3, 2, 2, 3, 3, 3, 6, 6, 5, 6, 8, 8, 7, 5, 4, 3, 5, 5, 2, 3, 5, 4, 4, 3, 3, 6, 3, 7, 4, 3, 5, 7, 4, 9, 4, 3, 7, 11, 5, 4, 5, 2, 5, 4, 3, 7, 5, 4, 7, 4, 7, 4, 3, 8, 3, 3, 8, 3, 4, 3, 8, 3, 5, 4, 4, 6, 7, 4, 12, 6, 4, 4, 3, 3, 14, 3, 12, 3, 3, 3, 6, 5, 12, 4, 4, 10, 3, 7, 3, 12, 6, 8, 3, 6, 5, 4, 4, 3, 6, 14, 3, 7, 4, 10, 4, 6, 3, 5, 10, 4, 3, 5, 4, 2, 3, 8, 5, 6, 13, 5, 3, 4, 12, 6, 18, 3, 3, 13, 3, 17, 7, 6, 3, 7, 5, 4, 3, 3, 8, 7, 4, 4, 3, 3, 5, 4, 4, 3, 3, 3, 6, 8, 7, 5, 3, 11, 5, 4, 2, 4, 7, 5, 3, 3, 10, 4, 5, 5, 3, 4, 3, 4, 3, 4, 11, 3, 3, 3, 3, 2, 3, 7, 3, 6, 6, 3, 3, 10, 3, 3, 5, 3, 11, 3, 3, 3, 3, 3, 6, 5, 7, 3, 5, 6, 6, 6, 3, 5, 4, 10, 4, 4, 4, 4, 4, 7, 8, 2, 4, 4, 4, 5, 4, 11, 5, 4, 3, 8, 5, 11, 14, 4, 4, 3, 3, 6, 4, 4, 12, 3, 10, 5, 3, 3, 4, 5, 3, 4, 3, 2, 3, 4, 3, 5, 5, 5, 5, 3, 3, 5, 8, 3, 4, 5, 3, 3, 5, 6, 5, 4, 5, 7, 15, 5, 4, 11, 2, 3, 5, 3, 7, 3, 5, 4, 3, 4, 4, 6, 4, 3, 3, 2, 5, 5, 4, 5, 3, 3, 6, 6, 3, 11, 3, 3, 13, 8, 3, 4, 5, 3, 8, 5, 3, 11, 3, 6, 4, 3, 5, 4, 8, 6, 4, 7, 4, 4, 5, 8, 3, 4, 3, 2, 4, 3, 7, 4, 9, 9, 5, 3, 6, 3, 2, 3, 8, 8, 8, 8, 4, 2, 3, 8, 6, 4, 4, 4, 3, 2, 3, 5, 5, 4, 5, 4, 3, 3, 3, 4, 4, 3, 4, 7, 6, 3, 5, 3, 6, 4, 8, 8, 3, 5, 4, 5, 3, 6, 5, 3, 5, 4, 4, 3, 3, 5, 8, 2, 14, 3, 4, 4, 4, 4, 3, 3, 4, 3, 4, 3, 3, 6, 4, 3, 11, 19, 5, 3, 2, 4, 8, 4, 3, 4, 5, 5, 3, 6, 5, 3, 4, 5, 7, 7, 9, 7, 7, 6, 3, 3, 4, 7, 4, 5, 3, 7, 10, 6, 3, 5, 5, 4, 3, 10, 3, 7, 3, 4, 3, 6, 3, 6, 6, 5, 6, 2, 5, 4, 6, 3, 5, 4, 3, 3, 5, 5, 3, 4, 3, 5, 3, 4, 3, 3, 4, 4, 3, 5, 3, 6, 5, 9, 7, 12, 4, 4, 6, 2, 3, 14, 4, 2, 3, 3, 4, 8, 4, 4, 4, 3, 6, 7, 4, 14, 9, 4, 2, 4, 4, 5, 3, 3, 5, 6, 5, 5, 3, 9, 5, 4, 5, 5, 4, 3, 10, 9, 3, 3, 3, 8, 3, 3, 5, 4, 5, 3, 3, 6, 3, 3, 3, 6, 3, 4, 5, 3, 4, 5, 5, 2, 4, 5, 3, 4, 5, 3, 5, 5, 3, 14, 3, 4, 4, 3, 10, 3, 11, 3, 4, 4, 3, 3, 5, 5, 3, 6, 6, 3, 3, 6, 3, 3, 5, 5, 3, 10, 13, 3, 2, 5, 4, 4, 4, 5, 4, 5, 9, 6, 5, 4, 11, 4, 12, 3, 3, 4, 5, 4, 2, 3, 4, 9, 3, 7, 6, 8, 4, 4, 3, 8, 4, 4, 4, 4, 5, 3, 5, 9, 3, 3, 2, 7, 3, 4, 4, 3, 7, 4, 3, 3, 5, 3, 5, 3, 4, 3, 4, 3, 7, 8, 8, 3, 5, 4, 7, 6, 6, 5, 3, 6, 4, 7, 5, 3, 4, 4, 6, 6, 2, 3, 3, 4, 3, 3, 7, 6, 3, 4, 3, 7, 3, 2, 2, 3, 5, 4, 18, 2, 14, 5, 2, 4, 3, 3, 4, 8, 7, 4, 7, 5, 5, 3, 6, 5, 4, 12, 9, 4, 4, 7, 4, 4, 4, 3, 3, 4, 3, 3, 6, 3, 4, 8, 5, 2, 7, 7, 3, 4, 10, 5, 5, 8, 5, 10, 15, 3, 8, 2, 7, 3, 6, 7, 6, 2, 6, 4, 8, 4, 3, 6, 3, 6, 6, 5, 4, 4, 3, 3, 3, 4, 2, 8, 3, 3, 3, 4, 6, 6, 4, 2, 3, 3, 3, 4, 5, 5, 6, 3, 3, 4, 2, 2, 2, 3, 4, 5, 3, 3, 4, 7, 5, 6, 4, 4, 3, 3, 3, 2, 2, 7, 3, 3, 3, 9, 8, 6, 4, 5, 4, 5, 10, 5, 5, 16, 4, 5, 4, 3, 8, 4, 4, 3, 6, 3, 10, 3, 5, 9, 15, 6, 3, 4, 3, 3, 7, 4, 3, 4, 4, 4, 11, 4, 3, 3, 3, 9, 3, 5, 3, 4, 5, 8, 5, 7, 4, 4, 11, 3, 4, 6, 6, 5, 4, 3, 3, 6, 10, 5, 6, 4, 8, 5, 3, 5, 8, 4, 3, 4, 9, 5, 5, 4, 3, 3, 3, 3, 5, 3, 4, 4, 4, 5, 8, 6, 3, 3, 6, 9, 9, 5, 3, 3, 7, 3, 3, 5, 5, 3, 7, 7, 3, 4, 6, 4, 2, 4, 4, 4, 6, 7, 3, 9, 4, 3, 4, 5, 3, 6, 5, 8, 4, 3, 5, 5, 4, 5, 4, 4, 4, 3, 3, 5, 4, 5, 3, 5, 5, 4, 3, 3, 3, 10, 7, 4, 3, 4, 3, 5, 7, 5, 17, 3, 3, 4, 3, 6, 3, 4, 2, 4, 3, 7, 7, 6, 3, 6, 3, 5, 3, 3, 3, 4, 5, 8, 4, 3, 3, 4, 3, 4, 3, 3, 8, 4, 9, 11, 4, 6, 3, 3, 3, 3, 3, 4, 4, 4, 5, 3, 18, 7, 6, 2, 2, 3, 3, 4, 3, 3, 4, 7, 5, 4, 3, 3, 5, 3, 5, 4, 4, 4, 4, 5, 3, 3, 2, 4, 5, 4, 15, 5, 3, 3, 3, 5, 7, 4, 5, 6, 7, 5, 5, 3, 3, 7, 3, 3, 3, 4, 6, 5, 5, 5, 6, 4, 10, 2, 4, 3, 5, 4, 3, 4, 3, 4, 12, 4, 5, 5, 6, 10, 4, 3, 7, 3, 4, 3, 4, 6, 6, 9, 19, 9, 6, 4, 6, 6, 5, 3, 10, 5, 5, 3, 3, 11, 4, 5, 4, 3, 3, 3, 4, 3, 3, 5, 2, 3, 2, 9, 3, 3, 4, 6, 3, 3, 4, 4, 3, 13, 4, 4, 6, 7, 3, 6, 5, 5, 3, 3, 3, 6, 2, 4, 6, 4, 10, 3, 4, 4, 3, 7, 4, 6, 4, 4, 2, 3, 4, 4, 10, 3, 3, 9, 3, 4, 3, 4, 4, 4, 3, 4, 6, 3, 9, 5, 5, 3, 4, 3, 2, 4, 4, 11, 4, 3, 4, 3, 3, 3, 10, 4, 3, 3, 12, 8, 5, 4, 10, 4, 4, 3, 4, 2, 4, 5, 3, 3, 8, 4, 12, 10, 4, 4, 3, 6, 4, 4, 3, 3, 5, 4, 4, 5, 2, 3, 5, 4, 3, 3, 4, 3, 5, 5, 3, 4, 5, 6, 5, 3, 9, 4, 11, 3, 4, 3, 3, 5, 3, 4, 4, 3, 2, 2, 3, 4, 3, 14, 4, 6, 5, 3, 3, 4, 7, 4, 4, 5, 4, 4, 6, 4, 3, 4, 4, 3, 4, 8, 4, 4, 5, 4, 6, 3, 3, 4, 4, 6, 3, 3, 4, 4, 4, 4, 4, 4, 5, 10, 4, 3, 4, 4, 3, 7, 6, 3, 4, 6, 3, 2, 3, 4, 2, 3, 8, 4, 4, 3, 3, 15, 5, 4, 4, 4, 3, 4, 5, 3, 3, 4, 3, 12, 8, 6, 4, 4, 3, 4, 2, 3, 3, 8, 4, 4, 3, 3, 3, 3, 5, 5, 5, 2, 5, 3, 4, 4, 6, 4, 3, 12, 6, 6, 8, 6, 7, 4, 4, 5, 3, 4, 6, 6, 3, 13, 5, 3, 3, 5, 7, 3, 4, 3, 4, 7, 3, 5, 4, 4, 8, 4, 4, 7, 5, 12, 3, 3, 5, 3, 8, 5, 5, 3, 2, 5, 3, 6, 5, 4, 6, 4, 4, 3, 5, 3, 6, 2, 3, 6, 7, 4, 4, 2, 8, 3, 8, 3, 14, 8, 3, 4, 3, 3, 5, 11, 4, 4, 3, 3, 3, 3, 4, 4, 3, 7, 3, 2, 3, 6, 4, 6, 3, 6, 5, 5, 5, 3, 4, 4, 5, 6, 3, 6, 6, 3, 2, 4, 6, 3, 5, 3, 4, 3, 5, 3, 5, 10, 8, 2, 11, 7, 7, 4, 3, 4, 4, 6, 12, 4, 7, 5, 5, 4, 4, 5, 3, 4, 5, 3, 3, 5, 9, 5, 2, 3, 3, 5, 3, 3, 3, 4, 2, 2, 6, 7, 4, 3, 3, 5, 5, 12, 2, 5, 3, 4, 10, 3, 5, 4, 5, 5, 4, 13, 7, 10, 3, 5, 5, 6, 10, 8, 4, 4, 15, 5, 4, 3, 5, 6, 9, 3, 3, 4, 4, 3, 4, 3, 3, 3, 3, 5, 3, 5, 3, 4, 3, 9, 11, 6, 4, 3, 3, 7, 3, 4, 3, 7, 5, 6, 3, 3, 3, 3, 3, 3, 6, 3, 9, 6, 10, 3, 4, 5, 3, 5, 5, 5, 2, 3, 3, 2, 3, 4, 3, 4, 5, 4, 4, 6, 3, 4, 3, 4, 3, 3, 6, 5, 3, 7, 6, 3, 15, 11, 6, 3, 3, 21, 5, 3, 8, 9, 4, 4, 6, 4, 10, 10, 3, 6, 5, 6, 3, 4, 9, 5, 3, 3, 4, 7, 4, 4, 4, 10, 4, 3, 4, 4, 5, 10, 4, 5, 5, 4, 5, 7, 3, 4, 12, 9, 6, 6, 4, 3, 4, 4, 5, 4, 4, 5, 4, 3, 6, 7, 5, 4, 4, 5, 6, 5, 5, 7, 3, 4, 4, 7, 4, 14, 3, 4, 12, 10, 3, 4, 4, 3, 8, 4, 3, 4, 7, 3, 5, 3, 3, 2, 4, 4, 6, 7, 2, 4, 5, 2, 6, 7, 7, 4, 3, 4, 3, 7, 15, 2, 4, 5, 5, 6, 6, 3, 5, 6, 5, 3, 5, 4, 11, 5, 5, 4, 4, 4, 4, 4, 3, 5, 5, 13, 11, 4, 3, 4, 3, 4, 7, 4, 9, 4, 3, 7, 2, 6, 8, 4, 4, 4, 3, 3, 3, 2, 6, 3, 3, 10, 3, 3, 14, 4, 9, 5, 4, 3, 3, 4, 4, 4, 3, 3, 3, 4, 4, 3, 2, 9, 10, 5, 5, 9, 3, 5, 4, 4, 4, 8, 5, 4, 5, 3, 6, 4, 4, 5, 3, 8, 4, 11, 4, 3, 8, 4, 6, 6, 5, 3, 6, 2, 11, 4, 4, 3, 3, 8, 3, 5, 4, 4, 3, 3, 3, 5, 4, 5, 3, 3, 8, 3, 10, 2, 6, 3, 5, 10, 17, 8, 3, 4, 3, 4, 4, 5, 6, 8, 6, 4, 3, 4, 3, 4, 10, 7, 8, 5, 4, 4, 5, 3, 5, 5, 5, 5, 7, 5, 2, 10, 4, 4, 3, 3, 3, 3, 7, 5, 4, 8, 4, 2, 4, 5, 3, 7, 11, 4, 3, 12, 3, 5, 4, 3, 6, 3, 6, 3, 5, 4, 2, 4, 4, 4, 3, 3, 3, 4, 7, 3, 4, 10, 4, 5, 5, 3, 3, 4, 5, 4, 4, 11, 3, 4, 2, 6, 4, 3, 4, 6, 3, 4, 4, 3, 2, 5, 3, 5, 5, 3, 3, 5};
constexpr ull BASE_SCORES[5000] = {478968254, 946825397, 210129213, 553926146, 168102241, 125113151, 452380952, 461111111, 129166667, 538034188, 336111111, 152879528, 235171261, 140551860, 1334746626, 340570243, 94444444, 46634615, 434523810, 102978137, 428571429, 152882206, 691468254, 347222222, 99256554, 696836497, 495343219, 3042857143, 54434782, 411915901, 336192810, 113210445, 47883065, 248370443, 119772907, 816666667, 102500000, 314552999, 100000000, 43643322, 22249627, 250000000, 125689267, 200793651, 868589744, 583333333, 336111111, 98720397, 703968254, 133333333, 122148786, 155263158, 59131184, 547655123, 143814475, 666666667, 393358396, 631746032, 138095238, 299242424, 566149425, 866666667, 168623188, 1830296093, 40574463, 51442165, 156235953, 490010398, 220578194, 77329463, 106866297, 112639275, 93750000, 142857143, 452380952, 41013687, 129820393, 167832168, 58648317, 96399535, 155958389, 245298328, 840059940, 455089763, 574197513, 67198299, 190909091, 121078431, 491666667, 67094546, 452372042, 291941392, 152090984, 375000000, 183333333, 283318162, 142632731, 135167923, 102298851, 227033197, 245670996, 434523810, 213082751, 83603896, 507539683, 337301587, 488888889, 225042646, 95851426, 100000000, 88235294, 151111111, 64557338, 359401709, 515584416, 277777778, 182824039, 615878185, 185807656, 3126623377, 1105891331, 82625483, 239918064, 334498834, 202020202, 134387352, 168186389, 191135414, 1085714286, 106585082, 1258674629, 194796821, 28946622, 114285714, 377184972, 1706133121, 249646806, 77231068, 402493971, 136904762, 117697988, 87333506, 903169627, 320382182, 250000000, 140987141, 501190476, 94058245, 166666667, 182539683, 351646975, 736111111, 1644978632, 205555556, 187500000, 553647741, 410714286, 125758928, 98271076, 197324415, 49213335, 96170776, 323925330, 141909814, 55423197, 125641026, 533333333, 61105263, 65127461, 300137288, 235746606, 216666667, 315909091, 119074629, 790277778, 228556833, 378968254, 42026862, 64091674, 28429738, 91063425, 100000000, 344537815, 138535695, 1787141045, 476190476, 92064038, 162337662, 28612013, 220118111, 161198149, 93250690, 237990196, 282539683, 307539683, 202631579, 174242424, 88035431, 147142857, 214515557, 258319106, 834523810, 134526353, 294444444, 1031746032, 157894737, 412655326, 633503017, 509523810, 144444444, 415447154, 636446886, 119106520, 60897436, 353968254, 917287785, 1027777778, 837301587, 63991523, 253846154, 140462315, 120223837, 827020202, 703968254, 145238095, 37587284, 243589744, 201923077, 597628249, 263585434, 441700731, 58204051, 247697231, 1083766234, 321428571, 126811594, 237179487, 251771896, 300391782, 103983842, 67319537, 87308342, 672222222, 80000000, 200000000, 88235294, 1559523810, 69813958, 384274825, 257575758, 657575758, 304761905, 494140990, 71754537, 66610276, 3305158730, 458333333, 121485219, 355089763, 626190476, 179615706, 92888316, 701680672, 175766129, 22157024, 115126050, 1353943279, 617857143, 402777778, 272756410, 79360207, 158605974, 192196210, 59963668, 420634921, 743925519, 190909091, 453968254, 401769481, 396825397, 488034188, 1083333333, 1344047619, 66666667, 686446886, 309695274, 1087301587, 254204720, 52604611, 86562937, 91804355, 75805995, 105128205, 664930343, 700000000, 149199645, 40038959, 452380952, 187500000, 152882206, 43245789, 61224490, 353721741, 285732323, 178557505, 376214360, 71214475, 491666667, 500000000, 866666667, 206442577, 177631579, 773809524, 124920635, 63873626, 401923077, 103219697, 400432900, 296666667, 62238405, 141058941, 390054390, 56382472, 204761905, 204761905, 428571429, 341208791, 410714286, 92171717, 338288400, 409523810, 59834892, 66866927, 392857143, 122734255, 102768090, 194444444, 223086124, 372049689, 67676768, 63405797, 601190476, 267553688, 94758065, 379120879, 70874862, 38407034, 83548665, 788431013, 70239537, 53005394, 159602580, 138566068, 98924731, 660689311, 153530378, 144841270, 54615255, 216687901, 33599969, 327285068, 917857143, 82079343, 97849462, 300595238, 274853801, 690909091, 600432900, 437350338, 125000000, 611111111, 200580067, 30801172, 72921979, 239691528, 160818713, 1511638362, 476064583, 272727273, 246857717, 69531202, 310689311, 108465608, 165131579, 48827922, 225827600, 636127222, 61946290, 375000000, 84920635, 62249190, 181945501, 210256410, 321060179, 332981779, 504797980, 195833333, 1066399287, 34406526, 342857143, 86031452, 1004612629, 271464646, 333333333, 38659075, 694816645, 748629149, 775000000, 97020755, 198542909, 366655697, 450000000, 53896104, 677319902, 420634921, 493228993, 165481703, 576328696, 39473034, 84973166, 470848596, 177865613, 292832168, 187120121, 237179487, 452380952, 390909091, 24459321, 1030952381, 544444444, 75031270, 131293936, 81831832, 113675305, 108225108, 221275253, 218363729, 88985544, 46146953, 25664303, 793686869, 498619858, 153174603, 311111111, 381417602, 106531479, 390142211, 195833333, 1032952675, 287730415, 267880268, 214826840, 30202822, 225274725, 159621516, 159069659, 607049605, 139311193, 541666667, 785714286, 344877345, 409523810, 101149425, 239244663, 290909091, 89393939, 314752791, 311111111, 123928109, 267600820, 506587302, 910600408, 40279170, 60557549, 330778372, 197712418, 176737128, 230769231, 336580087, 410714286, 805573593, 169731740, 171428571, 112535613, 223272560, 91856061, 361111111, 237852599, 98809524, 23990493, 95312252, 184343434, 535617015, 53342817, 1083333333, 105263158, 383187135, 340909091, 138851628, 70475334, 199248092, 451129426, 763492063, 482646983, 793686869, 156488989, 22800881, 126679081, 47583558, 593461741, 872222222, 79245530, 866666667, 213732296, 27881688, 33419434, 182539683, 98780488, 161533816, 40923721, 107756813, 164037414, 173955108, 85987745, 44362603, 158609840, 64390706, 354425636, 250083250, 536742424, 1794444444, 195705172, 142972350, 1031746032, 37038963, 590909091, 430252101, 163333333, 48570874, 738053510, 53258145, 411111111, 57397648, 149994820, 315909091, 117380952, 272095235, 170172154, 474242424, 674873737, 212619788, 326923077, 82970489, 334498834, 100000000, 240665767, 90964795, 727906304, 79278813, 119047619, 193073593, 89400922, 107142857, 76543935, 82130273, 31748027, 992012106, 433333333, 133414817, 96774194, 232429995, 299029297, 40536719, 55232449, 157894737, 145695620, 261059284, 1398681190, 569444444, 144904115, 193960114, 333157426, 90017825, 633473879, 25953140, 186503876, 327777778, 141666667, 369047619, 108508733, 74482823, 114998057, 76102402, 61739130, 188988401, 1179336350, 367832168, 683333333, 392857143, 569444444, 428571429, 625000000, 324684874, 504336350, 840687994, 201179264, 324675325, 766666667, 229166667, 111154011, 410714286, 139738134, 38490893, 180035651, 950000000, 40659906, 195833333, 371323529, 338034188, 75031270, 209523810, 137466835, 307539683, 577380952, 520209366, 29894589, 97708314, 292156863, 176923077, 962301587, 70874862, 450000000, 210256410, 283823529, 356442577, 1002777778, 148291343, 226190476, 86554622, 82210918, 227899413, 697855235, 132411067, 89783223, 185978261, 257575758, 735714286, 112037247, 474242424, 285714286, 89929481, 277777778, 38115782, 518459735, 51994121, 269668616, 375616482, 402319902, 50552518, 66723953, 80882353, 832002208, 91740823, 253935586, 58421600, 136551854, 417787486, 138929340, 1734523810, 36222934, 65959350, 631875477, 157080603, 76829155, 397727273, 509523810, 167242543, 38838612, 243589744, 155242905, 144271118, 624842130, 188311688, 90522876, 491666667, 183862434, 72050172, 103342817, 194316165, 237205357, 130107527, 55592992, 411446886, 79035141, 253130154, 81453639, 257687166, 672944617, 80185185, 79658606, 64679028, 612179487, 52221289, 290909091, 413034188, 267832168, 39017087, 869047619, 273448773, 750000000, 455815626, 603113553, 87133523, 472655123, 420634921, 880882308, 200000000, 216346154, 2005555556, 50033670, 65024165, 245833333, 172616041, 291258170, 546717172, 299107143, 173009397, 308937773, 69767442, 530555556, 586446886, 583964646, 958333333, 116666667, 909523810, 684376668, 410714286, 526923077, 68275927, 75450693, 163210445, 82104434, 816666667, 171428571, 176785714, 37221218, 61701539, 349206349, 396367521, 258333333, 125061095, 181798246, 305607769, 209523810, 509523810, 95943513, 53610270, 181818182, 394934641, 122455156, 959956710, 231601732, 148160783, 37188739, 975490196, 29762461, 278846154, 37096311, 602777778, 126956522, 471096569, 1301686821, 183823529, 1366666667, 290909091, 117442557, 65631741, 196221323, 700000000, 539682540, 58332715, 39346847, 76398576, 296793601, 70256188, 209523810, 105263158, 221428571, 89699074, 90204678, 314627124, 569497563, 1265782828, 114505305, 1117857143, 174603175, 476190476, 123333333, 774242424, 847422893, 107271806, 49819928, 257575758, 367832168, 89393939, 783705184, 99123303, 89898220, 267832168, 816666667, 521825397, 342261905, 231052804, 448907881, 489597258, 350534188, 591666667, 628787879, 633333333, 285627186, 34622736, 907739000, 37147848, 125000000, 416666667, 492752101, 55839727, 282572751, 555648926, 884202732, 177832586, 1314014580, 57261593, 333549784, 392156863, 104788622, 706944444, 152421652, 404518759, 133707010, 398210083, 578726829, 110360360, 339260739, 743589744, 2036001093, 52666228, 170663862, 146076326, 759523810, 160519982, 281818182, 115384615, 35673941, 95766129, 41470906, 517857143, 90237731, 38307526, 254292841, 109211672, 708623188, 125379118, 361111111, 125000000, 176470588, 77967898, 116403966, 1338095238, 80503291, 98854619, 267832168, 126420891, 1147294372, 37507766, 148187135, 38615020, 291770262, 77485283, 31705182, 202020202, 400432900, 116923077, 153846154, 153246201, 120710590, 354318035, 801190476, 39411708, 184575569, 70197044, 415873016, 917146755, 92919829, 178552929, 39073064, 121666667, 253653646, 127733060, 119721579, 238400787, 681530866, 79712638, 66205534, 188382376, 953267974, 1119047619, 102298851, 166666667, 575000000, 373237449, 86956522, 73241424, 212500000, 315767974, 366666667, 347222222, 233333333, 391346079, 184020816, 43760711, 119740194, 128410652, 376190476, 334006090, 50397406, 578361345, 168544854, 188311688, 1261904762, 87394958, 801190476, 476623377, 13447541, 47432850, 182539683, 278846154, 345833333, 80889895, 108399988, 80585697, 350490196, 174341651, 102977667, 156573499, 241347935, 37472660, 23899950, 246458633, 52827607, 702380952, 29945622, 1704820131, 56955715, 225274725, 156992125, 787373166, 114430014, 132575758, 46012676, 488778765, 224242424, 67686717, 500000000, 24076193, 808480861, 125896900, 878571429, 1157133450, 198067439, 326473423, 36598907, 272408964, 63437940, 759523810, 416666667, 147164897, 357507999, 761111111, 411161192, 142857143, 75782410, 255980861, 122698119, 339230425, 300774761, 83206815, 151665395, 219780220, 161939799, 785725005, 201662172, 552155614, 445958134, 233031674, 157504127, 353846154, 245833333, 45520581, 684356389, 646825397, 477777778, 588492063, 36886480, 60897436, 156565657, 107700859, 1033333333, 32900433, 66909500, 63594088, 44477240, 142512077, 237179487, 1292857143, 63918231, 164086687, 195488722, 155555556, 225274725, 53738274, 107142857, 875639156, 235630111, 166666667, 391666667, 392857143, 40748897, 47068230, 186509786, 203418803, 466666667, 90699875, 53430485, 155263158, 215018315, 77631579, 77591999, 93478261, 191076165, 458333333, 194853129, 977710585, 329004329, 172631579, 389017415, 1493331186, 48571429, 191666667, 650865801, 98641104, 143073593, 35948765, 152631579, 188900748, 90000000, 121770400, 624013106, 892857143, 69003868, 247420021, 105911330, 182841344, 37983323, 313249898, 378968254, 78192115, 187990196, 292832168, 41085594, 56254368, 33265334, 143369176, 434523810, 31746032, 73780488, 225274725, 28571429, 378846154, 452020202, 6166666667, 115530588, 1507478632, 80129146, 152631579, 93602134, 174093722, 496825397, 136314056, 68027211, 83573854, 130335631, 56966618, 187500000, 30369913, 559726245, 635714286, 237609847, 235653236, 56059219, 256824126, 75539134, 48122160, 244179894, 307575758, 61830921, 286446886, 103845312, 1356580642, 376367502, 147727102, 273109244, 1648189311, 371428571, 185438302, 1099502018, 837301587, 129166667, 440324892, 388888889, 846825397, 525000000, 322222222, 65983087, 251165501, 53258145, 448412698, 85006219, 113628334, 498416480, 545634921, 420634921, 833333333, 201149425, 357255245, 444444444, 69070462, 700000000, 273219768, 73759151, 70959596, 26640103, 78272605, 900490196, 55676125, 46206968, 76612644, 444444444, 775000000, 447222222, 157165242, 125757576, 192613893, 121386017, 244298246, 392857143, 54796408, 155475342, 801747169, 215035613, 81666667, 178350358, 133854385, 138179379, 99268548, 530036630, 988391578, 611236802, 69767442, 1478391681, 186781609, 333766234, 485714286, 624145299, 63168449, 294775132, 64438061, 977777778, 125000000, 44444444, 90000000, 926190476, 384004884, 101735057, 117552335, 51885249, 37405758, 959523810, 38322625, 94300153, 374242424, 94639680, 451190476, 185581660, 187990196, 37513579, 416666667, 561111111, 1073448773, 433333333, 455357143, 31257813, 469047619, 120483193, 499074156, 442857143, 162107044, 571532999, 116296518, 794202511, 207175178, 180030115, 147619048, 199145299, 83622781, 685729847, 315018315, 402777778, 230769231, 416666667, 1695270416, 258741259, 333333333, 176923077, 97142857, 493589744, 422817460, 88998087, 86246957, 241225618, 172828623, 402777778, 58725791, 476190476, 347222222, 825119251, 283333333, 219780220, 146929825, 1394444444, 48755027, 297938859, 605394123, 621802043, 1176984127, 416666667, 388047138, 207738095, 346323529, 40733467, 67676768, 214285714, 53136578, 130940657, 91397371, 273448773, 362276612, 176984127, 101742160, 265151515, 2024238670, 158461538, 399104846, 991666667, 55539899, 1616666667, 496857717, 23870822, 216666667, 90831965, 452380952, 336111111, 99133540, 61735581, 434523810, 410714286, 485714286, 153174603, 1786111111, 301238045, 83624219, 286446886, 200392410, 30249755, 63823556, 248313395, 269999648, 52729952, 1050000000, 288034188, 38803136, 417832168, 639393939, 485714286, 123916084, 313492063, 210306220, 573298649, 242647059, 82357131, 992099567, 145358649, 1171629248, 450371850, 183646953, 376190476, 838414260, 181818182, 602777778, 101111111, 1896067821, 239379085, 139610964, 49497604, 361111111, 726461039, 140692641, 300432900, 333333333, 72009292, 2018196101, 36774629, 316468254, 74271353, 321573499, 925129446, 393452381, 188995215, 22654957, 81914245, 39187148, 950000000, 669877345, 183823529, 430409357, 44709854, 142244045, 325274725, 202020202, 32548742, 45828769, 608008658, 156909654, 668986569, 132411067, 1130065693, 1098412698, 42911011, 225243148, 72009292, 43635844, 100014569, 118883088, 781197969, 259123169, 182539683, 206089744, 568560606, 71008827, 2001547472, 200717703, 521015483, 1359482081, 434523810, 420634921, 13619047619, 195797652, 213155929, 33978746, 167289138, 299242424, 142156863, 73780488, 107707005, 220512821, 174242424, 152963434, 247712418, 378968254, 285256410, 413034188, 55882353, 675000000, 61891623, 528907204, 4450000000, 127428573, 491306884, 68301722, 78926282, 72009292, 423351648, 377777778, 292832168, 92803030, 176478009, 495424020, 330169830, 652777778, 155806182, 82210031, 21749854, 81081081, 458333333, 221230159, 154717964, 37648099, 50868711, 280860851, 86312119, 1811540959, 767857143, 184722222, 671367521, 521230159, 217261905, 545634921, 809523810, 33476384, 229166667, 50420939, 1197691198, 245670996, 111111111, 386540788, 102881218, 402777778, 187446718, 228631119, 928722797, 47253367, 123106061, 89592613, 217261905, 79195049, 238966983, 854285714, 213942329, 1042857143, 198412698, 850432900, 82737388, 418650794, 327020202, 174242424, 82231412, 57673094, 187500000, 205327080, 59614660, 104112554, 118589254, 420634921, 94315559, 109649123, 242857143, 133096396, 157154073, 77208995, 75976424, 65700483, 1059523810, 476190476, 172937972, 817767000, 73636992, 260256410, 559523810, 147058824, 390811966, 50685560, 810708272, 77062894, 89754857, 36951140, 467857143, 33654171, 434523810, 208333333, 76679204, 150997151, 68528951, 605158730, 152547920, 51234725, 237179487, 43464560, 458333333, 498114591, 233766234, 25061472, 42778036, 75825741, 43172542, 67733996, 181573499, 166666667, 70404937, 351165501, 76956815, 119047619, 144193509, 434523810, 278530378, 542735043, 194195964, 667451726, 490231990, 866666667, 361111111, 194311169, 92239422, 254761905, 191666667, 159274194, 152631579, 359492017, 181818182, 71101505, 1187796877, 114498349, 225027233, 35956568, 35128831, 441269841, 213450292, 84996305, 851190476, 400432900, 81038552, 467857143, 125162142, 444877345, 378927203, 168396930, 700923097, 825000000, 240554284, 333333333, 266666667, 309740260, 52631579, 130480091, 51515152, 517857143, 323893314, 218513319, 405952381, 549603175, 47280035, 105072464, 452380952, 93073593, 397727273, 159074156, 173202614, 47363899, 36467232, 365907643, 164801790, 180555556, 106735370, 100000000, 195769041, 75358852, 55896913, 147916849, 44354582, 81450402, 1050707626, 248484848, 509523810, 180147059, 434523810, 365909091, 365726817, 134680135, 288701190, 177906478, 405555556, 147570944, 79945799, 385353535, 46108022, 79723051, 26196172, 173955108, 245124464, 248736197, 235119048, 689700937, 454494525, 70948372, 122819180, 281818182, 65304991, 230804537, 520634921, 364260739, 190812617, 233333333, 137849462, 359525017, 71964956, 88393722, 933333333, 759523810, 392929293, 94684385, 98799520, 496825397, 159550662, 385714286, 1491149727, 399732620, 840909091, 114700823, 151273141, 50785527, 167832168, 500000000, 97702849, 69611529, 225274725, 21690739, 511544012, 215700483, 125849589, 47901003, 202631579, 86101028, 79211321, 926190476, 295555868, 181086729, 619886546, 509523810, 775000000, 599145299, 1122294372, 96560570, 402777778, 245409357, 222222222, 39012346, 68803023, 137083325, 47975510, 125241539, 389139515, 369256717, 147422481, 107582583, 513131313, 40439560, 61672314, 500000000, 382575758, 235584411, 32823684, 128623188, 452380952, 767857143, 428571429, 106871345, 80476560, 467857143, 629072682, 642156863, 424891775, 124043248, 1125000000, 31988669, 228176086, 985353535, 69090519, 73876770, 65086120, 830323326, 141696477, 603227722, 251494455, 326029423, 547619048, 378968254, 266666667, 60826118, 291411088, 50669424, 634298064, 456556980, 590079365, 496748869, 319913420, 194444444, 597222222, 19149716, 513888889, 448051948, 89620281, 75000000, 102219350, 282539683, 201539855, 476165501, 358537296, 883333333, 46220494, 524586421, 41533643, 434523810, 367857143, 1063622489, 136293595, 66474681, 408082789, 221813725, 230654001, 225990189, 33272569, 90487136, 216374269, 787029126, 94819159, 44512258, 57765152, 569444444, 725000000, 243795094, 321161192, 160646384, 723484848, 119047619, 18721700, 146815144, 65313632, 166666667, 71331981, 329004329, 139162562, 87212302, 68932437, 421703297, 580899553, 574716804, 640054390, 143212917, 268432863, 74318496, 62721807, 303174603, 200595238, 24534886, 69858713, 652380952, 174242424, 486948140, 60800598, 452380952, 67972350, 108055556, 68181818, 138000405, 281579940, 132411067, 147002310, 72430622, 620697957, 59230769, 290909091, 153095238, 333766234, 41379623, 260526316, 69033101, 111904762, 47504708, 181010869, 153174603, 87062869, 35823698, 146000175, 183823529, 585653236, 173381255, 53386912, 151305684, 34146906, 641363212, 325110211, 139640298, 214285714, 375797824, 241219242, 60577743, 261717512, 457575758, 59078309, 1209559885, 75391370, 34936683, 481818182, 847321959, 40186721, 85714286, 636204482, 38856305, 116078122, 230252101, 125955136, 181818182, 775000000, 72101339, 65488565, 369558847, 724044012, 902777778, 166151570, 143223443, 176691729, 439285714, 47764278, 361111111, 424853801, 3652380952, 91986745, 149572650, 990909091, 633333333, 109788360, 321106696, 176923077, 872619048, 653113553, 719298246, 155106209, 260964912, 1609835894, 582575758, 566666667, 77819122, 575000000, 897222222, 267857143, 183333333, 427020202, 97384115, 95766129, 186805556, 1009351121, 368686869, 93745498, 94298246, 388888889, 54499072, 216666667, 152631579, 103238866, 81666667, 50037793, 461111111, 525000000, 1025208898, 35267202, 138528139, 425591560, 151111111, 747619048, 888034188, 28438893, 268347339, 32776027, 284715285, 1057615995, 900000000, 66950245, 120000000, 170289855, 449242424, 325137363, 38840051, 195833333, 601190476, 300183242, 117647059, 1652380952, 610299831, 83918053, 60617761, 336111111, 169934641, 60159593, 1012500000, 340371148, 98823529, 76723319, 153121775, 106402608, 370698297, 71002710, 90197044, 40816327, 733333333, 24898466, 424051854, 48683285, 80457832, 32504014, 750000000, 358766234, 50494743, 335785953, 53488480, 749497676, 59259259, 58621370, 378968254, 196627817, 476190476, 126282051, 219079940, 361111111, 139471639, 200000000, 46019847, 593358396, 410714286, 214285714, 109017259, 759523810, 160818713, 215018315, 108573854, 51328861, 83086226, 87394958, 56200653, 143409015, 1154376179, 104070004, 683964646, 741666667, 58288009, 84889643, 116857652, 78309078, 61922922, 76263295, 393407012, 82130230, 223673066, 40600331, 88224904, 2009523810, 53391557, 552380952, 110442774, 48912818, 842857143, 852380952, 143481868, 223320158, 288136867, 285256410, 93157886, 158730159, 322939926, 476190476, 245833333, 109098352, 244318182, 685714286, 537404115, 158823529, 100787891, 69411765, 402281929, 278378185, 87813620, 233766234, 45712981, 152838873, 346502716, 863569764, 203891433, 187467750, 342857143, 121980256, 217460317, 291208791, 108465608, 41473444, 94851240, 361111111, 240639542, 54213667, 726190476, 420634921, 183333333, 398913952, 434523810, 402674254, 62993416, 390909091, 111111111, 200717703, 241998324, 372727273, 194444444, 1053373921, 679458450, 30949140, 38026021, 283333333, 117816092, 190909091, 567326546, 140700483, 1727777778, 53431966, 103277027, 71262128, 158522727, 57029043, 39680069, 42977067, 807272303, 647658028, 12188095238, 114364207, 33608583, 528960114, 706349206, 271416084, 158678955, 181818182, 109788360, 402252791, 87073233, 876190476, 664329108, 633445721, 87439614, 2757575758, 90909091, 71474359, 76923077, 240397028, 114255773, 619365722, 195912186, 101235208, 126560888, 428446533, 125509081, 120131303, 68826413, 490079365, 477777778, 81006406, 112254627, 280356632, 141625616, 302020202, 69238901, 111872873, 846051068, 51282051, 49726776, 108417508, 61309524, 931623932, 94162826, 252359561, 718228993, 534294872, 52814912, 60846561, 746825397, 198086124, 157894737, 113352827, 84105592, 46980586, 322222222, 1037244818, 23619047619, 250000000, 34732307, 274777854, 143724696, 120126884, 156251623, 122778366, 87750955, 107905983, 385714286, 165331658, 202020202, 218253968, 478968254, 276923077, 298611111, 779454941, 149062228, 173202614, 113801359, 433333333, 595799522, 74179037, 212500000, 816280941, 138528139, 115000000, 143369176, 161111111, 42793001, 467857143, 72772722, 159348099, 410714286, 410110211, 204061625, 150555255, 107226512, 287426185, 534880952, 440909091, 70782484, 118810182, 333766234, 222498632, 195238095, 100840336, 96822944, 35270147, 194444444, 64327593, 48131080, 35465689, 569852370, 3376190476, 356385438, 467857143, 214285714, 109281646, 417929293, 47403058, 34008502, 155310150, 196172249, 615079365, 106585082, 89019274, 426129426, 205194805, 174242424, 507575758, 576923077, 385353535, 154502165, 331820839, 620512821, 34380342, 73761550, 900000000, 118867411, 59479270, 306125263, 1452380952, 35589411, 189588101, 291666667, 327020202, 68795845, 58277027, 56539219, 191666667, 725432900, 133516575, 135123161, 1289682540, 423611111, 67475536, 330555556, 105911330, 166811594, 271645022, 81117202, 74728261, 120818815, 48646317, 100121380, 611111111, 214285714, 171428571, 182411067, 91169683, 139244663, 436111111, 88420957, 344877345, 114379085, 158187135, 1086421911, 91822276, 41984682, 728153464, 353246753, 510714286, 219948849, 866666667, 486111111, 85262515, 276923077, 237557764, 131763681, 53723781, 100014569, 889568153, 161455108, 428571429, 309989031, 55100803, 94501845, 632478632, 140459176, 527020202, 118206522, 150793651, 77139037, 49185026, 950000000, 1041666667, 29484906, 38136969, 1714565826, 233928571, 67750955, 116011042, 361455108, 250000000, 40888493, 166679216, 173202614, 133333333, 184026119, 200000000, 825272337, 24079443, 309322502, 328382283, 983333333, 94758065, 95111206, 552579365, 241302162, 316785329, 280115718, 233333333, 38279857, 214516129, 378968254, 615502070, 30796565, 166666667, 96064815, 574542125, 244755245, 126016179, 123912070, 19850629, 525432900, 434983189, 118478261, 105299158, 234179198, 67138174, 452380952, 369731146, 97590012, 36329417, 113097377, 150632122, 291208791, 115422097, 116036908, 876190476, 2203968254, 160063118, 194444444, 508741259, 46659506, 108831909, 104795643, 611111111, 353968254, 346764347, 252631579, 344318182, 99390244, 194444444, 792857143, 496138072, 767857143, 490909091, 168016194, 169047619, 311204482, 1608766234, 537442090, 63665098, 111450611, 283012821, 71511628, 82109929, 1415724976, 90909091, 605952381, 1026767677, 741666667, 32143674, 514220936, 360843731, 249284443, 71143830, 86624780, 120370370, 608608059, 341327723, 131710615, 520634921, 144440930, 152631579, 88445731, 127272727, 81831832, 572135218, 56947577, 952380952, 133333333, 953968254, 101206064, 105298473, 180344189, 400432900, 96444732, 136781609, 59337845, 337129055, 86554622, 321085859, 50282486, 486111111, 167010664, 333333333, 434523810, 1392099567, 95818815, 82057823, 855963481, 174621212, 148768473, 591321169, 701165501, 125100550, 160818713, 369852370, 68710359, 377777778, 425750024, 90157810, 84477830, 117317041, 100074156, 181818182, 61559989, 475454545, 419453474, 1018402846, 54129292, 725000000, 542492638, 1930494505, 421891309, 417857143, 129550468, 414807692, 732826384, 716666667, 24502709, 103448276, 30456906, 72049689, 59872417, 47111158, 52164086, 457548884, 412012987, 74728261, 104581096, 294535669, 246796926, 64755370, 25432347, 50641026, 496813725, 1057059966, 144843119, 402777778, 146904257, 99971324, 94758065, 88448313, 315909091, 132436955, 728928108, 64970289, 160256410, 121597096, 110144928, 50536131, 70705338, 55344719, 59291425, 137127963, 47774873, 282870370, 1354946442, 414285714, 58124473, 294864302, 29389672, 285852723, 290909091, 125971786, 50937082, 425000000, 176054018, 455357143, 147166783, 792543568, 24187958, 75887393, 46439021, 509523810, 284936197, 759016106, 56787843, 95059524, 395308347, 1169887520, 2212301587, 216983352, 154184704, 138095238, 64052288, 389111705, 418650794, 135464701, 816666667, 290909091, 80263406, 146927147, 312140217, 302631579, 484523810, 79824253, 94584313, 171428571, 434523810, 45909301, 425000000, 542488642, 36291893, 90233831, 135109646, 282549435, 399732620, 390909091, 21497947, 558333333, 79745378, 760401995, 21984907, 506944444, 1917832168, 813467088, 468347339, 150144928, 111111111, 326589097, 148351648, 176823036, 180555556, 25142400, 175681817, 144010309, 706944444, 224242424, 851355078, 245865654, 173554422, 373834335, 614281726, 425000000, 348290598, 215811966, 93961847, 605598976, 621428571, 767099567, 76517467, 568686869, 1183068895, 398955108, 186147186, 140476190, 170289855, 467604725, 68181818, 162751895, 60016006, 392857143, 276696923, 1219412663, 111111111, 208437761, 61442397, 236831502, 233333333, 82906257, 312990196, 61545373, 69862720, 90476190, 40062915, 674675325, 235512821, 374837662, 1026190476, 50608109, 152914420, 87557286, 172635102, 434523810, 27582750, 150000000, 150490196, 364652015, 187703421, 1460843731, 98199768, 227377946, 192107409, 270833333, 63194860, 322594073, 503968254, 284482759, 118300654, 33233215, 138435281, 188596491, 607458789, 213298934, 93620717, 112750032, 44580014, 249614817, 323387723, 1412301587, 158360566, 148351648, 116405077, 261241419, 206328321, 179166667, 81533101, 66825250, 85405140, 231684982, 596825397, 158730159, 154128387, 428571429, 100250627, 98377282, 169054137, 293650794, 876190476, 413916824, 146013226, 87092922, 196428571, 108187135, 183872350, 628546454, 151294353, 128932806, 342857143, 416666667, 69178412, 147259052, 185140736, 147464779, 224829060, 476190476, 416666667, 86129374, 300534114, 146332139, 201290162, 27498478, 214285714, 527035465, 54453964, 174060150, 183174603, 314969241, 94792610, 67993119, 357360510, 110415817, 472222222, 225490196, 123333333, 79574915, 44081619, 77597841, 1188131313, 280376805, 71417271, 556782107, 211255737, 200000000, 81962594, 224572650, 651190476, 866666667, 42553191, 215018315, 259462759, 60931199, 295440581, 174603175, 82582583, 133746160, 256089744, 50379618, 367261905, 208823529, 1330486997, 71373592, 82157182, 191666667, 257010664, 100737303, 285581193, 635604162, 577115204, 435110211, 348041753, 303869624, 89413061, 117647059, 451752822, 385353535, 117596175, 120953121, 542857143, 55171670, 60064103, 176811594, 228685597, 81914245, 958766234, 542589763, 52210520, 733333333, 146464646, 51596090, 68054221, 86977079, 110680421, 291208791, 219780220, 279061625, 832251138, 103508065, 184313725, 43811785, 67390942, 98170732, 402020202, 1148051948, 451923077, 43136980, 489410835, 121146953, 236413043, 57476445, 148556833, 163614673, 21390986, 69915719, 127636098, 211111111, 435446750, 96795340, 76158751, 753113553, 115937059, 93087558, 159074156, 500000000, 900000000, 71063815, 109969299, 485714286, 440262348, 91989664, 339627565, 1192575845, 106236441, 121739130, 324060150, 65160414, 194444444, 35843882, 204656863, 547222222, 47741353, 189809170, 151897122, 99323085, 69690550, 128623188, 225490196, 342074592, 452815052, 92226891, 183333333, 46657974, 146559473, 215018315, 361111111, 52804108, 324343295, 260256410, 31502371, 276923077, 137469801, 4070238095, 498669312, 200000000, 94188563, 45502369, 59446928, 38473409, 194978632, 67700376, 410714286, 165973104, 655291117, 399404762, 292929293, 176470588, 3857972583, 486111111, 60016006, 230434783, 416666667, 378968254, 117647059, 900000000, 102597841, 1341161661, 54738814, 265035261, 1182081807, 674242424, 467857143, 434523810, 500000000, 121323529, 151432806, 140886731, 56650410, 121893185, 58951288, 383333333, 426862027, 22045677, 113095238, 38039754, 885714286, 91150429, 92014833, 26048593, 114046622, 147905814, 40796068, 51841203, 274510596, 240131579, 74074074, 65984016, 127016129, 604249955, 533766234, 36500342, 155299335, 673314083, 557575758, 465079365, 176879085, 152688447, 989728721, 159029261, 35836040, 753846154, 97894892, 255599473, 436385836, 97069597, 520634921, 330299770, 150623886, 272727273, 75565039, 211932271, 100115207, 145238095, 935714286, 100686499, 40603080, 70983087, 28741181, 63870614, 353267974, 533766234, 98119122, 156994857, 80000000, 264957265, 110224178, 50705128, 360705912, 81365390, 251680672, 230332168, 519731146, 44794896, 539682540, 41751209, 501623377, 225714286, 159440929, 608741259, 106991214, 95365419, 313034188, 120128205, 428571429, 75412882, 287500000, 128107483, 382539683, 190909091, 154285714, 100490196, 24239316, 93291351, 273202614, 47017330, 704507072, 271869028, 68621013, 72827417, 43496272, 286147186, 1096726373, 44943294, 94537815, 965584416, 105911330, 65965716, 327020202, 291014438, 462301587, 97846804, 87050105, 54979161, 82076014, 120272492, 140692641, 140692641, 33744232, 444444444, 485714286, 851190476, 369238795, 67132116, 531888944, 87218422, 425956893, 150632184, 57734929, 77967517, 1274372775, 94179894, 144701843, 177226497, 181547619, 1132908609, 709956710, 1034239372, 41697214, 343589744, 819123932, 129742204, 64369516, 434798535, 615873016, 174242424, 752777778, 185063479, 225274725, 90672524, 173443117, 207393484, 101292602, 204618196, 81990232, 110651246, 65621171, 23262273, 45321706, 72258065, 584531155, 37252028, 500000000, 246008065, 84920635, 39176444, 65144520, 238095238, 64864191, 452380952, 533333333, 29727891, 65217391, 531746032, 937165130, 106673836, 392857143, 347222222, 134482759, 183730715, 107382798, 148904995, 194444444, 344877345, 42845604, 326923077, 434523810, 958333333, 69450039, 385714286, 545622793, 65079365, 717857143, 1070964083, 45772623, 120952381, 202571938, 126807729, 197380952, 18750000000, 206845844, 23649989, 52192002, 107234081, 63906981, 130749836, 607575758, 159003831, 218055556, 500000000, 692074592, 327276703, 386446886, 153906117, 780891331, 185807656, 120000000, 92079493, 296428571, 104523173, 47825227, 712301587, 528371745, 457557720, 89233193, 102298851, 587734488, 75599209, 952475224, 769047619, 402020202, 290909091, 45131502, 315018315, 850000000, 378968254, 102272727, 118831382, 1012413975, 159917120, 66716086, 191666667, 86956522, 115934043, 144755895, 1013034188, 200000000, 78207580, 553571429, 1086538821, 219780220, 82101806, 54804805, 37763954, 220512821, 313131313, 93262565, 82722398, 84929262, 1008333333, 993290043, 283945817, 1733766234, 611544012, 65178852, 790909091, 277597403, 74420677, 88888889, 260256410, 368864469, 281573499, 177619048, 361111111, 333766234, 123968539, 94887955, 152559717, 582575758, 452380952, 93566365, 125586283, 620634921, 576209419, 39575071, 28458113, 161016266, 827663900, 188494706, 192631579, 333461860, 188034188, 50543974, 766666667, 109411765, 206407800, 71952974, 387337662, 877380952, 89171375, 114908774, 338095238, 361111111, 677545548, 160818713, 206089744, 327348048, 58970100, 113576326, 410714286, 1162373737, 153641680, 165215517, 96507761, 642857143, 179345686, 501837928, 57562768, 496112020, 173542629, 250622471, 825000000, 75641026, 453968254, 635142128, 74515557, 1938406038, 175000000, 329004329, 100091224, 172178856, 86857808, 853603749, 577380952, 661111111, 1236544012, 136956522, 337812411, 375000000, 824419950, 716901154, 319395254, 392857143, 70778895, 121593575, 164337474, 515151515, 434523810, 134434137, 5037301587, 92083333, 505185969, 196248196, 561638350, 65724033, 310720652, 85714286, 785714286, 341860037, 73145150, 375000000, 53139089, 93926168, 183333333, 97849462, 263876884, 87155049, 494022644, 957689778, 280952381, 58506857, 79215686, 1399992785, 743540670, 2417460317, 340909091, 396825397, 1598088023, 141666667, 1024567230, 288117623, 1037301587, 225274725, 223696849, 697076023, 326923077, 535714286, 486111111, 183051579, 164253117, 65335464, 65276365, 38488613, 229166667, 349679487, 269780220, 102579216, 24139303, 163101604, 77631579, 227638014, 224061498, 522023810, 288982260, 50037078, 765391947, 274258669, 53861058, 93073593, 276190476, 1440079365, 165743674, 145944742, 140692641, 265079339, 232883680, 878968254, 102935140, 57575758, 335664336, 142512077, 458766234, 78854270, 528571429, 724077034, 434523810, 99066092, 180147059, 45340120, 208333333, 23512842, 298282967, 367857143, 245017462, 391897400, 90408163, 62238229, 402549477, 77597841, 219780220, 356150794, 137619048, 1171800422, 108823529, 53896104, 22362024, 467857143, 94587288, 129151536, 68837325, 142015300, 233766234, 73953151, 1551190476, 104908668, 126543551, 155263158, 211697047, 238095238, 603813759, 156748898, 46125117, 120627245, 53585304, 217401961, 1085353535, 317531197, 41934104, 82108500, 48163548, 153846154, 157319159, 716666667, 1399140323, 257411067, 79930303, 229004329, 250697767, 67877311, 723166402, 1154419551, 122036510, 70010483, 25887838, 45024516, 134870444, 436111111, 75051106, 851051623, 151111111, 386295006, 103124327, 436111111, 84920635, 500000000, 120590521, 164086687, 72438040, 110256410, 41241497, 509523810, 521825397, 29782270, 94124305, 143035619, 66035850, 174615707, 130073529, 37312030, 146637149, 297496202, 69844789, 183302808, 252631579, 299145299, 392857143, 306228887, 87189984, 54626626, 57293194, 67467992, 232280185, 1721897547, 290043917, 120376560, 441765133, 200000000, 434523810, 122161615, 98924731, 124621095, 28968935, 506944444, 223318713, 250000000, 322594073, 733333333, 123144687, 126587302, 156573499, 230769231, 215909091, 178835979, 53745495, 916666667, 76851792, 44988368, 191666667, 588131313, 188938500, 68351597, 773657489, 357575758, 194570136, 795049335, 483702393, 102644088, 291208791, 616363212, 41862285, 224430267, 152666228, 262337662, 529438771, 350000000, 660746606, 97545220, 108965517, 282883680, 1033333333, 809300422, 739682540, 61919639, 700371850, 308557626, 360653236, 86973048, 1301190476, 33864828, 56160899, 94179894, 114285714, 172077922, 185110211, 1785714286, 43583287, 919512840, 462573903, 101595526, 320512821, 89911002, 392857143, 181818182, 214285714, 631874558, 194374014, 1249299720, 415567919, 291889483, 166666667, 160818713, 1024242424, 119746067, 283589744, 866666667, 33728317, 219780220, 166666667, 361111111, 138382985, 297712418, 82908163, 80340148, 51377395, 262337662, 322222222, 458333333, 208333333, 67307714, 120859213, 103558844, 170401494, 114785507, 251165501, 201081326, 277777778, 91040009, 235308170, 653907204, 561973158, 392857143, 157049285, 56175728, 89480237, 90017825, 315096788, 311813187, 396825397, 509523810, 108900790, 705357143, 410714286, 491666667, 118640486, 1300371850, 62500000, 948758254, 104853129, 76695755, 103450073, 583333333, 48266725, 366666667, 34478230, 63005363, 95750128, 976190476, 333333333, 82934609, 98795599, 783333333, 37689859, 500119275, 2116597700, 85869565, 253968254, 176923077, 41252261, 295176468, 107936508, 40107508, 700000000, 90007352, 801923077, 476190476, 408333333, 318055556, 428571429, 130303380, 1900000000, 738034188, 303748743, 572777778, 1641666667, 934498834, 1185714286, 265151515, 433766234, 262730733, 292648090, 293650794, 120299101, 101620370, 346343874, 784444444, 165125197, 32793794, 900000000, 80007352, 601923077, 485714286, 500603432, 225274725, 386724387, 130434783, 82926256, 176470588, 84462151, 292857143, 624801587, 152834542, 227737520, 450764921, 88932806, 1366666667, 120293370, 139744812, 74390244, 78001186, 235515259, 120197044, 285353535, 178634420, 163649903, 64794686, 422619048, 200595238, 106635149, 258741259, 163492063, 200000000, 137776506, 51911053, 132994390, 34682990, 63847599, 117816092, 1653113553, 109931349, 751984127, 351665395, 506039817, 224033816, 133407490, 133225422, 121794872, 88732004, 1017075834, 162377622, 121323529, 204656863, 108831909, 101956815, 809498834, 499242424, 54123227, 369852370, 190018315, 105908058, 398099123, 123106061, 911588257, 862887989, 101435326, 174242424, 213728632, 1208333333, 93502556, 97285068, 375000000, 102100244, 111985303, 50982934, 384444444, 28377663, 497074168, 224508011, 154635687, 983333333, 390277778, 310851648, 75641026, 430824220, 299067339, 434523810, 190018315, 157894737, 199759268, 61197544, 264957265, 413320890, 71462159, 362703963, 87898308, 187990196, 389767074, 176470588, 353968254, 124647092, 88251577, 130434783, 258823529, 345512821, 84085307, 472222222, 369675178, 640909091, 162337662, 131847782, 135643201, 211111111, 222222222, 107831243, 39060715, 148185935, 259100036, 55929662, 831005202, 125144928, 900000000, 413690476, 434523810, 264709865, 33812899, 482191885, 42198299, 63014680, 279761905, 42196425, 132411067, 391941392, 220540882, 221161192, 108222229, 156391640, 392857143, 214285714, 819537815, 25998298, 226923077, 294047619, 59828260, 116394928, 719881155, 1307582184, 23788803, 59714795, 240206664, 387337662, 558766234, 76687287, 817857143, 250490196, 530891331, 799504662, 111646571, 608766234, 783333333, 2799242424, 110104530, 877604631, 147169394, 103530378, 85470552, 102754349, 105974395, 19819466, 41296651, 59112918, 329591465, 23332404, 1103700874, 198686371, 1000396825, 80273655, 85404776, 195488722, 563194444, 117795603, 71260269, 293650794, 131907187, 82707829, 79658606, 510207337, 180063123, 576190476, 214285714, 243589744, 147671829, 75641026, 529304029, 402020202, 229004329, 370162680, 54448822, 77806719, 27693603, 113147722, 476190476, 71063266, 142857143, 169191919, 74390244, 449675325, 100230415, 183225109, 645834238, 494555853, 457575758, 194583333, 350371850, 271121699, 663095238, 176805896, 148459384, 176879085, 116283560, 116964286, 151375015, 143744164, 72646058, 334498834, 273109244, 158403473, 90903585, 46511628, 410714286, 176785714, 35325945, 202413273, 225274725, 152928370, 635843731, 464285714, 134355967, 142857143, 258833408, 126080586, 208333333, 167832168, 155911330, 211361844, 67523865, 1242562068, 70719603, 1917857143, 83764718, 200000000, 331477733, 93154762, 436111111, 91708195, 179870056, 484686235, 176989677, 576226551, 109959162, 128214956, 265151515, 305913385, 282006399, 65053344, 1440981241, 742063492, 94358178, 135156159, 138064595, 709523810, 48094840, 70598642, 219079940, 127946128, 69617105, 203846154, 54335596, 438621509, 376623377, 314102564, 263581691, 79058360, 111111111, 203656491, 145634260, 140211640, 52913753, 360086413, 862337662, 68193545, 332565727, 67461698, 660709960, 846366153, 96543779, 681547619, 20792933, 145797839, 392857143, 154198989, 196675519, 200350529, 190909091, 561143432, 41339564, 318593347, 321428571, 173032917, 661179507, 392857143, 155398374, 111029166, 91127283, 70507155, 90932582, 79844961, 97849462, 204761905, 101543694, 182539683, 181519726, 392857143, 36861126, 53615819, 97771371, 92286789, 654304029, 50177128, 57142857, 47345824, 262337662, 159074156, 181962482, 90312964, 74908873, 127370700, 420634921, 287337662, 265998707, 41241497, 87121212, 97619048, 49314446, 183928571, 339423077, 242857143, 22442166, 274635495, 268570524, 615079365, 132321394, 82326007, 159937304, 391666667, 34310850, 375000000, 166666667, 183333333, 126880769, 95238095, 83914053, 268686869, 329064891, 232636462, 135674375, 139244663, 150348143, 174324701, 163591659, 408678262, 94844349, 53928157, 1165412752, 165273133, 203342953, 384462759, 434523810, 472770619, 51608244, 256150794, 57412587, 176587302, 533333333, 1351129426, 130128205, 207575758, 352650100, 1656168831, 110294761, 33250160, 101033592, 137776506, 164086687, 386763278, 37834993, 257478632, 69354371, 365873016, 132258065, 969222689, 108628759, 398351648, 231684982, 177631579, 324383055, 428571429, 181318681, 73373117, 300595238, 926923077, 632117882, 139365908, 1983333333, 368686869, 67419445, 321296096, 148187135, 283012821, 372408964, 202189780, 130425681, 51035169, 400432900, 278943279, 103716371, 333682308, 193306693, 148967595, 108147621, 196905234, 102246452, 23816087, 57722282, 186995857, 83456903, 378968254, 642857143, 343379953, 353685597, 96724872, 281385281, 410714286, 200000000, 410714286, 361111111, 150816373, 266666667, 45943306, 563492063, 82908163, 447351668, 215073944, 429159729, 452380952, 243589744, 190384615, 650639156, 425696919, 111447918, 43341176, 96969697, 156192489, 69767442, 32521141, 131519274, 333823529, 245670996, 813131313, 326454518, 162500000, 204296974, 185677971, 86825250, 194444444, 525432900, 68381064, 1125000000, 300502571, 206792717, 232478632, 487187181, 205263158, 434523810, 243545752, 137923181, 442857143, 241297036, 218636364, 399513588, 295833333, 281818182, 138544035, 110315238, 184841270, 62579386, 220944742, 298784549, 204061625, 118461538, 46110326, 424639250, 35144504, 208959806, 378968254, 91118077, 50185915, 403113553, 302020202, 68804468, 527777778, 416612773, 511702167, 42265052, 583333333, 263483227, 42655936, 422539683, 196431224, 184964630, 1290987662, 220463747, 183823529, 91952395, 525000000, 99497540, 71691176, 43310078, 130252101, 39549483, 108407490, 162092308, 247926094, 76914741, 93048128, 81929023, 268589744, 1450000000, 152631579, 204656863, 266666667, 172101449, 104028978, 933012543, 202750627, 343589744, 219444444, 97662523, 57614645, 200983227, 49935850, 184313725, 170668718, 54682798, 1824675325, 400861308, 548351648, 147818530, 477777778, 279761905, 161111111, 187990196, 163742690, 214629712, 66012473, 87143851, 148201567, 65787426, 1828978360, 165032547, 416649914, 208333333, 42307692, 452380952, 22138528, 80016006, 350000000, 410714286, 224498133, 140549431, 527510398, 522222222, 97990823, 83333333, 123216173, 113636364, 156427894, 267399267, 79291997, 360843731, 155341972, 247428400, 93750000, 58823529, 183333333, 60832462, 634523810, 176994836, 828120219, 146331214, 434523810, 65507365, 791666667, 252907223, 146924245, 88588947, 96168075, 344254956, 181299820, 57160917, 1253708791, 97189542, 195833333, 225751024, 41317678, 265151515, 265151515, 527777778, 98350169, 101665666, 174423307, 133198175, 185783954, 653968254, 342388391, 202020202, 59633290, 181045752, 646825397, 533333333, 25379612, 486111111, 81831832, 620634921, 580150327, 38689014, 52364366, 687990196, 240609305, 948484848, 92558647, 90017825, 2376190476, 74887218, 692099567, 77786797, 759523810, 216229536, 448351648, 683091908, 1449474436, 358627257, 504166667, 97009292, 1055158730, 88457375, 243263329, 222756410, 395509207, 1042857143, 162562741, 375000000, 278943279, 1174542125, 866666667, 233766234, 588888889, 452380952, 36079472, 88696856, 53755055, 226190476, 222756410, 160836475, 102301790, 72009292, 108187135, 351386434, 47717538, 60642257, 297671261, 90055300, 402777778, 91880342, 71788056, 35260831, 452380952, 738033551, 54820243, 197619048, 91419303, 206160959, 361111111, 628409091, 122065338, 68258177, 281818182, 558333333, 477777778, 599145299, 202020202, 566666667, 69203592, 54669988, 1210155123, 88931789, 99450549, 78565614, 72832234, 950371850, 41069043, 410119048, 312669683, 184387352, 114379085, 94017094, 270944742, 600432900, 344045739, 243589744, 94886364, 975992063, 21306587, 267261905, 176470588, 230753354, 108271823, 144679803, 54882155, 39557571, 134191711, 152777778, 294541191, 192879528, 178835979, 392857143, 457575758, 33167549, 102631579, 100686499, 72153528, 1200180375, 42150787, 200983227, 43287531, 452380952, 281818182, 201680672, 428452902, 1000000000, 82931989, 73170732, 649221174, 255673307, 141742846, 99145299, 544669342, 89297817, 306943830, 162068004, 48105593, 57330057, 53656027, 1444444444, 159074156, 262337662, 888762626, 125768601, 547142980, 577205066, 44806008, 59555990, 96035789, 717857143, 354166667, 396367521, 184722222, 233065954, 372076023, 97744361, 60161672, 706349206, 100250627, 175595238, 145023954, 45785792, 28499574, 77228327, 866666667, 167628205, 153003424, 72577509, 204761905, 209325397, 63485005, 256532448, 217594666, 22835964, 266283928, 107754847, 370916134, 409523810, 286560150, 660256410, 410714286, 102482993, 83080808, 44777894, 127583300, 26786478, 100000000, 11953186, 59029075, 75224324, 52854100, 1372105672, 115223665, 140788185, 592796093, 231684982, 476190476, 207220951, 152257778, 75722211, 136863711, 67699847, 68911877, 116170635, 99572247, 33560244, 251165501, 139335888, 47661619, 38571849, 51242239, 426084820, 365909091, 67115506, 580952381, 187018931, 109933675, 410714286, 361111111, 389957265, 469877345, 116019205, 25148496, 69949495, 47208539, 503267974, 78958732, 113538749, 69016822, 35871304, 266943173, 376609319, 65517543, 62943262, 526190476, 50255946, 44823636, 173106391, 285451089, 219298246, 56190476, 108448530, 157857346, 100000000, 233766234, 88888889, 64285714, 226874003, 602890470, 342832168, 153960114, 109890110, 434523810, 990368300, 735714286, 507936508, 70846395, 33661326, 281746032, 1000000000, 216551854, 125969843, 100545102, 71439964, 114175824, 1194981816, 995292208, 127169721, 85625159, 493589744, 467857143, 435786436, 116923077, 180555556, 76923077, 474696545, 285110211, 34309950, 183823529, 200595238, 91232423, 75886525, 689285714, 94232261, 262750627, 28110945, 455891331, 361111111, 136530920, 148397708, 425522517, 507539683, 84126984, 651029753, 126199076, 151121846, 1574242424, 230706976, 527184804, 572222222, 108041757, 72700998, 84920635, 447222222, 202716823, 387669683, 378968254, 763731721, 304554656, 90535092, 66271540, 851190476, 625898121, 24275362, 161594911, 151897122, 186863711, 127603034, 121794872, 599732620, 502777778, 550000000, 1147077922, 38156851, 204296974, 1134523810, 73012422, 602633452, 80499249, 50183866, 174843628, 245670996, 499523810, 628968254, 866666667, 27777122, 114379085, 56941480, 392857143, 200177933, 91928356, 892857143, 91214106, 95818815, 522222222, 157894737, 450980392, 195833333, 129289532, 118055556, 62074830, 731684982, 403583804, 119403394, 35732582, 42335116, 529010695, 340909091, 1143488456, 102745098, 1282493895, 413149351, 52976190, 103130448, 392857143, 267832168, 193725743, 730044669, 500432900, 216374269, 74390244, 385714286, 46470331, 34710465, 242407489, 472222222, 38280413, 106039852, 444444444, 117647059, 265151515, 346001221, 109940008, 153132127, 81024222, 122935536, 91696569, 121347068, 163057612, 35749359, 132258065, 115783885, 156937017, 334967320, 215018315, 198411496, 113181955, 71256039, 111455108, 415909091, 107474610, 66688625, 80729730, 77233087, 113802622, 396825397, 60069704, 92803030, 178345169, 889787010, 219801631, 90909091, 1387629446, 172950420, 244602284, 37172890, 141666667, 94140493, 47831388, 238578312, 596699024, 205992923, 139548990, 255894908, 78339947, 280952381, 70415061, 909523810, 391666667, 182390340, 403409091, 121666667, 221323529, 229437229, 299031792, 228571429, 37323037, 134387352, 95766129, 54428081, 61274510, 342857143, 155263158, 52911022, 10341510, 190909091, 86427239, 491376168, 59717323, 137776506, 173611111, 474542125, 307998463, 1078665501, 215909091, 97134183, 220512821, 136536139, 803003429, 75248087, 59426428, 54983906, 56321198, 808333333, 44351716, 1345075758, 176417428, 372842017, 207875458, 81639892, 452777778, 110216281, 508239236, 189035770, 214379085, 220229365, 1253333655, 505133755, 45473101, 666666667, 108642801, 78807813, 387266140, 402777778, 180147059, 164556692, 385843731, 21797752, 170952381, 100000000, 158187135, 75706033, 273611111, 678968254, 23847872, 514811985, 428571429, 77501836, 91341509, 322766738, 355910881, 97573964, 70249487, 66688625, 444444444, 370147059, 492857143, 134982833, 109295102, 267788433, 148989899, 132463873, 72065338, 110961768, 559523810, 49726776, 49097935, 155573593, 122005823, 204656863, 680169830, 93830946, 481026127, 179615706, 225706646, 139609133, 82373437, 58088352, 71931931, 160683693, 75498575, 245670996, 266666667, 110144928, 129558847, 842857143, 251165501, 103176452, 142909062, 143530378, 54968258, 82302389, 45592079, 492929293, 388888889, 48280388, 392857143, 30217087, 115919435, 249033816, 75641026, 204793934, 772222222, 69295102, 1317095813, 723844171, 436493213, 576923077, 142156863, 2020822593, 98246418, 56349060, 555400521, 339187273, 84625850, 151648001, 88193625, 113492063, 799080087, 1128113553, 327020202, 301371755, 61492555, 335818713, 392857143, 159235837, 539456522, 116401669, 250000000, 165700483, 202413714, 114217482, 39277405, 222333630, 61635456, 426755322, 162346374, 214285714, 90732368, 55532611, 312500000, 450266795, 86070231, 339592760, 237738095, 250742721, 135932281, 437950938, 224242424, 700000000, 1081829880, 239301665, 425898121, 258035714, 238095238, 434523810, 96161607, 104476672, 186405836, 783333333, 709523810, 64053885, 96398791, 91543156, 400250627, 133414015, 225230456, 101434676, 427020202, 682575758, 386560150, 258470696, 108963450, 704899267, 113675305, 733333333, 66056882, 515256734, 76905862, 1083552092, 102298851, 491208791, 873268309, 428918785, 141564385, 792857143, 232496670, 68594104, 594444444, 72739298, 108465608, 174271487, 335454545, 95003163, 428165767, 253246753, 49188641, 79937304, 950000000, 321428571, 175057370, 227701714, 70197044, 800000000, 179447443, 103174603, 82633281, 98078449, 686507937, 195454545, 491666667, 426680672, 31977022, 142945584, 992296326, 90909091, 44263066, 1926190476, 683333333, 591208791, 370436508, 161111111, 259852530, 163051861, 1108333333, 226655697, 59283051, 46993893, 694143317, 397023810, 168860142, 332478632, 65514175, 69235592, 516666667, 151405836, 299242424, 230369099, 117248436, 3483333333, 683409135, 48744521, 299242424, 320075758, 32878463, 161597096, 335193363, 866666667, 1072351668, 46338296, 452380952, 631818182, 160256410, 153188467, 967710944, 79623850, 359462759, 75604012, 306818182, 101223582, 69356083, 65591398, 286825600, 58441558, 131666667, 499342825, 50285175, 138716356, 836428697, 106122449, 400535467, 678968254, 741666667, 83556766, 138888889, 430555556, 604761905, 160401338, 107972350, 220512821, 94587288, 62769616, 783333333, 145629085, 104278075, 1192279942, 301613727, 485171261, 124390244, 718599864, 146416084, 191076165, 575000000, 396825397, 476190476, 495555556, 697222222, 52075572, 102429977, 35575986, 521857717, 105883006, 125951758, 69174773, 171679198, 383576568, 431684982, 439297526, 783333333, 300000000, 317458865, 73060109, 254720854, 385249518, 247707337, 378968254, 894444444, 121323529, 504899174, 502777778, 106010869, 40730594, 452380952, 184389933, 79212454, 209775132, 188311688, 114852602, 467857143, 465909091, 38985507, 159634380, 147164897, 226080586, 409523810, 62016908, 193165885, 182539683, 985087135, 35714286, 537612388, 434523810, 164767222, 393648801, 2317532468, 242389493, 79714795, 33468227, 66183575, 34403860, 47323339, 85650976, 97531390, 179619781, 342555995, 525432900, 65277778, 307692308, 230769231, 85884354, 1267370538, 522628174, 579554656, 645885547, 65359153, 116806723, 69556438, 157894737, 363665501, 74294127, 195087320, 425213675, 189361440, 77343515, 100250627, 941049594, 215700483, 161097187, 475000000, 65862709, 240909091, 235964912, 380141598, 56019306, 49123932, 552496480, 192530569, 28726708, 374542125, 174545849, 50958845, 467073667, 699564325, 487637363, 221428571, 1085033023, 98590067, 478943279, 582575758, 243589744, 307609934, 50123601, 924242424, 477777778, 130764810, 58993960, 95238095, 192219680, 168016194, 620634921, 302020202, 106039356, 258741259, 69526179, 841953368, 124158249, 465909091, 1385714286, 48714329, 72717840, 110893246, 233766234, 28436388, 73425404, 154951434, 305927406, 57221508, 1168163781, 21267798, 185850556, 162337662, 184741954, 49410602, 159833630, 465079365, 98498382, 410714286, 525432900, 52804108, 321428571, 181818182, 784523810, 452380952, 236926451, 126441103, 161998133, 272727273, 898351648};
constexpr int TABLE[10][5][9][30] = {{{{3664, 1003, 281, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {121, 9, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {6, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{2002, 196, 30, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {19, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{1856, 57, 7, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {8, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {4, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{1095, 122, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {25, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{820, 328, 192, 41, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}, {{{4294, 2288, 1387, 674, 328, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {307, 52, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {26, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {7, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{5263, 2586, 758, 226, 103, 27, 51, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {689, 40, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {124, 4, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {51, 4, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {34, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {26, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{3161, 1452, 787, 326, 137, 31, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {282, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {44, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{3423, 1627, 682, 368, 135, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {233, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{2968, 1323, 677, 451, 252, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {218, 45, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {33, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}, {{{3756, 2841, 2494, 1805, 1508, 1178, 1009, 730, 552, 150, 385, 333, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1250, 358, 42, 11, 7, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {244, 62, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {103, 33, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {54, 25, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {40, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {13, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{4588, 3176, 2055, 1468, 942, 519, 213, 152, 17, 29, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1278, 240, 55, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {289, 37, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {158, 8, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {83, 8, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {21, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {13, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{5514, 2813, 1674, 1135, 617, 568, 374, 403, 157, 171, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1006, 150, 44, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {171, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{4557, 2406, 1415, 811, 753, 523, 257, 207, 119, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {861, 169, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {164, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {46, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{5600, 2642, 1825, 1085, 642, 427, 242, 132, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {499, 70, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {78, 18, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {58, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {39, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}, {{{3868, 3407, 2470, 2039, 1955, 1819, 1497, 1423, 1289, 1165, 1107, 1126, 998, 849, 725, 574, 844, 690, 800, 0, 1000, 0, 0, -1, 2000, 0, -1, -1, 0, -1}, {2553, 775, 313, 102, 20, 15, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1}, {672, 169, 33, 7, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1}, {364, 88, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1}, {207, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1}, {127, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1}, {48, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1}, {11, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1}, {5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1}}, {{6000, 4591, 2388, 2180, 1844, 1320, 1099, 951, 707, 707, 590, 517, 599, 281, 407, 308, 244, 588, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {2548, 651, 239, 51, 57, 35, 36, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {837, 124, 44, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {405, 51, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {204, 23, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {89, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}}, {{6154, 3553, 2619, 1787, 1402, 1134, 943, 712, 589, 605, 591, 351, 208, 459, 345, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1}, {1834, 484, 189, 76, 9, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {543, 161, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {211, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {90, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {24, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}}, {{5152, 4409, 2131, 2048, 1528, 1168, 845, 642, 652, 480, 490, 472, 192, 167, 0, 0, 0, 0, -1, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1}, {1705, 441, 123, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {501, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {204, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {62, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{6667, 4706, 2464, 2157, 1154, 1016, 610, 856, 263, 559, 612, 323, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1590, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {422, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {192, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}, {{{7500, 2500, 2667, 2056, 1821, 1534, 1405, 1356, 1242, 1317, 1310, 1147, 1211, 1008, 1182, 1258, 1073, 1202, 998, 1004, 866, 1116, 955, 1269, 1232, 1250, 1806, 1321, 1429, 1520}, {3654, 1638, 817, 410, 270, 103, 33, 55, 55, 0, 53, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1092, 304, 107, 37, 28, 33, 15, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {592, 146, 50, 9, 11, 20, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {362, 89, 19, 0, 6, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {239, 38, 5, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {122, 8, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{4286, 4318, 3191, 2464, 1855, 1857, 1783, 1365, 1406, 1168, 1107, 963, 939, 896, 774, 797, 863, 553, 530, 787, 529, 355, 288, 952, 455, 400, 500, 0, 0, 0}, {3595, 1512, 607, 361, 164, 108, 140, 81, 0, 35, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1196, 328, 106, 33, 24, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {658, 134, 41, 6, 6, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {369, 69, 15, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {192, 30, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {75, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {32, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{5000, 4167, 3750, 3509, 1987, 1629, 1610, 1116, 1216, 847, 808, 783, 694, 688, 460, 640, 856, 443, 273, 820, 408, 0, 0, 0, 0, 0, -1, -1, -1, -1}, {2845, 1053, 343, 204, 114, 98, 31, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {1022, 248, 46, 16, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {537, 86, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {243, 47, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {124, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {37, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1}}, {{10000, 3750, 5000, 2376, 2216, 1968, 1262, 1285, 799, 988, 634, 717, 679, 599, 693, 330, 385, 380, 303, 769, 323, 952, 0, 0, 0, -1, -1, -1, -1, -1}, {2751, 894, 215, 46, 80, 63, 32, 50, 155, 211, 256, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1060, 220, 10, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {436, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {235, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{10000, 0, 4286, 5000, 1897, 1650, 1419, 1545, 1182, 826, 672, 861, 575, 331, 235, 182, 0, 0, 1429, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {2740, 763, 245, 27, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {922, 152, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {296, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {96, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}, {{{-1, 2500, 2500, 909, 2558, 1571, 1957, 1420, 1282, 1657, 1026, 1429, 1214, 1159, 1140, 953, 1101, 1064, 1096, 1025, 801, 1046, 941, 1016, 1153, 1030, 932, 1112, 1047, 1218}, {4769, 2465, 1386, 797, 406, 360, 194, 71, 38, 48, 46, 25, 17, 19, 25, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1213, 545, 212, 58, 62, 45, 40, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {703, 281, 115, 18, 27, 23, 20, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {442, 179, 57, 23, 5, 6, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {300, 102, 36, 9, 0, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {170, 61, 13, 5, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {59, 20, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {17, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{-1, 10000, 6667, 5455, 1935, 2159, 1846, 2103, 1837, 1326, 1517, 1319, 1123, 1284, 1077, 1018, 914, 763, 875, 891, 848, 954, 824, 752, 657, 529, 708, 539, 639, 573}, {4585, 2213, 1146, 674, 429, 171, 165, 128, 118, 97, 50, 23, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1689, 560, 177, 50, 32, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {884, 267, 58, 23, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {538, 118, 26, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {298, 44, 11, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {160, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{10000, -1, 6667, 4737, 2581, 3968, 1897, 1751, 2197, 1530, 1444, 1437, 1057, 1005, 964, 862, 867, 1065, 693, 662, 852, 727, 798, 682, 867, 636, 400, 189, 357, 286}, {4211, 2130, 889, 678, 288, 201, 265, 74, 122, 68, 435, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1713, 489, 232, 113, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {852, 236, 27, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {458, 54, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {165, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{-1, -1, -1, 0, 1333, 4545, 1864, 3038, 2479, 1813, 1412, 1238, 1433, 746, 949, 803, 694, 551, 957, 628, 796, 282, 957, 471, 233, 0, 0, 0, 0, 0}, {4608, 1653, 670, 365, 171, 102, 84, 37, 0, 0, 161, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1760, 424, 102, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {695, 144, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {306, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{-1, -1, 5000, 3333, 3000, 2727, 1351, 1111, 1194, 1264, 1074, 806, 1210, 1000, 738, 698, 714, 309, 300, 267, 156, 167, 204, 333, 0, 0, 0, 2500, 0, 0}, {2975, 890, 390, 229, 134, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1453, 205, 181, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {649, 186, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {156, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {99, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}, {{{-1, -1, -1, -1, 5000, 0, 2000, 1111, 2273, 2083, 612, 1341, 1477, 1304, 1724, 1186, 1044, 963, 1160, 1307, 1315, 1190, 1004, 973, 1239, 921, 963, 880, 970, 999}, {5895, 3965, 2505, 1606, 1037, 644, 386, 324, 294, 203, 76, 110, 66, 0, 22, 46, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1585, 616, 285, 170, 60, 40, 32, 28, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {916, 341, 143, 44, 25, 27, 17, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {608, 185, 70, 33, 6, 7, 9, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {376, 130, 40, 17, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {255, 91, 20, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {166, 33, 5, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {48, 5, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{-1, 0, -1, -1, 0, 5000, 4000, 3333, 3333, 2195, 2429, 1731, 1008, 1196, 1770, 1233, 1061, 1332, 1151, 1101, 946, 1150, 1047, 1170, 1171, 895, 850, 905, 976, 752}, {5651, 3156, 2171, 1228, 856, 595, 357, 316, 144, 211, 220, 90, 50, 28, 38, 44, 65, 88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {2387, 749, 297, 175, 94, 17, 43, 12, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1301, 316, 116, 137, 8, 9, 11, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {743, 205, 59, 75, 0, 10, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {552, 127, 38, 28, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {351, 38, 13, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {157, 25, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {40, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{-1, -1, -1, 10000, 5000, 1250, 2941, 2353, 2581, 2658, 1389, 1790, 1591, 1564, 1314, 1331, 1158, 1149, 1154, 1037, 944, 941, 803, 827, 640, 723, 873, 656, 743, 753}, {5728, 2928, 1649, 970, 392, 359, 174, 138, 76, 108, 37, 56, 82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1}, {2287, 690, 246, 128, 38, 31, 0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1}, {1171, 218, 137, 45, 13, 17, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1}, {590, 107, 42, 11, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1}, {310, 43, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1}, {161, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1}, {79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1}, {11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1}}, {{-1, -1, 0, -1, 2500, 6667, 5556, 2308, 2424, 1667, 972, 1359, 1345, 1534, 1584, 1373, 1316, 933, 862, 901, 760, 740, 781, 922, 809, 522, 674, 435, 806, 600}, {6000, 2652, 1313, 789, 550, 299, 196, 207, 60, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1}, {2571, 914, 190, 106, 50, 20, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1}, {1301, 281, 74, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1}, {672, 168, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1}, {317, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1}, {55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{-1, -1, -1, -1, 0, 0, 10000, 10000, 3333, 1000, 2000, 3125, 1455, 1143, 1158, 938, 1439, 1156, 1079, 1007, 479, 565, 439, 777, 732, 274, 566, 313, 0, 213}, {5000, 2585, 1024, 566, 440, 338, 222, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {2244, 551, 323, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {1158, 159, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {398, 76, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {143, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}, {{{-1, -1, -1, -1, -1, -1, -1, -1, 0, 5000, 2000, 5000, 0, 3000, 0, 1053, 1818, 625, 1136, 1364, 1311, 1034, 1176, 1053, 1333, 1056, 929, 939, 913, 961}, {6748, 5068, 4062, 2893, 2214, 1449, 1154, 767, 595, 439, 279, 246, 159, 163, 166, 83, 95, 140, 68, 0, 0, 0, 0, 0, 0, 72, 95, 0, 123, 175}, {2006, 869, 354, 271, 91, 37, 72, 8, 19, 49, 12, 15, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 303, 441, 0, 0, 0}, {1156, 554, 173, 86, 23, 12, 42, 16, 10, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 303, 308, 0, 0, 0}, {751, 338, 110, 52, 23, 12, 14, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 308, 0, 0, 0}, {541, 193, 71, 26, 17, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 145, 105, 156, 0, 0, 0}, {415, 119, 38, 16, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0}, {271, 46, 28, 11, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {140, 23, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{-1, -1, -1, 0, -1, 10000, 10000, 0, 2000, 0, 3333, 2273, 1714, 1818, 1642, 2500, 1771, 1736, 1563, 1620, 1198, 1019, 983, 1146, 1150, 1144, 1137, 1119, 1140, 871}, {6771, 4776, 3207, 2096, 1620, 1092, 893, 456, 380, 277, 180, 164, 94, 72, 44, 65, 0, 0, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {2848, 1011, 465, 261, 140, 35, 29, 0, 14, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1510, 421, 276, 88, 39, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {983, 242, 136, 34, 8, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {648, 124, 65, 14, 8, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {460, 64, 52, 0, 8, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {259, 63, 32, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {82, 6, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{-1, -1, -1, -1, -1, -1, -1, -1, -1, 5000, 0, 2000, 2000, 1364, 1250, 1525, 1212, 1413, 1167, 1655, 1364, 1500, 1277, 1265, 1075, 657, 811, 856, 909, 808}, {6752, 3942, 2323, 1632, 836, 639, 321, 417, 224, 308, 330, 96, 43, 0, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1}, {2725, 1101, 374, 157, 135, 53, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1}, {1264, 321, 124, 57, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1}, {686, 154, 66, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1}, {404, 69, 26, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1}, {121, 27, 13, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1}, {59, 14, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1}, {29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1}}, {{-1, -1, -1, -1, -1, -1, 10000, 10000, 0, 3333, 5000, 2857, 3750, 1905, 1714, 2391, 1404, 1111, 1359, 1282, 1377, 533, 700, 914, 892, 913, 1042, 962, 968, 671}, {6667, 3474, 2036, 1086, 596, 366, 263, 164, 102, 297, 57, 72, 88, 441, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {2595, 1042, 259, 340, 86, 30, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1}, {1404, 406, 115, 42, 25, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1}, {761, 145, 19, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1}, {316, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1}, {97, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}}, {{-1, -1, -1, -1, -1, -1, 10000, 0, 0, 2500, 1250, 2000, 2500, 2105, 968, 1667, 1667, 2391, 930, 1194, 1333, 536, 1579, 943, 566, 889, 957, 0, 930, 624}, {5472, 4000, 1727, 656, 393, 211, 56, 150, 96, 106, 448, 0, 0, 370, 0, 0, 0, -1, 0, 0, 0, -1, 0, -1, -1, -1, 0, -1, -1, -1}, {3000, 838, 282, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1}, {1357, 284, 88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1}, {675, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1}, {230, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1}, {56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1}}}, {{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, -1, -1, 3333, 3333, 0, 0, 6000, 5000, 3333, 625, 2083, 2143, 556, 969}, {8000, 5758, 5165, 4635, 3286, 2778, 1825, 1634, 1395, 1080, 767, 566, 601, 393, 317, 214, 191, 177, 92, 65, 35, 64, 88, 0, 0, 95, 0, 0, 0, 21}, {2328, 1056, 528, 294, 179, 91, 48, 45, 26, 10, 23, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1311, 527, 256, 130, 60, 25, 7, 8, 9, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {919, 361, 121, 95, 33, 13, 7, 8, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {630, 284, 67, 35, 11, 13, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {429, 200, 33, 25, 6, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {313, 120, 10, 25, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {159, 64, 5, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 3333, 0, 0, 1875, 1176, 1154, 1034, 1579, 1346, 2364, 1310, 1635, 1495, 1043, 978}, {7414, 5321, 4224, 3341, 2314, 1694, 1292, 912, 705, 683, 390, 407, 361, 253, 176, 187, 104, 37, 135, 57, 76, 180, 217, 0, 227, 435, 0, 0, 0, 0}, {3405, 1212, 623, 412, 208, 136, 87, 28, 17, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0}, {1825, 511, 242, 157, 79, 44, 27, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0}, {1200, 322, 121, 115, 41, 11, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0}, {830, 251, 48, 63, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0}, {610, 197, 24, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0}, {406, 133, 16, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0}, {145, 42, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0}}, {{-1, -1, -1, -1, -1, -1, 0, -1, 0, -1, -1, -1, 3333, 2500, 3333, 1429, 1538, 1111, 1935, 1569, 2000, 1408, 2118, 1172, 1583, 1127, 1152, 1394, 1233, 873}, {7647, 5843, 3459, 2285, 1493, 1053, 763, 625, 434, 159, 178, 166, 134, 0, 85, 131, 161, 0, 145, 222, 303, 714, 0, 0, 0, 0, 0, 0, 0, 0}, {3799, 1366, 655, 204, 57, 75, 31, 72, 0, 0, 0, 0, 0, 0, 154, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1}, {1944, 464, 196, 42, 12, 42, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1}, {1236, 206, 51, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1}, {501, 92, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1}, {287, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1}, {133, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1}, {36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1}}, {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10000, 0, 0, 0, 4000, 2000, 3077, 2941, 2000, 1290, 526, 1111, 1207, 1200, 1039, 1100, 1398, 734, 1296, 681}, {7778, 4516, 3206, 1658, 1081, 722, 294, 385, 237, 124, 56, 70, 0, 0, 333, 0, 0, 0, 0, 667, 833, 0, 833, 1111, 0, 0, 0, -1, -1, -1}, {3594, 1107, 513, 229, 175, 135, 72, 47, 119, 143, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1}, {2023, 301, 266, 50, 0, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {850, 96, 120, 26, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {327, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {94, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 6667, 0, 5000, 0, 2857, 909, 1765, 0, 1818, 1613, 882, 1429, 1481, 1957, 650}, {7647, 3333, 2273, 1974, 1304, 597, 115, 125, 0, 182, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {3750, 1549, 330, 488, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {2097, 641, 227, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {882, 260, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 230, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}, {{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10000, -1, -1, -1, -1, -1, 0, 5000, -1, -1, -1, 0, 959}, {10000, 8095, 6216, 4561, 4783, 3474, 3380, 2833, 2332, 1861, 1552, 1306, 1091, 890, 734, 629, 671, 390, 424, 297, 166, 221, 189, 84, 88, 150, 25, 0, 34, 34}, {2790, 1217, 733, 413, 220, 173, 94, 49, 61, 62, 30, 25, 26, 0, 19, 0, 26, 0, 0, 39, 0, 0, 74, 0, 0, 0, 0, 0, 0, 0}, {1561, 672, 290, 194, 59, 74, 19, 58, 24, 20, 0, 0, 29, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1131, 486, 175, 99, 51, 63, 6, 30, 17, 21, 0, 0, 29, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {859, 310, 113, 85, 36, 35, 0, 23, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {687, 226, 56, 43, 20, 29, 7, 8, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {542, 165, 50, 24, 15, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {316, 125, 28, 14, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, 3333, 0, 0, 0, 1429, 2000, 625, 1500, 1500, 1030}, {7000, 6222, 4956, 3964, 3238, 2729, 2267, 1780, 1729, 1333, 1089, 800, 588, 345, 336, 322, 287, 157, 136, 30, 41, 124, 60, 0, 0, 0, 0, 0, 0, 0}, {3699, 1893, 811, 409, 268, 99, 142, 40, 100, 0, 25, 26, 40, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {2202, 764, 326, 148, 85, 61, 0, 0, 19, 0, 0, 34, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1446, 476, 153, 131, 29, 11, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {982, 277, 116, 76, 19, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {706, 192, 100, 25, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {508, 121, 69, 33, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {238, 36, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, 10000, 5000, -1, 10000, 0, 2000, 0, 1111, 1250, 714, 1143, 1000, 2800, 1622, 1481, 1875, 965}, {7333, 5455, 4959, 3541, 2776, 1919, 1210, 1053, 647, 523, 287, 306, 274, 250, 144, 121, 104, 208, 190, 385, 179, 233, 244, 541, 0, 0, 0, 0, 0, 0}, {3969, 1740, 936, 348, 184, 98, 19, 73, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {2196, 672, 230, 80, 31, 18, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {1416, 289, 39, 26, 16, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1}, {718, 159, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1}, {409, 65, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1}, {160, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1}, {39, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1}}, {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10000, 0, 0, -1, 10000, 5000, 0, 0, 2000, 0, 0, 1667, 667, 500, 2667, 1818, 1563, 2000, 2000, 790}, {9000, 7000, 4125, 2708, 1852, 1057, 1086, 336, 380, 426, 433, 122, 110, 182, 81, 0, 141, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1}, {4386, 1969, 376, 459, 129, 57, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, -1, -1, -1, -1, -1}, {2226, 609, 268, 125, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {1053, 201, 119, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {442, 100, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {131, 25, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {77, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}, {26, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, -1, -1}}, {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, 0, 0, 5000, 10000, 2500, 1250, 1875, 2500, 2500, 1304, 1429, 807}, {8125, 5625, 4516, 2637, 1473, 1258, 385, 568, 349, 62, 270, 0, 0, 274, 0, 0, 357, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, -1}, {5604, 2177, 823, 321, 92, 58, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1}, {3456, 674, 154, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1}, {1733, 190, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {641, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {123, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}};


constexpr double PERIMETER_RATIOS[10][2] = {
    {4.84565, 8.04346},
    {4.32175, 8.4126},
    {4.31808, 8.82925},
    {4.02815, 9.61473},
    {3.91383, 10.1661},
    {3.6875, 10.9972},
    {3.59044, 11.2027},
    {3.40316, 11.8808},
    {3.37018, 12.9122},
    {3.23883, 13.0697},
};
constexpr double PERIMETER_RATIOS_MID[10] = {
	6.0291,
	5.8723,
	5.722,
	5.66639,
	5.48363,
	5.36105,
	5.40021,
	5.17708,
	5.26941,
	5.05869,
};

constexpr double LS_PARAMS[10][3] = {
    {2.1187, 0.1898, 1.9485},
    {2.9631, 0.4015, 1.6201},
    {3.8735, 0.4833, 1.6403},
    {5.1107, 1.0869, 0.8565},
    {6.5301, 0.7086, 1.4863},
    {8.0372, 0.5859, 1.7645},
    {9.5142, 0.4765, 2.0774},
    {11.4097, 0.7148, 1.9679},
    {13.6442, 0.6777, 2.4933},
    {15.7959, 0.4178, 2.9122},
};

bool isLocal = false;
constexpr bool USE_TL = true;
constexpr int ITER = USE_TL ? 10000 : 1;
constexpr int TL = USE_TL ? 5950 : 10000000;
double timeScale = 1.0;

class Solver {
private:
	static constexpr int RAND_BRANCH_PLUS = 123456789;

public:
	Problem p;
	Rnd rndBase;
	Rnd rnd2;
	Result res;
	Field f1;
	Field f2;
	SAParams params;
	double bestScore = INF;
	Solution bestSolution;
	double bestUnsuccessfulScore = INF;
	Solution bestUnsuccessfulSolution;
	vector<SAStat> stats;
	double volumeRatioExp = 0;
	double perimeterRatioNormalized = 0;
	int perimeterLevel = 0;
	int totalIter = 0;
	int saMetaIterCount = 0;
	int d = 0;
	int seed = -1;
	bool writeSA = isLocal && false;
	bool render = isLocal && false;
	bool write = isLocal && false;

	Solver() {
	}

	void load(istream& in) {
		p.load(in);
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

	double expectedN() const {
		auto params = LS_PARAMS[d - 5];
		return perimeterRatioNormalized * params[0] + volumeRatioExp * params[1] + params[2];
	}

	double computeScore() {
		double invSizeSum = 0;
		for (auto& b : f1.bs) {
			int size = len(b.b.poss);
			invSizeSum += 1.0 / size;
		}
		return computeScore(invSizeSum, f1.totalPenalty + f2.totalPenalty);
	}

	double computeScore(double invSizeSum, int penaltySum) {
		return 1e9 * (invSizeSum + penaltySum);
	}

	void solve() {
		trace("expected N: ", expectedN());
		trace("ref: ", (ull) round(seed >= 0 && seed < array_len(BASE_SCORES) ? BASE_SCORES[seed] : 1e12),
		    " ", seed >= 0 && seed < array_len(USED_BLOCKS) ? USED_BLOCKS[seed] : 100);
		int n = (int) floor(expectedN());
		int unsuccessfulStreak = 0;
		int increaseThreshold = 6;
		bool successfulAfterLastIncrease = false;
		int maxN = min(len(f1.empties), len(f2.empties));

		if (render) {
			mov.beginMovie("mov/" + to_string(seed) + ".smv");
		}

		rep0(iter, ITER) {
			if (timer() >= TL * timeScale)
				break;
			trace("trying with N=", n, " trial=", saMetaIterCount + 1);
			if (tryWithN(n, params.singleIterations)) {
				successfulAfterLastIncrease = true;
				maxN = min(maxN, n);
				n = max(n - 1, 2);
				unsuccessfulStreak = 0;
			} else {
				if (++unsuccessfulStreak >= increaseThreshold) {
					if (successfulAfterLastIncrease) {
						increaseThreshold += 2;
					}
					n = min(n + 1, maxN);
					successfulAfterLastIncrease = false;
					unsuccessfulStreak = 0;
				}
			}
		}
		if (bestScore == INF) {
			bestScore = bestUnsuccessfulScore;
			bestSolution = bestUnsuccessfulSolution;
		}
		ostringstream bestOutput;
		bestSolution.print(bestOutput, d);
		string toPrint = bestOutput.str();
		if (!isLocal) {
			cout << toPrint;
		}
		if (render) {
			mov.endMovie();
		}
		if (write) {
			writeText(to_string(seed) + ".txt", toPrint);
		}
		if (writeSA) {
			if (bestScore == bestUnsuccessfulScore) {
				stats.clear();
			}
			ostringstream os;
			os << len(stats) << endl;
			for (auto& stat : stats) {
				int np = len(stat.progs);
				os << stat.d << " " << stat.n << " " << stat.successful << " " << np << endl;
				rep0(i, np) {
					os << stat.progs[i] << " " << stat.lefts[i] << endl;
				}
			}
			cout << os.str();
			// writeText("s" + to_string(seed) + ".txt", os.str());
		}
		
		res.score = (ull) round(min(bestScore, 1e12));
		trace("------------------------------------------------------");
		trace("       final score: ", res.score, " (seed=", seed, ")");
		trace("------------------------------------------------------");
	}

private:
	void addSAFrame() {
		mov.target("SA-" + to_string(saMetaIterCount));
		mov.clear();
		mov.fill(1);
		mov.textSize(2);
		mov.textAlign(Movie::CENTER);
		mov.text("N=" + to_string(numBlocks()), d * 2 + 7, -4);
		f1.draw();
		mov.translate(d * 2 + 10, 0);
		f2.draw();
		mov.nextFrame();
	}

	void branchRand() {
		rndBase.nextInt();
		int seed = rndBase.getSeed() + RAND_BRANCH_PLUS;
		rnd2.setSeed(seed);
	}

	void solveFallback() {
		while (true) {
			int i = newBlock();
			addCube(i, Coord(0, 0, 0));
			rep0(t, 2) {
				auto& f = t == 0 ? f1 : f2;
				if (!f.empties.empty()) {
					Coord bestOff;
					int minUncov = f.uncoveredRaw;
					auto emptiesCopy = f.empties;
					for (auto& off : emptiesCopy) {
						f.moveBlock(i, Transform(0, off));
						f.placeBlock(i);
						if (f.uncoveredRaw <= minUncov) {
							minUncov = f.uncoveredRaw;
							bestOff = off;
						}
						f.unplaceBlock(i);
					}
					f.moveBlock(i, Transform(0, bestOff));
					f.placeBlock(i);
				}
			}
			if (isFeasible()) {
				return;
			}
		}
	}

	void removeBlocks() {
		int n = numBlocks();
		rep0r(i, n) {
			rep0(t, 2) {
				auto& f = t == 0 ? f1 : f2;
				if (f.bs[i].placed) {
					f.unplaceBlock(i);
				}
				f.popBlock();
			}
		}
	}

	bool tryWithN(int n, int maxIter) {
		saMetaIterCount++;
		branchRand();
		removeBlocks();
		rep0(i, n) {
			newBlock();
			addCube(i, Coord(0, 0, 0));
			{
				auto off = f1.empties[rnd2.nextInt(len(f1.empties))];
				f1.moveBlock(i, Transform(rnd2.nextInt(24), off));
				f1.placeBlock(i);
			}
			{
				auto off = f2.empties[rnd2.nextInt(len(f2.empties))];
				f2.moveBlock(i, Transform(rnd2.nextInt(24), off));
				f2.placeBlock(i);
			}
		}
		return continueSA(computeScore(), 0, maxIter);
	}

	bool continueSA(double score, int iterFrom, int maxIter) {
		double minScore = INF;
		bool successful = isFeasible();
		bool updated = false;
		double progress = (double) iterFrom / maxIter;
		double succAt = successful ? progress : 1;
		double T = 0;
		Selection sel;

		static Snapshot tempBest;
		Field::saveSnapshot(f1, f2, tempBest);
		int noBestCount = 0;

		int count = 0;

		int progressLevel = 0;
		int progressRenderLevel = 0;

		double allowEatGrow = 1;
		double allowEatTele = 0;
		double tele2 = 0;
		int minUncov = uncoveredSum();

		SAStat* stat = nullptr;
		if (writeSA) {
			stats.emplace_back(seed, d, numBlocks());
			stat = &stats.back();
		}

		rep(iter, iterFrom, maxIter) {
			branchRand();

			totalIter++;
			if (count++ % 500 == 0) {
				progress = (double) iter / maxIter;
				T = params.tFrom * pow(params.tTo / params.tFrom, progress);
				if (timer() >= TL * timeScale)
					break;
				f1.normalizeBlocks();
				f2.normalizeBlocks();

				sel.clear();
				sel.add(0, mix(params.growFrom, params.growTo, progress));
				sel.add(1, mix(params.shrinkFrom, params.shrinkTo, progress));
				sel.add(2, mix(params.teleFrom, params.teleTo, progress));
				allowEatGrow = mix(params.allowEatGrowFrom, params.allowEatGrowTo, progress);
				allowEatTele = mix(params.allowEatTeleFrom, params.allowEatTeleTo, progress);

				if (progress * 10 > progressLevel) {
					if (!writeSA && progressLevel >= params.minCutLevel && minUncov > 0) {
						int n = numBlocks() - 1;
						int dLevel = d - 5;
						double leftScale = 1.0 / (1 + (volumeRatioExp - 1) * 0.5);
						int left = min((int) ceil(minUncov * leftScale), 30) - 1;
						int value = TABLE[dLevel][perimeterLevel][progressLevel - 1][left];
						if (value != -1) {
							double prob = value / 10000.0;
							if (prob <
							    mix(params.pruneFrom, params.pruneTo, pow(progress, params.prunePow))) {
								trace("no hope... (pruning at proress ", progressLevel,
								    "/10, uncov=", minUncov, ", P[", dLevel, "][", perimeterLevel, "][",
								    progressLevel - 1, "][", left, "]=", prob, ", scale=", leftScale, ")");
								solveUnsuccessful();
								if (render)
									addSAFrame();
								return false;
							}
						}
					}

					if (writeSA)
						stat->addProgress(progress, minUncov);

					progressLevel++;
				}
				if (render && progress * 30 > progressRenderLevel) {
					Solution sol;
					Field::saveSolution(f1, f2, sol);
					ostringstream os;
					sol.print(os, d);
					writeText("sa.txt",
					    "// progress Level: " + to_string(progressLevel) +
					        (progressLevel == 0 ? " ----------------------------------------------\n"
					                            : "\n") +
					        os.str(),
					    true);
					progressRenderLevel++;
				}
			}
			if (render && count % 500 == 0) {
				addSAFrame();
			}

			minUncov = min(minUncov, uncoveredSum());

			int index = rnd2.nextInt(numBlocks());
			int index2;
			Grow grow;
			Teleport tp;
			Field* fa = nullptr;
			Field* fb = nullptr;
			Transform ptf;
			int selected = sel.choose(rnd2);
			double scoreDelta;

			switch (selected) {
			case 0: {
				grow = Field::randomGrow(f1, f2, index, rnd2, rnd2.nextFloat() < allowEatGrow);
				if (grow.pos.index == -1) {
					iter--;
					continue;
				}
				auto d = Field::growScoreDelta(f1, f2, grow);
				scoreDelta = computeScore(d.first, d.second);
			} break;
			case 1: {
				grow = f1.randomShrink(index, rnd2);
				if (grow.pos.index == -1) {
					iter--;
					continue;
				}
				
				auto d = Field::ungrowScoreDelta(f1, f2, grow);
				scoreDelta = computeScore(d.first, d.second);
			} break;
			case 2: {
				fa = &(rnd2.nextInt(2) == 0 ? f1 : f2);
				fb = &(fa == &f1 ? f2 : f1);
				tp = fa->randomTeleport(index, rnd2, rnd2.nextFloat() < allowEatTele);
				if (tp.index == -1) {
					iter--;
					continue;
				}
				auto d = Field::teleportScoreDelta(*fa, *fb, tp);
				scoreDelta = computeScore(d.first, d.second);
			} break;
			}

			bool pass = scoreDelta < 0;
			if (!pass) {
				double prob = exp(-scoreDelta / T);
				if (rnd2.nextFloat() < prob) {
					pass = true;
				}
			}

			if (!pass) {
				// do nothing
				switch (selected) {
				case 0: {
				} break;
				case 1: {
				} break;
				case 2: {
				} break;
				}
				assert(score == computeScore());
				continue;
			}
			// apply the changes actually
			switch (selected) {
			case 0: {
				Field::grow(f1, f2, grow);
			} break;
			case 1: {
				Field::ungrow(f1, f2, grow);
			} break;
			case 2: {
				Field::teleport(*fa, *fb, tp);
			} break;
			}

			double newScore = computeScore();
			assert(abs(newScore - (score + scoreDelta)) < 0.1);

			score = newScore;

			if (score >= minScore) {
				noBestCount++;
				if (noBestCount > 5000) {
					score = minScore;
					Field::loadSnapshot(f1, f2, tempBest);
					noBestCount = 0;
				}
			} else {
				// trace("new score for this SA: ", score, " ", iter);
				minScore = score;

				Field::saveSnapshot(f1, f2, tempBest);
				noBestCount = 0;

				if (isFeasible()) {
					successful = true;
					succAt = min(succAt, progress);
					if (writeSA)
						stat->successful = true;
					if (score < bestScore) {
						updated = true;
						bestScore = score;
						Field::saveSolution(f1, f2, bestSolution);
					}
				}
			}
		}
		if (updated)
			trace("best updated! ", (ull) round(bestScore), " N=", numBlocks(), " ", timer(),
			    " (trial count=", count, " succAt=", succAt, ")");
		else if (successful)
			trace("not best", " (trial count=", count, " uncov=", uncoveredSum(), " succAt=", succAt, ")");
		else
			trace("nope", " (trial count=", count, " uncov=", uncoveredSum(), ")");
		if (render)
			addSAFrame();
		if (!successful)
			solveUnsuccessful();
		return successful;
	}

	int uncoveredSum() const {
		return f1.uncoveredRaw + f2.uncoveredRaw;
	}

	bool isFeasible() const {
		return uncoveredSum() == 0;
	}

	void solveUnsuccessful() {
		if (bestScore == INF && (f1.uncoveredRaw == 0 || f2.uncoveredRaw == 0)) {
			int n = numBlocks();

			double coeff = 0;
			rep0(i, n) {
				coeff += 1.0 / len(f1.bs[i].b.poss);
			}
			double baseScore = coeff * 1e9;

			int minN = n + uncoveredSum();
			double possibleUnsuccessfulScore = baseScore + minN * 1e9;
			if (possibleUnsuccessfulScore < bestUnsuccessfulScore) {
				solveFallback();
				int extraBlocks = numBlocks() - n;
				double unsuccessfulScore = baseScore + extraBlocks * 1e9;
				if (unsuccessfulScore < bestUnsuccessfulScore) {
					bestUnsuccessfulScore = unsuccessfulScore;
					trace("best unsuccessful updated: ", (ull) round(bestUnsuccessfulScore),
					    " N=", numBlocks());
					Field::saveSolution(f1, f2, bestUnsuccessfulSolution);
				}
			}
		}
	}

	int numBlocks() {
		return len(f1.bs);
	}

	void unplace(int index) {
		f1.unplaceBlock(index);
		f2.unplaceBlock(index);
	}

	void place(int index) {
		f1.placeBlock(index);
		f2.placeBlock(index);
	}

	int newBlock() {
		int n = numBlocks();
		pushBlock();
		return n;
	}

	void pushBlock() {
		f1.pushBlock();
		f2.pushBlock();
	}

	void popBlock() {
		f1.popBlock();
		f2.popBlock();
	}

	void removeBlock(int index) {
		unplace(index);
		int back = numBlocks() - 1;
		if (index == back) {
			popBlock();
			return;
		}
		unplace(back);
		f1.bs[index] = f1.bs[back];
		f2.bs[index] = f2.bs[back];
		place(index);
		popBlock();
	}

	void addCube(int index, const Coord& c) {
		f1.addCube(index, c);
		f2.addCube(index, c);
	}

	void removeCube(int index, const Coord& c) {
		f1.removeCube(index, c);
		f2.removeCube(index, c);
	}

	void init() {
		d = p.d;
		
		bool dhigh = d >= 10;
		bool phigh = p.perimeterRatio() >= PERIMETER_RATIOS_MID[d - 5];
		params.set(dhigh, phigh);
		
		f1.init(d, p.s1, params);
		f2.init(d, p.s2, params);
		int e1 = len(f1.empties);
		int e2 = len(f2.empties);
		volumeRatioExp = exp((double) max(e1, e2) / min(e1, e2) - 1);
		perimeterRatioNormalized = (p.perimeterRatio() - PERIMETER_RATIOS[d - 5][0]) /
		    (PERIMETER_RATIOS[d - 5][1] - PERIMETER_RATIOS[d - 5][0]);
		perimeterLevel = clamp((int) (perimeterRatioNormalized * 5), 0, 4);
	}
};

void analyzeStats(int num) {
	vector<bool> dhigh;
	vector<bool> phigh;
	vector<double> pratios;
	vector<double> vratios;
	vector<double> pratiosd[10];
	double minRatio[16];
	double maxRatio[16];
	rep0(d, 15) {
		minRatio[d] = 1e9;
		maxRatio[d] = -1e9;
	}
	vector<int> ds;
	rep0(seed, num) {
		if (seed % 100 == 0)
			trace("loading ", seed, "...");
		Solver sol;
		sol.load(seed);
		auto& p = sol.p;
		int d = p.d;
		ds.push_back(d);
		double pratio = p.perimeterRatio();
		double vratio = sol.volumeRatioExp;
		pratios.push_back(pratio);
		pratiosd[d - 5].push_back(pratio);
		vratios.push_back(vratio);
		minRatio[d] = min(minRatio[d], pratio);
		maxRatio[d] = max(maxRatio[d], pratio);
		
		dhigh.push_back(sol.d >= 10);
		phigh.push_back(pratio >= PERIMETER_RATIOS_MID[d - 5]);
	}
	
	vector<int> groups[2][2];
	rep0(seed, num) {
		groups[dhigh[seed]][phigh[seed]].push_back(seed);
	}
	trace("D-lo P-lo (", groups[0][0].size(), "): ", groups[0][0]);
	trace("D-lo P-hi (", groups[0][1].size(), "): ", groups[0][1]);
	trace("D-hi P-lo (", groups[1][0].size(), "): ", groups[1][0]);
	trace("D-hi P-hi (", groups[1][1].size(), "): ", groups[1][1]);
	return;

	rep0(dl, 10) {
		int d = 5 + dl;
		ostringstream out;
		rep0(seed, num) {
			if (ds[seed] == d) {
				double p = (pratios[seed] - minRatio[d]) / (maxRatio[d] - minRatio[d]);
				if (USED_BLOCKS[seed] != 100) {
					out << p << " " << vratios[seed] << " " << USED_BLOCKS[seed] << ";";
				}
			}
		}
		writeText("data" + to_string(d) + ".txt", out.str());
	}
	return;

	trace("d ratios");
	rep(d, 5, 14 + 1) {
		trace(minRatio[d], " ", maxRatio[d]);
	}
	vector<int> pLevels;
	rep0(seed, num) {
		int d = ds[seed];
		double ratio = pratios[seed];
		double r = (ratio - minRatio[d]) / (maxRatio[d] - minRatio[d]);
		pLevels.push_back((int) ((r - 1e-5) * 5));
		trace(seed, ": ", pLevels.back());
	}

	constexpr int MAX_LEFT = 30;
	int counts[10][5][10][MAX_LEFT + 1] = {};
	int countsOK[10][5][10][MAX_LEFT + 1] = {};
	int countsNG[10][5][10][MAX_LEFT + 1] = {};

	int maxN = 0;
	rep0(seed, num) {
		ifstream in;
		in.open("sa/" + to_string(seed) + ".txt", ios::in);
		int numStats;
		in >> numStats;
		// trace("seed ", seed, " ns=", numStats);
		rep0(statIndex, numStats) {
			int np;
			int d;
			int n;
			bool successful;
			in >> d >> n >> successful >> np;
			// if (n >= 30) {
			// 	trace(seed, " ", n, " ", successful);
			// }
			int di = d - 5;
			int pi = pLevels[seed];
			// trace("d=", d, " n=", n, " suc=", successful, " np=", np);
			rep0(i, np) {
				double prog;
				int left;
				in >> prog >> left;
				int p = (int) (prog * 10);
				left = min(left, MAX_LEFT);
				counts[di][pi][p][left]++;
				if (successful)
					countsOK[di][pi][p][left]++;
				else
					countsNG[di][pi][p][left]++;
				// rep0(l, left + 1) {
				// 	counts[di][pi][p][l]++;
				// 	if (successful)
				// 		countsOK[di][pi][p][l]++;
				// 	else
				// 		countsNG[di][pi][p][l]++;
				// }
			}
		}
	}
	// return;

	trace("{");
	rep0(di, 10) {
		trace("{");
		rep0(pi, 5) {
			trace("{");
			rep(p, 1, 10) {
				vector<double> ps;
				rep(l, 1, MAX_LEFT + 1) {
					int total = counts[di][pi][p][l];
					int succ = countsOK[di][pi][p][l];
					int fail = countsNG[di][pi][p][l];
					ps.push_back(total == 0 ? -1 : (int) round((double) succ / total * 10000));
				}
				trace("{", ps, "},");
			}
			trace("},");
		}
		trace("},");
	}
	trace("}");
}

int main() {
#if 1
	isLocal = false;
	timer(true);
	Solver sol;
	sol.load(cin);
	sol.solve();
	trace("total iter: ", sol.totalIter);
#elif 0
	isLocal = true;
	timer(true);
	Solver sol;
	sol.load(0);
	sol.solve();
#elif 0
	analyzeStats(5000);
#elif 0
	isLocal = true;
	int seed;
	cin >> timeScale;
	cin >> seed;

	timer(true);
	Solver sol;
	sol.load(seed);
	sol.solve();
	trace("time: ", timer());

	cout << sol.res.score << " " << sol.totalIter << endl;
#elif 1
	int seed;
	cin >> timeScale;
	cin >> seed;
	SAParams params;

	// cin >> params.weight1;
	// cin >> params.weight2;
	// cin >> params.weight3;
	// cin >> params.weight4;
	// cin >> params.weightVol1;
	// cin >> params.weightVol2;
	// cin >> params.weightVol3;
	// cin >> params.singleIterations;
	// cin >> params.minCutLevel;
	// cin >> params.pruneFrom;
	// cin >> params.pruneTo;
	// cin >> params.prunePow;
	// cin >> params.tFrom;
	// cin >> params.tTo;
	// cin >> params.growFrom;
	// cin >> params.growTo;
	// cin >> params.shrinkFrom;
	// cin >> params.shrinkTo;
	// cin >> params.teleFrom;
	// cin >> params.teleTo;
	// cin >> params.allowEatGrowFrom;
	// cin >> params.allowEatGrowTo;
	// cin >> params.allowEatTeleFrom;
	// cin >> params.allowEatTeleTo;
	// debug = false;

	isLocal = true;
	Solver& sol = *(new Solver());
	sol.params = params;

	{
		timer(true);
		sol.load(seed);
		sol.solve();
		trace("time: ", timer());
		if (!sol.writeSA) {
			cout << sol.res.score << " "
			     << (ull) round(seed < array_len(BASE_SCORES)
			                ? (double) sol.res.score / BASE_SCORES[seed] * 1e9
			                : 1e12)
			     << " " << (sol.bestScore == sol.bestUnsuccessfulScore ? 100 : sol.bestSolution.n) << " "
			     << sol.totalIter << endl;
		}
		delete &sol;
	}
#endif
}
