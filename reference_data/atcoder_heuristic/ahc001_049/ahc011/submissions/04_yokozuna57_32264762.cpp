#include <bits/stdc++.h>
#include <atcoder/mincostflow>
using namespace std;
using namespace atcoder;

typedef long long ll;
typedef unsigned long long ull;

#define mp make_pair
#define fr first
#define sc second

template<class T> T T_INF(){ return 1000000000000000000; }
template<> int T_INF<int>(){ return 1000000000; }

int CNT=0;
struct UF{
    vector<int> par,r;
    UF(int n){ init(n); }
    UF(){}
    void init(int n=-1){
        if(n==-1)n=par.size();
        par.resize(n);
        r.resize(n);
        iota(par.begin(),par.end(),0);
        fill(r.begin(),r.end(),0);
    }
    int find(int x){
        if(par[x]==x)return x;
        return par[x]=find(par[x]);
    }
    bool same(int x,int y){ return find(x)==find(y); }
    void unit(int x,int y){
        if(same(x,y))return;
        x=find(x);
        y=find(y);
        if(r[x]>r[y])swap(x,y);
        par[x]=y;
        if(r[x]==r[y])r[y]++;
    }
};

vector<vector<bool>> rotate(const vector<vector<bool>> &v,int d){
    if(d==0)return v;
    int n=v.size();
    static vector<vector<bool>> ret(n,vector<bool>(n));
    switch(d){
        case 1:
            ret=v;
            for(int i=0;i<n;i++)for(int j=0;j<n;j++)ret[i][j]=v[n-1-j][i];
            return ret;
        case 2:
            ret=v;
            for(int i=0;i<n;i++)for(int j=0;j<n;j++)ret[i][j]=v[n-1-i][n-1-j];
            return ret;
        case 3:
            ret=v;
            for(int i=0;i<n;i++)for(int j=0;j<n;j++)ret[i][j]=v[j][n-1-i];
            return ret;
    }
    return ret;
}

const int dx[4]={0,-1,0,1};
const int dy[4]={-1,0,1,0};
int n,T;

inline int to_id(pair<int,int> p){
    return p.fr*n+p.sc;
}
inline pair<int,int> nex(pair<int,int> p,int d){
    p.fr+=dx[d];
    p.sc+=dy[d];
    return p;
}
inline pair<int,int> nex(pair<pair<int,int>,int> p){
    return nex(p.fr,p.sc);
}

int bfst[10][10];
pair<int,int> bfst_loc[102];

struct conf{
    int t[10][10];
    conf(){
        for(int i=0;i<n;i++)for(int j=0;j<n;j++)t[i][j]=0;
    }
    UF get_uf(){
        UF uf(n*n);
        for(int i=0;i<n;i++)for(int j=0;j+1<n;j++){
            if(t[i][j]&4){
                uf.unit(i*n+j,i*n+j+1);
            }
        }
        for(int i=0;i+1<n;i++)for(int j=0;j<n;j++){
            if(t[i][j]&8){
                uf.unit(i*n+j,(i+1)*n+j);
            }
        }
        return uf;
    }
    void get_bfst(){
        for(int i=0;i<n;i++)for(int j=0;j<n;j++)bfst[i][j]=0;
        int k=0,sz=1;
        bfst_loc[0]=mp(0,t[0][0]==0?1:0);
        while(k<sz){
            for(int d=0;d<4;d++){
                //CNT++;
                if(t[bfst_loc[k].fr][bfst_loc[k].sc]&(1<<d)){
                    int nx=bfst_loc[k].fr+dx[d]; //if(nx<0||nx>=n)continue;
                    int ny=bfst_loc[k].sc+dy[d]; //if(ny<0||ny>=n)continue;
                    if(bfst[nx][ny]==0){
                        bfst[nx][ny]=1;
                        bfst_loc[sz++]=mp(nx,ny);
                    }
                }
            }
            k++;
        }
    }
    void rotate(int d){
        if(d==0)return;
        if(d==2){
            for(int i=0;i<n/2;i++)for(int j=0;j<n;j++)swap(t[i][j],t[n-1-i][n-1-j]);
            if(n%2==1)for(int j=0;j<n/2;j++)swap(t[n/2][j],t[n/2][n-1-j]);
            return;
        }
        int t_[10][10];
        for(int i=0;i<n;i++)for(int j=0;j<n;j++)t_[i][j]=t[i][j];
        switch(d){
            case 1:
                for(int i=0;i<n;i++)for(int j=0;j<n;j++)t[i][j]=t_[n-1-j][i];
                break;
            /*case 2:
                for(int i=0;i<n;i++)for(int j=0;j<n;j++)t[i][j]=t_[n-1-i][n-1-j];
                break;*/
            case 3:
                for(int i=0;i<n;i++)for(int j=0;j<n;j++)t[i][j]=t_[j][n-1-i];
                break;
        }
    }
    void mir(){
        for(int i=0;i<n;i++)for(int j=0;i+j<n-1;j++)swap(t[i][j],t[n-1-j][n-1-i]);
    }
    void out(){
        cerr<<"$$$$$$$$$$$$$$$$$$$$"<<endl;
        for(int i=0;i<n;i++){
            string s[3]={"","",""};
            for(int j=0;j<n;j++){
                s[0]+=string(" ")+string(((t[i][j]>>1)&1)?"|":" ")+string(" ");
                s[1]+=string(((t[i][j]>>0)&1)?"-":" ")+string("+")+string(((t[i][j]>>2)&1)?"-":" ");
                s[2]+=string(" ")+string(((t[i][j]>>3)&1)?"|":" ")+string(" ");
            }
            for(int j=0;j<3;j++)cerr<<s[j]<<endl;
        }
        cerr<<"$$$$$$$$$$$$$$$$$$$$"<<endl;
    }
};

pair<int,int> que_me[102];
int ret_me[102];
pair<int,pair<int,int>> move_empty(conf &init,const vector<vector<bool>> &frozen,const vector<vector<bool>> &tar,int sz,int x0,int y0,vector<int>& ret,int sx,int sy){
    /*int sx=-1,sy=-1;
    for(int i=0;i<n;i++)for(int j=0;j<n;j++){
        if(init.t[i][j]==0){
            sx=i; sy=j; break;
        }
    }*/
    const int inf=1000;
    //queue<pair<int,int>> que;
    int que_sz=0,que_nex=0;
    static vector<vector<bool>> used(n,vector<bool>(n,false));
    static vector<vector<int>> dist(n,vector<int>(n,inf));
    for(int i=0;i<n;i++)for(int j=0;j<n;j++){ used[i][j]=false; dist[i][j]=inf; }
    dist[sx][sy]=0;
    que_me[0]=mp(sx,sy);
    que_sz++;
    //que.push(mp(sx,sy));
    int x=-1,y=-1;
    while(que_nex<que_sz&&x==-1){
        auto p=que_me[que_nex++];
        if(used[p.fr][p.sc])continue;
        used[p.fr][p.sc]=true;
        for(int k=0;k<4;k++){
            int nx=p.fr+dx[k]; if(nx<0||nx>=n)continue;
            int ny=p.sc+dy[k]; if(ny<0||ny>=n)continue;
            if(!frozen[nx][ny]&&dist[nx][ny]==inf){
                dist[nx][ny]=dist[p.fr][p.sc]+1;
                if(0<=nx-x0&&nx-x0<sz&&0<=ny-y0&&ny-y0<sz&&!tar[nx-x0][ny-y0]){
                    x=nx; y=ny;
                    break;
                }
                que_me[que_sz++]=mp(nx,ny);
                //que.push(mp(nx,ny));
            }
        }
    }
    //assert(dist[x][y]!=inf);
    //vector<int> ret(dist[x][y]);
    pair<int,int> loc(x,y);
    int pre=init.t[x][y]; init.t[x][y]=0;
    while(loc!=mp(sx,sy)){
        for(int k=0;k<4;k++){
            int nx=loc.fr+dx[k]; if(nx<0||nx>=n)continue;
            int ny=loc.sc+dy[k]; if(ny<0||ny>=n)continue;
            if(dist[nx][ny]==dist[loc.fr][loc.sc]-1){
                //ret[dist[nx][ny]]=k^2;
                ret_me[dist[nx][ny]]=k^2;
                swap(init.t[nx][ny],pre);
                loc=mp(nx,ny);
                break;
            }
        }
    }
    for(int i=0;i<dist[x][y];i++)ret.push_back(ret_me[i]);
    /*for(auto d: ret){
        int nx=loc.fr+dx[d];
        int ny=loc.sc+dy[d];
        init.t[loc.fr][loc.sc]=init.t[nx][ny];
        //swap(init.t[loc.fr][loc.sc],init.t[nx][ny]);
        loc=mp(nx,ny);
    }
    init.t[x][y]=0;*/
    return mp(dist[x][y],mp(x,y));
}

struct puzzle_solver{
    virtual vector<int> run(conf init,conf goal){return vector<int>();};
};

/*pair<int,int> neighbor(const conf &c,const vector<vector<bool>>& frozen, int x, int y, int t){
    pair<int,pair<int,int>> tar(1000,mp(-1,-1));
    for(int i=0;i<x;i++){
        for(int j=0;j<y;j++){
            if(!frozen[i][j]&&c.t[i][j]==t&&tar.fr>x+y-i-j)tar=mp(x+y-i-j,mp(i,j));
        }
        for(int j=y;j<n;j++){
            if(!frozen[i][j]&&c.t[i][j]==t&&tar.fr>x-y-i+j)tar=mp(x-y-i+j,mp(i,j));
        }
    }
    for(int i=x;i<n;i++){
        for(int j=0;j<y;j++){
            if(!frozen[i][j]&&c.t[i][j]==t&&tar.fr>-x+y+i-j)tar=mp(-x+y+i-j,mp(i,j));
        }
        for(int j=y;j<n;j++){
            if(!frozen[i][j]&&c.t[i][j]==t&&tar.fr>-x-y+i+j)tar=mp(-x-y+i+j,mp(i,j));
        }
    }
    return tar.sc;
}*/

pair<int,int> neighbor(const conf &c,const vector<vector<bool>>& frozen,int sz,int x,int y,pair<int,int> g,int t){
    pair<int,pair<int,int>> tar(1000,mp(-1,-1));
    for(int i=0;i<sz;i++)for(int j=0;j<sz;j++){
        if(!frozen[i][j]&&c.t[i+x][j+y]==t)tar=min(tar,mp(abs(i-g.fr)+abs(j-g.sc),mp(i,j)));
    }
    /*for(int i=0;i<n;i++)for(int j=0;j<n;j++){
        if(!frozen[i][j]&&c.t[i][j]==t)tar=min(tar,mp(abs(i-x)+abs(j-y),mp(i,j)));
    }*/
    return tar.sc;
}

struct sol2: puzzle_solver{
    vector<vector<pair<int,int>>> dec[11];
    vector<vector<vector<bool>>> frozen[11];
    vector<vector<int>> dist[11];
    vector<vector<pair<int,int>>> cons[11];
    
    vector<int> max_s_memo[11];
    
    struct shortcut{
        int sz,num,dx,dy;
    };
    vector<vector<shortcut>> sho[11];
    int to_int_(int *b){
        int len=8,ret=0;
        for(int i=0;i<len;i++){
            int cnt=0;
            for(int j=0;j<i;j++)if(b[j]<b[i])cnt++;
            ret+=b[i]-cnt;
            ret*=len-i;
        }
        return ret;
    }
    int to_int(int sz,int num,int *a){
        int len=dec[sz][num].size();
        int ret=0;
        if(sz==3){
            int b[10];
            for(int i=0;i<=len;i++)b[i]=a[2*i]*sz+a[2*i+1];
            return to_int_(b);
        }
        for(int i=0;i<2*len+2;i++){ ret*=sz; ret+=a[i]; }
        return ret;
    }
    int nex(int sz,int num,int q,int d){
        if(sz==3){
            int len=dec[sz][num].size();
            int a[10]={};
            bool used[10]; for(int i=0;i<9;i++)used[i]=false;
            for(int i=len-1;i>=0;i--){
                a[i]=q%(len-i+1);
                q/=len-i+1;
            }
            for(int i=0;i<=len;i++){
                int k=a[i]; a[i]=0;
                while(used[a[i]]||k>0){
                    if(!used[a[i]])k--;
                    a[i]++;
                }
                used[a[i]]=true;
            }
            if(d==0){ if(a[len]%sz==0)return -1; a[len]--; }
            else if(d==1){ if(a[len]<sz)return -1; a[len]-=sz; }
            else if(d==2){ if(a[len]%sz==sz-1)return -1; a[len]++; }
            else { if(a[len]>=sz*sz-sz)return -1; a[len]+=sz; }
            for(int i=0;i<len;i++){
                if(a[i]==a[len]){
                    a[i]-=dx[d]*sz+dy[d];
                }
            }
            return to_int_(a);
        }
        int ret=q;
        int len=dec[sz][num].size();
        int a[10]; int SZ=sz*sz;
        for(int i=len;i>=0;i--){
            a[i]=q%SZ;
            q/=SZ;
        }
        if(d==0){ if(a[len]%sz==0)return -1; a[len]--; ret--; }
        else if(d==1){ if(a[len]<sz)return -1; a[len]-=sz; ret-=sz; }
        else if(d==2){ if(a[len]%sz==sz-1)return -1; a[len]++; ret++; }
        else { if(a[len]>=SZ-sz)return -1; a[len]+=sz; ret+=sz; }
        if(frozen[sz][num][a[len]/sz][a[len]%sz])return -1;
        for(int i=0;i<len;i++){
            if(a[i]==a[len]){
                int q_=dx[d]*sz+dy[d];
                for(int j=0;j<len-i;j++)q_*=sz*sz;
                ret-=q_;
            }
        }
        return ret;
    }
#define MAX_S 10000000
    int di[MAX_S],que[MAX_S];
    void init(){
        for(int i=7;i<=n;i++){
            if(i%2==1)dec[i].push_back(vector<pair<int,int>>{mp(0,0)});
            for(int j=i%2;j<i;j+=2){
                dec[i].push_back(vector<pair<int,int>>{mp(0,j),mp(0,j+1)});
            }
            if(i%2==0)dec[i].push_back(vector<pair<int,int>>{mp(1,i-1)});
            for(int j=2-i%2;j<i;j+=2){
                dec[i].push_back(vector<pair<int,int>>{mp(j,i-1),mp(j+1,i-1)});
            }
        }
        /*if(n>=9){
            dec[9][0]=vector<pair<int,int>>{mp(0,0),mp(0,1)};
            dec[9][1]=vector<pair<int,int>>{mp(0,2)};
        }*/
        /*if(n>=7){
            dec[7].push_back(vector<pair<int,int>>{mp(0,0),mp(0,1)});
            dec[7].push_back(vector<pair<int,int>>{mp(0,2),mp(0,3)});
            dec[7].push_back(vector<pair<int,int>>{mp(0,4),mp(0,5),mp(0,6)});
            dec[7].push_back(vector<pair<int,int>>{mp(1,6),mp(2,6),mp(3,6)});
            dec[7].push_back(vector<pair<int,int>>{mp(4,6),mp(5,6),mp(6,6)});
        }*/
        if(n>=6){
            dec[6].push_back(vector<pair<int,int>>{mp(0,0),mp(0,1),mp(0,2)});
            dec[6].push_back(vector<pair<int,int>>{mp(0,3),mp(0,4),mp(0,5)});
            dec[6].push_back(vector<pair<int,int>>{mp(1,5),mp(2,5)});
            dec[6].push_back(vector<pair<int,int>>{mp(3,5),mp(4,5),mp(5,5)});
            //dec[6].push_back(vector<pair<int,int>>{mp(1,5)});
            //dec[6].push_back(vector<pair<int,int>>{mp(2,5),mp(3,5)});
            //dec[6].push_back(vector<pair<int,int>>{mp(4,5),mp(5,5)});
        }
        /*if(n>=5){
            dec[5].push_back(vector<pair<int,int>>{mp(0,0),mp(0,1),mp(0,2)});
            dec[5].push_back(vector<pair<int,int>>{mp(0,3),mp(0,4),mp(1,4)});
            dec[5].push_back(vector<pair<int,int>>{mp(2,4),mp(3,4),mp(4,4)});
        }*/
        if(n>=5){
            dec[5].push_back(vector<pair<int,int>>{mp(0,0),mp(0,1)});
            dec[5].push_back(vector<pair<int,int>>{mp(0,2),mp(0,3),mp(0,4)});
            dec[5].push_back(vector<pair<int,int>>{mp(1,4),mp(2,4),mp(3,4),mp(4,4)});
        }
        if(n>=4){
            dec[4].push_back(vector<pair<int,int>>{mp(0,0),mp(0,1),mp(0,2),mp(0,3)});
            dec[4].push_back(vector<pair<int,int>>{mp(1,3),mp(2,3),mp(3,3)});
        }
        if(n>=3){
            dec[3].push_back(vector<pair<int,int>>());
            for(int i=0;i<3;i++)for(int j=0;j<3;j++){
                if(i+j==4)continue;
                dec[3][0].push_back(mp(i,j));
            }
        }
        for(int i=3;i<=n;i++){
            frozen[i].resize(dec[i].size());
            for(int j=0;j<dec[i].size();j++){
                frozen[i][j]=vector<vector<bool>>(i,vector<bool>(i,false));
                for(int k=0;k<j;k++)for(int l=0;l<dec[i][k].size();l++)frozen[i][j][dec[i][k][l].fr][dec[i][k][l].sc]=true;
            }
            dist[i].resize(dec[i].size());
            cons[i].resize(dec[i].size());
            sho[i].resize(dec[i].size());
        }
        if(n>=9)sho[9][0].push_back(shortcut{6,0,0,0});
        //if(n>=9)sho[9][1].push_back(shortcut{6,0,0,2});
        if(n>=7)sho[7][0].push_back(shortcut{6,0,0,0});
        if(n>=10)sho[10][3].push_back(shortcut{4,0,0,6});
        if(n>=9)sho[9][3].push_back(shortcut{4,0,0,5});
        if(n>=8)sho[8][2].push_back(shortcut{4,0,0,4});
        if(n>=7)sho[7][2].push_back(shortcut{4,0,0,3});
        if(n>=10)sho[10][8].push_back(shortcut{5,2,5,5});
        if(n>=9)sho[9][7].push_back(shortcut{5,2,4,4});
        if(n>=8)sho[8][6].push_back(shortcut{5,2,3,3});
        if(n>=7)sho[7][5].push_back(shortcut{5,2,2,2});
        //if(n>=10)sho[10][4].push_back(shortcut{5,1,0,5});
        //if(n>=8)sho[8][3].push_back(shortcut{5,1,0,3});
        //if(n>=10)sho[10][5].push_back(shortcut{6,2,0,4});
        //if(n>=8)sho[8][4].push_back(shortcut{6,2,0,2});
        for(int sz=3;sz<=n;sz++)for(int num=0;num<dec[sz].size();num++){
            auto& ta=dec[sz][num]; int len=ta.size();
            //auto& di=dist[sz][num];
            auto& co=cons[sz][num];
            int a[20];
            for(int i=0;i<10;i++){ a[2*i]=sz-1; a[2*i+1]=sz-1-i; }
            int max_s=to_int(sz,num,a); max_s_memo[sz].push_back(max_s);
            const int inf=100000000;
            //di.resize(max_s+10,inf);
            for(int i=0;i<max_s+10;i++)di[i]=inf;
            co.resize(max_s+10,mp(-1,-1));
            //vector<int> que(max_s+10);
            int nex_que=0,sz_que=0;
            for(int i=0;i<ta.size();i++){ a[2*i]=ta[i].fr; a[2*i+1]=ta[i].sc; }
            for(int i=0;i<sz;i++)for(int j=0;j<sz;j++){
                bool ok=!frozen[sz][num][i][j];
                for(int k=0;k<ta.size();k++)ok&=mp(i,j)!=ta[k];
                if(ok){
                    a[2*len]=i; a[2*len+1]=j;
                    int s=to_int(sz,num,a);
                    di[s]=0;
                    co[s].sc=-2;
                    que[sz_que++]=s;
                }
            }
            while(nex_que<sz_que){
                int q=que[nex_que++];
                for(int d=0;d<4;d++){
                    int nq=nex(sz,num,q,d);
                    if(nq!=-1&&di[nq]==inf){
                        di[nq]=di[q]+1;
                        co[nq]=mp(d^2,q);
                        que[sz_que++]=nq;
                    }
                }
            }
        }
        //make_UR__used=vector<vector<bool>>(n,vector<bool>(n));
    }
    //vector<vector<bool>> make_UR__used;
    pair<vector<int>,vector<int>> make_UR(conf &init,const conf &goal,int sz,int x,int y){
        vector<int> ret;
        vector<int> rec;
        for(int num=0;num<dec[sz].size();num++){
            int zx_,zy_;
            for(int i=0;i<sz;i++)for(int j=0;j<sz;j++)if(init.t[x+i][y+j]==0){ zx_=i; zy_=j; }
            if(sho[sz][num].size()>0){
                auto sh=sho[sz][num][0];
                pair<int,int> memo=mp(sz,num);
                bool OK=true;
                sz=sh.sz; num=sh.num; x+=sh.dx; y+=sh.dy;
                int len=dec[sz][num].size();
                int a[20]={};
                static vector<vector<bool>> used(n,vector<bool>(n,true));
                //for(int i=0;i<n;i++)for(int j=0;j<n;j++)used[i][j]=true;
                for(int i=0;i<sz;i++)for(int j=0;j<sz;j++)used[i][j]=frozen[sz][num][i][j];
                for(int k=0;k<len;k++){
                    /*auto p=neighbor(init,used,x+dec[sz][num][k].fr,y+dec[sz][num][k].sc,goal.t[x+dec[sz][num][k].fr][y+dec[sz][num][k].sc]);
                    if(p.fr==-1){ OK=false; break; }
                    used[p.fr][p.sc]=true;
                    a[2*k]=p.fr-x; a[2*k+1]=p.sc-y;*/
                    auto p=neighbor(init,used,sz,x,y,dec[sz][num][k],goal.t[x+dec[sz][num][k].fr][y+dec[sz][num][k].sc]);
                    if(p.fr==-1){ OK=false; break; }
                    used[p.fr][p.sc]=true;
                    a[2*k]=p.fr; a[2*k+1]=p.sc;
                }
                if(OK){
                    int cnt=0;
                    if(0<=zx_-sh.dx&&zx_-sh.dx<sh.sz&&0<=zy_-sh.dy&&zy_-sh.dy<sh.sz&&!frozen[sh.sz][sh.num][zx_-sh.dx][zy_-sh.dy]){
                        a[2*len]=zx_-sh.dx; a[2*len+1]=zy_-sh.dy;
                    }
                    else {
                        /*for(int i=0;i<n;i++)for(int j=0;j<n;j++)used[i][j]=true;
                        for(int i=0;i<sz;i++)for(int j=0;j<sz;j++)used[x+i][y+j]=frozen[sz][num][i][j];
                        for(int i=0;i<len;i++)used[x+a[2*i]][y+a[2*i+1]]=true;*/
                        static vector<vector<bool>> used_(n,vector<bool>(n,true)); for(int i=0;i<n;i++)for(int j=0;j<n;j++)used_[i][j]=true;
                        for(int i=0;i<memo.fr;i++)for(int j=0;j<memo.fr;j++)used_[x-sh.dx+i][y-sh.dy+j]=frozen[memo.fr][memo.sc][i][j];
                        for(int i=0;i<len;i++)used_[a[2*i]+x][a[2*i+1]+y]=true;
                        //auto ret_=move_empty(init,used_,used,ret);
                        auto ret_=move_empty(init,used_,frozen[sz][num],sz,x,y,ret,x+zx_-sh.dx,y+zy_-sh.dy);
                        cnt=ret_.fr;
                        a[2*len]=ret_.sc.fr-x; a[2*len+1]=ret_.sc.sc-y;
                    }
                    int s=to_int(sz,num,a);
                    int zx=x+a[2*len],zy=y+a[2*len+1];
                    while(cons[sz][num][s].fr!=-1)
                    {
                        auto p=cons[sz][num][s];
                        //assert(p.fr!=-1);
                        ret.push_back(p.fr);
                        s=p.sc;
                        int nx=zx+dx[p.fr],ny=zy+dy[p.fr];
                        swap(init.t[zx][zy],init.t[nx][ny]);
                        zx=nx; zy=ny;
                        cnt++;
                    }
                    rec.push_back(cnt); rec.push_back(0);
                    sz=memo.fr; num=memo.sc+1; x-=sh.dx; y-=sh.dy;
                    continue;
                }
                else {
                    sz=memo.fr; num=memo.sc; x-=sh.dx; y-=sh.dy;
                }
            }
            int len=dec[sz][num].size();
            int a[20]={};
            static vector<vector<bool>> used(n,vector<bool>(n,true));
            for(int i=0;i<sz;i++)for(int j=0;j<sz;j++)used[i][j]=frozen[sz][num][i][j];
            for(int k=0;k<len;k++){
                auto p=neighbor(init,used,sz,x,y,dec[sz][num][k],goal.t[x+dec[sz][num][k].fr][y+dec[sz][num][k].sc]);
                //assert(p.fr!=-1);
                used[p.fr][p.sc]=true;
                a[2*k]=p.fr; a[2*k+1]=p.sc;
            }
            a[2*len]=zx_; a[2*len+1]=zy_;
            if(sz<=4||(sz==5&&num==2)){
                int id[24]; for(int i=0;i<24;i++)id[i]=-1;
                bool ok=false;
                for(int i=0;i<sz;i++)for(int j=0;j<sz;j++){
                    if(!used[i][j]){
                        if(id[init.t[x+i][y+j]]!=-1){ ok=true; i=sz-1; break; }
                        else id[init.t[x+i][y+j]]=i*sz+j;
                    }
                }
                if(!ok){
                    int id2[24]; for(int i=0;i<24;i++)id2[i]=-1;
                    if(num+1<dec[sz].size()){
                        for(int i=0;i<sz;i++)for(int j=0;j<sz;j++){
                            if(!frozen[sz][num+1][i][j]){
                                id2[goal.t[x+i][y+j]]=i*sz+j;
                            }
                        }
                    }
                    else {
                        if(sz>3){
                            for(int i=1;i<sz;i++)for(int j=0;j<sz-1;j++){
                                id2[goal.t[x+i][y+j]]=i*sz+j;
                            }
                        }
                        else id2[0]=8;
                    }
                    for(int k=0;k<len;k++){
                        id2[16+k]=dec[sz][num][k].fr*sz+dec[sz][num][k].sc;
                        id[16+k]=a[2*k]*sz+a[2*k+1];
                    }
                    int sgn=id[0]/sz+id[0]%sz+id2[0]/sz+id2[0]%sz;
                    for(int i=0;i<24;i++){
                        if(id[i]!=id2[i]){
                            sgn++;
                            for(int j=i+1;j<24;j++){
                                if(id2[j]==id[i]){
                                    swap(id2[j],id2[i]);
                                    break;
                                }
                                //assert(j!=23);
                            }
                        }
                    }
                    if(sgn&1){
                        for(int i=0;i<len;i++){
                            int temp=id[goal.t[x+dec[sz][num][i].fr][y+dec[sz][num][i].sc]];
                            if(temp!=-1){
                                a[2*i]=temp/sz;
                                a[2*i+1]=temp%sz;
                                ok=true; break;
                            }
                        }
                        if(!ok){
                            for(int i=0;i<len;i++)for(int j=i+1;j<len;j++){
                                if(goal.t[x+dec[sz][num][i].fr][y+dec[sz][num][i].sc]==goal.t[x+dec[sz][num][j].fr][y+dec[sz][num][j].sc]){
                                    ok=true;
                                    swap(a[2*i],a[2*j]);
                                    swap(a[2*i+1],a[2*j+1]);
                                    i=len-1; break;
                                }
                            }
                        }
                    }
                }
            }
            int s=to_int(sz,num,a);
            int zx=x+a[2*len],zy=y+a[2*len+1];
            int cnt=0;
            while(cons[sz][num][s].fr!=-1)
            {
                auto p=cons[sz][num][s];
                //assert(p.fr!=-1);
                ret.push_back(p.fr);
                s=p.sc;
                int nx=zx+dx[p.fr],ny=zy+dy[p.fr];
                swap(init.t[zx][zy],init.t[nx][ny]);
                zx=nx; zy=ny;
                cnt++;
            }
            rec.push_back(cnt);
        }
        return mp(ret,rec);
    }
    int make_UR3(conf &init,const conf &goal,int sz,int x,int y,vector<int> &ret,int d0){
        //vector<int> ret;
        //vector<int> rec;
        int num=0;
            int zx_,zy_;
            for(int i=0;i<sz;i++)for(int j=0;j<sz;j++)if(init.t[x+i][y+j]==0){ zx_=i; zy_=j; }
            int len=dec[sz][num].size();
            int a[20]={};
            static vector<vector<bool>> used(n,vector<bool>(n,true));
            for(int i=0;i<sz;i++)for(int j=0;j<sz;j++)used[i][j]=frozen[sz][num][i][j];
            for(int k=0;k<len;k++){
                auto p=neighbor(init,used,sz,x,y,dec[sz][num][k],goal.t[x+dec[sz][num][k].fr][y+dec[sz][num][k].sc]);
                //assert(p.fr!=-1);
                used[p.fr][p.sc]=true;
                a[2*k]=p.fr; a[2*k+1]=p.sc;
            }
            a[2*len]=zx_; a[2*len+1]=zy_;
            if(sz<=4||(sz==5&&num==2)){
                int id[24]; for(int i=0;i<24;i++)id[i]=-1;
                bool ok=false;
                for(int i=0;i<sz;i++)for(int j=0;j<sz;j++){
                    if(!used[i][j]){
                        if(id[init.t[x+i][y+j]]!=-1){ ok=true; i=sz-1; break; }
                        else id[init.t[x+i][y+j]]=i*sz+j;
                    }
                }
                if(!ok){
                    int id2[24]; for(int i=0;i<24;i++)id2[i]=-1;
                    if(num+1<dec[sz].size()){
                        for(int i=0;i<sz;i++)for(int j=0;j<sz;j++){
                            if(!frozen[sz][num+1][i][j]){
                                id2[goal.t[x+i][y+j]]=i*sz+j;
                            }
                        }
                    }
                    else {
                        if(sz>3){
                            for(int i=1;i<sz;i++)for(int j=0;j<sz-1;j++){
                                id2[goal.t[x+i][y+j]]=i*sz+j;
                            }
                        }
                        else id2[0]=8;
                    }
                    for(int k=0;k<len;k++){
                        id2[16+k]=dec[sz][num][k].fr*sz+dec[sz][num][k].sc;
                        id[16+k]=a[2*k]*sz+a[2*k+1];
                    }
                    int sgn=id[0]/sz+id[0]%sz+id2[0]/sz+id2[0]%sz;
                    for(int i=0;i<24;i++){
                        if(id[i]!=id2[i]){
                            sgn++;
                            for(int j=i+1;j<24;j++){
                                if(id2[j]==id[i]){
                                    swap(id2[j],id2[i]);
                                    break;
                                }
                                //assert(j!=23);
                            }
                        }
                    }
                    if(sgn&1){
                        for(int i=0;i<len;i++){
                            int temp=id[goal.t[x+dec[sz][num][i].fr][y+dec[sz][num][i].sc]];
                            if(temp!=-1){
                                a[2*i]=temp/sz;
                                a[2*i+1]=temp%sz;
                                ok=true; break;
                            }
                        }
                        if(!ok){
                            for(int i=0;i<len;i++)for(int j=i+1;j<len;j++){
                                if(goal.t[x+dec[sz][num][i].fr][y+dec[sz][num][i].sc]==goal.t[x+dec[sz][num][j].fr][y+dec[sz][num][j].sc]){
                                    ok=true;
                                    swap(a[2*i],a[2*j]);
                                    swap(a[2*i+1],a[2*j+1]);
                                    i=len-1; break;
                                }
                            }
                        }
                    }
                }
            }
            int s=to_int(sz,num,a);
            int zx=x+a[2*len],zy=y+a[2*len+1];
            int cnt=0;
            while(cons[sz][num][s].fr!=-1)
            {
                auto p=cons[sz][num][s];
                //assert(p.fr!=-1);
                int temp=p.fr; temp+=4-d0; temp&=3;
                ret.push_back(temp);
                //ret.push_back(p.fr);
                s=p.sc;
                int nx=zx+dx[p.fr],ny=zy+dy[p.fr];
                swap(init.t[zx][zy],init.t[nx][ny]);
                zx=nx; zy=ny;
                cnt++;
            }
            //rec.push_back(cnt);
            return cnt;
    }
    auto run(conf &init,const conf &goal){
        int zx=-1,zy=-1;
        for(int i=0;i<n;i++)for(int j=0;j<n;j++)if(goal.t[i][j]==0){ zx=i; zy=j; }
        int x0,y0,d_;
        if(zx>=2){
            if(zy>=2){ x0=zx-2; y0=zy-2; d_=0; }
            else { x0=zx-2; y0=zy; d_=3; }
        }
        else {
            if(zy>=2){ x0=zx; y0=zy-2; d_=1; }
            else { x0=zx; y0=zy; d_=2; }
        }
        int x=0,y=0;
        vector<int> RET;
        vector<int> rec;
        vector<pair<pair<int,int>,pair<int,int>>> pos(n+1);
        auto rot=[&](int n_){
            int temp=x; x=y; y=n-n_-temp;
            temp=zx; zx=zy; zy=n-1-temp;
            temp=x0+2; x0=y0; y0=n-1-temp;
        };
        auto mir=[&](int n_){
            int temp=x; x=n-n_-y; y=n-n_-temp;
            temp=zx; zx=n-1-zy; zy=n-1-temp;
            temp=x0; x0=n-3-y0; y0=n-3-temp;
        };
        for(int n_=n;n_>3;n_--){
            pair<int,pair<pair<vector<int>,vector<int>>,conf>> best(-1,mp(mp(vector<int>(),vector<int>()),conf()));
            for(int d=0;d<4;d++)for(int e=0;e<2;e++){
                auto init_=init; init_.rotate(d);
                auto goal_=goal; goal_.rotate(d);
                for(int k=0;k<d;k++)rot(n_);
                if(e==1){ init_.mir(); goal_.mir(); mir(n_); }
                if(x0!=x&&y0+2!=y+n_-1){
                    auto ret=make_UR(init_,goal_,n_,x,y);
                    if(best.fr==-1||ret.fr.size()<best.sc.fr.fr.size()){
                        if(e==1){
                            for(auto &x: ret.fr)x=3-x;
                            init_.mir();
                        }
                        for(auto &x: ret.fr){ x+=4-d; x&=3; }
                        init_.rotate((4-d)&3);
                        best=mp(d,mp(ret,init_));
                        pos[n_]=mp(mp(d,e),mp(x,y));
                    }
                }
                if(e==1)mir(n_);
                for(int k=0;k<4-d;k++)rot(n_);
            }
            RET.insert(RET.end(),best.sc.fr.fr.begin(),best.sc.fr.fr.end());
            rec.insert(rec.end(),best.sc.fr.sc.begin(),best.sc.fr.sc.end());
            init=best.sc.sc;
            if(best.fr==0){ x++; }
            else if(best.fr==1){ x++; y++; }
            else if(best.fr==2){ y++; }
        }
        int n_=3;
        for(int d=d_;d<d_+1;d++)for(int e=0;e<1;e++){
            //auto init_=init; init_.rotate(d);
            init.rotate(d);
            auto goal_=goal; goal_.rotate(d);
            for(int k=0;k<d;k++)rot(n_);
            rec.push_back(make_UR3(init,goal_,n_,x,y,RET,d));
            //for(auto &x: ret.fr){ x+=4-d; x&=3; }
            //init_.rotate((4-d)&3);
            init.rotate((4-d)&3);
            //best=mp(d,mp(ret,init_));
            pos[n_]=mp(mp(d,e),mp(x,y));
            for(int k=0;k<4-d;k++)rot(n_);
        }
        pos[0]=mp(mp(-1,-1),mp(zx,zy));
        return mp(RET,mp(rec,pos));
    }
};

void add_edge(conf &c, pair<pair<int,int>,int> ed){
    c.t[ed.fr.fr][ed.fr.sc]^=1<<ed.sc;
    c.t[ed.fr.fr+dx[ed.sc]][ed.fr.sc+dy[ed.sc]]^=1<<(ed.sc^2);
}

unsigned int RAND(void) {
  static unsigned int y = 2463534242;
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}
void xor_shuffle(vector<pair<int,int>> &v){
    int n=v.size();
    for(int i=n-1;i>=1;i--){
        int r=RAND()%(i+1);
        swap(v[i],v[r]);
    }
}
void xor_shuffle(vector<pair<pair<int,int>,int>> &v){
    int n=v.size();
    for(int i=n-1;i>=1;i--){
        int r=RAND()%(i+1);
        swap(v[i],v[r]);
    }
}

conf random(int h){
    conf ret{};
    UF uf(n*n);
    vector<pair<int,int>> ed_h;
    for(int i=0;i<n;i++)for(int j=0;j+1<n;j++){
        if(i==n-1&&j+1==n-1)break;
        else ed_h.push_back(mp(i,j));
    }
    //random_shuffle(ed_h.begin(),ed_h.end());
    xor_shuffle(ed_h);
    for(int i=0;i<n-1;i++){
        for(int j=i;;j++){
            if(ed_h[j].sc==i){
                swap(ed_h[j],ed_h[i]);
                break;
            }
        }
    }
    for(int i=0;i<h;i++){
        add_edge(ret,mp(ed_h[i],2));
        uf.unit(to_id(ed_h[i]),to_id(nex(ed_h[i],2)));
    }
    vector<pair<int,int>> ed_w;
    for(int i=0;i+1<n;i++)for(int j=0;j<n;j++){
        if(i+1==n-1&&j==n-1)break;
        else ed_w.push_back(mp(i,j));
    }
    //random_shuffle(ed_w.begin(),ed_w.end());
    xor_shuffle(ed_w);
    for(auto ed: ed_w){
        if(!uf.same(to_id(ed),to_id(nex(ed,3)))){
            add_edge(ret,mp(ed,3));
            uf.unit(to_id(ed),to_id(nex(ed,3)));
        }
    }
    return ret;
}

conf find_tree(const conf &init){
    int cnt[16]={},h=0,v=0;
    for(int i=0;i<n;i++)for(int j=0;j<n;j++){
        cnt[init.t[i][j]]++;
        h+=init.t[i][j]&1;
        v+=(init.t[i][j]>>1)&1;
    }
    conf now=random(h);
    int now_cnt[16]={},now_sc=0;
    for(int i=0;i<n;i++)for(int j=0;j<n;j++)now_cnt[now.t[i][j]]++;
    for(int i=0;i<16;i++)now_sc+=(cnt[i]-now_cnt[i])*(cnt[i]-now_cnt[i]);
    auto modify_cnt=[&](int k,int x){
        now_sc-=(cnt[k]-now_cnt[k])*(cnt[k]-now_cnt[k]);
        now_cnt[k]+=x;
        now_sc+=(cnt[k]-now_cnt[k])*(cnt[k]-now_cnt[k]);
    };
    auto add=[&](pair<pair<int,int>,int> ed){
        auto p=ed.fr;
        modify_cnt(now.t[p.fr][p.sc],-1);
        now.t[p.fr][p.sc]^=(1<<ed.sc);
        modify_cnt(now.t[p.fr][p.sc],+1);
        p.fr+=dx[ed.sc]; p.sc+=dy[ed.sc]; ed.sc^=2;
        modify_cnt(now.t[p.fr][p.sc],-1);
        now.t[p.fr][p.sc]^=(1<<ed.sc);
        modify_cnt(now.t[p.fr][p.sc],+1);
    };
    vector<pair<pair<int,int>,int>> ed1,ed2;
    for(int i=0;i<n;i++)for(int j=0;j+1<n;j++){
        if(now.t[i][j]&4)ed1.push_back(mp(mp(i,j),2));
        else ed2.push_back(mp(mp(i,j),2));
    }
    for(int i=0;i+1<n;i++)for(int j=0;j<n;j++){
        if(now.t[i][j]&8)ed1.push_back(mp(mp(i,j),3));
        else ed2.push_back(mp(mp(i,j),3));
    }
    int cnt_loop=0;
    int ng=1;
    while(1){
        int r=RAND()%ed1.size();
        pair<int,pair<pair<pair<int,int>,int>,int>> best(now_sc+ng,mp(ed1[r],-1));
        add(ed1[r]);
        now.get_bfst();
        for(int i=0;i<ed2.size();i++){
            auto p=ed2[i];
            if(p.sc==ed1[r].sc&&bfst[p.fr.fr][p.fr.sc]!=bfst[p.fr.fr+dx[p.sc]][p.fr.sc+dy[p.sc]])
            {
                add(p);
                if(best.fr>now_sc){
                    best=mp(now_sc,mp(p,i));
                }
                add(p);
            }
        }
        add(best.sc.fr);
        if(best.sc.sc!=-1){
            swap(ed1[r],ed2[best.sc.sc]);
            ng=1;
        }
        else ng++;
        if(now_sc==0)break;
        cnt_loop++;
    }
    return now;
}

conf find_tree_f(conf init,const conf& c,const vector<vector<bool>>& frozen,int rank){
    const int X=5;
    int cnt[16]={},h=0,v=0;
    for(int i=0;i<n;i++)for(int j=0;j<n;j++){
        cnt[init.t[i][j]]++;
        h+=init.t[i][j]&1;
        v+=(init.t[i][j]>>1)&1;
    }
    conf now=c;
    int now_cnt[16]={},now_sc=0;
    for(int i=0;i<n;i++)for(int j=0;j<n;j++)now_cnt[now.t[i][j]]++;
    for(int i=0;i<16;i++)now_sc+=(cnt[i]-now_cnt[i])*(cnt[i]-now_cnt[i]);
    auto modify_cnt=[&](int k,int x){
        now_sc-=(cnt[k]-now_cnt[k])*(cnt[k]-now_cnt[k]);
        now_cnt[k]+=x;
        now_sc+=(cnt[k]-now_cnt[k])*(cnt[k]-now_cnt[k]);
    };
    auto add=[&](pair<pair<int,int>,int> ed){
        auto p=ed.fr;
        modify_cnt(now.t[p.fr][p.sc],-1);
        now.t[p.fr][p.sc]^=(1<<ed.sc);
        modify_cnt(now.t[p.fr][p.sc],+1);
        p.fr+=dx[ed.sc]; p.sc+=dy[ed.sc]; ed.sc^=2;
        modify_cnt(now.t[p.fr][p.sc],-1);
        now.t[p.fr][p.sc]^=(1<<ed.sc);
        modify_cnt(now.t[p.fr][p.sc],+1);
    };
    vector<pair<pair<int,int>,int>> ed1,ed2;
    for(int i=0;i<n;i++)for(int j=0;j+1<n;j++){
        if(frozen[i][j]||frozen[i][j+1])continue;
        if(now.t[i][j]&4)ed1.push_back(mp(mp(i,j),2));
        else ed2.push_back(mp(mp(i,j),2));
    }
    for(int i=0;i+1<n;i++)for(int j=0;j<n;j++){
        if(frozen[i][j]||frozen[i+1][j])continue;
        if(now.t[i][j]&8)ed1.push_back(mp(mp(i,j),3));
        else ed2.push_back(mp(mp(i,j),3));
    }
    xor_shuffle(ed2);
    int cnt_loop=0;
    int ng=1;
    while(1){
        int r=RAND()%ed1.size();
        pair<int,pair<pair<pair<int,int>,int>,int>> best(now_sc+ng,mp(ed1[r],-1));
        add(ed1[r]);
        now.get_bfst();
        for(int i=0;i<ed2.size();i++){
            auto p=ed2[i];
            if(p.sc==ed1[r].sc&&bfst[p.fr.fr][p.fr.sc]!=bfst[p.fr.fr+dx[p.sc]][p.fr.sc+dy[p.sc]])
            {
                add(p);
                if(cnt_loop<=X||best.fr>now_sc){
                    best=mp(now_sc,mp(p,i));
                }
                add(p);
                if(cnt_loop<=X)break;
            }
        }
        add(best.sc.fr);
        if(best.sc.sc!=-1){
            swap(ed1[r],ed2[best.sc.sc]);
            ng=1;
        }
        else ng++;
        if(now_sc==0)break;
        cnt_loop++;
        if(cnt_loop>=2000)return c;
    }
    return now;
}

sol2 solver;

const clock_t max_time=2920000;

int main(){
    conf init;
    scanf("%d%d",&n,&T);
    for(int i=0;i<n;i++){
        scanf("\n");
        for(int j=0;j<n;j++){
            char c;
            scanf("%c",&c);
            if('0'<=c&&c<='9')init.t[i][j]=c-'0';
            else init.t[i][j]=10+c-'a';
        }
    }
    solver.init();
    
    clock_t start_time=clock();
    //cerr<<start_time<<endl;
    
    const int MAX_B=30;
    //const int B=7;
    int B_[11]={0,0,0,0,0,0,14,9,7,6,4};
    //int C_[11]={0,0,0,0,0,0,180,144,100,49,16};
    int B=B_[n];
    vector<int> ret;
    int num_step=0;
    for(int i=3;i<=n;i++)num_step+=solver.dec[i].size();
    vector<pair<int,pair<pair<conf,vector<vector<bool>>>,vector<int>>>> vec[MAX_B];
    for(int i=0;i<B;i++)vec[i]=vector<pair<int,pair<pair<conf,vector<vector<bool>>>,vector<int>>>>(num_step,mp(2*n*n*n,mp(mp(conf(),vector<vector<bool>>()),vector<int>())));
    vector<int> num_step_(n+2,0);
    num_step_[n]=solver.dec[n].size();
    for(int i=n-1;i>=3;i--)num_step_[i]=num_step_[i+1]+solver.dec[i].size();
    int cnt_loop=0;
    vector<pair<pair<int,int>,pair<int,int>>> seq;
    vector<int> dist(num_step);
    int dist_sum=0;
    if(1){
        int temp=0;
        for(int i=n;i>=5;i--){
            for(int j=temp;j<temp+solver.dec[i].size();j++){
                dist[j]=1;
            }
            temp+=solver.dec[i].size();
        }
        for(int x: dist)dist_sum+=x;
    }
    while(1){
        clock_t now_time=clock();
        if(now_time>=max_time)break;
        //int rank=min(cnt_loop/C_[n],(int)((num_step-3)*(now_time-start_time)/(max_time-start_time))); rank=min(rank,num_step-1);
        int rank=0;
        if(1){
            int temp=dist_sum*(long double)(now_time-start_time)/(max_time-start_time);
            int sum=0;
            while(rank<dist.size()&&temp>=sum+dist[rank]){ rank++; sum+=dist[rank]; }
            rank=min(rank,num_step-1); rank=max(rank,0);
        }
        //if(cnt_loop%10==0)cerr<<rank;
        //int rank=(int)((num_step-3)*(now_time-start_time)/(max_time-start_time)); rank=min(rank,num_step-1); rank=max(rank,0);
        int temp=0;
        while(rank!=0){
            while(temp<B&&vec[temp][rank-1].fr!=2*n*n*n)temp++;
            if(temp==0)rank--;
            else break;
        }
        if(temp==0)temp++;
        int r;
        if(n>=8){
            r=RAND()%(temp*temp);
            r=temp-1-(int)(sqrt(r+.5));
        }
        else {
            r=RAND()%temp;
        }
        while(rank!=0&&vec[r][rank-1].fr==2*n*n*n){
            rank--;
        }
        conf c=rank==0?find_tree(init):find_tree_f(init,vec[r][rank-1].sc.fr.fr,vec[r][rank-1].sc.fr.sc,rank);
        auto init_=init;
        auto ret__=solver.run(init_,c);
        auto ret_=ret__.fr;
        int sum=0;
        if(ret_.size()<=2*n*n*n){
            int n_=n;
            vector<vector<bool>> frozen(n,vector<bool>(n));
            for(int j=0;j<num_step;j++){
                sum+=ret__.sc.fr[j];
                while(j>=num_step_[n_])n_--;
                frozen=rotate(frozen,ret__.sc.sc[n_].fr.fr);
                auto mir=[&]{
                    for(int i=0;i<n;i++)for(int j=0;i+j<n-1;j++)swap(frozen[i][j],frozen[n-1-j][n-1-i]);
                };
                if(ret__.sc.sc[n_].fr.sc==1)mir();
                for(auto p: solver.dec[n_][j-num_step_[n_+1]]){
                    frozen[ret__.sc.sc[n_].sc.fr+p.fr][ret__.sc.sc[n_].sc.sc+p.sc]=true;
                }
                if(ret__.sc.sc[n_].fr.sc==1)mir();
                frozen=rotate(frozen,(4-ret__.sc.sc[n_].fr.fr)&3);
                auto eq=[&](auto p,auto q){
                    for(int i=0;i<n;i++)for(int j=0;j<n;j++){
                        if(p.sc[i][j]){
                            if(!q.sc[i][j])return false;
                            if(p.fr.t[i][j]!=q.fr.t[i][j])return false;
                        }
                    }
                    return true;
                };
                if(j>=rank-1&&sum<vec[B-1][j].fr&&(j+1==num_step||ret__.sc.fr[j+1]>0)){
                    int tar=B-1;
                    for(int k=0;k<B;k++){
                        if(vec[k][j].fr==2*n*n*n){ tar=k; break; }
                        if(eq(vec[k][j].sc.fr,mp(c,frozen))){
                            if(vec[k][j].fr>sum)tar=k;
                            else tar=-1;
                            break;
                        }
                    }
                    if(tar!=-1){
                        vec[tar][j]=mp(sum,mp(mp(c,frozen),ret_));
                        for(int k=tar;k>=1;k--){
                            if(vec[k][j].fr<vec[k-1][j].fr)swap(vec[k][j],vec[k-1][j]);
                        }
                    }
                }
            }
        }
        cnt_loop++;
    }
    //cerr<<cnt_loop<<endl;
    ret=vec[0][num_step-1].sc.sc;
    string LURD="LURD";
    for(int x: ret)cout<<LURD[x]; cout<<endl;
    cerr<<(int)(1000000*(-(double)(ret.size())/(double)(4*n*n*n)+1.))<<endl;
    
    //cout<<CNT<<endl;
}



