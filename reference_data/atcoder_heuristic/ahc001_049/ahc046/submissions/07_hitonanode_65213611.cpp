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
#include <array>
#include <cstdlib>

using namespace std;
using lint = long long;
using pint = std::pair<int, int>;
using plint = std::pair<lint, lint>;

struct fast_ios {
    fast_ios() {
        std::cin.tie(nullptr), std::ios::sync_with_stdio(false), std::cout << std::fixed << std::setprecision(20);
    };
} fast_ios_;


#include <array>
#include <bitset>
#include <vector>

/* ★ 問題は N = 20,  M = 40 固定 ★ */
constexpr int N = 20;
constexpr int M = 40;

/* 座標 → 1 次元 id への簡易射影 */
using Point = int;
inline Point f(int x, int y) { return x * N + y; }
inline bool isin(int x, int y) { return 0 <= x and x < N and 0 <= y and y < N; }

const std::array<int, 4> dx{1, -1, 0, 0};
const std::array<int, 4> dy{0, 0, 1, -1};

using AlterPlan = std::vector<Point>;
using AlterPlans = std::array<AlterPlan, M>;

using BanStates = std::bitset<N * N>;

#include <queue>

/* 方向テーブル */
const int DX[4] = {-1, 1, 0, 0};
const int DY[4] = {0, 0, -1, 1};
const char DIRC[4] = {'U', 'D', 'L', 'R'};

/* -------------------------------------------------------------------------- */
/* 1 区間 (start → goal,  AlterPlan = plan) を最短で組み立てる BFS            */
/* -------------------------------------------------------------------------- */
namespace detail {

struct Prev {
    int prev;
    char act, dir;
};

std::vector<std::pair<char, char>>
bfs_segment(Point start, Point goal, const AlterPlan &plan, const std::array<std::array<uint8_t, N>, N> &board0) {
    using namespace std;
    const int sx = start / N, sy = start % N;
    const int gx = goal / N, gy = goal % N;
    const int K = plan.size();

    /* ---- 盤面スナップショット (0 … K 回目の Alter 終了時) ---- */
    vector<array<array<uint8_t, N>, N>> snap(K + 1);
    snap[0] = board0;
    for (int i = 0; i < K; ++i) {
        snap[i + 1] = snap[i];
        int x = plan[i] / N, y = plan[i] % N;
        snap[i + 1][x][y] ^= 1; // toggle
    }

    const int SZ = (K + 1) * N * N; // 全状態数
    auto idx = [&](int x, int y, int s) { return s * N * N + x * N + y; };

    vector<char> seen(SZ, 0);
    vector<Prev> prv(SZ);
    queue<tuple<int, int, int>> que;

    int sidx = idx(sx, sy, 0);
    seen[sidx] = 1;
    que.emplace(sx, sy, 0);

    int gidx = -1;

    while (!que.empty()) {
        auto [x, y, s] = que.front();
        que.pop();

        if (x == gx && y == gy && s == K) { // 目的地＋Alter 完了
            gidx = idx(x, y, s);
            break;
        }

        /* ---- Move ---- */
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (!isin(nx, ny) || snap[s][nx][ny]) continue;
            int ni = idx(nx, ny, s);
            if (!seen[ni]) {
                seen[ni] = 1;
                prv[ni] = {idx(x, y, s), 'M', DIRC[d]};
                que.emplace(nx, ny, s);
            }
        }

        /* ---- Slide ---- */
        for (int d = 0; d < 4; ++d) {
            int nx = x, ny = y;
            while (true) {
                int tx = nx + DX[d], ty = ny + DY[d];
                if (!isin(tx, ty) || snap[s][tx][ty]) break;
                nx = tx;
                ny = ty;
            }
            if (nx == x && ny == y) continue; // 動けなかった
            int ni = idx(nx, ny, s);
            if (!seen[ni]) {
                seen[ni] = 1;
                prv[ni] = {idx(x, y, s), 'S', DIRC[d]};
                que.emplace(nx, ny, s);
            }
        }

        /* ---- Alter (順番厳守) ---- */
        if (s < K) {
            int ax = plan[s] / N, ay = plan[s] % N;
            for (int d = 0; d < 4; ++d) {
                if (x + DX[d] == ax && y + DY[d] == ay) {
                    int ni = idx(x, y, s + 1);
                    if (!seen[ni]) {
                        seen[ni] = 1;
                        prv[ni] = {idx(x, y, s), 'A', DIRC[d]};
                        que.emplace(x, y, s + 1);
                    }
                    break; // 隣接は高々 1 方向
                }
            }
        }
    }

    /* ---- 経路復元 ---- */
    vector<pair<char, char>> ret;
    if (gidx == -1) return ret; // 到達不可能 (設計ミス)

    for (int cur = gidx; cur != sidx;) {
        auto p = prv[cur];
        ret.emplace_back(p.act, p.dir);
        cur = p.prev;
    }
    reverse(ret.begin(), ret.end());
    return ret;
}

} // namespace detail

/* -------------------------------------------------------------------------- */
/* 要求関数：全区間まとめて文字列 (動作列) を生成                              */
/* -------------------------------------------------------------------------- */
std::vector<std::pair<char, char>> RetrieveAll(const std::array<Point, M> &points, const AlterPlans &plans) {
    using namespace std;
    vector<pair<char, char>> answer;

    /* 現在の盤面 (true = block) ―― 区間をまたいで持ち越し */
    array<array<uint8_t, N>, N> board{};
    for (auto &row : board) row.fill(0);

    for (int i = 1; i < M; ++i) {
        /* 区間 i-1 → i を最短で組み立てる */
        auto seg = detail::bfs_segment(points[i - 1], points[i], plans[i], board);
        answer.insert(answer.end(), seg.begin(), seg.end());

        /* 盤面を実際に更新 (トグル適用) */
        for (Point p : plans[i]) {
            int x = p / N, y = p % N;
            board[x][y] ^= 1;
        }
    }
    return answer; // vector<pair<action, dir>>
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

constexpr int inf = 1e7;
int EvaluateSteps(Point s, Point t, const AlterPlan &plan, BanStates ban) {
    std::array<int, N * N> dp, dp_next;

    if (ban.test(s)) return inf;
    if (ban.test(t)) return inf;

    dp.fill(inf);
    dp.at(s) = 0;

    auto is_ok = [&](int x, int y) {
        return isin(x, y) and !ban.test(f(x, y));
    };

    std::vector<Point> st{s};
    int left = 0;

    auto func = [&](Point p, bool neighbor) {
        const int px = p / N, py = p % N;

        int rem = 1;

        if (neighbor) {
            rem = 0;
            REP(dir, 4) {
                const int nx = px + dx[dir], ny = py + dy[dir];
                if (is_ok(nx, ny)) ++rem;
            }
        }

        while (left < (int)st.size()) {
            const Point now = st.at(left++);
            const int x = now / N, y = now % N;

            if (!neighbor and now == p) break;
            if (neighbor and abs(x - px) + abs(y - py) == 1) {
                --rem;
                if (rem <= 0) break;
            }

            REP(dir, 4) {
                int nx = x + dx[dir], ny = y + dy[dir];
                if (!is_ok(nx, ny)) continue;

                if (const int np = f(nx, ny); chmin(dp.at(np), dp.at(now) + 1)) {
                    st.push_back(np);
                }

                while (is_ok(nx + dx[dir], ny + dy[dir])) nx += dx[dir], ny += dy[dir];
                if (const int np = f(nx, ny); chmin(dp.at(np), dp.at(now) + 1)) {
                    st.push_back(np);
                }
            }
        }

        dp_next.fill(inf);
        if (neighbor) {
            const int x = p / N, y = p % N;
            st.clear();
            left = 0;

            REP(dir, 4) {
                const int nx = x + dx[dir];
                const int ny = y + dy[dir];
                const int np = f(nx, ny);
                if (is_ok(nx, ny) and chmin(dp_next.at(np), dp.at(np) + 1)) st.push_back(np);
            }
        } else {
            if (is_ok(px, py) and chmin(dp_next.at(p), dp.at(p))) {
                st.push_back(p);
            }
        }

        swap(dp, dp_next);
    };

    for (Point p : plan) {
        func(p, true);
        ban.set(p);
    }

    func(t, false);
    return dp.at(t);
}

std::array<int, N * N> ForwardDP(Point s, const BanStates &ban) {

    auto is_ok = [&](int x, int y) { return isin(x, y) and !ban.test(f(x, y)); };

    std::array<int, N * N> ret;
    ret.fill(inf);
    
    std::array<Point, N * N> st;
    int l = 0, r = 0;
    if (is_ok(s / N, s % N)) {
        ret.at(s) = 0;
        st[r++] = s;
    }

    ret.at(s) = 0;
    st[0] = s;
    while (l < r) {
        const Point now = st.at(l++);
        const int x = now / N, y = now % N;
        REP(dir, 4) {
            int nx = x + dx[dir], ny = y + dy[dir];
            if (!is_ok(nx, ny)) continue;
            if (chmin(ret.at(f(nx, ny)), ret.at(now) + 1)) { st[r++] = f(nx, ny); }

            while (is_ok(nx + dx[dir], ny + dy[dir])) {
                nx += dx[dir];
                ny += dy[dir];
            }

            if (chmin(ret.at(f(nx, ny)), ret.at(now) + 1)) { st[r++] = f(nx, ny); }
        }
    }

    return ret;
}

std::array<int, N * N> BackwardDP(Point t, const BanStates &ban) {
    auto is_ok = [&](int x, int y) { return isin(x, y) and !ban.test(f(x, y)); };

    std::array<int, N * N> ret;
    ret.fill(inf);
    std::array<Point, N * N> st;
    int l = 0, r = 0;
    if (is_ok(t / N, t % N)) {
        st[r++] = t;
        ret.at(t) = 0;
    }

    while (l < r) {
        const Point now = st.at(l++);
        const int x = now / N, y = now % N;
        REP(dir, 4) {
            int nx = x + dx[dir], ny = y + dy[dir];
            if (!is_ok(nx, ny)) continue;
            if (chmin(ret.at(f(nx, ny)), ret.at(now) + 1)) st[r++] = f(nx, ny);

            if (!is_ok(x + dx[dir ^ 1], y + dy[dir ^ 1])) {
                while (is_ok(nx, ny)) {
                    if (chmin(ret.at(f(nx, ny)), ret.at(now) + 1)) st[r++] = f(nx, ny);
                    nx += dx[dir];
                    ny += dy[dir];
                }
            }
        }
    }

    return ret;
}

std::array<int, N * N> GetReductionIf(Point s, Point t, const AlterPlan &plan, BanStates ban) {
    std::array<int, N * N> ret;
    ret.fill(0);

    Point now = s;

    auto eval = [&](Point nxt) {
        const auto forward = ForwardDP(now, ban);
        const auto backward = BackwardDP(nxt, ban);
        if (forward.at(nxt) != backward.at(now)) {
            dbg(make_tuple(s, t, plan, ban, forward.at(nxt), backward.at(now)));
        }
        assert(forward.at(nxt) == backward.at(now));

        const int base_cost = forward.at(nxt);


        REP(i, N * N) {
            if (ban.test(i)) continue;
            const int x = i / N, y = i % N;
            int best = 0;
            REP(dir, 4) {
                const int x1 = x + dx[dir], y1 = y + dy[dir];
                if (!isin(x1, y1)) continue;
                if (ban.test(f(x1, y1))) continue;

                int x2 = x1 + dx[dir], y2 = y1 + dy[dir];
                while (isin(x2, y2) and !ban.test(f(x2, y2))) {
                    int cost = forward.at(f(x2, y2)) + 1 + backward.at(f(x1, y1));
                    chmax(best, base_cost - cost);
                    x2 += dx[dir];
                    y2 += dy[dir];
                }
            }
            ret.at(i) += best;
        }
    };

    for (auto nxt : plan) {
        eval(nxt);
        ban.set(nxt);
    }
    eval(t);

    REP(i, N * N) {
        if (ban.test(i)) ret.at(i) = -inf;
    }
    ret.at(t) = -inf;

    return ret;
}

int FastEvaluateAll(std::array<Point, M> points, AlterPlans &plans) {

    int ret = 0;
    BanStates ban;
    FOR(i, 1, M) {
        ret += EvaluateSteps(points.at(i - 1), points.at(i), plans.at(i), ban);
        for (auto p : plans.at(i)) {
            ban.set(p);
        }
    }

    return ret;
}

void tspimprove(std::array<Point, M> points, AlterPlans &state) {
    BanStates ban;
    FOR(tick, 1, M) {
        auto &vec = state.at(tick);
        int eval = EvaluateSteps(points.at(tick - 1), points.at(tick), vec, ban);
        if (vec.size() > 1) {
            FOR(i, 1, vec.size()) {
                swap(vec.at(i - 1), vec.at(i));
                if (chmin(eval, EvaluateSteps(points.at(tick - 1), points.at(tick), vec, ban))) {
                } else {
                    swap(vec.at(i - 1), vec.at(i));
                }
            }
        }
        for (auto i : state.at(tick)) ban.set(i);
    }

    int eval = FastEvaluateAll(points, state);

    FOR(tick, 1, M - 1) {
        if (state.at(tick).empty()) continue;
        Point v = state.at(tick).back();
        state.at(tick).pop_back();
        state.at(tick + 1).insert(state.at(tick + 1).begin(), v);
        if (chmin(eval, FastEvaluateAll(points, state))) {

        } else {
            state.at(tick).push_back(v);
            state.at(tick + 1).erase(state.at(tick + 1).begin());
        }
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


int main(int argc, char *argv[]) {
    {
        int N_, M_;
        cin >> N_ >> M_;
        assert(N_ == N and M_ == M);
    }
    array<int, M> xs, ys;
    REP(i, M) cin >> xs.at(i) >> ys.at(i);
    dbg(xs);
    dbg(ys);

    array<Point, M> points;
    REP(i, M) points.at(i) = f(xs.at(i), ys.at(i));
    dbg(points);

    auto greedy_insert = [&](AlterPlans state) -> pair<int, AlterPlans> {
        vector<tuple<int, int, int, Point>> cands;

        BanStates ban;
        FOR(t, 1, M) for (int i : state.at(t)) ban.set(i);

        std::array<int, N * N> gri_sum;
        gri_sum.fill(0);

        IFOR(t, 1, M) {

            IREP(d, state.at(t).size() + 1) {
                const Point s = d ? state.at(t).at(d - 1) : points.at(t - 1);
                const Point g = d < (int)state.at(t).size() ? state.at(t).at(d) : points.at(t);

                const auto backward = BackwardDP(g, ban);
                if (d) ban.reset(state.at(t).at(d - 1));
                const auto forward = ForwardDP(s, ban);
                const int base_cost = backward.at(s);
                if (base_cost >= inf / 2) continue;
                REP(i, N * N) {
                    if (gri_sum.at(i) < 0) continue;
                    int max_reward = -1;

                    REP(dir, 4) {
                        const int x = i / N + dx[dir];
                        const int y = i % N + dy[dir];
                        if (!isin(x, y) or ban.test(f(x, y))) continue;
                        const int cost_inc = forward.at(f(x, y)) + 1 + backward.at(f(x, y)) - base_cost;
                        chmax(max_reward, gri_sum.at(i) - cost_inc);
                    }

                    if (max_reward > 0) {
                        cands.emplace_back(-max_reward, t, d, i);
                    }
                }
            }

            const auto gri = GetReductionIf(points.at(t - 1), points.at(t), state.at(t), ban);
            REP(i, N * N) gri_sum.at(i) += gri.at(i);
        }

        sort(cands.begin(), cands.end());
        while (cands.size() > 100) cands.pop_back();
        // dbg(cands);
        int best_e = inf;
        int best_t = -1, best_d = -1, best_idx = -1;
        for (auto [_, t, d, idx] : cands) {
            state.at(t).insert(state.at(t).begin() + d, idx);
            const int e = FastEvaluateAll(points, state);
            state.at(t).erase(state.at(t).begin() + d);
            if (chmin(best_e, e)) {
                best_t = t;
                best_d = d;
                best_idx = idx;
                // dbg(make_tuple(best_e, best_t, best_d, best_idx));
            }
        }

        if (best_e < inf) {
            state.at(best_t).insert(state.at(best_t).begin() + best_d, best_idx);
            return {best_e, state};
        } else {
            return {inf, state};
        }
    };

    auto greedy_delete = [&](AlterPlans state) -> pair<int, AlterPlans> {
        int best_e = inf;
        int best_t = -1, best_d = -1;
        FOR(t, 1, M) {
            REP(d, state.at(t).size()) {
                const int v = state.at(t).at(d);
                state.at(t).erase(state.at(t).begin() + d);
                if (chmin(best_e, FastEvaluateAll(points, state))) {
                    best_t = t;
                    best_d = d;
                }
                state.at(t).insert(state.at(t).begin() + d, v);
            }
        }

        if (best_e < inf) {
            state.at(best_t).erase(state.at(best_t).begin() + best_d);
            return {best_e, state};
        } else {
            return {inf, state};
        }
    };

    auto try_move = [&](AlterPlans state) -> pair<int, AlterPlans> {
        int best_e = inf;
        AlterPlans best_state = state;
        FOR(t, 1, M) {
            auto &v = state.at(t);
            REP(d, v.size()) {
                const Point s = v.at(d);
                REP(dir, 4) {
                    const int x = s / N + dx[dir];
                    const int y = s % N + dy[dir];
                    if (!isin(x, y)) continue;
                    v.at(d) = f(x, y);
                    if (chmin(best_e, FastEvaluateAll(points, state))) { best_state = state; }
                    v.at(d) = s;
                }
            }
        }

        if (best_e < inf) {
            return {best_e, best_state};
        } else {
            return {inf, state};
        }
    };

    AlterPlans state;
    int opt = FastEvaluateAll(points, state);

    while (true) {
        auto [e, new_state] = greedy_insert(state);
        if (e < opt) {
            opt = e;
            state = new_state;
            dbg(make_tuple(opt, e));
        } else {
            break;
        }
    }

    while (true) {
        auto [e, new_state] = greedy_delete(state);
        if (chmin(opt, e)) {
            opt = e;
            state = new_state;
            dbg(make_tuple(-2, opt, e));
        } else {
            auto [e, newnew_state] = greedy_insert(new_state);
            if (chmin(opt, e)) {
                opt = e;
                state = newnew_state;
                dbg(make_tuple(-3, opt, e));
            } else {
                break;
            }
        }
    }

    REP(_, 2) {
        FOR(tick, 1, M) {
            for (int d = 0; d < (int)state.at(tick).size(); d++) {
                if (timer.spent_ms() > 1600) break;
                auto nxt_state = state;
                nxt_state.at(tick).erase(nxt_state.at(tick).begin() + d);
                auto [e, new_state] = greedy_insert(nxt_state);
                if (chmin(opt, e)) {
                    opt = e;
                    state = new_state;
                    dbg(make_tuple(-5, tick, d, opt));
                }
            }
        }
    }

    REP(_, 0) {
        FOR(tick, 1, M) {
            if (state.at(tick).size()) {
                auto tmp = state;
                tmp.at(tick).clear();
                auto eval = FastEvaluateAll(points, state);
                while (true) {
                    auto [e, new_state] = greedy_insert(tmp);
                    tspimprove(points, new_state);
                    if (chmin(eval, e)) {
                        tmp = new_state;
                    } else {
                        break;
                    }
                }

                if (chmin(opt, eval)) {
                    dbg(make_tuple(_, tick, opt));
                    state = tmp;
                }
            }
        }
    }

    // REP(_, 10) {
    //     auto [e, new_state] = try_move(state);
    //     if (e < opt) {
    //         opt = e;
    //         state = new_state;
    //         dbg(make_tuple(choice, opt));
    //     }
    // }

    int loop = 0;
    while (timer.spent_ms() < 1900) {
        ++loop;
        const int choice = rand_int() % 3;
        if (choice == 0) {
            auto [e, new_state] = greedy_insert(state);
            tspimprove(points, new_state);
            e = FastEvaluateAll(points, new_state);
            if (e < opt) {
                opt = e;
                state = new_state;
                dbg(make_tuple(choice, opt));
            }
        } else if (choice == 1) {
            auto [e, new_state] = greedy_delete(state);
            tspimprove(points, new_state);
            e = FastEvaluateAll(points, new_state);
            if (e < opt) {
                opt = e;
                state = new_state;
                dbg(make_tuple(choice, opt));
            }
        } else {
            auto [e, new_state] = try_move(state);
            if (e < opt) {
                opt = e;
                state = new_state;
                dbg(make_tuple(choice, opt));
            }
        }
    }
    cerr << loop << '\n';

    {
        std::string retstr;
        int next_target = 1;
        int x = xs.at(0), y = ys.at(0);
        vector is_blocked(N, vector<int>(N));

        int cost = 0;

        dbg(state);

        for (auto [a, d] : RetrieveAll(points, state)) {
            ++cost;
            // dbg(make_tuple(a, d, x, y));
            retstr += a;
            retstr += ' ';
            retstr += d;
            retstr += '\n';

            int dx = 0, dy = 0;
            if (d == 'U') {
                dx = -1;
            } else if (d == 'D') {
                dx = 1;
            } else if (d == 'L') {
                dy = -1;
            } else if (d == 'R') {
                dy = 1;
            } else {
                dbg(d);
                assert(false);
            }

            if (a == 'M') {
                const int nx = x + dx, ny = y + dy;
                assert(isin(nx, ny));
                assert(!is_blocked.at(nx).at(ny));
                x = nx;
                y = ny;
            } else if (a == 'A') {
                const int nx = x + dx, ny = y + dy;
                assert(isin(nx, ny));
                is_blocked.at(nx).at(ny) ^= 1;
            } else if (a == 'S') {
                while (isin(x + dx, y + dy) and !is_blocked.at(x + dx).at(y + dy)) {
                    x += dx;
                    y += dy;
                }
            } else {
                assert(false);
            }

            if (next_target < M and xs.at(next_target) == x and ys.at(next_target) == y) { ++next_target; }
        }

        if (next_target < M) {
            dbg(make_tuple(next_target));
        }

        assert(next_target == M);
        dump_onlinejudge(retstr);

        const int score = 1640 - cost;
        jdump("cost", cost);
        jdump("score", score);
        jdump("score150", score * 150);
    }
    // if (argc >= 2) { X = std::stoi(argv[1]); }
}
