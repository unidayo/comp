// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <bits/stdc++.h>
using namespace std;

#ifdef ATCODER
#include <atcoder/all>
#include <ankerl/unordered_dense.h>
using namespace atcoder;
#endif

#ifdef ATCODER
template <typename K, typename V> using hash_map = ankerl::unordered_dense::map<K, V>;
template <typename K> using hash_set = ankerl::unordered_dense::set<K>;
#else
template <typename K, typename V> using hash_map = unordered_map<K, V>;
template <typename K> using hash_set = unordered_set<K>;
#endif

namespace cp_internal {
char char_buff[64];
template <ranges::range Range, class T>
auto sorted_lower_bound(const Range &r, const T &x) {
    if constexpr (requires { r.lower_bound(x); }) return r.lower_bound(x);
    else return ranges::lower_bound(r, x);
}
template <ranges::range Range, class T>
auto sorted_upper_bound(const Range &r, const T &x) {
    if constexpr (requires { r.upper_bound(x); }) return r.upper_bound(x);
    else return ranges::upper_bound(r, x);
}
};

/* type */
using uint = unsigned int;
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;
using uvll = __uint128_t;
template <typename T> concept RAR = ranges::random_access_range<T>;
template <ranges::range Range> using rvt = ranges::range_value_t<Range>;
/* ios*/
struct fast_ios {
    fast_ios() { ios::sync_with_stdio(false); cin.tie(nullptr); cout << fixed << setprecision(15); }
} fast_ios;

#ifdef ATCODER
template <int m>
istream &operator>>(istream &is, static_modint<m> &x) {
    ll val; is >> val; x = val;
    return is;
}
template <int m>
ostream &operator<<(ostream &os, const static_modint<m> &x) { return os << x.val(); }
#endif
template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &pr) { return os << pr.first << ' ' << pr.second; }
template <class T1, class T2>
istream &operator>>(istream &is, pair<T1, T2> &pr) { return is >> pr.first >> pr.second; }
template <class Tuple, size_t... I>
void print_tuple_impl(ostream &os, const Tuple &t, index_sequence<I...>) {
    ((os << (I == 0 ? "" : " ") << get<I>(t)), ...);
}
template <class... Ts>
ostream &operator<<(ostream &os, const tuple<Ts...> &t) {
    print_tuple_impl(os, t, index_sequence_for<Ts...>{}); return os;
}
template <class Tuple, size_t... I>
void read_tuple_impl(istream &is, Tuple &t, index_sequence<I...>) {
    ((is >> get<I>(t)), ...);
}
template <class... Ts>
istream &operator>>(istream &is, tuple<Ts...> &t) {
    read_tuple_impl(is, t, index_sequence_for<Ts...>{}); return is;
}
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
template <class T, template <class...> class Container>
istream &operator>>(istream &is, Container<T> &v)
    requires requires(Container<T> c) { c.begin(); c.end(); c.push_back(T{}); } && (!is_same_v<Container<T>, string>)
{
    for (auto &x : v) is >> x;
    return is;
}
template <class... T>
void input(T &...a) { (cin >> ... >> a); }
template <class T>
concept Iterable = requires(T a) { begin(a); end(a); } && !is_same_v<T, string>;
template <class T>
concept NestedIterable = Iterable<T> && Iterable<rvt<T>>;
template <class T>
void print_value(const T &v) {
    if constexpr (Iterable<T>) {
        bool first = true;
        for (auto const &x : v) {
            if (!first) cout << ' ';
            first = false;
            print_value(x);
        }
    } else {
        cout << v;
    }
}
template <NestedIterable T>
void print(const T &v) {
    bool first = true;
    for (auto const &row : v) {
        if (!first) cout << '\n';
        first = false;
        print_value(row);
    }
    cout << '\n';
}
template <class... Ts>
void print(const Ts &...a) {
    bool first = true;
    auto print_one = [&](const auto &x) {
        if (!first) cout << ' ';
        first = false;
        print_value(x);
    };
    (print_one(a), ...);
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
// x^n を整数で返す
ll powll(ll x, ll n) {
    ll ret = 1;
    while (n) {
        if (n & 1) ret = x * ret;
        x *= x;
        n >>= 1;
    }
    return ret;
}
// x/y の切り上げ
template <class T>
T divceil(T x, T y) {
    auto d = x / y, r = x % y;
    return d + (r != 0 && ((r ^ y) > 0));
}
// x/y の切り捨て
template <class T>
T divfloor(T x, T y) {
    auto d = x / y, r = x % y;
    return d - (r != 0 && ((r ^ y) < 0));
}
// x % m を常に非負で返す
template <class T1, class T2>
T1 smod(T1 x, T2 m) {
    return x < 0 ? (x % m + m) % m : x % m;
}
// x^n mod m (m が int範囲、中間積がオーバーフローしない場合)
ll pow_mod32(ll x, ll n, int m) {
    if (m == 1) return 0;
    ll ret = 1;
    x = smod(x, m);
    while (n) {
        if (n & 1) ret = smod(x * ret, m);
        x = smod(x * x, m);
        n >>= 1;
    }
    return ret;
}
// x^n mod m (m が ll範囲、__int128で中間積を計算)
ll pow_mod64(vll x, vll n, ll m) {
    if (m == 1) return 0;
    vll ret = 1;
    x = smod(x, m);
    while (n) {
        if (n & 1) ret = smod(x * ret, m);
        x = smod(x * x, m);
        n >>= 1;
    }
    return ret;
}
// m以下の最大の整数x で x ≡ q (mod p) を返す
ll nearest_px_plus_q_le(ll m, ll p, ll q) { return m - smod(m - q, p); }
// m以上の最小の整数x で x ≡ q (mod p) を返す
ll nearest_px_plus_q_ge(ll m, ll p, ll q) { return m + (p - smod(m - q, p)) % p; }
// 2点を通る直線 ax+by+c=0 を既約整数の {a,b,c} で返す
tuple<ll, ll, ll> line_from_two_points(ll x1, ll y1, ll x2, ll y2) {
    ll a = y1 - y2;
    ll b = x2 - x1;
    ll c = x1 * y2 - y1 * x2;
    ll g = gcd(gcd(a, b), c);
    a /= g;
    b /= g;
    c /= g;
    if (a < 0 || (a == 0 && b < 0)) {
        a *= -1;
        b *= -1;
        c *= -1;
    }
    return {a, b, c};
}
// 3点が同一直線上にあるか判定
bool collinear(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3) {
    return (x2 - x1) * vll(y3 - y1) - (y2 - y1) * vll(x3 - x1) == 0;
}
/* algorithms */
// ソート済みrにおけるx未満の要素数
template <RAR Range>
ll lower_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(ranges::begin(r), ranges::lower_bound(r, x));
}
// ソート済みrにおけるx以下の要素数
template <RAR Range>
ll upper_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(ranges::begin(r), ranges::upper_bound(r, x));
}
// ソート済みrにおけるx以下の最大値。存在しなければnullopt
template <ranges::bidirectional_range Range, class T>
optional<rvt<Range>> max_le(const Range &r, const T &x) {
    auto it = cp_internal::sorted_upper_bound(r, x);
    if (it == ranges::begin(r)) return nullopt;
    --it;
    return *it;
}
// ソート済みrにおけるx以上の最小値。存在しなければnullopt
template <ranges::range Range, class T>
optional<rvt<Range>> min_ge(const Range &r, const T &x) {
    auto it = cp_internal::sorted_lower_bound(r, x);
    if (it == ranges::end(r)) return nullopt;
    return *it;
}
template <class A, class B>
    requires requires(A &a, const B &b) {
        { a > b } -> std::convertible_to<bool>;
        { a = b };
    }
bool chmin(A &a, const B &b) {
    if (a > b) return a = b, true;
    return false;
}
template <class A, class B>
    requires requires(A &a, const B &b) {
        { a < b } -> std::convertible_to<bool>;
        { a = b };
    }
bool chmax(A &a, const B &b) {
    if (a < b) return a = b, true;
    return false;
}
// 座標圧縮。{元の値 -> 0始まりの番号} のhash_mapを返す
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
// 累積和。res[i] = r[0]+...+r[i] (長さはrと同じ)
// res[0]=init, res[i+1]=res[i]+r[i] (長さはr+1)
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
// RLE。{値, 連続個数} のvectorを返す。例: "aaabbc" -> {('a',3),('b',2),('c',1)}
template <RAR Range>
auto runlength(const Range &R) {
    vector<pair<rvt<Range>, ll>> ret;
    for (ll l = 0; l < ssize(R);) {
        ll r = l + 1;
        for (; r < ssize(R) && R[l] == R[r]; ++r);
        ret.emplace_back(R[l], r - l);
        l = r;
    }
    return ret;
}
// vをソートして重複を除去 (in-place)
template <RAR Range>
void remove_duplicates(Range &v) {
    ranges::sort(v);
    auto res = ranges::unique(v);
    v.erase(ranges::begin(res), ranges::end(res));
}
// Rの総和を返す
// 型Tで総和を返す (オーバーフロー対策などに)
template <ranges::range Range>
auto sum_of(const Range &R) { return reduce(ranges::begin(R), ranges::end(R)); }
template <ranges::range Range, class T>
T sum_of(const Range &R) { return reduce(ranges::begin(R), ranges::end(R), T()); }
/* data_structure */
template <class T> using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
template <class T> using priority_queue_max = priority_queue<T, vector<T>>;
template <class T>
auto make_vec(size_t n, const T &val) { return vector<T>(n, val); }
template <class T, class... Args>
auto make_vec(size_t n, Args... args) {
    return vector<decltype(make_vec<T>(args...))>(n, make_vec<T>(args...));
}
/* string */
// 非負整数xをbase進数文字列に変換 (base=2〜36)
string dec_to_base(ull x, int base) {
    auto b = begin(cp_internal::char_buff);
    auto [ptr, ec] = to_chars(b, end(cp_internal::char_buff), x, base);
    return string(b, ptr);
}
// base進数文字列xを10進数に変換
ull base_to_dec(const string &x, int base) { return stoull(x, 0, base); }
// 文字列sをwidthの幅になるよう先頭を'0'で埋める
void zfill(string &s, size_t width) { s = string(width > s.size() ? width - s.size() : 0, '0') + s; }

/* grid */
// d=0:右, 1:下, 2:左, 3:上 の移動量
[[maybe_unused]] constexpr int dr4[] = {0, 1, 0, -1};
[[maybe_unused]] constexpr int dc4[] = {1, 0, -1, 0};
// d=0から時計回り: 右, 右下, 下, 左下, 左, 左上, 上, 右上
[[maybe_unused]] constexpr int dr8[] = {0, 1, 1, 1, 0, -1, -1, -1};
[[maybe_unused]] constexpr int dc8[] = {1, 1, 0, -1, -1, -1, 0, 1};
// グリッド(h行w列)の範囲内かチェック
bool grid_check(ll row, ll col, ll h, ll w) { return 0 <= row && row < h && 0 <= col && col < w; }

// 2次元コンテナを時計回りに90度回転したものを返す。
// vector<string>, vector<vector<T>> など、Grid[row][col] でアクセスできる矩形を想定。
template <class Grid>
Grid rotate90_clockwise(const Grid &a) {
    if (a.empty() || a[0].empty()) return Grid{};
    using Row = ranges::range_value_t<Grid>;
    using Cell = ranges::range_value_t<Row>;
    const int h = int(a.size());
    const int w = int(a[0].size());
    Grid b(w, Row(h, Cell{}));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            b[j][h - 1 - i] = a[i][j];
        }
    }
    return b;
}

// 2次元コンテナを反時計回りに90度回転したものを返す。
template <class Grid>
Grid rotate90_counterclockwise(const Grid &a) {
    if (a.empty() || a[0].empty()) return Grid{};
    using Row = ranges::range_value_t<Grid>;
    using Cell = ranges::range_value_t<Row>;
    const int h = int(a.size());
    const int w = int(a[0].size());
    Grid b(w, Row(h, Cell{}));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            b[w - 1 - j][i] = a[i][j];
        }
    }
    return b;
}

template <class Grid>
Grid rotate90(const Grid &a) {
    return rotate90_clockwise(a);
}
/* bit */
// xの立っているビット数
int popcnt(ull x) { return popcount(x); }
// xの最上位ビット位置 (0-indexed, x>0)
int msb(ull x) { return 63 - countl_zero(x); }
// xの最下位ビット位置 (0-indexed, x>0)
int lsb(ull x) { return countr_zero(x); }

/* random */
struct Xoshiro256 {
    ull s[4]{};
    explicit Xoshiro256(ull seed = random_device{}()) { reseed(seed); }
    static ull splitmix64(ull x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }
    static ull rotl(ull x, int k) { return (x << k) | (x >> (64 - k)); }
    void reseed(ull seed) {
        for (int i = 0; i < 4; ++i) {
            seed = splitmix64(seed);
            s[i] = seed;
        }
        if ((s[0] | s[1] | s[2] | s[3]) == 0) s[0] = 1;
    }
    ull rand64() {
        const ull result = rotl(s[1] * 5, 7) * 9;
        const ull t = s[1] << 17;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], 45);
        return result;
    }
    uint rand32() { return uint(rand64() >> 32); }
    // [0, range)
    ull rand_range_exclusive(ull range) {
        assert(range > 0);
        ull threshold = -range % range;
        while (true) {
            ull x = rand64();
            if (x >= threshold) return x % range;
        }
    }
    // [l, r]
    uint rand_range(uint l, uint r) {
        assert(l <= r);
        return l + uint(rand_range_exclusive(ull(r) - ull(l) + 1));
    }
    // [l, r]
    ull rand_range64(ull l, ull r) {
        assert(l <= r);
        ull range = r - l + 1;
        if (range == 0) return rand64();
        return l + rand_range_exclusive(range);
    }
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
// Dynamic Wavelet Matrix。
//
// 使い方:
//   DynamicWaveletMatrix<long long> wm(a);  // 負の値を含む signed 整数にも対応
//
//   wm.access(k);                 // a[k]
//   wm.set(k, x);                 // a[k] = x
//   wm.insert(k, x);              // k 番目の直前に x を挿入
//   wm.erase(k);                  // k 番目を削除して値を返す。なければ nullopt
//   wm.rank(r, x);                // [0, r) にある x の個数
//   wm.rank(l, r, x);             // [l, r) にある x の個数
//   wm.kth_smallest(l, r, k);     // [l, r) の k 番目に小さい値（0-indexed）
//   wm.kth_largest(l, r, k);      // [l, r) の k 番目に大きい値（0-indexed）
//   wm.range_freq(l, r, upper);   // [l, r) にある upper 未満の値の個数
//   wm.range_freq(l, r, lower, upper);  // [l, r) にある [lower, upper) の値の個数
//   wm.prev_value(l, r, upper);   // [l, r) にある upper 未満の最大値。なければ nullopt
//   wm.next_value(l, r, lower);   // [l, r) にある lower 以上の最小値。なければ nullopt
//
// Dynamic Range Count-Distinct:
//   DynamicRangeCountDistinct<long long> cd(a);
//   cd.set(k, x);
//   cd.insert(k, x);
//   cd.erase(k);
//   cd.count(l, r);               // [l, r) にある相異なる値の個数
//   cd.kth_smallest(l, r, k);     // 通常の Dynamic Wavelet Matrix 機能も使える
//
// 注意:
//   - T は整数型を想定。
//   - デフォルトの LOG では signed の負の値にも対応し、大小順も正しく扱う。
//     内部では符号ビットを反転して unsigned として並べる。
//   - signed 型で LOG を小さく指定する場合は、値が非負で 2^LOG 未満に収まる用途を想定。
//   - 各階層を bit-packed block implicit treap の動的 bitvector で持つ。
//   - 構築 O(N log A) 程度、各操作 O(log A (log N + B))。B=256。

template <class T, int LOG = numeric_limits<make_unsigned_t<T>>::digits>
class DynamicWaveletMatrix {
    static_assert(is_integral_v<T>, "DynamicWaveletMatrix<T>: T must be an integral type");

   private:
    using U = make_unsigned_t<T>;
    static constexpr int FULL_LOG = numeric_limits<U>::digits;

    class DynamicBitVector {
        struct Node {
            Node *l, *r;
            unsigned priority;
            int cnt, sum;
            int len, ones;
            std::array<unsigned long long, 4> bits;
        };
        using Tree = Node *;

        static constexpr int BLOCK_BITS = 256;
        static constexpr int WORDS = BLOCK_BITS / 64;
        static constexpr int CHUNK_SIZE = 1 << 20;
        inline static vector<Node *> chunks;
        inline static int chunk_pos = CHUNK_SIZE;

        Tree root = nullptr;
        unsigned rng_state = 123456789;

        unsigned rng() {
            unsigned x = rng_state;
            x ^= x << 13;
            x ^= x >> 17;
            x ^= x << 5;
            return rng_state = x;
        }

        static int popcount64(unsigned long long x) { return __builtin_popcountll(x); }

        static bool get_block_bit(const Node *p, int k) { return (p->bits[k >> 6] >> (k & 63)) & 1ULL; }

        static void set_block_bit(Node *p, int k, bool bit) {
            unsigned long long mask = 1ULL << (k & 63);
            if (bit)
                p->bits[k >> 6] |= mask;
            else
                p->bits[k >> 6] &= ~mask;
        }

        static int block_rank1(const Node *p, int k) {
            int full = k >> 6;
            int rem = k & 63;
            int res = 0;
            for (int i = 0; i < full; i++) res += popcount64(p->bits[i]);
            if (rem) res += popcount64(p->bits[full] & ((1ULL << rem) - 1));
            return res;
        }

        static void recompute_block(Node *p) {
            p->ones = 0;
            for (int i = 0; i < WORDS; i++) p->ones += popcount64(p->bits[i]);
        }

        static void insert_block_bit(Node *p, int k, bool bit) {
            assert(0 <= k && k <= p->len && p->len < BLOCK_BITS);
            for (int i = p->len; i > k; i--) set_block_bit(p, i, get_block_bit(p, i - 1));
            set_block_bit(p, k, bit);
            p->len++;
            if (bit) p->ones++;
        }

        static bool erase_block_bit(Node *p, int k) {
            assert(0 <= k && k < p->len);
            bool res = get_block_bit(p, k);
            for (int i = k; i + 1 < p->len; i++) set_block_bit(p, i, get_block_bit(p, i + 1));
            p->len--;
            set_block_bit(p, p->len, false);
            if (res) p->ones--;
            return res;
        }

        Node *new_node() {
            if (chunk_pos == CHUNK_SIZE) {
                chunks.push_back(new Node[CHUNK_SIZE]);
                chunk_pos = 0;
            }
            Node *p = &chunks.back()[chunk_pos++];
            *p = {nullptr, nullptr, rng(), 0, 0, 0, 0, {}};
            return p;
        }

        Node *new_block_from(const vector<bool> &v, int l, int r) {
            Node *p = new_node();
            p->len = r - l;
            for (int i = l; i < r; i++) {
                if (v[i]) set_block_bit(p, i - l, true);
            }
            recompute_block(p);
            return update(p);
        }

        Node *new_single(bool bit) {
            Node *p = new_node();
            p->len = 1;
            if (bit) {
                p->bits[0] = 1;
                p->ones = 1;
            }
            return update(p);
        }

        int count(Tree t) const { return t ? t->cnt : 0; }
        int sum(Tree t) const { return t ? t->sum : 0; }

        Tree update(Tree t) {
            if (!t) return nullptr;
            t->cnt = count(t->l) + t->len + count(t->r);
            t->sum = sum(t->l) + t->ones + sum(t->r);
            return t;
        }

        Tree merge(Tree l, Tree r) {
            if (!l) return r;
            if (!r) return l;
            if (l->priority > r->priority) {
                l->r = merge(l->r, r);
                return update(l);
            }
            r->l = merge(l, r->l);
            return update(r);
        }

        Tree rotate_right(Tree t) {
            Tree s = t->l;
            t->l = s->r;
            s->r = update(t);
            return update(s);
        }

        Tree rotate_left(Tree t) {
            Tree s = t->r;
            t->r = s->l;
            s->l = update(t);
            return update(s);
        }

        Tree insert_rec(Tree t, int k, bool bit) {
            if (!t) return new_single(bit);
            int lc = count(t->l);
            if (k < lc) {
                t->l = insert_rec(t->l, k, bit);
                t = update(t);
                if (t->l && t->l->priority > t->priority) t = rotate_right(t);
                return t;
            }
            if (k > lc + t->len) {
                t->r = insert_rec(t->r, k - lc - t->len, bit);
                t = update(t);
                if (t->r && t->r->priority > t->priority) t = rotate_left(t);
                return t;
            }
            if (t->len < BLOCK_BITS) {
                insert_block_bit(t, k - lc, bit);
                return update(t);
            }
            vector<bool> v(t->len + 1);
            int pos = k - lc;
            for (int i = 0; i < pos; i++) v[i] = get_block_bit(t, i);
            v[pos] = bit;
            for (int i = pos; i < t->len; i++) v[i + 1] = get_block_bit(t, i);
            int mid = (int)v.size() >> 1;
            Tree l = t->l;
            Tree r = t->r;
            Tree a = new_block_from(v, 0, mid);
            Tree b = new_block_from(v, mid, (int)v.size());
            t->l = t->r = nullptr;
            Tree m = merge(a, b);
            return merge(merge(l, m), r);
        }

        pair<Tree, bool> erase_rec(Tree t, int k) {
            int lc = count(t->l);
            if (k < lc) {
                auto [nl, res] = erase_rec(t->l, k);
                t->l = nl;
                return {update(t), res};
            }
            if (k >= lc + t->len) {
                auto [nr, res] = erase_rec(t->r, k - lc - t->len);
                t->r = nr;
                return {update(t), res};
            }
            bool res = erase_block_bit(t, k - lc);
            if (t->len == 0) return {merge(t->l, t->r), res};
            return {update(t), res};
        }

       public:
        DynamicBitVector() = default;

        int size() const { return count(root); }
        int count1() const { return sum(root); }
        int count0() const { return size() - count1(); }

        bool get(int k) const {
            assert(0 <= k && k < size());
            Tree t = root;
            while (true) {
                int lc = count(t->l);
                if (k < lc) {
                    t = t->l;
                } else if (k < lc + t->len) {
                    return get_block_bit(t, k - lc);
                } else {
                    k -= lc + t->len;
                    t = t->r;
                }
            }
        }

        int rank1(int k) const {
            assert(0 <= k && k <= size());
            int res = 0;
            Tree t = root;
            while (t) {
                int lc = count(t->l);
                if (k <= lc) {
                    t = t->l;
                } else if (k <= lc + t->len) {
                    res += sum(t->l);
                    res += block_rank1(t, k - lc);
                    break;
                } else {
                    res += sum(t->l);
                    res += t->ones;
                    k -= lc + t->len;
                    t = t->r;
                }
            }
            return res;
        }

        int rank0(int k) const {
            assert(0 <= k && k <= size());
            return k - rank1(k);
        }

        void insert(int k, bool bit) {
            assert(0 <= k && k <= size());
            root = insert_rec(root, k, bit);
        }

        optional<bool> erase(int k) {
            if (k < 0 || k >= size()) return nullopt;
            auto [nr, res] = erase_rec(root, k);
            root = nr;
            return res;
        }

        void build(const vector<bool> &v) {
            root = nullptr;
            for (int l = 0; l < (int)v.size(); l += BLOCK_BITS) {
                int r = min<int>(l + BLOCK_BITS, v.size());
                root = merge(root, new_block_from(v, l, r));
            }
        }
    };

    int n = 0;
    array<DynamicBitVector, LOG> matrix;
    array<int, LOG> zeros{};

    static U sign_mask() {
        if constexpr (FULL_LOG == 0) return U(0);
        return U(1) << (FULL_LOG - 1);
    }

    static U encode(T x) {
        U u = static_cast<U>(x);
        if constexpr (is_signed_v<T> && LOG == FULL_LOG) {
            return u ^ sign_mask();
        } else {
            return u;
        }
    }

    static T decode(U x) {
        if constexpr (is_signed_v<T> && LOG == FULL_LOG) {
            x ^= sign_mask();
        }
        return static_cast<T>(x);
    }

    static bool bit_at(U x, int b) { return (x >> b) & U(1); }

    int range_freq_less_encoded(int l, int r, U upper) const {
        int res = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            int l0 = matrix[level].rank0(l);
            int r0 = matrix[level].rank0(r);
            if (bit_at(upper, b)) {
                res += r0 - l0;
                l = zeros[level] + l - l0;
                r = zeros[level] + r - r0;
            } else {
                l = l0;
                r = r0;
            }
        }
        return res;
    }

   public:
    DynamicWaveletMatrix() = default;
    explicit DynamicWaveletMatrix(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        *this = DynamicWaveletMatrix();
        n = (int)v.size();
        vector<U> cur(n), nxt(n);
        for (int i = 0; i < n; i++) cur[i] = encode(v[i]);
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            vector<bool> bits(n);
            int zero_count = 0;
            for (int i = 0; i < n; i++) {
                bits[i] = bit_at(cur[i], b);
                if (!bits[i]) zero_count++;
            }
            matrix[level].build(bits);
            zeros[level] = zero_count;
            int zi = 0, oi = zero_count;
            for (int i = 0; i < n; i++) {
                if (bits[i])
                    nxt[oi++] = cur[i];
                else
                    nxt[zi++] = cur[i];
            }
            cur.swap(nxt);
        }
    }

    int size() const { return n; }
    bool empty() const { return n == 0; }

    T access(int k) const {
        assert(0 <= k && k < n);
        U x = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            bool bit = matrix[level].get(k);
            if (bit) {
                x |= U(1) << b;
                k = zeros[level] + matrix[level].rank1(k);
            } else {
                k = matrix[level].rank0(k);
            }
        }
        return decode(x);
    }

    void insert(int k, T value) {
        assert(0 <= k && k <= n);
        U x = encode(value);
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            bool bit = bit_at(x, b);
            int next_k;
            if (bit) {
                next_k = zeros[level] + matrix[level].rank1(k);
            } else {
                next_k = matrix[level].rank0(k);
                zeros[level]++;
            }
            matrix[level].insert(k, bit);
            k = next_k;
        }
        n++;
    }

    optional<T> erase(int k) {
        if (k < 0 || k >= n) return nullopt;
        U x = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            bool bit = matrix[level].get(k);
            int next_k;
            if (bit) {
                x |= U(1) << b;
                next_k = zeros[level] + matrix[level].rank1(k);
            } else {
                next_k = matrix[level].rank0(k);
                zeros[level]--;
            }
            matrix[level].erase(k);
            k = next_k;
        }
        n--;
        return decode(x);
    }

    void set(int k, T value) {
        assert(0 <= k && k < n);
        erase(k);
        insert(k, value);
    }

    int rank(int r, T x) const {
        assert(0 <= r && r <= n);
        return rank(0, r, x);
    }

    int rank(int l, int r, T x) const {
        assert(0 <= l && l <= r && r <= n);
        U y = encode(x);
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            int l0 = matrix[level].rank0(l);
            int r0 = matrix[level].rank0(r);
            if (bit_at(y, b)) {
                l = zeros[level] + l - l0;
                r = zeros[level] + r - r0;
            } else {
                l = l0;
                r = r0;
            }
        }
        return r - l;
    }

    T kth_smallest(int l, int r, int k) const {
        assert(0 <= l && l <= r && r <= n);
        assert(0 <= k && k < r - l);
        U x = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            int l0 = matrix[level].rank0(l);
            int r0 = matrix[level].rank0(r);
            int zero_count = r0 - l0;
            if (k < zero_count) {
                l = l0;
                r = r0;
            } else {
                x |= U(1) << b;
                k -= zero_count;
                l = zeros[level] + l - l0;
                r = zeros[level] + r - r0;
            }
        }
        return decode(x);
    }

    T kth_largest(int l, int r, int k) const {
        assert(0 <= l && l <= r && r <= n);
        assert(0 <= k && k < r - l);
        return kth_smallest(l, r, r - l - 1 - k);
    }

    int range_freq(int l, int r, T upper) const {
        assert(0 <= l && l <= r && r <= n);
        return range_freq_less_encoded(l, r, encode(upper));
    }

    int range_freq(int l, int r, T lower, T upper) const {
        assert(0 <= l && l <= r && r <= n);
        if (!(lower < upper)) return 0;
        return range_freq(l, r, upper) - range_freq(l, r, lower);
    }

    optional<T> prev_value(int l, int r, T upper) const {
        assert(0 <= l && l <= r && r <= n);
        int cnt = range_freq(l, r, upper);
        if (cnt == 0) return nullopt;
        return kth_smallest(l, r, cnt - 1);
    }

    optional<T> next_value(int l, int r, T lower) const {
        assert(0 <= l && l <= r && r <= n);
        int cnt = range_freq(l, r, lower);
        if (cnt == r - l) return nullopt;
        return kth_smallest(l, r, cnt);
    }

    vector<T> to_vector() const {
        vector<T> res;
        res.reserve(n);
        for (int i = 0; i < n; i++) res.push_back(access(i));
        return res;
    }
};

// 動的配列の区間 distinct 数。
//
// 実装:
//   各値ごとに出現位置の set を持つ。
//   prev_pos[i] = i より左にある同じ値の直前位置、なければ -1。
//   prev_wm には prev_pos[i] + 1 を載せる。
//   count(l, r) は [l, r) のうち prev_pos[i] < l である i の個数。
//
// 計算量:
//   set O(log A (log N + B) + log N (log N + B))
//   insert/erase O(N(log A (log N + B) + log N (log N + B))) 補助構造を再構築するため重い
//   count O(log N (log N + B))
//
// 注意:
//   - 挿入削除が多い巨大ケース向けではない。
//   - set と count が多い用途向け。
template <class T, int LOG = numeric_limits<make_unsigned_t<T>>::digits,
          int POS_LOG = numeric_limits<unsigned int>::digits>
class DynamicRangeCountDistinct {
   private:
    DynamicWaveletMatrix<T, LOG> wm;
    DynamicWaveletMatrix<int, POS_LOG> prev_wm;
    map<T, std::set<int>> positions;

    void rebuild_auxiliary() {
        positions.clear();
        vector<int> prev_key(wm.size(), 0);
        map<T, int> last;
        for (int i = 0; i < wm.size(); i++) {
            T x = wm.access(i);
            auto it = last.find(x);
            if (it != last.end()) prev_key[i] = it->second + 1;
            last[x] = i;
            positions[x].insert(i);
        }
        prev_wm.build(prev_key);
    }

    static int prev_it(const std::set<int> &s, std::set<int>::const_iterator it) {
        if (it == s.begin()) return -1;
        return *std::prev(it);
    }

    static int next_it(const std::set<int> &s, std::set<int>::const_iterator it) {
        ++it;
        return it == s.end() ? -1 : *it;
    }

   public:
    DynamicRangeCountDistinct() = default;
    explicit DynamicRangeCountDistinct(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        wm.build(v);
        rebuild_auxiliary();
    }

    int size() const { return wm.size(); }
    bool empty() const { return wm.empty(); }

    T access(int k) const { return wm.access(k); }

    void set(int k, const T &x) {
        assert(0 <= k && k < size());
        T old = wm.access(k);
        if (old == x) return;

        auto old_map_it = positions.find(old);
        assert(old_map_it != positions.end());
        auto old_it = old_map_it->second.find(k);
        assert(old_it != old_map_it->second.end());
        int old_prev = prev_it(old_map_it->second, old_it);
        int old_next = next_it(old_map_it->second, old_it);

        int new_prev = -1;
        int new_next = -1;
        auto new_map_it = positions.find(x);
        if (new_map_it != positions.end()) {
            auto lb = new_map_it->second.lower_bound(k);
            new_next = lb == new_map_it->second.end() ? -1 : *lb;
            new_prev = lb == new_map_it->second.begin() ? -1 : *std::prev(lb);
        }

        old_map_it->second.erase(old_it);
        if (old_map_it->second.empty()) positions.erase(old_map_it);
        positions[x].insert(k);

        wm.set(k, x);
        prev_wm.set(k, new_prev + 1);
        if (old_next != -1) prev_wm.set(old_next, old_prev + 1);
        if (new_next != -1) prev_wm.set(new_next, k + 1);
    }

    void insert(int k, const T &x) {
        assert(0 <= k && k <= size());
        wm.insert(k, x);
        rebuild_auxiliary();
    }

    optional<T> erase(int k) {
        if (k < 0 || k >= size()) return nullopt;
        optional<T> res = wm.erase(k);
        rebuild_auxiliary();
        return res;
    }

    int rank(int r, T x) const { return wm.rank(r, x); }
    int rank(int l, int r, T x) const { return wm.rank(l, r, x); }
    T kth_smallest(int l, int r, int k) const { return wm.kth_smallest(l, r, k); }
    T kth_largest(int l, int r, int k) const { return wm.kth_largest(l, r, k); }
    int range_freq(int l, int r, T upper) const { return wm.range_freq(l, r, upper); }
    int range_freq(int l, int r, T lower, T upper) const { return wm.range_freq(l, r, lower, upper); }
    optional<T> prev_value(int l, int r, T upper) const { return wm.prev_value(l, r, upper); }
    optional<T> next_value(int l, int r, T lower) const { return wm.next_value(l, r, lower); }

    int count(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        return prev_wm.range_freq(l, r, l + 1);
    }

    vector<T> to_vector() const { return wm.to_vector(); }
};

int main() {
    int N,M;
    input(N,M);
    vector<int> A(N);
    hash_map<int,vector<pair<int,int>>> mp;
    rep(i,N){
        int a,d,b;
        input(a,d,b);
        a--,b--;
        A[i] = a;
        mp[d].emplace_back(i,b);
    }

    DynamicRangeCountDistinct<int,20> wm(A);

    rep(i,1,M+1){
        for(auto [idx,b]:mp[i]){
            wm.set(idx,b);
        }
        print(wm.count(0,N));
    }
}
