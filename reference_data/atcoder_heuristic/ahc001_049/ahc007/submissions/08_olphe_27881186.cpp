#include "iostream"
#include "climits"
#include "list"
#include "queue"
#include "stack"
#include "set"
#include "functional"
#include "algorithm"
#include "string"
#include "map"
#include "unordered_map"
#include "unordered_set"
#include "iomanip"
#include "cmath"
#include "random"
#include "bitset"
#include "cstdio"
#include "numeric"
#include "cassert"
#include "ctime"

using namespace std;

/*

辺のコストはユークリッド距離に近い(1倍～3倍)
点の座標と辺の重み以外の情報が与えられる。
辺の重みが1個ずつ順番に与えられるので、その辺を使うかどうかを返す。

グラフは5個のたがいに素な最小全域木の和なので、極端に小さい次数の頂点が無い。ある辺を使わないことによる影響が小さい？
スコアは最小全域木のコスト/自分の全域木のコストに比例する。

とりあえずは全部1倍だと仮定する。(4割ぐらいなんだろうなあ)
コストの改善の期待値が0の場合は得？(除算するので)

高速化をしたい
高確率で使うor使わないやつに時間をかけてもあんま意味なさそう
序盤と終盤の関係は？


*/

class UnionFind {
	vector<int>parent;
	vector<int>rank;
	int comp;
public:
	void Init(int num) {
		comp = num;
		parent.resize(num);
		rank.resize(num);
		for (int i = 0; i < num; i++) {
			parent[i] = i;
			rank[i] = 0;
		}
	}
	int Find(int node) {
		if (parent[node] == node)return node;
		else return parent[node] = Find(parent[node]);
	}
	void Unite(int u, int v) {
		u = Find(u);
		v = Find(v);
		if (u == v)return;
		comp--;
		if (rank[u] < rank[v])parent[u] = v;
		else {
			parent[v] = u;
			if (rank[u] == rank[v])rank[u]++;
		}
	}
	bool Check_Same(int u, int v) {
		return Find(u) == Find(v);
	}
	int ComponentsNum() {
		return comp;
	}
};

class XorShift {
	unsigned int x, y, z, w, t;
public:
	XorShift() {
		x = 133553533;
		y = 314867339;
		z = 664298413;
		w = 999999937;
		t = 0;
	}
	unsigned int rand() {
		t = x ^ (x << 11);
		x = y;
		y = z;
		z = w;
		w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
		return w & 0x7fffffff;
	}
};

constexpr int N = 400, M = 1995;
XorShift xs;

struct Node {
	int x, y;
	Node() {
		cin >> x >> y;
	}
	int operator - (const Node& n)const {
		return (int)(round(hypot(x - n.x, y - n.y)) + 0.1);
	}
};

struct Edge {
	int a, b, dist, cost, idx;
	Edge() {
		cin >> a >> b;
		idx = 0;
		dist = cost = 0;
	}
	bool operator<(const Edge& edge)const {
		return cost < edge.cost;
	}
	int Rand() {
		return xs.rand() % (dist * 2 + 1) + dist;
	}
	int Expect() {
		return dist * 1.8;
	}
};

vector<Node>node;
vector<Edge>edge;
vector<int>p;
vector<int>c;
UnionFind uf;
int sum = 0;

void Initialize() {
	node.resize(N);
	edge.resize(M);
	p.resize(M);
	c.resize(M);
	uf.Init(N);
	for (int i = 0; i < M; i++) {
		edge[i].dist = node[edge[i].a] - node[edge[i].b];
		edge[i].idx = i;
		edge[i].cost = edge[i].Expect();
	}
}

bool CheckUse(int idx) {
	auto befuf = uf;
	auto befedge = edge;

	if (uf.Check_Same(edge[0].a, edge[0].b)) {
		return false;
	}

	if (c[idx] > 0.9 * p[idx]) {
		return true;
	}
	double flavor = 0;
	int add = 1;
	int ok = 0;
	int ov = 0, ls = 0;
	for (int loop = 0; loop < 60 - edge.size() / 100; loop++) {
		for (int i = 1; i < edge.size(); i++) {
			edge[i].cost = edge[i].Rand() + 2;
		}
		int dif1 = edge[0].cost;
		uf.Unite(edge[0].a, edge[0].b);
		sort(edge.begin(), edge.end());
		for (auto i : edge) {
			if (i.idx <= idx)continue;
			p[i.idx] += add;
			if (uf.Check_Same(i.a, i.b))continue;
			c[i.idx] += add;
			uf.Unite(i.a, i.b);
			dif1 += i.cost;
		}

		int dif2 = 0;
		uf = befuf;
		for (auto i : edge) {
			if (i.idx <= idx)continue;
			p[i.idx] += add;
			if (uf.Check_Same(i.a, i.b))continue;
			c[i.idx] += add;
			uf.Unite(i.a, i.b);
			dif2 += i.cost;
		}
		if (uf.ComponentsNum() > 1) {
			uf = befuf;
			edge = befedge;
			return true;
		}
		ok += dif1 <= dif2;
		flavor += (double)1000000 / (sum+dif1) - (double)1000000 / (sum+dif2);
		uf = befuf;
		edge = befedge;
		if (loop > 8 && ok == 0)return false;
		//if (flavor >= 1) {
		//	ov = 1;
		//}
		//if (flavor <= -1) {
		//	ls = 1;
		//}
		if (flavor >= 1)return true;
		if (flavor <= -1)return false;
	}
	//cerr << flavor << " " << ov << " " << ls << endl;
	bool ret = false;
	if (flavor > 0)ret = true;
	return ret;
}

void Main() {
	for (int loop = 0; loop < M; loop++) {
		cin >> edge[0].cost;
		if (CheckUse(loop)) {
			cout << 1 << endl;
			uf.Unite(edge[0].a, edge[0].b);
			sum += edge[0].cost;
		}
		else {
			cout << 0 << endl;
		}
		edge.erase(edge.begin());
	}
}

void DEBUG() {

}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);

	Initialize();
	Main();
	DEBUG();
}
