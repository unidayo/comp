#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

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
#define all(x) x.begin(),x.end()
#define pb push_back

const int di4[4]={-1,0,1,0};
const int dj4[4]={0,-1,0,1};
const int di13[13]={-2,-1,-1,-1,0,0,0,0,0,1,1,1,2};
const int dj13[13]={0,-1,0,1,-2,-1,0,1,2,-1,0,1,0};
const int dk[13]={-108,-55,-54,-53,-2,-1,0,1,2,53,54,55,108};

pair<int,int> oppo(int c, pair<int,int> x, pair<int,int> y){
    if(c==1||c==2){
        return mp(x.fr*2-y.fr,x.sc*2-y.sc);
    }
    if(x.fr!=y.fr){
        if(c==3||c==4)return mp(x.fr,x.sc-1);
        else return mp(x.fr,x.sc+1);
    }
    else {
        if(c==4||c==5)return mp(x.fr-1,x.sc);
        else return mp(x.fr+1,x.sc);
    }
}

bool on(int nx,int ny){
    return 2<=nx&&nx<52&&2<=ny&&ny<52;
}

int N,M,K,T;
int si[1602],sj[1602],ti[1602],tj[1602];
int income[1602];
vector<pair<int,int>> people[54][54];

void input(){
    cin>>N>>M>>K>>T;
    rep(i,M){
        cin>>si[i]>>sj[i]>>ti[i]>>tj[i];
        si[i]+=2;
        sj[i]+=2;
        ti[i]+=2;
        tj[i]+=2;
        income[i] = abs(si[i]-ti[i])+abs(sj[i]-tj[i]);
        people[si[i]][sj[i]].pb(mp(i,0));
        people[ti[i]][tj[i]].pb(mp(i,1));
    }
}

void output(const vector<pair<int,pair<int,int>>> &ret){
    //assert(ret.size()<=T);
    for(int i=0;i<ret.size();i++){
        if(ret[i].fr!=-1)cout<<ret[i].fr<<" "<<ret[i].sc.fr-2<<" "<<ret[i].sc.sc-2<<endl;
        else cout<<"-1"<<endl;
    }
    for(int i=ret.size();i<T;i++){
        cout<<"-1"<<endl;
    }
}

vector<pair<int,pair<int,int>>> mkseq(const vector<pair<int,int>> &route){
    vector<pair<int,pair<int,int>>> ret;
    for(int i=1;i+1<route.size();i++){
        //assert(abs(route[i].fr-route[i-1].fr)+abs(route[i].sc-route[i-1].sc)==1);
        //assert(abs(route[i].fr-route[i+1].fr)+abs(route[i].sc-route[i+1].sc)==1);
        //assert(route[i-1]!=route[i+1]);
        bool f[4]={false,false,false,false};
        auto ff=[&](pair<int,int> p,pair<int,int> q){
            if(q.sc>p.sc)f[0]=true;
            if(q.fr<p.fr)f[1]=true;
            if(q.sc<p.sc)f[2]=true;
            if(q.fr>p.fr)f[3]=true;
        };
        ff(route[i],route[i-1]);
        ff(route[i],route[i+1]);
        if(f[0]&&f[2])ret.pb(mp(1,route[i]));
        if(f[1]&&f[3])ret.pb(mp(2,route[i]));
        if(f[2]&&f[3])ret.pb(mp(3,route[i]));
        if(f[1]&&f[2])ret.pb(mp(4,route[i]));
        if(f[0]&&f[1])ret.pb(mp(5,route[i]));
        if(f[0]&&f[3])ret.pb(mp(6,route[i]));
    }
    return ret;
}

int root=57;
int mod=996244353;
int HASH[3000];
int HASH2[3000];
int modpow(int x,int k){
    if(k==0)return 1;
    ll ret=modpow(x,k/2);
    ret*=ret; ret%=mod;
    if(k&1){ ret*=x; ret%=mod; }
    return ret;
}

// 途中状態を管理する構造体
struct status{
    int t; // 消費したターン数
    int m; // 現在の資金
    int ult_m; // 最後まで待機した時の資金
    int income;
    int areasum;
    int score2;
    vector<pair<int,pair<int,int>>> ret; // 操作の履歴

    int hash;
};

int calc_score2(int t, int m, int income, int areasum){
    //return areasum;
    //return m+(T-t)*income+(areasum-2*income)*max(0,T-t-100)/4;
    return m+(T-t)*income+areasum*max(0,T-t-100-max(0,(5000-m+max(1,income)-1)/max(1,income)))/2;
}

struct status_detail{
    int board[54][54];
    bool covered[3000];
    int dm;
    int ninc[3000];
    int areainc[3000];
};
status_detail make_status_detail(const status& s){
    status_detail ret;
    rep(i,54)rep(j,54)ret.board[i][j]=-1;
    for(auto p: s.ret){
        ret.board[p.sc.fr][p.sc.sc]=p.fr;
    }
    rep(i,3000)ret.covered[i]=false;
    for(auto p: s.ret){
        if(p.fr==0){
            rep(k,13){
                ret.covered[p.sc.fr*54+p.sc.sc+dk[k]]=true;
            }
        }
    }
    ret.dm=0;
    rep(i,3000)ret.ninc[i]=0;
    rep(i,3000)ret.areainc[i]=0;
    rep(i,M){
        if(ret.covered[si[i]*54+sj[i]]){
            if(ret.covered[ti[i]*54+tj[i]]){
                ret.dm += income[i];
            }
            else {
                rep(k,13)ret.ninc[ti[i]*54+tj[i]+dk[k]] += income[i];
            }
        }
        else {
            if(ret.covered[ti[i]*54+tj[i]]){
                rep(k,13)ret.ninc[si[i]*54+sj[i]+dk[k]] += income[i];
            }
        }
        if(!ret.covered[si[i]*54+sj[i]]){
            rep(k,13)ret.areainc[si[i]*54+sj[i]+dk[k]] += income[i];
        }
        if(!ret.covered[ti[i]*54+tj[i]]){
            rep(k,13)ret.areainc[ti[i]*54+tj[i]+dk[k]] += income[i];
        }
    }
    return ret;
}
auto calc_costs(int len, int m, int dm){ // 線路の距離・現在の資金・現在のincome
    int construct_cost, construct_turn, turn;
    construct_cost = 5000 + 100*len;
    construct_turn = 1 + len;
    if(dm==0){
        if(construct_cost>m)return tuple(1000000,1000000,1000000);
        else turn = construct_turn;
    }
    else {
        turn = max( (construct_cost-m+dm-1)/dm+1 , construct_turn );
    }
    return tuple(construct_cost,construct_turn,turn);
}

auto get_stations(const status& s){
    int used[1602]={};
    vector<pair<int,int>> station;
    int cnt=0;
    for(auto p: s.ret){
        if(p.fr==-1)continue;
        if(p.fr!=0)cnt++;
        else {
            int sum=0;
            rep(k,13)for(auto q: people[p.sc.fr+di13[k]][p.sc.sc+dj13[k]]){
                if(used[q.fr]!=3){
                    used[q.fr] |= 1<<q.sc;
                    if(used[q.fr]==3){
                        sum += income[q.fr];
                    }
                }
            }
            station.pb(mp(cnt,sum));
            cnt=0;
        }
    }
    return station;
}

int t3,t4,t5;

void check(const status& s){
    auto s_detail = make_status_detail(s);
    for(auto p: s.ret){
        if(p.fr==0){
            bool ok=false;
            rep(k,4){
                ok|=s_detail.board[p.sc.fr+di4[k]][p.sc.sc+dj4[k]]!=-1;
            }
            if(!ok){ output(s.ret); cerr<<__LINE__<<endl; exit(0); }
        }
    }
}

vector<status> get_next(const status &s, int c1, int c2, int minscore, int minscore2){ // sから派生してできる評価値の良いc個のstatusを作る
    //t3-=clock();
    auto s_detail = make_status_detail(s);
    //t3+=clock();
    //t4-=clock();
    static int dist[54][54];
    static int tie_break[54][54]; //大きい方が良い
    pair<int,int> que[3000];
    int que_sz=0,que_nx=0;
    //queue<pair<int,int>> que;
    rep(i,54)rep(j,54){
        dist[i][j]=-1;
        tie_break[i][j]=0;
    }
    for(auto p: s.ret){
        if(p.fr==0){
            rep(k,4){
                int ni=p.sc.fr+di4[k];
                int nj=p.sc.sc+dj4[k];
                if(dist[ni][nj]==-1&&s_detail.board[ni][nj]==-1&&on(ni,nj)){
                    dist[ni][nj]=0;
                    tie_break[ni][nj]=s_detail.ninc[ni*54+nj]+s_detail.areainc[ni*54+nj];
                    que[que_sz++]=mp(ni,nj);
                    //que.push(mp(ni,nj));
                }
            }
        }
    }
    while(que_nx<que_sz)
    //while(!que.empty())
    {
        auto q=que[que_nx++];
        //auto q=que.front(); que.pop();
        rep(k,4){
            int ni=q.fr+di4[k];
            int nj=q.sc+dj4[k];
            if(dist[ni][nj]==-1&&s_detail.board[ni][nj]==-1&&on(ni,nj)){
                dist[ni][nj]=dist[q.fr][q.sc]+1;
                tie_break[ni][nj]=0;
                rep(kk,4){
                    int nni=ni+di4[kk];
                    int nnj=nj+dj4[kk];
                    if(/*on(nni,nnj)&&*/dist[nni][nnj]==dist[ni][nj]-1)chmax(tie_break[ni][nj],tie_break[nni][nnj]);
                }
                tie_break[ni][nj]+=s_detail.ninc[ni*54+nj]+s_detail.areainc[ni*54+nj];
                que[que_sz++]=mp(ni,nj);
                //que.push(mp(ni,nj));
            }
        }
    }

    //t4+=clock();
    //t5-=clock();

    bool special[54][54];
    pair<int,int> ed_num[54][54];
    int ed_ul[54][54];
    int ed_dr[54][54];
    int ed_ur[54][54];
    int ed_dl[54][54];
    rep(i,54)rep(j,54){
        special[i][j]=false;
        ed_num[i][j]=mp(-1,-1);
        ed_ul[i][j]=-1;
        ed_dr[i][j]=-1;
        ed_ur[i][j]=-1;
        ed_dl[i][j]=-1;
    }
    {
        int cnt=0;
        rep(l,s.ret.size()){
            auto p=s.ret[l];
            if(p.fr==-1)continue;
            if(p.fr==0)cnt++;
            else if(s_detail.board[p.sc.fr][p.sc.sc]>0){
                ed_num[p.sc.fr][p.sc.sc] = mp(cnt,l);
                ed_ul[p.sc.fr][p.sc.sc] = cnt;
                ed_dr[p.sc.fr][p.sc.sc] = cnt;
                ed_ur[p.sc.fr][p.sc.sc] = cnt;
                ed_dl[p.sc.fr][p.sc.sc] = cnt;
            }
        }
    }
    rep(i,2,52)rep(j,2,52){
        if(s_detail.board[i][j]!=-1)continue;
        if(ed_ul[i-1][j]==ed_ul[i][j-1]&&ed_ul[i-1][j]!=-1&&s_detail.board[i-1][j-1]!=0){
            ed_ul[i][j]=ed_ul[i-1][j];
            special[i][j]=true;
        }
        if(s_detail.board[i-1][j-1]==6){
            if(s_detail.board[i-1][j]==0&&ed_ul[i][j-1]!=-1){
                ed_ul[i][j]=ed_ul[i][j-1];
                special[i][j]=true;
            }
            if(s_detail.board[i][j-1]==0&&ed_ul[i-1][j]!=-1){
                ed_ul[i][j]=ed_ul[i-1][j];
                special[i][j]=true;
            }
        }
    }
    for(int i=51;i>=2;i--)for(int j=51;j>=2;j--){
        if(s_detail.board[i][j]!=-1)continue;
        if(ed_dr[i+1][j]==ed_dr[i][j+1]&&ed_dr[i+1][j]!=-1&&s_detail.board[i+1][j+1]!=0){
            ed_dr[i][j]=ed_dr[i+1][j];
            special[i][j]=true;
        }
        if(s_detail.board[i+1][j+1]==4){
            if(s_detail.board[i+1][j]==0&&ed_dr[i][j+1]!=-1){
                ed_dr[i][j]=ed_dr[i][j+1];
                special[i][j]=true;
            }
            if(s_detail.board[i][j+1]==0&&ed_dr[i+1][j]!=-1){
                ed_dr[i][j]=ed_dr[i+1][j];
                special[i][j]=true;
            }
        }
    }
    rep(i,2,52)for(int j=51;j>=2;j--){
        if(s_detail.board[i][j]!=-1)continue;
        if(ed_ur[i-1][j]==ed_ur[i][j+1]&&ed_ur[i-1][j]!=-1&&s_detail.board[i-1][j+1]!=0){
            ed_ur[i][j]=ed_ur[i-1][j];
            special[i][j]=true;
        }
        if(s_detail.board[i-1][j+1]==3){
            if(s_detail.board[i-1][j]==0&&ed_ur[i][j+1]!=-1){
                ed_ur[i][j]=ed_ur[i][j+1];
                special[i][j]=true;
            }
            if(s_detail.board[i][j+1]==0&&ed_ur[i-1][j]!=-1){
                ed_ur[i][j]=ed_ur[i-1][j];
                special[i][j]=true;
            }
        }
    }
    for(int i=51;i>=2;i--)rep(j,2,52){
        if(s_detail.board[i][j]!=-1)continue;
        if(ed_dl[i+1][j]==ed_dl[i][j-1]&&ed_dl[i+1][j]!=-1&&s_detail.board[i+1][j-1]!=0){
            ed_dl[i][j]=ed_dl[i+1][j];
            special[i][j]=true;
        }
        if(s_detail.board[i+1][j-1]==5){
            if(s_detail.board[i+1][j]==0&&ed_dl[i][j-1]!=-1){
                ed_dl[i][j]=ed_dl[i][j-1];
                special[i][j]=true;
            }
            if(s_detail.board[i][j-1]==0&&ed_dl[i+1][j]!=-1){
                ed_dl[i][j]=ed_dl[i+1][j];
                special[i][j]=true;
            }
        }
    }

    vector<pair<pair<int,int>,pair<int,int>>> best;
    vector<pair<pair<int,int>,pair<int,int>>> best2;
    rep(i,2,52)rep(j,2,52){
        if(s_detail.board[i][j]==0)continue;
        if(s_detail.board[i][j]==-1&&dist[i][j]==-1)continue;
        auto [construct_cost, construct_turn, turn] = calc_costs(special[i][j]?0:max(0,dist[i][j]),s.m,s_detail.dm);
        if(s.t+turn>T)continue;
        int score = s.m + (turn-1)*s_detail.dm - construct_cost + (T-s.t-turn+1)*(s_detail.dm+s_detail.ninc[i*54+j]);
        int money_ = s.m + (turn-1)*s_detail.dm - construct_cost + (s_detail.dm+s_detail.ninc[i*54+j]);
        int score2 = calc_score2(s.t+turn, money_, s.income+s_detail.ninc[i*54+j], s.areasum+s_detail.areainc[i*54+j]);
        bool done=false;
        auto temp=mp(mp(score,score2),mp(i,j));
        if(score>=minscore)
        {
            int sz=best.size();
            if(best.size()>=c1&&best.back().fr.fr>=score){}
            else {
                if(best.size()>=c1){
                    auto temp_ = best.back();
                    best.back() = temp;
                    temp = temp_;
                }
                else {
                    best.pb(temp); sz++;
                    done = true;
                }
                for(int i=sz-1;i>0;i--){
                    if(best[i-1]<best[i])swap(best[i-1],best[i]);
                    else break;
                }
            }
        }
        swap(temp.fr.fr,temp.fr.sc);
        if(!done){
            if(temp.fr.fr>=minscore2){
                int sz_=best2.size();
                if(best2.size()>=c2&&best2.back().fr.fr>=score2){}
                else{
                    if(best2.size()>=c2){
                        best2.back()=temp;
                    }
                    else {
                        best2.pb(temp); sz_++;
                    }
                    for(int i=sz_-1;i>0;i--){
                        if(best2[i-1]<best2[i])swap(best2[i-1],best2[i]);
                        else break;
                    }
                }
            }
        }
    }
    for(auto p: best2)best.pb(p);
    vector<status> ret;
    for(auto p: best){
        int i=p.sc.fr;
        int j=p.sc.sc;
        if(!special[i][j]){
            vector<pair<int,int>> route;
            pair<int,int> loc(i,j);
            route.pb(loc);
            while(dist[loc.fr][loc.sc]>0){
                pair<int,pair<int,int>> nex(-1,mp(-1,-1));
                rep(k,4){
                    int ni=loc.fr+di4[k];
                    int nj=loc.sc+dj4[k];
                    if(on(ni,nj)&&dist[ni][nj]==dist[loc.fr][loc.sc]-1){
                        chmax(nex,mp(tie_break[ni][nj],mp(ni,nj)));
                    }
                }
                loc=nex.sc;
                route.pb(loc);
            }
            rep(k,4){
                int ni=loc.fr+di4[k];
                int nj=loc.sc+dj4[k];
                if(on(ni,nj)&&s_detail.board[ni][nj]==0){
                    loc=mp(ni,nj);
                    break;
                }
            }
            route.pb(loc);
            auto res = mkseq(route);

            auto [construct_cost, construct_turn, turn] = calc_costs(max(0,dist[i][j]),s.m,s_detail.dm);
            auto s_ = s;
            s_.t = s.t+turn;
            s_.m = s.m+s_detail.dm*(turn-1)+s_detail.dm+s_detail.ninc[i*54+j]-construct_cost;
            s_.ult_m = s_.m+(T-s_.t)*(s_detail.dm+s_detail.ninc[i*54+j]);
            rep(i,turn-construct_turn){
                s_.ret.pb(mp(-1,mp(-1,-1)));
            }
            for(auto p: res){
                s_.ret.pb(p);
            }
            s_.ret.pb(mp(0,mp(i,j)));

            s_.income = s.income+s_detail.ninc[i*54+j];
            s_.areasum = s.areasum+s_detail.areainc[i*54+j];
            s_.score2 = calc_score2(s_.t,s_.m,s_.income,s_.areasum);

            s_.hash = (s.hash + HASH[i*54+j])%mod;

            ret.pb(s_);
        }
        else {
            vector<pair<int,int>> route;
            int start=0,end=0;
            int id_st;
            if(ed_ul[i][j]!=-1)id_st=ed_ul[i][j];
            else if(ed_dr[i][j]!=-1)id_st=ed_dr[i][j];
            else if(ed_ur[i][j]!=-1)id_st=ed_ur[i][j];
            else if(ed_dl[i][j]!=-1)id_st=ed_dl[i][j];
            else continue;
            {
                {
                    int cnt=0;
                    rep(k,s.ret.size()){
                        auto p=s.ret[k];
                        if(p.fr==-1)continue;
                        if(p.fr==0){
                            cnt++;
                            if(cnt==id_st)start=k+1;
                            if(cnt==id_st+1)end=k;
                        }
                    }
                }
                while(s.ret[start].fr==-1)start++;
                route.pb(s.ret[end].sc);
                for(int k=start;k<end;k++){
                    //assert(s.ret[k].fr!=-1);
                    if(s.ret[k].fr!=-1){
                        route.pb(s.ret[k].sc);
                    }
                }
                {
                    //assert(route.size()>=2);
                    //assert(start<end);
                    if(route.size()<2||start>=end)continue;
                    route.pb(oppo(s.ret[end-1].fr,route.back(),route[route.size()-2]));
                }
            }
            if(ed_ul[i][j]!=-1){
                int flag=0;
                if(ed_ul[i][j+1]==ed_ul[i][j]&&s_detail.board[i][j+1]==-1){
                    flag=1; j++;
                }
                else if(ed_ul[i+1][j]==ed_ul[i][j]&&s_detail.board[i+1][j]==-1){
                    flag=2; i++;
                }
                pair<int,int> loc_u(i,j), loc_l(i,j);
                int id_u, id_l;
                {
                    while(s_detail.board[loc_u.fr][loc_u.sc]==-1)loc_u.fr --;
                    rep(k,route.size())if(route[k]==loc_u){ id_u=k; break; }
                    while(s_detail.board[loc_l.fr][loc_l.sc]==-1)loc_l.sc --;
                    rep(k,route.size())if(route[k]==loc_l){ id_l=k; break; }
                }
                //assert(abs(id_u-id_l)==abs(loc_u.fr-i)+abs(loc_l.sc-j));
                if(abs(id_u-id_l)!=abs(loc_u.fr-i)+abs(loc_l.sc-j))continue;
                if(id_u==id_l)continue;
                if(id_u<id_l){
                    for(int k=id_u;;k++){
                        route[k+1]=route[k];
                        route[k+1].fr++;
                        if(route[k+1].fr==i)break;
                    }
                    for(int k=id_l;;k--){
                        route[k-1]=route[k];
                        route[k-1].sc++;
                        if(route[k-1].sc==j)break;
                    }
                }
                else {
                    for(int k=id_u;;k--){
                        route[k-1]=route[k];
                        route[k-1].fr++;
                        if(route[k-1].fr==i)break;
                    }
                    for(int k=id_l;;k++){
                        route[k+1]=route[k];
                        route[k+1].sc++;
                        if(route[k+1].sc==j)break;
                    }
                }
                if(flag==1){ j--; }
                else if(flag==2){ i--; }
            }
            else if(ed_dr[i][j]!=-1){
                int flag=0;
                if(ed_dr[i][j-1]==ed_dr[i][j]&&s_detail.board[i][j-1]==-1){
                    flag=1; j--;
                }
                else if(ed_dr[i-1][j]==ed_dr[i][j]&&s_detail.board[i-1][j]==-1){
                    flag=2; i--;
                }
                pair<int,int> loc_d(i,j), loc_r(i,j);
                int id_d, id_r;
                {
                    while(s_detail.board[loc_d.fr][loc_d.sc]==-1)loc_d.fr ++;
                    rep(k,route.size())if(route[k]==loc_d){ id_d=k; break; }
                    while(s_detail.board[loc_r.fr][loc_r.sc]==-1)loc_r.sc ++;
                    rep(k,route.size())if(route[k]==loc_r){ id_r=k; break; }
                }
                //assert(abs(id_d-id_r)==abs(loc_d.fr-i)+abs(loc_r.sc-j));
                if(abs(id_d-id_r)!=abs(loc_d.fr-i)+abs(loc_r.sc-j))continue;
                if(id_d==id_r)continue;
                if(id_d<id_r){
                    for(int k=id_d;;k++){
                        route[k+1]=route[k];
                        route[k+1].fr--;
                        if(route[k+1].fr==i)break;
                    }
                    for(int k=id_r;;k--){
                        route[k-1]=route[k];
                        route[k-1].sc--;
                        if(route[k-1].sc==j)break;
                    }
                }
                else {
                    for(int k=id_d;;k--){
                        route[k-1]=route[k];
                        route[k-1].fr--;
                        if(route[k-1].fr==i)break;
                    }
                    for(int k=id_r;;k++){
                        route[k+1]=route[k];
                        route[k+1].sc--;
                        if(route[k+1].sc==j)break;
                    }
                }
                if(flag==1){ j++; }
                else if(flag==2){ i++; }
            }
            else if(ed_ur[i][j]!=-1){
                int flag=0;
                if(ed_ur[i][j-1]==ed_ur[i][j]&&s_detail.board[i][j-1]==-1){
                    flag=1; j--;
                }
                else if(ed_ur[i+1][j]==ed_ur[i][j]&&s_detail.board[i+1][j]==-1){
                    flag=2; i++;
                }
                pair<int,int> loc_u(i,j), loc_r(i,j);
                int id_u, id_r;
                {
                    while(s_detail.board[loc_u.fr][loc_u.sc]==-1)loc_u.fr --;
                    rep(k,route.size())if(route[k]==loc_u){ id_u=k; break; }
                    while(s_detail.board[loc_r.fr][loc_r.sc]==-1)loc_r.sc ++;
                    rep(k,route.size())if(route[k]==loc_r){ id_r=k; break; }
                }
                //assert(abs(id_u-id_r)==abs(loc_u.fr-i)+abs(loc_r.sc-j));
                if(abs(id_u-id_r)!=abs(loc_u.fr-i)+abs(loc_r.sc-j))continue;
                if(id_u==id_r)continue;
                if(id_u<id_r){
                    for(int k=id_u;;k++){
                        route[k+1]=route[k];
                        route[k+1].fr++;
                        if(route[k+1].fr==i)break;
                    }
                    for(int k=id_r;;k--){
                        route[k-1]=route[k];
                        route[k-1].sc--;
                        if(route[k-1].sc==j)break;
                    }
                }
                else {
                    for(int k=id_u;;k--){
                        route[k-1]=route[k];
                        route[k-1].fr++;
                        if(route[k-1].fr==i)break;
                    }
                    for(int k=id_r;;k++){
                        route[k+1]=route[k];
                        route[k+1].sc--;
                        if(route[k+1].sc==j)break;
                    }
                }
                if(flag==1){ j++; }
                else if(flag==2){ i--; }
            }
            else if(ed_dl[i][j]!=-1){
                int flag=0;
                if(ed_dl[i][j+1]==ed_dl[i][j]&&s_detail.board[i][j+1]==-1){
                    flag=1; j++;
                }
                else if(ed_dl[i-1][j]==ed_dl[i][j]&&s_detail.board[i-1][j]==-1){
                    flag=2; i--;
                }
                pair<int,int> loc_d(i,j), loc_l(i,j);
                int id_d, id_l;
                {
                    while(s_detail.board[loc_d.fr][loc_d.sc]==-1)loc_d.fr ++;
                    rep(k,route.size())if(route[k]==loc_d){ id_d=k; break; }
                    while(s_detail.board[loc_l.fr][loc_l.sc]==-1)loc_l.sc --;
                    rep(k,route.size())if(route[k]==loc_l){ id_l=k; break; }
                }
                //assert(abs(id_d-id_l)==abs(loc_d.fr-i)+abs(loc_l.sc-j));
                if(abs(id_d-id_l)!=abs(loc_d.fr-i)+abs(loc_l.sc-j))continue;
                if(id_d==id_l)continue;
                if(id_d<id_l){
                    for(int k=id_d;;k++){
                        route[k+1]=route[k];
                        route[k+1].fr--;
                        if(route[k+1].fr==i)break;
                    }
                    for(int k=id_l;;k--){
                        route[k-1]=route[k];
                        route[k-1].sc++;
                        if(route[k-1].sc==j)break;
                    }
                }
                else {
                    for(int k=id_d;;k--){
                        route[k-1]=route[k];
                        route[k-1].fr--;
                        if(route[k-1].fr==i)break;
                    }
                    for(int k=id_l;;k++){
                        route[k+1]=route[k];
                        route[k+1].sc++;
                        if(route[k+1].sc==j)break;
                    }
                }
                if(flag==1){ j--; }
                else if(flag==2){ i++; }
            }
            auto res = mkseq(route);

            auto [construct_cost, construct_turn, turn] = calc_costs(0,s.m,s_detail.dm);
            auto s_ = s;
            s_.t = s.t+turn;
            s_.m = s.m+s_detail.dm*(turn-1)+s_detail.dm+s_detail.ninc[i*54+j]-construct_cost;
            s_.ult_m = s_.m+(T-s_.t)*(s_detail.dm+s_detail.ninc[i*54+j]);
            rep(i,turn-construct_turn){
                s_.ret.pb(mp(-1,mp(-1,-1)));
            }
            s_.ret.pb(mp(0,mp(i,j)));

            rep(k,res.size()){
                s_.ret[end-k-1] = res[res.size()-1-k];
            }
            for(int k=end-res.size()-1;k>=start;k--){
                //assert(s_.ret[k]==mp(-1,mp(-1,-1)));
                s_.ret[k]=mp(-1,mp(-1,-1));
            }

            s_.income = s.income+s_detail.ninc[i*54+j];
            s_.areasum = s.areasum+s_detail.areainc[i*54+j];
            s_.score2 = calc_score2(s_.t,s_.m,s_.income,s_.areasum);

            s_.hash = (s.hash + HASH[i*54+j])%mod;

            ret.pb(s_);
        }
    }
    //t5+=clock();
    return ret;
}

status finalize1(const status& s){ //線路を先に作っておく //今のところ終状態にしか対応してない
    auto station = get_stations(s);
    pair<int,vector<int>> best(-1,0);
    int st_id=-1;
    for(int i=0;i<=station.size();i++){
        int prog_s=0;
        pair<int,int> prog_r(0,0);
        while(prog_r.fr<station.size()&&station[prog_r.fr].fr==prog_r.sc)prog_r=mp(prog_r.fr+1,0);
        vector<int> ret;
        int money=K;
        int income=0;
        for(int t=0;t<T;t++){
            if(money>=5000&&prog_s<prog_r.fr){
                ret.pb(0);
                prog_s++;
                money-=5000;
                income+=station[prog_s-1].sc;
            }
            else if(money>=100&&prog_r.fr<station.size()&&(prog_r.fr<=prog_s||prog_s>=i)){
                ret.pb(1);
                prog_r.sc++;
                while(prog_r.fr<station.size()&&station[prog_r.fr].fr==prog_r.sc)prog_r=mp(prog_r.fr+1,0);
                money-=100;
            }
            else {
                ret.pb(-1);
            }
            money += income;
        }
        if(best.fr<money){
            best.fr=money;
            best.sc=ret;
            st_id=i;
        }
    }

    //cerr<<s.ult_m<<" "<<best.fr<<endl;
    vector<pair<int,pair<int,int>>> ret0,ret1,ret;
    for(auto p: s.ret){
        if(p.fr==0)ret0.pb(p);
        if(p.fr>0)ret1.pb(p);
    }
    int nex0=0,nex1=0;
    rep(t,T){
        if(best.sc[t]==0){
            ret.pb(ret0[nex0++]);
        }
        else if(best.sc[t]>0){
            ret.pb(ret1[nex1++]);
        }
        else {
            ret.pb(mp(-1,mp(-1,-1)));
        }
    }
    auto s_=s;
    s_.ret=ret;
    s_.t=T;
    s_.m=s_.ult_m=best.fr;

    s_.score2 = s_.m;

    /*{
        cerr<<st_id<<" ";
        int CNT=0;
        rep(i,s.ret.size()){
            if(s.ret[i].fr==0)CNT++;
            if(CNT==st_id){ cerr<<i<<" "; break; }
        }
        if(st_id==station.size())cerr<<1000<<" ";

        int sum=0;
        rep(j,st_id)sum+=station[j].sc;
        cerr<<sum<<endl;
    }*/
    
    return s_;
}

void profile(const status& s){
    int num_st=0;
    int len=0;
    int cnt_wait=0;
    int last_op=0;
    rep(i,s.ret.size()){
        auto p=s.ret[i];
        if(p.fr==0)num_st++;
        if(p.fr>0)len++;
        if(p.fr==-1)cnt_wait++;
        if(p.fr!=-1)last_op=i;
    }

    int income=0;
    auto s_detail = make_status_detail(s);
    income = s_detail.dm;

    cerr<<M<<" "<<K<<" "<<num_st<<" "<<len<<" "<<cnt_wait<<" "<<last_op<<" "<<income<<endl;
}

int main(){
    HASH[0]=HASH2[0]=1;
    rep(i,2999){
        HASH[i+1]=HASH[i]*(ll)57%mod;
        //HASH2[i+1]=HASH2[i]*(ll)5%mod;
    }

    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.precision(20);

    input();

    // スコアが最も良くなる１つのペアに駅を設置する
    status best;
    {
        best.t=0;
        best.m=K;
        best.ult_m=K;

        best.income=0;
        best.areasum=0;
        best.score2=calc_score2(best.t,best.m,best.income,best.areasum);

        best.hash=0;
    }
    vector<vector<status>> dp(T);
    vector<vector<status>> dp2(T);
    rep(ai,2,52)rep(aj,2,52){
        status temp;
        temp.t=1;
        temp.m=K-5000;
        temp.ret.pb(mp(0,mp(ai,aj)));

        temp.income=0;
        temp.areasum=0;
        rep(k,13){
            int ni = ai+di13[k];
            int nj = aj+dj13[k];
            if(on(ni,nj)){
                for(auto p: people[ni][nj]){
                    temp.areasum += income[p.fr];
                }
            }
        }
        temp.score2=calc_score2(temp.t,temp.m,temp.income,temp.areasum);

        temp.hash=HASH[ai*54+aj];

        dp[0].pb(temp);
    }

    int t1=0,t2=0;

    unordered_map<int,int> mx_score, mx_score2;

    rep(i,T-1){
        //cerr<<__LINE__<<" "<<i<<" "<<endl;
        if(i%100==0)cerr<<i<<" "<<endl;
        if(clock()>2800000)goto last;
        int B=11; if(M<=200)B=12; if(M>=800)B=10;
        int B2=B;
        auto f=[&](const status& s, int minscore, int minscore2){
            //t1-=clock();
            //cerr<<__LINE__<<endl;
            auto res = get_next(s,min(B,20),min(B2,20),minscore,minscore2);
            //cerr<<__LINE__<<endl;
            //auto res = get_next(s,60,60,minscore,minscore2);
            //t1+=clock();
            //t2-=clock();
            for(auto st: res){
                bool ok=false;
                if(mx_score.find(st.hash)==mx_score.end()||mx_score[st.hash]<st.ult_m){
                    ok=true;
                    mx_score[st.hash]=st.ult_m;
                }
                if(mx_score2.find(st.hash)==mx_score2.end()||mx_score2[st.hash]<st.score2){
                    ok=true;
                    mx_score2[st.hash]=st.score2;
                }
                if(!ok)continue;
                int ni=min(T-1,st.t-1+max(0,(5000-st.m+max(1,st.income)-1)/max(1,st.income)));
                bool is_new=true;
                for(int k=0;k<dp[ni].size();k++){
                    if(dp[ni][k].hash == st.hash){
                        is_new=false;
                        if(dp[ni][k].ult_m<st.ult_m){
                            dp[ni][k] = st;
                            for(int k_=k;k_>0;k_--){
                                if(dp[ni][k_].ult_m>dp[ni][k_-1].ult_m)swap(dp[ni][k_],dp[ni][k_-1]);
                            }
                        }
                        break;
                    }
                }
                if(!is_new)continue;
                int sz=dp[ni].size();
                dp[ni].pb(st);
                for(int x=sz;x>0;x--){
                    if(dp[ni][x].ult_m>dp[ni][x-1].ult_m)swap(dp[ni][x],dp[ni][x-1]);
                }
                if(sz>=B){
                    auto s_ = dp[ni].back();
                    dp[ni].pop_back();
                    for(int k=0;k<dp2[ni].size();k++){
                        if(dp2[ni][k].hash == st.hash){
                            is_new=false;
                            if(dp2[ni][k].score2<st.score2){
                                dp2[ni][k] = st;
                                for(int k_=k;k_>0;k_--){
                                    if(dp2[ni][k_].score2>dp2[ni][k_-1].score2)swap(dp2[ni][k_],dp2[ni][k_-1]);
                                }
                            }
                            break;
                        }
                    }
                    if(!is_new)continue;
                    int sz_ = dp2[ni].size();
                    dp2[ni].pb(s_);
                    for(int x=sz_;x>0;x--){
                        if(dp2[ni][x].score2>dp2[ni][x-1].score2)swap(dp2[ni][x],dp2[ni][x-1]);
                    }
                    if(sz_>=B2)dp2[ni].pop_back();
                }
            }
            if(res.size()>0&&res[0].ult_m>best.ult_m){
                best=res[0];
            }
            //t2+=clock();
        };
        rep(j,dp[i].size()){
            //cerr<<__LINE__<<" "<<j<<endl;
            f(dp[i][j], (dp[i+1].size()==0)?K:dp[i+1].back().ult_m, (dp2[i+1].size()==0)?K:dp2[i+1].back().score2);
        }
        rep(j,dp2[i].size()){
            //cerr<<__LINE__<<" "<<j<<endl;
            /*if(i==276&&j==0){
                output(dp2[i][j].ret);
                exit(0);
            }*/
            f(dp2[i][j], (dp[i+1].size()==0)?K:dp[i+1].back().ult_m, (dp2[i+1].size()==0)?K:dp2[i+1].back().score2);
        }
    }

    last:;

    //output(best.ret);
    //exit(0);

    //cerr<<best.ult_m<<endl;
    //cerr<<clock()<<endl;
    best=finalize1(best);
    //cerr<<best.ult_m<<endl;
    rep(q,10){
        auto res = get_next(best,1,0,0,0);
        if(res.size()>0&&res[0].ult_m>best.ult_m){
            best = res[0];
        }
        else break;
    }
    output(best.ret);
    //cerr<<best.ult_m<<endl;
    cerr<<clock()<<endl;
    //cerr<<t1<<" "<<t2<<endl;
    //cerr<<t3<<" "<<t4<<" "<<t5<<endl;
    cerr<<best.ult_m<<endl;
    cerr<<clock()<<endl;

    //profile(best);
}