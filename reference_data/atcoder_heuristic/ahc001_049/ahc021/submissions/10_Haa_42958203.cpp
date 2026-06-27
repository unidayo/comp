#include <bits/stdc++.h>
using namespace std;
template<typename T> bool chmax(T& x, const T& y){return (x<y)?(x=y,true):false;};
template<typename T> bool chmin(T& x, const T& y){return (x>y)?(x=y,true):false;};

const int W=28;
const int dx[6]={-1,-1,0,0,1,1}, dy[6]={-1,0,-1,1,0,1};
const int N=30, M=465;
vector<int> H(M);
vector<vector<int>> B(N);
vector<pair<int,int>> P(M);

inline int id(int i, int j){
    return i*(i+1)/2+j;
}

inline int val(int i, int j, int k){
    return k*(abs(j*2-i)+(N-i-1)*4);
}

void init(){
    int cnt=0;
    for(int i=0;i<N;i++){
        for(int j=0;j<=i;j++){
            H[cnt++]=i;
        }
    }
}

void input(){
    for(int i=0;i<N;i++){
        B[i].resize(i+1);
        for(int j=0;j<=i;j++){
            cin >> B[i][j];
            P[B[i][j]]={i,j};
        }
    }
}

struct Stats{
    vector<vector<int>> b;
    vector<pair<int,int>> p;
    int costsum;
    vector<pair<pair<int,int>,pair<int,int>>> ans;
    bool operator<(const Stats &rhs)const{
        return costsum<rhs.costsum;
    }
};

void add_newstate(vector<Stats>& ss, Stats& s, const int& sx, const int& sy){
    vector<int> dist(M,-1e8);
    vector<int> sum(M,-1e8);
    vector<pair<int,int>> from(M,{-1,-1});
    priority_queue<pair<pair<int,int>,pair<int,int>>> que;
    dist[id(sx,sy)]=0;
    sum[id(sx,sy)]=0;
    que.push({{0,0},{sx,sy}});
    while(!que.empty()){
        auto p=que.top();
        que.pop();
        int i=p.second.first;
        int j=p.second.second;
        int v=id(i,j);
        if(dist[v]>p.first.first)
            continue;
        for(int k=0;k<6;k++){
            int ti=i+dx[k], tj=j+dy[k];
            if(ti<0||ti>=N||tj<0||tj>ti)
                continue;
            if(s.b[ti][tj]<s.b[sx][sy])
                continue;
            int cost1=1;
            int cost2=s.b[ti][tj];
            if(chmax(dist[id(ti,tj)],dist[v]-cost1)){
                chmax(dist[id(ti,tj)],dist[v]-cost1);
                chmax(sum[id(ti,tj)],sum[v]+cost2);
                que.push({{dist[id(ti,tj)],sum[id(ti,tj)]},{ti,tj}});
                from[id(ti,tj)]={i,j};
            }
        }
    }
    vector<pair<int,int>> nxys;
    for(int i=0;i<N;i++){
        if(i<N-1){
            for(int j=0;j<=i;j++){
                if(s.b[i][j]<s.b[sx][sy])
                    continue;
                if(from[id(i,j)].first==-1&&(sx!=i||sy!=j))
                    continue;
                if(i==0||((j==i||s.b[i-1][j]<s.b[sx][sy])&&(j==0||s.b[i-1][j-1]<s.b[sx][sy])))
                    nxys.push_back({i,j});
            }
        }
        else{
            for(int j=0;j<=i;j++){
                if(s.b[i][j]<s.b[sx][sy])
                    continue;
                if(from[id(i,j)].first==-1&&(sx!=i||sy!=j))
                    continue;
                if(i==0||((j==i||s.b[i-1][j]<s.b[sx][sy])&&(j==0||s.b[i-1][j-1]<s.b[sx][sy]))){
                    nxys.push_back({i,j});
                    break;
                }
            }
            for(int j=i;j>=0;j--){
                if(s.b[i][j]<s.b[sx][sy])
                    continue;
                if(from[id(i,j)].first==-1&&(sx!=i||sy!=j))
                    continue;
                if(i==0||((j==i||s.b[i-1][j]<s.b[sx][sy])&&(j==0||s.b[i-1][j-1]<s.b[sx][sy]))){
                    if(nxys.back()!=pair<int,int>{i,j})
                        nxys.push_back({i,j});
                    break;
                }
            }
        }
    }
    for(auto p:nxys){
        int nx=p.first, ny=p.second;
        vector<pair<int,int>> r;
        while(nx!=-1){
            r.push_back({nx,ny});
            int v=id(nx,ny);
            nx=from[v].first;
            ny=from[v].second;
        }
        reverse(r.begin(),r.end());
        Stats ns=s;
        int n=r.size();
        for(int j=0;j<n-1;j++){
            ns.ans.push_back({r[j],r[j+1]});
            ns.costsum-=val(r[j].first,r[j].second,ns.b[r[j].first][r[j].second]);
            ns.costsum-=val(r[j+1].first,r[j+1].second,ns.b[r[j+1].first][r[j+1].second]);
            swap(ns.p[ns.b[r[j].first][r[j].second]],ns.p[ns.b[r[j+1].first][r[j+1].second]]);
            swap(ns.b[r[j].first][r[j].second],ns.b[r[j+1].first][r[j+1].second]);
            ns.costsum+=val(r[j].first,r[j].second,ns.b[r[j].first][r[j].second]);
            ns.costsum+=val(r[j+1].first,r[j+1].second,ns.b[r[j+1].first][r[j+1].second]);
        }
        ns.costsum+=(n-1)*800;
        ss.push_back(ns);
    }
}

void solve(){
    init();
    input();
    int initval=0;
    for(int i=0;i<N;i++){
        for(int j=0;j<=i;j++){
            initval+=val(i,j,B[i][j]);
        }
    }
    Stats init=Stats{B,P,initval,vector<pair<pair<int,int>,pair<int,int>>>()};
    vector<Stats> ss(1,init);
    for(int i=0;i<M;i++){
        vector<Stats> nss;
        for(int j=0;j<min((int)ss.size(),W);j++){
            add_newstate(nss,ss[j],ss[j].p[i].first,ss[j].p[i].second);
        }
        sort(nss.begin(),nss.end());
        swap(ss,nss);
    }
    auto ans=ss[0].ans;
    int K=ans.size();
    cout << K << endl;
    for(int i=0;i<K;i++){
        cout << ans[i].first.first << " " << ans[i].first.second << " ";
        cout << ans[i].second.first << " " << ans[i].second.second << endl;
    }
    cerr << 100000-5*K << endl;
}

int main(){
    solve();
    return 0;
}