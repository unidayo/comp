#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")


#include <bits/stdc++.h>


#define FOR(i, s, e) for (int i = int(s); i < int(e); ++i)
#define REV(i, s, e) for (int i = int(s); i > int(e); --i)


using namespace std;


constexpr int DIFF_SCORE_COEF = 10000;
constexpr double ERR = 1e-6;
constexpr double MAX_TIME = 2960000;  // microseconds
constexpr double INITIAL_TEMPERATURE_COEF = 0.6;
constexpr double SOLVE_PARTIAL_MAX_TIME = 1000000;  // microseconds
constexpr double SOLVE_PARTIAL_INITIAL_TEMPERATURE_COEF = 0.001;
constexpr double MAX_ADD_AMOUNT_THRESHOLD = 1.11;
constexpr double MIN_ADD_AMOUNT_THRESHOLD = 1.02;
constexpr double SCORE_THRESHOLD_COEF = 0.4;
constexpr double MAX_SCORE_THRESHOLD = 5;
constexpr int LOG_TABLE_SIZE = 1024;
constexpr int SA_LOOP = 10;
constexpr int N = 20;


uint32_t randxor() {
    static uint32_t x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    uint32_t t;
    t = (x^(x<<11));
    x = y;
    y = z;
    z = w;
    return w = (w^(w>>19))^(t^(t>>8));
}


uint32_t randrange(const uint32_t n) {
    return uint64_t(randxor()) * n >> 32;
}


uint32_t randrange(const uint32_t l, const uint32_t r) {
    return randrange(r-l)+l;
}


template <typename T>
T choice(const vector<T>& vec) {
    return vec[randrange(vec.size())];
}


template <typename T>
uint32_t random_by_weight(const vector<T>& cum_sum_weight) {
    const T w = randrange(cum_sum_weight.back());
    const auto i = lower_bound(cum_sum_weight.begin(), cum_sum_weight.end(), w) - cum_sum_weight.begin();
    return i;
}


double uniform() {
    return (double)randxor()/UINT32_MAX;
}


// Ref: https://topcoder-tomerun.hatenablog.jp/entry/2021/06/12/134643
struct IndexSet {

    int que_size;
    array<int, N> que;
    array<int, N> pos;

    IndexSet() {
        clear();
    }

    void add(const int v) {
        if (pos[v] >= 0) {
            return;
        }
        pos[v] = que_size;
        que[que_size++] = v;
    }

    void remove(const int v) {
        int p = pos[v];
        if (p < 0) {
            return;
        }
        int b = que[--que_size];
        que[p] = b;
        pos[b] = p;
        pos[v] = -1;
    }

    bool contains(int v) const {
        return pos[v] != -1;
    }

    int size() const {
        return que_size;
    }

    void clear() {
        que_size = 0;
        fill(pos.begin(), pos.end(), -1);
    }
};


struct LogTable {
    vector<double> table;
    int size;

    LogTable(const int size_) : size(size_) {
        table.resize(size);
        FOR (i, 0, size) {
            table[i] = log2((double)i / (double)size);
        }
    }
};


struct Color {
    double cyan, magenta, yellow;

    Color() : cyan(0), magenta(0), yellow(0) {};

    Color(const double cyan_, const double magenta_, const double yellow_) : cyan(cyan_), magenta(magenta_), yellow(yellow_) {};

    friend ostream& operator<<(ostream& os, const Color& color);

    void operator+=(const Color& other) {
        cyan += other.cyan;
        magenta += other.magenta;
        yellow += other.yellow;
    }

    void operator-=(const Color& other) {
        cyan -= other.cyan;
        magenta -= other.magenta;
        yellow -= other.yellow;
    }

    void operator/=(const double other) {
        cyan /= other;
        magenta /= other;
        yellow /= other;
    }

    void reset() {
        cyan = 0;
        magenta = 0;
        yellow = 0;
    }
};


ostream& operator<<(ostream& os, const Color& color) {
    os << "Color: " << color.cyan << ", " << color.magenta << ", " << color.yellow;
    return os;
}


Color operator+(const Color& lhs, const Color& rhs) {
    return Color{lhs.cyan + rhs.cyan, lhs.magenta + rhs.magenta, lhs.yellow + rhs.yellow};
}

Color operator-(const Color& lhs, const Color& rhs) {
    return Color{lhs.cyan - rhs.cyan, lhs.magenta - rhs.magenta, lhs.yellow - rhs.yellow};
}

Color operator*(const Color& lhs, const double rhs) {
    return Color{lhs.cyan * rhs, lhs.magenta * rhs, lhs.yellow * rhs};
}

Color operator/(const Color& lhs, const double rhs) {
    return Color{lhs.cyan / rhs, lhs.magenta / rhs, lhs.yellow / rhs};
}

double inner_product(const Color& color, const Color& other_color) {
    return color.cyan*other_color.cyan + color.magenta*other_color.magenta + color.yellow*other_color.yellow;
}


double calculate_sq_diff(const Color& c1, const Color& c2) {
    double dc = c1.cyan - c2.cyan;
    double dm = c1.magenta - c2.magenta;
    double dy = c1.yellow - c2.yellow;
    return dc * dc + dm * dm + dy * dy;
}


double calculate_diff(const Color& c1, const Color& c2) {
    double dc = c1.cyan - c2.cyan;
    double dm = c1.magenta - c2.magenta;
    double dy = c1.yellow - c2.yellow;
    return sqrt(dc * dc + dm * dm + dy * dy);
}


struct Input {
    int N,K,H,T,D;
    vector<Color> tubes, targets;

    void get_input() {
        cin >> N >> K >> H >> T >> D;
        FOR (i,0,K) {
            double cyan, magenta, yellow;
            cin >> cyan >> magenta >> yellow;
            tubes.push_back({cyan * DIFF_SCORE_COEF, magenta * DIFF_SCORE_COEF, yellow * DIFF_SCORE_COEF});
        }
        FOR (i,K,N) {
            tubes.push_back(tubes[i%K]);
        }
        FOR (i,0,H) {
            double cyan, magenta, yellow;
            cin >> cyan >> magenta >> yellow;
            targets.push_back({cyan * DIFF_SCORE_COEF, magenta * DIFF_SCORE_COEF, yellow * DIFF_SCORE_COEF});
        }
        cerr << N << " " << K << " " << H << " " << T << " " << D << "\n";
    }
};


struct Config {
    Input input;
    int lane_count, lane_width;
    double max_add_amount;
    double put_score_coef, over_production_penalty_coef;

    void set_config(const Input& input_) {
        input = input_;
        // if (input.T < 10000) {
        //     lane_count = max(10, input.K);
        // } else {
        //     lane_count = input.N;
        // }
        lane_count = input.N;
        lane_width = input.N / lane_count;
        lane_count = input.N / lane_width;
        max_add_amount = MIN_ADD_AMOUNT_THRESHOLD + (MAX_ADD_AMOUNT_THRESHOLD - MIN_ADD_AMOUNT_THRESHOLD) * (input.K - 4) / 16.0;
        
        cerr << lane_count << " " << lane_width << " " << max_add_amount << "\n";
    }

    void update_score_coef(const int res_target_count, const int add_count) {
        const bool minimize_diff = false;

        if (minimize_diff) {  // デバッグ用、差分を最小化
            put_score_coef = 0;
            over_production_penalty_coef = 0;
            return;
        }
        put_score_coef = (res_target_count > 2) ? min(1.0, exp(add_count - input.H)) * input.D : input.D;
        over_production_penalty_coef = max(0.0, input.D - put_score_coef);
    }
};


struct Solver {
    Input input;
    Config config;
    vector<vector<int>> vs, hs, initial_vs, initial_hs;
    vector<vector<int>> ops;
    vector<vector<pair<int, int>>> lanes;
    int execute_count = 0, add_count = 0;
    int res_target_count;
    double start;
    LogTable logs{LOG_TABLE_SIZE};

    void solve() {
        start = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();

        input.get_input();

        vs = vector<vector<int>>(input.N, vector<int>(input.N-1, 0));
        hs = vector<vector<int>>(input.N-1, vector<int>(input.N, 0));

        config.set_config(input);

        solve_greedy();

        print_initial_partitions();

        for (const auto& op: ops) {
            for (const auto& v: op) {
                printf("%d ", v);
            }
            printf("\n");
        }
    }

    void print_initial_partitions() {
        for (const auto& vy: initial_vs) {
            for (const auto vyx: vy) {
                printf("%d ", vyx);
            }
            printf("\n");
        }
        for (const auto& hy: initial_hs) {
            for (const auto hyx: hy) {
                printf("%d ", hyx);
            }
            printf("\n");
        }
    }

    void execute(const int op, const int i, const int j, const int k = 0) {
        ++execute_count;
        switch (op) {
            case 1: {  // チューブkからマス(i,j)に色を1g追加
                ops.push_back({1, i, j, k % input.K});
                break;
            }
            case 2: {  // マス(i,j)に色を1g提供
                ops.push_back({2, i, j});
                break;
            }
            case 3: {  // マス(i,j)に色を1g破棄
                ops.push_back({3, i, j});
                break;
            }
            case 4: {  // 隣接するマス(i,j)と(i2,j2)の間の仕切りの状態を切り替える。kによって(i2,j2)を計算
                if (k == 0) {
                    vs[i][j] ^= 1;
                    ops.push_back({4, i, j, i, j+1});
                } else {
                    hs[i][j] ^= 1;
                    ops.push_back({4, i, j, i+1, j});
                }
                break;
            }
        }
    }

    void add(const int i, const int j, const int k) {
        ++add_count;
        execute(1, i, j, k);
    }

    void give(const int i, const int j) {
        execute(2, i, j);
    }

    void remove(const int i, const int j) {
        execute(3, i, j);
    }

    void flip_v(const int i, const int j) {
        execute(4, i, j, 0);
    }

    void flip_h(const int i, const int j) {
        execute(4, i, j, 1);
    }

    void add_lane(const vector<pair<int,int>>& lane, const int i, const int k) {
        const auto& [y, x] = lane[i];
        add(y, x, k);
    }

    void remove_laneadd_lane(const vector<pair<int,int>>& lane, const int i) {
        const auto& [y, x] = lane[i];
        remove(y, x);
    }

    void flip_lane(const vector<pair<int,int>>& lane, const int i) {
        const auto left_ = lane[i];
        const auto right_ = lane[i+1];
        if (left_.first == right_.first) {
            const auto y = left_.first;
            const auto x = min(left_.second, right_.second);
            flip_v(y, x);
        } else {
            const auto y = min(left_.first, right_.first);
            const auto x = left_.second;
            flip_h(y, x);
        }
    }

    void make_lane_partitions() {
        FOR (y,1,input.N-1) {
            vs[y][input.N-2] = 1;
        }
        FOR (y,0,input.N-1) {
            FOR (x,0,input.N-1) {
                hs[y][x] = 1;
            }
        }
        hs[0][input.N-1] = 1;
        hs[input.N-2][input.N-1] = 1;

        vector<pair<int,int>> lane;
        FOR (i,0,config.lane_count) {
            lane.clear();
            const auto y = i;
            FOR (x,0,input.N) {
                lane.push_back({y, x});
            }
            if (i == 0 || i+1 == config.lane_count) {
                const auto ny = (i == 0) ? y + 1 : y - 1;
                const auto nx = input.N-1;
                lane.push_back({ny, nx});
            }
            lanes.push_back(lane);
        }
    }

    void save_initial_partitions() {
        initial_vs = vs;
        initial_hs = hs;
    }

    int expect_execute_count(int now_cap, const int count, const int cap, const int put, const int max_cap) {
        if (count == 0) {
            return 0;
        }
        int execute_count = 0;
        if (put) {
            if (now_cap != max_cap) {
                ++execute_count;
            }
            if (now_cap > 0 && now_cap < max_cap) {
                ++execute_count;
            }
            ++execute_count;
            now_cap = max_cap;
        }

        if (now_cap < cap) {
            ++execute_count;
            if (now_cap > 0) {
                ++execute_count;
            }
        }

        const auto new_cap = cap - count;
        if (new_cap > 0) {
            ++execute_count;
        }
        if (cap > new_cap) {
            ++execute_count;
        }
        return execute_count;
    }

    void make_color_directly(const vector<pair<int,int>>& pattern) {
        const int max_turn = pattern.size();
        int now_turn = 0;
        for (const auto& [turn, i]: pattern) {
            while (now_turn < turn) {
                give(1, input.N-1);
                ++now_turn;
            }
            add(1, input.N-1, i);
        }
        while (now_turn < max_turn) {
            give(1, input.N-1);
            ++now_turn;
        }
    }

    void make_color_from_amounts(const int composed_color_cap, double& res_composed_color_amount, array<double, N>& now_amounts, array<int, N>& now_caps, const array<double, N>& amounts, const array<int, N>& counts, const array<int, N>& caps, const array<int, N>& puts, const array<int, N>& tube_indices) {
        double total_amount = res_composed_color_amount;
        FOR (i,0,config.lane_count) {  // 補充
            if (counts[i] == 0 || !puts[i]) {
                continue;
            }
            const auto now_cap = now_caps[i];
            const auto cap = caps[i];
            const auto& lane = lanes[i];
            if (now_cap != cap) {
                flip_lane(lane, cap-1);
            }
            if (now_cap > 0 && now_cap < cap) {
                flip_lane(lane, now_cap-1);
            }
            const int k = tube_indices[i];
            add_lane(lane, 0, k);
            const auto add_amount = res_composed_color_amount * (double)(cap - now_cap) / (double)composed_color_cap;
            now_amounts[i] += 1.0 + add_amount;
            now_caps[i] = cap;
            total_amount -= add_amount;
        }

        FOR (i,0,config.lane_count) {  // キャップの変更
            const auto now_cap = now_caps[i];
            const auto count = counts[i];
            const auto cap = caps[i];
            if (count == 0 || cap <= now_cap) {
                continue;
            }
            const auto& lane = lanes[i];
            flip_lane(lane, cap-1);
            if (now_cap > 0) {
                flip_lane(lane, now_cap-1);
            }
            const auto add_amount = res_composed_color_amount * (double)(cap - now_cap) / (double)composed_color_cap;
            now_amounts[i] += add_amount;
            total_amount -= add_amount;
        }

        FOR (i,0,config.lane_count) {  // 合成
            const auto now_amount = now_amounts[i];
            const auto amount = amounts[i];
            const auto count = counts[i];
            const auto cap = caps[i];
            if (count == 0) {
                continue;
            }
            const auto new_cap = cap - count;
            const auto& lane = lanes[i];
            if (new_cap > 0) {
                flip_lane(lane, new_cap-1);
            }
            if (cap > new_cap) {
                flip_lane(lane, cap-1);
            }
            now_amounts[i] = max(0.0, now_amount - amount);
            now_caps[i] = new_cap;
            total_amount += amount;
        }

        give(1, input.N-1);  // 提供
        total_amount -= 1.0;
        // while (total_amount > ERR) {  // 過剰生産分の削除
        //     remove(0, input.N-1);
        //     total_amount -= 1.0;
        // }
        res_composed_color_amount = total_amount;
    }

    double calculate_score(const int put_count, const double diff, const double total_amount) {
        const auto score = 
                        (double)put_count * config.put_score_coef
                        + diff  // * res_target_countとして最終的なコストの総和を現時点での値で見積もると良いかもしれない
                        // + input.D * (total_amount - 1.0) * config.over_production_penalty_coef;  // 過剰生産に対するペナルティ
                        ;
        return score;
    }


    double calculate_score(const vector<pair<int, int>> ans, const vector<Color>& targets, const int left, const int right) {
        double score = 0;
        Color composed_color;
        int composed_color_amount = 0;
        int index = left;
        FOR (j, left, right) {
            while (index < right && ans[index].first == j) {
                const auto i = ans[index].second;
                const auto add_color = input.tubes[i];
                composed_color = (composed_color * composed_color_amount + add_color) / (composed_color_amount + 1);
                ++composed_color_amount;
                ++index;
            }
            const auto& target = targets[j];
            const auto diff = calculate_diff(composed_color, target);
            score += diff;
            --composed_color_amount;
        }
        return score;
    }


    void solve_partial(const vector<Color>& targets) {
        if (targets.empty()) {
            return;
        }
        for (const auto& lane: lanes) {
            const auto& [sy, sx] = lane[lane.size() - 2];
            const auto& [ty, tx] = lane[lane.size() - 1];
            if (sy == ty) {
                vs[sy][min(sx,tx)] = 0;
            } else {
                hs[min(sy,ty)][sx] = 0;
            }
        }
        save_initial_partitions();

        vector<pair<int, int>> ans(targets.size()), best_ans(targets.size());
        double score = 0;
        FOR (i,0,targets.size()) {
            const auto& target = targets[i];
            double min_diff = INT_MAX;
            int arg_min = -1;
            FOR (i, 0, input.K) {
                const auto& color = input.tubes[i];
                const auto diff = calculate_diff(color, target);
                if (diff < min_diff) {
                    min_diff = diff;
                    arg_min = i;
                }
            }
            score += min_diff;
            ans[i] = {i, arg_min};
        }
    
        auto best_score = score;
        best_ans = ans;

        long long iter = 0, update = 0, best_update = 0;
        double time = 0;
        const double max_time = SOLVE_PARTIAL_MAX_TIME * (double)targets.size() / 400.0;

        while (true) {
            time = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() - start;
            if (time > max_time) {
                break;
            }
            ++iter;

            const int i = randrange(ans.size());
            const auto temp = ans[i];
            const auto min_turn = (i > 0) ? ans[i-1].first : 0;
            const auto max_turn = (i+1 < ans.size()) ? min(i, ans[i+1].first) : i;
            if (min_turn >= max_turn) {
                continue;
            }

            ans[i].first = randrange(min_turn, max_turn + 1);

            int left = i;
            while (ans[left].first != left) {
                --left;
            }
            int right = i+1;
            while (right < ans.size() && ans[right].first != right) {
                ++right;
            }

            double min_score = INT_MAX;
            int arg_min = -1;
            FOR (j,0,input.K) {
                ans[i].second = j;
                const auto temp_score = calculate_score(ans, targets, left, right);
                if (temp_score < min_score) {
                    min_score = temp_score;
                    arg_min = j;
                }
            }

            ans[i].second = arg_min;

            const auto temperature = score * SOLVE_PARTIAL_INITIAL_TEMPERATURE_COEF * (max_time - time) / max_time;
            const double update_threshold = score - temperature * choice(logs.table);

            const auto new_score = calculate_score(ans, targets, 0, ans.size());

            if (new_score < update_threshold) {
                ++update;
                score = new_score;
                if (score < best_score) {
                    ++best_update;
                    best_score = score;
                    best_ans = ans;
                }
            } else {
                ans[i] = temp;
            }
        }

        cerr << "iter: " << iter << "\n";
        cerr << "update: " << update << "\n";
        cerr << "best_update: " << best_update << "\n";
        cerr << "score: " << best_score << "\n";

        make_color_directly(best_ans);
    }

    void solve_greedy() {
        make_lane_partitions();
        save_initial_partitions();

        long long total_iter = 0, total_update = 0, total_best_update = 0;
        double total_score = 0, total_diff = 0;
        int total_expected_execute_count = 0;

        Color total_composed_color, temp_total_composed_color, new_total_composed_color, composed_color;
        array<int, N> tube_indices;
        using Candidate = tuple<double, double, int, int, int, int, Color, bool>;
        vector<Candidate> candidates;
        array<double, N> now_amounts, best_amounts;
        array<int, N> now_caps, best_counts, best_caps, best_puts;
        array<Color, N> now_colors;
        array<vector<Color>, N> lane_colors, add_colors, put_colors;
        array<vector<double>, N> amounts, add_amounts;
        array<vector<int>, N> counts, caps, execute_counts, min_execute_counts, puts;
        array<bool, N> is_first;
        array<int, N> candidate_indices, best_indices, zero_indices, merge_all_indices;

        IndexSet i_candidates, j_candidates;
        Color res_composed_color, put_color, temp_put_color, new_put_color, res_color;
        double res_composed_color_amount = 0;

        FOR (i,0,config.lane_count) {
            is_first[i] = true;
            now_amounts[i] = 0;
            tube_indices[i] = i % input.K;
            now_caps[i] = lanes[i].size() - 1;
        }
        for (const auto& i: tube_indices) {
            cerr << i << " ";
        }
        cerr << "\n";

        const int max_direct_make_color_count = 4;
        const int max_pattern_size = 5000;
        vector<vector<pair<int, int>>> each_turn_patterns, patterns, temp_patterns;
        each_turn_patterns.push_back({});
        FOR (_,0,max_direct_make_color_count) {
            temp_patterns.clear();
            const int max_temp_pattern_size = max_pattern_size - patterns.size();
            for (auto& pattern: each_turn_patterns) {
                const auto last = pattern.empty() ? 0 : pattern.back().first;
                FOR (i,last,pattern.size()+1) {
                    FOR (j,0,input.K) {
                        if (temp_patterns.size() >= max_temp_pattern_size) {
                            break;
                        }
                        pattern.push_back({i, j});
                        temp_patterns.push_back(pattern);
                        pattern.pop_back();
                    }
                }
            }
            copy(temp_patterns.begin(), temp_patterns.end(), back_inserter(patterns));
            swap(each_turn_patterns, temp_patterns);
        }
        cerr << patterns.size() << "\n";

        int complete_target_count = 0;

        int partial_solve_targets_size = 0;
        if (input.T < 7000) {
            partial_solve_targets_size = 400;
        }

        solve_partial(vector<Color>(input.targets.begin(), input.targets.begin() + partial_solve_targets_size));

        complete_target_count += partial_solve_targets_size;
        if (partial_solve_targets_size > 0) {
            FOR (i,0,config.lane_count) {
                is_first[i] = false;
                now_caps[i] = 0;
            }
        }

        int empty_area = input.N * input.N;
        FOR (i,0,config.lane_count) {
            empty_area -= lanes[i].size() - 1;
        }
        double total_success_score = 0;
        int success_count = 0;
        while (complete_target_count < input.H) {
            const bool next_is_last = (input.D >= 1000 && complete_target_count == input.H-2);
            const auto& target_color = input.targets[complete_target_count];
            const auto& next_target_color = input.targets[min(complete_target_count+1, input.H-1)];
            const double start_partial_time = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
            const double time_ = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() - start;
            const auto res_time = MAX_TIME - time_;
            res_target_count = input.H - complete_target_count;
            const auto res_execute_count = input.T - execute_count;
            const int max_execute_count = (input.D >= 1000 || res_target_count == 1) ? (res_execute_count + res_target_count - 1) / res_target_count : (res_execute_count - config.lane_count + res_target_count - 1) / res_target_count;

            const double avg_score = total_success_score / (double)max(1, success_count);
            const double score_threshold = (res_target_count == 1) ? 0.0 : min(MAX_SCORE_THRESHOLD, avg_score * SCORE_THRESHOLD_COEF);
            const auto max_time = res_time / (double)res_target_count;
            long long iter = 0, update = 0, best_update = 0;
            // cerr << score_threshold << " " << max_time << "\n";

            config.update_score_coef(res_target_count, add_count);

            int merge_all_count = 1;
            double merge_all_amount = res_composed_color_amount;

            auto total_color = res_composed_color * res_composed_color_amount;
            int composed_color_cap = empty_area;
            FOR (i,0,config.lane_count) {
                const int max_cap = lanes[i].size() - 1;
                composed_color_cap += max_cap - now_caps[i];
                total_color += now_colors[i] * now_amounts[i];
            }

            FOR (i,0,config.lane_count) {
                candidates.clear();

                const auto now_amount = now_amounts[i];
                const auto now_cap = now_caps[i];
                const auto now_color = now_colors[i];
                const auto add_color = input.tubes[tube_indices[i]];
                const int max_cap = lanes[i].size() - 1;

                if (is_first[i]) {
                    FOR (new_count, 0, max_cap+1) {
                        const auto new_cap = max_cap;
                        const double new_amount = (double)new_count / (double)new_cap;
                        const int put = (new_count > 0) ? 1 : 0;
                        const auto execute_count = expect_execute_count(now_cap, new_count, new_cap, put, max_cap);
                        const auto add_amount = 0;
                        const auto new_color = add_color;
                        const auto is_same = false;
                        candidates.push_back({new_amount, add_amount, new_count, new_cap, execute_count, put, new_color, is_same});
                    }
                } else {
                    FOR (new_count, 0, now_cap+1) {
                        const auto new_cap = now_cap;
                        const double new_amount = now_amount * (double)new_count / (double)max(1, new_cap);
                        const int put = 0;
                        const auto execute_count = expect_execute_count(now_cap, new_count, new_cap, put, max_cap);
                        const auto add_amount = 0;
                        const auto new_color = now_color;
                        const auto is_same = (new_count == now_cap);
                        candidates.push_back({new_amount, add_amount, new_count, new_cap, execute_count, put, new_color, is_same});
                    }
                    const auto new_cap = max_cap;
                    FOR (new_count, 1, new_cap+1) {  // 絵の具を追加しないパターン, 2ターン増えるが精度を向上できる
                        const auto add_amount = res_composed_color_amount * (double)(new_cap - now_cap) / (double)composed_color_cap;
                        const double new_amount = (now_amount + add_amount) * (double)new_count / (double)new_cap;
                        const int put = 0;
                        const auto execute_count = expect_execute_count(now_cap, new_count, new_cap, put, max_cap);
                        const auto new_color = (now_color * now_amount + res_composed_color * add_amount) / (now_amount + add_amount);
                        const auto is_same = false;
                        candidates.push_back({new_amount, add_amount, new_count, new_cap, execute_count, put, new_color, is_same});
                    }

                    const auto min_new_cap = (input.K >= 6 && input.T >= 40000) ? max_cap : max(now_cap+1, max_cap-4);
                    FOR (new_cap, min_new_cap, max_cap+1) {
                        FOR (new_count, 1, new_cap+1) {  // 絵の具を追加するパターン
                            const auto add_amount = res_composed_color_amount * (double)(new_cap - now_cap) / (double)composed_color_cap;
                            const double new_amount = (now_amount + add_amount + 1.0) * (double)new_count / (double)new_cap;
                            if (new_amount <= now_amount+ERR || new_amount - add_amount >= res_composed_color_amount + config.max_add_amount) {
                                continue;
                            }
                            const int put = 1;
                            const auto execute_count = expect_execute_count(now_cap, new_count, new_cap, put, max_cap);
                            const auto new_color = (now_color * now_amount + res_composed_color * add_amount + add_color) / (now_amount + add_amount + 1.0);
                            const auto is_same = false;
                            candidates.push_back({new_amount, add_amount, new_count, new_cap, execute_count, put, new_color, is_same});
                        }
                    }
                }

                sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
                    return get<0>(a)+get<1>(a) < get<0>(b)+get<1>(b);
                });

                auto& amounts_i = amounts[i];
                auto& add_amounts_i = add_amounts[i];
                auto& counts_i = counts[i];
                auto& caps_i = caps[i];
                auto& execute_counts_i = execute_counts[i];
                auto& puts_i = puts[i];
                auto& colors_i = lane_colors[i];
                auto& min_execute_counts_i = min_execute_counts[i];
                auto& add_colors_i = add_colors[i];
                auto& put_colors_i = put_colors[i];

                amounts_i.clear();
                add_amounts_i.clear();
                counts_i.clear();
                caps_i.clear();
                execute_counts_i.clear();
                puts_i.clear();
                colors_i.clear();
                min_execute_counts_i.clear();
                add_colors_i.clear();
                put_colors_i.clear();

                int index = 0;
                for (const auto& [amount, add_amount, count, cap, execute_count, put, color, is_same]: candidates) {
                    if (count == 0) {
                        zero_indices[i] = index;
                    }
                    if (is_same) {  // ただ仕切りを下げるだけの操作. インクの追加がH回に達し、かつ最後のターンであればこれを解の候補にする
                        merge_all_indices[i] = index;
                        if (now_cap > 0) {
                            ++merge_all_count;
                        }
                        merge_all_amount += amount;
                    }
                    amounts_i.emplace_back(amount);
                    add_amounts_i.emplace_back(add_amount);
                    counts_i.emplace_back(count);
                    caps_i.emplace_back(cap);
                    execute_counts_i.emplace_back(execute_count);
                    puts_i.emplace_back(put);
                    colors_i.emplace_back(color);
                    min_execute_counts_i.emplace_back(execute_count);
                    add_colors_i.emplace_back(color * amount - res_composed_color * add_amount);
                    put_colors_i.emplace_back(add_color * put);
                    ++index;
                }
                REV (j, min_execute_counts_i.size()-2, -1) {
                    min_execute_counts_i[j] = min(min_execute_counts_i[j], min_execute_counts_i[j+1]);
                }
            }

            double best_diff = 0;
            double best_score = -1;
            double best_composed_color_amount = 0.0;
            Color best_composed_color;
            int best_execute_count = 0;

            int arg_min_i = -1;
            int arg_min_j = -1;

            bool initial_ans_as_merge_all = (merge_all_count <= res_execute_count)
                                        && (add_count == input.H && complete_target_count == input.H - 1);
            if (initial_ans_as_merge_all) {
                candidate_indices = merge_all_indices;

                total_composed_color = res_composed_color * res_composed_color_amount;
                int put_count = 0;
                double total_amount = 0;
                double total_res_composed_color_amount = res_composed_color_amount;
                int each_lane_execute_count = 0;
                FOR (i,0,config.lane_count) {
                    const auto ci = candidate_indices[i];
                    const auto amount = amounts[i][ci];
                    const auto add_amount = add_amounts[i][ci];
                    const auto& color = add_colors[i][ci];
                    put_count += puts[i][ci];
                    total_composed_color += color;
                    total_amount += amount;
                    each_lane_execute_count += execute_counts[i][ci];
                    total_res_composed_color_amount -= add_amount;
                }

                composed_color = total_composed_color / (total_amount + total_res_composed_color_amount);

                const auto diff = calculate_diff(composed_color, target_color);
                auto score = calculate_score(put_count, diff, total_amount + total_res_composed_color_amount);

                best_score = score;
                best_indices = candidate_indices;
            }

            FOR (i,0,config.lane_count) {
                const auto& amounts_i = amounts[i];
                const auto& add_amounts_i = add_amounts[i];
                const auto& execute_counts_i = execute_counts[i];
                const auto& puts_i = puts[i];
                const auto& colors_i = add_colors[i];
                const auto& put_colors_i = put_colors[i];

                FOR (j,0,amounts_i.size()) {
                    const auto amount = amounts_i[j];
                    const auto add_amount = add_amounts_i[j];
                    const auto composed_color_amount = amount + res_composed_color_amount - add_amount;
                    const int expected_execute_count = execute_counts_i[j] + 1;
                    if (expected_execute_count > max_execute_count || composed_color_amount + ERR < 1.0) {
                        continue;
                    }
                    const int put_count = puts_i[j];
                    const auto composed_color = (colors_i[j] + res_composed_color * res_composed_color_amount) / composed_color_amount;

                    auto diff = calculate_diff(composed_color, target_color);
                    if (next_is_last && add_count+put_count <= input.H) {
                        const auto res_color = total_color + put_colors_i[j] - composed_color;
                        diff = (diff + calculate_diff(res_color, next_target_color));
                    }
                    const auto score = calculate_score(put_count, diff, composed_color_amount);
                    if (best_score < 0 || score < best_score) {
                        best_score = score;
                        arg_min_i = i;
                        arg_min_j = j;
                    }
                }
            }

            if (arg_min_i >= 0) {
                best_indices = zero_indices;
                best_indices[arg_min_i] = arg_min_j;
            }

            if (best_score >= 0) {
                candidate_indices = best_indices;

                total_composed_color = res_composed_color * res_composed_color_amount;
                int put_count = 0;
                double total_amount = 0;
                double total_res_composed_color_amount = res_composed_color_amount;
                int each_lane_execute_count = 0;
                put_color.reset();

                FOR (i,0,config.lane_count) {
                    const auto ci = candidate_indices[i];
                    const auto amount = amounts[i][ci];
                    const auto add_amount = add_amounts[i][ci];
                    const auto& color = add_colors[i][ci];
                    put_count += puts[i][ci];
                    total_composed_color += color;
                    total_amount += amount;
                    each_lane_execute_count += execute_counts[i][ci];
                    total_res_composed_color_amount -= add_amount;
                    put_color += put_colors[i][ci];
                }

                composed_color = total_composed_color / (total_amount + total_res_composed_color_amount);

                auto diff = calculate_diff(composed_color, target_color);
                if (next_is_last && add_count+put_count <= input.H) {
                    res_color = total_color + put_color - composed_color;
                    diff = (diff + calculate_diff(res_color, next_target_color));
                }
                auto score = calculate_score(put_count, diff, total_amount + total_res_composed_color_amount);

                const int expected_execute_count = each_lane_execute_count + 1;

                i_candidates.clear();
                j_candidates.clear();
                FOR (i,0,config.lane_count) {
                    if (candidate_indices[i] > 0) {
                        i_candidates.add(i);
                    }
                    if (candidate_indices[i] < amounts[i].size() - 1) {
                        j_candidates.add(i);
                    }
                }

                best_score = score;
                best_diff = diff;
                best_execute_count = expected_execute_count;
                best_composed_color = composed_color;
                best_composed_color_amount = total_amount + total_res_composed_color_amount;

                const double start_ = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
                FOR (i,0,SA_LOOP) {
                    const double start_partial_time = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
                    const double time_ = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() - start_;
                    const auto res_time = max_time - time_;
                    const auto max_time_ = res_time / (double)(SA_LOOP - i);
                    double time = 0;

                    while (true) {
                        if (best_score < score_threshold) {
                            break;
                        }
                        if (iter%100 == 0) {
                            time = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() - start_partial_time;
                            if (time > max_time_) {
                                break;
                            }
                        }
                        ++iter;

                        const int i = i_candidates.que[randrange(i_candidates.que_size)];
                        int j = j_candidates.que[randrange(j_candidates.que_size)];
                        while (i == j) {
                            j = j_candidates.que[randrange(j_candidates.que_size)];
                        }

                        const auto& amounts_i = amounts[i];
                        const auto& add_amounts_i = add_amounts[i];
                        const auto& execute_counts_i = execute_counts[i];
                        const auto& puts_i = puts[i];
                        const auto& add_colors_i = add_colors[i];

                        const auto& amounts_j = amounts[j];
                        const auto& add_amounts_j = add_amounts[j];
                        const auto& execute_counts_j = execute_counts[j];
                        const auto& puts_j = puts[j];
                        const auto& add_colors_j = add_colors[j];
                        const auto& min_execute_counts_j = min_execute_counts[j];

                        const auto ci = candidate_indices[i];
                        const auto amount_i = amounts_i[ci];
                        const auto add_amount_i = add_amounts_i[ci];

                        const auto cj = candidate_indices[j];
                        const auto amount_j = amounts_j[cj];
                        const auto add_amount_j = add_amounts_j[cj];

                        const int nci = randrange(min((int)amounts_i.size(), ci+2));

                        const auto new_amount_i = amounts_i[nci];
                        const auto new_add_amount_i = add_amounts_i[nci];

                        const auto temp_total_amount = total_amount + new_amount_i - amount_i - amount_j;
                        const auto temp_total_res_composed_color_amount = total_res_composed_color_amount - new_add_amount_i + add_amount_i + add_amount_j;
                        const auto lack_amount = 1.0 - ERR - temp_total_amount;  // - temp_total_res_composed_color_amount;

                        const auto temp_put_count = put_count
                                                    + puts_i[nci] - puts_i[ci]
                                                    - puts_j[cj];

                        temp_total_composed_color = total_composed_color
                                                    + add_colors_i[nci] - add_colors_i[ci]
                                                    - add_colors_j[cj];

                        const int temp_each_lane_execute_count = each_lane_execute_count
                                                    + execute_counts_i[nci] - execute_counts_i[ci]
                                                    - execute_counts_j[cj];

                        if (next_is_last) {
                            temp_put_color = put_color
                                                    + put_colors[i][nci] - put_colors[i][ci]
                                                    - put_colors[j][cj];
                        }

                        const auto max_each_lane_execute_count = max_execute_count - temp_each_lane_execute_count - 1;

                        const auto temperature = score * INITIAL_TEMPERATURE_COEF * (max_time_ - time) / max_time_;
                        const double update_threshold = score - temperature * choice(logs.table);
                        double min_score = update_threshold;
                        double min_diff = 0;
                        int arg_min = -1;
                        // const auto target_diff_color = target_color * (total_amount + total_res_composed_color_amount) - temp_total_composed_color;
                        // const auto& color_j = input.tubes[j] - target_color;
                        // const auto optimal_amount = inner_product(target_diff_color, color_j) / inner_product(color_j, color_j);
                        // cerr << optimal_amount << "\n";
                        const auto max_add_amount = config.max_add_amount - temp_total_amount;

                        FOR (ncj, max(0, cj-1), amounts_j.size()) {
                            if (min_execute_counts_j[ncj] > max_each_lane_execute_count) {
                                break;
                            }

                            if (execute_counts_j[ncj] > max_each_lane_execute_count || amounts_j[ncj] < lack_amount) {
                                continue;
                            }

                            const auto new_amount_j = amounts_j[ncj];
                            const auto new_add_amount_j = add_amounts_j[ncj];

                            if (new_amount_j - new_add_amount_j >= max_add_amount) {
                                break;
                            }
                            const auto new_total_res_composed_color_amount = temp_total_res_composed_color_amount - new_add_amount_j;
                            const auto new_total_amount = temp_total_amount + new_amount_j + new_total_res_composed_color_amount;
                            const auto new_put_count = temp_put_count + puts_j[ncj];

                            composed_color = (temp_total_composed_color + add_colors_j[ncj]) / new_total_amount;
                            auto diff = calculate_diff(composed_color, target_color);
                            if (next_is_last && add_count+new_put_count <= input.H) {
                                res_color = total_color + temp_put_color + put_colors[j][ncj] - composed_color;
                                diff = (diff + calculate_diff(res_color, next_target_color));
                            }
                            if (diff >= min_score) {
                                continue;
                            }
                            const auto new_score = calculate_score(new_put_count, diff, new_total_amount);
                            if (new_score < min_score) {
                                min_score = new_score;
                                min_diff = diff;
                                arg_min = ncj;
                            }
                        }

                        if (arg_min < 0) {
                            continue;
                        }

                        const auto ncj = arg_min;

                        ++update;
                        score = min_score;
                        candidate_indices[i] = nci;
                        candidate_indices[j] = ncj;
                        put_count = temp_put_count + puts_j[ncj];
                        total_composed_color = temp_total_composed_color + add_colors_j[ncj];
                        each_lane_execute_count = temp_each_lane_execute_count + execute_counts_j[ncj];
                        total_amount = temp_total_amount + amounts_j[ncj];
                        total_res_composed_color_amount = temp_total_res_composed_color_amount - add_amounts_j[ncj];
                        if (next_is_last) {
                            put_color = temp_put_color + put_colors[j][ncj];
                        }

                        i_candidates.remove(i);
                        j_candidates.remove(i);
                        i_candidates.remove(j);
                        j_candidates.remove(j);

                        if (nci > 0) {
                            i_candidates.add(i);
                        }
                        if (nci < amounts[i].size() - 1) {
                            j_candidates.add(i);
                        }
                        if (ncj > 0) {
                            i_candidates.add(j);
                        }
                        if (ncj < amounts[j].size() - 1) {
                            j_candidates.add(j);
                        }

                        if (score < best_score) {
                            ++best_update;
                            best_score = score;
                            best_indices = candidate_indices;
                            best_diff = min_diff;
                            best_execute_count = each_lane_execute_count + 1;
                            best_composed_color = total_composed_color / (total_amount + total_res_composed_color_amount);
                            best_composed_color_amount = total_amount + total_res_composed_color_amount;
                        }
                    }
                }
                if (!initial_ans_as_merge_all && best_update == 0) {
                    best_score = -1;
                }
            }

            int best_index = -1;
            FOR (i,0,patterns.size()) {
                if (next_is_last) {
                    break;
                }
                const auto& p = patterns[i];
                if (complete_target_count + p.size() > input.H) {
                    continue;
                }

                double total_diff = 0.0;
                auto composed_color = res_composed_color;
                auto composed_color_amount = res_composed_color_amount;
                const int max_turn = p.size();
                int index = 0;
                FOR (j,0,max_turn) {
                    while (index < p.size() && p[index].first == j) {
                        const auto i = p[index].second;
                        const auto add_color = input.tubes[i];
                        composed_color = (composed_color * composed_color_amount + add_color) / (composed_color_amount + 1.0);
                        composed_color_amount += 1.0;
                        ++index;
                    }
                    const auto& target = input.targets[j + complete_target_count];
                    const auto diff = calculate_diff(composed_color, target);
                    total_diff += diff;
                    composed_color_amount -= 1.0;
                }

                const double total_amount = 1.0;
                const int put_count = max_turn;
                const int expected_execute_count = 2 * put_count;

                const auto score = calculate_score(put_count, total_diff, total_amount);

                if (best_score < 0 || score < best_score * (double)max_turn) {
                    best_score = score / (double)max_turn;
                    best_diff = total_diff;
                    best_index = i;
                    best_execute_count = expected_execute_count;
                    best_composed_color = composed_color;
                    best_composed_color_amount = composed_color_amount;
                }
            }

            if (best_index >= 0) {
                make_color_directly(patterns[best_index]);
                complete_target_count += patterns[best_index].size();
                res_composed_color_amount = max(0.0, best_composed_color_amount);
                res_composed_color = best_composed_color;
            } else {
                total_success_score += best_score;
                ++success_count;
                FOR (i,0,config.lane_count) {
                    best_amounts[i] = amounts[i][best_indices[i]];
                    best_counts[i] = counts[i][best_indices[i]];
                    best_caps[i] = caps[i][best_indices[i]];
                    best_puts[i] = puts[i][best_indices[i]];
                    now_colors[i] = lane_colors[i][best_indices[i]];
                }

                make_color_from_amounts(composed_color_cap, res_composed_color_amount, now_amounts, now_caps, best_amounts, best_counts, best_caps, best_puts, tube_indices);
                FOR (i,0,config.lane_count) {
                    if (is_first[i] && best_counts[i] > 0) {
                        is_first[i] = false;
                    }
                }
                res_composed_color_amount = max(0.0, best_composed_color_amount - 1.0);
                res_composed_color = best_composed_color;
                // cerr << complete_target_count << "\n";
                // cerr << "total" << " " << best_composed_color_amount << " " << res_composed_color_amount << "\n";
                // FOR (i,0,config.lane_count) {
                //     cerr << i << " " << now_amounts[i] << " " << now_caps[i] << "\n";
                // }
                ++complete_target_count;
            }

            total_iter += iter;
            total_update += update;
            total_best_update += best_update;
            total_score += best_score;
            total_diff += best_diff;
            total_expected_execute_count += best_execute_count;
        }
        cerr << setprecision(9);
        cerr << "iter: " << total_iter << "\n";
        cerr << "update: " << total_update << "\n";
        cerr << "best_update: " << total_best_update << "\n";
        cerr << "diff: " << total_diff << "\n";
        cerr << "score: " << total_score << "\n";
        cerr << "execute: " << total_expected_execute_count << " " << execute_count << " / " << input.T << "\n";
    }
};


int main() {
    Solver solver;
    solver.solve();
    return 0;
}