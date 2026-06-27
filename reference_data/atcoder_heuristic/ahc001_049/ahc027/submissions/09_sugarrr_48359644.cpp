#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include<bits/stdc++.h>
#include<atcoder/all>
using namespace std;
using namespace atcoder;
typedef long long ll;
/*
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
namespace multi = boost::multiprecision;
typedef multi::cpp_int LL;
typedef multi::number<multi::cpp_dec_float<1024>> DD;// 仮数部が1024ビットの浮動小数点数型(TLEしたら小さくする)
//*/
typedef long double dd;
typedef pair<ll,ll> l_l;
typedef pair<dd,dd> d_d;
ll inf=(ll)1E18;
//#define rep(i,l,r) for(ll i=l;i<=r;i++)
//#define rrep(i,r,l) for(ll i=r;i>=l;i--)
#define rep(i,l,r) for(int i=l;i<=r;i++)
#define rrep(i,r,l) for(int i=r;i>=l;i--)
#define pb push_back
ll max(ll a,ll b){if(a<b)return b;else return a;}
ll min(ll a,ll b){if(a>b)return b;else return a;}
dd EPS=1E-12;
#define fi first
#define se second

#define SORT(v) sort(v.begin(),v.end())
#define ERASE(v) sort(v.begin(),v.end()); v.erase(unique(v.begin(),v.end()),v.end())
#define POSL(v,x) (lower_bound(v.begin(),v.end(),x)-v.begin())
#define POSU(v,x) (upper_bound(v.begin(),v.end(),x)-v.begin())
template<class T,class S>
inline bool chmax(T &a, S b) {
    if(a < b) {
        a = (T)b;
        return true;
    }
    return false;
}
template<class T,class S>
inline bool chmin(T &a, S b) {
    if(a > b) {
        a = (T)b;
        return true;
    }
    return false;
}
#define all(c) c.begin(),c.end()

//using mint = modint998244353;
//using mint = modint1000000007;
using mint=modint;
//using mint=static_modint<100>;
//using mint=dd;
//using mint=ll;

typedef vector<ll> vl;
typedef vector<vl> vvl;
typedef vector<vvl>vvvl;
typedef vector<vvvl>vvvvl;
typedef vector<vvvvl>vvvvvl;
/*
typedef vector<mint>vi;
typedef vector<vi>vvi;
typedef vector<vvi>vvvi;
typedef vector<vvvi>vvvvi;
typedef pair<mint,mint>i_i;
typedef vector<i_i>vi_i;
 //*/
typedef vector<l_l>vl_l;
typedef vector<vl_l>vvl_l;
typedef vector<vvl_l>vvvl_l;
typedef vector<string>vs;
typedef vector<vs>vvs;
typedef vector<dd> vd;
typedef vector<vd> vvd;
typedef vector<vvd> vvvd;
typedef vector<d_d>vd_d;
dd PI=acos((dd)-1);

#define fastio ios::sync_with_stdio(false); cin.tie(0); cout.tie(0); cout<<setprecision(10);cerr<<fixed<<setprecision(5);
template <class T> using pq = priority_queue<T>;
template <class T> using pqg = priority_queue<T, vector<T>, greater<T>>;

#define endl "\n"  //インタラクティブで消す！！！！！！！！！！！！！！！！！！！！！
#define popcount __builtin_popcountll
#define SHUFFLE(v) shuffle(all(v),default_random_engine(chrono::system_clock::now().time_since_epoch().count()))
//////////////////////////////////////////////////////////////////////////////
template<class S>
void DEBUG_PRINT(S x){
    cerr<<x<<endl;
}
template<class S,class T>
void DEBUG_PRINT(pair<S,T>x){
    cerr<<"("<<x.fi<<","<<x.se<<")"<<endl;
}
template<class S>
void DEBUG_PRINT(vector<S> x){
    for(auto y:x)cerr<<setw(2)<<y<<" ";
    cerr<<endl;
}
template<class S,class T>
void DEBUG_PRINT(vector<pair<S,T>>x){
    for(auto y:x)cerr<<"("<<y.fi<<","<<y.se<<") ";
    cerr<<endl;
}
template<class S>
void DEBUG_PRINT(vector<vector<S>> x){
    cerr<<endl;
    for(auto y:x){
        for(auto z:y){
            cerr<<setw(2)<<z<<" ";
        }cerr<<endl;
    }cerr<<endl;
}

#define TO_STRING_FOR_DEBUG(VariableName) # VariableName
#ifdef LOCAL
#define DEB(V) cerr<< TO_STRING_FOR_DEBUG(V) << ": "; DEBUG_PRINT(V);
#else
#define DEB(V) void(0)
#endif

#define importantBit(x) (63-__builtin_clzll(x))
/////////////////////////////////////////////////////////////////////////////////////////
unsigned int randxor(){
    //0 以上 (2^32)-1 以下の整数(だと思う)
    static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}
int rand(int a,int b){
    //a以上b以下の整数を返す。
    assert(a<=b);
    ll len=(b-a+1);
    return randxor()%len + a;
}
dd randdouble(dd l,dd r){
    //l以上r以下の実数を返す。
    assert(l<=r + EPS);
    return l + (r-l) * (dd)randxor()/4294967295;
}
template<typename T>
T randVec(vector<T>& a){
    //aから1つ選んでreturn
    assert(!a.empty());
    return a[rand(0,(int)a.size()-1)];
}

struct timespec START_TIME;
dd stop_watch() {
    // 経過時間をミリ秒単位で返す.
    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);
    ll sec = end_time.tv_sec - START_TIME.tv_sec;
    ll nsec = end_time.tv_nsec - START_TIME.tv_nsec;
    return 1000 * (dd)sec + (dd)nsec / (1000000);
}

template<typename T>
void shuffle(vector<T>& a) {
    int n = (int) a.size();
    for (int i = 1; i < n; i++) {
        swap(a[i], a[randxor() % (i + 1)]);
    }
}

#define clock_start clock_gettime(CLOCK_REALTIME, &START_TIME);
////////////////////////////////////////////////

typedef vector<int>vi;
typedef vector<vi>vvi;
typedef vector<vvi>vvvi;
typedef vector<vvvi>vvvvi;
typedef vector<vvvvi>vvvvvi;
typedef pair<int,int>i_i;
typedef vector<i_i>vi_i;

int N__;
vvi H__;
vvi V__;
vvi D__;

void INPUT_FOR_PROFILE(){
    N__ = 30;
    H__.resize(N__-1);
    rep(i,0,N__-2)H__[i].resize(N__);
    V__.resize(N__);
    rep(i,0,N__-1)V__[i].resize(N__-1);
    D__.resize(N__);
    rep(i,0,N__-1){
        rep(j,0,N__-1)D__[i].pb(rand(1,1000));
    }
}

void INPUT(){
    cin>>N__;
    H__.resize(N__-1);
    rep(i,0,N__-2){
        string s;cin>>s;
        for(char x:s)H__[i].pb(x - '0');
    }
    V__.resize(N__);
    rep(i,0,N__-1){
        string s;cin>>s;
        for(char x:s)V__[i].pb(x - '0');
    }
    D__.resize(N__);
    rep(i,0,N__-1){
        D__[i].resize(N__);
        rep(j,0,N__-1)cin>>D__[i][j];
    }
   
   // cerr<<"[DATA] N = "<<N__<<endl;
}

vd PARAMS;

int N;// N__ * N__
vvi NX;//サイズはN
vl D;//サイズはN

int P1D(i_i x){
    return x.fi * N__ + x.se;
}
i_i P2D(int x){
    return {x/N__,x%N__};
}

void ToConvert1D(){
    N = N__ * N__;
    NX.resize(N);
    rep(i,0,N__-1){
        rep(j,0,N__-1){
            int now = P1D({i,j});
            if(i >= 1 && H__[i-1][j] == 0){
                int op = P1D({i-1,j});
                NX[now].pb(op);
                NX[op].pb(now);
            }
            if(j >= 1 && V__[i][j-1] == 0){
                int op = P1D({i,j-1});
                NX[now].pb(op);
                NX[op].pb(now);
            }
        }
    }
    D.resize(N);
    rep(i,0,N__-1)rep(j,0,N__-1){
        D[P1D({i,j})] = D__[i][j];
    }
}

ll CalcSumdirt(vi poss){
    int L = poss.size() - 1;
    vvi ocr(N);
    rep(i,0,L-1){
        ocr[poss[i]].pb(i);
    }
    assert(poss[L] == poss[0]);
    vl dirt(N);
    rep(i,0,N-1){
        if(ocr[i].empty())return inf;
        //ありえるかもしれない
    }
    rep(i,0,N-1){
        assert(!ocr[i].empty());
        int sz = ocr[i].size();
        ocr[i].pb(ocr[i][0] + L);
        rep(j,0,sz-1){
            ll len = (ocr[i][j+1] - ocr[i][j]);
            dirt[i] += (ll)len * (len - 1) / 2 * D[i];
        }
    }
    
    ll sumdirt = 0;
    rep(i,0,N-1)sumdirt += dirt[i];
    
    return sumdirt;
}

void OUTPUT(vi poss){
    string ans;
    int L = poss.size()-1;
    rep(i,0,L-1){
        int dif = poss[i+1] - poss[i];
        if(dif == N__)ans += "D";
        else if(dif == 1)ans += "R";
        else if(dif == -1)ans += "L";
        else if(dif == -N__)ans += "U";
        else{
            DEB(dif);
            DEB(poss[i]);
            DEB(poss[i+1]);
            DEB(i);
            assert(0);
        }
    }
    cout<<ans<<endl;
  //  DEB(llround(CalcSumdirt(poss)/(dd)L));
    
}

vi searchgoodpath_dfs(int start,const vd& score){
    //scoreの和が正だと嬉しい
    vi used(N);
    used[start] = 1;
    vi best_path;
    dd best_score = 0;
    int dfs_cnt = 0;
    int DFS_CNT_LIMIT = 30;
    vi now_path = {start};
    auto dfs=[&](int now,dd sum,auto dfs){
        if(chmax(best_score,sum)){
            best_path = now_path;
        }
        dfs_cnt++;
        if(dfs_cnt >= DFS_CNT_LIMIT)return;
        sort(all(NX[now]),[&](int a,int b){return score[a] > score[b];});
        for(int nx:NX[now]){
            if(dfs_cnt >= DFS_CNT_LIMIT)return;
            if(used[nx])continue;
            used[nx] = 1;
            now_path.pb(nx);
            dfs(nx,sum + score[nx],dfs);
            used[nx] = 0;
            now_path.pop_back();
        }
    };
    dfs(start,0,dfs);
    return best_path;
}
typedef pair<dd,int> d_i;
typedef vector<d_i> vd_i;
vi searchgoodpath_beam(int start,const vd& score){
    //scoreの和が正だと嬉しい

    vi kouho;
    dd kouho_ave = 0;
    
    vi par = {-1};
    vi ver = {start};
    auto path_=[&](int s){
        vi ret;
        while(1){
            assert(s >= 0);
            ret.pb(ver[s]);
            if(s == 0)break;
            s = par[s];
        }
        return ret;
    };
    vd_i parents = {{0,0}};
    
    ll beam_width = 30;
    rep(length,1,40){
        vd_i childs;
        for(d_i pp:parents){
            int p = pp.se;
            vi path = path_(p);
            int now = ver[p];
            assert(path.front() == now && path.back() == start);
            for(int nx:NX[now]){
                bool ng = false;
                for(int pos:path)if(pos == nx)ng = true;
                if(ng)continue;
                par.pb(p);
                ver.pb(nx);
                childs.pb({pp.fi + score[nx],par.size()-1});
            }
        }
        sort(all(childs),[](d_i a,d_i b){return a.fi > b.fi;});
        while((int)childs.size() > beam_width)childs.pop_back();
        parents = childs;
        if(parents.empty())break;
        if(chmax(kouho_ave,parents[0].fi)){
            kouho = path_(parents[0].se);
        }
    }
    if(kouho.empty())return vi{};
    reverse(all(kouho));
    return kouho;
}


vvi pcdis;
vvi pcjun;

vi searchgoodpath(int start,bool Compulse,int anssize,vi& visited,vi &lastt){
    //Compulse:=強制的にvalidな解へ進めるモード
    //non-visitedな点 or 0 へ無理矢理進める
    
    static bool MyFunctionFirst = true;
    static vd Dpow(N);
    static vd diftpow(100001);
    
    dd dispowkei;
    if(N__ <= 25)dispowkei = 0.995;
    else if(N__ <= 34)dispowkei = 0.9975;
    else dispowkei = 0.9985;
    
    static vd dispow(N+1);//dispow[i] = (dispowkei) ^ i
    pcdis.resize(N);
    pcjun.resize(N);
    
    if(MyFunctionFirst){
        MyFunctionFirst = false;
        rep(i,0,N-1){
            Dpow[i] = pow(D[i],0.4);
        }
        rep(i,0,100000){
            diftpow[i] = pow((ll)i * (i+1) / 2  , 0.35);
        }
        dispow[0] = 1;
        rep(i,1,N)dispow[i] = dispow[i-1] * dispowkei;
        
        rep(s,0,N-1){
            vi dis(N,-1);
            vi jun;
            queue<int>q;
            q.push(s);dis[s] = 0;
            while(!q.empty()){
                int t = q.front();q.pop();
                jun.pb(t);
                for(int nx:NX[t]){
                    if(dis[nx] == -1){
                        dis[nx] = dis[t] + 1;
                        q.push(nx);
                    }
                }
            }
            pcdis[s] = dis;
            pcjun[s] = jun;
        }
    }
    
    vi dis = pcdis[start];
    vi prev(N,-1);
    vd summax(N,0);
    vd score(N);
    rep(i,0,N-1){
        ll dift = anssize - lastt[i] - 1;
        score[i] = diftpow[dift] * Dpow[i] * dispow[dis[i]];
    }
    
    for(int t:pcjun[start]){
        for(int nx:NX[t]){
            if(dis[nx] == dis[t] + 1){
                if(chmax(summax[nx],summax[t] + score[nx])){
                    prev[nx] = t;
                }
            }
        }
    }
    
    vd ave(N);
    int end = -1;
    if(!Compulse){
        rep(i,0,N-1){
            if(i != start){
                ave[i] = (dd)summax[i] / dis[i];
                if(end == -1 || ave[end] < ave[i]){
                    end = i;
                }
            }
        }
        
        /*
        {//dfsでパス探し
            rep(i,0,N-1)score[i] -= (ave[end] - 0.1); //0.1はdfsを優遇
            vi dfs_path = searchgoodpath_dfs(start,score);
            if(!dfs_path.empty()){
               // DEB(dfs_path.size());
                return dfs_path;
            }
        }
         //*/
        //*
        {
            //beamでパス探し
            dd score_correc;
            if(N__ <= 25)score_correc = 0.3;
            else if(N__ <= 34)score_correc = 0.2;
            else score_correc = 0.1;
            rep(i,0,N-1)score[i] -= (ave[end] - score_correc); //0.1はdfsを優遇
            vi beam_path = searchgoodpath_beam(start,score);
           // DEB(beam_path.size());
            if(!beam_path.empty()){
               // DEB(dfs_path.size());
                return beam_path;
            }
        }
        //*/
    }else{
        bool allvisited = true;
        for(int x:visited)if(x == 0)allvisited = false;
        if(allvisited){
            end = 0;
        }else{
            rep(i,0,N-1){
                if(i != start && visited[i] == 0){
                    ave[i] = (dd)summax[i] / dis[i];
                    if(end == -1 || ave[end] < ave[i]){
                        end = i;
                    }
                }
            }
        }
    }
    assert(end != -1);
    
    vi path = {end};
    while(1){
        int now = path.back();
        if(now == start){
            reverse(all(path));
            break;
        }
        int pre = prev[now];
        assert(dis[pre] +1 ==  dis[now]);
        path.pb(pre);
    }
    return path;
};

vi greedy_3_2(dd TIME_LIMIT,int LEN_LIMIT){
    vi lastt(N,-1);
    lastt[0] = 0;
    vi ans = {0};
    vi visited(N);
    visited[0] = true;
    int visited_cnt = 1;
    int first_end_count = 0;
    bool TIME_OUT = false;
    
    while(1){
        int pos = ans.back();
        if(stop_watch() > TIME_LIMIT || (int)ans.size() >= LEN_LIMIT)TIME_OUT = true;
        if(TIME_OUT && first_end_count != 0)break;
        vi goodpath = searchgoodpath(pos,TIME_OUT && first_end_count == 0,ans.size(),visited,lastt);
        if((int)goodpath.size() == 1){
            //Compulse = trueの時に発生する例外的なパターン
            assert(TIME_OUT && first_end_count == 0);
            assert(goodpath[0] == 0);
            break;
        }
        assert((int)goodpath.size() >= 2 && goodpath.front() == pos);
        rep(i,1,(int)goodpath.size()-1){
            lastt[goodpath[i]] = ans.size();
            ans.pb(goodpath[i]);
            if(visited[goodpath[i]]==0){
                visited_cnt ++;
                visited[goodpath[i]] = 1;
            }
            if(goodpath[i] == 0){
                if(visited_cnt == N){
                    if(first_end_count == 0){
                        first_end_count = ans.size();
                    }
                }
            }
        }
    }
   // cerr<<"[DATA] first = "<<first_end_count<<endl;
    //100ケース回してmax(first) < 20000なので、Compulseはほとんど発動してないと思う
    
   // while(ans.back() != 0)ans.pop_back();
    return ans;
};

ll SE[200001];
void calcSE(){
    rep(i,0,200000){
        SE[i] = (ll)i * (i-1) / 2;
    }
}

vi LRsearch(vi a,dd TIME_LIMIT,int LEN_LIMIT){
    
    int Asize = a.size();
    //長さ2倍にするのはやめてる
    
    vi best;
    dd best_cost = inf;
    bool find_LENLIMIT = false;
    
    //ll greedy_cost = llround((dd)CalcSumdirt(A_) / ((int)A_.size() - 1));

    vi IMPjun;
    rep(i,1,N-1)IMPjun.pb(i);
    shuffle(IMPjun);
    IMPjun.insert(IMPjun.begin(),0);
    //DEB(IMPjun);
    
    int IMPloop = 0;
    for(int IMP:IMPjun){
        if(stop_watch() > TIME_LIMIT){
            DEB(IMPloop);
            break;
        }
        IMPloop++;
        //dd best_nowIMP = inf;
        rep(start,0,Asize-2){
            if((start & 1023) == 1023 && stop_watch() > TIME_LIMIT){
                break;
            }
            if(a[start] != IMP)continue;
            vi firstt(N,-1);
            vi lastt(N,-1);
            int visitcnt = 0;
            ll sum_cost = 0;
            int LIMIT = min(start+30000,(int)a.size()-1);
            rep(time,start,LIMIT){
                if(find_LENLIMIT && time - start >= LEN_LIMIT)break;
                if(lastt[a[time]] == -1){
                    firstt[a[time]] = time;
                    visitcnt++;
                }else{
                    int dift = time - lastt[a[time]];
                    sum_cost += SE[dift] * D[a[time]];
                }
                lastt[a[time]] = time;
               // assert(visitcnt <= N);//OK!
                if(visitcnt == N && a[time] == IMP){
                    ll add_cost = 0;
                    rep(i,0,N-1){
                        assert(firstt[i] != -1 && lastt[i] != -1);
                        int dift = time - start + firstt[i] - lastt[i];
                        add_cost += SE[dift] * D[i];
                    }
                    dd COST = (dd)(sum_cost + add_cost) / (time - start);
                    if(find_LENLIMIT){
                        if(chmin(best_cost,COST)){
                           // best_nowIMP = COST;
                            best = {};
                            rep(i,start,time)best.pb(a[i]);
                        }
                    }else{
                        if(time - start <= LEN_LIMIT){
                            best_cost = COST;
                          //  best_nowIMP = COST;
                            best = {};
                            rep(i,start,time)best.pb(a[i]);
                            find_LENLIMIT = true;
                        }else{
                            if(best.empty() || time - start < (int)best.size()){
                                best_cost = COST;
                              //  best_nowIMP = COST;
                                best = {};
                                rep(i,start,time)best.pb(a[i]);
                            }
                        }
                    }
                }
            }
        }
       // DEB(best_nowIMP);
    }
    
  //  cerr<<"[DATA] LRscore = "<<llround(best_cost)<<endl;
  //DEB(best[0]);
    return best;
}

const int MaxLen = 8;

vvvvi Paths;

vi FindPath(int s,int g,int len){
    static bool MyFunctionFirst = true;
    assert(len >= 3);
    
    if(MyFunctionFirst){
        MyFunctionFirst = false;
        Paths.resize(N,vvvi(N));
        rep(s,0,N-1){
            vi used(N);
            vi v = {s};
            auto dfs=[&](int now,auto dfs){
                if((int)v.size() >= 3){
                    Paths[v.front()][v.back()].pb(v);
                }
                if((int)v.size() == MaxLen)return;
                if((int)v.size() >= 2 && v.front() == v.back())return;
                for(int x:NX[now]){
                    if(used[x])continue;
                    used[x] = true;
                    v.pb(x);
                    dfs(x,dfs);
                    used[x] = false;
                    v.pop_back();
                }
            };
            dfs(s,dfs);
        }
      //  cerr<<"findpathの前計算終了 "<<stop_watch()<<"ms"<<endl;
    }
    if(len > MaxLen)return vi{};
    int size = Paths[s][g].size();
    if(size == 0)return vi{};
    int rpos = rand(0,size - 1);
    if(Paths[s][g][rpos].size() != len) return vi{};
    return Paths[s][g][rpos];
}

bool HANTEI(dd X,dd temp){
    if(X < 0)return true;
    return randdouble(0,1) < exp(-X/temp);
}

vi annealing(vi ans,dd TIME_LIMIT,int LOOP_LIMIT){
    struct State{
        vi poss;
        ll sumdirt;
        vector<set<int>>S;
    };
    auto SUpdate=[](State &now,int time,int preval,int atoval){
        //now.Sだけ更新する
        assert(now.S[preval].find(time) != now.S[preval].end());
        now.S[preval].erase(time);
        now.S[atoval].insert(time);
    };
    
    State now = {ans,0};
    int L = now.poss.size() - 1;
    now.sumdirt = CalcSumdirt(now.poss);
   // now.sumdirt = 0;
    now.S.resize(N);
    rep(i,0,L)now.S[now.poss[i]].insert(i);
    
   // vl_l results;
    
    dd start_temp = 80;
    dd end_temp = 0;
    dd start_time = stop_watch();
    dd end_time = TIME_LIMIT;
    
    vi best_poss = now.poss;
    ll best_sumdirt = now.sumdirt;

    int valid_loop = 0;
    
    dd now_time;
    dd temp;
    
    rep(loop,1,2000000000){
        if((loop & 1023) == 1){
            now_time = stop_watch();
            temp = start_temp + (end_temp - start_temp) * (now_time - start_time) / (end_time - start_time);
        }
        
        if(now_time > TIME_LIMIT || valid_loop == LOOP_LIMIT){
          //  cerr<<"[DATA] loop1  = "<<valid_loop<<endl;
            break;
        }
        
        int pos1 = rand(0,L-2);
        int pos2;
        vi path;
        
        bool type = rand(1,100) <= 60;
        if(type){
            pos2 = min(L,pos1 + rand(2,MaxLen - 1));//加える下限が 2 ↔︎ 長さの下限は 3
            path = FindPath(now.poss[pos1] , now.poss[pos2] , pos2 - pos1 + 1);
        }else{
            pos2 = -1;
            rep(p,pos1 + 2,min(L,pos1 + 60)){
                if(now.poss[pos1] == now.poss[p]){
                    pos2 = p;
                }
            }
            if(pos2 != -1){
                rrep(t,pos2,pos1)path.pb(now.poss[t]);
                assert(now.poss[pos1] == now.poss[pos2]);
            }
        }
        if(path.empty())continue;
        
        
        vi pre_path;
        rep(t,pos1,pos2)pre_path.pb(now.poss[t]);
        if(path == pre_path)continue;
        valid_loop++;
        
        ll dif_sumdirt = 0;
        rep(pos,pos1 + 1 , pos2 - 1){
            auto searchpos=[&](ll target){
                i_i ret = {-1,-1};
                {
                    rep(t,pos + 1,pos2){
                        if(now.poss[t] == target){
                            ret.se = t - pos;
                            break;
                        }
                    }
                    if(ret.se == -1){
                        auto itr = now.S[target].upper_bound(pos);
                        while(1){
                            if(itr == now.S[target].end())break;
                            assert((*itr) > pos);
                            if((*itr) >= pos2){
                                assert((*itr) != pos2);
                                ret.se = (*itr) - pos;
                                break;
                            }else{
                                itr++;
                            }
                        }
                        if(ret.se == -1){
                            itr = now.S[target].begin();
                            if(itr !=  now.S[target].end()){
                                if((*itr) <= pos1){
                                    ret.se = (*itr) - pos + L;
                                }
                            }
                            if(ret.se == -1){
                                rep(t,pos1,pos - 1){
                                    if(now.poss[t] == target){
                                        assert(t != pos1);
                                        ret.se = t - pos + L;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if(ret.se == -1){
                        dif_sumdirt = 1e15;
                        ret = {0,0};//避難
                        return ret;
                    }
                }
                {
                    rrep(t,pos-1,pos1){
                        if(now.poss[t] == target){
                            ret.fi = pos - t;
                            break;
                        }
                    }
                    if(ret.fi == -1){
                        auto itr = now.S[target].lower_bound(pos1);
                        if(!now.S[target].empty()){
                            if(itr == now.S[target].end())itr--;
                            while(1){
                                assert(itr != now.S[target].end());
                                assert((*itr) != pos1);
                                if((*itr) < pos1){
                                    ret.fi = pos - (*itr);
                                    break;
                                }else{
                                    if(itr == now.S[target].begin())break;
                                    itr--;
                                }
                            }
                            if(ret.fi == -1){
                                itr = now.S[target].end();
                                itr--;
                                if((*itr) >= pos2){
                                    ret.fi = pos - (*itr) + L;
                                }else{
                                    rrep(t,pos2,pos+1){
                                        if(now.poss[t] == target){
                                            ret.fi = pos - t + L;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    assert(ret.fi != -1);
                }
                return ret;
            };
            i_i maedift = searchpos(now.poss[pos]);
            dif_sumdirt -= (SE[maedift.fi] + SE[maedift.se] - SE[maedift.fi+maedift.se]) * D[now.poss[pos]];
            now.poss[pos] = path[pos - pos1];
            
            i_i atodift = searchpos(path[pos - pos1]);
            dif_sumdirt += (SE[atodift.fi] + SE[atodift.se] - SE[atodift.fi+atodift.se]) * D[path[pos-pos1]];
        }
        
        if(HANTEI((dd)dif_sumdirt/L,temp)){
            now.sumdirt += dif_sumdirt;
          //  results.pb({loop,llround((dd)now.sumdirt/L)});
            if(chmin(best_sumdirt,now.sumdirt)){
                best_poss = now.poss;
            }
            rep(t,pos1,pos2){
                SUpdate(now,t,pre_path[t-pos1],path[t-pos1]);
            }
        }else{
            rep(t,pos1,pos2){
                now.poss[t] = pre_path[t - pos1];
            }
        }
    }
    DEB(valid_loop);
    /*
    ll nowval = inf;
    for(auto result:results){
        //loop , cost
        cerr<<result.fi<<" "<<result.se<<endl;
    }
    //*/
    return best_poss;
}


/*
ll UpdateRpos(const vl &now_poss,ll pos1,ll pre_pos2,ll target){
    ll L = now_poss.size() - 1;
    
    assert(now_poss[pre_pos2] == target);
    rep(i,pre_pos2+1,L){
        if(now_poss[i] == target){
            return i;
        }
    }
    rep(i,0,pos1-1){
        if(now_poss[i] == target){
            return i + L;
        }
    }
    return -1;
}

ll parity_rand(ll a,ll b,ll c){
    assert(a < b);
    assert(c >= 0);
    rep(loop,1,1000){
        ll ret = rand(a,b);
        if((ret&1) == (c&1)){
            return ret;
        }
    }
    assert(0);
    return 0;
}
 */

vl CalcBanScore(vi Lpos,vi Rpos,int pos1,int pos2){
    vl ret(N);
    rep(i,0,N-1){
        ll len = Rpos[i]-Lpos[i];
        ll l1 = (pos1-Lpos[i]) + (pos2 - pos1) / 2;
        ll l2 = len - l1;
        ret[i] = SE[len] - SE[l1] - SE[l2];
        ret[i] *= D[i];
    }
    return ret;
}

vi FindPathByBanScore2(vl BanScore,int S,int G,bool update){

    static vvi kouho;
    static vd kouho_ave;
    static vi jun;
    if(update){
        kouho.clear();
        kouho_ave.clear();
        jun.clear();
        
        int dfs_count = 0;
        int DFS_LIMIT = 100;
        int LEN_LIMIT = 10;
        vi v = {S};
        vi visited(N,0);//Sは1にしなくていいはず
        auto dfs=[&](int now,int sum,auto dfs){
            if(dfs_count >= DFS_LIMIT)return;
            if((int)v.size() >= LEN_LIMIT)return;
            dfs_count++;
            for(int x:NX[now]){
                if(dfs_count >= DFS_LIMIT)return;
                if(x == G){
                    v.pb(G);
                    if((int)v.size() >= 3){
                        kouho.pb(v);
                        kouho_ave.pb((dd)sum/((int)v.size()-2));
                    }
                    v.pop_back();
                    return;
                }
            }
            sort(all(NX[now]),[&](int a,int b){return BanScore[a] > BanScore[b];});
            for(int x:NX[now]){
                if(dfs_count >= DFS_LIMIT)return;
                if(x == G)return;
                if(visited[x])continue;
                if(v.size() + 1 + pcdis[x][G] >= LEN_LIMIT)continue;
                visited[x] = 1;
                v.pb(x);
                dfs(x,sum+BanScore[x],dfs);
                visited[x] = 0;
                v.pop_back();
            }
        };
        dfs(S,0,dfs);
        assert(kouho.size() == kouho_ave.size());
        rep(i,0,(int)kouho.size()-1)jun.pb(i);
        sort(all(jun),[&](int a,int b){return kouho_ave[a] < kouho_ave[b];});
    }
    if(jun.empty())return vi{};
    int pos = jun.back();
    jun.pop_back();
    return kouho[pos];
}

vi FindPathByBanScore3(vl &BanScore,int S,int G,bool update){
    static vector<pair<dd,int>> forsort;
    
    if(update){
        forsort.clear();
        int sz = Paths[S][G].size();
        rep(i,0,sz-1){
            dd ave = 0;
            int k = Paths[S][G][i].size();
            assert(k >= 3);
            rep(j,1,k-2){
                ave += BanScore[Paths[S][G][i][j]];
            }
            ave /= (k-2);
            forsort.pb({ave,i});
        }
        SORT(forsort);
    }
    if(forsort.empty())return vi{};
    int pos = forsort.back().se;
    forsort.pop_back();
    return Paths[S][G][pos];
}
vi FindPath4(int S,int G,bool update){
    static vvi pos(N,vi(N,-1));
    static int start_pos = -1;
    
    if(Paths[S][G].empty())return vi{};
    pos[S][G]++;
    if(Paths[S][G].size() == pos[S][G]){
        pos[S][G] = 0;
    }
    if(update){
        start_pos = pos[S][G];
        return Paths[S][G][start_pos];
    }
    assert(start_pos != -1);
    if(start_pos == pos[S][G]){
        return vi{};
    }
    
    return Paths[S][G][pos[S][G]];
}

void CalcLposRpos(const vi&now_poss,int pos1,int pos2,vi&Lpos,vi&Rpos,bool&NG){
    int L = now_poss.size() - 1;
    Rpos.assign(N,-1);
    {
        int cnt = 0;
        rep(i,pos2+1,L){
            if(cnt == N)break;
            if(Rpos[now_poss[i]] == -1){
               // Rdis[now_poss[i]] = i - pos2;
                Rpos[now_poss[i]] = i;
                cnt++;
            }
        }
        rep(i,0,pos1-1){
            if(cnt == N)break;
            if(Rpos[now_poss[i]] == -1){
                //Rdis[now_poss[i]] = i + L - pos2;
                Rpos[now_poss[i]] = i + L;
                cnt++;
            }
        }
        if(cnt < N){
            NG = true;
            return;
        }
    }
    int infint = 2e9;
    Lpos.assign(N,infint);
    {
        int cnt = 0;
        rrep(i,pos1-1,0){
            if(cnt == N)break;
            if(Lpos[now_poss[i]] == infint){
                //Ldis[now_poss[i]] = pos1 - i;
                Lpos[now_poss[i]] = i;
                cnt++;
            }
        }
        rrep(i,L,pos2+1){
            if(cnt == N)break;
            if(Lpos[now_poss[i]] == infint){
               // Ldis[now_poss[i]] = L - i + pos1;
                Lpos[now_poss[i]] = i - L;
                cnt++;
            }
        }
        assert(cnt == N);
        //cnt < N でも pathに含まれてたりしてOKなパターンもあるけど、ややこしいので inf を return してしまう
    }
}

ll calcRET1(const vi &Lpos,const vi&Rpos,int pos1,int pos2,const vi&pre_path){
    ll ret1 = 0;
    int len1 = pre_path.size();
    
    vvi a1(N);
    
    rep(i,0,len1-1)a1[pre_path[i]].pb(i);
    rep(i,0,N-1){
        int size = a1[i].size();
        if(size == 0){
            ret1 -= SE[(Rpos[i]-pos2) + (pos1-Lpos[i]) + len1 - 1] * D[i];
        }else{
            ret1 -= SE[a1[i].front() + (pos1-Lpos[i])]* D[i];
            ret1 -= SE[(Rpos[i]-pos2) + len1 - 1 - a1[i].back()] * D[i];
            rep(j,0,size-2){
                ret1 -= SE[a1[i][j+1] - a1[i][j]] * D[i];
            }
        }
    }
    return ret1;
}

vl calcKEIRET2(const vi&Lpos,const vi&Rpos,int pos1,int pos2){
    vl keiret2(3);
    
    rep(i,0,2)keiret2[i] = 0;
    rep(i,0,N-1){
        ll a = Rpos[i] - pos2 + pos1 - Lpos[i] - 1;
        keiret2[0] += D[i] * (a * a - a);
        keiret2[1] += D[i] * (2 * a - 1);
        keiret2[2] += D[i];
    }
    
    return keiret2;
}

ll calcRET2(const vi&Lpos,const vi&Rpos,int pos1,int pos2,int L,const vi &path,vl KEIRET2){
    int len2 = path.size();
    
    ll ret2 = KEIRET2[0] + KEIRET2[1] * len2 + KEIRET2[2] * len2 * len2;
    ret2 /= 2;
    
    rep(z,0,len2-1){
        //pathが単純パス(同じ頂点を2回通らない)ことを利用
        
        int pos = path[z];
        int X = z + pos1 - Lpos[pos];
        int Y = Rpos[pos] - pos2 + len2 - 1 - z;
        ret2 += (SE[X] + SE[Y] - SE[X+Y]) * D[pos];
    }
    
    return ret2;
};

void OneMovePos1Pos2(const vi&now_poss,const vi&now_next,int &pos1,int &pos2,vi&Lpos,vi&Rpos,ll&RET1,vl &KEIRET2,bool&NG){

    {
        pos2++;
        int target = now_poss[pos2];
        int ret = now_next[pos2];
        if(ret == -1){
            NG = true;
            return;
        }
        {
            //KEIRET2
            ll pa = Rpos[target] - (pos2 - 1) + pos1 - Lpos[target] - 1;
            ll da = ret - Rpos[target];
            KEIRET2[0] += (2 * pa * da + da * da - da) * D[target];
            KEIRET2[1] += (2 * da) * D[target];
        }
        Rpos[target] = ret;
        int P2 = ret;
        RET1 -= SE[P2 - pos2] * D[target];
    }
    {
        int target = now_poss[pos1];
        int P1 = Lpos[target];
        RET1 += SE[pos1 - P1] * D[target];
        {
            //KEIRET2
            ll pa = Rpos[target] - (pos2 - 1) + pos1 - Lpos[target] - 1;
            ll da = Lpos[target] - pos1;
            KEIRET2[0] += (2 * pa * da + da * da - da) * D[target];
            KEIRET2[1] += (2 * da) * D[target];
        }
        
        Lpos[now_poss[pos1]] = pos1;
        pos1++;
    }
}


vi yamanobori_Lchange(vi ans,dd TIME_LIMIT){
    
    struct State{
        vi poss;
        vi next;
        ll sumdirt;
        void fillnext(){
            int L = poss.size() - 1;
            next.assign(L,-1);
            vl first(N,-1);
            vl last(N);
            rep(i,0,L-1){
                if(first[poss[i]] == -1){
                    first[poss[i]] = i;
                }else{
                    next[last[poss[i]]] = i;
                }
                last[poss[i]] = i;
            }
            rep(i,0,N-1){
                assert(next[last[i]] == -1);
                if(last[i] == first[i]){
                    next[last[i]] = -1;
                }else{
                    next[last[i]] = first[i] + L;
                }
            }
        }
    };
    
    State now = {ans};
    now.fillnext();
    now.sumdirt = CalcSumdirt(now.poss);
  //  now.sumdirt = 0;
   // vvl results;
    
    int pos1,pos2;
    bool pos12update = true;
    vi Lpos,Rpos;
    vl BanScore;
    
    dd start_temp = 90;
    dd end_temp = 0;
    dd start_time = stop_watch();
    dd end_time = TIME_LIMIT;
    
    dd now_time;
    dd temp;
    ll RET1;
    vl KEIRET2;
    
    vi BEST_POSS = now.poss;
    dd BEST_SCORE = (dd)now.sumdirt / ((int)now.poss.size() - 1);
    
    rep(loop,1,10000000){
        if((loop & 127) == 1){
            now_time = stop_watch();
            temp = start_temp + (end_temp - start_temp) * (now_time - start_time) / (end_time - start_time);
        }
        if(now_time > TIME_LIMIT){
          //  cerr<<"[DATA] loop2  = "<<loop<<endl;
            break;
        }
        
        int L = now.poss.size() - 1;
        
        //pos1,pos2の定義がannealingと違うことに注意
        //pos1〜pos2を端点を含んで変更する

        
        vi pre_path;
        
        if(pos12update){
            pos12update = false;
          //  pos1 = rand(1,L-2);
            pos1 = rand(1,L/2);
            pos2 = min(L-1,pos1 + rand(0,MaxLen - 3));//加える下限が 0 ↔︎ 長さの下限は 1
            
            bool NG = false;
            CalcLposRpos(now.poss,pos1,pos2,Lpos,Rpos,NG);
            if(NG){
                pos12update = true;
                continue;
            }
            rep(t,pos1,pos2)pre_path.pb(now.poss[t]);
            RET1 = calcRET1(Lpos,Rpos,pos1,pos2,pre_path);
            KEIRET2 = calcKEIRET2(Lpos,Rpos,pos1,pos2);
        }else{
            int move_len = 50;
            if(pos2 + move_len >= L-1){
                pos12update = true;
                continue;
            }
            bool NG = false;
            rep(move,1,move_len){
                OneMovePos1Pos2(now.poss,now.next,pos1,pos2,Lpos,Rpos,RET1,KEIRET2,NG);
                if(NG)break;
                //assert(KEIRET2 == calcKEIRET2(Lpos,Rpos,pos1,pos2));//OK!
            }
            if(NG){
                pos12update = true;
                continue;
            }
            
            pre_path.clear();
            rep(t,pos1,pos2)pre_path.pb(now.poss[t]);
            
          //  assert(RET1 == calcRET1(Lpos,Rpos,pos1,pos2,pre_path));//OK!
            
        }
        
        
        bool first_try_loop = true;
        
        vi best_path;
        dd best_score = inf;
        ll best_sumdirt;
        int best_try_loop = -1;
        
        int type;
        if(rand(1,100) <= 90){
            type = 4;
        }else{
            type = 2;
            BanScore = CalcBanScore(Lpos,Rpos,pos1,pos2);
        }
        int try_loop_limit = (type==4?50:10);
        
        rep(try_loop,1,try_loop_limit){
            vi path;
          //  path = FindPath(now.poss[pos1-1],now.poss[pos2+1],rand(3,MaxLen));
          //  if(path.empty())continue;
          //  path = FindPathByBanScore3(BanScore,now.poss[pos1-1],now.poss[pos2+1],try_loop == 1);//pos1とpos2を含んで変更なので
            if(type == 4){
                path = FindPath4(now.poss[pos1-1],now.poss[pos2+1],try_loop==1);
            }else if(type == 2){
                path = FindPathByBanScore2(BanScore,now.poss[pos1-1],now.poss[pos2+1],try_loop == 1);
            }else{
                assert(0);
            }
           
            if(path.empty())break;
            
            path.pop_back();
            path.erase(path.begin());
            if(path == pre_path)continue;
            
            ll dif_sumdirt = RET1 + calcRET2(Lpos,Rpos,pos1,pos2,L,path,KEIRET2);
            first_try_loop = false;
            //assert(dif_sumdirt <= 1e15/2);//OK！
            //if(dif_sumdirt > 1e15/2)break;//continueではないことに注意
            ll new_sumdirt = now.sumdirt + dif_sumdirt;
            dd new_score = (dd)new_sumdirt/(now.poss.size() + path.size() - pre_path.size() -1);
            if(chmin(best_score , new_score)){
                best_path = path;
                best_sumdirt = new_sumdirt;
                best_try_loop = try_loop;
            }
        }
        dd now_score = (dd)now.sumdirt/(now.poss.size()-1);
        
        if(HANTEI(best_score - now_score,temp)){
            
            //cerr<<pre_path.size()<<"→"<<best_path.size()<<endl;
            vi newposs;
            
            rep(i,pos2+1,L-1)newposs.pb(now.poss[i]);
            rep(i,0,pos1-1)newposs.pb(now.poss[i]);
            newposs.insert(newposs.end(),all(best_path));
            newposs.pb(now.poss[pos2+1]);
            
            now.sumdirt = best_sumdirt;
            now.poss = newposs;
            now.fillnext();
           // results.pb({loop,llround(best_score),best_try_loop});
            //assert(now.sumdirt == CalcSumdirt(now.poss));//OK!
            pos12update = true;
            if(chmin(BEST_SCORE,(dd)now.sumdirt/((int)now.poss.size() - 1))){
                BEST_POSS = now.poss;
            }
        }
    }
    /*
    for(vl result:results){
        //loop , cost
        for(ll x:result)cerr<<x<<" ";cerr<<endl;
    }
     //*/
    return BEST_POSS;
}
vi cyclic(vi ans){
    int sz = ans.size();
    
    int f0 = -1;
    rep(i,0,sz-1){
        if(ans[i] == 0){
            f0 = i;
            break;
        }
    }
    assert(f0 != -1);
    vi ret;
    rep(i,f0,sz-2)ret.pb(ans[i]);
    rep(i,0,f0)ret.pb(ans[i]);
    return ret;
}

void solve_21_11(){
    //21_10
    //2周
    
    ToConvert1D();
    calcSE();
    
    dd Greedy_Time_Limit;
    
    if(N__ <= 28){
        Greedy_Time_Limit = 400;
    }else if(N__ <= 31){
        Greedy_Time_Limit = 500;
    }else{
        Greedy_Time_Limit = 600;
    }
    
    dd LR_Time_Limit = Greedy_Time_Limit + 100;
    dd anneal_Time_Limit1 = 1000;
    dd anneal_Loop_Limit = 3e5;
    dd yamanobori_Time_Limit1 = 1250;
    
    dd anneal_Time_Limit2 = 1500;
    dd yamanobori_Time_Limit2 = 1970;
    
    
#if defined(LOCAL)
    dd keisuu = 0.72;
    Greedy_Time_Limit *= keisuu;
    LR_Time_Limit *= keisuu;
    anneal_Time_Limit1 *= keisuu;
    anneal_Time_Limit2 *= keisuu;
    //anneal_Loop_Limit *= keisuu;
    yamanobori_Time_Limit1 *= keisuu;
    yamanobori_Time_Limit2 *= keisuu;
#endif
    
    vi ans = greedy_3_2(Greedy_Time_Limit,100000);
  //  cerr<<"greedy終了 "<<stop_watch()<<"ms"<<endl;
  //  cerr<<"[DATA] greedy_size =  "<<ans.size()<<endl;
    
    int LRsize;
    if(N__ <= 22)LRsize = 4000;
    else if(N__ <= 28)LRsize = 6000;
    else if(N__ <= 31)LRsize = 8000;
    else if(N__ <= 34)LRsize = 10000;
    else if(N__ <= 37)LRsize = 12000;
    else LRsize = 14000;
    ans = LRsearch(ans,LR_Time_Limit,LRsize);

  //  cerr<<"LRsearch終了 "<<stop_watch()<<"ms"<<endl;
  //  cerr<<"ans.size() "<<ans.size()<<endl;
   // cerr<<"[DATA] LR_size = "<<ans.size()-1<<endl;
    
   // ans = cyclic(ans);OUTPUT(ans);return;
    
    ans = annealing(ans,anneal_Time_Limit1,anneal_Loop_Limit);
    ans = yamanobori_Lchange(ans,yamanobori_Time_Limit1);
    ans = annealing(ans,anneal_Time_Limit2,anneal_Loop_Limit);
    ans = yamanobori_Lchange(ans,yamanobori_Time_Limit2);
    
    ans = cyclic(ans);
  //  cerr<<"[DATA] L = "<<ans.size()-1<<endl;
    OUTPUT(ans);
}

signed main(int argc,char *argv[]){
    fastio
    clock_start
    ///////////////////////////////////////
    
    if(argc >= 2){
        rep(i,1,argc-1){
            PARAMS.pb(stoi(argv[i]));
        }
    }
    
#if defined(LOCAL) || defined(ATCODER)
    INPUT();
#else
    INPUT_FOR_PROFILE();
#endif
    
    solve_21_11();
    
   // DEB(stop_watch());
    
#if defined(ATCODER)
    quick_exit(0);
#endif
    
    return 0;
}


