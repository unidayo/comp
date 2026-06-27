#include <bits/stdc++.h>
using namespace std;

/*** ===== RNG / Timer ===== ***/
struct XorShift {
    uint64_t x = 88172645463325252ull;
    inline uint32_t operator()(){ x^=x<<7; x^=x>>9; return (uint32_t)x; }
    inline uint32_t next(uint32_t m){ return m ? (uint32_t)((uint64_t)(*this)()*m >> 32) : (*this)(); }
    inline double   drand(){ return (double)((*this)() & 0xFFFFFF) / 16777216.0; }
} rng;

struct Timer {
    chrono::steady_clock::time_point st;
    Timer(){ reset(); }
    inline void reset(){ st=chrono::steady_clock::now(); }
    inline double elapsed() const { return chrono::duration<double>(chrono::steady_clock::now()-st).count(); }
};

/*** ===== FastOut ===== ***/
struct FastOut {
    string s; FastOut(){ s.reserve(2'500'000); }
    inline void puti(int v){
        if(!v){ s.push_back('0'); return; }
        if(v<0){ s.push_back('-'); v=-v; }
        char b[12]; int n=0; while(v){ b[n++]=char('0'+v%10); v/=10; }
        while(n--) s.push_back(b[n]);
    }
    inline void pairln(int a,int b){ puti(a); s.push_back(' '); puti(b); s.push_back('\n'); }
    inline void flush(){ fwrite(s.data(),1,s.size(),stdout); }
};

/*** ===== Inline macro ===== ***/
#if defined(__GNUC__) || defined(__clang__)
#  define FINLINE __attribute__((always_inline)) inline
#else
#  define FINLINE inline
#endif

/*** ===== Consts ===== ***/
static constexpr int MAXN=200;
static constexpr int K_CAND=40;            // max Top-K per weapon
static constexpr double TL_SA=1.99;        // seconds for SA
static constexpr double T0=100.0, T1=0.02; // temperature
static constexpr double ALPHA0=1.0;        // clustering weight (accept-only)
static constexpr int REBUILD_TH=256;       // closure rebuild threshold

/*** ===== Input ===== ***/
int N;
int H[MAXN], C[MAXN];
int A[MAXN][MAXN];

/*** ===== 256-bit bitset (N<=200) ===== ***/
struct BS{
    static constexpr int W=4; // 256 bits
    uint64_t v[W];
    FINLINE void clear(){ v[0]=v[1]=v[2]=v[3]=0ull; }
    FINLINE void set(int i){ v[i>>6] |= 1ull<<(i&63); }
    FINLINE void reset(int i){ v[i>>6] &= ~(1ull<<(i&63)); }
    FINLINE bool test(int i) const { return (v[i>>6]>>(i&63))&1ull; }
    FINLINE void oreq(const BS& o){ v[0]|=o.v[0]; v[1]|=o.v[1]; v[2]|=o.v[2]; v[3]|=o.v[3]; }
};
template<class F>
FINLINE void for_each_bit(const BS& b, F&& f){
    for(int w=0; w<BS::W; ++w){
        uint64_t x=b.v[w];
        while(x){
            int t=__builtin_ctzll(x);
            f((w<<6)|t);
            x&=x-1;
        }
    }
}

/*** ===== Candidates (A>1 only) ===== ***/
uint16_t candB[MAXN][K_CAND]; // box id
uint16_t candA[MAXN][K_CAND]; // damage
int      candSz[MAXN];
int16_t  candIdx[MAXN][MAXN]; // [w][b] -> k or -1

/*** ===== Assignment / Score ===== ***/
uint8_t  xcnt[MAXN][K_CAND];
uint8_t  usedW[MAXN];
int      targetCntW[MAXN];
int      usedShotsTotal=0;

int      Dbox[MAXN];
int      Sbox[MAXN];
long long deficitSum=0;

/*** ===== DAG closure ===== ***/
BS adjOut[MAXN];
BS reachF[MAXN], reachR[MAXN];
bool hasEdgeWK[MAXN][K_CAND];
int  dirtyRem=0;

FINLINE void closure_rebuild(){
    for(int i=0;i<N;++i) reachF[i]=adjOut[i];
    for(int k=0;k<N;++k) for(int i=0;i<N;++i) if (reachF[i].test(k)) reachF[i].oreq(reachF[k]);
    for(int i=0;i<N;++i) reachR[i].clear();
    for(int i=0;i<N;++i) for_each_bit(reachF[i], [&](int j){ reachR[j].set(i); });
    dirtyRem=0;
}
FINLINE bool can_add_edge(int u,int v){
    if (u==v) return false;
    if (reachF[v].test(u)) return false;
    return true;
}
FINLINE void add_edge_monotone(int u,int v){
    if (adjOut[u].test(v)) return;
    adjOut[u].set(v);
    BS U=reachR[u]; U.set(u);
    BS V=reachF[v]; V.set(v);
    for_each_bit(U, [&](int i){ reachF[i].oreq(V); });
    for_each_bit(V, [&](int j){ reachR[j].oreq(U); });
}
FINLINE void remove_edge_lazy(int u,int v){
    if (!adjOut[u].test(v)) return;
    adjOut[u].reset(v);
    if (++dirtyRem>=REBUILD_TH) closure_rebuild();
}

/*** ===== ΔS helpers ===== ***/
FINLINE int dS_add_one(int b,int a){
    int oldS=Sbox[b];
    int newD=Dbox[b]+a;
    int newS=H[b]-newD; if (newS<0) newS=0;
    return newS-oldS; // <=0
}
FINLINE int dS_remove_one(int b,int a){
    int oldS=Sbox[b];
    int newD=Dbox[b]-a;
    int newS=H[b]-newD; if (newS<0) newS=0;
    return newS-oldS; // >=0（ただしoverkill部分は0）
}
FINLINE int dS_add_bulk(int b,int a,int t){
    long long oldS=Sbox[b];
    long long dec=min<long long>(oldS, 1LL*a*t);
    return (int)(-dec);
}
FINLINE int dS_remove_bulk(int b,int a,int t){
    int D=Dbox[b];
    int over=max(0, D-H[b]);
    int R=a*t;
    if (R<=over) return 0;
    return R-over;
}

/*** ===== Apply (mutations) ===== ***/
FINLINE void apply_add_one(int w,int k){
    int b=candB[w][k], a=candA[w][k];
    if (!hasEdgeWK[w][k]){ add_edge_monotone(w,b); hasEdgeWK[w][k]=true; targetCntW[w]++; }
    int d=dS_add_one(b,a);
    Dbox[b]+=a; Sbox[b]+=d; deficitSum+=d;
    xcnt[w][k]++; usedW[w]++; usedShotsTotal++;
}
FINLINE void apply_remove_one(int w,int k){
    int b=candB[w][k], a=candA[w][k];
    int d=dS_remove_one(b,a);
    Dbox[b]-=a; Sbox[b]+=d; deficitSum+=d;
    xcnt[w][k]--; usedW[w]--; usedShotsTotal--;
    if (xcnt[w][k]==0){ hasEdgeWK[w][k]=false; targetCntW[w]--; remove_edge_lazy(w,b); }
}
FINLINE void apply_add_bulk_existing(int w,int k,int t){
    int b=candB[w][k], a=candA[w][k];
    int d=dS_add_bulk(b,a,t);
    Dbox[b]+=a*t; Sbox[b]+=d; deficitSum+=d;
    xcnt[w][k]+=t; usedW[w]+=t; usedShotsTotal+=t;
}
FINLINE void apply_remove_bulk(int w,int k,int t){
    int b=candB[w][k], a=candA[w][k];
    int d=dS_remove_bulk(b,a,t);
    Dbox[b]-=a*t; Sbox[b]+=d; deficitSum+=d;
    xcnt[w][k]-=t; usedW[w]-=t; usedShotsTotal-=t;
    if (xcnt[w][k]==0){ hasEdgeWK[w][k]=false; targetCntW[w]--; remove_edge_lazy(w,b); }
}

/*** ===== Accept helper ===== ***/
FINLINE bool accept(double delta, double temp){
    if (delta<=0.0) return true;
    if (delta>=8.0*temp) return false;
    return rng.drand() < exp(-delta/temp);
}

/*** ===== Build candidates (A>1) ===== ***/
void build_candidates(){
    for(int w=0;w<N;++w) for(int b=0;b<N;++b) candIdx[w][b]=-1;
    vector<pair<int,int>> v; v.reserve(MAXN);
    for(int w=0;w<N;++w){
        v.clear();
        for(int b=0;b<N;++b){ int a=A[w][b]; if (a>1) v.emplace_back(a,b); }
        if ((int)v.size()>K_CAND){
            nth_element(v.begin(), v.begin()+K_CAND, v.end(),
                [](auto&x,auto&y){ return x.first>y.first; });
            v.resize(K_CAND);
        }
        sort(v.begin(), v.end(), [](auto&x,auto&y){ return x.first>y.first; });
        int cap=min(K_CAND, 4*C[w]+8); // auto shrink by durability
        candSz[w]=min((int)v.size(), cap);
        for(int k=0;k<candSz[w];++k){
            candA[w][k]=(uint16_t)v[k].first;
            candB[w][k]=(uint16_t)v[k].second;
            candIdx[w][ v[k].second ]=k;
        }
        for(int k=candSz[w];k<K_CAND;++k){ candA[w][k]=0; candB[w][k]=0xFFFF; }
    }
}

/*** ===== Initial greedy ===== ***/
void build_initial(){
    memset(xcnt,0,sizeof(xcnt));
    memset(usedW,0,sizeof(usedW));
    memset(targetCntW,0,sizeof(targetCntW));
    memset(hasEdgeWK,0,sizeof(hasEdgeWK));
    usedShotsTotal=0;
    for(int i=0;i<N;++i){ Dbox[i]=0; Sbox[i]=H[i]; adjOut[i].clear(); reachF[i].clear(); reachR[i].clear(); }
    deficitSum=0; for(int i=0;i<N;++i) deficitSum+=Sbox[i];
    dirtyRem=0;

    struct It{int a,w,k;};
    vector<It> all; all.reserve(N*K_CAND);
    for(int w=0;w<N;++w) for(int k=0;k<candSz[w];++k){
        int b=candB[w][k]; if (b==w) continue; all.push_back({candA[w][k],w,k});
    }
    sort(all.begin(), all.end(), [](const It&x,const It&y){ return x.a>y.a; });

    for(const auto &it: all){
        int w=it.w,k=it.k,b=candB[w][k],a=candA[w][k];
        if (usedW[w]>=C[w]) continue;
        int deltaReal = dS_add_one(b,a) + 1;
        if (deltaReal>=0) continue;
        if (!hasEdgeWK[w][k] && !can_add_edge(w,b)) continue;
        apply_add_one(w,k);
    }
}

/*** ===== SA ===== ***/
long long SA_ITER=0;

void simulated_annealing(){
    Timer tim;
    const int P_ADD=32, P_MOVE=25, P_COAL1=18, P_REM=5, P_BULK_ADD=10, P_BULK_COAL=10;
    const int P_SUM=P_ADD+P_MOVE+P_COAL1+P_REM+P_BULK_ADD+P_BULK_COAL;

    while(true){
        double el=tim.elapsed(); if (el>TL_SA) break;
        double prog=el/TL_SA;
        double temp=T0*pow(T1/T0, prog);
        double alpha=ALPHA0*(1.0-prog);

        int mv=rng.next(P_SUM);

        if (mv < P_ADD){
            int w=rng.next(N);
            if (usedW[w]>=C[w] || candSz[w]==0){ ++SA_ITER; continue; }
            int best_k=-1; int bestReal=0; int bestCl=0;
            for(int t=0;t<6;++t){
                int k=rng.next(candSz[w]);
                int b=candB[w][k], a=candA[w][k];
                if (!hasEdgeWK[w][k] && !can_add_edge(w,b)) continue;
                int dR=dS_add_one(b,a)+1;
                int dC=!hasEdgeWK[w][k] ? +1 : 0;
                if (best_k==-1 || dR+(dC<0?-1:0) < bestReal+(bestCl<0?-1:0)){ best_k=k; bestReal=dR; bestCl=dC; }
            }
            if (best_k==-1){ ++SA_ITER; continue; }
            int b=candB[w][best_k], a=candA[w][best_k];
            double deltaAug=(double)(dS_add_one(b,a)+1) + alpha*(double)(!hasEdgeWK[w][best_k]? +1:0);
            if (accept(deltaAug, temp)) apply_add_one(w,best_k);

        } else if (mv < P_ADD+P_MOVE){
            int w=rng.next(N);
            if (usedW[w]==0 || candSz[w]<=1){ ++SA_ITER; continue; }
            int k1=-1;
            for(int t=0;t<6;++t){ int kk=rng.next(candSz[w]); if (xcnt[w][kk]>0){ k1=kk; break; } }
            if (k1==-1){ for(int kk=0;kk<candSz[w];++kk) if (xcnt[w][kk]>0){ k1=kk; break; } if (k1==-1){ ++SA_ITER; continue; } }
            int k2=-1; int bestReal=0; int bestCl=0;
            for(int t=0;t<12;++t){
                int kk=rng.next(candSz[w]); if (kk==k1) continue;
                int b2=candB[w][kk];
                if (!hasEdgeWK[w][kk] && !can_add_edge(w,b2)) continue;
                int b1=candB[w][k1];
                int dR = dS_remove_one(b1,candA[w][k1]) + dS_add_one(b2,candA[w][kk]);
                int dC = (xcnt[w][k1]==1 ? -1:0) + (!hasEdgeWK[w][kk]? +1:0);
                if (k2==-1 || dR+(dC<0?-1:0) < bestReal+(bestCl<0?-1:0)){ k2=kk; bestReal=dR; bestCl=dC; }
            }
            if (k2==-1){ ++SA_ITER; continue; }
            int b1=candB[w][k1], b2=candB[w][k2];
            double deltaAug=(double)(dS_remove_one(b1,candA[w][k1]) + dS_add_one(b2,candA[w][k2]))
                           + alpha*(double)((xcnt[w][k1]==1?-1:0)+(!hasEdgeWK[w][k2]?+1:0));
            if (accept(deltaAug, temp)){ apply_remove_one(w,k1); apply_add_one(w,k2); }

        } else if (mv < P_ADD+P_MOVE+P_COAL1){
            int w=rng.next(N);
            if (targetCntW[w]<2){ ++SA_ITER; continue; }
            int kf=-1;
            for(int t=0;t<6;++t){ int kk=rng.next(candSz[w]); if (xcnt[w][kk]>0){ kf=kk; break; } }
            if (kf==-1){ for(int kk=0;kk<candSz[w];++kk) if (xcnt[w][kk]>0){ kf=kk; break; } if (kf==-1){ ++SA_ITER; continue; } }
            int kt=-1;
            for(int t=0;t<12;++t){ int kk=rng.next(candSz[w]); if (kk!=kf && xcnt[w][kk]>0){ kt=kk; break; } }
            if (kt==-1){ for(int kk=0;kk<candSz[w];++kk) if (kk!=kf && xcnt[w][kk]>0){ kt=kk; break; } if (kt==-1){ ++SA_ITER; continue; } }
            int b1=candB[w][kf], b2=candB[w][kt];
            double deltaAug=(double)(dS_remove_one(b1,candA[w][kf]) + dS_add_one(b2,candA[w][kt]))
                           + alpha*(double)(xcnt[w][kf]==1 ? -1:0);
            if (accept(deltaAug, temp)){ apply_remove_one(w,kf); apply_add_one(w,kt); }

        } else if (mv < P_ADD+P_MOVE+P_COAL1+P_REM){
            int w=rng.next(N); if (usedW[w]==0){ ++SA_ITER; continue; }
            int k=-1; for(int t=0;t<6;++t){ int kk=rng.next(candSz[w]); if (xcnt[w][kk]>0){ k=kk; break; } }
            if (k==-1){ for(int kk=0;kk<candSz[w];++kk) if (xcnt[w][kk]>0){ k=kk; break; } if (k==-1){ ++SA_ITER; continue; } }
            int b=candB[w][k];
            double deltaAug=(double)(dS_remove_one(b,candA[w][k]) - 1) + alpha*(double)(xcnt[w][k]==1 ? -1:0);
            if (accept(deltaAug, temp)) apply_remove_one(w,k);

        } else if (mv < P_ADD+P_MOVE+P_COAL1+P_REM+P_BULK_ADD){
            int w=rng.next(N);
            int rem=C[w]-usedW[w];
            if (rem<=0){ ++SA_ITER; continue; }
            int kt=-1; for(int t=0;t<8;++t){ int kk=rng.next(candSz[w]); if (xcnt[w][kk]>0){ kt=kk; break; } }
            if (kt==-1){ ++SA_ITER; continue; }
            int tadd=1+(int)rng.next(rem);
            int b=candB[w][kt], a=candA[w][kt];
            double deltaAug=(double)(dS_add_bulk(b,a,tadd)+tadd); // clustering 0
            if (accept(deltaAug, temp)) apply_add_bulk_existing(w,kt,tadd);

        } else {
            int w=rng.next(N);
            if (targetCntW[w]<2){ ++SA_ITER; continue; }
            int kt=-1, bestC=-1;
            for(int t=0;t<6;++t){ int kk=rng.next(candSz[w]); if (xcnt[w][kk]>0){ int c=xcnt[w][kk]; if (c>bestC){bestC=c; kt=kk;} } }
            if (kt==-1){ for(int kk=0;kk<candSz[w];++kk) if (xcnt[w][kk]>0){ int c=xcnt[w][kk]; if (c>bestC){bestC=c; kt=kk;} } if (kt==-1){ ++SA_ITER; continue; } }
            int bto=candB[w][kt], ato=candA[w][kt];
            int moveShots=0, deltaReal=0, dCl=0;
            for(int k=0;k<candSz[w];++k){
                if (k==kt) continue;
                int cnt=xcnt[w][k]; if (!cnt) continue;
                int b=candB[w][k], a=candA[w][k];
                deltaReal += dS_remove_bulk(b,a,cnt);
                moveShots += cnt;
                if (cnt==xcnt[w][k]) dCl -= 1;
            }
            if (moveShots==0){ ++SA_ITER; continue; }
            deltaReal += dS_add_bulk(bto, ato, moveShots);
            double deltaAug=(double)deltaReal + alpha*(double)dCl;
            if (accept(deltaAug, temp)){
                for(int k=0;k<candSz[w];++k){
                    if (k==kt) continue;
                    int cnt=xcnt[w][k];
                    if (cnt) apply_remove_bulk(w,k,cnt);
                }
                apply_add_bulk_existing(w,kt,moveShots);
            }
        }
        ++SA_ITER;
        if ((SA_ITER & 4095)==0 && dirtyRem) closure_rebuild();
    }
    if (dirtyRem) closure_rebuild();
}

/*** ===== Overkill shrink (optional) ===== ***/
void shrink_overkill(){
    for(int b=0;b<N;++b){
        if (Sbox[b]>0) continue;
        struct It{int a,w,k;};
        vector<It> v; v.reserve(16);
        int sumD=Dbox[b];
        for(int w=0;w<N;++w){
            int k=candIdx[w][b]; if (k<0) continue;
            int c=xcnt[w][k]; while(c--) v.push_back({candA[w][k],w,k});
        }
        sort(v.begin(), v.end(), [](auto&x,auto&y){ return x.a<y.a; });
        for(auto &it: v){
            if (sumD - it.a >= H[b]){ apply_remove_one(it.w, it.k); sumD -= it.a; }
            else break;
        }
    }
}

/*** ===== Emit ===== ***/
void emit_solution(FastOut &out){
    int remH[MAXN]; bool opened[MAXN]={0};
    for(int b=0;b<N;++b) remH[b]=H[b];
    uint8_t exec[MAXN][K_CAND]; uint8_t execW[MAXN]; memset(exec,0,sizeof(exec)); memset(execW,0,sizeof(execW));
    for(int w=0;w<N;++w) for(int k=0;k<candSz[w];++k){ exec[w][k]=xcnt[w][k]; execW[w]+=xcnt[w][k]; }

    auto open_by_hand=[&](int b){ while(remH[b]>0){ out.pairln(-1,b); remH[b]--; } opened[b]=true; };
    auto fire_weapon=[&](int w, deque<int>&Q){
        if (execW[w]==0) return;
        for(int k=0;k<candSz[w];++k){
            int cnt=exec[w][k]; if(!cnt) continue;
            int b=candB[w][k], a=candA[w][k];
            while(cnt>0 && remH[b]>0){
                out.pairln(w,b);
                remH[b]-=a;
                exec[w][k]--; execW[w]--; cnt=exec[w][k];
                if (remH[b]<=0 && !opened[b]){ opened[b]=true; Q.push_back(b); }
            }
        }
    };

    int indeg[MAXN]={0};
    for(int u=0;u<N;++u) for(int v=0;v<N;++v) if (adjOut[u].test(v)) indeg[v]++;

    deque<int> Q;
    for(int b=0;b<N;++b) if (indeg[b]==0 && !opened[b]){ open_by_hand(b); Q.push_back(b); }
    while(true){
        while(!Q.empty()){
            int w=Q.front(); Q.pop_front();
            if (!opened[w]) open_by_hand(w);
            fire_weapon(w,Q);
            for(int v=0;v<N;++v) if (adjOut[w].test(v)) if (--indeg[v]==0 && !opened[v] && remH[v]>0){ open_by_hand(v); Q.push_back(v); }
        }
        bool all=true;
        for(int b=0;b<N;++b) if(!opened[b]){ all=false; if(indeg[b]==0 && remH[b]>0){ open_by_hand(b); Q.push_back(b); break; } }
        if (all) break;
    }
}

/*** ===== main ===== ***/
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if(!(cin>>N)) return 0;
    for(int i=0;i<N;++i) cin>>H[i];
    for(int i=0;i<N;++i) cin>>C[i];
    for(int i=0;i<N;++i) for(int j=0;j<N;++j) cin>>A[i][j];

    build_candidates();    // A=1 除外
    build_initial();       // DAG可行の強打貪欲
    simulated_annealing(); // 自由度高いDAGで焼く（序盤は集約バイアス）
    shrink_overkill();     // 過剰発の微削減

    FastOut out; emit_solution(out); out.flush();
    fprintf(stderr, "#$iter=%lld\n", (long long)SA_ITER);
    return 0;
}
