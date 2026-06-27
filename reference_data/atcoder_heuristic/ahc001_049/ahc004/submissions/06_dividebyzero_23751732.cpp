#include<iostream>
#include<iomanip>

#include<cmath>
#include<algorithm>
#include<numeric>

#include<vector>
#include<string>

using namespace std;
using ll = long long;
using v_ll = vector <ll>;
using vv_ll = vector <v_ll>;

ll max(ll x,ll y){return x>y?x:y;}
ll min(ll x,ll y){return x<y?x:y;}
template<class T> bool chmax(T& x,const T& y){ if (x>=y) return false; x=y; return true;}
template<class T> bool chmin(T& x,const T& y){ if (x<=y) return false; x=y; return true;}
template<class T> void quit(T x){std::cout << x << "\n"; exit(0);}

template<typename T> void vshow(T x){for(auto itr = x.begin();itr!=x.end();itr++){std::cerr << *itr << " ";} std::cerr << "\n";}
template<typename T> void vvshow(T x){for(auto itr = x.begin();itr!=x.end();itr++){vshow(*itr);} std::cerr << "\n";}

struct Problem{
  ll N, M;
  vector<string> Q;
  vv_ll adj,gain,G,discharge;
  void init(){
    cin >> N >> M;
    for(ll i=0;i<M;i++){
      string s;
      cin >> s;
      Q.push_back(s);
    }
  }
  void calc_adj(bool flg = true){
    adj=vv_ll(M,v_ll(M,-1));
    gain=vv_ll(M,v_ll(M,-1));
    for(ll i=0;i<M;i++){
      for(ll j=0;j<M;j++){
        if (i==j) continue;
        for(ll k=0;k<Q[i].size();k++){
          bool is_ok = true;
          for(ll l=k;l<Q[i].size() && l-k<Q[j].size();l++){
            if (Q[i][l]!=Q[j][l-k]){ is_ok=false; break;}
          }
          if (is_ok) {
            adj[i][j]=max(Q[i].size(),Q[j].size()+k)-ll(Q[i].size());
            gain[i][j]=Q[i].size()-k;
          }
        }
        if (adj[i][j]==-1){
          adj[i][j]=Q[j].size();
          gain[i][j]=0;
        }
      }
    }
    G=vv_ll(M);
    discharge=vv_ll(M);
    for(ll i=0;i<M;i++){
      if (flg){
        for(ll j=0;j<M;j++){
          if (adj[i][j]>0)G[i].push_back(j);
          if (adj[i][j]==0)discharge[i].push_back(j);
        }
      }else{
        for(ll j=M-1;j>=0;j--){
          if (adj[i][j]>0)G[i].push_back(j);
          if (adj[i][j]==0)discharge[i].push_back(j);
        }
      }
      // for(ll j=0;j<M;j++){
      
      sort(G[i].begin(),G[i].end(),[&](auto x, auto y){
        return gain[i][x]>gain[i][y];
      });
    }
  }
};

struct State{
  Problem *P;
  ll N,M;
  vector<bool>used,used2,used3;
  State(Problem *P):P(P){
    N=P->N;M=P->M;
    used=vector<bool>(M,false);
    used2=vector<bool>(M,false);
    used3=vector<bool>(M,false);
  }
  void dfs(ll c, string& s, ll b){
    used2[c]=true;
    for(ll i=0;i<P->discharge[c].size();i++){
      ll to = P->discharge[c][i];
      used2[to]=true;
    }
    for(ll i=0;i<P->G[c].size();i++){
      ll to = P->G[c][i];
      if (used2[to])continue;
      ll idx = P->gain[c][to];
      ll len =  P->Q[to].size() - idx;
      if (s.size()+len>N){
        if (s.size()+len==N+P->gain[to][b]){
          for(ll j=idx; j<P->Q[to].size() && s.size()<N; j++){
            s.push_back(P->Q[to][j]);
          }
        }else{
          continue;
        }
      }else{
        for(ll j=idx; j<P->Q[to].size(); j++){
          s.push_back(P->Q[to][j]);
        }
        dfs(to,s,b);
      }
    }
  }
  string get_str(ll x){
    used2=used;
    string ret;
    for(ll ii=x;ii<x+M;ii++){
      ll i=ii<M?ii:ii-M;
      if (used2[i])continue;
      ret = P->Q[i];
      dfs(i,ret, i);
      return ret;
    }
    return ret;
  }
  ll scores(string s){
    used2=used;
    ll score=0;
    for(ll i=0;i<M;i++){
      if (used[i])continue;
      string t = P->Q[i];
      for(ll k=0;k<N;k++){
        bool is_ok = true;
        for(ll l=0;l<t.size();l++){
          ll idx = k+l<N?k+l:k+l-N;
          if (t[l]!=s[idx]){
            is_ok=false;
            break;
          }
        }
        if (is_ok){
          used2[i]=true;
          score++;
          break;
        }
      }
    }
    return score;
  }
  void search(vector<string>& grid, vector<bool>&used_line,v_ll& I, char c,char c2,char c3,ll i1,ll i2, ll& row, ll&col){
    for(ll ii=0;ii<N;ii++){
      ll i=I[ii];
      if (used_line[i])continue;
      row=i;col=0;
        for(ll j=0;j<N;j++){
          char g = grid[i][j];
          char g2 = grid[i][j+i1<N?j+i1:j+i1-N];
          char g3 = grid[i][j+i2<N?j+i2:j+i2-N];
        if (g==c&&g2==c2&& g3==c3) {row=i;col=j; return;}
      }
    }
    for(ll ii=0;ii<N;ii++){
      ll i=I[ii];
      if (used_line[i])continue;
      row=i;col=0;
        for(ll j=0;j<N;j++){
          char g = grid[i][j];
          char g2 = grid[i][j+i1<N?j+i1:j+i1-N];
        if (g==c&&g2==c2) {row=i;col=j; return;}
      }
    }
    for(ll ii=0;ii<N;ii++){
      ll i=I[ii];
      if (used_line[i])continue;
      row=i;col=0;
        for(ll j=0;j<N;j++){
          char g = grid[i][j];
        if (g==c) {row=i;col=j; return;}
      }
    }
  }
};

ll solve(Problem* P, vector<string>& ret){
  State S(P);
  vector<string> ans(P->N+3);
  vector<string> ans2(P->N);
  for(ll i=0;i<P->N+3;i++){
    ll best = 0,scores=0;
    ll trials = 0;
    for(ll j=0;j<P->M && trials<800;j++){
      if (S.used[j]) continue;
      trials++;
      string s = S.get_str(j);
      while(s.size()<P->N)s.push_back('A');
      ll sc = S.scores(s);
      if (sc>scores){
        scores = sc; best = j;
      };
    }
    string s = S.get_str(best);
    while(s.size()<P->N)s.push_back('A');
    ans[i] = s;
    S.scores(s);
    S.used=S.used2;
    if (i==P->N-1)S.used3=S.used;
  }
  vv_ll counter(P->N,v_ll(8,0));
  for(ll i=0;i<P->N;i++){
    for(ll j=0;j<P->N;j++){
      counter[i][ans[i][j]-'A']++;
    }
  }
  v_ll prior(P->N);
  for(ll i=0;i<P->N;i++){
    for(ll j=0;j<8;j++){
      if (counter[i][j]>0)prior[i]++;
    }
  }
  v_ll I(P->N);
  for(ll i=0;i<P->N;i++)I[i]=i;
  sort(I.begin(),I.end(),[&](auto x, auto y){return prior[x]<prior[y];});

  S.used = S.used3;
  ll best=0,b1=1,b2=2;
  for(ll idx1=1;idx1<P->N;idx1++){
    for(ll idx2=1;idx2<P->N;idx2++){
      if (idx1==idx2)continue;
      vector<bool>used_line(P->N,false);
      
      for(ll i=0;i<P->N;i++){
        ll row,col;
        S.search(ans,used_line,I, ans[P->N][i],ans[P->N+1][i],ans[P->N+2][i],idx1,idx2,row,col);
        string s;
        for(ll ii=col;ii<col+P->N;ii++){
          ll j=ii<P->N?ii:ii-P->N;
          s.push_back(ans[row][j]);
        }
        ans2[i]=s;
        used_line[row]=true;
      }
      string C(P->N,'_');
      ll sc=0;
      // for(ll i=0;i<P->N;i++)C[i]=ans2[i][0];
      // sc += S.scores(C);
      // for(ll i=0;i<P->N;i++)C[i]=ans2[i][idx1];
      // sc += S.scores(C);
      // for(ll i=0;i<P->N;i++)C[i]=ans2[i][idx2];
      // sc += S.scores(C);
      for(ll j=0;j<P->N;j++){
        for(ll i=0;i<P->N;i++)C[i]=ans2[i][j];
        sc += S.scores(C);
      }
      if (best<sc){ best=sc; b1=idx1;b2=idx2;}
    }
  }
  vector<bool>used_line(P->N,false);
      
  for(ll i=0;i<P->N;i++){
    ll row,col;
    S.search(ans,used_line,I, ans[P->N][i],ans[P->N+1][i],ans[P->N+2][i],b1,b2,row,col);
    string s;
    for(ll ii=col;ii<col+P->N;ii++){
      ll j=ii<P->N?ii:ii-P->N;
      s.push_back(ans[row][j]);
    }
    ans2[i]=s;
    used_line[row]=true;
  }
  
  ll scr = 0;
  string C(P->N,'_');
  S.used = vector<bool>(P->M,false);
  for(ll j=0;j<P->N;j++){
    for(ll i=0;i<P->N;i++)C[i]=ans2[j][i];
    scr += S.scores(C);
    S.used = S.used2;
  }
  for(ll j=0;j<P->N;j++){
    for(ll i=0;i<P->N;i++)C[i]=ans2[i][j];
    scr += S.scores(C);
    S.used = S.used2;
  }
  
  // for(ll i=0;i<P->N;i++){
  //   cout << ans2[i] << "\n";
  // }
  ret = ans2;
  return scr;
}
int main(){
  cin.tie(nullptr);
  ios::sync_with_stdio(false);
  Problem P;
  P.init();

  vector<string>ans,ans2;
  P.calc_adj(true);
  ll x1=solve(&P,ans);
  P.calc_adj(false);
  ll x2 =solve(&P,ans2);
  // cerr << x1 << " " << x2 <<"\n";
  if (x1>x2){
    for(ll i=0;i<P.N;i++){
      cout << ans[i] << "\n";
    }
  }else{
    for(ll i=0;i<P.N;i++){
      cout << ans2[i] << "\n";
    }
  }
}