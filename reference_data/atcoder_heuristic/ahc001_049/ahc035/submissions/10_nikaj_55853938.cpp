int DBG = 0;
double TL = 1.0;
//#pragma GCC optimize("O3,Ofast,omit-frame-pointer,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize")
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
#include <chrono>

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

#ifdef ATCODERX
inline ll GetTSC() {
    ll lo, hi;
    asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
    return lo + (hi << 32);
}
inline double GetSeconds() {
    return GetTSC() / 3.0e9;
}
#else
chrono::system_clock::time_point init_time = chrono::system_clock::now();
inline double GetSeconds() {
     chrono::system_clock::time_point current_time = chrono::system_clock::now();
     double ret =  chrono::duration_cast<std::chrono::nanoseconds>(current_time - init_time).count();
     return ret * 1e-9;
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
Rand my(2026);
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
const int N = 8;
const int DX[]={-1,0,1,0};
const int DY[]={0,1,0,-1};
int n, m, T, n2, turn, mx;
ll bscore, score;
vector<pii> bsol, sol;
int a[N*N], b[N*N], w[N*N], x[64][16], sums[64];
vector<int> adj[N*N];
int q[N*N][N*N];
bool are_adj[N*N][N*N];
int maxn[64], maxes[64][64], cmax[64], CB;

const int LOGN = 1 << 16;
double logs[LOGN];

template<class T> void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }
template<class T> T sqr(T x) { return x*x; }

pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }

void Prepare() {
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);

    mx = 0;
    F0(i, n2) F0(j, m) mx = max(mx, x[i][j]);

    F0(i, n*n) {
        pii p(i / n, i % n);
        F0(k, 4) {
            pii q = Move(p, k);
            if (In(q)) {
                ::q[i][q.first * n + q.second] = 1;
                are_adj[i][q.first * n + q.second] = true;
                adj[i].push_back(q.first * n + q.second);
            }
        }
    }
    F0(i, n*n) F0(j, n*n) if (q[i][j]) {
        int b1 = SZ(adj[i]) == 4;
        int b2 = SZ(adj[j]) == 4;
        q[i][j] = (b1 + 1) * (b2 + 1) ;
    }
}

void UpdateBest() {
    if (score > bscore) {
        bscore = score;
        F0(i, n*n) b[i] = a[i];
    }
}

int N1(int i, int j) {
    int ret = 0;
    F0(k, m) {
        int v1 = max(x[i][k], x[j][k]);
        int v2 = x[i][k] ^ x[j][k] ^ v1;
        ret += ((40 - turn) * v1 * v1 + turn * v2 * v2);
    }
    return ret / 40;
}

int Calc(int cnt) {
    return (min(cnt, (10 - turn) / 2) * CB * CB);
}

int Naive() {
    int ret = 0;
    F0(i, n*n) for (int j : adj[i]) if (i < j) {
        ret += N1(a[i], a[j])*q[i][j];
    }
    F0(j, m) ret += Calc(cmax[j]);
    return ret;
}

#define TIME_BASED
void Improve() {
    int tot = 1, acc = 0;
    double T2 = mx * mx * 0.3, T1 = 0.05, TEMP = 0.0, r = 0.0;
    int itc = 120000000;
#ifdef TIME_BASED
    double startTime = Elapsed();
    double endTime = 1.95 * (turn + 1) / T; itc = 1000000000;
#endif
    F0(iter, itc) {
        if ((iter & 127) == 0) {
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
            //TEMP = pow(T1, 1-r) * pow(T2, r);
        }

        int i = my.nextInt(n*n);
        int j = my.nextInt(n2);
        while (j == a[i]) j = my.nextInt(n2);
        int delta = 0;
        if (w[j] == -1) {
            F0(index, maxn[a[i]]) {
                delta -= Calc(cmax[maxes[a[i]][index]]);
                delta += Calc(--cmax[maxes[a[i]][index]]);
            }
            F0(index, maxn[j]) {
                delta -= Calc(cmax[maxes[j][index]]);
                delta += Calc(++cmax[maxes[j][index]]);
            }

            for (int i2 : adj[i]) delta -= N1(a[i], a[i2])*q[i][i2];
            for (int i2 : adj[i]) delta += N1(j, a[i2])*q[i][i2];
        } else {
            int pj = w[j];
            if (are_adj[i][pj]) delta -= 2*N1(a[i], j)*q[i][pj];
            for (int i2 : adj[i]) delta -= N1(a[i], a[i2])*q[i][i2];
            for (int i2 : adj[i]) delta += N1(j, a[i2])*q[i][i2];
            for (int i2 : adj[pj]) delta -= N1(j, a[i2])*q[pj][i2];
            for (int i2 : adj[pj]) delta += N1(a[i], a[i2])*q[pj][i2];
        }

        tot++;
        if (delta >= 0 || delta >= -TEMP * logs[my.nextInt(LOGN)]) {
            acc++;
            if (w[j] == -1) {
                w[a[i]] = -1;
                w[j] = i;
                a[i] = j;
            } else {
                int pj = w[j];
                swap(a[i], a[pj]);
                w[a[pj]] = pj;
                w[a[i]] = i;
            }
            score += delta;
            UpdateBest();

            //assert(Naive() == score &&  "match");
        } else {
            if (w[j] == -1) {
                F0(index, maxn[a[i]]) cmax[maxes[a[i]][index]]++;
                F0(index, maxn[j]) cmax[maxes[j][index]]--;
            }
        }
    }
    if (1) {
        cerr << acc << "/" << tot << " " << 100.0 * acc / tot << " " << score << "/" << bscore << endl;
    }
}

int maxsum, potsum, base_potsum = -1;
pii pot[N*N];

void CalcSums() {
    maxsum = -1;
    F0(i, n2) {
        maxn[i] = 0;
        sums[i] = 0;
        F0(j, m) {
            sums[i] += x[i][j];
        }
        maxsum = max(maxsum, sums[i]);
    }
    potsum = 0;
    F0(j, m) {
        pii p(-1, 0);
        F0(i, n2) {
            if (x[i][j] > p.first) p = {x[i][j], 1};
            else if (x[i][j] == p.first) p.second++;
        }
        cerr << p;
        pot[j] = p;
        potsum += p.first;

        F0(i, n2) if (x[i][j] == p.first) maxes[i][maxn[i]++] = j;
    }

    cerr << endl;
    PR(potsum);
    if (base_potsum == -1) base_potsum = potsum;
}

void Solve() {
    Prepare();

    for (turn = 0; turn < T; turn++) {
        cerr << endl;

        CalcSums();
        vector<int> ids;
        F0(i, n2) {
            ids.push_back(i);
        }
        sort(ids.begin(), ids.end(), [&](int i1, int i2) {
            return sums[i1] > sums[i2];
        });

        F0(j, m) cmax[j] = 0;
        F0(i, n2) {
            if (i < n*n) {
                a[i] = i;
                w[i] = i;
                F0(j, maxn[i]) cmax[maxes[i][j]]++;
            } else {
                w[i] = -1;
            }
        }

        CB = 3*mx;
        score = Naive();
        bscore = -1;
        UpdateBest();
        Improve();

        F0(j, m) {
            int cnt = 0;
            F0(i, n*n) if (x[b[i]][j] == pot[j].first) cnt += SZ(adj[i]);
            cerr << cnt << " ";
        }
        cerr << endl;

        F0(i, n*n) {
            cout << b[i];
            if (i % n == n - 1) cout << endl; else cout << " ";
        }

        F0(i, n2) {
            F0(j, m) cin >> x[i][j];
        }
        //CalcSums();
        //cerr << turn << " : " << maxsum << " " << maxsum * 1e6 / base_potsum << " " << potsum * 1e6 / base_potsum << endl;
    }

    //Report();
}


void ReadInput() {
    cin >> n >> m >> T;
    n2 = 2 * n * (n - 1);
    F0(i, n2) F0(j, m) cin >> x[i][j];
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
