double TL = 1.8;
//#define TIME_BASED
//#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops"
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
#include <bitset>

int STANDARD = 1;
using namespace std;
#define F0(i,n) for (int i=0; i<n; i++)
#define F1(i,n) for (int i=1; i<=n; i++)
#define CL(a,x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())
const int inf = 1000000000;
const double pi = acos(-1.0);
typedef pair<int, int> pii;
typedef long long ll;
typedef unsigned long long ull;
const double EPS = 1e-9;
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
void print_all() { cerr << endl; }
template <typename H, typename... T>
void print_all(H head, T... tail) { cerr << " " << head; print_all(tail...); }
#ifdef LOCAL
#define PR(...) cerr << #__VA_ARGS__ << " =", print_all(__VA_ARGS__)
#else
#define PR(...)
#endif

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
     double ret = chrono::duration_cast<std::chrono::nanoseconds>(current_time - init_time).count();
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
    inline double nextDouble() { return rand() * 1.0 / MAX_RAND; }
    inline double nextDouble(double L, double R) { return L + (R - L) * nextDouble(); }
    template<class T> auto REL(T& v) { return v[nextInt(SZ(v))]; }
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

const int N = 32;
const int n = 30;
const int m = 10;
const int k = 10;
const int cn = 900;

enum { UP, RIGHT, DOWN, LEFT, STAY };
const int DX[]={-1,0,1,0,0};
const int DY[]={0,1,0,-1,0};
const string DS="URDLS";
int qx[N*N], qy[N*N], id[N][N];
int ver[N][N], hor[N][N];
int mapping[N][N], bmapping[N][N];
vector<int> commands, bcommands;
vector<int> adj[N*N];
int nx[1024][5];
int d[N*N][N*N], way[N*N][N*N];
int robot[N], a[N], b[N], painted[N*N];

ll bscore, score;
vector<pii> bsol, sol;

const int LOGN = 1 << 16;
double logs[LOGN];

template<class T> T sqr(T x) { return x*x; }

void BuildGraph() {
    F0(i, cn) adj[i].clear();
    F0(i, cn) F0(dir, 5) nx[i][dir] = i;
    F0(i, cn) {
        if (qx[i] + 1 < n && !hor[qx[i]][qy[i]]) {
            int i2 = id[qx[i] + 1][qy[i]];
            adj[i].push_back(i2);
            adj[i2].push_back(i);
            nx[i][DOWN] = i2;
            nx[i2][UP] = i;
        }
        if (qy[i] + 1 < n && !ver[qx[i]][qy[i]]) {
            int i2 = id[qx[i]][qy[i] + 1];
            adj[i].push_back(i2);
            adj[i2].push_back(i);
            nx[i][RIGHT] = i2;
            nx[i2][LEFT] = i;
        }
    }
}

void Distances() {
    F0(start, cn) {
        vector<int> q;
        q.push_back(start);
        F0(i, cn) d[start][i] = -1;
        d[start][start] = 0;
        F0(qi, SZ(q)) {
            int i = q[qi];
            for (int j : adj[i]) if (d[start][j] == -1) {
                d[start][j] = d[start][i] + 1;
                way[start][j] = i;
                q.push_back(j);
            }
        }
    }
}

void PrepareGrid() {
    int tmp = 0;
    F0(i, n) F0(j, n) {
        id[i][j] = tmp;
        qx[tmp] = i;
        qy[tmp] = j;
        tmp++;
    }

    F0(i, m) {
        int i0, j0;
        cin >> i0 >> j0;
        robot[i] = id[i0][j0];
    }
    string s;
    F0(i, n) {
        cin >> s;
        F0(j, n - 1) {
            ver[i][j] = s[j] - '0';
        }
    }
    F0(i, n - 1) {
        cin >> s;
        F0(j, n) {
            hor[i][j] = s[j] - '0';
        }
    }

    BuildGraph();

    Distances();
}

void Prepare() {
    PR(n, m, k);
    bscore = inf;
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
    PrepareGrid();
}

void UpdateBest() {
    if (SZ(commands) < bscore) {
        bscore = SZ(commands);
        bcommands = commands;
        F0(i, m) F0(j, m) bmapping[i][j] = mapping[i][j];
    }
}

int Naive() {
    F0(i, m) a[i] = robot[i];

    F0(i, cn) painted[i] = 0;
    F0(i, m) painted[a[i]] = 1;

    for (int x : commands) {
        F0(i, m) {
            a[i] = nx[a[i]][mapping[x][i]];
            painted[a[i]] = 1;
        }
    }
    int ret = cn;
    F0(i, cn) if (!painted[i]) ret--;

    if (ret == n*n) ret = 3*n*n - SZ(commands);

    return ret;
}

vector<int> pattern;
vector<int> covers[N*N];
vector<vector<int>> paths;
vector<pii> codes;
int dir[4];
int pid[10], pn, bpid[10];
int cnt[900];
int sumd, bsumd;

int F(int x, int y) {
    return d[x][y];
    //return d[x][y] * d[x][y];
}

void ImproveSum() {
    int tot = 1, acc = 0;
    double T2 = 2.00, T1 = 0.00, TEMP = 0.0, r = 0.0;
    int itc = 50000;

    bsumd = sumd;
    F0(i, m) bpid[i] = pid[i];
    vector<int> dels;
#ifdef TIME_BASED
    double startTime = Elapsed();
    double endTime = TL; itc = 1000000000;
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

        int i = my.nextInt(m);

        if (my.nextInt(2)) {
            int j = my.nextInt(m);
            if (i != j) {
                int sd = 0;
                sd -= F(robot[i], codes[pid[i]].first);
                sd -= F(robot[j], codes[pid[j]].first);

                sd += F(robot[i], codes[pid[j]].first);
                sd += F(robot[j], codes[pid[i]].first);

                if (sd <= 0 || sd <= TEMP * logs[my.nextInt(LOGN)]) {
                    sumd += sd;
                    swap(pid[i], pid[j]);
                    if (sumd < bsumd) {
                        bsumd = sumd;
                        F0(i, m) bpid[i] = pid[i];
                    }
                }
                continue;
            }
        }

        int old = pid[i];
        int score_diff = 0;

        dels.clear();

        for (int x : paths[pid[i]]) {
            if (cnt[x] == 1) {
                score_diff--;
                dels.push_back(x);
            }
            cnt[x]--;
        }

        if (dels.empty()) {
            for (int x : paths[pid[i]]) {
                cnt[x]++;
            }
            continue;
        }

        int sum_diff = -F(robot[i], codes[pid[i]].first);

        int cell_id = my.REL(dels);

        int pindex = my.REL(covers[cell_id]);
        pid[i] = pindex;

        for (int x : paths[pid[i]]) {
            if (cnt[x] == 0) {
                score_diff++;
            }
            cnt[x]++;
        }

        if (score_diff != 0) {
            for (int x : paths[pid[i]]) {
                cnt[x]--;
            }

            pid[i] = old;

            for (int x : paths[pid[i]]) {
                cnt[x]++;
            }
            continue;
        }

        sum_diff += F(robot[i], codes[pid[i]].first);

        double delta = sum_diff;

        if (delta > 0)
        tot++;
        if (delta <= 0 || delta <= TEMP * logs[my.nextInt(LOGN)]) {
            if (delta > 0)
            acc++;

            sumd += sum_diff;

            if (sumd < bsumd) {
                bsumd = sumd;
                F0(i, m) bpid[i] = pid[i];
            }
        } else {
            for (int x : paths[pid[i]]) {
                cnt[x]--;
            }

            pid[i] = old;

            for (int x : paths[pid[i]]) {
                cnt[x]++;
            }
        }
    }
    F0(i, m) pid[i] = bpid[i];
    sumd = bsumd;
    if (1) {
        cerr << acc << "/" << tot << " " << 100.0 * acc / tot << " " << sumd << "/" << sumd << endl;
    }
    PR("after opt:", sumd);
}

void Improve() {
    pn = SZ(paths);
    score = 0;
    CL(0, cnt);
    F0(i, m) {
        pid[i] = my.nextInt(pn);
        for (int x : paths[pid[i]]) {
            if (!cnt[x]) {
                score++;
            }
            cnt[x]++;
        }
    }
    PR(score);

    int tot = 1, acc = 0;
    double T2 = 10.00, T1 = 0.00, TEMP = 0.0, r = 0.0;
    int itc = 100000;
#ifdef TIME_BASED
    double startTime = Elapsed();
    double endTime = TL; itc = 1000000000;
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


        int score_diff = 0;

        if (score == cn) break;
        int cell_id = my.nextInt(cn);
        while (cnt[cell_id]) cell_id = my.nextInt(cn);

        assert(!covers[cell_id].empty());
        int pindex = my.REL(covers[cell_id]);
        int i = my.nextInt(m);
        int old = pid[i];

        for (int x : paths[pid[i]]) {
            if (cnt[x] == 1) {
                score_diff--;
            }
            cnt[x]--;
        }

        pid[i] = pindex;

        for (int x : paths[pid[i]]) {
            if (cnt[x] == 0) {
                score_diff++;
            }
            cnt[x]++;
        }
        assert(cnt[cell_id]);

        double delta = -score_diff;

        if (delta > 0)
        tot++;
        if (delta <= 0 || delta <= TEMP * logs[my.nextInt(LOGN)]) {
            if (delta > 0)
            acc++;

            score += score_diff;
        } else {
            for (int x : paths[pid[i]]) {
                cnt[x]--;
            }

            pid[i] = old;

            for (int x : paths[pid[i]]) {
                cnt[x]++;
            }
        }
    }

    if (1) {
        cerr << acc << "/" << tot << " " << 100.0 * acc / tot << " " << score << "/" << bscore << endl;
    }

    if (score != n * n) return;

    sumd = 0;
    F0(i, m) sumd += F(robot[i], codes[pid[i]].first);

    F0(i, m) F0(j, m) if (i != j) {
        int sd = 0;
        sd -= F(robot[i], codes[pid[i]].first);
        sd -= F(robot[j], codes[pid[j]].first);

        sd += F(robot[i], codes[pid[j]].first);
        sd += F(robot[j], codes[pid[i]].first);

        if (sd <= 0) {
            sumd += sd;
            swap(pid[i], pid[j]);
        }
    }
    PR(sumd);

    ImproveSum();
}

void Second() {
    F0(i, m) {
        int dcode = codes[pid[i]].second;
        mapping[0][i] = dcode / 4;
        mapping[1][i] = dcode % 4;
        mapping[2][i] = (dcode / 4) ^ 2;
        mapping[3][i] = (dcode % 4) ^ 2;
    }

    F0(i, m) a[i] = robot[i];

    sumd = 0;
    F0(i, m) sumd += F(a[i], codes[pid[i]].first);

    int kn = 4;
    while (kn <= 10) {
        while (1) {
            int bk = -1;
            F0(k, kn) {
                int sumd2 = 0;
                F0(i, m) {
                    b[i] = nx[a[i]][mapping[k][i]];
                    sumd2 += F(b[i], codes[pid[i]].first);
                }
                if (sumd2 < sumd) {
                    sumd = sumd2;
                    bk = k;
                }
            }
            if (bk == -1) break;
            commands.push_back(bk);
            F0(i, m) a[i] = nx[a[i]][mapping[bk][i]];
        }
        if (sumd == 0) break;

        if (kn == 10) break;

        if (1) {
            F0(i, m) {
                if (a[i] == codes[pid[i]].first) {
                    mapping[kn][i] = STAY;
                    continue;
                }
                int sh = my.nextInt(4);
                F0(kk, 4) {
                    int k = (kk + sh) % 4;
                    b[i] = nx[a[i]][k];
                    if (d[b[i]][codes[pid[i]].first] <
                        d[a[i]][codes[pid[i]].first]) {
                        sumd -= F(a[i], codes[pid[i]].first);
                        sumd += F(b[i], codes[pid[i]].first);
                        a[i] = b[i];
                        mapping[kn][i] = k;
                        break;
                    }
                }
            }
            commands.push_back(kn);

            kn++;
        }
    }
    for (int i : pattern) commands.push_back(i);
}

int n1, n2;
void Cover() {
    score = 0;
    paths.clear();
    F0(i, cn) covers[i].clear();
    codes.clear();
    pattern.clear();

    if (SZ(pattern) >= bscore) return;

    F0(i, n1) {
        if (i % 2 == 0) {
            F0(j, n2) pattern.push_back(0);
            if (i != n1 - 1) pattern.push_back(1);
        } else {
            F0(j, n2) pattern.push_back(2);
            if (i != n1 - 1) pattern.push_back(1);
        }
    }

    F0(p, cn) {
        for (dir[0] = 0; dir[0] < 4; dir[0]++) {
            for (dir[1] = (dir[0] & 1) ^ 1; dir[1] < 4; dir[1]+=2) {
                dir[2] = dir[0] ^ 2;
                dir[3] = dir[1] ^ 2;
                vector<int> path;
                bitset<900> vis;
                int pos = p;
                vis[pos] = 1;
                path.push_back(pos);
                for (int x : pattern) {
                    pos = nx[pos][dir[x]];
                    if (!vis[pos]) {
                        vis[pos] = 1;
                        path.push_back(pos);
                    }
                }
                pii code(p, 4 * dir[0] + dir[1]);
                for (int x : path) covers[x].push_back(SZ(paths));
                codes.push_back(code);
                paths.push_back(path);
            }
        }
    }
    Improve();
}

void RandSol() {
    F0(i, k) F0(j, m) {
        if (i < 4) mapping[i][j] = i;
        else mapping[i][j] = rand()%4;
    }

    F0(i, 2*n*n) commands.push_back(rand()%k);
}

void Solve() {
    Prepare();

    double each = 0.1;
    while (Elapsed() < 1.8) {
    //F0(its, 10) {
        commands.clear();
        CL(0, mapping);
        n1 = my.nextInt(8, 16);
        n2 = my.nextInt(8, 16);
        Cover();
        if (score != n * n) continue;
        Second();
        if (sumd != 0) continue;

        UpdateBest();
        PR(SZ(commands), bscore);
    }

    //RandSol();

    PR(bscore);

    F0(i, k) {
        F0(j, m) cout << DS[bmapping[i][j]] << " ";
        cout << endl;
    }
    for (int x : bcommands) cout << x << endl;

    //Report();
}


void ReadInput() {
    int tmp;
    cin >> tmp >> tmp >> tmp;
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

    for (int seed=seed1; seed<=seed2; seed++) {
        if (seed>=0 && seed<10000) {
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
