const double TL1 = 3.9 * 1;
const double TL2 = 5.95 * 1;
//#define IT_BASED
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
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "["; F0(i,SZ(v)) { if (i>0) os << ","; os << v[i]; } os << "]"; return os;
}
template<class T>
ostream& operator<<(ostream& os, const set<T>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ","; cerr << i; } os << "}" << endl; return os;
}

#pragma GCC optimize("O3")

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
Rand my(2023);
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
const int MCUT = 1 << 10;
const int N = 1 << 10;
const int M = 3 << 10;
const int R = 500;
const int LOGN = 1 << 16;
double logs[LOGN];
enum { UP, RIGHT, DOWN, LEFT };
const int DX[]={-1,0,1,0};
const int DY[]={0,1,0,-1};
const string DS="URDL";

int n, m, k, D, K, K0, nn;
ll score, bscore;
int que[N], qind, qn;
int d2[N][N], we[N][N];
pii P[N];
vector<int> srt[N];
int repair[M], brepair[M];
vector<pii> adj[N];
vector<int> ee[M];
vector<int> adje[N];
int an, en, cday;
int vused[M], vz;
int psize[32];
int d0[N][N];
int dist[N];
int es[4*M], af[N];
ll edge_loss[M], edge_loss2[M][M];
vector<pair<double, int>> h[M];
int md = 1;
int ce[16];
int u[M], v[M], w[M];
int cdist[32][MCUT][N];
int disabled[2*M], nond[2*M];
int cn[MCUT], dn[MCUT];
pii pa[MCUT][M], qi[MCUT][M];

#define ID(x, y) ((x)*n+(y))
#define PID(p) ID((p).first, (p).second)
#define X(z) ((z)/n)
#define Y(z) ((z)%n)
#define XY(z) pii((z)/n,(z)%n)

int d1(int x, int y) { return min(abs(x-y), n-abs(x-y)); }
int d1(pii p1, pii p2) { return d1(p1.first,p2.first)+d1(p1.second,p2.second); }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
bool In2(pii p) { return p.first >= 1 && p.first < n-1 && p.second >= 1 && p.second < n-1; }
//bool Good(pii p) { return In(p) && a[p.first][p.second] != -1; }
int GetDir(pii p1, pii p2) { F0(k, 4) if (d1(p1, p2) > d1(Move(p1, k), p2)) return k; return -1; }
int sqr(int x) { return x*x; }
template<class T>
void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }

int FindClosest(int x, int y) {
    ll bv = inf;
    int ret = 0;
    F1(i, n) {
        ll av = (P[i].first-x)*(P[i].first-x) +
                (P[i].second-y)*(P[i].second-y);
        if (av < bv) {
            bv = av;
            ret = i;
        }
    }
    return ret;
}

void SPT2(int r) {
    nn = 0;
    for (int i : srt[r]) if (!disabled[i]) nond[nn++] = i;
    F1(i, n) dist[i] = inf;
    dist[r] = 0;

    int sg = 1, i = 0;
    while (sg) {
        sg = 0;
        F0(ii, nn) {
            i = nond[ii];
            if (dist[v[i]] > dist[u[i]] + w[i]) {
                dist[v[i]] = dist[u[i]] + w[i];
                sg = 1;
            }

            if (dist[u[i]] > dist[v[i]] + w[i]) {
                dist[u[i]] = dist[v[i]] + w[i];
                sg = 1;
            }
        }
    }
}

void PrepareSRT(vector<int> a) {
    for (int i : a) {
        F0(j, m) srt[i].push_back(j);
        sort(srt[i].begin(), srt[i].end(), [&](int x, int y) {
            return min(d2[i][u[x]], d2[i][v[x]]) < min(d2[i][u[y]], d2[i][v[y]]);
        });

        SPT2(i);
        F1(j, n) {
            d0[i][j] = dist[j];
        }

        sort(srt[i].begin(), srt[i].end(), [&](int x, int y) {
            return min(d0[i][u[x]],d0[i][v[x]]) < min(d0[i][u[y]], d0[i][v[y]]);
        });
    }
}

void GenSample(int cut) {
    an = 0;
    set<int> S;
    F0(i, cut) {
        double angle = 2 * pi * i / cut;
        af[an++] = FindClosest(R + R * cos(angle), R + R * sin(angle));
        S.insert(af[an-1]);
    }
    int x = an;
    F1(i, n) if (!S.count(i)) af[x++] = i; assert(x == n);
}

void Prepare() {
    PR(n); PR(m); PR(D); PR(K); PR(1. * m / n); PR(1. * D * K / m);
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
    K0 = (m + D - 1) / D + 1;

    F0(i, m) {
        adje[u[i]].push_back(i);
        adje[v[i]].push_back(i);
        adj[u[i]].push_back(pii(v[i], w[i]));
        adj[v[i]].push_back(pii(u[i], w[i]));
        we[u[i]][v[i]] = we[v[i]][u[i]] = i;
    }

    F1(i, n) {
        sort(adj[i].begin(), adj[i].end(), [&](pii& x, pii& y) {
            return x.second < y.second;
        });
    }

    F1(i, n) for (int e1 : adje[i]) for (int e2 : adje[i]) if (e1 != e2) {
        ee[e1].push_back(e2);
    }

    F1(i, n) F1(j, n) d2[i][j] = (P[i].first-P[j].first)*(P[i].first-P[j].first) + (P[i].second-P[j].second)*(P[i].second-P[j].second);

    vector<int> u; F1(i, n) u.push_back(i);
    PrepareSRT(u);

    bscore = 1e18;
}

void UpdateBest() {
    if (score < bscore) {
        F0(i, m) brepair[i] = repair[i];
        bscore = score;
    }
}

bool IsShortcut(int day, int e) {
    vz++;
    int x = u[e], y = v[e];
    vused[x] = vz;
    qn = 1; que[0] = x;

    for (int qind = 0; qind < qn; qind++) {
        int x = que[qind];
        for (const pii& p : adj[x]) if (repair[we[x][p.first]] != day && vused[p.first] != vz) {
            if (y == p.first) return true;
            vused[p.first] = vz;
            que[qn++] = p.first;
            //if (qn++ > 64) return false;
        }
    }
    return false;
}

void Greedy() {
    F0(e, m) repair[e] = D;
    F0(d, D) psize[d] = 0;
    F0(e, m) {
        int d = my.nextInt(D);
        int f = 0;
        F0(u, D) {
            if (psize[d] < K) {
                repair[e] = d;
                if (IsShortcut(d, e)) {
                    psize[d]++;
                    f = 1;
                    break;
                }
            }
            if (++d == D) d = 0;
        }
        if (!f) {
            repair[e] = d;
            psize[d]++;
        }
    }
}

ll AddEdge(int ndist[N], int ai, int e) {
    vz++;
    en = 0;

    ll delta = 0;
    if (ndist[u[e]] + w[e] < ndist[v[e]]) {
        delta -= ndist[v[e]] - (ndist[u[e]] + w[e]);
        vused[v[e]] = vz;
        pa[ai][cn[ai]++] = pii(v[e], ndist[v[e]]);
        ndist[v[e]] = ndist[u[e]] + w[e];
        es[en++] = v[e];
    }
    if (ndist[v[e]] + w[e] < ndist[u[e]]) {
        delta -= ndist[u[e]] - (ndist[v[e]] + w[e]);
        vused[u[e]] = vz;
        pa[ai][cn[ai]++] = pii(u[e], ndist[u[e]]);
        ndist[u[e]] = ndist[v[e]] + w[e];
        es[en++] = u[e];
    }

    F0(index, en) {
        int x = es[index];
        for (const pii& p : adj[x]) {
            if (repair[we[x][p.first]] != cday && ndist[p.first] > ndist[x] + p.second) {
                delta -= ndist[p.first] - (ndist[x] + p.second);
                if (vused[p.first] != vz) {
                    vused[p.first] = vz;
                    pa[ai][cn[ai]++] = pii(p.first, ndist[p.first]);
                }
                es[en++] = p.first;
                ndist[p.first] = ndist[x] + p.second;
            }
        }
    }
    return delta;
}

ll RemoveEdge(int ndist[N], int ai, int e) {
    vz++;
    en = 0;
    if (ndist[v[e]] + w[e] == ndist[u[e]]) {
        es[en++] = u[e];
        vused[u[e]] = vz;
    }
    if (ndist[u[e]] + w[e] == ndist[v[e]]) {
        es[en++] = v[e];
        vused[v[e]] = vz;
    }

    F0(index, en) {
        int x = es[index];
        for (const pii& p : adj[x]) if (ndist[x] + p.second == ndist[p.first]) {
            if (vused[p.first] != vz) {
                vused[p.first] = vz;
                es[en++] = p.first;
            }
        }
    }
    ll delta = 0;
    dn[ai] = en;

    F0(index, en) {
        qi[ai][index] = pii(es[index], ndist[es[index]]);
        delta -= ndist[es[index]];
        ndist[es[index]] = inf;
    }
    int sg = 1;
    while (sg) {
        sg = 0;
        F0(index, en) {
            int x = es[index];
            for (const pii& p : adj[x]) if (ndist[x] > p.second + ndist[p.first] && repair[we[x][p.first]] != cday) {
                ndist[x] = p.second + ndist[p.first];
                sg = 1;
            }
        }
    }
    F0(index, en) {
        delta += ndist[es[index]];
    }
    return delta;
}

bool Gen(int& j, int& pi, int& pj) {
    j = (j + 1) % m;

    pj = repair[j];
    // j gets added, check where it should get removed
    pi = -1;

    if (my.nextInt(4)) {
        vector<pair<double, int>> v;
        for (auto p : h[j]) if (repair[p.second] != pj
                                && psize[repair[p.second]] < K) {
            v.push_back(p);
        }
        double ps = 0.0;
        for (auto p : v) ps += p.first;
        if (ps) {
            double q = my.nextDouble() * ps;
            for (auto p : v) {
                q -= p.first;
                if (q <= 0) {
                    pi = repair[p.second];
                    break;
                }
            }
        }
    }

    if (pi == -1) {
        pi = my.nextInt(D);
        int f = 0;
        F0(u, D) {
            if (psize[pi] < K && pi != pj) {
                f = 1; break;
            }
            if (++pi >= D) pi = 0;
        }
        if (!f) return false;;
    }
    return true;
}

void HC() {
    F0(d, D) psize[d] = 0;
    F0(i, m) psize[repair[i]]++;

    F0(d, D) {
        F0(x, m) disabled[x] = repair[x] == d;
        F0(ai, an) {
            SPT2(af[ai]);
            F1(j, n) {
                cdist[d][ai][j] = dist[j];
            }
        }
    }

    PR(Elapsed());

    int j = 0, pi = 0, pj = 0;

    double T2 = 3.5 * n / m * an * 100000.0, T1 = 0.0 * T2, TEMP = 0.0, r = 0;
    int acc = 0, tot = 1;
    double startTime = Elapsed(), endTime = TL1;
    int itc = 1000000000;
#ifdef IT_BASED
    itc = 400000;
#endif

    score = 0;
    UpdateBest();

    F0(iter, itc) {
        if ((iter & 15) == 0) {
#ifdef IT_BASED
            r = 1.0 * (itc - iter) / itc;
#else
            r = (endTime - Elapsed()) / (endTime - startTime);
#endif
            if (r <= 0) {
                cerr << iter << " " << acc << "/" << tot << " " << 100.0*acc / tot << endl;
                break;
            }
            TEMP = T1 + (T2 - T1) * r * r;
        }
        double maxd = TEMP * logs[my.nextInt(LOGN)];

        if (!Gen(j, pi, pj)) continue;

        repair[j] = pi;
        //if (!IsShortcut(pi, j)) { repair[j] = pj; continue; }

        F0(ai, an) cn[ai] = dn[ai] = 0;

        cday = pj;
        ll delta = 0;
        F0(ai, an) {
            delta += AddEdge(cdist[pj][ai], ai, j);
        }
        cday = pi;
        F0(ai, an) {
            delta += RemoveEdge(cdist[pi][ai], ai, j);
            if (delta > maxd) break;
        }

        if (delta > 0) tot++;
        if (delta <= maxd) {
            score += delta;
            if (r < 0.1) UpdateBest();
            if (delta > 0) acc++;
            psize[pi]++;
            psize[pj]--;
        } else {
            repair[j] = pj;

            F0(ai, an) {
                F0(j, dn[ai]) {
                    cdist[pi][ai][qi[ai][j].first] = qi[ai][j].second;
                }
            }

            F0(ai, an) {
                F0(j, cn[ai]) {
                    cdist[pj][ai][pa[ai][j].first] = pa[ai][j].second;
                }
            }
        }
    }
}

void Go(int at, int e, ll delta) {
    ce[at] = e;
    repair[e] = 0;
    F0(ai, an) dn[ai + at * an] = 0;
    F0(ai, an) {
        delta += RemoveEdge(cdist[0][ai], ai + at * an, e);
    }

    if (at == 0) edge_loss[ce[0]] = delta;
    if (at == 1) edge_loss2[ce[0]][ce[1]] = edge_loss2[ce[1]][ce[0]] = delta;

    if (at == md) {
    } else {
        for (int e2 : ee[e]) if (e2 > e) Go(at + 1, e2, delta);
    }

    repair[e] = D;
    F0(ai, an) {
        F0(j, dn[ai + at*an]) {
            cdist[0][ai][qi[ai + at*an][j].first] = qi[ai + at*an][j].second;
        }
    }
}

void SA() {
    int steps = 0;
    int tmp = an;
    while (tmp < n) {
        tmp *= 4;
        steps++;
    }
    PR(steps);
    double startTime = Elapsed(), endTime = TL2;

    F0(i, m) repair[i] = brepair[i]; score = bscore;
    F0(d, D) psize[d] = 0;
    F0(i, m) psize[repair[i]]++;

F1(step, steps) {
    double TL = startTime + (endTime - startTime) * step / steps;
    int new_an = min(4*an, n);
    if (step == 1) an = 0;
    F0(d, D) {
        F0(x, m) disabled[x] = repair[x] == d;
        for (int ai = an; ai < new_an; ai++) {
            SPT2(af[ai]);
            F1(j, n) {
                cdist[d][ai][j] = dist[j];
            }
        }
    }
    an = new_an;
    int j = 0, pi = 0, pj = 0;

    int acc = 0, tot = 1;
    int itc = 1000000000;
    ll maxd = 0;

    F0(iter, itc) {
        if (Elapsed() > TL) {
            cerr << iter << " " << acc << "/" << tot << " " << 100.0*acc / tot << endl;
            break;
        }

        if (!Gen(j, pi, pj)) continue;

        repair[j] = pi;
        if (!IsShortcut(pi, j)) { repair[j] = pj; continue; }

        F0(ai, an) cn[ai] = dn[ai] = 0;

        cday = pj;
        ll delta = 0;
        F0(ai, an) {
            delta += AddEdge(cdist[pj][ai], ai, j);
        }
        cday = pi;
        F0(ai, an) {
            delta += RemoveEdge(cdist[pi][ai], ai, j);
            if (delta > maxd) break;
        }

        tot++;
        if (delta <= maxd) {
            score += delta;
            acc++;
            psize[pi]++;
            psize[pj]--;
        } else {
            repair[j] = pj;

            F0(ai, an) {
                F0(j, dn[ai]) {
                    cdist[pi][ai][qi[ai][j].first] = qi[ai][j].second;
                }
            }

            F0(ai, an) {
                F0(j, cn[ai]) {
                    cdist[pj][ai][pa[ai][j].first] = pa[ai][j].second;
                }
            }
        }
    }
}
    UpdateBest();
}


void Synergies() {
    GenSample(20);

    F0(i, m) repair[i] = D;
    cday = 0;
    F0(ai, an) {
        F1(j, n) {
            cdist[0][ai][j] = d0[af[ai]][j];
        }
    }

    F0(e, m) {
        Go(0, e, 0);
    }

    F0(i, m) {
        vector<pair<ll,int>> v;
        for (int j : ee[i]) {
            ll w = (edge_loss[i] + edge_loss[j]) - edge_loss2[i][j];
            if (w > 0) v.push_back(make_pair(w, j));
        }
        if (!v.empty()) {
            ll mx = 0;
            for (auto p : v) if (p.first > mx) mx = p.first;
            for (auto p : v) {
                double x = 1.0 * p.first / mx;
                h[i].push_back(make_pair(x, p.second));
            }
        }
    }
}


// #0: 3971927
// #1: 7059845
// #4: 7277724
void Solve() {
    Prepare();

    score = 0;

    Synergies();
    Greedy();
    HC();
    SA();

    F0(i, m) cout << brepair[i] + 1 << " ";

    //PR(bscore);
    //Report();
}





void ReadInput() {
    cin >> n >> m >> D >> K;
    F0(i, m) cin >> u[i] >> v[i] >> w[i];
    F1(i, n) cin >> P[i].first >> P[i].second;
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
        if(seed>=0 && seed<1000) {
            char inp[128];
            sprintf(inp, "in/%04d.txt", seed);
            char outp[128];
            sprintf(outp, "out/%04d.txt", seed);
            ignore = freopen(inp, "r", stdin);
            ignore = freopen(outp, "w", stdout);
            ReadInput();
            Solve();
            //cout << "Score would be " << bscore << endl;
        } else {
            // Generate
            throw;
            Rand my(seed);
        }
    }

    return 0;
}
