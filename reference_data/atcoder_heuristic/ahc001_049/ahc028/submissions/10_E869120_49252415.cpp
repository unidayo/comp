#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

// ========================================================================================================================== Input & Initialize
struct State {
	vector<int> dp;
	vector<bool> used;
	int prev_letter;
	int prev_state;
	int prev_select;
	double max_score;
};

bool operator<(const State& a1, const State& a2) {
	if (a1.max_score < a2.max_score) return true;
	return false;
}

int N; char C[29][29];
int M; string S[209];
int sx, sy;
int Dist[29][29][209][209];
vector<pair<int, int>> List[27];

const int WIDTH = 1000;
vector<State> Beam[209];

void Initialize() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) List[C[i][j] - 'A'].push_back(make_pair(i, j));
	}
	List[26].push_back(make_pair(sx, sy));
	for (int i = 0; i < 27; i++) {
		for (int j = 0; j < 27; j++) {
			for (int k = 0; k < List[i].size(); k++) {
				for (int l = 0; l < List[j].size(); l++) {
					Dist[i][j][k][l] = abs(List[i][k].first - List[j][l].first) + abs(List[i][k].second - List[j][l].second) + 1;
				}
			}
		}
	}
}


// ========================================================================================================================== Score Function for Final
vector<pair<int, int>> GetScore(vector<int> Order) {
	vector<vector<int>> DP(1001, vector<int>(201, 0));
	vector<vector<int>> PR(1001, vector<int>(201, 0));
	vector<pair<int, int>> Answer;
	string Target = "";

	// Step 1. Decide Target
	for (int i = 0; i < M; i++) {
		if (i >= 1 && Target[Target.size() - 2] == S[Order[i]][0] && Target[Target.size() - 1] == S[Order[i]][1]) {
			Target += S[Order[i]].substr(2, S[Order[i]].size() - 2);
		}
		else if (i >= 1 && Target[Target.size() - 1] == S[Order[i]][0]) {
			Target += S[Order[i]].substr(1, S[Order[i]].size() - 1);
		}
		else {
			Target += S[Order[i]];
		}
	}

	// Step 2. Initialize
	for (int i = 0; i < Target.size(); i++) {
		int letter1 = Target[i] - 'A';
		for (int j = 0; j < List[letter1].size(); j++) {
			int cx = List[letter1][j].first;
			int cy = List[letter1][j].second;
			DP[i][j] = (i == 0 ? abs(sx - cx) + abs(sy - cy) + 1 : 1000000);
			PR[i][j] = -1;
		}
	}

	// Step 3. Dynamic Programming
	for (int i = 1; i < Target.size(); i++) {
		int letter1 = Target[i - 1] - 'A';
		int letter2 = Target[i - 0] - 'A';
		for (int j = 0; j < List[letter1].size(); j++) {
			for (int k = 0; k < List[letter2].size(); k++) {
				if (DP[i][k] > DP[i - 1][j] + Dist[letter1][letter2][j][k]) {
					DP[i][k] = DP[i - 1][j] + Dist[letter1][letter2][j][k];
					PR[i][k] = j;
				}
			}
		}
	}

	// Step 4. Get Min
	int minid = -1;
	int minx = 10000;
	for (int i = 0; i < List[Target[Target.size() - 1] - 'A'].size(); i++) {
		if (minx > DP[Target.size() - 1][i]) {
			minx = DP[Target.size() - 1][i];
			minid = i;
		}
	}

	// Step 5. Fukugen
	Answer.clear();
	for (int i = Target.size() - 1; i >= 0; i--) {
		int letter = Target[i] - 'A';
		Answer.push_back(List[letter][minid]);
		minid = PR[i][minid];
	}
	reverse(Answer.begin(), Answer.end());
	return Answer;
}


// ========================================================================================================================== DP Function
vector<int> Calculate(vector<int> OldDP, int prevl, string str) {
	string cur = ""; cur += ('A' + prevl); cur += str;
	vector<int> NewDP;

	// Dynamic Programming
	for (int i = 0; i < cur.size() - 1; i++) {
		int letter1 = cur[i + 0] - 'A';
		int letter2 = cur[i + 1] - 'A';
		NewDP = vector<int>(List[letter2].size(), 1000000);
		for (int j = 0; j < List[letter1].size(); j++) {
			for (int k = 0; k < List[letter2].size(); k++) {
				NewDP[k] = min(NewDP[k], OldDP[j] + Dist[letter1][letter2][j][k]);
			}
		}
		OldDP = NewDP;
	}
	return OldDP;
}


// ========================================================================================================================== Main Part
int main() {
	// Step 1. Input
	cin >> N >> M;
	cin >> sx >> sy;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) cin >> C[i][j];
	}
	for (int i = 0; i < M; i++) cin >> S[i];

	// Step 2. Initialize
	Initialize();
	State Zero;
	Zero.used.resize(M, false);
	Zero.dp.resize(1, 0);
	Zero.prev_letter = 26;
	Zero.prev_state = -1;
	Zero.prev_select = -1;
	Zero.max_score = 0;
	Beam[0].push_back(Zero);

	// Step 3. Calculate Base Cost
	vector<int> Base(M, 100000);
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < 26; j++) {
			vector<int> OldDP(List[j].size(), 0);
			vector<int> Return = Calculate(OldDP, j, S[i]);
			for (int k : Return) Base[i] = min(Base[i], k);
		}
	}

	// Step 4. Beam Search
	for (int pos = 0; pos < M; pos++) {
		vector<State> Candidate;

		// Enumerate Phase
		for (int idx = 0; idx < Beam[pos].size(); idx++) {
			State prevs = Beam[pos][idx];
			vector<int> L1;
			vector<int> L2;
			for (int i = 0; i < M; i++) {
				if (prevs.used[i] == true) continue;
				if (prevs.prev_select != -1 && S[i][0] == S[prevs.prev_select][4]) L1.push_back(i);
				else if (prevs.prev_select != -1 && S[i][0] == S[prevs.prev_select][3] && S[i][1] == S[prevs.prev_select][4]) L1.push_back(i);
				else L2.push_back(i);
			}
			vector<int> Rep = L1;
			if (L1.size() <= 1) {
				for (int i = 0; i < L2.size() * 3; i++) swap(L2[rand() % L2.size()], L2[rand() % L2.size()]);
				for (int i = 0; i < min(10, (int)L2.size()); i++) Rep.push_back(L2[i]);
			}
			int CurrentBase = 0;
			for (int i = 0; i < M; i++) CurrentBase += (prevs.used[i] == true ? 0 : Base[i]);

			// Brute Part
			for (int i : Rep) {
				string Addition = S[i];
				if (prevs.prev_select != -1 && S[i][0] == S[prevs.prev_select][4]) Addition = S[i].substr(1, 4);
				if (prevs.prev_select != -1 && S[i][0] == S[prevs.prev_select][3] && S[i][1] == S[prevs.prev_select][4]) Addition = S[i].substr(2, 3);
				vector<int> NewDP = Calculate(prevs.dp, prevs.prev_letter, Addition);
				State NewState = prevs;
				NewState.used[i] = true;
				NewState.dp = NewDP;
				NewState.prev_letter = S[i][4] - 'A';
				NewState.prev_state = idx;
				NewState.prev_select = i;
				NewState.max_score = 10000;
				for (int i = 0; i < NewDP.size(); i++) NewState.max_score = min(NewState.max_score, 1.0 * NewDP[i]);
				NewState.max_score += 1.0 * (CurrentBase - Base[i]);
				Candidate.push_back(NewState);
			}
		}

		// Sorting Phase
		sort(Candidate.begin(), Candidate.end());
		for (int i = 0; i < min(WIDTH, (int)Candidate.size()); i++) {
			Beam[pos + 1].push_back(Candidate[i]);
		}
	}

	// Step 5. Fukugen
	vector<int> FinalOrder(M, -1);
	int cx = 0;
	for (int pos = M - 1; pos >= 0; pos--) {
		FinalOrder[pos] = Beam[pos + 1][cx].prev_select;
		cx = Beam[pos + 1][cx].prev_state;
	}

	// Step 6. Output
	vector<pair<int, int>> Answer = GetScore(FinalOrder);
	for (pair<int, int> index : Answer) cout << index.first << " " << index.second << endl;
	return 0;
}