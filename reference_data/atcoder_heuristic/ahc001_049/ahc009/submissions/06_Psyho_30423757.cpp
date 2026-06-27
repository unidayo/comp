// Author: Psyho
// Twitter: https://twitter.com/fakepsyho
 
#include <bits/stdc++.h>
#include <sys/time.h>

const double TOTAL_TIME = 1.95;
 
using namespace std;
 
#define INLINE   inline __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))

#define byte        unsigned char
#define FOR(i,a,b)  for(int i=(a);i<(b);++i)
#define REP(i,a)    FOR(i,0,a)
#define ZERO(m)     memset(m,0,sizeof(m))
#define MINUS(m)    memset(m,-1,sizeof(m))
#define ALL(x)      x.begin(),x.end()
#define PB          push_back
#define S           size()
#define LL          long long
#define ULL         unsigned long long
#define LD          long double
#define MP          make_pair
#define X           first
#define Y           second
#define VC          vector
#define PII         pair<int, int>
#define PDD         pair<double, double>
#define PIII        pair<PII, int>
#define VB          VC<byte>
#define VVB         VC<VB>
#define VI          VC<int>
#define VVI         VC<VI>
#define VVVI        VC<VVI>
#define VPII        VC<PII>
#define VVPII       VC<VPII>
#define VD          VC<double>
#define VVD         VC<VD>
#define VVVD        VC<VVD>
#define VVVVD       VC<VVVD>
#define VF          VC<float>
#define VVF         VC<VF>
#define VVVF        VC<VVF>
#define VS          VC<string>
#define VVS         VC<VS>
 
template<typename A, typename B> ostream& operator<<(ostream &os, pair<A,B> p) {os << "(" << p.X << "," << p.Y << ")"; return os;}
template<typename A, typename B, typename C> ostream& operator<<(ostream &os, tuple<A,B,C> p) {os << "(" << get<0>(p) << "," << get<1>(p) << "," << get<2>(p) << ")"; return os;}
template<typename T> ostream& operator<<(ostream &os, VC<T> v) {os << "{"; REP(i, v.S) {if (i) os << ", "; os << v[i];} os << "}"; return os;}
template<typename T> ostream& operator<<(ostream &os, set<T> s) {VS vs(ALL(s)); return os << vs;}
template<typename T> string i2s(T x) {ostringstream o; o << x; return o.str();}
VS splt(string s, char c = ' ') {VS all; int p = 0, np; while (np = s.find(c, p), np >= 0) {if (np != p) all.PB(s.substr(p, np - p)); p = np + 1;} if (p < s.S) all.PB(s.substr(p)); return all;}

#define ARGS_SIZE_(a1,a2,a3,a4,a5,a6,a7,a8,a9,size,...) size
#define ARGS_SIZE(...) ARGS_SIZE_(__VA_ARGS__,9,8,7,6,5,4,3,2,1)

#define DB_1(x) #x << "=" << (x) <<
#define DB_2(x, ...) #x << "=" << (x) << ", " << DB_1(__VA_ARGS__)
#define DB_3(x, ...) #x << "=" << (x) << ", " << DB_2(__VA_ARGS__)
#define DB_4(x, ...) #x << "=" << (x) << ", " << DB_3(__VA_ARGS__)
#define DB_5(x, ...) #x << "=" << (x) << ", " << DB_4(__VA_ARGS__)
#define DB_6(x, ...) #x << "=" << (x) << ", " << DB_5(__VA_ARGS__)
#define DB_7(x, ...) #x << "=" << (x) << ", " << DB_6(__VA_ARGS__)
#define DB_8(x, ...) #x << "=" << (x) << ", " << DB_7(__VA_ARGS__)
#define DB_9(x, ...) #x << "=" << (x) << ", " << DB_8(__VA_ARGS__)
#define DB__(size, ...) DB_##size(__VA_ARGS__)
#define DB_(size, ...) DB__(size, __VA_ARGS__)
#define DB(...) do {cerr << DB_(ARGS_SIZE(__VA_ARGS__), __VA_ARGS__) endl;} while (0)
 
double get_time() {timeval tv; gettimeofday(&tv, NULL); return tv.tv_sec + tv.tv_usec * 1e-6;}
 
struct RNG {
    unsigned int MT[624];
    int index;
    RNG(int seed = 1) {init(seed);}
    void init(int seed = 1) {MT[0] = seed; FOR(i, 1, 624) MT[i] = (1812433253UL * (MT[i-1] ^ (MT[i-1] >> 30)) + i); index = 0; }
    void generate() {
        const unsigned int MULT[] = {0, 2567483615UL};
        REP(i, 227) {unsigned int y = (MT[i] & 0x8000000UL) + (MT[i+1] & 0x7FFFFFFFUL); MT[i] = MT[i+397] ^ (y >> 1); MT[i] ^= MULT[y&1]; }
        FOR(i, 227, 623) {unsigned int y = (MT[i] & 0x8000000UL) + (MT[i+1] & 0x7FFFFFFFUL); MT[i] = MT[i-227] ^ (y >> 1); MT[i] ^= MULT[y&1]; }
        unsigned int y = (MT[623] & 0x8000000UL) + (MT[0] & 0x7FFFFFFFUL); MT[623] = MT[623-227] ^ (y >> 1); MT[623] ^= MULT[y&1];
    }
    unsigned int rand() { if (index == 0) generate(); unsigned int y = MT[index]; y ^= y >> 11; y ^= y << 7  & 2636928640UL; y ^= y << 15 & 4022730752UL; y ^= y >> 18; index = index == 623 ? 0 : index + 1; return y;}
    INLINE int next() {return rand(); }
    INLINE int next(int x) {return rand() % x; }
    INLINE int next(int a, int b) {return a + (rand() % (b - a)); }
    INLINE double next_double() {return (rand() + 0.5) * (1.0 / 4294967296.0); }
    INLINE double next_double(double a, double b) {return a + next_double() * (b - a); }
};
 
static RNG rng;

double start_time = get_time();


int sx, sy, tx, ty;
double p;
int grid[44][44];

int dx[] = {-1,1,0,0};
int dy[] = {0,0,-1,1};
char dd[] = {'U','D','L','R'};


double simp1[20][20];
double simp2[20][20];
double (*lastp)[20];

double exact_sim(string &moves) {
	ZERO(simp1);
	simp1[sx][sy] = 1.0;
	
	double (*v1)[20] = simp1;
	double (*v2)[20] = simp2;
	
	double rv = 0;
	REP(turn, moves.S) {
		char d = moves[turn];
		int ad; REP(i, 4) if (d == dd[i]) ad = i;
		int mx = dx[ad];
		int my = dy[ad];
		REP(x, 20) REP(y, 20) v2[x][y] = 0;
		REP(x, 20) REP(y, 20) {
			if (grid[2+x*2+mx][2+y*2+my] == 1) {
				v2[x][y] += v1[x][y];
			} else {
				v2[x][y] += v1[x][y] * p;
				v2[x+mx][y+my] += v1[x][y] * (1 - p);
			}
			
		}
		
		rv += (401 - (turn + 1)) * v2[tx][ty];
		v2[tx][ty] = 0;
		
		swap(v1, v2);
	}
	lastp = v1;
	
	return 250000 * rv;
}

double exact_sim_more(string &moves) {
	double (*v1)[20] = simp1;
	double (*v2)[20] = simp2;
	
	double rv = 0;
	REP(turn, moves.S) {
		char d = moves[turn];
		int ad; REP(i, 4) if (d == dd[i]) ad = i;
		int mx = dx[ad];
		int my = dy[ad];
		REP(x, 20) REP(y, 20) v2[x][y] = 0;
		REP(x, 20) REP(y, 20) {
			if (grid[2+x*2+mx][2+y*2+my] == 1) {
				v2[x][y] += v1[x][y];
			} else {
				v2[x][y] += v1[x][y] * p;
				v2[x+mx][y+my] += v1[x][y] * (1 - p);
			}
			
		}
		
		rv += (401 - (turn + 1)) * v2[tx][ty];
		v2[tx][ty] = 0;
		
		swap(v1, v2);
	}
	lastp = v1;
	
	return 250000 * rv;
}

int tdst[20][20];

double score_lastp(int turn = 0) {
	double rv = 0;
	// REP(x, 20) REP(y, 20) rv += lastp[x][y] / max(1.0, pow(tdst[x][y], 2));
	REP(x, 20) REP(y, 20) if (turn + tdst[x][y] <= 200) rv += 250000 * (401 - (turn + 1 + tdst[x][y])) * lastp[x][y] / max(1.0, pow(tdst[x][y], 2)) / 10.0;
	
	return rv;
}

string greedy() {
	string moves = "";
	string bmoves;
	while (moves.S < 200) {
		double bv = -1e9;
		REP(d, 4) {
			string nmoves = moves + dd[d];
			double av = 0;
			av += exact_sim(nmoves);
			av += score_lastp(nmoves.S);
			if (av > bv) {
				bv = av;
				bmoves = nmoves;
			}
		}
		moves = bmoves;
	}
	DB(exact_sim(moves));
	
	return moves;
}

const int MAX_WIDTH = 1000;
pair<double, pair<double, string>> bs_cur[MAX_WIDTH];
pair<double, pair<double, string>> bs_next[MAX_WIDTH*4];
int bs_curno = 0;
int bs_nextno = 0;
VVD bs_pcur[MAX_WIDTH];
VVD bs_pnext[MAX_WIDTH*4];

string bs(int width) {
	REP(i, width) bs_pcur[i] = VVD(20, VD(20));
	REP(i, width*4) bs_pnext[i] = VVD(20, VD(20));
	bs_pcur[0][sx][sy] = 1.0;
	
	bs_cur[0] = MP(0.0, MP(0.0, ""));
	bs_curno = 1;
	VI order;
	REP(step, 200) {
		bs_nextno = 0;
		REP(i, bs_curno) {
			REP(d, 4) {
				string nmoves = string("") + dd[d];
				string allmoves = bs_cur[i].Y.Y + nmoves;
				REP(x,20) REP(y,20) simp1[x][y] = bs_pcur[i][x][y];
				double av = bs_cur[i].Y.X;
				av += exact_sim_more(nmoves);
				double ov = av;
				av += score_lastp(allmoves.S);
				REP(x,20) REP(y,20) bs_pnext[bs_nextno][x][y] = lastp[x][y];
				bs_next[bs_nextno++] = MP(av, MP(ov, allmoves));
			}
		}
		order.clear();
		REP(i, bs_nextno) order.PB(i);
		sort(ALL(order), [&](int a, int b){return bs_next[a].X > bs_next[b].X;});
		bs_curno = min(width, bs_nextno);
		REP(i, bs_curno) {
			bs_cur[i] = bs_next[order[i]];
			REP(x, 20) REP(y, 20) bs_pcur[i][x][y] = bs_pnext[order[i]][x][y];
		}
	}
	DB(bs_cur[0].X);
	return bs_cur[0].Y.Y;
}

string hc(string init, double max_time) {
	string state = init;
	double bv = exact_sim(state);
	DB(bv);
	
	int step = 0;
	int acc = 0;
	while (true) {
		double cur_time = get_time();
		if (cur_time > max_time) break;
		
		string nstate = state;
		REP(_, rng.next(1, 3)) {
			if (rng.next(2)) {
				nstate[rng.next(200)] = dd[rng.next(4)];
			} else {
				int p1 = rng.next(200);
				int p2 = rng.next(200);
				char c = nstate[p1];
				nstate.erase(nstate.begin() + p1);
				nstate.insert(nstate.begin() + p2, c);
			}
		}
		if (state == nstate) continue;
		step++;
		
		double av = exact_sim(nstate);
		if (av >= bv) {
			state = nstate;
			acc++;
			bv = av;
		} else {
		}
	}
	
	DB(step, acc, bv);
	return state;
}



int main(int argc, char **argv) {
	cin >> sx >> sy >> tx >> ty >> p;
	DB(sx); DB(sy); DB(tx); DB(ty); DB(p);
	REP(x, 20) {
		string s; cin >> s;
		REP(y, 19) if (s[y] == '1') grid[2+x*2][2+y*2+1] = 1;
	}
	REP(x, 19) {
		string s; cin >> s;
		REP(y, 20) if (s[y] == '1') grid[2+x*2+1][2+y*2] = 1;
	}
	REP(x, 20) {
		grid[2+x*2][2+0*2-1] = 1;
		grid[2+x*2][2+19*2+1] = 1;
	}
	REP(y, 20) {
		grid[2+0*2-1][2+y*2] = 1;
		grid[2+19*2+1][2+y*2] = 1;
	}
	
	MINUS(tdst);
	queue<int> q;
	q.push(tx), q.push(ty), q.push(0);
	while (!q.empty()) {
		int x = q.front(); q.pop();
		int y = q.front(); q.pop();
		int v = q.front(); q.pop();
		if (tdst[x][y] != -1) continue;
		tdst[x][y] = v;
		REP(d, 4) {
			int nx = x + dx[d];
			int ny = y + dy[d];
			if (grid[2+2*x+dx[d]][2+2*y+dy[d]] == 0) q.push(nx), q.push(ny), q.push(v+1);
		}
	}
	
	string moves = bs(200);
	DB(moves);
	DB(get_time() - start_time);
	moves = hc(moves, start_time + TOTAL_TIME);
	DB(moves);
	cout << moves << endl;
	return 0;
}
