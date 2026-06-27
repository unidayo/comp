#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

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

class simulated_annealing {
    public:
    simulated_annealing();
    inline bool end();
    inline bool accept(double current_score, double next_score);
    void print() const;
    
    private:
    constexpr static bool MAXIMIZE = true;
    constexpr static int LOG_SIZE = 0xFFFF;
    constexpr static int UPDATE_INTERVAL = 0xFF;
    constexpr static double TIME_LIMIT = 1.9;
    constexpr static double START_TEMP = 10000;
    constexpr static double END_TEMP = 1e-9;
    constexpr static double TEMP_RATIO = (END_TEMP - START_TEMP) / TIME_LIMIT;
    double log_probability[LOG_SIZE + 1];
    long long iteration = 0;
    long long accepted = 0;
    long long rejected = 0;
    double time = 0;
    double temp = START_TEMP;
    timer sa_timer;
};

simulated_annealing::simulated_annealing() {
    sa_timer.start();
    for (int i = 0; i <= LOG_SIZE; i++) log_probability[i] = log(random::probability());
}

inline bool simulated_annealing::end() {
    iteration++;
    if ((iteration & UPDATE_INTERVAL) == 0) {
        time = sa_timer.get_time();
        temp = START_TEMP + TEMP_RATIO * time;
        return time >= TIME_LIMIT;
    } else {
        return false;
    }
}

inline bool simulated_annealing::accept(double current_score, double next_score) {
    double diff = (MAXIMIZE ? next_score - current_score : current_score - next_score);
    if (diff >= 0 || diff > log_probability[random::get_fast(LOG_SIZE)] * temp) {
        accepted++;
        return true;
    } else {
        rejected++;
        return false;
    }
}

void simulated_annealing::print() const {
    fprintf(stderr, "iteration: %lld\n", iteration);
    fprintf(stderr, "accepted: %lld\n", accepted);
    fprintf(stderr, "rejected: %lld\n", rejected);
}


struct state {
    int x;
    int y;
    int cnt;
    int sum;
    
    state(int x, int y, int cnt, int sum) : x(x), y(y), cnt(cnt), sum(sum) {}
    
    bool operator<(const state& s) const {
        if (cnt != s.cnt) return cnt > s.cnt;
        return sum > s.sum;
    }
};

const int SIZE = 20;
const int TURN = 200;
const int INF = 1e9;
const int dx[4] = {-1, 1, 0, 0};
const int dy[4] = {0, 0, -1, 1};
const char moves[5] = "UDLR";
int si, sj, ti, tj;
double p;
char h[SIZE][SIZE];
char v[SIZE][SIZE];
bool can_move[SIZE][SIZE][4];
int dp[SIZE][SIZE][2];
int parent[SIZE][SIZE][3];
int num[SIZE][100];
double tmp[SIZE];
double prob[2][SIZE][SIZE];

void input() {
    scanf("%d %d %d %d %lf", &si, &sj, &ti, &tj, &p);
    for (int i = 0; i < SIZE; i++) scanf("%s", h[i]);
    for (int i = 0; i < SIZE - 1; i++) scanf("%s", v[i]);
}

bool check_move(int x1, int y1, int x2, int y2) {
    if (x2 < 0 || x2 >= SIZE || y2 < 0 || y2 >= SIZE) return false;
    if (x1 != x2) {
        return v[min(x1, x2)][y1] == '0';
    } else {
        return h[x1][min(y1, y2)] == '0';
    }
}

vector<pair<int, int>> get_route(int x, int y) {
    vector<pair<int, int>> v;
    while (x != si || y != sj) {
        int nx = parent[x][y][0];
        int ny = parent[x][y][1];
        int c = abs(x - nx) + abs(y - ny);
        v.emplace_back(parent[x][y][2], c);
        x = nx;
        y = ny;
    }
    reverse(v.begin(), v.end());
    return v;
}

int get_sum(const vector<pair<int, int>>& v) {
    int sum = 0;
    for (int i = 0; i < v.size(); i++) sum += v[i].second;
    return sum;
}

int score(const string& ans) {
    for (int j = 0; j < SIZE; j++) {
        for (int k = 0; k < SIZE; k++) {
            prob[0][j][k] = 0;
        }
    }
    prob[0][si][sj] = 1;
    
    double sum = 0;
    for (int i = 0; i < ans.size(); i++) {
        int d;
        if (ans[i] == 'U') {
            d = 0;
        } else if (ans[i] == 'D') {
            d = 1;
        } else if (ans[i] == 'L') {
            d = 2;
        } else {
            d = 3;
        }
        
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < SIZE; k++) {
                if (prob[0][j][k] == 0) continue;
                if (!can_move[j][k][d]) {
                    prob[1][j][k] += prob[0][j][k];
                } else {
                    int nx = j + dx[d];
                    int ny = k + dy[d];
                    prob[1][j][k] += prob[0][j][k] * p;
                    prob[1][nx][ny] += prob[0][j][k] * (1 - p);
                }
            }
        }
        
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < SIZE; k++) {
                prob[0][j][k] = prob[1][j][k];
                prob[1][j][k] = 0;
            }
        }
        sum += prob[0][ti][tj] * (400 - i);
        prob[0][ti][tj] = 0;
    }
    
    return round(250000 * sum);
}

int main() {
    simulated_annealing sa;
    
    input();
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < 4; k++) {
                can_move[i][j][k] = check_move(i, j, i + dx[k], j + dy[k]);
            }
        }
    }
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            dp[i][j][0] = dp[i][j][1] = INF;
        }
    }
    dp[si][sj][0] = dp[si][sj][1] = 0;
    
    priority_queue<state> q;
    q.emplace(si, sj, 0, 0);
    while (!q.empty()) {
        state s = q.top();
        q.pop();
        if (dp[s.x][s.y][0] != s.cnt || dp[s.x][s.y][1] != s.sum) continue;
        
        for (int i = 0; i < 4; i++) {
            int x = s.x, y = s.y;
            while (true) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (x == ti && y == tj) break;
                if (!can_move[x][y][i]) break;
                x = nx;
                y = ny;
            }
            if (x == s.x && y == s.y) continue;
            
            int cnt = s.cnt + 1;
            int sum = s.sum + abs(x - s.x) + abs(y - s.y);
            if (cnt >= TURN || sum >= TURN) continue;
            if (cnt < dp[x][y][0] || (cnt == dp[x][y][0] && sum < dp[x][y][1])) {
                dp[x][y][0] = cnt;
                dp[x][y][1] = sum;
                parent[x][y][0] = s.x;
                parent[x][y][1] = s.y;
                parent[x][y][2] = i;
                q.emplace(x, y, cnt, sum);
            }
        }
    }
    
    for (int i = 1; i < SIZE; i++) {
        for (int j = 70; j < 100; j++) {
            for (int k = 0; k <= i; k++) tmp[k] = 0;
            tmp[0] = 1;
            for (int k = 0; ; k++) {
                for (int l = i - 1; l >= 0; l--) {
                    tmp[l + 1] += tmp[l] * (1 - p);
                    tmp[l] *= p;
                }
                if (tmp[i] * 100 >= j) {
                    num[i][j] = k + 1;
                    break;
                }
            }
        }
    }
    
    string best_ans = "";
    for (int i = 0; i < TURN; i++) {
        int r = random::get(10);
        if (r < 4) {
            best_ans += "D";
        } else if (r < 8) {
            best_ans += "R";
        } else if (r < 9) {
            best_ans += "U";
        } else {
            best_ans += "L";
        }
    }
    int best_score = score(best_ans);
    
    if (dp[ti][tj][0] != INF) {
        vector<pair<int, int>> v = get_route(ti, tj);
        int sum = get_sum(v);
        
        int p = TURN / sum, q = TURN % sum / v.size(), r = TURN % sum % v.size();
        string ans = "";
        for (int i = 0; i < v.size(); i++) {
            int c = v[i].second * p + q;
            if (i < r) c++;
            ans += string(c, moves[v[i].first]);
        }
        int s = score(ans);
        if (s > best_score) {
            best_ans = ans;
            best_score = s;
        }
        
        for (int i = 70; i < 100; i++) {
            string ans = "";
            for (int j = 0; j < v.size(); j++) ans += string(num[v[j].second][i], moves[v[j].first]);
            if (ans.size() > TURN) break;
            while (ans.size() < TURN) ans += moves[random::get_fast(3)];
            int s = score(ans);
            if (s > best_score) {
                best_ans = ans;
                best_score = s;
            }
        }
    }
    
    for (int i = SIZE / 2; i < SIZE; i++) {
        for (int j = SIZE / 2; j < SIZE; j++) {
            if (dp[i][j][0] == INF) continue;
            
            vector<pair<int, int>> v = get_route(i, j);
            for (int k = 70; k < 100; k++) {
                string ans = "";
                for (int l = 0; l < v.size(); l++) ans += string(num[v[l].second][k], moves[v[l].first]);
                if (ans.size() > TURN) break;
                while (ans.size() < TURN) ans += moves[random::get_fast(3)];
                int s = score(ans);
                if (s > best_score) {
                    best_ans = ans;
                    best_score = s;
                }
            }
        }
    }
    
    for (int i = 0; i < 4; i++) {
        int x = ti, y = tj;
        while (true) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (!can_move[x][y][i]) break;
            for (int j = 0; j < 4; j++) {
                if (i / 2 == j / 2) continue;
                int nx1 = nx, ny1 = ny;
                while (true) {
                    int nnx1 = nx1 + dx[j];
                    int nny1 = ny1 + dy[j];
                    if (!can_move[nx1][ny1][j]) break;
                    if (can_move[nnx1][nny1][i ^ 1]) break;
                    if (dp[nnx1][nny1][0] != INF) {
                        int c1 = abs(nx - nnx1) + abs(ny - nny1);
                        int c2 = abs(ti - nx) + abs(tj - ny);
                        int d1 = j ^ 1;
                        int d2 = i ^ 1;
                        vector<pair<int, int>> v = get_route(nnx1, nny1);
                        int sum = get_sum(v);
                        
                        int p = (TURN - 30) / sum;
                        string ans = "";
                        for (int k = 0; k < v.size(); k++) ans += string(v[k].second * p, moves[v[k].first]);
                        if (c1 > 1) ans += string(c1 - 1, moves[d1]);
                        string last = "";
                        last += string(1, moves[d1]);
                        last += string(c2 * 2, moves[d2]);
                        while (ans.size() + last.size() <= TURN) ans += last;
                        while (ans.size() < TURN) ans += moves[d2];
                        int s = score(ans);
                        if (s > best_score) {
                            best_ans = ans;
                            best_score = s;
                        }
                        
                        for (int k = 70; k < 100; k++) {
                            string ans = "";
                            for (int l = 0; l < v.size(); l++) ans += string(num[v[l].second][k], moves[v[l].first]);
                            if (c1 > 1) ans += string(c1 - 1, moves[d1]);
                            if (ans.size() > TURN) break;
                            string last = "";
                            last += string(1, moves[d1]);
                            last += string(num[c2][k], moves[d2]);
                            while (ans.size() + last.size() <= TURN) ans += last;
                            while (ans.size() < TURN) ans += moves[d2];
                            int s = score(ans);
                            if (s > best_score) {
                                best_ans = ans;
                                best_score = s;
                            }
                        }
                    }
                    nx1 = nnx1;
                    ny1 = nny1;
                }
            }
            x = nx;
            y = ny;
        }
    }
    
    string ans = best_ans;
    int current_score = best_score;
    while (!sa.end()) {
        string tmp;
        int r = random::get(3);
        if (r == 0) {
            int x = random::get(TURN);
            int y = random::get_fast(3);
            if (ans[x] == moves[y]) continue;
            tmp = ans;
            tmp[x] = moves[y];
        } else if (r == 1) {
            int x = random::get(TURN);
            int y = random::get_fast(3);
            tmp = ans.substr(0, x) + ans.substr(x + 1) + moves[y];
        } else {
            int x = random::get(TURN);
            int y = random::get_fast(3);
            tmp = ans.substr(0, x) + moves[y] + ans.substr(x, TURN - x - 1);
        }
        int s = score(tmp);
        if (sa.accept(current_score, s)) {
            ans = tmp;
            current_score = s;
            if (current_score > best_score) {
                best_ans = ans;
                best_score = current_score;
            }
        }
    }
    
    printf("%s\n", best_ans.c_str());
    fprintf(stderr, "%d\n", best_score);
    
    return 0;
}
