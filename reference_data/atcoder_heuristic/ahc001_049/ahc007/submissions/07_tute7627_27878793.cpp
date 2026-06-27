//#define _GLIBCXX_DEBUG

#include<bits/stdc++.h>
using namespace std;

//#define endl '\n'
#define lfs cout<<fixed<<setprecision(10)
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
ll gcd(ll x,ll y){ll r;while(y!=0&&(r=x%y)!=0){x=y;y=r;}return y==0?x:y;}
vector<ll>dx={1,-1,0,0,1,1,-1,-1};vector<ll>dy={0,0,1,-1,1,-1,1,-1};
template<typename T>vector<T> make_v(size_t a,T b){return vector<T>(a,b);}
template<typename... Ts>auto make_v(size_t a,Ts... ts){return vector<decltype(make_v(ts...))>(a,make_v(ts...));}
template<typename T1, typename T2>ostream &operator<<(ostream &os, const pair<T1, T2>&p){return os << p.first << " " << p.second;}
template<typename T>ostream &operator<<(ostream &os, const vector<T> &v){for(auto &z:v)os << z << " ";cout<<"|"; return os;}
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
  sort(ord.begin(),ord.end(),[&](int i,int j){return v[i]<v[j];});
  return ord;
}
template<typename T> vector<int> descend(const vector<T>&v){
  vector<int>ord(v.size());iota(ord.begin(),ord.end(),0);
  sort(ord.begin(),ord.end(),[&](int i,int j){return v[i]>v[j];});
  return ord;
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
template< typename T = int >
struct edge {
  int to;
  T cost;
  int id;
  edge():id(-1){};
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

  //RandomNumberGenerator() : mt(chrono::steady_clock::now().time_since_epoch().count()) {}
  RandomNumberGenerator() : mt(0) {}
  RandomNumberGenerator(int x) : mt(x){
    cout<<"-----debug-----"<<endl;
  }
  ll operator()(ll a, ll b) { // [a, b)
    uniform_int_distribution< ll > dist(a, b - 1);
    return dist(mt);
  }

  ll operator()(ll b) { // [0, b)
    return (*this)(0, b);
  }
};
RandomNumberGenerator rng;
const int n=400,m=1995;
vector<int>x,y;
vector<int>u,v;
struct UnionFind {
  vector<ll> data;
  ll num;
  UnionFind(ll size) : data(size, -1) ,num(size){ }
  bool unite(int x, int y) {
    x = root(x); y = root(y);
    if (x != y) {
      if (data[y] < data[x]) swap(x, y);
      data[x] += data[y]; data[y] = x;
	  num--;
    }
    return x != y;
  }
  bool find(int x, int y) {
    return root(x) == root(y);
  }
  int root(int x) {
    return data[x] < 0 ? x : data[x] = root(data[x]);
  }
  ll size(int x) {
    return -data[root(x)];
  }
  vector<vector<int>>groups(){
    vector<vector<int>>ret(data.size());
    for(int i=0;i<data.size();i++){
      ret[root(i)].push_back(i);
    }
    return ret;
  }
  void print(){
    auto tmp=groups();
    for(int i=0;i<data.size();i++){
      if(!tmp[i].empty()){
        for(auto z:tmp[i])cout<<z<<",";
        cout<<endl;
      }
    }
  }
};
int main(){
  cin.tie(nullptr);
  ios_base::sync_with_stdio(false);
  ll res=0,buf=0;
  bool judge = true;
  //long long start=clock();
  chrono::system_clock::time_point start = chrono::system_clock::now();
  x.assign(n,0);
  y.assign(n,0);
  u.assign(m,0);
  v.assign(m,0);
  rep(i,0,n)cin>>x[i]>>y[i];
  rep(i,0,m)cin>>u[i]>>v[i];
  vector<int>d(m);
  rep(i,0,m){
    d[i]=round(hypot(x[u[i]]-x[v[i]],y[u[i]]-y[v[i]]));
  }
  vector<int>w(m);
  const int mult=10000000;
  UnionFind init(n);
  rep(e,0,m){
    cin>>w[e];
    vector<int>prob(m);
    rep(i,0,m){
      if(3*d[i]<w[e]){
        prob[i]=mult;
      }
      else if(d[i]<w[e]){
        prob[i]=(long long)mult*(w[e]-d[i])/(2*d[i]+1);
      }
    }
    int win=0,lose=0;
    if(!init.find(u[e],v[e])){
      auto preinit=init;
      vector<int>es;
      rep(i,e+1,m){
        if(prob[i]==mult)preinit.unite(u[i],v[i]);
        else if(prob[i]>0)es.PB(i);
      }
      vector<int>root(n);
      rep(i,0,n)root[i]=preinit.root(i);
      int sz;
      {
        vector<int>s(n);
        rep(i,0,n)s[i]=root[i];
        sort(ALL(s));
        UNIQUE(s);
        rep(i,0,n)root[i]=lower_bound(ALL(s),root[i])-s.begin();
        sz=s.size();
      }
      using pi=pair<int,int>;
      vector<vector<pi>>g(sz);
      for(auto i:es){
        g[root[u[i]]].EB(root[v[i]],i);
        g[root[v[i]]].EB(root[u[i]],i);
      }
      rep(_,0,10000){
        if(min(win,lose)==0&&abs(win-lose)>=100)break;
        if(abs(win-lose)>=1000)break;
        stack<int>st;
        vector<bool>used(sz);
        st.push(root[u[e]]);
        used[root[u[e]]]=true;
        while(!st.empty()&&!used[root[v[e]]]){
          auto v=poll(st);
          for(auto z:g[v]){
            if(used[z.fi]||rng(0,mult)>=prob[z.se])continue;
            st.push(z.fi);
            used[z.fi]=true;
          }
        }
        if(used[root[v[e]]])lose++;
        else win++;
      }
    }
    else win--;
    if(win>=0&&(lose==0||ld(win)/(win+lose)>=0.40)){
      cout<<1<<endl;
      res++;
      init.unite(u[e],v[e]);
    }
    else{
      cout<<0<<endl;
    }
    //cerr<<w[e] spa d[e] spa win spa lose<<endl;
    //cerr<<win spa lose<<endl;
  }
  //rep(i,0,100)cout<<rng(0,mult) spa mult<<endl;
  //cout<<res<<endl;
  return 0;
}