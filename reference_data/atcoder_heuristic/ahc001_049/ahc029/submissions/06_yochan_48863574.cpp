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
struct timespec timeA;
double timeB;
string file_No = "0000";
ifstream fin;
ofstream fout;
ofstream fout_debug;
ofstream fout_score;
int Gaussian_size = 1000000;
vector<double> Gaussian_(Gaussian_size);

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
	// time_ratio = 0.01;
	//time_ratio /= 2.0;
}

void debug_end() {
	fin.close();
	fout.close();
	fout_debug.close();
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
ll pow2[22];
struct Input {
	int N, M, K, T;
	void readProblem(istream& in = cin) {
		cin >> N >> M >> K >> T;
		return;
	}
};

Input input;

struct Card {
	int t;
	ll w;
	ll p = 0;
};
struct Project {
	ll h;
	ll v;
};

struct Info {
	Card cards[7];
	Project projects[8];
	ll money = 0;
	int L = 0;
	int last_used = -1;
	void resize_all(vector<Card>& cards_, vector<Project>& projects_, ll money_, int L_) {
		rep(n, input.N)
			cards[n] = cards_[n];
		rep(m, input.M)
			projects[m] = projects_[m];
		money = money_;
		L = L_;
		return;
	}
	void add_card(Card c, bool p2 = false) {
		ll p = c.p;
		if (p2) {
			p *= pow2[L];
			c.w *= pow2[L];
		}
		money -= p;
		c.p = 0;
		cards[last_used] = c;
		return;
	}
	void use(int c, int mtmp) {
		Card card = cards[c];
		if (card.t == 0) {
			projects[mtmp].h -= card.w;
			if (projects[mtmp].h <= 0) {
				projects[mtmp].h = 0;
				money += projects[mtmp].v;
			}
		}
		else if (card.t == 1) {
			rep(m, input.M) {
				projects[m].h -= card.w;
				if (projects[m].h <= 0) {
					projects[m].h = 0;
					money += projects[m].v;
				}
			}
		}
		else if (card.t == 2) {
			projects[mtmp].h = 0;
		}
		else if (card.t == 3) {
			rep(m, input.M)
				projects[m].h = 0;
		}
		else
			L++;
		last_used = c;
		return;
	}
};

Info info;

struct Param_est {
	vd xmax = { 20.0, 10.0, 10.0, 5.0, 3.0 };
	vd X = { 10.5, 5.5, 5.5, 3.0, 2.0 };
	vd x_raw = { 10.5, 5.5, 5.5, 3.0, 2.0 };
	vd x = { 10.5, 5.5, 5.5, 3.0, 2.0 };
	vi xcount = { 0, 0, 0, 0, 0 };
	double delta = 100000.0;
	double eps = 0.001;
	void update(int t) {
		int Scount = 0;
		rep(i, 5)
			Scount += xcount[i];
		rep(loop, 1000) {
			double S = 0;
			rep(i, 5)
				S += x_raw[i];
			double Ltmp = 0.0;
			rep(i, 5)
				Ltmp += log10(x_raw[i] / S) * (double)xcount[i] / (double)Scount;

			int i = loop % 5;
			vd xnew = x_raw;
			xnew[i] += eps;
			double Snew = S + eps;
			double Lnew = 0.0;
			rep(i, 5)
				Lnew += log10(xnew[i] / Snew) * (double)xcount[i] / (double)Scount;
			double diff = (Lnew - Ltmp) * delta;
			diff = max(diff, 1.0 - x_raw[i]);
			diff = min(diff, xmax[i] - x_raw[i]);
			x_raw[i] += diff;
			S += diff;
		}
		double r = (double)(t * input.K) / 100.0;
		r = min(r, 1.0);
		rep(i, 5)
			x[i] = (1 - r) * X[i] + r * x_raw[i];
		return;
	}
};

Param_est pe;

void solve_init() {
	int N = input.N;
	int M = input.M;
	vector<Card> cards_init(N);
	vector<Project> projects_init(M);
	rep(n, N) {
		cin >> cards_init[n].t >> cards_init[n].w;
		pe.xcount[cards_init[n].t]++;
	}
	rep(m, M)
		cin >> projects_init[m].h >> projects_init[m].v;
	info.resize_all(cards_init, projects_init, 0, 0);
	pow2[0] = 1;
	rep(i, 21)
		pow2[i + 1] = pow2[i] * 2LL;
	rep(i, Gaussian_size)
		Gaussian_[i] = Gaussian();
	timeB = time_diff_new();
	return;
}

Card cards[20][10];
Project projects[40];

void solve(int t, int type) {
	// type = 0: カード実行時
	// type = 1: カード補充時
	int N = input.N;
	int M = input.M;
	int K = input.K;
	int T = input.T;
	int NMK = N * M * K;
	vector<tuple<ll, int, int>> evalsort;
	vector<Card> cards_new(K);// type=1
	double rk = 0.0;
	if (N == 7)
		rk = 1.0;
	else if (N == 6)
		rk = 0.25;
	else if (N == 5)
		rk = 0.0625;
	else if (N == 4)
		rk = 0.0625 * 0.25;
	else if (N == 3)
		rk = 0.0625 * 0.25 * 0.25;
	else
		rk = 0.0625 * 0.25 * 0.25 * 0.25;
	rk *= 4.0;
	if (type == 0) {
		ll w1min = InfLL;
		rep(ntmp, N) {
			if (info.cards[ntmp].t == 1)
				w1min = max(w1min, info.cards[ntmp].w);
		}
		bool do1 = true;
		rep(mtmp, M) {
			if (info.projects[mtmp].h > info.projects[mtmp].v)
				do1 = false;
			if (info.projects[mtmp].h < w1min)
				do1 = false;
		}
		rep(n, N) {
			rep(m, M) {
				// 制約
				if (info.cards[n].t == 1 && m >= 1) continue;
				if (info.cards[n].t == 3 && m >= 1) continue;
				if (info.cards[n].t == 4 && m >= 1) continue;
				if (info.cards[n].t == 4 && info.L >= 20) continue;
				// 枝刈り
				if (info.cards[n].t == 0) {
					if (do1) continue;
				}
				if (info.cards[n].t == 0) {
					ll Hmin = InfLL / 10LL;
					rep(mtmp, M) {
						if (info.projects[mtmp].h > info.projects[mtmp].v) continue;
						if (info.projects[mtmp].h < info.cards[n].w) continue;
						if (Hmin > info.projects[mtmp].h)
							Hmin = info.projects[mtmp].h;
					}
					if (info.projects[m].h * 10LL > Hmin * 15LL) // より小さいpjがある
						continue;
				}
				if (info.cards[n].t == 0) {
					ll Hmin = InfLL / 10LL;
					rep(mtmp, M) {
						if (Hmin > info.projects[mtmp].h)
							Hmin = info.projects[mtmp].h;
					}
					if (info.cards[n].w < Hmin && info.projects[m].h * 10LL > Hmin * 15LL) // より小さいpjがある
						continue;
				}
				if (info.cards[n].t == 2) {
					bool can = true;
					rep(mtmp, M) {
						if (info.projects[mtmp].h > info.projects[m].h && info.projects[mtmp].v < info.projects[m].v) // 完全下位互換のpjがある
							can = false;
					}
					if (!can) continue;
				}
				evalsort.push_back(make_tuple(0, n, m));
			}
		}
	}
	else {
		cin >> info.money;
		rep(ktmp, K) {
			cin >> cards_new[ktmp].t >> cards_new[ktmp].w >> cards_new[ktmp].p;
			pe.xcount[cards_new[ktmp].t]++;
			// 制約
			if (info.money < cards_new[ktmp].p) continue;
			if (cards_new[ktmp].t == 4 && info.L >= 20) continue;
			// 枝刈り
			int num3 = 0;
			rep(ntmp, N) {
				if (info.last_used == ntmp) continue;
				if (info.cards[ntmp].t == 3)
					num3++;
			}
			if (cards_new[ktmp].t <= 1) {
				bool can = true;
				rep(ktmptmp, K) {
					if (cards_new[ktmptmp].t == cards_new[ktmp].t && cards_new[ktmptmp].p < cards_new[ktmp].p && cards_new[ktmptmp].w > cards_new[ktmp].w) // 完全上位互換のcdがある
						can = false;
				}
				if (!can) continue;
			}
			if (cards_new[ktmp].t == 3) {
				if (num3 >= 1)
					continue;
			}
			evalsort.push_back(make_tuple(0, ktmp, 0));
		}
		pe.update(t);
	}
	int E = evalsort.size();
	int Emax = N * M;
	if (type == 1)
		Emax = K;
	int loop_count = 0;
	int loop1 = 0, loop2 = 0;
	int Tlen = 8;
	if (N * M >= 30)
		Tlen = 6;
	else if (N * M >= 12)
		Tlen = 7;
	int Plen = Tlen * 2;
	while (1) {
		int Etmp = evalsort.size();
		rep(loop, 5) {
			loop_count++;
			rep(ttmp, Tlen) {
				rep(ktmp, K) {
					// 乱数生成
					loop2++;
					if (ktmp == 0) {
						cards[ttmp][ktmp].t = 0;
						cards[ttmp][ktmp].w = 1;
						cards[ttmp][ktmp].p = 0;
						continue;
					}
					double S = 0.0;
					rep(i, 5)
						S += pe.x[i];
					double r = RAND_() * S;
					double Stmp = 0.0;
					cards[ttmp][ktmp].t = 4;
					rep(i, 5) {
						Stmp += pe.x[i];
						if (Stmp >= r) {
							cards[ttmp][ktmp].t = i;
							break;
						}
					}
					if (cards[ttmp][ktmp].t == 0) {
						cards[ttmp][ktmp].w = xor128() % 50 + 1;
						cards[ttmp][ktmp].p = (ll)((double)cards[ttmp][ktmp].w * (1.0 + Gaussian_[xor128() % Gaussian_size] / 3.0) + 0.5);
						cards[ttmp][ktmp].p = min(cards[ttmp][ktmp].p, 10000LL);
						cards[ttmp][ktmp].p = max(cards[ttmp][ktmp].p, 1LL);
					}
					else if (cards[ttmp][ktmp].t == 1) {
						cards[ttmp][ktmp].w = xor128() % 50 + 1;
						cards[ttmp][ktmp].p = (ll)((double)M * (double)cards[ttmp][ktmp].w * (1.0 + Gaussian_[xor128() % Gaussian_size] / 3.0) + 0.5);
						cards[ttmp][ktmp].p = min(cards[ttmp][ktmp].p, 10000LL);
						cards[ttmp][ktmp].p = max(cards[ttmp][ktmp].p, 1LL);
					}
					else if (cards[ttmp][ktmp].t == 2) {
						cards[ttmp][ktmp].w = 0;
						cards[ttmp][ktmp].p = xor128() % 11;
					}
					else if (cards[ttmp][ktmp].t == 3) {
						cards[ttmp][ktmp].w = 0;
						cards[ttmp][ktmp].p = xor128() % 11;
					}
					else {
						cards[ttmp][ktmp].w = 0;
						cards[ttmp][ktmp].p = xor128() % 801 + 200;
					}
				}
			}
			rep(i, Plen) {
				loop2++;
				double b = RAND_() * 6.0 + 2.0;
				projects[i].h = (ll)(pow(2.0, b) + 0.5);
				double p = b + Gaussian_[xor128() % Gaussian_size] * 0.5;
				p = max(p, 0.0);
				p = min(p, 10.0);
				projects[i].v = (ll)(pow(2.0, p) + 0.5);
			}
			rep(e, Etmp) {
				ll evaltmp = get<0>(evalsort[e]);
				int n = get<1>(evalsort[e]);
				int k = get<1>(evalsort[e]);
				int m = get<2>(evalsort[e]);
				Info infotmp = info;
				int idx_project = 0;
				for (int ttmp = t; ttmp < min(t + Tlen, T); ttmp++) {
					// erase_card & erase_projects
					if (type == 0 && ttmp == t)
						infotmp.use(n, m);
					else if (type == 0 || ttmp > t) {
						int nmax = -1;
						int mmax = -1;
						ll dmax = -InfLL;
						int stop = -1;
						bool is_stop[8];
						ll evalstop = -InfLL;
						int stop_num = 0;
						bool is_short = false;
						int hargmin = 0;
						ll hmin = InfLL;
						int hargmax = 0;
						ll hmax = -InfLL;
						ll hvdiff = 0;
						ll havg = 0;
						rep(mtmp, M) {
							havg += infotmp.projects[mtmp].h;
							ll evalstoptmp = infotmp.projects[mtmp].h - infotmp.projects[mtmp].v;
							hvdiff -= evalstoptmp;
							if (evalstop < evalstoptmp) {
								evalstop = evalstoptmp;
								stop = mtmp;
							}
							if (evalstoptmp > 0) {
								stop_num++;
								is_stop[mtmp] = true;
							}
							else
								is_stop[mtmp] = false;
						}
						havg /= (ll)M;
						rep(mtmp, M) {
							loop1++;
							if (is_stop[mtmp]) continue;
							if (hmax < infotmp.projects[mtmp].h) {
								hmax = infotmp.projects[mtmp].h;
								hargmax = mtmp;
							}
							if (hmin > infotmp.projects[mtmp].h) {
								hmin = infotmp.projects[mtmp].h;
								hargmin = mtmp;
							}
						}
						ll money_est = infotmp.money;
						rep(ntmp, N) {
							loop1++;
							if (infotmp.cards[ntmp].t == 0)
								money_est += infotmp.cards[ntmp].w;
							if (infotmp.cards[ntmp].t == 1)
								money_est += infotmp.cards[ntmp].w * (ll)M;
						}
						if (money_est < hmin)
							is_short = true; // 資金不足
						rep(ntmp, N) {
							loop1++;
							ll dtmp = -InfLL;
							int mtmp = 0;
							if (infotmp.cards[ntmp].t == 0) {
								mtmp = hargmin;
								if (infotmp.cards[ntmp].w <= infotmp.projects[mtmp].h)
									dtmp = infotmp.cards[ntmp].w;
								else
									dtmp = (infotmp.projects[mtmp].h * 3 - infotmp.cards[ntmp].w) / 2;
							}
							else if (infotmp.cards[ntmp].t == 1) {
								dtmp = infotmp.cards[ntmp].w * (ll)(M - stop_num);
								dtmp += InfLL / 10;
							}
							else if (infotmp.cards[ntmp].t == 2) {
								if (stop_num == 0)
									dtmp = -InfLL / 2;
								else
									dtmp = InfLL - 2;
								mtmp = stop;
								if (stop == -1)
									mtmp = 0;
							}
							else if (infotmp.cards[ntmp].t == 3) {
								if (hvdiff < 0)
									dtmp = InfLL - 1;
								else
									dtmp = InfLL / 2;
								dtmp = InfLL - 1;
							}
							else if (infotmp.cards[ntmp].t == 4) {
								dtmp = InfLL;
							}
							if (dmax < dtmp) {
								dmax = dtmp;
								nmax = ntmp;
								mmax = mtmp;
							}
						}
						infotmp.use(nmax, mmax);
					}

					// insert_projects
					rep(mtmp, M) {
						if (infotmp.projects[mtmp].h == 0) {
							infotmp.projects[mtmp] = projects[idx_project % Plen];
							infotmp.projects[mtmp].h *= pow2[infotmp.L];
							infotmp.projects[mtmp].v *= pow2[infotmp.L];
							idx_project++;
						}
					}

					// insert_cards

					if (type == 1 && ttmp == t) {
						infotmp.add_card(cards_new[k], false);
					}
					else {
						int num_2 = 0;
						rep(ntmp, N) {
							if (ntmp == infotmp.last_used) continue;
							if (infotmp.cards[ntmp].t == 2)
								num_2++;
						}
						int num_3 = 0;
						rep(ntmp, N) {
							if (ntmp == infotmp.last_used) continue;
							if (infotmp.cards[ntmp].t == 3)
								num_3++;
						}
						ll hvdiff = 0;
						ll hmax = -InfLL;
						int stop_num = 0;
						rep(mtmp, M) {
							hmax = max(hmax, infotmp.projects[mtmp].h);
							ll evalstoptmp = infotmp.projects[mtmp].h - infotmp.projects[mtmp].v;
							hvdiff -= evalstoptmp;
							if (evalstoptmp > 0)
								stop_num++;
						}

						int kmax = -1;
						ll fmax = -InfLL;
						rep(ktmp, K) {
							ll p_ = cards[ttmp - t][ktmp].p;
							ll w_ = cards[ttmp - t][ktmp].w;
							int t_ = cards[ttmp - t][ktmp].t;
							if (infotmp.money < p_ * pow2[infotmp.L]) continue;
							ll ftmp = 0;
							if (t_ == 0) {
								ftmp = w_ - p_;
								//if (w_ > hmax / pow2[infotmp.L])
									//ftmp -= (ll)((double)(w_ - hmax / pow2[infotmp.L]) / (1.0 + rk * 16.0) + 0.5);
							}
							else if (t_ == 1) {
								ftmp = -p_;
								rep(mtmp, M) {
									ftmp += w_;
									if (w_ > infotmp.projects[mtmp].h / pow2[infotmp.L]) {
										ftmp -= (ll)((double)(w_ - infotmp.projects[mtmp].h / pow2[infotmp.L]) / (1.0 + rk) + 0.5);
									}
								}
							}
							else if (t_ == 2) {
								if (num_2 >= 1 && stop_num == 0)
									ftmp = -1000000 - p_;
								else
									ftmp = -p_;
							}
							else if (t_ == 3) {
								if (num_3 >= 1 || hvdiff >= 0)
									ftmp = -InfLL;
								else
									ftmp = InfLL;
							}
							else
								ftmp = -InfLL; // 一律禁止
							if (fmax < ftmp) {
								fmax = ftmp;
								kmax = ktmp;
							}
						}
						infotmp.add_card(cards[ttmp - t][kmax], true);
					}
				}
				evaltmp += infotmp.money;
				int Tend = max(N, M);
				Tend = max(Tend, K);
				if (t < T - Tlen - Tend) {
					double rp = 0.89;
					double rt = 1.0; // 終盤は盤面評価が減衰する
					if (t >= T - Tlen - 2 * Tend)
						rt = 1.0 - (double)(t - (T - Tlen - 2 * Tend)) / (double)(Tend);
					ll hmin = InfLL;
					ll hsum = 0;
					ll hmax = 0;
					rep(mtmp, M) {
						hmin = min(hmin, infotmp.projects[mtmp].h);
						hmin = max(hmax, infotmp.projects[mtmp].h);
						hsum += infotmp.projects[mtmp].h;
					}
					double money_est = infotmp.money;
					rep(ntmp, N) {
						if (infotmp.cards[ntmp].t == 0)
							money_est += infotmp.cards[ntmp].w;
						if (infotmp.cards[ntmp].t == 1)
							money_est += infotmp.cards[ntmp].w * (ll)M / 2LL;
					}
					money_est /= (double)pow2[infotmp.L];
					money_est = max(1.0, money_est);
					evaltmp -= (ll)((double)hmin * (4.0 / money_est) + 0.5) * rt;


					rep(ntmp, N) {
						if (infotmp.cards[ntmp].t == 0) {
							evaltmp += infotmp.cards[ntmp].w * rt;
							//if (infotmp.cards[ntmp].w > hmax)
								//evaltmp -= (ll)((double)(infotmp.cards[ntmp].w - hmax) * rt / (1.0 + rk * 16.0) + 0.5);
						}
						else if (infotmp.cards[ntmp].t == 1) {
							rep(mtmp, M) {
								evaltmp += (ll)infotmp.cards[ntmp].w * rt;
								//if (infotmp.cards[ntmp].w > infotmp.projects[mtmp].h)
									//evaltmp -= (ll)((double)(infotmp.cards[ntmp].w - infotmp.projects[mtmp].h) * rt / (1.0 + rk) + 0.5);
							}
						}
					}
					rep(mtmp, M) {
						ll evaltmptmp = infotmp.projects[mtmp].v * rp - infotmp.projects[mtmp].h;
						evaltmptmp = max(evaltmptmp, 0LL * infotmp.L);
						evaltmp += evaltmptmp * rt;
					}

					if (type == 0) {
						if (info.cards[n].t == 4)
							evaltmp = InfLL;
					}
					else {
						if (cards_new[k].t == 4) {
							double r4 = 0.0; // 増資カードのレア度
							rep(i, 5)
								r4 += pe.x[i];
							r4 /= pe.x[4];
							double R = 800.0; // 他のカードとの優先度比較
							rep(ktmp, K) {
								if (cards_new[ktmp].t == 0) {
									if (cards_new[ktmp].w < cards_new[ktmp].p) continue;
									double r = (cards_new[ktmp].w - cards_new[ktmp].p) / pow2[info.L];
									R = max(R, 800.0 + r * 1.0);
								}
								if (cards_new[ktmp].t == 1) {
									if (cards_new[ktmp].w * (ll)M < cards_new[ktmp].p) continue;
									double r = (cards_new[ktmp].w * (ll)M - cards_new[ktmp].p) / pow2[info.L];
									R = max(R, 800.0 + r * 1.0);
								}
							}
							double RR = pow((double)info.L, 0.9) * 0.9; // 増資間隔
							if (t < T / 2) RR = 10000000000.0;
							if (T - t > ((double)cards_new[k].p / ((double)pow2[info.L] * RR)) && (double)(info.money - cards_new[k].p) / ((double)pow2[info.L] / r4) > R)
								evaltmp = InfLL - cards_new[k].p;
							else
								evaltmp = -InfLL;
						}
					}
				}
				if (type == 0)
					evalsort[e] = make_tuple(evaltmp, n, m);
				else
					evalsort[e] = make_tuple(evaltmp, k, 0);
			}
		}
		sort(evalsort.begin(), evalsort.end(), greater<tuple<ll, int, int>>());
		double time_now = time_diff_new() - timeB;
		double t_ = (double)t;
		if (type == 1)
			t_ += 0.7;
		double t__ = t_;
		if (type == 0)
			t__ += 0.7;
		else
			t__ += 0.3;
		double time_begin = ((double)(t_) / 1000.0) * (1980.0 - timeB);
		double time_end = ((double)(t__) / 1000.0) * (1980.0 - timeB);
		double r = (time_now - time_begin) / (time_end - time_begin);
		if (get<0>(evalsort[0]) >= InfLL / 2LL) break;
		if (r >= 1.0)
			break;
		else {
			double logE = log2((double)E);
			int Emax = (int)(pow(0.5, (r - 0.3) * logE) * (double)E + 0.5);
			Emax = max(Emax, 2);
			if (Etmp > Emax) {
				rep(i, Etmp - Emax)
					evalsort.pop_back();
			}
		}
	}
	int n = get<1>(evalsort[0]);
	int k = get<1>(evalsort[0]);
	int m = get<2>(evalsort[0]);
	if (type == 0) {
		info.use(n, m);
		cout << n << " " << m << endl;
		rep(mtmp, M) {
			Project project;
			cin >> project.h >> project.v;
			info.projects[mtmp] = project;
		}
	}
	else {
		info.add_card(cards_new[k]);
		cout << k << endl;
	}
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
	rep(t, input.T) {
		solve(t, 0);
		solve(t, 1);
	}
	fout_score << info.money << endl;
	if (debug)
		debug_end();
	return 0;
}
