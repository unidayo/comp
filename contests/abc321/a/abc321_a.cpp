// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")
#include <bits/stdc++.h>

#include <algorithm>
#include <atcoder/all>

// clang-format off
namespace atcoder {};
namespace cp_internal {char char_buff[70];};

/* namespace */
using namespace std;
using namespace atcoder;

/* type */
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;
using vull = __uint128_t;

/* ios*/
struct fast_ios{fast_ios(){ios::sync_with_stdio(false);cin.tie(nullptr);cout<<fixed<<setprecision(15);}}fast_ios;
istream &operator>>(istream &is,modint998244353 &x){ll val;is>>val;x=val;return is;}
ostream &operator<<(ostream &os,const modint998244353 &x){os<<x.val();return os;}
istream &operator>>(istream &is,modint1000000007 &x){ll val;is>>val;x=val;return is;}
ostream &operator<<(ostream &os,const modint1000000007 &x){os<<x.val();return os;}

/* math*/
ll powll(ll x, ll n){ll ret=1;while(n){if(n&1)ret=x*ret;x*=x;n>>=1;}return ret;}
ll pow_mod64(vll x, vll n, ll m){if(m==1)return 0;vll ret=1;x%=m;while(n){if(n&1)ret=x*ret%m;x=x*x%m;n>>=1;}return ret;}
template <class T> T divceil(T x, T y) { return signbit(x) ^ signbit(y) ? x / y : (x + y - 1) / y; }
template <class T> T divfloor(T x, T y) { return signbit(x) ^ signbit(y) ? -divceil(abs(x), abs(y)) : x / y; }
template <class T> T smod(T x, T m) {if(x < 0) x += m;return x % m;}

/* functions */
template <class T> ll lower_count(const vector<T> &a, T x){return ranges::lower_bound(a,x)-begin(a);}
template <class T> ll upper_count(const vector<T> &a, T x){return ranges::upper_bound(a,x)-begin(a);}
template<class T> bool chmin(T &a,T b) {if(a>b){a=b;return true;}return false;}
template<class T> bool chmax(T &a,T b) {if(a<b){a=b;return true;}return false;}
template <class T> map<T, int> compression(vector<T> a){ranges::sort(a);auto res=ranges::unique(a);a.erase(res.begin(),res.end());map<T,int> ret;for(int i=0;i<ssize(a);++i)ret[a[i]]=i;return ret;}
template<class T>vector<T> accumulate_sum(const vector<T> &v){vector<T>S(v.size());S[0]=v[0];for(int i=0;i<ssize(v)-1;++i)S[i+1]=S[i]+v[i+1];return S;}
template<class T>vector<T> accumulate_sum(const vector<T> &v,T init){vector<T>S(v.size()+1);S[0]=init;for(int i=0;i<ssize(v);++i)S[i+1]=S[i]+v[i];return S;}
template <class T> vector<vector<T>> lpartition(vector<T> &v, int w) {int n=ssize(v);int h=(n+w-1)/w;vector<vector<T>> ret(h);for(int i=0;i<n;i+=w){vector<int> tv(min(w,n-i));for(int j=i;j<min(i+w,n);++j){tv[j-i]=v[j];}ret[i/w]=tv;}return ret;}
template <class T> vector<pair<T, ll>> runlength(vector<T> &a) {vector<pair<T, ll>> ret;for(ll l=0;l<ssize(a);){ll r=l+1;for(;r<ssize(a)&&a[l]==a[r];++r){}ret.emplace_back(a[l],r-l);l=r;}return ret;}
vector<pair<char, ll>> runlength(string &a) {vector<pair<char,ll>> ret;for(ll l=0;l<ssize(a);){ll r=l+1;for(;r<ssize(a)&&a[l]==a[r];++r){};ret.emplace_back(a[l],r-l);l=r;}return ret;}

/* data_structure */
template <class T> using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
template <class T> using priority_queue_max = priority_queue<T, vector<T>>;
template <class T> auto vec2d(size_t n1,size_t n2,T init=T()){return vector(n1,vector(n2,init));}
template <class T> auto vec3d(size_t n1,size_t n2,size_t n3,T init=T()){return vector(n1,vector(n2,vector(n3,init)));}
template <class T> auto vec4d(size_t n1,size_t n2,size_t n3,size_t n4,T init=T()){return vector(n1,vector(n2,vector(n3,vector(n4,init))));}
template <class T> auto evec2d(size_t n1){return vector(n1,vector<T>());}
template <class T> auto evec3d(size_t n1,size_t n2){return vector(n1,vector(n2,vector<T>()));}
template <class T> auto evec4d(size_t n1,size_t n2,size_t n3){return vector(n1,vector(n2,vector(n3,vector<T>())));}

/* string */
string dec_to_base(ull x, int base){auto b=begin(cp_internal::char_buff);auto[ptr,ec]=to_chars(b,end(cp_internal::char_buff),x,base);return string(b,ptr);}
ull base_to_dec(const string &x, int base){auto xc=x.c_str();ull val=0;from_chars(xc,xc+x.size(),val,base);return val;}
void zfill(string &s, size_t width){s=string(width>s.size()?width-s.size():0,'0')+s;}

/* grid */
bool grid_check(ll row,ll col,ll h,ll w){return 0<=row&&row<h&&0<=col&&col<w;}

/* io */
template<class... T> void input(T &...a){(cin>>...>>a);}
template<class T> void vinput(vector<T> &v){ranges::for_each(v,[](T &x){cin>>x;});}
template<class T,class...Ts> void print(const T &a,const Ts &...b){((cout<<a),...,(cout<<' '<<b));cout<<'\n';}
template<class T> void vprint(vector<T> &v,string sep = " "){if(!v.empty()){auto p=prev(end(v));for_each(begin(v),p,[&sep](T x){cout<<x<<sep;});cout<<*p;}cout<<'\n';}
template<class T> void vprint(T &v,int l,int r,string sep=" "){if(l<r){auto p=v.begin()+(r-1);for_each(v.begin()+l,p,[&sep](auto x){cout<<x<<sep;});cout<<*p;}cout<<'\n';}
#ifdef LOCAL
template <class T,class... Ts> void debprint(const T &a,const Ts &...b){cout<<"\x1b[35m";((cout<<a),...,(cout<<' '<<b));cout<<'\n';cout<<"\x1b[m";}
template <class T> void debvprint(vector<T> &v,string sep=" "){cout<<"\x1b[35m";if(!v.empty()){auto p=prev(end(v));for_each(begin(v),p,[&sep](T x){cout<<x<<sep;});cout<<*p;}cout<<'\n';cout<<"\x1b[m";}
template <class T> void debvprint(T &v,int l,int r,string sep=" "){cout<<"\x1b[35m";if(l<r){auto p=v.begin()+(r-1);for_each(v.begin()+l,p,[&sep](auto x){cout<<x<<sep;});cout<<*p;}cout<<'\n';cout<<"\x1b[m";}
#else
template <class T,class... Ts> void debprint(const T &a,const Ts &...b){}
template <class T> void debvprint(vector<T> &v,string sep=" "){}
template <class T> void debvprint(T &v,int l,int r,string sep=" "){}
#endif

/* util */
#define OL_REP(_1, _2, _3, _4, name, ...) name
#define REP2(i, n) for(ll i = 0; i < ll(n); ++(i))
#define REP3(i, a, b) for(ll i = ll(a); i < ll(b); ++(i))
#define REP4(i, a, b, c) for(ll i = ll(a); i < ll(b); i += c)
#define rep(...) OL_REP(__VA_ARGS__, REP4, REP3, REP2)(__VA_ARGS__)

/* constants */
//[[maybe_unused]] constexpr ll MOD = 1e9 + 7;
[[maybe_unused]] constexpr ll MOD = 998244353;
[[maybe_unused]] constexpr int  INF32 = (1 << 30) - 1;
[[maybe_unused]] constexpr ll INF64 = (1LL << 62) - 1;
[[maybe_unused]] const double PI = numbers::pi;

// clang-format on

bool judge(string &s) {
    rep(i, ssize(s) - 1) {
        if (s[i] <= s[i + 1]) return false;
    }
    return true;
}

int main() {
    string S;
    input(S);
    print(judge(S) ? "Yes" : "No");
}