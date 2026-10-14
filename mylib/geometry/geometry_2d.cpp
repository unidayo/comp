// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/geometry/geometry_2d.cpp
//
// 2 次元幾何の基本ライブラリ。詳細は geometry_2d.md を参照。
// 整数座標の dot / cross / orientation は __int128_t で計算する。

#include <bits/stdc++.h>
using namespace std;

namespace geometry2d {

using Real = long double;
constexpr Real EPS = 1e-12L;
constexpr Real PI = numbers::pi_v<Real>;

template <class T>
using Calc = conditional_t<is_integral_v<T>, __int128_t, long double>;

template <class T>
struct Point {
    T x{}, y{};

    Point() = default;
    Point(T x_, T y_) : x(x_), y(y_) {}

    template <class U>
    explicit Point(const Point<U>& p) : x(static_cast<T>(p.x)), y(static_cast<T>(p.y)) {}

    Point operator+() const { return *this; }
    Point operator-() const { return {-x, -y}; }
    Point operator+(const Point& p) const { return {x + p.x, y + p.y}; }
    Point operator-(const Point& p) const { return {x - p.x, y - p.y}; }
    Point operator*(const T& k) const { return {x * k, y * k}; }
    Point operator/(const T& k) const { return {x / k, y / k}; }

    Point& operator+=(const Point& p) { return x += p.x, y += p.y, *this; }
    Point& operator-=(const Point& p) { return x -= p.x, y -= p.y, *this; }
    Point& operator*=(const T& k) { return x *= k, y *= k, *this; }
    Point& operator/=(const T& k) { return x /= k, y /= k, *this; }

    friend Point operator*(const T& k, const Point& p) { return p * k; }
    friend bool operator==(const Point&, const Point&) = default;
    friend bool operator<(const Point& a, const Point& b) {
        return tie(a.x, a.y) < tie(b.x, b.y);
    }
    friend istream& operator>>(istream& is, Point& p) { return is >> p.x >> p.y; }
    friend ostream& operator<<(ostream& os, const Point& p) { return os << p.x << ' ' << p.y; }
};

template <class T>
Calc<T> dot(const Point<T>& a, const Point<T>& b) {
    return Calc<T>(a.x) * b.x + Calc<T>(a.y) * b.y;
}

template <class T>
Calc<T> cross(const Point<T>& a, const Point<T>& b) {
    return Calc<T>(a.x) * b.y - Calc<T>(a.y) * b.x;
}

template <class T>
Calc<T> cross(const Point<T>& o, const Point<T>& a, const Point<T>& b) {
    using C = Calc<T>;
    return (C(a.x) - C(o.x)) * (C(b.y) - C(o.y)) -
           (C(a.y) - C(o.y)) * (C(b.x) - C(o.x));
}

// (a-o) dot (b-o)
template <class T>
Calc<T> dot(const Point<T>& o, const Point<T>& a, const Point<T>& b) {
    using C = Calc<T>;
    return (C(a.x) - C(o.x)) * (C(b.x) - C(o.x)) +
           (C(a.y) - C(o.y)) * (C(b.y) - C(o.y));
}

// (b-a) cross (d-c)
template <class T>
Calc<T> cross_directions(const Point<T>& a, const Point<T>& b,
                         const Point<T>& c, const Point<T>& d) {
    using C = Calc<T>;
    return (C(b.x) - C(a.x)) * (C(d.y) - C(c.y)) -
           (C(b.y) - C(a.y)) * (C(d.x) - C(c.x));
}

// (b-a) dot (d-c)
template <class T>
Calc<T> dot_directions(const Point<T>& a, const Point<T>& b,
                       const Point<T>& c, const Point<T>& d) {
    using C = Calc<T>;
    return (C(b.x) - C(a.x)) * (C(d.x) - C(c.x)) +
           (C(b.y) - C(a.y)) * (C(d.y) - C(c.y));
}

template <class T>
Calc<T> norm2(const Point<T>& p) {
    return dot(p, p);
}

template <class T>
Real norm(const Point<T>& p) {
    return hypot(Real(p.x), Real(p.y));
}

template <class T>
Real distance(const Point<T>& a, const Point<T>& b) {
    return hypot(Real(a.x) - Real(b.x), Real(a.y) - Real(b.y));
}

template <class T>
int sign(Calc<T> x) {
    if constexpr (is_integral_v<T>) {
        return (x > 0) - (x < 0);
    } else {
        return (x > EPS) - (x < -EPS);
    }
}

template <class T>
int compare(T a, T b) {
    if constexpr (is_integral_v<T>) {
        return (a > b) - (a < b);
    } else {
        return (a > b + EPS) - (a + EPS < b);
    }
}

// 整数は完全一致、浮動小数点数は各座標を EPS で比較する。
template <class T>
bool same_point(const Point<T>& a, const Point<T>& b) {
    return compare(a.x, b.x) == 0 && compare(a.y, b.y) == 0;
}

// +1: 反時計回り, -1: 時計回り, 0: 一直線上
template <class T>
int orientation(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
    if constexpr (is_integral_v<T>) {
        return sign<T>(cross(a, b, c));
    } else {
        Point<Real> u{Real(b.x) - Real(a.x), Real(b.y) - Real(a.y)};
        Point<Real> v{Real(c.x) - Real(a.x), Real(c.y) - Real(a.y)};
        Real su = max(abs(u.x), abs(u.y));
        Real sv = max(abs(v.x), abs(v.y));
        if (su == 0 || sv == 0) return 0;
        u /= su;
        v /= sv;
        Real value = cross(u, v);
        // 外積を構成する 2 積の大きさに対する相対誤差で判定する。
        // 角度だけを基準にすると、遠方にある平行移動した点まで直線上と
        // 誤認し得るため、平行判定の許容角とは分ける。
        Real tolerance = EPS * (abs(u.x * v.y) + abs(u.y * v.x));
        return (value > tolerance) - (value < -tolerance);
    }
}

template <class T>
Point<Real> to_real(const Point<T>& p) {
    return {Real(p.x), Real(p.y)};
}

namespace detail {

inline bool scaled_parallel(Point<Real> u, Point<Real> v) {
    Real su = max(abs(u.x), abs(u.y));
    Real sv = max(abs(v.x), abs(v.y));
    assert(su > 0 && sv > 0);
    u /= su;
    v /= sv;
    return abs(cross(u, v)) <= EPS * norm(u) * norm(v);
}

inline bool scaled_orthogonal(Point<Real> u, Point<Real> v) {
    Real su = max(abs(u.x), abs(u.y));
    Real sv = max(abs(v.x), abs(v.y));
    assert(su > 0 && sv > 0);
    u /= su;
    v /= sv;
    return abs(dot(u, v)) <= EPS * norm(u) * norm(v);
}

}  // namespace detail

// 2 ベクトルの向き付けなしの角 [0, PI]。ゼロベクトルなら nullopt。
template <class T>
optional<Real> angle_between_vectors(const Point<T>& u, const Point<T>& v) {
    Point<Real> ur = to_real(u), vr = to_real(v);
    Real su = max(abs(ur.x), abs(ur.y));
    Real sv = max(abs(vr.x), abs(vr.y));
    if (su == 0 || sv == 0) return nullopt;
    ur /= su;
    vr /= sv;
    return atan2(abs(cross(ur, vr)), dot(ur, vr));
}

// 中央の b を頂点とする ∠abc [0, PI]。a == b または b == c なら nullopt。
template <class T>
optional<Real> angle(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
    return angle_between_vectors(to_real(a) - to_real(b), to_real(c) - to_real(b));
}

template <class T>
Point<Real> rotate(const Point<T>& p, Real theta) {
    Real c = cos(theta), s = sin(theta);
    return {Real(p.x) * c - Real(p.y) * s, Real(p.x) * s + Real(p.y) * c};
}

template <class T>
Point<Real> rotate(const Point<T>& p, const Point<T>& center, Real theta) {
    Point<Real> relative = to_real(p) - to_real(center);
    return to_real(center) + rotate(relative, theta);
}

template <class T>
struct Line {
    Point<T> a, b;
};

template <class T>
struct LineEquation {
    T a{}, b{}, c{};  // ax + by + c = 0
};

template <class T>
struct Segment {
    Point<T> a, b;
};

template <class T>
bool is_degenerate(const Line<T>& l) {
    return l.a == l.b;
}

template <class T>
bool is_degenerate(const Segment<T>& s) {
    return s.a == s.b;
}

// 線分の 2 端点を通る無限直線。退化線分からは退化直線ができる。
template <class T>
Line<T> supporting_line(const Segment<T>& s) {
    return {s.a, s.b};
}

// 点 p を通り、非ゼロベクトル direction と平行な直線。
inline Line<Real> line_from_point_direction(const Point<Real>& p, Point<Real> direction) {
    Real direction_scale = max(abs(direction.x), abs(direction.y));
    assert(direction_scale > 0);
    direction /= direction_scale;
    Real point_scale = max({Real(1), abs(p.x), abs(p.y)});
    // p が大きいと単位ステップは加算で消え、逆にステップが
    // 小さすぎると b-a の有効桁が減る。機械イプシロンの 1/4 乗で釣り合いを取る。
    Real step = point_scale * sqrt(sqrt(numeric_limits<Real>::epsilon()));
    return {p, p + direction * step};
}

// 2 点形式の直線から ax + by + c = 0 の係数を得る。
// 整数座標なら係数は __int128_t。係数の正規化は行わない。
template <class T>
LineEquation<Calc<T>> line_equation(const Line<T>& l) {
    assert(!is_degenerate(l));
    using C = Calc<T>;
    C a = C(l.a.y) - C(l.b.y);
    C b = C(l.b.x) - C(l.a.x);
    C c = -(a * C(l.a.x) + b * C(l.a.y));
    return {a, b, c};
}

// ax + by + c = 0 から直線上の 2 点を作る。(a, b) != (0, 0) が必要。
// 通過点が元の係数型で表せないことがあるため Line<Real> を返す。
template <class T>
Line<Real> line_from_equation(const LineEquation<T>& equation) {
    Real a = Real(equation.a), b = Real(equation.b), c = Real(equation.c);
    Real scale = max(abs(a), abs(b));
    assert(scale > 0);
    a /= scale;
    b /= scale;
    c /= scale;
    Real denominator = a * a + b * b;
    Point<Real> p{-a * c / denominator, -b * c / denominator};
    Point<Real> direction{-b, a};
    return line_from_point_direction(p, direction);
}

template <class T>
Line<Real> line_from_equation(T a, T b, T c) {
    return line_from_equation(LineEquation<T>{a, b, c});
}

template <class T>
bool parallel(const Line<T>& l, const Line<T>& m) {
    assert(!is_degenerate(l));
    assert(!is_degenerate(m));
    if constexpr (is_integral_v<T>) {
        return cross_directions(l.a, l.b, m.a, m.b) == 0;
    } else {
        return detail::scaled_parallel(to_real(l.b) - to_real(l.a),
                                       to_real(m.b) - to_real(m.a));
    }
}

template <class T>
bool orthogonal(const Line<T>& l, const Line<T>& m) {
    assert(!is_degenerate(l));
    assert(!is_degenerate(m));
    if constexpr (is_integral_v<T>) {
        return dot_directions(l.a, l.b, m.a, m.b) == 0;
    } else {
        return detail::scaled_orthogonal(to_real(l.b) - to_real(l.a),
                                         to_real(m.b) - to_real(m.a));
    }
}

// 向きを持たない 2 直線の小さい方の角 [0, PI/2]。退化直線なら nullopt。
template <class T>
optional<Real> angle_between_lines(const Line<T>& l, const Line<T>& m) {
    if (is_degenerate(l) || is_degenerate(m)) return nullopt;
    auto theta = angle_between_vectors(to_real(l.b) - to_real(l.a),
                                       to_real(m.b) - to_real(m.a));
    return min(*theta, PI - *theta);
}

// a->b と s.a->s.b の向き付けなしの角 [0, PI]。退化線分なら nullopt。
template <class T>
optional<Real> angle_between_segments(const Segment<T>& s, const Segment<T>& t) {
    if (is_degenerate(s) || is_degenerate(t)) return nullopt;
    return angle_between_vectors(to_real(s.b) - to_real(s.a),
                                 to_real(t.b) - to_real(t.a));
}

// 2 点 a, b から等距離にある点の集合（垂直二等分線）。
// a == b なら一意な直線にならないため nullopt。
template <class T>
optional<Line<Real>> perpendicular_bisector(const Point<T>& a, const Point<T>& b) {
    if (a == b) return nullopt;
    Point<Real> ar = to_real(a), br = to_real(b);
    Point<Real> midpoint = ar / 2.0L + br / 2.0L;
    Point<Real> direction = br - ar;
    Point<Real> perpendicular{-direction.y, direction.x};
    return line_from_point_direction(midpoint, perpendicular);
}

template <class T>
bool degenerate_triangle(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
    if constexpr (is_integral_v<T>) {
        return cross(a, b, c) == 0;
    } else {
        if (a == b || a == c) return true;
        return detail::scaled_parallel(to_real(b) - to_real(a), to_real(c) - to_real(a));
    }
}

// 三角形 abc の内心。退化三角形なら nullopt。
template <class T>
optional<Point<Real>> incenter(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
    if (degenerate_triangle(a, b, c)) return nullopt;
    Real wa = distance(b, c), wb = distance(c, a), wc = distance(a, b);
    Real scale = max({wa, wb, wc});
    wa /= scale;
    wb /= scale;
    wc /= scale;
    Real perimeter = wa + wb + wc;
    return to_real(a) * (wa / perimeter) + to_real(b) * (wb / perimeter) +
           to_real(c) * (wc / perimeter);
}

// 三角形 abc の外心。退化三角形なら nullopt。
template <class T>
optional<Point<Real>> circumcenter(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
    if (degenerate_triangle(a, b, c)) return nullopt;
    Point<Real> origin = to_real(a);
    Point<Real> u = to_real(b) - origin, v = to_real(c) - origin;
    Real scale = max({abs(u.x), abs(u.y), abs(v.x), abs(v.y)});
    u /= scale;
    v /= scale;

    Real denominator = 2 * cross(u, v);
    Real u2 = norm2(u), v2 = norm2(v);
    Point<Real> offset{(v.y * u2 - u.y * v2) / denominator,
                       (u.x * v2 - v.x * u2) / denominator};
    return origin + offset * scale;
}

template <class T>
bool on_line(const Line<T>& l, const Point<T>& p) {
    assert(!is_degenerate(l));
    if constexpr (is_integral_v<T>) {
        return cross(l.a, l.b, p) == 0;
    } else {
        Point<Real> a = to_real(l.a), direction = to_real(l.b) - a;
        direction /= max(abs(direction.x), abs(direction.y));
        Real perpendicular_distance = abs(cross(direction, to_real(p) - a)) / norm(direction);
        return perpendicular_distance <= EPS;
    }
}

// 通過点や向きが違っても、同じ無限直線を表すなら true。
template <class T>
bool same_line(const Line<T>& l, const Line<T>& m) {
    assert(!is_degenerate(l));
    assert(!is_degenerate(m));
    return parallel(l, m) && on_line(l, m.a) && on_line(l, m.b);
}

// 端点の位置や向きではなく、同じ無限直線かどうかを比較する。
template <class T>
bool operator==(const Line<T>& l, const Line<T>& m) {
    return same_line(l, m);
}

template <class T>
bool on_segment(const Segment<T>& s, const Point<T>& p) {
    if (is_degenerate(s)) return same_point(s.a, p);
    bool collinear;
    if constexpr (is_integral_v<T>) {
        collinear = cross(s.a, s.b, p) == 0;
    } else {
        Point<Real> a = to_real(s.a), direction = to_real(s.b) - a;
        direction /= max(abs(direction.x), abs(direction.y));
        Real perpendicular_distance = abs(cross(direction, to_real(p) - a)) / norm(direction);
        collinear = perpendicular_distance <= EPS;
    }
    return collinear &&
           compare(min(s.a.x, s.b.x), p.x) <= 0 &&
           compare(p.x, max(s.a.x, s.b.x)) <= 0 &&
           compare(min(s.a.y, s.b.y), p.y) <= 0 &&
           compare(p.y, max(s.a.y, s.b.y)) <= 0;
}

// 端点の順序を無視して同じ閉線分かを比較する。
template <class T>
bool same_segment(const Segment<T>& s, const Segment<T>& t) {
    return (same_point(s.a, t.a) && same_point(s.b, t.b)) ||
           (same_point(s.a, t.b) && same_point(s.b, t.a));
}

template <class T>
bool operator==(const Segment<T>& s, const Segment<T>& t) {
    return same_segment(s, t);
}

// l は非退化直線であること。
template <class T>
Point<Real> projection(const Line<T>& l, const Point<T>& p) {
    assert(!is_degenerate(l));
    Point<Real> a = to_real(l.a), d = to_real(l.b) - a;
    Real scale = max(abs(d.x), abs(d.y));
    d /= scale;
    Real length2 = dot(d, d);
    Real offset = dot(to_real(p) - a, d) / length2;
    return a + d * offset;
}

template <class T>
Point<Real> reflection(const Line<T>& l, const Point<T>& p) {
    return projection(l, p) * 2.0L - to_real(p);
}

template <class T>
Real distance(const Line<T>& l, const Point<T>& p) {
    assert(!is_degenerate(l));
    Point<Real> a = to_real(l.a), d = to_real(l.b) - a;
    Real scale = max(abs(d.x), abs(d.y));
    d /= scale;
    return abs(cross(d, to_real(p) - a)) / norm(d);
}

template <class T>
Real distance_to_segment(const Segment<T>& s, const Point<T>& p) {
    if (s.a == s.b) return geometry2d::distance(s.a, p);
    if constexpr (is_integral_v<T>) {
        if (dot(s.a, s.b, p) < 0) return geometry2d::distance(s.a, p);
        if (dot(s.b, s.a, p) < 0) return geometry2d::distance(s.b, p);
    } else {
        Point<Real> a = to_real(s.a), b = to_real(s.b), pr = to_real(p);
        Point<Real> d = b - a;
        Real scale = max(abs(d.x), abs(d.y));
        d /= scale;
        if (dot(d, pr - a) < 0) return geometry2d::distance(s.a, p);
        if (dot(-d, pr - b) < 0) return geometry2d::distance(s.b, p);
    }
    return geometry2d::distance(Line<T>{s.a, s.b}, p);
}

template <class T>
bool intersects(const Segment<T>& s, const Segment<T>& t) {
    int a = orientation(s.a, s.b, t.a), b = orientation(s.a, s.b, t.b);
    int c = orientation(t.a, t.b, s.a), d = orientation(t.a, t.b, s.b);
    if (on_segment(s, t.a) || on_segment(s, t.b) ||
        on_segment(t, s.a) || on_segment(t, s.b))
        return true;
    return a * b < 0 && c * d < 0;
}

template <class T>
bool properly_intersects(const Segment<T>& s, const Segment<T>& t) {
    return orientation(s.a, s.b, t.a) * orientation(s.a, s.b, t.b) < 0 &&
           orientation(t.a, t.b, s.a) * orientation(t.a, t.b, s.b) < 0;
}

template <class T>
Real distance(const Segment<T>& s, const Segment<T>& t) {
    if (intersects(s, t)) return 0;
    return min({distance_to_segment(s, t.a), distance_to_segment(s, t.b),
                distance_to_segment(t, s.a), distance_to_segment(t, s.b)});
}

// 一意な交点があるときだけ返す。平行または同一直線なら nullopt。
template <class T>
optional<Point<Real>> line_intersection(const Line<T>& l, const Line<T>& m) {
    assert(!is_degenerate(l));
    assert(!is_degenerate(m));
    if (parallel(l, m)) return nullopt;
    Point<Real> a = to_real(l.a), b = to_real(l.b);
    Point<Real> c = to_real(m.a), d = to_real(m.b);
    Point<Real> u = b - a, v = d - c;
    u /= max(abs(u.x), abs(u.y));
    v /= max(abs(v.x), abs(v.y));
    return a + u * (cross(c - a, v) / cross(u, v));
}

// 直線と閉線分が 1 点以上を共有するか。l は非退化直線であること。
template <class T>
bool intersects(const Line<T>& l, const Segment<T>& s) {
    assert(!is_degenerate(l));
    if (on_line(l, s.a) || on_line(l, s.b)) return true;
    return orientation(l.a, l.b, s.a) * orientation(l.a, l.b, s.b) < 0;
}

template <class T>
bool intersects(const Segment<T>& s, const Line<T>& l) {
    return intersects(l, s);
}

// 0 個: 交わらない, 1 個: 1 点で交わる, 2 個: 線分全体が直線上。
template <class T>
vector<Point<Real>> intersections(const Line<T>& l, const Segment<T>& s) {
    if (!intersects(l, s)) return {};
    if (is_degenerate(s)) return {to_real(s.a)};

    bool a_on_line = on_line(l, s.a), b_on_line = on_line(l, s.b);
    if (a_on_line && b_on_line) return {to_real(s.a), to_real(s.b)};
    if (a_on_line) return {to_real(s.a)};
    if (b_on_line) return {to_real(s.b)};
    if (auto p = line_intersection(l, supporting_line(s))) return {*p};
    return {};
}

template <class T>
vector<Point<Real>> intersections(const Segment<T>& s, const Line<T>& l) {
    return intersections(l, s);
}

// 0 個: 交わらない, 1 個: 1 点で交わる, 2 個: 重なり区間の両端。
template <class T>
vector<Point<Real>> segment_intersection(const Segment<T>& s, const Segment<T>& t) {
    if (!intersects(s, t)) return {};
    if (is_degenerate(s)) return {to_real(s.a)};
    if (is_degenerate(t)) return {to_real(t.a)};

    vector<Point<T>> candidates;
    for (const Point<T>& p : {s.a, s.b, t.a, t.b})
        if (on_segment(s, p) && on_segment(t, p)) candidates.push_back(p);
    sort(candidates.begin(), candidates.end());
    candidates.erase(unique(candidates.begin(), candidates.end(),
                            [](const Point<T>& a, const Point<T>& b) {
                                return same_point(a, b);
                            }),
                     candidates.end());
    if (candidates.size() == 1) return {to_real(candidates[0])};
    if (candidates.size() >= 2)
        return {to_real(candidates.front()), to_real(candidates.back())};

    if (auto p = line_intersection(supporting_line(s), supporting_line(t))) return {*p};
    return {};
}

struct Circle {
    Point<Real> center;
    Real radius{};
};

// 点 p を通る円 c の接線。p が内部なら 0 本、円周上なら 1 本、外部なら 2 本。
template <class T>
vector<Line<Real>> tangent_lines(const Circle& c, const Point<T>& p) {
    assert(c.radius > 0);
    Point<Real> pr = to_real(p), v = pr - c.center;
    Real d = norm(v);
    if (d == 0) return {};
    Real tolerance = EPS * max(d, c.radius);
    if (d < c.radius - tolerance) return {};

    Point<Real> unit = v / d;
    Point<Real> perpendicular{-unit.y, unit.x};
    if (abs(d - c.radius) <= tolerance)
        return {line_from_point_direction(pr, perpendicular)};

    Real ratio = c.radius / d;
    Real height = sqrt(max(Real(0), 1 - ratio * ratio));
    Point<Real> base = c.center + unit * (c.radius * ratio);
    Point<Real> offset = perpendicular * (c.radius * height);
    Point<Real> t1 = base - offset, t2 = base + offset;
    return {line_from_point_direction(pr, t1 - pr),
            line_from_point_direction(pr, t2 - pr)};
}

template <class T>
vector<Line<Real>> tangent_lines(const Point<T>& p, const Circle& c) {
    return tangent_lines(c, p);
}

// 2 円の共通接線。0～4 本。同心同半径の無限本は空配列で表す。
inline vector<Line<Real>> tangent_lines(const Circle& a, const Circle& b) {
    assert(a.radius > 0 && b.radius > 0);
    Point<Real> v = b.center - a.center;
    Real d = norm(v);
    if (d == 0) return {};

    Point<Real> unit = v / d;
    Point<Real> perpendicular{-unit.y, unit.x};
    vector<Line<Real>> result;
    for (int side_b : {-1, 1}) {
        Real ratio = (side_b * b.radius - a.radius) / d;
        if (abs(ratio) > 1 + EPS) continue;
        ratio = clamp(ratio, Real(-1), Real(1));
        Real height = sqrt(max(Real(0), 1 - ratio * ratio));
        int count = height <= EPS ? 1 : 2;
        for (int i = 0; i < count; ++i) {
            Real side = (i == 0 ? Real(-1) : Real(1));
            Point<Real> normal = unit * ratio + perpendicular * (side * height);
            Point<Real> tangent_point = a.center - normal * a.radius;
            Point<Real> direction{-normal.y, normal.x};
            result.push_back(line_from_point_direction(tangent_point, direction));
        }
    }
    return result;
}

// 直線と円の交点。接するときは 1 個。
inline vector<Point<Real>> intersections(const Line<Real>& l, const Circle& c) {
    assert(c.radius >= 0);
    Point<Real> h = projection(l, c.center);
    Real d = distance(h, c.center);
    Real tolerance = EPS * max(d, c.radius);
    if (d > c.radius + tolerance) return {};
    if (abs(d - c.radius) <= tolerance) return {h};
    if (c.radius == 0) return {h};
    Point<Real> direction = l.b - l.a;
    direction /= max(abs(direction.x), abs(direction.y));
    Point<Real> dir = direction / norm(direction);
    Real ratio = d / c.radius;
    Real x = c.radius * sqrt(max(Real(0), 1 - ratio * ratio));
    return {h - dir * x, h + dir * x};
}

// 線分と円周の交点。接するときは 1 個。円の内部だけにある線分は空配列。
template <class T>
vector<Point<Real>> intersections(const Segment<T>& s, const Circle& c) {
    assert(c.radius >= 0);
    Segment<Real> sr{to_real(s.a), to_real(s.b)};
    if (is_degenerate(sr)) {
        Real d = distance(sr.a, c.center);
        Real tolerance = EPS * max(abs(d), abs(c.radius));
        return abs(d - c.radius) <= tolerance ? vector<Point<Real>>{sr.a}
                                              : vector<Point<Real>>{};
    }

    vector<Point<Real>> result = intersections(supporting_line(sr), c);
    erase_if(result, [&](const Point<Real>& p) { return !on_segment(sr, p); });
    return result;
}

template <class T>
vector<Point<Real>> intersections(const Circle& c, const Segment<T>& s) {
    return intersections(s, c);
}

// 円の内部ではなく、円周と 1 点以上を共有するか。
template <class T>
bool intersects(const Segment<T>& s, const Circle& c) {
    return !intersections(s, c).empty();
}

template <class T>
bool intersects(const Circle& c, const Segment<T>& s) {
    return intersects(s, c);
}

// 2 円の交点。同心円（一致を含む）は表現できないため空配列を返す。
inline vector<Point<Real>> intersections(const Circle& a, const Circle& b) {
    assert(a.radius >= 0 && b.radius >= 0);
    Point<Real> v = b.center - a.center;
    Real d = norm(v);
    if (d == 0) return {};

    Real scale = max({d, a.radius, b.radius});
    Real dn = d / scale, ra = a.radius / scale, rb = b.radius / scale;
    if (dn > ra + rb + EPS || dn < abs(ra - rb) - EPS) return {};

    Real x = (ra * ra - rb * rb + dn * dn) / (2 * dn);
    Real h = sqrt(max(Real(0), ra * ra - x * x));
    Point<Real> unit = v / d;
    Point<Real> base = a.center + unit * (x * scale);
    if (h <= EPS) return {base};
    Point<Real> perpendicular{-unit.y, unit.x};
    h *= scale;
    return {base - perpendicular * h, base + perpendicular * h};
}

// 符号付き面積の 2 倍。反時計回りなら正。
template <class T>
Calc<T> signed_area2(const vector<Point<T>>& polygon) {
    Calc<T> res = 0;
    for (int i = 0, n = int(polygon.size()); i < n; ++i)
        res += cross(polygon[i], polygon[(i + 1) % n]);
    return res;
}

template <class T>
Real area(const vector<Point<T>>& polygon) {
    return abs(Real(signed_area2(polygon))) / 2;
}

enum class Containment { Outside, Boundary, Inside };

// 単純多角形用。頂点の向きは問わない。
template <class T>
Containment contains(const vector<Point<T>>& polygon, const Point<T>& p) {
    int winding = 0;
    for (int i = 0, n = int(polygon.size()); i < n; ++i) {
        Point<T> a = polygon[i], b = polygon[(i + 1) % n];
        if (on_segment(Segment<T>{a, b}, p)) return Containment::Boundary;
        int ay = compare(a.y, p.y), by = compare(b.y, p.y);
        int o = orientation(a, b, p);
        if (ay <= 0 && by > 0 && o > 0) ++winding;
        if (by <= 0 && ay > 0 && o < 0) --winding;
    }
    return winding == 0 ? Containment::Outside : Containment::Inside;
}

// 反時計回り、辞書順最小点から開始する凸包。
// keep_collinear=false なら辺上の不要な点を除く。
template <class T>
vector<Point<T>> convex_hull(vector<Point<T>> points, bool keep_collinear = false) {
    sort(points.begin(), points.end());
    points.erase(unique(points.begin(), points.end()), points.end());
    int n = int(points.size());
    if (n <= 2) return points;

    bool all_collinear = true;
    for (int i = 2; i < n; ++i)
        if (orientation(points[0], points[1], points[i]) != 0) all_collinear = false;
    if (all_collinear) {
        if (keep_collinear) return points;
        return {points.front(), points.back()};
    }

    vector<Point<T>> lower, upper;
    for (const auto& p : points) {
        while (lower.size() >= 2) {
            int o = orientation(lower[lower.size() - 2], lower.back(), p);
            if (o > 0 || (keep_collinear && o == 0)) break;
            lower.pop_back();
        }
        lower.push_back(p);
    }
    for (auto it = points.rbegin(); it != points.rend(); ++it) {
        while (upper.size() >= 2) {
            int o = orientation(upper[upper.size() - 2], upper.back(), *it);
            if (o > 0 || (keep_collinear && o == 0)) break;
            upper.pop_back();
        }
        upper.push_back(*it);
    }
    lower.pop_back();
    upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

}  // namespace geometry2d
