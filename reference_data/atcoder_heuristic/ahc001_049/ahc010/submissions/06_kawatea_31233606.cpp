#include <cstdio>
#include <cmath>
#include <vector>
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
    constexpr static double TIME_LIMIT = 0.99;
    constexpr static double START_TEMP = 1;
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

struct step {
    int p;
    int d;
    
    step() {}
    step(int p, int d) : p(p), d(d) {}
};

const int SIZE = 30;
const int SIZE2 = SIZE * SIZE;
const int dp[4] = {-1, -SIZE, 1, SIZE};
const int to[8][4] = {
    1, 0, -1, -1,
    3, -1, -1, 0,
    -1, -1, 3, 2,
    -1, 2, 1, -1,
    1, 0, 3, 2,
    3, 2, 1, 0,
    2, -1, 0, -1,
    -1, 3, -1, 1
};

int t[SIZE2];
int turns[SIZE2];
int tmp_turns[SIZE2];
int used[SIZE2];
int tmp_used[SIZE2];
int best_used[SIZE2];
int best_turns[SIZE2];
bool visited[SIZE2][4];
bool ng[SIZE2][4];

void input() {
    for (int i = 0; i < SIZE2; i++) {
        scanf("%1d", &t[i]);
        if (t[i] == 1) {
            t[i] = 0;
            best_turns[i] = 3;
        } else if (t[i] == 2) {
            t[i] = 0;
            best_turns[i] = 2;
        } else if (t[i] == 3) {
            t[i] = 0;
            best_turns[i] = 1;
        } else if (t[i] == 5) {
            t[i] = 4;
            best_turns[i] = 1;
        } else if (t[i] == 7) {
            t[i] = 6;
            best_turns[i] = 1;
        }
    }
    
    for (int i = 0; i < SIZE; i++) {
        ng[SIZE * i][0] = true;
        ng[i][1] = true;
        ng[SIZE2 - SIZE * i - 1][2] = true;
        ng[SIZE2 - i - 1][3] = true;
    }
}

pair<int, vector<int>> search(int l, int r) {
    simulated_annealing sa;
    
    vector<int> best_turns(SIZE2, 0);
    vector<step> steps, best_steps;
    for (int i = 0; i < 1000; i++) {
        for (int j = l; j < r; j++) used[j] = 0;
        
        int sp = random::get(l, r - 1);
        int sd;
        used[sp]++;
        if (t[sp] == 0) {
            turns[sp] = random::get_fast(3);
            sd = (4 - turns[sp]) & 3;
        } else if (t[sp] == 4) {
            turns[sp] = random::get_fast(1);
            sd = random::get_fast(3);
        } else {
            turns[sp] = random::get_fast(1);
            sd = turns[sp];
        }
        
        int p = sp, d = sd, len = 0;
        steps.clear();
        while (true) {
            if (ng[p][d]) break;
            steps.emplace_back(p, d);
            if (used[p] == 0) {
                if (t[p] == 0) {
                    int t1 = (4 - d) & 3;
                    int t2 = (5 - d) & 3;
                    if (to[t[p] + t1][d] == -1) {
                        turns[p] = t2;
                    } else if (to[t[p] + t2][d] == -1) {
                        turns[p] = t1;
                    } else {
                        turns[p] = (4 + random::get_fast(1) - d) & 3;
                    }
                } else if (t[p] == 4) {
                    if (to[t[p]][d] == -1) {
                        turns[p] = 1;
                    } else if (to[t[p] + 1][d] == -1) {
                        turns[p] = 0;
                    } else {
                        turns[p] = random::get_fast(1);
                    }
                } else {
                    turns[p] = d & 1;
                }
            }
            used[p]++;
            d = to[t[p] + turns[p]][d];
            if (d == -1) break;
            p += dp[d];
            if (p < l || p >= r) break;
            d = (d + 2) & 3;
            if (p == sp && d == sd) {
                if (steps.size() > best_steps.size()) {
                    used[sp]--;
                    for (int j = l; j < r; j++) {
                        best_used[j] = used[j];
                        best_turns[j] = turns[j];
                    }
                    best_steps = steps;
                }
                break;
            }
        }
    }
    
    vector<step> tmp_steps;
    steps = best_steps;
    for (int i = l; i < r; i++) {
        used[i] = best_used[i];
        turns[i] = best_turns[i];
    }
    while (!sa.end()) {
        int s = random::get(steps.size());
        int len = random::get(5, min(20, (int)steps.size() - 2));
        int e = s + len;
        if (e >= steps.size()) e -= steps.size();
        int sp = steps[s].p;
        int sd = steps[s].d;
        int tp = steps[e].p;
        int td = steps[e].d;
        for (int i = l; i < r; i++) {
            tmp_used[i] = used[i];
            tmp_turns[i] = turns[i];
        }
        for (int i = 0, pos = s; i < len; i++, pos++) {
            if (pos == steps.size()) pos = 0;
            tmp_used[steps[pos].p]--;
        }
        
        int p = sp, d = sd;
        tmp_steps.clear();
        while (true) {
            if (ng[p][d]) break;
            tmp_steps.emplace_back(p, d);
            if (tmp_used[p] == 0) {
                if (t[p] == 0) {
                    int t1 = (4 - d) & 3;
                    int t2 = (5 - d) & 3;
                    if (to[t[p] + t1][d] == -1) {
                        tmp_turns[p] = t2;
                    } else if (to[t[p] + t2][d] == -1) {
                        tmp_turns[p] = t1;
                    } else {
                        tmp_turns[p] = (4 + random::get_fast(1) - d) & 3;
                    }
                } else if (t[p] == 4) {
                    if (to[t[p]][d] == -1) {
                        tmp_turns[p] = 1;
                    } else if (to[t[p] + 1][d] == -1) {
                        tmp_turns[p] = 0;
                    } else {
                        tmp_turns[p] = random::get_fast(1);
                    }
                } else {
                    tmp_turns[p] = d & 1;
                }
            }
            tmp_used[p]++;
            d = to[t[p] + tmp_turns[p]][d];
            if (d == -1) break;
            p += dp[d];
            if (p < l || p >= r) break;
            d = (d + 2) & 3;
            if (p == tp && d == td) {
                if (sa.accept(best_steps.size(), steps.size() - len + tmp_steps.size())) {
                    for (int i = l; i < r; i++) {
                        used[i] = tmp_used[i];
                        turns[i] = tmp_turns[i];
                    }
                    for (int i = e; ; i++) {
                        if (i >= steps.size()) i -= steps.size();
                        if (i == s) break;
                        tmp_steps.push_back(steps[i]);
                    }
                    steps.swap(tmp_steps);
                    
                    if (steps.size() > best_steps.size()) {
                        for (int i = l; i < r; i++) {
                            best_used[i] = used[i];
                            best_turns[i] = turns[i];
                        }
                        best_steps = steps;
                    }
                }
                break;
            }
        }
    }
    return make_pair(best_steps.size(), best_turns);
}

int calc_length(int p, int d) {
    int sp = p, sd = d, len = 0;
    while (true) {
        if (ng[p][d]) return 0;
        visited[p][d] = true;
        d = to[t[p]][d];
        if (d == -1) return 0;
        visited[p][d] = true;
        p += dp[d];
        if (p < 0 || p >= SIZE2) return 0;
        d = (d + 2) & 3;
        len++;
        if (p == sp && d == sd) return len;
    }
    return 0;
}

int score() {
    int len1 = 0, len2 = 0;
    for (int i = 0; i < SIZE2; i++) {
        for (int j = 0; j < 4; j++) {
            visited[i][j] = false;
        }
    }
    for (int i = 0; i < SIZE2; i++) {
        for (int j = 0; j < 4; j++) {
            if (!visited[i][j]) {
                int len = calc_length(i, j);
                if (len > len1) {
                    len2 = len1;
                    len1 = len;
                } else if (len > len2) {
                    len2 = len;
                }
            }
        }
    }
    return len1 * len2;
}

int main() {
    input();
    
    pair<int, vector<int>> p1 = search(0, SIZE2 / 2);
    pair<int, vector<int>> p2 = search(SIZE2 / 2, SIZE2);
    for (int i = 0; i < SIZE2; i++) {
        int turn = p1.second[i] + p2.second[i];
        if (t[i] == 0) {
            t[i] += turn & 3;
        } else {
            t[i] += turn & 1;
        }
        best_turns[i] = (best_turns[i] + turn) & 3;
    }
    /*
    pair<int, vector<vector<int>>> p3 = search(0, 0, SIZE, SIZE / 2);
    pair<int, vector<vector<int>>> p4 = search(0, SIZE / 2, SIZE, SIZE);
    if (p1.first * p2.first > p3.first * p4.first) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                int turn = p1.second[i][j] + p2.second[i][j];
                if (t[i][j] == 0) {
                    t[i][j] += turn & 3;
                } else {
                    t[i][j] += turn & 1;
                }
                best_turns[i][j] = (best_turns[i][j] + turn) & 3;
            }
        }
    } else {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                int turn = p3.second[i][j] + p4.second[i][j];
                if (t[i][j] == 0) {
                    t[i][j] += turn & 3;
                } else {
                    t[i][j] += turn & 1;
                }
                best_turns[i][j] = (best_turns[i][j] + turn) & 3;
            }
        }
    }
    */
    
    for (int i = 0; i < SIZE2; i++) printf("%d", best_turns[i]);
    puts("");
    
    fprintf(stderr, "%d\n", score());
    
    return 0;
}
