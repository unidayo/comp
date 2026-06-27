#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef unsigned long long ull;

#define mp make_pair
#define fr first
#define sc second
#define chmin(a,b) a=min(a,b)
#define chmax(a,b) a=max(a,b)
#define rep_(_1,_2,_3,NAME,...) NAME
#define rep(...) rep_(__VA_ARGS__, rep2, rep1)(__VA_ARGS__)
#define rep1(i,j) for(int i=0;i<(j);i++)
#define rep2(i,j,k) for(int i=(j);i<(k);i++)

uint32_t RAND()
{
    static uint32_t x = 57;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

const int dx[4]={1,0,-1,0};
const int dy[4]={0,1,0,-1};
const int dz[4]={52*52,0,52*52-52,-1};

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
vector<pair<int,int>> mke(int (&c)[52][52]){
    vector<pair<int,int>> ret;
    rep(i,52)rep(j,51){
        if(c[i][j]<c[i][j+1])ret.push_back(mp(c[i][j],c[i][j+1]));
        else if(c[i][j]>c[i][j+1])ret.push_back(mp(c[i][j+1],c[i][j]));
    }
    rep(i,51)rep(j,52){
        if(c[i][j]<c[i+1][j])ret.push_back(mp(c[i][j],c[i+1][j]));
        else if(c[i][j]>c[i+1][j])ret.push_back(mp(c[i+1][j],c[i][j]));
    }
    sort(ret.begin(),ret.end());
    ret.erase(unique(ret.begin(),ret.end()),ret.end());
    return ret;
}
int cnt_unit(int (&c)[52][52]){
    UF uf(52*52);
    rep(i,52)rep(j,51){
        if(c[i][j]==c[i][j+1])uf.unit(i*52+j,i*52+j+1);
    }
    rep(i,51)rep(j,52){
        if(c[i+1][j]==c[i][j])uf.unit((i+1)*52+j,i*52+j);
    }
    int ret=0;
    rep(i,52*52)if(uf.find(i)==i)ret++;
    return ret;
}

int n,m;
int c[52][52];
set<int> ed[101][101];
bool ed_init[101][101];
int NG_ed;
int local[101][52][52];
int con[101];
int con_init[101];
int sz[101];
int sc1;
int sc2;

void init(){
    sc2=0;
    rep(i,52)rep(j,51){
        int s=min(c[i][j],c[i][j+1]);
        int t=max(c[i][j],c[i][j+1]);
        if(s!=t){
            ed[s][t].insert(i*52+j);
            sc2--;
        }
    }
    rep(i,51)rep(j,52){
        int s=min(c[i][j],c[i+1][j]);
        int t=max(c[i][j],c[i+1][j]);
        if(s!=t){
            ed[s][t].insert(52*52+i*52+j);
            sc2--;
        }
    }
    rep(i,101)rep(j,101){
        ed_init[i][j]=ed[i][j].size()>0;
    }
    rep(i,51)rep(j,51){
        bool used[2][2]={{false,false},{false,false}};
        rep(ii,2)rep(jj,2){
            if(used[ii][jj])continue;
            int t=c[i+ii][j+jj];
            int cnt=0;
            rep(iii,2)rep(jjj,2){
                if(c[i+iii][j+jjj]==t){
                    used[iii][jjj]=true;
                    cnt++;
                }
            }
            if(cnt==1)local[t][i][j]=1;
            else if(cnt==2){
                if(c[i+(ii^1)][j+(jj^1)]==t)local[t][i][j]=2;
            }
            else if(cnt==3)local[t][i][j]=-1;
        }
    }
    rep(t,101){
        rep(i,51)rep(j,51)con[t]+=local[t][i][j];
        con_init[t]=con[t];
    }
    rep(i,1,51)rep(j,1,51)sz[c[i][j]]++;
    
    sc1=0;
    rep(i,1,101)sc1-=sz[i]*sz[i];
}

bool upd(int i,int j,int x){
    //cerr<<i<<" "<<j<<" "<<x<<endl;
    rep(k,4){
        int ni=i+dx[k];
        int nj=j+dy[k];
        int s=min(c[i][j],c[ni][nj]);
        int t=c[i][j]+c[ni][nj]-s;
        if(s==t)continue;
        ed[s][t].erase(i*52+j+dz[k]);
        sc2++;
        if(ed[s][t].size()==0){
            if(ed_init[s][t])NG_ed++;
            else NG_ed--;
        }
    }
    if(c[i][j]!=0)sc1+=sz[c[i][j]]*sz[c[i][j]];
    sz[c[i][j]]--;
    if(c[i][j]!=0)sc1-=sz[c[i][j]]*sz[c[i][j]];
    int memo=c[i][j];
    c[i][j]=x;
    rep(k,4){
        int ni=i+dx[k];
        int nj=j+dy[k];
        int s=min(c[i][j],c[ni][nj]);
        int t=c[i][j]+c[ni][nj]-s;
        if(s==t)continue;
        ed[s][t].insert(i*52+j+dz[k]);
        sc2--;
        if(ed[s][t].size()==1){
            if(ed_init[s][t])NG_ed--;
            else NG_ed++;
        }
    }
    rep(t,2){
        int x_ = (t==0)?x:memo;
        for(int i_=i-1;i_<=i;i_++)for(int j_=j-1;j_<=j;j_++){
            con[x_]-=local[x_][i_][j_];
            int cnt=0;
            rep(ii,2)rep(jj,2){
                if(c[i_+ii][j_+jj]==x_)cnt++;
            }
            if(cnt==0)local[x_][i_][j_]=0;
            else if(cnt==1)local[x_][i_][j_]=1;
            else if(cnt==2){
                if(c[i_][j_]==x_&&c[i_+1][j_+1]==x_)local[x_][i_][j_]=2;
                else if(c[i_][j_+1]==x_&&c[i_+1][j_]==x_)local[x_][i_][j_]=2;
                else local[x_][i_][j_]=0;
            }
            else if(cnt==3)local[x_][i_][j_]=-1;
            else if(cnt==4)local[x_][i_][j_]=0;
            con[x_]+=local[x_][i_][j_];
            //cerr<<x_<<" "<<i_<<" "<<j_<<" "<<local[x_][i_][j_]<<endl;
        }
    }
    if(c[i][j]!=0)sc1+=sz[c[i][j]]*sz[c[i][j]];
    sz[c[i][j]]++;
    if(c[i][j]!=0)sc1-=sz[c[i][j]]*sz[c[i][j]];
    
    //sc1=0;
    //rep(i,1,101)sc1 -= sz[i]*sz[i];
    //cerr<<NG_ed<<" "<<con[memo]<<" "<<con[x]<<endl;
    //return (NG_ed==0) && (memo==0||con[memo]==4) && (x==0||con[x]==4);
    return (NG_ed==0) && (con[memo]==con_init[memo]) && (con[x]==con_init[x]);
}

int T=500;
bool f(){
    int r=RAND()%1000;
    return r<T;
}

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    cin>>n>>m;
    rep(i,1,n+1)rep(j,1,n+1)cin>>c[i][j];
    
    init();
    
    int score_type=0;
    pair<int,int> score(sz[0],sc1);
    rep(loop,1000000000){
        int i=RAND()%n+1;
        int j=RAND()%n+1;
        int d=RAND()%4;
        int memo=c[i][j];
        bool ok=upd(i,j,c[i+dx[d]][j+dy[d]]);
        if(!ok){
            if(con[c[i][j]]==con_init[c[i][j]]&&con[memo]==con_init[memo]){
                if(d==0&&j-1>0){
                    set<int> temp;
                    temp.insert(memo);
                    temp.insert(c[i][j-1]);
                    temp.insert(c[i+1][j-1]);
                    temp.insert(c[i+1][j]);
                    if(temp.size()==4||true){
                        int memo_=c[i][j-1];
                        ok=upd(i,j-1,c[i+1][j-1]);
                        if(!ok){
                            upd(i,j-1,memo_);
                            upd(i,j,memo);
                            continue;
                        }
                        else {
                            pair<int,int> nex_score;
                            if(score_type==0)nex_score=mp(sz[0],sc1);
                            else nex_score=mp(sz[0],sc2);
                            if(score <= nex_score || f()){
                                score = nex_score;
                            }
                            else {
                                upd(i,j-1,memo_);
                                upd(i,j,memo);
                            }
                            continue;
                        }
                    }
                }
                if(d==2&&j-1>0){
                    set<int> temp;
                    temp.insert(memo);
                    temp.insert(c[i][j-1]);
                    temp.insert(c[i-1][j-1]);
                    temp.insert(c[i-1][j]);
                    if(temp.size()==4||true){
                        int memo_=c[i][j-1];
                        ok=upd(i,j-1,c[i-1][j-1]);
                        if(!ok){
                            upd(i,j-1,memo_);
                            upd(i,j,memo);
                            continue;
                        }
                        else {
                            pair<int,int> nex_score;
                            if(score_type==0)nex_score=mp(sz[0],sc1);
                            else nex_score=mp(sz[0],sc2);
                            if(score <= nex_score || f()){
                                score = nex_score;
                            }
                            else {
                                upd(i,j-1,memo_);
                                upd(i,j,memo);
                            }
                            continue;
                        }
                    }
                }
                if(d==3&&i-1>0){
                    set<int> temp;
                    temp.insert(memo);
                    temp.insert(c[i][j-1]);
                    temp.insert(c[i-1][j]);
                    temp.insert(c[i-1][j-1]);
                    if(temp.size()==4||true){
                        int memo_=c[i-1][j];
                        ok=upd(i-1,j,c[i-1][j-1]);
                        if(!ok){
                            upd(i-1,j,memo_);
                            upd(i,j,memo);
                            continue;
                        }
                        else {
                            pair<int,int> nex_score;
                            if(score_type==0)nex_score=mp(sz[0],sc1);
                            else nex_score=mp(sz[0],sc2);
                            if(score <= nex_score || f()){
                                score = nex_score;
                            }
                            else {
                                upd(i-1,j,memo_);
                                upd(i,j,memo);
                            }
                            continue;
                        }
                    }
                }
                if(d==1&&i-1>0){
                    set<int> temp;
                    temp.insert(memo);
                    temp.insert(c[i][j+1]);
                    temp.insert(c[i-1][j]);
                    temp.insert(c[i-1][j+1]);
                    if(temp.size()==4||true){
                        int memo_=c[i-1][j];
                        ok=upd(i-1,j,c[i-1][j+1]);
                        if(!ok){
                            upd(i-1,j,memo_);
                            upd(i,j,memo);
                            continue;
                        }
                        else {
                            pair<int,int> nex_score;
                            if(score_type==0)nex_score=mp(sz[0],sc1);
                            else nex_score=mp(sz[0],sc2);
                            if(score <= nex_score || f()){
                                score = nex_score;
                            }
                            else {
                                upd(i-1,j,memo_);
                                upd(i,j,memo);
                            }
                            continue;
                        }
                    }
                }
            }
            upd(i,j,memo);
            continue;
        }
        pair<int,int> nex_score;
        if(score_type==0)nex_score=mp(sz[0],sc1);
        else nex_score=mp(sz[0],sc2);
        if(score <= nex_score || f()){
            score = nex_score;
            //cerr<<i<<" "<<j<<" "<<c[i+dx[d]][j+dy[d]]<<endl;
        }
        else {
            upd(i,j,memo);
        }
        if(loop%25000==0)T--;
        if(loop%100000==0){
            cerr<<loop<<" "<<score.fr<<" "<<score.sc<<endl;
            if(clock()>1900000){
                break;
            }
            score_type^=1;
            if(score_type==0)score=mp(sz[0],sc1);
            else score=mp(sz[0],sc2);
        }
    }
    rep(i,1,n+1){
        rep(j,1,n+1){
            cout<<c[i][j]<<(j==n?'\n':' ');
        }
    }
    fflush(stdout);
    return 0;
}

