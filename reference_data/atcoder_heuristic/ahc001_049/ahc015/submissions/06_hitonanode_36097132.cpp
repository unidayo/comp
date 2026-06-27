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
using namespace std;
using lint = long long;
using pint = pair<int, int>;
using plint = pair<lint, lint>;
struct fast_ios { fast_ios(){ cin.tie(nullptr), ios::sync_with_stdio(false), cout << fixed << setprecision(20); }; } fast_ios_;
#define ALL(x) (x).begin(), (x).end()
#define FOR(i, begin, end) for(int i=(begin),i##_end_=(end);i<i##_end_;i++)
#define IFOR(i, begin, end) for(int i=(end)-1,i##_begin_=(begin);i>=i##_begin_;i--)
#define REP(i, n) FOR(i,0,n)
#define IREP(i, n) IFOR(i,0,n)
template <typename T, typename V>
void ndarray(vector<T>& vec, const V& val, int len) { vec.assign(len, val); }
template <typename T, typename V, typename... Args> void ndarray(vector<T>& vec, const V& val, int len, Args... args) { vec.resize(len), for_each(begin(vec), end(vec), [&](T& v) { ndarray(v, val, args...); }); }
template <typename T> bool chmax(T &m, const T q) { return m < q ? (m = q, true) : false; }
template <typename T> bool chmin(T &m, const T q) { return m > q ? (m = q, true) : false; }
const std::vector<std::pair<int, int>> grid_dxs{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
int floor_lg(long long x) { return x <= 0 ? -1 : 63 - __builtin_clzll(x); }
template <class T1, class T2> T1 floor_div(T1 num, T2 den) { return (num > 0 ? num / den : -((-num + den - 1) / den)); }
template <class T1, class T2> std::pair<T1, T2> operator+(const std::pair<T1, T2> &l, const std::pair<T1, T2> &r) { return std::make_pair(l.first + r.first, l.second + r.second); }
template <class T1, class T2> std::pair<T1, T2> operator-(const std::pair<T1, T2> &l, const std::pair<T1, T2> &r) { return std::make_pair(l.first - r.first, l.second - r.second); }
template <class T> std::vector<T> sort_unique(std::vector<T> vec) { sort(vec.begin(), vec.end()), vec.erase(unique(vec.begin(), vec.end()), vec.end()); return vec; }
template <class T> int arglb(const std::vector<T> &v, const T &x) { return std::distance(v.begin(), std::lower_bound(v.begin(), v.end(), x)); }
template <class T> int argub(const std::vector<T> &v, const T &x) { return std::distance(v.begin(), std::upper_bound(v.begin(), v.end(), x)); }
template <class IStream, class T> IStream &operator>>(IStream &is, std::vector<T> &vec) { for (auto &v : vec) is >> v; return is; }

template <class OStream, class T> OStream &operator<<(OStream &os, const std::vector<T> &vec);
template <class OStream, class T, size_t sz> OStream &operator<<(OStream &os, const std::array<T, sz> &arr);
template <class OStream, class T, class TH> OStream &operator<<(OStream &os, const std::unordered_set<T, TH> &vec);
template <class OStream, class T, class U> OStream &operator<<(OStream &os, const pair<T, U> &pa);
template <class OStream, class T> OStream &operator<<(OStream &os, const std::deque<T> &vec);
template <class OStream, class T> OStream &operator<<(OStream &os, const std::set<T> &vec);
template <class OStream, class T> OStream &operator<<(OStream &os, const std::multiset<T> &vec);
template <class OStream, class T> OStream &operator<<(OStream &os, const std::unordered_multiset<T> &vec);
template <class OStream, class T, class U> OStream &operator<<(OStream &os, const std::pair<T, U> &pa);
template <class OStream, class TK, class TV> OStream &operator<<(OStream &os, const std::map<TK, TV> &mp);
template <class OStream, class TK, class TV, class TH> OStream &operator<<(OStream &os, const std::unordered_map<TK, TV, TH> &mp);
template <class OStream, class... T> OStream &operator<<(OStream &os, const std::tuple<T...> &tpl);

template <class OStream, class T> OStream &operator<<(OStream &os, const std::vector<T> &vec) { os << '['; for (auto v : vec) os << v << ','; os << ']'; return os; }
template <class OStream, class T, size_t sz> OStream &operator<<(OStream &os, const std::array<T, sz> &arr) { os << '['; for (auto v : arr) os << v << ','; os << ']'; return os; }
#if __cplusplus >= 201703L
template <class... T> std::istream &operator>>(std::istream &is, std::tuple<T...> &tpl) { std::apply([&is](auto &&... args) { ((is >> args), ...);}, tpl); return is; }
template <class OStream, class... T> OStream &operator<<(OStream &os, const std::tuple<T...> &tpl) { os << '('; std::apply([&os](auto &&... args) { ((os << args << ','), ...);}, tpl); return os << ')'; }
#endif
template <class OStream, class T, class TH> OStream &operator<<(OStream &os, const std::unordered_set<T, TH> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <class OStream, class T> OStream &operator<<(OStream &os, const std::deque<T> &vec) { os << "deq["; for (auto v : vec) os << v << ','; os << ']'; return os; }
template <class OStream, class T> OStream &operator<<(OStream &os, const std::set<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <class OStream, class T> OStream &operator<<(OStream &os, const std::multiset<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <class OStream, class T> OStream &operator<<(OStream &os, const std::unordered_multiset<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <class OStream, class T, class U> OStream &operator<<(OStream &os, const std::pair<T, U> &pa) { return os << '(' << pa.first << ',' << pa.second << ')'; }
template <class OStream, class TK, class TV> OStream &operator<<(OStream &os, const std::map<TK, TV> &mp) { os << '{'; for (auto v : mp) os << v.first << "=>" << v.second << ','; os << '}'; return os; }
template <class OStream, class TK, class TV, class TH> OStream &operator<<(OStream &os, const std::unordered_map<TK, TV, TH> &mp) { os << '{'; for (auto v : mp) os << v.first << "=>" << v.second << ','; os << '}'; return os; }
#ifdef HITONANODE_LOCAL
const string COLOR_RESET = "\033[0m", BRIGHT_GREEN = "\033[1;32m", BRIGHT_RED = "\033[1;31m", BRIGHT_CYAN = "\033[1;36m", NORMAL_CROSSED = "\033[0;9;37m", RED_BACKGROUND = "\033[1;41m", NORMAL_FAINT = "\033[0;2m";
#define dbg(x) cerr << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") " << __FILE__ << COLOR_RESET << endl
#define dbgif(cond, x) ((cond) ? cerr << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") " << __FILE__ << COLOR_RESET << endl : cerr)
#else
#define dbg(x) 0
#define dbgif(cond, x) 0
#endif
#ifdef BENCHMARK
#define bench(x) (cout << (x) << endl)
#define dump_onlinejudge(x) 0
#else
#define dump_onlinejudge(x) (cout << (x) << endl)
#define bench(x) 0
#endif

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


using State = pair<int, bitset<200>>;  // (現在手数, 各マス状態)
struct Comparer {
    bool operator()(const State &l, const State &r) const {
        if (l.first != r.first) return l.first < r.first;
        // REP(i, 200) {
        //     if (l.second[i] != r.second[i]) return l.second[i] < r.second[i];
        // }
        auto b = l.second ^ r.second;
        int i = b._Find_first();
        if (i < 200) return l.second[i] < r.second[i];
        return false;
    }
};

constexpr int D = 10;
bool isin(int x) noexcept { return 0 <= x and x < D; }
bool isin(int x, int y) noexcept { return isin(x) and isin(y); }

int f(int i, int j) noexcept {
    return i * D + j;
}

int get_f(const State &s, int fij) noexcept { return s.second[fij * 2] * 2 + s.second[fij * 2 + 1]; }

int get_ij(const State &s, int i, int j) noexcept { return get_f(s, f(i, j)); }

State initialize() noexcept {
    State ret;
    ret.first = 0;
    ret.second.reset();
    return ret;
}

State &set_f(State &s, int fij, int vnew) noexcept {
    int vold = get_f(s, fij);
    if (vold and !vnew) --s.first;
    if (!vold and vnew) ++s.first;
    s.second[fij * 2] = vnew / 2;
    s.second[fij * 2 + 1] = vnew % 2;
    return s;
}

State &set_ij(State &s, int i, int j, int vnew) noexcept { return set_f(s, f(i, j), vnew); }

State &spawn(State &s, int p, int vnew) noexcept {
    assert(1 <= p and s.first + p <= D * D);
    --p;
    int fij = 0;
    while (p) {
        if (!get_f(s, fij)) --p;
        fij++;
    }
    while (get_f(s, fij)) ++fij;
    assert(0 <= fij and fij < D * D);
    return set_f(s, fij, vnew);
}

vector<vector<int>> dump_vv(const State &s) {
    vector ret(D, vector<int>(D));
    REP(i, D) REP(j, D) ret.at(i).at(j) = get_ij(s, i, j);
    return ret;
}

// FBLR
// F: 上に移動
State &move_up_f(State &s) {
    REP(j, D) {
        int nxt = f(0, j);
        REP(i, D) {
            int v = get_ij(s, i, j);
            if (v) {
                set_ij(s, i, j, 0);
                set_f(s, nxt, v);
                nxt += D;
            }
        }
    }
    return s;
}

// B: 下に移動
State &move_down_b(State &s) {
    REP(j, D) {
        int nxt = f(D - 1, j);
        IREP(i, D) {
            int v = get_ij(s, i, j);
            if (v) {
                set_ij(s, i, j, 0);
                set_f(s, nxt, v);
                nxt -= D;
            }
        }
    }
    return s;
}

// L: 左に移動
State &move_left_l(State &s) {
    REP(i, D) {
        int nxt = f(i, 0);
        REP(j, D) {
            int v = get_ij(s, i, j);
            if (v) {
                set_ij(s, i, j, 0);
                set_f(s, nxt++, v);
            }
        }
    }
    return s;
}

// R: 右に移動
State &move_right_r(State &s) {
    REP(i, D) {
        int nxt = f(i, D - 1);
        IREP(j, D) {
            int v = get_ij(s, i, j);
            if (v) {
                set_ij(s, i, j, 0);
                set_f(s, nxt--, v);
            }
        }
    }
    return s;
}

const string hands_str = "FBLR";

State &move_to(State &s, int dir) {
    assert(0 <= dir and dir < 4);
    if (dir == 0) {
        return move_up_f(s);
    } else if (dir == 1) {
        return move_down_b(s);
    } else if (dir == 2) {
        return move_left_l(s);
    } else if (dir == 3) {
        return move_right_r(s);
    } else {
        exit(1);
    }
}

// 現時点の各連結成分のサイズの 2 乗の総和（割らない）
int sum_of_quad_of_connected_components(const State &s) {
    UnionFind uf(D * D);
    array<int, D * D> counter;
    counter.fill(0);
    REP(i, D) REP(j, D) {
        const int fij = f(i, j), vij = get_f(s, fij);
        if (!vij) continue;
        if (i + 1 < D and get_ij(s, i + 1, j) == vij) uf.unite(fij, f(i + 1, j));
        if (j + 1 < D and get_ij(s, i, j + 1) == vij) uf.unite(fij, f(i, j + 1));
    }
    REP(fij, D * D) {
        if (!get_f(s, fij)) continue;
        counter.at(uf.find(fij)) = uf.count(fij);
    }
    int ret = 0;
    for (auto x : counter) ret += x * x;
    return ret;
}

// 現時点の各連結成分のサイズの r 乗の総和（割らない）
double sum_of_pow_of_connected_components(const State &s, const double r) {
    if (r == 2) return sum_of_quad_of_connected_components(s);
    UnionFind uf(D * D);
    array<int, D * D> counter;
    counter.fill(0);
    REP(i, D) REP(j, D) {
        const int fij = f(i, j), vij = get_f(s, fij);
        if (!vij) continue;
        if (i + 1 < D and get_ij(s, i + 1, j) == vij) uf.unite(fij, f(i + 1, j));
        if (j + 1 < D and get_ij(s, i, j + 1) == vij) uf.unite(fij, f(i, j + 1));
    }
    REP(fij, D * D) {
        if (!get_f(s, fij)) continue;
        counter.at(uf.find(fij)) = uf.count(fij);
    }
    double ret = 0;
    for (auto x : counter) ret += pow(x, r);
    return ret;
}

// 現在の盤面のみ評価、現在が終了状態とした時、問題の目的関数 f(x) の次数を 2 -> r として評価
double current_connectivity_ratio_pow_r(const State &s, const double r) {
    double num = sum_of_pow_of_connected_components(s, r);
    int n1 = 0, n2 = 0, n3 = 0;
    REP(i, D) REP(j, D) {
        int v = get_ij(s, i, j);
        if (v == 1) ++n1;
        if (v == 2) ++n2;
        if (v == 3) ++n3;
    }
    double den = pow(n1, r) + pow(n2, r) + pow(n3, r) + 1e-6;
    return num / den;
}

// [0, 1], 現在の盤面のみ評価、現在が終了状態とした時の問題の目的関数 f(x)
double current_connectivity_ratio(const State &s) {
    int num = sum_of_quad_of_connected_components(s);
    int n1 = 0, n2 = 0, n3 = 0;
    REP(i, D) REP(j, D) {
        int v = get_ij(s, i, j);
        if (v == 1) ++n1;
        if (v == 2) ++n2;
        if (v == 3) ++n3;
    }
    double den = n1 * n1 + n2 * n2 + n3 * n3 + 1e-6;
    return num / den;
}

// [0, 1], 現在の盤面のみ評価、問題の目的関数
double calc_score(const State &s, const vector<int> &flavor_count) {
    int num = sum_of_quad_of_connected_components(s);
    int den = 0;
    for (auto x : flavor_count) den += x * x;
    return 1.0 * num / den;
}

// 現在の盤面で色 0 と色 i の境界線の長さをそれぞれの i について求める
//   0   1   2   3   4   5   6,  7,  8,  9
// (--, 01, 02, 03, 12, 13, 23, 11, 22, 33)
array<int, 10> calc_border_len(const State &s) {
    array<int, 10> ret;
    ret.fill(0);
    REP(i, D) REP(j, D) {
        const int fij = f(i, j), v = get_f(s, fij);
        if (!v) continue;
        for (auto [dx, dy] : grid_dxs) {
            int ni = i + dx, nj = j + dy;
            if (!isin(ni, nj)) continue;
            int vnxt = get_ij(s, ni, nj);
            if (vnxt and v != vnxt) ret.at(v + vnxt + 1)++;
            if (vnxt and v == vnxt) ret.at(v + 6)++;
            if (!vnxt) ret.at(v)++;
        }
    }
    FOR(d, 4, 10) ret[d] /= 2;
    return ret;
}

// [0, 1], 一手だけ動かすことでそれぞれの色 i と色 0 の境界がどの程度確保できるか
array<double, 4> calc_border_clarity(const State &s) noexcept {
    array<double, 4> hi;
    hi.fill(0);
    REP(dir, 4) {
        auto snxt = s;
        move_to(snxt, dir);
        const auto b = calc_border_len(snxt);
        const double den = 1e-6 + b[1] + b[2] + b[3];
        FOR(p, 1, 4) chmax(hi.at(p), b.at(p) / den);
    }
    return hi;
}
double calc_border_clarity_all(const State &s) noexcept {
    auto hi = calc_border_clarity(s);
    return (hi[1] + hi[2] + hi[3]) / 3;
}

// 違う色同士の境界線の少なさを評価
double calc_nomix_score(const State &s) noexcept {
    auto v = calc_border_len(s);
    return (v[7] + v[8] + v[9]) / (v[4] + v[5] + v[6] + v[7] + v[8] + v[9] + 1e-10);
}

// [0, 1], 一手だけ動かすことでそれぞれの色がどれだけ結合できるか
// double calc_coupling_coeff(const State &s) {
//     array<double, 4> hi;
//     hi.fill(0);
//     REP(dir, 4) {
//         auto snxt = s;
//         move_to(snxt, dir);
//     }
// }

struct OptunaParam {
    double pdeg = 3.266570776962251;
    double c0_connectivity = 0.6315824305675417;
    double c0_nomix = 0.25159979312799735;
    double c0_clarity = 0.4067725806522986;
    double c0_border = 0.19210396746161096;

    double t1_nomix = 0.7219454894853917;
    double t1_clarity = 0.5311704752871291;
    double t1_border = 0.41678987302566145;
} param;

unordered_map<bitset<200>, double> memo_eval_0; // 評価値メモ

double evaluate_current_situation(const State &s, const vector<int> &flavor, const vector<int> &flavor_count, int rem_recursive = 0) {

    // if (!rem_recursive and memo_eval_0.count(s.second)) return memo_eval_0[s.second];

    // if (rem_recursive and s.first < D * D) {
    //     const int vnew = flavor.at(s.first);
    //     double num = 0.0, den = 0.0;
    //     FOR(p, 1, D * D - s.first + 1) {
    //         double hi = -1e30;
    //         REP(dir, 4) {
    //             auto s2 = s;
    //             spawn(s2, p, vnew);
    //             move_to(s2, dir);
    //             chmax(hi, evaluate_current_situation(s2, flavor, flavor_count, rem_recursive - 1));
    //         }
    //         num += hi;
    //         den += 1.0;
    //     }
    //     return num / den;
    // }

    // 連結度評価 1 手のみ動かして良い
    double connectivity_score = 0.0;
    REP(dir, 4) {
        auto ss = s;
        move_to(ss, dir);
        chmax(connectivity_score, current_connectivity_ratio_pow_r(ss, param.pdeg));
    }

    // 境界線評価 1 手のみ動かして良い
    double clarity_score = calc_border_clarity_all(s);

    // 非混合度評価
    double nomix_score = calc_nomix_score(s);

    // 境界線が次の手にふさわしいか評価
    double border_score = 0.0;
    if (s.first < D * D) {
        const auto bl = calc_border_len(s);
        const int vnxt = flavor.at(s.first);
        border_score = 1.0 * bl.at(vnxt) / (1e-10 + bl[1] + bl[2] + bl[3]);
    }

    const double timing = 1.0 / (D * D) * s.first;  // 0 -> 1

    double ret = (param.c0_connectivity * connectivity_score * timing //
                  + param.c0_nomix * nomix_score * (1.0 - timing * param.t1_nomix)     //
                  + param.c0_clarity * clarity_score * (1.0 - timing * param.t1_clarity) //
                  + param.c0_border * border_score * (1.0 - timing * param.t1_border));

    return ret;
    // return memo_eval_0[s.second] = ret;
}

unordered_map<bitset<200>, pair<double, int>> memo_exp; // 期待値計算

// 終盤の全探索
pair<double, int> search_best(const State &state, const vector<int> &flavor, const vector<int> &flavor_count) {

    if (state.first == D * D) return memo_exp[state.second] = pair<double, int>(calc_score(state, flavor_count), -1);

    if (memo_exp.count(state.second)) return memo_exp.at(state.second);

    const int vnew = flavor.at(state.first);

    double best_exp = -1;
    int best_dir = -1;
    REP(dir, 4) {
        auto s2 = state;
        move_to(s2, dir);

        double num = 0.0, den = 0.0;
        FOR(p, 1, D * D - state.first + 1) {
            auto s3 = s2;
            spawn(s3, p, vnew);
            num += search_best(s3, flavor, flavor_count).first;
            den += 1.0;
        }
        if (chmax(best_exp, num / den)) best_dir = dir;
    }

    return memo_exp[state.second] = make_pair(best_exp, best_dir);
}

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        // param.pdeg = std::stod(argv[1]);
        param.c0_connectivity = std::stod(argv[1]);
        param.c0_nomix = std::stod(argv[2]);
        param.c0_clarity = std::stod(argv[3]);
        param.c0_border = std::stod(argv[4]);
        param.t1_nomix = std::stod(argv[5]);
        param.t1_clarity = std::stod(argv[6]);
        param.t1_border = std::stod(argv[7]);
    }
    vector<int> flavor(D * D);
    for (auto &x : flavor) cin >> x;
    vector<int> flavor_count(4);
    for (auto x : flavor) flavor_count.at(x)++;

    State state = initialize();
    string ret_to_oj = "";
    REP(i, D * D) {
        int p;
        cin >> p;
        spawn(state, p, flavor.at(i));

        if (i + 1 < D * D) {
            int best_dir = -1;
            double best_eval = -1e300;

            if (state.first < D * D - 6) {
                REP(dir, 4) {
                    auto state2 = state;
                    move_to(state2, dir);
                    if (chmax(best_eval, evaluate_current_situation(state2, flavor, flavor_count, 0))) {
                        best_dir = dir;
                    }
                }
            } else {
                tie(best_eval, best_dir) = search_best(state, flavor, flavor_count);
            }

            dump_onlinejudge(hands_str.at(best_dir));
            ret_to_oj += hands_str.at(best_dir);
            ret_to_oj += "\n";
            move_to(state, best_dir);
        }
    }
    bench(calc_score(state, flavor_count));
    bench(ret_to_oj);
    dbg(make_tuple(calc_score(state, flavor_count), memo_exp.size()));
    dbg(calc_border_len(state));
}
