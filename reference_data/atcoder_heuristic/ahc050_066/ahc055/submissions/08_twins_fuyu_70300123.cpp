#pragma GCC optimize("O3,unroll-loops")
#include <bits/stdc++.h>   
using namespace std;
using vi = vector<int>; using vvi = vector<vi>;using vvvi = vector<vvi>;
using vb = vector<int>; using vvb = vector<vb>;using vvvb = vector<vvb>;
using pi = pair<int,int>; using vpi = vector<pi>;using vvpi = vector<vpi>;
#define rep1(a) for (int _ = 0; _ < (a); ++_)
#define rep2(i, a) for (int i = 0; i < (a); ++i)
#define rep3(i, a, b) for (int i = (a); i < (b); ++i)
#define rrep1(a) for (int i = (a)-1; i >= 0; --i)
#define rrep2(i, a) for (int i = (a)-1; i >= 0; --i)
#define rrep3(i, a, b) for (int i = (b)-1; i >= (a); --i)
#define overload3(a, b, c, d, ...) d
#define rep(...) overload3(__VA_ARGS__, rep3, rep2, rep1)(__VA_ARGS__)
#define rrep(...) overload3(__VA_ARGS__, rrep3, rrep2, rrep1)(__VA_ARGS__)
#define in(i,vec) for (auto i:(vec))
template<class T> using pqueue = priority_queue<T, vector<T>>;//大きい順
template<class T> using pqueue_g = priority_queue<T, vector<T>, greater<T>>;//小さい順

#define RESET   "\033[0m"
#define RED     "\033[38;2;255;0;0m"     // #FF0000
#define GREEN   "\033[38;2;0;255;0m"     // #00FF00
#define BLUE    "\033[38;2;0;0;255m"     // #0000FF
#define BROWN   "\033[38;2;165;42;42m"   // #A52A2A (Web標準のBrown, 彩度強め)

class xorshift128plus {
private:
    uint64_t s[2];

public:
    using result_type = uint64_t;

    xorshift128plus(uint64_t seed = 1) {
        seed_gen(seed);
    }

    void seed(uint64_t seed_val) {
        seed_gen(seed_val);
    }

    static constexpr result_type min() {
        return std::numeric_limits<result_type>::min();
    }

    static constexpr result_type max() {
        return std::numeric_limits<result_type>::max();
    }

    result_type operator()() {
        uint64_t s1 = s[0];
        uint64_t s0 = s[1];
        s[0] = s0;
        s1 ^= s1 << 23;
        s[1] = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26);
        return s[1] + s0;
    }

private:
    void seed_gen(uint64_t seed_val) {
        // SplitMix64 で状態2個を初期化
        s[0] = splitmix64(seed_val);
        s[1] = splitmix64(s[0]);
    }

    static uint64_t splitmix64(uint64_t& x) {
        x += 0x9E3779B97F4A7C15ULL;
        uint64_t z = x;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }
};
xorshift128plus rng(42);

struct Timer{
    chrono::steady_clock::time_point start;
    Timer(): start(chrono::steady_clock::now()) {}
    double elapse(){
        return chrono::duration<double>(chrono::steady_clock::now()-start).count();
    }
    bool over(double t){
        return elapse() > t;
    }
    bool operator()(double t){
        return elapse() < t;
    }
};
Timer timer;

struct Temperature{
    double start;
    double end;

    Temperature(double start_, double end_): start(start_), end(end_){}

    double operator()(double s){
        return start * pow(end/start, s);
    }
};

constexpr int N=200;
int C[N];
int H[N];
int A[N][N];
int H_sum=0;

vi Vlist20[N],Vinvlist20[N],Vlist10[N],Vinvlist10[N],Vlist2[N],Vinvlist2[N];

void get_input(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int _; cin>>_;
    rep(i,N) cin>>H[i], H_sum += H[i];
    rep(i,N) cin >> C[i];
    rep(i,N) rep(j,N) cin>>A[i][j];
    rep(i,N)rep(j,N)if(A[i][j]>=2){
        Vlist2[i].push_back(j);
        Vinvlist2[j].push_back(i);
        if(A[i][j]>=10){
            Vlist10[i].push_back(j);
            Vinvlist10[j].push_back(i);
            if(A[i][j]>=20){
                Vlist20[i].push_back(j);
                Vinvlist20[j].push_back(i);
            }
        }
    }
};

int random_v(int u){
    vi& lst = timer.elapse() < 1.5? Vlist20[u]:
             timer.elapse() < 1.8? Vlist10[u]:
             Vlist2[u];
    if(lst.empty()) return u;
    return lst[rng() % lst.size()];
}
int random_vinv(int u){
    vi& lst = timer.elapse() < 1.5? Vinvlist20[u]:
             timer.elapse() < 1.8? Vinvlist10[u]:
             Vinvlist2[u];
    if(lst.empty()) return u;
    return lst[rng() % lst.size()];
}

struct Action{
    int type = 0; // 0:add_edge, 1:remove_edge, 2:change_edge, 3:swap_edge 4:change_src
    int a,b,c,d;
};

int stk[N];
int vis[N];

struct State{
    vi G[N];
    int h[N];
    int score;

    bool has_path(int src, int dst){
        memset(vis,0,sizeof(vis));
        stk[0]=src;
        vis[src]=1;
        int head=1;
        while(head){
            int v=stk[--head];
            if(v==dst) return true;
            in(u,G[v]){
                if(!vis[u]){
                    vis[u]=1;
                    stk[head++]=u;
                }
            }
        }
        return false;
    }

    bool isValidAction(const Action& action){
        auto[t,a,b,c,d] = action;
        if(t == 0){
            if(G[a].size() >= C[a]) return false;
            if(find(G[a].begin(),G[a].end(),b) != G[a].end()) return true;
            if(has_path(b,a)) return false;
            return true;
        }
        else if(t == 1){
            return true;
        }
        else if(t == 2){
            if(find(G[a].begin(),G[a].end(),c) != G[a].end()) return true;
            if(has_path(c,a)) return false;
            return true;
        }
        else if(t == 3){
            if(a==c) return false;
            if(has_path(b,c)) return false;
            if(has_path(d,a)) return false;
            return true;
        }
        else if (t == 4){
            if(G[b].size() >= C[b]) return false;
            if(has_path(c,b)) return false;
            return true;
        }
        else{
            return false;
        }
    }

    int score_diff(const Action& action){
        auto[t,a,b,c,d] = action;
        if(t == 0){
            int hb_new = h[b] - A[a][b];
            return max(0, hb_new) - max(0, h[b]) + 1;
        }
        else if(t == 1){
            int hb_new = h[b] + A[a][b];
            return max(0, hb_new) - max(0, h[b]) - 1;
        }
        else if(t == 2){
            int hb_new = h[b] + A[a][b];
            int hc_new = h[c] - A[a][c];
            return (max(0, hc_new) - max(0, h[c])) + (max(0, hb_new) - max(0, h[b]));
        }
        else if(t == 3){
            int hb_new = h[b] + A[a][b] - A[c][b];
            int hd_new = h[d] + A[c][d] - A[a][d];
            return (max(0, hb_new) - max(0, h[b])) + (max(0, hd_new) - max(0, h[d]));    
        }
        else if (t == 4){
            int hc_new = h[c] + A[a][c] - A[b][c];
            return max(0, hc_new) - max(0, h[c]);
        }
        else{
            return 0;
        }
    }

    void applyAction(const Action& action){
        auto[t,a,b,c,d] = action;
        if(t==0){
            G[a].push_back(b);
            int hb_new = h[b] - A[a][b];
            score += max(0, hb_new) - max(0, h[b]) + 1;
            h[b] = hb_new;
        }
        else if(t==1){
            G[a].erase(find(G[a].begin(),G[a].end(),b));
            int hb_new = h[b] + A[a][b];
            score += max(0, hb_new) - max(0, h[b]) - 1;
            h[b] = hb_new;
        }
        else if(t==2){
            G[a].erase(find(G[a].begin(),G[a].end(),b));
            int hb_new = h[b] + A[a][b];
            score += max(0, hb_new) - max(0, h[b]);
            h[b] = hb_new;

            G[a].push_back(c);
            int hc_new = h[c] - A[a][c];
            score += max(0, hc_new) - max(0, h[c]);
            h[c] = hc_new;
        }
        else if(t==3){
            G[a].erase(find(G[a].begin(),G[a].end(),b));
            G[c].push_back(b);
            int hb_new = h[b] + A[a][b] - A[c][b];
            score += max(0, hb_new) - max(0, h[b]);
            h[b] = hb_new;

            G[c].erase(find(G[c].begin(),G[c].end(),d));
            G[a].push_back(d);
            int hd_new = h[d] + A[c][d] - A[a][d];
            score += max(0, hd_new) - max(0, h[d]);
            h[d] = hd_new;
        }
        else if(t==4){
            G[a].erase(find(G[a].begin(),G[a].end(),c));
            G[b].push_back(c);
            int hc_new = h[c] + A[a][c] - A[b][c];
            score += max(0, hc_new) - max(0, h[c]);
            h[c] = hc_new;
        }
    }

    void output(){
        int deg[N] ={};
        rep(u, N) for(auto v:G[u]) deg[v]++;
        queue<int> que;
        rep(u,N)if(deg[u]==0) que.push(u);
        int H_copy[N];
        rep(i,N) H_copy[i] = H[i];
        while(!que.empty()){
            int u = que.front(); que.pop();
            rep(i,h[u]){
                cout << "-1 " << u << endl;
                H_copy[u]--;
            }
            for(auto v:G[u]){
                if(H_copy[v] > 0){
                    cout << u << " " << v << endl;
                    H_copy[v]-=A[u][v];
                }
                deg[v]--;
                if(deg[v]==0) que.push(v);
            }
        }
    }

    void clean(){
        vi Ginv[N];
        rep(u,N) in(v,G[u]) Ginv[v].push_back(u);
        rep(u,N)if(h[u]<0){
            sort(Ginv[u].begin(),Ginv[u].end());
        }
    }

    Action random_action(){
        Action action;
        do{
            action.type = rng() % 5;
        }while(action.type == 3);
        if(action.type == 0){
            action.a = rng() % N;
            action.b = random_v(action.a);
        }
        else if(action.type == 1){
            action.a = rng() % N;
            if(G[action.a].empty()){
                action.type = 0;
                action.b = random_v(action.a);
            }
            else{
                action.b = G[action.a][rng() % G[action.a].size()];
            }
        }
        else if(action.type == 2){
            action.a = rng() % N;
            if(G[action.a].empty()){
                action.type = 0;
                action.b = random_v(action.a);
            }
            else{
                action.b = G[action.a][rng() % G[action.a].size()];
                action.c = random_v(action.a);
            }
        }
        else if(action.type == 3){
            action.a = rng() % N;
            if(G[action.a].empty()){
                action.type = 0;
                action.b = random_v(action.a);
            }
            else{
                action.b = G[action.a][rng() % G[action.a].size()];
                action.c = random_v(action.a);
                if(G[action.c].empty()){
                    action.type = 0;
                    action.b = random_v(action.a);
                }
                else{
                    action.d = G[action.c][rng() % G[action.c].size()];
                }
            }
        }
        else if(action.type == 4){
            action.a = rng() % N;
            if(G[action.a].empty()){
                action.type = 0;
                action.b = random_v(action.a);
            }
            else{
                action.c = G[action.a][rng() % G[action.a].size()];
                action.b = random_vinv(action.c);
            }
        }
        return action;
    }

    int true_score(){
        return H_sum + 1 - score;
    }

    int debug_score(){
        int res = 0;
        rep(i,N){
            res += max(0, h[i]);
        }
        return H_sum + 1 - res;
    }

    int debug_score2(){
        int res = 0;
        int h_debug[N] = {};
        rep(i,N) h_debug[i] = H[i];
        rep(i,N){
            in(v,G[i]){
                h_debug[v] -= A[i][v];
            }
        }
        rep(i,N){
            res += max(0, h_debug[i]);
        }
        return H_sum + 1 - res;
    }

    void debug_info(){
        cerr << "Score" << score << endl;
        int n_leaves = 0;
        int unused_edges = 0;
        rep(i,N){
            if(G[i].empty()) n_leaves++;
            unused_edges += C[i] - G[i].size();
        }
        cerr << "Leaf nodes: " << n_leaves << endl;
        cerr << "Unused edges: " << unused_edges << endl;
    }

};


State initial_state(){
    State state;
    state.score  = 0;
    rep(i,N){
        state.G[i].clear();
        state.score += H[i];
        state.h[i] = H[i];
    }
    return state;
}

struct SA{
    State state;
    State best;
    int best_score;
    SA(const State& initial): state(initial) { 
        best = state;
        best_score = state.score;
    }
    bool step(double temperature){
        Action action = state.random_action();
        if(state.isValidAction(action)){
            int diff = state.score_diff(action);
            if(diff < 0 || exp(-diff/temperature) > (double)(rng() % 1000000)/1000000.0){
                state.applyAction(action);
                if(state.score < best_score){
                    best = state;
                    best_score = state.score;
                }
                return true;
            }
        }
        return false;
    }
};

struct VecSa{
    vector<SA> sas;
    VecSa(const State& initial, int n): sas(n, SA(initial)){}
    int step(double temperature){
        int acc = 0;
        for(auto& sa:sas){
            acc += sa.step(temperature);
        }
        return acc;
    }
    State best(){
        State best_state = sas[0].best;
        int best_score = sas[0].best_score;
        for(auto& sa:sas){
            if(sa.best_score < best_score){
                best_state = sa.best;
                best_score = sa.best_score;
            }
        }
        return best_state;
    }
};

constexpr double TIME_LIMIT = 1.9;

int main(){
    get_input();
    State master = initial_state();
    SA sa(master);
    Temperature alpha(30.0, 1.0);
    uint64_t iter=0,acc=0;
    while(timer(TIME_LIMIT)){
        acc += sa.step(alpha(timer.elapse()/TIME_LIMIT));
        iter++;
    }
    sa.state = sa.best;
    while(timer(1.98)){
        acc += sa.step(1e-8);
        iter++;
    }
    sa.best.output();
    cerr << acc << "/" << iter << " accepted." << endl;
    sa.best.debug_info();
}