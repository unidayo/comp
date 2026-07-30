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
// Wavelet Matrix。
//
// 使い方:
//   WaveletMatrix<long long> wm(a);  // 負の値を含む signed 整数にも対応
//
//   wm.access(k);                 // a[k]
//   wm.rank(x, r);                // [0, r) にある x の個数
//   wm.rank(x, l, r);             // [l, r) にある x の個数
//   wm.kth_smallest(l, r, k);     // [l, r) の k 番目に小さい値（0-indexed）
//   wm.kth_largest(l, r, k);      // [l, r) の k 番目に大きい値（0-indexed）
//   wm.range_freq(l, r, upper);   // [l, r) にある upper 未満の値の個数
//   wm.range_freq(l, r, lower, upper);  // [l, r) にある [lower, upper) の値の個数
//   wm.prev_value(l, r, upper);   // [l, r) にある upper 未満の最大値。なければ nullopt
//   wm.next_value(l, r, lower);   // [l, r) にある lower 以上の最小値。なければ nullopt
//
// Static Range Count-Distinct:
//   RangeCountDistinct<long long> cd(a);
//   cd.count(l, r);               // [l, r) にある相異なる値の個数
//
// 注意:
//   - T は整数型を想定。
//   - デフォルトの LOG では signed の負の値にも対応し、大小順も正しく扱う。
//     内部では符号ビットを反転して unsigned として並べる。
//   - signed 型で LOG を小さく指定する場合は、値が非負で 2^LOG 未満に収まる用途を想定。
//   - 構築 O(N log A)、`access` は O(1)、それ以外のクエリは O(log A)。

template <class T, int LOG = numeric_limits<make_unsigned_t<T>>::digits>
class WaveletMatrix {
    static_assert(is_integral_v<T>, "WaveletMatrix<T>: T must be an integral type");

   private:
    using U = make_unsigned_t<T>;
    static constexpr int FULL_LOG = numeric_limits<U>::digits;

    struct BitVector {
        int n = 0;
        int zeros = 0;
        vector<unsigned long long> bit;
        vector<int> sum;

        BitVector() = default;
        explicit BitVector(int n_) : n(n_), bit((n_ + 63) >> 6, 0) {}

        void set(int k) { bit[k >> 6] |= 1ULL << (k & 63); }
        void build() {
            sum.assign(bit.size() + 1, 0);
            for (int i = 0; i < (int)bit.size(); i++) {
                sum[i + 1] = sum[i] + __builtin_popcountll(bit[i]);
            }
            zeros = n - sum.back();
        }
        bool get(int k) const { return (bit[k >> 6] >> (k & 63)) & 1ULL; }
        int rank1(int k) const {
            int block = k >> 6;
            int offset = k & 63;
            int res = sum[block];
            if (offset) res += __builtin_popcountll(bit[block] & ((1ULL << offset) - 1));
            return res;
        }
        int rank0(int k) const { return k - rank1(k); }
    };

    int n = 0;
    vector<T> data;
    array<BitVector, LOG> matrix;

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
                l = matrix[level].zeros + l - l0;
                r = matrix[level].zeros + r - r0;
            } else {
                l = l0;
                r = r0;
            }
        }
        return res;
    }

   public:
    WaveletMatrix() = default;
    explicit WaveletMatrix(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        n = (int)v.size();
        data = v;
        vector<U> cur(n), nxt(n);
        for (int i = 0; i < n; i++) cur[i] = encode(v[i]);

        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            matrix[level] = BitVector(n);

            int zero_count = 0;
            for (int i = 0; i < n; i++) {
                if (bit_at(cur[i], b))
                    matrix[level].set(i);
                else
                    zero_count++;
            }
            matrix[level].build();

            int zi = 0, oi = zero_count;
            for (int i = 0; i < n; i++) {
                if (bit_at(cur[i], b))
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
        return data[k];
    }

    int rank(T x, int r) const {
        assert(0 <= r && r <= n);
        return rank(x, 0, r);
    }

    int rank(T x, int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        U y = encode(x);
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            int l0 = matrix[level].rank0(l);
            int r0 = matrix[level].rank0(r);
            if (bit_at(y, b)) {
                l = matrix[level].zeros + l - l0;
                r = matrix[level].zeros + r - r0;
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
                l = matrix[level].zeros + l - l0;
                r = matrix[level].zeros + r - r0;
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
};

// 静的配列の区間 distinct 数。
//
// 考え方:
//   next[i] = i より右で最初に a[i] と同じ値が出る位置。なければ N。
//   [l, r) 内の値ごとに最後の出現位置 i だけが next[i] >= r になるため、
//   distinct 数 = i in [l, r) かつ next[i] >= r の個数。
//
// 計算量:
//   構築 O(N log N)、クエリ O(log N)
template <class T>
class RangeCountDistinct {
   private:
    int n = 0;
    WaveletMatrix<int> next_pos;

   public:
    RangeCountDistinct() = default;
    explicit RangeCountDistinct(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        n = (int)v.size();
        vector<int> nxt(n);
        map<T, int> last;
        for (int i = n - 1; i >= 0; i--) {
            auto it = last.find(v[i]);
            nxt[i] = (it == last.end() ? n : it->second);
            last[v[i]] = i;
        }
        next_pos.build(nxt);
    }

    int size() const { return n; }

    int count(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        return r - l - next_pos.range_freq(l, r, r);
    }
};

int main() {
    int N,Q;
    input(N,Q);
    vector<ll> A(N);
    input(A);
    RangeCountDistinct<ll> wm(A);
    rep(i,Q){
        ll l,r;
        input(l,r);
        print(wm.count(l,r));
    }
}
