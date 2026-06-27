int STANDARD=1;
#include <bits/stdc++.h>
using namespace std;
#define F0(i,n) for (int i=0; i<n; i++)
#define FR(i,n) for (int i=n-1; i>=0; i--)
#define F1(i,n) for (int i=1; i<=n; i++)
#define CL(a,x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())
const int inf = 1000009;
const double pi = acos(-1.0);
typedef pair<int, int> pii;
typedef long long ll;
typedef unsigned long long ull;
const double EPS = 1e-9;
#define PR(x) cerr << #x << "=" << x << endl
ostream& operator<<(ostream& os, const pii& p) { os << "(" << p.first << "," << p.second << ")"; return os; }
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "["; F0(i,SZ(v)) { if (i>0) os << ","; os << v[i]; } os << "]"; return os;
}
void Print(set<int> v) {
    cerr << "{"; int f=1; for(int i:v) { if(f)f=0;else cerr << ","; cerr << i; }
    cerr << "}" << endl;
}

inline ll GetTSC() {
    ll lo, hi;
    asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
    return lo + (hi << 32);
}
inline double GetSeconds() {
    return GetTSC() / 2.8e9;
}

const int MAX_RAND = 1 << 30;
struct Rand {
    ll x, y, z, w, o;
    Rand(ll seed) { reseed(seed); o = 0; }
    inline void reseed(ll seed) { x = 0x498b3bc5 ^ seed; y = 0; z = 0; w = 0;  F0(i, 20) mix(); }
    inline void mix() { ll t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8); }
    inline ll rand() { mix(); return x & (MAX_RAND - 1); }
    inline int nextInt(int n) { return rand() % n; }
    inline int nextInt(int L, int R) { return rand()%(R - L + 1) + L; }
    inline int nextBool() { if (o < 4) o = rand(); o >>= 2; return o & 1; }
    double nextDouble() { return rand() * 1.0 / MAX_RAND; }
};
Rand my(2022);
double saveTime;
double t_o[20];
ll c_o[20];
void Init() {
    saveTime = GetSeconds();
    F0(i, 20) t_o[i] = 0.0;
    F0(i, 20) c_o[i] = 0;
}
double Elapsed() { return GetSeconds() - saveTime; }
void Report() {
    double tmp = Elapsed();
    cerr << "-------------------------------------" << endl;
    cerr << "Elapsed time: " << tmp << " sec" << endl;
    double total = 0.0; F0(i, 20) { if (t_o[i] > 0) cerr << "t_o[" << i << "] = " << t_o[i] << endl; total += t_o[i]; } cerr << endl; //if (total > 0) cerr << "Total spent: " << total << endl;
    F0(i, 20) if (c_o[i] > 0) cerr << "c_o[" << i << "] = " << c_o[i] << endl;
    cerr << "-------------------------------------" << endl;
}
struct AutoTimer {
    int x;
    double t;
    AutoTimer(int x) : x(x) {
        t = Elapsed();
    }
    ~AutoTimer() {
        t_o[x] += Elapsed() - t;
    }
};
#define AT(i) AutoTimer a##i(i)
//#define AT(i)

// CONSTANTS
const int N = 512;
const int M = 2048;
const int LOGN = 1 << 16;
double logs[LOGN];
enum { UP, RIGHT, DOWN, LEFT };
const int DX[]={-1,0,1,0};
const int DY[]={0,1,0,-1};
const string DS="URDL";
int n, m, e, cn, px, py;
const int MAXSN = 41;
int SN = MAXSN;
int score, bscore;
int sol;
int u[M], v[M], x[N], y[N], d[N][N], len[M], lb[M];
int slen[MAXSN][M];
vector<int> ssorted[MAXSN];
vector<int> lens;
vector<double> dp;
vector<int> par, spar;
int get(int x) {
    return (par[x] == x) ? x : (par[x] = get(par[par[x]]));
}

int d1(int x, int y) { return abs(x-y); }
int d1(pii p1, pii p2) { return d1(p1.first,p2.first)+d1(p1.second,p2.second); }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
//bool Good(pii p) { return In(p) && a[p.first][p.second] != -1; }
int GetDir(pii p1, pii p2) { F0(k, 4) if (d1(p1, p2) > d1(Move(p1, k), p2)) return k; return -1; }
int sqr(int x) { return x*x; }

void PrepS() {
    F0(s, MAXSN) {
        ssorted[s].clear();
        F0(i, m) {
            if (s == 0) slen[s][i] = 2 * lb[i];
            else if (s % 2 == 1) slen[s][i] = my.nextInt(lb[i], 3 * lb[i]);
            else slen[s][i] = 4 * lb[i] - slen[s - 1][i];
            ssorted[s].push_back(i);
        }
        vector<int> tmp;
        set<pii> S;
        sort(ssorted[s].begin(), ssorted[s].end(), [&](int x, int y) { return slen[s][x] < slen[s][y]; });
        for (int x : ssorted[s]) {
            pii z(u[x], v[x]);
            if (z.first > z.second) swap(z.first, z.second);
            if (!S.count(z)) {
                tmp.push_back(x);
                S.insert(z);
            }
        }
        ssorted[s] = tmp;
    }
}

void Prepare() {
    bscore = inf;
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
    F0(i, n) F0(j, i) {
        d[i][j] = d[j][i] = (int)hypot(1.0*x[i]-x[j], 1.0*y[i]-y[j]) + 0.5;
    }
    F0(i, m) {
        lb[i] = d[u[i]][v[i]];
    }
    PrepS();
}

void UpdateBest() {
    if (score < bscore) {
        bscore=score;
    }
}

void Input() {
    cin >> len[e];
}

void Output() {
    cout << sol << endl;
    if (sol) {
        bscore += len[e];
        par[px] = py;
        cn--;
    }
}

int Est(int s, int cn) {
    int ret = 0;
    if (cn > 1) {
        F0(i, m) if (ssorted[s][i] > e) {
            int z = ssorted[s][i];
            int x = u[z], y = v[z];
            int px = get(x), py = get(y);
            if (px != py) {
                ret += slen[s][z];
                cn--;
                par[px] = py;
                if (cn == 1) break;
            }
        }
    }
    if (cn > 1) return inf;
    return ret;
}

int Decide() {
    int x = u[e], y = v[e];
    px = get(x); py = get(y);
    if (px == py) {
        return 0;
    }

    spar = par;
    int a0 = 0, a1 = 0;
    double awith = 0.0, awithout = 0.0;
    F0(s, SN) {
        int est_without = Est(s, cn);
        par = spar;

        if (est_without == inf) {
            return 1;
        }

        par[px] = py;
        int est_with = Est(s, cn - 1);
        par = spar;

        if (s == 0) {
            a0 = est_without;
            a1 = est_with;
        } else {
            awithout += est_without;
            awith += est_with;
        }
    }

    awithout /= (SN - 1);
    awith /= (SN - 1);

    double coeff = 0.25;
    awithout = awithout * (1 - coeff) + a0 * coeff;
    awith = awith * (1 - coeff) + a1 * coeff;

    if (len[e] > (awithout - awith) * 1) {
        return 0;
    }

    return 1;
}

void Solve() {
    Init();
    Prepare();

    par.assign(n, -1);
    F0(i, n) par[i] = i;
    bscore = 0;
    cn = n;

    for (e = 0; e < m; e++) {
        F0(i, n) par[i] = get(par[i]);
        //if (e % 50 == 49) PrepS();
        F0(s, SN) {
            F0(i, SZ(ssorted[s])) if (ssorted[s][i] == e) {
                ssorted[s].erase(ssorted[s].begin() + i);
                break;
            }
        }

        Input();

        sol = Decide();

        Output();
    }
    PR(bscore);

    Report();
}

void ReadInput() {
    n = 400;
    m = 1995;
    F0(i, n) cin >> x[i] >> y[i];
    F0(i, m) cin >> u[i] >> v[i];
}

void Eval() {
    int act_score=bscore;
    cout << "Local score = " << act_score << endl;
}

int main(int argc, char* argv[]) {
    int seed1 = 0, seed2 = 0;
    if(argc>1) {
        if (argc == 2) {
            seed1 = seed2 = atoi(argv[1]);
        } else {
            seed1 = atoi(argv[1]);
            seed2 = atoi(argv[2]);
        }
        STANDARD=0;
    }

    if(STANDARD) {
        ReadInput();
        Solve();
        return 0;
    }

    for (int seed=seed1; seed<=seed2; seed++) {
        if(seed>=0 && seed<50) {
            char inp[128];
            sprintf(inp, "in/%04d.txt", seed);
            ignore = freopen(inp, "r", stdin);
            ReadInput();
            Solve();
            cout << "Score would be " << bscore << endl;
            //Eval();
        } else {
            // Generate
            throw;
            Rand my(seed);
        }
    }

    return 0;
}
