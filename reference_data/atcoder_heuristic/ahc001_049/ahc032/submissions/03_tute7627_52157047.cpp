//#define _GLIBCXX_DEBUG

#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include<bits/stdc++.h>
using namespace std;


#ifdef LOCAL
#include <debug_print.hpp>
#define OUT(...) debug_print::multi_print(#__VA_ARGS__, __VA_ARGS__)
#else
#define OUT(...) (static_cast<void>(0))
#endif

#define endl '\n'
#define lfs cout<<fixed<<setprecision(15)
#define ALL(a)  (a).begin(),(a).end()
#define ALLR(a)  (a).rbegin(),(a).rend()
#define UNIQUE(a) (a).erase(unique((a).begin(),(a).end()),(a).end())
#define spa << " " <<
#define fi first
#define se second
#define MP make_pair
#define MT make_tuple
#define PB push_back
#define EB emplace_back
#define rep(i,n,m) for(ll i = (n); i < (ll)(m); i++)
#define rrep(i,n,m) for(ll i = (ll)(m) - 1; i >= (ll)(n); i--)
using ll = int;
using ld = long double;
const ll MOD1 = 1e9+7;
const ll MOD9 = 998244353;
const ll INF = 1e18;
using P = pair<ll, ll>;
template<typename T> using PQ = priority_queue<T>;
template<typename T> using QP = priority_queue<T,vector<T>,greater<T>>;
template<typename T1, typename T2>bool chmin(T1 &a,T2 b){if(a>b){a=b;return true;}else return false;}
template<typename T1, typename T2>bool chmax(T1 &a,T2 b){if(a<b){a=b;return true;}else return false;}
ll median(ll a,ll b, ll c){return a+b+c-max({a,b,c})-min({a,b,c});}
void ans1(bool x){if(x) cout<<"Yes"<<endl;else cout<<"No"<<endl;}
void ans2(bool x){if(x) cout<<"YES"<<endl;else cout<<"NO"<<endl;}
void ans3(bool x){if(x) cout<<"Yay!"<<endl;else cout<<":("<<endl;}
template<typename T1,typename T2>void ans(bool x,T1 y,T2 z){if(x)cout<<y<<endl;else cout<<z<<endl;}  
template<typename T1,typename T2,typename T3>void anss(T1 x,T2 y,T3 z){ans(x!=y,x,z);};  
template<typename T>void debug(const T &v,ll h,ll w,string sv=" "){for(ll i=0;i<h;i++){cout<<v[i][0];for(ll j=1;j<w;j++)cout<<sv<<v[i][j];cout<<endl;}};
template<typename T>void debug(const T &v,ll n,string sv=" "){if(n!=0)cout<<v[0];for(ll i=1;i<n;i++)cout<<sv<<v[i];cout<<endl;};
template<typename T>void debug(const vector<T>&v){debug(v,v.size());}
template<typename T>void debug(const vector<vector<T>>&v){for(auto &vv:v)debug(vv,vv.size());}
template<typename T>void debug(stack<T> st){while(!st.empty()){cout<<st.top()<<" ";st.pop();}cout<<endl;}
template<typename T>void debug(queue<T> st){while(!st.empty()){cout<<st.front()<<" ";st.pop();}cout<<endl;}
template<typename T>void debug(deque<T> st){while(!st.empty()){cout<<st.front()<<" ";st.pop_front();}cout<<endl;}
template<typename T>void debug(PQ<T> st){while(!st.empty()){cout<<st.top()<<" ";st.pop();}cout<<endl;}
template<typename T>void debug(QP<T> st){while(!st.empty()){cout<<st.top()<<" ";st.pop();}cout<<endl;}
template<typename T>void debug(const set<T>&v){for(auto z:v)cout<<z<<" ";cout<<endl;}
template<typename T>void debug(const multiset<T>&v){for(auto z:v)cout<<z<<" ";cout<<endl;}
template<typename T,size_t size>void debug(const array<T, size> &a){for(auto z:a)cout<<z<<" ";cout<<endl;}
template<typename T,typename V>void debug(const map<T,V>&v){for(auto z:v)cout<<"["<<z.first<<"]="<<z.second<<",";cout<<endl;}
template<typename T>vector<vector<T>>vec(ll x, ll y, T w){vector<vector<T>>v(x,vector<T>(y,w));return v;}
vector<ll>dx={1,-1,0,0,1,1,-1,-1};vector<ll>dy={0,0,1,-1,1,-1,1,-1};
template<typename T>vector<T> make_v(size_t a,T b){return vector<T>(a,b);}
template<typename... Ts>auto make_v(size_t a,Ts... ts){return vector<decltype(make_v(ts...))>(a,make_v(ts...));}
template<typename T1, typename T2>ostream &operator<<(ostream &os, const pair<T1, T2>&p){return os << "(" << p.first << "," << p.second << ")";}
template<typename T>ostream &operator<<(ostream &os, const vector<T> &v){os<<"[";for(auto &z:v)os << z << ",";os<<"]"; return os;}
template<typename T>void rearrange(vector<int>&ord, vector<T>&v){
  auto tmp = v;
  for(int i=0;i<tmp.size();i++)v[i] = tmp[ord[i]];
}
template<typename Head, typename... Tail>void rearrange(vector<int>&ord,Head&& head, Tail&&... tail){
  rearrange(ord, head);
  rearrange(ord, tail...);
}
template<typename T> vector<int> ascend(const vector<T>&v){
  vector<int>ord(v.size());iota(ord.begin(),ord.end(),0);
  sort(ord.begin(),ord.end(),[&](int i,int j){return make_pair(v[i],i)<make_pair(v[j],j);});
  return ord;
}
template<typename T> vector<int> descend(const vector<T>&v){
  vector<int>ord(v.size());iota(ord.begin(),ord.end(),0);
  sort(ord.begin(),ord.end(),[&](int i,int j){return make_pair(v[i],-i)>make_pair(v[j],-j);});
  return ord;
}
template<typename T> vector<T> inv_perm(const vector<T>&ord){
  vector<T>inv(ord.size());
  for(int i=0;i<ord.size();i++)inv[ord[i]] = i;
  return inv;
}
ll FLOOR(ll n,ll div){assert(div>0);return n>=0?n/div:(n-div+1)/div;}
ll CEIL(ll n,ll div){assert(div>0);return n>=0?(n+div-1)/div:n/div;}
ll digitsum(ll n){ll ret=0;while(n){ret+=n%10;n/=10;}return ret;}
ll modulo(ll n,ll d){return (n%d+d)%d;};
template<typename T>T min(const vector<T>&v){return *min_element(v.begin(),v.end());}
template<typename T>T max(const vector<T>&v){return *max_element(v.begin(),v.end());}
template<typename T>T acc(const vector<T>&v){return accumulate(v.begin(),v.end(),T(0));};
template<typename T>T reverse(const T &v){return T(v.rbegin(),v.rend());};
//mt19937 mt(chrono::steady_clock::now().time_since_epoch().count());
int popcount(ll x){return __builtin_popcountll(x);};
int poplow(ll x){return __builtin_ctzll(x);};
int pophigh(ll x){return 63 - __builtin_clzll(x);};
template<typename T>T poll(queue<T> &q){auto ret=q.front();q.pop();return ret;};
template<typename T>T poll(priority_queue<T> &q){auto ret=q.top();q.pop();return ret;};
template<typename T>T poll(QP<T> &q){auto ret=q.top();q.pop();return ret;};
template<typename T>T poll(stack<T> &s){auto ret=s.top();s.pop();return ret;};
ll MULT(ll x,ll y){if(LLONG_MAX/x<=y)return LLONG_MAX;return x*y;}
ll POW2(ll x, ll k){ll ret=1,mul=x;while(k){if(mul==LLONG_MAX)return LLONG_MAX;if(k&1)ret=MULT(ret,mul);mul=MULT(mul,mul);k>>=1;}return ret;}
ll POW(ll x, ll k){ll ret=1;for(int i=0;i<k;i++){if(LLONG_MAX/x<=ret)return LLONG_MAX;ret*=x;}return ret;}
std::ostream &operator<<(std::ostream &dest, __int128_t value) {
  std::ostream::sentry s(dest);
  if (s) {
    __uint128_t tmp = value < 0 ? -value : value;
    char buffer[128];
    char *d = std::end(buffer);
    do {
      --d;
      *d = "0123456789"[tmp % 10];
      tmp /= 10;
    } while (tmp != 0);
    if (value < 0) {
      --d;
      *d = '-';
    }
    int len = std::end(buffer) - d;
    if (dest.rdbuf()->sputn(d, len) != len) {
      dest.setstate(std::ios_base::badbit);
    }
  }
  return dest;
}
namespace converter{
  int dict[500];
  const string lower="abcdefghijklmnopqrstuvwxyz";
  const string upper="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const string digit="0123456789";
  const string digit1="123456789";
  void regi_str(const string &t){
    for(int i=0;i<t.size();i++){
      dict[t[i]]=i;
    }
  }
  void regi_int(const string &t){
    for(int i=0;i<t.size();i++){
      dict[i]=t[i];
    }
  }
  vector<int>to_int(const string &s,const string &t){
    regi_str(t);
    vector<int>ret(s.size());
    for(int i=0;i<s.size();i++){
      ret[i]=dict[s[i]];
    }
    return ret;
  }
  vector<int>to_int(const string &s){
    auto t=s;
    sort(t.begin(),t.end());
    t.erase(unique(t.begin(),t.end()),t.end());
    return to_int(s,t);
  }
  
  vector<vector<int>>to_int(const vector<string>&s,const string &t){
    regi_str(t);
    vector<vector<int>>ret(s.size(),vector<int>(s[0].size()));
    for(int i=0;i<s.size();i++){
      for(int j=0;j<s[0].size();j++){
        ret[i][j]=dict[s[i][j]];
      }
    }
    return ret;
  }
  vector<vector<int>>to_int(const vector<string>&s){
    string t;
    for(int i=0;i<s.size();i++){
      t+=s[i];
    }
    sort(t.begin(),t.end());t.erase(unique(t.begin(),t.end()),t.end());
    return to_int(s,t);
  }
  string to_str(const vector<int>&s,const string &t){
    regi_int(t);
    string ret;
    for(auto z:s)ret+=dict[z];
    return ret;
  }
  vector<string> to_str(const vector<vector<int>>&s,const string &t){
    regi_int(t);
    vector<string>ret(s.size());
    for(int i=0;i<s.size();i++){
      for(auto z:s[i])ret[i]+=dict[z];
    }
    return ret;
  }
}
template< typename T = int >
struct edge {
  int to;
  T cost;
  int id;
  edge():to(-1),id(-1){};
  edge(int to, T cost = 1, int id = -1):to(to), cost(cost), id(id){}
  operator int() const { return to; }
};

template<typename T>
using Graph = vector<vector<edge<T>>>;
template<typename T>
Graph<T>revgraph(const Graph<T> &g){
  Graph<T>ret(g.size());
  for(int i=0;i<g.size();i++){
    for(auto e:g[i]){
      int to = e.to;
      e.to = i;
      ret[to].push_back(e);
    }
  }
  return ret;
}
template<typename T>
Graph<T> readGraph(int n,int m,int indexed=1,bool directed=false,bool weighted=false){
  Graph<T> ret(n);
  for(int es = 0; es < m; es++){
    int u,v;
    T w=1;
    cin>>u>>v;u-=indexed,v-=indexed;
    if(weighted)cin>>w;
    ret[u].emplace_back(v,w,es);
    if(!directed)ret[v].emplace_back(u,w,es);
  }
  return ret;
}
template<typename T>
Graph<T> readParent(int n,int indexed=1,bool directed=true){
  Graph<T>ret(n);
  for(int i=1;i<n;i++){
    int p;cin>>p;
    p-=indexed;
    ret[p].emplace_back(i);
    if(!directed)ret[i].emplace_back(p);
  }
  return ret;
}
struct RandomNumberGenerator {
  mt19937_64 mt;

  RandomNumberGenerator() : mt(chrono::steady_clock::now().time_since_epoch().count()) {}
  RandomNumberGenerator(int x) : mt(x){
    //cerr<<"-----debug-----"<<endl;
  }
  ll operator()(ll a, ll b) { // [a, b)
    uniform_int_distribution< ll > dist(a, b - 1);
    return dist(mt);
  }

  ll operator()(ll b) { // [0, b)
    return (*this)(0, b);
  }
};
RandomNumberGenerator rng(0);
const int N=9,M=20,K=81;
int a[N][N];
int s[M][9];
struct S{
  array<int,81>arr;
  vector<tuple<int,int,int>>ope;
  int resource;
  long long score;
  S(){
    resource=0;
    score=0;
  }
};
int main(){
  cin.tie(nullptr);
  ios_base::sync_with_stdio(false);
  int scr;
  cin>>scr>>scr>>scr;
  rep(i,0,N)rep(j,0,N){
    cin>>a[i][j];
  }
  rep(i,0,M){
    rep(j,0,3){
      rep(o,0,3){
        cin>>s[i][j*3+o];
      }
    }
  }
  int K=400;
  vector<S>vstate;
  {
    S init;
    rep(i,0,N)rep(j,0,N){
      init.arr[i*9+j]=a[i][j];
    }
    vstate.PB(init);
  }
  for(int i=0;i<N-2;i+=1){
    for(int j=0;j<N-2;j+=1){
      array<bool,9>bt={0};
      if(i==6&&j==6)rep(o,0,9)bt[o]=true;
      else if(i==6)rep(o,0,3)bt[3*o]=true;
      else if(j==6)rep(o,0,3)bt[o]=true;
      else bt[0]=true;
      long long mx=-1;
      vector<int>vg;
      int lim;
      ll cnt=0;
      rep(o,0,9)if(bt[o])cnt++;
      if(cnt==1)lim=1;
      else if(cnt==3)lim=3;
      else{
        lim=9;
        vstate.resize(4);
        K=1;
      }
      vector<S>nvstate;
      for(auto state:vstate){
        //OUT(state.ope.size());
        array<int,9>v;
        rep(x,0,3)rep(y,0,3){
          v[x*3+y]=state.arr[(i+x)*9+j+y];
        }
        
        //OUT(lim,cnt);
        auto dfs=[&](auto &&f,int p)->void {
          //OUT(i,vg.size());
          if(p==M){
            long long val=0;
            rep(x,0,9){
              if(bt[x])val+=v[x];
            }
            if(nvstate.size()<K||nvstate.back().score<state.score+val){
              auto instate=state;
              instate.resource+=lim-vg.size();
              instate.score=state.score+val;
              rep(x,0,3)rep(y,0,3){
                instate.arr[(i+x)*9+j+y]=v[x*3+y];
              }
              for(auto o:vg)instate.ope.EB(o,i,j);
              if(nvstate.size()==K)nvstate.pop_back();
              for(int j=0;j<=nvstate.size();j++){
                if(j==nvstate.size()||nvstate[j].score<instate.score){
                  nvstate.insert(nvstate.begin()+j,instate);
                  break;
                }
              }
            }
            return;
          }
          f(f,p+1);
          if(vg.size()<lim+(cnt!=9&&state.resource>0)){
            vg.PB(p);
            //OUT(i,vg);
            rep(x,0,9){
              v[x]+=s[p][x];
              if(v[x]>=MOD9)v[x]-=MOD9;
            }
            f(f,p);
            rep(x,0,9){
              v[x]-=s[p][x];
              if(v[x]<0)v[x]+=MOD9;
            }
            //OUT(i,vg);
            //assert(!vg.empty());
            vg.pop_back();
          }
        };
        //if(cnt==1&&v[0]>=950000000)lim--;
        dfs(dfs,0);
      }
      //OUT(vstate.size(),nvstate.size());
      //OUT(lim,ret.size());
      vstate.swap(nvstate);
      //exit(0);
    }
  }
  auto ret=vstate[0];
  cout<<ret.ope.size()<<endl;
  for(auto [i,x,y]:ret.ope){
    cout<<i spa x spa y<<endl;
  }
  OUT(ret.score,ret.resource,ret.ope.size());
  //OUT(ret.arr);
  return 0;
}