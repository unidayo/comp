#include <bits/stdc++.h>
#include <atcoder/all>
#include <cstdlib>
#include <cmath>
#include <random>

#define rep(i, begin, end) for (int i = begin; i < end; i++)
#define repint(i, begin, end) for (int i = begin; i < end; i++)
#define drep(i, begin, end) for (int i = begin; i >= end; i--)
#define rep1toN(i, n) for (int i = 1; i <= (int)(n); i++)
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
using ll = long long;
// #define rep(i, begin, end) for (ll i = (ll)begin; i < (ll)(end); i++)
using si = __int16_t;
#define repsi(i, begin, end) for (si i = (si)begin; i < (si)end; i++)
// using ld = double;
using namespace std;
using namespace atcoder;
template<class T> using v = vector<T>;
template<class T> using vv = v<v<T>>;
template<class T> using vvv = vv<v<T>>;
template<class T> using vvvv = vv<vv<T>>;
template<class T> using priority_queue_rev = priority_queue<T, vector<T>, greater<T>>;
template<class T> inline bool chmin(T &a, const T b) { if (a > b) { a = b; return true; } return false; }
template<class T> inline bool chmax(T &a, const T b) { if (a < b) { a = b; return true; } return false; }
template<class T> istream &operator>>(istream &is, vector<T> &v) { for (auto &e : v) is >> e; return is; }
template<class T> ostream &operator<<(ostream &os, const vector<T> &v) { for (auto &e : v) os << e << ' '; return os; }
struct fast_ios { fast_ios(){ cin.tie(nullptr); ios::sync_with_stdio(false); }; } fast_ios_;

mt19937 rnd(0);
auto system_now = std::chrono::system_clock::now();
int check_time() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - system_now).count();
}

#if LOCAL
ll time_limit = 1500;
#else
ll time_limit = 2000-50;
#endif

const int N = 6,M = 15,T = 10, N2 = N*N;
const int FEED = 60;
int MaxElem[M];
const float ScorePow = 5.5;
const float DotAlpha = 0.1;

struct Seed{
    int val[M];
    float powval[M];
    float powscore;

    void calc_powscore(int turn){
        float powvalue = (1.0 * turn + ScorePow * (T - turn)) / T;
        // float powvalue = turn < T-2 ? ScorePow : 1.0;
        // float powvalue = ScorePow;
        rep(i,0,M){
            float base = val[i] / (float)MaxElem[i];
            float mxbase = pow((float)MaxElem[i] /100, 1);
            powval[i] = pow(base, powvalue) * mxbase;
        }
        powscore = accumulate(powval,powval+M,0.0);
    }

    int calc_rawscore(){
        return accumulate(val,val+M,0);
    }
};

struct SimpleSolver{
    int res[N][N];
    void run(Seed seeds[FEED]){
        v<pair<int,int>> ss;
        rep(i,0,FEED){
            int score = seeds[i].calc_rawscore();
            ss.push_back({score,i});
        }
        sort(rall(ss));
        // 最初のN*N個をランダムに並び替える
        v<pair<int,int>> ss2;
        rep(i,0,N*N) ss2.push_back(ss[i]);
        shuffle(all(ss2),rnd);
        rep(i,0,N) rep(j,0,N) res[i][j] = ss2[i*N+j].second;
    }
};

struct SimpleSolver2{
    int res[N][N];
    void run(Seed seeds[FEED]){
        v<pair<float,int>> ss;
        rep(i,0,FEED){
            ss.push_back({seeds[i].powscore,i});
        }
        sort(rall(ss));
        // 最初のN*N個をランダムに並び替える
        v<int> ss2;
        rep(i,0,N*N) ss2.push_back(ss[i].second);
        shuffle(all(ss2),rnd);
        rep(i,0,N) rep(j,0,N) res[i][j] = ss2[i*N+j];
    }
};

struct Annealing{
    float board_score_coef[N][N]; // B[i][j]に配置した時のpowscoreの反映率
    int board_arrange[N][N]; // B[i][j]に配置されたseedのindex
    int seed_pos[FEED]; // 盤面のindex or 配置しない場合は-1

    float dots[FEED][FEED];
    int best_board_arrange[N][N];

    void calc_dots(Seed seeds[FEED], int turn){
        // v<float> c(10, DotAlpha);
        // rep(i,0,10) c[i] *= 1 - (turn - 5) * 0.01;
        // float dotAlpha = c[turn];
        float dotAlpha = DotAlpha;

        rep(i,0,FEED) rep(j,0,FEED){
            float dot = 0;
            rep(k,0,M) dot += pow(seeds[i].powval[k] * seeds[j].powval[k], 0.8);
            dots[i][j] = dot * dotAlpha;
        }
    }

    void set_board_score_coef(){
        // 中心ほど高い
        v<float> c = {0.25, 0.4, 0.5, 0.5, 0.4, 0.25};
        // v<float> c(N,0.5);
        rep(i,0,N) rep(j,0,N){
            board_score_coef[i][j] = c[i] + c[j];
        }
    }

    void run(Seed seeds[FEED], ll time_limit, int turn){
        calc_dots(seeds, turn);
        set_board_score_coef();
        rep(i,0,FEED) seed_pos[i] = -1;
        rep(i,0,N) rep(j,0,N){
            board_arrange[i][j] = i*N+j;
            seed_pos[i*N+j] = i*N+j;
        }

        float board_score = 0;
        rep(i,0,N) rep(j,0,N){
            board_score += seeds[board_arrange[i][j]].powscore * board_score_coef[i][j];
            if(j < N-1) board_score -= dots[board_arrange[i][j]][board_arrange[i][j+1]];
            if(i < N-1) board_score -= dots[board_arrange[i][j]][board_arrange[i+1][j]];
        }

        float best_score = board_score;
        rep(i,0,N) rep(j,0,N) best_board_arrange[i][j] = board_arrange[i][j];

        ll start_time = check_time();
        ll total_time = time_limit - start_time;
        float temp = 15;
        float start_temp = 10;
        float end_temp = 0.0001;
        int loop_count = 0;
        while(check_time() < time_limit){
        // while(loop_count < 10){
            int s0 = rnd() % FEED;
            int s1 = rnd() % FEED;
            if(seed_pos[s0] == seed_pos[s1]) continue;
            if(seed_pos[s0] == -1 && seed_pos[s1] == -1) continue;
            
            // s0とs1を交換。片方が-1の場合は片方を配置から外して、もう片方を配置する
            float score_diff = 0;
            for(auto idx:{s0,s1}){
                if(seed_pos[idx] != -1){
                    int i = seed_pos[idx] / N;
                    int j = seed_pos[idx] % N;
                    score_diff -= seeds[idx].powscore * board_score_coef[i][j];
                    if(j < N-1) score_diff += dots[board_arrange[i][j]][board_arrange[i][j+1]];
                    if(j > 0) score_diff += dots[board_arrange[i][j]][board_arrange[i][j-1]];
                    if(i < N-1) score_diff += dots[board_arrange[i][j]][board_arrange[i+1][j]];
                    if(i > 0) score_diff += dots[board_arrange[i][j]][board_arrange[i-1][j]];
                }
            }
            swap(seed_pos[s0],seed_pos[s1]);
            for(auto idx:{s0,s1}){
                if(seed_pos[idx] != -1){
                    int i = seed_pos[idx] / N;
                    int j = seed_pos[idx] % N;
                    board_arrange[i][j] = idx;
                }
            }

            
            for(auto idx:{s0,s1}){
                if(seed_pos[idx] != -1){
                    int i = seed_pos[idx] / N;
                    int j = seed_pos[idx] % N;
                    score_diff += seeds[idx].powscore * board_score_coef[i][j];
                    if(j < N-1) score_diff -= dots[board_arrange[i][j]][board_arrange[i][j+1]];
                    if(j > 0) score_diff -= dots[board_arrange[i][j]][board_arrange[i][j-1]];
                    if(i < N-1) score_diff -= dots[board_arrange[i][j]][board_arrange[i+1][j]];
                    if(i > 0) score_diff -= dots[board_arrange[i][j]][board_arrange[i-1][j]];
                }
            }

            // float temp = start_temp + (end_temp - start_temp) * (check_time() - start_time) / total_time;
            if(score_diff > 0 || exp(score_diff / temp) > (float)rnd() / (float)rnd.max()){
                board_score += score_diff;
                if(chmax(best_score,board_score)){
                    rep(i,0,N) rep(j,0,N) best_board_arrange[i][j] = board_arrange[i][j];
                }
            }else{
                swap(seed_pos[s0],seed_pos[s1]);
                for(auto idx:{s0,s1}){
                    if(seed_pos[idx] != -1){
                        int i = seed_pos[idx] / N;
                        int j = seed_pos[idx] % N;
                        board_arrange[i][j] = idx;
                    }
                }
            }
            temp *= 0.9993;
            loop_count++;
        }
        cerr<< "loop_count=" << loop_count <<endl;
    }
};

void input_seeds(Seed seeds[FEED], int turn){
    rep(i,0,FEED){
        rep(j,0,M){
            cin>>seeds[i].val[j];
        }
        seeds[i].calc_powscore(turn);
    }
}

void output_arrange(int res[N][N]){    

    rep(i,0,N){
        rep(j,0,N){
            cout<<res[i][j]<<" ";
            cerr<<res[i][j]<<" ";
        }
        cout<<endl;
        cerr<<endl;
    }
}

int main(){
    int _; cin>>_>>_>>_;

    Seed seeds[FEED];
    input_seeds(seeds, 0);
    rep(i,0,M){
        MaxElem[i] = 0;
        rep(j,0,FEED) chmax(MaxElem[i],seeds[j].val[i]);
    }
    rep(i,0,FEED) seeds[i].calc_powscore(0);

    rep(t,0,T){
        Annealing annealing;
        annealing.run(seeds, time_limit * (t+1) / T, t);

        output_arrange(annealing.best_board_arrange);
        input_seeds(seeds, t+1);
    }

    int max_score = 0;
    for(auto seed:seeds){
        chmax(max_score,seed.calc_rawscore());
    }
    int frac = accumulate(MaxElem,MaxElem+M,0);
    cerr<< "Score=" << max_score * 1000000 / frac <<endl;
}