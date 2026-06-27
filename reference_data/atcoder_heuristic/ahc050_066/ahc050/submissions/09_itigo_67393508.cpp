#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define ld long double
#define rep(i,n) for(int (i)=0;(i)<(n);(i)++)
#define Yes(q) cout << ((q) ? "Yes" : "No") << endl;

int dx[4]={0,1,0,-1},dy[4]={-1,0,1,0};

struct solve{
    public:
        short n,m;
        vector<vector<short>> s,ss;
        vector<vector<ld>> route;
        ld ran=(ld)1/(ld)4;
        
        void input(){
            cin >> n >> m;
            s.resize(n+2);
            rep(i,n+2) s[i].resize(n+2,1);
            route.resize(n);
            rep(i,n) route[i].resize(n,0);
            rep(i,n){
                rep(j,n){
                    char a;
                    cin >> a;
                    if (a=='.') s[i+1][j+1]=0;
                }
            }
            ss=s;
        }

        // 元のロジックを保持しつつ、小さな最適化を適用
        vector<vector<ld>> cal(vector<vector<ld>> &pos){
            vector<vector<ld>> ans(n,vector<ld>(n,0));
            rep(i,n){
                rep(j,n) route[i][j]=0;
            }
            short y,x;
            rep(i,n){
                rep(j,n){
                    if (s[i+1][j+1]==1 || pos[i][j] == 0) continue; // pos[i][j]==0の場合をスキップ
                    ld val = pos[i][j] * ran;
                    rep(k,4){
                        y=i+1;x=j+1;
                        while(true){
                            y+=dy[k];x+=dx[k];
                            if (s[y][x]==1){
                                y-=dy[k];x-=dx[k];
                                break;
                            }
                            route[y-1][x-1]+=val;
                        }
                        ans[y-1][x-1]+=val;
                    }
                }
            }
            return ans;
        }

        void greedy(){
            vector<vector<ld>> pos(n,vector<ld>(n));
            vector<pair<short,short>> best_ans;
            ld best_S=0;
            int iterations = 0;
            
            while(clock()<CLOCKS_PER_SEC*1.8){
                s=ss;
                vector<pair<short,short>> ans(n*n-m);
                ld S=0;
                
                // 初期化の最適化
                int free_cells = 0;
                rep(i,n){
                    rep(j,n){
                        if (s[i+1][j+1]==1) {
                            pos[i][j] = 0;
                        } else {
                            free_cells++;
                        }
                    }
                }
                ld initial_prob = (ld)1/(ld)free_cells;
                rep(i,n){
                    rep(j,n){
                        if (s[i+1][j+1]==0) pos[i][j] = initial_prob;
                    }
                }
                
                rep(turn,n*n-m){
                    pos=cal(pos);
                    
                    // 上位5つから選択する新しいロジック
                    vector<pair<ld,pair<short,short>>> candidates;
                    rep(i,n){
                        rep(j,n){
                            if (s[i+1][j+1]==1) continue;
                            ld score=pos[i][j];
                            score+=route[i][j]/200;
                            rep(k,4){
                                if (i+dy[k]<0 || j+dx[k]<0 || i+dy[k]>=n || j+dx[k]>=n) continue;
                                score-=pos[i+dy[k]][j+dx[k]]/600;
                            }
                            candidates.push_back({score, {i, j}});
                        }
                    }
                    
                    // スコアの昇順でソート（最小スコアが最良）
                    sort(candidates.begin(), candidates.end());
                    
                    // 上位5つから選択
                    int selectFrom = min(100, (int)candidates.size());
                    int selected = 0;
                    if (turn < 200 && selectFrom > 1) {
                        selected = rand() % selectFrom;
                    }
                    
                    short y = candidates[selected].second.first;
                    short x = candidates[selected].second.second;
                    
                    ans[turn]={y,x};
                    S+=pos[y][x]*turn;
                    pos[y][x]=0;
                    s[y+1][x+1]=1;
                }
                iterations++;
                if (S>best_S){
                    best_S=S;
                    best_ans=ans;
                }
            }
            cerr << "Iterations: " << iterations << ", Best Score: " << best_S << endl;
            for (auto [f,g]:best_ans) cout << f << " " << g << endl;
        }
};

int main(){
    solve S;
    S.input();
    S.greedy();
}