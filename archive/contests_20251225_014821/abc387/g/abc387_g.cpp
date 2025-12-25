// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <bits/stdc++.h>

#ifdef ATCODER
#include <atcoder/all>
using namespace atcoder;
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
ll nearest_px_plus_q_le(ll m, ll p, ll q) {
    // Find the largest integer x not exceeding m such that x ≡ q (mod p)
    return m - smod(m - q, p);
}
ll nearest_px_plus_q_ge(ll m, ll p, ll q) {
    // Find the smallest integer x not less than m such that x ≡ q (mod p)
    return m + (p - smod(m - q, p)) % p;
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
map<rvt<Range>, ll> compression(Range r) {
    ranges::sort(r);
    auto res = ranges::unique(r);
    r.erase(ranges::begin(res), ranges::end(res));
    map<rvt<Range>, ll> ret;
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

// Segment Tree Beats (range chmin, chmax, add, assign, sum/min/max query)
// 可変長対応版（固定長配列をvectorに置き換え）

class SegmentTree {
    using ll = long long;
    const ll inf = (ll)4e18;

    int n, n0;

    vector<ll> max_v, smax_v, max_c;
    vector<ll> min_v, smin_v, min_c;
    vector<ll> sum;
    vector<ll> len, ladd, lval;

    // ----------- helper -----------
    void update_node_max(int k, ll x) {
        sum[k] += (x - max_v[k]) * max_c[k];

        if (max_v[k] == min_v[k]) {
            max_v[k] = min_v[k] = x;
        } else if (max_v[k] == smin_v[k]) {
            max_v[k] = smin_v[k] = x;
        } else {
            max_v[k] = x;
        }

        if (lval[k] != inf && x < lval[k]) lval[k] = x;
    }

    void update_node_min(int k, ll x) {
        sum[k] += (x - min_v[k]) * min_c[k];

        if (max_v[k] == min_v[k]) {
            max_v[k] = min_v[k] = x;
        } else if (smax_v[k] == min_v[k]) {
            min_v[k] = smax_v[k] = x;
        } else {
            min_v[k] = x;
        }

        if (lval[k] != inf && lval[k] < x) lval[k] = x;
    }

    void addall(int k, ll x) {
        max_v[k] += x;
        if (smax_v[k] != -inf) smax_v[k] += x;
        min_v[k] += x;
        if (smin_v[k] != inf) smin_v[k] += x;

        sum[k] += len[k] * x;
        if (lval[k] != inf)
            lval[k] += x;
        else
            ladd[k] += x;
    }

    void updateall(int k, ll x) {
        max_v[k] = x;
        smax_v[k] = -inf;
        min_v[k] = x;
        smin_v[k] = inf;
        max_c[k] = min_c[k] = len[k];
        sum[k] = x * len[k];
        lval[k] = x;
        ladd[k] = 0;
    }

    void push(int k) {
        if (k >= n0 - 1) return;

        if (lval[k] != inf) {
            updateall(2 * k + 1, lval[k]);
            updateall(2 * k + 2, lval[k]);
            lval[k] = inf;
            return;
        }

        if (ladd[k] != 0) {
            addall(2 * k + 1, ladd[k]);
            addall(2 * k + 2, ladd[k]);
            ladd[k] = 0;
        }

        if (max_v[k] < max_v[2 * k + 1]) update_node_max(2 * k + 1, max_v[k]);
        if (max_v[k] < max_v[2 * k + 2]) update_node_max(2 * k + 2, max_v[k]);
        if (min_v[2 * k + 1] < min_v[k]) update_node_min(2 * k + 1, min_v[k]);
        if (min_v[2 * k + 2] < min_v[k]) update_node_min(2 * k + 2, min_v[k]);
    }

    void update(int k) {
        sum[k] = sum[2 * k + 1] + sum[2 * k + 2];

        // max
        if (max_v[2 * k + 1] < max_v[2 * k + 2]) {
            max_v[k] = max_v[2 * k + 2];
            max_c[k] = max_c[2 * k + 2];
            smax_v[k] = max(max_v[2 * k + 1], smax_v[2 * k + 2]);
        } else if (max_v[2 * k + 1] > max_v[2 * k + 2]) {
            max_v[k] = max_v[2 * k + 1];
            max_c[k] = max_c[2 * k + 1];
            smax_v[k] = max(smax_v[2 * k + 1], max_v[2 * k + 2]);
        } else {
            max_v[k] = max_v[2 * k + 1];
            max_c[k] = max_c[2 * k + 1] + max_c[2 * k + 2];
            smax_v[k] = max(smax_v[2 * k + 1], smax_v[2 * k + 2]);
        }

        // min
        if (min_v[2 * k + 1] < min_v[2 * k + 2]) {
            min_v[k] = min_v[2 * k + 1];
            min_c[k] = min_c[2 * k + 1];
            smin_v[k] = min(smin_v[2 * k + 1], min_v[2 * k + 2]);
        } else if (min_v[2 * k + 1] > min_v[2 * k + 2]) {
            min_v[k] = min_v[2 * k + 2];
            min_c[k] = min_c[2 * k + 2];
            smin_v[k] = min(min_v[2 * k + 1], smin_v[2 * k + 2]);
        } else {
            min_v[k] = min_v[2 * k + 1];
            min_c[k] = min_c[2 * k + 1] + min_c[2 * k + 2];
            smin_v[k] = min(smin_v[2 * k + 1], smin_v[2 * k + 2]);
        }
    }

    // --- range operations ---
    void _update_min(ll x, int a, int b, int k, int l, int r) {
        if (b <= l || r <= a || max_v[k] <= x) return;
        if (a <= l && r <= b && smax_v[k] < x) {
            update_node_max(k, x);
            return;
        }
        push(k);
        int m = (l + r) >> 1;
        _update_min(x, a, b, 2 * k + 1, l, m);
        _update_min(x, a, b, 2 * k + 2, m, r);
        update(k);
    }

    void _update_max(ll x, int a, int b, int k, int l, int r) {
        if (b <= l || r <= a || x <= min_v[k]) return;
        if (a <= l && r <= b && x < smin_v[k]) {
            update_node_min(k, x);
            return;
        }
        push(k);
        int m = (l + r) >> 1;
        _update_max(x, a, b, 2 * k + 1, l, m);
        _update_max(x, a, b, 2 * k + 2, m, r);
        update(k);
    }

    void _add_val(ll x, int a, int b, int k, int l, int r) {
        if (b <= l || r <= a) return;
        if (a <= l && r <= b) {
            addall(k, x);
            return;
        }
        push(k);
        int m = (l + r) >> 1;
        _add_val(x, a, b, 2 * k + 1, l, m);
        _add_val(x, a, b, 2 * k + 2, m, r);
        update(k);
    }

    void _update_val(ll x, int a, int b, int k, int l, int r) {
        if (b <= l || r <= a) return;
        if (a <= l && r <= b) {
            updateall(k, x);
            return;
        }
        push(k);
        int m = (l + r) >> 1;
        _update_val(x, a, b, 2 * k + 1, l, m);
        _update_val(x, a, b, 2 * k + 2, m, r);
        update(k);
    }

    ll _query_max(int a, int b, int k, int l, int r) {
        if (b <= l || r <= a) return -inf;
        if (a <= l && r <= b) return max_v[k];
        push(k);
        int m = (l + r) >> 1;
        return max(_query_max(a, b, 2 * k + 1, l, m), _query_max(a, b, 2 * k + 2, m, r));
    }

    ll _query_min(int a, int b, int k, int l, int r) {
        if (b <= l || r <= a) return inf;
        if (a <= l && r <= b) return min_v[k];
        push(k);
        int m = (l + r) >> 1;
        return min(_query_min(a, b, 2 * k + 1, l, m), _query_min(a, b, 2 * k + 2, m, r));
    }

    ll _query_sum(int a, int b, int k, int l, int r) {
        if (b <= l || r <= a) return 0;
        if (a <= l && r <= b) return sum[k];
        push(k);
        int m = (l + r) >> 1;
        return _query_sum(a, b, 2 * k + 1, l, m) + _query_sum(a, b, 2 * k + 2, m, r);
    }

   public:
    SegmentTree(int n) : n(n) {
        n0 = 1;
        while (n0 < n) n0 <<= 1;

        int sz = 2 * n0;
        max_v.assign(sz, 0);
        smax_v.assign(sz, 0);
        max_c.assign(sz, 0);
        min_v.assign(sz, 0);
        smin_v.assign(sz, 0);
        min_c.assign(sz, 0);
        sum.assign(sz, 0);
        len.assign(sz, 0);
        ladd.assign(sz, 0);
        lval.assign(sz, inf);

        len[0] = n0;
        for (int i = 0; i < n0 - 1; i++) len[2 * i + 1] = len[2 * i + 2] = (len[i] >> 1);

        for (int i = 0; i < n; i++) {
            max_v[n0 - 1 + i] = min_v[n0 - 1 + i] = sum[n0 - 1 + i] = 0;

            smax_v[n0 - 1 + i] = -inf;
            smin_v[n0 - 1 + i] = inf;

            max_c[n0 - 1 + i] = min_c[n0 - 1 + i] = 1;
        }

        for (int i = n; i < n0; i++) {
            max_v[n0 - 1 + i] = smax_v[n0 - 1 + i] = -inf;
            min_v[n0 - 1 + i] = smin_v[n0 - 1 + i] = inf;
            max_c[n0 - 1 + i] = min_c[n0 - 1 + i] = 0;
        }

        for (int i = n0 - 2; i >= 0; i--) update(i);
    }

    SegmentTree(vector<ll> a) : n(ssize(a)) {
        n0 = 1;
        while (n0 < n) n0 <<= 1;

        int sz = 2 * n0;
        max_v.assign(sz, 0);
        smax_v.assign(sz, 0);
        max_c.assign(sz, 0);
        min_v.assign(sz, 0);
        smin_v.assign(sz, 0);
        min_c.assign(sz, 0);
        sum.assign(sz, 0);
        len.assign(sz, 0);
        ladd.assign(sz, 0);
        lval.assign(sz, inf);

        len[0] = n0;
        for (int i = 0; i < n0 - 1; i++) len[2 * i + 1] = len[2 * i + 2] = (len[i] >> 1);

        for (int i = 0; i < n; i++) {
            max_v[n0 - 1 + i] = min_v[n0 - 1 + i] = sum[n0 - 1 + i] = a[i];

            smax_v[n0 - 1 + i] = -inf;
            smin_v[n0 - 1 + i] = inf;

            max_c[n0 - 1 + i] = min_c[n0 - 1 + i] = 1;
        }

        for (int i = n; i < n0; i++) {
            max_v[n0 - 1 + i] = smax_v[n0 - 1 + i] = -inf;
            min_v[n0 - 1 + i] = smin_v[n0 - 1 + i] = inf;
            max_c[n0 - 1 + i] = min_c[n0 - 1 + i] = 0;
        }

        for (int i = n0 - 2; i >= 0; i--) update(i);
    }

    // --- public interface ---
    void update_min(int a, int b, ll x) { _update_min(x, a, b, 0, 0, n0); }
    void update_max(int a, int b, ll x) { _update_max(x, a, b, 0, 0, n0); }
    void add_val(int a, int b, ll x) { _add_val(x, a, b, 0, 0, n0); }
    void update_val(int a, int b, ll x) { _update_val(x, a, b, 0, 0, n0); }

    ll query_max(int a, int b) { return _query_max(a, b, 0, 0, n0); }
    ll query_min(int a, int b) { return _query_min(a, b, 0, 0, n0); }
    ll query_sum(int a, int b) { return _query_sum(a, b, 0, 0, n0); }
};

int main() {
    int N, Q;
    input(N, Q);
    vector<ll> A(N);
    input(A);
    SegmentTree seg(A);
    ll q, l, r, b;
    rep(i, Q) {
        input(q);
        if (q == 0) {
            input(l, r, b);
            seg.update_min(l, r, b);
        } else if (q == 1) {
            input(l, r, b);
            seg.update_max(l, r, b);
        } else if (q == 2) {
            input(l, r, b);
            seg.add_val(l, r, b);
        } else {
            input(l, r);
            print(seg.query_sum(l, r));
        }
    }
}