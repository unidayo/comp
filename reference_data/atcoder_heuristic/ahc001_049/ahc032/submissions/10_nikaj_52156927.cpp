int DBG = 0;
//#define TIME_BASED
double TL = 1.0;
double TL2 = 1.9;
#pragma GCC optimize("O3,Ofast,omit-frame-pointer,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize")
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
const int M = 998244353;
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
const int Q = 3;
int n, m, K, usage;
int a[N][N], t[128][4][4], b[N][N], tmp[N][N];
int target[128];
int c[N][N][Q];
ll bscore, score;
int stepi[N*N], stepj[N*N];
vector<tuple<int, int, int>> bsol, sol;

const int LOGN = 1 << 16;
double logs[LOGN];

template<class T> void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }
template<class T> T sqr(T x) { return x*x; }


void Prepare() {
    PR(n); PR(m);

    vector<pii> v;
    F0(i, n - 2) F0(j, n - 2) v.push_back({i, j});
    sort(v.begin(), v.end(), [&](pii p1, pii p2){
        auto [x1,y1] = p1;
        auto [x2,y2] = p2;
        if (min(x1, y1) != min(x2, y2)) return min(x1,y1) < min(x2,y2);
        swap(p1.first, p1.second); swap(p2.first, p2.second);
        return p1<p2;
    });
    PR(v);

    F0(step, (n - 2) * (n - 2)) {
        stepi[step] = v[step].first;
        stepj[step] = v[step].second;
    }

    bscore = -1;
    F0(i, n) F0(j, n) b[i][j] = a[i][j];
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
    F1(i, 49) {
        target[i] = i * 80 / 49 - 1;
        target[i] = max(target[i], 0);
        target[i] = min(target[i], m);
    }
    target[47] = 79;
    target[48] = 80;
    target[49] = 81;
    PR(vector<int>(target, target + 50));
}

void UpdateBest() {
    if (score > bscore) {
        sol.clear();
        F0(i, n - 2) F0(j, n - 2) F0(q, Q) if (c[i][j][q] != K) sol.push_back({c[i][j][q], i, j});

        bscore = score;
        bsol = sol;
    }
}

void PickEmpty() {
    score = 0;
    F0(i, n) F0(j, n) score += a[i][j];
    F0(i, n) F0(j, n) F0(q, Q) c[i][j][q] = K;
    UpdateBest();
    usage = 0;
}


void Apply(int k, int i, int j) {
    F0(ii, 3) F0(jj, 3) {
        int x = a[i + ii][j + jj] + t[k][ii][jj];
        if (x >= M) x -= M;
        a[i + ii][j + jj] = x;
    }
}

void PickBest() {
    F0(i, n) F0(j, n) a[i][j] = b[i][j];
    F0(i, n) F0(j, n) F0(q, Q) c[i][j][q] = K;

    usage = 0;
    for (auto [k, i, j] : bsol) {
        Apply(k, i, j);
        if (c[i][j][0] == K) c[i][j][0] = k;
        else if (c[i][j][1] == K) c[i][j][1] = k;
        else c[i][j][2] = k;
        usage++;
    }
    score = 0;
    F0(i, n) F0(j, n) score += a[i][j];
}

void Local() {
    PickBest();
    int tot = 1, acc = 0;
    double T2 = 0.5 * 1e9, T1 = 0.3 * T2, TEMP = 0.0, r = 0.0;
    int itc = 1000;

    int x1 = n - 4, x2 = n - 3;
    int y1 = n - 4, y2 = n - 3;

    F0(iter, itc) {
        if ((iter & 127) == 0) {
            r = 1.0 * (itc - iter) / itc;
            if (r <= 0) {
                //PR(iter);
                break;
            }
            TEMP = T1 + (T2 - T1) * r;
            //TEMP = pow(T1, 1-r) * pow(T2, r);
        }

        ll delta = 0;
        int i = my.nextInt(x1, x2);
        int j = my.nextInt(y1, y2);
        int q = my.nextInt(Q);

        if (usage == m && c[i][j][q] == K) continue;

        F0(ii, 3) F0(jj, 3) delta -= a[i + ii][j + jj];

        int k0 = c[i][j][q];

        int k = my.nextInt(K + 1);
        while (k0 == k) k = my.nextInt(K + 1);

        F0(ii, 3) F0(jj, 3) {
            int x = a[i + ii][j + jj] - t[k0][ii][jj] + t[k][ii][jj];
            if (x < 0) x += M;
            if (x >= M) x -= M;
            delta += x;
        }

        if (delta < 0)
        tot++;
        if (delta >= 0 || delta >= -TEMP * logs[my.nextInt(LOGN)]) {
            if (delta < 0)
            acc++;
            score += delta;

            usage -= (k0 != K);
            usage += (k != K);

            F0(ii, 3) F0(jj, 3) {
                int x = a[i + ii][j + jj] - t[k0][ii][jj] + t[k][ii][jj];
                if (x < 0) x += M;
                if (x >= M) x -= M;
                a[i + ii][j + jj] = x;
            }
            c[i][j][q] = k;
            UpdateBest();

        } else {
        }
    }
    if (1) {
        //cerr << acc << "/" << tot << " " << 100.0 * acc / tot << " " << score << "/" << bscore << endl;
    }
}





// Diff (Memoryless) Beam Search
const int MSTEPS = 50;
const int MAX_EACH = 3;
typedef int MoveType;
typedef ll EvalType;
typedef ll HashType;
struct TreeState {
    int prev;
    MoveType last_move;
};
// eval, new state
typedef pair<pair<EvalType, int>, TreeState> DeltaType;
vector<DeltaType> deltas;
vector<TreeState> beam[MSTEPS];
unordered_map<HashType, EvalType> all_hashes;
int at_trail[MSTEPS], last_step;
int m_trail[MSTEPS];
HashType hash_trail[MSTEPS];
MoveType lm_trail[MSTEPS];
EvalType score_trail[MSTEPS], eval_trail[MSTEPS];
int BW = 850;
int cnt[1024];

void InitBeam() {
    all_hashes.reserve(1 << 21);

    for (int i = 0; i < MSTEPS; i++) beam[i].clear();
    deltas.clear();
    last_step = 0;
    at_trail[0] = 0;

    m_trail[0] = 0;
    eval_trail[0] = 0;
    score_trail[0] = 0;
    F0(i, n) F0(j, n) {
        score_trail[0] += a[i][j];
    }
    hash_trail[0] = 0;
    beam[0].push_back({-1, MoveType()});
}

void RevertMove(int step, const MoveType& lm) {
    int y = lm;
    int i = stepi[step - 1], j = stepj[step - 1];

    while (y) {
        int k = y % (K + 1) - 1;
        y /= (K + 1);

        F0(ii, 3) F0(jj, 3) {
            int x = a[i + ii][j + jj] - t[k][ii][jj];
            if (x < 0) x += M;
            a[i + ii][j + jj] = x;
        }
    }
}

void MakeMove(int step, const MoveType& lm) {
    lm_trail[step] = lm;
    m_trail[step] = m_trail[step - 1];
    eval_trail[step] = eval_trail[step - 1];
    score_trail[step] = score_trail[step - 1];
    int i = stepi[step - 1], j = stepj[step - 1];

    int y = lm;
    while (y) {
        int k = y % (K + 1) - 1;
        y /= (K + 1);

        m_trail[step]++;
        F0(ii, 3) F0(jj, 3) {
            score_trail[step] -= a[i + ii][j + jj];
            int x = a[i + ii][j + jj] + t[k][ii][jj];
            if (x >= M) x -= M;
            a[i + ii][j + jj] = x;
            score_trail[step] += a[i + ii][j + jj];
        }
    }
    eval_trail[step] += a[i][j];
    if (j == n - 3) {
        eval_trail[step] += a[i][j + 1];
        eval_trail[step] += a[i][j + 2];
    }
    if (i == n - 3) {
        eval_trail[step] += a[i + 1][j];
        eval_trail[step] += a[i + 2][j];
    }
    if (i == n - 3 && j == n - 3) {
        eval_trail[step] += a[i + 1][j + 1];
        eval_trail[step] += a[i + 2][j + 2];
        eval_trail[step] += a[i + 1][j + 2];
        eval_trail[step] += a[i + 2][j + 1];
    }
    //hash_trail[step] = score_trail[step] - eval_trail[step];
}

void Reach(int step, int at) {
    if (last_step > step) {
        RevertMove(last_step, lm_trail[last_step]);
        last_step--;
    }
    if (step == last_step && at_trail[step] == at) return;
    Reach(step - 1, beam[step][at].prev);
    MakeMove(step, beam[step][at].last_move);
    at_trail[step] = at;
    last_step++;
}

void Bimsa() {
    InitBeam();

    for (int step = 0; step <= (n - 2) * (n - 2); step++) {
        if (step > 0) {
            //nth_element(deltas.begin(), deltas.begin() + BW, deltas.end(),
            sort(deltas.begin(), deltas.end(),
                [&](const DeltaType& d1, const DeltaType& d2) {
                    ll s1 = d1.first.first, s2 = d2.first.first;
                    return s1 > s2;
                }
            );
            for (int x = 0; x <= MAX_EACH * step && x <= m; x++) cnt[x] = 0;

            for (int index = 0; index < SZ(deltas); index++) {
                ll m1 = deltas[index].first.second;
                if (m1 > target[step]) {
                    int over = m1 - target[step];
                    if (++cnt[m1] > (BW >> (2*over))) continue;
                }
                beam[step].push_back(deltas[index].second);
                if (SZ(beam[step]) == 2 * BW) break;
            }
            sort(beam[step].begin(), beam[step].end(),
                 [&](const TreeState& t1, const TreeState& t2) {
                    return t1.prev > t2.prev;
                 }
            );
            deltas.clear();
        }

        for (int bi = 0; bi < SZ(beam[step]); bi++) {
            Reach(step, bi);

            if (step == (n - 2) * (n - 2)) {
                ll score = score_trail[step];
                if (score > bscore) {
                    bscore = score;
                    bsol.clear();
                    F1(i, step) {
                        int y = lm_trail[i];
                        while (y) {
                            int k = y % (K + 1) - 1;
                            y /= (K + 1);

                            bsol.push_back({k, stepi[i - 1], stepj[i - 1]});
                        }
                    }
                }
                continue;
            }

            int i = stepi[step], j = stepj[step];

            F0(k, K + 1) if (k == 0 || m_trail[step] + 1 <= m) {
                MoveType lm = k;
                MakeMove(step + 1, lm);
                deltas.push_back({{eval_trail[step + 1], m_trail[step + 1]}, {bi, lm}});
                RevertMove(step + 1, lm);

                if (k != 0 && m_trail[step] + 2 <= m) for (int k2 = k; k2 < K; k2++) {
                    MoveType lm = (k + 1) * (K + 1) + (k2 + 1);
                    MakeMove(step + 1, lm);
                    deltas.push_back({{eval_trail[step + 1], m_trail[step + 1]}, {bi, lm}});
                    RevertMove(step + 1, lm);

                    if (i >= n - 3 && j >= n - 3 && m_trail[step] + 3 <= m) {
                        for (int k3 = k2; k3 < K; k3++) {
                            MoveType lm = k + 1;
                            lm = lm * (K + 1) + (k2 + 1);
                            lm = lm * (K + 1) + (k3 + 1);
                            MakeMove(step + 1, lm);
                            deltas.push_back({{eval_trail[step + 1], m_trail[step + 1]}, {bi, lm}});
                            RevertMove(step + 1, lm);
                        }
                    }
                }
            }
        }
    }
    Report();
}



void Solve() {
    Prepare();

    //Improve();

    Bimsa();
    //while (Elapsed() < TL2) Local();

    PR(bscore);

    cout << SZ(bsol) << endl;
    F0(i, SZ(bsol)) {
        auto [k, x, y] = bsol[i];
        cout << k << " " << x << " " << y << endl;
    }

    //Report();
}


void ReadInput() {
    cin >> n >> K >> m;
    F0(i, n) F0(j, n) cin >> a[i][j];
    F0(k, K) F0(i, 3) F0(j, 3) cin >> t[k][i][j];
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
