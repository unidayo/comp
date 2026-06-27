#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#define NDEBUG

#include <bits/stdc++.h>
// #include <atcoder/all>

#ifndef ONLINE_JUDGE
#include <omp.h>
#endif // ONLINE_JUDGE

using namespace std;
// using namespace atcoder;

#define ll long long int
#define ull unsigned long long int

// Library Section

template <class T>
inline T sum(const vector<T>& data) {
    return accumulate(data.begin(), data.end(), static_cast<T>(0));
}

template <class T>
inline int argmax(const vector<T>& data) {
    assert(!data.empty());
    return distance(data.begin(), max_element(data.begin(), data.end()));
}

template <class T>
inline int bisect_left(const vector<T>& data, const T x) {
    return distance(data.begin(), lower_bound(data.begin(), data.end(), x));
}

template <class T>
inline bool contain(const vector<T>& data, T x) {
    return find(data.begin(), data.end(), x) != data.end();
}

template <class T>
inline int index(const vector<T>& data, const T x) {
    return distance(data.begin(), find(data.begin(), data.end(), x));
}

class Xorshift {
    public:
        Xorshift(uint32_t seed) : x_(seed) {
            assert(seed);
        }

        uint32_t randrange(uint32_t stop) {
            // [0, stop)
            assert(stop > 0);
            next();
            return x_ % stop;
        }

        uint32_t randrange(uint32_t start, uint32_t stop) {
            // [start, stop)
            assert(start < stop);
            next();
            return start + x_ % (stop - start);
        }

        uint32_t randint(uint32_t a, uint32_t b) {
            // [a, b]
            assert(a <= b);
            return randrange(a, b + 1);
        }

        double random() {
            // [0.0, 1.0]
            next();
            return static_cast<double>(x_) * (1.0 / static_cast<double>(UINT32_MAX));
        }

        double uniform(double a, double b) {
            // [a, b] or [b, a]
            return a + (b - a) * random();
        }

    private:
        void next() {
            x_ ^= x_ << 13;
            x_ ^= x_ >> 17;
            x_ ^= x_ << 5;
        }

        uint32_t x_;
};

class Timer {
    public:
        Timer() {
            begin();
            elapsed_time_ = 0.0;
        }

        void begin() {
            start_time_ = chrono::system_clock::now();
        }

        double get_time() {
            chrono::system_clock::time_point end_time = chrono::system_clock::now();
            elapsed_time_ = chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time_).count();
            elapsed_time_ *= 1e-9; // nanoseconds -> seconds
            return elapsed_time_;
        }

        double get_last_time() const {
            return elapsed_time_;
        }

        bool yet(double time_limit) {
            return get_time() < time_limit;
        }

        double progress(double time_limit) {
            return get_time() / time_limit;
        }

        double get_last_progress(double time_limit) const {
            return get_last_time() / time_limit;
        }

    private:
        chrono::system_clock::time_point start_time_;
        double elapsed_time_;
};

// Input Section

#ifdef ONLINE_JUDGE
constexpr double time_limit = 1.97;
#else
constexpr double time_limit = 3.97;
#endif // ONLINE_JUDGE
constexpr size_t max_output_length = 100000;

struct Input {
    int n;
    vector<vector<bool>> h, v;
    vector<ll> d;

    void input() {
        cin >> n;

        h = vector<vector<bool>>(n - 1, vector<bool>(n));
        for (int x = 0; x < n - 1; ++x) {
            string row;
            cin >> row;
            assert(static_cast<int>(row.size()) == n);
            for (int y = 0; y < n; ++y) {
                h[x][y] = (row[y] == '1');
            }
        }
        v = vector<vector<bool>>(n, vector<bool>(n - 1));
        for (int x = 0; x < n; ++x) {
            string row;
            cin >> row;
            assert(static_cast<int>(row.size()) == n - 1);
            for (int y = 0; y < n - 1; ++y) {
                v[x][y] = (row[y] == '1');
            }
        }
        d = vector<ll>(n * n);
        for (int i = 0; i < n * n; ++i) {
            cin >> d[i];
        }
    }

    void input(const string& filename) {
        ifstream in(filename);
        assert(in);

        in >> n;

        h = vector<vector<bool>>(n - 1, vector<bool>(n));
        for (int x = 0; x < n - 1; ++x) {
            string row;
            in >> row;
            assert(static_cast<int>(row.size()) == n);
            for (int y = 0; y < n; ++y) {
                h[x][y] = (row[y] == '1');
            }
        }
        v = vector<vector<bool>>(n, vector<bool>(n - 1));
        for (int x = 0; x < n; ++x) {
            string row;
            in >> row;
            assert(static_cast<int>(row.size()) == n - 1);
            for (int y = 0; y < n - 1; ++y) {
                v[x][y] = (row[y] == '1');
            }
        }
        d = vector<ll>(n * n);
        for (int i = 0; i < n * n; ++i) {
            in >> d[i];
        }
    }
};

struct InputINFO {
    int n;
    vector<ll> d;
    vector<vector<int>> edges;
    ll sum_d;
    double wall_rate;
    double d_sd;

    InputINFO(const Input& input) {
        n = input.n;
        d = input.d;
        init_edges(input.h, input.v);
        sum_d = sum(d);
        init_wall_rate();
        init_d_sd();
    }

    void init_edges(const vector<vector<bool>>& h, const vector<vector<bool>>& v) {
        edges = vector<vector<int>>(n * n);
        for (int x = 0; x < n; ++x) {
            for (int y = 0; y < n; ++y) {
                int p = n * x + y;
                edges[p].reserve(4);
                
                // up
                if (x && !h[x - 1][y]) {
                    edges[p].push_back(p - n);
                }
                // left
                if (y && !v[x][y - 1]) {
                    edges[p].push_back(p - 1);
                }
                // right
                if (y < n - 1 && !v[x][y]) {
                    edges[p].push_back(p + 1);
                }
                // down
                if (x < n - 1 && !h[x][y]) {
                    edges[p].push_back(p + n);
                }
            }
        }
    }

    void init_wall_rate() {
        int walls = 0;
        for (int p = 0; p < n * n; ++p) {
            walls += 4 - edges[p].size();
        }
        wall_rate = static_cast<double>(walls) / (n * n);
    }

    void init_d_sd() {
        ll sum_d2 = 0;
        for (int p = 0; p < n * n; ++p) {
            sum_d2 += d[p] * d[p];
        }
        double mean_d2 = static_cast<double>(sum_d2) / (n * n);
        double mean_d = static_cast<double>(sum_d) / (n * n);
        d_sd = sqrt(mean_d2 - mean_d * mean_d);
    }
};

// Beam Search Section

struct BeamSearchConfig {
    ll max_turn;
    double dirtiness_coefficient;
    double cost_coefficient;
    double dxy_coefficient;
    double dxy_decay;
    double dxy_decay_2;
    int initial_time;
    double min_fixed_d;
    double fixed_exp;
    int start_position;
    size_t nodes_capacity;
    double beam_search_time_limit;

    BeamSearchConfig(const InputINFO& input_info) {
        max_turn = 2.7 * input_info.n * input_info.n;
        if (max_turn % 2) {
            ++max_turn;
        }
        dirtiness_coefficient = 20.0;
        cost_coefficient = 1.5e-2 - 2e-4 * (input_info.n - 20);
        dxy_coefficient = 2.0 - 2e-2 * (input_info.n - 20);
        dxy_decay = 0.95;
        dxy_decay_2 = 1.0 - dxy_decay;
        initial_time = -4000;
        min_fixed_d = 3.0 + 0.2 * (input_info.n - 20);
        fixed_exp = 0.83 + 2e-3 * (input_info.n - 20);
        start_position = argmax(input_info.d);
        nodes_capacity = 20000;
        beam_search_time_limit = 0.8 * time_limit;
    }
};

using Action = int;

struct Evaluation {
    double integrated_evaluation;
    ll accumulate_dirtiness;
    ll dirtiness;
    ll cost;
    double dx_average;
    double dy_average;

    Evaluation(ll accumulate_dirtiness, ll dirtiness, ll cost, double dx_average, double dy_average, const BeamSearchConfig& config) :
        accumulate_dirtiness(accumulate_dirtiness),
        dirtiness(dirtiness),
        cost(cost),
        dx_average(dx_average),
        dy_average(dy_average)
    {
        integrated_evaluation =
            accumulate_dirtiness +
            (config.dirtiness_coefficient - config.dxy_coefficient * (abs(dx_average) + abs(dy_average))) * dirtiness +
            config.cost_coefficient * cost;
    }

    Evaluation get_new_evaluation(ll sum_fixed_d, ll harvest, ll delta_cost, double new_dx_average, double new_dy_average, const BeamSearchConfig& config) const {
        ll new_dirtiness = dirtiness + sum_fixed_d - harvest;
        return Evaluation(accumulate_dirtiness + new_dirtiness, new_dirtiness, cost + delta_cost, new_dx_average, new_dy_average, config);
    }
};

bool operator<(const Evaluation& a, const Evaluation& b) {
    return a.integrated_evaluation < b.integrated_evaluation;
}

struct Candidate {
    Evaluation evaluation;
    int parent;

    Candidate(const Evaluation& evaluation, int parent) :
        evaluation(evaluation),
        parent(parent) {}
};

bool operator<(const Candidate& a, const Candidate& b) {
    return a.evaluation < b.evaluation;
}

inline int get_direction(int delta) {
    if (delta < 0) {
        if (delta == -1) {
            // left
            return 2;
        } else {
            // up
            return 0;
        }
    } else {
        if (delta == 1) {
            // right
            return 3;
        } else {
            // down
            return 1;
        }
    }
}

struct CandidateManager {
    const int n;
    vector<Candidate> candidates;
    vector<bool> is_valid;

    CandidateManager(const InputINFO& input, const BeamSearchConfig& config) :
        n(input.n)
    {
        candidates = vector<Candidate>(4 * n * n, Candidate(Evaluation(0, 0, 0, 0, 0, config), -1));
        is_valid = vector<bool>(4 * n * n, false);
    }

    void clear() {
        fill(is_valid.begin(), is_valid.end(), false);
    }

    void push(const Candidate& candidate, Action p, Action from1, Action from2) {
        int i = get_index(p, from1, from2);
        if (!is_valid[i]) {
            is_valid[i] = true;
            candidates[i] = candidate;
        } else if (candidate < candidates[i]) {
            candidates[i] = candidate;
        }
    }

    int get_index(Action p, Action from1, Action from2) {
        return 4 * p + get_direction(p - from1);
    }
};

inline ll square_interval(ll l, ll r) {
    assert(l <= r);
    ll interval = r - l;
    return interval * (interval - 1);
}

inline ll insertion_square_interval(ll l, ll m, ll r) {
    assert(l <= m && m <= r);
    return square_interval(l, m) + square_interval(m, r) - square_interval(l, r);
}

struct State {
    ll depth = 0;

    const BeamSearchConfig config;
    const int n;
    const ll max_turn;
    const vector<ll> d;
    vector<ll> fixed_d;
    const vector<vector<int>> edges;
    ll sum_fixed_d;
    ll initial_interval;
    ll next_interval;

    vector<vector<ll>> timestamp;

    State(const InputINFO& input_info, const BeamSearchConfig& config) :
        config(config),
        n(input_info.n),
        max_turn(config.max_turn),
        d(input_info.d),
        edges(input_info.edges)
    {
        fixed_d = vector<ll>(n * n);
        for (int p = 0; p < n * n; ++p) {
            fixed_d[p] = pow(max(config.min_fixed_d, static_cast<double>(d[p])), config.fixed_exp);
        }
        sum_fixed_d = sum(fixed_d);

        initial_interval = square_interval(0, 2 * config.max_turn) / 2;
        next_interval = square_interval(0, max_turn) / 2 - initial_interval;

        timestamp = vector<vector<ll>>(n * n);
        timestamp[config.start_position].push_back(-1);
    }

    Evaluation make_first_evaluation() {
        ll initial_cost = sum(d) * initial_interval;
        initial_cost += d[config.start_position] * next_interval;

        ll initial_dirtiness = (-config.initial_time - 1) * (sum_fixed_d - fixed_d[config.start_position]);

        return Evaluation(0, initial_dirtiness, initial_cost, 0.0, 0.0, config);
    }

    void update_candidates(Action from1, Action from2, const Evaluation& evaluation, int parent, CandidateManager& candidate_manager) const {
        double dx = config.dxy_decay * evaluation.dx_average;
        double dy = config.dxy_decay * evaluation.dy_average;
        for (int p : edges[from1]) {
            ll harvest, delta_cost;
            if (timestamp[p].empty()) {
                harvest = fixed_d[p] * (depth - config.initial_time);
                delta_cost = d[p] * next_interval;
            } else {
                ll last_time = timestamp[p].back();
                harvest = fixed_d[p] * (depth - last_time);
                delta_cost = d[p] * insertion_square_interval(last_time, depth, timestamp[p][0] + max_turn) / 2;
            }
            double dx_average = dx;
            double dy_average = dy;
            if (abs(p - from1) == 1) {
                if (p - from1 == 1) {
                    dy_average += config.dxy_decay_2;
                } else {
                    dy_average -= config.dxy_decay_2;
                }
            } else {
                if (p - from1 == n) {
                    dx_average += config.dxy_decay_2;
                } else {
                    dx_average -= config.dxy_decay_2;
                }
            }
            Candidate candidate(evaluation.get_new_evaluation(sum_fixed_d, harvest, delta_cost, dx_average, dy_average, config), parent);
            candidate_manager.push(candidate, p, from1, from2);
        }
    }

    void move_forward(Action action) {
        timestamp[action].push_back(depth++);
    }

    void move_backward(Action action) {
        --depth;
        timestamp[action].pop_back();
    }
};

// doubly chained tree

struct Node {
    Action action;
    int parent;
    int child = -1;
    int left = -1; // sibling
    int right; // sibling
    Evaluation evaluation;

    Node(Action action, int parent, int right, const Evaluation& evaluation) :
        action(action),
        parent(parent),
        right(right),
        evaluation(evaluation) {}
};

inline int add_leaf(Action action, const Candidate& candidate, vector<Node>& nodes, stack<int>& garbage) {
    int parent = candidate.parent;
    int child = nodes[parent].child;

    int v;
    if (garbage.empty()) {
        v = nodes.size();
        nodes.emplace_back(Node(action, parent, child, candidate.evaluation));
    } else {
        v = garbage.top();
        garbage.pop();
        nodes[v] = Node(action, parent, child, candidate.evaluation);
    }
    nodes[parent].child = v;

    if (child != -1) {
        nodes[child].left = v;
    }
    return v;
}

inline void remove_leaf(int v, vector<Node>& nodes, stack<int>& garbage) {
    while (true) {
        garbage.push(v);
        int left = nodes[v].left;
        int right = nodes[v].right;
        if (left == -1) {
            int parent = nodes[v].parent;
            nodes[parent].child = right;
            if (right != -1) {
                nodes[right].left = -1;
                return;
            }
            v = parent;
        } else {
            nodes[left].right = right;
            if (right != -1) {
                nodes[right].left = left;
            }
            return;
        }
    }
}

inline vector<Action> get_path(int v, vector<Node>& nodes) {
    // get path from the root of the tree
    vector<Action> path;
    while (nodes[v].parent != -1) {
        path.push_back(nodes[v].action);
        v = nodes[v].parent;
    }
    reverse(path.begin(), path.end());
    return path;
}

struct Tree {
    State state;
    int root;

    Tree(const State& state, int root) :
        state(state),
        root(root) {}

    void dfs(int turn, vector<Node>& nodes, CandidateManager& candidates_manager) {
        // go down the direct road
        while (nodes[root].child != -1 && nodes[nodes[root].child].right == -1) {
            root = nodes[root].child;
            state.move_forward(nodes[root].action);
        }
        int v = root;
        while (true) {
            // go to child
            while (nodes[v].child != -1) {
                v = nodes[v].child;
                state.move_forward(nodes[v].action);
            }
            assert(state.depth == turn);

            Action from1 = nodes[v].action;
            Action from2 = (nodes[v].parent == -1) ? state.edges[from1].front() : nodes[nodes[v].parent].action;
            state.update_candidates(from1, from2, nodes[v].evaluation, v, candidates_manager);

            while (v != root && nodes[v].right == -1) {
                // go to parent
                state.move_backward(nodes[v].action);
                v = nodes[v].parent;
            }
            if (v == root) {
                break;
            }
            // go to right sibling
            state.move_backward(nodes[v].action);
            v = nodes[v].right;
            state.move_forward(nodes[v].action);
        }
    }
};

vector<Action> beam_search(const InputINFO& input_info, const BeamSearchConfig& config, Timer timer) {
    int n = input_info.n;
    State state(input_info, config);

    vector<Node> nodes;
    nodes.reserve(config.nodes_capacity);

    stack<int> garbage;

    vector<int> temp_nodes;
    temp_nodes.reserve(4 * (n * n + 1) / 2);

    vector<int> next_nodes;
    next_nodes.reserve(4 * (n * n + 1) / 2);

    CandidateManager candidate_manager(input_info, config);

    Tree tree(state, 0);
    nodes.emplace_back(Node(config.start_position, -1, -1, state.make_first_evaluation()));
    temp_nodes.push_back(0);

    for (int turn = 0; turn < config.max_turn; ++turn) {
        if (turn % 2 == 0 && !timer.yet(config.beam_search_time_limit)) {
            break;
        }
        tree.dfs(turn, nodes, candidate_manager);
        for (size_t i = 0; i < candidate_manager.candidates.size(); ++i) {
            if (candidate_manager.is_valid[i]) {
                int p = i / 4;
                next_nodes.push_back(add_leaf(p, candidate_manager.candidates[i], nodes, garbage));
            }
        }
        candidate_manager.clear();

        for (int v : temp_nodes) {
            if (nodes[v].child == -1) {
                remove_leaf(v, nodes, garbage);
            }
        }
        swap(temp_nodes, next_nodes);
        next_nodes.clear();
    }
    // choose best leaf
    int best_leaf = -1;
    for (int v : temp_nodes) {
        if (nodes[v].action != config.start_position) {
            continue;
        }
        if (best_leaf == -1 || nodes[v].evaluation.cost < nodes[best_leaf].evaluation.cost) {
            best_leaf = v;
        }
    }
    assert(best_leaf != -1);
    // cerr << n << " " << nodes.size() << endl;
    return get_path(best_leaf, nodes);
}

// Simulated Annealing Section

struct SimulatedAnnealingConfig {
    ll non_visit_cost;
    int cross_threshold;
    double first_temperature;
    double last_temperature;
    uint32_t switch_L_percent;
    uint32_t destroy_and_reconstruct_percent;
    int min_destroy_length;
    int max_destroy_length;
    int max_extend_length;
    ll sort_randomness_min;
    ll sort_randomness_max;

    SimulatedAnnealingConfig(const InputINFO& input_info) {
        non_visit_cost = 1e12;
        cross_threshold = 150;
        //first_temperature;
        //last_temperature;
        switch_L_percent = 60;
        destroy_and_reconstruct_percent = switch_L_percent + 10;
        min_destroy_length = 4;
        max_destroy_length = 8;
        max_extend_length = 2;
        sort_randomness_min = 10;
        sort_randomness_max = 50;
    }

    void set_temperature(ll average_cost) {
        first_temperature = 0.3 * average_cost;
        last_temperature = 0.03 * average_cost;
    }
};

struct Simulator {
    Xorshift rng;

    SimulatedAnnealingConfig config;

    const int n;
    const vector<ll> d;
    const vector<vector<int>> edges;
    vector<bool> is_articulation_point;

    vector<int> circuit;
    vector<vector<ll>> timestamp;
    ll cost;

    vector<int> visited;
    vector<tuple<ll,ll,ll>> insertion_info;
    vector<int> active_insertion_info;

    Simulator(const InputINFO& input_info, const vector<int>& circuit) :
        rng(1),
        config(input_info),
        n(input_info.n),
        d(input_info.d),
        edges(input_info.edges),
        circuit(circuit)
    {
        timestamp = vector<vector<ll>>(n * n);
        init_timestamp_and_cost();

        init_is_articulation_point();

        visited = vector<int>(n * n, 0);
        insertion_info = vector<tuple<ll,ll,ll>>(n * n, {-1, -1, -1});

        config.set_temperature(get_average_cost());
    }

    void init_timestamp_and_cost() {
        for (int p = 0; p < n * n; ++p) {
            timestamp[p].clear();
        }
        for (size_t t = 0; t < circuit.size(); ++t) {
            timestamp[circuit[t]].push_back(t);
        }
        cost = 0;
        for (int p = 0; p < n * n; ++p) {
            if (timestamp[p].empty()) {
                cost += config.non_visit_cost;
                continue;
            }
            timestamp[p].push_back(circuit.size() + timestamp[p].front());
            ll s = 0;
            for (size_t i = 1; i < timestamp[p].size(); ++i) {
                s += square_interval(timestamp[p][i - 1], timestamp[p][i]);
            }
            cost += d[p] * s / 2;
            timestamp[p].pop_back();
        }
    }

    void init_is_articulation_point() {
        is_articulation_point = vector<bool>(n * n, false);
        vector<int> ord(n * n, -1);
        vector<int> low(n * n, n * n);
        low_link(0, 0, ord, low);
    }

    int low_link(int u, int i, vector<int>& ord, vector<int>& low) {
        assert(ord[u] == -1);
        ord[u] = i++;
        int num_children = 0;
        for (int v : edges[u]) {
            if (ord[v] == -1) {
                // forward edge
                ++num_children;
                i = low_link(v, i, ord, low);
                low[u] = min(low[u], low[v]);
                if (ord[u] && ord[u] <= low[v]) {
                    is_articulation_point[u] = true;
                }
            } else {
                // backward edge
                if (ord[v] < ord[u] - 1) {
                    low[u] = min(low[u], ord[v]);
                }
            }
        }
        if (ord[u] == 0 && num_children >= 2) {
            // root
            is_articulation_point[u] = true;
        }
        return i;
    }

    double get_temperature(const Timer& timer) const {
        double progress = timer.get_last_progress(time_limit);
        return config.first_temperature * (1.0 - progress) + config.last_temperature * progress;
    }

    bool scheduler(ll delta, const Timer& timer) {
        if (delta <= 0) {
            return true;
        }
        double t = get_temperature(timer);
        return rng.random() < exp2(-delta / t);
    }

    ll erase_delta(int p, int i) const {
        assert(0 <= i && i < static_cast<int>(timestamp[p].size()));
        if (timestamp[p].size() == 1) {
            return config.non_visit_cost - d[p] * square_interval(0, circuit.size()) / 2;
        }
        ll t = timestamp[p][i];
        ll t_prev = i ? timestamp[p][i - 1] : (timestamp[p].back() - circuit.size());
        ll t_next = (i + 1 < static_cast<int>(timestamp[p].size())) ? timestamp[p][i + 1] : (timestamp[p][0] + circuit.size());
        return -d[p] * insertion_square_interval(t_prev, t, t_next) / 2;
    }

    void erase(int p, ll t) {
        int i = bisect_left(timestamp[p], t);
        assert(timestamp[p][i] == t);
        cost += erase_delta(p, i);
        timestamp[p].erase(timestamp[p].begin() + i);
    }

    ll insert_delta(int p, int i, ll t) const {
        assert(0 <= i && i <= static_cast<int>(timestamp[p].size()));
        if (timestamp[p].empty()) {
            return d[p] * square_interval(0, circuit.size()) / 2 - config.non_visit_cost;
        }
        ll t_prev = i ? timestamp[p][i - 1] : (timestamp[p].back() - circuit.size());
        ll t_next = (i < static_cast<int>(timestamp[p].size())) ? timestamp[p][i] : (timestamp[p][0] + circuit.size());
        return d[p] * insertion_square_interval(t_prev, t, t_next) / 2;
    }

    void insert(int p, ll t) {
        int i = bisect_left(timestamp[p], t);
        cost += insert_delta(p, i, t);
        timestamp[p].insert(timestamp[p].begin() + i, t);
    }

    void range_reverse(int l, int r) {
        assert(l < r && r < static_cast<int>(circuit.size()));
        assert(circuit[l] == circuit[r]);
        for (int t = l + 1; t < r; ++t) {
            erase(circuit[t], t);
        }
        reverse(circuit.begin() + (l + 1), circuit.begin() + r);
        for (int t = l + 1; t < r; ++t) {
            insert(circuit[t], t);
        }
    }

    void resolve_cross(const Timer& timer) {
        int p = rng.randrange(n * n);
        if (timestamp[p].size() <= 1 || is_articulation_point[p]) {
            return;
        }
        for (size_t i = 1; i < timestamp[p].size(); ++i) {
            ll t1 = timestamp[p][i - 1];
            ll t2 = timestamp[p][i];
            if (t2 - t1 > config.cross_threshold) {
                continue;
            }
            int d11 = p - (t1 == 0 ? circuit.back() : circuit[t1 - 1]);
            int d12 = circuit[t1 + 1] - p;
            if (d11 != d12) {
                continue;
            }
            int d21 = p - circuit[t2 - 1];
            int d22 = (t2 + 1 == static_cast<ll>(circuit.size()) ? circuit[0] : circuit[t2 + 1]) - p;
            if (d21 != d22) {
                continue;
            }
            if (abs(d11) == abs(d21)) {
                continue;
            }
            ll old_cost = cost;
            range_reverse(t1, t2);
            if (!scheduler(cost - old_cost, timer)) {
                range_reverse(t1, t2);
            }
        }
    }

    void switch_L(const Timer& timer) {
        ll t = rng.randrange(1, circuit.size() - 1);
        int q1 = circuit[t];
        if (timestamp[q1].size() == 1) {
            return;
        }
        int p = circuit[t - 1];
        int r = circuit[t + 1];
        int q2 = p + r - q1;
        if (abs(q1 - p) == abs(r - q1) || !contain(edges[q2], p) || !contain(edges[q2], r)) {
            return;
        }
        int i = bisect_left(timestamp[q1], t);
        assert(timestamp[q1][i] == t);
        ll delta = erase_delta(q1, i);

        int j = bisect_left(timestamp[q2], t);
        delta += insert_delta(q2, j, t);

        if (scheduler(delta, timer)) {
            // accepted
            timestamp[q1].erase(timestamp[q1].begin() + i);
            timestamp[q2].insert(timestamp[q2].begin() + j, t);
            circuit[t] = q2;
            cost += delta;
        }
    }

    int manhattan_distance(int p, int q) const {
        return abs(p / n - q / n) + abs(p % n - q % n);
    }

    ll get_insertion_delta(int p, ll t, ll t_min, ll t_max) {
        auto [t_prev, t_next, delta] = insertion_info[p];
        if (t_next == -1) {
            if (timestamp[p].empty()) {
                t_prev = 0;
                t_next = circuit.size();
                delta = d[p] * square_interval(t_prev, t_next) / 2 - config.non_visit_cost;
            } else {
                int i = bisect_left(timestamp[p], t_min);
                t_prev = i ? timestamp[p][i - 1] : (timestamp[p].back() - circuit.size());
                t_next = (i < static_cast<int>(timestamp[p].size())) ? timestamp[p][i] : (timestamp[p][0] + circuit.size());
                delta = -square_interval(t_prev, t_next);
            }
            insertion_info[p] = {t_prev, t_next, delta};
            active_insertion_info.push_back(p);
        }
        if (timestamp[p].empty()) {
            return delta;
        }
        t = min(t, t_max);
        delta += square_interval(t_prev, t) + square_interval(t, t_next);
        return d[p] * delta / 2;
    }

    void reset_insertion_info() {
        for (int p : active_insertion_info) {
            insertion_info[p] = {-1, -1, -1};
        }
        active_insertion_info.clear();
    }

    void destroy_and_reconstruct(const Timer& timer) {
        int destroy_length = rng.randint(config.min_destroy_length, config.max_destroy_length);
        ll t1 = rng.randrange(circuit.size() - destroy_length - config.max_extend_length);
        ll t2 = t1 + destroy_length;
        int start = circuit[t1];
        int goal = circuit[t2];

        ll old_cost = cost;
        ll average_cost = get_average_cost();
        for (int t = t1 + 1; t < t2; ++t) {
            erase(circuit[t], t);
        }
        int max_new_length = destroy_length + config.max_extend_length;
        visited[start] = 1;
        vector<pair<int,ll>> path = {{start, cost - old_cost}};
        path.reserve(max_new_length);

        vector<vector<tuple<ll,ll,int>>> branches;

        auto push_branches = [&](int p, int length) {
            vector<tuple<ll,ll,int>> branch;
            for (int q : edges[p]) {
                if (q == goal) {
                    branch.push_back({LLONG_MIN, LLONG_MIN, q});
                }
                if (length + manhattan_distance(q, goal) > max_new_length) {
                    continue;
                }
                ll delta;
                if (!visited[q]) {
                    delta = get_insertion_delta(q, t1 + path.size(), t1, t2);
                } else if (visited[q] == 1 && is_articulation_point[q]) {
                    delta = 0; // this is not accurate
                } else {
                    continue;
                }
                branch.push_back({rng.randrange(config.sort_randomness_min, config.sort_randomness_max) * delta, delta, q});
            }
            sort(branch.begin(), branch.end(),
                [](const tuple<ll,ll,int>& a, const tuple<ll,ll,int>& b) {
                    return get<0>(a) > get<0>(b);
                }
            );
            branches.emplace_back(branch);
        };
        
        push_branches(start, 1);
        
        ll update_threshold = -get_temperature(timer) * log2(rng.random());

        // backtracking
        while (!path.empty()) {
            if (branches.back().empty()) {
                // back
                --visited[path.back().first];
                path.pop_back();
                branches.pop_back();
                continue;
            }
            auto [_, d_delta, p] = branches.back().back();
            branches.back().pop_back();
            if (p == goal) {
                // find new path
                ll delta = path.back().second + average_cost * (static_cast<ll>(path.size()) - destroy_length);
                if (delta <= update_threshold) {
                    // accepeted
                    break;
                } else {
                    // rejected
                    continue;
                }
            }
            ++visited[p];
            path.push_back({p, path.back().second + d_delta});
            push_branches(p, path.size());
        }
        reset_insertion_info();
        for (auto [p, _] : path) {
            --visited[p];
        }
        if (path.empty()) {
            // restore
            for (ll t = t1 + 1; t < t2; ++t) {
                insert(circuit[t], t);
            }
            assert(cost == old_cost);
        } else if (static_cast<int>(path.size()) == destroy_length) {
            // update
            for (ll t = t1 + 1; t < t2; ++t) {
                int p = path[t - t1].first;
                circuit[t] = p;
                insert(p, t);
            }
        } else {
            // update
            vector<int> new_circuit;
            new_circuit.reserve(circuit.size() - destroy_length + path.size());
            new_circuit.insert(new_circuit.end(), circuit.begin(), circuit.begin() + t1);
            for (auto [p, _] : path) {
                new_circuit.push_back(p);
            }
            new_circuit.insert(new_circuit.end(), circuit.begin() + t2, circuit.end());

            int start_index = rng.randrange(new_circuit.size() / 4, 3 * new_circuit.size() / 4);
            circuit = vector<int>(new_circuit.begin() + start_index, new_circuit.end());
            circuit.insert(circuit.end(), new_circuit.begin(), new_circuit.begin() + start_index);

            if (rng.randrange(2)) {
                reverse(circuit.begin(), circuit.end());
            }
            init_timestamp_and_cost();
        }
    }

    void simulated_annealing(const Timer& timer) {
        uint32_t percent = rng.randrange(100);
        if (percent < config.switch_L_percent) {
            switch_L(timer);
        } else if (percent < config.destroy_and_reconstruct_percent) {
            destroy_and_reconstruct(timer);
        } else {
            resolve_cross(timer);
        }
    }

    ll get_average_cost() const {
        return cost / circuit.size();
    }
};

// Control Section

constexpr double non_double_time_ratio = 0.8;

struct Solver {
    Timer timer;

    const InputINFO input_info;
    const int n;
    vector<vector<int>> edges;
    string output;
    ll cost;

    Solver(const Input& input) :
        input_info(input),
        n(input.n)
    {
        edges = input_info.edges;
    }
    
    void solve() {
        const BeamSearchConfig beam_search_config(input_info);
        vector<int> circuit = beam_search(input_info, beam_search_config, timer);
        assert(circuit.back() == beam_search_config.start_position);

        circuit = make_valid_solution(circuit);

        Simulator simulator(input_info, circuit);
        cost = simulator.get_average_cost();
        double double_circuit_time = timer.get_time() + non_double_time_ratio * time_limit - timer.get_time();
        for (int i = 0; i < 2; ++i) {
            while (timer.yet(i ? time_limit : double_circuit_time)) {
                simulator.simulated_annealing(timer);
                if (simulator.get_average_cost() < cost) {
                    cost = simulator.get_average_cost();
                    circuit = simulator.circuit;
                }
            }
            if (i == 0) {
                circuit.insert(circuit.end(), circuit.begin(), circuit.end());
                simulator.circuit = circuit;
                simulator.init_timestamp_and_cost();
            }
        }
        size_t offset = index(circuit, 0);
        for (size_t i = offset; i < offset + circuit.size(); ++i) {
            output.push_back(get_char(circuit[i % circuit.size()], circuit[(i + 1) % circuit.size()]));
        }
    }

    vector<int> make_valid_solution(vector<int> path) {
        vector<bool> visited(n * n, false);
        for (int p : path) {
            visited[p] = true;
        }

        for (int permit_back = 0; permit_back <= 1; ++permit_back) {
            vector<int> circuit;
            circuit.reserve(path.size() + n * n);

            for (size_t i = 0; i < path.size(); ++i) {
                vector<int> extension = {path[i]};
                size_t j = 0;
                while (j < extension.size()) {
                    int p = extension[j];
                    int q = (j + 1 == extension.size()) ? path[(i + 1) % path.size()] : extension[j + 1];
                    ++j;
                    for (int r : edges[p]) {
                        if (visited[r]) {
                            continue;
                        }
                        if (!visited[r + q - p] && contain(edges[r], r + q - p) && contain(edges[r + q - p], q)) {
                            extension.insert(extension.begin() + j, r + q - p);
                            extension.insert(extension.begin() + j, r);
                            visited[r] = true;
                            visited[r + q - p] = true;
                        } else if (permit_back) {
                            extension.insert(extension.begin() + j, p);
                            extension.insert(extension.begin() + j, r);
                            visited[r] = true;
                        } else {
                            continue;
                        }
                        --j;
                        break;
                    }
                }
                circuit.insert(circuit.end(), extension.begin(), extension.end());
            }
            path = circuit;
        }
        return path;
    }

    char get_char(int fr, int to) {
        return "UDLR"[get_direction(to - fr)];
    }

    void print() const {
        cout << output << "\n";
    }

    void print(const string& filename) const {
        ofstream out(filename);
        out << output << "\n";
    }

    ll score() const {
        return cost;
    }
};

void multi_test(int cases, int num_threads) {
    if (cases <= 0) {
        return;
    }
    cerr << "cases: " << cases << endl;

    vector<double> log_costs(cases);
    vector<double> times(cases);

#ifndef ONLINE_JUDGE
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
#endif
    for (int seed = 0; seed < cases; ++seed) {
        string filename = to_string(seed);
        filename = "in/" + string(4 - filename.size(), '0') + filename + ".txt";
 
        Input input;
        input.input(filename);
 
        Solver solver(input);
        solver.solve();

        times[seed] = solver.timer.get_time();
        ll cost = solver.score();
        log_costs[seed] = log2(cost);

        cerr << filename << " " << cost << " " << times[seed] << " sec" << endl;

        solver.print("out" + filename.substr(2));
    }
    cerr << "Average Log2 Cost: " << accumulate(log_costs.begin(), log_costs.end(), 0.0) / cases << endl;
    cerr << "Max Time: " << *max_element(times.begin(), times.end()) << " sec" << endl;
    cerr << "Average Time: " << accumulate(times.begin(), times.end(), 0.0) / cases << " sec" << endl;
}

int main() {
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Input input;
    input.input();

    Solver solver(input);
    solver.solve();
    solver.print();

#ifndef ONLINE_JUDGE
    cerr << "Score = " << solver.score() << ", " << solver.timer.get_time() << " sec" << endl;

    int cases = 100;
    int num_threads = 20;
    multi_test(cases, num_threads);
#endif // ONLINE_JUDGE

    return 0;
}
