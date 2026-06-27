#include <algorithm>
#include <iostream>
#include <cstdio>
#include <map>
#include <numeric>
#include <cmath>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <complex>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <iomanip>
#include <sys/time.h>
#include <tuple>
#include <random>
using namespace std;

#define endl '\n'
#define ALL(v) (v).begin(), (v).end()
#define RALL(v) (v).rbegin(), (v).rend()
#define UNIQ(v) (v).erase(unique((v).begin(), (v).end()), (v).end())

typedef long long ll;
typedef long double ld;
typedef pair<int, int> P;
typedef complex<double> comp;
typedef vector< vector<ld> > matrix;
struct pairhash {
public:
    template<typename T, typename U>
    size_t operator()(const pair<T, U> &x) const {
    size_t seed = hash<T>()(x.first);
    return hash<U>()(x.second) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
};
const int INF = 1e7 + 9;
//const ll INF = 1e18 + 9;
const ll MOD = 1e9 + 7;
//const ll MOD = 998244353;
const double EPS = 1e-8;
const double PI = acos(-1);

unsigned long xor128(void) {
    static unsigned long x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

// [0, k)
int rand(int k) {
    return xor128() % k;
}

// [a, b)
int rand(int a, int b) {
    return a + xor128() % (b - a);
}

double get_elapsed_time(struct timeval *begin, struct timeval *end) {
    return (end->tv_sec - begin->tv_sec) * 1000
	+ (end->tv_usec - begin->tv_usec) / 1000.0;
}

struct line {
    int px, py, qx, qy;
};

int n, k;
int a[11];
int x[5500], y[5500];

const int R = 10000;
int sum_a = 0;
int t = 1;

vector<int> xp[2*R+10];
vector<int> yp[2*R+10];

vector<line> h, v;
vector<line> best_h, best_v;
int hs, vs;
int b[5500];
int nb[5500];
int num[101][101];

const double B = 100.0;
const int HD = 30;
const int VD = 30;

void init() {
    for (int i = 1; i <= 10; i++) sum_a += a[i];
    double min_diff = INF;
    for (int i = 1; i <= 50; i++) {
        const double d = abs(i * (100 - i) - sum_a * 4 / PI);
        if (d < min_diff) {
            min_diff = d;
            t = i;
        }
    }

    h.push_back(line{-INF, -R, INF, -R});
    for (int i = 1; i <= t; i++) {
        const int _y = -R + 2 * R / (t+1) * i;
        h.push_back(line{-INF, _y, INF, _y+1});
    }
    h.push_back(line{-INF, R, INF, R});
    hs = t+1;

    v.push_back(line{-R, -INF, -R, INF});
    for (int i = 1; i <= 100-t; i++) {
        const int _x = -R + 2 * R / (101-t) * i;
        v.push_back(line{_x, -INF, _x+1, INF});
    }
    v.push_back(line{R, -INF, R, INF});
    vs = 101-t;

    for (int i = 0; i < n; i++) {
        xp[x[i]+R].push_back(y[i]);
        yp[y[i]+R].push_back(x[i]);
    }
    for (int i = 0; i <= 2*R; i++) {
        sort(ALL(xp[i]));
        sort(ALL(yp[i]));
    }

    int h_idx = 1;
    for (int i = 0; i <= 2*R; i++) {
        const int _y = i-R;
        if (h[h_idx].py < _y) h_idx++;
        int v_idx = 1;
        for (const int _x : yp[i]) {
            while (v[v_idx].px < _x) v_idx++;
            num[h_idx-1][v_idx-1]++;
        }
    }

    for (int i = 0; i < hs; i++) {
        for (int j = 0; j < vs; j++) {
            b[num[i][j]]++;
        }
    }
}

int hd[2][101];
double diff_h(const int r, const int m) {
    memset(hd, 0, sizeof(hd));
    if (m > 0) {
        for (int i = h[r].qy; i <= h[r].py+m; i++) {
            int v_idx = 1;
            for (const int _x : yp[i+R]) {
                while (v[v_idx].px < _x) v_idx++;
                hd[0][v_idx-1]++;
                hd[1][v_idx-1]--;
            }
        }
    } else {
        for (int i = h[r].py; i > h[r].py+m; i--) {
            int v_idx = 1;
            for (const int _x : yp[i+R]) {
                while (v[v_idx].px < _x) v_idx++;
                hd[0][v_idx-1]--;
                hd[1][v_idx-1]++;
            }
        }
    }

    for (int i = 1; i <= 10; i++) nb[i] = b[i];
    for (int i = 0; i < vs; i++) {
        nb[num[r-1][i]]--;
        nb[num[r-1][i]+hd[0][i]]++;

        nb[num[r][i]]--;
        nb[num[r][i]+hd[1][i]]++;
    }

    double d_cur = 0, d_next = 0;
    for (int i = 1; i <= 10; i++) {
        d_cur += max(0, a[i] - b[i]) + max(0, b[i] - a[i]) / B;
        d_next += max(0, a[i] - nb[i]) + max(0, nb[i] - a[i]) / B;
    }

    return d_cur - d_next;
}

void change_h(const int r, const int m) {
    h[r].py += m;
    h[r].qy += m;

    for (int i = 0; i < vs; i++) {
        num[r-1][i] += hd[0][i];
        num[r][i] += hd[1][i];
    }
    for (int i = 1; i <= 10; i++) {
        b[i] = nb[i];
    }
}

int vd[2][101];
double diff_v(const int r, const int m) {
    memset(vd, 0, sizeof(vd));
    if (m > 0) {
        for (int i = v[r].qx; i <= v[r].px+m; i++) {
            int h_idx = 1;
            for (const int _y : xp[i+R]) {
                while (h[h_idx].py < _y) h_idx++;
                vd[0][h_idx-1]++;
                vd[1][h_idx-1]--;
            }
        }
    } else {
        for (int i = v[r].px; i > v[r].px+m; i--) {
            int h_idx = 1;
            for (const int _y : xp[i+R]) {
                while (h[h_idx].py < _y) h_idx++;
                vd[0][h_idx-1]--;
                vd[1][h_idx-1]++;
            }
        }
    }

    for (int i = 1; i <= 10; i++) nb[i] = b[i];
    for (int i = 0; i < hs; i++) {
        nb[num[i][r-1]]--;
        nb[num[i][r-1]+vd[0][i]]++;

        nb[num[i][r]]--;
        nb[num[i][r]+vd[1][i]]++;
    }

    double d_cur = 0, d_next = 0;
    for (int i = 1; i <= 10; i++) {
        d_cur += max(0, a[i] - b[i]) + max(0, b[i] - a[i]) / B;
        d_next += max(0, a[i] - nb[i]) + max(0, nb[i] - a[i]) / B;
    }
    return d_cur - d_next;
}

void change_v(const int r, const int m) {
    v[r].px += m;
    v[r].qx += m;

    for (int i = 0; i < hs; i++) {
        num[i][r-1] += vd[0][i];
        num[i][r] += vd[1][i];
    }
    for (int i = 1; i <= 10; i++) {
        b[i] = nb[i];
    }
}

int score() {
    int res = 0;
    for (int i = 1; i <= 10; i++) {
        res += max(0, a[i] - b[i]);
    }
    return res;
}

vector<line> solve() {
    init();

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    int iter;
    const double T_init = 0.27;
    const double T_end = 0.01;
    double T = T_init;
    int move = 0;
    int best_score = INF;
    for (iter = 1; best_score > 0; iter++) {
        int r = rand(1, 101);
        if (r <= t) {
            const int m = rand(1, HD) * (rand(2) == 0 ? 1 : -1);
            if ((m > 0 && h[r].py + m >= h[r+1].py) ||
                (m < 0 && h[r].py + m <= h[r-1].py)) {
                continue;
            }
            const double d = diff_h(r, m);
            if (d > 0 || exp(d/T) > (double)rand(1e9) / 1e9) {
                change_h(r, m);
                move++;
                const int s = score();
                if (s < best_score) {
                    best_score = s;
                    best_h = h;
                    best_v = v;
                }
            }
        } else {
            r -= t;
            const int m = rand(1, VD) * (rand(2) == 0 ? 1 : -1);
            if ((m > 0 && v[r].px + m >= v[r+1].px) ||
                (m < 0 && v[r].px + m <= v[r-1].px)) {
                continue;
            }
            const double d = diff_v(r, m);
            if (d > 0 || exp(d/T) > (double)rand(1e9) / 1e9) {
                change_v(r, m);
                move++;
                const int s = score();
                if (s < best_score) {
                    best_score = s;
                    best_h = h;
                    best_v = v;
                }
            }
        }

        if (iter % 1000 == 0) {
            gettimeofday(&t2, NULL);
            const double et = get_elapsed_time(&t1, &t2);
            if (et > 2950) break;
            //T *= 0.9999;
            T = (T_end - T_init) / 2950 * et + T_init;
        }
    }
    /*
    for (int i = 1; i <= 10; i++) {
        cerr << i << ": " << b[i] << endl;
    }
    */
    cerr << "iter: " << iter << endl;
    cerr << "move: " << move << endl;
    cerr << "T: " << T << endl;
    vector<line> res;
    for (int i = 1; i <= t; i++) {
        res.push_back(best_h[i]);
    }
    for (int i = 1; i <= 100-t; i++) {
        res.push_back(best_v[i]);
    }
    return res;
}

void input() {
    cin >> n >> k;
    for (int i = 1; i <= 10; i++) cin >> a[i];
    for (int i = 0; i < n; i++) cin >> x[i] >> y[i];
}

void output(const vector<line>& res) {
    cout << (int)res.size() << endl;
    for (const line& l : res) {
        cout << l.px << " " << l.py << " " << l.qx << " " << l.qy << endl;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout << fixed << setprecision(15);

    input();
    const auto& res = solve();
    output(res);
}
