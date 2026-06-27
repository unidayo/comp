#include <bits/stdc++.h>
using namespace std;
using Graph=vector<vector<int>>;

int N,M;

//適当に選んだa,bに対しs[a],s[b]をこの順につなげる
void connect(vector<string> &s,vector<int> &cnt,int K){
  int a=rand()%(int)s.size();
  int b=rand()%(int)s.size();
  if(a==b){
    return;
  }
  int n=s[a].size();
  int m=s[b].size();
  if(n==N){
    for(int i=0;i<n;i++){
      bool flag=true;
      for(int j=0;j<m;j++){
        if(s[a][(i+j)%N]!=s[b][j]){
          flag=false;
          break;
        }
      }
      if(flag==true){
        cnt[a]+=cnt[b];
        s.erase(s.begin()+b);
        cnt.erase(cnt.begin()+b);
        return;
      }
    }
    return ;
  }
  for(int i=0;i<n;i++){
    int x=0;
    for(int j=0;j<m;j++){
      if(i+j==n){
        if(n+(m-j)<=N){
          if(x>=K){
            while(j<m){
              s[a]+=s[b][j];
              j++;
            }
            cnt[a]+=cnt[b];
            s.erase(s.begin()+b);
            cnt.erase(cnt.begin()+b);
            return;
          }
          x=0;
          break;
        }else{
          int pre_j=j;
          j+=N-n;
          while(j<m){
            if(s[a][(i+j)%N]==s[b][j]){
              x++;
            }else{
              x=0;
              break;
            }
            j++;
          }
          if(x>=K){
            while(s[a].size()<N){
              s[a]+=s[b][pre_j];
              pre_j++;
            }
            cnt[a]+=cnt[b];
            s.erase(s.begin()+b);
            cnt.erase(cnt.begin()+b);
            return;
          }
          x=0;
          break;
        }
      }
      if(s[a][i+j]==s[b][j]){
        x++;
      }else{
        x=0;
        break;
      }
    }
    if(x>0){
      cnt[a]+=cnt[b];
      s.erase(s.begin()+b);
      cnt.erase(cnt.begin()+b);
      return;
    }
  }
}

int calculate_score(vector<vector<char>> &ans,vector<string> &s){
  int c=0;
  for(int k=0;k<M;k++){
    bool flag=false;
    for(int i=0;i<N;i++){
      for(int j=0;j<N;j++){
        bool sub=true;
        for(int l=0;l<s[k].size();l++){
          if(ans[i][(j+l)%N]!=s[k][l]){
            sub=false;
            break;
          }
        }
        if(sub==true){
          flag=true;
          c++;
          break;
        }
        sub=true;
        for(int l=0;l<s[k].size();l++){
          if(ans[(i+l)%N][j]!=s[k][l]){
            sub=false;
            break;
          }
        }
        if(sub==true){
          flag=true;
          c++;
          break;
        }
      }
      if(flag==true){
        break;
      }
    }
  }
  int ret=0;
  if(c<M){
    ret=round(1e8*(double)c/(double)M);
  }else{
    int d=0;
    for(int i=0;i<N;i++){
      for(int j=0;j<N;j++){
        if(ans[i][j]=='.'){
          d++;
        }
      }
    }
    ret=round(1e8*(double)(2*N*N)/(double)(2*N*N-d));
  }
  return ret;
}

vector<vector<char>> solve_board(vector<string> &initial_s,int K){
  clock_t start,end;
  double elapsed_time=0.0;
  start=clock();

  vector<string> s=initial_s;
  vector<int> cnt(M,1);
  int ii=0;
  while(elapsed_time<=0.119){
    //cout<<s.size()<<endl;
    connect(s,cnt,K);
    end=clock();
    elapsed_time=(double)(end-start)/CLOCKS_PER_SEC;
    //cout<<s.size()<<endl;
    /*
    if((double)ii/100.0<elapsed_time){
      cout<<s.size()<<endl;
      ii++;
    }
    */
  }
  int m=s.size();
  //cout<<m<<endl;
  //assert(s.size()==cnt.size());
  //cout<<m<<endl;
  priority_queue<pair<int,int>> pq;
  for(int i=0;i<m;i++){
    pq.push(make_pair(cnt[i],i));
  }
  vector<vector<char>> ans(N,vector<char>(N,'.'));
  while(!pq.empty()){
    int k=pq.top().second;
    pq.pop();
    int tmp_i=0;
    int tmp_j=0;
    int tmp_d=0;
    int tmp_cnt=s[k].size()+1;
    for(int i=0;i<N;i++){
      for(int j=0;j<N;j++){
        //d=0
        int cnt=0;
        bool ok=true;
        for(int l=0;l<s[k].size();l++){
          if(ans[i][(j+l)%N]=='.'){
            cnt++;
            continue;
          }
          if(ans[i][(j+l)%N]!=s[k][l]){
            ok=false;
            break;
          }
        }
        if(ok==true){
          if(cnt<tmp_cnt){
            tmp_i=i;
            tmp_j=j;
            tmp_d=0;
            tmp_cnt=cnt;
          }
        }
        //d=1
        cnt=0;
        ok=true;
        for(int l=0;l<s[k].size();l++){
          if(ans[(i+l)%N][j]=='.'){
            cnt++;
            continue;
          }
          if(ans[(i+l)%N][j]!=s[k][l]){
            ok=false;
            break;
          }
        }
        if(ok==true){
          if(cnt<tmp_cnt){
            tmp_i=i;
            tmp_j=j;
            tmp_d=1;
            tmp_cnt=cnt;
          }
        }
      }
    }
    if(tmp_cnt<=s[k].size()){
      if(tmp_d==0){
        for(int l=0;l<s[k].size();l++){
          ans[tmp_i][(tmp_j+l)%N]=s[k][l];
        }
      }else{
        for(int l=0;l<s[k].size();l++){
          ans[(tmp_i+l)%N][tmp_j]=s[k][l];
        }
      }
    }
  }

  /*
  int score=calculate_score(ans,s);
  vector<int> order(m);
  random_device seed_gen;
  mt19937 engine(seed_gen());
  for(int i=0;i<m;i++){
    order[i]=i;
  }
  start=clock();
  end=clock();
  elapsed_time=(double)(end-start)/CLOCKS_PER_SEC;
  while(elapsed_time<=0.25){
    shuffle(order.begin(),order.end(),engine);
    vector<vector<char>> tmp_ans(N,vector<char>(N,'.'));
    for(int kkk=0;kkk<m;kkk++){
      int k=order[kkk];
      int tmp_i=0;
      int tmp_j=0;
      int tmp_d=0;
      int tmp_cnt=s[k].size()+1;
      for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
          //d=0
          int cnt=0;
          bool ok=true;
          for(int l=0;l<s[k].size();l++){
            if(tmp_ans[i][(j+l)%N]=='.'){
              cnt++;
              continue;
            }
            if(tmp_ans[i][(j+l)%N]!=s[k][l]){
              ok=false;
              break;
            }
          }
          if(ok==true){
            if(cnt<tmp_cnt){
              tmp_i=i;
              tmp_j=j;
              tmp_d=0;
              tmp_cnt=cnt;
            }
          }
          //d=1
          cnt=0;
          ok=true;
          for(int l=0;l<s[k].size();l++){
            if(ans[(i+l)%N][j]=='.'){
              cnt++;
              continue;
            }
            if(tmp_ans[(i+l)%N][j]!=s[k][l]){
              ok=false;
              break;
            }
          }
          if(ok==true){
            if(cnt<tmp_cnt){
              tmp_i=i;
              tmp_j=j;
              tmp_d=1;
              tmp_cnt=cnt;
            }
          }
        }
      }
      if(tmp_cnt<=s[k].size()){
        if(tmp_d==0){
          for(int l=0;l<s[k].size();l++){
            tmp_ans[tmp_i][(tmp_j+l)%N]=s[k][l];
          }
        }else{
          for(int l=0;l<s[k].size();l++){
            tmp_ans[(tmp_i+l)%N][tmp_j]=s[k][l];
          }
        }
      }
    }
    int tmp_score=calculate_score(tmp_ans,initial_s);
    if(tmp_score>score){
      score=tmp_score;
      ans=tmp_ans;
      cout<<"increased"<<endl;
    }
    end=clock();
    elapsed_time=(double)(end-start)/CLOCKS_PER_SEC;
    //cout<<elapsed_time<<endl;
  }
  */

  return ans;
}

int main(){
  cin>>N>>M;
  vector<string> s(M);
  srand((unsigned)time(NULL));
  //int minimum_length=20;
  //int maximum_length=0;
  for(int i=0;i<M;i++){
    cin>>s[i];
    //minimum_length=min(minimum_length,(int)s[i].size());
    //maximum_length=max(maximum_length,(int)s[i].size());
  }
  //int L=(minimum_length+maximum_length)/2;

  int score=0;
  vector<vector<char>> ans;
  //cout<<L<<endl;
  for(int K=0;K<24;K++){
    vector<vector<char>> tmp_ans=solve_board(s,4+K/12);
    int tmp_score=calculate_score(tmp_ans,s);
    //cout<<K/6<<" "<<tmp_score<<endl;
    if(tmp_score>score){
      score=tmp_score;
      ans=tmp_ans;
    }
  }
  //cout<<score<<endl;
  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      cout<<ans[i][j];
    }
    cout<<endl;
  }
}
