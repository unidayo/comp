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

#ifndef BAYES_HPP
#define BAYES_HPP

#include <array>
#include <cassert>
#include <cmath>
#include <vector>

#ifndef PARAMS_HPP
#define PARAMS_HPP

constexpr int MAP_LB = 0;
constexpr int MAP_UB = 1000;

constexpr int NLLMultiply = 10000;

constexpr int MAX_MEASUREMENT = 10000;

#endif

namespace {

double normalCDF(double x) // Phi(-∞, x) aka N(x)
{
    return std::erfc(-x / std::sqrt(2)) / 2;
}

} // namespace

double log_fac(int n) {
    double ret = 0.0;
    for (int i = 1; i <= n; ++i) ret += std::log(i);
    return ret;
}

class NormalDistirubionInfo {
    static constexpr int max_diff = 1000;

    int sigma_; // 標準偏差

public:
    using Float = double;
    std::array<Float, max_diff * 2 + 1> cdf_table_;

public:
    NormalDistirubionInfo(int sigma) : sigma_(sigma) {
        assert(1 <= sigma and sigma <= 900);

        for (int idiff = -max_diff; idiff <= max_diff; ++idiff) {
            Float r = (idiff + 0.5) / sigma_;
            cdf_table_.at(idiff + max_diff) = normalCDF(r);
        }
    }

    double get(int observed, int assumed) const {
        assert(MAP_LB <= observed and observed <= MAP_UB);
        assert(MAP_LB <= assumed and assumed <= MAP_UB);

        const int diff = observed - assumed;

        double r = (observed == MAP_UB) ? 1.0 : cdf_table_.at(diff + max_diff);
        double l = (observed == MAP_LB) ? 0.0 : cdf_table_.at(diff - 1 + max_diff);

        return r - l;
    }
};

class PrecalculatedNLL {
    static constexpr int NLLUB = 30;
    std::array<int, MAP_UB - MAP_LB + 1> table_; // 確率の negative log の NLLMultiply 倍 の round

public:
    PrecalculatedNLL() = default;
    PrecalculatedNLL(int map_val, const NormalDistirubionInfo &ndi) {
        for (int i = MAP_LB; i <= MAP_UB; ++i) {
            const double p = ndi.get(i, map_val);
            table_.at(i - MAP_LB) = (p > 0 ? -std::log(p) : NLLUB) * NLLMultiply;
        }

        for (auto &x : table_) {
            if (x > NLLUB * NLLMultiply) x = NLLUB * NLLMultiply;
            assert(0 <= x and x <= NLLUB * NLLMultiply);
        }
    }

    int nllint(int observed) const {
        assert(MAP_LB <= observed and observed <= MAP_UB);
        return table_.at(observed - MAP_LB);
    }
};

class PrecalculatedNLLList {
    std::array<PrecalculatedNLL, MAP_UB - MAP_LB + 1> list_;

public:
    PrecalculatedNLLList(const NormalDistirubionInfo &ndi) {
        for (int i = MAP_LB; i <= MAP_UB; ++i) list_.at(i - MAP_LB) = PrecalculatedNLL(i, ndi);
    }

    int nllint(int observed, int map_val) const {
        assert(MAP_LB <= map_val and map_val <= MAP_UB);
        assert(MAP_LB <= observed and observed <= MAP_UB);
        return list_.at(map_val - MAP_LB).nllint(observed);
    }
};

#endif
#ifndef IO_PROXY_HPP
#define IO_PROXY_HPP

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>


#ifndef WORLDMAP_HPP
#define WORLDMAP_HPP

#include <algorithm>
#include <cassert>
#include <utility>
#include <string>
#include <vector>

#ifndef GAUSS_SEIDEL_HPP
#define GAUSS_SEIDEL_HPP

#include <vector>

int gauss_seidel(std::vector<std::vector<int>> &data, const std::vector<std::vector<int>> &is_fixed) {

    const int L = data.size();
    int iter = 0;

    while (true) {
        ++iter;
        bool updated = false;
        for (int i = 0; i < L; ++i) {
            for (int j = 0; j < L; ++j) {
                if (is_fixed.at(i).at(j)) continue;
                int sum = data.at((i + 1) % L).at(j) + data.at((i + L - 1) % L).at(j) + data.at(i).at((j + 1) % L) +
                          data.at(i).at((j + L - 1) % L);
                int upd = (sum + 2) / 4;
                if (data.at(i).at(j) != upd) {
                    updated = true;
                    data.at(i).at(j) = upd;
                }
            }
        }
        if (!updated) break;
    }

    return iter;
}

int gauss_seidel_vec(int L, std::vector<int> &data, const std::vector<int> &is_fixed) {
    int iter = 0;
    while (true) {
        ++iter;
        bool updated = false;
        for (int i = 0; i < L; ++i) {
            for (int j = 0; j < L; ++j) {
                if (is_fixed.at(i * L + j)) continue;
                int sum = data.at(((i + 1) % L) * L + j) + data.at(((i + L - 1) % L) * L + j) +
                          data.at(i * L + (j + 1) % L) + data.at(i * L + (j + L - 1) % L);
                int upd = (sum + 2) / 4;
                if (data.at(i * L + j) != upd) {
                    updated = true;
                    data.at(i * L + j) = upd;
                }
            }
        }
        if (!updated) break;
    }
    return iter;
}

#endif

#include <algorithm>
#include <vector>

#ifdef HITONANODE_LOCAL

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
        int j = rand_int() % (i + 1);
        std::swap(vec.at(i), vec.at(j));
    }
}
int rnd(int x) { return rand_int() % x; } // [0, x)
int rnd(int l, int r) { return l + rnd(r - l); }

#else

struct rand_int_ {
    std::mt19937 mt;
    rand_int_() : mt(std::chrono::steady_clock::now().time_since_epoch().count()) {}
    int operator()(int x) { return this->operator()(0, x); } // [0, x)
    int operator()(int l, int r) {
        std::uniform_int_distribution<int> d(l, r - 1);
        return d(mt);
    }
} rnd;

template <class T> void shuffle_vec(std::vector<T> &vec) { std::shuffle(vec.begin(), vec.end(), rnd.mt); }

#endif

class WorldMap {
    int L = 0;
    std::vector<int> data_;

public:
    WorldMap() = default;

    int getL() const noexcept { return L; }

    int max_grad() const {
        int ret = 0;
        for (int y = 0; y < L; ++y) {
            for (int x = 0; x < L; ++x) {
                int nx = (x + 1) % L;
                int ny = (y + 1) % L;
                ret = std::max(ret, abs(at(y, x) - at(y, nx)));
                ret = std::max(ret, abs(at(y, x) - at(ny, x)));
            }
        }
        return ret;
    }

    WorldMap(const std::vector<int> &data, int L) : L(L), data_(data) {}

    WorldMap(const std::vector<std::vector<int>> &data) : L(data.size()) {
        data_.reserve(L * L);

        for (int y = 0; y < L; ++y) {
            assert((int)data.at(y).size() == L);
            for (int x = 0; x < L; ++x) { data_.push_back(data.at(y).at(x)); }
        }
    }

    int at(int y, int x) const { return data_.at(y * L + x); }

    std::string to_string() const {
        assert(1 <= L);

        std::string ret;
        for (int y = 0; y < L; ++y) {
            for (int x = 0; x < L; ++x) ret += std::to_string(at(y, x)) + ' ';
            ret.back() = '\n';
        }

        ret.pop_back();
        return ret;
    }

    std::vector<std::vector<int>> to_mat() const {
        std::vector<std::vector<int>> ret(L, std::vector<int>(L, 0));
        for (int y = 0; y < L; ++y) {
            for (int x = 0; x < L; ++x) ret.at(y).at(x) = at(y, x);
        }
        return ret;
    }

    long long cost() const {
        long long ret = 0;
        for (int y = 0; y < L; ++y) {
            for (int x = 0; x < L; ++x) {
                {
                    int nx = (x + 1) % L;
                    int diff = at(y, x) - at(y, nx);
                    ret += diff * diff;
                }
                {
                    int ny = (y + 1) % L;
                    int diff = at(y, x) - at(ny, x);
                    ret += diff * diff;
                }
            }
        }
        return ret;
    }
};

int dist1d(int L, int y1, int y2) { return std::min(abs(y1 - y2), L - abs(y1 - y2)); };
int dist2d(int L, int y1, int x1, int y2, int x2) { return dist1d(L, y1, y2) + dist1d(L, x1, x2); };

std::pair<int, int> find_centroid(int L, const std::vector<std::pair<int, int>> &yxs) {
    int best_cy = -1, best_cx = -1;
    int best_dist_sum = 1 << 29;

    for (int cy = 0; cy < L; ++cy) {
        for (int cx = 0; cx < L; ++cx) {
            int dist_sum = 0;
            for (auto [y, x] : yxs) dist_sum += dist2d(L, y, x, cy, cx);
            if (dist_sum < best_dist_sum) {
                best_dist_sum = dist_sum;
                best_cy = cy;
                best_cx = cx;
            }
        }
    }
    return {best_cy, best_cx};
}

WorldMap generate_distinct_map(int L, int ub, const std::vector<std::pair<int, int>> &yxs) {
    const int n = yxs.size();

    ub = std::min(1000, ub);
    std::vector<std::vector<int>> data(L, std::vector<int>(L)), is_fixed(L, std::vector<int>(L, 0));

    const int c = L / 2;

    std::vector<int> ids(n);
    for (int i = 0; i < n; ++i) ids.at(i) = i;

    const int cy = c, cx = c;

    std::stable_sort(ids.begin(), ids.end(), [&](int i, int j) -> bool {
        const int di = dist2d(L, yxs.at(i).first, yxs.at(i).second, cy, cx);
        const int dj = dist2d(L, yxs.at(j).first, yxs.at(j).second, cy, cx);
        return di < dj;
    });

    for (int d = 0; d < n; ++d) {
        auto [y, x] = yxs.at(ids.at(d));
        data.at(y).at(x) = ub * d / (n - 1);
        is_fixed.at(y).at(x) = 1;
    }

    gauss_seidel(data, is_fixed);
    long long best_score = WorldMap(data).cost();
    std::vector<std::vector<int>> best_mat = data;

    for (int t = 0; t < 1000; ++t) {
        assert(n > 10);
        int a = rnd(n - 10);
        int b = a + 1 + rnd(10);

        int i = ids.at(a), j = ids.at(b);
        auto [yi, xi] = yxs.at(i);
        auto [yj, xj] = yxs.at(j);
        data = best_mat;

        std::swap(data.at(yi).at(xi), data.at(yj).at(xj));
        gauss_seidel(data, is_fixed);
        auto score = WorldMap(data).cost();
        if (score < best_score) {
            best_score = score;
            best_mat = data;
            std::swap(ids.at(a), ids.at(b));
        }
    }

    return WorldMap(best_mat);
}

WorldMap generate_distinct_with_cliff(int L, int ub, const std::vector<std::pair<int, int>> &yxs, int sigma) {
    std::vector<std::vector<int>> d = generate_distinct_map(L, ub, yxs).to_mat();
    std::vector is_fixed(L, std::vector<int>(L));
    for (auto [y, x] : yxs) is_fixed.at(y).at(x) = 1;
    d.at(0).at(0) = 0;
    is_fixed.at(0).at(0) = 1;

    gauss_seidel(d, is_fixed);
    if (WorldMap(d).max_grad() < sigma * 2) {
        d.at(0).at(1) = std::min(1000, sigma * 2);
        d.at(1).at(0) = std::min(1000, sigma * 2);
        is_fixed.at(0).at(1) = 1;
        is_fixed.at(1).at(0) = 1;
        gauss_seidel(d, is_fixed);
    }
    return WorldMap(d);
}

WorldMap generate_geo_cliff(int L, int ub, const std::vector<std::pair<int, int>> &yxs, int cxcy = -1) {
    if (cxcy == -1) cxcy = L / 4;

    auto [best_cy, best_cx] = find_centroid(L, yxs);

    const int cy = cxcy, cx = cxcy;

    const int originy = (best_cy - cy + L) % L, originx = (best_cx - cx + L) % L;

    std::vector<std::vector<int>> d(L, std::vector<int>(L));
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            int dist = std::hypot(i - L / 2, j - L / 2);
            d.at((originy + i) % L).at((originx + j) % L) =
                std::min<int>(1000, ub * std::min(1.0, 1.0 / L * sqrt(2) * dist));
        }
    }

    std::vector is_fixed(L, std::vector<int>(L));
    for (auto [y, x] : yxs) is_fixed.at(y).at(x) = 1;

    d.at((cy + originy) % L).at((cx + originx) % L) = 0;
    is_fixed.at((cy + originy) % L).at((cx + originx) % L) = 1;

    gauss_seidel(d, is_fixed);

    if (ub <= 1000) {
        int bias = (1000 - ub) / 2;
        for (auto &vec : d) {
            for (int &x : vec) x += bias;
        }
    }

    return WorldMap(d);
}

std::vector<std::vector<int>> autoscale(const std::vector<std::vector<double>> &data, int ub) {
    const int L = data.size();
    double lo = data.at(0).at(0), hi = lo;
    for (auto &vec : data) {
        for (double x : vec) {
            lo = std::min(lo, x);
            hi = std::max(hi, x);
        }
    }
    dbg(std::make_tuple(lo, hi));

    std::vector ret(L, std::vector<int>(L));
    for (int y = 0; y < L; ++y) {
        for (int x = 0; x < L; ++x) { ret.at(y).at(x) = 1.0 * ub * (data.at(y).at(x) - lo) / (hi - lo); }
    }

    return ret;
}

WorldMap generate_log_cliff(int L, int ub, const std::vector<std::pair<int, int>> &yxs, int cxcy) {

    auto [best_cy, best_cx] = find_centroid(L, yxs);

    const int cy = cxcy, cx = cxcy;

    const int originy = (best_cy - cy + L) % L, originx = (best_cx - cx + L) % L;

    std::vector ddouble(L, std::vector<double>(L));
    std::vector is_fixed(L, std::vector<int>(L));
    const double r0 = L * 0.08;
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            double dist = std::hypot(i - L / 2, j - L / 2) + 1e-5;
            auto f = [&](double d) -> double { return std::log(d); };
            ddouble.at((originy + i) % L).at((originx + j) % L) = std::max(f(r0), f(dist));
        }
    }

    auto d = autoscale(ddouble, ub);
    for (auto &vec : d) {
        for (int &x : vec) x = std::min(x, 1000);
    }

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            if (dist2d(L, i, j, cy + originy, cx + originx) >= 3) is_fixed.at(i).at(j) = 1;
        }
    }

    d.at((cy + originy) % L).at((cx + originx) % L) = 0;
    is_fixed.at((cy + originy) % L).at((cx + originx) % L) = 1;

    gauss_seidel(d, is_fixed);

    if (ub <= 1000) {
        int bias = (1000 - ub) / 2;
        for (auto &vec : d) {
            for (int &x : vec) x += bias;
        }
    }

    return WorldMap(d);
}

WorldMap generate_log_cliff_parametrized(int L, const std::vector<std::pair<int, int>> &yxs, double r0, double r1) {

    const int ub = 1000;

    const double r2 = 2.0;

    auto [best_cy, best_cx] = find_centroid(L, yxs);

    const int cxcy = 0;
    const int cy = cxcy, cx = cxcy;

    const int originy = (best_cy - cy + L) % L, originx = (best_cx - cx + L) % L;

    std::vector d(L, std::vector<int>(L));
    std::vector is_fixed(L, std::vector<int>(L));

    const double f0 = std::log(r0 * L);
    const double f1 = std::log(r1 * L);

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            double dist = std::hypot(i - L / 2, j - L / 2);
            if (dist <= r0 * L) {
                d.at((originy + i) % L).at((originx + j) % L) = 0;
                is_fixed.at((originy + i) % L).at((originx + j) % L) = 1;
            } else if (r0 * L < dist and dist < r1 * L) {
                d.at((originy + i) % L).at((originx + j) % L) = round(ub * (std::log(dist) - f0) / (f1 - f0));
            } else if (dist >= r1 * L) {
                d.at((originy + i) % L).at((originx + j) % L) = ub;
                if (dist <= r1 * L + 1.45) is_fixed.at((originy + i) % L).at((originx + j) % L) = 1;
            }

            if (auto d = dist2d(L, i, j, best_cy, best_cx); r2 <= d and d < r2 + 1.45) is_fixed.at(i).at(j) = 1;
        }
    }

    d.at(best_cy).at(best_cx) = 0;
    is_fixed.at(best_cy).at(best_cx) = 1;

    gauss_seidel(d, is_fixed);
    return WorldMap(d);
}

#endif

class IOProxy {
    int N = 0;
    std::vector<std::pair<int, int>> yxs;

    int L = 0;

    std::vector<std::vector<int>> P;
    int map_cost_ = 0;

    std::vector<int> ground_truth_A;

    std::vector<int> random_biases;

    int num_query_ = 0;
    int query_cost_ = 0;

    int num_wrong_estimation_ = 0;

    void calc_map_cost() {
        map_cost_ = 0;
        for (int y = 0; y < L; ++y) {
            for (int x = 0; x < L; ++x) {
                {
                    int ny = (y + 1) % L;
                    int d = P.at(y).at(x) - P.at(ny).at(x);
                    map_cost_ += d * d;
                }
                {
                    int nx = (x + 1) % L;
                    int d = P.at(y).at(x) - P.at(y).at(nx);
                    map_cost_ += d * d;
                }
            }
        }
    }

public:
    IOProxy() = default;

    void set_yxs(const std::vector<std::pair<int, int>> &yxs_) {
        yxs = yxs_;
        N = yxs_.size();
    }

    void set_P(const std::vector<std::vector<int>> &P_) {
        P = P_;
        L = P.size();
        calc_map_cost();
    }

    void dump_P() const {
        assert(!P.empty());
        dump_onlinejudge(WorldMap(P).to_string());
    }

    void set_ground_truth(const std::vector<int> &A, const std::vector<int> &random_biases_) {
        assert((int)random_biases_.size() == MAX_MEASUREMENT);
        ground_truth_A = A;
        random_biases = random_biases_;
    }

    int query(int i, int y, int x) {
        assert(0 <= i and i < N);
        assert(-L < y and y < L);
        assert(-L < x and x < L);

        query_cost_ += 100 * (10 + abs(y) + abs(x));

        if (ground_truth_A.empty() or random_biases.empty()) {
            std::string s;
            s += std::to_string(i) + ' ';
            s += std::to_string(y) + ' ';
            s += std::to_string(x);
            dump_onlinejudge(s);

            int resp;
            std::cin >> resp;
            return resp;

        } else {
            int j = ground_truth_A.at(i);
            auto [y0, x0] = yxs.at(j);
            int retval = P.at((y0 + y + L) % L).at((x0 + x + L) % L) + random_biases.at(num_query_++);
            return std::clamp(retval, 0, 1000);
        }
    }

    void answer(const std::vector<int> &mate, bool dump) {
        assert((int)mate.size() == N);
        if (dump) {
            std::string s = "-1 -1 -1\n";
            for (int x : mate) {
                assert(0 <= x and x < N);
                s += std::to_string(x) + '\n';
            }
            assert(!s.empty());
            s.pop_back();
            dump_onlinejudge(s);
        }

        num_wrong_estimation_ = 0;
        if (!ground_truth_A.empty()) {
            for (int i = 0; i < N; ++i) {
                if (mate.at(i) != ground_truth_A.at(i)) ++num_wrong_estimation_;
            }
        }
    }

    int num_query() const noexcept { return num_query_; }

    int query_cost() const noexcept { return query_cost_; }

    int num_wrong_estimation() const noexcept { return num_wrong_estimation_; }

    int map_cost() const noexcept { return map_cost_; }

    int calc_score() const noexcept {
        return ceill(1e14l * powl(0.8, num_wrong_estimation()) / (1e5l + map_cost() + query_cost()));
    }
};

#endif

#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <cassert>
#include <numeric>
#include <utility>
#include <vector>

#ifndef ASSIGNMENT_MANAGER_HPP
#define ASSIGNMENT_MANAGER_HPP

#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>

#include <cassert>
#include <tuple>
#include <vector>

namespace linear_assignment_sum {

template <class T>
bool augment(int N, const std::vector<std::vector<T>> &C, std::vector<T> &f, std::vector<T> &g, int s, int tgoal,
             std::vector<int> &mate, std::vector<int> &mate_inv, T augment_limit, const std::vector<int> jlist) {

    assert(0 <= s and s < N);
    assert(mate.at(s) < 0);

    static std::vector<T> dist;
    static std::vector<int> prv;
    dist.resize(N);
    prv.resize(N);

    augment_limit += f.at(s);

    f.at(s) = C.at(s).at(jlist.front()) - g.at(jlist.front());
    for (int j : jlist) f.at(s) = std::min(f.at(s), C.at(s).at(j) - g.at(j));

    augment_limit -= f.at(s);

    for (int j : jlist) {
        dist.at(j) = C.at(s).at(j) - f.at(s) - g.at(j);
        prv.at(j) = s;
    }

    std::vector<bool> done(N);

    int t = -1;
    std::vector<int> stk;

    while (t == -1) {
        int j1 = -1;
        for (int j : jlist) {
            if (done.at(j)) continue;
            if (j1 == -1 or dist.at(j) < dist.at(j1) or (dist.at(j) == dist.at(j1) and mate_inv.at(j) < 0)) j1 = j;
        }

        if (j1 == -1) return false;

        if (dist.at(j1) >= augment_limit) return false;

        if (mate_inv.at(j1) < 0) {
            t = j1;
            break;
        }

        done.at(j1) = 1;
        stk = {j1};

        while (!stk.empty()) {
            const int i = mate_inv.at(stk.back());
            if (i < 0) {
                t = stk.back();
                break;
            }
            stk.pop_back();

            for (int j : jlist) {
                if (done.at(j)) continue;

                const T len = C.at(i).at(j) - f.at(i) - g.at(j);
                assert(len >= 0);

                if (dist.at(j) > dist.at(j1) + len) {
                    dist.at(j) = dist.at(j1) + len;
                    prv.at(j) = i;
                }

                if (len == T()) {
                    stk.push_back(j);
                    done.at(j) = 1;
                }
            }
        }
    }

    assert(t == tgoal);

    const T len = dist.at(t);

    f.at(s) += len;

    for (int j : jlist) {
        if (!done.at(j)) continue;

        g.at(j) -= len - dist.at(j);
        if (mate_inv.at(j) >= 0) {
            f.at(mate_inv.at(j)) += len - dist.at(j);
        } else {
            assert(j == t);
        }
    }

    for (int cur = t; cur >= 0;) {
        const int i = prv.at(cur);
        mate_inv.at(cur) = i;
        if (i == -1) break;
        std::swap(cur, mate.at(i));
    }

    return true;
}

} // namespace linear_assignment_sum

class AssignmentManager {
private:
    static constexpr long long INF = 1LL << 61;

    int N;
    std::vector<std::vector<long long>> nllsum;

    long long min_cost = INF;
    std::vector<int> mate_, mate_inv;
    std::vector<long long> f, g;

    long long second_min_cost = INF;
    std::vector<int> second_mate_;

    struct MateDiff {
        int s;
        int t1; // 現在の最適解でワームホール s と対応する出口
        int t2; // 準最適解でワームホール s と対応する出口
        template <class OStream> friend OStream &operator<<(OStream &os, const MateDiff &md) {
            return os << "MateDiff(" << md.s << ", " << md.t1 << ", " << md.t2 << ")";
        }
    };
    std::vector<MateDiff> second_mate_diffs_;
    std::vector<MateDiff> all_diffs_;

    bool is_second_min_updated_ = false;

public:
    AssignmentManager(int N_)
        : N(N_), nllsum(N, std::vector<long long>(N)), min_cost(0), mate_(N), mate_inv(N), f(N), g(N) {
        std::iota(mate_.begin(), mate_.end(), 0);
        std::iota(mate_inv.begin(), mate_inv.end(), 0);

        second_min_cost = 0;
        second_mate_ = mate_;
        std::rotate(second_mate_.begin(), second_mate_.begin() + 1, second_mate_.end());
        second_mate_diffs_.resize(N);
        for (int s = 0; s < N; ++s) second_mate_diffs_.at(s) = {s, s, (s + 1) % N};
        all_diffs_ = second_mate_diffs_;
    }

    const std::vector<std::vector<long long>> &get_nllsum() const noexcept { return nllsum; }

    bool is_second_min_updated() const noexcept { return is_second_min_updated_; }

    std::vector<std::pair<int, double>> get_mate_candidates(int s) const {
        assert(0 <= s and s < N);

        long long min_nllsum = 1LL << 60;
        for (int t = 0; t < N; ++t) {
            if (nllsum.at(s).at(t) < min_nllsum) min_nllsum = nllsum.at(s).at(t);
        }
        std::vector<std::pair<int, double>> ret;
        const double thres = std::log(1e3);
        double prob_sum = 0;
        for (int t = 0; t < N; ++t) {
            const double diff = 1.0 * (nllsum.at(s).at(t) - min_nllsum) / NLLMultiply;
            const double prob = std::exp(-diff);
            prob_sum += prob;
            if (diff < thres) ret.emplace_back(t, prob);
        }
        for (auto &p : ret) p.second /= prob_sum;

        std::sort(ret.begin(), ret.end(), [](const auto &p1, const auto &p2) { return p1.second > p2.second; });
        while (ret.size() and ret.back().second < 1e-3) ret.pop_back();
        return ret;
    }

    long long cost_gap() const noexcept {
        assert(is_second_min_updated());
        auto ret = second_min_cost - min_cost;
        assert(ret >= 0);
        return ret;
    }

    const std::vector<int> &mate() const noexcept { return mate_; }

    int mate(int s) const {
        assert(0 <= s and s < N);
        return mate_.at(s);
    }

    const std::vector<int> &second_mate() const noexcept { return second_mate_; }

    int second_mate(int s) const {
        assert(0 <= s and s < N);
        return second_mate_.at(s);
    }

    void add_row(int s, const std::vector<long long> &add) {
        assert(0 <= s and s < N);
        assert((int)add.size() == N);

        long long max_reduce = 1LL << 58;
        for (int j = 0; j < N; ++j) {
            long long tmp = nllsum.at(s).at(j) + add.at(j) - f.at(s) - g.at(j);
            if (tmp < max_reduce) max_reduce = tmp;
        }

        for (int j = 0; j < N; ++j) nllsum.at(s).at(j) += add.at(j) - max_reduce;

        const int t = mate_.at(s);
        assert(mate_inv.at(t) == s);
        mate_.at(s) = -1;
        mate_inv.at(t) = -1;

        std::vector<int> tlist(N);
        std::iota(tlist.begin(), tlist.end(), 0);
        shuffle_vec(tlist);

        linear_assignment_sum::augment(N, nllsum, f, g, s, t, mate_, mate_inv, 1LL << 60, tlist);

        min_cost = std::accumulate(f.cbegin(), f.cend(), 0LL) + std::accumulate(g.cbegin(), g.cend(), 0LL);

        is_second_min_updated_ = false;
    }

    void update_second_best() {
        if (is_second_min_updated()) return;

        static constexpr long long M = 1LL << 50;

        second_min_cost = INF;

        all_diffs_.clear();

        std::vector<int> tlist(N);
        std::iota(tlist.begin(), tlist.end(), 0);
        shuffle_vec(tlist);
        while (tlist.size() >= 2) {
            const int t = tlist.back();
            const int s = mate_inv.at(t);
            assert(mate_inv.at(t) == s);

            auto mate2 = mate_;
            auto mate_inv2 = mate_inv;
            auto f2 = f, g2 = g;
            mate2.at(s) = -1;
            mate_inv2.at(t) = -1;
            nllsum.at(s).at(t) += M;

            const bool found = linear_assignment_sum::augment(
                N, nllsum, f2, g2, s, t, mate2, mate_inv2, second_min_cost - min_cost, tlist);
            tlist.pop_back();
            nllsum.at(s).at(t) -= M;
            if (!found) continue;

            long long second_min_cost_tmp =
                std::accumulate(f2.cbegin(), f2.cend(), 0LL) + std::accumulate(g2.cbegin(), g2.cend(), 0LL);
            if (second_min_cost_tmp <= second_min_cost) {
                second_min_cost = second_min_cost_tmp;
                second_mate_ = mate2;
                for (int s = 0; s < N; ++s) {
                    if (mate_.at(s) != mate2.at(s)) all_diffs_.push_back({s, mate_.at(s), mate2.at(s)});
                }
            } else {
                assert(false);
            }
        }

        is_second_min_updated_ = true;
        second_mate_diffs_.clear();
        for (int s = 0; s < N; ++s) {
            if (mate(s) != second_mate(s)) second_mate_diffs_.push_back({s, mate(s), second_mate(s)});
        }
        assert(!second_mate_diffs_.empty());
    }

    const std::vector<MateDiff> &second_mate_diffs() const noexcept { return second_mate_diffs_; }

    const MateDiff &second_mate_diff(int i) const {
        assert(0 <= i and i < (int)second_mate_diffs_.size());
        return second_mate_diffs_.at(i);
    }

    const MateDiff &choose_random_second_mate_diff() const {
        assert(!second_mate_diffs_.empty());
        return second_mate_diffs_.at(rnd(0, second_mate_diffs_.size()));
    }

    const MateDiff &choose_random_all_diff() const {
        assert(!all_diffs_.empty());
        return all_diffs_.at(rnd(0, all_diffs_.size()));
    }

    std::string to_string() const {
        std::string ret = "-1 -1 -1\n";
        for (int s = 0; s < N; ++s) {
            assert(0 <= mate(s) and mate(s) < N);
            ret += std::to_string(mate(s)) + '\n';
        }
        ret.pop_back();
        return ret;
    }
};

#endif

class Solver {
    int N;
    std::vector<std::pair<int, int>> yxs;
    WorldMap wm;
    int num_observed_ = 0;
    int sigma_ = -1;

public:
    AssignmentManager am;
    PrecalculatedNLLList nll_list;

    Solver() = default;
    Solver(const std::vector<std::pair<int, int>> &yxs_, const WorldMap &map_, const PrecalculatedNLLList &nll_list_,
           int sigma)
        : N(yxs_.size()), yxs(yxs_), wm(map_), sigma_(sigma), am(yxs_.size()), nll_list(nll_list_) {}

    void observe(int s, int observed, int dy, int dx) {
        assert(-wm.getL() < dy && dy < wm.getL());
        assert(-wm.getL() < dx && dx < wm.getL());
        std::vector<long long> nlls;
        for (auto [y, x] : yxs) {
            int map_val = wm.at((y + dy + wm.getL()) % wm.getL(), (x + dx + wm.getL()) % wm.getL());
            nlls.push_back(nll_list.nllint(observed, map_val));
        }
        am.add_row(s, nlls);
        ++num_observed_;
    }

    int num_observed() const noexcept { return num_observed_; }

    bool is_finished() {
        if (!am.is_second_min_updated()) am.update_second_best();
        double c = 8;
        if (sigma_ < 100) c = 6;
        if (sigma_ == 9) c = 6.3;
        if (sigma_ == 100) c = 7;
        return am.cost_gap() > NLLMultiply * c; // log(100!) = 363.73
    }

    std::tuple<int, int, int> step() {

        const auto &diff =
            am.is_second_min_updated() ? am.choose_random_second_mate_diff() : am.choose_random_all_diff();
        const int starget = diff.s, ttarget1 = diff.t1, ttarget2 = diff.t2;

        int best_diff = -1;
        int num_best_diff = 0;
        int best_dy = 0, best_dx = 0;

        auto [y1, x1] = yxs.at(ttarget1);
        auto [y2, x2] = yxs.at(ttarget2);

        for (int dy = 0; dy < wm.getL(); ++dy) {
            for (int dx = 0; dx < wm.getL(); ++dx) {
                int a = wm.at((y1 + dy) % wm.getL(), (x1 + dx) % wm.getL());
                int b = wm.at((y2 + dy) % wm.getL(), (x2 + dx) % wm.getL());
                int diff = std::min(abs(a - b), sigma_ * 5) * 1e3 / (10 + dist2d(wm.getL(), 0, 0, dy, dx));
                if (diff > best_diff) {
                    best_diff = diff;
                    num_best_diff = 0;
                }
                if (diff == best_diff) {
                    ++num_best_diff;
                    if (rnd(num_best_diff) == 0) {
                        best_dy = dy;
                        best_dx = dx;
                    }
                }
            }
        }

        if (best_dy > wm.getL() / 2) best_dy -= wm.getL();
        if (best_dx > wm.getL() / 2) best_dx -= wm.getL();
        return {starget, best_dy, best_dx};
    }

    void distinct_solve(IOProxy &io_proxy) {

        std::vector<int> dys{0, -1, 1, 0, 0, 1, -1, 1, -1, 2, -2, 0, 0};
        std::vector<int> dxs{0, 0, 0, -1, 1, 1, -1, -1, 1, 0, 0, 2, -2};

        while (num_observed() < MAX_MEASUREMENT) {
            am.update_second_best();


            if (am.cost_gap() > NLLMultiply * 4.9) break;

            const auto &v = am.choose_random_second_mate_diff();
            const int s = v.s, t1 = v.t1, t2 = v.t2;
            int best_diff = -1, best_dir = -1;
            for (int dir = 0; dir < (int)dys.size(); ++dir) {
                int dy = dys.at(dir), dx = dxs.at(dir);
                int a = wm.at(
                    (yxs.at(t1).first + dy + wm.getL()) % wm.getL(), (yxs.at(t1).second + dx + wm.getL()) % wm.getL());
                int b = wm.at(
                    (yxs.at(t2).first + dy + wm.getL()) % wm.getL(), (yxs.at(t2).second + dx + wm.getL()) % wm.getL());
                int diff = abs(a - b);
                if (diff > best_diff) {
                    best_diff = diff;
                    best_dir = dir;
                }
            }

            int m = io_proxy.query(s, dys.at(best_dir), dxs.at(best_dir));
            observe(s, m, dys.at(best_dir), dxs.at(best_dir));
        }
    }

    void normal_solve(IOProxy &io_proxy) {
        const std::vector<int> dys{0, 1, -1, 0, 0, 1, 1, -1, -1};
        const std::vector<int> dxs{0, 0, 0, 1, -1, 1, -1, 1, -1};

        for (int iter = 0; num_observed() < MAX_MEASUREMENT; ++iter) {
            if (am.is_second_min_updated() and is_finished()) break;

            dbgif((iter + 1) % 1024 == 0, iter);
            int s, dy, dx;
            int nc = 4;
            if (sigma_ <= 81) nc = 1;
            if (100 <= sigma_ and sigma_ <= 144) nc = 2;
            const int neighbor_len = N * nc;
            if (iter < neighbor_len) {
                s = iter % N;
                dy = dys.at(iter / N % dys.size());
                dx = dxs.at(iter / N % dys.size());
            } else {
                std::tie(s, dy, dx) = step();
            }
            if (s < 0) break;
            int m = io_proxy.query(s, dy, dx);
            observe(s, m, dy, dx);

            if (iter < N - 1) continue;

            if (num_observed() < 100 or sigma_ <= 500 or N <= 60 or num_observed() % 2 == 0) am.update_second_best();
        }
    }
};

#endif // SOLVER_HPP

using namespace std;

struct fast_ios {
    fast_ios() { cin.tie(nullptr), ios::sync_with_stdio(false), cout << fixed << setprecision(20); };
} fast_ios_;

int main(int argc, char *argv[]) {
    int L, N, sigma;
    cin >> L >> N >> sigma;
    dbg(make_tuple(L, N, sigma));

    vector<pair<int, int>> yxs(N);
    for (auto &[y, x] : yxs) cin >> y >> x;

    const NormalDistirubionInfo nd(sigma);
    const PrecalculatedNLLList pnlls(nd);

    WorldMap wm;
    const bool flg = sigma <= 4;
    if (flg) {
        std::mt19937 mt(0);

        std::vector<int> random_noises(MAX_MEASUREMENT);
        std::normal_distribution<double> nd(0, sigma);
        for (auto &x : random_noises) x = round(nd(mt));

        std::vector<int> dummyA(N);
        for (int i = 0; i < N; ++i) dummyA.at(i) = i;
        std::shuffle(dummyA.begin(), dummyA.end(), mt);

        int last_H = 0;
        long long best_score = -1;
        double best_c = -1;
        const double ccenter = (1.7 - 0.1 * N / L) / sqrt(sigma);
        vector<double> ccands{ccenter * 0.9, ccenter * 0.95, ccenter, ccenter * 1.05, ccenter * 1.1};
        {
            for (double c : ccands) {
                const int H = sigma * c * (N - 1);
                if (H <= last_H) continue;
                last_H = H;

                auto wmtmp = generate_distinct_map(L, H, yxs);

                IOProxy trial_proxy;
                trial_proxy.set_yxs(yxs);
                trial_proxy.set_ground_truth(dummyA, random_noises);

                Solver solver(yxs, wmtmp, pnlls, sigma);

                trial_proxy.set_P(wmtmp.to_mat());
                solver.distinct_solve(trial_proxy);

                trial_proxy.answer(solver.am.mate(), false);

                long long score = trial_proxy.calc_score();
                dbg(std::make_tuple(c, H, score));
                if (score > best_score) {
                    best_score = score;
                    best_c = c;
                    wm = wmtmp;
                }
            }
        }
        dbg(best_c / ccenter);
    } else if (sigma == 900) {
        // wm = generate_log_cliff(L, 1300, yxs, 0);
        wm = generate_log_cliff_parametrized(L, yxs, 0.127, 0.45);
    } else if (sigma == 841) {
        // wm = generate_circular_table(L, 1000, yxs);
        wm = generate_log_cliff(L, 1300, yxs, 0);
        // wm = generate_log_cliff_parametrized(L, yxs, 0.12073197819641737, 0.44287986979032906);
    } else if (sigma == 784) {
        // wm = generate_circular_table(L, 1000, yxs);
        wm = generate_log_cliff(L, 1300, yxs, 0);
        // wm = generate_log_cliff_parametrized(L, yxs, 0.10556927707245306, 0.46314392422904127);
    } else if (sigma == 729) {
        // wm = generate_circular_table(L, 1000, yxs);
        wm = generate_log_cliff(L, 1300, yxs, 0);
        // wm = generate_log_cliff_parametrized(L, yxs, r0, r1);
    } else if (sigma == 676) {
        // wm = generate_circular_table(L, 1000, yxs);
        wm = generate_log_cliff(L, 1300, yxs, 0);
        // wm = generate_log_cliff_parametrized(L, yxs, r0, r1);
    } else if (sigma == 625) {
        // wm = generate_circular_table(L, 900, yxs);
        wm = generate_log_cliff(L, 1200, yxs, 0);
        // wm = generate_log_cliff_parametrized(L, yxs, r0, r1);
    } else if (sigma == 576) {
        // wm = generate_circular_table(L, 850, yxs);
        wm = generate_geo_cliff(L, 1700, yxs, L / 3.5); // 本当は深さで決めたい
        // wm = generate_log_cliff_parametrized(L, yxs, r0, r1);
    } else if (sigma == 529) {
        // wm = generate_circular_table(L, 800, yxs);
        wm = generate_geo_cliff(L, 1600, yxs, L / 4);
        // wm = generate_log_cliff_parametrized(L, yxs, r0, r1);
    } else if (sigma == 16) {
        // L が 10 台だと generate_distinct_with_cliff の方が優秀なケースもまれにある
        // L が大きいほど sigma に乗じる係数を大きくした方がよい
        wm = generate_geo_cliff(L, sigma * 18, yxs);
    } else if (sigma == 4) {
        assert(false);
        wm = generate_geo_cliff(L, sigma * 40, yxs); // sigma = 4 もものによっては勝てそうだが 9 から
    } else if (sigma == 9) {
        wm = generate_geo_cliff(L, sigma * 24, yxs);
    } else if (sigma == 25) {
        wm = generate_geo_cliff(L, sigma * 12, yxs);
    } else if (sigma == 36) {
        wm = generate_geo_cliff(L, sigma * 12, yxs);
    } else if (sigma == 49) {
        wm = generate_geo_cliff(L, sigma * 9, yxs);
    } else if (sigma == 64) {
        wm = generate_geo_cliff(L, sigma * 8, yxs);
    } else if (sigma == 81) {
        wm = generate_geo_cliff(L, sigma * 7, yxs);
    } else if (sigma == 100) {
        wm = generate_geo_cliff(L, sigma * 6, yxs);
    } else if (sigma == 121) {
        wm = generate_geo_cliff(L, sigma * 6, yxs);
    } else if (sigma == 144) {
        wm = generate_geo_cliff(L, sigma * 5, yxs);
    } else if (sigma == 169) {
        wm = generate_geo_cliff(L, sigma * 4, yxs);
    } else if (sigma == 196) {
        wm = generate_geo_cliff(L, sigma * 4, yxs);
    } else if (sigma == 225) {
        wm = generate_geo_cliff(L, sigma * 4, yxs);
    } else if (sigma == 256) {
        wm = generate_geo_cliff(L, sigma * 4, yxs);
    } else if (sigma == 289) {
        wm = generate_geo_cliff(L, 1000, yxs, L / 5);
    } else if (sigma == 324) {
        wm = generate_geo_cliff(L, 1000, yxs, L / 6);
    } else if (sigma == 361) {
        wm = generate_geo_cliff(L, 1200, yxs, L / 6);
    } else if (sigma == 400) {
        wm = generate_geo_cliff(L, 1300, yxs, L / 6);
    } else if (sigma == 441) {
        wm = generate_geo_cliff(L, 1400, yxs, L / 6);
    } else if (sigma == 484) {
        wm = generate_geo_cliff(L, 1500, yxs, L / 6);
        // wm = generate_log_cliff(L, 1000, yxs, L / 4);
    } else if (324 < sigma) {
        assert(false);
        wm = generate_distinct_with_cliff(L, 1000, yxs, sigma);
    } else {
        exit(1);
    }

    IOProxy io_proxy;
    io_proxy.set_yxs(yxs);
#ifdef BENCHMARK
    {
        std::vector<int> ground_truth_A(N);
        for (auto &x : ground_truth_A) std::cin >> x;

        std::vector<int> random_biases(MAX_MEASUREMENT);
        for (auto &x : random_biases) std::cin >> x;
        io_proxy.set_ground_truth(ground_truth_A, random_biases);
    }
#endif
    io_proxy.set_P(wm.to_mat());
    io_proxy.dump_P();

    Solver solver(yxs, wm, pnlls, sigma);

    if (flg) {
        solver.distinct_solve(io_proxy);
    } else {
        solver.normal_solve(io_proxy);
    }

    io_proxy.answer(solver.am.mate(), true);

    const long long map_cost = io_proxy.map_cost();
    const long long query_cost = io_proxy.query_cost();
    jdump("L", L);
    jdump("N", N);
    jdump("sigma", sigma);
    jdump("map_cost", io_proxy.map_cost());
    jdump("num_query", io_proxy.num_query());
    jdump("query_cost", io_proxy.query_cost());
    jdump("wrong", io_proxy.num_wrong_estimation());
    jdump("query_ratio", query_cost / (map_cost + query_cost + 1e-9));
    jdump("raw", io_proxy.calc_score());
    jdump("score", log(io_proxy.calc_score()));
}
