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
#define TIMELIMIT 5000
const string COLOR_RESET = "\033[0m", BRIGHT_GREEN = "\033[1;32m", BRIGHT_RED = "\033[1;31m", BRIGHT_CYAN = "\033[1;36m", NORMAL_CROSSED = "\033[0;9;37m", RED_BACKGROUND = "\033[1;41m", NORMAL_FAINT = "\033[0;2m";
#define dbg(x) cerr << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") " << __FILE__ << COLOR_RESET << endl
#define dbgif(cond, x) ((cond) ? cerr << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") " << __FILE__ << COLOR_RESET << endl : cerr)
#else
#define TIMELIMIT 1950
#define dbg(x) 0
#define dbgif(cond, x) 0
#endif

template <typename T, T INF = std::numeric_limits<T>::max() / 2, int INVALID = -1>
struct ShortestPath {
    int V, E;
    bool single_positive_weight;
    T wmin, wmax;
    std::vector<std::vector<std::pair<int, T>>> to;

    ShortestPath(int V = 0) : V(V), E(0), single_positive_weight(true), wmin(0), wmax(0), to(V) {}
    void add_edge(int s, int t, T w) {
        assert(0 <= s and s < V);
        assert(0 <= t and t < V);
        to[s].emplace_back(t, w);
        E++;
        if (w > 0 and wmax > 0 and wmax != w) single_positive_weight = false;
        wmin = std::min(wmin, w);
        wmax = std::max(wmax, w);
    }

    std::vector<T> dist;
    std::vector<int> prev;

    // Dijkstra algorithm
    // Complexity: O(E log E)
    using Pque = std::priority_queue<std::pair<T, int>, std::vector<std::pair<T, int>>,
                                     std::greater<std::pair<T, int>>>;
    template <class Heap = Pque> void Dijkstra(int s) {
        assert(0 <= s and s < V);
        dist.assign(V, INF);
        dist[s] = 0;
        prev.assign(V, INVALID);
        Heap pq;
        pq.emplace(0, s);
        while (!pq.empty()) {
            T d;
            int v;
            std::tie(d, v) = pq.top();
            pq.pop();
            if (dist[v] < d) continue;
            for (auto nx : to[v]) {
                T dnx = d + nx.second;
                if (dist[nx.first] > dnx) {
                    dist[nx.first] = dnx, prev[nx.first] = v;
                    pq.emplace(dnx, nx.first);
                }
            }
        }
    }

    // Dijkstra algorithm, O(V^2 + E)
    void DijkstraVquad(int s) {
        assert(0 <= s and s < V);
        dist.assign(V, INF);
        dist[s] = 0;
        prev.assign(V, INVALID);
        std::vector<char> fixed(V, false);
        while (true) {
            int r = INVALID;
            T dr = INF;
            for (int i = 0; i < V; i++) {
                if (!fixed[i] and dist[i] < dr) r = i, dr = dist[i];
            }
            if (r == INVALID) break;
            fixed[r] = true;
            int nxt;
            T dx;
            for (auto p : to[r]) {
                std::tie(nxt, dx) = p;
                if (dist[nxt] > dist[r] + dx) dist[nxt] = dist[r] + dx, prev[nxt] = r;
            }
        }
    }

    // Bellman-Ford algorithm
    // Complexity: O(VE)
    bool BellmanFord(int s, int nb_loop) {
        assert(0 <= s and s < V);
        dist.assign(V, INF), prev.assign(V, INVALID);
        dist[s] = 0;
        for (int l = 0; l < nb_loop; l++) {
            bool upd = false;
            for (int v = 0; v < V; v++) {
                if (dist[v] == INF) continue;
                for (auto nx : to[v]) {
                    T dnx = dist[v] + nx.second;
                    if (dist[nx.first] > dnx) dist[nx.first] = dnx, prev[nx.first] = v, upd = true;
                }
            }
            if (!upd) return true;
        }
        return false;
    }

    // Bellman-ford algorithm using queue (deque)
    // Complexity: O(VE)
    // Requirement: no negative loop
    void SPFA(int s) {
        assert(0 <= s and s < V);
        dist.assign(V, INF);
        prev.assign(V, INVALID);
        std::deque<int> q;
        std::vector<char> in_queue(V);
        dist[s] = 0;
        q.push_back(s), in_queue[s] = 1;
        while (!q.empty()) {
            int now = q.front();
            q.pop_front(), in_queue[now] = 0;
            for (auto nx : to[now]) {
                T dnx = dist[now] + nx.second;
                int nxt = nx.first;
                if (dist[nxt] > dnx) {
                    dist[nxt] = dnx;
                    if (!in_queue[nxt]) {
                        if (q.size() and dnx < dist[q.front()]) { // Small label first optimization
                            q.push_front(nxt);
                        } else {
                            q.push_back(nxt);
                        }
                        prev[nxt] = now, in_queue[nxt] = 1;
                    }
                }
            }
        }
    }

    void ZeroOneBFS(int s) {
        assert(0 <= s and s < V);
        dist.assign(V, INF), prev.assign(V, INVALID);
        dist[s] = 0;
        std::deque<int> que;
        que.push_back(s);
        while (!que.empty()) {
            int v = que.front();
            que.pop_front();
            for (auto nx : to[v]) {
                T dnx = dist[v] + nx.second;
                if (dist[nx.first] > dnx) {
                    dist[nx.first] = dnx, prev[nx.first] = v;
                    if (nx.second) {
                        que.push_back(nx.first);
                    } else {
                        que.push_front(nx.first);
                    }
                }
            }
        }
    }

    bool dag_solver(int s) {
        assert(0 <= s and s < V);
        std::vector<int> indeg(V, 0);
        std::queue<int> que;
        que.push(s);
        while (que.size()) {
            int now = que.front();
            que.pop();
            for (auto nx : to[now]) {
                indeg[nx.first]++;
                if (indeg[nx.first] == 1) que.push(nx.first);
            }
        }
        dist.assign(V, INF), prev.assign(V, INVALID);
        dist[s] = 0;
        que.push(s);
        while (que.size()) {
            int now = que.front();
            que.pop();
            for (auto nx : to[now]) {
                indeg[nx.first]--;
                if (dist[nx.first] > dist[now] + nx.second)
                    dist[nx.first] = dist[now] + nx.second, prev[nx.first] = now;
                if (indeg[nx.first] == 0) que.push(nx.first);
            }
        }
        return *max_element(indeg.begin(), indeg.end()) == 0;
    }

    // Retrieve a sequence of vertex ids that represents shortest path [s, ..., goal]
    // If not reachable to goal, return {}
    std::vector<int> retrieve_path(int goal) const {
        assert(int(prev.size()) == V);
        assert(0 <= goal and goal < V);
        if (dist[goal] == INF) return {};
        std::vector<int> ret{goal};
        while (prev[goal] != INVALID) {
            goal = prev[goal];
            ret.push_back(goal);
        }
        std::reverse(ret.begin(), ret.end());
        return ret;
    }

    void solve(int s) {
        if (wmin >= 0) {
            if (single_positive_weight) {
                ZeroOneBFS(s);
            } else {
                if ((long long)V * V < (E << 4)) {
                    DijkstraVquad(s);
                } else {
                    Dijkstra(s);
                }
            }
        } else {
            BellmanFord(s, V);
        }
    }

    // Warshall-Floyd algorithm
    // Complexity: O(E + V^3)
    std::vector<std::vector<T>> dist2d;
    void WarshallFloyd() {
        dist2d.assign(V, std::vector<T>(V, INF));
        for (int i = 0; i < V; i++) {
            dist2d[i][i] = 0;
            for (auto p : to[i]) dist2d[i][p.first] = std::min(dist2d[i][p.first], p.second);
        }
        for (int k = 0; k < V; k++) {
            for (int i = 0; i < V; i++) {
                if (dist2d[i][k] == INF) continue;
                for (int j = 0; j < V; j++) {
                    if (dist2d[k][j] == INF) continue;
                    dist2d[i][j] = std::min(dist2d[i][j], dist2d[i][k] + dist2d[k][j]);
                }
            }
        }
    }

    void dump_graphviz(std::string filename = "shortest_path") const {
        std::ofstream ss(filename + ".DOT");
        ss << "digraph{\n";
        for (int i = 0; i < V; i++) {
            for (const auto &e : to[i])
                ss << i << "->" << e.first << "[label=" << e.second << "];\n";
        }
        ss << "}\n";
        ss.close();
        return;
    }
};


constexpr int D = 20;
int f(int i, int j) { return i * D + j; }
pint finv(int x) { return {x / D, x % D}; }
int isin(int i, int j) { return i >= 0 and i < D and j >= 0 and j < D; }
const vector<int> dx{1, -1, 0, 0};
const vector<int> dy{0, 0, 1, -1};
const string mvstr = "DURL";
vector<int> chstrmap(256);

using Float = double;

vector<vector<Float>> matmul(const vector<vector<Float>> &A, const vector<vector<Float>> &B) {
    vector<vector<Float>> ret(A.size(), vector<Float>(B[0].size()));
    REP(i, A.size()) {
        REP(j, A[0].size()) {
            REP(k, B[0].size()) ret[i][k] += A[i][j] * B[j][k];
        }
    }
    return ret;
}

vector<map<vector<int>, vector<vector<Float>>>> mv_to_mat(65);

struct rand_int_ {
    using lint = long long;
    mt19937 mt;
    rand_int_() : mt(chrono::steady_clock::now().time_since_epoch().count()) {}
    lint operator()(lint x) { return this->operator()(0, x); } // [0, x)
    lint operator()(lint l, lint r) {
        uniform_int_distribution<lint> d(l, r - 1);
        return d(mt);
    }
} rnd;




int main() {
    auto START = std::chrono::system_clock::now();

    REP(d, mvstr.size()) chstrmap.at(mvstr[d]) = d;

    int si, sj, ti, tj;
    double p;
    cin >> si >> sj >> ti >> tj >> p;
    vector<string> H(D), V(D - 1);
    cin >> H >> V;
    ShortestPath<int> graph(D * D);
    ShortestPath<int> graph01(D * D);
    REP(i, D) REP(j, D) {
        if (j + 1 < D and H[i][j] == '0') {
            graph.add_edge(f(i, j), f(i, j + 1), (i <= ti and j <= tj) ? 1 : 1);
            graph.add_edge(f(i, j + 1), f(i, j), 1);

            graph01.add_edge(f(i, j), f(i, j + 1), 1);
            graph01.add_edge(f(i, j + 1), f(i, j), 1);
        }
        if (i + 1 < D and V[i][j] == '0') {
            graph.add_edge(f(i, j), f(i + 1, j), (i <= ti and j <= tj) ? 1 : 1);
            graph.add_edge(f(i + 1, j), f(i, j), 1);

            graph01.add_edge(f(i, j), f(i + 1, j), 1);
            graph01.add_edge(f(i + 1, j), f(i, j), 1);
        }
    }
    graph.solve(f(ti, tj));
    const auto dist_to_t = graph.dist;

    vector<Float> penalty_coeff(D * D);
    REP(i, D * D) penalty_coeff[i] = cbrt(dist_to_t[i]);

    auto calc_penalty = [&](const vector<Float> &state) -> Float {
        Float ret = 0.0;
        REP(i, state.size()) ret += state[i] * penalty_coeff[i];
        return ret;
    };

    vector dmap(D, vector<int>(D));
    REP(i, D) REP(j, D) dmap[i][j] = graph.dist[f(i, j)];

    for (auto v : dmap) dbg(v);

    vector sparse_mats(mvstr.size(), vector<vector<pair<int, Float>>>(D * D));
    REP(dir, mvstr.size()) {
        vector mat(D * D, vector<Float>(D * D));
        REP(i, D) REP(j, D) {
            int from = f(i, j);
            graph01.solve(from);
            int ni = i + dx[dir], nj = j + dy[dir];
            if (isin(ni, nj) and graph01.dist[f(ni, nj)] == 1) {
                //
            } else {
                ni = i, nj = j;
            }
            mat[from][from] += p;
            mat[f(ni, nj)][from] += 1 - p;
        }

        REP(i, D * D) mat[f(ti, tj)][i] = 0;
        REP(i, D * D) REP(j, D * D) if (mat[j][i]) sparse_mats[dir][i].emplace_back(j, mat[j][i]);
    }

    auto eval_score = [&](const string &s) -> lint {
        vector<Float> current(D * D);
        current[f(si, sj)] = 1;
        Float remain = 1.0;
        Float ret = 0.0;
        REP(t, s.size()) {
            vector<Float> nxtdist(D * D);
            int dir = find(mvstr.begin(), mvstr.end(), s.at(t)) - mvstr.begin();
            REP(i, current.size()) for (auto [j, c] : sparse_mats[dir][i]) nxtdist[j] += current[i] * c;
            current = nxtdist;
            Float remain_nxt = accumulate(ALL(current), 0.0);
            Float er = remain - remain_nxt;
            ret += er * (400 - t);
            remain = remain_nxt;
        }
        return llround(250000 * ret);
    };

    auto calc_mv = [&](const vector<Float> &dp, int dir) -> vector<Float> {
        vector<Float> ret(dp.size());
        REP(i, dp.size()) {
            for (auto [j, c] : sparse_mats[dir][i]) ret[j] += dp[i] * c;
        }
        return ret;
    };

    auto calc_str = [&](const string &str) -> vector<Float> {
        vector<Float> dp(D * D);
        dp[f(si, sj)] = 1;
        for (auto c : str) dp = calc_mv(dp, chstrmap[c]);
        return dp;
    };

    auto choose_nxt = [&](const vector<Float> &dp) -> pair<vector<int>, vector<Float>> {
        Float bestcost = 1e30;
        vector<int> argbest{};
        vector<Float> bestdp;
        REP(dir, mvstr.size()) {
            vector<Float> nxtdp = calc_mv(dp, dir);
            Float eval = calc_penalty(nxtdp);
            if (chmin(bestcost, eval)) argbest = {dir}, bestdp = nxtdp;
        }
        return {{argbest}, bestdp};
    };

    auto choose_nxnx = [&](const vector<Float> &dp) -> pair<vector<int>, vector<Float>> {
        Float bestcost = 1e30;
        vector<int> argbest{};
        vector<Float> bestdp;
        auto eval0 = calc_penalty(dp);
        REP(dir1, mvstr.size()) {
            auto dp1 = calc_mv(dp, dir1);
            auto eval1 = calc_penalty(dp1);
            REP(dir2, mvstr.size()) {
                auto dp2 = calc_mv(dp1, dir2);
                auto eval2 = calc_penalty(dp2);
                // if (eval2 > eval0) continue;
                REP(dir3, mvstr.size()) {
                    auto dp3 = calc_mv(dp2, dir3);
                    auto eval3 = calc_penalty(dp3);
                    if (eval3 > eval1) continue;
                    REP(dir4, mvstr.size()) {
                        auto dp4 = calc_mv(dp3, dir4);
                        auto eval4 = calc_penalty(dp4);
                        if (eval4 > eval2) continue;
                        REP(dir5, mvstr.size()) {
                            auto dp5 = calc_mv(dp4, dir5);
                            auto eval5 = calc_penalty(dp5);
                            // if (eval5 > eval3) continue;
                            REP(dir6, mvstr.size()) {
                                auto dp6 = calc_mv(dp5, dir6);
                                auto eval6 = calc_penalty(dp6);
                                // if (eval6 > eval4) continue;
                                REP(dir7, mvstr.size()) {
                                    auto dp7 = calc_mv(dp6, dir7);
                                    auto eval7 = calc_penalty(dp7);
                                    if (chmin(bestcost, eval7)) argbest = {dir1, dir2, dir3, dir4, dir5, dir6, dir7}, bestdp = dp7;
                                }
                            }
                        }
                    }
                }
            }
        }
        return {{argbest}, bestdp};
    };

    auto build_greedy = [&](string ret) -> string {
        vector<Float> dp = calc_str(ret);
        while (ret.size() < 200) {
            auto [dirs, nxtdp] = choose_nxnx(dp);
            for (auto dir : dirs) ret += mvstr.at(dir);
            dp = nxtdp;
        }
        while (ret.size() > 200) ret.pop_back();
        return ret;
    };

    int iter = 0;
    auto anneal = [&](string ret) -> string {
        auto f = eval_score(ret);
        while (true) {
            iter++;
            int64_t spent_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - START).count();
            if (spent_ms > TIMELIMIT) break;
            int tp = rnd(2);
            if (tp == 0) {
                string tmp = ret;
                int i = rnd(tmp.size() + 1);
                tmp.insert(tmp.begin() + i, mvstr[rnd(mvstr.size())]);
                int j = rnd(tmp.size());
                tmp.erase(tmp.begin() + j);
                if (chmax(f, eval_score(tmp))) {
                    dbg(make_tuple(iter, tp, f));
                    ret = tmp;
                }
            } else if (tp == 1) {
                string tmp = ret;
                int i = rnd(tmp.size());
                char c = tmp[i];
                tmp.insert(tmp.begin() + i, c);
                int j = rnd(tmp.size());
                tmp.erase(tmp.begin() + j);
                if (chmax(f, eval_score(tmp))) {
                    dbg(make_tuple(iter, tp, f));
                    ret = tmp;
                }
            }
        }
        cerr << iter << '\n';
        return ret;
    };

    auto solve_heur = [&]() {
        ShortestPath<int> all_mv_graph(D * D);
        REP(i, D) REP(j, D) {
            graph01.solve(f(i, j));
            REP(d, dx.size()) {
                int ni = i, nj = j;
                while (isin(ni, nj) and graph01.dist[f(ni, nj)] == abs(ni - i) + abs(nj - j)) {
                    bool breaker = false;
                    if (pint(ni, nj) == pint(ti, tj)) breaker = true;
                    ni += dx[d];
                    nj += dy[d];
                    if (breaker) break;
                }
                ni -= dx[d];
                nj -= dy[d];

                all_mv_graph.add_edge(f(i, j), f(ni, nj), graph01.dist[f(ni, nj)] + 20);
            }

        }
        all_mv_graph.solve(f(si, sj));
        vector<int> path;
        pint tgt;
        REP(dd, D * D) {
            REP(i, D) REP(j, D) {
                if (path.size()) break;
                if (abs(i - ti) + abs(j - tj) == dd and all_mv_graph.dist[f(i, j)] < 1 << 29) {
                    tgt = make_pair(i, j);
                    path = all_mv_graph.retrieve_path(f(i, j));
                    break;
                }
            }
        }
        dbg(tgt);
        dbg(path);

        int all_len = 0;
        FOR(i, 1, path.size()) {
            pint dv = finv(path[i]) - finv(path[i - 1]);
            int L = abs(dv.first) + abs(dv.second);
            all_len += L;
        }

        vector<pint> proc;
        FOR(i, 1, path.size()) {
            pint dv = finv(path[i]) - finv(path[i - 1]);
            int L = abs(dv.first) + abs(dv.second);
            int nuse = 190 * L / all_len;
            REP(d, mvstr.size()) {
                if (dv.first * dx[d] + dv.second * dy[d] > 0) {
                    proc.emplace_back(d, nuse);
                }
            }
        }
        dbg(proc);

        auto simul = [&](const vector<pint> &seq) -> Float {
            vector<Float> dp(D * D);
            dp[f(si, sj)] = 1;
            for (auto [dir, len] : seq) REP(t, len) dp = calc_mv(dp, dir);
            return dp[f(tgt.first, tgt.second)];
        };
        Float eproc = simul(proc);
        REP(iter, 500) {
            int s = rnd(proc.size()), t = rnd(proc.size());
            if (proc[s].second == 0) continue;
            proc[s].second--;
            proc[t].second++;
            if (chmax(eproc, simul(proc))) {
                dbg(iter);
            } else {
                proc[s].second++;
                proc[t].second--;
            }
        }

        string tmp;
        for (auto [d, len] : proc) tmp += string(len, mvstr[d]);
        return build_greedy(tmp);
    };

    string ret = build_greedy("");
    auto simul_dp = calc_str(ret);
    if (simul_dp.at(f(ti, tj)) < 0.5) {
        auto ret_heur = solve_heur();
        if (eval_score(ret_heur) > eval_score(ret)) ret = ret_heur;
    }
    ret = anneal(ret);
    dbg(eval_score(ret));


#ifdef BENCHMARK
    cout << eval_score(ret) << ' ' << p << ' ' << dmap[si][sj] << ' ' << iter << ' ' << ret << '\n';
#else
    cout << ret << '\n';
#endif
}
