#include <iostream>
#include <algorithm>
#include <bitset>
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

#define InfL 2000000000
#define InfLL 4000000000000000000LL
#define mod 1000000007
#define rep(i,n) for(int i=0;i<n;i++)
#define rrep(i,n) for(int i=(n-1);i>=0;i--)
using namespace std;
typedef long long ll;
typedef double db;
typedef long double ld;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef vector<int> vi;
typedef vector<ll> vl;
typedef vector<bool> vb;
typedef vector<db> vd;

bool debug = false; // false : 提出, true : fin - fout
double time_ratio = 1.0;
clock_t time_ini;
string file_No = "0000";
ifstream fin;
ofstream fout;
ofstream fout_debug;
ofstream fout_score;
int Gaussian_size = 1000000;
vector<double> Gaussian_(Gaussian_size);
struct timespec timeA;

const vi dx = { -1, 1, 0, 0 };
const vi dy = { 0, 0, -1, 1 };

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

	time_ratio = 0.133; // 5950X
	// time_ratio = 0.183; // 12700KF
}

void debug_end() {
	fin.close();
	fout.close();
	fout_debug.close();
	fout_score.close();
}

double time_diff()
{
	ll time_tmp = clock();
	const double time = time_ratio * static_cast<double>(time_tmp - time_ini) / CLOCKS_PER_SEC * 1000.0;
	return time;
}

double time_diff_new()
{
	struct timespec timeC;
	timespec_get(&timeC, TIME_UTC);
	double ans = timeC.tv_nsec - timeA.tv_nsec;
	ans += (double)(timeC.tv_sec - timeA.tv_sec) * 1000000000.0;
	return time_ratio * ans / 1000000.0;
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

double Gaussian() {
	double X = RAND_();
	double Y = RAND_();
	double Z = sqrt(-2.0 * log(X)) * cos(2.0 * acos(-1.0) * Y);
	return Z;
}


struct Input {
	int N, M;
	int si, sj;
	int A[15][15];
	int T[200][5];

	void readProblem(istream& in = cin) {
		in >> N >> M;
		in >> si >> sj;
		rep(i, N) {
			rep(j, N) {
				char c;
				in >> c;
				A[i][j] = c - 'A';
			}
		}
		rep(i, M) {
			string S;
			in >> S;
			rep(j, 5)
				T[i][j] = S[j] - 'A';
		}
		return;
	}
};

struct Output {
	vi i, j;
	void writeSolution(ostream& out = cout) {
		int L = i.size();
		rep(k, L)
			out << i[k] << " " << j[k] << "\n";
		return;
	}
};

Input input;
Output output;

int evaltmp(int kL, int kR) {
	rrep(p, 6) {
		bool can = true;
		rep(k, p) {
			if (input.T[kL][k + 5 - p] != input.T[kR][k])
				can = false;
		}
		if (can)
			return p;
	}
	return 0;
}

int dist2(int ifrom, int jfrom, int ito, int jto) {
	return abs(ifrom - ito) + abs(jfrom - jto) + 1;
}

struct Info {
	int N, M;
	int L = 0;
	pii a_to_ij[27][15 * 15];
	int Sa[27];
	int P[200];
	int sc2[200][200];
	void resize_all(int N_) {
		rep(k, 26)
			Sa[k] = 0;
		N = N_;
		M = 200;
		rep(i, N) {
			rep(j, N) {
				int k = input.A[i][j];
				a_to_ij[k][Sa[k]] = make_pair(i, j);
				Sa[k]++;
			}
		}
		rep(k, M)
			P[k] = k;
		rep(k1, M) {
			rep(k2, M) {
				if (k1 == k2)
					sc2[k1][k2] = -InfL;
				else
					sc2[k1][k2] = evaltmp(k1, k2);
			}
		}
		a_to_ij[26][0] = make_pair(input.si, input.sj);
		Sa[26] = 1;
		return;
	}
};

Info info;


void solve_init() {
	int N = input.N;
	info.resize_all(N);

	return;
}

int DP[1001][15 * 15];
int DPfrom[1001][15 * 15];

int solveB(bool p = false) {
	int N = input.N;
	vi S;
	rep(k, 200) {
		int pp = 5;
		int kL = info.P[k];
		if (k != 199) {
			int kR = info.P[k + 1];
			pp -= info.sc2[kL][kR];
		}
		rep(i, pp)
			S.push_back(input.T[kL][i]);
	}
	int L = S.size();
	DP[0][0] = 0;
	rep(k, L) {
		int Sfrom = 1;
		if (k >= 1)
			Sfrom = info.Sa[S[k - 1]];
		int Sto= info.Sa[S[k]];
		rep(pto, Sto)
			DP[k + 1][pto] = InfL;
		rep(pfrom, Sfrom) {
			rep(pto, Sto) {
				// 行先減らせば高速化できるかも？
				int ifrom = input.si;
				int jfrom = input.sj;
				if (k >= 1) {
					ifrom = info.a_to_ij[S[k - 1]][pfrom].first;
					jfrom = info.a_to_ij[S[k - 1]][pfrom].second;
				}
				int ito = info.a_to_ij[S[k]][pto].first;
				int jto = info.a_to_ij[S[k]][pto].second;
				int d = dist2(ifrom, jfrom, ito, jto);
				if (DP[k + 1][pto] > DP[k][pfrom] + d) {
					DP[k + 1][pto] = DP[k][pfrom] + d;
					DPfrom[k + 1][pto] = pfrom;
				}
			}
		}
	}
	int ptmp = -1;
	int scmin = InfL;
	rep(ptmptmp, info.Sa[S[L - 1]]) {
		if (scmin > DP[L][ptmptmp]) {
			scmin = DP[L][ptmptmp];
			ptmp = ptmptmp;
		}
	}
	//cout<< scmin<<endl;
	if (p) {
		cout << 10000 - scmin << endl;
		fout_score << 10000 - scmin << endl;
	}
	output.i.clear();
	output.j.clear();
	rrep(k, L) {
		output.i.push_back(info.a_to_ij[S[k]][ptmp].first);
		output.j.push_back(info.a_to_ij[S[k]][ptmp].second);
		ptmp = DPfrom[k + 1][ptmp];
	}
	reverse(output.i.begin(), output.i.end());
	reverse(output.j.begin(), output.j.end());
	return 10000 - scmin;
}

void solveA() {
	int N = input.N;
	int M = input.M;
	ll sc = 0;
	ll scall = -InfL;
	bool once = true;
	rep(k, M - 1)
		sc += (ll)info.sc2[info.P[k]][info.P[k + 1]];


	double time_p = 70.0;
	double start_temp = 0.1;
	double end_temp = 0.01;
	double start_temp2 = 0.1;
	double end_temp2 = 0.03;
	double TIME_LIMIT = 1985.0;
	ll loop_count = 0;
	double time_now = 0.0;
	double temp = 0.0;
	double temp2 = 0.0;
	while (1) {
		//break;
		int L = 10000;
		if (!once)
			L = 1000;
		if (loop_count % L == 0) {
			time_now = time_diff();
			if (time_now > TIME_LIMIT)
				break;
			temp = start_temp + (end_temp - start_temp) * time_now / TIME_LIMIT;
			if (once && time_now > time_p) {
				temp2 = start_temp2 + (end_temp2 - start_temp2) * time_now / TIME_LIMIT;
				once = false;
				scall = solveB();
			}
		}

		loop_count++;
		ll scdiff = 0;
		int k0 = xor128() % 200;
		int k1 = xor128() % 200;
		int k2 = xor128() % 200;

		vi Pnew;
		if (k1 > k2)
			swap(k1, k2);
		k0 = xor128() % (M - (k2 - k1));
		vi Ptmp;
		rep(k, M) {
			if (k >= k1 && k <= k2) continue;
			Ptmp.push_back(info.P[k]);
		}
		int kk = k0;
		if (k0 >= k1)
			kk++;
		rep(k, Ptmp.size() + 2) {
			if (k == kk) {
				for (int k = k1; k <= k2; k++)
					Pnew.push_back(info.P[k]);
			}
			if (k < Ptmp.size())
				Pnew.push_back(Ptmp[k]);
		}
		ll sc_ = 0;
		rep(k, M - 1)
			sc_ += info.sc2[Pnew[k]][Pnew[k + 1]];
		scdiff = sc_ - sc;

		double prob = exp((double)scdiff / temp);
		bool ch = false;
		if (time_now <= time_p) {
			if (prob > RAND_())
				ch = true;
		}
		else {
			ll sctmp = 0;
			if (scdiff >= 0) {
				int Pold[200];
				rep(i, 200)
					Pold[i] = info.P[i];
				rep(i, 200)
					info.P[i] = Pnew[i];
				sctmp = solveB();
				rep(i, 200)
					info.P[i] = Pold[i];
			}
			//cout << sctmp << endl;
			ll scdiff2 = sctmp - scall;
			if (true) {
				double prob2 = exp((double)scdiff2 / temp2);
				if (prob2 > RAND_()) {
					scall = sctmp;
					ch = true;
				}
			}
		}
		if (ch) {
			sc += scdiff;
			//cout << sc << endl;
			//cout << k0 << " " << k1 << endl;
			rep(i, 200)
				info.P[i] = Pnew[i];
		}
	}

	info.L = 1000 - sc;
	solveB();
	return;
}

int main(int argc, char* argv[]) {
	if (argc >= 2) file_No = argv[1];
	time_ini = clock();
	timespec_get(&timeA, TIME_UTC);
	if (debug)
		debug_init();
	debug ? input.readProblem(fin) : input.readProblem();
	solve_init();
	solveA();
	debug ? output.writeSolution(fout) : output.writeSolution();
	if (debug)
		debug_end();
	return 0;
}
