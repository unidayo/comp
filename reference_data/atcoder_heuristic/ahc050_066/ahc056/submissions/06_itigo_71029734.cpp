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

int dy[4]={-1,1,0,0},dx[4]={0,0,-1,1};
string rou="UDLR";

int neko=0;

// #include <atcoder/all>

class solve{
    public:
        int n,k,t;
        vector<vector<int>> v,h;
        vector<pair<int,int>> c;
        
        void input(){
            cin >> n >> k >> t;
            v.resize(n);
            rep(i,n){
                v[i].resize(n-1,0);
                string s;
                cin >> s;
                rep(j,n-1){
                    if (s[j]=='1') v[i][j]=1;
                }
            }
            h.resize(n-1);
            rep(i,n-1){
                h[i].resize(n,0);
                string s;
                cin >> s;
                rep(j,n){
                    if (s[j]=='1') h[i][j]=1;
                }
            }
            c.resize(k);
            rep(i,k) cin >> c[i].first >> c[i].second;
        }

        pair<int,string> route(vector<vector<int>> &m){
            vector<vector<vector<vector<tuple<int,int,int>>>>> d(k-1,vector<vector<vector<tuple<int,int,int>>>>(n,vector<vector<tuple<int,int,int>>>(n,vector<tuple<int,int,int>>(4,{inf,-1,-1}))));
            d[0][c[0].first][c[0].second][0]={0,-1,-1};
            priority_queue<tuple<int,int,int,int,int>, vector<tuple<int,int,int,int,int>>, greater<tuple<int,int,int,int,int>>> q;
            q.push({0,0,c[0].first,c[0].second,0});
            int yy=0;
            while(!q.empty()){
                auto [dis,r,f,g,type]=q.top();
                q.pop();
                if (dis!=get<0>(d[r][f][g][type])) continue;
                rep(i,4){
                    if (f+dy[i]<0 || f+dy[i]>=n) continue;
                    if (g+dx[i]<0 || g+dx[i]>=n) continue;
                    if (m[f][g]==1 && i!=type) continue;
                    if (i==0 && h[f-1][g]==1) continue;
                    if (i==1 && h[f][g]==1) continue;
                    if (i==2 && v[f][g-1]==1) continue;
                    if (i==3 && v[f][g]==1) continue;
                    if (m[f+dy[i]][g+dx[i]]==0){
                        if (get<0>(d[r][f+dy[i]][g+dx[i]][0])<=dis+1) continue;
                        d[r][f+dy[i]][g+dx[i]][0]={dis+1,f,g};
                        q.push({dis+1,r,f+dy[i],g+dx[i],0});
                        if (f+dy[i]==c[r+1].first && g+dx[i]==c[r+1].second && r!=k-2){
                            d[r+1][f+dy[i]][g+dx[i]][0]={dis+1,f,g};
                            q.push({dis+1,r+1,f+dy[i],g+dx[i],0});
                        }
                    } else if (m[f][g]==1){
                        if (get<0>(d[r][f+dy[i]][g+dx[i]][type])<=dis) continue;
                        d[r][f+dy[i]][g+dx[i]][type]={dis,f,g};
                        q.push({dis,r,f+dy[i],g+dx[i],type});
                        if (f+dy[i]==c[r+1].first && g+dx[i]==c[r+1].second && r!=k-2){
                            d[r+1][f+dy[i]][g+dx[i]][type]={dis,f,g};
                            q.push({dis,r+1,f+dy[i],g+dx[i],type});
                        }
                    } else {
                        rep(j,4){
                            if (get<0>(d[r][f+dy[i]][g+dx[i]][j])<=dis) continue;
                            d[r][f+dy[i]][g+dx[i]][j]={dis,f,g};
                            q.push({dis,r,f+dy[i],g+dx[i],j});
                            if (f+dy[i]==c[r+1].first && g+dx[i]==c[r+1].second && r!=k-2){
                                d[r+1][f+dy[i]][g+dx[i]][j]={dis,f,g};
                                q.push({dis,r+1,f+dy[i],g+dx[i],j});
                            }
                        }
                    }
                }
            }
            string ans;
            int y=c[k-1].first,x=c[k-1].second,t=k-2,type=0,di=inf;
            rep(i,4){
                auto [dis,f,g]=d[t][y][x][i];
                if (di>dis){
                    di=dis;
                    type=i;
                }
            }
            if (di==inf){
                return {inf,""};
            }
            while(true){
                if (y==c[0].first && x==c[0].second && t==0) break;
                auto [dis,f,g]=d[t][y][x][type];
                if (y==f-1){
                    ans.push_back('U');
                    type=0;
                }
                if (y==f+1){
                    ans.push_back('D');
                    type=1;
                }
                if (x==g-1){
                    ans.push_back('L');
                    type=2;
                }
                if (x==g+1){
                    ans.push_back('R');
                    type=3;
                }
                if (m[f][g]==0) type=0;
                if (f==c[t].first && g==c[t].second && t!=0){
                    t--;
                }
                y=f;x=g;
            }
            reverse(all(ans));
            return {di,ans};
        }

        pair<int,string> route2(vector<vector<int>> &m){
            vector<pair<int,int>> to_m;
            vector<vector<int>> from_m(n,vector<int>(n));
            int co=0;
            rep(i,n){
                rep(j,n){
                    if (m[i][j]==0){
                        to_m.push_back({i,j});
                        from_m[i][j]=co;
                        co++;
                    }
                }
            }
            vector<set<pair<int,int>>> is_route(k-1);
            vector<vector<tuple<int,string>>> e(co);
            rep(ia,to_m.size()){
                auto [ff,gg]=to_m[ia];
                int u=0;
                rep(i,4){
                    rep(j,4){
                        int f=ff,g=gg;
                        queue<pair<int,int>> qq;
                        if (f+dy[i]<0 || f+dy[i]>=n) continue;
                        if (g+dx[i]<0 || g+dx[i]>=n) continue;
                        if (i==0 && h[f-1][g]==1) continue;
                        if (i==1 && h[f][g]==1) continue;
                        if (i==2 && v[f][g-1]==1) continue;
                        if (i==3 && v[f][g]==1) continue;
                        f+=dy[i];g+=dx[i];
                        qq.push({f,g});
                        while(true){
                            if (m[f][g]==0) break;
                            if (f+dy[j]<0 || f+dy[j]>=n) break;
                            if (g+dx[j]<0 || g+dx[j]>=n) break;
                            if (j==0 && h[f-1][g]==1) break;
                            if (j==1 && h[f][g]==1) break;
                            if (j==2 && v[f][g-1]==1) break;
                            if (j==3 && v[f][g]==1) break;
                            f+=dy[j];g+=dx[j];
                            qq.push({f,g});
                        }
                        if (m[f][g]==1) continue;
                        vector<int> r(k-1,0);
                        string ss;
                        ss.push_back(rou[i]);
                        rep(ii,qq.size()-1) ss.push_back(rou[j]);
                        while(!qq.empty()){
                            auto [fa,ga]=qq.front();
                            qq.pop();
                            rep(ii,k-1){
                                if (c[ii+1].first==fa && c[ii+1].second==ga) r[ii]=1;
                            }
                        }
                        rep(ii,k-1){
                            if (r[ii]==1){
                                is_route[ii].insert({ia,u});
                            }
                        }
                        e[ia].push_back({from_m[f][g],ss});
                        u++;
                    }
                }
            }
            vector<vector<tuple<int,int,int,int,int>>> d(k,vector<tuple<int,int,int,int,int>>(co,{inf,-1,-1,-1,inf}));
            d[0][from_m[c[0].first][c[0].second]]={0,-1,-1,-1,0};
            queue<tuple<int,int>> q;
            q.push({0,from_m[c[0].first][c[0].second]});
            while(!q.empty()){
                auto [dis,id]=q.front();
                q.pop();
                rep(ia,e[id].size()){
                    int ne=get<0>(e[id][ia]);
                    int di=dis;
                    if (dis!=k-1){
                        if (is_route[dis].find({id,ia})!=is_route[dis].end()) di++;
                    }
                    if (get<0>(d[di][ne])!=inf) {
                        if (get<0>(d[di][ne])!=get<0>(d[dis][id])+1) continue;
                        if (get<4>(d[di][ne])>get<4>(d[dis][id])+get<1>(e[id][ia]).size()){
                            d[di][ne]={get<0>(d[dis][id])+1,dis,id,ia,get<4>(d[dis][id])+get<1>(e[id][ia]).size()};
                        }
                        continue;
                    }
                    d[di][ne]={get<0>(d[dis][id])+1,dis,id,ia,get<4>(d[dis][id])+get<1>(e[id][ia]).size()};
                    q.push({di,ne});
                }
            }

            string ans;
            int id=-1,t=k-1,di=inf,cal_dis;
            rep(i,co){
                if (get<0>(d[k-1][i])<di){
                    cal_dis=get<4>(d[k-1][i]);
                    di=get<0>(d[k-1][i]);
                    id=i;
                }
            }
            if (id==-1){
                return {inf,""};
            }
            while(true){
                auto [dd,dir,iid,ia,dist]=d[t][id];
                if (dir==-1) break;
                auto [nx,st]=e[iid][ia];
                if (t==k-1){
                    auto [f,g]=to_m[id];
                    rep(i,st.size()-(abs(f-c[k-1].first)+abs(g-c[k-1].second))){}
                    for (int i=(abs(f-c[k-1].first)+abs(g-c[k-1].second));i<st.size();i++){
                        ans.push_back(st[st.size()-1-i]);
                    }
                } else {
                    rep(i,st.size()){
                        ans.push_back(st[st.size()-1-i]);
                    }
                }
                t=dir;id=iid;
            }
            reverse(all(ans));
            return {di,ans};
        }

        bool valid_route(vector<vector<int>> &m){
            vector<vector<int>> d(n,vector<int>(n,0));
            queue<pair<int,int>> q;
            q.push({c[0].first,c[0].second});
            d[c[0].first][c[0].second]=1;
            while(!q.empty()){
                auto [ff,gg]=q.front();
                q.pop();
                rep(i,4){
                    rep(j,4){
                        int f=ff,g=gg;
                        queue<pair<int,int>> qq;
                        if (f+dy[i]<0 || f+dy[i]>=n) continue;
                        if (g+dx[i]<0 || g+dx[i]>=n) continue;
                        if (i==0 && h[f-1][g]==1) continue;
                        if (i==1 && h[f][g]==1) continue;
                        if (i==2 && v[f][g-1]==1) continue;
                        if (i==3 && v[f][g]==1) continue;
                        f+=dy[i];g+=dx[i];
                        qq.push({f,g});
                        while(true){
                            if (m[f][g]==0) break;
                            if (f+dy[j]<0 || f+dy[j]>=n) break;
                            if (g+dx[j]<0 || g+dx[j]>=n) break;
                            if (j==0 && h[f-1][g]==1) break;
                            if (j==1 && h[f][g]==1) break;
                            if (j==2 && v[f][g-1]==1) break;
                            if (j==3 && v[f][g]==1) break;
                            f+=dy[j];g+=dx[j];
                            qq.push({f,g});
                        }
                        if (m[f][g]==1) continue;
                        if (d[f][g]==0){
                            q.push({f,g});
                        }
                        while(!qq.empty()){
                            auto [fa,ga]=qq.front();
                            qq.pop();
                            d[fa][ga]=1;
                        }
                    }
                }
            }
            bool ok=true;
            rep(i,k){
                if (d[c[i].first][c[i].second]==0) ok=false;
            }
            return ok;
        }

        bool is_strongly_connected(const vector<vector<int>> &e) {
            int n = e.size();
            vector<int> order;
            vector<int> used(n, 0);
            order.reserve(n);

            // 1st DFS: 帰りがけ順を取る
            auto dfs1 = [&](auto &&dfs1, int v) -> void {
                used[v] = 1;
                for (int to : e[v]) if (!used[to]) dfs1(dfs1, to);
                order.push_back(v);
            };

            for (int i = 0; i < n; i++) if (!used[i]) dfs1(dfs1, i);

            // 転置グラフを作る
            vector<vector<int>> re(n);
            for (int i = 0; i < n; i++) {
                for (int to : e[i]) re[to].push_back(i);
            }

            // 2nd DFS on reversed graph
            fill(used.begin(), used.end(), 0);

            int comp_cnt = 0;

            auto dfs2 = [&](auto &&dfs2, int v) -> void {
                used[v] = 1;
                for (int to : re[v]) if (!used[to]) dfs2(dfs2, to);
            };

            // order の逆から辿る
            for (int i = n - 1; i >= 0; i--) {
                int v = order[i];
                if (!used[v]) {
                    dfs2(dfs2, v);
                    comp_cnt++;
                    if (comp_cnt > 1) return false;  // 早期終了可能
                }
            }

            return comp_cnt == 1;
        }

        bool valid_route2(vector<vector<int>> &m){
            vector<pair<int,int>> to_m;
            vector<vector<int>> from_m(n,vector<int>(n));
            int co=0;
            rep(i,n){
                rep(j,n){
                    if (m[i][j]==0){
                        to_m.push_back({i,j});
                        from_m[i][j]=co;
                        co++;
                    }
                }
            }
            vector<vector<int>> e(co);
            rep(ia,to_m.size()){
                auto [ff,gg]=to_m[ia];
                int u=0;
                rep(i,4){
                    rep(j,4){
                        int f=ff,g=gg;
                        queue<pair<int,int>> qq;
                        if (f+dy[i]<0 || f+dy[i]>=n) continue;
                        if (g+dx[i]<0 || g+dx[i]>=n) continue;
                        if (i==0 && h[f-1][g]==1) continue;
                        if (i==1 && h[f][g]==1) continue;
                        if (i==2 && v[f][g-1]==1) continue;
                        if (i==3 && v[f][g]==1) continue;
                        f+=dy[i];g+=dx[i];
                        qq.push({f,g});
                        while(true){
                            if (m[f][g]==0) break;
                            if (f+dy[j]<0 || f+dy[j]>=n) break;
                            if (g+dx[j]<0 || g+dx[j]>=n) break;
                            if (j==0 && h[f-1][g]==1) break;
                            if (j==1 && h[f][g]==1) break;
                            if (j==2 && v[f][g-1]==1) break;
                            if (j==3 && v[f][g]==1) break;
                            f+=dy[j];g+=dx[j];
                            qq.push({f,g});
                        }
                        if (m[f][g]==1) continue;
                        vector<int> r(k-1,0);
                        string ss;
                        ss.push_back(rou[i]);
                        rep(ii,qq.size()-1) ss.push_back(rou[j]);
                        while(!qq.empty()){
                            auto [fa,ga]=qq.front();
                            qq.pop();
                            rep(ii,k-1){
                                if (c[ii+1].first==fa && c[ii+1].second==ga) r[ii]=1;
                            }
                        }
                        e[ia].push_back(from_m[f][g]);
                        u++;
                    }
                }
            }
            return is_strongly_connected(e);
        }

        struct result{
            public:
                int p,q,t,si;
                vector<vector<int>> col;
                vector<tuple<int,int,int,int,char>> a;

                result(int p_, int q_, int t_,
                const vector<vector<int>>& col_,
                const vector<tuple<int, int, int, int, char>>& a_, int si_)
                : p(p_), q(q_), t(t_), si(si_), col(col_), a(a_) {}

                void output(int n){
                    cout << p << " " << q << " " << a.size() << endl;
                    rep(i,n){
                        rep(j,n) cout << col[i][j] << " ";
                        cout << endl;
                    }
                    for (auto [f1,g1,f2,g2,dir]:a) cout << f1 << " " << g1 << " " << f2 << " " << g2 << " " << dir << endl;
                }
        };

        result cal2(vector<vector<int>> &m, int dif,bool out=false){
            auto [dis,ans]=route2(m);
            if (ans.size()>t || ans.size()==0){
                result res(inf,0,inf,{},{},inf);
                return res;
            }
            result res(100,100,ans.size(),{},{},dis);
            return res;
        }

        result cal(vector<vector<int>> &m, int dif,bool out=false){
            auto [dis,ans]=route2(m);
            if (ans.size()>t || ans.size()==0){
                result res(inf,0,inf,{},{},inf);
                return res;
            }
            result best_res(inf,0,inf,{},{},inf);
            ld st_clock=clock();
            int times=max(1,1000/dis);
            ld remain_clock=(CLOCKS_PER_SEC*1.95-clock())/(ld)times;
            int last_result=0;
            rep(ani,times){
                int y=c[0].first,x=c[0].second;
                vector<vector<int>> col(n,vector<int>(n,-1));
                vector<vector<int>> pre(n,vector<int>(n,-1));
                vector<int> to_col,type,fix;
                vector<char> dir;
                int ac=1;
                to_col.push_back(-1);
                type.push_back(0);
                fix.push_back(2);
                dir.push_back(ans[0]);
                col[y][x]=0;
                pre[y][x]=0;
                int o=0;
                vector<int> sum(4,0);
                rep(i,ans.size()-1){
                    int ny=y,nx=x;
                    if (ans[i]=='D') ny++;
                    if (ans[i]=='U') ny--;
                    if (ans[i]=='R') nx++;
                    if (ans[i]=='L') nx--;
                    if (m[ny][nx]==0){
                        dir.push_back(ans[i+1]);
                        if (pre[ny][nx]==-1){
                            col[ny][nx]=ac;
                        } else {
                            to_col[pre[ny][nx]]=ac;
                        }
                        pre[ny][nx]=ac;
                        to_col.push_back(-1);
                        if (o==0) fix.push_back(0);
                        else fix.push_back(1);
                        int ty=0;
                        rep(j,4){
                            if (rou[j]==ans[i]) ty=j;
                        }
                        if (o==0){
                            int mi=inf;
                            rep(j,4){
                                if (mi>sum[j]){
                                    ty=j;
                                    mi=sum[j];
                                }
                            }
                        }
                        type.push_back(ty);
                        sum[ty]++;
                        o=0;
                        ac++;
                    } else {
                        o=1;
                    }
                    y=ny;x=nx;
                }
                vector<vector<vector<vector<int>>>> stock(4,vector<vector<vector<int>>>(4,vector<vector<int>>(4)));
                vector<int> stock_type(type.size(),0);
                rep(i,type.size()){
                    int u;
                    rep(j,4){
                        if (rou[j]==dir[i]) u=j;
                    }
                    int ne;
                    if (i!=type.size()-1) ne=type[i+1];
                    else {
                        rep(j,4){
                            if (rou[j]==ans[ans.size()-1]) ne=j;
                        }
                    }
                    stock[u][type[i]][ne].push_back(i);
                    stock_type[i]=u*16+type[i]*4+ne;
                }

                vector<int> sum2(4,0);
                rep(i,type.size()){
                    if (i==0) continue;
                    if (fix[i]!=0) sum2[type[i]]++;
                }

                sum[0]++;

                int q=0,best=inf,b=1;
                for (int i=3;i<=300;i++){
                    int Q=0;
                    rep(j,4){
                        Q+=(sum[j]-1)/(i*0.4)+1;
                    }
                    if (best>i+Q){
                        best=i+Q;
                        b=i;
                    }
                }
                // b*=1.5;
                if (last_result>=1) b-=(last_result+1)/2;
                if (last_result<=-1) b+=(-last_result+1)/2;

                vector<int> hash,hash_st(4),q_c(4);
                rep(j,4){
                    hash_st[j]=hash.size();
                    int Q=(sum[j]-1)/b+1;
                    q_c[j]=Q;
                    q+=Q;
                    rep(ii,Q) hash.push_back(j);
                }
                vector<int> s(q,0);
                rep(i,4) sum[i]=0;
                rep(i,type.size()){
                    int ty=type[i];
                    if (i==0) ty=0;
                    type[i]=(sum[ty]/b)+hash_st[ty];
                    s[type[i]]++;
                    sum[ty]++;
                }
                
                vector<vector<pair<int,int>>> color_row(type.size(),vector<pair<int,int>>(b));
                vector<int> letter(type.size(),-1);
                rep(i,type.size()){
                    int seed=letter[i];
                    if (seed==-1) seed=rand();
                    mt19937 rng(seed);
                    rep(j,b/2) color_row[i][j]={rng()%10000,j};
                    rep(j,b-b/2) color_row[i][b/2+j]={200000+j,j+b/2};
                    sort(all(color_row[i]));
                    if (to_col[i]!=-1){
                        letter[to_col[i]]=stock_type[i];
                    }
                }
                vector<int> color(type.size(),-1);
                vector<vector<int>> used(q,vector<int>(b,0));
                vector<vector<int>> fix_col(q,vector<int>(b,-1));
                vector<set<int>> same_type(type.size()+1);
                rep(i,type.size()+1) same_type[i].insert(i);
                while(clock()<st_clock+remain_clock*(ani+1)){
                // while(false){
                    if (rand()%3==0){
                        int ra=rand()%(ac);
                        int ty=rand()%q;
                        if (hash[ty]!=hash[type[ra]]) continue;
                        if (s[type[ra]]<=s[ty]) continue;
                        bool ok=true;
                        int co=0;
                        set<int> cc2;
                        for (auto it=same_type[ra].begin();it!=same_type[ra].end();it++){
                            if (*it==type.size()) continue;
                            if (*it==0) ok=false;
                            if (color[*it]!=-1){
                                if (used[ty][color[*it]]!=0) ok=false;
                                cc2.insert(color[*it]);
                            } else {
                                co++;
                            }
                        }
                        // if (cc2.size()>1) continue;
                        if (s[type[ra]]<s[ty]+co+cc2.size()) continue;
                        if (!ok) continue;
                        int pre_type=type[ra];
                        for (auto it=same_type[ra].begin();it!=same_type[ra].end();it++){
                            if (*it==type.size()) continue;
                            type[*it]=ty;
                            if (color[*it]!=-1){
                                if (fix_col[pre_type][color[*it]]!=-1){
                                    fix_col[ty][color[*it]]=fix_col[pre_type][color[*it]];
                                    fix_col[pre_type][color[*it]]=-1;
                                }
                                used[pre_type][color[*it]]=0;
                                used[ty][color[*it]]=1;
                            }
                        }
                        s[pre_type]-=co+cc2.size();
                        s[ty]+=co+cc2.size();
                    } else if (rand()%2==0){
                        int ra=rand()%(ac);
                        int rb=rand()%(ac);
                        if (fix[ra]==2 || fix[rb]==2) continue;
                        if (hash[type[ra]]!=hash[type[rb]]) continue;
                        if (type[ra]==type[rb]) continue;
                        int type1=type[ra],type2=type[rb];
                        bool ok=true;
                        int co=0,co2=0;
                        set<int> cc2,cc3;
                        for (auto it=same_type[ra].begin();it!=same_type[ra].end();it++){
                            if (*it==type.size()) continue;
                            if (*it==0) ok=false;
                            if (color[*it]!=-1){
                                if (used[type2][color[*it]]!=0) ok=false;
                                cc2.insert(color[*it]);
                            } else {
                                co++;
                            }
                        }
                        for (auto it=same_type[rb].begin();it!=same_type[rb].end();it++){
                            if (*it==type.size()) continue;
                            if (*it==0) ok=false;
                            if (color[*it]!=-1){
                                if (used[type1][color[*it]]!=0) ok=false;
                                cc3.insert(color[*it]);
                            } else {
                                co2++;
                            }
                        }
                        // if (cc2.size()>1) continue;
                        // if (cc3.size()>1) continue;
                        if (!ok) continue;
                        if (max(s[type1],s[type2])<max(s[type1]-co-cc2.size()+co2+cc3.size(),s[type2]-co2-cc3.size()+co+cc2.size())) continue;
                        for (auto it=same_type[ra].begin();it!=same_type[ra].end();it++){
                            if (*it==type.size()) continue;
                            type[*it]=type2;
                            if (color[*it]!=-1){
                                if (fix_col[type1][color[*it]]!=-1){
                                    fix_col[type2][color[*it]]=fix_col[type1][color[*it]];
                                    fix_col[type1][color[*it]]=-1;
                                }
                                used[type1][color[*it]]=0;
                                used[type2][color[*it]]=1;
                            }
                        }
                        for (auto it=same_type[rb].begin();it!=same_type[rb].end();it++){
                            if (*it==type.size()) continue;
                            type[*it]=type1;
                            if (color[*it]!=-1){
                                if (fix_col[type2][color[*it]]!=-1){
                                    fix_col[type1][color[*it]]=fix_col[type2][color[*it]];
                                    fix_col[type2][color[*it]]=-1;
                                }
                                used[type2][color[*it]]=0;
                                used[type1][color[*it]]=1;
                            }
                        }
                        s[type1]+=-co-cc2.size()+co2+cc3.size();
                        s[type2]+=-co2-cc3.size()+co+cc2.size();
                    } else {
                        int ka=rand()%4,kb=rand()%4,kc=rand()%4;
                        if (stock[ka][kb][kc].size()<=1) continue;
                        if (clock()<st_clock+remain_clock*((ld)ani+(ld)0.1) && stock[ka][kb][kc].size()<=4) continue;
                        if (clock()<st_clock+remain_clock*((ld)ani+(ld)0.2) && stock[ka][kb][kc].size()<=3) continue;
                        if (clock()<st_clock+remain_clock*((ld)ani+(ld)0.4) && stock[ka][kb][kc].size()<=2) continue;
                        int sii=stock[ka][kb][kc].size();
                        int ra=stock[ka][kb][kc][0];
                        if (clock()>st_clock+remain_clock*((ld)ani+(ld)0.5)){
                            ra=stock[ka][kb][kc][rand()%sii];
                        }
                        int rb=stock[ka][kb][kc][rand()%sii];
                        // if (clock()<st_clock+remain_clock*((ld)ani-0.2+(ld)0.8*(ra+rb)/(ac*2))) continue;
                        if (color[ra]==color[rb] && color[ra]!=-1 && type[ra]!=type[rb]){
                            if (ra!=ac-1 && rb!=ac-1){
                                if (type[ra+1]!=type[rb+1]) continue;
                            } else {
                                continue;
                            }
                            int col1=-1,col2=-1;
                            if (to_col[ra]!=-1){
                                col1=color[to_col[ra]];
                            }
                            if (to_col[rb]!=-1){
                                col2=color[to_col[rb]];
                            }
                            if (col1!=-1 && col2!=-1 && col1!=col2) continue;
                            if (to_col[ra]==rb || to_col[rb]==ra) continue;
                            bool ok=true;
                            int now_b=(sum[hash[type[ra]]]-1)/q_c[hash[type[ra]]]+1;
                            rep(i,4){
                                if (now_b<(sum[i]-1)/q_c[i]+1) ok=false;
                            }
                            if (!ok) continue;
                            
                            int this_col=-1,this_type=-1;
                            ok=true;
                            int co2=0;
                            set<int> cc2;
                            set<int> cc3;
                            for (auto it=same_type[ra].begin();it!=same_type[ra].end();it++){
                                if (*it==type.size()) continue;
                                if (*it==0) ok=false;
                                if (color[*it]!=-1){
                                    if (color[*it]==color[ra]) continue;
                                    if (used[type[rb]][color[*it]]!=0) ok=false;
                                    cc2.insert(color[*it]);
                                } else {
                                    co2++;
                                }
                            }
                            if (s[type[rb]]+co2+cc2.size()-1>s[type[rb]]) continue;
                            if (ok){
                                this_col=color[rb];
                                this_type=type[rb];
                            } else {
                                ok=true;
                                co2=0;
                                for (auto it=same_type[rb].begin();it!=same_type[rb].end();it++){
                                    if (*it==type.size()) continue;
                                    if (*it==0) ok=false;
                                    if (color[*it]!=-1){
                                        if (color[*it]==color[rb]) continue;
                                        if (used[type[ra]][color[*it]]!=0) ok=false;
                                        cc3.insert(color[*it]);
                                    } else {
                                        co2++;
                                    }
                                }
                                if (s[type[ra]]+co2+cc3.size()-1>s[type[ra]]) continue;
                                if (ok){
                                    this_col=color[ra];
                                    this_type=type[ra];
                                } else {
                                    continue;
                                }
                            }
                            
                            int next_col=-1;
                            if (fix_col[type[ra]][this_col]!=-1 && fix_col[type[rb]][this_col]!=-1 && fix_col[type[ra]][this_col]!=fix_col[type[rb]][this_col]) continue;
                            int fix_c=-1;
                            if (same_type[ra].size()>1) continue;
                            if (same_type[rb].size()>1) continue;
                            set<int> new_type;
                            for (auto it=same_type[ra].begin();it!=same_type[ra].end();it++){
                                new_type.insert(*it);
                            }
                            for (auto it=same_type[rb].begin();it!=same_type[rb].end();it++){
                                new_type.insert(*it);
                            }
                            ok=true;
                            for (auto it=new_type.begin();it!=new_type.end();it++){
                                if (color[*it]!=-1){
                                    if (fix_col[type[*it]][color[*it]]!=-1) ok=false;
                                }
                            }
                            if (!ok) continue;
                            // if (fix_col[type[ra]][this_col]!=-1) fix_c=fix_col[type[ra]][this_col];
                            // if (fix_col[type[rb]][this_col]!=-1) fix_c=fix_col[type[rb]][this_col];
                            if (fix_col[type[ra]][this_col]!=-1) continue;
                            if (fix_col[type[rb]][this_col]!=-1) continue;
                            if (to_col[ra]!=-1 && to_col[rb]!=-1){
                                if (type[to_col[ra]]==type[to_col[rb]]) continue;
                            }
                            if (fix_c!=-1){
                                if (col1!=fix_c && col1!=-1) continue;
                                if (col2!=fix_c && col2!=-1) continue;
                                if (col1==-1){
                                    if (to_col[ra]!=-1){
                                        if (used[type[to_col[ra]]][fix_c]==1 || (this_type==type[to_col[ra]] && fix_c==this_col)) continue;
                                    }
                                }
                                if (col2==-1){
                                    if (to_col[rb]!=-1){
                                        if (used[type[to_col[rb]]][fix_c]==1 || (this_type==type[to_col[rb]] && fix_c==this_col)) continue;
                                    }
                                }
                                next_col=fix_c;
                            } else if (col1==-1 && col2==-1){
                                if (to_col[ra]!=-1 && to_col[rb]!=-1){
                                    rep(i,b){
                                        if (used[type[to_col[ra]]][color_row[to_col[ra]][i].second]==0 && used[type[to_col[rb]]][color_row[to_col[ra]][i].second]==0){
                                            if (this_type==type[to_col[ra]] && this_col==color_row[to_col[ra]][i].second) continue;
                                            if (this_type==type[to_col[rb]] && this_col==color_row[to_col[ra]][i].second) continue;
                                            next_col=color_row[to_col[ra]][i].second;
                                            break;
                                        }
                                    }
                                    if (next_col==-1) continue;
                                } else if (to_col[ra]!=-1){
                                    rep(i,b){
                                        if (used[type[to_col[ra]]][color_row[to_col[ra]][i].second]==0){
                                            if (this_type==type[to_col[ra]] && this_col==color_row[to_col[ra]][i].second) continue;
                                            next_col=color_row[to_col[ra]][i].second;
                                            break;
                                        }
                                    }
                                    if (next_col==-1) continue;
                                } else if (to_col[rb]!=-1){
                                    rep(i,b){
                                        if (used[type[to_col[rb]]][color_row[to_col[rb]][i].second]==0){
                                            if (this_type==type[to_col[rb]] && this_col==color_row[to_col[rb]][i].second) continue;
                                            next_col=color_row[to_col[rb]][i].second;
                                            break;
                                        }
                                    }
                                    if (next_col==-1) continue;
                                }
                            } else if (col1!=-1 && col2!=-1) {
                                next_col=col1;
                            } else if (col1!=-1){
                                next_col=col1;
                                if (to_col[rb]!=-1){
                                    if (used[type[to_col[rb]]][col1]==1 || (this_type==type[to_col[rb]] && next_col==this_col)) continue;
                                }
                            } else if (col2!=-1){
                                next_col=col2;
                                if (to_col[ra]!=-1){
                                    if (used[type[to_col[ra]]][col2]==1 || (this_type==type[to_col[ra]] && next_col==this_col)) continue;
                                }
                            }
                            if (abs(ra-rb)<=1) continue;
                            for (auto it=new_type.begin();it!=new_type.end();it++){
                                same_type[*it]=new_type;
                            }
                            if (type[ra]!=this_type){
                                s[type[ra]]--;
                                sum[hash[type[ra]]]--;
                            } else {
                                s[type[rb]]--;
                                sum[hash[type[rb]]]--;
                            }
                            for (auto it=new_type.begin();it!=new_type.end();it++){
                                type[*it]=this_type;
                            }
                            set<int> new_type2;
                            for (auto it=same_type[ra+1].begin();it!=same_type[ra+1].end();it++){
                                new_type2.insert(*it);
                            }
                            for (auto it=same_type[rb+1].begin();it!=same_type[rb+1].end();it++){
                                new_type2.insert(*it);
                            }
                            for (auto it=new_type2.begin();it!=new_type2.end();it++){
                                same_type[*it]=new_type2;
                            }
                            // }
                            color[ra]=this_col;
                            color[rb]=this_col;
                            if (to_col[ra]!=-1){
                                // fix[to_col[ra]]=2;
                                color[to_col[ra]]=next_col;
                                used[type[to_col[ra]]][next_col]=1;
                            }
                            if (to_col[rb]!=-1){
                                // fix[to_col[rb]]=2;
                                color[to_col[rb]]=next_col;
                                used[type[to_col[rb]]][next_col]=1;
                            }
                            fix_col[type[ra]][this_col]=next_col;
                            continue;
                        }
                        if (type[ra]!=type[rb]) continue;
                        if (ra!=ac-1 && rb!=ac-1){
                            if (type[ra+1]!=type[rb+1]) continue;
                        } else {
                            continue;
                        }
                        // if (dir[ra]!=dir[rb]) continue;
                        if (ra==rb) continue;
                        if (color[ra]!=-1 && color[rb]!=-1) continue;
                        int col1=-1,col2=-1;
                        if (to_col[ra]!=-1){
                            col1=color[to_col[ra]];
                        }
                        if (to_col[rb]!=-1){
                            col2=color[to_col[rb]];
                        }
                        if (col1!=-1 && col2!=-1 && col1!=col2) continue;
                        if (to_col[ra]==rb || to_col[rb]==ra) continue;
                        bool ok=true;
                        int now_b=(sum[hash[type[ra]]]-1)/q_c[hash[type[ra]]]+1;
                        rep(i,4){
                            if (now_b<(sum[i]-1)/q_c[i]+1) ok=false;
                        }
                        if (!ok) continue;

                        int this_col=-1;
                        if (color[ra]==-1 && color[rb]==-1){
                            rep(i,b){
                                if (used[type[ra]][color_row[ra][i].second]==0){
                                    this_col=color_row[ra][i].second;
                                    break;
                                }
                            }
                            if (this_col==-1) continue;
                        } else if (color[ra]!=-1){
                            this_col=color[ra];
                        } else {
                            this_col=color[rb];
                        }

                        int next_col=-1;
                        if (to_col[ra]!=-1 && to_col[rb]!=-1){
                            if (type[to_col[ra]]==type[to_col[rb]]) continue;
                        }
                        if (fix_col[type[ra]][this_col]!=-1){
                            if (col1!=fix_col[type[ra]][this_col] && col1!=-1) continue;
                            if (col2!=fix_col[type[ra]][this_col] && col2!=-1) continue;
                            if (col1==-1){
                                if (to_col[ra]!=-1){
                                    if (used[type[to_col[ra]]][fix_col[type[ra]][this_col]]==1 || (type[ra]==type[to_col[ra]] && fix_col[type[ra]][this_col]==this_col)) continue;
                                }
                            }
                            if (col2==-1){
                                if (to_col[rb]!=-1){
                                    if (used[type[to_col[rb]]][fix_col[type[ra]][this_col]]==1 || (type[ra]==type[to_col[rb]] && fix_col[type[ra]][this_col]==this_col)) continue;
                                }
                            }
                            next_col=fix_col[type[ra]][this_col];
                        } else if (col1==-1 && col2==-1){
                            if (to_col[ra]!=-1 && to_col[rb]!=-1){
                                rep(i,b){
                                    if (used[type[to_col[ra]]][color_row[to_col[ra]][i].second]==0 && used[type[to_col[rb]]][color_row[to_col[ra]][i].second]==0){
                                        if (type[ra]==type[to_col[ra]] && this_col==color_row[to_col[ra]][i].second) continue;
                                        if (type[ra]==type[to_col[rb]] && this_col==color_row[to_col[ra]][i].second) continue;
                                        next_col=color_row[to_col[ra]][i].second;
                                        break;
                                    }
                                }
                                if (next_col==-1) continue;
                            } else if (to_col[ra]!=-1){
                                rep(i,b){
                                    if (used[type[to_col[ra]]][color_row[to_col[ra]][i].second]==0){
                                        if (type[ra]==type[to_col[ra]] && this_col==color_row[to_col[ra]][i].second) continue;
                                        next_col=color_row[to_col[ra]][i].second;
                                        break;
                                    }
                                }
                                if (next_col==-1) continue;
                            } else if (to_col[rb]!=-1){
                                rep(i,b){
                                    if (used[type[to_col[rb]]][color_row[to_col[rb]][i].second]==0){
                                        if (type[ra]==type[to_col[rb]] && this_col==color_row[to_col[rb]][i].second) continue;
                                        next_col=color_row[to_col[rb]][i].second;
                                        break;
                                    }
                                }
                                if (next_col==-1) continue;
                            }
                        } else if (col1!=-1 && col2!=-1) {
                            next_col=col1;
                        } else if (col1!=-1){
                            next_col=col1;
                            if (to_col[rb]!=-1){
                                if (used[type[to_col[rb]]][col1]==1 || (type[ra]==type[to_col[rb]] && next_col==this_col)) continue;
                            }
                        } else if (col2!=-1){
                            next_col=col2;
                            if (to_col[ra]!=-1){
                                if (used[type[to_col[ra]]][col2]==1 || (type[ra]==type[to_col[ra]] && next_col==this_col)) continue;
                            }
                        }
                        set<int> new_type;
                        for (auto it=same_type[ra].begin();it!=same_type[ra].end();it++){
                            new_type.insert(*it);
                        }
                        for (auto it=same_type[rb].begin();it!=same_type[rb].end();it++){
                            new_type.insert(*it);
                        }
                        for (auto it=new_type.begin();it!=new_type.end();it++){
                            same_type[*it]=new_type;
                        }
                        // if (ra+1<type.size() && rb+1<type.size()){
                        set<int> new_type2;
                        for (auto it=same_type[ra+1].begin();it!=same_type[ra+1].end();it++){
                            new_type2.insert(*it);
                        }
                        for (auto it=same_type[rb+1].begin();it!=same_type[rb+1].end();it++){
                            new_type2.insert(*it);
                        }
                        for (auto it=new_type2.begin();it!=new_type2.end();it++){
                            same_type[*it]=new_type2;
                        }
                        color[ra]=this_col;
                        color[rb]=this_col;
                        s[type[ra]]--;
                        sum[hash[type[ra]]]--;
                        used[type[ra]][this_col]=1;
                        if (to_col[ra]!=-1){
                            // fix[to_col[ra]]=2;
                            color[to_col[ra]]=next_col;
                            used[type[to_col[ra]]][next_col]=1;
                        }
                        if (to_col[rb]!=-1){
                            // fix[to_col[rb]]=2;
                            color[to_col[rb]]=next_col;
                            used[type[to_col[rb]]][next_col]=1;
                        }
                        fix_col[type[ra]][this_col]=next_col;
                    }
                }

                vector<vector<int>> u(q,vector<int>(b,0));
                rep(i,type.size()){
                    if (color[i]!=-1){
                        u[type[i]][color[i]]=1;
                    }
                }
                rep(i,type.size()){
                    if (color[i]==-1){
                        rep(j,b){
                            if (u[type[i]][j]==0){
                                u[type[i]][j]=1;
                                color[i]=j;
                                break;
                            }
                        }
                    }
                    color[i]++;
                }

                int qq1=0,qq2=0;
                rep(i,q) qq1=max(qq1,s[i]);
                rep(i,q){
                    rep(j,b){
                        if (u[i][j]==1) qq2=max(qq2,j+1);
                    }
                }

                rep(i,q){
                    rep(j,b) u[i][j]=0;
                }

                vector<tuple<int,int,int,int,char>> a;
                rep(i,type.size()){
                    int to_type;
                    if (i!=type.size()-1) to_type=type[i+1];
                    else{
                        rep(j,4){
                            if (rou[j]==ans[ans.size()-1]) to_type=hash_st[j];
                        }
                    }
                    int co;
                    if (fix_col[type[i]][color[i]-1]!=-1) co=fix_col[type[i]][color[i]-1]+1;
                    else if (to_col[i]!=-1) co=color[to_col[i]];
                    else co=0;

                    if (u[type[i]][color[i]-1]==0) {
                        a.push_back({color[i],type[i],co,to_type,dir[i]});
                        u[type[i]][color[i]-1]=1;
                    }
                }
                rep(i,q){
                    a.push_back({0,i,0,i,rou[hash[i]]});
                }
                rep(i,n){
                    rep(j,n){
                        if (col[i][j]!=-1) col[i][j]=color[col[i][j]];
                        else col[i][j]=0;;
                    }
                }
                result res(qq2+1,q,ans.size(),col,a,a.size());
                if (best_res.p+best_res.q>res.p+res.q) best_res=res;
                last_result+=qq2-q;
            }
            return best_res;
        }

        void sol(){
            int T;
            vector<vector<int>> m(n,vector<int>(n,0));
            vector<pair<int,int>> block;
            rep(i,n){
                rep(j,n){
                    if ((i+j)%2!=(c[0].first+c[0].second)%2) m[i][j]=1;
                    else if (i!=c[0].first || j!=c[0].second) block.push_back({i,j});
                }
            }
            result best_res=cal2(m,1);
            if (best_res.p+best_res.q==inf){
                block.clear();
                rep(i,n){
                    rep(j,n) m[i][j]=0;
                }
                rep(i,n){
                    rep(j,n){
                        if ((i+j)%2==(c[0].first+c[0].second)%2 && (i!=c[0].first || j!=c[0].second)) m[i][j]=1;
                        else if (i!=c[0].first || j!=c[0].second) block.push_back({i,j});
                    }
                }
                best_res=cal2(m,1);
            }
            T=best_res.t;

            int ct=block.size();

            vector<vector<int>> mm=m;
            rep(i,40){
                while(true){
                    if (block.size()<=ct*(39-i)/40) break;
                    int l=block.size();
                    int ra=rand()%l;
                    auto [f,g]=block[ra];
                    mm[f][g]=1;
                    bool ok=valid_route2(mm);
                    if (!valid_route(mm)) ok=false;
                    if (!ok) mm[f][g]=0;
                    block.erase(block.begin()+ra);
                }
                result res=cal2(mm,1);
                if (res.p+res.q<=best_res.p+best_res.q){
                    m=mm;
                    best_res=res;
                } else {
                    break;
                }
            }

            while(clock()<CLOCKS_PER_SEC*1.3){
                if (rand()%4==0){
                    int f=rand()%n,g=rand()%n;
                    if (c[0].first==f && c[0].second==g) continue;
                    m[f][g]=1-m[f][g];
                    if (!valid_route(m)){
                        m[f][g]=1-m[f][g];
                        continue;
                    }
                    result res=cal2(m,1);
                    ld t2=0;
                    if ((ld)t/(ld)T<1.25) t2=min((ld)50.0,(ld)pow(5,(1.3-(ld)t/(ld)T)*10));
                    if (res.t<=t && ((res.si*100+res.t*t2)<=(best_res.si*100+best_res.t*t2))){
                        best_res=res;
                    } else {
                        m[f][g]=1-m[f][g];
                    }
                } else {
                    int f1=rand()%n,g1=rand()%n;
                    int f=-1+rand()%3,g=-1+rand()%3;
                    int f2=f1+f,g2=g1+g;
                    if (f2<0 || f2>=n) continue;
                    if (g2<0 || g2>=n) continue;
                    if (c[0].first==f1 && c[0].second==g1) continue;
                    if (c[0].first==f2 && c[0].second==g2) continue;
                    if (m[f1][g1]==m[f2][g2]) continue;
                    swap(m[f1][g1],m[f2][g2]);
                    if (!valid_route(m)){
                        swap(m[f1][g1],m[f2][g2]);
                        continue;
                    }
                    result res=cal2(m,1);
                    ld t2=0;
                    if ((ld)t/(ld)T<1.25) t2=min((ld)50.0,(ld)pow(5,(1.3-(ld)t/(ld)T)*10));
                    if (res.t<=t && ((res.si*100+res.t*t2)<=(best_res.si*100+best_res.t*t2))){
                        best_res=res;
                    } else {
                        swap(m[f1][g1],m[f2][g2]);
                    }
                }
            }
            best_res=cal(m,0,true);
            best_res.output(n);
        }
};

int main(){
    solve SOL;
    SOL.input();
    SOL.sol();
}
