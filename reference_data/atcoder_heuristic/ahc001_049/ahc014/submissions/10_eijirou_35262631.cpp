// 800 cases average score: 1386066

#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <bits/stdc++.h>
//#include <atcoder/all>
using namespace std;
//using namespace atcoder;

template <class T>
T sum(const vector<T>& data) {
    return accumulate(data.begin(), data.end(), (T)0);
}

template <class T>
int index(const vector<T>& data, T x) {
    return distance(data.begin(), find(data.begin(),data.end(),x));
}

template <class T>
bool contain(const vector<T>& data, T x) {
    return !(index(data, x) == (int)data.size());
}

template <class T>
int count(const vector<T>& data, T x) {
    int cnt = 0;
    for (auto y : data) {
        if (x == y) {
            cnt++;
        }
    }
    return cnt;
}

template <class T>
int bisect_left(const vector<T>& data, T x) {
    // binary search
    int left = 0;
    int right = (int)data.size();
    while (left < right) {
        int center = (left + right) / 2;
        if (data[center] < x) {
            left = center + 1;
        } else {
            right = center;
        }
    }
    return left;
}

struct Randomizer {
    mt19937_64 engine;
    uniform_int_distribution<> dist = uniform_int_distribution<>(0, INT_MAX);

    Randomizer() {
        random_device seed_gen;
        engine = mt19937_64(seed_gen());
    }

    Randomizer(int seed) {
        engine = mt19937_64(seed);
    }

    double uniform(double l, double r) {
        assert(l <= r);
        return l + (r-l)*dist(engine)/INT_MAX;
    }

    int randrange(int l, int r) {
        assert(l < r);
        return l + dist(engine)%(r-l);
    }

    template <class T>
    T choice(const vector<T>& data) {
        return data.at(randrange(0, data.size()));
    }

    vector<int> sample(int l, int r, int num) {
        assert(0 < num && num <= r - l);
        vector<int> ret;
        while ((int)ret.size() < num) {
            int x = randrange(l, r);
            if (!contain(ret, x)) {
                ret.push_back(x);
            }
        }
        sort(ret.begin(), ret.end());
        return ret;
    }

    template <class T>
    void shuffle_vector(vector<T>& data) {
        shuffle(data.begin(), data.end(), engine);
    }
};

struct Timer {
    timespec start;
    double lapse;
    Randomizer randomizer;

    Timer() {}

    Timer(int seed) {
        randomizer = Randomizer(seed);
    }

    void begin() {
        clock_gettime(CLOCK_REALTIME, &start);
    }

    double stopwatch() {
        timespec end;
        clock_gettime(CLOCK_REALTIME, &end);
        double sec = end.tv_sec - start.tv_sec;
        double nsec = end.tv_nsec - start.tv_nsec;
        lapse = sec + nsec/1000000000;
        return lapse;
    }

    bool scheduler(double delta, double time_limit, double t0, double t1) {
        assert(0.0 < time_limit && 0.0 <= t1 && t1 <= t0);
        if (0.0 <= delta) {
            return true;
        } else {
            double ratio = lapse / time_limit;
            double t = t0 * (1.0 - ratio) + t1 * ratio;
            return randomizer.uniform(0.0, 1.0) < pow(2.0, delta/t);
        }
    }
};

constexpr double time_limit = 4.9;
constexpr array<pair<int,int>, 8> dxdy = {
    make_pair(1, 0),
    make_pair(1, 1),
    make_pair(0, 1),
    make_pair(-1, 1),
    make_pair(-1, 0),
    make_pair(-1, -1),
    make_pair(0, -1),
    make_pair(1, -1)
};

inline bool in_grid(int n, int x, int y) {
    return 0 <= x && x < n && 0 <= y && y < n;
}

inline int get_direction(int sx, int sy, int tx, int ty) {
    if (sx == tx) {
        if (sy < ty) {
            return 2;
        } else {
            return 6;
        }
    } else if (sx < tx) {
        if (sy == ty) {
            return 0;
        } else if (sy < ty) {
            return 1;
        } else {
            return 7;
        }
    } else {
        if (sy == ty) {
            return 4;
        } else if (sy < ty) {
            return 3;
        } else {
            return 5;
        }
    }
}

struct Input {
    int n, m;
    vector<pair<int,int>> xy;

    void input() {
        cin >> n >> m;
        xy.resize(m);
        for (int i = 0; i < m; i++) {
            cin >> xy[i].first >> xy[i].second;
        }
    }

    void generate(int seed) {
        Randomizer randomizer(seed);
        n = (15 + seed % 16) * 2 + 1;
        m = randomizer.randrange(n, n * n / 12 + 1);
        int l = n / 4;
        int r = 3 * n / 4 + 1;
        vector<int> sampled = randomizer.sample(0, (r - l) * (r - l), m);
        xy.resize(m);
        for (int i = 0; i < m; ++i) {
            xy[i].first = l + sampled[i] / (r - l);
            xy[i].second = l + sampled[i] % (r - l);
        }
    }

    void print() {
        cout << n << ' ' << m << '\n';
        for (auto [x, y] : xy) {
            cout << x << ' ' << y << '\n';
        }
    }
};

struct Checker {
    int n;
    int m;
    vector<vector<bool>> marked;
    vector<vector<vector<bool>>> edges;

    long long check(const Input& input, const vector<array<pair<int,int>, 4>>& answer) {
        n = input.n;
        m = input.m;
        marked = vector<vector<bool>>(n, vector<bool>(n, false));
        edges = vector<vector<vector<bool>>>(n, vector<vector<bool>>(n, vector<bool>(8, false)));
        for (auto [x, y] : input.xy) {
            marked[x][y] = true;
        }
        for (const array<pair<int,int>, 4>& rect : answer) {
            for (int i = 0; i < 4; ++i) {
                auto [sx, sy] = rect[i];
                auto [tx, ty] = rect[(i + 1) % 4];
                if (i == 0) {
                    assert(!marked[sx][sy]);
                    marked[sx][sy] = true;
                } else {
                    assert(marked[sx][sy]);
                }
                int d = get_direction(sx, sy, tx, ty);
                auto [dx, dy] = dxdy[d];
                int x = sx;
                int y = sy;
                while (!(x == tx && y == ty)) {
                    assert(in_grid(n, x, y));
                    assert(!edges[x][y][d]);
                    edges[x][y][d] = true;
                    x += dx;
                    y += dy;
                    assert(!edges[x][y][(d + 4) % 8]);
                    edges[x][y][(d + 4) % 8] = true;
                }
            }
        }
        return calculate_score();
    }

    long long calculate_score() {
        long long s = 0;
        long long score = 0;
        int c = n / 2;
        for (int x = 0; x < n; x++) {
            for (int y = 0; y < n; y++) {
                long long w = (x - c) * (x - c) + (y - c) * (y - c) + 1;
                s += w;
                if (marked[x][y]) {
                    score += w;
                }
            }
        }
        score *= 1000000ll * n * n;
        score += (m * s + 1) / 2;
        score /= m * s;
        return score;
    }
};

Randomizer randomizer(0);

struct Action {
    array<pair<int,int>, 4> rect;
    int evaluation;

    Action(const array<pair<int,int>, 4>& _rect, int _evaluation) {
        rect = _rect;
        evaluation = _evaluation;
    }
};

bool operator<(const Action& a, const Action& b) {
    return a.evaluation < b.evaluation;
}

constexpr pair<int,int> none = {INT_MIN, INT_MIN};

struct State {
    int n, c;
    int marks = 0;
    vector<vector<bool>> marked;
    vector<vector<int>> locks;
    vector<vector<array<pair<int,int>, 8>>> edges;
    vector<vector<array<pair<int,int>, 4>>> rectangulars;
    priority_queue<Action> actions;
    vector<pair<int,int>> revert_points;
    set<pair<int,int>> unlocked_marks;
    int phase;
    long long score = 0;

    State() {}

    State(const Input& input, int _phase) {
        // initialize n
        n = input.n;

        // initialize c
        c = (n - 1) / 2;

        // initialize marked
        marked = vector<vector<bool>>(n, vector<bool>(n, false));
        for (auto [x, y] : input.xy) {
            marked[x][y] = true;
        }
        // initialize locks
        locks = vector<vector<int>>(n, vector<int>(n, 0));
        for (auto [x, y] : input.xy) {
            locks[x][y] = 1;
        }
        // initialize edges
        edges = vector<vector<array<pair<int,int>, 8>>>(n, vector<array<pair<int,int>, 8>>(n, {none,none,none,none,none,none,none,none}));
        for (auto [x, y] : input.xy) {
            for (int d = 0; d < 8; ++d) {
                fill(x, y, d, {x, y});
            }
        }
        // initialize rectangulars
        rectangulars = vector<vector<array<pair<int,int>, 4>>>(n, vector<array<pair<int,int>, 4>>(n));

        // initialize actions
        for (auto [x, y] : input.xy) {
            for (int d = 0; d < 8; ++d) {
                push_action(x, y, d);
            }
        }
        // initialize phase
        assert(0 <= _phase && _phase < 256);
        phase = _phase;

        // initialize score
        for (auto [x, y] : input.xy) {
            score += (x - c) * (x - c) + (y - c) * (y - c) + 1;
        }
    }

    pair<int,int> fill(int x, int y, int d, const pair<int,int> v) {
        auto [dx, dy] = dxdy[d];
        x += dx;
        y += dy;
        while (in_grid(n, x, y)) {
            edges[x][y][(d + 4) % 8] = v;
            if (marked[x][y]) {
                break;
            }
            x += dx;
            y += dy;
        }
        return {x, y};
    }

    bool push_action(int x, int y, int d) {
        assert(marked[x][y]);
        if (edges[x][y][(d + 2) % 8].first < 0 || edges[x][y][d].first < 0) {
            return false;
        }
        auto [x2, y2] = edges[x][y][(d + 2) % 8];
        int x3 = x;
        int y3 = y;
        auto [x4, y4] = edges[x][y][d];
        int x1 = x2 + x4 - x3;
        int y1 = y2 + y4 - y3;
        if (in_grid(n, x1, y1) && !marked[x1][y1] && edges[x1][y1][(d + 4) % 8] == make_pair(x2, y2) && edges[x1][y1][(d + 6) % 8] == make_pair(x4, y4)) {
            const array<pair<int,int>, 4> rect = {make_pair(x1, y1), make_pair(x2, y2), make_pair(x3, y3), make_pair(x4, y4)};
            Action action(rect, evaluate(rect));
            actions.push(action);
            return true;
        }
        return false;
    }

    bool can_push_action(int x, int y, int d) {
        if (edges[x][y][(d + 2) % 8].first < 0 || edges[x][y][d].first < 0) {
            return false;
        }
        auto [x2, y2] = edges[x][y][(d + 2) % 8];
        int x3 = x;
        int y3 = y;
        auto [x4, y4] = edges[x][y][d];
        int x1 = x2 + x4 - x3;
        int y1 = y2 + y4 - y3;
        if (in_grid(n, x1, y1) && !marked[x1][y1] && edges[x1][y1][(d + 4) % 8] == make_pair(x2, y2) && edges[x1][y1][(d + 6) % 8] == make_pair(x4, y4)) {
            return true;
        }
        return false;
    }

    inline bool sandwiched(int sx, int sy, int x, int y, int tx, int ty) {
        return min(sx, tx) <= x && x <= max(sx, tx) && min(sy, ty) <= y && y <= max(sy, ty);
    }

    int get_bonus(const array<pair<int,int>, 4>& rect) {
        auto [x1, y1] = rect[0];
        auto [x2, y2] = rect[1];
        int dr = get_direction(x1, y1, x2, y2);
        int ret = 0;
        // push actions
        for (int d = 0; d < 8; ++d) {
            if (d == dr || d == (dr + 2) % 8 || d == (dr + 6) % 8) {
                continue;
            }
            if (can_push_action(x1, y1, d)) {
                ++ret;
            }
        }
        for (int d = 0; d < 8; ++d) {
            if (edges[x1][y1][d].first < 0 || d == dr || d == (dr + 2) % 8) {
                continue;
            }
            auto [xd, yd] = edges[x1][y1][d];
            if (d != (dr + 6) % 8 && 0 <= edges[xd][yd][(d + 2) % 8].first) {
                auto [xd2, yd2] = edges[xd][yd][(d + 2) % 8];
                int x = xd2 + x1 - xd;
                int y = yd2 + y1 - yd;
                if (in_grid(n, x, y) && !marked[x][y]) {
                    auto [xd6, yd6] = edges[x][y][(d + 6) % 8];
                    if (edges[x][y][(d + 6) % 8] == none || (0 <= xd6 && !sandwiched(x1, y1, x, y, xd2, yd2))) {
                        ++ret;
                    }
                }
            }
            if (d != (dr + 4) % 8 && 0 <= edges[xd][yd][(d + 4) % 8].first) {
                auto [xd6, yd6] = edges[xd][yd][(d + 6) % 8];
                int x = xd6 + x1 - xd;
                int y = yd6 + y1 - yd;
                if (in_grid(n, x, y) && !marked[x][y]) {
                    auto [xd2, yd2] = edges[x][y][(d + 2) % 8];
                    if (edges[x][y][(d + 2) % 8] == none || (0 <= xd2 && !sandwiched(x1, y1, x, y, xd6, yd6))) {
                        ++ret;
                    }
                }
            }
        }
        return ret;
    }

    int evaluate(const array<pair<int,int>, 4>& rect) {
        auto [x1, y1] = rect[0];
        auto [x2, y2] = rect[1];
        auto [x3, y3] = rect[2];
        int d = get_direction(x1, y1, x2, y2);
        auto [dx, dy] = dxdy[d];
        if (!(x1 + dx == x2 && y1 + dy == y2 && x2 - dy == x3 && y2 + dx == y3)) {
            int rect_size = max(abs(x1 - x2), abs(y1 - y2)) + max(abs(x2 - x3), abs(y2 - y3));
            return randomizer.randrange(-10*rect_size, 0);
        }
        auto [x, y] = *min_element(rect.begin(), rect.end());
        int group = get_group(x, y);
        if (d % 2 == 0) {
            if ((x + y + (phase >> 2*group)) % 2 == 0) {
                return 0;
            } else {
                return randomizer.randrange(-20, 0);
            }
        } else if (group % 2 == 0) {
            if ((x + (phase >> (2 * group + 1))) % 2 == 0) {
                return 0;
            } else {
                return randomizer.randrange(-20, 0);
            }
        } else {
            if ((y + (phase >> (2 * group + 1))) % 2 == 0) {
                return 0;
            } else {
                return randomizer.randrange(-20, 0);
            }
        }
    }

    inline int get_group(int x, int y) {
        //   2
        // 3   1
        //   0
        if (x - y < 0) {
            if (x + y < 0) {
                return 3;
            } else {
                return 2;
            }
        } else {
            if (x + y < 0) {
                return 0;
            } else {
                return 1;
            }
        }
    }

    void join(const array<pair<int,int>, 4>& rect) {
        auto [x, y] = rect.front();

        // update marks
        ++marks;

        // update marked
        assert(!marked[x][y]);
        marked[x][y] = true;

        // update unlocked_marks
        unlocked_marks.insert({x, y});

        // update locks
        for (int i = 1; i < 4; ++i) {
            auto [xi, yi] = rect[i];
            if (locks[xi][yi]++ == 0) {
                unlocked_marks.erase({xi, yi});
            }
        }
        // update edges
        for (int d = 0; d < 8; ++d) {
            if (none.first < edges[x][y][d].first && edges[x][y][d].first < 0) {
                // if (x, y) is on the ohter rectangular, update locks.
                auto [xd, yd] = edges[x][y][d];
                xd = - xd - 1;
                yd = - yd - 1;
                if (locks[xd][yd]++ == 0) {
                    unlocked_marks.erase({xd, yd});
                }
            } else {
                fill(x, y, d, {x, y});
            }
        }
        for (int i = 0; i < 4; ++i) {
            auto [xi, yi] = rect[i];
            auto [xj, yj] = rect[(i + 1) % 4];
            int d = get_direction(xi, yi, xj, yj);
            fill(xi, yi, d, {-x-1, -y-1});
            fill(xj, yj, (d + 4) % 8, {-x-1, -y-1});
        }
        // update rectangulars
        rectangulars[x][y] = rect;

        // update score
        score += (x - c) * (x - c) + (y - c) * (y - c) + 1;

        // push actions
        for (int d = 0; d < 8; ++d) {
            push_action(x, y, d);
        }
        for (int d = 0; d < 8; ++d) {
            if (edges[x][y][d].first < 0) {
                continue;
            }
            auto [xd, yd] = edges[x][y][d];
            push_action(xd, yd, (d + 2) % 8);
            push_action(xd, yd, (d + 4) % 8);
        }
    }

    void revert(int x, int y) {
        // assert we can revert (x, y)
        assert(locks[x][y] == 0);

        const array<pair<int,int>, 4> rect = rectangulars[x][y];

        // update score
        score -= (x - c) * (x - c) + (y - c) * (y - c) + 1;

        // update unlocked_marks
        unlocked_marks.erase({x, y});

        // update edges
        for (int i = 0; i < 4; ++i) {
            auto [xi, yi] = rect[i];
            auto [xj, yj] = rect[(i + 1) % 4];
            int d = get_direction(xi, yi, xj, yj);
            fill(xi, yi, d, {xi, yi});
            fill(xj, yj, (d + 4) % 8, {xj, yj});
        }
        for (int d = 0; d < 8; ++d) {
            if (none.first < edges[x][y][d].first && edges[x][y][d].first < 0) {
                // if (x, y) is on the ohter rectangular, update locks.
                auto [xd, yd] = edges[x][y][d];
                xd = - xd - 1;
                yd = - yd - 1;
                if (--locks[xd][yd] == 0) {
                    unlocked_marks.insert({xd, yd});
                    revert_points.push_back({xd, yd});
                }
            } else {
                fill(x, y, d, edges[x][y][(d + 4) % 8]);
            }
        }
        // update locks
        for (int i = 1; i < 4; ++i) {
            auto [xi, yi] = rect[i];
            if (--locks[xi][yi] == 0) {
                unlocked_marks.insert({xi, yi});
                revert_points.push_back({xi, yi});
            }
        }
        // update marked
        assert(marked[x][y]);
        marked[x][y] = false;

        // update marks
        --marks;

        // update actions
        auto [x1, y1] = rect[0];
        auto [x2, y2] = rect[1];
        auto [x3, y3] = rect[2];
        auto [x4, y4] = rect[3];
        int d = get_direction(x1, y1, x2, y2);
        push_action(x2, y2, d);
        push_action(x3, y3, (d + 2) % 8);
        push_action(x3, y3, (d + 6) % 8);
        push_action(x4, y4, d % 8);
        for (int d = 0; d < 8; ++d) {
            auto [xd, yd] = edges[x][y][d];
            if (xd < 0) {
                continue;
            }
            push_action(xd, yd, (d + 2) % 8);
            auto [xdd2, ydd2] = edges[xd][yd][(d + 2) % 8];
            if (0 <= xdd2) {
                push_action(xdd2, ydd2, (d + 4) % 8);
            }
            push_action(xd, yd, (d + 4) % 8);
            auto [xdd6, ydd6] = edges[xd][yd][(d + 6) % 8];
            if (0 <= xdd6) {
                push_action(xdd6, ydd6, (d + 2) % 8);
            }
        }
    }

    inline bool can_act(const array<pair<int,int>, 4>& rect) {
        auto [x1, y1] = rect[0];
        auto [x2, y2] = rect[1];
        auto [x3, y3] = rect[2];
        auto [x4, y4] = rect[3];
        int d = get_direction(x1, y1, x2, y2);
        return !marked[x1][y1] &&
               marked[x2][y2] &&
               marked[x3][y3] &&
               marked[x4][y4] &&
               edges[x1][y1][d] == make_pair(x2, y2) &&
               edges[x1][y1][(d + 2) % 8] == make_pair(x4, y4) &&
               edges[x3][y3][(d + 6) % 8] == make_pair(x2, y2) &&
               edges[x3][y3][(d + 4) % 8] == make_pair(x4, y4);
    }

    void update_actions() {
        while (!actions.empty() && !can_act(actions.top().rect)) {
            actions.pop();
        }
    }

    array<pair<int,int>, 4> choose_action() {
        constexpr int lookahead = 4;
        Action best_action = actions.top();
        actions.pop();
        int max_bonus = get_bonus(best_action.rect);
        vector<Action> other_actions;
        other_actions.reserve(lookahead - 1);
        for (int i = 0; i < lookahead - 1; ++i) {
            update_actions();
            if (actions.empty()) {
                break;
            }
            Action action = actions.top();
            actions.pop();
            int bonus = get_bonus(action.rect);
            if (max_bonus < bonus) {
                other_actions.push_back(best_action);
                best_action = action;
                max_bonus = bonus;
            } else {
                other_actions.push_back(action);
            }
        }
        for (Action& action : other_actions) {
            actions.push(action);
        }
        return best_action.rect;
    }

    vector<pair<int,int>> random_greedy() {
        vector<pair<int,int>> ret;
        update_actions();
        while (!actions.empty()) {
            const array<pair<int,int>, 4> rect = choose_action();
            join(rect);
            ret.push_back(rect.front());
            update_actions();
        }
        return ret;
    }

    vector<array<pair<int,int>, 4>> range_revert(int x_min, int x_max, int y_min, int y_max, int revert_limit) {
        assert(0 <= x_min && x_min < x_max && x_max <= n);
        assert(0 <= y_min && y_min < y_max && y_max <= n);
        for (auto [x, y] : unlocked_marks) {
            if (x_min <= x && x < x_max && y_min <= y && y < y_max) {
                revert_points.push_back({x, y});
            }
        }
        randomizer.shuffle_vector(revert_points);
        vector<array<pair<int,int>, 4>> ret;
        while (!revert_points.empty() && revert_limit--) {
            auto [x, y] = revert_points.back();
            revert_points.pop_back();
            ret.push_back(rectangulars[x][y]);
            revert(x, y);
        }
        revert_points.clear();
        reverse(ret.begin(), ret.end());
        return ret;
    }

    vector<array<pair<int,int>, 4>> make_answer() {
        return range_revert(0, n, 0, n, n * n);
    }

    void all_join(const vector<array<pair<int,int>, 4>>& all_actions) {
        for (const array<pair<int,int>, 4>& action : all_actions) {
            join(action);
        }
        actions = priority_queue<Action>();
    }

    void all_revert(const vector<pair<int,int>>& all_points) {
        for (auto [x, y] : all_points) {
            revert(x, y);
        }
        revert_points.clear();
    }
};

bool operator<(const State& a, const State& b) {
    return a.score < b.score;
}

struct Solver {
    Timer timer;
    int n;
    vector<State> best_states;
    vector<State> states;
    vector<array<pair<int,int>, 4>> answer;

    inline tuple<int,int,int,int> random_range() {
        int x1 = n * randomizer.randrange(0, 2);
        int y1 = n * randomizer.randrange(0, 2);
        int x2, y2;
        if (x1 == 0) {
            x2 = randomizer.randrange(n / 4, n + 1);
        } else {
            x2 = randomizer.randrange(0, 3 * n / 4 + 1);
        }
        if (y1 == 0) {
            y2 = randomizer.randrange(n / 4, n + 1);
        } else {
            y2 = randomizer.randrange(0, 3 * n / 4 + 1);
        }
        return {min(x1, x2), max(x1, x2), min(y1, y2), max(y1, y2)};
    }

    void select(int selects) {
        vector<pair<long long,int>> data(best_states.size());
        for (int i = 0; i < (int)best_states.size(); ++i) {
            data[i] = {-best_states[i].score, i};
        }
        sort(data.begin(), data.end());
        vector<State> new_states(selects);
        for (int i = 0; i < selects; ++i) {
            new_states[i] = best_states[data[i].second];
        }
        best_states = states = new_states;
    }

    void improve(int i, double t0, double t1) {
        const long long old_score = states[i].score;
        auto [x_min, x_max, y_min, y_max] = random_range();
        const int max_limit = states[i].marks * (x_max - x_min) * (y_max - y_min) / (n * n);
        vector<array<pair<int,int>, 4>> destroyed_rectangulars = states[i].range_revert(x_min, x_max, y_min, y_max, randomizer.randrange(10, max(11, max_limit)));
        vector<pair<int,int>> added_points = states[i].random_greedy();
        if (timer.scheduler(states[i].score - old_score, time_limit, t0, t1)) {
            if (best_states[i].score < states[i].score) {
                best_states[i] = states[i];
            }
        } else {
            reverse(added_points.begin(), added_points.end());
            states[i].all_revert(added_points);
            states[i].all_join(destroyed_rectangulars);
        }
    }

    void solve(const Input& input) {
        timer.begin();
        n = input.n;

        best_states.reserve(256);
        for (int phase = 0; phase < 256; ++phase) {
            best_states.emplace_back(State(input, phase));
            best_states.back().random_greedy();
        }
        double tl = 0.0;
        for (int selects = 16; 0 < selects; selects /= 2) {
            tl += time_limit / 5.0;
            select(selects);
            int i = 0;
            while (timer.stopwatch() < tl) {
                improve(i, 1000.0, 100.0);
                ++i;
                i %= selects;
            }
        }
        answer = best_states.front().make_answer();
    }

    void print() {
        cout << answer.size() << '\n';
        for (const array<pair<int,int>, 4>& rect : answer) {
            for (int i = 0; i < 4; ++i) {
                cout << rect[i].first << ' ' << rect[i].second;
                if (i == 3) {
                    cout << '\n';
                } else {
                    cout << ' ';
                }
            }
        }
    }
};

struct Tester {
    vector<long long> scores = vector<long long>(16, 0);
    vector<int> counts = vector<int>(16, 0);

    void test(int cases) {
        for (int seed = 0; seed < cases; ++seed) {
            Input input;
            input.generate(seed);
            Solver solver;
            solver.solve(input);
            Checker checker;
            long long score = checker.check(input, solver.answer);
            cerr << "Seed = " << seed << ", Score = " << score << '\n';
            int n = input.n / 2 - 15;
            scores[n] += score;
            counts[n]++;
        }
        for (int n = 0; n < 16; n++) {
            cerr << "N = " << 2 * n + 31 << ", Average Score = " << scores[n] / counts[n] << '\n';
        }
        cerr << "All Average Score = " << sum(scores) / sum(counts) << '\n';
    }
};

int main() {
    Input input;
    input.input();

    Solver solver;
    solver.solve(input);
    solver.print();

#ifndef ONLINE_JUDGE
    Checker checker;
    long long score = checker.check(input, solver.answer);
    cerr << "Score = " << score << '\n';
    Tester tester;
    tester.test(800);
#endif

    return 0;
}
