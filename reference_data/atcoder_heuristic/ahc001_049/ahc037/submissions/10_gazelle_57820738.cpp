#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define FOR(i, n, m) for (long long i = n; i < (int)m; i++)
#define REP(i, n) FOR(i, 0, n)
#define ALL(v) v.begin(), v.end()
#define PB push_back
using namespace std;
using LL = long long;
using P = pair<long long, long long>;
constexpr LL INF = 1000000000;
constexpr LL MOD = 1000000007;

static int xorshift(int n) {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;

    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8))) % n;
}

int N;
vector<P> A;
LL A_max = 0;

void naive() {
    LL C = 0;

    cout << N << endl;
    REP(i, N) {
        P base = {0, 0};
        int min_diff = 2 * INF + 1;
        for (int j = 0; j < i; j++) {
            if (A[j].second > A[i].second) continue;
            int diff =
                abs(A[i].first - A[j].first) + abs(A[i].second - A[j].second);
            if (diff < min_diff) {
                min_diff = diff;
                base = A[j];
            }
        }

        C += A[i].first - base.first + A[i].second - base.second;

        cout << base.first << " " << base.second << " " << A[i].first << " "
             << A[i].second << endl;
    }

    cerr << 1000000LL * 1000 * A_max / C << endl;
}

LL select_best(P p) {
    LL sum = p.first + p.second;
    LL best = sum;
    REP(i, A.size()) {
        if (A[i].first > p.first or A[i].second > p.second) continue;
        if (A[i] == p) continue;
        best = min(best, sum - A[i].first - A[i].second);
    }
    return best;
}

set<pair<int, int>> st;

void unite(int lb, int ub) {
    for (int i = 0; i < A.size(); i++) {
        if (A.size() == 5 * N) break;
        vector<pair<int, int>> v;
        for (int j = i + 1; j < A.size(); j++) {
            int d =
                abs(A[i].first - A[j].first) + abs(A[i].second - A[j].second);
            v.PB({d, j});
        }
        sort(ALL(v));
        int pr = A.size();
        for (auto [d, j] : v) {
            if (A.size() == 5 * N) break;
            int x = i, y = j;
            if (A[x].first > A[y].first) swap(x, y);
            if (A[x].first <= A[y].first and A[x].second <= A[y].second)
                continue;
            int dist =
                abs(A[x].first - A[y].first) + abs(A[x].second - A[y].second);
            if (dist < lb or dist >= ub) continue;
            P app = {min(A[x].first, A[y].first),
                     min(A[x].second, A[y].second)};
            if (st.count(app) > 0) continue;

            int cur = select_best(A[x]) + select_best(A[y]);
            int next = select_best(app) +
                       (A[x].first - app.first + A[x].second - app.second) +
                       (A[y].first - app.first + A[y].second - app.second);
            if (cur > next) {
                st.insert(app);
                A.PB(app);
            }
        }
        int ppr = A.size();
        for (int j = pr; j < A.size(); j++) {
            if (A.size() == 5 * N) break;
            int x = i, y = j;
            if (A[x].first > A[y].first) swap(x, y);
            if (A[x].first <= A[y].first and A[x].second <= A[y].second)
                continue;
            int dist =
                abs(A[x].first - A[y].first) + abs(A[x].second - A[y].second);
            if (dist < lb or dist >= ub) continue;
            P app = {min(A[x].first, A[y].first),
                     min(A[x].second, A[y].second)};
            if (st.count(app) > 0) continue;
            int cur = select_best(A[x]) + select_best(A[y]);
            int next = select_best(app) +
                       (A[x].first - app.first + A[x].second - app.second) +
                       (A[y].first - app.first + A[y].second - app.second);
            if (cur > next) {
                st.insert(app);
                A.PB(app);
            }
        }
        for (int j = ppr; j < A.size(); j++) {
            if (A.size() == 5 * N) break;
            int x = i, y = j;
            if (A[x].first > A[y].first) swap(x, y);
            if (A[x].first <= A[y].first and A[x].second <= A[y].second)
                continue;
            int dist =
                abs(A[x].first - A[y].first) + abs(A[x].second - A[y].second);
            if (dist < lb or dist >= ub) continue;
            P app = {min(A[x].first, A[y].first),
                     min(A[x].second, A[y].second)};
            if (st.count(app) > 0) continue;
            int cur = select_best(A[x]) + select_best(A[y]);
            int next = select_best(app) +
                       (A[x].first - app.first + A[x].second - app.second) +
                       (A[y].first - app.first + A[y].second - app.second);
            if (cur > next) {
                st.insert(app);
                A.PB(app);
            }
        }
    }
}
mt19937 engine(1);
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    cin >> N;
    A.resize(N);
    REP(i, N) { cin >> A[i].first >> A[i].second; }
    REP(i, N) {
        A_max = max(A_max, A[i].first);
        A_max = max(A_max, A[i].second);
        st.insert(A[i]);
    }

    // sort(ALL(A));
    shuffle(ALL(A), engine);
    for (int i = 1000000; i < 10000000; i += 1000000) {
        unite(0, i);
    }
    for (int i = 10000000; i < 100000000; i += 10000000) {
        unite(0, i);
    }
    unite(0, 100000000);
    // cerr << A.size() << endl;
    unite(0, 250000000);
    // cerr << A.size() << endl;
    unite(0, 350000000);
    // cerr << A.size() << endl;

    N = A.size();
    sort(ALL(A));

    naive();
}
