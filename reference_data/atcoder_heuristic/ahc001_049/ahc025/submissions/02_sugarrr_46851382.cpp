#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include<bits/stdc++.h>
#include<atcoder/all>
using namespace std;
using namespace atcoder;
typedef long long ll;
//typedef int ll;

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
#define rep(i,l,r) for(ll i=l;i<=r;i++)
#define rrep(i,r,l) for(ll i=r;i>=l;i--)
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
//*
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

//#define endl "\n"  //インタラクティブで消す！！！！！！！！！！！！！！！！！！！！！
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
ll rand(ll a,ll b){
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

bool TimeGiveUp = false;
dd LIMIT_TIME = 1950;

ll XLIMIT;

ll N,D,Q;
vl hidden;
void INPUT_FOR_PROFILE(){
    N = 100;
   // D = 25;
    D = 2;
    Q = N * 32;
    auto Exp=[](){
        const dd lamda = 1e-5;
        ll ret = -1/lamda * log(randdouble(0,1));
        return ret;
    };
    hidden.resize(N);
    rep(i,0,N-1){
        do{
            hidden[i] = max(1,llround(Exp()));
            if(hidden[i] >= 1e5 * N / D){
                hidden[i] = -1;
            }
        }while(hidden[i] == -1);
    }
}
ll hidden_sum(vl v){
    #if defined(ATCODER)
        return 0;
    #else
        ll sum = 0;
        for(ll x:v)sum += hidden[x];
        return sum;
    #endif
}


void INPUT(){
    cin>>N>>D>>Q;
    #if defined(LOCAL)
        hidden.resize(N);
        rep(i,0,N-1)cin>>hidden[i];
    #endif
    
}
vd PARAMS;

const ll Left = 1;
const ll Right = 2;
const ll Equal = 3;

ll AskCount = 0;
void Answer(vl d,bool debug = false){
    assert((int)d.size() == N);
    if(debug){
        cout<<"#c ";
    }else{
        assert(AskCount == Q);
    }
    rep(i,0,N-1)cout<<d[i]<<" ";cout<<endl;
    fflush(stdout);
}

vl PickGroup(vl d,ll g){
    assert(0<=g && g<=D-1);
    vl ret;
    rep(i,0,N-1){
        if(d[i] == g)ret.pb(i);
    }
    return ret;
}

vl ADD(vl a,vl b){
    /*
    vl cnt(N);
    for(ll x:a){
        assert(0<=x && x<=N-1);
        cnt[x]++;
        assert(cnt[x] == 1);
    }
    for(ll x:b){
        assert(0<=x && x<=N-1);
        cnt[x]++;
        assert(cnt[x] == 1);
    }*/
    vl c = a;
    c.insert(c.end(),all(b));
    return c;
}
vl SUB(vl a,vl b){
    //return a-b
    
    vl cnt(N);
    for(ll x:a){
        assert(0<=x && x<=N-1);
        cnt[x]++;
        assert(cnt[x] == 1);
    }
    for(ll x:b){
        assert(0<=x && x<=N-1);
        assert(cnt[x] == 1);
        cnt[x] = 0;
    }
    vl c;
    rep(i,0,N-1)if(cnt[i] == 1)c.pb(i);
    return c;
}
ll Exp(){
    const dd lamda = 1e-5;
    ll ret = -1/lamda * log(randdouble(0,1));
    return ret;
}
vl pred;
ll pred_sum(vl L){
    ll sum = 0;
    for(ll x:L)sum += pred[x];
    return sum;
}
/*
void check_variance(){
    vl hosei_pred(N);
    ll pred_sum = 0;
    ll hidd_sum = 0;
    rep(i,0,N-1)pred_sum += pred[i];
    rep(i,0,N-1)hidd_sum += hidden[i];
    rep(i,0,N-1)hosei_pred[i] = pred[i] * hidd_sum / pred_sum;
    ll zure = 0;
    ll bunsan = 0;
    rep(i,0,N-1){
        zure += abs(hidden[i] - hosei_pred[i]);
        bunsan += powl(hidden[i] - hosei_pred[i],2);
    }
    bunsan = bunsan/N;
    zure = zure/N;
    DEB(sqrt(bunsan));
    DEB(zure);
}
 */
/*
void check_variance_XLIMIT(){
    vl hosei_pred(XLIMIT);
    ll pred_sum = 0;
    ll hidd_sum = 0;
    rep(i,0,XLIMIT-1)pred_sum += pred[i];
    rep(i,0,XLIMIT-1)hidd_sum += hidden[i];
    rep(i,0,XLIMIT-1)hosei_pred[i] = pred[i] * hidd_sum / pred_sum;
    ll zure = 0;
    ll bunsan = 0;
    rep(i,0,XLIMIT-1){
        zure += abs(hidden[i] - hosei_pred[i]);
        bunsan += powl(hidden[i] - hosei_pred[i],2);
    }
    bunsan = bunsan/XLIMIT;
    zure = zure/XLIMIT;
    DEB(sqrt(bunsan));
    DEB(zure);
}
 */
void solve2_After_Ask_avoidRE(vl L,vl R){
    // L <= Rと条件が与えられた
    // predを更新していく
    static bool isFirst = true;
    static vvl nows;
    static vvl Ls;
    static vvl Rs;
    Ls.pb(L);
    Rs.pb(R);
    ll T = 1;
    if(isFirst){
        isFirst = false;
        nows.resize(T);
        rep(i,0,T-1){
            rep(j,0,N-1){
                nows[i].pb(llround(Exp()));
                //nows[i].pb(1e5);
            }
        }
    }
    auto sum_now = [](vl &now,vl &L){
        ll suml = 0;
        for(ll x:L)suml += now[x];
        return suml;
    };
    auto ask_now = [&](vl &now,vl &L,vl &R){
        assert(!L.empty() && !R.empty());
        ll suml = sum_now(now,L);
        ll sumr = sum_now(now,R);
        return suml <= sumr;
    };
    
    bool isCheckNeccesary = false;
    for(vl now:nows){
        ll sumL = sum_now(now,L);
        ll sumR = sum_now(now,R);
        if(sumL > sumR)isCheckNeccesary = true;
    }
    ll skip_mod;
    if(Q <= 500)skip_mod = 1;
    else if(Q <= 1000)skip_mod = 2;
    else if(Q <= 1500)skip_mod = 3;
    else if(Q <= 2000)skip_mod = 6;
    else skip_mod = 10;
    
    if(isCheckNeccesary){
        vvl OK_nows;
        for(vl now:nows){
            ll loop_limit = 1000;
            rep(loop,1,loop_limit){
                bool upd = false;
                for(ll zz = loop % skip_mod;zz <= (int)Ls.size()-1;zz += skip_mod){
                    
                    vl &L = Ls[zz];
                    vl &R = Rs[zz];
                    ll sumL = sum_now(now,L);
                    ll sumR = sum_now(now,R);
                    
                    
                    if(sumL < sumR)continue;
                 //   ll add = (sumR - sumL)/2 + 5;
                    
                    ll add;
                    ll start_move = 50000;
                    ll end_move = 2000;
                  //  add = start_move - ((start_move-end_move) *Query_Loop)/1000;
                    add = start_move - (start_move-end_move) * loop/loop_limit;
                  //  add *= randdouble(0.3,1.3);
                    for(ll l:L){
                        now[l] -= add;// + rand(-10000,10000);
                        chmax(now[l],1);
                    }
                    for(ll r:R)now[r] += add;// + rand(-10000,10000);
                     
                    upd = true;
                    
                   
                }
                
                bool breakFlag = false;
                if(loop == loop_limit)breakFlag = true;
                if((dd)AskCount/Q <= stop_watch()/LIMIT_TIME){
                    breakFlag = true;
                }
                // if(loop == loop_limit || ((dd)AskCount/Q <= stop_watch()/1850)){
                if(breakFlag){
                   //  cerr<<"失敗 loopは "<<loop<<endl;
                     OK_nows.pb(now);
                     break;
                 }
                 if(!upd){
                     OK_nows.pb(now);
                    // DEB(loop);
                     break;
                 }
            }
        }
        nows = OK_nows;
    }
    
    
    //DEB(nows.size());
    assert(!nows.empty());
    rep(j,0,N-1){
        ll sum = 0;
        rep(i,0,nows.size()-1){
            sum += nows[i][j];
        }
        pred[j] = sum / nows.size();
    }
    ll pred_ave = 0;
    rep(i,0,N-1)pred_ave += pred[i];
    pred_ave /= N;
    rep(i,0,N-1)pred[i] = pred[i] * 1e5 / pred_ave;
    /*
    #ifdef ATCODER
    #else
       // check_variance();
      //  check_variance_XLIMIT();
    #endif
     */
}

ll Lucky_Ask_cnt = 0;
ll Lucky_Ask2_cnt = 0;

//map<pair<vl,vl>,ll>mp;

typedef bitset<200> BS2;
unordered_map<BS2,ll> MP;
BS2 Con(BS2 L,BS2 R){
    return (L<<100) | R;
}
BS2 to_BS2(vl v){
    BS2 ret;
    for(ll x:v)ret[x] = true;
    return ret;
}
BS2 maskR;

ll Ask(vl L,vl R,bool waste = false){
    if(waste){
        cerr<<"Waste "<<AskCount<<endl;
        AskCount++;
        cout<<1<<" "<<1<<" "<<0<<" "<<1<<endl;
        fflush(stdout);
    #if defined(ATCODER)
        string s;
        cin>>s;
    #endif
        return 0;
    }
    
    //軽い方の番号(Left or Right or Equal) をreturnする
    {
        auto count=[](vl v){
            vl cnt(N);
            for(ll x:v){
                assert(cnt[x] == 0);
                cnt[x]++;
            }
            return cnt;
        };
        vl cntL = count(L);
        vl cntR = count(R);
        vl newL,newR;
        for(ll x:L)if(cntR[x]==0)newL.pb(x);
        for(ll x:R)if(cntL[x]==0)newR.pb(x);
        L = newL;
        R = newR;
    }
    BS2 bsL = to_BS2(L);
    BS2 bsR = to_BS2(R);
    
    if(L.empty() && R.empty())return Equal;
    if(L.empty())return Left;
    if(R.empty())return Right;
    if(MP.find(Con(bsL,bsR)) != MP.end()){
        return MP[Con(bsL,bsR)];
    }
    /*
    DEB(hidden_sum(L));
    DEB(hidden_sum(R));
    DEB(hidden_sum(R) - hidden_sum(L));
    DEB(pred_sum(L));
    DEB(pred_sum(R));
    DEB(pred_sum(R) - pred_sum(L));
     */

    for(auto [LLRR,result]:MP){
        auto Include=[](BS2 A,BS2 a){
            //Aがaを含むかどうか
            if((A|a) != A)return false;
            return true;
        };
        BS2 LL = LLRR >> 100;
       // BS2 RR = (LLRR << 100) >> 100;
        BS2 RR = (LLRR & maskR);
        
        BS2 Lxor = (LL^bsL);//LLとLがいずれかの含有関係にあるなら補集合を意味。そうでないなら特に意味がない
        BS2 Rxor = (RR^bsR);

      //  if(1){
        if(Q <= 1000){
            if(result != Right){
                //LL <= RR
                if(Include(LL,bsL) && Include(bsR,RR)){
                    Lucky_Ask2_cnt++;
                  //  assert(hidden_sum(L) < hidden_sum(R));
                    MP[Con(bsL,bsR)] = Left;
                    MP[Con(bsR,bsL)] = Right;
                    return Left;
                }
            }
            if(result != Left){
                //LL >= RR
                if(Include(bsL,LL) && Include(RR,bsR)){
                    Lucky_Ask2_cnt++;
                  //  assert(hidden_sum(L) > hidden_sum(R));
                    MP[Con(bsL,bsR)] = Right;
                    MP[Con(bsR,bsL)] = Left;
                    return Right;
                }
            }
        }
      //  if(Q <= 1000){
        if(1){
            if(Include(LL,bsL) && Include(RR,bsR)){
                if(MP.find(Con(Lxor,Rxor)) != MP.end() && result != MP[Con(Lxor,Rxor)]){
                    Lucky_Ask_cnt++;
                    if(result == Equal){
                        ll result_xor = MP[Con(Lxor,Rxor)];
                        MP[Con(bsL,bsR)] = (result_xor == Left?Right:Left);
                        MP[Con(bsR,bsL)] = result_xor;
                        /*
                        if(MP[Con(bsL,bsR)] == Left){
                            assert(hidden_sum(L) < hidden_sum(R));
                        }else{
                            assert(hidden_sum(L) > hidden_sum(R));
                        }
                         */
                        return result;
                    }else{
                        MP[Con(bsL,bsR)] = result;
                        MP[Con(bsR,bsL)] = (result == Left?Right:Left);
                        /*
                        if(MP[Con(bsL,bsR)] == Left){
                            assert(hidden_sum(L) < hidden_sum(R));
                        }else{
                            assert(hidden_sum(L) > hidden_sum(R));
                        }
                         */
                        return result;
                    }
                }
            }
        }
    }
    
  //  cerr<<"# "<<AskCount<<endl;
  //  cout<<"# "<<AskCount<<endl;
    assert(AskCount < Q);
    AskCount++;
    assert(!L.empty());
    assert(!R.empty());
    cout<<L.size()<<" "<<R.size()<<" ";
    for(ll x:L)cout<<x<<" ";
    for(ll x:R)cout<<x<<" ";
    cout<<endl;
    fflush(stdout);
    string s;
#if defined(ATCODER)
    cin>>s;
#else
    ll suml = hidden_sum(L);
    ll sumr = hidden_sum(R);
    if(suml < sumr)s = "<";
    else if(suml > sumr)s = ">";
    else s = "=";
#endif
    if(s == "<"){
        MP[Con(bsL,bsR)] = Left;
        MP[Con(bsR,bsL)] = Right;
       // if(!TimeGiveUp || stop_watch() <= 1000){
        if(1){
            solve2_After_Ask_avoidRE(L,R);
        }
        
        return Left;
    }else if(s == ">"){
        MP[Con(bsL,bsR)] = Right;
        MP[Con(bsR,bsL)] = Left;
    //    if(!TimeGiveUp || stop_watch() <= 1000){
        if(1){
            solve2_After_Ask_avoidRE(R,L);
        }
        return Right;
    }else{
        assert(s == "=");
        MP[Con(bsL,bsR)] = Equal;
        MP[Con(bsR,bsL)] = Equal;
        return Equal;
    }
}
    

void Search_Move_One(vl G1,vl G2,vl &G2a,dd DifGoal,set<vl> &Searchd_A){
    //DEB(DifGoal);
    //G2a - G1a = DifGoal を目指したい
    //G1a = 0なので、 G2a = DifGoalを目指す
  
    dd DifDeltaMin = inf;
    for(ll y:G2){
        vl G2b = SUB(G2,{y});
        //if(mp.find({G1,G2b}) != mp.end())continue;
        if(Searchd_A.find({y}) != Searchd_A.end())continue;
        dd Dif = pred_sum({y});
        dd DifDelta = abs(Dif - DifGoal);
        if(chmin(DifDeltaMin,DifDelta)){
            G2a = {y};
        }
    }
    Searchd_A.insert(G2a);
    DEB(DifGoal);
    DEB(DifDeltaMin);
    DEB(pred_sum(G2a));
    DEB(hidden_sum(G2a));
    /*
    {
        vl v;
        for(ll y:G2)v.pb(pred[y].Expect());
        SORT(v);
        DEB(v);
    }
     */
}
void Search_Swap_Pair(vl G1,vl G2,vl &G1a,vl &G2a,dd DifGoal,set<vl>&Searchd_A){
    //G2a - G1a = DifGoal を目指したい
    //DEB(DifGoal);
    dd DifDeltaMin = inf;
    for(ll x:G1){
        for(ll y:G2){
            //if(mp.find({{x},{y}}) != mp.end())continue;
            {
                vl g1ag2a = {x,y};
                SORT(g1ag2a);
                if(Searchd_A.find(g1ag2a) != Searchd_A.end())continue;
            }
            dd Dif = pred_sum({y}) - pred_sum({x});
            dd DifDelta = abs(Dif - DifGoal);
            if(chmin(DifDeltaMin,DifDelta)){
                G1a = {x};
                G2a = {y};
            }
        }
    }
    DEB(DifGoal);
    DEB(DifDeltaMin);
    DEB(pred_sum(G1a));
    DEB(pred_sum(G2a));
    DEB(hidden_sum(G1a));
    DEB(hidden_sum(G2a));
    vl g1ag2a = ADD(G1a,G2a);
    SORT(g1ag2a);
    Searchd_A.insert(g1ag2a);
    
   // DEB(DifGoal);
   // DEB(DifDeltaMin);
}


struct g_data{
    vl g;
    ll psum;
};
void rekkyo_data(vector<g_data> &data,vl G_old,bool empty_ok){
    vvl gs;
    if(empty_ok){
        gs.pb({});
    }
    vl G;
    for(ll x:G_old){
        if(x >= XLIMIT)continue;
        G.pb(x);
    }
    {
        ll xlimit_now = XLIMIT;
        while((int)G.empty()){
            cerr<<"[DATA] __!!!___ = "<<1<<endl;
            if(xlimit_now == N)break;
            xlimit_now += 10;
            for(ll x:G_old){
                if(x >= xlimit_now)continue;
                G.pb(x);
            }
        }
    }
    
    if((int)G.size() <= 8){
        ll Gsz = G.size();
        rep(i,1,(1<<Gsz)-1){
            vl g;
            rep(j,0,Gsz-1){
                if((i>>j)&1){
                    g.pb(G[j]);
                }
            }
            gs.pb(g);
        }
    }else{
        rep(try_loop,1,300){
            dd p = randdouble(0.1,0.4);
            vl g;
            for(ll x:G){
                if(randdouble(0,1) < p){
                    g.pb(x);
                }
            }
            if(g.empty())continue;
            gs.pb(g);
        }
        ERASE(gs);
    }
    

    
    ll gsz = gs.size();
    vl sum(gsz);
    rep(i,0,gsz-1)sum[i] = pred_sum(gs[i]);
    vl_l ids;
    rep(i,0,gsz-1){
        ids.pb({sum[i],i});
    }
    SORT(ids);
    for(auto[sum,num]:ids){
        data.pb({gs[num],sum});
    }
}
void Search_Swap_Set_Parallel(vl G1,vl G2,vl &L,vl &R,set<vl>&SearchdLR){

    vector<g_data> data1,data2;
    rekkyo_data(data1,G1,false);
    rekkyo_data(data2,G2,false);
    
    
    vl big_min(data2.size());//data2[i].psumを、data1[j].psumが超えるjのなかで、最大のもの(j)を保存
    ll pos1 = 0;
    rep(pos2,0,(int)data2.size()-1){
        while(pos1 <= (int)data1.size()-1 && data1[pos1].psum < data2[pos2].psum)pos1++;
        big_min[pos2] = pos1;
    }
    struct kouho{
        vl g1,g2;
        ll wdif;
        dd penalty;
    };
    vector<kouho>kouhos;
    rep(pos2,0,(int)data2.size()-1){
        rep(pos1,big_min[pos2]-1,big_min[pos2]){
            if(pos1 < 0 || pos1 >= (int)data1.size())continue;
            kouhos.pb({data1[pos1].g,data2[pos2].g,pred_sum(data2[pos2].g) - pred_sum(data1[pos1].g)});
        }
    }
    assert(!kouhos.empty());
    
    for(kouho&k:kouhos){
        auto func=[](kouho a){
            if(Q <= 300 || (Q <= 750 && XLIMIT > 50)){
                return (100 + abs(a.wdif)) * powl(2,a.g1.size() + a.g2.size());
            }else{
                return (dd)abs(a.wdif);
            }
        };
        k.penalty = func(k);
    }
    ll ksz = kouhos.size();
    vector<pair<dd,ll>> ids(ksz);
    rep(i,0,ksz-1)ids[i] = {kouhos[i].penalty,i};
    SORT(ids);
    rep(z,0,999){
        if(z >= (int)kouhos.size())return;
        //kouhosのなかでpenaltyが少ないものからuseKを選ぶ
       // kouho useK = kouhos[ids[rand(0,min(3,ksz-1))].se];
        kouho useK = kouhos[ids[z].se];
        L = useK.g1;
        R = useK.g2;
        vl LR = ADD(L,R);
        if(SearchdLR.find(LR) == SearchdLR.end()){
            SearchdLR.insert(LR);
            DEB(L);
            DEB(R);
            DEB(pred_sum(L));
            DEB(pred_sum(R));
            DEB(hidden_sum(L));
            DEB(hidden_sum(R));
            return;
        }
    }
};



ll XLIMIT_optimize(){
    vvl datas = {
        //D,N,Q,Xlimit
        {2,30,60,9},
        {2,30,120,14},
        {2,30,240,14},
        {2,30,480,18},
        {2,30,960,26},
        
        {2,45,90,18},
        {2,45,180,18},
        {2,45,360,18},
        {2,45,720,20},
        {2,45,1440,26},
        
        {2,60,120,18},
        {2,60,240,18},
        {2,60,480,18},
        {2,60,960,26},
        {2,60,1920,26},
        
        {2,80,160,18},
        {2,80,320,18},
        {2,80,640,18},
        {2,80,1280,26},
        {2,80,2560,26},
        
        {2,100,200,16},
        {2,100,400,19},
        {2,100,800,23},
        {2,100,1600,26},
        {2,100,3200,26},
        
        {4,30,60,16},
        {4,30,120,18},
        {4,30,240,25},
        {4,30,480,30},
        {4,30,960,30},
        
        {4,45,90,20},
        {4,45,180,24},
        {4,45,360,25},
        {4,45,720,35},
        {4,45,1440,45},
        
        {4,60,120,26},
        {4,60,240,26},
        {4,60,480,26},
        {4,60,960,35},
        {4,60,1920,45},
        
        {4,80,160,26},
        {4,80,320,26},
        {4,80,640,30},
        {4,80,1280,45},
        {4,80,2560,45},
        
        {4,100,200,26},
        {4,100,400,26},
        {4,100,800,35},
        {4,100,1600,45},
        {4,100,3200,45},
        
        {5,30,60,24},
        {5,30,120,27},
        {5,30,240,30},
        {5,30,480,30},
        {5,30,960,30},
        
        {5,45,90,26},
        {5,45,180,26},
        {5,45,360,30},
        {5,45,720,45},
        {5,45,1440,45},
        
        {5,60,120,30},
        {5,60,240,35},
        {5,60,480,35},
        {5,60,960,45},
        {5,60,1920,60},
        
        {5,80,160,35},
        {5,80,320,35},
        {5,80,640,35},
        {5,80,1280,50},
        {5,80,2560,60},
        
        {5,100,200,35},
        {5,100,400,35},
        {5,100,800,35},
        {5,100,1600,50},
        {5,100,3200,60},
        
        {7,30,60,30},
        {7,30,120,30},
        {7,30,240,30},
        {7,30,480,30},
        {7,30,960,30},
        
        {7,45,90,45},
        {7,45,180,45},
        {7,45,360,45},
        {7,45,720,45},
        {7,45,1440,45},
        
        {7,60,120,35},
        {7,60,240,35},
        {7,60,480,40},
        {7,60,960,60},
        {7,60,1920,60},
        
        {7,80,160,40},
        {7,80,320,40},
        {7,80,640,40},
        {7,80,1280,60},
        {7,80,2560,70},
        
        {7,100,200,60},
        {7,100,400,60},
        {7,100,800,60},
        {7,100,1600,60},
        {7,100,3200,60}
        
    };
    ll KEISUU = 10000;
    for(auto &data:datas){
        data[3] *= KEISUU;
    }
    auto VALID=[](vl data,bool f0,bool f1,bool f2){
        assert((int)data.size() == 4);
        if(!f0){
            if(data[0] > D)return false;
        }else{
            if(data[0] < D)return false;
        }
        if(!f1){
            if(data[1] > N)return false;
        }else{
            if(data[1] < N)return false;
        }
        if(!f2){
            if(data[2] > Q)return false;
        }else{
            if(data[2] < Q)return false;
        }
        return true;
    };
    vvl B(8);
    rep(i,0,7){
        vl da;
        for(vl data:datas){
            if(VALID(data,(i>>0)&1,(i>>1)&1,(i>>2)&1)){
                if(da.empty())da = data;
                else{
                    assert((int)da.size() == 4);
                    if(!((i>>2)&1)){
                        if(da[2] < data[2])da = data;
                    }else{
                        if(da[2] > data[2])da = data;
                    }
                }
            }
        }
      //  assert(!da.empty());
        B[i] = da;
    }
    auto SENKEI=[](vl a,vl b,ll pos,ll mid){
        assert(!a.empty() || !b.empty());
        if(b.empty())swap(a,b);
        if(a.empty()){
            assert((int)b.size() == 4);
            return b;
        }
        assert((int)a.size() == 4);
        assert((int)b.size() == 4);
        rep(i,0,2){
           // if(i!=pos)assert(a[i] == b[i]);
        }
        assert(a[pos] <= mid && mid <= b[pos]);
        ll ap = a[pos];
        ll bp = b[pos];
        if(ap == bp){
            return a;
        }
        vl c(4);
        rep(i,0,3){
            c[i] = a[i] + llround((dd)(b[i] - a[i]) * (mid - ap) / (dd)(bp - ap));
        }
        return c;
    };
    DEB(B);
    vvl C(4);
    rep(i,0,3){
        C[i] = SENKEI(B[i],B[i+4],2,Q);
    }
    DEB(C);
    vvl E(2);
    rep(i,0,1){
        E[i] = SENKEI(C[i],C[i+2],1,N);
    }
    DEB(E);
    vl F = SENKEI(E[0],E[1],0,D);
    DEB(F);
    
   // cerr<<"XLIMIT_optimize "<<stop_watch()<<"ms"<<endl;
    ll ret = llround((dd)F[3]/KEISUU);
    DEB(ret);
    return ret;
    
}

    
void solve_last(){
    //18_6からコメント削除
    
    
    TimeGiveUp = true;
    {
        rep(i,0,N-1)pred.pb(1e5);
    }
    XLIMIT = N;
    if(D <= 7){
        chmin(XLIMIT,XLIMIT_optimize());
    }else if(D <= 12){
        chmin(XLIMIT,40 + ((D - 4) * 25 / 4));
        //(D,xlimit) = (4,40) , (8,65) , (12,90)
    }
    rep(i,0,99)maskR[i] = 1;
    
    vl d(N);
    rep(i,0,N-1)d[i] = i%D;
    auto sort_init=[](vl d){
        vl ret(D);rep(i,0,D-1)ret[i] = i;
        auto sort=[](vl v,vl d,auto sort){
            ll sz = v.size();
            assert(sz >= 1);
            if(sz == 1)return v;
            ll Mid = sz/2;
            vl v1(v.begin(),v.begin()+Mid);
            vl v2(v.begin()+Mid,v.end());
            assert(v1.size() + v2.size() == v.size());
            v1 = sort(v1,d,sort);
            v2 = sort(v2,d,sort);
            vl new_v;
            ll v1pos = 0;
            ll v2pos = 0;
            ll v1end = v1.size();
            ll v2end = v2.size();
            while(v1pos < v1end || v2pos < v2end){
                assert(AskCount <= Q-1);
                if(v1pos == v1end){
                    new_v.pb(v2[v2pos++]);
                }else if(v2pos == v2end){
                    new_v.pb(v1[v1pos++]);
                }else{
                    vl G1 = PickGroup(d,v1[v1pos]);
                    vl G2 = PickGroup(d,v2[v2pos]);
                    if(Ask(G1,G2) == Left){
                        new_v.pb(v1[v1pos++]);
                    }else{
                        new_v.pb(v2[v2pos++]);
                    }
                }
            }
            assert(v.size() == new_v.size());
            return new_v;
        };
        ret = sort(ret,d,sort);
        return ret;
    };
    vl jun = sort_init(d);
    Answer(d,true);
    
    auto jun_push=[](vl &jun,vl d,ll num){
        ll L = 0;
        ll R = jun.size();
        vl Gnum = PickGroup(d,num);
        //[L,R]
        while(L<R){
            if(AskCount == Q)break;
            ll mid = (L+R)/2;
            vl Gop = PickGroup(d,jun[mid]);
            if(Ask(Gnum,Gop) == Left){
                R = mid;
            }else{
                L = mid + 1;
            }
        }
        jun.insert(jun.begin() + L,num);
    };
    
    dd DifGoalAll = 5e5;
    bool isTimeUp = false;
    vvl skip(D,vl(D));
    while(1){
        
        if(AskCount == Q)break;
        bool update = false;
        
        ll challenge_ID = 0;
        ll failure_loop = 0;
        
        static ll move_one_limit = 20;
        
        if(Q/XLIMIT <= 4){
            move_one_limit = 20;
        }else if(Q/XLIMIT <= 8){
            move_one_limit = max(1,move_one_limit-1);
        }else{
            chmin(move_one_limit,10);
            move_one_limit = max(1,move_one_limit-1);
        }
        
        
        set<vl> Searchd_A;
      //  cerr<<"---新しい探索開始---"<<endl;
        while(1){
            ll EvacL = 0;
            ll EvacR = D-1;
            
            while(PickGroup(d,jun[EvacR]).size() == 1){
                rep(i,0,D-1)skip[i][EvacR] = 1;
                EvacR--;
                assert(EvacL < EvacR);
            }
            
            while(skip[EvacL][EvacR]){
                if(EvacL + 1 < EvacR){
                    EvacL ++;
                }else{
                    EvacL = 0;
                    EvacR--;
                }
            }
            if(EvacL >= EvacR){
              //  cerr<<"--------------EvacL,EvacR-----------------"<<endl;
                isTimeUp = true;//時間オーバーってことにしちゃおう
                break;
            }
            assert(EvacL < EvacR);
            if(Q <= 1000){
                while(EvacL + 1 < EvacR && rand(0,9) == 0)EvacL++;
                while(EvacL + 1 < EvacR && rand(0,9) == 0)EvacR--;
            }else{
                while(EvacL + 1 < EvacR && rand(0,3) == 0)EvacL++;
                while(EvacL + 1 < EvacR && rand(0,3) == 0)EvacR--;
            }
            assert(EvacL < EvacR);
            
            ll g1 = jun[EvacL];
            ll g2 = jun[EvacR];
            vl G1 = PickGroup(d,g1);
            vl G2 = PickGroup(d,g2);
            dd G1sum = pred_sum(G1);
            dd G2sum = pred_sum(G2);
            dd DifGoal = min(DifGoalAll , max(0, (G2sum - G1sum) / 2));
            DifGoalAll = DifGoal * 0.5;
            
        //    cout<<"# EvacL,EvacR "<<EvacL<<" "<<EvacR<<endl;
        //    cerr<<"# EvacL,EvacR "<<EvacL<<" "<<EvacR<<endl;
            
        //    cerr<<"本当のDifGoal = ";cerr<<(hidden_sum(G2) - hidden_sum(G1))/2<<endl;
            failure_loop++;
         //   cerr<<"--challenge_ID,failure_loop--:"<<challenge_ID<<","<<failure_loop<<"--"<<endl;
            
            if(TimeGiveUp && stop_watch() > LIMIT_TIME){
                EvacR = 999;
             //   cerr<<"-------------時間で緊急避難----------------"<<endl;
                isTimeUp = true;
                break;
            }
            
            if(update || AskCount == Q)break;
            vl G1a,G2a;
            if(challenge_ID == 0){
                Search_Move_One(G1,G2,G2a,DifGoal,Searchd_A);
                if(G2a.empty() || failure_loop >= move_one_limit){
                    challenge_ID += 2;
                    failure_loop = 0;
                    continue;
                }
            }else if(challenge_ID == 2){
                //Search_Swap_Set(G1,G2,G1a,G2a,Searchd_A);
                //Search_Swap_Set_Dp(G1,G2,G1a,G2a,Searchd_A,DifGoal);
                Search_Swap_Set_Parallel(G1,G2,G1a,G2a,Searchd_A);
                if((G1a.empty() && G2a.empty()) || failure_loop > 10000){
                    //failure_loopの設定だいぶ大きいほうがよさそう
                    
                    challenge_ID ++;
                    failure_loop = 0;
                    continue;
                }
            }else{
                if((int)G2.size() == 1 || EvacL+1 == EvacR){
                    rep(i,0,D-1)skip[i][EvacR] = 1;
                }else{
                    skip[EvacL][EvacR] = 1;
                }
                break;
            }
            if(Ask(G1a,G2a) == Left){
                vl G1b = SUB(G1,G1a);
                vl G2b = SUB(G2,G2a);
                if(AskCount == Q)break;
                if(Ask(G1b,G2b) == Left){
                    DEB(AskCount);
                    for(ll x:G1a)d[x] = g2;
                    for(ll x:G2a)d[x] = g1;
                    update = true;
                    Answer(d,true);
                    jun.erase(jun.begin() + EvacR);
                    jun.erase(jun.begin() + EvacL);
                    jun_push(jun,d,g1);
                    jun_push(jun,d,g2);
                    rep(i,0,D-1)skip[i][EvacR] = 0;
               //     cerr<<"成功,EvacL,R,ID,loop "<<EvacL<<" "<<EvacR<<" "<<challenge_ID<<","<<failure_loop<<endl;
                    break;
                }
            }
        }
        if(isTimeUp)break;
        
        assert(jun.size() == D);
      //  PRINT_JUN(jun);
        Answer(d,true);
    }
    
    while(AskCount < Q){Ask({},{},true);}
    Answer(d,false);
    /*
    {
        struct A{
            ll hid;
            ll pre;
            ll hidnum;
            ll prenum;
        };
        vector<A> v;
        rep(i,0,N-1){
            v.pb({hidden[i],pred[i],0,0});
        }
        sort(all(v),[](A a,A b){return a.hid < b.hid;});
        rep(i,0,N-1)v[i].hidnum = i;
        sort(all(v),[](A a,A b){return a.pre < b.pre;});
        cerr<<"順序を推定できているか？ ";
        rep(i,0,N-1)cerr<<v[i].hidnum<<" ";cerr<<endl;
    }*/
    /*
    {
        struct A{
            ll hid;
            ll pre;
            ll hidnum;
            ll prenum;
        };
        vector<A> v;
        rep(i,0,XLIMIT-1){
            v.pb({hidden[i],pred[i],0,0});
        }
        sort(all(v),[](A a,A b){return a.hid < b.hid;});
        rep(i,0,XLIMIT-1)v[i].hidnum = i;
        sort(all(v),[](A a,A b){return a.pre < b.pre;});
        cerr<<"順序を推定できているか？ ";
        rep(i,0,XLIMIT-1)cerr<<v[i].hidnum<<" ";cerr<<endl;
    }
    cerr<<"[DATA] time = "<<llround(stop_watch())<<endl;
    cerr<<"[DATA] lucky = "<<Lucky_Ask_cnt<<endl;
    cerr<<"[DATA] lucky2 = "<<Lucky_Ask2_cnt<<endl;
    */

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
    
    solve_last();
    
    return 0;
}
