#include <map>
#include <cmath>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

const int INF = 1012345678;

class cell {
public:
	int x, y;
	cell() : x(-1), y(-1) {}
	cell(int x_, int y_) : x(x_), y(y_) {}
	bool operator==(const cell& c) const {
		return x == c.x && y == c.y;
	}
	bool operator!=(const cell& c) const {
		return x != c.x || y != c.y;
	}
	bool operator<(const cell& c) const {
		return x != c.x ? x < c.x : y < c.y;
	}
	bool operator>(const cell& c) const {
		return x != c.x ? x > c.x : y > c.y;
	}
	int dist(const cell& c) const {
		return abs(x - c.x) + abs(y - c.y);
	}
	string to_string() const {
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};

struct flow_edge {
	int t, w, cap, rev;
};

int min_cost_flow(int V, int S, int T, int F, vector<vector<flow_edge> >& G) {
	struct state {
		int v, d;
		bool operator<(const state& s) const {
			return d > s.d;
		}
	};
	int ans = 0;
	vector<int> potential(V, 0);
	for (int rep = 0; rep < F; rep++) {
		vector<int> d(V, INF);
		d[S] = 0;
		priority_queue<state> que;
		que.push(state{S, 0});
		vector<flow_edge> pre(V);
		while (!que.empty()) {
			state z = que.top();
			que.pop();
			if (d[z.v] == z.d) {
				for (flow_edge e : G[z.v]) {
					int nd = z.d + e.w - (potential[e.t] - potential[z.v]);
					if (e.cap > 0 && d[e.t] > nd) {
						d[e.t] = nd;
						pre[e.t] = G[e.t][e.rev];
						que.push(state{e.t, nd});
					}
				}
			}
		}
		if (d[T] == INF) return INF;
		ans += d[T] + potential[T];
		int pos = T;
		while (pos != S) {
			flow_edge& e = G[pre[pos].t][pre[pos].rev];
			e.cap -= 1;
			G[e.t][e.rev].cap += 1;
			pos = pre[pos].t;
		}
		for (int i = 0; i < V; i++) {
			potential[i] += d[i];
		}
	}
	return ans;
}

struct flow {
	cell s, t; int v;
};

pair<int, vector<flow> > optimal_transport(int N, const vector<vector<int> >& H) {
	// step #1. make graph
	vector<vector<flow_edge> > G(N * N + 2);
	auto add_edge = [&](int a, int b, int c, int d) -> void {
		G[a].push_back(flow_edge{b, d, c, int(G[b].size())});
		G[b].push_back(flow_edge{a, -d, 0, int(G[a].size()) - 1});
	};
	const vector<int> dx = { 1, 0, -1, 0 };
	const vector<int> dy = { 0, 1, 0, -1 };
	for (int i = 0; i < N; i++) {
		for (int j = 1; j < N; j++) {
			if (i % 2 == 0 || ((i == 0 || i == N - 1) && j % 5 == 0)) {
				add_edge(i * N + j - 1, i * N + j, INF, 1);
			}
			if (i % 2 == 1 || ((i == 0 || i == N - 1) && j % 5 == 0)) {
				add_edge(i * N + j, i * N + j - 1, INF, 1);
			}
		}
	}
	for (int i = 1; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (j % 5 == 0 || ((j == 0 || j == N - 1) && i % 2 == 0)) {
				add_edge(i * N + j, i * N + j - N, INF, 1);
			}
			if (j % 5 == 4 || ((j == 0 || j == N - 1) && i % 2 == 0)) {
				add_edge(i * N + j - N, i * N + j, INF, 1);
			}
		}
	}
	int Z = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			int x = i * N + j;
			if (H[i][j] > 0) {
				add_edge(N * N + 0, x, H[i][j], 0);
				Z += H[i][j];
			} else {
				add_edge(x, N * N + 1, -H[i][j], 0);
			}
		}
	}

	// step #2. calculate flow
	int cost = min_cost_flow(N * N + 2, N * N + 0, N * N + 1, Z, G);

	// step #3. restore answer
	vector<flow> res;
	for (int i = 0; i < N * N + 2; i++) {
		for (flow_edge e : G[i]) {
			if (e.w == -1 && e.cap > 0) {
				res.push_back(flow{cell(e.t / N, e.t % N), cell(i / N, i % N), e.cap});
			}
		}
	}
	return make_pair(cost, res);
}

char direction(cell c1, cell c2) {
	if (c2 == cell(c1.x + 1, c1.y)) return 'D';
	if (c2 == cell(c1.x - 1, c1.y)) return 'U';
	if (c2 == cell(c1.x, c1.y + 1)) return 'R';
	if (c2 == cell(c1.x, c1.y - 1)) return 'L';
	return '.';
}

int main() {
	// step #1. input
	int N;
	cin >> N;
	vector<vector<int> > H(N, vector<int>(N));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			cin >> H[i][j];
		}
	}

	// step #2. calculate optimal transport
	pair<int, vector<flow> > opt_pack = optimal_transport(N, H);

	// step #3. prepare track
	vector<cell> track;
	for (int i = 0; i < N - 1; i++) {
		if (i % 2 == 0) {
			for (int j = 0; j < N; j++) {
				track.push_back(cell(i, j));
			}
			if (i != 0) {
				track.push_back(cell(i - 1, N - 1));
				track.push_back(cell(i, N - 1));
			}
		} else {
			for (int j = N - 1; j >= 0; j--) {
				track.push_back(cell(i, j));
			}
		}
	}
	for (int i = N - 1; i >= 0; i--) {
		if (i % 5 == 4 && i != N - 1) {
			for (int j = 0; j < N; j++) {
				track.push_back(cell(j, i));
			}
			track.push_back(cell(N - 1, i + 1));
			track.push_back(cell(N - 1, i));
		} else if (i % 5 == 0) {
			for (int j = N - 1; j >= 0; j--) {
				track.push_back(cell(j, i));
			}
		} else {
			track.push_back(cell(N - 1, i));
		}
	}

	// step #4. output solution
	map<pair<cell, cell>, int> d;
	for (flow f : opt_pack.second) {
		d[make_pair(f.s, f.t)] = f.v;
	}
	int carry = 0;
	for (int i = 0; i < int(track.size()); i++) {
		int x = (i != int(track.size()) - 1 ? d[make_pair(track[i], track[i + 1])] : 0) - carry;
		if (x > 0) {
			cout << "+" << x << endl;
		}
		if (x < 0) {
			cout << "-" << -x << endl;
		}
		carry += x;
		if (i != int(track.size()) - 1) {
			cout << direction(track[i], track[i + 1]) << endl;
		}
	}

	return 0;
}