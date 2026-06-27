#define _USE_MATH_DEFINES
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include<bits/stdc++.h>
#include<unordered_map>
#include<random>
#include <array>
#include <complex>
#include<chrono>
using namespace std;
#define LP(I,S,G) for (long long int I = (S); I < (G); ++I)
#define IN(X) 	for (int in = 0; in < X.size(); in++)cin >> X[in]
#define OUT(X) 	for (int in = 0; in < X.size(); in++)cout << X[in]<<" "
#define SORT(X) sort((X).begin(), (X).end())
#define CSORT(X,Y) sort(X.begin(), X.end(),Y)
#define COPY(X,Y) copy(X.begin(), X.end(), Y.begin())
#define ALL(X,Y) for (auto (X) :(Y))
#define FULL(a)  (a).begin(),(a).end()
#define BFS(Q,S) for(Q.push(S);Q.size()!=0;Q.pop())
typedef long long int ll;
typedef unsigned long long int ull;
long long int M = 998244353;

chrono::system_clock::time_point starttime;
using namespace std::chrono;
#ifndef ONLINE_JUDGE
#define DEBUG
#endif

inline float getTime() {
#ifdef DEBUG
	return duration_cast<milliseconds>(system_clock::now() - starttime).count() * 2.0;
#else
	return duration_cast<milliseconds>(system_clock::now() - starttime).count();
#endif
}


int dx[] = { 0,0,-1,1 }, dy[] = { -1,1,0,0 };

inline long long int xor128() {
	static long long int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
	long long int t = (x ^ (x << 11));
	x = y; y = z; z = w;
	return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

struct point {
	int x, y;
	point() {
		x = -1, y = -1;
	}
	point(int ix, int iy) {
		x = ix;
		y = iy;
	}
	point(pair<int, int> p) {
		x = p.first;
		y = p.second;
	}
};


struct Data {
	int score;
	vector<point> now_point;
	vector<vector<int>> bord;
	vector<int> rank;
	Data(int is, vector<point> ip, vector<vector<int>> ib, vector<int> ir) :score(is), now_point(ip), bord(ib), rank(ir) {}
	Data() :score(INT_MAX), now_point(), bord(), rank() {}
	array<int, 3> get_score(int id) {
		if (bord[id / 5][4] == id - 1)return { score,id / 5,4 };
		array<int, 3> result = { INT_MAX,0,0 };
		bitset<5> check = 0;
		int sx = now_point[id].x, gx = id / 5;
		int b = (4 - bord[id / 5][4]) / 2 + bord[id / 5][4] + 1;
		if (sx > gx)swap(sx, gx);
		for (int i = 0; i < 5; ++i)check[i] = (bord[i][2] != -1 || bord[i][3] != -1);
		for (int x = 0; x < 5; ++x) {
			for (int y = 1; y < 4; ++y) {
				if (bord[x][y] == -1) {
					int new_score = score + 2;
					if (x < sx)new_score += (sx - x) * 2;
					if (x > gx)new_score += (x - gx) * 2;
					if (id < b && y < 3)new_score += 1;
					if (id >= b && y == 3)new_score += 1;
					if (y == 1)new_score += 100;
					else if (y == 0)new_score += 2;
					else if ((check | bitset<5>(1 << x)).count() == 5)new_score += 100;
					if (result[0] > new_score)result = { new_score,x,y };
				}
			}
		}
		return result;
	}

	vector<pair<int, pair<point, point>>> update(int id, int tx, int ty, vector<vector<int>>& a) {
		int fx = now_point[id].x, fy = now_point[id].y;
		vector<pair<int, pair<point, point>>> sub_tasks;
		sub_tasks.push_back({ id,{{fx,fy},{tx,ty}} });
		bord[tx][ty] = id;
		if (ty == 4) {
			for (int i = id + 1; i < 5 * (id / 5 + 1); ++i) {
				if (now_point[i].y > 0) {
					bord[id / 5][4] = i;
					bord[now_point[i].x][now_point[i].y] = -1;
					sub_tasks.push_back({ i,{now_point[i],{tx,ty}} });
					now_point[i] = { i / 5,4 };
				}
				else break;
			}
		}
		if (++rank[fx] != 5) {
			bord[fx][fy] = a[fx][rank[fx]];
			now_point[a[fx][rank[fx]]] = { fx,fy };
		}
		else {
			bord[fx][fy] = -1;
		}
		now_point[id] = { tx,ty };
		return sub_tasks;
	}
};

struct Solver_GRD {
	bool err = 0;
	string move = "RLDU";
	int n, min_turn = 0;
	vector<vector<vector<pair<int, int>>>> bord;
	vector<string> ans;
	vector<vector<int>> a, item_cnt, out;
	vector<point> now_point;
	vector<int> rank, belong, require, level;
	static vector<pair<int, pair<point, point>>> tasks;
	Solver_GRD(int in, vector<vector<int>> ia) :n(in), a(ia) {
		bord.push_back(vector<vector<pair<int, int>>>(n, vector<pair<int, int>>(n, { -1,-1 })));
		item_cnt = vector<vector<int>>(n, vector<int>(n, 0));
		now_point = vector<point>(n * n, point(-1, -1));
		rank = vector<int>(n * n, 0);
		belong = vector<int>(n * n, 0);
		require = vector<int>(n, 0);
		level = vector<int>(n, 0);
		ans = vector<string>(n);
		for (int i = 0; i < n; ++i) {
			require[i] = i * 5;
			bord[0][i][0].second = i;
			bord[0][i][4].first = -(i * 5 + 1);
			for (int k = 0; k < n; ++k) {
				rank[a[i][k]] = k;
				belong[a[i][k]] = i;
				now_point[a[i][k]].x = i;
				if (k == 0) {
					now_point[a[i][k]] = { i,0 };
					bord[0][i][0].first = a[i][k];
				}
			}
		}
		if (tasks.size() == 0)make_tasks();
	}

	void make_tasks() {
		map<int, Data> dp;
		map<int, pair<int, vector<pair<int, pair<point, point>>>>> from;
		vector<vector<int>> init_bord(n, vector<int>(n));
		for (int i = 0; i < n; ++i)for (int k = 0; k < 4; ++k)init_bord[i][k] = bord[0][i][k].first;
		for (int i = 0; i < n; ++i)init_bord[i][4] = i * 5 - 1;
		dp[0] = Data(0, now_point, init_bord, { 0,0,0,0,0 });
		for (auto state : dp) {
			int now = state.first;
			for (int i = 0; i < n; ++i) {
				if (dp[now].rank[i] != n) {
					int id = dp[now].bord[i][0];
					array<int, 3> score = dp[now].get_score(id);
					if (score[0] == INT_MAX)continue;
					if (dp.find(now + (1 << id)) == dp.end() || dp[now + (1 << id)].score > score[0]) {
						dp[now + (1 << id)] = dp[now];
						dp[now + (1 << id)].score = score[0];
						vector<pair<int, pair<point, point>>> sub_tasks = dp[now + (1 << id)].update(id, score[1], score[2], a);
						from[now + (1 << id)] = { now,sub_tasks };
					}
				}
			}
		}
		int now = (1 << (n * n)) - 1;
		while (now != 0) {
			vector<pair<int, pair<point, point>>> sub_tasks = from[now].second;
			reverse(sub_tasks.begin(), sub_tasks.end());
			for (auto task : sub_tasks)tasks.push_back(task);
			now = from[now].first;
		}
		reverse(tasks.begin(), tasks.end());
	}

	bool check_local_connectivity(vector<vector<int>> t) {
		bitset<8> b = 0;
		int mx[] = { -1, 0, 1,1,1,0,-1,-1 };
		int my[] = { -1,-1,-1,0,1,1, 1, 0 };
		for (int i = 0; i < 8; ++i)b[i] = t[1 + mx[i]][1 + my[i]];
		for (int i = 0; i < 8; ++i) {
			if (b[i] == 1) {
				bitset<8> chk = 255;
				for (int k = 0; k < 8 && b[(i + k) % 8]; ++k)chk[(i + k) % 8] = 0;
				for (int k = 0; k < 8 && b[(8 + i - k) % 8]; ++k)chk[(8 + i - k) % 8] = 0;
				if ((b & chk).count())return false;
			}
		}
		return true;
	}

	void append_turn() {
		bord.push_back(*bord.rbegin());
		for (int x = 0; x < n; ++x) {
			for (int y = 0; y < n; ++y)(*bord.rbegin())[x][y].second = -1;
		}
	}

	string get_route(int rid, pair<int, pair<point, point>> task) {
		int sx, sy;
		for (int x = 0; x < n; ++x) {
			for (int y = 0; y < n; ++y)if (bord[ans[rid].size()][x][y].second == rid)sx = x, sy = y;
		}
		vector<vector<vector<vector<int>>>> visit;
		visit.push_back(vector<vector<vector<int>>>(2, vector<vector<int>>(n, vector<int>(n, 0))));
		visit[0][0][sx][sy] = 1;
		queue<array<int, 4>> q;
		q.push({ 0,0,sx,sy });
		while (q.size() != 0) {
			auto[now, mode, x, y] = q.front();
			if (now >= 10000)return "";
			q.pop();
			if (visit.size() == now + 1)visit.push_back(vector<vector<vector<int>>>(2, vector<vector<int>>(n, vector<int>(n, 0))));
			if (bord.size() == now + 1 + ans[rid].size())append_turn();
			if (visit[now + 1][mode][x][y] == 0 && bord[ans[rid].size() + now + 1][x][y].second == -1) {
				visit[now + 1][mode][x][y] = 1;
				q.push({ now + 1,mode,x,y });
			}
			if (mode == 0 && x == task.second.first.x && y == task.second.first.y && bord[ans[rid].size() + now][x][y].first == task.first) {
				if (visit[now + 1][mode + 1][x][y] == 0 && bord[ans[rid].size() + now + 1][x][y].second == -1) {
					visit[now + 1][mode + 1][x][y] = 1;
					q.push({ now + 1,mode + 1,x,y });
				}
			}
			if (mode == 1 && x == task.second.second.x && y == task.second.second.y && bord[ans[rid].size() + now + 1][x][y].second == -1) {
				if (y == n - 1 && task.first + 1 == -bord[ans[rid].size() + now][x][y].first)break;
				if (y != n - 1 && bord[ans[rid].size() + now][x][y].first < 0) {
					bool ok = 1;
					for (int i = ans[rid].size() + now + 1; i < bord.size(); ++i)if (bord[i][x][y].second != -1)ok = 0;
					if (ok)break;
				}
			}
			for (int i = 0; i < 4; ++i) {
				int nx = x + dx[i], ny = y + dy[i];
				if (nx < 0 || ny < 0 || nx >= n || ny >= n)continue;
				if (bord[ans[rid].size() + now + 1][nx][ny].second >= 0)continue;

				if (bord[ans[rid].size() + now][nx][ny].second >= 0 && bord[ans[rid].size() + now][nx][ny].second == bord[ans[rid].size() + now + 1][x][y].second)continue;
				if (mode == 1 && rid != 0 && bord[ans[rid].size() + now + 1][nx][ny].first >= 0)continue;
				if (visit[now + 1][mode][nx][ny] == 0) {
					visit[now + 1][mode][nx][ny] = 1;
					q.push({ now + 1,mode,nx,ny });
				}
			}
		}
		string res = "Q";
		int x = task.second.second.x, y = task.second.second.y, mode = 1, now = visit.size() - 2;
		if (visit[visit.size() - 2][1][x][y] == 0)return "";
		for (; now != 0; --now) {
			if (mode == 1 && x == task.second.first.x && y == task.second.first.y) {
				res += 'P';
				mode = 0;
				continue;
			}
			if (x > 1 && x < n - 1 && y>1 && y < n - 1 && visit[now - 1][mode][x][y]) {
				vector<vector<int>> t(3, vector<int>(3, 0));
				for (int i = 0; i < 3; ++i) {
					for (int k = 0; k < 3; ++k) {
						if (bord[ans[rid].size() + now - 1][x + i - 1][y + k - 1].first >= 0 || bord[ans[rid].size() + now - 1][x + i - 1][y + k - 1].second >= 0) {
							t[i][k] = 1;
						}
					}
				}
				if (check_local_connectivity(t)) {
					res += '.';
					continue;
				}
			}
			bool ok = 0;
			for (int i = 0; i < 4; ++i) {
				int nx = x + dx[i], ny = y + dy[i];
				if (nx < 0 || ny < 0 || nx >= n || ny >= n)continue;
				if (bord[ans[rid].size() + now][nx][ny].second >= 0 && bord[ans[rid].size() + now][nx][ny].second == bord[ans[rid].size() + now - 1][x][y].second)continue;
				if (visit[now - 1][mode][nx][ny]) {
					res += move[i];
					x = nx, y = ny;
					ok = 1;
					i = 4;
				}
			}
			if (ok)continue;
			res += '.';
		}
		reverse(res.begin(), res.end());
		return res;
	}

	void run_task(pair<int, string>& plan) {
		int rid = plan.first;
		int sx, sy, now = ans[rid].size(), pick;
		for (int x = 0; x < n; ++x) {
			for (int y = 0; y < n; ++y)if (bord[now][x][y].second == rid)sx = x, sy = y;
		}
		for (auto c : plan.second) {
			bord[now][sx][sy].second = rid;
			if (c == 'P') {
				pick = bord[now][sx][sy].first;
				int next = -1;
				if (sy == 0 && level[sx] + 1 != n) {
					++level[sx];
					next = a[sx][level[sx]];
					rank[next] = 0;
					now_point[next] = { sx,sy };
				}
				for (int i = now; i < bord.size(); ++i)bord[i][sx][sy].first = next;
			}
			if (c == 'Q') {
				if (sy == 4)rank[pick] = INT_MAX;
				else now_point[pick] = { sx,sy };
				for (int i = now; i < bord.size(); ++i) {
					if (sy == 4)bord[i][sx][sy].first = -pick - 2;
					else bord[i][sx][sy].first = pick;
				}
			}
			if (c == 'U')--sx;
			if (c == 'D')++sx;
			if (c == 'L')--sy;
			if (c == 'R')++sy;
			++now;
		}
		bord[now][sx][sy].second = rid;
		ans[rid] += plan.second;
	}

	virtual pair<int, string> make_task(pair<int, pair<point, point>> task) = 0;

	vector<string> solve() {
		for (auto task : tasks) {
			pair<int, string> result = make_task(task);
			run_task(result);
			min_turn = INT_MAX;
			for (int i = 0; i < n; ++i)min_turn = min(min_turn, (int)ans[i].size());
		}
		int max_len = 0;
		for (int i = 0; i < n; ++i)max_len = max(max_len, (int)ans[i].size());
		for (int i = 0; i < n; ++i)if (ans[i].size() != max_len)ans[i] += 'B';
		return ans;
	}
};

vector<pair<int, pair<point, point>>> Solver_GRD::tasks = vector<pair<int, pair<point, point>>>();

struct Solver_T1 :Solver_GRD {
	Solver_T1(int in, vector<vector<int>> ia) :Solver_GRD(in, ia) {}

	int get_score(int id, int add) {
		vector<int> t(n, 0);
		for (int i = 0; i < n; ++i) {
			t[i] = ans[i].size();
		}
		t[id] += add;
		sort(t.begin(), t.end());
		int b = 1, score = 0;
		for (int i = 0; i < n; ++i) {
			score += t[i] * b;
			b *= 2;
		}
		return score;
	}

	pair<int, string> make_task(pair<int, pair<point, point>> task) {
		pair<int, string> res = { -1,"" };
		int min_score = INT_MAX;
		for (int i = 0; i < n; ++i) {
			string route = get_route(i, task);
			if (route == "")continue;
			if (res.first == -1)res = { i,route }, min_score = get_score(i, route.size());
			else {
				int score = get_score(i, route.size());
				if (min_score > score) {
					res = { i,route };
					min_score = score;
				}
			}
		}
		return res;
	}

};

struct Solver_T2 :Solver_GRD {
	Solver_T2(int in, vector<vector<int>> ia) :Solver_GRD(in, ia) {}

	pair<int, string> make_task(pair<int, pair<point, point>> task) {
		pair<int, string> res = { -1,"" };
		for (int i = 0; i < n; ++i) {
			string route = get_route(i, task);
			if (route == "")continue;
			if (res.first == -1)res = { i,route };
			else {
				if (ans[res.first].size() + res.second.size() > ans[i].size() + route.size()) {
					res = { i,route };
				}
			}
		}
		return res;
	}

};


struct Solver_DP {
	int ans_size = INT_MAX;
	string move = "RLDU";
	int n;
	vector<vector<vector<pair<int, int>>>> bord;
	vector<string> ans;
	array<int, 5> now_step;
	array<array<int, 5>, 5> a;
	array<point, 25> now_point;
	array<int, 5> level;
	map<pair<int, array<int, 6>>, int> dp, depth;
	map<pair<int, array<int, 6>>, pair<pair<int, array<int, 6>>, pair<int, string>>> from;
	Solver_DP(int in, vector<vector<int>> ia) :n(in) {
		bord.push_back(vector<vector<pair<int, int>>>(n, vector<pair<int, int>>(n, { -1,-1 })));
		for (int i = 0; i < n; ++i)for (int k = 0; k < n; ++k)a[i][k] = ia[i][k];
		for (int i = 0; i < n; ++i) {
			now_step[i] = level[i] = 0;
			bord[0][i][0].second = i;
			bord[0][i][4].first = -(i * 5 + 1);
			for (int k = 0; k < n; ++k) {
				now_point[a[i][k]].x = i;
				if (k == 0) {
					now_point[a[i][k]] = { i,0 };
					bord[0][i][0].first = a[i][k];
				}
			}
		}
	}

	int get_score(int id, int add) {
		vector<int> t(n, 0);
		for (int i = 0; i < n; ++i) {
			t[i] = now_step[i];
		}
		t[id] += add;
		sort(t.begin(), t.end());
		int b = 1, c = 1, score = 0;
		for (int i = 0; i < n; ++i) {
			score += t[i] * b / c;
			b *= 3, c *= 2;
		}
		return score;
	}

	int get_border(int id, int score) {
		int l = 0, r = 32;
		while (r - l > 1) {
			int mid = (l + r) / 2;
			int s = get_score(id, mid);
			if (s < score)l = mid;
			else r = mid;
		}
		return l + now_step[id];
	}

	pair<int, pair<int, string>> make_task(pair<int, pair<point, point>> task, int id, int lim) {
		pair<int, string> res = { -1,"" };
		int min_score = lim;
		int border = 100;
		if (min_score != INT_MAX)border = get_border(id, min_score);
		string route = get_route(id, task, border);
		res = { id,route }, min_score = get_score(id, route.size());
		if (min_score >= lim) min_score = INT_MAX;
		if (route.size() == 0)min_score = INT_MAX;
		return { min_score,res };
	}



	bool check_local_connectivity(vector<vector<int>> t) {
		bitset<8> b = 0;
		int mx[] = { -1, 0, 1,1,1,0,-1,-1 };
		int my[] = { -1,-1,-1,0,1,1, 1, 0 };
		for (int i = 0; i < 8; ++i)b[i] = t[1 + mx[i]][1 + my[i]];
		for (int i = 0; i < 8; ++i) {
			if (b[i] == 1) {
				bitset<8> chk = 255;
				for (int k = 0; k < 8 && b[(i + k) % 8]; ++k)chk[(i + k) % 8] = 0;
				for (int k = 0; k < 8 && b[(8 + i - k) % 8]; ++k)chk[(8 + i - k) % 8] = 0;
				if ((b & chk).count())return false;
			}
		}
		return true;
	}

	void append_turn() {
		bord.push_back(*bord.rbegin());
		for (int x = 0; x < n; ++x) {
			for (int y = 0; y < n; ++y)(*bord.rbegin())[x][y].second = -1;
		}
	}

	bool check_connect(point s, point g) {
		queue<point> q;
		vector<vector<int>> v(n, vector<int>(n, 0));
		v[s.x][s.y] = 1;
		q.push(s);
		while (q.size() != 0) {
			point now = q.front();
			q.pop();
			if (now.x == g.x && now.y == g.y)return 1;
			int x = now.x, y = now.y;
			for (int i = 0; i < 4; ++i) {
				int nx = x + dx[i], ny = y + dy[i];
				if (nx < 0 || ny < 0 || nx >= n || ny >= n)continue;
				if (bord[bord.size() - 1][nx][ny].first >= 0)continue;
				if (v[nx][ny] == 0) {
					v[nx][ny] = 1;
					q.push({ nx,ny });
				}
			}
		}
		return 0;
	}

	string get_route(int rid, pair<int, pair<point, point>> task, int border) {
		static int call_cnt = 0;
		static array<array<array<array<int, 5>, 5>, 2>, 101> visit;
		++call_cnt;
		int sx, sy;
		for (int x = 0; x < n; ++x) {
			for (int y = 0; y < n; ++y)if (bord[now_step[rid]][x][y].second == rid)sx = x, sy = y;
		}
		if (rid != 0 && border - now_step[rid] >= 10) {
			if (check_connect(task.second.first, task.second.second) == 0)return "";
		}
		visit[0][0][sx][sy] = call_cnt;
		array<array<int, 4>, 10000> q;
		int front = 0, back = 0;
		q[back++] = { 0,0,sx,sy };
		int start = 0;
		while (back != front) {
			auto[now, mode, x, y] = q[front++];
			if (front == 10000)front = 0;
			start = now;
			if (now + now_step[rid] >= border)return "";
			if (bord.size() == now + 1 + now_step[rid])append_turn();
			if (visit[now + 1][mode][x][y] != call_cnt && bord[now_step[rid] + now + 1][x][y].second == -1) {
				visit[now + 1][mode][x][y] = call_cnt;
				q[back++] = { now + 1,mode,x,y };
				if (back == 10000)back = 0;
			}
			if (mode == 0 && x == task.second.first.x && y == task.second.first.y && bord[now_step[rid] + now][x][y].first == task.first) {
				if (visit[now + 1][mode + 1][x][y] != call_cnt && bord[now_step[rid] + now + 1][x][y].second == -1) {
					visit[now + 1][mode + 1][x][y] = call_cnt;
					q[back++] = { now + 1,mode + 1,x,y };
					if (back == 10000)back = 0;
				}
			}
			if (mode == 1 && x == task.second.second.x && y == task.second.second.y && bord[now_step[rid] + now + 1][x][y].second == -1) {
				if (y == n - 1 && task.first + 1 == -bord[now_step[rid] + now][x][y].first)break;
				if (y != n - 1 && bord[now_step[rid] + now][x][y].first < 0) {
					bool ok = 1;
					for (int i = now_step[rid] + now + 1; i < bord.size(); ++i)if (bord[i][x][y].second != -1)ok = 0;
					if (ok)break;
				}
			}
			for (int i = 0; i < 4; ++i) {
				int nx = x + dx[i], ny = y + dy[i];
				if (nx < 0 || ny < 0 || nx >= n || ny >= n)continue;
				if (bord[now_step[rid] + now + 1][nx][ny].second >= 0)continue;

				if (bord[now_step[rid] + now][nx][ny].second >= 0 && bord[now_step[rid] + now][nx][ny].second == bord[now_step[rid] + now + 1][x][y].second)continue;
				if (mode == 1 && rid != 0 && bord[now_step[rid] + now + 1][nx][ny].first >= 0)continue;
				if (visit[now + 1][mode][nx][ny] != call_cnt) {
					visit[now + 1][mode][nx][ny] = call_cnt;
					q[back++] = { now + 1,mode,nx,ny };
					if (back == 10000)back = 0;
				}
			}
		}
		string res = "Q";
		int x = task.second.second.x, y = task.second.second.y, mode = 1, now = start;
		if (visit[start][1][x][y] != call_cnt)return "";
		for (; now != 0; --now) {
			if (mode == 1 && x == task.second.first.x && y == task.second.first.y) {
				res += 'P';
				mode = 0;
				continue;
			}
			if (x > 1 && x < n - 1 && y>1 && y < n - 1 && (visit[now - 1][mode][x][y] == call_cnt)) {
				vector<vector<int>> t(3, vector<int>(3, 0));
				for (int i = 0; i < 3; ++i) {
					for (int k = 0; k < 3; ++k) {
						if (bord[now_step[rid] + now - 1][x + i - 1][y + k - 1].first >= 0 || bord[now_step[rid] + now - 1][x + i - 1][y + k - 1].second >= 0) {
							t[i][k] = 1;
						}
					}
				}
				if (check_local_connectivity(t)) {
					res += '.';
					continue;
				}
			}
			bool ok = 0;
			for (int i = 0; i < 4; ++i) {
				int nx = x + dx[i], ny = y + dy[i];
				if (nx < 0 || ny < 0 || nx >= n || ny >= n)continue;
				if (bord[now_step[rid] + now][nx][ny].second >= 0 && bord[now_step[rid] + now][nx][ny].second == bord[now_step[rid] + now - 1][x][y].second)continue;
				if (visit[now - 1][mode][nx][ny] == call_cnt) {
					res += move[i];
					x = nx, y = ny;
					ok = 1;
					i = 4;
				}
			}
			if (ok)continue;
			res += '.';
		}
		reverse(res.begin(), res.end());
		return res;
	}

	void run_task(pair<int, string>& plan) {
		int rid = plan.first;
		int sx, sy, now = now_step[rid], pick;
		for (int x = 0; x < n; ++x) {
			for (int y = 0; y < n; ++y)if (bord[now][x][y].second == rid)sx = x, sy = y;
		}
		for (auto c : plan.second) {
			bord[now][sx][sy].second = rid;
			if (c == 'P') {
				pick = bord[now][sx][sy].first;
				int next = -1;
				if (sy == 0) {
					++level[sx];
					if (level[sx] != n) {
						next = a[sx][level[sx]];
						now_point[next] = { sx,sy };
					}
				}
				for (int i = now; i < bord.size(); ++i)bord[i][sx][sy].first = next;
			}
			if (c == 'Q') {
				now_point[pick] = { sx,sy };
				for (int i = now; i < bord.size(); ++i) {
					if (sy == 4)bord[i][sx][sy].first = -pick - 2;
					else bord[i][sx][sy].first = pick;
				}
			}
			if (c == 'U')--sx;
			if (c == 'D')++sx;
			if (c == 'L')--sy;
			if (c == 'R')++sy;
			++now;
		}
		now_step[rid] = now;
		bord[now][sx][sy].second = rid;
	}

	void rollback_task(pair<int, string>& plan) {
		int rid = plan.first;
		int sx, sy, now = now_step[rid], pick;
		for (int x = 0; x < n; ++x) {
			for (int y = 0; y < n; ++y)if (bord[now][x][y].second == rid)sx = x, sy = y;
		}
		reverse(plan.second.begin(), plan.second.end());
		for (auto c : plan.second) {
			bord[now][sx][sy].second = -1;
			--now;
			if (c == 'Q') {
				pick = bord[now][sx][sy].first;
				int next = -1;
				if (sy == 4) {
					pick = -pick - 2;
					for (int i = now; i < bord.size(); ++i)bord[i][sx][sy].first += 1;
				}
				else {
					for (int i = now; i < bord.size(); ++i)bord[i][sx][sy].first = -1;
				}
			}
			if (c == 'P') {
				now_point[pick] = { sx,sy };
				if (sy == 0)--level[sx];
				if (sy == 0 && bord[now][sx][sy].first != -1) {
					now_point[bord[now][sx][sy].first].y = -1;
				}
				for (int i = now; i < bord.size(); ++i) {
					bord[i][sx][sy].first = pick;
				}
			}
			if (c == 'U')++sx;
			if (c == 'D')--sx;
			if (c == 'L')++sy;
			if (c == 'R')--sy;
		}
		reverse(plan.second.begin(), plan.second.end());
		now_step[rid] = now;
	}

	void move_state(pair<int, array<int, 6>> start, pair<int, array<int, 6>> goal) {
		queue<pair<int, string>> rollback;
		stack<pair<int, string>> run;
		while (depth[start] > depth[goal]) {
			rollback.push(from[start].second);
			start = from[start].first;
		}
		while (depth[goal] > depth[start]) {
			run.push(from[goal].second);
			goal = from[goal].first;
		}
		while (start != goal) {
			rollback.push(from[start].second);
			start = from[start].first;
			run.push(from[goal].second);
			goal = from[goal].first;
		}
		while (rollback.size() != 0) {
			rollback_task(rollback.front());
			rollback.pop();
		}
		while (run.size() != 0) {
			run_task(run.top());
			run.pop();
		}
	}

	point get_point(int id) {
		if (bord[bord.size() - 1][id / 5][4].first == -(id + 1))return { id / 5,4 };
		array<int, 3> result = { INT_MAX,-1,-1 };
		bitset<5> check = 0;
		int sx = now_point[id].x, gx = id / 5;
		int b = (4 + (bord[bord.size() - 1][id / 5][4].first - 1)) / 2 + -(bord[bord.size() - 1][id / 5][4].first + 1) + 1;
		if (sx > gx)swap(sx, gx);
		for (int i = 0; i < 5; ++i)check[i] = (bord[bord.size() - 1][i][2].first != -1 || bord[bord.size() - 1][i][3].first != -1);
		for (int x = 0; x < 5; ++x) {
			for (int y = 1; y < 4; ++y) {
				if (bord[bord.size() - 1][x][y].first == -1) {
					int new_score = 0;
					if (x < sx)new_score += (sx - x) * 2000;
					if (x > gx)new_score += (x - gx) * 2000;
					if (x == now_point[id].x) {
						for (int i = 1; i < y; ++i) {
							if (bord[bord.size() - 1][x][i].first >= 0) {
								new_score += 100;
								break;
							}
						}
					}
					for (int i = 0; i < n; ++i) {
						if (i == id)continue;
						if (now_point[i].y >= 1 && now_point[i].y < 4) {
							if (i < id && abs(x - id / n) + abs(4 - y) < abs(now_point[i].x - i / n) + abs(4 - now_point[i].y))new_score += 1;
							if (i > id && abs(x - id / n) + abs(4 - y) > abs(now_point[i].x - i / n) + abs(4 - now_point[i].y))new_score += 1;
						}
					}
					new_score += (3 - y) * 10;
					if (y == 1)new_score += 2000 * (n - level[now_point[id].x] - 1);
					else if ((check | bitset<5>(1 << x)).count() == 5)new_score += 100000;
					if (result[0] > new_score)result = { new_score,x,y };
				}
			}
		}
		return { result[1],result[2] };
	}

	void get_ans(pair<int, array<int, 6>> now) {
		vector<string> pre_ans(n);

		pair<int, array<int, 6>> start = { 0,{0,0,0,0,0} };
		while (now != start) {
			pair<int, string> task = from[now].second;
			reverse(task.second.begin(), task.second.end());
			pre_ans[task.first] += task.second;
			now = from[now].first;
		}
		int max_len = 0;
		for (int i = 0; i < n; ++i) {
			reverse(pre_ans[i].begin(), pre_ans[i].end());
			max_len = max(max_len, (int)pre_ans[i].size());
		}
		if (ans_size > max_len) {
			ans_size = max_len;
			ans = pre_ans;
		}
	}


	vector<string> solve() {
		int border = 1200;
		dp[{0, { 0,0,0,0,0,0 }}] = 0;
		depth[{0, { 0,0,0,0,0,0 }}] = 0;
		pair<int, array<int, 6>> past = { 0,{0,0,0,0,0,0} };
		priority_queue<pair<int, pair<int, array<int, 6>>>, vector<pair<int, pair<int, array<int, 6>>>>, greater<pair<int, pair<int, array<int, 6>>>>> pq;
		pq.push({ 0,{0,{0,0,0,0,0}} });
		vector<int> cap(n * n + 1, 0);
		while (pq.size() != 0) {
#ifndef DEBUG
			float time = getTime();
			if (time>= 2900)break;
			else if (time >= 2600)border = 400;
			else if (time >= 2300)border = 700;
			else if (time >= 2000) border = 1000;
#endif

			int s = pq.top().first;
			pair<int, array<int, 6>> now = pq.top().second;
			pq.pop();
			if (dp[now] != s)continue;
			int prog = 0;
			for (int i = 0; i < n; ++i)prog += now.second[i];
			if (cap[prog] >= border)continue;
			++cap[prog];
			move_state(past, now);
			past = now;
			int max_len = 0;
			for (int i = 0; i < n; ++i)max_len = max(now_step[i], max_len);
			if (max_len >= ans_size) {
				--cap[prog];
				continue;
			}
			for (int i = 0; i < n; ++i) {
				if (level[i] != n) {
					pair<int, array<int, 6>> next = now;
					int id = bord[bord.size() - 1][i][0].first;
					point to = get_point(id);
					if (to.x == -1)continue;
					if (to.y == 4)next.second[to.x]++;
					if (next.second[to.x] > n) {
						cout << "1";
					}
					next.first += (1 << id);
					for (int k = 0; k < n; ++k) {
						next.second[n] = k;
						if (dp.find(next) == dp.end())dp[next] = INT_MAX;
						if (dp[now] >= dp[next])continue;
						auto[score, task] = make_task({ id,{{i,0},to} }, k, dp[next]);
						if (score == INT_MAX)continue;
						if (task.second.size() + now_step[k] >= ans_size)continue;
						if (dp[next] > score) {
							dp[next] = score;
							from[next] = { past,task };
							depth[next] = depth[past] + 1;
							if (prog + (to.y == 4) == n * n)get_ans(next);
							else pq.push({ dp[next],next });
						}
					}
				}
			}
			for (int x = 0; x < n; ++x) {
				for (int y = 1; y < n - 1; ++y) {
					int id = bord[bord.size() - 1][x][y].first;
					if (id == -1)continue;
					if (bord[bord.size() - 1][id / n][4].first == -(id + 1)) {
						pair<int, array<int, 6>> next = now;
						next.second[id / n]++;
						for (int k = 0; k < n; ++k) {
							next.second[n] = k;
							if (dp.find(next) == dp.end())dp[next] = INT_MAX;
							if (dp[now] >= dp[next])continue;
							auto[score, task] = make_task({ id,{{x,y},{id / n,4}} }, k, dp[next]);
							if (task.second.size() + now_step[k] >= ans_size)continue;
							if (score == INT_MAX)continue;
							if (dp[next] > score) {
								dp[next] = score;
								from[next] = { past,task };
								depth[next] = depth[past] + 1;
								pq.push({ dp[next],next });
								if (prog + 1 == n * n)get_ans(next);
								else pq.push({ dp[next],next });
							}
						}
					}
				}
			}
		}
		cerr << dp.size() << "\n";
		for (int i = 0; i < n; ++i) {
			if (from.find({ (1 << (n * n)) - 1, { 5,5,5,5,5,i } }) == from.end())continue;
			get_ans({ (1 << (n * n)) - 1, { 5,5,5,5,5,i } });
		}
		if (ans_size == INT_MAX)return {};
		cerr << ans_size << "\n";
		for (int i = 0; i < n; ++i)if (ans[i].size() != ans_size)ans[i] += 'B';
		return ans;
	}
};


/*
手動で解いてみた結果:
90手位で解ける
=>M1とM0のトレードオフみたいなものは考えなくていい
大ロボットは基本的に小ロボットとみなしていい
爆発するまでの間に動作を割り込む形で考えると処理が楽
5台フルに動作させた方がよさそう


解の下界から得られる情報:
この問題は入力の配置が理想のときでも46手かかる
=>相対スコアが25G以下なら平均手数は92手以上(実際には92手ならもっとスコアは高くなる)
今回の方針は平均90手位になりそうだから25Gの順位を見ておくとよさそう


方針:
出口に積める荷物のうち,最も取り出しやすい(入口スタックの浅いところに存在する)荷物を出口に移動させる
それぞれの移動タスクは現状最も行動していないロボットが行う
荷物を移動させるとき,なるべく道をふさがない(出口から連結性を守る)ように移動
どうしても無理な荷物は大ロボットを使う

出口に積める荷物のうち,最も取り出しやすい(入口スタックの浅いところに存在する)荷物を出口に移動させる
=>入口スタックから特定の座標に移動させるタスクに分解

タスク順序をDAGで持とうとしたけど末端同士の状態共有が厄介
=>いっそのこと線形に持つ?
=>並列で行えるタスク群のリストで持つとよさそう
=>タスク順序を持つのではなくタスクと状態を持つ
	 +*-*-*+
...$-+*-*-*+-$-...
	 +*-*--+

タスクを動的に発行する方が都合がいい?
=>タスク群のリストだと並列実行=>同期...の繰り返しになるから同期の分損
=>動的に発行した場合衝突防止,出口移動以外の同期が不要になる

動作はするようになったが,すれ違いの判定が上手いこと行かない

1回目提出:相対:33G,2TLE
TLEに関してはタスクが発行できなくなるパターンが存在することが原因
=>これは一旦放置

すれ違い判定バグは経路復元側ですれ違いをしてないことが原因?
=>解決

2回目提出:相対:37G,2TLE
現状の1位は70手前後?

タスク発行順の改善:
rankを一般化して,自身より前に存在する荷物の優先度で重みづけ?
よくよく考えたら現時点で静的にタスクを発行することは可能
ある貪欲パターンにおけるタスク発行順の最適化?をbitDPで解けそう(O(n^3*2^(n^2)))


タスク割り当ての改善:
DP,ビームサーチ,chokudaiサーチ?

can_make_task_strongが壊れてる?
=>壊れてたのはcan_make_task

DP[取り出したアイテムの集合][それぞれのレーンの進捗]でどうにかするbitDPが強い
ただし現時点ではTLE
DP[取り出したアイテムの集合][格納したアイテム数]に緩和?

ビームサーチっぽい見た目
bitDP風ビームサーチ?なんだこれ

DP[取り出したアイテムの集合][それぞれのレーンの進捗]のほうでも同様の枝狩りを試す
スコアが改善した
枝狩りを深さごとに行っても改善せず

DP[取り出したアイテムの集合][それぞれのレーンの進捗][最後に動かしたクレーン]に変更
設置場所選択の貪欲を変更
*/

int main(int argc, char* argv[]) {
	starttime = chrono::system_clock::now();
	ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	int n;
	cin >> n;
	vector<vector<int>> a(n, vector<int>(n, 0));
	for (int i = 0; i < n; ++i)for (int k = 0; k < n; ++k)cin >> a[i][k];
	Solver_DP solve1(n, a);
	vector<string> ans = solve1.solve();
	if (ans.size() != 0)for (int i = 0; i < n; ++i)cout << ans[i] << "\n";
	else {
		Solver_T1 solve1(n, a);
		Solver_T2 solve2(n, a);
		vector<string> ans1 = solve1.solve(), ans2 = solve2.solve();
		if (solve1.err && solve2.err) {
			return 0;
		}
		if (solve1.err) {
			for (int i = 0; i < n; ++i)cout << ans2[i] << "\n";
		}
		else if (solve2.err) {
			for (int i = 0; i < n; ++i)cout << ans1[i] << "\n";
		}
		else {
			int s1 = 0, s2 = 0;
			for (int i = 0; i < n; ++i) {
				s1 = max((int)ans1[i].size(), s1);
				s2 = max((int)ans2[i].size(), s2);
			}
			if (s1 < s2) {
				for (int i = 0; i < n; ++i)cout << ans1[i] << "\n";
			}
			else {
				for (int i = 0; i < n; ++i)cout << ans2[i] << "\n";
			}
		}
	}
	exit(0);
}