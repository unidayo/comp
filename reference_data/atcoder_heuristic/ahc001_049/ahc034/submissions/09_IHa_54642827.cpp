#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <cstdio>
#include <math.h>
#include <algorithm>
#include <vector>
#include <string>
#include <stdlib.h>
#include <queue>
#include <stack>
#include <utility>
#include <fstream>
#include <random>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <functional>
#include <time.h>
//#include <chrono>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <iostream>
#include <limits>
#include <set>
#include <numeric>
//#include <intrin.h>
#include <sstream> // std::stringstream
#include <bitset>  // std::bitset<N>
//#include <type_traits>
//#include <atcoder/dsu>
#include <cassert>
#include <chrono>
#include <tuple>
//#include <atcoder/segtree>;
//using namespace atcoder;

#define ll long long
#define rep(i,n) for(ll i=0;i<n;i++)
#define rrep(i,n) for(ll i=n-1;i>=0;i--)
#define FOR(i,a,b) for(ll i=a;i<b;i++)
#define RFOR(i,a,b) for(ll i=b-1;i>=a;i--)
//#define INF 1000000001
#define SORT(s) sort(s.begin(), s.end());
#define REV(s) reverse(s.begin(), s.end());
#define SUM(s) accumulate(s.begin(), s.end(), 0LL);
//#define P pair<ll,ll>
#define mp make_pair
#define valid(x,y,h,w) (0<=x&&x<h&&0<=y&&y<w)
//#define vint vector<int>
//#define vvint vector<vector<int>>
#define vll vector<ll>
#define vint vector<int>
#define vvll vector<vector<ll>>
#define vvint vector<vector<int>>
#define ALL(a)  (a).begin(),(a).end()
#define co(a)  cout << a << endl
//#define int ll
//ll dx[4] = { 0,1,0,-1 };
//ll dy[4] = { 1,0,-1,0 };
ll ndx[8] = { 0,1,0,-1, 1,1,-1,-1 };
ll ndy[8] = { 1,0,-1,0, 1,-1,-1,1 };
double pi = 3.141592653589793;
using namespace std;
#define all(v) (v).begin(), (v).end()

std::mt19937 mt_for_action(0);

bool LOCAL_TEST = 0;

static constexpr const int dx[5] = { 0, 1, 0, -1, 0 };
static constexpr const int dy[5] = { 1, 0, -1, 0, 0 };
static constexpr const char dir[5] = { 'R', 'D', 'L', 'U', '.' };

const ll INF = 1000000000000000000;

using ScoreType = int64_t;
//constexpr const ScoreType INF = 1000000000LL;

struct StopWatch
{
	StopWatch() { pre_ = std::chrono::high_resolution_clock::now(); }

	//前回のlap関数コールからの経過時間をmilli sec単位で返す
	double lap()
	{
		auto tmp = std::chrono::high_resolution_clock::now();  // 計測終了時刻を保存
		auto dur = tmp - pre_;
		return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() / 1000000.0;
	}
	std::chrono::high_resolution_clock::time_point pre_;
};
ll n;
vvint h0;


// 座標を保持する
struct Coord
{
	ll x;
	ll y;
	Coord(const ll x_ = 0, const ll y_ = 0) : x(x_), y(y_) {}

	void move(ll k) {
		x += dx[k];
		y += dy[k];
	}
	bool ok() {
		return valid(x, y, n, n);
	}
	pair<ll, ll> get_p() {
		return mp(x, y);
	}

	//Coord get_next() {
	//	if (x % 2 == 0) {
	//		if (y == n - 1) {
	//			Coord(x + 1, y);
	//		}
	//		else {
	//			Coord(x, y + 1);
	//		}
	//	}
	//	else {
	//		if (y == 0) {
	//			Coord(x + 1, y);
	//		}
	//		else {
	//			Coord(x, y - 1);
	//		}
	//	}
	//}
	int get_next_k(int now_k, vvint& seen) {
		int next_k = now_k;
		int next_x = x + dx[next_k];
		int next_y = y + dy[next_k];
		if (!valid(next_x, next_y, n, n) || seen[next_x][next_y] == 1) {
			// 方向転換
			rep(k, 4) {
				next_x = x + dx[k];
				next_y = y + dy[k];
				if (valid(next_x, next_y, n, n) && seen[next_x][next_y] == 0) {
					next_k = k;
					break;
				}
			}
		}

		return next_k;
	}

	vint get_ok_k_list(vvint& seen) {
		vint res;
		rep(k, 4) {
			int next_x = x + dx[k];
			int next_y = y + dy[k];
			if (valid(next_x, next_y, n, n) && seen[next_x][next_y] == 0) {
				res.push_back(k);
			}
		}

		return res;
	}

};


class BoardState
{

public:

	vvint h;
	Coord place;
	int amount;
	vector<pair<int, int>> actions;
	int bad_cnt;
	int cost;
	vvint seen;

	int now_k_save;

	BoardState()
	{
		init();
	}

	// 答えを出力
	void print_ans()
	{
		for (auto item : actions) {
			if (item.first == 4) {
				if (item.second >= 0) {
					cout << "+" << abs(item.second) << endl;
				}
				else {
					cout << "-" << abs(item.second) << endl;
				}
			}
			else {
				co(dir[item.first]);
			}
		}
	}

	// [実装しなくてもよいが実装すると便利] : 現在のゲーム状況を文字列にする
	void toString() const
	{

	}

	// [どのゲームでも実装する] : スコア計算をする。(toStringを実装しない場合は引数is_printとそれの不随する処理は不要)
	ScoreType getScore(bool is_print = false) const
	{

	}
	// [どのゲームでも実装する] : 終了判定
	bool is_none(int shr) {
		ll ok_cnt = 0;
		rep(i, n) {
			rep(j, n) {
				if (h[i][j] == 0) {
					ok_cnt++;
				}
			}
		}
		return ok_cnt >= shr;
	}

	// [どのゲームでも実装する] : 初期化する
	void init()
	{
		h = h0;
		place = Coord(0, 0);
		bad_cnt = 0;
		rep(i, n) {
			rep(j, n) {
				if (h[i][j] != 0) {
					bad_cnt++;
				}
			}
		}

		cost = 0;

		seen.resize(n);
		rep(i, n) {
			seen[i].resize(n);
		}

		now_k_save = 0;
	}

	// [どのゲームでも実装する] : 
	void move(int action, int d = 0, bool ignore_seen = false) {
		actions.push_back(mp(action, d));
		if (action != 4) {
			if (h[place.x][place.y] == 0 && !ignore_seen) {
				seen[place.x][place.y] = 1;
			}
			place.move(action);
			cost += (100 + abs(amount));
		}
		else {
			assert(d != 0);
			amount += d;
			h[place.x][place.y] -= d;
			if (h[place.x][place.y] == 0) {
				bad_cnt--;
			}
			if (h[place.x][place.y] != 0) {
				bad_cnt++;
			}

			cost += (abs(d));
		}
	}

	int get_now_h() {
		if (!valid(place.x, place.y, n, n)) {
			print_ans();
		}
		return h[place.x][place.y];
	}


	bool operator<(const BoardState& another) const
	{
		return cost < another.cost;//年齢を比較
	};
};


using State = BoardState;



// 0のマスがok_shrに達するまでプレイ
State Greedy(State state, int ok_shr)
{
	int now_k = state.now_k_save;
	while (!state.is_none(ok_shr)) {

		// ランダムに進む方向を決める
		if (now_k == -1) {
			auto k_list = state.place.get_ok_k_list(state.seen);
			if (k_list.size() == 0) {
				state.print_ans();
			}
			now_k = k_list[mt_for_action() % k_list.size()];
		}


		// 現在の場所が0 → 次に進む
		if (state.get_now_h() == 0) {

			int next_k = state.place.get_next_k(now_k, state.seen);

			// 方向転換が起きたときには、now_kを-1にする→次ターンではランダムに進む方向を決める
			if (now_k != next_k) {
				now_k = -1;
			}
			state.move(next_k);
			//turn--;
			continue;
		}
		// 現在の場所が正 → その分積む
		if (state.get_now_h() > 0) {
			state.move(4, state.get_now_h());
			continue;
		}
		// 現在の場所が負 
		if (state.get_now_h() < 0) {
			// 積んでいる → 必要な分おろす
			if (state.amount > 0) {
				// 必要な量 >  積んでいる量 → 積んでいる量 おろす
				if (abs(state.get_now_h()) > state.amount) {
					state.move(4, -1 * state.amount);
				}
				// 必要な量 <  積んでいる量 → 必要な量 おろす
				else {
					state.move(4, -1 * abs(state.get_now_h()));
				}
			}
			// 積んでいない → 次のマスに行って必要な分をもってくる
			else {
				int need = abs(state.get_now_h());
				int next = state.place.get_next_k(now_k, state.seen);
				vint prob;
				rep(k, 4) {
					int x = state.place.x + dx[k];
					int y = state.place.y + dy[k];
					if (valid(x, y, n, n) && state.h[x][y] != 0) {
						prob.push_back(k);
					}
				}
				if (prob.size() != 0) {
					next = prob[mt_for_action() % prob.size()];
				}

				int back = (next + 2) % 4;
				state.move(next, 0, true);
				state.move(4, need);
				state.move(back, 0, true);
				state.move(4, -1 * need);
			}
			continue;
		}

	}


	state.now_k_save = now_k;

	return state;
}

// ゲームを1回プレイしてゲーム状況を表示する
void playGame(State state)
{
	State state_res = state;
	state_res.cost = 1000000000;
	StopWatch sw;
	int try_cnt = 0;

	int look_turn = 80;
	int remain_cnt = 120;
	int try_per_remain = 120;

	vector<State> remain_state(remain_cnt, state_res);
	remain_state.push_back(state);

	rep(ii, 5) {
		vector<State> remain_state_next;
		int ok_shr = min(400, look_turn * ((int)ii + 1));

		if (ii == 4) {
			state_res = Greedy(remain_state[0], ok_shr);
			while (sw.lap() < 1980) {
				for (auto state_tmp : remain_state) {
					if (sw.lap() > 1980) {
						break;
					}
					auto state_tmp_2 = Greedy(state_tmp, ok_shr);
					if (state_res.cost > state_tmp_2.cost) {
						state_res = state_tmp_2;
					}
				}
			}
		}
		else {
			rep(jj, try_per_remain) {
				for (auto state_tmp : remain_state) {
					auto state_tmp_res = Greedy(state_tmp, ok_shr);
					remain_state_next.push_back(state_tmp_res);
				}
			}
			std::sort(remain_state_next.begin(), remain_state_next.end());//ソート実行

			rep(jj, remain_cnt) {
				remain_state[jj] = remain_state_next[jj];
			}
		}


	}

	/*state_res = remain_state[0];*/

	//while (sw.lap() < 1990) {
	//	if (state_res.cost > state_tmp.cost) {
	//		state_res = state_tmp;
	//	}
	//}
	state_res.print_ans();


	return;
}



int main() {

	cin >> n;
	h0.resize(n);
	rep(i, n) {
		h0[i].resize(n);
		rep(j, n) {
			cin >> h0[i][j];
		}
	}

	auto state = State();
	playGame(state);
}