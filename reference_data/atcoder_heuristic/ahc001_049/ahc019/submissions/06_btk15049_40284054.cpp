#ifndef VSCODE
// clang-format off
 #pragma GCC optimize("Ofast")
 #ifndef FLAME_GRAPH
 #pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")
 #pragma GCC optimize("O3")
 #pragma GCC optimize("omit-frame-pointer")
 #pragma GCC optimize("inline")
 #pragma GCC optimize("unroll-loops")
 #define NDEBUG
 #endif
// clang-format on
#endif

constexpr double TIMELIMIT = 5.7;

constexpr double T_START =
#ifdef PARAM_T_START
    PARAM_T_START
#else
    0.5
#endif
    ;

constexpr double T_END =
#ifdef PARAM_T_END
    PARAM_T_END
#else
    0.15
#endif
    ;

constexpr int ADD_TH_5 =
#ifdef PARAM_ADD_TH_5
    PARAM_ADD_TH_5
#else
    1
#endif
    ;

constexpr int ADD_TH_14 =
#ifdef PARAM_ADD_TH_14
    PARAM_ADD_TH_14
#else
    1
#endif
    ;

constexpr int ADD_TH_END =
#ifdef PARAM_ADD_TH_END
    PARAM_ADD_TH_END
#else
    1
#endif
    ;

constexpr int ERASE_TH_5 =
#ifdef PARAM_ERASE_TH_5
    PARAM_ERASE_TH_5
#else
    15
#endif
    ;

constexpr int ERASE_TH_14 =
#ifdef PARAM_ERASE_TH_14
    PARAM_ERASE_TH_14
#else
    10
#endif
    ;

constexpr int ERASE_TH_END =
#ifdef PARAM_ERASE_TH_END
    PARAM_ERASE_TH_END
#else
    60
#endif
    ;

constexpr int RESET_TH_5 =
#ifdef PARAM_RESET_TH_5
    PARAM_RESET_TH_5
#else
    100
#endif
    ;

constexpr int RESET_TH_14 =
#ifdef PARAM_RESET_TH_14
    PARAM_RESET_TH_14
#else
    100
#endif
    ;

constexpr int RESET_TH_END =
#ifdef PARAM_RESET_TH_END
    PARAM_RESET_TH_END
#else
    100
#endif
    ;

constexpr int RESET_CUBE_TH =
#ifdef PARAM_RESET_CUBE_TH
    PARAM_RESET_CUBE_TH
#else
    50
#endif
    ;


constexpr int KICK_TH =
#ifdef PARAM_KICK_TH
    PARAM_KICK_TH
#else
    5
#endif
    ;


constexpr int ROLLBACK_TH =
#ifdef PARAM_ROLLBACK_TH
    PARAM_ROLLBACK_TH
#else
    10
#endif
    ;

#include <algorithm>
#include <array>
#include <atcoder/dsu>
#include <atcoder/maxflow>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;

namespace xorshift {
    constexpr uint64_t next(uint64_t p) {
        p = p ^ (p << 13);
        p = p ^ (p >> 7);
        return p ^ (p << 17);
    }

    struct Generator {
        uint64_t seed;
        Generator(uint64_t seed = 939393939393llu) : seed(seed) {}
        inline uint64_t gen() {
            seed = next(seed);
            return seed;
        }
    } _gen;

    // https://github.com/yosupo06/library-checker-problems/blob/5face7acd002a8fe2cd76fa6744f0901c194eb36/common/random.h#L38-L51
    // random choice from [0, upper]
    inline uint64_t gen_with_upper(uint64_t upper) {
        if (!(upper & (upper + 1))) {
            // b = 00..0011..11
            return _gen.gen() & upper;
        }
        int lg        = 63 - __builtin_clzll(upper);
        uint64_t mask = (lg == 63) ? ~0ULL : (1ULL << (lg + 1)) - 1;
        while (true) {
            uint64_t r = _gen.gen() & mask;
            if (r <= upper) return r;
        }
    }

    inline int64_t getInt(int64_t n) { return gen_with_upper(n - 1); }
    inline uint64_t getUint() { return _gen.gen(); }

    inline double getDouble() { return (_gen.gen() % 65536) / 65535.0; }
    template <typename T>
    inline void shuffle(std::vector<T>& v) {
        const int n = v.size();
        for (int i = 0; i < n - 1; i++) {
            std::swap(v[i], v[i + 1 + getInt(n - i - 1)]);
        }
    }

    void set_seed(uint64_t seed) { _gen = Generator(seed); }
    uint64_t get_seed() { return _gen.seed; }
} // namespace xorshift

using namespace std;


constexpr int dx[] = {0, 0, 1, 0, 0, -1};
constexpr int dy[] = {0, 1, 0, 0, -1, 0};
constexpr int dz[] = {1, 0, 0, -1, 0, 0};

constexpr int dr[][6] = {
    {0, 1, 2, 3, 4, 5}, {0, 5, 1, 3, 2, 4}, {0, 4, 5, 3, 1, 2},
    {0, 2, 4, 3, 5, 1}, {2, 1, 3, 5, 4, 0}, {1, 5, 3, 4, 2, 0},
    {5, 4, 3, 2, 1, 0}, {4, 2, 3, 1, 5, 0}, {3, 1, 5, 0, 4, 2},
    {3, 5, 4, 0, 2, 1}, {3, 4, 2, 0, 1, 5}, {3, 2, 1, 0, 5, 4},
    {5, 1, 0, 2, 4, 3}, {4, 5, 0, 1, 2, 3}, {2, 4, 0, 5, 1, 3},
    {1, 2, 0, 4, 5, 3}, {4, 0, 2, 1, 3, 5}, {2, 0, 1, 5, 3, 4},
    {1, 0, 5, 4, 3, 2}, {5, 0, 4, 2, 3, 1}, {4, 2, 3, 1, 5, 0},
    {2, 1, 3, 5, 4, 0}, {1, 5, 3, 4, 2, 0}, {5, 4, 3, 2, 1, 0},
    {4, 3, 5, 1, 0, 2}, {2, 3, 4, 5, 0, 1}, {1, 3, 2, 4, 0, 5},
    {5, 3, 1, 2, 0, 4}, {4, 5, 0, 1, 2, 3}, {2, 4, 0, 5, 1, 3},
    {1, 2, 0, 4, 5, 3}, {5, 1, 0, 2, 4, 3}, {3, 4, 2, 0, 1, 5},
    {3, 2, 1, 0, 5, 4}, {3, 1, 5, 0, 4, 2}, {3, 5, 4, 0, 2, 1},
    {5, 4, 3, 2, 1, 0}, {4, 2, 3, 1, 5, 0}, {2, 1, 3, 5, 4, 0},
    {1, 5, 3, 4, 2, 0}, {0, 4, 5, 3, 1, 2}, {0, 2, 4, 3, 5, 1},
    {0, 1, 2, 3, 4, 5}, {0, 5, 1, 3, 2, 4}, {2, 4, 0, 5, 1, 3},
    {1, 2, 0, 4, 5, 3}, {5, 1, 0, 2, 4, 3}, {4, 5, 0, 1, 2, 3},
    {1, 3, 2, 4, 0, 5}, {5, 3, 1, 2, 0, 4}, {4, 3, 5, 1, 0, 2},
    {2, 3, 4, 5, 0, 1}, {1, 5, 3, 4, 2, 0}, {5, 4, 3, 2, 1, 0},
    {4, 2, 3, 1, 5, 0}, {2, 1, 3, 5, 4, 0}, {1, 0, 5, 4, 3, 2},
    {5, 0, 4, 2, 3, 1}, {4, 0, 2, 1, 3, 5}, {2, 0, 1, 5, 3, 4},
    {1, 2, 0, 4, 5, 3}, {5, 1, 0, 2, 4, 3}, {4, 5, 0, 1, 2, 3},
    {2, 4, 0, 5, 1, 3},
};

int D, N;
bool F1[16][16];
bool R1[16][16];
bool F2[16][16];
bool R2[16][16];

// allocable
bool A1[16 * 16 * 16];
bool A2[16 * 16 * 16];


bitset<6> adj_bitset1[16 * 16 * 16][64];
bitset<6> adj_bitset2[16 * 16 * 16][64];
vector<int8_t> allocatable_dir[16 * 16 * 16][64];

struct Point {
    int x, y, z;
    Point(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
    inline uint32_t to_id() const { return (x << 8) | (y << 4) | z; }
    inline bool a1() const { return A1[to_id()]; }
    inline bool a2() const { return A2[to_id()]; }
    inline Point adj(int r, int d) const {
        return Point(x + dx[dr[r][d]], y + dy[dr[r][d]], z + dz[dr[r][d]]);
    }

    inline static Point from_id(int id) {
        return Point(id >> 8, (id & 255) >> 4, id & 15);
    }
    string str() const {
        stringstream ss;
        ss << "(" << x << "," << y << "," << z << ")";
        return ss.str();
    }
};

int connections1 = 0;
int connections2 = 0;
vector<Point> allocatable_points1;
vector<Point> allocatable_points2;

void input() {
    cin >> D;
    N = D + 2;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            F1[i][j] = false;
            R1[i][j] = false;
            F2[i][j] = false;
            R2[i][j] = false;
            for (int k = 0; k < N; k++) {
                A1[Point(i, j, k).to_id()] = false;
                A2[Point(i, j, k).to_id()] = false;
            }
        }
    }
    for (auto p : {F1, R1, F2, R2}) {
        for (int i = 0; i < D; i++) {
            string s;
            cin >> s;
            for (int j = 0; j < D; j++) {
                p[j + 1][i + 1] = s[j] == '1';
            }
        }
    }
    for (int x = 1; x <= D; x++) {
        for (int y = 1; y <= D; y++) {
            for (int z = 1; z <= D; z++) {
                A1[Point(x, y, z).to_id()] = F1[x][z] && R1[y][z];
                A2[Point(x, y, z).to_id()] = F2[x][z] && R2[y][z];
            }
        }
    }
    for (int x = 1; x <= D; x++) {
        for (int y = 1; y <= D; y++) {
            for (int z = 1; z <= D; z++) {
                if (A1[Point(x, y, z).to_id()])
                    allocatable_points1.emplace_back(x, y, z);
                if (A2[Point(x, y, z).to_id()])
                    allocatable_points2.emplace_back(x, y, z);
                for (int r = 0; r < 64; r++) {
                    adj_bitset1[Point(x, y, z).to_id()][r].reset();
                    for (int d = 0; d < 6; d++) {
                        Point p(x, y, z);
                        Point q = p.adj(r, d);
                        if (p.a1() && q.a1()) {
                            adj_bitset1[p.to_id()][r].set(d);
                            allocatable_dir[p.to_id()][r].emplace_back(d);
                            connections1++;
                        }
                        if (p.a2() && q.a2()) {
                            adj_bitset2[p.to_id()][r].set(d);
                            connections2++;
                        }
                    }
                }
            }
        }
    }
}

// t * 500000 = e * 3.4 / 1000 + 1.6
// t = (e * 3.4 / 1000 + 1.6) / 500000
//

int arg_xyz(int x, int y, int z) {
    for (int i = 0; i < 6; i++) {
        if (x == dx[i] && y == dy[i] && z == dz[i]) {
            return i;
        }
    }
    return 999999;
}

int color = 1;

int RET1[16][16][16];
int RET2[16][16][16];

int output() {
    map<int, int> id;
    for (int i = 1; i <= D; i++) {
        for (int j = 1; j <= D; j++) {
            for (int k = 1; k <= D; k++) {
                id[RET1[i][j][k]] = (RET1[i][j][k] <= 0 ? 0 : 1);
                id[RET2[i][j][k]] = (RET2[i][j][k] <= 0 ? 0 : 1);
            }
        }
    }

    int n = 1;
    for (auto& i : id) {
        if (i.second != 0) {
            i.second = n++;
        }
    }
    cout << n - 1 << endl;
    for (int i = 1; i <= D; i++) {
        for (int j = 1; j <= D; j++) {
            for (int k = 1; k <= D; k++) {
                if (i + j + k != 3) cout << " ";
                cout << id[RET1[i][j][k]];
            }
        }
    }
    cout << endl;
    for (int i = 1; i <= D; i++) {
        for (int j = 1; j <= D; j++) {
            for (int k = 1; k <= D; k++) {
                if (i + j + k != 3) cout << " ";
                cout << id[RET2[i][j][k]];
            }
        }
    }
    cout << endl;
    return n;
}

vector<Point> calc_need(decltype(F1) f, decltype(R1) r, decltype(RET1) ret) {
    set<int> need_f, need_r;
    for (int i = 1; i <= D; i++) {
        for (int j = 1; j <= D; j++) {
            if (f[i][j]) need_f.insert(i * 16 + j);
            if (r[i][j]) need_r.insert(i * 16 + j);
        }
    }
    for (int i = 1; i <= D; i++) {
        for (int j = 1; j <= D; j++) {
            for (int k = 1; k <= D; k++) {
                if (ret[i][j][k] > 0) {
                    if (need_f.count(i * 16 + k)) need_f.erase(i * 16 + k);
                    if (need_r.count(j * 16 + k)) need_r.erase(j * 16 + k);
                }
            }
        }
    }

    vector<pair<int, int>> edges;
    using namespace atcoder;
    mf_graph<int> g(512 + 2);
    for (int i : need_f) {
        for (int j : need_r) {
            if (i % 16 != j % 16) continue;
            edges.emplace_back(i, j);
            g.add_edge(i, 256 + j, 1);
        }
    }
    for (auto i : need_f) g.add_edge(512, i, 1);
    for (auto j : need_r) g.add_edge(256 + j, 513, 1);

    g.flow(512, 513);
    vector<Point> pts;
    for (int i = 0; i < (int)edges.size(); i++) {
        if (g.get_edge(i).flow == 1) {
            const int x = edges[i].first / 16;
            const int y = edges[i].second / 16;
            const int z = edges[i].first % 16;
            assert(z == edges[i].second % 16);
            need_f.erase(edges[i].first);
            need_r.erase(edges[i].second);
            pts.emplace_back(x, y, z);
        }
    }

    for (int i = 1; i <= D; i++) {
        for (int j = 1; j <= D; j++) {
            for (int k = 1; k <= D; k++) {
                if (ret[i][j][k] == -1) {
                    if (need_f.count(i * 16 + k)) {
                        pts.emplace_back(i, j, k);
                        need_f.erase(i * 16 + k);
                    }
                    if (need_r.count(j * 16 + k)) {
                        pts.emplace_back(i, j, k);
                        need_r.erase(j * 16 + k);
                    }
                }
            }
        }
    }

    return pts;
}

int last_matching() {
    int score           = 0;
    vector<Point> need1 = calc_need(F1, R1, RET1);
    vector<Point> need2 = calc_need(F2, R2, RET2);
    while ((!need1.empty()) && (!need2.empty())) {
        Point p = need1.back();
        need1.pop_back();
        Point q = need2.back();
        need2.pop_back();
        RET1[p.x][p.y][p.z] = color;
        RET2[q.x][q.y][q.z] = color;
        color++;
        score++;
    }

    while (!need1.empty()) {
        Point p = need1.back();
        need1.pop_back();
        RET1[p.x][p.y][p.z] = color;
        color++;
        score++;
    }
    while (!need2.empty()) {
        Point q = need2.back();
        need2.pop_back();
        RET2[q.x][q.y][q.z] = color;
        color++;
        score++;
    }
    return score;
}

constexpr int max_pool_size = 50 * 3 * 3;
uint32_t raw_array[max_pool_size][2744];

int pool_size = 0;
uint32_t* pool[max_pool_size];

void init_pool() {
    for (int i = 0; i < max_pool_size; i++) {
        pool[pool_size++] = raw_array[i];
    }
}

inline uint32_t* pull_ptr() {
    assert(pool_size > 0);
    // cerr << "pull " << pool_size << " " << pool[pool_size - 1] << endl;
    return pool[--pool_size];
}

inline void push_ptr(uint32_t* p) {
    // cerr << "push " << pool_size << " " << p << endl;
    pool[pool_size++] = p;
}


struct Container {
    int sz        = 0;
    uint32_t* ptr = nullptr;
    inline bool empty() const { return sz == 0; }
    inline int size() const { return sz; }
    inline void push(uint32_t x) { ptr[sz++] = x; }
    inline void pop() { sz--; }
    inline void clear() {
        // cerr << "clear"
        //      << " " << ptr << endl;
        sz = 0;
    }
    inline uint32_t& back() { return ptr[sz - 1]; }
    inline uint32_t& operator[](int i) { return ptr[i]; }
    inline uint32_t operator[](int i) const { return ptr[i]; }

    inline uint32_t const* data() const { return ptr; }

    Container() : ptr(pull_ptr()) {}
    inline void release() {
        push_ptr(ptr);
        ptr = nullptr;
    }
    inline Container copy() {
        Container ret;
        ret.sz = sz;
        for (int i = 0; i < sz; i++) {
            ret.ptr[i] = ptr[i];
        }
        return ret;
    }
};

#define SWAP_MEMBER(member) swap(lhs.member, rhs.member)

inline void swap_container(Container& lhs, Container& rhs) {
    SWAP_MEMBER(sz);
    SWAP_MEMBER(ptr);
}


struct PolyCube {
    int adj_size = 0;
    int r1;
    int r2;
    Container points;
    Container adj_point_ids;
    Container occupied_adj_point_ids;
    bool modified;

    inline PolyCube copy() const {
        PolyCube ret(r1, r2, modified);
        ret.adj_size = adj_size;
        for (int i = 0; i < points.size(); i++) {
            ret.points.push(points[i]);
        }
        for (int i = 0; i < adj_point_ids.size(); i++) {
            ret.adj_point_ids.push(adj_point_ids[i]);
        }
        for (int i = 0; i < occupied_adj_point_ids.size(); i++) {
            ret.occupied_adj_point_ids.push(occupied_adj_point_ids[i]);
        }

        return ret;
    }
    PolyCube(int r1 = 0, int r2 = 0, bool modified = true)
        : r1(r1), r2(r2), modified(modified) {
        // points.reserve(16);
        // adj_point_ids.reserve(16);
        // occupied_adj_point_ids.reserve(16);
    }
    inline int size() const { return points.size(); }
    inline void add_adj_point(Point p, Point q) {
        uint32_t id_pair = (p.to_id() << 16) + q.to_id();
        const auto* ar   = adj_point_ids.data();
        for (int i = 0; i < adj_size; i++) {
            if (ar[i] == id_pair) return;
        }
        adj_point_ids.push(id_pair);
        adj_size++;
    }
    inline void add_occupied_adj_point(Point p, Point q) {
        occupied_adj_point_ids.push((p.to_id() << 16) + q.to_id());
    }
    inline bool has_adj() { return adj_size != 0; }
    inline pair<Point, Point> choice_adj() {
        const int r  = xorshift::getInt(adj_size);
        auto id_pair = adj_point_ids[r];
        swap(adj_point_ids[r], adj_point_ids.back());
        adj_point_ids.pop();
        adj_size--;
        return pair<Point, Point>(Point::from_id(id_pair >> 16),
                                  Point::from_id(id_pair & ((1 << 16) - 1)));
    }
    void color(int c) const {
        for (int i = 0; i < points.size(); i++) {
            const auto id_pair  = points[i];
            Point p             = Point::from_id(id_pair >> 16);
            Point q             = Point::from_id(id_pair & ((1 << 16) - 1));
            RET1[p.x][p.y][p.z] = c;
            RET2[q.x][q.y][q.z] = c;
        }
    }
    inline void release_container() {
        points.release();
        adj_point_ids.release();
        occupied_adj_point_ids.release();
    }
};

inline void cube_swap(PolyCube& lhs, PolyCube& rhs) {
    SWAP_MEMBER(r1);
    SWAP_MEMBER(r2);
    SWAP_MEMBER(modified);
    SWAP_MEMBER(adj_size);
    swap_container(lhs.adj_point_ids, rhs.adj_point_ids);
    swap_container(lhs.occupied_adj_point_ids, rhs.occupied_adj_point_ids);
    swap_container(lhs.points, rhs.points);
}

bool b1[16][16][16]; // 立体的に埋まっているか
bool b2[16][16][16]; // 立体的に埋まっているか
int s1[16][16];      // 平面的に埋まっているか
int t1[16][16];      // 平面的に埋まっているか
int s2[16][16];      // 平面的に埋まっているか
int t2[16][16];      // 平面的に埋まっているか

void reset_all() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                b1[i][j][k] = false;
                b2[i][j][k] = false;
            }
            s1[i][j] = 0;
            t1[i][j] = 0;
            s2[i][j] = 0;
            t2[i][j] = 0;
        }
    }
}

inline bool is_occupied1(Point p) { return b1[p.x][p.y][p.z]; }
inline bool is_occupied2(Point p) { return b2[p.x][p.y][p.z]; }
inline void occupy1(Point p) {
    assert(!b1[p.x][p.y][p.z]);
    b1[p.x][p.y][p.z] = true;
    s1[p.x][p.z]++;
    t1[p.y][p.z]++;
}
inline void occupy2(Point p) {
    assert(!b2[p.x][p.y][p.z]);
    b2[p.x][p.y][p.z] = true;
    s2[p.x][p.z]++;
    t2[p.y][p.z]++;
}
inline void remove1(Point p) {
    assert(b1[p.x][p.y][p.z]);
    b1[p.x][p.y][p.z] = false;
    s1[p.x][p.z]--;
    t1[p.y][p.z]--;
}
inline void remove2(Point p) {
    assert(b2[p.x][p.y][p.z]);
    b2[p.x][p.y][p.z] = false;
    s2[p.x][p.z]--;
    t2[p.y][p.z]--;
}

struct SearchResult {
    vector<PolyCube> cubes;
    double actual_score    = 0;
    double estimated_score = 0;
};

inline bool ok_p(Point p, int r) {
    return p.a1() && !is_occupied1(p) && adj_bitset1[p.to_id()][r].any();
}

inline bool ok_pq(Point p, int rp, Point q, int rq) {
    return q.a2() && !is_occupied2(q)
           && (adj_bitset1[p.to_id()][rp] & adj_bitset2[q.to_id()][rq]).any();
}

inline void reset(const PolyCube& cube) {
    const int sz     = cube.points.size();
    const auto* data = cube.points.data();
    for (int i = 0; i < sz; i++) {
        const auto id_pair = data[i];
        // cerr << "reset " << id_pair << endl;
        remove1(Point::from_id(id_pair >> 16));
        remove2(Point::from_id(id_pair & ((1 << 16) - 1)));
    }
}
inline void sync(const PolyCube& cube) {
    const int sz     = cube.points.size();
    const auto* data = cube.points.data();
    for (int i = 0; i < sz; i++) {
        const auto id_pair = data[i];
        occupy1(Point::from_id(id_pair >> 16));
        occupy2(Point::from_id(id_pair & ((1 << 16) - 1)));
    }
}
void sync(const vector<PolyCube>& cubes) {
    // reset と併用
    for (const auto& cube : cubes) {
        sync(cube);
    }
}

vector<PolyCube> backup;

inline void add_adj(PolyCube& cube, Point p, Point q) {
    if (D < 9) {
        for (int d : allocatable_dir[p.to_id()][cube.r1]) {
            // cerr << d << endl;
            const Point np = p.adj(cube.r1, d);
            const Point nq = q.adj(cube.r2, d);
            if (np.a1() && nq.a2() && !is_occupied1(np) && !is_occupied2(nq)) {
                // cerr << "add" << endl;
                cube.add_adj_point(np, nq);
            }
        }
    }
    else {
        for (int d = 0; d < 6; d++) {
            // cerr << d << endl;
            const Point np = p.adj(cube.r1, d);
            const Point nq = q.adj(cube.r2, d);
            if (np.a1() && nq.a2() && !is_occupied1(np) && !is_occupied2(nq)) {
                // cerr << "add" << endl;
                cube.add_adj_point(np, nq);
            }
        }
    }
}

void add_cubes(vector<PolyCube>& cubes, int n) {
    for (int i = 0; i < n; i++) {
        Point p, q;
        int rp, rq;
        for (int ii = 0; ii < 100 && !(ok_p(p, rp) && ok_pq(p, rp, q, rq));
             ii++) {
            for (int j = 0; j < 50 && !ok_p(p, rp); j++) {
                p  = allocatable_points1[xorshift::getInt(
                    allocatable_points1.size())];
                rp = xorshift::getInt(64);
            }
            if (!ok_p(p, rp)) break;
            for (int j = 0; j < 50 && !ok_pq(p, rp, q, rq); j++) {
                q  = allocatable_points2[xorshift::getInt(
                    allocatable_points2.size())];
                rq = xorshift::getInt(64);
            }
        }
        if (!(ok_p(p, rp) && ok_pq(p, rp, q, rq))) break;
        occupy1(p);
        occupy2(q);
        // cerr << "add" << endl;
        auto& cube = cubes.emplace_back(rp, rq);
        // cerr << "done emplace_back" << endl;
        cube.points.push((p.to_id() << 16) + q.to_id());
        add_adj(cube, p, q);
    }
    // cerr << "add end" << endl;
}

bool erase_cube(vector<PolyCube>& cubes) {
    // cerr << "erase_cube" << endl;
    if (cubes.empty()) {
        return false;
    }
    int r = xorshift::getInt(cubes.size());
    // cerr << backup.capacity() << endl;
    backup.emplace_back();
    // cerr << backup.capacity() << endl;

    reset(cubes[r]);
    if (r + 1 != (int)cubes.size()) {
        cube_swap(cubes[r], cubes.back());
    }
    cube_swap(cubes.back(), backup.back());
    cubes.back().release_container();
    cubes.pop_back();
    return true;
}

void reset_cube(PolyCube& cube) {
    // cerr << "reset_cube s"
    //      << " " << cube.points.data() << endl;
    reset(cube);
    assert(!cube.points.empty());
    const int r        = xorshift::getInt(cube.points.size());
    const auto id_pair = cube.points[r];
    Point p            = Point::from_id(id_pair >> 16);
    Point q            = Point::from_id(id_pair & ((1 << 16) - 1));

    cube.points.clear();
    cube.adj_point_ids.clear();
    cube.adj_size = 0;
    cube.occupied_adj_point_ids.clear();
    cube.points.push((p.to_id() << 16) + q.to_id());
    occupy1(p);
    occupy2(q);
    add_adj(cube, p, q);
}

bool reset_cube(vector<PolyCube>& cubes) {
    if (cubes.empty()) {
        return false;
    }
    auto& cube = cubes[xorshift::getInt(cubes.size())];
    if (cube.size() == 1u) {
        return false;
    }
    backup.emplace_back(cube);
    const int r        = xorshift::getInt(cube.points.size());
    const auto id_pair = cube.points[r];
    reset(cube);
    cube    = PolyCube(cube.r1, cube.r2);
    Point p = Point::from_id(id_pair >> 16);
    Point q = Point::from_id(id_pair & ((1 << 16) - 1));
    cube.points.push(id_pair);
    occupy1(p);
    occupy2(q);
    add_adj(cube, p, q);
    return true;
}

void search_prepare(vector<PolyCube>& cubes) {
    for (auto& cube : cubes) {
        const int sz   = cube.occupied_adj_point_ids.size();
        const auto* ar = cube.occupied_adj_point_ids.data();
        for (int i = 0; i < sz; i++) {
            const auto id_pair = ar[i];
            cube.add_adj_point(Point::from_id(id_pair >> 16),
                               Point::from_id(id_pair & ((1 << 16) - 1)));
        }
        cube.occupied_adj_point_ids.clear();
        cube.modified = false;
    }
}

namespace scheduler {
    using namespace std::chrono;
    struct Scheduler {
        high_resolution_clock::time_point bg;
        int64_t limit;
        int64_t progress;
        int64_t latest;
        int64_t latest_interval  = 0;
        constexpr static int buf = 100;
        int not_updated;
        Scheduler() {
            latest      = 0;
            bg          = high_resolution_clock::now();
            limit       = TIMELIMIT * 1e6;
            not_updated = 0;
        }
        inline bool update() {
            if (not_updated >= buf) {
                const int64_t elapsed = duration_cast<microseconds>(
                                            high_resolution_clock::now() - bg)
                                            .count();
                not_updated     = 0;
                latest_interval = elapsed - latest;
                latest          = elapsed;
                progress        = elapsed;
            }
            else {
                not_updated++;
                progress = latest + latest_interval * not_updated / buf;
            }

            return progress < limit;
        }
    };
} // namespace scheduler


SearchResult search(vector<PolyCube>& cubes) {
    SearchResult ret;
    if (cubes.empty()) {
        ret.estimated_score = 1000;
        return ret;
    }
    vector<int> cand(cubes.size());
    iota(cand.begin(), cand.end(), 0);
    for (int i = 0; i < 5000; i++) {
        const int cand_i = xorshift::getInt(cand.size());
        const int cube_i = cand[cand_i];
        auto& cube       = cubes[cube_i];
        if (cube.has_adj()) {
            const auto [p, q] = cube.choice_adj();
            // cerr << p.str() << " " << q.str() << endl;
            if (!is_occupied1(p) && !is_occupied2(q)) {
                // cerr << "ok" << endl;
                occupy1(p);
                occupy2(q);
                if (!cube.modified) {
                    cube.modified = true;
                    backup.emplace_back(cube.copy());
                    backup.back().adj_point_ids.push((p.to_id() << 16)
                                                     + q.to_id());
                }
                cube.points.push((p.to_id() << 16) + q.to_id());
                add_adj(cube, p, q);
            }
            else {
                cube.add_occupied_adj_point(p, q);
            }
        }
        if (!cube.has_adj()) {
            swap(cand[cand_i], cand.back());
            cand.pop_back();
            if (cand.empty()) break;
        }
    }
    for (const auto& cube : cubes) {
        ret.actual_score += 1.0 / cube.size();
    }

    int rest1 = 0;
    int rest2 = 0;

    for (int i = 1; i <= D; i++) {
        for (int j = 1; j <= D; j++) {
            if (F1[i][j] && s1[i][j] == 0) rest1++;
            if (F2[i][j] && s2[i][j] == 0) rest2++;
            if (R1[i][j] && t1[i][j] == 0) rest1++;
            if (R2[i][j] && t2[i][j] == 0) rest2++;
        }
    }
    // cerr << rest1 << " " << rest2 << endl;

    ret.estimated_score = ret.actual_score + max(rest1, rest2);

    return ret;
}


constexpr int initial_size = 200;
constexpr double timelimit = 5.0;

inline int calc_param(int lo, int hi) { return lo + (hi - lo) * (D - 5) / 10; }
inline int calc_param(int lo, int hi, int cur, int lim) {
    return lo + (hi - lo) * cur / lim;
}

double calc_temperature(int64_t i, int64_t tot) {
    // const double delta = (T_END - T_START) / (tot - 1);
    // return T_START + delta * i;
    double alpha = pow(T_END / T_START, 1.0 / (tot - 1));
    return T_START * pow(alpha, i);
}

double calc_probability(double current_score, double next_score, double t) {
    return exp((current_score - next_score) / t);
}

struct SearchResultWithCube {
    vector<PolyCube> cubes;
    SearchResult result;
    SearchResultWithCube copy() const {
        SearchResultWithCube ret;
        for (int i = 0; i < (int)cubes.size(); i++) {
            ret.cubes.emplace_back(cubes[i].copy());
        }
        ret.result = result;
        return ret;
    }
};


void release_container(vector<PolyCube>& cubes) {
    for (auto& cube : cubes) {
        cube.release_container();
    }
}

inline void rollback(vector<PolyCube>& cubes) {
    // cerr << "rollback start" << endl;
    for (int x = -1 + (int)cubes.size(); x >= 0; x--) {
        if (cubes[x].modified) {
            // cerr << x << " rollback " << cubes[x].points.ptr << endl;
            // cerr << "delete" << endl;

            reset(cubes[x]);
            if (x + 1 != (int)cubes.size()) {
                // cerr << "swap s" << endl;
                cube_swap(cubes[x], cubes.back());
                // cerr << "swap d" << endl;
            }
            // cerr << "pop s" << endl;
            cubes.back().release_container();
            cubes.pop_back();
            // cerr << "pop d" << endl;
        }
    }
    const int size = backup.size();
    for (int x = 0; x < size; x++) {
        sync(backup[x]);
        cubes.emplace_back();
        cube_swap(backup[x], cubes.back());
    }

    release_container(backup);
    backup.clear();
}


int main() {
    init_pool();
    backup.reserve(100);
    double score = 0;
    input();
#ifdef LOCAL
    cerr << "D: " + to_string(D) << endl;
#endif
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                RET1[i][j][k] = A1[Point(i, j, k).to_id()] ? -1 : 0;
                RET2[i][j][k] = A2[Point(i, j, k).to_id()] ? -1 : 0;
            }
        }
    }

    SearchResultWithCube best;
    best.result.estimated_score = 10000000;
    for (int i = 0; i < initial_size; i++) {
        // cerr << "#" << i << endl;
        reset_all();
        SearchResultWithCube cur;
        cur.cubes.reserve(100);
        add_cubes(cur.cubes, 4 + xorshift::getInt(3));

        for (int j = 0; j < 5; j++) {
            // cerr << "##" << j << endl;
            search_prepare(cur.cubes);

            cur.result = search(cur.cubes);
            if (best.result.estimated_score > cur.result.estimated_score) {
                // cerr << "update" << endl;
                release_container(best.cubes);
                best = cur.copy();
                assert(best.result.estimated_score
                       == cur.result.estimated_score);
            }
            // cerr << "x" << endl;
            //  cerr << i << " " << j << " " << ret.estimated_score << endl;
            rollback(cur.cubes);
        }
        release_container(cur.cubes);
    }


    const int min_e       = min(connections1, connections2) / 128;
    constexpr double y750 = 3.85;
    constexpr double y0   = 0.80;
    constexpr double a    = (y750 - y0) / 750.0;
    constexpr double b    = y0;
    const int loops       = int(timelimit / ((min_e * a + b) / 500000));
#ifdef LOCAL
    cerr << "v1: " + to_string(allocatable_points1.size()) << endl;
    cerr << "v2: " + to_string(allocatable_points2.size()) << endl;
    cerr << "e1: " + to_string(connections1 / 128) << endl;
    cerr << "e2: " + to_string(connections2 / 128) << endl;

    cerr << "loops: " + to_string(loops) << endl;
#endif

    auto current = best.copy();
    {
        reset_all();
        sync(current.cubes);
        int not_updated = 0;
        for (scheduler::Scheduler s; s.update(); not_updated++) {
            search_prepare(current.cubes);
            const bool has_cube = !current.cubes.empty();
            if (const int r = xorshift::getInt(loops);
                r < KICK_TH && has_cube) {
                for (auto& cube : current.cubes) {
                    if (xorshift::getInt(100) < RESET_CUBE_TH) {
                        reset_cube(cube);
                    }
                }
                current.result.estimated_score = 1000;
            }
            const int add_th = calc_param(calc_param(ADD_TH_5, ADD_TH_14),
                                          ADD_TH_END, s.progress, s.limit);
            const int erase_th =
                add_th
                + calc_param(calc_param(ERASE_TH_5, ERASE_TH_14), ERASE_TH_END,
                             s.progress, s.limit);
            const int reset_th =
                erase_th
                + calc_param(calc_param(RESET_TH_5, RESET_TH_14), RESET_TH_END,
                             s.progress, s.limit);

            if (const int r = xorshift::getInt(1000); r < add_th) {
                // cerr << "add opt" << endl;
                add_cubes(current.cubes, 1);
            }
            else if (r < erase_th && has_cube) {
                // cerr << "erase opt" << endl;
                erase_cube(current.cubes);
            }
            else if (r < reset_th && has_cube) {
                // cerr << "reset opt" << endl;
                const int tt = xorshift::getInt(2) + 2;
                for (int ttt = 0; ttt < tt; ttt++) {
                    // cerr << ttt << "/" << tt << endl;
                    reset_cube(current.cubes);
                }
            }
            else {
                // cerr << "normal opt" << endl;
                if (has_cube) erase_cube(current.cubes);
                // cerr << "normal_opt mid" << endl;
                add_cubes(current.cubes, 1);
                // cerr << "normal_opt done" << endl;
            }
            // cerr << "search" << endl;
            auto local_best = search(current.cubes);
            const double t  = calc_temperature(s.progress, s.limit);
            const double p  = calc_probability(current.result.estimated_score,
                                               local_best.estimated_score, t);
            if (const double r = xorshift::getDouble(); r < p) {
                current.result = local_best;
                assert(current.result.estimated_score
                       == local_best.estimated_score);
#ifdef LOCAL
                if (0) {
                    cerr << to_string(s.progress) + "/" + to_string(s.limit)
                                + ") updated! "
                                + to_string(current.result.estimated_score)
                                + " / " + to_string(best.result.estimated_score)
                                + " " + to_string(t) + " " + to_string(p) + " "
                                + to_string(r)
                         << endl;
                }
#endif
                if (best.result.estimated_score
                    > current.result.estimated_score) {
                    release_container(best.cubes);
                    best = current.copy();

                    not_updated = 0;
                }
                release_container(backup);
                backup.clear();
            }
            else {
                if (not_updated < loops / ROLLBACK_TH) {
                    rollback(current.cubes);
                }
                else {
                    release_container(current.cubes);
                    current     = best.copy();
                    not_updated = 0;
                    reset_all();
                    release_container(backup);
                    backup.clear();
                    sync(best.cubes);
                }
            }
        }
    }
    reset_all();
    for (const auto& cube : best.cubes) {
        cube.color(color++);
    }
    score += best.result.actual_score;
    score += last_matching();
    output();
#ifdef LOCAL
    cerr << "final_score: " + to_string(score) << endl;
    cerr << "log(final_score): " + to_string(log(score)) << endl;
#endif
    return 0;
}
