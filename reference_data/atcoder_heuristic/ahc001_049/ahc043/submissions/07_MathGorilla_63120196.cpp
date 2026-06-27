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

#if DEBUG
constexpr float machine_spec = 0.5;
#elif LOCAL
constexpr float machine_spec = 1.4;
#else
constexpr float machine_spec = 1;
#endif
constexpr ll time_limit = (3000-50);

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

/**
 * @brief Gumbel分布からのサンプリング
 */
struct GumbelSampler{
    const static int sample_size_exp = 15;
    float samples[1<<sample_size_exp];
    int mask = (1<<sample_size_exp) - 1;
    
    GumbelSampler(float scale){
        rep(i,0,1<<sample_size_exp)
            samples[i] = -log(-log(rnd.rand())) * scale;
    }
    
    float sample(){
        return samples[rnd() & mask];
    }

} gumbel_sampler(2.0);

constexpr int N = 50;
int M;
int InitK;
constexpr int T = 800;
constexpr int RailCost = 100;
constexpr int TerminalCost = 5000;
constexpr int TerminalRange = 2;

struct Pos{
    int x, y;
    Pos(int x, int y): x(x), y(y){}
    Pos(){}
    
    Pos get_flip(){
        return Pos(-x, -y);
    }
    Pos get_abs(){
        return Pos(abs(x), abs(y));
    }
    Pos operator+(const Pos& other) const{
        return Pos(x + other.x, y + other.y);
    }
    Pos operator-(const Pos& other) const{
        return Pos(x - other.x, y - other.y);
    }
    bool operator==(const Pos& other) const{
        return x == other.x && y == other.y;
    }
    bool operator<(const Pos& other) const{
        return x < other.x || (x == other.x && y < other.y);
    }
    // TODO: 中継の処理はこっち側でabs取る方がPosの生成がなくて良いかも
    int dist(const Pos& other) const{
        return abs(x - other.x) + abs(y - other.y);
    }
    bool valid(){
        return 0 <= x && x < N && 0 <= y && y < N;
    }
    // TODO: is_relayの方がvalidよりはほんの少し速い
};

// Pos用のハッシュ定義
namespace std {
    template <>
    struct hash<Pos> {
        std::size_t operator()(const Pos& p) const {
            std::size_t h1 = std::hash<int>()(p.x);
            std::size_t h2 = std::hash<int>()(p.y);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
    template <>
    struct hash<std::pair<Pos, Pos>> {
        size_t operator()(const std::pair<Pos, Pos>& pp) const {
            auto h1 = std::hash<Pos>()(pp.first);
            auto h2 = std::hash<Pos>()(pp.second);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
}

Pos STs[1600][2]; // (Start, Terminal)の座標
int STDists[1600]; // StartからTerminalまでの距離

void input(){
    int _; cin>>_>>M>>InitK>>_;
    rep(i,0,M)rep(j,0,2){
        int x, y; cin >> x >> y;
        STs[i][j] = Pos(x, y);
    }
    rep(i,0,M)
        STDists[i] = abs(STs[i][0].x - STs[i][1].x) + abs(STs[i][0].y - STs[i][1].y);
}

/**
 * @brief 盤面情報。各マスの良さや、マス同士の相性など
 */
struct BoardInfo{
    // 各マスの価値。対となるマスとの距離の総和
    int cell_worth[N][N];
    // 各マスに駅を置いた時に得られる最大スコア
    int cell_terminal_reward[N][N];
    // マスiとマスjに駅を置いて繋いだ時に得られるスコア
    int connect_reward[N][N][N][N];
    // マスiに駅を置いた時にカバーできるマスのリスト。駅が無いマスは無視
    Pos terminal_cover_poss[N][N][15];
    int terminal_cover_poss_size[N][N];
    // cell_rewardの大きい順
    V<pair<int, Pos>> good_cells;
    int good_cells_size;
    // 相性の良いマスのペアの上位
    // TODO: 距離の近さによるコスパも考慮した方が良さそう
    V<tuple<int, Pos, Pos>> good_pairs;
    // pos(i,j)と対になるマスのリスト
    Pos pair_poss[N][N][200];
    int pair_poss_score[N][N][200];
    int pair_poss_size[N][N];
    
    void init(){
        // Pos(i,j)に駅を置いた時にカバーできるマスのリストを作成
        bool exists_terminal[N][N];
        rep(i,0,N)rep(j,0,N) exists_terminal[i][j] = false;
        rep(i,0,M)rep(j,0,2) exists_terminal[STs[i][j].x][STs[i][j].y] = true;
        rep(i,0,N)rep(j,0,N){
            rep(di,-TerminalRange,TerminalRange+1)rep(dj,-TerminalRange+abs(di),TerminalRange-abs(di)+1){
                Pos pos = Pos(i, j) + Pos(di, dj);
                if(pos.valid() && exists_terminal[pos.x][pos.y])
                    terminal_cover_poss[i][j][terminal_cover_poss_size[i][j]++] = pos;
            }
        }

        // 各マスに駅を置いた時に得られる最大スコア
        rep(i,0,M)rep(j,0,2){
            cell_worth[STs[i][j].x][STs[i][j].y] += STDists[i];
            rep(di,-TerminalRange,TerminalRange+1)rep(dj,-TerminalRange+abs(di),TerminalRange-abs(di)+1){
                Pos pos = STs[i][j] + Pos(di, dj);
                if(pos.valid())
                    cell_terminal_reward[pos.x][pos.y] += STDists[i];
            }
        }

        // マスiとマスjに駅を置いて繋いだ時に得られるスコア
        rep(i,0,M){
            rep(di,-TerminalRange,TerminalRange+1)rep(dj,-TerminalRange+abs(di),TerminalRange-abs(di)+1){
                Pos pos = STs[i][0] + Pos(di, dj);
                if(!pos.valid()) continue;
                rep(di2,-TerminalRange,TerminalRange+1)rep(dj2,-TerminalRange+abs(di2),TerminalRange-abs(di2)+1){
                    Pos pos2 = STs[i][1] + Pos(di2, dj2);
                    if(!pos2.valid()) continue;
                    connect_reward[pos.x][pos.y][pos2.x][pos2.y] += STDists[i];
                    connect_reward[pos2.x][pos2.y][pos.x][pos.y] += STDists[i];
                }
            }
        }

        // pos(i,j)と対になるマスのリスト
        rep(i,0,M){
            Pos p1 = STs[i][0];
            Pos p2 = STs[i][1];
            pair_poss[p1.x][p1.y][pair_poss_size[p1.x][p1.y]] = p2;
            pair_poss[p2.x][p2.y][pair_poss_size[p2.x][p2.y]] = p1;
            pair_poss_score[p1.x][p1.y][pair_poss_size[p1.x][p1.y]] = STDists[i];
            pair_poss_score[p2.x][p2.y][pair_poss_size[p2.x][p2.y]] = STDists[i];
            pair_poss_size[p1.x][p1.y]++;
            pair_poss_size[p2.x][p2.y]++;
        }
        

        // 期待報酬が高い順にソート。ただし(0, 0)は後の中継処理で対応できないので除外
        good_cells.clear();
        rep(i,0,N)rep(j,0,N) if(cell_terminal_reward[i][j] > 0 && Pos(i, j) != Pos(0, 0))
            good_cells.push_back({cell_terminal_reward[i][j], Pos(i, j)});
        sort(rall(good_cells));
        good_cells_size = good_cells.size();

        // 相性の良いマスのペアを取得。ただし、(0, 0)は後の中継処理で対応できないので除外
        int first_enable_dist = (InitK - TerminalCost*2) / RailCost + 1;
        priority_queue<tuple<int, Pos, Pos>> score_queue;
        rep(i,0,N)rep(j,0,N)rep(k,0,N)rep(l,0,N) if(connect_reward[i][j][k][l] > 0 && Pos(i, j) < Pos(k, l) && Pos(i, j).dist(Pos(k, l)) <= first_enable_dist && Pos(i, j) != Pos(0, 0))
            score_queue.push({connect_reward[i][j][k][l], Pos(i, j), Pos(k, l)});
        cerr << "score_queue.size(): " << score_queue.size() << endl;
        unordered_set<pair<Pos, Pos>> used;
        int cnt = 0;
        // while(!score_queue.empty() && cnt < 100){
        while(!score_queue.empty() && cnt < 1000){
            auto [s, p1, p2] = score_queue.top();
            score_queue.pop();
            // if(used.count({p1, p2})) continue;
            cnt++;
            good_pairs.push_back({s, p1, p2});
            // score (x1, y1) (x2, y2)
            // cerr << s << " (" << p1.x << ", " << p1.y << ") (" << p2.x << ", " << p2.y << ")" << endl;
            // static int min_di = -4, max_di = 4;
            // rep(di,min_di,max_di+1)rep(dj,min_di+abs(di),max_di-abs(di)+1){
            //     Pos pos = p1 + Pos(di, dj);
            //     if(!pos.valid()) continue;
            //     rep(di2,min_di,max_di+1)rep(dj2,min_di+abs(di2),max_di-abs(di2)+1){
            //         Pos pos2 = p2 + Pos(di2, dj2);
            //         if(!pos2.valid()) continue;
            //         used.insert({pos, pos2});
            //         used.insert({pos2, pos});
            //     }
            // }
        }
    }
} board_info;

struct Operation{
    Pos from; // Posが負の値の場合は中継地点. (0,0)は負の表現ができない&(1,1)の下位互換なので禁止。
    Pos to; // fromと同様
    bool x_prior; // firstからtoに線路を伸ばす際、x→yの順に線路を伸ばすか
};

float total_covered_worth_coef = 0.4;

int coef_max_turn = T;
struct SimulationResult{
    int turn;
    int k;
    int income;
    int total_covered_worth;

    int final_k() const{
        return k + income * (T - turn);
    }

    int score() const{
        // まだ片方しかカバーしていない対に関しても、少しだけ収入が発生すると考える
        float coef = min<float>(1, 0.9 + (coef_max_turn - turn) * 0.001);
        // float coef = min<float>(1, (coef_max_turn - turn) * 0.05);

        return k + (income + (total_covered_worth - (income << 1)) * total_covered_worth_coef) * (min(T, coef_max_turn + 100) - turn);

        // float coef = max<float>(0, (1 - (coef_max_turn - turn) * 0.01) * 1);
        // float corrected_income = income - ((float)income * 10 / turn * coef);

        // return k + (corrected_income + (total_covered_worth - (income << 1)) * multi(turn)) * (T - turn);
    }
};

struct Simulator{
    int simulate_cnt = 0;

    // 線路の配置シミュレーションは行わないunsafeなシミュレーション
    SimulationResult simulate_unsafe(V<Operation>& operations){
        simulate_cnt++;
        int k = InitK;
        int turn = 0;
        int income = 0;
        int total_covered_worth = 0; // 駅でカバーしたマスの収入総和.まだペアとして収入が発生していない分も。
        static int covered_map[N][N];
        static int cur;
        cur++;

        int accumulate_dist = 0;
        for(auto& op: operations){
            int dist = op.from.get_abs().dist(op.to.get_abs());
            if(!op.to.valid()){
                accumulate_dist += dist;
                continue;
            }
            dist += accumulate_dist;
            accumulate_dist = 0;
            
            int need_k = max(0,(dist-1)) * RailCost + TerminalCost;
            
            if(income == 0 && need_k > k) return {T, k, 0, -1000000};
            int need_turn = max(max(1,dist), (max(0, need_k - k) + income - 1) / max(1, income) + 1);
            turn += need_turn;
            int new_income = income;
            // 新しいincomeの計算
            int cover_pos_size = board_info.terminal_cover_poss_size[op.to.x][op.to.y];
            rep(i,0,cover_pos_size){
                auto& pos = board_info.terminal_cover_poss[op.to.x][op.to.y][i];
                if(covered_map[pos.x][pos.y] != cur){
                    covered_map[pos.x][pos.y] = cur;
                    total_covered_worth += board_info.cell_worth[pos.x][pos.y];
                    int pair_poss_size = board_info.pair_poss_size[pos.x][pos.y];
                    rep(d,0,pair_poss_size){
                        auto& p = board_info.pair_poss[pos.x][pos.y][d];
                        if(covered_map[p.x][p.y] == cur)
                            new_income += board_info.pair_poss_score[pos.x][pos.y][d];
                    }
                }
            }
            
            k = k - need_k + (need_turn - 1) * income + new_income;
            income = new_income;
        }
        return {turn, k, income, total_covered_worth};
    }

    SimulationResult simulate_unsafe_one_add_terminal_operation(Operation& op, int turn, int income, int k, int new_income, int new_total_covered_worth){
        int dist = op.from.get_abs().dist(op.to.get_abs());
        int need_k = max(0,dist-1) * RailCost + TerminalCost;
        if(income == 0 && need_k > k) return {T, k, 0, -1000000};
        int need_turn = max(max(1,dist), (max(0, need_k - k) + income - 1) / max(1, income) + 1);
        turn += need_turn;
        k = k - need_k + (need_turn - 1) * income + new_income;
        return {turn, k, new_income, new_total_covered_worth};
    }

    bool contains_pos_in_operation(Operation& op, Pos& pos){
        Pos from = op.from.get_abs();
        Pos to = op.to.get_abs();
        if(from.x != pos.x && from.y != pos.y && to.x != pos.x && to.y != pos.y) return false;
        
        Pos bend = op.x_prior ? Pos(to.x, from.y) : Pos(from.x, to.y);
        // from,bend間にあるか
        if(from.dist(pos) + bend.dist(pos) == from.dist(bend)) return true;
        // bend, to間にあるか
        if(bend.dist(pos) + to.dist(pos) == bend.dist(to)) return true;
        
        return false;
    }

    bool operation_rail_check(V<Operation>& operations){
        static int used[N][N];
        static int cur = 0;
        cur++;
        used[operations[0].to.x][operations[0].to.y] = cur;
        rep(i,1,operations.size()){
            auto& op = operations[i];
            Pos cursor = op.from.get_abs();
            Pos to = op.to.get_abs();
            while(cursor != to){
                if(cursor.y == to.y || (cursor.x != to.x && op.x_prior)){
                    cursor.x += (cursor.x < to.x ? 1 : -1);
                }else{
                    cursor.y += (cursor.y < to.y ? 1 : -1);
                }
                if(used[cursor.x][cursor.y] == cur) return false;
                used[cursor.x][cursor.y] = cur;
            }
        }
        return true;
    }

    void output(V<Operation>& operations){
        // (type, 座標): type=0なら駅, type=1なら線路, type=2ならCursorMove
        V<pair<int, Pos>> decodes;
        Pos cursor = operations[0].from.get_abs();
        for(auto& op: operations){
            Pos from = op.from.get_abs();
            Pos to = op.to.get_abs();
            if(cursor != from){
                // CursorMove
                decodes.push_back({2, from});
                cursor = from;
            }
            if(cursor == to){
                decodes.push_back({0, cursor});
                continue;
            }
            // 線路を引く
            while(cursor != to){
                // x方向に動かす
                if(cursor.y == to.y || (cursor.x != to.x && op.x_prior)){
                    cursor.x += (cursor.x < to.x ? 1 : -1);
                }else{
                    cursor.y += (cursor.y < to.y ? 1 : -1);
                }
                int type = (cursor == to && op.to.valid()) ? 0 : 1;
                decodes.push_back({type, cursor});
            }
        }

        // 残金をsimulationしながら貪欲でdecodesを処理していく
        V<pair<int, Pos>> ans;
        int k = InitK;
        int turn = 0;
        int reward = 0;
        static int covered_map[N][N];
        static int cur;
        cur++;

        auto add_terminal = [&](Pos pos){
            ans.push_back({0, pos});
            rep(i,-TerminalRange,TerminalRange+1)rep(j,-TerminalRange+abs(i),TerminalRange-abs(i)+1){
                Pos p = pos + Pos(i, j);
                if(!p.valid()) continue;
                if(covered_map[p.x][p.y] != cur){
                    covered_map[p.x][p.y] = cur;
                    rep(d,0,board_info.pair_poss_size[p.x][p.y]){
                        auto p2 = board_info.pair_poss[p.x][p.y][d];
                        if(covered_map[p2.x][p2.y] == cur)
                            reward += board_info.pair_poss_score[p.x][p.y][d];
                    }
                }
            }
            turn++;
            k = k - TerminalCost + reward;
        };

        auto add_rail = [&](Pos pos, Pos prev, Pos next){
            int rail_type = 0;
            Pos in = pos - prev;
            Pos out = next - pos;
            if(in.y * out.y == 1) rail_type = 1;
            else if(in.x * out.x == 1) rail_type = 2;
            else if(in.y + out.x == 2 || in.x + out.y == -2) rail_type = 3;
            else if(in.y + out.x == -2 || in.x + out.y == 2) rail_type = 5;
            else if((in.y == 1 && out.x == -1) || (in.x == 1 && out.y == -1)) rail_type = 4;
            else rail_type = 6;
            ans.push_back({rail_type, pos});
            turn++;
            k = k - RailCost + reward;
        };

        auto wait = [&](){
            ans.push_back({-1, Pos()});
            turn++;
            k += reward;
        };

        rep(i,0,decodes.size()){
            auto [type, pos] = decodes[i];
            if(type == 0){
                // 駅を置く
                while(k < TerminalCost) wait();
                add_terminal(pos);
            }
            if(type == 1){
                // 線路を引く
                while(k < RailCost) wait();
                add_rail(pos, decodes[i-1].second, decodes[i+1].second);
            }
        }
        while(turn < T) wait();

        for(auto [type, pos]: ans){
            if(type >= 0) cout << type << " " << pos.x << " " << pos.y << endl;
            else cout << type << endl;
        }
    }

    void debug_rail_check(V<Operation>& operations, bool state_used[N][N]){
        static int used[N][N];
        static int cur = 0;
        cur++;
        used[operations[0].to.x][operations[0].to.y] = cur;
        rep(i,1,operations.size()){
            auto& op = operations[i];
            Pos cursor = op.from.get_abs();
            Pos to = op.to.get_abs();
            while(cursor != to){
                if(cursor.y == to.y || (cursor.x != to.x && op.x_prior)){
                    cursor.x += (cursor.x < to.x ? 1 : -1);
                }else{
                    cursor.y += (cursor.y < to.y ? 1 : -1);
                }
                assert(used[cursor.x][cursor.y] != cur);
                used[cursor.x][cursor.y] = cur;
            }
        }
        // state_usedと比較
        rep(i,0,N)rep(j,0,N) assert((used[i][j] == cur) == state_used[i][j]);
    }

    void debug_terminal_topological_sort(V<Operation>& operations){
        map<Pos, int> terminals;
        for(auto& op: operations){
            if(op.from.valid()) assert(terminals[op.from] > 0);
            else assert(terminals[op.from.get_flip()] == 0);
            if(op.to.valid()) terminals[op.to]++;
            else assert(terminals[op.to.get_flip()] == 0);
        }
    }

    void debug_print_used(bool used[N][N], bool terminal_placed[N][N]){
        rep(i,0,N){
            rep(j,0,N){
                if(terminal_placed[i][j]) cerr << "@ ";
                else cerr << (used[i][j] ? "/ " : ". ");
            }
            cerr << endl;
        }
        cerr << endl;
    }

} simulator;

struct State{
    V<Operation> operations;
    V<Pos> terminals;
    bool terminal_placed[N][N]; // 駅が置かれているか
    bool used[N][N]; // 線路か駅で使われているか
    int terminal_covered_cnt[N][N];
    bool fixed[N][N]; // 駅が固定されているか
    SimulationResult result;
    
    State(){
        memset(terminal_placed, 0, sizeof(terminal_placed));
        memset(used, 0, sizeof(used));
        memset(terminal_covered_cnt, 0, sizeof(terminal_covered_cnt));
        memset(fixed, 0, sizeof(fixed));
    }

    void flip_rail_used(Operation& op){
        Pos cursor = op.from.get_abs();
        Pos to = op.to.get_abs();
        // TODO: これ系全部、if(op.x_prior)で最初に分岐した方が少しだけ速い。ただ、やる価値はなさそう
        while(cursor != to){
            if(cursor.y == to.y || (cursor.x != to.x && op.x_prior)){
                cursor.x += (cursor.x < to.x ? 1 : -1);
            }else{
                cursor.y += (cursor.y < to.y ? 1 : -1);
            }
            if(cursor != to) used[cursor.x][cursor.y] = !used[cursor.x][cursor.y];
        }
    }

    void update_terminal_cover(Pos pos, bool put){
        terminal_placed[pos.x][pos.y] = put;
        int diff = put ? 1 : -1;
        rep(i,-TerminalRange,TerminalRange+1)rep(j,-TerminalRange+abs(i),TerminalRange-abs(i)+1){
            Pos p = pos + Pos(i, j);
            if(p.valid())
                terminal_covered_cnt[p.x][p.y] += diff;
        }
    }

    bool can_create_rail(Operation& op){
        Pos cursor = op.from.get_abs();
        Pos to = op.to.get_abs();
        while(cursor != to){
            if(cursor.y == to.y || (cursor.x != to.x && op.x_prior)){
                cursor.x += (cursor.x < to.x ? 1 : -1);
            }else{
                cursor.y += (cursor.y < to.y ? 1 : -1);
            }
            if(cursor != to && used[cursor.x][cursor.y]) return false;
        }
        return true;
    }
};

/**
 * @brief 普通のSA
 * operationsを焼く。駅の追加、駅の削除、駅の1マス移動、順序Swap、中継の追加など
 */
struct SA{
    struct Transition{
        int max_turn = T;
        int max_terminal = INT_MAX;

        int types[2000];
        int type_size = 0;
        
        long long try_cnt[100];
        long long accept_cnt[100];
        const int TransitionTypeSize = 13;

        Transition(){
            rep(i,0,20) types[type_size++] = 0; // add_terminal
            rep(i,0,15) types[type_size++] = 1; // delete_terminal
            rep(i,0,10) types[type_size++] = 2; // move_terminal
            rep(i,0,20) types[type_size++] = 3; // swap_operation
            rep(i,0,10) types[type_size++] = 4; // delete_relay_terminal
            rep(i,0,20) types[type_size++] = 5; // flip_xprior
            rep(i,0,30) types[type_size++] = 6; // change_parent
            rep(i,0,20) types[type_size++] = 7; // merge_to_relay
            rep(i,0,15) types[type_size++] = 8; // change_parent_relay
            rep(i,0,5) types[type_size++] = 9; // add_terminal_with_relay
            rep(i,0,10) types[type_size++] = 10; // merget_to_other_relay
            rep(i,0,10) types[type_size++] = 11; // swap_init_2_terminals
            // rep(i,0,10) types[type_size++] = 12; // add_and_delete_terminal
        }

        void set_final_stage_transition_rate(){
            type_size = 0;
            rep(i,0,3) types[type_size++] = 0; // add_terminal
            rep(i,0,3) types[type_size++] = 1; // delete_terminal
            rep(i,0,20) types[type_size++] = 2; // move_terminal
            rep(i,0,20) types[type_size++] = 3; // swap_operation
            rep(i,0,1) types[type_size++] = 4; // delete_relay_terminal
            rep(i,0,20) types[type_size++] = 5; // flip_xprior
            rep(i,0,10) types[type_size++] = 6; // change_parent
            rep(i,0,1) types[type_size++] = 7; // merge_to_relay
            rep(i,0,1) types[type_size++] = 8; // change_parent_relay
            // rep(i,0,1) types[type_size++] = 9; // add_terminal_with_relay
            // rep(i,0,1) types[type_size++] = 10; // merget_to_other_relay
            // rep(i,0,3) types[type_size++] = 12; // add_and_delete_terminal
            // rep(i,0,11) rep(j,0,10) types[type_size++] = i;
        }

        void set_transition_fix(int type){
            type = 2;
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
                case 0: res = add_terminal(state, current_temp); break;
                case 1: res = delete_terminal(state, current_temp); break;
                case 2: res = move_terminal(state, current_temp); break;
                case 3: res = swap_operation(state, current_temp); break;
                case 4: res = delete_relay_terminal(state, current_temp); break;
                case 5: res = flip_xprior(state, current_temp); break;
                case 6: res = change_parent(state, current_temp); break;
                case 7: res = merge_to_relay(state, current_temp); break;
                case 8: res = change_parent_relay(state, current_temp); break;
                case 9: res = add_terminal_with_relay(state, current_temp); break;
                case 10: res = merge_to_other_relay(state, current_temp); break;
                case 11: res = swap_init_2_terminals(state, current_temp); break;
                case 12: res = add_and_delete_terminal(state, current_temp); break;

                case 13: res = debug_simulate_only(state); break;
            }
            try_cnt[type]++;
            if(res) accept_cnt[type]++;
            return res;
        }

        bool should_accept_transition(State& state, SimulationResult& res, float current_temp){
            if(res.turn > max_turn) return false;
            int score_diff = res.score() - state.result.score();
            return score_diff > 0 || rnd.rand() < exp(score_diff / current_temp);
        }

        bool debug_simulate_only(State& state){
            simulator.simulate_unsafe(state.operations);
            return true;
        }

        bool add_terminal(State& state, float current_temp){
            if(state.terminals.size() >= max_terminal) return false;
            Pos best_new_pos;
            Pos best_from;
            Operation best_new_op;
            int best_new_income = -1;
            int best_new_total_covered_worth = -1;

            int tr = 20;
            while(tr--){
                Pos new_pos;
                int max_trial = 5;
                while(max_trial--){
                    int idx = min(rnd() % board_info.good_cells_size, rnd() % board_info.good_cells_size);
                    new_pos = board_info.good_cells[idx].second;
                    if(!state.terminal_placed[new_pos.x][new_pos.y]) break;
                }
                if(max_trial < 0) continue;
                
                // 新しいincomeの計算
                int new_income = state.result.income;
                int new_total_covered_worth = state.result.total_covered_worth;
                int cover_pos_size = board_info.terminal_cover_poss_size[new_pos.x][new_pos.y];
                rep(i,0,cover_pos_size){
                    auto& pos = board_info.terminal_cover_poss[new_pos.x][new_pos.y][i];
                    if(state.terminal_covered_cnt[pos.x][pos.y] == 0){
                        new_total_covered_worth += board_info.cell_worth[pos.x][pos.y];
                        int pair_poss_size = board_info.pair_poss_size[pos.x][pos.y];
                        rep(d,0,pair_poss_size){
                            auto& p = board_info.pair_poss[pos.x][pos.y][d];
                            if(state.terminal_covered_cnt[p.x][p.y] > 0)
                                new_income += board_info.pair_poss_score[pos.x][pos.y][d];
                        }
                    }
                }
                if(new_income + new_total_covered_worth * total_covered_worth_coef <= best_new_income + best_new_total_covered_worth * total_covered_worth_coef) continue;

                // add_terminalについては、full simulationを行わなくても、追加されるincomeさえ計算出来れば、O(1)でturn, income, k, final_kは計算できる（操作列の一番後ろに追加する前提）
                Pos from;
                if(state.used[new_pos.x][new_pos.y]){
                    // 線路上に直接作る場合
                    from = new_pos.get_flip();
                }else{
                    // まだ線路がない点に作る場合
                    float gumbel_trick_min = 1e9;
                    for(auto& pos: state.terminals){
                        float gumbel = gumbel_sampler.sample();
                        if(chmin(gumbel_trick_min, gumbel + pos.dist(new_pos)))
                            from = pos;
                    }
                }
                Operation new_op = {from, new_pos, rnd() % 2 == 0};
                if(!state.can_create_rail(new_op)) continue;

                // if(chmax(best_new_income, new_income)){
                best_new_income = new_income;
                best_new_pos = new_pos;
                best_new_op = new_op;
                best_from = from;
                best_new_total_covered_worth = new_total_covered_worth;
                // }
            }
            if(best_new_income == -1) return false;
            Pos new_pos = best_new_pos;
            Operation new_op = best_new_op;
            int new_income = best_new_income;
            int new_total_covered_worth = best_new_total_covered_worth;
            

            // full-simulationせずに、final_kを計算して、焼きなましの判定
            auto res = simulator.simulate_unsafe_one_add_terminal_operation(new_op, state.result.turn, state.result.income, state.result.k, new_income, new_total_covered_worth);

            if(!should_accept_transition(state, res, current_temp)) return false;

            // acceptなので、stateを更新
            if(state.used[new_pos.x][new_pos.y]){
                static V<Operation> new_operations;
                new_operations.clear();
                bool inserted = false;
                rep(i,0,state.operations.size()){
                    auto& op = state.operations[i];
                    if(!inserted && simulator.contains_pos_in_operation(op, new_pos)){
                        new_operations.push_back({op.from, new_pos.get_flip(), op.x_prior});
                        new_operations.push_back({new_pos.get_flip(), op.to, op.x_prior});
                        inserted = true;
                    }else{
                        new_operations.push_back(op);
                    }
                }
                new_operations.push_back(new_op);
                swap(state.operations, new_operations);
            }else{
                state.operations.push_back(new_op);
                state.flip_rail_used(state.operations.back());
                state.used[new_pos.x][new_pos.y] = true;
            }

            state.terminals.push_back(new_pos);
            state.update_terminal_cover(new_pos, true);
            state.result = res;

            return true;
        }

        bool delete_terminal(State& state, float current_temp){
            // 葉である駅を1つ削除
            if(state.operations.size() < 2) return false;
            static int leaf[N][N];
            static int cur;
            cur++;
            for(auto& op: state.operations){
                Pos from = op.from.get_abs();
                Pos to = op.to.get_abs();
                leaf[from.x][from.y] = 0;
                if(op.from.valid()) leaf[to.x][to.y] = cur;
            }
            static V<Pos> candidates;
            candidates.clear();
            for(auto& terminal : state.terminals){
                if(leaf[terminal.x][terminal.y] == cur) candidates.push_back(terminal);
            }
            if(candidates.empty()) return false;

            Pos best_del_pos;
            int best_new_income = -1;
            rep(_,0,1){
                auto del_pos = candidates[rnd() % candidates.size()];
                if(state.fixed[del_pos.x][del_pos.y]) continue;

                int new_income = state.result.income;
                int cover_pos_size = board_info.terminal_cover_poss_size[del_pos.x][del_pos.y];
                rep(i,0,cover_pos_size){
                    auto& pos = board_info.terminal_cover_poss[del_pos.x][del_pos.y][i];
                    if(state.terminal_covered_cnt[pos.x][pos.y] == 1){
                        int pair_poss_size = board_info.pair_poss_size[pos.x][pos.y];
                        rep(d,0,pair_poss_size){
                            auto& p = board_info.pair_poss[pos.x][pos.y][d];
                            if(state.terminal_covered_cnt[p.x][p.y] > 0)
                                new_income -= board_info.pair_poss_score[pos.x][pos.y][d];
                        }
                    }
                }
                if(chmax(best_new_income, new_income)) best_del_pos = del_pos;
            }

            if(best_new_income == -1) return false;
            Pos del_pos = best_del_pos;

            Operation del_op;
            static V<Operation> new_operations;
            new_operations.clear();
            for(auto& op: state.operations){
                if(op.to != del_pos) new_operations.push_back(op);
                else del_op = op;
            }
            auto res = simulator.simulate_unsafe(new_operations);

            if(!should_accept_transition(state, res, current_temp)) return false;
            // accept
            state.terminals.erase(find(all(state.terminals), del_pos));
            swap(state.operations, new_operations);
            state.flip_rail_used(del_op);
            state.used[del_pos.x][del_pos.y] = false;
            state.update_terminal_cover(del_pos, false);
            state.result = res;
            return true;            
        }

        bool add_and_delete_terminal(State& state, float current_temp){
            // 駅を1つ追加して、1つ削除
            // 削除駅をdeleteと同様に選ぶ
            // Add駅をAddと同様に選ぶ。ただし、fromは削除駅以外になる
            if(state.operations.size() < 2) return false;
            // static int leaf[N][N];
            // static int cur;
            // cur++;
            // static int del_op_idxs[N][N];
            // // for(auto& op: state.operations){
            // rep(i,0,state.operations.size()){
            //     auto& op = state.operations[i];
            //     Pos from = op.from.get_abs();
            //     Pos to = op.to.get_abs();
            //     leaf[from.x][from.y] = 0;
            //     if(op.from.valid()) leaf[to.x][to.y] = cur, del_op_idxs[to.x][to.y] = i;
            // }
            // static V<Pos> candidates;
            // candidates.clear();
            // for(auto& terminal : state.terminals){
            //     if(leaf[terminal.x][terminal.y] == cur) candidates.push_back(terminal);
            // }
            // if(candidates.empty()) return false;
            // auto del_pos = candidates[rnd() % candidates.size()];
            // if(state.fixed[del_pos.x][del_pos.y]) return false;

            // auto del_op_idx = del_op_idxs[del_pos.x][del_pos.y];
            // auto del_op = state.operations[del_op_idx];
            if(!state.operations.back().from.valid()) return false;
            auto del_op_idx = state.operations.size() - 1;
            auto del_op = state.operations.back();
            auto del_pos = del_op.to.get_abs();
            if(state.fixed[del_pos.x][del_pos.y]) return false;

            // addを選ぶ前に消しておく
            state.flip_rail_used(del_op);
            state.used[del_pos.x][del_pos.y] = false;
            
            // Addを選ぶ
            Pos new_pos;
            int max_trial = 10;
            while(max_trial--){
                int idx = min(rnd() % board_info.good_cells_size, rnd() % board_info.good_cells_size);
                new_pos = board_info.good_cells[idx].second;
                // new_pos = board_info.good_cells[rnd() % board_info.good_cells.size()].second;
                if(!state.terminal_placed[new_pos.x][new_pos.y]) break;
            }
            if(max_trial < 0) {
                return false;
                state.flip_rail_used(del_op);
                state.used[del_pos.x][del_pos.y] = true;
            }
            Pos from;
            if(state.used[new_pos.x][new_pos.y]){
                // 線路上に直接作る場合
                from = new_pos.get_flip();
            }else{
                // まだ線路がない点に作る場合
                float gumbel_trick_min = 1e9;
                for(auto& pos: state.terminals){
                    if(pos == del_pos) continue;
                    float gumbel = gumbel_sampler.sample();
                    if(chmin(gumbel_trick_min, gumbel + pos.dist(new_pos)))
                        from = pos;
                }
            }
            Operation new_op = {from, new_pos, rnd() % 2 == 0};
            if(!state.can_create_rail(new_op)) {
                state.flip_rail_used(del_op);
                state.used[del_pos.x][del_pos.y] = true;
                return false;
            }

            static V<Operation> new_operations;
            new_operations.clear();
            if(state.used[new_pos.x][new_pos.y]){
                bool inserted = false;
                rep(i,0,state.operations.size()){
                    if(i == del_op_idx) continue;

                    auto& op = state.operations[i];
                    if(!inserted && simulator.contains_pos_in_operation(op, new_pos)){
                        new_operations.push_back({op.from, new_pos.get_flip(), op.x_prior});
                        new_operations.push_back({new_pos.get_flip(), op.to, op.x_prior});
                        inserted = true;
                    }else{
                        new_operations.push_back(op);
                    }
                }
                new_operations.push_back(new_op);
            }else{
                rep(i,0,del_op_idx) new_operations.push_back(state.operations[i]);
                rep(i,del_op_idx+1,state.operations.size()) new_operations.push_back(state.operations[i]);
                new_operations.push_back(new_op);
            }

            auto res = simulator.simulate_unsafe(new_operations);
            if(should_accept_transition(state, res, current_temp)){
                state.terminals.erase(find(all(state.terminals), del_pos));
                state.terminals.push_back(new_pos);
                state.update_terminal_cover(del_pos, false);
                state.update_terminal_cover(new_pos, true);
                swap(state.operations, new_operations);
                state.flip_rail_used(new_op);
                state.used[new_pos.x][new_pos.y] = true;
                state.result = res;
                simulator.debug_rail_check(state.operations, state.used);
                simulator.debug_terminal_topological_sort(state.operations);
                return true;
            }else{
                state.flip_rail_used(del_op);
                state.used[del_pos.x][del_pos.y] = true;
                return false;
            }
        }
        bool move_terminal(State& state, float current_temp){
            // 駅を1マス移動
            int t_idx = rnd() % state.terminals.size();
            auto t_pos = state.terminals[t_idx];
            if(state.fixed[t_pos.x][t_pos.y]) return false;

            Pos new_pos;
            // TODO: TerminalRange内をbool[N][N]で表現したデータをStateに持たせて、O(1)で判定できるようにする
            int max_trial = 3;
            while(max_trial--){
                new_pos = t_pos + Pos(rnd() % 5 - 2, rnd() % 5 - 2);
                if(new_pos == t_pos || new_pos == Pos(0, 0) || !new_pos.valid()) continue;
                if(board_info.cell_terminal_reward[new_pos.x][new_pos.y] == 0) continue;
                if(!state.terminal_placed[new_pos.x][new_pos.y]) break;
                
            }
            if(max_trial < 0) return false;
            static V<pair<int, Operation>> changes;
            changes.clear();
            rep(i,0,state.operations.size()){
                auto& op = state.operations[i];
                auto from_valid = op.from.get_abs();
                auto to_valid = op.to.get_abs();
                if(from_valid == t_pos || to_valid == t_pos){
                    changes.push_back({i, op});
                }
            }

            // 線路が作れるかチェック
            state.used[t_pos.x][t_pos.y] = false;
            for(auto& [_, op] : changes) state.flip_rail_used(op);
            bool ok = !state.used[new_pos.x][new_pos.y];
            static bool new_x_prior;
            if(ok){
                rep(i,0,changes.size()){
                    auto [idx, op] = changes[i];
                    auto new_op = op;
                    if(op.from == t_pos) new_op.from = new_pos;
                    if(op.from == t_pos.get_flip()) new_op.from = new_pos.get_flip();
                    if(op.to == t_pos) new_op.to = new_pos;
                    if(op.to == t_pos.get_flip()) new_op.to = new_pos.get_flip();
                    new_x_prior = rnd() % 2 == 0;
                    bool can_create_rail = false;
                    rep(_,0,2){
                        new_x_prior = !new_x_prior;
                        new_op.x_prior = new_x_prior;
                        if(state.can_create_rail(new_op)){
                            state.operations[idx] = new_op;
                            state.flip_rail_used(new_op);
                            can_create_rail = true;
                            break;
                        }
                    }
                    if(!can_create_rail){
                        // rollback
                        rep(j,0,i){
                            auto [idx, op] = changes[j];
                            state.flip_rail_used(state.operations[idx]);
                            state.operations[idx] = op;
                        }
                        ok = false;
                        break;
                    }
                }
            }
            if(!ok){
                // rollback
                rep(i,0,changes.size()){
                    auto [idx, op] = changes[i];
                    state.operations[idx] = op;
                    state.flip_rail_used(op);
                }
                state.used[t_pos.x][t_pos.y] = true;
                return false;
            }
            

            // new_operationでsimulation
            auto res = simulator.simulate_unsafe(state.operations);
            if(should_accept_transition(state, res, current_temp)){
                state.terminals[t_idx] = new_pos;
                state.used[new_pos.x][new_pos.y] = true;
                state.update_terminal_cover(t_pos, false);
                state.update_terminal_cover(new_pos, true);
                state.result = res;
                return true;
            }else{
                // rollback
                rep(i,0,changes.size()){
                    auto [idx, op] = changes[i];
                    state.flip_rail_used(state.operations[idx]);
                    state.operations[idx] = op;
                    state.flip_rail_used(op);
                }
                state.used[t_pos.x][t_pos.y] = true;
                return false;
            } 
        }
    
        bool swap_operation(State& state, float current_temp){
            if(state.operations.size() < 3) return false;
            // after_idxのOperationを一つ前のOperationとswapする。ただし、中継経由のアクションは一塊でswapする
            int after_idx = rnd() % (state.operations.size() - 2) + 2;
            auto after_op = state.operations[after_idx];
            // 中継から別の点に移動しているOperationはswap不可
            if(!after_op.from.valid() && (after_op.from.get_flip() != after_op.to)) return false;
            auto prev_op = state.operations[after_idx - 1];
            // 中継点に駅を追加した直後に、その駅から伸ばすOperationはswap不可
            if(prev_op.from.get_flip() == prev_op.to && (prev_op.to == after_op.from)) return false;
            int after_batch_end = after_idx + 1;
            while(!state.operations[after_batch_end-1].to.valid()) after_batch_end++;
            /**
             * prev_end.toとafter.fromが同一の場合、prev_end.toを経由→afterの塊→prev_end.toに駅追加
             * after_op.fromが中継の場合、
             *      prevの塊にafter_opがあるなら、中継削除してそのまま駅追加OPに変更
             *      prevの塊にafter_opがないなら、prevの塊の前にafterの塊を持ってくる
             * それ以外（=別の駅から線路を伸ばしている時）、prevの塊の前にafterの塊を持ってくる
             * 
             */
            static V<Operation> new_operations;
            new_operations.clear();
            if(prev_op.to == after_op.from){
                rep(i,0,after_idx) new_operations.push_back(state.operations[i]);
                new_operations.back().to = new_operations.back().to.get_flip();
                new_operations.push_back({after_op.from.get_flip(), after_op.to, after_op.x_prior});
                rep(i,after_idx+1,after_batch_end) new_operations.push_back(state.operations[i]);
                new_operations.push_back({prev_op.to.get_flip(), prev_op.to, prev_op.x_prior});
                rep(i,after_batch_end,state.operations.size()) new_operations.push_back(state.operations[i]);
            }else{
                int prev_batch_start = after_idx - 1;
                int relay_idx = -1;
                while(!state.operations[prev_batch_start].from.valid() && (state.operations[prev_batch_start].from.get_flip() != state.operations[prev_batch_start].to)){
                    prev_batch_start--;
                    if(state.operations[prev_batch_start].to == after_op.from)
                        relay_idx = prev_batch_start;
                }
                if(relay_idx >= 0){
                    // 中継削除してそのまま駅追加OPに変更
                    rep(i,0,relay_idx) new_operations.push_back(state.operations[i]);
                    new_operations.push_back({state.operations[relay_idx].from, state.operations[relay_idx].to.get_flip(), state.operations[relay_idx].x_prior});
                    new_operations.push_back({state.operations[relay_idx+1].from.get_flip(), state.operations[relay_idx+1].to, state.operations[relay_idx+1].x_prior});
                    rep(i,relay_idx+2,after_idx) new_operations.push_back(state.operations[i]);
                    rep(i,after_idx+1,state.operations.size()) new_operations.push_back(state.operations[i]);
                }else{
                    // prevの塊の前にafterの塊を持ってくる
                    rep(i,0,prev_batch_start) new_operations.push_back(state.operations[i]);
                    rep(i,after_idx,after_batch_end) new_operations.push_back(state.operations[i]);
                    rep(i,prev_batch_start,after_idx) new_operations.push_back(state.operations[i]);
                    rep(i,after_batch_end,state.operations.size()) new_operations.push_back(state.operations[i]);
                }
            }

            auto res = simulator.simulate_unsafe(new_operations);
            if(should_accept_transition(state, res, current_temp)){
                swap(state.operations, new_operations);
                state.result = res;
                return true;
            }else{
                return false;
            }
        }

        bool delete_relay_terminal(State& state, float current_temp){
            // 中継地点から生成する駅を削除。ただし、その駅から伸ばす線路がある場合は削除不可
            static int relay_terminal[N][N];
            static int cur;
            cur++;
            rep(i,1,state.operations.size()){
                auto& op = state.operations[i];
                if(op.from.get_flip() == op.to) relay_terminal[op.to.x][op.to.y] = cur;
                else if(op.from.valid()) relay_terminal[op.from.x][op.from.y] = 0;
            }
            static V<Pos> candidates;
            candidates.clear();
            for(auto& terminal : state.terminals){
                if(relay_terminal[terminal.x][terminal.y] == cur) candidates.push_back(terminal);
            }
            if(candidates.empty()) return false;
            auto del_pos = candidates[rnd() % candidates.size()];
            if(state.fixed[del_pos.x][del_pos.y]) return false;

            int del_relay_idx = -1;
            int del_create_idx = -1;
            rep(i,0,state.operations.size()){
                if(state.operations[i].to.get_flip() == del_pos)
                    del_relay_idx = i;
                if(state.operations[i].to == del_pos){
                    del_create_idx = i;
                    break;
                }
            }
            // 線路の繋げ直しが可能かチェック。中継箇所で、中継を経由せずに行ければOK
            state.flip_rail_used(state.operations[del_relay_idx]);
            state.flip_rail_used(state.operations[del_relay_idx+1]);
            state.used[del_pos.x][del_pos.y] = false;
            
            bool ok = false;
            // from = del_relay_idx.from, to = del_relay_idx+1.toの線路を引けるか
            Operation new_op;
            rep(i,0,2){
                new_op = {state.operations[del_relay_idx].from, state.operations[del_relay_idx+1].to, i == 0};
                if(state.can_create_rail(new_op)){
                    ok = true;
                    break;
                }
            }
            if(!ok){
                // rollback
                state.used[del_pos.x][del_pos.y] = true;
                state.flip_rail_used(state.operations[del_relay_idx]);
                state.flip_rail_used(state.operations[del_relay_idx+1]);
                return false;
            }

            // new_operationでsimulation
            static V<Operation> new_operations;
            new_operations.clear();
            rep(i,0,del_relay_idx) new_operations.push_back(state.operations[i]);
            new_operations.push_back(new_op);
            rep(i,del_relay_idx+2,del_create_idx) new_operations.push_back(state.operations[i]);
            rep(i,del_create_idx+1,state.operations.size()) new_operations.push_back(state.operations[i]);

            auto res = simulator.simulate_unsafe(new_operations);
            if(should_accept_transition(state, res, current_temp)){
                swap(state.operations, new_operations);
                state.terminals.erase(find(all(state.terminals), del_pos));
                state.flip_rail_used(new_op);
                state.update_terminal_cover(del_pos, false);
                state.result = res;
                return true;
            }else{
                // rollback
                state.used[del_pos.x][del_pos.y] = true;
                state.flip_rail_used(state.operations[del_relay_idx]);
                state.flip_rail_used(state.operations[del_relay_idx+1]);
                return false;
            }
        }
        
        bool flip_xprior(State& state, float current_temp){
            // x_priorを反転
            int idx = rnd() % state.operations.size();
            
            auto op = state.operations[idx];
            auto new_op = op; new_op.x_prior = !op.x_prior;
            if(!state.can_create_rail(new_op)) return false;
            state.flip_rail_used(op);
            state.flip_rail_used(new_op);
            state.operations[idx] = new_op;
            return true;
        }

        bool change_parent(State& state, float current_temp){
            // 線路を伸ばす親を変更
            if(state.operations.size() < 2) return false;
            int max_trial = 5;
            int change_op_idx;
            while(max_trial--){
                change_op_idx = rnd() % state.operations.size();
                // 実点から生成してる駅のみ対象
                if(state.operations[change_op_idx].from.valid()) break;
            }
            if(max_trial < 0) return false;
            auto op = state.operations[change_op_idx];
            
            Pos new_from = op.from;
            float gumbel_trick_min = 1e9;
            rep(i,0,change_op_idx){
                auto pos = state.operations[i].to;
                if(!pos.valid()) continue;
                if(pos == op.from) continue;
                float gumbel = gumbel_sampler.sample();
                if(chmin(gumbel_trick_min, gumbel + pos.dist(op.to.get_abs())))
                    new_from = pos;
            }
            if(new_from == op.from) return false;
            Operation new_op = {new_from, op.to, op.x_prior};
            // new_opに差し替えられるかチェック
            state.flip_rail_used(op);
            if(!state.can_create_rail(new_op)){
                state.flip_rail_used(op);
                return false;
            }
            state.operations[change_op_idx] = new_op;
            auto res = simulator.simulate_unsafe(state.operations);
            if(should_accept_transition(state, res, current_temp)){
                state.flip_rail_used(new_op);
                state.result = res;
                return true;
            }else{
                state.operations[change_op_idx] = op;
                state.flip_rail_used(op);
                return false;
            }
        }

        bool change_parent_relay(State& state, float current_temp){
            // 中継地点からその場生成してる駅の親を変更
            if(state.operations.size() < 3) return false;
            int max_trial = 5;
            int change_op_idx;
            while(max_trial--){
                change_op_idx = rnd() % (state.operations.size() - 1) + 1;
                if(state.operations[change_op_idx].from.get_flip() == state.operations[change_op_idx].to) break;
            }
            if(max_trial < 0) return false;
            auto op = state.operations[change_op_idx];
            Pos t_pos = op.to;
            // 中継を生成するOPindexを探す
            int relay_start_idx = -1;
            rep(i,0,change_op_idx){
                if(state.operations[i].to == op.from){
                    relay_start_idx = i;
                    break;
                }
            }
            // 元の中継生成OPを中継無くして繋ぎ直せるかチェック
            state.flip_rail_used(state.operations[relay_start_idx]);
            state.flip_rail_used(state.operations[relay_start_idx+1]);
            bool ok = false;
            Operation new_relay_op;
            static bool relay_x_prior;
            rep(_,0,2){
                relay_x_prior = !relay_x_prior;
                new_relay_op = {state.operations[relay_start_idx].from, state.operations[relay_start_idx+1].to, relay_x_prior};
                if(state.can_create_rail(new_relay_op)){
                    ok = true;
                    break;
                }
            }
            if(!ok){
                // rollback
                state.flip_rail_used(state.operations[relay_start_idx]);
                state.flip_rail_used(state.operations[relay_start_idx+1]);
                return false;
            }
            state.flip_rail_used(new_relay_op);

            Pos new_from = op.from;
            float gumbel_trick_min = 1e9;
            rep(i,0,change_op_idx){
                auto pos = state.operations[i].to;
                if(!pos.valid()) continue;
                if(pos == op.from) continue;
                float gumbel = gumbel_sampler.sample();
                if(chmin(gumbel_trick_min, gumbel + pos.dist(t_pos)))
                    new_from = pos;
            }
            assert(new_from != op.from);
            
            // new_opに差し替えられるかチェック
            Operation new_op;
            static bool new_x_prior;
            bool can_create_new_op = false;
            rep(_,0,2){
                new_x_prior = !new_x_prior;
                new_op = {new_from, op.to, new_x_prior};
                if(state.can_create_rail(new_op)){
                    can_create_new_op = true;
                    break;
                }
            }
            
            if(!can_create_new_op){
                state.flip_rail_used(new_relay_op);
                state.flip_rail_used(state.operations[relay_start_idx]);
                state.flip_rail_used(state.operations[relay_start_idx+1]);
                return false;
            }

            static V<Operation> new_operations;
            new_operations.clear();
            rep(i,0,relay_start_idx) new_operations.push_back(state.operations[i]);
            new_operations.push_back(new_relay_op);
            rep(i,relay_start_idx+2,change_op_idx) new_operations.push_back(state.operations[i]);
            new_operations.push_back(new_op);
            rep(i,change_op_idx+1,state.operations.size()) new_operations.push_back(state.operations[i]);

            auto res = simulator.simulate_unsafe(new_operations);
            if(should_accept_transition(state, res, current_temp)){
                swap(state.operations, new_operations);
                state.flip_rail_used(new_op);
                state.result = res;
                return true;
            }else{
                state.flip_rail_used(new_relay_op);
                state.flip_rail_used(state.operations[relay_start_idx]);
                state.flip_rail_used(state.operations[relay_start_idx+1]);
                return false;
            }
        }

        bool merge_to_relay(State& state, float current_temp){
            // 実点から生成してる駅を、それより前のOPの中継地点に変更
            if(state.operations.size() < 3) return false;
            int max_trial = 5;
            int change_op_idx;
            while(max_trial--){
                change_op_idx = rnd() % state.operations.size();
                if(state.operations[change_op_idx].from.valid() && state.operations[change_op_idx].to.valid()) break;
            }
            if(max_trial < 0) return false;
            auto change_op = state.operations[change_op_idx];
            Pos change_t_pos = change_op.to;
            
            // どのOperationのtoに中継地点を突っ込むか
            int relay_op_idx = -1;
            float gumbel_trick_min = 1e9;
            rep(i,0,change_op_idx){
                auto op = state.operations[i];
                if(op.from.get_abs() == op.to.get_abs()) continue;
                Pos op_from = op.from.get_abs();
                Pos op_to = op.to.get_abs();
                int add_dist = op_from.dist(change_t_pos) + change_t_pos.dist(op_to) - op_from.dist(op_to);
                float gumbel = gumbel_sampler.sample();
                if(chmin(gumbel_trick_min, gumbel + add_dist))
                    relay_op_idx = i;
            }
            if(relay_op_idx < 0) return false;
            auto relay_op = state.operations[relay_op_idx];
            // 中継に変更して、線路を繋げられるかチェック
            state.flip_rail_used(relay_op);
            state.flip_rail_used(change_op);
            static Operation new_ops[2];
            bool ok = false;
            rep(i,0,4){
                new_ops[0] = {relay_op.from, change_t_pos.get_flip(), i == 1};
                new_ops[1] = {change_t_pos.get_flip(), relay_op.to, (i >> 1) == 1};
                if(!state.can_create_rail(new_ops[0]))
                    continue;
                state.flip_rail_used(new_ops[0]);
                if(!state.can_create_rail(new_ops[1])){
                    state.flip_rail_used(new_ops[0]);
                    continue;
                }
                state.flip_rail_used(new_ops[1]);
                ok = true;
                break;
            }
            if(!ok){
                state.flip_rail_used(relay_op);
                state.flip_rail_used(change_op);
                return false;
            }
            // new_operationでsimulation
            static V<Operation> new_operations;
            new_operations.clear();
            rep(i,0,relay_op_idx) new_operations.push_back(state.operations[i]);
            rep(i,0,2) new_operations.push_back(new_ops[i]);
            rep(i,relay_op_idx+1,change_op_idx) new_operations.push_back(state.operations[i]);
            new_operations.push_back({change_t_pos.get_flip(), change_t_pos, change_op.x_prior});
            rep(i,change_op_idx+1,state.operations.size()) new_operations.push_back(state.operations[i]);
            
            auto res = simulator.simulate_unsafe(new_operations);
            if(should_accept_transition(state, res, current_temp)){
                swap(state.operations, new_operations);
                state.result = res;
                return true;
            }else{
                state.flip_rail_used(relay_op);
                state.flip_rail_used(change_op);
                rep(i,0,2) state.flip_rail_used(new_ops[i]);
                return false;
            }
        }
    
        bool add_terminal_with_relay(State& state, float current_temp){
            // 既存のOPに中継点を作って、最後に駅を追加
            if(state.terminals.size() >= max_terminal) return false;
            if(state.operations.size() < 2) return false;
            Pos best_new_pos;
            int best_new_income = -1;
            rep(_,0,1){
                int max_trial = 5;
                Pos new_pos;
                while(max_trial--){
                    int idx = min(rnd() % board_info.good_cells_size, rnd() % board_info.good_cells_size);
                    new_pos = board_info.good_cells[idx].second;
                    // new_pos = board_info.good_cells[rnd() % board_info.good_cells.size()].second;
                    // 線路上の点は選ばない。その線路を作るOPの中継点として見れるが、それはadd_terminalでカバーできているので不要
                    if(state.used[new_pos.x][new_pos.y]) continue;
                    if(state.terminal_placed[new_pos.x][new_pos.y]) continue;
                    break;
                }
                if(max_trial < 0) continue;

                // new income計算
                int new_income = state.result.income;
                int cover_pos_size = board_info.terminal_cover_poss_size[new_pos.x][new_pos.y];
                rep(i,0,cover_pos_size){
                    auto& pos = board_info.terminal_cover_poss[new_pos.x][new_pos.y][i];
                    if(state.terminal_covered_cnt[pos.x][pos.y] == 0){
                        int pair_poss_size = board_info.pair_poss_size[pos.x][pos.y];
                        rep(d,0,pair_poss_size){
                            auto& p = board_info.pair_poss[pos.x][pos.y][d];
                            if(state.terminal_covered_cnt[p.x][p.y] > 0)
                                new_income += board_info.pair_poss_score[pos.x][pos.y][d];
                        }
                    }
                }
                if(chmax(best_new_income, new_income)) best_new_pos = new_pos;
            }
            if(best_new_income == -1) return false;
            Pos new_pos = best_new_pos;
            
            // 中継地点を作るOPを選ぶ
            int relay_op_idx = -1;
            float gumbel_trick_min = 1e9;
            rep(i,0,state.operations.size()){
                auto op = state.operations[i];
                Pos op_from = op.from.get_abs();
                Pos op_to = op.to.get_abs();
                if(op_from == op_to) continue;
                int add_dist = op_from.dist(new_pos) + new_pos.dist(op_to) - op_from.dist(op_to);
                float gumbel = gumbel_sampler.sample();
                if(chmin(gumbel_trick_min, gumbel + add_dist))
                    relay_op_idx = i;
            }

            auto relay_op = state.operations[relay_op_idx];
            // 中継に変更して、線路を繋げられるかチェック
            state.flip_rail_used(relay_op);
            static Operation new_ops[2];
            bool ok = false;
            rep(i,0,4){
                new_ops[0] = {relay_op.from, new_pos.get_flip(), i == 1};
                new_ops[1] = {new_pos.get_flip(), relay_op.to, (i >> 1) == 1};
                if(!state.can_create_rail(new_ops[0]))
                    continue;
                state.flip_rail_used(new_ops[0]);
                if(!state.can_create_rail(new_ops[1])){
                    state.flip_rail_used(new_ops[0]);
                    continue;
                }
                state.flip_rail_used(new_ops[1]);
                ok = true;
                break;
            }
            if(!ok){
                state.flip_rail_used(relay_op);
                return false;
            }
            // new_operationでsimulation
            static V<Operation> new_operations;
            new_operations.clear();
            rep(i,0,relay_op_idx) new_operations.push_back(state.operations[i]);
            rep(i,0,2) new_operations.push_back(new_ops[i]);
            rep(i,relay_op_idx+1,state.operations.size()) new_operations.push_back(state.operations[i]);
            new_operations.push_back({new_pos.get_flip(), new_pos, rnd() % 2 == 0});

            auto res = simulator.simulate_unsafe(new_operations);
            if(should_accept_transition(state, res, current_temp)){
                swap(state.operations, new_operations);
                state.terminals.push_back(new_pos);
                state.used[new_pos.x][new_pos.y] = true;
                state.update_terminal_cover(new_pos, true);
                state.result = res;
                return true;
            }else{
                state.flip_rail_used(relay_op);
                rep(i,0,2) state.flip_rail_used(new_ops[i]);
                return false;
            }
        }

        bool merge_to_other_relay(State& state, float current_temp){
            // 中継地点を他の中継地点に移す
            if(state.operations.size() < 3) return false;
            int max_trial = 5;
            int change_op_idx;
            while(max_trial--){
                change_op_idx = rnd() % (state.operations.size() - 1) + 1;
                if(state.operations[change_op_idx].from.get_flip() == state.operations[change_op_idx].to) break;
            }
            if(max_trial < 0) return false;
            auto change_op = state.operations[change_op_idx];
            Pos change_t_pos = change_op.to;

            // 元々の中継OPを探す
            int prev_relay_start_idx = -1;
            rep(i,0,change_op_idx){
                if(state.operations[i].to == change_op.from){
                    prev_relay_start_idx = i;
                    break;
                }
            }
            auto prev_relay_from = state.operations[prev_relay_start_idx].from;
            auto prev_relay_to = state.operations[prev_relay_start_idx+1].to;
            
            // どのOperationのtoに中継地点を突っ込むか
            int relay_op_idx = -1;
            float gumbel_trick_min = 1e9;
            rep(i,0,change_op_idx){
                if(i == prev_relay_start_idx || i == prev_relay_start_idx+1) continue;
                auto op = state.operations[i];
                if(op.from.get_abs() == op.to.get_abs()) continue;
                Pos op_from = op.from.get_abs();
                Pos op_to = op.to.get_abs();
                int add_dist = op_from.dist(change_t_pos) + change_t_pos.dist(op_to) - op_from.dist(op_to);
                float gumbel = gumbel_sampler.sample();
                if(chmin(gumbel_trick_min, gumbel + add_dist))
                    relay_op_idx = i;
            }
            if(relay_op_idx < 0) return false;
            auto relay_op = state.operations[relay_op_idx];

            // 中継に変更して、線路を繋げられるかチェック
            rep(i,0,2) state.flip_rail_used(state.operations[prev_relay_start_idx+i]);
            state.flip_rail_used(relay_op);

            static Operation new_prev_op;
            static Operation new_ops[2];
            bool ok = false;
            rep(i,0,8){
                new_prev_op = {prev_relay_from, prev_relay_to, bool(i & 1)};
                new_ops[0] = {relay_op.from, change_t_pos.get_flip(), bool((i >> 1) & 1)};
                new_ops[1] = {change_t_pos.get_flip(), relay_op.to, bool((i >> 2) & 1)};
                if(!state.can_create_rail(new_prev_op))
                    continue;
                state.flip_rail_used(new_prev_op);
                if(!state.can_create_rail(new_ops[0])){
                    state.flip_rail_used(new_prev_op);
                    continue;
                }
                state.flip_rail_used(new_ops[0]);
                if(!state.can_create_rail(new_ops[1])){
                    state.flip_rail_used(new_prev_op);
                    state.flip_rail_used(new_ops[0]);
                    continue;
                }
                state.flip_rail_used(new_ops[1]);
                ok = true;
                break;
            }
            if(!ok){
                rep(i,0,2) state.flip_rail_used(state.operations[prev_relay_start_idx+i]);
                state.flip_rail_used(relay_op);
                return false;
            }
            // new_operationでsimulation
            static V<Operation> new_operations;
            new_operations.clear();
            if(prev_relay_start_idx < relay_op_idx){
                rep(i,0,prev_relay_start_idx) new_operations.push_back(state.operations[i]);
                new_operations.push_back(new_prev_op);
                rep(i,prev_relay_start_idx+2,relay_op_idx) new_operations.push_back(state.operations[i]);
                rep(i,0,2) new_operations.push_back(new_ops[i]);
                rep(i,relay_op_idx+1,state.operations.size()) new_operations.push_back(state.operations[i]);
            }else{
                rep(i,0,relay_op_idx) new_operations.push_back(state.operations[i]);
                rep(i,0,2) new_operations.push_back(new_ops[i]);
                rep(i,relay_op_idx+1,prev_relay_start_idx) new_operations.push_back(state.operations[i]);
                new_operations.push_back(new_prev_op);
                rep(i,prev_relay_start_idx+2,state.operations.size()) new_operations.push_back(state.operations[i]);
            }
            
            auto res = simulator.simulate_unsafe(new_operations);
            if(should_accept_transition(state, res, current_temp)){
                swap(state.operations, new_operations);
                state.result = res;
                return true;
            }else{
                rep(i,0,2) state.flip_rail_used(state.operations[prev_relay_start_idx+i]);
                state.flip_rail_used(relay_op);
                state.flip_rail_used(new_prev_op);
                rep(i,0,2) state.flip_rail_used(new_ops[i]);
                return false;
            }
        }

        bool swap_init_2_terminals(State& state, float current_temp){
            // 初期2駅を入れ替える
            if(state.terminals.size() < 2) return false;
            static V<Pos> route; route.clear();
            route.push_back(state.operations[0].to);
            int t_idx = 0;
            while(++t_idx){
                route.push_back(state.operations[t_idx].to.get_abs());
                if(state.operations[t_idx].to.valid()) break;
            }
            // 逆順にする
            reverse(all(route));
            static V<bool> new_x_priors; new_x_priors.clear();
            drep(i, t_idx, 0) new_x_priors.push_back(!state.operations[i].x_prior);
            state.operations[0] = {route[0].get_flip(), route[0]};
            rep(i,1,route.size()){
                auto from = i == 1 ? route[0] : route[i-1].get_flip();
                auto to = i == route.size() - 1 ? route[i] : route[i].get_flip();
                auto x_prior = new_x_priors[i-1];
                state.operations[i] = {from, to, x_prior};
            }
            return true;
        }

    } transition;

    struct Temp{
        // TODO: Mによって、1駅の価値が変わるので、Mで調整
        float start_temp = 100000;
        float mid_temp = 2000;
        float end_temp = 1;

        void init_temp(int terminal_score_average){
            // TODO: 1駅の価値によって変える
            
        }
        
        float get_temp(float progress){
            float threshold = 0.3;
            if(progress < threshold) // 指数
                return pow(start_temp, 1.0 - progress / threshold) * pow(mid_temp, progress / threshold);
                // return start_temp + (mid_temp - start_temp) * progress / threshold;
            else // 線形
                return mid_temp + (end_temp - mid_temp) * (progress - threshold) / (1.0 - threshold);
            // st^(1-p) * et^p
            // return pow(start_temp, 1.0 - progress) * pow(end_temp, progress);
            // return start_temp + (end_temp - start_temp) * progress;
        }
    } temp;

    int min_turn = 150;
    int max_turn = T;

    int min_terminal = N*N;
    int max_terminal = N*N;

    State run(int time_limit, State state){
        timer.mark_lap();

        State best_state = state;
        float current_temp = temp.start_temp;
        int loop_cnt = 0;
        int accept_cnt = 0;
        float detach_fix_temp = 2000;
        bool has_fix_detached = false;
        
        int last_loop_cnt = 0;
        int last_accept_cnt = 0;
        float next_log_progress = 0.095;
        while(timer.lap_ms() < time_limit){
            float progress = (float)timer.lap_ms() / time_limit;
            transition.max_turn = min_turn + (max_turn - min_turn) * min<float>(progress * 1.2, 1.0f);
            coef_max_turn = min_turn + (max_turn - min_turn) * progress * 1.2;
            transition.max_terminal = min_terminal + (max_terminal - min_terminal) * min<float>(progress * 1.2, 1.0f);
            total_covered_worth_coef = min<float>(0.3, 1.5 * (1 - progress));
            current_temp = temp.get_temp(progress);
            // if(current_temp < detach_fix_temp){
            if(!has_fix_detached && current_temp < min<float>(state.result.score() * 0.01, detach_fix_temp)){
                rep(i,0,N)rep(j,0,N) state.fixed[i][j] = false;
                // detach_fix_temp = 0; // 以降、やらないために
                cerr << "detach fix" << endl;
                has_fix_detached = true;
            }
            if(progress > next_log_progress){
                cerr << "progress: " << (int)((next_log_progress + 0.005) * 100) << "%, loop_cnt: " << loop_cnt - last_loop_cnt 
                << ", temp: " << (int)current_temp << ", accept_rate: " << (float)(accept_cnt - last_accept_cnt) / (loop_cnt - last_loop_cnt) 
                << ", simulated_cnt: " << simulator.simulate_cnt << ", op_length: " << state.operations.size() 
                << ", terminal_cnt: " << state.terminals.size() << ", final_k: " << state.result.final_k() << ", score: " << state.result.score() << ", turn: " << state.result.turn << endl;
                last_loop_cnt = loop_cnt;
                last_accept_cnt = accept_cnt;
                next_log_progress += 0.1;
                
                simulator.simulate_cnt = 0;
                transition.log_accept_rate();
                transition.reset_accept_log();
                // if(next_log_progress > 0.7) transition.set_transition_fix(2);
                // if(next_log_progress > 0.85) transition.set_final_stage_transition_rate();
            }

            if(progress > 0.8) transition.set_final_stage_transition_rate();

            rep(_,0,1024){
                loop_cnt++;
                bool accept = transition.execute(state, current_temp);
                accept_cnt += accept;

                if(accept && state.result.final_k() > best_state.result.final_k()){
                // if(accept && state.result.score() > best_state.result.score()){
                    int progress_int = progress * 100;
                    Pos t0 = state.operations[0].to;
                    Pos t1 = Pos(0, 0);
                    if(state.terminals.size() >= 2){
                        int t1_idx = 1;
                        while(t1 = state.operations[t1_idx].to, !t1.valid()) t1_idx++;
                    }
                    // cerr << "loop: " << loop_cnt << ", progress: " << progress_int << "%, temp: " << (int)current_temp
                    // << ", turn: " << state.result.turn << ", income: " << state.result.income
                    // << ", terminal_cnt: " << state.terminals.size() << ", final_score: " << state.result.final_k() << ", score: " << state.result.score()
                    // << " max_turn: " << transition.max_turn
                    // << endl; 
                    best_state = state;
                }
            }
        }
        cerr << "loop_cnt: " << loop_cnt << ", accept_rate: " << (float)accept_cnt / loop_cnt << endl;
        return best_state;
    }
};

/**
 * @brief 最初の数手分だけの最適解を求める
 * 駅D個まで作った段階での income/turn が最大になるような経路を求める
 * 駅数をキーとしてビムサを行う
 * 駅の生成順を固定すれば、V<Operation>はそれなりの数で全列挙可能（x_priorはsimulation_unsafeには影響しないので無視。最後にvalidな初期解を作る時だけ真面目にやる）
 */
struct InitialPhaseStateCreator{
    struct BeamState{
        V<Pos> terminals; // 生成する駅順に並べた駅リスト. ただし、各ターミナルは距離5以上離れて、重なりを作らない縛り
        int income;
        int turn;
        int k;
        float cost_performance;

        bool operator>(const BeamState& r) const{
            if (turn == r.turn) return income > r.income;
            int turn_diff = turn - r.turn;
            if(turn_diff > 0){
                float diff_rate = (float)turn_diff * r.income / TerminalCost;
                return income > r.income * (1 + diff_rate / terminals.size() * 2);
            }else{
                float diff_rate = (float)(-turn_diff) * income / TerminalCost;
                return income * (1 + diff_rate / terminals.size() * 2) > r.income;
            }
        }
    };
    const float COSPER_TURN_POW = 2;

    State best_state;
    float best_cost_performance = 0;
    
    /// {from, to}のterminal_indexのリストを全列挙
    VV<pair<int, int>> list_all_possible_templates(int terminal_cnt){
        VV<pair<int, int>> templates = {{{0, 1}}};
        rep(t_cnt, 2, terminal_cnt){
            VV<pair<int, int>> new_templates;

            for(auto temp : templates){
                rep(i,0,temp.size()){
                    if(abs(temp[i].first) == abs(temp[i].second)) continue;
                    // i番目の操作に新しい駅を挟む
                    V<pair<int, int>> new_temp;
                    rep(j,0,i) new_temp.push_back(temp[j]);
                    new_temp.push_back({temp[i].first, -t_cnt});
                    new_temp.push_back({-t_cnt, temp[i].second});
                    rep(j,i+1,temp.size()) new_temp.push_back(temp[j]);
                    new_temp.push_back({-t_cnt, t_cnt});
                    new_templates.push_back(new_temp);
                }
                rep(i,0,t_cnt){
                    // i番目の駅から新しい駅を生成する操作を追加
                    V<pair<int, int>> new_temp = temp;
                    new_temp.push_back({i, t_cnt});
                    new_templates.push_back(new_temp);
                }
            }
            swap(templates, new_templates);
        }
        return templates;
    }

    // templateに実際の駅のリストを当てはめる
    V<Operation> apply_terminals_to_template(V<pair<int, int>>& temp, V<Pos>& terminals){
        V<Operation> operations = {{terminals[0].get_flip(), terminals[0]}};
        for(auto [temp_from, temp_to]: temp){
            auto from = temp_from < 0 ? terminals[-temp_from].get_flip() : terminals[temp_from];
            auto to = temp_to < 0 ? terminals[-temp_to].get_flip() : terminals[temp_to];
            operations.push_back({from, to});
        }
        return operations;
    }

    V<BeamState> create_next_beam(V<BeamState>& beam, int next_terminal_cnt, int beam_size, bool is_last_beam){
        timer.mark_lap();
        priority_queue_rev<BeamState> next_pq;
        auto templates = list_all_possible_templates(next_terminal_cnt);

        for(auto state: beam){
            if(timer.lap_ms() > 200) break;
            // 全マス、次のincomeの増加量を計算
            static int ng_pos[N][N];
            static int ng_cur; ng_cur++;
            int ng_range = 4;
            for(auto& terminal: state.terminals){
                rep(i,-ng_range,ng_range+1)rep(j,-ng_range+abs(i),ng_range-abs(i)+1){
                    Pos pos = terminal + Pos(i, j);
                    if(!pos.valid()) continue;
                    ng_pos[pos.x][pos.y] = ng_cur;
                }
            }
            static V<pair<int, Pos>> new_pos_candidates;
            new_pos_candidates.clear();
            for(auto& [_, new_pos]: board_info.good_cells){
                if(ng_pos[new_pos.x][new_pos.y] == ng_cur) continue;
                int add_income = 0;
                for(auto& terminal: state.terminals)
                    add_income += board_info.connect_reward[terminal.x][terminal.y][new_pos.x][new_pos.y];
                if(add_income == 0){
                    if(is_last_beam && best_state.result.turn > 0) continue;
                    if(!is_last_beam && next_pq.size() >= 0) continue;
                }
                new_pos_candidates.push_back({add_income, new_pos});
            }
            int new_pos_cnt = min<int>(new_pos_candidates.size(), 100);
            nth_element(new_pos_candidates.begin(), new_pos_candidates.begin() + new_pos_cnt, new_pos_candidates.end(), greater<>());
            new_pos_candidates.resize(new_pos_cnt);
            for(auto& [add_income, new_pos]: new_pos_candidates){
                // r側のコストパフォーマンスが良いか判断
                auto is_better_cost_performance = [&](int turn_r, int income_r, int turn_l, int income_l){
                    if(turn_r == turn_l) return income_r > income_l;
                    // r側のターン数を大きくして考える。l側のincomeで差分ターン数との兼ね合いで追いつけるかチェック
                    bool swapped = turn_r < turn_l;
                    if(swapped) swap(turn_r, turn_l), swap(income_r, income_l);
                    float diff_rate = (float)(turn_r - turn_l) * income_l / TerminalCost;
                    float val = diff_rate;
                    // ↓ バグだけどローカルだとこっちの方がスコア出てるので一応残し
                    // float diff_rate = (float)(turn_r - turn_l) * income_l / TerminalCost;
                    // float val = diff_rate > 1 ? pow(diff_rate, 2) / 2 : diff_rate / 2;
                    bool r_big = income_r > income_l * (1 + val / next_terminal_cnt * 2);
                    return swapped ? !r_big : r_big;
                };

                // 枝刈りするか判断
                int ideal_turn = state.turn + max((TerminalCost - state.k) + state.income - 1, 0)/state.income + 1;
                
                if(is_last_beam){
                    if(!is_better_cost_performance(ideal_turn, state.income + add_income, best_state.result.turn, best_state.result.income)) continue;
                }else if(next_pq.size() >= beam_size){
                    if(!is_better_cost_performance(ideal_turn, state.income + add_income, next_pq.top().turn, next_pq.top().income)) continue;
                }

                // 実際に駅を追加して、各テンプレ順のOperationを作ってみて、シミュレーション。よければBeamに入れる
                BeamState next_state;
                next_state.terminals = state.terminals;
                next_state.terminals.push_back(new_pos);

                BeamState best_beam_state{next_state.terminals, 0, T, 0};
                for(auto& temp: templates){
                    // TODO: 最高のtemplate結果だけを入れるようにするべき
                    auto operations = apply_terminals_to_template(temp, next_state.terminals);
                    auto res = simulator.simulate_unsafe(operations);
                    if(res.final_k() <= 0) continue;
                    
                    auto beam_state = BeamState{next_state.terminals, res.income, res.turn, res.k};

                    // if(best_beam_state > beam_state) continue;
                    if(is_better_cost_performance(best_beam_state.turn, best_beam_state.income, beam_state.turn, beam_state.income)) continue;
                    best_beam_state = beam_state;

                    if(is_last_beam && is_better_cost_performance(res.turn, res.income, best_state.result.turn, best_state.result.income)){
                        cerr << "income: " << res.income << ", turn: " << res.turn << ", timer: " << timer.lap_ms() << "ms" << endl;
                        bool can_create_rail = false;
                        rep(bit,0,1<<(next_terminal_cnt-1)){
                            int tmp_bit = bit;
                            for(auto& op : operations){
                                if(op.from.get_abs() != op.to.get_abs()){
                                    op.x_prior = tmp_bit & 1;
                                    tmp_bit >>= 1;
                                }
                            }
                            can_create_rail = simulator.operation_rail_check(operations);
                            if(can_create_rail) break;
                        }

                        if(!can_create_rail) continue;
                        cerr  << "can create rail" << endl;
                        State new_best_state;
                        new_best_state.operations = operations;
                        new_best_state.terminals = next_state.terminals;
                        for(auto& terminal: next_state.terminals) {
                            new_best_state.used[terminal.x][terminal.y] = true;
                            new_best_state.update_terminal_cover(terminal, true);
                            new_best_state.fixed[terminal.x][terminal.y] = true;
                        }
                        for(auto op: operations) new_best_state.flip_rail_used(op);
                        new_best_state.result = res;
                        best_state = new_best_state;
                    }
                }
                if(!is_last_beam && best_beam_state.income > 0){
                    next_pq.push(best_beam_state);
                    if(next_pq.size() > beam_size) next_pq.pop();
                }
            }
        }
        cerr << "simulate cnt: " << simulator.simulate_cnt << endl;
        V<BeamState> next_beam;
        while(!next_pq.empty()){
            next_beam.push_back(next_pq.top());
            next_pq.pop();
        }
        reverse(all(next_beam));
        return next_beam;
    }

    V<BeamState> create_2_terminal_beams(){
        V<BeamState> beams;
        for(auto [income, p1, p2]: board_info.good_pairs){
            int dist = p1.dist(p2);
            if(dist < 5) continue;
            if(TerminalCost * 2 + RailCost * (dist-1) > InitK) continue;
            BeamState state;
            state.terminals = {p1, p2};
            state.income = income;
            state.turn = 1 + dist;
            state.k = InitK - TerminalCost * 2 - RailCost * (dist-1) + income;
            state.cost_performance = (float)state.income / state.turn;
            beams.push_back(state);
        }
        return beams;
    }

    State run(){
        int max_terminal = 5;
        auto beam = create_2_terminal_beams();

        rep(i,2,max_terminal){
            cerr << "[InitialPhase] terminal_cnt: " << i+1 << endl;
            int beam_size = 1000;
            beam = create_next_beam(beam, i+1, beam_size, i+1 == max_terminal);
        }
        cerr << "[initial phase] turn: " << best_state.result.turn << ", income: " << best_state.result.income << ", time: " << timer.elapsed_ms() << "ms" << endl;
        // simulator.output(best_state.operations);
        // cerr << "Score = " << best_state.income << "/" << best_state.turn << endl;
        // cerr << "Score = " << timer.elapsed_ms() << endl;
        // exit(0);
        return best_state;
    }
};

struct Solver{
    void run(){
        board_info.init();

        auto best_state = InitialPhaseStateCreator().run();

        SA sa2;
        sa2.min_turn = min(T, best_state.result.turn + 50);
        best_state = sa2.run(time_limit - timer.elapsed_ms(), best_state);

        // auto init_state = InitialPhaseStateCreator().run();
        // auto best_state = init_state;
        // rep(i,0,10){
        //     SA sa;
        //     sa.min_turn = min(T, init_state.result.turn + 50);
        //     sa.max_turn = min(T, init_state.result.turn + 200);
        //     auto new_state = sa.run(500, init_state);
        //     if(new_state.result.final_k() > best_state.result.final_k())
        //         best_state = new_state;
        // }

        // SA sa;
        // sa.min_turn = min(T, best_state.result.turn + 200);
        // sa.max_turn = T;
        // sa.temp.start_temp = 10000;
        // sa.temp.mid_temp = 2000;
        // sa.temp.end_temp = 1;
        // best_state = sa.run(3000, best_state);

        
        simulator.output(best_state.operations);
        cerr << "Score = " << best_state.result.final_k() << endl;
    }
};


int main(int argc, char *argv[])
{
#if DEBUG
    freopen("tools/in/0058.txt", "r", stdin);
#endif
    input();

    Solver solver;
    solver.run();
    cerr << "elapsed time: " << timer.elapsed_ms() << "ms" << endl;
}