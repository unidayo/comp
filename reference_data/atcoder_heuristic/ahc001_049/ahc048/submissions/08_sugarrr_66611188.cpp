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
const dd EPS=1E-6;
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
////////////////////////////////////////////////

typedef pair<dd,ll> d_l;
typedef vector<d_l> vd_l;
typedef vector<vd_l> vvd_l;

const ll N = 20;

struct INK{
    vd c = vd(3);
    dd g = 0.0;
};
typedef vector<INK> vI;
typedef vector<vI> vvI;
void ADDInk(INK &a,const INK &b){
    rep(i,0,2)a.c[i] += b.c[i];
    a.g += b.g;
}
INK Mix(INK a,INK b){
    INK c;
    rep(i,0,2)c.c[i] = a.c[i] + b.c[i];
    c.g = a.g + b.g;
    return c;
}
void SUBInk(INK &a,const INK &b){
    assert(a.g + EPS>= b.g);
    rep(i,0,2)a.c[i] -= b.c[i];
    a.g -= b.g;
}
void INKsub(const INK &a,const INK &b,INK &ans){
    assert(a.g + EPS >= b.g);
    rep(i,0,2)ans.c[i] = a.c[i] - b.c[i];
    ans.g = a.g - b.g;
}

struct Ban{
    vvl v = vvl(N,vl(N-1));
    vvl h = vvl(N-1,vl(N));
};
typedef vector<Ban> vB;
typedef vector<vB> vvB;

struct Man{
    ll type = -1;
    ll i1 = -1;
    ll j1 = -1;
    ll i2 = -1;//type=1ならば、i2はkを意味する
    ll j2 = -1;
};
typedef vector<Man> vM;
typedef vector<vM> vvM;

ll K,H,T,D;
vI oI;//own ink
vI tI;//target ink

void INPUT_FOR_PROFILE(){
    K = 20;
    H = 1000;
    T = 12000;
    D = 1000;
    
    /*------------- チューブ絵の具（own） ----------------*/
       struct RGB { double c, m, y; };
       vector<RGB> own(K);

       for (int k = 0; k < K; ++k) {
           own[k].c = rand(0, 100000) * 1e-5;
           own[k].m = rand(0, 100000) * 1e-5;
           own[k].y = rand(0, 100000) * 1e-5;
       }

       /*------------- 目標色（target, H = 1000） ----------*/
       vector<RGB> target(H);

       for (int i = 0; i < H; ++i) {
           /* Dirichlet(1,…,1) に相当する重み x_k’ */
           vector<double> x(K);
           double sum = 0.0;
           for (int k = 0; k < K; ++k) {
               double u = max(1e-12, randdouble(0.0, 1.0));   // ln(0) 回避
               x[k] = -log(u);
               sum += x[k];
           }
           for (int k = 0; k < K; ++k) x[k] /= sum;             // 正規化

           /* 離散 10⁻⁵ 精度で各成分を計算 */
           auto round5 = [](double v) {
               return static_cast<double>(llround(v * 1e5)) * 1e-5;
           };
           double c = 0, m = 0, y = 0;
           for (int k = 0; k < K; ++k) {
               c += x[k] * own[k].c;
               m += x[k] * own[k].m;
               y += x[k] * own[k].y;
           }
           target[i].c = round5(c);
           target[i].m = round5(m);
           target[i].y = round5(y);
       }
    oI.resize(K);
    rep(i,0,K-1){
        oI[i].c = {own[i].c,own[i].m,own[i].y};
        oI[i].g = 1.0;
    }
    tI.resize(H);
    rep(i,0,H-1){
        tI[i].c = {target[i].c,target[i].m,target[i].y};
        tI[i].g = 1.0;
    }
}

void INPUT(){
    ll gomi_N;
    cin>>gomi_N>>K>>H>>T>>D;
    
    oI.resize(K);
    rep(i,0,K-1){
        rep(j,0,2)cin>>oI[i].c[j];
        oI[i].g = 1.0;
    }
    tI.resize(H);
    rep(i,0,H-1){
        rep(j,0,2)cin>>tI[i].c[j];
        tI[i].g = 1.0;
    }
    
    cerr<<"[DATA] K = "<<K<<endl;
    cerr<<"[DATA] T = "<<T<<endl;
    cerr<<"[DATA] D = "<<D<<endl;
}
vd PARAMS;





namespace MakeBanDFS {

constexpr int N   = 20;                    // 20×20
constexpr int COL_PASS = 0;                // 通路は列 0
const int  dr[4]={-1,0,1,0};               // 上→右→下→左
const int  dc[4]={ 0,1,0,-1};
typedef l_l Cell;
bool dfs(int i,int j,int color,int K,int M,
         vector<vector<Cell>>& bd,
         vector<vector<char>>& vis,
         vector<int>& cnt)
{
    /* 全色完了 */
    if(color==K-1 && cnt[color]==M) return true;

    /* まだ現在色が M 未満なら延長を試す */
    if(cnt[color] < M){
        for(int d=0; d<4; ++d){
            int ni=i+dr[d], nj=j+dc[d];
            if(0<=ni && ni<N && 1<=nj && nj<N && !vis[ni][nj]){
                vis[ni][nj]=1;
                bd[ni][nj]={color,cnt[color]};
                ++cnt[color];
                if(dfs(ni,nj,color,K,M,bd,vis,cnt)) return true;
                --cnt[color];
                bd[ni][nj] = {-2,-2};
                vis[ni][nj]=0;
            }
        }
    }
    /* 現色がちょうど M に達したら次色を開始 */
    if(cnt[color]==M){
        int nxt = color+1;
        if(nxt==K) return false;
        for(int r=0;r<N;++r){              // 上から列 1 を走査
            int c = 1;
            if(!vis[r][c]){
                vis[r][c]=1;
                bd[r][c]={nxt,0};
                cnt[nxt]=1;
                if(dfs(r,c,nxt,K,M,bd,vis,cnt)) return true;
                cnt[nxt]=0;
                bd[r][c]={-2,-2};
                vis[r][c]=0;
            }
        }
    }
    return false;
}

/* --------- 盤面生成 API --------- */
vector<vector<Cell>> makeBoard(int K)
{
    const int M = (N*(N-1))/K;             // 各色セル数 (切り捨て)

    vector<vector<Cell>> bd(N, vector<Cell>(N,{-2,-2}));
    vector<vector<char>> vis(N, vector<char>(N,0));
    vector<int> cnt(K,0);

    /* 通路を固定 */
    for(int r=0;r<N;++r){
        bd[r][COL_PASS] = {-1,-1};
        vis[r][COL_PASS]=1;
    }

    /* 開始セル (0,1) */
    bd[0][1] = {0,0};
    vis[0][1]=1;
    cnt[0]=1;

    dfs(0,1,0,K,M,bd,vis,cnt);
    return bd;
}

} // namespace MakeBanDFS


void InitBan(Ban &ban,vl &Q,vvl_l &idxs,vector<pair<l_l,l_l>>&windows,l_l &Root){
    
    
    vector<vector<pair<ll,ll>>> s(N,vl_l(N));
    auto fill_snake_new = [&]() {
        // 中央列は空けておく（特殊セルとして -1, -1 を代入）
        rep(i,0,N-1)rep(j,0,N-1)s[i][j] = {-2,-2};
        for (int i = 0; i < N; ++i) {
            s[i][N / 2] = {-1, -1};
        }

        int leftT = (K + 1) / 2;     // ceil(K/2)
        int rightT = K - leftT;      // floor(K/2)

        int left_height = N / leftT;
        int right_height = left_height;
       // int right_height = (rightT > 0) ? N / rightT : 0;

        // ---------- 左側 ----------
        for (int t = 0; t < leftT; ++t) {
            for (int i = 0; i < left_height; ++i) {
                int x = t * left_height + i;
                if (x >= N) break;

                if (x % 2 == 0) {
                    for (int y = N / 2 - 1; y >= 1; --y) {
                        s[x][y] = {t, y};
                    }
                } else {
                    for (int y = 1; y < N / 2; ++y) {
                        s[x][y] = {t, y};
                    }
                }
            }
        }

        // ---------- 右側 ----------
        for (int i = 0; i < rightT; ++i) {
            int t = leftT + i;
            for (int j = 0; j < right_height; ++j) {
                int x = i * right_height + j;
                if (x >= N) break;

                if (x % 2 == 0) {
                    for (int y = N / 2 + 1; y < N; ++y) {
                        s[x][y] = {t, y};
                    }
                } else {
                    for (int y = N - 1; y > N / 2; --y) {
                        s[x][y] = {t, y};
                    }
                }
            }
        }

        // ---------- スネーク番号付与 ----------
        for (int t = 0; t < K; ++t) {
            // s[x][y].first == t の範囲を確認
            vector<vector<bool>> is_t(N, vector<bool>(N, false));
            for (int x = 0; x < N; ++x)
                for (int y = 1; y < N; ++y)
                    if (s[x][y].first == t)
                        is_t[x][y] = true;

            // 開始点探索: 中央列の隣にある t を探す
            int start_x = -1, start_y = -1;
            for (int x = 0; x < N; ++x) {
                if (N / 2 - 1 >= 0 && is_t[x][N / 2 - 1]) {
                    start_x = x;
                    start_y = N / 2 - 1;
                    break;
                }
                if (N / 2 + 1 < N && is_t[x][N / 2 + 1]) {
                    start_x = x;
                    start_y = N / 2 + 1;
                    break;
                }
            }

            if (start_x == -1) continue; // 該当なし

            int dir = (start_y < N / 2) ? -1 : 1;
            int x = start_x, y = start_y;
            int count = 0;
            while (0 <= x && x < N) {
                while (1 <= y && y < N && is_t[x][y]) {
                    s[x][y].second = count++;
                    y += dir;
                }
                x += 1;
                dir *= -1;
                y += dir;
                if (y < 1) y = 1;
                if (y >= N) y = N - 1;
            }
        }
    };
    if(K == 10){
        fill_snake_new();
        Root = {0,N/2};
    }else{
        s = MakeBanDFS::makeBoard(K);
        Root = {0,0};
        /*
        {
            vl cnt(K);
            rep(i,0,N-1)rep(j,0,N-1){
                if(s[i][j].fi >= 0){
                    assert(s[i][j].fi <= K-1);
                    cnt[s[i][j].fi]++;
                }
            }
            DEB(cnt);
        }*/
    }
    
    
    rep(i,0,N-1){
        rep(j,0,N-1){
            auto F=[](l_l a,l_l b){
                if(a.fi != b.fi)return true;
                if(a.fi == -1)return false;
                if(abs(a.se - b.se) != 1)return true;
                return false;
            };
            if(j < N-1)if(F(s[i][j],s[i][j+1]))ban.v[i][j] = 1;
            if(i < N-1)if(F(s[i][j],s[i+1][j]))ban.h[i][j] = 1;
        }
    }
    rep(z,0,K-1){
        vector<pair<ll,l_l>> v;
        rep(i,0,N-1)rep(j,0,N-1)if(s[i][j].fi == z){
            v.pb({s[i][j].se,{i,j}});
        }
        SORT(v);
        for(auto x:v)idxs[z].pb(x.se);
        Q[z] = idxs[z].size();
    }
    rep(z,0,K-1){
        l_l p = idxs[z][0];
        windows[z] = {p,{p.fi,Root.se}};
    }
    /*
    rep(i,0,K-1){
        DEB(i);
        DEB(idxs[i]);
    }*/
}


void OUTPUT(Ban b,vM ms){
    rep(i,0,N-1){
        rep(j,0,N-2){
            cout<<b.v[i][j]<<" ";
        }cout<<endl;
    }cout<<endl;
    rep(i,0,N-2){
        rep(j,0,N-1){
            cout<<b.h[i][j]<<" ";
        }cout<<endl;
    }cout<<endl;
    
    if((int)ms.size() > T){
        cerr<<"[DATA] OverError = "<<ms.size() - T<<endl;
        //おいのり
        while((int)ms.size() > T){
            rrep(p,(int)ms.size()-1,0){
                if(ms[p].type != 2){
                    ms.erase(ms.begin() + p);
                    break;
                }
            }
        }
    }
    
    for(Man m:ms){
        cout<<m.type<<" ";
        if(m.type == 1){
            cout<<m.i1<<" "<<m.j1<<" "<<m.i2<<endl;
        }else if(m.type == 2){
            cout<<m.i1<<" "<<m.j1<<endl;
        }else if(m.type == 3){
            cout<<m.i1<<" "<<m.j1<<endl;
        }else if(m.type == 4){
            cout<<m.i1<<" "<<m.j1<<" "<<m.i2<<" "<<m.j2<<endl;
        }else{
            assert(0);
        }
    }
    cerr<<"[DATA] rT = "<<ms.size()<<endl;
}

dd CalcErr(const INK& a,const INK &b){
    assert(abs(a.g) > EPS);
    assert(abs(b.g) > EPS);
    
    dd err = 0;
    rep(i,0,2){
        err += powl(a.c[i]/a.g - b.c[i]/b.g, 2.0);
    }
    return sqrt(err);
}
/*****************************************************************
   CalcScore  ―  AtCoder AHC “Mixing on the Palette” 用ローカル評価器
   ・Ban  : 初期仕切り状態（v : 縦壁  N×(N-1)， h : 横壁 (N-1)×N）
   ・ms   : 操作列（長さ ≤T，構造体 Man）
   ・oI   : K 本のチューブ色（グローバル）
   ・tI   : H 個のターゲット色（グローバル）
   ・K,H,T,D は既に入力済み（グローバル）

   返値  : 問題文の絶対スコア
 *****************************************************************/


//============================================================
//  AHC ― Palette Mixing  (bug‑fixed & optimized)
//  ・Ink/Well を vector<double> で統一
//  ・壁トグル時は接するウェルのみを正確に再構築
//  ・質量・色比率の保持ロジックを修正
//============================================================
// GPTsimulation 名前空間
//------------------------------------------------------------
namespace GPTsimulation{
//--------------------------------------------------
struct Ink{
    vd c = vd(3,0);
    dd g = 0;
};
// Well 構造体
//--------------------------------------------------
struct Well{
    dd g;   // 総質量[g]
    vd c;   // 比率 (3要素)
    int cap;// セル数
    Well(): g(0.0), c(3,0.0), cap(0) {}
};
using vWell = vector<Well>;
//--------------------------------------------------
// 隣接可否 (壁チェック)
//--------------------------------------------------
inline bool adjacent(const Ban& b,int i1,int j1,int i2,int j2){
    if(i1==i2){ int j=min(j1,j2); return b.v[i1][j]==0; }
    int i=min(i1,i2); return b.h[i][j1]==0;
}
//--------------------------------------------------

dd CalcErrInk(const INK& a,const Ink &b){
    //aがtargetなのでこれでOK
    
    dd err = 0;
    rep(i,0,2){
        err += powl(a.c[i] - b.c[i] ,2.0);
    }
    return sqrt(err);
}


// 全ウェル再構築 (フル)
//--------------------------------------------------
pair<vWell,vector<int>> rebuildWells(const Ban& b,
                                     const vWell& oldWell,
                                     const vector<int>& oldBelong)
{
    const int SZ=N*N; vector<int> belong(SZ,-1); vWell nwells;
    /*--- 旧ウェルのセル密度 ---*/
    vector<dd> dens(oldWell.size(),0.0); for(size_t w=0;w<oldWell.size();++w)
        if(oldWell[w].cap>0) dens[w]=oldWell[w].g/oldWell[w].cap;
    auto idx=[&](int i,int j){return i*N+j;};
    const int di[4]={1,-1,0,0}, dj[4]={0,0,1,-1};

    for(int si=0;si<N;++si)for(int sj=0;sj<N;++sj){
        int id=idx(si,sj); if(belong[id]!=-1) continue;
        nwells.emplace_back(); int wid=(int)nwells.size()-1; Well &w=nwells.back();
        queue<pair<int,int>> q; q.push({si,sj}); belong[id]=wid;
        while(!q.empty()){
            auto [i,j]=q.front(); q.pop(); ++w.cap;
            int old=oldBelong.empty()? -1 : oldBelong[idx(i,j)];
            if(old!=-1 && oldWell[old].cap>0){ dd d=dens[old]; w.g+=d; for(int k=0;k<3;k++) w.c[k]+=d*oldWell[old].c[k]; }
            for(int dir=0;dir<4;++dir){ int ni=i+di[dir],nj=j+dj[dir];
                if(ni<0||ni>=N||nj<0||nj>=N) continue; if(!adjacent(b,i,j,ni,nj)) continue;
                int nid=idx(ni,nj); if(belong[nid]!=-1) continue; belong[nid]=wid; q.push({ni,nj}); }
        }
        if(w.g>EPS) for(int k=0;k<3;k++) w.c[k]/=w.g;
    }
    return {nwells,belong};
}
//--------------------------------------------------
// ★局所更新：壁トグルによるウェル再構築
//--------------------------------------------------
void updateWellsAfterToggle(const Ban& b,
                            int i1,int j1,int i2,int j2,
                            vWell& wells,
                            vector<int>& belong)
{
    const int SZ=N*N; auto idx=[&](int i,int j){return i*N+j;};
    /*--- 旧 belong をバックアップ ---*/
    vector<int> prevBelong = belong;
    /*--- 旧ウェル毎セル質量（dens） ---*/
    vector<dd> dens(wells.size(),0.0); for(size_t w=0;w<wells.size();++w)
        if(wells[w].cap>0) dens[w]=wells[w].g/wells[w].cap;
    /*--- 影響ウェル ID ---*/
    int widA = prevBelong[idx(i1,j1)], widB = prevBelong[idx(i2,j2)];
    unordered_set<int> affected; if(widA!=-1) affected.insert(widA); if(widB!=-1) affected.insert(widB);
    if(affected.empty()) return;   // 壁外周の場合

    /*--- 影響セル列挙 & oldWid 記録 ---*/
    vector<int> targetCells; targetCells.reserve(SZ);
    vector<int> oldWidForCell(SZ,-1);
    for(int p=0;p<SZ;++p){ int ow=prevBelong[p]; if(affected.count(ow)){
            targetCells.push_back(p); oldWidForCell[p]=ow; belong[p]=-1; }
    }

    /*--- unaffected wells を保持 ---*/
    vector<Well> newWells; newWells.reserve(wells.size());
    vector<int> remapOld(wells.size(),-1);
    for(size_t w=0; w<wells.size(); ++w){ if(!affected.count((int)w)){
            remapOld[w]= (int)newWells.size(); newWells.push_back(wells[w]); } }

    /*--- unaffected cells の belong 更新 ---*/
    for(int p=0;p<SZ;++p){ if(belong[p]!=-1) belong[p]=remapOld[prevBelong[p]]; }

    /*--- BFS で新ウェル生成 ---*/
    const int di[4]={1,-1,0,0}, dj[4]={0,0,1,-1};
    vector<int> qbuf; qbuf.reserve(SZ);
    vector<char> visited(SZ,0);
    for(int root: targetCells){ if(visited[root]) continue;
        Well w; qbuf.clear(); qbuf.push_back(root); visited[root]=1; belong[root]=(int)newWells.size();
        for(size_t qi=0; qi<qbuf.size(); ++qi){ int id=qbuf[qi]; int i=id/N, j=id%N; ++w.cap;
            int ow=oldWidForCell[id]; if(ow!=-1 && wells[ow].cap>0){ dd d=dens[ow]; w.g+=d; for(int k=0;k<3;k++) w.c[k]+=d*wells[ow].c[k]; }
            for(int dir=0;dir<4;++dir){ int ni=i+di[dir], nj=j+dj[dir]; if(ni<0||ni>=N||nj<0||nj>=N) continue; if(!adjacent(b,i,j,ni,nj)) continue;
                int nid=idx(ni,nj); if(oldWidForCell[nid]==-1) continue; if(visited[nid]) continue; visited[nid]=1; belong[nid]=(int)newWells.size(); qbuf.push_back(nid); }
        }
        if(w.g>EPS) for(int k=0;k<3;k++) w.c[k]/=w.g; newWells.push_back(std::move(w));
    }
    wells.swap(newWells);
}

} // namespace GPTsimulation
long long CalcScore(Ban b,const vM& ms)
{
    using namespace GPTsimulation; const int SZ=N*N;
    /*--- 初期ウェル ---*/
    vector<int> belong; vWell wells; tie(wells,belong)=rebuildWells(b,wells,belong);

    /*--- シミュレーション ---*/
    long long vhc=0,vh2=0; dd Esum=0.0; size_t tgtIdx=0;
    auto idx=[&](int i,int j){return i*N+j;};

    for(const Man& m: ms){
        if(m.type==1){           // tube
            ++vhc; int w=belong[idx(m.i1,m.j1)]; auto &W=wells[w]; dd remain=W.cap-W.g; dd add=min<dd>(1.0,remain); if(add<EPS) continue;
            dd newG=W.g+add; for(int k=0;k<3;k++) W.c[k]=(W.c[k]*W.g+oI[m.i2].c[k]*add)/newG; W.g=newG;
        }
        else if(m.type==2){      // deliver
            int w=belong[idx(m.i1,m.j1)]; auto &W=wells[w];
            Ink cur={W.c, W.g};
            if(tgtIdx<tI.size()) Esum+=CalcErrInk(tI[tgtIdx],cur); ++tgtIdx; ++vh2; W.g = (W.g>=1.0? W.g-1.0 : 0.0);
        }
        else if(m.type==3){      // dump
            int w=belong[idx(m.i1,m.j1)]; wells[w].g=max<dd>(0.0,wells[w].g-1.0);
        }
        else if(m.type==4){      // toggle
            if(m.i1==m.i2){ int jj=min(m.j1,m.j2); b.v[m.i1][jj]^=1; }
            else{ int ii=min(m.i1,m.i2); b.h[ii][m.j1]^=1; }
            updateWellsAfterToggle(b,m.i1,m.j1,m.i2,m.j2,wells,belong);
        }
    }

    if(vh2!=H) cerr<<"[WARN] deliver count "<<vh2<<" != H="<<H<<"\n";
    vhc-=vh2; long long absScore=1LL + D*vhc + llround(Esum*1e4);
    cerr<<"score detail: (V-H)="<<vhc<<"  D*(V-H)="<<D*vhc
        <<" / Err="<<Esum<<"  1e4*Err="<<llround(Esum*1e4)
        <<" / TOTAL="<<absScore<<"\n";
    cerr<<"[DATA] ds = "<<llround(D * vhc)<<endl;
    cerr<<"[DATA] es = "<<llround(Esum*  1e4)<<endl;
    
    return absScore;
}


dd INKdispose(INK &a){
    assert(a.g > EPS);
    dd sub = min(1.0,a.g);
    dd keisuu = (a.g - sub) / a.g;
    assert(0 <= keisuu && keisuu < 1);
    rep(i,0,2)a.c[i] *= keisuu;
    a.g -= sub;
    return sub;//捨てた重さをreturn
}


pair<Ban,vM> BYsolve_1_6(){
    ll sz_array = 10;
    vector<vector<pair<vl,INK>>> precals(sz_array + 1);
    precals[0] = {{}};
    {
        ll maxko = (K <= 15 ? 4 : 3);
        rep(sz,1,maxko){
            for(pair<vl,INK> pre:precals[sz-1]){
                ll last = pre.fi.empty() ? 0 : pre.fi.back();
                rep(add,last,K-1){
                    pair<vl,INK> next = pre;
                    next.fi.pb(add);
                    ADDInk(next.se,oI[add]);
                    precals[sz].pb(next);
                }
            }
        }
    }
    
    
    ll wsz = 20;
  //  if(K >= 10)wsz = 10;
  //  if(K >= 15)wsz = 5;
    dd welllimit = 10;
    vM mans;
    vI nowinks(wsz);
    vl_l SP(wsz);rep(i,0,wsz-1)SP[i] = {i,0};
    
    
    INK ink;
    
    rep(i,0,H-1){
        INK target = tI[i];
        ll MAXKO = llround((T - mans.size()) / (dd)(H - i));
        MAXKO /= 2;
        if(K <= 15){
            chmin(MAXKO,4);
        }else{
            chmin(MAXKO,3);
        }
        dd minerr = inf;
        vl minv;
        ll minrmv;
        ll minwnum;
        rep(wnum,0,wsz-1){
            ll maxko = MAXKO;
            chmin(maxko,welllimit - nowinks[wnum].g);
            {
                bool ok = true;
                rep(wp,0,wnum-1){
                    bool same = true;
                    rep(j,0,2)if(abs(nowinks[wnum].c[j] - nowinks[wp].c[j]) > EPS)same = false;
                    if(abs(nowinks[wnum].g - nowinks[wp].g) > EPS)same = false;
                    if(same){ok=false;break;}
                }
                if(!ok)continue;
            }
            rep(sz,0,maxko)for(const pair<vl,INK>& pre:precals[sz]){
                const vl &v = pre.fi;
                const INK &addink = pre.se;
             //   ll maxrmv = llround(nowinks[wnum].g) - (v.empty() ? 1 : 0);
                ll maxrmv = (ll)(nowinks[wnum].g) - (v.empty()? 1 : 0); //たいてい空のセルがあるので、3.99gのとき4回削除はしなくていいかな
                rep(rmv,0,maxrmv){
                    ink = nowinks[wnum];
                //    ink.g -= rmv;chmax(ink.g,0);
                    rep(rmvkai,1,rmv)INKdispose(ink);
                //    for(ll x:v)ink = Mix(ink,oI[x]);
                //    ink = Mix(ink,addink);
                    ADDInk(ink, addink);
                    dd err = CalcErr(ink,target) * 1e4 + D * ((int)v.size() + min(rmv,nowinks[wnum].g) * 0.01);//0.01適当だけど D小 についての戦略なのでOK
                    if(chmin(minerr,err)){
                        minv = v;
                        minrmv = rmv;
                        minwnum = wnum;
                    }
                }
            }
        }
        {
            INK &nowink = nowinks[minwnum];
            l_l SPpos = SP[minwnum];
            //  assert(!minv.empty());
              assert(minerr < inf/2);
              rep(_,1,minrmv){
                  mans.pb({3,SPpos.fi,SPpos.se});
                 // nowink.g -= 1.0;chmax(nowink.g,0);
                  INKdispose(nowink);
              }
              for(ll x:minv){
                  mans.pb({1,SPpos.fi,SPpos.se,x});
                  nowink = Mix(nowink,oI[x]);
              }
              {
                  mans.pb({2,SPpos.fi,SPpos.se});
                  assert(nowink.g >= 1 - EPS);
                //  nowink.g -= 1.0; chmax(nowink.g,0);
                  INKdispose(nowink);
              }
        }
     
    }
    
    Ban ban;
    rep(i,0,N-2){
        rep(j,0,N-1){
            ban.h[i][j] = 1;
        }
    }
    return {ban,mans};
}

namespace mix4 {
/*==============================================================
   mix4 名前空間
   -------------------------------------------------------------
   solve_coeff_mix4
      Ink A,B,C,D,Z が与えられたとき
         a·A ⊕ b·B ⊕ c·C ⊕ d·D  =  Z
      を満たす係数 {a,b,c,d} を実数で返す。
      （⊕ は Mix 関数と同じ合成規則）

      ・4 本で一意解が存在しない場合は空 vector を返す。
      ・グローバル typedef double dd; が存在する前提。
================================================================*/

/*------------ 4 × 4 ガウス消去（部分ピボット） ---------------*/
static bool solve4x4(dd M[4][5], dd out[4])
{
    for(int col=0; col<4; ++col){
        /* pivot 探索 */
        int piv = col;
        for(int r=col+1; r<4; ++r)
            if(std::fabs(M[r][col]) > std::fabs(M[piv][col])) piv = r;
        if(std::fabs(M[piv][col]) < 1e-12) return false;          // ランク落ち
        if(piv != col)
            for(int j=col; j<5; ++j) std::swap(M[piv][j], M[col][j]);

        /* 正規化 */
        dd inv = M[col][col];
        for(int j=col; j<5; ++j) M[col][j] /= inv;

        /* 他行を 0 に */
        for(int r=0; r<4; ++r) if(r != col){
            dd f = M[r][col];
            for(int j=col; j<5; ++j) M[r][j] -= f * M[col][j];
        }
    }
    for(int i=0;i<4;i++) out[i] = M[i][4];
    return true;
}

inline bool solve4x4_fast(const dd M_in[4][5], dd out[4]) {
    dd M[4][5];
    std::memcpy(M, M_in, sizeof(M));        // 20 個の double を一括コピー
    int idx[4] = {0,1,2,3};                 // 実際の行を示すインデックス表

    for (int col=0; col<4; ++col) {
        /* ---- pivot 選択（abs を避け２乗で比較） ---- */
        int piv = col;
        dd best = M[idx[piv]][col] * M[idx[piv]][col];
        for (int r=col+1; r<4; ++r) {
            dd v = M[idx[r]][col];
            v *= v;
            if (v > best) { best = v; piv = r; }
        }
        if (best < 1e-24) return false;     // ランク落ち

        if (piv != col) std::swap(idx[piv], idx[col]);

        /* ---- pivot 行の正規化 ---- */
        dd* prow = M[idx[col]];
        dd inv   = 1.0 / prow[col];
        for (int j=col; j<5; ++j) prow[j] *= inv;

        /* ---- 他行から消去 ---- */
        for (int r=0; r<4; ++r) if (r != col) {
            dd* row = M[idx[r]];
            dd f = row[col];
            if (f == 0.0) continue;         // スキップで分岐予測を助ける
            for (int j=col; j<5; ++j) row[j] -= f * prow[j];
        }
    }
    for (int i=0; i<4; ++i) out[i] = M[idx[i]][4];
    return true;
}


/*--------------------------------------------------------------
   solve_coeff_mix4
   ------------------------------------------------------------*/
std::vector<dd> solve_coeff_mix4(const INK& A, const INK& B,
                                 const INK& C, const INK& D,
                                 const INK& Z)
{
    /* Z.g == 0 の特殊ケース：0 係数で一致 */
    if(std::fabs(Z.g) < 1e-12) return {0,0,0,0};

    /* 拡大係数行列 4×5 を作成 */
    dd M[4][5] = {
        { A.g,           B.g,          C.g,          D.g,       Z.g },          // 質量
        { A.c[0],      B.c[0],      C.c[0],      D.c[0],      Z.c[0] },  // 色0
        { A.c[1],      B.c[1],      C.c[1],      D.c[1],      Z.c[1] },  // 色1
        { A.c[2],      B.c[2],      C.c[2],      D.c[2],      Z.c[2] }   // 色2
    };
    dd x[4];

    if(!solve4x4_fast(M, x)) return {};   // ランク落ち → 解なし
    return {x[0], x[1], x[2], x[3]};
}

} // namespace mix4



namespace convex_efficient {
/*================================================================
  convex_efficient  —  K≤20, H=1000 用・4頂点凸包内判定
  ---------------------------------------------------------------
  1. build_point_buffer(oI,tI)
       oI(サイズ K) と tI(サイズ H) から座標だけ抜き取る。

  2. inside_tetra(h,a,b,c,d,buf)
       tI[h] が oI[{a,b,c,d}] の張る四面体に
       内包（面上含む）していれば true を返す。

  許容誤差 HABA (=1e-2) 内なら「両側に属する」と見なすので
  EPS 幅の ON 面点は true になります。
================================================================*/

constexpr dd HABA = 1e-6;

using Vec3 = std::array<dd,3>;

/*--------------- 基本演算 ---------------*/
inline Vec3 operator-(const Vec3& a,const Vec3& b){
    return {a[0]-b[0],a[1]-b[1],a[2]-b[2]};
}
inline Vec3 cross(const Vec3& a,const Vec3& b){
    return { a[1]*b[2]-a[2]*b[1],
             a[2]*b[0]-a[0]*b[2],
             a[0]*b[1]-a[1]*b[0] };
}
inline dd dot(const Vec3& a,const Vec3& b){
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

/*--------------- 座標バッファ ---------------*/
struct PointBuf{
    int K,H;
    std::vector<Vec3> P;   // oI 座標 (size K)
    std::vector<Vec3> T;   // tI 座標 (size H)
};

inline PointBuf build_point_buffer(const std::vector<INK>& oI,
                                   const std::vector<INK>& tI)
{
    const int K=oI.size(), H=tI.size();
    std::vector<Vec3> P(K), T(H);
    for(int i=0;i<K;i++) P[i] = {oI[i].c[0],oI[i].c[1],oI[i].c[2]};
    for(int h=0;h<H;h++) T[h] = {tI[h].c[0],tI[h].c[1],tI[h].c[2]};
    return {K,H,std::move(P),std::move(T)};
}

/*--------------- 四面体内判定 ---------------*/
inline bool inside_tetra(int h,int a,int b,int c,int d,
                         const PointBuf& buf)
{
    const Vec3& A = buf.P[a];
    const Vec3& B = buf.P[b];
    const Vec3& C = buf.P[c];
    const Vec3& D = buf.P[d];
    const Vec3& P = buf.T[h];

    auto face_ok = [&](const Vec3& v0,const Vec3& v1,
                       const Vec3& v2,const Vec3& opp)->bool{
        Vec3 n = cross(v1-v0, v2-v0);          // 法線
        if(dot(n, opp-v0) > 0)                 // 外向きに反転
            n = {-n[0],-n[1],-n[2]};
        dd side = dot(n, P-v0);
        return (side <= HABA);                 // 内側 or 面上
    };

    return  face_ok(A,B,C,D) &&
            face_ok(A,C,D,B) &&
            face_ok(A,D,B,C) &&
            face_ok(B,D,C,A);
}

} // namespace convex_efficient




struct OneAns{
    dd err;
    ll type;
    dd MainInkG;
    
    //type = 1
    ll oneadd;//1つだけ追加
    bool onemain;//trueならmainに追加、falseならperiに追加
    /*
    //type = 2
    vl adds;//periに追加する絵の具
    vl_l cuts;//絵の具[i]を、jマス分加える
     */
    
    //type = 3
    struct Operate{
        ll p;//number 0 ~ K-1
        bool reset;//windowをclose + oldをopen
        ll addg;//投入する絵の具の量 0~3くらい
        ll nxL;//L[p] に nxL を代入
    };
    vector<Operate> opes;
    
    //type = 4
    struct OpeInter{
        ll p;//number 0 ~ K-1
        ll addg;//投入する絵の具の量 0~3くらい
        ll set;//-1ならwindowをset、そうでないなら、あいだにset
        vl vs;//仕切りを置く場所
    };
    vector<OpeInter> OIs;
};

OneAns SearchOneSimple(const vl&Q,const vI&R, const vl&L, const INK&preink,const INK&target,const dd Dkei,const ll& tIindex){
    OneAns best{.err = (dd)inf};
    
    rep(i,0,K-1){
        rep(main,0,1){
            INK ink = (main == 1 ? preink : R[i]);
            
            if(main == 0){
                if((dd)R[i].g + 2 >= (dd)Q[i]-L[i])continue;
                
            }else{
                if(preink.g + 2 >= (dd)N)continue;
            }
            
            ADDInk(ink,oI[i]);
         //   dd err = CalcErr(ink,target) * 1e4;
            dd err = CalcErr(ink,target) * 1e4 + 1 * Dkei;
            if(best.err > err){
                best = {.err=err, .type=1, .oneadd=i, .onemain=(main==1)};
            }
        }
    }
    return best;
}

namespace mix_mass {
/****************************************************************
 *  mix_mass namespace  : 2 本・3 本インク最小二乗フィット
 * --------------------------------------------------------------
 *  solve_coeff_mix2_mass(A,B,Z) → {a,b}
 *  solve_coeff_mix3_mass(A,B,C,Z) → {a,b,c}
 *
 *  ・「⊕」は Mix 関数（質量加重平均）に準拠
 *  ・質量制約  aA.g + bB.g (+ cC.g) = Z.g  を厳密に満たす
 *  ・解が特異（ランク落ち）のときは空 vector を返す
 ****************************************************************/

//constexpr dd EPS = 1e-12;

/*―――――――――――――――― 3×3／4×4 拡大行列ガウス消去 ――――――――――――――――*/
template<int N>   /* N = 未知数個数（λ を含む） */
static bool gauss(dd (&A)[N][N+1], dd out[N])
{
    for(int col = 0; col < N; ++col){
        /* pivot 選択 */
        int piv = col;
        for(int r = col+1; r < N; ++r)
            if(std::fabs(A[r][col]) > std::fabs(A[piv][col])) piv = r;
        if(std::fabs(A[piv][col]) < EPS) return false;              // ランク落ち
        if(piv != col)
            for(int j = col; j <= N; ++j) std::swap(A[piv][j], A[col][j]);

        /* 正規化 */
        dd inv = 1.0 / A[col][col];
        for(int j = col; j <= N; ++j) A[col][j] *= inv;

        /* 消去 */
        for(int r = 0; r < N; ++r) if(r != col){
            dd f = A[r][col];
            for(int j = col; j <= N; ++j) A[r][j] -= f * A[col][j];
        }
    }
    for(int i = 0; i < N; ++i) out[i] = A[i][N];
    return true;
}

/*―――――――――――――――― 内積ユーティリティ（RGB×質量） ――――――――――――――――*/
static inline void massColor(const INK& I, dd v[3])
{
  //  v[0] = I.g * I.c[0];
  //  v[1] = I.g * I.c[1];
  //  v[2] = I.g * I.c[2];
    v[0] = I.c[0];
    v[1] = I.c[1];
    v[2] = I.c[2];
}
static inline dd dot3(const dd a[3], const dd b[3])
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

/*================================================================
 * 2 本インク： a·A ⊕ b·B = Z  （質量一致制約付き最小二乗）
 *===============================================================*/
inline std::vector<dd> solve_coeff_mix2_mass(const INK& A,
                                             const INK& B,
                                             const INK& Z)
{
    dd vA[3], vB[3], t[3];
    massColor(A, vA);  massColor(B, vB);  massColor(Z, t);

    /* (MᵀM)・x = Mᵀt  を Lagrange 乗数 λ で拡張 */
    dd g0 = A.g, g1 = B.g, gZ = Z.g;

    dd M[3][4] = {
        { dot3(vA,vA), dot3(vA,vB), g0, dot3(vA,t) },
        { dot3(vA,vB), dot3(vB,vB), g1, dot3(vB,t) },
        { g0,          g1,          0 , gZ          }
    };
    dd sol[3];
    if(!gauss(M, sol)) return {};           // 特異
    return { sol[0], sol[1] };              // a, b
}

/*================================================================
 * 3 本インク： a·A ⊕ b·B ⊕ c·C = Z  （質量一致制約付き最小二乗）
 *===============================================================*/
inline std::vector<dd> solve_coeff_mix3_mass(const INK& A,
                                             const INK& B,
                                             const INK& C,
                                             const INK& Z)
{
    dd vA[3], vB[3], vC[3], t[3];
    massColor(A, vA);  massColor(B, vB);  massColor(C, vC);  massColor(Z, t);

    dd g0 = A.g, g1 = B.g, g2 = C.g, gZ = Z.g;

    dd M[4][5] = {
        { dot3(vA,vA), dot3(vA,vB), dot3(vA,vC), g0, dot3(vA,t) },
        { dot3(vA,vB), dot3(vB,vB), dot3(vB,vC), g1, dot3(vB,t) },
        { dot3(vA,vC), dot3(vB,vC), dot3(vC,vC), g2, dot3(vC,t) },
        { g0,          g1,          g2,          0 , gZ          }
    };
    dd sol[4];
    if(!gauss(M, sol)) return {};
    return { sol[0], sol[1], sol[2] };      // a, b, c
}

} // namespace mix_mass

dd EvalAnsKeiT;
void SetEvalAnsKeiT(ll canuse_t){
    /*
    dd kei_t = 0;
    if(T <= 6000)kei_t = 100;
    else if(T <= 9000)kei_t = 30;
    else if(T <= 12000)kei_t = 10;
    else if(T <= 17000)kei_t = 3;
    
    EvalAnsKeiT = kei_t;
     //*/
    //*
    EvalAnsKeiT =   canuse_t < 9 ? 200:
                    canuse_t < 10 ? 100:
                    canuse_t < 11 ? 60:
                    canuse_t < 12 ? 30:
                    canuse_t < 13 ? 10:
                    canuse_t < 14 ? 5 :
                    canuse_t < 15 ? 3 :
                    canuse_t < 16 ? 1 :
                    canuse_t < 17 ? 0.3:
                    canuse_t < 25 ? 0.1: 0.01;
     //*/
}

dd EvalAns(const OneAns &ans){
    if(ans.err > inf/2){
        return ans.err;
    }
    dd kei_t = EvalAnsKeiT;
    
    ll sz = 0;
    if(ans.type == 1)sz = 2;
    else if(ans.type == 3){
        sz = 2;
        rep(j,0,(int)ans.opes.size()-1){
            sz += 2;
            if(ans.opes[j].reset)sz += 2;
        }
    }else if(ans.type == 4){
        sz = 0;
    }else{
        DEB(ans.type);
        assert(0);
    }
    dd cost_t = sz * kei_t;
    dd cost_d = 0;
    if(T >= 32000){
        if(ans.type == 3 || ans.type == 4){
            assert(ans.MainInkG >= 1-EPS);
            cost_d += (ans.MainInkG - 1) * D;
        }
    }
    return cost_t + ans.err + cost_d;
    //err : CalcErr + Dkei(tIindex終盤で加算)
    //cost_t : Tの消費量によるコスト
    //cost_d : 捨てることになるインクのコスト
};


void INKmul(const INK &a,dd kei,INK &ans){
    assert(0 <= kei && kei <= 1);
    rep(i,0,2)ans.c[i] = a.c[i] * kei;
    ans.g = a.g * kei;
}
void INKgram(const INK &a,dd gram,INK &ans){
    assert(a.g >= EPS);
    dd kei = gram / a.g;
    rep(i,0,2)ans.c[i] = a.c[i] * kei;
    ans.g = gram;
  //  return a;
}


vl DebPartialCount(5);
ll DebInter = 0;
dd DebFastInterTime = 0;

OneAns SearchPartialReset(const vl &preQ,const vI &preR,const vl &preL,const INK&preInk,const INK&target,const ll p_size,const dd &Dkei,ll tIindex,convex_efficient::PointBuf& buf,dd TimeLimit){
    OneAns best{.err = (dd)inf,.type=1};
    vvl ps;
    if(p_size == 2)rep(p0,0,K-1)rep(p1,p0+1,K-1)ps.pb({p0,p1});
    else if(p_size == 3)rep(p0,0,K-1)rep(p1,p0+1,K-1)rep(p2,p1+1,K-1)ps.pb({p0,p1,p2});
    else if(p_size == 4)rep(p0,0,K-1)rep(p1,p0+1,K-1)rep(p2,p1+1,K-1)rep(p3,p2+1,K-1){
        if(convex_efficient::inside_tetra(tIindex,p0,p1,p2,p3,buf)){
                ps.pb({p0,p1,p2,p3});
        }
    }else assert(0);

    vI nowR(K);
    INK nowInk;
    INK toPeri;
    vI wells(p_size);
    INK ink;
    INK addInk;
    
    INK SubTarNow;
    vl l(p_size);
    vd ss(p_size);
    
    
    auto PartialReset=[&](const vl &p,const vl &re,const vvl &as)__attribute__((noinline)){
        ll sz = p.size();assert(sz == p_size && p.size() == re.size());
        
        //nowR = preR;
        rep(j,0,sz-1){
            nowR[p[j]] = preR[p[j]];
        }
        vl nowL = preL;
        nowInk = preInk;
        
        {
            ll menseki = N;
            rep(j,0,K-1)menseki += preL[j];
            rep(j,0,sz-1){
                if(re[j]){
                 //   toPeri = {nowInk.c,nowInk.g * (dd)(preL[p[j]]) / menseki};
                    //toPeri = INKmul(nowInk,(dd)(preL[p[j]]) / menseki);
                    INKmul(nowInk,(dd)(preL[p[j]]) / menseki, toPeri);
                    menseki -= preL[p[j]];
                //    nowInk.g -= toPeri.g;
                    SUBInk(nowInk,toPeri);
                    ADDInk(nowR[p[j]], toPeri);
                    nowL[p[j]] = 0;
                }
            }
        }
        vl nowQ(K);
        rep(i,0,K-1)nowQ[i] = preQ[i] - nowL[i];
        for(const vl&a:as){
            rep(j,0,sz-1){
                wells[j] = nowR[p[j]];
                rep(kai,1,a[j]){
                    ADDInk(wells[j],oI[p[j]]);
                }
            }
            bool ok = true;
            rep(j,0,sz-1){
                if(wells[j].g > (dd)2)ok = false;
                if(wells[j].g > (dd)nowQ[p[j]])ok=false;
                
            }
            if(!ok)continue;
            
            vd kei;
            if(p_size == 2){
                INKsub(target,nowInk,SubTarNow);
                kei = mix_mass::solve_coeff_mix2_mass(wells[0],wells[1],SubTarNow);
            }else if(p_size == 3){
                INKsub(target,nowInk,SubTarNow);
                kei = mix_mass::solve_coeff_mix3_mass(wells[0],wells[1],wells[2],SubTarNow);
            }else if(p_size == 4){
                INKsub(target,nowInk,SubTarNow);
                kei = mix4::solve_coeff_mix4(wells[0],wells[1],wells[2],wells[3],SubTarNow);
            }else{
                assert(0);
            }

            if(kei.empty())continue;
            assert((int)kei.size() == sz);
            //DEB(kei);
            
            rep(j,0,sz-1){
                if(kei[j] < -0.2 || kei[j] > 1.2)ok=false;
                chmax(kei[j],0);
                chmin(kei[j],1);
            }
            if(!ok)continue;
           
            
          //  vd ss(sz);
            rep(j,0,sz-1)ss[j] = (dd)nowQ[p[j]] * kei[j];
            
            /*
            {
                cerr<<endl;
                DEB(kei);
                DEB(ss);
                cerr<<"Q[p[j]] ";rep(j,0,sz-1)cerr<<Q[p[j]]<<" ";
            }*/
            
            rep(mbit,0,(1<<sz)-1){
                bool OK = true;
                
                rep(j,0,sz-1){
                    
                    if(j == 0){
                        l[j] = (ss[j] + 1 - EPS)  + ((mbit >> j)&1);
                    }else{
                        if(ss[0] > EPS){
                            l[j] = llround(l[0] * ss[j] / ss[0]) + ((mbit >> j) & 1);
                        }else{
                            l[j] = llround(ss[j]) + ((mbit >> j) & 1);
                        }
                    }
                    if(l[j] > nowQ[p[j]])OK = false;
                }
                
                if(!OK && Dkei < EPS)continue;
                if(Dkei >= EPS){
                    rep(j,0,sz-1){
                        chmax(l[j],0);
                        chmin(l[j],nowQ[p[j]]);
                    }
                }
                
                
                ink = nowInk;
                rep(j,0,sz-1){
                  //  dd gram = wells[j].g * l[j] / nowQ[p[j]];
                  //  ADDInk(ink,{wells[j].c, gram});
                  //  addInk = INKmul(wells[j],(dd) l[j] / nowQ[p[j]]);
                    INKmul(wells[j],(dd) l[j] / nowQ[p[j]],  addInk);
                    ADDInk(ink,addInk);
                }
                
                if(ink.g < 1)continue;
                if(ink.g > 1.5 && Dkei < EPS)continue;
                if(ink.g > 2 - EPS)continue;
                // dd err = CalcErr(ink,target) * 1e4 + (ink.g - 1) * D;
                //dd err = CalcErr(ink,target) * 1e4;
                
                ll Addcount = 0;rep(j,0,sz-1)Addcount+=a[j];
                dd err = CalcErr(ink,target) * 1e4 + Addcount * Dkei;
                
                
                dd EvalAnsKouho = 0;
                {
                    EvalAnsKouho += err;
                    ll v_sz = 2;
                    rep(j,0,p_size-1){
                        v_sz += 2;
                        if(re[j])v_sz += 2;
                    }
                    EvalAnsKouho += v_sz * EvalAnsKeiT;
                    if(T >= 32000)EvalAnsKouho += (ink.g - 1) * D;
                }
               
                
                if(EvalAns(best) > EvalAnsKouho){
                    vector<OneAns::Operate> opes;
                    rep(j,0,sz-1){
                        ll num = p[j];
                        ll add = (a[j] == 1);
                        ll nxL = l[j] + nowL[p[j]];
                        
                        assert(nxL <= preQ[p[j]]);
                        opes.pb(OneAns::Operate{.p=num, .reset=(re[j] == 1), .addg=add, .nxL=nxL});
                    }
                    OneAns kouho{.err = err, .type = 3, .MainInkG = ink.g, .opes = opes};
                    
                    /*
                    assert(abs(EvalAnsKouho - EvalAns(kouho)) < EPS);
                    if(abs(EvalAnsKouho - EvalAns(kouho)) > EPS){
                        DEB(EvalAnsKouho);
                        DEB(EvalAns(kouho));
                        DEB(err);
                        DEB(ink.g);
                        DEB(D);
                        rep(j,0,sz-1)cerr<<re[j]<<" ";cerr<<endl;
                    }
                     */
                    
                    best = kouho;
                }
            }
        }
    };
    
    shuffle(ps);
  //  while((int)ps.size() >= 100)ps.pop_back();
    
    ll LoopCountPartialReset = 0;
    for(const vl &p:ps){
        if(stop_watch() > TimeLimit){
          //  cerr<<"LoopCountPartialReset "<<LoopCountPartialReset<<endl;
            break;
        }
        LoopCountPartialReset++;
        ll sz = p.size();
        if(K < 15 || T > 18000){
            vvl as;
            {
                dd limit = (p_size == 3 ? 0.5 : 0.3);
                if(p_size == 2 || K <= 20){
                    rep(abit,0,(1<<sz)-1){
                        vl a(sz);
                        rep(j,0,sz-1)a[j] = (abit >> j) & 1;
                        as.pb(a);
                    }
                }else if(K <= 14){
                    assert(3 <= p_size && p_size <= 4);
                    rep(abit,0,(1<<sz)-1){
                        bool ok = true;
                        vl a(sz);
                        rep(j,0,sz-1){
                            a[j] = (abit >> j) & 1;
                            if((a[j] == 1) && (preR[p[j]].g > limit))ok=false;
                        }
                        if(ok)as.pb(a);
                    }
                }else{
                    assert(3 <= p_size && p_size <= 4);
                    vl a(sz);
                    rep(j,0,sz-1){
                        a[j] = (preR[p[j]].g < limit);
                    }
                    as.pb(a);
                }
            }
            vl re(sz,1);//All Reset
            PartialReset(p,re,as);
        }
        if(K < 15 || T <= 18000){
            vvl reS;//TODO  あとで調節する
            if(tIindex == 0){
                rep(rbit,0,0){
                    vl re;
                    rep(j,0,sz-1)re.pb((rbit>>j)&1);
                    reS.pb(re);
                }
            }else{
                if(p_size == 2 || K <= 20){
                    rep(rbit,0,(1<<sz)-1){
                        vl re;
                        rep(j,0,sz-1)re.pb((rbit>>j)&1);
                        reS.pb(re);
                    }
                }else{
                    dd limit = (p_size == 3 ? 0.5 : 0.3);
                    vl re(sz);
                    rep(j,0,sz-1){
                        if(preR[p[j]].g < limit)re[j] = 1;
                    }
                }
            }
            for(vl re:reS){
                vvl as;
                if(K <= 10){
                    rep(I,0,(1<<sz)-1){
                        vl a(sz);
                        rep(j,0,sz-1){
                            a[j] = (I>>j)&1;
                        }
                        as.pb(a);
                    }
                }else{
                    //なんだこれ...
                    vl a(sz);
                    rep(j,0,sz-1){
                        if(re[j])a[j] = 1;
                        as.pb(a);
                    }
                }
                PartialReset(p,re,as);
            }
        }
        
    }
   // assert(best.err < inf/2);
    
    DebPartialCount[p_size] += LoopCountPartialReset;
    return best;
}


OneAns SearchAllUse(const vl &Q,const vI&R,const vl &L,const INK&preink,const INK&target,const dd Dkei){
    OneAns best{.err = (dd)inf, .type=1};
    {
        dd sum = 0;
        for(INK x:R)sum += x.g;
      //  if(sum <= 1 - EPS) return best;
    }
    
    
    rep(i,0,(1<<K)-1){
        INK ink = preink;
        rep(j,0,K-1){
            if((i>>j)&1){
                ADDInk(ink,R[j]);
            }
        }
     //   DEB(i);
     //   DEB(ink.g);
        if(ink.g <= 1){
            vl tuikas;
            
            if(i == 0)continue;
            rep(j,0,K-1){
                if((i>>j)&1){
                    if(L[j] +1 < Q[j]){//L[j]+1< Q[j]とかかな
                        tuikas.pb(j);
                    }
                }
            }
            if(tuikas.empty())continue;
            if(K >= 16)tuikas.resize(1);
            
            for(ll tui:tuikas){
                ll DebAddCnt = 0;
                INK Ink = Mix(ink,oI[tui]);
                dd err = CalcErr(Ink,target) * 1e4 + 1 * Dkei;
                if(best.err > err){
                    vector<OneAns::Operate> opes;
                    rep(j,0,K-1){
                        if((i>>j)&1){
                            opes.pb(OneAns::Operate{.p=j, .reset=false, .addg=(j == tui), .nxL=Q[j]});
                            if(j == tui)DebAddCnt++;
                        }
                    }
                    OneAns kouho{.err=err, .type=3, .MainInkG=1 ,.opes=opes};
                    best = kouho;
                    assert(DebAddCnt == 1);
                }
            }
        }else{
            dd err = CalcErr(ink,target) * 1e4 + 0 * Dkei;
            if(best.err > err){
                vector<OneAns::Operate> opes;
                rep(j,0,K-1){
                    if((i>>j)&1){
                        opes.pb(OneAns::Operate{.p=j, .reset=false, .addg=0, .nxL=Q[j]});
                    }
                }
                OneAns kouho{.err=err, .type=3, .MainInkG=1 ,.opes=opes};
                best = kouho;
            }
        }
    }
    
    return best;
}

namespace SimulateBan{

/********************************************************************
 *  SimulateBan
 *  ────────────────────────────────────────────────────────────────
 *  ‣ initialize()   : 盤面とインク配列の初期化
 *  ‣ connected_ink(): 与えたマスと同じウェルにあるインク量と平均色を返す
 *  ‣ simulate()     : 1 手 (Man) を愚直に反映する
 *
 *  依存関係
 *    ─ 事前に以下が定義されていること
 *        ll              : typedef long long
 *        const ll N      : パレットの一辺 (この問題では 20)
 *        vector<Ink> oI  : チューブ絵の具 (K 要素)
 *
 *  メモ
 *    ‣ 各ウェル内のインクは「代表マス (cells[0]) に集中」させる
 *      └ 容量チェックはウェルのマス数で行うので仕様を満たす
 *    ‣ 分割時は容量比で質量だけ分配 (色は同一なのでそのまま)
 *******************************************************************/

struct Ink{
    vd c = vd(3,0);
    dd g;
};
INK InkToINK(const Ink &a){
    INK ret = {a.c,a.g};
    rep(i,0,2)ret.c[i] *= ret.g;
    return ret;
}

struct SimulateBan {
    Ban ban;                           // 仕切り状態
    std::vector<std::vector<Ink>> inks; // 各マスのインク
    
    /* 初期化 ---------------------------------------------------- */
    void initialize(const Ban& ban_) {
        ban  = ban_;
        inks.assign(N, std::vector<Ink>(N)); // 全マス g=0, c=(0,0,0)
    }
    
    //---------------------------
    bool isOpen(l_l a,l_l b){
        if(a > b)swap(a,b);
        if(a.fi == b.fi){
            assert(a.se +1 == b.se);
            assert(0 <= a.fi && a.fi <= N-1 && 0 <= a.se && a.se <= N-2);
            return ban.v[a.fi][a.se] == 0;
        }else{
            assert(a.se == b.se && a.fi+1 == b.fi);
            assert(0 <= a.fi && a.fi <= N-2 && 0 <= a.se && a.se <= N-1);
            return ban.h[a.fi][a.se] == 0;
        }
    }
    
    /* 4 近傍 BFS で同一ウェルのマス列挙 ------------------------- */
    std::vector<std::pair<ll,ll>> get_well_cells(std::pair<ll,ll> st) const {
        static const int di[4] = {-1,1,0,0};
        static const int dj[4] = {0,0,-1,1};
        
        std::vector<std::pair<ll,ll>> cells;
        std::vector<std::vector<char>> vis(N, std::vector<char>(N, 0));
        std::queue<std::pair<ll,ll>> q; q.push(st); vis[st.first][st.second] = 1;
        
        while(!q.empty()) {
            auto [i,j] = q.front(); q.pop();
            cells.push_back({i,j});
            for(int d=0; d<4; ++d){
                ll ni = i+di[d], nj = j+dj[d];
                if(ni<0 || ni>=N || nj<0 || nj>=N || vis[ni][nj]) continue;
                
                bool connect = false;
                if(d==0 && ban.h[i-1][j]==0)           connect = true;     // up
                if(d==1 && ban.h[i][j]==0)             connect = true;     // down
                if(d==2 && ban.v[i][j-1]==0)           connect = true;     // left
                if(d==3 && ban.v[i][j]==0)             connect = true;     // right
                if(connect){ vis[ni][nj]=1; q.push({ni,nj}); }
            }
        }
        return cells;
    }
    
    /* ウェル全体のインク（量 g と平均色 c）を取得 -------------- */
    Ink connected_ink(std::pair<ll,ll> pos){
        auto cells = get_well_cells(pos);
        Ink ret; double sumg = 0, sumc[3]={0,0,0};
        for(auto [i,j]: cells){
            const Ink& ik = inks[i][j];
            sumg         += ik.g;
            sumc[0]      += ik.c[0]*ik.g;
            sumc[1]      += ik.c[1]*ik.g;
            sumc[2]      += ik.c[2]*ik.g;
        }
        ret.g = sumg;
        if(sumg > 1e-12){
            ret.c = { sumc[0]/sumg, sumc[1]/sumg, sumc[2]/sumg };
        }
        return ret;
    }
    INK connected_INK(l_l pos){
        Ink c = connected_ink(pos);
        return InkToINK(c);
    }
    
    /* 指定ウェルのインクを「代表マス 1 つ」にまとめて上書き ------ */
    void set_well(const std::vector<std::pair<ll,ll>>& cells, const Ink& ink){
        for(auto [i,j]: cells) inks[i][j] = Ink();      // 全クリア
        if(!cells.empty()) inks[cells[0].first][cells[0].second] = ink;
    }
    
    /* 1 手シミュレーション ------------------------------------- */
    void simulate(const Man& man){
        /********** 1) チューブから 1 g 追加 ************************/
        if(man.type == 1){
            ll i = man.i1, j = man.j1, k = man.i2;      // i2==k
            auto cells      = get_well_cells({i,j});
            ll   capacity   = (ll)cells.size();
            Ink  well       = connected_ink({i,j});
            double add_g    = std::min(1.0, std::max(0.0, (double)capacity - well.g));
            if(add_g <= 0) return;                      // 満杯
            INK tube_INK = oI[k];
            Ink tube = {tube_INK.c,tube_INK.g};//oI[]は重さ1なのでInkとINKが完全に一致する
            Ink merged;
            merged.g = well.g + add_g;
            for(int c=0;c<3;++c){
                merged.c[c] = (well.c[c]*well.g + tube.c[c]*add_g)/merged.g;
            }
            set_well(cells, merged);
            return;
        }
        
        /********** 2) 画伯に 1 g 渡す *****************************/
        if(man.type == 2){
            auto cells = get_well_cells({man.i1, man.j1});
            Ink well   = connected_ink({man.i1, man.j1});
            //  DEB(well.c);
            if(well.g < 1.0 - 1e-6) return;             // 1 g 未満
            double take = (well.g >= 1.0 ? 1.0 : well.g);
            well.g -= take;
            if(well.g < 1e-9) well = Ink();             // 空になったら色リセット
            set_well(cells, well);
            return;
        }
        
        /********** 3) 1 g 廃棄 ***********************************/
        if(man.type == 3){
            auto cells = get_well_cells({man.i1, man.j1});
            Ink well   = connected_ink({man.i1, man.j1});
            if(well.g < 1e-9) return;                   // そもそも空
            double discard = (well.g >= 1.0 ? 1.0 : well.g);
            well.g -= discard;
            if(well.g < 1e-9) well = Ink();
            set_well(cells, well);
            return;
        }
        
        /********** 4) 仕切りの出し入れ ***************************/
        if(man.type == 4){
            ll i1=man.i1,j1=man.j1,i2=man.i2,j2=man.j2;
            bool horizontal = (i1 == i2);
            /* --- 変更前に状態を取っておく --- */
            auto pre_cells = get_well_cells({i1,j1});
            Ink  pre_ink   = connected_ink({i1,j1});
            
            /* --- 仕切りをトグル --- */
            bool opened_before;
            if(horizontal){
                opened_before = (ban.v[i1][std::min(j1,j2)] == 0);
                ban.v[i1][std::min(j1,j2)] ^= 1;
            }else{
                opened_before = (ban.h[std::min(i1,i2)][j1] == 0);
                ban.h[std::min(i1,i2)][j1] ^= 1;
            }
            
            /* --- 仕切りを出した（分割した）場合 ---------------- */
            if(opened_before){
                auto cells1 = get_well_cells({i1,j1});
                auto cells2 = get_well_cells({i2,j2});
                ll s1 = (ll)cells1.size(), s2 = (ll)cells2.size(), tot = s1+s2;
                if(tot==0) return;
                Ink ink1 = pre_ink, ink2 = pre_ink;     // 色そのまま
                ink1.g = pre_ink.g * ((double)s1 / tot);
                ink2.g = pre_ink.g - ink1.g;
                if(ink1.g < 1e-12) ink1 = Ink();
                if(ink2.g < 1e-12) ink2 = Ink();
                set_well(cells1, ink1);
                set_well(cells2, ink2);
            }
            /* --- 仕切りを引っ込めた（合併した）場合 ------------ */
            else{
                auto cells_after = get_well_cells({i1,j1});      // 既に合併後
                Ink ink1 = connected_ink({i1,j1});               // 新しい全体
                set_well(cells_after, ink1);
            }
        }
    }
};
}//namespace SimulateBan




bool boolTrySearchOneArray = false;
vl FastInter(ll Q,ll L,ll tlim,dd goal,dd &ret_best,dd &initialize_time){
    struct Dat{
        dd sum;
        vector<int8_t> v;
        const bool operator<(const Dat &op)const{
            return sum < op.sum;
        };
    };
    typedef vector<Dat> vD;
    typedef vector<vD> vvD;
    typedef vector<vvD> vvvD;
    typedef vector<vvvD> vvvvD;
    const ll tlim_max = 10;
    static vvl searchd(101,vl(101));
    static vvvvD results(101,vvvD(101,vvD(tlim_max + 1)));
    //DEB(goal);
    if(searchd[Q][L]){
       // initialize_time = 0;これ書いてはダメ
        
        ll btsz_ = 0;
        ll bpos_ = 0;
        dd bestdif = inf;
        rep(tsz,0,tlim){
            Dat kijun = {goal,{}};
            ll posst = POSL(results[Q][L][tsz],kijun);
            rep(pos,posst,posst+4){
                if(pos <= (int)results[Q][L][tsz].size()-1){
                    dd dif = results[Q][L][tsz][pos].sum - goal;
                    assert(dif >= 0);
                    auto Eval=[&](const vector<int8_t> &v,dd dif){
                      //  if(v.empty())return dif;
                        ll hazi = 0;
                        if((int)v.size() >= 2)hazi = min(v[v.size()-2] , Q - v[v.size()-2]);
                        if(hazi <= Q / 5){
                            if(hazi == 0)return dif + 0.02;
                            return dif + 0.01 / hazi;
                        }else{
                            return dif;
                        }
                    };
                  //  if(bestdif > dif){
                 //   if(Eval(bestv,bestdif) > Eval(results[Q][L][tsz][pos].v,dif)){
                    if(bestdif > inf/2 || Eval(results[Q][L][btsz_][bpos_].v,bestdif) > Eval(results[Q][L][tsz][pos].v,dif)){
                        bestdif = dif;
                      //  bestv = results[Q][L][tsz][pos].v;
                      //  assert((int)bestv.size() == tsz);
                        btsz_ = tsz;
                        bpos_ = pos;
                        
                    }
                }
            }
           
        }
        /*
        if(bestdif > inf/2){
            cerr<<"Fast Inter みつからず"<<endl;
            DEB(Q);
            DEB(L);
            DEB(tlim);
            DEB(goal);
        }*/
     //   assert(bestdif != inf);//見つからないときもあるのかなあ
        ret_best = (goal + bestdif);
        if(bestdif > inf/2){
            vector<int8_t> bestv;
            return vl(bestv.begin(), bestv.end());
        }else{
            vector<int8_t> bestv = results[Q][L][btsz_][bpos_].v;
            return vl(bestv.begin(), bestv.end());
        }
      //  return bestv;
    }
    initialize_time = - stop_watch();
    {
        assert(0 < L && L < Q);
        assert(0 <= goal && goal <= Q-L);
        auto Mul=[&](const d_l &x,dd bai){
            d_l y = x;
            y.fi *= bai;
            y.se = llround(y.se * bai);
            return y;
        };
        auto Sub=[&](d_l x,const d_l &y){
            x.fi -= y.fi;
            x.se -= y.se;
            return x;
        };
        auto Mix=[&](d_l a,d_l b){
            d_l c;
            c.fi = a.fi+b.fi;
            c.se = a.se+b.se;
           // c.fi = a.fi*a.se + b.fi*b.se;
            return c;
        };
        
        auto challenge=[&]()__attribute__((noinline)){
            ll f1 = L;
            ll f2;// 0 < f < Q
            vector<int8_t> v;
            auto Check=[&](d_l lf,d_l ri)__attribute__((noinline)){
                //もっと工夫できる
                dd sum = 0;
                auto check=[&](d_l add,ll len)__attribute__((noinline)){
                    rep(_,1,add.se){
                        sum += add.fi / add.se;
                        
                      //  vl goodv = v;
                        ll f3 = len + _;
                        if(f3 == f1){
                            //goodvに追加はしない
                        }else{
                         //   goodv.pb(f3);
                         //   goodv.pb(f1);
                            v.pb(f3);v.pb(f1);
                        }
                       // if(2 <= f3 && f3 <= Q - 2){ //端を除いた
                        if(0 <= f3 && f3 <= Q - 0){ //端を除いた
                            if((int)v.size() <= tlim_max){
                                results[Q][L][v.size()].pb({sum,v});
                            }
                        }
                        if(f3 != f1){
                            v.pop_back();v.pop_back();//f1,f3
                        }
                    }
                };
                check(lf,0);
                check(ri,lf.se);
            };
            
            d_l lf,ri,mid;//(色素量、長さ)
            lf = {0,L};
            ri = {Q-L,Q-L};
            
            rep(_,1,tlim_max/2){
                if(results[Q][L][2].empty() && _ == 1){
                    Check(lf,ri);
                }
                f2 = f1;
                while(f2 == f1)f2 = rand(1,Q-1);
                v.pb(f2);//新しい仕切りを追加
                if(f1 < f2){
                    mid = Mul(ri,(dd)(f2-f1) / ri.se);
                    ri =  Sub(ri,mid);
                    assert(ri.se >= 1);
                }else{
                    assert(f1 - f2 >= 1);
                    mid = Mul(lf,(dd)(f1-f2) / lf.se);
                    lf =  Sub(lf,mid);
                    assert(lf.se >= 1);
                }

                v.pb(f1);//古い仕切りを取り除く
                if(f1 < f2){
                    lf = Mix(lf,mid);
                    mid = {0,0};
                }else{
                    ri = Mix(ri,mid);
                    mid = {0,0};
                }
                f1 = f2;//仕切りの番号を古いものから新しいものに変更
                Check(lf,ri);
            }
        };
        
        ll Turn_max = 300 * powl((dd)90.0 / Q , 1.0);
      //  ll Turn_max = 300;
        if(boolTrySearchOneArray)Turn_max = 100;
        
        rep(turn,1,Turn_max){
            challenge();
        }
        rep(tsz,0,tlim_max){
            SORT(results[Q][L][tsz]);
            /*
            const vD &vec = results[Q][L][tsz];
            vD unique;
            rep(i,0,(int)vec.size()-1){
                if(i == 0 || abs(vec[i].sum - vec[i-1].sum) < 1e-4){
                    unique.pb(vec[i]);
                }
            }
            swap(results[Q][L][tsz],unique);
             */
        }
        /*
        rep(tsz,0,10){
            DEB(tsz);
            if(results[Q][L][tsz].empty())continue;
            auto x = results[Q][L][tsz].back();
            DEB(x.sum);
            DEB(x.v);
        }*/
        results[Q][L][1].pb({(dd)(Q-L),{(int8_t)(L)}});
        searchd[Q][L] = 1;
    }
    initialize_time += stop_watch();
    DebFastInterTime += initialize_time;
    return FastInter(Q,L,tlim,goal,ret_best,initialize_time);
}

OneAns SearchInter(const vl &preQ,const vI&preR,const vl&preL,const INK& preInk,const INK&target,const dd Dkei,const ll canuse_t,dd TimeLimit){
    OneAns best{.err = (dd)inf,.type=1};
    ll p_size = 4;
    static vvl ps;
    if(ps.empty()){
        rep(p0,0,K-1)rep(p1,p0+1,K-1)rep(p2,p1+1,K-1)rep(p3,p2+1,K-1)ps.pb({p0,p1,p2,p3});
    }
    
    shuffle(ps);
   // while((int)ps.size() >= 500)ps.pop_back();
    
    vvl as;
    rep(abit,0,(1<<p_size)-1){
        vl a;rep(j,0,p_size-1)a.pb((abit >> j)&1);
        as.pb(a);
    }
    
    vI wells(p_size);
    vvl Inters(p_size);
    ll LoopCountSearchInter = 0;
    
    INK addInk;
    
    
    vl set(p_size,-1);
    vl nowQ(K);
    
    INK SubTarNow;
    
    rep(pnum,0,(int)ps.size()-1){
        const vl &p = ps[pnum];
        if(stop_watch() > TimeLimit){
          //  cerr<<"LoopCountSearchInter "<<LoopCountSearchInter<<endl;
            break;
        }
        LoopCountSearchInter++;
       // DEB(p);
        ll sz = p.size();
        const vI &nowR = preR;
        vl nowL = preL;
        INK nowInk = preInk;
        rep(j,0,sz-1)set[j] = -1;
        {
            bool OK = true;
            rep(j,0,sz-1){
                if(nowL[p[j]] == preQ[p[j]])OK=false;
                else if(nowL[p[j]] == 0){
                    if(nowR[p[j]].g < EPS)set[j] = preQ[p[j]] / 2;
                    else OK = false;
                }
              //  if(nowR[p[j]].g < 0.2)a[j] = 1;
              //  if(K >= 10 && nowR[p[j]].g < 0.2 && a[j] == 0)OK = false;
            }
            if(!OK)continue;
        }
        {
            ll menseki = N;
            rep(j,0,K-1)menseki += nowL[j];
            ll sub_menseki = 0;
            rep(j,0,sz-1){
                if(set[j] != -1){
                    assert(nowR[p[j]].g < 2 * EPS);
                    nowL[p[j]] = set[j];
                }else{
                    //本当はpreInkがperiに移動するが、preInk.g が0に近いということで...
                    //この調整まで含むと、Inter関数では厳密には解けていない
                    //preInkが大きい時(あるいは、Kが大きく、かつ、前回がSearchInterではない時)にはdisposeするのもアリだと思う
                    sub_menseki += nowL[p[j]];
                }
            }
            assert(sub_menseki <= menseki);
          //  nowInk.g *= (dd)(menseki - sub_menseki) /menseki;
          //  nowInk = INKmul(nowInk,(dd)(menseki - sub_menseki) /menseki);
            INKmul(nowInk,(dd)(menseki - sub_menseki) /menseki,  nowInk);
        }
        rep(j,0,sz-1)nowQ[j] = preQ[p[j]] - nowL[p[j]];
            
        for(const vl &a:as){
            rep(j,0,sz-1){
                wells[j] = nowR[p[j]];
                rep(_,1,a[j])ADDInk(wells[j],oI[p[j]]);
            }
            {
                bool ok = true;
                rep(j,0,sz-1){
                    if(wells[j].g > (dd)2)ok = false;
                    if(wells[j].g > (dd)nowQ[j])ok=false;
                }
                if(!ok)continue;
            }
           /*
            {
                cerr<<" -------------- DEBUG PERI ";
                rep(j,0,sz-1){
                    cerr<<nowR[p[j]].g<<" ";
                }cerr<<endl;
            }
                */
            
            INKsub(target,nowInk,SubTarNow);
            vd kei = mix4::solve_coeff_mix4(wells[0],wells[1],wells[2],wells[3],SubTarNow);
            if(kei.empty())continue;
            assert((int)kei.size() == sz);
            
             //   DEB(kei);
            {
                bool ok = true;
                rep(j,0,sz-1){
                  //  if(kei[j] < -0.2 || kei[j] > 1.2)ok=false;
                 //   chmax(kei[j],0);
                 //   chmin(kei[j],1);
                   // if(K >= 10 && kei[j] < 0.05)ok=false;
                    if(kei[j] < 0 || kei[j] > 1)ok=false;
                 //   if(kei[j] > 0.9)ok = false;//空になると次が困る(例外処理書けば不可能ではない）
                }
            //    DEB(ok);
                if(!ok)continue;
            }
            
            vd IntersBest(sz);
            vd IntersGram(sz);
            rep(j,0,sz-1){
                //ll tlim = 10;
                //ll tlim = min(10,canuse_t / 8);
                ll tlim = min(10,(canuse_t-10) / 4);
            //    Inters[j] = Inter(preQ[p[j]],nowL[p[j]],tlim,(preQ[p[j]] - nowL[p[j]]) * kei[j], IntersBest[j]);
                dd initialize_time = 0;
                Inters[j] = FastInter(preQ[p[j]],nowL[p[j]],tlim,(preQ[p[j]] - nowL[p[j]]) * kei[j], IntersBest[j],initialize_time);
                TimeLimit += initialize_time;
                IntersGram[j] = (dd)IntersBest[j] /(preQ[p[j]] - nowL[p[j]]) * wells[j].g;
            }
            /*
            DEB(Inters);
            DEB(IntersBest);
            vd Goal(sz);rep(j,0,sz-1)Goal[j] = (preQ[p[j]] - nowL[p[j]]) * kei[j];
            DEB(Goal);
            DEB(IntersGram);
            {
                cerr<<"  ------------------DEBUG ";
                dd sum = nowInk.g;cerr<<sum<<" ";
                rep(j,0,sz-1){
                    sum += IntersGram[j];
                    cerr<<sum<<" ";
                }cerr<<endl;
            }
            //*/
                    
            INK ink = nowInk;
            rep(j,0,sz-1){
             //   dd gram = IntersGram[j];
             //   ADDInk(ink,{wells[j].c, gram});
                assert(IntersGram[j] <= wells[j].g);
             //   addInk = INKmul(wells[j], (dd)(IntersGram[j]) /wells[j].g);
             //   addInk = INKgram(wells[j],IntersGram[j]);
                INKgram(wells[j],IntersGram[j], addInk);
                ADDInk(ink,addInk);
            }
           // DEB(ink.g);
            if(ink.g < 1){
              //  cerr<<"[DATA] deb = notFoundInter"<<endl;
                return {};
            }
            assert(ink.g >= 1);
            if(ink.g < 1)continue;
            if(ink.g > 1.5)continue;
            // dd err = CalcErr(ink,target) * 1e4 + (ink.g - 1) * D;
            //dd err = CalcErr(ink,target) * 1e4;
            
            ll Addcount = 0;rep(j,0,sz-1)if(a[j])Addcount++;
            dd err = CalcErr(ink,target) * 1e4 + Addcount * Dkei;
         //   cerr<<err<<"   ";
         //   DEB(IntersBest);
            
            dd EvalAnsKouho = 0;{
                EvalAnsKouho = err;
                if(T >= 32000)EvalAnsKouho += (ink.g - 1) * D;
            }
            
            if(EvalAns(best) > EvalAnsKouho){
                vector<OneAns::OpeInter> OIs;
                rep(j,0,sz-1){
                    ll num = p[j];
                    ll add = (a[j] == 1);
                 //   ll nxL = l[j] + nowL[p[j]];
                    
                //    assert(nxL <= preQ[p[j]]);
                //    opes.pb(OneAns::Operate{.p=num, .reset=(re[j] == 1), .addg=add, .nxL=nxL});
                    OIs.pb(OneAns::OpeInter{.p=num,.addg=add,.set=set[j], .vs=Inters[j]});
                }
                OneAns kouho{.err = err, .type =4, .MainInkG = ink.g, .OIs = OIs};
              //  if(EvalAns(best) > EvalAns(kouho)){
                    best = kouho;
              //  }
                
               // assert(abs(EvalAnsKouho - EvalAns(kouho)) < EPS);
                
                bool breakFlag = false;
                if(T <= 32000){
                    if(err < 1)breakFlag = true;
                }else{
                   // if(err < 0.5)breakFlag = true;
                    if(err < 0.1)breakFlag = true;
                    if(pnum >= 100 && err < 0.2)breakFlag = true;
                    if(pnum >= 500 && err < 0.3)breakFlag = true;
                    if(pnum >= 2000 && err < 0.5)breakFlag = true;
                }
                
                if(breakFlag){
                 //   cerr<<"LoopCountSearchInter Success "<<LoopCountSearchInter<<endl;
                    DebInter += LoopCountSearchInter;
                    return best;//(p,a)1通りでできるかな？っていう実験でもある。本番はいくつかためしてもよい
                }
            }
        }
    }
    
    DebInter += LoopCountSearchInter;
    return best;
}

dd interpolateY(const vector<pair<dd, dd>> &v, dd X) {
    auto it = lower_bound(v.begin(), v.end(), make_pair(X, -1e100),
        [](const pair<dd, dd> &a, const pair<dd, dd> &b) {
            return a.first < b.first;
        });

    if (it == v.begin()) return it->second;
    if (it == v.end()) return prev(it)->second;

    auto [x1, y1] = *prev(it);
    auto [x2, y2] = *it;

    dd t = (X - x1) / (x2 - x1);
    return y1 + t * (y2 - y1);
}

pair<Ban,vM>BYsolve_3_4(){
    vl Q(K);//Q[i]:= 絵の具oI[i]が占めるマス、つまりperi[i] の数
    vl L(K);//L[i]:= 絵の具oI[i]の仕切りの位置 0 ~ Q[i]
    vvl_l idxs(K);
    vector<pair<l_l,l_l>>windows(K);
    l_l Root;
    Ban initban;
    InitBan(initban,Q,idxs,windows,Root);
    
    
    vI R(K);//R[i]:= peri[i] の色、量
    vM mans;
    INK preink;
    SimulateBan::SimulateBan Sim;
    Sim.initialize(initban);
    auto PushMan=[&](Man man){
        mans.pb(man);
        Sim.simulate(man);
    };
    
    convex_efficient::PointBuf bufConvex4 = convex_efficient::build_point_buffer(oI, tI);
    
    auto ManWindowChange=[&](ll p){
        pair<l_l,l_l> wi = windows[p];
        PushMan({4,wi.fi.fi,wi.fi.se,wi.se.fi,wi.se.se});
    };
    auto ManDivChange=[&](ll p,ll Lp){
        if(Lp == 0){
            ManWindowChange(p);
            return;
        }
        assert(0 < Lp && Lp <= Q[p]);
        if(Lp == Q[p])return;
        l_l cp1 = idxs[p][Lp-1];
        l_l cp2 = idxs[p][Lp];
        PushMan({4,cp1.fi,cp1.se,cp2.fi,cp2.se});
    };
    auto ManAddInk=[&](ll p){
        l_l axend = idxs[p].back();
        PushMan({1,axend.fi,axend.se,p});
    };
    auto ManProvide=[&](){
        PushMan({2,Root.fi,Root.se});
    };
    auto ManDispose=[&](){
        PushMan({3,Root.fi,Root.se});
    };
    
    map<string,ll>anstypecnt;
    
    
    
 //   dd End_Time_p1 = 1800;
 //   dd End_Time_p2 = 2200;
    auto MakeEndTime=[&](ll tI){
     //   dd prog = max(0,(dd)(tI - 800) / (H - 800));
     //   return prog * (End_Time_p2 - End_Time_p1)  + End_Time_p1;
        return 2000 + DebFastInterTime; //2000は、DebFastInterTimeと、SearchAllUseと、CalcScoreの分を引いてる
    };

    INK toPeri;
    INK toMain;
    
    rep(tIindex,0,H-1){
        //  cerr<<"----------------------------------------"<<endl;
        //  DEB(tIindex);
        INK target = tI[tIindex];
        OneAns best;best.err = inf;best.type=1;
        
        
        dd baicanuse;
        {
            vd_d v = {{0, 1.3}, {200,1.3}, {401,1.1}, {700,1.1},{901, 1}, {H,1}};
            baicanuse = interpolateY(v,tIindex)
            * (T <= 30000 ? 1 : T <= 37000 ? 1.1 : T <= 41000 ? 1.2 : T<=45000 ? 1.5 : 2.0);
        }
        
        ll canuse_t = llround( (T - 6 - (int)mans.size()) / (dd)(H - tIindex) * baicanuse);
        
        ll canuse_t_zure = 0;
        if(tIindex == H - 1){
            canuse_t = T - (int)mans.size();
            canuse_t_zure = 5;
        }
     //   cerr<<"tI & canuse_t "<<tIindex<<" "<<canuse_t<<endl;
        
        SetEvalAnsKeiT(canuse_t);
        
        ll pre_mans_size = mans.size();
        
        
        assert(canuse_t >= 2);
        //cerr<<tIindex<<" "<<stop_watch()<<endl;
        dd End_Time = MakeEndTime(tIindex);
        dd given_time = 1.0 * (End_Time - stop_watch()) / (H -tIindex);
     //   DEB(given_time);
        
        dd Dkei;
        {
            ll len = K;
            dd prog = (dd)(tIindex - (H - 1) + len) / (dd)len;
            chmax(prog,0);
            Dkei = prog * D;
          //  DEB(Dkei);
        }
        
     //   if(canuse_t >= 32 && Dkei < 1 && D * preink.g < 10.0 && 1e-3 < preink.g){
        if(canuse_t >= 32 && Dkei < 1 && 1e-3 < preink.g){
            while(preink.g > EPS){
                ManDispose();
                INKdispose(preink);
              //  preink.g --;
              //  chmax(preink.g, 0);
            }
        }
        
        if(canuse_t >= 32){
            OneAns cand = SearchInter(Q,R,L,preink,target,Dkei,canuse_t-canuse_t_zure,stop_watch()+given_time);//canuse_t_zureここは原因不明
            if(EvalAns(best) > EvalAns(cand)){
                best = cand;
            }
            given_time *= 0.3;
        }
        if(EvalAns(best) > 1.0){
            
            if(canuse_t  - canuse_t_zure>= 14){
                OneAns cand = SearchPartialReset(Q,R,L,preink,target,4,Dkei,tIindex,bufConvex4,stop_watch()+given_time);
             //   cerr<<"search4 "<<cand.type<<endl;
                if(EvalAns(best) > EvalAns(cand)){
               // if(best.err > cand.err){
                    best = cand;
                }
             //   given_time *= 0.3;
            }else if(canuse_t  - canuse_t_zure >= 10){
                OneAns cand = SearchPartialReset(Q,R,L,preink,target,3,Dkei,tIindex,bufConvex4,stop_watch()+given_time);
             //   cerr<<"search3 "<<cand.type<<endl;
                if(EvalAns(best) > EvalAns(cand)){
               // if(best.err > cand.err){
                    best = cand;
                }
             //   given_time *= 0.3;
            }
            if(EvalAns(best) > 1000.0){
                if(canuse_t  - canuse_t_zure >= 6){
                    OneAns cand = SearchPartialReset(Q,R,L,preink,target,2,Dkei,tIindex,bufConvex4,stop_watch()+given_time);
                //    cerr<<"search2 "<<cand.type<<endl;
                    if(EvalAns(best) > EvalAns(cand)){
                   // if(best.err > cand.err){
                        best = cand;
                    }
                }
                if(EvalAns(best) > inf/2){
                    if(canuse_t >= 2){
                        OneAns cand = SearchOneSimple(Q,R,L,preink,target,Dkei,tIindex);
                     //   cerr<<"onesimple "<<cand.type<<endl;
                        if(EvalAns(best) > EvalAns(cand)){
                       // if(best.err > cand.err){
                            best = cand;
                        }
                    }
                }
            }
        }
       
    
        
        if(tIindex == H - 1){
          //  DEB(EvalAns(best));
            OneAns cand = SearchAllUse(Q,R,L,preink,target,Dkei);
          //  DEB(EvalAns(best));
            
            ll add_cnt = 0;
            {
                add_cnt += 1;//Provide
                for(auto &op :cand.opes){
                    add_cnt += 1;//WindowOpen
                    if(op.addg)add_cnt += 1;
                 //   assert(op.addg == 0);
                }
            }
            
            if((int)mans.size() + add_cnt<= T){
                if(best.err > cand.err){//EvalAns(best)としてはいけない
                    best = cand;
                  //  cerr<<"[DATA] alu = "<<mans.size() + add_cnt<<endl;
                }
            }
        }
       // cerr<<tIindex<<" "<<best.err<<" "<<canuse_t<<" "<<best.opes.size()<<" "<<best.OIs.size()<<endl;
        if(best.err > inf/2){
            cerr<<"[DATA] debug = notFound"<<endl;
            OUTPUT(initban,mans);
            exit(0);
            return {};
        }
        
        if(best.type == 1){
            ll ax = best.oneadd;
            if(best.onemain){
                PushMan({1,Root.fi,Root.se,ax});
                ADDInk(preink,oI[ax]);
                PushMan({2,Root.fi,Root.se});
               // preink.g -= 1;
                INKdispose(preink);
            }else{
                l_l axend = idxs[ax].back();
                PushMan({1,axend.fi,axend.se,ax});
                ADDInk(R[best.oneadd],oI[best.oneadd]);
                PushMan({2,axend.fi,axend.se});
              //  R[best.oneadd].g -= 1;
                INKdispose(R[best.oneadd]);
            }
        }else if(best.type == 3){
            {
                ll menseki = N;
                rep(j,0,K-1)menseki += L[j];
                for(const auto &op:best.opes){
                    if(op.reset && L[op.p] > 0){
                     //   Ink toPeri = {preink.c,preink.g * (dd)(L[op.p])/menseki};
                      //  INK toPeri = INKmul(preink,(dd)(L[op.p])/menseki);
                        INKmul(preink,(dd)(L[op.p])/menseki,  toPeri);
                        SUBInk(preink,toPeri);
                      //  preink.g -= toPeri.g;
                        menseki -= L[op.p];
                        ADDInk(R[op.p],toPeri);
                        ManWindowChange(op.p);//window close
                        ManDivChange(op.p,L[op.p]);//old open
                        L[op.p] = 0;
                    }
                }
            }
            for(const auto &op:best.opes){
                rep(_,1,op.addg){
                    ManAddInk(op.p);
                    ADDInk(R[op.p],oI[op.p]);
                }
               // if(!op.reset)assert(L[op.p] < op.nxL && op.nxL <= Q[op.p]);
               // else assert(L[op.p] == 0 && 0 <= op.nxL && op.nxL <= Q[op.p]);
                assert(0 <= op.nxL && op.nxL <= Q[op.p]);
                if(L[op.p] < op.nxL){
                 //   Ink toMain = {R[op.p].c, R[op.p].g * (dd)(op.nxL - L[op.p]) / (Q[op.p] - L[op.p])};
                  //  INK toMain = INKmul(R[op.p],(dd)(op.nxL - L[op.p]) / (Q[op.p] - L[op.p]));
                    INKmul(R[op.p],(dd)(op.nxL - L[op.p]) / (Q[op.p] - L[op.p]),   toMain);
                    
                  //  R[op.p].g -= toMain.g;
                    SUBInk(R[op.p],toMain);
                    ADDInk(preink,toMain);
                    ManDivChange(op.p,op.nxL);
                    ManDivChange(op.p,L[op.p]);
                    L[op.p] = op.nxL;
                }
            }
            if(preink.g <= 1){
                DEB(preink.g);
            }
            assert(preink.g >= 1);
        //    preink.g -= 1;chmax(preink.g,0);
            INKdispose(preink);
            ManProvide();
        }else if(best.type == 4){
            vector<OneAns::OpeInter> OIs;
            for(const OneAns::OpeInter &oi:best.OIs){
                if(oi.set == -1){
                    assert(Sim.isOpen(windows[oi.p].fi,windows[oi.p].se));
                    ManWindowChange(oi.p);
                }else{
                    assert(!Sim.isOpen(windows[oi.p].fi,windows[oi.p].se));
                    ManDivChange(oi.p,oi.set);
                }
            }
            for(const OneAns::OpeInter &oi:best.OIs){
                rep(_,1,oi.addg)ManAddInk(oi.p);
                for(ll v:oi.vs)ManDivChange(oi.p,v);
            }
            for(const OneAns::OpeInter &oi:best.OIs){
                assert(!Sim.isOpen(windows[oi.p].fi,windows[oi.p].se));
                ManWindowChange(oi.p);
            }
            ManProvide();
            for(const OneAns::OpeInter &oi:best.OIs){
                //L[oi.p]を求める
                vl siki;
                rep(i,1,Q[oi.p]-1){
                    if(!Sim.isOpen(idxs[oi.p][i-1],idxs[oi.p][i]))siki.pb(i);
                }
                assert((int)siki.size() <= 1);
              //  L[oi.p] = (siki.empty() ? Q[oi.p] : siki[0]);
                if(siki.empty() || siki[0] == Q[oi.p] - 1){
                    L[oi.p] = Q[oi.p]/2;
                    ManDivChange(oi.p,L[oi.p]);
                    if(!siki.empty())ManDivChange(oi.p,siki[0]);
                }else{
                    L[oi.p] = siki[0];
                }
            }
            preink = Sim.connected_INK(Root);
            for(const OneAns::OpeInter &oi:best.OIs){
                R[oi.p] = Sim.connected_INK(idxs[oi.p].back());
            }
        }else{
            assert(0);
        }
        
        /*
        {
            cerr<<tIindex<<" "<<best.err<<" ";
            if(best.type == 3)cerr<<"ReSet "<<best.opes.size();
            else if(best.type == 4)cerr<<"InTer "<<best.opes.size();
            cerr<<endl;
            
            if(best.type == 1)anstypecnt["1"]++;
            else if(best.type == 3)anstypecnt["3" + to_string(best.opes.size())]++;
            else if(best.type == 4)anstypecnt["4"]++;
            else{
                cerr<<"[DATA] invalidAnsType = 1"<<endl;
            }
        }*/
    }
    /*
    {
        cerr<<"------- anstypecnt --------"<<endl;
        for(pair<string,ll> x:anstypecnt){
            cerr<<"[DATA] Q"<<x.fi<<" = "<<x.se<<endl;
        }
    }*/
    
    return {initban,mans};
}


namespace OneArray {

/* --------------------------------------------------
   基本定数とユーティリティ
--------------------------------------------------*/
constexpr int N             = 20;   // 盤面: 20×20
//constexpr int INIT_GROUPS    = 20;   // 初期ウェル数（任意に変更可）
constexpr double EPS         = 1e-9;

/* ========= 渦巻き版 idx2rc / rc2idx ========= */

/* 1D → (row, col) : 右回りスパイラル */
inline std::pair<int,int> idx2rc(int idx){
    int layer = 0;
    int rem   = idx;
    while(true){
        int side = N - 2*layer;
        int per  = (side == 1) ? 1 : 4*(side - 1);
        if(rem < per) break;
        rem -= per;
        ++layer;
    }
    int side = N - 2*layer;
    if(side == 1) return {layer, layer};                 // 中心 1 マス

    int r, c;
    if(rem < side - 1){                                  // 上辺  → 右へ
        r = layer;
        c = layer + rem;
    }else if(rem < 2*(side - 1)){                        // 右辺  ↓ 下へ
        r = layer + (rem - (side - 1));
        c = N - layer - 1;
    }else if(rem < 3*(side - 1)){                        // 下辺  ← 左へ
        r = N - layer - 1;
        c = N - layer - 1 - (rem - 2*(side - 1));
    }else{                                               // 左辺  ↑ 上へ
        r = N - layer - 1 - (rem - 3*(side - 1));
        c = layer;
    }
    return {r, c};
}

/* (row, col) → 1D   : 右回りスパイラル */
inline int rc2idx(int r,int c){
    int layer = std::min({r, c, N - 1 - r, N - 1 - c});
    int idx   = 0;
    for(int l = 0; l < layer; ++l){
        int side = N - 2*l;
        idx += (side == 1) ? 1 : 4*(side - 1);
    }
    int side = N - 2*layer;
    if(side == 1) return idx;                            // 中心 1 マス

    if(r == layer)                                       // 上辺
        idx += c - layer;
    else if(c == N - layer - 1)                          // 右辺
        idx += (side - 1) + (r - layer);
    else if(r == N - layer - 1)                          // 下辺
        idx += 2*(side - 1) + (N - layer - 1 - c);
    else                                                 // 左辺
        idx += 3*(side - 1) + (N - layer - 1 - r);

    return idx;
}

inline void toggle_wall(Ban& ban,int a,int b){
    auto [r1,c1]=idx2rc(a); auto [r2,c2]=idx2rc(b);
    if(r1==r2) ban.v[r1][std::min(c1,c2)] ^= 1;
    else       ban.h[std::min(r1,r2)][c1] ^= 1;         // c1==c2
}

/* --------------------------------------------------
   色・ウェル関連
--------------------------------------------------*/
struct Ink{ std::vector<double> c = std::vector<double>(3,0.0); double g=0.0; };
struct Group{
    int l,r;                 // 1D idx 範囲 [l,r)
    Ink ink;
    int cap() const { return r-l; }
};

/* 重み付き平均色 */
inline void mix(const std::vector<double>& a,double ga,
                const std::vector<double>& b,double gb,
                std::vector<double>& out)
{
    if(ga+gb < EPS){ out.assign(3,0.0); return; }
    for(int i=0;i<3;i++) out[i]=(a[i]*ga + b[i]*gb)/(ga+gb);
}
/* ユークリッド距離 */
inline double dist(const std::vector<double>& a,const std::vector<double>& b){
    double s=0; for(int i=0;i<3;i++){ double d=a[i]-b[i]; s+=d*d; } return std::sqrt(s);
}

/* --------------------------------------------------
   主要関数
--------------------------------------------------*/

dd ScoreOneArray;
std::pair<Ban,std::vector<Man>> SearchOneArray(ll INIT_GROUPS)
{
    const int CELLS = N*N;
    /* ==== 初期ウェル作成（INIT_GROUPS で等分） ==== */
   // int G = std::clamp(INIT_GROUPS,1,CELLS);
    int G = INIT_GROUPS;
    int base = CELLS / G;
    int big  = CELLS - base*G;                       // 先頭 big 個だけ +1

    std::vector<int> gid(CELLS);
    int cur=0;
    for(int g=0; g<G; ++g){
        int sz = base + (g<big);
        for(int k=0;k<sz;++k) gid[cur++]=g;
    }

    Ban ban;
    ban.v.assign(N, std::vector<long long>(N-1,1));
    ban.h.assign(N-1,std::vector<long long>(N,1));
    /* 同一ウェル間は壁開放 */
    for(int i=0;i<CELLS-1;++i) if(gid[i]==gid[i+1]){
        auto [r1,c1]=idx2rc(i); auto [r2,c2]=idx2rc(i+1);
        if(r1==r2) ban.v[r1][std::min(c1,c2)]=0;
        else       ban.h[std::min(r1,r2)][c1]=0;
    }
    ScoreOneArray = (INIT_GROUPS - H) * D / 10000.0;
    /* Group 配列 */
    std::vector<Group> gs;
    int l=0;
    for(int g=0; g<G; ++g){
        int sz = base + (g<big);
        if(sz==0) continue;
        gs.push_back({l,l+sz,{{0,0,0},0.0}});
        l+=sz;
    }

    Ban initban = ban;                               // 返却用

    /* 操作列 */
    std::vector<Man> ops; ops.reserve(T);
    auto push=[&](long long tp,long long i1,long long j1,
                  long long i2=-1,long long j2=-1){
        if(ops.size()<static_cast<size_t>(T)) ops.push_back({tp,i1,j1,i2,j2});
    };

    /* 各ウェルへ 1g 初期投入 */
    for(auto& g:gs){
        int tub = rand(0,K-1);
        g.ink.c = oI[tub].c; g.ink.g = 1.0;
        auto [r,c]=idx2rc(g.l); push(1,r,c,tub);
    }

    /* 一時バッファ */
    std::vector<double> col0(3), col1(3);

    /* 候補構造 */
    struct Cand{
        double err=1e100; int act=0;                // 1 ONE / 2 MERGE / 3 BRIDGE / 4 NO_ADD
        int gL=-1,gR=-1,sl=-1,sr=-1,tub=-1;
        std::vector<double> col; double vTot=0; int cost=0;
        int idxA=-1,idxB=-1;                        // 壁端点 (BRIDGE/NO_ADD)
    };

    const double addPenalty = (double)D / 10000.0;  // 1 回の追加に相当する誤差補正

    /* =================================================
       ターゲット ループ
    =================================================*/
    for(int t=0; t<H && ops.size()+2<=static_cast<size_t>(T); ++t){

        int canUseT = max(2, (T - (int)ops.size()) / (H - t));
        Cand best;

        /* idx → gid テーブル */
        std::vector<int> gidOf(CELLS);
        for(int g=0; g<(int)gs.size(); ++g)
            for(int i=gs[g].l;i<gs[g].r;++i) gidOf[i]=g;

        /* ---------- 1) ONE (3 手) ----------*/
        if(canUseT >= 3){
            for(int g=0; g<(int)gs.size(); ++g){
                int cap=gs[g].cap(); if(cap<=3) continue;
                for(int s=2;s<=cap-2;++s){
                    int capL=s; double vL=gs[g].ink.g*(double)capL/cap;
                    if(vL+1.0>capL+EPS) continue;
                    for(int k=0;k<K;++k){
                        mix(gs[g].ink.c,vL,oI[k].c,1.0,col1);
                        double e=dist(col1,tI[t].c) + addPenalty; // 追加コスト加算
                        if(e<best.err) best={e,1,g,-1,s,-1,k,col1,vL,3,-1,-1};
                    }
                }
            }
        }

        /* ---------- 2) MERGE (3–5 手) ----------*/
        for(int g=0; g+1<(int)gs.size(); ++g){
            auto &L=gs[g], &R=gs[g+1];
            int capL=L.cap(), capR=R.cap();
            for(int sl=-1;sl<=capL-2;++sl){
                if(sl!=-1 && (sl<=1||capL-sl<=1)) continue;
                int sufL=(sl==-1)?capL:capL-sl;
                double vSufL=(sl==-1)?L.ink.g: L.ink.g*(double)sufL/capL;
                for(int sr=-1;sr<=capR-2;++sr){
                    if(sr!=-1 && (sr<=1||capR-sr<=1)) continue;
                    int preR=(sr==-1)?capR:sr;
                    double vPreR=(sr==-1)?R.ink.g: R.ink.g*(double)preR/capR;
                    int capC=sufL+preR; double vC=vSufL+vPreR;
                    if(capC==0 || vC+1.0>capC+EPS) continue;
                    int cost=3+((sl!=-1)+(sr!=-1)); if(cost>canUseT) continue;
                    mix(L.ink.c,vSufL,R.ink.c,vPreR,col0);
                    for(int k=0;k<K;++k){
                        mix(col0,vC,oI[k].c,1.0,col1);
                        double e=dist(col1,tI[t].c) + addPenalty; // 追加 1 回
                        if(e<best.err) best={e,2,g,g+1,sl,sr,k,col1,vC,cost,-1,-1};
                    }
                }
            }
        }

        /* ---------- 3) BRIDGE (4 手: 任意閉壁) ----------*/
        if(canUseT>=4){
            auto eval_wall=[&](int idxA,int idxB){
                int g1=gidOf[idxA], g2=gidOf[idxB]; if(g1==g2) return;
                auto &G1=gs[g1], &G2=gs[g2];
                int capU=G1.cap()+G2.cap(); double vU=G1.ink.g+G2.ink.g;
                if(vU+1.0>capU+EPS) return;
                mix(G1.ink.c,G1.ink.g,G2.ink.c,G2.ink.g,col0);
                for(int k=0;k<K;++k){
                    mix(col0,vU,oI[k].c,1.0,col1);
                    double e=dist(col1,tI[t].c) + addPenalty;  // 追加 1 回
                    if(e<best.err) best={e,3,g1,g2,-1,-1,k,col1,vU,4,idxA,idxB};
                }
            };
            for(int r=0;r<N;r++) for(int c=0;c<N-1;c++)
                if(ban.v[r][c]) eval_wall(rc2idx(r,c),rc2idx(r,c+1));
            for(int r=0;r<N-1;r++) for(int c=0;c<N;c++)
                if(ban.h[r][c]) eval_wall(rc2idx(r,c),rc2idx(r+1,c));
        }

        /* ---------- 4) NO_ADD (壁連続破壊のみ) ----------*/
        if(t + INIT_GROUPS >= H && canUseT>=2){
            int Gsz = (int)gs.size();
            for(int Lidx=0; Lidx<Gsz; ++Lidx){
                double gSum=0; std::vector<double> sumC(3,0.0);
                int R0=-1;
                for(int Ridx=Lidx; Ridx<Gsz; ++Ridx){
                    gSum += gs[Ridx].ink.g;
                    for(int i=0;i<3;i++) sumC[i]+=gs[Ridx].ink.c[i]*gs[Ridx].ink.g;
                    if(R0==-1 && gSum >= 1.0 - EPS) R0 = Ridx;
                }
                if(R0==-1) break;                // 1g に届かない
                double gPrefix = 0;              // 追加で色計算しやすく

                auto eval_segment=[&](int L,int R,double gTot,
                                      std::vector<double> &sumColor){
                    int walls = R-L;
                    int costOps = walls + 1;     // 壁× + 提出
                    if(costOps>canUseT) return;
                    std::vector<double> col(3);
                    for(int i=0;i<3;i++) col[i]=sumColor[i]/gTot;
                 //   double e = dist(col, tI[t].c) - addPenalty; // 追加無し → D 削減
                    double e = dist(col, tI[t].c);
                    if(e < best.err){
                        best.err = e; best.act = 4;
                        best.gL=L; best.gR=R; best.col=col;
                        best.vTot=gTot; best.cost=costOps;
                    }
                };

                /* 候補 R0, R0+1, R0+2 */
                gSum=0; sumC.assign(3,0);
                for(int Ridx=Lidx; Ridx<Gsz && Ridx<=R0+2; ++Ridx){
                    gSum += gs[Ridx].ink.g;
                    for(int i=0;i<3;i++) sumC[i]+=gs[Ridx].ink.c[i]*gs[Ridx].ink.g;
                    if(Ridx>=R0){
                        eval_segment(Lidx,Ridx,gSum,sumC);
                    }
                }
            }
        }
        
        if(best.act == 0){
          //  cerr<<"[DATA] ERROR_act0 = "<<1<<endl;
        }
        ScoreOneArray += best.err;

        /* -------------- 実行 or フォールバック -------------- */
        if(best.act==0){
            auto [r,c]=idx2rc(gs[0].l); push(1,r,c,0); push(2,r,c);
            mix(gs[0].ink.c,gs[0].ink.g,oI[0].c,1.0,gs[0].ink.c);
            continue;
        }

        /* ===== ONE ===== */
        if(best.act==1){
            int g=best.gL, s=best.sl;
            int idxCut=gs[g].l + s - 1;
            auto [ra,ca]=idx2rc(idxCut); auto [rb,cb]=idx2rc(idxCut+1);
            push(4,ra,ca,rb,cb); toggle_wall(ban,idxCut,idxCut+1);
            auto [r0,c0]=idx2rc(gs[g].l); push(1,r0,c0,best.tub); push(2,r0,c0);

            int cap=gs[g].cap();
            double vL=best.vTot, vR=gs[g].ink.g-vL;
            Group left {gs[g].l,idxCut+1,{best.col,vL}};
            Group right{idxCut+1,gs[g].r,{gs[g].ink.c,vR}};
            gs[g]=left; gs.insert(gs.begin()+g+1,right);
        }
        /* ===== MERGE ===== */
        else if(best.act==2){
            int gL=best.gL, gR=best.gR, sl=best.sl, sr=best.sr;

            /* 左ウェル分割 */
            Group preL{}, sufL{};
            if(sl==-1) sufL=gs[gL];
            else{
                int idx=gs[gL].l+sl-1;
                auto [ra,ca]=idx2rc(idx); auto [rb,cb]=idx2rc(idx+1);
                push(4,ra,ca,rb,cb); toggle_wall(ban,idx,idx+1);
                int capL=gs[gL].cap();
                int capP=sl, capS=capL-sl;
                double vP=gs[gL].ink.g*(double)capP/capL;
                double vS=gs[gL].ink.g-vP;
                preL={gs[gL].l,idx+1,{gs[gL].ink.c,vP}};
                sufL={idx+1, gs[gL].r,{gs[gL].ink.c,vS}};
            }

            /* 右ウェル分割 */
            Group preR{}, sufR{};
            if(sr==-1) preR=gs[gR];
            else{
                int idx=gs[gR].l+sr-1;
                auto [ra,ca]=idx2rc(idx); auto [rb,cb]=idx2rc(idx+1);
                push(4,ra,ca,rb,cb); toggle_wall(ban,idx,idx+1);
                int capR=gs[gR].cap();
                int capP=sr, capS=capR-sr;
                double vP=gs[gR].ink.g*(double)capP/capR;
                double vS=gs[gR].ink.g-vP;
                preR={gs[gR].l,idx+1,{gs[gR].ink.c,vP}};
                sufR={idx+1, gs[gR].r,{gs[gR].ink.c,vS}};
            }

            /* 中央壁開放 */
            int idxC=gs[gR].l - 1;
            auto [rc1,cc1]=idx2rc(idxC); auto [rc2,cc2]=idx2rc(idxC+1);
            push(4,rc1,cc1,rc2,cc2); toggle_wall(ban,idxC,idxC+1);

            /* 追加→提出 */
            auto [rAdd,cAdd]=idx2rc(sufL.l); push(1,rAdd,cAdd,best.tub); push(2,rAdd,cAdd);

            /* groups 再構築 */
            std::vector<Group> ngs;
            for(int i=0;i<(int)gs.size();++i){
                if(i<gL || i>gR) ngs.push_back(gs[i]);
                else if(i==gL){
                    if(preL.cap()) ngs.push_back(preL);
                    ngs.push_back({sufL.l, preR.r, {best.col, best.vTot}});
                    if(sufR.cap()) ngs.push_back(sufR);
                }
            }
            gs.swap(ngs);
        }
        /* ===== BRIDGE ===== */
        else if(best.act==3){
            int idxA=best.idxA, idxB=best.idxB;
            auto [r1,c1]=idx2rc(idxA); auto [r2,c2]=idx2rc(idxB);
            push(4,r1,c1,r2,c2); toggle_wall(ban,idxA,idxB);
            push(1,r1,c1,best.tub); push(2,r1,c1);
            push(4,r1,c1,r2,c2); toggle_wall(ban,idxA,idxB);

            int g1=best.gL, g2=best.gR;
            int cap1=gs[g1].cap(), cap2=gs[g2].cap();
            double v1=best.vTot*(double)cap1/(cap1+cap2);
            double v2=best.vTot-v1;
            gs[g1].ink.c=best.col; gs[g2].ink.c=best.col;
            gs[g1].ink.g=v1;       gs[g2].ink.g=v2;
        }
        /* ===== NO_ADD (壁のみ) ===== */
        else if(best.act==4){
            int L=best.gL, R=best.gR;
            /* 連続壁開放 */
            for(int k=L;k<R;++k){
                int idx=gs[k+1].l - 1;
                auto [ra,ca]=idx2rc(idx); auto [rb,cb]=idx2rc(idx+1);
                push(4,ra,ca,rb,cb); toggle_wall(ban,idx,idx+1);
            }
            /* 提出 */
            auto [r0,c0]=idx2rc(gs[L].l); push(2,r0,c0);

            /* groups 更新 */
            Group merged{gs[L].l, gs[R].r, {best.col, best.vTot - 1.0}};
            if(merged.ink.g < EPS) merged.ink.g = 0.0;
            gs[L]=merged;
            gs.erase(gs.begin()+L+1, gs.begin()+R+1);
        }

        /* ---------- ウェル数調整 (10–30) ---------- */
        if((int)gs.size() < 10){
            int id = std::max_element(gs.begin(),gs.end(),
                      [](auto&a,auto&b){return a.cap()<b.cap();}) - gs.begin();
            if(gs[id].cap()>=4){
                int mid=(gs[id].l+gs[id].r)/2;
                auto [ra,ca]=idx2rc(mid-1); auto [rb,cb]=idx2rc(mid);
                push(4,ra,ca,rb,cb); toggle_wall(ban,mid-1,mid);
                int capL=mid-gs[id].l, capR=gs[id].r-mid;
                double vT=gs[id].ink.g;
                double vL=vT*(double)capL/(capL+capR);
                double vR=vT-vL;
                Group right{mid,gs[id].r,{gs[id].ink.c,vR}};
                gs[id].r=mid; gs[id].ink.g=vL;
                gs.insert(gs.begin()+id+1,right);
            }
        }else if((int)gs.size() > 30){
            int id=0,bestSum=1e9;
            for(int i=0;i+1<(int)gs.size();++i){
                int s=gs[i].cap()+gs[i+1].cap(); if(s<bestSum){bestSum=s; id=i;}
            }
            int idx=gs[id+1].l-1;
            auto [ra,ca]=idx2rc(idx); auto [rb,cb]=idx2rc(idx+1);
            push(4,ra,ca,rb,cb); toggle_wall(ban,idx,idx+1);
            mix(gs[id].ink.c,gs[id].ink.g, gs[id+1].ink.c,gs[id+1].ink.g,col0);
            gs[id].r+=gs[id+1].cap();
            gs[id].ink.c=col0; gs[id].ink.g+=gs[id+1].ink.g;
            gs.erase(gs.begin()+id+1);
        }
    }

    return {initban, ops};
}

} // namespace OneArray


void solve_9_11(){
    //9_10_6 うずまき
    
    Ban bestban;
    vM bestmans;
    ll bestscore = inf;
    ll besttype = 0;
    
    bool boolTry1_6Only = (7000 <= T && T <= 10000 && D <= 20);
    boolTrySearchOneArray = (!boolTry1_6Only && T <= 9500);
    
    if(boolTry1_6Only){
        pair<Ban,vM> ret = BYsolve_1_6();
    //    ll score = CalcScore(ret.fi,ret.se);
    //    if(chmin(bestscore,score)){
            bestban = ret.fi;
            bestmans = ret.se;
            besttype = 1;
    //    }
    //    cerr<<"[DATA] S1 = "<<score<<endl;
    }else if(boolTrySearchOneArray){
        if(bestscore < inf/2){
            cerr<<"[DATA] TooManySearchError = "<<1<<endl;
        }
      //  vl Gs = {41,53,60,67,79};
        vl Gs = {60,55,65,50,70,40,80,30,90,58,62,53,67,46,42,73,77,35,25,20};
        ll minG = 0;
        for(ll Gpos = 0;Gpos <= (int)Gs.size()-1;Gpos++){
            ll G = Gs[Gpos];
            if(stop_watch() > 2200){
                break;
            }
            pair<Ban,vM> ret = OneArray::SearchOneArray(G);
            // ll score = CalcScore(ret.fi,ret.se);
            ll score = llround(OneArray::ScoreOneArray * 10000);
            if(chmin(bestscore,score)){
                bestban = ret.fi;
                bestmans = ret.se;
                besttype = 3;
                minG = G;
            }
            Gs.pb(rand(21,99));
            cerr<<"G & score : "<<G<<" "<<score<<" "<<OneArray::ScoreOneArray<<endl;
        }
     //   cerr<<"[DATA] minG = "<<minG<<endl;
     //   cerr<<"[DATA] S3 = "<<bestscore<<endl;
     //   cerr<<"[DATA] t_OneArray = "<<stop_watch()<<endl;
    }else{
        pair<Ban,vM> ret = BYsolve_3_4();
        if(!ret.se.empty()){
     //       ll score = CalcScore(ret.fi,ret.se);
     //       if(chmin(bestscore,score)){
                bestban = ret.fi;
                bestmans = ret.se;
                besttype = 2;
      //      }
      //      cerr<<"[DATA] S2 = "<<score<<endl;
        }
    }
    OUTPUT(bestban,bestmans);
 //   CalcScore(bestban,bestmans);
    cerr<<"[DATA] Type = "<<besttype<<endl;
    
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
    
    solve_9_11();
    
  //  rep(i,2,4)cerr<<"[DATA] c"<<i<<" = "<<DebPartialCount[i]<<endl;
  //  cerr<<"[DATA] t_i = "<<DebFastInterTime<<endl;
    cerr<<"[DATA] time = "<<llround(stop_watch())<<endl;
    quick_exit(0);
    return 0;
}


