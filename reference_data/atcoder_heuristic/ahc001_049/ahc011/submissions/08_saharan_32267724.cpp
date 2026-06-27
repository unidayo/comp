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

#define NDEBUG
//#undef NDEBUG
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

constexpr int MIN_N = 6;
constexpr int MAX_N = 10;

constexpr int CELL_EMPTY = 0;
constexpr int CELL_LEFT = 1;
constexpr int CELL_RIGHT = 4;
constexpr int CELL_UP = 2;
constexpr int CELL_DOWN = 8;
constexpr int CELL_FULL = 15;
constexpr int CELL_NULL = 16;

constexpr int INF = 10000000;

constexpr bool PRINT_SUBGAME_INFO = false;
constexpr bool PRINT_ENDGAME = false;
constexpr bool PRINT_SUBGAME_TIMES = false;
constexpr bool PRINT_PIECE_FILLING_INFO = false;
constexpr bool PRINT_PROGRESS = false;
constexpr bool PRINT_BEST_UPDATE = false;
constexpr bool PRINT_BLOCK_RESULT = false;
constexpr bool PRINT_MOVE_COUNTS = false;

#define rep2() rep0(i, n)rep0(j, n)

class Cell {
public:
	char val;

	Cell() : val(CELL_NULL) {
	}

	Cell(char val) : val(val) {
	}

	Cell(bool left, bool right, bool up, bool down) : val((left ? CELL_LEFT : 0) | (right ? CELL_RIGHT : 0) | (up ? CELL_UP : 0) | (down ? CELL_DOWN : 0)) {
	}

	bool left() const {
		return (val & CELL_LEFT) != 0;
	}

	bool right() const {
		return (val & CELL_RIGHT) != 0;
	}

	bool up() const {
		return (val & CELL_UP) != 0;
	}

	bool down() const {
		return (val & CELL_DOWN) != 0;
	}

	bool dir(int di, int dj) const {
		if (dj == -1) return left();
		if (dj == 1) return right();
		if (di == -1) return up();
		if (di == 1) return down();
		assert(false);
	}

	bool empty() const {
		return val == CELL_EMPTY;
	}

	bool null() const {
		return val == CELL_NULL;
	}

	array<string, 3> toString() const {
		array<string, 3> res = { "     ", "     ", "     " };
		if (null()) {
			res[1][2] = '?';
			return res;
		}
		if (empty()) {
			return res;
		}
		res[1][2] = '*';
		if (left())
			res[1][0] = res[1][1] = '-';
		if (right())
			res[1][3] = res[1][4] = '-';
		if (up())
			res[0][2] = '|';
		if (down())
			res[2][2] = '|';
		return res;
	}
};

class Board {
public:
	int n = 0;
	Cell data[MAX_N][MAX_N] = {};

	Cell get(int i, int j) const {
		return data[i][j];
	}

	void set(int i, int j, Cell c) {
		data[i][j] = c;
	}

	void read(int seed) {
		auto res = readText("in/"s + (seed < 10 ? "000" : seed < 100 ? "00" : seed < 1000 ? "0" : "") + to_string(seed) + ".txt");
		istringstream ist(res.first);
		read(ist);
	}

	void clear(int n) {
		this->n = n;
		memset(data, CELL_NULL, sizeof(data));
	}

	void read(istream& in) {
		in >> n;
		clear(n);
		int t;
		in >> t;
		rep2() {
			char c;
			in >> c;
			set(i, j, Cell((int) "0123456789abcdef"s.find(c)));
		}
	}

	void print() const {
		rep0(i, n) {
			vector<array<string, 3>> line;
			rep0(j, n) {
				line.push_back(get(i, j).toString());
			}
			rep0(k, 3) {
				rep0(j, n) {
					tracen(line[j][k]);
				}
				trace();
			}
		}
	}

	void rotate() {
		Board tmp = *this;
		int m = n - 1;
		rep2() {
			Cell c = tmp.data[j][m - i];
			data[i][j] = Cell(c.up(), c.down(), c.right(), c.left());
		}
	}

	void flipJ() {
		Board tmp = *this;
		int m = n - 1;
		rep2() {
			Cell c = tmp.data[i][m - j];
			data[i][j] = Cell(c.right(), c.left(), c.up(), c.down());
		}
	}
};

class Transform {
public:
	int rot = 0;
	bool flip = false;

	Transform() {
	}

	Transform(int rot, bool flip) : rot(rot), flip(flip) {
	}

	Transform(int index) : rot(index % 4), flip(index / 4 % 2 != 0) {
	}

	void applyBoard(Board& b) {
		if (flip) {
			b.flipJ();
		}
		rep0(i, rot) {
			b.rotate();
		}
	}

	void applyString(string& s) {
		for (char& c : s) {
			rep0(i, rot) {
				switch (c) {
				case 'L':
					c = 'U';
					break;
				case 'R':
					c = 'D';
					break;
				case 'U':
					c = 'R';
					break;
				case 'D':
					c = 'L';
					break;
				}
			}
			if (flip) {
				c = c == 'L' ? 'R' : c == 'R' ? 'L' : c;
			}
		}
	}
};


class Piece {
public:
	int i = 0;
	int j = 0;
	int gi = 0;
	int gj = 0;

	void set(int i, int j, int gi, int gj) {
		this->i = i;
		this->j = j;
		this->gi = gi;
		this->gj = gj;
	}

	int dist() {
		return abs(gi - i) + abs(gj - j);
	}
};

class AStarNode {
public:
	short f = 0;
	ull prev = 0;
	bool open = true;

	AStarNode() : f(0), prev(-1), open(true) {
	}

	AStarNode(short f, ull prev) : f(f), prev(prev), open(true) {
	}

	void init(int f, ull prev) {
		this->f = f;
		this->prev = prev;
		open = true;
	}
};

constexpr int MAX_STATES = 0x1000000;

class NodesArray {
public:
	NodesArray() {
		nodes = new AStarNode[MAX_STATES];
		times = new int[MAX_STATES];
	}

	~NodesArray() {
		delete[] nodes;
		delete[] times;
	}

	void clear() {
		time++;
	}

	bool exists(ull key) {
		return times[key] == time;
	}

	void set(ull key, AStarNode node) {
		times[key] = time;
		nodes[key] = node;
	}

	AStarNode& get(ull key) {
		return nodes[key];
	}

private:
	AStarNode* nodes;
	int* times;
	int time = 1;
};


class SolverResult {
public:
	int steps = 0;
	vector<pii> deltas;
};

constexpr int MAX_SUBGAME_PIECES = 12;
using SubGamePieces = array<Piece, MAX_SUBGAME_PIECES>;


class SubGameSolver {
public:
	SubGamePieces ps;
	int numPieces = 0;
	bool walls[MAX_N][MAX_N] = {};
	int n = 0;
	SolverResult res;

	SubGameSolver() {
	}

	~SubGameSolver() {
	}

private:
	bool shrinkedWalls[MAX_N][MAX_N] = {};
	int poss[MAX_N * MAX_N][2] = {};
	int posIndices[MAX_N][MAX_N] = {};
	bool corePath[MAX_N][MAX_N] = {};
	int occupation[MAX_N][MAX_N] = {};
	int dist = 0;
	ull currentState = 0;
	int numPos = 0;
	int posShift = 0;
	ull posMask = 0;
public:

	void solve(int max) {
		bool solvable = checkSolvable();
		assert(solvable);
		search(max);
	}

	bool checkSolvable() {
		rep2() {
			corePath[i][j] = false;
			occupation[i][j] = 0;
		}
		rep0(idx, numPieces) {
			Piece& p = ps[idx];
			if (idx > 0) {
				occupation[p.i][p.j] = idx;
			}
			assert(!walls[p.i][p.j]);
		}
		computeCorePath();
		shrinkWalls();
		computeIndices();
		if (posShift * numPieces > 63) {
			return false;
		}
		return true;
	}

	void print() {
		char b[MAX_N][MAX_N] = {};
		rep2() {
			b[i][j] = shrinkedWalls[i][j] ? '.' : ' ';
		}
		rep0(i, numPieces) {
			Piece& p = ps[i];
			b[p.i][p.j] = '0' + i;
		}
		trace("--");
		rep2() {
			tracen(b[i][j]);
			if (j == n - 1) trace();
		}
		trace("--");
	}

private:
	//AStarNode* nodes_old;
	ull lastState = -1;
	int lastSwap = 0;
	int lastDist = 0;
	unordered_map<ull, AStarNode> nodes;

	NodesArray nodesArray;

	void search(int max) {
		nodes.clear();
		nodesArray.clear();
		//timer(true);

		constexpr int MAX_MOVES = 400;

		static int time = 0;

		time++;

		res.steps = -1;
		res.deltas.clear();

		if (PRINT_SUBGAME_INFO) {
			print();
		}

		vector<ull> indices[MAX_MOVES];

		computeState();
		ull start = currentState;
		loadState(start);
		computeState();
		assert(currentState == start);

		bool useHash = numPieces * posShift > log2(MAX_STATES);

		if (useHash)
			nodes.emplace(start, AStarNode(dist, -1));
		else
			nodesArray.set(start, AStarNode(dist, -1));

		int targetF = dist;

		indices[dist].push_back(start);

		ull goal = -1;
		int trial = 0;

		while (true) {
			while (indices[targetF].empty()) {
				targetF++;
				if (targetF == MAX_MOVES) break;
			}
			if (targetF > max || targetF == MAX_MOVES) {
				if (PRINT_SUBGAME_INFO) {
					trace("could not find............");
				}
				return;
			}
			if (++trial % 10000 == 0 && timer() > 2850) {
				trace("subame alarm");
				return;
			}
			ull state = indices[targetF].back();
			indices[targetF].pop_back();

			auto& n = useHash ? nodes[state] : nodesArray.get(state);
			if (!n.open) {
				//trace("not open");
				continue;
			}
			n.open = false;

			int f = targetF;
			loadState(state);
			if (dist == 0) {
				if (PRINT_SUBGAME_INFO) {
					trace("found! f: ", f, ", state: ", state);
				}
				goal = state;
				break;
			}
			int g = f - dist;

			const int dis[4] = { -1, 1, 0, 0 };
			const int djs[4] = { 0, 0, -1, 1 };
			rep0(dir, 4) {
				int di = dis[dir];
				int dj = djs[dir];
				Piece& empty = ps[0];
				if (!move(di, dj)) continue;
				int f2 = g + 1 + dist;
				ull state2 = currentState;

				if (useHash) {
					if (nodes.count(state2) == 0) {
						nodes.emplace(state2, AStarNode(f2, state));
						indices[f2].push_back(state2);
					} else {
						auto& n2 = nodes[state2];
						if (f2 < n2.f) {
							n2.init(f2, state);
							indices[f2].push_back(state2);
						}
					}
				} else {
					if (!nodesArray.exists(state2)) {
						nodesArray.set(state2, AStarNode(f2, state));
						indices[f2].push_back(state2);
					} else {
						auto& n2 = nodesArray.get(state2);
						if (f2 < n2.f) {
							n2.init(f2, state);
							indices[f2].push_back(state2);
						}
					}
				}

				//if (!nodes.exists(state2)) {
				//	nodes.set(state2, AStarNode(f2, state));
				//	indices[f2].push_back(state2);
				//} else {
				//	auto n2 = nodes.get(state2);
				//	if (f2 < n2.f) {
				//		n2.init(f2, state);
				//		indices[f2].push_back(state2);
				//	}
				//}


				//auto& n2 = nodes_old[state2];
				//if (!n2.visited(time) || f2 < n2.f) { // update
				//	n2.init(time, f2, state);
				//	indices[f2].push_back(state2);
				//}
				undoMove(di, dj);
			}
		}
		//trace("time: ", timer());
		ull s = goal;
		vector<ull> path;
		while (s != -1) {
			path.push_back(s);
			s = useHash ? nodes[s].prev : nodesArray.get(s).prev;
		}
		reverse(path.begin(), path.end());
		//trace("ans:");
		res.steps = len(path) - 1;
		int pi = -1;
		int pj = -1;
		for (ull st : path) {
			loadState(st);
			int i = ps[0].i;
			int j = ps[0].j;
			if (pi != -1) {
				res.deltas.emplace_back(i - pi, j - pj);
			}
			pi = i;
			pj = j;
			//print();
		}
		loadState(start);
	}

	bool move(int di, int dj) {
		Piece& empty = ps[0];
		int pi = empty.i;
		int pj = empty.j;
		int i = pi + di;
		int j = pj + dj;
		if (i < 0 || i >= n || j < 0 || j >= n) return false;
		if (shrinkedWalls[i][j]) return false;
		empty.i += di;
		empty.j += dj;
		lastDist = dist;
		lastState = currentState;
		currentState &= ~posMask;
		currentState |= posIndices[i][j];
		int idx = occupation[i][j];
		lastSwap = idx;
		if (idx != 0) {
			occupation[pi][pj] = idx;
			occupation[i][j] = 0;
			Piece& p = ps[idx];
			dist -= p.dist();
			p.i -= di;
			p.j -= dj;
			dist += p.dist();
			const int shift = posShift * idx;
			const ull mask = posMask << shift;
			currentState &= ~mask;
			currentState |= (ull) posIndices[p.i][p.j] << shift;
		}
		return true;
	}

	void undoMove(int di, int dj) {
		Piece& empty = ps[0];
		empty.i -= di;
		empty.j -= dj;
		if (lastSwap != 0) {
			Piece& p = ps[lastSwap];
			occupation[p.i][p.j] = 0;
			p.i += di;
			p.j += dj;
			occupation[p.i][p.j] = lastSwap;
		}
		dist = lastDist;
		currentState = lastState;
	}

	void computeState() {
		currentState = 0;
		int shift = 0;
		rep0(idx, numPieces) {
			Piece& p = ps[idx];
			currentState |= (ull) posIndices[p.i][p.j] << shift;
			shift += posShift;
		}
	}

	void loadState(ull newState) {
		currentState = newState;
		dist = 0;
		rep(idx, 1, numPieces) {
			Piece& p = ps[idx];
			occupation[p.i][p.j] = 0;
		}
		rep0(idx, numPieces) {
			ull pidx = newState & posMask;
			Piece& p = ps[idx];
			p.i = poss[pidx][0];
			p.j = poss[pidx][1];
			if (idx > 0) {
				dist += p.dist();
				occupation[p.i][p.j] = idx;
			}
			newState >>= posShift;
		}
	}

	void computeIndices() {
		numPos = 0;
		rep2() {
			if (shrinkedWalls[i][j]) continue;
			posIndices[i][j] = numPos;
			poss[numPos][0] = i;
			poss[numPos][1] = j;
			numPos++;
		}
		posShift = log2Ceil(numPos);
		posMask = (1 << posShift) - 1;
	}

	int log2Ceil(int x) {
		x--;
		x |= x | x >> 1;
		x |= x | x >> 2;
		x |= x | x >> 4;
		x |= x | x >> 8;
		return popcount32(x | x >> 16);
	}

	void shrinkWalls() {
		int minI = ps[0].i;
		int minJ = ps[0].j;
		int maxI = ps[0].i;
		int maxJ = ps[0].j;
		rep(idx, 1, numPieces) {
			Piece& p = ps[idx];
			minI = min(minI, min(p.i, p.gi));
			minJ = min(minJ, min(p.j, p.gj));
			maxI = max(maxI, max(p.i, p.gi));
			maxJ = max(maxJ, max(p.j, p.gj));
		}
		bool makeWallTightI = maxI - minI + 1 >= 3;
		bool makeWallTightJ = maxJ - minJ + 1 >= 3;

		rep2() {
			bool wall = walls[i][j];
			//if ((i < minI || i > maxI) && makeWallTightI) wall = true;
			//if ((j < minJ || j > maxJ) && makeWallTightJ) wall = true;
			if (wall) {
				shrinkedWalls[i][j] = true;
				continue;
			}
			bool pathFound = false;
			rep(di, -1, 2) {
				rep(dj, -1, 2) {
					int i2 = i + di;
					int j2 = j + dj;
					if (i2 < 0 || i2 >= n || j2 < 0 || j2 >= n) continue;
					if (corePath[i2][j2]) {
						pathFound = true;
					}
				}
			}
			shrinkedWalls[i][j] = !pathFound;
		}
	}

	void computeCorePath() {
		// pos of empty
		int i0 = ps[0].i;
		int j0 = ps[0].j;
		// pos of the other
		rep(idx, 1, numPieces) {
			Piece& p = ps[idx];
			int i = p.i;
			int j = p.j;
			int gi = p.gi;
			int gj = p.gj;
			drawCoreRect(i, j, gi, gj);
			drawCoreRect(i, j, i0, j0);
		}
	}

	void drawCoreRect(int i1, int j1, int i2, int j2) {
		int minI = min(i1, i2);
		int minJ = min(j1, j2);
		int maxI = max(i1, i2);
		int maxJ = max(j1, j2);
		rep(i, minI, maxI + 1) {
			corePath[i][minJ] = true;
			corePath[i][maxJ] = true;
		}
		rep(j, minJ, maxJ + 1) {
			corePath[minI][j] = true;
			corePath[maxI][j] = true;
		}
	}
public:

};

class EndGameSolver {
public:
	int n = 0;
	char goal[MAX_N][MAX_N] = {};
	Piece ps[128];
	char b[MAX_N][MAX_N] = {}; // 0: empty, -1: null, -2: fixed
	SolverResult res;

private:
	int dist = 0;
	int maxNum = 0;
	int maxF = 0;
	vector<pii> deltas;
public:

	void solve(int maxCount) {
		assert(n <= MAX_N);
		rep0(i, 128) {
			Piece& p = ps[i];
			p.i = -1;
			p.j = -1;
			p.gi = -1;
			p.gj = -1;
		}
		maxNum = -1;
		vector<int> seqFrom;
		vector<int> seqTo;
		rep2() {
			int idx;
			if ((idx = b[i][j]) >= 0) {
				Piece& p = ps[idx];
				assert(p.i == -1);
				p.i = i;
				p.j = j;
				seqFrom.push_back(idx);
				maxNum = max(maxNum, idx);
			}
			if ((idx = goal[i][j]) >= 0) {
				Piece& p = ps[idx];
				assert(p.gi == -1);
				p.gi = i;
				p.gj = j;
				seqTo.push_back(idx);
				maxNum = max(maxNum, idx);
			}
		}
		maxNum++;
		computeDist();
		assert(len(seqFrom) == maxNum && len(seqTo) == maxNum);
		int parity = 0;
		rep0(i, maxNum) {
			int to = seqTo[i];
			if (seqFrom[i] != to) {
				rep(j, i + 1, maxNum) {
					if (seqFrom[j] == to) {
						swap(seqFrom[i], seqFrom[j]);
						parity ^= 1;
						break;
					}
				}
			}
		}
		res.deltas.clear();
		res.steps = -1;
		bool solvable = ps[0].dist() % 2 == parity;
		if (!solvable) {
			//trace("unsolvable!");
		} else {
			//trace("solvable!");
			//print();
			//search(maxCount);
			search(maxCount);
		}
	}

private:

	void search(int maxCount) {
		maxF = dist;
		int iter = 0;
		int st = timer();
		while (true) {
			iter++;
			//trace("iter ", iter, ": max F = ", maxF);
			if (maxF > maxCount) {
				if (PRINT_ENDGAME) {
					tracen("could not find... (F = ", maxF, ") time = ");
				}
				break;
			}
			maxF = searchIDA(0, ps[0].i, ps[0].j, 0, 0);
			if (maxF == 0) {
				break;
			}
			if (timer() > 2850) {
				trace("alarm endgame (F = ", maxF, ") ", timer());
				return;
			}
		}
		if (PRINT_ENDGAME) {
			trace(timer() - st, " ms");
		}
	}

	void print() {
		trace("current:");
		rep2() {
			if (j > 0) {
				tracen("  ");
			}
			tracen(b[i][j] == -2 ? "." : b[i][j] == -1 ? " " : to_string(b[i][j]));
			if (j == n - 1) {
				trace();
			}
		}
		trace("goal:");
		rep2() {
			if (j > 0) {
				tracen("  ");
			}
			tracen(goal[i][j] <= 0 ? " " : to_string(goal[i][j]));
			if (j == n - 1) {
				trace();
			}
		}
	}

	int searchIDA(int g, int i, int j, int pdi, int pdj) {
		int f = g + dist;
		if (f > maxF) {
			return f;
		}
		if (dist == 0) {
			if (PRINT_ENDGAME) {
				tracen("found! moves = ", g, " time = ");
			}
			res.deltas = deltas;
			res.steps = g;
			return 0; // found
		}
		int best = INF;

		do {
			if (i > 0 && pdi != 1 && move(i, j, -1, 0)) {
				deltas.emplace_back(-1, 0);
				int t = searchIDA(g + 1, i - 1, j, -1, 0);
				deltas.pop_back();
				best = min(best, t);
				move(i - 1, j, 1, 0);
				if (best == 0) break;
			}
			if (i < n - 1 && pdi != -1 && move(i, j, 1, 0)) {
				deltas.emplace_back(1, 0);
				int t = searchIDA(g + 1, i + 1, j, 1, 0);
				deltas.pop_back();
				best = min(best, t);
				move(i + 1, j, -1, 0);
				if (best == 0) break;
			}
			if (j > 0 && pdj != 1 && move(i, j, 0, -1)) {
				deltas.emplace_back(0, -1);
				int t = searchIDA(g + 1, i, j - 1, 0, -1);
				deltas.pop_back();
				best = min(best, t);
				move(i, j - 1, 0, 1);
				if (best == 0) break;
			}
			if (j < n - 1 && pdj != -1 && move(i, j, 0, 1)) {
				deltas.emplace_back(0, 1);
				int t = searchIDA(g + 1, i, j + 1, 0, 1);
				deltas.pop_back();
				best = min(best, t);
				move(i, j + 1, 0, -1);
				if (best == 0) break;
			}
		} while (false);
		return best;
	}

	bool move(int i, int j, int di, int dj) {
		assert(b[i][j] == 0);
		assert(i + di >= 0 && i + di < n);
		assert(j + dj >= 0 && j + dj < n);
		int idx = b[i + di][j + dj];
		if (idx == -2) return false; // fixed, cannot move
		swap(b[i][j], b[i + di][j + dj]);
		Piece& empty = ps[0];
		if (idx == -1) {
			empty.i += di;
			empty.j += dj;
		} else {
			assert(idx > 0);
			Piece& p = ps[idx];
			dist -= p.dist();
			empty.i += di;
			empty.j += dj;
			p.i -= di;
			p.j -= dj;
			dist += p.dist();
		}
		int tmp = dist;
		return true;
	}

	void computeDist() {
		dist = 0;
		rep(idx, 1, maxNum) {
			Piece& p = ps[idx];
			assert(p.i != -1);
			assert(p.gi != -1);
			dist += p.dist();
		}
	}
};

class TreeBuilder {
public:
	Board input;
	Board built;
	bool noEmptyAt[MAX_N][MAX_N] = {};
	vector<Board> builtAll;

	void init(Board& initial, Board& fixed) {
		input = fixed;
		n = initial.n;
		rep0(i, 16) {
			counts[i] = 0;
		}
		rep2() {
			Cell c = initial.get(i, j);
			counts[c.val]++;
		}
		rep2() {
			Cell c = fixed.get(i, j);
			if (!c.null()) {
				counts[c.val]--;
			}
		}
	}

	bool build(int endI = -1, int endJ = -1, int maxIter = 100) {
		// increment pos
		endI = endI == -1 ? n - 1 : endI;
		endJ = endJ == -1 ? n - 1 : endJ;
		endJ = (endJ + 1) % n;
		endI = endJ == 0 ? endI + 1 : endI;
		this->endI = endI;
		this->endJ = endJ;

		int indices[MAX_N] = {};
		int sticks[MAX_N] = {};
		vector<int> unusedIndices;
		rep0(i, n) {
			unusedIndices.push_back(i);
		}
		built.clear(n);

		//timer(true);
		bool ok = false;
		rep0(iter, maxIter) {
			count = 0;
			b = input;
			if (buildTreeDfs(0, 0, 0, counts, indices, sticks, unusedIndices)) {
				ok = true;
				break;
			}
			if (timer() > 2850) {
				trace("build alarm");
				break;
			}
		}
		//trace(timer(), "ms");
		return ok;
	}

	void buildAll(int maxCount) {
		endI = n;
		endJ = 0;

		int indices[MAX_N] = {};
		int sticks[MAX_N] = {};
		vector<int> unusedIndices;
		rep0(i, n) {
			unusedIndices.push_back(i);
		}
		builtAll.clear();
		b = input;
		alarm = false;
		buildTreeDfsAll(0, 0, 0, counts, indices, sticks, unusedIndices, maxCount);
	}

private:
	int n = 0;
	int counts[16] = {};
	int count = 0;
	int endI = 0;
	int endJ = 0;
	bool alarm = false;
	Rnd rnd;
	Board b;

	bool buildTreeDfs(int i, int j, int numPlacedNonEmpty, int counts[16], int indices[MAX_N], int sticks[MAX_N], vector<int>& unusedIndices) {
		constexpr int MAX_COUNT = 50000;

		if (i == endI && j == endJ) {
			built = b;
			return true;
		}

		if (++count >= MAX_COUNT) {
			//trace("fail!");
			//b.print();
			return false;
		}

		if (i < n - 1) {
			int bottoms = 0;
			int bottomCount = 0;
			rep0(j, n) {
				bottomCount += (int) b.get(n - 1, j).null();
			}
			rep0(idx, 16) {
				if (!Cell(idx).down()) {
					bottoms += counts[idx];
				}
			}
			if (bottoms < bottomCount) {
				return false;
			}
		}

		int left = 2; // 0: never, 1: must, 2: free
		int right = 2;
		int up = 2;
		int down = 2;

		Cell tmp;
		if (j == 0) {
			left = 0;
		} else if (!(tmp = b.get(i, j - 1)).null()) {
			left = (int) tmp.right();
		}
		if (j == n - 1) {
			right = 0;
		} else if (!(tmp = b.get(i, j + 1)).null()) {
			right = (int) tmp.left();
		}
		if (i == 0) {
			up = 0;
		} else if (!(tmp = b.get(i - 1, j)).null()) {
			up = (int) tmp.down();
		}
		if (i == n - 1) {
			down = 0;
		} else if (!(tmp = b.get(i + 1, j)).null()) {
			down = (int) tmp.up();
		}

		pii bests[2] = {};
		int numTrials = 0;
		bool fixed = !b.get(i, j).null();
		int fixedPiece = b.get(i, j).val;
		int pieceCount = fixed ? 1 : 16;

		rep0(idx, pieceCount) {
			if (fixed) idx = fixedPiece;
			else if (counts[idx] == 0 || idx == 0 && noEmptyAt[i][j]) continue;

			Cell c(idx);
			if (left == 0 && c.left() || left == 1 && !c.left()) continue;
			if (right == 0 && c.right() || right == 1 && !c.right()) continue;
			if (up == 0 && c.up() || up == 1 && !c.up()) continue;
			if (down == 0 && c.down() || down == 1 && !c.down()) continue;


			if (c.empty()) {
			} else if (c.left() && c.up()) { // trying to merge groups
				int gidxL = indices[j - 1];
				int gidxU = indices[j];
				if (gidxL == gidxU) { // loop
					continue;
				}
				int stickDelta = -((int) c.left() + (int) c.up()) + ((int) c.right() + (int) c.down());
				int stickSum = sticks[gidxL] + sticks[gidxU] + stickDelta;
				if (stickSum == 0 && (numPlacedNonEmpty + (int) !c.empty()) < n * n - 1) { // isolation
					continue;
				}
				if (c.right() && i > 0 && b.get(i - 1, j + 1).down() && (gidxL == indices[j + 1] || gidxU == indices[j + 1])) { // loop in the next depth
					continue;
				}
			} else if (c.left() || c.up()) {
				int gidx;
				if (c.left()) {
					gidx = indices[j - 1];
				} else {
					gidx = indices[j];
				}
				int stickDelta = -((int) c.left() + (int) c.up()) + ((int) c.right() + (int) c.down());
				int stickSum = sticks[gidx] + stickDelta;
				if (stickSum == 0 && (numPlacedNonEmpty + (int) !c.empty()) < n * n - 1) { // isolation
					continue;
				}
				if (c.right() && i > 0 && b.get(i - 1, j + 1).down() && gidx == indices[j + 1]) { // loop in the next depth
					continue;
				}
			}
			numTrials++;
			if (fixed) {
				bests[0].second = idx;
			} else {
				int score = rnd.nextInt((counts[idx] << 16) + 1);
				if (score < bests[1].first) continue;
				bests[1].first = score;
				bests[1].second = idx;
				if (score < bests[0].first) continue;
				swap(bests[0], bests[1]);
			}
		}
		numTrials = min(numTrials, 2);

		rep0(trialIdx, numTrials) {
			int idx = bests[trialIdx].second;
			Cell c(idx);

			int nextNumPlacedNonEmpty = numPlacedNonEmpty + (int) !c.empty();
			int stickDelta = -((int) c.left() + (int) c.up()) + ((int) c.right() + (int) c.down());

			if (c.empty()) {
				// search deeper
				b.set(i, j, c);
				counts[idx] -= (int) !fixed;
				int j2 = (j + 1) % n;
				int i2 = j2 == 0 ? i + 1 : i;
				bool ok = buildTreeDfs(i2, j2, nextNumPlacedNonEmpty, counts, indices, sticks, unusedIndices);
				counts[idx] += (int) !fixed;

				if (ok) return true;
			} else if (c.left() && c.up()) { // trying to merge groups
				int gidxL = indices[j - 1];
				int gidxU = indices[j];
				int stickL = sticks[gidxL];
				int stickU = sticks[gidxU];
				int stickSum = stickL + stickU + stickDelta;

				// merge L to U

				// keep old
				int oldIndices[MAX_N];
				memcpy(oldIndices, indices, sizeof(int) * n);

				// update
				rep0(i, n) {
					if (indices[i] == gidxL) indices[i] = gidxU;
				}
				sticks[gidxL] = 0;
				sticks[gidxU] = stickSum;
				unusedIndices.push_back(gidxL);

				// search deeper
				b.set(i, j, c);
				counts[idx] -= (int) !fixed;
				int j2 = (j + 1) % n;
				int i2 = j2 == 0 ? i + 1 : i;
				bool ok = buildTreeDfs(i2, j2, nextNumPlacedNonEmpty, counts, indices, sticks, unusedIndices);
				counts[idx] += (int) !fixed;

				// restore
				memcpy(indices, oldIndices, sizeof(int) * n);
				sticks[gidxL] = stickL;
				sticks[gidxU] = stickU;
				assert(unusedIndices.back() == gidxL);
				unusedIndices.pop_back();

				if (ok) return true;
			} else {
				int gidx = -1;
				if (c.left()) {
					gidx = indices[j - 1];
				} else if (c.up()) {
					gidx = indices[j];
				}
				int stickSum = (gidx == -1 ? 0 : sticks[gidx]) + stickDelta;

				// update
				int oldIndex = indices[j];
				if (gidx == -1) {
					assert(!unusedIndices.empty());
					indices[j] = unusedIndices.back();
					unusedIndices.pop_back();
				} else {
					indices[j] = gidx;
				}
				int newIndex = indices[j];
				int oldStick = sticks[newIndex];
				sticks[newIndex] = stickSum;

				// search deeper
				b.set(i, j, c);
				counts[idx] -= (int) !fixed;
				int j2 = (j + 1) % n;
				int i2 = j2 == 0 ? i + 1 : i;
				bool ok = buildTreeDfs(i2, j2, nextNumPlacedNonEmpty, counts, indices, sticks, unusedIndices);
				counts[idx] += (int) !fixed;

				// restore
				sticks[newIndex] = oldStick;
				if (gidx == -1) {
					unusedIndices.push_back(newIndex);
				}
				indices[j] = oldIndex;

				if (ok) return true;
			}

			//if (i < 5 && count < MAX_COUNT) {
			//	b.print();
			//	trace("we had");
			//	rep0(idx, 16) {
			//		if (counts[idx] == 0) continue;
			//		auto s = Cell(idx).toString();
			//		trace(s[0]);
			//		trace(s[1], " : ", counts[idx]);
			//		trace(s[2]);
			//	}
			//	trace("fail! i = ", i);
			//}

			if (!fixed) {
				b.set(i, j, Cell(CELL_NULL));
			}
			if (count >= MAX_COUNT) return false;
		}
		return false;
	}

	void buildTreeDfsAll(int i, int j, int numPlacedNonEmpty, int counts[16], int indices[MAX_N], int sticks[MAX_N], vector<int>& unusedIndices, int maxCount) {
		if (alarm || len(builtAll) >= maxCount) return;

		if (++count % 1000 == 0 && timer() > 2850) {
			alarm = true;
			return;
		}

		if (i == endI && j == endJ) {
			builtAll.push_back(b);
			return;
		}

		int left = 2; // 0: never, 1: must, 2: free
		int right = 2;
		int up = 2;
		int down = 2;

		Cell tmp;
		if (j == 0) {
			left = 0;
		} else if (!(tmp = b.get(i, j - 1)).null()) {
			left = (int) tmp.right();
		}
		if (j == n - 1) {
			right = 0;
		} else if (!(tmp = b.get(i, j + 1)).null()) {
			right = (int) tmp.left();
		}
		if (i == 0) {
			up = 0;
		} else if (!(tmp = b.get(i - 1, j)).null()) {
			up = (int) tmp.down();
		}
		if (i == n - 1) {
			down = 0;
		} else if (!(tmp = b.get(i + 1, j)).null()) {
			down = (int) tmp.up();
		}

		vector<int> trials;
		bool fixed = !b.get(i, j).null();
		int fixedPiece = b.get(i, j).val;
		int pieceCount = fixed ? 1 : 16;

		rep0(idx, pieceCount) {
			if (fixed) idx = fixedPiece;
			else if (counts[idx] == 0 || idx == 0 && noEmptyAt[i][j]) continue;

			Cell c(idx);
			if (left == 0 && c.left() || left == 1 && !c.left()) continue;
			if (right == 0 && c.right() || right == 1 && !c.right()) continue;
			if (up == 0 && c.up() || up == 1 && !c.up()) continue;
			if (down == 0 && c.down() || down == 1 && !c.down()) continue;

			if (c.empty()) {
			} else if (c.left() && c.up()) { // trying to merge groups
				int gidxL = indices[j - 1];
				int gidxU = indices[j];
				if (gidxL == gidxU) { // loop
					continue;
				}
				int stickDelta = -((int) c.left() + (int) c.up()) + ((int) c.right() + (int) c.down());
				int stickSum = sticks[gidxL] + sticks[gidxU] + stickDelta;
				if (stickSum == 0 && (numPlacedNonEmpty + (int) !c.empty()) < n * n - 1) { // isolation
					continue;
				}
				if (c.right() && i > 0 && b.get(i - 1, j + 1).down() && (gidxL == indices[j + 1] || gidxU == indices[j + 1])) { // loop in the next depth
					continue;
				}
			} else if (c.left() || c.up()) {
				int gidx;
				if (c.left()) {
					gidx = indices[j - 1];
				} else {
					gidx = indices[j];
				}
				int stickDelta = -((int) c.left() + (int) c.up()) + ((int) c.right() + (int) c.down());
				int stickSum = sticks[gidx] + stickDelta;
				if (stickSum == 0 && (numPlacedNonEmpty + (int) !c.empty()) < n * n - 1) { // isolation
					continue;
				}
				if (c.right() && i > 0 && b.get(i - 1, j + 1).down() && gidx == indices[j + 1]) { // loop in the next depth
					continue;
				}
			}
			trials.push_back(idx);
		}

		for (int idx : trials) {
			Cell c(idx);

			int nextNumPlacedNonEmpty = numPlacedNonEmpty + (int) !c.empty();
			int stickDelta = -((int) c.left() + (int) c.up()) + ((int) c.right() + (int) c.down());

			if (c.empty()) {
				// search deeper
				b.set(i, j, c);
				counts[idx] -= (int) !fixed;
				int j2 = (j + 1) % n;
				int i2 = j2 == 0 ? i + 1 : i;
				buildTreeDfsAll(i2, j2, nextNumPlacedNonEmpty, counts, indices, sticks, unusedIndices, maxCount);
				counts[idx] += (int) !fixed;
			} else if (c.left() && c.up()) { // trying to merge groups
				int gidxL = indices[j - 1];
				int gidxU = indices[j];
				int stickL = sticks[gidxL];
				int stickU = sticks[gidxU];
				int stickSum = stickL + stickU + stickDelta;

				// merge L to U

				// keep old
				int oldIndices[MAX_N];
				memcpy(oldIndices, indices, sizeof(int) * n);

				// update
				rep0(i, n) {
					if (indices[i] == gidxL) indices[i] = gidxU;
				}
				sticks[gidxL] = 0;
				sticks[gidxU] = stickSum;
				unusedIndices.push_back(gidxL);

				// search deeper
				b.set(i, j, c);
				counts[idx] -= (int) !fixed;
				int j2 = (j + 1) % n;
				int i2 = j2 == 0 ? i + 1 : i;
				buildTreeDfsAll(i2, j2, nextNumPlacedNonEmpty, counts, indices, sticks, unusedIndices, maxCount);
				counts[idx] += (int) !fixed;

				// restore
				memcpy(indices, oldIndices, sizeof(int) * n);
				sticks[gidxL] = stickL;
				sticks[gidxU] = stickU;
				assert(unusedIndices.back() == gidxL);
				unusedIndices.pop_back();
			} else {
				int gidx = -1;
				if (c.left()) {
					gidx = indices[j - 1];
				} else if (c.up()) {
					gidx = indices[j];
				}
				int stickSum = (gidx == -1 ? 0 : sticks[gidx]) + stickDelta;

				// update
				int oldIndex = indices[j];
				if (gidx == -1) {
					assert(!unusedIndices.empty());
					indices[j] = unusedIndices.back();
					unusedIndices.pop_back();
				} else {
					indices[j] = gidx;
				}
				int newIndex = indices[j];
				int oldStick = sticks[newIndex];
				sticks[newIndex] = stickSum;

				// search deeper
				b.set(i, j, c);
				counts[idx] -= (int) !fixed;
				int j2 = (j + 1) % n;
				int i2 = j2 == 0 ? i + 1 : i;
				buildTreeDfsAll(i2, j2, nextNumPlacedNonEmpty, counts, indices, sticks, unusedIndices, maxCount);
				counts[idx] += (int) !fixed;

				// restore
				sticks[newIndex] = oldStick;
				if (gidx == -1) {
					unusedIndices.push_back(newIndex);
				}
				indices[j] = oldIndex;
			}
			if (!fixed) {
				b.set(i, j, Cell(CELL_NULL));
			}
			if (alarm || len(builtAll) >= maxCount) return;
		}
	}
};

class PiecePos {
public:
	vector<array<int, 2>> pos;
	vector<bool> used;
	int num = 0;

	void clear() {
		pos.clear();
		used.clear();
		num = 0;
	}

	void addPos(int i, int j) {
		pos.push_back({ i, j });
		used.emplace_back(false);
		num++;
	}

	void resetUsage() {
		used.assign(num, false);
	}
};

class Hole {
public:
	int i = 0;
	int j = 0;
	int type = 0;

	Hole(int i, int j, int type) : i(i), j(j), type(type) {
	}
};

class PieceConfigCandidate {
public:
	vector<pii> poss;
	int distEstimation = 0;

	PieceConfigCandidate() {
	}

	PieceConfigCandidate(vector<pii> poss, int distEstimation) : poss(poss), distEstimation(distEstimation) {
	}
};

void print(bool b[MAX_N][MAX_N], int n) {
	string line;
	rep2() {
		line += b[i][j] ? "O" : ".";
		if (j == n - 1) {
			trace(line);
			line = "";
		}
	}
}

class PieceFiller {
public:
	TreeBuilder tb;
	Board currentBoard;
	SubGameSolver sol;
	bool fixed[MAX_N][MAX_N] = {};
	bool next[MAX_N][MAX_N] = {};
	bool lastRect[MAX_N][MAX_N] = {};
	SolverResult res;

	PieceFiller() {
	}

	void findPos() {
		n = currentBoard.n;
		sol.n = n;
		Board b;
		b.clear(n);
		int maxI = -1;
		int maxJ = -1;
		numHoles = 0;
		holes.clear();
		rep0(idx, 16) {
			ps[idx].clear();
		}
		rep2() {
			assert(!(fixed[i][j] && next[i][j]));
			sol.walls[i][j] = fixed[i][j];
			tb.noEmptyAt[i][j] = !lastRect[i][j];
			if (fixed[i][j]) {
				b.set(i, j, currentBoard.get(i, j));
			} else {
				int idx = currentBoard.get(i, j).val;
				ps[idx].addPos(i, j);
			}
			if (currentBoard.get(i, j).empty()) {
				emptyI = i;
				emptyJ = j;
			}
			if (next[i][j]) {
				numHoles++;
				holes.emplace_back(i, j);
				if (i > maxI || i == maxI && j > maxJ) {
					maxI = i;
					maxJ = j;
				}
			}
		}
		tb.init(currentBoard, b);
		//print(tb.noEmptyAt, n);d

		//if (tb.build()) {
		//	//trace("feasible example:");
		//	//tb.built.print();
		//} else {
		//	trace("WHAT???????????????");
		//	currentBoard.print();
		//	trace("--");
		//	b.print();
		//	assert(false);
		//}

		res.steps = -1;
		res.deltas.clear();

		int iter = 0;
		set<int> tried;

		candidates.clear();

		int numIter = -1;
		switch (n) {
		case 6:
			numIter = 50;
			break;
		case 7:
			numIter = 30;
			break;
		case 8:
			numIter = 20;
			break;
		case 9:
			numIter = 15;
			break;
		case 10:
			numIter = 10;
			break;
		}

		rep0(iter, numIter) {
			bool ok = tb.build();
			int en = timer();
			if (!ok) {
				trace("couldn't build!!!");
				//currentBoard.print();
				//b.print();
				continue;
			}
			int id = 0;
			rep0(hidx, numHoles) {
				int i = holes[hidx].first;
				int j = holes[hidx].second;
				id <<= 4;
				id |= tb.built.get(i, j).val & CELL_FULL;
			}
			if (tried.count(id) == 0) {
				//trace("new! ", id);
				tried.insert(id);
				addCandidate(tb.built);
			}
		}

		if (PRINT_PIECE_FILLING_INFO) {
			trace("number of candidates: ", len(candidates));
		}

		sort(candidates.begin(), candidates.end(), [](const PieceConfigCandidate& p1, const PieceConfigCandidate& p2) {
			return p1.distEstimation < p2.distEstimation;
			});
		sol.numPieces = numHoles + 1;

		if (PRINT_SUBGAME_TIMES) {
			tracen("subgame:");
		}
		int st = timer();
		minDist = INF; // do not exceed steps
		for (auto& cand : candidates) {
			if (timer() > 2850) break;
			sol.ps[0].set(emptyI, emptyJ, -1, -1);
			rep0(hidx, numHoles) {
				pii start = cand.poss[hidx + 1];
				pii goal = holes[hidx];
				sol.ps[hidx + 1].set(start.first, start.second, goal.first, goal.second);
			}
			int st = timer();
			sol.solve(minDist - 1);
			if (PRINT_SUBGAME_TIMES) {
				tracen(" ", timer() - st);
			}
			if (sol.res.steps != -1) {
				minDist = sol.res.steps;
				res = sol.res;
				if (PRINT_SUBGAME_TIMES) {
					tracen("[", minDist, "]");
				}
			}
		}
		if (PRINT_SUBGAME_TIMES) {
			trace(" total ", timer() - st, " ms");
		}
		if (PRINT_PIECE_FILLING_INFO) {
			trace("best: ", minDist);
		}







		//while (true) {
		//	//trace("building ", iter);
		//	int st = timer();
		//	bool ok = tb.build(maxI, maxJ);
		//	int en = timer();
		//	//trace("build: ", en - st, " ms");
		//	if (!ok) {
		//		trace("couldn't build!!!");
		//		return;
		//	}
		//	int id = 0;
		//	rep0(hidx, numHoles) {
		//		int i = holes[hidx].first;
		//		int j = holes[hidx].second;
		//		id <<= 4;
		//		id |= tb.built.get(i, j).val & CELL_FULL;
		//	}
		//	if (tried.count(id) == 0) {
		//		//trace("new! ", id);
		//		tried.insert(id);
		//		checkDist(tb.built);
		//	} else {
		//		if (iter > 1000) {
		//			trace("couldn't solve! iter > 1000");
		//			return;
		//		}
		//		if (iter > 100) {
		//			//trace("already tried, but trying again... ", id);
		//			checkDist(tb.built);
		//		} else {
		//			//trace("already tried. ", id);
		//		}
		//	}
		//	if (++iter >= 100 && minDist < INF) break;
		//}


		if (minDist == INF) {
			trace("could not solve!!!");
			return;
		}
		//sol.ps = bestPieceConfig;
		//sol.numPieces = numHoles + 1;
		//int st = timer();
		//sol.solve(200);
		//if (PRINT_BLOCK_RESULT) {
		//	trace("sub-problem time: ", timer() - st, " ms");
		//	trace("dist: ", minDist, "(manhattan) -> ", sol.res.steps, " (actual)");
		//}

		//currentBoard.print();
		//best.print();
		//res = sol.res;
	}

	void solveLast() {
		n = currentBoard.n;
		esol.n = n;
		Board b;
		b.clear(n);
		numHoles = 0;
		holes.clear();
		rep0(idx, 16) {
			ps[idx].clear();
		}
		rep2() {
			next[i][j] = !fixed[i][j];
			if (fixed[i][j]) {
				esol.b[i][j] = -2;
				esol.goal[i][j] = -2;
			}
			tb.noEmptyAt[i][j] = false;
			if (fixed[i][j]) {
				b.set(i, j, currentBoard.get(i, j));
			} else {
				int idx = currentBoard.get(i, j).val;
				ps[idx].addPos(i, j);
			}
			if (next[i][j]) {
				numHoles++;
				holes.emplace_back(i, j);
			}
		}
		tb.init(currentBoard, b);

		res.steps = -1;
		res.deltas.clear();

		tb.buildAll(10);
		//for (auto& b : tb.builtAll) {
		//	b.print();
		//}
		minDist = 41; // do not excced 40 for the last moves
		for (auto& b : tb.builtAll) {
			if (timer() > 2850 && minDist < INF) {
				trace("alarm beforeend");
				break;
			}
			checkDistLast(b);
		}
		if (minDist == INF) {
			trace("could not solve!!");
			return;
		}
		//trace("last moves: ");
		//for (pii m : deltas) {
		//	trace("(", m.first, ", ", m.second, ")");
		//}
		//currentBoard.print();
		//best.print();
	}

private:
	int n = 0;
	int numHoles = 0;
	SubGamePieces bestPieceConfig;
	SubGamePieces pieceConfig;
	PiecePos ps[16];
	vector<pii> holes;
	int emptyI = -1;
	int emptyJ = -1;
	int minDist = 0;
	Board best;
	EndGameSolver esol;
	vector<PieceConfigCandidate> candidates;

	void checkDistLastDfs(Board& b, int hidx) {
		if (hidx == numHoles) {
			// do check
			if (timer() > 2850) {
				return;
			}
			esol.solve(minDist - 1);
			if (esol.res.steps != -1) {
				int dist = esol.res.steps;
				if (dist < minDist) {
					res = esol.res;
					minDist = dist;
					if (PRINT_BEST_UPDATE) {
						trace("best found! ", dist);
					}
				}
			}
			return;
		}

		pii hpos = holes[hidx];
		int i = hpos.first;
		int j = hpos.second;
		int want = b.get(i, j).val;
		PiecePos& p = ps[want];

		int tmp1 = esol.goal[i][j];
		esol.goal[i][j] = hidx;

		rep0(pidx, p.num) {
			if (p.used[pidx]) continue;
			int i2 = p.pos[pidx][0];
			int j2 = p.pos[pidx][1];

			p.used[pidx] = true;
			int tmp2 = esol.b[i2][j2];
			esol.b[i2][j2] = hidx;

			checkDistLastDfs(b, hidx + 1);

			esol.b[i2][j2] = tmp2;
			p.used[pidx] = false;
		}

		esol.goal[i][j] = tmp1;
	}

	void checkDistLast(Board& b) {
		rep0(idx, 16) {
			ps[idx].resetUsage();
		}
		rep0(hidx, numHoles) {
			int i = holes[hidx].first;
			int j = holes[hidx].second;
			if (b.get(i, j).empty() && hidx > 0) {
				swap(holes[0], holes[hidx]);
				break;
			}
		}
		checkDistLastDfs(b, 0);
	}

	void addCandidate(Board& b) {
		rep0(idx, 16) {
			ps[idx].resetUsage();
		}

		vector<int> indices;
		rep0(i, numHoles) {
			indices.push_back(i);
		}

		set<vector<pii>> made;

		do {
			vector<pii> usedIndices;
			vector<pii> nextConfig;
			nextConfig.resize(numHoles + 1);
			nextConfig[0] = pii(emptyI, emptyJ);

			int dist = 0;
			rep0(hidx, numHoles) {
				pii hpos = holes[indices[hidx]];
				int i = hpos.first;
				int j = hpos.second;
				int want = b.get(i, j).val;
				int minDist = INF;
				int minIdx = -1;
				PiecePos& p = ps[want];
				rep0(pidx, p.num) {
					if (p.used[pidx]) continue;
					int i2 = p.pos[pidx][0];
					int j2 = p.pos[pidx][1];
					int dist = abs(i - i2) + abs(j - j2);
					if (dist < minDist) {
						minDist = dist;
						minIdx = pidx;
					}
				}
				dist += minDist;
				assert(minIdx != -1);
				int pi = p.pos[minIdx][0];
				int pj = p.pos[minIdx][1];
				assert(!fixed[pi][pj]);
				p.used[minIdx] = true;
				usedIndices.emplace_back(want, minIdx);
				nextConfig[indices[hidx] + 1] = pii(pi, pj);
				//pieceConfig[hidx + 1].set(pi, pj, i, j);
			}
			for (pii usedIndex : usedIndices) {
				ps[usedIndex.first].used[usedIndex.second] = false;
			}
			if (made.count(nextConfig) == 0) {
				made.emplace(nextConfig);
				candidates.emplace_back(nextConfig, dist);
				//string s = "";
				//rep0(pidx, numHoles + 1) {
				//	s += " ";
				//	s += "(" + to_string(nextConfig[pidx].first) + ", " + to_string(nextConfig[pidx].second) + ")";
				//}
				//trace(s);
			}
		} while (next_permutation(indices.begin(), indices.end()));

		//trace("num configs: ", len(configs));
		//for (auto& config : configs) {
		//	sol.numPieces = numHoles + 1;
		//	rep0(pidx, numHoles + 1) {
		//		pii spos = config[pidx].first;
		//		pii gpos = config[pidx].second;
		//		//trace(spos.first, " ", spos.second, " ", gpos.first, " ", gpos.second);
		//		sol.ps[pidx].set(spos.first, spos.second, gpos.first, gpos.second);
		//	}
		//	sol.solve(minDist - 1);
		//	int dist = sol.res.steps == -1 ? INF : sol.res.steps;

		//	if (dist < minDist) {
		//		if (PRINT_BEST_UPDATE) {
		//			trace("best dist: ", dist);
		//		}
		//		minDist = dist;
		//		best = b;
		//		bestPieceConfig = sol.ps;
		//	}
		//}
	}
};


class MainResult {
public:
	string ans;
	int steps = -1;
	int score = 0;
};


class Solver {
public:
	Board origB;
	Board b;
	int n = 0;
	Rnd rnd;
	TreeBuilder tb;
	PieceFiller pf;
	MainResult res;

	Solver() {
	}

	pii getEmptyPos() const {
		rep2() {
			if (b.get(i, j).val == CELL_EMPTY) {
				return pii(i, j);
			}
		}
		return pii(-1, -1);
	}

	void buildTree() {
		Board b2;
		b2.clear(n);
		tb.init(b, b2);
		rep0(iter, 100) {
			tb.build(3, n - 1);
		}
	}

	void read(int seed) {
		b.read(seed);
		origB = b;
		n = b.n;
	}

	void read(istream& in) {
		b.read(in);
		origB = b;
		n = b.n;
	}

	void solve() {
		int maxScore = 0;
		string maxAns = "";
		int ansRot = 0;
		int minMoves = 10000;
		rep0(iter, 65536) {
			Transform tf(iter);
			b = origB;
			tf.applyBoard(b);
			fillPiece(minMoves - 1);
			if (res.score > maxScore) {
				trace("best score at iter ", iter, ": ", res.score);
				minMoves = res.steps;
				maxScore = res.score;
				maxAns = res.ans;
				tf.applyString(maxAns);
			}
			if (timer() > 2850) {
				trace("time limit! ", timer());
				break;
			}
		}
		trace("final score: ", maxScore);
		cout << maxAns << endl;
		res.score = maxScore;
		res.ans = maxAns;
	}

private:
	void subGameTest() {
		SubGameSolver sol;
		sol.numPieces = 2;
		int n = 4;
		sol.n = n;
		rep2() {
			sol.walls[i][j] = false;
		}
		sol.ps[0].set(3, 0, -1, -1);
		sol.ps[1].set(3, 1, 0, 0);
		sol.solve(200);
		trace(sol.ps[0].i);
		trace(sol.ps[0].j);
		trace(sol.ps[0].gi);
		trace(sol.ps[0].gj);
		trace(sol.ps[1].i);
		trace(sol.ps[1].j);
		sol.solve(200);
		for (pii d : sol.res.deltas) {
			trace(d.first, " ", d.second);
		}
	}

	void fillPiece(int maxMoves) {
		int moveCount = 0;

		pii epos = getEmptyPos();
		int i = epos.first;
		int j = epos.second;

		res.score = 0;
		res.steps = -1;
		res.ans = "";

		vector<string> scheme;

		switch (n) {
		case 6:
			scheme.emplace_back("BBBAAA");
			scheme.emplace_back("CCCDDD");
			scheme.emplace_back("FFFEEE");
			scheme.emplace_back("GH    ");
			scheme.emplace_back("GH    ");
			scheme.emplace_back("GH    ");
			break;
		case 7:
			scheme.emplace_back("AAABBCC");
			scheme.emplace_back("LLLKHGD");
			scheme.emplace_back("MMMKHGD");
			scheme.emplace_back("   KHGD");
			scheme.emplace_back("   JIFE");
			scheme.emplace_back("   JIFE");
			scheme.emplace_back("   JIFE");
			break;
		case 8:
			scheme.emplace_back("AAABBBCC");
			scheme.emplace_back("FFEEEDDD");
			scheme.emplace_back("HILMPQQQ");
			scheme.emplace_back("HILMPRRR");
			scheme.emplace_back("HILMP   ");
			scheme.emplace_back("GJKNO   ");
			scheme.emplace_back("GJKNO   ");
			scheme.emplace_back("GJKNO   ");
			//scheme.emplace_back("AAAABBBB");
			//scheme.emplace_back("DDDDCCCC");
			//scheme.emplace_back("EEEEFFFF");
			//scheme.emplace_back("HHHHGGGG");
			//scheme.emplace_back("IIIIJJJJ");
			//scheme.emplace_back("    NMLK");
			//scheme.emplace_back("    NMLK");
			//scheme.emplace_back("    NMLK");
			break;
		case 9:
			scheme.emplace_back("AAABBBCCC");
			scheme.emplace_back("FFFEEEDDD");
			scheme.emplace_back("GGGHHHIII");
			scheme.emplace_back("LLLKKKJJJ");
			scheme.emplace_back("MMMNNNOOO");
			scheme.emplace_back("RRRQQQPPP");
			scheme.emplace_back("STUVW    ");
			scheme.emplace_back("STUVW    ");
			scheme.emplace_back("STUVW    ");
			break;
		case 10:
			scheme.emplace_back("DDCCBBBAAA");
			scheme.emplace_back("EJKPQQQRRR");
			scheme.emplace_back("EJKPTTTSSS");
			scheme.emplace_back("EJKPUUUVVV");
			scheme.emplace_back("FILOXXXWWW");
			scheme.emplace_back("FILOYYYZZZ");
			scheme.emplace_back("FILO\\\\\\[[[");
			scheme.emplace_back("GHMN]^    ");
			scheme.emplace_back("GHMN]^    ");
			scheme.emplace_back("GHMN]^    ");
			break;
		}
		rep2() {
			pf.lastRect[i][j] = true;
		}
		vector<vector<pii>> poss;
		rep0(t, 64) {
			bool end = true;
			vector<pii> pos;
			rep2() {
				if (scheme[i][j] == 'A' + t) {
					pos.emplace_back(i, j);
					pf.lastRect[i][j] = false;
					end = false;
				}
			}
			if (end) break;
			poss.emplace_back(move(pos));
		}


		//rep0(i, n - 3) {
		//	int j = i % 2 == 0 ? -1 : n;
		//	int dj = i % 2 == 0 ? 1 : -1;
		//	switch (n) {
		//	case 6:
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) , pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		break;
		//	case 7:
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		break;
		//	case 8:
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		break;
		//	case 9:
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		break;
		//	case 10:
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		poss.push_back({ pii(i, j += dj), pii(i, j += dj), pii(i, j += dj) });
		//		break;
		//	}
		//}
		//{
		//	int rowUntil = n - 4;
		//	rep0(j, rowUntil) {
		//		int i = n - 3;
		//		if (j + 1 < rowUntil) {
		//			poss.push_back({ pii(n - 3, j), pii(n - 2, j) , pii(n - 1, j), pii(n - 3, ++j), pii(n - 2, j) , pii(n - 1, j) });
		//		} else {
		//			poss.push_back({ pii(n - 3, j), pii(n - 2, j) , pii(n - 1, j) });
		//		}
		//	}
		//}

		rep2() {
			pf.fixed[i][j] = false;
			pf.next[i][j] = false;
		}
		reverse(poss.begin(), poss.end());

		if (PRINT_PROGRESS) {
			trace("current board (", moveCount, " move(s)):");
			b.print();
		}
		while (!poss.empty()) {
			for (pii pos : poss.back()) {
				pf.next[pos.first][pos.second] = true;
			}
			poss.pop_back();

			pf.currentBoard = b;
			pf.findPos();
			if (pf.res.steps == -1) {
				return;
			}

			for (pii delta : pf.res.deltas) {
				int di = delta.first;
				int dj = delta.second;

				res.ans += di == -1 ? 'U' : di == 1 ? 'D' : dj == -1 ? 'L' : 'R';

				swap(b.data[i + di][j + dj], b.data[i][j]);
				if (++moveCount > maxMoves) return;
				i += di;
				j += dj;
			}

			rep2() {
				pf.fixed[i][j] |= pf.next[i][j];
				pf.next[i][j] = false;
			}

			if (PRINT_PROGRESS) {
				trace("current board (", moveCount, " move(s)):");
				b.print();
			}
			if (PRINT_MOVE_COUNTS) {
				trace("moves: ", moveCount);
			}
		}

		pf.currentBoard = b;
		pf.solveLast();
		if (pf.res.steps == -1) {
			return;
		}

		for (pii delta : pf.res.deltas) {
			int di = delta.first;
			int dj = delta.second;

			res.ans += di == -1 ? 'U' : di == 1 ? 'D' : dj == -1 ? 'L' : 'R';

			swap(b.data[i + di][j + dj], b.data[i][j]);
			if (++moveCount > maxMoves) return;
			i += di;
			j += dj;
		}
		if (PRINT_PROGRESS) {
			trace("current board (", moveCount, " move(s)):");
			b.print();
		}
		if (PRINT_MOVE_COUNTS) {
			trace("moves: ", moveCount);
		}
		res.steps = moveCount;
		res.ans = res.ans;
		res.score = (int) (500000 * (2.0 - moveCount / (n * n * n * 2.0)) + 0.5);
		trace("==== solved. steps: ", res.steps, ", score: ", res.score, ", time: ", timer(), " ms(total) ====");
	}
};


int main() {
#if 0

	int num = 50;
	int from = 0;
	int stride = 1;

	int single = -1;

	pii times[1000] = {};

	int scoreSum = 0;

	rep0(t, num) {
		timer(true);
		int seed = single != -1 ? single : from + t * stride;
		trace("\n------------ SOLVING SEED ", seed, " ------------");
		//tracen(seed, ": ");
		Solver s;
		s.read(seed);
		s.solve();
		scoreSum += s.res.score;
		times[t] = pii(timer(), seed);
		if (single != -1) break;
	}
	if (single == -1) {
		trace("--- summary ---");
		sort(times, times + num);
		rep0r(t, num) {
			trace("seed ", times[t].second, ": ", times[t].first, " ms");
	}
}
	trace("total score: ", scoreSum, " time = ", timer());

#elif 1
	timer(true);
	Solver s;
	s.read(cin);
	s.solve();
#elif 1
#endif
	return 0;
}