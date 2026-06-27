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
#include <atcoder/dsu.hpp>

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
string file_No = "0000";
ifstream fin;
ofstream fout;
ofstream fout_debug;
ofstream fout_score;
const int Gaussian_size = 1000000;
double Gaussian_[Gaussian_size];
struct timespec timeini;

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

	time_ratio = 0.88; // 5950X
	//time_ratio = 0.88; // 12700KF
}

void debug_end() {
	fin.close();
	fout.close();
	fout_debug.close();
	fout_score.close();
}

double time_diff()
{
	struct timespec timetmp;
	timespec_get(&timetmp, TIME_UTC);
	double ans = timetmp.tv_nsec - timeini.tv_nsec;
	ans += (double)(timetmp.tv_sec - timeini.tv_sec) * 1000000000.0;
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

int RAND_clamp(int L, int R) {
	int n = R - L;
	return L + (xor128() % n);
}

double Gaussian() {
	double X = RAND_();
	double Y = RAND_();
	double Z = sqrt(-2.0 * log(X)) * cos(2.0 * acos(-1.0) * Y);
	return Z;
}


struct Input {
	int W, D, N;
	int A[51][51];

	void readProblem(istream& in = cin) {
		in >> W >> D >> N;
		rep(d, D) {
			rep(n, N)
				in >> A[d][n];
		}
		return;
	}
};
Input input;

struct Info {
	int W, D, N, E;
	int Nx;
	int X[51]; // 焼く
	int Y[51][51][51]; // 焼く
	int Ny[51][51];
	int Xmem[51];
	int Ymem[51][51][51];
	int Nymem[51][51];
	int sc1[51][51];
	int sc2[51];
	int sc1mem[51][51];
	int sc2mem[51];
	vi S;
	int Xlist[2];
	void mem_copy(int d, bool is_all = false) {
		if (is_all) {
			rep(i, Nx - 1) {
				Nymem[d][i] = Ny[d][i];
				rep(j, Ny[d][i])
					Ymem[d][i][j] = Y[d][i][j];
				if (d > 0)
					sc1mem[d - 1][i] = sc1[d - 1][i];
				if (d < D - 1)
					sc1mem[d][i] = sc1[d][i];
			}
		}
		else {
			rep(idx, 2) {
				int i = Xlist[idx];
				if (i == -1) continue;
				Nymem[d][i] = Ny[d][i];
				rep(j, Ny[d][i])
					Ymem[d][i][j] = Y[d][i][j];
				if (d > 0)
					sc1mem[d - 1][i] = sc1[d - 1][i];
				if (d < D - 1)
					sc1mem[d][i] = sc1[d][i];
			}
		}
		sc2mem[d] = sc2[d];
		return;
	}
	void mem_copy_all(int d0, int d1, bool is_all = false) {
		for (int d = d0; d <= d1; d++)
			mem_copy(d, is_all);
		return;
	}
	void mem_copy_X() {
		rep(i, Nx)
			Xmem[i] = X[i];
		rep(d, D) {
			if (d < D - 1) {
				rep(idx, 2) {
					int i = Xlist[idx];
					if (i == -1) continue;
					sc1mem[d][i] = sc1[d][i];
				}
			}
			sc2mem[d] = sc2[d];
		}
		return;
	}
	void mem_back(int d, bool is_all = false) {
		if (is_all) {
			rep(i, Nx - 1) {
				Ny[d][i] = Nymem[d][i];
				rep(j, Ny[d][i])
					Y[d][i][j] = Ymem[d][i][j];
				if (d > 0)
					sc1[d - 1][i] = sc1mem[d - 1][i];
				if (d < D - 1)
					sc1[d][i] = sc1mem[d][i];
			}
		}
		else {
			rep(idx, 2) {
				int i = Xlist[idx];
				if (i == -1) continue;
				Ny[d][i] = Nymem[d][i];
				rep(j, Ny[d][i])
					Y[d][i][j] = Ymem[d][i][j];
				if (d > 0)
					sc1[d - 1][i] = sc1mem[d - 1][i];
				if (d < D - 1)
					sc1[d][i] = sc1mem[d][i];
			}
		}
		sc2[d] = sc2mem[d];
		return;
	}
	void mem_back_all(int d0, int d1, bool is_all = false) {
		for (int d = d0; d <= d1; d++)
			mem_back(d, is_all);
		return;
	}
	void mem_back_X() {
		rep(i, Nx)
			X[i] = Xmem[i];
		rep(d, D) {
			if (d < D - 1) {
				rep(idx, 2) {
					int i = Xlist[idx];
					if (i == -1) continue;
					sc1[d][i] = sc1mem[d][i];
				}
			}
			sc2[d] = sc2mem[d];
		}
		return;
	}
	void resize_all(int W_, int D_, int N_) {
		W = W_;
		D = D_;
		N = N_;
		S.resize(N);

		vi Amax(N, 0);
		E = 0;
		rep(d, D) {
			rep(n, N) {
				Amax[N - n - 1] = max(Amax[N - n - 1], input.A[d][n]);
				E += input.A[d][n];
			}
		}
		E /= D;
		E = W * W - E;


		vi rlist;
		rep(k, 501)
			rlist.push_back(k);
		if (E * N > 5000000)
			rlist = { -1 };
		else if (E * N > 1000000)
			rlist = { 0 };
		int P = rlist.size();
		vector<vi> Xall(P);
		vi Nxall(P);
		vi scall(P, -InfL);
		vector<vector<vector<set<int>>>> Nsetall(P);

		int stopth = -100 * D;
		int Nxprev = -1;
		int ploop = 0;
		bool stop = false;
		rep(p, P + 1) {
			int r = 0;
			if (p == P) {
				int scmax = -InfL;
				int scargmax = -1;
				rep(p, P) {
					if (scmax < scall[p]) {
						scmax = scall[p];
						scargmax = p;
					}
				}
				r = rlist[scargmax];
			}
			else {
				if (stop || time_diff() > 1000.0) {
					scall[p] = -InfL;
					continue;
				}
				r = rlist[p];
			}
			int Lmin = 0;
			if (P == 1) {
				p = P;
				r = 0;
			}
			Lmin = r;
			Lmin = max(Lmin, 0); 
			int BSscsum = 0;
			X[0] = 0;
			Nx = 1;


			if (r == -1) {
				int q = 0;
				vi idxtmp(D, N - 1);
				rep(n, N) {
					int a = -InfL;
					rep(d, D) {
						int i = idxtmp[d];
						if (i == -1) continue;
						a = max(a, input.A[d][i]);
					}
					if (a == -InfL)
						break;
					int L = max(Lmin, (a + W - 1) / W);
					if (L == Lmin) {
						L += q;
						q++;
					}
					X[n + 1] = X[n] + L;
					if (X[n + 1] > W) {
						X[n] = W;
						break;
					}
					rep(d, D) {
						int asum = 0;
						while (1) {
							int i = idxtmp[d];
							if (i == -1) break;
							asum += input.A[d][i];
							if (asum > L * W)
								break;
							idxtmp[d]--;
						}
					}
					Nx++;
				}
				X[Nx - 1] = W;

			}
			else {
				int q = 0;
				rep(n, N) {
					int a = Amax[n];
					int L = max(Lmin, (a + W - 1) / W);
					if (L == Lmin) {
						L += q;
						q++;
					}
					X[n + 1] = X[n] + L;
					if (X[n + 1] > W) {
						X[n] = W;
						break;
					}
					Nx++;
				}
				X[Nx - 1] = W;
			}

			if (p != P) {
				if (Nx == Nxprev) continue;
				Nxprev = Nx;
				ploop++;
			}

			int Xdiff[51];
			rep(i, Nx - 1)
				Xdiff[i] = X[i + 1] - X[i];

			int BSsize = 300;
			int R = N * Nx * D;
			R = min(R, 50 * 10 * 50);
			R = max(R, 5 * 2 * 5);
			BSsize *= 50 * 10 * 50;
			BSsize /= R;
			if (P == 1)
				BSsize = 300;
			else if (p != P || time_diff() > 1000.0) {
				BSsize /= 3;
				if (ploop >= 1 && p != P)
					BSsize /= ploop;
			}

			rep(d, D) {
				int ni_to_y[51][51];
				int ni_to_a[51][51];
				rep(n, N) {
					rep(i, Nx - 1) {
						int x = Xdiff[i];
						int a = input.A[d][n];
						ni_to_y[n][i] = (a + x - 1) / x;
						ni_to_a[n][i] = ((a + x - 1) / x) * x;
					}
				}
				vector<vector<pii>> idxfrom(N, vector<pii>(BSsize, make_pair(-1, -1)));
				vector<pair<pii, vi>> BS(BSsize); // itmp, jfrom
				vi BSinit_second(Nx - 1, 0);
				BS[0] = make_pair(make_pair(-1, -1), BSinit_second);
				int BSsizetmp = 1;
				rrep(n, N) {
					priority_queue<pair<double, pii>, vector<pair<double, pii>>, less<pair<double, pii>>> BSsort;
					int idx = 0;
					int j = 0;
					rep(k, BSsizetmp) {
						auto solnew = BS[k];
						solnew.first.second = j;
						double sc = 0.0;
						int empcount = 0;
						rep(i, Nx - 1) {
							if (solnew.second[i] == 0)
								empcount++;
							if (solnew.second[i] > W)
								sc -= (double)((solnew.second[i] - W) * Xdiff[i]);
						}

						rep(i, Nx - 1) {
							solnew.first.first = i;
							double scdiff = 0;
							int empcount_diff = 0;
							if (solnew.second[i] > W)
								scdiff += (solnew.second[i] - W) * Xdiff[i];
							if (solnew.second[i] == 0)
								empcount_diff--;

							solnew.second[i] += ni_to_y[n][i];
							if (solnew.second[i] > W)
								scdiff -= (solnew.second[i] - W) * Xdiff[i];
							if (solnew.second[i] == 0)
								empcount_diff++;

							if (n >= empcount + empcount_diff)
								BSsort.push(make_pair(sc + scdiff + RAND_(), make_pair(i, j)));

							solnew.second[i] -= ni_to_y[n][i];
						}
						j++;
					}
					int L = min((int)BSsort.size(), BSsize);
					auto BSold = BS;
					if (n == 0) {
						BSscsum += (int)(BSsort.top()).first;
					}
					rep(jtmp, L) {
						idxfrom[n][jtmp] = (BSsort.top()).second;
						BSsort.pop();
						int i = idxfrom[n][jtmp].first;
						int jnew = idxfrom[n][jtmp].second;
						auto solnew = BSold[jnew];
						solnew.first = make_pair(i, jnew);
						solnew.second[i] += ni_to_y[n][i];
						BS[jtmp] = solnew;
					}
					BSsizetmp = L;
				}

				int jtmp = 0;
				set<int> Nset[51];
				rep(n, N) {
					pii idxtmp = idxfrom[n][jtmp];
					int i = idxtmp.first;
					int jnext = idxtmp.second;
					Nset[i].insert(n);
					jtmp = jnext;
				}

				ll sc = 0;
				vi asum(Nx - 1, 0);
				vi amax(Nx - 1, 0);
				rep(i, Nx - 1) {
					int x = X[i + 1] - X[i];
					for (int n : Nset[i]) {
						int a = input.A[d][n];
						int atmp = ((a + x - 1) / x) * x;
						asum[i] += atmp;
					}
					amax[i] = x * W;
					sc -= max(0, asum[i] - amax[i]);
				}

				double start_temp = 50.0;
				double end_temp = 0.0;
				double TIME_INIT = time_diff();
				double T = 40.0 / (double)D;
				if (p == P)
					T = 200.0 / (double)D;
				double TIME_LIMIT = TIME_INIT + T;
				ll loop_count = 0;
				double time_now = 0.0;
				double temp = 0.0;
				while (1) {
					//break;
					//if (p != P)
						//break;
					if (sc == 0)
						break;
					if (loop_count % 2000 == 0) {
						time_now = time_diff();
						if (time_now > TIME_LIMIT)
							break;
						temp = start_temp + (end_temp - start_temp) * (time_now - TIME_INIT) / (TIME_LIMIT - TIME_INIT);
					}

					loop_count++;
					ll scdiff = 0;

					int type = xor128() % 4;
					int ifrom = xor128() % (Nx - 1);
					int ito = xor128() % (Nx - 1);
					int nfrom = -1, nto = -1, ntosub = -1;
					if (type == 0) {
						// swap
						//continue;
						if (ifrom == ito) continue;
						int jfrom = xor128() % Nset[ifrom].size();
						int jto = xor128() % Nset[ito].size();
						auto itrf = next(Nset[ifrom].begin(), jfrom);
						auto itrt = next(Nset[ito].begin(), jto);
						nfrom = *itrf;
						nto = *itrt;

						int asumfrom = asum[ifrom];
						int asumto = asum[ito];
						int afrom = input.A[d][nfrom];
						int ato = input.A[d][nto];
						int xfrom = X[ifrom + 1] - X[ifrom];
						int xto = X[ito + 1] - X[ito];
						scdiff += max(0, asumfrom - amax[ifrom]);
						scdiff += max(0, asumto - amax[ito]);

						asumfrom -= ((afrom + xfrom - 1) / xfrom) * xfrom;
						asumfrom += ((ato + xfrom - 1) / xfrom) * xfrom;
						asumto += ((afrom + xto - 1) / xto) * xto;
						asumto -= ((ato + xto - 1) / xto) * xto;
						scdiff -= max(0, asumfrom - amax[ifrom]);
						scdiff -= max(0, asumto - amax[ito]);
					}
					else if (type == 1) {
						// insert
						//continue;
						if (ifrom == ito) continue;
						if (Nset[ifrom].size() <= 1) continue;
						int jfrom = xor128() % Nset[ifrom].size();
						auto itrf = next(Nset[ifrom].begin(), jfrom);
						nfrom = *itrf;

						int asumfrom = asum[ifrom];
						int asumto = asum[ito];
						int afrom = input.A[d][nfrom];
						int xfrom = X[ifrom + 1] - X[ifrom];
						int xto = X[ito + 1] - X[ito];
						scdiff += max(0, asumfrom - amax[ifrom]);
						scdiff += max(0, asumto - amax[ito]);

						asumfrom -= ((afrom + xfrom - 1) / xfrom) * xfrom;
						asumto += ((afrom + xto - 1) / xto) * xto;
						scdiff -= max(0, asumfrom - amax[ifrom]);
						scdiff -= max(0, asumto - amax[ito]);
					}
					else if (type == 2) {
						// swap_all
						//continue;
						if (ifrom == ito) continue;
						int xfrom = X[ifrom + 1] - X[ifrom];
						int xto = X[ito + 1] - X[ito];
						scdiff += max(0, asum[ifrom] - amax[ifrom]);
						scdiff += max(0, asum[ito] - amax[ito]);

						int asumfrom = 0;
						int asumto = 0;
						for (int n : Nset[ito]) {
							int a = input.A[d][n];
							asumfrom += ((a + xfrom - 1) / xfrom) * xfrom;
						}
						for (int n : Nset[ifrom]) {
							int a = input.A[d][n];
							asumto += ((a + xto - 1) / xto) * xto;
						}
						scdiff -= max(0, asumfrom - amax[ifrom]);
						scdiff -= max(0, asumto - amax[ito]);
					}
					else if (type == 3) {
						// swap(1-2)
						//continue;
						if (ifrom == ito) continue;
						int jfrom = xor128() % Nset[ifrom].size();
						int jto1 = xor128() % Nset[ito].size();
						int jto2 = xor128() % Nset[ito].size();
						if (jto1 == jto2) continue;
						auto itrf = next(Nset[ifrom].begin(), jfrom);
						auto itrt1 = next(Nset[ito].begin(), jto1);
						auto itrt2 = next(Nset[ito].begin(), jto2);
						nfrom = *itrf;
						nto = *itrt1;
						ntosub = *itrt2;

						int asumfrom = asum[ifrom];
						int asumto = asum[ito];
						int afrom = input.A[d][nfrom];
						int ato1 = input.A[d][nto];
						int ato2 = input.A[d][ntosub];
						int xfrom = X[ifrom + 1] - X[ifrom];
						int xto = X[ito + 1] - X[ito];
						scdiff += max(0, asumfrom - amax[ifrom]);
						scdiff += max(0, asumto - amax[ito]);

						asumfrom -= ((afrom + xfrom - 1) / xfrom) * xfrom;
						asumfrom += ((ato1 + xfrom - 1) / xfrom) * xfrom;
						asumfrom += ((ato2 + xfrom - 1) / xfrom) * xfrom;
						asumto += ((afrom + xto - 1) / xto) * xto;
						asumto -= ((ato1 + xto - 1) / xto) * xto;
						asumto -= ((ato2 + xto - 1) / xto) * xto;
						scdiff -= max(0, asumfrom - amax[ifrom]);
						scdiff -= max(0, asumto - amax[ito]);
					}

					double prob = exp((double)scdiff / temp);
					//if (scdiff > 0) {
					if (prob > RAND_()) {
						sc += scdiff;
						if (type == 0) {
							Nset[ifrom].erase(nfrom);
							Nset[ifrom].insert(nto);
							Nset[ito].erase(nto);
							Nset[ito].insert(nfrom);
							int asumfrom = asum[ifrom];
							int asumto = asum[ito];
							int afrom = input.A[d][nfrom];
							int ato = input.A[d][nto];
							int xfrom = X[ifrom + 1] - X[ifrom];
							int xto = X[ito + 1] - X[ito];
							asumfrom -= ((afrom + xfrom - 1) / xfrom) * xfrom;
							asumfrom += ((ato + xfrom - 1) / xfrom) * xfrom;
							asumto += ((afrom + xto - 1) / xto) * xto;
							asumto -= ((ato + xto - 1) / xto) * xto;
							asum[ifrom] = asumfrom;
							asum[ito] = asumto;
						}
						else if (type == 1) {
							Nset[ifrom].erase(nfrom);
							Nset[ito].insert(nfrom);
							int asumfrom = asum[ifrom];
							int asumto = asum[ito];
							int afrom = input.A[d][nfrom];
							int xfrom = X[ifrom + 1] - X[ifrom];
							int xto = X[ito + 1] - X[ito];
							asumfrom -= ((afrom + xfrom - 1) / xfrom) * xfrom;
							asumto += ((afrom + xto - 1) / xto) * xto;
							asum[ifrom] = asumfrom;
							asum[ito] = asumto;
						}
						else if (type == 2) {
							swap(Nset[ifrom], Nset[ito]);
							int xfrom = X[ifrom + 1] - X[ifrom];
							int xto = X[ito + 1] - X[ito];
							int asumfrom = 0;
							int asumto = 0;
							for (int n : Nset[ifrom]) {
								int a = input.A[d][n];
								asumfrom += ((a + xfrom - 1) / xfrom) * xfrom;
							}
							for (int n : Nset[ito]) {
								int a = input.A[d][n];
								asumto += ((a + xto - 1) / xto) * xto;
							}
							asum[ifrom] = asumfrom;
							asum[ito] = asumto;
						}
						else if (type == 3) {
							Nset[ifrom].erase(nfrom);
							Nset[ifrom].insert(nto);
							Nset[ifrom].insert(ntosub);
							Nset[ito].erase(nto);
							Nset[ito].erase(ntosub);
							Nset[ito].insert(nfrom);
							int asumfrom = asum[ifrom];
							int asumto = asum[ito];
							int afrom = input.A[d][nfrom];
							int ato1 = input.A[d][nto];
							int ato2 = input.A[d][ntosub];
							int xfrom = X[ifrom + 1] - X[ifrom];
							int xto = X[ito + 1] - X[ito];
							asumfrom -= ((afrom + xfrom - 1) / xfrom) * xfrom;
							asumfrom += ((ato1 + xfrom - 1) / xfrom) * xfrom;
							asumfrom += ((ato2 + xfrom - 1) / xfrom) * xfrom;
							asumto += ((afrom + xto - 1) / xto) * xto;
							asumto -= ((ato1 + xto - 1) / xto) * xto;
							asumto -= ((ato2 + xto - 1) / xto) * xto;
							asum[ifrom] = asumfrom;
							asum[ito] = asumto;
						}
					}
				}

				if (p != P) {
					Nsetall[p].resize(D);
					rep(d, D) {
						Nsetall[p][d].resize(Nx - 1);
						rep(i, Nx - 1)
							Nsetall[p][d][i] = Nset[i];
					}
				}

				rep(i, Nx - 1) {
					int x = X[i + 1] - X[i];
					Ny[d][i] = Nset[i].size() + 1;
					int Ysum = 0;
					for (int n : Nset[i])
						Ysum += (input.A[d][n] + x - 1) / x;
					int Ysumtmp = 0;
					auto itr = Nset[i].begin();
					rep(j, Ny[d][i] - 1) {
						Y[d][i][j] = ((W - (Ny[d][i] - 1)) * Ysumtmp) / Ysum;
						Y[d][i][j] += j;
						int n = *itr;
						Ysumtmp += (input.A[d][n] + x - 1) / x;
						itr++;
					}
					Y[d][i][Ny[d][i] - 1] = W;
				}

			}

			if (p != P) {
				Nxall[p] = Nx;
				Xall[p].resize(Nx);
				rep(i, Nx)
					Xall[p][i] = X[i];
				scall[p] = BSscsum + Nx * 10 * D;
				if (scall[p] > stopth) {
					scall[p] = 0;
					stop = true;
				}
			}
		}
		mem_copy_all(0, D - 1, true);
		return;
	}
	int evaltmp1tmp(int d, int i) {
		int sc = 0;
		int L = X[i + 1] - X[i];
		int p = 0, q = 0;
		while (1) {
			if (p == Ny[d][i] - 1 && q == Ny[d + 1][i] - 1)
				break;
			if (Y[d][i][p + 1] < Y[d + 1][i][q + 1]) {
				sc -= L;
				p++;
			}
			else if (Y[d][i][p + 1] > Y[d + 1][i][q + 1]) {
				sc -= L;
				q++;
			}
			else {
				p++;
				q++;
			}
		}
		return sc;
	}
	void evaltmp1(int d, bool is_all = false) {
		int sc = 0;
		if (is_all) {
			rep(i, Nx - 1)
				sc1[d][i] = evaltmp1tmp(d, i);
		}
		else {
			rep(idx, 2) {
				int i = Xlist[idx];
				if (i == -1) continue;
				sc1[d][i] = evaltmp1tmp(d, i);
			}
		}
		return;
	}
	void evaltmp2(int d) {
		int nn = 0;
		int i = 0;
		rep(i, Nx - 1) {
			int x = X[i + 1] - X[i];
			rep(j, Ny[d][i] - 1) {
				int y = Y[d][i][j + 1] - Y[d][i][j];
				S[nn++] = x * y;
			}
		}
		sort(S.begin(), S.end()); // !
		int sc = 0;
		rep(n, N)
			sc -= 100 * max(0, input.A[d][n] - S[n]);
		sc2[d] = sc;
		return;
	}
	ll evalall(bool can_stop, bool is_all, int scmin = 0) {
		ll sc = 0;
		rep(d, D - 1) {
			evaltmp1(d, is_all);
			rep(i, Nx - 1)
				sc += (ll)sc1[d][i];
			if (can_stop && sc <= scmin)
				return -InfLL;
		}
		rep(d, D) {
			evaltmp2(d);
			sc += (ll)sc2[d];
			if (can_stop && sc <= scmin)
				return -InfLL;
		}
		return sc;
	}
};
Info info;

struct Output {
	int W, D, N;
	int ans;
	void resize_all(int W_, int D_, int N_) {
		W = W_;
		D = D_;
		N = N_;
	}
	void writeSolution(ostream& out = cout) {
		rep(d, D) {
			vector<tuple<int, int, int, int, int>> S(N);
			int nn = 0;
			int i = 0;
			while(1) {
				int inext = i + 1;
				int xfrom = info.X[i];
				int xto = info.X[inext];
				int x = xto - xfrom;
				rep(j, info.Ny[d][i] - 1) {
					int yfrom = info.Y[d][i][j];
					int yto = info.Y[d][i][j + 1];
					int y = yto - yfrom;
					S[nn++] = make_tuple(x * y, xfrom, yfrom, xto, yto);
				}
				i = inext;
				if (i == info.Nx - 1) break;
			}
			sort(S.begin(), S.end());
			rep(idx, N) {
				int i = get<1>(S[idx]);
				int j = get<2>(S[idx]);
				int ip = get<3>(S[idx]);
				int jp = get<4>(S[idx]);
				out << i << " " << j << " " << ip << " " << jp << "\n";
			}
		}
		return;
	}
};
Output output;

bool is_inside(int x, int y) {
	int W = input.W;
	if (x < 0) return false;
	if (y < 0) return false;
	if (x >= W) return false;
	if (y >= W) return false;
	return true;
}

void solve_init() {
	int W = input.W;
	int D = input.D;
	int N = input.N;
	output.resize_all(W, D, N);
	info.resize_all(W, D, N);
	rep(i, Gaussian_size)
		Gaussian_[i] = Gaussian();
	return;
}

void SA() {
	int W = input.W;
	int D = input.D;
	int N = input.N;
	ll sc = info.evalall(false, true);
	rep(d, D - 1) {
		rep(i, info.Nx - 1)
			info.sc1mem[d][i] = info.sc1[d][i];
	}
	rep(d, D)
		info.sc2mem[d] = info.sc2[d];


	double start_temp = 5000.0;
	double end_temp = 0.0;
	double TIME_LIMIT = 2970.0;
	ll loop_count = 0;
	double time_now = 0.0;
	double temp = 0.0;
	double time_r = 1.0;

	int from_mem[51];
	int to_mem[51];

	while (1) {
		//break;
		if (loop_count % 10000 == 0) {
			time_now = time_diff();
			if (time_now > TIME_LIMIT)
				break;

			ll scsum1 = 0, scsum2 = 0;
			rep(d, D) {
				if (d < D - 1) {
					rep(i, info.Nx - 1)
						scsum1 += (ll)info.sc1[d][i];
				}
				scsum2 += info.sc2[d];
			}
			double r = InfLL;
			if (scsum2 != 0)
				r = (double)scsum1 / (double)scsum2;
			if (r > 1.0)
				r = sqrt(r);
			else
				r = pow(r, 3.0);
			r = min(r, 25.0);
			r = max(r, 0.35);
			start_temp = 1000.0 / r;
			start_temp /= sqrt(N);
			start_temp *= 6.0;
			temp = start_temp + (end_temp - start_temp) * time_now / TIME_LIMIT;
			time_r = (TIME_LIMIT - time_now) / TIME_LIMIT;
		}

		loop_count++;
		ll scdiff = 0;
		int d = xor128() % D;

		int d0 = d;
		int d1 = d;

		int type = xor128() % 500;
		if (type < 300)
			type = 1;
		else if (type < 350)
			type = 2;
		else if (type < 400)
			type = 3;
		else if (type < 450)
			type = 4;
		else if (type < 453)
			type = 0;
		else if (type < 456)
			type = 5;
		else if (type < 475)
			type = 6;
		else
			type = 7;

		int is_all = false;
		info.Xlist[0] = -1;
		info.Xlist[1] = -1;
		if (type == 0) {
			// 横壁間の距離を増減させる
			//continue;
			is_all = true;
			if (info.Nx <= 2)
				continue;
			if (time_now < TIME_LIMIT * 0.5) continue;
			int ifrom = RAND_clamp(0, info.Nx - 1);
			int ito = RAND_clamp(0, info.Nx - 1);
			if (ifrom == ito) continue;
			int len = 1;
			int lenfrom = info.X[ifrom + 1] - info.X[ifrom];
			int lento = info.X[ito + 1] - info.X[ito];
			if (lenfrom <= len) continue;

			info.Xlist[0] = ifrom;
			info.Xlist[1] = ito;
			info.mem_copy_X();

			if (ifrom < ito) {
				for (int itmp = ifrom + 1; itmp <= ito; itmp++)
					info.X[itmp] -= len;
			}
			else {
				for (int itmp = ito + 1; itmp <= ifrom; itmp++)
					info.X[itmp] += len;
			}
		}
		else if (type == 1) {
			// 縦壁を適当に移動させる(確率0.5で前日または翌日に存在する座標と合わせる)
			//continue;
			int ifrom = RAND_clamp(0, info.Nx - 1);
			if (info.Ny[d][ifrom] <= 2) continue;
			int jfrom = RAND_clamp(1, info.Ny[d][ifrom] - 1);
			int ito = RAND_clamp(0, info.Nx - 1);
			if (xor128() % 3 == 0)
				ito = ifrom;
			int yold = info.Y[d][ifrom][jfrom];
			int ynew = RAND_clamp(1, W);
			int dto = d + (xor128() % 2) * 2 - 1;
			if (xor128() % 2 == 0) {
				dto = d - 1;
				if ((d == 0 || xor128() % 2 == 0) && d != D - 1)
					dto = d + 1;
				if (info.Ny[dto][ito] <= 2) continue;
				int j = RAND_clamp(1, info.Ny[dto][ito] - 1);
				ynew = info.Y[dto][ito][j];
			}
			if (ifrom == ito && yold == ynew) continue;

			int dlen = D;
			int dx = d - dto;
			d0 = d;
			d1 = d;
			rep(ddiff, dlen) {
				if (d1 < 0)
					break;
				if (d1 >= D)
					break;

				// 移動元に壁があるか確認
				int L = 0, R = info.Ny[d1][ifrom];
				while (1) {
					int M = (L + R) / 2;
					if (L == M) break;
					if (info.Y[d1][ifrom][M] <= yold)
						L = M;
					else
						R = M;
				}
				from_mem[d1] = L;
				if (info.Y[d1][ifrom][L] != yold)
					break;

				// 移動先に壁がないか確認
				L = 0, R = info.Ny[d1][ito];
				while (1) {
					int M = (L + R) / 2;
					if (L == M) break;
					if (info.Y[d1][ito][M] <= ynew)
						L = M;
					else
						R = M;
				}
				to_mem[d1] = L;
				if (info.Y[d1][ito][L] == ynew)
					break;
				d1 += dx;
			}
			if (d0 == d1) continue;

			d1 -= dx;
			if (d0 > d1) swap(d0, d1);

			info.Xlist[0] = ifrom;
			if (ifrom != ito)
				info.Xlist[1] = ito;
			info.mem_copy_all(d0, d1);

			for (d = d0; d <= d1; d++) {
				jfrom = from_mem[d];
				int L = to_mem[d];
				int jto = L + 1;
				if (ifrom == ito && ynew > yold)
					jto--;

				info.Ny[d][ifrom]--;
				for (int k = jfrom; k < info.Ny[d][ifrom]; k++)
					info.Y[d][ifrom][k] = info.Y[d][ifrom][k + 1];
				info.Ny[d][ito]++;
				for (int k = info.Ny[d][ito] - 1; k > jto; k--)
					info.Y[d][ito][k] = info.Y[d][ito][k - 1];
				info.Y[d][ito][jto] = ynew;
			}
		}
		else if (type == 2) {
			// 縦壁を適当に前後させる
			//continue;
			int i = RAND_clamp(0, info.Nx - 1);
			if (info.Ny[d][i] <= 2) continue;
			int j = RAND_clamp(1, info.Ny[d][i] - 1);

			int yfrom = info.Y[d][i][j - 1];
			int yold = info.Y[d][i][j];
			int yto = info.Y[d][i][j + 1];
			int ynew = RAND_clamp(yfrom + 1, yto);
			ynew = yold + (int)(Gaussian_[xor128() % Gaussian_size] * 5.0 + 0.5);
			if (yold == ynew)
				ynew = yold + (xor128() % 2) * 2 - 1;
			ynew = min(ynew, yto - 1);
			ynew = max(ynew, yfrom + 1);
			if (yold == ynew) continue;

			info.Xlist[0] = i;
			info.mem_copy_all(d0, d1);

			info.Y[d][i][j] = ynew;
		}
		else if (type == 3) {
			// 縦壁を隣の前日または翌日に存在する座標と合わせる
			//continue;
			int i = RAND_clamp(0, info.Nx - 1);
			if (info.Ny[d][i] <= 2) continue;
			int j = RAND_clamp(1, info.Ny[d][i] - 1);
			int dfrom = d - 1;
			if ((d == 0 || xor128() % 2 == 0) && d != D - 1)
				dfrom = d + 1;
			int yfrom = info.Y[d][i][j - 1];
			int yold = info.Y[d][i][j];
			int yto = info.Y[d][i][j + 1];
			int ifrom = i;

			int L = 0, R = info.Ny[dfrom][ifrom];
			while (1) {
				int M = (L + R) / 2;
				if (L == M) break;
				if (info.Y[dfrom][ifrom][M] <= yfrom)
					L = M;
				else
					R = M;
			}
			L++;
			R = L;
			while (1) {
				if (info.Y[dfrom][ifrom][R] >= yto)
					break;
				R++;
			}
			if (L == R) continue;
			int ynew = info.Y[dfrom][ifrom][RAND_clamp(L, R)];
			if (yold == ynew) continue;

			info.Xlist[0] = i;
			info.mem_copy_all(d0, d1);

			info.Y[d][i][j] = ynew;
		}
		else if (type == 4) {
			// ある行の区間yfrom<=y<=ytoに含まれる縦壁を同じ長さだけ適当に前後させる
			//continue;
			d0 = d;
			d1 = d0 + (int)(abs(Gaussian_[xor128() % Gaussian_size] * 1.0) + 0.5);
			if (d1 >= D) continue;
			int i = RAND_clamp(0, info.Nx - 1);
			if (info.Ny[d][i] <= 2) continue;
			int yfrom = RAND_clamp(1, W);
			int yto = RAND_clamp(1, W);
			if (yfrom > yto) swap(yfrom, yto);
			int len = (int)(Gaussian_[xor128() % Gaussian_size] * 5.0 + 0.5);
			if (len == 0) continue;

			bool can = true;
			int Ycount = 0;
			for (d = d0; d <= d1; d++) {
				int L = 0, R = info.Ny[d][i];
				while (1) {
					int M = (L + R) / 2;
					if (L == M) break;
					if (info.Y[d][i][M] < yfrom)
						L = M;
					else
						R = M;
				}
				from_mem[d] = L + 1;
				int jfromfrom = L;
				int jfrom = L + 1;


				L = 0, R = info.Ny[d][i];
				while (1) {
					int M = (L + R) / 2;
					if (L == M) break;
					if (info.Y[d][i][M] <= yto)
						L = M;
					else
						R = M;
				}
				to_mem[d] = L;
				int jto = L;
				int jtoto = L + 1;

				int yfromfrom = info.Y[d][i][jfromfrom];
				int ytoto = info.Y[d][i][jtoto];

				if (len > 0) {
					if (yto + len >= ytoto) {
						can = false;
						break;
					}
				}
				else {
					if (yfrom + len <= yfromfrom) {
						can = false;
						break;
					}
				}
				Ycount += (jto - jfrom + 1);
			}

			if (!can) continue;
			if (Ycount == 0) continue;

			info.Xlist[0] = i;
			info.mem_copy_all(d0, d1);

			for (d = d0; d <= d1; d++) {
				int jfromfrom = from_mem[d] - 1;
				int jfrom = from_mem[d];
				int jto = to_mem[d];
				int jtoto = to_mem[d] + 1;

				int yfromfrom = info.Y[d][i][jfromfrom];
				int ytoto = info.Y[d][i][jtoto];
				for (int j = jfrom; j <= jto; j++)
					info.Y[d][i][j] += len;
			}
		}
		else if (type == 5) {
			// ifrom, itoの縦線スワップ
			//continue;
			if (info.Nx != 3) continue;
			if (time_now < TIME_LIMIT * 0.9) continue;
			int d0 = d;
			int d1 = d;
			int ifrom = RAND_clamp(0, info.Nx - 1);
			int ito = RAND_clamp(0, info.Nx - 1);
			if (ifrom == ito) continue;

			info.Xlist[0] = ifrom;
			info.Xlist[1] = ito;
			info.mem_copy_all(d0, d1);
			for (d = d0; d <= d1; d++) {
				int Nymax = max(info.Ny[d][ifrom], info.Ny[d][ito]);
				swap(info.Ny[d][ifrom], info.Ny[d][ito]);
				rep(j, Nymax)
					swap(info.Y[d][ifrom][j], info.Y[d][ito][j]);
			}
		}
		else if (type == 6) {
			// 縦壁を1つだけ面積が等しくなるように移動させる
			//continue;
			int ifrom = RAND_clamp(0, info.Nx - 1);
			if (info.Ny[d][ifrom] <= 2) continue;
			int jfrom = RAND_clamp(1, info.Ny[d][ifrom] - 1);
			int ito = RAND_clamp(0, info.Nx - 1);
			int jto = RAND_clamp(0, info.Ny[d][ito] - 1);
			if (ifrom == ito && jto + 1 == jfrom) continue;
			if (ifrom == ito && jto == jfrom) continue;
			//if (xor128() % 3 == 0)
				//ito = ifrom;
			int yfromprev = info.Y[d][ifrom][jfrom - 1];
			int yold = info.Y[d][ifrom][jfrom];
			int yfromnext = info.Y[d][ifrom][jfrom + 1];
			int ytoprev = info.Y[d][ito][jto];
			int ytonext = info.Y[d][ito][jto + 1];
			int lenfrom = info.X[ifrom + 1] - info.X[ifrom];
			int lento = info.X[ito + 1] - info.X[ito];
			int s1 = (yold - yfromprev) * lenfrom;
			int s2 = (yfromnext - yold) * lenfrom;
			//int s3 = (ytonext - ytoprev) * lento;
			int ynew = ytoprev;
			if (xor128() % 2 == 0)
				ynew += (s1 + lento - 1) / lento;
			else
				ynew += (s2 + lento - 1) / lento;
			if (ytoprev >= ynew) continue;
			if (ynew >= ytonext) continue;
			if (yold == ynew) continue;


			info.Xlist[0] = ifrom;
			if (ifrom != ito)
				info.Xlist[1] = ito;
			info.mem_copy_all(d0, d1);

			if (ifrom == ito && jto > jfrom)
				jto--;
			info.Ny[d][ifrom]--;
			for (int k = jfrom; k < info.Ny[d][ifrom]; k++)
				info.Y[d][ifrom][k] = info.Y[d][ifrom][k + 1];
			info.Ny[d][ito]++;
			for (int k = info.Ny[d][ito] - 1; k > jto + 1; k--)
				info.Y[d][ito][k] = info.Y[d][ito][k - 1];
			info.Y[d][ito][jto + 1] = ynew;
		}
		else if (type == 7) {
			// 面積変更なし、縦線移動
			//continue;
			if (time_now < TIME_LIMIT * 0.5) continue;
			int ifrom = RAND_clamp(0, info.Nx - 1);
			if (info.Ny[d][ifrom] <= 2) continue;
			int jfrom = RAND_clamp(1, info.Ny[d][ifrom] - 1);
			int yfromprev = info.Y[d][ifrom][jfrom - 1];
			int yold = info.Y[d][ifrom][jfrom];
			int yfromnext = info.Y[d][ifrom][jfrom + 1];
			int ynew = yfromprev + yfromnext - yold;
			if (ynew == yold) continue;


			info.Xlist[0] = ifrom;
			info.mem_copy_all(d0, d1);

			info.Y[d][ifrom][jfrom] = ynew;
		}

		double R = RAND_();
		int scth = (int)(temp * log(R)) - 1;
		bool is_out = false;
		if (is_all) {
			ll sctmp = info.evalall(true, false, sc + scth);
			scdiff = sctmp - sc;
			if (sctmp == InfLL)
				is_out = true;
		}
		else {
			for (int dd = d0 - 1; dd <= d1; dd++) {
				if (dd == -1) continue;
				if (dd == D - 1) continue;
				rep(idx, 2) {
					int i = info.Xlist[idx];
					if (i == -1) continue;
					scdiff -= (ll)info.sc1[dd][i];
				}
			}

			if (type != 7) {
				for (int dd = d0; dd <= d1; dd++)
					scdiff -= (ll)info.sc2[dd];
			}
			for (int dd = d0 - 1; dd <= d1; dd++) {
				if (dd == -1) continue;
				if (dd == D - 1) continue;
				info.evaltmp1(dd);
				rep(idx, 2) {
					int i = info.Xlist[idx];
					if (i == -1) continue;
					scdiff += (ll)info.sc1[dd][i];
				}
				if (scdiff <= scth) {
					is_out = true;
					break;
				}
			}
			if (!is_out && type != 7) {
				for (int dd = d0; dd <= d1; dd++) {
					info.evaltmp2(dd);
					scdiff += (ll)info.sc2[dd];
					if (scdiff <= scth) {
						is_out = true;
						break;
					}
				}
			}
		}
		if (!is_out && scdiff > scth)
			sc += scdiff;
		else {
			if (is_all)
				info.mem_back_X();
			else
				info.mem_back_all(d0, d1);
		}
	}
	return;
}

int main(int argc, char* argv[]) {
	if (argc >= 2) file_No = argv[1];
	timespec_get(&timeini, TIME_UTC);
	if (debug)
		debug_init();
	debug ? input.readProblem(fin) : input.readProblem();
	solve_init();
	SA();
	debug ? output.writeSolution(fout) : output.writeSolution();
	if (debug)
		debug_end();
	return 0;
}
