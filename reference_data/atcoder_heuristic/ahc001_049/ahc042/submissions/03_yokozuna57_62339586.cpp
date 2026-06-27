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

template<class T> T T_INF(){ return 1000000000000000000; }
template<> int T_INF<int>(){ return 1000000000; }

int N;
struct A{
    int a[22][22];
    int run(char c, int x){
        int ret;
        if(c=='L'){
            ret=a[x][0];
            rep(i,N-1)a[x][i]=a[x][i+1];
            a[x][N-1]=0;
        }
        else if(c=='R'){
            ret=a[x][N-1];
            for(int i=N-1;i>0;i--)a[x][i]=a[x][i-1];
            a[x][0]=0;
        }
        else if(c=='U'){
            ret=a[0][x];
            rep(i,N-1)a[i][x]=a[i+1][x];
            a[N-1][x]=0;
        }
        else {
            ret=a[N-1][x];
            for(int i=N-1;i>0;i--)a[i][x]=a[i-1][x];
            a[0][x]=0;
        }
        return ret;
    }
    int run(pair<char,int> p){
        return run(p.fr,p.sc);
    }
    void sim(vector<pair<char,int>> seq){
        for(auto p: seq){
            run(p);
        }
    }
};

int score(const A& a){
    int ret=0;
    int cnt_f=0;
    rep(i,N)rep(j,N){
        if(a.a[i][j]==1){
            int cnt0=i+1; rep(i_,i)if(a.a[i_][j]==2)cnt0++;
            int cnt1=N-i; rep(i_,i+1,N)if(a.a[i_][j]==2)cnt1++;
            int cnt2=j+1; rep(j_,j)if(a.a[i][j_]==2)cnt2++;
            int cnt3=N-j; rep(j_,j+1,N)if(a.a[i][j_]==2)cnt3++;
            int c= min(min(cnt0,cnt1),min(cnt2,cnt3));
            ret += (200-c)*c;
        }
        if(a.a[i][j]==2)cnt_f++;
    }
    return ret+10000000*(2*N-cnt_f);
}

vector<string> board;

int PEN=250;

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.precision(20);
    
    cin>>N;
    board.resize(N);
    rep(i,N){
        cin>>board[i];
    }
    
    A init;
    rep(i,N)rep(j,N){
        if(board[i][j]=='.')init.a[i][j]=0;
        else if(board[i][j]=='x')init.a[i][j]=1;
        else if(board[i][j]=='o')init.a[i][j]=2;
    }
    
    vector<pair<char,int>> best_seq;
    while(clock()<800000){
        vector<pair<char,int>> seq;
        //A cur=init;
        A as[1602];
        as[0]=init;
        for(int i=0;i<4*N*N;i++){
            pair<int,pair<pair<int,int>,pair<char,int>>> best(10000000,mp(mp(-1,-1),mp('.',-1)));
            auto f=[&](pair<pair<int,int>,pair<char,int>> p){
                A cur=as[i-p.fr.fr];
                rep(q,p.fr.sc)cur.run(p.sc);
                for(int j=i-p.fr.fr;j<i;j++)cur.run(seq[j]);
                chmin(best,mp(score(cur)+PEN*p.fr.sc,p));
            };
            rep(s,1,4){
                rep(t,min(i+1,100)){
                    rep(i,N){
                        f(mp(mp(t,s),mp('L',i)));
                        f(mp(mp(t,s),mp('R',i)));
                        f(mp(mp(t,s),mp('U',i)));
                        f(mp(mp(t,s),mp('D',i)));
                    }
                }
            }
            if(best.sc.fr.fr==-1)break;
            {
                int t=best.sc.fr.fr;
                int s=best.sc.fr.sc;
                auto p=best.sc.sc;
                auto cur=as[i-t];
                rep(q,s){
                    cur.run(p);
                    as[i-t+1+q]=cur;
                }
                for(int j=i-t;j<i;j++){
                    cur.run(seq[j]);
                    as[j+s+1]=cur;
                }
                rep(q,s)seq.pb(mp('L',-1));
                for(int j=i-1;j>=i-t;j--){
                    seq[j+s]=seq[j];
                }
                rep(q,s)seq[i-t+q]=p;
                i += s-1;
            }
            if(best.fr==PEN*best.sc.fr.sc)break;
        }
        if(best_seq.size()==0||best_seq.size()>seq.size())best_seq=seq;
        PEN+=50;
    }
    
    auto seq=best_seq;
    
    for(auto p: seq){
        cout<<p.fr<<" "<<p.sc<<endl;
    }
    
    /*int sc=0;
    int cnt=40;
    rep(i,N)rep(j,N){
        if(as[seq.size()].a[i][j]==1)cnt++;
        else if(as[seq.size()].a[i][j]==2)cnt--;
    }
    if(cnt==0)cerr<<seq.size()<<endl;
    else cerr<<1600+40*cnt<<endl;*/
}


