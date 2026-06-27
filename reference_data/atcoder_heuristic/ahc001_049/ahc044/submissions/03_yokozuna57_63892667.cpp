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

uint32_t RAND()
{
    static uint32_t x = 57;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

int N,L;
int T[100];

struct A{
    int a[100][2];
    int sum[100];
    int sc2;
    void mksc2(){
        rep(i,100){
            sum[i]=0;
        }
        rep(i,100){
            sum[a[i][0]] += T[i]/2;
            sum[a[i][1]] += (T[i]+1)/2;
        }
        rep(i,100){
            sc2 += abs(T[i]-sum[i]);
        }
    }
    void output(){
        rep(i,100){
            cout<<a[i][1]<<" "<<a[i][0]<<endl;
        }
    }

    void mknxt(int r,int s,int t);
};

void A::mknxt(int r,int s,int t){
    int cur = a[r][s];
    sc2 -= abs(T[cur]-sum[cur]);
    sum[cur] -= (T[r]+s)/2;
    sc2 += abs(T[cur]-sum[cur]);
    sc2 -= abs(T[t]-sum[t]);
    sum[t] += (T[r]+s)/2;
    sc2 += abs(T[t]-sum[t]);
    a[r][s] = t;
}

int score(const A &ans){
    int cnt[100]={};
    int now=0;
    rep(q,500000){
        cnt[now]++;
        now=ans.a[now][cnt[now]&1];
    }
    int ret=0;
    rep(i,100){
        ret+=abs(cnt[i]-T[i]);
    }
    return ret;
}

/*int score2(const A &ans){
    int cnt[100]={};
    rep(i,100){
        cnt[ans.a[i][0]] += T[i]/2;
        cnt[ans.a[i][1]] += (T[i]+1)/2;
    }
    int ret=0;
    rep(i,100){
        ret+=abs(cnt[i]-T[i]);
    }
    return ret;
}*/

int fixed_from[100][2];
int fixed_to[100];

A mkinit(){
    {
        rep(i,100){
            fixed_from[i][0]=fixed_from[i][1]=-1;
            fixed_to[i]=-1;
        }

        vector<pair<int,pair<int,int>>> bests;
        rep(j,N){
            pair<int,pair<int,int>> best(1000000,mp(-1,-1));
            rep(i,N){
                if(j==i)continue;
                chmin(best,mp(abs(T[j]-T[i]/2),mp(i,j)));
            }
            if(best.fr<=21)bests.pb(best);
        }
        sort(all(bests));

        for(auto p: bests){
            int i=p.sc.fr;
            int j=p.sc.sc;
            if(fixed_from[i][0]==-1){
                fixed_from[i][0]=j;
                fixed_to[j]=i;
            }
            else if(fixed_from[i][1]==-1){
                fixed_from[i][1]=j;
                fixed_to[j]=i;
            }
        }
    }
    /*{
        rep(i,100){
            fixed_from[i][0]=fixed_from[i][1]=-1;
            fixed_to[i]=-1;
        }

        pair<int,pair<int,int>> best(1000000,mp(-1,-1));
        pair<int,pair<int,int>> best2(1000000,mp(-1,-1));
        rep(i,N){
            rep(j,N){
                if(j==i)continue;
                if(best.sc.sc==j){
                    chmin(best,mp(abs(T[j]-T[i]/2),mp(i,j)));
                }
                else {
                    chmin(best2,mp(abs(T[j]-T[i]/2),mp(i,j)));
                    if(best>best2){
                        swap(best,best2);
                    }
                }
            }
        }
        {
            int i=best.sc.fr;
            int j=best.sc.sc;
            fixed_from[i][0]=j;
            fixed_to[j]=i;
        }
        {
            int i=best2.sc.fr;
            int j=best2.sc.sc;
            fixed_from[i][1]=j;
            fixed_to[j]=i;
        }
    }*/

    A ret;
    int cnt[100] = {};

    rep(i,N){
        rep(t,2){
            if(fixed_from[i][t]>=0){
                ret.a[i][t]=fixed_from[i][t];
                cnt[fixed_from[i][t]] += (T[i]+t)/2;
            }
        }
    }

    vector<pair<int,int>> vs;
    rep(i,N){
        vs.pb(mp(T[i],i));
    }
    sort(all(vs));
    reverse(all(vs));
    for(auto p: vs){
        rep(t,2){
            if(fixed_from[p.sc][t]>=0)continue;
            pair<int,int> best(-1000000,-1);
            rep(i,N){
                if(i!=p.sc)chmax(best,mp(T[i]-cnt[i],i));
            }
            ret.a[p.sc][t] = best.sc;
            cnt[best.sc] += (T[p.sc]+1)/2;
        }
    }
    return ret;
}

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.precision(20);

    cin>>N>>L;
    rep(i,N)cin>>T[i];

    A best;
    {
        rep(i,N){
            best.a[i][0]=rand()%N;
            best.a[i][1]=rand()%N;
        }
    }
    best = mkinit();
    best.mksc2();
    int best_sc = score(best);
    //A cur = best;
    rep(q,200){
        int CNT=0;
        A a = best;
        //a = cur;
        //A temp = a;
        rep(qq,1){
            //a = temp;
            rep(q2,200000){
                //a.mknxt(rand()%N,rand()%2,rand()%N);
                //a.mknxt(rand()%N,rand()%2,rand()%N);
                int cur_sc = a.sc2;
                int r[2],s[2],cur[2],nex[2];
                rep(t,2){
                    do{
                        r[t]=RAND()%100;
                        s[t]=RAND()%2;
                        cur[t]=a.a[r[t]][s[t]];
                    }
                    while(fixed_from[r[t]][s[t]]>=0);
                }
                if(RAND()%4!=0){
                    rep(t,2){
                        nex[t]=cur[t^1];
                    }
                }
                else {
                    rep(t,2){
                        nex[t]=RAND()%100;
                    }
                }
                rep(t,2){
                    while(nex[t]==r[t]||fixed_to[nex[t]]>=0/*||(nex[t]==a.a[r[t]][s[t]^1]&&r[0]!=r[1])*/){
                        nex[t]=RAND()%100;
                    }
                }
                rep(t,2){
                    a.mknxt(r[t],s[t],nex[t]);
                }
                if(cur_sc >= a.sc2 || q2==0){
                    CNT=q2;
                }
                else {
                    for(int t=1;t>=0;t--){
                        a.mknxt(r[t],s[t],cur[t]);
                    }
                }
            }
            //if(a.sc2 <= temp.sc2)temp = a;
        }
        int sc = score(a);
        if(best_sc > sc){
            best_sc = sc;
            best = a;
        }
        //cur = a;
        cerr<<q<<": "<<best_sc<<" "<<sc<<" "<<a.sc2<<" "<<CNT<<endl;
    }

    cerr<<clock()<<endl;
    cerr<<1000000-best_sc<<endl;
    best.output();
}