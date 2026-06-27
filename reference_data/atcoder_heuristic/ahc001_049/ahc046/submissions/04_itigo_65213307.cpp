# pragma GCC target("avx2")
# pragma GCC optimize("O3")
# pragma GCC optimize("unroll-loops")

#include <bits/stdc++.h>
using namespace std;
#define rep(i,n) for(int (i)=0;(i)<(n);(i)++)
#define ll long long
#define pp pair<ll,ll>
#define ld long double
#define all(a) (a).begin(),(a).end()
#define mk make_pair
#define difine define
#define Yes(q) cout << ((q) ? "Yes" : "No") << endl;
int inf=1000000000;
constexpr ll INF=2e18;
constexpr ll mod=1000000007;
constexpr ll MOD=998244353;
int dx[4]={1,0,-1,0},dy[4]={0,-1,0,1};
int dx1[8]={0,0,-1,1,1,1,-1,-1},dy1[8]={-1,1,0,0,1,-1,1,-1};
 
 
// #include <atcoder/all>
// typedef atcoder::modint998244353 mint;
// typedef atcoder::modint1000000007 mint;

struct sol{
    public:
        int n,m;
        vector<pair<int,int>> a;
        
        void input(){
            cin >> n >> m;
            a.resize(m);
            rep(i,m) cin >> a[i].first >> a[i].second;
        }

        void change_dir(vector<vector<vector<pair<int,int>>>> &dir,vector<vector<int>> &b,int y,int x){
            for(int i=x-1;i>=0;i--){
                if (b[y][i]==1) break;
                dir[0][y][i]={y,x-1};
            }
            for(int i=y+1;i<n;i++){
                if (b[i][x]==1) break;
                dir[1][i][x]={y+1,x};
            }
            for(int i=x+1;i<n;i++){
                if (b[y][i]==1) break;
                dir[2][y][i]={y,x+1};
            }
            for(int i=y-1;i>=0;i--){
                if (b[i][x]==1) break;
                dir[3][i][x]={y-1,x};
            }
        }

        pair<int,vector<vector<int>>> cal_turn(vector<vector<int>> pt,bool output=false){
            vector<vector<int>> b(n,vector<int>(n,0));
            vector<vector<vector<pair<int,int>>>> dir(4,vector<vector<pair<int,int>>>(n,vector<pair<int,int>>(n)));
            rep(i,n){
                rep(j,n){
                    dir[0][i][j]={i,n-1};
                    dir[1][i][j]={0,j};
                    dir[2][i][j]={i,0};
                    dir[3][i][j]={n-1,j};
                }
            }
            int turn=0;
            vector<vector<tuple<int,int,int>>> dis(n,vector<tuple<int,int,int>>(n));
            vector<vector<int>> route(n,vector<int>(n,0));
            for (int t=1;t<m;t++){
                // rep(j,4){
                //     if (pt[j][t-1]==1){
                //         turn++;
                //         if (b[a[t-1].first+dy[j]][a[t-1].second+dx[j]]==1) return inf;
                //         b[a[t-1].first+dy[j]][a[t-1].second+dx[j]]=1;
                //         change_dir(dir,b,a[t-1].first+dy[j],a[t-1].second+dx[j]);
                //         if (output){
                //             if (j==0) cout << "A R" << endl;
                //             if (j==1) cout << "A U" << endl;
                //             if (j==2) cout << "A L" << endl;
                //             if (j==3) cout << "A D" << endl;
                //         }
                //     }
                // }
                rep(i,n){
                    rep(j,n) dis[i][j]={inf,-1,-1};
                }
                dis[a[t-1].first][a[t-1].second]={0,-1,-1};
                queue<pair<int,int>> q;
                q.push({a[t-1].first,a[t-1].second});
                while(!q.empty()){
                    auto [f,g]=q.front();
                    q.pop();
                    rep(i,4){
                        if (f+dy[i]<0 || f+dy[i]>=n) continue;
                        if (g+dx[i]<0 || g+dx[i]>=n) continue;
                        if (b[f+dy[i]][g+dx[i]]==1) continue;
                        if (get<0>(dis[f+dy[i]][g+dx[i]])==inf){
                            dis[f+dy[i]][g+dx[i]]={get<0>(dis[f][g])+1,f,g};
                            q.push({f+dy[i],g+dx[i]});
                        }
                    }
                    rep(i,4){
                        if (b[dir[i][f][g].first][dir[i][f][g].second]==0 && get<0>(dis[dir[i][f][g].first][dir[i][f][g].second])==inf){
                            dis[dir[i][f][g].first][dir[i][f][g].second]={get<0>(dis[f][g])+1,f,g};
                            q.push({dir[i][f][g].first,dir[i][f][g].second});
                        }
                    }
                    if (get<0>(dis[a[t].first][a[t].second])!=inf) break;
                }
                if (get<0>(dis[a[t].first][a[t].second])==inf) return {inf,b};
                turn+=get<0>(dis[a[t].first][a[t].second]);
                if (true){
                    vector<pair<char,char>> ans;
                    vector<pair<int,int>> pos;
                    int y=a[t].first,x=a[t].second;
                    // cerr << y << " " << x << endl;
                    while(get<0>(dis[y][x])!=0){
                        auto [_,f,g]=dis[y][x];
                        if (f+1==y) ans.push_back({'M','D'});
                        else if (f-1==y) ans.push_back({'M','U'});
                        else if (g+1==x) ans.push_back({'M','R'});
                        else if (g-1==x) ans.push_back({'M','L'});
                        else if (f<y) {
                            for (int i=f+1;i<=y;i++) route[i][g]++;
                            ans.push_back({'S','D'});
                        }
                        else if (f>y) {
                            for (int i=y;i<f;i++) route[i][g]++;
                            ans.push_back({'S','U'});
                        }
                        else if (g<x) {
                            for (int i=g+1;i<=x;i++) route[f][i]++;
                            ans.push_back({'S','R'});
                        }
                        else if (g>x) {
                            for (int i=x;i<g;i++) route[f][i]++;
                            ans.push_back({'S','L'});
                        }
                        pos.push_back({y,x});
                        if (ans[ans.size()-1].first=='M') route[y][x]++;
                        y=f;x=g;
                    }
                    pos.push_back({y,x});
                    reverse(all(ans));
                    reverse(all(pos));
                    rep(ii,ans.size()){
                        auto [f,g]=pos[ii];
                        auto [ff,gg]=ans[ii];
                        rep(i,4){
                            if (f+dy[i]<0 || f+dy[i]>=n) continue;
                            if (g+dx[i]<0 || g+dx[i]>=n) continue;
                            if (pt[f+dy[i]][g+dx[i]]==0) continue;
                            if (b[f+dy[i]][g+dx[i]]==1) continue;
                            if (route[f+dy[i]][g+dx[i]]>0) continue;
                            turn++;
                            b[f+dy[i]][g+dx[i]]=1;
                            change_dir(dir,b,f+dy[i],g+dx[i]);
                            if (output){
                                // cerr << route[f+dy[i]][g+dx[i]] << " " << f+dy[i] << " " << g+dx[i] << endl;
                                if (i==0) cout << "A R" << endl;
                                if (i==1) cout << "A U" << endl;
                                if (i==2) cout << "A L" << endl;
                                if (i==3) cout << "A D" << endl;
                            }
                        }
                        if (output) cout << ff << " " << gg << endl;
                        auto [f2,g2]=pos[ii+1];
                        if (f<f2){
                            for (int i=f+1;i<=f2;i++) route[i][g]--;
                        }
                        if (f>f2){
                            for (int i=f2;i<f;i++) route[i][g]--;
                        }
                        if (g<g2){
                            for (int i=g+1;i<=g2;i++) route[f][i]--;
                        }
                        if (g>g2){
                            for (int i=g2;i<g;i++) route[f][i]--;
                        }
                        // if (output) cerr << f << " pos " << g << endl;
                    }
                }
            }
            return {turn,b};
        }

        void annealing(){
            vector<vector<int>> pt(n,vector<int>(n,0));
            auto [score,tt]=cal_turn(pt);
            int best_score=score;
            vector<vector<int>> best_pt=pt;
            int co=0;
            ld start_time=clock();
            ld start_temp=0.01,end_temp=0.00001;
            int kk=1;

            while(clock()<CLOCKS_PER_SEC*1.9){
                if (clock()>CLOCKS_PER_SEC*0.5*kk){
                    kk++;
                    rep(i,n){
                        rep(j,n) pt[i][j]=0;
                        score=inf/2;
                    }
                }
                if (rand()%4==0){
                    int ra=rand()%n;
                    int ri=rand()%n;
                    pt[ri][ra]=1-pt[ri][ra];
                    ld temp = start_temp + (end_temp - start_temp) * (clock()-start_time) / (CLOCKS_PER_SEC*1.9);
                    auto [sc,b]=cal_turn(pt);
                    ld prob = exp((score-sc)/temp);
                    if (prob > (rand()%INF)/(double)INF){
                        score=sc;
                        pt=b;
                        if (score<best_score){
                            best_score=score;
                            best_pt=pt;
                        }
                    } else {
                        pt[ri][ra]=1-pt[ri][ra];
                    }
                    co++;
                } else {
                    int ra1=rand()%n;
                    int ri1=rand()%n;
                    int ra2=rand()%n;
                    int ri2=rand()%n;
                    if (pt[ri1][ra1]==pt[ri2][ra2]) continue;
                    swap(pt[ri1][ra1],pt[ri2][ra2]);
                    ld temp = start_temp + (end_temp - start_temp) * (clock()-start_time) / (CLOCKS_PER_SEC*1.9);
                    auto [sc,b]=cal_turn(pt);
                    ld prob = exp((score-sc)/temp);
                    if (prob > (rand()%INF)/(double)INF){
                        score=sc;
                        pt=b;
                        if (score<best_score){
                            best_score=score;
                            best_pt=pt;
                        }
                    } else {
                        swap(pt[ri1][ra1],pt[ri2][ra2]);
                    }
                    co++;
                }
            }
            cal_turn(best_pt,true);
        }
};

 
int main() {
   sol S;
   S.input();
   S.annealing();
}