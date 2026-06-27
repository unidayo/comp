#include <bits/stdc++.h>
int STANDARD = 1;
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
template<class A, class B>
ostream& operator<<(ostream& os, const pair<A, B>& p) { os << "(" << p.first << "," << p.second << ")"; return os; }
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "["; for (int i=0;i<v.size();i++) { if (i>0) os << ","; os << v[i]; } os << "]"; return os;
}
template<class T>
ostream& operator<<(ostream& os, const set<T>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ","; os << i; } os << "}"; return os;
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
    Rand() {}
    Rand(ll seed) { reseed(seed); o = 0; }
    inline void reseed(ll seed) { x = 0x498b3bc5 ^ seed; y = 0; z = 0; w = 0;  F0(i, 20) mix(); }
    inline void mix() { ll t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8); }
    inline ll rand() { mix(); return x & (MAX_RAND - 1); }
    inline int nextInt(int n) { return rand() % n; }
    inline int nextInt(int L, int R) { return rand()%(R - L + 1) + L; }
    inline int nextBool() { if (o < 4) o = rand(); o >>= 2; return o & 1; }
    double nextDouble() { return rand() * 1.0 / MAX_RAND; }
};
Rand my(2020);
double saveTime;
const int _SZ = 32;
double t_o[_SZ];
ll c_o[_SZ];
void Init() {
    saveTime = GetSeconds();
    F0(i, _SZ) t_o[i] = 0.0;
    F0(i, _SZ) c_o[i] = 0;
}
double Elapsed() { return GetSeconds() - saveTime; }
void Report() {
    double tmp = Elapsed();
    cerr << "-------------------------------------" << endl;
    cerr << "Elapsed time: " << tmp << " sec" << endl;
    double total = 0.0; F0(i, _SZ) { if (t_o[i] > 0) cerr << "t_o[" << i << "] = " << t_o[i] << endl; total += t_o[i]; } cerr << endl; //if (total > 0) cerr << "Total spent: " << total << endl;
    F0(i, _SZ) if (c_o[i] > 0) cerr << "c_o[" << i << "] = " << c_o[i] << endl;
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

const int N = 32;
const int LOGN = 1 << 16;
double logs[LOGN];
enum {UP, RIGHT, DOWN, LEFT, STAY};
typedef pair<pii, int> Act;
const int DX[]={-1,0,1,0,0};
const int DY[]={0,1,0,-1,0};
const string DS="URDLN";
const int T = 200;
int n, elapsedTime;
int hb[N][N], vb[N][N];
pii nx[N][N][4];
pii p1, p2, no(-1, -1);
double score, bscore, P;
double bw[T+1][N][N];
vector<int> d, bd;

#define ID(x, y) ((x)*n+(y))
#define PID(p) ID((p).first, (p).second)
#define X(z) ((z)/n)
#define Y(z) ((z)%n)
#define XY(z) pii((z)/n,(z)%n)

int d1(int x, int y) { return abs(x-y); }
int dbord(int x) { return min(x, n-1-x); }
int d1(pii p1, pii p2) { return d1(p1.first,p2.first)+d1(p1.second,p2.second); }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
//int GetDir(pii p1, pii p2) { F0(k, 4) if (d1(p1, p2) > d1(Move(p1, k), p2)) return k; return -1; }
int GetDir(pii p1, pii p2) { F0(k, 4) if (Move(p1, k) == p2) return k; throw; return 4; }
int GetDir(int a1, int a2) { return GetDir(XY(a1), XY(a2)); }
int sqr(int x) { return x*x; }

void Prepare() {
    bscore = 0.0;
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);

    F0(i, n) F0(j, n) F0(dir, 4) {
        nx[i][j][dir] = pii(i, j);
        pii p = Move(pii(i, j), dir);
        if (!In(p)) continue;
        pii q = min(pii(i, j), p);
        if (p.first == i && hb[q.first][q.second]) continue;
        if (p.second == j && vb[q.first][q.second]) continue;
        nx[i][j][dir] = p;
    }
}

void UpdateBest() {
    if (score > bscore) {
        bd=d;
        bscore=score;
    }
}

double dp[N][N], dp2[N][N], a[N][N], b[N][N], tdp[T + 1][N][N];

void Tran(double a[N][N], double b[N][N], int dir) {
    F0(i, n) F0(j, n) b[i][j] = 0.0;
    F0(i, n) F0(j, n) if (a[i][j] > 0) {
        if (pii(i, j) == p2) {
        } else {
            pii p = nx[i][j][dir];
            b[i][j] += P * a[i][j];
            b[p.first][p.second] += (1 - P) * a[i][j];
        }
    }
}

int tlen = 1;
double bt;
int tail[T], btail[T];

void go(int step, double ret) {
    if (step == T) {
        if (ret > bt) {
            bt = ret;
            for (int i = T-tlen; i < T; i++) btail[i] = tail[i];
        }
        return;
    }
    for (tail[step] = 0; tail[step] < 4; tail[step]++) {
        CL(0, tdp[step + 1]);
        int dir = tail[step];

        F0(i, n) F0(j, n) if (tdp[step][i][j]) {
            pii p = nx[i][j][dir];
            tdp[step+1][i][j] += P * tdp[step][i][j];
            tdp[step+1][p.first][p.second] += (1 - P) * tdp[step][i][j];
        }
        double val = (400 - step) * tdp[step+1][p2.first][p2.second];
        tdp[step+1][p2.first][p2.second] = 0;
        go(step + 1, ret + val);
    }
}

double DEval() {
    CL(0, dp);
    dp[p1.first][p1.second] = 1.0;
    double ret = 0.0;
    F0(step, T-tlen) {
       CL(0, dp2);
       int dir = d[step];

       F0(i, n) F0(j, n) if (dp[i][j]) {
           pii p = nx[i][j][dir];
           dp2[i][j] += P * dp[i][j];
           dp2[p.first][p.second] += (1 - P) * dp[i][j];
       }
       F0(i, n) F0(j, n) dp[i][j] = dp2[i][j];
       ret += (400 - step) * dp[p2.first][p2.second];
       dp[p2.first][p2.second] = 0;
    }

    bt = ret;
    F0(i, n) F0(j, n) tdp[T-tlen][i][j] = dp[i][j];
    go(T - tlen, ret);

    ret = bt;
    for (int i = T-tlen; i < T; i++) d[i] = btail[i];
    return ret;
}

void Solve() {
    Init();
    Prepare();

    d.clear();
    CL(0, a);
    a[p1.first][p1.second] = 1.0;

    F0(its, 10) {
        CL(0, bw);
        double coeff = 0.05 + its * 0.1;
        bw[0][p2.first][p2.second] = 0;
        for (int t = 1; t <= T; t++) {
            F0(i, n) F0(j, n) {
                if (pii(i, j) == p2) {
                    bw[t][i][j] = 0.0;
                } else {
                    vector<double> vals;
                    F0(k, 4) {
                        double val = 0.0;
                        pii p = nx[i][j][k];
                        val = 1.0
                            + (1 - P) * bw[t - 1][p.first][p.second]
                            + P * bw[t - 1][i][j];
                        vals.push_back(val);
                    }
                    double res = 0.0;
                    sort(vals.begin(), vals.end());
                    double mult = 1.0;
                    double sm = 0.0;
                    for (double d : vals) {
                        res += d * mult;
                        sm += mult;
                        mult *= coeff;
                    }
                    res /= sm;
                    bw[t][i][j] = res;
                }
            }
        }
        d.clear();
        F0(t, T) {
            double bv = inf;
            int bk = -1;
            F0(k, 4) {
                Tran(a, b, k);
                double av = 0.0;
                F0(i, n) F0(j, n) av += bw[T - t - 1][i][j] * b[i][j];
                if (av < bv) {
                    bv = av;
                    bk = k;
                }
            }
            //bk = t%2 ? DOWN : RIGHT;
            d.push_back(bk);
            Tran(a, b, bk);
            F0(i, n) F0(j, n) a[i][j] = b[i][j];
        }
        score = DEval();
        PR(score);
        UpdateBest();
    }
    PR(Elapsed());

    bscore = score;
    d = bd;

    int tot = 1, acc = 0;
    double T1 = 0.0, T2 = 0.3, TEMP = 0.0;
    double startTime = Elapsed(), endTime = 2.0;
    int itc=1000000000;
    F0(iter, itc) {
        if ((iter & 31) == 0) {
            double r = (endTime - Elapsed()) / (endTime - startTime);
            if (r <= 0) {
                break;
            }
            //PR(iter);
            TEMP = T1 + (T2 - T1) * r;
        }

        vector<int> saved = d;
        if (my.nextInt(2) == 0) {
            int i = my.nextInt(T - 1);
            int j = d[i];
            if (my.nextInt(2)) {
                d.insert(d.begin() + i, j);
                d.pop_back();
            } else {
                d.erase(d.begin() + i);
                d.push_back(my.nextInt(4));
            }
        }
        else if (my.nextInt(8)) {
            int i = my.nextInt(T);
            int j = d[i];
            while (d[i] == j) {
                int distr[] = {1, 2, 2, 1};
                int q = my.nextInt(distr[0] + distr[1] + distr[1] + distr[2]);
                if (q < distr[0]) j = 0;
                else if (q < distr[1] + distr[0]) j = 1;
                else if (q < distr[2] + distr[1] + distr[0]) j = 2; else j = 3;
            }
            d[i] = j;
        } else {
            int i = my.nextInt(T - 1);
            while (d[i] == d[i + 1]) i = my.nextInt(T - 1);
            swap(d[i], d[i + 1]);
        }

        double nscore = DEval();
        tot++;
        double maxd = TEMP * logs[my.nextInt(LOGN)];
        if (nscore - score >= 0 || nscore - score > -maxd) {
            acc++;
            score = nscore;
            UpdateBest();
            //PR(nscore);
        } else {
            d = saved;
        }
    }
    PR(tot);
    PR(100.0*acc/tot);

    if (STANDARD) {
        for (int d : bd) cout << DS[d];
        cout << endl;
    }

    PR(int(bscore * 250000+0.5));
    //Report();
}

void ReadInput() {
    n = 20;
    cin >> p1 >> p2 >> P;
    string s;
    F0(i, n) {
        cin >> s;
        F0(j, n-1) hb[i][j] = s[j]-'0';
    }
    F0(i, n-1) {
        cin >> s;
        F0(j, n) vb[i][j] = s[j]-'0';
    }
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
            //cout << "Score would be " << bscore << endl;
            //Eval();
        } else {
            // Generate
            throw;
            Rand my(seed);
        }
    }

    return 0;
}
