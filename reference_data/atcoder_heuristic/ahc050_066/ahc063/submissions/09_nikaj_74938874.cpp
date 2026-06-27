double TL = 1.95;
//#pragma GCC optimize("O3,Ofast,omit-frame-pointer,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize")
#include <bits/stdc++.h>

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

chrono::system_clock::time_point init_time = chrono::system_clock::now();
inline double GetSeconds() {
     chrono::system_clock::time_point current_time = chrono::system_clock::now();
     double ret = chrono::duration_cast<std::chrono::nanoseconds>(current_time - init_time).count();
     return ret * 1e-9;
}

const int MAX_RAND = 1 << 30;
struct Rand {
    ll x, y, z, w, o;
    Rand() { throw; }
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

#ifdef LOCAL
#define PR(...) cerr << #__VA_ARGS__ << " =", print_all(__VA_ARGS__)
struct AutoTimer {
    int x;
    double t;
    AutoTimer(int x) : x(x) { t = Elapsed(); }
    ~AutoTimer() { t_o[x] += Elapsed() - t; }
};
#define AT(i) AutoTimer _a##i(i)
#else
#pragma GCC optimize("O3,unroll-all-loops")
#define PR(...)
#define AT(i)
#endif

// CONSTANTS
const int N = 16;
const int N2 = N*N;
const int MAXL = 2048;
enum { RIGHT, DOWN, LEFT, UP };
const int DX[]={0,1,0,-1};
const int DY[]={1,0,-1,0};
const string DS="RDLU";
int n, m, C;
double mr;
bool sol_found = false;
ll bscore, score;
vector<short> bsol, sol, csol, last_sol;
vector<int> adj[N*N];
int qx[N*N], qy[N*N], id[N][N], nx[N*N][5], cn, d2[N*N][N*N];
int target[N2], g[N2], a[N2], b[MAXL];
ll zh[N2], zh2[N2], zhx[8][N2];
int L, R, sz;
pii GET(int id) { return pii(qx[id], qy[id]); }

template<class T> T sqr(T x) { return x*x; }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
int GetDir(int p1, int p2) { F0(k,4) if (nx[p1][k]==p2) return k; throw; }

void PrepareGrid() {
    cn = 0;
    F0(i, n) F0(j, n) {
        qx[cn] = i;
        qy[cn] = j;
        id[i][j] = cn++;
    }
    F0(i, cn) {
        pii p(qx[i], qy[i]);
        F0(k, 4) {
            nx[i][k] = -1;
            pii q = Move(p, k);
            if (In(q)) {
                nx[i][k] = id[q.first][q.second];
                adj[i].push_back(nx[i][k]);
            }
        }
    }
}


void Prepare() {
    mr = 1.0 * m / n / n; // 0.25-0.75
    PR(n, m, C, mr);

    PrepareGrid();
    F0(i, cn) cin >> g[i];

    F0(i,cn) {
        zh[i] = my.zh();
        zh2[i] = my.zh();
        F0(j,8) zhx[j][i]=my.zh();
    }

    vector<int> freq(C);
    F0(i,cn) if (g[i]) freq[g[i]-1]++;
    sort(freq.begin(),freq.end());
    PR(freq);

    //F0(i,N2) F0(j,N2) d2[i][j]=sqrt(sqr(qx[i]-qx[j])+sqr(qy[i]-qy[j]))+0.5;
    F0(i,N2) F0(j,N2) d2[i][j]=abs(qx[i]-qx[j])+abs(qy[i]-qy[j]);
    bscore = inf;
}

void UpdateBest2() {
    if (score < bscore) {
        bscore = score;
        int pos = id[4][0];
        bsol.clear();
        for (int x : sol) {
            bsol.push_back(GetDir(pos, x));
            pos = x;
        }
    }
}

void UpdatePart() {
    int pos = id[4][0];
    bsol.clear();
    for (int x : sol) {
        bsol.push_back(GetDir(pos, x));
        pos = x;
    }
}

int tn, t[N2], at;
bool solved;
int ak[N2];
int col[N2], ci;
// Diff (Stateless) Beam Search
const int MSTEPS = 2005;
typedef short MoveType;
typedef int EvalType;
typedef ll HashType;
struct TreeState {
    int prev;
    short prev_step;
    MoveType last_move;
};
// eval, new state
typedef pair<EvalType, TreeState> DeltaType;
vector<DeltaType> a_deltas[MSTEPS];
vector<TreeState> beam[MSTEPS];
EvalType a_eval_bound[MSTEPS];
struct FastHashTable {
    static const int CAP = 1 << 17;
    static const int MASK = CAP - 1;
    uint16_t stamp[CAP];
    HashType keys[CAP];
    EvalType vals[CAP];
    uint16_t cur_stamp;

    FastHashTable() : cur_stamp(1) { memset(stamp, 0, sizeof(stamp)); }

    static inline uint64_t Mix(uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    inline void clear() {
        ++cur_stamp;
        if (cur_stamp == 0) {
            memset(stamp, 0, sizeof(stamp));
            cur_stamp = 1;
        }
    }

    inline EvalType* find_ptr(HashType key) {
        int pos = Mix((uint64_t)key) & MASK;
        while (stamp[pos] == cur_stamp) {
            if (keys[pos] == key) return &vals[pos];
            pos = (pos + 1) & MASK;
        }
        return nullptr;
    }

    inline void set(HashType key, EvalType val) {
        int pos = Mix((uint64_t)key) & MASK;
        while (stamp[pos] == cur_stamp) {
            if (keys[pos] == key) {
                vals[pos] = val;
                return;
            }
            pos = (pos + 1) & MASK;
        }
        stamp[pos] = cur_stamp;
        keys[pos] = key;
        vals[pos] = val;
    }
};
const int MAX_STEP_JUMP = 16;
const int ACTIVE_HASH_STEPS = 16;
const int MAX_BW = 100000;
FastHashTable a_all_hashes[ACTIVE_HASH_STEPS];
struct FastEvalHeap {
    int sz;
    EvalType heap[MAX_BW];

    inline void clear() { sz = 0; }

    inline void push(EvalType v) {
        int i = sz++;
        while (i > 0) {
            int p = (i - 1) >> 1;
            if (heap[p] >= v) break;
            heap[i] = heap[p];
            i = p;
        }
        heap[i] = v;
    }

    inline void sift_down(int i) {
        EvalType v = heap[i];
        while (true) {
            int l = i * 2 + 1;
            if (l >= sz) break;
            int r = l + 1;
            int c = (r < sz && heap[r] > heap[l]) ? r : l;
            if (heap[c] <= v) break;
            heap[i] = heap[c];
            i = c;
        }
        heap[i] = v;
    }

    inline void insert_best(EvalType v, int limit) {
        if (limit <= 0) return;
        if (sz < limit) {
            push(v);
        } else if (v < heap[0]) {
            heap[0] = v;
            sift_down(0);
        }
    }

    inline EvalType bound(int limit) const {
        return sz == limit ? heap[0] : inf;
    }
};
FastEvalHeap a_best_evals[ACTIVE_HASH_STEPS];
int at_stack[MSTEPS], last_stack[MSTEPS], last_step;
MoveType lm_stack[MSTEPS];
EvalType eval_stack[MSTEPS];
HashType hash_stack[MSTEPS]; // anything irreversible
int ap_stack[MSTEPS], l_stack[MSTEPS], atm_stack[MSTEPS];
int lm_size_stack[MSTEPS];
HashType current_hash, gh;
int BW = 100000;
vector<int> current_path;
bool load_initial = false;
int bn[8], board_pools[8][N2], w[N2];


void Add(int x) {
    int c = a[x];
    gh ^= zhx[c][x];
    w[x] = bn[c];
    board_pools[c][bn[c]++] = x;
}
void Del(int x) {
    int c = a[x];
    gh ^= zhx[c][x];
    int i = w[x];
    board_pools[c][i] = board_pools[c][--bn[c]];
    w[board_pools[c][i]] = i;
}

inline FastHashTable& GetHashTable(int step) {
    return a_all_hashes[step % ACTIVE_HASH_STEPS];
}

inline FastEvalHeap& GetBestEvalHeap(int step) {
    return a_best_evals[step % ACTIVE_HASH_STEPS];
}

void Fill() {
    for(int i=L; i<=R; i++) {
        int t = 0;
        if (i>=L+2 && i<=R-3) t = -i;
        a[b[i]] = t;
    }
}

void Load() {
    ci=0;
    L=0; R=-1;
    F0(i,5) b[++R]=id[i][0];
    F0(i,cn) a[i]=g[i];
    F1(c,C) bn[c]=0;
    F0(i,cn) if (a[i]>0) Add(i);
    Fill();
}

void InitBeam() {
    solved = false;
    for (int i = 0; i < MSTEPS; i++) beam[i].clear();
    F0(i, ACTIVE_HASH_STEPS) {
        a_all_hashes[i].clear();
        a_best_evals[i].clear();
    }
    F0(i, MSTEPS) {
        lm_stack[i] = -1;
        a_eval_bound[i] = 1e9;
        a_deltas[i].clear();
        lm_size_stack[i] = 0;
    }
    //deltas.clear();
    last_step = 0;
    at_stack[0] = 0;

    if (load_initial) {
        at = 0;
        Load();
    }
    else {
        //LoadAB();
    }

    l_stack[0] = L;
    atm_stack[0] = at;
    eval_stack[0] = 0;
    hash_stack[0] = 0;
    beam[0].push_back({-1, -1, MoveType()});
}

void RevertMove(int step, int step2, const MoveType& lm) {
    // Revert last move (lm = lm_stack[step2])
    int p = lm;
    int ap = ap_stack[step2];

    at=atm_stack[step];
    if (ap>0) {
        --ci;
        a[p]=ap;
        Add(p);
        R--;
        a[b[R-2]]=0;
    } else if (ap==0) {
        L--;
        a[b[L+2]]=-(L+2);
        R--;
        a[b[R-2]]=0;
    } else {
        if(!lm_size_stack[step2]) {
            int index = L;
            R--;
            a[b[R-2]] = 0;
            L = l_stack[step];
            for (int i=index-1;i>L;i--) { Del(b[i]); col[ci++]=a[b[i]]; }
            for (int i = L+1; i <= index+1 && i <= R-3; i++) {
                if (i >= L+2) a[b[i]] = -i;
                else a[b[i]] = 0;
            }
        } else {
            int regen=lm_size_stack[step2];
            lm_size_stack[step2]=0;
            int index = L;
            R -= regen + 1;
            a[b[R-2]] = 0;
            a[b[R-1]] = 0;
            a[b[R]] = 0;

            L = l_stack[step];
            for (int i=index-1-regen;i>L;i--) { Del(b[i]); col[ci++]=a[b[i]]; }
            for (int i = L+1; i <= index+1 && i <= R-3; i++) {
                if (i >= L+2) a[b[i]] = -i;
                else a[b[i]] = 0;
            }
        }
    }
}


int MakeMove(int step, const MoveType& lm) {
    int p = lm;
    int step2 = step + 1;
    int regen = 0;
    if (a[p]<0) {
        int index=-a[p];
        int loop_length = R - index - 3;
        if (loop_length < at) {
            regen = at - loop_length;
            step2 += regen;
            lm_size_stack[step2]=regen;
        }
    }
    assert(step2 - step <= MAX_STEP_JUMP);
    ap_stack[step2] = a[p];
    hash_stack[step2] = hash_stack[step];

    int nh = 0;
    b[++R]=p;
    a[b[R-3]]=-(R-3);
    if (a[p]>0) {
        if(at < tn && at+5==R-L && a[p]==t[at]) {
            at++;
            nh = 1;
        } else if (at < tn && a[p] == t[at]) {
            nh = 1;
        }
        Del(p);
        col[ci++]=a[p];
        a[p]=0;
    } else if (a[p]==0) {
        ++L;
        a[b[L+1]]=0;
    } else {
        int index=-a[p];
        if (!regen) {
            for (int i=L+1; i<=index-1; i++) { a[b[i]]=col[--ci]; Add(b[i]); }
            L = index;
            a[b[L]]=a[b[L+1]]=0;
            nh = 1;
        } else {
            for (int i=L+1; i<=index-1-regen; i++) { a[b[i]]=col[--ci]; Add(b[i]); }
            F1(i,regen) b[R+i]=b[index-i];
            R += regen;
            L = index;
            a[b[L + 1]] = 0;
            for (int i = R - regen - 2; i <= R; i++) {
                if (i >= L+2 && i <= R-3) a[b[i]] = -i;
                else a[b[i]] = 0;
            }
            nh = 1;
        }
    }
    if (nh) {
        if (0) {
            HashType current_hash = gh;
            for (int x : adj[b[R]]) current_hash=current_hash*10007+a[x];
            for (int i=R;i<=R;i++) current_hash=current_hash*10007+b[i];
            //for (int i=max(L,R-5-at+2);i<=min(R-5-at+3,R);i++) current_hash=current_hash*10007+b[i];
            hash_stack[step2] = current_hash;
        } else {
            hash_stack[step2] = my.zh();
        }
    }
    last_stack[step2] = step;
    l_stack[step2] = L;
    atm_stack[step2] = at;
    lm_stack[step2] = lm;
    eval_stack[step2] = eval_stack[step];
    return step2;
}

void Reach(int step, int at) {
    while (last_step > step) {
        RevertMove(last_stack[last_step], last_step, lm_stack[last_step]);
        last_step = last_stack[last_step];
    }
    if (step == last_step && at_stack[step] == at) return;
    int prev_step = beam[step][at].prev_step;
    Reach(prev_step, beam[step][at].prev);
    last_step = MakeMove(prev_step, beam[step][at].last_move);
    at_stack[step] = at;
}

int Gra(int L, int R, int i, int x1, int x2) {
    return x1 + (x2 - x1) * (i - L) / (R - L + 1);
}

void Bimsa2K() {
    pii pens = {400, 300};
    int RADD = 400;

    int PEN1 = pens.first, PEN2 = pens.second;

    double COEFF = 1;
    if (C == 3) COEFF = 0.4;
    if (C == 4) COEFF = 0.6;
    if (C == 5) COEFF = 0.8;
    if (C == 6) COEFF = 0.9;

    int E = 2;
    if (C == 3) E = 1;

    InitBeam();
    //my.reseed(time(0));

    BW = COEFF * 5000 * (12 * 12 / 2) * 5 / C / m;
    assert(BW <= MAX_BW);
    //PR(BW, bscore);
    assert(bscore < MSTEPS);
    for (short step = 0; step < bscore; step++) {
        if (Elapsed() > TL) { PR("TL", step); return; }
        if (step > 0) {
            vector<DeltaType>& deltas = a_deltas[step];
            int sz = SZ(deltas);
            if (sz > BW) {
                nth_element(deltas.begin(), deltas.begin() + BW, deltas.end(),
                    [&](const DeltaType& d1, const DeltaType& d2) {
                        return d1.first < d2.first;
                    }
                );
                sz = BW;
            }
            for (int index = 0; index < sz; index++) {
                beam[step].push_back(deltas[index].second);
            }
            sort(beam[step].begin(), beam[step].end(),
                 [&](const TreeState& t1, const TreeState& t2) {
                    if (t1.prev_step != t2.prev_step) return t1.prev_step > t2.prev_step;
                    return t1.prev > t2.prev;
                 }
            );
            deltas.clear();
        }

        //PR(step, SZ(beam[step]), SZ(a_all_hashes[step]), SZ(a_best_evals[step]));
        for (int bi = 0; bi < SZ(beam[step]); bi++) {
            Reach(step, bi);
            //if (step % 1 == 0 && bi == 0) PR(step, at, tn);
            //if (bi == 0 && SZ(sol) < R-3) sol.assign(b + 5, b + R + 1);
            if (at == tn) {
                solved = 1;
                score = step;
                sol.assign(b + 5, b + R + 1);
                PR("new solve in ", step);
                sol_found = true;
                //UpdateBest2();
                return;
            }

            int head = b[R];
            int head2 = b[R - 1];
            for (int x : adj[head]) if (x != head2) {
                if (a[x] < 0) {
                    int index=-a[x];
                    int loop_length = R - index - 3;
                    if (loop_length < at - 15) continue;
                }
                MoveType lm = x;
                int step2 = MakeMove(step, lm);

                EvalType neval = -at * 1000;
                EvalType& eval_bound = a_eval_bound[step2];

                if (at < tn && neval < eval_bound) {
                    int d1 = 1e9;
                    F0(index, bn[t[at]]) {
                        int i = board_pools[t[at]][index];
                        d1 = min(d1, d2[b[R]][i]);
                    }
                    for (int i = at; i < ci; i++) if (col[i]==t[at]) {
                        d1 = min(d1, (d2[b[R]][b[R-4-i]]+1) / 2);
                    }
                    if (at < ci) {
                        int eat_d = 0;
                        if (at % 2 == 1) {
                            eat_d = d2[b[R]][b[R-3-at]] / 2 + 2;
                        } else {
                            eat_d = d2[b[R]][b[R-4-at]] / 2 + 1;
                        }
                        d1 = max(d1, eat_d);
                    }
                    int pen = Gra(0, tn-1, at, PEN1, PEN2);
                    //int pen = 350;
                    neval += d1*pen;
                }
                neval += my.nextInt(RADD);
                if (neval < eval_bound) {
                    //current_hash = hash_stack[step + 1];
                    if (hash_stack[step] != hash_stack[step2]) {
                        current_hash = hash_stack[step2];
                    } else {
                        current_hash = ci;
                        for (int x : adj[b[R]]) current_hash=current_hash*10007+a[x];
                        for (int i=R;i<=R;i++) current_hash=current_hash*10007+b[i];
                        for (int i=max(L,R-5-at+E);i<=min(R-5-at+3,R);i++) current_hash=current_hash*10007+b[i];
                        //for(int i=at;i<ci;i++) current_hash=current_hash*10007+(col[at]==t[at]);
                    }
                    FastHashTable& all_hashes = GetHashTable(step2);
                    FastEvalHeap& best_evals = GetBestEvalHeap(step2);
                    EvalType* old_eval = all_hashes.find_ptr(current_hash);
                    if (!old_eval || *old_eval > neval) {
                        all_hashes.set(current_hash, neval);
                        a_deltas[step2].push_back({neval, {bi, step, lm}});
                        best_evals.insert_best(neval, BW);
                        eval_bound = best_evals.bound(BW);
                    }
                }
                RevertMove(step, step2, lm);
            }
        }
        //map<int, int> M; for (double x : a_best_evals[step]) M[(int)x]++; PR(step, M);
        GetBestEvalHeap(step).clear();
        GetHashTable(step).clear();
    }
}

namespace failure {
vector<int> rp, rc;
int ra[N2];
vector<int> snake, c;
int vis_cnt[N2], vis[N2], vz, w[N2], qd[N2];

void Apply(int p) {
    rp.insert(rp.begin(), p);
    if (ra[p] != 0) {
        int c = ra[p];
        ra[p] = 0;
        rc.push_back(c);
    } else {
        rp.pop_back();
    }
    for (int h = 1; h <= SZ(rp) - 2; h++) if (p == rp[h]) {
        for (int index = h + 1; index < SZ(rp); index++) {
            int c = rc[index];
            ra[rp[index]] = c;
        }
        rp.resize(h+1);
        rc.resize(h+1);
        break;
    }
}

void Partial() {
    while (SZ(sol) < 99000 && at < tn) {
        int pos = snake[0];
        int f = 0;

        if (SZ(snake) == at) {
            vector<int> q;
            q.push_back(pos);
            F0(i, cn) vis[i] = 0;
            vis[pos] = 1;
            qd[pos] = 0;
            vector<int> path;
            F0(qi, SZ(q)) {
                if (f) break;
                int p = q[qi];
                for (int p2 : adj[p]) if (!ak[p2] && !vis[p2] && ((a[p2] < 0 && -a[p2] > SZ(snake)-qd[p]-3) || a[p2] == 0 || a[p2] == t[at])) {
                    w[p2] = p;
                    qd[p2] = qd[p] + 1;
                    if (a[p2] == t[at]) {
                        while (p2 != pos) {
                            path.push_back(p2);
                            p2 = w[p2];
                        }
                        f = 1;
                        break;
                    } else {
                        vis[p2] = 1;
                        q.push_back(p2);
                    }
                }
            }
            if (f) {
                reverse(path.begin(), path.end());
                for (int new_pos : path) {
                    snake.insert(snake.begin(), new_pos);
                    if (new_pos != path.back()) {
                        snake.pop_back();
                        a[snake.back()] = 0;
                    } else {
                        a[new_pos] = 0;
                        c.push_back(t[at]);
                    }
                    sol.push_back(new_pos);
                    Apply(new_pos);
                }

                F1(i, SZ(snake) - 2) a[snake[i]] = -i;
                at++;
                F0(i, cn) vis_cnt[i] = 0;
                continue;
            }
        }

        vis_cnt[snake[0]]++;
        vector<int> cand;
        int minv = 8;
        F0(k, 4) if (nx[pos][k] != -1 && !ak[nx[pos][k]] && 0 <= a[nx[pos][k]]) {
            int new_pos = nx[pos][k];
            int cnt = vis_cnt[new_pos];
            if (a[nx[pos][k]] > 0) {
                cnt += 1000;
            }
            if (a[new_pos] < 0) cnt += SZ(snake);
            if (cnt < minv) {
                cand.clear();
                minv = cnt;
            }
            if (cnt == minv) cand.push_back(new_pos);
        }
        int np = -1;
        if (!cand.empty()) np = my.REL(cand);

        int k = my.nextInt(4);

        // -1 is previous node
        F0(sh, 4) if (nx[pos][k] != -1 && !ak[nx[pos][k]] && -1 != a[nx[pos][k]] && (nx[pos][k] == np || np == -1)) {
            if (0 == a[nx[pos][k]] || nx[pos][k] == snake[SZ(snake)-2]) {
                int new_pos = nx[pos][k];
                sol.push_back(new_pos);
                Apply(new_pos);
                snake.insert(snake.begin(), new_pos);
                snake.pop_back();
                a[snake.back()] = 0;
                F1(i, SZ(snake) - 2) a[snake[i]] = -i;
            } else {
                int new_pos = nx[pos][k];
                sol.push_back(new_pos);
                Apply(new_pos);
                if (a[new_pos] > 0) {
                    snake.insert(snake.begin(), new_pos);
                    c.push_back(a[new_pos]);
                    a[new_pos] = 0;
                    F1(i, SZ(snake) - 2) a[snake[i]] = -i;
                } else {
                    int index = -a[new_pos];

                    vector<int> to_ins;
                    to_ins.push_back(new_pos);

                    if (index+1 < at-1) {
                        for (int i = index + 1; i < at - 1; i++) {
                            to_ins.push_back(snake[i]);
                            Apply(snake[i]);
                            a[snake[i]] = 0;
                            sol.push_back(snake[i]);
                        }
                        for (int i = at; i < SZ(snake); i++) {
                            a[snake[i - 1]] = c[i];
                        }
                        for (int x : to_ins) snake.insert(snake.begin(), x);
                        snake.resize(at);
                        c.resize(at);
                    } else {
                        for (int i = index; i < SZ(snake); i++) {
                            a[snake[i - 1]] = c[i];
                        }
                        snake.insert(snake.begin(), new_pos);
                        snake.resize(index+2);
                        c.resize(index+2);
                    }

                    a[snake.back()] = 0;
                    F1(i, SZ(snake) - 2) a[snake[i]] = -i;
                }
            }
            f = 1;
            break;
        } else {
            k = (k + 1) % 4;
        }

        if (f) continue;
    }
}
void Trash() {
    tn = m;
    F0(i, tn) t[i] = target[i];
    F0(i, cn) a[i] = g[i];
    c.clear();
    snake.clear();
    F0(i, 5) {
        snake.push_back(id[4 - i][0]);
        c.push_back(1);
    }
    rp = snake;
    rc = c;
    F0(i, cn) ra[i] = g[i];
    for (int x : snake) ra[x] = 0;
    F0(i, cn) vis_cnt[i] = 0;
    F1(i, SZ(snake) - 2) a[snake[i]] = -i;
    at = 5;
    sol.clear();
    Partial();
    UpdatePart();
}}

void BB() {
    bscore = 1600;
    while (Elapsed() < TL) {
        tn = m - 5;
        F0(i, tn) t[i] = target[i + 5];
        load_initial = true;
        Bimsa2K();
        if (solved) {
            score = SZ(sol);
            UpdateBest2();
        } else if (!sol_found) {
            sol_found = 1;
            double c1 = Elapsed();
            failure::Trash();
            double c2 = Elapsed();
            PR(c2-c1);
        }
        //break;
    }
}

void Solve() {
    Prepare();

    BB();

    PR(bscore);
    F0(i, SZ(bsol)) cout << DS[bsol[i]] << endl;

    Report();
}


void ReadInput() {
    cin >> n >> m >> C;
    F0(i, m) cin >> target[i];
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
