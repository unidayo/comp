#include <cmath>
#include <ctime>
#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
using namespace std;

uint64_t seed = 123456789;
uint64_t xorshift64() {
	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 7;
	return seed;
}
int rand_int(int l, int r) {
	return l + int(xorshift64() % (r - l));
}
double randouble() {
	return double(xorshift64()) / double(uint64_t(-1));
}

const vector<int> dx = { 1, 0, -1, 0 };
const vector<int> dy = { 0, 1, 0, -1 };
const string ds = "DRUL";

class state {
public:
	vector<int> seq;
	vector<double> dp;
	double penalty, reachturn;
	state() : seq(vector<int>()), dp(vector<double>()), penalty(0.0), reachturn(0.0) {};
	state(const vector<int>& seq_, const vector<double>& dp_, double penalty_, double reachturn_) : seq(seq_), dp(dp_), penalty(penalty_), reachturn(reachturn_) {};
	bool operator<(const state& s) const {
		return penalty - reachturn < s.penalty - s.reachturn;
	}
};

vector<int> beam_search(int N, int L, double PROB, int SP, int GP, const vector<vector<int> >& nxt) {
	// step #1. calculate distance
	vector<int> dist(N * N, -1);
	dist[GP] = 0;
	queue<int> que;
	que.push(GP);
	while (!que.empty()) {
		int u = que.front(); que.pop();
		for (int i = 0; i < 4; i++) {
			if (dist[nxt[i][u]] == -1) {
				dist[nxt[i][u]] = dist[u] + 1;
				que.push(nxt[i][u]);
			}
		}
	}

	// step #2. beam search
	const int BEAM_SIZE = 200;
	vector<double> initial_dp(N * N, 0.0);
	initial_dp[SP] = 1.0;
	vector<state> cands(1, state(vector<int>(), initial_dp, dist[SP], 0.0));
	for (int i = 0; i < L; i++) {
		vector<state> ncands;
		for (state s : cands) {
			for (int j = 0; j < 4; j++) {
				vector<double> ndp(N * N, 0.0);
				for (int k = 0; k < N * N; k++) {
					ndp[k] += s.dp[k] * PROB;
					ndp[nxt[j][k]] += s.dp[k] * (1 - PROB);
				}
				double nxt_penalty = 0.0, reach_prob = 1.0;
				for (int k = 0; k < N * N; k++) {
					nxt_penalty += ndp[k] * dist[k];
					reach_prob -= ndp[k];
				}
				double nxt_reachturn = s.reachturn + reach_prob / pow(1.0 - PROB, 3.0);
				ndp[GP] = 0.0;
				ncands.push_back(state(s.seq, ndp, nxt_penalty, nxt_reachturn));
				ncands.back().seq.push_back(j);
			}
		}
		sort(ncands.begin(), ncands.end());
		if (int(ncands.size()) > BEAM_SIZE) {
			ncands.resize(BEAM_SIZE);
		}
		cands = ncands;
		// cerr << "Step #" << i + 1 << ": Score = " << cands[0].reachturn - cands[0].penalty << endl;
	}
	cerr << "Beam Search Evaluation = " << cands[0].reachturn - cands[0].penalty << endl;
	return cands[0].seq;
}

int main() {
	// step #1. input & initial setup
	const int N = 20;
	const int L = 200;
	int bsx, bsy, bgx, bgy; double PROB;
	vector<string> HS(N), VS(N - 1);
	cin >> bsx >> bsy >> bgx >> bgy >> PROB;
	for (int i = 0; i < N; i++) {
		cin >> HS[i];
	}
	for (int i = 0; i < N - 1; i++) {
		cin >> VS[i];
	}
	const int SP = bsx * N + bsy;
	const int GP = bgx * N + bgy;

	// step #2. calculate "next place"
	vector<vector<int> > realnxt(4, vector<int>(N * N));
	for (int i = 0; i < N * N; i++) {
		int x = i / N, y = i % N;
		realnxt[0][i] = (x == N - 1 || VS[x][y] == '1' ? i : i + N); // (x, y) --> (x+1, y)
		realnxt[1][i] = (y == N - 1 || HS[x][y] == '1' ? i : i + 1); // (x, y) --> (x, y+1)
		realnxt[2][i] = (x == 0 || VS[x - 1][y] == '1' ? i : i - N); // (x, y) --> (x-1, y)
		realnxt[3][i] = (y == 0 || HS[x][y - 1] == '1' ? i : i - 1); // (x, y) --> (x, y-1)
	}
	vector<vector<int> > nxt = realnxt;
	nxt[0][GP] = GP;
	nxt[1][GP] = GP;
	nxt[2][GP] = GP;
	nxt[3][GP] = GP;

	// step #3. hill climbing
	vector<int> seq = beam_search(N, L, PROB, SP, GP, realnxt);
	vector<vector<double> > dp(L + 1, vector<double>(N * N, 0.0));
	vector<vector<double> > dp2(L + 1, vector<double>(N * N, 0.0));
	dp[0][SP] = 1.0;
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < N * N; j++) {
			dp[i + 1][j] += dp[i][j] * PROB;
			dp[i + 1][nxt[seq[i]][j]] += dp[i][j] * (1.0 - PROB);
		}
	}
	double score = (401.0 - L) * dp[L][GP];
	for (int i = 0; i < L; i++) {
		score += dp[i][GP];
	}
	cerr << "Beam Search Score = " << score << endl;
	double best_score = score;
	vector<int> best_seq = seq;
	int iteration = 0;
	while (clock() < 1.95 * CLOCKS_PER_SEC) {
		iteration++;
		vector<int> preseq = seq;
		int tp = rand_int(0, 3);
		int idx = rand_int(0, L);
		if (tp == 0) {
			seq[idx] = (seq[idx] + rand_int(1, 4)) % 4;
		}
		if (tp == 1) {
			for (int i = idx; i < L - 1; i++) {
				seq[i] = seq[i + 1];
			}
			seq[L - 1] = rand_int(0, 4);
		}
		if (tp == 2) {
			for (int i = L - 2; i >= idx; i--) {
				seq[i + 1] = seq[i];
			}
		}
		dp2[idx] = dp[idx];
		for (int i = idx; i < L; i++) {
			for (int j = 0; j < N * N; j++) {
				dp2[i + 1][j] = 0.0;
			}
			for (int j = 0; j < N * N; j++) {
				dp2[i + 1][j] += dp2[i][j] * PROB;
				dp2[i + 1][nxt[seq[i]][j]] += dp2[i][j] * (1.0 - PROB);
			}
		}
		double next_score = (401.0 - L) * dp2[L][GP];
		for (int i = 0; i < L; i++) {
			next_score += (i <= idx ? dp[i][GP] : dp2[i][GP]);
		}
		if (score <= next_score || randouble() < exp(-(1.0 - next_score / score) * 5000.0)) {
			if (best_score < next_score) {
				// cerr << "Iteration #" << iteration << ": Score " << best_score << " -> " << next_score << endl;
				best_score = next_score;
				best_seq = seq;
			}
			for (int i = idx + 1; i <= L; i++) {
				dp[i] = dp2[i];
			}
			score = next_score;
		}
		else {
			seq = preseq;
		}
	}

	// step #4. output the answer
	for (int i = 0; i < L; i++) {
		cout << ds[best_seq[i]];
	}
	cout << endl;
	cerr << "Final Score = " << best_score << endl;
	cerr << "Iterations = " << iteration << endl;

	return 0;
}