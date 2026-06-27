#include <bits/stdc++.h>
using namespace std;
using pii=pair<int,int>;
using tii=tuple<int,int,int>;
using qii=tuple<int,int,int,int>;
using ll=long long;
using ull=unsigned long long;
using ld=long double;
const int INF=(int)2e9+1;
const ll INF_ll=1e18;
#define rep(i,n) for (int i=0;i<(int)(n);i++)
#define replr(i,l,r) for (int i=(int)(l);i<(int)(r);i++)
#define all(v) v.begin(),v.end()
#define len(v) ((int)v.size())
template<class T> inline bool chmin(T &a,T b){
    if(a>b){
        a=b;
        return true;
    }
    return false;
}
template<class T> inline bool chmax(T &a,T b){
    if(a<b){
        a=b;
        return true;
    }
    return false;
}

namespace timer{
    auto start=chrono::system_clock::now();
    void snap(){
        start=chrono::system_clock::now();
    }
    int get_ms(){
        auto now=chrono::system_clock::now();
        int ms=chrono::duration_cast<chrono::milliseconds>(now-start).count();
        return ms;
    }
}

mt19937 mt;
int rand_int(int r){  //[0,r)
    assert(r!=0);
    return mt()%r;
}
int rand_int(int l,int r){  //[l,r)
    return l+mt()%(r-l);
}
double rand_double(){  //[0.0,1.0]
    return (double)mt()/(double)mt19937::max();
}

constexpr int N=1000;

vector<int> A,B;

namespace Solver{

    void output_answer(vector<vector<int>> ans){
        int m=len(ans);
        assert(m<=5*N);
        cout << m << '\n';
        ll c=0;
        rep(i,m){
            assert(len(ans[i])==4);
            assert(ans[i][0]<=ans[i][2]);
            assert(ans[i][1]<=ans[i][3]);
            cout << ans[i][0] << ' ' << ans[i][1] << ' ' << ans[i][2] << ' ' << ans[i][3] << '\n';
            c+=ans[i][2]-ans[i][0];
            c+=ans[i][3]-ans[i][1];
        }

        int l=0;
        rep(i,N){
            chmax(l,A[i]);
            chmax(l,B[i]);
        }
        cerr << (ll)round(1e6*(double)((double)((ll)N*l)/(double)(1+c))) << ' ' << timer::get_ms();
    }


    ll point_dist(int ax,int ay,int bx,int by){
        return abs((ll)ax-(ll)bx)+abs((ll)ay-(ll)by);
    }

    ll calc_cost(vector<pii> &point){
        if(5*N<len(point)) return INF_ll;
        vector<pii> v=point;
        sort(all(v),[](pii i,pii j){
            if(i.first==j.first) return i.second<j.second;
            return i.first<j.first;
        });
        vector<pii> vp={{0,0}};
        ll ret=0;
        rep(i,len(point)){
            auto [a,b]=v[i];
            pii mn={-1,-1};
            int cost=INF;
            for(auto [x,y]:vp){
                if(x<=a&&y<=b){
                    if(chmin(cost,(a-x)+(b-y))) mn={x,y};
                }
            }
            ret+=a-mn.first;
            ret+=b-mn.second;
            vp.emplace_back(a,b);
        }
        return ret;
    }

    vector<vector<int>> point_to_ans(vector<pii> &point){
        vector<pii> v=point;
        sort(all(v),[](pii i,pii j){
            if(i.first==j.first) return i.second<j.second;
            return i.first<j.first;
        });
        vector<pii> vp={{0,0}};
        vector<vector<int>> ans;
        rep(i,len(point)){
            auto [a,b]=v[i];
            pii mn={-1,-1};
            int cost=INF;
            for(auto [x,y]:vp){
                if(x<=a&&y<=b){
                    if(chmin(cost,(a-x)+(b-y))) mn={x,y};
                }
            }
            vp.emplace_back(a,b);
            ans.push_back({mn.first,mn.second,a,b});
        }
        return ans;
    }

    void solve(){
        vector<pii> point(N);
        rep(i,N) point[i]={A[i],B[i]};


        map<pii,vector<pair<int,pii>>> min_dist_k;

        int nx=-1,ny=-1;

        constexpr int K=11;

        while(len(point)<5*N){
            sort(all(point),[](pii i,pii j){
                if(i.first==j.first) return i.second<j.second;
                return i.first<j.first;
            });
            vector<pii> vp={{0,0}};
            map<pii,vector<tii>> graph;
            rep(i,len(point)){
                auto [a,b]=point[i];
                vector<pair<int,pii>> vc;
                if(!min_dist_k.contains({a,b})){
                    for(auto [x,y]:vp){
                        if(x<=a&&y<=b){
                            vc.emplace_back((a-x)+(b-y),(pii){x,y});
                        }
                    }
                }else{
                    vc=min_dist_k[{a,b}];
                    if(nx<=a&&ny<=b){
                        vc.emplace_back((a-nx)+(b-ny),(pii){nx,ny});
                    }
                }
                sort(all(vc));
                while(K<len(vc)) vc.pop_back();
                min_dist_k[{a,b}]=vc;
                int mn=vc[0].first;
                rep(j,min(len(vc),K)){
                    graph[{vc[j].second.first,vc[j].second.second}].emplace_back(a,b,mn);
                    if(j==0) vp.emplace_back(a,b);
                }
            }
            double max_diff=1;
            int max_diff_x=-1,max_diff_y=-1;
            vector<pair<int,pii>> vec;
            for(auto [s,vg]:graph){
                auto [sx,sy]=s;
                int c=0;
                for(auto g:vg){
                    auto [ax,ay,ca]=g;
                    int c2=0;
                    for(auto g2:vg){
                        if(g==g2) continue;
                        if(c<=c2) break;
                        auto [bx,by,cb]=g2;
                        int x=sx+min(ax-sx,bx-sx),y=sy+min(ay-sy,by-sy);
                        ll pre_cost=(ll)ca+(ll)cb,new_cost=point_dist(sx,sy,x,y)+point_dist(x,y,ax,ay)+point_dist(x,y,bx,by);
                        if(pre_cost<=new_cost) continue;
                        if(chmax(max_diff,(double)(2e9-(double)(sx+sy))*(double)pre_cost/(double)new_cost)){
                            max_diff_x=x;
                            max_diff_y=y;
                        }
                        c2++;
                    }
                    c++;
                }
            }
            if(max_diff_x==-1||max_diff_y==-1) break;
            point.emplace_back(max_diff_x,max_diff_y);

            nx=max_diff_x;
            ny=max_diff_y;

            //cerr << max_diff << " "<< len(point) << endl;

            //if(1900<timer::get_ms()) break;
        }
        output_answer(point_to_ans(point));
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    A.resize(N);
    B.resize(N);
    rep(i,n) cin >> A[i] >> B[i];

    Solver::solve();
    exit(0);
}