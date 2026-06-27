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

// #define NDEBUG
#undef NDEBUG
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

bool isLocal = false;
constexpr bool USE_TL = true;
constexpr int ITER = USE_TL ? 10000 : 1;
constexpr int TL = USE_TL ? 1850 : 10000000;
double timeScale = 1.0;

// define N and stuff here
constexpr int N = 30;
constexpr double INF = 1e18;
#define rep2() rep0(i, N) rep0(j, i + 1)

constexpr int makeIJ(int i, int j) {
	return i << 16 | j;
}

constexpr void getIJ(int ij, int& i, int& j) {
	i = ij >> 16;
	j = ij & 0xffff;
}

template <class T>
class ArrayNN {
private:
	T data[N * N] = {};

public:
	ArrayNN() {
	}

	T& at(int i, int j) {
		return data[i * N + j];
	}

	const T& at(int i, int j) const {
		return data[i * N + j];
	}
};

class State {
public:
	State* prev = nullptr;
	int prevCandIndex = 0;
	int nextIJ = 0;
	int totalSwap = 0;
	double totalDist = 0;
	ArrayNN<int> bs;
	ArrayNN<double> dists;
	ArrayNN<int> distsInt;
	ArrayNN<int> prevs;
	vector<int> cands;

	State() {
	}

	void transition(int candIndex, State& out) {
		out.prev = this;
		out.prevCandIndex = candIndex;
		out.bs = bs;

		int ij = cands.at(candIndex);
		int dstI, dstJ;
		getIJ(ij, dstI, dstJ);

		vector<int> path;
		path.push_back(ij);
		while (ij != nextIJ) {
			int i, j;
			getIJ(ij, i, j);
			ij = prevs.at(i, j);
			path.push_back(ij);
		}
		reverse(path.begin(), path.end());

		int numSwaps = len(path) - 1;
		rep0(i, numSwaps) {
			int a = path[i];
			int b = path[i + 1];
			int ai, aj, bi, bj;
			getIJ(a, ai, aj);
			getIJ(b, bi, bj);
			out.swap(ai, aj, bi, bj);
		}

		out.totalDist = totalDist + dists.at(dstI, dstJ);
		out.totalSwap = totalSwap + numSwaps;
	}

	void getCandInfo(int candIndex, double& outTotalDist, int& outTotalSwap) {
		int ij = cands.at(candIndex);
		int dstI, dstJ;
		getIJ(ij, dstI, dstJ);
		outTotalDist = totalDist + dists.at(dstI, dstJ);
		outTotalSwap = totalSwap + distsInt.at(dstI, dstJ);
	}

	// the smaller the better
	double evaluateCand(int candIndex) {
		int i, j;
		getIJ(nextIJ, i, j);
		int nextNum = bs.at(i, j);
		double dist;
		int swap;
		getCandInfo(candIndex, dist, swap);
		double res = mix(dist * 1e-3, (double) swap, nextNum / 464.0);
		// trace("num: ", nextNum, " ", res);
		return res;
	}

	vector<array<int, 4>> getSwaps(int candIndex, bool reversed) {
		vector<array<int, 4>> res;
		int ij = cands[candIndex];
		int dstI, dstJ;
		getIJ(ij, dstI, dstJ);

		vector<int> path;
		path.push_back(ij);
		while (ij != nextIJ) {
			int i, j;
			getIJ(ij, i, j);
			ij = prevs.at(i, j);
			path.push_back(ij);
		}
		if (!reversed)
			reverse(path.begin(), path.end());

		int numSwaps = len(path) - 1;
		rep0(i, numSwaps) {
			int a = path[i];
			int b = path[i + 1];
			int ai, aj, bi, bj;
			getIJ(a, ai, aj);
			getIJ(b, bi, bj);
			res.push_back({ai, aj, bi, bj});
		}
		return res;
	}

private:
	void swap(int i1, int j1, int i2, int j2) {
		std::swap(bs.at(i1, j1), bs.at(i2, j2));
	}
};

constexpr int MAX_STATES = 50000;
State* states;
int numStatesUsed = 0;

State* requestState() {
	if (numStatesUsed >= MAX_STATES)
		trace("STATES OVERFLOWED!!!");
	return states + numStatesUsed++;
}

class Problem {
public:
	ArrayNN<int> bs;

	Problem() {
	}

	void load(istream& in) {
		rep2() {
			in >> bs.at(i, j);
		}
	}
};

class BS {
public:
	vector<State*> entries;

	BS() {
	}

	void init(State* initial) {
		entries.clear();
		entries.push_back(initial);
	}

	void next(int width) {
		vector<State*> newEntries;

		priority_queue<pair<double, pii>, vector<pair<double, pii>>, greater<pair<double, pii>>> q;
		int numEntries = len(entries);
		rep0(ei, numEntries) {
			auto s = entries[ei];
			int numCands = len(s->cands);
			rep0(ci, numCands) {
				double cost = s->evaluateCand(ci);
				q.emplace(cost, pii(ei, ci));
			}
		}
		while (!q.empty() && len(newEntries) < width) {
			auto top = q.top();
			q.pop();
			State* from = entries[top.second.first];
			int candIndex = top.second.second;
			State* newState = requestState();
			from->transition(candIndex, *newState);
			newEntries.push_back(newState);
		}
		entries = newEntries;
	}
};

class Solver {
public:
	Problem p;
	Rnd rnd;
	vector<array<int, 4>> sol;
	BS bs;
	int seed = -1;
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

	void solve() {
		auto initial = requestState();
		initial->bs = p.bs;
		initial->totalDist = 0;
		initial->totalSwap = 0;
		bs.init(initial);
		rep0(i, 465) {
			for (auto s : bs.entries) {
				dijkstra(*s, i);
			}
			bs.next(107);
			// solveFor(i);
		}

		// int num = 0;
		// while (true) {
		// 	bool end = true;
		// 	rep2() {
		// 		int b = bs.at(i, j);
		// 		if (b != num)
		// 			continue;
		// 		if (i > 0) {
		// 			int sw1 = bs.at(i - 1, j) - b;
		// 			int sw2 = j == 0 ? -1 : bs.at(i - 1, j - 1) - b;
		// 			if (sw1 > 0 || sw2 > 0) {
		// 				if (sw1 > sw2) {
		// 					bs.swap(i, j, i - 1, j);
		// 					sol.push_back({i, j, i - 1, j});
		// 					end = false;
		// 				} else {
		// 					bs.swap(i, j, i - 1, j - 1);
		// 					sol.push_back({i, j, i - 1, j - 1});
		// 					end = false;
		// 				}
		// 			}
		// 		}
		// 	}
		// 	if (end) {
		// 		num++;
		// 		if (num == 465)
		// 			break;
		// 		end = false;
		// 	}
		// }
		buildSwaps(bs.entries[0]);
		print(cout);
	}

private:
	void buildSwaps(State* finalState) {
		sol.clear();
		auto st = finalState;
		while (st->prev != nullptr) {
			int candIndex = st->prevCandIndex;
			st = st->prev;
			for (auto& s : st->getSwaps(candIndex, true)) {
				sol.push_back(s);
				// trace("swap: ", s);
			}
		}
		reverse(sol.begin(), sol.end());
	}

	// void solveFor(int num) {
	// 	trace("solving for ", num, "...");
	// 	dijkstra(*state, num);

	// 	auto nextState = states + num + 1;
	// 	state->transition(0, *nextState);

	// 	trace("dist: ", nextState->totalDist);
	// 	trace("swaps: ", nextState->totalSwap);

	// 	state = nextState;

	// 	// dijkstra(num);
	// }

	void dijkstra(State& state, int from) {
		auto& dists = state.dists;
		auto& distsInt = state.distsInt;
		auto& prevs = state.prevs;
		static bool fixed[N][N] = {};
		rep2() {
			dists.at(i, j) = INF;
			distsInt.at(i, j) = 1000000;
			prevs.at(i, j) = makeIJ(-1, -1);
			fixed[i][j] = false;
		}
		priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> q;
		rep2() {
			if (state.bs.at(i, j) == from) {
				state.nextIJ = makeIJ(i, j);
				q.emplace(0, state.nextIJ);
				dists.at(i, j) = 0;
				distsInt.at(i, j) = 0;
			}
		}
		while (!q.empty()) {
			auto top = q.top();
			q.pop();
			int ij = top.second;
			int i, j;
			getIJ(ij, i, j);
			if (fixed[i][j])
				continue;
			fixed[i][j] = true;
			int distInt = distsInt.at(i, j);
			double dist = top.first;
			int count = 0;

			if (i > 0) {
				int i2 = i - 1;
				rep0(flip, 2) {
					int j2 = j - flip;
					if (j2 < 0 || j2 > i2)
						continue;
					int b2 = state.bs.at(i2, j2);
					if (from < b2) {
						count++;
						double cost = pow(465 - b2, 2.0);
						double d = dist + cost;
						if (d < dists.at(i2, j2)) {
							dists.at(i2, j2) = d;
							distsInt.at(i2, j2) = distInt + 1;
							prevs.at(i2, j2) = ij;
							q.emplace(d, makeIJ(i2, j2));
						}
					}
				}
			}
			if (count == 0) {
				state.cands.push_back(ij);
			}
		}
	}

	void print(ostream& out) {
		trace(len(sol));
		out << len(sol) << endl;
		for (auto& s : sol) {
			out << s[0] << " " << s[1] << " " << s[2] << " " << s[3] << endl;
		}
	}

	void init() {
	}
};

int main() {
	states = (State*) malloc(sizeof(State) * MAX_STATES);
#if 1
	isLocal = false;
	timer(true);
	Solver sol;
	sol.load(cin);
	sol.solve();
#elif 1
	isLocal = true;
	timer(true);
	Solver sol;
	sol.load(0);
	sol.solve();
#endif
}
