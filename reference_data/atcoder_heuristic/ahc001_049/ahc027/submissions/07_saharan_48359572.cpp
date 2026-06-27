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
#include <unordered_set>
#include <utility>
#include <vector>

#include <mutex>
#include <thread>

#ifdef ONLINE_JUDGE
#define NDEBUG
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#else
#undef NDEBUG
#endif
#include <cassert>

using namespace std;
using uc = unsigned char;
using us = unsigned short;
using ui = unsigned int;
using ull = unsigned long long;
using ll = long long;
using pii = pair<int, int>;
using pdi = pair<double, int>;
#define array_len(a) (int) (sizeof(a) / sizeof(decltype(a[0])))
#define rep(i, from, until) for (int i = (from); i < (until); i++)
#define repr(i, from, until) for (int i = (until) -1; i >= (from); i--)
#define rep0(i, until) rep(i, 0, until)
#define rep0r(i, until) repr(i, 0, until)

template <typename T>
int len(const vector<T>& a) {
	return (int) a.size();
}

template <typename T>
int len(const set<T>& a) {
	return (int) a.size();
}

template <typename T>
int len(const deque<T>& a) {
	return (int) a.size();
}

template <size_t T>
int len(const bitset<T>& a) {
	return (int) a.count();
}

template <class T>
T mix(T a, T b, double ratio) {
	return a + ratio * (b - a);
}

template <class T>
string tos(T a) {
	return to_string(a);
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

class Rng {
public:
	Rng() : y(0x95fdd98a11be9c09ull) {
	}

	ull getState() {
		return y;
	}

	void seed(int s) {
		y = s + INCR;
		next();
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
	static constexpr ull MULT = 0x8b46ad15ae59daadull;
	static constexpr ull INCR = 0xf51827be20401689ull;
	ull y;

	int next() {
		ui r = (ui) (y >> 59);
		y = y * MULT + INCR;
		y ^= y >> 18;
		ui t = (ui) (y >> 27);
		return (int) ((t >> r | t << (-r & 31)) & 0x7fffffff);
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

//
// --- trace functions ---
//

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

//
// --- file utils ---
//

string pad4(int n) {
	return (n < 10 ? "000" : n < 100 ? "00" : n < 1000 ? "0" : "") + tos(n);
}

string seedToFileName(int seed) {
	return "in/" + pad4(seed) + ".txt";
}

bool writeTextSafe(string fileName, string text, bool append = false) {
	ofstream fout;
	fout.open(fileName, append ? ios::out | ios::app : ios::out);
	if (!fout)
		return false;
	fout << text;
	fout.close();
	return true;
}

pair<string, bool> readTextSafe(string fileName) {
	ifstream fin;
	fin.open(fileName, ios::in);
	if (!fin)
		return make_pair("", false);
	string res;
	string line;
	while (getline(fin, line)) {
		res += line;
		res += "\n";
	}
	return make_pair(res, true);
}

void writeText(string fileName, string text, bool append = false) {
	auto res = writeTextSafe(fileName, text, append);
	assert(res);
}

string readText(string fileName) {
	auto res = readTextSafe(fileName);
	assert(res.second);
	return res.first;
}

//
// --- movie lib ---
//

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
bool render = false;
constexpr bool USE_TL = true;
constexpr int ITER = 100000000;
constexpr int TL = USE_TL ? 1950 : 10000000;

// define N and stuff here
constexpr int N_SHIFT = 6;
constexpr int MAX_N = 1 << N_SHIFT; // use PoT
constexpr int MAX_N2 = MAX_N * MAX_N;
constexpr int N_MASK = MAX_N - 1;
constexpr int INF = 10000000;
constexpr int DIRS[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
constexpr int UP = 0;
constexpr int DOWN = 1;
constexpr int LEFT = 2;
constexpr int RIGHT = 3;
#define rep2() rep0(i, n) rep0(j, n)
#define repd() rep0(dir, 4)

constexpr int flipDir(int dir) {
	return dir ^ 1;
}

#define inBounds(i, j) ((i) >= 0 && (i) < n && (j) >= 0 && (j) < n)

constexpr int toIndex(int i, int j) {
	return i << N_SHIFT | j;
}

class Pos {
public:
	int index;

	constexpr Pos(int index = 0) : index(index) {
	}

	constexpr Pos(int i, int j) : index(i << N_SHIFT | j) {
		// assert(i >= 0 && j >= 0);
	}

	constexpr int i() const {
		return index >> N_SHIFT;
	}

	constexpr int j() const {
		return index & N_MASK;
	}

	constexpr Pos move(int dir) const {
		return Pos(i() + DIRS[dir][0], j() + DIRS[dir][1]);
	}

	int distTo(Pos pos) const {
		return abs(i() - pos.i()) + abs(j() - pos.j());
	}

	constexpr bool operator<(const Pos& rhs) const {
		return index < rhs.index;
	}

	constexpr bool operator==(const Pos& rhs) const {
		return index == rhs.index;
	}

	constexpr bool operator!=(const Pos& rhs) const {
		return index != rhs.index;
	}

	friend ostream& operator<<(ostream& out, const Pos& pos) {
		out << "(" << pos.i() << ", " << pos.j() << ")";
		return out;
	}
};

constexpr int toDir(Pos p1, Pos p2) {
	int di = p2.i() - p1.i();
	int dj = p2.j() - p1.j();
	assert((di == 0) ^ (dj == 0));
	return di < 0 ? UP : di > 0 ? DOWN : dj < 0 ? LEFT : RIGHT;
}

template <typename T>
class Map {
private:
	T data[MAX_N2] = {};

public:
	Map() {
	}

	void clear(int val) {
		memset(data, val, sizeof(data));
	}

	T& at(Pos pos) {
		return data[pos.index];
	}

	const T& at(Pos pos) const {
		return data[pos.index];
	}

	T& at(int i, int j) {
		return at(Pos(i, j));
	}

	const T& at(int i, int j) const {
		return at(Pos(i, j));
	}
};

class Problem {
private:
	bool wallI[MAX_N - 1][MAX_N] = {};
	bool wallJ[MAX_N][MAX_N - 1] = {};

public:
	int n = 0;
	Map<int> ds;

	Problem() {
	}

	void load(istream& in) {
		in >> n;
		rep0(i, n - 1) {
			string line;
			in >> line;
			rep0(j, n) {
				wallI[i][j] = line[j] == '1';
			}
		}
		rep0(i, n) {
			string line;
			in >> line;
			rep0(j, n - 1) {
				wallJ[i][j] = line[j] == '1';
			}
		}
		rep2() {
			in >> ds.at(i, j);
		}
	}

	int d(int i, int j) const {
		assert(inBounds(i, j));
		return ds.at(i, j);
	}

	bool wall(int i, int j, int dir) const {
		assert(inBounds(i, j));
		switch (dir) {
		case UP:
			return i == 0 || wallI[i - 1][j];
		case DOWN:
			return i == n - 1 || wallI[i][j];
		case LEFT:
			return j == 0 || wallJ[i][j - 1];
		case RIGHT:
			return j == n - 1 || wallJ[i][j];
		default:
			assert(false);
			return true;
		}
	}

	int d(Pos pos) const {
		return d(pos.i(), pos.j());
	}

	int wall(Pos pos, int dir) const {
		return wall(pos.i(), pos.j(), dir);
	}

	void drawWalls() const {
		mov.stroke(0);
		mov.strokeWeight(0.1);
		rep2() {
			Pos pos(i, j);
			if (i < n - 1 && wall(pos, DOWN))
				mov.line(j, i + 1, j + 1, i + 1);
			if (j < n - 1 && wall(pos, RIGHT))
				mov.line(j + 1, i, j + 1, i + 1);
		}
	}
};

class DijkstraRes {
private:
public:
	Map<bool> vis;
	Map<Pos> prevs;
	Map<double> dists;
	Pos goal;
	vector<Pos> path;

	DijkstraRes() {
	}

	void clear(int n) {
		rep2() {
			Pos pos(i, j);
			vis.at(pos) = false;
			prevs.at(pos) = Pos(-1);
			dists.at(pos) = INF;
		}
	}

	void restorePath(Pos to) {
		path.clear();
		Pos pos = to;
		while (prevs.at(pos).index != -1) {
			path.push_back(pos);
			pos = prevs.at(pos);
		}
		reverse(path.begin(), path.end());
	}
};

class Visit {
private:
	int n = 0;
	int lbIndex = -1;
	int ubIndex = -1;

public:
	vector<int> times;
	vector<int> toInsert;

	Visit() {
	}

	int count() {
		return n;
	}

	void clear() {
		n = 0;
		times.clear();
	}

	void insert(int t) {
		int i = 0;
		while (i < n && times[i] < t)
			i++;
		assert(i == n || times[i] != t);
		times.insert(times.begin() + i, t);
		n++;
	}

	void erase(int t) {
		rep0(i, n) {
			if (times[i] == t) {
				times.erase(times.begin() + i);
				n--;
				return;
			}
		}
		assert(false);
	}

	void shift(int from, int amount) {
		rep0r(i, n) {
			if (times[i] < from)
				return;
			times[i] += amount;
		}
	}

	int lowerBound(int t, int l) {
		assert(n > 0);
		int ok = -1;
		int ng = n;
		while (ng - ok > 1) {
			int mid = ng + ok >> 1;
			if (times[mid] <= t)
				ok = mid;
			else
				ng = mid;
		}
		if (ok == -1) {
			return times[lbIndex = n - 1] - l;
		} else {
			return times[lbIndex = ok];
		}
		// rep0r(i, n) {
		// 	if (times[i] <= t)
		// 		return times[lbIndex = i];
		// }
		// return times[lbIndex = n - 1] - l;
	}

	int upperBound(int t, int l) {
		assert(n > 0);
		int ok = n;
		int ng = -1;
		while (ok - ng > 1) {
			int mid = ng + ok >> 1;
			if (times[mid] > t)
				ok = mid;
			else
				ng = mid;
		}
		if (ok == n) {
			return times[ubIndex = 0] + l;
		} else {
			return times[ubIndex = ok];
		}
		// rep0(i, n) {
		// 	if (times[i] > t)
		// 		return times[ubIndex = i];
		// }
		// return times[ubIndex = 0] + l;
	}

	// O(log(n))
	ll computeSumDiff(int l, int from, int length, int lengthAfter) {
		if (n == 0) {
			return computeSum(toInsert, l + (lengthAfter - length));
		}
		ll res = 0;

		int lo = lowerBound(from - 1, l);
		if (from - lo > l - length) {
			// no points outside the cut area
			res -= computeSum(l);
			res += computeSum(toInsert, l + (lengthAfter - length));
		} else {
			// there is at least one
			int hi = from + length <= l ? upperBound(from + length - 1, l)
			                            : upperBound(from + length - 1 - l, l) + l;
			int i = lbIndex;
			do {
				int it = interval(i, l);
				// trace("removing interval at index=", i, " (", it, ")");
				res -= weight(it);
				i++;
				if (i == n)
					i = 0;
			} while (i != ubIndex);
			int hiAfter = hi + (lengthAfter - length);
			int prev = lo;
			for (int t : toInsert) {
				int it = t - prev;
				// trace("adding interval [", prev, ", ", t, "] (", it, ")");
				res += weight(it);
				prev = t;
			}
			int it = hiAfter - prev;
			// trace("adding interval [", prev, ", ", hiAfter, "] (", it, ")");
			res += weight(it);
		}
		return res;
	}

	// O(n)
	void rotate(int l, int amount) {
		int minVal = INF;
		int minIndex = -1;
		rep0(i, n) {
			times[i] = (times[i] + l - amount) % l;
			if (times[i] < minVal) {
				minVal = times[i];
				minIndex = i;
			}
		}
		std::rotate(times.begin(), times.begin() + minIndex, times.end());
	}

	// O(n)
	void applyEdit(int l, int from, int length, int lengthAfter) {
		assert(from + length <= l);
		int newL = l + (lengthAfter - length);
		if (n == 0) {
			for (int t : toInsert) {
				insert(t);
			}
			toInsert.clear();
			return;
		}
		int insertPos = n;
		int eraseNum = 0;
		rep0(i, n) {
			if (times[i] >= from && insertPos == n) {
				insertPos = i;
			}
			if (times[i] >= from && times[i] < from + length) {
				eraseNum++;
			} else if (times[i] >= from + length) {
				times[i] += lengthAfter - length;
			}
		}
		if (eraseNum > 0) {
			times.erase(times.begin() + insertPos, times.begin() + insertPos + eraseNum);
		}
		times.insert(times.begin() + insertPos, toInsert.begin(), toInsert.end());
		n = len(times);
		toInsert.clear();
	}

	ll computeSum(int l) const {
		return computeSum(times, l);
	}

	static ll computeSum(const vector<int>& times, int l) {
		int n = len(times);
		ll res = 0;
		rep0(i, n - 1) {
			assert(times[i + 1] - times[i] > 0);
			res += weight(times[i + 1] - times[i]);
		}
		assert(times[0] - (times[n - 1] - l) > 0);
		res += weight(times[0] - (times[n - 1] - l));
		return res;
	}

	void dump(int l) {
		trace("[", times, "]");
		int n = len(times);
		rep0(i, n - 1) {
			trace(times[i + 1] - times[i]);
		}
		trace(times[0] - (times[n - 1] - l));
	}

	static ll weight(int interval) {
		return (ll) interval * (interval - 1) >> 1;
	}

private:
	int interval(int index, int l) const {
		return index == n - 1 ? times[0] - (times[n - 1] - l) : times[index + 1] - times[index];
	}
};

class Node {
private:
public:
	Pos v;
	int index = 0;
	Node* prev = nullptr;
	Node* next = nullptr;
	vector<Node*> neighbors;

	Node() {
	}
};

class State {
private:
public:
	int n = 0;
	vector<Pos> cycle;
	Map<Visit> visits;
	ull sum = 0;

	State() {
	}

	void clear(int n) {
		this->n = n;
		cycle.clear();
		rep2() {
			visits.at(i, j).clear();
		}
		sum = 0;
	}

	void rotate(int amount) {
		if (amount == 0)
			return;
		int l = len(cycle);
		std::rotate(cycle.begin(), cycle.begin() + amount, cycle.end());
		rep2() {
			visits.at(i, j).rotate(l, amount);
		}
	}

	void normalize() {
		int l = len(cycle);
		rep0(i, l) {
			if (cycle[i] == Pos(0, 0)) {
				rotate(i);
				return;
			}
		}
	}
};

struct SolverResult {
	int score = 0;
};

// Map<Map<int>> distMap;
int distMap[16777216];

int& distBetween(Pos p1, Pos p2) {
	return distMap[p1.index << 12 | p2.index];
}

class Solver {
public:
	int seed;
	Rng rng;
	SolverResult res;
	string seedString = "XXXX";
	int n = 0;
	Problem p;
	vector<Pos> ans;
	DijkstraRes dres;
	int numG = 0;
	Map<int> groups;
	vector<int> groupSizes;
	vector<int> groupCycleLengths;
	vector<vector<Pos>> groupAreas;
	vector<int> groupDs;
	vector<vector<int>> groupDistMap;
	vector<vector<array<Pos, 2>>> groupDistEdges;

	State st;

	Solver() {
	}

	void load(istream& in) {
		p.load(in);
		n = p.n;
		render &= isLocal;
		if (render)
			mov.beginMovie(seedString + "_m.smv");
		init();
	}

	void load(int seed) {
		this->seed = seed;
		seedString = tos(seed);
		istringstream in(readText(seedToFileName(seed)));
		isLocal = true;
		load(in);
	}

	void solve() {

		solveMain();

		if (render)
			mov.endMovie();

		if (isLocal) {
			// res.score = -1;
			ostringstream out;
			print(out);
			writeTextSafe("out/" + pad4(seed) + ".txt", out.str());
		} else {
			print(cout);
		}
	}

private:
	void init() {
		computeDists();
		splitAreas();
	}

	void bilateralBlur(Map<double>& logds) const {
		Map<double> tmp;
		rep2() {
			Pos pos(i, j);
			int r = 4;
			double sigma1 = 1;
			double sigma2 = 0.2;
			double sum = 0;
			double wsum = 0;
			rep(di, -r, r + 1) {
				rep(dj, -r, r + 1) {
					int i2 = i + di;
					int j2 = j + dj;
					if (!inBounds(i2, j2))
						continue;
					Pos pos2(i2, j2);
					int dist = distBetween(pos, pos2);
					double distWeight = exp(-dist * dist / (sigma1 * sigma1));
					double diff = logds.at(pos2) - logds.at(pos);
					double diffWeight = exp(-diff * diff / (sigma2 * sigma2));
					if (diff < 0)
						diffWeight *= 0.75;
					double weight = distWeight * diffWeight;
					sum += weight * logds.at(pos2);
					wsum += weight;
				}
			}
			tmp.at(pos) = sum / wsum;
		}
		logds = tmp;
	}

	vector<int> splitGroup(const vector<Pos>& area, int count) {
		vector<Pos> centers;
		vector<int> indices;
		vector<int> counts(count, 0);
		int m = len(area);
		rep0(sg, count) {
			centers.emplace_back(rng.nextInt(n), rng.nextInt(n));
		}
		indices.resize(m);
		auto bestIndices = indices;
		double best = INF;
		static vector<Pos> path[1000];
		rep0(iter, 100) {
			rep0(sg, count) {
				while (true) {
					centers[sg] = area[rng.nextInt(m)];
					bool ng = false;
					rep0(i, sg) {
						if (centers[sg] == centers[i]) {
							ng = true;
							break;
						}
					}
					if (!ng)
						break;
				}
				counts[sg] = 0;
				path[sg].clear();
			}
			rep0(i, m) {
				Pos pos = area[i];
				int minIndex = -1;
				int minDist = INF;
				rep0(sg, count) {
					int dist = distBetween(pos, centers[sg]);
					if (dist < minDist) {
						minDist = dist;
						minIndex = sg;
					}
				}
				indices[i] = minIndex;
				counts[minIndex]++;
				path[minIndex].push_back(pos);
			}
			// double gsigma = 0;
			// rep0(sg, count) {
			// 	double mi = 0;
			// 	double mj = 0;
			// 	int l = len(path[sg]);
			// 	for (Pos pos : path[sg]) {
			// 		mi += pos.i();
			// 		mj += pos.j();
			// 	}
			// 	mi /= l;
			// 	mj /= l;
			// 	double var = 0;
			// 	for (Pos pos : path[sg]) {
			// 		var += (pos.i() - mi) * (pos.i() - mi);
			// 		var += (pos.j() - mj) * (pos.j() - mj);
			// 	}
			// 	var = sqrt(var);
			// 	gsigma += var;
			// }
			int distSum = 0;
			rep0(sg, count) {
				optimizePath(path[sg], true);
				distSum += cycleDist(path[sg]);
			}
			double sum = 0;
			rep0(sg, count) {
				sum += counts[sg];
			}
			double mean = sum / count;
			double var = 0;
			rep0(sg, count) {
				var += (counts[sg] - mean) * (counts[sg] - mean);
			}
			double score = sqrt(var) + distSum;
			if (score < best) {
				best = score;
				bestIndices = indices;
			}
		}
		return bestIndices;
	}

	void splitAreas() {
		Map<double> ds;
		Map<double> logds;
		rep2() {
			double d = p.ds.at(i, j);
			ds.at(i, j) = d;
			logds.at(i, j) = log10(max(2.0, d));
		}

		// d map
		if (render) {
			mov.target("split");
			mov.clearFrame();
			mov.stroke(1);
			mov.strokeWeight(0.1);
			mov.noFill();
			mov.rect(0, 0, n, n);
			mov.fill(0, 1, 0);
			mov.noStroke();
			rep2() {
				mov.tooltip(tos(ds.at(i, j)));
				double r = log10(ds.at(i, j)) / 3;
				mov.fill(r, 1 - r, 0);
				mov.rect(j, i, 1, 1);
				mov.noTooltip();
			}
			p.drawWalls();
			mov.endFrame();
		}

		rep0(iter, 40) {
			// smoothed d map
			bilateralBlur(logds);
			if (render && iter % 10 == 9) {
				mov.clearFrame();
				mov.stroke(1);
				mov.strokeWeight(0.1);
				mov.noFill();
				mov.rect(0, 0, n, n);
				mov.fill(0, 1, 0);
				mov.noStroke();
				rep2() {
					mov.tooltip(tos(pow(10, logds.at(i, j))));
					double r = logds.at(i, j) / 3;
					mov.fill(r, 1 - r, 0);
					mov.rect(j, i, 1, 1);
					mov.noTooltip();
				}
				p.drawWalls();
				mov.endFrame();
			}
		}

		const int NUM_LEVELS = 10;
		Map<int> levels;
		rep2() {
			Pos pos(i, j);
			levels.at(i, j) = (int) (logds.at(pos) / 3 * NUM_LEVELS);
		}
		if (render) {
			mov.clearFrame();
			mov.stroke(1);
			mov.strokeWeight(0.1);
			mov.noFill();
			mov.rect(0, 0, n, n);
			mov.fill(0, 1, 0);
			mov.noStroke();
			rep2() {
				double r = (levels.at(i, j) + 0.5) / NUM_LEVELS;
				mov.tooltip(tos(levels.at(i, j)));
				mov.fill(r, 1 - r, 0);
				mov.rect(j, i, 1, 1);
				mov.noTooltip();
			}
			p.drawWalls();
			mov.endFrame();
		}
		Map<bool> visited;
		vector<Pos> area;
		auto dfsArea = [&](Pos at, int& minAdjLevel) {
			area.clear();
			int level = levels.at(at);
			minAdjLevel = INF;
			deque<Pos> q;
			q.emplace_back(at);
			area.push_back(at);
			visited.at(at) = true;
			int count = 1;
			while (!q.empty()) {
				Pos pos = q.front();
				q.pop_front();
				repd() {
					if (!p.wall(pos, dir)) {
						Pos pos2 = pos.move(dir);
						int level2 = levels.at(pos2);
						if (level2 != level && level2 >= level - 1) {
							minAdjLevel = min(minAdjLevel, level2);
						}
						if (!visited.at(pos2) && level2 == level) {
							visited.at(pos2) = true;
							area.push_back(pos2);
							q.push_back(pos2);
							count++;
						}
					}
				}
			}
			return count;
		};
		const int MIN_AREA = n / 3;
		rep0(level, NUM_LEVELS) {
			rep2() {
				Pos pos(i, j);
				if (levels.at(pos) != level || visited.at(pos))
					continue;
				int promoteTo;
				int size = dfsArea(pos, promoteTo);
				if (size < MIN_AREA && promoteTo != INF) {
					for (Pos pos2 : area) {
						levels.at(pos2) = promoteTo;
						visited.at(pos2) = false;
					}
				}
			}
		}
		if (render) {
			mov.clearFrame();
			mov.stroke(1);
			mov.strokeWeight(0.1);
			mov.noFill();
			mov.rect(0, 0, n, n);
			mov.fill(0, 1, 0);
			mov.noStroke();
			rep2() {
				double r = (levels.at(i, j) + 0.5) / NUM_LEVELS;
				mov.tooltip(tos(levels.at(i, j)));
				mov.fill(r, 1 - r, 0);
				mov.rect(j, i, 1, 1);
				mov.noTooltip();
			}
			p.drawWalls();
			mov.endFrame();
		}
		visited.clear(0);
		numG = 0;
		groupAreas.clear();
		vector<vector<Pos>> paths;
		rep2() {
			Pos pos(i, j);
			if (visited.at(pos))
				continue;
			int dummy;
			int size = dfsArea(pos, dummy);
			vector<Pos> path;
			for (Pos pos : area) {
				path.push_back(pos);
			}
			const int TARGET_GROUP_SIZE = 35;
			int numSubs = (size + TARGET_GROUP_SIZE - 1) / TARGET_GROUP_SIZE;

			auto subs = splitGroup(path, numSubs);

			rep0(sg, numSubs) {
				int i0 = size * sg / numSubs;
				int i1 = size * (sg + 1) / numSubs;
				vector<Pos> sub;
				rep0(i, len(path)) {
					if (subs[i] == sg) {
						sub.push_back(path[i]);
						groups.at(path[i]) = numG;
					}
				}
				groupAreas.push_back(sub);
				optimizePath(sub, true);

				int l = len(sub);
				int cycleLength = 0;
				rep0(i, l) {
					cycleLength += distBetween(sub[i], sub[(i + 1) % l]);
				}
				groupCycleLengths.push_back(cycleLength);
				paths.push_back(sub);
				numG++;
			}
		}

		rep0(g, len(paths)) {
			int dsum = 0;
			int size = len(paths[g]);
			for (Pos pos : paths[g]) {
				dsum += p.ds.at(pos);
			}
			groupDs.push_back(dsum);
			groupSizes.push_back(size);
		}

		int st = timer();
		groupDistMap.resize(numG);
		groupDistEdges.resize(numG);
		rep0(g, numG) {
			groupDistMap[g].resize(numG);
			groupDistEdges[g].resize(numG);
		}
		rep(g1, 1, numG) {
			rep0(g2, g1) {
				int minDist = INF;
				Pos minp1;
				Pos minp2;
				for (Pos p1 : paths[g1]) {
					for (Pos p2 : paths[g2]) {
						int dist = distBetween(p1, p2);
						if (dist < minDist) {
							minDist = dist;
							minp1 = p1;
							minp2 = p2;
						}
					}
				}
				groupDistMap[g1][g2] = minDist;
				groupDistMap[g2][g1] = minDist;
				groupDistEdges[g1][g2] = {minp1, minp2};
				groupDistEdges[g2][g1] = {minp2, minp1};
			}
		}
		rep0(g, numG) {
			trace(g, ": size=", len(paths[g]), " d=", groupDs[g]);
		}
		trace("group dist: ", timer() - st, " ms");
		if (render) {
			mov.clearFrame();
			mov.stroke(1);
			mov.strokeWeight(0.1);
			mov.noFill();
			mov.rect(0, 0, n, n);
			mov.fill(0, 1, 0);
			mov.noStroke();
			rep2() {
				double ang = 2.4 * groups.at(i, j);
				double da = 2.0943951023931953333;
				mov.fill(sin(ang) * 0.5 + 0.5, sin(ang - da) * 0.5 + 0.5, sin(ang + da) * 0.5 + 0.5);
				mov.rect(j, i, 1, 1);
			}
			p.drawWalls();
			mov.endFrame();

			mov.clearFrame();
			mov.stroke(1);
			mov.strokeWeight(0.1);
			mov.noFill();
			mov.rect(0, 0, n, n);
			mov.noStroke();
			rep2() {
				mov.tooltip(tos(groups.at(i, j)));
				mov.fill(0.3 + groups.at(i, j) / (double) numG * 0.7);
				mov.rect(j, i, 1, 1);
				mov.noTooltip();
			}
			p.drawWalls();
			mov.strokeWeight(0.2);
			rep0(g, numG) {
				auto& path = paths[g];
				int l = len(path);
				rep0(i, l) {
					Pos p1 = path[i];
					Pos p2 = path[(i + 1) % l];
					double ang = 2.4 * g;
					double da = 2.0943951023931953333;
					mov.stroke(sin(ang) * 0.5 + 0.5, sin(ang - da) * 0.5 + 0.5, sin(ang + da) * 0.5 + 0.5);
					mov.line(p1.j() + 0.5, p1.i() + 0.5, p2.j() + 0.5, p2.i() + 0.5);
					mov.rect(p1.j() + 0.5, p1.i() + 0.5, 1e-6, 1e-6);
				}
			}
			mov.endFrame();
		}
		vector<int> cycle;
		rep0(g, numG) {
			cycle.push_back(g);
		}

		{
			vector<int> lastVisit(numG, -1);
			int currentGroup = 0;
			int time = 0;
			rep0(t, 100) {
				int next = -1;
				double best = 0;
				rep0(g, numG) {
					int t0 = lastVisit[g];
					int moveTime = groupDistMap[currentGroup][g];
					int cycleTime = groupCycleLengths[g];
					int t1 = time + moveTime + cycleTime;
					double meanA = (t1 - t0) * groupDs[g] / (double) groupSizes[g];
					double meanD = groupDs[g] / (double) groupSizes[g];
					double dlevel = meanD * 1e-3;
					double score = meanA * mix(500.0, 1.0, dlevel);
					if (score > best) {
						best = score;
						next = g;
					}
				}
				assert(next != -1);
				int g = next;
				time += groupDistMap[currentGroup][g] + groupCycleLengths[g];
				lastVisit[g] = time;
				currentGroup = g;
				cycle.push_back(g);
				trace(g);
			}
			trace(lastVisit);
		}

		int l = 0;
		double score = evaluateGroupCycle(cycle, l);
		trace("initial score: ", (int) round(score), " (l=", l, ")");

		double temp = 1.0;
		double bestScore = score;
		vector<int> bestCycle = cycle;
		int numIters = 200000;
		rep0(iter, numIters) {
			double progress = iter / (double) numIters;
			// double progress = timer() / 1000.0;
			// if (progress > 1)
			// 	break;
			temp = pow(1000, 1 - progress) * pow(1, progress);
			auto oldCycle = cycle;
			if (rng.nextFloat() < 0.5) {
				if (rng.nextFloat() < 0.5 && len(cycle)) {
					int trials = 0;
					int i;
					do {
						i = rng.nextInt(len(cycle));
					} while (count(cycle.begin(), cycle.end(), cycle[i]) == 1 && ++trials < 1000);
					if (trials < 1000)
						cycle.erase(cycle.begin() + i);
				} else {
					int g = rng.nextInt(numG);
					int i = rng.nextInt(len(cycle) + 1);
					cycle.insert(cycle.begin() + i, g);
				}
			} else if (rng.nextFloat() < 0.8) {
				int i, length, j;
				do {
					length = min(len(cycle) - 1, (int) (10 * pow(rng.nextFloat(), 3) + 1));
					i = rng.nextInt(len(cycle) - length);
					j = rng.nextInt(len(cycle));
				} while (j >= i && j < i + length);
				vector<int> sub(cycle.begin() + i, cycle.begin() + i + length);
				cycle.erase(cycle.begin() + i, cycle.begin() + i + length);
				if (j > i)
					j -= length;
				cycle.insert(cycle.begin() + j, sub.begin(), sub.end());
			} else {
				int i, j;
				do {
					i = rng.nextInt(numG);
					j = rng.nextInt(numG);
				} while (i == j);
				swap(cycle[i], cycle[j]);
			}
			double newScore = evaluateGroupCycle(cycle, l);
			double tolerance = 0;
			if (newScore > score + tolerance) {
				cycle = oldCycle;
				continue;
			}
			score = newScore;
			if (score < bestScore) {
				bestScore = score;
				bestCycle = cycle;
				if (iter > 10000)
					trace("best updated! ", (int) round(score), " T=", temp, " (l=", l, ")");
			}
		}
		cycle = bestCycle;

		{
			vector<Pos> ps;
			int gl = len(cycle);
			Pos pos = groupDistEdges[cycle.back()][cycle.front()][0];
			ps.push_back(pos);
			rep0(i, gl) {
				int g = cycle[i];
				int ng = cycle[(i + 1) % gl];
				Pos exit = groupDistEdges[g][ng][0];
				vector<Pos> subPath;
				subPath.push_back(pos);
				for (Pos pos2 : paths[g]) {
					if (pos2 != exit)
						subPath.push_back(pos2);
				}
				subPath.push_back(exit);
				optimizePath(subPath, false);
				auto fullPath = restorePath(subPath);
				ps.insert(ps.end(), fullPath.begin() + 1, fullPath.end());
				pos = exit;
			}
			ps.pop_back();
			rotate(ps.begin(), find(ps.begin(), ps.end(), Pos(0, 0)), ps.end());
			ps.push_back(Pos(0, 0));
			trace(len(ps));
			ans = ps;
		}
	}

	double evaluateGroupCycle(const vector<int> cycle, int& l) {
		int gl = len(cycle);
		int time = 0;
		static int visits[1000][1000] = {};
		static int numVisits[1000] = {};
		rep0(g, numG) {
			numVisits[g] = 0;
		}
		rep0(i, gl) {
			int g0p = cycle[(i - 1 + gl) % gl];
			int g0 = cycle[i];
			int g1 = cycle[(i + 1) % gl];
			int g1n = cycle[(i + 2) % gl];
			int moveTime = groupDistMap[g0][g1];

			int cycleTime0 = groupCycleLengths[g0];
			int cycleTime1 = groupCycleLengths[g1];
			visits[g0][numVisits[g0]++] = time;
			time += cycleTime0 / 2 + moveTime + cycleTime1 / 2;
		}
		l = time;
		ll sum = 0;
		rep0(g, numG) {
			int n = numVisits[g];
			// trace("group ", g, ": visits=", n);
			rep0(i, n) {
				int t0 = visits[g][i];
				int t1 = visits[g][(i + 1) % n];
				if (t1 <= t0)
					t1 += l;
				int it = t1 - t0;
				// trace("  ", it, " (", t0, ", ", t1, ")");
				sum += Visit::weight(it) * groupDs[g];
			}
		}
		// trace("sum=", sum, " l=", l);
		double res = sum / (double) l;
		return res;
	}

	void computeDists() {
		int st = timer();
		rep2() {
			Pos from(i, j);
			Map<bool> visited;
			deque<Pos> q;
			q.emplace_back(from);
			visited.at(from) = true;
			int dist = 0;
			while (!q.empty()) {
				int num = len(q);
				rep0(t, num) {
					Pos pos = q.front();
					q.pop_front();
					repd() {
						if (!p.wall(pos, dir)) {
							Pos pos2 = pos.move(dir);
							if (!visited.at(pos2)) {
								visited.at(pos2) = true;
								distBetween(from, pos2) = dist + 1;
								q.push_back(pos2);
							}
						}
					}
				}
				dist++;
			}
		}
		trace("compute pos: ", timer() - st, " ms");
	}

	void updateScore(State& state) {
		int l = len(state.cycle);
		state.sum = 0;
		rep2() {
			Pos pos(i, j);
			auto& vis = state.visits.at(pos);
			state.sum += vis.computeSum(l) * p.d(pos);
		}
	}

	void solveMain() {
		// createInitialCycle();
		sa();
	}

	void sa() {
		{ // build state
			assert(ans.front() == Pos(0, 0));
			assert(ans.back() == Pos(0, 0));
			ans.pop_back();

			int time = 0;
			st.clear(n);
			for (Pos pos : ans) {
				st.visits.at(pos).insert(time);
				st.cycle.push_back(pos);
				time++;
			}
			updateScore(st);
			// trace("expected score: ", (int) round(st.sum / (double) len(st.path)));

			ans.emplace_back(0, 0);
		}
		const auto& ds = p.ds;
		static bool important[50000] = {};
		static ll shrinkBenefit[50000] = {};
		static ll extendCost[50000] = {};
		double temp = 0;

		auto updateImportance = [&]() {
			int l = len(st.cycle);
			ll benefit = 0;
			ll cost = 0;
			rep2() {
				Pos pos(i, j);
				int it = st.visits.at(pos).upperBound(-1, l) - st.visits.at(pos).lowerBound(-1, l);
				benefit += (it - 1) * ds.at(pos);
				cost += it * ds.at(pos);
			}
			rep(t, 0, l) {
				Pos pos = st.cycle[t];
				bool imp = st.visits.at(pos).count() == 1;
				auto& visit = st.visits.at(pos);
				if (!imp) {
					int t0 = visit.lowerBound(t - 1, l);
					int t1 = visit.upperBound(t, l);
					int it0 = t - t0;
					int it1 = t1 - t;
					int d = ds.at(pos);
					benefit -= (it0 - 1) * d;
					cost -= it0 * d;
					shrinkBenefit[t] = benefit;
					extendCost[t] = cost;

					ll removeCost =
					    (Visit::weight(it0 + it1 - 1) - (Visit::weight(it0) + Visit::weight(it1))) * d;
					ll deltaSum = removeCost - benefit;
					double expectedDelta = (st.sum + deltaSum) / (double) (l - 1) - st.sum / (double) l;
					// assert(abs(expectedDelta - computeDelta(st, t, 1, {})) < 1e-6);
					if (expectedDelta > 100 * -log(rng.nextFloat()))
						imp = true;

					benefit += (it1 - 1) * d;
					cost += it1 * d;
				} else {
					shrinkBenefit[t] = benefit;
					extendCost[t] = cost;
				}
				important[t] = imp;
			}
		};

		bool flip = true;
		bool shrink = true;
		bool balance = true;
		updateScore(st);
		updateImportance();
		auto& cycle = st.cycle;
		bool doubled = false;
		auto bestCycle = cycle;
		double bestScore = st.sum / (double) len(cycle);
		double progress = 0;
		rep0(iter, 100000) {
			if ((iter & 15) == 0) {
				progress = timer() / 1950.0;
				temp = pow(500, 1 - progress) * pow(0.5, progress);
				if (progress >= 1)
					break;
				if (progress > 0.5 && !doubled) {
					trace("doubled!!");
					auto newCycle = cycle;
					newCycle.insert(newCycle.begin(), cycle.begin(), cycle.end());
					applyDelta(st, 0, len(cycle), newCycle);
					updateScore(st);
					updateImportance();
					doubled = true;
				}
			}
			int possibleTs[1000];
			int numPossibleTs = 0;
			// type-1
			if (balance && (iter & 31) == 0) {
				int l = len(cycle);
				ll minDiff = 0;
				vector<Pos> minCycle;

				rep0(t0, l - 3) {
					Pos p0 = cycle[t0];
					Pos p1 = cycle[t0 + 1];
					Pos p2 = cycle[t0 + 2];
					Pos p3 = cycle[t0 + 3];
					if (distBetween(p0, p2) != 2)
						continue;
					if (distBetween(p0, p3) != 1)
						continue;
					if (p1 == p3)
						continue;
					int d01 = toDir(p0, p1);
					if (p.wall(p1, d01) || p.wall(p2, d01))
						continue;
					Pos pa = p1.move(d01);
					Pos pb = p2.move(d01);
					int d12 = toDir(p1, p2);
					int d1 = ds.at(p1);
					int d2 = ds.at(p2);
					ll oldSum = st.visits.at(p1).computeSum(l) * d1 + st.visits.at(p2).computeSum(l) * d2;
					st.visits.at(p1).erase(t0 + 1);
					st.visits.at(p2).erase(t0 + 2);
					numPossibleTs = 0;
					for (int t : st.visits.at(pa).times)
						possibleTs[numPossibleTs++] = t;
					for (int t : st.visits.at(pb).times)
						possibleTs[numPossibleTs++] = t;
					rep0(index, numPossibleTs) {
						int t1 = possibleTs[index];
						if (t1 + 3 >= l)
							continue;
						if (t1 >= t0 - 5 && t1 < t0 + 5)
							continue; // too close
						if (shrinkBenefit[t0] > extendCost[t1]) {
							Pos p4 = cycle[t1];
							Pos p5 = cycle[t1 + 1];
							Pos p6 = cycle[t1 + 2];
							Pos p7 = cycle[t1 + 3];
							if (pa == p4 && pb == p5 && p6 != p2 && p7 != p1) {
								// 0   3
								// |   |
								// 1---2
								// 4---5
								st.visits.at(p1).insert(t1 + 1);
								st.visits.at(p2).insert(t1 + 2);
								ll newSum =
								    st.visits.at(p1).computeSum(l) * d1 + st.visits.at(p2).computeSum(l) * d2;
								ll expectedSumDiff =
								    (extendCost[t1] - shrinkBenefit[t0]) * 2 + (newSum - oldSum);
								if (expectedSumDiff < minDiff) {
									minDiff = expectedSumDiff;
									minCycle = cycle;
									if (t1 > t0) {
										minCycle.insert(minCycle.begin() + t1 + 1, {p1, p2});
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 3);
									} else {
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 3);
										minCycle.insert(minCycle.begin() + t1 + 1, {p1, p2});
									}
								}
								st.visits.at(p1).erase(t1 + 1);
								st.visits.at(p2).erase(t1 + 2);
							} else if (pa == p5 && pb == p4 && p6 != p1 && p7 != p2) {
								// 0   3
								// |   |
								// 1---2
								// 5---4
								st.visits.at(p2).insert(t1 + 1);
								st.visits.at(p1).insert(t1 + 2);
								ll newSum =
								    st.visits.at(p1).computeSum(l) * d1 + st.visits.at(p2).computeSum(l) * d2;
								ll expectedSumDiff =
								    (extendCost[t1] - shrinkBenefit[t0]) * 2 + (newSum - oldSum);
								if (expectedSumDiff < minDiff) {
									minDiff = expectedSumDiff;
									minCycle = cycle;
									if (t1 > t0) {
										minCycle.insert(minCycle.begin() + t1 + 1, {p2, p1});
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 3);
									} else {
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 3);
										minCycle.insert(minCycle.begin() + t1 + 1, {p2, p1});
									}
								}
								st.visits.at(p2).erase(t1 + 1);
								st.visits.at(p1).erase(t1 + 2);
							}
						}
					}
					st.visits.at(p1).insert(t0 + 1);
					st.visits.at(p2).insert(t0 + 2);
				}
				if (minDiff < 0) {
					applyDelta(st, 0, l, minCycle);
					updateScore(st);
					updateImportance();
					double score = st.sum / (double) len(cycle);
					if (score < bestScore) {
						bestScore = score;
						bestCycle = cycle;
						trace("best updated! ", (int) round(bestScore), " T=", temp);
					}
				}
			}
			// type-2
			if (balance && (iter & 31) == 0) {
				int l = len(cycle);
				ll minDiff = 0;
				vector<Pos> minCycle;

				rep0(t0, l - 5) {
					Pos p0 = cycle[t0];
					Pos p1 = cycle[t0 + 1];
					Pos p2 = cycle[t0 + 2];
					Pos p3 = cycle[t0 + 3];
					Pos p4 = cycle[t0 + 4];
					Pos p5 = cycle[t0 + 5];
					if (distBetween(p0, p2) != 2)
						continue;
					if (distBetween(p0, p3) != 3)
						continue;
					if (distBetween(p0, p4) != 2)
						continue;
					if (distBetween(p0, p5) != 1)
						continue;
					if (distBetween(p1, p4) != 1)
						continue;
					if (p1 == p3 || p1 == p5 || p2 == p4)
						continue;
					assert(toDir(p0, p1) == toDir(p1, p2));
					assert(toDir(p3, p4) == toDir(p4, p5));
					assert(toDir(p0, p1) == flipDir(toDir(p3, p4)));
					int d23 = toDir(p2, p3);
					int d32 = flipDir(d23);
					// these can be invalid
					Pos pa = p1.move(d32);
					Pos pb = p2.move(d32);
					Pos pc = p4.move(d23);
					Pos pd = p3.move(d23);
					int d1 = ds.at(p1);
					int d2 = ds.at(p2);
					int d3 = ds.at(p3);
					int d4 = ds.at(p4);
					ll oldSum = st.visits.at(p1).computeSum(l) * d1 + st.visits.at(p2).computeSum(l) * d2 +
					    st.visits.at(p3).computeSum(l) * d3 + st.visits.at(p4).computeSum(l) * d4;
					st.visits.at(p1).erase(t0 + 1);
					st.visits.at(p2).erase(t0 + 2);
					st.visits.at(p3).erase(t0 + 3);
					st.visits.at(p4).erase(t0 + 4);
					numPossibleTs = 0;
					if (!p.wall(p1, d32))
						for (int t : st.visits.at(pa).times)
							possibleTs[numPossibleTs++] = t;
					if (!p.wall(p2, d32))
						for (int t : st.visits.at(pb).times)
							possibleTs[numPossibleTs++] = t;
					if (!p.wall(p4, d23))
						for (int t : st.visits.at(pc).times)
							possibleTs[numPossibleTs++] = t;
					if (!p.wall(p3, d23))
						for (int t : st.visits.at(pd).times)
							possibleTs[numPossibleTs++] = t;
					rep0(index, numPossibleTs) {
						int t1 = possibleTs[index];
						if (t1 + 4 >= l)
							continue;
						if (t1 >= t0 - 5 && t1 < t0 + 5)
							continue; // too close
						if (shrinkBenefit[t0] > extendCost[t1]) {
							Pos p6 = cycle[t1];
							Pos p7 = cycle[t1 + 1];
							Pos p8 = cycle[t1 + 2];
							Pos p9 = cycle[t1 + 3];
							Pos p10 = cycle[t1 + 4];
							if (p6 == pa && p7 == pb && !p.wall(p6, d23) && !p.wall(p7, d23) && p9 != p3 &&
							    p10 != p2) {
								//   0   5
								//   |   |
								// 6 1   4
								// | |   |
								// 7 2---3
								st.visits.at(p1).insert(t1 + 1);
								st.visits.at(p4).insert(t1 + 2);
								st.visits.at(p3).insert(t1 + 3);
								st.visits.at(p2).insert(t1 + 4);
								ll newSum = st.visits.at(p1).computeSum(l) * d1 +
								    st.visits.at(p2).computeSum(l) * d2 +
								    st.visits.at(p3).computeSum(l) * d3 + st.visits.at(p4).computeSum(l) * d4;
								ll expectedSumDiff =
								    (extendCost[t1] - shrinkBenefit[t0]) * 4 + (newSum - oldSum);
								if (expectedSumDiff < minDiff) {
									// trace("!!!! ", t0, " ", t1, " ", expectedSumDiff / l, " ",
									//     vector<Pos>{p6, p7});
									minDiff = expectedSumDiff;
									minCycle = cycle;
									if (t1 > t0) {
										minCycle.insert(minCycle.begin() + t1 + 1, {p1, p4, p3, p2});
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 5);
									} else {
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 5);
										minCycle.insert(minCycle.begin() + t1 + 1, {p1, p4, p3, p2});
									}
								}
								st.visits.at(p1).erase(t1 + 1);
								st.visits.at(p4).erase(t1 + 2);
								st.visits.at(p3).erase(t1 + 3);
								st.visits.at(p2).erase(t1 + 4);
							} else if (p6 == pb && p7 == pa && !p.wall(p6, d23) && !p.wall(p7, d23) &&
							    p9 != p4 && p10 != p1) {
								//   0   5
								//   |   |
								// 7 1   4
								// | |   |
								// 6 2---3
								st.visits.at(p2).insert(t1 + 1);
								st.visits.at(p3).insert(t1 + 2);
								st.visits.at(p4).insert(t1 + 3);
								st.visits.at(p1).insert(t1 + 4);
								ll newSum = st.visits.at(p1).computeSum(l) * d1 +
								    st.visits.at(p2).computeSum(l) * d2 +
								    st.visits.at(p3).computeSum(l) * d3 + st.visits.at(p4).computeSum(l) * d4;
								ll expectedSumDiff =
								    (extendCost[t1] - shrinkBenefit[t0]) * 4 + (newSum - oldSum);
								if (expectedSumDiff < minDiff) {
									// trace("!!!! ", t0, " ", t1, " ", expectedSumDiff / l, " ",
									//     vector<Pos>{p6, p7});
									minDiff = expectedSumDiff;
									minCycle = cycle;
									if (t1 > t0) {
										minCycle.insert(minCycle.begin() + t1 + 1, {p2, p3, p4, p1});
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 5);
									} else {
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 5);
										minCycle.insert(minCycle.begin() + t1 + 1, {p2, p3, p4, p1});
									}
								}
								st.visits.at(p2).erase(t1 + 1);
								st.visits.at(p3).erase(t1 + 2);
								st.visits.at(p4).erase(t1 + 3);
								st.visits.at(p1).erase(t1 + 4);
							} else if (p6 == pc && p7 == pd && !p.wall(p6, d32) && !p.wall(p7, d32) &&
							    p9 != p2 && p10 != p3) {
								//  0   5
								//  |   |
								//  1   4 6
								//  |   | |
								//  2---3 7
								st.visits.at(p4).insert(t1 + 1);
								st.visits.at(p1).insert(t1 + 2);
								st.visits.at(p2).insert(t1 + 3);
								st.visits.at(p3).insert(t1 + 4);
								ll newSum = st.visits.at(p1).computeSum(l) * d1 +
								    st.visits.at(p2).computeSum(l) * d2 +
								    st.visits.at(p3).computeSum(l) * d3 + st.visits.at(p4).computeSum(l) * d4;
								ll expectedSumDiff =
								    (extendCost[t1] - shrinkBenefit[t0]) * 4 + (newSum - oldSum);
								if (expectedSumDiff < minDiff) {
									// trace("!!!! ", t0, " ", t1, " ", expectedSumDiff / l, " ",
									//     vector<Pos>{p6, p7});
									minDiff = expectedSumDiff;
									minCycle = cycle;
									if (t1 > t0) {
										minCycle.insert(minCycle.begin() + t1 + 1, {p4, p1, p2, p3});
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 5);
									} else {
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 5);
										minCycle.insert(minCycle.begin() + t1 + 1, {p4, p1, p2, p3});
									}
								}
								st.visits.at(p4).erase(t1 + 1);
								st.visits.at(p1).erase(t1 + 2);
								st.visits.at(p2).erase(t1 + 3);
								st.visits.at(p3).erase(t1 + 4);
							} else if (p6 == pd && p7 == pc && !p.wall(p6, d32) && !p.wall(p7, d32) &&
							    p9 != p1 && p10 != p4) {
								//  0   5
								//  |   |
								//  1   4 7
								//  |   | |
								//  2---3 6
								st.visits.at(p3).insert(t1 + 1);
								st.visits.at(p2).insert(t1 + 2);
								st.visits.at(p1).insert(t1 + 3);
								st.visits.at(p4).insert(t1 + 4);
								ll newSum = st.visits.at(p1).computeSum(l) * d1 +
								    st.visits.at(p2).computeSum(l) * d2 +
								    st.visits.at(p3).computeSum(l) * d3 + st.visits.at(p4).computeSum(l) * d4;
								ll expectedSumDiff =
								    (extendCost[t1] - shrinkBenefit[t0]) * 4 + (newSum - oldSum);
								if (expectedSumDiff < minDiff) {
									// trace("!!!! ", t0, " ", t1, " ", expectedSumDiff / l, " ",
									//     vector<Pos>{p6, p7});
									minDiff = expectedSumDiff;
									minCycle = cycle;
									if (t1 > t0) {
										minCycle.insert(minCycle.begin() + t1 + 1, {p3, p2, p1, p4});
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 5);
									} else {
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 5);
										minCycle.insert(minCycle.begin() + t1 + 1, {p3, p2, p1, p4});
									}
								}
								st.visits.at(p3).erase(t1 + 1);
								st.visits.at(p2).erase(t1 + 2);
								st.visits.at(p1).erase(t1 + 3);
								st.visits.at(p4).erase(t1 + 4);
							}
						}
					}
					st.visits.at(p1).insert(t0 + 1);
					st.visits.at(p2).insert(t0 + 2);
					st.visits.at(p3).insert(t0 + 3);
					st.visits.at(p4).insert(t0 + 4);
				}
				if (minDiff < 0) {
					applyDelta(st, 0, l, minCycle);
					updateScore(st);
					updateImportance();
					double score = st.sum / (double) len(cycle);
					if (score < bestScore) {
						bestScore = score;
						bestCycle = cycle;
						trace("best updated! ", (int) round(bestScore), " T=", temp);
					}
				}
			}
			// type-3
			if (balance && (iter & 31) == 0) {
				int l = len(cycle);
				ll minDiff = 0;
				vector<Pos> minCycle;

				rep0(t0, l - 2) {
					Pos p0 = cycle[t0];
					Pos p1 = cycle[t0 + 1];
					Pos p2 = cycle[t0 + 2];
					if (p2 != p0)
						continue;
					int d01 = toDir(p0, p1);
					int d1 = ds.at(p1);
					ll oldSum = st.visits.at(p1).computeSum(l) * d1;
					st.visits.at(p1).erase(t0 + 1);
					numPossibleTs = 0;
					repd() {
						if (dir != flipDir(d01) && !p.wall(p1, dir))
							for (int t : st.visits.at(p1.move(dir)).times)
								possibleTs[numPossibleTs++] = t;
					}
					rep0(index, numPossibleTs) {
						int t1 = possibleTs[index];
						if (t1 >= t0 - 5 && t1 < t0 + 5)
							continue; // too close
						if (t1 + 1 >= l)
							continue;
						if (shrinkBenefit[t0] > extendCost[t1]) {
							Pos p3 = cycle[t1];
							Pos p4 = cycle[t1 + 1];
							if (distBetween(p3, p1) == 1 && p4 != p1) {
								//  0=2
								//   |
								// 3 1 3
								//   3
								st.visits.at(p1).insert(t1 + 1);
								ll newSum = st.visits.at(p1).computeSum(l) * d1;
								ll expectedSumDiff =
								    (extendCost[t1] - shrinkBenefit[t0]) * 2 + (newSum - oldSum);
								if (expectedSumDiff < minDiff) {
									minDiff = expectedSumDiff;
									minCycle = cycle;
									if (t1 > t0) {
										minCycle.insert(minCycle.begin() + t1 + 1, {p1, p3});
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 3);
									} else {
										minCycle.erase(minCycle.begin() + t0 + 1, minCycle.begin() + t0 + 3);
										minCycle.insert(minCycle.begin() + t1 + 1, {p1, p3});
									}
								}
								st.visits.at(p1).erase(t1 + 1);
							}
						}
					}
					st.visits.at(p1).insert(t0 + 1);
				}
				if (minDiff < 0) {
					applyDelta(st, 0, l, minCycle);
					updateScore(st);
					updateImportance();
					double score = st.sum / (double) len(cycle);
					if (score < bestScore) {
						bestScore = score;
						bestCycle = cycle;
						trace("best updated! ", (int) round(bestScore), " T=", temp);
					}
				}
			}
			if (flip) {
				while (true) {
					int l = len(cycle);
					int t0 = rng.nextInt(l);
					Pos pos = cycle[t0];
					int t1 = st.visits.at(pos).upperBound(t0, l);
					while (t1 < l && rng.nextFloat() < 0.1)
						t1 = st.visits.at(pos).upperBound(t1, l);
					int length = t1 - t0 + 1;
					if (t1 >= l || length <= 10)
						continue;
					vector<Pos> subPath;
					subPath.insert(subPath.end(), cycle.begin() + t0, cycle.begin() + t1 + 1);
					reverse(subPath.begin(), subPath.end());
					double diff = computeDelta(st, t0, length, subPath);
					if (diff < 0) {
						trace("flip ", diff, " ", t0, " ", t1, " ", length);
						applyDelta(st, t0, length, subPath);
						updateImportance();
						updateScore(st);
						double score = st.sum / (double) len(cycle);
						if (score < bestScore) {
							bestScore = score;
							bestCycle = cycle;
							trace("best updated! ", (int) round(bestScore), " T=", temp);
						}
					}
					break;
				}
			}
			int l = len(cycle);
			Map<int> visCount;
			int length = min(min(l - 100, 200), (int) (4 + n * 2 * -log(rng.nextFloat())));
			int minT = rng.nextInt(l - length);

			// generate subpath
			vector<Pos> subPath;
			{
				Map<bool> visited;
				Map<bool> skipped;
				Pos first = cycle[minT];
				Pos last = cycle[minT + length - 1];
				visited.at(first) = true;
				visited.at(last) = true;
				subPath.push_back(first);
				rep(dt, 1, length - 1) {
					int t = minT + dt;
					Pos pos = cycle[t];
					if (!visited.at(pos)) {

						bool imp = skipped.at(pos) || important[t];

						if (!shrink)
							imp = true;

						if (imp) {
							visited.at(cycle[t]) = true;
							subPath.push_back(pos);
						} else {
							skipped.at(pos) = true;
						}
					}
				}
				subPath.push_back(last);
			}

			// try to optimize subpath
			int distBefore = pathDist(subPath);
			if (optimizePath(subPath, false)) {
				vector<Pos> path = restorePath(subPath);
				double diff = computeDelta(st, minT, length, path);
				double tolerance = temp * -log(rng.nextFloat());
				if (diff > tolerance)
					continue;
				applyDelta(st, minT, length, path);
				updateScore(st);
				updateImportance();
				double score = st.sum / (double) len(cycle);
				if (score < bestScore) {
					bestScore = score;
					bestCycle = cycle;
					trace("best updated! ", (int) round(bestScore), " T=", temp);
				}
			}
		}

		applyDelta(st, 0, len(cycle), bestCycle);
		st.normalize();
		updateScore(st);
		res.score = (int) round(st.sum / (double) len(cycle));
		ans = cycle;
		ans.push_back(Pos(0, 0));
	}

	int pathDist(const vector<Pos>& path) {
		int l = len(path);
		int res = 0;
		rep0(t, l - 1) res += distBetween(path[t], path[t + 1]);
		return res;
	}

	int cycleDist(const vector<Pos>& cycle) {
		return pathDist(cycle) + distBetween(cycle.front(), cycle.back());
	}

	bool optimizePath(vector<Pos>& path, bool cycle) {

		bool changed = false;

		int l = len(path);
		int dist = pathDist(path);
		int e = l - 1;

		int loopCount = 0;
		while (true) {
			bool shrunk = false;
			if (cycle) {
				rep(t2, 1, l) {
					int t2n = t2 == l - 1 ? 0 : t2 + 1;
					rep0(t1, t2) {
						int d1 = distBetween(path[t1], path[t1 + 1]);
						int d2 = distBetween(path[t2], path[t2n]);
						int d3 = distBetween(path[t1], path[t2]);
						int d4 = distBetween(path[t1 + 1], path[t2n]);
						int diff = (d3 + d4) - (d1 + d2);
						if (diff <= 0) {
							// [t1+1, t2]
							reverse(path.begin() + t1 + 1, path.begin() + t2 + 1);
							shrunk |= diff < 0;
							changed = true;
						}
					}
				}
			} else {
				rep(t2, 2, e) {
					rep0(t1, t2 - 1) {
						int d1 = distBetween(path[t1], path[t1 + 1]);
						int d2 = distBetween(path[t2], path[t2 + 1]);
						int d3 = distBetween(path[t1], path[t2]);
						int d4 = distBetween(path[t1 + 1], path[t2 + 1]);
						int diff = (d3 + d4) - (d1 + d2);
						if (diff <= 0) {
							// [t1+1, t2]
							reverse(path.begin() + t1 + 1, path.begin() + t2 + 1);
							shrunk |= diff < 0;
							changed = true;
						}
					}
				}
			}
			if (cycle) {
				rep0(t1, l) {
					int t1n = t1 == l - 1 ? 0 : t1 + 1;
					rep(t2, 0, l) {
						if (t2 == t1 || t2 == t1n)
							continue;
						int t2p = t2 == 0 ? l - 1 : t2 - 1;
						int t2n = t2 == l - 1 ? 0 : t2 + 1;
						int d1 = distBetween(path[t1], path[t1n]);
						int d2 = distBetween(path[t2p], path[t2n]);
						int d2a = distBetween(path[t2p], path[t2]);
						int d2b = distBetween(path[t2], path[t2n]);
						int d1a = distBetween(path[t1], path[t2]);
						int d1b = distBetween(path[t2], path[t1n]);
						int diff = (d1a + d1b + d2) - (d2a + d2b + d1);
						if (diff <= 0) {
							if (t1 < t2) {
								// [t1+1, t2]
								reverse(path.begin() + t1 + 1, path.begin() + t2 + 1);
								// [t1+2, t2]
								reverse(path.begin() + t1 + 2, path.begin() + t2 + 1);
							} else {
								// [t2, t1]
								reverse(path.begin() + t2, path.begin() + t1 + 1);
								// [t2, t1-1]
								reverse(path.begin() + t2, path.begin() + t1);
							}
							shrunk |= diff < 0;
							changed = true;
						}
					}
				}
			} else {
				rep0(t1, e) {
					rep(t2, 1, e) {
						if (t2 == t1 || t2 == t1 + 1)
							continue;
						int d1 = distBetween(path[t1], path[t1 + 1]);
						int d2 = distBetween(path[t2 - 1], path[t2 + 1]);
						int d2a = distBetween(path[t2 - 1], path[t2]);
						int d2b = distBetween(path[t2], path[t2 + 1]);
						int d1a = distBetween(path[t1], path[t2]);
						int d1b = distBetween(path[t2], path[t1 + 1]);
						int diff = (d1a + d1b + d2) - (d2a + d2b + d1);
						if (diff <= 0) {
							if (t1 < t2) {
								// [t1+1, t2]
								reverse(path.begin() + t1 + 1, path.begin() + t2 + 1);
								// [t1+2, t2]
								reverse(path.begin() + t1 + 2, path.begin() + t2 + 1);
							} else {
								// [t2, t1]
								reverse(path.begin() + t2, path.begin() + t1 + 1);
								// [t2, t1-1]
								reverse(path.begin() + t2, path.begin() + t1);
							}
							shrunk |= diff < 0;
							changed = true;
						}
					}
				}
			}
			loopCount++;
			if (!shrunk) {
				if (loopCount <= 2 && e > 4) { // double bridge
					int minCost = INF;
					array<int, 4> bestTs;
					rep0(iter, 4) {
						array<int, 4> ts;
						do {
							ts[0] = rng.nextInt(e);
							ts[1] = rng.nextInt(e);
							ts[2] = rng.nextInt(e);
							ts[3] = rng.nextInt(e);
							sort(ts.begin(), ts.end());
						} while (ts[0] == ts[1] || ts[1] == ts[2] || ts[2] == ts[3]);
						int pdist = distBetween(path[ts[0]], path[ts[0] + 1]) +
						    distBetween(path[ts[1]], path[ts[1] + 1]) +
						    distBetween(path[ts[2]], path[ts[2] + 1]) +
						    distBetween(path[ts[3]], path[ts[3] + 1]);
						int ndist = distBetween(path[ts[0]], path[ts[2] + 1]) +
						    distBetween(path[ts[1]], path[ts[3] + 1]) +
						    distBetween(path[ts[2]], path[ts[0] + 1]) +
						    distBetween(path[ts[3]], path[ts[1] + 1]);
						int cost = ndist - pdist;
						if (cost < minCost) {
							minCost = cost;
							bestTs = ts;
						}
					}
					reverse(path.begin() + bestTs[0] + 1, path.begin() + bestTs[1] + 1);
					reverse(path.begin() + bestTs[1] + 1, path.begin() + bestTs[2] + 1);
					reverse(path.begin() + bestTs[2] + 1, path.begin() + bestTs[3] + 1);
					reverse(path.begin() + bestTs[0] + 1, path.begin() + bestTs[3] + 1);
				} else {
					break;
				}
			}
		}
		int dist2 = pathDist(path);
		// trace("dist: ", dist2, " (opt=", l - 1, ")");
		// return dist2 < dist;

		// if (!cycle && path.front() != path.back())
		// 	return changed | optimizePathLk(path);

		return changed;
	}

	vector<Pos> restorePath(const vector<Pos>& subPath) {
		vector<Pos> res;
		Pos pos = subPath[0];
		res.push_back(pos);
		int l = len(subPath);
		rep0(t, l - 1) {
			Pos target = subPath[t + 1];
			int dist = distBetween(pos, target);
			while (dist > 0) {
				repd() {
					if (!p.wall(pos, dir)) {
						Pos pos2 = pos.move(dir);
						if (distBetween(pos2, target) == dist - 1) {
							pos = pos2;
							break;
						}
					}
				}
				dist--;
				res.push_back(pos);
			}
		}
		return res;
	}

	void setupLastVisit(Map<int>& lastVisit, const vector<Pos>& cycle) {
		rep0(i, len(cycle)) {
			lastVisit.at(cycle[i]) = i;
		}
		int l = len(cycle) - 1;
		rep2() {
			lastVisit.at(i, j) -= l;
		}
	}

	vector<Pos> createCycle(int targetTurns, Map<int> lastVisit, bool fast) {
		double invTargetTurns = 1.0 / targetTurns;

		vector<Pos> res;
		Pos currentPos(0, 0);
		Map<bool> visited;
		int time = 0;

		res.emplace_back(currentPos);
		visited.at(currentPos) = true;
		assert(lastVisit.at(currentPos) == 0);

		auto& ds = p.ds;

		ll sum = 0;
		ll dsum = 0;

		auto aAt = [&](Pos pos) {
			int t0 = lastVisit.at(pos);
			return ds.at(pos) * (time - t0);
		};

		rep2() {
			Pos pos(i, j);
			assert(lastVisit.at(pos) <= 0);
			sum += aAt(pos);
			dsum += ds.at(pos);
		}

		auto follow = [&](const vector<Pos>& path) {
			for (Pos pos : path) {
				time++;
				assert(currentPos.distTo(pos) == 1);
				sum += dsum;
				sum -= aAt(pos);
				lastVisit.at(pos) = time;
				visited.at(pos) = true;
				res.push_back(pos);
				currentPos = pos;
			}
		};

		while (true) {
			double progress = len(res) * invTargetTurns;
			if (progress >= 1) { // wrap it up
				dijkstra(currentPos, Pos(0, 0), [&](Pos pos, int dir) {
					// return 1.0;
					return 500.0 / (500.0 + aAt(pos));
				});
				dres.restorePath(Pos(0, 0));
				follow(dres.path);
				break;
			}
			Pos target(-1);
			double maxValue = 0;
			// pick the target pos
			rep2() {
				Pos pos(i, j);
				int dist = distBetween(currentPos, pos);
				int d = p.d(pos);
				double dlevel = d * 1e-3;
				double weight = aAt(pos) * mix(40.0, 1.0, dlevel);
				weight += p.d(pos) * dist;
				if (!visited.at(pos))
					weight += progress * 1e6;
				double value = weight / (1.0 + dist * 0.2);
				if (value > maxValue) {
					maxValue = value;
					target = pos;
				}
			}
			dijkstra(currentPos, target, [&](Pos pos, int dir) {
				// return 1.0;
				double unvisitedCoeff = mix(1.0, 0.01, (1 - pow(progress, 3)) * !visited.at(pos));
				double baseCost = 500.0 / (500.0 + aAt(pos));
				return baseCost * unvisitedCoeff;
			});
			dres.restorePath(target);
			int maxLength = fast ? INF : 2;
			if (len(dres.path) > maxLength) {
				dres.path.resize(maxLength);
			}
			follow(dres.path);
		}
		return res;
	}

	void fixCycle(vector<Pos>& cycle) {
		Map<bool> visited;
		set<Pos> unvisited;
		for (Pos pos : cycle) {
			visited.at(pos) = true;
		}
		rep2() {
			if (!visited.at(i, j))
				unvisited.emplace(i, j);
		}
		rep0(i, len(cycle)) {
			for (auto pos : unvisited) {
				bool done = false;
				repd() {
					if (!p.wall(cycle[i], dir) && cycle[i].move(dir) == pos) {
						cycle.insert(cycle.begin() + i + 1, {pos, cycle[i]});
						unvisited.erase(pos);
						done = true;
						break;
					}
				}
				if (done)
					break;
			}
		}
	}

	void createInitialCycle() {
		vector<Pos> bestAns;
		double minScore = 1e9;

		Map<int> lastVisit;
		int targetTurns = (int) mix(4.0, 5.0, (n - 20.0) / (40.0 - 20.0)) * n * n;

		auto cycle = createCycle(targetTurns, lastVisit, true);
		rep0(iter, 100000) {
			if (timer() > 500)
				break;
			setupLastVisit(lastVisit, cycle);
			cycle = createCycle(targetTurns, lastVisit, false);

			ans = cycle;

			fixCycle(ans);

			{ // build state
				assert(ans.front() == Pos(0, 0));
				assert(ans.back() == Pos(0, 0));
				ans.pop_back();

				int time = 0;
				st.clear(n);
				for (Pos pos : ans) {
					st.visits.at(pos).insert(time);
					st.cycle.push_back(pos);
					time++;
				}
				updateScore(st);
				// trace("expected score: ", (int) round(st.sum / (double) len(st.path)));

				ans.emplace_back(0, 0);
			}

			double score = st.sum / (double) len(st.cycle);
			if (score < minScore) {
				minScore = score;
				trace("min score updated: ", (int) round(minScore), " ", iter);
				bestAns = ans;
			}
		}
		ans = bestAns;
		res.score = (int) round(minScore);
	}

	double computeDelta(State& st, int from, int length, const vector<Pos>& path) {
		int lengthAfter = len(path);
		rep2() {
			st.visits.at(i, j).toInsert.clear();
		}
		rep0(t, lengthAfter) {
			Pos pos = path[t];
			st.visits.at(pos).toInsert.push_back(from + t);
		}
		ll totalDiff = 0;
		int l = len(st.cycle);
		rep2() {
			// ll sum = st.visits.at(i, j).computeSum(l);
			ll diff = st.visits.at(i, j).computeSumDiff(l, from, length, lengthAfter);
			totalDiff += p.d(i, j) * diff;
		}
		ll newSum = st.sum + totalDiff;
		int newL = l + (lengthAfter - length);
		return newSum / (double) newL - st.sum / (double) l;
	}

	void applyDelta(State& st, int from, int length, const vector<Pos>& newPath) {
		int lengthAfter = len(newPath);
		rep2() {
			st.visits.at(i, j).toInsert.clear();
		}
		rep0(t, lengthAfter) {
			Pos pos = newPath[t];
			st.visits.at(pos).toInsert.push_back(from + t);
		}
		auto& path = st.cycle;
		int l = len(path);
		rep2() {
			st.visits.at(i, j).applyEdit(l, from, length, lengthAfter);
		}
		path.erase(path.begin() + from, path.begin() + from + length);
		path.insert(path.begin() + from, newPath.begin(), newPath.end());
	}

	void dijkstra(Pos start, Pos goal, function<double(Pos, int)> cost) {
		dijkstra(
		    start,
		    [&](Pos pos) {
			    return pos == goal;
		    },
		    cost);
	}

	void dijkstra(Pos start, function<bool(Pos)> isGoal, function<double(Pos, int)> cost) {
		dres.clear(n);
		auto& vis = dres.vis;
		auto& prevs = dres.prevs;
		auto& dists = dres.dists;

		priority_queue<pdi, vector<pdi>, greater<pdi>> q;
		dists.at(start) = 0;
		q.emplace(0, start.index);

		while (!q.empty()) {
			auto top = q.top();
			q.pop();
			Pos pos(top.second);
			if (vis.at(pos))
				continue;
			vis.at(pos) = true;
			if (isGoal(pos)) {
				dres.goal = pos;
				break;
			}
			double dist = top.first;
			rep0(dir, 4) {
				if (!p.wall(pos, dir)) {
					Pos pos2 = pos.move(dir);
					double dist2 = dist + cost(pos2, dir);
					if (dist2 < dists.at(pos2)) {
						dists.at(pos2) = dist2;
						prevs.at(pos2) = pos;
						q.emplace(dist2, pos2.index);
					}
				}
			}
		}
	}

	void print(ostream& out) const {
		int m = len(ans);
		rep0(i, m - 1) {
			Pos pp = ans[i];
			Pos p = ans[i + 1];
			int di = p.i() - pp.i();
			int dj = p.j() - pp.j();
			if (pp.distTo(p) != 1) {
				trace("??????? ", pp, " -> ", p);
			}
			assert(pp.distTo(p) == 1);
			out << (di == -1 ? "U" : di == 1 ? "D" : dj == -1 ? "L" : "R");
		}
		out << endl;
	}
};

constexpr int SCORES[100] = {1405225, 5852842, 1609966, 4335918, 7339085, 12953800, 4122274, 3639048, 7070174,
    5339657, 11621252, 12606700, 5960189, 11391447, 8453214, 9967530, 12400559, 3589478, 1442097, 24555661,
    11628164, 5271688, 8496998, 3024952, 8631703, 1049672, 5921085, 7264804, 1919099, 13005269, 19141001,
    4079984, 2473060, 2205982, 1735275, 3271886, 1766218, 609595, 4122653, 868619, 7810677, 2804852, 25364670,
    5933579, 4819390, 4620261, 5425124, 11989714, 22042341, 4733578, 3975471, 23119868, 6544551, 7300999,
    12971030, 17064449, 4240195, 20836429, 13530897, 5536919, 22011818, 8573749, 19126993, 17334540, 11240994,
    2366815, 4309308, 16139228, 3927588, 1112138, 21796780, 31523308, 2163435, 3286036, 3393221, 13912158,
    6378775, 5797630, 2924375, 7642011, 5088141, 15941850, 16327429, 4852858, 5492668, 1477076, 1733603,
    5121901, 16551890, 7594231, 14463851, 7630012, 34807678, 19970070, 16311908, 21657057, 15711794, 23818618,
    4230526, 20373261};

int main() {
#if 0 || ONLINE_JUDGE
	isLocal = false;
	timer(true);
	Solver sol;
	sol.load(cin);
	sol.solve();
#elif 0
	compareScores("aaa", "bbb");
	// compareScores("new6", "new10");
#elif 0
	ostringstream out;
	debug = false;
	rep0(seed, 10000) {
		if (seed % 100 == 0)
			cerr << seed << endl;
		timer(true);
		Solver sol;
		sol.load(seed);
		if (!sol.shouldSortItems)
			out << seed << " " << -1 << endl;
		else if (sol.shouldGreedy)
			else out << seed << " " << 1 << endl;
		out << seed << " " << 100000000 << endl;
	}
	writeText("should_greedy.txt", out.str());
	compareScores("base", "should_greedy");
#elif 0
	// debug = false;
	int seed;
	cin >> seed;
	cin >> timeScale;
	int iter = 1;
	double sum = 0;
	rep0(t, iter) {
		timer(true);
		Solver sol;
		sol.load(seed);
		// if (sol.shouldSortItems) {
		// 	cout << -1 << endl;
		// 	return 0;
		// }
		// sol.rng.seed(12345 + t * 1000);
		sol.solve();
		sum += sol.res.score;
	}
	int score = (int) round(sum / iter);
	cout << (int) round(sum / iter) << " " << (seed < 100 ? (double) score / SCORES[seed] : 0) << endl;
#elif 1
	timeScale = 1.0;
	int num = 10;
	int from = 0;
	int stride = 1;
	int single = 0; // 648 yabai
	render = false;

	int seeds[1000] = {};
	int times[1000] = {};
	int scores[1000] = {};

	int scoreSum = 0;
	double scoreLogSum = 0;
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

		bool tmp = debug;
		debug = single != -1;
		Solver s;
		s.load(seed);
		s.solve();
		debug = tmp;

		if (single == -1)
			trace("score: ", s.res.score);
		if (s.res.score == -1) {
			numInvalid++;
		} else {
			scoreSum += s.res.score;
			scoreLogSum += log2(s.res.score);
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
		double numValid = num - numInvalid;
		trace("total score: ", scoreSum, ", mean: ", (int) (scoreSum / numValid * 100) / 100.0,
		    ", mean(log2): ", scoreLogSum / numValid);
		if (numInvalid > 0) {
			trace(numInvalid, " invalid cases");
		}
	} else {
		trace("total score: ", scoreSum, " time = ", timer());
	}
#endif
}
