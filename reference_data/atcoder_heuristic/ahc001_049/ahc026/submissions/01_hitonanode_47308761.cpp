#ifndef HITONANODE_LOCAL
#pragma GCC optimize("O3,unroll-loops")
#endif

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <complex>
#include <deque>
#include <forward_list>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>


#include <iostream>
#include <string>
#include <utility>
#include <vector>

class JsonDumper {
    struct KeyValue {
        std::string key;
        std::string value;
    };

    std::vector<KeyValue> _items;

    bool dump_at_end = false;

public:
    JsonDumper(bool dump_at_end_ = false) : dump_at_end(dump_at_end_) {}

    ~JsonDumper() {
        if (dump_at_end) std::cout << dump() << std::endl;
    }

    void set_dump_at_end() { dump_at_end = true; }

    void operator()(const std::string &key, const std::string &value) {
        _items.push_back(KeyValue{key, "\"" + value + "\""});
    }

    template <class T> void operator()(const std::string &key, T value) {
        _items.push_back(KeyValue{key, std::to_string(value)});
    }

    std::string dump() const {
        std::string ret = "{\n";

        if (!_items.empty()) {
            for (const auto &[k, v] : _items) ret += "    \"" + k + "\": " + v + ",\n";

            ret.erase(ret.end() - 2);
        }

        ret += "}";
        return ret;
    }
} jdump;

#define ALL(x) (x).begin(), (x).end()
#define FOR(i, begin, end) for (int i = (begin), i##_end_ = (end); i < i##_end_; i++)
#define IFOR(i, begin, end) for (int i = (end)-1, i##_begin_ = (begin); i >= i##_begin_; i--)
#define REP(i, n) FOR(i, 0, n)
#define IREP(i, n) IFOR(i, 0, n)

template <typename T> bool chmax(T &m, const T q) { return m < q ? (m = q, true) : false; }
template <typename T> bool chmin(T &m, const T q) { return m > q ? (m = q, true) : false; }

int floor_lg(long long x) { return x <= 0 ? -1 : 63 - __builtin_clzll(x); }
template <class T1, class T2> T1 floor_div(T1 num, T2 den) {
    return (num > 0 ? num / den : -((-num + den - 1) / den));
}
template <class T1, class T2> std::pair<T1, T2> operator+(const std::pair<T1, T2> &l, const std::pair<T1, T2> &r) {
    return std::make_pair(l.first + r.first, l.second + r.second);
}
template <class T1, class T2> std::pair<T1, T2> operator-(const std::pair<T1, T2> &l, const std::pair<T1, T2> &r) {
    return std::make_pair(l.first - r.first, l.second - r.second);
}
template <class T> std::vector<T> sort_unique(std::vector<T> vec) {
    sort(vec.begin(), vec.end()), vec.erase(unique(vec.begin(), vec.end()), vec.end());
    return vec;
}
template <class T> int arglb(const std::vector<T> &v, const T &x) {
    return std::distance(v.begin(), std::lower_bound(v.begin(), v.end(), x));
}
template <class T> int argub(const std::vector<T> &v, const T &x) {
    return std::distance(v.begin(), std::upper_bound(v.begin(), v.end(), x));
}
template <class IStream, class T> IStream &operator>>(IStream &is, std::vector<T> &vec) {
    for (auto &v : vec) is >> v;
    return is;
}

template <class OStream, class T> OStream &operator<<(OStream &os, const std::vector<T> &vec);
template <class OStream, class T, size_t sz> OStream &operator<<(OStream &os, const std::array<T, sz> &arr);
template <class OStream, class T, class TH> OStream &operator<<(OStream &os, const std::unordered_set<T, TH> &vec);
template <class OStream, class T, class U> OStream &operator<<(OStream &os, const std::pair<T, U> &pa);
template <class OStream, class T> OStream &operator<<(OStream &os, const std::deque<T> &vec);
template <class OStream, class T> OStream &operator<<(OStream &os, const std::set<T> &vec);
template <class OStream, class T> OStream &operator<<(OStream &os, const std::multiset<T> &vec);
template <class OStream, class T> OStream &operator<<(OStream &os, const std::unordered_multiset<T> &vec);
template <class OStream, class T, class U> OStream &operator<<(OStream &os, const std::pair<T, U> &pa);
template <class OStream, class TK, class TV> OStream &operator<<(OStream &os, const std::map<TK, TV> &mp);
template <class OStream, class TK, class TV, class TH>
OStream &operator<<(OStream &os, const std::unordered_map<TK, TV, TH> &mp);
template <class OStream, class... T> OStream &operator<<(OStream &os, const std::tuple<T...> &tpl);

template <class OStream, class T> OStream &operator<<(OStream &os, const std::vector<T> &vec) {
    os << '[';
    for (auto v : vec) os << v << ',';
    os << ']';
    return os;
}
template <class OStream, class T, size_t sz> OStream &operator<<(OStream &os, const std::array<T, sz> &arr) {
    os << '[';
    for (auto v : arr) os << v << ',';
    os << ']';
    return os;
}
template <class... T> std::istream &operator>>(std::istream &is, std::tuple<T...> &tpl) {
    std::apply([&is](auto &&...args) { ((is >> args), ...); }, tpl);
    return is;
}
template <class OStream, class... T> OStream &operator<<(OStream &os, const std::tuple<T...> &tpl) {
    os << '(';
    std::apply([&os](auto &&...args) { ((os << args << ','), ...); }, tpl);
    return os << ')';
}
template <class OStream, class T, class TH> OStream &operator<<(OStream &os, const std::unordered_set<T, TH> &vec) {
    os << '{';
    for (auto v : vec) os << v << ',';
    os << '}';
    return os;
}
template <class OStream, class T> OStream &operator<<(OStream &os, const std::deque<T> &vec) {
    os << "deq[";
    for (auto v : vec) os << v << ',';
    os << ']';
    return os;
}
template <class OStream, class T> OStream &operator<<(OStream &os, const std::set<T> &vec) {
    os << '{';
    for (auto v : vec) os << v << ',';
    os << '}';
    return os;
}
template <class OStream, class T> OStream &operator<<(OStream &os, const std::multiset<T> &vec) {
    os << '{';
    for (auto v : vec) os << v << ',';
    os << '}';
    return os;
}
template <class OStream, class T> OStream &operator<<(OStream &os, const std::unordered_multiset<T> &vec) {
    os << '{';
    for (auto v : vec) os << v << ',';
    os << '}';
    return os;
}
template <class OStream, class T, class U> OStream &operator<<(OStream &os, const std::pair<T, U> &pa) {
    return os << '(' << pa.first << ',' << pa.second << ')';
}
template <class OStream, class TK, class TV> OStream &operator<<(OStream &os, const std::map<TK, TV> &mp) {
    os << '{';
    for (auto v : mp) os << v.first << "=>" << v.second << ',';
    os << '}';
    return os;
}
template <class OStream, class TK, class TV, class TH>
OStream &operator<<(OStream &os, const std::unordered_map<TK, TV, TH> &mp) {
    os << '{';
    for (auto v : mp) os << v.first << "=>" << v.second << ',';
    os << '}';
    return os;
}

#ifdef HITONANODE_LOCAL
const std::string COLOR_RESET = "\033[0m", BRIGHT_GREEN = "\033[1;32m", BRIGHT_RED = "\033[1;31m",
                  BRIGHT_CYAN = "\033[1;36m", NORMAL_CROSSED = "\033[0;9;37m", RED_BACKGROUND = "\033[1;41m",
                  NORMAL_FAINT = "\033[0;2m";
#define dbg(x)                                                                                                        \
    std::cerr << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") "        \
              << __FILE__ << COLOR_RESET << std::endl
#define dbgif(cond, x)                                                                                                \
    ((cond) ? std::cerr << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__      \
                        << ") " << __FILE__ << COLOR_RESET << std::endl                                               \
            : std::cerr)
#else
#define dbg(x) 0
#define dbgif(cond, x) 0
#endif

#ifdef BENCHMARK
#define dump_onlinejudge(x) 0
struct setenv {
    setenv() { jdump.set_dump_at_end(); }
} setenv_;
#else
#define dump_onlinejudge(x) (std::cout << (x) << std::endl)
#endif

#include <cstdint>
#include <vector>

uint32_t rand_int() // XorShift random integer generator
{
    static uint32_t x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}
double rand_double() { return (double)rand_int() / UINT32_MAX; }

template <class T> void shuffle_vec(std::vector<T> &vec) {
    for (int i = 1; i < (int)vec.size(); ++i) {
        const int j = rand_int() % (i + 1);
        std::swap(vec.at(i), vec.at(j));
    }
}

#include <chrono>

class timer_ {
    std::chrono::system_clock::time_point start_;

public:
    timer_() : start_(now()) {}

    static std::chrono::system_clock::time_point now() { return std::chrono::system_clock::now(); }

    int spent_ms() const {
        auto diff = now() - start_;
        return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
    }
} timer;

using namespace std;
using lint = long long;
using pint = std::pair<int, int>;
using plint = std::pair<lint, lint>;

struct fast_ios {
    fast_ios() {
        std::cin.tie(nullptr), std::ios::sync_with_stdio(false), std::cout << std::fixed << std::setprecision(20);
    };
} fast_ios_;

constexpr int N = 200, M = 10;

struct Operation {
    int v;
    int from;
    int to;

    std::string to_str() const {
        std::string ret = std::to_string(v) + " " + std::to_string((to >= 0) ? (to + 1) : 0);
        return ret;
    }

    template <class OStream> friend OStream &operator<<(OStream &os, const Operation &op) {
        os << op.to_str();
        return os;
    }
};

struct State {
    int cost = 0;

    std::vector<std::vector<int>> B;
    std::vector<Operation> ops;
    vector<int> ds;

    int next_pop = 1;

    State() : B(M, std::vector<int>(N / M)) {
        ds.resize(M);
        iota(ALL(ds), 0);
    }

    void move(int v, int from, int to) {
        assert(0 <= from and from < M);
        assert(0 <= to and to < M);
        assert(from != to);

        int p = 0;
        while (B.at(from).at(p) != v) p++;
        cost += 1 + (int)B.at(from).size() - p;

        B.at(to).insert(B.at(to).end(), B.at(from).begin() + p, B.at(from).end());
        B.at(from).resize(p);

        ops.emplace_back(Operation{v, from, to});
    }

    void move_by_count(int length, int from, int to) {
        assert(0 <= from and from < M);
        assert(0 <= to and to < M);
        assert((int)B.at(from).size() >= length);
        int v = B.at(from).at(B.at(from).size() - length);

        const int old_to_len = B.at(to).size();
        move(v, from, to);
        assert((int)B.at(to).size() == old_to_len + length);
    }

    void pop(int v) {
        assert(next_pop == v);
        ++next_pop;
        REP(m, M) {
            if (B.at(m).size() and B.at(m).back() == v) {
                B.at(m).pop_back();
                ops.emplace_back(Operation{v, m, -1});
                return;
            }
        }

        assert(false);
    }

    void erase_v_fast(int v) {

        int cur = -1;
        REP(i, B.size()) if (count(ALL(B.at(i)), v)) {
            cur = i;
            break;
        }
        assert(cur != -1);

        if (v != B.at(cur).back()) {
            int i = find(ALL(B.at(cur)), v) - B.at(cur).begin() + 1;
            int u = B.at(cur).at(i);
            int nxt = -1;
            REP(i, M) if (cur != i and (nxt < 0 or B.at(nxt).size() > B.at(i).size())) nxt = i;

            move(u, cur, nxt);
        }

        pop(v);
    }

    void erase_v(int v) {
        int cur = -1;

        REP(i, B.size()) if (count(ALL(B.at(i)), v)) {
            cur = i;
            break;
        }
        assert(cur != -1);

        if (v != B.at(cur).back()) {
            const int c = find(ALL(B.at(cur)), v) - B.at(cur).begin();
            const int rem_sz = c + 1;

            vector<int> &vec = B.at(cur);
            while ((int)vec.size() > rem_sz) {
                int l = (int)vec.size() - 1;
                while (l > rem_sz and vec.at(l - 1) > vec.at(l)) {
                    --l;
                    // if ((int)vec.size() - l > 8) break;
                }

                // vec[l] 以降を move する
                std::vector<int> to_move(vec.begin() + l, vec.end());

                int best_dest = -1;
                int best_inv = 1 << 30;
                REP(m, M) {
                    if (m == cur) continue;
                    // int inv = 0;
                    int eval = 1000;
                    if (B.at(m).size()) {
                        if (B.at(m).back() > to_move.at(0))
                            eval = B.at(m).back() - to_move.at(0);
                        else {
                            eval = 10000;
                            for (int x : B.at(m)) {
                                for (int y : to_move) eval += x < y;
                            }
                        }
                    }
                    // for (int x : B.at(m)) {
                    //     for (int y : to_move) inv += x < y;
                    // }
                    if (chmin(best_inv, eval)) best_dest = m;
                }
                move(to_move.at(0), cur, best_dest);
            }
        }

        pop(v);
    }

    void presort_column(int m, vector<int> is_column_done) {

        shuffle_vec(ds);

        vector<vector<int>> state(M);
        state.at(m) = B.at(m);

        auto goal_vec = B.at(m);
        sort(goal_vec.rbegin(), goal_vec.rend());

        auto check_pop = [&]() {
            while (true) {
                bool found = false;
                const int tgtval = next_pop;
                REP(d, M) {
                    if (B.at(d).size() and B.at(d).back() == tgtval) {
                        // dbg(tgtval);
                        if (state.at(d).size() and state.at(d).back() == tgtval) state.at(d).pop_back();
                        pop(tgtval);
                        if (goal_vec.size() and goal_vec.back() == tgtval) goal_vec.pop_back();
                        found = true;
                        break;
                    }
                }
                if (!found) break;
            }
        };

        check_pop();

        auto try_move = [&](int from, int l, int ngdest) -> void {
            const vector<int> to_be_moved{state.at(from).begin() + l, state.at(from).end()};

            const int bottom_goal_pos = find(ALL(goal_vec), state.at(from).at(l)) - goal_vec.begin();
            int bottom_preferred = -1;
            if (bottom_goal_pos) bottom_preferred = goal_vec.at(bottom_goal_pos - 1);

            int best_tgt = -1;
            int best_val = -1e9;

            constexpr int DONE_VAL = 1e7;

            for (int d : ds) {
                if (d == from) continue;
                if (d == ngdest) continue;

                int eval = -1e9;

                if (state.at(d).empty()) {
                    eval = 0;
                    if (B.at(d).size() and B.at(d).back() > to_be_moved.front()) {
                        eval += 500 - (B.at(d).back() - to_be_moved.front());
                    }
                } else {
                    if (state.at(d).back() < to_be_moved.front()) {
                        eval = state.at(d).back(); // 既にある値が大きい方がうれしい
                    } else {
                        // 既に突っ込んでいて、しかも逆転する　全然うれしくない
                        eval = -1e7;
                        for (int x : state.at(d)) {
                            for (int y : to_be_moved) eval -= x > y; // ?
                        }
                    }
                }

                if (B.at(d).size() and B.at(d).back() == bottom_preferred) eval = 1e5;

                if (B.at(d).empty() or B.at(d).back() < bottom_preferred)
                    chmax(eval, 100000000);

                if (is_column_done.at(d) and B.at(d).size() and to_be_moved.size() == 1 and state.at(d).empty() and
                    B.at(d).back() > to_be_moved.front() and B.at(d).back() - to_be_moved.front() <= 20)
                    chmax(eval, DONE_VAL);
                if (chmax(best_val, eval)) best_tgt = d;
            }

            assert(best_tgt != -1);

            move_by_count((int)state.at(from).size() - l, from, best_tgt);

            if (best_val == DONE_VAL) {
                for (int v : to_be_moved) { goal_vec.erase(find(ALL(goal_vec), v)); }
            } else {
                state.at(best_tgt).insert(state.at(best_tgt).end(), ALL(to_be_moved));
            }
            state.at(from).resize(l);
        };

        // 排出
        while (true) {
            const int current_len = state.at(m).size();
            {
                auto sub = goal_vec;
                sub.resize(current_len);
                if (sub == state.at(m)) break;
            }
            int l = state.at(m).size() - 1;
            while (l > 0 and state.at(m).at(l - 1) != next_pop) {
                bool good = false;

                if (state.at(m).at(l - 1) < state.at(m).at(l)) {
                    good = true;
                } else {
                    int p = find(ALL(goal_vec), state.at(m).at(l)) - goal_vec.begin();
                    int q = find(ALL(goal_vec), state.at(m).at(l - 1)) - goal_vec.begin();
                    if (abs(p - q) <= 1) good = true;
                }

                if (good) {
                    --l;
                } else {
                    break;
                }
            }

            try_move(m, l, -1);

            check_pop();
        }

        // 注入
        while (true) {
            const int i = state.at(m).size();

            const int v = i < (int)goal_vec.size() ? goal_vec.at(i) : 0; // 次に突っ込む値

            int prv = N + 1;
            if (state.at(m).size()) prv = state.at(m).back();

            {
                // 他の未処理列から持ってきてしまう
                int pending_hi = -1;
                int arghi = -1;
                REP(d, M) {
                    if (is_column_done.at(d)) continue;
                    if (B.at(d).empty()) continue;
                    if (state.at(d).size()) continue;
                    int w = B.at(d).back();
                    if (prv > w and w > v and chmax(pending_hi, w)) arghi = d;
                }

                if (arghi != -1) {

                    int len = 1;
                    while (true) {
                        if (len == (int)B.at(arghi).size()) break;
                        if (B.at(arghi).at(B.at(arghi).size() - len - 1) < B.at(arghi).at(B.at(arghi).size() - len))
                            break;
                        if (B.at(arghi).at(B.at(arghi).size() - len - 1) > prv) break;
                        ++len;
                    }
                    goal_vec.insert(goal_vec.begin() + i, B.at(arghi).end() - len, B.at(arghi).end());
                    state.at(m).insert(state.at(m).end(), B.at(arghi).end() - len, B.at(arghi).end());
                    move_by_count(len, arghi, m);
                    continue;
                }
            }

            if (i == (int)goal_vec.size()) break;

            int from = -1;
            REP(d, M) {
                if (count(ALL(state.at(d)), v)) {
                    from = d;
                    break;
                }
            }
            assert(from != -1);

            int l = find(ALL(state.at(from)), v) - state.at(from).begin();

            bool all_conn = true;
            int lpos = find(ALL(goal_vec), v) - goal_vec.begin();
            FOR(i, l + 1, state.at(from).size()) {
                int rpos = find(ALL(goal_vec), state.at(from).at(i)) - goal_vec.begin();
                if (rpos != lpos + 1) {
                    all_conn = false;
                    break;
                }
                lpos = rpos;
            }

            if (is_column_done.at(from) and state.at(from).size() == 1 and
                (B.at(from).size() == 1 or B.at(from).at(B.at(from).size() - 2) > v)) {
                goal_vec.erase(find(ALL(goal_vec), v));
                state.at(from).pop_back();
            } else if (all_conn) {
                int cnt = state.at(from).size() - l;
                int prv = N + 1;

                vector<int> added;
                if (B.at(m).size()) prv = B.at(m).back();
                while (cnt < (int)B.at(from).size()) {
                    const int a = B.at(from).at(B.at(from).size() - cnt - 1);
                    if (a > prv) break;
                    if (a < B.at(from).at(B.at(from).size() - cnt)) break;
                    added.push_back(a);
                    ++cnt;
                }

                if (added.size()) {
                    for (int a : added) goal_vec.insert(goal_vec.begin() + i, a);
                    sort(goal_vec.rbegin(), goal_vec.rend());
                }

                state.at(m).insert(
                    state.at(m).end(), B.at(from).end() - min<int>(cnt, B.at(from).size()), B.at(from).end());
                REP(_, cnt) {
                    if (state.at(from).size()) state.at(from).pop_back();
                }
                move_by_count(cnt, from, m);
            } else {
                try_move(from, l + 1, m);
                continue;
            }
            check_pop();
        }
    }

    int score() const { return max(1, 10000 - cost); }
};

int main(int argc, char *argv[]) {

    State init_state;
    {
        int N_, M_;
        cin >> N_ >> M_;
        assert(N_ == N);
        assert(M_ == M);
        vector B(M, vector<int>(N / M));
        cin >> B;
        init_state.B = B;
    }

    State best_state;
    int best_score = -1;

    vector<int> best_seq;
    {
        auto state = init_state;
        vector<int> is_column_done(M);

        REP(_, M) {
            int mi = 1e9, argmi = -1;
            REP(m, M) {
                if (is_column_done.at(m)) continue;
                int tmp = *min_element(ALL(state.B.at(m)));
                if (chmin(mi, tmp)) argmi = m;
            }

            best_seq.push_back(argmi);

            state.presort_column(argmi, is_column_done);
            is_column_done.at(argmi) = 1;
        }
        FOR(v, state.next_pop, N + 1) state.erase_v(v);
        dbg(state.score());

        if (chmax(best_score, state.score())) best_state = state;
    }

    int iter = 0, last_upd_iter = 0;
    while (timer.spent_ms() < 1980) {
        ++iter;
        auto seq = best_seq;
        REP(_, 1 + (iter - last_upd_iter > 10000)) {
            int l = rand_int() % M, r = rand_int() % M;
            if (l > r) swap(l, r);
            r += 1;
            std::reverse(seq.begin() + l, seq.begin() + r);
        }

        auto state = init_state;
        vector<int> is_column_done(M);
        for (int argmi : seq) {
            state.presort_column(argmi, is_column_done);
            is_column_done.at(argmi) = 1;
        }

        FOR(v, state.next_pop, N + 1) state.erase_v(v);

        if (chmax(best_score, state.score())) {
            dbg(make_tuple(iter, best_score));
            best_state = state;
            best_seq = seq;
            last_upd_iter = iter;
        }
    }

    for (auto &op : best_state.ops) dump_onlinejudge(op.to_str());

    // if (argc >= 2) { X = std::stoi(argv[1]); }

    jdump("score", best_state.score());
    dbg(best_state.score());
    // cerr << iter << endl;
}
