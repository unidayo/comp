#include <cstdio>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <vector>
#include <chrono>
#include <queue>
#include <tuple>
#include <set>
#include <map>
using namespace std;

long long seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
unsigned int rx = seed, ry = seed >> 12, rz = seed >> 24;
inline unsigned int rnd()
{
	unsigned int t;
	rx ^= rx << 16; rx ^= rx >> 5; rx ^= rx << 1;
	t = rx; rx = ry; ry = rz; rz = t ^ rx ^ ry;
	return t;
}

double lg[65536];

int range(int a, int b)
{
	return a + rnd() % (b - a + 1);
}

double z()
{
	return (rnd() + 0.5) / ((double)4294967296);
}

int N, sx, sy, id[77][77], px[3700], py[3700], cnt; char b[77][77];
int dist[3700][3700], grp, hor[77][77], ver[77][77];
pair<int, int> via[3700][3700];
vector<int> par[3700]; int real[3700];

int dx[4] = { 0, 1, 0, -1 };
int dy[4] = { 1, 0, -1, 0 };

string trace(vector<int> &f)
{
	string ret;
	for (int i = 1; i < f.size(); i++){
		int p = f[i - 1], q = f[i];
		string shard;
		while (p != q){
			auto[k, r] = via[p][q];
			shard += "RDLU"[k];
			q = r;
		}
		reverse(shard.begin(), shard.end());
		ret += shard;
	}
	return ret;
}

void proc()
{
	auto start = std::chrono::system_clock::now();

	scanf ("%d %d %d", &N, &sx, &sy);
	for (int i = 0; i < N; i++) scanf ("%s", b[i]);

	memset(id, -1, sizeof(id));
	for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) if (b[i][j] != '#'){
		id[i][j] = cnt;
		px[cnt] = i;
		py[cnt] = j;
		cnt++;
	}

	for (int p = 0; p < cnt; p++){
		for (int q = 0; q < cnt; q++) dist[p][q] = 0x7fffffff;

		queue<tuple<int, int, int> > que[5];
		que[0].push({ 0, px[p], py[p] });
		dist[p][p] = 0;

		while (1){
			int u = -1, mn = 0x7fffffff;
			for (int i = 0; i < 5; i++) if (!que[i].empty()){
				auto[c, x, y] = que[i].front();
				if (mn > c){
					u = i;
					mn = c;
				}
			}
			if (u == -1) break;

			auto[c, x, y] = que[u].front();
			que[u].pop();
			int r = id[x][y];
			if (dist[p][r] < c) continue;

			for (int k = 0; k < 4; k++){
				int nx = x + dx[k], ny = y + dy[k];
				if (nx < 0 || ny < 0 || nx >= N || ny >= N || b[nx][ny] == '#') continue;
				int nc = c + b[nx][ny] - '0';
				int q = id[nx][ny];
				if (dist[p][q] > nc){
					que[b[nx][ny] - '0' - 5].push({ nc, nx, ny });
					dist[p][q] = nc;
					via[p][q] = { k, r };
				}
			}
		}
	}

	memset(hor, -1, sizeof(hor));
	for (int i = 0; i < N; i += 2){
		for (int j = 0; j < N; j++){
			if (b[i][j] != '#'){
				if (j > 0 && hor[i][j - 1] >= 0) hor[i][j] = hor[i][j - 1];
				else hor[i][j] = grp++;
			}
		}
	}

	memset(ver, -1, sizeof(ver));
	for (int j = 0; j < N; j += 2){
		for (int i = 0; i < N; i++){
			if (b[i][j] != '#'){
				if (i > 0 && ver[i - 1][j] >= 0) ver[i][j] = ver[i - 1][j];
				else ver[i][j] = grp++;
			}
		}
	}

	for (int i = 0; i < N; i += 2) for (int j = 0; j < N; j += 2){
		if (hor[i][j] != -1 && ver[i][j] != -1){
			par[hor[i][j]].push_back(id[i][j]);
			par[ver[i][j]].push_back(id[i][j]);
		}
	}
	par[grp].push_back(id[sx][sy]);

	for (int i = 0; i < N; i++) for (int j = 0; j < N; j++){
		if (hor[i][j] != -1) real[hor[i][j]]++;
		if (ver[i][j] != -1) real[ver[i][j]]++;
	}

	int length = 0x7fffffff;
	vector<pair<int, int> > order, best;
	order.push_back({ grp, 0 });
	for (int g = 0; g < grp; g++){
		if (g == hor[sx][sy] || g == ver[sx][sy] || real[g] == 1);
		else order.push_back({ g, 0 });
	}
	order.push_back({ grp, 0 });

	vector<pair<int, int> > t1; int o1 = 0;
	vector<int> t2, tp[3700]; int o2 = 0, op[3700] = { 0, };
	for (int i = 1; i + 1 < order.size(); i++){
		for (int j = i + 1; j + 1 < order.size(); j++) t1.push_back({ i, j });
		t2.push_back(i);
		for (int j = 0; j < par[order[i].first].size(); j++) tp[order[i].first].push_back(j);
	}

	double TL = 2.94;
	for (int outer = 0;; outer++){
		double dur = std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
		if (dur >= TL) break;
		int step = order.size() * 500;
		double T = N * ((TL - dur) / TL + z() * 0.3);
		double rat = pow(1. / N, 1. / step);
		int v = 127 - (dur / TL) * 91;

		if (outer % 50 == 0){
			for (int i = 1; i < t1.size(); i++) swap(t1[i], t1[range(0, i)]);
			for (int i = 1; i < t2.size(); i++) swap(t2[i], t2[range(0, i)]);
			for (int i = 1; i + 1 < order.size(); i++){
				for (int j = 1; j < tp[order[i].first].size(); j++) swap(tp[order[i].first][j], tp[order[i].first][range(0, j)]);
			}
		}

		for (int att = 0; att < step; att++){
			T *= rat;
			if ((rnd() & 0x7f) < v){
				auto[i1, i2] = t1[o1++];
				if (o1 == t1.size()) o1 = 0;

				int prv = par[order[i1 - 1].first][order[i1 - 1].second];
				int u = par[order[i1].first][order[i1].second];
				int v = par[order[i2].first][order[i2].second];
				int nxt = par[order[i2 + 1].first][order[i2 + 1].second];

				int diff = (dist[u][prv] + dist[v][nxt]) - (dist[v][prv] + dist[u][nxt]);
				if (diff > T * lg[rnd() & 0xffff]){
					reverse(order.begin() + i1, order.begin() + i2 + 1);
				}
			}
			else{
				int i = t2[o2++];
				if (o2 == t2.size()) o2 = 0;
				int w = order[i].first;
				int j = tp[w][op[w]++];
				if (op[w] == tp[w].size()) op[w] = 0;
				if (order[i].second == j) continue;

				int prv = par[order[i - 1].first][order[i - 1].second];
				int u = par[w][j];
				int v = par[w][order[i].second];
				int nxt = par[order[i + 1].first][order[i + 1].second];

				int diff = (dist[prv][v] + dist[v][nxt]) - (dist[prv][u] + dist[u][nxt]);
				if (diff > T * lg[rnd() & 0xffff]){
					order[i].second = j;
				}
			}
		}

		int sum = 0;
		for (int i = 1; i < order.size(); i++){
			int p = par[order[i - 1].first][order[i - 1].second];
			int q = par[order[i].first][order[i].second];
			sum += dist[p][q];
		}
		if (length > sum){
			length = sum;
			best = order;
		}
		else{
			order = best;
		}
	}

	vector<int> fin;
	for (int i = 0; i < best.size(); i++) fin.push_back(par[best[i].first][best[i].second]);
	puts(trace(fin).c_str());
}

int main()
{
	for (int i = 0; i < 65536; i++) lg[i] = log((2 * i + 1) / (65536 * 2.));

	proc();
}