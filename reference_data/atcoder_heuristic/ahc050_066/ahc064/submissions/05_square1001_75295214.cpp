// greedy -> beam search via Gemini 3.1 Pro

#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <unordered_set>

using namespace std;

constexpr int R = 10;
constexpr int Y_LIMIT = 15;
constexpr int Z_LIMIT = 20;

constexpr int INF = 1012345678;

constexpr int PARAM_1 = 10;
constexpr int PARAM_2 = 10;
constexpr int PARAM_3 = 1;
constexpr int PARAM_4 = 2;

// Tunable Beam Search parameters
constexpr int BEAM_WIDTH = 65;
constexpr int K_DP = 20;
constexpr int MAX_TURNS = 90;

mt19937_64 mt(1);

class action {
public:
	int t, x, y, z;
	action() : t(-1), x(-1), y(-1), z(-1) {}
	action(int t_, int x_, int y_, int z_) : t(t_), x(x_), y(y_), z(z_) {}
};

bool check(int x, int y) {
	return (y - x == 1 && y % 10 != 0);
}

int get_score(const vector<vector<int>>& Y, const vector<vector<int>>& Z) {
	int score = 0;
	for (int i = 0; i < R; i++) {
		int s = 0;
		for (int j = 0; j < int(Y[i].size()) && j < 10; j++) {
			if (Y[i][j] != 10 * i + j) break;
			s++;
		}
		score += s * PARAM_1;
	}
	for (int i = 0; i < R; i++) {
		for (int j = 0; j < int(Y[i].size()) - 1; j++) {
			score += int(check(Y[i][j], Y[i][j + 1])) * PARAM_2;
		}
		for (int j = 0; j < int(Z[i].size()) - 1; j++) {
			score += int(check(Z[i][j], Z[i][j + 1])) * PARAM_2;
		}
		score += int(!Z[i].empty()) * PARAM_3;
	}
	for (int i = 0; i < R; i++) {
		for (int j = 0; j < R; j++) {
			if (!Y[i].empty() && !Z[j].empty()) {
				score += int(check(Y[i].back(), Z[j].front())) * PARAM_4;
			}
		}
	}
	return score;
}

uint64_t state_hash(const vector<vector<int>>& Y, const vector<vector<int>>& Z) {
	uint64_t h = 14695981039346656037ULL;
	for (int i = 0; i < R; ++i) {
		for (int x : Y[i]) { h ^= x; h *= 1099511628211ULL; }
		h ^= 997; h *= 1099511628211ULL;
	}
	for (int i = 0; i < R; ++i) {
		for (int x : Z[i]) { h ^= x; h *= 1099511628211ULL; }
		h ^= 997; h *= 1099511628211ULL;
	}
	return h;
}

uint64_t action_hash(const vector<action>& acts) {
	uint64_t h = 0;
	for (const auto& a : acts) {
		h ^= (a.t ^ (a.x << 4) ^ (a.y << 8) ^ (a.z << 12));
		h *= 1099511628211ULL;
	}
	return h;
}

struct BeamState {
	vector<vector<int>> Y;
	vector<vector<int>> Z;
	int score;
	int parent_idx;
	vector<action> last_moves;
	uint64_t hash;
	bool operator<(const BeamState& o) const { return score > o.score; }
};

struct DPNode {
	int score;
	int pi, pj, pw, pidx;
	int tp, g; // tp: 0/1 for skips, 2 for Y->Z, 3 for Z->Y
};

bool is_target(const vector<vector<int>>& Y) {
	for (int i = 0; i < R; i++) {
		if (Y[i].size() != 10) return false;
		for (int j = 0; j < 10; j++) {
			if (Y[i][j] != 10 * i + j) return false;
		}
	}
	return true;
}

vector<vector<action>> solve(const vector<vector<int>>& Y_INITIAL) {
	BeamState init_state;
	init_state.Y = Y_INITIAL;
	init_state.Z.resize(R);
	init_state.score = get_score(init_state.Y, init_state.Z);
	init_state.parent_idx = -1;
	init_state.hash = state_hash(init_state.Y, init_state.Z);

	vector<BeamState> cur_beam = { init_state };
	vector<vector<BeamState>> history;
	unordered_set<uint64_t> globally_seen;
	globally_seen.insert(init_state.hash);

	int best_turn = -1, best_idx = -1;

	// Reusable DP table to avoid continuous re-allocation overhead
	static vector<DPNode> dp[R + 1][R + 1][2];

	for (int turn = 0; turn < MAX_TURNS; turn++) {
		history.push_back(cur_beam);

		// Terminate early if target is found
		for (int i = 0; i < cur_beam.size(); i++) {
			if (is_target(cur_beam[i].Y)) {
				best_turn = turn;
				best_idx = i;
				break;
			}
		}
		if (best_turn != -1) break;

		vector<BeamState> next_cands;

		for (int b_idx = 0; b_idx < cur_beam.size(); b_idx++) {
			const auto& Y = cur_beam[b_idx].Y;
			const auto& Z = cur_beam[b_idx].Z;

			// Precalculation
			vector<int> goal_y(R), match_y(R), match_z(R);
			for (int i = 0; i < R; i++) {
				while (goal_y[i] != min(int(Y[i].size()), 10) && Y[i][goal_y[i]] == 10 * i + goal_y[i]) {
					goal_y[i]++;
				}
			}
			for (int i = 0; i < R; i++) {
				for (int j = 0; j < R; j++) {
					if (!Y[i].empty() && !Z[j].empty() && check(Y[i].back(), Z[j].front())) {
						match_y[i]++;
						match_z[j]++;
					}
				}
			}

			// Clear and initialize DP DP
			for (int i = 0; i <= R; i++) {
				for (int j = 0; j <= R; j++) {
					dp[i][j][0].clear();
					dp[i][j][1].clear();
				}
			}
			dp[0][0][0].push_back({ 0, -1, -1, -1, -1, -1, -1 });

			// DP Execution
			for (int i = 0; i <= R; i++) {
				for (int j = 0; j <= R; j++) {
					vector<DPNode> cand[2];

					if (i >= 1) {
						for (int w = 0; w <= 1; w++) {
							for (int pidx = 0; pidx < dp[i - 1][j][w].size(); pidx++) {
								cand[w].push_back({ dp[i - 1][j][w][pidx].score, i - 1, j, w, pidx, 0, -1 });
							}
						}
					}
					if (j >= 1) {
						for (int w = 0; w <= 1; w++) {
							for (int pidx = 0; pidx < dp[i][j - 1][w].size(); pidx++) {
								cand[w].push_back({ dp[i][j - 1][w][pidx].score, i, j - 1, w, pidx, 1, -1 });
							}
						}
					}

					if (i >= 1 && j >= 1) {
						int s = i - 1;
						int t = j - 1;
						vector<tuple<int, int, int>> opts;

						// Type 2: Y -> Z
						for (int k = 1; k <= int(Y[s].size()) && k + int(Z[t].size()) <= Z_LIMIT; k++) {
							int al = (k != Y[s].size() ? Y[s][Y[s].size() - k - 1] : INF);
							int ar = Y[s][Y[s].size() - k];
							int delta = 0;
							delta -= (goal_y[s] - min(int(Y[s].size()) - k, goal_y[s])) * PARAM_1;
							delta -= int(check(al, ar)) * PARAM_2;
							delta += int(!Z[t].empty() && check(Y[s].back(), Z[t].front())) * PARAM_2;
							delta += int(Z[t].empty()) * PARAM_3;
							delta -= match_y[s] * PARAM_4;
							delta -= match_z[t] * PARAM_4;
							delta += int(!Z[t].empty() && check(Y[s].back(), Z[t].front())) * PARAM_4;
							delta += int(check(al, ar)) * PARAM_4;
							for (int l = 0; l < R; l++) {
								if (l != s) delta += int(!Y[l].empty() && check(Y[l].back(), ar)) * PARAM_4;
								if (l != t) delta += int(!Z[l].empty() && check(al, Z[l].front())) * PARAM_4;
							}
							opts.push_back({ delta, 2, k });
						}

						// Type 3: Z -> Y
						for (int k = 1; k <= int(Z[t].size()) && k + int(Y[s].size()) <= Y_LIMIT; k++) {
							int al = Z[t][k - 1];
							int ar = (k != Z[t].size() ? Z[t][k] : INF);
							int delta = 0;
							if (Y[s].size() == goal_y[s]) {
								int aux = 0;
								while (aux < min(int(Z[t].size()), 10 - goal_y[s]) && Z[t][aux] == 10 * s + (goal_y[s] + aux)) {
									aux++;
								}
								delta += aux * PARAM_1;
							}
							delta -= int(check(al, ar)) * PARAM_2;
							delta += int(!Y[s].empty() && check(Y[s].back(), Z[t].front())) * PARAM_2;
							delta -= int(k == Z[t].size()) * PARAM_3;
							delta -= match_y[s] * PARAM_4;
							delta -= match_z[t] * PARAM_4;
							delta += int(!Y[s].empty() && check(Y[s].back(), Z[t].front())) * PARAM_4;
							delta += int(check(al, ar)) * PARAM_4;
							for (int l = 0; l < R; l++) {
								if (l != s) delta += int(!Y[l].empty() && check(Y[l].back(), ar)) * PARAM_4;
								if (l != t) delta += int(!Z[l].empty() && check(al, Z[l].front())) * PARAM_4;
							}
							opts.push_back({ delta, 3, k });
						}

						for (const auto& opt : opts) {
							int delta = get<0>(opt);
							int tp = get<1>(opt);
							int g = get<2>(opt);
							for (int prev_w = 0; prev_w <= 1; prev_w++) {
								for (int pidx = 0; pidx < dp[i - 1][j - 1][prev_w].size(); pidx++) {
									int new_score = dp[i - 1][j - 1][prev_w][pidx].score + delta;
									cand[1].push_back({ new_score, i - 1, j - 1, prev_w, pidx, tp, g });
								}
							}
						}
					}

					// Keep top K_DP variations for the next step of the DP
					for (int w = 0; w <= 1; w++) {
						sort(cand[w].begin(), cand[w].end(), [](const DPNode& a, const DPNode& b) {
							return a.score > b.score;
						});
						for (int idx = 0; idx < min((int)cand[w].size(), K_DP); idx++) {
							dp[i][j][w].push_back(cand[w][idx]);
						}
					}
				}
			}

			// Extract best paths and generate new states
			unordered_set<uint64_t> local_actions;
			for (int pidx = 0; pidx < dp[R][R][1].size(); pidx++) {
				vector<action> sub_action;
				int cy = R, cz = R, cw = 1, cidx = pidx;

				while (cy != 0 || cz != 0) {
					const auto& node = dp[cy][cz][cw][cidx];
					if (node.tp == 2) {
						sub_action.push_back(action(0, cy - 1, cz - 1, node.g));
					} else if (node.tp == 3) {
						sub_action.push_back(action(1, cy - 1, cz - 1, node.g));
					}
					cy = node.pi;
					cz = node.pj;
					cw = node.pw;
					cidx = node.pidx;
				}
				reverse(sub_action.begin(), sub_action.end());

				if (sub_action.empty()) continue;

				// Ensure identical parallel moves aren't processed twice for the same state
				uint64_t ah = action_hash(sub_action);
				if (local_actions.count(ah)) continue;
				local_actions.insert(ah);

				auto NY = Y;
				auto NZ = Z;
				for (const auto& a : sub_action) {
					if (a.t == 0) {
						NZ[a.y].insert(NZ[a.y].begin(), NY[a.x].end() - a.z, NY[a.x].end());
						NY[a.x].erase(NY[a.x].end() - a.z, NY[a.x].end());
					} else {
						NY[a.x].insert(NY[a.x].end(), NZ[a.y].begin(), NZ[a.y].begin() + a.z);
						NZ[a.y].erase(NZ[a.y].begin(), NZ[a.y].begin() + a.z);
					}
				}

				uint64_t sh = state_hash(NY, NZ);
				if (!globally_seen.count(sh)) {
					globally_seen.insert(sh);
					next_cands.push_back({ NY, NZ, get_score(NY, NZ), b_idx, sub_action, sh });
				}
			}
		}

		if (next_cands.empty()) break;

		sort(next_cands.begin(), next_cands.end());
		if (next_cands.size() > BEAM_WIDTH) {
			next_cands.resize(BEAM_WIDTH);
		}
		cur_beam = next_cands;
	}

	// Fallback if full solution wasn't found in MAX_TURNS
	if (best_turn == -1) {
		best_turn = history.size() - 1;
		best_idx = 0;
		for (int i = 0; i < history.back().size(); i++) {
			if (history.back()[i].score > history.back()[best_idx].score) {
				best_idx = i;
			}
		}
	}

	vector<vector<action>> ans;
	int c_turn = best_turn;
	int c_idx = best_idx;

	while (c_turn > 0) {
		ans.push_back(history[c_turn][c_idx].last_moves);
		c_idx = history[c_turn][c_idx].parent_idx;
		c_turn--;
	}
	reverse(ans.begin(), ans.end());

	return ans;
}

int test(int samples) {
	int result = 0;
	for (int id = 0; id < samples; id++) {
		vector<int> P(R * 10);
		for (int i = 0; i < R * 10; i++) {
			P[i] = i;
			swap(P[i], P[mt() % (i + 1)]);
		}
		vector<vector<int>> Y(R, vector<int>(10));
		for (int i = 0; i < R; i++) {
			for (int j = 0; j < 10; j++) {
				Y[i][j] = P[i * 10 + j];
			}
		}
		vector<vector<action>> res = solve(Y);
		result += res.size();
		cerr << "id=" << id << ": " << res.size() << endl;
	}
	return result;
}

int main() {
	// cerr << test(50) << endl;
	// return 0;

	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	int R_TMP;
	if (!(cin >> R_TMP)) return 0;
	vector<vector<int>> Y(R, vector<int>(10));
	for (int i = 0; i < R; i++) {
		for (int j = 0; j < 10; j++) {
			cin >> Y[i][j];
		}
	}

	vector<vector<action>> ans = solve(Y);
	cout << ans.size() << "\n";
	for (const vector<action>& v : ans) {
		cout << v.size() << "\n";
		for (const action& a : v) {
			cout << a.t << " " << a.x << " " << a.y << " " << a.z << "\n";
		}
	}
	return 0;
}