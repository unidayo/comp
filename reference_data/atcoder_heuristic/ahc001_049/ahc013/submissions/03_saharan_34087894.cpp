#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <complex>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <thread>
#include <mutex>

//#define NDEBUG
#undef NDEBUG
#include <cassert>

using namespace std;
using ull = unsigned long long;
using ll = long long;
using pii = pair<int, int>;
using pdi = pair<double, int>;
#define len(c) (int) (c.size())
#define rep(i, from, until) for (int i = (from); i < (until); i++)
#define repr(i, from, until) for (int i = (until) - 1; i >= (from); i--)
#define rep0(i, until) rep(i, 0, until)
#define rep0r(i, until) repr(i, 0, until)
#define clamp(a, minimum, maximum) min(max(a, (minimum)), (maximum))

#ifdef _MSC_VER
#include <intrin.h>
#define popcount32(a) ((int) __popcnt(a))
#define popcount64(a) ((int) __popcnt64(a))
#define clz32(a) ((int) __lzcnt(a))
#define clz64(a) ((int) __lzcnt64(a))
inline int ctz32(int a) { int i; _BitScanForward((unsigned long*) &i, a); return i; }
inline int ctz64(ull a) { int i; _BitScanForward64((unsigned long*) &i, a); return i; }
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

	void seed(int y) {
		this->y = y;
	}

	int next() {
		y = y ^ (y << 13); y = y ^ (y >> 17);
		return (y = y ^ (y << 5)) & 0x7fffffff;
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

void tracen() {
}

template <class Head, class... Tail>
void tracen(Head&& head, Tail&&... tail) {
	if (!debug) return;
	cerr << head;
	tracen(forward<Tail>(tail)...);
}

template <class... Tail>
void trace(Tail&&... tail) {
	if (!debug) return;
	tracen(forward<Tail>(tail)...);
	cerr << endl;
}

template <class	Iter>
void tracein(Iter from, Iter until) {
	if (!debug) return;
	bool first = true;
	while (from != until) {
		if (first) {
			first = false;
		} else {
			cerr << ", ";
		}
		cerr << *from;
		from++;
	}
}

template <class	Iter>
void tracei(Iter from, Iter until) {
	if (!debug) return;
	tracein(from, until);
	cerr << endl;
}

void sleepUntil(int until) {
	volatile unsigned int a = 0;
	while (timer() < until) a++;
}

void segv() {
	volatile int* a = nullptr;
	*a = 0;
}

void waitForever() {
	while (true) getchar();
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

namespace svg {
	using namespace std;

	string s(double x) {
		return to_string(x);
	}

	string rgb(double r, double g, double b) {
		return "rgb(" + s(lround(r * 255)) + "," + s(lround(g * 255)) + "," + s(lround(b * 255)) + ")";
	}

	string p(string name, double val) {
		return name + "=\"" + s(val) + "\" ";
	}

	string p(string name, string val) {
		return name + "=\"" + val + "\" ";
	}

	string tag(string name, string props) {
		return "<" + name + " " + props + "/>";
	}

	string tag(string name, string props, string content) {
		return "<" + name + " " + props + ">" + content + "</" + name + ">";
	}

	class Style {
	public:
		double sr, sg, sb, sa; // stroke
		double fr, fg, fb, fa; // fill
		double e00, e01, e02, e10, e11, e12; // affine matrix
		int textAlign;
		double fs;
		double sw;
		string tf;

		Style() : sr(0), sg(0), sb(0), sa(1), fr(1), fg(1), fb(1), fa(1), e00(1), e01(0), e02(0), e10(0), e11(1), e12(0), textAlign(0), fs(16), sw(1), tf() {
		}

		string stroke() const {
			return sa <= 0 ? "" : p("stroke", rgb(sr, sg, sb)) + p("stroke-opacity", sa) + p("stroke-width", sw);
		}

		string fill() const {
			return fa <= 0 ? p("fill", "none") : p("fill", rgb(fr, fg, fb)) + p("fill-opacity", fa);
		}

		string textAnchor() const {
			return p("text-anchor", textAlign < 0 ? "left" : textAlign > 0 ? "right" : "middle");
		}

		string transform() const {
			return tf.empty() ? "" : p("transform", tf);
		}

		string font() const {
			return p("font-size", fs);
		}
	};

	class Graphics {
	public:
		double screenW;
		double screenH;
		string data;

		Graphics() : screenW(100), screenH(100) {
		}

		void screen(double width, double height) {
			screenW = width;
			screenH = height;
		}

		void clear() {
			data = "";
			st = Style();
		}

		void save() {
			stack.push_back(st);
		}

		void restore() {
			st = stack.back();
			stack.pop_back();
		}

		void stroke(double r, double g, double b, double a = 1) {
			st.sr = r;
			st.sg = g;
			st.sb = b;
			st.sa = a;
		}

		void noStroke() {
			stroke(0, 0, 0, 0);
		}

		void strokeWidth(double w) {
			st.sw = w;
		}

		void fontSize(double size) {
			st.fs = size;
		}

		void fill(double r, double g, double b, double a = 1) {
			st.fr = r;
			st.fg = g;
			st.fb = b;
			st.fa = a;
		}

		void noFill() {
			fill(0, 0, 0, 0);
		}

		void translate(double tx, double ty) {
			st.tf = "translate(" + s(tx) + " " + s(ty) + ")" + st.tf;
		}

		void rotate(double rad, double x = 0, double y = 0) {
			st.tf = "rotate(" + s(rad) + " " + s(x) + " " + s(y) + ")" + st.tf;
		}

		void scale(double sx, double sy) {
			st.tf = "scale(" + s(sx) + " " + s(sy) + ")" + st.tf;
		}

		void line(double x1, double y1, double x2, double y2) {
			data += tag("line", p("x1", x1) + p("y1", y1) + p("x2", x2) + p("y2", y2) + st.stroke() + st.transform()) + "\n";
		}

		void rect(double x, double y, double w, double h) {
			data += tag("rect", p("x", x) + p("y", y) + p("width", w) + p("height", h) + st.stroke() + st.fill() + st.transform()) + "\n";
		}

		void circle(double x, double y, double r) {
			data += tag("circle", p("cx", x) + p("cy", y) + p("r", r) + st.stroke() + st.fill() + st.transform()) + "\n";
		}

		void text(string str, double x, double y) {
			data += tag("text", st.textAnchor() + p("x", x) + p("y", y) + st.font() + st.fill() + st.transform(), str) + "\n";
		}

		string dumpSvg(string id = "", string style = "") const {
			string res;
			res += "<svg ";
			if (id != "") res += p("id", id);
			if (style != "") res += p("style", style);
			res += p("viewBox", "-1 -1 " + s(screenW + 2) + " " + s(screenH + 2)) + p("xmlns", "http://www.w3.org/2000/svg");
			res += ">\n" + data + "</svg>";
			return res;
		}

	private:
		Style st;
		vector<Style> stack;
	};

	class Movie {
	public:
		vector<string> svgs;

		Movie() {
		}

		void clear() {
			svgs.clear();
		}

		void addFrame(const Graphics& g) {
			svgs.push_back(g.dumpSvg("f" + to_string(svgs.size()), "display:none;pointer-events:none;user-select:none;"));
		}

		string dumpHtml() {
			ostringstream oss;
			int num = (int) svgs.size();
			oss << "<html><body><div id=\"text\">loading...</div>" << endl;
			oss << "<div style=\"display:flex;\"><input type=\"button\" value=\"prev\" style=\"height:32px;\" id=\"prev\"><input type=\"button\" value=\"&#x25b6;\" style=\"width:60px;height:32px;\" id=\"play\"><input type=\"button\" value=\"next\" style=\"height:32px;margin-right:4px;\" id=\"next\"><label>slow<input type=\"range\" min=\"2\" max=\"60\" step=\"1\" value=\"10\" id=\"fps\">fast</label></div>" << endl;
			oss << "<input type=\"range\" min=\"1\" max=\"" << num << "\" step=\"1\" id=\"bar\" style=\"width:500px\"></div>" << endl;
			for (int i = 0; i < num; i++) {
				string& svg = svgs[i];
				oss << svg << endl;
			}

			oss << "<script>\nlet numFrames = " << num << ";";
			oss << R"(
				let text = document.getElementById("text");
				let toggle = document.getElementById("play");
				let bar = document.getElementById("bar");
				let fps = document.getElementById("fps");
				let frames = [];
				for (let i = 0; i < numFrames; i++) {
					let f = document.getElementById("f" + i);
					frames.push(f);
					f.style.display = "none";
				}
				let currentFrame = 0;
				let playing = false;
				let id = 0;
				function play() {
					if (playing) return;
					if (currentFrame == numFrames - 1) setFrame(0);
					playing = true;
					toggle.value = "\u25a0";
					id++;
					setTimeout(() => frame(id), 1000 / fps.value);
				}
				function stop() {
					if (!playing) return;
					id++;
					playing = false;
					toggle.value = "\u25b6";
				}
				toggle.onclick = () => {
					if (playing) stop();
					else play();
				}
				bar.oninput = () => {
					if (currentFrame != bar.value - 1) setFrame(bar.value - 1);
				}
				document.getElementById("prev").onclick = () => { stop(); currentFrame > 0 && setFrame(currentFrame - 1); }
				document.getElementById("next").onclick = () => { stop(); nextFrame(); }
				function setFrame(at) {
					frames[currentFrame].style.display = "none";
					frames[at].style.display = null;
					text.innerText = (at + 1) + " / " + numFrames;
					bar.value = at + 1;
					currentFrame = at;
				}
				function nextFrame() {
					if (currentFrame == numFrames - 1) return;
					setFrame(currentFrame + 1);
					if (currentFrame == numFrames - 1) stop();
				}
				function frame(i) {
					if (i != id) return;
					nextFrame();
					setTimeout(() => frame(i), 1000 / fps.value);
				}
				play();
			)";
			oss << "</script></body></html>" << endl;
			return oss.str();
		}
	};
}

using Graphics = svg::Graphics;
using Movie = svg::Movie;

struct SolverResult {
	int score = 0;
};

constexpr int MAX_N = 48;
constexpr int MAX_K = 5;
constexpr int MAX_COMP = 100 * MAX_K;

constexpr int DIRS[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
constexpr int DIR_L = 0;
constexpr int DIR_R = 1;
constexpr int DIR_U = 2;
constexpr int DIR_D = 3;

constexpr int INF = 100000000;

#define isinf DO_NOT_USE_THIS
#define isfinite DO_NOT_USE_THIS

constexpr bool isInf(int a) {
	return a >= INF / 2;
}

constexpr bool isInf(double a) {
	return a >= INF / 2;
}

constexpr bool isFinite(int a) {
	return !isInf(a);
}

constexpr bool isFinite(double a) {
	return !isInf(a);
}

constexpr int flipDir(int dir) {
	return dir ^ 1;
}

constexpr int perpDir1(int dir) {
	return dir ^ 2;
}

constexpr int perpDir2(int dir) {
	return dir ^ 3;
}

constexpr bool isHorizontal(int dir) {
	return (dir & 2) == 0;
}

constexpr bool isVertical(int dir) {
	return !isHorizontal(dir);
}

constexpr int dirFromDiffs(int di, int dj) {
	assert((di == 0) ^ (dj == 0));
	return dj < 0 ? DIR_L : dj > 0 ? DIR_R : di < 0 ? DIR_U : DIR_D;
}

#define rep2() rep0(i,n)rep0(j,n)
#define repC() rep0(i,100*k)
#define repCNamed(i) rep0(i,100*k)
#define inBounds(i,j) ((i)>=0&&(i)<n&&(j)>=0&&(j)<n)
#define defineDirDiffs(dir,di,dj) int di=DIRS[dir][0],dj=DIRS[dir][1]
#define assignDirDiffs(dir,di,dj) di=DIRS[dir][0],dj=DIRS[dir][1]

class Problem {
public:
	int n = 0;
	int k = 0;
	int data[MAX_N][MAX_N] = {};
	pii poss[MAX_K][100] = {};

	Problem() {
	}

	void read(istream& in) {
		in >> n >> k;
		int counts[MAX_K] = {};
		rep2() {
			char c;
			in >> c;
			int kind = c - '0';
			data[i][j] = kind;
			if (kind > 0) {
				poss[kind - 1][counts[kind - 1]++] = pii(i, j);
			}
		}
		rep0(i, k) {
			assert(counts[i] == 100);
		}
	}

	void print() {
		trace("N=", n, " K=", k);
		rep0(i, n) {
			string s;
			rep0(j, n) {
				s += data[i][j] == 0 ? "." : to_string(data[i][j] - 1);
			}
			trace(s);
		}
	}
};


class Computer {
public:
	int index = 0;
	int kind = 0;
	int i = 0;
	int j = 0;
	int origI = 0;
	int origJ = 0;
	int conns[4] = { -1, -1, -1, -1 }; // for each dir
	int clusterIndex = -1;
	bool fixed = false;
	int load = 0;

	Computer() {
	}

	int dist() {
		return abs(i - origI) + abs(j - origJ);
	}

	bool canMoveHorizontally() {
		return conns[DIR_U] == -1 && conns[DIR_D] == -1;
	}

	bool canMoveVertically() {
		return conns[DIR_L] == -1 && conns[DIR_R] == -1;
	}

	bool canMoveTo(int dir) {
		return conns[perpDir1(dir)] == -1 && conns[perpDir2(dir)] == -1;
	}

	bool isLonely() {
		return (conns[0] == -1) && (conns[1] == -1) && (conns[2] == -1) && (conns[3] == -1);
	}
};

// computer or cable
class Cell {
public:
	int index1 = -1;
	int index2 = -1;

	Cell() {
	}

	void setAsComputer(int i) {
		index1 = i;
		index2 = -1;
	}

	void setAsCable(int i1, int i2) {
		index1 = min(i1, i2);
		index2 = max(i1, i2);
	}

	void clear() {
		index1 = -1;
		index2 = -1;
	}

	static Cell computer(int i) {
		Cell res;
		res.setAsComputer(i);
		return res;
	}

	static Cell cable(int i1, int i2) {
		Cell res;
		res.setAsCable(i1, i2);
		return res;
	}

	static Cell empty() {
		Cell res;
		res.clear();
		return res;
	}

	bool isEmpty() const {
		return index1 == -1 && index2 == -1;
	}

	bool isComputer() const {
		return index1 != -1 && index2 == -1;
	}

	bool isComputerOf(int index) const {
		return isComputer() && getComputerIndex() == index;
	}

	bool isCable() const {
		return index1 != -1 && index2 != -1;
	}

	bool isCableOf(int i1, int i2) const {
		return index1 == i1 && index2 == i2 || index1 == i2 && index2 == i1;
	}

	bool isCableHorizontal(const Computer cs[]) {
		assert(isCable());
		const Computer& c1 = cs[index1];
		const Computer& c2 = cs[index2];
		assert((c1.i == c2.i) ^ (c1.j == c2.j));
		return c1.i == c2.i;
	}

	bool isCableVertical(const Computer cs[]) {
		return !isCableHorizontal(cs);
	}

	int getComputerIndex() const {
		assert(isComputer());
		return index1;
	}

	pii getCableIndices() const {
		assert(isCable());
		return pii(index1, index2);
	}
};

enum class OperationType {
	UnsafeConnect, UnsafeDisconnect, UnsafeMove, CellUpdate
};

class Operation {
public:
	OperationType type = {};
	int data[3] = {};

	Operation() {
	}

	void getConnect(int& i1, int& i2, int& d1, int& d2) const {
		assert(type == OperationType::UnsafeConnect);
		i1 = data[0] >> 16;
		i2 = data[0] & 0xffff;
		d1 = data[1] >> 8;
		d2 = data[1] & 0xff;
	}

	void getDisconnect(int& i1, int& i2, int& d1, int& d2) const {
		assert(type == OperationType::UnsafeDisconnect);
		i1 = data[0] >> 16;
		i2 = data[0] & 0xffff;
		d1 = data[1] >> 8;
		d2 = data[1] & 0xff;
	}

	void getUnsafeMove(int& i, int& di, int& dj) const {
		assert(type == OperationType::UnsafeMove);
		i = data[0];
		di = data[1];
		dj = data[2];
	}

	void getCellUpdate(int& i, int& j, int& dindex1, int& dindex2) const {
		assert(type == OperationType::CellUpdate);
		i = data[0] >> 8;
		j = data[0] & 0xff;
		dindex1 = data[1];
		dindex2 = data[2];
	}

	string toString() const {
		if (type == OperationType::UnsafeConnect) {
			int i1;
			int i2;
			int d1;
			int d2;
			getConnect(i1, i2, d1, d2);
			return "[op connect " + to_string(i1) + "[" + to_string(d1) + "] - " + to_string(i2) + "[" + to_string(d2) + "]]";
		} else if (type == OperationType::UnsafeDisconnect) {
			int i1;
			int i2;
			int d1;
			int d2;
			getDisconnect(i1, i2, d1, d2);
			return "[op disconnect " + to_string(i1) + "[" + to_string(d1) + "] - " + to_string(i2) + "[" + to_string(d2) + "]]";
		} else if (type == OperationType::UnsafeMove) {
			int i;
			int di;
			int dj;
			getUnsafeMove(i, di, dj);
			return "[op move " + to_string(i) + ".pos += (" + to_string(di) + ", " + to_string(dj) + ")]";
		} else if (type == OperationType::CellUpdate) {
			int i;
			int j;
			int dindex1;
			int dindex2;
			getCellUpdate(i, j, dindex1, dindex2);
			return "[op cell (" + to_string(i) + ", " + to_string(j) + ").indices += (" + to_string(dindex1) + ", " + to_string(dindex2) + ")]";
		} else {
			assert(false);
			return "[op error]";
		}
	}

	static Operation connect(int i1, int i2, int d1, int d2) {
		Operation res;
		res.type = OperationType::UnsafeConnect;
		res.data[0] = i1 << 16 | i2;
		res.data[1] = d1 << 8 | d2;
		return res;
	}

	static Operation disconnect(int i1, int i2, int d1, int d2) {
		Operation res;
		res.type = OperationType::UnsafeDisconnect;
		res.data[0] = i1 << 16 | i2;
		res.data[1] = d1 << 8 | d2;
		return res;
	}

	static Operation unsafeMove(int i, int di, int dj) {
		Operation res;
		res.type = OperationType::UnsafeMove;
		res.data[0] = i;
		res.data[1] = di;
		res.data[2] = dj;
		return res;
	}

	static Operation cellUpdate(int i, int j, int dindex1, int dindex2) {
		Operation res;
		res.type = OperationType::CellUpdate;
		res.data[0] = i << 8 | j;
		res.data[1] = dindex1;
		res.data[2] = dindex2;
		return res;
	}
};

class Cluster {
public:
	int index = -1;
	vector<int> indices;
	int counts[MAX_K] = {};
	int kind = -1;
	int size = 0;

	Cluster() {
	}
};


enum class PushMode {
	OnlyCost, Try, Confirm
};

Graphics g;
Movie mov;

#if _DEBUG
bool drawMovieReq = false;
bool writeMovieVerbose = false;
#else
bool drawMovieReq = true;
bool writeMovieVerbose = false;
#endif

bool solvePosVerbose = false;
bool saveMovieWithSeed = true;
constexpr bool timeout = true;
int maxTrial = timeout ? 1000 : 3;

bool drawMovie = drawMovieReq && !timeout;

constexpr int TIMEOUT_LIMIT = timeout ? 2950 : 10000000;

class Diff {
public:
	int historyPoint = 0;
	vector<Operation> ops;

	Diff() {
	}
};

int globalCount = 0;

class Room {
public:
	int n = 0;
	int k = 0;
	int numConnections = 0;
	int totalDistance = 0;
	int totalCableLengths[MAX_K] = {};
	Computer cs[MAX_COMP];
	Cell cells[MAX_N][MAX_N];
	bool freeCells[MAX_N][MAX_N] = {};
	Cluster cls[MAX_COMP];
	int numClusters = 0;

	vector<Operation> history;

	Room() {
	}

	void setup(Problem& p) {
		history.clear();
		n = p.n;
		k = p.k;
		totalDistance = 0;
		numClusters = 0;
		numConnections = 0;
		int index = 0;
		rep0(i, k) {
			rep0(j, 100) {
				Computer& c = cs[index];
				c.kind = i;
				c.index = index;
				c.clusterIndex = -1;
				rep0(d, 4) c.conns[d] = -1;
				c.i = p.poss[i][j].first;
				c.j = p.poss[i][j].second;
				c.origI = c.i;
				c.origJ = c.j;
				index++;
			}
		}
		rep2() {
			cells[i][j].clear();
		}
		repC() {
			Computer& c = cs[i];
			cells[c.i][c.j].setAsComputer(i);
		}
	}

	void makeDiff(Diff& diff, int fromHistoryPoint) const {
		diff.historyPoint = fromHistoryPoint;
		auto& ops = diff.ops;
		ops.clear();
		ops.insert(ops.end(), history.begin() + fromHistoryPoint, history.end());
	}

	void applyDiff(const Diff& diff) {
		assert(currentHistoryPoint() == diff.historyPoint);
		for (auto& op : diff.ops) {
			doAndAddToHistory(op);
		}
	}

	int currentHistoryPoint() {
		return len(history);
	}

	void discardHistory() {
		history.clear();
	}

	void addFrame(string msg = "", bool onlyComputers = false) {
		if (drawMovie) {
			multiset<pii> ch;
			g.clear();
			draw(g, onlyComputers, ch);
			g.screenH += 12;
			g.fontSize(10);
			g.fill(0, 0, 0);
			g.text(msg, g.screenW * 0.5, g.screenH - 2);
			mov.addFrame(g);
			if (writeMovieVerbose) {
				writeMovie();
			}
		}
	}

	void writeMovie(string name = "movie.html") {
		if (drawMovie) {
			writeText(name, mov.dumpHtml());
		}
	}

	void clearMovie() {
		if (drawMovie) {
			mov.clear();
		}
	}

private:
	void undo(Operation& op) {
		if (op.type == OperationType::UnsafeConnect) {
			int i1;
			int i2;
			int d1;
			int d2;
			op.getConnect(i1, i2, d1, d2);
			cs[i1].conns[d1] = -1;
			cs[i2].conns[d2] = -1;
			numConnections--;
		} else if (op.type == OperationType::UnsafeDisconnect) {
			int i1;
			int i2;
			int d1;
			int d2;
			op.getDisconnect(i1, i2, d1, d2);
			cs[i1].conns[d1] = i2;
			cs[i2].conns[d2] = i1;
			numConnections++;
		} else if (op.type == OperationType::UnsafeMove) {
			int i;
			int di;
			int dj;
			op.getUnsafeMove(i, di, dj);
			auto& c = cs[i];
			totalDistance -= c.dist();
			c.i -= di;
			c.j -= dj;
			totalDistance += c.dist();
		} else if (op.type == OperationType::CellUpdate) {
			int i;
			int j;
			int dindex1;
			int dindex2;
			op.getCellUpdate(i, j, dindex1, dindex2);
			auto& cell = cells[i][j];
			if (cell.isCable()) {
				pii cable = cell.getCableIndices();
				totalCableLengths[cs[cable.first].kind]--;
				totalCableLengths[cs[cable.second].kind]--;
			}
			cell.index1 -= dindex1;
			cell.index2 -= dindex2;
			if (cell.isCable()) {
				pii cable = cell.getCableIndices();
				totalCableLengths[cs[cable.first].kind]++;
				totalCableLengths[cs[cable.second].kind]++;
			}
		} else {
			assert(false);
		}
	}

	void doAndAddToHistory(const Operation& op) {
		globalCount++;

		//constexpr int CRITICAL = 1043325;
		//if (globalCount == CRITICAL - 200) {
		//	drawMovie = true;
		//	mov.clear();
		//}
		//if (globalCount >= CRITICAL - 200) {
		//	addFrame(op.toString());
		//}
		//if (globalCount == CRITICAL) {
		//	writeMovie();
		//}

		if (op.type == OperationType::UnsafeConnect) {
			int i1;
			int i2;
			int d1;
			int d2;
			op.getConnect(i1, i2, d1, d2);
			cs[i1].conns[d1] = i2;
			cs[i2].conns[d2] = i1;
			numConnections++;
		} else if (op.type == OperationType::UnsafeDisconnect) {
			int i1;
			int i2;
			int d1;
			int d2;
			op.getDisconnect(i1, i2, d1, d2);
			cs[i1].conns[d1] = -1;
			cs[i2].conns[d2] = -1;
			numConnections--;
		} else if (op.type == OperationType::UnsafeMove) {
			int i;
			int di;
			int dj;
			op.getUnsafeMove(i, di, dj);
			auto& c = cs[i];
			totalDistance -= c.dist();
			c.i += di;
			c.j += dj;
			totalDistance += c.dist();
		} else if (op.type == OperationType::CellUpdate) {
			int i;
			int j;
			int dindex1;
			int dindex2;
			op.getCellUpdate(i, j, dindex1, dindex2);
			auto& cell = cells[i][j];
			if (cell.isCable()) {
				pii cable = cell.getCableIndices();
				totalCableLengths[cs[cable.first].kind]--;
				totalCableLengths[cs[cable.second].kind]--;
			}
			cell.index1 += dindex1;
			cell.index2 += dindex2;
			if (cell.isCable()) {
				pii cable = cell.getCableIndices();
				totalCableLengths[cs[cable.first].kind]++;
				totalCableLengths[cs[cable.second].kind]++;
			}
		} else {
			assert(false);
		}
		history.push_back(op);
	}
public:

	void undoTo(int historyPoint) {
		while (history.size() > historyPoint) {
			undo(history.back());
			history.pop_back();
		}
	}

	void computeClusters() {
		bool visited[MAX_COMP] = {};
		numClusters = 0;
		repC() {
			auto& c = cs[i];
			if (visited[i]) continue;
			auto& cl = cls[numClusters];
			cl.index = numClusters;
			cl.indices.clear();
			rep0(k1, k) {
				cl.counts[k1] = 0;
			}
			clusterDfs(visited, cl, numClusters, i);
			if (len(cl.indices) == 1) {
				c.clusterIndex = -1;
				continue;
			}
			numClusters++; // add the cluster
			cl.kind = -1;
			cl.size = 0;
			rep0(k1, k) {
				int count = cl.counts[k1];
				if (count > cl.size) {
					cl.size = count;
					cl.kind = k1;
				}
			}
		}
	}

	int computeScore() {
		computeClusters();
		int res = 0;
		rep0(i, numClusters) {
			auto& cl = cls[i];
			int* counts = cl.counts;
			rep0(k1, k) {
				res += counts[k1] * (counts[k1] - 1) >> 1;
				rep(k2, k1 + 1, k) {
					res -= counts[k1] * counts[k2];
				}
			}
		}
		return res;
	}

	int explodeSmallerThan(int size) {
		computeClusters();
		int res = 0;
		rep0(i, numClusters) {
			auto& cl = cls[i];
			if (cl.size < size) {
				for (int index : cl.indices) {
					isolate(index);
				}
			}
		}
		return res;
	}

	void analyze() {
		computeClusters();
		int res = 0;
		trace("clusters:");
		rep0(i, numClusters) {
			auto& cl = cls[i];
			int plus = 0;
			int minus = 0;
			int* counts = cl.counts;
			rep0(k1, k) {
				plus += counts[k1] * (counts[k1] - 1) >> 1;
				rep(k2, k1 + 1, k) {
					minus += counts[k1] * counts[k2];
				}
			}
			tracen("  ");
			tracein(counts, counts + k);
			trace(" (", plus, " - ", minus, " = ", plus - minus, ")");
			res += plus - minus;
		}
		trace("total distance: ", totalDistance);
		trace("total score: ", res);
	}

private:
	void clusterDfs(bool visited[], Cluster& cl, int clusterIndex, int i) {
		visited[i] = true;
		cl.indices.push_back(i);
		auto& c = cs[i];
		if (clusterIndex != -1) {
			c.clusterIndex = clusterIndex;
		}
		cl.counts[c.kind]++;
		rep0(d, 4) {
			int i2 = c.conns[d];
			if (i2 != -1 && !visited[i2]) {
				clusterDfs(visited, cl, clusterIndex, i2);
			}
		}
	}
public:

	int computeMaxComputersInConnectedArea(int kind) {
		bool visited[MAX_N][MAX_N] = {};
		bool wall[MAX_N][MAX_N] = {};
		repC() {
			auto& c = cs[i];
			if (c.kind == kind) continue;
			if (c.conns[DIR_D] != -1) {
				int index2 = c.conns[DIR_D];
				int j = c.j;
				int i1 = c.i;
				int i2 = cs[index2].i;
				if (i1 > i2) swap(i1, i2);
				rep(i, i1, i2 + 1) {
					wall[i][j] = true;
				}
			}
			if (c.conns[DIR_R] != -1) {
				int index2 = c.conns[DIR_R];
				int i = c.i;
				int j1 = c.j;
				int j2 = cs[index2].j;
				if (j1 > j2) swap(j1, j2);
				rep(j, j1, j2 + 1) {
					wall[i][j] = true;
				}
			}
		}
		int maxCount = 0;
		rep2() {
			if (wall[i][j] || visited[i][j]) continue;
			int counts[MAX_K] = {};
			collectIndicesDfs(i, j, counts, visited, wall);
			if (counts[kind] > maxCount) {
				maxCount = counts[kind];
			}
		}
		return maxCount;
	}

private:
	void collectIndicesDfs(int i, int j, int counts[MAX_K], bool visited[MAX_N][MAX_N], bool wall[MAX_N][MAX_N]) {
		visited[i][j] = true;
		if (cells[i][j].isComputer()) {
			counts[cs[cells[i][j].getComputerIndex()].kind]++;
		}
		rep0(d, 4) {
			int di = DIRS[d][0];
			int dj = DIRS[d][1];
			int i2 = i + di;
			int j2 = j + dj;
			if (inBounds(i2, j2) && !wall[i2][j2] && !visited[i2][j2]) {
				collectIndicesDfs(i2, j2, counts, visited, wall);
			}
		}
	}
public:

	int distBetween(int i1, int i2) {
		auto& c1 = cs[i1];
		auto& c2 = cs[i2];
		return abs(c1.i - c2.i) + abs(c1.j - c2.j);
	}

	bool isConnected(int i1, int i2) {
		bool visited[MAX_COMP] = {};
		Cluster cl;
		clusterDfs(visited, cl, -1, i1);
		return count(cl.indices.begin(), cl.indices.end(), i2) != 0;
	}

	bool isDirectlyConnected(int i1, int i2) {
		auto& c1 = cs[i1];
		auto& c2 = cs[i2];
		rep0(d, 4) {
			if (c1.conns[d] == i2 && c2.conns[flipDir(d)] == i1) return true;
		}
		return false;
	}

	void isolate(int i) {
		rep0(d, 4) {
			int i2 = cs[i].conns[d];
			if (i2 != -1) disconnect(i, i2);
		}
	}

	void connect(int i1, int i2) {
		auto& c1 = cs[i1];
		auto& c2 = cs[i2];
		assert((c1.i == c2.i) ^ (c1.j == c2.j));
		if (c1.i == c2.i) {
			// horizontal
			auto& a = c1.j < c2.j ? c1 : c2;
			auto& b = c1.j > c2.j ? c1 : c2;
			assert(a.conns[DIR_R] == -1);
			assert(b.conns[DIR_L] == -1);
			unsafeConnect(a.index, b.index, DIR_R, DIR_L);
			int i = a.i;
			rep(j, a.j + 1, b.j) {
				assert(cells[i][j].isEmpty());
				cellUpdate(i, j, Cell::cable(a.index, b.index));
			}
		} else {
			// vertical
			auto& a = c1.i < c2.i ? c1 : c2;
			auto& b = c1.i > c2.i ? c1 : c2;
			assert(a.conns[DIR_D] == -1);
			assert(b.conns[DIR_U] == -1);
			unsafeConnect(a.index, b.index, DIR_D, DIR_U);
			int j = a.j;
			rep(i, a.i + 1, b.i) {
				assert(cells[i][j].isEmpty());
				cellUpdate(i, j, Cell::cable(a.index, b.index));
			}
		}
	}

	void disconnect(int i1, int i2) {
		auto& c1 = cs[i1];
		auto& c2 = cs[i2];
		assert((c1.i == c2.i) ^ (c1.j == c2.j));
		if (c1.i == c2.i) {
			// horizontal
			auto& a = c1.j < c2.j ? c1 : c2;
			auto& b = c1.j > c2.j ? c1 : c2;
			assert(a.conns[DIR_R] == b.index);
			assert(b.conns[DIR_L] == a.index);
			unsafeDisconnect(a.index, b.index, DIR_R, DIR_L);
			int i = a.i;
			rep(j, a.j + 1, b.j) {
				assert(cells[i][j].isCableOf(a.index, b.index));
				cellUpdate(i, j, Cell::empty());
			}
		} else {
			// vertical
			auto& a = c1.i < c2.i ? c1 : c2;
			auto& b = c1.i > c2.i ? c1 : c2;
			assert(a.conns[DIR_D] == b.index);
			assert(b.conns[DIR_U] == a.index);
			unsafeDisconnect(a.index, b.index, DIR_D, DIR_U);
			int j = a.j;
			rep(i, a.i + 1, b.i) {
				assert(cells[i][j].isCableOf(a.index, b.index));
				cellUpdate(i, j, Cell::empty());
			}
		}
	}

	void sanityCheck() {
		repC() {
			auto& c = cs[i];
			assert(cells[c.i][c.j].getComputerIndex() == i);
		}
		rep2() {
			if (cells[i][j].isComputer()) {
				auto& c = cs[cells[i][j].getComputerIndex()];
				assert(i == c.i && j == c.j);
			}
		}
	}

private:
	vector<Operation> currentOps;
	vector<Operation> bestOps;
	int pushStartPoint = 0;
	int minPushDist = 0;
	int currentDir = 0;
public:

	// note that the cost can be negative
	int multiPush(int index, int dir, int amount, bool exactDistance, PushMode mode) {
		assert(amount >= 0);
		if (amount == 0) return 0;
		if (amount == 1 && !exactDistance) return push(index, dir, mode);
		auto& c = cs[index];
		defineDirDiffs(dir, di, dj);
		int toI = c.i + amount * di;
		int toJ = c.j + amount * dj;
		int hp = currentHistoryPoint();
		int totalCost = 0;
		rep0(iter, amount) {
			totalCost += push(index, dir, PushMode::Try);
			if (isInf(totalCost)) {
				undoTo(hp);
				if (mode == PushMode::Confirm) assert(false);
				return INF;
			}
			if (exactDistance && c.i == toI && c.j == toJ) break;
		}
		if (exactDistance && (c.i != toI || c.j != toJ)) {
			undoTo(hp);
			if (mode == PushMode::Confirm) assert(false);
			return INF;
		}
		if (mode == PushMode::OnlyCost) {
			undoTo(hp);
		}
		return totalCost;
	}

	// note that the cost can be negative
	int push(int index, int dir, PushMode mode) {
		currentOps.clear();
		bestOps.clear();
		minPushDist = INF;
		auto& c = cs[index];
		if (!c.canMoveTo(dir)) {
			if (mode == PushMode::Confirm) assert(false);
			return INF;
		}
		int i = c.i;
		int j = c.j;
		defineDirDiffs(dir, di, dj);
		i += di;
		j += dj;
		if (!inBounds(i, j)) {
			if (mode == PushMode::Confirm) assert(false);
			return INF;
		}
		currentDir = dir;
		pushStartPoint = currentHistoryPoint();

		int connBehind = c.conns[flipDir(dir)];
		if (connBehind == -1) cellUpdate(c.i, c.j, Cell::empty());
		else cellUpdate(c.i, c.j, Cell::cable(c.index, connBehind));

		pushDfs(i, j, index, dir, 0);

		undoTo(pushStartPoint);

		if (minPushDist == INF) {
			if (mode == PushMode::Confirm) assert(false);
			return INF;
		}

		int cost = minPushDist - totalDistance;

		if (mode != PushMode::OnlyCost) {
			static int moveIndices[1000];
			int numMoveIndices = 0;
			int numOps = len(bestOps);
			rep0(i, numOps) {
				auto& op = bestOps[i];
				if (op.type == OperationType::UnsafeMove) {
					moveIndices[numMoveIndices++] = i;
				} else {
					doAndAddToHistory(op);
				}
			}
			// apply PC moves in backwords, otherwise they will overlap
			while (numMoveIndices > 0) {
				auto& op = bestOps[moveIndices[--numMoveIndices]];
				doAndAddToHistory(op);
			}
		}

		return cost;
	}

private:
	void pushDfs(int i, int j, int comingIndex, int dir, int depth) {
		if (!inBounds(i, j)) return;

		if (totalDistance > minPushDist + 5) return;

		auto& cell = cells[i][j];
		auto& coming = cs[comingIndex];
		defineDirDiffs(dir, di, dj);

		int hp = currentHistoryPoint();
		unsafeMove(comingIndex, di, dj);

		if (cell.isEmpty()) {
			// fulfilled
			cellUpdate(i, j, Cell::computer(comingIndex)); // place the coming PC

			if (totalDistance < minPushDist) {
				minPushDist = totalDistance;
				bestOps.clear();
				bestOps.insert(bestOps.end(), history.begin() + pushStartPoint, history.end());
			}

		} else if (cell.isCable()) {
			pii indices = cell.getCableIndices();
			if (indices.first == comingIndex || indices.second == comingIndex) {
				// fulfilled
				cellUpdate(i, j, Cell::computer(comingIndex)); // place the coming PC

				if (totalDistance < minPushDist) {
					minPushDist = totalDistance;
					bestOps.clear();
					bestOps.insert(bestOps.end(), history.begin() + pushStartPoint, history.end());
				}

			} else if (coming.isLonely()) {
				// can move across the cable, push it twice
				pushDfs(i + di, j + dj, comingIndex, dir, depth + 1);
			}
		} else {
			assert(cell.isComputer());
			// another PC found, push it too if can
			int index = cell.getComputerIndex();
			auto& c = cs[index];
			bool connected = coming.conns[dir] == index;
			if (!c.fixed) {
				// after getting the index, try placing the coming PC
				cellUpdate(i, j, Cell::computer(comingIndex));
				int perp1 = perpDir1(dir);
				int perp2 = perpDir2(dir);
				int dirs[3] = { dir, perp1, perp2 };

				rep0(dirIndex, 3) {
					int d = dirs[dirIndex];
					if (depth >= 2 && d != dir) continue; // go only straight if deep
					if (d == flipDir(currentDir)) continue; // don't go back

					int hp2 = currentHistoryPoint();

					if (!connected) {
						if (c.conns[flipDir(d)] != -1) continue; // don't extend
						if (!c.canMoveTo(d)) continue;
					} else {
						if (d == dir) {
							// from
							//           |
							//        --[c]-[coming]
							//           |
							// to
							//           |
							// --[c]-[coming]
							//           |

							int conn1 = c.conns[perp1];
							int conn2 = c.conns[perp2];
							if (conn1 != -1) {
								disconnect(index, conn1);
								connect(comingIndex, conn1);
							}
							if (conn2 != -1) {
								disconnect(index, conn2);
								connect(comingIndex, conn2);
							}
						} else {
							// from
							//      |
							//      |
							//      |
							//   --[c]-[coming]
							//      |
							// to
							//      |
							//     [c]
							//      |
							// --[coming]
							//      |

							int fdir = flipDir(dir);
							int fd = flipDir(d);
							int connDir = c.conns[dir];
							int connFD = c.conns[fd];

							// do this first
							if (connFD != -1) {
								disconnect(index, connFD);
								connect(comingIndex, connFD);
							}

							// then do this
							unsafeDisconnect(comingIndex, index, dir, fdir);
							unsafeConnect(comingIndex, index, d, fd);

							// and finally do this
							if (connDir != -1) {
								disconnect(index, connDir);
								connect(comingIndex, connDir);
							}
						}
					}

					defineDirDiffs(d, di, dj);
					pushDfs(i + di, j + dj, index, d, depth + 1);

					undoTo(hp2);
				}
			}
		}
		undoTo(hp);
	}
public:

	void move(int i, int dir) {
		auto& c = cs[i];
		assert(cells[c.i][c.j].isComputer());
		assert(cells[c.i][c.j].index1 == i);
		assert(c.canMoveTo(dir));
		int back = c.conns[flipDir(dir)];
		defineDirDiffs(dir, di, dj);
		cellUpdate(c.i, c.j, back == -1 ? Cell::empty() : Cell::cable(c.index, back));
		unsafeMove(i, di, dj);
		assert(inBounds(c.i, c.j));
		assert(cells[c.i][c.j].isEmpty());
		cellUpdate(c.i, c.j, Cell::computer(i));
	}

	void teleport(int i, int toI, int toJ) {
		auto& c = cs[i];
		assert(cells[c.i][c.j].isComputer());
		assert(cells[c.i][c.j].index1 == i);
		cellUpdate(c.i, c.j, Cell::empty());
		unsafeMove(i, toI - c.i, toJ - c.j);
		assert(inBounds(c.i, c.j));
		assert(cells[c.i][c.j].isEmpty());
		cellUpdate(c.i, c.j, Cell::computer(i));
	}

	void draw(Graphics& g, bool onlyComputers, multiset<pii>& cablesHatred) {
		double colors[5][3] = {
			{0.8, 0, 0},
			{0, 0.5, 0},
			{0, 0, 0.9},
			{0.6, 0.3, 0},
			{0.6, 0, 0.8},
		};
		const double cellSize = 10.0;
		g.screen(n * cellSize + 100, n * cellSize);

		g.fontSize(12);
		g.fill(0, 0, 0);
		int score = computeScore();
		g.text("score: " + to_string(score), g.screenW - 50, 50);
		g.text("dist: " + to_string(totalDistance), g.screenW - 50, 100);
		g.text("movs: " + to_string(totalDistance + numConnections), g.screenW - 50, 150);

		g.fontSize(cellSize * 0.5);
		g.strokeWidth(1);

		rep0(i, n + 1) {
			if (i % 5 == 0) {
				if (i % 10 == 0)
					g.stroke(0.7, 0.7, 0.7);
				else
					g.stroke(0.9, 0.9, 0.9);
				g.line(0, cellSize * i, cellSize * n, cellSize * i);
				g.line(cellSize * i, 0, cellSize * i, cellSize * n);
			}
		}

		g.strokeWidth(2);

		if (onlyComputers) {
			repC() {
				auto& c = cs[i];
				g.stroke(colors[c.kind][0], colors[c.kind][1], colors[c.kind][2]);
				double cx = cellSize * (c.j + 0.5);
				double cy = cellSize * (c.i + 0.5);
				double ocx = cellSize * (c.origJ + 0.5);
				double ocy = cellSize * (c.origI + 0.5);
				if (c.dist() > 0) g.line(cx, cy, ocx, ocy);
			}
		} else {
			rep2() {
				auto& cell = cells[i][j];
				double cx = cellSize * (j + 0.5);
				double cy = cellSize * (i + 0.5);
				double r = cellSize * 0.5;
				if (cell.isComputer()) {
					auto& c = cs[cell.index1];
					g.stroke(colors[c.kind][0], colors[c.kind][1], colors[c.kind][2]);
					double ocx = cellSize * (c.origJ + 0.5);
					double ocy = cellSize * (c.origI + 0.5);
					if (c.dist() > 0) g.line(cx, cy, ocx, ocy);
				}
			}
		}

		g.strokeWidth(1.5);

		if (onlyComputers) {
			rep2() {
				double cx = cellSize * (j + 0.5);
				double cy = cellSize * (i + 0.5);
				double r = cellSize * 0.5;
				if (freeCells[i][j]) {
					g.noStroke();
					g.fill(0.5, 0.5, 0.5);
					g.rect(cx - r * 0.2, cy - r * 0.2, r * 0.4, r * 0.4);
				}
			}
			repC() {
				auto& c = cs[i];
				double cx = cellSize * (c.j + 0.5);
				double cy = cellSize * (c.i + 0.5);
				double r = cellSize * 0.5;
				g.fill(colors[c.kind][0], colors[c.kind][1], colors[c.kind][2]);
				g.noStroke();
				g.rect(cx - r, cy - r, r * 2, r * 2);
				double coeff = 1.0 / (1.0 + c.load * 0.5);
				g.fill(0, 0, 0, 1 - coeff);
				g.rect(cx - r * 0.8, cy - r * 0.8, r * 1.6, r * 1.6);
				g.fill(1, 1, 1);
				g.text((c.index < 10 ? "00" : c.index < 100 ? "0" : "") + to_string(c.index), cx, cy + cellSize * 0.25);
			}
		} else {
			rep2() {
				auto& cell = cells[i][j];
				double cx = cellSize * (j + 0.5);
				double cy = cellSize * (i + 0.5);
				double r = cellSize * 0.5;
				if (cell.isEmpty()) {
				} else if (cell.isComputer()) {
					auto& c = cs[cell.index1];
					if (c.fixed)
						g.fill(colors[c.kind][0] * 0.5, colors[c.kind][1] * 0.5, colors[c.kind][2] * 0.5);
					else
						g.fill(colors[c.kind][0], colors[c.kind][1], colors[c.kind][2]);
					g.noStroke();
					g.rect(cx - r, cy - r, r * 2, r * 2);
					g.stroke(0, 0, 0);
					if (c.conns[DIR_L] != -1) g.line(cx, cy, cx - r, cy);
					if (c.conns[DIR_R] != -1) g.line(cx, cy, cx + r, cy);
					if (c.conns[DIR_U] != -1) g.line(cx, cy, cx, cy - r);
					if (c.conns[DIR_D] != -1) g.line(cx, cy, cx, cy + r);
					g.fill(1, 1, 1);
					g.text((c.index < 10 ? "00" : c.index < 100 ? "0" : "") + to_string(c.index), cx, cy + cellSize * 0.25);
				} else {
					double hatred = cablesHatred.count(cell.getCableIndices()) * 0.005;
					g.stroke(min(1.0, hatred), 0, min(1.0, max(0.0, hatred - 1)));

					auto& c1 = cs[cell.index1];
					auto& c2 = cs[cell.index2];
					if (!((c1.i == c2.i) ^ (c1.j == c2.j))) {
						g.line(cx - r, cy - r, cx + r, cy + r);
						g.line(cx - r, cy + r, cx + r, cy - r);
					} else {
						if (cell.isCableHorizontal(cs)) {
							g.line(cx - r, cy, cx + r, cy);
						} else {
							g.line(cx, cy - r, cx, cy + r);
						}
					}
				}
			}
		}
	}

	void print() {
		trace("room:");
		rep0(i, n) {
			string s[3];
			rep0(j, n) {
				Cell& cell = cells[i][j];
				string ss[3] = {
					"   ",
					"   ",
					"   "
				};
				if (cell.isEmpty()) {
				} else if (cell.isComputer()) {
					auto& c = cs[cell.index1];
					ss[1][1] = '0' + c.kind;
					if (c.conns[DIR_L] != -1) ss[1][0] = '-';
					if (c.conns[DIR_R] != -1) ss[1][2] = '-';
					if (c.conns[DIR_U] != -1) ss[0][1] = '|';
					if (c.conns[DIR_D] != -1) ss[2][1] = '|';
				} else {
					if (cell.isCableHorizontal(cs)) {
						ss[1][0] = '-';
						ss[1][1] = '-';
						ss[1][2] = '-';
					} else {
						ss[0][1] = '|';
						ss[1][1] = '|';
						ss[2][1] = '|';
					}
				}
				s[0] += ss[0];
				s[1] += ss[1];
				s[2] += ss[2];
			}
			trace("  " + s[0]);
			trace("  " + s[1]);
			trace("  " + s[2]);
		}
	}

private:
	void unsafeConnect(int i1, int i2, int d1, int d2) {
		doAndAddToHistory(Operation::connect(i1, i2, d1, d2));
	}

	void unsafeDisconnect(int i1, int i2, int d1, int d2) {
		doAndAddToHistory(Operation::disconnect(i1, i2, d1, d2));
	}

	void unsafeMove(int index, int di, int dj) {
		auto& c = cs[index];
		doAndAddToHistory(Operation::unsafeMove(index, di, dj));
	}

	void cellUpdate(int i, int j, Cell to) {
		int dindex1 = to.index1 - cells[i][j].index1;
		int dindex2 = to.index2 - cells[i][j].index2;
		doAndAddToHistory(Operation::cellUpdate(i, j, dindex1, dindex2));
	}
};

struct GreedyMove {
	double cost = 0;
	Diff diff;
};

class CellEvent {
public:
	int index = -1;
	int in = 0;
	int out = INF;
	int dir = 0;

	CellEvent() {
	}

	CellEvent(int index, int in) : index(index), in(in) {
	}
};

class Move {
public:
	int data;

	Move() : data(-1) {
	}

	Move(int index, int dir) : data(index << 16 | dir) {
		assert(data == -1 || this->dir() >= 0 && this->dir() < 4 && this->index() >= 0 && this->index() < MAX_COMP);
	}

	int index() {
		return data >> 16;
	}

	int dir() {
		return data & 0xffff;
	}

	void clear() {
		data = -1;
	}

	bool isEmpty() {
		return data == -1;
	}
};

class Solution {
public:
	int score = 0;
	Room r;
	vector<array<int, 4>> moves;
	vector<array<int, 4>> conns;

	Solution() {
	}

	void clear() {
		score = 0;
		moves.clear();
		conns.clear();
	}

	void print() {
		cout << len(moves) << endl;
		for (auto& move : moves) {
			cout << move[0] << " " << move[1] << " " << move[2] << " " << move[3] << endl;
		}
		cout << len(conns) << endl;
		for (auto& conn : conns) {
			cout << conn[0] << " " << conn[1] << " " << conn[2] << " " << conn[3] << endl;
		}
	}
};


class Solver {
public:
	SolverResult res;
	Solution bestSol;
	Room bestRoom;
	Problem p;
	Room r;
	Room roomBeforeGreedyBuild;
	Room tmpRoom;
	Room tmpRoom2;
	Room beforeThirdRoom;
	int n = 0;
	int k = 0;
	int seed = -1;

	Solver() {
	}

	void init(istream& in) {
		p.read(in);
		n = p.n;
		k = p.k;
		bestSol.clear();
	}

	void load(int seed) {
		this->seed = seed;
		string data = readText("in/"s + (seed < 10 ? "000" : seed < 100 ? "00" : seed < 1000 ? "0" : "") + to_string(seed) + ".txt").first;
		istringstream in(data);
		init(in);
	}

	void solve() {
		res.score = 0;

		r.setup(p);
		double means[MAX_K][2] = {};
		repC() {
			auto& c = r.cs[i];
			means[c.kind][0] += c.i * 0.01;
			means[c.kind][1] += c.j * 0.01;
		}
		double vars[MAX_K][2][2] = {};
		repC() {
			auto& c = r.cs[i];
			double diffI = c.i - means[c.kind][0];
			double diffJ = c.j - means[c.kind][1];
			vars[c.kind][0][0] += diffI * diffI;
			vars[c.kind][0][1] += diffI * diffJ;
			vars[c.kind][1][0] += diffJ * diffI;
			vars[c.kind][1][1] += diffJ * diffJ;
		}
		double vols[MAX_K] = {};
		rep0(k1, k) {
			vols[k1] = vars[k1][0][0] * vars[k1][1][1] - vars[k1][1][0] * vars[k1][0][1];
			trace("vol ", k1, ": ", vols[k1]);
		}

		int movCnt = 0;
		rep0(trial, maxTrial) {
			if (trial > 0) {
				r.writeMovie("movie" + to_string(movCnt++) + ".html");
			}
			r.clearMovie();
			r.setup(p);

			int first;
			int second;

			if (k <= 3) {
				first = rnd.nextInt(k);
				do {
					second = rnd.nextInt(k);
				} while (second == first);
			} else {
				pdi volSorted[MAX_K] = {};
				rep0(k1, k) {
					volSorted[k1] = pdi(vols[k1] * rnd.nextFloat(0.8, 1.25), k1);
				}
				sort(volSorted, volSorted + k);

				first = volSorted[0].second;
				second = volSorted[k - 1].second;
			}

			buildGreedy(first, 0);
			r.computeClusters();
			int pivotIndex = r.cls[0].indices[0];
			if (timer() > TIMEOUT_LIMIT) break;
			refineTree(pivotIndex, second);
			if (timer() > TIMEOUT_LIMIT) break;
			buildGreedy(second, 1);
			if (timer() > TIMEOUT_LIMIT) break;
			if (k >= 3) {
				beforeThirdRoom = r;
				rep0(thirdIter, 10) {
					r = beforeThirdRoom;
					int third;
					do {
						third = rnd.nextInt(k);
					} while (third == first || third == second);
					buildGreedy(third, 2);
					if (timer() > TIMEOUT_LIMIT) break;
				}
			}
		}

		r.writeMovie("movie" + to_string(movCnt++) + ".html");

		if (saveMovieWithSeed) {
			r.writeMovie("movie_" + to_string(k) + "_" + to_string(seed) + ".html");
		} else {
			r.writeMovie("movie.html");
		}

		bestSol.print();
		res.score = bestSol.score;
	}

private:
	Rnd rnd;
	multiset<pii> cablesHatred;

	void refineTree(int pivotIndex, int kindAgainst) {
		auto& pivot = r.cs[pivotIndex];
		int kind = pivot.kind;
		int count = r.computeMaxComputersInConnectedArea(kindAgainst);
		trace("count: ", count);

		r.computeClusters();
		auto& cl = r.cls[pivot.clusterIndex];

		vector<pii> taboo;

		int st = timer();
		int ITER = 200;
		rep0(iter, ITER) {
			int index = -1;
			int dir = -1;
			int trial = 0;
			while (true) {
				if (++trial > 500) {
					index = -1;
					break;
				}
				index = rnd.nextInt(100) + kind * 100;
				dir = rnd.nextInt(2) * 2;
				int index2 = r.cs[index].conns[dir];
				if (index2 == -1) continue;
				auto& c1 = r.cs[index];
				auto& c2 = r.cs[index2];
				if (abs(c1.i - c2.i) + abs(c1.j - c2.j) < 5) continue;
				break;
			}
			if (index == -1) {
				trace("couldn't find a connection to cut");
				break;
			}
			int index2 = r.cs[index].conns[dir];
			r.disconnect(index, index2);

			int newCount = r.computeMaxComputersInConnectedArea(kindAgainst);

			if (newCount > count) {
				//trace("improve? ", count, " -> ", newCount);
				r.addFrame("improve? " + to_string(newCount));
				taboo.emplace_back(min(index, index2), max(index, index2));
				r.computeClusters();
				//tmpRoom = r;
				joinClusters(index, index2, taboo);
				newCount = r.computeMaxComputersInConnectedArea(kindAgainst);
				if (newCount > count) {
					//trace("improved! ", count, " -> ", newCount);
					r.addFrame("improved! " + to_string(count) + " -> " + to_string(newCount));
					count = newCount;
					int leftMoves = 100 * k - r.totalDistance - r.numConnections;
					if (leftMoves <= count) {
						trace("halting refinement... (left moves: ", leftMoves, ", count: ", count, ")");
						break;
					}
					continue;
				}
				//trace("nah ", newCount);
				r.addFrame("nah " + to_string(newCount));
				count = newCount;
				//r = tmpRoom;
				taboo.pop_back();
			} else {
				r.connect(index, index2);
			}
			r.computeClusters();
		}
		trace("time: ", timer() - st);

		if (k >= 4) {
			// shrink leaves
			for (int index : cl.indices) {
				auto& c = r.cs[index];
				int count = 0;
				rep0(d, 4) {
					if (c.conns[d] != -1) count++;
				}
				if (count != 1) continue;
				rep0(d, 4) {
					if (c.conns[d] != -1) {
						auto& c2 = r.cs[c.conns[d]];
						int dist = abs(c.i - c2.i) + abs(c.j - c2.j);
						if (dist > 1) {
							r.multiPush(index, d, min(2, dist - 1), true, PushMode::Confirm);
						}
					}
				}
			}
		}

		r.addFrame();
	}

	void buildGreedy(int kind, int level) {
		//trace("buildGreedy ", level);

		int numPasses = level == 1 ? 4 : 1;

		vector<pii> taboo;
		int bestScore = 0;
		bestRoom = r;

		rep0(pass, numPasses) {
			cablesHatred.clear();

			roomBeforeGreedyBuild = r;

			r.addFrame();

			bool spacious = (n - 3) * (n - 3) >= 100 * k;
			if (spacious && r.numConnections == 0) {
				int border = k >= 4 ? 2 : 1;
				rep0(off, 100) {
					int index = off + kind * 100;
					auto& c = r.cs[index];
					rep0(iter, border) {
						if (c.i < border) r.push(index, DIR_D, PushMode::Try);
						if (c.i >= n - border) r.push(index, DIR_U, PushMode::Try);
						if (c.j < border) r.push(index, DIR_R, PushMode::Try);
						if (c.j >= n - border) r.push(index, DIR_L, PushMode::Try);
					}
				}
			}

			r.addFrame();

			while (true) {
				bool end = true;
				rep(i, kind * 100, kind * 100 + 100) {
					auto& c = r.cs[i];
					rep0(d, 4) {
						if (c.conns[d] == -1) {
							int di = DIRS[d][0];
							int dj = DIRS[d][1];
							int i = c.i + di;
							int j = c.j + dj;
							while (inBounds(i, j)) {
								auto& cell = r.cells[i][j];
								if (cell.isComputer()) {
									auto& c2 = r.cs[cell.getComputerIndex()];
									if (c2.kind == kind && !r.isConnected(c.index, c2.index)) {
										r.connect(c.index, c2.index);
										end = false;
									}
								}
								if (!cell.isEmpty()) break;
								i += di;
								j += dj;
							}
						}
					}
				}
				if (end) break;
			}

			r.addFrame();

			r.computeClusters();

			constexpr int TIGHT_THRESHOLD[6] = { 0, 0, 15, 20, 23, 26 };
			bool tight = n <= TIGHT_THRESHOLD[k];
			if (tight && level == 0) {
				trace("applying the tight tactics...");
				tmpRoom = r; // we can't use undo after any solvePos
				int MAX_COHERE_RADIUS = 5;
				int MIN_COHERE_RADIUS = 1;
				bool completed = false;
				bestRoom = r;
				int bestConns = 0;
				rep(cohereRadius, MIN_COHERE_RADIUS, MAX_COHERE_RADIUS + 1) {
					r = tmpRoom;
					rep0(gatherIter, 2) {
						while (true) {
							if (!gatherSameKind(kind)) break;
							r.addFrame();
						}
						int actualDist = solvePos();
						int actualMoves = actualDist + r.numConnections;
						r.addFrame("actual dist: " + to_string(actualDist) + ", actual moves: " + to_string(actualMoves));
						if (actualMoves > k * 100) {
							break;
						}
						if (r.numConnections > bestConns) {
							bestConns = r.numConnections;
							bestRoom = r;
							if (bestConns == 99) {
								trace("gather completed!");
								completed = true;
								break;
							}
						}
						if (gatherIter == 0) {
							// break all cables
							repC() {
								auto& c = r.cs[i];
								int cl = c.conns[DIR_L];
								int cu = c.conns[DIR_U];
								if (cl != -1) r.disconnect(i, cl);
								if (cu != -1) r.disconnect(i, cu);
							}
							r.addFrame();
							while (true) {
								if (!cohere(kind, cohereRadius)) break;
							}
							r.addFrame("cohere");
						}
					}
					if (completed) break;
				}
			} else {
				// apply utual tactics
				int maxSize = 0;
				int clusterIndex = -1;
				int pivotIndex = -1;
				rep0(i, r.numClusters) {
					auto& cl = r.cls[i];
					if (cl.kind == kind && cl.size > maxSize) {
						maxSize = cl.size;
						pivotIndex = cl.indices[1];
						clusterIndex = i;
					}
				}
				auto& pivot = r.cs[pivotIndex];

				repC() {
					auto& c = r.cs[i];
					if (c.clusterIndex != clusterIndex && c.kind == kind) {
						r.isolate(i);
					}
				}

				{
					tmpRoom = r;
					int actualDist = solvePos();
					if (actualDist + r.numConnections > 100 * k) {
						//trace("have to shrink");
						int numDelete = actualDist + r.numConnections - 100 * k;
						while (numDelete > 0) {
							bool end = true;
							rep0(off, 100) {
								int index = off + kind * 100;
								auto& c = r.cs[index];
								int numConns = (c.conns[0] != -1) + (c.conns[1] != -1) + (c.conns[2] != -1) + (c.conns[3] != -1);
								if (numConns == 1) {
									r.isolate(index);
									r.addFrame("shrinking");
									end = false;
									if (--numDelete <= 0) break;
								}
							}
							if (end) break;
						}
						actualDist = solvePos();
						int actualMoves = actualDist + r.numConnections;
						r.addFrame("actual dist: " + to_string(actualDist) + ", actual moves: " + to_string(actualMoves));
						if (r.numConnections >= 198) trace("actual moves: ", to_string(actualMoves), ", conns: ", bestScore);
						bestRoom = r;
						break;
					}
					r = tmpRoom;
				}

				r.computeClusters();
				clusterIndex = pivot.clusterIndex;

				r.addFrame();

				int left = 100 - r.cls[clusterIndex].size;
				rep0(iter, left) {
					GreedyMove cand;
					cand.cost = INF;

					r.computeClusters();
					clusterIndex = pivot.clusterIndex;

					//  seed   77(k=3): 8193 (time 3095 ms) 0.025
					//  seed   69(k=3): 7876 (time 3100 ms)
					//  seed   89(k=3): 7803 (time 3099 ms)
					//  seed   81(k=3): 7726 (time 3101 ms)
					//total score: 31598, mean: 7899.5

					//  seed   81(k=3): 8110 (time 3097 ms) 0.05
					//  seed   69(k=3): 7879 (time 3087 ms)
					//  seed   77(k=3): 7877 (time 3092 ms)
					//  seed   89(k=3): 7579 (time 3096 ms)
					//total score: 31445, mean: 7861.25

					//  seed   77(k=3): 7800 (time 3087 ms) 0.1
					//  seed   69(k=3): 7725 (time 3097 ms)
					//  seed   81(k=3): 7506 (time 3101 ms)
					//  seed   89(k=3): 7368 (time 3096 ms)
					//total score: 30399, mean: 7599.75

					//  seed   77(k=3): 7806 (time 3089 ms) 0.15
					//  seed   69(k=3): 7652 (time 3090 ms)
					//  seed   89(k=3): 7507 (time 3095 ms)
					//  seed   81(k=3): 7506 (time 3094 ms)
					//total score: 30471, mean: 7617.75

					//  seed   77(k=3): 8110 (time 3092 ms) 0.2
					//  seed   89(k=3): 7652 (time 3097 ms)
					//  seed   69(k=3): 7506 (time 3101 ms)
					//  seed   81(k=3): 7366 (time 3100 ms)
					//total score: 30634, mean: 7658.5

					//  seed   77(k=3): 7800 (time 3091 ms) 0.25
					//  seed   69(k=3): 7296 (time 3092 ms)
					//  seed   81(k=3): 7095 (time 3093 ms)
					//  seed   89(k=3): 6903 (time 3093 ms)
					//total score: 29094, mean: 7273.5

					// ---

					//  seed   69(k=3): 8353 (time 3094 ms) 0.1
					//  seed   77(k=3): 8193 (time 3097 ms)
					//  seed   81(k=3): 7879 (time 3097 ms)
					//  seed   89(k=3): 7234 (time 3094 ms)
					//total score: 31659, mean: 7914.75

					// density 0.22



					//  seed   53(k=3): 8190 (time 3106 ms) 0.1
					//  seed  137(k=3): 7581 (time 3099 ms)
					//  seed  165(k=3): 7445 (time 3099 ms)
					//  seed   49(k=3): 7436 (time 3088 ms)
					//total score: 30652, mean: 7663

					// density 0.5


					double density = 100.0 * k / (n * n);
					double cableLengthCost = level == 0 ? 0.05 + 0.14 * density : -0.3 * density + 0.03;

					findCandidatePush(clusterIndex, cand, cableLengthCost);
					if (timer() > TIMEOUT_LIMIT) break;
					findCandidateJoin(clusterIndex, false, cand, cableLengthCost);
					if (timer() > TIMEOUT_LIMIT) break;
					findCandidateJoin(clusterIndex, true, cand, cableLengthCost);
					if (timer() > TIMEOUT_LIMIT) break;

					if (isFinite(cand.cost)) {
						applyMove(cand);
						r.addFrame();
					} else {
						int actualDist = solvePos();
						int actualMoves = actualDist + r.numConnections;
						r.addFrame("actual dist: " + to_string(actualDist) + ", actual moves: " + to_string(actualMoves));
						if (actualMoves <= 100 * k && r.numConnections > bestScore) {
							bestScore = r.numConnections;
							bestRoom = r;
							if (r.numConnections >= 198) trace("actual moves: ", to_string(actualMoves), ", conns: ", bestScore);
						}
						break;
					}

					if (r.totalDistance + r.numConnections >= k * 100 - 60) {
						decreaseDist();
						int actualDist = solvePos();
						int actualMoves = actualDist + r.numConnections;
						r.addFrame("actual dist: " + to_string(actualDist) + ", actual moves: " + to_string(actualMoves));

						if (timer() > TIMEOUT_LIMIT) break;

						if (actualMoves <= 100 * k && r.numConnections > bestScore) {
							bestScore = r.numConnections;
							bestRoom = r;
							if (r.numConnections >= 198) trace("actual moves: ", to_string(actualMoves), ", conns: ", bestScore);
						}
						if (actualDist + r.numConnections >= k * 100) {
							break;
						}
					} else {
						if (r.numConnections > bestScore) {
							bestScore = r.numConnections;
							bestRoom = r;
						}
					}
				}
			}

			if (level == 1 && pass < numPasses - 1) {
				vector<pii> sorted;

				auto iter = cablesHatred.begin();
				while (iter != cablesHatred.end()) {
					pii cable = *iter;
					sorted.emplace_back((int) cablesHatred.count(cable), cable.first << 16 | cable.second);
					iter = cablesHatred.upper_bound(cable);
				}

				sort(sorted.begin(), sorted.end(), greater<pii>());

				r = roomBeforeGreedyBuild;

				r.addFrame();

				int maxCuts = 5;
				rep0(index, min(len(sorted), maxCuts)) {
					pii item = sorted[index];
					int hatred = item.first;
					pii cable = pii(item.second >> 16, item.second & 0xffff);
					if (!r.isDirectlyConnected(cable.first, cable.second)) continue; // obsolete
					taboo.push_back(cable);
					r.disconnect(cable.first, cable.second);
					r.addFrame();
					r.computeClusters();
					joinClusters(cable.first, cable.second, taboo);
					if (timer() > TIMEOUT_LIMIT) break;
					r.addFrame();
				}
			}
		}
		r = bestRoom;
		r.addFrame("get the best state");
	}

	void computeCellEvents(vector<CellEvent> celles[MAX_N][MAX_N], vector<Move>& moves) {
		rep2() {
			celles[i][j].clear();
		}
		repC() {
			auto& c = r.cs[i];
			c.i = c.origI;
			c.j = c.origJ;
			celles[c.i][c.j].emplace_back(i, -1);
		}
		int turn = 0;
		for (auto move : moves) {
			int index = move.index();
			int dir = move.dir();
			defineDirDiffs(dir, di, dj);
			auto& c = r.cs[index];
			int i = c.i;
			int j = c.j;
			auto& e1 = celles[i][j];
			auto& e2 = celles[i + di][j + dj];
			c.i += di;
			c.j += dj;
			assert(isInf(e1.back().out));
			assert(e2.empty() || e2.back().out < turn);
			e1.back().out = turn;
			e1.back().dir = dir;
			e2.emplace_back(index, turn);
			turn++;
		}
	}

	void updateOccupation(vector<CellEvent>& ces, bool isFree[], int from, int to) {
		for (auto& ce : ces) {
			int st = max(from, ce.in + 1);
			int en = min(to, ce.out);
			rep(i, st, en + 1) {
				isFree[i] = false; // someone in
			}
		}
	}

	void initOccupation(bool isFree[], int from, int to) {
		rep(i, from, to + 1) isFree[i] = true;
	}

	bool improveMoves(vector<CellEvent> celles[MAX_N][MAX_N], vector<Move>& moves) {
		computeCellEvents(celles, moves);

		int numMoves = len(moves);
		int maxTime = numMoves;
		bool* isFree = new bool[maxTime + 1];

		int insertBefore = -1;
		vector<Move> movesToInsert;

		repCNamed(index) {
			auto& c = r.cs[index];
			vector<int> poss; // eindex << 16 | i << 8 | j
			int i = c.origI;
			int j = c.origJ;
			int time = -1;
			while (true) {
				auto& es = celles[i][j];
				int eindex = -1;
				int numE = len(es);
				rep0(i, numE) {
					if (es[i].in == time) {
						eindex = i;
						break;
					}
				}
				assert(eindex != -1);
				poss.push_back(eindex << 16 | i << 8 | j);
				auto& e = es[eindex];
				if (isInf(e.out)) break;
				defineDirDiffs(e.dir, di, dj);
				i += di;
				j += dj;
				time = e.out;
			}
			int numP = len(poss);
			int moveCount = numP - 1;
			if (moveCount > c.dist()) {
				// try to reduce moves
				rep0(posA, numP - 1) {
					// make better found first
					repr(posB, posA + 1, numP) {
						int dataA = poss[posA];
						int dataB = poss[posB];
						int ia = dataA >> 8 & 0xff;
						int ja = dataA & 0xff;
						int ib = dataB >> 8 & 0xff;
						int jb = dataB & 0xff;
						int eia = dataA >> 16;
						int eib = dataB >> 16;
						auto& esa = celles[ia][ja];
						auto& esb = celles[ib][jb];
						auto& ea = esa[eia];
						auto& eb = esb[eib];
						bool isALast = eia == len(esa) - 1;
						bool isBFirst = eib == 0;
						int dist = abs(ia - ib) + abs(ja - jb);
						int steps = posB - posA;
						if (steps == dist) continue; // can't improve
						// can leave A during
						int minA = ea.in + 1;
						int maxA = isALast ? INF : esa[eia + 1].in;
						// can reach B during
						int minB = isBFirst ? 0 : esb[eib - 1].out + 1;
						int maxB = eb.out;
						int minT = max(minA, minB);
						int maxT = min(min(maxA, maxB), maxTime); // prevent INF
						if (minT > maxT) continue; // can't leave A and reach B
						//trace("chance! ", minT, " ", maxT, " ", index, " ", posA, " ", posB);
						int diffi = ib - ia;
						int diffj = jb - ja;

						vector<int> dirs;
						int successfulTime = -1;

						if (diffi == 0 && diffj == 0) {
							if (solvePosVerbose) trace("trivial case ", index); // this should be done prevously
							successfulTime = minT;
						} else if (diffi == 0 || diffj == 0) {
							// single path
							int dir = dirFromDiffs(diffi, diffj);
							defineDirDiffs(dir, di, dj);
							int i = ia + di;
							int j = ja + dj;
							dirs.push_back(dir);
							initOccupation(isFree, minT, maxT);
							while (i != ib || j != jb) {
								updateOccupation(celles[i][j], isFree, minT, maxT);
								i += di;
								j += dj;
								dirs.push_back(dir);
							}
							rep(t, minT, maxT + 1) {
								if (isFree[t]) {
									successfulTime = t;
									if (solvePosVerbose) trace("successful 1!! ", index, " t=", t);
									break;
								}
							}
						} else {
							// multiple pathes
							int di = diffi < 0 ? -1 : 1;
							int dj = diffj < 0 ? -1 : 1;
							int dirI = dirFromDiffs(di, 0);
							int dirJ = dirFromDiffs(0, dj);
							int numI = abs(diffi);
							int numJ = abs(diffj);

							rep0(path, numI + numJ) {
								dirs.clear();
								initOccupation(isFree, minT, maxT);
								if (path < numJ) {
									int goI = path;
									rep0(iter, numJ) {
										if (iter == goI) {
											rep0(iter2, numI) {
												dirs.push_back(dirI);
											}
										}
										dirs.push_back(dirJ);
									}
								} else {
									int goJ = path - numJ;
									rep0(iter, numI) {
										if (iter == goJ) {
											rep0(iter2, numJ) {
												dirs.push_back(dirJ);
											}
										}
										dirs.push_back(dirI);
									}
								}
								int i = ia;
								int j = ja;
								for (int dir : dirs) {
									defineDirDiffs(dir, di, dj);
									i += di;
									j += dj;
									if (i != ib || j != jb) updateOccupation(celles[i][j], isFree, minT, maxT);
								}
								rep(t, minT, maxT + 1) {
									if (isFree[t]) {
										successfulTime = t;
										if (solvePosVerbose) trace("successful 2!! ", index, " t=", t);
										break;
									}
								}
								if (successfulTime != -1) break;
							}
						}

						if (successfulTime == -1) continue; // couldn't improve

						int removeMin = ea.out;
						int removeMax = eb.in;
						rep(t, removeMin, removeMax + 1) {
							if (moves[t].index() == index) {
								moves[t].clear();
							}
						}

						for (int dir : dirs) {
							movesToInsert.emplace_back(index, dir);
						}
						insertBefore = successfulTime;
						break;
					}
					if (insertBefore != -1) break;
				}
				if (insertBefore != -1) break;
			}
		}

		delete[] isFree;

		if (insertBefore == -1) return false;

		vector<Move> tmp;
		rep0(i, numMoves) {
			if (i == insertBefore) {
				tmp.insert(tmp.end(), movesToInsert.begin(), movesToInsert.end());
			}
			if (!moves[i].isEmpty()) {
				tmp.push_back(moves[i]);
			}
		}
		if (insertBefore == numMoves) { // just append
			tmp.insert(tmp.end(), movesToInsert.begin(), movesToInsert.end());
		}
		moves = move(tmp);

		return true;
	}

	bool removeTrailingMoves(vector<CellEvent> celles[MAX_N][MAX_N], vector<Move>& moves) {
		computeCellEvents(celles, moves);
		int numMoves = len(moves);
		bool res = false;
		repCNamed(index) {
			auto& c = r.cs[index];
			if (c.isLonely()) {
				vector<int> poss; // eindex << 16 | i << 8 | j
				int i = c.origI;
				int j = c.origJ;
				int time = -1;
				while (true) {
					auto& es = celles[i][j];
					int eindex = -1;
					int numE = len(es);
					rep0(i, numE) {
						if (es[i].in == time) {
							eindex = i;
							break;
						}
					}
					assert(eindex != -1);
					poss.push_back(eindex << 16 | i << 8 | j);
					auto& e = es[eindex];
					if (isInf(e.out)) break;

					// this cell's last
					if (eindex == len(es) - 1) {
						if (!r.cells[i][j].isCable()) {
							int count = 0;
							rep(t, e.out, numMoves) {
								if (moves[t].index() == index) {
									moves[t].clear();
									count++;
								}
							}
							if (solvePosVerbose) trace("removed ", count, " trailing mov(s) of ", index);
							res = true;
							break;
						}
					}

					defineDirDiffs(e.dir, di, dj);
					i += di;
					j += dj;
					time = e.out;
				}
			}
		}
		if (res) {
			filterMoves(moves);
			cleanupHistory(moves);
		}
		return res;
	}

	bool escapeToFreeCels(vector<CellEvent> celles[MAX_N][MAX_N], vector<Move>& moves) {
		computeCellEvents(celles, moves);
		computeFreeCells(celles);
		int numMoves = len(moves);
		bool res = false;
		repCNamed(index) {
			auto& c = r.cs[index];
			if (c.isLonely()) {
				vector<int> poss; // eindex << 16 | i << 8 | j
				int i = c.origI;
				int j = c.origJ;
				int time = -1;
				while (true) {
					auto& es = celles[i][j];
					int eindex = -1;
					int numE = len(es);
					rep0(i, numE) {
						if (es[i].in == time) {
							eindex = i;
							break;
						}
					}
					assert(eindex != -1);
					poss.push_back(eindex << 16 | i << 8 | j);
					auto& e = es[eindex];
					if (isInf(e.out)) break;

					defineDirDiffs(e.dir, di, dj);
					bool isNextCellFree = len(celles[i + di][j + dj]) == 1;

					if (!isNextCellFree) {
						rep0(d, 4) {
							defineDirDiffs(d, di, dj);
							int i2 = i + di;
							int j2 = j + dj;
							if (inBounds(i2, j2) && r.freeCells[i2][j2]) {
								int count = 0;
								rep(t, e.out, numMoves) {
									if (moves[t].index() == index) {
										moves[t].clear();
										count++;
									}
								}
								moves.emplace(moves.begin() + e.in + 1, index, d);
								if (solvePosVerbose) trace("escaped and reduced ", count - 1, " moves ", index);

								filterMoves(moves);
								cleanupHistory(moves);
								return true;
							}
						}
					}

					i += di;
					j += dj;
					time = e.out;
				}
			}
		}
		return false;
	}

	void computeLoads(vector<Move>& moves) {
		int counts[MAX_COMP] = {};
		for (auto move : moves) {
			counts[move.index()]++;
		}
		repC() {
			auto& c = r.cs[i];
			c.load = counts[i] - c.dist();
		}
	}

	void filterMoves(vector<Move>& moves) {
		vector<Move> tmp;
		for (auto move : moves) {
			if (!move.isEmpty()) tmp.push_back(move);
		}
		moves = move(tmp);
	}

	void cleanupHistory(vector<Move>& moves) {
		vector<pii> conns;
		repC() {
			auto& c = r.cs[i];
			int cl = c.conns[DIR_L];
			int cu = c.conns[DIR_U];
			if (cl != -1) conns.emplace_back(i, cl);
			if (cu != -1) conns.emplace_back(i, cu);
		}
		r.undoTo(0);
		for (auto move : moves) {
			r.move(move.index(), move.dir());
		}
		for (pii conn : conns) {
			r.connect(conn.first, conn.second);
		}
	}

	void computeFreeCells(vector<CellEvent> celles[MAX_N][MAX_N]) {
		rep2() {
			r.freeCells[i][j] = celles[i][j].empty() && !r.cells[i][j].isCable();
		}
	}

	void tweakMoveTime(vector<CellEvent> celles[MAX_N][MAX_N], vector<Move>& moves) {
		if (len(moves) == 0) return;
		computeCellEvents(celles, moves);
		int left = 100;
		int numMoves = len(moves);
		int maxTime = numMoves;
		rep0(iter, 200) {
			int i;
			int j;
			int trial = 0;
			do {
				i = rnd.nextInt(n);
				j = rnd.nextInt(n);
			} while (len(celles[i][j]) == 0 || isInf(celles[i][j][0].out));
			auto& es1 = celles[i][j];
			int numE1 = len(es1);
			int ei1;
			do {
				ei1 = rnd.nextInt(numE1);
			} while (isInf(es1[ei1].out));
			auto& e1 = es1[ei1];
			int dir = e1.dir;
			defineDirDiffs(dir, di, dj);
			int time = e1.out;
			auto& es2 = celles[i + di][j + dj];
			int ei2 = -1;
			int numE2 = len(es2);
			rep0(i, numE2) {
				if (es2[i].in == time) {
					ei2 = i;
					break;
				}
			}
			assert(ei2 != -1);
			auto& e2 = es2[ei2];

			int min1 = e1.in + 1;
			int max1 = ei1 == numE1 - 1 ? INF : es1[ei1 + 1].in - 1;
			int min2 = ei2 == 0 ? 0 : es2[ei2 - 1].out + 1;
			int max2 = e2.out - 1;
			int minT = max(min1, min2);
			int maxT = min(min(max1, max2), maxTime);

			int newTime = rnd.nextInt(minT, maxT);
			e1.out = newTime;
			e2.in = newTime;
		}
		priority_queue<pii, vector<pii>, greater<pii>> q;
		rep2() {
			for (auto& e : celles[i][j]) {
				if (isFinite(e.out)) {
					q.emplace(e.out, e.index << 16 | e.dir);
				}
			}
		}
		moves.clear();
		while (!q.empty()) {
			int m = q.top().second;
			q.pop();
			moves.emplace_back(m >> 16, m & 0xffff);
		}
	}

	void writeMoves(vector<Move>& moves) {
		r.clearMovie();
		int poss[MAX_COMP][2] = {};
		repC() {
			auto& c = r.cs[i];
			poss[i][0] = c.i;
			poss[i][1] = c.j;
			c.i = c.origI;
			c.j = c.origJ;
		}
		r.addFrame("", true);
		for (auto move : moves) {
			int index = move.index();
			int dir = move.dir();
			defineDirDiffs(dir, di, dj);
			auto& c = r.cs[index];
			c.i += di;
			c.j += dj;
			r.addFrame("", true);
		}
		r.addFrame();
		r.writeMovie("movs" + to_string(len(moves)) + ".html");
		repC() {
			auto& c = r.cs[i];
			c.i = poss[i][0];
			c.j = poss[i][1];
		}
	}

	int solvePos() {
		auto& his = r.history;
		vector<Move> moves;
		for (auto& op : his) {
			if (op.type == OperationType::UnsafeMove) {
				int index;
				int di;
				int dj;
				op.getUnsafeMove(index, di, dj);
				assert(abs(di) + abs(dj) == 1);
				int dir = dirFromDiffs(di, dj);
				moves.emplace_back(index, dir);
			}
		}
		vector<CellEvent> celles[MAX_N][MAX_N];

		computeCellEvents(celles, moves);
		int numRemoved = 0;
		while (true) {
			bool end = true;
			rep2() {
				auto& es = celles[i][j];
				int numEs = len(es);
				rep0(index, numEs - 1) {
					auto& e1 = es[index];
					auto& e2 = es[index + 1];
					if (e1.index == e2.index) {
						// remove events in between e1 and e2
						end = false;

						int time = e1.out;
						moves[time].clear();
						numRemoved++;
						defineDirDiffs(e1.dir, di, dj);
						int i2 = i + di;
						int j2 = j + dj;
						while (i2 != i || j2 != j) {
							auto& es2 = celles[i2][j2];
							int delIndex = 0;
							for (auto& e : es2) {
								if (e.in == time) break;
								delIndex++;
							}
							assert(delIndex < len(es2));
							auto& del = es2[delIndex];
							//trace("remove! ", del.index, " [", del.in, ", ", del.out, ") at (", i2, ", ", j2, ")");
							assignDirDiffs(del.dir, di, dj);
							i2 += di;
							j2 += dj;
							time = del.out;
							moves[time].clear();
							numRemoved++;
							es2.erase(es2.begin() + delIndex);
						}

						// merge e1 and e2
						e1.out = e2.out;
						e1.dir = e2.dir;
						es.erase(es.begin() + index + 1);
						numEs--;
						index--;
					}
				}
			}
			if (end) break;
		}
		filterMoves(moves);

		while (true) {
			while (true) {
				//writeMoves(moves);
				//r.sanityCheck();
				if (!improveMoves(celles, moves)) {
					tweakMoveTime(celles, moves);
					if (!improveMoves(celles, moves)) break;
					if (timer() > TIMEOUT_LIMIT) break;
				}
				if (timer() > TIMEOUT_LIMIT) break;
			}
			//writeMoves(moves);
			//r.sanityCheck();
			if (!removeTrailingMoves(celles, moves) && !escapeToFreeCels(celles, moves)) {
				tweakMoveTime(celles, moves);
				if (!removeTrailingMoves(celles, moves) && !escapeToFreeCels(celles, moves)) break;
				if (timer() > TIMEOUT_LIMIT) break;
			}
			if (timer() > TIMEOUT_LIMIT) break;
		}

		//computeCellEvents(celles, moves);
		//computeFreeCells(celles);
		//computeLoads(moves);
		//r.clearMovie();
		//repC() {
		//	auto& c = r.cs[i];
		//	c.i = c.origI;
		//	c.j = c.origJ;
		//}
		//r.addFrame("", true);
		//for (auto move : moves) {
		//	int index = move.index();
		//	int dir = move.dir();
		//	defineDirDiffs(dir, di, dj);
		//	auto& c = r.cs[index];
		//	c.i += di;
		//	c.j += dj;
		//	r.addFrame("", true);
		//}
		//r.addFrame();
		//r.writeMovie("movs" + to_string(len(moves)) + ".html");
		//trace("total actual move: ", len(moves), ", distance: ", r.totalDistance, ", removable: ", numRemoved);

		int totalMoves = len(moves) + r.numConnections;
		int score = r.computeScore();
		if (totalMoves <= k * 100 && score > bestSol.score) {
			bestSol.clear();
			bestSol.score = score;
			repC() {
				auto& c = r.cs[i];
				c.i = c.origI;
				c.j = c.origJ;
			}
			for (auto move : moves) {
				int index = move.index();
				int dir = move.dir();
				defineDirDiffs(dir, di, dj);
				auto& c = r.cs[index];
				bestSol.moves.push_back({ c.i, c.j, c.i + di, c.j + dj });
				c.i += di;
				c.j += dj;
			}
			repC() {
				auto& c = r.cs[i];
				int i1 = c.i;
				int j1 = c.j;
				int cl = c.conns[DIR_L];
				int cu = c.conns[DIR_U];
				if (cl != -1) {
					auto& c2 = r.cs[cl];
					int i2 = c2.i;
					int j2 = c2.j;
					bestSol.conns.push_back({ i1, j1, i2, j2 });
				}
				if (cu != -1) {
					auto& c2 = r.cs[cu];
					int i2 = c2.i;
					int j2 = c2.j;
					bestSol.conns.push_back({ i1, j1, i2, j2 });
				}
			}
			bestSol.r = r;
		}

		return len(moves);
	}

	void decreaseDist() {
		int totalDist = r.totalDistance;
		while (true) {
			bool end = true;
			repC() {
				auto& c = r.cs[i];
				rep0(d, 4) {
					int hp = r.currentHistoryPoint();
					int moveCost = r.push(i, d, PushMode::Try);
					if (moveCost > 0) {
						r.undoTo(hp);
					}
					if (moveCost < 0) {
						end = false;
					}
				}
			}
			if (end) break;
		}
		int totalDist2 = r.totalDistance;
		if (solvePosVerbose) trace("decreased ", totalDist - totalDist2, "!");
	}

	bool gatherSameKind(int kind) {
		r.computeClusters();
		priority_queue<pii> pairs;
		int base = kind * 100;
		rep0(off1, 100) {
			auto& c1 = r.cs[off1 + base];
			rep0(off2, off1) {
				auto& c2 = r.cs[off2 + base];
				if (c1.clusterIndex != -1 && c2.clusterIndex != -1 && c1.clusterIndex == c2.clusterIndex) continue;
				int i1 = c1.i;
				int j1 = c1.j;
				int i2 = c2.i;
				int j2 = c2.j;
				int di = abs(i1 - i2);
				int dj = abs(j1 - j2);
				int cost = di + dj;
				pairs.emplace(cost, c1.index << 16 | c2.index);
				if (pairs.size() > 50) {
					pairs.pop();
				}
			}
		}

		if (pairs.empty()) {
			trace("gather completed!");
			return false;
		}

		GreedyMove cand;
		cand.cost = INF;
		int hp = r.currentHistoryPoint();
		int dist1 = r.totalDistance;

		int limit = 50;
		int numPairsCheck = min(limit, len(pairs));
		rep0(pairIndex, numPairsCheck) {
			pii pair = pairs.top();
			pairs.pop();
			if (timer() > TIMEOUT_LIMIT) break;
			int index1 = pair.second >> 16;
			int index2 = pair.second & 0xffff;
			findCandidateMeet(index1, index2, cand, 10);
		}

		if (isInf(cand.cost)) {
			//trace("couldn't gather");
			return false;
		}

		r.applyDiff(cand.diff);
		return true;
	}

	bool cohere(int kind, int border) {
		bool res = false;
		rep0(off, 100) {
			int index = off + kind * 100;
			auto& c = r.cs[index];
			int i = c.i;
			int j = c.j;
			if (!(i < border || i >= n - border || j < border || j >= n - border)) continue;
			int di = (n - 1) - (i << 1);
			int dj = (n - 1) - (j << 1);
			bool ifirst = abs(di) > abs(dj);
			if (abs(di) <= 1) di = 0;
			if (abs(dj) <= 1) dj = 0;
			// take signum
			di = (di > 0) - (di < 0);
			dj = (dj > 0) - (dj < 0);
			int i2 = i + di;
			int j2 = j + dj;
			int dirI = di == 0 ? -1 : dirFromDiffs(di, 0);
			int dirJ = dj == 0 ? -1 : dirFromDiffs(0, dj);
			if (ifirst) {
				if (inBounds(i2, j) && r.cells[i2][j].isEmpty() && c.canMoveTo(dirI)) {
					r.move(index, dirI);
					res = true;
				} else if (inBounds(i, j2) && r.cells[i][j2].isEmpty() && c.canMoveTo(dirJ)) {
					r.move(index, dirJ);
					res = true;
				}
			} else {
				if (inBounds(i, j2) && r.cells[i][j2].isEmpty() && c.canMoveTo(dirJ)) {
					r.move(index, dirJ);
					res = true;
				} else if (inBounds(i2, j) && r.cells[i2][j].isEmpty() && c.canMoveTo(dirI)) {
					r.move(index, dirI);
					res = true;
				}
			}
		}
		return res;
	}

	void findCandidateMeet(int index1, int index2, GreedyMove& cand, double cableLengthCost) {
		auto& c1 = r.cs[index1];
		auto& c2 = r.cs[index2];
		c1.fixed = true;
		c2.fixed = true;
		bool canMoveH1 = c1.conns[DIR_U] == -1 && c1.conns[DIR_D] == -1;
		bool canMoveV1 = c1.conns[DIR_L] == -1 && c1.conns[DIR_R] == -1;
		bool canMoveH2 = c2.conns[DIR_U] == -1 && c2.conns[DIR_D] == -1;
		bool canMoveV2 = c2.conns[DIR_L] == -1 && c2.conns[DIR_R] == -1;
		int i1 = c1.i;
		int j1 = c1.j;
		int i2 = c2.i;
		int j2 = c2.j;
		int minI = min(i1, i2);
		int maxI = max(i1, i2);
		int minJ = min(j1, j2);
		int maxJ = max(j1, j2);

		int hp = r.currentHistoryPoint();
		int prevDist = r.totalDistance;

		do {
			if (j1 == j2) break;
			if (j1 < j2 && (c1.conns[DIR_R] != -1 || c2.conns[DIR_L] != -1)) break;
			if (j1 > j2 && (c1.conns[DIR_L] != -1 || c2.conns[DIR_R] != -1)) break;
			rep(meetI, minI, maxI + 1) {
				if (timer() > TIMEOUT_LIMIT) break;
				int d1 = meetI - i1;
				int d2 = meetI - i2;
				if (d1 != 0 && !canMoveV1) continue;
				if (d2 != 0 && !canMoveV2) continue;
				int cost1 = r.multiPush(index1, d1 < 0 ? DIR_U : DIR_D, abs(d1), true, PushMode::Try);
				int cost2 = r.multiPush(index2, d2 < 0 ? DIR_U : DIR_D, abs(d2), true, PushMode::Try);
				if (isFinite(cost1 + cost2)) {
					if (isFinite(forceConnect(index1, index2))) {
						double cost = r.totalDistance - prevDist + cableLengthCost * (r.distBetween(index1, index2) - 1);
						if (cost < cand.cost || cost <= cand.cost && rnd.nextInt(2) == 0) {
							cand.cost = cost;
							r.makeDiff(cand.diff, hp);
						}
					}
				}
				r.undoTo(hp);
			}
		} while (false);

		do {
			if (i1 == i2) break;
			if (i1 < i2 && (c1.conns[DIR_D] != -1 || c2.conns[DIR_U] != -1)) break;
			if (i1 > i2 && (c1.conns[DIR_U] != -1 || c2.conns[DIR_D] != -1)) break;
			rep(meetJ, minJ, maxJ + 1) {
				if (timer() > TIMEOUT_LIMIT) break;
				int d1 = meetJ - j1;
				int d2 = meetJ - j2;
				if (d1 != 0 && !canMoveH1) continue;
				if (d2 != 0 && !canMoveH2) continue;
				int cost1 = r.multiPush(index1, d1 < 0 ? DIR_L : DIR_R, abs(d1), true, PushMode::Try);
				int cost2 = r.multiPush(index2, d2 < 0 ? DIR_L : DIR_R, abs(d2), true, PushMode::Try);
				if (isFinite(cost1 + cost2)) {
					if (isFinite(forceConnect(index1, index2))) {
						double cost = r.totalDistance - prevDist + cableLengthCost * (r.distBetween(index1, index2) - 1);
						if (cost < cand.cost || cost <= cand.cost && rnd.nextInt(2) == 0) {
							cand.cost = cost;
							r.makeDiff(cand.diff, hp);
						}
					}
				}
				r.undoTo(hp);
			}
		} while (false);

		c1.fixed = false;
		c2.fixed = false;
	}

	void joinClusters(int pivot1, int pivot2, vector<pii>& taboo) {
		int ci1 = r.cs[pivot1].clusterIndex;
		int ci2 = r.cs[pivot2].clusterIndex;
		if (ci1 == -1) {
			ci1 = r.numClusters++;
			auto& cl = r.cls[ci1];
			cl.indices.clear();
			cl.indices.push_back(pivot1);
		}
		if (ci2 == -1) {
			ci2 = r.numClusters++;
			auto& cl = r.cls[ci2];
			cl.indices.clear();
			cl.indices.push_back(pivot2);
		}

		assert(ci1 != ci2);
		auto& cl1 = r.cls[ci1];
		auto& cl2 = r.cls[ci2];
		auto& is1 = cl1.indices;
		auto& is2 = cl2.indices;
		vector<pii> pairs;
		for (int index1 : is1) {
			auto& c1 = r.cs[index1];
			for (int index2 : is2) {
				if (count(taboo.begin(), taboo.end(), pii(min(index1, index2), max(index1, index2))) > 0) continue;
				auto& c2 = r.cs[index2];
				int i1 = c1.i;
				int j1 = c1.j;
				int i2 = c2.i;
				int j2 = c2.j;
				int di = abs(i1 - i2);
				int dj = abs(j1 - j2);
				int cost = di + dj;
				pairs.emplace_back(cost, index1 << 16 | index2);
			}
		}
		sort(pairs.begin(), pairs.end());

		GreedyMove cand;
		cand.cost = INF;
		int hp = r.currentHistoryPoint();
		int dist1 = r.totalDistance;

		for (pii pair : pairs) {
			if (timer() > TIMEOUT_LIMIT) break;
			int index1 = pair.second >> 16;
			int index2 = pair.second & 0xffff;
			auto& c1 = r.cs[index1];
			auto& c2 = r.cs[index2];
			c1.fixed = true;
			c2.fixed = true;
			bool canMoveH1 = c1.conns[DIR_U] == -1 && c1.conns[DIR_D] == -1;
			bool canMoveV1 = c1.conns[DIR_L] == -1 && c1.conns[DIR_R] == -1;
			bool canMoveH2 = c2.conns[DIR_U] == -1 && c2.conns[DIR_D] == -1;
			bool canMoveV2 = c2.conns[DIR_L] == -1 && c2.conns[DIR_R] == -1;
			int i1 = c1.i;
			int j1 = c1.j;
			int i2 = c2.i;
			int j2 = c2.j;
			int minI = min(i1, i2);
			int maxI = max(i1, i2);
			int minJ = min(j1, j2);
			int maxJ = max(j1, j2);

			do {
				if (j1 == j2) break;
				if (j1 < j2 && (c1.conns[DIR_R] != -1 || c2.conns[DIR_L] != -1)) break;
				if (j1 > j2 && (c1.conns[DIR_L] != -1 || c2.conns[DIR_R] != -1)) break;
				rep(meetI, minI, maxI + 1) {
					int d1 = meetI - i1;
					int d2 = meetI - i2;
					if (d1 != 0 && !canMoveV1) continue;
					if (d2 != 0 && !canMoveV2) continue;
					int cost1 = r.multiPush(index1, d1 < 0 ? DIR_U : DIR_D, abs(d1), true, PushMode::Try);
					int cost2 = r.multiPush(index2, d2 < 0 ? DIR_U : DIR_D, abs(d2), true, PushMode::Try);
					if (isFinite(cost1 + cost2)) {
						if (isFinite(forceConnect(index1, index2))) {
							int cost = pair.first + (r.totalDistance - dist1) * 2;
							if (cost < cand.cost || cost <= cand.cost && rnd.nextInt(2) == 0) {
								cand.cost = cost;
								r.makeDiff(cand.diff, hp);
							}
						}
					}
					r.undoTo(hp);
				}
			} while (false);

			do {
				if (i1 == i2) break;
				if (i1 < i2 && (c1.conns[DIR_D] != -1 || c2.conns[DIR_U] != -1)) break;
				if (i1 > i2 && (c1.conns[DIR_U] != -1 || c2.conns[DIR_D] != -1)) break;
				rep(meetJ, minJ, maxJ + 1) {
					int d1 = meetJ - j1;
					int d2 = meetJ - j2;
					if (d1 != 0 && !canMoveH1) continue;
					if (d2 != 0 && !canMoveH2) continue;
					int cost1 = r.multiPush(index1, d1 < 0 ? DIR_L : DIR_R, abs(d1), true, PushMode::Try);
					int cost2 = r.multiPush(index2, d2 < 0 ? DIR_L : DIR_R, abs(d2), true, PushMode::Try);
					if (isFinite(cost1 + cost2)) {
						if (isFinite(forceConnect(index1, index2))) {
							int cost = pair.first + (r.totalDistance - dist1) * 2;
							if (cost < cand.cost || cost <= cand.cost && rnd.nextInt(2) == 0) {
								cand.cost = cost;
								r.makeDiff(cand.diff, hp);
							}
						}
					}
					r.undoTo(hp);
				}
			} while (false);

			c1.fixed = false;
			c2.fixed = false;
		}

		if (isInf(cand.cost)) {
			trace("couldn't join");
			return;
		}

		r.applyDiff(cand.diff);
	}

	int forceConnect(int index1, int index2) {
		auto& c1 = r.cs[index1];
		auto& c2 = r.cs[index2];
		int i1 = c1.i;
		int j1 = c1.j;
		int i2 = c2.i;
		int j2 = c2.j;
		int d = dirFromDiffs(i2 - i1, j2 - j1);
		int di = DIRS[d][0];
		int dj = DIRS[d][1];
		int i = i1 + di;
		int j = j1 + dj;
		int hp = r.currentHistoryPoint();
		int dist1 = r.totalDistance;
		while (true) {
			assert(inBounds(i, j));
			if (i == i2 && j == j2) {
				r.connect(index1, index2);
				return r.totalDistance - dist1; // successful
			}
			auto& cell = r.cells[i][j];
			if (cell.isComputer()) {
				auto& c2 = r.cs[cell.getComputerIndex()];
				// perpendicular dirs
				int d1 = perpDir1(d);
				int d2 = perpDir2(d);
				int cost1 = c2.conns[d2] == -1 ? r.push(c2.index, d1, PushMode::OnlyCost) : INF;
				int cost2 = c2.conns[d1] == -1 ? r.push(c2.index, d2, PushMode::OnlyCost) : INF;

				if (isInf(min(cost1, cost2))) {
					r.undoTo(hp);
					return INF; // failed
				}
				if (cost1 < cost2) {
					r.push(c2.index, d1, PushMode::Confirm);
				} else {
					r.push(c2.index, d2, PushMode::Confirm);
				}
			}
			if (cell.isCable()) {
				r.undoTo(hp);
				return INF; // failed
			}
			i += di;
			j += dj;
		}
	}

	void applyMove(GreedyMove& move) {
		r.applyDiff(move.diff);
	}

	void findCandidatePush(int clusterIndex, GreedyMove& cand, double cableLengthCost) {
		auto& cl = r.cls[clusterIndex];
		int kind = cl.kind;
		int len1 = r.totalCableLengths[kind];

		int maxDelta = (int) (cand.cost + 2);

		for (int index : cl.indices) {
			auto& c = r.cs[index];
			bool canMoveH = c.conns[DIR_U] == -1 && c.conns[DIR_D] == -1;
			bool canMoveV = c.conns[DIR_L] == -1 && c.conns[DIR_R] == -1;

			int delta = 0;

			int hpBase = r.currentHistoryPoint();
			int distBase = r.totalDistance;

			while (true) {
				int forceMoveDir = canMoveH ? (delta < 0 ? DIR_L : DIR_R) : (delta < 0 ? DIR_U : DIR_D);
				int forceMoveCost = delta == 0 ? 0 : r.push(index, forceMoveDir, PushMode::Try);
				int hp = r.currentHistoryPoint();
				if (isInf(forceMoveCost)) {
					if (delta >= 0) {
						delta = -1;
						r.undoTo(hpBase);
					} else {
						r.undoTo(hpBase);
						break;
					}
				}

				int fromI = c.i;
				int fromJ = c.j;

				rep0(d, 4) {
					if (c.conns[d] != -1) continue;
					int di = DIRS[d][0];
					int dj = DIRS[d][1];
					int i = fromI + di;
					int j = fromJ + dj;
					vector<int> pushDirs;
					while (inBounds(i, j)) {
						auto& cell = r.cells[i][j];
						if (cell.isComputer()) {
							auto& c2 = r.cs[cell.getComputerIndex()];
							if (c2.kind == kind && c2.clusterIndex != clusterIndex) {
								int hp2 = r.currentHistoryPoint();
								if (isFinite(forceConnect(index, c2.index))) {
									double cost = (r.totalDistance - distBase) + (r.totalCableLengths[kind] - len1) * cableLengthCost;
									if (isFinite(cost) && (cost < cand.cost || cost <= cand.cost && rnd.nextInt(2) == 0)) {
										cand.cost = cost;
										r.makeDiff(cand.diff, hpBase);
									}
								}
								r.undoTo(hp2);
								break;
							}
						}
						i += di;
						j += dj;
					}
				}
				r.undoTo(hp);

				if (delta >= 0) delta++;
				else delta--;
			}
		}
	}

	void findCandidateJoin(int clusterIndex, bool allowPush, GreedyMove& cand, double cableLengthCost) {
		auto& cl = r.cls[clusterIndex];
		int kind = cl.kind;

		int prev[MAX_N][MAX_N];
		memset(prev, -1, sizeof(prev));

		int len1 = r.totalCableLengths[kind];
		queue<int> q;
		for (int i : cl.indices) {
			auto& c = r.cs[i];
			rep0(d, 4) {
				int di = DIRS[d][0];
				int dj = DIRS[d][1];
				int i = c.i + di;
				int j = c.j + dj;
				if (c.conns[d] == -1) {
					while (inBounds(i, j)) {
						auto& cell = r.cells[i][j];
						if (cell.isCable()) break;
						if (cell.isComputer()) {
							if (!allowPush) break;
							auto& c = r.cs[cell.getComputerIndex()];
							if (c.conns[d] != -1) break; // unpushable
							if (c.conns[perpDir1(d)] != -1 && c.conns[perpDir2(d)] != -1) break; // pushable, but cannot connect
						}
						if (!cell.isEmpty()) break;
						prev[i][j] = c.index; // mark the target
						q.push(i << 8 | j);
						i += di;
						j += dj;
					}
				} else {
					while (inBounds(i, j)) {
						auto& cell = r.cells[i][j];
						if (cell.isComputer()) break; // reached the end
						pii cable = cell.getCableIndices();
						prev[i][j] = cable.first << 16 | cable.second; // mark the targets
						q.push(i << 8 | j);
						i += di;
						j += dj;
					}
				}
			}
		}
		int dist = 0;
		int maxDist = n >> 1;
		vector<pii> targets;
		while (!q.empty()) {
			int sameDistsCount = len(q);
			rep0(iter, sameDistsCount) {
				int pos = q.front();
				q.pop();
				int i = pos >> 8;
				int j = pos & 0xff;
				auto& cell = r.cells[i][j];
				if (cell.isComputer()) {
					auto& c = r.cs[cell.getComputerIndex()];
					if (c.clusterIndex != clusterIndex && c.kind == kind) {
						// found!
						targets.emplace_back(cell.getComputerIndex(), dist);
					}
					continue;
				}
				rep0(d, 4) {
					int i2 = i + DIRS[d][0];
					int j2 = j + DIRS[d][1];
					if (!inBounds(i2, j2)) continue;
					if (prev[i2][j2] == -1) {
						prev[i2][j2] = i << 8 | j;
						q.push(i2 << 8 | j2);
					}
				}
			}
			dist++;
			if (dist > maxDist) break;
		}
		bool res = false;
		//trace("cluster info: kind = ", cl.kind, ", size = ", cl.size);
		for (pii target : targets) {
			if (timer() > TIMEOUT_LIMIT) break;
			int index = target.first;
			int dist = target.second;
			auto& c = r.cs[index];
			//trace("target found! ", index, " at distance ", dist, "(", c.i, ", ", c.j, ") cluster is ", c.clusterIndex);
			int i = c.i;
			int j = c.j;
			set<pii> cables;
			vector<int> dirs;
			rep0(iter, dist) {
				int next = prev[i][j];
				int ni = next >> 8;
				int nj = next & 0xff;
				dirs.push_back(dirFromDiffs(ni - i, nj - j));
				i = ni;
				j = nj;
				if (iter < dist - 1 && r.cells[i][j].isCable()) {
					cables.insert(r.cells[i][j].getCableIndices());
				}
			}
			cablesHatred.insert(cables.begin(), cables.end());

			auto& cell = r.cells[i][j];
			//trace("move to (", i, ", ", j, ")");

			int hp = r.currentHistoryPoint();

			bool joinCable = cell.isCable();
			int target1 = prev[i][j] & 0xffff;
			int target2 = prev[i][j] >> 16;

			r.cs[target1].fixed = true;
			if (joinCable) r.cs[target2].fixed = true;

			double cost = 0;
			int cutCable = -1;
			int minI;
			int minJ;
			int maxI;
			int maxJ;
			rep0(dirIndex, dist) {
				bool lastMove = dirIndex == dist - 1;
				if (joinCable && lastMove) {
					pii cable = cell.getCableIndices();
					r.disconnect(cable.first, cable.second);
				}

				defineDirDiffs(dirs[dirIndex], di, dj);
				int ni = c.i + di;
				int nj = c.j + dj;
				auto& nextCell = r.cells[ni][nj];
				int nextCutCable = -1;
				if (!lastMove && nextCell.isCable()) {
					pii cable = nextCell.getCableIndices();
					nextCutCable = cable.first << 16 | cable.second;
					r.disconnect(cable.first, cable.second);
				}

				cost += r.push(index, dirs[dirIndex], PushMode::Try);
				if (isInf(cost)) break;

				if (cutCable != -1) {
					if (c.i < minI || c.i > maxI || c.j < minJ || c.j > maxJ) {
						r.connect(cutCable >> 16, cutCable & 0xffff);
						cutCable = -1;
					}
				}

				if (nextCutCable != -1) {
					assert(!lastMove);
					assert(cutCable == -1);
					int index1 = nextCutCable >> 16;
					int index2 = nextCutCable & 0xffff;
					auto& c1 = r.cs[index1];
					auto& c2 = r.cs[index2];
					minI = min(c1.i, c2.i);
					maxI = max(c1.i, c2.i);
					minJ = min(c1.j, c2.j);
					maxJ = max(c1.j, c2.j);
					cutCable = nextCutCable;
				}
			}
			assert(cutCable == -1);
			if (isFinite(cost)) {
				if (joinCable) {
					r.connect(index, target1);
					r.connect(index, target2);
				} else {
					cost += forceConnect(index, prev[i][j]);
				}
			}
			cost += (r.totalCableLengths[kind] - len1) * cableLengthCost;
			if (isFinite(cost) && (cost < cand.cost || cost <= cand.cost && rnd.nextInt(2) == 0)) {
				cand.cost = cost;
				r.makeDiff(cand.diff, hp);
			}

			r.cs[target1].fixed = false;
			if (joinCable) r.cs[target2].fixed = false;

			r.undoTo(hp);
		}
	}

	void neighborConnect() {
		int index1;
		int dir;
		do {
			index1 = rnd.nextInt(100 * k);
			dir = rnd.nextInt(4);
		} while (r.cs[index1].conns[dir] != -1);

		auto& c = r.cs[index1];
		int di = DIRS[dir][0];
		int dj = DIRS[dir][1];
		int i = c.i + di;
		int j = c.j + dj;

		double cutProb = 0.5;
		double stopProb = 0.75;
		double robProb = 0.25;
		vector<pii> cutList;

		while (inBounds(i, j)) {
			auto& cell = r.cells[i][j];
			if (cell.isComputer()) {
				int index2 = cell.getComputerIndex();

				// cut links in the way
				for (pii cut : cutList) {
					r.disconnect(cut.first, cut.second);
				}
				cutList.clear();

				if (rnd.nextFloat() < robProb) {
					// rob the computer
					r.isolate(index2);
				}

				r.connect(index1, index2);

				if (rnd.nextFloat() < stopProb || r.cs[index2].conns[dir] != -1) {
					// finish connecting
					break;
				}

				index1 = index2;
			} else if (cell.isCable()) {
				if (rnd.nextFloat() < cutProb) {
					// decide to cut the link
					cutList.push_back(cell.getCableIndices());
				} else {
					// give up
					break;
				}
			}
			i += di;
			j += dj;
		}
	}

	void neighborDisconnect() {
		int index1;
		int d;
		do {
			index1 = rnd.nextInt(100 * k);
			d = rnd.nextInt(4);
		} while (r.cs[index1].conns[d] == -1);
		r.disconnect(index1, r.cs[index1].conns[d]);
	}

	void neighborMove() {
		double backProb = r.numConnections + r.totalDistance > 100 * k ? 1 : 0.5;

		bool back;
		int index;
		bool moved;
		int trial = 0;
		do {
			back = rnd.nextFloat() < backProb;
			index = rnd.nextInt(100 * k);
			auto& c = r.cs[index];
			moved = c.i != c.origI || c.j != c.origJ;
			if (++trial >= 50) return;
		} while (back && !moved);

		auto& c = r.cs[index];

		int d;
		int di;
		int dj;
		do {
			d = rnd.nextInt(4);
			di = DIRS[d][0];
			dj = DIRS[d][1];
		} while (
			!inBounds(c.i + di, c.j + dj) ||
			(back && (
				isHorizontal(d) && (c.j == c.origJ) ||
				isVertical(d) && (c.i == c.origI))
				)
			);
		int dist1 = abs(c.i - c.origI) + abs(c.j - c.origJ);
		int dist2 = abs(c.i + di - c.origI) + abs(c.j + dj - c.origJ);
		if (back && dist2 > dist1) {
			d = flipDir(d);
			di = DIRS[d][0];
			dj = DIRS[d][1];
		}
		int ni = c.i + di;
		int nj = c.j + dj;
		auto& cell = r.cells[ni][nj];
		if (cell.isComputer()) return; // cannot move
		vector<pii> connList;
		if (isHorizontal(d)) {
			if (c.conns[DIR_L] != -1) connList.emplace_back(index, c.conns[DIR_L]);
			if (c.conns[DIR_R] != -1) connList.emplace_back(index, c.conns[DIR_R]);
		} else {
			if (c.conns[DIR_U] != -1) connList.emplace_back(index, c.conns[DIR_U]);
			if (c.conns[DIR_D] != -1) connList.emplace_back(index, c.conns[DIR_D]);
		}
		r.isolate(index);
		if (cell.isCable()) {
			pii cable = cell.getCableIndices();
			r.disconnect(cable.first, cable.second);
			connList.emplace_back(index, cable.first);
			connList.emplace_back(index, cable.second);
		}
		r.move(index, d);
		for (pii conn : connList) {
			r.connect(conn.first, conn.second);
		}
	}
};


int main(int argc, char* argv[]) {
#if 1
	int num = 50;
	int from = 0;
	int stride = 1;
	int single = 0;

	int seeds[1000] = {};
	int times[1000] = {};
	int scores[1000] = {};

	int scoreSum = 0;

	vector<int> seedList = {};
	bool useList = false;

	if (!seedList.empty()) {
		useList = true;
		num = len(seedList);
		single = -1;
	}

	if (argc > 1) {
		useList = false;
		num = atoi(argv[1]);
		from = argc > 2 ? atoi(argv[2]) : 0;
		stride = argc > 3 ? atoi(argv[3]) : 1;
		single = -1;
	}

	rep0(t, num) {
		timer(true);
		int seed = useList ? seedList[t] : single != -1 ? single : from + t * stride;
		trace("\n------------ SOLVING SEED ", seed, " ------------");
		//tracen(seed, ": ");
		Solver s;
		s.init(cin);
		s.solve();
		if (single == -1) trace("score: ", s.res.score);
		scoreSum += s.res.score;
		seeds[t] = seed;
		scores[t] = s.res.score;
		times[t] = timer();
		if (single != -1) break;
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
		trace("time:");
		rep0r(t, num) {
			int i = timesRank[t].second;
			int s = seeds[i];
			string space = s < 10 ? "   " : s < 100 ? "  " : s < 1000 ? " " : "";
			trace("  seed ", space, s, "(k=", s % 4 + 2, "): ", times[i], " ms (score ", scores[i], ")");
		}
		trace("score:");
		rep0r(t, num) {
			int i = scoresRank[t].second;
			int s = seeds[i];
			string space = s < 10 ? "   " : s < 100 ? "  " : s < 1000 ? " " : "";
			trace("  seed ", space, s, "(k=", s % 4 + 2, "): ", scores[i], " (time ", times[i], " ms)");
		}
		trace("total score: ", scoreSum, ", mean: ", (int) (scoreSum / (double) num * 100) / 100.0);
	} else {
		trace("total score: ", scoreSum, " time = ", timer());
	}

#elif 1
	timer(true);
	Solver s;
	s.read(cin);
	s.solve();
#elif 1
#endif
	return 0;
}

// seed   15(k=5): 10090 (time 3238 ms)
// seed   27(k=5): 10071 (time 3065 ms)
// seed   11(k=5): 10036 (time 3198 ms)
// seed   47(k=5): 10005 (time 3058 ms)
// seed   19(k=5): 9966 (time 3228 ms)
// seed   39(k=5): 9928 (time 3067 ms)
// seed   23(k=5): 9921 (time 3060 ms)
// seed    3(k=5): 9921 (time 3212 ms)
// seed   22(k=4): 9906 (time 3052 ms)
// seed   14(k=4): 9516 (time 3152 ms)
// seed   43(k=5): 9139 (time 3061 ms)
// seed   31(k=5): 8983 (time 3115 ms)
// seed   38(k=4): 8869 (time 3048 ms)
// seed    6(k=4): 8691 (time 3161 ms)
// seed   35(k=5): 8620 (time 3062 ms)
// seed   42(k=4): 8523 (time 3031 ms)
// seed   10(k=4): 8439 (time 3158 ms)
// seed    2(k=4): 8131 (time 3147 ms)
// seed   33(k=3): 8031 (time 3022 ms)
// seed   45(k=3): 7877 (time 3014 ms)
// seed    1(k=3): 7801 (time 3093 ms)
// seed   21(k=3): 7512 (time 3032 ms)
// seed    5(k=3): 7512 (time 3115 ms)
// seed   29(k=3): 7436 (time 3017 ms)
// seed   49(k=3): 7393 (time 3028 ms)
// seed   17(k=3): 7299 (time 3134 ms)
// seed   18(k=4): 7033 (time 3178 ms)
// seed   24(k=2): 7030 (time 2996 ms)
// seed   13(k=3): 7030 (time 3094 ms)
// seed   41(k=3): 6969 (time 3021 ms)
// seed   44(k=2): 6661 (time 3001 ms)
// seed   48(k=2): 6546 (time 2997 ms)
// seed   12(k=2): 6546 (time 3063 ms)
// seed    0(k=2): 6546 (time 3040 ms)
// seed   28(k=2): 6490 (time 3000 ms)
// seed    8(k=2): 6490 (time 3050 ms)
// seed    4(k=2): 6276 (time 3057 ms)
// seed   16(k=2): 5896 (time 3051 ms)
// seed   34(k=4): 5881 (time 3038 ms)
// seed   20(k=2): 5770 (time 3049 ms)
// seed   26(k=4): 5731 (time 3038 ms)
// seed   36(k=2): 5691 (time 2998 ms)
// seed   32(k=2): 5691 (time 3000 ms)
// seed   46(k=4): 5455 (time 3040 ms)
// seed   30(k=4): 5421 (time 3046 ms)
// seed    9(k=3): 5341 (time 3095 ms)
// seed   40(k=2): 5328 (time 2996 ms)
// seed    7(k=5): 5217 (time 3199 ms)
// seed   25(k=3): 5107 (time 3020 ms)
// seed   37(k=3): 5082 (time 3009 ms)
// total score: 374844, mean: 7496.88
//              -> 375541