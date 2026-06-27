#include <bits/stdc++.h>
#define rep(i,n) for(int i=0; i<n; i++)
#define reps(i,s,n) for(int i=s; i<n; i++)
#define per(i,n) for(int i=n-1; i>=0; i--)
#define pers(i,n,s) for(int i=n-1; i>=s; i--)
#define all(v) v.begin(),v.end()
#define fi first
#define se second
#define pb push_back
#define si(v) int(v.size())
#define lb(v,n) lower_bound(all(v),n)
#define lbi(v,n) int(lower_bound(all(v),n) - v.begin())
#define ub(v,n) upper_bound(all(v),n)
#define ubi(v,n) int(upper_bound(all(v),n) - v.begin())
 
#define mod 1000000007
#define infi 1010000000
#define infl 1100000000000000000
 
#define outve(v) {for(auto i : v) cout << i << " ";cout << endl;}
#define outmat(v) for(auto i : v){for(auto j : i) cout << j << " ";cout << endl;}
#define errve(v) {for(auto i : v) cerr << i << " ";cerr << endl;}
#define errmat(v) for(auto i : v){for(auto j : i) cerr << j << " ";cerr << endl;}
#define in(n,v) for(int i=0; i<(n); i++){cin >> v[i];}
#define IN(n,m,v) rep(i,n) rep(j,m){cin >> v[i][j];}
#define cyes cout << "Yes" << endl
#define cno cout << "No" << endl
#define cYES cout << "YES" << endl
#define cNO cout << "NO" << endl
#define csp << " " <<
#define outset(n) cout << fixed << setprecision(n);
 
using namespace std;
using ll = long long;
using ull = unsigned long long;
using uint = unsigned int;
using ld = long double;
using vi = vector<int>;
using vvi = vector<vector<int>>;
using vd = vector<double>;
using vvd = vector<vector<double>>;
using vl = vector<ll>;
using vvl = vector<vector<ll>>;
using vs = vector<string>;
using pii = pair<int,int>;
using pll = pair<ll,ll>;
template<typename T> using ve = vector<T>;
template<typename T> using vv = vector<vector<T>>;
template<typename T> using pq2 = priority_queue<T>;
template<typename T> using pq1 = priority_queue<T,vector<T>,greater<T>>;
template<typename T> bool chmax(T &a, T b) {if(a < b) {a = b;return 1;}return 0;}
template<typename T> bool chmin(T &a, T b) {if(a > b) {a = b;return 1;}return 0;}

int popcnt(uint n) {return __builtin_popcount(n);}
int popcntl(ull n) {return __builtin_popcountll(n);}
int bsr(uint n) {return 31 - __builtin_clz(n);}
int bsrl(ull n) {return 63 - __builtin_clzll(n);}
int bsf(uint n) {return __builtin_ctz(n);}
int bsfl(ull n) {return __builtin_ctzll(n);}

unsigned int xorshift() {
    static unsigned int tx = 123456789, ty=362436069, tz=521288629, tw=88675123;
    unsigned int tt = (tx^(tx<<11));
    tx = ty; ty = tz; tz = tw;
    return ( tw=(tw^(tw>>19))^(tt^(tt>>8)) );
}

class TimerChrono {
    std::chrono::system_clock::time_point ti;
    double time;
public:
    TimerChrono(){}
    void start(){ti = std::chrono::system_clock::now();}
    inline double get_time(){
        return time = static_cast<double>(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - ti).count() / 1000.0);
    }
};
TimerChrono timer;
double TIME_LIMIT = 1980;

string ULDR = "ULDR";
const int dh[4] = {-1,0,1,0};
const int dw[4] = {0,-1,0,1};
constexpr int N = 20;
string bord_init;
int N_oni = 0;

bool inline in_grid(int h, int w){return h >= 0 && h < N && w >= 0 && w < N;}

int decide_move_size(string &bord, int idx ,int dir){
    int oni = -1;
    int h = 0, w = 0;
    if(dir == 0) h = 0, w = idx;
    else if(dir == 1) h = idx, w = 0;
    else if(dir == 2) h = N-1, w = idx;
    else if(dir == 3) h = idx, w = N-1;
    rep(j,N){
        if(bord[h*N+w] == 'o') break;
        else if(bord[h*N+w] == 'x') oni = j;
        h += dh[(dir+2)%4], w += dw[(dir+2)%4];
    }
    return oni+1;
}
pii advance_move(string &bord, int idx ,int dir, int L){
    int h = 0, w = 0, h1 = 0, w1 = 0;
    if(dir == 0) h = 0, w = idx, h1 = L, w1 = idx;
    else if(dir == 1) h = idx, w = 0, h1 = idx, w1 = L;
    else if(dir == 2) h = N-1, w = idx, h1 = N-1-L, w1 = idx;
    else if(dir == 3) h = idx, w = N-1, h1 = idx, w1 = N-1-L;
    int huku = 0, oni = 0;
    rep(j,max(N-L,L)){
        if(in_grid(h,w)){
            if(bord[h*N+w] == 'x') bord[h*N+w] = '.', oni++;
            else if(bord[h*N+w] == 'o') huku++;
            if(in_grid(h1,w1)) {
                bord[h*N+w] = bord[h1*N+w1];
                bord[h1*N+w1] = '.';
            }
        }else{
            if(in_grid(h1,w1)) {
                bord[h1*N+w1] = '.';
            }
        }
        h += dh[(dir+2)%4], w += dw[(dir+2)%4];
        h1 += dh[(dir+2)%4], w1 += dw[(dir+2)%4];
    }

    return make_pair(oni,huku);
}

int cal_score(ve<tuple<int,int,int>> &actions){
    string bord = bord_init;
    int num_ope = 0;
    int cn_oni = N_oni, cn_huku = 0;
    for(auto [i,k,l] : actions){
        auto [cn_oni_tmp, cn_huku_tmp] = advance_move(bord,i,k,l);
        cn_oni -= cn_oni_tmp, cn_huku += cn_huku_tmp;
        num_ope += l;
    }

    if(cn_oni == 0 && cn_huku == 0) return 8*N*N-num_ope;
    else return 4*N*N-N*(cn_oni+cn_huku);
}

int cal_score_from_actions_ord(ve<pair<int,pii>> &action_ord){
    string bord_tmp_score = bord_init;
    int num_ope = 0;
    int cn_oni = N_oni, cn_huku = 0;
    for(auto [k,il] : action_ord) {
        auto [i,L] = il;
        if(L == infi) L = decide_move_size(bord_tmp_score, i, k);
        if(L == 0) continue;
        else if(L < 0) k = (k+2)%4, L *= -1;
        auto [cn_oni_tmp, cn_huku_tmp] = advance_move(bord_tmp_score,i,k,L);
        cn_oni -= cn_oni_tmp, cn_huku += cn_huku_tmp;
        num_ope += L;
        if(cn_huku > 0) return -infi;
    }
    if(cn_oni == 0 && cn_huku == 0) return 8*N*N-num_ope;
    else return 4*N*N-N*(cn_oni+cn_huku);
}

ve<tuple<int,int,int>> solve_greedy(){

    string bord = bord_init;

    ve<pair<int,pii>> action_ord;
    rep(k,4) rep(i,N) action_ord.pb(make_pair(k,make_pair(i,infi)));
    rep(k,2) rep(i,N) action_ord.pb(make_pair(k,make_pair(i,0)));
    // rep(k,4) rep(i,N) action_ord.pb(make_pair(k,make_pair(i,0)));

    rep(i,N){
        int n = xorshift()%(N-i);
        // int n = i+xorshift()%(N-i);
        swap(action_ord[i],action_ord[n]);
    }

    int score = cal_score_from_actions_ord(action_ord);
    int score_best = score;
    ve<pair<int,pii>> action_ord_best = action_ord;
    pair<int,pii> action_ord_i_tmp;

    double T0 = 0.0, T1 = 1.0;
    double T = T1;
    double time = timer.get_time();
    int loop = 0;
    while(time < TIME_LIMIT) {
        loop++;

        int p = xorshift()%100;
        int n = xorshift()%si(action_ord);
        int m = xorshift()%(si(action_ord)-1);
        int l0 = 0;
        if(n == m) m++;
        
        if(p <= 50){
            swap(action_ord[n],action_ord[m]);
        }else{
            while(action_ord[n].se.se == infi) n = xorshift()%si(action_ord);
            l0 = action_ord[n].se.se;
            action_ord[n].se.se += (xorshift()%2==0?-1:1) * (xorshift()%4+1);
            action_ord[n].se.se = clamp(action_ord[n].se.se, -(N-1), N-1);
            while(l0 == action_ord[n].se.se) {
                action_ord[n].se.se += (xorshift()%2==0?-1:1) * (xorshift()%4+1);
                action_ord[n].se.se = clamp(action_ord[n].se.se, -(N-1), N-1);
            }
        }
        
        int score1 = cal_score_from_actions_ord(action_ord);

        if(score <= score1){
            score = score1;
        }else{
            if((double)(xorshift()%UINT_MAX)/(double)(UINT_MAX) < exp((double)(score1-score)/T)){
                score = score1;
            }else{
                if(p <= 50){
                    swap(action_ord[n],action_ord[m]);
                }else{
                    action_ord[n].se.se = l0;
                }
            }
        }
        if(chmax(score_best,score)) action_ord_best = action_ord;
        time = timer.get_time();
        T = T1 + (T0-T1)*time/TIME_LIMIT;

        // if(loop%10000 == 0) cerr << loop csp score csp score1 csp score_best csp T csp timer.get_time() << endl;
    }
    cerr << loop << endl;


    // construct action
    ve<tuple<int,int,int>> actions;
    action_ord = action_ord_best;
    bord = bord_init;
    int cn_oni = N_oni, cn_huku = 0;
    for(auto [k,il] : action_ord){
        auto [i,L] = il;
        if(L == infi) L = decide_move_size(bord, i, k);
        if(L == 0) continue;
        else if(L < 0) k = (k+2)%4, L *= -1;
        actions.emplace_back(i,k,L);
        auto [cn_oni_tmp, cn_huku_tmp] = advance_move(bord,i,k,L);
        cn_oni -= cn_oni_tmp, cn_huku += cn_huku_tmp;
        if(cn_oni == 0) break;
    }

    return actions;

}


void solve(){
    
    int N_;
    cin >> N_;
    bord_init = "";
    rep(i,N) {
        string s;
        cin >> s;
        bord_init += s;
    }
    rep(i,N) rep(j,N) if(bord_init[i*N+j] == 'x') N_oni++;

    ve<tuple<int,int,int>> ans = solve_greedy();
    

    ll score = cal_score(ans);
    
    stringstream s_output;
    s_output << score csp timer.get_time() << "\n";
    #ifndef TESTER
    cerr << s_output.str() << endl;
    for(auto [i,k,l] : ans) {
        rep(j,l) cout << ULDR[k] csp i  << endl;
    }
    #else
    cout << s_output.str() << endl;
    #endif
}

int main(){
    
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    #ifdef LOCAL
    TIME_LIMIT = TIME_LIMIT*2/3;
    #endif
    timer.start();

    solve();

    return 0;
}
