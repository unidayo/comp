#include <bits/stdc++.h>
using namespace std;
#include <atcoder/all>
using namespace atcoder;

// #define int long long
// #define endl "\n"

#ifndef _LOCAL
#pragma GCC optimize("-O3")
#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#endif

void solve();
typedef long long ll;
typedef __int128_t LL;
typedef unsigned long long ull;
typedef double db;
typedef long double ld;
typedef pair<int, int> pi;
typedef pair<int, pair<int, int>> pip;
typedef vector<int> vi;
typedef vector<double> vd;
typedef vector<bool> vb;
typedef vector<string> vs;
typedef vector<char> vc;
typedef vector<pair<int, int>> vp;
typedef vector<vector<int>> vvi;
typedef vector<vector<double>> vvd;
typedef vector<vector<bool>> vvb;
typedef vector<vector<string>> vvs;
typedef vector<vector<char>> vvc;
typedef vector<vector<pair<int, int>>> vvp;
typedef vector<vector<vector<int>>> vvvi;
typedef vector<vector<vector<vector<int>>>> vvvvi;
template <typename T>
using vec = vector<T>;
template <typename T>
using vv = vector<vector<T>>;
template <typename T>
using vvv = vector<vector<vector<T>>>;
template <typename T>
using vvvv = vector<vector<vector<vector<T>>>>;
template <typename T>
using pq = priority_queue<T>;
template <typename T>
using pqg = priority_queue<T, vector<T>, greater<T>>;
template <typename T>
using mset = multiset<T>;
template <typename T>
using uset = unordered_set<T>;
template <typename T, typename U>
using umap = unordered_map<T, U>;

#define _PI 3.14159265358979323846
#define _E 2.7182818284590452354
#define fi first
#define se second
#define pb push_back
#define eb emplace_back
#define mp make_pair
#define bg begin()
#define ed end()
#define mt make_tuple
#define td typedef
#define elif else if
#define ifnot(x) if (!(x))
#define si(x) (int)((x).size())
#define all(obj) (obj).begin(), (obj).end()
#define rall(obj) (obj).rbegin(), (obj).rend()
#define lb(v, a) (lower_bound(begin(v), end(v), a) - begin(v))
#define ub(v, a) (upper_bound(begin(v), end(v), a) - begin(v))
#define inr(l, x, r) (l <= x && x < r)
#define cbit(x) __builtin_popcountll(x)
#define tbit(t) (t == 0 ? -1 : 63 - __builtin_clzll(t))
#define bbit(t) (t == 0 ? 64 : __builtin_ctzll(t))
#define gb(msk, i) ((msk) >> (i) & 1)
#define mask(x) ((1LL << (x)) - 1)
#define setbits(i, n) \
    for (int j_ = (n), i = bbit(j_); j_; j_ ^= 1LL << i, i = bbit(j_))

#define rep1(a)                                                   \
    for (int NEVER_USE_VARIABLE = 0; NEVER_USE_VARIABLE < (int)a; \
         NEVER_USE_VARIABLE++)
#define rep2(i, a) for (int i = 0; i < (int)a; i++)
#define rep3(i, a, b) for (int i = a; i < (int)b; i++)
#define rep4(i, a, b, c) for (int i = a; i < (int)b; i += c)
#define overload4(a, b, c, d, e, ...) e
#define rep(...) overload4(__VA_ARGS__, rep4, rep3, rep2, rep1)(__VA_ARGS__)
#define rrep1(n) for (ll NEVER_USE_VARIABLE = n; NEVER_USE_VARIABLE-- > 0;)
#define rrep2(i, n) for (ll i = n; i-- > 0;)
#define rrep3(i, a, b) for (ll i = b; i-- > (a);)
#define rrep4(i, a, b, c) \
    for (ll i = (a) + ((b) - (a)-1) / (c) * (c); i >= (a); i -= c)
#define rrep(...) \
    overload4(__VA_ARGS__, rrep4, rrep3, rrep2, rrep1)(__VA_ARGS__)
#define reps(i, a) for (int i = 0; i < (int)a.size(); i++)
#define rreps(i, a) for (int i = (int)a.size() - 1; i >= 0; i--)
#define fore1(i, a) for (auto &&i : a)
#define fore2(x, y, a) for (auto &&[x, y] : a)
#define fore3(x, y, z, a) for (auto &&[x, y, z] : a)
#define fore(...) overload4(__VA_ARGS__, fore3, fore2, fore1)(__VA_ARGS__)
#define ryes return yes();
#define rno return no();
#define rerr return err();
istream &operator>>(istream &is, modint998244353 &a) {
    long long v;
    is >> v;
    a = v;
    return is;
}
ostream &operator<<(ostream &os, const modint998244353 &a) {
    return os << a.val();
}
istream &operator>>(istream &is, modint1000000007 &a) {
    long long v;
    is >> v;
    a = v;
    return is;
}
ostream &operator<<(ostream &os, const modint1000000007 &a) {
    return os << a.val();
}
// 十進数からb進数へ
template <class T>
string to_baseB(T x, int b = 10) {
    string ans;
    bool is_minus = 0;
    if (x < 0) {
        x *= -1;
        is_minus = 1;
    }
    do {
        int num = x % b;
        ans += (char)((num <= 9) ? ('0' + num) : ('A' + num - 10));
        x /= b;
    } while (x != 0);

    if (is_minus) ans += '-';
    reverse(all(ans));
    return ans;
}
// b進数から十進数へ
long long to_base10(const string &x, int b = 10) {
    long long ans = 0, base = 1;
    for (int i = x.length() - 1; i >= 0; --i) {
        int num =
            ('0' <= x[i] && x[i] <= '9') ? (x[i] - '0') : (x[i] - 'A' + 10);
        ans += base * num;
        base *= b;
    }
    return ans;
}

ostream &operator<<(ostream &s, const __int128_t &p) {
    s << to_baseB(p);
    return s;
}
istream &operator>>(istream &is, __int128_t &v) {
    string s;
    is >> s;
    v = 0;
    for (int i = 0; i < (int)s.size(); i++) {
        if (isdigit(s[i])) {
            v = v * 10 + s[i] - '0';
        }
    }
    if (s[0] == '-') {
        v *= -1;
    }
    return is;
}
template <class T, class U>
istream &operator>>(istream &is, pair<T, U> &p) {
    is >> p.first >> p.second;
    return is;
}
template <class T, class U>
ostream &operator<<(ostream &os, const pair<T, U> &p) {
    os << p.first << "," << p.second;
    return os;
}
template <class T>
ostream &operator<<(ostream &s, set<T> P) {
    fore(it, P) {
        s << it << " ";
    }
    return s;
}
template <class T1, class T2>
ostream &operator<<(ostream &s, map<T1, T2> P) {
    fore(x, y, P) {
        s << "<" << x << "->" << y << "> ";
    }
    return s;
}
template <class T>
ostream &operator<<(ostream &s, multiset<T> P) {
    fore(it, P) {
        s << it << " ";
    }
    return s;
}
template <class T>
ostream &operator<<(ostream &s, unordered_set<T> P) {
    fore(it, P) {
        s << it << " ";
    }
    return s;
}
template <class T1, class T2>
ostream &operator<<(ostream &s, unordered_map<T1, T2> P) {
    fore(x, y, P) {
        s << "<" << x << "->" << y << "> ";
    }
    return s;
}
template <class T>
istream &operator>>(istream &is, vector<T> &v) {
    for (auto &e : v) is >> e;
    return is;
}
template <class T>
ostream &operator<<(ostream &os, const vector<T> &v) {
    for (auto &e : v) os << e << ' ';
    return os;
}
template <class T>
ostream &operator<<(ostream &os, const vector<vector<T>> &v) {
    for (auto &e : v) {
        for (auto &c : e) os << c << ' ';
        os << endl;
    }
    return os;
}
template <class t, size_t n>
ostream &operator<<(ostream &os, const array<t, n> &a) {
    return os << vector<t>(all(a));
}

template <int i, class T>
void print_tuple(ostream &, const T &) {
}
template <int i, class T, class H, class... Args>
void print_tuple(ostream &os, const T &t) {
    if (i) os << ",";
    os << get<i>(t);
    print_tuple<i + 1, T, Args...>(os, t);
}
template <class... Args>
ostream &operator<<(ostream &os, const tuple<Args...> &t) {
    print_tuple<0, tuple<Args...>, Args...>(os, t);
    return os;
}
template <class T>
ostream &operator<<(ostream &os, queue<T> q) {
    while (!q.empty()) {
        os << q.front() << ' ';
        q.pop();
    }
    return os;
}
template <class T>
ostream &operator<<(ostream &os, stack<T> q) {
    while (!q.empty()) {
        os << q.top() << ' ';
        q.pop();
    }
    return os;
}
template <class T>
ostream &operator<<(ostream &os, deque<T> q) {
    while (!q.empty()) {
        os << q.front() << ' ';
        q.pop_front();
    }
    return os;
}
template <class T>
ostream &operator<<(ostream &os, priority_queue<T> q) {
    while (!q.empty()) {
        os << q.top() << ' ';
        q.pop();
    }
    return os;
}
template <class T>
ostream &operator<<(ostream &os, priority_queue<T, vector<T>, greater<T>> q) {
    while (!q.empty()) {
        os << q.top() << ' ';
        q.pop();
    }
    return os;
}
template <class T>
vector<T> &operator++(vector<T> &v) {
    for (auto &e : v) e++;
    return v;
}
template <class T>
vector<T> operator++(vector<T> &v, signed) {
    auto res = v;
    for (auto &e : v) e++;
    return res;
}
template <class T>
vector<T> &operator--(vector<T> &v) {
    for (auto &e : v) e--;
    return v;
}
template <class T>
vector<T> operator--(vector<T> &v, signed) {
    auto res = v;
    for (auto &e : v) e--;
    return res;
}
template <class T, class U>
pair<T, U> &operator+=(pair<T, U> &a, pair<T, U> b) {
    a.first += b.first;
    a.second += b.second;
    return a;
}
template <class T, class U>
pair<T, U> &operator-=(pair<T, U> &a, pair<T, U> b) {
    a.first -= b.first;
    a.second -= b.second;
    return a;
}
template <class T, class U>
pair<T, U> operator+(pair<T, U> a, pair<T, U> b) {
    return make_pair(a.first + b.first, a.second + b.second);
}
template <class T, class U>
pair<T, U> operator-(pair<T, U> a, pair<T, U> b) {
    return make_pair(a.first - b.first, a.second - b.second);
}

// debug methods
// usage: debug(x,y);
#define CHOOSE(a) CHOOSE2 a
#define CHOOSE2(a0, a1, a2, a3, a4, x, ...) x
#define debug_1(x1) cout << #x1 << ": " << x1 << endl
#define debug_2(x1, x2) \
    cout << #x1 << ": " << x1 << ", " #x2 << ": " << x2 << endl
#define debug_3(x1, x2, x3)                                                 \
    cout << #x1 << ": " << x1 << ", " #x2 << ": " << x2 << ", " #x3 << ": " \
         << x3 << endl
#define debug_4(x1, x2, x3, x4)                                             \
    cout << #x1 << ": " << x1 << ", " #x2 << ": " << x2 << ", " #x3 << ": " \
         << x3 << ", " #x4 << ": " << x4 << endl
#define debug_5(x1, x2, x3, x4, x5)                                         \
    cout << #x1 << ": " << x1 << ", " #x2 << ": " << x2 << ", " #x3 << ": " \
         << x3 << ", " #x4 << ": " << x4 << ", " #x5 << ": " << x5 << endl
#ifdef _DEBUG
#define debug(...)                                                        \
    CHOOSE((__VA_ARGS__, debug_5, debug_4, debug_3, debug_2, debug_1, ~)) \
    (__VA_ARGS__)
#else
#define debug(...)
#endif

void out() {
    cout << endl;
}
template <class T>
void out(const T &a) {
    cout << a;
    cout << endl;
}
template <class T, class... Ts>
void out(const T &a, const Ts &...b) {
    cout << a;
    (cout << ... << (cout << ' ', b));
    cout << endl;
}
void ofs_in(ostream &ofs) {
    ofs << endl;
}
template <class T>
void ofs_in(ostream &ofs, const T &a) {
    ofs << a;
    ofs << endl;
}
template <class T, class... Ts>
void ofs_in(ostream &ofs, const T &a, const Ts &...b) {
    ofs << a;
    (ofs << ... << (ofs << ' ', b));
    ofs << endl;
}
#define rout_1(x1) return out(x1)
#define rout_2(x1, x2) return out(x1, x2)
#define rout_3(x1, x2, x3) return out(x1, x2, x3)
#define rout_4(x1, x2, x3, x4) return out(x1, x2, x3, x4)
#define rout_5(x1, x2, x3, x4, x5) return out(x1, x2, x3, x4, x5)
#define rout(...)                                                    \
    CHOOSE((__VA_ARGS__, rout_5, rout_4, rout_3, rout_2, rout_1, ~)) \
    (__VA_ARGS__)
struct fast_ios {
    fast_ios() {
        cin.tie(nullptr);
        ios::sync_with_stdio(false);
        cout << fixed << setprecision(12);
    };
} fast_ios_;

template <class T = long long>
struct Binomial {
    int p;
    int MAX;
    vector<long long> fac, finv, inv;

    // テーブルを作る前処理
    Binomial(int p_, int n = 1) : p(p_), MAX(1), fac(2), finv(2), inv(2) {
        fac[0] = fac[1] = 1;
        finv[0] = finv[1] = 1;
        inv[1] = 1;
        if (n != 1) build(n);
    }

    void build(int new_max) {
        MAX++;
        fac.resize(new_max + 1);
        inv.resize(new_max + 1);
        finv.resize(new_max + 1);
        for (; MAX <= new_max; MAX++) {
            fac[MAX] = fac[MAX - 1] * MAX % p;
            inv[MAX] = p - inv[p % MAX] * (p / MAX) % p;
            finv[MAX] = finv[MAX - 1] * inv[MAX] % p;
        }
        MAX--;
    }

    // nCk
    T mod_comb(int n, int k) {
        if (n < k) return 0;
        if (n < 0 || k < 0) return 0;
        if (n > MAX) build(n);
        return fac[n] * (finv[k] * finv[n - k] % p) % p;
    }
    T operator()(int n, int k) {
        return mod_comb(n, k);
    }

    // nPk
    T mod_perm(int n, int k) {
        if (n < k) return 0;
        if (n < 0 || k < 0) return 0;
        if (n > MAX) build(n);
        return fac[n] * finv[n - k] % p;
    }
    // n!
    T operator[](int n) {
        if (n > MAX) build(n);
        return fac[n];
    }
    // 1/n!
    T operator()(int n) {
        if (n > MAX) build(n);
        return finv[n];
    }
};

template <typename T = long long>
struct modpow {
    long long x, m;
    int n;
    vector<T> d;
    modpow(long long x) : x(x), n(1), d(1, 1) {
    }
    T operator[](int i) {
        while (n <= i) d.push_back(d.back() * x), ++n;
        return d[i];
    }
};
modpow two(2), ten(10);

/*
座標圧縮
https://youtu.be/fR3W5IcBGLQ?t=8550
を参考に
(T x):xが何番目か
[T i]:i番目の値
*/
template <typename T = int>
struct CC {
    bool initialized;
    vector<T> xs;
    CC() : initialized(false) {
    }
    CC(vector<T> v) : initialized(false) {
        for (auto x : v) xs.push_back(x);
    }
    void add(T x) {
        xs.push_back(x);
    }
    void add(vector<T> v) {
        for (auto x : v) xs.push_back(x);
    }
    void init() {
        sort(xs.begin(), xs.end());
        xs.erase(unique(xs.begin(), xs.end()), xs.end());
        initialized = true;
    }
    int operator()(T x) {
        if (!initialized) init();
        return upper_bound(xs.begin(), xs.end(), x) - xs.begin() - 1;
    }
    T operator[](int i) {
        if (!initialized) init();
        return xs[i];
    }
    int size() {
        if (!initialized) init();
        return xs.size();
    }
    friend ostream &operator<<(ostream &os, const CC &cc) {
        for (int i = 0; i < (int)cc.xs.size(); i++) {
            os << cc.xs[i] << " ";
        }
        os << endl;
        return (os);
    }
};

struct RandomNumberGenerator {
    mt19937 engine;

    RandomNumberGenerator(int seed = -1) {
        if (seed == -1)
            engine =
                mt19937(chrono::steady_clock::now().time_since_epoch().count());
        else
            engine = mt19937(seed);
    }

    long long operator()(long long a, long long b) {  // [a, b)
        uniform_int_distribution<long long> dist(a, b - 1);
        return dist(engine);
    }

    long long operator()(long long b) {  // [0, b)
        return (*this)(0, b);
    }

    long long operator()() {
        return (*this)(0, 1LL << 60);
    }

    double operator[](double a) {
        return (double)(*this)(0, 1LL << 60) / (1LL << 60) * a;
    }

    double normal_dist(double sigma, double mean = 0) {
        std::normal_distribution<> dist(mean, sigma);
        return dist(engine);
    }
} rnd;

clock_t start_time = clock();
double now_time() {
    clock_t end_time = clock();
    return (double)(end_time - start_time) / CLOCKS_PER_SEC;
}
template <class T = int, size_t n, size_t idx = 0>
auto mv(const size_t (&d)[n], const T &init) noexcept {
    if constexpr (idx < n)
        return std::vector(d[idx], mv<T, n, idx + 1>(d, init));
    else
        return init;
}
template <class T = int, size_t n>
auto mv(const size_t (&d)[n]) noexcept {
    return mv(d, T{});
}
template <class T>
void rnd_shuffle(vector<T> &v) {
    shuffle(v.begin(), v.end(), rnd.engine);
}
template <class T>
T sample(T v, int n) {
    T res;
    sample(v.begin(), v.end(), back_inserter(res), n, rnd.engine);
    return res;
}
template <class F>
long long bin_search(long long ok, long long ng, const F &f) {
    while (abs(ok - ng) > 1) {
        long long mid = (ok + ng) >> 1;
        (f(mid) ? ok : ng) = mid;
    }
    return ok;
}
template <class T, class F>
T bin_search_double(T ok, T ng, const F &f, int iter = 90) {
    while (iter--) {
        T mid = (ok + ng) / 2;
        (f(mid) ? ok : ng) = mid;
    }
    return ok;
}
void read_edges(vector<vector<int>> &g, int m = -1, int bidirected = true) {
    if (m == -1) m = g.size() - 1;
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--;
        v--;
        g[u].push_back(v);
        if (bidirected) g[v].push_back(u);
    }
}
vector<int> counter(vector<int> &v, int mx = -1) {
    if (mx == -1) mx = *max_element(v.begin(), v.end());
    vector<int> res(mx + 1);
    for (auto x : v) res[x]++;
    return res;
}
template <class T = int, class U = vector<T>>
map<T, int> map_counter(U &v) {
    map<T, int> res;
    for (auto x : v) res[x]++;
    return res;
}
vector<int> iota(int n, int s = 0) {
    vi a(n);
    iota(a.begin(), a.end(), s);
    return a;
}
template <class T>
void sort(T &v) {
    sort(all(v));
}
template <class T>
void rsort(T &v) {
    sort(rall(v));
}
template <class T>
void reverse(T &v) {
    reverse(all(v));
}
template <class T>
auto max(const T &a) {
    return *max_element(a.begin(), a.end());
}
template <class T>
auto min(const T &a) {
    return *min_element(a.begin(), a.end());
}
template <class T>
int argmax(const T &a) {
    return max_element(a.begin(), a.end()) - a.begin();
}
template <class T>
int argmin(const T &a) {
    return min_element(a.begin(), a.end()) - a.begin();
}
long long max(signed x, long long y) {
    return max((long long)x, y);
}
long long max(long long x, signed y) {
    return max(x, (long long)y);
}
long long min(signed x, long long y) {
    return min((long long)x, y);
}
long long min(long long x, signed y) {
    return min(x, (long long)y);
}
template <class T, class S>
bool chmax(T &a, const S &b) {
    if (a < (T)b) {
        a = (T)b;
        return 1;
    }
    return 0;
}
template <class T, class S>
bool chmin(T &a, const S &b) {
    if ((T)b < a) {
        a = (T)b;
        return 1;
    }
    return 0;
}
template <class T>
vector<int> argsort(vector<T> v, bool ascending_order = true) {
    vector<int> res(v.size());
    iota(res.begin(), res.end(), 0);
    if (ascending_order)
        sort(res.begin(), res.end(), [&](int i, int j) { return v[i] < v[j]; });
    else
        sort(res.begin(), res.end(), [&](int i, int j) { return v[i] > v[j]; });
    return res;
}
template <class T>
T sumv(vector<T> &v) {
    T res = 0;
    int n = v.size();
    for (int i = 0; i < n; i++) res += v[i];
    return res;
}
template <class T>
vector<T> uniq(vector<T> v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    return v;
}
template <class T>
vector<T> compress(vector<T> v) {
    vector<T> v2(v.size());
    v2 = v;
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());

    for (int i = 0; i < (int)v2.size(); i++) {
        v2[i] = lower_bound(v.begin(), v.end(), v2[i]) - v.begin();
    }
    return v2;
}
vector<int> inverse(vector<int> &p) {
    int n = p.size();
    vector<int> inv(n);
    for (int i = 0; i < n; i++) inv[p[i]] = i;
    return inv;
}
template <typename T>
vector<pair<T, int>> idx_pair(vector<T> &a) {
    int n = a.size();
    vector<pair<T, int>> res(n);
    for (int i = 0; i < n; i++) res[i] = {a[i], i};
    return res;
}
template <typename T>
vector<T> acc0(vector<T> &v) {
    vector<T> res(v.size());
    if ((int)v.size() == 0) return res;
    res[0] = v[0];
    for (int i = 1; i < (int)v.size(); i++) {
        res[i] = res[i - 1] + v[i];
    }
    return res;
}
template <typename T>
vector<T> acc1(vector<T> &v) {
    vector<T> res(v.size() + 1);
    for (int i = 0; i < (int)v.size(); i++) {
        res[i + 1] = res[i] + v[i];
    }
    return res;
}
template <typename T>
vector<vector<T>> acc0(vector<vector<T>> v) {
    int h = v.size(), w = v[0].size();
    for (int i = 0; i < h; i++) {
        for (int j = 1; j < w; j++) {
            v[i][j] += v[i][j - 1];
        }
    }
    for (int i = 1; i < h; i++) {
        for (int j = 0; j < w; j++) {
            v[i][j] += v[i - 1][j];
        }
    }
    return v;
}
template <typename T>
vector<vector<T>> acc1(vector<vector<T>> &v) {
    int h = v.size(), w = v[0].size();
    vector<vector<T>> res(h + 1, vector<T>(w + 1));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            res[i + 1][j + 1] = v[i][j] + res[i + 1][j];
        }
    }
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            res[i + 1][j + 1] += res[i][j + 1];
        }
    }
    return res;
}

long long exp(long long x, int n) {
    long long res = 1;
    while (n > 0) {
        if (n & 1) res = res * x;
        x = x * x;
        n >>= 1;
    }
    return res;
}
template <class T>
T ABS(const T &x) {
    return x > 0 ? x : -x;
}
bool ispow2(int i) {
    return i && (i & -i) == i;
}
int countDigits(long long n) {
    string tmp = to_string(n);
    return (int)tmp.size();
}
template <class T>
T sq(T n) {
    return n * n;
}
long long ceil(long long x, long long y) {
    return (x + y - 1) / y;
}
long long floor(long long x, long long y) {
    return (y < 0 ? floor(-x, -y)
                  : (x > 0 ? x / y : x / y - (x % y == 0 ? 0 : 1)));
}
constexpr long long tri(long long n) {
    return n * (n + 1) / 2;
}
// l + ... + r
constexpr long long tri(long long l, long long r) {
    return (l + r) * (r - l + 1) / 2;
}
template <typename T>
T modulo(T n, T d) {
    return (n % d + d) % d;
}
int ctoi(const char &c, const char start = '0') {
    return c - start;
}
int atoi(const char &c, const char start = 'a') {
    return c - start;
}
vector<int> ctoi(string &s, const char start = '0') {
    vector<int> res;
    for (auto &c : s) {
        int x = c - start;
        if (x < 0 || x >= 10) x = -1;
        res.push_back(x);
    }
    return res;
}
vector<int> atoi(string &s, const char start = 'a') {
    vector<int> res;
    for (auto &c : s) {
        int x = c - start;
        if (x < 0 || x >= 26) x = -1;
        res.push_back(x);
    }
    return res;
}
vector<vector<int>> ctoi(vector<string> &s, const char start = '0') {
    int n = s.size();
    vector<vector<int>> res(n);
    for (int i = 0; i < n; i++) res[i] = ctoi(s[i], start);
    return res;
}
vector<vector<int>> atoi(vector<string> &s, const char start = 'a') {
    int n = s.size();
    vector<vector<int>> res(n);
    for (int i = 0; i < n; i++) res[i] = atoi(s[i], start);
    return res;
}
string itoc(vector<int> &v, const char start = '0') {
    int n = v.size();
    string res;
    for (int i = 0; i < n; i++) {
        res.push_back(start + v[i]);
    }
    return res;
}
string itoa(vector<int> &v, const char start = 'a') {
    return itoc(v, start);
}
vector<string> itoc(vector<vector<int>> &v, const char start = '0') {
    int n = v.size();
    vector<string> res(n);
    for (int i = 0; i < n; i++) {
        int m = v[i].size();
        for (int j = 0; j < m; j++) {
            res[i] = itoc(v[i], start);
        }
    }
    return res;
}
vector<string> itoa(vector<vector<int>> &v, const char start = 'a') {
    return itoc(v, start);
}
template <class T>
int mex(T &a) {
    int n = a.size();
    vector<int> cnt(n + 1);
    for (auto x : a) {
        if (x > n) continue;
        cnt[x]++;
    }
    int res = 0;
    while (cnt[res]) res++;
    return res;
}
void yes() {
    cout << "Yes" << endl;
    // cout << "Alice" << endl;
    // cout << "Takahashi" << endl;
}
void no() {
    cout << "No" << endl;
    // cout << "Bob" << endl;
    // cout << "Aoki" << endl;
}
void yesno(bool x) {
    if (x)
        yes();
    else
        no();
}
void err() {
    cout << -1 << endl;
}

int dx[] = {1, 0, -1, 0, 1, 1, -1, -1};
int dy[] = {0, 1, 0, -1, -1, 1, 1, -1};

long long inf = (1 << 30) + (1LL << 60) - 2;
double eps = 1e-9;

// long long mod = 67280421310721;
// using mint = static_modint<1000000009>;
// using mint = dynamic_modint<1000000009>;
// long long mod = 1000000007;
// using mint = modint1000000007;
int mod = 998244353;
// using mint = modint998244353;
// typedef vector<mint> vm;
// typedef vector<vector<mint>> vvm;
// typedef vector<vector<vector<mint>>> vvvm;
// Binomial<mint> C(mod);
// modpow<mint> mtwo(2), mten(10);
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

signed main() {
    int testcase = 1;
    // cin >> testcase;
    for (int i = 0; i < testcase; i++) {
        solve();
    }
}
const int n = 9, m = 20;
const int beam_size = 1000;
int a[n][n];
int s[20][3][3];

struct Board {
    ll score;
    int rem;
    vvi b;
    vector<tuple<int, int, int>> ans;
    Board() {
    }
    Board(ll score_, int rem_, vvi &b_) : score(score_), rem(rem_), b(b_) {
    }
    Board(const Board &other) {
        score = other.score;
        rem = other.rem;
        b = other.b;
        ans = other.ans;
    }
    bool operator<(const Board &other) const {
        // return score + ten[8] * rem < other.score + ten[8] * other.rem;
        return score < other.score;
    }
    bool operator>(const Board &other) const {
        return !(*this < other);
    }
    void calc_score(int i, int j) {
        if (j == 0) {
            rep(x, 2) {
                swap(b[x], b[x + 1]);
            }
            rep(x, n) {
                b[2][x] = a[i + 2][x];
            }
        }
        int ri = 1, rj = j + 1;
        if (i == n - 3) ri = 3;
        if (j == n - 3) rj = n;
        rep(x, ri) rep(y, j, rj) score += b[x][y];
    }
    void add(int id, int i, int j) {
        rem--;
        ans.emplace_back(id, i, j);
        int ri = 1, rj = j + 1;
        if (i == n - 3) ri = 3;
        if (j == n - 3) rj = n;
        rep(x, ri) rep(y, j, rj) {
            score -= b[x][y];
        }
        rep(x, 3) rep(y, 3) {
            b[x][j + y] += s[id][x][y];
            if (b[x][j + y] >= mod) b[x][j + y] -= mod;
        }
        rep(x, ri) rep(y, j, rj) {
            score += b[x][y];
        }
    }
    void del(int id, int i, int j) {
        rem++;
        ans.pop_back();
        int ri = 1, rj = j + 1;
        if (i == n - 3) ri = 3;
        if (j == n - 3) rj = n;
        rep(x, ri) rep(y, j, rj) {
            score -= b[x][y];
        }
        rep(x, 3) rep(y, 3) {
            b[x][j + y] -= s[id][x][y];
            if (b[x][j + y] < 0) b[x][j + y] += mod;
        }
        rep(x, ri) rep(y, j, rj) {
            score += b[x][y];
        }
    }
};
int num[7][7];
void solve() {
    {
        int tmp;
        cin >> tmp >> tmp;
    }
    int k;
    cin >> k;
    rep(i, n) rep(j, n) cin >> a[i][j];
    rep(i, m) {
        rep(j, 3) rep(l, 3) cin >> s[i][j][l];
    }
    vvi init_b(3, vi(n));
    rep(i, 2) rep(j, n) init_b[i + 1][j] = a[i][j];
    Board init_board(0, k, init_b);
    pq<Board> q;
    q.emplace(init_board);
    vector<tuple<int, int, int>> ans;
    ll best_score = 0;
    {
        int cur = 81;
        rep(i, n - 2) {
            rep(j, n - 2) {
                if (j == n - 3 || i == n - 3)
                    cur -= 3;
                else
                    cur -= 1;
                num[i][j] = cur;
            }
            cur--;
        }
    }

    rep(i, n - 2) {
        rep(j, n - 2) {
            pq<Board> nq;
            int cnt = 0;
            pqg<ll> scores;
            auto push = [&](Board &b) {
                if (si(scores) == beam_size) {
                    if (scores.top() >= b.score) return;
                    scores.push(b.score);
                    scores.pop();
                } else {
                    scores.push(b.score);
                }
                nq.push(b);
            };
            while (q.size() && cnt < beam_size) {
                Board b = q.top();
                q.pop();
                cnt++;
                b.calc_score(i, j);
                nq.push(b);
                if (i == n - 3 && j == n - 3) {
                    if (chmax(best_score, b.score)) {
                        ans = b.ans;
                    }
                    int r = b.rem;
                    debug(r);
                    if (b.rem <= 3) {
                        if (b.rem) {
                            rep(x, m) {
                                b.add(x, i, j);
                                if (chmax(best_score, b.score)) {
                                    ans = b.ans;
                                }
                                if (b.rem) {
                                    rep(y, x + 1, m) {
                                        b.add(y, i, j);
                                        if (chmax(best_score, b.score)) {
                                            ans = b.ans;
                                        }
                                        if (b.rem) {
                                            rep(z, y + 1, m) {
                                                b.add(z, i, j);
                                                if (chmax(best_score,
                                                          b.score)) {
                                                    ans = b.ans;
                                                }
                                                b.del(z, i, j);
                                            }
                                        }
                                        b.del(y, i, j);
                                    }
                                }
                                b.del(x, i, j);
                            }
                        }
                    } else {
                        rep(x, 1500) {
                            Board nb(b);
                            rep(ri, r) {
                                int nm = rnd(m);
                                nb.add(nm, i, j);
                                if (chmax(best_score, nb.score)) {
                                    ans = nb.ans;
                                }
                            }
                        }
                    }
                }
                elif (i == n - 3 || j == n - 3) {
                    if (b.rem) {
                        rep(x, m) {
                            b.add(x, i, j);
                            push(b);
                            if (b.rem) {
                                rep(y, x + 1, m) {
                                    b.add(y, i, j);
                                    push(b);
                                    if (b.rem >= num[i][j]) {
                                        rep(z, y + 1, m) {
                                            b.add(z, i, j);
                                            push(b);
                                            b.del(z, i, j);
                                        }
                                    }
                                    b.del(y, i, j);
                                }
                            }
                            b.del(x, i, j);
                        }
                    }
                }
                else {
                    if (b.rem) {
                        rep(x, m) {
                            b.add(x, i, j);
                            push(b);
                            if (b.rem >= num[i][j]) {
                                rep(y, x + 1, m) {
                                    b.add(y, i, j);
                                    push(b);
                                    b.del(y, i, j);
                                }
                            }
                            b.del(x, i, j);
                        }
                    }
                }
            }
            swap(q, nq);
        }
    }
    // debug(best_score);
    // debug(now_time());
    out(ans.size());
    for (auto [m_, i_, j_] : ans) out(m_, i_, j_);
}