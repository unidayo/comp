#include <bits/stdc++.h>
using namespace std;
#include <atcoder/all>
using namespace atcoder;

#ifdef _DEBUG
#else
#pragma GCC target("avx")
#endif
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")

//高速化 
struct ponjuice{ponjuice(){cin.tie(0);ios::sync_with_stdio(0);cout<<fixed<<setprecision(20);}}PonJuice;

//型
using ll = long long;
using ld = long double;
using mint = modint998244353;//1000000007;
template<class T>using vc = vector<T>; template<class T>using vvc = vc<vc<T>>; template<class T>using vvvc = vvc<vc<T>>;
using vi = vc<int>;  using vvi = vvc<int>;  using vvvi = vvvc<int>;
using vl = vc<ll>;   using vvl = vvc<ll>;   using vvvl = vvvc<ll>;
using pi = pair<int, int>;  using pl = pair<ll, ll>;
using ull = unsigned ll;
template<class T>using priq = priority_queue<T>;
template<class T>using priqg = priority_queue<T, vc<T>, greater<T>>;

// for文
#define overload4(a, b, c, d, e, ...) e
#define rep1(n)             for(ll i = 0; i < n; i++)
#define rep2(i, n)          for(ll i = 0; i < n; i++)
#define rep3(i, a, b)       for(ll i = a; i < b; i++)
#define rep4(i, a, b, step) for(ll i = a; i < b; i+= step)
#define rep(...) overload4(__VA_ARGS__, rep4, rep3, rep2, rep1)(__VA_ARGS__)
#define per1(n)             for(ll i = n-1; i >= 0; i--)
#define per2(i, n)          for(ll i = n-1; i >= 0; i--)
#define per3(i, a, b)       for(ll i = b-1; i >= a; i--)
#define per4(i, a, b, step) for(ll i = b-1; i >= a; i-= step)
#define per(...) overload4(__VA_ARGS__, per4, per3, per2, per1)(__VA_ARGS__)
#define fore1(a)       for(auto&& i : a)	
#define fore2(i,a)     for(auto&& i : a)
#define fore3(x,y,a)   for(auto&& [x, y] : a)
#define fore4(x,y,z,a) for(auto&& [x, y, z] : a)
#define fore(...) overload4(__VA_ARGS__, fore4, fore3, fore2, fore1)(__VA_ARGS__)

//関数
#define a first
#define b second
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define si(x) (ll)(x).size()
template<class S, class T>inline bool chmax(S& a, T b){return a < b && ( a = b , true);}
template<class S, class T>inline bool chmin(S& a, T b){return a > b && ( a = b , true);}
template<class T>void uniq(vc<T>&a){sort(all(a));a.erase(unique(all(a)),a.end());}
template<class T>vc<T> operator++(vc<T>&v,signed){auto res = v;fore(e,v)e++;return res;}
template<class T>vc<T> operator--(vc<T>&v,signed){auto res = v;fore(e,v)e--;return res;}
template<class T>vc<T> operator++(vc<T>&v){fore(e,v)e++;return v;}
template<class T>vc<T> operator--(vc<T>&v){fore(e,v)e--;return v;}

//入出力(operator)
template<int T>istream&operator>>(istream&is,static_modint<T>&a){ll v;is>>v;a=v;return is;}
istream&operator>>(istream&is,modint&a){ll v;cin>>v;a=v;return is;}
template<class S,class T>istream&operator>>(istream&is,pair<S,T>&a){is>>a.a>>a.b;return is;}
template<class T>istream&operator>>(istream&is,vc<T>&a){fore(e,a)is>>e;return is;}

template<int T>ostream&operator<<(ostream&os,static_modint<T>a){return os<<a.val();}
ostream&operator<<(ostream&os,modint a){return os<<a.val();}
template<class S,class T>ostream&operator<<(ostream&os,pair<S,T>&a){return os<<a.a<<" "<<a.b;}
template<class T>ostream&operator<<(ostream&os,set<T>&a){fore(it,a){os<<it<<" ";}return os;}
template<class T>ostream&operator<<(ostream&os,multiset<T>&a){fore(it,a){os<<it<<" ";}return os;}
template<class S,class T>ostream&operator<<(ostream&os,map<S,T>&a){fore(x,y,a){os<<x<<" "<<y<<"\n";}return os;}
template<class T>ostream&operator<<(ostream&os,unordered_set<T>&a){fore(it,a){os<<it<<" ";}return os;}
template<class S,class T>ostream&operator<<(ostream&os,unordered_map<S,T>&a){fore(x,y,a){os<<x<<" "<<y<<"\n";}return os;}
template<class T>ostream&operator<<(ostream&os,vc<T>&a){fore(e,a)os<<e<<" ";return os;}
template<class T>ostream&operator<<(ostream&os,vvc<T>&a){fore(e,a)os<<e<<"\n";return os;}

//入出力(関数)
vi readvi(ll n){vi a(n);cin>>a;return a;}
vl readvl(ll n){vl a(n);cin>>a;return a;}

inline void yes(){cout << "Yes\n";}
inline void no(){cout << "No\n";}
inline void yesno(bool y = true){if(y)yes();else no();}
inline void print(){cout<<endl;}
template<class T>inline void print(T a){cout<<a<<endl;}
template<class T,class... Ts>inline void print(T a,Ts ...b){cout<<a;(cout<<...<<(cout<<' ',b));cout<<endl;}

//定数
constexpr ll mod = 998244353;
constexpr ll minf=-(1<<29);
constexpr ll inf=(1<<29);
constexpr ll MINF=-(1LL<<60);
constexpr ll INF=(1LL<<60);
constexpr ld EPS = 1e-8;
const ld PI = acosl(-1);
#define equals(a, b) (abs((a) - (b)) < EPS)
const int dx[4] ={-1, 0, 1, 0};
const int dy[4] ={ 0, 1, 0,-1};
const int dx8[8] ={-1,-1,-1, 0, 1, 1, 1, 0};
const int dy8[8] ={-1, 0, 1, 1, 1, 0,-1,-1};

using TimePoint = chrono::high_resolution_clock::time_point;
struct ChronoTimer {
private:
    TimePoint startTime_;
    TimePoint endTime_;
    double mps;

public:
    inline void Start(int limit) {
        startTime_ = chrono::high_resolution_clock::now();
        endTime_ = startTime_ + chrono::milliseconds(limit);
        mps = 1 / (double)(endTime_ - startTime_).count();
    }
    
    inline bool IsOver() const {
        return chrono::high_resolution_clock::now() >= endTime_;
    }
    
    inline double NowRate() const {
        return (double)(chrono::high_resolution_clock::now() - startTime_).count() * mps;
    }

}timer;

random_device seed_gen;
mt19937 rnd(42/*seed_gen()*/);
//ここまでテンプレ
int n,m,t,seed_n;
const ld delta = 0.9;

bool isin(int x,int y){
    return 0 <= x && x < n && 0 <= y && y < n;
}

ld score(vi& a){
    ld res = 0;
    rep(i,m){
        res += a[i] * (i + 1);
    }
    return res;
}

vi mxa;
vi cnt;
int mxsum(vi& a){
    int res = 0;
    rep(i, m){
        if(a[i] == mxa[i] * delta) res++;
    }
    return res;
}

ld rare(vi& a){
    ld res = 0;
    rep(i, m){
        if(a[i] >= mxa[i] * delta) res += (ld)1 / cnt[i] * a[i] / mxa[i];
    }
    return res;
}

vvi search_perm = {
    {5, 4, 3, 3, 4, 5},
    {4, 2, 1, 1, 2, 4},
    {3, 1, 0, 0, 1, 3},
    {3, 1, 0, 0, 1, 3},
    {4, 2, 1, 1, 2, 4},
    {5, 4, 3, 3, 4, 5},
};

int main(){
    cin >> n >> m >> t;
    seed_n = n * (n - 1) * 2;
    mxa.resize(m);
    cnt.resize(m);

    rep(turn, t){

        vvi seeds(seed_n, vi(m));
        rep(j,seed_n){
            seeds[j] = readvi(m);
            rep(i,m){
                chmax(mxa[i], seeds[j][i]);
            }
        }
        cnt.assign(m, 0);
        rep(j,seed_n){
            rep(i,m){
                if(seeds[j][i] >= mxa[i] * delta) cnt[i]++;
            }
        }


        vi iot(seed_n, 0);
        iota(all(iot), 0);
        sort(all(iot), [&](int a, int b){
            if(rare(seeds[a]) != rare(seeds[b])) return rare(seeds[a]) > rare(seeds[b]);
            return score(seeds[a]) > score(seeds[b]);
        });
        

        vvi ans(n, vi(n, -1));

        auto culc = [&]() -> int {
            int res = 0;
            rep(i,n){
                rep(j,n){
                    if(i != n-1 && ans[i][j] != -1 && ans[i+1][j] != -1){
                        rep(k, m){
                            if(seeds[ans[i+1][j]][k] >= mxa[k] * delta && seeds[ans[i][j]][k] >= mxa[k] * delta) res++;
                        }
                    }
                    if(j != n-1 && ans[i][j] != -1 && ans[i][j+1] != -1){
                        rep(k, m){
                            if(seeds[ans[i][j+1]][k] >= mxa[k] * delta && seeds[ans[i][j]][k] >= mxa[k] * delta) res++;
                        }
                    }
                }
            }
            return -res;
        };
        
        vvc<pi> pems(6);
        rep(i,n) rep(j,n){
            pems[search_perm[i][j]].emplace_back(i,j);
        }

        int ind = 0;
        for(auto pem : pems){
            vi use(si(pem), 0);
            iota(all(use), ind);
            ind += si(pem);
            rep(i,si(pem)){
                use[i] = iot[use[i]];
            }
            sort(all(use));

            int bestc = minf;
            vi best = use;

            do{
                rep(i,si(pem)){
                    auto [x,y] = pem[i];
                    ans[x][y] = use[i];
                }
                if(chmax(bestc, culc())){
                    best = use;
                }
            }while(next_permutation(all(use)));
            rep(i,si(pem)){
                auto [x,y] = pem[i];
                ans[x][y] = best[i];
            }
        }

        rep(i,n){
            print(ans[i]);
        }
    }
}