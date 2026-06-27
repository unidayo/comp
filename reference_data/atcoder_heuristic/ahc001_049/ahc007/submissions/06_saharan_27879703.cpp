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
		return (y = y ^ (y << 15)) & 0x7fffffff;
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

constexpr int N = 400;
constexpr int M = 1995;

class UF {
public:
	int ps[N] = {};
	int ss[N] = {};

	UF() {
		rep0(i, N) {
			ps[i] = i;
			ss[i] = 1;
		}
	}

	int root(int x) {
		while (ps[x] != x) {
			x = ps[x] = ps[ps[x]];
		}
		return x;
	}

	bool unite(int x, int y) {
		x = root(x);
		y = root(y);
		if (x == y) {
			return false;
		}
		if (ss[x] < ss[y]) {
			x ^= y;
			y ^= x;
			x ^= y;
		}
		ss[x] += ss[y];
		ps[y] = x;
		return true;
	}

	bool find(int x, int y) {
		return root(x) == root(y);
	}
};

class Solver {
public:
	Rnd rnd;
	UF uf;
	UF uf2;
	int vs[N][2] = {};
	int es[M][2] = {};
	int ds[M] = {};
	int ls[M] = {};
	int next = 0;
	int maxD = 0;
	int score = 0;
	bool silent = false;
	int type = 0;
	bool skip = false;

	Solver() {
	}

	void init(istream& is) {
		rep0(i, N) {
			is >> vs[i][0] >> vs[i][1];
			type += vs[i][0];
		}
		rep0(i, M) {
			is >> es[i][0] >> es[i][1];
			int a = es[i][0];
			int b = es[i][1];
			int dx = vs[a][0] - vs[b][0];
			int dy = vs[a][1] - vs[b][1];
			ds[i] = (int) (sqrt(dx * dx + dy * dy) + 0.5);
			maxD = max(maxD, ds[i]);
		}

		//type %= 3;

		//if (type != 0) {
		//	skip = true;
		//}
		//rnd.seed(12345678);

		skip = false;

		next = 0;
		score = 0;

	}

	void solveNext(istream& is) {
		int l;
		is >> l;
		bool res;
		if (skip) res = true;
		else res = determine(l);
		if (res) {
			uf.unite(es[next][0], es[next][1]);
			score += l;
			if (!silent) {
				cout << 1 << endl;
			}
		} else {
			if (!silent) {
				cout << 0 << endl;
			}
		}
		next++;
	}

private:
	bool determine(int l) {
		static int counts[2400];
		static int offs[2400];
		static int dst[M];

		int pos = 0;
		int neg = 0;
		int off = next;
		int m = M - next;
		ls[off] = l;
		constexpr int NUM = 1600;
		constexpr int HALF = NUM / 2;
		bool res = 0;
		const double prob = 0.37;
		const double svar = sqrt(prob * (1 - prob));
		rep0(iter, NUM) {
			uf2 = uf;
			int maxL = l;
			rep(i, off + 1, M) {
				ls[i] = rnd.nextInt(ds[i], ds[i] * 3);
				maxL = max(maxL, ls[i]);
			}
			maxL++;
			rep0(i, maxL) {
				counts[i] = 0;
			}
			rep(i, off, M) {
				counts[ls[i]]++;
			}
			offs[0] = 0;
			rep(i, 1, maxL) {
				offs[i] = offs[i - 1] + counts[i - 1];
			}
			rep(i, off, M) {
				dst[offs[ls[i]]] = i;
				offs[ls[i]]++;
			}
			rep0(i, m) {
				int idx = dst[i];
				int a = es[idx][0];
				int b = es[idx][1];
				bool res = uf2.unite(a, b);
				if (idx == next) {
					if (res) {
						pos++;
					} else {
						neg++;
					}
					break;
				}
			}

			res = pos > neg;

			int trial = pos + neg;
			if (trial >= 25) {
				double z = (pos - trial * prob) / (svar * sqrt(trial));
				constexpr double REJ = 3.87;
				if (z > REJ) {
					res = true;
					break;
				}
				if (z < -REJ) {
					res = false;
					break;
				}

				if (pos >= HALF) {
					res = true;
					break;
				}
				if (neg > HALF) {
					res = false;
					break;
				}
			}
		}
		trace("pos = ", pos, ", neg = ", neg);
		return res;
	}

};

Solver s;

int main() {
	timer(true);
#if 1
	debug = false;
	s.init(cin);
	rep0(i, M) {
		s.solveNext(cin);
	}
#elif 1
	debug = false;
	int num = 30;
	ll total = 0;

	rep0(i, num) {
		cerr << i + 1 << "/" << num << endl;
		Solver s;
		s.silent = true;
		string in = readText("ins/" + to_string(10000 + i).substr(1, 4) + ".txt").first;
		istringstream is(in);
		s.init(is);

		rep0(i, M) {
			s.solveNext(is);
		}
		total += s.score;
	}
	debug = true;
	trace("score: ", total);
#endif

	return 0;
}
