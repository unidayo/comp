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
int floor_lg(long long x) { return x <= 0 ? -1 : 63 - __builtin_clzll(x); }
template <typename T1, typename T2> pair<T1, T2> operator+(const pair<T1, T2> &l, const pair<T1, T2> &r) { return make_pair(l.first + r.first, l.second + r.second); }
template <typename T1, typename T2> pair<T1, T2> operator-(const pair<T1, T2> &l, const pair<T1, T2> &r) { return make_pair(l.first - r.first, l.second - r.second); }
template <typename T> vector<T> sort_unique(vector<T> vec) { sort(vec.begin(), vec.end()), vec.erase(unique(vec.begin(), vec.end()), vec.end()); return vec; }
template <typename T> int arglb(const std::vector<T> &v, const T &x) { return std::distance(v.begin(), std::lower_bound(v.begin(), v.end(), x)); }
template <typename T> int argub(const std::vector<T> &v, const T &x) { return std::distance(v.begin(), std::upper_bound(v.begin(), v.end(), x)); }
template <typename T> istream &operator>>(istream &is, vector<T> &vec) { for (auto &v : vec) is >> v; return is; }
template <typename T> ostream &operator<<(ostream &os, const vector<T> &vec) { os << '['; for (auto v : vec) os << v << ','; os << ']'; return os; }
template <typename T, size_t sz> ostream &operator<<(ostream &os, const array<T, sz> &arr) { os << '['; for (auto v : arr) os << v << ','; os << ']'; return os; }
#if __cplusplus >= 201703L
template <typename... T> istream &operator>>(istream &is, tuple<T...> &tpl) { std::apply([&is](auto &&... args) { ((is >> args), ...);}, tpl); return is; }
template <typename... T> ostream &operator<<(ostream &os, const tuple<T...> &tpl) { os << '('; std::apply([&os](auto &&... args) { ((os << args << ','), ...);}, tpl); return os << ')'; }
#endif
template <typename T> ostream &operator<<(ostream &os, const deque<T> &vec) { os << "deq["; for (auto v : vec) os << v << ','; os << ']'; return os; }
template <typename T> ostream &operator<<(ostream &os, const set<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <typename T, typename TH> ostream &operator<<(ostream &os, const unordered_set<T, TH> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <typename T> ostream &operator<<(ostream &os, const multiset<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <typename T> ostream &operator<<(ostream &os, const unordered_multiset<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <typename T1, typename T2> ostream &operator<<(ostream &os, const pair<T1, T2> &pa) { os << '(' << pa.first << ',' << pa.second << ')'; return os; }
template <typename TK, typename TV> ostream &operator<<(ostream &os, const map<TK, TV> &mp) { os << '{'; for (auto v : mp) os << v.first << "=>" << v.second << ','; os << '}'; return os; }
template <typename TK, typename TV, typename TH> ostream &operator<<(ostream &os, const unordered_map<TK, TV, TH> &mp) { os << '{'; for (auto v : mp) os << v.first << "=>" << v.second << ','; os << '}'; return os; }
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

constexpr int D = 30;

bool isin(int i) { return i >= 0 and i < D; }

int ij1_ij2_to_dir(int ij1, int ij2) {
    if (ij2 == ij1 + D) return 3;
    if (ij2 == ij1 + 1) return 2;
    if (ij2 == ij1 - D) return 1;
    if (ij2 == ij1 - 1) return 0;
    throw;
}

int i_j_to_ij(int i, int j) { return i * D + j; }

const array<int, 4> di{0, -1, 0, 1};
const array<int, 4> dj{-1, 0, 1, 0};
const vector<vector<int>> to = {
    {1, 0, -1, -1},
    {3, -1, -1, 0},
    {-1, -1, 3, 2},
    {-1, 2, 1, -1},
    {1, 0, 3, 2},
    {3, 2, 1, 0},
    {2, -1, 0, -1},
    {-1, 3, -1, 1}
};

struct Agent {
    int i : 8;
    int j : 8;
    int dir : 4;
    int feasible : 4;
    int ij() const { return feasible ? i * D + j : -1; }
    Agent(int i_, int j_, int dir_, int feasible_) : i(i_), j(j_), dir(dir_), feasible(feasible_) {}
    bool operator==(const Agent &x) const {
        return i == x.i and j == x.j and dir == x.dir and feasible == x.feasible;
    }
};

void nextstep(const vector<int> &state, Agent &agent) {
    if (!agent.feasible) return;
    int d2 = to[state[agent.ij()]][agent.dir];
    if (d2 < 0) {
        agent.feasible = false;
        return;
    }
    agent.i += di[d2];
    agent.j += dj[d2];
    agent.dir = d2 ^ 2;
    if (!isin(agent.i) or !isin(agent.j)) agent.feasible = false;
}

lint eval(const vector<int> &state) {
    vector visited(4, vector(D, vector<int>(D)));
    int longest = 0, secondlongest = 0, thirdlongest = 0;
    REP(i_, D) REP(j_, D) REP(dir_, 4) {
        if (visited[dir_][i_][j_]) continue;
        int len = 0;
        Agent agent(i_, j_, dir_, true);
        while (!visited[agent.dir][agent.i][agent.j]) {
            visited[agent.dir][agent.i][agent.j] = 1;
            ++len;
            nextstep(state, agent);
            if (!agent.feasible) break;
        }

        if (agent.dir == dir_ and agent.i == i_ and agent.j == j_) {
            if (len > longest) {
                thirdlongest = secondlongest;
                secondlongest = longest;
                longest = len;
            } else if (len > secondlongest) {
                thirdlongest = secondlongest;
                secondlongest = len;
            } else if (len > thirdlongest) {
                thirdlongest = len;
            }
        }
    }
    return lint(longest) * thirdlongest;
}

int op(int orig, int turn) {
    if (orig < 4) {
        return (orig + turn) % 4;
    } else {
        return orig ^ (turn % 2);
    }
}

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


using Path = vector<int>;
using State = vector<int>;
vector<int> dfs(vector<int> state, int ijstart, int dirstart, int ijgoal, int dirgoal, vector<int> &can_turn, int maxdepth = 20) {
    Agent start(ijstart / D, ijstart % D, dirstart, true);

    vector<int> best_path;
    vector<Agent> seen{start};

    auto rec = [&](auto &&self) -> void {

        if (seen.size() > 1 and seen.back().ij() == ijgoal and seen.back().dir == dirgoal) {
            if (seen.size() > best_path.size()) {
                best_path.clear();
                REP(i, seen.size() - 1) best_path.push_back(seen[i].ij());
            }
            return;
        }
        if (seen.size() > maxdepth) return;

        int ij = seen.back().ij();

        bool turnable = (can_turn[ij] > 0);

        --can_turn[ij];

        auto agentnxt = seen.back();
        nextstep(state, agentnxt);
        if (agentnxt.feasible) {
            seen.push_back(agentnxt);
            self(self);
            seen.pop_back();
        }

        if (turnable) {
            int niter = (state[ij] < 4) ? 3 : 1;
            REP(_, niter) {
                state[ij] = op(state[ij], 1);
                auto agentnxt = seen.back();
                nextstep(state, agentnxt);
                if (agentnxt.feasible) {
                    seen.push_back(agentnxt);
                    self(self);
                    seen.pop_back();
                }
            }
        }

        ++can_turn[ij];
    };
    rec(rec);
    return best_path;
}

void modify_state_for_path(State &state, const Path &path) {
    REP(d, path.size() - 2) {
        int prvij = path[d], ij = path[(d + 1) % path.size()], nxtij = path[(d + 2) % path.size()];

        int dirprv = ij1_ij2_to_dir(ij, prvij);
        int dirnxt = ij1_ij2_to_dir(ij, nxtij);

        while (true) {
            if (to[state[ij]][dirprv] == dirnxt and to[state[ij]][dirnxt] == dirprv) break;
            else {
                state[ij] = op(state[ij], 1);
            }
        }
    }
}

void modify_state_for_circ(State &state, Path circ_path) {
    assert(circ_path.size());
    int a = circ_path.at(0);
    int b = circ_path.at(1);
    circ_path.push_back(a);
    circ_path.push_back(b);
    modify_state_for_path(state, circ_path);
}

vector<int> reader() {
    vector<string> S(D);
    cin >> S;
    vector<int> ret;
    for (auto s : S) {
        for (auto c : s) ret.push_back(c - '0');
    }
    return ret;
}

string dump_result(vector<int> init, const vector<int> &goal) {
    string ret;
    REP(i, D) REP(j, D) {
        int t = 0;
        while (init[i * D + j] != goal[i * D + j]) ++t, init[i * D + j] = op(init[i * D + j], 1);
        ret += (char)('0' + t);
    }
    return ret;
}

void trim_double_loop(vector<int> &loop) {
    if (loop.size() % 2 == 0) {
        int D = loop.size() / 2;
        bool doubled = true;
        REP(i, D) if (loop[i] != loop[i + D]) doubled = false;
        if (doubled) { loop.resize(D); }
    }
}

using Tpl = tuple<State, vector<int>, vector<int>>;
Tpl solve(State S, int niter, vector<int> path1, vector<int> path2, int maxdepth = 20) {
    vector<int> can_turn(D * D, 1);
    if (path1.empty()) {
        // int i1 = -1, j1 = -1;

        bool type = rand_int() % 2;
        while (path1.empty()) {
            // i1 = rand_int() % (D - 2) + 1, j1 = rand_int() % (D - 2) + 1;
            int i1 = (2 + rand_int() % 5);
            if (type) i1 = D - 1 - i1;
            int ij = i1 * D + 2 + rand_int() % 5;
            // int ij = i1 * D + j1;
            int dir = rand_int() % 4;
            path1 = dfs(S, ij, dir, ij, dir, can_turn, 20);
        }
        modify_state_for_circ(S, path1);
        for (auto i : path1) can_turn[i]--;

        while (path2.empty()) {
            // int i2 = rand_int() % (D - 2) + 1, j2 = rand_int() % (D - 2) + 1;
            int i2 = (D - 1 - 2 - rand_int() % 5);
            if (type) i2 = D - 1 - i2;
            int ij = i2 * D + (D - 1 - 2 - rand_int() % 5);
            // if (abs(i1 - i2) + abs(j1 - j2) <= 12) continue;
            // int ij = i2 * D + j2;
            int dir = rand_int() % 4;
            path2 = dfs(S, ij, dir, ij, dir, can_turn, 20);
        }
        modify_state_for_circ(S, path2);
        for (auto i : path2) can_turn[i]--;
    } else {
        modify_state_for_circ(S, path1);
        for (auto i : path1) can_turn[i]--;
        modify_state_for_circ(S, path2);
        for (auto i : path2) can_turn[i]--;
    }

    REP(t, niter) {
        int len1 = path1.size(), len2 = path2.size();
        int prob = rand_int() % (len1 + len2);
        if (prob < len1) { swap(path1, path2); }
        int l = rand_int() % path1.size();
        rotate(path1.begin(), path1.begin() + l, path1.end());
        int w = rand_int() % 9 + 12;
        chmin<int>(w, path1.size() - 2);
        FOR(i, 1, w + 1)++ can_turn[path1[i]];
        auto arg_path = dfs(S, path1[1], ij1_ij2_to_dir(path1[1], path1[0]), path1[w + 1], ij1_ij2_to_dir(path1[w + 1], path1[w]), can_turn, maxdepth);

        path1.erase(path1.begin() + 1, path1.begin() + 1 + w);
        for (auto i : arg_path) --can_turn[i];
        path1.insert(path1.begin() + 1, arg_path.begin(), arg_path.end());
        modify_state_for_circ(S, path1);
    }
    return {S, path1, path2};
}

void climb(State &S) {
    lint score = eval(S);
    int r = rand_int() % D, c = rand_int() % D, w = rand_int() % 4;
    REP(iter, 12000) {
        auto T = S;
        FOR(i, r - w, r + w + 1) {
            if (!isin(i)) continue;
            FOR(j, c - w, c + w + 1) {
                if (!isin(j)) continue;
                T[i * D + j] = op(T[i * D + j], rand_int() % 4);
            }
        }
        if (chmax(score, eval(T))) {
            dbg(make_tuple(string("Score improved"), score));
            S = T;
        }
    }
}

int main(int argc, char *argv[]) {
    const auto Sinit = reader();
    Tpl beststate = make_tuple(Sinit, vector<int>{}, vector<int>{});
    lint bestscore = -1;

    auto START = std::chrono::system_clock::now();

    int nloop = 0;
    while (true) {
        int64_t spent_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - START)
            .count();
        if (spent_ms > 1400) break;
        auto S = Sinit;
        REP(i, D * D) S[i] = op(S[i], rand_int() % 4);
        Tpl cand = solve(S, 10000, {}, {});
        if (chmax(bestscore, eval(get<0>(cand)))) beststate = cand;
        ++nloop;
    }
    cerr << nloop << endl;

    {
        auto [bestS, bestpath1, bestpath2] = beststate;
        Tpl cand = solve(bestS, 30000, bestpath1, bestpath2, 22);
        if (chmax(bestscore, eval(get<0>(cand)))) beststate = cand;
    }

    {
        auto [bestS, bestpath1, bestpath2] = beststate;
        dump_onlinejudge(dump_result(Sinit, bestS));
        dbg(eval(bestS));
        bench(eval(bestS));
        bench(dump_result(Sinit, bestS));
    }
}
