#pragma GCC target("avx2")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")
#include <immintrin.h>
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;

const double TL_ratio=1;

int profile_CNT0;
int profile_CNT1;
int profile_CNT2;

typedef long long ll;
typedef unsigned long long ull;

#define mp make_pair
#define fr first
#define sc second
#define chmin(a,b) a=min(a,b)
#define chmax(a,b) a=max(a,b)
#define rep_(_1,_2,_3,NAME,...) NAME
#define rep(...) rep_(__VA_ARGS__, rep2, rep1)(__VA_ARGS__)
#define rep1(i,j) for(int i=0;i<(j);++i)
#define rep2(i,j,k) for(int i=(j);i<(k);++i)

struct UF{
    vector<int> par,r;
    UF(int n){ init(n); }
    UF(){}
    void init(int n=-1){
        if(n==-1)n=par.size();
        par.resize(n);
        r.resize(n);
        iota(par.begin(),par.end(),0);
        fill(r.begin(),r.end(),0);
    }
    int find(int x){
        if(par[x]==x)return x;
        return par[x]=find(par[x]);
    }
    bool same(int x,int y){ return find(x)==find(y); }
    void unit(int x,int y){
        if(same(x,y))return;
        x=find(x);
        y=find(y);
        if(r[x]>r[y])swap(x,y);
        par[x]=y;
        if(r[x]==r[y])r[y]++;
    }
};

unsigned int RAND(void) {
  static unsigned int y = 2463534242;
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}

int N,M,D,K;
int u[3000],v[3000],w[3000];
int x[1000],y[1000];
vector<int> nex[1000];
int nex_arr[1000][20]; int deg[1000];

struct ans{
    bitset<3000> bs[30];
    void set_random(){
        vector<int> cnt(D);
        rep(i,M){
            int r=RAND()%D; while(cnt[r]==K)r=RAND()%D;
            bs[r][i]=true;
            cnt[r]++;
        }
    }
    void output(){
        rep(i,M){
            rep(j,D){
                if(bs[j][i]){ printf("%d%c",j+1,(i+1==M)?'\n':' '); break; }
            }
        }
    }
};

struct sol1{
    bitset<3000> bs[30];
    using score_t=int;
    score_t now_score;
    vector<int> num;
    void calc_sc(int i){
        num[i]=0;
        UF uf(N);
        rep(j,M){
            if(!bs[i][j])uf.unit(u[j],v[j]);
        }
        rep(j,N)if(uf.find(j)==j)num[i]++;
    }
    score_t score(){
        score_t ret=0;
        rep(i,D){
            ret+=num[i];
        }
        return ret;
    }
    void MOVE(int i,int s,int j){
        bs[i][s]=false;
        bs[j][s]=true;
        calc_sc(i);
        calc_sc(j);
    }
    void yama_MOVE(){
        int s=RAND()%M;
        int i=0; while(!bs[i][s])i++;
        if(bs[i].count()+K*(D-1)==M)return;
        int j=RAND()%D; while(i==j||bs[j].count()==K)j=RAND()%D;
        MOVE(i,s,j);
        if(now_score>score())now_score=score();
        else {
            MOVE(j,s,i);
        }
    }
    /*void SWAP(int i,int s,int j,int t){
        bs[i][s]=false; bs[i][t]=true;
        bs[j][s]=true; bs[j][t]=false;
    }
    void yama_SWAP(){
        int s=RAND()%M;
        int t=RAND()%M; while(t==s)t=RAND()%M;
        int i=0; while(!bs[i][s])i++;
        int j=0; while(!bs[j][t])j++; if(i==j)return;
        SWAP(i,s,j,t);
        if(now_score>score())now_score=score();
        else {
            SWAP(i,t,j,s);
        }
    }*/
    void set_random(){
        vector<int> cnt(D);
        rep(i,M){
            int r=RAND()%D; while(cnt[r]==K)r=RAND()%D;
            bs[r][i]=true;
            cnt[r]++;
        }
        num.resize(D);
        rep(i,D)calc_sc(i);
        now_score=score();
    }
    void output(){
        rep(i,M){
            rep(j,D){
                if(bs[j][i]){ printf("%d%c",j+1,(i+1==M)?'\n':' '); break; }
            }
        }
    }
};

//priority_queue
const int INF=1000000000;
const int pq_INF=INF+1024;
__m128i pq[2000];
void pq_init(){
    rep(i,2*N)pq[i] = _mm_set_epi32(0,0,0,pq_INF);
}
void pq_upd_dec(int d){
    int p=(d&1023)+N;
    pq[p] = _mm_set_epi32(0,0,0,d);
    for(int j=1;j<=10;++j){
        pq[p>>j] = _mm_min_epi32(pq[p], pq[p>>j]);
    }
}
inline void pq_set(int d){
    pq[(d&1023)+N] = _mm_set_epi32(0,0,0,d);
}
void pq_build(int d){
    int p=d+N;
    for(int j=1;j<=10;++j){
        pq[p>>j] = _mm_min_epi32(pq[p], pq[p>>j]);
    }
}
inline int pq_top(){ return _mm_extract_epi32(pq[1], 0); }
inline bool pq_empty(){ return pq_top()==pq_INF; }
void pq_pop(){
    int p=(pq_top()&1023)+N;
    pq[p] = _mm_set_epi32(0,0,0,pq_INF);
    for(int j=1;j<=10;++j){
        pq[p>>j] = _mm_min_epi32(pq[p>>(j-1)],pq[(p>>(j-1))^1]);
    }
}

vector<vector<int>> dist_G;
vector<vector<int>> pre_edge_G;
void init_dist(){
    const int mask=(1<<30)-1-1023;
    dist_G=vector<vector<int>>(N,vector<int>(N,INF));
    pre_edge_G=vector<vector<int>>(N,vector<int>(N,-1));
    rep(i,N){
        auto &d=dist_G[i];
        auto &pe=pre_edge_G[i];
        bitset<1000> used; used.reset();
        //priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> que;
        d[i]=0;
        //que.push(mp(0,i));
        pq_upd_dec(i);
        //while(!que.empty())
        while(!pq_empty())
        {
            //auto p=que.top(); que.pop();
            int p=pq_top()&1023; pq_pop();
            if(used[p])continue;
            used[p]=true;
            for(auto ed: nex[p]){
                int to=u[ed]+v[ed]-p;
                if(d[to]>d[p]+w[ed]){
                    d[to]=d[p]+w[ed];
                    pe[to]=ed;
                    pq_upd_dec((d[to]&mask)+to);
                    //que.push(mp(d[to],to));
                }
            }
        }
    }
}

bitset<1000> score_day_s_erase_used;
bitset<1000> score_day_s_add_used;

struct sol2{
    bitset<3000> bs[30];
    using score_t=ll;
    score_t now_score;
    vector<int> start_points;
    vector<vector<ll>> dist_sum;
    vector<vector<vector<int>>> dist;
    vector<vector<vector<int>>> pre_edge;
    vector<int> wt;
    void score_day_s(int i,int j){
        const int mask=(1<<30)-1-1023;
        auto &d=dist[i][j];
        rep(k,N)d[k]=INF;
        vector<bool> used(N,false);
        auto &pe=pre_edge[i][j]; pe=vector<int>(N,-1);
        //priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> que;
        d[start_points[j]]=0;
        pq_upd_dec(start_points[j]);
        //que.push(mp(d[start_points[j]],start_points[j]));
        //while(!que.empty())
        while(!pq_empty())
        {
            //auto p=que.top(); que.pop();
            int p=pq_top()&1023; pq_pop();
            if(used[p])continue;
            used[p]=true;
            for(auto ed: nex[p]){
                if(bs[i][ed])continue;
                int to=u[ed]+v[ed]-p;
                if(d[to]>d[p]+w[ed]){
                    d[to]=d[p]+w[ed];
                    pe[to]=ed;
                    pq_upd_dec((d[to]&mask)+to);
                    //que.push(mp(d[to],to));
                }
            }
        }
        dist_sum[i][j]=0;
        rep(k,N)dist_sum[i][j]+=d[k];
    }
    void score_day(int i){
        rep(j,start_points.size())score_day_s(i,j);
    }
    vector<int> score_init_d;
    void score_init(int S=19){
        ll DIST_SUM=INF*(ll)N;
        clock_t START=clock();
        vector<int> v_cent;
        //rep(i,N)if(x[i]*x[i]+y[i]*y[i]<=900*900)v_cent.push_back(i);
        rep(qqq,N){
            score_init_d=dist_G[qqq];
            vector<int> sp(1,qqq);
            while(sp.size()<S){
                auto &d=score_init_d;
                pair<int,vector<int>> temp(-1,vector<int>());
                rep(i,N)
                //for(auto i: v_cent)
                {
                    if(temp.fr<d[i]){
                        temp=mp(d[i],vector<int>(1,i));
                    }
                    else if(temp.fr==d[i])temp.sc.push_back(i);
                }
                int v=temp.sc[RAND()%temp.sc.size()];
                rep(i,N)chmin(d[i],dist_G[v][i]);
                sp.push_back(v);
            }
            ll DS=0;
            rep(i,N)DS+=score_init_d[i];
            //rep(i,N)chmax(DS,(ll)score_init_d[i]);
            if(DS<DIST_SUM){
                DIST_SUM=DS;
                start_points=sp;
            }
        }
        double TEMP=600000.;
        rep(qqq,20000){
            TEMP*=0.99975;
            vector<int> sp=start_points;
            int r=RAND()%S;
            int ed=nex[sp[r]][RAND()%nex[sp[r]].size()];
            sp[r]=u[ed]+v[ed]-sp[r];
            vector<int> d(N,INF);
            for(int vv: sp){
                rep(i,N)d[i]=min(d[i],dist_G[vv][i]);
            }
            ll DS=0;
            rep(i,N)DS+=d[i];
            //rep(i,N)chmax(DS,(ll)score_init_d[i]);
            if(DS<DIST_SUM||RAND()%10000<exp((DIST_SUM-DS)/TEMP)*10000){
                DIST_SUM=DS;
                start_points=sp;
                //cerr<<qqq<<" "<<DIST_SUM<<endl;
            }
        }
        //cerr<<clock()-START<<" "<<DIST_SUM<<endl;
        dist_sum=vector<vector<ll>>(D,vector<ll>(100));
        dist=vector<vector<vector<int>>>(D,vector<vector<int>>(100,vector<int>(N)));
        pre_edge=vector<vector<vector<int>>>(D,vector<vector<int>>(100,vector<int>(N)));
        wt=vector<int>(start_points.size(),0);
        rep(i,N){
            pair<int,int> temp(INF,-1);
            rep(j,start_points.size()){
                chmin(temp,mp(dist_G[start_points[j]][i],j));
            }
            wt[temp.sc]++;
        }
        rep(i,D)score_day(i);
        now_score=score();
    }
    /*void start_points_add(){
        if(1){
            auto &d=score_init_d;
            pair<int,vector<int>> temp(-1,vector<int>());
            rep(i,N){
                if(temp.fr<d[i]){
                    temp=mp(d[i],vector<int>(1,i));
                }
                else if(temp.fr==d[i])temp.sc.push_back(i);
            }
            int v=temp.sc[RAND()%temp.sc.size()];
            rep(i,N)chmin(d[i],dist_G[v][i]);
            start_points.push_back(v);
        }
        wt=vector<int>(start_points.size(),0);
        rep(i,N){
            pair<int,int> temp(INF,-1);
            rep(j,start_points.size()){
                chmin(temp,mp(dist_G[start_points[j]][i],j));
            }
            wt[temp.sc]++;
        }
        rep(i,D)score_day_s(i,start_points.size()-1);
        now_score=score();
    }*/
    score_t score(){
        score_t ret=0;
        rep(i,D)rep(j,start_points.size())ret+=dist_sum[i][j]*wt[j];
        return ret;
    }
    void SWAP(int i,int s,int j,int t){
        ERASE(i,s);
        ERASE(j,t);
        ADD(i,t);
        ADD(j,s);
    }
    void yama_SWAP(){
        int s=RAND()%M;
        int t=RAND()%M; while(t==s)t=RAND()%M;
        int i=0; while(!bs[i][s])i++;
        int j=0; while(!bs[j][t])j++; if(i==j)return;
        SWAP(i,s,j,t);
        if(now_score>score())now_score=score();
        else {
            SWAP(i,t,j,s);
        }
    }
    void score_day_s_erase(int i,int j,int s){
        const int mask=(1<<30)-1-1023;
        auto &d=dist[i][j];
        auto &sum=dist_sum[i][j];
        auto &used=score_day_s_erase_used; used.reset();
        auto &pe=pre_edge[i][j];
        if(d[u[s]]>d[v[s]]+w[s]){
            sum-=d[u[s]];
            d[u[s]]=d[v[s]]+w[s];
            sum+=d[u[s]];
            pe[u[s]]=s;
            pq_upd_dec((d[u[s]]&mask)+u[s]);
        }
        else {
            sum-=d[v[s]];
            d[v[s]]=d[u[s]]+w[s];
            sum+=d[v[s]];
            pe[v[s]]=s;
            pq_upd_dec((d[v[s]]&mask)+v[s]);
        }
        while(!pq_empty())
        {
            int p=pq_top()&1023; pq_pop();
            if(used[p])continue;
            used[p]=true;
            for(auto ed: nex[p]){
                if(bs[i][ed])continue;
                int to=u[ed]+v[ed]-p;
                if(d[to]>d[p]+w[ed]){
                    sum-=d[to];
                    d[to]=d[p]+w[ed];
                    sum+=d[to];
                    pe[to]=ed;
                    pq_upd_dec((d[to]&mask)+to);
                }
            }
        }
    }
    void ERASE(int i,int s){
        bs[i][s]=false;
        rep(j,start_points.size()){
            if(abs(dist[i][j][u[s]]-dist[i][j][v[s]])>w[s])score_day_s_erase(i,j,s);
        }
    }
    bool score_day_s_add(int i,int j,int s){
        const int mask=(1<<30)-1-1023;
        auto &d=dist[i][j];
        auto &sum=dist_sum[i][j];
        auto &used=score_day_s_add_used; used.reset();
        auto &pe=pre_edge[i][j];
        static int vs[1000]; int vs_sz=0; int nex_vs=0;
        if(1){
            if(pe[u[s]]==s){ vs[vs_sz++]=u[s]; sum-=d[u[s]]; d[u[s]]=INF; pe[u[s]]=-1; }
            else { vs[vs_sz++]=v[s]; sum-=d[v[s]]; d[v[s]]=INF; pe[v[s]]=-1; }
            while(nex_vs<vs_sz){
                int q=vs[nex_vs++];
                for(int ed: nex[q])
                {
                    int to=u[ed]+v[ed]-q;
                    if(pe[to]==ed){
                        sum-=d[to];
                        d[to]=INF;
                        pe[to]=-1;
                        vs[vs_sz++]=to;
                    }
                }
            }
        }
        if(1){
            rep(vs_i,vs_sz){
                int k=vs[vs_i];
                for(int ed: nex[k])
                {
                    if(bs[i][ed])continue;
                    int to=u[ed]+v[ed]-k;
                    if(d[k]>d[to]+w[ed]){
                        d[k]=d[to]+w[ed];
                        pe[k]=ed;
                        //pq_upd_dec((d[k]&mask)+k);
                        pq_set((d[k]&mask)+k);
                    }
                }
            }
        }
        rep(vs_i,vs_sz)pq_build(vs[vs_i]);
        while(!pq_empty())
        {
            int p=pq_top()&1023; pq_pop();
            if(used[p])continue;
            used[p]=true;
            for(auto ed: nex[p])
            {
                if(bs[i][ed])continue;
                int to=u[ed]+v[ed]-p;
                if(d[to]>d[p]+w[ed]){
                    d[to]=d[p]+w[ed];
                    pe[to]=ed;
                    pq_upd_dec((d[to]&mask)+to);
                }
            }
        }
        bool ret=true;
        rep(vs_i,vs_sz){
            sum+=d[vs[vs_i]];
            ret&=d[vs[vs_i]]!=INF;
        }
        return ret;
    }
    void ADD(int i,int s){
        bs[i][s]=true;
        rep(j,start_points.size()){
            if(pre_edge[i][j][u[s]]==s||pre_edge[i][j][v[s]]==s){
                score_day_s_add(i,j,s);
            }
        }
    }
    bool ADD2(int i,int s){
        bs[i][s]=true;
        rep(j,start_points.size()){
            if(pre_edge[i][j][u[s]]==s||pre_edge[i][j][v[s]]==s){
                if(!score_day_s_add(i,j,s)){
                    bs[i][s]=false;
                    score_day_s_erase(i,j,s);
                    return false;
                }
            }
        }
        return true;
    }
    void MOVE(int i,int s,int j){
        ERASE(i,s);
        ADD(j,s);
    }
    bool MOVE2(int i,int s,int j){
        if(i==j)return true;
        if(!ADD2(j,s))return false;
        ERASE(i,s);
        return true;
    }
    void yaki_MOVE(clock_t T){
        //profile_CNT0++;
        double X=log(2.)*pow(10.,(6800000.*TL_ratio-T)/(1000000.*TL_ratio));
        int s=RAND()%M;
        int i=0; while(!bs[i][s])i++;
        if(bs[i].count()+K*(D-1)==M)return;
        int j=RAND()%D; while(i==j||bs[j].count()==K)j=RAND()%D;
        if(bs[j].count()<K){
            MOVE(i,s,j);
            if(now_score>score()||RAND()%10000<exp((now_score-score())/X)*10000){
                now_score=score();
            }
            else {
                MOVE(j,s,i);
            }
        }
        else {
            int t=RAND()%M; while(!bs[j][t])t=RAND()%M;
            SWAP(i,s,j,t);
            if(now_score>score()||RAND()%10000<exp((now_score-score())/X)*10000){
                now_score=score();
            }
            else {
                SWAP(i,t,j,s);
            }
        }
    }
    void K_fit(clock_t T){
        //profile_CNT1++;
        int i=RAND()%D; while(bs[i].count()<=K)i=RAND()%D;
        int s=RAND()%M; while(!bs[i][s])s=RAND()%M;
        int j=RAND()%D; while(bs[j].count()>=K)j=RAND()%D;
        MOVE(i,s,j);
        now_score=score();
        /*if(now_score>score()||T>=5500000*TL_ratio){
            now_score=score();
        }
        else MOVE(j,s,i);*/
    }
    void yaki_path(clock_t T,int K_relax=0){
        double X=log(2.)*pow(10.,(6800000.*TL_ratio-T)/(1000000.*TL_ratio));
        //X=0.;
        int s=RAND()%N;
        int t=RAND()%N; while(s==t)t=RAND()%N;
        bitset<3000> edges; edges.reset();
        vector<pair<int,int>> moves;
        while(s!=t){
            int ed=pre_edge_G[t][s];
            edges[ed]=true;
            s=v[ed]+u[ed]-s;
            moves.push_back(mp(ed,0));
        }
        while(moves.size()<5){
            s=RAND()%N;
            t=RAND()%N; while(s==t)t=RAND()%N;
            edges.reset();
            moves.clear();
            while(s!=t){
                int ed=pre_edge_G[t][s];
                edges[ed]=true;
                s=v[ed]+u[ed]-s;
                moves.push_back(mp(ed,0));
            }
        }
        vector<int> cand;
        rep(i,D){
            if((bs[i]|edges).count()<=K)cand.push_back(i);
        }
        if(cand.size()==0&&K_relax>0){
            rep(i,D){
                if((bs[i]|edges).count()<=K+K_relax)cand.push_back(i);
            }
        }
        if(cand.size()==0){
            yaki_MOVE(T);
            return;
        }
        int i=cand[RAND()%cand.size()];
        for(auto &p: moves){
            while(!bs[p.sc][p.fr])p.sc++;
        }
        pair<ll,int> best(now_score,-1);
        for(int ii=0;ii<moves.size();ii++){
            auto p=moves[ii];
            if(p.sc!=i){
                bool ok=false;
                for(auto t: nex[u[p.fr]])if(t!=p.fr&&!bs[i][t])ok=true;
                if(M>=2*N)ok&=score()<=now_score+100000000;
                else ok&=score()<=now_score+1000000000;
                if(ok){
                    ok=false;
                    for(auto t: nex[v[p.fr]])if(t!=p.fr&&!bs[i][t])ok=true;
                }
                if(ok){
                    ok&=MOVE2(p.sc,p.fr,i);
                    if(ok)best=chmin(best,mp(score(),p.fr));
                }
                if(!ok){
                    if(best.sc!=-1){
                        for(int j=ii-1;j>=0;j--){
                            auto q=moves[j];
                            if(q.fr==best.sc)break;
                            if(q.sc!=i)MOVE(i,q.fr,q.sc);
                        }
                        now_score=score();
                    }
                    else {
                        for(int j=0;j<ii;j++){
                            auto q=moves[j];
                            if(q.sc!=i)MOVE(i,q.fr,q.sc);
                            if(now_score>score()){
                                now_score=score();
                                break;
                            }
                        }
                    }
                    return;
                }
            }
        }
        if(best.sc!=-1){
            for(int j=moves.size()-1;j>=0;j--){
                auto p=moves[j];
                if(p.fr==best.sc)break;
                if(p.sc!=i)MOVE(i,p.fr,p.sc);
            }
            now_score=score();
        }
        else {
            if(now_score>score()||RAND()%10000<exp((now_score-score())/X)*10000){
                now_score=score();
            }
            else {
                for(auto p: moves){
                    if(p.sc!=i)MOVE(i,p.fr,p.sc);
                    if(now_score>score()){
                        now_score=score();
                        break;
                    }
                }
            }
        }
    }
    void set_random(){
        vector<int> cnt(D);
        rep(i,M){
            int r=RAND()%D; while(cnt[r]==K)r=RAND()%D;
            bs[r][i]=true;
            cnt[r]++;
        }
        score_init();
    }
    void output(){
        rep(i,M){
            rep(j,D){
                if(bs[j][i]){ printf("%d%c",j+1,(i+1==M)?'\n':' '); break; }
            }
        }
    }
};

int main(){
    clock_t START=clock();
    
    scanf("%d%d%d%d",&N,&M,&D,&K);
    rep(i,M){ scanf("%d%d%d",&u[i],&v[i],&w[i]); u[i]--; v[i]--; }
    rep(i,N)scanf("%d%d",&x[i],&y[i]);
    rep(i,M){
        nex[u[i]].push_back(i);
        nex[v[i]].push_back(i);
    }
    
    /*int X=0;
    rep(i,N)chmax(X,(int)nex[i].size());
    
    if(X<=20){
        rep(i,M){
            nex_arr[u[i]][deg[u[i]]++]=i;
            nex_arr[v[i]][deg[v[i]]++]=i;
        }
    }*/
    
    sol1 ret; ret.set_random();
    for(int i=0;;i++){
        ret.yama_MOVE();
        if(ret.now_score==D)break;
        if(i%1000==0)if(clock()>5000000)break;
    }
    
    //cerr<<clock()<<endl;
    
    /*sol2 ret2;
    rep(i,D)ret2.bs[i]=ret.bs[i]; ret2.score_init();
    for(int i=0;;i++){
        ret2.yama_MOVE();
        if(i%100==0)if(clock()>5000000){ cerr<<N<<" "<<M<<" "<<D<<" "<<K<<" "<<i<<endl; break; }
    }
    ret2.output();*/
    pq_init();
    init_dist();
    
    sol2 ret2;
    rep(i,D)ret2.bs[i]=ret.bs[i]; ret2.score_init();
    
    //cerr<<clock()<<endl;
    
    int TTT=1;
    
    if(K-M/D<=15){
        for(int i=0;;i++){
            static clock_t T=clock();
            static bool fit_done=false;
            if(T<5000000*TL_ratio){
                ret2.yaki_path(T,16-(K-M/D));
                ret2.yaki_MOVE(T);
            }
            else {
                int max_cnt=0;
                rep(i,D){
                    chmax(max_cnt,(int)ret2.bs[i].count());
                }
                if(max_cnt<=K)fit_done=true;
                if(fit_done){
                    ret2.yaki_path(T);
                    ret2.yaki_MOVE(T);
                }
                else {
                    ret2.K_fit(T);
                }
            }
            if(i%40==0){
                T=clock();
                if(T>5850000*TL_ratio){ cerr<<i<<endl; break; }
            }
        }
        ret2.output();
    }
    else {
        for(int i=0;;i++){
            static clock_t T=clock();
            //ll memo=ret2.now_score;
            ret2.yaki_path(T);
            ret2.yaki_MOVE(T);
            //if(memo<ret2.now_score){ cerr<<memo<<"->"<<ret2.now_score<<"\n"; }
            if(i%40==0){
                T=clock();
                if(T>5850000*TL_ratio){ cerr<<i<<endl; break; }
                /*if(i>=10000&&i<100000&&i%1000==0){
                 ret2.start_points_add();
                 }*/
            }
        }
        ret2.output();
    }
    
    //cerr<<profile_CNT0<<" "<<profile_CNT1<<endl;
    
    return 0;
}
