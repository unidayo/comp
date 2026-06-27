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
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <thread>
#include <mutex>

// #define NDEBUG
#include <cassert>

using namespace std;
using ull = unsigned long long;
using ll = long long;
using pii = pair<int, int>;
using pdi = pair<double, int>;
using pfi = pair<float, int>;
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
		return (y = y ^ (y << 15)) & 0x7fffffff;
	}

	int nextInt() {
		return next();
	}

	int nextInt(int n) {
		return next() % n;
	}

	float nextFloat() {
		return (float) next() / 0x80000000;
	}

	double nextDouble() {
		return (double) next() / 0x80000000;
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
		double sr; // stroke RGBA
		double sg;
		double sb;
		double sa;
		double fr; // fill RGBA
		double fg;
		double fb;
		double fa;
		double e00; // affine matrix
		double e01;
		double e02;
		double e10;
		double e11;
		double e12;
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
			return fa <= 0 ? "" : p("fill", rgb(fr, fg, fb)) + p("fill-opacity", fa);
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

		void screen(int width, int height) {
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

		string dump(string id = "", string style = "") const {
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
			svgs.push_back(g.dump("f" + to_string(len(svgs)), "display:none;pointer-events:none;user-select:none;"));
		}

		string dumpHtml(int fps) {
			ostringstream oss;
			oss << "<html><body><div id=\"text\">loading...</div>" << endl;
			rep0(i, len(svgs)) {
				string& svg = svgs[i];
				oss << svg << endl;
			}

			oss << "<script>\nlet numFrames = " << len(svgs) << ", fps = " << fps << ";";
			oss << R"(
	let text = document.getElementById("text");
	let frames = [];
	for (let i = 0; i < numFrames; i++) {
		let f = document.getElementById("f" + i);
		frames.push(f);
		f.style.display = "none";
	}
	let currentFrame = 0;
	let playing = true;
	setInterval(() => {
		if (!playing) return;
		text.innerText = (currentFrame + 1) + " / " + numFrames;
		frames[(currentFrame - 1 + numFrames) % numFrames].style.display = "none";
		frames[currentFrame].style.display = null;
		currentFrame = (currentFrame + 1) % numFrames;
		if (currentFrame == 0) playing = false;
	}, 1000 / fps);
	window.onmousedown = e => { if (e.button == 0) playing = true; };
;)";
			oss << "</script>" << endl;
			oss << "</body></html>" << endl;
			return oss.str();
		}
	private:
	};
}

using Graphics = svg::Graphics;
using Movie = svg::Movie;


constexpr int N = 30;

#define repH(i, j) rep0(i, N) rep0(j, N - 1)
#define repV(i, j) rep0(i, N - 1) rep0(j, N)

constexpr int MAX_K = 20;

template <class T>
class Dijkstra {
public:
	T dist[N][N];
	int prev[N][N];

	Dijkstra() : dist(), prev() {
	}

	void solve(const T hw[N][N - 1], const T vw[N - 1][N], int si, int sj, int ti, int tj) {
		constexpr T INF = 1000000000;
		using pti = pair<T, int>;
		rep0(i, N) {
			rep0(j, N) {
				dist[i][j] = i == si && j == sj ? 0 : INF;
			}
		}
		bool fixed[N][N] = {};
		priority_queue<pti, vector<pti>, greater<pti>> q;
		q.push(pti(0, si * N + sj));
		while (!q.empty()) {
			pti top = q.top();
			q.pop();
			T d = top.first;
			int idx = top.second;
			int i = idx / N;
			int j = idx % N;
			if (fixed[i][j]) continue;
			fixed[i][j] = true;

			if (i == ti && j == tj) return;

			if (i > 0) { // -v
				int i2 = i - 1;
				int j2 = j;
				T d2 = d + vw[i - 1][j];
				if (d2 < dist[i2][j2]) {
					dist[i2][j2] = d2;
					prev[i2][j2] = idx;
					q.push(make_pair(d2, i2 * N + j2));
				}
			}
			if (i < N - 1) { // +v
				int i2 = i + 1;
				int j2 = j;
				T d2 = d + vw[i][j];
				if (d2 < dist[i2][j2]) {
					dist[i2][j2] = d2;
					prev[i2][j2] = idx;
					q.push(make_pair(d2, i2 * N + j2));
				}
			}
			if (j > 0) { // -h
				int i2 = i;
				int j2 = j - 1;
				T d2 = d + hw[i][j - 1];
				if (d2 < dist[i2][j2]) {
					dist[i2][j2] = d2;
					prev[i2][j2] = idx;
					q.push(make_pair(d2, i2 * N + j2));
				}
			}
			if (j < N - 1) { // +h
				int i2 = i;
				int j2 = j + 1;
				T d2 = d + hw[i][j];
				if (d2 < dist[i2][j2]) {
					dist[i2][j2] = d2;
					prev[i2][j2] = idx;
					q.push(make_pair(d2, i2 * N + j2));
				}
			}
		}
	}

	void solveAStar(const T hw[N][N - 1], const T vw[N - 1][N], int si, int sj, int ti, int tj) {
		T minHCost = 9000;
		T minVCost = 9000;
		repH(i, j) {
			minHCost = min(minHCost, hw[i][j]);
		}
		repV(i, j) {
			minVCost = min(minVCost, vw[i][j]);
		}

		constexpr T INF = 1000000000;
		using pti = pair<T, int>;
		rep0(i, N) {
			rep0(j, N) {
				dist[i][j] = i == si && j == sj ? 0 : INF;
			}
		}
		bool fixed[N][N] = {};
		priority_queue<pti, vector<pti>, greater<pti>> q;
		q.push(pti(0, si * N + sj));
		while (!q.empty()) {
			pti top = q.top();
			q.pop();
			int idx = top.second;
			int i = idx / N;
			int j = idx % N;
			if (fixed[i][j]) continue;
			fixed[i][j] = true;
			T d = dist[i][j];

			if (i == ti && j == tj) return;

			if (i > 0) { // -v
				int i2 = i - 1;
				int j2 = j;
				T d2 = d + vw[i - 1][j];
				T heuristic = minVCost * abs(i2 - ti) + minHCost * abs(j2 - tj);
				if (d2 < dist[i2][j2]) {
					dist[i2][j2] = d2;
					prev[i2][j2] = idx;
					q.push(make_pair(d2 + heuristic, i2 * N + j2));
				}
			}
			if (i < N - 1) { // +v
				int i2 = i + 1;
				int j2 = j;
				T d2 = d + vw[i][j];
				T heuristic = minVCost * abs(i2 - ti) + minHCost * abs(j2 - tj);
				if (d2 < dist[i2][j2]) {
					dist[i2][j2] = d2;
					prev[i2][j2] = idx;
					q.push(make_pair(d2 + heuristic, i2 * N + j2));
				}
			}
			if (j > 0) { // -h
				int i2 = i;
				int j2 = j - 1;
				T d2 = d + hw[i][j - 1];
				T heuristic = minVCost * abs(i2 - ti) + minHCost * abs(j2 - tj);
				if (d2 < dist[i2][j2]) {
					dist[i2][j2] = d2;
					prev[i2][j2] = idx;
					q.push(make_pair(d2 + heuristic, i2 * N + j2));
				}
			}
			if (j < N - 1) { // +h
				int i2 = i;
				int j2 = j + 1;
				T d2 = d + hw[i][j];
				T heuristic = minVCost * abs(i2 - ti) + minHCost * abs(j2 - tj);
				if (d2 < dist[i2][j2]) {
					dist[i2][j2] = d2;
					prev[i2][j2] = idx;
					q.push(make_pair(d2 + heuristic, i2 * N + j2));
				}
			}
		}
	}
};

int fixedM = -1;

class Answer {
public:
	int m;
	int d;
	int h[N][2];
	int v[N][2];
	int x[N];
	int y[N];
	int hw[N][N - 1];
	int vw[N - 1][N];
	Rnd& rnd;

	Answer(Rnd& rnd) : m(), d(), h(), v(), x(), y(), hw(), vw(), rnd(rnd) {
	}

	void genRandom() {
		m = fixedM == -1 ? randIn(0, 1) : fixedM;
		d = randIn(100, 2000);
		rep0(i, N) {
			h[i][0] = randIn(1000 + d, 9000 - d);
			h[i][1] = randIn(1000 + d, 9000 - d);
			v[i][0] = randIn(1000 + d, 9000 - d);
			v[i][1] = randIn(1000 + d, 9000 - d);
			x[i] = randIn(1, 28);
			y[i] = randIn(1, 28);
		}
		if (m == 0) {
			rep0(i, N) {
				h[i][1] = h[i][0];
				v[i][1] = v[i][0];
			}
		}
		repH(i, j) {
			int k = j < x[i] ? 0 : 1;
			hw[i][j] = h[i][k] + randIn(-d, d);
			assert(hw[i][j] >= 1000 && hw[i][j] <= 9000);
		}
		repV(i, j) {
			int k = i < y[j] ? 0 : 1;
			vw[i][j] = v[j][k] + randIn(-d, d);
			assert(vw[i][j] >= 1000 && vw[i][j] <= 9000);
		}
	}

private:
	int randIn(int from, int to) const {
		return from + rnd.nextInt(to - from + 1);
	}
};

class Server {
public:
	Dijkstra<int> dijk;
	Answer ans;
	int qs[1000][4];
	double es[1000];
	int queryResult;
	int actualDist;
	double e;
	double ratio;
	int step;
	Rnd& rnd;

	int exactDistMat[N][N][N][N];

	Server(Rnd& rnd) : ans(rnd), qs(), es(), queryResult(), actualDist(), e(), ratio(), step(-1), rnd(rnd), exactDistMat() {
	}

	void setup() {
		ans.genRandom();
		rep0(i, 1000) {
			int si;
			int sj;
			int ti;
			int tj;
			do {
				si = rnd.nextInt(N);
				sj = rnd.nextInt(N);
				ti = rnd.nextInt(N);
				tj = rnd.nextInt(N);
			} while (abs(si - ti) + abs(sj - tj) < 10);
			qs[i][0] = si;
			qs[i][1] = sj;
			qs[i][2] = ti;
			qs[i][3] = tj;
			es[i] = 0.9 + rnd.nextFloat() * 0.2;
		}
		setupMat();
	}

	void setup(int id) {
		string name = to_string(id);
		while (name.length() < 4) name = "0" + name;
		name = "in/" + name + ".txt";
		auto res = readText(name);
		if (!res.second) {
			trace("cannot read ", name);
			segv();
		}
		string data = res.first;
		istringstream in(data);
		repH(i, j) in >> ans.hw[i][j];
		repV(i, j) in >> ans.vw[i][j];
		rep0(i, 1000) {
			in >> qs[i][0];
			in >> qs[i][1];
			in >> qs[i][2];
			in >> qs[i][3];
			int dist;
			in >> dist;
			in >> es[i];
		}
		setupMat();
	}

	void nextStep() {
		step++;
	}

	void chooseRandomPoints(int& si, int& sj, int& ti, int& tj) const {
		si = qs[step][0];
		sj = qs[step][1];
		ti = qs[step][2];
		tj = qs[step][3];
	}

	double exactRatio(int si, int sj, int ti, int tj, const string& path) {
		return (double) shortestDist(si, sj, ti, tj) / queryExact(si, sj, ti, tj, path);
	}

	void query(int si, int sj, int ti, int tj, const string& path) {
		int givenDist = queryExact(si, sj, ti, tj, path);
		assert(givenDist >= 1000);
		e = es[step];
		queryResult = (int) (givenDist * e + 0.5);
		actualDist = shortestDist(si, sj, ti, tj);
		assert(actualDist >= 1000);
		ratio = (double) actualDist / givenDist;
	}

	void drawAnswer(Graphics& g) const {
		double size = 100.0 / (N - 1);
		repH(i, j) {
			double x1 = j * size;
			double y1 = i * size;
			double x2 = x1 + size;
			double y2 = y1;
			double t = (ans.hw[i][j] - 1000.0) / 8000.0;
			g.strokeWidth(0.1 + (1.2 - 0.1) * t);
			g.stroke(0, 0, 0);
			g.line(x1, y1, x2, y2);
		}
		repV(i, j) {
			double x1 = j * size;
			double y1 = i * size;
			double x2 = x1;
			double y2 = y1 + size;
			double t = (ans.vw[i][j] - 1000.0) / 8000.0;
			g.strokeWidth(0.1 + (1.2 - 0.1) * t);
			g.stroke(0, 0, 0);
			g.line(x1, y1, x2, y2);
		}
	}

private:
	void setupMat() {
		rep0(si, N) {
			rep0(sj, N) {
				dijk.solve(ans.hw, ans.vw, si, sj, -1, -1);
				rep0(ti, N) {
					rep0(tj, N) {
						exactDistMat[si][sj][ti][tj] = dijk.dist[ti][tj];
					}
				}
			}
		}
	}

	int shortestDist(int si, int sj, int ti, int tj) {
		return exactDistMat[si][sj][ti][tj];
	}

	int queryExact(int si, int sj, int ti, int tj, const string& path) const {
		int i = si;
		int j = sj;
		int res = 0;
		for (char c : path) {
			int w = 0;
			switch (c) {
			case 'U':
				assert(i > 0);
				w = ans.vw[i - 1][j];
				i--;
				break;
			case 'D':
				assert(i < N - 1);
				w = ans.vw[i][j];
				i++;
				break;
			case 'L':
				assert(j > 0);
				w = ans.hw[i][j - 1];
				j--;
				break;
			case 'R':
				assert(j < N - 1);
				w = ans.hw[i][j];
				j++;
				break;
			default:
				assert(("invalid direction", false));
				break;
			}
			res += w;
			assert(w >= 1000 && w <= 9000);
			assert(res > 0);
		}
		assert(i == ti && j == tj);
		return res;
	}
};

// -----------------------------------------------------------------------

class Query {
public:
	vector<pii> hs;
	vector<pii> vs;
	double distance;

	Query() : distance() {
	}
};

mutex mut;
Movie globalMov;

namespace hyper0 {
	double betaFrom = 0.5; // [0.4, 0.6], lin
	double betaTo = 1.2;   // [0.5, 0.8], lin
	double betaPow = 1.0;  // [0.1, 4.0], lin

	double noiseSmoothFrom = 0.2; // [0.05, 0.2], lin
	double noiseSmoothTo = 0.2;   // [0, 0.05], lin
	double noiseSmoothPow = 1.0;  // [1.3, 2.0], lin

	double lrFrom = 0.005; // [0.004, 0.02], lin
	double lrTo = 0.005;   // [0.001, 0.005], lin

	double lrNoiseCoeff = 5.0; // [8, 15], lin

	double decayFrom = 0; // [0, 0.01], lin
	double decayTo = 0;   // [0, 0.0005], lin
	double decayPow = 1.0;   // [0.8, 1.5], lin
};

namespace hyper {
	double alphaFrom = 3;  // [0, 10], lin
	double alphaTo = 60;   // [30, 150], lin
	double alphaPow = 2.0; // [1, 3.0], lin

	double betaFrom = 0.5; // [0.5, 1], lin
	double betaTo = 1.2;   // [0.8, 2.5], lin
	double betaPow = 1.0;  // [1, 4], lin

	double noiseSmoothFrom = 0.2; // [0, 0.02], lin
	double noiseSmoothTo = 0.2;   // [0.007, 0.25], lin
	double noiseSmoothPow = 1.0;  // [0.1, 1.5], lin

	double lrFrom = 0.005; // [0.0025, 0.025], lin
	double lrTo = 0.005;   // [0.005, 0.025], lin

	double lrThetaCoeff = 2.0e-6; // [1e-8, 1e-7], lin
	double lrNoiseCoeff = 5.0;    // [0.1, 1.0], lin

	double explPow = 0.005;     // [0.005, 0.2], lin
	double explWeight = 1.0;  // [0.25, 1.25], lin
	double explMaxNum = 4;    // [6, 15], lin
	double explSmooth = 0.05; // [0, 0.15], lin

	double decayFrom = 1e-8; // [0.0001, 0.008], lin
	double decayTo = 1e-8;   // [0, 0.0015], lin
	double decayPow = 1.0;   // [0.1, 4.0], lin

	double divFrom = 5000; // [500, 8000], lin
	double divTo = 5000;   // [2000, 8000], lin

	double shiftFrom = 1.0; // [0, 2.5], lin
	double shiftPow = 0.5;   // [0.1, 1], lin
};

double fastExp(double x) {
	ll tmp = (ll) (1512775 * x + 1072632447);
	tmp <<= 32;
	return *((double*) &tmp);
}

class Param0 {
public:
	double hs[N];
	double vs[N];
	double noiseH[N][N - 1];
	double noiseV[N - 1][N];
	double beta;

	Param0() : hs(), vs(), noiseH(), noiseV(), beta() {
	}

	void reset() {
		rep0(i, N) {
			hs[i] = 5000;
			vs[i] = 5000;
		}
		repH(i, j) {
			noiseH[i][j] = 0;
		}
		repV(i, j) {
			noiseV[i][j] = 0;
		}
		beta = 0;
	}

	double hw(int i, int j) {
		return clamp(hs[i] + noiseH[i][j] * beta, 1000.0, 9000.0);
	}

	double vw(int i, int j) {
		return clamp(vs[j] + noiseV[i][j] * beta, 1000.0, 9000.0);
	}
};

class Param1 {
public:
	double hs[N][3]; // a, b, theta
	double vs[N][3]; // a, b, theta

	double noiseH[N][N - 1];
	double noiseV[N - 1][N];

	double computedHS[N][N - 1];
	double computedVS[N - 1][N];

	double alpha;
	double beta;

	Param1() : hs(), vs(), noiseH(), noiseV(), computedHS(), computedVS(), alpha(), beta() {
	}

	void reset() {
		rep0(i, N) {
			hs[i][0] = 5000;
			hs[i][1] = 5000;
			hs[i][2] = 0.5;
			vs[i][0] = 5000;
			vs[i][1] = 5000;
			vs[i][2] = 0.5;
		}
		repH(i, j) {
			noiseH[i][j] = 0;
		}
		repV(i, j) {
			noiseV[i][j] = 0;
		}
		alpha = 20.0;
		beta = 0;
	}

	double hw(int i, int j) {
		double a = hs[i][0];
		double b = hs[i][1];
		double theta = hs[i][2];
		double x = j / 29.0;
		double s = 1 / (1 + fastExp(alpha * (theta - x)));
		computedHS[i][j] = s;
		return clamp(a + (b - a) * s + noiseH[i][j] * beta, 1000.0, 9000.0);
	}

	double vw(int i, int j) {
		double a = vs[j][0];
		double b = vs[j][1];
		double theta = vs[j][2];
		double x = i / 29.0;
		double s = 1 / (1 + fastExp(alpha * (theta - x)));
		computedVS[i][j] = s;
		return clamp(a + (b - a) * s + noiseV[i][j] * beta, 1000.0, 9000.0);
	}
};

bool verbose = false;

class Solver {
public:
	Dijkstra<double> dijk;
	string query;
	double hw[N][N - 1];
	double vw[N - 1][N];
	double hw2[N][N - 1];
	double vw2[N - 1][N];
	int step;
	Param0 param0;
	Param1 param1;
	Rnd& rnd;
	Answer* currentAns;

	Solver(Rnd& rnd) : hw(), vw(), hn(), vn(), step(-1), hw2(), vw2(), param1(), rnd(rnd), currentAns(nullptr), weightMix() {
		repH(i, j) {
			hw[i][j] = 5000;
		}
		repV(i, j) {
			vw[i][j] = 5000;
		}
		param0.reset();
		param1.reset();
	}

	void nextStep() {
		step++;

		const double t = step / 1000.0;
		const double maxNum = hyper::explMaxNum;
		double weightCoeff = pow(hyper::explPow, t);
		const double weight = hyper::explWeight * weightCoeff;
		const double offset = 1 / (maxNum + 1.0);
		const double scale = 1.0 / (1 - offset);

		double hns[N] = {};
		double vns[N] = {};
		rep0(i, N) {
			rep0(k, N - 1) {
				hns[i] += hn[i][k];
				vns[i] += hn[k][i];
			}
			hns[i] /= N;
			vns[i] /= N;
		}

		repH(i, j) {
			double n = hn[i][j];
			double w = max(0.0, (1 / (n + 1) - offset) * scale);
			hw2[i][j] = hw[i][j] / (1.0 + w * weight);
		}
		repV(i, j) {
			double n = vn[i][j];
			double w = max(0.0, (1 / (n + 1) - offset) * scale);
			vw2[i][j] = vw[i][j] / (1.0 + w * weight);
		}
	}

	void nextInput(int si, int sj, int ti, int tj) {
		if (verbose) {
			trace("step: ", step);
			trace("from (", si, ", ", sj, ")");
			trace("to (", ti, ", ", tj, ")");
		}
		query = computePath(hw2, vw2, si, sj, ti, tj);
	}

	double measurePath(const string& path, double hw[N][N - 1], double vw[N - 1][N], int si, int sj) {
		int i = si;
		int j = sj;
		double res = 0;
		for (char c : path) {
			switch (c) {
			case 'U':
				res += vw[--i][j];
				break;
			case 'D':
				res += vw[i++][j];
				break;
			case 'L':
				res += hw[i][--j];
				break;
			case 'R':
				res += hw[i][j++];
				break;
			}
		}
		return res;
	}

	string computePath(double hw[N][N - 1], double vw[N - 1][N], int si, int sj, int ti, int tj) {
		dijk.solveAStar(hw, vw, si, sj, ti, tj);
		string res;
		int i = ti;
		int j = tj;
		int cnt = 0;
		while (i != si || j != sj) {
			if (++cnt > 1000) {
				assert(("too long", false));
			}
			int p = dijk.prev[i][j];
			int pi = p / N;
			int pj = p % N;
			int di = i - pi;
			int dj = j - pj;
			switch (di * 2 + dj) {
			case -2:
				res += 'U';
				break;
			case 2:
				res += 'D';
				break;
			case -1:
				res += 'L';
				break;
			case 1:
				res += 'R';
				break;
			default:
				assert(("invalid direction found", false));
			}
			i = pi;
			j = pj;
		}
		reverse(res.begin(), res.end());
		return res;
	}

	void queryResult(int si, int sj, int ti, int tj, int val) {
		if (verbose) {
			trace("result: ", val);
			trace("from (", si, ", ", sj, ")");
			trace("to (", ti, ", ", tj, ")");
		}

		int l = len(query);
		double mean = (double) val / l;
		int i = si;
		int j = sj;
		Query q;
		q.distance = (double) val;
		for (char c : query) {
			switch (c) {
			case 'U':
				q.vs.push_back(pii(--i, j));
				break;
			case 'D':
				q.vs.push_back(pii(i++, j));
				break;
			case 'L':
				q.hs.push_back(pii(i, --j));
				break;
			case 'R':
				q.hs.push_back(pii(i, j++));
				break;
			}
		}
		qs.push_back(q);

		double tmpH[N][N - 1] = {};
		double tmpV[N - 1][N] = {};
		for (const auto& ij : q.hs) {
			int i = ij.first;
			int j = ij.second;
			tmpH[i][j]++;
		}
		for (const auto& ij : q.vs) {
			int i = ij.first;
			int j = ij.second;
			tmpV[i][j]++;
		}
		smooth(hyper::explSmooth, hn, vn);
		repH(i, j) hn[i][j] += tmpH[i][j];
		repV(i, j) vn[i][j] += tmpV[i][j];

		updateWeights0();
		updateWeights1();
		//weightMix = 0;

		repH(i, j) {
			double w0 = param0.hw(i, j);
			double w1 = param1.hw(i, j);
			hw[i][j] = w0 + (w1 - w0) * weightMix;
		}
		repV(i, j) {
			double w0 = param0.vw(i, j);
			double w1 = param1.vw(i, j);
			vw[i][j] = w0 + (w1 - w0) * weightMix;
		}
	}

	void drawGuess(Graphics& g) const {
		double size = 100.0 / (N - 1);
		double mi = 1e9;
		double ma = -1e9;
		repH(i, j) {
			mi = min(mi, hw[i][j]);
		}
		repV(i, j) {
			ma = max(ma, vw[i][j]);
		}

		mi = 1000;
		ma = 9000;

		double a = 1 / (ma - mi + 1);
		double b = -mi * a;
		repH(i, j) {
			double x1 = j * size;
			double y1 = i * size;
			double x2 = x1 + size;
			double y2 = y1;
			double t = hw[i][j] * a + b;
			g.strokeWidth(0.1 + (1.2 - 0.1) * t);
			g.stroke(0, 0, 0);
			g.line(x1, y1, x2, y2);
		}
		repV(i, j) {
			double x1 = j * size;
			double y1 = i * size;
			double x2 = x1;
			double y2 = y1 + size;
			double t = vw[i][j] * a + b;
			g.strokeWidth(0.1 + (1.2 - 0.1) * t);
			g.stroke(0, 0, 0);
			g.line(x1, y1, x2, y2);
		}
	}

	void drawHeatmap(Graphics& g) const {
		g.strokeWidth(1);
		double size = 100.0 / (N - 1);
		double mi = 0;
		double ma = 10;
		double a = 1 / (ma - mi + 1);
		double b = -mi * a;
		repH(i, j) {
			double x1 = j * size;
			double y1 = i * size;
			double x2 = x1 + size;
			double y2 = y1;
			double t = hn[i][j] * a + b;
			g.stroke(0, t, 0);
			g.line(x1, y1, x2, y2);
		}
		repV(i, j) {
			double x1 = j * size;
			double y1 = i * size;
			double x2 = x1;
			double y2 = y1 + size;
			double t = vn[i][j] * a + b;
			g.stroke(0, t, 0);
			g.line(x1, y1, x2, y2);
		}
	}

	void drawNoise(Graphics& g) const {
		double size = 100.0 / (N - 1);
		double mi = -1000;
		double ma = 1000;
		double a = 1 / (ma - mi + 1);
		double b = -mi * a;
		repH(i, j) {
			double x1 = j * size;
			double y1 = i * size;
			double x2 = x1 + size;
			double y2 = y1;
			double t = param0.noiseH[i][j] * a + b;
			g.strokeWidth(0.1 + (1.2 - 0.1) * t);
			g.stroke(0, 0, 0);
			g.line(x1, y1, x2, y2);
		}
		repV(i, j) {
			double x1 = j * size;
			double y1 = i * size;
			double x2 = x1;
			double y2 = y1 + size;
			double t = param0.noiseV[i][j] * a + b;
			g.strokeWidth(0.1 + (1.2 - 0.1) * t);
			g.stroke(0, 0, 0);
			g.line(x1, y1, x2, y2);
		}
	}

private:
	vector<Query> qs;
	double hn[N][N - 1];
	double vn[N - 1][N];
	double weightMix;

	double computeExpected0(const Query& q) {
		double expected = 0;
		for (const auto& ij : q.hs) {
			int i = ij.first;
			int j = ij.second;
			expected += param0.hw(i, j);
		}
		for (const auto& ij : q.vs) {
			int i = ij.first;
			int j = ij.second;
			expected += param0.vw(i, j);
		}
		return expected;
	}

	double computeExpected1(const Query& q) {
		double expected = 0;
		for (const auto& ij : q.hs) {
			int i = ij.first;
			int j = ij.second;
			expected += param1.hw(i, j);
		}
		for (const auto& ij : q.vs) {
			int i = ij.first;
			int j = ij.second;
			expected += param1.vw(i, j);
		}
		return expected;
	}

	void learnSingle0(const Query& q, double t, double lr) {
		int n = len(q.hs) + len(q.vs);
		double actual = q.distance;
		double expected = computeExpected0(q);
		double diff = (actual - expected) / n;

		double delta = diff * lr;

		double noiseAmp = 1000 * param0.beta;
		double baseAmp = 4000 - noiseAmp;

		double noiseCoeff = hyper0::lrNoiseCoeff;

		for (const auto& ij : q.hs) {
			int i = ij.first;
			int j = ij.second;
			param0.hs[i] = clamp(param0.hs[i] + delta, 5000 - baseAmp, 5000 + baseAmp);
			param0.noiseH[i][j] = clamp(param0.noiseH[i][j] + delta * noiseCoeff, -noiseAmp, noiseAmp);
		}
		for (const auto& ij : q.vs) {
			int i = ij.first;
			int j = ij.second;
			param0.vs[j] = clamp(param0.vs[j] + delta, 5000 - baseAmp, 5000 + baseAmp);
			param0.noiseV[i][j] = clamp(param0.noiseV[i][j] + delta * noiseCoeff, -noiseAmp, noiseAmp);
		}
	}

	void learnSingle1(const Query& q, double t, double lr) {
		int n = len(q.hs) + len(q.vs);
		double actual = q.distance;
		double expected = computeExpected1(q);
		double diff = (actual - expected) / n;

		double delta = diff * lr;

		double noiseAmp = 1000 * param1.beta;
		double baseAmp = 4000 - noiseAmp;

		double thetaCoeff = hyper::lrThetaCoeff;
		double noiseCoeff = hyper::lrNoiseCoeff;

		for (const auto& ij : q.hs) {
			int i = ij.first;
			int j = ij.second;
			double s = param1.computedHS[i][j];
			double* p = param1.hs[i];
			double a = p[0];
			double b = p[1];
			double theta = p[2];
			double dwda = 1 - s;
			double dwdb = s;
			double dwdtheta = (b - a) * s * (s - 1) * thetaCoeff;
			p[0] += delta * dwda;
			p[1] += delta * dwdb;
			p[2] += delta * dwdtheta;
			p[0] = clamp(p[0], 5000 - baseAmp, 5000 + baseAmp);
			p[1] = clamp(p[1], 5000 - baseAmp, 5000 + baseAmp);
			p[2] = clamp(p[2], 0.1, 0.9);
			param1.noiseH[i][j] = clamp(param1.noiseH[i][j] + delta * noiseCoeff, -noiseAmp, noiseAmp);
		}
		for (const auto& ij : q.vs) {
			int i = ij.first;
			int j = ij.second;
			double s = param1.computedVS[i][j];
			double* p = param1.vs[j];
			double a = p[0];
			double b = p[1];
			double theta = p[2];
			double dwda = 1 - s;
			double dwdb = s;
			double dwdtheta = (b - a) * s * (s - 1) * thetaCoeff;
			p[0] += delta * dwda;
			p[1] += delta * dwdb;
			p[2] += delta * dwdtheta;
			p[0] = clamp(p[0], 5000 - baseAmp, 5000 + baseAmp);
			p[1] = clamp(p[1], 5000 - baseAmp, 5000 + baseAmp);
			p[2] = clamp(p[2], 0.1, 0.9);
			param1.noiseV[i][j] = clamp(param1.noiseV[i][j] + delta * noiseCoeff, -noiseAmp, noiseAmp);
		}
	}

	double powMix(double from, double to, double t, double p) {
		return from + (to - from) * pow(t, p);
	}

	void updateWeights0() {
		int n = len(qs);
		double t = step / 1000.0;

		double lr = hyper0::lrFrom + (hyper0::lrTo - hyper0::lrFrom) * t;

		param0.beta = powMix(hyper0::betaFrom, hyper0::betaTo, t, hyper0::betaPow);

		rep0(_, 2000) {
			int i = rnd.nextInt(n);
			learnSingle0(qs[i], t, lr);
		}

		double smoothCoeff = powMix(hyper0::noiseSmoothFrom, hyper0::noiseSmoothTo, t, hyper0::noiseSmoothPow);
		smooth(smoothCoeff, param0.noiseH, param0.noiseV);

		double decay = powMix(hyper0::decayFrom, hyper0::decayTo, t, hyper0::decayPow);
		rep0(i, N) {
			param0.hs[i] += (1000 - param0.hs[i]) * decay;
			param0.vs[i] += (1000 - param0.vs[i]) * decay;
		}
	}

	void updateWeights1() {
		int n = len(qs);
		double t = step / 1000.0;

		double lr = hyper::lrFrom + (hyper::lrTo - hyper::lrFrom) * t;

		param1.alpha = powMix(hyper::alphaFrom, hyper::alphaTo, t, hyper::alphaPow);
		param1.beta = powMix(hyper::betaFrom, hyper::betaTo, t, hyper::betaPow);

		rep0(_, 2000) {
			int i = rnd.nextInt(n);
			learnSingle1(qs[i], t, lr);
		}

		double smoothCoeff = powMix(hyper::noiseSmoothFrom, hyper::noiseSmoothTo, t, hyper::noiseSmoothPow);
		smooth(smoothCoeff, param1.noiseH, param1.noiseV);

		double decay = powMix(hyper::decayFrom, hyper::decayTo, t, hyper::decayPow);
		rep0(i, N) {
			param1.hs[i][0] += (1000 - param1.hs[i][0]) * decay;
			param1.hs[i][1] += (1000 - param1.hs[i][1]) * decay;
			param1.vs[i][0] += (1000 - param1.vs[i][0]) * decay;
			param1.vs[i][1] += (1000 - param1.vs[i][1]) * decay;
		}

		double diffSum = 0.0;
		rep0(i, N) {
			diffSum += abs(param1.hs[i][0] - param1.hs[i][1]);
			diffSum += abs(param1.vs[i][0] - param1.vs[i][1]);
		}
		double mean = (((((-2.277e6 * t + 8.83568e+06) * t + -1.38415e7) * t + 1.11289e7) * t + -4.77589e6) * t + 986762) * t + 397.718;
		double div = hyper::divFrom + (hyper::divTo - hyper::divFrom) * t;
		double p = (diffSum - mean) / div;
		p += powMix(hyper::shiftFrom, 0, t, hyper::shiftPow);
		weightMix = 1 / (1 + exp(-p));
	}

	void smooth(double coeff, double hw[N][N - 1], double vw[N - 1][N]) {
		double l[N - 1];
		rep0(i, N) {
			rep0(k, N - 1) {
				int p = max(0, k - 1);
				int n = min(N - 2, k + 1);
				l[k] = hw[i][k] * 2 - hw[i][p] - hw[i][n];
			}
			rep0(k, N - 1) {
				hw[i][k] -= l[k] * coeff;
			}
		}
		rep0(j, N) {
			rep0(k, N - 1) {
				int p = max(0, k - 1);
				int n = min(N - 2, k + 1);
				l[k] = vw[k][j] * 2 - vw[p][j] - vw[n][j];
			}
			rep0(k, N - 1) {
				vw[k][j] -= l[k] * coeff;
			}
		}
	}
};

// -----------------------------------------------------------------------

class Judge {
public:
	bool localTest;
	bool multiEval;

	Judge() : localTest(false), multiEval(false) {
	}

	ll run(int seed, bool write = false, bool addSeed = false, bool render = false) {
		Rnd rnd;
		if (!localTest) {
			rnd.seed(seed);
		}

		Graphics g;
		Movie mov;

		if (render) {
			g.screen(210, 210);
		}

		Server& server = *new Server(rnd);
		Solver& solver = *new Solver(rnd);
		if (localTest) {
			server.setup(seed);
		} else {
			server.setup();
		}

		vector<vector<array<int, 4>>> additionalInputs;

		constexpr int NUM_ADDITIONALS = 100;

		if (multiEval) {
			additionalInputs.resize(1000);
			rep0(i, 1000) {
				rep0(j, NUM_ADDITIONALS) {
					int si;
					int sj;
					int ti;
					int tj;
					do {
						si = rnd.nextInt(N);
						sj = rnd.nextInt(N);
						ti = rnd.nextInt(N);
						tj = rnd.nextInt(N);
					} while (abs(si - ti) + abs(sj - tj) < 10);
					additionalInputs[i].push_back({ si, sj, ti, tj });
				}
			}
		}

		solver.currentAns = &server.ans;

		ostringstream in;
		ostringstream out;

		Answer& ans = server.ans;
		rep0(i, N) {
			rep0(j, N - 1) {
				if (j > 0) in << " ";
				in << ans.hw[i][j];
			}
			in << endl;
		}
		rep0(i, N - 1) {
			rep0(j, N) {
				if (j > 0) in << " ";
				in << ans.vw[i][j];
			}
			in << endl;
		}

		if (render) {
			g.clear();
			solver.drawGuess(g);

			g.save();
			g.translate(0, 110);
			solver.drawHeatmap(g);
			g.restore();

			g.save();
			g.translate(110, 110);
			solver.drawNoise(g);
			g.restore();

			g.save();
			g.translate(110, 0);
			server.drawAnswer(g);
			g.restore();

			mov.addFrame(g);
		}

		double accScore = 0;
		double additionalAccScore = 0;
		rep0(i, 1000) {

			solver.nextStep();
			server.nextStep();

			if (multiEval) {
				additionalAccScore *= 0.998;
				rep0(j, NUM_ADDITIONALS) {
					int si = additionalInputs[i][j][0];
					int sj = additionalInputs[i][j][1];
					int ti = additionalInputs[i][j][2];
					int tj = additionalInputs[i][j][3];
					solver.nextInput(si, sj, ti, tj);
					additionalAccScore += server.exactRatio(si, sj, ti, tj, solver.query) / NUM_ADDITIONALS;
				}
			}

			int si;
			int sj;
			int ti;
			int tj;
			server.chooseRandomPoints(si, sj, ti, tj);

			solver.nextInput(si, sj, ti, tj);
			server.query(si, sj, ti, tj, solver.query);
			solver.queryResult(si, sj, ti, tj, server.queryResult);

			in << si << " " << sj << " " << ti << " " << tj << " " << server.actualDist << " " << server.e << endl;
			out << solver.query << endl;

			double score = server.ratio;
			accScore *= 0.998;
			accScore += score;

			if (render && i < 100) {
				g.clear();
				solver.drawGuess(g);

				g.save();
				g.translate(0, 110);
				solver.drawHeatmap(g);
				g.restore();

				g.save();
				g.translate(110, 110);
				solver.drawNoise(g);
				g.restore();

				g.save();
				g.translate(110, 0);
				server.drawAnswer(g);
				g.restore();

				mov.addFrame(g);
			}
		}
		ll finalScore = (ll) (2312311 * (multiEval ? additionalAccScore : accScore) + 0.5);
		trace("result: ", finalScore);

		if (write) {
			if (addSeed) {
				writeText("s" + to_string(seed) + "_i.txt", in.str());
				writeText("s" + to_string(seed) + "_o.txt", out.str());
			} else {
				writeText("in.txt", in.str());
				writeText("out.txt", out.str());
			}
		}
		if (render) {
			writeText("movie.html", mov.dumpHtml(60));
		}

		delete& server;
		delete& solver;

		return finalScore;
	}

	ll runMult(int seed, int num, bool printNums, int numThreads, bool localTest = false) {
		this->localTest = localTest;
		bool pdebug = debug;
		debug = false;

		Rnd rnd;
		if (!localTest) {
			rnd.seed(seed);
		}
		vector<int> seeds;
		rep0(i, num) {
			seeds.push_back(localTest ? i : rnd.getSeed());
			rnd.nextInt();
		}

		vector<thread*> ts;
		vector<ll> scores(numThreads);

		rep0(id, numThreads) {
			ts.push_back(new thread([this, &seeds, id, num, numThreads, &scores, printNums]() {
				int from = num * id / numThreads;
				int until = num * (id + 1) / numThreads;
				rep(i, from, until) {
					ll score = this->run(seeds[i]);
					if (score < 900000000) {
						lock_guard<mutex> lock(mut);
						cerr << "ARIENAI!!!!!" << endl;
						segv();
					}
					scores[id] += score;
					if (printNums) {
						lock_guard<mutex> lock(mut);
						cerr << i << ": " << score << " (s = " << seeds[i] << ")" << endl;
					}
				}
				}));
		}
		rep0(id, numThreads) {
			ts[id]->join();
		}
		rep0(id, numThreads) {
			delete ts[id];
		}
		ll sum = 0;
		rep0(id, numThreads) {
			sum += scores[id];
		}

		debug = pdebug;
		trace("result (n = ", num, "): ", sum / num);

		return sum / num;
	}
};








int main() {
	double m0_betaFrom[] = { 0.43168050979787403, 0.43411032371465424, 0.4853369794346569, 0.4496347092502997, 0.4544993141279772, 0.41410922764920066, 0.43639460690269183, 0.43256613430501634, 0.4520734668178796, 0.43849743242674355, 0.4432053851022531, 0.4304148595792855, 0.43160771956317084, 0.43989001990796317, 0.48439290258934864, 0.44840137250503664, 0.44827288173773927, 0.44240269097431956, 0.43664581470119074, 0.426023866649538, 0.445772660508688, 0.4329873978117679, 0.4408793399244499, 0.44069271149530415, 0.4346929930962663, 0.43992845636794586, 0.44639740998747046, 0.4328871847755384, 0.45441231910691576, 0.4429194517945822, 0.4333971737670561, 0.43273615679068034, 0.4271534881992227, 0.4837720964423976, 0.4315750425098687, 0.4411893935627629, 0.44633780007011437, 0.4356063188742885, 0.44782097950234856, 0.4321563116008821, 0.4390878472178816, 0.42957824758253993, 0.4222590160356275, 0.490890716824163, 0.4284072699235328, 0.4501578097355411, 0.44063579132185765, 0.4368330660386387, 0.42556268004908665, 0.42344688165439004, 0.4901509367806999, 0.5109528287293508, 0.5218632837013867 };
	double m0_betaTo[] = { 0.5906716743550079, 0.6430733844725854, 0.6709464817411802, 0.6246010118935142, 0.662284072964581, 0.6021588054004554, 0.6346840781139669, 0.6214925065789416, 0.6635871608804182, 0.5840061683514195, 0.6402104076077839, 0.5815838476648549, 0.6353090739884881, 0.6036183094818687, 0.6644000733619913, 0.6261082325038594, 0.6300164922179232, 0.6408213149224231, 0.6377080984052824, 0.5603752588300142, 0.620461407278517, 0.6088614765056167, 0.6153412025959764, 0.6364673961170056, 0.6025492073013382, 0.6049646183913275, 0.5912302964504068, 0.570623158783991, 0.5974968254370474, 0.6378991952039493, 0.6193739459190752, 0.6166087943045018, 0.5781034966077868, 0.6575370564519195, 0.5781941859998555, 0.6560834812799929, 0.6441233234431276, 0.5622021145429296, 0.5855801473626614, 0.5538090157240292, 0.5959717764951054, 0.5782955193319258, 0.6212056495146584, 0.6544551256717788, 0.6225428229940744, 0.6570663236136601, 0.6373127187378799, 0.5968696438206821, 0.5531708380522601, 0.5718816076744477, 0.5269420498319566, 0.5374241988126676, 0.6172175108258777 };
	double m0_betaPow[] = { 1.5993323530863932, 1.4026181612300517, 3.8307394044553003, 1.2472232599616295, 3.8206554753926856, 2.7954308581719602, 3.9291998605588314, 1.6783520212529233, 3.936277240406794, 1.6855519446649032, 2.1557085043508413, 1.5594576781437448, 1.9144431852272312, 1.8281700766313123, 3.996703621253424, 1.8457549983852692, 1.8968451287408017, 2.503893636026983, 1.978607550390526, 1.55213762033087, 1.7463200364688296, 2.36728376981675, 1.2666947115386693, 1.6720062828503734, 1.9682543562866373, 2.687202897958336, 1.6428986640829752, 1.9800130851193682, 1.90195911638957, 1.547840185378408, 1.8973097648693629, 1.6819592037053501, 1.7631756320800245, 3.949557156386459, 1.9103209188702643, 3.8568445625869505, 1.2202979448237405, 1.6944907945742707, 1.8766735641219752, 1.6371605323735328, 1.6795979817472488, 1.7315920132625955, 1.7047545011553562, 3.6648922774666657, 1.9594137487010859, 3.8678525374227917, 3.9227885011319206, 2.087562380394637, 1.712451622219596, 1.7790867587122259, 3.7361785539607046, 3.7005811127916357, 2.2145943374814943 };
	double m0_noiseSmoothFrom[] = { 0.16888867411709524, 0.17511148288937875, 0.1545222226015977, 0.1762146051013661, 0.1594634073327123, 0.18514258538346817, 0.16599498844305066, 0.16754163048465734, 0.16138115974746703, 0.16287207115495636, 0.18273553140770174, 0.11612139861994611, 0.16250404202984955, 0.1630398294580243, 0.15316889034811645, 0.16107334852118296, 0.167209940133437, 0.16730556445952333, 0.18314271914121874, 0.10008947883526108, 0.1706255103521532, 0.18272822250205592, 0.18565434631633726, 0.16556099096994673, 0.17107292681818417, 0.17587181586748035, 0.1634194271656944, 0.16831511151068404, 0.18517312372062622, 0.17082227078167633, 0.16306666414660623, 0.1672977536940046, 0.18850777681454547, 0.1609814199561256, 0.17113227907823841, 0.1685387932275433, 0.18092918287047755, 0.10470786485099054, 0.16397109828648115, 0.1822300285456666, 0.16511105418529162, 0.18574462422922286, 0.16915699701079306, 0.15429368034722704, 0.16432703009005842, 0.16564008939871155, 0.16182188632746497, 0.17684296952304007, 0.11022676482397359, 0.10084847977783833, 0.17108498957391188, 0.17033951881239098, 0.1551702065961937 };
	double m0_noiseSmoothTo[] = { 0.028117101020565624, 0.026739082458261787, 0.018864754733005175, 0.03053656378450674, 0.03167776084750219, 0.02396416776706156, 0.0285700513555078, 0.03112847854525206, 0.03110669543583216, 0.03047569968808563, 0.03488624443957493, 0.011189703744881277, 0.030123759785399565, 0.028938756720651784, 0.01601078499707379, 0.031965154718624204, 0.02668492066723912, 0.030148087496070158, 0.032222057349528155, 0.011610325982912282, 0.028489174130947103, 0.02896573688009431, 0.03177487960813639, 0.026838763611018283, 0.027175368515717852, 0.029589599672107433, 0.030652338748107688, 0.025956750563971894, 0.025205077787091648, 0.026652708470891098, 0.029121712293451573, 0.031682097863558266, 0.029049991063546932, 0.014539032619683, 0.025840117417502247, 0.030291131815126323, 0.03314343211794585, 0.012708110773044132, 0.03312845863620956, 0.010694455547312548, 0.030947834839382172, 0.02921172258831686, 0.03215980624207281, 0.01732646199613297, 0.02965957109879551, 0.03102061383634232, 0.031274426679856596, 0.012982246040813246, 0.01167636409655517, 0.008797454055716542, 0.02294736058652253, 0.017336645322473677, 0.005678414231518539 };
	double m0_noiseSmoothPow[] = { 1.7894911597581407, 1.7169987658993608, 1.741599633803018, 1.8133855262001504, 1.694290671381971, 1.7382261050703454, 1.7044954372146066, 1.7526780986866133, 1.6797236265826525, 1.7893661331789807, 1.7538002461429336, 1.7812336459671227, 1.7210858717903166, 1.8415268345016433, 1.7030462683383691, 1.8006048476524827, 1.813056706953221, 1.6964516334404696, 1.7121366894555603, 1.790265289505291, 1.8034974819292446, 1.8519843025130298, 1.7616813408076901, 1.7964222509724586, 1.74695399200243, 1.877935471386513, 1.7740565779163835, 1.8005731404555594, 1.797522653090598, 1.788893775831984, 1.7652449420065215, 1.7678580867323448, 1.7849693664500221, 1.6879746307279138, 1.807911268591369, 1.69593129973425, 1.7585163637435721, 1.745475180659265, 1.8022454825805527, 1.797506700793291, 1.7893538143272547, 1.7961537366497833, 1.7508283203680395, 1.9625957356836985, 1.7379551075243174, 1.683330428805148, 1.7227507543376903, 1.7001924503898618, 1.7799039758961108, 1.772087493194701, 1.9257640746813514, 1.9973261827398918, 1.808119635246289 };
	double m0_lrFrom[] = { 0.007253488240152035, 0.007864460548113362, 0.011139930362512821, 0.00629155753144889, 0.007548484601265552, 0.005923168109111466, 0.007363002441289779, 0.0072697658691431325, 0.008113138299879075, 0.006448499985370686, 0.006980376037516471, 0.005458966482114446, 0.007050034717687145, 0.007070927047813333, 0.00953787971679795, 0.00691767361939756, 0.007341123013480134, 0.007429603060839833, 0.006662242416458304, 0.005042463468854374, 0.007030763394766499, 0.00816380292093397, 0.006636802050066419, 0.006669814308197331, 0.006298938054162339, 0.007508900318647202, 0.006573545088912586, 0.007202722918234064, 0.0064079087246825366, 0.005970615773978559, 0.007998409506051223, 0.007307853458301535, 0.004971617487997708, 0.009331399847363765, 0.007051495611229083, 0.00728324849891336, 0.007338114333724337, 0.0052697717081382585, 0.006064796858833172, 0.005191092966647245, 0.006383967568451039, 0.005557809749057888, 0.006891787095779055, 0.011259425783344847, 0.0071531230808165695, 0.007549359896769578, 0.007082933398069647, 0.00785427053513697, 0.004914523395586812, 0.005254802439646773, 0.01643758837984981, 0.01633764956745028, 0.012951282058387645 };
	double m0_lrTo[] = { 0.003154513655901542, 0.003885391138177943, 0.00430519019298029, 0.003647645750915417, 0.004106861181321888, 0.003912415966409197, 0.0039044160520589574, 0.003984760354612552, 0.004095842685950474, 0.0026383486594240865, 0.0036843703303189542, 0.003936920070300974, 0.003905582360392863, 0.002947859052072087, 0.004331498087264393, 0.003668442475125323, 0.0038915693865251857, 0.004017260062368589, 0.0038297202262433756, 0.004179081332063771, 0.0037998131978131083, 0.003986238723686864, 0.0039323694984581105, 0.003845587505220532, 0.0036871215191588507, 0.0039378944440842745, 0.0037436943300615303, 0.0037984018331384756, 0.003619408993692092, 0.0037024822339990666, 0.003938705132383605, 0.0039003431798798308, 0.004059173785684689, 0.004354764980385779, 0.003786053716022216, 0.00419709707231426, 0.0037474512027496454, 0.003968882675658626, 0.0030531591452936475, 0.003967577529898393, 0.0026489878448177494, 0.0040008734769904275, 0.0035574651715299306, 0.004792776242181053, 0.003724305173778409, 0.004179546231819961, 0.003993822519032402, 0.004065951578875971, 0.003997431259947032, 0.004035662022572327, 0.003975120966442305, 0.003959044912186084, 0.003906340828337396 };
	double m0_lrNoiseCoeff[] = { 8.803878986262681, 9.721128151151772, 9.634441740655916, 8.921009930873547, 9.312397053688272, 9.399105586810386, 9.507544137552967, 9.677568978582954, 9.367347591963437, 8.6715584647016, 9.907210321501724, 8.538194851886832, 9.41600292645547, 8.579052919309857, 9.719663650926453, 8.659170572842628, 9.594974331689546, 9.23845846054707, 9.491637855519189, 8.61163854195151, 8.00818715187322, 9.096894377303633, 9.267581866377654, 8.695025468581829, 9.482419519015062, 9.419927468460656, 8.667949627540947, 8.213434353429408, 9.817094417777383, 8.347502428103759, 9.386008579331675, 9.7170643536327, 8.990096380445179, 9.039527326676067, 8.125710166260616, 9.322564601778852, 9.874945789831678, 9.203597728068614, 8.638439559960803, 8.800838299029103, 8.260149963331608, 9.208186449739175, 9.792921669607464, 11.247707373977024, 9.572790394762295, 9.404593135265225, 9.722106589915022, 9.351409078742634, 8.946695984655566, 9.220611985950924, 13.97534771987497, 14.543640078137694, 10.823431404515485 };
	double m0_decayFrom[] = { 0.0022726620681522342, 0.003393595996009679, 0.004764764147638811, 0.002981641398009799, 0.00397962801093908, 0.0037120342644663363, 0.004250964685488821, 0.0035072594074641206, 0.0035397820156859647, 0.0030337444159756954, 0.0032338775480089317, 0.0034309089640348974, 0.0035999809759079825, 0.002631457129966545, 0.003959730404553528, 0.002655064342396906, 0.0029794670412879197, 0.004575873419026617, 0.003317701387230476, 0.0024681601122029558, 0.0026986933475177033, 0.0029987283633574175, 0.003012400939127929, 0.0027789429035571652, 0.0033585624936563973, 0.0029608443884856475, 0.002344226914337784, 0.0031205106365901373, 0.0025184374778150173, 0.002739982708908869, 0.002625522813356212, 0.0030924028607826944, 0.002329463531185458, 0.00419872796398721, 0.002518612425085223, 0.0035390159540990957, 0.003334734364572398, 0.002539826979513164, 0.0028503473958997696, 0.0033944822355142863, 0.0027218512694237285, 0.003546399698264654, 0.0032230638844057157, 0.004797408330299268, 0.0029584101398647325, 0.0035735992976301703, 0.003684029237032339, 0.003238390987266815, 0.0023519710588157257, 0.003207887619250173, 0.004191370098446317, 0.0030863203150191905, 0.005547863073471035 };
	double m0_decayTo[] = { 1.6618693388011768e-06, 4.8385001835098966e-05, 1.9073253439624198e-05, 1.736011063854464e-05, 1.6094365525987833e-05, 4.89121675592579e-05, 1.0430295978166989e-05, 9.434114906297854e-06, 3.904072992717334e-06, 1.1066996667206033e-06, 6.835051868176107e-08, 0.00027396396779756567, 1.0057886949175728e-05, 3.417066035793079e-05, 7.11705499749132e-05, 1.680191743423383e-05, 2.2120862779005708e-05, 9.278122063404746e-06, 2.2346388021781704e-05, 0.00030622879051937947, 3.155093190308006e-05, 0.0002714157717442764, 3.463620260850288e-05, 2.628135062085701e-07, 2.9372890590682008e-05, 8.995777992288664e-06, 4.4402377064050677e-07, 2.883073377699467e-07, 2.6900353168832452e-05, 2.035668637073171e-05, 3.1389772428767555e-07, 9.330794899456878e-06, 0.00027470447902269, 3.7205562926884455e-05, 1.0188389102771902e-05, 1.1467116894321797e-05, 9.168909054243601e-08, 0.00030434386011792875, 5.0188848294699867e-08, 0.0003066629091476736, 9.238936262646886e-06, 0.00028201439546988777, 2.000446539574605e-07, 5.378183673083396e-05, 8.787867048613105e-07, 6.526626511804876e-06, 1.0837386858791288e-05, 3.2927708997610043e-05, 0.0002965387237371067, 0.0003219070397512778, 5.908384456087243e-05, 6.614571688372672e-05, 0.0002868762806082863 };
	double m0_decayPow[] = { 1.3222410492733099, 1.3151860953996841, 0.8235272459088977, 1.3414028029748353, 1.034649338444941, 1.3129587468505126, 0.9995961019928843, 1.023422181640907, 1.0497961741579795, 1.3213878204157476, 1.3569089367834515, 1.2952626956822129, 1.0388760225758882, 1.405916076643271, 0.861124560102971, 1.373047180428614, 1.3344924589049192, 1.014837632744992, 1.0167675204869582, 1.1796349705827454, 1.3389619905158592, 1.2144318908907916, 1.2158552542911072, 1.3491744119323106, 1.3716545174023538, 1.2458738412458685, 1.3236770253433696, 1.3124775512762088, 1.3874143307423312, 1.3286196369891177, 1.0300581918242853, 1.0211402313719344, 1.1926711500285823, 0.8324538987267995, 1.3160178736489327, 1.0470130414872474, 1.3510758908886527, 1.1088191715788782, 1.307287101461221, 1.1466644534425166, 1.420795363626522, 1.2025775367176124, 1.335882149359249, 0.8556596355248555, 1.0380744794156078, 1.030829437867098, 1.0299223540911095, 1.1996559497908101, 1.1932703772755373, 1.1284919134907014, 0.9998236969419342, 1.013926146448449, 0.9165831444714669 };
	double alphaFrom[] = { 5.254824608565864, 5.029962873991268, 5.079780176414592, 5.615768890721559, 4.366163132694595, 4.756377865338031, 5.3452262413670875, 5.401332114602997, 4.4356701130126615, 5.8196960565227585, 5.433970994758639, 5.50311530925711, 5.181362698266986, 5.2324412327923975, 4.106345493975514, 5.610200112419199, 5.096184548805686, 5.167371895173334, 5.514032216161075, 5.05561637201198, 5.150201839527635, 6.0524716876494, 5.461636204303084, 5.244597093290478, 5.5276122125860905, 5.460658123698279, 5.210781137569556, 5.533005586246783, 5.627250967070817, 5.233797976588157, 5.730622961993453, 5.811901847639899, 5.512641343910437, 4.652994412562135, 5.304680559919751, 5.030592031838445, 5.350443632678879, 5.403654266354811, 5.0697992426191325, 5.67718177746904, 5.154947821285264, 5.535036809310824, 5.751223041987219, 3.802276869132301, 5.819785011130775, 4.6992875038013775, 5.103060697610112, 5.2268177530418, 5.163248954970268, 5.480510884100727, 2.239146460723636, 1.80539083816742, 1.5426973178220007 };
	double alphaTo[] = { 64.38908183176082, 48.93684333293102, 129.3457592862684, 59.88280085695596, 84.89728550937032, 67.33521124319157, 67.0075009016032, 65.10055453643602, 65.54279800695552, 66.82971535348771, 57.157685888592944, 73.79347997857478, 71.29001130246033, 67.13958784581797, 121.56954234404458, 59.689111936738726, 50.27775983943112, 64.6697051794315, 62.90270139288728, 70.94017700097173, 50.41041036990016, 75.76697599192187, 63.80433736039006, 51.65273660958788, 59.96444905208205, 68.64359327413838, 70.06308160487815, 64.35589477139483, 59.90460901590304, 55.876040440090435, 50.897506337687624, 67.03725705600209, 78.97084415806717, 122.26787103822713, 68.38583540086837, 57.78406350005931, 62.305676643334536, 71.84826352714586, 67.94592938918541, 77.9986711365404, 68.9081406633988, 63.47542533291319, 61.692657587586666, 126.41627041487668, 52.65892738690281, 49.18015587707519, 67.58303415119225, 66.69391208935163, 70.1698390838653, 71.50290987205959, 106.8878160520919, 105.97994493105917, 105.32190460052178 };
	double alphaPow[] = { 2.3588543152510373, 2.3869845162938437, 2.296755736570605, 2.4388956232105925, 2.484544892439434, 2.351874220292242, 2.3373721595714443, 2.4155887273481906, 2.4743494460340223, 2.3526372643853404, 2.608681918553901, 2.4277115402056926, 2.4077141228066647, 2.4660328368977686, 2.395959153434846, 2.551865183179642, 2.342418722399619, 2.3237315291809937, 2.3487648141982707, 2.4823401143760546, 2.455795358983645, 2.320443902402416, 2.3140909685542788, 2.485457794917189, 2.405553841997535, 2.364126783425907, 2.4148580964102506, 2.468617165543564, 2.3824533696356887, 2.4709344900550634, 2.332139401544215, 2.4291284562416804, 2.4404135012572628, 2.4249736092501886, 2.431177015192679, 2.4056585795911065, 2.592764541014404, 2.6353289922602987, 2.488559581774511, 2.3939179444821184, 2.4215662936860514, 2.4245705250546776, 2.499448940183542, 1.9337171587028361, 2.274336002760071, 2.432762459091093, 2.372209131909712, 2.4098960429697325, 2.517180721620845, 2.653057524661569, 2.0470579546134515, 1.8592858041175802, 1.0940364377459224 };
	double betaFrom[] = { 0.8962796798193784, 0.878205928139804, 0.8087267368106024, 0.9597810922326715, 0.871136101499319, 0.9034442032506221, 0.8956592167202506, 0.9108378184551579, 0.8813167749680394, 0.8917575565857594, 0.970063578407369, 0.924361261612236, 0.9007856185309137, 0.8809618135636923, 0.8132253126974417, 0.9489958616761389, 0.9141481904818188, 0.8727958093557874, 0.886706515547767, 0.8985353142377684, 0.936219952094677, 0.8705022442066299, 0.9379216029232647, 0.9409346948403751, 0.9146316761511788, 0.8902135795881406, 0.8964764651469181, 0.9016389371101552, 0.9342533226081988, 0.9332875789871997, 0.9098315320582879, 0.9257035128565049, 0.9188590705296611, 0.8054616291611587, 0.8993573077443441, 0.8763494141339399, 0.9532900244802438, 0.9101787727428663, 0.919291384343942, 0.9149530380141657, 0.9050942652476346, 0.8837551662427136, 0.9463295510211707, 0.8297507874116831, 0.8894290175905387, 0.8739426851899232, 0.9051906379963888, 0.8833952620714709, 0.9189283509362084, 0.8981538415568846, 0.7574597891681315, 0.7053763994908713, 0.7607367294763577 };
	double betaTo[] = { 1.6208396080449103, 1.9753155657328385, 2.438543451796024, 1.9397578572652003, 2.244534376960529, 2.002871350629573, 2.060794204119176, 2.048368012291417, 2.2561973388491245, 1.600226400371723, 1.8032098495671107, 1.6762007232784382, 1.9303280430616652, 1.7295222375691355, 2.4005942789059374, 1.850735191178998, 1.7514278900413494, 2.0279835166722173, 1.8826273822878579, 1.540998971556078, 1.7273560096631688, 1.63368517144263, 2.031395920518213, 1.6911709381076303, 1.8635916160752766, 1.6923507402884184, 1.5949900049542418, 1.9139512690793272, 1.8053316314966368, 1.7039190354474927, 1.9442317253321666, 1.9294240704161902, 1.612496425338431, 2.3690963705826737, 1.9550858397525612, 2.123538152883599, 1.7951338237993912, 1.6871055696159163, 1.7273656177945125, 1.5803631949403072, 1.5753540433332907, 1.6262328013680611, 1.8696263359930858, 2.4444085415756516, 1.9424165845006516, 2.1432072124841333, 2.0336620478735368, 1.6221363953655243, 1.5462533173659172, 1.5101855396751562, 2.293552650007355, 2.3464173541389717, 1.642282768283551 };
	double betaPow[] = { 3.4571475835660417, 3.834077197092487, 3.3069838197918195, 3.397879734420542, 3.567920904943655, 3.556950131640732, 3.534704694114803, 3.4766571842304397, 3.709242565015779, 3.4647209233435072, 3.5443665280731667, 3.724053959305341, 3.5507601451797237, 3.5443704558711153, 3.6013360903629734, 3.522065693357022, 3.464720894650407, 3.6032749554951558, 3.4963824361982674, 3.798493045633606, 3.5357544195287223, 3.6874117044773276, 3.353496276433063, 3.5228687975408275, 3.772569463776348, 3.685655244748061, 3.517538102595095, 3.4580449330472067, 3.7267366535773623, 3.547118169740701, 3.435115491797867, 3.4585958457459522, 3.8988729828834567, 3.5380065455200858, 3.5025441015107104, 3.5408497280969073, 3.7133625245404995, 3.9944472215524933, 3.5919120980069237, 3.7639600902180685, 3.534164428547499, 3.907534898851024, 3.390976847040358, 3.503492571914976, 3.54468374144552, 3.541415226187822, 3.496392574278016, 3.6919707505988413, 3.7811505419065963, 3.9391672465039336, 1.8238060177528959, 1.8340724069139813, 1.0897594086453337 };
	double noiseSmoothFrom[] = { 0.009495254110607825, 0.01827254602188722, 0.01934513287798574, 0.009625450480530361, 0.01922567744713412, 0.010733572463730967, 0.01958006017846734, 0.01994420697591297, 0.019288949933144346, 0.00725384420631992, 0.011087826743111977, 0.009711631431869153, 0.019973096447023143, 0.0064663027598850994, 0.01778514759297988, 0.008334858999234683, 0.019003945691585387, 0.01887366076204842, 0.01997749848136767, 0.009132455053808029, 0.019047896702570566, 0.019611728045235286, 0.010234840361416685, 0.019117552157321166, 0.01006929657334231, 0.019986617406309703, 0.00937009725244259, 0.009515719037272729, 0.00985033892715164, 0.018757559666343397, 0.019599887349875708, 0.01994131742213355, 0.009276982858735083, 0.01801231923462067, 0.009341946933831682, 0.019988346036000706, 0.00889036641339041, 0.010846585699127033, 0.008750004156834253, 0.009947775863135003, 0.008477369883427977, 0.0196495803463844, 0.009117466763145449, 0.018373232156565267, 0.019618043672786543, 0.01925914235056496, 0.019948391029493084, 0.0196384782373224, 0.010513676747818996, 0.011117858254512218, 0.014112660365317534, 0.011780543636670412, 0.016013768465276388 };
	double noiseSmoothTo[] = { 0.06211671380272435, 0.07865296131299815, 0.1436386451884818, 0.06688190827277765, 0.08580197168728362, 0.08341139149399183, 0.08640309266134057, 0.05304150922599454, 0.09285060659060483, 0.08146874625752458, 0.059509140600263966, 0.061094289983995985, 0.06332010287374679, 0.08452653910345137, 0.07901553222234643, 0.07351044540601726, 0.0731593969123977, 0.08168542864301369, 0.07339966939076008, 0.046406350012333064, 0.06943284320201788, 0.09402701244873618, 0.07787554615501957, 0.07881583146692867, 0.05736310642764382, 0.07650323089214056, 0.08531249984167737, 0.06689130485361486, 0.07875163582018672, 0.0778412172222697, 0.07182530283835106, 0.05616120289185859, 0.053415067081070675, 0.08315868053419541, 0.07023043796058351, 0.09474517906753008, 0.06889172603860474, 0.04949963630125549, 0.06801227667203595, 0.06209468078426367, 0.08147880311308804, 0.06601662463053072, 0.0557826135362981, 0.15503898960063858, 0.0619613735567328, 0.09305187031870167, 0.07031612833987438, 0.08182943486389184, 0.04662341476783394, 0.04058216473860647, 0.14153580398772375, 0.1307262147028439, 0.12620802148554552 };
	double noiseSmoothPow[] = { 0.6121860081673645, 0.5099007224490157, 1.1727650590296406, 0.5850044086311734, 0.5323116381724065, 0.5591428118260166, 0.5291262285435533, 0.5881982544223013, 0.5377490351795045, 0.5314209549401628, 0.6074580894044375, 0.3947016995312751, 0.5701991259234492, 0.6106114187211452, 0.5093043478495929, 0.6709953418273422, 0.5587496026008624, 0.5290218536058737, 0.5607895441331386, 0.2696936459287618, 0.5891507103631379, 0.4267865093600051, 0.5497162634848686, 0.5718631715104452, 0.5577039043007632, 0.43329007404228825, 0.6722707288471961, 0.5459720719992212, 0.6249699729445561, 0.6237817650099278, 0.5723430956300215, 0.6186438638476072, 0.317766614189213, 0.35750098212229414, 0.6194181662744813, 0.5395717104176097, 0.6013116165589263, 0.36428904295449727, 0.5810215234726798, 0.3851253906872479, 0.6109589834724232, 0.37641601619545884, 0.6271869759821626, 0.5397539351910364, 0.5630905925035571, 0.5447727991433112, 0.5674271691422698, 0.5037751189573426, 0.27369451022971564, 0.4164837369213963, 0.8025966394408517, 0.8343116466119108, 0.6759464017868984 };
	double lrFrom[] = { 0.023332638168537506, 0.02499489124859026, 0.02042551876143923, 0.024138284111425858, 0.024968223415836422, 0.02169822087919382, 0.024018536114831096, 0.02363021474828258, 0.024809796498769406, 0.023828012660292372, 0.02160748514239494, 0.02177745028437582, 0.024572802355378592, 0.02325891380342952, 0.02156294073752774, 0.023120991568940943, 0.02499205374379939, 0.024160081079358447, 0.02446238972100269, 0.021245407480358088, 0.023960399858772564, 0.021031239617329205, 0.021964351090258627, 0.022785359841163158, 0.02337164880029271, 0.02256308588030549, 0.022405844511807507, 0.024245238331264577, 0.02200506712862474, 0.02289131858570348, 0.02356937069329402, 0.02456203215473262, 0.02087383328938515, 0.022603998748520662, 0.02250235771255032, 0.02438144333472694, 0.021669995294122377, 0.022420747305787782, 0.022333000955910245, 0.02196454041112023, 0.02238692443568315, 0.021418917253974667, 0.0231033498601067, 0.021246060583374964, 0.024989022145705333, 0.024955292766528633, 0.024581757653966724, 0.021550873094938118, 0.0218192897981379, 0.022608067628998467, 0.02163925837400762, 0.020112363268202458, 0.014208554350719285 };
	double lrTo[] = { 0.00624847477464662, 0.006680524650165458, 0.008690018394711462, 0.0060850501670202135, 0.006757878386803595, 0.00813983899577583, 0.00642701764050344, 0.007493516766481235, 0.006275607080112474, 0.006905301634213992, 0.005382336935227863, 0.006845493770489097, 0.007114115765221817, 0.006958314390118674, 0.010048214819229448, 0.005308254181344869, 0.007078528346859704, 0.006437856254893608, 0.006262280209631143, 0.007849981838086722, 0.006522230186488853, 0.006373122926381494, 0.005721125880582885, 0.0062861833374551574, 0.007429323457468747, 0.006050433928219566, 0.005873892329078364, 0.007074600352594786, 0.005522200762382317, 0.005824341676457287, 0.007648244029100838, 0.007629102689485784, 0.00799457984976805, 0.010260135921414545, 0.0057754049561453856, 0.005991503695891675, 0.0062918499073892405, 0.008304597305203538, 0.006737152833482714, 0.007046950567446529, 0.005703119123451931, 0.006584491039166249, 0.006823499546899981, 0.009531715511817782, 0.007171071777847411, 0.005766381488531346, 0.007414320886814332, 0.006452120957972506, 0.007322731866477778, 0.007690206092188521, 0.01137888525422293, 0.011783601239156396, 0.01179479111373691 };
	double lrThetaCoeff[] = { 1.4670217632768437e-08, 1.539797958877004e-08, 1.762929213429671e-08, 1.430417430100013e-08, 1.326887632077011e-08, 1.7892430584119757e-08, 1.5051290366310777e-08, 1.2597494137147588e-08, 1.1642290945785141e-08, 1.0057547397374019e-08, 1.7892496577639477e-08, 1.4296231002960824e-08, 1.0180870489451456e-08, 1.5724679069959492e-08, 1.813355948195628e-08, 1.3851912752764181e-08, 1.582657892290593e-08, 1.4884981428217738e-08, 1.528770757498841e-08, 1.174865646878661e-08, 1.153665424506538e-08, 1.4482941122933423e-08, 1.7328489476845144e-08, 1.1533209488661705e-08, 1.6466693055318738e-08, 1.3700070579869018e-08, 1.5683711250902045e-08, 1.419247492533577e-08, 1.84648469487668e-08, 1.1133487951729767e-08, 1.4300774423867728e-08, 1.1433295230427624e-08, 1.2951966872473484e-08, 2.142070825551687e-08, 1.2772120797684719e-08, 1.1657712005909638e-08, 1.6053697817717083e-08, 1.0057775819349152e-08, 1.3090867486598986e-08, 1.5506690601797253e-08, 1.468194658877204e-08, 1.4034478393498764e-08, 1.171451155253118e-08, 1.606304708798057e-08, 1.1722775683964783e-08, 1.1799442159133703e-08, 1.3296891984623513e-08, 1.559124627021582e-08, 1.75310594423031e-08, 1.4332489570464263e-08, 2.7542864119011365e-08, 2.350471424250425e-08, 2.7329186210812712e-08 };
	double lrNoiseCoeff[] = { 0.5924407949536235, 0.5020926308276394, 0.7343316066988536, 0.5808019499557638, 0.6929676471439024, 0.5576639840524087, 0.6557907995445457, 0.6484101642433759, 0.6817632974632571, 0.6119719641769106, 0.5358943570592449, 0.5013276313416366, 0.651571625264212, 0.5989747952323096, 0.7371721020835476, 0.6093966369249977, 0.6146378454495484, 0.6753530190149994, 0.659601714235452, 0.48825886634830074, 0.5872596041759037, 0.6847249019340439, 0.5397035262009141, 0.5753912299682404, 0.5730362848069336, 0.6782258058600397, 0.5545463587767453, 0.623165332392796, 0.5620336308165318, 0.5945305745366473, 0.6401075611580653, 0.6017946487261977, 0.4760636533899195, 0.7062531449542123, 0.6238028806718323, 0.6161358891383029, 0.5315195099962239, 0.5447090670837557, 0.5596604924218975, 0.5184183085929992, 0.5642071583254458, 0.5143827199575426, 0.6217002334661699, 0.726465021942014, 0.6358567232805926, 0.6476994255415158, 0.6563688534041597, 0.6524201733020606, 0.5064246666702691, 0.5134425233962948, 0.7570703540429197, 0.8394274952767969, 0.8222016106342592 };
	double explPow[] = { 0.0886869882940854, 0.06241785476524272, 0.07221887711351634, 0.07295142950909468, 0.08649638747470682, 0.0736347161766379, 0.07827202264008795, 0.07478452380450096, 0.08539918984282052, 0.07626179443831752, 0.09325032138477819, 0.08010728059578195, 0.07206953552512349, 0.08002339870478281, 0.09932484560501645, 0.08940148050718741, 0.08185922896208675, 0.07975011185654882, 0.061118690338018575, 0.07375096251996272, 0.07923088926465303, 0.06188524576476345, 0.06523966204573951, 0.08408879471616053, 0.08844173325446902, 0.0660261152844353, 0.0850085679007349, 0.08433309100675015, 0.08473431199705513, 0.08674252904728408, 0.07539744563598089, 0.07336178680267719, 0.07316545462267793, 0.09541899504062551, 0.08238739394316481, 0.07823564987092185, 0.08954840486124949, 0.083133708598384, 0.08316148556393613, 0.06961233404858092, 0.08212898325914285, 0.07140587653660681, 0.07397957538508149, 0.1553373479088585, 0.06128876162761042, 0.07905328044577652, 0.07037332787311201, 0.07658654475583891, 0.07439831209922021, 0.06769390920767367, 0.16094769509702345, 0.16510662046209848, 0.1200007413106781 };
	double explWeight[] = { 0.3220928561141716, 0.3603232092675027, 0.3823086606076308, 0.3446911811675691, 0.38669096371542044, 0.39028601038082644, 0.4112836178246164, 0.36461824436913437, 0.36505280173670374, 0.3434195462970399, 0.38672301951191557, 0.4535849371792126, 0.3539403005016813, 0.3516246923527705, 0.359519701518198, 0.3323569292844484, 0.36009923973548613, 0.40741756630842835, 0.32883622907029525, 0.4631530256187106, 0.35624298558860407, 0.44068202030281944, 0.3646128542997215, 0.36340164540417996, 0.35361007473454775, 0.4354611690499478, 0.3180423679851507, 0.3494007394242655, 0.39529411824607386, 0.3754073546809689, 0.3351681091341061, 0.35010870966196694, 0.45151379836614114, 0.4128747290003149, 0.32297660684437846, 0.36596838167430346, 0.37687982260938446, 0.4733093635272323, 0.3218352714936175, 0.4355178651622064, 0.31884968048898815, 0.453143498181824, 0.3664639706745352, 0.38131500566427573, 0.316442814104244, 0.3640528317338837, 0.39610007106907863, 0.4395748084502705, 0.4099069879257088, 0.44498403775979983, 0.46170655383971976, 0.45830442736282295, 0.3098142239467164 };
	double explMaxNum[] = { 12.478181866367198, 14.256033561637533, 7.547882683859277, 12.764575579491245, 6.489811868338236, 13.491626938304348, 6.348777116564281, 13.675767500262985, 6.513895924219063, 12.236569424315366, 14.276449007568196, 13.99484355657761, 6.409959670259515, 12.448541429437002, 7.977769440499204, 12.266422603555204, 12.081754524033798, 6.539615582032537, 13.280331709193668, 14.56164792676957, 12.28962256957353, 13.24202067548241, 12.69559537514049, 12.386140098497837, 13.28444464499541, 13.394358326087927, 11.83307823662081, 11.803646606671963, 13.243469091728874, 12.184007073945342, 13.282352767385714, 13.443312363714867, 14.032596028166946, 8.150584547065021, 12.109164931881365, 6.0063289969687705, 13.747884381196265, 13.958381731267588, 13.659543979975536, 13.859316234249635, 12.735038750802316, 14.047470551367446, 13.869667621416465, 7.215967931472939, 12.928603282624291, 6.305118199054293, 12.373603311906393, 13.585122712470849, 14.411875443992448, 13.85691903982948, 9.13070615256611, 8.167585282060951, 8.973653814503681 };
	double explSmooth[] = { 0.026442295951817397, 0.01856965954339899, 0.023173508262278604, 0.022278918499289983, 0.01923219971386727, 0.014522708181064378, 0.02628928218250898, 0.022073086262744658, 0.019032698822950025, 0.03637496745723381, 0.007034003261651361, 0.0274073445226175, 0.024935287314826464, 0.02729037867520972, 0.012250991796621577, 0.025925001268747274, 0.02079509107633968, 0.021766377362418217, 0.0338848008274678, 0.023452928292374654, 0.017887052197718748, 0.01587002609460937, 0.00040746462154590823, 0.025984644243842454, 0.02013898131338429, 0.014937154548313295, 0.029393276328640798, 0.022131490438239584, 0.016675273429632348, 0.02881118843586443, 0.024988623616723722, 0.021614999218820204, 0.021493457990253085, 0.010137365278011661, 0.02251714627539616, 0.018657415850896836, 0.009523069304394232, 0.030294490914076704, 0.0366413929832521, 0.012285403269278686, 0.030637841147577356, 0.020445431865212308, 0.02262216647263605, 0.052944336282036014, 0.01724257974799223, 0.017954031253441728, 0.025429384912594825, 0.014489976563535747, 0.022698612806714938, 0.019396812177530424, 0.14502590238796698, 0.14495834599751956, 0.07250991043566424 };
	double decayFrom[] = { 0.00578343660448812, 0.0060208318683248, 0.005876406624799595, 0.005961896782628535, 0.006220806375926909, 0.005869419651573885, 0.00595346269713484, 0.006378383671883614, 0.006303175045232365, 0.006048644131172283, 0.006978038740765592, 0.00789058899916887, 0.006332387452299315, 0.006162634524552263, 0.005546122232234292, 0.006496883149742868, 0.006165056450472294, 0.005956382909500003, 0.006256882545535048, 0.007297760909990642, 0.006318599797402095, 0.005892173426136848, 0.005607665652941265, 0.006316462804517047, 0.0066619598475234024, 0.0061223302126919175, 0.006060688772934195, 0.00590015074763896, 0.007126697501802558, 0.006576063385961072, 0.005944768979238821, 0.0059930278795082646, 0.005997982458306347, 0.005803411097087558, 0.005849671875883708, 0.006270441899843968, 0.0066296295332882536, 0.006474040869914722, 0.00595218269999907, 0.006104518697308329, 0.006179929006781122, 0.00604524667290365, 0.006764582571840064, 0.005303060452644267, 0.006295005999136627, 0.006170087213646209, 0.006064044093411088, 0.005982515155513273, 0.007455376419780415, 0.006192125375555212, 0.0038537439901450655, 0.004984447905191583, 0.005566077846209949 };
	double decayTo[] = { 0.00014597605957139401, 0.00022977470568971185, 0.0002010517542572844, 0.00014793270708471292, 0.00024722993300762343, 0.00025591122982167905, 0.00022586227454601552, 0.00023429363956805555, 0.0002314496061519249, 0.00016659835029933143, 0.0001958748308968181, 0.0012561725719541466, 0.00021036964891175218, 0.00025577241688570985, 0.00024018279999457148, 0.00017536045363953224, 0.000230542035639414, 0.00024981804365467115, 0.00018686934507488825, 4.0702565359450435e-05, 0.0001554026850980145, 0.00012382846081957353, 0.00011058676583440376, 0.00017395598311174403, 0.00014757963235420264, 0.00011729092515331805, 0.00019486429014632626, 0.00012167389463956095, 0.00011614379145863855, 5.057763280247775e-05, 0.00019870117378662877, 0.00016555044466262664, 4.264951721183322e-05, 0.00015896980768553127, 0.0001234726018822313, 0.0002574909258381816, 0.00019214305917354137, 0.00010766673296715006, 0.00020177909845588442, 4.8415191087919796e-05, 0.00019431513375111058, 4.7078690166305236e-05, 0.00022858888065885254, 0.00037033981873577353, 0.00019124021514869846, 0.00029351567985098356, 0.00022224397472717997, 0.00023388852220972617, 4.404628235464764e-05, 7.789004629344938e-07, 0.0003102516145270543, 0.00029430656371852296, 0.0013187155454013387 };
	double decayPow[] = { 0.5801876247053878, 0.6694044243894136, 0.6964290956834542, 0.5413278303405946, 0.7252218758570808, 0.6166168058631613, 0.688940914061285, 0.39356635323361544, 0.667381145620751, 0.6937644896700486, 0.3569137602726529, 0.10036059023940058, 0.5029089875463969, 0.5119527335777839, 0.743529994622667, 0.5021100567163586, 0.5477685457701826, 0.6882330758759203, 0.5971264808195861, 0.41800432709790714, 0.5477250329231191, 0.31760574641829864, 0.5866534134164917, 0.4643897729232479, 0.4764268694138979, 0.4278992814795242, 0.5624327400107775, 0.3707282718746575, 0.3789986659865383, 0.3626893095647743, 0.43892976234011344, 0.3610212100424279, 0.2857010616318284, 0.7794947653154648, 0.39558281031075104, 0.5355797623469484, 0.5506304611639795, 0.36254663611131177, 0.3840475418197923, 0.33485947869876337, 0.45560352728743425, 0.4801366853607799, 0.4301592726802527, 0.7701689863368537, 0.6644971803903168, 0.6808491008790853, 0.5927340790832343, 0.6238843659534561, 0.47683463998085224, 0.3141287194302338, 0.9024564757578307, 0.758445405181012, 0.9382309278547696 };
	double divFrom[] = { 7759.469905344231, 7600.2099880240585, 7136.803328776367, 7997.177607053557, 7773.800418542919, 7998.612672662138, 7619.989390786792, 7755.987573082909, 7985.252475517984, 7636.151848034903, 7781.559980829476, 7856.299135732354, 7858.078648200234, 7841.194539538382, 6998.260231570446, 7834.8331970207155, 7774.6906234465305, 7445.992508750515, 7846.271701121822, 7649.856822844337, 7855.014134084476, 7749.5708093609255, 7994.623818069597, 7766.560898429917, 7991.326379794835, 7712.574854001616, 7982.791723541789, 7579.127287452469, 7745.690684987099, 7587.14343005674, 7987.6550294458375, 7618.208381356963, 7723.682996418983, 7033.966846943305, 7528.701386404503, 7861.949776163354, 7853.007349307015, 7863.789288916906, 7997.266861304514, 7998.8673030826885, 7871.531067465766, 7880.2288086184235, 7997.296405128605, 7021.095224073854, 7840.151611220595, 7849.654603648538, 7986.501582823165, 7546.855145112422, 7647.9637241966375, 7612.751631794625, 7446.386751767488, 7307.415768175359, 7811.520832398932 };
	double divTo[] = { 3534.1294959183333, 4453.876265835707, 7876.1194433640385, 3902.9700771323596, 7987.224436126478, 4445.34934748754, 7773.358881586452, 7760.078487984974, 7892.316461085486, 3784.037855080772, 4410.492090565533, 4129.306117219708, 7987.836212202744, 3895.8635655411363, 7996.550923555172, 3561.665178876869, 7690.571365783602, 7752.695867810822, 7785.315262077008, 4033.0659262917206, 3519.6792851423106, 4182.430538153277, 4056.411521423886, 4077.8992187240724, 3756.294474719976, 4042.65446502126, 3403.8564573112044, 3399.7315730578903, 4009.6183243624378, 3310.372175551157, 7644.144726627803, 7749.926522829317, 3993.5825992851114, 7690.310700112653, 3644.840457204932, 7881.030270199966, 3529.640814784037, 3699.4917808498776, 3888.9684547192815, 4314.397138609503, 3832.420947539183, 3850.271740443414, 3755.5335203955074, 7369.068731973313, 7654.921151992126, 7978.8786281741, 7999.766675275559, 3866.976821955599, 4090.5521365338427, 3854.896609558897, 7335.33293779615, 7345.042290407665, 3400.1714338939037 };
	double shiftFrom[] = { 0.003612462060524176, 0.00034094756458993803, 0.26579485597428215, 0.16451948168566258, 0.00023509248145809833, 0.04211799408720673, 0.0531828317184051, 0.054027696871161886, 0.004982742728140643, 0.09585079002524438, 0.12572285992105783, 0.15673704660934124, 0.003234527830605228, 0.08963961014288539, 0.09938192731361203, 0.1346444460101946, 0.0016237841115471384, 0.04449555241388703, 0.002147824942512589, 0.20950015244922796, 0.0012262734373975036, 0.13198591573762697, 0.04862859140501092, 0.04548667325854215, 0.09393396128658486, 0.07948765740870813, 0.0037196483580275493, 0.10685280982219379, 0.09103212620644327, 0.002503477480367007, 0.0807083927223602, 0.08562244290295573, 0.126436448370291, 0.1043875440144461, 0.09936336070287113, 0.05542614406962042, 0.08617342712780525, 0.051377103553932886, 0.05195591083918532, 0.14500414794450242, 0.0012512842615009516, 0.07974253926590048, 0.17314105006620076, 0.21767386990094667, 0.0003230607919905294, 0.057471549124455484, 0.003377959948358815, 0.044757221521968205, 0.2286748885984402, 0.15221725914936657, 0.4362942701742768, 0.38743795944763537, 1.048104565221231 };
	double shiftPow[] = { 0.3267532323966636, 0.34793907088654313, 0.594400160672369, 0.2852770486753564, 0.39963689072953396, 0.35035196531297413, 0.3886024912814546, 0.32715970331199656, 0.4095068560390141, 0.33445495995370833, 0.24779703277479945, 0.21529084721910477, 0.3841117860511315, 0.3390938972225791, 0.4116528961530501, 0.2850457428575224, 0.32131038180247184, 0.37319745587402625, 0.34052308115463276, 0.2737319243960685, 0.3301822174109397, 0.3284555582155611, 0.3040956676959635, 0.325274091185406, 0.32107768395778874, 0.3257497604594406, 0.2974000914555742, 0.3247260998066797, 0.28433438854733173, 0.3223000396784351, 0.3743742576753358, 0.3189826063524013, 0.24166758277431616, 0.3458355544545935, 0.32536236453564193, 0.33681400375586945, 0.28326327466553775, 0.28321052406196734, 0.3050889199120455, 0.27419326208050127, 0.3096216247131613, 0.27480389190581334, 0.30709513917761383, 0.5715728289241683, 0.3803266607649577, 0.36798409824530876, 0.37842085578313567, 0.3506224171944188, 0.2801761325245244, 0.2826780784364987, 0.9467242331916422, 0.9279843945162181, 0.22194498537121962 };

	int i = 39;

	Judge j;
	j.multiEval = false;
	hyper0::betaFrom = m0_betaFrom[i];
	hyper0::betaTo = m0_betaTo[i];
	hyper0::betaPow = m0_betaPow[i];
	hyper0::noiseSmoothFrom = m0_noiseSmoothFrom[i];
	hyper0::noiseSmoothTo = m0_noiseSmoothTo[i];
	hyper0::noiseSmoothPow = m0_noiseSmoothPow[i];
	hyper0::lrFrom = m0_lrFrom[i];
	hyper0::lrTo = m0_lrTo[i];
	hyper0::lrNoiseCoeff = m0_lrNoiseCoeff[i];
	hyper0::decayFrom = m0_decayFrom[i];
	hyper0::decayTo = m0_decayTo[i];
	hyper0::decayPow = m0_decayPow[i];
	hyper::alphaFrom = alphaFrom[i];
	hyper::alphaTo = alphaTo[i];
	hyper::alphaPow = alphaPow[i];
	hyper::betaFrom = betaFrom[i];
	hyper::betaTo = betaTo[i];
	hyper::betaPow = betaPow[i];
	hyper::noiseSmoothFrom = noiseSmoothFrom[i];
	hyper::noiseSmoothTo = noiseSmoothTo[i];
	hyper::noiseSmoothPow = noiseSmoothPow[i];
	hyper::lrFrom = lrFrom[i];
	hyper::lrTo = lrTo[i];
	hyper::lrThetaCoeff = lrThetaCoeff[i];
	hyper::lrNoiseCoeff = lrNoiseCoeff[i];
	hyper::explPow = explPow[i];
	hyper::explWeight = explWeight[i];
	hyper::explMaxNum = explMaxNum[i];
	hyper::explSmooth = explSmooth[i];
	hyper::decayFrom = decayFrom[i];
	hyper::decayTo = decayTo[i];
	hyper::decayPow = decayPow[i];
	hyper::divFrom = divFrom[i];
	hyper::divTo = divTo[i];
	hyper::shiftFrom = shiftFrom[i];
	hyper::shiftPow = shiftPow[i];

	Rnd rnd;
	Solver sol(rnd);
	rep0(i, 1000) {
		int si, sj, ti, tj;
		cin >> si >> sj >> ti >> tj;
		sol.nextStep();
		sol.nextInput(si, sj, ti, tj);
		cout << sol.query << endl;
		int res;
		cin >> res;
		sol.queryResult(si, sj, ti, tj, res);
	}
}
