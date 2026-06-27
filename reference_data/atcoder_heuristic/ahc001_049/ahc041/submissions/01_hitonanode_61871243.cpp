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
#define IFOR(i, begin, end) for (int i = (end) - 1, i##_begin_ = (begin); i >= i##_begin_; i--)
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

using namespace std;
using lint = long long;
using pint = std::pair<int, int>;
using plint = std::pair<lint, lint>;

struct fast_ios {
    fast_ios() {
        std::cin.tie(nullptr), std::ios::sync_with_stdio(false), std::cout << std::fixed << std::setprecision(20);
    };
} fast_ios_;

constexpr int N = 1000;
constexpr int H = 10;

using BS = bitset<1000>;

// level : 次に決める
vector<vector<int>> gen_next_states(const array<int, N> &A, const vector<vector<int>> &to, const vector<int> &init_state, int level) {
    assert(0 <= level and level <= H);

    if (level == H) {
        vector<int> ret;
        REP(i, N) if (init_state.at(i) == -1) ret.push_back(i);
        return {ret};
    }

    array<array<vector<int>, N>, H + 1> di2j;  // di2j[d][i] = i からちょうど d 歩かかる

    vector<int> frontiers;
    vector<int> lasts;

    if (level == 0) {
        REP(i, N) frontiers.push_back(i);
    } else {
        REP(i, N) {
            if (init_state[i] == level - 1) {
                lasts.push_back(i);
                for (int j : to.at(i)) {
                    if (init_state[j] == -1) {
                        frontiers.push_back(j);
                    }
                }
            }
        }
    }
    frontiers = sort_unique(frontiers);

    sort(ALL(frontiers), [&](int i, int j) { return A[i] < A[j]; });


    BS is_invalid;
    REP(i, N) {
        if (init_state[i] >= 0 and init_state[i] < level) is_invalid.set(i);
    }

    for (int i : frontiers) {
        vector<int> st{i}, next_st;

        BS visited = is_invalid;
        visited.set(i);
        di2j.at(0).at(i).push_back(i);

        for (int s = 0; s + level < H; ++s) {
            next_st.clear();

            for (int j : st) {
                for (int k : to.at(j)) {
                    if (visited.test(k)) continue;
                    visited.set(k);
                    next_st.push_back(k);
                    di2j.at(s + 1).at(i).push_back(k);
                }
            }
            swap(st, next_st);
        }
    }

    vector<int> cntr(N);
    REP(i, N) if (is_invalid.test(i)) cntr.at(i) = 1;

    vector<int> cands;
    vector<BS> bss;

    int timelimit = 10000;
    for (int i : frontiers) {
        bool used = false;
        REP(d, H - level + 1) {
            for (int j : di2j.at(d).at(i)) {
                if (cntr.at(j) == 0) used = true;
            }
        }

        if (used) {
            BS add = is_invalid;
            REP(d, H - level + 1) {
                for (int j : di2j.at(d).at(i)) {
                    cntr.at(j)++;
                    add.set(j);
                }
            }
            cands.push_back(i);
            bss.push_back(add);
        }
        if (*min_element(ALL(cntr)) > 0) {
            chmin(timelimit, 20);
        }
        --timelimit;
        if (timelimit < 0) break;
    }

    vector<vector<int>> ret;

    vector<int> minimal;
    vector<int> notminimal;

    vector<int> used_minimal(cands.size());

    {
        IREP(d, cands.size()) {
            const int i = cands.at(d);
            bool req = false;
            REP(d, H - level + 1) {
                for (int j : di2j.at(d).at(i)) {
                    if (cntr.at(j) <= 1) req = true;
                }
            }
            if (req) {
                minimal.push_back(i);
                used_minimal.at(d) = 1;
            } else {
                REP(d, H - level + 1) {
                    for (int j : di2j.at(d).at(i)) { cntr.at(j)--; }
                }
                notminimal.push_back(i);
            }
        }

        reverse(ALL(minimal));

        ret.push_back(minimal);
    }

    if (level < 5) {

        BS is1;
        REP(i, N) {
            if (!is_invalid.test(i) and cntr.at(i) == 1) is1.set(i);
        }

        REP(d, cands.size()) {
            if (used_minimal.at(d)) {
                const BS tight = is1 & bss.at(d);
                REP(e, cands.size()) {
                    if (!used_minimal.at(e)) {
                        if ((tight & bss.at(e)) == tight) {
                            auto tmp = minimal;
                            tmp.erase(find(ALL(tmp), cands.at(d)));
                            tmp.push_back(cands.at(e));
                            ret.push_back(tmp);
                        }

                        // REP(f, e) {
                        //     if (!used_minimal.at(f)) {
                        //         if (((tight & (bss.at(e) | bss.at(f))) == tight)) {
                        //             auto tmp = minimal;
                        //             tmp.erase(find(ALL(tmp), cands.at(d)));
                        //             tmp.push_back(cands.at(e));
                        //             tmp.push_back(cands.at(f));
                        //             ret.push_back(tmp);
                        //         }
                        //     }
                        // }
                    }
                }
            }

            if (!used_minimal.at(d)) {
                auto tmp = minimal;
                tmp.push_back(cands.at(d));
                ret.push_back(tmp);
            }
        }
    }

    return ret;
}

vector<int> encode(const vector<vector<int>> &to, const vector<int> &state) {
    vector<int> ret(N, -1);
    REP(i, N) {
        for (int j : to.at(i)) {
            if (state.at(j) == state.at(i) - 1) ret.at(i) = j;
        }
    }

    return ret;
}

string gen(const vector<int> &encoded) {
    string ret;
    for (int x : encoded) ret += to_string(x) + ' ';
    ret.pop_back();
    return ret;
}

int calc_score(const array<int, N> &A, const vector<int> &state) {
    int ret = 1;
    REP(i, N) ret += A.at(i) * (1 + state.at(i));
    return ret;
}

int eval_state(const array<int, N> &A, const vector<int> &state, int h) {
    int ret = 0;
    double e = (h > 9 ? 11 : 10.5);
    REP(i, N) {
        if (state.at(i) == -1) {
            ret -= A.at(i) * e;
        } else {
            ret -= A.at(i) * (1 + state.at(i));
        }
    }
    return ret;
}

#include <chrono>
#include <random>

struct rand_int_ {
    using lint = long long;
    std::mt19937 mt;
    rand_int_() : mt(std::chrono::steady_clock::now().time_since_epoch().count()) {}
    lint operator()(lint x) { return this->operator()(0, x); } // [0, x)
    lint operator()(lint l, lint r) {
        std::uniform_int_distribution<lint> d(l, r - 1);
        return d(mt);
    }
} rnd;


int main(int argc, char *argv[]) {


    array<int, N> A;
    array<unsigned long long, N> frontier_hash;
    REP(i, N) frontier_hash.at(i) = rnd(1LL << 62);

    auto get_hash = [&](int level, const vector<int> &state) {
        unsigned long long ret = 0;
        REP(i, N) if (state.at(i) == level) ret ^= frontier_hash.at(i);
        return ret;
    };

    int M;
    {
        int N_, H_;
        cin >> N_ >> M >> H_;
    }

    for (auto &a : A) cin >> a;

    vector<vector<int>> to(N);

    REP(e, M) {
        int a, b;
        cin >> a >> b;
        to.at(a).push_back(b);
        to.at(b).push_back(a);
    }

    vector<pair<int, vector<int>>> states;
    {
        vector<int> state(N, -1);
        states.emplace_back(0, state);
    }

    FOR(h, 0, H + 1) {
        constexpr int BW = 400;
        dbg(make_tuple(h, states.size()));

        unordered_map<unsigned long long, pair<int, vector<int>>> mp;

        for (const auto &[v, s] : states) {
            REP(iter, (h < 2) ? 12 : 1) {
                auto Atmp = A;
                if (iter > 0) {
                    REP(i, N) Atmp.at(i) += rnd(h < 6 ? 50 : 100);
                }

                auto cands = gen_next_states(Atmp, to, s, h);

                for (auto &cand : cands) {
                    vector<int> nxts = s;
                    for (int i : cand) nxts.at(i) = h;

                    auto hash = get_hash(h, nxts);
                    int e = eval_state(A, nxts, h);
                    if (mp.count(hash)) {
                        chmin(mp[hash], make_pair(e, nxts));
                    } else {
                        mp[hash] = {e, nxts};
                    }
                }
            }
        }

        vector<pair<int, vector<int>>> next_states;
        for (const auto &[_, s] : mp) next_states.push_back(s);
        const int sz = min(BW, (int)next_states.size());
        std::partial_sort(next_states.begin(), next_states.begin() + sz, next_states.end());
        next_states.resize(sz);
        swap(states, next_states);
    }

    auto state = states.front().second;

    dump_onlinejudge(gen(encode(to, state)));
    dbg(calc_score(A, state));

    jdump("score", calc_score(A, state));
}
