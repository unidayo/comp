#pragma GCC optimize("O3,Ofast,omit-frame-pointer,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize")
int times = 1;
double TL = 2.85;
int STANDARD = 1;
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
#ifdef RED
int DBG = 1;
#define AT(i) AutoTimer a##i(i)
#define PR(x) cerr << #x << "=" << x << endl
#else
int DBG = 0;
#define PR(x)
#define AT(i)
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

// CONSTANTS
const int N = 32;
typedef unsigned __int128 MaskType;
const int ML = 128;
int n, m, sv, rscore, rn;
double e;
int sz[N], px[N][N*N], py[N][N*N], lx[N], ly[N];
vector<int> pos[N];
int cn[N];
MaskType rmask[N][N*N][4];
vector<int> r[N][N*N];
int a[N][N], b[N*N];
double cheat_samples[2*N*N];
int ask_id;
int cheat_posx[N], cheat_posy[N];
int cheat_v[N][N], cheat_ans_size;
double test_difficulty;
set<int> sol;
double score, score_entropy;
typedef vector<int> Config;
vector<int> current_cand;
int coverage[N*N], current_v[2*N*N];
double stats[N][N*N];
int ok[N][N*N];
const int GS = 1<<10;
const int MOUT = 501;
const int tries = 64000;
double pre_tries[tries];
double log2_num[tries + 1];
double out_prob[1 << 9], log2_out_prob[1 << 9];
unsigned short prec_distr[401][401][MOUT];
int prec_mn[401][401], prec_mx[401][401];
double prec_log_1_tries;
vector<int> q2[2*N*N];
MaskType q2mask[2*N*N][4];
int q2k[2*N*N], q2res[2*N*N], q2n;
double g_max;
int q_order[2*N*N], pos_left[N*N], pos_count;
double q2log[2*N*N][401];
const int MCAND = 1 << 9;
const double log2e = log2(exp(1.0));
Config best_cands[MCAND];
vector<Config> last_step_cands;
double best_probs[MCAND], best_e[MCAND];
int u[N*N], uk;
char best_freq[MCAND][512];
short best_v[MCAND];
bool timeout = false;
int n_affected[N*N], affected[N*N][MCAND];
int g_count[N*N], g_active[N*N], g_where[N*N], gn;
double g_entropy[N*N], g_psum[N*N], old_entropy;
int n_cand, n_restart, n_each;
double new_entropy[MOUT];
set<pair<double, int>> candS;
unordered_set<ll> hashS, banned;
double set_lb, max_lb = 10;
int opened[2*N*N], on, ores[2*N*N], ocount[2*N*N];
Config smpl;
pii GetPos(int i, int id) { return pii(id / (n - ly[i]), id % (n - ly[i])); }
double g_prob[N*N], g_sump[N*N];
set<set<int>> all_sols;
vector<int> cells_to_combine;
int save_u[N*N];
int sect[20][400][800];
vector<pii> odiff[20][401][401];

inline int popcnt(MaskType m) {
    return __builtin_popcountll(m) + __builtin_popcountll(m >> 64);
}

const int LOGN = 1 << 16;
double logs[LOGN];

template<class T> void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }
template<class T> T sqr(T x) { return x*x; }
#define ID(x, y) ((x)*n)+(y)
pii CELL(int x) { return pii((x)/n, (x)%n); }

std::normal_distribution<double> distr(0.0, 1.0);
//std::mt19937 gen(time(0));
std::mt19937 gen(2029);

void SetAt(MaskType* m, int at) { m[at / ML] |= MaskType(1) << at % ML; }
bool HasAt(MaskType* m, int at) { return (m[at / ML] >> (at % ML)) & 1; }

bool NoMoreQ() { return ask_id == 2*n*n; }

void Prepare() {
    ask_id = 0;
    score = score_entropy = 0.0;
    rn = (n*n + ML - 1) / ML;
    PR(n); PR(m); PR(e);
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
    F0(i, tries) log2_num[i] = log2(i);
    prec_log_1_tries = log(1.0 / tries);

    F0(i, n) F0(j, n) a[i][j] = b[ID(i, j)] = -1;

    sv = 0;
    F0(i, m) {
        sv += sz[i];
        lx[i] = ly[i] = 0;
        F0(j, sz[i]) {
            lx[i] = max(lx[i], px[i][j]);
            ly[i] = max(ly[i], py[i][j]);
        }
    }

    test_difficulty = 0;
    F0(i, m) { test_difficulty += log(1.0 * n - lx[i]) + log(1.0 * n - ly[i]); }
    //cerr << test_difficulty << " " << e << " " << times << " ";
    if (DBG) cerr << "Diff=" << setw(5) << setprecision(3) << test_difficulty << " EPS=" << e / 0.01 << endl;

    F0(i, n*n+1) F0(j, sv + 1) {
        prec_mn[i][j] = inf;
        prec_mx[i][j] = -1;
    }

    F0(i, m) {
        cn[i] = 0;
        F0(x, n - lx[i]) F0(y, n - ly[i]) {
            r[i][cn[i]].clear();
            F0(j, sz[i]) {
                r[i][cn[i]].push_back(ID(x + px[i][j], y + py[i][j]));
                SetAt(rmask[i][cn[i]], ID(x + px[i][j], y + py[i][j]));
            }
            pos[i].push_back(cn[i]);
            ok[i][cn[i]++] = 1;
        }
    }

    F0(i, tries) pre_tries[i] = distr(gen);
    sort(pre_tries, pre_tries + tries);
    //cerr << setprecision(10) << pre_tries[0] * 1000 << " " << pre_tries[tries - 1] * 1000 << endl;
}

bool Valid(Config& config) {
    F0(o, on) {
        int cnt = 0;
        F0(i, m) cnt += HasAt(rmask[i][config[i]], opened[o]);
        if (cnt != ores[o]) return false;
    }
    return true;
}

set<int> GenSol(Config& config) {
    set<int> sol;
    F0(i, m) {
        int j = config[i];
        for (int x : r[i][j]) sol.insert(x);
    }
    return sol;
}

int Sample(double mean, double sigma, int try_id) {
    return max((int)(mean + sigma * pre_tries[try_id] + 0.5), 0);
}

void CalcDistr(int k, int v) {
    double sigma = sqrt(k * e * (1.0 - e));
    double mean = (k - v) * e + v * (1.0 - e);
    prec_mn[k][v] = Sample(mean, sigma, 0);
    prec_mx[k][v] = Sample(mean, sigma, tries - 1);
    int lb = 0, L = 0, R = 0, M = 0;
    for (int x = prec_mn[k][v]; x < prec_mx[k][v]; x++) {
        double p = (x + 0.5 - mean) / sigma;
        R = tries - 1;
        while (L < R) {
            M = (L + R) / 2;
            if (pre_tries[M] < p) L = M + 1;
            else R = M;
        }
        prec_distr[k][v][x] = max(L - lb, 1);
        lb = L;
    }
    prec_distr[k][v][prec_mx[k][v]] = tries - lb;
}

void CalcLogs(int qi) {
    int v1 = 0, v2 = 0;
    F0(i, m) {
        int minv = inf, maxv = -1;
        for (int p : pos[i]) {
            int v = 0;
            F0(j, rn) v += popcnt(rmask[i][p][j] & q2mask[qi][j]);
            sect[i][p][qi] = v;
            minv = min(minv, v);
            maxv = max(maxv, v);
        }
        v1 += minv;
        v2 += maxv;
    }
    for (int v = v1; v <= v2; v++) {
        q2log[qi][v] = 0.0;
        F0(t, times) {
            int k = q2k[qi];
            int x = q2res[t];
            if (prec_mx[k][v] == -1) CalcDistr(k, v);
            if (x < prec_mn[k][v] || x > prec_mx[k][v]) {
                double d = (x < prec_mn[k][v]) ? prec_mn[k][v] - x
                                               : x - prec_mx[k][v];
                q2log[qi][v] += prec_log_1_tries - 1.3 * d;
            } else {
                q2log[qi][v] += log(1.0 * prec_distr[k][v][x] / tries);
            }
        }
    }
}

int d1(int x) { if (2*x>n) x = n-1-x; x=n/2-x; return x; }
int d1(pii p) {
    return d1(p.first) + d1(p.second);
}

int Ask1(int x, int y) {
    if (a[x][y] == -1) {
        ask_id++;
        score += 1.0;
        if (STANDARD) {
            cout << "q 1 " << x << " " << y << endl;
            cin >> a[x][y];
        } else {
            a[x][y] = cheat_v[x][y];
        }
    }
    return b[ID(x, y)] = a[x][y];
}

int Ask1(int x) { return Ask1(x / n, x % n); }

int Ask2Helper(vector<int> x) {
    int k = SZ(x);
    score += sqrt(1.0 / k);
    int res = -1;
    if (!STANDARD) {
        int v = 0;
        for (int p : x) {
            v += cheat_v[p / n][p % n];
        }
        double mean = (k - v) * e + v * (1.0 - e);
        double sigma = sqrt(k * e * (1.0 - e));
        res = max(int(mean + cheat_samples[ask_id++] * sigma + 0.5), 0);
    } else {
        ask_id++;
        cout << "q " << SZ(x);
        for (int p : x) {
            cout << " " << p / n << " " << p % n;
        }
        cout << endl;
        cin >> res;
    }
    return res;
}

void Ask2(vector<int> x) {
    int k = SZ(x);
    q2[q2n] = x;
    F0(i, rn) q2mask[q2n][i] = 0;
    for (int at : x) SetAt(q2mask[q2n], at);
    q2k[q2n++] = k;
    F0(i, times) q2res[i] = Ask2Helper(x);

    CalcLogs(q2n - 1);
}

int Ans(set<int> sol) {
    if (all_sols.count(sol)) {
        return 0;
    }
    all_sols.insert(sol);
    ask_id++;
    score += 1.0;
    if (!STANDARD) {
        if (SZ(sol) != cheat_ans_size) return 0;
        for (int p : sol) if (p < 0 || p >= n*n) throw;
        else if (!cheat_v[p / n][p % n]) return 0;
        score -= 1.0;
        return 1;
    } else {
        cout << "a " << SZ(sol);
        for (int p : sol) cout << " " << p / n << " " << p % n;
        cout << endl;
        int res;
        cin >> res;
        if (res == 1) score -= 1.0;
        return res;
    }
}

void Naive() {
    int r = 0;
    F0(i, m) r += sz[i];
    vector<pii> ord;
    F0(i, n) F0(j, n) {
        ord.push_back({i, j});
    }
    sort(ord.begin(), ord.end(), [&](pii p1, pii p2) {
        return d1(p1) < d1(p2);
    });
    pii lst = ord.back();
    ord.pop_back();
    for (pii p : ord) {
        int i = p.first, j = p.second;
        if (NoMoreQ()) {
            score = 1000.0;
            return;
        }
        int x = Ask1(i, j);
        r -= x;
        if (!r) break;
    }
    if (r) a[lst.first][lst.second] = r;

    if (NoMoreQ()) {
        score = 1000.0;
        return;
    }
    sol.clear();
    F0(i, n) F0(j, n) if (a[i][j] > 0) sol.insert(ID(i, j));
    Ans(sol);
}

Config RandomSample() {
    Config ret;
    F0(i, m) ret.push_back(pos[i][my.nextInt(cn[i])]);
    return ret;
}

double LogEvalSingle(const Config& sample, int qi) {
    current_v[qi] = 0;
    F0(i, m) current_v[qi] += sect[i][sample[i]][qi];
    return q2log[qi][ current_v[qi] ];
}

double LogEval(const Config& sample) {
    double ret = 0.0;
    F0(qi, q2n) {
        ret += LogEvalSingle(sample, qi);
    }

    F0(o, on) {
        ocount[o] = 0;
        F0(i, m) ocount[o] += HasAt(rmask[i][sample[i]], opened[o]);
    }

    return ret;
}

ll Hash() {
    ll r = 0;
    F0(i, m) r = r * 401 + smpl[i];
    return r;
}

void AppendCand(double av) {
    if (av >= set_lb && (SZ(candS) < n_cand || candS.begin()->first < av)) {
        ll h = Hash();
        if (!hashS.count(h)) {
            hashS.insert(h);
            int i = SZ(candS);
            if (i == n_cand) {
                i = candS.begin()->second;
                candS.erase(candS.begin());
            }
            best_cands[i] = smpl;
            best_probs[i] = av;
            candS.insert(make_pair(av, i));

            if ((--candS.end())->first - max_lb > set_lb) {
                set_lb = (--candS.end())->first - max_lb;
                //while (candS.begin()->first < set_lb) candS.erase(candS.begin());
            }
        }
    }
}

double F(double p) {
    return p * p;
}

void ReorderQ() {
    F0(x, q2n) swap(q_order[x], q_order[my.nextInt(x + 1)]);
}

int cc(int x, int y) {
    return g_prob[x] > g_prob[y];
}

void GetBestCands() {
    set_lb = -inf;
    candS.clear();
    hashS = banned;
    n_cand = MCAND;
    n_restart = 10000 / (4*m);
    n_each = 4*m;
    if (q2n == 0) {
        F0(ci, n_cand) {
            smpl = RandomSample();
            AppendCand(0.0);
        }
    } else {
        int last_sz = SZ(last_step_cands);
        int at = my.nextInt(m);
        PR(n_restart);
        int last_at = 0;

        F0(x, q2n) q_order[x] = x;
        reverse(q_order, q_order + q2n);

        F0(itr, n_restart) {
            if (itr % 2 == 0 && last_sz) {
                smpl = last_step_cands[last_at];
                if (++last_at == last_sz) last_at = 0;
            } else {
                smpl = RandomSample();
            }
            double av = LogEval(smpl);
            AppendCand(av);

            ReorderQ();

            F0(it, n_each) {
                if (++at == m) {
                    at = 0;
                }
                if (cn[at] == 1) continue;
                //if (it % (m) == 0) { ReorderQ(); }
                int prev_at = smpl[at];

                F0(qi, q2n) {
                    current_v[qi] -= sect[at][prev_at][qi];
                }

                pos_count = 0;
                for (int c : pos[at]) {
                    g_prob[c] = 0.0;
                    for (pii p : odiff[at][prev_at][c]) {
                        int newo = ocount[p.first] + p.second;
                        int delta = abs(newo - ores[p.first]) - abs(ocount[p.first] - ores[p.first]);
                        g_prob[c] -= delta * 2.5;
                    }
                    pos_left[pos_count++] = c;
                }

                int l = (q2n + 5) / 10 + 1;

                int steps = 0;
                if (l > 1) {
                    int tmp = pos_count;
                    while (tmp > 1) {
                        steps++;
                        tmp = (tmp + l - 1) / l;
                    }
                    if (steps > q2n - 1) steps = q2n - 1;
                }

                c_o[1] += q2n * SZ(pos[at]) * rn;
                int bstep = 0;
                F0(i, q2n) {
                    int qi = q_order[i];
                    c_o[2] += pos_count * rn;

                    F0(index, pos_count) {
                        int c = pos_left[index];
                        int v = current_v[qi];
                        v += sect[at][c][qi];
                        g_prob[c] += q2log[qi] [ v ];
                    }

                    if (l > 1 && i + 1 != q2n && 1) {
                        int cstep = (i + 1) * (steps + 1) / q2n;
                        if (bstep != cstep) {
                            bstep++;
                            nth_element(pos_left, pos_left + (pos_count + l - 1) / l, pos_left + pos_count, cc);
                            pos_count = (pos_count + l - 1) / l;
                        }
                    }
                }
                int best_new_cand = pos_left[0];
                if (pos_count > 1) {
                    g_max = -1e9;
                    F0(index, pos_count) {
                        int c = pos_left[index];
                        g_max = max(g_max, g_prob[c]);
                    }
                    best_new_cand = 0;
                    double psum = 0.0;
                    F0(index, pos_count) {
                        int c = pos_left[index];
                        g_prob[c] -= g_max;
                        g_prob[c] = exp(g_prob[c]);
                        psum += F(g_prob[c]);
                    }
                    double cutoff = my.nextDouble() * psum;
                    psum = 0.0;
                    F0(index, pos_count) {
                        int c = pos_left[index];
                        psum += F(g_prob[c]);
                        if (psum >= cutoff - EPS) {
                            best_new_cand = c;
                            break;
                        }
                    }
                }
                double av = 0;
                smpl[at] = best_new_cand;
                F0(qi, q2n) {
                    current_v[qi] += sect[at][best_new_cand][qi];
                    av += q2log[qi] [ current_v[qi] ];
                }
                if (prev_at != best_new_cand) {
                    for (pii p : odiff[at][prev_at][best_new_cand]) {
                        ocount[p.first] += p.second;
                    }
                    AppendCand(av);
                }
            }
        }
        if (n_restart <= last_sz * 2) {
            for (int i = last_at; i < last_sz; i++) {
                smpl = last_step_cands[i];
                double av = LogEval(smpl);
                AppendCand(av);
            }
        }
    }

    if (!STANDARD && 0) {
        vector<int> best_smpl;
        double mx = -1e9;
        F0(ci, SZ(candS)) {
            if (best_probs[ci] > mx) {
                mx = max(mx, best_probs[ci]);
                best_smpl = best_cands[ci];
            }
        }
        vector<int> actual;
        F0(i, m) actual.push_back(cheat_posx[i] * (n - ly[i]) + cheat_posy[i]);
        int match = GenSol(actual) == GenSol(best_smpl);
        double cv = LogEval(actual) - mx;
        if (DBG) {
            cerr << "Actual: " << cv << "  Match: " << match << endl;
        }
    }

    last_step_cands.clear();
    for (auto it = candS.end(); it != candS.begin(); ) {
        it--;
        last_step_cands.push_back(best_cands[it->second]);
    }

    n_cand = SZ(candS);
    double psum = 0.0, mx = -1e9;
    F0(ci, n_cand) mx = max(mx, best_probs[ci]);

    F0(ci, n_cand) {
        best_probs[ci] -= mx;
        best_probs[ci] = exp(best_probs[ci]);
        psum += best_probs[ci];
    }
    F0(ci, n_cand) {
        best_probs[ci] /= psum;
        best_e[ci] = -best_probs[ci] * log2(best_probs[ci]);
    }

    if (DBG) {
        vector<double> pr;
        F0(ci, n_cand) pr.push_back(best_probs[ci]);
        sort(pr.rbegin(), pr.rend());
        pr.resize(8);
        cerr << "Score: " << score << " " << pr << endl;
        cerr << endl;
    }
}

void AddGroup(int v) {
    g_where[v] = gn;
    g_active[gn++] = v;
}

void DeleteGroup(int v) {
    int i = g_where[v];
    if (i != gn - 1) {
        g_active[i] = g_active[gn - 1];
        g_where[g_active[i]] = i;
    }
    gn--;
}

void RemoveFromGroup(int ci) {
    int v = best_v[ci];
    g_entropy[v] -= best_e[ci];
    g_psum[v] -= best_probs[ci];
    if (--g_count[v] == 0) {
        DeleteGroup(v);
    }
}

void AddToGroup(int ci) {
    int v = best_v[ci];
    g_entropy[v] += best_e[ci];
    g_psum[v] += best_probs[ci];
    if (g_count[v]++ == 0) {
        AddGroup(v);
    }
}

void Add(int x) {
    uk++;
    u[x] = 1;
    F0(index, n_affected[x]) {
        int ci = affected[x][index];
        RemoveFromGroup(ci);
        best_v[ci] += best_freq[ci][x];
        AddToGroup(ci);
    }
}

void Remove(int x) {
    u[x] = 0;
    uk--;
    F0(index, n_affected[x]) {
        int ci = affected[x][index];
        RemoveFromGroup(ci);
        best_v[ci] -= best_freq[ci][x];
        AddToGroup(ci);
    }
}

double EvalGain() {
    int x1 = inf, x2 = -1;
    F0(index, gn) {
        int v = g_active[index];
        if (prec_mx[uk][v] == -1) CalcDistr(uk, v);
        if (prec_mn[uk][v] < x1) x1 = prec_mn[uk][v];
        if (prec_mx[uk][v] > x2) x2 = prec_mx[uk][v];
    }

    for (int x = x1; x <= x2; x++) {
        out_prob[x] = new_entropy[x] = 0.0;
    }

    F0(index, gn) {
        int v = g_active[index];
        for (int x = prec_mn[uk][v]; x <= prec_mx[uk][v]; x++) {
            out_prob[x] += g_psum[v] * prec_distr[uk][v][x];
        }
    }

    for (int x = x1; x <= x2; x++) {
        log2_out_prob[x] = log2(out_prob[x]);
    }

    double res = 0.0;
    F0(index, gn) {
        int v = g_active[index];
        for (int x = prec_mn[uk][v]; x <= prec_mx[uk][v]; x++) {
            new_entropy[x] += prec_distr[uk][v][x] / out_prob[x] * ((log2_out_prob[x] - log2_num[prec_distr[uk][v][x]]) * g_psum[v] + g_entropy[v]);
        }
    }

    for (int x = x1; x <= x2; x++) {
        res += out_prob[x] * (old_entropy - new_entropy[x]);
    }

    return res;
}

void GetBestInfoGain() {
    F0(ci, n_cand) F0(x, n*n) best_freq[ci][x] = 0;
    F0(ci, n_cand) {
        F0(i, m) for (int x : r[i][best_cands[ci][i]]) best_freq[ci][x]++;
    }
    F0(x, n*n) {
        n_affected[x] = 0;
        F0(ci, n_cand) if (best_freq[ci][x]) affected[x][n_affected[x]++] = ci;
    }

    uk = 0;
    F0(x, n*n) u[x] = 0;

    F0(i, sv + 1) {
        g_entropy[i] = g_psum[i] = 0.0;
        g_count[i] = 0;
    }
    F0(ci, n_cand) {
        best_v[ci] = 0;
        g_entropy[0] += best_e[ci];
        g_psum[0] += best_probs[ci];
        g_count[0]++;
    }
    gn = 1;
    g_active[0] = 0;
    g_where[0] = 0;

    if (q2n == 0) {
        F0(x, n*n) if (my.nextInt(2)) Add(x);
    } else {
        F0(x, n*n) if (save_u[x]) Add(x);
        F0(x, n*n) {
            int y = my.nextInt(n*n);
            if (u[x] != u[y]) {
                if (u[x]) { Remove(x); Add(y); }
                else { Add(x); Remove(y); }
            }
        }
    }

    old_entropy = 0.0;
    F0(index, gn) {
        int v = g_active[index];
        old_entropy += g_entropy[v];
    }

    PR(q2n);
    double bv = 0.0;
    int move_type = 0;

    double current_av = EvalGain();
    current_av *= sqrt(1.0 * uk);
    F0(steps, 2000) {
        int x = my.nextInt(n*n);
        int y = my.nextInt(n*n);
        if (u[x] > u[y]) swap(x, y);
        if (u[x] != u[y]) {
            Add(x); Remove(y);
            move_type = 1;
        } else {
            if (u[x]) Remove(x);
            else Add(x);
            move_type = 2;
        }
        double entropy_gain = EvalGain();
        double av = entropy_gain;
        av *= sqrt(1.0 * uk);
        if (av >= current_av) {
            current_av = av;
            if (av > bv) {
                bv = av;
                F0(x, n*n) save_u[x] = u[x];
            }
        } else {
            if (move_type == 2) {
                if (u[x]) Remove(x);
                else Add(x);
            } else {
                Add(y); Remove(x);
            }
        }
    }

    F0(x, n*n) u[x] = save_u[x];
    cells_to_combine.clear();

    F0(x, n*n) if (u[x]) cells_to_combine.push_back(x);

    if (0) {
        cerr << "K=" << uk << " " << bv << endl;
        PR(bv);
        F0(x, n) {
            F0(y, n) {
                cerr << u[ID(x, y)];
            }
            cerr << endl;
        }
    }
}

void SolveSampling() {
    F0(o, on) F0(i, m) {
        vector<int> h1, h2;
        for (int c : pos[i]) if (HasAt(rmask[i][c], opened[o]))
            h1.push_back(c);
        else
            h2.push_back(c);
        for (int c1 : h1) for (int c2 : h2) {
            odiff[i][c1][c2].push_back({o, -1});
            odiff[i][c2][c1].push_back({o, 1});
        }
    }

    F0(i, m) cn[i] = SZ(pos[i]);
    //vector<int> all; F0(x, n*n) all.push_back(x); RS(all);
    banned.clear();
    while (ask_id < 2*n*n - times) {
        GetBestCands();

        if (Elapsed() > TL) {
            for (auto it = candS.end(); ask_id < 2*n*n && it != candS.begin(); ) {
                it--;
                if (Valid(best_cands[it->second])) {
                    int i = it->second;
                    sol = GenSol(best_cands[i]);
                    if (Ans(sol)) {
                        PR("LUCKY!");
                        return;
                    }
                }
            }
            break;
        }

        int best = -1;
        double lost_prob = 0.0;
        for (auto it = candS.end(); it != candS.begin(); ) {
            it--;
            if (Valid(best_cands[it->second])) {
                best = it->second;
                break;
            } else lost_prob += it->first;
        }

        if (best != -1) {
            sol = GenSol(best_cands[best]);
            double high_prob = 0;

            for (auto p : candS) {
                if (GenSol(best_cands[p.second]) == sol && Valid(best_cands[p.second])) {
                    high_prob += best_probs[p.second];
                }
            }

            if (high_prob > 0.65 * (1.01 - lost_prob)) {
                bool f = Ans(sol);
                if (f) {
                    PR("HIT!");
                    return;
                } else {
                    for (auto p : candS) {
                        if (GenSol(best_cands[p.second]) == sol) {
                            smpl = best_cands[p.second];
                            ll h = Hash();
                            banned.insert(h);
                        }
                    }
                    PR("Miss!");
                    continue;
                }
            }
        }
        GetBestInfoGain();
        Ask2(cells_to_combine);
    }
    Naive();
}

double p_zero[N*N];
int nonzero[N*N];
int f_contr[N*N];
set<int> coverers[N*N];
int solved[N*N];
bool found = false;

void Solve(int i) {
    found = true;
    solved[i] = 1;
    for (int p : pos[i]) {
        for (int x : r[i][p]) f_contr[x]++;
    }
}

void SolveSeparate() {
    F0(i, m) {
        if (SZ(pos[i]) == 1) Solve(i);
    }

    double th = 45;
    if (m == 10 || m == 11) th = 42;
    else if (m == 12 || m == 13) th = 46;
    else if (m == 14 || m == 15 || m == 16) th = 48;

    while (true) {
        F0(x, n*n) { p_zero[x] = 1.0; coverers[x].clear(); }
        found = false;
        F0(i, m) if (SZ(pos[i]) > 1) {
            vector<int> SN;
            F0(x, n*n) coverage[x] = 0;
            for (int p : pos[i]) {
                int bad = 0;
                for (int x : r[i][p]) {
                    if (b[x] == f_contr[x]) { bad = 1; break; }
                }
                if (!bad) {
                    SN.push_back(p);
                    for (int x : r[i][p]) {
                        coverage[x]++;
                        coverers[x].insert(i);
                    }
                }
            }
            pos[i].swap(SN);
            if (SZ(pos[i]) == 1) Solve(i);
            F0(x, n*n) if (b[x] == -1) {
                if (coverage[x] != 0) p_zero[x] *= 1.0 * (SZ(pos[i]) - coverage[x]) / SZ(pos[i]);
            }
        }

        if (found) continue;

        F0(x, n*n) if (b[x] == f_contr[x] + SZ(coverers[x])) {
            for (int i : coverers[x]) {
                vector<int> SN;
                for (int p : pos[i]) if (HasAt(rmask[i][p], x)) {
                    SN.push_back(p);
                }
                if (SZ(pos[i]) != SZ(SN)) {
                    if (DBG) cout << "#c" << x / n << " " << x % n << " blue" << endl;
                    PR(SZ(pos[i]) - SZ(SN));
                    pos[i].swap(SN);
                    if (SZ(pos[i]) == 1) Solve(i);
                    found = true;
                }
            }
            if (found) break;
        }
        if (found) continue;

        F0(x, n*n) if (b[x] == -1 && coverers[x].empty()) {
            if (DBG) cout << "#c" << x / n << " " << x % n << " yellow" << endl;
            a[x / n][x % n] = b[x] = f_contr[x];
            found = true;
        }
        if (found) continue;

        double bp = 0, actualp = 0;
        int bi = -1;
        F0(x, n*n) if (b[x] == -1 && !nonzero[x]) {
            double p = p_zero[x];
            if (p == 0.0 && !nonzero[x]) {
                nonzero[x] = 1;
                if (DBG) cout << "#c" << x / n << " " << x % n << " green" << endl;
                found = true;
                break;
            }

            double elims = 0;
            for (int i : coverers[x]) {
                int cnt = 0;
                for (int p : pos[i]) if (HasAt(rmask[i][p], x)) cnt++;
                elims += log(SZ(pos[i])) - log(SZ(pos[i]) - cnt);
            }

            double ap = p * elims;

            if (ap > bp) {
                actualp = p;
                bp = ap;
                bi = x;
            }
        }
        if (found) continue;

        double prod = 0.0;
        F0(i, m) prod += log(SZ(pos[i]));
        if (prod <= th) {
            SolveSampling();
            return;
        }

        if (DBG) {
            vector<int> szs;
            F0(i, m) szs.push_back(SZ(pos[i]));
            if (DBG) { cerr << CELL(bi) << " p:" << actualp << " bp:" << bp << " " << " "; PR(szs); }
        }

        if (bi == -1) break;
        if (DBG) cout << "#c" << bi / n << " " << bi % n << " purple" << endl;
        int y = Ask1(bi);
        if (y > 0) {
            ores[on] = y;
            opened[on++] = bi;
        }
    }

    F0(x, n*n) if (b[x] > 0 || f_contr[x] || nonzero[x]) sol.insert(x);
    Ans(sol);
}

void Solve() {
    Prepare();
    if (m < 10) {
        if (m == 7) { if (test_difficulty > 30) times = 2; }
        if (m == 8) times = 2;
        if (m == 9) times = 3;
        SolveSampling();
    }
    else {
        SolveSeparate();
    }
    cerr << score << endl;
    //if (DBG) Report();
}


void ReadInput() {
    cin >> n >> m >> e;
    F0(i, m) {
        cin >> sz[i];
        F0(j, sz[i]) cin >> px[i][j] >> py[i][j];
    }
    if (!STANDARD) {
        //e = 0.01;
        cheat_ans_size = 0;
        F0(k, m) cin >> cheat_posx[k] >> cheat_posy[k];
        F0(x, n) F0(y, n) { cin >> cheat_v[x][y]; cheat_ans_size += cheat_v[x][y] > 0; }
        F0(x, 2*n*n) cin >> cheat_samples[x];
    }
}

int main(int argc, char* argv[]) {
    Init();

    int seed1 = 0, seed2 = 0;
    if(argc>1) {
        if (argc == 2) {
            seed1 = seed2 = atoi(argv[1]);
        } else {
            seed1 = seed2 = atoi(argv[1]);
            my.reseed(atoi(argv[2]));
        }
        STANDARD=0;
    }

    if(STANDARD) {
        //STANDARD=0;
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
            //ignore = freopen(outp, "w", stdout);
            ReadInput();
            Solve();
            cout << "Score would be " << int(score * 1000000 + 0.5) << endl;
        } else {
            // Generate
            throw;
            Rand my(seed);
        }
    }

    return 0;
}
