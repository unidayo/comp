#include <bits/stdc++.h>
using namespace std;

namespace myrandom {
	uint64_t seed = 1234567891234567891;
	uint64_t xorshift64() {
		seed ^= seed << 13;
		seed ^= seed >> 7;
		seed ^= seed << 17;
		return seed;
	}
	int next_int(int l, int r) {
		return l + int(xorshift64() % (r - l));
	}
	double next_double(double l, double r) {
		return l + double(xorshift64()) / double(uint64_t(-1)) * (r - l);
	}
}

int main() {
	// step #1. input
	const int INF = 1012345678;
	const int K = 5;
	int N, M, sx, sy;
	cin >> N >> M >> sx >> sy;
	vector<string> AS(N);
	for (int i = 0; i < N; i++) {
		cin >> AS[i];
	}
	vector<string> TS(M);
	for (int i = 0; i < M; i++) {
		cin >> TS[i];
		assert(TS[i].size() == K);
	}

	// step #2. preparation
	auto cost = [&](int va, int vb) -> int {
		return abs(va / N - vb / N) + abs(va % N - vb % N) + 1;
	};
	int start = sx * N + sy;
	vector<vector<int> > G(26);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			G[AS[i][j] - 'A'].push_back(i * N + j);
		}
	}
	vector<vector<int> > T(M, vector<int>(K));
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < K; j++) {
			T[i][j] = TS[i][j] - 'A';
		}
	}
	vector<vector<int> > overlap(M + 1, vector<int>(M + 1));
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			for (int k = 1; k <= K; k++) {
				if (vector<int>(T[i].end() - k, T[i].end()) == vector<int>(T[j].begin(), T[j].begin() + k)) {
					overlap[i][j] = k;
				}
			}
		}
	}

	// step #3. dynamic programming
	vector<vector<vector<int> > > optval(M);
	for (int i = 0; i < M; i++) {
		optval[i] = vector<vector<int> >(G[T[i][K - 1]].size(), vector<int>(N * N * K));
		for (int j = 0; j < int(G[T[i][K - 1]].size()); j++) {
			vector<int> dp(G[T[i][K - 1]].size(), INF);
			dp[j] = 0;
			for (int k = K - 1; k >= 0; k--) {
				for (int l = 0; l < N * N; l++) {
					int opt = INF;
					for (int m = 0; m < int(G[T[i][k]].size()); m++) {
						opt = min(opt, dp[m] + cost(l, G[T[i][k]][m]));
					}
					optval[i][j][k * N * N + l] = opt;
				}
				if (k != 0) {
					vector<int> ndp(G[T[i][k - 1]].size(), INF);
					for (int l = 0; l < int(G[T[i][k - 1]].size()); l++) {
						for (int m = 0; m < int(G[T[i][k]].size()); m++) {
							ndp[l] = min(ndp[l], dp[m] + cost(G[T[i][k - 1]][l], G[T[i][k]][m]));
						}
					}
					dp = ndp;
				}
			}
		}
	}

	// step #4. function to compute optimal cost
	auto eval_adj = [&](int p1, int e1, int p2, int e2) -> int {
		return (p2 != M ? optval[p2][e2][overlap[p1][p2] * N * N + (p1 != M ? G[T[p1][K - 1]][e1] : start)] : 0);
	};
	auto eval = [&](const vector<int>& p, const vector<int>& e) -> int {
		int res = 0;
		for (int i = 1; i <= M; i++) {
			res += eval_adj(p[i - 1], e[p[i - 1]], p[i], e[p[i]]);
		}
		return res;
	};

	// step #5. function to compute optimal sequence of operations
	auto optimal = [&](const vector<int>& p) -> vector<int> {
		vector<int> seq;
		for (int i = 1; i <= M; i++) {
			for (int j = (i != 1 ? overlap[p[i - 1]][p[i]] : 0); j < K; j++) {
				seq.push_back(T[p[i]][j]);
			}
		}
		vector<vector<int> > dp(seq.size());
		vector<vector<int> > pre(seq.size());
		for (int i = 0; i < int(seq.size()); i++) {
			dp[i].resize(G[seq[i]].size(), INF);
			pre[i].resize(G[seq[i]].size(), -1);
			if (i == 0) {
				for (int j = 0; j < int(G[seq[i]].size()); j++) {
					dp[i][j] = cost(start, G[seq[i]][j]);
				}
			}
			else {
				for (int j = 0; j < int(G[seq[i]].size()); j++) {
					for (int k = 0; k < int(G[seq[i - 1]].size()); k++) {
						int ncost = dp[i - 1][k] + cost(G[seq[i - 1]][k], G[seq[i]][j]);
						if (dp[i][j] > ncost) {
							dp[i][j] = ncost;
							pre[i][j] = k;
						}
					}
				}
			}
		}
		vector<int> res;
		int pos = min_element(dp.back().begin(), dp.back().end()) - dp.back().begin();
		for (int i = int(seq.size()) - 1; i >= 1; i--) {
			res.push_back(G[seq[i]][pos]);
			pos = pre[i][pos];
		}
		res.push_back(G[seq[0]][pos]);
		reverse(res.begin(), res.end());
		return res;
	};

	// step #6. hill climbing
	vector<int> p(M + 2), e(M + 1);
	p[0] = M;
	p[M + 1] = M;
	e[M] = -1;
	for (int i = 0; i < M; i++) {
		p[i + 1] = i;
		e[i] = myrandom::next_int(0, G[T[p[i + 1]][K - 1]].size());
	}
	int curscore = eval(p, e);
	int bestscore = curscore;
	vector<int> bestp = p;
	uint64_t loops = 0;
	const double TLE = 1.97;
	double timeval;
	while (true) {
		loops += 1;
		if (loops % 1000 == 0) {
			timeval = double(clock()) / (TLE * CLOCKS_PER_SEC);
			if (timeval > 1.0) {
				break;
			}
		}
		int tp = myrandom::next_int(0, 2);
		int a, b, c;
		int nxtscore = curscore;
		if (tp == 0) {
			a = myrandom::next_int(1, M + 1);
			b = myrandom::next_int(0, G[T[p[a]][K - 1]].size());
			c = -1;
			nxtscore -= eval_adj(p[a - 1], e[p[a - 1]], p[a], e[p[a]]);
			nxtscore += eval_adj(p[a - 1], e[p[a - 1]], p[a], b);
			nxtscore -= eval_adj(p[a], e[p[a]], p[a + 1], e[p[a + 1]]);
			nxtscore += eval_adj(p[a], b, p[a + 1], e[p[a + 1]]);
		}
		if (tp == 1) {
			do {
				a = myrandom::next_int(1, M + 2);
				b = myrandom::next_int(1, M + 2);
				c = myrandom::next_int(1, M + 2);
			} while (a == b || b == c || c == a);
			if (a > b) swap(a, b);
			if (a > c) swap(a, c);
			if (b > c) swap(b, c);
			nxtscore -= eval_adj(p[a - 1], e[p[a - 1]], p[a], e[p[a]]);
			nxtscore += eval_adj(p[c - 1], e[p[c - 1]], p[a], e[p[a]]);
			nxtscore -= eval_adj(p[b - 1], e[p[b - 1]], p[b], e[p[b]]);
			nxtscore += eval_adj(p[a - 1], e[p[a - 1]], p[b], e[p[b]]);
			nxtscore -= eval_adj(p[c - 1], e[p[c - 1]], p[c], e[p[c]]);
			nxtscore += eval_adj(p[b - 1], e[p[b - 1]], p[c], e[p[c]]);
		}
		double tmp = (timeval < 0.05 ? 1.3 : 1.0 - timeval);
		if (curscore >= nxtscore || myrandom::next_double(0.0, 1.0) < exp((curscore - nxtscore) / tmp)) {
			if (tp == 0) {
				e[p[a]] = b;
			}
			if (tp == 1) {
				vector<int> np = p;
				for (int i = b; i < c; i++) {
					np[i - (b - a)] = p[i];
				}
				for (int i = a; i < b; i++) {
					np[i + (c - b)] = p[i];
				}
				p = np;
			}
			if (bestscore > nxtscore) {
				// cerr << "loop #" << loops << ": score " << curscore << " -> " << nxtscore << endl;
				bestscore = nxtscore;
				bestp = p;
			}
			curscore = nxtscore;
		}
	}
	cerr << "loops: " << loops << endl;

	// step #5. output
	vector<int> ans = optimal(bestp);
	for (int i : ans) {
		cout << i / N << ' ' << i % N << endl;
	}

	return 0;
}