#include <bits/stdc++.h>

using namespace std;
#define int long long
mt19937_64 rnd;
#define app push_back
#define all(x) (x).begin(),(x).end()
#ifdef LOCAL
#define debug(...) [](auto...a){ ((cout << a << ' '), ...) << endl;}(#__VA_ARGS__, ":", __VA_ARGS__)
#define debugv(v) do {cout<< #v <<" : {"; for(int izxc=0;izxc<v.size();++izxc) {cout << v[izxc];if(izxc+1!=v.size()) cout << ","; }cout <<"}"<< endl;} while(0)
#else
#define debug(...)
#define debugv(v)
#endif
#define lob(a,x) lower_bound(all(a),x)
#define upb(a,x) upper_bound(all(a),x)
#define print(x) cout<<(x)<<'\n'
int get(int l,int r) {
    return l+rnd()%(r-l);
}
int n,m,l,u;
//bitset<20000000> ban;
const int mans=35;
int cnt[500];
int ID[50];
int32_t main()
{
    ios_base::sync_with_stdio(false);cin.tie(0);cout.tie(0);
    #ifdef LOCAL
    freopen("100","r",stdin);
    #endif
    cin>>n>>m>>l>>u;
    int mid=(l+u)/2;
    int B=(mid-l);
    int le=mid/6-B;int ri=mid/6+B;
    vector<int> a(n);
    for(int i=0;i<n;++i){
        a[i]=get(le,ri);
    }
    for(int i=0;i<n;++i) {cout<<a[i]<<' ';} cout<<endl;
    vector<int> x(n,-1);
    vector<int> b(m);for(int &x:b) {cin>>x;}
    vector<pair<int,int> > su;
    for(int i=0;i<n;++i) {
        for(int j=i+1;j<n;++j) {
            for(int k=j+1;k<n;++k) {
                if(k-i<=140)
                su.app({a[i]+a[j]+a[k],i*n*n+j*n+k});
            }
        }
    }
    //vector<int> sh(m);iota(all(sh),0LL);
    //sort(all(sh),[&](int i,int j) {return abs(b[i]-mid)>abs(b[j]-mid);});
    sort(all(su));
    //debug(clock());
    debug(su.size());
    //debug("sorted");
    vector<array<int,6> > uu[m];
    auto f=[&](array<int,6> u) {
            int s=0;
            for(int i:u) s+=a[i];
            return s;
        };
    for(int i=0;i<m;++i) {
        //debug(b[i]);
        //debug(i);
        int r=su.size()-1;
        for(int l=0;l<su.size();++l) {
            int I=su[l].second/(n*n);
            int J=(su[l].second/n)%n;
            int K=su[l].second%n;
            {
            //if(rnd()%10000==0 && i==22) debug(l,r);
            while(r>l && su[r].first+su[l].first>b[i]) {
                --r;
            }
            if(r<=l) break;
            int r1=r;
            while(r1>l) {
            if(su[l].first+su[r1].first<b[i]-mans){
                break;
            }
            int I1=su[r1].second/(n*n);
            int J1=(su[r1].second/n)%n;
            int K1=su[r1].second%n;
            if(I1!=I && J1!=I && K1!=I && I1!=J && J1!=J && K1!=J && I1!=K && J1!=K && K1!=K) {
                uu[i].app({I,J,K,I1,J1,K1});
            }
            --r1;
            }
            }
            }
            sort(all(uu[i]),[&](array<int,6> u1,array<int,6> u2) {return f(u1)>f(u2);});
        debug(i,uu[i].size());
    }
    debug(clock());
    fill(ID,ID+50,0);
    for(int i=0;i<50;++i) {
            if(ID[i]!=(-1)) {
                for(int j:uu[i][ID[i]]) {
                    cnt[j]++;
                }
            }
    }
    while(true) {
        int mx=*max_element(cnt,cnt+500);
        int o=*min_element(ID,ID+50);
        if(mx<=1 && o>=0) break;
        for(int i=0;i<50;++i) {
            if(ID[i]!=(-1)) {
                for(int j:uu[i][ID[i]]) {
                    cnt[j]--;
                }
            }
            ID[i]=(-1);
            vector<int> bs;int curbest=1e18;
            for(int k=0;k<uu[i].size();++k) {
                int s=0;
                for(int j:uu[i][k]) {
                    s+=cnt[j];
                }
                if(s<curbest) {curbest=s;bs={k};}
                else if(s==curbest) {bs.app(k);}
            }
            //if(curbest) {debug(curbest);debugv(bs);}
            ID[i]=bs[0];
            for(int k=1;k<bs.size();++k) {
                if(rnd()%2) break;
                ID[i]=bs[k];
            }
            //ID[i]=bs[rnd()%(bs.size())];
            if(rnd()%100==0) {
                ID[i]=rnd()%uu[i].size();
            }
            for(int j:uu[i][ID[i]]) {
                cnt[j]++;
            }
        }
    }
    for(int iter=0;iter<100;++iter) {
        //int mx=*max_element(cnt,cnt+500);
        //int o=*min_element(ID,ID+50);
        //if(o>=0) break;
        for(int i=0;i<50;++i) {
            if(ID[i]!=(-1)) {
                for(int j:uu[i][ID[i]]) {
                    cnt[j]--;
                }
            }
            ID[i]=(-1);
            vector<int> bs;int curbest=1e18;
            for(int k=0;k<uu[i].size();++k) {
                int s=0;
                for(int j:uu[i][k]) {
                    s+=cnt[j];
                }
                if(s<curbest) {curbest=s;bs={k};}
                else if(s==curbest) {bs.app(k);}
            }
            //if(curbest) {debug(curbest);debugv(bs);}
            ID[i]=bs[0];
            for(int j:uu[i][ID[i]]) {
                cnt[j]++;
            }
        }
    }
    for(int i=0;i<50;++i) debug(ID[i]);
    for(int i=0;i<50;++i) for(int j:uu[i][ID[i]]) x[j]=i;
    for(int i=0;i<n;++i) {if(x[i]!=(-1)) b[x[i]]-=a[i]; cout<<x[i]+1<<' ';} cout<<endl;
    debugv(b);
    debug(accumulate(all(b),0LL));
    return 0;
}
