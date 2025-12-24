// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

#include <bits/stdc++.h>

#include <atcoder/all>

namespace atcoder {};
namespace cp_internal {
char char_buff[64];
};

/* namespace */
using namespace std;
using namespace atcoder;

/* type */
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;
using vull = __uint128_t;

/* ios*/
struct fast_ios {
    fast_ios() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        cout << fixed << setprecision(15);
    }
} fast_ios;

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

/* functions */
template <ranges::forward_range Range, class T>
ll lower_count(const Range &r, T x) {
    return ranges::distance(begin(r), ranges::lower_bound(r, x));
}
template <ranges::forward_range Range, class T>
ll upper_count(const Range &r, T x) {
    return ranges::distance(begin(r), ranges::upper_bound(r, x));
}

template <class T>
bool chmin(T &a, T b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}
template <class T>
bool chmax(T &a, T b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}

template <ranges::range Range, class T>
map<T, ll> compression(Range r) {
    ranges::sort(r);
    auto res = ranges::unique(r);
    r.erase(begin(res), end(res));
    map<T, ll> ret;
    for (ll i = 0; auto v : r) {
        ret[v] = i;
        ++i;
    }
    return ret;
}

template <ranges::range Range, class T>
vector<T> accumulate_sum(const Range &r) {
    vector<T> res(ssize(r));
    partial_sum(begin(r), end(r), begin(res));
    return res;
}

template <ranges::range Range, class T>
vector<T> accumulate_sum(const Range &r, T init) {
    vector<T> res(ssize(r) + 1);
    res[0] = init;
    inclusive_scan(begin(r), end(r), begin(res) + 1, plus<>(), init);
    return res;
}

template <ranges::range Range, class T>
vector<vector<T>> lpartition(const Range &r, int w) {
    int n = ssize(r);
    int h = (n + w - 1) / w;
    vector<vector<T>> ret(h, vector<T>(w, T()));
    for (int i = 0; i < n; i += w) {
        ret[i / w][i % w] = r[i];
    }
    return ret;
}

template <ranges::random_access_range Range>
auto runlength(const Range &R) {
    vector<pair<ranges::range_value_t<Range>, ll>> ret;
    for (ll l = 0; l < ssize(R);) {
        ll r = l + 1;
        for (; r < ssize(R) && R[l] == R[r]; ++r) {
        }
        ret.emplace_back(R[l], r - l);
        l = r;
    }
    return ret;
}

template <class T>
void remove_duplicates(vector<T> &v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
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

/* io */
template <class... T>
void input(T &...a) {
    (cin >> ... >> a);
}
template <class T>
void input(vector<T> &v) {
    ranges::for_each(v, [](T &x) { cin >> x; });
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

/* util */
#define OL_REP(_1, _2, _3, _4, name, ...) name
#define REP2(i, n) for (ll i = 0; i < ll(n); ++(i))
#define REP3(i, a, b) for (ll i = ll(a); i < ll(b); ++(i))
#define REP4(i, a, b, c) for (ll i = ll(a); i < ll(b); i += c)
#define rep(...) OL_REP(__VA_ARGS__, REP4, REP3, REP2)(__VA_ARGS__)

/* constants */
//[[maybe_unused]] constexpr ll MOD = 1e9 + 7;
[[maybe_unused]] constexpr ll MOD = 998244353;
[[maybe_unused]] constexpr int INF32 = (1 << 30) - 1;
[[maybe_unused]] constexpr ll INF64 = (1LL << 62) - 1;
[[maybe_unused]] const double PI = numbers::pi;

template <ranges::range Range>
auto sum_of(Range R) {
    return reduce(begin(R), end(R));
}
template <ranges::range Range, class T>
T sum_of(Range R) {
    return reduce(begin(R), end(R), T());
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

int main() {
    int N,M;
    input(N,M);
    ll ans=0;
    vector<string> C(N);
    vector<string> D(M);
    vector<int> P(M+1);
    input(C);
    input(D);
    input(P);
    map<string,int> mp;
    rep(i,M){
        mp[D[i]] = P[i+1];
    }
    rep(i,N){
        if(mp.contains(C[i])){
            ans += mp[C[i]];
        } else {
            ans+=P[0];
        }
    }
    print(ans);
}