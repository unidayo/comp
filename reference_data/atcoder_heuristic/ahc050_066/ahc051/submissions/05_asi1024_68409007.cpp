#pragma GCC target("arch=skylake-avx512")
#pragma GCC optimize("O3","inline")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,fma,avx512f")

// #include <atcoder/dsu>
#include <atcoder/segtree>
// #include <atcoder/mincostflow>

#include <immintrin.h>

#define FINAL_SUBMIT 0

#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <climits>
#include <complex>
#include <cstdio>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define REP(i, n) for (int i = 0; i < (int)(n); i++)
#define ALL(x) (x).begin(), (x).end()

#if FINAL_SUBMIT
#define HHH(x)
#define LOG(x)
#define FORCE_LOG(x) std::cerr << #x << " = " << x << "\n"
#define GRAPH(x, y)
#define CERR() if (false) std::cerr
#define NDEBUG
#else
#define HHH(x) std::cerr << "L" << __LINE__ << ": " << #x << " : " << (x) << "\n"
#define LOG(x) std::cerr << #x << " = " << x << "\n"
#define FORCE_LOG(x) std::cerr << #x << " = " << x << "\n"
#define GRAPH(x, y) std::cerr << "graph " << x << " " << y << "\n"
#define CERR() std::cerr
#endif

template <typename T> T &chmin(T &a, const T &b) { return a = std::min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = std::max(a, b); }

using ll = long long;
using ld = double;
using Vec = std::complex<double>;
// using P = std::pair<int, int>;
constexpr int INF = 1e9;
constexpr double PI = acos(-1.0);

template <typename T> T sq(T x) { return x * x; }

template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& p) {
  return os << "(" << p.first << ", " << p.second << ")";
}
template <typename T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
  os << "[";
  for (const T& e : vec) os << " " << e;
  os << " ]";
  return os;
}

class Timer {
public:
  // static constexpr double time_limit = 1.98;
  static constexpr int64_t CYCLES_PER_SEC = 2900000000;
  // static constexpr double CYCLES = CYCLES_PER_SEC * time_limit;
  int64_t start, stop;
#if 0
  // Deterministic mode
  inline int64_t get_cycle() const {
    static int64_t cycle = 0;
    return cycle += 30000;
  }
#else
  inline int64_t get_cycle() const {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return int64_t(low) | (int64_t(high) << 32);
  }
#endif
  Timer(double time_limit) : start(get_cycle()), stop(start + CYCLES_PER_SEC * time_limit) {}
  inline void reset_stop(double time_limit) { stop = start + CYCLES_PER_SEC *  time_limit; }
  inline bool within(int64_t end) const { return get_cycle() < end; }
  inline bool within() const { return within(stop); }
  inline double elapsed() const { return (get_cycle() - start) / double(CYCLES_PER_SEC); }
};

class XorShift {
  unsigned int x, y, z, w;
public:
  XorShift() : x(314159265), y(358979323), z(846264338), w(327950288) {}
  inline unsigned int next() {
    unsigned int t = x ^ x << 11;
    x = y; y = z; z = w;
    return w = w ^ w >> 19 ^ t ^ t >> 8;
  }
  inline int next(int n) { return next() % n; }
  inline int next(size_t n) { return next() % n; }
  inline int next(int lo, int hi) { return lo + next(hi - lo + 1); }
  inline double next(double x) { return double(next()) / (1LL << 32) * x; }
  inline double next(double lo, double hi) { return lo + next(hi - lo); }
};

double get_param(const char* env_var) {
  char* ret = getenv(env_var);
  assert (ret != nullptr);
  return stod(std::string(ret));
}

// double a_param = get_param("A_PARAM");
// double b_param = get_param("B_PARAM");
// double c_param = get_param("C_PARAM");
// int a_param = get_param("A_PARAM");
// int j_param = get_param("J_PARAM");

Timer timer(1.98);
Timer timer0(1.75);
XorShift rnd;

// https://github.com/delfrrr/delaunator-cpp
/*
MIT License

Copyright (c) 2018 Volodymyr Bilonenko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
namespace delaunator {

//@see https://stackoverflow.com/questions/33333363/built-in-mod-vs-custom-mod-function-improve-the-performance-of-modulus-op/33333636#33333636
inline size_t fast_mod(const size_t i, const size_t c) {
    return i >= c ? i % c : i;
}

// Kahan and Babuska summation, Neumaier variant; accumulates less FP error
inline double sum(const std::vector<double>& x) {
    double sum = x[0];
    double err = 0.0;

    for (size_t i = 1; i < x.size(); i++) {
        const double k = x[i];
        const double m = sum + k;
        err += std::fabs(sum) >= std::fabs(k) ? sum - m + k : k - m + sum;
        sum = m;
    }
    return sum + err;
}

inline double dist(
    const double ax,
    const double ay,
    const double bx,
    const double by) {
    const double dx = ax - bx;
    const double dy = ay - by;
    return dx * dx + dy * dy;
}

inline double circumradius(
    const double ax,
    const double ay,
    const double bx,
    const double by,
    const double cx,
    const double cy) {
    const double dx = bx - ax;
    const double dy = by - ay;
    const double ex = cx - ax;
    const double ey = cy - ay;

    const double bl = dx * dx + dy * dy;
    const double cl = ex * ex + ey * ey;
    const double d = dx * ey - dy * ex;

    const double x = (ey * bl - dy * cl) * 0.5 / d;
    const double y = (dx * cl - ex * bl) * 0.5 / d;

    if ((bl > 0.0 || bl < 0.0) && (cl > 0.0 || cl < 0.0) && (d > 0.0 || d < 0.0)) {
        return x * x + y * y;
    } else {
        return std::numeric_limits<double>::max();
    }
}

inline bool orient(
    const double px,
    const double py,
    const double qx,
    const double qy,
    const double rx,
    const double ry) {
    return (qy - py) * (rx - qx) - (qx - px) * (ry - qy) < 0.0;
}

inline std::pair<double, double> circumcenter(
    const double ax,
    const double ay,
    const double bx,
    const double by,
    const double cx,
    const double cy) {
    const double dx = bx - ax;
    const double dy = by - ay;
    const double ex = cx - ax;
    const double ey = cy - ay;

    const double bl = dx * dx + dy * dy;
    const double cl = ex * ex + ey * ey;
    const double d = dx * ey - dy * ex;

    const double x = ax + (ey * bl - dy * cl) * 0.5 / d;
    const double y = ay + (dx * cl - ex * bl) * 0.5 / d;

    return std::make_pair(x, y);
}

struct compare {

    std::vector<double> const& coords;
    double cx;
    double cy;

    bool operator()(std::size_t i, std::size_t j) {
        const double d1 = dist(coords[2 * i], coords[2 * i + 1], cx, cy);
        const double d2 = dist(coords[2 * j], coords[2 * j + 1], cx, cy);
        const double diff1 = d1 - d2;
        const double diff2 = coords[2 * i] - coords[2 * j];
        const double diff3 = coords[2 * i + 1] - coords[2 * j + 1];

        if (diff1 > 0.0 || diff1 < 0.0) {
            return diff1 < 0;
        } else if (diff2 > 0.0 || diff2 < 0.0) {
            return diff2 < 0;
        } else {
            return diff3 < 0;
        }
    }
};

inline bool in_circle(
    const double ax,
    const double ay,
    const double bx,
    const double by,
    const double cx,
    const double cy,
    const double px,
    const double py) {
    const double dx = ax - px;
    const double dy = ay - py;
    const double ex = bx - px;
    const double ey = by - py;
    const double fx = cx - px;
    const double fy = cy - py;

    const double ap = dx * dx + dy * dy;
    const double bp = ex * ex + ey * ey;
    const double cp = fx * fx + fy * fy;

    return (dx * (ey * cp - bp * fy) -
            dy * (ex * cp - bp * fx) +
            ap * (ex * fy - ey * fx)) < 0.0;
}

constexpr double EPSILON = std::numeric_limits<double>::epsilon();
constexpr std::size_t INVALID_INDEX = std::numeric_limits<std::size_t>::max();

inline bool check_pts_equal(double x1, double y1, double x2, double y2) {
    return std::fabs(x1 - x2) <= EPSILON &&
           std::fabs(y1 - y2) <= EPSILON;
}

// monotonically increases with real angle, but doesn't need expensive trigonometry
inline double pseudo_angle(const double dx, const double dy) {
    const double p = dx / (std::abs(dx) + std::abs(dy));
    return (dy > 0.0 ? 3.0 - p : 1.0 + p) / 4.0; // [0..1)
}

struct DelaunatorPoint {
    std::size_t i;
    double x;
    double y;
    std::size_t t;
    std::size_t prev;
    std::size_t next;
    bool removed;
};

class Delaunator {

public:
    std::vector<double> const& coords;
    std::vector<std::size_t> triangles;
    std::vector<std::size_t> halfedges;
    std::vector<std::size_t> hull_prev;
    std::vector<std::size_t> hull_next;
    std::vector<std::size_t> hull_tri;
    std::size_t hull_start;

    Delaunator(std::vector<double> const& in_coords);

    double get_hull_area();

private:
    std::vector<std::size_t> m_hash;
    double m_center_x;
    double m_center_y;
    std::size_t m_hash_size;
    std::vector<std::size_t> m_edge_stack;

    std::size_t legalize(std::size_t a);
    std::size_t hash_key(double x, double y) const;
    std::size_t add_triangle(
        std::size_t i0,
        std::size_t i1,
        std::size_t i2,
        std::size_t a,
        std::size_t b,
        std::size_t c);
    void link(std::size_t a, std::size_t b);
};

Delaunator::Delaunator(std::vector<double> const& in_coords)
    : coords(in_coords),
      triangles(),
      halfedges(),
      hull_prev(),
      hull_next(),
      hull_tri(),
      hull_start(),
      m_hash(),
      m_center_x(),
      m_center_y(),
      m_hash_size(),
      m_edge_stack() {
    std::size_t n = coords.size() >> 1;

    double max_x = std::numeric_limits<double>::min();
    double max_y = std::numeric_limits<double>::min();
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    std::vector<std::size_t> ids;
    ids.reserve(n);

    for (std::size_t i = 0; i < n; i++) {
        const double x = coords[2 * i];
        const double y = coords[2 * i + 1];

        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;

        ids.push_back(i);
    }
    const double cx = (min_x + max_x) / 2;
    const double cy = (min_y + max_y) / 2;
    double min_dist = std::numeric_limits<double>::max();

    std::size_t i0 = INVALID_INDEX;
    std::size_t i1 = INVALID_INDEX;
    std::size_t i2 = INVALID_INDEX;

    // pick a seed point close to the centroid
    for (std::size_t i = 0; i < n; i++) {
        const double d = dist(cx, cy, coords[2 * i], coords[2 * i + 1]);
        if (d < min_dist) {
            i0 = i;
            min_dist = d;
        }
    }

    const double i0x = coords[2 * i0];
    const double i0y = coords[2 * i0 + 1];

    min_dist = std::numeric_limits<double>::max();

    // find the point closest to the seed
    for (std::size_t i = 0; i < n; i++) {
        if (i == i0) continue;
        const double d = dist(i0x, i0y, coords[2 * i], coords[2 * i + 1]);
        if (d < min_dist && d > 0.0) {
            i1 = i;
            min_dist = d;
        }
    }

    double i1x = coords[2 * i1];
    double i1y = coords[2 * i1 + 1];

    double min_radius = std::numeric_limits<double>::max();

    // find the third point which forms the smallest circumcircle with the first two
    for (std::size_t i = 0; i < n; i++) {
        if (i == i0 || i == i1) continue;

        const double r = circumradius(
            i0x, i0y, i1x, i1y, coords[2 * i], coords[2 * i + 1]);

        if (r < min_radius) {
            i2 = i;
            min_radius = r;
        }
    }

    if (!(min_radius < std::numeric_limits<double>::max())) {
        triangles.clear();
        return;
        // throw std::runtime_error("not triangulation");
    }

    double i2x = coords[2 * i2];
    double i2y = coords[2 * i2 + 1];

    if (orient(i0x, i0y, i1x, i1y, i2x, i2y)) {
        std::swap(i1, i2);
        std::swap(i1x, i2x);
        std::swap(i1y, i2y);
    }

    std::tie(m_center_x, m_center_y) = circumcenter(i0x, i0y, i1x, i1y, i2x, i2y);

    // sort the points by distance from the seed triangle circumcenter
    std::sort(ids.begin(), ids.end(), compare{ coords, m_center_x, m_center_y });

    // initialize a hash table for storing edges of the advancing convex hull
    m_hash_size = static_cast<std::size_t>(std::llround(std::ceil(std::sqrt(n))));
    m_hash.resize(m_hash_size);
    std::fill(m_hash.begin(), m_hash.end(), INVALID_INDEX);

    // initialize arrays for tracking the edges of the advancing convex hull
    hull_prev.resize(n);
    hull_next.resize(n);
    hull_tri.resize(n);

    hull_start = i0;

    size_t hull_size = 3;

    hull_next[i0] = hull_prev[i2] = i1;
    hull_next[i1] = hull_prev[i0] = i2;
    hull_next[i2] = hull_prev[i1] = i0;

    hull_tri[i0] = 0;
    hull_tri[i1] = 1;
    hull_tri[i2] = 2;

    m_hash[hash_key(i0x, i0y)] = i0;
    m_hash[hash_key(i1x, i1y)] = i1;
    m_hash[hash_key(i2x, i2y)] = i2;

    std::size_t max_triangles = n < 3 ? 1 : 2 * n - 5;
    triangles.reserve(max_triangles * 3);
    halfedges.reserve(max_triangles * 3);
    add_triangle(i0, i1, i2, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX);
    double xp = std::numeric_limits<double>::quiet_NaN();
    double yp = std::numeric_limits<double>::quiet_NaN();
    for (std::size_t k = 0; k < n; k++) {
        const std::size_t i = ids[k];
        const double x = coords[2 * i];
        const double y = coords[2 * i + 1];

        // skip near-duplicate points
        if (k > 0 && check_pts_equal(x, y, xp, yp)) continue;
        xp = x;
        yp = y;

        // skip seed triangle points
        if (
            check_pts_equal(x, y, i0x, i0y) ||
            check_pts_equal(x, y, i1x, i1y) ||
            check_pts_equal(x, y, i2x, i2y)) continue;

        // find a visible edge on the convex hull using edge hash
        std::size_t start = 0;

        size_t key = hash_key(x, y);
        for (size_t j = 0; j < m_hash_size; j++) {
            start = m_hash[fast_mod(key + j, m_hash_size)];
            if (start != INVALID_INDEX && start != hull_next[start]) break;
        }

        start = hull_prev[start];
        size_t e = start;
        size_t q;

        while (q = hull_next[e], !orient(x, y, coords[2 * e], coords[2 * e + 1], coords[2 * q], coords[2 * q + 1])) { //TODO: does it works in a same way as in JS
            e = q;
            if (e == start) {
                e = INVALID_INDEX;
                break;
            }
        }

        if (e == INVALID_INDEX) continue; // likely a near-duplicate point; skip it

        // add the first triangle from the point
        std::size_t t = add_triangle(
            e,
            i,
            hull_next[e],
            INVALID_INDEX,
            INVALID_INDEX,
            hull_tri[e]);

        hull_tri[i] = legalize(t + 2);
        hull_tri[e] = t;
        hull_size++;

        // walk forward through the hull, adding more triangles and flipping recursively
        std::size_t next = hull_next[e];
        while (
            q = hull_next[next],
            orient(x, y, coords[2 * next], coords[2 * next + 1], coords[2 * q], coords[2 * q + 1])) {
            t = add_triangle(next, i, q, hull_tri[i], INVALID_INDEX, hull_tri[next]);
            hull_tri[i] = legalize(t + 2);
            hull_next[next] = next; // mark as removed
            hull_size--;
            next = q;
        }

        // walk backward from the other side, adding more triangles and flipping
        if (e == start) {
            while (
                q = hull_prev[e],
                orient(x, y, coords[2 * q], coords[2 * q + 1], coords[2 * e], coords[2 * e + 1])) {
                t = add_triangle(q, i, e, INVALID_INDEX, hull_tri[e], hull_tri[q]);
                legalize(t + 2);
                hull_tri[q] = t;
                hull_next[e] = e; // mark as removed
                hull_size--;
                e = q;
            }
        }

        // update the hull indices
        hull_prev[i] = e;
        hull_start = e;
        hull_prev[next] = i;
        hull_next[e] = i;
        hull_next[i] = next;

        m_hash[hash_key(x, y)] = i;
        m_hash[hash_key(coords[2 * e], coords[2 * e + 1])] = e;
    }
}

double Delaunator::get_hull_area() {
    std::vector<double> hull_area;
    size_t e = hull_start;
    do {
        hull_area.push_back((coords[2 * e] - coords[2 * hull_prev[e]]) * (coords[2 * e + 1] + coords[2 * hull_prev[e] + 1]));
        e = hull_next[e];
    } while (e != hull_start);
    return sum(hull_area);
}

std::size_t Delaunator::legalize(std::size_t a) {
    std::size_t i = 0;
    std::size_t ar = 0;
    m_edge_stack.clear();

    // recursion eliminated with a fixed-size stack
    int loop = 0;
    while (true) {
        const size_t b = halfedges[a];

        /* if the pair of triangles doesn't satisfy the Delaunay condition
        * (p1 is inside the circumcircle of [p0, pl, pr]), flip them,
        * then do the same check/flip recursively for the new pair of triangles
        *
        *           pl                    pl
        *          /||\                  /  \
        *       al/ || \bl            al/    \a
        *        /  ||  \              /      \
        *       /  a||b  \    flip    /___ar___\
        *     p0\   ||   /p1   =>   p0\---bl---/p1
        *        \  ||  /              \      /
        *       ar\ || /br             b\    /br
        *          \||/                  \  /
        *           pr                    pr
        */
        const size_t a0 = 3 * (a / 3);
        ar = a0 + (a + 2) % 3;

        if (b == INVALID_INDEX) {
            if (i > 0) {
                i--;
                a = m_edge_stack[i];
                continue;
            } else {
                //i = INVALID_INDEX;
                break;
            }
        }

        const size_t b0 = 3 * (b / 3);
        const size_t al = a0 + (a + 1) % 3;
        const size_t bl = b0 + (b + 2) % 3;

        const std::size_t p0 = triangles[ar];
        const std::size_t pr = triangles[a];
        const std::size_t pl = triangles[al];
        const std::size_t p1 = triangles[bl];

        const bool illegal = in_circle(
            coords[2 * p0],
            coords[2 * p0 + 1],
            coords[2 * pr],
            coords[2 * pr + 1],
            coords[2 * pl],
            coords[2 * pl + 1],
            coords[2 * p1],
            coords[2 * p1 + 1]);

        if (illegal) {
            triangles[a] = p1;
            triangles[b] = p0;

            auto hbl = halfedges[bl];

            // edge swapped on the other side of the hull (rare); fix the halfedge reference
            if (hbl == INVALID_INDEX) {
                std::size_t e = hull_start;
                do {
                    if (hull_tri[e] == bl) {
                        hull_tri[e] = a;
                        break;
                    }
                    e = hull_next[e];
                    if (++loop > 10000) throw std::runtime_error("infinity loop");  // Delaunator bug!?
                } while (e != hull_start);
            }
            link(a, hbl);
            link(b, halfedges[ar]);
            link(ar, bl);
            std::size_t br = b0 + (b + 1) % 3;

            if (i < m_edge_stack.size()) {
                m_edge_stack[i] = br;
            } else {
                m_edge_stack.push_back(br);
            }
            i++;

        } else {
            if (i > 0) {
                i--;
                a = m_edge_stack[i];
                continue;
            } else {
                break;
            }
        }
    }
    return ar;
}

inline std::size_t Delaunator::hash_key(const double x, const double y) const {
    const double dx = x - m_center_x;
    const double dy = y - m_center_y;
    return fast_mod(
        static_cast<std::size_t>(std::llround(std::floor(pseudo_angle(dx, dy) * static_cast<double>(m_hash_size)))),
        m_hash_size);
}

std::size_t Delaunator::add_triangle(
    std::size_t i0,
    std::size_t i1,
    std::size_t i2,
    std::size_t a,
    std::size_t b,
    std::size_t c) {
    std::size_t t = triangles.size();
    triangles.push_back(i0);
    triangles.push_back(i1);
    triangles.push_back(i2);
    link(t, a);
    link(t + 1, b);
    link(t + 2, c);
    return t;
}

void Delaunator::link(const std::size_t a, const std::size_t b) {
    std::size_t s = halfedges.size();
    if (a == s) {
        halfedges.push_back(b);
    } else if (a < s) {
        halfedges[a] = b;
    } else {
        throw std::runtime_error("Cannot link edge");
    }
    if (b != INVALID_INDEX) {
        std::size_t s2 = halfedges.size();
        if (b == s2) {
            halfedges.push_back(a);
        } else if (b < s2) {
            halfedges[b] = a;
        } else {
            throw std::runtime_error("Cannot link edge");
        }
    }
}

} //namespace delaunator

using namespace std;

struct Point {
  int x, y;
  bool operator<(const Point& rhs) const {
    return x < rhs.x || (x == rhs.x && y < rhs.y);
  }
  Point& operator+= (const Point& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }
  Point& operator-= (const Point& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }
  Point operator+ (const Point& rhs) const {
    return Point{x + rhs.x, y + rhs.y};
  }
  Point operator- (const Point& rhs) const {
    return Point{x - rhs.x, y - rhs.y};
  }
  inline int norm() const {
    return x * x + y * y;
  }
  complex<double> to_complex() const {
    return complex<double>(x, y);
  }
};

inline int dot(Point a, Point b) {
  return a.x * b.x + a.y * b.y;
}
    
inline int cross(Point a, Point b) {
  return a.x * b.y - a.y * b.x;
}

inline int norm(Point a) {
  return dot(a, a);
}

constexpr int MAX_N = 20;
constexpr int MAX_K = MAX_N * 4;
int N, M, K;
Point X[MAX_N * 51 + 1];
double P[MAX_K][MAX_N];

constexpr int source_index = MAX_N * 51;

static inline __attribute__((always_inline))
int ccw(int ai, int bi, int ci) {
  Point b = {X[bi].x - X[ai].x, X[bi].y - X[ai].y};
  Point c = {X[ci].x - X[ai].x, X[ci].y - X[ai].y};
  int cross_product = cross(b, c);
  if (cross_product > 0) return 1;    // counter-clockwise
  if (cross_product < 0) return -1;   // clockwise
  // if (dot(b, c) < 0) return 2;        // c--a--b
  // if (b.norm() < c.norm()) return -2; // a--b--c
  return 0;                           // a--c--b
}

struct L { int a, b; };

static inline __attribute__((always_inline)) bool isis_ss(L s, L t) {
  if (s.a == t.a || s.a == t.b || s.b == t.a || s.b == t.b) return false;
  // if (max(X[s.a].x, X[s.b].x) < min(X[t.a].x, X[t.b].x) ||
  //     max(X[t.a].x, X[t.b].x) < min(X[s.a].x, X[s.b].x) ||
  //     max(X[s.a].y, X[s.b].y) < min(X[t.a].y, X[t.b].y) ||
  //     max(X[t.a].y, X[t.b].y) < min(X[s.a].y, X[s.b].y)) {
  //   return false;
  // }
  return ccw(s.a, s.b, t.a) * ccw(s.a, s.b, t.b) <= 0 &&
         ccw(t.a, t.b, s.a) * ccw(t.a, t.b, s.b) <= 0;
}

vector<int> ConvexHull(vector<int> ps) {
  if (ps.size() <= 2) return ps;
  int n = ps.size();
  int k = 0;
  std::sort(ps.begin(), ps.end(), [](int l, int r) { return X[l].x < X[r].x; });
  vector<int> ch(2 * n);
  for (int i = 0; i < n; ch[k++] = ps[i++])
    while (k >= 2 && ccw(ch[k - 2], ch[k - 1], ps[i]) <= 0) --k;
  for (int i = n - 2, t = k + 1; i >= 0; ch[k++] = ps[i--])
    while (k >= t && ccw(ch[k - 2], ch[k - 1], ps[i]) <= 0) --k;
  ch.resize(k - 1);
  return ch;
}

class SimulatedAnnealing {
public:
  constexpr static int LOG_SIZE = 0x10000;
  constexpr static int UPDATE_INTERVAL = 0xFF;
  constexpr static double START_TEMP = 0.00150;
  constexpr static double END_TEMP   = 0.00001;
  const int64_t CYCLES;
  // const double TEMP_RATIO = (END_TEMP - START_TEMP) / CYCLES;  // linear
  const double TEMP_RATIO; // = log(END_TEMP / START_TEMP) / CYCLES;  // exponential
  double logp[LOG_SIZE];
  long long iteration = 0;
  int64_t cycle = 0;
  int64_t start = timer.get_cycle();
  const int64_t medium;
  const int64_t stop;
  bool record = false;
  double temp = START_TEMP;

  SimulatedAnnealing(int64_t n_cycles) :
      CYCLES(n_cycles),
      TEMP_RATIO(log(END_TEMP / START_TEMP) / CYCLES),
      medium(start + CYCLES * 0.10),
      stop(start + CYCLES)
  {
    constexpr double inv = 1.0 / LOG_SIZE;
    REP(i,LOG_SIZE) logp[i] = log((i + 0.5) * inv);
    std::mt19937 rng;
    std::shuffle(logp, logp + LOG_SIZE, rng);
  }

  inline bool End() {
    iteration++;
    if ((iteration & UPDATE_INTERVAL) != 0) return false;
    cycle = timer.get_cycle();
    // temp = START_TEMP + (cycle - start) * TEMP_RATIO;  // linear
    temp = START_TEMP * exp((cycle - start) * TEMP_RATIO);  // exponential
    return cycle >= stop;
  }

  inline bool Accept(double diff) {
    static unsigned short index = 0;
    return diff >= 0 || diff > logp[index++] * temp;
  }
};

void SetUp() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout << fixed << setprecision(15);

  cin >> N >> M >> K;
  REP(i,N+M) cin >> X[i].x >> X[i].y;
  REP(i,K) REP(j,N) cin >> P[i][j];
  LOG(N);
  LOG(M);
  LOG(K);

  X[source_index] = {0, 5000};
}

int lbit[MAX_K], rbit[MAX_K];
float slopep[MAX_K][MAX_N], slopep_neg[MAX_K][MAX_N];
float dp[1 << MAX_N];
int dp2[1 << MAX_N];
float dp3[1 << MAX_N][MAX_N];

void DFS(int bit) {
  if (dp2[bit] >= 0) return;  // Already computed
  int popcount = __builtin_popcount(bit);
  if (popcount <= 1) {
    dp[bit] = 1.0;
    dp2[bit] = __builtin_ctz(bit);
    dp3[bit][__builtin_ctz(bit)] = 1.0;
    return;
  } else if (popcount == 2) {
    int g1 = __builtin_ctz(bit);
    int g2 = __builtin_ctz(bit & ~(1 << g1));
    REP(i,K) {
      if (P[i][g1] < P[i][g2]) swap(g1, g2);
      double s = slopep[i][g1] + slopep_neg[i][g2];
      if (s > dp[bit]) {
        dp[bit] = s;
        dp2[bit] = i;
      }
    }
    {
      int i = dp2[bit];
      if (P[i][g1] < P[i][g2]) swap(g1, g2);
      dp3[bit][g1] = slopep[i][g1];
      dp3[bit][g2] = slopep_neg[i][g2];
    }
  }
  REP(i,K) {
    int bit1 = lbit[i] & bit;
    int bit2 = rbit[i] & bit;
    if (bit1 == 0 || bit2 == 0) continue;  // No split
    DFS(bit1);
    DFS(bit2);

    double s = 0;
    REP(j,N) {
      if ((bit1 >> j) & 1) {
        s += dp3[bit1][j] * slopep[i][j];
      } else if ((bit2 >> j) & 1) {
        s += dp3[bit2][j] * slopep_neg[i][j];
      }
    }
    if (s > dp[bit]) {
      dp[bit] = s;
      dp2[bit] = i;
    }
  }
  if (dp2[bit] < 0) {
    dp2[bit] = 0;
  } else {
    int i = dp2[bit];
    int bit1 = lbit[i], bit2 = rbit[i];
    bit1 &= bit;
    bit2 &= bit;
    REP(j,N) {
      if ((bit1 >> j) & 1) {
        dp3[bit][j] = dp3[bit1][j] * slopep[i][j];
      } else if ((bit2 >> j) & 1) {
        dp3[bit][j] = dp3[bit2][j] * slopep_neg[i][j];
      }
    }
  }
}

constexpr double num_garbage_param = 0.900;  // Param

double num_garbage_ratio[1 << MAX_N];
double num_garbage[1 << MAX_N];

int DFS2(int garbage) {
  int garbage1 = -1, garbage2 = -1;
  int machine_id = dp2[garbage];
  int popcount = __builtin_popcount(garbage);
  if (popcount == 1) {
    return 0;
  } else if (popcount == 2) {
    int g1 = __builtin_ctz(garbage);
    int g2 = __builtin_ctz(garbage & ~(1 << g1));
    if (P[machine_id][g1] < P[machine_id][g2]) swap(g1, g2);
    garbage1 = (1 << g1);
    garbage2 = (1 << g2);
  } else {
    garbage1 = garbage & lbit[machine_id];
    garbage2 = garbage & rbit[machine_id];
  }
  double s1 = DFS2(garbage1);
  double s2 = DFS2(garbage2);

  num_garbage_ratio[garbage] = std::pow(double(popcount), num_garbage_param) + s1 + s2;
  return num_garbage_ratio[garbage] = std::pow(double(popcount), num_garbage_param) + s1 + s2;
}

void FindBestMachine() {
  memset(dp2, -1, sizeof(dp2));
  REP(i,K) REP(j,N) {
    if (P[i][j] >= 0.5) {
      lbit[i] |= (1 << j);
    } else {
      rbit[i] |= (1 << j);
    }
  }

  auto slope = [](double x) { return x * x * (3 - 2 * x); };  // TODO: Param Function
  REP(i,K) REP(j,N) {
    slopep[i][j] = slope(slope(P[i][j]));
    slopep_neg[i][j] = slope(slope(1.0 - P[i][j]));
  }

  DFS((1 << N) - 1);

  double sum_ratio = DFS2((1 << N) - 1);
  REP(bit,(1<<N)-1) {
    // int popcount = __builtin_popcount(bit);
    num_garbage[bit] = num_garbage_ratio[bit] / sum_ratio * (N + M);
  }
}

vector<int> GetInitPoints() {
  vector<int> ps;
  REP(i,N+M) ps.push_back(i);
  vector<int> ch = ConvexHull(ps);

  int n = ch.size();
  vector<pair<int, int>> vec;
  for (int i = 0; i < n; ++i) {
    bool ok = true;
    for (int j = 0; j < n; ++j) {
      if (isis_ss(L{ch[j], ch[(j + 1) % n]}, L{ch[i], source_index})) {
        ok = false;
        break;
      }
    }
    if (ok && ch[i] >= N) {
      int min_norm = 1e9;
      REP(j,N) chmin(min_norm, norm(X[ch[i]] - X[j]));
      vec.emplace_back(-min_norm, ch[i]);
    }
  }
  sort(ALL(vec));
  vector<int> res(vec.size());
  REP(i,vec.size()) res[i] = vec[i].second;

  // UNSAFE POINT
  int nearest = 1e9;
  REP(i,N) chmin(nearest, norm(X[i] - X[source_index]));
  vector<int> ps2;
  REP(i,M) {
    if (norm(X[i + N] - X[source_index]) < nearest) ps2.push_back(i + N);
  }

  if (ps2.empty()) {
    res.push_back(-1);
    int mind = 1e9;
    REP(i,M) {
      int d = norm(X[N + i] - X[source_index]);
      if (d < mind) {
        mind = d;
        res.back() = N + i;
      }
    }
    return res;
  }

  if (ps2.size() > 3) ps2 = ConvexHull(ps2);
  nearest = 1e9;
  res.push_back(-1);
  for (int i : ps2) {
    int d = norm(X[i] - X[source_index]);
    if (d < nearest) {
      nearest = d;
      res.back() = i;
    }
  }
  return res;
}

struct Task {
  int src = -1;
  int dest1 = -1, dest2 = -1;
  int machine_id = -1;
  int garbage1 = 0, garbage2 = 0;
  vector<int> targets;
  vector<vector<int>> dests;
  vector<pair<int, int>> edge;
  double init1, init2;
  double ratio1, ratio2;
  vector<int> machine_ids;
};

vector<Task> GenerateTasks(
    int root,
    int garbage,
    const vector<int>& targets,
    vector<int>& garbage_mapping) {
  int popcount = __builtin_popcount(garbage);
  assert(popcount >= 2);
  Task task;
  task.src = root;
  task.machine_id = dp2[garbage];
  assert(task.machine_id >= 0);
  if (popcount == 2) {
    int g1 = __builtin_ctz(garbage);
    int g2 = __builtin_ctz(garbage & ~(1 << g1));
    if (P[task.machine_id][g1] < P[task.machine_id][g2]) swap(g1, g2);
    task.garbage1 = (1 << g1);
    task.garbage2 = (1 << g2);
  } else {
    task.garbage1 = garbage & lbit[task.machine_id];
    task.garbage2 = garbage & rbit[task.machine_id];
  }

  int c1 = __builtin_popcount(task.garbage1);
  int c2 = __builtin_popcount(task.garbage2);
  assert(c1 > 0);
  assert(c2 > 0);
  // LOG(task.machine_id + 1);
  // LOG(c1);
  // LOG(c2);
  bool swapped = rnd.next(2);  // TODO: Use swapped
  if (swapped) {
    swap(c1, c2);
    swap(task.garbage1, task.garbage2);
  }

  vector<pair<double, int>> vec1;
  double angle;
  Vec center;
  {
    Vec base_vec = ((X[targets[0]] - X[root]) + (X[targets[1]] - X[root])).to_complex();
    double angle_diff = std::arg(base_vec);
    double max_len = 0;
    for (int t: targets) {
      if (t >= N) continue;
      Vec v = (X[t] - X[root]).to_complex();
      double arg = std::arg(v) - angle_diff;
      if (arg < -PI) arg += 2 * PI; else if (arg >= PI) arg -= 2 * PI;
      vec1.emplace_back(arg, t);
      chmax(max_len, abs(v));
    }
    // cerr << vec1 << endl;
    // CERR() << vec1 << " " << c1 << " " << c2 << endl;
    // assert(int(vec1.size()) == c1 + c2);
    if (int(vec1.size()) != c1 + c2) return {};
    std::nth_element(vec1.begin(), vec1.begin() + (c1 - 1), vec1.end(), [](auto l, auto r) { return l.first < r.first; });
    double angle1 = vec1[c1 - 1].first;
    double angle2 = std::min_element(vec1.begin() + c1, vec1.end())->first;
    angle = rnd.next(angle1, angle2) + arg(base_vec);
    double len = c1 > 1 && c2 > 1 ? rnd.next(0.1, 0.7) * max_len : rnd.next(2e4, 3e4);
    center = std::polar(len, angle) + X[root].to_complex();
  }
  // LOG(angle);
  // LOG(center);

  vector<pair<double, int>> vec;
  for (int t: targets) {
    if (t == root) continue;
    Vec v = X[t].to_complex() - center;
    double arg = std::arg(-v) - angle;
    if (arg < -PI) arg += 2 * PI; else if (arg >= PI) arg -= 2 * PI;
    vec.emplace_back(arg, t);
  }
  std::sort(ALL(vec), [](auto l, auto r) { return l.first < r.first; });
  // cerr << vec << endl;
  int r = lower_bound(ALL(vec), make_pair(0.0, 0)) - vec.begin();
  int l = r - 1;

  // LOG(l);
  // LOG(r);
  vector<int> targets1, targets2;
  int num_garbage1 = num_garbage[task.garbage1];
  int num_garbage2 = num_garbage[task.garbage2];

  if (c1 == 1) {
    r = vec.size() - 1;
    task.dest1 = vec1.front().second;
    garbage_mapping[task.dest1] = __builtin_ctz(task.garbage1);
  } else {
    assert (r < int(vec.size()));
    if (vec[r].second < N) return {};  // Retry
    if (vec[r].first >= 0.49 * PI) return {};
    while (r + 1 + num_garbage1 < int(vec.size()) && vec[r + 1].first < 0.49 * PI && vec[r + 1].second >= N) ++r;  // TODO: Param
    for (int i = r; i < int(vec.size()); ++i) {
      targets1.push_back(vec[i].second);
    }
    task.dest1 = vec[r].second;
  }

  if (c2 == 1) {
    l = 0;
    task.dest2 = vec1.back().second;
    garbage_mapping[task.dest2] = __builtin_ctz(task.garbage2);
  } else {
    assert (0 <= l);
    if (vec[l].second < N) return {};  // Retry
    if (vec[l].first <= -0.49 * PI) return {};
    while (l - num_garbage2 >= 0 && vec[l - 1].first > -0.49 * PI && vec[l - 1].second >= N) --l;  // TODO: Param
    for (int i = 0; i <= l; ++i) {
      targets2.push_back(vec[i].second);
    }
    task.dest2 = vec[l].second;
  }

  task.targets = {root};
  for (int i = l; i <= r; ++i) {
    task.targets.push_back(vec[i].second);
  }

  if (swapped) {
    swap(c1, c2);
    swap(task.garbage1, task.garbage2);
    swap(task.dest1, task.dest2);
    swap(targets1, targets2);
  }

  vector<Task> res1, res2;
  vector<Task> res;
  res.reserve(c1 + c2 + 1);
  constexpr int n_retry = 5;  // TODO: Param
  if (c1 > 1) {
    REP(i_retry, n_retry) {
      res1 = GenerateTasks(task.dest1, task.garbage1, targets1, garbage_mapping);
      if (!res1.empty()) break;
    }
    if (res1.empty()) return {};  // Retry
  }
  if (c2 > 1) {
    REP(i_retry, n_retry) {
      res2 = GenerateTasks(task.dest2, task.garbage2, targets2, garbage_mapping);
      if (!res2.empty()) break;
    }
    if (res2.empty()) return {};  // Retry
  }
  res.push_back(std::move(task));
  for (auto&& t : res1) res.push_back(std::move(t));
  for (auto&& t : res2) res.push_back(std::move(t));
  return res;
}

constexpr int MAX_T = 1024;

void Solve1(int task_id, Task& task, vector<vector<double>>& prob) {
  const int T = task.targets.size() - 2;
  const int src_index = 0;
  const int dest1_index = T;
  const int dest2_index = T + 1;
  vector<vector<int>> dests(task.targets.size());

  // Topolocail sort
  {
    vector<double> coords;
    coords.reserve(task.targets.size() * 2);
    for (int t : task.targets) {
      coords.push_back(X[t].x);
      coords.push_back(X[t].y);
    }
    delaunator::Delaunator d(coords);

    vector<vector<int>> next(task.targets.size());
    for (size_t i = 0; i < d.triangles.size(); i += 3) {
      int a = d.triangles[i];
      int b = d.triangles[i + 1];
      int c = d.triangles[i + 2];
      next[a].push_back(b); next[b].push_back(a);
      next[b].push_back(c); next[c].push_back(b);
      next[c].push_back(a); next[a].push_back(c);
    }
    REP(i,task.targets.size()) {
      sort(ALL(next[i]));
      next[i].erase(std::unique(ALL(next[i])), next[i].end());
      assert(next[i].size() >= 2);
    }
    int far_dist = -1, far_index = -1;
    REP(i,task.targets.size()) {
      int d = norm(X[task.targets[i]] - X[task.src]);
      if (d > far_dist) {
        far_dist = d;
        far_index = i;
      }
    }

    vector<bool> visited(task.targets.size(), false);
    static priority_queue<pair<int, int>> que1, que2;
    vector<int> order;
    auto push = [&](auto& que, int v) -> bool {
      if (visited[v]) return false;
      visited[v] = true;
      for (int u : next[v]) {
        int dist = norm(X[task.targets[u]] - X[task.src]);
        que.emplace(dist, u);
      }
      order.push_back(v);
      return true;
    };
    int dest1 = std::find(ALL(task.targets), task.dest1) - task.targets.begin();
    int dest2 = std::find(ALL(task.targets), task.dest2) - task.targets.begin();
    push(que1, dest1);
    push(que2, dest2);
    bool que1_done = false, que2_done = false;
    while (!que1_done && !que2_done) {
      if (!que1.empty() && !que1_done) {
        auto [dist, v] = que1.top();
        if (v == far_index) {
          que1_done = true;
        } else {
          que1.pop();
          if (!push(que1, v)) que1_done = true;
        }
      }
      if (!que2.empty() && !que2_done) {
        auto [dist, v] = que2.top();
        if (v == far_index) {
          que2_done = true;
        } else {
          que2.pop();
          if (!push(que2, v)) que2_done = true;
        }
      }
    }
    while (!que2.empty()) {
      que1.push(que2.top());
      que2.pop();
    }
    while (!que1.empty()) {
      auto [dist, v] = que1.top();
      que1.pop();
      push(que1, v);
    }

    std::reverse(ALL(order));

    vector<int> new_targets;
    new_targets.reserve(task.targets.size());
    for (int v: order) {
      new_targets.push_back(task.targets[v]);
    }
    // LOG(new_targets);
    task.targets = new_targets;

    if (task.targets[dest1_index] != task.dest1) {
      swap(task.targets[dest1_index], task.targets[dest2_index]);
      swap(order[dest1_index], order[dest2_index]);
    }
    assert(task.targets[src_index] == task.src);
    assert(task.targets[dest1_index] == task.dest1);
    assert(task.targets[dest2_index] == task.dest2);

    int order_rev[MAX_T];
    REP(i, order.size()) order_rev[order[i]] = i;
    REP(i,order.size()) {
      int from = order[i];
      for (int to : next[from]) {
        int j = order_rev[to];
        if (i < j) dests[i].push_back(j);
      }
    }
    // LOG(task.targets);
    // LOG(dests);
  }

  // Initial state
  pair<int, int> edge[MAX_T];
  std::fill(edge, edge + T, make_pair(-1, -1));
  // LOG(task.src);
  // LOG(task.dest1);
  // LOG(task.dest2);
  // CERR() << task.targets << endl;

  assert(task.src >= N);
  assert(__builtin_popcount(task.garbage1) >= 1);
  assert(__builtin_popcount(task.garbage2) >= 1);

  REP(i,T) {
    assert (!dests[i].empty());
    edge[i] = {dests[i][0], dests[i][0]};
  }

  double init1 = 1.0, init2 = 1.0;
  double ratio1 = 0.6, ratio2 = 0.4;
  if (__builtin_popcount(task.garbage1) == 1 && false) {  // TODO: Param
    int g = __builtin_ctz(task.garbage1);
    init1 = prob[task.src][g];
    ratio1 = P[task.machine_id][g];
  } else {
    ratio1 = 1;
    for (int g = 0; g < N; ++g) {
      if (((task.garbage1 >> g) & 1) == 0) continue;
      ratio1 = min(ratio1, P[task.machine_id][g]);
    }
  }
  if (__builtin_popcount(task.garbage2) == 1 && false) {  // TODO: Param
    int g = __builtin_ctz(task.garbage2);
    init2 = prob[task.src][g];
    ratio2 = P[task.machine_id][g];
  } else {
    ratio2 = 0;
    for (int g = 0; g < N; ++g) {
      if (((task.garbage2 >> g) & 1) == 0) continue;
      ratio2 = max(ratio2, P[task.machine_id][g]);
    }
  }

  pair<double, double> fwd[MAX_T], bwd[MAX_T];
  std::fill(fwd, fwd + T + 2, make_pair(1.0, 1.0));
  std::fill(bwd, bwd + T + 2, make_pair(1.0, 1.0));
  fwd[src_index] = { init1, init2 };
  bwd[dest1_index] = { 1.0, 0.0 };
  bwd[dest2_index] = { 0.0, 1.0 };
  auto backward = [&](){
    for (int i = T - 1; i >= 0; --i) {
      double s1_max = 0, s2_max = 0;
      for (int j : dests[i]) {
        constexpr double eps = 1e-6;  // TODO: Param
        double p1 = (fwd[i].first + eps) * bwd[j].first;
        double p2 = (fwd[i].second + eps) * bwd[j].second;
        double s1 = p1 * ratio1 + p2 * ratio2;
        double s2 = p2 * (1 - ratio2) + p1 * (1 - ratio1);
        // double s1 = sqrt(p1 * ratio1) + sqrt(p2 * ratio2);
        // double s2 = sqrt(p2 * (1 - ratio2)) + sqrt(p1 * (1 - ratio1));
        if (s1 > s1_max) {
          s1_max = s1;
          edge[i].first = j;
        }
        if (s2 > s2_max) {
          s2_max = s2;
          edge[i].second = j;
        }
      }
      bwd[i] = {
        bwd[edge[i].first].first * ratio1 + bwd[edge[i].second].first * (1 - ratio1),
        bwd[edge[i].first].second * ratio2 + bwd[edge[i].second].second * (1 - ratio2)
      };
    }
  };
  auto forward = [&](){
    std::fill(fwd + 1, fwd + T + 2, make_pair(0.0, 0.0));
    REP(i,T) {
      fwd[edge[i].first].first += fwd[i].first * ratio1;
      fwd[edge[i].second].first += fwd[i].first * (1 - ratio1);
      fwd[edge[i].second].second += fwd[i].second * (1 - ratio2);
      fwd[edge[i].first].second += fwd[i].second * ratio2;
    }
  };

  backward();
  constexpr int n_changes = 2;  // TODO: Param
  REP(lp,100) {
    if (T >= 3) {
      int i = rnd.next(1, T - 2);
      if (edge[i].first != i + 1 && edge[i].second != i + 1) {
        swap(edge[i], edge[i + 1]);
        swap(task.targets[i], task.targets[i + 1]);
        swap(dests[i], dests[i + 1]);
        REP(j,i) {
          if (edge[j].first == i) edge[j].first = i + 1;
          else if (edge[j].first == i + 1) edge[j].first = i;
          if (edge[j].second == i) edge[j].second = i + 1;
          else if (edge[j].second == i + 1) edge[j].second = i;
          for (int& x: dests[j]) {
            if (x == i) x = i + 1;
            else if (x == i + 1) x = i;
          }
        }
        auto it = std::find(ALL(dests[i + 1]), i + 1);
        if (it != dests[i + 1].end()) {
          dests[i + 1].erase(it);
          dests[i].push_back(i + 1);
        }
      }
    }

    REP(loop,n_changes) {
      int from = rnd.next(T);
      int to = dests[from][rnd.next(dests[from].size())];
      if (rnd.next(2)) {
        edge[from].first = to;
      } else {
        edge[from].second = to;
      }
    }

    forward();
    backward();
  }
  // LOG(dests);
  // LOG(edge);
  // LOG(fwd);
  // LOG(bwd);
  task.dests = dests;
  task.edge.resize(T);
  std::copy(edge, edge + T, task.edge.begin());
  task.init1 = init1; task.ratio1 = ratio1;
  task.init2 = init2; task.ratio2 = ratio2;

  // Write
  auto best_edge = edge;
  vector<vector<double>> p(T + 2, vector<double>(N, 0.0));
  auto sim = [&]() {
    p[0] = prob[task.src];
    for (int i = 1; i < T + 2; ++i) {
      std::fill(ALL(p[i]), 0.0);
    }
    const double* PP = P[task.machine_id];
    REP(i,T) REP(j,N) {
      p[best_edge[i].first][j] += p[i][j] * PP[j];
      p[best_edge[i].second][j] += p[i][j] * (1.0 - PP[j]);
    }
  };

  sim();
  prob[task.dest1] = p[dest1_index];
  prob[task.dest2] = p[dest2_index];
}

int solve2_iter = 0;
int failmem[MAX_T][MAX_T];

inline double diff1(int from, int to, const pair<double, double>* __restrict fwd, const pair<double, double>* __restrict bwd, const pair<int,int>* __restrict edge, double r1, double r2) {
  return fwd[from].first * (bwd[to].first - bwd[edge[from].first].first) * r1
  + fwd[from].second * (bwd[to].second - bwd[edge[from].first].second) * r2;
}

inline double diff2(int from, int to, const pair<double, double>* __restrict fwd, const pair<double, double>* __restrict bwd, const pair<int,int>* __restrict edge, double r1, double r2) {
  return fwd[from].first * (bwd[to].first - bwd[edge[from].second].first) * r1
    + fwd[from].second * (bwd[to].second - bwd[edge[from].second].second) * r2;
}

void Solve2(int task_id, Task& task, vector<vector<double>>& prob, int64_t n_cycles) {
  // CERR() << "=== Solve task " << task_id << " ===" << endl;
  const int T = task.targets.size() - 2;
  // const int src_index = 0;  // Unused
  const int dest1_index = T;
  const int dest2_index = T + 1;
  double init1 = task.init1, ratio1 = task.ratio1;
  double init2 = task.init2, ratio2 = task.ratio2;

  vector<vector<int>> dests = task.dests;
  pair<int, int> edge[MAX_T];
  int targets[MAX_T], best_targets[MAX_T];
  std::copy(ALL(task.edge), edge);
  std::copy(ALL(task.targets), targets);
  // Lite simulation
  alignas(64) static pair<double, double> fwd[MAX_T], bwd[MAX_T];
  fwd[0] = {init1, init2};
  bwd[dest1_index] = { 1.0, 0.0 };
  bwd[dest2_index] = { 0.0, 1.0 };
  double ratio1_neg = 1 - ratio1, ratio2_neg = 1 - ratio2;
  auto forward = [&]() -> double {
    std::fill(fwd + 1, fwd + T + 2, make_pair(0.0, 0.0));
    REP(i,T) {
      fwd[edge[i].first].first += fwd[i].first * ratio1;  // TODO: Param
      fwd[edge[i].second].first += fwd[i].first * ratio1_neg;
      fwd[edge[i].second].second += fwd[i].second * ratio2_neg;
      fwd[edge[i].first].second += fwd[i].second * ratio2;
    }
    return fwd[dest1_index].first + fwd[dest2_index].second;
  };
  auto backward = [&](int updated) {
    for (int i = updated; i >= 0; --i) {
      bwd[i] = {
        bwd[edge[i].first].first * ratio1 + bwd[edge[i].second].first * ratio1_neg,
        bwd[edge[i].first].second * ratio2 + bwd[edge[i].second].second * ratio2_neg
      };
    }
  };

  // SA
  double cur_s = forward();
  backward(T - 1);
  // LOG(cur_s);
  double best_s = cur_s;
  pair<int, int> best_edge[1024];
  std::copy(edge, edge + T, best_edge);
  std::copy(targets, targets + T + 2, best_targets);
  SimulatedAnnealing sa(n_cycles);

  while (!sa.End()) {
    ++solve2_iter;
    // if (sa.iteration % 100000 == 0) {
    //   CERR() << "Iteration: " << sa.iteration << ", Temp: " << sa.temp << ", Score: " << cur_s << "\n";
    // }
    if (sa.cycle > sa.medium) break;
    if (T >= 3 && rnd.next() % 16 < 1) {  // TODO: Param
      int i = rnd.next(1, T - 2);
      if (edge[i].first == i + 1 || edge[i].second == i + 1) continue;
      swap(edge[i], edge[i + 1]);
      swap(targets[i], targets[i + 1]);
      swap(dests[i], dests[i + 1]);
      swap(fwd[i], fwd[i + 1]);
      swap(bwd[i], bwd[i + 1]);
      REP(j,i) {
        if (edge[j].first == i) edge[j].first = i + 1;
        else if (edge[j].first == i + 1) edge[j].first = i;
        if (edge[j].second == i) edge[j].second = i + 1;
        else if (edge[j].second == i + 1) edge[j].second = i;
        for (int& x: dests[j]) {
          if (x == i) x = i + 1;
          else if (x == i + 1) x = i;
        }
      }
      auto it = std::find(ALL(dests[i + 1]), i + 1);
      if (it != dests[i + 1].end()) {
        dests[i + 1].erase(it);
        dests[i].push_back(i + 1);
      }
      continue;
    }
    int i = rnd.next(T * 2);
    int j = i & 1;
    i >>= 1;
    int& e = *(j ? &edge[i].first : &edge[i].second);
    int next = dests[i][rnd.next(dests[i].size())];
    if (e == next) continue;  // No change

    double diff = j ? diff1(i, next, fwd, bwd, edge, ratio1, ratio2) : diff2(i, next, fwd, bwd, edge, ratio1_neg, ratio2_neg);
    // diff = next_s - cur_s;
    if (__builtin_expect(sa.Accept(diff), 0)) {
      swap(e, next);
      cur_s += diff;
      forward();
      backward(i);
      if (cur_s > best_s) {
        best_s = cur_s;
        std::copy(targets, targets + T + 2, best_targets);
        std::copy(edge, edge + T, best_edge);
      }
    }
  }

  while (!sa.End()) {
    ++solve2_iter;
    if (T >= 3 && rnd.next() % 64 < 10) {  // TODO: Param
      int i = rnd.next(1, T - 2);
      if (edge[i].first == i + 1 || edge[i].second == i + 1) continue;
      swap(edge[i], edge[i + 1]);
      swap(targets[i], targets[i + 1]);
      swap(fwd[i], fwd[i + 1]);
      swap(bwd[i], bwd[i + 1]);
      REP(j,i) {
        if (edge[j].first == i) edge[j].first = i + 1;
        else if (edge[j].first == i + 1) edge[j].first = i;
        if (edge[j].second == i) edge[j].second = i + 1;
        else if (edge[j].second == i + 1) edge[j].second = i;
      }
      continue;
    }

    int i = rnd.next(T * 2);
    int j = i & 1;
    i >>= 1;
    int& e = *(j ? &edge[i].first : &edge[i].second);
    int next = rnd.next(i + 1, T + 1);
    if (e == next) continue;  // No change

    double diff = j ? diff1(i, next, fwd, bwd, edge, ratio1, ratio2) : diff2(i, next, fwd, bwd, edge, ratio1_neg, ratio2_neg);
    if (__builtin_expect(sa.Accept(diff), 0)) {
      bool is_intersect = false;
      int& mem = failmem[targets[i]][targets[next]];
      mem %= T;
      // int mem = 0;
      L l1 = {targets[i], targets[next]};
      for (int ki = 0, k = mem; ki < T; ++ki, ++k) {
        if (k >= T) k = 0;
        if (k == i) continue;
        L l2 = {targets[k], targets[edge[k].first]};
        L l3 = {targets[k], targets[edge[k].second]};
        if (isis_ss(l1, l2) || isis_ss(l1, l3)) {
          mem = k;
          is_intersect = true;
          break;
        }
      }
      if (is_intersect) continue;

      swap(e, next);
      cur_s += diff;
      forward();
      backward(i);
      if (cur_s > best_s) {
        best_s = cur_s;
        std::copy(targets, targets + T + 2, best_targets);
        std::copy(edge, edge + T, best_edge);
      }
    }
  }

  // task.targets = best_targets;
  std::copy(best_targets, best_targets + T + 2, task.targets.begin());
  std::copy(best_edge, best_edge + T, task.edge.begin());
  // task.dests = dests;
  // LOG(edge);
  // LOG(cur_s);

  // Update probability
  vector<vector<double>> p(T + 2, vector<double>(N, 0.0));
  auto sim = [&]() {
    p[0] = prob[task.src];
    for (int i = 1; i < T + 2; ++i) {
      std::fill(ALL(p[i]), 0.0);
    }
    const double* PP = P[task.machine_id];
    REP(i,T) REP(j,N) {
      p[best_edge[i].first][j] += p[i][j] * PP[j];
      p[best_edge[i].second][j] += p[i][j] * (1.0 - PP[j]);
    }
  };

  sim();
  REP(i,T+2) prob[task.targets[i]] = p[i];
}

double fwd[MAX_T][MAX_N];
double bwd[MAX_T][MAX_N];

void Simulate(int task_id, const Task& task, vector<vector<double>>& prob1) {
  const int T = (int)task.targets.size() - 2;
  if (T <= 0) return;

  auto& targets = task.targets;

  std::copy(ALL(prob1[task.src]), fwd[0]);
  for (int i = 1; i < T + 2; ++i) REP(j,N) fwd[i][j]= 0.0;
  REP(i,T) {
    if (!timer.within()) return;
    REP(j,N) {
      int machine_id = task.machine_ids[i];
      fwd[task.edge[i].first][j] += fwd[i][j] * P[machine_id][j];
      fwd[task.edge[i].second][j] += fwd[i][j] * (1.0 - P[machine_id][j]);
    }
  }
  REP(i,T+2) std::copy(fwd[i], fwd[i] + N, prob1[task.targets[i]].begin());
}

void Solve3(int task_id, Task& task,
            vector<vector<double>>& prob1,
            vector<vector<double>>& prob2) {
  const int T = (int)task.targets.size() - 2;
  if (T <= 0) return;

  auto& targets = task.targets;
  auto& edge = task.edge;

  int machine_ids[MAX_T];
  std::copy(ALL(prob2[task.dest1]), bwd[T]);
  std::copy(ALL(prob2[task.dest2]), bwd[T + 1]);
  std::fill(machine_ids, machine_ids + T, task.machine_id);

  for (int i = T - 1; i >= 0; --i) {
    if (!timer.within()) break;
    double s1_max = 0, s2_max = 0;
    int machine_id = machine_ids[i];
    for (int j = i + 1; j < T + 2; ++j) {
      bool is_intersect = false;
      L l1 = {targets[i], targets[j]};
      REP(k,T) {
        if (k == i) continue;
        L l2 = {targets[k], targets[edge[k].first]};
        L l3 = {targets[k], targets[edge[k].second]};
        if (isis_ss(l1, l2) || isis_ss(l1, l3)) {
          is_intersect = true;
          break;
        }
      }
      if (is_intersect) continue;

      double s1 = 0, s2 = 0;
      REP(g,N) {
        s1 += prob1[task.targets[i]][g] * P[machine_id][g] * bwd[j][g];
        s2 += prob1[task.targets[i]][g] * (1 - P[machine_id][g]) * bwd[j][g];
      }
      if (s1 > s1_max) {
        s1_max = s1;
        edge[i].first = j;
      }
      if (s2 > s2_max) {
        s2_max = s2;
        edge[i].second = j;
      }
    }
    double s_max = s1_max + s2_max;
    REP(j,K) {
      double s = 0;
      REP(g,N) {
        s += prob1[task.targets[i]][g] * P[j][g] * bwd[edge[i].first][g];
        s += prob1[task.targets[i]][g] * (1 - P[j][g]) * bwd[edge[i].second][g];
      }
      if (s > s_max) {
        s_max = s;
        machine_id = j;
      }
    }
    machine_ids[i] = machine_id;
    REP(g,N) bwd[i][g] = bwd[edge[i].first][g] * P[machine_id][g] + bwd[edge[i].second][g] * (1 - P[machine_id][g]);
  }
  REP(g,N) prob2[task.src][g] = bwd[0][g];
  std::copy(machine_ids, machine_ids + T, task.machine_ids.begin());
}

void Write(int task_id, const Task& task, vector<tuple<int, int, int>>& edges) {
  const int T = task.targets.size() - 2;
  for (int i = 0; i < T; ++i) {
    assert (task.targets[i] >= N);
    assert (get<0>(edges[task.targets[i] - N]) == -1);
    auto &e = edges[task.targets[i] - N];
    get<0>(e) = task.machine_ids[i];
    get<1>(e) = task.targets[task.edge[i].first];
    get<2>(e) = task.targets[task.edge[i].second];
  }
}

// ===== Global Post-processing Simulated Annealing =====

// ★ P と (1-P) を float で事前に持つ（後述）
alignas(64) float Ppos[MAX_K][MAX_N], Pneg[MAX_K][MAX_N];
void buildP() {
  for (int k=0;k<K;++k) for (int g=0; g<N; ++g) {
    Ppos[k][g] = (float)P[k][g];
    Pneg[k][g] = 1.f - Ppos[k][g];
  }
}

alignas(64) static float D[MAX_N * 51 + 1][MAX_N]; // [node][g]
alignas(64) static float Fwd[(MAX_N*51 + 1)][MAX_N]; // F[u][g]

// 初期化（搬入口から前向き）
void buildF(int inlet_dest,
            const vector<tuple<int,int,int>>& edges,
            const vector<int>& topo) {
  for (int u=0; u<N+M; ++u) for (int g=0; g<N; ++g) Fwd[u][g] = 0.f;
  for (int g=0; g<N; ++g) Fwd[inlet_dest][g] = 1.f;

  for (int u_rank=0; u_rank<(int)topo.size(); ++u_rank) {
    int u = topo[u_rank];
    if (u < N) continue;
    int sidx = u - N;
    auto [k,v1,v2] = edges[sidx];
    if (k == -1) continue;
    const float* __restrict Pu = Fwd[u];
    float* __restrict P1 = Fwd[v1];
    float* __restrict P2 = Fwd[v2];
    const float* __restrict Pp = Ppos[k];
    const float* __restrict Pn = Pneg[k];
    for (int g=0; g<N; ++g) {
      float x = Pu[g];
      if (x==0.f) continue;
      P1[g] += x * Pp[g];
      P2[g] += x * Pn[g];
    }
  }
}

double evaluate_sumQ_fast(const vector<tuple<int,int,int>>& edges,
                          const vector<int>& assign, int inlet_dest,
                          const vector<int>& topo) {
  // 末端(処理装置) 初期化
  for (int i=0;i<N;++i) {
    for (int g=0; g<N; ++g) D[i][g] = (assign[i]==g);
  }
  // ソーター側はゼロクリア
  for (int u=N; u<N+M; ++u) for (int g=0; g<N; ++g) D[u][g] = 0.f;

  // トポ順の逆で後ろ向き
  for (int it=(int)topo.size()-1; it>=0; --it) {
    int u = topo[it];
    if (u < N) continue;
    int sidx = u - N;
    auto [k,v1,v2] = edges[sidx];
    if (k == -1) continue;
    float* __restrict Du = D[u];
    const float* __restrict D1 = D[v1];
    const float* __restrict D2 = D[v2];
    const float* __restrict Pp = Ppos[k];
    const float* __restrict Pn = Pneg[k];

    // N<=20 なので手動アンローリングも効果あり
    for (int g=0; g<N; ++g) Du[g] = fmaf(Pp[g], D1[g], Pn[g]*D2[g]);
  }
  double sumQ = 0.0;
  for (int g=0; g<N; ++g) sumQ += D[inlet_dest][g];
  return sumQ;
}

struct GlobalAnnealer {
  vector<tuple<int,int,int>>& edges;   // size M, (k, v1, v2) or (-1,-1,-1)
  vector<int>& assign;                 // size N, d_i: processor place i -> type
  int& inlet_dest;                     // s
  const int N_, M_, K_;
  vector<int> topo, ord;               // topological order & rank
  vector<vector<int>> cand;            // candidate destinations for each node
  vector<int> installed_sorters;       // sorter indices (0..M-1) that are installed
  vector<pair<int,int>> segs;          // all segments (u->v) including inlet
  vector<vector<double>> prob;         // reused buffer (N+M x N)
  vector<int> nodes;                   // 0..N+M-1

  GlobalAnnealer(vector<tuple<int,int,int>>& E, vector<int>& D, int& S, int N, int M, int K)
    : edges(E), assign(D), inlet_dest(S), N_(N), M_(M), K_(K),
      cand(N+M), prob(N+M, vector<double>(N, 0.0)) {
    nodes.resize(N_ + M_);
    iota(nodes.begin(), nodes.end(), 0);
    buildInstalled();
    buildTopo();
    buildCandidates(24);      // keep top-24 near forward candidates
    rebuildSegments();
  }

  inline bool isInstalledSorter(int node) const {
    if (node < N_) return false;
    int sidx = node - N_;
    return get<0>(edges[sidx]) != -1;
  }

  void buildInstalled() {
    installed_sorters.clear();
    for (int i = 0; i < M_; ++i) if (get<0>(edges[i]) != -1) installed_sorters.push_back(i);
  }

  void buildTopo() {
    // Build indegree with current edges and inlet edge
    vector<int> indeg(N_ + M_, 0);
    auto add_edge = [&](int u, int v) { if (0 <= u && u < N_ + M_ && 0 <= v && v < N_ + M_) ++indeg[v]; };
    // from inlet
    if (0 <= inlet_dest && inlet_dest < N_ + M_) add_edge(-1, inlet_dest); // only counts to inlet_dest
    // sorter edges
    for (int i = 0; i < M_; ++i) {
      auto [k, v1, v2] = edges[i];
      if (k == -1) continue;
      int u = N_ + i;
      if (0 <= v1 && v1 < N_ + M_) add_edge(u, v1);
      if (0 <= v2 && v2 < N_ + M_) add_edge(u, v2);
    }
    // Kahn
    topo.clear(); topo.reserve(N_ + M_);
    deque<int> dq;
    for (int v = 0; v < N_ + M_; ++v) if (indeg[v] == 0) dq.push_back(v);
    while (!dq.empty()) {
      int v = dq.front(); dq.pop_front();
      topo.push_back(v);
      // out edges
      if (v >= N_) {
        int sidx = v - N_;
        auto [k, v1, v2] = edges[sidx];
        if (k != -1) {
          if (0 <= v1 && v1 < N_ + M_) if (--indeg[v1] == 0) dq.push_back(v1);
          if (0 <= v2 && v2 < N_ + M_) if (--indeg[v2] == 0) dq.push_back(v2);
        }
      }
    }
    if ((int)topo.size() < N_ + M_) {
      // fallback: complete with remaining nodes to have a total order
      vector<char> seen(N_ + M_, 0);
      for (int v : topo) seen[v] = 1;
      for (int v = 0; v < N_ + M_; ++v) if (!seen[v]) topo.push_back(v);
    }
    ord.assign(N_ + M_, 0);
    for (int i = 0; i < (int)topo.size(); ++i) ord[topo[i]] = i;
  }

  void buildCandidates(int Kkeep) {
    static vector<pair<int,int>> buf; buf.reserve(N+M);
    for (int u=0; u<N+M; ++u) {
      buf.clear();
      for (int v=0; v<N+M; ++v) {
        if (u==v || ord[v] <= ord[u]) continue;
        if (v >= N && !isInstalledSorter(v)) continue;
        int dx=X[u].x - X[v].x, dy=X[u].y - X[v].y;
        buf.emplace_back(dx*dx+dy*dy, v);
      }
      if ((int)buf.size() > Kkeep) {
        nth_element(buf.begin(), buf.begin()+Kkeep, buf.end(),
                    [](auto& a, auto& b){ return a.first < b.first; });
        buf.resize(Kkeep);
      } else {
        sort(buf.begin(), buf.end(),
             [](auto& a, auto& b){ return a.first < b.first; });
      }
      cand[u].clear(); cand[u].reserve(buf.size());
      for (auto &p: buf) cand[u].push_back(p.second);
    }
  }

  void rebuildSegments() {
    segs.clear();
    // inlet -> s
    segs.emplace_back(source_index, inlet_dest);
    for (int i = 0; i < M_; ++i) {
      auto [k, v1, v2] = edges[i];
      if (k == -1) continue;
      int u = N_ + i;
      segs.emplace_back(u, v1);
      segs.emplace_back(u, v2);
    }
  }

  inline bool intersectsNew(int u, int v, int skip_u = -1, int skip_v = -1) {
    L l1{u, v};
    for (auto [a, b] : segs) {
      if ((a == skip_u && b == skip_v) || (a == u && b == v)) continue;
      if (isis_ss(l1, L{a, b})) return true;
    }
    return false;
  }

  double evaluate_sumQ() {
    // // zero
    // for (int i = 0; i < N_ + M_; ++i) std::fill(prob[i].begin(), prob[i].end(), 0.0);
    // // inject from inlet
    // if (0 <= inlet_dest && inlet_dest < N_ + M_) {
    //   for (int g = 0; g < N_; ++g) prob[inlet_dest][g] += 1.0;
    // }
    // // propagate along fixed topo
    // for (int idx = 0; idx < (int)topo.size(); ++idx) {
    //   int u = topo[idx];
    //   if (u < N_) continue;     // processors are sinks
    //   int sidx = u - N_;
    //   auto [k, v1, v2] = edges[sidx];
    //   if (k == -1) continue;
    //   const double* PP = P[k];
    //   auto& pu = prob[u];
    //   auto& pv1 = prob[v1];
    //   auto& pv2 = prob[v2];
    //   for (int g = 0; g < N_; ++g) {
    //     double x = pu[g];
    //     if (x == 0.0) continue;
    //     pv1[g] += x * PP[g];
    //     pv2[g] += x * (1.0 - PP[g]);
    //   }
    // }
    // // sum of q_i at assigned processors
    // double sumQ = 0.0;
    // for (int i = 0; i < N_; ++i) {
    //   int t = assign[i];
    //   if (0 <= t && t < N_) sumQ += prob[i][t];
    // }
    // return sumQ;
    return evaluate_sumQ_fast(edges, assign, inlet_dest, topo);
  }

  // Moves
  bool move_rewire_exit(double& cur) {
    if (installed_sorters.empty()) return false;
    int sidx = installed_sorters[rnd.next(installed_sorters.size())];
    int u = N_ + sidx;
    auto [k, v1, v2] = edges[sidx];
    if (k == -1) return false;

    int which = rnd.next(2);
    int vold = which ? v2 : v1;

    if (cand[u].empty()) return false;
    int vnew = cand[u][rnd.next(cand[u].size())];
    if (vnew == vold) return false;

    // geometry & DAG checks
    if (intersectsNew(u, vnew, u, vold)) return false;

    // apply
    if (which) get<2>(edges[sidx]) = vnew; else get<1>(edges[sidx]) = vnew;
    // update segments
    auto it = find(segs.begin(), segs.end(), make_pair(u, vold));
    if (it != segs.end()) *it = make_pair(u, vnew);
    else segs.emplace_back(u, vnew); // should exist, but guard

    double nw = evaluate_sumQ();
    double diff = nw - cur;
    if (diff >= 0) {
      cur = nw;
      return true;
    } else {
      // revert
      if (which) get<2>(edges[sidx]) = vold; else get<1>(edges[sidx]) = vold;
      auto it2 = find(segs.begin(), segs.end(), make_pair(u, vnew));
      if (it2 != segs.end()) *it2 = make_pair(u, vold);
      return false;
    }
  }

  bool move_swap_outputs(double& cur) {
    if (installed_sorters.empty()) return false;
    int sidx = installed_sorters[rnd.next(installed_sorters.size())];
    auto& e = edges[sidx];
    if (get<0>(e) == -1) return false;
    int u = N_ + sidx;

    // Segments remain identical (u->v1, u->v2) after swap, so no geom check needed.
    swap(get<1>(e), get<2>(e));

    double nw = evaluate_sumQ();
    double diff = nw - cur;
    if (diff >= 0) {
      // segments unchanged; accept
      cur = nw;
      return true;
    } else {
      swap(get<1>(e), get<2>(e));
      return false;
    }
  }

  bool move_change_sorter_type(double& cur) {
    if (installed_sorters.empty()) return false;
    int sidx = installed_sorters[rnd.next(installed_sorters.size())];
    auto& e = edges[sidx];
    if (get<0>(e) == -1) return false;

    int k_old = get<0>(e);
    int k_new = rnd.next(K_);
    if (k_new == k_old) return false;
    get<0>(e) = k_new;

    double nw = evaluate_sumQ();
    double diff = nw - cur;
    if (diff >= 0) {
      cur = nw;
      return true;
    } else {
      get<0>(e) = k_old;
      return false;
    }
  }

  bool move_swap_assignment(double& cur) {
    if (N_ <= 1) return false;
    int a = rnd.next(N_), b = rnd.next(N_);
    if (a == b) return false;
    swap(assign[a], assign[b]);

    double nw = evaluate_sumQ();
    double diff = nw - cur;
    if (diff >= 0) {
      cur = nw;
      return true;
    } else {
      swap(assign[a], assign[b]);
      return false;
    }
  }

  bool move_change_inlet(double& cur) {
    // build inlet candidates: installed sorters or processors (take near ones)
    static vector<pair<int,int>> tmp;
    tmp.clear();
    for (int v = 0; v < N_ + M_; ++v) {
      if (v >= N_ && !isInstalledSorter(v)) continue;
      int dx = X[source_index].x - X[v].x, dy = X[source_index].y - X[v].y;
      tmp.emplace_back(dx * dx + dy * dy, v);
    }
    if (tmp.empty()) return false;
    nth_element(tmp.begin(), tmp.begin() + min<int>(16, (int)tmp.size()-1), tmp.end());
    int vnew = tmp[rnd.next(min<int>(16, (int)tmp.size()))].second;
    if (vnew == inlet_dest) return false;

    // geom: inlet edge against all others
    if (intersectsNew(source_index, vnew, source_index, inlet_dest)) return false;

    // apply
    int vold = inlet_dest;
    inlet_dest = vnew;
    // update segments
    auto it = find(segs.begin(), segs.end(), make_pair(source_index, vold));
    if (it != segs.end()) *it = make_pair(source_index, vnew);

    double nw = evaluate_sumQ();
    double diff = nw - cur;
    if (diff >= 0) {
      cur = nw;
      return true;
    } else {
      inlet_dest = vold;
      auto it2 = find(segs.begin(), segs.end(), make_pair(source_index, vnew));
      if (it2 != segs.end()) *it2 = make_pair(source_index, vold);
      return false;
    }
  }

  void run_seconds(double seconds) {
    // Use remaining cycles budget if requested seconds exceed
    double cur = evaluate_sumQ();
    // keep best
    auto best_edges = edges;
    auto best_assign = assign;
    int best_s = inlet_dest;
    double best = cur;

    // two phases: exploratory (geom-safe rewire) -> polish (cheap swaps)
    while (timer.within()) {
      int r = rnd.next() % 100;
      bool ok = false;
      if      (r < 50) ok = move_rewire_exit(cur);        // 50%
      else if (r < 65) { continue; } // ok = move_swap_outputs(cur);       // 15%
      else if (r < 85) ok = move_change_sorter_type(cur); // 20%
      else if (r < 97) { continue; } // ok = move_swap_assignment(cur);    // 12%
      else             ok = move_change_inlet(cur);       // 3%

      if (ok && cur > best) {
        best = cur;
        best_edges = edges;
        best_assign = assign;
        best_s = inlet_dest;
      }
    }
    // finalize best
    edges = std::move(best_edges);
    assign = std::move(best_assign);
    inlet_dest = best_s;
    rebuildSegments(); // for cleanliness
  }
};
// ===== end of Global SA =====

int main() {
  SetUp();
  buildP();
  FindBestMachine();

  int n_solver1 = 0;
  vector<int> init_points = GetInitPoints();
  assert(init_points.size() >= 1);
  int garbage = (1 << N) - 1;
  vector<int> targets(N + M);
  std::iota(ALL(targets), 0);

  vector<tuple<int, vector<Task>, vector<int>>> vec;
  vector<int> mapping(N, -1);
  // vector<int> garbage_mapping(N, -1), mapping(N, -1);
  // vector<Task> tasks;
  // int best = 1e9;

  int lp = 0;
  int size = init_points.size() - 1;
  if (size == 0) ++size;
  for (lp = 0;; ++lp) {
    int root = init_points[lp % size];
    vector<Task> t = GenerateTasks(root, garbage, targets, mapping);
    if (!t.empty()) {
      double min_size = 1e9;
      for (const Task& task : t) {
        double num_g = __builtin_popcount(task.garbage1 | task.garbage2);
        chmin(min_size, double(task.targets.size()) / std::pow(num_g, num_garbage_param));
      }
      vec.emplace_back(min_size, t, mapping);
    }
    double elapsed = timer.elapsed();
    if (vec.empty()) {
      if (elapsed > 0.15 && size < int(init_points.size())) {
        CERR() << "UNSAFE START POINT" << std::endl;
        ++size;
      }
    } else {
      if (elapsed > 0.36) break;
    }
  }
  sort(ALL(vec), [](const auto& l, const auto& r) { return get<0>(l) > get<0>(r); });

  int best_score = 1e9;
  int best_index = -1;
  REP(i, vec.size()) {
    vector<Task>& t = get<1>(vec[i]);
    auto& mapping = get<2>(vec[i]);
    if (i > 0 && timer.elapsed() > 0.40) break;
    vector<vector<double>> prob(N + M, vector<double>(N, 0.0));
    REP(i,N) prob[t[0].src][i] = 1.0;
    try {
      REP(i,N-1) {
        Task& task = t[i];
        Solve1(i, task, prob);
      }
    } catch (const std::runtime_error& e) {
      CERR() << "CATCH EXCEPTION!!" << std::endl;
      continue;
    }
    double qsum = 0;
    REP(i,N) {
      int t = mapping[i];
      double q = prob[i][t];
      qsum += 1 - q;
    }
    int score = qsum / N * 1e9 + 0.5;
    if (score < best_score) {
      best_score = score;
      best_index = i;
    }
    ++n_solver1;
  }
  CERR() << "score1 = " << best_score << std::endl;
  auto [_, tasks, garbage_mapping] = vec[best_index];

  LOG(lp);
  LOG(n_solver1);
  // LOG(best);
  assert(int(tasks.size()) == N - 1);

  vector<tuple<int, int, int>> edges(M, {-1, -1, -1});
  vector<vector<double>> prob1(N + M, vector<double>(N, 0.0));
  vector<vector<double>> prob2(N + M, vector<double>(N, 0.0));
  REP(i,N) {
    prob1[tasks[0].src][i] = 1.0;
    prob2[i][garbage_mapping[i]] = 1.0;
  }
  int sum_targets = 0;
  REP(i,N-1) sum_targets += tasks[i].targets.size();
  const int64_t sum_cycles = timer.start + Timer::CYCLES_PER_SEC * 1.65 - timer.get_cycle();
  REP(i,N-1) {
    Task& task = tasks[i];
    int64_t n_cycles = sum_cycles * task.targets.size() / sum_targets;
    Solve2(i, task, prob1, n_cycles);
    task.machine_ids.resize(task.targets.size() - 2);
  }
  for (int loop = 0;; ++loop) {
    if (!timer0.within()) break;
    for (int i = N - 2; i >= 0; --i) {
      if (!timer0.within()) break;
      Task& task = tasks[i];
      Solve3(i, task, prob1, prob2);
    }
    REP(i,N-1) {
      if (!timer0.within()) break;
      const Task& task = tasks[i];
      Simulate(i, task, prob1);
    }
  }
  REP(i,N-1) {
    const Task& task = tasks[i];
    Write(i, task, edges);
  }
  LOG(solve2_iter);

  {
    double budget_sec = 0.10;  // 必要に応じて調整
    GlobalAnnealer ga(edges, garbage_mapping, /*inlet*/ tasks[0].src, N, M, K);
    ga.run_seconds(budget_sec);
  }

  {
    // Evaluate
    double qsum = 0;
    REP(i,N) {
      int t = garbage_mapping[i];
      double q = prob1[i][t];
      std::cerr << "Vertex: " << i << ", Processor: " << t << ", P: " << q << "\n";
      qsum += 1 - q;
    }
    int score = qsum / N * 1e9 + 0.5;
    LOG(score);
  }

  REP(i,N) cout << garbage_mapping[i] << " ";
  cout << "\n";
  cout << tasks[0].src << "\n";
  REP(i,M) {
    auto [k, v1, v2] = edges[i];
    if (k == -1) {
      cout << "-1\n";
    } else {
      cout << k << " " << v1 << " " << v2 << "\n";
    }
  }
  cout << flush;

  double elapsed = timer.elapsed();
  LOG(elapsed);
  std::_Exit(0);
  return 0;
}
