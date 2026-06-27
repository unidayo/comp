// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")
// #pragma GCC optimize("unroll-loops")
#define INTERACTIVE

#include <bits/stdc++.h>
using namespace std;

namespace templates {
// type
using ll  = long long;
using ull = unsigned long long;
using Pii = pair<int, int>;
using Pil = pair<int, ll>;
using Pli = pair<ll, int>;
using Pll = pair<ll, ll>;
template <class T>
using pq = priority_queue<T>;
template <class T>
using qp = priority_queue<T, vector<T>, greater<T>>;
// clang-format off
#define vec(T, A, ...) vector<T> A(__VA_ARGS__);
#define vvec(T, A, h, ...) vector<vector<T>> A(h, vector<T>(__VA_ARGS__));
#define vvvec(T, A, h1, h2, ...) vector<vector<vector<T>>> A(h1, vector<vector<T>>(h2, vector<T>(__VA_ARGS__)));
// clang-format on

// for loop
#define fori1(a) for (ll _ = 0; _ < (a); _++)
#define fori2(i, a) for (ll i = 0; i < (a); i++)
#define fori3(i, a, b) for (ll i = (a); i < (b); i++)
#define fori4(i, a, b, c) for (ll i = (a); ((c) > 0 || i > (b)) && ((c) < 0 || i < (b)); i += (c))
#define overload4(a, b, c, d, e, ...) e
#define fori(...) overload4(__VA_ARGS__, fori4, fori3, fori2, fori1)(__VA_ARGS__)

// declare and input
// clang-format off
#define INT(...) int __VA_ARGS__; inp(__VA_ARGS__);
#define LL(...) ll __VA_ARGS__; inp(__VA_ARGS__);
#define STRING(...) string __VA_ARGS__; inp(__VA_ARGS__);
#define CHAR(...) char __VA_ARGS__; inp(__VA_ARGS__);
#define DOUBLE(...) double __VA_ARGS__; STRING(str___); __VA_ARGS__ = stod(str___);
#define VEC(T, A, n) vector<T> A(n); inp(A);
#define VVEC(T, A, n, m) vector<vector<T>> A(n, vector<T>(m)); inp(A);
// clang-format on

// const value
const ll MOD1   = 1000000007;
const ll MOD9   = 998244353;
const double PI = acos(-1);

// other macro
#if !defined(RIN__LOCAL) && !defined(INTERACTIVE)
#define endl "\n"
#endif
#define spa ' '
#define len(A) ll(A.size())
#define all(A) begin(A), end(A)

// function
vector<char> stoc(string &S) {
    int n = S.size();
    vector<char> ret(n);
    for (int i = 0; i < n; i++) ret[i] = S[i];
    return ret;
}
string ctos(vector<char> &S) {
    int n      = S.size();
    string ret = "";
    for (int i = 0; i < n; i++) ret += S[i];
    return ret;
}

template <class T>
auto min(const T &a) {
    return *min_element(all(a));
}
template <class T>
auto max(const T &a) {
    return *max_element(all(a));
}
template <class T, class S>
auto clamp(T &a, const S &l, const S &r) {
    return (a > r ? r : a < l ? l : a);
}
template <class T, class S>
inline bool chmax(T &a, const S &b) {
    return (a < b ? a = b, 1 : 0);
}
template <class T, class S>
inline bool chmin(T &a, const S &b) {
    return (a > b ? a = b, 1 : 0);
}
template <class T, class S>
inline bool chclamp(T &a, const S &l, const S &r) {
    auto b = clamp(a, l, r);
    return (a != b ? a = b, 1 : 0);
}

template <typename T>
T sum(vector<T> &A) {
    T tot = 0;
    for (auto a : A) tot += a;
    return tot;
}

template <typename T>
vector<T> compression(vector<T> X) {
    sort(all(X));
    X.erase(unique(all(X)), X.end());
    return X;
}

// input and output
namespace io {
// __int128_t
std::ostream &operator<<(std::ostream &dest, __int128_t value) {
    std::ostream::sentry s(dest);
    if (s) {
        __uint128_t tmp = value < 0 ? -value : value;
        char buffer[128];
        char *d = std::end(buffer);
        do {
            --d;
            *d = "0123456789"[tmp % 10];
            tmp /= 10;
        } while (tmp != 0);
        if (value < 0) {
            --d;
            *d = '-';
        }
        int len = std::end(buffer) - d;
        if (dest.rdbuf()->sputn(d, len) != len) {
            dest.setstate(std::ios_base::badbit);
        }
    }
    return dest;
}

// vector<T>
template <typename T>
istream &operator>>(istream &is, vector<T> &A) {
    for (auto &a : A) is >> a;
    return is;
}
template <typename T>
ostream &operator<<(ostream &os, vector<T> &A) {
    for (size_t i = 0; i < A.size(); i++) {
        os << A[i];
        if (i != A.size() - 1) os << ' ';
    }
    return os;
}

// vector<vector<T>>
template <typename T>
istream &operator>>(istream &is, vector<vector<T>> &A) {
    for (auto &a : A) is >> a;
    return is;
}
template <typename T>
ostream &operator<<(ostream &os, vector<vector<T>> &A) {
    for (size_t i = 0; i < A.size(); i++) {
        os << A[i];
        if (i != A.size() - 1) os << endl;
    }
    return os;
}

// pair<S, T>
template <typename S, typename T>
istream &operator>>(istream &is, pair<S, T> &A) {
    is >> A.first >> A.second;
    return is;
}
template <typename S, typename T>
ostream &operator<<(ostream &os, pair<S, T> &A) {
    os << A.first << ' ' << A.second;
    return os;
}

// vector<pair<S, T>>
template <typename S, typename T>
istream &operator>>(istream &is, vector<pair<S, T>> &A) {
    for (size_t i = 0; i < A.size(); i++) {
        is >> A[i];
    }
    return is;
}
template <typename S, typename T>
ostream &operator<<(ostream &os, vector<pair<S, T>> &A) {
    for (size_t i = 0; i < A.size(); i++) {
        os << A[i];
        if (i != A.size() - 1) os << endl;
    }
    return os;
}

// tuple
template <typename T, size_t N>
struct TuplePrint {
    static ostream &print(ostream &os, const T &t) {
        TuplePrint<T, N - 1>::print(os, t);
        os << ' ' << get<N - 1>(t);
        return os;
    }
};
template <typename T>
struct TuplePrint<T, 1> {
    static ostream &print(ostream &os, const T &t) {
        os << get<0>(t);
        return os;
    }
};
template <typename... Args>
ostream &operator<<(ostream &os, const tuple<Args...> &t) {
    TuplePrint<decltype(t), sizeof...(Args)>::print(os, t);
    return os;
}

// io functions
void FLUSH() {
    cout << flush;
}

void print() {
    cout << endl;
}
template <class Head, class... Tail>
void print(Head &&head, Tail &&...tail) {
    cout << head;
    if (sizeof...(Tail)) cout << spa;
    print(std::forward<Tail>(tail)...);
}

template <typename T, typename S>
void prisep(vector<T> &A, S sep) {
    int n = A.size();
    for (int i = 0; i < n; i++) {
        cout << A[i];
        if (i != n - 1) cout << sep;
    }
    cout << endl;
}
template <typename T, typename S>
void priend(T A, S end) {
    cout << A << end;
}
template <typename T>
void prispa(T A) {
    priend(A, spa);
}
template <typename T, typename S>
bool printif(bool f, T A, S B) {
    if (f)
        print(A);
    else
        print(B);
    return f;
}

template <class... T>
void inp(T &...a) {
    (cin >> ... >> a);
}

} // namespace io
using namespace io;

// read graph
vector<vector<int>> read_edges(int n, int m, bool direct = false, int indexed = 1) {
    vector<vector<int>> edges(n, vector<int>());
    for (int i = 0; i < m; i++) {
        INT(u, v);
        u -= indexed;
        v -= indexed;
        edges[u].push_back(v);
        if (!direct) edges[v].push_back(u);
    }
    return edges;
}
vector<vector<int>> read_tree(int n, int indexed = 1) {
    return read_edges(n, n - 1, false, indexed);
}

template <typename T = long long>
vector<vector<pair<int, T>>> read_wedges(int n, int m, bool direct = false, int indexed = 1) {
    vector<vector<pair<int, T>>> edges(n, vector<pair<int, T>>());
    for (int i = 0; i < m; i++) {
        INT(u, v);
        T w;
        inp(w);
        u -= indexed;
        v -= indexed;
        edges[u].push_back({v, w});
        if (!direct) edges[v].push_back({u, w});
    }
    return edges;
}
template <typename T = long long>
vector<vector<pair<int, T>>> read_wtree(int n, int indexed = 1) {
    return read_wedges<T>(n, n - 1, false, indexed);
}

// yes / no
namespace yesno {

// yes
inline bool yes(bool f = true) {
    cout << (f ? "yes" : "no") << endl;
    return f;
}
inline bool Yes(bool f = true) {
    cout << (f ? "Yes" : "No") << endl;
    return f;
}
inline bool YES(bool f = true) {
    cout << (f ? "YES" : "NO") << endl;
    return f;
}

// no
inline bool no(bool f = true) {
    cout << (!f ? "yes" : "no") << endl;
    return f;
}
inline bool No(bool f = true) {
    cout << (!f ? "Yes" : "No") << endl;
    return f;
}
inline bool NO(bool f = true) {
    cout << (!f ? "YES" : "NO") << endl;
    return f;
}

// possible
inline bool possible(bool f = true) {
    cout << (f ? "possible" : "impossible") << endl;
    return f;
}
inline bool Possible(bool f = true) {
    cout << (f ? "Possible" : "Impossible") << endl;
    return f;
}
inline bool POSSIBLE(bool f = true) {
    cout << (f ? "POSSIBLE" : "IMPOSSIBLE") << endl;
    return f;
}

// impossible
inline bool impossible(bool f = true) {
    cout << (!f ? "possible" : "impossible") << endl;
    return f;
}
inline bool Impossible(bool f = true) {
    cout << (!f ? "Possible" : "Impossible") << endl;
    return f;
}
inline bool IMPOSSIBLE(bool f = true) {
    cout << (!f ? "POSSIBLE" : "IMPOSSIBLE") << endl;
    return f;
}

// Alice Bob
inline bool Alice(bool f = true) {
    cout << (f ? "Alice" : "Bob") << endl;
    return f;
}
inline bool Bob(bool f = true) {
    cout << (f ? "Bob" : "Alice") << endl;
    return f;
}

// Takahashi Aoki
inline bool Takahashi(bool f = true) {
    cout << (f ? "Takahashi" : "Aoki") << endl;
    return f;
}
inline bool Aoki(bool f = true) {
    cout << (f ? "Aoki" : "Takahashi") << endl;
    return f;
}

} // namespace yesno
using namespace yesno;

} // namespace templates
using namespace templates;

struct RandomNumberGenerator {
    mt19937 mt;
    RandomNumberGenerator() : mt(chrono::steady_clock::now().time_since_epoch().count()) {}
    RandomNumberGenerator(int seed) : mt(seed) {}

    int operator()(int a, int b) {
        uniform_int_distribution<int> dist(a, b - 1);
        return dist(mt);
    }

    int operator()(int b) {
        return (*this)(0, b);
    }

    int normal_distribution(int a, int b) {
        std::normal_distribution<> dist(a, b);
        return dist(mt);
    }

    template <typename T>
    void shuffle(vector<T> &v) {
        std::shuffle(v.begin(), v.end(), mt);
    }
};

struct State {
    ll score;
    vector<vector<int>> A;
    vector<bool> used;

    friend bool operator<(const State &a, const State &b) {
        return a.score < b.score;
    }

    friend bool operator>(const State &a, const State &b) {
        return a.score > b.score;
    }
};

void solve() {
    RandomNumberGenerator rng(42);
    INT(n, m, T);
    int nn = 2 * n * (n - 1);
    VVEC(int, X, nn, m);

    vec(int, top, m, 0);
    vec(int, top_c, m, 0);
    fori(i, nn) fori(j, m) {
        if (chmax(top[j], X[i][j])) {
            top_c[j] = 1;
        } else if (top[j] == X[i][j]) {
            top_c[j]++;
        }
    }

    auto max_sum = [&](int i, int j, int t) {
        int tot          = 0;
        double top_count = 0;
        fori(k, m) {
            tot += pow(max(X[i][k], X[j][k]), (1 + (15 - t) / 15)) * 10 + min(X[i][k], X[j][k]);
            if (max(X[i][k], X[j][k]) == top[k]) top_count += 1.0 / sqrt(top_c[k]);
        }
        return top_count * 10000000 + tot;
    };

    auto f = [&](int t) {
        const int beam_w = 30;
        int u            = -1;
        int v            = -1;
        int ma           = -1;
        using Ti3        = tuple<int, int, int>;
        qp<Ti3> init_hq;
        fori(i, nn) fori(j, i + 1, nn) {
            init_hq.push({max_sum(i, j, t), i, j});
            if (init_hq.size() > beam_w) init_hq.pop();
        }

        vec(bool, used, nn, false);
        vvec(int, out, n, n, -1);
        qp<State> state_hq;
        while (init_hq.size()) {
            auto [score, i, j] = init_hq.top();
            init_hq.pop();
            used[i]   = true;
            used[j]   = true;
            out[2][2] = i;
            out[2][3] = j;
            state_hq.push({score, out, used});
            used[i] = false;
            used[j] = false;
        }

        vvvec(Pii, nex, n, n);
        fori(i, n) fori(j, n) {
            if (i != 0) nex[i][j].push_back({i - 1, j});
            if (i != n - 1) nex[i][j].push_back({i + 1, j});
            if (j != 0) nex[i][j].push_back({i, j - 1});
            if (j != n - 1) nex[i][j].push_back({i, j + 1});

            sort(all(nex[i][j]), [&](Pii a, Pii b) {
                int aa = min({a.first, n - 1 - a.first, a.second, n - 1 - a.second});
                int bb = min({b.first, n - 1 - b.first, b.second, n - 1 - b.second});
                return aa < bb;
            });
        }

        fori(n * n - 2) {
            qp<State> next_hq;
            vec(State, states, 0);
            while (!state_hq.empty()) {
                auto [score, A, used] = state_hq.top();
                state_hq.pop();
                states.push_back({score, A, used});
            }
            reverse(all(states));
            set<pair<vector<vector<int>>, vector<bool>>> used_set;
            for (auto &state : states) {
                auto [score, out, used] = state;
                if (used_set.count({out, used})) continue;
                used_set.insert({out, used});
                fori(i, n) fori(j, n) {
                    if (out[i][j] != -1) continue;
                    fori(k, nn) {
                        if (used[k]) continue;

                        int tot = 0;
                        for (auto [ni, nj] : nex[i][j]) {
                            if (out[ni][nj] == -1) continue;
                            tot += max_sum(k, out[ni][nj], t);
                        }
                        if (tot == 0) continue;

                        if (!next_hq.empty() and next_hq.top().score > score + tot) continue;

                        out[i][j] = k;
                        used[k]   = true;
                        next_hq.push({score + tot, out, used});
                        if (next_hq.size() > beam_w) next_hq.pop();
                        out[i][j] = -1;
                        used[k]   = false;
                    }
                }
            }
            state_hq = next_hq;
        }
        while (state_hq.size() > 1) {
            state_hq.pop();
        }
        return state_hq.top().A;
    };

    fori(t, T) {
        auto out = f(t);
        print(out);
        inp(X);
        top.assign(m, 0);
        top_c.assign(m, 0);
        fori(i, nn) fori(j, m) {
            if (chmax(top[j], X[i][j])) {
                top_c[j] = 1;
            } else if (top[j] == X[i][j]) {
                top_c[j]++;
            }
        }
    }
}

int main() {
#ifndef INTERACTIVE
    std::cin.tie(0)->sync_with_stdio(0);
#endif
    // std::cout << std::fixed << std::setprecision(12);
    int t;
    t = 1;
    // std::cin >> t;
    while (t--) solve();
    return 0;
}
