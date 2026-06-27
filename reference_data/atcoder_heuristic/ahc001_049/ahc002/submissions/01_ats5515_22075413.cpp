#include <assert.h>

#include <algorithm>
#include <bitset>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#define dump(x) cerr << #x << " = " << (x) << endl;
#define debug(x) cerr << #x << " = " << (x) << " (L" << __LINE__ << ")" \
					  << " " << __FILE__ << endl;

using namespace std;

const int64_t CYCLES_PER_SEC = 3000000000;
const double TIMELIMIT = 2.95;
struct Timer {
	int64_t start;
	Timer() { reset(); }
	void reset() { start = getCycle(); }
	void plus(double a) { start -= (a * CYCLES_PER_SEC); }
	inline double get() { return (double)(getCycle() - start) / CYCLES_PER_SEC; }
	inline int64_t getCycle() {
		uint32_t low, high;
		__asm__ volatile("rdtsc"
						 : "=a"(low), "=d"(high));
		return ((int64_t)low) | ((int64_t)high << 32);
	}
};
class XorShift {
   public:
	unsigned int x, y, z, w;
	double nL[65536];

	XorShift() {
		init();
	}

	void init() {
		x = 314159265;
		y = 358979323;
		z = 846264338;
		w = 327950288;
		double n = 1 / (double)(2 * 65536);
		for (int i = 0; i < 65536; i++) {
			nL[i] = log(((double)i / 65536) + n);
		}
	}

	inline unsigned int next() {
		unsigned int t = x ^ x << 11;
		x = y;
		y = z;
		z = w;
		return w = w ^ w >> 19 ^ t ^ t >> 8;
	}

	inline double nextLog() {
		return nL[next() & 0xFFFF];
	}

	inline int nextInt(int m) {
		return (int)(next() % m);
	}

	int nextInt(int min, int max) {
		return min + nextInt(max - min + 1);
	}

	inline double nextDouble() {
		return (double)next() / ((long long)1 << 32);
	}
};
XorShift rnd;
struct FastSet {
	vector<int> list;
	vector<int> pos;
	void init(int N) {
		pos.resize(N, -1);
		list.reserve(N);
	}
	void insert_all() {
		list.clear();
		list.resize(pos.size());
		for (int i = 0; i < list.size(); i++) {
			pos[i] = list[i] = i;
		}
	}
	void insert(int a) {
		if (pos[a] == -1) {
			pos[a] = list.size();
			list.push_back(a);
		}
	}
	void erase(int a) {
		if (pos[a] >= 0) {
			swap(list[pos[a]], list.back());
			pos[list[pos[a]]] = pos[a];
			pos[a] = -1;
			list.pop_back();
		}
	}
	void flip(int a) {
		if (pos[a] == -1) {
			pos[a] = list.size();
			list.push_back(a);
		} else {
			swap(list[pos[a]], list.back());
			pos[list[pos[a]]] = pos[a];
			pos[a] = -1;
			list.pop_back();
		}
	}
	inline int size() {
		return list.size();
	}
	inline int rand() {
		return list[rnd.nextInt(list.size())];
	}
	inline void erase_all() {
		for (int i : list) {
			pos[i] = -1;
		}
		list.clear();
	}
};
struct FastSet2 {
	vector<int> list;
	vector<int> pos;
	int sz;
	void init(int N) {
		sz = 0;
		pos.resize(N);
		list.resize(N);
		for (int i = 0; i < N; i++) {
			pos[i] = list[i] = i;
		}
	}
	void insert_all() {
		sz = list.size();
	}
	int getAny() {
		return list[sz++];
	}
	void insert(int a) {
		if (pos[a] >= sz) {
			pos[list[sz]] = pos[a];
			swap(list[pos[a]], list[sz]);
			pos[a] = sz;
			sz++;
		}
	}
	void erase(int a) {
		if (pos[a] < sz) {
			sz--;
			pos[list[sz]] = pos[a];
			swap(list[pos[a]], list[sz]);
			pos[a] = sz;
		}
	}
	inline int size() {
		return sz;
	}
	inline int rand() {
		return list[rnd.nextInt(sz)];
	}
	inline void erase_all() {
		sz = 0;
	}
};

const int DX[4] = {1, 0, -1, 0};
const int DY[4] = {0, 1, 0, -1};
const int REV[4] = {2, 3, 0, 1};
const string DIR = "DRUL";
const int N = 50;
int S;
int A[N * N];
int B[N * N];
int M = 0;
int ne[2500][4];
int nesz[2500];
struct Solver {
	Timer timer;
	bitset<1500> bs;
	vector<int> path;
	vector<int> best;
	int mxscore = 0;
	int score = 0;
	int rem;
	void dfs(int a) {
		//cerr << a << " " << score << endl;
		path.push_back(a);
		score += B[a];
		bs[A[a]] = 1;

		if (mxscore < score) {
			mxscore = score;
			best = path;
			//cerr << rem << " " << score << endl;
		}
		rem--;
		if (rem == 0) return;
		vector<pair<double, int> > X;
		for (int i = 0; i < nesz[a]; i++) {
			if (!bs[A[ne[a][i]]]) {
				double tmp = rnd.nextDouble() * 0.1;
				tmp += -max(abs(25 - (ne[a][i] / 50)), abs(25 - (ne[a][i] % 50)));
				tmp -= 0.01 * (abs(25 - (ne[a][i] / 50)) + abs(25 - (ne[a][i] % 50)));
				//tmp = (ne[a][i] / 50) + (ne[a][i] % 50);
				//tmp += B[ne[a][i]] * 0.01;
				X.emplace_back(tmp, ne[a][i]);
			}
		}
		sort(X.begin(), X.end());
		for (int i = 0; i < X.size(); i++) {
			if (!bs[A[X[i].second]]) {
				dfs(X[i].second);
				if (rem == 0) return;
			}
		}
		path.pop_back();
		score -= B[a];
		bs[A[a]] = 0;
	}
	int xx = 0;
	void dfs2(int a) {
		//cerr << a << " " << rem << endl;
		path.push_back(a);
		score += B[a];
		bs[A[a]] = 1;

		rem--;
		if (rem == 0) return;
		if (a != xx) {
			vector<pair<double, int> > X;
			for (int i = 0; i < nesz[a]; i++) {
				if (!bs[A[ne[a][i]]]) {
					double tmp = rnd.nextDouble() * 0.1;
					//tmp += -max(abs(25 - (ne[a][i] / 50)), abs(25 - (ne[a][i] % 50)));
					//tmp -= 0.01 * (abs(25 - (ne[a][i] / 50)) + abs(25 - (ne[a][i] % 50)));
					//tmp += -max(abs((xx / 50) - (ne[a][i] / 50)), abs((xx % 25) - (ne[a][i] % 50)));
					//tmp = (ne[a][i] / 50) + (ne[a][i] % 50);
					//tmp += B[ne[a][i]] * 0.01;
					X.emplace_back(tmp, ne[a][i]);
				} else if (ne[a][i] == xx) {
					if (mxscore < score) {
						mxscore = score;
						best = path;
					}
				}
			}
			sort(X.begin(), X.end());
			for (int i = 0; i < X.size(); i++) {
				if (!bs[A[X[i].second]]) {
					dfs2(X[i].second);
					if (rem == 0) return;
				}
			}
		}

		path.pop_back();
		score -= B[a];
		bs[A[a]] = 0;
	}

	void dfs3(int a) {
		//cerr << a << " " << score << endl;
		path.push_back(a);
		score += B[a];
		bs[A[a]] = 1;

		if (mxscore < score) {
			mxscore = score;
			best = path;
		}
		rem--;
		if (rem == 0) return;
		vector<pair<double, int> > X;
		for (int i = 0; i < nesz[a]; i++) {
			if (!bs[A[ne[a][i]]]) {
				double tmp = rnd.nextDouble() * 0.1;
				//tmp += -max(abs(25 - (ne[a][i] / 50)), abs(25 - (ne[a][i] % 50)));
				//tmp -= 0.01 * (abs(25 - (ne[a][i] / 50)) + abs(25 - (ne[a][i] % 50)));
				//tmp = (ne[a][i] / 50) + (ne[a][i] % 50);
				//tmp += B[ne[a][i]] * 0.01;
				X.emplace_back(tmp, ne[a][i]);
			}
		}
		sort(X.begin(), X.end());
		for (int i = 0; i < X.size(); i++) {
			if (!bs[A[X[i].second]]) {
				dfs3(X[i].second);
				if (rem == 0) return;
			}
		}
		path.pop_back();
		score -= B[a];
		bs[A[a]] = 0;
	}

	void dfsSolver() {
		best.clear();
		for (int i = 0; i < 300; i++) {
			score = 0;
			for (int j = 0; j < 1500; j++) {
				bs[j] = 0;
			}
			path.clear();
			rem = 8000;
			dfs(S);
		}
		cerr << "score = " << mxscore << endl;
	}

	void SA(double timelimit) {
		int sc = mxscore;
		vector<int> res = best;
		bitset<1500> bbs;
		for (int j = 0; j < 1500; j++) {
			bbs[j] = 0;
		}
		for (int i = 0; i < res.size(); i++) {
			bbs[A[res[i]]] = 1;
		}
		vector<int> tmp;
		double st = timer.get();
		double ti = st;
		double coef = 1.0 / (timelimit - st);
		int cnt = 0;
		double interval = (timelimit - st) / 10;
		//cerr << interval << endl;
		double dd = st;
		double T0 = 300;
		double T;
		const int kagen = 500;
		while (true) {
			ti = timer.get();
			if (ti > timelimit) break;
			double remti = (timelimit - ti) * coef;
			T = T0 * remti;
			// if (dd < ti) {
			// 	dd += interval;
			// 	cerr << remti << " " << T << " " << sc << endl;
			// }
			int W = rnd.nextInt((int)(1 + res.size() * 0.1 * (0.01 + remti)));
			int a = rnd.nextInt(res.size() - W);
			int b = a + W;
			cnt++;

			if (b + 3 > res.size()) {
				b = res.size();
				mxscore = 0;
				int nsc = sc;
				bs = bbs;
				rem = 10 * W;
				path.clear();
				best.clear();

				for (int i = a; i < b; i++) {
					nsc -= B[res[i]];
					mxscore += B[res[i]];
					bs[A[res[i]]] = 0;
				}
				score = 0;
				mxscore -= kagen;
				dfs3(res[a]);
				int diff = nsc + mxscore - sc;
				if (best.size() > 0 && diff > T * rnd.nextLog()) {
					sc = nsc + mxscore;
					tmp.clear();

					for (int i = a; i < b; i++) {
						bbs[A[res[i]]] = 0;
					}
					for (int i = 0; i < best.size(); i++) {
						bbs[A[best[i]]] = 1;
					}
					for (int i = 0; i < a; i++) {
						tmp.push_back(res[i]);
					}
					for (int i = 0; i < best.size(); i++) {
						tmp.push_back(best[i]);
					}
					swap(res, tmp);
				}
			} else {
				//cerr << cnt << " " << a <<" " << b << endl;
				bool rev = false;
				if (rnd.nextDouble() < 0.5) {
					rev = true;
				}
				mxscore = 0;
				int nsc = sc;
				bs = bbs;
				rem = 10 * W;
				path.clear();
				best.clear();

				if (rev) {
					for (int i = a + 1; i <= b; i++) {
						nsc -= B[res[i]];
						mxscore += B[res[i]];
						bs[A[res[i]]] = 0;
					}
					score = 0;
					mxscore -= kagen;
					xx = res[a];
					dfs2(res[b]);
				} else {
					for (int i = a; i < b; i++) {
						nsc -= B[res[i]];
						mxscore += B[res[i]];
						bs[A[res[i]]] = 0;
					}
					score = 0;
					mxscore -= kagen;
					xx = res[b];
					dfs2(res[a]);
				}
				int diff = nsc + mxscore - sc;
				if (best.size() > 0 && diff > T * rnd.nextLog()) {
					sc = nsc + mxscore;
					tmp.clear();

					if (rev) {
						for (int i = a + 1; i <= b; i++) {
							bbs[A[res[i]]] = 0;
						}
						for (int i = 0; i < best.size(); i++) {
							bbs[A[best[i]]] = 1;
						}
						for (int i = 0; i <= a; i++) {
							tmp.push_back(res[i]);
						}
						for (int i = best.size() - 1; i >= 0; i--) {
							tmp.push_back(best[i]);
						}
						for (int i = b + 1; i < res.size(); i++) {
							tmp.push_back(res[i]);
						}
					} else {
						for (int i = a; i < b; i++) {
							bbs[A[res[i]]] = 0;
						}
						for (int i = 0; i < best.size(); i++) {
							bbs[A[best[i]]] = 1;
						}
						for (int i = 0; i < a; i++) {
							tmp.push_back(res[i]);
						}
						for (int i = 0; i < best.size(); i++) {
							tmp.push_back(best[i]);
						}
						for (int i = b; i < res.size(); i++) {
							tmp.push_back(res[i]);
						}
					}

					swap(res, tmp);
				}
			}
		}
		cerr << "cnt = " << cnt << endl;
		best = res;
		cerr << "score = " << sc << endl;
	}

	void init() {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				int a = i * N + j;
				nesz[a] = 0;
				for (int d = 0; d < 4; d++) {
					int ii = i + DX[d];
					int jj = j + DY[d];
					if (ii >= 0 && ii < N && jj >= 0 && jj < N) {
						int b = ii * N + jj;
						if (A[a] != A[b]) {
							ne[a][nesz[a]] = b;
							nesz[a]++;
						}
					}
				}
			}
		}
	}

	int solve() {
		init();
		dfsSolver();
		SA(1.95);
		return 0;
	}

	void input() {
		cin.tie(0);
		ios::sync_with_stdio(false);
		int s1, s2;
		cin >> s1 >> s2;
		S = s1 * N + s2;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				cin >> A[i * N + j];
				M = max(M, A[i * N + j]);
			}
		}
		M++;
		cerr << "M = " << M << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				cin >> B[i * N + j];
			}
		}
	}

	void output() {
		string res;
		for (int i = 0; i < best.size() - 1; i++) {
			int k = best[i + 1] - best[i];
			if (k == 1) {
				res.push_back('R');
			} else if (k == -1) {
				res.push_back('L');
			} else if (k == N) {
				res.push_back('D');
			} else if (k == -N) {
				res.push_back('U');
			}
		}
		cout << res << endl;
	}
};

Solver solver;
int main(int argc, char *argv[]) {
	solver.input();
	solver.solve();
	solver.output();
	cerr << "timer = " << solver.timer.get() << endl;
	return 0;
}