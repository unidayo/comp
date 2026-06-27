int LOCAL_RUN = 0;
//#define LOCAL
double R = 1;
double TL1 = 1.60;
double TL = 1.85;

// ----------------------------- 3   4   5   6   7   8  9 10 11 12 13 14 15
const double MULT[16] = {0,0,0,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1};
const int SAMPLEN[16] = {0,0,0,128, 40, 32, 19, 12, 11, 8, 7, 5, 4, 4, 3, 3};
const int DIVS   [16] = {0,0,0,100, 15,  5,  4,  4,  3, 3, 3, 3, 3, 3, 3, 3};
const int POST   [16] = {0,0,0, 50, 40, 50, 50, 50, 45, 0, 0, 0, 0, 0, 0, 0};
const int ITS    [16] = {0,0,0,160,150,160,133, 90, 90,70,60,40,33,30,28,26};

//#pragma GCC optimize("O3,Ofast,omit-frame-pointer,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize")
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <cstring>
#include <cmath>
#include <cassert>
#include <random>
#include <iomanip>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <atcoder/all>

using dsu = atcoder::dsu;

//https://github.com/mapbox/delaunator/blob/main/LICENSE
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
        //throw std::runtime_error("not triangulation");
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

int STANDARD = 1;
using namespace std;
#define F0(i,n) for (int i=0; i<n; i++)
#define F1(i,n) for (int i=1; i<=n; i++)
#define CL(a,x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())
#define MP make_pair
const int inf = 1000000000;
const double pi = acos(-1.0);
typedef pair<int, int> pii;
typedef long long ll;
typedef unsigned long long ull;
const double EPS = 1e-9;
template<class A, class B>
ostream& operator<<(ostream& os, const pair<A, B>& p) { os << "(" << p.first << "," << p.second << ")"; return os; }
template<class A, class B, class C>
ostream& operator<<(ostream& os, const tuple<A, B, C>& p) { os << "(" << get<0>(p) << "," << get<1>(p) << "," << get<2>(p) << ")"; return os; }
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "["; F0(i,SZ(v)) { if (i>0) os << ","; os << v[i]; } os << "]"; return os;
}
template<class T>
ostream& operator<<(ostream& os, const set<T>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ","; cerr << i; } os << "}"; return os;
}
template<class T, class R>
ostream& operator<<(ostream& os, const map<T,R>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ", "; cerr << i.first << ":" << i.second; } os << "}"; return os;
}
void print_all() { cerr << endl; }
template <typename H, typename... T>
void print_all(H head, T... tail) { cerr << " " << head; print_all(tail...); }
#ifdef LOCAL
#define PR(...) cerr << #__VA_ARGS__ << " =", print_all(__VA_ARGS__)
#else
#define PR(...)
#endif

#ifdef ATCODERX
inline ll GetTSC() {
    ll lo, hi;
    asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
    return lo + (hi << 32);
}
inline double GetSeconds() {
    return GetTSC() / 3.0e9;
}
#else
chrono::system_clock::time_point init_time = chrono::system_clock::now();
inline double GetSeconds() {
     chrono::system_clock::time_point current_time = chrono::system_clock::now();
     double ret =  chrono::duration_cast<std::chrono::nanoseconds>(current_time - init_time).count();
     return ret * 1e-9;
}
#endif

const int MAX_RAND = 1 << 30;
struct Rand {
    ll x, y, z, w, o;
    Rand() {}
    Rand(ll seed) { reseed(seed); o = 0; }
    inline void reseed(ll seed) { x = 0x498b3bc5 ^ seed; y = 0; z = 0; w = 0;  F0(i, 20) mix(); }
    inline void mix() { ll t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8); }
    inline ll rand() { mix(); return x & (MAX_RAND - 1); }
    inline ll zh() { mix(); return x; }
    inline int nextInt(int n) { return rand() % n; }
    inline int nextInt(int L, int R) { return rand()%(R - L + 1) + L; }
    inline int nextBool() { if (o < 4) o = rand(); o >>= 2; return o & 1; }
    double nextDouble() { return rand() * 1.0 / MAX_RAND; }
};
Rand my(2025);
double saveTime;
double t_o[20];
ll c_o[20];
void Init() {
    saveTime = GetSeconds();
    F0(i, 20) t_o[i] = 0.0;
    F0(i, 20) c_o[i] = 0;
}
double Elapsed() { return (GetSeconds() - saveTime) / R; }
void Report() {
#ifndef LOCAL
    return;
#endif
    double tmp = Elapsed();
    cerr << "-------------------------------------" << endl;
    cerr << "Elapsed time: " << tmp << " sec" << endl;
    double total = 0.0; F0(i, 20) { if (t_o[i] > 0) cerr << "t_o[" << i << "] = " << t_o[i] << endl; total += t_o[i]; } cerr << endl; //if (total > 0) cerr << "Total spent: " << total << endl;
    F0(i, 20) if (c_o[i] > 0) cerr << "c_o[" << i << "] = " << c_o[i] << endl;
    cerr << "-------------------------------------" << endl;
}
struct AutoTimer {
    int x;
    double t;
    AutoTimer(int x) : x(x) {
        t = Elapsed();
    }
    ~AutoTimer() {
        t_o[x] += Elapsed() - t;
    }
};
//#define AT(i) AutoTimer a##i(i)
#define AT(i)

// CONSTANTS
const int N = 1024;
int n, m, Q, L, W;
int G[N], COORD[4][N];
ll bscore, score;
ll zh[N][N];
ll d[N][N];
int samples;
int max_d[N][N], min_d[N][N], v_id[N];
pii real_pos[N], pos[N];
const int MAX_SAMPLES = 128;
int SAMPLES = 64;
pii a[MAX_SAMPLES][N], p[N];
ll scores[MAX_SAMPLES];
ll each[MAX_SAMPLES][N];
int pre_d[MAX_SAMPLES][4][4];
int cd;
vector<pii> adj[N];
vector<int> graph[16];
int g_par[16], g_h[16];
vector<int> q, g, bg;
int v[N][N], vz, v_in[N];
int conflicts, pointless, interesting;
int duplicates;
int cnt[16];
vector<int> involve[N];
vector<pair<pii, pii>> conds;
vector<vector<int>> cond_parts;
set<pair<pii, pii>> all_conds;
map<pii, vector<pii>> PO;
set<pii> Starts;

void ShowCycles() {
    return;
    for (auto& p : conds) {
        PO[p.first].push_back(p.second);
        Starts.insert(p.first);
    }
    PR(SZ(PO));

    set<pair<pii, pii>> removals;

    for (pii p0 : Starts) {
        if (!PO.count(p0)) { PR("?"); throw; }
        for (pii p1 : PO[p0]) {
            if (!PO.count(p1)) continue;
            for (pii p2 : PO[p1]) {
                if (all_conds.count(make_pair(p2, p0))) continue; // THROW?
                if (all_conds.count(make_pair(p0, p2))) {
                    removals.insert(make_pair(p0, p2));
                }
            }
        }
    }
    PR(SZ(removals));

    exit(0);
}

pii RandPoint(int i) {
    return pii(my.nextInt(COORD[0][i], COORD[1][i]), my.nextInt(COORD[2][i], COORD[3][i]));
}

bool In(pii p, int i) {
    return COORD[0][i] <= p.first && p.first <= COORD[1][i]
        && COORD[2][i] <= p.second && p.second <= COORD[3][i];
}

pii RandNei(pii r, int i, int x) {
    int w = 1500 / (x + L);
    r.first += my.nextInt(-w, w);
    r.second += my.nextInt(-w, w);
    if (!In(r, i)) r = RandPoint(i);
    return r;
}

template<class T> auto REL(T& v) { return v[my.nextInt(SZ(v))]; }
template<class T> void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }
template<class T> T sqr(T x) { return x*x; }

inline pii pd(pii p1, pii p2) {
    return pii(p2.first - p1.first, p2.second - p1.second);
}

inline int d2(int dx, int dy) {
    return sqrt(sqr(dx) + sqr(dy));
}

inline int d2(pii p1, pii p2) {
    return d2(p1.first - p2.first,
              p1.second - p2.second);
}

inline int dfast(pii p1, pii p2) {
    return sqr(p1.first - p2.first) + sqr(p1.second - p2.second);
}

int IsConflict(int sid, int ci) {
    pii& c1 = conds[ci].first, &c2 = conds[ci].second;
    int d1 = dfast(a[sid][c2.first], a[sid][c2.second]);
    int d2 = dfast(a[sid][c1.first], a[sid][c1.second]);
    if (d1 > d2) return d1 - d2;
    return 0;
}

vector<pii> Ask(vector<int> a) {
    sort(a.begin(), a.end());
    int sz = SZ(a);
    assert(sz >= 3 && sz <= L);
    vector<pii> r(sz - 1);

    if (LOCAL_RUN) {
        dsu uf(sz);
        vector<pair<int, pii>> edges;
        F0(x, sz) v_id[a[x]] = x;
        F0(i0, sz) F0(i1, i0) edges.emplace_back(d2(real_pos[a[i0]], real_pos[a[i1]]), pii(min(a[i0], a[i1]), max(a[i0], a[i1])));
        sort(edges.begin(), edges.end());
        int cnt = 0;
        for (auto e : edges) {
            int x = v_id[e.second.first], y = v_id[e.second.second];
            if (!uf.same(x, y)) {
                r[cnt] = e.second;
                uf.merge(x, y);
                if (++cnt == sz - 1) break;
            }
        }
        sort(r.begin(), r.end());
        return r;
    }

    cout << "? " << sz;
    for (int x : a) cout << " " << x;
    cout << endl;
    F0(i, sz - 1) cin >> r[i];
    return r;
}

struct Solution {
    vector<vector<int>> group_cities;
    vector<vector<pii>> group_edges;
    void Init() {
        group_cities.assign(m, vector<int>());
        group_edges.assign(m, vector<pii>());
    }

    int MstLen(int i, bool set_edges = false) {
        vector<int>& g = group_cities[i];
        assert(SZ(g) == G[i]);
        dsu uf(G[i]);
        vector<pair<int, pii>> edges;
        F0(x, G[i]) v_id[g[x]] = x;
        F0(i0, G[i]) F0(i1, i0) edges.emplace_back(d[g[i0]][g[i1]], pii(min(g[i0], g[i1]), max(g[i0], g[i1])));
        sort(edges.begin(), edges.end());
        int ret = 0;

        if (set_edges) group_edges[i].clear();
        int cnt = 0;
        for (auto e : edges) {
            int x = v_id[e.second.first], y = v_id[e.second.second];
            if (!uf.same(x, y)) {
                cnt++;
                ret += e.first;
                uf.merge(x, y);
                if (set_edges) group_edges[i].push_back(e.second);
            }
        }
        assert(cnt == G[i] - 1);

        return ret;
    }

    int TotLen() {
        int ret = 0;
        F0(i, m) ret += MstLen(i);
        return ret;
    }

    int RealLen(int i, bool set_edges = false) {
        vector<int>& g = group_cities[i];
        assert(SZ(g) == G[i]);
        dsu uf(G[i]);
        vector<pair<int, pii>> edges;
        F0(x, G[i]) v_id[g[x]] = x;
        F0(i0, G[i]) F0(i1, i0) edges.emplace_back(d2(real_pos[g[i0]], real_pos[g[i1]]), pii(min(g[i0], g[i1]), max(g[i0], g[i1])));
        sort(edges.begin(), edges.end());
        int ret = 0;

        if (set_edges) group_edges[i].clear();
        int cnt = 0;
        for (auto e : edges) {
            int x = v_id[e.second.first], y = v_id[e.second.second];
            if (!uf.same(x, y)) {
                cnt++;
                ret += e.first;
                uf.merge(x, y);
                if (set_edges) group_edges[i].push_back(e.second);
            }
        }
        assert(cnt == G[i] - 1);

        return ret;
    }

    void Print() {
        if (LOCAL_RUN) {
            int oscore = 0;
            bscore = 0;
            F0(i, m) {
                assert(SZ(group_cities[i]) == G[i]);
                assert(SZ(group_edges[i]) == G[i] - 1);
                int cscore = 0;
                F0(j, G[i] - 1) {
                    cscore += d2(real_pos[group_edges[i][j].first],
                                 real_pos[group_edges[i][j].second]);
                }
                int rscore = RealLen(i);

                if (cscore != rscore) {
                    cerr << i << "/" << m << " " << G[i] << " " << cscore << " " << rscore << " " << cscore - rscore << endl;
                }

                bscore += cscore;
                oscore += rscore;
            }
            PR(100.0 * bscore / oscore);
            return;
        }

        cout << "!" << endl;
        assert(SZ(group_cities) == m);
        F0(i, m) {
            assert(SZ(group_cities[i]) == G[i]);
            F0(j, G[i]) {
                cout << group_cities[i][j];
                if (j == G[i] - 1) cout << endl;
                else cout << " ";
            }
            assert(SZ(group_edges[i]) == G[i] - 1);
            F0(j, G[i] - 1) cout << group_edges[i][j].first << " " << group_edges[i][j].second << endl;
        }
    }
} bsol, sol;

double logs[MAX_SAMPLES + 1][MAX_SAMPLES + 1];

void CalcPre() {
    F0(i, n) F0(j, n) {
        d[i][j] = d2(pos[i], pos[j]);
    }
}

void Prepare() {
    // n=800
    // m-partitions(1-399)
    // Q=400
    // L-size(3-15)
    // W-width(500-2500)
    F0(c1, MAX_SAMPLES + 1) F0(c2, MAX_SAMPLES + 1) if (c1 && c2) {
        double p = c1 + c2;
        double p1 = 1.0 * c1 / p;
        double p2 = 1.0 * c2 / p;
        logs[c1][c2] += -p1 * log2(p1) * p;
        logs[c1][c2] += -p2 * log2(p2) * p;
    }

    PR(m, L, W);

    F1(i, n) F0(j, n + 1) zh[i][j] = my.zh();

    bscore = 8e18;

    F0(i, n) {
        pos[i].first = (COORD[0][i] + COORD[1][i]) / 2;
        pos[i].second = (COORD[2][i] + COORD[3][i]) / 2;
    }
    CalcPre();
    F0(i, n) F0(j, n) {
        int dx1 = 0;
        if (COORD[0][i] > COORD[1][j]) dx1 = abs(COORD[0][i] - COORD[1][j]);
        if (COORD[0][j] > COORD[1][i]) dx1 = abs(COORD[0][j] - COORD[1][i]);
        int dx2 = max(abs(COORD[0][i] - COORD[1][j]), abs(COORD[0][j] - COORD[1][i]));

        int dy1 = 0;
        if (COORD[2][i] > COORD[3][j]) dy1 = abs(COORD[2][i] - COORD[3][j]);
        if (COORD[2][j] > COORD[3][i]) dy1 = abs(COORD[2][j] - COORD[3][i]);
        int dy2 = max(abs(COORD[2][i] - COORD[3][j]), abs(COORD[2][j] - COORD[3][i]));

        min_d[i][j] = d2(dx1, dy1);
        max_d[i][j] = d2(dx2, dy2);
    }

    F0(i, n) {
        F0(j, n) if (i != j) adj[i].emplace_back(d[i][j], j);
        sort(adj[i].begin(), adj[i].end());
    }
}

void UpdateBest() {
    if (score < bscore) {
        bscore = score;
        bsol = sol;
    }
}

#ifdef VIS
#include "CImg.h"
using namespace cimg_library;
CImg < unsigned char > img = CImg<unsigned char>(1600, 1100, 1, 3);
CImgDisplay disp;
unsigned char white[] = {255, 255, 255};
unsigned char grey[] = {192, 192, 192};
unsigned char red[] = {255, 100, 100};
unsigned char lred[] = {255, 200, 200};
unsigned char color1[] = {123, 20, 200};
unsigned char green[] = {100, 255, 100};
unsigned char lgreen[] = {200, 255, 200};
unsigned char blue[] = {100, 100, 255};
unsigned char lblue[] = {200, 200, 255};
unsigned char black[] = { 0,0,0 };
unsigned char rcol[] = {128, 128, 128};

void ShowIt(int sid, int keep = 0) {
    img = (unsigned char)0;
    // grid
    img.draw_rectangle(0, 0, 1000, 1000, white);

    F0(i, n) {
        //pii p2 = real_pos[i];
        pii p2 = a[sid][i];
        img.draw_circle(p2.first / 10, p2.second / 10, 3, blue);
    }

    F0(i, n) if (each[sid][i]) {
        img.draw_line(COORD[0][i] / 10, COORD[2][i] / 10, COORD[1][i] / 10, COORD[2][i] / 10, grey);
        img.draw_line(COORD[0][i] / 10, COORD[3][i] / 10, COORD[1][i] / 10, COORD[3][i] / 10, grey);
        img.draw_line(COORD[0][i] / 10, COORD[2][i] / 10, COORD[0][i] / 10, COORD[3][i] / 10, grey);
        img.draw_line(COORD[1][i] / 10, COORD[2][i] / 10, COORD[1][i] / 10, COORD[3][i] / 10, grey);
    }

    F0(ci, SZ(conds)) {
        if (IsConflict(sid, ci)) {
            pii p, p2;
            pii& c1 = conds[ci].first, &c2 = conds[ci].second;

            int w = 0;
            for (int dx = -w; dx <= w; dx++) for (int dy = -w; dy <= w; dy++) {
                p = a[sid][c1.first]; p2 = a[sid][c1.second];
                img.draw_line(p.first / 10 + dx, p.second / 10 + dy,
                              p2.first / 10 + dx, p2.second / 10 + dy,
                              red);
                p = a[sid][c2.first]; p2 = a[sid][c2.second];
                img.draw_line(p.first / 10 + dx, p.second / 10 + dy,
                              p2.first / 10 + dx, p2.second / 10 + dy,
                              blue);
            }
        }
    }

    F0(i, n) {
        pii p = a[sid][i];
        pii p2 = real_pos[i];

        int w = 1;

        for (int dx = -w; dx <= w; dx++) for (int dy = -w; dy <= w; dy++)
        img.draw_line(p.first / 10 + dx, p.second / 10 + dy,
                      p2.first / 10 + dx, p2.second / 10 + dy,
                      lgreen);
    }
    string msg = to_string(scores[sid]);
    img.draw_text(1200, 0, msg.c_str(), red, white, 1, 60);

    disp.display(img);

    if (keep)
    while (1) {
        disp.wait();
        if (disp.is_keyR()) {
            exit(0);
        }
        if (disp.is_keyQ()) {
            return;
        }
    }
}
#else
template<typename... Args>
void ShowIt(Args... args) {}
#endif

void DFS(int i, int par = -1) {
    for (int j : graph[i]) if (j != par) {
        g_h[j] = g_h[i] + 1;
        g_par[j] = i;
        DFS(j, i);
    }
}

void AddCondition(int i1, int i2, int j1, int j2) {
    if (i1 > i2) swap(i1, i2);
    if (j1 > j2) swap(j1, j2);

    pii p1(i1, i2);
    pii p2(j1, j2);

    if (d[i1][i2] < d[j1][j2]) {
        conflicts++;
    }
    if (min_d[i1][i2] > max_d[j1][j2]) {
        pointless++;
    } else {
        if (all_conds.count(make_pair(p1, p2))) {
            duplicates++;
            return;
        }
        all_conds.insert(make_pair(p1, p2));

        cond_parts.push_back(vector<int>{i1, i2});
        involve[i1].push_back(SZ(conds));
        involve[i2].push_back(SZ(conds));
        if (j1 != i1 && j1 != i2) {
            involve[j1].push_back(SZ(conds));
            cond_parts.back().push_back(j1);
        }
        if (j2 != i1 && j2 != i2) {
            involve[j2].push_back(SZ(conds));
            cond_parts.back().push_back(j2);
        }
        conds.emplace_back(p1, p2);
        interesting++;
    }
}

ll Naive(int sid) {
    ll ret = 0;
    F0(i, SZ(conds)) {
        ret += IsConflict(sid, i);
    }
    return ret;
}

int conf_id[MAX_SAMPLES][1<<17], conf_w[MAX_SAMPLES][1<<17];
bool cd1[1<<20], cd2[1<<20];
int cn[MAX_SAMPLES];

void Resolve(int sid) {
    int itc = n * MULT[L];
    ll delta = 0;
    int i = 0;

    int DIV = DIVS[L];

    pii newp, tmp;
    int w = 100, dx = 0, dy = 0;
    int tp = 0;

    F0(iter, itc) {
        if (iter % DIV == 0) {
            int cid = conf_id[sid][my.nextInt(cn[sid])];
            i = REL(cond_parts[cid]);

            if (1) {
                tp = 1;
                newp = a[sid][i];
                dx = my.nextInt(-w, w);
                dy = my.nextInt(-w, w);
                newp.first += dx;
                newp.second += dy;
                if (newp == a[sid][i] || !In(newp, i)) {
                    tp = 0;
                    newp = RandPoint(i);
                }
            } else {
                tp = 0;
                newp = RandPoint(i);
            }
        } else {
            i = my.nextInt(n);
            newp = RandPoint(i);
            tp = 0;
        }
        pii oldp = a[sid][i];
        delta = -each[sid][i];
        a[sid][i] = newp;

        for (int index : involve[i]) {
            delta += IsConflict(sid, index);
            if (delta > 0) break;
        }

        if (1 && delta < 0 && tp == 1) {
            while (1) {
                tmp = a[sid][i];
                a[sid][i].first += dx;
                a[sid][i].second += dy;
                if (!In(a[sid][i], i)) {
                    a[sid][i] = tmp;
                    break;
                }
                ll delta2 = -each[sid][i];
                for (int index : involve[i]) {
                    delta2 += IsConflict(sid, index);
                    if (delta2 >= delta) break;
                }
                if (delta2 >= delta) {
                    a[sid][i] = tmp;
                    break;
                }
                delta = delta2;
            }
        }

        if (delta <= 0) {
            pii savep = a[sid][i];
            int saved = -each[sid][i];

            for (int index : involve[i]) cd1[index] = cd2[index] = false;

            a[sid][i] = oldp;
            if (each[sid][i]) {
                for (int index : involve[i]) if ((cd = IsConflict(sid, index))) {
                    cd1[index] = true;
                    for (int x : cond_parts[index]) each[sid][x] -= cd;
                }
            }

            a[sid][i] = savep;
            if (saved != delta) {
                for (int index : involve[i]) if ((cd = IsConflict(sid, index))) {
                    cd2[index] = true;
                    for (int x : cond_parts[index]) each[sid][x] += cd;
                }
            }

            for (int index : involve[i]) if (cd1[index] != cd2[index]) {
                if (cd1[index]) {
                    int w = conf_w[sid][index];
                    swap(conf_id[sid][w], conf_id[sid][--cn[sid]]);
                    conf_w[sid][conf_id[sid][w]] = w;
                } else {
                    conf_w[sid][index] = cn[sid];
                    conf_id[sid][cn[sid]++] = index;
                }
            }

            scores[sid] += delta;
            if (scores[sid] == 0) {
                break;
            }
        } else {
            a[sid][i] = oldp;
        }
    }
}

void Analyze(vector<int> a) {
    AT(3);
    int psz = SZ(conds);

    auto r = Ask(a);
    F0(i, L) {
        graph[i].clear();
        v_id[a[i]] = i;
    }
    F0(i, L - 1) {
        graph[v_id[r[i].first]].push_back(v_id[r[i].second]);
        graph[v_id[r[i].second]].push_back(v_id[r[i].first]);
    }
    DFS(0);
    F0(i2, L) F0(i1, i2) {
        if (g_par[i2] == i1 || g_par[i1] == i2) continue;
        int x = i1, y = i2;
        while (x != y) {
            if (g_h[x] < g_h[y]) swap(x, y);
            AddCondition(a[i1], a[i2], a[x], a[g_par[x]]);
            x = g_par[x];
        }
    }

    int nsz = SZ(conds);
    F0(sid, SAMPLES) {
        for (int i = psz; i < nsz; i++) {
            if ((cd = IsConflict(sid, i))) {
                scores[sid] += cd;
                conf_w[sid][i] = cn[sid];
                conf_id[sid][cn[sid]++] = i;
                for (int x : cond_parts[i]) each[sid][x] += cd;
            }
        }
    }
}

void ExpandSamples(int sample_count) {
    //if (Elapsed() > TL1) throw;
    //TL1 = Elapsed() + 0.5;
    int expansions = 0;
    while (Elapsed() < TL1) {
        ++expansions;
        F0(sid, sample_count) {
            if (Elapsed() > TL1) {
                PR(expansions, sid);
                break;
            }
            F0(i, n) {
                if (Elapsed() > TL1) {
                    PR(expansions, sid, i);
                    break;
                }
                pii oldp = a[sid][i];
                F0(x, 16) {
                    int delta = -each[sid][i];

                    if (x >= 4) {
                        a[sid][i] = RandNei(oldp, i, x);
                    } else {
                        a[sid][i] = RandPoint(i);
                    }

                    for (int index : involve[i]) {
                        delta += IsConflict(sid, index);
                        if (delta > 0) break;
                    }
                    if (delta <= 0) {
                        pii savep = a[sid][i];
                        int saved = -each[sid][i];

                        a[sid][i] = oldp;
                        if (each[sid][i]) {
                            for (int index : involve[i]) if ((cd = IsConflict(sid, index))) {
                                for (int x : cond_parts[index]) each[sid][x] -= cd;
                            }
                        }

                        a[sid][i] = savep;
                        if (saved != delta) {
                            for (int index : involve[i]) if ((cd = IsConflict(sid, index))) {
                                for (int x : cond_parts[index]) each[sid][x] += cd;
                            }
                        }
                        scores[sid] += delta;
                        break;
                    }
                    a[sid][i] = oldp;
                }
            }
            samples++;
            F0(i, n) F0(j, i) d[i][j] += dfast(a[sid][i], a[sid][j]);
        }
    }
    PR(expansions, vector<ll>(scores, scores + SAMPLES));
}

int prim_d[N], prim_i[N], prim_u[N];
int prim_v[N], prim_v2[N];
int prim_d2[N], prim_i2[N];
int e_freq[16][16];
vector<int> e[MAX_SAMPLES][N];
int g_w[2][N];
int g_value[2][N];
int g_heap[2][N];

struct QSet {
    int* w;
    int* value;
    int* heap;
    int sz, i;

    QSet(int i) : w(g_w[i]), value(g_value[i]), heap(g_heap[i]) {

    }

    void erase(pii p) {
        int x = w[p.second];
        swap(heap[x], heap[--sz]);
        w[heap[x]] = x;
        while (2 * x + 1 < sz) {
            int y = x;
            if (value[heap[y]] > value[heap[2 * x + 1]]) y = 2 * x + 1;
            if (2 * x + 2 < sz && value[heap[y]] > value[heap[2 * x + 2]]) y = 2 * x + 2;
            if (x == y) break;
            swap(heap[x], heap[y]);
            w[heap[x]] = x;
            w[heap[y]] = y;
            x = y;
        }
    }
    void insert(pii p) {
        value[p.second] = p.first;
        w[p.second] = sz;
        heap[sz] = p.second;
        int x = sz++;
        while (x>0 && value[heap[(x-1)/2]] > value[heap[x]]) {
            swap(w[heap[x]], w[heap[(x-1)/2]]);
            swap(heap[(x-1)/2], heap[x]);
            x = (x-1) / 2;
        }
    }

    void clear() {
        sz = 0;
    }

    int begin() {
        return heap[0];
    }

    bool empty() {
        return sz == 0;
    }
} PQ(0), PQ2(1);

void vis1(int sid, int j) {
    if (prim_u[j] == vz) return;
    for (int i : g) {
        int dist = dfast(a[sid][i], a[sid][j]);
        if (prim_v[j] != vz || prim_d[j] > dist) {
            if (prim_v[j] == vz) {
                PQ.erase(pii(prim_d[j], j));
            }
            prim_d[j] = dist;
            prim_i[j] = i;
            prim_v[j] = vz;
            PQ.insert(pii(prim_d[j], j));
        }
    }
}

void vis2(int sid2, int j) {
    if (prim_u[j] == vz) return;
    for (int i : g) {
        int dist = dfast(a[sid2][i], a[sid2][j]);
        if (prim_v2[j] != vz || prim_d2[j] > dist) {
            if (prim_v2[j] == vz) {
                PQ2.erase(pii(prim_d2[j], j));
            }
            prim_d2[j] = dist;
            prim_i2[j] = i;
            prim_v2[j] = vz;
            PQ2.insert(pii(prim_d2[j], j));
        }
    }
}

void Expand(int sid, int sid2, int j) {
    for (int x : e[sid][j]) vis1(sid, x);
    for (int x : e[sid2][j]) vis2(sid2, x);
}

void Discriminate(int sid, int sid2, int center) {
    AT(12);
    g.clear();
    g.push_back(center);

    vz += 2;
    prim_u[center] = vz;
    PQ.clear();
    PQ2.clear();
    for (int i : e[sid][center]) {
        prim_v[i] = vz;
        prim_i[i] = center;
        prim_d[i] = dfast(a[sid][center], a[sid][i]);
        PQ.insert(pii(prim_d[i], i));
    }
    for (int i : e[sid2][center]) {
        prim_v2[i] = vz;
        prim_i2[i] = center;
        prim_d2[i] = dfast(a[sid2][center], a[sid2][i]);
        PQ2.insert(pii(prim_d2[i], i));
    }

    int its = 0;
    while (SZ(g) < L) {
        its++;

        if (PQ.empty()) {
            PR(vz);
            g.clear();
            return;
        }

        int bi = PQ.begin();
        int bi2 = PQ2.begin();

        vis1(sid, bi2);
        vis2(sid2, bi);

        if (prim_i[bi] != prim_i2[bi] && prim_i[bi2] != prim_i2[bi2]) {
            if (prim_d[bi] > prim_d2[bi2]) bi = bi2;
        } else if (prim_i[bi] != prim_i2[bi]) {
            // good enough
        } else if (prim_i[bi2] != prim_i2[bi2]) {
            bi = bi2; // good
        } else if (bi == bi2) {
            prim_u[bi] = vz;
            Expand(sid, sid2, bi);
            PQ.erase(pii(prim_d[bi], bi));
            PQ2.erase(pii(prim_d2[bi], bi));
            continue;
        } else {
            // find next edges
            int bc = bi, bc2 = bi2;
            int best_d = dfast(a[sid][bi2], a[sid][bi]);
            int best_d2 = dfast(a[sid2][bi], a[sid2][bi2]);

            if (prim_d[bi2] < best_d) bc = prim_i[bi2];
            if (prim_d2[bi] < best_d2) bc2 = prim_i2[bi];

            // check that next edges aren't just swapped
            if (prim_i[bi] == bc2 && prim_i2[bi2] == bc) {
                if (its <= 10 * L) {
                    if (best_d > best_d2) {
                        prim_u[bi] = vz;
                        Expand(sid, sid2, bi);
                        PQ.erase(pii(prim_d[bi], bi));
                        PQ2.erase(pii(prim_d2[bi], bi));
                    } else {
                        prim_u[bi2] = vz;
                        Expand(sid, sid2, bi2);
                        PQ.erase(pii(prim_d[bi2], bi2));
                        PQ2.erase(pii(prim_d2[bi2], bi2));
                    }
                    continue;
                }
            }
        }

        g.push_back(bi);
        prim_u[bi] = vz;

        PQ.erase(pii(prim_d[bi], bi));
        PQ2.erase(pii(prim_d2[bi], bi));

        for (int i : e[sid][bi]) if (prim_u[i] != vz) {
            int dist = dfast(a[sid][bi], a[sid][i]);
            if (prim_v[i] != vz || prim_d[i] > dist) {
                if (prim_v[i] == vz) {
                    PQ.erase(pii(prim_d[i], i));
                }
                prim_v[i] = vz;
                prim_d[i] = dist;
                prim_i[i] = bi;
                PQ.insert(pii(prim_d[i], i));
            }
        }
        for (int i : e[sid2][bi]) if (prim_u[i] != vz) {
            int dist = dfast(a[sid2][bi], a[sid2][i]);
            if (prim_v2[i] != vz || prim_d2[i] > dist) {
                if (prim_v2[i] == vz) {
                    PQ2.erase(pii(prim_d2[i], i));
                }
                prim_v2[i] = vz;
                prim_d2[i] = dist;
                prim_i2[i] = bi;
                PQ2.insert(pii(prim_d2[i], i));
            }
        }
    }
}

void GenQuery(int sid, int center) {
    AT(2);
    g.clear();
    g.push_back(center);

    F0(i, n) { prim_d[i] = inf; prim_u[i] = 0; }
    prim_u[center] = 1;
    F0(i, n) {
        prim_d[i] = dfast(a[sid][center], a[sid][i]);
        prim_i[i] = 0;
    }
    F1(step, L + 4) {
        int best_d = inf;
        int bi = -1;
        F0(i, n) if (!prim_u[i] && prim_d[i] < best_d) {
            best_d = prim_d[i];
            bi = i;
        }
        g.push_back(bi);
        prim_u[bi] = 1;
        F0(i, n) if (!prim_u[i]) {
            int nd = dfast(a[sid][bi], a[sid][i]);
            if (nd < prim_d[i]) {
                prim_d[i] = nd;
                prim_i[i] = bi;
            }
        }
    }
}

double EvalOld(const vector<int>& g) {
    int L = SZ(g);
    AT(14);
    double ret = 0.0;

    F0(i, L) F0(j, L) e_freq[i][j] = 0;

    F0(sid, SAMPLES) {
        F1(i, L-1) { prim_d[i] = inf; prim_u[i] = 0; }
        prim_u[0] = 1;
        F1(i, L-1) {
            prim_d[i] = dfast(a[sid][g[0]], a[sid][g[i]]);
            prim_i[i] = 0;
        }
        F1(step, L-1) {
            int best_d = inf;
            int bi = -1;
            F1(i, L-1) if (!prim_u[i] && prim_d[i] < best_d) {
                best_d = prim_d[i];
                bi = i;
            }
            //prim_e[sid][step] = pii(bi, prim_i[bi]);
            e_freq[min(bi, prim_i[bi])][max(bi, prim_i[bi])]++;
            prim_u[bi] = 1;
            F1(i, L-1) if (!prim_u[i]) {
                int nd = dfast(a[sid][g[bi]], a[sid][g[i]]);
                if (nd < prim_d[i]) {
                    prim_d[i] = nd;
                    prim_i[i] = bi;
                }
            }
        }
        //PR(sid, vector<pii>(prim_e[sid] + 1, prim_e[sid] + L));
    }
    F0(j, L) F0(i, j) {
        double p = 1.0 * e_freq[i][j] / SAMPLES;
        if (p) ret += -p * log2(p);
    }
    return ret;
}

int f[16][16][16][16];
void AddIt(int i1, int i2, int j1, int j2) {
    if (j1 > j2) swap(j1, j2);
    f[i1][i2][j1][j2]++;
}

double EvalQuery(const vector<int>& g) {
    if (::L >= 8) return EvalOld(g);
    int L = SZ(g);
    AT(4);

    F0(i2, L) F0(i1, i2) F0(j2, L) F0(j1, j2) {
        f[i1][i2][j1][j2] = 0;
    }

    F0(sid, SAMPLES) {
        F0(i, L) {
            graph[i].clear();
        }

        F1(i, L-1) { prim_d[i] = inf; prim_u[i] = 0; }
        prim_u[0] = 1;
        F1(i, L-1) {
            prim_d[i] = dfast(a[sid][g[0]], a[sid][g[i]]);
            prim_i[i] = 0;
        }
        F1(step, L-1) {
            int best_d = inf;
            int bi = -1;
            F1(i, L-1) if (!prim_u[i] && prim_d[i] < best_d) {
                best_d = prim_d[i];
                bi = i;
            }
            //prim_e[sid][step] = pii(bi, prim_i[bi]);
            graph[bi].push_back(prim_i[bi]);
            graph[prim_i[bi]].push_back(bi);
            prim_u[bi] = 1;
            F1(i, L-1) if (!prim_u[i]) {
                int nd = dfast(a[sid][g[bi]], a[sid][g[i]]);
                if (nd < prim_d[i]) {
                    prim_d[i] = nd;
                    prim_i[i] = bi;
                }
            }
        }

        DFS(0);
        F0(i2, L) F0(i1, i2) {
            if (g_par[i2] == i1 || g_par[i1] == i2) continue;
            int x = i1, y = i2;
            while (x != y) {
                if (g_h[x] < g_h[y]) swap(x, y);
                AddIt(i1, i2, x, g_par[x]);
                x = g_par[x];
            }
        }
    }
    double ret = 0.0;
    F0(i2, L) F0(i1, i2) for (int j1 = i1; j1 < L; j1++) {
        int st = j1 + 1;
        if (i1 == j1) st = i2 + 1;
        for (int j2 = st; j2 < L; j2++) {
            ret += logs[f[i1][i2][j1][j2]][f[j1][j2][i1][i2]];
        }
    }
    return ret;
}


void OnlineUpdate() {
    AT(5);
    F0(sid, SAMPLES) {
        if (scores[sid] == 0) continue;
        Resolve(sid);
    }
}

void Prep(int sid) {
    AT(11);
    if (L == 3) {
        return;
    }

    set<pii> allp;
    F0(i, n) p[i] = a[sid][i];

    F0(i, n) {
        int w = 1;
        int tries = 0;
        while (allp.count(p[i])) {
            c_o[19]++;
            if ((++tries) % 10 == 0) w++;
            if (my.nextInt(2))
                p[i].first = a[sid][i].first + my.nextInt(-w, w);
            else
                p[i].second = a[sid][i].second + my.nextInt(-w, w);
        }
        allp.insert(p[i]);
    }

    vector<double> coords;
    F0(i, n) {
        coords.push_back(p[i].first);
        coords.push_back(p[i].second);
    }
    delaunator::Delaunator d(coords);
    if (d.triangles.empty()) {
        return;
    }

    F0(i, n) e[sid][i].clear();

    for (int i = 0; i < SZ(d.triangles); i += 3) {
        int x = d.triangles[i];
        int y = d.triangles[i + 1];
        int z = d.triangles[i + 2];
        if ((int)d.halfedges[i] < i) {
            e[sid][x].push_back(y);
            e[sid][y].push_back(x);
        }
        if ((int)d.halfedges[i + 1] < i + 1) {
            e[sid][y].push_back(z);
            e[sid][z].push_back(y);
        }
        if ((int)d.halfedges[i + 2] < i + 2) {
            e[sid][x].push_back(z);
            e[sid][z].push_back(x);
        }
    }
}

void Queries() {
    int Q_left = Q;

    vector<int> ids;
    F0(i, n) ids.push_back(i);

    SAMPLES = min(SAMPLEN[L] + 0, MAX_SAMPLES);
    F0(sid, SAMPLES) F0(i, n) a[sid][i] = RandPoint(i);

    int counter = 0;

    int sid = 0, sid2 = 0;

    while (Q_left > 0) {
        if ((counter++) % 10 == 0) {
            sid = my.nextInt(SAMPLES);
            sid2 = sid;
            while (sid2 == sid) sid2 = my.nextInt(SAMPLES);
            Prep(sid);
            Prep(sid2);
        }
        RS(ids);
        if (L == 3) {
            double max_entropy = -10.0;
            for (int ni = 0; ni < n / 5; ni++) F0(i2, 3) F0(i1, i2) {
                int node = ids[ni];
                g.clear();
                g.push_back(node);
                g.push_back(adj[node][i1].second);
                g.push_back(adj[node][i2].second);
                F0(i, L) cnt[i] = 0;

                F0(sid, SAMPLES) F0(i, L) F0(j, i) pre_d[sid][i][j] = pre_d[sid][j][i] = dfast(a[sid][g[i]], a[sid][g[j]]);

                F0(sid, SAMPLES) {
                    int x = 0;
                    F1(y, 2) {
                        if (pre_d[sid][x][(x + 1) % 3] < pre_d[sid][y][(y + 1) % 3]) x = y;
                    }
                    cnt[x]++;
                }
                double entropy = 0.0;
                F0(i, L) {
                    double p = 1.0 * cnt[i] / SAMPLES;
                    if (p) entropy += -p * log2(p);
                }
                if (entropy > max_entropy) {
                    max_entropy = entropy;
                    bg = g;
                }
            }
        } else {
            AT(1);
            double bv = -1;

            F0(ni, ITS[L]) {
                int center = ids[ni];

                Discriminate(sid, sid2, center);

                if (g.empty()) continue;

                double av = EvalQuery(g);
                if (av > bv) {
                    bv = av;
                    bg = g;
                }
            }

            F0(x, POST[L]) {
                g = bg;
                vz++;
                F0(i, L) v_in[g[i]] = vz;
                int index = my.nextInt(L);
                int i = g[index];
                int j = REL(e[my.nextInt(2) ? sid : sid2][i]);
                if (v_in[j] != vz) {
                    g[my.nextInt(L)] = j;
                    double av = EvalQuery(g);
                    if (av > bv) {
                        bv = av;
                        bg = g;
                    }
                }
            }
        }

        Analyze(bg);

        OnlineUpdate();

        Q_left--;
    }
    PR(conflicts, pointless, interesting, duplicates);

    PR(vector<ll>(scores, scores + SAMPLES));
    //ShowCycles();

    PR("PRE_SAMPLING", Elapsed());
    CL(0, d);

    F0(sid, SAMPLES) {
        samples++;
        F0(i, n) F0(j, i) d[i][j] += dfast(a[sid][i], a[sid][j]);
    }

    ExpandSamples(SAMPLES);
    PR("EXTRA_STEPS", Elapsed());

    if (samples) {
        F0(i, n) F0(j, i) {
            d[i][j] /= samples;
            d[i][j] = sqrt(d[i][j]);
            d[j][i] = d[i][j];
        }
    } else {
        exit(0);
    }
}

int nei[N][N], nein[N];
int sub_sz[N];
int current_sum;
ll current_score;
ll f1[N], f2[N];
int i1, i2;
ll current_hash;
int cuse[N*N];
int csize[N], par[N];
int A = 0, B = 0, C = 0, D = 0;
vector<pair<pii, int>> cuts;
vector<pii> c;

void Go(int i, int pr = -1) {
    sub_sz[i] = 1;
    v_in[i] = vz;
    csize[vz]++;
    F0(index, nein[i]) {
        int j = nei[i][index];
        if (j != pr) {
            par[j] = i;
            Go(j, i);
            sub_sz[i] += sub_sz[j];
        }
    }
}

void Upd(int id, int add = 1) {
    int x = c[id].first, y = c[id].second;
    current_sum += add * d[x][y];
    cuse[id] += add;
    if (add == 1) {
        nei[x][nein[x]++] = y;
        nei[y][nein[y]++] = x;
    } else {
        F0(index, nein[x]) if (nei[x][index] == y) {
            swap(nei[x][index], nei[x][--nein[x]]);
            break;
        }
        F0(index, nein[y]) if (nei[y][index] == x) {
            swap(nei[y][index], nei[y][--nein[y]]);
            break;
        }
    }
}

void CalcSizes() {
    vz++;
    F0(x, n) if (v_in[x] != vz) {
        q.clear();
        q.push_back(x);
        v_in[x] = vz;
        F0(qi, SZ(q)) {
            int i = q[qi];
            F0(index, nein[i]) {
                int j = nei[i][index];
                if (v_in[j] != vz) {
                    v_in[j] = vz;
                    q.push_back(j);
                }
            }
        }
        f1[SZ(q)]++;
    }
}

void ConstructSol() {
    vector<int> gtaken(m, 0);
    vector<int> gid(n, -1);
    sol.Init();

    vz++;
    F0(x, n) if (v_in[x] != vz) {
        q.clear();
        q.push_back(x);
        v_in[x] = vz;
        F0(qi, SZ(q)) {
            int i = q[qi];
            F0(index, nein[i]) {
                int j = nei[i][index];
                if (v_in[j] != vz) {
                    v_in[j] = vz;
                    q.push_back(j);
                }
            }
        }
        int j = 0;
        while (gtaken[j] || SZ(q) != G[j]) j++;
        gtaken[j] = 1;
        for (int x : q) {
            sol.group_cities[j].push_back(x);
            gid[x] = j;
        }
    }
}

void AddBad() {
    vector<int> ord;
    F0(i, n) ord.push_back(i);
    int at = 0;
    F0(i, m) {
        vector<int> group;
        F0(j, G[i]) {
            group.push_back(ord[at++]);
        }
        vector<pii> edges;
        F0(j, G[i] - 1) edges.emplace_back(group[j], group[j + 1]);
        sol.group_cities.push_back(group);
        sol.group_edges.push_back(edges);
    }
}

void UpdFreq(int i, int sgn) {
    current_hash ^= zh[i][f1[i]];
    current_score -= abs(f1[i] - f2[i]);
    f1[i] += sgn;
    current_hash ^= zh[i][f1[i]];
    current_score += abs(f1[i] - f2[i]);
}

// adding edge
void Make(int sgn) {
    UpdFreq(A, -sgn);
    UpdFreq(B, -sgn);
    UpdFreq(A + B, sgn);
}

struct State {
    int step;
    int score;
    int sum;
    int cid;
    pii last_move;
    int prev;
    bool operator<(const State& o) const {
        if (sum != o.sum) return sum < o.sum;
        if (prev != o.prev) return prev > o.prev;
        return cid < o.cid;
    }
};

State st[1<<20];
int sn;
set<State> S[N];
unordered_map<ll, int> ah;
int at_stack[1<<16], step;
int SL = 128;

void RevertLast() {
    State& s = st[at_stack[step--]];
    A = s.last_move.first;
    B = s.last_move.second;
    if (s.step % 2 == 1) {
        // was deletion
        Upd(s.cid, 1);
        Make(1);
    } else {
        // was addition
        Upd(s.cid, -1);
        Make(-1);
    }
}

void MakeMove(int state_id) {
    State& s = st[state_id];
    A = s.last_move.first;
    B = s.last_move.second;
    // is addition
    if (step % 2 == 1) {
        Upd(s.cid, 1);
        Make(1);
    } else {
        // is deletion
        Upd(s.cid, -1);
        Make(-1);
    }
    at_stack[++step] = state_id;
}

void Reach(int state_id) {
    while (step > st[state_id].step) RevertLast();
    if (at_stack[step] == state_id) return;
    Reach(st[state_id].prev);
    MakeMove(state_id);
}

bool LinkCut() {
    if (m == 1) {
        AddBad();
        return true;
    }

    dsu uf(n);
    vector<pair<int, pii>> edges;
    F0(i1, n) F0(i0, i1) edges.emplace_back(d[i0][i1], pii(i0, i1));
    sort(edges.begin(), edges.end());

    current_sum = 0;
    F0(i, m) f2[G[i]]++;

    int target_c = 3000;
    if (m == 2) target_c = 4000;
    if (m == 2) SL *= 4;
    if (m == 3) SL *= 2;

    vector<pii> not_inserted;
    for (auto e : edges) {
        int x = e.second.first, y = e.second.second;
        if (uf.same(x, y)) {
            not_inserted.push_back(e.second);
            continue;
        }
        uf.merge(x, y);
        c.push_back(e.second);
        if (SZ(c) == n - 1 && SZ(c) + SZ(not_inserted) >= target_c) break;
    }

    PR(SZ(c));
    for (auto x : not_inserted) {
        c.push_back(x);
        if (SZ(c) == target_c) break;
    }
    PR(SZ(c));

    F0(i, n - m) {
        Upd(i, 1);
    }
    CalcSizes();

    current_score = 0;
    F1(i, n) current_score += abs(f1[i] - f2[i]);
    PR(current_score, current_sum);

    State starting_state;
    starting_state.step = 0;
    starting_state.score = current_score;
    starting_state.sum = current_sum;
    starting_state.prev = -1;
    ah[current_hash] = current_sum;
    S[current_score].insert(starting_state);

    //int iters = 0;
    double endTime = TL;
    while (1) {
        //PR(++brr);
        if (Elapsed() > endTime && bscore < 1e9) break;
        int ts = 0;
        pii l;
        for (int score_i = n; score_i >= 0; score_i--) if (!S[score_i].empty()) {
            ts += SZ(S[score_i]);
            l = pii(score_i, SZ(S[score_i]));
        }
        //PR(++iters, sn, SZ(ah), ts, l, bscore);
        if (!ts) break;
        for (int score_i = min(n, l.first + 0); score_i >= 0; score_i--) if (!S[score_i].empty()) {
        //for (int score_i = 0; score_i <= min(n, l.first + 10); score_i++) if (!S[score_i].empty()) {
            State s = *(S[score_i].begin());
            S[score_i].erase(S[score_i].begin());
            st[sn] = s;
            Reach(sn);

            if (score_i == 0) {
                if (s.sum < bscore) {
                    PR(endTime - Elapsed(), s.sum);
                    vector<int> v;
                    F0(sid, step + 1) {
                        v.push_back(st[at_stack[sid]].score);
                    }
                    bscore = s.sum;
                    ConstructSol();
                }
            }

            if (ah.find(current_hash) != ah.end() && ah[current_hash] < s.sum) {
                RevertLast();
                continue;
            }
            sn++;

            vz = 0;
            F0(x, n) v_in[x] = 0;
            F1(i, m + 1) csize[i] = 0;
            F0(x, n) if (!v_in[x]) {
                vz++;
                Go(x);
            }

            if (step % 2 == 0) {
                F0(i, SZ(c)) if (cuse[i]) {
                    int upper = c[i].first, lower = c[i].second;
                    if (sub_sz[upper] < sub_sz[lower]) swap(upper, lower);
                    int id = v_in[c[i].first];
                    A = sub_sz[lower];
                    B = csize[id] - A;
                    Upd(i, -1);
                    Make(-1);

                    auto& Q = S[current_score];
                    ll last_sum = current_sum + 1;
                    if (SZ(Q) >= SL) last_sum = (--Q.end())->sum;
                    if (current_sum < last_sum) {
                        if (ah.find(current_hash) == ah.end() || ah[current_hash] > current_sum) {
                            ah[current_hash] = current_sum;
                            State ns = s;
                            ns.step = s.step + 1;
                            ns.score = current_score;
                            ns.sum = current_sum;
                            ns.cid = i;
                            ns.last_move = pii(A, B);
                            ns.prev = sn - 1;
                            Q.insert(ns);
                            if (SZ(Q) > SL) Q.erase(--Q.end());
                        }
                    }

                    Upd(i, 1);
                    Make(1);
                }
            } else {
                F0(i, SZ(c)) if (!cuse[i]) {
                    if (v_in[c[i].first] == v_in[c[i].second]) continue;

                    A = csize[v_in[c[i].first]];
                    B = csize[v_in[c[i].second]];

                    Upd(i, 1);
                    Make(1);

                    auto& Q = S[current_score];
                    ll last_sum = current_sum + 1;
                    if (SZ(Q) >= SL) last_sum = (--Q.end())->sum;
                    if (current_sum < last_sum) {
                        if (ah.find(current_hash) == ah.end() || ah[current_hash] > current_sum) {
                            ah[current_hash] = current_sum;
                            State ns = s;
                            ns.step = s.step + 1;
                            ns.score = current_score;
                            ns.sum = current_sum;
                            ns.cid = i;
                            ns.last_move = pii(A, B);
                            ns.prev = sn - 1;
                            Q.insert(ns);
                            if (SZ(Q) > SL) Q.erase(--Q.end());
                        }
                    }

                    Upd(i, -1);
                    Make(-1);
                }
            }
        }
    }
    return true;
}

void Solve() {
    Prepare();

    Queries();

    LinkCut();

    UpdateBest();

    PR("print");

    PR(bsol.TotLen());
    F0(i, m) {
        bsol.MstLen(i, true);
    }
    bsol.Print();

    //Report();
}


void ReadInput() {
    cin >> n >> m >> Q >> L >> W;
    F0(i, m) cin >> G[i];
    F0(i, n) F0(j, 4) cin >> COORD[j][i];
    if (LOCAL_RUN) {
        F0(i, n) cin >> real_pos[i];
    }
}

int main(int argc, char* argv[]) {
    Init();

    int seed1 = 0, seed2 = 0;
    if(argc>1) {
        if (argc == 2) {
            seed1 = seed2 = atoi(argv[1]);
        } else {
            seed1 = atoi(argv[1]);
            seed2 = atoi(argv[2]);
        }
        STANDARD=0;
    }

    if(STANDARD) {
        ReadInput();
        Solve();
        return 0;
    }
    LOCAL_RUN = 1;

    for (int seed=seed1; seed<=seed2; seed++) {
        if(seed>=0 && seed<10000) {
            char inp[128];
            sprintf(inp, "in/%04d.txt", seed);
            char outp[128];
            sprintf(outp, "out/%04d.txt", seed);
            ignore = freopen(inp, "r", stdin);
            ignore = freopen(outp, "w", stdout);
            ReadInput();
            Solve();
            cerr << "Seed #" << seed << " ";
            cerr << bscore << endl;
            //cout << "Score would be " << bscore << endl;
        } else {
            // Generate
            throw;
            Rand my(seed);
        }
    }

    return 0;
}
