int DBG = 1;
#define TIME_BASED
double TL = 1.9;
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <cstring>
#include <cmath>
#include <cassert>
#include <random>
#include <iomanip>
#include <unordered_set>
#include <unordered_map>
int STANDARD = 1;
using namespace std;
#define F0(i,n) for (int i=0; i<n; i++)
#define FR(i,n) for (int i=n-1; i>=0; i--)
#define F1(i,n) for (int i=1; i<=n; i++)
#define CL(a,x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())
const int inf = 1000000000;
const double pi = acos(-1.0);
typedef pair<int, int> pii;
typedef long long ll;
typedef unsigned long long ull;
const double EPS = 1e-9;
#define PR(x) cerr << #x << "=" << x << endl
template<class A, class B>
ostream& operator<<(ostream& os, const pair<A, B>& p) { os << "(" << p.first << "," << p.second << ")"; return os; }
template<class A, class B, class C>
ostream& operator<<(ostream& os, const tuple<A, B, C>& p) { os << "(" << get<0>(p) << "," << get<1>(p) << "," << get<2>(p) << ")"; return os; }
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "["; F0(i,SZ(v)) { if (i>0) os << ","; os << v[i]; } os << "]"; return os;
}
template<class T>
ostream& operator<<(ostream& os, const set<T>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ","; cerr << i; } os << "}"; return os;
}
template<class T, class R>
ostream& operator<<(ostream& os, const map<T,R>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ", "; cerr << i.first << ":" << i.second; } os << "}"; return os;
}

//#pragma GCC optimize("O3,Ofast,omit-frame-pointer,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize")

#ifndef __APPLE__
inline ll GetTSC() {
    ll lo, hi;
    asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
    return lo + (hi << 32);
}
inline double GetSeconds() {
    return GetTSC() / 3.0e9;
}
#else
#include <sys/time.h>
double GetSeconds() {
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}
#endif

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
Rand my(2025);
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
const int N = 256;
int n, m, sx, sy;
string s[N], pattern[N];
ll bscore, score;
vector<int> bsol, sol;
vector<int> by_letter[26];
int sz[26];
int d[N][N], d0[N], pm[N][N], any[N], pend[N][N][N], pbeg[N][N][N], pex[N][N][N];
int prec[N][N], sp[N];

const int LOGN = 1 << 16;
double logs[LOGN];
int dp[256][1000], way[256][1000];

template<class T> void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }
template<class T> T sqr(T x) { return x*x; }


void Prepare() {
    bscore = inf;
    //s0 = sx * m + sy;
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);

    F0(i, m) F0(j, m) {
        by_letter[s[i][j] - 'A'].push_back(i * m + j);
        d0[i * m + j] = abs(i - sx) + abs(j - sy);
        F0(x, m) F0(y, m) {
            d[i * m + j][x * m + y] = abs(i - x) + abs(j - y) + 1;
        }
    }

    F0(i, n) F0(j, n) {
        for (int k = 4; k >= 1; k--) if (pattern[i].substr(5 - k) == pattern[j].substr(0, k)) {
            pm[i][j] = k;
            if (k >= 3 && 0) {
                cerr << k << " " <<pattern[i] << " " << pattern[j] << endl;
            }
            break;
        }
    }

    F0(p1, n) {
        prec[n][p1] = inf;
        F0(i, 5) {
            int c = pattern[p1][i] - 'A';
            if (i == 0) {
                for (int x : by_letter[c]) dp[x][i] = d0[x];
            } else {
                int pc = pattern[p1][i - 1] - 'A';
                for (int x : by_letter[c]) {
                    dp[x][i] = inf;
                    for (int y : by_letter[pc]) {
                        if (dp[x][i] > dp[y][i - 1] + d[x][y]) {
                            dp[x][i] = dp[y][i - 1] + d[x][y];
                        }
                    }
                    if (i == 4) {
                        prec[n][p1] = min(prec[n][p1], dp[x][i]);
                    }
                }
            }
        }
    }
    F0(p1, n) {
        //PR(p1);
        any[p1] = inf;
        F0(i, 5) {
            int c = pattern[p1][i] - 'A';
            if (i == 0) {
                for (int x : by_letter[c]) dp[x][i] = 0;
            } else {
                int pc = pattern[p1][i - 1] - 'A';
                for (int x : by_letter[c]) {
                    dp[x][i] = inf;
                    for (int y : by_letter[pc]) {
                        if (dp[x][i] > dp[y][i - 1] + d[x][y]) {
                            dp[x][i] = dp[y][i - 1] + d[x][y];
                        }
                    }
                    if (i == 4) {
                        any[p1] = min(any[p1], dp[x][i]);
                    }
                }
            }
        }

        int lc = pattern[p1][4] - 'A';

        F0(p2, n) {
            prec[p1][p2] = inf;
            int p2c = pattern[p2][4] - 'A';
            for (int y : by_letter[p2c]) pend[p1][p2][y] = inf;

            int at = pm[p1][p2];
            for (int i = 5; i < 10 - pm[p1][p2]; i++, at++) {
                int c = pattern[p2][at] - 'A';
                int pc = i == 5 ? lc : pattern[p2][at - 1] - 'A';
                for (int x : by_letter[c]) {
                    dp[x][i] = inf;
                    for (int y : by_letter[pc]) {
                        if (dp[x][i] > dp[y][i - 1] + d[x][y]) {
                            dp[x][i] = dp[y][i - 1] + d[x][y];
                        }
                    }
                    if (i == 10 - pm[p1][p2] - 1) {
                        pend[p1][p2][x] = min(pend[p1][p2][x], dp[x][i]);
                        prec[p1][p2] = min(prec[p1][p2], dp[x][i]);
                    }
                }
            }
        }

        F0(p2, n) for (int lx : by_letter[lc]) {
            pbeg[p1][p2][lx] = inf;
            int at = pm[p1][p2];
            for (int i = 5; i < 10 - pm[p1][p2]; i++, at++) {
                int c = pattern[p2][at] - 'A';
                int pc = i == 5 ? lc : pattern[p2][at - 1] - 'A';
                for (int x : by_letter[c]) {
                    dp[x][i] = inf;

                    if (i == 5) {
                        int y = lx;
                        if (dp[x][i] > dp[y][i - 1] + d[x][y]) {
                            dp[x][i] = dp[y][i - 1] + d[x][y];
                        }
                    } else {
                        for (int y : by_letter[pc]) {
                            if (dp[x][i] > dp[y][i - 1] + d[x][y]) {
                                dp[x][i] = dp[y][i - 1] + d[x][y];
                            }
                        }
                    }
                    if (i == 10 - pm[p1][p2] - 1) {
                        pbeg[p1][p2][lx] = min(pbeg[p1][p2][lx], dp[x][i]);
                    }
                }
            }
        }
    }

    F0(p1, n) F0(p2, n) {
        int c = pattern[p2][4] - 'A';
        F0(p3, n) {
            pex[p1][p2][p3] = inf;
            for (int x : by_letter[c]) {
                int add = pend[p1][p2][x] - prec[p1][p2] + pbeg[p2][p3][x] - prec[p2][p3];
                if (add < pex[p1][p2][p3]) {
                    pex[p1][p2][p3] = add;
                    if (!add) break;
                }
            }
        }
    }
    PR(Elapsed());

    F0(p1, n) F0(p2, n) {
        prec[p1][p2] -= any[p2];
    }
}

int wh[N], nn[N], p[N], tmp[N], bp[N];
int xscore;
void UpdateBestOrder() {
    if (score < xscore) {
        xscore = score;
        F0(i, n + 2) bp[i] = p[i];
    }
}

int Dist(int x, int y) {
    //return 5 - pm[x][y];
    return prec[x][y];
}


int NaiveX(int* p) {
    int ret = 0;
    F0(i, n) ret += Dist(p[i], p[i + 1]);
    return ret;
}

int Naive2(int* p) {
    int ret = 0;
    F0(i, n) ret += Dist(p[i], p[i + 1]);
    for (int i = 1; i <= n - 2; i++) {
        ret += pex[p[i]][p[i+1]][p[i+2]];
    }
    return ret;
}

void Improve(double endTime = TL) {
    xscore = inf;
    if (n <= 1) return;
    int tot = 1, acc = 0;

    score = Naive2(p);

    PR(score);
    UpdateBestOrder();

    double T2 = 0.05 * score / n, T1 = 0.0*T2, TEMP = 0.0, r = 0.0;
    PR(T2);

    F1(i, n) wh[p[i]] = i;
    int seg_len = 200;

    double delta, delta2;
    int bi;

    int itc = 1000000;
#ifdef TIME_BASED
    double startTime = Elapsed(); itc = 1000000000;
#endif
    int i2 = 0;
    F0(iter, itc) {
        if ((iter & 255) == 0) {
#ifdef TIME_BASED
            r = (endTime - Elapsed()) / (endTime - startTime);
#else
            r = 1.0 * (itc - iter) / itc;
#endif
            if (r <= 0) {
                //PR(iter);
                break;
            }
            TEMP = T1 + (T2 - T1) * r;
        }
        if (++i2 > n) i2 = 1;
        int i = i2;
        int j = i;
        double maxd = TEMP * logs[my.nextInt(LOGN)];
        delta = inf;
        while (j == i) j = my.nextInt(1, n);
        if (i < j) {
            // .. [i'..i]...j... ->  ...i'-1 i+1 .... j[i'..i]...
            for (int i2 = i; i2 >= 1 && i2 >= i - seg_len; i2--) {
                delta2 = Dist(p[i2-1], p[i+1])
                         +Dist(p[j], p[i2])
                         - Dist(p[i2-1], p[i2]);
                if (delta2 < delta) {
                    delta = delta2;
                    bi = i2;
                }
            }
        } else {
            //continue;
            // ... j ... [i'..i] .. -> ... [i'..i] j .. i'-1 i+1
            for (int i2 = i; i2 > j && i2 >= i - seg_len; i2--) {
                delta2 = Dist(p[j-1], p[i2])
                         + Dist(p[i2-1], p[i+1])
                         - Dist(p[i2-1], p[i2]);
                if (delta2 < delta) {
                    delta = delta2;
                    bi = i2;
                }
            }
        }
        F1(x, n) sp[x] = p[x];
        int h = i + 1 - bi;
        for (int k = bi; k <= i; k++) tmp[k] = p[k];
        if (i < j) {
            for (int k = bi; k <= j - h; k++) {
                p[k] = p[k + h];
                wh[p[k]] = k;
            }
            for (int k = j - h + 1; k <= j; k++) {
                p[k] = tmp[k - j + i];
                wh[p[k]] = k;
            }
        } else {
            for (int k = i; k >= j + h; k--) {
                p[k] = p[k - h];
                wh[p[k]] = k;
            }
            for (int k = j; k < j + h; k++) {
                p[k] = tmp[k - j + bi];
                wh[p[k]] = k;
           }
        }
        int nscore = Naive2(p);
        delta = nscore - score;

        tot++;
        if (delta < maxd) {
            acc++;
            score += delta;
            UpdateBestOrder();
        } else {
            F1(x, n) {
                p[x] = sp[x];
                wh[p[x]] = x;
            }
        }
    }
    if (DBG) {
        cerr << acc << "/" << tot << " " << 100.0 * acc / tot << " " << c_o[1] << " " << c_o[2] << endl;
    }
}

bool Gen() {
    string x;
    F1(i, n) x += pattern[p[i]].substr(pm[p[i - 1]][p[i]]);

    int l = SZ(x);
    score = inf;
    int bx = -1;
    sol.clear();
    for (int i = l - 1; i >= 0; i--) {
        int c = x[i] - 'A';
        if (i == l - 1) {
            for (int x : by_letter[c]) dp[x][i] = 0;
        } else {
            int pc = x[i + 1] - 'A';
            for (int x : by_letter[c]) {
                dp[x][i] = inf;
                for (int y : by_letter[pc]) {
                    if (dp[x][i] > dp[y][i + 1] + d[x][y]) {
                        dp[x][i] = dp[y][i + 1] + d[x][y];
                        way[x][i] = y;
                    }
                }
                if (i == 0) {
                    if (dp[x][i] + d0[x] < score) {
                        score = dp[x][i] + d0[x];
                        bx = x;
                    }
                }
            }
        }
    }
    if (score < bscore) {
        F0(i, l) {
            sol.push_back(bx);
            bx = way[bx][i];
        }
        bscore = score;
        bsol = sol;
        return true;
    }
    return false;
}

void Solve() {
    Prepare();

    p[0] = p[n + 1] = n;
    F1(i, n) p[i] = i - 1;

    double t1 = Elapsed();
    int runs = 4;
    double deltas = (TL - t1) / runs;
    F0(xr, runs) {
        F0(x, n) {
            int i = my.nextInt(1, n);
            int j = my.nextInt(1, n);
            swap(p[i], p[j]);
        }

        t1 += deltas;
        Improve(t1);
        F1(i, n) p[i] = bp[i];
        Gen();
    }

    F0(i, SZ(bsol)) {
        cout << bsol[i] / m << " " << bsol[i] % m << endl;
    }

    //Report();
}


void ReadInput() {
    cin >> m >> n >> sx >> sy;
    F0(i, m) cin >> s[i];
    F0(i, n) cin >> pattern[i];
}

int main(int argc, char* argv[]) {
    Init();

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
        if(seed>=0 && seed<100) {
            char inp[128];
            sprintf(inp, "in/%04d.txt", seed);
            char outp[128];
            sprintf(outp, "out/%04d.txt", seed);
            ignore = freopen(inp, "r", stdin);
            ignore = freopen(outp, "w", stdout);
            ReadInput();
            Solve();
            cerr << "Seed #" << seed << " ";
            cerr << bscore << endl;
            //cout << "Score would be " << bscore << endl;
        } else {
            // Generate
            throw;
            Rand my(seed);
        }
    }

    return 0;
}
