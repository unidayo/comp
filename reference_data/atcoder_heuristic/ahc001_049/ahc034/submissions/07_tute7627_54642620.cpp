//#define _GLIBCXX_DEBUG

//#pragma GCC target("avx2")
//#pragma GCC optimize("O3")
//#pragma GCC optimize("unroll-loops")

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
#define rep(i,n,m) for(int i = (n); i < (int)(m); i++)
#define rrep(i,n,m) for(int i = (int)(m) - 1; i >= (int)(n); i--)

namespace template_tute{
  using ll = long long;
  using ld = long double;
  const ll MOD1 = 1e9+7;
  const ll MOD9 = 998244353;
  const ll INF = 1e18;
  using P = pair<ll, ll>;
  template<typename T> using PQ = priority_queue<T>;
  template<typename T> using QP = priority_queue<T,vector<T>,greater<T>>;
  template<typename T1, typename T2>bool chmin(T1 &a,T2 b){if(a>b){a=b;return true;}else return false;}
  template<typename T1, typename T2>bool chmax(T1 &a,T2 b){if(a<b){a=b;return true;}else return false;}
  ll median(ll a,ll b, ll c){return a+b+c-max<ll>({a,b,c})-min<ll>({a,b,c});}
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
}
using namespace template_tute;

const int N=20;
int h[N][N];
int hv[N*N];
string dir="DURL";
struct State{
  //vector<bool>used;
  vector<int>path;
  vector<bool>used;
  vector<int>nh;
  vector<string>cmd;
  int cost,sum;
  double score=0;
  State(){
    used.assign(N*N,false);
    used[0]=true;
    nh.assign(N*N,0);
    cost=0;
    sum=0;
    rep(i,0,N*N)nh[i]=hv[i];;
    if(nh[0]>0){
      sum+=nh[0];
      cost+=nh[0];
      cmd.PB("+"+to_string(nh[0]));
      nh[0]=0;
    }
    path.push_back(0);

  }
  void go(int d){
    int x = path.back()/N;
    int y = path.back()%N;
    int nx = x+dx[d];
    int ny = y+dy[d];
    //OUT(x,y,nx,ny);
    if(nx<0||nx>=N||ny<0||ny>=N)assert(false);
    int pos=path.back();
    int npos=nx*N+ny;
    cost+=sum+100;
    cmd.PB(string(1,dir[d]));
    path.PB(npos);
    used[nx*N+ny]=true;
    if(nh[npos]<0){
      int mi=min(-nh[npos],sum);
      if(mi>0){
        nh[npos]+=mi;
        cost+=mi;
        sum-=mi;
        cmd.PB("-"+to_string(mi));
      }
    }
    else if(nh[npos]>0){
      sum+=nh[npos];
      cost+=nh[npos];
      cmd.PB("+"+to_string(nh[npos]));
      nh[npos]=0;
    }
  }
  void down(){go(0);};
  void up(){go(1);};
  void right(){go(2);};
  void left(){go(3);};

  void reach(int to){
    int nx=path.back()/N;
    int ny=path.back()%N;
    int x=to/N;
    int y=to%N;
    rep(_,0,x-nx)down();
    rep(_,0,nx-x)up();
    rep(_,0,y-ny)right();
    rep(_,0,ny-y)left();
  }
  void reach(int x,int y){
    reach(x*N+y);
  }
  void remain(){
    int sz=path.size();
    int idx=sz-1;
    while(min(nh)<0){
      while(nh[path[idx]]>=0)idx--;
      reach(path[idx]);
      //OUT(nh,idx,sz);
    }
  }
  int rem_num(){
    int cnt=0;
    rep(i,0,N*N)if(!used[i])cnt++;
    return cnt;
  }
  void recalc(){
    score=0;
    int rem=0;
    rep(i,0,N*N){
      if(used[i])rem+=max(0,-nh[i]);
    }
    int rem_cnt=rem_num();
    score=rem*(10.0+rem_cnt/10.0)*3.5+cost+sum*5;
  }
};

void solve_tekitou(){
  vector<vector<State>>st(N*N);
  st[N*N-1].PB(State());
  for(int i=N*N-1;i>0;i--){
    for(auto &s:st[i])s.recalc();
    sort(ALL(st[i]),[](State &a,State &b){return a.score<b.score;});
    st[i].resize(min(160,(int)st[i].size()));
    for(auto s:st[i]){
      rep(d,0,i==N*N-1?1:4){
        State ns=s;
        if(i!=N*N-1){
          int px=ns.path.back()/N;
          int py=ns.path.back()%N;
          int nx=px+dx[d];
          int ny=py+dy[d];
          if(nx<0||nx>=N||ny<0||ny>=N||ns.used[nx*N+ny])continue;
          ns.go(d);
        }
        rep(d,0,4){
          State nns=ns;
          int cnt=0;
          int px=nns.path.back()/N;
          int py=nns.path.back()%N;
          while(1){
            int nx=px+dx[d];
            int ny=py+dy[d];
            if(nx<0||nx>=N||ny<0||ny>=N||nns.used[nx*N+ny])break;
            cnt++;
            px+=dx[d];
            py+=dy[d];
          }
          rep(t,0,cnt){
            nns.go(d);
            int x=nns.path.back()/N;
            int y=nns.path.back()%N;
            rep(o,0,4){
              if(o==(d^1))continue;
              int nx=x+dx[o];
              int ny=y+dy[o];
              if(nx<0||nx>=N||ny<0||ny>=N||!nns.used[nx*N+ny])continue;
              if(nns.nh[nx*N+ny]>=0||nns.sum<-nns.nh[nx*N+ny])continue;
              nns.go(o); 
              if(o==d||t==cnt-1)nns.go(o^1);
              else{
                nns.go(d);
                nns.go(o^1);
                t++;
              }
            }
          }
          if(cnt==0)continue;
          int rem_num=nns.rem_num();
          //OUT(i,rem_num,cnt,d,nns.cmd,nns.path);
          st[rem_num].PB(nns);
          
          rep(t,0,cnt){
            //int chira=nns.sum;
            int chira=nns.sum;
            nns.go(d^1);
            // if(chira>nns.sum){
            //   auto nnns=nns;
            //   rep(j,0,t+1)nnns.go(d);
            //   st[rem_num].PB(nnns);
            // }
            int x=nns.path.back()/N;
            int y=nns.path.back()%N;
            rep(o,0,4){
              if(o==(d^1))continue;
              int nx=x+dx[o];
              int ny=y+dy[o];
              if(nx<0||nx>=N||ny<0||ny>=N||!nns.used[nx*N+ny])continue;
              if(nns.nh[nx*N+ny]>=0||nns.sum==0)continue;
              nns.go(o); 
              if(o==d||t==cnt-1)nns.go(o^1);
              else{
                nns.go(d^1);
                nns.go(o^1);
                t++;
              }
            }
          }
          st[rem_num].PB(nns);
        }
      }
    }
    //OUT(i,st[i].size());
  }
  //OUT(st[0].size());
  int ret=1e9;
  int pret;
  vector<string>ans;
  for(auto s:st[0]){
    //OUT(s.nh);
    //OUT(s.sum);
    int ppret=s.cost;
    s.remain();
    if(chmin(ret,s.cost)){
      ans=s.cmd;
      pret=ppret;
    }
    //break;
  }
  OUT(pret,ret);
  for(auto z:ans){
    cout<<z<<endl;
  }
}

int main(){
  cin.tie(nullptr);
  ios_base::sync_with_stdio(false);
  ll res=0,buf=0;
  bool judge = true;
  {
    int mm;cin>>mm;
  }
  rep(i,0,N)rep(j,0,N){
    cin>>h[i][j];
    hv[i*N+j]=h[i][j];
  }
  solve_tekitou();
  return 0;
}