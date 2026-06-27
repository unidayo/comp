#include <bits/stdc++.h>
using namespace std;
using pii=pair<int,int>;
using tii=tuple<int,int,int>;
using qii=tuple<int,int,int,int>;
using ll=long long;
using ull=unsigned long long;
using ld=long double;
constexpr int INF=1e9;
constexpr ll INF_ll=1e18;
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



namespace Timer{
    chrono::system_clock::time_point program_start,start;
    void program_start_snap(){
        program_start=start=chrono::system_clock::now();
    }
    void snap(){
        start=chrono::system_clock::now();
    }
    int get_ms(){
        auto now=chrono::system_clock::now();
        int ms=chrono::duration_cast<chrono::milliseconds>(now-start).count();
        return ms;
    }
    int get_ms_all_program(){
        auto now=chrono::system_clock::now();
        int ms=chrono::duration_cast<chrono::milliseconds>(now-program_start).count();
        return ms;
    }
}

//参考：https://ei1333.github.io/library/other/xor-shift.hpp.html

namespace Random{
    uint64_t x=88172645463325252;
    constexpr uint32_t mask=numeric_limits<uint32_t>::max();
    constexpr double k=1.0/(double)(numeric_limits<uint64_t>::max());
    inline uint64_t xorshift64(){
        x^=x<<7;
        x^=x>>9;
        return x;
    }
    inline uint32_t rand_int(uint32_t r){  //[0,r)
        assert(r!=0);
        return (xorshift64()&mask)*r>>32;
    }
    inline int rand_int(int l,int r){  //[l,r)
        assert(l<=r);
        return l+rand_int(r-l);
    }
    inline double rand_double(){  // [0.0,1.0]
        return xorshift64()*k;
    }
}
using Random::rand_int,Random::rand_double;

template<class T> T get_random_element(const vector<T> &v){
    assert(!v.empty());
    return v[rand_int(len(v))];
}

template<class T> void add(vector<T> &a,vector<T> b){
    for(auto i:b) a.push_back(i);
}

constexpr int N=200;
vector<int> H,C;
vector<vector<int>> A;


bool is_topological(const auto &g){
    int n=(int)g.size();
    vector<int> in(n,0);
    for(int i=0;i<n;i++){
        rep(idx,C[i]){
            int j=g[i][idx];
            if(j!=-1) in[j]++;
        }
    }
    queue<int> q;
    for(int i=0;i<n;i++){
        if(in[i]==0) q.push(i);
    }
    
    int cnt=0;
    
    while(!q.empty()){
        int x=q.front();
        q.pop();
        cnt++;
        rep(idx,C[x]){
            int i=g[x][idx];
            if(i==-1) continue;
            in[i]--;
            if(in[i]==0) q.push(i);
        }
    }
    return cnt==n;
}

vector<int> topological_sort(const auto &g){
    int n=(int)g.size();
    vector<int> in(n,0);
    for(int i=0;i<n;i++){
        rep(idx,C[i]){
            int j=g[i][idx];
            if(j!=-1) in[j]++;
        }
    }
    queue<int> q;
    for(int i=0;i<n;i++){
        if(in[i]==0) q.push(i);
    }
    vector<int> ret;
    while(!q.empty()){
        int x=q.front();
        q.pop();
        ret.push_back(x);
        rep(idx,C[x]){
            int i=g[x][idx];
            if(i==-1) continue;
            in[i]--;
            if(in[i]==0) q.push(i);
        }
    }
    if((int)ret.size()!=n) return {};
    return ret;
}

namespace Solver{
    
    /*int calc_ord_score(const vector<int> &ord){
        
        int score=H[ord[0]];
        
        rep(i,N-1){
            score+=max(H[ord[i+1]]-C[i]*A[ord[i]][ord[i+1]],0);
        }
        return score;
    }
    
    vector<int> get_ord(){
        
        vector<int> ord(N);
        rep(i,N) ord[i]=i;
        
        int score=calc_ord_score(ord);
        
        int loop_cnt=0;
        
        while(Timer::get_ms()<100){
            int x=rand_int(N),y=rand_int(N);
            if(x==y) continue;
            vector<int> new_ord=ord;
            
            if(rand_int(2)){
                new_ord.erase(new_ord.begin()+x);
                new_ord.insert(new_ord.begin()+y,ord[x]);
            }else{
                swap(new_ord[x],new_ord[y]);
            }
            int new_score=calc_ord_score(new_ord);
            
            int ms=Timer::get_ms();
            
            double temp=100-100.0/100*ms;
            double prob=exp((score-new_score)/temp);
            if(rand_double()<prob){
                score=new_score;
                ord=new_ord;
            }
            loop_cnt++;
        }
        
        cerr << loop_cnt << endl;
        
        return ord;
    }*/
    
    
    int calc_graph_score(const auto &graph){
        
        if(topological_sort(graph).empty()) return INF;
        
        vector<int> h=H;
        
        rep(i,N){
            rep(idx,C[i]){
                int j=graph[i][idx];
                if(j!=-1) h[j]-=A[i][j];
            }
        }
        
        int ret=0;
        
        rep(i,N){
            ret+=max(0,h[i]);
        }
        
        return ret;
        
    }
    
    auto get_graph(){
        
        vector<int> ord(N);
        
        rep(i,N) ord[i]=i;
        
        sort(all(ord),[&](int i,int j){return H[i]<H[j];});
        
        array<array<int,6>,N> graph;
        
        rep(i,N-1){
            rep(j,C[ord[i]]){
                graph[ord[i]][j]=ord[i+1];
            }
        }
        
        rep(j,C[ord.back()]){
            graph[ord.back()][j]=-1;
        }
        
        int score=0;
        
        
        vector<int> h=H;
        
        rep(i,N){
            rep(idx,C[i]){
                int j=graph[i][idx];
                if(j!=-1) h[j]-=A[i][j];
            }
        }
        
        rep(i,N){
            score+=max(0,h[i]);
        }
        
        //assert(score==calc_graph_score(graph));
        
        
        int loop_cnt=0;
        
        vector<vector<int>> good_attack(N);
        rep(i,N){
            rep(j,N){
                good_attack[i].push_back(j);
            }
            sort(all(good_attack[i]),[&](int a,int b){return A[i][a]>A[i][b];});
            good_attack[i].resize(20);
        }
        
        int ms=0;
        
        int best_score=score;
        
        auto best_graph=graph;
        
        int update_cnt=0;
        
        constexpr int T=1<<17;
        
        array<double,T> ln_rand;
        
        rep(i,T){
            ln_rand[i]=log(rand_double());
        }
        
        while(true){
            
            loop_cnt++;
            
            int a=rand_int(N);
            
            int b=rand_int(C[a]),nx=get_random_element(good_attack[a]);
            
            if(graph[a][b]==nx) continue;
            
            int px=graph[a][b];
            
            int new_score=score;
            
            if(px!=-1){
                new_score-=max(0,h[px]);
                new_score+=max(0,h[px]+A[a][px]);
            }
            new_score-=max(0,h[nx]);
            new_score+=max(0,h[nx]-A[a][nx]);
            
            
            
            if((loop_cnt&63)==63) ms=Timer::get_ms();
            
            if(1990<ms) break;
            
            double temp=10-10.0/1990*ms;
            
            double target_score=score-temp*ln_rand[loop_cnt%T];
            
            if(target_score<new_score) continue;
            
            graph[a][b]=nx;
            
            if(!is_topological(graph)){
                graph[a][b]=px;
                continue;
            }
            
            update_cnt++;
            
            score=new_score;
            
            h[px]+=A[a][px];
            h[nx]-=A[a][nx];
            
            if(chmin(best_score,score)){
                best_graph=graph;
            }
            
        }
        
        cerr << loop_cnt << ' ' << update_cnt << ' ' << best_score << endl;
        
        return best_graph;
    }
    
    void solve(){
        
        
        
        auto graph=get_graph();
        
        
        vector<int> ord=topological_sort(graph);
        
        assert(!ord.empty());
        
        
        vector<pii> ans;
        
        
        vector<vector<int>> weapons(N);
        
        for(auto i:ord){
            int x=H[i];
            
            sort(all(weapons[i]),[&](int a,int b){return A[a][i]<A[b][i];});
            
            while(0<x){
                
                if(weapons[i].empty()) break;
                
                x-=A[weapons[i].back()][i];
                ans.emplace_back(weapons[i].back(),i);
                weapons[i].pop_back();
            }
            
            while(0<x){
                x--;
                ans.emplace_back(-1,i);
            }
            rep(idx,C[i]){
                int j=graph[i][idx];
                
                if(j==-1) continue;
                weapons[j].push_back(i);
                
            }
        }
        
        for(auto [w,b]:ans){
            cout << w << ' ' << b << '\n';
        }
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    Timer::program_start_snap();
    int n;
    cin >> n;
    assert(n==N);
    H.resize(N);
    rep(i,N) cin >> H[i];
    C.resize(N);
    rep(i,N) cin >> C[i];
    A.resize(N,vector<int>(N));
    rep(i,N){
        rep(j,N){
            cin >> A[i][j];
        }
    }


    Solver::solve();
    exit(0);
}