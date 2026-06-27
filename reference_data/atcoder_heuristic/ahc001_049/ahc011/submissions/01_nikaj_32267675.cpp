const double TL = 2.65;
#include <bits/stdc++.h>
int STANDARD = 1;
using namespace std;
#define F0(i,n) for (int i=0; i<n; i++)
#define FR(i,n) for (int i=n-1; i>=0; i--)
#define F1(i,n) for (int i=1; i<=n; i++)
#define CL(a,x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())

#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC target ("avx2")

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
Rand my(2021);
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

const int N = 16;
const int LOGN = 1 << 16;
double logs[LOGN];
enum {LEFT, UP, RIGHT, DOWN, STAY};
const int DX[]={0,-1,0,1,0};
const int DY[]={-1,0,1,0,0};
const string DS="LURDN";
int n, elapsedTime, T;
vector<int> all;
char buff[1<<27];
char *bb = buff;
int gcorner;
int score, bscore, grid[N][N], a[N*N], g[N*N], tmp[N][N], dist[N*N][N*N], cl[N*N][16];
vector<int> gpos[16], nei[N*N];
vector<int> f;
int nx[N*N][4], nk[N*N][N*N], dist2[N*N][N*N];
vector<vector<int>> starta;
vector<vector<pii>> starteq;
string ans;

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
template<class T>
void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }

void Prepare() {
    bscore = 0;
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);

    F0(i, n) F0(j, n) {
        int id = ID(i, j);
        g[id] = grid[i][j];
        gpos[g[id]].push_back(id);
        all.push_back(id);
        F0(k, 4) {
            pii p = Move(pii(i, j), k);
            nx[id][k] = -1;
            if (In(p)) {
                int np = PID(p);
                nei[id].push_back(np);
                nx[id][k] = np;
                nk[id][np] = k;
            }
        }
    }
    for (int i : all) for (int j : all) {
        dist[i][j] = d1(XY(i), XY(j));
        dist2[i][j] = max(dist[i][j], 1);
    }
    f.assign(16, 0);
    F0(i, n) F0(j, n) f[grid[i][j]]++;

    F0(i, n*n) F0(j, 16) cl[i][j] = 2*n;
    F0(i, n*n) F0(j, n*n) cl[i][g[j]] = min(cl[i][g[j]], dist2[i][j]);
}

vector<int> adj[N*N], pr, of;
vector<int> q;
int vis[N*N];
int dq[8*N*N],di, dj;
void BFS(int x, int y) {
    pr.assign(n*n, -1);
    pr[x] = x;
    q.clear();
    q.push_back(x);
    F0(index, SZ(q)) {
        int i = q[index];
        for (int j : adj[i]) if (pr[j] == -1) {
            pr[j] = i;
            if (j == y) return;
            q.push_back(j);
        }
    }
}

int seq[N], seqn, lis[N], w[N*N];

int SeqPenalty() {
    if (is_sorted(seq, seq + seqn)) return 0;
    int ret = 0;
    F0(i, seqn) {
        lis[i] = 1;
        F0(j, i) {
            if (seq[j] < seq[i] && lis[j] + 1 > lis[i]) lis[i] = lis[j] + 1;
        }
        ret = max(ret, lis[i]);
    }
    return seqn - ret;
}

pii lst;
int npen[5] = {14, 13, 5, 1, 0};
int sv[N*N], sn;

struct State {
    char *a;
    int pind;
    int aindex;
    int lastk;
    int dsum, psum, bsum, mx;
    int eval;
    int corner;

    void DSum() {
        dsum = 0;
        F0(i, n*n) if (a[i] != corner) dsum += dist[a[i]][i];
    }

    void Shuffle(int val) {
        vector<int>& sa = starta[aindex];
        sn = 0;
        F0(i, n*n) if (sa[a[i]] == sa[val]) sv[sn++] = i;
        int bw = 0;
        int sg = 1;
        while (sg) {
            sg = 0;
            F0(jj, sn) F0(ii, jj) {
                int i = sv[ii], j = sv[jj];
                int av = (dist[a[i]][i] + dist[a[j]][j]) - (dist[a[i]][j] + dist[a[j]][i]);
                int cv = 10 * av + ((a[i] == i) + (a[j] == j) - (a[i] == j) - (a[j] == i));
                if (cv > 0) {
                    sg = 1;
                    swap(a[i], a[j]);
                    dsum -= av;
                    bw++;
                }
            }
        }
        if (bw&1) {
            Process();
            vector<pii>& eq = starteq[aindex];
            int bv = -inf, dv = -inf;
            for (pii& p : eq) {
                int i = w[p.first], j = w[p.second];
                int av = (dist[a[i]][i] + dist[a[j]][j]) - (dist[a[i]][j] + dist[a[j]][i]);
                int cv = 10 * av + ((a[i] == i) + (a[j] == j) - (a[i] == j) - (a[j] == i));
                if (cv > dv) {
                    dv = cv;
                    bv = av;
                    lst = pii(i, j);
                    //if (bv == 0) break;
                }
            }
            dsum -= bv;
            swap(a[lst.first], a[lst.second]);
        }
        if (bw) {
            PSum();
            AdjPenalty();
        }
    }

    void AdjPenalty() {
        bsum = 0;
        F0(i, n*n) if (a[i] != corner && a[i] != i) {
            int cnt = 0;
            for (int x : nei[a[i]]) {
                if (a[x] != x || a[x] == corner) cnt++;
            }
            bsum += npen[cnt];
        }
    }

    int AdjPenalties(int u) {
        int ret = 0;
        for (int i : nei[u]) if (a[i] != i && i != corner) {
            int cnt = 0;
            for (int x : nei[i]) {
                if (a[x] != x || a[x] == corner) cnt++;
            }
            ret += npen[cnt];
        }
        return ret;
    }

    void Reachability() {
        di = dj = 4*N*N;
        F0(i, n*n) vis[i] = -1;
        dq[dj++] = corner;
        vis[corner] = 0;
        mx = 0;
        while (di < dj) {
            int i = dq[di++];
            for (int j : nei[i]) {
                int cost = vis[i] + (a[j] == j);
                if (vis[j] == -1 || vis[j] > cost) {
                    vis[j] = cost;
                    if (cost == vis[i]) {
                        mx = cost;
                        dq[--di] = j;
                    } else dq[dj++] = j;
                }
            }
        }
    }

    int RowPenalty(int i) {
        seqn = 0;
        F0(j, n) {
            int x = a[ID(i, j)];
            if (x != corner && x / n == i) seq[seqn++] = x % n;
        }
        return SeqPenalty();
    }
    int ColPenalty(int j) {
        seqn = 0;
        F0(i, n) {
            int x = a[ID(i, j)];
            if (x != corner && x % n == j) seq[seqn++] = x / n;
        }
        return SeqPenalty();
    }
    void PSum() {
        psum = 0;
        F0(i, n) psum += RowPenalty(i);
        F0(i, n) psum += ColPenalty(i);
    }

    void Evaluate() {
        eval = 10 * dsum + 16 * psum + 1 * bsum + 21 * mx;
    }
    void Process() {
        F0(i, n*n) w[a[i]] = i;
    }
    ll Hash() {
        ll ret = 0;
        F0(i, n*n) ret = ret * 239 + a[i];
        return ret;
    }
    void Print() {
        F0(ii, n) {
            F0(jj, n) if (a[ii*n+jj]==corner) fprintf(stderr, "   "); else
            if (a[ii*n+jj]==ii*n+jj) fprintf(stderr, " * "); else fprintf(stderr, "%3d", a[ii*n+jj]);
            fprintf(stderr, "\n");
        }
    }
};

int BW = 1000;
string last_path;
vector<State> beam[2048];
set<pii> pq[2048];
unordered_map<ll, int> Hashes;

bool Solvable(const vector<int>& a) {
    int inv = 0;
    F0(j, n*n) F0(i, j) if (a[i]>a[j] && a[i] != gcorner && a[j] != gcorner) inv^=1;
    if (n%2==0) F0(i, n*n) if (a[i] == gcorner) inv ^= (X(gcorner)^X(i)) & 1;
    return !inv;
}

void AddStart(pair<vector<int>,vector<int>> p) {
    vector<int>& v = p.first;
    vector<int>& a = p.second;
    vector<pii> eq;
    F0(i, n*n) F0(j, i) if (a[i] == a[j]) eq.push_back(pii(j, i));
    int aindex = SZ(starta);
    starta.push_back(a);
    starteq.push_back(eq);


    State s0;
    s0.a = bb;
    bb += n*n;
    F0(j, n*n) s0.a[j] = v[j];
    F0(i,SZ(a)) if (a[i]==0) s0.corner=i;
    s0.aindex = aindex;
    s0.lastk = -1;
    s0.DSum();
    s0.PSum();
    s0.AdjPenalty();
    s0.Reachability();

    ll h = s0.Hash();
    if (!Hashes.count(h)) {
        pq[0].insert(pii(SZ(beam[0]), SZ(beam[0])));
        beam[0].push_back(s0);
        Hashes[h] = 0;
    }
}

int maxT;
State ns;
void BeamStep() {
    for (int depth = 0; depth < maxT; depth++) {
        F0(e, BW) {
            if (pq[depth].empty()) break;
            int i = pq[depth].begin()->second;
            pq[depth].erase(pq[depth].begin());
            if (beam[depth][i].dsum == 0) {
                last_path = "";
                int d = depth;
                while (d > 0) {
                    last_path.push_back(DS[beam[d][i].lastk]);
                    i = beam[d][i].pind;
                    d--;
                }
                maxT = depth;
                reverse(last_path.begin(), last_path.end());
                return;
            }
            if (beam[depth][i].dsum + depth > maxT) continue;

            State& s = beam[depth][i];
            s.Process();
            int cpos = w[s.corner];
            int cx = X(cpos), cy = Y(cpos);

            F0(k, 4) if ((2^k) != s.lastk) {
                int npos = nx[cpos][k];
                if (npos == -1) continue;
                ns = s;
                ns.a = bb;
                bb += n*n;
                memcpy(ns.a, s.a, n*n);
                ns.dsum -= dist[ns.a[npos]][npos];

                int bdelta = 0;
                if (ns.a[npos] == npos || ns.a[npos] == cpos) {
                    bdelta -= ns.AdjPenalties(npos);
                    bdelta -= ns.AdjPenalties(cpos);
                }

                if (k&1) {
                    ns.psum -= ns.RowPenalty(X(npos));
                    ns.psum -= ns.RowPenalty(cx);
                } else {
                    ns.psum -= ns.ColPenalty(Y(npos));
                    ns.psum -= ns.ColPenalty(cy);
                }
                swap(ns.a[cpos], ns.a[npos]);

                ll h = ns.Hash();
                if (!Hashes.count(h) || Hashes[h] > depth) {
                    Hashes[h] = depth;

                    ns.dsum += dist[ns.a[cpos]][cpos];
                    if (k&1) {
                        ns.psum += ns.RowPenalty(X(npos));
                        ns.psum += ns.RowPenalty(cx);
                    } else {
                        ns.psum += ns.ColPenalty(Y(npos));
                        ns.psum += ns.ColPenalty(cy);
                    }
                    if (ns.a[cpos] == cpos || ns.a[cpos] == npos) {
                        bdelta += ns.AdjPenalties(npos);
                        bdelta += ns.AdjPenalties(cpos);
                        ns.bsum += bdelta;
                    }
                    ns.Shuffle(ns.a[cpos]);
                    if (ns.a[cpos] == cpos || ns.a[cpos] == npos) {
                        ns.Reachability();
                    }
                    ns.Evaluate();
                    ns.lastk = k;
                    ns.pind = i;

                    pq[depth + 1].insert(pii(ns.eval * 128 + my.nextInt(128), SZ(beam[depth + 1])));
                    beam[depth + 1].push_back(ns);
                }
            }
        }
    }
}

void SliderPuzzle() {
    maxT = min(T, 450);
    if (n == 10) maxT = 600;
    int steps = 0;
    while (Elapsed() < TL) {
        ++steps;
        if (steps > 1) BW = 1;
        BeamStep();
    }
    //PR(steps);
}

void ClearGraph() {
    of.assign(16, 0);
    of[0] = n*n;
    F0(i, n*n) a[i] = 0;
    F0(i, n*n) {
        adj[i].clear();
    }
}

void AddEdge(int x, int y) {
    adj[x].push_back(y);
    adj[y].push_back(x);
    int k = nk[x][y];
    of[a[x]]--;
    of[a[y]]--;
    a[x] |= (1<<k);
    a[y] |= (1<<(k^2));
    of[a[x]]++;
    of[a[y]]++;
}

void TryAddEdge(int x, int y) {
    int k = nk[x][y];
    of[a[x]]--;
    of[a[y]]--;
    a[x] |= (1<<k);
    a[y] |= (1<<(k^2));
    of[a[x]]++;
    of[a[y]]++;
}

void TryRemoveEdge(int x, int y) {
    int k = nk[x][y];
    of[a[x]]--;
    of[a[y]]--;
    a[x] &= ~(1<<k);
    a[y] &= ~(1<<(k^2));
    of[a[x]]++;
    of[a[y]]++;
}

void RemoveEdge(int x, int y) {
    for(int i=SZ(adj[x])-1; i>=0; i--) if (adj[x][i] == y) { swap(adj[x][i], adj[x].back()); adj[x].pop_back(); break; }
    for(int i=SZ(adj[y])-1; i>=0; i--) if (adj[y][i] == x) { swap(adj[y][i], adj[y].back()); adj[y].pop_back(); break; }
    int k = nk[x][y];
    of[a[x]]--;
    of[a[y]]--;
    a[x] &= ~(1<<k);
    a[y] &= ~(1<<(k^2));
    of[a[x]]++;
    of[a[y]]++;
}

vector<int> v;
int hn, hans;
int ha[N*N][N*N];
int hp[N*N];
vector<int> hpos[16];

void Hungarian() {
    vector<int> u(hn+1), v(hn+1), p(hn+1), way(hn+1);
    F1(i, hn) {
        p[0] = i;
        int j0 = 0;
        vector<int> minv (hn+1, inf);
        vector<char> used (hn+1, false);
        do {
            used[j0] = true;
            int i0 = p[j0],  delta = inf,  j1 = 0;
            F1(j, hn) if (!used[j]) {
                int cur = ha[i0][j]-u[i0]-v[j];
                if (cur < minv[j]) minv[j] = cur, way[j] = j0;
                if (minv[j] < delta) delta = minv[j],  j1 = j;
            }
            F0(j, hn+1) {
                if (used[j]) u[p[j]] += delta,  v[j] -= delta;
                else minv[j] -= delta;
            }
            j0 = j1;
        } while (p[j0] != 0);
        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }
    hans = -v[0];
    F1(i, hn) hp[p[i]] = i;
}

int ComputeCost() {
    v.assign(n*n, -1);
    int ret = 0;
    F0(i, 16) hpos[i].clear();
    F0(i, n*n) hpos[a[i]].push_back(i);
    F0(o, 16) {
        hn = SZ(gpos[o]);
        F1(i, hn) F1(j, hn) ha[i][j] = dist2[gpos[o][i-1]][hpos[o][j-1]];
        Hungarian();
        F1(i, hn) v[gpos[o][i-1]] = hpos[o][hp[i]-1];
        if (o != 0) ret += hans;
    }

    if (!Solvable(v)) {
        int bv = inf, bx = -1, by = -1;
        F0(x, n*n) for (int y : gpos[g[x]]) if (y > x) {
            int d = -dist2[x][v[x]] - dist2[y][v[y]] + dist2[x][v[y]] + dist2[y][v[x]];
            if (d < bv) {
                bv = d; bx = x; by = y;
            }
        }
        swap(v[bx], v[by]);
        ret += bv;
    }

    if (1) {
        int pen = 0;
        F0(i, n) {
            seqn = 0;
            F0(j, n) {
                int x = v[ID(i, j)];
                if (x != gcorner && x / n == i) seq[seqn++] = x % n;
            }
            pen += SeqPenalty();
        }
        F0(j, n) {
            seqn = 0;
            F0(i, n) {
                int x = v[ID(i, j)];
                if (x != gcorner && x % n == j) seq[seqn++] = x / n;
            }
            pen += SeqPenalty();
        }
        ret += 2 * pen;
    }

    return ret;
}

void BuildRandomGraph() {
    vector<pii> edges;
    F0(i, n) F0(j, n) if (ID(i, j) != gcorner) {
        if (i + 1 < n && ID(i + 1, j) != gcorner) edges.push_back(pii(ID(i, j), ID(i + 1, j)));
        if (j + 1 < n && ID(i, j + 1) != gcorner) edges.push_back(pii(ID(i, j), ID(i, j + 1)));
    }

    ClearGraph();

    RS(edges);
    for (pii e : edges) {
        int x = e.first, y = e.second;
        BFS(x, y);
        if (pr[y] == -1) {
            AddEdge(x, y);
        }
    }
}

int an, af[4];
int STEPS[11] = {0, 0, 0, 0, 0, 0, 4, 2, 1, 1, 1};

void FullSA() {
    set<ll> S;
    vector<pair<int, pair<vector<int>, vector<int>>>> cc;

    int nsteps = STEPS[n];
F0(st, nsteps) {
    if (st == 0) gcorner = ID(n/2,n/2);
    else if (st == 1) gcorner = ID(n/2,n/2-1);
    else if (st == 2) gcorner = ID(n/2-1,n/2);
    else if (st == 3) gcorner = ID(n/2-1,n/2-1);
    BuildRandomGraph();

    int fsum = 0;
    int dsum = 0;
    F0(o, 16) fsum += abs(f[o] - of[o]);
    F0(i, n*n) dsum += cl[i][a[i]];

    int tot = 1, acc = 0;
    double T1 = 0.0, T2 = 1.0, TEMP = 0.0, r = 0;
    //double startTime = Elapsed(), endTime = steps * TL / nsteps;
    int itc = 4000000;
    if (n == 6) itc = 3600000;
    if (n == 7) itc = 3000000;
    if (n == 8) itc = 2400000;
    if (n == 9) itc = 1800000;
    if (n == 10) itc = 1200000;

    itc /= nsteps;

    F0(iter, itc) {
        if ((iter & 255) == 0) {
            //r = (endTime - Elapsed()) / (endTime - startTime);
            r = 1.0 * (itc - iter) / itc;
            if (r <= 0) {
                cerr << iter << " " << fsum << " " << dsum << " " << 100.0*acc / tot << endl;
                break;
            }
            //PR(iter);
            TEMP = T1 + (T2 - T1) * r;
        }

        double maxd = TEMP * logs[my.nextInt(LOGN)];

        int x = my.nextInt(n*n);
        while (x == gcorner) x = my.nextInt(n*n);
        int k = my.nextInt(4), y = nx[x][k];
        while (y == -1 || y == gcorner) { k = (k+1)%4; y = nx[x][k]; }

        BFS(x, y);
        if (pr[y] == x) continue;

        vector<int> bpath;
        int z = y;
        while (z != x) {
            bpath.push_back(z);
            z = pr[z];
        }

        int sz = SZ(bpath);
        int index = my.nextInt(sz);

        double bdelta = maxd;
        int bx = -1, bx2 = -1, by = -1, by2 = -1, bdelta_fsum = -1;

        F0(brun, sz) {
            index++;
            if (index == sz) index = 0;

            int x2 = pr[bpath[index]], y2 = bpath[index];

            an = 0;
            af[an++] = x; af[an++] = y;
            if (x2 != x && x2 != y) af[an++] = x2;
            if (y2 != x && y2 != y) af[an++] = y2;

            int delta_dsum = 0, delta_fsum = 0;

            F0(o, 16) delta_fsum -= abs(f[o] - of[o]);
            F0(i, an) if(a[af[i]] != 0) delta_dsum -= cl[af[i]][a[af[i]]];

            TryRemoveEdge(x2, y2);
            TryAddEdge(x, y);

            F0(o, 16) delta_fsum += abs(f[o] - of[o]);
            F0(i, an) if(a[af[i]] != 0) delta_dsum += cl[af[i]][a[af[i]]];

            TryRemoveEdge(x, y);
            TryAddEdge(x2, y2);

            double delta = delta_fsum + delta_dsum * r * 1.0;

            if (delta < bdelta) {
                bdelta = delta;
                bx = x;
                by = y;
                bx2 = x2;
                by2 = y2;
                bdelta_fsum = delta_fsum;
            }
        }
        if (bx != -1) {
            RemoveEdge(bx2, by2);
            AddEdge(bx, by);
            fsum += bdelta_fsum;

            if (fsum == 0) {
                ll h = 0;
                F0(i,n*n) h = h*239+a[i];
                if (!S.count(h)) {
                    S.insert(h);
                    int cost = ComputeCost();
                    cc.push_back(make_pair(cost, make_pair(v, vector<int>(a, a+n*n))));
                }
            }
        }
    }
}
    sort(cc.begin(), cc.end());

    if (n == 10) BW = 500;
    else if (n == 9) BW = 650;
    else if (n == 8) BW = 850;
    else if (n == 7) BW = 1300;
    else BW = 1500;
    if (SZ(cc) > 100) cc.resize(100);
    vector<int> costs;
    for (auto p : cc) {
        AddStart(p.second);
        costs.push_back(p.first);
    }
    SliderPuzzle();
    //PR(bb - buff);
    cout << last_path << endl;
}

void Solve() {
    Prepare();

    FullSA();

    if (STANDARD) {
    }

    //Report();
}

void ReadInput() {
    cin >> n >> T;
    F0(i, n) {
        string s;
        cin >> s;
        F0(j, n) if (isdigit(s[j])) grid[i][j] = s[j] - '0';
        else grid[i][j] = s[j] - 'a' + 10;
    }
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
            ignore = freopen(inp, "r", stdin);
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
