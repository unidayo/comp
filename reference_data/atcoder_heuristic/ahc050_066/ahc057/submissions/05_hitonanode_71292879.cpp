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
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

struct PointState {
    double t;
    double x;
    double y;
    double vx;
    double vy;
};

namespace {

constexpr double EPS = 1e-9;

double normalize_position(double value, double modulo) {
    double res = std::fmod(value, modulo);
    if (res < 0.0) { res += modulo; }
    if (res >= modulo) { res -= modulo; }
    return res;
}

double squared_distance_at_time(const PointState &a, const PointState &b, int t, double L) {
    double ax = normalize_position(a.x + a.vx * (t - a.t), L);
    double ay = normalize_position(a.y + a.vy * (t - a.t), L);
    double bx = normalize_position(b.x + b.vx * (t - b.t), L);
    double by = normalize_position(b.y + b.vy * (t - b.t), L);
    double dx = std::remainder(ax - bx, L);
    double dy = std::remainder(ay - by, L);
    return dx * dx + dy * dy;
}

std::vector<double> gather_wrap_events(double origin, double v_rel, int tlo, int thi, double L) {
    std::vector<double> events;
    if (std::abs(v_rel) < EPS || tlo + 1 >= thi) { return events; }
    double val_lo = origin + v_rel * tlo;
    double val_hi = origin + v_rel * (thi - 1);
    double mn = std::min(val_lo, val_hi);
    double mx = std::max(val_lo, val_hi);
    double ratio_min = mn / L;
    double ratio_max = mx / L;
    int m_start = static_cast<int>(std::ceil(ratio_min - 0.5 - EPS));
    int m_end = static_cast<int>(std::floor(ratio_max - 0.5 + EPS));
    if (m_start > m_end) { return events; }
    for (int m = m_start; m <= m_end; ++m) {
        double t_cross = ((m + 0.5) * L - origin) / v_rel;
        if (t_cross >= tlo - EPS && t_cross <= thi + EPS) { events.push_back(t_cross); }
    }
    return events;
}

long long wrap_index(double value, double L) { return static_cast<long long>(std::floor(value / L + 0.5)); }

int floor_to_int(double value) { return static_cast<int>(std::floor(value + EPS)); }

int ceil_to_int(double value) { return static_cast<int>(std::ceil(value - EPS)); }

} // namespace

int find_closest_approach_time(const PointState &a, const PointState &b, int tlo, int thi, double L) {
    if (tlo >= thi) { return tlo; }

    double vx_rel = a.vx - b.vx;
    double vy_rel = a.vy - b.vy;
    double origin_x = (a.x - a.vx * a.t) - (b.x - b.vx * b.t);
    double origin_y = (a.y - a.vy * a.t) - (b.y - b.vy * b.t);

    std::vector<double> events_x = gather_wrap_events(origin_x, vx_rel, tlo, thi, L);
    std::vector<double> events_y = gather_wrap_events(origin_y, vy_rel, tlo, thi, L);

    std::vector<int> candidates;
    auto add_candidate = [&](int t) {
        if (t >= tlo && t < thi) { candidates.push_back(t); }
    };

    add_candidate(tlo);
    add_candidate(thi - 1);
    for (double e : events_x) {
        add_candidate(floor_to_int(e));
        add_candidate(ceil_to_int(e));
    }
    for (double e : events_y) {
        add_candidate(floor_to_int(e));
        add_candidate(ceil_to_int(e));
    }

    std::vector<double> cuts{static_cast<double>(tlo), static_cast<double>(thi)};
    cuts.insert(cuts.end(), events_x.begin(), events_x.end());
    cuts.insert(cuts.end(), events_y.begin(), events_y.end());
    std::sort(cuts.begin(), cuts.end());
    cuts.erase(std::unique(cuts.begin(), cuts.end(), [](double lhs, double rhs) { return std::abs(lhs - rhs) < 1e-8; }),
               cuts.end());

    double denom = vx_rel * vx_rel + vy_rel * vy_rel;
    for (size_t i = 0; i + 1 < cuts.size(); ++i) {
        double seg_lo = std::max(static_cast<double>(tlo), cuts[i]);
        double seg_hi = std::min(static_cast<double>(thi), cuts[i + 1]);
        if (seg_hi - seg_lo <= 1e-8) { continue; }
        double sample = (seg_lo + seg_hi) * 0.5;
        long long kx = wrap_index(origin_x + vx_rel * sample, L);
        long long ky = wrap_index(origin_y + vy_rel * sample, L);
        double shift_x = origin_x - kx * L;
        double shift_y = origin_y - ky * L;
        if (denom > EPS) {
            double t_star = -(shift_x * vx_rel + shift_y * vy_rel) / denom;
            double clamped = std::max(seg_lo, std::min(seg_hi, t_star));
            add_candidate(floor_to_int(clamped));
            add_candidate(ceil_to_int(clamped));
        }
        add_candidate(ceil_to_int(seg_lo));
        add_candidate(floor_to_int(seg_hi - EPS));
    }

    std::sort(candidates.begin(), candidates.end());
    candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

    double best_dist = std::numeric_limits<double>::infinity();
    int best_time = tlo;
    for (int t : candidates) {
        double dist = squared_distance_at_time(a, b, t, L);
        if (dist + 1e-9 < best_dist || (std::abs(dist - best_dist) <= 1e-9 && t < best_time)) {
            best_dist = dist;
            best_time = t;
        }
    }
    return best_time;
}

#pragma once
#include <cassert>
#include <queue>
#include <tuple>
#include <vector>

namespace linear_sum_assignment {

template <class T> struct Result {
    T opt;
    std::vector<int> mate;
    std::vector<T> f, g; // dual variables
};

template <class T>
T augment(int nr, int nc, const std::vector<std::vector<T>> &C, std::vector<T> &f, std::vector<T> &g,
          int s, // source row
          std::vector<int> &mate,
          std::vector<int> &mate_inv, // duplicates are allowed (used for k-best algorithms)
          int fixed_rows = 0 // Ignore first rows and corresponding columns (used for k-best algorithms)
) {

    assert(0 <= s and s < nr);
    assert(mate.at(s) < 0);

    static std::vector<T> dist;
    static std::vector<int> prv;
    dist.resize(nc);
    prv.resize(nc);

    std::vector<bool> done(nc);

    for (int i = 0; i < fixed_rows; ++i) {
        if (int j = mate.at(i); j >= 0) done.at(j) = 1;
    }

    {
        int h = 0;
        while (done.at(h)) ++h;

        f.at(s) = C.at(s).at(h) - g.at(h);
        for (int j = h + 1; j < nc; ++j) {
            if (done.at(j)) continue;
            f.at(s) = std::min(f.at(s), C.at(s).at(j) - g.at(j));
        }
    }

    for (int j = 0; j < nc; ++j) {
        if (!done.at(j)) {
            dist.at(j) = C.at(s).at(j) - f.at(s) - g.at(j);
            prv.at(j) = -1;
        }
    }

    int t = -1;
    std::vector<int> stk;

    while (t == -1) {
        int j1 = -1;
        for (int j = 0; j < nc; ++j) {
            if (done.at(j)) continue;
            if (j1 == -1 or dist.at(j) < dist.at(j1) or
                (dist.at(j) == dist.at(j1) and mate_inv.at(j) < 0)) {
                j1 = j;
            }
        }

        if (mate_inv.at(j1) < 0) {
            t = j1;
            break;
        }

        done.at(j1) = 1;
        stk = {j1};

        while (!stk.empty()) {
            const int j2 = stk.back();
            const int i = mate_inv.at(j2);
            if (i < 0) {
                t = stk.back();
                break;
            }
            stk.pop_back();

            for (int j = 0; j < nc; ++j) {
                if (done.at(j)) continue;

                const T len = C.at(i).at(j) - f.at(i) - g.at(j);

                if (dist.at(j) > dist.at(j1) + len) {
                    dist.at(j) = dist.at(j1) + len;
                    prv.at(j) = j2;
                }

                if (len == T()) {
                    stk.push_back(j);
                    done.at(j) = 1;
                }
            }
        }
    }

    const T len = dist.at(t);

    f.at(s) += len;

    for (int i = 0; i < fixed_rows; ++i) {
        if (const int j = mate.at(i); j >= 0) done.at(j) = 0;
    }

    for (int j = 0; j < nc; ++j) {
        if (!done.at(j)) continue;

        g.at(j) -= len - dist.at(j);
    }

    for (int i = fixed_rows; i < nr; ++i) {
        const int j = mate.at(i);
        if (j < 0 or !done.at(j) or j >= nc) continue;
        f.at(i) += len - dist.at(j);
    }

    T ret = T();

    for (int cur = t; cur >= 0;) {
        const int nxt = prv.at(cur);
        if (nxt < 0) {
            mate_inv.at(cur) = s;
            mate.at(s) = cur;
            ret += C.at(s).at(cur);
            break;
        }
        const int i = mate_inv.at(nxt);

        ret += C.at(i).at(cur) - C.at(i).at(nxt);

        mate_inv.at(cur) = i;
        mate.at(i) = cur;
        cur = nxt;
    }

    return ret;
}

// Complexity: O(nr^2 nc)
template <class T> Result<T> _solve(int nr, int nc, const std::vector<std::vector<T>> &C) {

    assert(nr <= nc);

    std::vector<int> mate(nr, -1);
    std::vector<int> mate_inv(nc, -1);
    std::vector<T> f(nr), g(nc); // dual variables, f[i] + g[j] <= C[i][j] holds

    if (nr == 0 or nc == 0) return {T(), mate, f, g};

    if (nr == nc) {
        // Column reduction
        for (int j = nc - 1; j >= 0; --j) {
            g.at(j) = C.at(0).at(j) - f.at(0);
            int imin = 0;
            for (int i = 1; i < nr; ++i) {
                if (g.at(j) > C.at(i).at(j) - f.at(i)) {
                    imin = i;
                    g.at(j) = C.at(i).at(j) - f.at(i);
                }
            }

            if (mate.at(imin) < 0) {
                mate.at(imin) = j;
                mate_inv.at(j) = imin;
            } else if (g.at(j) < g.at(mate.at(imin))) {
                mate_inv.at(mate.at(imin)) = -1;
                mate.at(imin) = j;
                mate_inv.at(j) = imin;
            }
        }

        // Reduction transfer (can be omitted)
        if (nc > 1) {
            for (int i = 0; i < nr; ++i) {
                if (mate.at(i) < 0) continue;
                T best = C.at(i).at(0) - g.at(0), second_best = C.at(i).at(1) - g.at(1);
                int argbest = 0;
                if (best > second_best) std::swap(best, second_best), argbest = 1;

                for (int j = 2; j < nc; ++j) {
                    if (T val = C.at(i).at(j) - g.at(j); val < best) {
                        second_best = best;
                        best = val;
                        argbest = j;
                    } else if (val < second_best) {
                        second_best = val;
                    }
                }

                g.at(argbest) -= second_best - best;
                f.at(i) = second_best;
            }
        }

        // Augmenting row reduction: not implemented
    }

    // Augmentation
    for (int i = 0; i < nr; ++i) {
        if (mate.at(i) < 0) augment(nr, nc, C, f, g, i, mate, mate_inv);
    }

    T ret = 0;
    for (int i = 0; i < nr; ++i) ret += C.at(i).at(mate.at(i));

    return {ret, mate, std::move(f), std::move(g)};
}

// Jonker–Volgenant algorithm: find minimum weight assignment
// Dual problem (nr == nc): maximize sum(f) + sum(g) s.t. f_i + g_j <= C_ij
// Complexity: O(nr nc min(nr, nc))
template <class T> Result<T> solve(int nr, int nc, const std::vector<std::vector<T>> &C) {

    const bool transpose = (nr > nc);

    if (!transpose) return _solve(nr, nc, C);

    std::vector trans(nc, std::vector<T>(nr));

    for (int i = 0; i < nr; ++i) {
        for (int j = 0; j < nc; ++j) trans.at(j).at(i) = C.at(i).at(j);
    }

    auto [v, mate, f, g] = _solve(nc, nr, trans);

    std::vector<int> mate2(nr, -1);
    for (int j = 0; j < nc; ++j) {
        if (mate.at(j) >= 0) mate2.at(mate.at(j)) = j;
    }

    return {v, mate2, g, f};
}

} // namespace linear_sum_assignment

template <class T> struct best_assignments {

    struct Node {
        T opt;
        std::vector<int> mate;
        std::vector<T> f, g; // dual variables
        int fixed_rows;
        std::vector<int> banned_js; // C[fixed_rows][j] が inf となる j の集合

        // for priority queue
        // NOTE: reverse order
        bool operator<(const Node &rhs) const { return opt > rhs.opt; }

        linear_sum_assignment::Result<T> to_output(bool transpose) const {
            if (transpose) {
                std::vector<int> mate2(g.size(), -1);
                for (int i = 0; i < (int)mate.size(); ++i) mate2.at(mate.at(i)) = i;
                return {opt, mate2, g, f};
            } else {
                return {opt, mate, f, g};
            }
        }
    };

    bool transpose;
    int nr_, nc_;
    T inf;
    std::vector<std::vector<T>> C_, Ctmp_;
    std::priority_queue<Node> pq;

    best_assignments(int nr, int nc, const std::vector<std::vector<T>> &C, T inf)
        : transpose(nr > nc), inf(inf) {

        assert((int)C.size() == nr);
        for (int i = 0; i < nr; ++i) assert((int)C.at(i).size() == nc);

        nr_ = transpose ? nc : nr;
        nc_ = transpose ? nr : nc;

        C_.assign(nr_ + (nr_ != nc_), std::vector<T>(nc_, T()));
        for (int i = 0; i < nr; ++i) {
            for (int j = 0; j < nc; ++j) {
                C_.at(transpose ? j : i).at(transpose ? i : j) = C.at(i).at(j);
            }
        }

        Ctmp_ = C_;

        auto [opt, mate, f, g] = linear_sum_assignment::solve(C_.size(), nc, C_);

        pq.emplace(Node{opt, std::move(mate), std::move(f), std::move(g), 0, {}});
    }

    bool finished() const { return pq.empty(); }

    linear_sum_assignment::Result<T> yield() {
        assert(!pq.empty());

        const Node ret = pq.top();
        pq.pop();

        for (int fixed_rows = ret.fixed_rows; fixed_rows < nr_; ++fixed_rows) {
            std::vector<int> banned_js;
            if (fixed_rows == ret.fixed_rows) banned_js = ret.banned_js;

            const int s = fixed_rows;
            banned_js.push_back(ret.mate.at(s));

            if ((int)banned_js.size() >= nc_) continue;

            auto f = ret.f;
            auto g = ret.g;
            auto mate = ret.mate;

            std::vector<int> mate_inv(nc_, nr_);
            for (int i = 0; i < nr_; ++i) mate_inv.at(mate.at(i)) = i;

            std::vector<int> iscoldone(nc_);
            for (int i = 0; i < fixed_rows; ++i) iscoldone.at(mate.at(i)) = 1;

            for (int j : banned_js) Ctmp_.at(s).at(j) = inf;

            mate_inv.at(mate.at(s)) = -1;
            mate.at(s) = -1;

            auto aug = linear_sum_assignment::augment(
                nr_, nc_, Ctmp_, f, g, s, mate, mate_inv, fixed_rows);

            for (int j = 0; j < nc_; ++j) {
                if (mate_inv.at(j) < 0) { // nrows < ncols
                    g.at(j) = -f.back();
                    for (int i = fixed_rows; i < nr_; ++i) {
                        g.at(j) = std::min(g.at(j), Ctmp_.at(i).at(j) - f.at(i));
                    }
                }
            }

            if (Ctmp_.at(s).at(mate.at(s)) < inf) {
                pq.emplace(Node{
                    ret.opt + aug - C_.at(s).at(ret.mate.at(s)),
                    std::move(mate),
                    std::move(f),
                    std::move(g),
                    fixed_rows,
                    banned_js,
                });
            }

            for (int j : banned_js) Ctmp_.at(s).at(j) = C_.at(s).at(j);
        }

        return ret.to_output(transpose);
    }
};


using namespace std;
using lint = long long;
using pint = std::pair<int, int>;
using plint = std::pair<lint, lint>;

struct fast_ios {
    fast_ios() {
        std::cin.tie(nullptr), std::ios::sync_with_stdio(false), std::cout << std::fixed << std::setprecision(20);
    };
} fast_ios_;

constexpr int N = 300, T = 1000, M = 10, K = 30, L = 100000;

double Normalize(double x) {
    while (x < 0) x += L;
    while (x >= L) x -= L;
    return x;
}
struct Point {
    double x, y, vx, vy;
    template <class OStream>
    friend OStream &operator<<(OStream &os, const Point &p) {
        return os << "(" << p.x << "," << p.y << "," << p.vx << "," << p.vy << ")";
    }

    Point After(int steps) const {
        const double ret_x = Normalize(x + vx * steps);
        const double ret_y = Normalize(y + vy * steps);
        return Point{ret_x, ret_y, vx, vy};
    }
};

double Distance(const Point &a, const Point &b) {
    double dx = Normalize(a.x - b.x);
    double dy = Normalize(a.y - b.y);
    dx = min(dx, L - dx);
    dy = min(dy, L - dy);
    return sqrt(dx * dx + dy * dy);
}

// UnionFind Tree (0-indexed), based on size of each disjoint set
struct UnionFind {
    std::vector<int> par, cou;
    UnionFind(int N = 0) : par(N), cou(N, 1) { iota(par.begin(), par.end(), 0); }
    int find(int x) { return (par[x] == x) ? x : (par[x] = find(par[x])); }
    bool unite(int x, int y) {
        x = find(x), y = find(y);
        if (x == y) return false;
        if (cou[x] < cou[y]) std::swap(x, y);
        par[y] = x, cou[x] += cou[y];
        return true;
    }
    int count(int x) { return cou[find(x)]; }
    bool same(int x, int y) { return find(x) == find(y); }
    std::vector<std::vector<int>> groups() {
        std::vector<std::vector<int>> ret(par.size());
        for (int i = 0; i < int(par.size()); ++i) ret[find(i)].push_back(i);
        ret.erase(std::remove_if(ret.begin(), ret.end(),
                                 [&](const std::vector<int> &v) { return v.empty(); }),
                  ret.end());
        return ret;
    }
};


using Ops = vector<tuple<int, int, int>>;

string ToAnswer(const Ops &answer) {
    ostringstream oss;
    for (const auto &[t, i, j] : answer) { oss << t << ' ' << i << ' ' << j << '\n'; }
    auto ret = oss.str();
    if (!ret.empty() && ret.back() == '\n') ret.pop_back();
    return ret;
}

double EnergyLossRatio(int w1, int w2, const Point &p1, const Point &p2) {
    double before = (p1.vx * p1.vx + p1.vy * p1.vy) * w1 + (p2.vx * p2.vx + p2.vy * p2.vy) * w2;
    double mx = (p1.vx * w1 + p2.vx * w2) / (w1 + w2);
    double my = (p1.vy * w1 + p2.vy * w2) / (w1 + w2);
    double after = (mx * mx + my * my) * (w1 + w2);
    return (before - after) / before;
}

void MergeSpeeds(int i, int j, int tick, vector<int> &last_ticks, UnionFind &uf, vector<Point> &points, const vector<int> &all_indices) {
    const double mx = (points[uf.find(i)].vx * uf.count(i) + points[uf.find(j)].vx * uf.count(j)) / (uf.count(i) + uf.count(j));
    const double my =
        (points[uf.find(i)].vy * uf.count(i) + points[uf.find(j)].vy * uf.count(j)) / (uf.count(i) + uf.count(j));

    uf.unite(i, j);
    for (int k : all_indices) {
        if (uf.same(k, i)) {
            const int td = tick - last_ticks.at(k);
            if (td > 0) {
                points[k] = points[k].After(td);
                last_ticks[k] = tick;
            }
            points[k].vx = mx;
            points[k].vy = my;
        }
    }
}

void MergeSpeeds(int w1, int w2, Point &p1, Point &p2) {
    double mx = (p1.vx * w1 + p2.vx * w2) / (w1 + w2);
    double my = (p1.vy * w1 + p2.vy * w2) / (w1 + w2);
    p1.vx = mx;
    p1.vy = my;
    p2.vx = mx;
    p2.vy = my;
}

// pair<lint, Ops> OptimizeOpsTimingsBeam(const vector<Point> &initial_points_, const Ops &original_ops_) {
//     vector<int> unique_ids;
//     for (auto [t, i, j] : original_ops_) unique_ids.push_back(i), unique_ids.push_back(j);
//     unique_ids = sort_unique(unique_ids);

//     vector<Point> points;
//     for (int i : unique_ids) points.push_back(initial_points_.at(i));


// }

pair<lint, vector<tuple<double, int, int>>> EvaluateCost(const vector<Point> &initial_points, const Ops &ops, const vector<int> &all_indices) {
    lint total_cost = 0;
    vector<Point> points = initial_points;

    static vector<int> last_ticks;
    if (last_ticks.empty()) last_ticks.resize(N, 0);
    static UnionFind uf;
    if (uf.par.size() != N) uf = UnionFind(N);

    for (int i : all_indices) {
        last_ticks[i] = 0;
        uf.par[i] = i;
        uf.cou[i] = 1;
    }

    vector<tuple<double, int, int>> edges;

    for (auto [t, i, j] : ops) {
        for (int idx : {i, j}) {
            const int dt = t - last_ticks.at(idx);
            if (dt > 0) {
                points.at(idx) = points.at(idx).After(dt);
                last_ticks.at(idx) = t;
            }
        }
        const double dist = Distance(points.at(i), points.at(j));
        total_cost += round(dist);
        MergeSpeeds(i, j, t, last_ticks, uf, points, all_indices);
        edges.emplace_back(dist, i, j);
    }
    return {total_cost, edges};
}


pair<lint, Ops> OptimizeOpsTimings(const vector<Point> &initial_points, const Ops &ops) {
    Ops ret;

    static vector<int> last_ticks;
    if (last_ticks.empty()) last_ticks.resize(N, 0);
    static UnionFind uf;
    if (uf.par.size() != N) uf = UnionFind(N);

    auto points = initial_points;

    vector<int> all_indices;
    for (auto [t, i, j] : ops) {
        all_indices.push_back(i);
        if (j >= 0) all_indices.push_back(j);
    }
    all_indices = sort_unique(all_indices);

    for (int i : all_indices) {
        last_ticks[i] = 0;
        uf.par[i] = i;
        uf.cou[i] = 1;
    }

    lint ret_cost = 0.0;

    for (auto [original_t, i_, j_] : ops) {
        if (j_ >= 0 and uf.same(i_, j_)) continue;

        vector<int> is, js;
        for (int k : all_indices) {
            if (uf.same(i_, k)) is.push_back(k);
            if (j_ >= 0) {
                if (uf.same(j_, k)) js.push_back(k);
            } else {
                if (!uf.same(i_, k)) js.push_back(k);
            }
        }
        double best_cost = 1e18;
        int tlo = 0;
        int best_i = -1, best_j = -1;
        int best_t = tlo;
        for (int i : is) chmax(tlo, last_ticks.at(i));
        for (int j : js) chmax(tlo, last_ticks.at(j));

        for (int i : is) {
            if (last_ticks.at(i) < tlo) points[i] = points[i].After(tlo - last_ticks.at(i)), last_ticks[i] = tlo;
        }

        for (int j : js) {
            if (last_ticks.at(j) < tlo) points[j] = points[j].After(tlo - last_ticks.at(j)), last_ticks[j] = tlo;
        }

        for (int i : is) for (int j : js) {
            const int tt =
                find_closest_approach_time(PointState{(double)tlo, points[i].x, points[i].y, points[i].vx, points[i].vy},
                                           PointState{(double)tlo, points[j].x, points[j].y, points[j].vx, points[j].vy}, tlo,
                                           max(tlo + 1, min(T, original_t + T / 20)), L);
            double new_cost = Distance(points[i].After(tt - tlo), points[j].After(tt - tlo));
            if (chmin(best_cost, new_cost)) {
                best_i = i;
                best_j = j;
                best_t = tt;
            }
        }

        MergeSpeeds(best_i, best_j, best_t, last_ticks, uf, points, all_indices);
        ret.emplace_back(best_t, best_i, best_j);
        ret_cost += round(best_cost);
    }

    return {ret_cost, ret};
}


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

pair<lint, Ops> ImproveOpsTimings(const vector<Point> &initial_points, Ops original_ops) {
    vector<int> all_indices;
    for (auto [t, i, j] : original_ops) all_indices.push_back(i), all_indices.push_back(j);
    all_indices = sort_unique(all_indices);

    lint original_cost = EvaluateCost(initial_points, original_ops, all_indices).first;

    {
        auto [new_cost, new_ops] = OptimizeOpsTimings(initial_points, original_ops);
        if (new_cost <= original_cost) {
            original_cost = new_cost;
            original_ops = new_ops;
        }
    }

    if (original_ops.size() < 2) return {original_cost, original_ops};

    REP(_, 200) {
        auto op2 = original_ops;

        REP(_, 1) {
            int i = 0, j = 0;
            while (i == j) {
                i = rand_int() % ((int)original_ops.size() + 1);
                j = rand_int() % ((int)original_ops.size() + 1);
            }
            if (i > j) swap(i, j);

            if (rand_int() % 2) {
                rotate(op2.begin() + i, op2.begin() + i + 1, op2.begin() + j);
            } else {
                rotate(op2.begin() + i, op2.begin() + j - 1, op2.begin() + j);
            }
        }

        auto [new_cost, new_ops] = OptimizeOpsTimings(initial_points, op2);
        if (new_cost <= original_cost) {
            original_cost = new_cost;
            original_ops = new_ops;
        }
    }
    return {original_cost, original_ops};
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

Ops TrySwap(const vector<Point> &initial_points, const Ops &initial_ops) {
    vector<Ops> group_ops(N);
    vector<vector<int>> group_ids(N);
    UnionFind uf(N);
    vector<int> deg(N);
    for (auto [t, i, j] : initial_ops) { uf.unite(i, j); deg.at(i)++; deg.at(j)++; }
    REP(i, N) { group_ids[uf.find(i)].push_back(i); }

    for (auto [t, i, j] : initial_ops) { group_ops[uf.find(i)].emplace_back(t, i, j); }

    vector<int> all_indices;
    REP(i, N) all_indices.push_back(i);

    {
        auto [cost, edges] = EvaluateCost(initial_points, initial_ops, all_indices);

        vector<double> max_weight(N);
        for (auto [w, i, j] : edges) {
            chmax(max_weight.at(i), w);
            chmax(max_weight.at(j), w);
        }

        vector<pair<double, int>> heavy_leaves;
        REP(i, N) {
            if (deg.at(i) == 1) {
                heavy_leaves.emplace_back(max_weight.at(i), i);
            }
        }
        sort(heavy_leaves.rbegin(), heavy_leaves.rend());
        // dbg(heavy_leaves);
        const int K = min<int>(heavy_leaves.size(), 10);
        REP(k, K) REP(l, k) {
            const int i = heavy_leaves.at(k).second;
            const int j = heavy_leaves.at(l).second;
            if (uf.same(i, j)) continue;

            const int gi = uf.find(i);
            const int gj = uf.find(j);

            const auto cost_i = EvaluateCost(initial_points, group_ops.at(gi), group_ids.at(gi)).first;
            const auto cost_j = EvaluateCost(initial_points, group_ops.at(gj), group_ids.at(gj)).first;

            auto swapped_gis = group_ids.at(gi);
            auto swapped_gjs = group_ids.at(gj);
            swapped_gis.erase(std::remove(swapped_gis.begin(), swapped_gis.end(), i), swapped_gis.end());
            swapped_gis.push_back(j);
            swapped_gjs.erase(std::remove(swapped_gjs.begin(), swapped_gjs.end(), j), swapped_gjs.end());
            swapped_gjs.push_back(i);
            sort(ALL(swapped_gis));
            sort(ALL(swapped_gjs));

            auto swapped_ops_i = group_ops.at(gi);
            {
                auto it = std::remove_if(swapped_ops_i.begin(), swapped_ops_i.end(),
                                    [&](const auto &triplet) {
                                        const auto &[t, x, y] = triplet;
                                        return x == i or y == i;
                                    });
                swapped_ops_i.erase(it, swapped_ops_i.end());
            }
            auto swapped_ops_j = group_ops.at(gj);
            {
                auto it = std::remove_if(swapped_ops_j.begin(), swapped_ops_j.end(),
                                    [&](const auto &triplet) {
                                        const auto &[t, x, y] = triplet;
                                        return x == j or y == j;
                                    });
                swapped_ops_j.erase(it, swapped_ops_j.end());
            }

            auto FindBestPos = [&](int idx, const Ops ops) -> pair<Ops, lint> {
                lint best_cost = 1e18;
                Ops best_ops;
                for (int pos = 0; pos <= (int)ops.size(); ++pos) {
                    auto new_ops = ops;
                    new_ops.insert(new_ops.begin() + pos, make_tuple(0, idx, -1));
                    auto [new_cost, finalized_ops] = OptimizeOpsTimings(initial_points, new_ops);
                    if (chmin(best_cost, new_cost)) best_ops = finalized_ops;
                }
                return {best_ops, best_cost};
            };

            auto [new_ops_i, new_cost_i] = FindBestPos(j, swapped_ops_i);
            auto [new_ops_j, new_cost_j] = FindBestPos(i, swapped_ops_j);

            if (new_cost_i + new_cost_j < cost_i + cost_j) {
                tie(new_cost_i, new_ops_i) = ImproveOpsTimings(initial_points, new_ops_i);
                tie(new_cost_j, new_ops_j) = ImproveOpsTimings(initial_points, new_ops_j);

                Ops ret;
                ret.insert(ret.end(), ALL(new_ops_i));
                ret.insert(ret.end(), ALL(new_ops_j));
                REP(r, N) {
                    if (r == gi or r == gj) continue;
                    ret.insert(ret.end(), ALL(group_ops.at(r)));
                }
                return ret;
            }
        }
    }

    return initial_ops;
}

int main(int argc, char *argv[]) {
    {
        int N_, T_, M_, K_, L_;
        cin >> N_ >> T_ >> M_ >> K_ >> L_;
        dbg(make_tuple(N_, T_, M_, K_, L_));
        assert(N_ == N);
        assert(T_ == T);
        assert(M_ == M);
        assert(K_ == K);
        assert(L_ == L);
    }

    vector<Point> initial_points(N);
    for (auto &p : initial_points) cin >> p.x >> p.y >> p.vx >> p.vy;
    dbg(initial_points);

    UnionFind uf(N);

    auto points = initial_points;

    vector<tuple<int, int, int>> answer;

    bool core_found = false;


    vector<vector<pint>> t2plans(T);
    REP(t, T) {
        const double threshold_0 = 2000.0;
        const double dist_threshold = threshold_0 + (80000.0 - threshold_0) * pow(min(1.0, t / (T - 2.0)), 4);
        vector<tuple<double, int, int>> edges_candidates;

        if (core_found) {
            edges_candidates.clear();
            for (auto [i, j] : t2plans.at(t)) {
                const auto d = Distance(points[i], points[j]);
                edges_candidates.emplace_back(d, i, j);
            }
        } else {

            REP(i, N) REP(j, i) {
                if (uf.same(i, j)) continue;
                if (uf.count(i) + uf.count(j) > K) continue;
                const auto d = Distance(points[i], points[j]);
                const auto loss = EnergyLossRatio(uf.count(i), uf.count(j), points[i], points[j]);
                if (!core_found and loss < 0.1) continue;
                if (d <= dist_threshold /* and loss < 0.5 */) { edges_candidates.emplace_back(d, i, j); }
            }
        }

        sort(ALL(edges_candidates));

        for (auto [_, i, j] : edges_candidates) {
            if (uf.same(i, j)) continue;
            if (uf.count(i) + uf.count(j) > K) continue;

            vector<int> iis, jjs;

            REP(k, N) {
                if (uf.same(i, k)) iis.push_back(k);
                if (uf.same(j, k)) jjs.push_back(k);
            }
            double best_distance = 1e18;
            for (int ii : iis)
                for (int jj : jjs) {
                    const auto d = Distance(points[ii], points[jj]);
                    if (chmin(best_distance, d)) {
                        i = ii, j = jj;
                }
            }

            answer.emplace_back(t, i, j);

            double mx = points[uf.find(i)].vx * uf.count(i) + points[uf.find(j)].vx * uf.count(j);
            double my = points[uf.find(i)].vy * uf.count(i) + points[uf.find(j)].vy * uf.count(j);

            mx /= (uf.count(i) + uf.count(j));
            my /= (uf.count(i) + uf.count(j));

            uf.unite(i, j);

            points[uf.find(i)].vx = mx;
            points[uf.find(i)].vy = my;
            REP(k, N) {
                points[k].vx = points[uf.find(k)].vx;
                points[k].vy = points[uf.find(k)].vy;
            }
        }

        vector<int> sizes;
        for (auto g : uf.groups()) sizes.push_back((int)g.size());

        const int core_threshold = K * 0.5;
        if (!core_found) {
            const int num_cores = std::count_if(ALL(sizes), [](int sz) { return sz > core_threshold; });
            if (num_cores >= M) {
                dbg(make_tuple(t, num_cores));
                core_found = true;
                vector<int> is_not_core(N, 1);
                int rem_core = M;
                UnionFind uf_new(N);

                vector<vector<int>> cores;

                vector<vector<int>> gs = uf.groups();
                sort(ALL(gs), [](const auto &a, const auto &b) { return a.size() > b.size(); });

                for (auto g : gs) {
                    if (rem_core <= 0) break;
                    if ((int)g.size() > core_threshold) {
                        for (int i : g) {
                            is_not_core[i] = 0;
                            uf_new.unite(g.front(), i);
                        }
                        rem_core--;
                        cores.push_back(g);
                    }
                }
                uf = uf_new;

                assert(rem_core == 0);

                REP(i, N) {
                    if (is_not_core.at(i)) {
                        points[i] = initial_points[i].After(t);
                    }
                }

                auto it = std::remove_if(ALL(answer), [&](const auto &triplet) {
                    const auto &[time, i, j] = triplet;
                    return is_not_core[i] or is_not_core[j];
                });
                answer.erase(it, answer.end());

                vector<vector<int>> cost_matrix;
                vector<vector<int>> time_matrix;
                REP(i, N) {
                    if (is_not_core.at(i)) {
                        vector<int> cost_row;
                        vector<int> time_row;
                        for (const auto &c : cores) {
                            const int capacity = K - (int)c.size();
                            lint min_cost = 1e18;
                            int min_t = 0;
                            for (int j : c) {
                                const int tt = find_closest_approach_time(
                                    PointState{(double)t, points[i].x, points[i].y, points[i].vx, points[i].vy},
                                    PointState{(double)t, points[j].x, points[j].y, points[j].vx, points[j].vy}, t + 1, T,
                                    L);
                                const lint d = round(Distance(points[i].After(tt - t), points[j].After(tt - t)));
                                if (chmin(min_cost, d)) {
                                    min_t = tt;
                                }
                            }
                            REP(_, capacity) {
                                cost_row.push_back(min_cost);
                                time_row.push_back(min_t);
                            }
                        }
                        cost_matrix.push_back(cost_row);
                        time_matrix.push_back(time_row);
                    }
                }
                assert((int)cost_matrix.size() == (int)cost_matrix.front().size());

                auto res =
                    linear_sum_assignment::solve((int)cost_matrix.size(), (int)cost_matrix[0].size(), cost_matrix);

                int row = 0;
                REP(i, N) {
                    if (is_not_core.at(i)) {
                        int col = res.mate.at(row);
                        assert(col >= 0);

                        for (const auto &c : cores) {
                            const int capacity = K - (int)c.size();
                            if (col < capacity) {
                                const int j = c.front();
                                const int tt = time_matrix.at(row).at(res.mate.at(row));
                                assert(tt > t);
                                t2plans.at(tt).emplace_back(i, j);
                                col = -1;
                                break;
                            } else {
                                col -= capacity;
                            }
                        }
                        assert(col < 0);
                        ++row;
                    }
                }
                assert(row == (int)cost_matrix.size());
                // dbg(t2plans);
            }
        }

        REP(i, N) points[i] = points[i].After(1);
    }
    dbg(timer.spent_ms());

    vector<int> all_indices(N);
    iota(ALL(all_indices), 0);
    double cost = EvaluateCost(initial_points, answer, all_indices).first;
    dbg(cost);
    tie(cost, answer) = OptimizeOpsTimings(initial_points, answer);
    dbg(cost);

    vector<Ops> group_ops(N);
    vector<vector<int>> group_ids(N);
    REP(i, N) { group_ids[uf.find(i)].push_back(i); }
    for (auto [t, i, j] : answer) { group_ops[uf.find(i)].emplace_back(t, i, j); }

    answer.clear();
    REP(r, N) {
        if (group_ids.at(r).empty()) continue;
        assert((int)group_ids.at(r).size() == K);
        // const auto original_cost = EvaluateCost(initial_points, group_ops.at(r), group_ids.at(r));
        auto [new_cost, new_ops] = ImproveOpsTimings(initial_points, group_ops.at(r));
        dbg(make_tuple(r, new_cost, (int)group_ops.at(r).size()));
        answer.insert(answer.end(), ALL(new_ops));
    }

    dbg(timer.spent_ms());

    cost = EvaluateCost(initial_points, answer, all_indices).first;
    dbg(cost);


    REP(_, 10) {
        if (timer.spent_ms() > 1600) break;
        answer = TrySwap(initial_points, answer);
        cost = EvaluateCost(initial_points, answer, all_indices).first;
        dbg(cost);
        dbg(timer.spent_ms());
        cerr << timer.spent_ms() << " ms" << endl;
    }

    const double score = round(1e6 * log2(L * (N - M) / (cost + 1)));
    dbg(score);

    dbg(answer.size());
    {
        vector<int> sizes;
        for (auto g : uf.groups()) sizes.push_back((int)g.size());
        sort(ALL(sizes));
        dbg(sizes);
    }

    assert((int)answer.size() == N - M);

    dump_onlinejudge(ToAnswer(answer));
    jdump("cost", cost);
    jdump("score", score);
}
