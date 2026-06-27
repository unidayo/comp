// cl cmd.cpp -O2 /std:c++17
#include <iostream>
#include <algorithm>
#include <bitset>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <deque>
#include <set>
#include <stack>
#include <string>
#include <cstring>
#include <utility>
#include <vector>
#include <complex>
#include <valarray>
#include <fstream>
#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <numeric>
#include <climits>
#include <random>
#include <time.h>
//#include <atcoder/dsu.hpp>

#define rep(i,n) for(int i=0;i<n;i++)
#define rrep(i,n) for(int i=(n-1);i>=0;i--)
using namespace std;
typedef long long ll;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef vector<int> vi;
typedef vector<ll> vl;
typedef vector<bool> vb;
typedef vector<double> vd;

bool debug = false; // false : 提出, true : fin - fout
double time_ratio = 1.0;
string file_No = "0000";
ifstream fin;
ofstream fout;
ofstream fout_debug;
ofstream fout_score;
int Gaussian_size = 1000000;
vector<double> Gaussian_(Gaussian_size);
struct timespec timeini;

const int InfL = 2000000000;
const ll InfLL = 4000000000000000000LL;
const vi dx = { 1, 0, -1, 0 }; // DRUL
const vi dy = { 0, 1, 0, -1 }; // DRUL
const vi dx8 = { 1, 1, 0, -1, -1, -1, 0, 1 };
const vi dy8 = { 0, 1, 1, 1, 0, -1, -1, -1 };
vi nextXY; // DRUL

void debug_init() {
	string txt = ".txt";

	string ifname = "in\\";
	ifname = ifname + file_No + txt;
	fin.open(ifname);
	string ofname = "out\\";
	ofname = ofname + file_No + txt;
	fout.open(ofname);
	string ofname_debug = "debug\\";
	ofname_debug = ofname_debug + file_No + txt;
	fout_debug.open(ofname_debug);
	string ofname_score = "score\\";
	ofname_score = ofname_score + file_No + txt;
	fout_score.open(ofname_score);

	//time_ratio = 1.15; // 14900K
	time_ratio = 0.11; // 5950X
	//time_ratio = 0.7; // 1255U
}

void debug_end() {
	fin.close();
	fout.close();
	fout_debug.close();
}

double time_diff()
{
	struct timespec timetmp;
	timespec_get(&timetmp, TIME_UTC);
	double ans = timetmp.tv_nsec - timeini.tv_nsec;
	ans += (double)(timetmp.tv_sec - timeini.tv_sec) * 1000000000.0;
	return time_ratio * ans / 1000000.0;
}

void output_vi(vi &A) {
	if (!debug) return;
	int N = A.size();
	cout << "A ";
	rep(i, N)
		cout << A[i] << " ";
	cout << endl;
	return;
}

int xor128() {
	static int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
	int t = (x ^ (x << 11));
	x = y; y = z; z = w;
	return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

double RAND_() {
	double rand_ = (double)(xor128()) / (double)INT32_MAX;
	return rand_;
}

int RAND_clamp(int L, int R, int Q = 0) {
	if (L >= R) {
		cout << Q << endl;
		fout_debug << L << " " << R << endl;
		assert(L < R);
	}
	int n = R - L;
	return L + (xor128() % n);
}

double Gaussian() {
	double X = RAND_();
	double Y = RAND_();
	double Z = sqrt(-2.0 * log(X)) * cos(2.0 * acos(-1.0) * Y);
	return Z;
}

struct phash {
	inline size_t operator()(const pair<int, int>& p) const {
		const auto h1 = hash<int>()(p.first);
		const auto h2 = hash<int>()(p.second);
		return h1 ^ (h2 << 1);
	}
};

ll C1(ll d) { return d; }
ll C2(ll d) { return d * (d - 1) / 2; }
ll C3(ll d) { return d * (d - 1) * (d - 2) / 6; }
ll C4(ll d) { return d * (d - 1) * (d - 2) * (d - 3) / 24; }

struct Input {
	int N, L, T, K;
	vl A, C;
	vi Level, Id;
	vl P, B;
	vi idxtot;
	int idxMax = -1;
	vl C1memo;
	vl C2memo;
	vl C3memo;
	vl C4memo;
	void readProblem(istream& in = cin) {
		in >> N >> L >> T >> K;
		A.resize(N);
		C.resize(L << 4);
		P.resize(L << 4);
		B.resize(L << 4);
		rep(n, N)
			in >> A[n];
		rep(i, L) {
			rep(n, N)
				in >> C[i << 4| n];
		}
		Level.resize(T, 0);
		Id.resize(T, 0);
		idxtot.resize(T, 0);
		C1memo.resize(T + 10);
		C2memo.resize(T + 10);
		C3memo.resize(T + 10);
		C4memo.resize(T + 10);
		rep(t, T + 10) {
			C1memo[t] = C1(t);
			C2memo[t] = C2(t);
			C3memo[t] = C3(t);
			C4memo[t] = C4(t);
		}
		return;
	}

	double EvalAll() {
		ll scmax = 0;
		int scmaxidx = 0;
		int t = 0;
		ll a = 1;
		rep(idx, L << 4) {
			P[idx] = 0;
			B[idx] = 1;
		}
		int idx = 0;
		rep(t, T) {
			int inext = Level[idx];
			int jnext = Id[idx];
			ll cost = C[inext << 4 | jnext] * (P[inext << 4 | jnext] + 1);
			if (a >= cost) {
				a -= cost;
				idxtot[idx] = t;
				P[inext << 4 | jnext]++;
				idx++;
			}

			ll dt = T - t;
			ll adiff = 0;
			rep(j, N) {
				int ij0 = (0 << 4) | j;
				int ij1 = (1 << 4) | j;
				int ij2 = (2 << 4) | j;
				int ij3 = (3 << 4) | j;
				ll term0 = B[ij0] * C1memo[dt];
				ll term1 = B[ij1] * C2memo[dt] * P[ij1];
				ll term2 = B[ij2] * C3memo[dt] * P[ij1] * P[ij2];
				ll term3 = B[ij3] * C4memo[dt] * P[ij1] * P[ij2] * P[ij3];
				adiff += A[j] * P[ij0] * (term0 + term1 + term2 + term3);
			}
			if (scmax < a + adiff) {
				scmax = a + adiff;
				idxMax = idx;
			}

			rep(i, L) {
				rep(j, N) {
					if (i == 0)
						a += A[j] * B[i << 4 | j] * P[i << 4 | j];
					else
						B[(i - 1) << 4 | j] += B[i << 4 | j] * P[i << 4 | j];
				}
			}
		}

		return log2(scmax);
	}
};

Input input;
struct Output {
	vector<pii> commands;
	int T;

	void resize_all(int T_) {
		T = T_;
		commands.resize(T);
	}
	void writeSolution(ostream& out = cout) {
		rep(t, T)
			commands[t] = { -1,-1 };
		rep(idx, input.idxMax) {
			int t = input.idxtot[idx];
			commands[t] = { input.Level[idx], input.Id[idx] };
		}

		rep(t, T) {
			if (commands[t].first == -1)
				out << -1 << "\n";
			else
				out << commands[t].first << " " << commands[t].second << "\n";
		}
		return;
	}
};

Output output;

struct Info {
	int N;
	void resize_all(int N_) {
		N = N_;

		return;
	}
};

Info info;

void solve_init() {
	int N = input.N;
	int T = input.T;
	output.resize_all(T);
	info.resize_all(N);

	return;
}

void solveA() {
	int N = input.N;

	return;
}

using Cost = double;

class WeightedRandomIndex {
public:
	vd prefix;
	double total = 0.0;
	void reset(vd& weights) {
		int n = weights.size();
		prefix.resize(n + 1);
		prefix[0] = 0.0;
		rep(i, n) {
			double w = weights[i];
			prefix[i + 1] = prefix[i] + w;
		}
		total = prefix.back();
	}
	int sample() {
		double r = RAND_() * total;
		auto it = upper_bound(prefix.begin() + 1, prefix.end(), r);
		int idx = (it - prefix.begin() - 1);
		return idx;
	}
};

void solveSA() {
	int N = input.N;
	int T = input.T;
	int L = input.L;

	Cost sc = input.EvalAll();

	WeightedRandomIndex wi;
	WeightedRandomIndex wj;
	WeightedRandomIndex wjStart;
	WeightedRandomIndex widx;


	vd wiinit(N);
	wiinit[0] = 4.0;
	wiinit[1] = 3.0;
	wiinit[2] = 2.0;
	wiinit[3] = 1.0;
	wi.reset(wiinit);

	vd winit(N);
	rep(n, N) {
		winit[n] = pow((double)input.A[n], 1.5);
		winit[n] *= pow(input.C[0 << 4 | n], -0.25);
		winit[n] *= pow(input.C[1 << 4 | n], -0.25);
		winit[n] *= pow(input.C[2 << 4 | n], -0.25);
		winit[n] *= pow(input.C[3 << 4 | n], -0.25);
	}
	wj.reset(winit);


	vd winitStart(N);
	rep(n, N) {
		winitStart[n] = pow((double)input.A[n], 0.6);
		winitStart[n] *= pow(input.C[0 << 4 | n], -0.1);
		winitStart[n] *= pow(input.C[1 << 4 | n], -0.1);
		winitStart[n] *= pow(input.C[2 << 4 | n], -0.1);
		winitStart[n] *= pow(input.C[3 << 4 | n], -0.1);
	}
	wjStart.reset(winit);


	vd widxinit(T);
	rep(t, T)
		widxinit[t] = 1.0 / (double)(t + 100);
	widx.reset(widxinit);

	double start_temp = 0.015;
	double end_temp = 0.0002;
	double TIME_INIT = time_diff();
	double TIME_LIMIT = 1950.0 - TIME_INIT;
	ll loop_count = 0;
	double time_now = 0.0;
	double temp = 0.0;
	double progress = 0.0;

	int idx = -1;
	int iold = -1, inew = -1;
	int jold = -1, jnew = -1;

	vi LevelMemo = input.Level;
	vi IdMemo = input.Id;

	while (1) {
		//break;
		if (loop_count % 100 == 0) {
			time_now = time_diff() - TIME_INIT;
			if (time_now > TIME_LIMIT)
				break;
			//temp = start_temp + (end_temp - start_temp) * time_now / TIME_LIMIT;
			progress = time_now / TIME_LIMIT;
			temp = start_temp + (end_temp - start_temp) * pow(progress, 0.3);
		}

		loop_count++;
		Cost scdiff = 0;

		double scth = temp * log(RAND_());
		//scth = 0.0;
		int seed = xor128() % 25;
		int type = 0;
		if (seed < 10)
			type = 0;
		else if (seed < 15)
			type = 1;
		else if (seed < 20)
			type = 2;
		else if (seed < InfL)
			type = 3;
		if (type == 0) {
			//continue;
			idx = widx.sample();
			if (idx == 0) continue;
			iold = input.Level[idx];
			jold = input.Id[idx];
			inew = wi.sample();
			jnew = wj.sample();
			if (idx < 5) jnew = xor128() % N;
			else if (idx < 30) jnew = wjStart.sample();
			if (progress > 0.5 && xor128() % 2 == 0) {
				if (xor128() % 2 == 0 && idx > 0)
					jnew = input.Id[idx - 1];
				else if (idx < T - 1)
					jnew = input.Id[idx + 1];
			}

			input.Level[idx] = inew;
			input.Id[idx] = jnew;
			Cost scnew = input.EvalAll();
			scdiff = scnew - sc;
		}
		else if (type == 1) {
			//continue;
			idx = widx.sample();
			if (idx == 0) continue;
			if (idx == T - 1) continue;
			swap(input.Level[idx], input.Level[idx + 1]);
			swap(input.Id[idx], input.Id[idx + 1]);
			Cost scnew = input.EvalAll();
			scdiff = scnew - sc;
		}
		else if (type == 2) {
			//erase
			//continue;
			idx = widx.sample();
			if (idx == 0) continue;
			rep(t, T) {
				LevelMemo[t] = input.Level[t];
				IdMemo[t] = input.Id[t];
			}
			for (int t = idx + 1; t < T; t++) {
				input.Level[t - 1] = LevelMemo[t];
				input.Id[t - 1] = IdMemo[t];
			}
			input.Level[T - 1] = LevelMemo[idx];
			input.Id[T - 1] = IdMemo[idx];
			Cost scnew = input.EvalAll();
			scdiff = scnew - sc;
		}
		else if (type == 3) {
			//insert
			//continue;
			idx = widx.sample();
			if (idx == 0) continue;
			rep(t, T) {
				LevelMemo[t] = input.Level[t];
				IdMemo[t] = input.Id[t];
			}

			inew = wi.sample();
			jnew = wj.sample();
			if (idx < 5) jnew = xor128() % N;
			else if (idx < 30) jnew = wjStart.sample();
			if (progress > 0.5 && xor128() % 2 == 0) {
				if (xor128() % 2 == 0 && idx > 0)
					jnew = input.Id[idx - 1];
				else if (idx < T - 1)
					jnew = input.Id[idx + 1];
			}
			input.Level[idx] = inew;
			input.Id[idx] = jnew;

			for (int t = idx + 1; t < T; t++) {
				input.Level[t] = LevelMemo[t - 1];
				input.Id[t] = IdMemo[t - 1];
			}
			Cost scnew = input.EvalAll();
			scdiff = scnew - sc;
		}

		if (scdiff > scth) { 
			sc += scdiff;
			//if (scdiff > 0) cout << sc << endl;
		}
		else {
			if (type == 0) {
				input.Level[idx] = iold;
				input.Id[idx] = jold;
			}
			else if (type == 1) {
				swap(input.Level[idx], input.Level[idx + 1]);
				swap(input.Id[idx], input.Id[idx + 1]);
			}
			else if (type <= 3) {
				rep(t, T) {
					input.Level[t] = LevelMemo[t];
					input.Id[t] = IdMemo[t];
				}
			}
		}
	}
	sc = input.EvalAll();

	return;
}

int main(int argc, char* argv[]) {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	if (argc >= 2) file_No = argv[1];
	timespec_get(&timeini, TIME_UTC);
	if (debug)
		debug_init();
	debug ? input.readProblem(fin) : input.readProblem();
	solve_init();
	solveA();
	solveSA();
	debug ? output.writeSolution(fout) : output.writeSolution();
	if (debug)
		debug_end();
	return 0;
}
