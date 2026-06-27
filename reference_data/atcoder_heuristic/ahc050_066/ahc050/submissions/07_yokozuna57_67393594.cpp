# pragma GCC target("avx2")
# pragma GCC optimize("O3")
# pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef unsigned long long ull;

#define mp make_pair
#define fr first
#define sc second
#define chmin(a,b) a=min(a,b)
#define chmax(a,b) a=max(a,b)
#define rep_(_1,_2,_3,NAME,...) NAME
#define rep(...) rep_(__VA_ARGS__, rep2, rep1)(__VA_ARGS__)
#define rep1(i,j) for(int i=0;i<(j);i++)
#define rep2(i,j,k) for(int i=(j);i<(k);i++)
#define all(x) x.begin(),x.end()
#define pb push_back

template<class T> T T_INF(){ return 1000000000000000000; }
template<> int T_INF<int>(){ return 1000000000; }

int dx[4]={0,-1,0,1};
int dy[4]={1,0,-1,0};

int n,m;
vector<string> s;

struct board{
    ll h[42];
    ll v[42];
    void set(int i,int j){
        h[i] += 1LL<<j;
        v[j] += 1LL<<i;
    }
    pair<int,int> stop_pos(int i, int j, int k){
        int di = dx[k], dj = dy[k];
        if (di == 0) {
            // 横に進む
            uint64_t row = h[i];
            if (dj > 0) {
                // →方向
                uint64_t masked = row >> (j + 1);
                if (masked == 0) return {i, 41};  // 右端まで
                int next = __builtin_ctzll(masked);
                return {i, j + next};
            } else {
                //return stop_pos_old(i,j,k);
                // ←方向
                uint64_t masked = row & ((1ULL << j) - 1);
                if (masked == 0) return {i, 0};  // 左端まで
                int next = 63 - __builtin_clzll(masked);  // 最も右の1
                return {i, next+1};
            }
        } else {
            // 縦に進む
            uint64_t col = v[j];
            if (di > 0) {
                // ↓方向
                uint64_t masked = col >> (i + 1);
                if (masked == 0) return {41, j};  // 下端まで
                int next = __builtin_ctzll(masked);
                return {i + next, j};
            } else {
                //return stop_pos_old(i,j,k);
                // ↑方向
                uint64_t masked = col & ((1ULL << i) - 1);
                if (masked == 0) return {0, j};  // 上端まで
                int next = 63 - __builtin_clzll(masked);
                return {next+1, j};
            }
        }
    }
    /*pair<int,int> stop_pos(int i,int j,int k){
        while(!used(i,j)){
            i+=dx[k];
            j+=dy[k];
        }
        i -= dx[k];
        j -= dy[k];
        return {i,j};
    }*/
    inline bool used(int i,int j){
        return (h[i]>>j)&1LL;
    }
};
board init_board;

struct forw{
    board b;
    float p[42][42];
    void iterate(){
        float q[42][42] = {};
        rep(i,42)rep(j,42){
            if(b.used(i,j))continue;
            rep(k,4){
                auto [i_,j_] = b.stop_pos(i,j,k);
                q[i_][j_] += p[i][j] / 4.;
            }
        }
        rep(i,42)rep(j,42){
            p[i][j]=q[i][j];
        }
    }
    void iterate_n(int n=50){
        rep(x,n){
            float q[42][42] = {};
            rep(i,42)rep(j,42){
                if(b.used(i,j))continue;
                rep(k,4){
                    auto [i_,j_] = b.stop_pos(i,j,k);
                    q[i_][j_] += p[i][j];
                }
            }
            if(x%5==0){
                rep(i,42)rep(j,42){
                    p[i][j]=q[i][j] / 1024.;
                }
            }
            else {
                rep(i,42)rep(j,42){
                    p[i][j]=q[i][j];
                }
            }
        }
    }
};
forw init_forward;

void in(){
    cin>>n>>m;
    s.resize(n);
    rep(i,n)cin>>s[i];

    rep(i,42){
        init_board.h[i]=init_board.v[i]=0;
    }
    rep(i,42)rep(j,42){
        if(1<=i&&i<=40&&1<=j&&j<=40&&s[i-1][j-1]=='.'){}
        else {
            init_board.set(i,j);
        }
    }
    init_forward.b = init_board;
    rep(i,42)rep(j,42){
        if(init_board.used(i,j))init_forward.p[i][j]=0.;
        else init_forward.p[i][j]=1./(n*n-m);
    }
}

vector<pair<int,int>> solve_forward(){
    vector<pair<int,int>> ret;
    forw f = init_forward;
    rep(q,n*n-m){
        f.iterate();
        pair<float,pair<int,int>> best(10.,mp(-1,-1));
        rep(i,42)rep(j,42){
            if(f.b.used(i,j))continue;
            chmin(best,mp(f.p[i][j],mp(i,j)));
        }
        ret.pb(best.sc);
        f.b.set(best.sc.fr,best.sc.sc);
    }
    return ret;
}

const double alpha = 1000.;
float eval(const forw& f, pair<int,int> p, int iter, bool tim){
    auto g = f;
    g.b.set(p.fr,p.sc);
    //g.iterate(); g.iterate(); //g.iterate();
    if(tim)
    //if(iter<500)
    {
        //rep(q,52)g.iterate();
        g.iterate_n(50);
    }
    else {
        rep(q,3)g.iterate();
    }
    float ret = 0.;
    rep(i,42)rep(j,42){
        if(!g.b.used(i,j)&&g.p[i][j]>1e-9){
            if(g.p[i][j]>0.02)ret += g.p[i][j]*log(1./g.p[i][j]);
            else ret += g.p[i][j]*log(1./0.02);
        }
    }
    return exp(ret) + f.p[p.fr][p.sc] * (n*n-m-1-iter) * alpha;
}

vector<pair<int,int>> solve_forward2(){
    vector<pair<int,int>> ret;
    forw f = init_forward;
    rep(q,n*n-m){
        cerr<<q<<endl;
        f.iterate();
        //const int B = 100;
        const int B=min(n*n-m-q,10);
        vector<pair<float,pair<int,int>>> best(B,{10.,{-1,-1}});
        rep(i,42)rep(j,42){
            if(f.b.used(i,j))continue;
            auto temp = f.p[i][j] + rand()%10 * 1e-6;
            if(temp<best.back().fr){
                best.back() = {temp,{i,j}};
                for(int k=B-1;k>0;k--){
                    if(best[k].fr<best[k-1].fr)swap(best[k],best[k-1]);
                    else break;
                }
            }
        }
        bool tim = clock()<1830000;
        pair<float,pair<int,int>> best2 = {-1,{-1,-1}};
        for(auto p: best){
            if(p.sc.fr==-1)continue;
            float ev = eval(f, p.sc,q, tim);
            /*if(q/10==14){
                cerr<<q<<": "<<p.sc.fr<<" "<<p.sc.sc<<" "<<ev<<endl;
            }*/
            if(best2.sc.fr==-1||best2.fr>ev){
                best2 = {ev, p.sc};
            }
        }
        ret.pb(best2.sc);
        f.b.set(best2.sc.fr,best2.sc.sc);
    }
    return ret;
}

void out(const vector<pair<int,int>> &res){
    for(auto p: res){
        cout<<p.fr-1<<" "<<p.sc-1<<endl;
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    in();
    auto res = solve_forward2();

    out(res);

    cerr<<clock()<<endl;
}