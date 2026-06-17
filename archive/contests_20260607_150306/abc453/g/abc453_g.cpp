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
template <typename K>
using hash_set = ankerl::unordered_dense::set<K>;
#else
template <typename K, typename V>
using hash_map = std::unordered_map<K, V>;
template <typename K>
using hash_set = std::unordered_set<K>;
#endif

namespace cp_internal {
char char_buff[64];
};

/* namespace */
using namespace std;

/* type */
using uint = unsigned int;
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;   // 128bit符号付き整数
using uvll = __uint128_t; // 128bit符号なし整数
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
template <int m>
istream &operator>>(istream &is, static_modint<m> &x) {
    ll val;
    is >> val;
    x = val;
    return is;
}
template <int m>
ostream &operator<<(ostream &os, const static_modint<m> &x) {
    os << x.val();
    return os;
}
#endif
template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &pr) {
    os << pr.first << ' ' << pr.second;
    return os;
}
template <class T1, class T2>
istream &operator>>(istream &is, pair<T1, T2> &pr) {
    is >> pr.first >> pr.second;
    return is;
}

template <class Tuple, size_t... I>
void print_tuple_impl(ostream &os, const Tuple &t, index_sequence<I...>) {
    ((os << (I == 0 ? "" : " ") << get<I>(t)), ...);
}
template <class... Ts>
ostream &operator<<(ostream &os, const tuple<Ts...> &t) {
    print_tuple_impl(os, t, index_sequence_for<Ts...>{});
    return os;
}
template <class Tuple, size_t... I>
void read_tuple_impl(istream &is, Tuple &t, index_sequence<I...>) {
    ((is >> get<I>(t)), ...);
}
template <class... Ts>
istream &operator>>(istream &is, tuple<Ts...> &t) {
    read_tuple_impl(is, t, index_sequence_for<Ts...>{});
    return is;
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
    requires requires(Container<T> c) {
        c.begin();
        c.end();
        c.push_back(T{});
    } && (!is_same_v<Container<T>, string>)
{
    for (auto &x : v) is >> x;
    return is;
}

template <class... T>
void input(T &...a) {
    (cin >> ... >> a);
}

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

/* debug */
// ローカルのみ変数名付きで標準エラーに出力、提出時は無効
#ifdef LOCAL
template <Iterable T>
void debug_impl(const T &v) {
    bool first = true;
    for (auto const &x : v) {
        if (!first) cerr << " ";
        first = false;
        cerr << x;
    }
    cerr << '\n';
}
template <class T, class... Ts>
void debug_impl(const T &a, const Ts &...b) {
    ((cerr << a), ..., (cerr << ' ' << b));
    cerr << '\n';
}
#define debug(...) do { cerr << "\033[33m[" << #__VA_ARGS__ << "]\033[0m "; debug_impl(__VA_ARGS__); } while(0)
#else
#define debug(...)
#endif

/* math*/
// x^n を整数で返す (オーバーフロー注意、mod不要なとき用)
ll powll(ll x, ll n) {
    ll ret = 1;
    while (n) {
        if (n & 1) ret = x * ret;
        x *= x;
        n >>= 1;
    }
    return ret;
}
// x/y の切り上げ (負の数も正しく動作)
template <class T>
T divceil(T x, T y) {
    auto d = x / y, r = x % y;
    return d + (r != 0 && ((r ^ y) > 0));
}
// x/y の切り捨て (負の数も正しく動作)
template <class T>
T divfloor(T x, T y) {
    auto d = x / y, r = x % y;
    return d - (r != 0 && ((r ^ y) < 0));
}
// x % m を常に非負で返す
template <class T1, class T2>
T1 smod(T1 x, T2 m) {
    if (x < 0) return (x % m + m) % m;
    return x % m;
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
ll nearest_px_plus_q_le(ll m, ll p, ll q) {
    return m - smod(m - q, p);
}
// m以上の最小の整数x で x ≡ q (mod p) を返す
ll nearest_px_plus_q_ge(ll m, ll p, ll q) {
    return m + (p - smod(m - q, p)) % p;
}
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
    vll cross = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    return cross == 0;
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
// a = min(a, b)。更新したらtrue
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
// a = max(a, b)。更新したらtrue
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
// rをw個ずつに分割したvector<vector<T>>を返す
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

// RLE。{値, 連続個数} のvectorを返す。例: "aaabbc" -> {('a',3),('b',2),('c',1)}
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
auto sum_of(const Range &R) {
    return reduce(ranges::begin(R), ranges::end(R));
}
template <ranges::range Range, class T>
T sum_of(const Range &R) {
    return reduce(ranges::begin(R), ranges::end(R), T());
}

/* data_structure */
// 最小値優先ヒープ (pq.top()が最小)
template <class T>
using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
// 最大値優先ヒープ (pq.top()が最大)
template <class T>
using priority_queue_max = priority_queue<T, vector<T>>;
// vector<T>(n, val)
// vector<vector<T>>(n, m, val)  (多次元も可)
template <class T>
auto make_vec(size_t n, const T &val) {
    return vector<T>(n, val);
}
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
[[maybe_unused]] constexpr int dx[] = {0, 1, 0, -1};
[[maybe_unused]] constexpr int dy[] = {1, 0, -1, 0};
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
// 高速な疑似乱数生成器
// 32bit乱数
// 64bit乱数
// [l,r]の32bit一様乱数
// [l,r]の64bit一様乱数
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
    uint rand_range(uint l, uint r) {
        uint range = r - l + 1;
        uint limit = ~uint(0) - (~uint(0) % range);
        while (true) {
            uint x = (*this)();
            if (x <= limit) {
                return l + (x % range);
            }
        }
    }
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
// ACL風の永続セグメント木
//
// 使い方:
//   S op(S a, S b) { ... }
//   S e() { ... }
//   PersistentSegmentTree<S, op, e> seg(n);   // 空木で初期化
//   PersistentSegmentTree<S, op, e> seg(v);   // vector から構築
//
//   int t0 = 0;                               // 初期バージョン
//   int t1 = seg.set(t0, p, x);               // version t0 を元に a[p] = x
//
//   S x = seg.get(t1, p);                     // version t1 の p 番目の値
//   S y = seg.prod(t1, l, r);                 // version t1 の区間 [l, r) の積
//   S z = seg.all_prod(t1);                   // version t1 全体の積
//
//   seg.get(p), seg.prod(l, r), seg.all_prod()
//   は常に最新バージョンに対して作用する。
//
// 注意:
//   - 更新のたびに新しいバージョンが末尾に追加され、その番号を返す。
//   - `set` は `node*` を直接渡して、その root から分岐することもできる。

template <class S, auto op, auto e, int NODES = 20000000>
struct PersistentSegmentTree {
    using ll = long long;

    struct node {
        S data;
        node *l, *r;
        explicit node(const S &data_) : data(data_), l(nullptr), r(nullptr) {}
        node(const S &data_, node *l_, node *r_) : data(data_), l(l_), r(r_) {}
    };

    node *pool;
    int pid;
    int _n;
    node *nil;
    vector<node *> roots;

    PersistentSegmentTree() : PersistentSegmentTree(0) {}

    explicit PersistentSegmentTree(int n)
        : pool(static_cast<node *>(::operator new(sizeof(node) * NODES))), pid(0), _n(n), nil(nullptr) {
        nil = new_node(e());
        nil->l = nil->r = nil;
        roots.reserve(262144);
        roots.push_back(nil);
    }

    explicit PersistentSegmentTree(const vector<S> &v)
        : pool(static_cast<node *>(::operator new(sizeof(node) * NODES))), pid(0), _n((int)v.size()), nil(nullptr) {
        nil = new_node(e());
        nil->l = nil->r = nil;
        roots.reserve(262144);
        roots.push_back(_n == 0 ? nil : build(0, _n, v));
    }

    ~PersistentSegmentTree() {
        for (int i = 0; i < pid; i++) pool[i].~node();
        ::operator delete(pool);
    }

   private:
    node *new_node(const S &data) {
        node *ptr = &pool[pid++];
        ::new (static_cast<void *>(ptr)) node(data);
        ptr->l = ptr->r = nil;
        return ptr;
    }

    node *merge(node *l, node *r) {
        node *ptr = &pool[pid++];
        ::new (static_cast<void *>(ptr)) node(op(l->data, r->data), l, r);
        return ptr;
    }

    node *build(int l, int r, const vector<S> &v) {
        if (l + 1 == r) return new_node(v[l]);
        int m = (l + r) >> 1;
        return merge(build(l, m, v), build(m, r, v));
    }

    node *set_impl(int p, const S &x, node *n, int l, int r) {
        if (l + 1 == r) return new_node(x);
        int m = (l + r) >> 1;
        if (p < m) return merge(set_impl(p, x, n->l, l, m), n->r);
        return merge(n->l, set_impl(p, x, n->r, m, r));
    }

    S get_impl(int p, node *n, int l, int r) const {
        if (n == nil) return e();
        if (l + 1 == r) return n->data;
        int m = (l + r) >> 1;
        if (p < m) return get_impl(p, n->l, l, m);
        return get_impl(p, n->r, m, r);
    }

    S prod_impl(int a, int b, node *n, int l, int r) const {
        if (n == nil) return e();
        if (r <= a || b <= l) return e();
        if (a <= l && r <= b) return n->data;
        int m = (l + r) >> 1;
        return op(prod_impl(a, b, n->l, l, m), prod_impl(a, b, n->r, m, r));
    }

    int append_root(node *root) {
        roots.push_back(root);
        return (int)roots.size() - 1;
    }

   public:
    int size() const { return _n; }

    int versions() const { return (int)roots.size(); }

    node *root(int t) const {
        assert(0 <= t && t < versions());
        return roots[t];
    }

    int set(node *n, int p, const S &x) {
        assert(0 <= p && p < _n);
        return append_root(set_impl(p, x, n, 0, _n));
    }

    int set(int t, int p, const S &x) {
        assert(0 <= t && t < versions());
        return set(roots[t], p, x);
    }

    int set(int p, const S &x) { return set(roots.back(), p, x); }

    S get(node *n, int p) const {
        assert(0 <= p && p < _n);
        return get_impl(p, n, 0, _n);
    }

    S get(int t, int p) const {
        assert(0 <= t && t < versions());
        return get(roots[t], p);
    }

    S get(int p) const { return get(roots.back(), p); }

    S prod(node *n, int l, int r) const {
        assert(0 <= l && l <= r && r <= _n);
        return prod_impl(l, r, n, 0, _n);
    }

    S prod(int t, int l, int r) const {
        assert(0 <= t && t < versions());
        return prod(roots[t], l, r);
    }

    S prod(int l, int r) const { return prod(roots.back(), l, r); }

    S all_prod(node *n) const { return n == nil ? e() : n->data; }

    S all_prod(int t) const {
        assert(0 <= t && t < versions());
        return all_prod(roots[t]);
    }

    S all_prod() const { return all_prod(roots.back()); }

    node *new_tree() const { return nil; }
};

int main() {
    ll N,M,Q;
    input(N,M,Q);
    auto op = [](ll x, ll y){return x+y;};
    auto e = []{return 0LL;};
    PersistentSegmentTree<ll,op,e> seg(M);
    vector<int> X(N);
    
    ll q,x,y,z,l,r;
    rep(i,Q){
        input(q);
        if (q == 1){
            input(x,y);
            x--,y--;
            X[x] = X[y];
        } else if (q==2){
            input(x,y,z);
            x--,y--;
            X[x] = seg.set(X[x],y,z);
        } else {
            input(x,l,r);
            x--,l--;
            print(seg.prod(X[x],l,r));
        }
    }
}