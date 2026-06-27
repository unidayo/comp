// AHC051.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#pragma GCC optimize("Ofast,fast-math,unroll-all-loops,-O3")
#include <fstream>
#include <iostream>
#include <tuple>
#include <math.h>
#include <vector>
#include <chrono>
#include <immintrin.h>
#include <array>
#include <cassert>
#include <map>
#include <cstring>
#include <cstring>    // std::memcpy
#include <bitset>
#include <algorithm>
#include <queue>

#include <set>
#define REP(i,n) for(int i=0;i<n;i++)
#define double float

using namespace std;

#include <immintrin.h> // AVX/AVX2/FMA intrinsics
#include <cstring>

// 仮定: N == 16
constexpr int SIMD_N[3] = { 8,16,24 };
constexpr int SIMD_K[10] = { 8,16,24,32,40,48,56,64,72,80 };
int S_N = 0;
int S_K = 0;

#define USE_AVX2

void zero_clear_p(float* p) {
#if defined(USE_AVX512)
	constexpr int STEP = 16;
	__m512 zero512 = _mm512_setzero_ps();
	_mm512_storeu_ps(p, zero512);
	_mm512_storeu_ps(p + 16, zero512);
#elif defined(USE_AVX2)
	constexpr int STEP = 8;
	__m256 zero256 = _mm256_setzero_ps();
	for (int k = 0; k < SIMD_N[S_N]; k += STEP) {
		_mm256_storeu_ps(p + k, zero256);
	}
#else
	for (int k = 0; k < 32; ++k) p[k] = 0.f;
#endif
}


// ヘルパー: dest += src  (length SIMD_N)
static inline void add16(float* dest, const float* src) {
#if defined(__AVX2__)
	for (int k = 0; k < SIMD_N[S_N]; k += 8) {
		__m256 d = _mm256_loadu_ps(dest + k);
		__m256 s = _mm256_loadu_ps(src + k);
		__m256 r = _mm256_add_ps(d, s);
		_mm256_storeu_ps(dest + k, r);
	}
#else
	for (int k = 0; k < SIMD_N; ++k) dest[k] += src[k];
#endif
}

// ヘルパー: dest += src * mul  (elementwise mul, length SIMD_N)
static inline void add16_scaled(float* dest, const float* src, const float* mul) {
#if defined(__AVX2__) && defined(__FMA__)
	for (int k = 0; k < SIMD_N[S_N]; k += 8) {
		__m256 d = _mm256_loadu_ps(dest + k);
		__m256 s = _mm256_loadu_ps(src + k);
		__m256 m = _mm256_loadu_ps(mul + k);
		// fused multiply-add: d + s * m
		__m256 r = _mm256_fmadd_ps(s, m, d);
		_mm256_storeu_ps(dest + k, r);
	}
#elif defined(__AVX2__)
	for (int k = 0; k < SIMD_N[S_N]; k += 8) {
		__m256 d = _mm256_loadu_ps(dest + k);
		__m256 s = _mm256_loadu_ps(src + k);
		__m256 m = _mm256_loadu_ps(mul + k);
		__m256 prod = _mm256_mul_ps(s, m);
		__m256 r = _mm256_add_ps(d, prod);
		_mm256_storeu_ps(dest + k, r);
	}
#else
	for (int k = 0; k < SIMD_N; ++k) dest[k] += src[k] * mul[k];
#endif
}


// ヘルパー: dest += src  (length SIMD_N)
static inline void add16_K(float* dest, const float* src) {
#if defined(__AVX2__)
	for (int k = 0; k < SIMD_K[S_K]; k += 8) {
		__m256 d = _mm256_loadu_ps(dest + k);
		__m256 s = _mm256_loadu_ps(src + k);
		__m256 r = _mm256_add_ps(d, s);
		_mm256_storeu_ps(dest + k, r);
	}
#else
	for (int k = 0; k < SIMD_N; ++k) dest[k] += src[k];
#endif
}

static inline void add16_scaled_K(float* dest, float src, const float* mul) {
#if defined(__AVX2__) && defined(__FMA__)
	for (int k = 0; k < SIMD_K[S_K]; k += 8) {
		__m256 d = _mm256_loadu_ps(dest + k);
		__m256 m = _mm256_loadu_ps(mul + k);
		__m256 s = _mm256_set1_ps(src);  // スカラを全レーンにコピー
		__m256 r = _mm256_fmadd_ps(s, m, d);
		_mm256_storeu_ps(dest + k, r);
	}
#elif defined(__AVX2__)
	for (int k = 0; k < SIMD_K[S_K]; k += 8) {
		__m256 d = _mm256_loadu_ps(dest + k);
		__m256 m = _mm256_loadu_ps(mul + k);
		__m256 s = _mm256_set1_ps(src);
		__m256 prod = _mm256_mul_ps(s, m);
		__m256 r = _mm256_add_ps(d, prod);
		_mm256_storeu_ps(dest + k, r);
	}
#else
	for (int k = 0; k < SIMD_N; ++k) dest[k] += src * mul[k];
#endif
}



struct Timer
{
	using T = std::chrono::steady_clock;
private:
	std::chrono::time_point<T> t;
public:
	Timer() :t(std::chrono::steady_clock::now())
	{}
	double GetSeconds()const
	{
		return std::chrono::duration_cast<std::chrono::duration<double>>(T::now() - t).count();
	}
	void Start()
	{
		t = std::chrono::steady_clock::now();
	}
};

Timer timer;

struct Xor64
{
private:
	Xor64(const Xor64& r) = delete;
	Xor64& operator =(const Xor64& r) = delete;
public:

	uint64_t x;
	inline Xor64(uint64_t seed = 0)
	{
		x = 8817264546332111ULL + seed;
	}

	inline uint64_t operator()()
	{
		x = x ^ (x << 7);
		return x = x ^ (x >> 9);
	}
	void SetSeed(int i)
	{
		x += i;
	}
};

Xor64 rnd;

struct Point {
	long long x, y;
	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}
};

// 符号関数
int sign(long long x) {
	return (x > 0) - (x < 0);
}

// 3点の向きを返す（反時計回りなら1、時計回りなら-1、一直線なら0）
int orientation(const Point& a, const Point& b, const Point& c) {
	long long cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
	return sign(cross);
}

bool on_segment(const Point& a, const Point& b, const Point& p) {
	return std::min(a.x, b.x) <= p.x && p.x <= std::max(a.x, b.x) &&
		std::min(a.y, b.y) <= p.y && p.y <= std::max(a.y, b.y);
}


// 線分p1p2とq1q2が交差するかどうかを判定
bool segments_intersect(const Point& p1, const Point& p2, const Point& q1, const Point& q2) {
	if (std::max(p1.x, p2.x) < std::min(q1.x, q2.x) ||
		std::max(q1.x, q2.x) < std::min(p1.x, p2.x) ||
		std::max(p1.y, p2.y) < std::min(q1.y, q2.y) ||
		std::max(q1.y, q2.y) < std::min(p1.y, p2.y)) {
		return false;
	}

	int o1 = orientation(p1, p2, q1);
	int o2 = orientation(p1, p2, q2);
	int o3 = orientation(q1, q2, p1);
	int o4 = orientation(q1, q2, p2);

	bool intersect = (o1 * o2 <= 0) && (o3 * o4 <= 0);

	if (!intersect) return false;

	// 端点で接しているだけなら false を返す
	if ((p1 == q1 || p1 == q2 || p2 == q1 || p2 == q2)) {
		return false;
	}

	return true;
}


#define M_PI 3.14159265358979323846

// 2点間の距離の2乗
inline double dist2(pair<int, int> a, pair<int, int> b) {
	double dx = a.first - b.first;
	double dy = a.second - b.second;
	return dx * dx + dy * dy;
}

// 2点のなす角度（0～2π）
inline double angle(pair<int, int> from, pair<int, int> to) {
	double dx = to.first - from.first;
	double dy = to.second - from.second;
	double theta = atan2(dy, dx);
	if (theta < 0) theta += 2 * M_PI;
	return theta;
}

const double TIME_LIMIT = 1.98;
const int MAX_N = 20;
const int MAX_M = MAX_N * 50;
const int MAX_K = MAX_N * 4;
const int MAX_C = 20;

int N, M, K;
int d_x[MAX_N], d_y[MAX_N];
int s_x[MAX_M], s_y[MAX_M];

int target_p[MAX_N];
int target_p_tmp[MAX_N];

//float p[MAX_K][MAX_N];
double p_l[MAX_K][MAX_N], p_r[MAX_K][MAX_N];

int close_n[MAX_M + MAX_N][MAX_C];
int close_n_cnt[MAX_M + MAX_N];
vector<int> intersect_edge[MAX_C * (MAX_M + MAX_N + 1)];
int intersect_cnt[MAX_C * (MAX_M + MAX_N + 1)];
int intersect_cnt_tmp[MAX_C * (MAX_M + MAX_N + 1)];
bool not_used[MAX_M + MAX_N + 1];

// メイン関数：pointsの各点からK方向に最近点への辺を返す
vector<pair<int, int>> findDirectionalEdges(
	const vector<pair<int, int>>& points, int K
) {
	const int N = points.size();
	vector<pair<int, int>> edges;
	const double sector = 2 * M_PI / K;

	for (int i = 0; i < N; ++i) {
		vector<pair<double, int>> closest(K, { numeric_limits<double>::max(), -1 });

		for (int j = 0; j < N; ++j) {
			if (i == j) continue;

			double ang = angle(points[i], points[j]);
			int k = static_cast<int>(ang / sector) % K;

			double d = dist2(points[i], points[j]);
			if (d < closest[k].first) {
				closest[k] = { d, j };
			}
		}

		// エッジ追加
		for (int k = 0; k < K; ++k) {
			if (closest[k].second != -1 && closest[k].second != N + M) {
				close_n[i][close_n_cnt[i]++] = closest[k].second;
				//edges.emplace_back(i, closest[k].second);
			}
		}
	}

	return edges;
}



void Input(string path)
{
	ifstream ifs(path);
	ifs >> N >> M >> K;
	REP(i, N)ifs >> d_x[i] >> d_y[i];
	REP(i, M)ifs >> s_x[i] >> s_y[i];
	REP(i, K)REP(j, N)
	{
		double pa;
		ifs >> pa;
		p_l[i][j] = pa;
		p_r[i][j] = 1.0 - pa;
		//p[i][j] = pa;
	}
}

void CInput()
{
	cin >> N >> M >> K;
	REP(i, N)cin >> d_x[i] >> d_y[i];
	REP(i, M)cin >> s_x[i] >> s_y[i];
	REP(i, K)REP(j, N)
	{
		double pa;
		cin >> pa;
		p_l[i][j] = pa;
		p_r[i][j] = 1.0 - pa;
		//p[i][j] = pa;
	}
}

double distance(int sx, int sy, int tx, int ty)
{
	return sqrt((sx - tx) * (sx - tx) + (sy - ty) * (sy - ty));
}

int edge[MAX_M + MAX_N + 1][MAX_M + MAX_N + 1];

void Init()
{
	REP(i, MAX_M + MAX_N + 1)
		REP(j, MAX_M + MAX_N + 1)
		edge[i][j] = -1;

	S_N = N / 8;
	S_K = K / 8;
	REP(i, MAX_N)target_p[i] = i;

	vector<int>tmp_x, tmp_y;
	REP(i, M)tmp_x.push_back(s_x[i]), tmp_y.push_back(s_y[i]);
	REP(i, N)tmp_x.push_back(d_x[i]), tmp_y.push_back(d_y[i]);
	tmp_x.push_back(0), tmp_y.push_back(5000);

	vector<pair<int, int>> vp;
	REP(i, M)vp.push_back({ s_x[i],s_y[i] });
	REP(i, N)vp.push_back({ d_x[i],d_y[i] });
	vp.push_back({ 0,5000 });

	findDirectionalEdges(vp, MAX_C);


	vector<tuple<int, int, int>> t[4];
	REP(i1, N + M + 1) REP(i2, close_n_cnt[i1])
	{
		int sx1 = tmp_x[i1];
		int sy1 = tmp_y[i1];
		int i3 = close_n[i1][i2];
		int sx2 = tmp_x[i3];
		int sy2 = tmp_y[i3];
		t[0].push_back({ max(sy1,sy2),i1,i2 });
		t[1].push_back({ max(sx1,sx2),i1,i2 });
		t[2].push_back({ min(sy1,sy2),i1,i2 });
		t[3].push_back({ min(sx1,sx2),i1,i2 });

		int idx1 = i1 * MAX_C + i2;
		edge[i1][i3] = idx1;
	}
	REP(i, 4)sort(t[i].begin(), t[i].end());

	int sum = 0;
	REP(i1, N + M + 1) REP(i2, close_n_cnt[i1])
	{
		int sx1 = tmp_x[i1];
		int sy1 = tmp_y[i1];
		int i3 = close_n[i1][i2];
		int sx2 = tmp_x[i3];
		int sy2 = tmp_y[i3];

		int idx1 = i1 * MAX_C + i2;

		int s1 = upper_bound(t[2].begin(), t[2].end(), make_tuple(max(sy1, sy2), 1e9, 1e9)) - t[2].begin(); s1--;
		int s2 = upper_bound(t[3].begin(), t[3].end(), make_tuple(max(sx1, sx2), 1e9, 1e9)) - t[3].begin(); s2--;
		int s3 = upper_bound(t[0].begin(), t[0].end(), make_tuple(min(sy1, sy2), 1e9, 1e9)) - t[0].begin();
		int s4 = upper_bound(t[1].begin(), t[1].end(), make_tuple(min(sx1, sx2), 1e9, 1e9)) - t[1].begin();
		vector<tuple<int, int, int, int, int>> v = { {s1,s1,0,2,-1},{s2,s2,1,3,-1},{t[0].size() - s3,s3,2,0,1},{t[0].size() - s4,s4,3,1,1} };
		sort(v.begin(), v.end());
		auto [dump, idx, a, tar, dir] = v[0];
		for (int i = idx; 0 <= i && i < t[0].size(); i += dir)
		{
			sum++;
			auto [y, j1, j2] = t[tar][i];
			int tx1 = tmp_x[j1];
			int ty1 = tmp_y[j1];

			int j3 = close_n[j1][j2];
			int tx2 = tmp_x[j3];
			int ty2 = tmp_y[j3];

			int idx2 = j1 * MAX_C + j2;

			bool f = segments_intersect({ sx1,sy1 }, { sx2,sy2 }, { tx1,ty1 }, { tx2,ty2 });
			if (f)
			{
				// 重複除去
				intersect_edge[idx1].push_back(idx2);
				intersect_edge[idx2].push_back(idx1);
			}
		}
	}
	//cerr << sum << endl;
}

struct Node
{
	vector<int> parent;
	int left = -1;
	int right = -1;
	int type = 0;

	//Float24 p_avx;
	//Float24 score_avx; // コピーできてる？

	double p[24];
	double score[24];
};

Node tree[MAX_M + MAX_N + 1];
Node treeCopy[MAX_M + MAX_N + 1];

bitset<MAX_C* (MAX_M + MAX_N + 1)> used_edges;
bitset<MAX_C* (MAX_M + MAX_N + 1)> used_edges_copy;

int used_parent[MAX_N + MAX_M + 1];
void Set_used_parent(int x)
{
	REP(i, N + M + 1)used_parent[i] = x;
}

int used_child[MAX_N + MAX_M + 1];
void Set_used_child(int x)
{
	REP(i, N + M + 1)used_child[i] = x;
}

void make_children(int idx)
{
	used_child[idx] = 1;
	if (tree[idx].left != -1 && !used_child[tree[idx].left]) make_children(tree[idx].left);
	if (tree[idx].right != -1 && !used_child[tree[idx].right]) make_children(tree[idx].right);
}

void BackUp()
{
	REP(i, MAX_N)target_p_tmp[i] = target_p[i];
	used_edges_copy = used_edges;
	REP(i, MAX_M + MAX_N + 1)
	{
		treeCopy[i].parent = tree[i].parent;
		treeCopy[i].left = tree[i].left;
		treeCopy[i].right = tree[i].right;
		treeCopy[i].type = tree[i].type;
		memcpy(treeCopy[i].p, tree[i].p, sizeof(tree[i].p));
		memcpy(treeCopy[i].score, tree[i].score, sizeof(tree[i].score));
	}
	REP(i, MAX_C * (N + M + 1))
		intersect_cnt_tmp[i] = intersect_cnt[i];
}

void RollBack()
{
	REP(i, MAX_N)target_p[i] = target_p_tmp[i];
	used_edges = used_edges_copy;
	REP(i, MAX_M + MAX_N + 1)
	{
		tree[i].parent = treeCopy[i].parent;
		tree[i].left = treeCopy[i].left;
		tree[i].right = treeCopy[i].right;
		tree[i].type = treeCopy[i].type;
		memcpy(tree[i].p, treeCopy[i].p, sizeof(treeCopy[i].p));
		memcpy(tree[i].score, treeCopy[i].score, sizeof(treeCopy[i].score));
	}
	REP(i, MAX_C * (N + M + 1))
		intersect_cnt[i] = intersect_cnt_tmp[i];
}

int GetIndex(int i)
{
	if (i < M)return N + i;
	return i - M;
}

bitset<MAX_M + MAX_N + 1> b_used;
bool is_ancestor(int cur, int anc)
{
	if (cur == -1)return false;
	if (cur == anc) return true;
	for (auto e : tree[cur].parent)
	{
		if (b_used.test(e)) continue;
		b_used.set(e);
		if (is_ancestor(e, anc)) return true;
	}
	return false;
}
bool make_ancestor(int cur, bitset<MAX_M + MAX_N + 1>& used)
{
	if (cur == -1)return false;
	used.set(cur);
	for (auto e : tree[cur].parent)
	{
		if (used.test(e)) continue;
		make_ancestor(e, used);
	}
	return false;
}

void deleteValue(vector<int>& v, int x)
{
	v.erase(std::remove(v.begin(), v.end(), x), v.end());
}

long long count_set[MAX_M + MAX_N + 1];
long long count_set_idx;
int K_parent(vector<int>& v)
{
	int cnt = 0;
	count_set_idx++;
	for (auto e : v)
	{
		if (count_set[e] != count_set_idx) cnt++;
		count_set[e] = count_set_idx;
	}
	return cnt;
}

void add_intersect(int idx1)
{
	for (auto e : intersect_edge[idx1])
	{
		intersect_cnt[e]++;
	}
}

void sub_intersect(int idx1)
{
	for (auto e : intersect_edge[idx1])
	{
		intersect_cnt[e]--;
	}
}

void deleteEdge(int a, int b)
{
	if (edge[a][b] != -1) sub_intersect(edge[a][b]);
	if (edge[b][a] != -1) sub_intersect(edge[b][a]);
}

void addEdge(int a, int b)
{
	if (edge[a][b] != -1) add_intersect(edge[a][b]);
	if (edge[b][a] != -1) add_intersect(edge[b][a]);
}

bool Connect(int s)
{
	vector<int>used(N + M + 1, -1);
	vector<int>prev(N + M + 1, -1);
	queue<int>q;
	q.push(s);
	used[s] = 1;
	while (!q.empty())
	{
		auto cur = q.front(); q.pop();
		if (tree[cur].left != -1 && tree[cur].left == tree[cur].right)
		{
			break;
		}
		REP(i, close_n_cnt[cur])
		{
			int nex = close_n[cur][i];
			bool ok = false;
			REP(j, MAX_C) if (j < close_n_cnt[nex]) if (close_n[nex][j] == cur)ok = true;
			if (!ok)continue;
			if (used[nex] != -1) continue;
			if (intersect_cnt[cur * MAX_C + i]) continue;
			if (nex == N + M && tree[nex].left != -1) continue;
			if (tree[nex].left != -1 && tree[nex].left != tree[nex].right)continue;
			if (M <= nex && nex < N + M) continue;
			if (is_ancestor(nex, cur))continue;
			prev[nex] = cur;
			used[nex] = i;
			q.push(nex);
		}
	}
	int t = N + M;
	REP(i, M)
	{
		if (tree[i].parent.size() && tree[i].left == tree[i].right)
		{
			if (used[i] != -1)
				t = i;
		}
	}
	if (used[t] == -1) return -1;

	while (t != s)
	{
		int nex = prev[t];
		tree[nex].parent.push_back(t);
		if (tree[t].left == -1)
		{
			tree[t].left = nex;
			tree[t].right = nex;
		}
		else
		{
			tree[t].right = nex;
		}
		addEdge(nex, t);
		t = nex;
	}

	return 0;
}


void Output()
{
	REP(i, N) cout << target_p[tree[i + M].type] << ' '; cout << "\n";
	cout << GetIndex(tree[N + M].left) << "\n";
	REP(i, M)
	{
		if (tree[i].left == -1)cout << -1 << "\n";
		else
		{
			cout << (tree[i].type) << ' ' << GetIndex(tree[i].left) << ' ' << GetIndex(tree[i].right) << "\n";
		}
	}
	cout << endl;

}

void NodeInit()
{
	REP(i, N) tree[i + M].type = i;
	REP(i, N) tree[N + M].p[i] = 1;
	REP(i, N + M + 1)
	{
		tree[i].left = -1;
		tree[i].right = -1;
		tree[i].parent.clear();
	}
}

int cnt[MAX_N + MAX_M + 1];

int que[MAX_M + MAX_N + 1];
int q_front = 0;
int q_back = 0;
int qsz = MAX_M + MAX_N + 1;

double p_l2[MAX_K][MAX_N];
double p_r2[MAX_K][MAX_N];
double p_l3[MAX_N][MAX_K];
double p_r3[MAX_N][MAX_K];

void SetP()
{
	REP(i, K)
	{
		REP(j, N)
		{
			p_l2[i][j] = p_l[i][target_p[j]];
			p_r2[i][j] = p_r[i][target_p[j]];
			p_l3[j][i] = p_l[i][target_p[j]];
			p_r3[j][i] = p_r[i][target_p[j]];
		}
	}
}

/*

void BackProp()
{
	REP(i, N + M)zero_clear_p(tree[i].score);
	REP(i, N) tree[i + M].score[i] = 1.0;
	memset(cnt, 0, sizeof(cnt));
	REP(i, N + M + 1)for (auto e : tree[i].parent) cnt[e]++;

	{
		q_front = 0;
		q_back = 0;
		REP(i, N)que[(q_back++) % qsz] = i + M;
		while (q_front != q_back)
		{
			int x = que[(q_front++) % qsz];
			for (auto e : tree[x].parent)
			{
				int e2 = tree[e].type;
				if (tree[e].left == tree[e].right)
				{
					add16(tree[e].score, tree[x].score);
				}
				else
				{
					if (tree[e].left == x)
					{
						add16_scaled(tree[e].score, tree[x].score, p_l2[e2]);
					}
					if (tree[e].right == x)
					{
						add16_scaled(tree[e].score, tree[x].score, p_r2[e2]);
					}
				}

				cnt[e]--;
				if (cnt[e] == 0) que[(q_back++) % qsz] = e;
			}
		}
	}
}
*/


long long updateA[MAX_M + MAX_N + 1];
long long updateC[MAX_M + MAX_N + 1];
long long updateB[MAX_M + MAX_N + 1];
long long uc;
long long ua, ub;

void BackProp(int prev_idx = -1)
{
	REP(i, N + M)
	{
		if (used_parent[i])
		{
			zero_clear_p(tree[i].score);
		}
	}
	REP(i, N) tree[i + M].score[i] = 1.0;
	memset(cnt, 0, sizeof(cnt));
	REP(i, N + M + 1)if (used_parent[i])for (auto e : tree[i].parent)if (used_parent[e]) cnt[e]++;

	{
		q_front = 0;
		q_back = 0;
		//if (q_front == q_back)cerr << "?" << endl;
		if (prev_idx == -1)REP(i, N) que[(q_back++) % qsz] = i + M;
		else que[(q_back++) % qsz] = prev_idx;
		while (q_front != q_back)
		{
			int x = que[(q_front++) % qsz];
			updateA[x] = ua;

			int e2 = tree[x].type;
			if (tree[x].left != -1)
			{
				if (tree[x].left == tree[x].right)
				{
					add16(tree[x].score, tree[tree[x].left].score);
				}
				else
				{
					add16_scaled(tree[x].score, tree[tree[x].left].score, p_l2[e2]);
					add16_scaled(tree[x].score, tree[tree[x].right].score, p_r2[e2]);
				}
			}

			for (auto e : tree[x].parent) if (used_parent[e])
			{
				cnt[e]--;
				if (cnt[e] == 0) que[(q_back++) % qsz] = e;
			}
		}
	}
	//cerr << tree[N + M].score[0] << endl;
}

int id = 0;
bitset<MAX_M + MAX_N + 1> ancestor[MAX_M + MAX_N + 1];
double score(set<int> prev_st = set<int>())
{
	memset(cnt, 0, sizeof(cnt));
	REP(i, N + M + 1)if (used_child[i])
	{
		ancestor[i].reset();
		zero_clear_p(tree[i].p);
		if (tree[i].left != -1)
		{
			if (used_child[tree[i].left]) {
				cnt[tree[i].left]++;
			}
			if (tree[i].left != tree[i].right &&
				used_child[tree[i].right])
			{
				cnt[tree[i].right]++;
			}
		}
	}
	REP(j, N) tree[N + M].p[j] = 1.0;

	q_front = 0;
	q_back = 0;
	if (prev_st.size())
	{
		for (auto e : prev_st)
			if (cnt[e] == 0) que[(q_back++)] = e;
	}
	else que[(q_back++)] = N + M;

	while (q_front != q_back)
	{
		int cur = que[(q_front++)];
		updateB[cur] = ua;
		ancestor[cur].set(cur);

		for (auto e : tree[cur].parent)
		{
			if (tree[e].left == tree[e].right)
			{
				add16(tree[cur].p, tree[e].p);
			}
			else
			{
				int e2 = tree[e].type;
				if (tree[e].left == cur)
				{
					add16_scaled(tree[cur].p, tree[e].p, p_l2[e2]);
				}
				if (tree[e].right == cur)
				{
					add16_scaled(tree[cur].p, tree[e].p, p_r2[e2]);
				}
			}
			ancestor[cur] |= ancestor[e];
		}

		if (tree[cur].left == -1)
		{
			continue;
		}
		if (tree[cur].left == tree[cur].right)
		{
			//if (used_child[tree[cur].left])
			{
				cnt[tree[cur].left]--;
				if (cnt[tree[cur].left] == 0)que[(q_back++)] = tree[cur].left;
			}
		}
		else
		{
			//int up = 0;
			//if (used_child[tree[cur].left])
			{
				cnt[tree[cur].left]--;
				if (cnt[tree[cur].left] == 0)que[(q_back++)] = tree[cur].left;
			}
			//if (used_child[tree[cur].right])
			{
				cnt[tree[cur].right]--;
				if (cnt[tree[cur].right] == 0)que[(q_back++)] = tree[cur].right;
			}
		}
	}
	double sum = 0;
	REP(i, N)sum += tree[i + M].p[i];

	return (N - sum) / N * 10000000;
}

void addNode()
{
	int idx = rnd() % M;
	if (tree[idx].parent.size())return;
	set<int>st;
	REP(i, close_n_cnt[idx])
	{
		if (!intersect_cnt[idx * MAX_C + i])
		{
			if (tree[close_n[idx][i]].parent.size())
				st.insert(close_n[idx][i]);
		}
	}

	for (auto e : st)
	{
		if (tree[e].parent.size())
		{
			for (auto par : tree[e].parent)
				if (st.count(par))
				{
					for (auto e2 : st)
					{
						b_used.reset();
						if (e == e2)
						{

							if (e == e2) deleteValue(tree[e].parent, par);
							deleteEdge(e, par);
							//if (tree[par].left == tree[par].right && e == e2)deleteEdge(e, par);
							addEdge(idx, e);
							//addEdge(idx, e2);
							addEdge(idx, par);
							//if (tree[par].left == tree[par].right && e == e2) addEdge(idx, par);

							if (tree[par].left == e) tree[par].left = idx;
							if (tree[par].right == e) tree[par].right = idx;
							tree[idx].parent.push_back(par);
							tree[idx].left = e;
							tree[idx].right = e2;
							tree[e2].parent.push_back(idx);
							tree[idx].type = rnd() % K;

							goto L;
						}
					}
				}
		}
	}
L:;
}

double row_a[MAX_N];
double row_left[MAX_C][MAX_K];
double row_right[MAX_C][MAX_K];

int close_used[MAX_C];
int close_idx = 0;

void Kick()
{
	double temp;
	int kick_cnt = 0;
	double prev_score = 1e9;
	int minus_flag = 0;
	vector<int>v;
	REP(i, M) v.push_back(i);
	bitset<MAX_M + MAX_N + 1> easy_ancestor;
	SetP();
	Set_used_parent(1);
	Set_used_child(1);
	int prev_backprop = -1;
	set<int> prev_child; int yy = 0, zz = 0;
	long long uu = 0;

	double start_temp = 0.02;      // 初期温度
	double end_temp = 0.001;    // 最終温度
	double now_time = timer.GetSeconds();
	double progress = now_time / TIME_LIMIT; // 0.0～1.0

	// 温度スケジュール（指数 or 線形減衰）
	double T = start_temp * pow(end_temp / start_temp, progress);

	double delta;// = cur - prev_score; // スコア差（良ければ正）

	while (timer.GetSeconds() <= TIME_LIMIT)
	{
		double cur = -114514;
		ua++;
		if (!minus_flag)
		{
			BackProp(prev_backprop);
			cur = score(prev_child);

		}
		REP(i, M)
		{

			if (updateC[v[i]] == updateA[v[i]] && updateC[v[i]] == updateB[v[i]])
			{
				//\cerr << 1;
				zz++;
				continue;
			}
			yy++;
			updateC[v[i]] = ua;
			if (i % 30 == 0 && timer.GetSeconds() > TIME_LIMIT)break;
			int idx = v[i];
			if (tree[idx].parent.size() == 0)continue;
			if (tree[idx].left == -1)continue;

			bool a = tree[tree[idx].left].parent.size() == 1;
			bool b = tree[tree[idx].right].parent.size() == 1;
			if (a && b && tree[idx].right != tree[idx].left)continue;

			int fix_id = (!a && !b) ? -1 : (a) ? tree[idx].left : tree[idx].right;


			double cur_score = 0, cur_score2 = 0;
			REP(k, N)
			{
				cur_score += tree[idx].score[k] * tree[idx].p[k];
			}
			if (minus_flag)
			{
				easy_ancestor.reset();
				make_ancestor(idx, easy_ancestor);
			}
			close_idx = 0;
			REP(j, close_n_cnt[idx])
			{
				int child = close_n[idx][j];
				if (!minus_flag)
				{
					if (ancestor[idx].test(child))continue;
				}
				else
				{
					if (easy_ancestor.test(child)) continue;
				}
				if (tree[child].parent.size() == 0)continue;
				bool is_child = tree[idx].left == child || tree[idx].right == child;

				if (!is_child && intersect_cnt[idx * MAX_C + j]) continue;

				close_used[close_idx++] = j;


				if (!minus_flag)
				{
					REP(k, K)
					{
						row_left[j][k] = 0;
						row_right[j][k] = 0;
					}
					REP(a, N)//REP(b, N)
					{
						double x = tree[child].score[a] * tree[idx].p[a];
						add16_scaled_K(row_left[j], x, p_l3[a]);
						add16_scaled_K(row_right[j], x, p_r3[a]);
					}
				}
			}

			//1個固定 or 2個固定 or 0個固定

			double best = minus_flag ? 0 : 0;
			int best_k = -1;
			int best_l = -1;
			int best_r = -1;
			int kp = rnd() % K;
			REP(k, K)if (!minus_flag || k == kp)
			{
				double max_left = minus_flag ? -1 : -1;
				double max_right = minus_flag ? -1 : -1;
				int id_l = -1;
				int id_r = -1;

				double max_left_1 = minus_flag ? -1 : -1;
				double max_right_1 = minus_flag ? -1 : -1;
				int id_l_1 = -1;
				int id_r_1 = -1;

				REP(x, close_idx)
				{
					int j = close_used[x];

					double nex_score_l = row_left[j][k];
					double nex_score_r = row_right[j][k];
					if (minus_flag)
					{
						nex_score_l = rnd() % 100000;
						nex_score_r = rnd() % 100000;
					}
					if (max_left < nex_score_l) {
						max_left = nex_score_l;
						id_l = close_n[idx][j];
					}
					if (max_right < nex_score_r) {
						max_right = nex_score_r;
						id_r = close_n[idx][j];
					}
					if (fix_id == close_n[idx][j])
					{
						if (max_left_1 < nex_score_l) {
							max_left_1 = nex_score_l;
							id_l_1 = close_n[idx][j];
						}
						if (max_right_1 < nex_score_r) {
							max_right_1 = nex_score_r;
							id_r_1 = close_n[idx][j];
						}
					}
				}
				if (fix_id == -1)
				{
					if (best < max_left + max_right)
					{
						best = max_left + max_right;  // parentが自分しかない場合は付け替えダメ
						best_l = id_l;
						best_r = id_r;
						best_k = k;
					}
				}
				else
				{
					if (best < max_left + max_right_1)
					{
						best = max_left + max_right_1;  // parentが自分しかない場合は付け替えダメ
						best_l = id_l;
						best_r = id_r_1;
						best_k = k;
					}
					if (best < max_left_1 + max_right)
					{
						best = max_left_1 + max_right;  // parentが自分しかない場合は付け替えダメ
						best_l = id_l_1;
						best_r = id_r;
						best_k = k;
					}
				}
			}
			if (best_l == -1 || best_r == -1) continue;
			if (best - 1e-5 > cur_score)
			{
				if (!minus_flag)
				{
					Set_used_parent(0);
					REP(i, N + M + 1)if (ancestor[idx].test(i))
						used_parent[i] = 1;
					prev_backprop = idx;


					Set_used_child(0);
					prev_child.clear();
					make_children(idx);
					prev_child.insert(idx);
					prev_child.insert(tree[idx].left);
					prev_child.insert(tree[idx].right);
					id = idx;
				}
				deleteValue(tree[tree[idx].left].parent, idx);
				if (tree[idx].left != tree[idx].right) deleteValue(tree[tree[idx].right].parent, idx);
				tree[best_l].parent.push_back(idx);
				if (best_l != best_r) tree[best_r].parent.push_back(idx);

				deleteEdge(idx, tree[idx].left);
				if (tree[idx].left != tree[idx].right) deleteEdge(idx, tree[idx].right);
				addEdge(idx, best_l);
				if (best_l != best_r) addEdge(idx, best_r);

				tree[idx].type = best_k;
				tree[idx].left = best_l;
				tree[idx].right = best_r;

				if (!minus_flag)
				{
					make_children(idx);
				}
				REP(j, i + 1)swap(v[j], v[rnd() % M]);

				if (timer.GetSeconds() <= 1.9 && i >= M / 2)break;
				else goto L;
			}
		}
		uu++;
		kick_cnt++;
		Set_used_child(1);
		prev_child.clear();
		cur = score();

		now_time = timer.GetSeconds();
		progress = now_time / TIME_LIMIT; // 0.0～1.0

		// 温度スケジュール（指数 or 線形減衰）
		T = start_temp + (end_temp - start_temp) * progress;

		delta = prev_score - cur; // スコア差（良ければ正）

		if (delta >= 0 || (exp(delta / T) > (double)rand() / RAND_MAX)) {
			//cerr << cur << ' ' << prev_score << endl;
			//cerr << cur << ' ' << timer.GetSeconds() <<' '<<temp<< endl;
			prev_score = cur;
			BackUp();
		}
		else
		{
			RollBack();
		}
		if (timer.GetSeconds() <= 1)
		{
			REP(i, N * (1.0 - timer.GetSeconds() / 1) / 2)
			{
				swap(target_p[rnd() % N], target_p[rnd() % N]);
			}
			minus_flag = M * (0.5 + (1.0 - timer.GetSeconds() / 1));
		}
		else minus_flag = M / (5 + 15 * (timer.GetSeconds() - 1));
		SetP();
		Set_used_parent(1);
		Set_used_child(1);
		prev_backprop = -1;
		prev_child.clear();
	L:;
		minus_flag = max(minus_flag - 1, 0);
	}
	Set_used_child(1);
	prev_child.clear();
	if (prev_score < score()) RollBack();

	//cerr << yy << ' ' << zz << endl;
	//cerr << "KICK回数: " << kick_cnt << endl;
}

void intersection_init()
{
	REP(i, MAX_C * (N + M + 1))intersect_cnt[i] = 0;
	REP(i, N + M + 1)
	{
		if (tree[i].left != -1) addEdge(i, tree[i].left);
		if (tree[i].right != -1 && tree[i].right != tree[i].left) addEdge(i, tree[i].right);
	}
}

bool IsAll()
{
	bool f = true;
	REP(i, N + M + 1)
		if (tree[i].parent.size() == 0 && tree[i].left == -1 && tree[i].right == -1)
		{
			not_used[i] = true;
			f = false;
		}
	return f;
}

double Solve()
{
	int c = 0;
	bool ng = true;
	int mis = 0, mid = 0;
	while (ng)
	{
		ng = false;
		intersection_init();
		NodeInit();
		vector<pair<double, int>> v;
		if (c++ == 0)REP(i, N)v.push_back({ -distance(d_x[i],d_y[i],0,5000),i });
		else REP(i, N)v.push_back({ rnd() % 10000,i });
		sort(v.begin(), v.end());
		Connect(v[0].second + M);
		REP(i, N - 1)
		{
			Connect(v[i + 1].second + M);
		}
		REP(i, 10000)addNode();
		mis = 0;
		mid = 0;
		REP(i, N + M)
		{
			if (tree[i].parent.size() == 0)mis++;
			if (tree[i].parent.size() == 0 && i >= M)mid++;
		}
		if (mis > M / 100 || mid) ng = true;
	}
	intersection_init();
	Kick();
	Output();
	Set_used_child(1);
	cerr << score() << endl;
	return score();
	//cerr << score() << endl;
}

void Reset()
{
	REP(i, MAX_M + MAX_N + 1)
	{
		tree[i].parent.clear();
		tree[i].left = -1;
		tree[i].right = -1;
		close_n_cnt[i] = 0;
		not_used[i] = false;
		REP(j, MAX_C)
		{
			intersect_cnt[i * MAX_C + j] = 0;
			intersect_edge[i * MAX_C + j].clear();
		}
	}
}

double Test(string path)
{
	timer.Start();
	Input(path);
	Init();
	double score = Solve();
	return score;
}

void Test10()
{
	for (int i = 0; i < 30; i++)
	{
		REP(j, 2)
		{
			string num = to_string(i);
			while (num.size() < 4) num = "0" + num;
			string path = "./in/" + num + ".txt";

			Reset();
			double score = Test(path);
		}
	}
}

int Submit()
{
	//	Test10();
	timer.Start();
	//	Input("./in/0001.txt");
	CInput();
	Init();
	//cerr << "Init: " << timer.GetSeconds() << "ms" << endl;
	Solve();
	return 0;
}





void SystemTest()
{
	int cnt[8][8] = {};
	double sco[8][8] = {};
	double sum = 0;
	double c = 0;
	for (int i = 0; i < 2000; i++)
	{
		string num = to_string(i);
		while (num.size() < 4) num = "0" + num;
		string path = "./in/" + num + ".txt";

		Reset();
		double score = Test(path);

		if (score >= 4e6) cerr << "Error? :" << path << endl;

		c += 1;
		sum += score;

		ifstream ifs(path);
		ifs >> N >> M >> K;
		int a = (N - 5) / 2;
		int b = (M - 10 * N) / N / 5;
		//cerr << a << ' ' << b << ' ' << score << endl;
		cnt[a][b]++;
		sco[a][b] += score;

		if (i % 100 == 99)
		{
			cerr << i << endl;
			REP(i, 8)
			{
				REP(j, 8)cerr << (long long)(sco[i][j] / cnt[i][j]) << '\t';
				cerr << endl;
			}
			cerr << sum / c << endl;
		}
	}
	exit(0);
}

int main()
{
	//SystemTest();
	Submit();
}
