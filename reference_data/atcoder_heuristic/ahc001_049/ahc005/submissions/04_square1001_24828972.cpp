#include <cmath>
#include <ctime>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
using namespace std;
uint64_t seed = 123456789;
uint64_t xorshift64() {
	seed ^= (seed << 13);
	seed ^= (seed >> 7);
	seed ^= (seed << 17);
	return seed;
}
int rand_int(int l, int r) {
	return l + int(xorshift64() % (r - l));
}
double randouble() {
	return double(xorshift64() + 0.5) / double(1LL << 32) / double(1LL << 32);
}
int main() {
	// Step #1. Initialization
	const vector<int> dx = { 0, 1, 0, -1 };
	const vector<int> dy = { 1, 0, -1, 0 };
	const string ds = "RDLU";
	const int inf = 1012345678;
	int N, sx, sy;
	cin >> N >> sx >> sy;
	vector<string> S(N);
	for (int i = 0; i < N; ++i) {
		cin >> S[i];
	}

	// Step #2. Find All Roads
	vector<vector<int> > roadx(N, vector<int>(N, -1));
	vector<vector<int> > roady(N, vector<int>(N, -1));
	int nroadx = 0, nroady = 0;
	for (int i = 0; i < N; i += 2) {
		int pre = 0;
		for (int j = 0; j <= N; ++j) {
			if (j == N || S[i][j] == '#') {
				if (j - pre >= 2) {
					for (int k = pre; k < j; ++k) {
						roadx[i][k] = nroadx;
					}
					++nroadx;
				}
				pre = j + 1;
			}
		}
	}
	for (int i = 0; i < N; i += 2) {
		int pre = 0;
		for (int j = 0; j <= N; ++j) {
			if (j == N || S[j][i] == '#') {
				if (j - pre >= 2) {
					for (int k = pre; k < j; ++k) {
						roady[k][i] = nroady;
					}
					++nroady;
				}
				pre = j + 1;
			}
		}
	}

	// Step #3. Cut Unnecessary Parts
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			if (i == sx && j == sy) {
				continue;
			}
			if ((j == 0 || roadx[i][j - 1] == -1) && roadx[i][j] != -1 && roady[i][j] == -1) {
				roadx[i][j] = -1;
			}
		}
		for (int j = N - 1; j >= 0; --j) {
			if (i == sx && j == sy) {
				continue;
			}
			if ((j == N - 1 || roadx[i][j + 1] == -1) && roadx[i][j] != -1 && roady[i][j] == -1) {
				roadx[i][j] = -1;
			}
		}
	}
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			if (j == sx && i == sy) {
				continue;
			}
			if ((j == 0 || roady[j - 1][i] == -1) && roady[j][i] != -1 && roadx[j][i] == -1) {
				roady[j][i] = -1;
			}
		}
		for (int j = N - 1; j >= 0; --j) {
			if (j == sx && i == sy) {
				continue;
			}
			if ((j == N - 1 || roady[j + 1][i] == -1) && roady[j][i] != -1 && roadx[j][i] == -1) {
				roady[j][i] = -1;
			}
		}
	}
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			if (roadx[i][j] == -1 && roady[i][j] == -1) {
				S[i][j] = '#';
			}
		}
	}

	// Step #4. Find Shortest Paths
	vector<vector<int> > dist(N * N, vector<int>(N * N, inf));
	for (int i = 0; i < N * N; ++i) {
		dist[i][i] = 0;
		vector<vector<int> > que(1);
		que[0].push_back(i);
		for (int j = 0; j < que.size(); ++j) {
			for (int k : que[j]) {
				for (int l = 0; l < 4; ++l) {
					int tx = k / N + dx[l], ty = k % N + dy[l];
					if (0 <= tx && tx < N && 0 <= ty && ty < N && S[tx][ty] != '#') {
						int nd = dist[i][k] + int(S[tx][ty] - '0');
						if (dist[i][tx * N + ty] > nd) {
							while (que.size() <= nd) {
								que.push_back(vector<int>());
							}
							dist[i][tx * N + ty] = nd;
							que[nd].push_back(tx * N + ty);
						}
					}
				}
			}
		}
	}
	vector<vector<int> > dual_dist(N * N, vector<int>(N * N));
	for (int i = 0; i < N * N; ++i) {
		for (int j = 0; j < N * N; ++j) {
			dual_dist[i][j] = dist[i][j] + dist[j][i];
		}
	}

	// Step #5. Minimize Tour Cost
	int nroads = nroadx + nroady;
	vector<vector<int> > cand(nroads + 1);
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			if (roadx[i][j] != -1 && roady[i][j] != -1) {
				cand[roadx[i][j]].push_back(i * N + j);
				cand[roady[i][j] + nroadx].push_back(i * N + j);
			}
		}
	}
	cand[nroads] = { sx * N + sy };
	vector<int> sel(nroads + 1);
	for (int i = 0; i < nroads; ++i) {
		sel[i] = rand_int(0, cand[i].size());
	}
	sel[N] = 0;
	vector<int> perm(nroads + 2);
	for (int i = 0; i <= nroads + 1; ++i) {
		perm[i] = (nroads + i) % (nroads + 1);
	}
	int current_cost = 0;
	for (int i = 0; i <= nroads; ++i) {
		current_cost += dual_dist[cand[perm[i]][sel[perm[i]]]][cand[perm[i + 1]][sel[perm[i + 1]]]];
	}
	vector<int> best_sel = sel;
	vector<int> best_perm = perm;
	int best_cost = current_cost;
	const int repeats = 25000000;
	int iteration = 0;
	while (iteration < repeats) {
		++iteration;
		int id = -1, preval = -1, nxtval = -1, va = -1, vb = -1;
		int mutation_type = rand_int(0, 2);
		if (mutation_type == 0) {
			id = rand_int(1, nroads + 1);
			preval = sel[perm[id]];
			nxtval = rand_int(0, cand[perm[id]].size());
		}
		else {
			do {
				va = rand_int(1, nroads + 1);
				vb = rand_int(1, nroads + 1);
			} while (va >= vb);
		}
		int total_cost = current_cost;
		if (mutation_type == 0) {
			int prep = cand[perm[id - 1]][sel[perm[id - 1]]];
			int nxtp = cand[perm[id + 1]][sel[perm[id + 1]]];
			total_cost -= dual_dist[prep][cand[perm[id]][preval]];
			total_cost -= dual_dist[nxtp][cand[perm[id]][preval]];
			total_cost += dual_dist[prep][cand[perm[id]][nxtval]];
			total_cost += dual_dist[nxtp][cand[perm[id]][nxtval]];
		}
		else {
			int apl = cand[perm[va - 1]][sel[perm[va - 1]]];
			int apr = cand[perm[va]][sel[perm[va]]];
			int bpl = cand[perm[vb]][sel[perm[vb]]];
			int bpr = cand[perm[vb + 1]][sel[perm[vb + 1]]];
			total_cost -= dual_dist[apl][apr];
			total_cost -= dual_dist[bpl][bpr];
			total_cost += dual_dist[apl][bpl];
			total_cost += dual_dist[apr][bpr];
		}
		double temps = (1.1 - double(iteration) / repeats) * 40.0;
		if (current_cost >= total_cost || randouble() < exp((current_cost - total_cost) / temps)) {
			if (mutation_type == 0) {
				sel[perm[id]] = nxtval;
			}
			else {
				reverse(perm.begin() + va, perm.begin() + vb + 1);
			}
			if (best_cost > total_cost) {
				cerr << "#" << iteration << ": " << total_cost / 2 << endl;
				best_sel = sel;
				best_perm = perm;
				best_cost = total_cost;
			}
			current_cost = total_cost;
		}
	}

	// Step #6. Get the Shortest Route
	function<string(int, int, int, int)> shortest_route = [&](int ax, int ay, int bx, int by) {
		string res_route;
		while (!(ax == bx && ay == by)) {
			for (int i = 0; i < 4; ++i) {
				int tx = bx + dx[i], ty = by + dy[i];
				if (0 <= tx && tx < N && 0 <= ty && ty < N && S[tx][ty] != '#') {
					int nxt_dist = dist[ax * N + ay][tx * N + ty];
					if (nxt_dist == dist[ax * N + ay][bx * N + by] - int(S[bx][by] - '0')) {
						res_route += ds[(i + 2) % 4];
						bx = tx;
						by = ty;
						break;
					}
				}
			}
		}
		reverse(res_route.begin(), res_route.end());
		return res_route;
	};

	// Step #7. Determine the Route
	string ans;
	for (int i = 0; i <= nroads; ++i) {
		int px = cand[best_perm[i]][best_sel[best_perm[i]]];
		int py = cand[best_perm[i + 1]][best_sel[best_perm[i + 1]]];
		ans += shortest_route(px / N, px % N, py / N, py % N);
	}
	cout << ans << endl;
	cerr << best_cost / 2 << endl;
	return 0;
}