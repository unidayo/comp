#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include<atcoder/all>
#include<bits/stdc++.h>
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
typedef vector<mint>vi;
typedef vector<vi>vvi;
typedef vector<vvi>vvvi;
typedef vector<vvvi>vvvvi;
typedef pair<mint,mint>i_i;
typedef vector<i_i>vi_i;
typedef vector<l_l>vl_l;
typedef vector<vl_l>vvl_l;
typedef vector<string>vs;
typedef vector<vs>vvs;
typedef vector<dd> vd;
typedef vector<vd> vvd;
typedef vector<vvd> vvvd;
typedef vector<d_d>vd_d;
dd PI=acos((dd)-1);

#define fastio ios::sync_with_stdio(false); cin.tie(0); cout.tie(0); cout<<setprecision(20);
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

////////////////////////////////////////////////



string vvl_to_str(vvl a){
    ll n=a.size();
    rep(i,0,n-1){
        rep(j,0,n-1){
            if(i==j)assert(a[i][j]==0);
            else assert(a[i][j]==a[j][i]);
        }
    }
    string s;
    rep(i,0,n-1){
        rep(j,i+1,n-1){
            s += (char)(a[i][j] + '0');
        }
    }
    return s;
}
vvl str_to_vvl(string s){
    ll len = s.size();
    ll n=-1;
    rep(i,4,100){
        if(i*(i-1)/2 == len){
            n=i;
            break;
        }
    }
    assert(n!=-1);
    vvl a(n,vl(n));
    ll pos=0;
    rep(i,0,n-1){
        rep(j,i+1,n-1){
            a[i][j] = a[j][i] = s[pos++]-'0';
        }
    }
    return a;
}


struct OUTLINE{
    ll N;
    vvl V;
    bool operator==(const OUTLINE op)const{
        return N==op.N && V==op.V;
    }
};

struct GRAPH{
    OUTLINE OL;
    vl SZ;
    vvl graph;
    bool operator==(const GRAPH op)const{
        return OL==op.OL && SZ==op.SZ;
    }
};
typedef vector<GRAPH> vG;
typedef vector<OUTLINE> vO;

void PRINT_INIT(ll n,vG g){
    cout<<n<<endl;
    for(auto graph:g){
        cout<<vvl_to_str(graph.graph)<<endl;
    }
    fflush(stdout);
}
void PRINT_ANS(ll n){
    cout<<n<<endl;
    fflush(stdout);
}

void MAKE_GRAPH(GRAPH &g){
    vl flag;
    rep(i,0,(int)g.SZ.size()-1){
        rep(zz,1,(int)g.SZ[i]){
            flag.pb(i);
        }
    }
    ll n=flag.size();
    vvl a(n,vl(n));
    rep(i,0,n-1){
        rep(j,0,n-1){
            if(i==j)continue;
            a[i][j] = g.OL.V[flag[i]][flag[j]];
        }
    }
    g.graph = a;
    
}
/*
ll GRAPH_DIFF(GRAPH &a,GRAPH &b){
    ll cnt=0;
    ll n=a.graph.size();
    rep(i,0,n-1){
        rep(j,0,n-1){
            if(a.graph[i][j] != b.graph[i][j])cnt++;
        }
    }
    return cnt;
}
ll GRAPH_DIFF_FAST(GRAPH &a,GRAPH &b){
    ll n=a.graph.size();
    vl v={0};
    {
        ll pos=0;
        for(auto x:a.SZ){
            pos+=x;
            v.pb(pos);
        }
        pos=0;
        for(auto x:b.SZ){
            pos+=x;
            v.pb(pos);
        }
    }
    ERASE(v);
    vl fa,fb;
    {
        ll I=0;
        for(auto x:a.SZ){
            rep(zz,1,x)fa.pb(I);
            I++;
        }
        I=0;
        for(auto x:b.SZ){
            rep(zz,1,x)fb.pb(I);
            I++;
        }
    }
    ll vs=v.size();
    vl ga(vs-1),gb(vs-1);
    rep(i,0,vs-2){
        ga[i] = fa[v[i]];
        gb[i] = fb[v[i]];
    }
    
    ll cnt=0;
    rep(i,0,vs-2){
        rep(j,0,vs-2){
            ll A=a.OL.V[ga[i]][ga[j]];
            ll B=b.OL.V[gb[i]][gb[j]];
            if(A!=B){
                if(i==j){
                    ll hen = v[i+1]-v[i];
                    cnt += hen*hen - hen;
                }else{
                    ll heni = v[i+1]-v[i];
                    ll henj = v[j+1]-v[j];
                    cnt += heni * henj;
                }
            }
        }
    }
    return cnt;
}*/
ll GRAPH_DIFF_FAST2(GRAPH &a,GRAPH &b){
    vl v={0};
    {
        ll pos=0;
        for(auto x:a.SZ){
            pos+=x;
            v.pb(pos);
        }
        pos=0;
        for(auto x:b.SZ){
            pos+=x;
            v.pb(pos);
        }
    }
    ERASE(v);
    ll vs=v.size();
    vl ga(vs-1),gb(vs-1);
    {
        vl sa={0};
        vl sb={0};
        for(auto x:a.SZ)sa.pb(sa.back()+x);
        for(auto x:b.SZ)sb.pb(sb.back()+x);
        
        rep(i,0,vs-2){
            ll num=-1;
            rep(j,0,(int)sa.size()-2){
                if(sa[j]<=v[i] && v[i+1]<=sa[j+1]){
                    assert(num==-1);
                    num=j;
                }
            }
            assert(num!=-1);
            ga[i]=num;
        }
        
        rep(i,0,vs-2){
            ll num=-1;
            rep(j,0,(int)sb.size()-2){
                if(sb[j]<=v[i] && v[i+1]<=sb[j+1]){
                    assert(num==-1);
                    num=j;
                }
            }
            assert(num!=-1);
            gb[i]=num;
        }
    }
    
    ll cnt=0;
    rep(i,0,vs-2){
        rep(j,0,vs-2){
            ll A=a.OL.V[ga[i]][ga[j]];
            ll B=b.OL.V[gb[i]][gb[j]];
            if(A!=B){
                if(i==j){
                    ll hen = v[i+1]-v[i];
                    cnt += hen*hen - hen;
                }else{
                    ll heni = v[i+1]-v[i];
                    ll henj = v[j+1]-v[j];
                    cnt += heni * henj;
                }
            }
        }
    }
    return cnt;
}
ll GRAPH_DIFF_FAST3(GRAPH a,GRAPH b){
    ll ret=inf;
    if(a.OL.N>b.OL.N)swap(a,b);
    vl junb(b.OL.N);rep(i,0,b.OL.N-1)junb[i]=i;
    do{
        GRAPH B;
        B.SZ.resize(b.OL.N);
        rep(i,0,b.OL.N-1)B.SZ[i] = b.SZ[junb[i]];
        
        B.OL.N=b.OL.N;
        B.OL.V.resize(b.OL.N);
        rep(i,0,b.OL.N-1)B.OL.V[i].resize(b.OL.N);
        rep(i,0,b.OL.N-1){
            rep(j,0,b.OL.N-1){
                B.OL.V[i][j] = b.OL.V[junb[i]][junb[j]];
            }
        }
       // DEB(junb);
       // cerr<<GRAPH_DIFF_FAST2(a,B)<<endl;
        chmin(ret,GRAPH_DIFF_FAST2(a,B));
    }while(next_permutation(all(junb)));
    
    return ret;
}


ll calc_miss(vvl &a,OUTLINE &ol,vl &f){
    ll n=a.size();

    //vl CNT(2,1);これ使うなら差分計算のところも変えないとダメ
    
    ll miss=0;
    rep(i,0,n-1){
        rep(j,0,n-1){
            if(i==j)continue;
            if(ol.V[f[i]][f[j]] != a[i][j]){
               // miss += CNT[1-a[i][j]];
                miss++;
            }
        }
    }
    return miss;
}

vvl PATTERN2_1 = {{1,0},{0,0}};
vvl PATTERN2_2 = {{1,1},{1,0}};
vvl PATTERN2_3 = {{1,0},{0,1}};
vvl PATTERN2_4 = {{0,1},{1,0}};
vvl PATTERN3_1 = {{1,1,0},{1,0,0},{0,0,0}};
vvl PATTERN3_2 = {{1,1,1},{1,1,0},{1,0,0}};
vvl PATTERN3_3 = {{1,0,0},{0,1,0},{0,0,1}};
vvl PATTERN3_4 = {{1,0,0},{0,1,0},{0,0,0}};
vvl PATTERN3_5 = {{1,1,0},{1,0,0},{0,0,1}};
vvl PATTERN3_6 = {{0,1,1},{1,0,1},{1,1,0}};
vvl PATTERN3_7 = {{1,1,1},{1,1,0},{1,0,1}};
vvl PATTERN3_8 = {{1,1,1},{1,0,1},{1,1,0}};
vvl PATTERN3_9 = {{0,1,0},{1,0,0},{0,0,0}};
vvl PATTERN3_10= {{1,0,1},{0,1,1},{1,1,0}};

vvl PATTERN4_1 = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
vvl PATTERN4_2 = {{0,1,1,1},{1,0,1,1},{1,1,0,1},{1,1,1,0}};

ll change(vvl &a,vl &f,ll pos,ll val,OUTLINE &ol,ll &n,ll OP=inf){
    //f[pos]をvalに変更した時にmissがいくつ変わるか
    
    ll delta_miss=0;
    rep(i,0,n-1){
        if(i==pos)continue;
        if(i==OP)continue;
        if(ol.V[f[i]][val] != a[i][pos]){
            delta_miss++;
        }
        if(ol.V[f[i]][f[pos]] != a[i][pos]){
            delta_miss--;
        }
    }
    delta_miss *= 2;
    return delta_miss;
};
ll swap(vvl &a,vl &f,ll pos1,ll pos2,OUTLINE &ol,ll &n){
    //f[pos1]とf[pos2]を変更した時にmissがいくつ変わるか
    return change(a,f,pos1,f[pos2],ol,n,pos2) + change(a,f,pos2,f[pos1],ol,n,pos1);
}


vl equidist_vec(ll ko,ll a,ll b){
    //[a,b]でko個(以下)のvector
    //だいたい等間隔
    
    vl ret;
    if(b-a+1<=ko){
        rep(i,a,b)ret.pb(i);
        return ret;
    }
    
    rep(i,1,ko){
        ret.pb(a);
        if(i==ko)break;
        ll add = (llround)((dd)(b-a)/(ko-i));
        a += add;
    }
    return ret;
};
void zeroeps(ll M){
    ll n;
    if(M<=11)n=4;
    else if(M<=34)n=5;
    else n=6;
    
    auto diff=[](vvl a,vvl b,bool samecheck_only=false){
        auto C=[](vvl a){
            ll c=0;
            for(auto x:a)for(auto y:x)c+=y;
            return c;
        };
        if(samecheck_only){
            if(C(a)!=C(b))return 1ll;
        }
        
        ll n=a.size();
        vl jun(n);rep(i,0,n-1)jun[i]=i;
        
        ll MIN=inf;
        do{
            ll cnt=0;
            rep(i,0,n-1){
                rep(j,i+1,n-1){
                    if(a[i][j] != b[jun[i]][jun[j]])cnt++;
                }
            }
            chmin(MIN,cnt);
        }while(next_permutation(all(jun)));
        
        return MIN;
    };
    
    ll k=n*(n-1)/2;
    vvvl vec;
    rep(i,0,(1ll<<k)-1){
        auto make=[](ll I,ll n){
            vvl a(n,vl(n));
            ll pos=0;
            rep(i,0,n-1){
                rep(j,i+1,n-1){
                    a[i][j] = (I>>pos)&1;
                    a[j][i] = a[i][j];
                    pos++;
                }
            }
            return a;
        };
        vvl a = make(i,n);
        
        bool flag=true;
        for(auto b:vec){
            if(diff(a,b,true)==0){
                flag=false;
                break;
            }
        }
        if(flag)vec.pb(a);
        if((int)vec.size()>=M)break;
    }
    cerr<<"vec.size"<<vec.size()<<endl;
    cout<<n<<endl;
    for(auto x:vec)cout<<vvl_to_str(x)<<endl;
    fflush(stdout);
    cerr<<"print"<<stop_watch()<<"ms"<<endl;
    rep(testcases,1,100){
        vvl a;
        {
            string s;cin>>s;
            a=str_to_vvl(s);
        }
        vl_l t;
        rep(i,0,M-1){
            t.pb({diff(a,vec[i]),i});
        }
        SORT(t);
        cout<<t[0].se<<endl;
        fflush(stdout);
    }
    cerr<<"終了までに"<<stop_watch()<<"ms"<<endl;
    exit(0);
}



dd res_time;
ll n;

//vO ols={{2,PATTERN2_1},{2,PATTERN2_2},{2,PATTERN2_3},{2,PATTERN2_4},{3,PATTERN3_1},{3,PATTERN3_2},{3,PATTERN3_3},{3,PATTERN3_5},{3,PATTERN3_6}};
vO ols={{2,PATTERN2_1},{2,PATTERN2_2},{2,PATTERN2_3},{2,PATTERN2_4},{3,PATTERN3_1},{3,PATTERN3_2},{3,PATTERN3_3},{3,PATTERN3_4},{3,PATTERN3_5},{3,PATTERN3_6},{3,PATTERN3_7}};//,{3,PATTERN3_8}};
//vO ols={{2,PATTERN2_1},{2,PATTERN2_4},{3,PATTERN3_9}};
//vO ols={{2,PATTERN2_1},{2,PATTERN2_4}};


void small_enumerate(ll n,ll M,vG &g){
    cerr<<"small_enumerate"<<endl;
    ll P_LIMIT=0;
    
    vG kouho2;
    vG kouho3;
    {//kouhoを列挙
        for(auto ol:ols){
            if(ol.N==2){
                if(ol.V==PATTERN2_4)continue;
                if(ol.V==PATTERN2_3){
                    rep(p0,(n+1)/2,n){
                        ll p1 = n-p0;
                        GRAPH graph={ol,{p0,p1},{}};
                        kouho2.pb(graph);
                    }
                }else{
                    ll plim=0;
                    rep(p0,plim,n){
                        ll p1 = n-p0;
                        GRAPH graph={ol,{p0,p1},{}};
                        kouho2.pb(graph);
                    }
                }
            }else if(ol.N==3){
                
                if(ol.V==PATTERN3_1 || ol.V==PATTERN3_2){
                    
                    vl vecp = equidist_vec(100,P_LIMIT,n-1);
                    for(ll p0:vecp){
                        for(ll p1:vecp){
                            ll p2=n-p0-p1;
                            if(p2<=P_LIMIT)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                }else if(ol.V==PATTERN3_3){
                    vl vecp = equidist_vec(100,P_LIMIT,n-1);
                    for(ll p0:vecp){
                        for(ll p1:vecp){
                            if(p0<p1)continue;
                            ll p2=n-p0-p1;
                            if(p2>p1 || p2<P_LIMIT)continue;
                            assert(p0>=p1 && p1>=p2 && p2>=P_LIMIT);
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                }else if(ol.V==PATTERN3_4){
                    ll p_limit = P_LIMIT;
                    vl vecp=equidist_vec(100,p_limit,n-1);
                    for(ll p0:vecp){
                        for(ll p1:vecp){
                            if(p0<p1)continue;
                            ll p2=n-p0-p1;
                            if(n>=20 && p2<=P_LIMIT)continue;
                            if(p2<=1)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                     
                }else if(ol.V==PATTERN3_5){
                    vl vecp0= equidist_vec(100,P_LIMIT ,n-1);
                    vl vecp1= equidist_vec(100,P_LIMIT,n-1);
                    for(ll p0:vecp0){
                        for(ll p1:vecp1){
                            ll p2=n-p0-p1;
                           // if(p2<=P_LIMIT|| p2>=n/2-5)continue;
                            if(p2<=P_LIMIT)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                }else if(ol.V==PATTERN3_6){
                    continue;
                }else if(ol.V==PATTERN3_7 || ol.V==PATTERN3_8){
                    vl vecp = equidist_vec(100,P_LIMIT,n-1);
                    for(ll p0:vecp){
                        for(ll p1:vecp){
                            ll p2=n-p0-p1;
                            if(p2<=P_LIMIT || p1<p2)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                }else if(ol.V==PATTERN3_9){
                    assert(0);
                    /*
                    ll p_limit = P_LIMIT;
                    if(n>20)p_limit = (ll)((dd)P_LIMIT * 2);
                    ll CC=max(1,n/15);
                    if(n<=20)CC=1;
                    for(ll p0=p_limit;p0<=n-1;p0+=CC){
                        for(ll p1=p_limit;p1<=p0;p1+=CC){
                            ll p2=n-p0-p1;
                            if(n>=20 && p2<=15)continue;//空白部分大きい方がいいと思う
                            if(p2<=2)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            MAKE_GRAPH(graph);
                            kouho.pb(graph);
                        }
                    }*/
                }else if(ol.V==PATTERN3_10){
                    assert(0);
                    /*ll CC=max(1,n/5);
                    ll p_limit=P_LIMIT;
                    if(n<=20)CC=1;
                    for(ll p0=p_limit;p0<=n-1;p0+=CC){
                        for(ll p1=p_limit;p1<=n-1;p1+=CC){
                            ll p2=n-p0-p1;
                            if(p2<=p_limit || p0<p1)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            MAKE_GRAPH(graph);
                            kouho.pb(graph);
                        }
                    }*/
                }else{
                    assert(0);
                }
            }else if(ol.N==4){
                if(ol.V==PATTERN4_2)continue;
                /*
                assert(ol.N==4);
                if(ol.V==PATTERN4_1){
                    for(ll p0=11;p0<=n-1;p0+=5){
                        for(ll p1=11;p1<=p0;p1+=5){
                            for(ll p2=11;p2<=p1;p2+=5){
                                ll p3=n-p0-p1-p2;
                                if(p3<=11 || p2<p3)continue;
                                assert(p0>=p1 && p1>=p2 && p2>=p3 && p3>=12);
                                GRAPH graph = {ol,{p0,p1,p2,p3},{}};
                                MAKE_GRAPH(graph);
                                kouho.pb(graph);
                            }
                        }
                    }
                    
                }else{
                    assert(0);
                }
                 */
                assert(0);
            }else{
                assert(0);
            }
        }
    }
    
    vG rev_kouho;
    for(auto ol:ols){//PATTERN2_4を入れる
        if(ol==(OUTLINE){2,PATTERN2_4}){
            rep(p0,(n+1)/2,n){
                ll p1=n-p0;
                GRAPH graph = {ol,{p0,p1},{}};
                rev_kouho.pb(graph);
            }
        }else if(ol==(OUTLINE){3,PATTERN3_6}){
            vl vecp=equidist_vec(100,0,n-1);
            for(ll p0:vecp){
                for(ll p1:vecp){
                    if(p0<p1)continue;
                    ll p2=n-p0-p1;
                    if(p2<P_LIMIT || p2>p1)continue;
                    assert(p0>=p1 && p1>=p2 && p2>=P_LIMIT);
                    GRAPH graph = {ol,{p0,p1,p2},{}};
                    rev_kouho.pb(graph);
                }
            }
            
        }else if(ol.V==PATTERN4_2){
            assert(0);
            /*
            vl vecp=equidist_vec(10,n*0.15,n-1);
            for(ll p0:vecp){
                for(ll p1:vecp){
                    if(p0<p1)continue;
                    for(ll p2:vecp){
                        if(p1<p2)continue;
                        
                        ll p3=n-p0-p1-p2;
                        if(p2<p3)continue;
                        if(p3<P_LIMIT)continue;
                        assert(p0>=p1 && p1>=p2 && p2>=p3 && p3>=P_LIMIT);
                        GRAPH graph = {ol,{p0,p1,p2,p3},{}};
                        rev_kouho.pb(graph);
                    }
                }
            }*/
        }
    }
    
    auto add_graph_to_main=[](vG &g, vG &kouho,ll ko){
        //kouhoからko個選んでgに入れる
        ll ksz=kouho.size();
        //DEB(ksz);
        vvl dist(ksz,vl(ksz));
        rep(i,0,ksz-1){
            rep(j,i+1,ksz-1){
                dist[i][j] = GRAPH_DIFF_FAST3(kouho[i],kouho[j]);
                dist[j][i] = dist[i][j];
            }
        }
        ll first = kouho.size()-1;
        vl selected(ksz);
        selected[first] = 1;
        
        vl mn(ksz,inf);
        rep(j,0,ksz-1){
            chmin(mn[j],dist[first][j]);
        }
        
        rep(loop,1,ko-1){
            ll pos=0;
            rep(i,0,ksz-1){
                if(mn[i] > mn[pos]){
                    pos=i;
                }
            }
            cerr<<mn[pos]<<" ";if(loop==ko-1)cerr<<endl;
            assert(selected[pos]==0);
            selected[pos]=1;
            rep(j,0,ksz-1){
                chmin(mn[j],dist[pos][j]);
            }
        }
        rep(i,0,ksz-1){
            if(selected[i]){
                g.pb(kouho[i]);
            }
        }
       // DEB(selected);
    };
    auto add_add_graph_to_main=[](vG &g, vG &kouho,ll ko){
        //すでにgにはいくつか入ってる
        assert(!g.empty());
        //kouhoからko個選んでgに入れる
        ll ksz=kouho.size();
        //DEB(ksz);
        vvl dist(ksz,vl(ksz));
        rep(i,0,ksz-1){
            rep(j,i+1,ksz-1){
                dist[i][j] = GRAPH_DIFF_FAST3(kouho[i],kouho[j]);
                dist[j][i] = dist[i][j];
            }
        }
        
        vl selected(ksz);
        
        
        vl mn(ksz,inf);
        rep(i,0,ksz-1){
            for(auto x:g){
                chmin(mn[i],GRAPH_DIFF_FAST3(kouho[i],x));
            }
        }
        
        rep(loop,1,ko){
            ll pos=0;
            rep(i,0,ksz-1){
                if(mn[i] > mn[pos]){
                    pos=i;
                }
            }
            cerr<<mn[pos]<<" ";if(loop==ko)cerr<<endl;
            assert(selected[pos]==0);
            selected[pos]=1;
            rep(j,0,ksz-1){
                chmin(mn[j],dist[pos][j]);
            }
        }
        rep(i,0,ksz-1){
            if(selected[i]){
                g.pb(kouho[i]);
            }
        }
       // DEB(selected);
    };
    
    /*
    if(1){
        //真っ白とか真っ黒なグラフは削除してみる
     //   cerr<<kouho2.size()<<" "<<kouho3.size()<<" "<<rev_kouho.size()<<endl;
        auto delete_sparse=[](vG &kouho){
            rrep(i,(int)kouho.size()-1,0){
                vl cnt(2);
                rep(j,0,kouho[i].OL.N-1){
                    rep(k,0,kouho[i].OL.N-1){
                        if(j==k){
                            ll hen = kouho[i].SZ[j];
                            cnt[kouho[i].OL.V[j][k]] += hen*(hen-1);
                        }else{
                            ll henj=kouho[i].SZ[j];
                            ll henk=kouho[i].SZ[k];
                            cnt[kouho[i].OL.V[j][k]] += henj * henk;
                        }
                    }
                }
                if(cnt[0]>=cnt[1]*20 || cnt[0]*20<=cnt[1]){
                    kouho.erase(kouho.begin()+i);
                }
            }
        };
        delete_sparse(kouho2);
        delete_sparse(kouho3);
        delete_sparse(rev_kouho);
    }*/
    
    {
        auto erase=[](vG &kouho,vvl olv,vl sz){
            rrep(i,(int)kouho.size()-1,0){
                if(kouho[i].OL.V==olv && kouho[i].SZ==sz){
                    kouho.erase(kouho.begin()+i);
                    cerr<<"erase!"<<endl;
                }
            }
        };
        //貪欲に入れるやつ
        ll ks2=kouho2.size();
        ll ks3=kouho3.size();
        ll rks=rev_kouho.size();
        
        cerr<<"ks2,ks3,rks "<<ks2<<" "<<ks3<<" "<<rks<<endl;
        assert(ks2+ks3+rks>=M);
        
        //ll REV = min(round(M*0.4),rks);
        ll REV = min(round(M*0.25),rks);
        ll KOU2 = min(round(M*0.3),ks2);
        ll KOU3 = M-REV-KOU2;
        
        if((int)rev_kouho.size()<REV || (int)kouho2.size()<KOU2 || (int)kouho3.size()<KOU3){
            cerr<<"ASSERT!!!"<<endl;
            rev_kouho.insert(rev_kouho.end(),kouho2.begin(),kouho2.end());
            rev_kouho.insert(rev_kouho.end(),kouho3.begin(),kouho3.end());
            add_graph_to_main(g,rev_kouho,M);
        }else{
            add_graph_to_main(g,rev_kouho,REV);
            add_add_graph_to_main(g,kouho2,KOU2);
            add_add_graph_to_main(g,kouho3,KOU3);
        }
        
        for(auto &x:g){
            MAKE_GRAPH(x);
        }
        return;
    }
}


void enumerate(ll n,ll M,vG &g,dd eps){
    if(n<=20 && eps<=0.04+EPS){
        small_enumerate(n,M,g);
        return;
    }
    
    ll P_LIMIT;//ol.N==3の時に使うやつ
    if(n<=20)P_LIMIT=2;
    else if(n<=40)P_LIMIT=3;
    else if(n<=50)P_LIMIT=4;
    else if(n<=60)P_LIMIT=5;
    else if(n<=80)P_LIMIT=7;
    else P_LIMIT=10;
    
    
    
    vG kouho2;
    vG kouho3;
    {//kouhoを列挙
        for(auto ol:ols){
            if(ol.N==2){
                if(ol.V==PATTERN2_4)continue;
                if(ol.V==PATTERN2_3){
                    rep(p0,(n+1)/2,n){
                        ll p1 = n-p0;
                        GRAPH graph={ol,{p0,p1},{}};
                        kouho2.pb(graph);
                    }
                }else{
                    ll plim;
                    if(n<=60){
                        plim=0;
                    }else{
                        if(ol.V==PATTERN2_1)plim=n*0.15;
                        else if(ol.V==PATTERN2_2)plim=n*0.1;
                        else assert(0);
                    }
                    
                    rep(p0,plim,n){
                        ll p1 = n-p0;
                        GRAPH graph={ol,{p0,p1},{}};
                        kouho2.pb(graph);
                    }
                }
            }else if(ol.N==3){
                
                if(ol.V==PATTERN3_1 || ol.V==PATTERN3_2){
                    
                    vl vecp = equidist_vec(20,P_LIMIT,n-1);
                    for(ll p0:vecp){
                        for(ll p1:vecp){
                            ll p2=n-p0-p1;
                            if(p2<=P_LIMIT)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                }else if(ol.V==PATTERN3_3){
                    
                    vl vecp = equidist_vec(80,P_LIMIT,n-1);
                    //このパターンよさそうなので細かくしてみる
                    for(ll p0:vecp){
                        for(ll p1:vecp){
                            if(p0<p1)continue;
                            ll p2=n-p0-p1;
                            if(p2>p1 || p2<P_LIMIT)continue;
                            assert(p0>=p1 && p1>=p2 && p2>=P_LIMIT);
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                }else if(ol.V==PATTERN3_4){
                    
                    ll p_limit = P_LIMIT;
                    if(n>20)p_limit = (ll)(P_LIMIT * 2);
                    
                    vl vecp=equidist_vec(16,p_limit,n-1);
                    for(ll p0:vecp){
                        for(ll p1:vecp){
                            if(p0<p1)continue;
                            ll p2=n-p0-p1;
                            if(n>=20 && p2<=P_LIMIT)continue;
                            if(p2<=1)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                     
                }else if(ol.V==PATTERN3_5){
                    vl vecp0= equidist_vec(16,n*0.15 ,n-1);
                    vl vecp1= equidist_vec(16,P_LIMIT,n-1);
                    for(ll p0:vecp0){
                        for(ll p1:vecp1){
                            ll p2=n-p0-p1;
                            if(p2<=P_LIMIT|| p2>=n/2-5)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                }else if(ol.V==PATTERN3_6){
                    continue;
                }else if(ol.V==PATTERN3_7 || ol.V==PATTERN3_8){
                    vl vecp = equidist_vec(12,P_LIMIT,n-1);
                    for(ll p0:vecp){
                        for(ll p1:vecp){
                            ll p2=n-p0-p1;
                            if(p2<=P_LIMIT || p1<p2)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            kouho3.pb(graph);
                        }
                    }
                }else if(ol.V==PATTERN3_9){
                    assert(0);
                    /*
                    ll p_limit = P_LIMIT;
                    if(n>20)p_limit = (ll)((dd)P_LIMIT * 2);
                    ll CC=max(1,n/15);
                    if(n<=20)CC=1;
                    for(ll p0=p_limit;p0<=n-1;p0+=CC){
                        for(ll p1=p_limit;p1<=p0;p1+=CC){
                            ll p2=n-p0-p1;
                            if(n>=20 && p2<=15)continue;//空白部分大きい方がいいと思う
                            if(p2<=2)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            MAKE_GRAPH(graph);
                            kouho.pb(graph);
                        }
                    }*/
                }else if(ol.V==PATTERN3_10){
                    assert(0);
                    /*ll CC=max(1,n/5);
                    ll p_limit=P_LIMIT;
                    if(n<=20)CC=1;
                    for(ll p0=p_limit;p0<=n-1;p0+=CC){
                        for(ll p1=p_limit;p1<=n-1;p1+=CC){
                            ll p2=n-p0-p1;
                            if(p2<=p_limit || p0<p1)continue;
                            GRAPH graph = {ol,{p0,p1,p2},{}};
                            MAKE_GRAPH(graph);
                            kouho.pb(graph);
                        }
                    }*/
                }else{
                    assert(0);
                }
            }else if(ol.N==4){
                if(ol.V==PATTERN4_2)continue;
                /*
                assert(ol.N==4);
                if(ol.V==PATTERN4_1){
                    for(ll p0=11;p0<=n-1;p0+=5){
                        for(ll p1=11;p1<=p0;p1+=5){
                            for(ll p2=11;p2<=p1;p2+=5){
                                ll p3=n-p0-p1-p2;
                                if(p3<=11 || p2<p3)continue;
                                assert(p0>=p1 && p1>=p2 && p2>=p3 && p3>=12);
                                GRAPH graph = {ol,{p0,p1,p2,p3},{}};
                                MAKE_GRAPH(graph);
                                kouho.pb(graph);
                            }
                        }
                    }
                    
                }else{
                    assert(0);
                }
                 */
                assert(0);
            }else{
                assert(0);
            }
        }
    }
    
    vG rev_kouho;
    for(auto ol:ols){//PATTERN2_4を入れる
        if(ol==(OUTLINE){2,PATTERN2_4}){
            rep(p0,(n+1)/2,n*0.9){
                ll p1=n-p0;
                GRAPH graph = {ol,{p0,p1},{}};
                rev_kouho.pb(graph);
            }
        }else if(ol==(OUTLINE){3,PATTERN3_6}){
            vl vecp=equidist_vec(20,n*0.15,n-1);
            for(ll p0:vecp){
                for(ll p1:vecp){
                    if(p0<p1)continue;
                    ll p2=n-p0-p1;
                    if(p2<P_LIMIT || p2>p1)continue;
                    assert(p0>=p1 && p1>=p2 && p2>=P_LIMIT);
                    GRAPH graph = {ol,{p0,p1,p2},{}};
                    rev_kouho.pb(graph);
                }
            }
            
        }else if(ol.V==PATTERN4_2){
            assert(0);
            /*
            vl vecp=equidist_vec(10,n*0.15,n-1);
            for(ll p0:vecp){
                for(ll p1:vecp){
                    if(p0<p1)continue;
                    for(ll p2:vecp){
                        if(p1<p2)continue;
                        
                        ll p3=n-p0-p1-p2;
                        if(p2<p3)continue;
                        if(p3<P_LIMIT)continue;
                        assert(p0>=p1 && p1>=p2 && p2>=p3 && p3>=P_LIMIT);
                        GRAPH graph = {ol,{p0,p1,p2,p3},{}};
                        rev_kouho.pb(graph);
                    }
                }
            }*/
        }
    }
    
    auto add_graph_to_main=[](vG &g, vG &kouho,ll ko){
        //kouhoからko個選んでgに入れる
        ll ksz=kouho.size();
        //DEB(ksz);
        vvl dist(ksz,vl(ksz));
        rep(i,0,ksz-1){
            rep(j,i+1,ksz-1){
                dist[i][j] = GRAPH_DIFF_FAST3(kouho[i],kouho[j]);
                dist[j][i] = dist[i][j];
            }
        }
        ll first = kouho.size()-1;
        vl selected(ksz);
        selected[first] = 1;
        
        vl mn(ksz,inf);
        rep(j,0,ksz-1){
            chmin(mn[j],dist[first][j]);
        }
        
        rep(loop,1,ko-1){
            ll pos=0;
            rep(i,0,ksz-1){
                if(mn[i] > mn[pos]){
                    pos=i;
                }
            }
            cerr<<mn[pos]<<" ";if(loop==ko-1)cerr<<endl;
            assert(selected[pos]==0);
            selected[pos]=1;
            rep(j,0,ksz-1){
                chmin(mn[j],dist[pos][j]);
            }
        }
        rep(i,0,ksz-1){
            if(selected[i]){
                g.pb(kouho[i]);
            }
        }
       // DEB(selected);
    };
    auto add_add_graph_to_main=[](vG &g, vG &kouho,ll ko){
        //すでにgにはいくつか入ってる
        assert(!g.empty());
        //kouhoからko個選んでgに入れる
        ll ksz=kouho.size();
        //DEB(ksz);
        vvl dist(ksz,vl(ksz));
        rep(i,0,ksz-1){
            rep(j,i+1,ksz-1){
                dist[i][j] = GRAPH_DIFF_FAST3(kouho[i],kouho[j]);
                dist[j][i] = dist[i][j];
            }
        }
        
        vl selected(ksz);
        
        
        vl mn(ksz,inf);
        rep(i,0,ksz-1){
            for(auto x:g){
                chmin(mn[i],GRAPH_DIFF_FAST3(kouho[i],x));
            }
        }
        
        rep(loop,1,ko){
            ll pos=0;
            rep(i,0,ksz-1){
                if(mn[i] > mn[pos]){
                    pos=i;
                }
            }
            cerr<<mn[pos]<<" ";if(loop==ko)cerr<<endl;
            assert(selected[pos]==0);
            selected[pos]=1;
            rep(j,0,ksz-1){
                chmin(mn[j],dist[pos][j]);
            }
        }
        rep(i,0,ksz-1){
            if(selected[i]){
                g.pb(kouho[i]);
            }
        }
       // DEB(selected);
    };
    
    
    if(1){
        //真っ白とか真っ黒なグラフは削除してみる
     //   cerr<<kouho2.size()<<" "<<kouho3.size()<<" "<<rev_kouho.size()<<endl;
        auto delete_sparse=[](vG &kouho){
            rrep(i,(int)kouho.size()-1,0){
                vl cnt(2);
                rep(j,0,kouho[i].OL.N-1){
                    rep(k,0,kouho[i].OL.N-1){
                        if(j==k){
                            ll hen = kouho[i].SZ[j];
                            cnt[kouho[i].OL.V[j][k]] += hen*(hen-1);
                        }else{
                            ll henj=kouho[i].SZ[j];
                            ll henk=kouho[i].SZ[k];
                            cnt[kouho[i].OL.V[j][k]] += henj * henk;
                        }
                    }
                }
                if(cnt[0]>=cnt[1]*20 || cnt[0]*20<=cnt[1]){
                    kouho.erase(kouho.begin()+i);
                }
            }
        };
        delete_sparse(kouho2);
        delete_sparse(kouho3);
        delete_sparse(rev_kouho);
    }
    
    {
        auto erase=[](vG &kouho,vvl olv,vl sz){
            rrep(i,(int)kouho.size()-1,0){
                if(kouho[i].OL.V==olv && kouho[i].SZ==sz){
                    kouho.erase(kouho.begin()+i);
                    cerr<<"erase!"<<endl;
                }
            }
        };
        if(n==60){
            erase(kouho3,PATTERN3_7,(vl){30,15,15});
            erase(kouho2,PATTERN2_1,(vl){14,46});
            erase(kouho3,PATTERN3_4,(vl){16,16,28});
        }
        
        //貪欲に入れるやつ
        ll ks2=kouho2.size();
        ll ks3=kouho3.size();
        ll rks=rev_kouho.size();
        
      //  cerr<<"ks2,ks3,rks "<<ks2<<" "<<ks3<<" "<<rks<<endl;
        assert(ks2+ks3+rks>=M);
        
        //ll REV = min(round(M*0.4),rks);
        ll REV = min(round(M*0.25),rks);
        ll KOU2 = min(round(M*0.3),ks2);
        ll KOU3 = M-REV-KOU2;
        
        if((int)rev_kouho.size()<REV || (int)kouho2.size()<KOU2 || (int)kouho3.size()<KOU3){
            cerr<<"ASSERT!!!"<<endl;
            rev_kouho.insert(rev_kouho.end(),kouho2.begin(),kouho2.end());
            rev_kouho.insert(rev_kouho.end(),kouho3.begin(),kouho3.end());
            add_graph_to_main(g,rev_kouho,M);
        }else{
            add_graph_to_main(g,rev_kouho,REV);
            add_add_graph_to_main(g,kouho2,KOU2);
            add_add_graph_to_main(g,kouho3,KOU3);
        }
        
        for(auto &x:g){
            MAKE_GRAPH(x);
        }
        return;
    }
}

signed main(int argc, char *argv[]){fastio
    clock_gettime(CLOCK_REALTIME, &START_TIME);
    ///////////////////////////////////////
    
    
    
    const ll num_of_testcases = 100;
    
    /*
    rep(n,6,20){
        dd time=stop_watch();
        vG g;
        cerr<<n<<":";
        enumerate(n,1,g,0.01);
        cerr<<stop_watch()-time<<"ms"<<endl;
    }return 0;
    //*/
    
    ll M;dd eps;
    cin>>M>>eps;
    cerr<<"M,eps "<<M<<" "<<eps<<endl;
    
    
    //OTAMESHI(M,10);
    
    if(eps<=EPS){
        zeroeps(M);
        return 0;
    }
    
    auto CALC_N=[](vl_l v,ll m){
        
        if(v[(int)v.size()-1].fi!=100)v.pb({100,v[(int)v.size()-1].se});
        if(v[0].fi!=10)v.pb({10,v[0].se});
        SORT(v);
        
        
        rep(i,0,(int)v.size()-2){
            if(v[i].fi<=m && m<=v[i+1].fi){
                auto calc=[](l_l a,l_l b,ll c){
                    dd kei = (dd)(c-a.fi)/(dd)(b.fi-a.fi);
                    return (ll)round(a.se + kei * (b.se-a.se));
                };
                return calc(v[i],v[i+1],m);
            }
        }
      //  assert(0);
        return (ll)100;
    };
    if(eps<=0.01 + EPS){
        if(M<=15)n=8;
        else if(M<=25)n=9;
        else if(M<=40)n=9;
        else if(M<=55)n=11;
        else if(M<=70)n=13;
        else if(M<=85)n=14;
        else n=15;
    }else if(eps<=0.02 + EPS){
        if(M<=15)n=8;
        else if(M<=25)n=9;
        else if(M<=40)n=9;
        else if(M<=55)n=11;
        else if(M<=70)n=13;
        else if(M<=85)n=14;
        else n=16;
    }else if(eps<=0.03 + EPS){
        if(M<=15)n=8;
        else if(M<=25)n=10;
        else if(M<=40)n=11;
        else if(M<=55)n=13;
        else if(M<=70)n=13;
        else if(M<=85)n=16;
        else n=16;
    }else if(eps<=0.04 + EPS){
        if(M<=15)n=8;
        else if(M<=25)n=10;
        else if(M<=40)n=11;
        else if(M<=55)n=13;
        else if(M<=70)n=13;
        else if(M<=85)n=16;
        else n=16;
    }else if(eps<=0.05 + EPS){
        if(M<=15)n=9;
        else if(M<=25)n=10;
        else if(M<=40)n=12;
        else if(M<=55)n=14;
        else if(M<=70)n=16;
        else n=18;
    }else if(eps<=0.06 + EPS){
        if(M<=15)n=11;
        else if(M<=25)n=13;
        else if(M<=40)n=13;
        else if(M<=55)n=15;
        else if(M<=70)n=16;
        else n=18;
    }else if(eps<=0.07 + EPS){
        if(M<=15)n=11;
        else if(M<=25)n=13;
        else if(M<=40)n=14;
        else if(M<=55)n=16;
        else if(M<=70)n=16;
        else if(M<=85)n=19;
        else n=19;
    }else if(eps<=0.08 + EPS){
        if(M<=15)n=11;
        else if(M<=25)n=14;
        else if(M<=40)n=14;
        else if(M<=70)n=16;
        else if(M<=85)n=20;
        else n=20;
    }else if(eps<=0.09 + EPS){
        if(M<=15)n=11;
        else if(M<=25)n=15;
        else if(M<=40)n=16;
        else if(M<=70)n=16;
        else n=20;
    }else if(eps<=0.1 + EPS){
        if(M<=15)n=14;
        else if(M<=25)n=16;
        else if(M<=40)n=16;
        else if(M<=70)n=20;
        else n=20;
    }else if(eps<=0.11 + EPS){
        if(M<=15)n=14;
        else if(M<=25)n=16;
        else if(M<=40)n=18;
        else if(M<=70)n=20;
        else n=23;
    }else if(eps<=0.12 + EPS){
        vl_l vec = {{10,14},{15,14},{25,16},{40,18},{70,20},{100,24}};
        n=CALC_N(vec,M);
    }else if(eps<=0.13 + EPS){
        vl_l vec = {{10,14},{15,15},{25,17},{40,19},{70,22},{100,25}};
        n=CALC_N(vec,M);
    }else if(eps<=0.14 + EPS){
        vl_l vec = {{10,15},{15,16},{25,18},{40,20},{70,24},{100,26}};
        n=CALC_N(vec,M);
    }else if(eps<=0.15 + EPS){
        vl_l vec = {{10,16},{15,16},{25,18},{40,20},{70,24},{100,26}};
        n=CALC_N(vec,M);
    }else if(eps<=0.16 + EPS){
        vl_l vec = {{10,17},{15,17},{25,18},{40,20},{70,24},{100,26}};
        n=CALC_N(vec,M);
    }else if(eps<=0.17 + EPS){
        vl_l vec = {{10,18},{15,19},{25,20},{40,23},{70,27},{100,28}};
        n=CALC_N(vec,M);
    }else if(eps<=0.18 + EPS){
        vl_l vec = {{10,20},{15,21},{25,23},{40,26},{70,30},{100,30}};
        n=CALC_N(vec,M);
    }else if(eps<=0.19 + EPS){
        vl_l vec = {{10,20},{15,22},{25,25},{40,27},{70,31},{100,31}};
        n=CALC_N(vec,M);
    }else if(eps<=0.2 + EPS){
        vl_l vec = {{10,20},{15,23},{25,27},{40,28},{70,33},{100,33}};
        n=CALC_N(vec,M);
    }else if(eps<=0.21 + EPS){
        vl_l vec = {{10,21},{15,24},{25,28},{40,29},{70,34},{100,36}};
        n=CALC_N(vec,M);
    }else if(eps<=0.22 + EPS){
        vl_l vec = {{10,22},{15,25},{25,30},{40,30},{70,35},{100,40}};
        n=CALC_N(vec,M);
    }else if(eps<=0.23 + EPS){
        vl_l vec = {{10,24},{15,26},{25,31},{40,33},{70,39},{100,44}};
        n=CALC_N(vec,M);
    }else if(eps<=0.24 + EPS){
        vl_l vec = {{10,26},{15,28},{25,33},{40,36},{70,44},{100,48}};
        n=CALC_N(vec,M);
    }else if(eps<=0.25 + EPS){
        vl_l vec = {{10,28},{15,29},{25,34},{40,39},{70,45},{100,49}};
        n=CALC_N(vec,M);
    }else if(eps<=0.26 + EPS){
        vl_l vec = {{10,30},{15,30},{25,36},{40,42},{70,46},{100,50}};
        n=CALC_N(vec,M);
    }else if(eps<=0.27 + EPS){
        vl_l vec = {{10,31},{15,33},{25,40},{40,43},{70,49},{100,57}};
        n=CALC_N(vec,M);
    }else if(eps<=0.28 + EPS){
        vl_l vec = {{10,33},{15,36},{25,44},{40,44},{70,52},{100,64}};
        n=CALC_N(vec,M);
    }else if(eps<=0.29 + EPS){
        vl_l vec = {{10,36},{15,40},{25,44},{40,52},{70,58},{100,67}};
        n=CALC_N(vec,M);
    }else if(eps<=0.3 + EPS){
        vl_l vec = {{10,40},{15,45},{25,45},{40,60},{70,65},{100,70}};
        n=CALC_N(vec,M);
    }else if(eps<=0.31 + EPS){
        vl_l vec = {{10,45},{15,47},{25,50},{40,62},{70,70},{100,75}};
        n=CALC_N(vec,M);
    }else if(eps<=0.32 + EPS){
        vl_l vec = {{10,50},{15,50},{25,55},{40,65},{70,75},{100,80}};
        n=CALC_N(vec,M);
    }else if(eps<=0.33 + EPS){
        vl_l vec = {{10,55},{15,60},{25,62},{40,72},{70,82},{100,90}};
        n=CALC_N(vec,M);
    }else if(eps<=0.34 + EPS){
        vl_l vec = {{10,65},{15,70},{25,70},{40,80},{70,90},{100,100}};
        n=CALC_N(vec,M);
    }else if(eps<=0.35 + EPS){
        vl_l vec = {{10,67},{15,77},{25,77},{40,90},{70,95},{100,100}};
        n=CALC_N(vec,M);
    }else if(eps<=0.36 + EPS){
        vl_l vec = {{10,70},{15,85},{25,85},{40,100},{100,100}};
        n=CALC_N(vec,M);
    }else if(eps<=0.37 + EPS){
        vl_l vec = {{10,80},{15,90},{20,95},{25,100},{100,100}};
        n=CALC_N(vec,M);
    }else{
        n=100;
    }
    
    
    
    
    if(argc==2)n=atoi(argv[1]);
    //cerr<<"argc"<<argc<<endl;
    
    
    //OTAMESHI2(M);
    
    
    vG g;
    enumerate(n,M,g,eps);
    
    
    /*
    {
        for(auto x:g){
            for(auto y:g){
                cerr<<setw(2)<<GRAPH_DIFF_FAST3(x,y)<<" ";
            }cerr<<endl;
        }cerr<<endl;
    }
    //*/
    
    
    PRINT_INIT(n,g);
    
    /*
    if(1){
        map<vvl,ll>mp;
        for(auto x:g){
            mp[x.OL.V]++;
        }
        for(auto x:mp){
            DEB(x.fi);
            DEB(x.se);
        }
    }
    //*/
    cerr<<endl<<"initまでで "<<stop_watch()<<"ms"<<endl;
    
    res_time=4500-stop_watch();
    rep(testcase,0,num_of_testcases-1){
        
        vl_l hoz1;
        //vl_l hoz2;
        
        if(0){
            string s;cin>>s;
            cout<<0<<endl;
            fflush(stdout);
            continue;
        }
        vvl a;
        {
            string s;cin>>s;
            a = str_to_vvl(s);
        }
        auto match_graph_and_ol=[](vvl a,OUTLINE ol){
            dd zero_time = stop_watch();
            ll n=a.size();
            vl now_f(n);
            rep(i,0,n-1)now_f[i]=rand(0,ol.N-1);
            ll now_miss=calc_miss(a,ol,now_f);
            
            vl min_f=now_f;
            ll min_miss=now_miss;
            
            ll loopcount=0;
            
            dd start_temp = 20;
            dd end_temp = 0;
            dd end_time = res_time/(num_of_testcases * (int)ols.size());
            //dd end_time=0.1;
            ll last_update=0;
            
            while(1){
                loopcount++;
                dd time = stop_watch()-zero_time;
                if(time>end_time)break;
                dd temp = (end_temp-start_temp)/end_time*time + start_temp;
                
                ll pos=rand(0,n-1);
                ll val;
                if(ol.N==2)val=1-now_f[pos];
                else{
                    val = rand(0,ol.N-1);
                    while(val == now_f[pos]){
                        val = rand(0,ol.N-1);
                    }
                }
                ll delta_miss=change(a,now_f,pos,val,ol,n);
                
                if(delta_miss<=0 || exp(-delta_miss/temp)>randdouble(0,1)){
                    now_miss+=delta_miss;
                    now_f[pos] = val;
                    if(min_miss>now_miss){
                        last_update=loopcount;
                        min_miss=now_miss;
                        min_f=now_f;
                       // cerr<<loopcount<<":"<<now_miss<<endl;
                    }
                }
                //cerr<<now_miss<<" "<<calc_miss(a,ol,now_f)<<endl;
            }
           // cerr<<"loopcount"<<loopcount<<" "<<last_update<<endl;
          //  DEB(now_f);
            return min_f;
        };
        
        
        /*
        auto match_swap_only=[](vvl &a,vl &now_f,OUTLINE &ol){
            
            dd zero_time = stop_watch();
            ll n=a.size();
            ll now_miss=calc_miss(a,ol,now_f);
            
            vl min_f=now_f;
            ll min_miss=now_miss;
            
            ll loopcount=0;
            
            dd start_temp = 10;
            dd end_temp = 0;
            dd end_time = 5;
            ll last_update=0;
            
            while(1){
                loopcount++;
                dd time = stop_watch()-zero_time;
                if(time>end_time)break;
                dd temp = (end_temp-start_temp)/end_time*time + start_temp;
                
                ll pos1=rand(0,n-1);
                ll pos2=rand(0,n-1);
                if(now_f[pos1]==now_f[pos2])continue;
                
                ll delta_miss=swap(a,now_f,pos1,pos2,ol,n);
                
                if(delta_miss<=0 || exp(-delta_miss/temp)>randdouble(0,1)){
                    now_miss+=delta_miss;
                    swap(now_f[pos1],now_f[pos2]);
                    if(min_miss>now_miss){
                        last_update=loopcount;
                        min_miss=now_miss;
                        min_f=now_f;
                       // cerr<<loopcount<<":"<<now_miss<<endl;
                    }
                }
                
                //cerr<<now_miss<<" "<<calc_miss(a,ol,now_f)<<endl;
            }
           // cerr<<"loopcount"<<loopcount<<" "<<last_update<<endl;
          //  DEB(now_f);
            now_f=min_f;
        };
        //*/
        
        
        for(auto ol:ols){
            if(ol.N==2){
                vl_l t;
                rep(i,0,M-1){
                    if(ol == g[i].OL){
                        t.pb({g[i].SZ[1],i});
                    }
                }
                if(t.empty())continue;
                SORT(t);
              //  DEB(t);
                vl f=match_graph_and_ol(a,ol);
                ll fcnt=0;for(auto x:f)if(x)fcnt++;
                if(ol.V==PATTERN2_3){
                    if(chmin(fcnt,n-fcnt)){
                        for(auto &x:f)x=1-x;
                    }
                }
                if(ol.V==PATTERN2_4){
                    if(chmin(fcnt,n-fcnt)){
                        for(auto &x:f)x=1-x;
                    }
                }
                {
                    rep(i,0,(int)t.size()){
                        if(i==(int)t.size()){
                            t = {t.back()};
                            break;
                        }else if(t[i].fi==fcnt){
                            t = {t[i]};
                            break;
                        }else if(t[i].fi>fcnt){
                            if(i>=1)t={t[i-1],t[i]};
                            else t={t[i]};
                            break;
                        }
                    }
                    assert((int)t.size()==1 || (int)t.size()==2);
                }
             //   DEB(t);
             //   DEB(fcnt);
                for(auto T:t){
                    vl F=f;
                    ll FCNT=fcnt;
                    if(FCNT<T.fi){
                        vl_l ret;
                        rep(i,0,n-1){
                            if(F[i]==0){
                                ret.pb({change(a,f,i,1,ol,n),i});
                            }
                        }
                        SORT(ret);
                        rep(z,0,(T.fi-FCNT)-1){
                            F[ret[z].se]=1;
                        }
                    }else if(FCNT>T.fi){
                        vl_l ret;
                        rep(i,0,n-1){
                            if(F[i]==1){
                                ret.pb({change(a,f,i,0,ol,n),i});
                            }
                        }
                        SORT(ret);
                        rep(z,0,(FCNT-T.fi)-1){
                            F[ret[z].se]=0;
                        }
                    }
                    {
                        FCNT=0;
                        for(auto x:F)if(x)FCNT++;
                        assert(FCNT==T.fi);
                    }
                    hoz1.pb({calc_miss(a,ol,F),T.se});
                  //  match_swap_only(a,F,ol);
                    
                 //   hoz2.pb({calc_miss(a,ol,F),T.se});
                    
                  //  DEB(F);
                }
            }else if(ol.N==3){
                vl_l t;
                vl f=match_graph_and_ol(a,ol);
                
                
                if(ol.V==PATTERN3_3 || ol.V==PATTERN3_6){
                    //順序入れ替え
                    //PATTERN2_3と同じこと
                    vl cnt(ol.N);
                    for(auto x:f)cnt[x]++;
                    
                   // DEB(cnt);
                    vl jun(ol.N);rep(i,0,ol.N-1)jun[i]=i;
                    sort(all(jun),[&](ll x,ll y){return cnt[x]>cnt[y];});
                    //DEB(jun);
                    vl rjun(ol.N);rep(i,0,ol.N-1)rjun[jun[i]]=i;
                    for(auto &x:f){
                        x = rjun[x];
                    }
                }else if(ol.V==PATTERN3_4 || ol.V==PATTERN3_9 || ol.V==PATTERN3_10){
                    //順序入れ替え
                    vl cnt(ol.N);
                    for(auto x:f)cnt[x]++;
                    
                    if(cnt[0]<cnt[1]){
                        for(auto &x:f){
                            if(x<=1)x = 1-x;
                        }
                    }
                }else if(ol.V==PATTERN3_7 || ol.V==PATTERN3_8){
                    vl cnt(ol.N);
                    for(auto x:f)cnt[x]++;
                    if(cnt[1]<cnt[2]){
                        for(auto &x:f){
                            if(x==1)x=2;
                            else if(x==2)x=1;
                        }
                    }
                }
                
                vl fsm(ol.N);
                for(auto x:f)fsm[x]++;
                
                if(ol.V==PATTERN3_3 || ol.V==PATTERN3_6){
                    assert(fsm[0]>=fsm[1] && fsm[1]>=fsm[2]);
                }else if(ol.V==PATTERN3_4){
                    assert(fsm[0]>=fsm[1]);
                }
                
                rep(i,0,M-1){
                    if(ol == g[i].OL){
                        t.pb({GRAPH_DIFF_FAST3({g[i].OL,fsm,{}},{g[i].OL,g[i].SZ,{}}),i});
                    }
                }
               // DEB(t);
                if(t.empty())continue;
                SORT(t);
                while(t.size()>3)t.pop_back();
                for(auto T:t){
                    vl F=f;
                    vl FSM=fsm;
                    vl GSZ=g[T.se].SZ;
                    rep(i,0,n-1){
                        ll val=F[i];
                        if(FSM[val]>GSZ[val]){
                            rep(j,0,ol.N-1){
                                if(FSM[j]<GSZ[j]){
                                    F[i]=j;
                                    FSM[val]--;
                                    FSM[j]++;
                                    break;
                                }
                            }
                        }
                    }
                    assert(FSM==GSZ);
                    
                    hoz1.pb({calc_miss(a,ol,F),T.se});
                  //  match_swap_only(a,F,ol);
                  //  hoz2.pb({calc_miss(a,ol,F),T.se});
                }
            }else if(ol.N==4){
                vl_l t;
                vl f=match_graph_and_ol(a,ol);
                
                if(ol.V==PATTERN4_1 || ol.V==PATTERN4_2){
                    //順序入れ替え
                    //PATTERN2_3と同じこと
                    vl cnt(ol.N);
                    for(auto x:f)cnt[x]++;
                    
                   // DEB(cnt);
                    vl jun(ol.N);rep(i,0,ol.N-1)jun[i]=i;
                    sort(all(jun),[&](ll x,ll y){return cnt[x]>cnt[y];});
                    //DEB(jun);
                    vl rjun(ol.N);rep(i,0,ol.N-1)rjun[jun[i]]=i;
                    for(auto &x:f){
                        x = rjun[x];
                    }
                }
                
                vl fsm(ol.N);
                for(auto x:f)fsm[x]++;
                
                rep(i,0,M-1){
                    if(ol == g[i].OL){
                        t.pb({GRAPH_DIFF_FAST3({g[i].OL,fsm,{}},{g[i].OL,g[i].SZ,{}}),i});
                    }
                }
               // DEB(t);
                if(t.empty())continue;
                SORT(t);
                while(t.size()>3)t.pop_back();
                for(auto T:t){
                    vl F=f;
                    vl FSM=fsm;
                    vl GSZ=g[T.se].SZ;
                    rep(i,0,n-1){
                        ll val=F[i];
                        if(FSM[val]>GSZ[val]){
                            rep(j,0,ol.N-1){
                                if(FSM[j]<GSZ[j]){
                                    F[i]=j;
                                    FSM[val]--;
                                    FSM[j]++;
                                    break;
                                }
                            }
                        }
                    }
                    assert(FSM==GSZ);
                    
                    hoz1.pb({calc_miss(a,ol,F),T.se});
                  //  match_swap_only(a,F,ol);
                  //  hoz2.pb({calc_miss(a,ol,F),T.se});
                }
                
            }else{
                assert(0);
            }
        }
        
        SORT(hoz1);
       // SORT(hoz2);
        
      //  DEB(hoz1);
      //  DEB(hoz2);
        
        assert(!hoz1.empty());
        PRINT_ANS(hoz1[0].se);
    }
    
    
    dd TIME=stop_watch();
    cerr<<"終了までで "<<TIME<<"ms"<<endl;
    assert(TIME<=5000);
    return 0;
}

/*
 olsとして11つ
 [2] 1,2,3,4
 [3] 1,2,3,4,5,6,7
 */

//解を絞ってからの焼きなましは中止
//理由：#36404156が悪化したから&手元でも悪化するから。過学習?みたいな現象起きてる


//PATTERN3_5を条件厳しくして、全ケースに追加
//(M>=85,eps>=0.35)とかでは悪化してると思う
//これはパターン増やせば解決すると思う
//解決しなかったらhttps://atcoder.jp/contests/ahc016/submissions/36475676 にもどそう

//eps小さくてM大きいケース注意！(nが小さくなるので)(特にn=10,12とか！)
//kouho.sizeとrev_kouho.sizeが足りてるか確認する必要あり

//olsの種類を増やした

//GRAPH_DIFF_FAST3を使うようにした
//nが昔のやつに最適化されてるので、修正するとスコア良くなるはず
//n修正した


//delete_sparseを20で追加(変わらないかな...

//CALC_DIFF → GRAPH_DIFF_FAST3に変更

//REV,KOU2,KOU3に変更
//start_tempを50→20に変更



//add_graph_to_main_yakiを追加
//SETは使わないで愚直にやった方が早い
//add_add_graph_to_main_yakiを追加
//all_in(バグとっただけ)

//上の塊やめて貪欲に入れることにする
//これ最終日のメルクマールにしてみる


//eps小さいところで全探索
//enumerateのif文も変えた
//eps0.12以上にcall_n追加
