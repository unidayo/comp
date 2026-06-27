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
typedef pair<int, P> PP;
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

int n, si, sj;
string c[70];
vector<P> road;

const int dy[4] = {0, 1, 0, -1};
const int dx[4] = {1, 0, -1, 0};

const int INF = 1e9+9;
const int MAX_N = 70*70;
struct edge {
    P to;
    int cost;
};

struct way {
    int sy, sx;
    int gy, gx;
};
vector<way> ways;
bool used[70][70];
bool is_junc[70][70];
vector<P> all_junc;
vector<P> juncs[70*70];

vector<edge> G[70][70];
int dist[70][70][70][70];

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

void add_edge(const P& from, const P& to, const int cost) {
    G[from.first][from.second].push_back(edge{to, cost});
}

void calc(const P& s) {
    const int sy = s.first;
    const int sx = s.second;
    priority_queue<PP, vector<PP>, greater<PP>> que;
    for (const P& p : road) dist[sy][sx][p.first][p.second] = INF;
    dist[sy][sx][sy][sx] = 0;
    que.push(PP(0, s));
    while (!que.empty()) {
        const PP p = que.top(); que.pop();
        const P& v = p.second;
        const int cur_dist = dist[sy][sx][v.first][v.second];
        if (cur_dist < p.first) continue;
        for (const edge& e : G[v.first][v.second]) {
            if (dist[sy][sx][e.to.first][e.to.second] > cur_dist + e.cost) {
                dist[sy][sx][e.to.first][e.to.second] = cur_dist + e.cost;
                que.push(PP(dist[sy][sx][e.to.first][e.to.second], e.to));
            }
        }
    }
}

inline bool on_road(const int y, const int x) {
    return 0 <= y && y < n && 0 <= x && x < n && c[y][x] != '#';
}

void make_graph() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (c[i][j] == '#') continue;
            road.push_back(make_pair(i, j));
            const P p = make_pair(i, j);
            for (int k = 0; k < 4; k++) {
                const int y = i + dy[k];
                const int x = j + dx[k];
                if (on_road(y, x)) {
                    add_edge(p, make_pair(y, x), (int)(c[y][x]-'0'));
                }
            }
        }
    }
}

void calc_ways() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (c[i][j] == '#') continue;
            bool d1 = false, d2 = false;
            for (int k = 0; k < 4; k++) {
                const int y = i + dy[k];
                const int x = j + dx[k];
                if (on_road(y, x)) {
                    if (k % 2)
                        d1 = true;
                    else
                        d2 = true;
                }
                if (d1 && d2) {
                    is_junc[i][j] = true;
                    calc(make_pair(i, j));
                    all_junc.push_back(make_pair(i, j));
                }
            }
        }
    }
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (c[i][j] == '#' || used[i][j]) continue;
            int sy = i, gy = i;
            while (sy >= 0 && c[sy][j] != '#') sy--;
            while (gy < n && c[gy][j] != '#') gy++;
            int sx = j, gx = j;
            while (sx >= 0 && c[i][sx] != '#') sx--;
            while (gx < n && c[i][gx] != '#') gx++;
            if (gy - sy > gx - sx) {
                sy++;
                gy--;
                bool f = false;
                for (int k = sy; k <= gy; k++) {
                    used[k][j] = true;
                    if (k == si && j == sj) f = true;
                }
                if (!f) {
                    const int idx = (int)ways.size();
                    ways.push_back(way{sy, j, gy, j});
                    for (int k = sy; k <= gy; k++) {
                        if (is_junc[k][j]) {
                            juncs[idx].push_back(make_pair(k, j));
                        }
                    }
                }
            } else {
                sx++;
                gx--;
                bool f = false;
                for (int k = sx; k <= gx; k++) {
                    used[i][k] = true;
                    if (i == si && k == sj) f = true;
                }
                if (!f) {
                    const int idx = (int)ways.size();
                    ways.push_back(way{i, sx, i, gx});
                    for (int k = sx; k <= gx; k++) {
                        if (is_junc[i][k]) {
                            juncs[idx].push_back(make_pair(i, k));
                        }
                    }
                }
            }
        }
    }
}

int calc_dist(const vector<int>& p, const int nw) {
    int y = si, x = sj;
    int d = 0;
    for (int i = 0; i < nw; i++) {
        const int idx = p[i];
        int min_d = INF;
        int ny, nx;
        for (const P& jun : juncs[idx]) {
            if (min_d > dist[y][x][jun.first][jun.second]) {
                min_d = dist[y][x][jun.first][jun.second];
                ny = jun.first;
                nx = jun.second;
            }
        }
        d += min_d;
        y = ny;
        x = nx;
    }
    d += dist[y][x][si][sj];
    return d;
}

struct route {
    int y, x;
    int d;
    string command;
};

string calc_command(const vector<int>& p, const int nw) {
    vector<route> rs[nw+1];
    rs[0].push_back(route{si, sj, 0, ""});
    for (int i = 0; i < nw; i++) {
        const int idx = p[i];
        for (const P& jun : juncs[idx]) {
            int min_d = INF;
            int sidx;
            for (int j = 0; j < (int)rs[i].size(); j++) {
                const route& r = rs[i][j];
                if (min_d > dist[r.y][r.x][jun.first][jun.second] + r.d) {
                    min_d = dist[r.y][r.x][jun.first][jun.second] + r.d;
                    sidx = j;
                }
            }
            route nr;
            nr.y = jun.first;
            nr.x = jun.second;
            nr.d = min_d;            
            int y = rs[i][sidx].y, x = rs[i][sidx].x;
            int ny = nr.y, nx = nr.x;
            string command = "";
            while (!(y == ny && x == nx)) {
                int md = INF;
                int ty, tx;
                for (const P& jun : all_junc) {
                    if (jun.first == y && jun.second == x) continue;
                    if ((jun.first == y || jun.second == x) &&
                        dist[y][x][jun.first][jun.second] + dist[jun.first][jun.second][ny][nx] == dist[y][x][ny][nx] &&
                        md > dist[y][x][jun.first][jun.second]) {
                        md = dist[y][x][jun.first][jun.second];
                        ty = jun.first;
                        tx = jun.second;
                    }
                }
                if (y == ty) {
                    while (x < tx) {
                        x++;
                        command += "R";
                    }
                    while (x > tx) {
                        x--;
                        command += "L";
                    }
                } else {
                    while (y < ty) {
                        y++;
                        command += "D";
                    }
                    while (y > ty) {
                        y--;
                        command += "U";
                    }
                }
            }
            nr.command = rs[i][sidx].command + command;
            rs[i+1].push_back(nr);
        }
    }

    int min_d = INF;
    int sidx;
    for (int j = 0; j < (int)rs[nw].size(); j++) {
        const route& r = rs[nw][j];
        if (min_d > dist[r.y][r.x][si][sj] + r.d) {
            min_d = dist[r.y][r.x][si][sj] + r.d;
            sidx = j;
        }
    }

    int y = rs[nw][sidx].y, x = rs[nw][sidx].x;
    int ny = si;
    int nx = sj;
    all_junc.push_back(make_pair(si, sj));
    string command = "";
    while (!(y == ny && x == nx)) {
        int md = INF;
        int ty, tx;
        for (const P& jun : all_junc) {
            if (jun.first == y && jun.second == x) continue;
            if ((jun.first == y || jun.second == x) &&
                dist[y][x][jun.first][jun.second] + dist[jun.first][jun.second][ny][nx] == dist[y][x][ny][nx] &&
                md > dist[y][x][jun.first][jun.second]) {
                md = dist[y][x][jun.first][jun.second];
                ty = jun.first;
                tx = jun.second;
            }
        }
        if (y == ty) {
            while (x < tx) {
                x++;
                command += "R";
            }
            while (x > tx) {
                x--;
                command += "L";
            }
        } else {
            while (y < ty) {
                y++;
                command += "D";
            }
            while (y > ty) {
                y--;
                command += "U";
            }
        }
    }
    return rs[nw][sidx].command + command;
}

string solve() {
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    make_graph();
    calc_ways();
    calc(make_pair(si, sj));

    const int nw = (int)ways.size();
    vector<int> p(nw);
    for (int i = 0; i < nw; i++) p[i] = i;
    int cur_dist = calc_dist(p, nw);
    int iter;
    const double T_init = 40;
    const double T_end = 0.0001;
    double T = T_init;
    for (iter = 1; ; iter++) {
        int l = rand(nw);
        int r = rand(nw);
        if (l == r) continue;
        if (l > r) swap(l, r);
        reverse(p.begin()+l, p.begin()+r+1);
        int next_dist = calc_dist(p, nw);
        if (next_dist < cur_dist || exp((cur_dist-next_dist)/T) > (double)rand(INF) / INF) {
            cur_dist = next_dist;
        } else {
            reverse(p.begin()+l, p.begin()+r+1);
        }

        if (iter % 100 == 0) {
            gettimeofday(&t2, NULL);
            const double et = get_elapsed_time(&t1, &t2);
            if (et > 2950) break;
            //T *= 0.9996;
            T = (T_end - T_init) / 2950 * et + T_init;
        }
    }
    cerr << "iter: " << iter << endl;
    cerr << "T: " << T << endl;
    cerr << "num ways: " << ways.size() << endl;
    
    return calc_command(p, nw);
}

void input() {
    cin >> n >> si >> sj;
    for (int i = 0; i < n; i++) cin >> c[i];
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout << fixed << setprecision(15);

    input();
    cout << solve() << endl;
    //solve();
    //cout << (solve()?"Yes":"No") << endl;
}
