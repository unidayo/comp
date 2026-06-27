#include <bits/stdc++.h>
#include <atcoder/all>
#include <cstdlib>
#include <cmath>
#include <random>
#include <fstream>

#define rep(i, begin, end) for (int i = begin; i < end; i++)
#define drep(i, begin, end) for (int i = begin; i >= end; i--)
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
using namespace std;
using namespace atcoder;
#ifdef LOCAL
void debug_out() {
    cerr << endl;
}
template <class Head, class... Tail> void debug_out(Head H, Tail... T) {
    cerr << ' ' << H;
    debug_out(T...);
}
#define debug(...) cerr << 'L' << __LINE__ << " [" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#define dump(x) cerr << 'L' << __LINE__ << " " << #x << " = " << (x) << endl
#else
#define debug(...) (void(0))
#define dump(x) (void(0))
#endif
using ll = long long;
using pi = pair<int, int>;
template<class T> using V = vector<T>;
template<class T> using VV = vector<vector<T>>;
template<class T> using VVV = vector<vector<vector<T>>>;
template<class T> using priority_queue_rev = priority_queue<T, vector<T>, greater<T>>;
template<class T> inline bool chmin(T &a, const T b) { if (a > b) { a = b; return true; } return false; }
template<class T> inline bool chmax(T &a, const T b) { if (a < b) { a = b; return true; } return false; }
template<class T> istream &operator>>(istream &is, vector<T> &v) { for (auto &e : v) is >> e; return is; }
template<class T> ostream &operator<<(ostream &os, const vector<T> &v) { for (auto &e : v) os << e << ' '; return os; }
template<class T> inline void to_unique(vector<T> &a) { sort(all(a)); a.erase(unique(all(a)), a.end()); }
struct fast_ios { fast_ios(){ cin.tie(nullptr); ios::sync_with_stdio(false); }; } fast_ios_;

#if DEBUG
constexpr float machine_spec = 0.5;
#elif LOCAL
constexpr float machine_spec = 1.4;
#else
constexpr float machine_spec = 1;
#endif
constexpr ll time_limit = (2000-50);

struct Timer{
    private:
        chrono::system_clock::time_point start = chrono::system_clock::now();
        chrono::system_clock::time_point lap = chrono::system_clock::now();
    public:
        // 実行開始時からの経過時間(ms)を返す
        int elapsed_ms(){
            auto now = chrono::system_clock::now();
            #if DEBUG or LOCAL
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() * machine_spec;
            #else
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            #endif
        }
        // ラップタイム(ms)を返す
        int lap_ms(){
            auto now = chrono::system_clock::now();
            #if DEBUG or LOCAL
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - lap).count() * machine_spec;
            #else
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - lap).count();
            #endif
        }
        // ラップを刻む
        void mark_lap(){
            lap = chrono::system_clock::now();
        }
} timer;

class xrand {
    uint64_t x;

   public:
    using result_type = uint32_t;
    static constexpr result_type min() {
        return std::numeric_limits<result_type>::min();
    }
    static constexpr result_type max() {
        return std::numeric_limits<result_type>::max();
    }
    xrand(uint64_t k) : x(k) {}
    xrand() : xrand(1) {}
    result_type operator()() {
        x ^= x << 9;
        return x ^= x >> 7;
        // return (x * 0x123456789abcdef) >> 32;
    }
    int64_t operator()(int m) { return int64_t((*this)()) * m >> 32; }
    double rand() { return double((*this)()) / (int64_t(1) << 32); }
}rnd;

constexpr int N = 100;
constexpr int L = 500000;
constexpr int LOOP_CNT = 4096 << 4;
int T[N];
int ST[N+1]; // 累積和
int RND_TABLE[L];
int ORDER[N]; // [i] := i番目の頂点のT[i]が何番目に小さいか
int ORDER_INV[N]; // [i] := i番目に小さいT[i]は頂点いくつか

void input(){
    int _; cin>>_>>_;
    rep(i,0,N) cin>>T[i];
    ST[0] = 0;
    rep(i,0,N) ST[i+1] = ST[i] + T[i];

    int idx = 0;
    rep(i,0,N){
        rep(j,0,T[i]){
            RND_TABLE[idx++] = i;
        }
    }

    int sortedT[N];
    rep(i,0,N) sortedT[i] = T[i];
    sort(sortedT, sortedT+N);
    
    rep(i,0,N) ORDER[i] = i;
    sort(ORDER, ORDER+N, [](int i, int j){ return T[i] < T[j]; });
    rep(i,0,N) ORDER_INV[ORDER[i]] = i;
    
    int ng_idx = 0;
    rep(i,0,3){
        if(sortedT[i] * 3 < sortedT[i+1]){
            ng_idx = i+1;
        }
    }
    // cerr << "ng_idx: " << ng_idx << endl;
    // rep(i,0,N) cerr << sortedT[i] << " ";
    // cerr << endl;
    // exit(1);

    rep(i,0,ng_idx) T[ORDER[i]] = 0;
}


static float tmp_val[N];
struct State{
    float error;
    int edges[N][2];
    float errors[N];
    State(){}

    float init_error(){
        rep(i,0,N) errors[i] = -T[i] * 2;
        rep(i,0,N){
            rep(j,0,2){
                int idx = edges[i][j];
                errors[idx] += T[i];
            }
        }
        error = 0;
        rep(i,0,N) error += abs(errors[i]);
        return error;
    }

    void output(){
        rep(i,0,N) cout << edges[i][0] << " " << edges[i][1] << endl;
    }
};

struct SA{
    struct Transition{
        int types[2000];
        int type_size = 0;
        
        long long try_cnt[100];
        long long accept_cnt[100];
        const int TransitionTypeSize = 3;

        Transition(){
            rep(i,0,1) types[type_size++] = 0;
            rep(i,0,1) types[type_size++] = 1;
            rep(i,0,1) types[type_size++] = 2;
        }

        void set_last_trainsition(){
            type_size = 0;
            rep(i,0,10) types[type_size++] = 0;
            rep(i,0,1) types[type_size++] = 1;
            rep(i,0,50) types[type_size++] = 2;
        }

        void set_transition_fix(int type){
            type_size = 0;
            types[type_size++] = type;
        }

        void log_accept_rate(){
            cerr << "accept rate " << endl;
            rep(i,0,TransitionTypeSize){
                cerr << i << ": " << accept_cnt[i] << " / " << try_cnt[i] << " (" << accept_cnt[i] * 100 / max(1ll,try_cnt[i]) << "%)" << endl;
            }
            cerr << endl;
        }

        void reset_accept_log(){
            rep(i,0,TransitionTypeSize){
                try_cnt[i] = 0;
                accept_cnt[i] = 0;
            }
        }

        bool execute(State& state, float current_temp){
            int type = types[rnd() % type_size];
            bool res;
            switch(type){
                case 0: res = change_edge(state, current_temp); break;
                case 1: res = swap_transition(state, current_temp); break;
                case 2: res = near_swap_transition(state, current_temp); break;
            }
            try_cnt[type]++;
            if(res) accept_cnt[type]++;
            return res;
        }

        bool should_accept_transition(float score_diff, float current_temp){
            if(score_diff <= 0) return true;
            if(score_diff > current_temp * 10) return false;
            return rnd.rand() < exp(-score_diff / current_temp);
            // return score_diff <= 0 || rnd.rand() < exp(-score_diff / current_temp);
        }

        bool change_edge(State& state, float current_temp){
            // 1. ランダムに選んだ辺を変更
            int idx = rnd() % N;
            int j = rnd() & 1;
            // 累積和の分布に従ってランダムに選ぶ
            int prev_idx = state.edges[idx][j];
            // int new_idx = RND_TABLE[rnd() % L];
            int new_idx = rnd() % N;
            if(new_idx == prev_idx) return false;
            if(new_idx == idx) return false;
            // if(new_idx == idx && state.edges[idx][j^1] == idx) return false;

            static float new_error[2];
            new_error[0] = state.errors[prev_idx];
            new_error[1] = state.errors[new_idx];
            new_error[0] -= T[idx];
            new_error[1] += T[idx];

            float error_diff = abs(new_error[0]) + abs(new_error[1]) - abs(state.errors[prev_idx]) - abs(state.errors[new_idx]);
            // debug(error_diff);

            if(should_accept_transition(error_diff, current_temp)){
                // accept
                state.edges[idx][j] = new_idx;
                // TODO: update errors
                state.errors[prev_idx] = new_error[0];
                state.errors[new_idx] = new_error[1];
                // TODO: update error
                state.error += error_diff;
                return true;
            }else{
                // rollback
                return false;
            }
        }

        bool swap_transition(State& state, float current_temp){
            // ランダムな state.edges[i][j] と state.edges[k][l] を選び、swap する
            int i = rnd() % N;
            int j = rnd() & 1;
            int k = rnd() % N;
            int l = rnd() & 1;
            if(i == k) return false;
            if(state.edges[i][j] == state.edges[k][l]) return false;
            if(state.edges[i][j] == k || state.edges[k][l] == i) return false;
            // if(state.edges[i][j^1] == i && state.edges[k][l] == i) return false;
            // if(state.edges[k][l^1] == k && state.edges[i][j] == k) return false;

            int to[2] = {state.edges[i][j], state.edges[k][l]};

            static float new_error[2];
            new_error[0] = state.errors[to[0]] - T[i] + T[k];
            new_error[1] = state.errors[to[1]] - T[k] + T[i];
            
            float error_diff = abs(new_error[0]) + abs(new_error[1]) - abs(state.errors[to[0]]) - abs(state.errors[to[1]]);
            // debug(error_diff);

            if(should_accept_transition(error_diff, current_temp)){
                // accept
                swap(state.edges[i][j], state.edges[k][l]);
                // TODO: update errors
                state.errors[to[0]] = new_error[0];
                state.errors[to[1]] = new_error[1];
                // TODO: update error
                state.error += error_diff;
                return true;
            }else{
                // rollback
                return false;
            }
        }

        bool near_swap_transition(State& state, float current_temp){
            // swap元の頂点のT[i]が近いものを選ぶ
            int i = rnd() % N;
            int j = rnd() & 1;
            // kはORDER, ORDER_INVを使って、 i に近いものを選ぶ
            int order = ORDER_INV[i];
            int diff = (rnd() % 5) + 1;
            diff *= (rnd() & 1) == 0 ? 1 : -1;
            int k_order = min(N-1, max(0, order + diff));
            int k = ORDER[k_order];
            int l = rnd() & 1;
            if(i == k) return false;
            if(state.edges[i][j] == state.edges[k][l]) return false;
            if(state.edges[i][j] == k || state.edges[k][l] == i) return false;
            // if(state.edges[i][j^1] == i && state.edges[k][l] == i) return false;
            // if(state.edges[k][l^1] == k && state.edges[i][j] == k) return false;
            
            int to[2] = {state.edges[i][j], state.edges[k][l]};

            static float new_error[2];
            new_error[0] = state.errors[to[0]] - T[i] + T[k];
            new_error[1] = state.errors[to[1]] - T[k] + T[i];
            
            float error_diff = abs(new_error[0]) + abs(new_error[1]) - abs(state.errors[to[0]]) - abs(state.errors[to[1]]);
            // debug(error_diff);

            if(should_accept_transition(error_diff, current_temp)){
                // accept
                swap(state.edges[i][j], state.edges[k][l]);
                // TODO: update errors
                state.errors[to[0]] = new_error[0];
                state.errors[to[1]] = new_error[1];
                // TODO: update error
                state.error += error_diff;
                return true;
            }else{
                // rollback
                return false;
            }
        }

    } transition;

    struct Temp{
        float start_temp = 20000;
        float mid_temp = 200;
        float end_temp = 2;
        
        float get_temp(float progress){
            // float threshold = 0.3;
            // if(progress < threshold)
            //     return pow(start_temp, 1.0 - progress / threshold) * pow(mid_temp, progress / threshold);
            // else
            //     return mid_temp + (end_temp - mid_temp) * (progress - threshold) / (1.0 - threshold);
            return pow(start_temp, 1.0 - progress) * pow(end_temp, progress);
            return start_temp + (end_temp - start_temp) * progress;
        }
    } temp;


    
    State run(int time_limit, State state){
        timer.mark_lap();

        State best_state = state;
        float current_temp = temp.start_temp;
        int loop_cnt = 0;
        int accept_cnt = 0;
        
        float log_progress = 0.09;
        while(timer.lap_ms() < time_limit){
            float progress = (float)timer.lap_ms() / time_limit;
            current_temp = temp.get_temp(progress);

            if(progress > log_progress){
                log_progress += 0.1;
                transition.log_accept_rate();
                transition.reset_accept_log();
            }
            
            if(progress > 0.5) transition.set_last_trainsition();

            rep(_,0,LOOP_CNT){
                loop_cnt++;
                bool accept = transition.execute(state, current_temp);
                accept_cnt += accept;

                if(accept && state.error < best_state.error){
                    best_state = state;
                    debug(loop_cnt, state.error);
                }
            }
        }
        cerr << "loop_cnt: " << loop_cnt << ", accept_rate: " << (float)accept_cnt / loop_cnt << endl;
        return best_state;
    }
};

struct InitStateSolver{
    State run(){
        State state;
        rep(i,0,N)rep(j,0,2) state.edges[i][j] = rnd() % N;
        state.init_error();

        State best_state = state;
        
        rep(i,0,1024){
            rep(i,0,N)rep(j,0,2){
                // 累積和の分布に従ってランダムに選ぶ
                int idx = RND_TABLE[rnd() % L];
                state.edges[i][j] = idx;
            }
            // errorsを算出
            state.init_error();

            if(state.error < best_state.error){
                debug(i,state.error);
                best_state = state;
            }
        }
        return best_state;
    }
};

struct Solver{
    void run(){
        State best_state = InitStateSolver().run();
        
        SA sa;
        best_state = sa.run(time_limit - timer.elapsed_ms(), best_state);
        best_state.output();
        // rep(i,0,N) rep(j,0,2) if(best_state.edges[i][j] == i) cerr << "self loop: " << i << endl;
        // rep(i,0,N) debug(best_state.errors[i]);
    }
};


int main(int argc, char *argv[])
{
#if DEBUG
    freopen("tools/in/0000.txt", "r", stdin);
#endif
    input();
    
    Solver solver;
    solver.run();
    cerr << "elapsed time: " << timer.elapsed_ms() << "ms" << endl;
}