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
    return GetTSC() / 2.9e9;
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
Rand my(2020);
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
const int N = 72;
const int LOGN = 1 << 16;
double logs[LOGN];
enum { UP, RIGHT, DOWN, LEFT };
const int DX[]={-1,0,1,0};
const int DY[]={0,1,0,-1};
const string DS="URDL";

int n, m, si, sj;
ll bscore, score;
int a[N][N], c[N][N], cn;
int d[1024][1024], pc[1024][1024], o[1024], w[1024], b[1024], hn, ec[1024], wneed[1024], wn, newj[1024], jun;
int covers[1024], active[1024];
vector<int> f[1024];
vector<int> h[1024];
vector<int> bo;
vector<pii> adj[1024];
vector<pii> crosses;

int d1(int x, int y) { return abs(x-y); }
int d1(pii p1, pii p2) { return d1(p1.first,p2.first)+d1(p1.second,p2.second); }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
bool Good(pii p) { return In(p) && a[p.first][p.second] != -1; }
int GetDir(pii p1, pii p2) {
    F0(k, 4) if (d1(p1, p2) > d1(Move(p1, k), p2)) return k;
    return -1;
}
int sqr(int x) { return x*x; }

void Prepare() {
    bscore=inf;
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
}

void UpdateBest() {
    if (score < bscore) {
        bscore=score;
        bo = vector<int>(o, o + m + 1);
    }
}

void Solve() {
    Init();
    Prepare();
    my.reseed(2020);

    CL(-1, c); crosses.clear();
    c[si][sj] = 0;
    PR(Good(pii(si,sj)));
    crosses.push_back(pii(si, sj));
    F0(i, n) F0(j, n) if (a[i][j] != -1 && c[i][j] == -1) {
        pii p(i, j);
        int mask=0;
        F0(k, 4) {
            pii p2 = Move(p, k);
            if (Good(p2)) {
                mask|=(1<<(k%2));
            }
        }
        if(mask==3) {
            c[i][j] = SZ(crosses);
            crosses.push_back(pii(i,j));
        }
    }
    cn = SZ(crosses);
    PR(cn);
    F0(i, cn) {
        b[i] = a[crosses[i].first][crosses[i].second];
        adj[i].clear();
        F0(k, 4) {
            pii p = crosses[i];
            int dist = 0;
            while (1) {
                p = Move(p, k);
                if (!Good(p)) break;
                dist += a[p.first][p.second];
                if (c[p.first][p.second] != -1) {
                    int j = c[p.first][p.second];
                    adj[i].push_back(pii(j, dist));
                    break;
                }
            }
        }
    }
    F0(start, cn) {
        if (start == cn-1) {
            PR(start);
        }
        F0(j, cn) d[start][j] = -1;
        d[start][start] = 0;
        set<pii> S;
        S.insert(pii(0, start));
        while (!S.empty()) {
            int i = S.begin()->second;
            int dist = S.begin()->first;
            S.erase(S.begin());
            for (pii p : adj[i]) {
                int ndist = dist + p.second;
                int j = p.first;
                if (d[start][j] > ndist || d[start][j] == -1) {
                    d[start][j] = ndist;
                    pc[start][j] = i;
                    S.insert(pii(ndist, j));
                }
            }
        }
    }
    F0(i, cn) f[i].clear();

    hn = 0;
    for (int x = 0; x < n; x += 2) {
        int y = 0;
        while (1) {
            int bad = 0;
            while (y < n && a[x][y] == -1) y++;
            if (y == n) break;
            h[hn].clear();
            while (y < n && a[x][y] != -1) {
                int z = c[x][y];
                if (z != -1) {
                    if (z == 0) bad = 1;
                    h[hn].push_back(z);
                }
                y++;
            }
            if (!bad && !h[hn].empty()) {
                for (int z : h[hn]) f[z].push_back(hn);
                hn++;
            }
        }
    }
    for (int y = 0; y < n; y += 2) {
        int x = 0;
        while (1) {
            int bad = 0;
            while (x < n && a[x][y] == -1) x++;
            if (x == n) break;
            h[hn].clear();
            while (x < n && a[x][y] != -1) {
                int z = c[x][y];
                if (z != -1) {
                    if (z == 0) bad = 1;
                    h[hn].push_back(z);
                }
                x++;
            }
            if (!bad && !h[hn].empty()) {
                for (int z : h[hn]) f[z].push_back(hn);
                hn++;
            }
        }
    }
    PR(hn);

    double SA_START = Elapsed(), SA_END = 3.0;

int NUMST = 100;
F1(ST, NUMST) {

    F0(i, hn) covers[i] = 0;
    F0(i, cn) {
        active[i] = 1;
        for (int j : f[i]) covers[j]++;
    }

    vector<int> rs;
    F1(i, cn - 1) rs.push_back(i);
    random_shuffle(rs.begin(), rs.end());
    for (int i : rs) {
        if (!active[i]) continue;
        int lastc = 0;
        for (int j : f[i]) if (covers[j] == 1) lastc = 1;
        if (!lastc) {
            active[i] = 0;
            for (int j : f[i]) covers[j]--;
        }
    }

    score = 0;
    m = 0;
    F0(i, cn) if (active[i]) {
        o[m] = i;
        w[i] = m++;
    }
    o[m] = 0;
    F0(i, m) score += d[o[i]][o[i+1]];

    int itc=1000000000;
    int tot=1,succ=0, tot2=1, succ2=0;
    double T1 = 0.0, T2 = 15.0, T = 0.0;
    // Tweak
    double startTime = Elapsed(), endTime = SA_START + (SA_END - SA_START) * ST / NUMST;
    F0(iter, itc) {
        if ((iter & 255) == 0) {
            double r = (endTime - Elapsed()) / (endTime - startTime);
            if (r <= 0) {
                break;
            }
            //PR(iter);
            T = T1 + (T2 - T1) * r;
        }

        double maxd = T * logs[my.nextInt(LOGN)];

        if (my.nextInt(4)) {
            int i = my.nextInt(m - 1) + 1;
            int j = my.nextInt(m - 1) + 1;
            if (i == j) continue;
            if (i > j) swap(i, j);
            int delta = d[o[i-1]][o[j]] + d[o[i]][o[j+1]] - d[o[i-1]][o[i]] - d[o[j]][o[j+1]];
            delta += b[o[i]] - b[o[j]];
            tot++;
            if(delta<=0 || delta<=2*maxd) {
                succ++;
                score += delta;
                reverse(o + i, o + j + 1);
                UpdateBest();
            }
        } else {
            int pos = my.nextInt(m - 1) + 1;
            int i = o[pos];
            wn = 0;
            for (int z : f[i]) {
                if (covers[z] == 1) wneed[wn++] = z;
                ec[z] = 0;
            }
            random_shuffle(wneed, wneed + wn);
            jun = 0;
            F0(k, wn) {
                int z = wneed[k];
                if (ec[z]) continue;
                int y = h[z][my.nextInt(SZ(h[z]))];
                newj[jun++] = y;
                for (int z : f[y]) ec[z]=1;
            }
            if (jun > 2) continue;
            if (jun == 1 && newj[0] == i) continue;
            if (jun == 0) {
                int delta = d[o[pos-1]][o[pos+1]] - (d[o[pos-1]][i] + d[i][o[pos+1]]);
                score += delta;
                for (int z : f[i]) covers[z]--;
                active[i] = 0;
                for (int k = pos; k < m; k++) {
                    o[k] = o[k + 1];
                }
                m--;
                UpdateBest();
                continue;
            }
            if (jun == 2) {
                int e1 = newj[0], e2 = newj[1];
                if (d[o[pos-1]][e1] + d[e1][e2] + d[e2][o[pos+1]] > d[o[pos-1]][e2] + d[e2][e1] + d[e1][o[pos+1]]) {
                    swap(e1, e2);
                }
                int delta = d[o[pos-1]][e1] + d[e1][e2] + d[e2][o[pos+1]] - (d[o[pos-1]][i] + d[i][o[pos+1]]);
                if(delta<=0 || delta<=maxd) {
                    score += delta;
                    for (int i = m+1; i >= pos+2; i--) {
                        o[i] = o[i-1];
                    }
                    o[pos]=e1;
                    o[pos+1]=e2;
                    active[i] = 0;
                    active[e1] = 1;
                    active[e2] = 1;
                    for (int z : f[i]) covers[z]--;
                    for (int z : f[e1]) covers[z]++;
                    for (int z : f[e2]) covers[z]++;
                    m++;
                    UpdateBest();
                }
                continue;
            }
            int e = newj[0];
            int delta = d[o[pos-1]][e] + d[e][o[pos+1]] - (d[o[pos-1]][i] + d[i][o[pos+1]]);
            tot2++;
            if(delta<=0 || delta<=maxd) {
                succ2++;
                score += delta;
                o[pos] = e;
                active[i] = 0;
                active[e] = 1;
                for (int z : f[i]) covers[z]--;
                for (int z : f[e]) covers[z]++;
                UpdateBest();
            }
        }
    }

    //PR(score);
    int ascore = 0; F0(i, m) ascore += d[o[i]][o[i+1]]; if (ascore != score) {
        PR(ascore);
        PR(score);
        throw;
    }

    //cerr<<succ<<"/"<<tot-1<<" " << 100.0*succ/tot<<endl;
    //cerr<<succ<<"/"<<tot2-1<<" " << 100.0*succ2/tot2<<endl;
    UpdateBest();
}

    //Report();
    if(STANDARD) {
        F0(k, SZ(bo)-1) {
            int i = bo[k], j = bo[k + 1];
            vector<int> path;
            while (i != j) {
                int pj = pc[i][j];
                int dist = d1(crosses[pj], crosses[j]);
                int dir = GetDir(crosses[pj], crosses[j]);
                F0(j, dist) path.push_back(dir);
                j = pj;
            }
            reverse(path.begin(), path.end());
            for (int dir : path) cout << DS[dir];
        }
        cout << endl;
    }
}

void ReadInput() {
    cin>>n >> si >> sj;
    F0(i,n) {
        string s;
        cin >> s;
        F0(j,n) if (s[j]=='#') a[i][j]=-1; else a[i][j]=s[j]-'0';
    }
}

void Eval() {
    int act_score=0;
    act_score = (1e4 + 1e7 * n / bscore) + 0.5;
    act_score=12;
    cout << bscore << " " << act_score << endl;
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
        if(seed>=0 && seed<100) {
            char inp[128];
            sprintf(inp, "in/%04d.txt", seed);
            ignore = freopen(inp, "r", stdin);
            ReadInput();
        } else {
            // Generate
            Rand rnd(seed);
        }
        Solve();
        Eval();
    }

    return 0;
}
