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
    cerr << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") " << __FILE__ \
         << COLOR_RESET << endl
#define dbgif(cond, x)                                                                                                \
    ((cond) ? cerr << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") "   \
                   << __FILE__ << COLOR_RESET << endl                                                                 \
            : cerr)
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
using pint = pair<int, int>;
using plint = pair<lint, lint>;

struct fast_ios {
    fast_ios() { cin.tie(nullptr), ios::sync_with_stdio(false), cout << fixed << setprecision(20); };
} fast_ios_;

using Move = pint;  // (pos1, pos2)

#include <cstdint>

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
#include <algorithm>
#include <cassert>
#include <deque>
#include <fstream>
#include <functional>
#include <limits>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

template <typename T, T INF = std::numeric_limits<T>::max() / 2, int INVALID = -1>
struct shortest_path {
    int V, E;
    bool single_positive_weight;
    T wmin, wmax;

    std::vector<std::pair<int, T>> tos;
    std::vector<int> head;
    std::vector<std::tuple<int, int, T>> edges;

    void build_() {
        if (int(tos.size()) == E and int(head.size()) == V + 1) return;
        tos.resize(E);
        head.assign(V + 1, 0);
        for (const auto &e : edges) ++head[std::get<0>(e) + 1];
        for (int i = 0; i < V; ++i) head[i + 1] += head[i];
        auto cur = head;
        for (const auto &e : edges) {
            tos[cur[std::get<0>(e)]++] = std::make_pair(std::get<1>(e), std::get<2>(e));
        }
    }

    shortest_path(int V = 0) : V(V), E(0), single_positive_weight(true), wmin(0), wmax(0) {}
    void add_edge(int s, int t, T w) {
        assert(0 <= s and s < V);
        assert(0 <= t and t < V);
        edges.emplace_back(s, t, w);
        ++E;
        if (w > 0 and wmax > 0 and wmax != w) single_positive_weight = false;
        wmin = std::min(wmin, w);
        wmax = std::max(wmax, w);
    }

    void add_bi_edge(int u, int v, T w) {
        add_edge(u, v, w);
        add_edge(v, u, w);
    }

    std::vector<T> dist;
    std::vector<int> prev;

    using Pque = std::priority_queue<std::pair<T, int>, std::vector<std::pair<T, int>>,
                                     std::greater<std::pair<T, int>>>;
    template <class Heap = Pque> void dijkstra(int s, int t = INVALID) {
        assert(0 <= s and s < V);
        build_();
        dist.assign(V, INF);
        prev.assign(V, INVALID);
        dist[s] = 0;
        Heap pq;
        pq.emplace(0, s);
        while (!pq.empty()) {
            T d;
            int v;
            std::tie(d, v) = pq.top();
            pq.pop();
            if (t == v) return;
            if (dist[v] < d) continue;
            for (int e = head[v]; e < head[v + 1]; ++e) {
                const auto &nx = tos[e];
                T dnx = d + nx.second;
                if (dist[nx.first] > dnx) {
                    dist[nx.first] = dnx, prev[nx.first] = v;
                    pq.emplace(dnx, nx.first);
                }
            }
        }
    }

    void dijkstra_vquad(int s, int t = INVALID) {
        assert(0 <= s and s < V);
        build_();
        dist.assign(V, INF);
        prev.assign(V, INVALID);
        dist[s] = 0;
        std::vector<char> fixed(V, false);
        while (true) {
            int r = INVALID;
            T dr = INF;
            for (int i = 0; i < V; i++) {
                if (!fixed[i] and dist[i] < dr) r = i, dr = dist[i];
            }
            if (r == INVALID or r == t) break;
            fixed[r] = true;
            int nxt;
            T dx;
            for (int e = head[r]; e < head[r + 1]; ++e) {
                std::tie(nxt, dx) = tos[e];
                if (dist[nxt] > dist[r] + dx) dist[nxt] = dist[r] + dx, prev[nxt] = r;
            }
        }
    }

    bool bellman_ford(int s, int nb_loop) {
        assert(0 <= s and s < V);
        build_();
        dist.assign(V, INF), prev.assign(V, INVALID);
        dist[s] = 0;
        for (int l = 0; l < nb_loop; l++) {
            bool upd = false;
            for (int v = 0; v < V; v++) {
                if (dist[v] == INF) continue;
                for (int e = head[v]; e < head[v + 1]; ++e) {
                    const auto &nx = tos[e];
                    T dnx = dist[v] + nx.second;
                    if (dist[nx.first] > dnx) dist[nx.first] = dnx, prev[nx.first] = v, upd = true;
                }
            }
            if (!upd) return true;
        }
        return false;
    }

    void spfa(int s) {
        assert(0 <= s and s < V);
        build_();
        dist.assign(V, INF);
        prev.assign(V, INVALID);
        dist[s] = 0;
        std::deque<int> q;
        std::vector<char> in_queue(V);
        q.push_back(s), in_queue[s] = 1;
        while (!q.empty()) {
            int now = q.front();
            q.pop_front(), in_queue[now] = 0;
            for (int e = head[now]; e < head[now + 1]; ++e) {
                const auto &nx = tos[e];
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

    void zero_one_bfs(int s, int t = INVALID) {
        assert(0 <= s and s < V);
        build_();
        dist.assign(V, INF), prev.assign(V, INVALID);
        dist[s] = 0;
        std::vector<int> q(V * 4);
        int ql = V * 2, qr = V * 2;
        q[qr++] = s;
        while (ql < qr) {
            int v = q[ql++];
            if (v == t) return;
            for (int e = head[v]; e < head[v + 1]; ++e) {
                const auto &nx = tos[e];
                T dnx = dist[v] + nx.second;
                if (dist[nx.first] > dnx) {
                    dist[nx.first] = dnx, prev[nx.first] = v;
                    if (nx.second) {
                        q[qr++] = nx.first;
                    } else {
                        q[--ql] = nx.first;
                    }
                }
            }
        }
    }

    void dial(int s, int t = INVALID) {
        assert(0 <= s and s < V);
        build_();
        dist.assign(V, INF), prev.assign(V, INVALID);
        dist[s] = 0;
        std::vector<std::vector<std::pair<int, T>>> q(wmax + 1);
        q[0].emplace_back(s, dist[s]);
        int ninq = 1;

        int cur = 0;
        T dcur = 0;
        for (; ninq; ++cur, ++dcur) {
            if (cur == wmax + 1) cur = 0;
            while (!q[cur].empty()) {
                int v = q[cur].back().first;
                T dnow = q[cur].back().second;
                q[cur].pop_back(), --ninq;
                if (v == t) return;
                if (dist[v] < dnow) continue;

                for (int e = head[v]; e < head[v + 1]; ++e) {
                    const auto &nx = tos[e];
                    T dnx = dist[v] + nx.second;
                    if (dist[nx.first] > dnx) {
                        dist[nx.first] = dnx, prev[nx.first] = v;
                        int nxtcur = cur + int(nx.second);
                        if (nxtcur >= int(q.size())) nxtcur -= q.size();
                        q[nxtcur].emplace_back(nx.first, dnx), ++ninq;
                    }
                }
            }
        }
    }

    bool dag_solver(int s) {
        assert(0 <= s and s < V);
        build_();
        dist.assign(V, INF), prev.assign(V, INVALID);
        dist[s] = 0;
        std::vector<int> indeg(V, 0);
        std::vector<int> q(V * 2);
        int ql = 0, qr = 0;
        q[qr++] = s;
        while (ql < qr) {
            int now = q[ql++];
            for (int e = head[now]; e < head[now + 1]; ++e) {
                const auto &nx = tos[e];
                ++indeg[nx.first];
                if (indeg[nx.first] == 1) q[qr++] = nx.first;
            }
        }
        ql = qr = 0;
        q[qr++] = s;
        while (ql < qr) {
            int now = q[ql++];
            for (int e = head[now]; e < head[now + 1]; ++e) {
                const auto &nx = tos[e];
                --indeg[nx.first];
                if (dist[nx.first] > dist[now] + nx.second)
                    dist[nx.first] = dist[now] + nx.second, prev[nx.first] = now;
                if (indeg[nx.first] == 0) q[qr++] = nx.first;
            }
        }
        return *max_element(indeg.begin(), indeg.end()) == 0;
    }

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

    void solve(int s, int t = INVALID) {
        if (wmin >= 0) {
            if (single_positive_weight) {
                zero_one_bfs(s, t);
            } else if (wmax <= 10) {
                dial(s, t);
            } else {
                if ((long long)V * V < (E << 4)) {
                    dijkstra_vquad(s, t);
                } else {
                    dijkstra(s, t);
                }
            }
        } else {
            bellman_ford(s, V);
        }
    }

    std::vector<std::vector<T>> floyd_warshall() {
        build_();
        std::vector<std::vector<T>> dist2d(V, std::vector<T>(V, INF));
        for (int i = 0; i < V; i++) {
            dist2d[i][i] = 0;
            for (const auto &e : edges) {
                int s = std::get<0>(e), t = std::get<1>(e);
                dist2d[s][t] = std::min(dist2d[s][t], std::get<2>(e));
            }
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
        return dist2d;
    }

    void to_dot(std::string filename = "shortest_path") const {
        std::ofstream ss(filename + ".DOT");
        ss << "digraph{\n";
        build_();
        for (int i = 0; i < V; i++) {
            for (int e = head[i]; e < head[i + 1]; ++e) {
                ss << i << "->" << tos[e].first << "[label=" << tos[e].second << "];\n";
            }
        }
        ss << "}\n";
        ss.close();
        return;
    }
};

constexpr int N = 30;
constexpr int SZ = 465;

vector<int> pos2x, pos2y;
vector<int> xstarts;

string to_str(int pos) {
    assert(pos >= 0 and pos < int(pos2x.size()));
    return to_string(pos2x.at(pos)) + " " + to_string(pos2y.at(pos));
}

struct Solver {
    vector<int> pos2c;
    vector<int> c2pos;
    vector<int> pivots;
    vector<Move> moves;
    string message;

    void swap_pos(int p1, int p2) {
        moves.emplace_back(p1, p2);
        int c1 = pos2c.at(p1);
        int c2 = pos2c.at(p2);
        swap(pos2c.at(p1), pos2c.at(p2));
        swap(c2pos.at(c1), c2pos.at(c2));
    }

    Solver(const vector<int> &pos2c) : pos2c(pos2c) {
        c2pos.assign(SZ, -1);
        REP(i, pos2c.size()) c2pos.at(pos2c.at(i)) = i;
    }

    void run_base() {
        vector<int> is_fixed(SZ);
        REP(c, SZ) {
            while (true) {
                const int pos = c2pos.at(c);
                const int x = pos2x.at(pos), y = pos2y.at(pos);
                int cp1 = -1, cp2 = -1;
                if (x and y and !is_fixed.at(pos - x - 1)) cp1 = pos - x - 1;
                if (x and y < x and !is_fixed.at(pos - x)) cp2 = pos - x;
                if (cp1 < 0 and cp2 < 0) {
                    break;
                } else if (cp1 < 0) {
                    swap_pos(pos, cp2);
                } else if (cp2 < 0) {
                    swap_pos(pos, cp1);
                } else {
                    if (pos2c.at(cp1) < pos2c.at(cp2)) {
                        swap_pos(pos, cp2);
                    } else {
                        swap_pos(pos, cp1);
                    }
                }
            }
            is_fixed.at(c2pos.at(c)) = true;
        }
    }

    bool can_be_goal(const vector<int> &is_fixed, int pos) {
        const int x = pos2x.at(pos), y = pos2y.at(pos);
        if (x == 0) return true;
        if (x and y and !is_fixed.at(pos - x - 1)) return false;
        if (x and y < x and !is_fixed.at(pos - x)) return false;
        return true;
    };

    lint est_cost(lint W, const vector<double> &ex, int from, int to) {
        if (pos2x.at(from) == pos2x.at(to)) return W;
        lint tmp = W - pos2c.at(to);
        // if (pos2c.at(to) > ex.at(to)) {
        //     double ov = pos2c.at(to) - ex.at(to);
        //     tmp -= ov * ov * 0.0001;
        // }
        return tmp;
    }

    void run(const lint W, const vector<double> &ex, int c_start = 0, int c_perterb = -1, const vector<int> &guide = {}) {
        message += to_string(W);
        vector<int> is_fixed(SZ);

        FOR(c, c_start, SZ) {
            // shortest_path<lint> graph(SZ + 1);

            vector<lint> dists(SZ + 1, 1LL << 60);
            dists.at(c2pos.at(c)) = 0;
            vector<int> prvs(SZ + 1, -1);

            auto trans = [&](int from, int to, lint w) {
                if (chmin(dists.at(to), dists.at(from) + w)) prvs.at(to) = from;
            };

            auto retrieve = [&](int goal) {
                vector<int> ret;
                while (goal >= 0) ret.push_back(goal), goal = prvs.at(goal);
                std::reverse(ret.begin(), ret.end());
                return ret;
            };

            // REP(pos, SZ) {
            //     if (is_fixed.at(pos)) continue;
            //     if (can_be_goal(is_fixed, pos)) {
            //         // graph.add_edge(pos, SZ, 0);
            //         continue;
            //     }

            //     const int x = pos2x.at(pos), y = pos2y.at(pos);
            //     if (x and y and !is_fixed.at(pos - x - 1)) graph.add_edge(pos, pos - x - 1, est_cost(W, ex, pos, pos - x - 1));
            //     if (x and y < x and !is_fixed.at(pos - x)) graph.add_edge(pos, pos - x, est_cost(W, ex, pos, pos - x));
            //     if (y and !is_fixed.at(pos - 1)) graph.add_edge(pos, pos - 1, est_cost(W, ex, pos, pos - 1));
            //     if (y < x and !is_fixed.at(pos + 1)) graph.add_edge(pos, pos + 1, est_cost(W, ex, pos, pos + 1));
            // }

            IREP(x, N) {
                const int start = xstarts.at(x), end = xstarts.at(x + 1);
                FOR(i, start, end - 1) if (!is_fixed.at(i) and !is_fixed.at(i + 1) and !can_be_goal(is_fixed, i)) trans(i, i + 1, W);
                IFOR(i, start, end - 1) if (!is_fixed.at(i) and !is_fixed.at(i + 1) and !can_be_goal(is_fixed, i)) trans(i + 1, i, W);

                FOR(pos, start, end) {
                    if (is_fixed.at(pos)) continue;
                    if (can_be_goal(is_fixed, pos)) {
                        trans(pos, SZ, 0);
                        continue;
                    }
                    const int x = pos2x.at(pos), y = pos2y.at(pos);
                    if (x and y and !is_fixed.at(pos - x - 1)) trans(pos, pos - x - 1, est_cost(W, ex, pos, pos - x - 1));
                    if (x and y < x and !is_fixed.at(pos - x)) trans(pos, pos - x, est_cost(W, ex, pos, pos - x));
                    // if (y and !is_fixed.at(pos - 1)) trans(pos, pos - 1, est_cost(W, ex, pos, pos - 1));
                    // if (y < x and !is_fixed.at(pos + 1)) trans(pos, pos + 1, est_cost(W, ex, pos, pos + 1));

                }
            }

            vector<int> path;
            if (c == c_perterb) {
                // graph.solve(c2pos.at(c));
                vector<int> goals;
                REP(pos, SZ) {
                    if (can_be_goal(is_fixed, pos) and dists.at(pos) < 1 << 30) goals.push_back(pos);
                }
                sort(ALL(goals), [&](int p, int q) { return dists.at(p) < dists.at(q); });
                if (goals.size() > 1) goals.erase(goals.begin());
                int to = goals.at(goals.size() * rand_double() * rand_double());
                path = retrieve(to);
            } else {
                if (c < c_perterb and guide.size()) {
                    // graph.solve(c2pos.at(c), guide.at(c));
                    path = retrieve(guide.at(c));
                } else {
                    // graph.solve(c2pos.at(c), SZ);
                    path = retrieve(SZ);
                    path.pop_back();
                }
            }
            pivots.emplace_back(path.back());
            FOR(i, 1, path.size()) swap_pos(path.at(i - 1), path.at(i));
            is_fixed.at(c2pos.at(c)) = true;
        }
    }

    // 軽く評価関数を回して毎回ちゃんと探索する
    void heavy_run(lint W, const vector<double> &ex) {
        vector<int> is_fixed(SZ);

        REP(c, SZ) {
            shortest_path<lint> graph(SZ);

            vector<int> cands;
            REP(pos, SZ) {
                if (is_fixed.at(pos)) continue;
                if (can_be_goal(is_fixed, pos)) {
                    cands.push_back(pos);
                    continue;
                }

                const int x = pos2x.at(pos), y = pos2y.at(pos);
                if (x and y and !is_fixed.at(pos - x - 1)) graph.add_edge(pos, pos - x - 1, est_cost(W, ex, pos, pos - x - 1));
                if (x and y < x and !is_fixed.at(pos - x)) graph.add_edge(pos, pos - x, est_cost(W, ex, pos, pos - x));
                if (y and !is_fixed.at(pos - 1)) graph.add_edge(pos, pos - 1, est_cost(W, ex, pos, pos - 1));
                if (y < x and !is_fixed.at(pos + 1)) graph.add_edge(pos, pos + 1, est_cost(W, ex, pos, pos + 1));
            }
            dbg(make_tuple(c, cands.size()));

            graph.solve(c2pos.at(c));
            sort(cands.begin(), cands.end(), [&](int p, int q) { return graph.dist.at(p) < graph.dist.at(q); });
            while (cands.size() > 2) cands.pop_back();

            // if (c % 5) cands.resize(1);

            int best = 1 << 30, argbest = -1;
            if (cands.size() == 1) {
                argbest = cands.front();
            } else {
                for (int cand_pos : cands) {
                    if (graph.dist.at(cand_pos) > 1 << 30) continue;

                    auto path = graph.retrieve_path(cand_pos);
                    Solver tmp(*this);
                    FOR(i, 1, path.size()) tmp.swap_pos(path.at(i - 1), path.at(i));
                    tmp.run(W, ex, c + 1);
                    if(chmin<int>(best, tmp.moves.size())) argbest = cand_pos;
                }
            }
            auto path = graph.retrieve_path(argbest);
            FOR(i, 1, path.size()) swap_pos(path.at(i - 1), path.at(i));
            is_fixed.at(c2pos.at(c)) = true;
        }
    }

    void run_rev() {
        vector<int> is_fixed(SZ);
        IREP(c, SZ) {
            while (true) {
                const int pos = c2pos.at(c);
                const int x = pos2x.at(pos);
                int cp1 = -1, cp2 = -1;
                if (x + 1 < N and !is_fixed.at(pos + x + 1)) cp1 = pos + x + 1;
                if (x + 1 < N and !is_fixed.at(pos + x + 2)) cp2 = pos + x + 2;
                if (cp1 < 0 and cp2 < 0) {
                    break;
                } else if (cp1 < 0) {
                    swap_pos(pos, cp2);
                } else if (cp2 < 0) {
                    swap_pos(pos, cp1);
                } else {
                    if (pos2c.at(cp1) > pos2c.at(cp2)) {
                        swap_pos(pos, cp2);
                    } else {
                        swap_pos(pos, cp1);
                    }
                }
            }
            is_fixed.at(c2pos.at(c)) = true;
        }
    }

    string dump() {
        assert(moves.size() <= 10000);
        string ret;
        // REP(pos, SZ) {
        //     const int x = pos2x.at(pos), y = pos2y.at(pos);
        //     if (x + 1 == N) continue;
        //     assert(pos2c.at(pos) < pos2c.at(pos + x + 1));
        //     assert(pos2c.at(pos) < pos2c.at(pos + x + 2));
        // }

        ret += to_string(moves.size()) + " ";
        for (auto [p1, p2] : moves) ret += to_str(p1) + " " + to_str(p2) + " ";
        if (ret.size() and ret.back() == ' ') ret.pop_back();
        return ret;
    }
};

struct rand_int_ {
    using lint = long long;
    std::mt19937 mt;
    // rand_int_() : mt(std::chrono::steady_clock::now().time_since_epoch().count()) {}
    rand_int_() : mt(42) {}
    lint operator()(lint x) { return this->operator()(0, x); } // [0, x)
    lint operator()(lint l, lint r) {
        std::uniform_int_distribution<lint> d(l, r - 1);
        return d(mt);
    }
} rnd;


#include <vector>

const std::vector<double> ex = {
    0,       2.77,    2.54,    6.115,   9.885,   5.62,    9.15,    18.545,  18.085,  8.23,    12.12,   26.59,
    31.595,  26.245,  10.76,   14.82,   33.93,   45.115,  45.23,   33.415,  13.12,   17.125,  39.865,  57.415,
    62.515,  55.165,  39.095,  15.395,  19.71,   45.055,  67.25,   79.84,   76.175,  65.48,   44.16,   17.725,
    22.005,  50.655,  74.415,  92.465,  97.58,   88.295,  72.65,   48.46,   20.18,   24.37,   55.38,   81.415,
    101.44,  114.565, 112.225, 98.24,   79.08,   52.85,   22.795,  27.17,   59.585,  87.965,  109.74,  127.44,
    132.445, 123,     107.545, 84.885,  57.9,    25.605,  30.22,   63.75,   93.755,  118.045, 138.845, 147.55,
    146.735, 132.995, 115.025, 91.1,    63.01,   28.865,  33.355,  68.655,  99.13,   125.715, 146.975, 161.435,
    164.365, 158.45,  142.3,   121.915, 98.1,    67.96,   31.94,   36.745,  73.985,  105.365, 133.09,  155.455,
    171.495, 180.65,  177.52,  169.08,  150.73,  129.58,  105.03,  73.085,  35.3,    40.205,  78.93,   112.055,
    140.01,  164.045, 181.635, 193.96,  197.3,   190.155, 178.155, 159.05,  137.475, 111.365, 78.425,  39.01,
    44.09,   84.69,   119.485, 148.865, 172.46,  192.66,  206.285, 212.48,  211.495, 202.36,  188.15,  168.005,
    145.94,  119.27,  84.805,  43.085,  48.17,   91.5,    126.95,  156.615, 181.27,  201.95,  217.115, 226.835,
    228.39,  224.84,  214.895, 198.295, 177.425, 153.775, 126.87,  92.175,  47.505,  53.265,  98.53,   135.515,
    166.11,  191.1,   212.535, 228.625, 239.865, 244.675, 243.665, 237.485, 225.105, 208.285, 187.65,  164.295,
    136.22,  100.42,  52.53,   58.855,  106.4,   144.25,  175.515, 200.935, 223.42,  240.78,  253.79,  259.39,
    260.215, 259.115, 249.285, 236.955, 220.06,  197.745, 175.23,  145.445, 108.51,  58.34,   65.49,   115.495,
    154.025, 186.94,  212.02,  233.88,  252.83,  265.91,  274.435, 277.87,  275.755, 271.2,   261.36,  248.765,
    231.675, 210.195, 186.455, 154.83,  117.745, 64.265,  73.12,   125.24,  166.465, 199.205, 224.92,  247.18,
    265.105, 277.53,  287.83,  295.02,  295,     290.81,  285.475, 274.805, 260.665, 242.895, 222.225, 198.575,
    166.725, 127.805, 71.515,  82.195,  137.09,  179.2,   213.845, 240.905, 260.575, 278.865, 291.44,  301.35,
    308.86,  311.93,  309.625, 305.355, 299.065, 287.97,  274.45,  256.605, 237.925, 211.99,  180.975, 139.945,
    80.37,   92.45,   152.495, 194.6,   229.41,  255.335, 276.515, 293.575, 306.595, 316.885, 323.165, 326.62,
    328.705, 323.455, 319.565, 312.125, 301.45,  288.185, 272.445, 253.71,  228.73,  196.74,  153.76,  91,
    107.12,  169.095, 213.93,  246.675, 272.85,  292.76,  309.895, 321.355, 331.275, 338.85,  343.51,  345.02,
    344.01,  338.85,  335.415, 327.76,  316.335, 305.005, 288.755, 271.99,  246.405, 216.505, 173.135, 105,
    124.445, 189.485, 234.53,  267.085, 293.88,  311.915, 326.52,  339.285, 347.76,  354.31,  359.455, 361.825,
    362.785, 360.335, 354.955, 351.69,  344.67,  336.02,  322.62,  308.7,   294.27,  267.81,  238.305, 193.61,
    124.165, 146.56,  216.795, 259.17,  288.97,  315.925, 334.135, 346.995, 357.445, 365.675, 372.3,   375.73,
    379.85,  380.71,  379.965, 377.33,  373.03,  369.295, 363.08,  356.06,  342.655, 329.03,  317.245, 294.205,
    263.36,  218.065, 147.66,  174.75,  250.87,  293.495, 320.51,  341.92,  357.535, 367.24,  377.295, 384.865,
    389.79,  394.605, 396.245, 397.35,  399.425, 397.145, 396.56,  392.29,  388.59,  382.08,  376.805, 365.595,
    354.955, 343.035, 322.075, 294.09,  252.735, 179.935, 221.175, 295.705, 331.66,  355.52,  372.235, 384.71,
    391.815, 399.185, 405.645, 411.925, 412.225, 413.785, 415.685, 416.885, 417.175, 415.905, 414.05,  411.265,
    407.935, 405.92,  399.335, 391.88,  382.17,  370.895, 353.36,  329.915, 296.99,  224.87,  283.48,  349.28,
    376.42,  393.14,  403.88,  413.21,  419.44,  422.695, 427.42,  430.99,  432.13,  434.065, 434.21,  435.53,
    435.665, 434.895, 434.415, 435.305, 429.915, 429.88,  427.815, 424.77,  417.47,  410.7,   404.08,  393.955,
    373.39,  347.64,  285.165, 367.565, 413.34,  425.8,   433.63,  436.305, 441.16,  445.905, 448.545, 450.155,
    450.845, 452.02,  450.955, 452.445, 453.025, 451.575, 452.81,  451.63,  452.395, 450.75,  451.38,  450.35,
    447.9,   445.85,  444.295, 444.03,  436.57,  432.025, 423.27,  405.035, 369.34};

vector<double> statistics() {
    vector<double> e(SZ);
    const int niter = 200;
    REP(_, niter) {
        vector<int> A(SZ);
        REP(i, SZ) A.at(i) = i;
        shuffle(A.begin(), A.end(), rnd.mt);
        Solver sol(A);
        sol.run(380, ex);
        REP(p, SZ) e.at(p) += sol.pos2c.at(p);
    }
    REP(p, SZ) e.at(p) /= niter;
    return e;
}

int main(int argc, char *argv[]) {
    auto START = std::chrono::system_clock::now();
    

    pos2x.reserve(SZ), pos2y.reserve(SZ);
    REP(x, N) {
        xstarts.push_back(pos2x.size());
        REP(y, x + 1) pos2x.emplace_back(x), pos2y.emplace_back(y);
    }
    xstarts.push_back(SZ);

    int X = 0;
    if (argc >= 2) {
        string s = argv[1];
        s.erase(find(s.begin(), s.end(), '.'), s.end());
        X = std::stoi(s);
    }

    vector<int> A(SZ);
    cin >> A;

    vector<Solver> solutions;

    // Solver solver(A);
    // solver.run(SZ, ex);
    // solutions.push_back(solver);

    auto sample_w = [&]() -> int {
        // int mean = 430;
        return std::normal_distribution<>(420, 50)(rnd.mt);
        // int div = 100 * rand_double() * rand_double();
        // return mean + div * (rand_int() % 2 ? 1 : -1);
    };
    // REP(_, 10) dbg(sample_w());
    while (true) {
        int64_t spent_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - START).count();
        if (spent_ms > 1900) break;
        int w = sample_w();
        Solver s(A);
        // dbg(w);
        s.run(w, ex);
        solutions.push_back(s);

        vector<int> cs;
        REP(_, 16) cs.push_back(rand_int() % SZ);
        cs = sort_unique(cs);
        for (int cp : cs) {
            Solver sp(A);
            sp.run(w, ex, 0, cp, s.pivots);
            if (sp.moves.size() < s.moves.size()) s = sp;
            solutions.push_back(s);
        }
    }

    // for (int w = 325; w <= 500; w += 5) {
    //     Solver s(A);
    //     // dbg(w);
    //     s.run(w, ex);
    //     solutions.push_back(s);

    //     vector<int> cs;
    //     REP(_, 16) cs.push_back(rand_int() % SZ);
    //     cs = sort_unique(cs);
    //     for (int cp : cs) {
    //         Solver sp(A);
    //         sp.run(w, ex, 0, cp, s.pivots);
    //         if (sp.moves.size() < s.moves.size()) s = sp;
    //         solutions.push_back(s);
    //     }
    // }

    {
        Solver best = solutions.front();
        for (auto &s : solutions) {
            if (best.moves.size() > s.moves.size()) best = s;
        }
        dump_onlinejudge(best.dump());
        jdump("len", best.moves.size());
        jdump("message", best.message);
        jdump("score", 100000 - 5 * best.moves.size());
        dbg(best.moves.size());

        if (X <= 20) {
            jdump("sol", best.dump());
            // jdump("base_sol", base_solver.dump());
        }
    }
}
