#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <numeric>
#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2")
#define REP(i, N) for (int i = 0; i < (int)N; i++)
#define FOR(i, a, b) for (int i = a; i < (int)b; i++)
#define ALL(v) v.begin(), v.end()

using namespace std;
using ll = long long;
constexpr int dy[4] = {-1, 0, 1, 0}, dx[4] = {0, -1, 0, 1};

struct Timer
{
    static constexpr double ONE_CLOCK = 0.000000333;
    unsigned long long get_cycle()
    {
        unsigned int low, high;
        __asm__ volatile("rdtsc"
                         : "=a"(low), "=d"(high));
        return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
    }
    unsigned long long start_time, last_time;
    Timer() { start_time = last_time = get_cycle(); }
    int get_time() { return get_cycle() * ONE_CLOCK - start_time * ONE_CLOCK; }
    int split_time() { return get_cycle() * ONE_CLOCK - last_time * ONE_CLOCK; }
    void split() { last_time = get_cycle(); }
} timer;

class RandInt
{
public:
    int x = 8753, y = 239017, z = 1000000123;
    int next(int d)
    {
        int t = x ^ (x << 11);
        x = y;
        y = z;
        z ^= (z >> 19) ^ t ^ (t >> 8);
        return z % d;
    }
} ri;

template <class T>
string to_string(T s);
template <class S, class T>
string to_string(pair<S, T> p);
string to_string(char c) { return string(1, c); }
string to_string(string s) { return s; }
string to_string(const char s[]) { return string(s); }

template <class T>
string to_string(T v)
{
    if (v.empty())
        return "{}";
    string ret = "{";
    for (auto x : v)
        ret += to_string(x) + ",";
    ret.back() = '}';
    return ret;
}

template <class S, class T>
string to_string(pair<S, T> p)
{
    return "{" + to_string(p.first) + ":" + to_string(p.second) + "}";
}
void debug() { cerr << endl; }

template <class Head, class... Tail>
void debug(Head head, Tail... tail)
{
    cerr << to_string(head) << " ";
    debug(tail...);
}

template <class T>
bool chmax(T &a, const T &b)
{
    if (a < b)
    {
        a = b;
        return 1;
    }
    return 0;
}
template <class T>
bool chmin(T &a, const T &b)
{
    if (b < a)
    {
        a = b;
        return 1;
    }
    return 0;
}

using ll = long long;

struct Point
{
    int y, x;
    Point() {}
    Point(int _y, int _x) : y(_y), x(_x) {}
};

using Pii = pair<int, int>;
int N;
array<int, 11> a;
int A = 0;
vector<Point> P;
vector<int> byx[20000], byy[20000];

int M = 28;

struct State
{
    array<int, 40> X, Y;
    array<array<int, 41>, 41> mp;
    array<int, 256> b;
    Pii belong[6000];
    int score = 0;
    State()
    {
        REP(i, M)
        {
            X[i] = Y[i] = 20000 / M * i + (20000 / M / 2);
        }
    }
    void updatex(int i, int d)
    {
        if (d < 0)
        {
            for (int x = X[i] - 1; x >= X[i] + d; --x)
            {
                for (int n : byx[x])
                {
                    b[mp[belong[n].first][belong[n].second]]--;
                    mp[belong[n].first][belong[n].second]--;
                    b[mp[belong[n].first][belong[n].second]]++;

                    belong[n].first++;
                    b[mp[belong[n].first][belong[n].second]]--;

                    mp[belong[n].first][belong[n].second]++;
                    b[mp[belong[n].first][belong[n].second]]++;
                }
            }
        }
        else
        {
            for (int x = X[i]; x < X[i] + d; ++x)
            {
                for (int n : byx[x])
                {
                    b[mp[belong[n].first][belong[n].second]]--;
                    mp[belong[n].first][belong[n].second]--;
                    b[mp[belong[n].first][belong[n].second]]++;

                    belong[n].first--;
                    b[mp[belong[n].first][belong[n].second]]--;

                    mp[belong[n].first][belong[n].second]++;
                    b[mp[belong[n].first][belong[n].second]]++;
                }
            }
        }
        X[i] += d;
        score = 0;
        FOR(i, 1, 11)
        score += min(a[i], b[i]);
    }
    void updatey(int i, int d)
    {
        if (d < 0)
        {
            for (int y = Y[i] - 1; y >= Y[i] + d; --y)
            {
                for (int n : byy[y])
                {
                    b[mp[belong[n].first][belong[n].second]]--;
                    mp[belong[n].first][belong[n].second]--;
                    b[mp[belong[n].first][belong[n].second]]++;

                    belong[n].second++;
                    b[mp[belong[n].first][belong[n].second]]--;

                    mp[belong[n].first][belong[n].second]++;
                    b[mp[belong[n].first][belong[n].second]]++;
                }
            }
        }
        else
        {
            for (int y = Y[i]; y < Y[i] + d; ++y)
            {
                for (int n : byy[y])
                {
                    b[mp[belong[n].first][belong[n].second]]--;
                    mp[belong[n].first][belong[n].second]--;
                    b[mp[belong[n].first][belong[n].second]]++;

                    belong[n].second--;
                    b[mp[belong[n].first][belong[n].second]]--;

                    mp[belong[n].first][belong[n].second]++;
                    b[mp[belong[n].first][belong[n].second]]++;
                }
            }
        }
        Y[i] += d;
        score = 0;
        FOR(i, 1, 11)
        score += min(a[i], b[i]);
    }
    void set_score(int db = 0)
    {
        score = 0;
        REP(i, M + 1)
        REP(j, M + 1)
        mp[i][j] = 0;
        REP(i, N)
        {
            auto p = P[i];
            auto xitr = upper_bound(X.begin(), X.begin() + M, p.x);
            auto yitr = upper_bound(Y.begin(), Y.begin() + M, p.y);
            belong[i] = {xitr - X.begin(), yitr - Y.begin()};
            mp[belong[i].first][belong[i].second]++;
        }
        REP(i, 256)
        b[i] = 0;
        REP(i, M + 1)
        REP(j, M + 1)

        {
            b[mp[i][j]]++;
        }
        FOR(i, 1, 11)
        score += min(a[i], b[i]);
        if (db)
            debug(score, 1.0 * score / A, b);
    }
    void print_solution()
    {
        cout << M * 2 << endl;
        REP(i, M)
        {
            cout << X[i] - 10000 << " " << -10000 << " " << X[i] - 10001 << " " << 100000000 << endl;
        }
        REP(i, M)
        {
            cout << -10000 << " " << Y[i] - 10000 << " " << 100000000 << " " << Y[i] - 10001 << endl;
        }
    }
};

int main()
{
    int _;
    cin >> N >> _;

    FOR(i, 1, 11)
    cin >> a[i];
    FOR(i, 1, 11)
    A += a[i];
    P.resize(N);
    REP(i, N)
    cin >> P[i].x >> P[i].y;
    REP(i, N)
    {
        P[i].x += 10000;
        P[i].y += 10000;
        byx[P[i].x].push_back(i);
        byy[P[i].y].push_back(i);
    }
    int bestM;
    int bestsc = -1;
    State best_state;
    best_state.score = -1;
    constexpr int mg = 2;

    for (int m = 20; m <= 36; m += 2)
    {
        M = m;
        State state;
        state.set_score();
        int L = 300000;
        REP(t, L)
        {
            int w = 15 + (L - t) / 3000;
            int tp = ri.next(2);
            int x = ri.next(M);
            int d = ri.next(w * 2 + 1) - w; // debug(t,tp,x,d,state.X);
            if (tp == 0)
            {
                while (d == 0 || x == 0 && state.X[x] + d <= mg || x == M - 1 && state.X[x] + d >= 20000 - mg ||
                       x > 0 && state.X[x - 1] + mg >= state.X[x] + d || x + 1 < M && state.X[x + 1] - mg <= state.X[x] + d)
                {
                    x = ri.next(M);
                    d = ri.next(w * 2 + 1) - w;
                }
            }
            else
            {
                while (d == 0 || x == 0 && state.Y[x] + d <= mg || x == M - 1 && state.Y[x] + d >= 20000 - mg ||
                       x > 0 && state.Y[x - 1] + mg >= state.Y[x] + d || x + 1 < M && state.Y[x + 1] - mg <= state.Y[x] + d)
                {
                    x = ri.next(M);
                    d = ri.next(w * 2 + 1) - w;
                }
            } // debug(t,tp,x,d);
            auto prev = state.score;
            if (tp == 0)
                state.updatex(x, d);
            else
                state.updatey(x, d);
            int diff = state.score - prev;
            if (diff < 0)
            {
                double T = 1.0 * (L - t) / L;
                if (exp(diff / T / 0.8) < ri.next(100000) / 100000.0)
                {
                    if (tp == 0)
                        state.updatex(x, -d);
                    else
                        state.updatey(x, -d);
                    state.score = prev;
                }
            }
            if (best_state.score < state.score)
            {
                best_state = state;
                bestsc = state.score;
                bestM = M;
            }
        }
        // debug(A, M, 1.0 * state.score / A);
    }

    M = bestM;
    State state = best_state;
    state.set_score();
    int L = 2000000;
    REP(lp, 3)
    REP(t, L)
    {
        int w = 15 + (L - t) / 20000;
        int tp = ri.next(2);
        int x = ri.next(M);
        int d = ri.next(w * 2 + 1) - w; // debug(t,tp,x,d,state.X);
        if (tp == 0)
        {
            while (d == 0 || x == 0 && state.X[x] + d <= mg || x == M - 1 && state.X[x] + d >= 20000 - mg ||
                   x > 0 && state.X[x - 1] + mg >= state.X[x] + d || x + 1 < M && state.X[x + 1] - mg <= state.X[x] + d)
            {
                x = ri.next(M);
                d = ri.next(w * 2 + 1) - w;
            }
        }
        else
        {
            while (d == 0 || x == 0 && state.Y[x] + d <= mg || x == M - 1 && state.Y[x] + d >= 20000 - mg ||
                   x > 0 && state.Y[x - 1] + mg >= state.Y[x] + d || x + 1 < M && state.Y[x + 1] - mg <= state.Y[x] + d)
            {
                x = ri.next(M);
                d = ri.next(w * 2 + 1) - w;
            }
        } // debug(t,tp,x,d);
        auto prev = state.score;
        if (tp == 0)
            state.updatex(x, d);
        else
            state.updatey(x, d);
        int diff = state.score - prev;
        if (diff < 0)
        {
            double T = 1.0 * (L - t) / L;
            if (exp(diff / T / 0.4) < ri.next(100000) / 100000.0)
            {
                if (tp == 0)
                    state.updatex(x, -d);
                else
                    state.updatey(x, -d);
                state.score = prev;
            }
        }
        if (best_state.score < state.score)
        {
            best_state = state;
        }
        if (0 && t % 5000 == 0)
            debug(t, state.score);
    }
    state=best_state;
    while (timer.get_time() < 2950)
    {
        int w = 10;
        int tp = ri.next(2);
        int x = ri.next(M);
        int d = ri.next(w * 2 + 1) - w; // debug(t,tp,x,d,state.X);
        if (tp == 0)
        {
            while (d == 0 || x == 0 && state.X[x] + d <= mg || x == M - 1 && state.X[x] + d >= 20000 - mg ||
                   x > 0 && state.X[x - 1] + mg >= state.X[x] + d || x + 1 < M && state.X[x + 1] - mg <= state.X[x] + d)
            {
                x = ri.next(M);
                d = ri.next(w * 2 + 1) - w;
            }
        }
        else
        {
            while (d == 0 || x == 0 && state.Y[x] + d <= mg || x == M - 1 && state.Y[x] + d >= 20000 - mg ||
                   x > 0 && state.Y[x - 1] + mg >= state.Y[x] + d || x + 1 < M && state.Y[x + 1] - mg <= state.Y[x] + d)
            {
                x = ri.next(M);
                d = ri.next(w * 2 + 1) - w;
            }
        } // debug(t,tp,x,d);
        auto prev = state.score;
        if (tp == 0)
            state.updatex(x, d);
        else
            state.updatey(x, d);
        int diff = state.score - prev;
        if (diff < 0)
        {

            if (tp == 0)
                state.updatex(x, -d);
            else
                state.updatey(x, -d);
            state.score = prev;
        }
        if (best_state.score < state.score)
        {
            best_state = state;
        }
    }
    // debug(N, M, 1.0 * state.score / A);
    debug(1.0 * best_state.score / A);
    //  state.set_score(1);
    best_state.print_solution();
    return 0;
}