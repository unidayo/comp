int LAST_AVG = 1;
int pl2_t = 2;

#ifdef ONLINE_JUDGE
double TL = 1.97;
#else
double TL = 1.9 * 0.9;
#endif
int PL2_TEST_SAMPLES = 1;
int PL2_VALID_SAMPLES = 1;
double PL2_TEST_SUM_RATIO = 1.0;

int slow_start = 0;
int horizon = 12;
const int default_seed = 2029;
#include <bits/stdc++.h>
int STANDARD = 1;
using namespace std;
#define F0(i,n) for (int i=0; i<n; i++)
#define F1(i,n) for (int i=1; i<=n; i++)
#define CL(a,x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())
const int inf = 1000000000;
typedef pair<int, int> pii;
typedef long long ll;
const double EPS = 1e-9;
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
#ifdef LOCAL
#include "debug.h"
#define PR(...) cerr << #__VA_ARGS__ << " =", print_all(__VA_ARGS__)
#else
#pragma GCC optimize("O3")
#define PR(...)
#endif

chrono::system_clock::time_point init_time = chrono::system_clock::now();
inline double GetSeconds() {
     chrono::system_clock::time_point current_time = chrono::system_clock::now();
     double ret = chrono::duration_cast<std::chrono::nanoseconds>(current_time - init_time).count();
     return ret * 1e-9;
}

const int MAX_RAND = 1 << 30;
struct Rand {
    ll x, y, z, w, o;
    Rand() { }
    Rand(ll seed) { reseed(seed); o = 0; }
    inline void reseed(ll seed) { x = 0x498b3bc5 ^ seed; y = 0; z = 0; w = 0;  F0(i, 20) mix(); }
    inline void mix() { ll t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8); }
    inline ll rand() { mix(); return x & (MAX_RAND - 1); }
    inline ll zh() { mix(); return x; }
    inline int nextInt(int n) { return rand() % n; }
    inline int nextInt(int L, int R) { return rand()%(R - L + 1) + L; }
    inline int nextBool() { if (o < 4) o = rand(); o >>= 2; return o & 1; }
    inline double nextDouble() { return rand() * 1.0 / MAX_RAND; }
    inline double nextDouble(double L, double R) { return L + (R - L) * nextDouble(); }
    template<class T> auto REL(T& v) { return v[nextInt(SZ(v))]; }
    template<class T> auto REL(T& v, double cutoff) { return v[(int)(SZ(v)*cutoff)]; }
    template<class T> void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = nextInt(0, i); swap(v[i], v[j]); } }
    int ws_n; vector<double> ws_prob; vector<int> ws_alias;
    inline void PrepareWeightedSample(const vector<double>& p) {
        ws_n = SZ(p); ws_prob.assign(ws_n, 0); ws_alias.assign(ws_n, 0);
        vector<double> sc(ws_n);
        vector<int> small, large;
        double sum = 0; F0(i, ws_n) sum += p[i];
        F0(i, ws_n) {
            sc[i] = p[i]/sum * ws_n; if (sc[i] < 1) small.push_back(i); else large.push_back(i);
        }
        while(!small.empty() && !large.empty()){
            int l = small.back(); small.pop_back();
            int g = large.back(); large.pop_back();
            ws_prob[l] = sc[l]; ws_alias[l] = g;
            sc[g] = sc[g] - (1 - sc[l]);
            if(sc[g] < 1) small.push_back(g); else large.push_back(g);
        }
        for(int g : large){ ws_prob[g]=1; ws_alias[g]=g; }
        for(int l : small){ ws_prob[l]=1; ws_alias[l]=l; }
    }
    inline int WeightedSample() { int i = nextInt(ws_n); return nextDouble() < ws_prob[i] ? i : ws_alias[i]; }
};
Rand my(default_seed);
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

#ifdef LOCAL
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
#else
#define AT(i)
#endif

// CONSTANTS
const int N = 10;
const int N2 = 100;
const int MAXM = 8;
const int MAXT = 100;
int n, m, T, U, t, rt, bmove;
int seed;
int bscore, score;
double score_ratio;
int V[N2], L[N2], O[N2];
double V2[N2];
int st[MAXM], last[MAXM];
double s[MAXM];
using B = unsigned __int128;
B all[MAXM];
int chosen_sid[MAXM][MAXT];
double chosen_threshold[MAXM][MAXT];
double chosen_ratio[MAXM][MAXT];
double scores[N2 + N2];
vector<int> cand_moves;
int cand_1st[N2 + N2];
int last2, last_active = -1;
double V2s[N2 + N2][N2];
vector<double> weights;
B all2[MAXM];
int threatened[N2];
int candn;
const B one = (B)1;

inline B Bit(int u) { return one << u; }

inline int PopCount(B x) {
    unsigned long long lo = (unsigned long long)x;
    unsigned long long hi = (unsigned long long)(x >> 64);
    return __builtin_popcountll(lo) + __builtin_popcountll(hi);
}

inline int FirstBit(B x) {
    unsigned long long lo = (unsigned long long)x;
    if (lo) return __builtin_ctzll(lo);
    unsigned long long hi = (unsigned long long)(x >> 64);
    if (hi) return 64 + __builtin_ctzll(hi);
    return -1;
}

inline int PopBit(B& x) {
    int u = FirstBit(x);
    x &= x - 1;
    return u;
}

inline int GetByIndex(B x, int idx) {
    int last = -1;
    while (x) {
        int u = PopBit(x);
        last = u;
        if (idx <= 0) return u;
        idx--;
    }
    return last;
}

const int LOGN = 1 << 16;
double logs[LOGN];
template<class T> T sqr(T x) { return x*x; }

enum { RIGHT, DOWN, LEFT, UP };
const int DX[]={0,1,0,-1};
const int DY[]={1,0,-1,0};

int d1(int x, int y) { return abs(x-y); }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
int qx[N*N], qy[N*N], id[N][N], nx[N*N][4];
bool Good(pii p) { return In(p); }
vector<int> adj[N2];
int aa[N2][4], an[N2];
int hO[MAXT][N2], hL[MAXT][N2], hlast[MAXT][MAXM], hst[MAXT][MAXM];
// Possible moves turn/player
int hN[MAXT][MAXM], hMax[MAXT][MAXM][4], hCnt[MAXT][MAXM][4];
vector<int> heqs[MAXT][MAXM][4];
pii hC[MAXT][MAXM][N2];
pii hLastC[MAXT][MAXM];
int sure_random[MAXM];
vector<int> ts[MAXM];
int vz, mvis[N2];
vector<int> all_targets;
unsigned char target_cnt[N2];
unsigned short target_mask[N2];
double W[N2];
int touch;

// a - occupy
// b - reinforce
// c - steal
// d - weaken
double wa = 0.5400;
double wb = 0.3900;
double wc = 1.100;
double wd = 1.110;

const int MAX_SAMPLEN = 16;
int SAMPLEN = MAX_SAMPLEN, RESAMPLE;
double a[MAX_SAMPLEN][MAXM][5], w[MAX_SAMPLEN][MAXM];
int f[MAX_SAMPLEN][MAXM][MAXT];
int cur_random[MAX_SAMPLEN][MAXM];
int vis0[N2], viz0;
double surv[N2], attack[N2];
double cat[4];
double rprob[MAXM];
vector<pair<int, double>> active_samples[MAXM];

const int SIMN = 0;
int current_sid = 0;
double f_w[MAXM][5];
double f_r1[SIMN + 1][MAXT][MAXM], f_r2[SIMN + 1][MAXT][MAXM];
Rand samplers[MAXM];
B connected_mask, can_mask;
vector<int> Q;
B bl;
extern B ac[MAXM], pc[MAXM], mc[MAXM];
B own_cells[MAXM];
void MakeBL() {
    bl = 0;
    F0(p, m) bl ^= Bit(st[p]);
}

inline B RealAllMask(int p, B mask) {
    return (mask & ~bl) | Bit(st[p]);
}

inline B RealAllMask(int p) {
    return RealAllMask(p, all[p]);
}

int NextPos() { int x, y; cin >> x >> y; return id[x][y]; }

void PrepareGrid() {
    int cn = 0;
    F0(i, n) F0(j, n) {
        qx[cn] = i;
        qy[cn] = j;
        id[i][j] = cn++;
    }
    F0(i, cn) { adj[i].clear(); an[i] = 0; }
    F0(i, cn) {
        pii p(qx[i], qy[i]);
        F0(k, 4) {
            nx[i][k] = -1;
            pii q = Move(p, k);
            if (Good(q)) {
                nx[i][k] = id[q.first][q.second];
                adj[i].push_back(nx[i][k]);
                aa[i][an[i]++] = nx[i][k];
            }
        }
    }
}

double wmax, wmid;
void Prepare() {
    touch = 0;
    PR(m, U);
    F0(i, N2) { O[i] = -1; L[i] = 0; }
    F0(p, m) { O[st[p]] = p; L[st[p]] = 1; s[p] = V[st[p]]; }

    cand_moves.reserve(N2);
    all_targets.reserve(N2);
    Q.reserve(N2);
    F0(tt, T) F0(pp, MAXM) F0(c, 4) heqs[tt][pp][c].reserve(N2);

    double mn = 1e9;
    F0(u, N2) mn = min(mn, 1.0*V[u]);
    F0(u, N2) W[u] = V[u] / mn;
    wmax = 0;
    F0(u, N2) wmax = max(wmax, W[u]);

    wmid = 1.0 + (wmax - 1.0) * 0.2;

    F1(p, m-1) samplers[p].reseed(p * 1000);

    bscore = inf;
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
}

pii Classify(int p, int u) {
    if (O[u] == -1) return {0, V[u]};
    else if (O[u] == p) return {1, (L[u] == U) ? 0 : V[u] };
    else if (L[u] == 1) return {2, V[u]};
    return {3, V[u]};
}

void FindSingle(int p) {
    AT(3);
    all[p] = 0;
    ac[p] = 0;
    Q.clear();
    vz++;
    mvis[st[p]] = vz;
    Q.push_back(st[p]);
    all[p] |= Bit(st[p]);
    ac[p] |= Bit(st[p]);
    F0(qi, SZ(Q)) {
        int u = Q[qi];
        F0(k, an[u]) {
            int v = aa[u][k];
            if (mvis[v] == vz) continue;
            mvis[v] = vz;
            all[p] |= Bit(v);
            if (O[v] == p) {
                ac[p] |= Bit(v);
                Q.push_back(v);
            }
        }
    }
}

void FindC() {
    AT(4);
    F1(p, m-1) {
        B real = RealAllMask(p);
        int hn = PopCount(real);
        hN[t][p] = hn;
        c_o[7] += hn;
        F0(C, 4) {
            hMax[t][p][C] = -1;
            hCnt[t][p][C] = 0;
            heqs[t][p][C].clear();
        }
        for (B b = real; b; ) {
            int u = PopBit(b);
            int C, val;
            if (O[u] == -1) {
                C = 0;
                val = V[u];
            } else if (O[u] == p) {
                C = 1;
                val = (L[u] == U) ? 0 : V[u];
            } else if (L[u] == 1) {
                C = 2;
                val = V[u];
            } else {
                C = 3;
                val = V[u];
            }

            if (val > hMax[t][p][C]) {
                hMax[t][p][C] = val;
                hCnt[t][p][C] = 0;
                heqs[t][p][C].clear();
            }
            if (val == hMax[t][p][C]) {
                hCnt[t][p][C]++;
                heqs[t][p][C].push_back(u);
            }
        }
    }
}

void FindAllMoves() {
    F0(p, m) {
        FindSingle(p);
    }
    FindC();
    F0(p, m) own_cells[p] = 0;
    F0(u, N2) if (O[u] != -1) own_cells[O[u]] |= Bit(u);
    F0(p, m) {
        pc[p] = own_cells[p] & ~ac[p];
        mc[p] = all[p] & ~ac[p];
    }
}

double F_EvalMove(int p, int u) {
    pii x = Classify(p, u);
    return f_w[p][x.first] * x.second;
}

void Basics() {
    AT(1);
    connected_mask = 0;
    can_mask = 0;
    F0(p, m) connected_mask |= ac[p];
    F1(p, m - 1) {
        B own_p = ac[p] | pc[p];
        can_mask |= RealAllMask(p) & ~own_p;
    }
}

// TODO: finish M2 somehow
//
void M2() {
    AT(8);

    B my_mask = RealAllMask(0);
    const int hm_steal = hMax[t][1][2];
    const int hm_reinf = hMax[t][1][1];
    const int ac1_pop = PopCount(ac[1]);

    double bv = -1.0;
    for (B b = my_mask; b; ) {
        int u = PopBit(b);
        const int ou = O[u];
        const int lu = L[u];
        const int vu = V[u];

        int C, val;
        if (ou == -1) { C = 0; val = vu; }
        else if (ou == 0) { C = 1; val = (lu == U) ? 0 : vu; }
        else if (lu == 1) { C = 2; val = vu; }
        else { C = 3; val = vu; }

        if (!val) {
            if (bv < 0) { bv = 0; bmove = u; }
            continue;
        }

        int sz = ((can_mask & Bit(u)) != 0);

        double c = 0.0;
        if (C == 0) {
            c = wa;
            int is_adj = 0;
            F0(k, an[u]) if (O[aa[u][k]] == 1) { is_adj = 1; break; }
            if (is_adj) {
                if (sz) {
                    c *= 2.4;
                } else {
                    c *= 1.3;
                }
            } else c *= 0.5;
        }
        else if (C == 1) {
            c = wb;
            if (lu == 1) {
                c *= 1.02;
                if (sz && hm_steal == vu) {
                    if (ac1_pop == 1) c *= 10.0;
                    else c *= 5.0;
                }
            } else {
                c *= 0.5;
            }
        }
        else if (C == 2) {
            c = wc;
            if (!(connected_mask & Bit(u))) {
                c *= 1.4;
            } else {
                c *= 2.5;
                // TODO: bug?
                if (hm_reinf == vu) c *= 0.7;
            }
        }
        else {
            c = wd;
            if (!(connected_mask & Bit(u))) {
                if (lu == 2) c *= 1.25;
                else if (lu == 3) c *= 1.15;
                else c *= 1.1;
            } else {
                // TODO: bug?
                if (vu < hm_reinf) c *= 2.5;
            }
        }

        double av = vu * c;

        if (av > bv) {
            bv = av;
            bmove = u;
        }
    }
}

double Spectrum(double E1, double E2, int I1, int I2, int I) {
    return E1 + (E2 - E1) / (I2 - I1) * (I - I1);
}

double Spectrum2(double E1, double E2, int I1, int I2, int I) {
    return E1 * pow(E2 / E1, 1.0 * (I - I1) / (I2 - I1));
}

double CellValue(int u, int t) {
    double r = t / 99.0;
    return wmid + (1.0 - pow(1 - r, 2.0)) * (W[u] - wmid);
}

void GreedyAllC(int best_by_c[4]) {
    double bv = -1.0;
    double bv_c[4];
    F0(C, 4) {
        best_by_c[C] = -1;
        bv_c[C] = -1.0;
    }
    B my_mask = RealAllMask(0);
    int my_cells[N2];
    int my_n = 0;
    viz0++;
    for (B b = my_mask; b; ) {
        int u = PopBit(b);
        my_cells[my_n++] = u;
        surv[u] = 1.0;
        vis0[u] = viz0;
    }

  { AT(10);
    F1(p, m-1) {
        B opp_mask = RealAllMask(p);
        B overlap_mask = opp_mask & my_mask;
        if (!overlap_mask) continue;
        int overlap_cells[N2];
        int overlap_n = 0;
        for (B b = overlap_mask; b; ) {
            int u = PopBit(b);
            overlap_cells[overlap_n++] = u;
            attack[u] = 0.0;
        }
        F0(C, 4) cat[C] = 0.0;
        const int hm0 = hMax[t][p][0];
        const int hm1 = hMax[t][p][1];
        const int hm2 = hMax[t][p][2];
        const int hm3 = hMax[t][p][3];

        for (const auto& smpl : active_samples[p]) {
            int si = smpl.first;
            double r = smpl.second;
            const double* ap = a[si][p];

            int bestC = -1;
            double bv = -1;
            double av = hm0 * ap[0]; if (av > bv) { bv = av; bestC = 0; }
            av = hm1 * ap[1]; if (av > bv) { bv = av; bestC = 1; }
            av = hm2 * ap[2]; if (av > bv) { bv = av; bestC = 2; }
            av = hm3 * ap[3]; if (av > bv) { bv = av; bestC = 3; }
            cat[bestC] += r * (1 - ap[4]);
        }

        F0(C, 4) {
            if (cat[C] == 0.0) continue;
            vector<int>& eqs = heqs[t][p][C];
            if (eqs.empty()) continue;
            double add = cat[C] / SZ(eqs);
            for (int u : eqs) {
                if (vis0[u] == viz0) attack[u] += add;
            }
        }

        double add_rand = rprob[p] / hN[t][p];
        F0(i, overlap_n) {
            int u = overlap_cells[i];
            double r = attack[u] + add_rand;
            surv[u] *= (1 - r);
        }
    }
  }
    AT(9);

    const double deny = 1.0 / (m - 1);
    const bool is_u1 = (U == 1);
    F0(i, my_n) {
        int u = my_cells[i];
        pii x = Classify(0, u);
        int C = x.first;

        double av = 0.0;
        double lose = 1 - surv[u];
        if (C == 0) {
            av += surv[u];
            av += lose * deny;
        } else if (C == 1) {
            // TODO: more cases
            av += lose;
            if (L[u] == 1) av += lose * deny;
            if (L[u] < U) av += 1.0;
        } else if (C == 2) {
            av += surv[u];
            av += surv[u] * deny; // their loss
            if (is_u1) av *= 1.2;
            // TODO deny second attacker steal (not defender)
        } else {
            av += surv[u] * deny; // their loss
        }

        if (is_u1) {
            av *= CellValue(u, t);
        }
        else {
            double vu = V2[u];
            if (vu > V[u] * 1.5 && O[u] == 0) vu = V[u];
            av = av * vu;
        }

        if (av > bv) {
            bv = av;
            bmove = u;
        }
        if (av > bv_c[C]) {
            bv_c[C] = av;
            best_by_c[C] = u;
        }
    }
}

void GreedyMove() {
    int best_by_c[4];
    GreedyAllC(best_by_c);
}


void QuickMove() {
    if (m == 2) {
        connected_mask = ac[0] | ac[1];
        B own1 = ac[1] | pc[1];
        can_mask = RealAllMask(1) & ~own1;
        M2();
        return;
    }
    Basics();
    M2();
}

// active cells
// passive cells
// move cells
B ac[MAXM], pc[MAXM], mc[MAXM];

inline void SyncPlayerSets(int p) {
    all[p] = ac[p] | mc[p];
}

void Add(int p, int u) {
    // Merge new owned cell into active component and absorb reachable passive components.
    O[u] = p;
    L[u] = 1;

    B bu = one << u;
    own_cells[p] |= bu;
    ac[p] |= bu;
    pc[p] &= ~bu;
    mc[p] &= ~bu;
    int qq[N2];
    int qh = 0, qt = 0;
    qq[qt++] = u;
    while (qh < qt) {
        int x = qq[qh++];
        F0(k, an[x]) {
            int v = aa[x][k];
            if (O[v] == p) {
                B bv = one << v;
                if (!(ac[p] & bv)) {
                    ac[p] |= bv;
                    pc[p] &= ~bv;
                    mc[p] &= ~bv;
                    qq[qt++] = v;
                }
            } else {
                mc[p] |= one << v;
            }
        }
    }
    SyncPlayerSets(p);
}

void Del(int p, int u, int root) {
    // Remove owned cell. If removed cell is a leaf in active component, update locally.
    // Otherwise rebuild active/passive/move sets from root.
    B bu = one << u;
    int in_ac = (ac[p] & bu) ? 1 : 0;
    int deg_ac = 0;
    if (in_ac) {
        F0(k, an[u]) {
            int v = aa[u][k];
            if (ac[p] & (one << v)) deg_ac++;
        }
    }

    own_cells[p] &= ~bu;
    if (O[u] == p) {
        O[u] = -1;
        L[u] = 0;
    }

    if (!in_ac) {
        pc[p] &= ~bu;
        int touch_u = 0;
        F0(k, an[u]) {
            int v = aa[u][k];
            if (ac[p] & (one << v)) { touch_u = 1; break; }
        }
        if (touch_u) mc[p] |= bu;
        else mc[p] &= ~bu;
        SyncPlayerSets(p);
        return;
    }

    if (in_ac && u != root && deg_ac <= 1) {
        ac[p] &= ~bu;
        pc[p] &= ~bu;

        // Removed cell itself may become frontier (stealable back) if still adjacent to active.
        int touch_u = 0;
        F0(k, an[u]) {
            int v = aa[u][k];
            if (ac[p] & (one << v)) { touch_u = 1; break; }
        }
        if (touch_u) mc[p] |= bu;
        else mc[p] &= ~bu;

        // Only neighbors of removed cell can change frontier status.
        F0(k, an[u]) {
            int v = aa[u][k];
            if (O[v] == p) continue;
            B bv = one << v;
            int touch = 0;
            F0(k2, an[v]) {
                int w = aa[v][k2];
                if (ac[p] & (one << w)) { touch = 1; break; }
            }
            if (touch) mc[p] |= bv;
            else mc[p] &= ~bv;
        }
        SyncPlayerSets(p);
        return;
    }

    ac[p] = one << root;
    pc[p] = 0;
    mc[p] = 0;

    B vis = one << root;
    int qq[N2];
    int qh = 0, qt = 0;
    qq[qt++] = root;
    while (qh < qt) {
        int x = qq[qh++];
        F0(k, an[x]) {
            int v = aa[x][k];
            if (O[v] == p) {
                B bv = one << v;
                if (!(vis & bv)) {
                    vis |= bv;
                    ac[p] |= bv;
                    qq[qt++] = v;
                }
            } else {
                mc[p] |= one << v;
            }
        }
    }
    pc[p] = own_cells[p] & ~ac[p];
    SyncPlayerSets(p);
}

void Local() {
    AT(2);
    vz++;
    all_targets.clear();
    F0(p, m) {
        int targ = last[p];
        if (mvis[targ] != vz) {
            mvis[targ] = vz;
            all_targets.push_back(targ);
            target_cnt[targ] = 0;
            target_mask[targ] = 0;
        }
        target_cnt[targ]++;
        target_mask[targ] |= (1u << p);
    }
    for (int cell : all_targets) {
        int cnt = target_cnt[cell];
        unsigned short mask = target_mask[cell];
        if (O[cell] == -1) {
            if (cnt == 1) {
                int p = __builtin_ctz((unsigned int)mask);
                st[p] = cell;
                O[cell] = p;
                L[cell] = 1;
            }
        } else {
            int have_owner = (mask >> O[cell]) & 1;
            if (have_owner) {
                L[cell] = min(L[cell] + 1, U);
                st[O[cell]] = cell;
            }
            else if (cnt == 1) {
                int p = __builtin_ctz((unsigned int)mask);
                L[cell]--;
                if (L[cell] == 0) {
                    st[p] = cell;
                    O[cell] = p;
                    L[cell] = 1;
                }
            }
        }
    }
    MakeBL();
}

void LocalExtra(int final) {
    vz++;
    all_targets.clear();
    F0(p, m) {
        int targ = last[p];
        if (mvis[targ] != vz) {
            mvis[targ] = vz;
            all_targets.push_back(targ);
            target_cnt[targ] = 0;
            target_mask[targ] = 0;
        }
        target_cnt[targ]++;
        target_mask[targ] |= (1u << p);
    }

    int nst[MAXM];
    F0(p, m) nst[p] = st[p];
    int cn = 0;
    int ctype[MAXM + 1], ccell[MAXM + 1], cp1[MAXM + 1], cp2[MAXM + 1];
    // ctype: 0 occupy, 1 reinforce, 2 weaken, 3 steal
    for (int cell : all_targets) {
        int cnt = target_cnt[cell];
        unsigned short mask = target_mask[cell];
        int owner = O[cell];
        if (owner == -1) {
            if (cnt == 1) {
                int p = __builtin_ctz((unsigned int)mask);
                nst[p] = cell;
                ctype[cn] = 0;
                ccell[cn] = cell;
                cp1[cn] = p;
                cp2[cn] = -1;
                cn++;
            }
        } else {
            int have_owner = (mask >> owner) & 1;
            if (have_owner) {
                nst[owner] = cell;
                ctype[cn] = 1;
                ccell[cn] = cell;
                cp1[cn] = owner;
                cp2[cn] = -1;
                cn++;
            } else if (cnt == 1) {
                int attacker = __builtin_ctz((unsigned int)mask);
                if (L[cell] == 1) {
                    nst[attacker] = cell;
                    ctype[cn] = 3;
                    ccell[cn] = cell;
                    cp1[cn] = owner;
                    cp2[cn] = attacker;
                    cn++;
                } else {
                    ctype[cn] = 2;
                    ccell[cn] = cell;
                    cp1[cn] = owner;
                    cp2[cn] = attacker;
                    cn++;
                }
            }
        }
    }

    // Pass 1: apply L-only effects (reinforce/weaken).
    F0(i, cn) {
        int tp = ctype[i];
        int cell = ccell[i];
        if (tp == 1) {
            L[cell] = min(L[cell] + 1, U);
        } else if (tp == 2) {
            L[cell]--;
        }
    }

    int changed_mask = 0;

    // Pass 2: apply all gains (occupy / steal gain side).
    F0(i, cn) {
        int tp = ctype[i];
        int cell = ccell[i];
        if (tp == 0) {
            int p = cp1[i];
            Add(p, cell);
            changed_mask |= 1 << p;
        } else if (tp == 3) {
            int p2 = cp2[i];
            Add(p2, cell);
            changed_mask |= 1 << p2;
        }
    }

    // Pass 3: apply all losses (steal loss side), rooted at final position.
    F0(i, cn) if (ctype[i] == 3) {
        int owner = cp1[i];
        int cell = ccell[i];
        Del(owner, cell, nst[owner]);
        changed_mask |= 1 << owner;
    }

    F0(p, m) st[p] = nst[p];

    MakeBL();
    if (!final) {
        c_o[1] += m;
        c_o[3] += __builtin_popcount(changed_mask);
        FindC();
    }
}

double ScoreRaw() {
    F0(p, m) s[p] = 0;
    F0(i, N2) if (O[i] != -1) s[O[i]] += V[i] * L[i];

    double mx = 0;
    F1(p, m-1) mx = max(mx, s[p]);
    score_ratio = 1.0 * s[0] / mx;
    return log2(1.0 + score_ratio);
}

double SoftMax(double tau) {
    double mx = *max_element(s + 1, s + m) / tau / s[0];
    double sum = 0.0;
    F1(p, m-1) sum += exp(s[p] / s[0] / tau - mx);
    return tau * (mx + log(sum) - log(m - 1.0));
}


int apr_score = 100;
//  - (m - 5.5) * 0.05
double Score(int t) {
    if (t >= apr_score) return ScoreRaw();

    F0(p, m) s[p] = 0;

    if (U == 1) {
        double pw = Spectrum(-1.001, 1.0, 0, 100, t);
        F0(u, N2) if (O[u] != -1) s[O[u]] += pow(V[u], pw);
    } else {
        F0(i, N2) if (O[i] != -1) s[O[i]] += V[i] * L[i];
    }

    double tau = Spectrum2(1.0, 0.05, 0, apr_score, t);
    double smax = SoftMax(tau);
    double rat = 1.0 / smax;
    return log2(1.0 + rat);
}

void GenLastSample(int t) {
    last[0] = bmove;
    F1(p, m-1) {
        B real = RealAllMask(p);
        int si = chosen_sid[p][t];
        double eps = a[si][p][4];
        double ratio = chosen_ratio[p][t];
        if (chosen_threshold[p][t] < eps) {
            int sz = PopCount(real);
            int idx = (int)(sz * ratio);
            if (idx >= sz) idx = sz - 1;
            last[p] = GetByIndex(real, idx);
        } else {
            int bestC = -1;
            double bv = -1;
            F0(C, 4) {
                double av = hMax[t][p][C] * a[si][p][C];
                if (av > bv) { bv = av; bestC = C; }
            }
            vector<int>& eqs = heqs[t][p][bestC];
            int idx = (int)(SZ(eqs) * ratio);
            if (idx >= SZ(eqs)) idx = SZ(eqs) - 1;
            last[p] = eqs[idx];
        }
    }
}

int start_sz;
vector<double> ds[N2 + N2];
void Halve(int new_sz) {
    if (candn <= new_sz) return;

    nth_element(cand_moves.begin(), cand_moves.begin() + new_sz, cand_moves.end(), [&](int i1, int i2) {
        return scores[i1] > scores[i2];
    });
    cand_moves.resize(new_sz);
    candn = SZ(cand_moves);
}

double ft_1 = 0.11;
double ft_2 = 0.6;
void HalvingStep(double pr) {
    // 0.1 -> start_sz
    // 0.7 -> final_sz=2
    double sz = 2;
    if (pr < ft_1) sz = start_sz;
    else if (pr < ft_2) sz = start_sz * pow(2.0 / start_sz, (pr - ft_1) / (ft_2 - ft_1));

    Halve(sz + 0.999);
}

vector<int> takeovers;

void AddCand() {
    cand_moves.push_back(last2);
    int x = takeovers.back();
    takeovers.pop_back();
    F0(i, N2) V2s[last2][i] = V[i];
    V2s[last2][x] *= 100.0;
    last2++;
}

void SlowMove() {
    double start_time = Elapsed();
    double time_left = TL - start_time;
    double until = start_time + time_left / (T - rt);

    int horizon_l = horizon + 6 - rt / 10 + 1;
    if (m == 2) {
        horizon_l = horizon + 6 - rt / 10 + 2;
        //PL2_TEST_SAMPLES = 2;
        //PL2_VALID_SAMPLES = 0;
        //ft_1 = 0.10;
        //ft_2 = 0.55;
    } else {
    }
    //PR(rt, horizon_l);

    int horizon_turn = min(T, rt + horizon_l);
    F1(p, m-1) {
        weights.clear();
        for (auto smpl : active_samples[p]) weights.push_back(smpl.second);
        samplers[p].PrepareWeightedSample(weights);
    }

    F0(p, m) all2[p] = all[p];
    cand_moves.clear();
    double bv = -1.0;
    int bu = -1;

    F0(i, N2) threatened[i] = 0;
    F1(p, m-1) for (B b = RealAllMask(p); b; ) {
        int u = PopBit(b);
        threatened[u]++;
    }

    pii best_reinf(-1,-1);
    if (U==1) best_reinf.second=0;

    takeovers.clear();

    for (B b = RealAllMask(0, all2[0]); b; ) {
        int u = PopBit(b);
        pii x = Classify(0, u);
        if (x.first == 1 && !threatened[u]) {
            if (x.second > best_reinf.second) {
                best_reinf = {u, x.second};
            }
            continue;
        }
        if (1)
        if (m != 2 && L[u] > 2 && x.first == 3) {
            takeovers.push_back(u);
            continue;
        }
        cand_moves.push_back(u);
    }
    if (best_reinf.first != -1) cand_moves.push_back(best_reinf.first);

    last2 = N2;
    while (!takeovers.empty()) AddCand();

    if (cand_moves.empty()) cand_moves.push_back(st[0]);

    candn = SZ(cand_moves);
    for (int u : cand_moves) { scores[u] = 0.0; ds[u].clear(); }
    start_sz = candn;

    int tries = -1;
    int pl2_test_sid[PL2_TEST_SAMPLES][MAXM][MAXT];
    double pl2_test_thr[PL2_TEST_SAMPLES][MAXM][MAXT];
    double pl2_test_ratio[PL2_TEST_SAMPLES][MAXM][MAXT];
    int pl2_val_sid[PL2_VALID_SAMPLES][MAXM][MAXT];
    double pl2_val_thr[PL2_VALID_SAMPLES][MAXM][MAXT];
    double pl2_val_ratio[PL2_VALID_SAMPLES][MAXM][MAXT];

    auto FillChosenFrom = [&](int from_t) {
        F1(p, m-1) {
            for (int tt = from_t; tt < horizon_turn; tt++) {
                int sid = active_samples[p][samplers[p].WeightedSample()].first;
                chosen_sid[p][tt] = sid;
                chosen_threshold[p][tt] = my.nextDouble();
                chosen_ratio[p][tt] = my.nextDouble();
            }
        }
    };
    auto FillPackFrom = [&](int sid_pack[MAXM][MAXT], double thr_pack[MAXM][MAXT], double ratio_pack[MAXM][MAXT], int from_t) {
        F1(p, m-1) {
            for (int tt = from_t; tt < horizon_turn; tt++) {
                sid_pack[p][tt] = active_samples[p][samplers[p].WeightedSample()].first;
                thr_pack[p][tt] = my.nextDouble();
                ratio_pack[p][tt] = my.nextDouble();
            }
        }
    };

    while (candn >= 2) {
        tries++;
        bool need_pl2 = (rt != T - 1 && tries >= 2 && candn <= pl2_t && (rt + 1) < horizon_turn);
        FillChosenFrom(rt);

        if (need_pl2) {
            AT(13);
            F0(rep, PL2_TEST_SAMPLES) FillPackFrom(pl2_test_sid[rep], pl2_test_thr[rep], pl2_test_ratio[rep], rt + 1);
            F0(rep, PL2_VALID_SAMPLES) FillPackFrom(pl2_val_sid[rep], pl2_val_thr[rep], pl2_val_ratio[rep], rt + 1);
        }

        F0(ci, candn) {
            bmove = cand_moves[ci];
            F0(u, N2) V2[u] = V[u];
            if (bmove >= N2) {
                F0(u, N2) V2[u] = V2s[bmove][u];
                int& nm = cand_1st[bmove];
                GreedyMove();
                nm = bmove;
            }
            int ply1_mv = bmove;
            GenLastSample(t);
            Local();
            double cscore = 0.0;
            if (rt != T - 1) {
                t = rt + 1;
                FindAllMoves();

                bool use_2ply = need_pl2;
                if (!use_2ply) {
                    AT(11);
                    for (; t < horizon_turn; ) {
                        if (m == 2) QuickMove();
                        else GreedyMove();
                        GenLastSample(t);
                        t++;
                        LocalExtra(t == horizon_turn);
                    }
                    cscore = Score(horizon_turn);
                } else {
                    AT(12);
                    int O1[N2], L1[N2], st1[MAXM];
                    B all1[MAXM], ac1[MAXM], pc1[MAXM], mc1[MAXM], own1[MAXM];
                    F0(i, N2) { O1[i] = O[i]; L1[i] = L[i]; }
                    F0(p, m) {
                        st1[p] = st[p];
                        all1[p] = all[p];
                        ac1[p] = ac[p];
                        pc1[p] = pc[p];
                        mc1[p] = mc[p];
                        own1[p] = own_cells[p];
                    }
                    B bl1 = bl;

                    int sec_mv[12], secn = 0;
                    auto add_sec = [&](int u) {
                        if (u < 0) return;
                        F0(i, secn) if (sec_mv[i] == u) return;
                        sec_mv[secn++] = u;
                    };

                    int best_greedy_c[4];
                    GreedyAllC(best_greedy_c);
                    add_sec(bmove);
                    if (ply1_mv >= 0 && (RealAllMask(0) & Bit(ply1_mv))) add_sec(ply1_mv);
                    F0(C, 4) add_sec(best_greedy_c[C]);
                    if (secn == 0) add_sec(st1[0]);
                    if (m == 2) {
                        QuickMove(); add_sec(bmove);
                    }

                    auto EvalSecond = [&](int mv) {
                        F0(i, N2) { O[i] = O1[i]; L[i] = L1[i]; }
                        F0(p, m) {
                            st[p] = st1[p];
                            all[p] = all1[p];
                            ac[p] = ac1[p];
                            pc[p] = pc1[p];
                            mc[p] = mc1[p];
                            own_cells[p] = own1[p];
                        }
                        bl = bl1;

                        t = rt + 1;
                        bmove = mv;
                        GenLastSample(t);
                        Local();

                        if (t + 1 < horizon_turn) {
                            t = rt + 2;
                            FindAllMoves();
                            for (; t < horizon_turn; ) {
                                if (m == 2) QuickMove();
                                else GreedyMove();
                                GenLastSample(t);
                                t++;
                                LocalExtra(t == horizon_turn);
                            }
                        }
                        return Score(horizon_turn);
                    };

                    auto ApplyPack = [&](int sid_pack[MAXM][MAXT], double thr_pack[MAXM][MAXT], double ratio_pack[MAXM][MAXT]) {
                        F1(pp, m-1) {
                            for (int tt = rt + 1; tt < horizon_turn; tt++) {
                                chosen_sid[pp][tt] = sid_pack[pp][tt];
                                chosen_threshold[pp][tt] = thr_pack[pp][tt];
                                chosen_ratio[pp][tt] = ratio_pack[pp][tt];
                            }
                        }
                    };

                    // 1) test samples: choose ply-2 move by best SUM over test samples.
                    double sec_test_sum[12];
                    F0(si2, secn) sec_test_sum[si2] = 0.0;
                    F0(rep, PL2_TEST_SAMPLES) {
                        ApplyPack(pl2_test_sid[rep], pl2_test_thr[rep], pl2_test_ratio[rep]);
                        F0(si2, secn) sec_test_sum[si2] += EvalSecond(sec_mv[si2]);
                    }

                    int best2_idx = 0;
                    double best2_test_sum = sec_test_sum[0];
                    F1(si2, secn - 1) if (sec_test_sum[si2] > best2_test_sum) {
                        best2_test_sum = sec_test_sum[si2];
                        best2_idx = si2;
                    }
                    int best2_mv = sec_mv[best2_idx];

                    // 2) validation samples: evaluate chosen ply-2 move.
                    double val_sum = 0.0;
                    F0(rep, PL2_VALID_SAMPLES) {
                        ApplyPack(pl2_val_sid[rep], pl2_val_thr[rep], pl2_val_ratio[rep]);
                        val_sum += EvalSecond(best2_mv);
                    }
                    cscore = PL2_TEST_SUM_RATIO * val_sum + PL2_TEST_SUM_RATIO * best2_test_sum;
                }
            }
            else cscore = Score(horizon_turn);
            ds[cand_moves[ci]].push_back(cscore);
            scores[cand_moves[ci]] += cscore;
            t = rt;
            F0(i, N2) {
                O[i] = hO[t][i];
                L[i] = hL[t][i];
            }
            F0(p, m) st[p] = hst[t][p];
            F0(p, m) all[p] = all2[p];
            MakeBL();
        }

        //double pr = 1.0 * tries / 20;
        double pr = (Elapsed() - start_time) / (until - start_time);
        if (pr > 1.0) break;
        HalvingStep(pr);

        if (m == 2) // TODO: m=3, tr=8
        if (tries + 1 == 10) {
            AT(15);
            sort(cand_moves.begin(), cand_moves.end(), [&](int i1, int i2) {
                return scores[i1] > scores[i2];
            });
            for (int i = candn - 1; i >= 0; i--) {
                int u = cand_moves[i];
                int dom = 0;
                F0(j, i) {
                    dom = 1;
                    int v = cand_moves[j];
                    F0(k, SZ(ds[u])) if (ds[u][k] > ds[v][k]) { dom = 0; break; }
                    if (dom) break;
                }
                if (dom) { cand_moves.erase(cand_moves.begin() + i); candn--; }
            }
        }
    }

    for (int u : cand_moves) {
        double av = scores[u];
        if (av > bv) {
            bv = av;
            bu = u < N2 ? u : cand_1st[u];
            if (u >= N2) last_active=u; else last_active=-1;
        }
    }

    //PR(rt, tries+1, Elapsed() - start_time);
    bmove = bu;
}

void TouchMove() {
    int bv2 = inf;
    int bv = -1;
    for (B b = RealAllMask(0); b; ) {
        int u = PopBit(b);
        for (B b2 = ac[1]; b2; ) {
            int v = PopBit(b2);
            int av2 = sqr(qx[u] - qx[v]) + sqr(qy[u] - qy[v]);
            int av = V[u];
            if (av2 < bv2 || (av2 == bv2 && av < bv)) {
                bv2 = av2;
                bv = av;
                bmove = u;
            }
        }
    }
}

void ChooseMove() {
    if (m == 2) {
        // TODO: predict
        if (!touch) {
            for (B b = ac[0]; b; ) {
                int u = PopBit(b);
                for (int v : adj[u]) if (O[v] > 0) { touch = 1; break; }
            }

            if (touch) slow_start = rt; else {
                TouchMove();
                return;
            }
        }
    }

    if (rt >= slow_start) {
        SlowMove();
    } else {
        QuickMove();
    }
}

Rand s_rng(1);

double RandomPar(int k) {
    if (k == 4) return s_rng.nextDouble(0.1, 0.5);
    return s_rng.nextDouble(0.3, 1.0);
}

void ActivateSamples() {
    F1(p, m-1) {
        active_samples[p].clear();
        rprob[p] = 0.0;
        double tot = 0.0, mx = -1e9;
        F0(si, SAMPLEN) mx = max(mx, w[si][p]);
        F0(si, SAMPLEN) if (mx - w[si][p] < 5) tot += exp(w[si][p] - mx);
        F0(si, SAMPLEN) if (mx - w[si][p] < 5) {
            double r = exp(w[si][p] - mx) / tot;
            double e = a[si][p][4];
            rprob[p] += r * e;
            active_samples[p].push_back({si, r});
        }
    }
}

void InitStats() {
    s_rng.reseed(1);
    F0(si, SAMPLEN) F1(p, m-1) {
        F0(t, T) f[si][p][t] = 0;
        cur_random[si][p] = 0;
        w[si][p] = 0.0;
        F0(k, 5) a[si][p][k] = RandomPar(k);
    }
    F1(p, m-1) {
        sure_random[p] = 0;
        ts[p].clear();
    }
    ActivateSamples();
}

void UpdateStats() {
    AT(10);

    RESAMPLE = 40 / (m - 1);
    SAMPLEN = min(MAX_SAMPLEN, 400 / (t + 1));

    F1(p, m-1) {
        pii x = hLastC[t][p];
        int C = x.first, val = x.second;
        int is_random = 1;
        if (hMax[t][p][C] == val) {
            is_random = 0;
            F0(C2, 4) if (C2 != C && hMax[t][p][C2] * 0.3 > hMax[t][p][C] * 1.0) {
                is_random = 1;
                break;
            }
        }
        if (is_random) {
            sure_random[p]++;
            F0(si, SAMPLEN) {
                cur_random[si][p]++;
                double e = a[si][p][4];
                w[si][p] += log(e / hN[t][p]);
            }
        } else {
            ts[p].push_back(t);

            F0(si, SAMPLEN) {
                double v1 = a[si][p][C] * val;
                F0(C2, 4) if (C2 != C) {
                    if (a[si][p][C2] * hMax[t][p][C2] > v1) {
                        f[si][p][t]++;
                        break;
                    }
                }
                double e = a[si][p][4];
                if (f[si][p][t]) {
                    cur_random[si][p]++;
                    w[si][p] += log(e / hN[t][p]);
                }
                else {
                    double prob = e / hN[t][p] + (1 - e) / hCnt[t][p][C];
                    w[si][p] += log(prob);
                }
            }
        }
        F0(si, SAMPLEN) {
            F0(ri, RESAMPLE) {
                c_o[5]++;
                F0(k, 5) {
                    double v2 = RandomPar(k);
                    double v1 = a[si][p][k];
                    a[si][p][k] = v2;
                    double w_delta = 0;
                    if (k < 4) {
                        for (int t : ts[p]) {
                            pii x = hLastC[t][p];
                            int C = x.first, val = x.second;
                            double v1 = a[si][p][C] * val;
                            int nf = 0;
                            F0(C2, 4) if (C2 != C) {
                                if (a[si][p][C2] * hMax[t][p][C2] > v1) {
                                    nf++;
                                    break;
                                }
                            }
                            double e = a[si][p][4];
                            if (f[si][p][t] != nf) {
                                if (f[si][p][t]) {
                                    w_delta -= log(e / hN[t][p]);
                                    w_delta += log(e / hN[t][p] + (1 - e) / hCnt[t][p][C]);
                                } else {
                                    w_delta -= log(e / hN[t][p] + (1 - e) / hCnt[t][p][C]);
                                    w_delta += log(e / hN[t][p]);
                                }
                            }
                        }
                    } else {
                        w_delta += cur_random[si][p] * (log(v2) - log(v1));
                        for (int t : ts[p]) if (!f[si][p][t]) {
                            pii x = hLastC[t][p];
                            int C = x.first;

                            double prob1 = v1 / hN[t][p] + (1 - v1) / hCnt[t][p][C];
                            w_delta -= log(prob1);
                            double prob2 = v2 / hN[t][p] + (1 - v2) / hCnt[t][p][C];
                            w_delta += log(prob2);
                        }
                    }
                    if (w_delta >= 0.0 || log(max(EPS, s_rng.nextDouble())) < w_delta) {
                        w[si][p] += w_delta;
                        if (k < 4) {
                            for (int t : ts[p]) {
                                pii x = hLastC[t][p];
                                int C = x.first, val = x.second;
                                double v1 = a[si][p][C] * val;
                                int nf = 0;
                                F0(C2, 4) if (C2 != C) {
                                    if (a[si][p][C2] * hMax[t][p][C2] > v1) {
                                        nf++;
                                        break;
                                    }
                                }
                                if (f[si][p][t]) cur_random[si][p]--;
                                f[si][p][t] = nf;
                                if (nf) cur_random[si][p]++;
                            }
                        }
                    } else {
                        a[si][p][k] = v1;
                    }
                }
            }
        }
    }
    ActivateSamples();
}

void GenLastRandom() {
    last[0] = bmove;
    F1(p, m-1) {
        vector<int> cand;
        static int vis[N2];
        static int vis_id = 0;
        vis_id++;
        queue<int> q;
        q.push(st[p]);
        vis[st[p]] = vis_id;
        while (!q.empty()) {
            int u = q.front();
            q.pop();

            bool ok = true;
            F0(i, m) if (i != p && st[i] == u) { ok = false; break; }
            if (ok) cand.push_back(u);

            if (O[u] == p) {
                F0(k, an[u]) {
                    int v = aa[u][k];
                    if (vis[v] == vis_id) continue;
                    vis[v] = vis_id;
                    q.push(v);
                }
            }
        }

        double r1 = my.nextDouble();
        double r2 = my.nextDouble();
        if (r1 < f_w[p][4]) {
            int idx = (int)floor(r2 * (double)SZ(cand));
            if (idx >= SZ(cand)) idx = SZ(cand) - 1;
            last[p] = cand[idx];
        } else {
            vector<double> scores;
            scores.reserve(SZ(cand));
            double mx = -1e100;
            for (int u : cand) {
                double sc = F_EvalMove(p, u);
                scores.push_back(sc);
                mx = max(mx, sc);
            }
            double tol = 1e-9 * max(1.0, abs(mx));
            vector<int> best;
            best.reserve(SZ(cand));
            F0(i, SZ(cand)) if (scores[i] >= mx - tol) best.push_back(i);
            int idx = (int)floor(r2 * (double)SZ(best));
            if (idx >= SZ(best)) idx = SZ(best) - 1;
            last[p] = cand[best[idx]];
        }
    }
}

void GenLast() {
    last[0] = bmove;
    F1(p, m-1) {
        vector<int> cand;
        static int vis[N2];
        static int vis_id = 0;
        vis_id++;
        queue<int> q;
        q.push(st[p]);
        vis[st[p]] = vis_id;
        while (!q.empty()) {
            int u = q.front();
            q.pop();

            bool ok = true;
            F0(i, m) if (i != p && st[i] == u) { ok = false; break; }
            if (ok) cand.push_back(u);

            if (O[u] == p) {
                F0(k, an[u]) {
                    int v = aa[u][k];
                    if (vis[v] == vis_id) continue;
                    vis[v] = vis_id;
                    q.push(v);
                }
            }
        }

        double r1 = f_r1[current_sid][t][p];
        double r2 = f_r2[current_sid][t][p];
        if (r1 < f_w[p][4]) {
            int idx = (int)floor(r2 * (double)SZ(cand));
            if (idx >= SZ(cand)) idx = SZ(cand) - 1;
            last[p] = cand[idx];
        } else {
            vector<double> scores;
            scores.reserve(SZ(cand));
            double mx = -1e100;
            for (int u : cand) {
                double sc = F_EvalMove(p, u);
                scores.push_back(sc);
                mx = max(mx, sc);
            }
            double tol = 1e-9 * max(1.0, abs(mx));
            vector<int> best;
            best.reserve(SZ(cand));
            F0(i, SZ(cand)) if (scores[i] >= mx - tol) best.push_back(i);
            int idx = (int)floor(r2 * (double)SZ(best));
            if (idx >= SZ(best)) idx = SZ(best) - 1;
            last[p] = cand[best[idx]];
        }
    }
}

void Resolve() {
    if (!STANDARD) {
        GenLast();
        F1(p, m-1) hLastC[t][p] = Classify(p, last[p]);
        Local();
    } else {
        cout << qx[bmove] << " " << qy[bmove] << endl;
        F0(p, m) last[p] = NextPos();
        F1(p, m-1) hLastC[t][p] = Classify(p, last[p]);
        F0(p, m) st[p] = NextPos();
        F0(i, N2) cin >> O[i];
        F0(i, N2) cin >> L[i];
    }
    MakeBL();
    F0(p, m) hlast[t][p] = last[p];
}

#ifdef VIS
#include "vis.h"
#else
inline void VisSnapshotEstimatedAttackTurn(int) {}
void ShowIt(bool f = false) { (void)f; }
#endif

void Solve() {
    Prepare();

    int gp = -1;

    if (m >= 3 && U >= 2) {
        if (gp != -1 && gp != 0) exit(0);
    } else if (m >= 3 && U == 1) {
        if (gp != -1 && gp != 1) exit(0);
        slow_start = 0;
    } else if (m == 2) {
        if (gp != -1 && gp != 2) exit(0);
        slow_start = 55;
    } else throw;

    InitStats();
    for (rt = 0; rt < T; rt++) {
        t = rt;
        F0(i, N2) {
            hO[t][i] = O[i];
            hL[t][i] = L[i];
        }
        F0(p, m) hst[t][p] = st[p];
        FindAllMoves();
        VisSnapshotEstimatedAttackTurn(t);
        ChooseMove();
        t = rt;
        if (rt == T - 1 && !STANDARD && LAST_AVG) {
            int xO[N2], xL[N2], xst[MAXM];
            F0(i, N2) { xO[i] = O[i]; xL[i] = L[i]; }
            F0(p, m) xst[p] = st[p];
            B xbl = bl;

            double avg = 0.0;
            F1(its, 1000) {
                GenLastRandom();
                Local();

                avg += ScoreRaw();

                F0(i, N2) { O[i] = xO[i]; L[i] = xL[i]; }
                F0(p, m) st[p] = xst[p];
                bl = xbl;
            }
            bscore = 1e5 * avg / 1000.0 + 0.5;
        } else {
            Resolve();
        }
        if (rt != T - 1) UpdateStats();
    }

    if (!LAST_AVG) {
        bscore = 1e5 * ScoreRaw() + 0.5;
    }
    PR(bscore);

    ShowIt();

    Report();
}

void ReadInput() {
    cin >> n >> m >> T >> U;
    PrepareGrid();
    F0(i, N2) cin >> V[i];
    F0(p, m) st[p] = NextPos();
    MakeBL();
    if (!STANDARD) {
        F1(p, m-1) F0(k, 5) cin >> f_w[p][k];
        F0(t, T) F1(p, m-1) cin >> f_r1[0][t][p] >> f_r2[0][t][p];
    }
}

int main(int argc, char* argv[]) {
    Init();

    int seed1 = 0, seed2 = 0;
    if (argc>1) {
        if (argc == 2) {
            seed1 = seed2 = atoi(argv[1]);
        } else {
            seed1 = atoi(argv[1]);
            seed2 = atoi(argv[2]);
        }
        STANDARD=0;
    }

    if (STANDARD) {
        ReadInput();
        Solve();
        return 0;
    }

    for (int fseed=seed1; fseed<=seed2; fseed++) {
        seed = fseed;
        char inp[128];
        sprintf(inp, "in/%04d.txt", seed);
        //char outp[128];
        //sprintf(outp, "out/%04d.txt", seed);
        ignore = freopen(inp, "r", stdin);
        //ignore = freopen(outp, "w", stdout);
        ReadInput();
        if (0 && (U != 1 || m == 2)) {
            cerr << "Skip " << seed << endl;
            continue;
        }
        Solve();
        cerr << "Seed #" << seed << " ";
        cerr << bscore << endl;
        //cout << "Score would be " << bscore << endl;
    }

    return 0;
}
