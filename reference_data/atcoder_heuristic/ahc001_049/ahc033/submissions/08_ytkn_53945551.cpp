#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <queue>
#include <deque>
#include <set>
#include <map>
#include <tuple>
#include <cmath>
#include <numeric>
#include <functional>
#include <cassert>
#include <time.h>

#define debug_value(x) cerr << "line" << __LINE__ << ":<" << __func__ << ">:" << #x << "=" << x << endl;
#define debug(x) cerr << "line" << __LINE__ << ":<" << __func__ << ">:" << x << endl;

template<class T> inline bool chmax(T& a, T b) { if (a < b) { a = b; return 1; } return 0; }
template<class T> inline bool chmin(T& a, T b) { if (a > b) { a = b; return 1; } return 0; }

using namespace std;
typedef long long ll;

template<typename T>
vector<vector<T>> vec2d(int n, int m, T v){
    return vector<vector<T>>(n, vector<T>(m, v));
}

template<typename T>
vector<vector<vector<T>>> vec3d(int n, int m, int k, T v){
    return vector<vector<vector<T>>>(n, vector<vector<T>>(m, vector<T>(k, v)));
}

template<typename T>
void print_vector(vector<T> v, char delimiter=' '){
    if(v.empty()) {
        cout << endl;
        return;
    }
    for(int i = 0; i+1 < (int)v.size(); i++) cout << v[i] << delimiter;
    cout << v.back() << endl;
}

using P = pair<int, int>;
using T3 = tuple<int, int, int>;
using T4 = tuple<int, int, int, int>;

const int n = 5;
const P pos_broken(-1, -1);
int no_container = n*n;
const string UDLR = "UDLR";

const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};

namespace input{
    int a[n][n];
    int initial_x[n*n];
    int initial_height[n*n];
    void read(){
        int dummy; cin >> dummy;
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                cin >> a[i][j];
                initial_x[a[i][j]] = i;
                initial_height[a[i][j]] = j;
            }
        }
    }
}

bool is_in_field(int x, int y){
    return 0 <= x && x < n && 0 <= y && y < n;
}

int target_row(int x){
    return x/n;
}

struct Crane{
    int idx;
    P pos;
    int picking_container;

    Crane(): idx(0), pos(-1, -1), picking_container(no_container) {}

    Crane(int idx, P pos, int picking_container): idx(idx), pos(pos), picking_container(picking_container) {}

    bool is_large(){
        return idx == 0;
    }

    void explode(){
        assert(picking_container == no_container);
        pos = pos_broken;
    }

    bool is_broken(){
        return pos == pos_broken;
    }

    bool is_picking(){
        return picking_container != no_container;
    }

    void pick(int container){
        assert(container != no_container);
        assert(picking_container == no_container);
        picking_container = container;   
    }

    int put(){
        assert(picking_container != no_container);
        int ret = picking_container;
        picking_container = no_container;
        return ret;
    }

    void move_pos(int dx, int dy){
        auto [x, y] = pos;
        pos = P(x+dx, y+dy);
        assert(is_in_field(pos.first, pos.second));
    }
};

struct State{
    int turn;
    int next_conteiner[n];
    int picking_container[n];
    int containers[n][n];
    Crane cranes[n];
    int delivered[n];

    State(): turn(0){
        for(int i = 0; i < n; i++) {
            delivered[i] = 0;
            cranes[i] = Crane(i, P(i, 0), no_container);
            next_conteiner[i] = 1;
            containers[i][0] = input::a[i][0];
            for(int j = 1; j < n; j++){
                containers[i][j] = no_container;
            }
        }
    }

    void pick(int i_crane){
        auto [x, y] = cranes[i_crane].pos;
        // 拾う
        int container = containers[x][y];
        cranes[i_crane].pick(container);
        containers[x][y] = no_container;
    }

    void put(int i_crane) {
        auto [x, y] = cranes[i_crane].pos;
        assert(containers[x][y] == no_container);
        containers[x][y] = cranes[i_crane].put();
    }

    void apply(string commands){
        turn++;
        P prev_pos[n];
        for(int i_crane = 0; i_crane < n; i_crane++){
            auto &crane = cranes[i_crane];
            prev_pos[i_crane] = crane.pos;
            char c = commands[i_crane];
            // 破壊されていて'.'以外のコマンドはNG
            assert(!crane.is_broken() || c == '.');
            if(c == 'P'){
                pick(i_crane);
            }else if(c == 'Q'){
                put(i_crane);
            }else if(c == 'U'){
                crane.move_pos(-1, 0);
            }else if(c == 'D'){
                crane.move_pos(1, 0);
            }else if(c == 'L'){
                crane.move_pos(0, -1);
            }else if(c == 'R'){
                crane.move_pos(0, 1);
            }else if(c == 'B'){
                crane.explode();
            }else{
                assert(c == '.');
                // 何もしない
            }
        }
#if DEBUG
        // 位置の交換をしていないか
        for(int i_crane = 0; i_crane < n; i_crane++){
            auto i_pos = cranes[i_crane].pos;
            auto i_prev_pos = prev_pos[i_crane];
            if(i_pos == pos_broken) continue;
            for(int j_crane = i_crane+1; j_crane < n; j_crane++){
                auto j_pos = cranes[j_crane].pos;
                auto j_prev_pos = prev_pos[j_crane];
                if(j_pos == pos_broken) continue;
                assert(i_pos != j_prev_pos || j_pos != i_prev_pos);
            }
        }
        check_constraints();
#endif

        // 一番左に置けるなら置く
        for(int i = 0; i < n; i++){
            if(next_conteiner[i] == n) continue;
            if(containers[i][0] != no_container) continue;
            bool ok = true;
            for(int i_crane = 0; i_crane < n; i_crane++){
                if(cranes[i_crane].pos == P(i, 0) && cranes[i_crane].is_picking()){
                    ok = false;
                    break;
                }
            }
            if(ok){
                containers[i][0] = input::a[i][next_conteiner[i]];
                next_conteiner[i]++;
                // cerr << containers[i][0] << endl; 
            }
        }
        // 一番右から払い出す
        for(int i = 0; i < n; i++){
            if(containers[i][n-1] != no_container){
                delivered[i]++;
                containers[i][n-1] = no_container;
            }
        }
    }

    /**
     * 配置制約を違反していないか
     */
    void check_constraints(){
        for(int i = 0; i < n; i++){
            if(cranes[i].is_broken()) continue;
            auto [ix, iy] = cranes[i].pos;
            if(containers[ix][iy] != no_container && !cranes[i].is_large() && cranes[i].picking_container != no_container){
                // 小さいクレーンが持ち上げた状態でコンテナが置かれたマスに入った
                assert(false);
            }
            for(int j = i+1; j < n; j++){
                if(cranes[j].is_broken()) continue;
                auto [jx, jy] = cranes[j].pos;
                // クレーンが同じマスにいる
                assert(ix != jx || iy != jy);
            }
        }
    }
};

/**
 * (x, y)にcontainerを置く
 */
struct PutOperation{
    int x, y, container;
    PutOperation(int x, int y, int container): x(x), y(y), container(container) {}
};

// (i_crane, op, command)
using NextMove = tuple<int, PutOperation, string>; 

char prev_move[1000][n][n][n*n+1];
int bfs[1000][n][n][n*n+1];
int latest_put[n][n][n*n];
int bfs_start_turn = 0;
int bfs_cnt = 0;

string recover(int turn, int x, int y, int container){
    string command;
    while(turn > bfs_start_turn){
        // debug_value(turn)
        assert(bfs[turn][x][y][container] == bfs_cnt);
        char c = prev_move[turn][x][y][container];
        // cerr << turn << ' ' << x << ' ' << y << ' ' << container << ' ' << c << endl;
        command += c;
        if(c == 'U') x++;
        if(c == 'D') x--;
        if(c == 'L') y++;
        if(c == 'R') y--;
        if(c == 'P') container = no_container;
        assert(c != 'B' && c != 'Q');
        // .は何もしない
        turn--;
    }
    reverse(command.begin(), command.end());
    return command;
}

namespace weights{
    double dist = 1.6263121635804418;
    // double dist = 2.0;
    double height = 0.4278048221059412; 
    double inv = 0.3803966036246753; 
    // double inv = 1.0; 
    double inv_height = 0.6846859681510498; 
    // double inv_height = 1.0; 
    double turn = 1.0;
    double goal = 1.9478328559381723;
}

// namespace weights{
//     double dist = 1.5885387137454425;
//     // double dist = 2.0;
//     double height = 0.09870490201280702; 
//     double inv = 0.09870490201280702; 
//     // double inv = 1.0; 
//     double inv_height = 1.1364335830261274; 
//     // double inv_height = 1.0; 
//     double turn = 1.0;
//     double goal = 2.5101665112942446;
// }

vector<NextMove> next_moves;

struct Solution{
    State state;
    string ans[n];
    int max_small_crane_cross[n][n];
    int last_put[n][n];
    P put_pos[n*n];
    int last_pick[n*n];
    P crane_pos[n];
    Solution(){
        // state = State;
        for(int i = 0; i < n; i++) {
            ans[i] = "";
            crane_pos[i] = {i, 0};
            for(int j = 0; j < n; j++){
                max_small_crane_cross[i][j] = -1;
                last_put[i][j] = -1;
                int x = input::a[i][j];
                put_pos[x] = {i, 0};
            }
        }
        for(int x = 0; x < n*n; x++){
            last_pick[x] = -1;
        }
    }

    bool is_finished(){
        for(int x = 0; x < n*n; x++){
            if(put_pos[x].second != n-1) return false;
        } 
        return true;
    }

    bool is_all_broken(){
        for(int i = 0; i < n; i++){
            if(!is_broken(i)) return false;
        }
        return true;
    }
    
    void add_moves(int i_crane, PutOperation op, string commands){
        bool is_picking = false;
        for(int i = 0; i < (int)commands.size(); i++){
            int turn = ans[i_crane].size()+i;
            char c = commands[i];
            if(c == 'P') {
                // NOTE: + commands.size()はもともとバグだが、これがあったほうがスコアがいい？ 
                last_pick[op.container] = turn + commands.size();
                is_picking = true;
            }
            if(c == 'Q') is_picking = false;
            if(c == 'U') crane_pos[i_crane].first--;
            if(c == 'D') crane_pos[i_crane].first++;
            if(c == 'L') crane_pos[i_crane].second--;
            if(c == 'R') crane_pos[i_crane].second++;
            if(is_picking && i_crane >= 1) {
                auto [x, y] = crane_pos[i_crane];
                chmax(max_small_crane_cross[x][y], turn);
            }
        }
        ans[i_crane] += commands;
        if(op.container != no_container){
            put_pos[op.container] = {op.x, op.y};
            last_put[op.x][op.y] = ans[i_crane].size();
        }
    }

    bool is_broken(int i_crane) const {
        return !ans[i_crane].empty() && ans[i_crane].back() == 'B';
    }

    /**
     * 一番進んでないクレーンを選ぶ
     */
    int next_target() const {
        int min_turn = 100000;
        int i_crane = -1;
        for(int i = 0; i < n; i++){
            if(is_broken(i)) continue;
            if(chmin(min_turn, (int)ans[i].size())){
                i_crane = i;
            }
        }

        return i_crane;
    }

    int eval(){
        int dist_sum = 0;
        int inv_sum = 0;
        int inv_height_sum = 0;
        int height_sum = 0;
        int goal = 0;
        int left_cnt[n];
        for(int i = 0; i < n; i++) left_cnt[i] = 0;
        for(int val = 0; val < n*n; val++){
            auto [x, y] = put_pos[val];
            if(y == 0) left_cnt[x]++;
            if(y == n-1) goal++;
        }
        for(int i = 0; i < n; i++){
            int dists[n];
            int heights[n];
            for(int j = 0; j < n; j++){
                int val = i*n+j;
                auto [x, y] = put_pos[val];
                heights[j] = 0;
                if(y == 0 && x == input::initial_x[val]){
                    int h = input::initial_height[val];
                    int diff = 5-left_cnt[x];
                    assert(h-diff >= 0);
                    chmax(heights[j], h-diff);
                }
                // debug_value(heights[j])
                dists[j] = abs(i-x) + n-1-y;
                dist_sum += dists[j];
                height_sum += heights[j];
                for(int k = 0; k < j; k++){
                    if(dists[k] > dists[j]) inv_sum++;
                    if(heights[k] > heights[j]) inv_height_sum++;
                }
            }
        }
        int turn_sum = 0;
        for(int i_crane = 0; i_crane < n; i_crane++){
            turn_sum += ans[i_crane].size();
        }
        int large_broken = is_broken(0);
        int small_broken = 0;
        for(int i_crane = 1; i_crane < n; i_crane++){
            if(is_broken(i_crane)) small_broken++;
        }

        return dist_sum*weights::dist+ 
                inv_sum*weights::inv + 
                inv_height_sum*weights::inv_height + 
                turn_sum*weights::turn + 
                height_sum*weights::height +
                large_broken*10000 + 
                small_broken*100 -
                goal*weights::goal;
    } 


    int eval_if_acted(int i_crane, PutOperation op, string commands){
        int dist_sum = 0;
        int inv_sum = 0;
        int inv_height_sum = 0;
        int goal = 0;
        int height_sum = 0;
        int left_cnt[n];
        P prev_pos;
        if(op.container != no_container){
            prev_pos = put_pos[op.container];
            put_pos[op.container] = {op.x, op.y};
        }
        for(int i = 0; i < n; i++) left_cnt[i] = 0;
        for(int val = 0; val < n*n; val++){
            auto [x, y] = put_pos[val];
            if(y == 0) left_cnt[x]++;
            if(y == n-1) goal++;
        }
        for(int i = 0; i < n; i++){
            int dists[n];
            int heights[n];
            for(int j = 0; j < n; j++){
                int val = i*n+j;
                auto [x, y] = put_pos[val];
                heights[j] = 0;
                if(y == 0 && x == input::initial_x[val]){
                    int h = input::initial_height[val];
                    int diff = 5-left_cnt[x];
                    assert(h-diff >= 0);
                    chmax(heights[j], h-diff);
                }
                // debug_value(heights[j])
                dists[j] = abs(i-x) + n-1-y;
                dist_sum += dists[j];
                height_sum += heights[j];
                for(int k = 0; k < j; k++){
                    if(dists[k] > dists[j]) inv_sum++;
                    if(heights[k] > heights[j]) inv_height_sum++;
                }
            }
        }
        int turn_sum = commands.size();
        for(int i_crane = 0; i_crane < n; i_crane++){
            turn_sum += ans[i_crane].size();
        }
        int large_broken = is_broken(0);
        int small_broken = 0;
        for(int i_crane = 1; i_crane < n; i_crane++){
            if(is_broken(i_crane)) small_broken++;
        }
        if(commands.back() == 'B'){
            if(i_crane == 0) large_broken = 1;
            else small_broken++;
        }

        if(op.container != no_container){
            put_pos[op.container] = prev_pos;
        }

        return dist_sum*weights::dist+ 
                height_sum*weights::height +
                inv_sum*weights::inv + 
                inv_height_sum*weights::inv_height + 
                turn_sum*weights::turn + 
                large_broken*10000 + 
                small_broken*100 -
                goal*weights::goal;
    }

    /**
     * 予定が決まっているやつは動かし、そうでないやつは爆破
     */
    string make_command(int turn) const {
        string command;
        for(int j_crane = 0; j_crane < n; j_crane++){
            if((int)ans[j_crane].size() > turn){
                command += ans[j_crane][turn];
            }else if((int)ans[j_crane].size() == turn && !is_broken(j_crane)){
                command += 'B';
            }else{
                command += '.';
            }
        }
        return command;
    }

    /**
     * 一つ掴んで動かす
     */
    void make_next_moves(int max_turn, int x_ord[5], int y_ord[5]) {
        // i_craneを動かす
        int i_crane = next_target();
        // cerr << "target crane: " << i_crane << endl;

        // (turn, x, y, container)
        static vector<T4> put_terminal, put_middle;

        put_terminal.clear();
        put_middle.clear();

        int start_turn = ans[i_crane].size();
        bfs_cnt++;
        bfs_start_turn = start_turn;
        // 未来の状態の列をキャッシュしておく
        while(state.turn < start_turn){
            state.apply(make_command(state.turn));
        }
        State current_state = state;;

        // 初期化 
        auto [x, y] = current_state.cranes[i_crane].pos;
        // cerr << "start pos: " << x << ' ' << y << endl;
        bfs[start_turn][x][y][no_container] = bfs_cnt;
        for(int turn = start_turn; turn < start_turn+max_turn; turn++){
            auto next_state = current_state;
            next_state.apply(make_command(turn));
            // 次の状態で埋まっているか
            bool crane_exists_next[n][n];
            bool container_exists_next[n][n];

            for(int x = 0; x < n; x++){
                for(int y = 0; y < n; y++){
                    container_exists_next[x][y] = next_state.containers[x][y] != no_container;
                    crane_exists_next[x][y] = false;
                }
            }

            for(int j_crane = 0; j_crane < n; j_crane++){
                auto &crane = next_state.cranes[j_crane];
                if(crane.is_broken()) continue;
                auto [x, y] = crane.pos;
                crane_exists_next[x][y] = true;
            }

            for(int i = 0; i < n; i++){
                int x = x_ord[i];
                for(int j = 0; j < n; j++){
                    int y = y_ord[j];
                    // 交差チェック & はみ出るかチェック
                    bool check_cross_or_out[4];
                    for(int d = 0; d < 4; d++){
                        check_cross_or_out[d] = true;
                        int xx = x+dx[d], yy = y+dy[d];
                        if(!is_in_field(xx, yy)) {
                            check_cross_or_out[d] = false;
                            continue;
                        }
                        for(int j_crane = 0; j_crane < n; j_crane++){
                            // 今の位置
                            auto [x_cur, y_cur] = current_state.cranes[j_crane].pos;
                            // 次の位置
                            auto [x_nx, y_nx] = next_state.cranes[j_crane].pos;
                            // 交換している
                            if(x_cur == xx && y_cur == yy && x_nx == x && y_nx == y){
                                check_cross_or_out[d] = false;
                            }
                        }
                    }

                    // TODO: 適当に飛ばしたい
                    // 持っているパターン
                    for(int container = 0; container < n*n; container++){
                        if(bfs[turn][x][y][container] != bfs_cnt) continue;
                        // 今持っているコンテナがあることになってた
                        if(next_state.containers[x][y] == container){
                            // 一番左は次のが来る
                            if(y != 0){
                                container_exists_next[x][y] = false;
                            }
                            // TODO: 一番左でも最後のやつなら平気
                        }

                        // 次のターンで空いている
                        if(!container_exists_next[x][y] && !crane_exists_next[x][y]){
                            bool already_put = latest_put[x][y][container] == bfs_cnt;
                            bool is_crossed = max_small_crane_cross[x][y] >= turn;
                            bool is_put_future = last_put[x][y] >= turn;
                            // 置く
                            if(!already_put && !is_crossed && !is_put_future){
                                if(y == n-1){
                                    // 右端は目標の位置と同じでないとだめ
                                    bool can_put = container/n == x && (int)current_state.delivered[x] == (container%n);
                                    if(can_put){
                                        put_terminal.push_back({turn, x, y, container});
                                        latest_put[x][y][container] = bfs_cnt;
                                        // cerr << "put " << turn << ':' << container << ' ' << x << ' ' << y << endl;
                                    }
                                }else{
                                    // こっちは置ける
                                    auto [prev_x, prev_y] = put_pos[container];
                                    // int target_x = target_row(container);
                                    // int prev_dist = abs(target_x - prev_x) + n-1-prev_y;
                                    // int dist = abs(target_x - x) + n-1-y;
                                    // if(dist < prev_dist && y >= 1){
                                    if(y > prev_y && y >= 1){
                                        put_middle.push_back({turn, x, y, container});
                                        latest_put[x][y][container] = bfs_cnt;
                                        // cerr << turn << ':' << container << ' ' << x << ' ' << y << endl;
                                    }
                                }
                            }
                            // とどまる
                            bfs[turn+1][x][y][container] = bfs_cnt;
                            prev_move[turn+1][x][y][container] = '.';
                        }

                        for(int d = 0; d < 4; d++){
                            // 動けるか
                            if(!check_cross_or_out[d]) continue;
                            int xx = x+dx[d], yy = y+dy[d];
                            if(crane_exists_next[xx][yy]) continue;
                            // 持った状態でコンテナと干渉しないか
                            if(i_crane != 0 && container_exists_next[xx][yy]) continue;
                            // 動けた
                            bfs[turn+1][xx][yy][container] = bfs_cnt;
                            prev_move[turn+1][xx][yy][container] = UDLR[d];
                            // if(x == 0 && y == 2) cerr << UDLR[d] << endl;
                        }

                        if(next_state.containers[x][y] == container){
                            container_exists_next[x][y] = true;
                        }
                    }

                    // 何も持っていないパターン
                    if(bfs[turn][x][y][no_container] != bfs_cnt) continue;

                    if(!crane_exists_next[x][y]){
                        // 掴む
                        // コンテナがあり、そのコンテナが以降で動かされる予定がない
                        int current_container = current_state.containers[x][y]; 
                        if(current_container != no_container && last_pick[current_container] < turn){
                            int container = current_state.containers[x][y];
                            bfs[turn+1][x][y][container] = bfs_cnt;
                            prev_move[turn+1][x][y][container] = 'P';
                            // cerr << "pick " << container << ": turn " << turn << " at " << x << ',' << y << endl;
                        }
                        // とどまる
                        bfs[turn+1][x][y][no_container] = bfs_cnt;
                        prev_move[turn+1][x][y][no_container] = '.';
                    }

                    // 動く
                    for(int d = 0; d < 4; d++){
                        // 動けるか
                        if(!check_cross_or_out[d]) continue;
                        int xx = x+dx[d], yy = y+dy[d];
                        if(crane_exists_next[xx][yy]) continue;
                        // NOTE: コンテナとの干渉チェックは不要
                        // 動けた
                        bfs[turn+1][xx][yy][no_container] = bfs_cnt;
                        prev_move[turn+1][xx][yy][no_container] = UDLR[d];
                        // if(x == 0 && y == 2) cerr << UDLR[d] << endl;
                    }
                }
            }
            current_state = next_state;
        }

        string command;
        next_moves.clear();

        // 終着点に置いた
        for(auto [turn, x, y, container]: put_terminal){
            command = recover(turn, x, y, container);
            command += 'Q';
            auto op = PutOperation(x, y, container);
            next_moves.push_back({i_crane, op, command});
        }
        // 途中に置いた
        for(auto [turn, x, y, container]: put_middle){
            command = recover(turn, x, y, container);
            command += 'Q';
            auto op = PutOperation(x, y, container);
            next_moves.push_back({i_crane, op, command});
        }
        // 候補なしパターン
        if(next_moves.empty()){
            // 運ぶことはできなかったがある程度生存できた
            bool ok = false;
            for(int turn = start_turn + 10; turn >= start_turn + 5 && !ok; turn--){
                for(int x = 0; x < n && !ok; x++){
                    for(int y = 0; y < n && !ok; y++){
                        if(bfs[turn][x][y][no_container] == bfs_cnt){
                            ok = true;
                            command = recover(turn, x, y, no_container);
                        }
                    }
                }
            }
            // 無理だったので爆破 
            if(!ok) command = "B";
            PutOperation op(-1, -1, no_container);
            next_moves.push_back({i_crane, op, command});
        }

        // return ans;
    }

    int get_hash(){
        int i_crane = [&](){
            for(int i = 0; i < n; i++) {
                if(!is_broken(i)) return i;
            }
        }();
        auto [x, y] = crane_pos[i_crane];
        int ans = x*n+y;
        for(int container = 0; container < n*n; container++){
            if(put_pos[container].second == n-1){
                ans ^= container;
            }
        }
        ans %= (n*n);
        return ans; 
    }

    void output(){
        int max_len = 0;
        for(int i = 0; i < n; i++) chmax(max_len, (int)ans[i].size()); 
        for(int i = 0; i < n; i++){
            cout << ans[i];
            if(ans[i].back() != 'B'&& (int)ans[i].size() < max_len) cout << 'B';
            cout << endl;
        }
        cerr << "len:" << max_len << endl;
    }
};

clock_t start;

double elapsed_time(){
    clock_t cur = clock();
    return (double)(cur-start)/CLOCKS_PER_SEC;
}

vector<tuple<int, int, PutOperation, string>> moves;

void solve(){
    int beam_width = 1600;
    int max_by_hash = beam_width/5;
    vector<string> commands(n);
    vector<Solution> finished;
    vector<Solution> beam;

    int required_finish = 100;
    bool is_limit_near = false;

    moves.reserve(100000);
    
    int iter = 0;
    beam.push_back(Solution());
    while(finished.size() < required_finish && iter < 200){
        if(beam.empty()) break;
        // cerr << "iter: " << iter << " beams: " << beam.size() << ' ' << "finished: " << finished.size() << endl;
        // vector<P> by_score;
        // (i_beam, i_crane, op, command)
        // vector<tuple<int, int, PutOperation, string>> moves;
        moves.clear();
        priority_queue<P, vector<P>, greater<P>> by_score;
        int i_beam = 0;
        for(auto &sol: beam){
            int ord0[5] = {0, 1, 2, 3, 4};
            int ord1[5] = {4, 3, 2, 1, 0};
            sol.make_next_moves(15, i_beam%2 == 0 ? ord0 : ord1, i_beam%4 < 2 ? ord0 : ord1);
            for(auto [i_crane, op, command]: next_moves){
                // cerr << "exec: " << command << endl;
                int new_score = sol.eval_if_acted(i_crane, op, command);
                by_score.push({new_score, moves.size()});
                moves.push_back({i_beam, i_crane, op, command});
            }
            i_beam++;
        }
        // cerr << "best score: " << by_score.top().first << " size:" << by_score.size() << endl;

        if(!is_limit_near && elapsed_time() >= 2.8){
            is_limit_near = true;
            beam_width /= 50;
            required_finish = 1;
            cerr << "shrink at:" << iter << ' ' << elapsed_time() << "[sec]" << endl;
        }

        vector<Solution> next_beam;

        int hash_count[n*n];
        for(int h = 0; h < n*n; h++) hash_count[h] = 0;
        while(!by_score.empty() && (int)next_beam.size() < beam_width){
            int idx = by_score.top().second;
            by_score.pop();
            auto [i_beam, i_crane, op, commands] = moves[idx];
            auto new_sol = beam[i_beam];
            new_sol.add_moves(i_crane, op, commands);
            if(new_sol.is_finished()){
                finished.push_back(new_sol);
            }else if(!new_sol.is_all_broken()){
                int h = new_sol.get_hash();
                // if(hash_count[h] >= max_by_hash) continue;
                next_beam.push_back(new_sol);
                hash_count[h]++;
                // cerr << by_score[i].first << ' ';
            }
        }
        // cerr << endl;

        beam = next_beam;
        iter++;
    }
    debug_value(finished.size());
    if(finished.empty()){
        auto sol = beam[0];
        sol.output();
        return;
    }
    
    auto sol = [&](){
        int mn = 1e9;
        int min_idx = -1;
        for(int i = 0; i < (int)finished.size(); i++){
            auto &sol = finished[i];
            int mx = 0;
            for(int i_crane = 0; i_crane < n; i_crane++){
                chmax(mx, (int)sol.ans[i_crane].size());
            }
            if(chmin(mn, mx)) min_idx = i;
        }
        return finished[min_idx];
    }();
    sol.output(); 
    cerr << "iter:" << iter << endl;
}

int main(int argc, char* argv[]){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout << setprecision(10) << fixed;
    start = clock();
    if(argc == 7){
        weights::dist = atof(argv[1]);
        weights::height = atof(argv[2]);
        weights::inv = atof(argv[3]);
        weights::inv_height = atof(argv[4]);
        weights::turn = atof(argv[5]);
        weights::goal = atof(argv[6]);
    }
    input::read();
    solve();
    clock_t end = clock();
    cerr << (double)(end-start)/CLOCKS_PER_SEC << "[sec]" << endl;
}