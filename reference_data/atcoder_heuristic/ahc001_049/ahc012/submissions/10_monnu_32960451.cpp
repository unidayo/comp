#include <bits/stdc++.h>
using namespace std;
#define INF (200000000)

random_device seed;
mt19937 engine(seed());
uniform_real_distribution<> uniform_real_dist(0.0,1.0);
// x = uniform_real_dist(engine); xに[0.0,1.0)の一様乱数を代入
clock_t start_t,end_t;
double elapsed_time;
//実行制限時間-ε
double time_limit=2.9;
//初期温度
double T0=1e4;
//最終温度
double T1=1e2;

double prob[]={0.3,0.6,0.7,0.8,0.85,0.9,0.95,1.0};
//double prob[]={0.2,0.4,0.55,0.7,0.775,0.85,0.925,1.0};

// 最初は　-INF,最後は+INF
int evaluate(set<int> &verSet,set<int> &horSet,vector<vector<int>> &sum,vector<int> &a,bool seeCnt=false){
  vector<int> verLines;
  vector<int> horLines;
  for(int x:verSet){
    verLines.push_back(x);
  }
  for(int x:horSet){
    horLines.push_back(x);
  }
  int n=verLines.size();
  int m=horLines.size();
  if((n-2)+(m-2)>100){
    return -1;
  }
  vector<int> cnt(10,0);
  for(int i=1;i<n;i++){
    for(int j=1;j<m;j++){
      int x=sum[verLines[i]][horLines[j]]-sum[verLines[i-1]][horLines[j]]-sum[verLines[i]][horLines[j-1]]+sum[verLines[i-1]][horLines[j-1]];
      if(0<x&&x<=10){
        cnt[x-1]++;
      }
    }
  }
  int A=0;
  int B=0;
  for(int i=0;i<10;i++){
    A+=min(a[i],cnt[i]);
    B+=a[i];
  }
  if(seeCnt){
    for(int i=0;i<10;i++){
      cout<<cnt[i]<<" ";
    }
    cout<<endl;
  }

  int score=round(1.0e6*(double)A/(double)B);
  return score;
}

int main(){
  start_t=clock();
  int N,K;
  cin>>N>>K;
  vector<int> a(10);
  for(int i=0;i<10;i++){
    cin>>a[i];
  }
  vector<int> x(N),y(N);
  vector<int> Xs,Ys;
  for(int i=0;i<N;i++){
    cin>>x[i]>>y[i];
    Xs.push_back(x[i]);
    Ys.push_back(y[i]);
  }

  sort(Xs.begin(),Xs.end());
  sort(Ys.begin(),Ys.end());
  Xs.erase(unique(Xs.begin(),Xs.end()),Xs.end());
  Ys.erase(unique(Ys.begin(),Ys.end()),Ys.end());

  int n=Xs.size();
  int m=Ys.size();

  vector<vector<int>> sum(n+1,vector<int>(m+1,0));
  for(int i=0;i<N;i++){
    int k=lower_bound(Xs.begin(),Xs.end(),x[i])-Xs.begin();
    int l=lower_bound(Ys.begin(),Ys.end(),y[i])-Ys.begin();
    sum[k+1][l+1]++;
  }
  for(int i=1;i<=n;i++){
    for(int j=0;j<m;j++){
      sum[i][j+1]+=sum[i][j];
    }
    for(int j=0;j<=m;j++){
      sum[i][j]+=sum[i-1][j];
    }
  }

  int best_score=-2;
  int best_k=0;
  set<int> best_verSet;
  set<int> best_horSet;
  for(int ii=0;ii<1;ii++){
    start_t=clock();
    set<int> verSet;
    set<int> horSet;
    verSet.insert(0);
    verSet.insert(n);
    horSet.insert(0);
    horSet.insert(m);
    vector<int> verUsed(n+1,0);
    vector<int> horUsed(m+1,0);
    verUsed[0]=1;
    verUsed[n]=1;
    horUsed[0]=1;
    horUsed[m]=1;
    int cnt=0;
    int score=evaluate(verSet,horSet,sum,a);
    end_t=clock();
    elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
    int loopCnt=0;
    while(elapsed_time<=time_limit){
      /*
      if(cnt>K){
        assert(false);
      }
      */
      double pp=uniform_real_dist(engine);
      int i;
      set<int>::iterator iter;
      if(pp<prob[0]){ // verSetに何か追加
        if(cnt==100){
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
        i=rand()%(n-1)+1;
        if(verUsed[i]==0){
          verUsed[i]=1;
          verSet.insert(i);
          cnt++;
        }else{
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
      }
      else if(pp<prob[1]){ // horSetに何か追加
        if(cnt==100){
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
        i=rand()%(m-1)+1;
        if(horUsed[i]==0){
          horUsed[i]=1;
          horSet.insert(i);
          cnt++;
        }else{
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
      }
      else if(pp<prob[2]){ // verSetから何か削除
        if(verSet.size()==2){
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;;
        }
        i=rand()%((int)verSet.size()-2)+1;
        iter=verSet.begin();
        while(i>0){
          iter++;
          i--;
        }
        i=*iter;
        verUsed[i]=0;
        verSet.erase(iter);
        cnt--;
      }
      else if(pp<prob[3]){ // horSetから何か削除
        if(horSet.size()==2){
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
        i=rand()%((int)horSet.size()-2)+1;
        iter=horSet.begin();
        while(i>0){
          iter++;
          i--;
        }
        i=*iter;
        horUsed[i]=0;
        horSet.erase(iter);
        cnt--;
      }
      else if(pp<prob[4]){ // 縦線を一つ右にずらす
        if(verSet.size()==2){
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;;
        }
        i=rand()%((int)verSet.size()-2)+1;
        iter=verSet.begin();
        while(i>0){
          iter++;
          i--;
        }
        i=*iter;
        if(verUsed[i+1]==0){
          verSet.erase(iter);
          verSet.insert(i+1);
          verUsed[i]=0;
          verUsed[i+1]=1;
        }else{
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
      }
      else if(pp<prob[5]){ // 縦線を一つ左にずらす
        if(verSet.size()==2){
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
        i=rand()%((int)verSet.size()-2)+1;
        iter=verSet.begin();
        while(i>0){
          iter++;
          i--;
        }
        i=*iter;
        if(verUsed[i-1]==0){
          verSet.erase(iter);
          verSet.insert(i-1);
          verUsed[i]=0;
          verUsed[i-1]=1;
        }else{
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
      }
      else if(pp<prob[6]){ // 横線を一つ上にずらす
        if(horSet.size()==2){
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
        i=rand()%((int)horSet.size()-2)+1;
        iter=horSet.begin();
        while(i>0){
          iter++;
          i--;
        }
        i=*iter;
        if(horUsed[i+1]==0){
          horSet.erase(iter);
          horSet.insert(i+1);
          horUsed[i]=0;
          horUsed[i+1]=1;
        }else{
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
      }
      else{ // 横線を一つ下にずらす
        if(horSet.size()==2){
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
        i=rand()%((int)horSet.size()-2)+1;
        iter=horSet.begin();
        while(i>0){
          iter++;
          i--;
        }
        i=*iter;
        if(horUsed[i-1]==0){
          horSet.erase(iter);
          horSet.insert(i-1);
          horUsed[i]=0;
          horUsed[i-1]=1;
        }else{
          end_t=clock();
          elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
          continue;
        }
      }

      //loopCnt++;

      double T=pow(T0,1.0-elapsed_time/time_limit)*pow(T1,elapsed_time/time_limit);
      //操作の決定 && diffを求める
      int tmp_score=evaluate(verSet,horSet,sum,a);
      int diff=tmp_score-score;
      //遷移確率
      double p=min<double>(1.0,exp(diff/T));
      if(uniform_real_dist(engine)>p){
        //トレースバック
        if(pp<prob[0]){
          verSet.erase(i);
          verUsed[i]=0;
          cnt--;
        }else if(pp<prob[1]){
          horSet.erase(i);
          horUsed[i]=0;
          cnt--;
        }else if(pp<prob[2]){
          verSet.insert(i);
          verUsed[i]=1;
          cnt++;
        }else if(pp<prob[3]){
          horSet.insert(i);
          horUsed[i]=1;
          cnt++;
        }else if(pp<prob[4]){
          verSet.erase(i+1);
          verSet.insert(i);
          verUsed[i+1]=0;
          verUsed[i]=1;
        }else if(pp<prob[5]){
          verSet.erase(i-1);
          verSet.insert(i);
          verUsed[i-1]=0;
          verUsed[i]=1;
        }else if(pp<prob[6]){
          horSet.erase(i+1);
          horSet.insert(i);
          horUsed[i+1]=0;
          horUsed[i]=1;
        }else{
          horSet.erase(i-1);
          horSet.insert(i);
          horUsed[i-1]=0;
          horUsed[i]=1;
        }
      }else{
        score=tmp_score;
        /*
        cout<<cnt<<endl;
        for(auto x:verSet){
          if(x==0||x==n){
            continue;
          }
          cout<<Xs[x]<<" "<<(-INF)<<" "<<Xs[x]+1<<" "<<(INF)<<'\n';
        }
        for(auto y:horSet){
          if(y==0||y==m){
            continue;
          }
          cout<<(-INF)<<" "<<Ys[y]<<" "<<INF<<" "<<Ys[y]+1<<'\n';
        }
        */
      }
      end_t=clock();
      elapsed_time=(double)(end_t-start_t)/CLOCKS_PER_SEC;
    }

    if(score>best_score){
      best_score=score;
      best_k=cnt;
      best_verSet=verSet;
      best_horSet=horSet;
    }
  }


  cout<<best_k<<endl;
  for(auto x:best_verSet){
    if(x==0||x==n){
      continue;
    }
    cout<<Xs[x]<<" "<<(-INF)<<" "<<Xs[x]-1<<" "<<(INF)<<'\n';
  }
  for(auto y:best_horSet){
    if(y==0||y==m){
      continue;
    }
    cout<<(-INF)<<" "<<Ys[y]<<" "<<INF<<" "<<Ys[y]-1<<'\n';
  }


  //cout<<best_score<<endl;
  //evaluate(best_verSet,best_horSet,sum,a,true);


  //cout<<loopCnt<<endl;


}
