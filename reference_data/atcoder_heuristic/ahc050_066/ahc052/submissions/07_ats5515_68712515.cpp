#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>	// For memcpy/memmove
#include <iomanip>	// For std::fixed
#include <iostream>
#include <string>
#include <utility>	// For std::pair
#include <vector>

using namespace std;

// --- Provided Timer and XorShift classes ---
const int64_t CYCLES_PER_SEC = 2900000000;
const double TIMELIMIT = 1.95;
struct Timer {
	int64_t start;
	Timer() { reset(); }
	void reset() { start = getCycle(); }
	inline double get() { return (double)(getCycle() - start) / CYCLES_PER_SEC; }
	inline int64_t getCycle() {
		uint32_t low, high;
		__asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
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
		if (m == 0) return 0;
		return (int)(next() % m);
	}

	int nextInt(int min, int max) {
		return min + nextInt(max - min + 1);
	}

	inline double nextDouble() {
		return (double)next() / ((long long)1 << 32);
	}
};
Timer timer;
XorShift rnd;
// --- End of provided classes ---

// Problem constants
constexpr int N = 30;
constexpr int M = 10;
constexpr int K = 10;
constexpr int MAX_T = 2 * N * N;

// Globals for input data (using raw arrays)
pair<int, int> initial_pos[M];
bool v_walls[N][N - 1];	 // Vertical walls
bool h_walls[N - 1][N];	 // Horizontal walls
bool can_move[N][N][5];
// Move mapping: 0:U, 1:D, 2:L, 3:R, 4:S
const string MOVES = "UDLRS";
const int di[] = {-1, 1, 0, 0, 0};
const int dj[] = {0, 0, -1, 1, 0};

using Config = int[K][M];
// State representation using raw array for actions
struct State {
	Config config;
	int actions[MAX_T];
	int T;	// Current length of actions
};

// Simulates and returns {unwaxed_count, effective_turns}
pair<int, int> simulate(const Config config, const int* actions, int T) {
	bool waxed[N][N] = {};
	int unwaxed_count = N * N;
	pair<int, int> robot_pos[M];
	memcpy(robot_pos, initial_pos, sizeof(initial_pos));

	for (int m = 0; m < M; ++m) {
		if (!waxed[robot_pos[m].first][robot_pos[m].second]) {
			waxed[robot_pos[m].first][robot_pos[m].second] = true;
			unwaxed_count--;
		}
	}

	if (unwaxed_count == 0) {
		return {0, 0};
	}

	for (int t = 0; t < T; ++t) {
		int button = actions[t];
		for (int m = 0; m < M; ++m) {
			int action = config[button][m];
			if (action == 4) continue;

			int cur_i = robot_pos[m].first;
			int cur_j = robot_pos[m].second;

			if (can_move[cur_i][cur_j][action]) {
				robot_pos[m].first += di[action];
				robot_pos[m].second += dj[action];
			}

			if (!waxed[robot_pos[m].first][robot_pos[m].second]) {
				waxed[robot_pos[m].first][robot_pos[m].second] = true;
				unwaxed_count--;
				if (unwaxed_count == 0) {
					return {0, t + 1};	// All waxed, return effective turns
				}
			}
		}
	}
	return {unwaxed_count, T};	// Not all waxed, return total turns
}

// Evaluation function for simulated annealing
long long eval_score(int unwaxed_count, int effective_T) {
	if (unwaxed_count == 0) {
		return effective_T;
	}
	return unwaxed_count * 10 + effective_T;
}

int convert_move(int a) {
	a = (a + 4) % 4;
	if (a == 1) {
		a = 2;
	} else if (a == 2) {
		a = 1;
	}
	return a;
}
int moves_type_pre_calc[8][3];
void read_input() {
	int n_dummy, m_dummy, k_dummy;
	cin >> n_dummy >> m_dummy >> k_dummy;

	for (int m = 0; m < M; ++m) {
		cin >> initial_pos[m].first >> initial_pos[m].second;
	}

	for (int i = 0; i < N; ++i) {
		string s;
		cin >> s;
		for (int j = 0; j < N - 1; ++j) {
			v_walls[i][j] = (s[j] == '1');
		}
	}
	for (int i = 0; i < N - 1; ++i) {
		string s;
		cin >> s;
		for (int j = 0; j < N; ++j) {
			h_walls[i][j] = (s[j] == '1');
		}
	}
	// can_move
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			for (int d = 0; d < 5; ++d) {
				can_move[i][j][d] = true;
				if (d < 4) {
					if (d == 0) {  // U
						if (i == 0 || h_walls[i - 1][j]) can_move[i][j][d] = false;
					} else if (d == 1) {  // D
						if (i == N - 1 || h_walls[i][j]) can_move[i][j][d] = false;
					} else if (d == 2) {  // L
						if (j == 0 || v_walls[i][j - 1]) can_move[i][j][d] = false;
					} else {  // R
						if (j == N - 1 || v_walls[i][j]) can_move[i][j][d] = false;
					}
				}
			}
		}
	}

	moves_type_pre_calc[8][3];
	for (int i = 0; i < 8; i++) {
		int start = i % 4;
		int dir = i < 4 ? 1 : -1;
		moves_type_pre_calc[i][0] = convert_move(start);
		moves_type_pre_calc[i][1] = convert_move(start + dir * 2);
		moves_type_pre_calc[i][2] = convert_move(start + dir);
	}
}

const int T_ub = 600;
// UUUUU...LDDDDD...LU.....のような動きのみを考える
void update_move_config(int m, int movetype, State& current_state) {
	current_state.config[0][m] = moves_type_pre_calc[movetype][0];
	current_state.config[1][m] = moves_type_pre_calc[movetype][1];
	current_state.config[2][m] = moves_type_pre_calc[movetype][2];
}

int update_move_seq(int cur_seq[], State& current_state) {
	int t = 0;
	int cur = 0;
	int move_a = 0;
	while (cur < current_state.T) {
		for (int i = 0; i < cur_seq[t]; i++) {
			if (cur >= current_state.T) break;
			current_state.actions[cur++] = move_a;
		}
		move_a = 1 - move_a;
		if (cur >= current_state.T) break;
		current_state.actions[cur++] = 2;
		t++;
	}
	return t;
}
State SA1() {
	const int MAX_SEQ = 1000;
	// Simulated Annealing Phase 1
	State current_state;
	for (int k = 0; k < K; ++k) {
		for (int m = 0; m < M; ++m) {
			current_state.config[k][m] = rnd.nextInt(5);
		}
	}

	current_state.T = T_ub;

	int cur_movetype[M];
	int cur_seq[MAX_SEQ];
	int cur_max_t = 0;
	for (int m = 0; m < M; ++m) {
		cur_movetype[m] = rnd.nextInt(8);
		update_move_config(m, cur_movetype[m], current_state);
	}
	for (int i = 0; i < MAX_SEQ; i++) {
		cur_seq[i] = 15;
	}
	cur_max_t = update_move_seq(cur_seq, current_state);
	pair<int, int> sim_result = simulate(current_state.config, current_state.actions, current_state.T);
	int current_score = eval_score(sim_result.first, sim_result.second);

	State best_state = current_state;
	memcpy(best_state.actions, current_state.actions, current_state.T * sizeof(int));
	int best_score = current_score;

	// --- Simulated Annealing ---
	double time_limit = TIMELIMIT * 0.8;
	double start_temp = 15.0;
	long long iter_count = 0;
	double st = timer.get();
	double debug_interval = (time_limit - st) / 5.0;
	double next_debug_time = st + debug_interval;
	while (true) {
		double ti = timer.get();
		if (ti > time_limit) break;
		iter_count++;
		double progress = ti / time_limit;
		double temp = start_temp * (1 - progress);

		// --- DEBUG OUTPUT ---
		if (ti > next_debug_time) {
			next_debug_time += debug_interval;
			cerr << fixed << setprecision(4)
				 << "Time: " << timer.get() << "s, "
				 << "Temp: " << temp << ", "
				 << "Current Score: " << current_score << ", "
				 << "Best Score: " << best_score << endl;
		}

		double move_type = rnd.nextDouble();
		bool updated = false;

		if (move_type < 0.5) {
			int m = rnd.nextInt(M);
			int new_move_type = rnd.nextInt(7);
			int old_move_type = cur_movetype[m];
			if (old_move_type <= new_move_type) new_move_type++;
			update_move_config(m, new_move_type, current_state);

			pair<int, int> next_sim_result = simulate(current_state.config, current_state.actions, current_state.T);
			int next_score = eval_score(next_sim_result.first, next_sim_result.second);
			int delta_score = current_score - next_score;
			if (delta_score >= 0 || (delta_score >= temp * rnd.nextLog())) {
				updated = true;
				current_score = next_score;
				cur_movetype[m] = new_move_type;
				if (next_sim_result.first == 0) {
					current_state.T = min(current_state.T, next_sim_result.second + 10);
				}
			} else {
				update_move_config(m, old_move_type, current_state);
			}
		} else {
			int t = rnd.nextInt(cur_max_t);
			int new_seq = rnd.nextInt(N - 2);
			int old_seq = cur_seq[t];
			if (old_seq <= new_seq) new_seq++;
			cur_seq[t] = new_seq;
			cur_max_t = update_move_seq(cur_seq, current_state);
			pair<int, int> next_sim_result = simulate(current_state.config, current_state.actions, current_state.T);
			int next_score = eval_score(next_sim_result.first, next_sim_result.second);
			int delta_score = current_score - next_score;
			if (delta_score >= 0 || (delta_score >= temp * rnd.nextLog())) {
				updated = true;
				current_score = next_score;
				if (next_sim_result.first == 0) {
					current_state.T = min(current_state.T, next_sim_result.second + 10);
				}
			} else {
				cur_seq[t] = old_seq;
				cur_max_t = update_move_seq(cur_seq, current_state);
			}
		}

		if (updated) {
			if (current_score < best_score) {
				best_state = current_state;
				best_score = current_score;
				memcpy(best_state.actions, current_state.actions, current_state.T * sizeof(int));
			}
		}
	}
	cerr << "total iters = " << iter_count << endl;
	return best_state;
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	read_input();

	State current_state = SA1();

	current_state.T = T_ub;

	pair<int, int> sim_result = simulate(current_state.config, current_state.actions, current_state.T);
	int current_score = eval_score(sim_result.first, sim_result.second);

	State best_state = current_state;
	memcpy(best_state.actions, current_state.actions, current_state.T * sizeof(int));
	int best_score = current_score;

	// --- Simulated Annealing ---
	double start_temp = 1.0;
	long long iter_count = 0;
	double st = timer.get();
	double debug_interval = (TIMELIMIT - st) / 5.0;
	double next_debug_time = st + debug_interval;
	int no_improvements = 0;
	while (true) {
		double ti = timer.get();
		if (ti > TIMELIMIT) break;
		iter_count++;
		double progress = (ti - st) / (TIMELIMIT - st);
		double temp = start_temp * (1 - progress);

		// --- DEBUG OUTPUT ---
		if (ti > next_debug_time) {
			next_debug_time += debug_interval;
			cerr << fixed << setprecision(4)
				 << "Time: " << timer.get() << "s, "
				 << "Temp: " << temp << ", "
				 << "Current Score: " << current_score << ", "
				 << "Best Score: " << best_score << endl;
		}

		double move_type = rnd.nextDouble();
		bool updated = false;

		if (move_type < 0.5) {	// Change button config
			int k = rnd.nextInt(K);
			int m = rnd.nextInt(M);
			int new_m = rnd.nextInt(4);
			int old_m = current_state.config[k][m];
			if (old_m <= new_m) new_m++;
			current_state.config[k][m] = new_m;

			pair<int, int> next_sim_result = simulate(current_state.config, current_state.actions, current_state.T);
			int next_score = eval_score(next_sim_result.first, next_sim_result.second);
			int delta_score = current_score - next_score;
			if (delta_score >= 0 || (delta_score >= temp * rnd.nextLog())) {
				updated = true;
				current_state.T = min(T_ub, next_sim_result.second + 10);
				current_score = next_score;
			} else {
				current_state.config[k][m] = old_m;
			}
		} else {  // Change an action
			int t = rnd.nextInt(current_state.T);
			int new_k = rnd.nextInt(K - 1);
			int old_k = current_state.actions[t];
			if (old_k <= new_k) new_k++;
			current_state.actions[t] = new_k;
			pair<int, int> next_sim_result = simulate(current_state.config, current_state.actions, current_state.T);
			int next_score = eval_score(next_sim_result.first, next_sim_result.second);
			int delta_score = current_score - next_score;
			if (delta_score >= 0 || (delta_score >= temp * rnd.nextLog())) {
				updated = true;
				current_score = next_score;
				current_state.T = min(T_ub, next_sim_result.second + 10);
			} else {
				current_state.actions[t] = old_k;
			}
		}

		if (updated) {
			if (current_score < best_score) {
				best_state = current_state;
				best_score = current_score;
				memcpy(best_state.actions, current_state.actions, current_state.T * sizeof(int));
			} 
		}

		// if (no_improvements >= 10000) {
		// 	current_score = best_score;
		// 	memcpy(current_state.actions, best_state.actions, best_state.T * sizeof(int));
		// }
	}

	cerr << "total iters = " << iter_count << endl;

	// --- Output the best found solution ---
	pair<int, int> final_sim_result = simulate(best_state.config, best_state.actions, best_state.T);
	int final_T = final_sim_result.second;

	for (int k = 0; k < K; ++k) {
		for (int m = 0; m < M; ++m) {
			cout << MOVES[best_state.config[k][m]] << (m == M - 1 ? "" : " ");
		}
		cout << "\n";
	}

	for (int i = 0; i < final_T; ++i) {
		cout << best_state.actions[i] << "\n";
	}

	return 0;
}