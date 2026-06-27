#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

unsigned int seed = 1010101;
unsigned int Rand() { seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5; return (seed & 2147483647); }
double Randouble() { return Rand() / 2147483648.0; }
double GetTime() { return 1.0 * clock() / CLOCKS_PER_SEC; }
double Arr[10000];
double double_prob[12][12];

struct State {
	int score;
	vector<vector<int>> prob;
};

bool operator<(const State& a1, const State& a2) {
	if (a1.score < a2.score) return true;
	return false;
}

int getscore(vector<vector<int>>& Prob, vector<string> S, vector<int> P, int N, int M, int L, int border) {
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			double_prob[i][j] = 0.01 * Prob[i][j];
		}
	}

	// 最初の確率の計算
	vector<double> dp(M, 1.0 / M);
	vector<double> ndp(M, 0.0);
	for (int loops = 0; loops < 13; loops++) {
		for (int i = 0; i < M; i++) ndp[i] = 0;
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < M; j++) ndp[j] += double_prob[i][j] * dp[i];
		}
		dp = ndp;
	}

	// スコアの期待値の計算
	vector<double> dp1(13, 0.0);
	vector<double> dp2(13, 0.0);
	double score = 0.0;
	for (int i = 0; i < N; i++) {
		if (P[i] < border) continue;
		int c0 = S[i][0] - 'a';
		int c3 = S[i][S[i].size() - 1] - 'a';
		for (int j = 0; j < S[i].size(); j++) dp1[j] = 0;
		for (int j = 0; j < S[i].size(); j++) dp2[j] = 0;
		dp1[0] = dp[c0 + 0];
		dp2[0] = dp[c0 + 6];
		for (int j = 1; j < S[i].size(); j++) {
			int c1 = S[i][j - 1] - 'a';
			int c2 = S[i][j - 0] - 'a';
			dp1[j] += double_prob[c1 + 0][c2 + 0] * dp1[j - 1];
			dp1[j] += double_prob[c1 + 6][c2 + 0] * dp2[j - 1];
			dp2[j] += double_prob[c1 + 0][c2 + 6] * dp1[j - 1];
			dp2[j] += double_prob[c1 + 6][c2 + 6] * dp2[j - 1];
		}
		double final_prob = dp1[S[i].size() - 1] + dp2[S[i].size() - 1];
		double ac = (final_prob < 1.0e-8 ? 1.0 * L * final_prob : Arr[min(9999, (int)(1.0e9 * final_prob))]);
		score += 10000.0 * P[i] * ac;
	}
	return (int)score;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------- 焼きなまし関数
// ------------------------------------------------------------------------------------------------------------------------------------------------------
State Yakinamashi(vector<vector<int>> prob, int border, vector<string> S, vector<int> P, int N, int M, int L, double TL) {
	int current_score = getscore(prob, S, P, N, M, L, border);
	double start_time = GetTime();

	// 焼きなまし法開始
	while (GetTime() - start_time < TL) {
		// 一点変換
		if (Rand() % 2 == 0) {
			double elapsed = 1.0 * (GetTime() - start_time) / TL;
			double temp = 200000.0 / pow(20.0, elapsed);
			int s1 = Rand() % M;
			int t1 = Rand() % M;
			int t2 = Rand() % M;
			int d = Rand() % 10;
			d = min({ d, prob[s1][t1], 100 - prob[s1][t2] });
			if (d == 0 || t1 == t2) continue;
			prob[s1][t1] -= d;
			prob[s1][t2] += d;
			int new_score = getscore(prob, S, P, N, M, L, border);
			if (Randouble() < exp(1.0 * (new_score - current_score) / temp)) {
				current_score = new_score;
			}
			else {
				prob[s1][t1] += d;
				prob[s1][t2] -= d;
			}
		}

		// 二点変換
		else {
			double elapsed = 1.0 * (GetTime() - start_time) / TL;
			double temp = 200000.0 / pow(20.0, elapsed);
			int s1 = Rand() % M;
			int t1 = Rand() % M;
			int t2 = Rand() % M;
			int u1 = Rand() % M;
			int v1 = Rand() % M;
			int v2 = Rand() % M;
			int d = Rand() % 10;
			int e = Rand() % 10;
			d = min({ d, prob[s1][t1], 100 - prob[s1][t2] });
			e = min({ e, prob[u1][v1], 100 - prob[u1][v2] });
			if (d == 0 || e == 0 || s1 == u1 || t1 == t2 || v1 == v2) continue;
			prob[s1][t1] -= d;
			prob[s1][t2] += d;
			prob[u1][v1] -= e;
			prob[u1][v2] += e;
			int new_score = getscore(prob, S, P, N, M, L, border);
			if (Randouble() < exp(1.0 * (new_score - current_score) / temp)) {
				current_score = new_score;
			}
			else {
				prob[s1][t1] += d;
				prob[s1][t2] -= d;
				prob[u1][v1] += e;
				prob[u1][v2] -= e;
			}
		}
	}

	// 答えを返す
	return State{ current_score, prob };
}



// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------- ソルバー関数
// ------------------------------------------------------------------------------------------------------------------------------------------------------
int solve() {
	// step 1. 入力
	int N, M, L; cin >> N >> M >> L;
	vector<string> S(N, "");
	vector<int> P(N, 0);
	for (int i = 0; i < N; i++) cin >> S[i] >> P[i];

	// step 2. P[i] で降順ソート
	vector<int> Sorted;
	for (int i = 0; i < N; i++) Sorted.push_back(P[i]);
	sort(Sorted.begin(), Sorted.end());
	reverse(Sorted.begin(), Sorted.end());
	const int thres[16] = {
		0, Sorted[0],
		Sorted[1], Sorted[2], Sorted[3], Sorted[4], Sorted[5],
		Sorted[7], Sorted[8], Sorted[10], Sorted[12], Sorted[15],
		Sorted[18], Sorted[22], Sorted[27], Sorted[35]
	};

	// step 3. 閾値を下げていく焼きなまし法・動的計画法による最適化
	State fin; fin.score = -1;
	for (int t = 0; t < 2; t++) {
		vector<State> dp(16);
		for (int i = 0; i < 16; i++) dp[i].score = 0;
		dp[0].prob = vector<vector<int>>(M, vector<int>(M, 0));
		for (int i = 0; i < M; i++) {
			while (true) {
				int total = 0;
				for (int j = 0; j < M; j++) dp[0].prob[i][j] = Rand() % 18;
				for (int j = 0; j < M; j++) total += dp[0].prob[i][j];
				if (total == 100) break;
			}
		}
		for (int turn = 0; turn < 16; turn++) {
			for (int d = 1; d <= min(15 - turn, 2); d++) {
				State nex = Yakinamashi(dp[turn].prob, thres[turn + d], S, P, N, M, L, 0.031);
				dp[turn + d] = max(dp[turn + d], nex);
			}
		}
		fin = max(fin, dp[dp.size() - 1]);
	}

	// step 4. 出力
	for (int i = 0; i < M; i++) {
		cout << (char)('a' + (i % 6));
		for (int j = 0; j < M; j++) cout << ' ' << fin.prob[i][j];
		cout << endl;
	}
	return fin.score;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------- メイン関数
// ------------------------------------------------------------------------------------------------------------------------------------------------------
int main() {
	for (int i = 0; i < 10000; i++) Arr[i] = 1.0 - pow(1.0 - 1.0 * i / 1.0e9, 1.0e6);
	int DEBUG = 1;
	if (DEBUG == 1) {
		solve();
	}
	else {
		for (int t = 0; t < 20; t++) {
			string ID = to_string(t); while (ID.size() < 4) ID = "0" + ID;
			FILE* inp = freopen(("./in/" + ID + ".txt").c_str(), "r", stdin);
			FILE* ou = freopen(("./out/" + ID + ".txt").c_str(), "w", stdout);
			cerr << solve() << endl;
		}
	}
	return 0;
}