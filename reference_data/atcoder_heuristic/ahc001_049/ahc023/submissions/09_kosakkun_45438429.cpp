#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <chrono>
#include <random>
#include <bitset>
#include <algorithm>
#include <cmath>
#include <map>
#include <functional>
#include <cassert>
#include <cstdint>

constexpr int MAX_GRID_H = 20;
constexpr int MAX_GRID_W = 20;
constexpr int MAX_TURN = 100 + 1;

using turn_bit = std::array<std::bitset<MAX_GRID_W>, MAX_GRID_H>;
using cube_bit = std::array<turn_bit, MAX_TURN>;

class timer
{
private:
    std::chrono::high_resolution_clock::time_point start_point;

public:
    void start() {
        start_point = std::chrono::high_resolution_clock::now();
    }
    double get_time() const {
        auto end_point = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end_point - start_point).count();
    }
};

struct position
{
    int x, y;
    position(){}
    position(const int _x, const int _y): x(_x), y(_y){}
    bool operator < (const position& rhs) const {
        return (x == rhs.x) ? (y < rhs.y) : (x < rhs.x);
    }
    bool operator == (const position& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator != (const position& rhs) const {
        return x != rhs.x || y != rhs.y;
    }
};

struct input_data
{
    int turn_num;
    int grid_height;
    int grid_width;
    int crop_num;
    position entrance_pos;
    std::vector<std::pair<int, int>> crops;
    std::vector<std::vector<std::vector<position>>> G;
    std::array<std::array<int, MAX_TURN>, MAX_TURN> span_num;
};

struct solver_state
{
    int score;
    cube_bit harvest_plan;
    std::array<std::array<int, MAX_TURN>, MAX_TURN> exist_space;
    std::array<std::vector<position>, MAX_TURN> extend_pos;
    std::array<std::vector<position>, MAX_TURN> shrink_pos;
    std::array<int, MAX_TURN> harvest_score;
    std::array<std::array<int, MAX_GRID_W>, MAX_GRID_H> last_trial;
    std::bitset<MAX_TURN> dirty_shrink_pos;
};

struct answer_data
{
    int k,i,j,s;
    answer_data() {}
    answer_data(const int _k, const int _i, const int _j, const int _s):
        k(_k), i(_i), j(_j), s(_s) {}
};

using neighbor_f = std::function<bool(solver_state&, const input_data&, const int, const double)>;

struct neighbor
{
    neighbor_f func;
    double temp_start;
    double temp_end;
    double call_probability;
    neighbor(const neighbor_f& f, const double ts, const double te, const double cp):
        func(f), temp_start(ts), temp_end(te), call_probability(cp) {}
};

struct solver_parameter
{
    double time_limit;
    std::vector<neighbor> n;
};

bool accept(const double diff, const double temperature)
{
    static std::random_device seed_gen;
    static std::mt19937 engine(seed_gen());

    if (diff >= 0) {
        return true;
    }

    const double P = std::exp(diff / temperature) * 4294967295.0;
    return engine() < (uint32_t)P;
}

/* ------- 8< ------- 8< ------- 8< ------- 8< ------- 8< ------- 8< -------- */

int get_harvest_score(const input_data& input, const solver_state& state, const int turn)
{
    int score = 0;
    int space_left = 0;
    for (int S = 1; S < turn; S++) {
        const int& crop  = input.span_num[S][turn];
        const int& space = state.exist_space[S][turn];
        space_left += space;
        const int place_num = std::min(crop, space_left);
        score += place_num * (turn - S + 1);
        space_left -= place_num;
    }
    return score;
}

void update_extend_pos(solver_state& state, const input_data& input, const int turn,
    const int update_pos_idx, const bool shrink)
{
    auto& extend_pos = state.extend_pos[turn];

    if (shrink) {
        const auto shr_pos = state.shrink_pos[turn][update_pos_idx];
        extend_pos.push_back(shr_pos);

        for (const auto& [dx1, dy1]: input.G[shr_pos.x][shr_pos.y]) {
            const int nx1 = shr_pos.x + dx1;
            const int ny1 = shr_pos.y + dy1;
            const position pos(nx1, ny1);
            for (int i = 0; i < (int)extend_pos.size(); i++) {
                if (extend_pos[i] != pos) {
                    continue;
                }
                bool erase = true;
                for (const auto& [dx2, dy2]: input.G[pos.x][pos.y]) {
                    const int nx2 = pos.x + dx2;
                    const int ny2 = pos.y + dy2;
                    if (state.harvest_plan[turn][nx2][ny2]) {
                        erase = false;
                        break;
                    }
                }
                if (erase) {
                    std::swap(extend_pos[i], extend_pos.back());
                    extend_pos.pop_back();
                }
                break;
            }
        }
    }
    else {
        const auto ext_pos = extend_pos[update_pos_idx];
        std::swap(extend_pos[update_pos_idx], extend_pos.back());
        extend_pos.pop_back();

        for (const auto& [dx, dy]: input.G[ext_pos.x][ext_pos.y]) {
            const int nx = ext_pos.x + dx;
            const int ny = ext_pos.y + dy;
            const position pos(nx, ny);
            bool flag = false;
            for (int i = 0; i < (int)extend_pos.size(); i++) {
                if (extend_pos[i] == pos) {
                    flag = true;
                    break;
                }
            }
            if (!flag && !state.harvest_plan[turn][pos.x][pos.y]) {
                extend_pos.push_back(pos);
            }
        }

        if (state.extend_pos[turn].size() == 0) {
            if (!state.harvest_plan[turn][input.entrance_pos.x][input.entrance_pos.y]) {
                state.extend_pos[turn].push_back(input.entrance_pos);
            }
        }
    }
}

int  ord[MAX_GRID_W * MAX_GRID_H + 10];
int  low[MAX_GRID_W * MAX_GRID_H + 10];
int  aps[MAX_GRID_W * MAX_GRID_H + 10];
int used[MAX_GRID_W * MAX_GRID_H + 10];
int used_merker;

int lowlink_dfs(
    const input_data& input, solver_state& state, const int turn,
    const position& pos, int count)
{
    const int cur_idx = pos.x * MAX_GRID_W + pos.y;

    if (used[cur_idx] == used_merker) {
        return count;
    } else {
        used[cur_idx] = used_merker;
        ord[cur_idx] = count;
        low[cur_idx] = count;
        count++;
    }

    for (const auto& [dx, dy]: input.G[pos.x][pos.y]) {
        const int nx = pos.x + dx;
        const int ny = pos.y + dy;
        if (state.harvest_plan[turn][nx][ny]) {
            const int nxt_idx = nx * MAX_GRID_W + ny;
            if (used[nxt_idx] == used_merker) {
                low[cur_idx] = std::min(low[cur_idx], ord[nxt_idx]);
            } else {
                count = lowlink_dfs(input, state, turn, position(nx, ny), count);
                low[cur_idx] = std::min(low[cur_idx], low[nxt_idx]);
                if (ord[cur_idx] <= low[nxt_idx]) {
                    aps[cur_idx] = used_merker;
                }
            }
        }
    }

    if (aps[cur_idx] != used_merker) {
        state.shrink_pos[turn].push_back(pos);
    }

    return count;
}

void update_shrink_pos(solver_state& state, const input_data& input, const int turn)
{
    state.shrink_pos[turn].clear();
    if (!state.harvest_plan[turn][input.entrance_pos.x][input.entrance_pos.y]) {
        return;
    }

    static int count = 0; count += 1000;
    used_merker = count;
    lowlink_dfs(input, state, turn, input.entrance_pos, count);
}

bool try_extend_harvest_erea(solver_state& state, const input_data& input,
    const int turn, const double temperature)
{
    static std::random_device seed_gen;
    static std::mt19937 engine(seed_gen());
    static int count = 1;

    const auto& candidate_pos = state.extend_pos[turn];
    if (candidate_pos.size() == 0) {
        return false;
    }

    int update_pos_idx = -1;
    int min_trial = 1e9;
    for (int i = 0; i < (int)candidate_pos.size(); i++) {
        const auto& [x, y] = candidate_pos[i];
        if (state.last_trial[x][y] < min_trial) {
            min_trial = state.last_trial[x][y];
            update_pos_idx = i;
        }
    }

    const auto& pos = candidate_pos[update_pos_idx];
    state.last_trial[pos.x][pos.y] = count++;

    int old_S, old_D;
    for (old_S = turn; !state.harvest_plan[old_S - 1][pos.x][pos.y]; old_S--);
    for (old_D = turn; !state.harvest_plan[old_D][pos.x][pos.y]; old_D++);

    int score_diff = 0;
    score_diff -= state.harvest_score[turn];
    score_diff -= state.harvest_score[old_D];
    state.exist_space[old_S][old_D]--;
    state.exist_space[old_S][turn]++;
    state.exist_space[turn + 1][old_D]++;
    const int hs_1 = get_harvest_score(input, state, turn);
    const int hs_2 = get_harvest_score(input, state, old_D);
    score_diff += hs_1 + hs_2;

    if (accept(score_diff, temperature)) {
        state.score += score_diff;
        state.harvest_plan[turn][pos.x][pos.y] = true;
        state.harvest_score[turn] = hs_1;
        state.harvest_score[old_D] = hs_2;
        state.dirty_shrink_pos[turn] = true;
        update_extend_pos(state, input, turn, update_pos_idx, false);
        return true;
    }

    state.exist_space[old_S][old_D]++;
    state.exist_space[old_S][turn]--;
    state.exist_space[turn + 1][old_D]--;

    return false;
}

bool try_shrink_harvest_erea(solver_state& state, const input_data& input,
    const int turn, const double temperature)
{
    static std::random_device seed_gen;
    static std::mt19937 engine(seed_gen());

    if (state.dirty_shrink_pos[turn]) {
        update_shrink_pos(state, input, turn);
        state.dirty_shrink_pos[turn] = false;
    }

    const auto& candidate_pos = state.shrink_pos[turn];
    if (candidate_pos.size() == 0) {
        return false;
    }

    const int update_pos_idx = engine() % candidate_pos.size();
    const auto& pos = candidate_pos[update_pos_idx];

    int new_S, new_D;
    state.harvest_plan[turn][pos.x][pos.y] = false;
    for (new_S = turn; !state.harvest_plan[new_S - 1][pos.x][pos.y]; new_S--);
    for (new_D = turn; !state.harvest_plan[new_D][pos.x][pos.y]; new_D++);

    int score_diff = 0;
    score_diff -= state.harvest_score[turn];
    score_diff -= state.harvest_score[new_D];
    state.exist_space[new_S][new_D]++;
    state.exist_space[new_S][turn]--;
    state.exist_space[turn + 1][new_D]--;
    const int hs_1 = get_harvest_score(input, state, turn);
    const int hs_2 = get_harvest_score(input, state, new_D);
    score_diff += hs_1 + hs_2;

    if (accept(score_diff, temperature)) {
        state.score += score_diff;
        state.harvest_score[turn] = hs_1;
        state.harvest_score[new_D] = hs_2;
        state.dirty_shrink_pos[turn] = true;
        update_extend_pos(state, input, turn, update_pos_idx, true);
        return true;
    }

    state.harvest_plan[turn][pos.x][pos.y] = true;
    state.exist_space[new_S][new_D]--;
    state.exist_space[new_S][turn]++;
    state.exist_space[turn + 1][new_D]++;

    return false;
}

void simulated_annealing(solver_state& best_state, const input_data& input,
    const solver_parameter& param)
{
    static std::random_device seed_gen;
    static std::mt19937 engine(seed_gen());
    static timer l_timer;

    std::vector<double> prob;
    for (const auto& n: param.n) {
        prob.push_back(n.call_probability);
    }

    std::vector<double> turn_weight;
    for (int t = 0; t < input.turn_num; t++) {
        double span_sum = 0;
        for (int S = 1; S < t; S++) {
            span_sum += (double)input.span_num[S][t];
        }
        turn_weight.push_back(span_sum);
    }

    std::discrete_distribution<std::size_t> dist_call(prob.begin(), prob.end());
    std::discrete_distribution<std::size_t> dist_turn(turn_weight.begin(), turn_weight.end());

    const int nnum = (int)param.n.size();
    std::vector<int> trials_count(nnum, 0);
    std::vector<int> accept_count(nnum, 0);
    std::vector<int> update_count(nnum, 0);

    std::vector<double> temp_diff(nnum);
    for (int i = 0; i < nnum; i++) {
        temp_diff[i] = param.n[i].temp_start - param.n[i].temp_end;
    }

    l_timer.start();
    solver_state cur_state = best_state;

    while (true) {
        const auto fidx = dist_call(engine);
        const double cur_time = l_timer.get_time();
        const double cur_temp = cur_time * temp_diff[fidx];
        if (cur_time > param.time_limit) {
            break;
        }
        trials_count[fidx]++;
        const int turn = (int)dist_turn(engine);
        if (param.n[fidx].func(cur_state, input, turn, cur_temp)) {
            accept_count[fidx]++;
        }
        if (best_state.score < cur_state.score) {
            update_count[fidx]++;
            best_state = cur_state;
        }
    }

#ifdef DEBUG
    const int HWT = input.grid_height * input.grid_width * input.turn_num;
    const int scr = (int)(1.0e6 * (double)best_state.score / (double)HWT);
    std::cerr << "[best score = " << scr << "]" << std::endl;
    for (int i = 0; i < nnum; i++) {
        std::cerr << "[func: " << i << "]" << std::endl;
        std::cerr << "  trials count = " << trials_count[i] << std::endl;
        std::cerr << "  accept count = " << accept_count[i] << std::endl;
        std::cerr << "  update count = " << update_count[i] << std::endl;
    }
#endif
}

/* ------- 8< ------- 8< ------- 8< ------- 8< ------- 8< ------- 8< -------- */

template<typename T>
input_data get_input(T& in)
{
    input_data input;

    in >> input.turn_num;
    in >> input.grid_height;
    in >> input.grid_width;

    input.entrance_pos.y = 0;
    in>> input.entrance_pos.x;

    input.G.resize(input.grid_height);
    for (auto& row: input.G) {
        row.resize(input.grid_width);
    }

    std::string s;
    for (int x = 0; x < input.grid_height - 1; x++) {
        in >> s;
        for (int y = 0; y < input.grid_width; y++) {
            if (s[y] == '0') {
                input.G[x + 0][y].push_back(position( 1, 0));
                input.G[x + 1][y].push_back(position(-1, 0));
            }
        }
    }
    for (int x = 0; x < input.grid_height; x++) {
        in >> s;
        for (int y = 0; y < input.grid_width - 1; y++) {
            if (s[y] == '0') {
                input.G[x][y + 0].push_back(position(0,  1));
                input.G[x][y + 1].push_back(position(0, -1));
            }
        }
    }

    for (int i = 0; i < MAX_TURN; i++) {
        for (int j = 0; j < MAX_TURN; j++) {
            input.span_num[i][j] = 0;
        }
    }

    in >> input.crop_num;
    input.crops.resize(input.crop_num);
    for (auto& c: input.crops) {
        in >> c.first >> c.second;
        input.span_num[c.first][c.second]++;
    }

    return input;
}

solver_state get_init_solver_state(const input_data& input)
{
    solver_state ret;

    for (int t = 0; t <= input.turn_num; t++) {
        for (int x = 0; x < input.grid_height; x++) {
            for (int y = 0; y < input.grid_width; y++) {
                if (t == 0 || t == input.turn_num) {
                    ret.harvest_plan[t][x][y] = true;
                } else {
                    ret.harvest_plan[t][x][y] = false;
                }
            }
        }
    }

    for (int i = 0; i < MAX_TURN; i++) {
        for (int j = 0; j < MAX_TURN; j++) {
            ret.exist_space[i][j] = 0;
        }
    }
    ret.exist_space[1][input.turn_num] = input.grid_width * input.grid_height;
    ret.score = get_harvest_score(input, ret, input.turn_num);

    for (int t = 1; t < input.turn_num; t++) {
        ret.shrink_pos[t].clear();
        ret.extend_pos[t].clear();
        ret.extend_pos[t].push_back(input.entrance_pos);
    }

    for (int t = 1; t <= input.turn_num; t++) {
        ret.harvest_score[t] = get_harvest_score(input, ret, t);
    }

    for (int x = 0; x < input.grid_height; x++) {
        for (int y = 0; y < input.grid_width; y++) {
            ret.last_trial[x][y] = 0;
        }
    }

    return ret;
}

void finalize_state(solver_state& state, const input_data& input)
{
    for (int t = input.turn_num - 1; t >= 1; t--) {
        update_shrink_pos(state, input, t);
        for (int x = 0; x < input.grid_height; x++) {
            for (int y = 0; y < input.grid_width; y++) {
                if (state.harvest_plan[t - 1][x][y] && state.harvest_plan[t][x][y]) {
                    const position pt(x, y);
                    for (const auto& pos: state.shrink_pos[t]) {
                        if (pt == pos) {
                            state.harvest_plan[t][x][y] = false;
                            update_shrink_pos(state, input, t);
                        }
                    }
                }
            }
        }
    }
}

std::vector<answer_data> get_answer(const input_data& input, const solver_state& state)
{
    std::map<std::pair<int,int>, std::vector<position>> space_pos;
    std::array<std::array<std::vector<int>, MAX_TURN>, MAX_TURN> crops_id;

    for (int x = 0; x < input.grid_height; x++) {
        for (int y = 0; y < input.grid_width; y++) {
            int S = -1;
            for (int t = 1; t <= input.turn_num; t++) {
                if (state.harvest_plan[t - 1][x][y] && !state.harvest_plan[t][x][y]) {
                    S = t;
                } else if (state.harvest_plan[t][x][y] && S >= 0) {
                    space_pos[std::make_pair(S, t)].push_back(position(x, y));
                    S = -1;
                }
            }
        }
    }

    for (int i = 0; i < input.crop_num; i++) {
        const auto& [S, D] = input.crops[i];
        crops_id[S][D].push_back(i);
    }

    std::vector<answer_data> ret;
    for (int D = 1; D <= input.turn_num; D++) {
        std::vector<std::pair<int, position>> space_left;
        for (int S = 1; S < D; S++) {
            const std::pair<int,int> sp(S, D);
            for (const auto& pt: space_pos[sp]) {
                space_left.push_back(std::make_pair(S, pt));
            }
            auto& crops = crops_id[S][D];
            const int pop_num = (int)std::min(space_left.size(), crops.size());
            for (int i = 0; i < pop_num; i++) {
                const int crop_id = crops.back();
                const auto [St, pos] = space_left.back();
                crops.pop_back();
                space_left.pop_back();
                ret.push_back(answer_data(crop_id + 1, pos.x, pos.y, St));
            }
        }
    }

    return ret;
}

solver_parameter get_param(int argc, char **argv)
{
    solver_parameter param;

    param.time_limit = 1.9;

#ifdef TUNING
    assert(argc == 5);
    param.n = {
        neighbor(try_extend_harvest_erea, std::stod(argv[1]), 0.0, std::stod(argv[2])),
        neighbor(try_shrink_harvest_erea, std::stod(argv[3]), 0.0, std::stod(argv[4])),
    };
#else
    param.n = {
        neighbor(try_extend_harvest_erea, 0.8233737988794844, 0.0, 74.24771801578433),
        neighbor(try_shrink_harvest_erea, 0.1440563708253237, 0.0, 66.90782862879715),
    };
#endif

    return param;
}

int main(int argc, char **argv)
{
    const auto&& param = get_param(argc, argv);
    const auto&& input = get_input(std::cin);

    auto&& state = get_init_solver_state(input);
    simulated_annealing(state, input, param);
    finalize_state(state, input);

    const auto&& ans = get_answer(input, state);
    std::cout << ans.size() << std::endl;
    for (const auto& p: ans) {
        std::cout << p.k << " " << p.i << " " << p.j << " " << p.s << std::endl;
    }

    return 0;
}
