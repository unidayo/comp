#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
using namespace std;
using i8 = int8_t; using i16 = int16_t; using i32 = int32_t; using i64 = int64_t; using u8 = uint8_t; using u16 = uint16_t; using u32 = uint32_t; using u64 = uint64_t; using f32 = float; using f64 = double;
template <class T> inline T sq(T a) { return a * a; }
template <class T> inline bool chmin(T& a, const T& b) { if (b < a) { a = b; return true; } return false; }
template <class T> inline bool chmax(T& a, const T& b) { if (a < b) { a = b; return true; } return false; }
void _u() { cerr << endl; } template <class H, class... T> void _u(H&& h, T&&... t) { cerr << h << ", "; _u(move(t)...); }
#define _overload3(a, b, c, d, ...) d
#define _rep3(i, a, b) for (i32 i = i32(a); i < i32(b); ++i)
#define _rep2(i, b) _rep3(i, 0, b)
#define _rep1(i) for (i32 i = 0; ; ++i)
#define rep(...) _overload3(__VA_ARGS__, _rep3, _rep2, _rep1)(__VA_ARGS__)
#define A(a) begin(a), end(a)
#define S(a) i32((a).size())
#define U(...) { cerr << #__VA_ARGS__ << ": "; _u(__VA_ARGS__); }
#define lin U(__LINE__)
#define exi exit(0);

class timer {
    vector<timer> timers;
    int n = 0;
public:
    double limit = 5;
    double t = 0;
    timer() {}
    timer(int size) : timers(size) {}
    bool elapses() const {
        return time() - t > limit;
    }
    void measure() {
        t = time() - t;
        ++n;
    }
    void measure(char id) {
        timers[id].measure();
    }
    void print() {
        if (n % 2)
            measure();
        for (int i = 0; i < 128; ++i) {
            if (timers[i].n)
                cerr << (char)i << ' ' << timers[i].t << 's' << endl;
        }
        cerr << "  " << t << 's' << endl;
    }
    static double time() {
        using namespace chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() / 1000.0;
    }
} timer(128);

constexpr bool Deterministic = 0;
class rando {
    unsigned y;
public:
    rando(unsigned y) : y(y) {}
    unsigned next() {
        return y ^= (y ^= (y ^= y << 13) >> 17) << 5;
    }
    int next(int b) {
        return next() % b;
    }
    int next(int a, int b) {
        return next(b - a) + a;
    }
    double nextDouble(double b = 1) {
        return b * next() / 4294967296.0;
    }
    double nextDouble(double a, double b) {
        return nextDouble(b - a) + a;
    }
    int operator() (int b) {
        return next(b);
    }
} rando(Deterministic ? 2463534242 : chrono::system_clock::now().time_since_epoch().count());

i32 N;
i32 score, bscore = 1 << 30;
i32 sx0[204];
i32 sy0[204];
i32 sx1[204];
i32 sy1[204];
struct Ad {
    i32 x, y, r, x0, y0, x1, y1, score, ix0, iy0, ix1, iy1;
    f64 rinv;
    void set(i32 x0, i32 y0, i32 x1, i32 y1) {
        this->x0 = x0;
        this->y0 = y0;
        this->x1 = x1;
        this->y1 = y1;
    }
} as[204], be[200];

namespace RR {

constexpr f64 SC = 5000000;
i32 w2h[200][10001];
i32 dst1[200][14][10001];
i32 dst[200][14][10001];

inline void init() {
    rep (ai, N) {
        i32 r = as[ai].r;
        i32 pw = 10001;
        rep (h, 1, 10001) {
            i32 w = min(10000, r / h);
            i32 w1 = min(10000, (r + h - 1) / h);
            i32 s = sq(1 - w * h / f64(r)) * SC;
            i32 s1 = sq(1 - min(r, w1 * h) / f64(max(r, w1 * h))) * SC;
            if (chmin(s, s1)) {
                w = w1;
            }
            dst[ai][0][h] = s;
            dst1[ai][0][h] = h << 16 | w;
            fill(w2h[ai] + w, w2h[ai] + pw, h - 1);
            pw = w;
        }
        fill(w2h[ai] + 1, w2h[ai] + pw, 10000);
        rep (x, 1, 14) {
            i32 h = 1 << x;
            for (i32 y = 0; y < 10001; y += h << 1) {
                i32 y1 = min(10001, y + h);
                dst[ai][x][y1 - 1] = dst[ai][0][y1 - 1];
                dst1[ai][x][y1 - 1] = dst1[ai][0][y1 - 1];
                for (i32 y2 = y1 - 2; y2 >= y; y2--) {
                    dst[ai][x][y2] = min(dst[ai][x][y2 + 1], dst[ai][0][y2]);
                    dst1[ai][x][y2] = dst[ai][x][y2 + 1] < dst[ai][0][y2] ? dst1[ai][x][y2 + 1] : dst1[ai][0][y2];
                }
                if (y1 > 10000) break;
                dst[ai][x][y1] = dst[ai][0][y1];
                dst1[ai][x][y1] = dst1[ai][0][y1];
                for (i32 y2 = y1 + 1; y2 < min(10001, y1 + h); y2++) {
                    dst[ai][x][y2] = min(dst[ai][x][y2 - 1], dst[ai][0][y2]);
                    dst1[ai][x][y2] = dst[ai][x][y2 - 1] < dst[ai][0][y2] ? dst1[ai][x][y2 - 1] : dst1[ai][0][y2];
                }
            }
        }
    }
}

inline i32 size(i32 ai, i32 w, i32 h) {
    if (w2h[ai][w] >= h) return h << 16 | w;
    if (w2h[ai][w] == h - 1) return dst1[ai][0][h];
    i32 x = 31 - __builtin_clz((w2h[ai][w] + 1) ^ h);
    return dst[ai][x][w2h[ai][w] + 1] < dst[ai][x][h] ? dst1[ai][x][w2h[ai][w] + 1] : dst1[ai][x][h];
}

inline i32 size(i32 ai) {
    auto& a = as[ai];
    return size(ai, a.x1 - a.x0, a.y1 - a.y0);
}

inline i32 get(i32 ai, i32 w, i32 h) {
    if (w2h[ai][w] >= h) return sq(1 - w * h * as[ai].rinv) * SC;
    if (w2h[ai][w] == h - 1) return dst[ai][0][h];
    i32 x = 31 - __builtin_clz((w2h[ai][w] + 1) ^ h);
    return min(dst[ai][x][w2h[ai][w] + 1], dst[ai][x][h]);
}

inline i32 get(i32 ai) {
    auto& a = as[ai];
    return get(ai, a.x1 - a.x0, a.y1 - a.y0);
}
}

inline void input() {
    cin >> N;
    rep (ai, N) {
        auto& a = as[ai];
        cin >> a.x >> a.y >> a.r;
        a.rinv = 1.0 / a.r;
    }
}

inline i32 eval(i32 ar, i32 pr) {
    return ar >= pr ? 0 : sq(1 - ar / f64(pr)) * RR::SC;
}

inline void greedy() {
    struct Rec {
        i32 x0, y0, x1, y1;
        vector<i32> a;
    };
    struct GreadyRe {
        i32 s, i, z;
        bool h;
        GreadyRe() : s(1 << 30) {}
        GreadyRe(i32 s, bool h, i32 i, i32 z)
            : s(s), i(i), z(z), h(h) {}
        bool operator < (const GreadyRe& o) const {
            return s < o.s;
        }
    } gre;
    static vector<Rec> res(N);
    res.resize(1);
    res[0].x0 = 0;
    res[0].y0 = 0;
    res[0].x1 = 10000;
    res[0].y1 = 10000;
    res[0].a.resize(N);
    rep (ai, N) {
        res[0].a[ai] = ai;
    }
    while (S(res) < N) {
        i32 ri = 0;
        for (; S(res[ri].a) < 2; ri++) ;
        auto& re = res[ri];
        gre = {};
        if (S(res) == 1) {
            i32 x = as[*min_element(A(re.a), [](i32 i, i32 j) { return as[i].x < as[j].x; })].x + 1;
            i32 w = as[*max_element(A(re.a), [](i32 i, i32 j) { return as[i].x < as[j].x; })].x + 1 - x;
            i32 y = as[*min_element(A(re.a), [](i32 i, i32 j) { return as[i].y < as[j].y; })].y + 1;
            i32 h = as[*max_element(A(re.a), [](i32 i, i32 j) { return as[i].y < as[j].y; })].y + 1 - y;
            i32 r = rando.next(w + h);
            gre.h = r < w;
            if (gre.h) {
                gre.z = x + r;
                sort(A(re.a), [](i32 i, i32 j) { return as[i].x < as[j].x; });
                rep (i, N) {
                    if (as[re.a[i]].x >= gre.z) {
                        gre.i = i;
                        break;
                    }
                }
            } else {
                gre.z = y + r - w;
                sort(A(re.a), [](i32 i, i32 j) { return as[i].y < as[j].y; });
                rep (i, N) {
                    if (as[re.a[i]].y >= gre.z) {
                        gre.i = i;
                        break;
                    }
                }
            }
        } else {
            i64 r0 = 0;
            i64 r = 0;
            for (i32 ai : re.a) {
                r += as[ai].r;
            }
            sort(A(re.a), [](i32 i, i32 j) { return as[i].x < as[j].x; });
            rep (i, 1, S(re.a)) {
                auto& a = as[re.a[i - 1]];
                auto& b = as[re.a[i]];
                r0 += a.r;
                if (a.x == b.x) continue;
                i64 px = (re.x1 - re.x0) * r0;
                if (px < (a.x + 1) * r) {
                    i32 x = a.x + 1;
                    i32 score = eval((x - re.x0) * (re.y1 - re.y0), r0) * i
                        + eval((re.x1 - x) * (re.y1 - re.y0), r - r0) * (S(re.a) - i);
                    chmin(gre, {score, true, i, x});
                } else if (b.x * r <= px) {
                    i32 x = b.x;
                    i32 score = eval((x - re.x0) * (re.y1 - re.y0), r0) * i
                        + eval((re.x1 - x) * (re.y1 - re.y0), r - r0) * (S(re.a) - i);
                    chmin(gre, {score, true, i, x});
                } else {
                    i32 x = px / r;
                    i32 score = eval((x - re.x0) * (re.y1 - re.y0), r0) * i
                        + eval((re.x1 - x) * (re.y1 - re.y0), r - r0) * (S(re.a) - i);
                    chmin(gre, {score, true, i, x});
                    if (x * r != px) {
                        x++;
                        i32 score = eval((x - re.x0) * (re.y1 - re.y0), r0) * i
                            + eval((re.x1 - x) * (re.y1 - re.y0), r - r0) * (S(re.a) - i);
                        chmin(gre, {score, true, i, x});
                    }
                }
            }
            r0 = 0;
            sort(A(re.a), [](i32 i, i32 j) { return as[i].y < as[j].y; });
            rep (i, 1, S(re.a)) {
                auto& a = as[re.a[i - 1]];
                auto& b = as[re.a[i]];
                r0 += a.r;
                if (a.y == b.y) continue;
                i64 py = (re.y1 - re.y0) * r0;
                if (py < (a.y + 1) * r) {
                    i32 y = a.y + 1;
                    i32 score = eval((re.x1 - re.x0) * (y - re.y0), r0) * i
                        + eval((re.x1 - re.x0) * (re.y1 - y), r - r0) * (S(re.a) - i);
                    chmin(gre, {score, false, i, y});
                } else if (b.y * r <= py) {
                    i32 y = b.y;
                    i32 score = eval((re.x1 - re.x0) * (y - re.y0), r0) * i
                        + eval((re.x1 - re.x0) * (re.y1 - y), r - r0) * (S(re.a) - i);
                    chmin(gre, {score, false, i, y});
                } else {
                    i32 y = py / r;
                    i32 score = eval((re.x1 - re.x0) * (y - re.y0), r0) * i
                        + eval((re.x1 - re.x0) * (re.y1 - y), r - r0) * (S(re.a) - i);
                    chmin(gre, {score, false, i, y});
                    if (y * r != py) {
                        y++;
                        i32 score = eval((re.x1 - re.x0) * (y - re.y0), r0) * i
                            + eval((re.x1 - re.x0) * (re.y1 - y), r - r0) * (S(re.a) - i);
                        chmin(gre, {score, false, i, y});
                    }
                }
            }
        }
        res.emplace_back();
        auto& r0 = res[ri];
        auto& r1 = res.back();
        if (gre.h) {
            sort(A(r0.a), [](i32 i, i32 j) { return as[i].x < as[j].x; });
            r1.x0 = gre.z;
            r1.y0 = r0.y0;
            r1.x1 = r0.x1;
            r1.y1 = r0.y1;
            r0.x1 = gre.z;
        } else {
            sort(A(r0.a), [](i32 i, i32 j) { return as[i].y < as[j].y; });
            r1.x0 = r0.x0;
            r1.y0 = gre.z;
            r1.x1 = r0.x1;
            r1.y1 = r0.y1;
            r0.y1 = gre.z;
        }
        r1.a.assign(r0.a.begin() + gre.i, r0.a.end());
        r0.a.resize(gre.i);
    }
    score = 0;
    for (auto& re : res) {
        i32 ai = re.a[0];
        auto& a = as[ai];
        a.x0 = re.x0;
        a.y0 = re.y0;
        a.x1 = re.x1;
        a.y1 = re.y1;
        a.score = RR::get(re.a[0]);
        score += a.score;
    }
    sort(sx0, sx0 + N + 4, [](i32 i, i32 j) { return as[i].x0 < as[j].x0; });
    sort(sy0, sy0 + N + 4, [](i32 i, i32 j) { return as[i].y0 < as[j].y0; });
    sort(sx1, sx1 + N + 4, [](i32 i, i32 j) { return as[i].x1 < as[j].x1; });
    sort(sy1, sy1 + N + 4, [](i32 i, i32 j) { return as[i].y1 < as[j].y1; });
    rep (i, N + 4) {
        as[sx0[i]].ix0 = i;
        as[sy0[i]].iy0 = i;
        as[sx1[i]].ix1 = i;
        as[sy1[i]].iy1 = i;
    }
}

inline void updateBest() {
    if (chmin(bscore, score)) {
        copy_n(as, N, be);
    }
}

inline void restoreBest() {
    score = bscore;
    copy_n(be, N, as);
    rep (ai, N) {
        auto& a = as[ai];
        sx0[a.ix0] = ai;
        sy0[a.iy0] = ai;
        sx1[a.ix1] = ai;
        sy1[a.iy1] = ai;
    }
}

inline void output() {
    restoreBest();
    rep (ai, N) {
        auto& a = as[ai];
        i32 size = RR::size(ai);
        i32 w = size & 0xffff;
        i32 h = size >> 16;
        i32 x = max(a.x0, a.x - w + 1);
        i32 y = max(a.y0, a.y - h + 1);
        cout << x << ' ' << y << ' ' << x + w << ' ' << y + h << endl;
    }
}

inline void decx0(i32 ai, i32 x) {
    auto& a = as[ai];
    i32 i = a.ix0;
    for (; as[sx0[i - 1]].x0 > x; i--) {
        sx0[i] = sx0[i - 1];
        as[sx0[i - 1]].ix0++;
    }
    sx0[i] = ai;
    a.ix0 = i;
    a.x0 = x;
}

inline void incx0(i32 ai, i32 x) {
    auto& a = as[ai];
    i32 i = a.ix0;
    for (; as[sx0[i + 1]].x0 < x; i++) {
        sx0[i] = sx0[i + 1];
        as[sx0[i + 1]].ix0--;
    }
    sx0[i] = ai;
    a.ix0 = i;
    a.x0 = x;
}

inline void setx0(i32 ai, i32 x) {
    if (x < as[ai].x0) decx0(ai, x);
    else incx0(ai, x);
}

inline void decy0(i32 ai, i32 y) {
    auto& a = as[ai];
    i32 i = a.iy0;
    for (; as[sy0[i - 1]].y0 > y; i--) {
        sy0[i] = sy0[i - 1];
        as[sy0[i - 1]].iy0++;
    }
    sy0[i] = ai;
    a.iy0 = i;
    a.y0 = y;
}

inline void incy0(i32 ai, i32 y) {
    auto& a = as[ai];
    i32 i = a.iy0;
    for (; as[sy0[i + 1]].y0 < y; i++) {
        sy0[i] = sy0[i + 1];
        as[sy0[i + 1]].iy0--;
    }
    sy0[i] = ai;
    a.iy0 = i;
    a.y0 = y;
}

inline void sety0(i32 ai, i32 y) {
    if (y < as[ai].y0) decy0(ai, y);
    else incy0(ai, y);
}

inline void decx1(i32 ai, i32 x) {
    auto& a = as[ai];
    i32 i = a.ix1;
    for (; as[sx1[i - 1]].x1 > x; i--) {
        sx1[i] = sx1[i - 1];
        as[sx1[i - 1]].ix1++;
    }
    sx1[i] = ai;
    a.ix1 = i;
    a.x1 = x;
}

inline void incx1(i32 ai, i32 x) {
    auto& a = as[ai];
    i32 i = a.ix1;
    for (; as[sx1[i + 1]].x1 < x; i++) {
        sx1[i] = sx1[i + 1];
        as[sx1[i + 1]].ix1--;
    }
    sx1[i] = ai;
    a.ix1 = i;
    a.x1 = x;
}

inline void setx1(i32 ai, i32 x) {
    if (x < as[ai].x1) decx1(ai, x);
    else incx1(ai, x);
}

inline void decy1(i32 ai, i32 y) {
    auto& a = as[ai];
    i32 i = a.iy1;
    for (; as[sy1[i - 1]].y1 > y; i--) {
        sy1[i] = sy1[i - 1];
        as[sy1[i - 1]].iy1++;
    }
    sy1[i] = ai;
    a.iy1 = i;
    a.y1 = y;
}

inline void incy1(i32 ai, i32 y) {
    auto& a = as[ai];
    i32 i = a.iy1;
    for (; as[sy1[i + 1]].y1 < y; i++) {
        sy1[i] = sy1[i + 1];
        as[sy1[i + 1]].iy1--;
    }
    sy1[i] = ai;
    a.iy1 = i;
    a.y1 = y;
}

inline void sety1(i32 ai, i32 y) {
    if (y < as[ai].y1) decy1(ai, y);
    else incy1(ai, y);
}

inline void set(i32 ai, i32 x0, i32 y0, i32 x1, i32 y1) {
    setx0(ai, x0);
    sety0(ai, y0);
    setx1(ai, x1);
    sety1(ai, y1);
}

inline void expandLeft(i32 ai) {
    auto& a = as[ai];
    for (i32 i = -1; a.x0 > 0; ) {
        a.x0--;
        i32 diff = RR::get(ai) - a.score;
        a.x0++;
        if (diff >= 0) return;
        if (i == -1) {
            for (i = a.ix1 - 1; as[sx1[i]].x1 > a.x0; i--) ;
        }
        for (i32 j = i; ; j--) {
            i32 aj = sx1[j];
            auto& b = as[aj];
            if (b.x1 != a.x0) break;
            if (b.y0 < a.y1 && a.y0 < b.y1) {
                if (b.x == a.x0 - 1) return;
                b.x1--;
                diff += RR::get(aj) - b.score;
                b.x1++;
                if (diff >= 0) return;
            }
        }
        score += diff;
        while (true) {
            i32 aj = sx1[i];
            auto& b = as[aj];
            if (b.x1 != a.x0) break;
            if (b.y0 < a.y1 && a.y0 < b.y1) {
                decx1(aj, b.x1 - 1);
                b.score = RR::get(aj);
            } else {
                i--;
            }
        }
        decx0(ai, a.x0 - 1);
        a.score = RR::get(ai);
    }
}

inline void expandRight(i32 ai) {
    auto& a = as[ai];
    for (i32 i = -1; a.x1 < 10000; ) {
        a.x1++;
        i32 diff = RR::get(ai) - a.score;
        a.x1--;
        if (diff >= 0) return;
        if (i == -1) {
            for (i = a.ix0 + 1; as[sx0[i]].x0 < a.x1; i++) ;
        }
        for (i32 j = i; ; j++) {
            i32 aj = sx0[j];
            auto& b = as[aj];
            if (b.x0 != a.x1) break;
            if (b.y0 < a.y1 && a.y0 < b.y1) {
                if (b.x == a.x1) return;
                b.x0++;
                diff += RR::get(aj) - b.score;
                b.x0--;
                if (diff >= 0) return;
            }
        }
        score += diff;
        while (true) {
            i32 aj = sx0[i];
            auto& b = as[aj];
            if (b.x0 != a.x1) break;
            if (b.y0 < a.y1 && a.y0 < b.y1) {
                incx0(aj, b.x0 + 1);
                b.score = RR::get(aj);
            } else {
                i++;
            }
        }
        incx1(ai, a.x1 + 1);
        a.score = RR::get(ai);
    }
}

inline void expandUp(i32 ai) {
    auto& a = as[ai];
    for (i32 i = -1; a.y0 > 0; ) {
        a.y0--;
        i32 diff = RR::get(ai) - a.score;
        a.y0++;
        if (diff >= 0) return;
        if (i == -1) {
            for (i = a.iy1 - 1; as[sy1[i]].y1 > a.y0; i--) ;
        }
        for (i32 j = i; ; j--) {
            i32 aj = sy1[j];
            auto& b = as[aj];
            if (b.y1 != a.y0) break;
            if (b.x0 < a.x1 && a.x0 < b.x1) {
                if (b.y == a.y0 - 1) return;
                b.y1--;
                diff += RR::get(aj) - b.score;
                b.y1++;
                if (diff >= 0) return;
            }
        }
        score += diff;
        while (true) {
            i32 aj = sy1[i];
            auto& b = as[aj];
            if (b.y1 != a.y0) break;
            if (b.x0 < a.x1 && a.x0 < b.x1) {
                decy1(aj, b.y1 - 1);
                b.score = RR::get(aj);
            } else {
                i--;
            }
        }
        decy0(ai, a.y0 - 1);
        a.score = RR::get(ai);
    }
}

inline void expandDown(i32 ai) {
    auto& a = as[ai];
    for (i32 i = -1; a.y1 < 10000; ) {
        a.y1++;
        i32 diff = RR::get(ai) - a.score;
        a.y1--;
        if (diff >= 0) return;
        if (i == -1) {
            for (i = a.iy0 + 1; as[sy0[i]].y0 < a.y1; i++) ;
        }
        for (i32 j = i; ; j++) {
            i32 aj = sy0[j];
            auto& b = as[aj];
            if (b.y0 != a.y1) break;
            if (b.x0 < a.x1 && a.x0 < b.x1) {
                if (b.y == a.y1) return;
                b.y0++;
                diff += RR::get(aj) - b.score;
                b.y0--;
                if (diff >= 0) return;
            }
        }
        score += diff;
        while (true) {
            i32 aj = sy0[i];
            auto& b = as[aj];
            if (b.y0 != a.y1) break;
            if (b.x0 < a.x1 && a.x0 < b.x1) {
                incy0(aj, b.y0 + 1);
                b.score = RR::get(aj);
            } else {
                i++;
            }
        }
        incy1(ai, a.y1 + 1);
        a.score = RR::get(ai);
    }
}

inline void expand(i32 ai, i32 d) {
    if (d == 0) expandLeft(ai);
    else if (d == 1) expandRight(ai);
    else if (d == 2) expandUp(ai);
    else expandDown(ai);
}

inline i32 dist(i32 ai, i32 aj) {
    auto& a = as[ai];
    auto& b = as[aj];
    return max(0, max(a.x0 - b.x1, b.x0 - a.x1))
        + max(0, max(a.y0 - b.y1, b.y0 - a.y1));
}

namespace RE {

struct OptRe {
    i32 s, x0, y0, x1, y1;
    OptRe() : s(1 << 30) {}
    OptRe(i32 s, i32 x0, i32 y0, i32 x1, i32 y1)
        : s(s), x0(x0), y0(y0), x1(x1), y1(y1) {}
    bool operator < (const OptRe& o) const {
        return s < o.s;
    }
} ore;
array<i32, 5> so[2], bso[2];

inline void makeCs(i32 ai) {
    auto& a = as[ai];
    static vector<pair<i32, i32>> q;
    q.resize(1);
    for (i32 i = a.ix1 - 1; ; i--) {
        auto& b = as[sx1[i]];
        if (u32(a.y - b.y0) < u32(b.y1 - b.y0) && b.x1 <= a.x) {
            q[0].first = b.x1;
            break;
        }
    }
    for (i32 i = a.ix0 + 1; ; i++) {
        auto& b = as[sx0[i]];
        if (u32(a.y - b.y0) < u32(b.y1 - b.y0) && a.x < b.x0) {
            q[0].second = b.x0;
            break;
        }
    }
    i32 i0 = a.iy1 - 1;
    for (; a.y < as[sy1[i0]].y1; i0--) ;
    i32 i1 = a.iy0 + 1;
    for (; as[sy0[i1]].y0 <= a.y; i1++) ;
    ore = {};
    rep (qi, S(q)) {
        i32 x0 = q[qi].first;
        i32 x1 = q[qi].second;
        i32 i;
        i32 ux0 = x0;
        i32 ux1 = x1;
        for (i = i0; ; i--) {
            auto& b = as[sy1[i]];
            if (x0 < b.x1 && b.x0 < x1) {
                if (b.x1 <= a.x) {
                    ux0 = max(ux0, b.x1);
                } else if (a.x < b.x0) {
                    ux1 = min(ux1, b.x0);
                } else {
                    ux1 = 0;
                }
                break;
            }
        }
        i32 y0 = as[sy1[i]].y1;
        for (i--; as[sy1[i]].y1 == y0; i--) {
            auto& b = as[sy1[i]];
            if (x0 < b.x1 && b.x0 < x1) {
                if (b.x1 <= a.x) {
                    ux0 = max(ux0, b.x1);
                } else if (a.x < b.x0) {
                    ux1 = min(ux1, b.x0);
                } else {
                    ux1 = 0;
                    break;
                }
            }
        }
        i32 dx0 = x0;
        i32 dx1 = x1;
        for (i = i1; ; i++) {
            auto& b = as[sy0[i]];
            if (x0 < b.x1 && b.x0 < x1) {
                if (b.x1 <= a.x) {
                    dx0 = max(dx0, b.x1);
                } else if (a.x < b.x0) {
                    dx1 = min(dx1, b.x0);
                } else {
                    dx1 = 0;
                }
                break;
            }
        }
        i32 y1 = as[sy0[i]].y0;
        for (i++; as[sy0[i]].y0 == y1; i++) {
            auto& b = as[sy0[i]];
            if (x0 < b.x1 && b.x0 < x1) {
                if (b.x1 <= a.x) {
                    dx0 = max(dx0, b.x1);
                } else if (a.x < b.x0) {
                    dx1 = min(dx1, b.x0);
                } else {
                    dx1 = 0;
                    break;
                }
            }
        }
        i32 score = RR::get(ai, x1 - x0, y1 - y0);
        if (ore.s > score) {
            i32 size = RR::size(ai, x1 - x0, y1 - y0);
            i32 w = size & 0xffff;
            i32 h = size >> 16;
            i32 x = max(x0, a.x - w + 1);
            i32 y = max(y0, a.y - h + 1);
            i32 x_ = min(x1 - w, a.x);
            i32 y_ = min(y1 - h, a.y);
            if (x != x_ && (rando.next() & 1) != 0) {
                x = x_;
            }
            if (y != y_ && (rando.next() & 1) != 0) {
                y = y_;
            }
            ore = {score, x, y, x + w, y + h};
            if (score == 0) break;
        }
        if (ux1 != 0 && find(A(q), make_pair(ux0, ux1)) == q.end()) {
            q.emplace_back(ux0, ux1);
        }
        if (dx1 != 0 && find(A(q), make_pair(dx0, dx1)) == q.end()) {
            q.emplace_back(dx0, dx1);
        }
    }
}

inline void se1(i32 ai) {
    auto& a = as[ai];
    makeCs(ai);
    set(ai, ore.x0, ore.y0, ore.x1, ore.y1);
    score += ore.s - a.score;
    a.score = ore.s;
}

inline void se2(i32 ai, i32 aj) {
    i32 ais[]{ai, aj};
    i32 bscore = score;
    rep (i, 2) {
        i32 ai = ais[i];
        auto& a = as[ai];
        bso[i] = {ai, a.x0, a.y0, a.x1, a.y1};
    }
    rep (i, 2) {
        i32 ai = ais[0];
        i32 aj = ais[1];
        auto& b = as[aj];
        incx0(aj, b.x);
        incy0(aj, b.y);
        decx1(aj, b.x + 1);
        decy1(aj, b.y + 1);
        makeCs(ai);
        i32 nscore = score + ore.s - as[ai].score;
        set(ai, ore.x0, ore.y0, ore.x1, ore.y1);
        so[0] = {ai, ore.x0, ore.y0, ore.x1, ore.y1};
        makeCs(aj);
        nscore += ore.s - b.score;
        if (bscore > nscore) {
            bscore = nscore;
            bso[0] = so[0];
            bso[1] = {aj, ore.x0, ore.y0, ore.x1, ore.y1};
        }
        swap(ais[0], ais[1]);
    }
    score = bscore;
    for (auto& r : bso) {
        i32 ai = r[0];
        set(ai, r[1], r[2], r[3], r[4]);
        as[ai].score = RR::get(ai);
    }
}
}

namespace NE {

vector<i32> ais, ais1, ds;
vector<pair<i32, i32>> ds2;

inline void init() {
    rep (ai, N) {
        ais.emplace_back(ai);
        ais1.emplace_back(ai);
    }
    rep (d, 4) {
        ds.emplace_back(d);
        ds2.emplace_back(0, d);
        ds2.emplace_back(1, d);
    }
}

inline void n1(vector<i32>& ais) {
    random_shuffle(A(ais), rando);
    for (i32 ai : ais) {
        if (as[ai].score == 0) continue;
        RE::se1(ai);
        random_shuffle(A(ds), rando);
        for (i32 d : ds) {
            if (as[ai].score == 0) break;
            expand(ai, d);
        }
    }
}

inline void n1() {
    n1(ais);
}

inline void n2() {
    random_shuffle(A(ais1), rando);
    for (i32 ai : ais1) {
        i32 n = N;
        for (i32 i = 0; i < n; ) {
            if (ais[i] == ai || (as[ai].score | as[ais[i]].score) == 0) {
                n--;
                swap(ais[i], ais[n]);
            } else {
                i++;
            }
        }
        constexpr i32 K = 5;
        nth_element(ais.begin(), ais.begin() + min(K, n), ais.begin() + n, [ai](i32 i, i32 j) {
            return dist(ai, i) < dist(ai, j);
        });
        random_shuffle(ais.begin(), ais.begin() + min(K, n), rando);
        rep (i, min(K, n)) {
            i32 aj = ais[i];
            if ((as[ai].score | as[aj].score) == 0) continue;
            RE::se2(ai, aj);
            random_shuffle(A(ds2), rando);
            for (auto& p : ds2) {
                i32 ak = p.first ? aj : ai;
                if (as[ak].score == 0) continue;
                expand(ak, p.second);
            }
        }
        if (timer.elapses()) return;
    }
}

inline void kick() {
    i32 ai = 0;
    do {
        ai = rando.next(N);
    } while (as[ai].score == 0);
    constexpr i32 K = 9;
    nth_element(ais.begin(), ais.begin() + K, ais.end(), [ai](i32 i, i32 j) {
        if (i == ai) return true;
        if (j == ai) return false;
        return dist(ai, i) < dist(ai, j);
    });
    random_shuffle(ais.begin(), ais.begin() + K, rando);
    rep (i, K) {
        i32 ai = ais[i];
        auto& a = as[ai];
        incx0(ai, a.x);
        incy0(ai, a.y);
        decx1(ai, a.x + 1);
        decy1(ai, a.y + 1);
        i32 s = RR::get(ai);
        score += s - a.score;
        a.score = s;
    }
    static vector<i32> a;
    a.assign(ais.begin(), ais.begin() + K);
    n1(a);
}
}

inline void init() {
    rep (i, N + 4) {
        sx0[i] = sy0[i] = sx1[i] = sy1[i] = i;
    }
    as[N].set(-1, -1, 0, 10001);
    as[N + 1].set(10000, -1, 10001, 10001);
    as[N + 2].set(-1, -1, 10001, 0);
    as[N + 3].set(-1, 10000, 10001, 10001);
    RR::init();
    NE::init();
}

i32 main() {
    timer.measure();
    input();
    init();
    timer.limit = 1.2;
    while (!timer.elapses()) {
        greedy();
        NE::n1();
        rep (_, 3) {
            NE::n2();
        }
        updateBest();
    }
    restoreBest();
    timer.limit = 4.9;
    while (score > 0 && !timer.elapses()) {
        NE::kick();
        NE::n2();
        if (bscore > score) {
            updateBest();
        } else {
            restoreBest();
        }
    }
    output();
    return 0;
}
