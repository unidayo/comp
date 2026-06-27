#include <cmath>
#include <bitset>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
using namespace std;

const long long INF = 3LL << 59;

class point {
public:
	int x, y;
	point() : x(0), y(0) {}
	point(int x_, int y_) : x(x_), y(y_) {}
};

class reducer {
public:
	int r;
	long long d;
	long long s;
	long long z;
	bitset<5001> b;
	reducer() : r(0), d(0), s(0), z(0), b() {}
	reducer(int r_, long long d_, long long s_, long long z_) : r(r_), d(d_), s(s_), z(z_), b() {}
};

class segment_tree {
private:
	int size;
	vector<pair<long long, int> > val;
public:
	segment_tree(int n) : size(1 << (n >= 2 ? 32 - __builtin_clz(n - 1) : 0)) {
		val.resize(2 * size, make_pair(-INF, -1));
		for (int i = 0; i < n; i++) {
			val[size + i] = make_pair(0LL, i);
		}
		for (int i = size - 1; i >= 1; i--) {
			val[i] = max(val[i * 2 + 0], val[i * 2 + 1]);
		}
	}
	void update(int pos, long long x) {
		pos += size;
		val[pos].first = x;
		while (pos > 1) {
			pos >>= 1;
			val[pos] = max(val[pos * 2 + 0], val[pos * 2 + 1]);
		}
	}
	pair<long long, int> global_max() const {
		return val[1];
	}
};

int main() {
	// step #1. input
	int N;
	cin >> N;
	vector<long long> IX(N + 1), IY(N + 1);
	for (int i = 1; i <= N; i++) {
		cin >> IX[i] >> IY[i];
	}

	// step #2. compression
	vector<long long> cx = IX, cy = IY;
	sort(cx.begin(), cx.end());
	sort(cy.begin(), cy.end());
	vector<point> P(N + 1);
	for (int i = 0; i <= N; i++) {
		P[i].x = lower_bound(cx.begin(), cx.end(), IX[i]) - cx.begin();
		P[i].y = lower_bound(cy.begin(), cy.end(), IY[i]) - cy.begin();
	}
	auto height = [&](const point& p) -> long long {
		return cx[p.x] + cy[p.y];
	};

	// step #3. initial answer
	vector<int> par(N + 1, 0);
	for (int i = 1; i <= N; i++) {
		long long maxh = 0;
		for (int j = 1; j <= N; j++) {
			long long h = height(P[j]);
			if (P[j].x < P[i].x && P[j].y < P[i].y && maxh < h) {
				maxh = h;
				par[i] = j;
			}
		}
	}

	// step #4. greedy algorithm: preparation
	segment_tree seg((N + 1) * (N + 1));
	vector<vector<reducer> > tbl(N + 1, vector<reducer>(N + 1, reducer(0, INF, 0, 0)));
	auto update = [&](const point& p) -> void {
		tbl[p.x][p.y].z = tbl[p.x][p.y].s - tbl[p.x][p.y].d;
		seg.update(p.x * (N + 1) + p.y, tbl[p.x][p.y].z);
	};
	auto add_point = [&](int idx) -> void {
		int base_h = height(P[par[idx]]);
		for (int i = P[idx].x; i >= 0 && height(point(i, P[idx].y)) > base_h; i--) {
			for (int j = P[idx].y; j >= 0 && height(point(i, j)) > base_h; j--) {
				if (i != P[idx].x || j != P[idx].y) {
					tbl[i][j].s += height(point(i, j)) - base_h;
					tbl[i][j].b[idx] = 1;
					update(point(i, j));
				}
			}
		}
	};
	auto remove_point = [&](int idx) -> void {
		int base_h = height(P[par[idx]]);
		for (int i = P[idx].x; i >= 0 && height(point(i, P[idx].y)) > base_h; i--) {
			for (int j = P[idx].y; j >= 0 && height(point(i, j)) > base_h; j--) {
				if (i != P[idx].x || j != P[idx].y) {
					tbl[i][j].s -= height(point(i, j)) - base_h;
					tbl[i][j].b[idx] = 0;
					update(point(i, j));
				}
			}
		}
	};
	for (int i = 1; i <= N; i++) {
		add_point(i);
	}
	auto repaint = [&](auto& self, const point& p, int r) -> void {
		tbl[p.x][p.y].d = height(p) - height(P[r]);
		tbl[p.x][p.y].r = r;
		update(p);
		if (p.y != N && tbl[p.x][p.y + 1].d > height(point(p.x, p.y + 1)) - height(P[r])) {
			self(self, point(p.x, p.y + 1), r);
		}
		if (p.x != N && tbl[p.x + 1][p.y].d > height(point(p.x + 1, p.y)) - height(P[r])) {
			self(self, point(p.x + 1, p.y), r);
		}
	};
	vector<int> repaint_ord(N + 1);
	for (int i = 0; i <= N; i++) {
		repaint_ord[i] = i;
	}
	sort(repaint_ord.begin(), repaint_ord.end(), [&](int va, int vb) {
		return height(P[va]) > height(P[vb]);
	});
	for (int i : repaint_ord) {
		repaint(repaint, P[i], i);
	}

	// step #5. greedy algorithm: main
	const int MAX_M = N * 5;
	for (int i = N + 1; i <= MAX_M; i++) {
		pair<long long, int> best_reduce = seg.global_max();
		point best(best_reduce.second / (N + 1), best_reduce.second % (N + 1));
		if (best_reduce.first == 0) {
			break;
		}
		P.push_back(best);
		par.push_back(tbl[best.x][best.y].r);
		for (int j = 1; j < i; j++) {
			if (tbl[best.x][best.y].b[j] == 1) {
				remove_point(j);
				par[j] = i;
				add_point(j);
			}
		}
		add_point(i);
		repaint(repaint, best, i);
		cerr << i << ": " << best_reduce.first << endl;
	}

	// step #6. construct answer & output
	int M = int(P.size()) - 1;
	cout << M << endl;
	vector<int> output_ord(M + 1);
	for (int i = 0; i <= M; i++) {
		output_ord[i] = i;
	}
	sort(output_ord.begin(), output_ord.end(), [&](int va, int vb) {
		return height(P[va]) < height(P[vb]);
	});
	for (int i = 1; i <= M; i++) {
		int id = output_ord[i];
		cout << cx[P[par[id]].x] << ' ' << cy[P[par[id]].y] << ' ' << cx[P[id].x] << ' ' << cy[P[id].y] << endl;
	}

	return 0;
}