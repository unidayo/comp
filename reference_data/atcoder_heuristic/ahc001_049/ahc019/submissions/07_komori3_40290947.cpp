#define _CRT_NONSTDC_NO_WARNINGS
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include <bits/stdc++.h>
#include <random>
#include <unordered_set>
#include <array>
#include <optional>
#ifdef _MSC_VER
#include <opencv2/core.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <conio.h>
#include <ppl.h>
#include <filesystem>
#include <intrin.h>
/* g++ functions */
int __builtin_clz(unsigned int n) { unsigned long index; _BitScanReverse(&index, n); return 31 - index; }
int __builtin_ctz(unsigned int n) { unsigned long index; _BitScanForward(&index, n); return index; }
namespace std { inline int __lg(int __n) { return sizeof(int) * 8 - 1 - __builtin_clz(__n); } }
/* enable __uint128_t in MSVC */
//#include <boost/multiprecision/cpp_int.hpp>
//using __uint128_t = boost::multiprecision::uint128_t;
#else
#pragma GCC target("avx2")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")
#endif

/** compro io **/
namespace aux {
    template<typename T, unsigned N, unsigned L> struct tp { static void output(std::ostream& os, const T& v) { os << std::get<N>(v) << ", "; tp<T, N + 1, L>::output(os, v); } };
    template<typename T, unsigned N> struct tp<T, N, N> { static void output(std::ostream& os, const T& v) { os << std::get<N>(v); } };
}
template<typename... Ts> std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& t) { os << '{'; aux::tp<std::tuple<Ts...>, 0, sizeof...(Ts) - 1>::output(os, t); return os << '}'; } // tuple out
template<class Ch, class Tr, class Container> std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, const Container& x); // container out (fwd decl)
template<class S, class T> std::ostream& operator<<(std::ostream& os, const std::pair<S, T>& p) { return os << '{' << p.first << ", " << p.second << '}'; } // pair out
template<class S, class T> std::istream& operator>>(std::istream& is, std::pair<S, T>& p) { return is >> p.first >> p.second; } // pair in
std::ostream& operator<<(std::ostream& os, const std::vector<bool>::reference& v) { os << (v ? '1' : '0'); return os; } // bool (vector) out
std::ostream& operator<<(std::ostream& os, const std::vector<bool>& v) { bool f = true; os << '{'; for (const auto& x : v) { os << (f ? "" : ", ") << x; f = false; } os << '}'; return os; } // vector<bool> out
template<class Ch, class Tr, class Container> std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, const Container& x) { bool f = true; os << '{'; for (auto& y : x) { os << (f ? "" : ", ") << y; f = false; } return os << '}'; } // container out
template<class T, class = decltype(std::begin(std::declval<T&>())), class = typename std::enable_if<!std::is_same<T, std::string>::value>::type> std::istream& operator>>(std::istream& is, T& a) { for (auto& x : a) is >> x; return is; } // container in
template<typename T> auto operator<<(std::ostream& out, const T& t) -> decltype(out << t.stringify()) { out << t.stringify(); return out; } // struct (has stringify() func) out
/** io setup **/
struct IOSetup { IOSetup(bool f) { if (f) { std::cin.tie(nullptr); std::ios::sync_with_stdio(false); } std::cout << std::fixed << std::setprecision(15); } }
iosetup(true); // set false when solving interective problems
/** string formatter **/
template<typename... Ts> std::string format(const std::string& f, Ts... t) { size_t l = std::snprintf(nullptr, 0, f.c_str(), t...); std::vector<char> b(l + 1); std::snprintf(&b[0], l + 1, f.c_str(), t...); return std::string(&b[0], &b[0] + l); }
/** dump **/
#define DUMPOUT std::cerr
std::ostringstream DUMPBUF;
#define dump(...) do{DUMPBUF<<"  ";DUMPBUF<<#__VA_ARGS__<<" :[DUMP - "<<__LINE__<<":"<<__FUNCTION__<<']'<<std::endl;DUMPBUF<<"    ";dump_func(__VA_ARGS__);DUMPOUT<<DUMPBUF.str();DUMPBUF.str("");DUMPBUF.clear();}while(0);
void dump_func() { DUMPBUF << std::endl; }
template <class Head, class... Tail> void dump_func(Head&& head, Tail&&... tail) { DUMPBUF << head; if (sizeof...(Tail) == 0) { DUMPBUF << " "; } else { DUMPBUF << ", "; } dump_func(std::move(tail)...); }
/** timer **/
class Timer {
    double t = 0, paused = 0, tmp;
public:
    Timer() { reset(); }
    static double time() {
#ifdef _MSC_VER
        return __rdtsc() / 3.0e9;
#else
        unsigned long long a, d;
        __asm__ volatile("rdtsc"
            : "=a"(a), "=d"(d));
        return (d << 32 | a) / 3.0e9;
#endif
    }
    void reset() { t = time(); }
    void pause() { tmp = time(); }
    void restart() { paused += time() - tmp; }
    double elapsed_ms() const { return (time() - t - paused) * 1000.0; }
};
/** rand **/
struct Xorshift {
    static constexpr uint64_t M = INT_MAX;
    static constexpr double e = 1.0 / M;
    uint64_t x = 88172645463325252LL;
    Xorshift() {}
    Xorshift(uint64_t seed) { reseed(seed); }
    inline void reseed(uint64_t seed) { x = 0x498b3bc5 ^ seed; for (int i = 0; i < 20; i++) next(); }
    inline uint64_t next() { x = x ^ (x << 7); return x = x ^ (x >> 9); }
    inline int next_int() { return next() & M; }
    inline int next_int(int mod) { return next() % mod; }
    inline int next_int(int l, int r) { return l + next_int(r - l + 1); }
    inline double next_double() { return next_int() * e; }
};
/** shuffle **/
template<typename T> void shuffle_vector(std::vector<T>& v, Xorshift& rnd) { int n = v.size(); for (int i = n - 1; i >= 1; i--) { int r = rnd.next_int(i); std::swap(v[i], v[r]); } }
/** split **/
std::vector<std::string> split(const std::string& str, const std::string& delim) {
    std::vector<std::string> res;
    std::string buf;
    for (const auto& c : str) {
        if (delim.find(c) != std::string::npos) {
            if (!buf.empty()) res.push_back(buf);
            buf.clear();
        }
        else buf += c;
    }
    if (!buf.empty()) res.push_back(buf);
    return res;
}
/** misc **/
template<typename A, size_t N, typename T> inline void Fill(A(&array)[N], const T& val) { std::fill((T*)array, (T*)(array + N), val); } // fill array
template<typename T, typename ...Args> auto make_vector(T x, int arg, Args ...args) { if constexpr (sizeof...(args) == 0)return std::vector<T>(arg, x); else return std::vector(arg, make_vector<T>(x, args...)); }
template<typename T> bool chmax(T& a, const T& b) { if (a < b) { a = b; return true; } return false; }
template<typename T> bool chmin(T& a, const T& b) { if (a > b) { a = b; return true; } return false; }
/** using **/
using ll = long long;
using ld = long double;
//using ld = boost::multiprecision::cpp_bin_float_quad;
using pii = std::pair<int, int>;
using pll = std::pair<ll, ll>;
using std::cin, std::cout, std::cerr, std::endl, std::string, std::vector, std::array;

/* fast queue */
class FastQueue {
    int front = 0;
    int back = 0;
    int v[4096];
public:
    inline bool empty() { return front == back; }
    inline void push(int x) { v[front++] = x; }
    inline int pop() { return v[back++]; }
    inline void reset() { front = back = 0; }
    inline int size() { return front - back; }
};

class RandomQueue {
    int sz = 0;
    int v[4096];
public:
    inline bool empty() const { return !sz; }
    inline int size() const { return sz; }
    inline void push(int x) { v[sz++] = x; }
    inline void reset() { sz = 0; }
    inline int pop(int i) {
        std::swap(v[i], v[sz - 1]);
        return v[--sz];
    }
    inline int pop(Xorshift& rnd) {
        return pop(rnd.next_int(sz));
    }
};

struct UnionFind {
    vector<int> data;

    UnionFind() = default;

    explicit UnionFind(size_t sz) : data(sz, -1) {}

    void init(size_t sz) { data.resize(sz, -1); }

    bool unite(int x, int y) {
        x = find(x), y = find(y);
        if (x == y) return false;
        if (data[x] > data[y]) std::swap(x, y);
        data[x] += data[y];
        data[y] = x;
        return true;
    }

    int find(int k) {
        if (data[k] < 0) return (k);
        return data[k] = find(data[k]);
    }

    int size(int k) {
        return -data[find(k)];
    }

    bool same(int x, int y) {
        return find(x) == find(y);
    }

    vector<vector<int>> groups() {
        int n = (int)data.size();
        vector<vector<int>> ret(n);
        for (int i = 0; i < n; i++) {
            ret[find(i)].emplace_back(i);
        }
        ret.erase(std::remove_if(std::begin(ret), std::end(ret), [&](const vector<int>& v) {
            return v.empty();
            }), ret.end());
        return ret;
    }
};

constexpr int dx[6] = { 1, 0, 0, -1, 0, 0 };
constexpr int dy[6] = { 0, 1, 0, 0, -1, 0 };
constexpr int dz[6] = { 0, 0, 1, 0, 0, -1 };
constexpr int d6[6] = { 256, 16, 1, -256, -16, -1 };
constexpr int d26[26] = {
    -273, -272, -271, -257, -256, -255, -241, -240, -239,
    -17, -16, -15, -1, 1, 15, 16, 17,
    239, 240, 241, 255, 256, 257, 271, 272, 273
};
constexpr int dtbl[24][6] = {
{0, 1, 2, 3, 4, 5},
{0, 2, 4, 3, 5, 1},
{0, 4, 5, 3, 1, 2},
{0, 5, 1, 3, 2, 4},
{1, 0, 5, 4, 3, 2},
{1, 2, 0, 4, 5, 3},
{1, 3, 2, 4, 0, 5},
{1, 5, 3, 4, 2, 0},
{2, 0, 1, 5, 3, 4},
{2, 1, 3, 5, 4, 0},
{2, 3, 4, 5, 0, 1},
{2, 4, 0, 5, 1, 3},
{3, 1, 5, 0, 4, 2},
{3, 2, 1, 0, 5, 4},
{3, 4, 2, 0, 1, 5},
{3, 5, 4, 0, 2, 1},
{4, 0, 2, 1, 3, 5},
{4, 2, 3, 1, 5, 0},
{4, 3, 5, 1, 0, 2},
{4, 5, 0, 1, 2, 3},
{5, 0, 4, 2, 3, 1},
{5, 1, 0, 2, 4, 3},
{5, 3, 1, 2, 0, 4},
{5, 4, 3, 2, 1, 0}
}; // direction table

inline int enc(int x, int y, int z) {
    return (x << 8) | (y << 4) | z;
}
inline std::tuple<int, int, int> dec(int xyz) {
    return { xyz >> 8, xyz >> 4 & 0xF, xyz & 0xF };
}
inline int fproj(int xyz) {
    return ((xyz & 0xF) << 4) | (xyz >> 8);
}
inline int rproj(int xyz) {
    return ((xyz & 0xF) << 4) | (xyz >> 4 & 0xF);
}
inline int fproj(int x, int y, int z) {
    return fproj(enc(x, y, z));
}
inline int rproj(int x, int y, int z) {
    return rproj(enc(x, y, z));
}

inline double get_temp(double stemp, double etemp, double t, double T) {
    return stemp * pow(etemp / stemp, t / T);
};

struct Input {
    int D;
    vector<vector<string>> f, r;
    Input(std::istream& in) {
        in >> D;
        f.resize(2, vector<string>(D));
        r.resize(2, vector<string>(D));
        in >> f[0] >> r[0] >> f[1] >> r[1];
    }
};

struct Blob {

    int rot;                    // rotation pattern id
    array<vector<int>, 2> pts;  // close list (TODO: array)
    array<FastQueue, 2> open;   // open list
    array<std::bitset<4096>, 2> oflag;
    bool finished;

    void reset(int sp0_, int sp1_, int rot_) {
        for (int i = 0; i < 2; i++) {
            pts[i].clear();
            open[i].reset();
            oflag[i].reset();
        }
        rot = rot_;
        open[0].push(sp0_);
        open[1].push(sp1_);
        oflag[0][sp0_] = oflag[1][sp1_] = true;
        finished = false;
    }

};

struct Trans {
    enum struct Type { CHANGE, ADD, REMOVE, INVALID };
    Type type;
    int idx;
    array<int, 3> pdata, ndata;
    Trans() : type(Type::INVALID) {}
};

struct Solver {

    // static

    int D;
    array<std::bitset<256>, 2> fsil;
    array<std::bitset<256>, 2> rsil;
    array<std::bitset<4096>, 2> dom;
    array<vector<int>, 2> pdom;
    array<int, 2> ndom;

    // dynamic

    array<vector<int>, 2> sps; // seed points
    vector<int> rots;          // rotation pattern ids

    array<std::bitset<4096>, 2> sused;

    array<vector<int>, 2> tmp_sps;
    vector<int> tmp_rots;

    // 遷移
    // 1. sps[0][i] の変更
    // 2. sps[1][i] の変更
    // 3. rots[i] の変更
    // 4. 1~3 の複合
    // 5. blob の追加 (位置は考慮したほうがよさそう)
    // 6. blob の削除
    // 7. kick

    vector<Blob> blobs;
    array<std::bitset<4096>, 2> used;

    array<array<int, 256>, 2> fsum;
    array<array<int, 256>, 2> rsum;
    array<int, 2> frem;
    array<int, 2> rrem;

    vector<pii> pp_blocks;

    double min_cost = DBL_MAX;

    double cost;
    int num_blocks_cap = 10000;
    int num_blocks_min = 10000;

    Solver(const Input& input) : D(input.D) {

        // sil
        for (int i = 0; i < 2; i++) {
            for (int z = 1; z <= D; z++) {
                for (int x = 1; x <= D; x++) {
                    if (input.f[i][z - 1][x - 1] == '1') {
                        fsil[i][fproj(x, 0, z)] = true;
                    }
                }
            }
        }
        for (int i = 0; i < 2; i++) {
            for (int z = 1; z <= D; z++) {
                for (int y = 1; y <= D; y++) {
                    if (input.r[i][z - 1][y - 1] == '1') {
                        rsil[i][rproj(0, y, z)] = true;
                    }
                }
            }
        }

        // dom
        for (int i = 0; i < 2; i++) {
            for (int x = 1; x <= D; x++) {
                for (int y = 1; y <= D; y++) {
                    for (int z = 1; z <= D; z++) {
                        if (fsil[i][fproj(x, y, z)] && rsil[i][rproj(x, y, z)]) {
                            dom[i][enc(x, y, z)] = true;
                            pdom[i].push_back(enc(x, y, z));
                        }
                    }
                }
            }
            ndom[i] = (int)pdom[i].size();
        }

    }

    void set_initial_conditions(int num_seeds, Xorshift& rnd) {
        chmin(num_seeds, (int)std::min(dom[0].count(), dom[1].count()));
        array<vector<int>, 2> sps_;
        vector<int> rots_;
        for (int i = 0; i < 2; i++) {
            for (int p = 0; p < 4096; p++) if (dom[i][p]) {
                sps_[i].push_back(p);
            }
            shuffle_vector(sps_[i], rnd);
            sps_[i].erase(sps_[i].begin() + num_seeds, sps_[i].end());
        }
        for (int i = 0; i < num_seeds; i++) {
            rots_.push_back(rnd.next_int(24));
        }
        set_initial_conditions(sps_, rots_);
    }

    void set_initial_conditions(const array<vector<int>, 2>& sps_, const vector<int>& rots_) {
        sps = sps_;
        rots = rots_;
    }

    void reset_states() {
        blobs.resize(sps[0].size());
        for (int i = 0; i < 2; i++) {
            sused[i].reset();
            used[i].reset();
            frem[i] = fsil[i].count(); // TODO: memo
            rrem[i] = rsil[i].count();
        }
        fsum = {};
        rsum = {};
        cost = 0;
        for (int i = 0; i < (int)blobs.size(); i++) {
            int sp0 = sps[0][i], sp1 = sps[1][i], rot = rots[i];
            sused[0][sp0] = true;
            sused[1][sp1] = true;
            blobs[i].reset(sp0, sp1, rot);
        }
    }

    bool grow(int i) {
        auto& [rot, pts, open, oflag, finished] = blobs[i];
        if (finished) return false;
        while (!open[0].empty()) {
            auto p0 = open[0].pop();
            auto p1 = open[1].pop();
            oflag[0][p0] = oflag[1][p1] = false;
            if (used[0][p0] | used[1][p1]) continue;
            pts[0].push_back(p0);
            pts[1].push_back(p1);
            used[0][p0] = used[1][p1] = true;

            frem[0] -= (++fsum[0][fproj(p0)]) == 1;
            frem[1] -= (++fsum[1][fproj(p1)]) == 1;
            rrem[0] -= (++rsum[0][rproj(p0)]) == 1;
            rrem[1] -= (++rsum[1][rproj(p1)]) == 1;

            for (int d = 0; d < 6; d++) {
                auto np0 = p0 + d6[d];
                if (~dom[0][np0] | used[0][np0] | oflag[0][np0]) continue;
                auto np1 = p1 + d6[dtbl[rot][d]];
                if (~dom[1][np1] | used[1][np1] | oflag[1][np1]) continue;
                open[0].push(np0);
                open[1].push(np1);
                oflag[0][np0] = oflag[1][np1] = true;
            }
            return true;
        }
        finished = true;
        cost += 1.0 / pts[0].size();
        return false;
    }

    int calc_postprocess_cost() const {
        return std::max(frem[0] + rrem[0], frem[1] + rrem[1]);
    }

    int choose_critical_point(int i, Xorshift& rnd) const {
        vector<int> cands;
        for (int p : pdom[i]) {
            if (!fsum[i][fproj(p)] && !rsum[i][rproj(p)]) cands.push_back(p);
        }
        if (!cands.empty()) {
            int id = rnd.next_int(cands.size());
            return cands[id];
        }
        for (int p : pdom[i]) {
            if (!fsum[i][fproj(p)] || !rsum[i][rproj(p)]) cands.push_back(p);
        }
        if (!cands.empty()) {
            int id = rnd.next_int(cands.size());
            return cands[id];
        }
        int nsp = -1;
        while (true) {
            int id = rnd.next_int(ndom[i]);
            nsp = pdom[i][id];
            if (sused[i][nsp]) continue;
            break;
        }
        return nsp;
    }

    int choose_near_point(int i, int bid, Xorshift& rnd) const {
        int p = sps[i][bid];
        if (D <= 9) {
            vector<int> dirs({ 0,1,2,3,4,5 });
            shuffle_vector(dirs, rnd);
            for (int d : dirs) {
                int np = p + d6[d];
                if (!dom[i][np] || sused[i][np]) continue;
                return np;
            }
        }
        else {
            vector<int> dirs(26);
            std::iota(dirs.begin(), dirs.end(), 0);
            shuffle_vector(dirs, rnd);
            for (int d : dirs) {
                int np = p + d26[d];
                if (!dom[i][np] || sused[i][np]) continue;
                return np;
            }
        }
        return p;
    }

    Trans create_change_trans(Xorshift& rnd) const {
        Trans t;
        if ((int)sps[0].size() == std::min(ndom[0], ndom[1])) return t; // invalid
        int bid = rnd.next_int((int)sps[0].size());
        t.type = Trans::Type::CHANGE;
        t.idx = bid;
        t.pdata = { sps[0][bid], sps[1][bid], rots[bid] };
        int nsp0 = rnd.next_int(2) ? choose_critical_point(0, rnd) : choose_near_point(0, bid, rnd);
        int nsp1 = rnd.next_int(2) ? choose_critical_point(1, rnd) : choose_near_point(1, bid, rnd);
        int nrot = rnd.next_int(24);
        t.ndata = { nsp0, nsp1, nrot };
        return t;
    }

    Trans create_slide_trans(Xorshift& rnd) const {
        Trans t;
        int bid = rnd.next_int((int)sps[0].size());
        t.type = Trans::Type::CHANGE;
        t.idx = bid;
        t.pdata = { sps[0][bid], sps[1][bid], rots[bid] };
        t.ndata = t.pdata;
        int i = rnd.next_int(2);
        t.ndata[i] = choose_near_point(i, bid, rnd);
        return t.pdata[i] == t.ndata[i] ? Trans() : t;
    }

    Trans create_rotate_trans(Xorshift& rnd) const {
        Trans t;
        int bid = rnd.next_int((int)sps[0].size());
        t.type = Trans::Type::CHANGE;
        t.idx = bid;
        t.pdata = { sps[0][bid], sps[1][bid], rots[bid] };
        t.ndata = t.pdata;
        t.ndata[2] = rnd.next_int(24);
        return t;
    }

    Trans create_add_trans(Xorshift& rnd) const {
        Trans t;
        if ((int)sps[0].size() == std::min(ndom[0], ndom[1])) return t; // invalid
        if ((int)sps[0].size() >= num_blocks_cap) return t; // invalid
        t.type = Trans::Type::ADD;
        int nsp0 = choose_critical_point(0, rnd);
        int nsp1 = choose_critical_point(1, rnd);
        int nrot = rnd.next_int(24);
        t.ndata = { nsp0, nsp1, nrot };
        return t;
    }

    Trans create_remove_trans(Xorshift& rnd) const {
        Trans t;
        if (sps[0].size() == 1) return t; // invalid
        int bid = rnd.next_int((int)sps[0].size());
        t.type = Trans::Type::REMOVE;
        t.idx = bid;
        t.pdata = { sps[0][bid], sps[1][bid], rots[bid] };
        return t;
    }

    Trans create_trans(Xorshift& rnd) const {
        // TODO: adjust trans ratio
        int r = rnd.next_int(100);
        if (r <= 0) return create_add_trans(rnd);
        if (r <= 2) return create_remove_trans(rnd);
        if (r <= 33) return create_change_trans(rnd);
        if (r <= 66) return create_slide_trans(rnd);
        return create_rotate_trans(rnd);
    }

    void accept_change(const Trans& t) {
        sused[0][t.pdata[0]] = false;
        sused[1][t.pdata[1]] = false;
        sps[0][t.idx] = t.ndata[0];
        sps[1][t.idx] = t.ndata[1];
        rots[t.idx] = t.ndata[2];
        sused[0][t.ndata[0]] = true;
        sused[1][t.ndata[1]] = true;
    }

    void accept_add(const Trans& t) {
        sused[0][t.ndata[0]] = true;
        sused[1][t.ndata[1]] = true;
        sps[0].push_back(t.ndata[0]);
        sps[1].push_back(t.ndata[1]);
        rots.push_back(t.ndata[2]);
    }

    void accept_remove(const Trans& t) {
        sused[0][t.pdata[0]] = false;
        sused[1][t.pdata[1]] = false;
        std::swap(sps[0][t.idx], sps[0].back()); sps[0].pop_back();
        std::swap(sps[1][t.idx], sps[1].back()); sps[1].pop_back();
        std::swap(rots[t.idx], rots.back()); rots.pop_back();
    }

    void accept(const Trans& t) {
        if (t.type == Trans::Type::CHANGE) accept_change(t);
        else if (t.type == Trans::Type::ADD) accept_add(t);
        else if (t.type == Trans::Type::REMOVE) accept_remove(t);
        else assert(false);
    }

    void undo_change(const Trans& t) {
        sused[0][t.ndata[0]] = false;
        sused[1][t.ndata[1]] = false;
        sps[0][t.idx] = t.pdata[0];
        sps[1][t.idx] = t.pdata[1];
        rots[t.idx] = t.pdata[2];
        sused[0][t.pdata[0]] = true;
        sused[1][t.pdata[1]] = true;
    }

    void undo_add(const Trans& t) {
        sused[0][t.ndata[0]] = false;
        sused[1][t.ndata[1]] = false;
        sps[0].pop_back();
        sps[1].pop_back();
        rots.pop_back();
    }

    void undo_remove(const Trans& t) {
        sused[0][t.pdata[0]] = true;
        sused[1][t.pdata[1]] = true;
        sps[0].push_back(t.pdata[0]); std::swap(sps[0][t.idx], sps[0].back());
        sps[1].push_back(t.pdata[1]); std::swap(sps[1][t.idx], sps[1].back());
        rots.push_back(t.pdata[2]); std::swap(rots[t.idx], rots.back());
    }

    void undo(const Trans& t) {
        if (t.type == Trans::Type::CHANGE) undo_change(t);
        else if (t.type == Trans::Type::ADD) undo_add(t);
        else if (t.type == Trans::Type::REMOVE) undo_remove(t);
        else assert(false);
    }

    double compute_estimated_cost() {
        reset_states();
        while (true) {
            bool update = false;
            for (int i = 0; i < (int)sps[0].size(); i++) update |= grow(i);
            if (!update) break;
        }
        int pp_cost = calc_postprocess_cost();
        cost += pp_cost;
        if (!pp_cost) {
            chmin(num_blocks_cap, (int)sps[0].size());
            chmin(num_blocks_min, (int)sps[0].size());
        }
        return cost;
    }

    void save_state() {
        tmp_sps = sps;
        tmp_rots = rots;
    }

    void load_state() {
        sps = tmp_sps;
        rots = tmp_rots;
    }

    void preprocess(Xorshift& rnd) {
        sps[0].clear();
        sps[1].clear();
        rots.clear();
        reset_states();
        while (calc_postprocess_cost()) {
            auto t = create_add_trans(rnd);
            if (t.type == Trans::Type::INVALID) break;
            accept(t);
            compute_estimated_cost();
            if (chmin(min_cost, cost)) {
                dump(min_cost);
                save_state();
            }
        }
    }

    void postprocess() {
        while (frem[0] + rrem[0] > 0 || frem[1] + rrem[1] > 0) {
            vector<int> primary[2], secondary[2];
            int bs[2] = { -1, -1 };
            for (int i = 0; i < 2; i++) {
                if (frem[i] + rrem[i] == 0) continue;
                for (int p : pdom[i]) {
                    if (used[i][p]) continue;
                    int fp = fproj(p), rp = rproj(p);
                    if (!fsum[i][fp] && !rsum[i][rp]) primary[i].push_back(p);
                    else if (!fsum[i][fp] || !rsum[i][rp]) secondary[i].push_back(p);
                }
                if (!primary[i].empty()) bs[i] = primary[i].front();
                else if (!secondary[i].empty()) bs[i] = secondary[i].front();
            }
            for (int i = 0; i < 2; i++) {
                if (bs[i] == -1) continue;
                frem[i] -= (++fsum[i][fproj(bs[i])]) == 1;
                rrem[i] -= (++rsum[i][rproj(bs[i])]) == 1;
                used[i][bs[i]] = true;
            }
            pp_blocks.emplace_back(bs[0], bs[1]);
        }
    }

    void solve(Xorshift& rnd, Timer& timer, double duration) {

        double end_time = duration;

        for (int i = 0; i < 1000 && timer.elapsed_ms() < 1000; i++) preprocess(rnd);
        dump(timer.elapsed_ms(), min_cost);
        load_state();
        compute_estimated_cost();

        int loop = 0;
        int reject_count = 0;
        double updated_time = 0.0;
        double start_time = timer.elapsed_ms(), now_time;
        const double start_temp = 0.5, end_temp = 0.2;
        while ((now_time = timer.elapsed_ms()) < end_time) {

            loop++;
            if (!(loop & 0x7FFF)) {
                cerr << format("mcost=%6.3f, cost=%6.3f, blk=%2d, mblk=%5d, cblk=%5d, upd=%6.1f\n", min_cost, cost, (int)sps[0].size(), num_blocks_min, num_blocks_cap, updated_time);
            }

            auto t = create_trans(rnd);
            if (t.type == Trans::Type::INVALID) continue;
            accept(t);

            double prev_cost = cost;
            compute_estimated_cost();
            double diff = cost - prev_cost;
            double temp = get_temp(start_temp, end_temp, now_time - start_time, end_time - start_time);
            double prob = exp(-diff / temp);

            if (rnd.next_double() < prob) {
                if (chmin(min_cost, cost)) {
                    reject_count = 0;
                    updated_time = now_time;
                    save_state();
                }
            }
            else {
                reject_count++;
                undo(t);
                cost = prev_cost;
                if (reject_count >= 10000) {
                    reject_count = 0;
                    load_state();
                    compute_estimated_cost();
                }
            }

            if (
                num_blocks_cap != 10000 &&
                num_blocks_cap == num_blocks_min &&
                now_time - updated_time > 300.0
                )
            {
                num_blocks_cap = num_blocks_min - 1;
                updated_time = now_time;
                cerr << format("mcost=%6.3f, cost=%6.3f, blk=%2d, mblk=%5d, cblk=%5d, upd=%6.1f\n", min_cost, cost, (int)sps[0].size(), num_blocks_min, num_blocks_cap, updated_time);
            }

            if (
                num_blocks_cap != 10000 &&
                num_blocks_cap < num_blocks_min &&
                now_time - updated_time > 300.0
                )
            {
                num_blocks_cap = num_blocks_min;
                updated_time = now_time;
                cerr << format("mcost=%6.3f, cost=%6.3f, blk=%2d, mblk=%5d, cblk=%5d, upd=%6.1f\n", min_cost, cost, (int)sps[0].size(), num_blocks_min, num_blocks_cap, updated_time);
            }

        }

        load_state();
        min_cost = compute_estimated_cost();
        dump(loop, min_cost);

        postprocess();

    }

    void output(std::ostream& out) const {
        int num_seeds = (int)sps[0].size();
        array<array<int, 4096>, 2> ans = {};
        for (int k = 0; k < num_seeds; k++) {
            int id = k + 1;
            for (int i = 0; i < 2; i++) {
                for (int p : blobs[k].pts[i]) {
                    ans[i][p] = id;
                }
            }
        }
        for (int k = 0; k < (int)pp_blocks.size(); k++) {
            int id = num_seeds + k + 1;
            auto [p0, p1] = pp_blocks[k];
            if (p0 != -1) ans[0][p0] = id;
            if (p1 != -1) ans[1][p1] = id;
        }
        out << num_seeds + pp_blocks.size() << '\n';
        for (int i = 0; i < 2; i++) {
            for (int x = 1; x <= D; x++) {
                for (int y = 1; y <= D; y++) {
                    for (int z = 1; z <= D; z++) {
                        out << ans[i][enc(x, y, z)] << ' ';
                    }
                }
            }
            out << '\n';
        }
    }

};


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {

    Timer timer;

#ifdef HAVE_OPENCV_HIGHGUI
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
#endif

#ifdef _MSC_VER
    std::ifstream ifs("../../tools_win/in_large/0001.txt");
    std::istream& in = ifs;
    std::ofstream ofs("../../tools_win/out/0009.txt");
    std::ostream& out = ofs;
#else
    std::istream& in = cin;
    std::ostream& out = cout;
#endif

    Input input(in);

    Xorshift rnd;

    Solver solver(input);
    solver.solve(rnd, timer, 5950);
    solver.output(out);

    dump(timer.elapsed_ms());

    return 0;
}