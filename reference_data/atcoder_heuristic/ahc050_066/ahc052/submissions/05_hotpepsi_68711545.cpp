#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <vector>
#include <queue>
#include <string>
#include <set>
#include <map>
#include <bitset>
#include <unordered_map>
#include <random>

using namespace std;

const int TIME_LIMIT = 2000 - 100;
//const int TIME_LIMIT = 500 - 100;

typedef pair<int, int> II;

#ifdef _MSC_VER
#define popcount __popcnt64
#else
#define popcount __builtin_popcountll
#endif

std::random_device seed_gen;
std::mt19937_64 engine(seed_gen());
static inline int _rand(int range) { return int(engine() % range); }

auto start_time = chrono::high_resolution_clock::now();
inline int get_elapsed_ms() { return int(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_time).count()); }

const int dy[5] = { 0, 1, 0, -1, 0 };
const int dx[5] = { 1, 0, -1, 0, 0 };
const string dirs = "RDLUS";
int char_to_dir[256];
#define BSSZ 15

int tries = 0;

struct Bitset {
	uint64_t bits[BSSZ];

	Bitset() { clear(); }
	void clear() { memset(bits, 0, BSSZ * 8); }
	Bitset& operator=(const Bitset& src) { memcpy(bits, src.bits, BSSZ * 8); return *this; }

	inline void set(int pos) {
		bits[pos / 64] |= 1LL << (pos & 63);
	}

	inline bool is_set(int pos) const {
		return (bits[pos / 64] & (1LL << (pos & 63))) != 0;
	}

	int count() const {
		int cnt = 0;
		for (int i = 0; i < BSSZ; ++i) {
			cnt += int(popcount(bits[i]));
		}
		return cnt;
	}

	//bool operator ==(const Bitset& r) const { return memcmp(bits, r.bits, BSSZ * 8) == 0; }

	inline Bitset& operator |=(const Bitset& r) {
		for (int i = 0; i < BSSZ; ++i) {
			bits[i] |= r.bits[i];
		}
		return *this;
	}

	inline Bitset& operator &=(const Bitset& r) {
		for (int i = 0; i < BSSZ; ++i) {
			bits[i] &= r.bits[i];
		}
		return *this;
	}

	inline bool operator[](int pos) const {
		return (bits[pos / 64] & (1LL << (pos & 63))) != 0;
	}
};

struct Command {
	int op;
	int count;
};

struct History {
	int score;
	int turns;
	vector<vector<int>> ops;
	vector<Command> commands;
	vector<Bitset> clean;
};

struct Params {
	int init_ops_base;
	int init_ops_range;
	int init_step_base;
	int init_step_range;
	int move_ops_base;
	int move_ops_range;
	int move_step_base;
	int move_step_range;
};

struct Solver {
	int N, M, K, H, W;
	vector<II> rpos;
	vector<vector<vector<int>>> wall;

	void input() {
		cin >> N >> M >> K;
		H = N;
		W = N;
		rpos.resize(M);
		for (int i = 0; i < M; ++i) {
			cin >> rpos[i].first >> rpos[i].second;
		}
		wall = vector<vector<vector<int>>>(H, vector<vector<int>>(W, vector<int>(4, 1)));
		string s;
		for (int i = 0; i < H; ++i) {
			cin >> s;
			for (int j = 0; j < W - 1; ++j) {
				wall[i][j + 1][2] = wall[i][j][0] = s[j] != '0';
			}
		}
		for (int i = 0; i < H - 1; ++i) {
			cin >> s;
			for (int j = 0; j < W; ++j) {
				wall[i + 1][j][3] = wall[i][j][1] = s[j] != '0';
			}
		}
	}

	void prepare() {
		for (int i = 0; i < 4; ++i) {
			char_to_dir[dirs[i]] = i;
		}

	}

	void output_result(const History& history) {
		for (int k = 0; k < K; ++k) {
			for (int i = 0; i < M; ++i) {
				if (i) cout << " ";
				const vector<int>& ops = history.ops[i];
				cout << dirs[k < ops.size() ? ops[k] : 4];
			}
			cout << endl;
		}
		for (const auto& command : history.commands) {
			for (int i = 0; i < command.count; ++i) {
				cout << command.op << endl;
			}
		}
	}

	void init_history(History& history) {
		history.score = 0;
		history.ops = vector<vector<int>>(M);
		history.commands.clear();
		history.clean = vector<Bitset>(M);
		for (int i = 0; i < M; ++i) {
			history.clean[i].set(rpos[i].first * N + rpos[i].second);
		}
	}

	void simulate(const History& history, int ri, const vector<int>& ops, Bitset& clean) {
		int y = rpos[ri].first, x = rpos[ri].second;
		clean.set(y * N + x);
		for (const auto& command : history.commands) {
			for (int i = 0; i < command.count; ++i) {
				int op = ops[command.op];
				if (!wall[y][x][op]) {
					y += dy[op];
					x += dx[op];
					clean.set(y * N + x);
				}
			}
		}
	}

	int evaluate(const History& history, int target_ri, const Bitset& target_clean) {
		Bitset bits = target_clean;
		for (int i = 0; i < M; ++i) {
			if (i != target_ri) {
				bits |= history.clean[i];
			}
		}
		int score = int(bits.count());
		if (score >= N * N) {
			score = 3 * N * N - history.turns;
		}
		return score;
	}

	bool update_one_robot(History& history, int ri, int nr_ops) {
		vector<int> ops(nr_ops);
		for (int i = 0; i < nr_ops; ++i) {
			ops[i] = _rand(4);
		}
		ops[nr_ops - 1] = ops[nr_ops - 2] ^ 2;
		ops[nr_ops - 3] = ops[nr_ops - 1] ^ 1;
		Bitset clean;
		simulate(history, ri, ops, clean);

		int score = evaluate(history, ri, clean);
		if (score <= history.score) {
			return false;
		}
		history.score = score;
		history.ops[ri] = ops;
		history.clean[ri] = clean;
		return true;
	}

	void gen(History& history, const Params &params) {
		++tries;
		init_history(history);

		int nr_ops = 0;
		int init_ops = min(7, params.init_ops_base + _rand(params.init_ops_range));
		for (int i = 0; i < init_ops; ++i) {
			int steps = params.init_step_base + _rand(params.init_step_range);
			history.commands.emplace_back(Command({ nr_ops++, steps }));
		}
		int op1 = nr_ops++;
		int op2 = nr_ops++;
		int op3 = nr_ops++;
		int move_ops = params.move_ops_base + _rand(params.move_ops_range);
		int steps = params.move_step_base + _rand(params.move_step_range);
		for (int i = 0; i < move_ops; ++i) {
			history.commands.emplace_back(Command({ op2, steps }));
			history.commands.emplace_back(Command({ op1, 1 }));
			history.commands.emplace_back(Command({ op3, steps }));
			history.commands.emplace_back(Command({ op1, 1 }));
		}

		int turns = 0;
		for (const auto& command : history.commands) {
			turns += command.count;
		}
		history.turns = turns;

		for (int i = 0; i < 200; ++i) {
			update_one_robot(history, _rand(M), nr_ops);
		}
	}

	History run() {
		History best_history;
		best_history.score = 0;

		Params params;
		params.init_ops_base = 3;
		params.init_ops_range = 5;
		params.init_step_base = 4;
		params.init_step_range = 4;
		params.move_ops_base = 4;
		params.move_ops_range = 6;
		params.move_step_base = 4;
		params.move_step_range = 6;

		for (int i = 1; i <= 10; ++i) {
			while (get_elapsed_ms() < i * 100) {
				History history;
				gen(history, params);
				if (history.score > best_history.score) {
					best_history = history;
				}
			}

			if (best_history.score >= N * N) {
				break;
			}

			params.init_step_range += 1;
			params.move_ops_range += 1;
			params.move_step_range += 1;
			
			cerr << "ELAPSED: " << get_elapsed_ms() << ", SCORE: " << best_history.score << endl;
		}

		while (get_elapsed_ms() < TIME_LIMIT) {
			History history;
			gen(history, params);
			if (history.score > best_history.score) {
				best_history = history;
			}
		}

		cerr << "SCORE: " << best_history.score << ", tries: " << tries << endl;
		return best_history;
	}

};

int main(int argc, char* argv[]) {
	Solver solver;
	solver.input();
	solver.prepare();
	solver.output_result(solver.run());
}
