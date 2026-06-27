#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3,inline")  // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens
#pragma GCC target("bmi,bmi2,lzcnt,popcnt")                       // bit manipulation
#pragma GCC target("movbe")                                       // byte swap
#pragma GCC target("aes,pclmul,rdrnd")                            // encryption
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2")  // SIMD




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
typedef double dd;
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

#define fastio ios::sync_with_stdio(false); cin.tie(0); cout.tie(0); cout<<setprecision(10);cerr<<fixed<<setprecision(10);
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
//#define DEB(V) cerr<< TO_STRING_FOR_DEBUG(V) << ": "; DEBUG_PRINT(V);
#define DEB(V) cerr << "[" << __LINE__ << "] " << TO_STRING_FOR_DEBUG(V) << ": "; DEBUG_PRINT(V);
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
/*
ll rand(ll a,ll b){
    //a以上b以下の整数を返す。
    assert(a<=b);
    ll len=(b-a+1);
    return randxor()%len + a;
}
 */
ll rand(ll a,ll b){
    //a以上b以下の整数を返す。
    assert(a<=b);
    ll len=(b-a+1);
    return ((len * randxor()) >> 32) + a;
    //randxorが[0,2^32)なので、後から2^32で割ってる
}
dd randdouble(dd l,dd r){
    //l以上r以下の実数を返す。
    assert(l<=r + EPS);
    return l + (r-l) * (dd)randxor()/4294967295;
}
template<typename T>
T randVec(const vector<T>& a){
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
template<typename T>
int argmin(vector<T>& a){
    assert(!a.empty());
    int ret = 0;
    for (int i = 1; i < (int)a.size(); ++i) {
        if (a[i] < a[ret]) {
            ret = i;
        }
    }
    return ret;
}
template<typename T>
int argmax(vector<T>& a){
    assert(!a.empty());
    int ret = 0;
    for (int i = 1; i < (int)a.size(); ++i) {
        if (a[i] > a[ret]) {
            ret = i;
        }
    }
    return ret;
}


namespace ANNEALING{
//手元だと遅くなるけどAtcoderでは速くなる?
constexpr int LOG_TABLE_SIZE = 1<<20;
double log_table[LOG_TABLE_SIZE];
double log_table_stable[LOG_TABLE_SIZE];
void init_log_table() {
    for (int i = 0; i < LOG_TABLE_SIZE; i++) {
        double x = ((double)i + 0.5) / LOG_TABLE_SIZE;
        log_table[i] = log(x);
        log_table_stable[i] = log_table[i];
    }
    // shuffle
    for (int i = 1; i < LOG_TABLE_SIZE; i++) {
        int j = rand(0,i-1);//rng.range_random_safe(LOG_TABLE_SIZE);
        swap(log_table[i], log_table[j]);
    }
}
inline double log_random() {
    static int idx = 0;
    if (idx == LOG_TABLE_SIZE) idx = 0;
    return log_table[idx++];
}
inline bool Accept(double diff, double T) {
    if (diff >= 0) return true;
    return diff > T * log_random();
  //  return diff > T * std::log(g.next_double());
}
}//ANNEALING

///////////////////////////////////////////////////////////////////////////////////////////////
typedef pair<dd,ll> d_l;
typedef vector<d_l> vd_l;
typedef vector<vd_l> vvd_l;

ll N,M,K;
vl_l RDin_;//(N)
vl_l RS_;//(M)
vvd CH_;//(K,N)

const ll NIL = -1;
const l_l NIL2 = {NIL,NIL};

void INPUT_FOR_PROFILE(){
    /*
    N = 20;
    M = 50 * N;
    K = 4 * N;
     //*/
    //*
    N = 5;
    M = 10 * N;
    K = N;
    //*/
    vl_l RD(N),RS(M);
    vvd CH(K,vd(N));
    rep(i,0,N-1){
        RD[i].fi = rand(0,10000);
        RD[i].se = rand(0,10000);
    }
    rep(i,0,M-1){
        RS[i].fi = rand(0,10000);
        RS[i].se = rand(0,10000);
    }
    rep(i,0,K-1)rep(j,0,N-1)CH[i][j] = randdouble(0.1,0.9);
    
    RDin_ = RD;
    RS_ = RS;
    CH_ = CH;
}

void INPUT(){
    cin>>N>>M>>K;
    vl_l RD(N),RS(M);
    rep(i,0,N-1)cin>>RD[i].fi>>RD[i].se;
    rep(i,0,M-1)cin>>RS[i].fi>>RS[i].se;
    RDin_ = RD;
    RS_ = RS;
    vvd CH(K,vd(N));
    rep(i,0,K-1)rep(j,0,N-1)cin>>CH[i][j];
    CH_ = CH;
    
    cerr<<"[DATA] N = "<<N<<endl;
    cerr<<"[DATA] M = "<<M<<endl;
    cerr<<"[DATA] K = "<<K<<endl;
    cerr<<"[DATA] KN = "<<llround(K * 10 / (dd)N)<<endl;
}
vd PARAMS;


void OUTPUT(vl d,ll st,vvl to){
    assert((int)d.size() == N && (int)to.size() == M);
    rep(i,0,N-1)cout<<d[i]<<" ";cout<<endl;
    cout<<st<<endl;
    for(const vl &v:to){
        if(v[0] == NIL){
            assert((int)v.size() == 1);
        }else{
            assert((int)v.size() == 3);
        }
        for(ll x:v)cout<<x<<" ";cout<<endl;
    }
}
void ReStore(vl JUN,vl RJUN,vl_l to,vl ks,vl dret, vl&od, ll&ost, vvl&oto){
    od = dret;
    ost = JUN[0];
    oto = vvl(M);
    rep(i,0,M-1){
        if(to[i] == NIL2){
            oto[JUN[i] - N] = {-1};
        }else{
            assert(to[i].fi != NIL && to[i].se != NIL);
            
            ll to1 = JUN[to[i].fi];
            ll to2 = JUN[to[i].se];
            oto[JUN[i] - N] = {ks[i], to1, to2};
        }
    }
}

ll NUM(ll kd,ll i){
    if(kd == 0){//dispose
        return i;
    }else if(kd == 1){//separate
        return N + i;
    }else if(kd == 2){//enter
        return N + M;
    }else{
        assert(0);return NIL;
    }
}
l_l XYin(ll i){
    if(0 <= i && i < N){
        return RDin_[i];
    }else if(i < N + M){
        return RS_[i - N];
    }else if(i == N + M){
        return {0,5000};
    }else{
        DEB(i);
        assert(0);return NIL2;
    }
}

bool MyIntersect(const l_l& p1, const l_l& p2, const l_l& q1, const l_l& q2) {
    
    // 共有端点だけの接触はセーフ
    // 線分として一致していてもセーフ
        if (p1 == q1 || p1 == q2 || p2 == q1 || p2 == q2) return false;//追加してる
    
    auto sgn = [](ll x) -> int { return (x > 0) - (x < 0); };

    auto orientation = [&](const l_l& a, const l_l& b, const l_l& c) -> int {
        __int128 cross =
            (__int128)(b.first  - a.first ) * (c.second - a.second) -
            (__int128)(b.second - a.second) * (c.first  - a.first );
        return (cross > 0) ? 1 : (cross < 0) ? -1 : 0;
    };

    if (std::max(p1.first,  p2.first)  < std::min(q1.first,  q2.first) ||
        std::max(q1.first,  q2.first)  < std::min(p1.first,  p2.first) ||
        std::max(p1.second, p2.second) < std::min(q1.second, q2.second) ||
        std::max(q1.second, q2.second) < std::min(p1.second, p2.second)) {
        return false;
    }

    int o1 = orientation(p1, p2, q1);
    int o2 = orientation(p1, p2, q2);
    int o3 = orientation(q1, q2, p1);
    int o4 = orientation(q1, q2, p2);

    return (o1 * o2 <= 0) && (o3 * o4 <= 0);
}

dd DIST(ll a,ll b){
    l_l c = XYin(a);
    l_l d = XYin(b);
    return sqrt(powl(c.fi-d.fi,2) + powl(c.se-d.se,2));
}
dd DIST(l_l c,l_l d){
    return sqrt(powl(c.fi-d.fi,2) + powl(c.se-d.se,2));
}


namespace Delaunay {

struct Tri {
    int a,b,c; // 頂点インデックス（ps拡張配列の添字）
};

struct P {
    double x,y;
    int id; // 元の点のインデックス（0..n-1）。スーパー三角形は -1 を使う
};

static constexpr long double EPS = 1e-12L;

// 三角形abcの外接円に点pが「厳密に内側」ならtrue（境界上はfalse）
inline bool in_circumcircle(const P& a, const P& b, const P& c, const P& p) {
    // 行列表現での判定（ロバスト性を多少上げるために translate）
    double ax = a.x - p.x, ay = a.y - p.y;
    double bx = b.x - p.x, by = b.y - p.y;
    double cx = c.x - p.x, cy = c.y - p.y;

    double det =
        (ax*ax + ay*ay) * (bx*cy - cx*by)
      - (bx*bx + by*by) * (ax*cy - cx*ay)
      + (cx*cx + cy*cy) * (ax*by - bx*ay);

    // 三角形abcの向きで符号が反転するので、向きも合わせる
    double orient = (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
    if (fabsl(orient) < EPS) return false; // 退化（三点共線）は外接円を作らない→内側ではない扱い

    // orient > 0 のとき det > 0 が内側。orient < 0 のとき det < 0 が内側。
    return (orient > 0 ? det > EPS : det < -EPS);
}

// 無向辺（小さい方→大きい方に正規化）
struct Edge {
    int u,v; // ps拡張配列の添字
    Edge() {}
    Edge(int a,int b){ if(a<b){u=a;v=b;} else {u=b;v=a;} }
    bool operator<(const Edge& o) const {
        if(u!=o.u) return u<o.u;
        return v<o.v;
    }
    bool operator==(const Edge& o) const { return u==o.u && v==o.v; }
};

// Bowyer–Watson 本体：三角集合を返す
static std::vector<Tri> bowyer_watson(const std::vector<P>& pts) {
    const int N = (int)pts.size();

    // バウンディングボックス
    double minx=pts[0].x, maxx=pts[0].x, miny=pts[0].y, maxy=pts[0].y;
    for (int i=1;i<N;i++){
        minx = std::min(minx, pts[i].x); maxx = std::max(maxx, pts[i].x);
        miny = std::min(miny, pts[i].y); maxy = std::max(maxy, pts[i].y);
    }
    double dx = maxx - minx, dy = maxy - miny;
    double delta = std::max(dx, dy);
    double cx = (minx + maxx) / 2.0L;
    double cy = (miny + maxy) / 2.0L;

    // スーパー三角形（十分大きい正三角形）
    std::vector<P> ps = pts;
    int iA = (int)ps.size();
    int iB = iA + 1;
    int iC = iA + 2;
    ps.push_back(P{cx - 2*delta, cy - delta, -1});
    ps.push_back(P{cx + 2*delta, cy - delta, -1});
    ps.push_back(P{cx,          cy + 2*delta, -1});

    std::vector<Tri> tris;
    tris.push_back({iA,iB,iC});

    // 各点を順に挿入
    for (int ip=0; ip<N; ++ip){
        const P& p = ps[ip];

        // 1) 外接円に p を含む三角形を集める
        std::vector<int> bad;
        bad.reserve(tris.size());
        for (int tidx=0; tidx<(int)tris.size(); ++tidx){
            const Tri& T = tris[tidx];
            if (in_circumcircle(ps[T.a], ps[T.b], ps[T.c], p)) {
                bad.push_back(tidx);
            }
        }

        // 2) bad の「外周」を構成する辺（= 一度だけ現れる辺）を集める
        std::multiset<Edge> used;
        for (int tidx : bad){
            const Tri& T = tris[tidx];
            used.insert(Edge(T.a,T.b));
            used.insert(Edge(T.b,T.c));
            used.insert(Edge(T.c,T.a));
        }
        std::vector<Edge> boundary;
        boundary.reserve(used.size());
        for (auto it = used.begin(); it!=used.end(); ){
            auto range = used.equal_range(*it);
            int cnt = std::distance(range.first, range.second);
            if (cnt == 1) boundary.push_back(*it); // 1 回だけ→境界
            it = range.second;
        }

        // 3) bad 三角形を削除
        //    インデックスがずれるので「残すものだけコピー」方式にする
        std::vector<Tri> keep;
        keep.reserve(tris.size() - bad.size() + boundary.size());
        std::vector<char> is_bad(tris.size(), 0);
        for (int x : bad) is_bad[x]=1;
        for (int t=0;t<(int)tris.size();++t) if(!is_bad[t]) keep.push_back(tris[t]);
        tris.swap(keep);

        // 4) 境界辺 + p で新しい三角形を作る
        for (const auto& e : boundary){
            tris.push_back({e.u, e.v, ip});
        }
    }

    // スーパー三角形と接する三角形は捨てる
    std::vector<Tri> final_tris;
    final_tris.reserve(tris.size());
    for (const auto& T : tris){
        if (ps[T.a].id==-1 || ps[T.b].id==-1 || ps[T.c].id==-1) continue;
        final_tris.push_back(T);
    }
    return final_tris;
}

// 返却：ドロネー三角形分割に現れる「元の点インデックス」の辺集合
inline vl_l build_edges_from_tris(const std::vector<P>& ps,
                                  const std::vector<Tri>& tris){
    std::set<std::pair<ll,ll>> S; // 重複排除
    auto add_edge = [&](int u,int v){
        int iu = ps[u].id, iv = ps[v].id; // 元インデックス
        if (iu<0 || iv<0) return;         // スーパー三角形は除外
        ll a = std::min<ll>(iu,iv);
        ll b = std::max<ll>(iu,iv);
        S.insert({a,b});
    };
    for (const auto& T : tris){
        add_edge(T.a,T.b);
        add_edge(T.b,T.c);
        add_edge(T.c,T.a);
    }
    vl_l ret;
    ret.reserve(S.size());
    for (auto &e : S) ret.push_back(e);
    return ret;
}

inline vl_l Delaunay(const vl_l& ps_ll){
    // 入力点を P に詰め替え
    std::vector<P> ps; ps.reserve(ps_ll.size());
    for (int i=0;i<(int)ps_ll.size();++i){
        ps.push_back(P{(double)ps_ll[i].first,
                       (double)ps_ll[i].second, i});
    }
    auto tris = bowyer_watson(ps);
    return build_edges_from_tris(ps, tris);
}

} // namespace Delaunay


vvl_l TourBlockMulti(ll sep,const vvl &c){
    struct Q{
        vl_l tour;
        ll val;
    };
    vector<Q> Qs;
  //  vl_l besttour;
  //  ll bestval = 0;
    vl_l tour;
    ll cnt = 0;
    ll val = 0;
    vvl used(sep,vl(sep));
    vl_l ddxy = {{1,0},{0,1},{-1,0},{0,-1}};
    auto IN=[&](l_l a){
        return 0<=a.fi && a.fi<=sep-1 && 0<=a.se && a.se<=sep-1;
    };
    auto ADD=[](l_l a,l_l b){
        return (l_l){a.fi+b.fi,a.se+b.se};
    };
    auto dfs=[&](l_l pos,auto dfs)->void{
        assert(used[pos.fi][pos.se] == 0);
        used[pos.fi][pos.se] = 1;
        cnt += c[pos.fi][pos.se];
        val += c[pos.fi][pos.se] * tour.size();
        tour.pb(pos);
        if(cnt == N){
            Qs.pb({tour,val});
        //   if(chmax(bestval,val)){
        //        besttour = tour;
        //    }
            //returnするのはダメ（この後で逆戻しがあるから）
        }else{
            //tour.size() == 1の時は制限つけるか？
            for(l_l dxy:ddxy){
                l_l nx = ADD(pos,dxy);
                if(!IN(nx) || used[nx.fi][nx.se])continue;
                dfs(nx,dfs);
            }
        }
        
        tour.pop_back();
        val -= c[pos.fi][pos.se] * tour.size();
        cnt -= c[pos.fi][pos.se];
        used[pos.fi][pos.se] = 0;
    };
    rep(j,0,sep-1){
        assert(cnt == 0 && val == 0 && tour.empty());
        rep(a,0,sep-1)rep(b,0,sep-1)assert(used[a][b] == 0);
        dfs({0,j},dfs);
    }
    sort(all(Qs),[](Q a,Q b){return a.val > b.val;});
    vvl_l ret;
    for(Q q:Qs)ret.pb(q.tour);
    return ret;
}
vl OrderJun(vl_l tour,ll sep){
    vvvl ps(sep,vvl(sep));
    rep(i,N,N+M-1){
        l_l xy = {
            (XYin(i).fi * sep) / 10001,
            (XYin(i).se * sep) / 10001
        };
        ps[xy.fi][xy.se].pb(i);
    }
    vl JUN;
    auto ADD=[](l_l a,l_l b){
        return (l_l){a.fi+b.fi,a.se+b.se};
    };
    auto SUB=[](l_l a,l_l b){
        return (l_l){a.fi-b.fi,a.se-b.se};
    };
    auto MUL=[](l_l a,ll k){
        return (l_l){a.fi*k, a.se*k};
    };
    assert((int)tour.size() >= 2);
    rep(i,0,(int)tour.size()-1){
        l_l b = tour[i];
      //  l_l a = i>=1 ? tour[i-1] : SUB(MUL(tour[i],2),tour[i+1]);
        l_l a = i>=1 ? tour[i-1] : ADD(tour[i],{-1,0});
        l_l c = i<=(int)tour.size()-2 ? tour[i+1] : SUB(MUL(tour[i],2), tour[i-1]);
        
        l_l dir = SUB(c,a);
        assert(abs(dir.fi) + abs(dir.se) == 2);
        
        ll far = NIL;
        if(i == (int)tour.size()-1){
            vl_l v;
            rep(p,0,N-1){
                l_l XY = {
                    (XYin(p).fi * sep) / 10001,
                    (XYin(p).se * sep) / 10001
                };
                l_l xy = XYin(p);
                if(XY == tour.back()){
                    v.pb({xy.fi * dir.fi + xy.se * dir.se, p});
                }
            }
            SORT(v);
            assert(!v.empty());
            far = v.back().se;
        }
        
        vl_l v;
        for(ll x:ps[b.fi][b.se]){
            l_l xy = XYin(x);
            
            ll weight;
            if(i < (int)tour.size() - 1){
                weight = xy.fi * dir.fi + xy.se * dir.se;
            }else{
                assert(far != NIL);
                weight = - DIST(x,far);
            }
            v.pb({weight,x});
        }
        SORT(v);
        for(l_l x:v)JUN.pb(x.se);
    }
    /*
    {//あとから追加にする
        //無駄だけど300ケースに1つくらいなので...
        vvl flag(sep,vl(sep));
        for(l_l x:tour)flag[x.fi][x.se] = 1;
        rep(i,0,sep-1)rep(j,0,sep-1)if(flag[i][j] == 0){
            JUN.insert(JUN.end(),all(ps[i][j]));
        }
    }*/
   // rep(i,0,N-1)JUN.pb(i);
    return JUN;
}

void VerticalOrdering(vl&JUN, vl&RJUN){
    
    vvl nears(N);
    ll near_ko = 3;
    rep(i,0,N-1){
        vd_l v;
        rep(j,N,N+M-1){
            v.pb({DIST(i,j),j});
        }
        SORT(v);
        rep(j,0,near_ko - 1){
            nears[i].pb(v[j].se);
        }
    }
    
    auto JUNval_=[&](const vl &jun){
        assert((int)jun.size() <= M);
        vl rjun(N+M,NIL);//(-1のポイントが入っちゃう時があるけど、さすがに採用されないはず）
        rep(i,0,(int)jun.size()-1)rjun[jun[i]] = i;
        rep(i,0,N+M-1){
            if(i <= N-1)assert(rjun[i]==NIL);
           // else assert(rjun[i] != NIL);
        }
        dd val = 0;
        rep(i,0,N-1){
            dd sum = 0;
            assert((int)nears[i].size() == near_ko);
            for(ll j:nears[i]){
               // assert(0 <= rjun[j] && rjun[j] <= M-1);/(-1のポイントが入っちゃう時があるけど、さすがに採用されないはず）
                sum += (dd)rjun[j] / (M-1);
            }
            dd ave = sum / (dd)near_ko;
            val += ave;
            if(ave <= 1.0 / 8) val -= 1;
            if(ave <= 1.0 / 16)val -= 1;
            if(ave <= 1.0 / 32)val -= 1;
            if(ave <= 1.0 / 64)val -= 1;
            if(ave <= 1.0 / 128)val -= 1;
            if(ave <= 1.0 / 256)val -= 1;
            if(ave <= 1.0 / 512)val -= 1;
            if(ave <= 1.0 / 1024)val -= 1;
            //cerr<<ave<<" ";
        }
        //cerr<<" val/N"<<val / N<<endl;
        return val;
    };
    dd JUNval = -inf;
    
    ll bestsep = NIL;
    rep(sep,3,4){//sep=5の高速化やる？1%あがるかどうかくらい
        vvl c(sep,vl(sep));
        rep(i,0,N-1){
            l_l xy = {
                (XYin(i).fi * sep) / 10001,
                (XYin(i).se * sep) / 10001
            };
            c[xy.fi][xy.se]++;
        }
        vvl_l tourblocks = TourBlockMulti(sep,c);
        
        for(vl_l tourblock:tourblocks){
            vl jun = OrderJun(tourblock, sep);
            dd junval = JUNval_(jun);
            if(chmax(JUNval,junval)){
                JUN = jun;
                /*
                 DEB(c);
                vvl revtour(sep,vl(sep,NIL));
                rep(i,0,(int)tourblock.size()-1){
                    revtour[tourblock[i].fi][tourblock[i].se] = i;
                }
                DEB(revtour);
                 //*/
                bestsep = sep;
            }
        }
    }
    cerr<<"[DATA] sep = "<<bestsep<<endl;
    {
        cerr<<"[DATA] UP1 = "<<M -JUN.size()<<endl;
        vl used(N+M);
        for(ll x:JUN)used[x]=1;
      //  DEB(used);
        rep(i,0,N-1)assert(used[i]==0);
        rep(i,N,N+M-1)if(used[i]==0)JUN.pb(i);
    }
    rep(i,0,N-1)JUN.pb(i);
    assert((int)JUN.size() == M+N);
    RJUN = vl(N+M,NIL);
    rep(i,0,N+M-1){
        assert(0<=JUN[i] && JUN[i]<=N+M-1 && RJUN[JUN[i]] == NIL);
        if(i<=M-1)assert(N<=JUN[i] && JUN[i]<=N+M-1);
        else assert(0<=JUN[i] && JUN[i] <= N-1);
        RJUN[JUN[i]] = i;
    }
    /*
    {
        rep(i,0,N+M-1){
            l_l p = XYin(JUN[i]);
            cout<<p.fi<<" "<<p.se<<endl;
        }
        exit(0);
    }
     */
 //   DEB(JUN);
 //   DEB(RJUN);
}

void ChoiceEdges(const vl&JUN, const vl&RJUN, vvl&CHI){
    /*
     辺の数は割と多くても良さそうだが、時間がかかりすぎてはいけない。
     なので辺を増やしてスコアが変わらないなら、増やさない
     ・ドロネー分割
     ・幾何的に近い頂点
     　・startSEPの近く
     　・Disposeの近く
     ・JUNの順序が近い頂点
     
     */
    ll startSEP = JUN[0];
    
    vl_l edges;
    edges.pb({startSEP,NUM(2,0)});
    vvl edgeflag(N+M,vl(N+M));
    auto push=[&](l_l e){
        assert(edgeflag[e.fi][e.se] == 0);
        assert(edgeflag[e.se][e.fi] == 0);
        if(e.fi > e.se)swap(e.fi,e.se);
        edgeflag[e.fi][e.se] = edgeflag[e.se][e.fi] = 1;
        edges.pb(e);
    };
    
    vl_l keis;
    for(ll i:(vl){1,2,3,4,5,10}){
        keis.pb({1,i});
        if(i != 1)keis.pb({i,1});
    }
    
    for(l_l kei:keis){
        vl_l ps;
        rep(i,0,N+M-1){
            ps.pb({XYin(i).fi * kei.fi, XYin(i).se * kei.se});
        }
        vl_l alledges = Delaunay::Delaunay(ps);
        for(l_l e:alledges){
            assert(e.fi != e.se);
            if(e.fi > e.se)swap(e.fi,e.se);
            assert(e.se < NUM(2,0));
            if(edgeflag[e.fi][e.se])continue;
            if(e.se < N)continue;
            if(!MyIntersect(XYin(startSEP),XYin(NUM(2,0)),XYin(e.fi),XYin(e.se))){
                push(e);
            }
        }
    }
    
    rep(i,0,N+M-1){
        vl_l v;
        rep(j,0,N+M-1){
            if(i == j)continue;
            if(i <= N-1 && j <= N-1)continue;
            v.pb({DIST(i,j),j});
        }
        SORT(v);
        /*
        rep(j,0,9){
            cerr<<edgeflag[i][v[j].se]<<" ";
        }cerr<<endl;
         */
        ll sz_dis = 30;
        ll sz_start = 20;
        ll sz_sep = 15;
        ll sz = (i <= N - 1 ? sz_dis : i == startSEP ? sz_start : sz_sep);
        rep(j,0,sz-1){
            l_l e = {i,v[j].se};
            if(edgeflag[e.fi][e.se] == 1)continue;
            if(!MyIntersect(XYin(e.fi),XYin(e.se),XYin(startSEP),XYin(NUM(2,0)))){
                push({i,v[j].se});
            }
        }
    }
    
    /*
    {
        ll sz = edges.size();
        ERASE(edges);
        assert(sz == (int)edges.size());
    }*/
    
    ERASE(edges);
    
    CHI = vvl(M);
    {
        ll cnt = 0;
        for(l_l e:edges){
            if(e.fi == NUM(2,0))cnt++;
            if(e.se == NUM(2,0))cnt++;
        }
        assert(cnt == 1);
    }
    
    for(l_l e:edges){
        if(e.fi == NUM(2,0) || e.se == NUM(2,0))continue;
        ll f1 = RJUN[e.fi];
        ll f2 = RJUN[e.se];
        if(f1 > f2)swap(f1,f2);
        assert(f1 <= M-1 && f2 <= N+M-1);
        CHI[f1].pb(f2);
    }
//    DEB(CHI);
    
    /*
    rep(UPDcount,1,10000000){
        vl ng(M + N);
        rep(i,0,M-1){
            if(CHI[i].empty())ng[i] = 1;
        }
        bool upd = false;
        rep(i,0,M-1){
            rrep(j,(int)CHI[i].size()-1,0){
                if(ng[CHI[i][j]]){
                    CHI[i].erase(CHI[i].begin() + j);
                    upd = true;
                }
            }
        }
        if(upd){
            cerr<<"[DATA] UPD = "<<UPDcount<<endl;
        }
        if(!upd)break;
    }*/
    //*
   // ll addI = M <= 200 ? 5 : M <= 250 ? 1 : 0;//TODO　調整
    ll addI = 5;
    rep(i,0,M-2){
        rep(j,i+1,i + addI){
            if(j >= M)break;
            if(!MyIntersect(XYin(startSEP),XYin(NUM(2,0)),XYin(JUN[i]),XYin(JUN[j]))){
                CHI[i].pb(j);
            }
        }
    }//*/
    rep(i,0,M-1){
        ERASE(CHI[i]);
    }
  //  DEB(CHI);
}

ll dpid(ll a,ll b){
    return a * N + b;
}

void Filldp1(const vl_l &to, const vl &ks,vd &dp1f){
    
    assert((int)dp1f.size() == (N+M) * N);
    if(dp1f.empty()){
        dp1f = vd((N+M) * N);
    }
    const size_t bytesVec = sizeof(double) * N * (N+M);
    memset(dp1f.data(),0,bytesVec);
    rep(j,0,N-1)dp1f[dpid(0,j)] = 1;
    
    vl visited(N+M);visited[0] = 1;
    
    rep(i,0,M-1){
        /*
        if(to[i].fi == NIL){
            assert(to[i].se == NIL);
        }else{
            assert(i < to[i].fi && i < to[i].se);
        }*/
        
        if(to[i].fi == NIL)continue;
        if(visited[i] == 0)continue;
        
        ll k = ks[i];
        rep(j,0,N-1){
            dp1f[dpid(to[i].fi,j)] += dp1f[dpid(i,j)] * CH_[k][j];
            dp1f[dpid(to[i].se,j)] += dp1f[dpid(i,j)] * (1 - CH_[k][j]);
        }
        visited[to[i].fi] = visited[to[i].se] = 1;
    }
}

vd deldp1f;//(N+M) * N
vl delupds;
void Filldeltadp1(const vd&dp1f, const vl_l&to,const vl&ks,vl mae,vl ato,ll start){
    //dp1fはdpid(start,*)しか使わないはず
    assert((int)mae.size() == 3 && (int)ato.size() == 3);
    assert(0 <= mae[0] && mae[0] <= K-1);
    assert(0 <= ato[0] && ato[0] <= K-1);
    rep(i,1,2){
        assert(start < mae[i]);
        assert(start < ato[i]);
    }
    if(deldp1f.empty())deldp1f = vd((N+M) * N);
    
    assert((int)deldp1f.size() == (N+M) * N);
    delupds.clear();
    {
        dd sump = 0;
        rep(j,0,N-1)sump += dp1f[dpid(start,j)];
    //    assert(sump >= -(1e-6));
        if(sump < EPS)return;
    }
    
    static ll st_loop = 0;
    st_loop++;
    static vl visited(N+M);
    rep(i,1,2){
        visited[mae[i]] = st_loop;
        visited[ato[i]] = st_loop;
    }
    rep(i,start+1,N+M-1){
        if(visited[i] != st_loop)continue;
        if(i <= M - 1){
         //   assert(to[i].fi != NIL && to[i].se != NIL);
            visited[to[i].fi] = visited[to[i].se] = st_loop;
        }
        delupds.pb(i);
        rep(j,0,N-1)deldp1f[dpid(i,j)] = 0;
    }
    {
        ll k = mae[0];
        rep(j,0,N-1){
            deldp1f[dpid(mae[1],j)] -= dp1f[dpid(start,j)] * CH_[k][j];
            deldp1f[dpid(mae[2],j)] -= dp1f[dpid(start,j)] * (1 - CH_[k][j]);
        }
    }
    {
        ll k = ato[0];
        rep(j,0,N-1){
            deldp1f[dpid(ato[1],j)] += dp1f[dpid(start,j)] * CH_[k][j];
            deldp1f[dpid(ato[2],j)] += dp1f[dpid(start,j)] * (1 - CH_[k][j]);
        }
    }
    
    for(ll i:delupds){
        if(i >= M)break;
        ll k = ks[i];
        rep(j,0,N-1){
            deldp1f[dpid(to[i].fi,j)] += deldp1f[dpid(i,j)] * CH_[k][j];
            deldp1f[dpid(to[i].se,j)] += deldp1f[dpid(i,j)] * (1 - CH_[k][j]);
        }
    }
}

// 最大重み割当て（ハンガリー法, O(N^3)）
// 入力: W[i][j]（dd=double）
// 出力: d[i] = マッチングされた列 index（0-based）
vl HungarianMax(const vvd &W) {
    const int n = (int)W.size();
    assert(n > 0);
    for (int i = 0; i < n; ++i) assert((int)W[i].size() == n);

    // 1-indexed にする（典型的実装）
    vector<dd> u(n + 1), v(n + 1);
    vector<int> p(n + 1), way(n + 1);

    for (int i = 1; i <= n; ++i) {
        p[0] = i;
        int j0 = 0;
        vector<dd> minv(n + 1, 1e100);
        vector<char> used(n + 1, false);

        do {
            used[j0] = true;
            int i0 = p[j0];
            dd delta = 1e100;
            int j1 = 0;

            for (int j = 1; j <= n; ++j) if (!used[j]) {
                // 最大化したいので -W をコストとして扱う
                dd cur = -(W[i0 - 1][j - 1]) - u[i0] - v[j];
                if (cur < minv[j]) {
                    minv[j] = cur;
                    way[j] = j0;
                }
                if (minv[j] < delta) {
                    delta = minv[j];
                    j1 = j;
                }
            }

            for (int j = 0; j <= n; ++j) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }
            j0 = j1;
        } while (p[j0] != 0);

        // 増加道でマッチングを反転
        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    // p[j] = 行、j = 列（どちらも1-indexed）
    vl d(n, -1);
    for (int j = 1; j <= n; ++j) {
        int i = p[j];
        if (i >= 1 && i <= n) d[i - 1] = j - 1;
    }
    return d;
}
void FillValueHung(const vd &dp1f,vl &dret, vd &Values){
    //dp1fは、i=M ~ (M+N-1)までしかつかわないはず
    vvd W(N,vd(N));
    rep(i,M,M+N-1){
        rep(j,0,N-1){
            W[i - M][j] = dp1f[dpid(i,j)];
        }
    }
    dret = HungarianMax(W);
    Values = vd(N,0);
    rep(i,0,N-1)Values[i] = W[i][dret[i]];

}

void FillValueGreedyFast(const vd &dp1f,vl &dret, vd&Values){
    static vl past_dret;
    if(!past_dret.empty()){
        assert((int)past_dret.size() == N);
        bool OK = true;
        rep(i,0,N-1){
            if(dp1f[dpid(i+M,past_dret[i])] >= 0.5)continue;
            rep(j,0,N-1){
                if(dp1f[dpid(i+M,j)] > dp1f[dpid(i+M,past_dret[i])]){
                    OK=false;break;
                }
            }
            if(!OK)break;
        }
        if(OK){
            dret = past_dret;
            if((int)Values.size() != N)Values = vd(N,0);
            rep(i,0,N-1)Values[i] = dp1f[dpid(i+M,dret[i])];
            return;
        }
    }
    
    vector<pair<dd,ll>> junI;
    rep(i,M,M+N-1){
        dd mx = 0;
        rep(j,0,N-1)chmax(mx,dp1f[dpid(i,j)]);
        junI.pb({mx,i});
    }
    SORT(junI);reverse(all(junI));
   
    dret = vl(N,0);
    vl used(N,0);
    if((int)Values.size() != N)Values = vd(N,0);
    for(auto [_,i]:junI){//TODO？
        //i:頂点の番号（を、Mずらしたもの）
        //j:ゴミの種類
        
        ll mxj = -1;
        rep(j,0,N-1){
            if(used[j])continue;
            if(mxj == -1 || dp1f[dpid(i,mxj)] < dp1f[dpid(i,j)])mxj = j;
            
        }
        used[mxj] = 1;
        Values[mxj] = dp1f[dpid(i,mxj)];
        dret[i-M] = mxj;
    }
    past_dret = dret;
}
//vvd dp2;//vvd(N+M,vd(N)); dp1の逆から
vd dp2f;//(N+M) * N
void Filldp2(const vl_l &to, const vl &ks,const vl&dret){
    if(dp2f.empty()){
        dp2f = vd((N+M) * N);
    }
    
    const size_t bytesVec = sizeof(double) * N * (N+M);
    memset(dp2f.data(),0,bytesVec);
    
    rep(i,0,N-1){
        dp2f[dpid(i+M,dret[i])] = 1;
    }
    rrep(i,M-1,0){
        /*
        if(to[i].fi == NIL){
            assert(to[i].se == NIL);
        }else{
            assert(i < to[i].fi && i < to[i].se);
        }*/
        if(to[i].fi == NIL)continue;
        //if(visited[i] == 0)continue;
        
        ll k = ks[i];
        rep(j,0,N-1){
            dp2f[dpid(i,j)] += dp2f[dpid(to[i].fi,j)] * CH_[k][j];
            dp2f[dpid(i,j)] += dp2f[dpid(to[i].se,j)] * (1 - CH_[k][j]);
        }
        //visited[to[i].fi] = visited[to[i].se] = 1;
    }
}
void Updatedp2(vd &dp2f, const vl_l &to, const vl &ks,vl upd_starts){
 //   const size_t bytesVec = sizeof(double) * N * (N+M);
 //   memset(dp2f.data(),0,bytesVec);
 //   rep(i,0,start)rep(j,0,N-1)dp2f[dpid(i,j)] = 0;
//    memset(dp2f.data(),0,sizeof(double) * N * (start+1));
    
    assert(!upd_starts.empty());
    ll start = NIL;
    
    static ll st_loop = 0;
    st_loop++;
    static vl starts_flag(N+M);
    static vl visited_flag(N+M);
    for(ll x:upd_starts){
      //  assert(x <= M-1);
        bool noupd = true;
        ll k = ks[x];
        rep(j,0,N-1){
            if(abs(dp2f[dpid(to[x].fi,j)] * CH_[k][j] + dp2f[dpid(to[x].se,j)] * (1 - CH_[k][j])
                   - dp2f[dpid(x,j)]) > EPS){
                noupd = false;break;
            }
        }
        if(noupd)continue;
        chmax(start,x);
        starts_flag[x] = st_loop;
    }
    if(start == NIL)return;
    assert((int)starts_flag.size() == N+M);
    assert((int)visited_flag.size() == N+M);
    rrep(i,start,0){
        /*
        if(to[i].fi == NIL){
            assert(to[i].se == NIL);
        }else{
            assert(i < to[i].fi && i < to[i].se);
        }//*/
        if(to[i].fi == NIL)continue;
        if(starts_flag[i] != st_loop && visited_flag[to[i].fi] != st_loop && visited_flag[to[i].se] != st_loop)continue;
        visited_flag[i] = st_loop;
       // rep(j,0,N-1)dp2f[dpid(i,j)] = 0;
        
        ll k = ks[i];
        rep(j,0,N-1){
            /*
            dp2f[dpid(i,j)] = 0;
            dp2f[dpid(i,j)] += dp2f[dpid(to[i].fi,j)] * CH_[k][j];
            dp2f[dpid(i,j)] += dp2f[dpid(to[i].se,j)] * (1 - CH_[k][j]);
             //こっちなら9_1_4と一致する
             */
            dp2f[dpid(i,j)] = dp2f[dpid(to[i].fi,j)] * CH_[k][j] + dp2f[dpid(to[i].se,j)] * (1 - CH_[k][j]);
            //こっちなら9_1_5と一致する
        }
    }
}


void DebugToCheck(vl_l to){
    vl f(N+M);
    rep(i,0,M-1){
        ll cnt = 0;
        if(to[i].fi == NIL)cnt++;
        if(to[i].se == NIL)cnt++;
        assert(cnt == 0 || cnt == 2);
        if(cnt == 0){
            assert(i < to[i].fi && to[i].fi < N+M);
            assert(i < to[i].se && to[i].se < N+M);
        }
        if(cnt == 0)f[i] = 1;
    }
    rep(i,M,M+N-1)f[i] = 1;
   // DEB(f);
   // DEB(to);
    rep(i,0,M-1){
        if(to[i].fi != NIL){
            if(f[to[i].fi] == 0){
                DEB(i);
                DEB(to[i].fi);
                DEB(f[to[i].fi]);
            }
            assert(f[to[i].fi] == 1);
        }
        if(to[i].se != NIL){
            if(f[to[i].se] == 0){
                DEB(i);
                DEB(to[i].se);
                DEB(f[to[i].se]);
            }
            assert(f[to[i].se] == 1);
        }
    }
    cerr<<"DebugToCheck OK!"<<endl;
}

struct DynMask {
    std::vector<uint64_t> w;
    DynMask() = default;
    explicit DynMask(size_t nbits) : w((nbits + 63) >> 6, 0ull) {}
    inline void set(size_t i)       { w[i >> 6] |=  (1ull << (i & 63)); }
    inline void reset(size_t i)     { w[i >> 6] &= ~(1ull << (i & 63)); }
    inline bool test(size_t i) const{ return (w[i >> 6] >> (i & 63)) & 1ull; }
};
// 交差判定：テンポラリ無し、早期return
inline bool any_and(const DynMask& a, const DynMask& b) {
    const uint64_t* __restrict pa = a.w.data();
    const uint64_t* __restrict pb = b.w.data();
    const size_t words = a.w.size(); // b も同じ長さにしておく
    for (size_t i = 0; i < words; ++i) {
        if (pa[i] & pb[i]) return true;
    }
    return false;
}

//inline bool CanAdd(int e, const DynMask& nowbs) {
    
//#ifdef __AVX2__
//    return !any_and_avx(INTER[e], nowbs);
//#else
//    return !any_and(INTER[e], nowbs);
//#endif
//}

/*
//const ll BS = 20000;//TODO変更
void BuildEdges(vvl &edmp,vector<DynMask> &INTER,const vvl&CHI,const vl&JUN) {
    vl_l es;
    rep(i,0,M-1){
        for(ll j:CHI[i]){
            assert(i < j);
            es.pb({i,j});
        }
    }
    ll sz = es.size();
  //  assert(sz <= BS);
    cerr<<"[DATA] esz = "<<sz<<endl;
    edmp = vvl(N+M,vl(N+M,NIL));
    INTER = vector<DynMask>(sz,DynMask(sz));
    vl_l XY(N+M);
    rep(i,0,N+M-1)XY[i] = XYin(JUN[i]);
    
    rep(i,0,sz-1){
        rep(j,i+1,sz-1){
            //ここが(mt → mt2)の律速 TODO
            bool inter = MyIntersect(XY[es[i].fi],XY[es[i].se], XY[es[j].fi],XY[es[j].se]);
            if(inter){
                INTER[i].set(j);
                INTER[j].set(i);
            }
        }
    }
    rep(i,0,sz-1){
        assert(0<=es[i].fi && es[i].fi < es[i].se && es[i].se <= N+M-1);
        edmp[es[i].fi][es[i].se] = i;
        edmp[es[i].se][es[i].fi] = i;
    }
}*/

namespace BuildEdgesFast{

// ===== 高速セグメント交差ユーティリティ =====
// 共有端点は非交差扱い（元の仕様）
inline bool share_endpoint(const l_l& p1, const l_l& p2, const l_l& q1, const l_l& q2) {
    return (p1 == q1) || (p1 == q2) || (p2 == q1) || (p2 == q2);
}

inline int orient(const l_l& a, const l_l& b, const l_l& c) {
    __int128 cx =
        (__int128)(b.first  - a.first ) * (c.second - a.second) -
        (__int128)(b.second - a.second) * (c.first  - a.first );
    return (cx > 0) ? 1 : (cx < 0) ? -1 : 0;
}

// 事前に「バウンディングボックスが重なる」ことを確認済みのときに使う軽量版
inline bool seg_intersect_noBB(const l_l& p1, const l_l& p2,
                               const l_l& q1, const l_l& q2) {
    if (share_endpoint(p1,p2,q1,q2)) return false;
    int o1 = orient(p1,p2,q1);
    int o2 = orient(p1,p2,q2);
    int o3 = orient(q1,q2,p1);
    int o4 = orient(q1,q2,p2);
    return (o1 * o2 <= 0) && (o3 * o4 <= 0);
}

// バウンディングボックスの重なり（厳密な交差判定の前に軽く弾く）
inline bool bbox_overlap(const l_l& p1, const l_l& p2,
                         const l_l& q1, const l_l& q2) {
    ll pminx = std::min(p1.first,  p2.first);
    ll pmaxx = std::max(p1.first,  p2.first);
    ll pminy = std::min(p1.second, p2.second);
    ll pmaxy = std::max(p1.second, p2.second);

    ll qminx = std::min(q1.first,  q2.first);
    ll qmaxx = std::max(q1.first,  q2.first);
    ll qminy = std::min(q1.second, q2.second);
    ll qmaxy = std::max(q1.second, q2.second);

    if (pmaxx < qminx || qmaxx < pminx) return false;
    if (pmaxy < qminy || qmaxy < pminy) return false;
    return true;
}

// ===== ここから置き換え BuildEdges（スイープで候補圧縮） =====
void BuildEdgesFast(vvl &edmp, std::vector<DynMask> &INTER,
                const vvl& CHI, const vl& JUN) {
    // 1) エッジ列挙（es: 0..sz-1）
    vl_l es;
    es.reserve(1 << 15);
    rep(i,0,M-1){
        for (ll j : CHI[i]) {
            assert(i < j);
            es.pb({i,j});
        }
    }
    const int sz = (int)es.size();
    cerr << "[DATA] esz = " << sz << endl;

    edmp.assign(N+M, vl(N+M, NIL));
    INTER.assign(sz, DynMask(sz));

    // 2) 座標配列（JUN順でキャッシュする）
    vl_l XY(N+M);
    rep(i,0,N+M-1) XY[i] = XYin(JUN[i]);

    // 3) 各エッジのバウンディングボックス（スイープ前処理）
    struct EInfo {
        int idx;
        ll minx, maxx, miny, maxy;
    };
    std::vector<EInfo> info(sz);
    info.reserve(sz);

    for (int i = 0; i < sz; ++i) {
        const l_l &a = XY[es[i].fi], &b = XY[es[i].se];
        ll minx = std::min(a.first,  b.first);
        ll maxx = std::max(a.first,  b.first);
        ll miny = std::min(a.second, b.second);
        ll maxy = std::max(a.second, b.second);
        info[i] = { i, minx, maxx, miny, maxy };
    }

    // 4) minx 昇順にソートして x スイープ
    std::vector<int> order(sz);
    for (int i = 0; i < sz; ++i) order[i] = i;
    std::sort(order.begin(), order.end(),
              [&](int a, int b){ return info[a].minx < info[b].minx; });

    std::vector<int> active;     // 現在の minx にまだ被る候補
    active.reserve(1024);
    std::vector<int> nextActive; // compaction 用
    nextActive.reserve(1024);

    // 5) スイープ：同じ x 区間に重なる候補だけ厳密判定
    for (int ii = 0; ii < sz; ++ii) {
        const int ei = order[ii];
        const auto &I = info[ei];

        // active を圧縮（maxx < I.minx を除去）
        nextActive.clear();
        for (int ej : active) {
            if (info[ej].maxx >= I.minx) nextActive.pb(ej);
        }
        active.swap(nextActive);

        // 候補と照合
        const l_l p1 = XY[es[ei].fi], p2 = XY[es[ei].se];
        for (int ej : active) {
            const auto &J = info[ej];

            // y 区間で弾く
            if (J.maxy < I.miny || I.maxy < J.miny) continue;

            // 共有端点なら非交差（重い判定を避ける）
            const l_l q1 = XY[es[ej].fi], q2 = XY[es[ej].se];
            if (share_endpoint(p1, p2, q1, q2)) continue;

            // バウンディングボックスは重なっている前提だが、
            // 整合性のため軽量版を使う
            if (seg_intersect_noBB(p1, p2, q1, q2)) {
                INTER[ei].set(ej);
                INTER[ej].set(ei);
            }
        }

        // 自分を active に入れる
        active.pb(ei);
    }

    // 6) edmp の埋め（エッジ→インデックス）
    for (int i = 0; i < sz; ++i) {
        const int a = es[i].fi, b = es[i].se;
        assert(0 <= a && a < b && b <= N + M - 1);
        edmp[a][b] = i;
        edmp[b][a] = i;
    }
}
}//BuildEdgesFast


void BuildKpool(vl &nowks,vl &k_pool){
    vd_l v(K);
    rep(i,0,K-1){
        auto value_=[](vd p){
            dd value = 0;
            for(dd x:p)value += powl(max(x,1-x),5);
            return value;
        };
        dd value = value_(CH_[i]);
        v[i] = {value,i};
    }
    rep(i,0,K-1){
        rep(z,1,100000){
            k_pool.pb(i);
            if(z * 0.01 > v[i].fi)break;
        }
    }
    SORT(v);
    reverse(all(v));
    DEB(v);
    ll maxv = v[0].se;
    //nowks = vl(M,maxv);
    nowks = vl(M,0);
    ll ko = 5;
    rep(i,0,M-1){
        ll j = (i * ko) / M;
        assert(0 <= j && j <= ko - 1);
        nowks[i] = v[j].se;
    }
}
void BuildNowto(vl_l &nowto,vvl &CHI,vvl &edmp,vector<DynMask>&INTER,DynMask &nowbs){
    //CHIが変更されることに注意
    rep(UPDcount2,1,1000000000){
        nowto = vl_l(M,NIL2);
        vl_l es;
        vl ng(M + N);
        rep(i,0,M-1){
            if(CHI[i].empty())ng[i] = 1;
        }
        bool upd = false;
        rep(fise,1,2){
            rep(i,0,M-1){
                if(CHI[i].empty())continue;
                auto search_j=[&](){
                    vl chis = CHI[i];
                    if(fise == 1)reverse(all(chis));
                    for(ll j:chis){
                        if(fise == 2 && nowto[i].fi == j)continue;
                        bool ok = true;
                        for(l_l e:es){
                            assert(edmp[e.fi][e.se] != NIL && edmp[i][j] != NIL);
                            if(INTER[edmp[e.fi][e.se]].test(edmp[i][j])){
                                ok = false;break;
                            }
                        }
                        if(ok){
                            return j;
                        }
                    }
                    return NIL;
                };
                ll j = search_j();
                if(j != NIL){
                    es.pb({i,j});
                    if(fise == 1)nowto[i].fi = nowto[i].se = j;
                    else if(fise == 2)nowto[i].se = j;
                }
                
                if(nowto[i].fi == NIL){
                    CHI[i].clear();
                    ng[i] = 1;
                    upd = true;
                    break;
                }
            }
            if(upd)break;
        }
        rep(i,0,M-1){
            rrep(j,(int)CHI[i].size()-1,0){
                if(ng[CHI[i][j]]){
                    CHI[i].erase(CHI[i].begin() + j);
                    upd = true;
                }
            }
        }
        if(upd == false)break;
        cerr<<"[DATA] UPD2 = "<<UPDcount2<<endl;
    }
    rep(i,0,M-1){
        if(nowto[i] == NIL2)continue;
        assert(nowto[i].fi != NIL && nowto[i].se != NIL);
        nowbs.set(edmp[i][nowto[i].fi]);
        nowbs.set(edmp[i][nowto[i].se]);
    }
}



void Erase(vl &v,ll k){
    //vからkを見つけて、削除する
    //先頭の1つだけを削除する
    
    rep(i,0,(int)v.size()-1){
        if(v[i] == k){
            v.erase(v.begin() + i);
            return;
        }
    }
    assert(0);
}


void Anneal1(vvl CHI, const vl &JUN, vl_l &to__, vl &ks__, vl&dret__,bool TIMER,ll LOOP_MAX,dd end_time){
    vl lastdret;
    assert((int)CHI.size() == M);
    assert((int)JUN.size() == N + M);
    
    vvl edmp;//(N+M,vl(N+M,NIL));
    vector<DynMask> INTER;//edgesize * edgesize
    BuildEdgesFast::BuildEdgesFast(edmp,INTER,CHI,JUN);
    
    auto CanAdd=[&](l_l e,const DynMask &nowbs)__attribute__((noinline)) {
       // return !((INTER[edmp[e.fi][e.se]] & nowbs).any());
        return !any_and(INTER[edmp[e.fi][e.se]],nowbs);
        //    return !any_and(INTER[e], nowbs);
        
    };
    auto CrossProduct=[&](ll o,ll a,ll b){
        auto SUB=[](l_l a,l_l b){
            return (l_l){a.fi-b.fi,a.se-b.se};
        };
        l_l v1 = SUB(XYin(JUN[a]), XYin(JUN[o]));
        l_l v2 = SUB(XYin(JUN[b]), XYin(JUN[o]));
        return v1.fi * v2.se - v1.se * v2.fi > 0;
    };
    
    vl nowks;//(M,0);
    vl k_pool;
    BuildKpool(nowks,k_pool);
    vl_l nowto;//(M);
    DynMask nowbs(INTER.size());
    BuildNowto(nowto,CHI,edmp,INTER,nowbs);
    
    {
        rep(i,0,M-1){
            if(nowto[i] == NIL2)continue;
            if(nowto[i].fi == nowto[i].se)continue;
            if(CrossProduct(i,nowto[i].fi,nowto[i].se)){
                swap(nowto[i].fi,nowto[i].se);
            }
        }
    }
    /*
     //visualize
    rep(i,0,M-1){
        if(nowto[i].fi != nowto[i].se){
            ll a = JUN[i];
            ll b = JUN[nowto[i].fi];
            ll c = JUN[nowto[i].se];
            cout<<XYin(a).fi<<" "<<XYin(a).se<<" "<<XYin(b).fi<<" "<<XYin(b).se<<" "<<XYin(c).fi<<" "<<XYin(c).se<<endl;
        }
    }exit(0);*/
    
  //  DebugToCheck(nowto);
    cerr<<"[DATA] mt2 = "<<llround(stop_watch())<<endl;

    auto Neigh1 = [&](ll nowks_p) __attribute__((noinline)){
        while(1){
            ll ret = randVec(k_pool);
            if(ret != nowks_p)return ret;
        }
        assert(0);
    };
    auto Neigh2 = [&](ll p) __attribute__((noinline)){
        l_l nowto_p = nowto[p];
        static vl ok_childs(1000);//ok_childsでforループなどはダメ
        ll csz = 0;
        for(const ll &c:CHI[p]){
            if(CanAdd({p,c},nowbs)){
               // ok_childs.pb(c);
                ok_childs[csz] = c;
                csz++;
            }
        }
        assert(csz >= 1);//assert(!ok_childs.empty());
        if(csz == 1){//if((int)ok_childs.size() == 1){
            assert(nowto_p.fi == nowto_p.se && nowto_p.fi == ok_childs[0]);
            return nowto_p;//このとき変更なし
        }
        
        l_l nxto_p = nowto_p;
        if(rand(0,1)){
            while(nxto_p.fi == nowto_p.fi){
                nxto_p.fi = ok_childs[rand(0,csz-1)];//randVec(ok_childs);
            }
        }else{
            while(nxto_p.se == nowto_p.se){
                nxto_p.se = ok_childs[rand(0,csz-1)];//randVec(ok_childs);
            }
        }
        return nxto_p;
    };
    auto Neigh4=[&](l_l nowto_p,ll nowks_p) __attribute__((noinline)){
        bool f1 = false;
        bool f2 = false;
        ll ck1=NIL;ll ck2 = NIL;
        if(0 <= nowto_p.fi && nowto_p.fi <= M-1){
            ck1 = nowks[nowto_p.fi];
            assert(0<=ck1 && ck1<=K-1);
            if(nowks_p != ck1){
                f1 = true;
            }
        }
        if(0 <= nowto_p.se && nowto_p.se <= M-1){
            ck2 = nowks[nowto_p.se];
            assert(0<=ck2 && ck2 <=K-1);
            if(nowks_p != ck2){
                f2 = true;
            }
        }
        ll nxks_p = NIL;
        if(f1 && f2){
            if(rand(0,1) == 0)nxks_p = ck1;
            else nxks_p = ck2;
        }else if(f1){
            nxks_p = ck1;
        }else if(f2){
            nxks_p = ck2;
        }else{
            nxks_p = randVec(k_pool);//kind=1とだいたいおなじ
        }
        assert(nxks_p != NIL);
        return nxks_p;
    };
    auto simpleSum=[](const vd &vals){
        assert((int)vals.size() == N);
        dd sum = 0;
        rep(i,0,N-1)sum += vals[i];
        return sum;
    };
    
    vvl revCHI(N+M);rep(i,0,M-1)for(ll j:CHI[i])revCHI[j].pb(i);
    
    
    
    vd lastValues(N);
    vd nxdp1f((N+M) * N);//dp[i][j]:= ごみjが、頂点iに来る確率
    vd nowdp1f((N+M) * N);
    Filldp1(nowto,nowks,nowdp1f);
    FillValueGreedyFast(nowdp1f,lastdret,lastValues);
    auto revto=[](vl_l to){
        assert((int)to.size() == M);
        vvl rev(N+M);
        rep(i,0,M-1){
            if(to[i] == NIL2)continue;
            assert(i < to[i].fi && to[i].fi <= N+M-1 && i < to[i].se && to[i].se <= N+M-1);
            rev[to[i].fi].pb(i);
            rev[to[i].se].pb(i);
        }
        //ERASEしないべきだとおもう！
        return rev;
    };
    vvl nowrevto = revto(nowto);
    
    dd nowSumValue = simpleSum(lastValues);
    vl nowdret = lastdret;
    Filldp2(nowto,nowks,nowdret);
    vd nowdp2f = dp2f;
    bool nowdp2good = true;
    bool nowdp2dretgood = true;
    vl nowdp2_starts;
    
    auto Neigh5=[&]()__attribute((noinline)){
        ll target = NIL;//頂点番号
        vl all;
        //rep(i,N,N+M-1)all.pb(i);これはバグだなあ
        rep(i,M,M+N-1)all.pb(i);
        shuffle(all);//TODO
        for(ll i:all){
            bool zero = true;
            rep(j,0,N-1){
                if(nowdp1f[dpid(i,j)] > EPS){zero=false;break;}
            }
            if(zero){
                target = i;
               // rep(j,0,N-1)cerr<<nowdp1f[dpid(i,j)]<<" ";cerr<<endl;
               // rep(j,0,N-1)assert(nowdp1f[dpid(i,j)] <= 1e-5);
                break;
            }
        }
        assert(target != NIL);
        if(target == NIL)return NIL2;
        vl hasi(N+M);hasi[target] = 1;
        rrep(i,N+M-1,0){
            if(hasi[i] == 0)continue;
            if(!nowrevto[i].empty()){
                hasi[i] = 0;
                for(ll p:nowrevto[i]){
                    assert(p < i);
                    hasi[p] = 1;
                }
            }
        }
        vl hasis;
        rep(i,0,N+M-1)if(hasi[i])hasis.pb(i);shuffle(hasis);
        for(ll c:hasis){
            for(ll p:revCHI[c]){
                if(CanAdd({p,c},nowbs)){
                    return (l_l){p,c};
                }
            }
        }
        return NIL2;
    };
    auto Neigh3=[&](dd &bdelval,ll p,vl &bkcc) __attribute__((noinline)){
        bkcc[0] = NIL;
        bdelval = -inf;
        
        dd initdelval = 0;
        {
            ll k = nowks[p];
            ll c1 = nowto[p].fi;
            ll c2 = nowto[p].se;
            rep(i,0,N-1){
                initdelval -= nowdp1f[dpid(p,i)] * nowdp2f[dpid(c1,i)] * CH_[k][i];
                initdelval -= nowdp1f[dpid(p,i)] * nowdp2f[dpid(c2,i)] * (1 - CH_[k][i]);
            }
        }
        {
            static vl ok_childs(1000);//ok_childsでforループなどはダメ
            ll csz = 0;
            for(const ll &c:CHI[p]){
                if(CanAdd({p,c},nowbs)){
                    //ok_childs.pb(c);
                    ok_childs[csz] = c;
                    csz++;
                }
            }
            static vvd dp1fdp2f(1000,vd(N));
            rep(cp,0,csz-1){
                ll c = ok_childs[cp];
                rep(i,0,N-1){
                    dp1fdp2f[cp][i] = max(EPS,nowdp1f[dpid(p,i)]) * max(EPS,nowdp2f[dpid(c,i)]);
                    //dp1fdp2fに足し算とかはダメ　代入で
                }
            }
            rep(k,0,K-1){
                dd pv1 = -inf,pv2 = -inf;ll pn1 = NIL, pn2 = NIL;
                dd qv1 = -inf, qv2 = -inf;ll qn1 = NIL, qn2 = NIL;
                rep(cp,0,csz-1){
                    ll c = ok_childs[cp];
                    dd sval_p = 0;
                    dd sval_q = 0;
                    rep(i,0,N-1){
                        sval_p += dp1fdp2f[cp][i] * CH_[k][i];
                        sval_q += dp1fdp2f[cp][i] * (1 - CH_[k][i]);
                    }
                    if(pv1 < sval_p){
                        pn2 = pn1;pv2 = pv1;pv1 = sval_p;pn1 = c;
                    }else if(chmax(pv2,sval_p)){
                        pn2 = c;
                    }
                    if(qv1 < sval_q){
                        qn2 = qn1;qv2 = qv1;qv1 = sval_q;qn1 = c;
                    }else if(chmax(qv2,sval_q)){
                        qn2 = c;
                    }
                }
                if(pn1 == NIL || qn1 == NIL)continue;
                //assert(pn1 != NIL && qn1 != NIL && pn2 != NIL && qn2 != NIL);
                if(k == nowks[p] && pn1 == nowto[p].fi && qn1 == nowto[p].se){
                    if(pv1 - pv2 > qv1 - qv2){
                        swap(qv1,qv2);
                        swap(qn1,qn2);
                    }else{
                        swap(pv1,pv2);
                        swap(pn1,pn2);
                    }
                }
                if(pn1 == NIL || qn1 == NIL)continue;
                if(k == nowks[p] && pn1 == nowto[p].fi && qn1 == nowto[p].se){
                    assert(0);
                }
                
                dd delval = initdelval + pv1 + qv1;
                if(chmax(bdelval,delval)){
                    bkcc[0] = k;bkcc[1] = pn1;bkcc[2] = qn1;
                }
            }
        }
        return bkcc;
    };
    
    vl bestks;
    vl_l bestto;
    dd bestSumValue = 0;
    
    ll last_debug_print = 0;
    
    dd start_temp = 3;//N,M,K 大きい時は低温でもよいのかも
    dd end_temp = 1e-4;
    auto ANFN=[](dd diff,dd temp){
        if(diff >= 0)return true;
        return exp(diff / temp) > randdouble(0,1);
    };
    dd start_time = stop_watch();
    
    cerr<<"[DATA] ins = "<<llround((1 - nowSumValue / N)*1e9)<<endl;
    
    auto KEI_ = [](dd prog, bool &upd_to_best){
        static vd prog_resets;
        if(prog_resets.empty()){
            prog_resets.pb(-EPS);
            rep(i,1,10000){
                dd prog = 0.1 * i;
                prog_resets.pb(prog);
                if(prog >= 0.5)break;
                
                if(prog >= 0.99)break;//これはこの後のやつのために必要
            }
            prog_resets.pb(1 + EPS);
        }
        static ll prog_pos = 0;
        assert(prog_pos+1 <= (int)prog_resets.size() - 1);
        if(prog_resets[prog_pos + 1] < prog){
            prog_pos++;
            upd_to_best = true;
        }
        assert(prog_resets[prog_pos] <= prog && prog <= prog_resets[prog_pos+1]);
        dd KEI = (prog - prog_resets[prog_pos]) / (prog_resets[prog_pos+1] - prog_resets[prog_pos]);

        if(prog_pos+1 == (int)prog_resets.size()-1){
            KEI = (1.0 + KEI)/ 2.0;
        }
        return KEI;
    };
    
    
    dd prog = 0;
    dd temp = start_temp;
    
   // vl_l nxto;vl nxks;
    rep(loop,1,1e9){
        //dd prog = (dd)loop / LOOP_MAX;
        //dd prog = (stop_watch() - start_time) / (end_time - start_time);
        if((loop & 15) == 0){
            if(TIMER){
                prog = (stop_watch() - start_time) / (end_time - start_time);
            }else{
                prog = (dd)loop / LOOP_MAX;
            }
            if(prog > 1){
                cerr<<"[DATA] loop = "<<loop<<endl;
                break;
            }
            dd KEI;
            bool upd_to_best = false;
            KEI = KEI_(prog,upd_to_best);
        //    assert(-(1e-2) <= KEI && KEI <= 1 + 1e-2);
            if(upd_to_best){
                rep(i,0,M-1){
                    if(nowto[i] == NIL2)continue;
                    assert(nowto[i].fi != NIL && nowto[i].se != NIL);
                    nowbs.reset(edmp[i][nowto[i].fi]);
                    nowbs.reset(edmp[i][nowto[i].se]);
                }
                nowto = bestto;
                nowks = bestks;
                Filldp1(nowto,nowks,nowdp1f);
                FillValueGreedyFast(nowdp1f,lastdret,lastValues);
                nowSumValue = simpleSum(lastValues);
                rep(i,0,M-1)if(nowto[i] != NIL2){
                    nowbs.set(edmp[i][nowto[i].fi]);
                    nowbs.set(edmp[i][nowto[i].se]);
                }
                {
                    nowdret = lastdret;
                    Filldp2(nowto,nowks,nowdret);
                    nowdp2f = dp2f;
                    nowdp2good = true;
                    nowdp2dretgood = true;
                    nowdp2_starts = {};
                }
                nowrevto = revto(nowto);
            //   cerr<<"---------------------"<<loop<<":"<<nowSumValue<<endl;
            }
            temp = start_temp * powl(end_temp/start_temp,KEI);
        }
        
     //   nxks = nowks;
     //   nxto = nowto;
        
        
        ll p;
        do{
            p = rand(0,M-1);
        }while((int)CHI[p].size() <= 1);
        
        l_l nowto_p = nowto[p];
        ll nowks_p  = nowks[p];
        
        ll kind;
        while(1){
           // kind = randVec((vl){2,3,3,3,3,5});
            
            ll k2 = 1;
            ll k3 = 4;
            ll k5 = 1;
            ll rnd_gen = rand(1,k2+k3+k5);
            kind =  rnd_gen <= k2    ? 2:
                    rnd_gen <= k2+k3 ? 3: 5;
                    
          //  if(nowto[p].fi == nowto[p].se)if(kind == 4)continue;
            if(kind == 5){
                bool ex_zero = false;
                rep(i,M,M+N-1){
                    bool zero = true;
                    rep(j,0,N-1){
                        if(nowdp1f[dpid(i,j)] > EPS){zero=false;break;}
                    }
                    if(zero){
                        ex_zero=true;
                        break;
                    }
                }
                if(!ex_zero)kind = 2;
            }
            break;
        }
        
        dd difEval = -inf;
        l_l nxto_p = nowto_p;
        ll nxks_p = nowks_p;
        if(kind==1 || kind==2 ||kind==4 || kind==5){

        /*    if(kind == 4){
                ll nxks_p = Neigh4(nowto[p],nowks[p]);
                nxks[p] = nxks_p;//TODO ちゃんと子のkに変更されてるやつの比率確認したいね
            }else if(kind == 1){
                ll nxks_p = Neigh1(nowks[p]);
                assert(nxks_p != nowks[p]);
                nxks[p] = nxks_p;
            }else */if(kind == 2){
                nxto_p = Neigh2(p);
                if(nowto[p] == nxto_p){
                    continue;//SKIP　nowto,nowks変更してない
                }
              //  assert(CanAdd({p,nxto_p.fi},nowbs));
              //  assert(CanAdd({p,nxto_p.se},nowbs));
                nowto[p] = nxto_p;
            }else if(kind==5){
                l_l ed = Neigh5();
                if(ed != NIL2){
                    p = ed.fi;
                    nowto_p = nowto[p];
                    nowks_p = nowks[p];
                    nxto_p = nowto[p];
                    nxks_p = nowks[p];
                    assert(0 <= p && p <= M-1);
                    if(rand(0,1) == 0){
                   //     assert(nowto[ed.fi].fi != ed.se);//TODO念の為提出時は消す
                        nowto[ed.fi].fi = ed.se;
                    }else{
                  //      assert(nowto[ed.fi].se != ed.se);//TODO念の為提出時は消す
                        nowto[ed.fi].se = ed.se;
                    }
                    nxto_p = nowto[p];
                }else{
                    continue;//SKIP nowto,nowks変更してないのでOK
                }
            }else assert(0);
            Filldeltadp1(nowdp1f,nowto,nowks,{nowks_p,nowto_p.fi,nowto_p.se},{nxks_p,nxto_p.fi,nxto_p.se},p);
            rep(i,M,M+N-1)rep(j,0,N-1)nxdp1f[dpid(i,j)] = nowdp1f[dpid(i,j)];
            for(ll i:delupds)if(i >= M)rep(j,0,N-1)nxdp1f[dpid(i,j)] += deldp1f[dpid(i,j)];
            FillValueGreedyFast(nxdp1f,lastdret,lastValues);
            difEval = simpleSum(lastValues) - nowSumValue;
        }else if(kind == 3){
            if(!nowdp2good){
                if(nowdp2dretgood){
                    Updatedp2(nowdp2f,nowto,nowks,nowdp2_starts);
                }else{
                    Filldp2(nowto,nowks,lastdret);
                    nowdp2f = dp2f;
                }
                nowdp2good = true;
                nowdp2dretgood = true;
                nowdp2_starts = {};
            }
            
            dd bdelval = -inf;
            static vl bkcc(3);
            Neigh3(bdelval,p,bkcc);
            
            if(bkcc[0] == NIL)continue;//SKIP nowto,nowks変更してないのでOK
          //  assert(CanAdd({p,bkcc[1]},nowto));
          //  assert(CanAdd({p,bkcc[2]},nowto));
            assert((int)bkcc.size() == 3 && bkcc[0] != NIL);
            
            nxks_p = bkcc[0];
            nxto_p = {bkcc[1],bkcc[2]};
            /*
            {
                Filldp1(nxto,nxks,nxdp1f);
                vl debdret;
                vd debvalue;
                FillValueGreedy(nxdp1f,debdret,debvalue);
                dd debug_difEval = simpleSum(debvalue) - nowSumValue;
                if(debdret == nowdret){
                    assert(abs(debug_difEval - bdelval) < 1e-9);
                   // このassert通る(OK)
                    DEB(loop);
                }
            }//*/
            difEval = bdelval;//基本的に一致する。dretが変わるときのみズレる。
        }else{
            assert(0);
        }
        if(ANFN(difEval,temp)){
            
            if(kind == 3){
                Filldeltadp1(nowdp1f,nowto,nowks,{nowks_p,nowto_p.fi,nowto_p.se},{nxks_p,nxto_p.fi,nxto_p.se},p);
                for(ll i:delupds)rep(j,0,N-1)nowdp1f[dpid(i,j)] += deldp1f[dpid(i,j)];
                FillValueGreedyFast(nowdp1f,lastdret,lastValues);
            }else if(kind == 2 || kind == 5){
                for(ll i:delupds)rep(j,0,N-1)nowdp1f[dpid(i,j)] += deldp1f[dpid(i,j)];
            }else{
                assert(0);
            }
            nowSumValue = simpleSum(lastValues);//kind=3でもvalueを再計算しているのでこれでよい
            
            nowbs.reset(edmp[p][nowto_p.fi]);
            nowbs.reset(edmp[p][nowto_p.se]);
            nowbs.set(edmp[p][nxto_p.fi]);
            nowbs.set(edmp[p][nxto_p.se]);//下のif文（nowto[].se != nxto[].se）の中に入れてはいけない
            
            if(nowto_p.fi != nxto_p.fi){
                Erase(nowrevto[nowto_p.fi],p);
                nowrevto[nxto_p.fi].pb(p);
            }
            if(nowto_p.se != nxto_p.se){
                Erase(nowrevto[nowto_p.se],p);
                nowrevto[nxto_p.se].pb(p);
            }
            
            //nowks = nxks;nowto = nxto;
            nowto[p] = nxto_p;nowks[p] = nxks_p;
            //注意！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！nowks[p]とnowto[p]しか変えてないことを確認
            
            
            nowdp2good = false;
            nowdp2_starts.pb(p);
            if(nowdret != lastdret)nowdp2dretgood = false;
            
            /*
            if(lastdret != nowdret){
                DEB(loop);
                DEB(lastdret);
            }
             //*/
            nowdret = lastdret;
            
          //  cerr<<loop<<" "<<nowSumValue<<" "<<kind<<" "<<temp<<endl;
            
            if(bestSumValue < nowSumValue){
                bestSumValue = nowSumValue;
                bestks = nowks;
                bestto = nowto;
            }
        }else{
            //rollback
            
            nowto[p] = nowto_p;
            nowks[p] = nowks_p;
        }
        if(last_debug_print + LOOP_MAX/100 <= loop){
          //  cerr<<"loop "<<loop<<" "<<nowSumValue<<" "<<temp<<endl;
            last_debug_print = loop;
        }
    }
    to__ = bestto;
    ks__ = bestks;
    Filldp1(to__,ks__,nowdp1f);
    FillValueHung(nowdp1f,dret__,lastValues);//dret__に入力
    //DebugToCheck(bestto);
    cerr<<"[DATA] mnv = "<<lastValues[argmin(lastValues)]<<endl;
    cerr<<"[DATA] bestdif = "<<llround(((bestSumValue- nowSumValue) / N)*1e9)<<endl;
    cerr<<"[DATA] mys = "<<llround((1 - bestSumValue / N)*1e9)<<endl;
    
    /*
    {
        vl kcnt(K);
        rep(i,0,M-1){
            kcnt[ks[i]]++;
        }
        rep(i,0,K-1)cerr<<"k is "<<i<<":"<<kcnt[i]<<endl;
    }
     */
}

void solve_19_1(){
    //18_1 sz_sep=15 addI5
    //500-699で → 18.676 (9_1_1,9_1_2) → 18.665(9_1_3) → 18.669(9_1_4) → 18.687(9_1_5, 9_1_6, 9_1_7)
    //18.666(9_4_1) → 18.686(9_5_1)
    //18.581(10_1_1) → 18.586(10_1_2) → 18.595(10_1_3)
    //18.584(10_4_1) → 18.581(10_4_2) → 18.586(10_4_3)
    //18.569(10_5_1) → 18.527(10_5_3)
    //18.543(10_8_1,10_9_4) → 18.525(10_9_5)
    //18.541(11_1_1) → 18.535(11_2_1)
    //18.720(8_4_2_time)→18.708(8_5_1_time)→18.669(8_7_1_time)
    
    vl JUN,RJUN;//(N+M)、jun[0~M-1]はN~N+M-1。jun[M~M+N-1]は0~N-1。
    VerticalOrdering(JUN,RJUN);
    
    vvl CHI;//(M)
    ChoiceEdges(JUN,RJUN,CHI);
    cerr<<"[DATA] mt = "<<llround(stop_watch())<<endl;
    /*
    rep(i,0,M-1){
        for(ll j:CHI[i]){
            l_l pi = XYin(JUN[i]);
            l_l pj = XYin(JUN[j]);
            cout<<pi.fi<<" "<<pi.se<<" "<<pj.fi<<" "<<pj.se<<endl;
        }
    }//*/
    
    vl_l to;//(M)
    vl ks;//(M)
    vl dret;//(N)
    bool TIMER = true;
    
    Anneal1(CHI,JUN,to,ks,dret,TIMER,1e5,1985);
    
    
    vl od;ll ost;vvl oto;
    ReStore(JUN,RJUN,to,ks,dret,od,ost,oto);
    OUTPUT(od,ost,oto);
}

signed main(int argc,char *argv[]){
    fastio
    clock_start
    //ANNEALING::init_log_table();//使わないなら消す
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
    
    solve_19_1();
    
    cerr<<"[DATA] time = "<<llround(stop_watch())<<endl;
    return 0;
}

