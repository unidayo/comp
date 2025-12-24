// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <bits/stdc++.h>

#ifdef ATCODER
    #include <atcoder/all>
    using namespace atcoder;
#endif
namespace cp_internal {char char_buff[64];};

/* namespace */
using namespace std;

/* type */
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;
using vull = __uint128_t;
template <typename T> concept RAR = ranges::random_access_range<T>;
template <ranges::range Range> using rvt = ranges::range_value_t<Range>;

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
istream &operator>>(istream &is, pair<T1, T2> &pr) {
    is >> pr.first >> pr.second;
    return is;
}

template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &pr) {
    os << pr.first << ' ' << pr.second;
    return os;
}

template <class T>
istream &operator>>(istream &is, vector<T> &v){
    for (auto &val:v) is >> val;
    return is;
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
void print(const vector<T> &v, string sep = " ") {
    if (!v.empty()) {
        auto p = prev(end(v));
        for_each(begin(v), p, [&sep](T x) { cout << x << sep; });
        cout << *p;
    }
    cout << '\n';
}
template <class T>
void print(const vector<T> &v, int l, int r, string sep = " ") {
    if (l < r) {
        auto p = begin(v) + min((int)ssize(v) - 1, r - 1);
        for_each(v.begin() + l, p, [&sep](auto x) { cout << x << sep; });
        cout << *p;
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
ll pow_mod64(vll x, vll n, ll m) {
    if (m == 1) return 0;
    vll ret = 1;
    x %= m;
    while (n) {
        if (n & 1) ret = x * ret % m;
        x = x * x % m;
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
template <class T>
T smod(T x, T m) {
    if (x < 0) return (x % m + m) % m;
    return x % m;
}

ll nearest_px_plus_q_le(ll m, ll p, ll q) {
    // pで割ってqで余る、m以下で最大の整数xを求める
    return m - smod(m - q, p);
}

ll nearest_px_plus_q_ge(ll m, ll p, ll q) {
    // pで割ってqで余る、m以上で最小の整数xを求める
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

template <class T>
bool chmin(T &a, const T &b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}
template <class T>
bool chmax(T &a, const T &b) {
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
        ret[v] = i;
        ++i;
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

template <class T>
vector<T> gen_sequence(T init, int n, auto op) {
    vector<T> res(n + 1);
    res[0] = init;
    for (int i = 0; i < n; ++i) {
        res[i + 1] = op(res[i]);
    }
    return res;
}

/* data_structure */
template <class T>
using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
template <class T>
using priority_queue_max = priority_queue<T, vector<T>>;

template <class T>
auto vec2d(size_t n1, size_t n2, T init = T()) {
    return vector(n1, vector(n2, init));
}
template <class T>
auto vec3d(size_t n1, size_t n2, size_t n3, T init = T()) {
    return vector(n1, vector(n2, vector(n3, init)));
}
template <class T>
auto vec4d(size_t n1, size_t n2, size_t n3, size_t n4, T init = T()) {
    return vector(n1, vector(n2, vector(n3, vector(n4, init))));
}
template <class T>
auto evec2d(size_t n1) {
    return vector(n1, vector<T>());
}
template <class T>
auto evec3d(size_t n1, size_t n2) {
    return vector(n1, vector(n2, vector<T>()));
}
template <class T>
auto evec4d(size_t n1, size_t n2, size_t n3) {
    return vector(n1, vector(n2, vector(n3, vector<T>())));
}

/* string */
string dec_to_base(ull x, int base) {
    auto b = begin(cp_internal::char_buff);
    auto [ptr, ec] = to_chars(b, end(cp_internal::char_buff), x, base);
    return string(b, ptr);
}
ull base_to_dec(const string &x, int base) {
    auto xc = x.c_str();
    ull val = 0;
    from_chars(xc, xc + x.size(), val, base);
    return val;
}
void zfill(string &s, size_t width) { s = string(width > s.size() ? width - s.size() : 0, '0') + s; }

/* grid */
bool grid_check(ll row, ll col, ll h, ll w) { return 0 <= row && row < h && 0 <= col && col < w; }

/* util */
#define OL_REP(_1, _2, _3, _4, name, ...) name
#define REP2(i, n) for (ll i = 0; i < ll(n); ++(i))
#define REP3(i, a, b) for (ll i = ll(a); i < ll(b); ++(i))
#define REP4(i, a, b, c) for (ll i = ll(a); i < ll(b); i += c)
#define rep(...) OL_REP(__VA_ARGS__, REP4, REP3, REP2)(__VA_ARGS__)
#define BIOP(func_name) [](auto x, auto y) { return (func_name)(x, y); };
#define CONSTF(x) [] { return (x); };

/* constants */
//[[maybe_unused]] constexpr ll MOD = 1e9 + 7;
[[maybe_unused]] constexpr ll MOD = 998244353;
[[maybe_unused]] constexpr int INF32 = (1 << 30) - 1;
[[maybe_unused]] constexpr ll INF64 = (1LL << 62) - 1;
[[maybe_unused]] const double PI = numbers::pi;

// clang-format on

int main() {
    string S;
    input(S);
    int N = ssize(S);
    int ans = 0;
    rep(i,2,N+1,2){
        rep(j,N-i+1){
            string Ssub = S.substr(j,i);
            string first = Ssub.substr(0,i/2);
            string second = Ssub.substr(i/2,i/2);
            ranges::reverse(second);
            if (first == second){
                print(first,second);
                ans++;
            }
        }
    }
    print(ans);
}