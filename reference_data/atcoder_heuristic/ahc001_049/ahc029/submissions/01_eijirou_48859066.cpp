#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#ifdef ONLINE_JUDGE
#define NDEBUG
#endif // ONLINE_JUDGE

#include <bits/stdc++.h>
// #include <atcoder/all>

#ifndef ONLINE_JUDGE
#include <omp.h>
#endif // ONLINE_JUDGE

using namespace std;
// using namespace atcoder;

#define ll long long int
#define ull unsigned long long int

template <class T>
inline int argmin(const vector<T>& data) {
    assert(!data.empty());
    return distance(data.begin(), min_element(data.begin(), data.end()));
}

template <class T>
inline int argmax(const vector<T>& data) {
    assert(!data.empty());
    return distance(data.begin(), max_element(data.begin(), data.end()));
}

class Xorshift {
    public:
        Xorshift(uint32_t seed): x_(seed) {
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

    private:
        chrono::system_clock::time_point start_time_;
        double elapsed_time_;
};

#ifdef ONLINE_JUDGE
constexpr double time_limit = 1.95;
#else
constexpr double time_limit = 2.95;
#endif // ONLINE_JUDGE

constexpr int max_turn = 1000;
constexpr int max_investments = 20;

enum struct CardType {
    RegularWork,
    HardWork,
    Cancel,
    Restructuring,
    Investment
};

struct Card {
    CardType t;
    ll w;

    Card() {}

    Card(CardType t, ll w) :
        t(t),
        w(w) {}
};

struct Project {
    ll h, v;

    Project() {}

    Project(ll h, ll v) :
        h(h),
        v(v) {}

    bool work(ll w) {
        h -= w;
        return h <= 0;
    }
};

struct State {
    int turn;
    vector<Card> cards;
    vector<Project> projects;
    ll money;
    int l;
    int empty_card_position;
    vector<int> empty_project_positions;
    int investments;

    State() {}

    State(const vector<Card>& cards, const vector<Project>& projects) :
        turn(0),
        cards(cards),
        projects(projects),
        money(0),
        l(0),
        empty_card_position(-1)
    {
        investments = 0;
        for (const Card& card : cards) {
            if (card.t == CardType::Investment) {
                ++investments;
            }
        }
    }

    void use_card(int c, int m) {
        empty_project_positions.clear();
        empty_card_position = c;

        const Card& card = cards[c];
        switch (card.t) {
            case CardType::RegularWork:
                work(m, card.w);
                break;
            case CardType::HardWork:
                for (size_t i = 0; i < projects.size(); ++i) {
                    work(i, card.w);
                }
                break;
            case CardType::Cancel:
                empty_project_positions.push_back(m);
                break;
            case CardType::Restructuring:
                empty_project_positions.resize(projects.size());
                iota(empty_project_positions.begin(), empty_project_positions.end(), 0);
                break;
            case CardType::Investment:
                ++l;
                break;
        }
    }

    void work(int m, int w) {
        if (projects[m].work(w)) {
            money += projects[m].v;
            empty_project_positions.push_back(m);
        }
    }

    void refill_card(const Card& card, ll p) {
        money -= p;
        cards[empty_card_position] = card;
        if (card.t == CardType::Investment) {
            ++investments;
        }
        ++turn;
    }
};

struct IO {
    bool online_judge;

    int n, m, k;
    vector<Card> initial_cards;
    vector<Project> initial_projects;

    ofstream out;
    vector<Project> future_projects;
    vector<vector<pair<Card,ll>>> future_cards;
    int completed_projects;

    void input() {
        online_judge = true;

        int t;
        cin >> n >> m >> k >> t;
        assert(t == max_turn);

        initial_cards.resize(n);
        for (int i = 0; i < n; ++i) {
            int t;
            ll w;
            cin >> t >> w;
            initial_cards[i] = Card(static_cast<CardType>(t), w);
        }
        initial_projects.resize(m);
        for (int i = 0; i < m; ++i) {
            ll h, v;
            cin >> h >> v;
            initial_projects[i] = Project(h, v);
        }
    }

    void input(const string& input_filename, const string& output_filename) {
        online_judge = false;

        ifstream in(input_filename);
        assert(in);

        int t;
        in >> n >> m >> k >> t;
        assert(t == max_turn);

        initial_projects.resize(m);
        for (int i = 0; i < m; ++i) {
            ll h, v;
            in >> h >> v;
            initial_projects[i] = Project(h, v);
        }
        future_projects.resize(max_turn * m);
        for (int i = 0; i < max_turn * m; ++i) {
            ll h, v;
            in >> h >> v;
            future_projects[i] = Project(h, v);
        }
        initial_cards.resize(n);
        for (int i = 0; i < n; ++i) {
            int t;
            ll w;
            in >> t >> w;
            initial_cards[i] = Card(static_cast<CardType>(t), w);
        }
        future_cards.resize(max_turn, vector<pair<Card,ll>>(k));
        for (int i = 0; i < max_turn; ++i) {
            for (int j = 0; j < k; ++j) {
                int t;
                ll w, p;
                in >> t >> w >> p;
                future_cards[i][j] = {Card(static_cast<CardType>(t), w), p};
            }
        }
        completed_projects = 0;

        out = ofstream(output_filename);
    }

    void use_card(State& state, int c, int i) {
        assert(0 <= c && c < n);
        assert(0 <= i && i < m);
        if (online_judge) {
            cout << c << " " << i << endl;
        } else {
            out << c << " " << i << endl;
        }
        state.use_card(c, i);
    }

    void update_projects(State& state) {
        if (online_judge) {
            for (int i = 0; i < m; ++i) {
                ll h, v;
                cin >> h >> v;
                state.projects[i] = Project(h, v);
            }
        } else {
            for (int i : state.empty_project_positions) {
                Project project = future_projects[completed_projects++];
                project.h <<= state.l;
                project.v <<= state.l;
                state.projects[i] = project;
            }
        }
    }

    void input_money(const State& state) {
        if (online_judge) {
            ll money;
            cin >> money;
            assert(money == state.money);
        }
    }

    vector<pair<Card,ll>> get_candidate_cards(const State& state) {
        if (online_judge) {
            vector<pair<Card,ll>> ret(k);
            for (int i = 0; i < k; ++i) {
                int t;
                ll w, p;
                cin >> t >> w >> p;
                ret[i] = {Card(static_cast<CardType>(t), w), p};
            }
            return ret;
        } else {
            vector<pair<Card,ll>> ret = future_cards[state.turn];
            for (int i = 0; i < k; ++i) {
                ret[i].first.w <<= state.l;
                ret[i].second <<= state.l;
            }
            return ret;
        }
    }

    void refill_card(State& state, const vector<pair<Card,ll>>& candidate_cards, int r) {
        assert(0 <= r && r < k);
        auto [card, p] = candidate_cards[r];
        assert(state.money >= p);

        if (online_judge) {
            cout << r << endl;
        } else {
            out << r << endl;
        }
        state.refill_card(card, p);
    }
};

constexpr int nd_size = 1024;
constexpr double initial_frequencies = 1.0;

struct InputGenerator {
    Xorshift rng;
    array<double,nd_size> nd;
    int sum_frequencies;
    array<int,5> frequencies;

    InputGenerator() :
        rng(1)
    {
        frequencies[0] = initial_frequencies * 21;
        frequencies[1] = initial_frequencies * 11;
        frequencies[2] = initial_frequencies * 11;
        frequencies[3] = initial_frequencies * 6;
        frequencies[4] = initial_frequencies * 4;
        sum_frequencies = accumulate(frequencies.begin(), frequencies.end(), 0);
        init_nd();
    }

    void init_nd() {
        mt19937 engine(0);
        normal_distribution<> dist(0.0, 1.0);
        for (int i = 0; i < nd_size; ++i) {
            nd[i] = dist(engine);
        }
    }

    double gauss(double mu, double sigma) {
        return mu + sigma * nd[rng.randrange(nd_size)];
    }

    void update(const vector<pair<Card,ll>>& candidate_cards) {
        sum_frequencies += candidate_cards.size();
        for (const auto& [card, _] : candidate_cards) { // This is a bug !
            ++frequencies[static_cast<int>(card.t)];
        }
    }

    CardType generate_card_type() {
        int x = rng.randrange(sum_frequencies);
        for (int i = 0; i < 4; ++i) {
            if (x < frequencies[i]) {
                return static_cast<CardType>(i);
            }
            x -= frequencies[i];
        }
        return CardType::Investment;
    }

    vector<pair<Card,ll>> generate_cards(int k, ll m) {
        vector<pair<Card,ll>> ret(k);
        ret[0] = {Card(CardType::RegularWork, 1), 0};
        for (int i = 1; i < k; ++i) {
            CardType t = generate_card_type();
            switch (t) {
                case CardType::RegularWork: {
                    ll w_dash = rng.randint(1, 50);
                    ll w = w_dash;
                    ll p = clamp(lround(gauss(w_dash, w_dash / 3.0)), 1l, 10000l);
                    ret[i] = {Card(t, w), p};
                    break;
                } case CardType::HardWork: {
                    ll w_dash = rng.randint(1, 50);
                    ll w = w_dash;
                    ll p = clamp(lround(gauss(w_dash * m, w_dash * m / 3.0)), 1l, 10000l);
                    ret[i] = {Card(t, w), p};
                    break;
                } case CardType::Cancel:
                    ret[i] = {Card(t, 0), rng.randint(0, 10)};
                    break;
                case CardType::Restructuring:
                    ret[i] = {Card(t, 0), rng.randint(0, 10)};
                    break;
                case CardType::Investment:
                    ret[i] = {Card(t, 0), rng.randint(200, 1000)};
                    break;
            }
        }
        return ret;
    }

    Project generate_project() {
        double b = rng.uniform(2.0, 8.0);
        ll h = lround(pow(2, b));
        ll v = lround(pow(2, clamp(gauss(b, 0.5), 0.0, 10.0)));
        return Project(h, v);
    }
};

constexpr int investment_turn_threshold_0 = 0.88 * max_turn;
constexpr int investment_turn_threshold_1 = 0.96 * max_turn;
constexpr int last_phase_threshold = 0.98 * max_turn;
constexpr double remaining_penalty = 0.6;
constexpr double work_coefficient = 1.1;
constexpr double v_rate = 0.98;
constexpr double regular_work_card_profit = 0.9;
constexpr double hard_work_card_profit = 0.6;

const array<double,257> make_pow_array(double c1, double c2, double c3) {
    array<double,257> ret;
    for (int i = 0; i < 257; ++i) {
        ret[i] = c3 / (1.0 + c1 * pow(1 + i, c2));
    }
    return ret;
}

static const array<double,257> pow_array_5_4 = make_pow_array(0.5, 0.4, work_coefficient);

inline double calculate_profit(ll w, const Project& project, int l, ll money) {
    if (project.h <= w) {
        return work_coefficient * project.v;
    }
    double profit = static_cast<double>(project.v * w) / project.h;
    ll i = max(0ll, project.h - w - money) >> l;
    assert(i < 257);
    return profit * pow_array_5_4[i];
}

inline bool is_superior_card(const Card& card, ll p, const vector<pair<Card,ll>>& candidate_cards) {
    for (auto [card2, p2] : candidate_cards) {
        if (card2.t == card.t && p2 < p && card2.w >= card.w) {
            return false;
        }
    }
    return true;
}

inline bool can_buy_card(const State& state, const Card& card, ll p, const vector<pair<Card,ll>>& candidate_cards) {
    if (p > state.money) {
        return false;
    } else if (card.t != CardType::Investment) {
        return is_superior_card(card, p, candidate_cards);
    } else if (state.investments >= max_investments) {
        return false;
    } else if (state.turn <= investment_turn_threshold_0) {
        return is_superior_card(card, p, candidate_cards);
    } else if (state.turn >= investment_turn_threshold_1) {
        return false;
    } else {
        double max_investment_rate = static_cast<double>(investment_turn_threshold_1 - state.turn) / (investment_turn_threshold_1 - investment_turn_threshold_0);
        if (p > state.money * max_investment_rate) {
            return false;
        }
        return is_superior_card(card, p, candidate_cards);
    }
}

inline double get_last_penalty(int turn) {
    if (turn <= last_phase_threshold) {
        return 1.0;
    } else {
        return static_cast<double>(max_turn - turn) / (max_turn - last_phase_threshold);
    }
}

inline double get_regular_work_card_profit(double last_penalty, ll w) {
    return last_penalty * regular_work_card_profit * w;
}

inline double get_hard_work_card_profit(double last_penalty, ll w, size_t m) {
    return last_penalty * hard_work_card_profit * m * w;
}

inline double get_cancel_card_profit(double last_penalty, int l) {
    return last_penalty * (1 << l);
}

inline double get_restructuring_card_profit(double last_penalty, int l) {
    return last_penalty * (1 << l);
}

inline double get_card_profit(const Card& card, double last_penalty, const State& state) {
    switch (card.t) {
        case CardType::RegularWork:
            return get_regular_work_card_profit(last_penalty, card.w);
        case CardType::HardWork:
            return get_hard_work_card_profit(last_penalty, card.w, state.projects.size());
        case CardType::Cancel:
            return get_cancel_card_profit(last_penalty, state.l);
        case CardType::Restructuring:
            return get_restructuring_card_profit(last_penalty, state.l);
        case CardType::Investment:
            return 0.0;
    }
    assert(false);
}

inline tuple<double,int,int> evaluate_card(State& state, const Card& card, ll p) {
    state.cards[state.empty_card_position] = card;

    double last_penalty = get_last_penalty(state.turn);
    ll money = state.money - p;
    double max_profit = -numeric_limits<double>::infinity();
    int best_c = -1;
    int best_m = -1;

    bool regular_done = false;
    bool cancel_done = false;
    bool restructuring_done = false;
    bool investment_done = false;

    for (size_t c = 0; c < state.cards.size(); ++c) {
        const Card& card = state.cards[c];
        switch (card.t) {
            case CardType::RegularWork: {
                if (card.w == (1 << state.l)) {
                    if (regular_done) {
                        break;
                    }
                    regular_done = true;
                }
                double regular_work_card_profit = get_regular_work_card_profit(last_penalty, card.w);
                for (size_t m = 0; m < state.projects.size(); ++m) {
                    double profit = calculate_profit(card.w, state.projects[m], state.l, money) - regular_work_card_profit;
                    if (profit > max_profit) {
                        max_profit = profit;
                        best_c = c;
                        best_m = m;
                    }
                }
                break;
            } case CardType::HardWork: {
                double profit = 0.0;
                for (const Project& project : state.projects) {
                    profit += calculate_profit(card.w, project, state.l, money);
                }
                profit -= get_hard_work_card_profit(last_penalty, card.w, state.projects.size());
                if (profit > max_profit) {
                    max_profit = profit;
                    best_c = c;
                    best_m = 0;
                }
                break;
            } case CardType::Cancel: {
                if (cancel_done) {
                    break;
                }
                cancel_done = true;
                double cancel_card_profit = get_cancel_card_profit(last_penalty, state.l);
                for (size_t m = 0; m < state.projects.size(); ++m) {
                    const Project& project = state.projects[m];
                    double profit = project.h - (v_rate * project.v + (1.0 - v_rate) * (32 << state.l)) - cancel_card_profit;
                    if (profit > max_profit) {
                        max_profit = profit;
                        best_c = c;
                        best_m = m;
                    }
                }
                break;
            } case CardType::Restructuring: {
                if (restructuring_done) {
                    break;
                }
                restructuring_done = true;
                double profit = 0.0;
                for (const Project& project : state.projects) {
                    profit += project.h - (v_rate * project.v + (1.0 - v_rate) * (32 << state.l));
                }
                profit -= get_cancel_card_profit(last_penalty, state.l);
                if (profit > max_profit) {
                    max_profit = profit;
                    best_c = c;
                    best_m = 0;
                }
                break;
            } case CardType::Investment: {
                if (investment_done) {
                    break;
                }
                investment_done = true;
                assert(state.l < max_investments);
                max_profit = 1e18;
                best_c = c;
                best_m = 0;
                break;
            }
        }
    }
    max_profit += get_card_profit(card, last_penalty, state) - p;
    return {max_profit, best_c, best_m};
}

inline vector<tuple<double,int,int>> evaluate_card_with_candidates(State& state, const Card& card, ll p) {
    state.cards[state.empty_card_position] = card;

    double last_penalty = get_last_penalty(state.turn);
    ll money = state.money - p;

    vector<tuple<double,int,int>> ret(state.cards.size());
    double fixed_profit = get_card_profit(card, last_penalty, state) - p;

    bool regular_done = false;
    bool cancel_done = false;
    bool restructuring_done = false;
    bool investment_done = false;

    for (size_t c = 0; c < state.cards.size(); ++c) {
        const Card& card = state.cards[c];
        double max_profit = -numeric_limits<double>::infinity();
        int best_m = 0;
        switch (card.t) {
            case CardType::RegularWork: {
                if (card.w == (1 << state.l)) {
                    if (regular_done) {
                        break;
                    }
                    regular_done = true;
                }
                for (size_t m = 0; m < state.projects.size(); ++m) {
                    double profit = calculate_profit(card.w, state.projects[m], state.l, money);
                    profit -= get_regular_work_card_profit(last_penalty, card.w);
                    if (profit > max_profit) {
                        max_profit = profit;
                        best_m = m;
                    }
                }
                break;
            } case CardType::HardWork: {
                double profit = 0.0;
                for (size_t m = 0; m < state.projects.size(); ++m) {
                    profit += calculate_profit(card.w, state.projects[m], state.l, money);
                }
                profit -= get_hard_work_card_profit(last_penalty, card.w, state.projects.size());
                if (profit > max_profit) {
                    max_profit = profit;
                    best_m = 0;
                }
                break;
            } case CardType::Cancel: {
                if (cancel_done) {
                    break;
                }
                cancel_done = true;
                for (size_t m = 0; m < state.projects.size(); ++m) {
                    const Project& project = state.projects[m];
                    double profit = project.h - (v_rate * project.v + (1.0 - v_rate) * (32 << state.l));
                    profit -= get_cancel_card_profit(last_penalty, state.l);
                    if (profit > max_profit) {
                        max_profit = profit;
                        best_m = m;
                    }
                }
                break;
            } case CardType::Restructuring: {
                if (restructuring_done) {
                    break;
                }
                restructuring_done = true;
                double profit = 0.0;
                for (size_t m = 0; m < state.projects.size(); ++m) {
                    const Project& project = state.projects[m];
                    profit += project.h - (v_rate * project.v + (1.0 - v_rate) * (32 << state.l));
                }
                profit -= get_cancel_card_profit(last_penalty, state.l);
                if (profit > max_profit) {
                    max_profit = profit;
                    best_m = 0;
                }
                break;
            } case CardType::Investment: {
                if (investment_done) {
                    break;
                }
                investment_done = true;
                assert(state.l < max_investments);
                max_profit = 1e18;
                best_m = 0;
                break;
            }
        }
        ret[c] = {max_profit + fixed_profit, c, best_m};
    }
    sort(ret.begin(), ret.end(),
        [](const tuple<double,int,int>& a, const tuple<double,int,int>& b) {
            return get<0>(a) > get<0>(b);
        }
    );
    return ret;
}

inline tuple<int,int,int> greedy(State& state, const vector<pair<Card,ll>>& candidate_cards) {
    int r = 0;
    int c = 0;
    int m = 0;
    double max_profit = -numeric_limits<double>::infinity();
    for (size_t i = 0; i < candidate_cards.size(); ++i) {
        const auto& [card, p] = candidate_cards[i];
        if (!can_buy_card(state, card, p, candidate_cards)) {
            continue;
        }
        auto [profit, ci, mi] = evaluate_card(state, card, p);
        if (profit > max_profit) {
            r = i;
            c = ci;
            m = mi;
            max_profit = profit;
        }
    }
    return {r, c, m};
}

constexpr double card_evaluation_coefficient = 0.8;
constexpr double project_evaluation_coefficient = 0.7;
constexpr double project_remaining_evaluation = 0.3;
constexpr double investments_coefficient = 425.0;
constexpr double saving_evaluation_coefficient = 20.0;
constexpr double cancel_card_correction = 0.4;
constexpr double state_evaluation_exp = 0.95;

const array<double,257> pow_array_3_4 = make_pow_array(0.3, 0.4, 1.0);

inline double get_money_profit(ll money, int l) {
    ll fixed_money = money >> l;
    if (fixed_money <= 200) {
        return 0.0;
    }
    double rate = fixed_money < 1000 ? (fixed_money - 200.0) / 800.0 : 1.0;
    return saving_evaluation_coefficient * rate * (1 << l);
}

double evaluate_state(const State& state) {
    if (state.turn >= max_turn - 1) {
        return state.money;
    }
    double last_penalty = get_last_penalty(state.turn);

    double card_evaluation = 0.0;
    bool have_cancel_card = false;
    for (const Card& card : state.cards) { // This is a bug !
        card_evaluation += get_card_profit(card, last_penalty, state);
        if (card.t == CardType::Cancel || card.t == CardType::Restructuring) {
            have_cancel_card = true;
        }
    }
    card_evaluation *= card_evaluation_coefficient;

    double project_evaluation = 0.0;
    for (const Project& project : state.projects) {
        ll i = max(0ll, project.h - state.money) >> state.l;
        assert(i < 257);
        if (project.h > project.v && have_cancel_card) {
            project_evaluation += cancel_card_correction * (project.v - project.h) * pow_array_3_4[i];
        } else {
            project_evaluation += (project.v - project.h) * pow_array_3_4[i];
        }
    }
    project_evaluation *= project_evaluation_coefficient;

    double investments_evaluation = investments_coefficient * (1 << state.l);

    double evaluation = state.money + card_evaluation + project_evaluation + investments_evaluation + get_money_profit(state.money, state.l);

    return pow(max(0.0, evaluation), state_evaluation_exp);
}

double play_out(State state, const Card& card0, ll p0, int c0, int m0, int simulation_turns, const vector<Project>& future_projects, const vector<vector<pair<Card,ll>>>& future_cards) {
    assert(static_cast<int>(future_cards.size()) == simulation_turns);

    int future_projects_index = 0;
    for (int t = 0; t < simulation_turns; ++t) {
        if (t == 0) {
            state.refill_card(card0, p0);
            state.use_card(c0, m0);
        } else {
            vector<pair<Card,ll>> candidate_cards = future_cards[t];
            for (size_t i = 0; i < candidate_cards.size(); ++i) {
                candidate_cards[i].first.w <<= state.l;
                candidate_cards[i].second <<= state.l;
            }
            auto [r, c, m] = greedy(state, candidate_cards);
            state.refill_card(candidate_cards[r].first, candidate_cards[r].second);
            state.use_card(c, m);
        }
        for (int i : state.empty_project_positions) {
            Project project = future_projects[future_projects_index++];
            project.h <<= state.l;
            project.v <<= state.l;
            state.projects[i] = project;
        }
    }
    return evaluate_state(state);
}

constexpr int max_simulation_turns = 10;
constexpr int max_play_out_candidates = 7;

struct Solver {
    Timer timer;
    IO io;
    State state;
    InputGenerator input_generator;
    vector<vector<Project>> future_projects;

    Solver() {
        io.input();
        initialize_state();
    }

    Solver(const string& input_filename, const string& output_filename) {
        io.input(input_filename, output_filename);
        initialize_state();
    }

    void initialize_state() {
        state = State(io.initial_cards, io.initial_projects);
    }

    void solve() {
        // first turn
        auto [c0, m0] = choose_card_in_first_turn();
        io.use_card(state, c0, m0);
        io.update_projects(state);
        io.input_money(state);

        // main section
        while (state.turn < max_turn - 1) {
            vector<pair<Card,ll>> candidate_cards = io.get_candidate_cards(state);
            input_generator.update(candidate_cards);
            auto [r, c, m] = decide_action(candidate_cards);
            io.refill_card(state, candidate_cards, r);
            io.use_card(state, c, m);
            io.update_projects(state);
            io.input_money(state);
        }
        // last turn
        vector<pair<Card,ll>> candidate_cards = io.get_candidate_cards(state);
        io.refill_card(state, candidate_cards, 0);
    }

    pair<int,int> choose_card_in_first_turn() {
        state.empty_card_position = 0;
        vector<pair<Card,ll>> candidate_cards = {{state.cards[0], 0}};
        auto [_, c, m] = decide_action(candidate_cards);
        return {c, m};
    }

    tuple<int,int,int> decide_action(const vector<pair<Card,ll>>& candidate_cards) {
        //return greedy(state, candidate_cards);

        vector<tuple<double,int,int,int>> candidates;

        if (state.turn == 0) {
            for (auto [profit, c, m] : evaluate_card_with_candidates(state, state.cards[0], 0)) {
                candidates.push_back({profit, 0, c, m});
            }
        } else {
            for (size_t i = 0; i < candidate_cards.size(); ++i) {
                const auto& [card, p] = candidate_cards[i];
                if (!can_buy_card(state, card, p, candidate_cards)) {
                    continue;
                }
                for (auto [profit, c, m] : evaluate_card_with_candidates(state, card, p)) {
                    candidates.push_back({profit, i, c, m});
                }
            }
        }
        assert(!candidates.empty());
        sort(candidates.begin(), candidates.end());
        reverse(candidates.begin(), candidates.end());

        if (candidates.size() > max_play_out_candidates) {
            candidates.resize(max_play_out_candidates);
        }

        if (candidates.size() == 1) {
            auto [_, r, c, m] = candidates[0];
            return {r, c, m};
        }
        // Monte Carlo Method
        double time_now = timer.get_time();
        double remaining_time = time_limit - time_now;
        double usable_time = remaining_time / (max_turn - state.turn);
        double local_time_limit = time_now + usable_time;
        int original_candidates_size = candidates.size();

        vector<double> sum_evaluation(candidates.size(), 0.0);
        int seed = 0;
        while (timer.yet(local_time_limit) && candidates.size() >= 2) {
            if (static_cast<int>(future_projects.size()) == seed) {
                vector<Project> projects(max_simulation_turns * state.projects.size());
                for (size_t i = 0; i < projects.size(); ++i) {
                    projects[i] = input_generator.generate_project();
                }
                future_projects.emplace_back(projects);
            }
            int simulation_turn = min(max_simulation_turns, max_turn - state.turn - 1);
            vector<vector<pair<Card,ll>>> future_cards(simulation_turn);
            for (int t = 0; t < simulation_turn; ++t) {
                future_cards[t] = input_generator.generate_cards(candidate_cards.size(), state.projects.size());
            }
            for (size_t i = 0; i < sum_evaluation.size(); ++i) {
                auto [_, r, c, m] = candidates[i];
                auto [card0, p0] = candidate_cards[r];
                sum_evaluation[i] += play_out(state, card0, p0, c, m, simulation_turn, future_projects[seed], future_cards);
            }
            if (timer.get_last_time() > time_now + usable_time * (original_candidates_size - candidates.size() + 2) / original_candidates_size) {
                int worst = argmin(sum_evaluation);
                candidates.erase(candidates.begin() + worst);
                sum_evaluation.erase(sum_evaluation.begin() + worst);
            }
            ++seed;
        }
        // cerr << state.turn << " " << seed << endl;
        int best = argmax(sum_evaluation);
        auto [_, r, c, m] = candidates[best];
        return {r, c, m};
    }

    ll score() const {
        return state.money;
    }
};

void multi_test(int cases, int num_threads) {
    if (cases <= 0) {
        return;
    }
    cerr << "cases: " << cases << endl;

    vector<double> scores(cases);
    vector<double> times(cases);

#ifndef ONLINE_JUDGE
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
#endif // ONLINE_JUDGE
    for (int seed = 0; seed < cases; ++seed) {
        string filename = to_string(seed);
        filename = string(4 - filename.size(), '0') + filename + ".txt";
 
        Solver solver("in/" + filename, "out/" + filename);
        solver.solve();

        times[seed] = solver.timer.get_time();
        ll score = solver.score();
        scores[seed] = log2(score + 1);

        cerr << seed << " " << score << " " << times[seed] << " sec" << endl;
    }
    cerr << "Average Score: " << accumulate(scores.begin(), scores.end(), 0.0) / cases << endl;
    cerr << "Max Time: " << *max_element(times.begin(), times.end()) << " sec" << endl;
    cerr << "Average Time: " << accumulate(times.begin(), times.end(), 0.0) / cases << " sec" << endl;
}

int main() {
#ifdef ONLINE_JUDGE
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    Solver solver;
    solver.solve();
#else
    int cases = 1000;
    int num_threads = 20;
    multi_test(cases, num_threads);
#endif // ONLINE_JUDGE

    return 0;
}
