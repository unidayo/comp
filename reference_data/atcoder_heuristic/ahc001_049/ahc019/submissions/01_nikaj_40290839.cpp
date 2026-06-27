int DBG = 0;
double TL = 5.0;
double TL3 = 5.8;
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
template<class A, class B, class C>
ostream& operator<<(ostream& os, const tuple<A, B, C>& p) { os << "(" << get<0>(p) << "," << get<1>(p) << "," << get<2>(p) << ")"; return os; }
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "["; F0(i,SZ(v)) { if (i>0) os << ","; os << v[i]; } os << "]"; return os;
}
template<class T>
ostream& operator<<(ostream& os, const set<T>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ","; cerr << i; } os << "}" << endl; return os;
}
template<class T, class R>
ostream& operator<<(ostream& os, const map<T,R>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ", "; cerr << i.first << ":" << i.second; } os << "}" << endl; return os;
}

//#pragma GCC optimize("O3")

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
const int N = 16;
const int DX[]={-1,0,1,0,0,0};
const int DY[]={0,1,0,-1,0,0};
const int DZ[]={0,0,0,0,-1,1};
enum {Top, Left, Bottom, Right, Front, Back};

int n, an[2];
bool f[2][2][N][N];
double score, bscore;
int ans[N*N*N], bans[N*N*N];
//int fill_id[2][N][N][N];
int m[2];

int d1(int x, int y) { return min(abs(x-y), n-abs(x-y)); }
int d1(pii p1, pii p2) { return d1(p1.first,p2.first)+d1(p1.second,p2.second); }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
bool In2(pii p) { return p.first >= 1 && p.first < n-1 && p.second >= 1 && p.second < n-1; }
//bool Good(pii p) { return In(p) && a[p.first][p.second] != -1; }
int sqr(int x) { return x*x; }
template<class T>
void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }

map<vector<int>, int> M;
int orientations[24][6], orientation_id;
vector<int> so[6][6];
void AddOrientation(vector<int> c) {
    if (M.count(c)) return;
    M[c] = orientation_id++;
    int curr_or = M[c];
    F0(i, 6) orientations[curr_or][i] = c[i];
    F0(k, 4) {
        vector<int> d = c;
        if (k == 0) {
            int tmp = d[Bottom];
            d[Bottom] = d[Right];
            d[Right] = d[Top];
            d[Top] = d[Left];
            d[Left] = tmp;
        } else if (k == 1) {
            int tmp = d[Bottom];
            d[Bottom] = d[Left];
            d[Left] = d[Top];
            d[Top] = d[Right];
            d[Right] = tmp;
        } else if (k == 2) {
            int tmp = d[Bottom];
            d[Bottom] = d[Front];
            d[Front] = d[Top];
            d[Top] = d[Back];
            d[Back] = tmp;
        } else if (k == 3) {
            int tmp = d[Bottom];
            d[Bottom] = d[Back];
            d[Back] = d[Top];
            d[Top] = d[Front];
            d[Front] = tmp;
        }
        AddOrientation(d);
    }
}

typedef __int128 MaskType;
int fn[2][2], fid[2][2][N][N];
MaskType maskf[2][2];
bool Inside(int x) { return 0 <= x && x < n; }
#define V(c) ans[(c)]
inline bool Empty(int c) { return c != -1 && V(c) == 0; }

const int MAXB = 32;
int nx[2];
int block[2][N*N*N], blockn;
int sn, wi[N*N*N], wj[N*N*N];
int cell_id[2][N][N][N], cell_t[N*N*N];
int fn_i[2][2][N*N], fn_j[2][2][N*N];
int nx_cell[N*N*N][6];
int GetDir(int p1, int p2) { F0(k, 6) if (nx_cell[p1][k] == p2) return k; return -1; }
int cell_fid[N*N*N][2];
double cell_value[2][2][N*N], value;
vector<int> outk[N*N*N], outcell[N*N*N];
const int C = 16;
int bos[C][MAXB+1], os[MAXB+1], bsn[C];
double bscores[C];
vector<int> blocks[MAXB+1][2], bblocks[C][MAXB+1][2];
int fc[2][2][N*N], unf, sunf;
const int LOGN = 1 << 16;
double logs[LOGN];

inline int bc(MaskType c) {
    return (__builtin_popcountll(c >> 64))
         + (__builtin_popcountll(c));
}

void Prepare() {
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);

    F0(t, 2) F0(side, 2) fn[t][side] = 0;
    CL(-1, wi); CL(-1, wj);
    CL(-1, fid);

    F0(t, 2) F0(i, n) F0(j, n) F0(side, 2) {
        if (f[t][side][i][j]) {
            fn_i[t][side][fn[t][side]] = i;
            fn_j[t][side][fn[t][side]] = j;
            fid[t][side][i][j] = fn[t][side]++;
        }
    }

    an[0] = an[1] = 0;
    CL(0, ans);
    F0(t, 2) {
        F0(x, n) F0(y, n) F0(z, n) {
            if (!f[t][0][z][x] || !f[t][1][z][y]) {
                cell_id[t][x][y][z] = -1;
            } else {
                // cell_x[t][an[t]] = x; cell_y[t][an[t]] = y; cell_z[t][an[t]] = z;
                cell_t[an[0] + an[1]] = t;
                cell_fid[an[0] + an[1]][0] = fid[t][0][z][x];
                cell_fid[an[0] + an[1]][1] = fid[t][1][z][y];
                cell_id[t][x][y][z] = an[0] + an[1];
                an[t]++;
            }
        }
    }

    F0(t, 2) F0(x, n) F0(y, n) F0(z, n) if (cell_id[t][x][y][z] >= 0) {
        F0(k, 6) {
            int x2 = x + DX[k], y2 = y + DY[k], z2 = z + DZ[k];
            if (!Inside(x2) || !Inside(y2) || !Inside(z2) || cell_id[t][x2][y2][z2] == -1) {
                nx_cell[cell_id[t][x][y][z]][k] = -1;
            } else {
                nx_cell[cell_id[t][x][y][z]][k] = cell_id[t][x2][y2][z2];
                if (t == 0) {
                    outk[cell_id[t][x][y][z]].push_back(k);
                    outcell[cell_id[t][x][y][z]].push_back(cell_id[t][x2][y2][z2]);
                }
            }
        }
    }

    sunf = unf = fn[0][0] + fn[0][1] + fn[1][0] + fn[1][1];
    vector<double> densities;
    F0(t, 2) F0(side, 2) densities.push_back(1.0 * fn[t][side] / n / n);

    vector<int> c;
    F0(i, 6) c.push_back(i);
    AddOrientation(c);

    F0(o, 24) F0(i, 6) so[i][orientations[o][i]].push_back(o);

    F0(c, C) bscores[c] = inf;

    bscore = 1e18;

    if (DBG) {
        PR(n);
        PR(fn[0][0]+fn[0][1]);
        PR(fn[1][0]+fn[1][1]);
        PR(unf);
        PR(densities);
        cerr << "an[] = " << an[0] << " " << an[1] << endl;
    }
}

void UpdateBest() {
    if (score < bscore) {
        bscore = score;
        F0(x, an[0] + an[1]) {
            bans[x] = ans[x];
        }
    }
}

bool fillx[N], filly[N];
void FillRest() {
    m[0] = m[1] = 0;
    F0(i, an[0] + an[1]) m[i >= an[0]] = max(m[i >= an[0]], ans[i]);
    F0(t, 2) {
        F0(z, n) {
            CL(0, fillx); CL(0, filly);
            F0(x, n) F0(y, n) if (cell_id[t][x][y][z] >= 0 && ans[cell_id[t][x][y][z]] > 0) { fillx[x] = 1; filly[y] = 1; }
            vector<int> vx, vy;
            F0(x, n) if (f[t][0][z][x] && !fillx[x]) vx.push_back(x);
            F0(y, n) if (f[t][1][z][y] && !filly[y]) vy.push_back(y);
            if (vx.empty() ^ vy.empty()) {
                if (vx.empty()) { F0(x, n) if (f[t][0][z][x]) { vx.push_back(x); break; } }
                if (vy.empty()) { F0(y, n) if (f[t][1][z][y]) { vy.push_back(y); break; } }
            }
            F0(i, max(SZ(vx), SZ(vy))) {
                ans[cell_id[t][vx[i < SZ(vx) ? i : 0]][vy[i < SZ(vy) ? i : 0]][z]] = ++m[t];
            }
        }
    }
}

void Fill0(int c) {
    int t = cell_t[c];
    V(c) = 1;
    if (++fc[t][0][cell_fid[c][0]] == 1) {
        maskf[t][0] ^= MaskType(1) << cell_fid[c][0];
    }
    if (++fc[t][1][cell_fid[c][1]] == 1) {
        maskf[t][1] ^= MaskType(1) << cell_fid[c][1];
    }
}

void UnFill0(int c) {
    int t = cell_t[c];
    V(c) = 0;
    if (--fc[t][0][cell_fid[c][0]] == 0) {
        maskf[t][0] ^= MaskType(1) << cell_fid[c][0];
    }
    if (--fc[t][1][cell_fid[c][1]] == 0) {
        maskf[t][1] ^= MaskType(1) << cell_fid[c][1];
    }
}

void Fill(int c) {
    int t = cell_t[c];
    if (++fc[t][0][cell_fid[c][0]] == 1) {
        unf--;
        maskf[t][0] ^= MaskType(1) << cell_fid[c][0];
        value += cell_value[t][0][cell_fid[c][0]];
    }
    if (++fc[t][1][cell_fid[c][1]] == 1) {
        unf--;
        maskf[t][1] ^= MaskType(1) << cell_fid[c][1];
        value += cell_value[t][1][cell_fid[c][1]];
    }
}

void UnFill(int c) {
    int t = cell_t[c];
    if (--fc[t][0][cell_fid[c][0]] == 0) {
        unf++;
        maskf[t][0] ^= MaskType(1) << cell_fid[c][0];
        value -= cell_value[t][0][cell_fid[c][0]];
    }
    if (--fc[t][1][cell_fid[c][1]] == 0) {
        unf++;
        maskf[t][1] ^= MaskType(1) << cell_fid[c][1];
        value -= cell_value[t][1][cell_fid[c][1]];
    }
}

bool VIS[24][1024][1024];
map<MaskType, int> hist3;
vector<int> fcovers[2][2][N*N];
int qn, vz, vis[N*N*N];
int q[N*N*N];
int outd[N*N*N], tree_parent[N*N*N];

void ResetBlock() {
    F0(i, blockn) {
        UnFill0(block[0][i]);
        UnFill0(block[1][i]);
    }
}

struct Block {
    vector<int> block[2];
    MaskType f[2][2];
    int sz, v, o;
    Block() {
    }
    void Coverage() {
        sz = SZ(block[0]);
        v = 0;
        F0(t, 2) F0(side, 2) {
            f[t][side] = maskf[t][side];
            v += bc(f[t][side]);
        }
    }
};

vector<Block> all_blocks;

void AddCovers(const Block& b, int ind) {
    F0(i, b.sz) {
        F0(t, 2) {
            int c = b.block[t][i];
            fcovers[t][0][cell_fid[c][0]].push_back(ind);
            fcovers[t][1][cell_fid[c][1]].push_back(ind);
        }
    }
}

void Erase(vector<int>& v, int value) {
    auto it = find(v.begin(), v.end(), value);
    v.erase(it);
}

void DelCovers(const Block& b, int ind) {
    F0(i, b.sz) {
        F0(t, 2) {
            int c = b.block[t][i];
            Erase(fcovers[t][0][cell_fid[c][0]], ind);
            Erase(fcovers[t][1][cell_fid[c][1]], ind);
        }
    }
}

bool cc(int x, int y) {
    return all_blocks[x].sz > all_blocks[y].sz;
}

inline void SetW(int t, int i, int j) {
    int c = blocks[i][t][j];
    wi[c] = i;
    wj[c] = j;
}

inline void UnSetW(int c) {
    wi[c] = -1;
    wj[c] = -1;
}

void Append(int i, int c0, int c1) {
    V(c0) = i + 1;
    V(c1) = i + 1;
    Fill(c0);
    Fill(c1);
    blocks[i][0].push_back(c0);
    blocks[i][1].push_back(c1);
    SetW(0, i, SZ(blocks[i][0]) - 1);
    SetW(1, i, SZ(blocks[i][1]) - 1);
}

void Remove(int i, int j) {
    F0(t, 2) {
        V(blocks[i][t][j]) = 0;
        UnFill(blocks[i][t][j]);
        UnSetW(blocks[i][t][j]);
        if (j + 1 != SZ(blocks[i][t])) {
            UnSetW(blocks[i][t].back());
            swap(blocks[i][t][j], blocks[i][t].back());
            SetW(t, i, j);
        }
        blocks[i][t].pop_back();
    }
}

void RemovePair(int c2) {
    int pi = wi[c2];
    int pj = wj[c2];
    Remove(pi, pj);
}

int BW;
double border_dist[N*N*4];
int tn;
vector<int> removals;
int pos_parent[MAXB], pos_sz[MAXB];
int qw[N*N*N];
inline int Get(int pos) {
    return pos_parent[pos] == pos ? pos : (pos_parent[pos] = Get(pos_parent[pos]));
}

bool Disconnects(int i, int j) {
    vz++;
    int c0 = blocks[i][0][j];
    int id = 0;
    for (int k : outk[c0]) {
        int c = nx_cell[c0][k];
        if (V(c) != i + 1) continue;
        vis[c] = vz + id;
        q[id++] = c;
    }
    if (id <= 1) { vz++; return false; }
    int edges_left = id - 1;

    qn = id;
    F0(i, id) {
        qw[i] = i; pos_parent[i] = i; pos_sz[i] = 1;
    }
    V(c0) = 0;
    F0(qi, qn) {
        int pos = qw[qi];
        int p1 = Get(pos);
        int cx = q[qi];
        for (int c : outcell[cx]) {
            if (V(c) != i + 1) continue;
            int& v = vis[c];
            if (v != vz + pos) {
                if (v < vz) {
                    v = vz + pos;
                    qw[qn] = pos;
                    q[qn++] = c;
                    pos_sz[p1]++;
                } else {
                    int p2 = Get(v - vz);
                    if (p1 != p2) {
                        if (!--edges_left) {
                            vz += id - 1;
                            V(c0) = i + 1;
                            return false;
                        }
                        pos_parent[p2] = p1;
                        pos_sz[p1] += pos_sz[p2];
                    }
                }
            }
        }
        if (!--pos_sz[p1]) {
            vz += id - 1;
            V(c0) = i + 1;
            return true;
        }
    }
    return true;
}

void ResetBlock(int i) {
    while (!blocks[i][0].empty()) {
        Remove(i, SZ(blocks[i][0]) - 1);
    }
}

void ResetAllBlocks(int sn) {
    F0(i, sn) ResetBlock(i);
}

void AddBlock(int& v, Block& b) {
    if (v) {
       DelCovers(all_blocks[v - 1], v - 1);
       all_blocks[v - 1] = b;
       AddCovers(b, v - 1);
    } else {
        AddCovers(b, SZ(all_blocks));
        all_blocks.push_back(b);
        v = SZ(all_blocks);
    }
}

void BFS(int o, int a0, int a1) {
    VIS[o][a0][a1-an[0]] = true;
    block[0][0] = a0; block[1][0] = a1;
    Fill0(a0); Fill0(a1);
    blockn = 1;
    F0(i, blockn) {
        for (int k : outk[block[0][i]]) {
            block[0][blockn] = nx_cell[block[0][i]][k];
            block[1][blockn] = nx_cell[block[1][i]][orientations[o][k]];
            if (V(block[0][blockn]) == 0 && Empty(block[1][blockn])) {
                VIS[o][block[0][blockn]][block[1][blockn]-an[0]] = true;
                Fill0(block[0][blockn]);
                Fill0(block[1][blockn]);
                blockn++;
            }
        }
    }
    if (blockn <= 2) {
        ResetBlock();
        return;
    }

    MaskType h = 0;
    F0(t, 2) F0(side, 2) {
        h = h * 239102384133377LL + maskf[t][side];
    }
    int& v = hist3[h];
    if (!v || all_blocks[v - 1].sz > blockn) {
        Block b;
        b.block[0].assign(block[0], block[0] + blockn);
        b.block[1].assign(block[1], block[1] + blockn);
        b.o = o;
        b.Coverage();
        AddBlock(v, b);
    }
    ResetBlock();
}

void CreateBlocks() {
    all_blocks.clear();
    hist3.clear();
    F0(t, 2) F0(side, 2) F0(i, n*n) {
        fcovers[t][side][i].clear();
    }

    //PR(an[0]); PR(an[1]);
    int all = 24 * an[0] * an[1];
    int limit = 1 << 22;
    if (DBG) { PR(all); }
    if (all < limit) {
        F0(o, 24) F0(a0, an[0]) F0(a1, an[1]) {
            if (VIS[o][a0][a1]) continue;
            BFS(o, a0, a1 + an[0]);
        }
    } else {
        int steps = 1LL * an[0] * limit / all;
        cerr << steps << "/" << an[1] << endl;
        F0(o, 24) F0(a1, an[1]) {
            int a0 = my.nextInt(an[0]);
            if (VIS[o][a0][a1]) continue;
            BFS(o, a0, a1 + an[0]);
        }
    }

    if (DBG) {
        PR(SZ(all_blocks));
    }

    F0(t, 2) F0(side, 2) {
        F0(i, fn[t][side]) sort(fcovers[t][side][i].begin(), fcovers[t][side][i].end(), cc);
    }
}

void StoreState(int c, int sn) {
    if (score < bscores[c]) {
        bscores[c] = score;
        bsn[c] = sn;
        F0(i, sn) {
            bos[c][i] = os[i];
            F0(t, 2) bblocks[c][i][t] = blocks[i][t];
        }
    }
}

void Rebuild(int sn)  {
    int sz = SZ(blocks[sn][0]);
    int j = my.nextInt(sz);
    int o = os[sn];
    F0(t, 2) nx[t] = blocks[sn][t][j];
    ResetBlock(sn);
    Append(sn, nx[0], nx[1]);
    tn = 1;
    outd[0] = 0;
    F0(ti, tn) {
        RS(outk[blocks[sn][0][ti]]);
        for (int k : outk[blocks[sn][0][ti]]) {
            nx[0] = nx_cell[blocks[sn][0][ti]][k];
            nx[1] = nx_cell[blocks[sn][1][ti]][orientations[o][k]];
            if (nx[1] == -1) continue;
            if (V(nx[0]) || V(nx[1])) {
                continue;
            }
            tree_parent[tn] = ti;
            outd[ti]++;
            Append(sn, nx[0], nx[1]);
            outd[tn++] = 0;
        }
    }
    for (int i = tn - 1; i >= 1; i--) if (!outd[i]) {
        bool covers = false;
        F0(t, 2) {
            int c = blocks[sn][t][i];
            if (fc[t][0][cell_fid[c][0]] == 1 || fc[t][1][cell_fid[c][1]] == 1) {
                covers = true;
                break;
            }
        }
        if (covers) continue;
        outd[tree_parent[i]]--;
        Remove(sn, i);
    }
}

void Big(int c) {
    vector<int> v;
    F0(i, sn) v.push_back(i);
    RS(v);
    v.resize(my.nextInt(sn + 1));
    int its = 4 * (an[0] + an[1]);

    for (int i : v) {
        Rebuild(i);
        if (Elapsed() > TL3) return;
    }

    F0(it, its) {
        if ((it & 127) == 127 && Elapsed() > TL3) return;
        // expand block by one cell
        // remove those cells from others
        int i = my.nextInt(sn);
        int i2 = my.nextInt(sn);
        if (SZ(blocks[i2][0]) < SZ(blocks[i][0])) i = i2;

        int sz = SZ(blocks[i][0]);
        int j = my.nextInt(sz);
        int cx = blocks[i][0][j];
        int k = outk[cx][my.nextInt(SZ(outk[cx]))];

        if (sz == 1) os[i] = my.nextInt(24);
        else if (sz == 2) {
            int d0 = GetDir(blocks[i][0][0], blocks[i][0][1]);
            int d1 = GetDir(blocks[i][1][0], blocks[i][1][1]);
            os[i] = so[d0][d1][my.nextInt(4)];
        } else if (sz <= n) {
            // straight test
            v.clear();
            F0(j, sz) v.push_back(blocks[i][0][j]);
            sort(v.begin(), v.end());
            int dir = -1;
            bool straight = true;
            F0(i, sz - 1) {
                int ndir = GetDir(v[i], v[i + 1]);
                if (i == 0) dir = ndir;
                else if (dir != ndir) { straight = false; break; }
            }
            if (straight) {
                os[i] = so[dir][orientations[os[i]][dir]][my.nextInt(4)];
            }
        }

        nx[0] = nx_cell[blocks[i][0][j]][k];
        nx[1] = nx_cell[blocks[i][1][j]][orientations[os[i]][k]];
        if (nx[1] == -1) continue;

        // free, good to go
        if (!V(nx[0]) && !V(nx[1])) {
            Append(i, nx[0], nx[1]);
            score = 0.0; F0(i, sn) score += 1.0 / SZ(blocks[i][0]);
            StoreState(c, sn);
            if (score < bscore) {
                UpdateBest();
            }
            continue;
        }
        // for now only removing one
        if (V(nx[0]) && V(nx[1])) {
            continue;
        }

        F0(t, 2) if (V(nx[t])) {
            int pi = wi[nx[t]];
            int pj = wj[nx[t]];
            if (pi == i || SZ(blocks[pi][0]) == 1 || Disconnects(pi, pj)) {
                continue;
            }
            int old_c0 = blocks[pi][0][pj], old_c1 = blocks[pi][1][pj];
            Remove(pi, pj);
            Append(i, nx[0], nx[1]);
            if (unf) {
                Remove(i, SZ(blocks[i][0]) - 1);
                Append(pi, old_c0, old_c1);
                continue;
            }
            score = 0.0; F0(i, sn) score += 1.0 / SZ(blocks[i][0]);
            StoreState(c, sn);
            if (score < bscore) {
                //cerr << "R: " << sz << " " << score << endl;
                UpdateBest();
            }
        }
    }
}

void BoostScore(int sn) {
    vector<int> ord, start_sz, sz;
    F0(i, sn) {
        ord.push_back(i);
        start_sz.push_back(SZ(blocks[i][0]));
    }
    sz = start_sz;
    sort(ord.begin(), ord.end(), [&](int x, int y) {
        return sz[x] < sz[y];
    });

    F0(ii, sn) {
        int i = ord[ii];
        int o = os[i];
        F0(qi, sz[i]) {
            //RS(outk[blocks[i][0][qi]]);
            for (int k : outk[blocks[i][0][qi]]) {
                int c = nx_cell[blocks[i][0][qi]][k];
                if (!Empty(c)) continue;
                int c2 = nx_cell[blocks[i][1][qi]][orientations[o][k]];
                if (!Empty(c2)) continue;
                V(c) = i + 1;
                V(c2) = i + 1;
                blocks[i][0].push_back(c);
                blocks[i][1].push_back(c2);
                sz[i]++;
            }
        }
    }
    score = 0;
    F0(i, sn) score += 1.0 / sz[i];
    if (score < bscore) {
        //double opt = 0.0; int on = sn;
        //F0(i, on) opt += 1.0 / ((room / on) + (i < (room % on)));
        //cerr << "UB: " << opt << " >=" << room / on << " "; cerr << sz << " "; PR(score);
        UpdateBest();
    }

    int c = 0;
    F0(i, C) if (bscores[i] == score) c++;
    if (!c) {
        F0(i, C) if (bscores[i] > bscores[c]) c = i;
        StoreState(c, sn);
    }

    F0(i, sn) {
        while (sz[i] > start_sz[i]) {
            sz[i]--;
            F0(t, 2) {
                V(blocks[i][t].back()) = 0;
                blocks[i][t].pop_back();
            }
        }
    }
}

typedef double EvalType;
struct BlockSet {
    int* last_block; int last_block_n;
    int* last_removals; int last_removals_n;
    int bid;
    int last_index;
    int sz;
    EvalType eval;
};

const int P11 = (1<<11) - 1;
int BlockID(int x, int y) {
    return (x << 11) | y;
}
int RemovalID(int x, int y, int z) {
    return (x << 22) | (y << 11) | z;
}

void GrowNext(int bid, int bi) {
    removals.clear();
    Block& b = all_blocks[bid];
    F0(t, 2) nx[t] = b.block[t][bi];
    Append(sn, nx[0], nx[1]);
    tn = 1;
    outd[0] = 0;
    F0(ti, tn) {
        for (int k : outk[blocks[sn][0][ti]]) {
            nx[0] = nx_cell[blocks[sn][0][ti]][k];
            nx[1] = nx_cell[blocks[sn][1][ti]][orientations[b.o][k]];
            if (nx[1] == -1) continue;

            if (V(nx[0]) && V(nx[1])) {
                continue;
            }
            if (V(nx[0]) || V(nx[1])) {
                int t = V(nx[0]) ? 0 : 1;
                int pi = wi[nx[t]];
                int pj = wj[nx[t]];
                UnFill(blocks[pi][1-t][pj]);
                int new_unf = unf;
                Fill(blocks[pi][1-t][pj]);
                if (SZ(blocks[pi][0]) > 1 && unf == new_unf && !Disconnects(pi, pj)) {
                    removals.push_back(RemovalID(pi, blocks[pi][0][pj], blocks[pi][1][pj]));
                    Remove(pi, pj);
                } else continue;
            }
            tree_parent[tn] = ti;
            outd[ti]++;
            Append(sn, nx[0], nx[1]);
            outd[tn++] = 0;
        }
    }

    for (int i = tn - 1; i >= 0; i--) if (!outd[i]) {
        bool covers = false;
        int x = 0;
        F0(t, 2) {
            int c = blocks[sn][t][i];
            if (fc[t][0][cell_fid[c][0]] == 1 || fc[t][1][cell_fid[c][1]] == 1) {
                covers = true;
                break;
            } else x += fc[t][0][cell_fid[c][0]] + fc[t][1][cell_fid[c][1]] - 4;
        }
        if (covers || x <= tn/2) continue;
        outd[tree_parent[i]]--;
        Remove(sn, i);
    }
}

const int BB = 1 << 20;
BlockSet bb[BB];
const int BUFF = 1 << 23;
int buff[BUFF], buff_pos;
int bn;
set<pair<EvalType, int>> pq[MAXB+1];
unordered_map<MaskType, int> H;
BlockSet ns;
int maxT;
vector<int> frs;

void Recreate(int level, int lindex) {
    if (level > 1) {
        Recreate(level - 1, bb[lindex].last_index);
    }
    F0(i, bb[lindex].last_removals_n) {
        RemovePair(bb[lindex].last_removals[i] & P11);
    }
    F0(j, bb[lindex].last_block_n) {
        Append(level - 1, bb[lindex].last_block[j] >> 11,
                          bb[lindex].last_block[j] & P11);
    }
}

int BC;
void BeamStep() {
    vector<pii> v;
    F0(level, maxT) {
        F0(e, BW) {
            if (pq[level].empty()) break;
            int lindex = pq[level].begin()->second;
            pq[level].erase(pq[level].begin());

            BlockSet s = bb[lindex];

            sn = level;

            if (level > 0) Recreate(level, lindex);

            MaskType h = 0;
            F0(t, 2) F0(side, 2) {
                h = h * 239102384133377LL + maskf[t][side];
            }
            //if (e == 0) cerr << level << " " << unf << "/" << sunf << endl;

            if (level == 0 || H[h] == s.sz) {
                int ii = 0;
                for (; ii < SZ(frs); ii++) {
                    int pid = frs[ii] >> 2, side = (frs[ii] & 2) >> 1, t = frs[ii] & 1;
                    if ((MaskType(1) << pid) & maskf[t][side]) continue;
                    break;
                }
                int pid = frs[ii] >> 2, side = (frs[ii] & 2) >> 1, t = frs[ii] & 1;
                int sz = SZ(fcovers[t][side][pid]);

                int left = 300;

                v.clear();
                sz = min(sz, left * 10);
                F0(cover_index, sz) {
                    int bid = fcovers[t][side][pid][cover_index];
                    Block& b = all_blocks[bid];
                    int cv = 0;
                    F0(t, 2) F0(side, 2) cv += bc(b.f[t][side] | maskf[t][side]);
                    v.push_back(pii(-(cv*100+b.sz), cover_index));
                }
                sort(v.begin(), v.end());

                for (pii w : v) {
                    int cover_index = w.second;
                    int bid = fcovers[t][side][pid][cover_index];
                    Block& b = all_blocks[bid];
                    int bx = my.nextInt(b.sz);
                    F0(iter, b.sz) {
                        int c0 = b.block[0][bx];
                        int c1 = b.block[1][bx];
                        int c = b.block[t][bx];
                        if (V(c0) || V(c1) || cell_fid[c][side] != pid) {
                            if (++bx == b.sz) bx = 0;
                            continue;
                        }

                        GrowNext(bid, bx);

                        int covers = sunf - unf;

                        if (unf == 0) {
                            maxT = min(maxT, level + 1);
                            os[level] = b.o;
                            int cindex = lindex;
                            for (int i = level; i >= 1; i--) {
                                os[i - 1] = all_blocks[bb[cindex].bid].o;
                                cindex = bb[cindex].last_index;
                            }
                            BoostScore(sn + 1);
                        } else {
                            MaskType h = 0;
                            F0(t, 2) F0(side, 2) {
                                h = h * 239102384133377LL + maskf[t][side];
                            }
                            int new_sz = s.sz + SZ(blocks[sn][0]) - SZ(removals);
                            EvalType eval = -covers-value;
                            if ((SZ(pq[level + 1]) < BC || (--pq[level + 1].end())->first > eval) &&
                                (!H.count(h) || H[h] > new_sz)) {
                                ns = bb[lindex];
                                ns.bid = bid;
                                ns.sz = new_sz;
                                ns.last_index = lindex;

                                ns.last_removals_n = SZ(removals);
                                ns.last_removals = buff + buff_pos;
                                F0(i, ns.last_removals_n) {
                                    ns.last_removals[i] = removals[i];
                                }
                                buff_pos += ns.last_removals_n;

                                ns.last_block_n = SZ(blocks[sn][0]);
                                ns.last_block = buff + buff_pos;
                                F0(i, SZ(blocks[sn][0])) {
                                    ns.last_block[i] = BlockID(blocks[sn][0][i], blocks[sn][1][i]);
                                }
                                buff_pos += ns.last_block_n;

                                ns.eval = eval;
                                H[h] = new_sz;
                                pq[level + 1].insert(make_pair(ns.eval, bn));
                                if (SZ(pq[level + 1]) > BC) {
                                    pq[level + 1].erase(--pq[level + 1].end());
                                }

                                bb[bn++] = ns;
                            }
                        }

                        ResetBlock(sn);
                        for (auto p : removals) {
                            Append(p >> 22, (p >> 11) & P11, p & P11);
                        }

                        break;
                    }

                    if (--left <= 0) break;
                }
            }
            for (int i = level; i >= 1; i--) {
                ResetBlock(i - 1);
            }
        }
    }
}

void BeamSearch() {
    maxT = MAXB;
    BC = 2000000 / n / n / n;
    frs.clear();
    F0(t, 2) F0(side, 2) F0(i, n) F0(j, n) {
        if (f[t][side][i][j]) {
            frs.push_back((fid[t][side][i][j] << 2) | (side << 1) | t);
            border_dist[(fid[t][side][i][j] << 2) | (side << 1) | t] = -cell_value[t][side][fid[t][side][i][j]] * pow(fn[t][side], 3.0);
        }
    }

    sort(frs.begin(), frs.end(), [&](int x, int y) {
        return pii(border_dist[x], y) < pii(border_dist[y], x);
    });

    bn = 0;

    BlockSet s0;
    s0.sz = s0.eval = 0;
    s0.bid = s0.last_index = -1;
    pq[0].insert(make_pair(0, 0));

    bb[bn++] = s0;

    int BEAM_STEPS = 0;
    while (Elapsed() < TL) {
        BW = 1;
        BEAM_STEPS += BW;
        BeamStep();
    }
    PR(BEAM_STEPS);
    PR(bn);
    //if (bn > BB / 4) throw;
    PR(buff_pos);
    //if (buff_pos > BUFF / 4) throw;

    if (DBG) {
        F0(i, MAXB) {
            if (SZ(pq[i])) cerr << i << " " << SZ(pq[i]) << endl;
        }
    }
}

void PostOptimization() {
    F0(i, MAXB + 1) pq[i].clear();
    all_blocks.clear();
    while (1) {
        if (Elapsed() > TL3) return;
        F0(c, C) if (bscores[c] < inf) {
            if (Elapsed() > TL3) return;
            sn = bsn[c];
            F0(i, sn) F0(j, SZ(bblocks[c][i][0])) Append(i, bblocks[c][i][0][j], bblocks[c][i][1][j]);
            F0(i, sn) os[i] = bos[c][i];
            Big(c);
            ResetAllBlocks(sn);
        }
    }
}

double freq[2][2][N*N];
void Solve() {
    Prepare();

    CreateBlocks();

    for (const Block& b : all_blocks) {
        int bcs = b.v;
        F0(t, 2) F0(side, 2) {
            F0(i, fn[t][side]) if ((MaskType(1) << i) & b.f[t][side]) freq[t][side][i] += bcs;
        }
    }

    F0(t, 2) F0(side, 2) {
        double s = 0;
        F0(i, fn[t][side]) {
            if (!freq[t][side][i]) continue;
            double v = pow(1.0 / freq[t][side][i], 2.5);
            s += v;
        }

        F0(i, fn[t][side]) {
            if (!freq[t][side][i]) continue;
            double v = pow(1.0 / freq[t][side][i], 2.5);
            cell_value[t][side][i] = 3 * fn[t][side] * v / s;
        }
    }

    if (DBG) {
        PR(Elapsed());
    }

    BeamSearch();

    PostOptimization();

    if (bscore > inf) {
        FillRest();
        UpdateBest();
    }

    int bm = 0;
    F0(i, an[0] + an[1]) bm = max(bm, bans[i]);
    cout << bm << endl;
    F0(t, 2) {
        F0(i, n) F0(j, n) F0(k, n) {
            if (cell_id[t][i][j][k] == -1) cout << 0 << " ";
            else cout << bans[cell_id[t][i][j][k]] << " ";
        }
        cout << endl;
    }
    if (DBG) PR(bscore);
    //Report();
}





void ReadInput() {
    cin >> n;
    string s;
    F0(t, 2) F0(side, 2) {
        F0(i, n) { cin >> s; F0(j, n) f[t][side][i][j] = s[j] == '1'; }
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
        if(seed>=0 && seed<5000) {
            char inp[128];
            sprintf(inp, "in/%04d.txt", seed);
            char outp[128];
            sprintf(outp, "out/%04d.txt", seed);
            ignore = freopen(inp, "r", stdin);
            ignore = freopen(outp, "w", stdout);
            ReadInput();
            cerr << "Seed #" << seed << " ";
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
