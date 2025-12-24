// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <bits/stdc++.h>
#include <atcoder/all>
using namespace atcoder;

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
        // ランダムアクセス or push_back が可能
        c.push_back(declval<T>());
    }
{
    for (auto &val : v) is >> val;
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
        { a > b } -> std::convertible_to<bool>;  // 比較できる
        { a = b };                               // 代入できる
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
        { a < b } -> std::convertible_to<bool>;  // 比較できる
        { a = b };                               // 代入できる
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

template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
struct lazy_segtree_sparse {
    struct Node {
        Node *l = nullptr, *r = nullptr;
        S val;
        F lazy;
        bool has_lazy;
        int len;  // このノードが表す区間の長さ
        Node(int len_, const S &v = e()) : val(v), lazy(id()), has_lazy(false), len(len_) {}
    };

    int n;
    Node *root;

    lazy_segtree_sparse(int _n) : n(_n), root(nullptr) {}
    lazy_segtree_sparse(const vector<S> &v) : n((int)v.size()), root(nullptr) {
        for (int i = 0; i < n; i++) {
            if (!(v[i] == e())) {
                set(i, v[i]);
            }
        }
    }

    ~lazy_segtree_sparse() { destroy(root); }

    void set(int idx, S x) {
        check_idx(idx);
        set_rec(root, 0, n, idx, x);
    }
    S get(int idx) {
        check_idx(idx);
        return get_rec(root, 0, n, idx);
    }
    void apply(int l, int r, F f) {
        check_range(l, r);
        apply_rec(root, 0, n, l, r, f);
    }
    S prod(int l, int r) {
        check_range(l, r);
        return prod_rec(root, 0, n, l, r);
    }

   private:
    void check_idx(int i) {
        if (i < 0 || i >= n) throw;
    }
    void check_range(int l, int r) {
        if (l < 0 || r > n || l > r) throw;
    }

    void destroy(Node *p) {
        if (!p) return;
        destroy(p->l);
        destroy(p->r);
        delete p;
    }

    inline Node *ensure_node(Node *&node, int len) {
        if (!node) node = new Node(len);
        return node;
    }

    void apply_node(Node *p, F f) {
        p->val = mapping(f, p->val, p->len);
        if (p->has_lazy)
            p->lazy = composition(f, p->lazy);
        else {
            p->lazy = f;
            p->has_lazy = true;
        }
    }

    void push(Node *p) {
        if (!p || !p->has_lazy) return;
        int L = p->len / 2;
        int R = p->len - L;
        if (!p->l) p->l = new Node(L);
        if (!p->r) p->r = new Node(R);
        apply_node(p->l, p->lazy);
        apply_node(p->r, p->lazy);
        p->lazy = id();
        p->has_lazy = false;
    }

    void pull(Node *p) {
        S lv = p->l ? p->l->val : e();
        S rv = p->r ? p->r->val : e();
        p->val = op(lv, rv);
    }

    S get_rec(Node *p, int nl, int nr, int idx) {
        if (!p) return e();
        if (nr - nl == 1) return p->val;
        push(p);
        int mid = (nl + nr) >> 1;
        if (idx < mid)
            return get_rec(p->l, nl, mid, idx);
        else
            return get_rec(p->r, mid, nr, idx);
    }

    void set_rec(Node *&p, int nl, int nr, int idx, S x) {
        if (!p) p = new Node(nr - nl);
        if (nr - nl == 1) {
            p->val = x;
            p->lazy = id();
            p->has_lazy = false;
            return;
        }
        push(p);
        int mid = (nl + nr) >> 1;
        if (idx < mid)
            set_rec(p->l, nl, mid, idx, x);
        else
            set_rec(p->r, mid, nr, idx, x);
        pull(p);
    }

    void apply_rec(Node *&p, int nl, int nr, int l, int r, F f) {
        if (r <= nl || nr <= l) return;
        if (!p) p = new Node(nr - nl);
        if (l <= nl && nr <= r) {
            apply_node(p, f);
            return;
        }
        push(p);
        int mid = (nl + nr) >> 1;
        apply_rec(p->l, nl, mid, l, r, f);
        apply_rec(p->r, mid, nr, l, r, f);
        pull(p);
    }

    S prod_rec(Node *p, int nl, int nr, int l, int r) {
        if (r <= nl || nr <= l) return e();
        if (!p) return e();
        if (l <= nl && nr <= r) return p->val;
        push(p);
        int mid = (nl + nr) >> 1;
        return op(prod_rec(p->l, nl, mid, l, r), prod_rec(p->r, mid, nr, l, r));
    }
};
using mint = modint998244353;

using S = mint;

struct F {
    mint a, b;
};

S op(S l, S r) { return l + r; }

S e() { return 0; }

S mapping(F l, S r, int len) { return r * l.a + len * l.b; }

F composition(F l, F r) { return F{r.a * l.a, r.b * l.a + l.b}; }

F id() { return F{1, 0}; }

int main() {
    int N, Q;
    input(N, Q);
    lazy_segtree_sparse<S, op, e, F, mapping, composition, id> seg(N);
    int q, l, r, b, c;
    rep(i, Q) {
        input(q);
        if (q == 0) {
            input(l, r, b, c);
            seg.apply(l, r, {b, c});
        } else {
            input(l, r);
            print(seg.prod(l, r));
        }
    }
}