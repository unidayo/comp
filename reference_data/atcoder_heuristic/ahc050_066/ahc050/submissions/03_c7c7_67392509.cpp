#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <time.h> 
#include <stdlib.h> 
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <fstream>
#include <cassert>
#include <stack>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <set>
#include <map>
#include <numeric> 
#include <bitset>
#include <queue>
#include <limits>
#include <iostream>
#define Y first
#define X second
using namespace std;
typedef pair<int, int>Pii;

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
#define REP(i,n) for(int i=0;i<n;i++)

Xor64 rnd;


int N;
int M;
struct Input
{
	bitset<40 * 40>bit;
	vector<vector<double>> prob;
	Input() : prob(40, std::vector<double>(40, 0.0)) {}
};
Input in;
double probTmp[40][40];
const int dy[4] = { -1,0,1,0 };
const int dx[4] = { 0,1,0,-1 };



int mas[40][40][4];

void Step(Input& in)
{
	REP(i, 40)REP(j, 40) if (!in.bit.test(i * 40 + j))
	{
		probTmp[i][j] = 0;
		REP(k, 4)
		{
			int y = i + dy[k];
			int x = j + dx[k];
			int ny = i;
			int nx = j;
			if (y < 0 || x < 0 || N <= y || N <= x || in.bit.test(y * 40 + x))
			{
				while (1)
				{
					mas[ny][nx][(k + 2) % 4] = i * 40 + j;
					ny += dy[(k + 2) % 4];
					nx += dx[(k + 2) % 4];
					if (ny < 0 || nx < 0 || N <= ny || N <= nx || in.bit.test(ny * 40 + nx))
					{
						break;
					}
				}
			}
		}
	}
	REP(i, 40)REP(j, 40)if (!in.bit.test(i * 40 + j))REP(k, 4)
	{
		int y = mas[i][j][k] / 40;
		int x = mas[i][j][k] % 40;
		probTmp[y][x] += in.prob[i][j] / 4;
	}
	REP(i, 40)REP(j, 40) in.prob[i][j] = probTmp[i][j];
}

double StepN(Input& in, int n, int turn) // コピーできてる？
{
	REP(i, 40)REP(j, 40) if (!in.bit.test(i * 40 + j))
	{
		REP(k, 4)
		{
			int y = i + dy[k];
			int x = j + dx[k];
			int ny = i;
			int nx = j;
			if (y < 0 || x < 0 || N <= y || N <= x || in.bit.test(y * 40 + x))
			{
				while (1)
				{
					mas[ny][nx][(k + 2) % 4] = i * 40 + j;
					ny += dy[(k + 2) % 4];
					nx += dx[(k + 2) % 4];
					if (ny < 0 || nx < 0 || N <= ny || N <= nx || in.bit.test(ny * 40 + nx))
					{
						break;
					}
				}
			}
		}
	}
	for (int p = 0; p < n; p++)
	{
		REP(i, 40)REP(j, 40) probTmp[i][j] = 0;
		REP(i, 40)REP(j, 40)if (!in.bit.test(i * 40 + j)) if (in.prob[i][j] > 1e-5)REP(k, 4)
		{
			int y = mas[i][j][k] / 40;
			int x = mas[i][j][k] % 40;
			probTmp[y][x] += in.prob[i][j] / 4;
		}
		REP(i, 40)REP(j, 40) in.prob[i][j] = probTmp[i][j];
	}

	double s = 0;
	REP(i, 40)REP(j, 40)
	{
		s += pow(in.prob[i][j], 5);
	}

	return s;
}

long long Score(vector<int>& order, long long best)
{
	double e = 0, esum = 0;
	Input in2 = in;
	bitset<40 * 40>b = in.bit;
	for (int k = 0; k < order.size(); k++)
	{
		Step(in2);
		int y = order[k] / 40;
		int x = order[k] % 40;
		in2.bit.set(y * 40 + x);
		esum += in2.prob[y][x];
		e += 1.0 - esum;
		//cerr << k << ' ' << in2.prob[y][x] << endl;
		in2.prob[y][x] = 0.0;
	}
	//cerr << e << endl;
	return (1000000 * e) / (N * N - M - 1);
}

long long Score(bitset<40 * 40>& in, int y, int x)
{
	REP(i, 40)REP(j, 40) if (!in.test(i * 40 + j))
	{
		probTmp[i][j] = 0;
		REP(k, 4)
		{
			int y = i + dy[k];
			int x = j + dx[k];
			int ny = i;
			int nx = j;
			if (y < 0 || x < 0 || N <= y || N <= x || in.test(y * 40 + x))
			{
				while (1)
				{
					mas[ny][nx][(k + 2) % 4] = i * 40 + j;
					ny += dy[(k + 2) % 4];
					nx += dx[(k + 2) % 4];
					if (ny < 0 || nx < 0 || N <= ny || N <= nx || in.test(ny * 40 + nx))
					{
						break;
					}
				}
			}
		}
	}
	REP(i, 40)REP(j, 40)if (!in.test(i * 40 + j))REP(k, 4)
	{
		int y = mas[i][j][k] / 40;
		int x = mas[i][j][k] % 40;
		probTmp[y][x] += 1.0 / 4;
	}
	return probTmp[y][x] * (N * N - in.count());
}

void Output(vector<int> order)
{
	for (int i = 0; i < order.size(); i++)
	{
		cout << order[i] / 40 << ' ' << order[i] % 40 << endl;
	}
}


vector<int> Greedy()
{
	vector<int>order;
	Input in2 = in;
	for (int k = 0; k < N * N - M; k++)
	{
		Step(in2);
		double minP = -1e9;
		int y = -1, x = -1;
		vector<pair<double, tuple<int, int, int>>>vd;
		REP(i, 40)REP(j, 40)if (!in2.bit.test(i * 40 + j))
		{
			vd.push_back({ in2.prob[i][j] < 1e-10 ? 0 : in2.prob[i][j],{rnd() % 100000, i,j} });
		}
		sort(vd.begin(), vd.end());

		for (int a2 = 0; a2 < max(1, min(20, (int)vd.size())); a2++)
		{
			if (a2 && abs(vd[a2].first - vd[a2 - 1].first) > 1e-5) break;
			auto [t1, t2] = vd[a2];
			auto [t3, i, j] = t2;
			Input in3 = in2;
			in3.bit.set(i * 40 + j);
			double p = in3.prob[i][j];
			in3.prob[i][j] = 0.0;
			double sco = StepN(in3, 10, k);
			if (k == 367)cerr << k << ' ' << i << ' ' << j << ' ' << sco << endl;
			if (sco > minP)
			{
				if (k == 367)cerr << "A" << k << ' ' << i << ' ' << j << ' ' << sco << endl;
				minP = sco;
				y = i; x = j;
			}
		}
		in2.bit.set(y * 40 + x);
		in2.prob[y][x] = 0.0;
		order.push_back(y * 40 + x);
	}
	cerr << Score(order, 0) << endl;
	cerr << timer.GetSeconds() << endl;
	//Output(order);
	//while (1) {}
	return order;
}

void Solve(vector<int>order)
{
	long long score = Score(order, 0);
	cerr << score << endl;
	while (timer.GetSeconds() <= 1.9)
	{
		int a = rnd() % order.size();
		int b = rnd() % order.size();
		swap(order[a], order[b]);
		long long nex = Score(order, score);
		if (nex > score)
		{
			score = nex;
			cerr << score << endl;
		}
		else
		{
			swap(order[a], order[b]);
		}
	}

	Output(order);
}

int main() {
	Timer timer;
	cin >> N >> M;
	for (int i = 0; i < N; ++i) {
		string s;
		cin >> s;
		for (int j = 0; j < N; ++j) {
			if (s[j] == '#') in.bit.set(i * 40 + j);
		}
	}
	REP(i, 40)REP(j, 40)if (!in.bit.test(i * 40 + j))in.prob[i][j] = 1.0 / (40.0 * 40 - M);

	auto v = Greedy();
	Solve(v);
	//while (1) {}
	return 0;
}