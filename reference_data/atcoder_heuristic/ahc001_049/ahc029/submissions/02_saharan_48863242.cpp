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

template <typename T>
int len(const priority_queue<T>& a) {
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

	void setState(ull y) {
		this->y = y;
	}

	void seed(int s) {
		y = s + INCR;
		next();
	}

	int nextInt() {
		return next();
	}

	int nextInt(int mod) {
		return (int) ((ull) next() * mod >> 31);
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
constexpr int T = 1000;
constexpr int MAX_N = 7;
constexpr int MAX_M = 8;
constexpr int MAX_K = 5;
constexpr int INF = 1000000;
#define repn(i) rep0(i, n)
#define repm(i) rep0(i, m)
#define repk(i) rep0(i, k)

constexpr double linearstep(double edge0, double edge1, double a) {
	return clamp((a - edge0) / (edge1 - edge0), 0.0, 1.0);
}

struct Card {
	int kind = 0;
	ll work = 0;

	Card(int kind = 0, ll work = 0) : kind(kind), work(work) {
	}

	// bool operator==(const Card& c) {
	// 	return kind == c.kind && work == c.work;
	// }

	bool similar(const Card& c, int invest) const {
		return kind == c.kind && (work >> invest) == (c.work >> invest);
	}
};

struct CardCand {
	int kind = 0;
	ll work = 0;
	ll cost = 0;

	CardCand(int kind = 0, ll work = 0, ll cost = 0) : kind(kind), work(work), cost(cost) {
	}

	CardCand operator*(int mult) const {
		return {kind, work * mult, cost * mult};
	}

	Card toCard() const {
		return {kind, work};
	}

	bool isBetterThan(const CardCand& c) {
		if (kind != c.kind)
			return false;
		return work >= c.work && cost <= c.cost && (work != c.work || cost != c.cost);
	}
};

struct Project {
	ll left = 0;
	ll value = 0;
	double usualValue = 0;
	double boostedValue = 0;

	Project(ll left = 0, ll value = 0) : left(left), value(value) {
	}

	// Project(ll total, ll left, ll value) : left(left), value(value) {
	// }

	Project operator*(int mult) {
		return {left * mult, value * mult};
	}
};

struct State {
	Card cards[MAX_N];
	array<Project, MAX_M> projs;
	ll money = 0;
	int invest = 0;
	int turn = 0;
	int used = -1;
	ll workSum = 0;
	ll workAllSum = 0;
	int fireCount = 0;
	int bombCount = 0;
	int investCount = 0;

	void updateSums(int n) {
		workSum = 0;
		workAllSum = 0;
		fireCount = 0;
		bombCount = 0;
		investCount = 0;
		repn(i) {
			const auto& card = cards[i];
			switch (card.kind) {
			case 0:
				workSum += card.work;
				break;
			case 1:
				workAllSum += card.work;
				break;
			case 2:
				fireCount++;
				break;
			case 3:
				bombCount++;
				break;
			case 4:
				investCount++;
				break;
			}
		}
	}

	void setCard(int i, const Card& card) {
		auto& oldCard = cards[i];
		switch (oldCard.kind) {
		case 0:
			workSum -= oldCard.work;
			break;
		case 1:
			workAllSum -= oldCard.work;
			break;
		case 2:
			fireCount--;
			break;
		case 3:
			bombCount--;
			break;
		case 4:
			investCount--;
			break;
		}
		switch (card.kind) {
		case 0:
			workSum += card.work;
			break;
		case 1:
			workAllSum += card.work;
			break;
		case 2:
			fireCount++;
			break;
		case 3:
			bombCount++;
			break;
		case 4:
			investCount++;
			break;
		}
		oldCard = card;
	}

	void dump(int n, int m) const {
		trace("-- turn=", turn, " money=", money, " invest=", invest);
		tracen("cards: [");
		repn(i) {
			if (i > 0)
				tracen(", ");
			auto& card = cards[i];
			switch (card.kind) {
			case 0:
				tracen("work=", card.work);
				break;
			case 1:
				tracen("work-all=", card.work);
				break;
			case 2:
				tracen("erase");
				break;
			case 3:
				tracen("erase-all");
				break;
			case 4:
				tracen("invest");
				break;
			}
			if (used == i)
				tracen("(used)");
		}
		trace("]");
		trace("projects:");
		repm(i) {
			trace("  left=", projs[i].left, " value=", projs[i].value);
		}
	}
};

struct Move {
	int data = 0;

	Move(int cand, int card, int proj) : data(cand << 6 | card << 3 | proj) {
	}

	Move(int data = 0) : data(data) {
	}

	int cand() const {
		return data >> 6 & 7;
	}

	int card() const {
		return data >> 3 & 7;
	}

	int proj() const {
		return data & 7;
	}
};

struct Pool {
private:
	int stack[1000] = {};
	int stackCount = 0;

public:
	Project projs[T * MAX_M];
	CardCand cands[T * MAX_K];
	int projsUsed = 0;
	int candsUsed = 0;

	void save() {
		assert(stackCount < 999);
		stack[stackCount++] = projsUsed;
		stack[stackCount++] = candsUsed;
	}

	void restore() {
		assert(stackCount > 1);
		candsUsed = stack[--stackCount];
		projsUsed = stack[--stackCount];
	}

	Project nextProj(int invest) {
		return projs[projsUsed++] * (1 << invest);
	}

	CardCand nextCand(int invest) {
		return cands[candsUsed++] * (1 << invest);
	}
};

class Problem {
private:
	istream* in = nullptr;
	bool isLocal = false;
	deque<Project> projPool;
	deque<CardCand> candPool;

public:
	int n = 0;
	int m = 0;
	int k = 0;
	State& st;
	array<CardCand, MAX_K> cands = {};

	Problem(State& st) : st(st) {
	}

	void load(istream& in, bool isLocal) {
		this->isLocal = isLocal;
		int t;
		in >> n >> m >> k >> t;
		st.money = 0;
		st.invest = 0;
		st.used = -1;
		assert(t == 1000);
		if (!isLocal) {
			this->in = &in;
			repn(i) {
				int kind, work;
				in >> kind >> work;
				st.cards[i] = {kind, work};
			}
			repm(i) {
				int left, value;
				in >> left >> value;
				st.projs[i] = {left, value};
			}
		} else {
			repm(i) {
				int left, value;
				in >> left >> value;
				st.projs[i] = {left, value};
			}
			rep0(t, T) {
				repm(i) {
					int left, value;
					in >> left >> value;
					projPool.emplace_back(left, value);
				}
			}
			repn(i) {
				int kind, work;
				in >> kind >> work;
				st.cards[i] = {kind, work};
			}
			rep0(t, T) {
				repk(i) {
					int kind, work, cost;
					in >> kind >> work >> cost;
					candPool.push_back({kind, work, cost});
				}
			}
		}
	}

	void act(int ci, int pi, ostream& out) {
		out << ci << " " << pi << endl;
		assert(st.used == -1);
		st.used = ci;
		auto& projs = st.projs;
		auto& cards = st.cards;
		static array<Project, MAX_M> newProjs;
		if (!isLocal) {
			repm(i) {
				int left, value;
				*in >> left >> value;
				newProjs[i] = {left, value};
			}
		}
		assert(ci >= 0 && ci < n);
		assert(pi >= 0 && pi < m);
		int kind = cards[ci].kind;
		int work = cards[ci].work;
		switch (kind) {
		case 0: {
			auto& proj = projs[pi];
			proj.left -= work;
			if (proj.left <= 0) {
				st.money += proj.value;
				if (isLocal) {
					proj = projPool.front() * (1 << st.invest);
					projPool.pop_front();
				} else {
					proj = newProjs[pi];
				}
			}
		} break;
		case 1:
			assert(pi == 0);
			repm(i) {
				auto& proj = projs[i];
				proj.left -= work;
				if (proj.left <= 0) {
					st.money += proj.value;
					if (isLocal) {
						proj = projPool.front() * (1 << st.invest);
						projPool.pop_front();
					} else {
						proj = newProjs[i];
					}
				}
			}
			break;
		case 2:
			if (isLocal) {
				projs[pi] = projPool.front() * (1 << st.invest);
				projPool.pop_front();
			} else {
				projs[pi] = newProjs[pi];
			}
			break;
		case 3:
			assert(pi == 0);
			if (isLocal) {
				repm(i) {
					projs[i] = projPool.front() * (1 << st.invest);
					projPool.pop_front();
				}
			} else {
				projs = newProjs;
			}
			break;
		case 4:
			assert(pi == 0);
			assert(st.invest < 20);
			st.invest++;
			break;
		default:
			assert(false);
		}
		if (isLocal) {
			repk(i) {
				cands[i] = candPool.front() * (1 << st.invest);
				candPool.pop_front();
			}
		} else {
			ll money;
			*in >> money;
			assert(money == st.money);
			repk(i) {
				auto& cand = cands[i];
				*in >> cand.kind >> cand.work >> cand.cost;
			}
		}
	}

	void choose(int ci, ostream& out) {
		out << ci << endl;
		assert(st.used != -1);
		st.cards[st.used] = {cands[ci].kind, cands[ci].work};
		st.used = -1;
		st.money -= cands[ci].cost;
		assert(st.money >= 0);
		st.turn++;
	}
};

struct SolverResult {
	ll score = 0;
};

bool verbose = false;

class Solver {
public:
	int seed;
	Rng rng;
	SolverResult res;
	string seedString = "XXXX";
	Problem p;
	State st;
	ostringstream out;
	Pool pool;
	int totalSeen = 0;
	int kindCounts[5] = {};

	int n = 0;
	int m = 0;
	int k = 0;

	Solver() : p(st) {
	}

	void load(istream& in) {
		p.load(in, isLocal);
		n = p.n;
		m = p.m;
		k = p.k;
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

		res.score = st.money == 0 ? 1 : st.money;
		// res.score = st.invest;

		if (isLocal) {
			// print(out);
			writeTextSafe("out/" + pad4(seed) + ".txt", out.str());
		} else {
			// print(cout);
		}
	}

private:
	// 1.8: 0.6575908230000007
	// 1.6: 0.7818325586699985
	// 1.5: 0.8801435253000006
	// 1.4: 0.8108118979000006
	// 1.3: 0.7982428438999997
	// 1.2: 0.6620637463900004
	double moneyEfficiency = 1.5;
	// 0.95: 0.4913762564
	// 0.92: 0.7558127614000005
	// 0.9 : 0.8502181248999998
	// 0.88: 0.8091534436999988
	// 0.85: 0.6873495304000008
	double maxSkipRatio = 0.9;
	double decay = 0.97;
	double decayLookup[4900] = {};
	int totalPlayouts = 0;
	int newProjTurns = 48;
	double newProjValue = 0;

	void init() {
		// regularization
		rep0(i, 10) seeCard(0);
		rep0(i, 10) seeCard(1);
		rep0(i, 10) seeCard(2);
		rep0(i, 5) seeCard(3);
		rep0(i, 1) seeCard(4);
		// see initial cards
		repk(i) {
			seeCard(st.cards[i].kind);
		}
		// precompute stuff
		rep0(i, 4900) {
			decayLookup[i] = pow(decay, i * 0.25);
		}
		newProjValue = 52 * decayByTurns(52);
	}

	void seeCard(int kind) {
		kindCounts[kind]++;
		totalSeen++;
	}

	double kindProb(int kind) const {
		return kindCounts[kind] / (double) totalSeen;
	}

	Project generateProj() {
		double b = rng.nextFloat(2, 8);
		int left = (int) round(pow(2, b));
		int value = (int) round(pow(2, clamp(b + rng.nextNormal() * 0.5, 0.0, 10.0)));
		return {left, value};
	}

	CardCand generateCand(int kind = -1) {
		if (totalSeen > 100)
			kind = -1;
		if (kind == -1) {
			double weights[5] = {
			    kindProb(0), kindProb(1), kindProb(2), kindProb(3), 0}; // won't generate invest card
			double weightSum = weights[0] + weights[1] + weights[2] + weights[3] + weights[4];
			double x = rng.nextFloat(0, weightSum);
			kind = 4;
			rep0(i, 5) {
				x -= weights[i];
				if (x < 0) {
					kind = i;
					break;
				}
			}
		}
		CardCand cand;
		cand.kind = kind;
		int w = rng.nextInt(1, 50);
		switch (kind) {
		case 0:
			cand.work = w;
			cand.cost = clamp(round(w + rng.nextNormal() * w / 3.0), 1.0, 10000.0);
			break;
		case 1:
			cand.work = w;
			cand.cost = clamp(round(w * m + rng.nextNormal() * w * m / 3.0), 1.0, 10000.0);
			break;
		case 2:
			cand.work = 0;
			cand.cost = rng.nextInt(0, 10);
			break;
		case 3:
			cand.work = 0;
			cand.cost = rng.nextInt(0, 10);
			break;
		case 4:
			cand.work = 0;
			cand.cost = rng.nextInt(200, 1000);
			break;
		default:
			assert(false);
		}
		return cand;
	}

	void shufflePool(int index, int depth) {
		int numProjs = depth * m;
		int numCands = depth * k;
		pool.projsUsed = 0;
		pool.candsUsed = 0;
		rep0(i, numProjs) {
			pool.projs[i] = generateProj();
		}
		rep0(i, numCands) {
			if (i % k == 0) {
				pool.cands[i] = {0, 1, 0};
				continue;
			}
			pool.cands[i] = generateCand(rng.nextInt(4));
		}
	}

	void solveMain() {
		auto& out = isLocal ? this->out : cout;
		rep0(t, T) {
			auto next = determineNextMove();
			auto move = next.first;
			if (t != 0)
				p.choose(move.cand(), out);
			p.act(move.card(), move.proj(), out);
			repk(i) {
				seeCard(p.cands[i].kind);
			}
		}
		p.choose(0, out);
	}

	void updateUsualValues(State& st) const {
		repm(i) {
			updateUsualValue(st.projs[i]);
		}
	}

	void updateUsualValue(Project& proj) const {
		double invScale = 1 / (double) (1 << st.invest);
		proj.usualValue = decayByTurns(proj.left * invScale) * proj.value;
		double maxUsableMoney = proj.left / moneyEfficiency * maxSkipRatio;
		double skipAmount = maxUsableMoney * moneyEfficiency;
		double requiredTurns = (proj.left - skipAmount) * invScale;
		proj.boostedValue = decayByTurns(requiredTurns) * proj.value - maxUsableMoney;
	}

	double decayByTurns(double turns) const {
		assert(turns >= 0 && turns <= 1200);
		turns *= 4;
		int i1 = (int) turns;
		int i2 = i1 + 1;
		double f = turns - i1;
		return mix(decayLookup[i1], decayLookup[i2], f);
	}

	double evaluate(State& st) {
		if (st.invest > 20)
			return -INF;

		auto& cards = st.cards;
		auto& projs = st.projs;

		double progress = st.turn / 999.0;
		double scale = 1 << st.invest;
		double invScale = 1 / scale;

		ll workSum = st.workSum;
		ll workAllSum = st.workAllSum;
		int effectiveInvest = st.invest + st.investCount;
		int fireCount = st.fireCount;
		int bombCount = st.bombCount;
		if (st.used != -1) {
			auto& card = st.cards[st.used];
			switch (card.kind) {
			case 0:
				workSum -= card.work;
				break;
			case 1:
				workAllSum -= card.work;
				break;
			case 2:
				fireCount--;
				break;
			case 3:
				bombCount--;
				break;
			case 4:
				effectiveInvest--;
				break;
			}
		}
		effectiveInvest = min(20, effectiveInvest);
		double effectiveWorkSum = workSum + workAllSum * m / (1 + m * 0.1);

		double score = st.money + effectiveWorkSum;
		double scoreDelta = 0;
		double fireGain = 0;
		double bombGain = 0;
		double leftMoney = st.money;
		double endTurn = min(1200.0, 1000 + (999 - st.turn) * 0.6);

		if (fireCount > 0) {
			double gains[MAX_M] = {};
			repm(i) {
				auto& proj = st.projs[i];
				if (st.turn + newProjTurns < endTurn) {
					gains[i] = max(0.0, newProjValue * scale - proj.usualValue);
				}
			}
			partial_sort(gains, gains + min(fireCount, m), gains + m, greater<double>());
			rep0(i, fireCount) {
				fireGain += gains[i];
			}
			scoreDelta += fireGain;
		}

		pdi sorted[MAX_M];
		repm(i) {
			sorted[i] = pdi(-projs[i].boostedValue, i);
		}
		rep(i, 1, m) {
			if (sorted[i - 1].first > sorted[i].first) {
				int j = i;
				auto tmp = sorted[i];
				do {
					sorted[j] = sorted[j - 1];
					j--;
				} while (j > 0 && sorted[j - 1].first > tmp.first);
				sorted[j] = tmp;
			}
		}

		repm(index) {
			int i = sorted[index].second;
			auto& proj = projs[i];
			double left = proj.left;
			double delta = proj.usualValue;
			if (left * invScale + st.turn >= endTurn)
				delta = 0;
			if (proj.boostedValue > delta) {
				double maxUsableMoney = left / moneyEfficiency * maxSkipRatio;
				double usableMoney = min(leftMoney, maxUsableMoney);
				double skipAmount = usableMoney * moneyEfficiency;
				double requiredTurns = (left - skipAmount) * invScale;
				if (requiredTurns + st.turn < endTurn && usableMoney > 0) {
					if (maxUsableMoney == usableMoney) {
						// full use
						leftMoney -= usableMoney;
						delta = proj.boostedValue;
					} else {
						// non-full use
						double moneyGainFullUse = decayByTurns(requiredTurns) * proj.value - usableMoney;
						if (moneyGainFullUse > delta) {
							leftMoney -= usableMoney;
							delta = moneyGainFullUse;
						}
					}
				}
			}
			scoreDelta += delta;
		}
		if (bombCount > 0) {
			repm(i) {
				auto& proj = projs[i];
				double oldValue = proj.usualValue;
				if (st.turn + newProjTurns < endTurn) {
					bombGain += newProjValue * scale - oldValue;
				}
			}
			scoreDelta = max(bombGain * (1 + (bombCount - 1) * 0.2), scoreDelta);
		}
		score += scoreDelta;

		// double potentialInvest = linearstep(200, 1000, st.money * invScale);
		// effectiveInvest = min(20.0, effectiveInvest + potentialInvest);
		double investGain = 4000 * (1 - progress) * (1 << effectiveInvest);
		score += investGain;

		// pool.restore();

		return score;
	}

	double evaluateFrom(State& st, int depth) {
		// 0, begin
		// --- turn=0
		// no choose
		// act
		// --- turn=0
		// choose, 0->1
		// act
		// ---
		//  :
		// --- turn=998
		// choose, 998->999
		// last act
		// --- turn=999
		// trivial choose, 999->1000
		// no act
		// ---
		// 1000, end

		assert(depth > 0);
		if (st.turn == 999) { // only trivial choose and no act
			updateUsualValues(st);
			return evaluate(st);
		}
		// update cands
		auto tmpCands = p.cands;
		pool.save();
		repk(i) {
			p.cands[i] = pool.nextCand(st.invest);
		}

		// evaluate all possible moves
		double maxScore = -INF;
		int used = st.used;
		int maxCands = min(k, 2);
		double scale = (double) (1 << st.invest);
		double invScale = 1 / scale;
		updateUsualValues(st);

		rep0(choice, maxCands) {
			const auto& cand = p.cands[choice];
			if (st.money < cand.cost)
				continue;
			if (used == -1 && choice > 0)
				break;
			Card tmpCard;
			if (used != -1) {
				tmpCard = st.cards[used];
				st.setCard(used, cand.toCard());
			}
			repn(ci) {
				bool duplicated = false;
				rep0(cj, ci) {
					if (st.cards[ci].similar(st.cards[cj], st.invest))
						duplicated = true;
				}
				if (duplicated)
					continue;
				const auto& card = st.cards[ci];
				if (card.kind == 0) {
					repm(pi) {
						double score = evaluateAfterMove(st, Move{choice, ci, pi}, depth - 1);
						maxScore = max(maxScore, score);
					}
				} else if (card.kind == 2) {
					pdi contribs[MAX_M] = {};
					repm(pi) {
						auto& proj = st.projs[pi];
						if (st.turn + newProjTurns < 1000) {
							double moneyGain = newProjValue * scale - proj.usualValue;
							if (moneyGain > 0) {
								contribs[pi] = pdi(-moneyGain, pi);
							}
						}
					}
					int see = min(3, m);
					partial_sort(contribs, contribs + see, contribs + m);
					rep0(i, see) {
						int pi = contribs[i].second;
						double score = evaluateAfterMove(st, Move{choice, ci, pi}, depth - 1);
						maxScore = max(maxScore, score);
					}
				} else {
					double score = evaluateAfterMove(st, Move{choice, ci, 0}, depth - 1);
					maxScore = max(maxScore, score);
				}
			}
			if (used != -1)
				st.setCard(used, tmpCard);
		}
		// restore state
		pool.restore();
		p.cands = tmpCands;
		return maxScore;
	}

	double evaluateAfterMove(State& st, Move move, int depth) {
		ll money = st.money;
		int turn = st.turn;
		int used = st.used;
		Card tmpUsed;
		if (st.used != -1) {
			int choice = move.cand();
			const auto& cand = p.cands[choice];
			tmpUsed = st.cards[used];
			st.setCard(used, cand.toCard());
			st.money -= cand.cost;
			st.used = -1;
		}
		st.turn++;
		assert(st.turn < 1000);
		pool.save();
		double score = 0;
		st.used = move.card();
		const auto& card = st.cards[move.card()];
		auto& proj = st.projs[move.proj()];
		switch (card.kind) {
		case 0: {
			auto tmpProj = proj;
			auto tmpMoney = st.money;
			if (proj.left <= card.work) {
				st.money += proj.value;
				proj = pool.nextProj(st.invest);
			} else {
				proj.left -= card.work;
			}
			updateUsualValue(proj);
			if (st.turn == 999) {
				score = st.money;
			} else if (depth == 0) {
				score = evaluate(st);
			} else {
				score = evaluateFrom(st, depth);
			}
			proj = tmpProj;
			st.money = tmpMoney;
		} break;
		case 1: {
			auto tmpProjs = st.projs;
			repm(pi) {
				auto& proj = st.projs[pi];
				if (proj.left <= card.work) {
					st.money += proj.value;
					proj = pool.nextProj(st.invest);
				} else {
					proj.left -= card.work;
				}
			}
			updateUsualValues(st);
			if (st.turn == 999) {
				score = st.money;
			} else if (depth == 0) {
				score = evaluate(st);
			} else {
				score = evaluateFrom(st, depth);
			}
			st.projs = tmpProjs;
		} break;
		case 2: {
			auto tmp = proj;
			proj = pool.nextProj(st.invest);
			updateUsualValue(proj);
			if (st.turn == 999) {
				score = st.money;
			} else if (depth == 0) {
				score = evaluate(st);
			} else {
				score = evaluateFrom(st, depth);
			}
			proj = tmp;
		} break;
		case 3: {
			auto tmpProjs = st.projs;
			repm(pi) {
				st.projs[pi] = pool.nextProj(st.invest);
			}
			updateUsualValues(st);
			if (st.turn == 999) {
				score = st.money;
			} else if (depth == 0) {
				score = evaluate(st);
			} else {
				score = evaluateFrom(st, depth);
			}
			st.projs = tmpProjs;
		} break;
		case 4: {
			auto tmpProjs = st.projs;
			st.invest++;
			updateUsualValues(st);
			if (st.turn == 999) {
				score = st.money;
			} else if (depth == 0) {
				score = evaluate(st);
			} else {
				score = evaluateFrom(st, depth);
			}
			st.invest--;
			st.projs = tmpProjs;
		} break;
		}
		pool.restore();
		st.money = money;
		st.turn = turn;
		st.used = used;
		if (used != -1)
			st.setCard(used, tmpUsed);
		return score;
	}

	pair<Move, double> determineNextMove() {
		if (verbose) {
			st.dump(n, m);
			repk(i) {
				trace("cand ", i, ": ", p.cands[i].kind);
			}
		}

		int possibleMoves[512];
		int numPossibleMoves = 0;

		// enumerate possible moves
		int skipped = 0;
		repk(choice) {
			const auto& cand = p.cands[choice];
			if (st.money < cand.cost)
				continue;
			if (st.used == -1) {
				if (choice > 0)
					break;
			} else {
				bool skip = false;
				repk(choice2) {
					if (choice != choice2 && p.cands[choice2].isBetterThan(p.cands[choice])) {
						skip = true;
						skipped++;
						break;
					}
				}
				if (skip)
					continue;
				double threshold = 50 * (double) (1 << st.invest);
				if (cand.kind == 4 && st.money - cand.cost < threshold)
					continue;
			}

			if (st.used != -1)
				st.cards[st.used] = cand.toCard();

			repn(ci) {
				bool duplicated = false;
				rep0(cj, ci) {
					if (st.cards[ci].similar(st.cards[cj], st.invest))
						duplicated = true;
				}
				if (duplicated)
					continue;
				const auto& card = st.cards[ci];
				if (card.kind == 0 || card.kind == 2) {
					repm(pi) {
						possibleMoves[numPossibleMoves++] = Move(choice, ci, pi).data;
					}
				} else {
					possibleMoves[numPossibleMoves++] = Move(choice, ci, 0).data;
				}
			}
		}

		st.updateSums(n);

		// evaluate moves
		double scoreSums[512] = {};

		double currentTime = pow(timer() / 2000.0, 1.0) * 2000.0;
		double meanTimePerPlayout = currentTime / (double) (totalPlayouts + 1);
		double leftTimePerTurn = (1950 - currentTime) / (double) (1000 - st.turn);
		double meanPlayoutsPerTurn = leftTimePerTurn / meanTimePerPlayout;

		bool isFirst = totalPlayouts == 0;
		int playouts = isFirst ? 100000 : max(1, (int) meanPlayoutsPerTurn);
		int playoutCount = 0;
		int stTime = timer();

		pdi sorted[512];
		bool excluded[512] = {};
		int count = 0;

		updateUsualValues(st);
		rep0(iter, playouts) {
			shufflePool(iter, 2);
			rep0(i, numPossibleMoves) {
				if (excluded[i])
					continue;
				count++;
				scoreSums[i] += evaluateAfterMove(st, Move(possibleMoves[i]), 1);
			}
			if (!isFirst) {
				rep0(i, numPossibleMoves) {
					sorted[i] = pdi(scoreSums[i], i);
				}
				sort(sorted, sorted + numPossibleMoves);
				rep0(i, numPossibleMoves * iter / playouts) {
					excluded[sorted[i].second] = true;
				}
			}
			playoutCount++;
			if (isFirst && timer() - stTime > 5)
				break;
		}
		totalPlayouts += playoutCount;

		if (st.turn % 10 == 0) {
			trace(st.turn, " ", count, " ", playoutCount);
		}

		// if (verbose) {
		// 	st.dump(n, m);
		// 	trace("cands");
		// 	repk(i) {
		// 		trace("cand ", i, ": ", p.cands[i].kind);
		// 	}
		// 	trace("mean scores at turn=", st.turn);
		// 	rep0(i, numPossibleMoves) {
		// 		auto move = Move(possibleMoves[i]);
		// 		trace("  choice=", move.cand(), " card=", move.card(), "(kind=",
		// 		    (move.card() == st.used ? p.cands[move.cand()].kind : st.cards[move.card()].kind), ")",
		// 		    " proj=", move.proj(), ": ", scoreSums[i] / playouts);
		// 	}
		// }

		Move bestMove;
		int bestIndex;
		double bestScore = -INF;
		rep0(i, numPossibleMoves) {
			if (scoreSums[i] > bestScore) {
				bestScore = scoreSums[i];
				bestMove = Move(possibleMoves[i]);
				bestIndex = i;
			}
		}
		// if (st.turn % 1 == 0) {
		// 	int rank1 = -1;
		// 	int rank2 = -1;
		// 	rep0(i, numPossibleMoves) {
		// 		if (sorted[i].second == bestIndex) {
		// 			rank1 = i;
		// 		}
		// 		if (sorted2[0][i].second == bestIndex) {
		// 			rank2 = i;
		// 		}
		// 	}
		// 	if (min(rank1, rank2) / (double) (numPossibleMoves - 1) > 0.5)
		// 		trace("t=", st.turn, " ", playoutCount, " ", numPossibleMoves, " ",
		// 		    min(rank1, rank2) / (double) (numPossibleMoves - 1));
		// }
		assert(bestScore != -INF);
		return make_pair(bestMove, bestScore / playouts);
	}

	void comment(const string& str) {
		if (isLocal) {
			out << "# " << str << endl;
		} else {
			cout << "# " << str << endl;
		}
	}

	// void print(ostream& out) const {
	// 	// print ans to out here
	// }
};

struct TestCase {
	int n;
	int m;
	int k;
	double nt;
	double mt;
	double kt;
	ll score1;
	ll score2;

	TestCase(int n, int m, int k, ll score1, ll score2) : n(n), m(m), k(k), score1(score1), score2(score2) {
		// nt = linearstep(2, 7, n);
		// mt = linearstep(2, 8, m);
		// kt = linearstep(2, 5, k);
		// nt = clamp(nt, 1e-3, 1 - 1e-3);
		// mt = clamp(mt, 1e-3, 1 - 1e-3);
		// kt = clamp(kt, 1e-3, 1 - 1e-3);
	}
};

void compareScores(string name1, string name2) {
	mov.beginMovie(name1 + " vs " + name2 + ".smv");
	name1 += ".txt";
	name2 += ".txt";
	ifstream in1(name1);
	ifstream in2(name2);
	constexpr int NUM_CASES = 2000;
	ll scores1[NUM_CASES] = {};
	ll scores2[NUM_CASES] = {};
	memset(scores1, -1, sizeof(scores1));
	memset(scores2, -1, sizeof(scores2));
	trace("loading ", name1);
	double sum1 = 0;
	double sum2 = 0;
	while (true) {
		int seed;
		ll score;
		in1 >> seed >> score;
		if (in1.eof())
			break;
		scores1[seed] = score;
		sum1 += log2(score);
	}
	trace("loading ", name2);
	while (true) {
		int seed;
		ll score;
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
		int n, m, k;
		in >> n >> m >> k;
		n -= 2;
		m -= 2;
		k -= 2;
		points.emplace_back(n, m, k, s1, s2);
	}
	trace(sum1, " ", sum2);
	auto drawTable = [&](string name, string xlabel, string ylabel, int xdiv, int ydiv, int frames,
	                     function<bool(int xi, int yi, int frame, const TestCase& t)> filter) {
		mov.target(name);
		rep0(frame, frames) {
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
						if (p.score1 >= p.score2)
							win1++;
						if (p.score1 <= p.score2)
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
	int ndivs = 6;
	int mdivs = 7;
	int kdivs = 4;
	// rep(divs, 2, 10 + 1) {
	drawTable("N-M plane div", "N lv", "M lv", ndivs, mdivs, kdivs + 1,
	    [&](int xi, int yi, int f, const TestCase& t) {
		    if (t.n != xi)
			    return false;
		    if (t.m != yi)
			    return false;
		    if (f != 0 && t.k != f - 1)
			    return false;
		    return true;
	    });
	drawTable("N-K plane div", "N lv", "K lv", ndivs, kdivs, mdivs + 1,
	    [&](int xi, int yi, int f, const TestCase& t) {
		    if (t.n != xi)
			    return false;
		    if (t.k != yi)
			    return false;
		    if (f != 0 && t.m != f - 1)
			    return false;
		    return true;
	    });
	drawTable("M-K plane div", "M lv", "K lv", mdivs, kdivs, ndivs + 1,
	    [&](int xi, int yi, int f, const TestCase& t) {
		    if (t.m != xi)
			    return false;
		    if (t.k != yi)
			    return false;
		    if (f != 0 && t.n != f - 1)
			    return false;
		    return true;
	    });
	// }
	mov.endMovie();
}

ll SCORES[] = {2590685, 95609, 1922115965, 342888278, 1436373, 135820458, 2995752506, 306909963, 2591613,
    10349114, 141311786, 1446590654, 344266, 99243, 49744286, 125971, 958669435, 169260, 265035, 5101156229,
    135065, 1181725726, 137631387, 18665788, 3261682, 2340061734, 775590601, 9519342, 200578857, 43012827,
    117152187, 3097802515, 1916691948, 16427039, 65135509, 472755922, 774632081, 2165107, 127642, 71440901,
    5396303265, 14796599, 2379693109, 1534347, 19251680, 4453428786, 67708639, 7562957813, 74136127, 246482,
    3362662792, 18092, 1761428, 381609145, 4245662, 18432697, 114576, 82890, 723068609, 160116, 51814603,
    204500263, 12199409, 4675707873, 23372, 87570784, 134057299, 6138916, 10179738, 2161433309, 126805244,
    1429849889, 24292698, 149045, 139238, 2926665, 55081082, 1326545, 34180600, 25519887, 103093452,
    538692815, 317637, 643040894, 73756, 23385213, 6948224, 13853468, 553721595, 11962364, 878788535,
    3347179025, 37152989, 161264, 6457914924, 386272770, 33197791, 324296297, 632392835, 15766853, 3956134,
    671600514, 502908781, 588121180, 48276524, 109457, 5578279433, 1324970606, 3845873635, 602252079, 154960,
    1496185, 4147416728, 50880951, 771046520, 3293271678, 249908, 2977384491, 800287628, 224695, 7314541097,
    7612660, 578852, 221429613, 142353, 96635441, 54459042, 146507, 319167, 5654144007, 23581994, 6239385,
    91398, 89226819, 4029406260, 6420490, 556627, 4125510, 14318422, 75800125, 414613011, 40141, 278470487,
    8692152, 21183598, 528856, 176043996, 1494483, 7229422141, 1640506910, 5899326439, 3350508, 507149544,
    4617150784, 7222948045, 553519, 12118352, 7644683, 2181869177, 152990386, 290239819, 332132724, 76789700,
    1424755714, 383897, 20829059, 249309738, 438454206, 18306524, 3139929135, 902919085, 1404429648,
    4360076233, 2451678720, 366831626, 5492521550, 7592023409, 434022554, 284736, 2517600253, 607105857,
    2308936125, 48973, 6913095, 404954, 16895634, 22004203, 6539433252, 1394640, 223770, 2769693456, 4199041,
    18009249, 91146, 132263, 4281861269, 111944, 87153, 43117102, 4342260125, 3905905206, 5627526, 373126515,
    759030, 264313934, 4845577534, 5836575, 6276739251, 430010, 42848994, 4645748, 3054305708, 164578,
    2534037836, 23607382, 14831906, 1778353144, 333988, 1878744713, 8994071, 3011467526, 51450710, 60235,
    3902382, 127432, 45423017, 907253920, 1734151516, 7382388448, 22787740, 25198724, 873051453, 80929438,
    115101, 9881861, 2969247, 1175002622, 434356495, 1501523783, 13104052, 167706, 58670, 4108115677,
    55558145, 409556034, 144876073, 90491, 1640765132, 3933133, 2787428102, 1004837144, 4183183, 211514338,
    2882344, 2411912, 8292622, 5595370, 100405241, 18928104, 480498, 176617690, 148214, 5891456, 115498345,
    280297, 202987, 78715497, 187693266, 1772210, 7398332, 1924923, 93682592, 4564745, 6073793167, 272219,
    874339, 2911963, 965655, 402986533, 111923, 170505, 97821763, 914883700, 5477768618, 96530406, 431031343,
    18554920, 2927764859, 8018394, 12091765, 6099980699, 265569, 43522950, 140192602, 1142079, 3055701416,
    2967669809, 201870, 119275480, 888123, 1916853324, 2628829897, 138711, 4026314, 70933, 31538154, 59889,
    1281517, 2024363276, 388559, 190152046, 608588, 1576992338, 9071278, 5538636020, 13244, 6487915, 66414,
    607545563, 30904185, 5159827465, 665105743, 6356461, 2000554409, 159878587, 545880122, 93243172, 90243783,
    1404577620, 2169922699, 321090, 16382354, 2979736898, 4543224, 157464137, 30671158, 310177858, 2837272047,
    239226, 7059530, 682536, 145060, 206140428, 1172530264, 593779581, 499209, 249951435, 71150860, 604041693,
    2008665, 121935, 96762, 4907091258, 2789172171, 33152, 375409044, 151000, 1105770342, 134948, 6239103653,
    1354408766, 1207522, 158554, 2011607179, 131336110, 66251081, 2002741158, 28619712, 2967588, 14029544452,
    220094964, 474045, 824471708, 4857632608, 89973, 2369823795, 5404011516, 320041778, 43406499, 320524,
    212253, 450386, 179239, 4095013, 344503, 3348893, 656346, 122794, 4358900, 11743045, 7854973, 686540703,
    3159572032, 267223050, 76925758, 5227063, 769603803, 1756623, 68574044, 403982, 30064704, 235001, 495579,
    97495146, 184021274, 73655000, 181421, 25559272, 135595, 10444235, 4863559017, 132617, 1659918064, 95493,
    5491056349, 319548, 27704109, 4711244, 1462800942, 5732, 21782206, 141051, 18458222, 47443714, 4348000896,
    565496, 5431853489, 84346695, 16034978, 4406759038, 360036, 1542663901, 35953, 229355161, 24931274,
    226333706, 15158406, 189776, 260603, 3777325544, 248504018, 31892, 3940878109, 16534380, 184673,
    1748919642, 184192, 137394, 31748, 504219, 365141059, 381783, 6058870520, 71641, 95021, 272196, 245324,
    196776336, 379944944, 19295240, 1734729366, 186758890, 1764019488, 529364, 14404184, 875777555,
    3086119165, 83049746, 944952909, 1768073131, 649878, 249806245, 8708406, 251986781, 510763, 245924,
    3675633642, 266042, 795655071, 1505192, 900795530, 132645, 90272, 397899704, 2573476058, 532098463,
    2938216145, 264303149, 8866711, 1496819294, 102898399, 25601112, 3621391339, 1702602, 592421708, 235944,
    68716196, 2135406041, 1284911848, 592370088, 333514632, 18602755, 61265447, 390557832, 646545836,
    1146745433, 3127416493, 1794951657, 18080161, 512024417, 1597206536, 10685577, 183136336, 991275005,
    291526, 4752068, 159357, 7971460718, 134570, 60340, 22105034, 296826, 4688584, 13904412, 332159, 748991,
    6183817106, 2178912918, 102682, 1142836682, 19703, 3037616607, 98932, 1248684548, 16777052, 82208,
    65452456, 2133263542, 8736245, 76050873, 1664385027, 3486043834, 70915551, 54296045, 740360, 7628359,
    13929929, 6176791, 3901867505, 207884, 4966583302, 37970961, 10025515, 4154421663, 545907148, 597953,
    59396939, 434535, 204052, 26894293, 394611216, 1198513927, 79274432, 14285336, 26218786, 257562, 4635839,
    2096857068, 3422029, 124075882, 109699701, 96084, 385554, 159653016, 105605288, 171175, 62693, 4274060680,
    263987, 346054, 48278810, 609198, 11637494, 384354, 222886, 747725011, 1027600097, 546245408, 3116900,
    14027606, 61746609, 353892, 2360891488, 1186787796, 5684906377, 461886352, 40902644, 289103, 96818,
    2636988956, 428330, 2842932471, 1956588424, 51858, 109867, 348628, 976663987, 691959775, 150147,
    126349176, 603968202, 116856214, 93298, 2714188904, 314396, 124444, 72990315, 318060, 280583, 247577,
    19384471, 14694001, 146860, 5152257, 3119073684, 213881077, 130012160, 1142564818, 131972, 23336961,
    5070788, 533635839, 165691783, 3742571411, 197481, 34163, 3343138, 61627945, 2725727, 131408221, 11529233,
    2651681483, 161724, 4008404111, 297821329, 7301914, 2066521878, 1082890202, 1945473200, 79428264,
    38069325, 36740, 544094, 36623006, 2850227, 828879531, 198645, 147232995, 93356, 70134759, 12414295,
    6123824, 893287, 4427358636, 117829, 1667271826, 672946562, 123870079, 2555146371, 86287924, 254159,
    7784259, 1296505106, 21603129, 4251901911, 1457903, 44827, 163669697, 64992, 324768600, 104240910,
    1052919, 212684151, 24919551, 7382552301, 3417086, 1457271, 7648228, 8300133, 1198988, 517870, 211370,
    2639112944, 39123591, 2034546997, 253771568, 9857083801, 694841860, 6311074, 6888557, 13844374, 290099,
    153179, 418657601, 7447488, 94642, 47875, 3389143744, 90703, 15885, 118741189, 126311400, 2342286,
    1698149369, 133079, 90430, 55064, 3281644, 207722, 169531, 865162392, 75333105, 309556, 653011761,
    52093894, 83790, 352178944, 2002791100, 174822, 112452, 21417884, 859028, 373982, 19410545, 416621972,
    283633508, 3684758636, 6125239610, 564607022, 7380173, 181724198, 313460284, 329341, 20256516, 29280,
    846167, 409074, 318467974, 33965575, 11162443, 6397411144, 3476548, 5988065, 10737591, 690112, 419878090,
    179173006, 482325, 1359375123, 10098477835, 719358738, 215689, 6304441478, 8759043, 1351354177, 550785,
    229334855, 596809848, 11021017, 4448066, 65404976, 199768, 37222, 139725, 287395830, 7492472, 279255,
    8936445854, 46045459, 4276527, 3922355649, 1984898599, 1614383, 42021130, 3020600119, 848285228, 33898029,
    215528147, 1991188080, 3922580167, 6958310565, 20646340, 548708, 5173026947, 20592, 3691134, 31604764,
    1984695045, 1031222269, 2445942, 41935551, 3233922363, 307238, 17639281, 162352, 97916019, 360556563,
    11743861, 2815044288, 12383, 91732, 61877661, 3708777254, 210491616, 2019770447, 257883305, 5003139507,
    179044, 444230257, 271084, 110126, 76375790, 3541904, 4367704144, 943703, 3689720370, 937342037,
    1066625235, 40018308, 135908, 566223, 42006676, 3987680, 624935, 7324260, 1618079763, 785468521, 20322096,
    64019, 417196, 5047856, 2247204852, 29144132, 1848226998, 8376748, 132600, 1342897035, 39037676,
    395892978, 5835662, 284212832, 125417, 12218877, 249062, 23029418, 3723580, 4936510940, 6271583582,
    776409506, 214410, 4534391956, 31237649, 1142997, 2885629, 384290644, 5747296, 20347991, 6367132,
    3219594337, 1050199275, 96827292, 105620, 9696230, 26417451, 259842, 1007880598, 13768530, 2733017,
    180235452, 1317255630, 15558620, 964354, 30033416, 6879683, 9993284, 97785, 20187547, 4995297360, 441812,
    2256945, 75569623, 328307, 1555386, 245602088, 817668, 6443279, 594552, 167254494, 112664, 752919,
    11987463, 107187649, 375470, 795387099, 3440299, 488133555, 117837695, 3383063, 377126809, 205158,
    25132855, 2226862964, 545900, 2347214592, 6033709095, 14709258, 3251552595, 1446806150, 24181483, 66297,
    51197225, 3667365, 1786349054, 9588066, 398037, 6178815, 68329196, 378366572, 5889843964, 1016990097,
    4182548, 120432, 1460458597, 4212283147, 218815481, 10975736376, 408642, 3641568188, 123895482,
    3600658246, 647915369, 1217319107, 420306, 349373, 185359972, 12599852, 18106047, 3275083393, 23930229,
    93047, 1438107, 1645594467, 9058462, 103403, 2903067171, 271317316, 79084349, 6248082756, 7354702922,
    3128091942, 97101932, 1268693059, 29735829, 161983586, 329594, 2518103484, 81421329, 97358, 601855185,
    2510352763, 35669, 63657, 2454318812, 126989353, 484082, 309972855, 2322625794, 38136299, 5243181, 181816,
    1217325636, 1117220, 35077481, 411993, 73035976, 237225, 2104603, 204839, 35196613, 76574, 243512, 105573,
    12946153, 1988622707, 1666176, 1951730013, 1971500679, 2058211274, 1164448205, 365955859, 2030792120,
    8260851, 303995093, 2329299, 141685319, 7728526385, 713743, 602085, 1189713, 13446518, 21785560, 84947,
    23903016, 2837937782, 22749028, 5930411167, 1110178545, 2492562255, 35445109, 22073601, 2024049340,
    1128497780, 3738292917, 22531636, 222693, 98972910, 2782233359, 6380008524, 98358, 320328334, 90746,
    19250661, 495387, 1630160426, 478174081, 59081794, 388978, 4793586368, 1564155021, 689791, 47336,
    4321667233, 2878870, 6433308797, 502652614, 72690278, 1677586, 155885, 1219253, 78313205, 65081, 94398,
    1304588122, 285250, 305825559, 322069612, 464815556, 274872601, 2460720, 2722636131, 1419371, 1247901,
    866746, 347014371, 88894232, 138638, 533688, 18685783, 8424807, 1550270729, 36022604, 2256795226,
    2832596818, 738326, 1414653810, 9073710, 380855, 1331776, 307172398, 4076339, 628392535, 5339380082,
    56393908, 925042, 1056236353, 226654, 42284425, 3783912, 219606615, 74761995, 393220564, 870763, 43875247,
    179487427, 14273949, 332917070, 401603, 3948548, 59899860, 43554884, 243845, 5250512, 1486248, 13622561,
    1315538032, 224418, 5588271465, 2987474444, 490533, 154325453, 227124, 59424871, 1855778361, 5940, 33660,
    7423564, 235166121, 1937777, 164718, 80997008, 176961127, 23676448, 3732677066, 72500, 110006, 2607612970,
    152941, 18105089, 168755, 195961746, 428569243, 130932, 45942167, 16549068, 95752, 1438298, 60312108,
    3456156260, 54128846, 265402, 34344246, 1653432, 5286870, 125372039, 2692032, 1267342523, 184679760,
    865411, 143179, 242474, 45579928, 10296832, 225558, 12940, 71072877, 339118, 1552650419, 1793775827,
    3495977361, 56044, 132364, 14501170, 39399, 1565275, 3372244, 140713, 51265, 12252646, 433535, 90629,
    9846042, 854333398, 2515421950, 2335711, 72876, 4721653, 3773276, 518124, 4564529670, 2571348353,
    3810721730, 16715143, 11755246, 1078687, 630563761, 207730, 1748541, 3759887893, 1532231731, 222907508,
    723117525, 3259211841, 48293020, 30391615, 49157270, 6714891140, 60061667, 409271, 85552, 2067198119,
    413643, 588288, 3904990967, 3460444720, 4897513576, 213085748, 992069, 6550828, 94822783, 3677191333,
    3156745578, 3661001, 1424063838, 532081809, 265151, 140644, 69962, 6920047, 31167414, 25275200,
    3515009820, 189340, 2493753723, 29942576, 3620758069, 2416040, 343756286, 23008998, 3094620684, 121840324,
    54484, 5546856980, 2244456347, 764954581, 371295, 4404122858, 134898, 17618511, 15863427, 86559,
    2172498294, 3017958595, 68824, 2515631335, 5653447, 85717, 157094, 271403, 61483, 75595997, 2781258014,
    132557, 10815, 78641, 499428, 12083678, 8969044, 526159, 88652, 1312102822, 1734408763, 915822781,
    1044302888, 44569596, 360535463, 286471, 1471449853, 46451, 55023, 80908796, 3103642, 11178473,
    5412691791, 59391588, 1559019, 299078, 2752506238, 1623289248, 3772746, 2536071, 11453851, 137028,
    68137581, 5398485297, 340601259, 348067930, 3495497899, 2578195871, 2637397, 908327457, 158791908,
    70049824, 247672, 194906, 576228511, 2030663340, 7006078506, 1115895545, 552156302, 2969529, 10619,
    1742652, 14571162, 1948785, 700476805, 363946334, 201900, 330518, 131728, 130278, 1509615840, 181433,
    122341, 649855609, 4386023, 44944139, 305984, 3519751598, 1756287130, 483703, 20801805, 740581295,
    109211749, 54417673, 57865676, 337701256, 7342661, 1346456839, 563812424, 3137017845, 2422634717, 2818460,
    299688389, 7902691, 38880, 277217, 47179912, 295509, 71614090, 5305584265, 659204, 97556543, 176918624,
    4030215673, 1234554523, 18942452, 660410121, 43231, 196589656, 617919886, 456139690, 318580320, 1379484,
    885387263, 4765326597, 41357, 103509, 2110251, 141727, 921084091, 206570, 6211380137, 3763264, 1552498795,
    929623006, 925346762, 319474, 6710425, 27193209, 3144408, 61904419, 2732082832, 249844, 1056458478, 14459,
    1854213792, 6459740397, 115630361, 346239047, 36431243, 756035, 1368122482, 3818904942, 66442, 68281978,
    10170937, 3593594, 851807007, 116409, 4950655, 268449, 459569922, 25044001, 715589, 321993706, 18702159,
    32426, 4530752667, 249669, 99710698, 104648073, 542201, 157563, 12084516, 35088, 39887, 28624773,
    700026201, 404870037, 2036797731, 1875418218, 795664154, 1205288353, 45446, 266196101, 3909181533,
    17499235, 2568699494, 1192785, 3896726, 3259572, 2061357493, 1279896328, 23663629, 645182137, 183391,
    1039363336, 1283428, 1728103585, 501496, 3486279910, 297852, 6985148, 8594209, 36484369, 6927082,
    2224921862, 488324, 954715, 28695586, 2488900704, 144002, 2726952, 1138860, 5212598284, 100402, 123606133,
    33946850, 228809129, 4711143, 377262343, 2171155595, 2816379683, 761240, 1424093, 111636893, 794102,
    4005324, 189625, 871566, 844475, 2184120, 1165967855, 3581579665, 59107203, 3457449154, 1685130, 20965,
    671165, 2732085262, 83735703, 367296, 2065223, 60244357, 596677688, 5301161, 884305, 1819707, 2926044467,
    2691030734, 462104550, 313353631, 28607352, 233110, 119573, 17632915, 358861956, 365719508, 2494885112,
    178171166, 1043526813, 284622, 931867, 180568, 6971498733, 16524107, 869374065, 2479280, 12545771, 710421,
    4649631983, 688061224, 134763740, 184057, 813894, 1484756, 1717732, 542524126, 5909675353, 3939802,
    2064456, 563413, 47827303, 485088, 518071, 62210, 26232225, 149898, 4640110900, 406041353, 1179064,
    1689354532, 448897104, 4914807139, 461530311, 115623596, 209560, 133629840, 466821736, 162871798,
    40047500, 9935863, 229884372, 291411547, 881264990, 20061361, 47288033, 537363930, 193269, 6827293,
    811456, 74717, 646374, 21684, 4956351005, 391448, 26640604, 1649942941, 27199200, 711145308, 95291769,
    163034490, 5403700, 4634152143, 62143691, 485438, 3837900528, 63618532, 18093804, 1138682462, 567076,
    196419, 1744483, 84475737, 52201124, 723507787, 8314693544, 4211213, 359026986, 92352, 3373527805,
    1042393765, 446435545, 3464047798, 4335980059, 41422261, 2648953207, 3497976413, 52777, 1881895557,
    640072194, 1147466408, 5798494, 498893312, 1475666, 229150, 2613711772, 3618527, 30711172, 131972,
    4673644, 4681810, 444687014, 5623581, 5472370996, 513642959, 1450952506, 96142574, 744850, 600730119,
    55937368, 1215913675, 5217546, 1873670363, 132404679, 20345, 383691275, 42948926, 4177456198, 88889,
    851535, 387388, 9771844, 252879, 8192208, 234894111, 92156, 91186628, 444410083, 10465286, 6981955,
    12996427, 7237181236, 4710928097, 1255185584, 516256658, 5966691, 4454174623, 55510603, 3108024810,
    649255800, 60356, 278591254, 1029745, 22787864, 380882, 60289821, 10310270, 176044573, 8774450,
    4511741492, 4463110, 1300896812, 872309360, 418320, 75605, 528553412, 11113582, 111847004, 21727074,
    23489757, 550333923, 79109247, 2186152, 960928215, 3194398, 1636801214, 24929, 32798069, 11865195,
    108458369, 4372050, 2560824885, 15089071, 25443357, 5091503636, 2585374756, 101523700, 124499, 839480793,
    228499, 6647682, 20052209, 245038, 190208706, 5438847, 1933714734, 261849475, 442778, 1557676639, 73911,
    95226516, 279929716, 24555892, 12447047, 891807, 4213579294, 72847477, 6671450436, 711906544, 136255,
    480162, 31411601, 1424045708, 67113237, 39131907, 317282, 21792353, 8964232, 475745, 481816, 505540,
    1695675, 1967249, 1161804953, 80336, 234647, 699609, 98841, 5989457708, 2238352488, 9643680, 4238367301,
    633369911, 3490258746, 50298, 6061553, 30836247, 3910596, 3827684, 220806, 51352893, 3960708, 2448698186,
    24172666, 82168620, 14459675, 2986798140, 160743, 34577, 32927415, 124966514, 36996191, 212528, 302465,
    304944646, 766033612, 4058976, 21595216, 16433630, 686681219, 14863412, 828353401, 2369987411, 1039290,
    186542, 4577158, 1171103436, 2630952264, 4557148, 1197524892, 6592253617, 32363428, 25122432, 3290624,
    222636, 2517569297, 2380869718, 394810, 528491, 10710053, 212723, 401414277, 4899845562, 1949066,
    21548023, 796172648, 3197465329, 1691513938, 2450337294, 140513, 3444966185, 43852057, 12171100,
    598267484, 1617765684, 26849370, 185614, 39929, 4786277, 249395, 4059842, 430451100, 1058191, 2018847321,
    16761786, 57276807, 257005, 3883368101, 412827335, 66453084, 263981, 1695370, 13527923, 97586293, 1897957,
    148200, 175392451, 109121, 97302790, 167805425, 1186318009, 6410946613, 14076905, 2646111887, 4962275,
    570057022, 176544, 1471077802, 597131184, 5966490, 1589053613, 12766508, 23558, 229250, 1450529, 20342903,
    1591213937, 17193764, 1898312973, 1334563397, 52907174, 26604771, 964172, 57747, 93449, 1997344,
    550354317, 596213, 93790, 4300240, 14141549, 939599168, 191160, 3397714749, 1838252063, 949135650,
    10762071, 3210211, 431650925, 944123070, 32761921, 5537400, 361976881, 2317033478, 142880, 146336, 56407,
    786216, 9889096, 490952, 2525763871, 13572278, 412767036, 9281931841, 5616927636, 2264880, 280137,
    254758160, 36158917, 1404054972, 2026874699, 244462933, 311415, 92185295, 380733, 132470, 5682004461,
    34068990, 136610, 3529947779, 186156, 16006385, 12609209, 32771558, 3301264011, 3185871, 508584341,
    765242459, 334258, 72349770, 22143807, 2026640, 224573, 6048066, 258353, 2310088921, 624078, 1893331,
    28880, 33175711, 1447098686, 197068, 28841315, 56889452, 127032275, 2254649232, 2411217879, 88323725,
    2293027, 105171222, 164591723, 1954959, 2714576504, 539328618, 592903006, 276234, 34343735, 32258278,
    1653078512, 6345696, 233798259, 1942518330, 125992, 336153, 30838959, 3815310784, 164151226, 5619481,
    19297107, 419956133, 179769, 2281583441, 206716424, 151656, 4222004024, 66363, 4833037307, 53785732,
    27275, 81616173, 291643, 331625734, 186502411, 243886, 876389420, 108457, 3735381, 146582286, 653646,
    21977825, 11827904, 3622796707, 74847262, 1447417212, 21023673, 36908, 134402, 72440974, 9028647,
    1345359217, 1609372455, 133307, 144620, 2240382, 1663285881, 2552851, 3874389314, 4466239290, 4384519347,
    35233302, 237363, 15127140, 1348062746, 20100480, 35944177, 22212895, 2778917598, 1436185776, 105716882,
    19766230, 6320235, 53628064, 462796577, 573662, 119715992, 4539714452, 433320995, 546099023, 1472156890,
    190087, 239439347, 507900427, 80764256, 493990, 853874055, 15311261, 2850314187, 23999231, 12332754,
    3686373217, 3050259242, 26081980, 538513, 100972, 3293853, 13680918, 3582430748, 831836793, 428795453};
constexpr int NUM_SCORES = sizeof(SCORES) / sizeof(ll);

int main() {
#if 0 || ONLINE_JUDGE
	isLocal = false;
	timer(true);
	Solver sol;
	sol.load(cin);
	sol.solve();
#elif 0
	constexpr int NUM_CASES = 2000;
	ll maxScores[NUM_CASES] = {};
	for (string name : {"clip0", "clip50", "clip100", "clipk15", "cut"}) {
		ifstream in1(name + ".txt");
		while (true) {
			int seed;
			ll score;
			in1 >> seed >> score;
			if (in1.eof())
				break;
			maxScores[seed] = max(maxScores[seed], score);
		}
	}
	tracei(maxScores, maxScores + NUM_CASES);
#elif 0
	compareScores("cut", "nocut");
	// compareScores("new6", "new10");
#elif 1
	debug = false;
	int seed;
	cin >> seed;
	cin >> timeScale;

	// bool readParams = false;
	// Param param;
	// if (readParams) {
	// 	array<double, NUM_PARAMS> params;
	// 	rep0(i, NUM_PARAMS) {
	// 		cin >> params[i];
	// 	}
	// 	param.load(params);
	// }
	timer(true);
	Solver sol;
	// if (readParams)
	// 	sol.param = param;
	sol.load(seed);
	sol.solve();
	ll score = sol.res.score;
	cout << score << " " << (seed < NUM_SCORES ? score / (double) SCORES[seed] : 0.0) << endl;
#elif 1
	timeScale = 1.0;
	int num = 10;
	int from = 0;
	int stride = 1;
	int single = -1;
	// debug = false;
	render = false;

	int seeds[1000] = {};
	int times[1000] = {};
	ll scores[1000] = {};

	ll scoreSum = 0;
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
	debug = true;

	auto print = [&](int i) {
		int s = seeds[i];
		string space = s < 10 ? "   " : s < 100 ? "  " : s < 1000 ? " " : "";
		double relScore = s < NUM_SCORES ? scores[i] / (double) SCORES[s] : -1;
		trace("  seed ", space, s, ": ", scores[i], " (rel=", relScore, ")", " (time ", times[i], " ms)");
	};

	if (single == -1) {
		trace("--- summary ---");
		array<ll, 2> timesRank[1000] = {};
		array<ll, 2> scoresRank[1000] = {};
		rep0(t, num) {
			timesRank[t] = {times[t], t};
			scoresRank[t] = {scores[t], t};
		}
		sort(timesRank, timesRank + num);
		sort(scoresRank, scoresRank + num);
		trace("seed:");
		rep0r(i, num) {
			if (scores[i] == -1)
				continue;
			print(i);
		}
		trace("score:");
		rep0r(t, num) {
			int i = scoresRank[t][1];
			if (scores[i] == -1)
				continue;
			print(i);
		}
		double numValid = num - numInvalid;
		trace("total score: ", scoreSum, ", mean: ", (ll) (scoreSum / numValid * 100) / 100.0,
		    ", mean(log2): ", scoreLogSum / numValid);
		if (numInvalid > 0) {
			trace(numInvalid, " invalid cases");
		}
	} else {
		print(0);
	}
#endif
}
