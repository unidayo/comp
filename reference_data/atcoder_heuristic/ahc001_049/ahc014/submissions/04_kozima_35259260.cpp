#include <set>
#include <fstream>
#include <array>
#include <map>
#include <cstdlib>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <cassert>
#include <vector>
#include <iostream>
using namespace std;

unsigned int xor128() {
    static unsigned int x=123456789, y=362436069, z=521288629, w=88675123;
    unsigned int t;
    t=(x^(x<<11)); x=y; y=z; z=w;
    return (w=(w^(w>>19))^(t^(t>>8)));
}

inline bool rand_bool(double prob) {
    constexpr double x = 1LL<<32; // uint_max+1
    return xor128() < prob * x;
}

inline bool rand_bool() {
    return xor128() < 1u<<31;
}

inline double rand(double lb, double ub) {
    assert(lb < ub);
    unsigned int x = xor128();
    return lb + (ub - lb) * x / double(1LL<<32);
}

inline int rand_int(int n) { return xor128()%n; }

int timelimit = 5 * 1000, elapsed = 0, margin = 50;

class Timer {
  chrono::system_clock::time_point start_time = chrono::system_clock::now();
public:
  Timer() {}
  long long get_elapsed_time() {
    auto diff = chrono::system_clock::now() - start_time;
    return chrono::duration_cast<chrono::milliseconds>(diff).count();
  }
} timer;

class Point {
public:
    constexpr Point() {}
    constexpr Point(int _x, int _y) : x(_x), y(_y) {}
    constexpr Point(const Point &rhs) : x(rhs.x), y(rhs.y) {}
    constexpr int getx() const { return static_cast<int>(x); }
    constexpr int gety() const { return static_cast<int>(y); }
    constexpr bool operator==(const Point p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point p) const { return !operator==(p); }
    template<size_t i> constexpr int get() const {
        static_assert(i == 0 || i == 1);
        if constexpr (i == 0) return getx(); else return gety();
    }
    Point &operator=(const Point &rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }
private:
    int x = -1, y = -1;
};

template<>
struct std::tuple_size<Point> : integral_constant<size_t, 2> {};
template<size_t n>
struct std::tuple_element<n, Point> { using type = int; };

constexpr Point operator+(Point p, Point q) {
    return Point(p.getx() + q.getx(), p.gety() + q.gety());
}
constexpr Point operator-(Point p, Point q) {
    return Point(p.getx() - q.getx(), p.gety() - q.gety());
}
ostream& operator<<(ostream& out, Point p) {
    return out << p.getx() << ' ' << p.gety();
}

int L1(Point p, Point q) {
    return abs(p.getx() - q.getx()) + abs(p.gety() - q.gety());
}

int L22(Point p, Point q) {
    const int dx = abs(p.getx() - q.getx()), dy = abs(p.gety() - q.gety());
    return dx * dx + dy * dy;
}

enum struct Dir {
    R = 0, U = 1, L = 2, D = 3,
    RU = 4, LU = 5, LD = 6, RD = 7
};
constexpr Point dxdy[] = {
    {1, 0}, {0, 1}, {-1, 0}, {0, -1},
    {1, 1}, {-1, 1}, {-1, -1}, {1, -1}
};

constexpr Dir turn_left(Dir d) {
    const int idx = static_cast<int>(d);
    // 0 -> 1 -> 2 -> 3 -> 0 and 4 -> 5 -> 6 -> 7 -> 4
    return static_cast<Dir>((idx & 4) | ((idx + 1) & 3));
}

constexpr Dir turn_right(Dir d) {
    const int idx = static_cast<int>(d);
    // 0 <- 1 <- 2 <- 3 <- 0 and 4 <- 5 <- 6 <- 7 <- 4
    return static_cast<Dir>((idx & 4) | ((idx + 3) & 3));
}

constexpr Dir reverse_dir(Dir d) {
    const int idx = static_cast<int>(d);
    // 0 <-> 2, 1 <-> 3, 4 <-> 6, and 5 <-> 7
    return static_cast<Dir>(idx ^ 2);
}

constexpr Point get_delta(Dir d) { return dxdy[static_cast<int>(d)]; }
constexpr Point apply_move(Point p, Dir d)  { return p + dxdy[static_cast<int>(d)]; }

constexpr Dir calc_dir(int x1, int y1, int x2, int y2) {
    const int dx = x2 - x1, dy = y2 - y1;
    assert(dx != 0 || dy != 0);
    return (dx == 0 ? (dy > 0 ? Dir::U : Dir::D) :
            dy == 0 ? (dx > 0 ? Dir::R : Dir::L) :
            dx == dy ? (dx > 0 ? Dir::RU : Dir::LD) :
            (dx > 0 ? Dir::RD : Dir::LU));
}

constexpr Dir calc_dir(Point p, Point q) {
    return calc_dir(p.getx(), p.gety(), q.getx(), q.gety());
}

namespace {

static_assert(turn_left(Dir::R) == Dir::U);
static_assert(turn_left(Dir::U) == Dir::L);
static_assert(turn_left(Dir::L) == Dir::D);
static_assert(turn_left(Dir::D) == Dir::R);
static_assert(turn_left(Dir::RU) == Dir::LU);
static_assert(turn_left(Dir::LU) == Dir::LD);
static_assert(turn_left(Dir::LD) == Dir::RD);
static_assert(turn_left(Dir::RD) == Dir::RU);
static_assert(turn_left(turn_right(Dir::R)) == Dir::R);
static_assert(turn_left(turn_right(Dir::U)) == Dir::U);
static_assert(turn_left(turn_right(Dir::L)) == Dir::L);
static_assert(turn_left(turn_right(Dir::D)) == Dir::D);
static_assert(turn_left(turn_right(Dir::RU)) == Dir::RU);
static_assert(turn_left(turn_right(Dir::LU)) == Dir::LU);
static_assert(turn_left(turn_right(Dir::LD)) == Dir::LD);
static_assert(turn_left(turn_right(Dir::RD)) == Dir::RD);
static_assert(reverse_dir(Dir::R) == Dir::L);
static_assert(reverse_dir(Dir::U) == Dir::D);
static_assert(reverse_dir(Dir::L) == Dir::R);
static_assert(reverse_dir(Dir::D) == Dir::U);
static_assert(reverse_dir(Dir::RU) == Dir::LD);
static_assert(reverse_dir(Dir::LU) == Dir::RD);
static_assert(reverse_dir(Dir::LD) == Dir::RU);
static_assert(reverse_dir(Dir::RD) == Dir::LU);
static_assert(calc_dir(Point(0, 0), dxdy[0]) == static_cast<Dir>(0));
static_assert(calc_dir(Point(0, 0), dxdy[1]) == static_cast<Dir>(1));
static_assert(calc_dir(Point(0, 0), dxdy[2]) == static_cast<Dir>(2));
static_assert(calc_dir(Point(0, 0), dxdy[3]) == static_cast<Dir>(3));
static_assert(calc_dir(Point(0, 0), dxdy[4]) == static_cast<Dir>(4));
static_assert(calc_dir(Point(0, 0), dxdy[5]) == static_cast<Dir>(5));
static_assert(calc_dir(Point(0, 0), dxdy[6]) == static_cast<Dir>(6));
static_assert(calc_dir(Point(0, 0), dxdy[7]) == static_cast<Dir>(7));

constexpr bool check(Dir d) {
    constexpr Point p0(0, 0);
    const auto [x1, y1] = apply_move(p0, d);
    const auto [x2, y2] = apply_move(p0, turn_left(d));
    // [[0, -1]; [1, 0]]
    return (x1 == y2 && x2 == -y1);
}

static_assert(check(Dir::R));
static_assert(check(Dir::U));
static_assert(check(Dir::L));
static_assert(check(Dir::D));
static_assert(check(Dir::RU));
static_assert(check(Dir::LU));
static_assert(check(Dir::LD));
static_assert(check(Dir::RD));

};

// 

constexpr int NMAX = 61;

struct Input {
    int N, M;
    vector<Point> ps;
    bool is_valid(int x, int y) const {
        return 0 <= x && x < N && 0 <= y && y < N;
    }
    bool is_valid(Point p) const {
        const auto &[x, y] = p;
        return is_valid(x, y);
    }
};

int weight(const Input &in, Point p) {
    return 1 + L22(Point(in.N / 2, in.N / 2), p);
}

struct Rect {
    Point p1, p2, p3;
    Rect(Point _p1, Point _p2, Point _p3) : p1(_p1), p2(_p2), p3(_p3) {};
    Point calc_p4() const {
        return p3 + p1 - p2;
    }
    bool operator==(const Rect &rhs) const {
        return p1 == rhs.p1 && p2 == rhs.p2 && p3 == rhs.p3;
    }
    bool is_tilted() const {
        return p1.getx() != p2.getx() && p1.gety() != p2.gety();
    }
};

ostream &operator<<(ostream &out, const Rect &r) {
    out << r.p1 << ' ' << r.p2 << ' ' << r.p3 << ' ' << r.calc_p4();
    return out;
}

Input read_input() {
    Input in;
    cin >> in.N >> in.M;
    for (int i = 0; i < in.M; ++i) {
        int x, y;
        cin >> x >> y;
        in.ps.emplace_back(x, y);
    }
    return in;
}

void write_ans(const vector<Rect> &ans, ostream &f = cout) {
    f << ans.size() << '\n';
    for (Rect r : ans) {
        f << r.p1 << ' ' << r.p2 << ' ' << r.p3 << ' ' << r.calc_p4() << '\n';
    }
}

// 原点周りに左へ45度回転
Point rotate_point_45degree(const Point p) {
    const auto [x, y] = p;
    return Point{x + y, x - y};
}

Rect rotate_rectangle_45degree(Rect r) {
    return Rect{rotate_point_45degree(r.p1),
        rotate_point_45degree(r.p2),
        rotate_point_45degree(r.p3)};
}

// bounding box (left, right, bottom, top)
// 辺が座標軸に並行なものしか与えられないと仮定して、対角線だけ見るとよさそうだがなぜか遅くなった
tuple<int, int, int, int> calc_bbox(Rect r) {
    const auto [p1, p2, p3] = r;
    const Point p4 = r.calc_p4();
    return {min({p1.getx(), p2.getx(), p3.getx(), p4.getx()}),
        max({p1.getx(), p2.getx(), p3.getx(), p4.getx()}),
        min({p1.gety(), p2.gety(), p3.gety(), p4.gety()}),
        max({p1.gety(), p2.gety(), p3.gety(), p4.gety()})};
}

// p = p1 または p が端点を除く辺上にある
bool point_lies_on_rectangle(const Point &p, const Rect &r) {
    if (p == r.p1) return true;
    const bool t = r.is_tilted();
    const Point q = t ? rotate_point_45degree(p) : p;
    const Rect s = t ? rotate_rectangle_45degree(r) : r;
    const auto [left, right, bot, top] = calc_bbox(s);
    const auto [x, y] = q;
    if (x == left || x == right) return bot < y && y < top;
    if (y == bot || y == top) return left < x && x < right;
    return false;
}

// r1 と r2 の辺の共通部分の長さが正かどうか
bool have_overlapping_edges(Rect r1, Rect r2) {
    const bool t1 = r1.is_tilted(), t2 = r2.is_tilted();
    if (t1 != t2) return false;
    if (t1) {
        r1 = rotate_rectangle_45degree(r1);
        r2 = rotate_rectangle_45degree(r2);
    }
    const auto [left1, right1, bot1, top1] = calc_bbox(r1);
    const auto [left2, right2, bot2, top2] = calc_bbox(r2);
    // vertical edges
    if (left1 == left2 || left1 == right2 || right1 == left2 || right1 == right2) {
        if (bot1 == bot2 || (bot1 < bot2 ? bot2 < top1 : bot1 < top2)) return true;
    }
    // horizontal edges
    if (bot1 == bot2 || bot1 == top2 || top1 == bot2 || top1 == top2) {
        if (left1 == left2 || (left1 < left2 ? left2 < right1 : left1 < right2)) return true;
    }
    return false;
}


class State {
    const int N;
    vector<unsigned long long> used_pos;
    vector<unsigned char> used_dirs;
    static_assert(NMAX < sizeof(decltype(used_pos)::value_type) * 8);
    void add_segment(Point p1, Point p2);
    int get_index(Point p) const { return p.getx() * N + p.gety(); }
    int get_index_diff(Dir d) const {
        const auto [dx, dy] = get_delta(d);
        return dx * N + dy;
    }
    void mark_as_used_pos(int x, int y) {
        assert(!is_used_pos(x, y));
        used_pos[x] |= 1ull << y;
    }
    void mark_as_used_pos(Point p) {
        mark_as_used_pos(p.getx(), p.gety());
    }
    void mark_as_used_dir(int x, int y, Dir d) {
        assert(!is_used_dir(x, y, d));
        used_dirs[x * N + y] |= 1 << static_cast<int>(d);
    }
    void mark_as_used_dir(Point p, Dir d) {
        mark_as_used_dir(p.getx(), p.gety(), d);
    }
    bool is_used_dir_by_index(int i, Dir d) const {
        return (used_dirs[i] >> static_cast<int>(d) & 1) != 0;
    }
    void mark_as_used_dir_by_index(int i, Dir d) {
        assert(!is_used_dir_by_index(i, d));
        used_dirs[i] |= 1 << static_cast<int>(d);
    }
public:
    bool is_used_pos(int x, int y) const {
        return (used_pos[x] >> y & 1) != 0;
    }
    bool is_used_pos(Point p) const {
        return is_used_pos(p.getx(), p.gety());
    }
    bool is_used_dir(int x, int y, Dir d) const {
        return (used_dirs[x * N + y] >> static_cast<int>(d) & 1) != 0;
    }
    bool is_used_dir(Point p, Dir d) const {
        return is_used_dir(p.getx(), p.gety(), d);
    }
    State(const Input &in) : N(in.N), used_pos(in.N, 0), used_dirs(in.N * in.N, 0) {
        for (const auto &[x, y] : in.ps) mark_as_used_pos(x, y);
    }
    void add_rectangle(const Rect &r);
};

void State::add_segment(Point p1, Point p2) {
    const Dir dir = calc_dir(p1, p2); // Rect に持たせるべき？
    const Dir rev = reverse_dir(dir);
    const int delta = get_index_diff(dir), stop = get_index(p2);
    for (int i = get_index(p1), j = i + delta; i != stop; i = j, j += delta) {
        assert(!is_used_dir_by_index(i, dir));
        mark_as_used_dir_by_index(i, dir);
        mark_as_used_dir_by_index(j, rev);
    }        
}

void State::add_rectangle(const Rect &r) {
    const Point p4 = r.calc_p4();
    mark_as_used_pos(r.p1);
    add_segment(r.p1, r.p2);
    add_segment(r.p2, r.p3);
    add_segment(r.p3, p4);
    add_segment(p4, r.p1);
}

int calc_score(const Input &in, const vector<Rect> &ans) {
    long long s = 0;
    for (Point p : in.ps) s += weight(in, p);
    for (Rect r : ans) s += weight(in, r.p1);
    const int total_weight = in.N * in.N * (in.N * in.N + 5) / 6;
    return static_cast<int>(round(1e6 * in.N * in.N * s / in.M / total_weight));
}

class Solver {
    Input in;
    bool is_valid(Point p) const { return in.is_valid(p); }
    vector<Rect> runSA();
    vector<Rect> generate_initial_solution() const;
public:
    Solver(const Input &in) : in(in) {};
    vector<Rect> run();
};

__attribute__((noinline))
Point search_next_point(const State &st, const Input &in, Point p, Dir d) {
    const Point delta = get_delta(d);
    Point q = p + delta;
    while (in.is_valid(q) && !st.is_used_pos(q)) q = q + delta;
    return q;
}

// 途中の点が使われてないかどうかだけチェックする
bool is_valid_edge(const State &st, const Input &in, Point from, Point to, Dir d) {
    const Point delta = get_delta(d);
    for (Point p = from + delta; p != to; p = p + delta) {
        assert(in.is_valid(p));
        if (st.is_used_pos(p)) return false;
        assert(!st.is_used_dir(p, d));
    }
    return true;
}

void update_candidates(const State &st, const Input &in,
                       vector<Rect> &candidates, const Rect &last_rect)
{
    // remove candidates invalidates by last_rect
    {
        auto is_invalidated = [&last_rect](const Rect &r) {
            return (point_lies_on_rectangle(last_rect.p1, r) ||
                    have_overlapping_edges(last_rect, r));
        };
        auto e = remove_if(candidates.begin(), candidates.end(), is_invalidated);
        candidates.erase(e, candidates.end());
    }

    Point nbr[8];
    for (int d = 0; d < 8; ++d)
        nbr[d] = (st.is_used_dir(last_rect.p1, static_cast<Dir>(d)) ?
                  Point(-1, -1) :
                  search_next_point(st, in, last_rect.p1, static_cast<Dir>(d)));

    // add new candidates using last p1
    // use p as p3
    for (int d = 0; d < 8; ++d) {
        const Point p3 = last_rect.p1;
        const Dir dir34 = static_cast<Dir>(d);
        const Dir dir32 = turn_left(dir34);
        const Point p4 = nbr[static_cast<size_t>(dir34)];
        const Point p2 = nbr[static_cast<size_t>(dir32)];
        if (!in.is_valid(p4) || !in.is_valid(p2)) continue;
        if (st.is_used_dir(p4, dir32)) continue; // 3->2 = 4->1
        if (st.is_used_dir(p2, dir34)) continue; // 2->1 = 3->4

        const Point p1 = p2 + p4 - p3;
        if (!in.is_valid(p1) || st.is_used_pos(p1)) continue;
        const Dir dir12 = turn_left(dir32); //3->2 = 4->1
        if (st.is_used_dir(p1, dir12)) continue;
        const Dir dir14 = reverse_dir(dir32); //3->2 = 4->1
        if (st.is_used_dir(p1, dir14)) continue;
        if (!is_valid_edge(st, in, p1, p4, dir14)) continue;
        if (!is_valid_edge(st, in, p1, p2, dir12)) continue;

        candidates.emplace_back(p1, p2, p3);
    }
    // use p as p2
    for (int d = 0; d < 8; ++d) {
        const Point p2 = last_rect.p1;
        const Dir dir23 = static_cast<Dir>(d);
        const Dir dir34 = turn_left(dir23);
        if (st.is_used_dir(p2, dir23) || st.is_used_dir(p2, dir34)) continue;
        if (const Point q = apply_move(p2, dir34);
            !in.is_valid(q) || st.is_used_pos(q)) continue;

        const Point p3 = nbr[static_cast<size_t>(dir23)];
        if (!in.is_valid(p3)) continue;
        if (st.is_used_dir(p3, dir34)) continue;

        Point p4 = search_next_point(st, in, p3, dir34);
        if (!in.is_valid(p4)) continue;
        const Dir dir41 = turn_left(dir34);
        if (st.is_used_dir(p4, dir41)) continue;

        const Point p1 = p2 + p4 - p3;
        if (!in.is_valid(p1) || st.is_used_pos(p1)) continue;
        const Dir dir12 = turn_left(dir41);
        if (st.is_used_dir(p1, dir12)) continue;
        if (!is_valid_edge(st, in, p4, p1, dir41)) continue;
        if (!is_valid_edge(st, in, p1, p2, dir12)) continue;

        candidates.emplace_back(p1, p2, p3);
    }
    // use p as p4
    for (int d = 0; d < 8; ++d) {
        const Point p4 = last_rect.p1;
        const Dir dir41 = static_cast<Dir>(d);
        const Dir dir43 = turn_left(dir41); // 1->2 = 4->3
        if (st.is_used_dir(p4, dir41) || st.is_used_dir(p4, dir43)) continue;
        if (const Point q = apply_move(p4, dir41);
            !in.is_valid(q) || st.is_used_pos(q)) continue;

        const Point p3 = nbr[static_cast<size_t>(dir43)];
        if (!in.is_valid(p3)) continue;
        if (st.is_used_dir(p3, dir41)) continue; // 3->2 = 4->1

        const Point p2 = search_next_point(st, in, p3, dir41);
        if (!in.is_valid(p2)) continue;
        const Dir dir21 = reverse_dir(dir43); // 2->1 = 3->4
        if (st.is_used_dir(p2, dir21)) continue;

        const Point p1 = p2 + p4 - p3;
        if (!in.is_valid(p1) || st.is_used_pos(p1)) continue;
        if (st.is_used_dir(p1, dir43)) continue; // 1->2 = 4->3
        const Dir dir14 = reverse_dir(dir41);
        if (st.is_used_dir(p1, dir14)) continue;
        if (!is_valid_edge(st, in, p4, p1, dir41)) continue;
        if (!is_valid_edge(st, in, p2, p1, dir21)) continue;

        candidates.emplace_back(p1, p2, p3);
    }
}

/*
d として R か RU だけ調べる
すべてのありうる p, q に対して以下のように上下に進んで、
a, b (or c, d) のうち最初にぶつかる点で止まる。
ぶつからなかった方の位置を p1 とする。
  a------b
  |      |
  |  d   |
  p----->q
  |      |
  |      |
  c------d

todo:
上向きと下向きで二回探索するので、直近の座標を覚えておいた方がよい？？
横方向も三回ぐらい探索するので覚えておいた方がいい？？
*/

void list_candidates_r(const State &st, const Input &in, vector<Rect> &result) {
    const int N = in.N;
    for (int y = 0; y < N; ++y) {
        // p-q のラインが {(x, y) | 0 <= x < N} 上にある
        int x1 = 0;
        while (x1 < N && !st.is_used_pos(x1, y)) ++x1;
        int x2 = x1 + 1;
        while (x2 < N && !st.is_used_pos(x2, y)) ++x2;
        while (x2 < N) {
            // cerr << y << ' ' << x1 << ' ' << x2 << ' ' << endl;
            if (!st.is_used_dir(x1, y, Dir::R)) {
                if (!st.is_used_dir(x1, y, Dir::D) && !st.is_used_dir(x2, y, Dir::D)) {
                    // downward; look for c, d
                    int y1 = y - 1;
                    while (y1 >= 0 && !st.is_used_pos(x1, y1) && !st.is_used_pos(x2, y1)) --y1;
                    if (y1 >= 0) {
                        if (st.is_used_pos(x1, y1)) {
                            if (!st.is_used_pos(x2, y1)) {
                                // (x1, y), (x2, y), (x2, y1), (x1, y1) with p1 = (x2, y1)
                                // 最後の辺が valid かどうかチェックして、 valid なら候補に追加
                                if (!st.is_used_dir({x1, y1}, Dir::R) && 
                                    is_valid_edge(st, in, {x1, y1}, {x2, y1}, Dir::R))
                                    result.push_back({{x2, y1}, {x2, y}, {x1, y}});
                            }
                        } else if (st.is_used_pos(x2, y1)) {
                            // (x1, y), (x2, y), (x2, y1), (x1, y1) with p1 = (x1, y1)
                            if (!st.is_used_dir({x2, y1}, Dir::L) && 
                                is_valid_edge(st, in, {x2, y1}, {x1, y1}, Dir::L))
                                result.push_back({{x1, y1}, {x2, y1}, {x2, y}});
                        }
                    }
                }
                if (!st.is_used_dir(x1, y, Dir::U) && !st.is_used_dir(x2, y, Dir::U)) {
                    // upward; look for a, b
                    int y1 = y + 1;
                    while (y1 < N && !st.is_used_pos(x1, y1) && !st.is_used_pos(x2, y1)) ++y1;
                    if (y1 < N) {
                        if (st.is_used_pos(x1, y1)) {
                            if (!st.is_used_pos(x2, y1)) {
                                // (x1, y), (x2, y), (x2, y1), (x1, y1) with p1 = (x2, y1)
                                if (!st.is_used_dir({x1, y1}, Dir::R) && 
                                    is_valid_edge(st, in, {x1, y1}, {x2, y1}, Dir::R))
                                    result.push_back({{x2, y1}, {x1, y1}, {x1, y}});
                            }
                        } else if (st.is_used_pos(x2, y1)) {
                            // (x1, y), (x2, y), (x2, y1), (x1, y1) with p1 = (x1, y1)
                            if (!st.is_used_dir({x2, y1}, Dir::L) && 
                                is_valid_edge(st, in, {x2, y1}, {x1, y1}, Dir::L))
                                result.push_back({{x1, y1}, {x1, y}, {x2, y}});
                        }
                    }
                }
            }
            x1 = x2;
            do { ++x2; } while (x2 < N && !st.is_used_pos(x2, y));
        }
    }
}

// z = y + x, w = y - x とする
// w = const は左下と右上に伸びる直線
// z = const は右下と左上に伸びる直線
void list_candidates_ru(const State &st, const Input &in, vector<Rect> &result) {
    const int N = in.N;
    for (int w = - N + 2; w <= N - 2; ++w) {
        // w = y - x, y >= 0 ==> x = y - w >= - w
        // w = y - x, y < N ==> x = y - w < N - w
        const int xmin = max(0, -w), // inclusive
            xmax = min(N, N - w); // exclusive
        int x1 = xmin;
        while (x1 < xmax && !st.is_used_pos(x1, w + x1)) ++x1;
        int x2 = x1 + 1;
        while (x2 < xmax && !st.is_used_pos(x2, w + x2)) ++x2;
        while (x2 < xmax) {
            if (!st.is_used_dir(x1, x1 + w, Dir::RU)) {
                if (!st.is_used_dir(x1, x1 + w, Dir::RD) && !st.is_used_dir(x2, x2 + w, Dir::RD)) {
                    // downward; look for c, d
                    Point p(x1 + 1, x1 + w - 1), q(x2 + 1, x2 + w - 1);
                    const Point delta(1, -1);
                    while (p.gety() >= 0 && q.getx() < N && !st.is_used_pos(p) && !st.is_used_pos(q))
                        p = p + delta, q = q + delta;
                    if (p.gety() >= 0 && q.getx() < N) {
                        if (st.is_used_pos(p)) {
                            if (!st.is_used_pos(q)) {
                                // (x1, x1 + w), (x2, x2 + w), p, q with p1 = q
                                if (!st.is_used_dir(p, Dir::RU) && 
                                    is_valid_edge(st, in, p, q, Dir::RU))
                                    // cerr << q << ' ' << x2 << ' ' << x2 + w << ' ' << x1 << ' ' << x1 + w << ' ' << p << __LINE__ << '\n',
                                    result.push_back({q, {x2, x2 + w}, {x1, x1 + w}});
                            }
                        } else if (st.is_used_pos(q)) {
                            // (x1, x1 + w), (x2, x2 + w), p, q with p1 = p
                            if (!st.is_used_dir(q, Dir::LD) &&
                                is_valid_edge(st, in, q, p, Dir::LD))
                                // cerr << q << ' ' << x2 << ' ' << x2 + w << ' ' << x1 << ' ' << x1 + w << ' ' << p << __LINE__ << '\n',
                                result.push_back({p, q, {x2, x2 + w}});
                        }
                    }
                }
                if (!st.is_used_dir(x1, x1 + w, Dir::LU) && !st.is_used_dir(x2, x2 + w, Dir::LU)) {
                    // upward; look for a, b
                    Point p(x1 - 1, x1 + w + 1), q(x2 - 1, x2 + w + 1);
                    const Point delta(-1, 1);
                    while (p.getx() >= 0 && q.gety() < N && !st.is_used_pos(p) && !st.is_used_pos(q))
                        p = p + delta, q = q + delta;
                    if (p.getx() >= 0 && q.gety() < N) {
                        if (st.is_used_pos(p)) {
                            if (!st.is_used_pos(q)) {
                                // (x1, x1 + w), (x2, x2 + w), p, q with p1 = q
                                if (!st.is_used_dir(p, Dir::RU) &&
                                    is_valid_edge(st, in, p, q, Dir::RU))
                                    // cerr << q << ' ' << x2 << ' ' << x2 + w << ' ' << x1 << ' ' << x1 + w << ' ' << p << __LINE__ << '\n',
                                    result.push_back({q, p, {x1, x1 + w}});
                            }
                        } else if (st.is_used_pos(q)) {
                            // (x1, x1 + w), (x2, x2 + w), p, q with p1 = p
                            if (!st.is_used_dir(q, Dir::LD) &&
                                is_valid_edge(st, in, q, p, Dir::LD))
                                // cerr << q << ' ' << x2 << ' ' << x2 + w << ' ' << x1 << ' ' << x1 + w << ' ' << p << __LINE__ << '\n',
                                result.push_back({p, {x1, x1 + w}, {x2, x2 + w}});
                        }
                    }
                }
            }
            x1 = x2;
            do { ++x2; } while (x2 < xmax && !st.is_used_pos(x2, x2 + w));
        }
    }
}

vector<Rect> list_candidates(const State &st, const Input &in) {
    vector<Rect> candidates;
    list_candidates_r(st, in, candidates); // R
    list_candidates_ru(st, in, candidates); // RU

    // check
    // vector<Rect> candidates2 = list_candidates_old(st, in);
    // set<Rect> ret(candidates.begin(), candidates.end()),
    //     expect(candidates2.begin(), candidates2.end()),
    //     extra, missing;
    // for (Rect r : ret) if (expect.find(r) == expect.end()) extra.insert(r);
    // for (Rect r : expect) if (ret.find(r) == ret.end()) missing.insert(r);
    // if (ret != expect) {
    //     cerr << "extra----------------\n";
    //     for (const Rect &r : extra) cerr << r << '\n';
    //     cerr << "missing----------------\n";
    //     for (const Rect &r : missing) cerr << r << '\n';
    //     cerr << '\n';
    // } else {
    //     // cerr << "OK\n";
    // }
    // assert(ret == expect);
    return candidates;
}

struct SAState {
    Input in;
    vector<Rect> cur_ans;
    int cur_score;
    double temp;
    bool step();
    // void destroy();
};

namespace debug {
int rc = 0, tt = 0, trans = 0;
bool outputvis = false;
};

bool SAState::step() {
    vector<Rect> ans = cur_ans;
    State st(in);
    int remove_count = 0;
    const int remove_count_max = min(int(ans.size()) / 2, 2 * in.N);
    class PointUsageManager {
        vector<bool> removed_flags;
        int N;
    public:
        PointUsageManager(int n) : removed_flags(n * n, false), N(n) {}
        bool is_removed(Point p) const { return removed_flags[p.getx() * N + p.gety()]; }
        void remove(Point p) { removed_flags[p.getx() * N + p.gety()] = true; };
    };
    class RandomChoice {
        int n = 0;
    public:
        RandomChoice() {}
        bool operator()() { return rand_int(++n) == 0; }
    } rand_choice;
    Rect tabu{{}, {}, {}};
    if (!ans.empty()) {
        PointUsageManager man(in.N);
        if (remove_count_max >= 2 && rand_bool(0.06)) {
            debug::tt = 0;
            vector<vector<int>> added_at(in.N, vector<int>(in.N, -1));
            vector<int> child_count(ans.size(), 0);
            for (int i = 0; i < int(ans.size()); ++i) {
                const auto &[x, y] = ans[i].p1;
                added_at[x][y] = i;
            }
            for (const Rect &r : ans) {
                const auto &[p1, p2, p3] = r;
                const Point p4 = r.calc_p4();
                for (const Point &p : {p2, p3, p4}) {
                    const auto &[x, y] = p;
                    const int i = added_at[x][y];
                    if (i >= 0) child_count[i]++;
                }
            }
            remove_count = remove_count_max / 2 + rand_int(remove_count_max / 2);
            vector<int> q;
            for (int i = 0; i < int(ans.size()); ++i)
                if (const auto &[x, y] = ans[i].p1; added_at[x][y] >= 0 && child_count[added_at[x][y]] == 0)
                    q.push_back(i);
            // // shuffle
            // for (int i = int(q.size()) - 1; i > 0; --i)
            //     if (const int j = rand_int(i + 1); i != j) swap(q[i], q[j]);
            for (int r = 0; r < remove_count; ++r) {
                assert(!q.empty());
                const int idx = rand_bool() ? rand_int(int(q.size())) : int(q.size()) - 1;
                const int i = q[idx];
                if (idx + 1 < q.size()) q[idx] = q.back();
                q.pop_back();
                const auto &[p1, p2, p3] = ans[i];
                man.remove(p1);
                if (rand_choice()) tabu = ans[i];
                const Point p4 = ans[i].calc_p4();
                for (const Point &p : {p2, p3, p4}) {
                    const auto &[x, y] = p;
                    const int j = added_at[x][y];
                    if (j >= 0 && --child_count[j] == 0) q.push_back(j);
                }
            }
            for (size_t i = 0, j = 0; i < ans.size(); ++i) {
                if (!man.is_removed(ans[i].p1)) {
                    if (j < i) ans[j] = ans[i];
                    ++j;
                }
            }
            assert(remove_count <= ans.size());
            ans.erase(ans.end() - remove_count, ans.end());
        } else if (rand_bool()) {
            debug::tt = 1;
            do {
                const size_t pivot = rand_int(int(ans.size()));
                man.remove(ans[pivot].p1);
                if (rand_choice()) tabu = ans[pivot];
                int count = 1;
                for (size_t i = pivot + 1, j = pivot; i < ans.size(); ++i) {
                    const auto &[p1, p2, p3] = ans[i];
                    const Point p4 = ans[i].calc_p4();
                    if (man.is_removed(p2) || man.is_removed(p3) || man.is_removed(p4)) {
                        man.remove(p1);
                        if (rand_choice()) tabu = ans[i];
                        ++count;
                    } else {
                        ans[j++] = ans[i];
                    }
                }
                ans.erase(ans.end() - count, ans.end());
                remove_count += count;
            } while (remove_count < remove_count_max / 2 && rand_bool() && !ans.empty());
        } else {
            const size_t pivot = rand_int(int(ans.size()));
            const Point p = ans[pivot].p1;
            man.remove(p);
            if (rand_bool()) {
                for (int i = 0; i < 4; ++i) {
                    const Point q = apply_move(p, static_cast<Dir>(i));
                    if (in.is_valid(q)) man.remove(q);
                }
                debug::tt = 2;
            } else {
                for (int i = 0; i < 8; ++i) {
                    const Point q = apply_move(p, static_cast<Dir>(i));
                    if (in.is_valid(q)) man.remove(q);
                }
                debug::tt = 3;
            }
            for (size_t i = 0, j = 0; i < ans.size(); ++i) {
                const auto &[p1, p2, p3] = ans[i];
                const Point p4 = ans[i].calc_p4();
                if (man.is_removed(p1) || man.is_removed(p2) || man.is_removed(p3) || man.is_removed(p4)) {
                    man.remove(p1);
                    if (rand_choice()) tabu = ans[i];
                    ++remove_count;
                } else {
                    if (j < i) ans[j] = ans[i];
                    ++j;
                }
                if (remove_count > remove_count_max) return false;
            }
            ans.erase(ans.end() - remove_count, ans.end());
        }
        for (Rect r : ans) st.add_rectangle(r);
    }
    debug::rc = remove_count;

    if (remove_count > remove_count_max) return false;
    ++debug::trans;

    const int target_tt = 1;
    if (debug::tt == target_tt && debug::outputvis && cur_score > 850000) {
        ofstream out_before("out_before.txt");
        write_ans(cur_ans, out_before);
        ofstream out_removed("out_removed.txt");
        write_ans(ans, out_removed);
    }

    const int compare_func = rand_bool(0.1) ? 3 : rand_int(3);
    const Point base_p = vector<Point>{{-1, -1}, {in.N, -1}, {-1, in.N}, {in.N, in.N}}.at(rand_int(4));
    vector<Rect> candidates;
    for (int iter = 0;; ++iter) {
        if (iter == 0) candidates = list_candidates(st, in);
        else {
            update_candidates(st, in, candidates, ans.back());
            // check
            // vector<Rect> expect = list_candidates(st, in);
            // auto compare_point = [](Point &l, Point &r) {
            //     return make_pair(l.getx(), l.gety()) < make_pair(r.getx(), r.gety());
            // };
            // auto compare_rect = [compare_point](Rect &l, Rect &r) {
            //     return l.p1 == r.p1 ? compare_point(l.p2, r.p2) : compare_point(l.p1, r.p1);
            // };
            // sort(expect.begin(), expect.end(), compare_rect);
            // sort(candidates.begin(), candidates.end(), compare_rect);
            // if (candidates != expect) {
            //     cerr << "difference detected\n";
            //     for (auto r : candidates) cerr << r << '\n';
            //     cerr << '\n';
            //     for (auto r : expect) cerr << r << '\n';
            //     cerr << "----\n";
            // }
        }
        if (candidates.empty()) break;
        else {
            auto compare1 = [base_p, tabu](const Rect &lhs, const Rect &rhs) {
                if (lhs == tabu) return true;
                if (rhs == tabu) return false;
                const bool tilt1 = lhs.is_tilted();
                const bool tilt2 = rhs.is_tilted();
                const int cost1 = (L1(lhs.p1, lhs.p2) + L1(lhs.p2, lhs.p3)) / (tilt1 ? 2 : 1),
                    cost2 = (L1(rhs.p1, rhs.p2) + L1(rhs.p2, rhs.p3)) / (tilt2 ? 2 : 1);
                return (L22(base_p, lhs.p1)) * cost2 < (L22(base_p, rhs.p1)) * cost1;
            };
            auto compare2 = [this, tabu](const Rect &lhs, const Rect &rhs) {
                if (lhs == tabu) return true;
                if (rhs == tabu) return false;
                return weight(in, lhs.p1) < weight(in, rhs.p1);
            };
            auto compare3 = [&st, this](const Rect &lhs, const Rect &rhs) {
                int c = 0;
                const auto [x1, y1] = lhs.p1;
                for (int x = max(0, x1 - 2); x <= min(x1 + 2, in.N - 1); ++x)
                    for (int y = max(0, y1 - 2); y <= min(y1 + 2, in.N - 1); ++y)
                        if (st.is_used_pos(x, y)) c--;
                const auto [x2, y2] = rhs.p1;
                for (int x = max(0, x2 - 2); x <= min(x2 + 2, in.N - 1); ++x)
                    for (int y = max(0, y2 - 2); y <= min(y2 + 2, in.N - 1); ++y)
                        if (st.is_used_pos(x, y)) c++;
                return c > 0;
            };
            const Rect r = (compare_func == 3 ?
                            candidates[rand_int(int(candidates.size()))] :
                            compare_func == 0 ?
                            *max_element(candidates.begin(), candidates.end(), compare1) :
                            compare_func == 1 ? 
                            *max_element(candidates.begin(), candidates.end(), compare2) :
                            *max_element(candidates.begin(), candidates.end(), compare3));
            ans.push_back(r);
            st.add_rectangle(r);
            // cerr << "add " << r << endl;
        }
    }

    const int score = calc_score(in, ans);
    bool accept = cur_score <= score;
    if (!accept) {
        const int diff = cur_score - score;
        const double prob = exp(-diff / temp);
        accept = rand_bool(prob);
    }

    if (accept) {
        if (debug::tt == target_tt && debug::outputvis && cur_score > 850000 && cur_score < score) {
            ofstream out_after("out_after.txt");
            write_ans(ans, out_after);
            debug::outputvis = false;
        }
        cur_score = score;
        cur_ans = ans;
    }

    return accept;
}

vector<Rect> Solver::generate_initial_solution() const {
    vector<Rect> best_ans;
    int best_score = 0;
    for (int pattern = 0; pattern < 5; ++pattern) {
        vector<Rect> ans;
        State st(in);
        const Point base_p = vector<Point>{{-1, -1}, {in.N, -1}, {-1, in.N}, {in.N, in.N}, {in.N, in.N}}.at(pattern);
        for (;;) {
            const vector<Rect> candidates = list_candidates(st, in);
            if (candidates.empty()) break;
            else {
                auto compare1 = [base_p](const Rect &lhs, const Rect &rhs) {
                    const bool tilt1 = lhs.p1.getx() != lhs.p2.getx() && lhs.p1.gety() != lhs.p2.gety();
                    const bool tilt2 = rhs.p1.getx() != rhs.p2.getx() && rhs.p1.gety() != rhs.p2.gety();
                    const int cost1 = (L1(lhs.p1, lhs.p2) + L1(lhs.p2, lhs.p3)) / (tilt1 ? 2 : 1),
                        cost2 = (L1(rhs.p1, rhs.p2) + L1(rhs.p2, rhs.p3)) / (tilt2 ? 2 : 1);
                    return (L22(base_p, lhs.p1)) * cost2 < (L22(base_p, rhs.p1)) * cost1;
                };
                auto compare2 = [this](const Rect &lhs, const Rect &rhs) {
                    return weight(in, lhs.p1) < weight(in, rhs.p1);
                };
                const Rect r = (pattern < 4 ?
                                *max_element(candidates.begin(), candidates.end(), compare1) :
                                *max_element(candidates.begin(), candidates.end(), compare2));
                ans.push_back(r);
                st.add_rectangle(r);
            }
        }

        const int score = calc_score(in, ans);
        if (best_score < score) {
            best_score = score;
            best_ans = ans;
        }
    }

    return best_ans;
}

vector<Rect> Solver::runSA() {
    auto run = [this] {
        vector<Rect> best_ans = generate_initial_solution();
        int best_score = calc_score(in, best_ans);
        int iter = 0, ac = 0;
        int upd_iter = 0;
        const long long start_time = timer.get_elapsed_time();
        const double start_temp = (in.N < 39 ? 2e4 : 1e4 * (1 + max(0.0, (61.0 - in.N) / (61 - 39) * 2))),
            end_temp = start_temp / 10;
        SAState sa{.in = in, .cur_ans = best_ans, .cur_score = 0, .temp = start_temp};
        cerr << iter << ' ' << best_score << endl;
        // array<array<int, 4000>, 4> rcs_all{}, rcs_ac{};
        const bool allow_restart = (in.N < 39);
        while (timer.get_elapsed_time() + margin < timelimit) {
            ++iter;
            const double progress = double(timer.get_elapsed_time() - start_time) / (timelimit - start_time);
            sa.temp = start_temp + (end_temp - start_temp) * progress;
            const bool accept = sa.step();

            // rcs_all[debug::tt][debug::rc]++;
            if (accept) {
                ++ac;
                // rcs_ac[debug::tt][debug::rc]++;
                // cout << iter << ' ' << sa.cur_score << ' ' << rc << ' ' << tt << endl;
                if (best_score < sa.cur_score) {
                    // cerr << iter << ' ' << sa.cur_score << ' ' << sa.cur_ans.size() << endl;
                    best_score = sa.cur_score;
                    best_ans = sa.cur_ans;
                    upd_iter = iter;
                }
            }

            if (allow_restart && upd_iter + 10000 < iter && timer.get_elapsed_time() + 500 < timelimit) {
                cerr << "restart\n";
                break;
            }
        }

        // for (int t = 0; t < 4; ++t) {
        //     cerr <<"t = " << t << endl;
        //     for (int i = 0; i < 300; ++i)
        //         if (rcs_all[t][i] > 0) cerr << i << ' ' << rcs_all[t][i] << ' ' << rcs_ac[t][i] << endl;
        // }
        const int score = calc_score(in, best_ans);
        cerr << iter << ' ' << ac << ' ' << score << endl;
        return make_tuple(best_ans, best_score, iter);
    };

    vector<Rect> best_ans;
    int best_score = 0;
    int total_iter = 0, run_count = 0;
    while (timer.get_elapsed_time() + margin < timelimit) {
        auto [ans, score, iter] = run();
        total_iter += iter;
        ++run_count;
        if (best_score < score) {
            best_score = score;
            best_ans= ans;
        }
    }
    cerr << "result = " << total_iter << ' ' << debug::trans << ' ' << run_count << ' ' << best_score << endl;
    return best_ans;
}

vector<Rect> Solver::run() {
    return runSA();
}


int main(int argc, char **argv) {
    // assert(have_overlapping_edges({{1,2}, {1,1}, {2,1}}, {{3,2}, {2,2}, {2,1}}));
    // assert(!point_lies_on_rectangle({10, 16}, {{9, 17}, {9, 16}, {10, 16}}));
    // assert(!have_overlapping_edges({{10, 16}, {9, 15}, {10, 14}}, {{9, 17}, {9, 16}, {10, 16}}));
    // assert(!have_overlapping_edges({{9, 17}, {9, 16}, {10, 16}}, {{10, 16}, {9, 15}, {10, 14}}));
    // return 0;

    // for (int i = 0; i < 10000; i++) {
    //     auto rand_pt = [] { return Point{rand_int(5), rand_int(5)}; };
    //     auto rand_rect = [rand_pt] {
    //         Point p1 = rand_pt();
    //         Dir d = static_cast<Dir>(rand_int(8));
    //         Point p2 = p1;
    //         for (int i = 0, len = rand_int(5) + 1; i < len; ++i) p2 = apply_move(p2, d);
    //         Point p3 = p2;
    //         d = turn_left(d);
    //         for (int i = 0, len = rand_int(5) + 1; i < len; ++i) p3 = apply_move(p3, d);
    //         return Rect{p1, p2, p3};
    //     };
    //     Rect r1 = rand_rect(), r2 = rand_rect();
    //     cerr << r1 << " vs. " << r2 << ' ' << have_overlapping_edges(r1, r2) << '\n';
    //     assert(have_overlapping_edges(r1, r2) == have_overlapping_edges2(r1, r2));
    // }

    // return 0;

    if (argc > 1) timelimit = atoi(argv[1]) * 1000;
    if (argc > 2) for (int i = 0; i < atoi(argv[2]); ++i) xor128();
    const Input in = read_input();

    Solver s(in);
    const vector<Rect> ans = s.run();
    write_ans(ans);

#ifndef ONLINE_JUDGE
    if (timer.get_elapsed_time() > timelimit) {
        cerr << "ELAPSED " << timer.get_elapsed_time() << '\n';
        return 1;
    }
#endif
}
