// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <bits/stdc++.h>

#ifdef ATCODER
#include <atcoder/all>
#include <ankerl/unordered_dense.h>
using namespace atcoder;
#endif

#ifdef ATCODER
template <typename K, typename V>
using hash_map = ankerl::unordered_dense::map<K, V>;
#else
template <typename K, typename V>
using hash_map = std::unordered_map<K, V>;
#endif

namespace cp_internal {
char char_buff[64];
};

/* namespace */
using namespace std;

/* type */
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;
using uvll = __uint128_t;
template <typename T>
concept RAR = ranges::random_access_range<T>;
template <ranges::range Range>
using rvt = ranges::range_value_t<Range>;

/* ios*/
struct fast_ios {
    fast_ios() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        cout << fixed << setprecision(15);
    }
} fast_ios;

#ifdef ATCODER
// modint出力
template <int m>
istream &operator>>(istream &is, static_modint<m> &x) {
    ll val;
    is >> val;
    x = val;
    return is;
}
// modint入力
template <int m>
ostream &operator<<(ostream &os, const static_modint<m> &x) {
    os << x.val();
    return os;
}
#endif
// pair出力
template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &pr) {
    os << pr.first << ' ' << pr.second;
    return os;
}
// pair入力
template <class T1, class T2>
istream &operator>>(istream &is, pair<T1, T2> &pr) {
    is >> pr.first >> pr.second;
    return is;
}

// tuple出力
template <class Tuple, size_t... I>
void print_tuple_impl(ostream &os, const Tuple &t, index_sequence<I...>) {
    ((os << (I == 0 ? "" : " ") << get<I>(t)), ...);
}
template <class... Ts>
ostream &operator<<(ostream &os, const tuple<Ts...> &t) {
    print_tuple_impl(os, t, index_sequence_for<Ts...>{});
    return os;
}
// tuple入力
template <class Tuple, size_t... I>
void read_tuple_impl(istream &is, Tuple &t, index_sequence<I...>) {
    ((is >> get<I>(t)), ...);
}
template <class... Ts>
istream &operator>>(istream &is, tuple<Ts...> &t) {
    read_tuple_impl(is, t, index_sequence_for<Ts...>{});
    return is;
}
// 128bit整数型出力
template <typename T>
typename enable_if<is_same<T, __int128>::value || is_same<T, __uint128_t>::value, ostream &>::type operator<<(ostream &os, T x) {
    if (x == 0) return os << '0';
    string s;
    if constexpr (is_signed<T>::value) {  // 符号付き
        bool neg = x < 0;
        if (neg) x = -x;
        while (x > 0) {
            s += '0' + x % 10;
            x /= 10;
        }
        if (neg) s += '-';
    } else {  // 符号なし
        while (x > 0) {
            s += '0' + x % 10;
            x /= 10;
        }
    }
    ranges::reverse(s);
    os << s;
    return os;
}
// 128bit整数型入力
template <typename T>
typename enable_if<is_same<T, __int128>::value || is_same<T, __uint128_t>::value, istream &>::type operator>>(istream &is, T &x) {
    string s;
    is >> s;
    x = 0;
    size_t i = 0;
    if constexpr (is_signed<T>::value) {
        bool neg = false;
        if (s[0] == '-') {
            neg = true;
            i = 1;
        }
        for (; i < s.size(); i++) x = x * 10 + (s[i] - '0');
        if (neg) x = -x;
    } else {
        for (; i < s.size(); i++) x = x * 10 + (s[i] - '0');
    }
    return is;
}
// コンテナ入力
template <class T, template <class...> class Container>
istream &operator>>(istream &is, Container<T> &v)
    requires requires(Container<T> c) {
        c.begin();
        c.end();
        c.push_back(T{});
    } && (!is_same_v<Container<T>, string>)
{
    for (auto &x : v) is >> x;
    return is;
}
// コンテナ出力
template <class T>
concept Iterable = requires(T a) {
    begin(a);
    end(a);
} && !is_same_v<T, string>;
template <Iterable T>
void print(const T &v) {
    bool first = true;
    for (auto const &x : v) {
        if (!first) cout << " ";
        first = false;
        cout << x;
    }
    cout << '\n';
}

template <class... T>
void input(T &...a) {
    (cin >> ... >> a);
}
template <class T, class... Ts>
void print(const T &a, const Ts &...b) {
    ((cout << a), ..., (cout << ' ' << b));
    cout << '\n';
}
template <class T>
concept RandomAccessContainer = requires(T a) {
    typename T::value_type;
    { a.begin() } -> random_access_iterator;
    { a.end() } -> random_access_iterator;
};
template <RandomAccessContainer C>
void print_range(const C &v, int l, int r, string sep = " ") {
    int n = v.size();
    l = max(0, l);
    r = min(n, r);
    if (l >= r) {
        cout << '\n';
        return;
    }
    cout << v[l];
    for (int i = l + 1; i < r; i++) {
        cout << sep << v[i];
    }
    cout << '\n';
}

/* math*/
ll powll(ll x, ll n) {
    ll ret = 1;
    while (n) {
        if (n & 1) ret = x * ret;
        x *= x;
        n >>= 1;
    }
    return ret;
}
template <class T>
T divceil(T x, T y) {
    return signbit(x) ^ signbit(y) ? x / y : (x + y - 1) / y;
}
template <class T>
T divfloor(T x, T y) {
    return signbit(x) ^ signbit(y) ? -divceil(abs(x), abs(y)) : x / y;
}
template <class T1, class T2>
T1 smod(T1 x, T2 m) {
    if (x < 0) return (x % m + m) % m;
    return x % m;
}
ll pow_mod32(ll x, ll n, int m) {
    if (m == 1) return 0;
    ll ret = 1;
    smod(x, m);
    while (n) {
        if (n & 1) ret = smod(x * ret, m);
        x = smod(x * x, m);
        n >>= 1;
    }
    return ret;
}
ll pow_mod64(vll x, vll n, ll m) {
    if (m == 1) return 0;
    vll ret = 1;
    smod(x, m);
    while (n) {
        if (n & 1) ret = smod(x * ret, m);
        x = smod(x * x, m);
        n >>= 1;
    }
    return ret;
}
// Find the largest integer x not exceeding m such that x ≡ q (mod p)
ll nearest_px_plus_q_le(ll m, ll p, ll q) {
    return m - smod(m - q, p);
}
// Find the smallest integer x not less than m such that x ≡ q (mod p)
ll nearest_px_plus_q_ge(ll m, ll p, ll q) {
    return m + (p - smod(m - q, p)) % p;
}
// 2点(x1, y1), (x2, y2)を通る直線の方程式ax+by+c=0を求める
tuple<ll, ll, ll> line_from_two_points(ll x1, ll y1, ll x2, ll y2) {
    ll a = y1 - y2;
    ll b = x2 - x1;
    ll c = x1 * y2 - y1 * x2;
    ll g = gcd(gcd(a, b), c);
    a /= g;
    b /= g;
    c /= g;
    if (a < 0 || (a == 0 || b < 0)) {
        a *= -1;
        b *= -1;
        c *= -1;
    }
    return {a, b, c};
}
// 3点(x1, y1), (x2, y2), (x3, y3)が同一直線状にあるか判定する
bool collinear(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3) {
    vll cross = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    return cross == 0;
}

/* algorithms */
template <RAR Range>
ll lower_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(ranges::begin(r), ranges::lower_bound(r, x));
}
template <RAR Range>
ll upper_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(ranges::begin(r), ranges::upper_bound(r, x));
}
template <class A, class B>
    requires requires(A &a, const B &b) {
        { a > b } -> std::convertible_to<bool>;
        { a = b };
    }
bool chmin(A &a, const B &b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}
template <class A, class B>
    requires requires(A &a, const B &b) {
        { a < b } -> std::convertible_to<bool>;
        { a = b };
    }
bool chmax(A &a, const B &b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}
template <RAR Range>
hash_map<rvt<Range>, ll> compression(Range r) {
    ranges::sort(r);
    auto res = ranges::unique(r);
    r.erase(ranges::begin(res), ranges::end(res));
    hash_map<rvt<Range>, ll> ret;
    for (ll i = 0; auto v : r) {
        ret[v] = i++;
    }
    return ret;
}
template <ranges::range Range>
vector<rvt<Range>> accumulate_sum(const Range &r) {
    vector<rvt<Range>> res(ssize(r));
    partial_sum(ranges::begin(r), ranges::end(r), ranges::begin(res));
    return res;
}
template <ranges::range Range>
vector<rvt<Range>> accumulate_sum(const Range &r, const rvt<Range> &init) {
    vector<rvt<Range>> res(ssize(r) + 1);
    res[0] = init;
    inclusive_scan(ranges::begin(r), ranges::end(r), ranges::begin(res) + 1, plus<>(), init);
    return res;
}
template <ranges::range Range>
vector<vector<rvt<Range>>> lpartition(const Range &r, int w) {
    int n = ssize(r);
    int h = (n + w - 1) / w;
    vector<vector<rvt<Range>>> ret(h, vector<rvt<Range>>(w));
    for (int i = 0; i < n; ++i) {
        ret[i / w][i % w] = r[i];
    }
    return ret;
}

/**
 * @brief Run-Length Encoding（RLE）を行う関数
 * @tparam Range  ランダムアクセス可能な Range（vector, string, array など）
 * @param R       入力 Range
 * @return vector<pair<rvt<Range>, long long>>
 *         (値, 連続出現回数) を格納したベクタを返す。
 * @details
 * 隣接する同値要素をまとめて (値, 個数) の形で列挙する。
 * 例: [a, a, b, b, b, c] → {(a,2), (b,3), (c,1)}
 */
template <RAR Range>
auto runlength(const Range &R) {
    vector<pair<rvt<Range>, ll>> ret;
    for (ll l = 0; l < ssize(R);) {
        ll r = l + 1;
        for (; r < ssize(R) && R[l] == R[r]; ++r) {
        }
        ret.emplace_back(R[l], r - l);
        l = r;
    }
    return ret;
}
template <RAR Range>
void remove_duplicates(Range &v) {
    ranges::sort(v);
    auto res = ranges::unique(v);
    v.erase(ranges::begin(res), ranges::end(res));
}
template <ranges::range Range>
auto sum_of(Range R) {
    return reduce(ranges::begin(R), ranges::end(R));
}
template <ranges::range Range, class T>
T sum_of(Range R) {
    return reduce(ranges::begin(R), ranges::end(R), T());
}

/* data_structure */
template <class T>
using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
template <class T>
using priority_queue_max = priority_queue<T, vector<T>>;
template <class T>
auto make_vec(size_t n, const T &val) {
    return vector<T>(n, val);
}
template <class T, class... Args>
auto make_vec(size_t n, Args... args) {
    return vector<decltype(make_vec<T>(args...))>(n, make_vec<T>(args...));
}

/* string */
string dec_to_base(ull x, int base) {
    auto b = begin(cp_internal::char_buff);
    auto [ptr, ec] = to_chars(b, end(cp_internal::char_buff), x, base);
    return string(b, ptr);
}
ull base_to_dec(const string &x, int base) { return stoull(x, 0, base); }
void zfill(string &s, size_t width) { s = string(width > s.size() ? width - s.size() : 0, '0') + s; }

/* grid */
bool grid_check(ll row, ll col, ll h, ll w) { return 0 <= row && row < h && 0 <= col && col < w; }

/* random*/
struct Xorshift128 {
    uint x = 123456789;
    uint y = 362436069;
    uint z = 521288629;
    uint w;
    Xorshift128() {
        random_device rd;
        w = rd();
        if (w == 0) w = 88675123;
    }
    uint operator()() {
        uint t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }
    ull rand64() {
        ull high = ull((*this)()) << 32;
        ull low = ull((*this)());
        return high | low;
    }
    // 完全一様な [l, r] 整数乱数
    uint rand_range(uint l, uint r) {
        uint range = r - l + 1;
        // 2^32 を超えない最大の「range の倍数」
        uint limit = ~uint(0) - (~uint(0) % range);
        while (true) {
            uint x = (*this)();
            if (x <= limit) {
                return l + (x % range);
            }
        }
    }
    // 完全一様な [l, r] の 64bit 整数
    ull rand_range64(ull l, ull r) {
        ull range = r - l + 1;
        ull limit = ~ull(0) - (~ull(0) % range);
        while (true) {
            ull x = rand64();
            if (x <= limit) {
                return l + (x % range);
            }
        }
    };
};

/* util */
#define OL_REP(_1, _2, _3, _4, name, ...) name
#define REP2(i, n) for (ll i = 0; i < ll(n); ++(i))
#define REP3(i, a, b) for (ll i = ll(a); i < ll(b); ++(i))
#define REP4(i, a, b, c) for (ll i = ll(a); i < ll(b); i += c)
#define rep(...) OL_REP(__VA_ARGS__, REP4, REP3, REP2)(__VA_ARGS__)
#define RREP2(i, n) for (ll i = ll(n); i >= 0; --i)
#define RREP3(i, a, b) for (ll i = ll(a); i > ll(b); --(i))
#define RREP4(i, a, b, c) for (ll i = ll(a); i > ll(b); i -= c)
#define rrep(...) OL_REP(__VA_ARGS__, RREP4, RREP3, RREP2)(__VA_ARGS__)

/* constants */
//[[maybe_unused]] constexpr ll MOD = 1e9 + 7;
[[maybe_unused]] constexpr ll MOD = 998244353;
[[maybe_unused]] constexpr int INF32 = (1 << 30) - 1;
[[maybe_unused]] constexpr ll INF64 = (1LL << 62) - 1;
[[maybe_unused]] const double PI = numbers::pi;

// clang-format on
#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <vector>

/**
 * @brief Slope Trick — 区分線形凸関数を効率的に管理するデータ構造
 *
 * 関数 f(x) を「最小値」と「左右の傾きの変化点（breakpoint）」で表現する。
 *   - L (max-heap): 傾きが減少する変化点群（左側、f が右下がりの区間）
 *   - R (min-heap): 傾きが増加する変化点群（右側、f が右上がりの区間）
 *
 * 遅延加算 (add_l, add_r) により、全変化点の一括シフトを O(1) で実現する。
 *
 * @see https://maspypy.com/slope-trick-1-%E8%A7%A3%E8%AA%AC%E7%B7%A8
 */
template <typename T>
struct SlopeTrick {
    static constexpr T INF = std::numeric_limits<T>::max() / 3;

    SlopeTrick() = default;

    // ──────────────────────────── クエリ ────────────────────────────

    struct Minimum {
        T left;   // min f を達成する区間の左端
        T right;  // min f を達成する区間の右端
        T value;  // min f の値
    };

    /// f の最小値と、それを達成する区間 [left, right] を返す
    Minimum query() const { return {top_left(), top_right(), min_f_}; }

    // ──────────────────── 関数の加算・変形操作 ────────────────────

    /// f(x) += constant
    void add_constant(T c) { min_f_ += c; }

    /// f(x) += max(a - x, 0)    （左下がり: ＼＿ の形）
    void add_left_ramp(T a) {
        min_f_ += std::max(T(0), a - top_right());
        push_right(a);
        push_left(pop_right());
    }

    /// f(x) += max(x - a, 0)    （右上がり: ＿／ の形）
    void add_right_ramp(T a) {
        min_f_ += std::max(T(0), top_left() - a);
        push_left(a);
        push_right(pop_left());
    }

    /// f(x) += |x - a|          （V字: ＼／ の形）
    void add_abs(T a) {
        add_left_ramp(a);
        add_right_ramp(a);
    }

    // ──────────────── 累積 min による平坦化 ────────────────

    /// f(x) = min_{y <= x} f(y)  （右側を平坦化: ＼＿ の形にする）
    void prefix_min() {
        while (!right_heap_.empty()) right_heap_.pop();
    }

    /// f(x) = min_{y >= x} f(y)  （左側を平坦化: ＿／ の形にする）
    void suffix_min() {
        while (!left_heap_.empty()) left_heap_.pop();
    }

    // ──────────────────── スライド min ────────────────────

    /// f(x) = min_{x-b <= y <= x-a} f(y)   (a <= b)
    /// 左の変化点を a、右の変化点を b だけシフトする
    void slide_min(T a, T b) {
        assert(a <= b);
        lazy_left_ += a;
        lazy_right_ += b;
    }

    /// f(x) = f(x - a)   （グラフ全体を右に a だけ平行移動）
    void translate(T a) { slide_min(a, a); }

    // ──────────────── 点 x での値の取得（破壊的） ────────────────

    /// f(x) を計算して返す。内部状態（ヒープ）は破壊される。
    T evaluate_destructive(T x) {
        T result = min_f_;
        while (!left_heap_.empty()) result += std::max(T(0), pop_left() - x);
        while (!right_heap_.empty()) result += std::max(T(0), x - pop_right());
        return result;
    }

    // ──────────────── 他の SlopeTrick との合成 ────────────────

    /// this に other を加算する（other は破壊される）
    void merge(SlopeTrick &other) {
        // 小さい方を大きい方にマージする（マージテク）
        if (other.size() > size()) {
            std::swap(left_heap_, other.left_heap_);
            std::swap(right_heap_, other.right_heap_);
            std::swap(lazy_left_, other.lazy_left_);
            std::swap(lazy_right_, other.lazy_right_);
            std::swap(min_f_, other.min_f_);
        }
        while (!other.right_heap_.empty()) add_right_ramp(other.pop_right());
        while (!other.left_heap_.empty()) add_left_ramp(other.pop_left());
        min_f_ += other.min_f_;
    }

   private:
    T min_f_ = 0;       // f の最小値
    T lazy_left_ = 0;   // 左ヒープへの遅延加算
    T lazy_right_ = 0;  // 右ヒープへの遅延加算

    std::priority_queue<T, std::vector<T>, std::less<>> left_heap_;      // max-heap
    std::priority_queue<T, std::vector<T>, std::greater<>> right_heap_;  // min-heap

    // ── 右ヒープ操作（遅延加算を考慮） ──

    void push_right(T val) { right_heap_.push(val - lazy_right_); }

    T top_right() const { return right_heap_.empty() ? INF : right_heap_.top() + lazy_right_; }

    T pop_right() {
        T val = top_right();
        if (!right_heap_.empty()) right_heap_.pop();
        return val;
    }

    // ── 左ヒープ操作（遅延加算を考慮） ──

    void push_left(T val) { left_heap_.push(val - lazy_left_); }

    T top_left() const { return left_heap_.empty() ? -INF : left_heap_.top() + lazy_left_; }

    T pop_left() {
        T val = top_left();
        if (!left_heap_.empty()) left_heap_.pop();
        return val;
    }

    // ── ユーティリティ ──

    std::size_t size() const { return left_heap_.size() + right_heap_.size(); }
};

int main() {
    int Q;
    input(Q);
    ll q, a, b;
    SlopeTrick<ll> st;
    rep(i, Q) {
        input(q);
        if (q == 1) {
            input(a, b);
            st.add_abs(a);
            st.add_constant(b);
        } else {
            auto [l,r,fmin] = st.query();
            print(l,fmin);
        }
    }
}