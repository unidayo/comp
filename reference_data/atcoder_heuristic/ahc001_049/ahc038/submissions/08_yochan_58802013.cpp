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
string file_No = "0000";
string Vstring = "_all";
ifstream fin;
ofstream fout;
ofstream fout_debug;
ofstream fout_score;
struct timespec timeini;

const vi dx = { 0, 1, 0, -1, 0 }; // RDLU.
const vi dy = { 1, 0, -1, 0, 0 }; // RDLU.
vector<unsigned long long> Shash, Thash, Vleafhash, Khash;
const vector<vi> dfromto = { {0,0,1,0},{0,1,1,0},{1,1,2,2},{0,0,2,3} };

void debug_init() {
	string txt = ".txt";

	string ifname = "in" + Vstring + "\\";
	ifname = ifname + file_No + txt;
	fin.open(ifname);
	string ofname = "out" + Vstring + "\\";
	ofname = ofname + file_No + txt;
	fout.open(ofname);
	string ofname_debug = "debug" + Vstring + "\\";
	ofname_debug = ofname_debug + file_No + txt;
	fout_debug.open(ofname_debug);
	string ofname_score = "score" + Vstring + "\\";
	ofname_score = ofname_score + file_No + txt;
	fout_score.open(ofname_score);

	time_ratio = 0.88; // 5950X
	// time_ratio = 0.88; // 12700KF
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

int xor128() {
	static int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
	int t = (x ^ (x << 11));
	x = y; y = z; z = w;
	return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

struct Input {
	int N, M, V;
	vector<vb> S, T;
	vector<pii> S_to_xy;
	vector<pii> T_to_xy;
	vector<vi> xy_to_S;
	vector<vi> xy_to_T;
	vector<vi> x_to_S;
	vector<vi> y_to_S;
	vector<vi> x_to_T;
	vector<vi> y_to_T;

	void readProblem(istream& in = cin) {
		in >> N >> M >> V;
		S.resize(N, vb(N));
		T.resize(N, vb(N));
		xy_to_S.resize(N, vi(N, -1));
		xy_to_T.resize(N, vi(N, -1));
		x_to_S.resize(N);
		y_to_S.resize(N);
		x_to_T.resize(N);
		y_to_T.resize(N);
		int Mnew = 0;
		rep(i, N) {
			rep(j, N) {
				char c;
				in >> c;
				S[i][j] = (c == '1');
			}
		}
		rep(i, N) {
			rep(j, N) {
				char c;
				in >> c;
				T[i][j] = (c == '1');
				if (S[i][j] && T[i][j]) {
					S[i][j] = false;
					T[i][j] = false;
				}
				else if (T[i][j])
					Mnew++;
			}
		}

		rep(i, N) {
			rep(j, N) {
				if (S[i][j])
					S_to_xy.push_back(make_pair(i, j));
			}
		}
		rep(i, N) {
			rep(j, N) {
				if (T[i][j])
					T_to_xy.push_back(make_pair(i, j));
			}
		}
		M = Mnew;
		rep(s, M) {
			int x = S_to_xy[s].first;
			int y = S_to_xy[s].second;
			xy_to_S[x][y] = s;
			x_to_S[x].push_back(s);
			y_to_S[y].push_back(s);
		}
		rep(t, M) {
			int x = T_to_xy[t].first;
			int y = T_to_xy[t].second;
			xy_to_T[x][y] = t;
			x_to_T[x].push_back(t);
			y_to_T[y].push_back(t);
		}
		return;
	}
};

struct Output {
	int V;
	vi P, L;
	int X, Y;
	vector<string> S;
	int T;
	void writeSolution(ostream& out = cout) {
		out << V << "\n";
		rep(v, V) {
			if (v == 0) continue;
			out << P[v] << " " << L[v] << "\n";
		}
		out << X << " " << Y << "\n";
		T = S.size();
		int sc = T;
		if (T == 1) sc = 10000;
		fout_score << sc << endl;
		rep(t, T)
			out << S[t] << "\n";

		return;
	}
};

Input input;
Output output;

struct BSinfo {
	vb S_picked;
	vb T_put;
	vi D;
	vb is_holding;
	unsigned long long hash = 0;
	int scoreA = 0;
	int scoreB = 0;
	void init(int N, int M, int V) {
		S_picked.resize(M, false);
		T_put.resize(M, false);
		D.resize(V, 0);
		is_holding.resize(V, false);
	}
};

struct BSdiffinfo {
	vi S_picked;
	vi T_put;
	vi D;
	vb is_holding;
	unsigned long long hash = 0;
	int scoreA = 0;
	int scoreB = 0;
	void init(int N, int M, int V) {
		D.resize(V, 0);
		is_holding.resize(V, false);
	}
};

struct Info {
	int N, M, V, N2;
	int Xm, Ym;
	vi LengthJoint, LengthLeaf; // 関節まで、指先までの腕の長さ
	int KNum = 1; // ロボットアームの姿勢の種類
	vector<vi> K2Angle; // K2Angle[k][vjoint]: 姿勢kの表す根の座標とロボットアームの全ての関節
	int Vjoint, Vleaf; // Vjoint: 関節数(根除く), Vleaf: 指先数
	vector<vector<vi>> K2PosX, K2PosY; // K2PosX[k][vleaf][dleaf]
	vector<vector<vi>> kvd2s; // kvd2s[k][vleaf][dleaf] = s or -1
	vector<vector<vi>> kvd2t; // kvd2t[k][vleaf][dleaf] = t or -1
	vector<vector<vector<vector<pii>>>> sv2kd; // sv2kd[droot][s][vleaf] = (k, dleaf)
	vector<vector<vector<vector<pii>>>> tv2kd; // tv2kd[droot][t][vleaf] = (k, dleaf)
	vector<vector<vi>> droot2nei; // droot2nei[droot][dist] = dxy
	vi smemo, tmemo;
	int smemoNum = 0;
	int tmemoNum = 0;
	int BSsizemax = 0;
	int BSsizemax2 = 0;
	vector<BSinfo> BSans;
	vector<pii> S_to_S;
	vector<pii> T_to_T;
	vector<pii> V_to_V;
	vector<vi> D_to_D;
	vi evalS, evalT;

	int scpick = 5000;
	int scput = 10000;
	int RL = 1900;
	int Radj = 50;
	double Rst = 150.0;

	bool can_one_turn(int dfrom, int dto) {
		if (dfrom == -1) return 1; // 方向不定
		if (dto == -1) return 1; // 方向不定
		return abs(dfrom - dto) != 2;
	}
	pii d_to_xy(int d) {
		return make_pair(d / N, d % N);
	}
	int xy_to_d(int x, int y) {
		return x * N + y;
	}
	int dist_xy(int sxy, int txy) {
		int sx = sxy / N;
		int sy = sxy % N;
		int tx = txy / N;
		int ty = txy % N;
		return abs(sx - tx) + abs(sy - ty);
	}

	int Calc_cost(vi& D, int k, int vleaf, int dleaf) {
		// 根,関節,vleafの判定(vleaf以外の指先は対象外)
		// D[0]: 根, D[1]...D[Vjoint]: 関節, D[Vjoint+1]...D[V-1]: 指先
		// k: 根&関節
		int ktmp = k;
		int sxy = ktmp % N2;
		ktmp /= N2;
		int dist = dist_xy(sxy, D[0]);
		if (dist >= 2) return dist;
		rep(vjoint, Vjoint) {
			int d = ktmp % 4;
			ktmp /= 4;
			if (!can_one_turn(D[vjoint + 1], d)) return 2;
		}
		if (!can_one_turn(D[Vjoint + 1 + vleaf], dleaf)) return 2;
		return 1;
	}

	vi ktoD(int k) {
		int ktmp = k;
		vi K2Angletmp;
		int dr = ktmp % N2;
		ktmp /= N2;
		K2Angletmp.push_back(dr); // 根
		rep(vjoint, Vjoint) {
			int d = ktmp % 4;
			ktmp /= 4;
			K2Angletmp.push_back(d); // 関節
		}
		return K2Angletmp;
	}

	bool is_inside(int x, int y) {
		if (x < 0) return false;
		if (y < 0) return false;
		if (x >= N) return false;
		if (y >= N) return false;
		return true;
	}

	void resize_all(int N_, int M_, int V_) {
		N = N_;
		M = M_;
		V = V_;
		N2 = N * N;
		Xm = N / 2;
		Ym = N / 2;
		int LengthMax = N / 2;

		if (V == 5) {
			LengthJoint.resize(1, 0);
			LengthJoint[0] = (N * 100) / 250;
			LengthLeaf = { (N + 15) / 16,(N + 7) / 8,(N + 3) / 4 };
		}
		else if (V <= 8) {
			LengthJoint.resize(2, 0);
			LengthJoint[1] = (N * 100) / 260;
			LengthJoint[0] = (LengthJoint[1] + 1) / 2;
			rep(v, V - LengthJoint.size() - 1)
				LengthLeaf.push_back((v % LengthMax) + 1);
		}
		else {
			LengthJoint.resize(3, 0);
			LengthJoint[2] = (N * 100) / 270;
			LengthJoint[1] = (LengthJoint[2] + 1) / 2;
			LengthJoint[0] = (LengthJoint[1] + 1) / 2;
			rep(v, V - LengthJoint.size() - 1)
				LengthLeaf.push_back((v % LengthMax) + 1);
		}
		Vjoint = LengthJoint.size();
		Vleaf = LengthLeaf.size();
		rep(i, Vjoint)
			KNum *= 4;
		KNum *= N2; // 根の置き場所はN*N通り

		K2PosX.resize(KNum, vector<vi>(Vleaf, vi(4, -1)));
		K2PosY.resize(KNum, vector<vi>(Vleaf, vi(4, -1)));
		kvd2s.resize(KNum, vector<vi>(Vleaf, vi(4, -1)));
		kvd2t.resize(KNum, vector<vi>(Vleaf, vi(4, -1)));
		sv2kd.resize(N2, vector<vector<vector<pii>>>(M, vector<vector<pii>>(Vleaf)));
		tv2kd.resize(N2, vector<vector<vector<pii>>>(M, vector<vector<pii>>(Vleaf)));
		smemo.resize(M * 2);
		tmemo.resize(M * 2);
		droot2nei.resize(N2, vector<vi>(N2));
		S_to_S.resize(M);
		T_to_T.resize(M);
		V_to_V.resize(Vleaf);
		evalS.resize(M);
		evalT.resize(M);
		K2Angle.resize(KNum);

		rep(droot, N2) {
			rep(dto, N2) {
				int d = dist_xy(droot, dto);
				droot2nei[droot][d].push_back(dto);
				if (d == 0) {
					droot2nei[droot][1].push_back(dto);
					droot2nei[droot][2].push_back(dto);
				}
				else if (d == 1) {
					droot2nei[droot][2].push_back(dto);
				}
			}
		}
		rep(k, KNum) {
			K2Angle[k] = ktoD(k);
			int droot = K2Angle[k][0];
			pii xydiff = d_to_xy(droot);
			int X = xydiff.first; // 指付根のx座標
			int Y = xydiff.second; // 指付根のy座標
			int d = 0;
			rep(vjoint, Vjoint) {
				int dtmp = K2Angle[k][vjoint + 1];
				d += dtmp;
				d %= 4;
				X += LengthJoint[vjoint] * dx[d];
				Y += LengthJoint[vjoint] * dy[d];
			}
			rep(dleaf, 4) {
				rep(vleaf, Vleaf) {
					int dleafabs = d + dleaf;
					dleafabs %= 4;
					int x = X + LengthLeaf[vleaf] * dx[dleafabs];
					int y = Y + LengthLeaf[vleaf] * dy[dleafabs];
					K2PosX[k][vleaf][dleaf] = x;
					K2PosY[k][vleaf][dleaf] = y;
					if (!is_inside(x, y)) continue;
					int s = input.xy_to_S[x][y];
					if (s != -1) {
						sv2kd[droot][s][vleaf].push_back(make_pair(k, dleaf));
						kvd2s[k][vleaf][dleaf] = s;
					}
					int t = input.xy_to_T[x][y];
					if (t != -1) {
						tv2kd[droot][t][vleaf].push_back(make_pair(k, dleaf));
						kvd2t[k][vleaf][dleaf] = t;
					}
				}
			}
		}

		double nr = 4.5;
		double n = (double)N / nr;
		rep(s, M) {
			int x = input.S_to_xy[s].first;
			int y = input.S_to_xy[s].second;
			evalS[s] = (abs(x - Xm) + abs(y - Ym)) * Rst;
			if (x < n) evalS[s] += (n - (double)x) * Rst * nr;
			if (y < n) evalS[s] += (n - (double)y) * Rst * nr;
			if (x >= N - n) evalS[s] += ((double)x - ((double)N - n - 1.0)) * Rst * nr;
			if (y >= N - n) evalS[s] += ((double)y - ((double)N - n - 1.0)) * Rst * nr;
		}
		rep(t, M) {
			int x = input.T_to_xy[t].first;
			int y = input.T_to_xy[t].second;
			evalT[t] = (double)(abs(x - Xm) + abs(y - Ym)) * Rst;
			if (x < n) evalT[t] += (n - (double)x) * Rst * nr;
			if (y < n) evalT[t] += (n - (double)y) * Rst * nr;
			if (x >= N - n) evalT[t] += ((double)x - ((double)N - n - 1.0)) * Rst * nr;
			if (y >= N - n) evalT[t] += ((double)y - ((double)N - n - 1.0)) * Rst * nr;
		}
		return;
	}

	void SolveBS() {
		vector<vector<BSdiffinfo>> BSdiff(10000); // 候補を差分で記録する
		vector<vector<pii>> BSfrom(10000);
		vector<vector<BSinfo>> BS(10000); // 遷移元に選ばれたらBSdiffを使ってdeepcopyする
		vector<vector<pii>> BSscore(10000);
		vector<multiset<int>> scset(10000);
		vector<set<unsigned long long>> hashset(10000);
		vector<pii> KD(100000); // 解の候補
		BSinfo BSinit;
		BSinit.init(N, M, V);
		int xm = Xm;
		int ym = Ym;
		rep(p, N * 2 - 1) {
			rep(len, p / 2 + 1) {
				BSinit.D[0] = xy_to_d(xm, ym);
				BSinit.hash = 0;
				BS[0].push_back(BSinit);
				int idx = BSscore[0].size();
				BSscore[0].push_back(make_pair(-idx, idx));
				scset[0].insert(0);
				if (p % 4 == 0)
					ym--;
				if (p % 4 == 1)
					xm--;
				if (p % 4 == 2)
					ym++;
				if (p % 4 == 3)
					xm++;
			}
			BSsizemax = 130000.0 / ((double)M * sqrt(V));
			if (N % 2 == 0 && BS[0].size() >= BSsizemax) break;
			if (N % 2 == 1 && BS[0].size() >= BSsizemax) break;
		}
		int turn = 0;
		while (true) {
			BSsizemax = 130000.0 / ((double)M * sqrt(V));
			double time_now = time_diff();
			sort(BSscore[turn].begin(), BSscore[turn].end(), greater<pii>());
			if (turn >= 1 && BSscore[turn].size() > 0) {
				double turnR = BSdiff[turn][BSscore[turn][0].second].scoreA / (double)(M * scput);
				double BSR = sqrt(turnR * (3000.0 / (time_now + 1.0)));
				BSR = min(BSR, 3.0);
				BSR = max(BSR, 0.3);
				BSsizemax *= BSR;
			}
			if (time_now > 2800.0)
				BSsizemax = min(BSsizemax, 1);
			else if (time_now > 2700.0)
				BSsizemax = min(BSsizemax, 3);
			else if (time_now > 2650.0)
				BSsizemax = min(BSsizemax, 10);
			else if (time_now > 2600.0)
				BSsizemax = min(BSsizemax, 30);
			else if (time_now > 2550.0)
				BSsizemax = min(BSsizemax, 50);
			else if (time_now > 2500.0)
				BSsizemax = min(BSsizemax, 100);
			else if (time_now > 2400.0)
				BSsizemax = min(BSsizemax, 300);
			else if (time_now > 2300.0)
				BSsizemax = min(BSsizemax, 1000);

			BSsizemax2 = BSsizemax;
			bool finish = false;
			int turn_end = -1;
			int qend = -1;
			int Q = BSdiff[turn].size();
			if (turn == 0)
				Q = BS[turn].size();
			else
				BS[turn].resize(Q);
			Q = min(Q, BSsizemax);

			rep(qq, Q) {
				int q = BSscore[turn][qq].second;
				BSinfo bs;
				if (turn == 0)
					bs = BS[turn][q];
				else {
					pii tqfrom = BSfrom[turn][q];
					int turnfrom = tqfrom.first;
					int qfrom = tqfrom.second;
					bs = BS[turnfrom][qfrom];
					bs.D = BSdiff[turn][q].D;
					bs.hash = BSdiff[turn][q].hash;
					bs.is_holding = BSdiff[turn][q].is_holding;
					bs.scoreA = BSdiff[turn][q].scoreA;
					bs.scoreB = BSdiff[turn][q].scoreB;
					for (int s : BSdiff[turn][q].S_picked)
						bs.S_picked[s] = true;
					for (int t : BSdiff[turn][q].T_put)
						bs.T_put[t] = true;
					BS[turn][q] = bs;
				}
				if ((bs.scoreA == M * scput && !finish)) {
					finish = true;
					turn_end = turn;
					qend = q;
					break;
				}
				int drootfrom = bs.D[0];
				int idx = 0;
				rep(st, M) {
					S_to_S[st] = make_pair(xor128(), st);
					T_to_T[st] = make_pair(xor128(), st);
				}
				rep(vleaf, Vleaf)
					V_to_V[vleaf] = make_pair(xor128(), vleaf);
				sort(S_to_S.begin(), S_to_S.end());
				sort(T_to_T.begin(), T_to_T.end());
				sort(V_to_V.begin(), V_to_V.end());
				rep(cost_ans, N2) {
					if (cost_ans == 0) continue;
					if (idx > BSsizemax2)
						break;
					rep(st, M) {
						int s = S_to_S[st].second;
						int t = T_to_T[st].second;
						if (bs.S_picked[s] && bs.T_put[t]) continue;
						rep(vleafidx, Vleaf) {
							if (idx > BSsizemax2)
								break;
							int kdidx = 0;
							int vleaf = V_to_V[vleafidx].second;
							// 指先vleafの状態を変化させる(把持フラグを反転させる)
							if (!bs.is_holding[vleaf]) {
								// 持ってないのでたこやきを探す
								if (bs.S_picked[s])
									continue;
								// cost_ans=1: costdroot=0,1 cost_ans=2: costdroot=0,1,2 cost_ans=c(>=3): costdroot=3
								for (int drootto : droot2nei[drootfrom][cost_ans]) {
									for (pii kdtmp : sv2kd[drootto][s][vleaf])
										KD[kdidx++] = kdtmp; // 解発見: 姿勢k, 指先vleaf, 方向dleafでたこやきsを掴める
								}
							}
							else {
								// 穴を探す
								if (bs.T_put[t])
									continue;
								for (int drootto : droot2nei[drootfrom][cost_ans]) {
									for (pii kdtmp : tv2kd[drootto][t][vleaf])
										KD[kdidx++] = kdtmp; // 解発見: 姿勢k, 指先vleaf, 方向dleafで穴tを埋める
								}
							}
							rep(kdidxtmp, kdidx) {
								if (idx > BSsizemax2)
									break;
								pii kd = KD[kdidxtmp];
								// この解に対し、次ターン(cost=1)あるいは次々ターン(cost=2, 要180°回転の場合)あるいはさらに後(要移動の場合)の状態を1つだけ探索する
								// 姿勢k, 指先vleaf, 方向dleafでたこやきsを掴まない、あるいは穴tを埋めない可能性があるので注意！！！
								int scnewA = bs.scoreA;
								int scnewB = bs.scoreB;
								int k = kd.first;
								int dleaf = kd.second;
								int cost = Calc_cost(bs.D, k, vleaf, dleaf);
								if (cost != cost_ans) continue;
								unsigned long long hashnew = bs.hash;
								hashnew += Khash[k];
								vi Dleaf(Vleaf);
								smemoNum = 0;
								tmemoNum = 0;
								vb is_D(4, false);
								rep(vleaftmp, Vleaf) {
									// vleaftmpの方向Dleaf[vleaftmp]を若い指先から順に貪欲に決める
									int dleafold = bs.D[Vjoint + 1 + vleaftmp];
									Dleaf[vleaftmp] = -1; // 初期値は方向不定
									if (!bs.is_holding[vleaftmp]) {
										rep(dleafnewtmpidx, 4) {
											int dleafnewtmp = D_to_D[dleaf][dleafnewtmpidx];
											// vleaftmpが持ってないのでたこやきを探す
											if (cost == 1 && !can_one_turn(dleafold, dleafnewtmp)) continue; // 2ターンかかる遷移は無視
											int stmp = kvd2s[k][vleaftmp][dleafnewtmp];
											if (stmp == -1) continue; // 指先のマスにたこやきがそもそも無い
											if (bs.S_picked[stmp]) continue; // 指先のマスにたこやきが有ったが、今は無い
											bs.S_picked[stmp] = true;
											smemo[smemoNum++] = stmp;
											scnewA += scpick;
											scnewB += evalS[stmp];
											int x = K2PosX[k][vleaftmp][dleafnewtmp];
											int y = K2PosY[k][vleaftmp][dleafnewtmp];
											bool notX = true, notY = true;
											for (int stmptmp : input.x_to_S[x]) {
												if (!bs.S_picked[stmptmp]) notX = false;
											}
											for (int stmptmp : input.y_to_S[y]) {
												if (!bs.S_picked[stmptmp]) notY = false;
											}
											if (notX) scnewB += RL;
											if (notY) scnewB += RL;
											rep(d, 4) {
												int xnext = x + dx[d];
												int ynext = y + dy[d];
												if (!is_inside(xnext, ynext)) continue;
												int snext = input.xy_to_S[xnext][ynext];
												if (snext == -1) continue;
												if (bs.S_picked[snext])
													scnewB += Radj;
											}
											Dleaf[vleaftmp] = dleafnewtmp; // 方向指定
											hashnew += Shash[stmp];
											hashnew += Vleafhash[vleaftmp];
											break;
										}
									}
									else {
										rep(dleafnewtmpidx, 4) {
											int dleafnewtmp = D_to_D[dleaf][dleafnewtmpidx];
											// vleaftmpが持っているので穴を探す
											if (cost == 1 && !can_one_turn(dleafold, dleafnewtmp)) continue; // 2ターンかかる遷移は無視
											int ttmp = kvd2t[k][vleaftmp][dleafnewtmp];
											if (ttmp == -1) continue; // 指先のマスに穴がそもそも無い
											if (bs.T_put[ttmp]) continue; // 指先のマスに穴が有ったが、今は埋まった
											bs.T_put[ttmp] = true;
											tmemo[tmemoNum++] = ttmp;
											scnewA += (scput - scpick);
											scnewB += evalT[ttmp];
											int x = K2PosX[k][vleaftmp][dleafnewtmp];
											int y = K2PosY[k][vleaftmp][dleafnewtmp];
											bool notX = true, notY = true;
											for (int ttmptmp : input.x_to_T[x]) {
												if (!bs.T_put[ttmptmp]) notX = false;
											}
											for (int ttmptmp : input.y_to_T[y]) {
												if (!bs.T_put[ttmptmp]) notY = false;
											}
											if (notX) scnewB += RL;
											if (notY) scnewB += RL;
											rep(d, 4) {
												int xnext = x + dx[d];
												int ynext = y + dy[d];
												if (!is_inside(xnext, ynext)) continue;
												int tnext = input.xy_to_T[xnext][ynext];
												if (tnext == -1) continue;
												if (bs.T_put[tnext])
													scnewB += Radj;
											}
											Dleaf[vleaftmp] = dleafnewtmp; // 方向指定
											hashnew += Thash[ttmp];
											hashnew -= Vleafhash[vleaftmp];
											break;
										}
									}
									if (Dleaf[vleaftmp] != -1)
										is_D[Dleaf[vleaftmp]] = true;
								}

								rep(sidx, smemoNum)
									bs.S_picked[smemo[sidx]] = false;
								rep(tidx, tmemoNum)
									bs.T_put[tmemo[tidx]] = false;

								// 枝刈りチェック
								int scnew = scnewA + scnewB;
								if (hashset[turn + cost].find(hashnew) != hashset[turn + cost].end())
									continue;
								idx++;
								if (idx > BSsizemax2)
									break;
								scset[turn + cost].insert(scnew);
								if (scset[turn + cost].size() > BSsizemax2) {
									auto scmin = scset[turn + cost].begin();
									if (scnew == *scmin) {
										scset[turn + cost].erase(scmin);
										continue;
									}
									else
										scset[turn + cost].erase(scmin);
								}

								int qold = q;
								BSdiffinfo bsnew;
								bsnew.init(N, M, V);
								bsnew.is_holding = BS[turn][qold].is_holding;
								vi Djoint = K2Angle[k];
								rep(vjoint, Vjoint + 1)
									bsnew.D[vjoint] = Djoint[vjoint];
								rep(vleaf, Vleaf)
									bsnew.D[Vjoint + 1 + vleaf] = Dleaf[vleaf];
								bsnew.scoreA = scnewA;
								bsnew.scoreB = scnewB;

								rep(vleaf, Vleaf) {
									int dleaf = Dleaf[vleaf];
									if (dleaf == -1) continue;
									if (!bsnew.is_holding[vleaf]) {
										int stmp = kvd2s[k][vleaf][dleaf];
										if (stmp == -1) continue;
										if (BS[turn][qold].S_picked[stmp]) continue;
										bool can = true;
										for (int stmptmp : bsnew.S_picked) {
											if (stmp == stmptmp) {
												can = false;
												break;
											}
										}
										if (!can) continue;
										bsnew.S_picked.push_back(stmp);
										bsnew.is_holding[vleaf] = true;
									}
									else {
										int ttmp = kvd2t[k][vleaf][dleaf];
										if (ttmp == -1) continue;
										if (BS[turn][qold].T_put[ttmp]) continue;
										bool can = true;
										for (int ttmptmp : bsnew.T_put) {
											if (ttmp == ttmptmp) {
												can = false;
												break;
											}
										}
										if (!can) continue;
										bsnew.T_put.push_back(ttmp);
										bsnew.is_holding[vleaf] = false;
									}
								}
								bsnew.hash = hashnew - Khash[k];
								BSdiff[turn + cost].push_back(bsnew);
								BSfrom[turn + cost].push_back(make_pair(turn, qold));
								BSscore[turn + cost].push_back(make_pair(scnew, BSscore[turn + cost].size()));
								hashset[turn + cost].insert(hashnew);
							}
						}
					}
					if (idx >= 1)
						break;
				}
			}
			if (finish) {
				int turntmp = turn_end;
				int qtmp = qend;
				while (true) {
					BSans.push_back(BS[turntmp][qtmp]);
					if (turntmp == 0) break;
					pii tqfrom = BSfrom[turntmp][qtmp];
					turntmp = tqfrom.first;
					qtmp = tqfrom.second;
				}
				int D0 = BS[turntmp][qtmp].D[0];
				pii XYm = d_to_xy(D0);
				Xm = XYm.first;
				Ym = XYm.second;
				reverse(BSans.begin(), BSans.end());
				int Q = BSans.size();
				rep(q, Q) {
					// 方向不定の指先を定める
					if (q == 0) continue;
					rep(vleaf, Vleaf) {
						if (BSans[q].D[Vjoint + 1 + vleaf] != -1) continue;
						if (q == Q - 1)
							BSans[q].D[Vjoint + 1 + vleaf] = BSans[q - 1].D[Vjoint + 1 + vleaf];
						else {
							int dfrom = BSans[q - 1].D[Vjoint + 1 + vleaf];
							int dto = BSans[q + 1].D[Vjoint + 1 + vleaf];
							if (dto == -1) BSans[q].D[Vjoint + 1 + vleaf] = dfrom;
							else BSans[q].D[Vjoint + 1 + vleaf] = dfromto[dfrom][dto];
						}
					}
				}
				break;
			}
			turn++;
		}
		return;
	}
};

Info info;

void solve_init() {
	int N = input.N;
	int M = input.M;
	int V = input.V;
	info.resize_all(N, M, V);
	Shash.resize(M, 1);
	Thash.resize(M, 1);
	Khash.resize(1000000, 1);
	Vleafhash.resize(V, 1);
	rep(s, M - 1)
		Shash[s + 1] = Shash[s] * mod;
	Thash[0] = Shash[M - 1] * mod;
	rep(t, M - 1)
		Thash[t + 1] = Thash[t] * mod;
	Vleafhash[0] = Thash[M - 1] * mod;
	rep(v, V - 1)
		Vleafhash[v + 1] = Vleafhash[v] * mod;
	Khash[0] = Vleafhash[V - 1] * mod;
	rep(k, 999999)
		Khash[k + 1] = Khash[k] * mod;
	info.D_to_D = { {0,2,1,3},{1,3,2,0},{2,0,3,1},{3,1,0,2} };
	return;
}

void MakeAns() {
	int N = input.N;
	int M = input.M;
	int V = input.V;
	output.V = V;
	output.P.resize(V, -1);
	output.L.resize(V, 0);
	rep(v, V) {
		if (v == 0) continue;
		else if (v <= info.Vjoint) {
			output.P[v] = v - 1;
			output.L[v] = info.LengthJoint[v - 1];
		}
		else {
			output.P[v] = info.Vjoint;
			output.L[v] = info.LengthLeaf[v - info.Vjoint - 1];
		}
	}
	output.X = info.Xm;
	output.Y = info.Ym;

	int L = info.BSans.size();
	rep(i, L - 1) {
		vi Dfrom = info.BSans[i].D;
		vi Dto = info.BSans[i + 1].D;
		vb hfrom = info.BSans[i].is_holding;
		vb hto = info.BSans[i + 1].is_holding;
		vector<string> ss(N * 2);
		int turn = 1;
		rep(v, V) {
			// 動かす
			vector<char> c(N * 2, '.');
			if (v == 0) {
				turn = max(turn, info.dist_xy(Dfrom[v], Dto[v]));
				pii sxy = info.d_to_xy(Dfrom[v]);
				pii txy = info.d_to_xy(Dto[v]);
				int sx = sxy.first;
				int sy = sxy.second;
				int tx = txy.first;
				int ty = txy.second;
				int diffx = abs(sx - tx);
				int diffy = abs(sy - ty);
				int a = 0;
				rep(x, diffx) {
					if (tx > sx)
						c[a++] = 'D';
					else
						c[a++] = 'U';
				}
				rep(y, diffy) {
					if (ty > sy)
						c[a++] = 'R';
					else
						c[a++] = 'L';
				}
			}
			else {
				if (!info.can_one_turn(Dfrom[v], Dto[v]))
					turn = max(turn, 2);
				int ds = Dfrom[v];
				int dt = Dto[v];
				if (ds == dt)
					c[0] = '.';
				else if ((dt - ds + 4) % 4 == 1)
					c[0] = 'R';
				else if ((dt - ds + 4) % 4 == 3)
					c[0] = 'L';
				else {
					c[0] = 'R';
					c[1] = 'R';
				}
			}
			rep(i, N * 2)
				ss[i] += c[i];
		}
		rep(v, V) {
			// 一部掴むor離す
			vector<char> c(N * 2, '.');
			if (v <= info.Vjoint)
				c[0] = '.';
			else {
				int vleaf = v - info.Vjoint - 1;
				if (hfrom[vleaf] == hto[vleaf])
					c[0] = '.';
				else
					c[0] = 'P';
			}
			rep(i, N * 2)
				ss[i] += c[i];
		}
		rep(t, turn)
			output.S.push_back(ss[turn - t - 1]);
	}
	return;
}

int main(int argc, char* argv[]) {
	if (argc >= 2) file_No = argv[1];
	if (argc >= 3) Vstring = argv[2];
	timespec_get(&timeini, TIME_UTC);
	if (debug)
		debug_init();
	debug ? input.readProblem(fin) : input.readProblem();
	solve_init();
	info.SolveBS();
	MakeAns();
	debug ? output.writeSolution(fout) : output.writeSolution();
	if (debug)
		debug_end();
	return 0;
}
