#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

//#include <atcoder/all>

#ifdef _MSC_VER
#include <intrin.h>
#endif
#ifdef __GNUC__
#include <x86intrin.h>
#endif

#ifdef __clang__
#pragma clang attribute push(__attribute__((target("arch=skylake"))),          \
                             apply_to = function)

#elif defined(__GNUC__)
/*
#pragma GCC target(                                                            \
     "sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,tune=native")
*/
#pragma GCC optimize("Ofast")
#endif

// ========================== macroes ==========================

//#define NDEBUG

#define rep(i, n) for (auto i = 0; (i) < (n); (i)++)
#define rep1(i, n) for (auto i = 1; (i) <= (n); (i)++)
#define rep3(i, s, n) for (auto i = (s); (i) < (n); (i)++)

#define CHECK(var)                                                             \
    do {                                                                       \
        std::cout << #var << '=' << var << endl;                               \
    } while (false)

// ========================== utils ==========================

using namespace std;
using ll = long long;
constexpr double PI = 3.1415926535897932;

// パラメータ
/*
static constexpr auto kErase = 2;             // OPTIMIZE [1, 5]
static constexpr auto kRadius = 3;            // OPTIMIZE [2, 6]
static constexpr auto kAnnealingA = 0.0;      // OPTIMIZE [-15.0, 15.0]
static constexpr auto kAnnealingB = 0.0;      // OPTIMIZE [0.0, 3.0]
static constexpr auto kAnnealingStart = 10.0; // OPTIMIZE LOG [1.0, 100.0]
static constexpr auto kSkipRatio = 0.5;       // OPTIMIZE [0.2, 0.8]
static constexpr auto kTargetDeterminationTrials = 5; // OPTIMIZE LOG [1, 20]
static constexpr auto kAttractionRatio = 0.1;      // OPTIMIZE LOG [0.01, 0.9]
static constexpr auto kMaxAttractionDistance = 11; // OPTIMIZE LOG [4, 99]
static constexpr auto kStartAttraction = 0.01;     // OPTIMIZE LOG [0.001, 0.9]
*/

struct Parameters {
    int kErase;
    int kRadius;
    double kAnnealingA;
    double kAnnealingB;
    double kAnnealingStart;
    double kSkipRatio;
    int kTargetDeterminationTrials;
    double kAttractionRatio;
    int kMaxAttractionDistance;
    double kStartAttraction;
};

// clang-format off
static constexpr auto kParams = array<array<Parameters, 4>, 6>{array<Parameters, 4>{}, {},
    {
        Parameters{3,2,-1.5473559790003477,2.394679104775665,49.113776624126906,0.7036113146961994,16,0.036126246564059016,77,0.002536726332503413},
        Parameters{2,3,-2.7997466782199543,0.8778231017416516,52.22855778054186,0.6755883944317002,17,0.022722374672110926,21,0.0015902744380508261},
        Parameters{3,3,-0.8599083914393143,2.925181215107081,34.252726962443575,0.7658343930163709,14,0.011372323886580011,80,0.1773136876531248},
        Parameters{2,2,-0.970878447624744,2.6933971227722817,32.02546068971461,0.7618535037800346,1,0.6790213567180794,28,0.8101099148122572},
    },
    {
        Parameters{1,2,1.2738622371290367,1.6552921630737858,40.694646724856256,0.5431626325813727,8,0.6065903075248422,65,0.001505397587350384},
        Parameters{2,4,6.746480615222879,1.5901862644204725,22.664344989910102,0.7302122360707419,5,0.3794791595976484,55,0.5733449839402591},
        Parameters{2,4,12.801970168802779,0.32280037269233675,23.366044094213727,0.7592970781871156,15,0.048344719005193965,8,0.05989574639148155},
        Parameters{3,6,10.66367141216971,0.8659339931190402,15.68688064813811,0.7993706157693532,7,0.4289688096539172,11,0.8696570733295481},
    },
    {
        Parameters{1,2,2.2508155287190688,1.9323641154182187,18.288492418318103,0.6868440567993311,6,0.7622903560583656,18,0.043004863796253026},
        Parameters{1,5,8.29331147231568,2.5594476546034066,8.846928181417638,0.796282953547888,10,0.2921342987874078,13,0.4420363438653956},
        Parameters{2,3,6.557866022361303,1.227625211914838,7.370471273129505,0.7173015971354177,9,0.6954617370929896,14,0.6672982285667602},
        Parameters{2,3,11.163084081074832,1.497812317376342,6.675842224388972,0.7467286977739873,12,0.5193328745169765,13,0.6716432245214757},
    },
    {
        Parameters{1,2,0.6087452845947905,2.7075576102742063,20.025154808113335,0.7665642617122794,9,0.41254517020977965,66,0.00964040221592311},
        Parameters{1,4,4.191053692304283,2.483976056853254,31.352544114426127,0.7260112488407435,17,0.7516674208635511,14,0.012207064273955482},
        Parameters{2,5,14.130349664766184,1.837918572197393,5.1018527266269444,0.6314700674149795,16,0.81833434531777,20,0.7278135346718853},
        Parameters{2,3,9.869878450186258,1.271201664578565,36.4455977801112,0.6232497726465488,13,0.46728161720753925,15,0.005453160502932086},
    },
};
// clang-format on

template <class T, class S> inline bool chmin(T& m, S q) {
    if (m > q) {
        m = q;
        return true;
    } else
        return false;
}

template <class T, class S> inline bool chmax(T& m, const S q) {
    if (m < q) {
        m = q;
        return true;
    } else
        return false;
}

// 2 次元ベクトル
template <typename T> struct Vec2 {
    /*
    y 軸正は下方向
    x 軸正は右方向
    回転は時計回りが正（y 軸正を上と考えると反時計回りになる）
    */
    using value_type = T;
    T y, x;
    constexpr inline Vec2() = default;
    constexpr Vec2(const T& arg_y, const T& arg_x) : y(arg_y), x(arg_x) {}
    inline Vec2(const Vec2&) = default;            // コピー
    inline Vec2(Vec2&&) = default;                 // ムーブ
    inline Vec2& operator=(const Vec2&) = default; // 代入
    inline Vec2& operator=(Vec2&&) = default;      // ムーブ代入
    template <typename S>
    constexpr inline Vec2(const Vec2<S>& v) : y((T)v.y), x((T)v.x) {}
    inline Vec2 operator+(const Vec2& rhs) const {
        return Vec2(y + rhs.y, x + rhs.x);
    }
    inline Vec2 operator+(const T& rhs) const { return Vec2(y + rhs, x + rhs); }
    inline Vec2 operator-(const Vec2& rhs) const {
        return Vec2(y - rhs.y, x - rhs.x);
    }
    template <typename S> inline Vec2 operator*(const S& rhs) const {
        return Vec2(y * rhs, x * rhs);
    }
    inline Vec2 operator*(const Vec2& rhs) const { // x + yj とみなす
        return Vec2(x * rhs.y + y * rhs.x, x * rhs.x - y * rhs.y);
    }
    template <typename S> inline Vec2 operator/(const S& rhs) const {
        assert(rhs != 0.0);
        return Vec2(y / rhs, x / rhs);
    }
    inline Vec2 operator/(const Vec2& rhs) const { // x + yj とみなす
        return (*this) * rhs.inv();
    }
    inline Vec2& operator+=(const Vec2& rhs) {
        y += rhs.y;
        x += rhs.x;
        return *this;
    }
    inline Vec2& operator-=(const Vec2& rhs) {
        y -= rhs.y;
        x -= rhs.x;
        return *this;
    }
    template <typename S> inline Vec2& operator*=(const S& rhs) const {
        y *= rhs;
        x *= rhs;
        return *this;
    }
    inline Vec2& operator*=(const Vec2& rhs) { return *this = (*this) * rhs; }
    inline Vec2& operator/=(const Vec2& rhs) { return *this = (*this) / rhs; }
    inline bool operator!=(const Vec2& rhs) const {
        return x != rhs.x || y != rhs.y;
    }
    inline bool operator==(const Vec2& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    inline void rotate(const double& rad) { *this = rotated(rad); }
    inline Vec2<double> rotated(const double& rad) const {
        return (*this) * rotation(rad);
    }
    static inline Vec2<double> rotation(const double& rad) {
        return Vec2(sin(rad), cos(rad));
    }
    static inline Vec2<double> rotation_deg(const double& deg) {
        return rotation(PI * deg / 180.0);
    }
    inline Vec2<double> rounded() const {
        return Vec2<double>(round(y), round(x));
    }
    inline Vec2<double> inv() const { // x + yj とみなす
        const double norm_sq = l2_norm_square();
        assert(norm_sq != 0.0);
        return Vec2(-y / norm_sq, x / norm_sq);
    }
    inline double l2_norm() const { return sqrt(x * x + y * y); }
    inline double l2_norm_square() const { return x * x + y * y; }
    inline T l1_norm() const { return std::abs(x) + std::abs(y); }
    inline double abs() const { return l2_norm(); }
    inline double phase() const { // [-PI, PI) のはず
        return atan2(y, x);
    }
    inline double phase_deg() const { // [-180, 180) のはず
        return phase() * (180.0 / PI);
    }
    inline Vec2 Right() const { return {y, (T)(x + 1)}; }
    inline Vec2 Down() const { return {(T)(y + 1), x}; }
    inline Vec2 Left() const { return {y, (T)(x - 1)}; }
    inline Vec2 Up() const { return {(T)(y - 1), x}; }
};
template <typename T, typename S>
inline Vec2<T> operator*(const S& lhs, const Vec2<T>& rhs) {
    return rhs * lhs;
}
template <typename T> ostream& operator<<(ostream& os, const Vec2<T>& vec) {
    os << vec.y << ' ' << vec.x;
    return os;
}

// 2 次元配列
template <class T, int height, int width> struct Board {
    array<T, height * width> data;
    template <class Int> constexpr inline auto& operator[](const Vec2<Int>& p) {
        return data[width * p.y + p.x];
    }
    template <class Int>
    constexpr inline const auto& operator[](const Vec2<Int>& p) const {
        return data[width * p.y + p.x];
    }
    template <class Int>
    constexpr inline auto& operator[](const initializer_list<Int>& p) {
        return data[width * *p.begin() + *(p.begin() + 1)];
    }
    template <class Int>
    constexpr inline const auto&
    operator[](const initializer_list<Int>& p) const {
        return data[width * *p.begin() + *(p.begin() + 1)];
    }
    constexpr inline void Fill(const T& fill_value) {
        fill(data.begin(), data.end(), fill_value);
    }
    void Print() const {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                cout << data[width * y + x] << " \n"[x == width - 1];
            }
        }
    }
};

// 時間 (秒)
inline double Time() {
    return static_cast<double>(
               chrono::duration_cast<chrono::nanoseconds>(
                   chrono::steady_clock::now().time_since_epoch())
                   .count()) *
           1e-9;
}

// ======================================================================

auto rng = mt19937(42);

namespace input {
int N, K;
}

using Point = Vec2<signed char>;
struct alignas(4) Move {
    Point from, to;
    inline void Print() const {
        cout << (int)from.y << ' ' << (int)from.x << ' ' << (int)to.y << ' '
             << (int)to.x << endl;
    }
    inline bool Empty() const {
        return from.y == 0 && from.x == 0 && to.y == 0 && to.x == 0;
    }
    inline void Reset() {
        from.y = 0;
        from.x = 0;
        to.y = 0;
        to.x = 0;
    }
};

struct Cell {
    signed char color;
    signed char index;
};

static constexpr auto max_ns = array<array<int, 4>, 6>{
    array<int, 4>{},  {}, {20, 26, 32, 39}, {23, 29, 35, 42}, {26, 32, 38, 45},
    {29, 35, 41, 48},
};

template <int K, int n_class> struct Input {
    static constexpr auto max_n = max_ns[K][n_class];
    Board<Cell, max_n, max_n> board;
    array<array<Point, 100>, 6> where;
    void Read() {
        auto number_counts = array<int, 6>();
        for (auto y = 0; y < input::N; y++) {
            string s;
            cin >> s;
            for (auto x = 0; x < input::N; x++) {
                int c = s[x] - '0';
                board[{y, x}] = {(signed char)c, (signed char)number_counts[c]};
                if (c)
                    where[c][number_counts[c]++] = {(signed char)y,
                                                    (signed char)x};
            }
        }
        assert(number_counts[1] == 100);
        assert(number_counts[2] == 100);
    }
};

// from https://atcoder.jp/contests/practice2/submissions/16581325
struct UnionFind {
    array<signed char, 100> data;
    inline UnionFind() { fill(data.begin(), data.end(), (signed char)-1); }
    inline signed char find(const signed char k) {
        return data[k] < 0 ? k : data[k] = find(data[k]);
    }
    inline int unite(signed char x, signed char y) {
        if ((x = find(x)) == (y = find(y)))
            return false;
        if (data[x] > data[y])
            swap(x, y);
        data[x] += data[y];
        data[y] = x;
        return true;
    }
    inline int size(signed char k) { return -data[find(k)]; }
    inline int same(signed char x, signed char y) { return find(x) == find(y); }
};

template <int N, int K> static constexpr auto NClass() {
    if constexpr (K == 2)
        return N < 21 ? 0 : N < 27 ? 1 : N < 33 ? 2 : 3;
    if constexpr (K == 3)
        return N < 24 ? 0 : N < 30 ? 1 : N < 36 ? 2 : 3;
    if constexpr (K == 4)
        return N < 27 ? 0 : N < 33 ? 1 : N < 39 ? 2 : 3;
    if constexpr (K == 5)
        return N < 30 ? 0 : N < 36 ? 1 : N < 42 ? 2 : 3;
    return -1;
}

template <int K, int n_class> struct BFS {
    static constexpr auto max_n = max_ns[K][n_class];
    // 最も size が大きい 1, 2 からの距離
    Board<signed char, max_n, max_n> result;
    int iteration;

    BFS() : result(), iteration(-1) {}

    void Search(const Board<Cell, max_n, max_n>& board, UnionFind& uf,
                const array<Point, 100>& where, const int iter) {
        iteration = iter;
        result.Fill((signed char)127);

        auto max_size = 0;
        auto best_indices = array<signed char, 100>();
        auto n_best_indices = 0;
        for (auto index = (signed char)0; index < 100; index++) {
            const auto siz = uf.size(index);
            if (chmax(max_size, siz)) {
                best_indices[0] = index;
                n_best_indices = 1;
            } else if (max_size == siz) {
                best_indices[n_best_indices++] = index;
            }
        }

        array<Point, max_n * max_n> q;
        auto q_left = 0;
        auto q_right = 0;
        for (auto idx_best_indices = 0; idx_best_indices < n_best_indices;
             idx_best_indices++) {
            const auto c = where[best_indices[idx_best_indices]];
            assert(board[c].color != 0);
            result[c] = 1;
            // 右
            for (auto p = c.Right();
                 p.x < input::N && board[p].color == 0 && result[p] == 127;
                 p.x++) {
                result[p] = 0;
                q[q_right++] = p;
            }
            // 左
            for (auto p = c.Left();
                 p.x >= 0 && board[p].color == 0 && result[p] == 127; p.x--) {
                result[p] = 0;
                q[q_right++] = p;
            }
            // 下
            for (auto p = c.Down();
                 p.y < input::N && board[p].color == 0 && result[p] == 127;
                 p.y++) {
                result[p] = 0;
                q[q_right++] = p;
            }
            // 上
            for (auto p = c.Up();
                 p.y >= 0 && board[p].color == 0 && result[p] == 127; p.y--) {
                result[p] = 0;
                q[q_right++] = p;
            }
        }

        while (q_left != q_right) {
            const auto v = q[q_left++];
            assert(board[v].color == 0 || board[v].color == 9);
            assert(result[v] % 2 == 0 || result[v] == 120);
            const auto r = v.Right();
            if (r.x < input::N && result[r] == 127) {
                if (board[r].color == 0 || board[r].color == 9) {
                    result[r] = result[v] + 2;
                    q[q_right++] = r;
                } else {
                    result[r] = result[v] + 3;
                }
                chmin(result[r], 120);
            }
            const auto l = v.Left();
            if (l.x >= 0 && result[l] == 127) {
                if (board[l].color == 0 || board[l].color == 9) {
                    result[l] = result[v] + 2;
                    q[q_right++] = l;
                } else {
                    result[l] = result[v] + 3;
                }
                chmin(result[l], 120);
            }
            const auto d = v.Down();
            if (d.y < input::N && result[d] == 127) {
                if (board[d].color == 0 || board[d].color == 9) {
                    result[d] = result[v] + 2;
                    q[q_right++] = d;
                } else {
                    result[d] = result[v] + 3;
                }
                chmin(result[d], 120);
            }
            const auto u = v.Up();
            if (u.y >= 0 && result[u] == 127) {
                if (board[u].color == 0 || board[u].color == 9) {
                    result[u] = result[v] + 2;
                    q[q_right++] = u;
                } else {
                    result[u] = result[v] + 3;
                }
                chmin(result[u], 120);
            }
        }
    }
};

template <int K_, int n_class_> struct State {
    static constexpr auto n_class = n_class_;
    static constexpr auto K = K_;
    static constexpr auto kP = kParams[K][n_class];
    static constexpr auto max_n = max_ns[K][n_class];
    Board<Cell, max_n, max_n> board; // 最終状態
    array<array<Point, 100>, 2> where12;
    double score;

    array<UnionFind, 2> ufs;

    array<Move, 500> moves;
    int n_moves;

    array<Move, 500> connections;
    int n_connections;

    int iteration;

    State()
        : board(), where12(), score(), ufs(), moves(), n_moves(), connections(),
          n_connections(), iteration() {}

    inline int RemainingMoves() const {
        return K * 100 - n_moves - n_connections;
    }

    inline void Print() const {
        cout << n_moves << endl;
        for (int i = 0; i < K * 100; i++)
            if (!moves[i].Empty())
                moves[i].Print();
        cout << n_connections << endl;
        for (int i = 0; i < K * 100; i++)
            if (!connections[i].Empty())
                connections[i].Print();
    }

    inline bool Movable(Move pp) const {
        return board[pp.from].color != 0 && board[pp.to].color == 0;
    }

    inline void ApplyMove(Move pp) {
        assert(board[pp.from].color != 0);
        assert(board[pp.to].color == 0);
        board[pp.to] = board[pp.from];
        board[pp.from] = {};
        if (board[pp.to].color <= 2)
            where12[board[pp.to].color - 1][board[pp.to].index] = pp.to;
    }

    inline bool Connect(Move pp, bool no_check = false) {
        const auto index_from = board[pp.from].index;
        const auto index_to = board[pp.to].index;

        if (!no_check) {
            // 右か下を仮定
            if (board[pp.from].color != board[pp.to].color)
                return false;
            if (board[pp.from].color != 1 && board[pp.from].color != 2)
                return false;
            auto& uf = ufs[board[pp.from].color - 1];
            if (uf.same(index_from, index_to))
                return false;
            if (pp.from.y == pp.to.y) {
                for (auto x = pp.from.x + 1; x < pp.to.x; x++)
                    if (board[{(int)pp.from.y, x}].color != 0)
                        return false;
            } else {
                assert(pp.from.x == pp.to.x);
                for (auto y = pp.from.y + 1; y < pp.to.y; y++)
                    if (board[{y, (int)pp.from.x}].color != 0)
                        return false;
            }
        }

        // ここまで来れば接続可能
        {
            auto& uf = ufs[board[pp.from].color - 1];
            if (pp.from.y == pp.to.y) {
                for (auto x = pp.from.x + 1; x < pp.to.x; x++)
                    board[{(int)pp.from.y, x}].color = 9;
            } else {
                assert(pp.from.x == pp.to.x);
                for (auto y = pp.from.y + 1; y < pp.to.y; y++)
                    board[{y, (int)pp.from.x}].color = 9;
            }
            score += uf.size(index_from) * uf.size(index_to);
            uf.unite(index_from, index_to);
        }
        return true;
    }

    void RandomUpdate(const Input<K, n_class>& initial_board, const int iter,
                      array<BFS<K, n_class>, 2>& bfss, const double progress) {
        // inplace で処理する
        // ランダムで 1 手消去する
        // 空いていたら、1/2 で手を追加

        static constexpr auto kCheckPerf = true;
        static auto t_move = 0.0;
        static auto t_conn_1 = 0.0;
        static auto t_conn_2 = 0.0;
        static auto t0 = 0.0;
        if constexpr (kCheckPerf) {
            t0 = Time();
        }

        // 移動に引力を持たせる
        // iteration は古いもの
        auto attraction_enabled =
            iteration >= 100 && progress > kP.kStartAttraction &&
            uniform_real_distribution<>()(rng) < kP.kAttractionRatio;
        const auto attraction_target_num =
            attraction_enabled ? uniform_int_distribution<>(1, 2)(rng) : -1;
        auto attracted_index = -1;
        if (attraction_enabled) {
            // 予め BFS しておく
            // iteration, board, where は古いもの
            auto& bfs = bfss[attraction_target_num - 1];
            const auto& where = where12[attraction_target_num - 1];
            if (iteration != bfs.iteration) {
                bfss[attraction_target_num - 1].Search(
                    board, ufs[attraction_target_num - 1], where, iteration);
            }
            // 引きつけられる機械を選ぶ
            auto n_candidates = 0.0;
            for (auto index = 0; index < 100; index++) {
                assert(board[where[index]].color == attraction_target_num);
                assert(bfs.result[where[index]] % 2 != 0 ||
                       bfs.result[where[index]] == 120);
                if (bfs.result[where[index]] >= 2 &&
                    bfs.result[where[index]] <= kP.kMaxAttractionDistance) {
                    if (uniform_real_distribution<>(0.0, ++n_candidates)(rng) <
                        1.0) {
                        attracted_index = index;
                    }
                }
            }
            if (attracted_index == -1)
                attraction_enabled = false;
        }

        iteration = iter;
        board = initial_board.board;
        where12[0] = initial_board.where[1];
        where12[1] = initial_board.where[2];
        ufs[0] = UnionFind();
        ufs[1] = UnionFind();
        score = 0.0;

        assert(n_moves + n_connections <= K * 100);
        auto steps_erase = array<int, kP.kErase>();
        for (auto& s : steps_erase)
            s = n_moves + n_connections == 0
                    ? -1
                    : uniform_int_distribution<>(0, n_moves + n_connections -
                                                        1)(rng);
        auto nth_move = 0;
        auto empty_indices = array<short, K * 100>(); // 後ろはconnection
        auto n_empty_indices = 0;
        auto n_seen_connections = 0;

        // 変化させる位置を決める
        // これ毎回変えるべきか……？
        auto target_center = Point();
        if (!attraction_enabled) {
            const auto target_num = uniform_int_distribution<>(0, 1)(rng);
            auto mi = 101;
            for (auto trial = 0; trial < kP.kTargetDeterminationTrials;
                 trial++) {
                const auto index = uniform_int_distribution<>(0, 99)(rng);
                const auto siz = ufs[target_num].size(index);
                if (chmin(mi, siz)) {
                    target_center = where12[target_num][index];
                }
            }
        }

        for (auto i = 0; i < K * 100; i++) {
            if (!moves[i].Empty()) {
                for (const auto s : steps_erase) {
                    if (nth_move == s) {
                        moves[i].Reset();
                        n_moves--;
                        break;
                    }
                }
                nth_move++;
            } else if (!connections[i].Empty()) {
                for (const auto s : steps_erase) {
                    if (nth_move == s) {
                        connections[i].Reset();
                        n_connections--;
                        break;
                    }
                }
                nth_move++;
            }

            // 移動不可なら取り除く
            if (!moves[i].Empty() && !Movable(moves[i])) {
                moves[i].Reset();
                n_moves--;
            }

            // メモ
            if (!connections[i].Empty()) {
                empty_indices[100 * K - ++n_seen_connections] = i;
            }

            if (moves[i].Empty() && connections[i].Empty()) {
                // 1/2 でスキップ
                const auto rn = uniform_real_distribution<>()(rng);
                if (rn < kP.kSkipRatio) {
                    empty_indices[n_empty_indices++] = i;
                    continue;
                }

                // 移動を追加
                if (attraction_enabled) {
                    const auto p =
                        where12[attraction_target_num - 1][attracted_index];
                    const auto& distance_board =
                        bfss[attraction_target_num - 1].result;
                    auto min_distance = distance_board[p];
                    auto best_destination = Point(-1, -1);
                    assert(board[p].color == attraction_target_num);

                    const auto r = p.Right();
                    if (r.x < input::N && board[r].color == 0 &&
                        chmin(min_distance, distance_board[r]))
                        best_destination = r;
                    const auto l = p.Left();
                    if (l.x >= 0 && board[l].color == 0 &&
                        chmin(min_distance, distance_board[l]))
                        best_destination = l;
                    const auto d = p.Down();
                    if (d.y < input::N && board[d].color == 0 &&
                        chmin(min_distance, distance_board[d]))
                        best_destination = d;
                    const auto u = p.Up();
                    if (u.y >= 0 && board[u].color == 0 &&
                        chmin(min_distance, distance_board[u]))
                        best_destination = u;

                    if (best_destination.y == -1) {
                        empty_indices[n_empty_indices++] = i;
                    } else {
                        moves[i] = {p, best_destination};
                        n_moves++;
                    }
                } else {
                    auto trial = 0;
                    do {
                        if (trial >= 50) {
                            empty_indices[n_empty_indices++] = i;
                            break;
                        }
                        trial++;
                        const auto rc = uniform_int_distribution<>(0, 1)(rng);
                        if (rc == 0) {
                            // 横
                            // const auto y =
                            //     uniform_int_distribution<>(0, N - 1)(rng);
                            // const auto x =
                            //     uniform_int_distribution<>(0, N - 2)(rng);
                            const auto y =
                                target_center.y +
                                uniform_int_distribution<>(0, kP.kRadius)(rng) -
                                uniform_int_distribution<>(0, kP.kRadius)(rng);
                            if (y < 0 || input::N <= y)
                                continue;
                            const auto x =
                                target_center.x +
                                uniform_int_distribution<>(0, kP.kRadius -
                                                                  1)(rng) -
                                uniform_int_distribution<>(0, kP.kRadius)(rng);
                            if (x < 0 || input::N - 1 <= x)
                                continue;
                            const auto l = Point(y, x);
                            const auto r = Point(y, x + 1);
                            if ((board[l].color == 0) == (board[r].color == 0))
                                continue;
                            if (board[l].color == 0) {
                                // ←
                                moves[i] = {r, l};
                            } else {
                                // →
                                moves[i] = {l, r};
                            }
                        } else {
                            // 縦
                            // const auto x =
                            //     uniform_int_distribution<>(0, N - 1)(rng);
                            // const auto y =
                            //     uniform_int_distribution<>(0, N - 2)(rng);
                            const auto x =
                                target_center.x +
                                uniform_int_distribution<>(0, kP.kRadius)(rng) -
                                uniform_int_distribution<>(0, kP.kRadius)(rng);
                            if (x < 0 || input::N <= x)
                                continue;
                            const auto y =
                                target_center.y +
                                uniform_int_distribution<>(0, kP.kRadius -
                                                                  1)(rng) -
                                uniform_int_distribution<>(0, kP.kRadius)(rng);
                            if (y < 0 || input::N - 1 <= y)
                                continue;
                            const auto u = Point(y, x);
                            const auto d = Point(y + 1, x);
                            if ((board[u].color == 0) == (board[d].color == 0))
                                continue;
                            if (board[u].color == 0) {
                                // ↑
                                moves[i] = {d, u};
                            } else {
                                // ↓
                                moves[i] = {u, d};
                            }
                        }
                        n_moves++;
                        break;
                    } while (true);
                }
            }

            // 盤面を更新
            if (!moves[i].Empty())
                ApplyMove(moves[i]);
        }

        if constexpr (kCheckPerf) {
            const auto t1 = Time();
            t_move += t1 - t0;
            t0 = t1;
        }

        assert(n_seen_connections == n_connections);
        assert(n_moves + n_connections <= K * 100);

        // connection
        // メモ:
        // ↑でconnectionが消されてmoveが追加されない場合、かならずConnectは成功する
        for (auto idx_empty_indices = K * 100 - n_connections;
             idx_empty_indices < K * 100; idx_empty_indices++) {
            const auto i = empty_indices[idx_empty_indices];
            assert(!connections[i].Empty());
            assert(moves[i].Empty());
            if (!Connect(connections[i])) {
                connections[i].Reset();
                n_connections--;
                empty_indices[n_empty_indices++] = i;
            }
        }

        if constexpr (kCheckPerf) {
            const auto t1 = Time();
            t_conn_1 += t1 - t0;
            t0 = t1;
        }

        // 貪欲で繋ぐ
        // メモ: 後から繋ぐので、繋ぐことで他の繋ぎが不可能になることはない
        auto idx_empty_indices = 0;
        for (auto target = 1; target <= 2; target++) {
            auto order = array<signed char, 100>();
            iota(order.begin(), order.end(), 0);
            shuffle(order.begin(), order.end(), rng);
            auto idx_order = 0;
            auto& uf = ufs[target - 1];
            const auto& where = where12[target - 1];

            for (; idx_empty_indices < n_empty_indices; idx_empty_indices++) {
                const auto i = empty_indices[idx_empty_indices];
                assert(moves[i].Empty() && connections[i].Empty());
                while (idx_order < 100) {
                    const auto from = where[order[idx_order++]];
                    // 横
                    for (auto to = Point(from.y, (signed char)(from.x + 1));
                         to.x < input::N; to.x++) {
                        if (board[to].color == target &&
                            !uf.same(board[to].index, board[from].index)) {
                            connections[i] = {from, to};
                            const auto con = Connect(connections[i], true);
                            assert(con);
                            n_connections++;
                            goto connected;
                        } else if (board[to].color != 0) {
                            break;
                        }
                    }
                    // 縦
                    for (auto to = Point((signed char)(from.y + 1), from.x);
                         to.y < input::N; to.y++) {
                        if (board[to].color == target &&
                            !uf.same(board[to].index, board[from].index)) {
                            connections[i] = {from, to};
                            const auto con = Connect(connections[i], true);
                            assert(con);
                            n_connections++;
                            goto connected;
                        } else if (board[to].color != 0) {
                            break;
                        }
                    }
                    // 次の from 候補へ
                }
                // from 候補がなくなった
                break;

            connected:;
            }
        }

        if constexpr (kCheckPerf) {
            const auto t1 = Time();
            t_conn_2 += t1 - t0;
            t0 = t1;

            if (iteration % 50000 == 0) {
                // cerr << "t_move,t_conn_1,t_conn_2=";
                // cerr << t_move << "," << t_conn_1 << "," << t_conn_2 << endl;
            }
        }
    }

    void SanityCheck() const {
        auto n_moves_count = 0;
        for (auto pp : moves) {
            if (!pp.Empty())
                n_moves_count++;
        }
        assert(n_moves == n_moves_count);
    }
};

inline double sigmoid(const double a, const double x) {
    return 1.0 / (1.0 + exp(-a * x));
}

// f: [0, 1] -> [0, 1]
inline double MonotonicallyIncreasingFunction(const double a, const double b,
                                              const double x) {
    if (a == 0.0)
        return x;
    const double x_left = a > 0 ? -b - 0.5 : b - 0.5;
    const double x_right = x_left + 1.0;
    const double left = sigmoid(a, x_left);
    const double right = sigmoid(a, x_right);
    const double y = sigmoid(a, x + x_left);
    return (y - left) /
           (right - left); // left とかが大きい値になると誤差がヤバイ　最悪 0
                           // 除算になる  // b が正なら大丈夫っぽい
}

// f: [0, 1] -> [start, end]
inline double MonotonicFunction(const double start, const double end,
                                const double a, const double b,
                                const double x) {
    return MonotonicallyIncreasingFunction(a, b, x) * (end - start) + start;
}

template <int K, int n_class> inline double Temperature(const double t) {
    static constexpr auto kP = kParams[K][n_class];
    return MonotonicFunction(kP.kAnnealingStart, 0.5, kP.kAnnealingA,
                             kP.kAnnealingB, t);
}

template <int K, int n_class> void SolveN() {

    auto input = Input<K, n_class>();
    input.Read();
    auto state = State<K, n_class>();
    auto bfss = array<BFS<K, n_class>, 2>();

    auto t0 = Time();
    auto iteration = 0;
    static constexpr auto kTimeLimit = 2.9;

    while (true) {
        const auto t = Time() - t0;
        if (t > kTimeLimit)
            break;
        const auto progress_rate = t / kTimeLimit;
        auto updated_state = state;
        updated_state.RandomUpdate(input, iteration, bfss, progress_rate);
        const auto gain = updated_state.score - state.score;
        const auto temperature = Temperature<K, n_class>(progress_rate);
        const auto acceptance_proba = exp(gain / temperature);
        if (uniform_real_distribution<>()(rng) < acceptance_proba) {
            state = updated_state;
        }

        iteration++;
        // if (iteration % 10000 == 0)
        //     state.Print();
    }

    state.Print();
    cerr << iteration << " iterations" << endl;
}

void Solve() {
    cin >> input::N >> input::K;
    switch (input::N) {
        // clang-format off

        #define CASE(n) case n: switch(input::K) { case 2: SolveN<2, NClass<n, 2>()>(); break; case 3: SolveN<3, NClass<n, 3>()>(); break; case 4: SolveN<4, NClass<n, 4>()>(); break; case 5: SolveN<5, NClass<n, 5>()>(); break; } break;
                                                     CASE(15) CASE(16) CASE(17) CASE(18) CASE(19)
        CASE(20) CASE(21) CASE(22) CASE(23) CASE(24) CASE(25) CASE(26) CASE(27) CASE(28) CASE(29)
        CASE(30) CASE(31) CASE(32) CASE(33) CASE(34) CASE(35) CASE(36) CASE(37) CASE(38) CASE(39)
        CASE(40) CASE(41) CASE(42) CASE(43) CASE(44) CASE(45) CASE(46) CASE(47) CASE(48)
        // clang-format on
    }
}

int main() {
    //
    Solve();
}

#ifdef __clang__
#pragma clang attribute pop
#endif

// python3 score_all.py
// flamegraph -- ./a.out < ./tools/in/0011.txt
// g++ main.cpp -Wall -Wextra -std=c++17 -g -O2