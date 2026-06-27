#include <cstdio>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

class timer {
    public:
    void start() {
        origin = rdtsc();
    }
    
    inline double get_time() {
        return (rdtsc() - origin) * SECONDS_PER_CLOCK;
    }
    
    private:
    constexpr static double SECONDS_PER_CLOCK = 1 / 3.0e9;
    unsigned long long origin;
    
    inline static unsigned long long rdtsc() {
        unsigned long long lo, hi;
        __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
        return (hi << 32) | lo;
    }
};

class random {
    public:
    // [0, x)
    inline static unsigned get(unsigned x) {
        return ((unsigned long long)xorshift() * x) >> 32;
    }
    
    // [x, y]
    inline static unsigned get(unsigned x, unsigned y) {
        return get(y - x + 1) + x;
    }
    
    // [0, x] (x = 2^c - 1)
    inline static unsigned get_fast(unsigned x) {
        return xorshift() & x;
    }
    
    // [0.0, 1.0]
    inline static double probability() {
        return xorshift() * INV_MAX;
    }
    
    inline static bool toss() {
        return xorshift() & 1;
    }
    
    private:
    constexpr static double INV_MAX = 1.0 / 0xFFFFFFFF;
    
    inline static unsigned xorshift() {
        static unsigned x = 123456789, y = 362436039, z = 521288629, w = 88675123;
        unsigned t = x ^ (x << 11);
        x = y, y = z, z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }
};

const int n = 50;
const int m = 100;
const int dx[4] = {0, 1, 0, -1};
const int dy[4] = {1, 0, -1, 0};
int c[n + 2][n + 2];
int d[n + 2][n + 2];
int d2[n + 2][n + 2];
int best[n + 2][n + 2];
int cnt[m + 1];
int cnt2[m + 1];
set<int> neighbors[m + 1];
int state[n + 2][n + 2];
int visited[n + 2][n + 2];

void input() {
    scanf("%*d %*d");
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            scanf("%d", &c[i][j]);
        }
    }
}

void output() {
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (j > 1) putchar(' ');
            printf("%d", d[i][j]);
        }
        puts("");
    }
}

int dfs(int x, int y, int c, int v, set<int>& s, int d[n + 2][n + 2]) {
    int num = 1;
    visited[x][y] = v;
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (d[nx][ny] != c) s.insert(d[nx][ny]);
        if (visited[nx][ny] < v && d[nx][ny] == c) num += dfs(nx, ny, c, v, s, d);
    }
    return num;
}

void dfs2(int x, int y, int v) {
    visited[x][y] = v;
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (visited[nx][ny] < v && d2[nx][ny] > 0) dfs2(nx, ny, v);
    }
}

void dfs3(int x, int y, int v) {
    visited[x][y] = v;
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx < 0 || nx > n + 1 || ny < 0 || ny > n + 1 || visited[nx][ny] == v || d2[nx][ny] > 0) continue;
        dfs3(nx, ny, v);
    }
}

int main() {
    input();
    
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            for (int k = 0; k < 4; k++) {
                int x = i + dx[k];
                int y = j + dy[k];
                if (c[i][j] != c[x][y]) neighbors[c[i][j]].insert(c[x][y]);
            }
        }
    }
    
    int v = 1;
    int best_score = 0;
    for (int t = 0; t < 2; t++) {
        timer tt;
        tt.start();
        
        for (int i = 1; i <= m; i++) cnt[i] = 0;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                d[i][j] = c[i][j];
                cnt[c[i][j]]++;
            }
        }
        
        vector<pair<int, int>> out;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (i == 1 || i == n || j == 1 || j == n) {
                    state[i][j] = 1;
                    out.emplace_back(i, j);
                }
            }
        }
        
        int score = 1;
        vector<pair<int, int>> candidates;
        set<int> s1, s2;
        while (tt.get_time() < 0.95) {
            for (int p = 0; p < 256; p++) {
                v++;
                int r = random::get(4000);
                if (r < 1) {
                    int pos = random::get(out.size());
                    int x = out[pos].first;
                    int y = out[pos].second;
                    int c = d[x][y];
                    if (cnt[c] == 1) continue;
                    bool ng = false;
                    for (int i = 0; i < 4; i++) {
                        int nx = x + dx[i];
                        int ny = y + dy[i];
                        if (d[nx][ny] > 0 && !neighbors[d[nx][ny]].count(0)) ng = true;
                    }
                    if (ng) continue;
                    int num;
                    s1.clear();
                    d[x][y] = 0;
                    for (int i = 0; i < 4; i++) {
                        int nx = x + dx[i];
                        int ny = y + dy[i];
                        if (d[nx][ny] == c) {
                            num = dfs(nx, ny, c, v, s1, d);
                            break;
                        }
                    }
                    if (num != cnt[c] - 1 || neighbors[c] != s1) {
                        d[x][y] = c;
                    } else {
                        score++;
                        cnt[c]--;
                        out[pos] = out.back();
                        out.pop_back();
                        for (int i = 0; i < 4; i++) {
                            int nx = x + dx[i];
                            int ny = y + dy[i];
                            if (d[nx][ny] > 0 && state[nx][ny] == 0) {
                                state[nx][ny] = 1;
                                out.emplace_back(nx, ny);
                            }
                        }
                    }
                } else if (r < 2000) {
                    int x = random::get(1, n);
                    int y = random::get(1, n);
                    if (d[x][y] == 0) continue;
                    int c1 = d[x][y];
                    candidates.clear();
                    for (int i = 0; i < 4; i++) {
                        int nx = x + dx[i];
                        int ny = y + dy[i];
                        if (d[nx][ny] == 0 || d[nx][ny] == c1 || cnt[d[nx][ny]] == 1) continue;
                        bool ng = false;
                        for (int j = 0; j < 4; j++) {
                            int nnx = nx + dx[j];
                            int nny = ny + dy[j];
                            if (d[nnx][nny] != c1 && !neighbors[c1].count(d[nnx][nny])) ng = true;
                        }
                        if (!ng) candidates.emplace_back(nx, ny);
                    }
                    if (candidates.empty()) continue;
                    int pos = random::get(candidates.size());
                    int nx = candidates[pos].first;
                    int ny = candidates[pos].second;
                    int c2 = d[nx][ny];
                    int num;
                    s1.clear();
                    d[nx][ny] = c1;
                    for (int i = 0; i < 4; i++) {
                        int nnx = nx + dx[i];
                        int nny = ny + dy[i];
                        if (d[nnx][nny] == c2) {
                            num = dfs(nnx, nny, c2, v, s1, d);
                            break;
                        }
                    }
                    if (num != cnt[c2] - 1 || neighbors[c2] != s1) {
                        d[nx][ny] = c2;
                    } else {
                        cnt[c1]++;
                        cnt[c2]--;
                    }
                } else if (r < 2400) {
                    int x = random::get(2, n);
                    int y = random::get(2, n);
                    if (d[x][y] == 0 || d[x][y - 1] == 0 || d[x - 1][y] == 0 || d[x - 1][y - 1] == 0 || d[x][y] == d[x][y - 1] || d[x][y] == d[x - 1][y] || d[x][y] == d[x - 1][y - 1] || d[x][y - 1] == d[x - 1][y] || d[x][y - 1] == d[x - 1][y - 1] || d[x - 1][y] == d[x - 1][y - 1]) continue;
                    int dir = random::get(4);
                    int x1, y1, x2, y2, x3, y3, x4, y4;
                    if (dir == 0) {
                        x1 = x - 1;
                        y1 = y - 1;
                        x2 = x;
                        y2 = y - 1;
                    } else if (dir == 1) {
                        x1 = x - 1;
                        y1 = y - 1;
                        x2 = x - 1;
                        y2 = y;
                    } else if (dir == 2) {
                        x1 = x - 1;
                        y1 = y;
                        x2 = x;
                        y2 = y;
                    } else {
                        x1 = x;
                        y1 = y - 1;
                        x2 = x;
                        y2 = y;
                    }
                    x3 = x1 + dx[dir];
                    y3 = y1 + dy[dir];
                    x4 = x2 + dx[dir];
                    y4 = y2 + dy[dir];
                    int c1 = d[x1][y1];
                    int c2 = d[x2][y2];
                    int c3 = d[x3][y3];
                    int c4 = d[x4][y4];
                    if (cnt[c3] == 1 || cnt[c4] == 1) continue;
                    d[x3][y3] = c1;
                    d[x4][y4] = c2;
                    bool ng = false;
                    for (int i = 0; i < 4; i++) {
                        int nx = x3 + dx[i];
                        int ny = y3 + dy[i];
                        if (d[nx][ny] != c1 && !neighbors[c1].count(d[nx][ny])) ng = true;
                    }
                    if (ng) {
                        d[x3][y3] = c3;
                        d[x4][y4] = c4;
                        continue;
                    }
                    for (int i = 0; i < 4; i++) {
                        int nx = x4 + dx[i];
                        int ny = y4 + dy[i];
                        if (d[nx][ny] != c2 && !neighbors[c2].count(d[nx][ny])) ng = true;
                    }
                    if (ng) {
                        d[x3][y3] = c3;
                        d[x4][y4] = c4;
                        continue;
                    }
                    int num1, num2;
                    s1.clear();
                    s2.clear();
                    for (int i = 0; i < 4; i++) {
                        int nx = x3 + dx[i];
                        int ny = y3 + dy[i];
                        if (d[nx][ny] == c3) {
                            num1 = dfs(nx, ny, c3, v, s1, d);
                            break;
                        }
                    }
                    if (num1 != cnt[c3] - 1 || neighbors[c3] != s1) {
                        d[x3][y3] = c3;
                        d[x4][y4] = c4;
                        continue;
                    }
                    for (int i = 0; i < 4; i++) {
                        int nx = x4 + dx[i];
                        int ny = y4 + dy[i];
                        if (d[nx][ny] == c4) {
                            num2 = dfs(nx, ny, c4, v, s2, d);
                            break;
                        }
                    }
                    if (num2 != cnt[c4] - 1 || neighbors[c4] != s2) {
                        d[x3][y3] = c3;
                        d[x4][y4] = c4;
                    } else {
                        cnt[c1]++;
                        cnt[c2]++;
                        cnt[c3]--;
                        cnt[c4]--;
                    }
                } else {
                    int sx = random::get(1, n);
                    int sy = random::get(1, n);
                    if (d[sx][sy] != 0) continue;
                    int dir = random::get(4);
                    while (d[sx][sy] == 0 && 1 <= sx && sx <= n && 1 <= sy && sy <= n) {
                        sx += dx[dir];
                        sy += dy[dir];
                    }
                    if (d[sx][sy] == 0) continue;
                    for (int i = 1; i <= m; i++) cnt2[i] = cnt[i];
                    bool ng = false;
                    int tx = sx, ty = sy, len = 0;
                    while (d[tx][ty] > 0) {
                        len++;
                        cnt2[d[tx][ty]]--;
                        if (cnt2[d[tx][ty]] == 0) {
                            ng = true;
                            break;
                        }
                        tx += dx[dir];
                        ty += dy[dir];
                    }
                    if (ng) continue;
                    int dir2 = (dir + 1) % 4;
                    int dir3 = (dir + 3) % 4;
                    for (int i = 1; i <= n; i++) {
                        for (int j = 1; j <= n; j++) {
                            d2[i][j] = d[i][j];
                        }
                    }
                    for (int x = sx, y = sy; x != tx || y != ty; x += dx[dir], y += dy[dir]) d2[x][y] = 0;
                    for (int x = sx, y = sy; x != tx || y != ty; x += dx[dir], y += dy[dir]) {
                        int nx = x + dx[dir2];
                        int ny = y + dy[dir2];
                        if (visited[nx][ny] < v && d2[nx][ny] > 0) dfs2(nx, ny, v);
                    }
                    for (int i = 1; i <= n; i++) {
                        for (int j = 1; j <= n; j++) {
                            if (visited[i][j] == v) {
                                int x = i + dx[dir3];
                                int y = j + dy[dir3];
                                if (x < 1 || x > n || y < 1 || y > n) {
                                    ng = true;
                                    i = n;
                                    break;
                                }
                                d2[i][j] = 0;
                            }
                        }
                    }
                    if (ng) continue;
                    for (int i = 1; i <= n; i++) {
                        for (int j = 1; j <= n; j++) {
                            if (visited[i][j] == v) d2[i + dx[dir3]][j + dy[dir3]] = d[i][j];
                        }
                    }
                    v++;
                    for (int i = 1; i <= n; i++) {
                        for (int j = 1; j <= n; j++) {
                            if (visited[i][j] == v || d2[i][j] == 0) continue;
                            s1.clear();
                            int num = dfs(i, j, d2[i][j], v, s1, d2);
                            if (num != cnt2[d2[i][j]] || neighbors[d2[i][j]] != s1) {
                                ng = true;
                                i = n;
                                break;
                            }
                        }
                    }
                    if (ng) continue;
                    dfs3(0, 0, v);
                    for (int i = 0; i <= n; i++) {
                        for (int j = 0; j <= n; j++) {
                            if (d2[i][j] == 0 && visited[i][j] < v) {
                                ng = true;
                                i = n;
                                break;
                            }
                        }
                    }
                    if (ng) continue;
                    score += len;
                    out.clear();
                    for (int i = 1; i <= n; i++) {
                        for (int j = 1; j <= n; j++) {
                            d[i][j] = d2[i][j];
                            state[i][j] = 0;
                            if (d[i][j] == 0) continue;
                            for (int k = 0; k < 4; k++) {
                                int x = i + dx[k];
                                int y = j + dy[k];
                                if (d2[x][y] == 0) {
                                    state[i][j] = 1;
                                    out.emplace_back(i, j);
                                    break;
                                }
                            }
                        }
                    }
                    for (int i = 1; i <= m; i++) cnt[i] = cnt2[i];
                }
            }
        }
        
        if (score > best_score) {
            best_score = score;
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    best[i][j] = d[i][j];
                }
            }
        }
    }
    
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            d[i][j] = best[i][j];
        }
    }
    
    output();
    
    fprintf(stderr, "%d\n", best_score);
    
    return 0;
}
