#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define elif else if
#define vi vector<int>
#define vll vector<ll>
#define vvi vector<vi>
#define pii pair<int,int>


#define repname(a, b, c, d, e, ...) e
#define rep(...)                    repname(__VA_ARGS__, rep3, rep2, rep1, rep0)(__VA_ARGS__)
#define rep0(x)                     for (int rep_counter = 0; rep_counter < (x); ++rep_counter)
#define rep1(i, x)                  for (int i = 0; i < (x); ++i)
#define rep2(i, l, r)               for (int i = (l); i < (r); ++i)
#define rep3(i, l, r, c)            for (int i = (l); i < (r); i += (c))





struct ScalarInput {
    template<class T>
    operator T(){
        T ret;
        cin >> ret;
        return ret;
    }
};
struct VectorInput {
    size_t n;
    VectorInput(size_t n): n(n) {}
    template<class T>
    operator vector<T>(){
        vector<T> ret(n);
        for(T &x : ret) cin >> x;
        return ret;
    }
};
ScalarInput input(){ return ScalarInput(); }
VectorInput input(size_t n){ return VectorInput(n); }

template<typename T>
void print(vector<T> a){
  for(int i=0;i<a.size();i++){
    cout<<a[i]<<" \n"[i+1==a.size()];
  }
}

template<class T>
void print(T x){
    cout << x << '\n';
}
 
template <class Head, class... Tail>
void print(Head&& head, Tail&&... tail){
  cout << head << ' ';
  print(forward<Tail>(tail)...);
}

double randdouble(double l, double r) {
  // l以上r以下の実数を返す.
  return l + (r - l) * (double)rand() / RAND_MAX;
}

// int randint(int l, int r) {
//   // l以上r以下の整数を返す.
//   return l + rand() % (r - l + 1);
// }

mt19937_64 rng(2024);  // [0, 2^64)
ll randint(ll l, ll r) { return l + rng() % (r - l + 1); }


using ld = double;

int N,M,L;
vvi S;
vector<ld> P;

double stop_watch(struct timespec start_time) {
    // 経過時間を秒単位で返す.
    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);
    long long int sec = end_time.tv_sec - start_time.tv_sec;
    long long int nsec = end_time.tv_nsec - start_time.tv_nsec;
    return (double)sec + (double)nsec / (1000 * 1000 * 1000);
}

/*
初期化
  struct timespec start_time;
  clock_gettime(CLOCK_REALTIME, &start_time);

時間取得
stop_watch(start_time);

*/

double SA_start,SA_end,T0,T1;
double get_temp(double Time){
  return T0+(Time-SA_start)*(T0-T1)/(SA_start-SA_end);
}


void init_input(){
  cin>>N>>M>>L;
  for(int i=0;i<N;i++){
    string s;
    cin>>s;
    vector<int>t;
    for(auto c:s){
      t.push_back(c-'a');
    }
    S.push_back(t);
    int p;
    cin>>p;
    P.push_back(p);
  }
}

ld estimate(vvi &mat){
  vector<ld>first(M);
  {
    vector<ld>dp(M,0),ndp(M,0);
    dp[0]=1;
    rep(20){
      fill(ndp.begin(), ndp.end(), 0.0);
      rep(i,M){
        ld dpi=dp[i];
        rep(j,M){
          ndp[j]+=dpi*mat[i][j]*0.01;
        }
      }
      swap(dp,ndp);
    }
    first=dp;
  }

  ld score=0;

  rep(i,N){
    int pre=S[i][0];
    ld dp0=first[pre*2],dp1=first[pre*2+1];
    rep(j,1,S[i].size()){
      int nxt=S[i][j];
      ld ndp0=(dp0*mat[pre*2][nxt*2]+dp1*mat[pre*2+1][nxt*2])*0.01;
      ld ndp1=(dp0*mat[pre*2][nxt*2+1]+dp1*mat[pre*2+1][nxt*2+1])*0.01;
      swap(dp0,ndp0);
      swap(dp1,ndp1);
      pre=nxt;
    }
    ld prob=dp0+dp1;
    // ld p=(1-powl(1-prob,1e6));
    ld p = 1 - exp(-1e6 * prob);
    score+=p*P[i];
  }
  return score;
}

ld estimate_fix_use(vvi &mat, vi &use_idx){
  vector<ld>first(M);
  {
    vector<ld>dp(M,0);
    dp[0]=1;
    rep(20){
      vector<ld>ndp(M,0);
      rep(i,M)rep(j,M){
        ndp[j]+=dp[i]*mat[i][j]*0.01;
      }
      swap(dp,ndp);
    }
    first=dp;
  }

  ld score=0;

  for(auto i:use_idx){
    int pre=S[i][0];
    ld dp0=first[pre*2],dp1=first[pre*2+1];
    rep(j,1,S[i].size()){
      int nxt=S[i][j];
      ld ndp0=(dp0*mat[pre*2][nxt*2]+dp1*mat[pre*2+1][nxt*2])*0.01;
      ld ndp1=(dp0*mat[pre*2][nxt*2+1]+dp1*mat[pre*2+1][nxt*2+1])*0.01;
      swap(dp0,ndp0);
      swap(dp1,ndp1);
      pre=nxt;
    }
    ld prob=dp0+dp1;
    // ld p=(1-powl(1-prob,1e6));
    ld p = 1 - exp(-1e6 * prob);
    score+=p*P[i];
  }
  return score;
}

void output_ans(vvi &mat){
  rep(i,M){
    char c='a'+(i/2);
    cout<<c<<" ";
    print(mat[i]);
  }

  cerr<<estimate(mat)<<endl;
}

int weight[15][15];
vvi gen_mat_from_pseudoSP(vvi pS,vi pP){
  int offset=10;
  rep(i,M)rep(j,M)weight[i][j]=0;
  int sz=pS.size();
  rep(i,sz){
    rep(j,1,pS[i].size()){
      int frm=pS[i][j-1];
      int to=pS[i][j];
      // weight[frm][to]+=sqrtl(pP[i])*(offset+j);
      weight[frm][to]+=pP[i]*(offset+j);
      // weight[frm][to]+=(offset+j)*10;
    }
    int last=pS[i].back();
    rep(j,M){
      weight[last][j]+=1;
    }
  }

  vvi mat(M,vi(M,0));
  rep(i,M){
    vector<pair<int,int>>order(M);
    int sm=1;
    rep(j,M)sm+=weight[i][j];
    int cnt=0;
    rep(j,M){
      int c=((ll)100*(ll)weight[i][j])/sm;
      cnt+=c;
      mat[i][j]=c;
      order[j]={weight[i][j],j};
    }
    while(cnt+M<=100){
      rep(j,M)mat[i][j]++;
      cnt+=M;     
    }
    int rem = (100-cnt) % M;
    if (rem > 0) {
        // rem個の最大値のインデックスを前に持ってくる
        nth_element(order.begin(), order.begin() + rem, order.end(),
                    greater<pair<int,int>>());

        // 上位rem個だけに +1
        rep(j, rem){
            mat[i][order[j].second]++;
        }
    }
  }
  return mat;
}


vvi gen_first_mat(){
  struct timespec start_time;
  clock_gettime(CLOCK_REALTIME, &start_time);

  vector<pair<int,int>>order;
  rep(i,N)order.push_back({P[i],i});
  sort(order.rbegin(),order.rend());


  int sz=6;

  vvi pseudo_S;
  vi pseudo_P;
  vi use_idx;
  rep(i,sz){
    int idx=order[i].second;
    vi T=S[idx];
    rep(j,T.size()){
      T[j]=2*T[j]+randint(0,1);
    }
    pseudo_S.push_back(T);
    pseudo_P.push_back(P[idx]);
    use_idx.push_back(idx);
  }


  vvi mat=gen_mat_from_pseudoSP(pseudo_S,pseudo_P);

  SA_start=0.0;
  SA_end=0.8;
  T0=100;
  T1=0.1;

  vvi best_mat=mat;

  ld now_score=estimate_fix_use(mat,use_idx);
  ld best_score=now_score;

  int loop = 0;
  while(true){
    double now_time=stop_watch(start_time);
    if(now_time>SA_end)break;
    loop++;
    double temp=get_temp(now_time);

    int idx=randint(0,sz-1);
    int idx2=randint(0,pseudo_S[idx].size()-1);
    pseudo_S[idx][idx2]^=1;
    vvi mat0=gen_mat_from_pseudoSP(pseudo_S,pseudo_P);
    ld new_score=estimate_fix_use(mat0,use_idx);
    ld diff_score=new_score-now_score;
    if(diff_score>-10*temp&&randdouble(0,1)<exp(min(0.0,diff_score/temp))){
      now_score=new_score;
      mat=mat0;
      if(now_score>best_score){
        best_score=now_score;
        best_mat=mat;
      }
    }
    else{
      pseudo_S[idx][idx2]^=1;
    }
  }
  cerr<<"loop "<<loop<<endl;
  cerr<<best_score<<endl;
  return best_mat;
}


vvi gen_second_mat(vvi mat){
  struct timespec start_time;
  clock_gettime(CLOCK_REALTIME, &start_time);


  vector<pair<int,int>>order;
  rep(i,N)order.push_back({P[i],i});
  sort(order.rbegin(),order.rend());


  int sz=15;

  vi use_idx;
  rep(i,sz){
    use_idx.push_back(order[i].second);
  }

  ld now_score=estimate_fix_use(mat,use_idx);
  SA_start=stop_watch(start_time);
  SA_end=0.5;
  T0=100;
  T1=0.1;
  
  ld best_score=now_score;
  vvi best_mat=mat;

  int loop2=0;

  while(true){
    double now_time=stop_watch(start_time);
    if(now_time>SA_end)break;
    double temp=get_temp(now_time);
    int idx=randint(0,M-1);
    int frm=randint(0,M-1);
    int to=randint(0,M-1);
    if(frm==to)continue;
    if(mat[idx][frm]==0)continue;
    loop2++;
    int d0=randint(1,10);
    d0=min(d0,mat[idx][frm]);
    mat[idx][frm]-=d0;
    mat[idx][to]+=d0;
    ld new_score=estimate_fix_use(mat,use_idx);
    ld diff_score=new_score-now_score;
    if(diff_score>-10*temp&&randdouble(0,1)<exp(min(0.0,diff_score/temp))){
      now_score=new_score;
      if(best_score<now_score){
        best_score=now_score;
        best_mat=mat;
      }
    }
    else{
      mat[idx][frm]+=d0;
      mat[idx][to]-=d0;
    }
  }
  cerr<<"loop2 "<<loop2<<endl;
  cerr<<best_score<<endl;
  return mat;
}

vvi test_SA(){
  struct timespec start_time;
  clock_gettime(CLOCK_REALTIME, &start_time);

  vvi mat=gen_first_mat();
  mat=gen_second_mat(mat);

  ld now_score=estimate(mat);
  SA_start=stop_watch(start_time);
  SA_end=1.99;
  T0=100;
  T1=0.1;
  
  ld best_score=now_score;
  vvi best_mat=mat;

  int loop2=0;

  while(true){
    double now_time=stop_watch(start_time);
    if(now_time>SA_end)break;
    double temp=get_temp(now_time);
    int idx=randint(0,M-1);
    int frm=randint(0,M-1);
    int to=randint(0,M-1);
    if(frm==to)continue;
    if(mat[idx][frm]==0)continue;
    loop2++;
    int d0=randint(1,10);
    d0=min(d0,mat[idx][frm]);
    mat[idx][frm]-=d0;
    mat[idx][to]+=d0;
    ld new_score=estimate(mat);
    ld diff_score=new_score-now_score;
    if(diff_score>-10*temp&&randdouble(0,1)<exp(min(0.0,diff_score/temp))){
      now_score=new_score;
      if(best_score<now_score){
        best_score=now_score;
        best_mat=mat;
      }
    }
    else{
      mat[idx][frm]+=d0;
      mat[idx][to]-=d0;
    }
  }
  cerr<<"loop3 "<<loop2<<endl;
  return best_mat;
}

int main(){
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  init_input();

  vvi mat=test_SA();

  output_ans(mat);

}