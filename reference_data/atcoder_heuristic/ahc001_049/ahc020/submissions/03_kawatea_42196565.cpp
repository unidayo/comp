#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <queue>
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

class simulated_annealing {
    public:
    simulated_annealing();
    inline bool end();
    inline bool accept(double current_score, double next_score);
    void print() const;
    
    private:
    constexpr static bool MAXIMIZE = false;
    constexpr static int LOG_SIZE = 0xFFFF;
    constexpr static int UPDATE_INTERVAL = 0xFF;
    constexpr static double TIME_LIMIT = 1.95;
    constexpr static double START_TEMP = 500000;
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

const int N = 100;
const int MAX_M = 300;
const int MAX_K = 5000;
const int INF = 1e9;
int M, K;
int x[N];
int y[N];
int u[MAX_M];
int v[MAX_M];
int w[MAX_M];
int a[MAX_K];
int b[MAX_K];
vector<int> out[N];
int dist_p[MAX_K][N];
int best[MAX_K];
int degree[N];
bool used[N];
bool connected[N];
int dist[N];
int parent[N][2];
int best_p[N];
int best_b[MAX_M];
int best_best[MAX_K];
int best_degree[N];
bool best_used[N];
int P[N];
int B[MAX_M];

void input() {
    scanf("%*d %d %d", &M, &K);
    for (int i = 0; i < N; i++) scanf("%d %d", &x[i], &y[i]);
    for (int i = 0; i < M; i++) {
        scanf("%d %d %d", &u[i], &v[i], &w[i]);
        u[i]--;
        v[i]--;
        out[u[i]].push_back(i);
        out[v[i]].push_back(i);
    }
    for (int i = 0; i < K; i++) scanf("%d %d", &a[i], &b[i]);
}

void output() {
    for (int i = 0; i < N; i++) {
        if (i > 0) putchar(' ');
        printf("%d", best_p[i]);
    }
    puts("");
    for (int i = 0; i < M; i++) {
        if (i > 0) putchar(' ');
        printf("%d", best_b[i]);
    }
    puts("");
}

void calc_dist_p() {
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            dist_p[i][j] = ceil(sqrt((x[j] - a[i]) * (x[j] - a[i]) + (y[j] - b[i]) * (y[j] - b[i])));
        }
    }
}

long long calc_s() {
    long long s = 0;
    for (int i = 0; i < N; i++) s += best_p[i] * best_p[i];
    for (int i = 0; i < M; i++) {
        if (best_b[i] == 1) s += w[i];
    }
    return s;
}

long long calc_score() {
    return round(1e6 * (1 + 1e8 / (calc_s() + 1e7)));
}

int main() {
    simulated_annealing sa;
    
    input();
    
    calc_dist_p();
    
    for (int i = 0; i < N; i++) P[i] = 500;
    for (int i = 0; i < K; i++) {
        int best_diff = INF;
        best[i] = -1;
        for (int j = 0; j < N; j++) {
            if (dist_p[i][j] < P[j]) {
                best[i] = j;
                best_diff = 0;
                break;
            } else if (dist_p[i][j] <= 5000) {
                int now_diff = -P[j] * P[j] + dist_p[i][j] * dist_p[i][j];
                if (now_diff < best_diff) {
                    best[i] = j;
                    best_diff = now_diff;
                }
            }
        }
        P[best[i]] = max(P[best[i]], dist_p[i][best[i]]);
    }
    
    for (int i = 0; i < N; i++) used[i] = true;
    for (int t = 0; t < 10; t++) {
        for (int target = 0; target < N; target++) {
            if (!used[target]) continue;
            int diff = 0;
            bool ng = false;
            vector<int> changed;
            vector<pair<int, int>> changed_p;
            used[target] = false;
            int cnt = 0;
            for (int i = 0; i < K; i++) {
                if (best[i] != target) continue;
                int best_diff = INF;
                best[i] = -1;
                for (int j = 0; j < N; j++) {
                    if (!used[j]) continue;
                    if (dist_p[i][j] < P[j]) {
                        best[i] = j;
                        best_diff = 0;
                        break;
                    } else if (dist_p[i][j] <= 5000) {
                        int now_diff = -P[j] * P[j] + dist_p[i][j] * dist_p[i][j];
                        if (now_diff < best_diff) {
                            best[i] = j;
                            best_diff = now_diff;
                        }
                    }
                }
                changed.push_back(i);
                if (best[i] == -1) {
                    ng = true;
                    break;
                }
                if (best_diff > 0) {
                    changed_p.emplace_back(best[i], P[best[i]]);
                    P[best[i]] = dist_p[i][best[i]];
                    diff += best_diff;
                }
            }
            
            if (ng || diff > 0) {
                used[target] = true;
                for (int i : changed) best[i] = target;
                for (int i = (int)changed_p.size() - 1; i >= 0; i--) P[changed_p[i].first] = changed_p[i].second;
            } else {
                P[target] = 0;
            }
        }
    }
    
    connected[0] = true;
    while (true) {
        bool reachable = true;
        for (int i = 0; i < N; i++) {
            if (used[i] && !connected[i]) {
                reachable = false;
                break;
            }
        }
        if (reachable) break;
        
        priority_queue<pair<int, int>> pq;
        for (int i = 0; i < N; i++) {
            if (connected[i]) {
                dist[i] = 0;
                parent[i][0] = parent[i][1] = -1;
                pq.emplace(-dist[i], i);
            } else {
                dist[i] = INF;
            }
        }
        while (!pq.empty()) {
            int now = pq.top().second;
            int d = -pq.top().first;
            pq.pop();
            if (!connected[now] && used[now]) {
                while (parent[now][0] >= 0) {
                    connected[now] = true;
                    B[parent[now][1]] = true;
                    degree[u[parent[now][1]]]++;
                    degree[v[parent[now][1]]]++;
                    now = parent[now][0];
                }
                break;
            }
            
            for (int e : out[now]) {
                int next = ((u[e] == now) ? v[e] : u[e]);
                if (dist[next] > dist[now] + w[e]) {
                    dist[next] = dist[now] + w[e];
                    parent[next][0] = now;
                    parent[next][1] = e;
                    pq.emplace(-dist[next], next);
                }
            }
        }
    }
    
    for (int i = 0; i < N; i++) {
        if (degree[i] > 0) used[i] = true;
    }
    
    for (int i = 0; i < N; i++) best_p[i] = P[i];
    for (int i = 0; i < M; i++) best_b[i] = B[i];
    for (int i = 0; i < K; i++) best_best[i] = best[i];
    for (int i = 0; i < N; i++) best_degree[i] = degree[i];
    for (int i = 0; i < N; i++) best_used[i] = used[i];
    long long best_s = calc_s(), current_s = best_s;
    
    int cnt = 0;
    vector<int> changed;
    vector<pair<int, int>> changed_p;
    while (!sa.end()) {
        cnt++;
        if (cnt == 10000) {
            current_s = best_s;
            for (int i = 0; i < N; i++) P[i] = best_p[i];
            for (int i = 0; i < M; i++) B[i] = best_b[i];
            for (int i = 0; i < K; i++) best[i] = best_best[i];
            for (int i = 0; i < N; i++) degree[i] = best_degree[i];
            for (int i = 0; i < N; i++) used[i] = best_used[i];
            cnt = 0;
        }
        int r = random::get(100);
        if (r < 85) {
            int target = random::get(N);
            if (P[target] == 0) continue;
            int reduce = random::get(1, min(P[target], 1000));
            if (random::get(10) == 0) reduce = P[target];
            changed.clear();
            changed_p.clear();
            int diff = -P[target] * P[target] + (P[target] - reduce) * (P[target] - reduce);
            bool ng = false;
            for (int i = 0; i < K; i++) {
                if (best[i] != target || dist_p[i][target] <= P[target] - reduce) continue;
                int best_diff = INF;
                best[i] = -1;
                for (int j = 0; j < N; j++) {
                    if (!used[j] || j == target) continue;
                    if (dist_p[i][j] < P[j]) {
                        best[i] = j;
                        best_diff = 0;
                        break;
                    } else if (dist_p[i][j] <= 5000) {
                        int now_diff = -P[j] * P[j] + dist_p[i][j] * dist_p[i][j];
                        if (now_diff < best_diff) {
                            best[i] = j;
                            best_diff = now_diff;
                        }
                    }
                }
                changed.push_back(i);
                if (best[i] == -1) {
                    ng = true;
                    break;
                }
                if (best_diff > 0) {
                    changed_p.emplace_back(best[i], P[best[i]]);
                    P[best[i]] = dist_p[i][best[i]];
                    diff += best_diff;
                }
            }
            
            if (ng || !sa.accept(0, diff)) {
                for (int i : changed) best[i] = target;
                for (int i = (int)changed_p.size() - 1; i >= 0; i--) P[changed_p[i].first] = changed_p[i].second;
            } else {
                P[target] -= reduce;
                current_s += diff;
                if (current_s < best_s) {
                    best_s = current_s;
                    for (int i = 0; i < N; i++) best_p[i] = P[i];
                    for (int i = 0; i < M; i++) best_b[i] = B[i];
                    for (int i = 0; i < K; i++) best_best[i] = best[i];
                    for (int i = 0; i < N; i++) best_degree[i] = degree[i];
                    for (int i = 0; i < N; i++) best_used[i] = used[i];
                    cnt = 0;
                }
            }
        } else if (r < 95) {
            int target = random::get(1, N - 1);
            if (degree[target] != 1) continue;
            int e, next, diff = -P[target] * P[target];
            for (int i : out[target]) {
                if (B[i] == 1) {
                    e = i;
                    next = ((u[i] == target) ? v[i] : u[i]);
                    diff -= w[i];
                }
            }
            
            if (P[target] == 0) {
                B[e] = 0;
                degree[target]--;
                degree[next]--;
                used[target] = false;
                current_s += diff;
                if (current_s < best_s) {
                    best_s = current_s;
                    for (int i = 0; i < N; i++) best_p[i] = P[i];
                    for (int i = 0; i < M; i++) best_b[i] = B[i];
                    for (int i = 0; i < K; i++) best_best[i] = best[i];
                    for (int i = 0; i < N; i++) best_degree[i] = degree[i];
                    for (int i = 0; i < N; i++) best_used[i] = used[i];
                    cnt = 0;
                }
            } else {
                bool ng = false;
                changed.clear();
                changed_p.clear();
                used[target] = false;
                for (int i = 0; i < K; i++) {
                    if (best[i] != target) continue;
                    int best_diff = INF;
                    best[i] = -1;
                    for (int j = 0; j < N; j++) {
                        if (!used[j]) continue;
                        if (dist_p[i][j] < P[j]) {
                            best[i] = j;
                            best_diff = 0;
                            break;
                        } else if (dist_p[i][j] <= 5000) {
                            int now_diff = -P[j] * P[j] + dist_p[i][j] * dist_p[i][j];
                            if (now_diff < best_diff) {
                                best[i] = j;
                                best_diff = now_diff;
                            }
                        }
                    }
                    changed.push_back(i);
                    if (best[i] == -1) {
                        ng = true;
                        break;
                    }
                    if (best_diff > 0) {
                        changed_p.emplace_back(best[i], P[best[i]]);
                        P[best[i]] = dist_p[i][best[i]];
                        diff += best_diff;
                    }
                }
                
                if (ng || !sa.accept(0, diff)) {
                    used[target] = true;
                    for (int i : changed) best[i] = target;
                    for (int i = (int)changed_p.size() - 1; i >= 0; i--) P[changed_p[i].first] = changed_p[i].second;
                } else {
                    B[e] = 0;
                    P[target] = 0;
                    degree[target]--;
                    degree[next]--;
                    current_s += diff;
                    if (current_s < best_s) {
                        best_s = current_s;
                        for (int i = 0; i < N; i++) best_p[i] = P[i];
                        for (int i = 0; i < M; i++) best_b[i] = B[i];
                        for (int i = 0; i < K; i++) best_best[i] = best[i];
                        for (int i = 0; i < N; i++) best_degree[i] = degree[i];
                        for (int i = 0; i < N; i++) best_used[i] = used[i];
                        cnt = 0;
                    }
                }
            }
        }
    }
    
    output();
    
    fprintf(stderr, "%lld\n", calc_score());
    
    return 0;
}
