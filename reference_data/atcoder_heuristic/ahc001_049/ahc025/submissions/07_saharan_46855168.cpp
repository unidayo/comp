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
constexpr int DIRS[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
constexpr int MAX_N = 100;
constexpr int MAX_D = 25;

// merge-insertion
constexpr int COMP_UPPER_BOUNDS[] = {0, 1, 3, 5, 7, 10, 13, 16, 19, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58,
    62, 66, 71, 76, 81, 86, 91, 96, 101, 106, 111, 116, 121, 126, 131, 136, 141, 146, 151, 156, 161, 166, 171,
    177, 183, 189, 195, 201, 207, 213, 219, 225, 231, 237, 243, 249, 255, 261, 267, 273, 279, 285, 291, 297,
    303, 309, 315, 321, 327, 333, 339, 345, 351, 357, 363, 369, 375, 381, 387, 393, 399, 405, 411, 417, 423,
    429, 436, 443, 450, 457, 464, 471, 478, 485, 492, 499, 506, 513, 520, 527, 534, 541, 548, 555, 562, 569,
    576, 583, 590, 597, 604, 611, 618, 625, 632, 639, 646, 653, 660, 667, 674, 681, 688, 695, 702, 709, 716,
    723, 730, 737, 744, 751, 758, 765, 772, 779, 786, 793, 800, 807, 814, 821, 828, 835, 842, 849, 856, 863,
    870, 877, 884, 891, 898, 905, 912, 919, 926, 933, 940, 947, 954, 961, 968, 975, 982, 989, 996, 1003, 1010,
    1017, 1024, 1032, 1040, 1048, 1056, 1064, 1072, 1080, 1088, 1096, 1104, 1112, 1120, 1128, 1136, 1144,
    1152, 1160, 1168, 1176, 1184, 1192, 1200, 1208, 1216, 1224, 1232, 1240, 1248, 1256, 1264};

// merge
constexpr int COMP_UPPER_BOUNDS2[] = {0, 1, 3, 5, 9, 11, 14, 17, 25, 27, 30, 33, 38, 41, 45, 49, 65, 67, 70,
    73, 78, 81, 85, 89, 98, 101, 105, 109, 115, 119, 124, 129, 161, 163, 166, 169, 174, 177, 181, 185, 194,
    197, 201, 205, 211, 215, 220, 225, 242, 245, 249, 253, 259, 263, 268, 273, 283, 287, 292, 297, 304};

#define repn(i) rep0(i, n)
#define repd(i) rep0(i, d)

using Set = bitset<MAX_N>;

void forin(const Set& s, function<void(int i)> f) {
	rep0(i, MAX_N) {
		if (s[i])
			f(i);
	}
}

int smallest(const Set& s) {
	rep0(i, MAX_N) {
		if (s[i])
			return i;
	}
	return -1;
}

int largest(const Set& s) {
	rep0r(i, MAX_N) {
		if (s[i])
			return i;
	}
	return -1;
}

template <typename Iter>
void insert(Set& s, Iter begin, Iter end) {
	while (begin != end) {
		s.set(*begin);
		begin++;
	}
}

bool isSubset(const Set& sub, const Set& sup) {
	return (sub & sup) == sub;
}

class Comparison {
public:
	Set ls;
	Set rs;
	int sign = 0;
	ull hash = 0;

	Comparison() {
	}

	void clear() {
		ls = 0;
		rs = 0;
		sign = 0;
	}

	void computeHash() {
		static constexpr ull MULT = 0x8b46ad15ae59daadull;
		static constexpr ull INCR = 0xf51827be20401689ull;
		ull y = 0x95fdd98a11be9c09ull;
		auto eat = [&y](int n) {
			y = y * MULT + INCR + n;
			y ^= y >> 18;
		};
		bool flip = smallest(ls) > smallest(rs);
		const auto& a = flip ? rs : ls;
		const auto& b = flip ? ls : rs;
		eat(len(a));
		forin(a, [&](int i) {
			eat(i);
		});
		eat(len(b));
		forin(b, [&](int i) {
			eat(i);
		});
		hash = y;
	}

	void normalize() {
		Set both = ls & rs;
		ls &= ~both;
		rs &= ~both;
	}
};

class MergeStreamBlock {
public:
	MergeStreamBlock* l = nullptr;
	MergeStreamBlock* r = nullptr;
	int keepL = -1;
	int keepR = -1;

	MergeStreamBlock() {
	}

	void clear() {
		l = nullptr;
		r = nullptr;
		keepL = -1;
		keepR = -1;
	}

	int next(function<int(int a, int b)> compare) {
		if (keepL == -1) {
			if (l) {
				keepL = l->next(compare);
			}
		}
		if (keepR == -1) {
			if (r) {
				keepR = r->next(compare);
			}
		}
		int res;
		if (keepL == -1) {
			res = keepR;
			keepR = -1;
			return res;
		}
		if (keepR == -1) {
			res = keepL;
			keepL = -1;
			return res;
		}
		if (compare(keepL, keepR) < 0) {
			res = keepL;
			keepL = -1;
			return res;
		} else {
			res = keepR;
			keepR = -1;
			return res;
		}
	}
};

class MergeStream {
public:
	int n = 0;
	int index = 0;
	MergeStreamBlock blocks[MAX_N * 10] = {};
	MergeStreamBlock* root = nullptr;

	MergeStream() {
	}

	void init(int n) {
		this->n = n;
		index = 0;
		root = make(0, n);
	}

private:
	MergeStreamBlock* make(int l, int r) {
		assert(r - l > 0);
		if (r - l == 1) {
			MergeStreamBlock* res = blocks + index++;
			res->clear();
			res->keepL = l;
			return res;
		}
		int mid = (l + r) / 2;
		MergeStreamBlock* left = make(l, mid);
		MergeStreamBlock* right = make(mid, r);
		MergeStreamBlock* res = blocks + index++;
		res->clear();
		res->l = left;
		res->r = right;
		return res;
	}
};

struct SolverResult {
	int score = 0;
	vector<pair<string, int>> groups;
};

class SetSet {
public:
	set<pair<ull, ull>> have;
	vector<Set> sets;

	SetSet() {
	}

	void insert(const Set& s) {
		constexpr Set lower = Set((ull) -1);
		ull lo = (s & lower).to_ullong();
		ull hi = (s >> 64).to_ullong();
		auto v = make_pair(lo, hi);
		if (have.count(v) > 0)
			return;
		have.insert(v);
		sets.push_back(s);
	}

	void prune(bool keepMax) {
		int m = len(sets);
		vector<bool> toErase(m);
		rep(i, 1, m) {
			rep0(j, i) {
				if (keepMax ? isSubset(sets[i], sets[j]) : isSubset(sets[j], sets[i])) {
					toErase[i] = true;
				}
			}
		}
		vector<Set> newSets;
		rep0(i, m) {
			if (!toErase[i]) {
				newSets.push_back(sets[i]);
			}
		}
		sets = newSets;
	}
};

constexpr int MAX_V = 10000;

class Vertex {
public:
	int time = 0;
	int index;
	Set set;
	vector<int> lts;
	bitset<MAX_V> bits;

	Vertex(int index, const Set& s) : index(index), set(s) {
	}
};

struct SetHash {
	ull operator()(const Set& s) const {
		static constexpr ull MULT = 0x8b46ad15ae59daadull;
		static constexpr ull INCR = 0xf51827be20401689ull;
		ull y = 0x95fdd98a11be9c09ull;
		auto eat = [&y](ull n) {
			y = y * MULT + INCR + n;
			y ^= y >> 18;
		};
		constexpr Set L64 = Set((ull) -1);
		ull lo = (s & L64).to_ullong();
		ull hi = (s >> 64).to_ullong();
		eat(lo);
		eat(hi);
		return y;
	}
};

class Graph {
public:
	int time = 0;
	vector<Vertex> vs;
	unordered_map<Set, int, SetHash> map;
	bool fast = false;
	// unordered_set<int> lts;

	Graph() {
	}

	int add(const Set& s) {
		assert(map.count(s) == 0);
		int idx = len(vs);
		assert(idx < MAX_V);
		vs.emplace_back(idx, s);
		map[s] = idx;
		rep0(i, idx) {
			Set t = vs[i].set;
			bool incl = false;
			if (isSubset(t, s)) {
				setLessThan(i, idx);
				incl = true;
			}
			if (isSubset(s, t)) {
				setLessThan(idx, i);
				incl = true;
			}
			if (!incl) {
				Set common = s & t;
				Set s2 = s ^ common;
				Set t2 = t ^ common;
				int si2 = index(s2);
				if (si2 == -1)
					continue;
				int ti2 = index(t2);
				if (ti2 == -1)
					continue;
				if (vs[ti2].bits.test(si2)) {
					setLessThan(i, idx);
				}
				if (vs[si2].bits.test(ti2)) {
					setLessThan(idx, i);
				}
			}
		}
		return idx;
	}

	int index(const Set& s) {
		if (map.count(s) > 0)
			return map[s];
		return -1;
	}

	void setLessThan(int li, int ri) {
		assert(li != -1);
		assert(ri != -1);
		vs[li].lts.push_back(ri);
		// lts.insert(li << 16 | ri);
		vs[li].bits[ri] = true;
		// trace("adding edge ", li, " ", ri, " ", li << 16 | ri);
	}

	bool isLessThan(int li, int ri) {
		assert(li != -1);
		assert(ri != -1);
		// trace("query ", li, " ", ri);
		if (vs[li].bits[ri])
			return true;
		time++;
		if (time % 10 == 0 && timer() > 1500)
			fast = true;
		if (fast)
			return false;
		vector<int> parents;
		vector<pii> edges;
		dfs(li, parents, edges, 0);
		for (auto e : edges) {
			int l = e.first;
			int r = e.second;
			// trace(l, " ", r);
			assert(vs[l].bits[r] == 0);
			setLessThan(l, r);
		}
		return vs[li].bits[ri];
	}

	void dfs(int i, vector<int>& parents, vector<pii>& edges, int depth) {
		auto& vi = vs[i];
		if (len(parents) > 1) {
			for (int p : parents) {
				if (vs[p].bits[i] == 0) {
					edges.emplace_back(p, i);
				}
			}
		}

		// notVis.set(i, false);
		vi.time = time;

		parents.push_back(i);
		if (depth == 0) {
			for (int j : vi.lts) {
				vs[j].time = time;
			}
			for (int j : vi.lts) {
				dfs(j, parents, edges, depth + 1);
			}
		} else {
			for (int j : vi.lts) {
				if (vs[j].time != time) {
					// trace("dfs ", i, " -> ", j);
					dfs(j, parents, edges, depth + 1);
				}
			}
		}
		parents.pop_back();
	}
};

class Solver {
public:
	int n = 0;
	int d = 0;
	int q = 0;
	Rng rng;
	SolverResult res;
	string seedString = "XXXX";
	ostringstream totalOut;

	int ans[MAX_N] = {};
	bool heavy[MAX_N] = {};

	int seed = -1;
	int weights[MAX_N] = {};
	vector<int> sortedGroups;
	int turn = 0;
	int leftQs = 0;
	bool bad = false;
	int iterCount = 0;
	vector<Comparison> comps;
	string reason;
	vector<pii> scoreGraph;
	Graph graph;
	int minScore = 100000000;

	bool shouldSortItems = false;
	bool shouldGreedy = false;
	bool allowMove[MAX_N] = {};
	int sortedItemIndices[MAX_N] = {};
	double roughSortRanks[MAX_N] = {};

	int scores[MAX_N] = {};

	int bestAns[MAX_N] = {};
	set<int> bestAnsLargest;

	Solver() {
	}

	void load(istream& in) {
		in >> n >> d >> q;
		render &= isLocal;
		if (render)
			mov.beginMovie(seedString + "_m.smv");
		if (isLocal) {
			rep0(i, n) {
				in >> weights[i];
			}
		}
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
		// if (shouldSortItems) {
		// 	res.score = -1;
		// 	return;
		// }

		solveMain();

		Comparison comp;
		comp.clear();
		comp.ls |= 1;
		comp.rs |= 2;
		reason = "amatta";
		while (leftQs > 0) {
			ask(comp, true);
		}

		if (render)
			mov.endMovie();

		if (isLocal) {
			res.score = computeScore();
			if (minScore < res.score) {
				res.score = minScore;
			}

			// ostringstream out;
			// print(out);
			// writeText(seedString + "_o.txt", out.str());

			print(totalOut);
			writeTextSafe("out/" + pad4(seed) + ".txt", totalOut.str());
		} else {
			print(cout);
		}
	}

private:
	bool knownToBeLessThan(const Set& l, const Set& r) {
		static int count = 0;
		static bool fast = false;
		if (++count % 1000 == 0) {
			fast = timer() > 1600;
		}
		if (isSubset(l, r))
			return true;
		if (!fast) {
			for (auto& c : comps) {
				if (isSubset(l, c.ls) && isSubset(c.rs, r) && c.sign <= 0)
					return true;
				if (isSubset(l, c.rs) && isSubset(c.ls, r) && c.sign >= 0)
					return true;
			}
		}
		return false;
	}

	int ask(Comparison comp, bool force = false, bool onlyCache = false) {
		comp.normalize();
		int nl = len(comp.ls);
		int nr = len(comp.rs);
		if (nl == 0 || nr == 0) {
			return nl < nr ? -1 : nl > nr ? 1 : 0;
		}

		int expected = 0;
		if (!force) {
			comp.computeHash();
			for (auto& c : comps) {
				if (c.hash == comp.hash) {
					if (c.ls == comp.ls && c.rs == comp.rs) {
						// trace("found in cache");
						return c.sign;
					}
					if (c.ls == comp.rs && c.rs == comp.ls) {
						// trace("found in cache (flipped)");
						return -c.sign;
					}
				}
			}
			if (!isLocal || true) { // slow prune
				{
					int li = graph.index(comp.ls);
					int ri = graph.index(comp.rs);
					if (li == -1)
						li = graph.add(comp.ls);
					if (ri == -1)
						ri = graph.add(comp.rs);
					if (graph.isLessThan(li, ri)) {
						// trace("<");
						expected = -1;
						return -1;
					}
					if (graph.isLessThan(ri, li)) {
						// trace(">");
						expected = 1;
						return 1;
					}
				}
			}
		}
		if (onlyCache) {
			return -2;
		}
		if (leftQs == 0) {
			bad = true;
			return 0;
		}
		leftQs--;
		if (leftQs == 0) {
			trace("num sets: ", len(graph.vs));
		}
		turn++;
		printComment(reason);
		int sign;
		if (isLocal) {
			printNext(comp, totalOut);
			int l = 0;
			int r = 0;
			forin(comp.ls, [&](int i) {
				l += weights[i];
			});
			forin(comp.rs, [&](int i) {
				r += weights[i];
			});
			sign = l < r ? -1 : l > r ? 1 : 0;
		} else {
			printNext(comp, cout);
			char c;
			cin >> c;
			sign = c == '<' ? -1 : c == '>' ? 1 : 0;
		}
		comp.sign = sign;
		comps.push_back(comp);
		int li = graph.index(comp.ls);
		int ri = graph.index(comp.rs);
		if (li == -1)
			li = graph.add(comp.ls);
		if (ri == -1)
			ri = graph.add(comp.rs);
		if (sign < 0)
			graph.setLessThan(li, ri);
		else if (sign > 0)
			graph.setLessThan(ri, li);
		return sign;
	}

	void getGroup(vector<int>& vs, int d) {
		repn(i) {
			if (ans[i] == d)
				vs.push_back(i);
		}
	}

	void getGroup(Set& s, int d) {
		repn(i) {
			if (ans[i] == d)
				s.set(i);
		}
	}

	int groupSize(int g) {
		int res = 0;
		repn(i) {
			if (ans[i] == g)
				res++;
		}
		return res;
	}

	void setGroup(const Set& s, int g) {
		assert(g >= 0 && g < d);
		forin(s, [&](int i) {
			ans[i] = g;
		});
	}

	void resortGroups(const vector<int>& gs) {
		reason = "resort groups";
		if (isLocal) {
			scoreGraph.emplace_back(turn, computeScore());
		}
		auto insert = [&](int g) {
			int ok = -1;
			int ng = len(sortedGroups);
			while (ng - ok > 1) {
				int mid = (ok + ng) / 2;
				if (compareGroups(g, sortedGroups[mid]) >= 0) {
					ok = mid;
				} else {
					ng = mid;
				}
			}
			sortedGroups.insert(sortedGroups.begin() + (ok + 1), g);
		};
		for (int g : gs) {
			assert(g >= 0 && g < d);
			sortedGroups.erase(find(sortedGroups.begin(), sortedGroups.end(), g));
		}
		for (auto& g : gs) {
			insert(g);
		}
	}

	int pickRandom(const Set& vs) {
		int index = rng.nextInt(len(vs));
		int res = -1;
		forin(vs, [&](int i) {
			if (index-- == 0) {
				res = i;
			}
		});
		assert(res != -1);
		return res;
	}

	// -1 if L is better, 1 if R is better, 0 if they are equally good
	int compareDivisions(const Comparison& l, const Comparison& r) {
		int sign = ask(l);
		auto heavierL = sign > 0 ? l.ls : l.rs;
		sign = ask(r);
		auto heavierR = sign > 0 ? r.ls : r.rs;
		Comparison c;
		c.ls = heavierL;
		c.rs = heavierR;
		return ask(c);
	}

	void printNext(const Comparison& comp, ostream& out) const {
		auto& ls = comp.ls;
		auto& rs = comp.rs;
		out << len(ls) << " " << len(rs);
		forin(ls, [&](int i) {
			out << " " << i;
		});
		forin(rs, [&](int i) {
			out << " " << i;
		});
		out << endl;
	}

	int computeScore() {
		if (!isLocal)
			return -1;
		double sums[MAX_D] = {};
		double mean = 0;
		repn(i) {
			int g = ans[i];
			sums[g] += weights[i];
		}
		if (leftQs == 0)
			trace("group sizes:");
		repd(i) {
			if (leftQs == 0)
				trace("  ", (int) sums[i]);
			mean += sums[i];
		}
		mean /= d;
		double var = 0;
		repd(i) {
			var += (sums[i] - mean) * (sums[i] - mean);
		}
		var /= d;
		return 1 + int(round(100 * sqrt(var)));
	}

	void printComment(string s) {
		if (!isLocal)
			return;
		totalOut << "# " << s << endl;
	}

	void printState() {
		if (!isLocal)
			return;
		totalOut << "#c";
		repn(i) {
			totalOut << " " << ans[i];
		}
		totalOut << endl;
	}

	void computeTiers() {
		repn(i) {
			roughSortRanks[i] = 0.0;
		}
		if (q < n * 4 || d > n / 8)
			return;

		reason = "tiers";
		vector<vector<int>> groups;
		int numGs = n / 2;
		groups.resize(numGs);
		repn(i) {
			groups[i % numGs].push_back(i);
		}
		for (auto& group : groups) {
			int m = len(group);
			int compCount = 0;
			mergeInsertionSort(group, [&](int a, int b) {
				compCount++;
				return compareItems(a, b);
			});
			trace("sorted: ", group, " comps=", compCount, "/", COMP_UPPER_BOUNDS[m - 1]);
			rep0(i, m) {
				roughSortRanks[group[i]] = (i + 0.5) / m;
			}
		}
		trace("--");
		repn(i) {
			trace(i, " ", roughSortRanks[i], " ", weights[i]);
		}
	}

	void sortItems() {
		reason = "full sort";
		vector<int> sortedItems;
		repn(i) {
			sortedItems.push_back(i);
		}
		int compCount = 0;
		mergeInsertionSort(
		    sortedItems,
		    [&](int a, int b) {
			    compCount++;
			    return compareItems(a, b);
		    },
		    false);
		trace("sorted: ", sortedItems, " comps=", compCount, "/", COMP_UPPER_BOUNDS[n - 1]);
		assert(compCount <= COMP_UPPER_BOUNDS[n - 1]);
		repn(i) {
			sortedItemIndices[sortedItems[i]] = i;
		}
		repn(i) {
			if (sortedItemIndices[i] < n * 0.1) {
				allowMove[i] = true;
			}
		}
	}

	void sortGroups() {
		reason = "sorting groups";
		sortedGroups.clear();
		repd(i) {
			sortedGroups.push_back(i);
		}
		if (isLocal) {
			vector<pii> sizes;
			repd(i) {
				int sum = 0;
				vector<int> vs;
				getGroup(vs, i);
				for (int i : vs) {
					sum += weights[i];
				}
				sizes.emplace_back(sum, i);
			}
			sort(sizes.begin(), sizes.end());
			trace("true sizes");
			for (pii s : sizes) {
				trace("  ", s.second, ": ", s.first);
			}
		}
		int compCount = 0;
		mergeInsertionSort(
		    sortedGroups,
		    [&](int a, int b) {
			    compCount++;
			    return compareGroups(a, b);
		    },
		    false);
		trace("sorted: ", sortedGroups, " comps=", compCount, "/", COMP_UPPER_BOUNDS[d - 1]);
		assert(compCount <= COMP_UPPER_BOUNDS[d - 1]);
	}

	int compareItems(int i1, int i2) {
		Comparison c;
		c.ls.set(i1);
		c.rs.set(i2);
		return ask(c);
	}

	int compareGroups(int d1, int d2) {
		Comparison c;
		getGroup(c.ls, d1);
		getGroup(c.rs, d2);
		return ask(c);
	}

	static void mergeInsertionSort(vector<int>& vs, function<int(int a, int b)> compare, bool print = true) {
		int m = len(vs);
		if (m <= 1)
			return;
		int numP = m / 2;
		vector<int> highs;
		int lowmap[MAX_N * 5];
		int highmap[MAX_N * 5];
		memset(lowmap, -1, sizeof(lowmap));
		memset(highmap, -1, sizeof(highmap));
		rep0(i, numP) {
			int a = vs[i * 2];
			int b = vs[i * 2 + 1];
			if (compare(a, b) < 0) {
				highs.push_back(b);
				highmap[a] = b;
				lowmap[b] = a;
			} else {
				highs.push_back(a);
				highmap[b] = a;
				lowmap[a] = b;
			}
		}
		mergeInsertionSort(highs, compare, print);
		vector<int> s;
		s.push_back(lowmap[highs[0]]);
		s.insert(s.end(), highs.begin(), highs.end());
		vector<int> lows;
		rep(i, 1, numP) {
			lows.push_back(lowmap[highs[i]]);
		}
		if (m % 2 == 1) {
			lows.push_back(vs.back());
		}
		int numL = len(lows);
		constexpr int sizes[] = {2, 2, 6, 10, 22, 42, 86, 170};
		int idx = 0;
		int begin = 0;
		while (begin < numL) {
			int size = sizes[idx++];
			repr(i, begin, begin + size) {
				if (i < numL) {
					int val = lows[i];
					int high = highmap[val];
					int until = 0;
					while (until < len(s) && s[until] != high) {
						until++;
					}
					if (print)
						trace("insertion size: ", until);
					int ok = -1;
					int ng = until;
					while (ng - ok > 1) {
						int mid = (ok + ng) / 2;
						if (compare(val, s[mid]) >= 0) {
							if (print)
								trace(val, " >= ", s[mid]);
							ok = mid;
						} else {
							if (print)
								trace(val, " < ", s[mid]);
							ng = mid;
						}
					}
					s.insert(s.begin() + (ok + 1), val);
					if (print)
						trace("inserted! ", s);
				}
			}
			begin += size;
		}
		assert(m == len(s));
		rep0(i, m) {
			vs[i] = s[i];
		}
		// rep(i, 1, len(gs)) {
		// 	rep0(j, i) {
		// 		assert(compareGroups(gs[j], gs[i]) <= 0);
		// 	}
		// }
	}

	void init() {
		leftQs = q;
		int sortCount = COMP_UPPER_BOUNDS[n - 1];
		trace(
		    "required comp count for full sort: ", sortCount, " (", sortCount / (double) q * 100, "% of Qs)");
		if (sortCount + 8 * (d - 2) < q * 0.75) {
			shouldSortItems = true;
			shouldGreedy = 1.0 * q / n - 1.1 * pow(n, 1.08) / (d * 0.9 + 4) > 0;
		}
	}

	void placeGreedy(vector<int> sortedItems) {
		repn(i) {
			ans[i] = -1;
		}
		sortedGroups.clear();
		repd(i) {
			sortedGroups.push_back(i);
			ans[sortedItems.back()] = i;
			sortedItems.pop_back();
		}
		reason = "greedy";
		reverse(sortedGroups.begin(), sortedGroups.end());
		printState();
		while (!sortedItems.empty()) {
			ans[sortedItems.back()] = sortedGroups[0];
			sortedItems.pop_back();
			resortGroups({sortedGroups[0]});
			printState();
		}
		printState();
	}

	void solveMain() {
		int binSearchCount = shouldSortItems ? 2 : (int) (sqrt(n * d) / 6 + 1);
		int binSearchSize = 1;

		if (shouldSortItems) {
			sortItems();
			int g = 0;
			int layer = 0;
			vector<pii> sorted;
			repn(i) {
				sorted.emplace_back(sortedItemIndices[i], i);
			}
			sort(sorted.begin(), sorted.end());
			vector<int> sortedItems;
			repn(i) {
				sortedItems.push_back(sorted[i].second);
			}
			if (shouldGreedy) {
				placeGreedy(sortedItems);
				binSearchCount = 0;
			} else {
				cerr << "no greedy since " << log2(d) * (n - d) << " " << leftQs * 0.7 << endl;
				repn(i) {
					ans[sortedItems[n - 1 - i]] = layer == 0 ? g : d - 1 - g;
					g++;
					if (g == d) {
						g = 0;
						layer++;
					}
				}
				sortGroups();
			}
		} else {
			computeTiers();
			makeInitialSolution();
			sortGroups();
		}
		printState();

		int noHit = 0;
		rep0(t, 100000) {
			iterCount = t;
			if (timer() > TL) {
				trace("TL ", t, " ", leftQs);
				break;
			}
			if (leftQs == 0) {
				trace("finish. trials=", t);
				break;
			}

			double progress = turn / (double) q;

			vector<pii> lrs;
			bool doBinSearch = t < binSearchCount;
			if (doBinSearch) {
				rep0(i, binSearchSize) {
					lrs.emplace_back(sortedGroups[i], sortedGroups[d - 1 - i]);
				}
			} else {
				int l = sortedGroups[0];
				int r = sortedGroups[d - 1];
				double prob = pow(1e-6, 1.0 / d) * min(1.0, t / 500.0);
				if (rng.nextFloat() < 0.5) {
					int i = 0;
					while (i < d - 2) {
						if (rng.nextFloat() > prob)
							break;
						i++;
					}
					l = sortedGroups[i];
				} else {
					int i = 0;
					while (i < d - 2) {
						if (rng.nextFloat() > prob)
							break;
						i++;
					}
					r = sortedGroups[d - 1 - i];
				}
				lrs.emplace_back(l, r);
			}
			int pq = leftQs;
			equalizePairs(lrs, doBinSearch);
			if (pq == leftQs) {
				noHit++;
			} else {
				noHit = 0;
				printState();
			}
		}

		// repn(i) {
		// 	trace(i, ": ", scores[i]);
		// }
	}

	void placeGreedyStream(int firstN) {
		MergeStream ms;
		ms.init(n);
		auto compare = [&](int a, int b) {
			return -compareItems(a, b);
		};
		int left = n;
		auto next = [&]() {
			int res = ms.root->next(compare);
			sortedItemIndices[res] = --left;
			return res;
		};
		reason = "greedy stream";
		repn(i) {
			ans[i] = -1;
		}
		sortedGroups.clear();
		int count = 0;
		repd(i) {
			sortedGroups.push_back(i);
			ans[next()] = i;
			count++;
		}
		reverse(sortedGroups.begin(), sortedGroups.end());
		printState();
		while (count < firstN) {
			ans[next()] = sortedGroups[0];
			resortGroups({sortedGroups[0]});
			printState();
			count++;
		}
		repn(i) {
			if (ans[i] != -1)
				heavy[i] = true;
		}
		vector<double> typical;
		double max = 1e5 * n / d;
		double sum = 0;
		repn(i) {
			double t = (i + 0.5) / n;
			double w = -1e5 * log(1 - (1 - exp(-1e-5 * max)) * t);
			typical.push_back(w);
			sum += w;
		}
		double leftSum = sum;
		double estimatedSums[MAX_D] = {};
		repn(i) {
			if (ans[i] != -1) {
				double w = typical[sortedItemIndices[i]];
				estimatedSums[ans[i]] += w;
				leftSum -= w;
			}
		}
		double mean = leftSum / left;
		rep0(t, left) {
			int minIndex = -1;
			double minW = 1e9;
			repd(i) {
				if (estimatedSums[i] < minW) {
					minW = estimatedSums[i];
					minIndex = i;
				}
			}
			assert(minIndex != -1);
			estimatedSums[minIndex] += mean;
			bool ok = false;
			repn(i) {
				if (ans[i] == -1) {
					ans[i] = minIndex;
					ok = true;
					break;
				}
			}
			assert(ok);
		}

		// // place the rest
		// int nextD = 0;
		// while (count < n) {
		// 	int nextItem = -1;
		// 	repn(i) {
		// 		if (ans[i] == -1) {
		// 			nextItem = i;
		// 			break;
		// 		}
		// 	}
		// 	ans[nextItem] = nextD;
		// 	nextD = (nextD + 1) % d;
		// 	count++;
		// }
		sortGroups();
		printState();
	}

	void makeInitialSolution() {
		// repn(i) {
		// 	ans[i] = i % d;
		// }
		int g = 0;
		int layer = 0;
		vector<pdi> sorted;
		repn(i) {
			sorted.emplace_back(roughSortRanks[i], i);
		}
		sort(sorted.begin(), sorted.end());
		vector<int> sortedItems;
		repn(i) {
			sortedItems.push_back(sorted[i].second);
		}
		repn(i) {
			ans[sortedItems[n - 1 - i]] = layer == 0 ? g : d - 1 - g;
			g++;
			if (g == d) {
				g = 0;
				layer++;
			}
		}
		sortGroups();
	}

	Comparison getDivision(int l, int r) {
		Comparison c;
		getGroup(c.ls, l);
		getGroup(c.rs, r);
		return c;
	}

	Comparison equalizeBinSearch(int l, int r) {
		reason = "bin " + tos(l) + " " + tos(r);
		vector<int> items;
		getGroup(items, l);
		getGroup(items, r);
		rep(i, 1, len(items)) {
			int j = rng.nextInt(i + 1);
			swap(items[i], items[j]);
		}
		int m = len(items);
		int ok = 0;
		int ng = m;
		while (ng - ok > 1) {
			int mid = (ok + ng) / 2;
			Comparison c;
			insert(c.ls, items.begin(), items.begin() + mid);
			insert(c.rs, items.begin() + mid, items.end());
			if (ask(c) <= 0) {
				ok = mid;
			} else {
				ng = mid;
			}
		}
		reason += " comp";
		Comparison c1;
		insert(c1.ls, items.begin(), items.begin() + ok);
		insert(c1.rs, items.begin() + ok, items.end());
		Comparison c2;
		insert(c2.ls, items.begin(), items.begin() + ng);
		insert(c2.rs, items.begin() + ng, items.end());
		return compareDivisions(c1, c2) < 0 ? c1 : c2;
	}

	Comparison equalizeMoveSwapSorted(Comparison div, int l, int r, int depth, Comparison base) {
		assert(shouldSortItems);
		if (ask(div) > 0) {
			swap(div.ls, div.rs);
			swap(l, r);
		}
		reason = "moveswap fs " + tos(l) + " " + tos(r) + " (depth=" + tos(depth) + ")";

		int i;
		int idx1;
		while (true) {
			i = pickRandom(div.rs);
			idx1 = sortedItemIndices[i];
			double ratio = idx1 / (double) n * 0.95;
			if (rng.nextFloat() < ratio || scores[i] == 0 && rng.nextFloat() < 0.9 * (1 - 4.0 * d / n))
				continue;
			break;
		}

		vector<int> swapTargets;
		{
			vector<pii> sorted;
			forin(div.ls, [&](int j) {
				int idx2 = sortedItemIndices[j];
				if (idx2 < idx1) {
					sorted.emplace_back(idx2, j);
				}
			});
			sort(sorted.begin(), sorted.end());
			swapTargets.push_back(-1);
			for (pii p : sorted) {
				swapTargets.push_back(p.second);
			}
		}
		bool first = true;
		Comparison ndiv = div;
		int usedJ = -1;
		while (!swapTargets.empty()) {
			int j = swapTargets.back();
			swapTargets.pop_back();
			Comparison ndiv2 = div;
			ndiv2.rs.set(i, false);
			ndiv2.ls.set(i);
			if (j != -1) {
				ndiv2.ls.set(j, false);
				ndiv2.rs.set(j);
			}
			int sign = ask(ndiv2);
			if (sign <= 0) {
				ndiv = ndiv2;
				usedJ = j;
				first = false;
			} else {
				if (first || compareDivisions(ndiv, ndiv2) >= 0) {
					usedJ = j;
					ndiv = ndiv2;
				}
				break;
			}
		}
		if (compareDivisions(base, ndiv) >= 0) {
			if (bad) {
				trace(":(");
				return div;
			}
			scores[i]++;
			if (usedJ != -1)
				scores[usedJ]++;
			// trace("got better! (sorted moveswap) i=", i, " j=", usedJ, " ", turn);
			printComment("got better! (sorted moveswap)");
			return ndiv;
		}

		if (rng.nextFloat() < 0.25) {
			// search further
			printComment("going deeper... " + tos(depth));
			ndiv = equalizeMoveSwapSorted(ndiv, l, r, depth + 1, base);
			if (compareDivisions(base, ndiv) > 0) {
				trace("got better! (sorted deeper swap) i=", i, " j=", usedJ, " ", turn);
				printComment("got better! (sorted deeper swap)");
				return ndiv;
			}
		}
		return div;
	}

	Comparison equalizeMoveSwap(Comparison div, int l, int r, int depth, Comparison base) {
		if (ask(div) > 0) {
			swap(div.ls, div.rs);
			swap(l, r);
		}
		reason = "moveswap " + tos(l) + " " + tos(r);
		int i;
		while (true) {
			i = pickRandom(div.rs);
			if (rng.nextFloat() < roughSortRanks[i] ||
			    scores[i] == 0 && rng.nextFloat() < 0.9 * (1 - 4.0 * d / n))
				continue;
			break;
		}

		int numPickTrials = 1 + (int) (min(8.0, 1.0 * q / n) * (1 - 4.0 * d / n));

		vector<int> knownSwapTargets;
		bool ng[MAX_N] = {};
		forin(div.ls, [&](int j) {
			Set l, r;
			l.set(i);
			r.set(j);
			if (knownToBeLessThan(l, r)) {
				ng[j] = true;
			}
			if (knownToBeLessThan(r, l)) {
				knownSwapTargets.push_back(j);
			}
		});
		for (int k : knownSwapTargets) {
			for (int j : knownSwapTargets) {
				if (k == j)
					continue;
				Set l, r;
				l.set(j);
				r.set(k);
				if (knownToBeLessThan(l, r)) {
					ng[j] = true;
				}
			}
		}
		// erase non-optimal targets
		rep0(i, len(knownSwapTargets)) {
			if (ng[knownSwapTargets[i]]) {
				knownSwapTargets[i] = -1;
			}
		}
		knownSwapTargets.erase(
		    remove(knownSwapTargets.begin(), knownSwapTargets.end(), -1), knownSwapTargets.end());

		int j;
		vector<int> swapTargets;
		swapTargets.push_back(-1);
		rep0(t, numPickTrials) {
			bool allNg = true;
			forin(div.ls, [&](int j) {
				if (!ng[j])
					allNg = false;
			});
			if (allNg)
				break;
			do {
				while (true) {
					j = pickRandom(div.ls);
					if (ng[j] || rng.nextFloat() < roughSortRanks[j] ||
					    scores[j] == 0 && rng.nextFloat() < 0.9 * (1 - 4.0 * d / n))
						continue;
					break;
				}
			} while (i == j);

			if (compareItems(i, j) > 0) {
				swapTargets.push_back(j);
				break;
			} else {
				ng[j] = true;
			}
		}
		if (len(swapTargets) == 1 && !knownSwapTargets.empty()) {
			swapTargets.push_back(knownSwapTargets[rng.nextInt(len(knownSwapTargets))]);
		}

		bool first = true;
		Comparison ndiv = div;
		int usedJ = -1;
		while (!swapTargets.empty()) {
			int j = swapTargets.back();
			swapTargets.pop_back();
			Comparison ndiv2 = div;
			ndiv2.rs.set(i, false);
			ndiv2.ls.set(i);
			if (j != -1) {
				ndiv2.ls.set(j, false);
				ndiv2.rs.set(j);
			}
			int sign = ask(ndiv2);
			if (sign <= 0) {
				ndiv = ndiv2;
				usedJ = j;
				first = false;
			} else {
				if (first || compareDivisions(ndiv, ndiv2) >= 0) {
					usedJ = j;
					ndiv = ndiv2;
				}
				break;
			}
		}
		if (compareDivisions(base, ndiv) >= 0) {
			if (bad) {
				trace(":(");
				return div;
			}
			scores[i]++;
			if (usedJ != -1)
				scores[usedJ]++;
			trace("got better! (moveswap) i=", i, " j=", usedJ, " ", turn);
			printComment("got better! (moveswap)");
			return ndiv;
		}

		double prob = 0.1 * min(8.0, 1.0 * q / n) / 8 * (1 - 4.0 * d / n);
		if (rng.nextFloat() < prob) {
			// search further
			printComment("going deeper... " + tos(depth));
			ndiv = equalizeMoveSwap(ndiv, l, r, depth + 1, base);
			if (compareDivisions(base, ndiv) > 0) {
				trace("got better! (deeper swap) i=", i, " j=", usedJ, " ", turn);
				printComment("got better! (deeper swap)");
				return ndiv;
			}
		}
		return div;
	}

	bool equalizePair(int l, int r, bool startWithBinSearch) {
		Comparison div = getDivision(l, r);
		Comparison ndiv = div;
		if (startWithBinSearch) {
			ndiv = equalizeBinSearch(l, r);
			if (compareDivisions(div, ndiv) >= 0) {
				if (len(div.ls & ndiv.rs) + len(div.rs & ndiv.ls) >
				    len(div.ls & ndiv.ls) + len(div.rs & ndiv.rs)) {
					swap(ndiv.ls, ndiv.rs);
				}
				trace("got better! (bin) ", turn);
				printComment("got better! (bin)");
			} else {
				trace("bruh (bin) ", turn);
				printComment("bruh (bin)");
				ndiv = div;
			}
		} else {
			if (shouldSortItems)
				ndiv = equalizeMoveSwapSorted(ndiv, l, r, 0, ndiv);
			else
				ndiv = equalizeMoveSwap(ndiv, l, r, 0, ndiv);
		}
		if (compareDivisions(div, ndiv) > 0) {
			if (bad) {
				trace(":(");
				return false;
			}
			setGroup(ndiv.ls, l);
			setGroup(ndiv.rs, r);
			return true;
		}
		return false;
	}

	void equalizePairs(const vector<pii>& lrs, bool startWithBinSearch) {
		vector<int> toSort;
		for (pii lr : lrs) {
			int l = lr.first;
			int r = lr.second;
			if (equalizePair(l, r, startWithBinSearch)) {
				toSort.push_back(l);
				toSort.push_back(r);
			}
		}
		resortGroups(toSort);
	}

	void print(ostream& out) const {
		repn(i) {
			if (i > 0)
				out << " ";
			out << ans[i];
		}
		out << endl;
	}
};

class TestCase {
private:
public:
	int n;
	int d;
	int q;
	double nt;
	double dt;
	double qt;
	int score1;
	int score2;

	TestCase(int n, int d, int q, int score1, int score2) : n(n), d(d), q(q), score1(score1), score2(score2) {
		double maxD = n / 4;
		nt = (n - 30.0) / (100.0 - 30.0);
		dt = (d - 2.0) / (maxD - 2.0);
		qt = (log2(q / (double) n) - 1.0) / (5.0 - 1.0);
		nt = clamp(nt, 1e-3, 1 - 1e-3);
		dt = clamp(dt, 1e-3, 1 - 1e-3);
		qt = clamp(qt, 1e-3, 1 - 1e-3);
	}
};

void compareScores(string name1, string name2) {
	mov.beginMovie(name1 + " vs " + name2 + ".smv");
	name1 += ".txt";
	name2 += ".txt";
	ifstream in1(name1);
	ifstream in2(name2);
	constexpr int NUM_CASES = 10000;
	int scores1[NUM_CASES] = {};
	int scores2[NUM_CASES] = {};
	memset(scores1, -1, sizeof(scores1));
	memset(scores2, -1, sizeof(scores2));
	trace("loading ", name1);
	double sum1 = 0;
	double sum2 = 0;
	while (true) {
		int seed, score;
		in1 >> seed >> score;
		if (in1.eof())
			break;
		scores1[seed] = score;
		sum1 += log2(score);
	}
	trace("loading ", name2);
	while (true) {
		int seed, score;
		in2 >> seed >> score;
		if (in2.eof())
			break;
		scores2[seed] = score;
		sum2 += log2(score);
	}
	sum1 = 0;
	sum2 = 0;
	rep0(i, NUM_CASES) {
		sum1 += scores1[i] * 1.0 / (max(scores1[i], scores2[i]));
		sum2 += scores2[i] * 1.0 / (max(scores1[i], scores2[i]));
	}
	vector<TestCase> points;
	rep0(i, NUM_CASES) {
		if (i % 100 == 0)
			trace("loading... ", i);
		int s1 = scores1[i];
		int s2 = scores2[i];
		if (s1 == -1 || s2 == -1)
			continue;
		ifstream in(seedToFileName(i));
		int n, d, q;
		in >> n >> d >> q;
		points.emplace_back(n, d, q, s1, s2);
	}
	trace(sum1, " ", sum2);
	auto drawTable = [&](string name, string xlabel, string ylabel, int xdiv, int ydiv, int frames,
	                     function<bool(int xi, int yi, int frame, const TestCase& t)> filter) {
		mov.target(name);
		rep0(frame, frames) {
			mov.clearFrame();
			double w = 30;
			double h = 30;
			double ts = 5;
			mov.fill(1);
			mov.textSize(ts);
			mov.textAlign(Movie::CENTER);
			mov.text(xlabel, w * xdiv * 0.5, -ts);
			mov.textAlign(Movie::RIGHT);
			mov.text(ylabel, 0, h * ydiv * 0.5 - ts * 0.5);
			rep0(xi, xdiv) {
				rep0(yi, ydiv) {
					int win1 = 0;
					int win2 = 0;
					int total = 0;
					for (auto& p : points) {
						if (!filter(xi, yi, frame, p))
							continue;
						if (p.score1 <= p.score2)
							win1++;
						if (p.score1 >= p.score2)
							win2++;
						total++;
					}
					if (total == 0)
						continue;
					double ratio = (double) win2 / (win1 + win2);
					double x = xi * w;
					double y = yi * h;
					mov.noStroke();
					mov.fill(1 - ratio, ratio, 0);
					mov.rect(x, y, w, h);
					mov.fill(1);
					mov.textAlign(Movie::CENTER);
					mov.text(tos(win1) + "/" + tos(win2) + "\n" + tos((int) (ratio * 100 + 0.5)) + "%",
					    x + w * 0.5, y + h * 0.5 - ts);
				}
			}
			mov.endFrame();
		}
	};
	rep(divs, 2, 10 + 1) {
		drawTable("D-Q plane div" + tos(divs), "D lv", "Q lv", divs, divs, divs + 1,
		    [&](int xi, int yi, int f, const TestCase& t) {
			    if ((int) (t.dt * divs) != xi)
				    return false;
			    if ((int) (t.qt * divs) != yi)
				    return false;
			    if (f != 0 && (int) (t.nt * divs) != f - 1)
				    return false;
			    return true;
		    });
		drawTable("N-Q plane div" + tos(divs), "N lv", "Q lv", divs, divs, divs + 1,
		    [&](int xi, int yi, int f, const TestCase& t) {
			    if ((int) (t.nt * divs) != xi)
				    return false;
			    if ((int) (t.qt * divs) != yi)
				    return false;
			    if (f != 0 && (int) (t.dt * divs) != f - 1)
				    return false;
			    return true;
		    });
		drawTable("N-D plane div" + tos(divs), "N lv", "D lv", divs, divs, divs + 1,
		    [&](int xi, int yi, int f, const TestCase& t) {
			    if ((int) (t.nt * divs) != xi)
				    return false;
			    if ((int) (t.dt * divs) != yi)
				    return false;
			    if (f != 0 && (int) (t.qt * divs) != f - 1)
				    return false;
			    return true;
		    });
	}
	mov.endMovie();
}

int main() {
#if 1
	isLocal = false;
	timer(true);
	Solver sol;
	sol.rng.seed(334334334); // ????????
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
	int iter = 10;
	double sum = 0;
	rep0(t, iter) {
		timer(true);
		Solver sol;
		sol.load(seed);
		// if (sol.shouldSortItems) {
		// 	cout << -1 << endl;
		// 	return 0;
		// }
		sol.rng.seed(12345 + t * 1000);
		sol.solve();
		sum += sol.res.score;
	}
	cout << (int) round(sum / iter) << endl;
#elif 1
	timeScale = 1.0;
	int num = 100;
	int from = 0;
	int stride = 1;
	int single = -1; // 21, 77
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
