int DBG = 0;
double TL = 2.7;
double PC = 0.49;
#pragma GCC optimize("O3,Ofast,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize")
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
#include <bitset>
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
void print_all() { cerr << endl; }
template <typename H, typename... T>
void print_all(H head, T... tail) { cerr << " " << head; print_all(tail...); }
#ifdef LOCAL
#define PR(...) do { cerr << #__VA_ARGS__ << " ="; print_all(__VA_ARGS__); } while(0)
#else
#define PR(...)
#endif
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

#ifdef EC2
inline ll GetTSC() {
    ll lo, hi;
    asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
    return lo + (hi << 32);
}
inline double GetSeconds() {
    return GetTSC() / 2.6e9;
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
    inline ll zh() { mix(); return x; }
    inline int nextInt(int n) { return rand() % n; }
    inline int nextInt(int L, int R) { return rand()%(R - L + 1) + L; }
    inline int nextBool() { if (o < 4) o = rand(); o >>= 2; return o & 1; }
    double nextDouble() { return rand() * 1.0 / MAX_RAND; }
    double nextDouble(double L, double R) { return (R - L) + nextDouble() + L; }
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

enum { RIGHT, DOWN, LEFT, UP, STAY };
const string HS = "RDLU.";
const string ROTS = "L.R";
const int DX[]={0,1,0,-1,0,0,0,0};
const int DY[]={1,0,-1,0,0,0,0,0};
int d1(int x, int y) { return abs(x-y); }
int d1(pii p1, pii p2) { return d1(p1.first,p2.first)+d1(p1.second,p2.second); }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
pii Add(pii p, pii q) { return pii(p.first + q.first, p.second + q.second); }
pii MoveD(pii p, int k, int d) { return pii(p.first + d * DX[k], p.second + d * DY[k]); }

// CONSTANTS
const int BITS = 450;
const int N = 32;
int BW = 64 * 1;
double bw_coeff = 1.0;
int n, m, V, version, versions, bversion;
ll bscore, score;
typedef ll MoveType;
vector<MoveType> bsol, sol;
string A[N], B[N];
int id[N][N];
vector<pii> p01, p10;
ll zh[N][N], zh2[16];
pii vert[16];
int orient[16];
char dirs[16];
int lp, mb;
bitset<BITS> inters;
bitset<BITS> r01[6][3*N][3*N][16], r10[6][3*N][3*N][16];
int rn[1024][16][N][N];
char r[1024][16][N][N][4];
short rp[1024][16][N][N][4];
vector<pii> start_positions;
bool reaches[N][N];
typedef ll TreeState;

bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
bool Good(pii p) { return p.first >= -n && p.first < 2*n && p.second >= -n && p.second < 2*n; }
int GetDir(pii p1, pii p2) { F0(k, 4) if (d1(p1, p2) > d1(Move(p1, k), p2)) return k; return -1; }
template<class T> auto REL(T& v) { return v[my.nextInt(SZ(v))]; }
template<class T> void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }
template<class T> T sqr(T x) { return x*x; }
ll Hash(pii p) { return (p.first << 5) | p.second; }

inline TreeState MakeState(int prev, MoveType lm) {
    return ((ll)(prev) << mb) | lm;
}

inline int GetPrev(TreeState x) {
    return x >> mb;
}

inline MoveType GetMove(TreeState x) {
    return x & ((1LL << mb) - 1);
}

struct State {
    vector<int> p, L, o, c;
    vector<pii> coord;

    void ApplyFix(int& mask, int i) {
        while (i != 0) {
            mask |= (1 << i);
            i = p[i];
        }
    }

    void CalcCoord() {
        coord[0] = {0, 0};
        F1(i, V - 1) {
            c[i] = (c[p[i]] + o[i]) % 4;
            coord[i] = MoveD(coord[p[i]], c[i], L[i]);
        }
    }

    void Ready() {
        o.resize(V);
        vector<bool> is_leaf(V, true);

        F1(i, V - 1) {
            is_leaf[p[i]] = false;
            o[i] = 0;
        }

        lp = 0;
        while (!is_leaf[lp]) lp++;
        leaf_pos = lp;

        mb = (3 * V + 1) - lp;

        CalcCoord();
    }

    void CreateTree(int n) {
        p.resize(V);
        L.resize(V);
        coord.resize(V);
        c.resize(V);

        if (V == 5) {
            // TODO
            p = {0, 0, 1, 1, 1};
            if (n == 15) L = {0, 6, 2, 3, 4};
            if (n == 16) L = {0, 6, 2, 3, 5};
            if (n == 17) L = {0, 6, 2, 5, 4};
            if (n == 18) L = {0, 6, 3, 4, 5};
            if (n == 19) L = {0, 7, 3, 4, 5};
            if (n == 20) L = {0, 7, 3, 4, 5};
            if (n == 21) L = {0, 7, 3, 4, 5};
            if (n == 22) L = {0, 8, 3, 4, 5};
            if (n == 23) L = {0, 8, 3, 5, 6};
            if (n == 24) L = {0, 8, 4, 5, 6};
            if (n == 25) L = {0, 9, 4, 5, 7};
            if (n == 26) L = {0, 10, 4, 5, 7};
            if (n == 27) L = {0, 10, 4, 6, 7};
            if (n == 28) L = {0, 10, 5, 6, 7};
            if (n == 29) L = {0, 10, 5, 6, 8};
            if (n == 30) L = {0, 11, 5, 6, 8};
            BW = 240000 / m;
        }

        if (V == 6) {
            p = {0, 0, 1, 2, 2, 2};
            if (n == 15) {
                p = {0, 0, 1, 1, 1, 1};
                L = {0, 5, 1, 2, 3, 4};
            }
            if (n == 16) L = {0, 5, 3, 1, 2, 3};
            if (n == 17) L = {0, 6, 3, 1, 2, 3};
            if (n == 18) L = {0, 7, 4, 1, 2, 3};
            if (n == 19) L = {0, 7, 4, 1, 2, 3};
            if (n == 20) L = {0, 7, 4, 1, 2, 3};
            if (n == 21) L = {0, 8, 4, 1, 2, 3};
            if (n == 22) L = {0, 8, 4, 1, 2, 3};
            if (n == 23) L = {0, 9, 5, 1, 2, 3};
            if (n == 24) L = {0, 9, 5, 1, 2, 3};
            if (n == 25) L = {0, 9, 5, 1, 2, 3};
            if (n == 26) L = {0, 9, 5, 2, 3, 4};
            if (n == 27) L = {0, 10, 5, 2, 3, 4};
            if (n == 28) L = {0, 10, 5, 2, 3, 4};
            if (n == 29) L = {0, 11, 6, 2, 3, 4};
            if (n == 30) L = {0, 11, 6, 2, 3, 4};
            BW = 200000 / m;
        }

        if (V == 7) {
            p = {0, 0, 1};
            if (n == 15) L = {0, 5, 2};
            if (n == 16) L = {0, 6, 2};
            if (n == 17) L = {0, 6, 3};
            if (n == 18) L = {0, 6, 4};
            if (n == 19) L = {0, 6, 4};
            if (n == 20) L = {0, 6, 4};
            if (n == 21) L = {0, 7, 5};
            if (n == 22) L = {0, 7, 5};
            if (n == 23) L = {0, 8, 5};
            if (n == 24) L = {0, 8, 5};
            if (n == 25) L = {0, 9, 5};
            if (n == 26) L = {0, 10, 5};
            if (n == 27) L = {0, 10, 5};
            if (n == 28) L = {0, 10, 5};
            if (n == 29) L = {0, 11, 6};
            if (n == 30) L = {0, 11, 6};
            F1(i, 4) p.push_back(2);
            F1(i, 4) L.push_back(i);
            BW = 200000 / m;
        }

        if (V == 8) {
            p = {0, 0, 1};
            if (n == 15) L = {0, 5, 2};
            if (n == 16) L = {0, 5, 3};
            if (n == 17) L = {0, 5, 3};
            if (n == 18) L = {0, 6, 3};
            if (n == 19) L = {0, 6, 3};
            if (n == 20) L = {0, 6, 4};
            if (n == 21) L = {0, 7, 4};
            if (n == 22) L = {0, 7, 4};
            if (n == 23) L = {0, 7, 5};
            if (n == 24) L = {0, 8, 5};
            if (n == 25) L = {0, 8, 5};
            if (n == 26) L = {0, 8, 6};
            if (n == 27) L = {0, 9, 6};
            if (n == 28) L = {0, 9, 6};
            if (n == 29) L = {0, 10, 6};
            if (n == 30) L = {0, 11, 6};
            F1(i, 5) p.push_back(2);
            F1(i, 5) L.push_back(i);
            BW = 300000 / m;
        }

        if (V == 9) {
            p = {0, 0, 1, 2};
            if (n == 15) L = {0, 4, 3, 2};
            if (n == 16) L = {0, 5, 3, 2};
            if (n == 17) L = {0, 5, 4, 2};
            if (n == 18) L = {0, 5, 4, 2};
            if (n == 19) L = {0, 6, 4, 2};
            if (n == 20) L = {0, 6, 5, 2};
            if (n == 21) L = {0, 6, 5, 3};
            if (n == 22) L = {0, 7, 5, 3};
            if (n == 23) L = {0, 7, 5, 3};
            if (n == 24) L = {0, 7, 5, 3};
            if (n == 25) L = {0, 7, 5, 3};
            if (n == 26) L = {0, 8, 5, 3};
            if (n == 27) L = {0, 8, 6, 3};
            if (n == 28) L = {0, 8, 6, 4};
            if (n == 29) L = {0, 8, 6, 4};
            if (n == 30) L = {0, 9, 6, 4};
            for (int i = 4; i < V; i++) p.push_back(3);
            for (int i = 4; i < V; i++) L.push_back(i-3);
            if (n <= 28) {
                p.back() = 2;
                L.back() = L[3];
            }
            BW = 220000 / m;
        }

        if (V == 10) {
            p = {0, 0, 1, 2};
            if (n == 15) L = {0, 4, 3, 2};
            if (n == 16) L = {0, 5, 3, 2};
            if (n == 17) L = {0, 5, 4, 2};
            if (n == 18) L = {0, 5, 4, 2};
            if (n == 19) L = {0, 6, 4, 2};
            if (n == 20) L = {0, 6, 5, 2};
            if (n == 21) L = {0, 6, 5, 2};
            if (n == 22) L = {0, 6, 5, 3};
            if (n == 23) L = {0, 6, 5, 3};
            if (n == 24) L = {0, 7, 5, 3};
            if (n == 25) L = {0, 7, 5, 3};
            if (n == 26) L = {0, 7, 5, 3};
            if (n == 27) L = {0, 8, 5, 3};
            if (n == 28) L = {0, 8, 5, 4};
            if (n == 29) L = {0, 8, 6, 4};
            if (n == 30) L = {0, 9, 6, 4};

            F1(i, 5) {
                p.push_back(3);
                L.push_back(i);
            }
            F1(i, 1) {
                p.push_back(2);
                L.push_back(L[3]);
            }
            BW = 220000 / m;
        }

        if (V == 11) {
            p = {0, 0, 1, 2};
            if (n == 15) L = {0, 4, 3, 2};
            if (n == 16) L = {0, 5, 3, 2};
            if (n == 17) L = {0, 5, 3, 2};
            if (n == 18) L = {0, 6, 3, 2};
            if (n == 19) L = {0, 6, 3, 2};
            if (n == 20) L = {0, 6, 4, 2};
            if (n == 21) L = {0, 6, 4, 3};
            if (n == 22) L = {0, 6, 4, 3};
            if (n == 23) L = {0, 6, 5, 3};
            if (n == 24) L = {0, 7, 5, 3};
            if (n == 25) L = {0, 7, 5, 4};
            if (n == 26) L = {0, 7, 6, 4};
            if (n == 27) L = {0, 8, 6, 4};
            if (n == 28) L = {0, 8, 6, 4};
            if (n == 29) L = {0, 8, 6, 4};
            if (n == 30) L = {0, 9, 6, 4};

            if (n <= 16) {
                F1(i, 4) {
                    p.push_back(3);
                    L.push_back(i);
                }
                F1(i, 3) {
                    p.push_back(2);
                    L.push_back(i);
                }
            } else if (n <= 18) {
                F1(i, 5) {
                    p.push_back(3);
                    L.push_back(i);
                }
                F1(i, 2) {
                    p.push_back(2);
                    L.push_back(i);
                }
            } else {
                F1(i, 6) {
                    p.push_back(3);
                    L.push_back(i);
                }
                F1(i, 1) {
                    p.push_back(2);
                    L.push_back(L[3]);
                }
            }
            BW = 210000 / m;
        }

        if (V == 12) {
            p = {0, 0, 1, 2};
            if (n == 15) L = {0, 4, 3, 2};
            if (n == 16) L = {0, 4, 3, 2};
            if (n == 17) L = {0, 5, 3, 2};
            if (n == 18) L = {0, 5, 4, 2};
            if (n == 19) L = {0, 6, 3, 2};
            if (n == 20) L = {0, 6, 4, 2};
            if (n == 21) L = {0, 6, 4, 2};
            if (n == 22) L = {0, 7, 4, 2};
            if (n == 23) L = {0, 7, 4, 2};
            if (n == 24) L = {0, 8, 4, 2};
            if (n == 25) L = {0, 8, 5, 2};
            if (n == 26) L = {0, 9, 5, 2};
            if (n == 27) L = {0, 9, 5, 3};
            if (n == 28) L = {0, 9, 5, 3};
            if (n == 29) L = {0, 10, 5, 3};
            if (n == 30) L = {0, 10, 6, 3};
            if (n <= 15) {
                F1(i, 5) {
                    p.push_back(3);
                    L.push_back(i);
                }
                F1(i, 3) {
                    p.push_back(2);
                    L.push_back(i);
                }
            } else if (n <= 22) {
                F1(i, 6) {
                    p.push_back(3);
                    L.push_back(i);
                }
                F1(i, 2) {
                    p.push_back(2);
                    L.push_back(i);
                }
            } else {
                F1(i, 7) {
                    p.push_back(3);
                    L.push_back(i);
                }
                F1(i, 1) {
                    p.push_back(2);
                    L.push_back(L[3]);
                }
            }
            BW = 170000 / m;
        }

        if (V == 13) {
            p = {0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 3, 3, 3};
            if (n == 15) L = {0, 4, 3, 2, 1};
            if (n == 16) L = {0, 4, 3, 2, 1};
            if (n == 17) L = {0, 5, 4, 2, 1};
            if (n == 18) L = {0, 5, 4, 2, 1};
            if (n == 19) L = {0, 5, 4, 3, 1};
            if (n == 20) L = {0, 6, 4, 3, 1};
            if (n == 21) L = {0, 6, 5, 3, 1};
            if (n == 22) L = {0, 7, 5, 3, 1};
            if (n == 23) L = {0, 7, 5, 3, 1};
            if (n == 24) L = {0, 7, 5, 3, 2};
            if (n == 25) L = {0, 7, 6, 3, 2};
            if (n == 26) L = {0, 8, 6, 3, 2};
            if (n == 27) L = {0, 8, 6, 3, 2};
            if (n == 28) L = {0, 8, 7, 3, 2};
            if (n == 29) L = {0, 9, 7, 3, 2};
            if (n == 30) L = {0, 9, 7, 4, 2};

            if (n < 26) {
                F1(i, 5) L.push_back(i);
                F1(i, 3) L.push_back(i);
            } else {
                p[V - 3] = 4;
                F1(i, 6) L.push_back(i);
                F1(i, 2) L.push_back(i);
            }
            BW = 70000 / m;
        }

        if (V == 14) {
            p = {0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 3, 3, 3, 3};
            if (n == 15) L = {0, 4, 3, 2, 2};
            if (n == 16) L = {0, 4, 3, 2, 2};
            if (n == 17) L = {0, 5, 4, 2, 2};
            if (n == 18) L = {0, 5, 4, 2, 2};
            if (n == 19) L = {0, 5, 4, 3, 2};
            if (n == 20) L = {0, 6, 4, 3, 2};
            if (n == 21) L = {0, 6, 5, 3, 2};
            if (n == 22) L = {0, 7, 5, 3, 2};
            if (n == 23) L = {0, 7, 5, 3, 2};
            if (n == 24) L = {0, 7, 5, 3, 2};
            if (n == 25) L = {0, 7, 6, 3, 2};
            if (n == 26) L = {0, 8, 6, 3, 2};
            if (n == 27) L = {0, 8, 6, 3, 2};
            if (n == 28) L = {0, 8, 7, 3, 2};
            if (n == 29) L = {0, 9, 7, 3, 2};
            if (n == 30) L = {0, 9, 7, 4, 2};

            if (n < 28) {
                F1(i, 5) L.push_back(i);
                F1(i, 4) L.push_back(i);
            } else {
                p[V - 4] = 4;
                F1(i, 6) L.push_back(i);
                F1(i, 3) L.push_back(i);
            }
            BW = 120000 / m;
        }

        if (V == 15) {
            BW = 60000 / m;

            if (n == 15) L = {0, 4, 3, 2};
            if (n == 16) L = {0, 4, 3, 2};
            if (n == 17) L = {0, 5, 4, 2};
            if (n == 18) L = {0, 5, 4, 2};
            if (n == 19) L = {0, 5, 4, 3};
            if (n == 20) L = {0, 6, 4, 3};
            if (n == 21) L = {0, 6, 5, 3};
            if (n == 22) L = {0, 7, 5, 3};
            if (n == 23) L = {0, 7, 5, 3};
            if (n == 24) L = {0, 7, 5, 3};
            if (n == 25) L = {0, 7, 6, 3};
            if (n == 26) L = {0, 8, 6, 3};
            if (n == 27) L = {0, 8, 6, 3};
            if (n == 28) L = {0, 8, 7, 3};
            if (n == 29) L = {0, 9, 7, 3};
            if (n == 30) L = {0, 9, 7, 4};
            if (n <= 26) {
                L.push_back(2);
                L.push_back(1);
                if (n <= 19) {
                    p = {0, 0, 1, 2, 3, 3, 4, 4, 4, 4, 3, 5, 5, 5, 5};
                    F1(i, 4) L.push_back(i);
                    L.push_back(1);
                    F1(i, 4) L.push_back(i);
                } else {
                    p = {0, 0, 1, 2, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5};
                    F1(i, 5) L.push_back(i);
                    F1(i, 4) L.push_back(i);
                }
            } else {
                L.push_back(3);
                p = {0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3};
                F1(i, 5) L.push_back(i);
                F1(i, 5) L.push_back(i);
                BW *= 1.5;
            }
        }

        Ready();
        assert(SZ(p) == V);
        assert(SZ(L) == V);
        //PR(p, L);
    }

    void ApplyVI(MoveType mv, int sgn) {
        F1(i, V - 1) {
            int dir_change = (mv & 3) - 1;
            mv >>= 2;
            if (dir_change) o[i] = (o[i] + sgn * dir_change) & 3;
        }
        CalcCoord();
    }

    int n_pick, n_deliver, leaf_pos;
    int mask, fixed_rot_mask;
    bitset<BITS> alive01, alive10;
    pii at;

    void Print(vector<MoveType> sol) {
        cout << V << endl;
        F1(i, V - 1) cout << p[i] << " " << L[i] << endl;

        vector<vector<int>> history;
        F1(i, SZ(sol) - 1) {
            MoveType lm = sol[i];
            vector<int> v;
            v.push_back(lm & 7);
            lm >>= 3;
            F1(i, V - 1) {
                v.push_back(lm & 3);
                lm >>= 2;
            }
            for (int i = leaf_pos; i < V; i++) {
                v.push_back(lm & 1);
                lm >>= 1;
            }
            history.push_back(v);
        }
        pii start_at = start_positions[sol[0]];
        cout << start_at.first << " " << start_at.second << endl;
        F1(hi, SZ(history) - 1) {
            vector<int>& s = history[hi];
            vector<int>& p = history[hi - 1];
            F1(i, V - 1) {
                if (s[i] == 3) {
                    if (p[i] != 2) {
                        s[i] = 2;
                        p[i]++;
                    } else {
                        s[i] = 1;
                        p[i] = 0;
                    }
                }
            }
        }

        for (auto s : history) {
            cout << HS[s[0]];
            F1(i, V - 1) cout << ROTS[s[i]];
            int index = V;
            F0(i, V) if (i >= leaf_pos) {
                int tp = s[index++];
                if (tp) cout << 'P'; else cout << '.';
            } else {
                cout << '.';
            }
            cout << endl;
        }
    }

    void ApplyFix(int i) {
        ApplyFix(fixed_rot_mask, i);
    }

    void Recalc(int i) {
        orient[i] = (orient[p[i]] + o[i] + (dirs[i] - 1)) & 3;
        vert[i] = MoveD(vert[p[i]], orient[i], L[i]);
    }

    void MakeMove(MoveType mv) {
        at = Move(at, mv & 7);
        mv >>= 3;
        ApplyVI(mv, 1);
        mv >>= (V - 1) * 2;

        fixed_rot_mask = 0;

        for (int i = lp; i < V; i++) {
            int tp = mv & 1;
            mv >>= 1;

            if (tp == 0) continue;

            pii p = Add(at, coord[i]);

            if (mask & (1 << i)) {
                alive01.set(id[p.first][p.second], 0);
                n_deliver--;
                ApplyFix(i);
                mask ^= (1 << i);
            } else {
                alive10.set(id[p.first][p.second], 0);
                n_pick--;
                ApplyFix(i);
                mask ^= (1 << i);
            }
        }
    }

    void RevertMove(MoveType mv) {
        int first_move = mv & 7;
        mv >>= 3;

        MoveType save_mv = mv;
        mv >>= 2 * (V - 1);

        for (int i = lp; i < V; i++) {
            int tp = mv & 1;
            mv >>= 1;

            if (tp == 0) continue;

            pii p = Add(at, coord[i]);

            if (!((1 << i) & mask)) {
                alive01.set(id[p.first][p.second], 1);
                n_deliver++;
                mask ^= (1 << i);
            } else {
                alive10.set(id[p.first][p.second], 1);
                n_pick++;
                mask ^= (1 << i);
            }
        }
        at = Move(at, first_move ^ 2);
        ApplyVI(save_mv, -1);
    }
};

void Prepare() {
    m = 0;
    F0(i, n) F0(j, n) {
        if (A[i][j] < B[i][j]) {
            p01.push_back(pii(i, j));
        } else if (A[i][j] > B[i][j]) {
            p10.push_back(pii(i, j));
        }
    }
    m = SZ(p01);
    PR(n, m, V);
    PR(sizeof(r01));
    PR(sizeof(r));
    PR(sizeof(rn));
    PR(sizeof(rp));

    F0(i, n) F0(j, n) zh[i][j] = my.zh();
    F0(i, 16) zh2[i] = my.zh();
    bscore = inf;
}

// Diff (Stateless) Beam Search
const int MSTEPS = 512;
typedef double EvalType;
typedef ll HashType;
// eval, new state
typedef pair<EvalType, TreeState> DeltaType;
set<DeltaType> deltas;
vector<TreeState> beam[MSTEPS];
#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;
gp_hash_table<HashType, EvalType> all_hashes;
int at_stack[MSTEPS], last_step;
HashType hash_stack[MSTEPS];
MoveType lm_stack[MSTEPS];
EvalType eval_stack[MSTEPS];
EvalType e[N][N];
int fix_mask_stack[MSTEPS];
int usedz[N][N];
vector<int> ks;
int uz;
State st, bst;
int step, bi;
vector<pii> gs;
double freq[N][N];
pii vert_dir[16][4];
double F[8];
int cands[16][4], candn[16];
int alive[16][16], aliven[16];
EvalType best_eval[16][16];
EvalType potential;
pii nat;
bool done;
int k;

void UpdateBest() {
    if (score < bscore) {
        bversion = version;
        bscore = score;
        bsol = sol;
        bst = st;
    }
}

void InitBeam() {
    all_hashes.clear();
    //all_hashes.reserve(1 << 21);

    for (int i = 0; i < MSTEPS; i++) beam[i].clear();
    last_step = 0;
    at_stack[0] = 0;
    eval_stack[0] = 0;
    fix_mask_stack[0] = (1 << V) - 1;

    F0(x, n) F0(y, n) start_positions.push_back(pii(x, y));

    hash_stack[0] = Hash(start_positions[0]);
    lm_stack[0] = 0;
    st.at = start_positions[0];

    F0(i, SZ(start_positions)) {
        beam[0].push_back(MakeState(i, i));
    }
}

void RevertMove(int step, const MoveType& lm) {
    st.RevertMove(lm);
}

void MakeMove(int step, const MoveType& lm) {
    lm_stack[step] = lm;
    hash_stack[step] = hash_stack[step - 1] ^ Hash(st.at);
    st.MakeMove(lm);
    hash_stack[step] ^= Hash(st.at);
    eval_stack[step] = eval_stack[step - 1];
    MoveType mv = lm >> (3 + 2 * (V - 1));
    for (int i = lp; i < V; i++) {
        int tp = mv & 1;
        mv >>= 1;
        if (tp) {
            pii p = Add(st.at, st.coord[i]);
            eval_stack[step] += e[p.first][p.second];
            hash_stack[step] ^= zh[p.first][p.second];
            hash_stack[step] ^= zh2[i];
       }
    }
    fix_mask_stack[step] = st.fixed_rot_mask;
}

void Reach(int step, int at) {
    while (last_step > step) {
        RevertMove(last_step, lm_stack[last_step]);
        last_step--;
    }
    if (step == last_step && at_stack[step] == at) return;
    if (step == 0) {
        st.at = start_positions[at];
        at_stack[0] = at;
        hash_stack[0] = Hash(start_positions[at]);
        lm_stack[0] = at;
        return;
    }
    Reach(step - 1, GetPrev(beam[step][at]));
    MakeMove(step, GetMove(beam[step][at]));
    at_stack[step] = at;
    last_step++;
}

void Next(int i, int mask) {
    if (i == lp) {
        gs.clear();
        for (int index = lp; index < V; index++) {
            candn[index] = 0;
            int n_rot = 4;
            if (st.fixed_rot_mask & (1 << index)) n_rot = 3;

            F0(ki, rn[mask][index][nat.first][nat.second]) {
                char no = r[mask][index][nat.first][nat.second][ki];
                dirs[index] = (no - orient[st.p[index]] - st.o[index] + 1) & 3;
                if (n_rot == 3 && dirs[index] == 3) {
                    continue;
                }

                pii p(rp[mask][index][nat.first][nat.second][ki] >> 5,
                      rp[mask][index][nat.first][nat.second][ki] & 31);
                if ((1 << index) & st.mask) {
                    if (A[p.first][p.second] < B[p.first][p.second] && st.alive01[id[p.first][p.second]]) {
                        cands[index][candn[index]++] = dirs[index];
                    }
                } else {
                    if (A[p.first][p.second] > B[p.first][p.second] && st.alive10[id[p.first][p.second]]) {
                        cands[index][candn[index]++] = dirs[index];
                    }
                }
            }
            dirs[index] = 0;
            if (candn[index]) gs.push_back(make_pair(candn[index], index));
        }

        if (gs.empty()) {
            return;
        }

        uz++;
        EvalType new_eval = eval_stack[step];
        HashType h = hash_stack[step] ^ Hash(st.at) ^ Hash(nat);
        MoveType lm = k;

        if (V > 12) sort(gs.begin(), gs.end());
        int cnt = 0;
        for (auto p : gs) {
            int index = p.second;
            F0(candi, candn[index]) {
                dirs[index] = cands[index][candi];
                st.Recalc(index);
                pii p = vert[index];

                if (usedz[p.first][p.second] != uz) {
                    usedz[p.first][p.second] = uz;
                    h ^= zh[p.first][p.second];
                    h ^= zh2[index];
                    lm |= (1LL << (3 + 2 * (V - 1) + (index - lp)));
                    new_eval += e[p.first][p.second];
                    cnt++;
                    break;
                }
            }
        }

        if (all_hashes.find(h) == all_hashes.end() || all_hashes[h] > step) {
            F1(i, V - 1) lm |= ((ll)dirs[i] << (3 + 2 * (i - 1)));
            all_hashes[h] = step;
            deltas.insert({-(new_eval), MakeState(bi, lm)});

            while (SZ(deltas) > BW) deltas.erase(--deltas.end());

            if (cnt == st.n_deliver && st.n_pick == 0) {
                //PR("over");
                done = true;
                return;
            }
        }
        return;
    }
    int n_rot = 4;
    if (st.fixed_rot_mask & (1 << i)) n_rot = 3;
    for (dirs[i] = 0; dirs[i] < n_rot; dirs[i]++) {
        st.Recalc(i);

        if (1) {
            pii q = vert[i];
            if (!Good(q)) continue;

            aliven[i] = 0;
            potential = eval_stack[step];;
            for (int index = 0; index < aliven[i - 1]; index++) {
                int j = alive[i - 1][index];
                if (!reaches[i][j]) {
                    best_eval[i][j] = best_eval[i - 1][j];
                    alive[i][aliven[i]++] = j;
                    potential += best_eval[i][j];
                    continue;
                }
                if ((1<<j)&st.mask) {
                    inters = r01[i][q.first+n][q.second+n][j] & st.alive01;
                    if (inters.any()) {
                        alive[i][aliven[i]++] = j;
                        int best_index = inters._Find_first();
                        best_eval[i][j] = e[p01[best_index].first][p01[best_index].second];
                        potential += best_eval[i][j];
                    }
                } else {
                    inters = r10[i][q.first+n][q.second+n][j] & st.alive10;
                    if (inters.any()) {
                        alive[i][aliven[i]++] = j;
                        int best_index = inters._Find_first();
                        best_eval[i][j] = e[p10[best_index].first][p10[best_index].second];
                        potential += best_eval[i][j];
                    }
                }
            }
            if (aliven[i] == 0) continue;
            if (SZ(deltas) >= BW && -(--deltas.end())->first > potential) {
                continue;
            }
        }
        Next(i + 1, mask | (orient[i] << (2 * (i - 1))));
        if (done) return;
    }
}

void BeamStep() {
    done = false;
    for (step = 0; ; step++) {
        all_hashes.clear();
        if (!done) {
            if (Elapsed() > TL) return;
            if (step >= bscore - 1 || step >= MSTEPS) return;
        }

        if (step > 0) {
            for (auto p : deltas) {
                beam[step].push_back(p.second);
            }
            sort(beam[step].begin(), beam[step].end(),
                 [&](const TreeState& t1, const TreeState& t2) {
                    return GetPrev(t1) > GetPrev(t2);
                 }
            );
        }
        deltas.clear();

        for (bi = 0; bi < SZ(beam[step]); bi++) {
            //if (bi == 0) PR(step, SZ(beam[step]), st.n_deliver, st.n_pick);
            if (((bi & 15) == 15) && Elapsed() > TL) return;

            Reach(step, bi);

            if (st.n_deliver == 0) {
                score = step;
                PR(score);
                sol = vector<MoveType>(lm_stack, lm_stack + step + 1);
                UpdateBest();
                return;
            }

            if (done) continue;

            st.fixed_rot_mask = fix_mask_stack[step];
            ks.clear();
            F0(kk, 5) {
                k = kk;
                if (eval_stack[step] == 0 && k != STAY) continue;
                if (In(Move(st.at, k))) {
                    ks.push_back(k);
                    nat = Move(st.at, k);
                    vert[0] = nat;
                    Next(1, 0);
                    if (done) break;
                }
            }
            if (done) break;
            // daamate carieli svlebi
            for (int k : ks) if (k != STAY) {
                pii nat = Move(st.at, k);
                EvalType new_eval = eval_stack[step];
                HashType h = hash_stack[step] ^ Hash(st.at) ^ Hash(nat);
                MoveType lm = k;
                if (all_hashes.find(h) == all_hashes.end() || all_hashes[h] > step) {
                    all_hashes[h] = step;
                    deltas.insert({-(new_eval - 0.001), MakeState(bi, lm)});

                    while (SZ(deltas) > BW) deltas.erase(--deltas.end());
                }
            }
        }
    }
}

void Bimsa() {
    InitBeam();
    BW *= bw_coeff;
    PR(BW);
    BeamStep();
}

void InitState() {
    st.n_pick = st.n_deliver = m;
    st.mask = 0;
    st.CreateTree(n);
    auto sp = st.p;
    auto sL = st.L;
    if (version == 1) {
        st.CreateTree(min(30, n + 1));
        if (sp == st.p && sL == st.L) st.L[1]++;
    } else if (version == 2) {
        st.CreateTree(max(15, n - 1));
        if (sp == st.p && sL == st.L) st.L[1]--;
    }
    st.fixed_rot_mask = (1 << V) - 1;
    F0(i, m) st.alive01.set(i, 1);
    F0(i, m) st.alive10.set(i, 1);
}

// ujrebis fasebis datvla da migcevadoba
void AssignWeights() {
    PR(lp, V - lp);
    assert(lp <= 6);
    assert(V - lp <= 10);
    vert[0] = pii(0, 0);
    F0(i, 8) F[i] = pow(i, PC);
    F0(i, n) F0(j, n) freq[i][j] = 0;
    F0(base_mask, 1 << (2 * (lp - 1))) {
        // motrialebebis shesruleba
        ll tmp = base_mask;
        F1(i, lp - 1) {
            orient[i] = tmp & 3;
            tmp >>= 2;
            vert[i] = MoveD(vert[st.p[i]], orient[i], st.L[i]);
        }
        for (int i = lp; i < V; i++) {
            F0(k, 4) vert_dir[i][k] = MoveD(vert[st.p[i]], k, st.L[i]);
        }
        for (int i = lp; i < V; i++) {
            F0(x, n) F0(y, n) rn[base_mask][i][x][y] = 0;
            F0(k, 4) {
                if (vert_dir[i][k].first + n < 0 || vert_dir[i][k].first >= n) continue;
                if (vert_dir[i][k].second + n < 0 || vert_dir[i][k].second >= n) continue;
                int x1 = max(0, -vert_dir[i][k].first);
                int x2 = min(n, n - vert_dir[i][k].first);
                int y1 = max(0, -vert_dir[i][k].second);
                int y2 = min(n, n - vert_dir[i][k].second);

                for (int x = x1; x < x2; x++) {
                    for (int y = y1; y < y2; y++) {
                        int& index = rn[base_mask][i][x][y];
                        int qx = vert_dir[i][k].first + x, qy = vert_dir[i][k].second + y;
                        if (A[qx][qy] != B[qx][qy]) {
                            rp[base_mask][i][x][y][index] = (qx << 5) | qy;
                            r[base_mask][i][x][y][index++] = k;
                        }
                    }
                }
            }
        }

        F0(x, n) F0(y, n) {
            double sm = 1;
            for (int i = lp; i < V; i++) sm += F[rn[base_mask][i][x][y]];
            for (int i = lp; i < V; i++) {
                F0(ki, rn[base_mask][i][x][y]) {
                    int qx = (rp[base_mask][i][x][y][ki] >> 5), qy = (rp[base_mask][i][x][y][ki] & 31);
                    freq[qx][qy] += sm - F[rn[base_mask][i][x][y]];
                }
            }
        }
    }
    double sm = 0;
    F0(i, n) F0(j, n) if (freq[i][j]) {
        e[i][j] = pow(1.0 / freq[i][j], 0.9);
        sm += e[i][j];
    }
    F0(i, n) F0(j, n) if (freq[i][j]) {
        e[i][j] = e[i][j] / sm * n * n;
    }

    sort(p01.begin(), p01.end(), [&](pii p1, pii p2){
        return e[p1.first][p1.second] > e[p2.first][p2.second];
    });
    sort(p10.begin(), p10.end(), [&](pii p1, pii p2){
        return e[p1.first][p1.second] > e[p2.first][p2.second];
    });
    F0(i, m) id[p01[i].first][p01[i].second] = i;
    F0(i, m) id[p10[i].first][p10[i].second] = i;

    F0(base_mask, 1 << (2 * (lp - 1))) {
        F0(x, n) F0(y, n) {
            for (int i = lp; i < V; i++) {
                char* arr = r[base_mask][i][x][y];
                short* arrp = rp[base_mask][i][x][y];
                int RN = rn[base_mask][i][x][y];

                for (int i1 = 0; i1 < RN; i1++) {
                    for (int i2 = i1 + 1; i2 < RN; i2++) {
                        if (e[arrp[i1] >> 5][arrp[i1] & 31] < e[arrp[i2] >> 5][arrp[i2] & 31]) {
                            swap(arr[i1], arr[i2]);
                            swap(arrp[i1], arrp[i2]);
                        }
                    }
                }
            }
        }
    }
}

void CalcReach() {
    F0(i, V) F0(j, V) reaches[i][j] = false;
    for (int i = lp; i < V; i++) {
        int j = i;
        while (j != 0) {
            j = st.p[j];
            reaches[j][i] = true;
        }
    }
    F0(i, lp) F0(x, 3*n) F0(y, 3*n) for (int j = lp; j < V; j++) {
        r01[i][x][y][j].reset();
        r10[i][x][y][j].reset();
    }
    for (int i = lp - 1; i >= 1; i--) for (int index = lp; index < V; index++) {
        if (!reaches[i][index]) {
            continue;
        }
        if (st.p[index] == i) {
            for (int x = -n; x < n + n; x++) for (int y = -n; y < n + n; y++) F0(k, 4) {
                pii q = MoveD(pii(x, y), k, st.L[index]);
                if (!In(q) || A[q.first][q.second] == B[q.first][q.second]) continue;
                if (A[q.first][q.second] > B[q.first][q.second]) {
                    r10[i][x+n][y+n][index].set(id[q.first][q.second], 1);
                } else {
                    r01[i][x+n][y+n][index].set(id[q.first][q.second], 1);
                }
            }
        }
        int j = st.p[i];
        for (int x = -n; x < n + n; x++) for (int y = -n; y < n + n; y++) F0(k, 4) {
            pii q = MoveD(pii(x, y), k, st.L[i]);
            if (!Good(q)) continue;
            r10[j][q.first+n][q.second+n][index] |= r10[i][x+n][y+n][index];
            r01[j][q.first+n][q.second+n][index] |= r01[i][x+n][y+n][index];
        }
    }
    aliven[0] = 0;
    for (int i = lp; i < V; i++) {
        alive[0][aliven[0]++] = i;
    }
}

void Solve() {
    Prepare();

    versions = 3;
    vector<double> bs_times;
    vector<double> prep_times;

    bw_coeff = 1.0 / versions;
    for (version = 0; version < versions; version++) {
        if (Elapsed() > TL - 0.25) break;

        double tmp1 = Elapsed();
        InitState();
        AssignWeights();
        CalcReach();
        double tmp2 = Elapsed();
        prep_times.push_back(tmp2 - tmp1);
        Bimsa();
        double tmp3 = Elapsed();
        bs_times.push_back((tmp3 - tmp2) / bw_coeff);
    }
    //PR(prep_times);
    //PR(bs_times);

    version = bversion;
    if (Elapsed() < TL - prep_times[version]) {
        PC = 0.59;
        InitState();
        AssignWeights();
        CalcReach();

        double time_left = TL - Elapsed();
        if (time_left > bs_times[version] / 2) {
            bw_coeff = time_left / bs_times[version] * (versions == 3 ? 0.8 : 0.4);
            PR(bw_coeff);
            Bimsa();
        }
    }

    //if (Elapsed() > TL + 0.5) return;
    bst.Print(bsol);

    PR(bscore);
    //Report();
}


void ReadInput() {
    cin >> n >> m >> V;
    F0(i, n) cin >> A[i];
    F0(i, n) cin >> B[i];
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
        if(seed>=0 && seed<10000) {
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
