// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <bits/stdc++.h>

#ifndef ATCODER_SEGTREE_HPP
#define ATCODER_SEGTREE_HPP 1

#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>

namespace atcoder {

#if __cplusplus >= 201703L

template <class S, auto op, auto e>
struct segtree {
    static_assert(std::is_convertible_v<decltype(op), std::function<S(S, S)>>, "op must work as S(S, S)");
    static_assert(std::is_convertible_v<decltype(e), std::function<S()>>, "e must work as S()");

#else

template <class S, S (*op)(S, S), S (*e)()>
struct segtree {

#endif

   public:
    segtree() : segtree(0) {}
    explicit segtree(int n) : segtree(std::vector<S>(n, e())) {}
    explicit segtree(const std::vector<S> &v) : _n(int(v.size())) {
        size = (int)std::bit_ceil((unsigned int)(_n));
        log = std::countr_zero((unsigned int)size);
        d = std::vector<S>(2 * size, e());
        for (int i = 0; i < _n; i++) d[size + i] = v[i];
        for (int i = size - 1; i >= 1; i--) {
            update(i);
        }
    }

    void set(int p, S x) {
        assert(0 <= p && p < _n);
        p += size;
        d[p] = x;
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    S get(int p) const {
        assert(0 <= p && p < _n);
        return d[p + size];
    }

    S prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= _n);
        S sml = e(), smr = e();
        l += size;
        r += size;

        while (l < r) {
            if (l & 1) sml = op(sml, d[l++]);
            if (r & 1) smr = op(d[--r], smr);
            l >>= 1;
            r >>= 1;
        }
        return op(sml, smr);
    }

    S all_prod() const { return d[1]; }

    template <bool (*f)(S)>
    int max_right(int l) const {
        return max_right(l, [](S x) { return f(x); });
    }
    template <class F>
    int max_right(int l, F f) const {
        assert(0 <= l && l <= _n);
        assert(f(e()));
        if (l == _n) return _n;
        l += size;
        S sm = e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(op(sm, d[l]))) {
                while (l < size) {
                    l = (2 * l);
                    if (f(op(sm, d[l]))) {
                        sm = op(sm, d[l]);
                        l++;
                    }
                }
                return l - size;
            }
            sm = op(sm, d[l]);
            l++;
        } while ((l & -l) != l);
        return _n;
    }

    template <bool (*f)(S)>
    int min_left(int r) const {
        return min_left(r, [](S x) { return f(x); });
    }
    template <class F>
    int min_left(int r, F f) const {
        assert(0 <= r && r <= _n);
        assert(f(e()));
        if (r == 0) return 0;
        r += size;
        S sm = e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!f(op(d[r], sm))) {
                while (r < size) {
                    r = (2 * r + 1);
                    if (f(op(d[r], sm))) {
                        sm = op(d[r], sm);
                        r--;
                    }
                }
                return r + 1 - size;
            }
            sm = op(d[r], sm);
        } while ((r & -r) != r);
        return 0;
    }

   private:
    int _n, size, log;
    std::vector<S> d;

    void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
};

}  // namespace atcoder

#endif  // ATCODER_SEGTREE_HPP
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

ll op(ll x, ll y){
    return min(x,y);
}
ll e(){
    return INF64;
}
template <class T>
class Treap {
   private:
    struct Node {
        T key;
        Node *l, *r;
        uint priority;
        int cnt;
    } *root = nullptr;
    using Tree = Node *;

    static constexpr int CHUNK_SIZE = 1 << 20;
    inline static vector<Node *> chunks;
    inline static int chunk_pos = CHUNK_SIZE;
    uint rng_state = 123456789;

    uint rng() {
        uint x = rng_state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return rng_state = x;
    }

    Node *new_node(T key) {
        if (chunk_pos == CHUNK_SIZE) {
            chunks.push_back(new Node[CHUNK_SIZE]);
            chunk_pos = 0;
        }
        Node *p = &chunks.back()[chunk_pos++];
        *p = {key, nullptr, nullptr, rng(), 1};
        return p;
    }

    int count(const Tree t) const { return t ? t->cnt : 0; }
    Tree update(Tree t) {
        t->cnt = count(t->l) + count(t->r) + 1;
        return t;
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

    Tree insert(Tree t, T key) {
        if (!t) return new_node(key);
        if (key < t->key) {
            t->l = insert(t->l, key);
            if (t->l->priority > t->priority) t = rotate_right(t);
        } else {
            t->r = insert(t->r, key);
            if (t->r->priority > t->priority) t = rotate_left(t);
        }
        return update(t);
    }

    Tree erase(Tree t, T key) {
        if (!t) return nullptr;
        if (key == t->key) {
            return merge(t->l, t->r);
        }
        if (key < t->key) {
            t->l = erase(t->l, key);
        } else {
            t->r = erase(t->r, key);
        }
        return update(t);
    }

    bool find(Tree t, T key) const {
        while (t) {
            if (key == t->key) return true;
            t = key < t->key ? t->l : t->r;
        }
        return false;
    }

    int lower_bound(Tree t, T key) const {
        int res = 0;
        while (t) {
            if (key <= t->key) {
                t = t->l;
            } else {
                res += count(t->l) + 1;
                t = t->r;
            }
        }
        return res;
    }

    int upper_bound(Tree t, T key) const {
        int res = 0;
        while (t) {
            if (key < t->key) {
                t = t->l;
            } else {
                res += count(t->l) + 1;
                t = t->r;
            }
        }
        return res;
    }

    optional<T> prev(Tree t, T x) const {
        Tree res = nullptr;
        while (t) {
            if (t->key < x) {
                res = t;
                t = t->r;
            } else {
                t = t->l;
            }
        }
        if (!res) return nullopt;
        return res->key;
    }
    optional<T> next(Tree t, T x) const {
        Tree res = nullptr;
        while (t) {
            if (t->key > x) {
                res = t;
                t = t->l;
            } else {
                t = t->r;
            }
        }
        if (!res) return nullopt;
        return res->key;
    }

    T min(Tree t) const {
        while (t->l) t = t->l;
        return t->key;
    }
    T max(Tree t) const {
        while (t->r) t = t->r;
        return t->key;
    }

    T kth(Tree t, int k) const {
        while (true) {
            int lc = count(t->l);
            if (k < lc) {
                t = t->l;
            } else if (k == lc) {
                return t->key;
            } else {
                k -= lc + 1;
                t = t->r;
            }
        }
    }

   public:
    Treap() = default;
    // key を1つ挿入する
    void insert(T key) { root = insert(root, key); }
    // key が存在しなければ挿入する
    bool insert_unique(T key) {
        if (find(key)) return false;
        root = insert(root, key);
        return true;
    }
    // key が存在するか判定する
    bool find(T key) { return find(root, key); }
    // 最小値を返す
    T min() { return min(root); }
    // 最大値を返す
    T max() { return max(root); }
    // key 未満の最大の要素を返す。存在しない場合は nullopt
    optional<T> prev(T key) { return prev(root, key); }
    // key より大きい最小の要素を返す。存在しない場合は nullopt
    optional<T> next(T key) { return next(root, key); }
    // key 未満の要素の個数（= key 以上の要素が始まるインデックス）を返す
    ll lower_bound(T key) { return lower_bound(root, key); }
    // key 以下の要素の個数（= key より大きい要素が始まるインデックス）を返す
    ll upper_bound(T key) { return upper_bound(root, key); }
    // key に等しい要素の [lower_bound, upper_bound) を返す
    pair<int, int> equal_range(T key) { return {lower_bound(key), upper_bound(key)}; }
    // key を1つ削除する。存在しなければ false を返す
    bool erase(T key) {
        if (!find(key)) return false;
        root = erase(root, key);
        return true;
    }
    // 昇順 k 番目（0-indexed）の要素を返す
    T kth(int k) { return kth(root, k); }
    // 要素数を返す
    int size() { return root ? root->cnt : 0; }
};
int main() {
    ll N,M;
    input(N,M);
    vector<ll> A(N);
    input(A);
    atcoder::segtree<ll,op,e> seg(A);
    Treap<ll> tr;


    vector<pair<ll,int>> B(N);
    rep(i,N){
        B[i] = {A[i],i};
    }
    ll mx = ranges::max(A);
    ranges::sort(B,ranges::greater());
    ll ans = 0;
    for(auto [x,idx]:B){
        ll p = 0;
        if(x==mx){
            p = x;
        } else {
            ll m = 0;
            auto l = tr.prev(idx);
            if (l){
                chmax(m,seg.prod(l.value()+1,idx));
            }
            auto r = tr.next(idx);
            if (r){
                chmax(m,seg.prod(idx+1,r.value()));
            }
            p = x-m;
        }
        if (p >= M){
            ans++;
        }
        tr.insert(idx);
    }
    print(ans);

}
