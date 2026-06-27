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
using ll = long long;
using namespace std;
using namespace atcoder;
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

// mt19937 rnd(0);
// mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());

#if DEBUG
constexpr float machine_spec = 0.9;
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


constexpr int N = 20;
constexpr int KOMA = 4 * N;
constexpr int ONI_NUM = 2 * N;

char flip_dir(char dir){
    if(dir == 'U') return 'D';
    if(dir == 'D') return 'U';
    if(dir == 'L') return 'R';
    if(dir == 'R') return 'L';
    return ' ';
}

struct Act{
    uint z;
    char dir;
};

constexpr int OUT = 1e9;
struct Pos{
    int x, y; // xが下方向、yが右方向
    Pos(){}
    Pos(int x, int y): x(x), y(y){}

    void apply(Act act){
        if(x == OUT) return;
        if(y == act.z){
            if(act.dir == 'U') x--;
            if(act.dir == 'D') x++;
        }
        if(x == act.z){
            if(act.dir == 'L') y--;
            if(act.dir == 'R') y++;
        }
        if(x < 0 || x >= N || y < 0 || y >= N) x = OUT;
    }
    
    bool is_in(){
        return x != OUT;
    }

    int take_out_cost(){
        // out of rangeになるまでの最小コスト
        if(x == OUT) return 0;
        return min({x+1, N-x, y+1, N-y});
    }

    bool operator==(const Pos& other) const{
        return x == other.x && y == other.y;
    }
};


V<Pos> init_koma_poss;

void input(){
    int _; cin>> _;
    V<string> board(N); cin >> board;
    // 鬼(=x)の位置を取得
    rep(i, 0, N)rep(j, 0, N){
        if(board[i][j] == 'x'){
            init_koma_poss.push_back(Pos(i, j));
        }
    }
    // 福(=o)の位置を取得
    rep(i, 0, N)rep(j, 0, N){
        if(board[i][j] == 'o'){
            init_koma_poss.push_back(Pos(i, j));
        }
    }
}

constexpr int TurnBatchSize = 30;

struct State{
    Pos koma_poss[KOMA][TurnBatchSize+1];
    V<Act> acts;
    int score; // 低いほど良い
    int penalty;

    void resimulate_act(int start_turn, V<int>& change_komas){
        for(auto koma_idx: change_komas){
            if(koma_idx < ONI_NUM){
                score -= koma_poss[koma_idx][TurnBatchSize].take_out_cost();
            }else{
                bool is_in = koma_poss[koma_idx][TurnBatchSize].is_in();
                penalty -= !is_in;
            }

            auto cur_pos = koma_poss[koma_idx][start_turn];
            rep(j, start_turn, TurnBatchSize){
                cur_pos.apply(acts[j]);
                koma_poss[koma_idx][j+1] = cur_pos;
            }

            if(koma_idx < ONI_NUM){
                score += cur_pos.take_out_cost();
            }else{
                bool is_in = cur_pos.is_in();
                penalty += !is_in;
            }
        }
        // cerr << "resimulate: score: " << score << ", penalty: " << penalty << endl;
    }

    void log(int turn){
        cerr << "turn: " << turn << endl;
        V<string> board(N, string(N, '.'));
        rep(i, 0, KOMA){
            auto pos = koma_poss[i][turn];
            if(pos.is_in()){
                board[pos.x][pos.y] = "ox"[i < ONI_NUM];
            }
        }
        rep(i, 0, N){
            cerr << board[i] << endl;
        }
    }
};

struct Annealing{
    struct Temp{
        float start_temp = 4.17;
        float end_temp = 0.06;

        float current_accept_prob = 0;
        float get_temp(float progress){
            // st^(1-p) * et^p
            return pow(start_temp, 1.0 - progress) * pow(end_temp, progress);
            return start_temp + (end_temp - start_temp) * progress;
        }
    } temp;

    State best_state;
    State state;

    float calc_score(State& state, float penalty_weight){
        return state.score + penalty_weight * state.penalty;
    }

    State build_state(V<Pos> koma_poss, V<Act> acts){
        State state;
        state.acts = acts;
        rep(i, 0, KOMA){
            state.koma_poss[i][0] = koma_poss[i];
            // next_actsを適用する
            auto cur_pos = koma_poss[i];
            rep(j, 0, TurnBatchSize){
                cur_pos.apply(acts[j]);
                state.koma_poss[i][j+1] = cur_pos;
            }
            bool is_in = cur_pos.is_in();
            if(i < ONI_NUM && is_in){
                state.score += cur_pos.take_out_cost();
            }else if(i >= ONI_NUM && !is_in){
                state.penalty += 1;
            }
        }
        return state;
    }

    void create_initial_state(){
        V<Act> acts;
        rep(i, 0, TurnBatchSize)
            acts.push_back(Act{rnd() % N, "UDLR"[rnd() % 4]});
        state = build_state(init_koma_poss, acts);
        best_state = state;
    }

    void partial_run(){
        int loop_cnt = 0;
        int accept_cnt = 0;
        float current_temp = temp.start_temp;
        
        // while(loop_cnt < 100){
        int time_limit = 210;
        timer.mark_lap();
        while(timer.lap_ms() < time_limit){
            loop_cnt++;
            if(loop_cnt % 100 == 0){
                current_temp = temp.get_temp((float)timer.lap_ms() / time_limit);
            }

            int change_idx = rnd() % TurnBatchSize;
            char change_dir = "UDLR"[rnd() % 4];
            uint change_z = rnd() % N;
            Act old_act = state.acts[change_idx];
            Act new_act = {change_z, change_dir};
            if(old_act.z == new_act.z && old_act.dir == new_act.dir) continue;

            int old_score = calc_score(state, 10);

            V<int> change_komas;
            rep(i,0,KOMA){
                // change_komas.push_back(i);
                // continue;

                // new_actとold_actに直接影響するコマのみ
                auto pos = state.koma_poss[i][change_idx];
                bool old_act_change = pos != state.koma_poss[i][change_idx+1];
                auto new_pos = pos;
                new_pos.apply(new_act);
                bool new_act_change = pos != new_pos;
                if(old_act_change || new_act_change){
                    change_komas.push_back(i);
                }
            }
            state.acts[change_idx] = new_act;
            state.resimulate_act(change_idx, change_komas);

            float new_score = calc_score(state, 10);
            float score_diff = new_score - old_score;
            if(score_diff <= 0 || rnd() / (float)(rnd.max()) < exp(-score_diff / current_temp)){
                accept_cnt++;
                float best_score = calc_score(best_state, 10);
                if(chmin(best_score, new_score)){
                    best_state = state;
                    cerr << "loop: " << loop_cnt << ", real_score: " << best_score << endl;
                }
            }else{
                // rollback
                state.acts[change_idx] = old_act;
                state.resimulate_act(change_idx, change_komas);
            }
        }
        cerr << "[Finish] loop: " << loop_cnt << endl;
        cerr << "accept_rate: " << (float)accept_cnt / loop_cnt << endl;
    }

    V<Act> run(){
        V<Act> ans_acts;
        create_initial_state();
        
        int loop_cnt = 0;
        while(best_state.score > 0){
            loop_cnt++;
            // 直近TurnBatchSizeターンの行動を焼きなます
            partial_run();
            
            // TODO: 直す
            bool has_clear = true;
            rep(i, 0, KOMA/2){
                if(best_state.koma_poss[i][TurnBatchSize].is_in()){
                    has_clear = false;
                    break;
                }
            }
            
            if(!has_clear){
                // TurnBatchSize/2ターン分の行動をstateに適用する
                rep(i, 0, TurnBatchSize/2){
                    ans_acts.push_back(best_state.acts[i]);
                }
            }
            if(has_clear){
                // 全て適用したいが、不要な行動が含まれるため、1つずつ抜いて、問題ないか確認する
                V<Act> best_acts = best_state.acts;
                int pop_cnt = 0;
                drep(i,TurnBatchSize-1,0){
                    V<Act> tmp_acts;
                    rep(j,0,i) tmp_acts.push_back(best_acts[j]);
                    rep(j,i+1,TurnBatchSize) tmp_acts.push_back(best_acts[j]);
                    tmp_acts.push_back(Act{10000, flip_dir(tmp_acts.back().dir)});
                    auto tmp_state = best_state;
                    tmp_state.acts = tmp_acts;
                    V<int> change_komas;
                    rep(i,0,KOMA) change_komas.push_back(i);
                    tmp_state.resimulate_act(0, change_komas);
                    bool has_clear = true;
                    rep(i, 0, KOMA){
                        bool is_in = tmp_state.koma_poss[i][TurnBatchSize].is_in();
                        if(i < ONI_NUM && is_in) has_clear = false;
                        if(i >= ONI_NUM && !is_in) has_clear = false;
                        if(!has_clear) break;
                    }
                    if(has_clear){
                        best_acts = tmp_acts;
                        pop_cnt++;
                    }
                }
                cerr << "pop_cnt: " << pop_cnt << endl;
                rep(i,0,TurnBatchSize-pop_cnt) ans_acts.push_back(best_acts[i]);
                break;
            }


            // stateをTurnBatchSize/2ターン分進めた状態をturn0として、次のStateを作る
            V<Act> next_acts;
            rep(i,0,TurnBatchSize/2)
                next_acts.push_back(best_state.acts[i+TurnBatchSize/2]);
            rep(i,0,TurnBatchSize/2)
                next_acts.push_back(Act{rnd() % N, "UDLR"[rnd() % 4]});
            V<Pos> next_init_koma_poss;
            rep(i,0,KOMA)
                next_init_koma_poss.push_back(best_state.koma_poss[i][TurnBatchSize/2]);
            state = build_state(next_init_koma_poss, next_acts);
            best_state = state;
            
            if(loop_cnt > 10) break;
        }
        return ans_acts;
    }
};

struct Solver{
    void run(){
        Annealing annealing;
        auto ans = annealing.run();
        for(auto act: ans){
            cout << act.dir << " " << act.z << endl;
        }

        annealing.best_state.log(TurnBatchSize);

        cerr << "Turn = " << ans.size() << endl;
        cerr << "Score = " << 8 * N * N - ans.size() << endl;
    }
};


int main(int argc, char *argv[])
{
#if DEBUG
    freopen("tools/in/0000.txt", "r", stdin);
    input();
#else
    input();
#endif

    Solver solver;
    solver.run();
    cerr << "elapsed time: " << timer.elapsed_ms() << "ms" << endl;
}