#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

const int N = 20;
const int N2 = N * N;
const int NUM = N2 / 2;

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
    constexpr static double SECONDS_PER_CLOCK = 1 / 2.4e9;
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
    constexpr static int LOG_SIZE = 0x10000;
    constexpr static int UPDATE_INTERVAL = 0xFF;
    constexpr static double TIME_LIMIT = 1.99;
    constexpr static double START_TEMP = 1.25;
    constexpr static double END_TEMP = 1e-9;
    constexpr static double TEMP_RATIO = (END_TEMP - START_TEMP) / TIME_LIMIT;
    double log_probability[LOG_SIZE];
    long long iteration = 0;
    long long accepted = 0;
    long long rejected = 0;
    double time = 0;
    double temp = START_TEMP;
    timer sa_timer;
};

simulated_annealing::simulated_annealing() {
    sa_timer.start();
    double inv = 1.0 / LOG_SIZE;
    for (int i = 0; i < LOG_SIZE; i++) log_probability[i] = log((i + 0.5) * inv);
    mt19937 engine;
    shuffle(log_probability, log_probability + LOG_SIZE, engine);
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
    static unsigned short index = 0;
    if (diff >= 0 || diff > log_probability[index++] * temp) {
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

int a[N][N];
int origin[N2][2];
int dist[N2 + 2][N2 + 2];
int rev[N2 + 2];

void input() {
    scanf("%*d");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) scanf("%d", &a[i][j]);
    }
}

void output(const vector<int>& ans) {
    int x = 0, y = 0;
    for (int i = 1; i + 1 < ans.size(); i++) {
        int t = ans[i], nx = origin[t][0], ny = origin[t][1];
        while (x > nx) {
            puts("U");
            x--;
        }
        while (x < nx) {
            puts("D");
            x++;
        }
        while (y > ny) {
            puts("L");
            y--;
        }
        while (y < ny) {
            puts("R");
            y++;
        }
        puts("Z");
    }
}

void init() {
    bool checked[NUM] = {};
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (checked[a[i][j]]) {
                a[i][j] += NUM;
            } else {
                checked[a[i][j]] = true;
            }
            origin[a[i][j]][0] = i;
            origin[a[i][j]][1] = j;
            rev[a[i][j]] = (a[i][j] + NUM) % N2;
        }
    }
    rev[N2] = N2 + 1;
    rev[N2 + 1] = N2;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dist[N2][a[i][j]] = i + j;
            dist[a[i][j]][N2 + 1] = 0;
            for (int k = 0; k < N; k++) {
                for (int l = 0; l < N; l++) {
                    dist[a[i][j]][a[k][l]] = abs(i - k) + abs(j - l);
                }
            }
        }
    }
}

int score(const vector<int>& ans) {
    int K = 0;
    for (int i = 0; i + 1 < ans.size(); i++) K += dist[ans[i]][ans[i + 1]];
    return N * N + 2 * N * N * N - K;
}

vector<int> greedy() {
    bool picked[NUM] = {};
    vector<int> v, ans;
    ans.push_back(N2);
    for (int x = 0; x < N; x++) {
        if (x % 2 == 0) {
            for (int y = 0; y < N; y++) {
                if (!picked[a[x][y] % NUM]) {
                    picked[a[x][y] % NUM] = true;
                    v.push_back(a[x][y]);
                    ans.push_back(a[x][y]);
                } else if (!v.empty() && v.back() == rev[a[x][y]]) {
                    v.pop_back();
                    ans.push_back(a[x][y]);
                }
            }
        } else {
            for (int y = N - 1; y >= 0; y--) {
                if (!picked[a[x][y] % NUM]) {
                    picked[a[x][y] % NUM] = true;
                    v.push_back(a[x][y]);
                    ans.push_back(a[x][y]);
                } else if (!v.empty() && v.back() == rev[a[x][y]]) {
                    v.pop_back();
                    ans.push_back(a[x][y]);
                }
            }
        }
    }
    while (!v.empty()) {
        ans.push_back(rev[v.back()]);
        v.pop_back();
    }
    ans.push_back(N2 + 1);
    return ans;
}

int pos[N2 + 2];
int height[NUM + 1];
int height_next[N2 + 2];
int height_prev[N2 + 2];

void calc(const vector<int>& ans) {
    for (int i = 0; i < ans.size(); i++) pos[i] = height_next[i] = height_prev[i] = -1;
    for (int i = 0; i <= NUM; i++) height[i] = -1;
    int h = 0;
    height[0] = 0;
    for (int i = 0; i < ans.size(); i++) {
        pos[ans[i]] = i;
        if (pos[rev[ans[i]]] == -1) {
            h++;
            height[h] = i + 1;
        } else {
            h--;
            height[h + 1] = -1;
            if (height[h] != -1) {
                height_next[height[h]] = i + 1;
                height_prev[i + 1] = height[h];
            }
        }
    }
}

int main() {
    simulated_annealing sa;
    
    input();
    
    init();
    
    int updated = 0;
    vector<int> ans = greedy(), best_ans = ans, tmp(ans);
    int current_score = score(ans), best_score = current_score;
    calc(ans);
    while (!sa.end()) {
        updated++;
        if (updated == 5000000) {
            current_score = best_score;
            ans = best_ans;
            calc(ans);
        }
        int select = random::get(100);
        if (select < 95) {
            int target1 = random::get(NUM), target2 = rev[target1], pos1 = pos[target1], pos2 = pos[target2], pos3 = random::get(1, N2 + 1);
            if (pos3 == pos1 || pos3 == pos1 + 1 || pos3 == pos2 || pos3 == pos2 + 1) continue;
            if (pos1 > pos2) {
                swap(target1, target2);
                swap(pos1, pos2);
            }
            int next_score = current_score;
            if (pos1 + 1 == pos2) {
                next_score += dist[ans[pos1 - 1]][target1];
                next_score += dist[target1][target2];
                next_score += dist[target2][ans[pos2 + 1]];
                next_score -= dist[ans[pos1 - 1]][ans[pos2 + 1]];
            } else {
                next_score += dist[ans[pos1 - 1]][target1];
                next_score += dist[target1][ans[pos1 + 1]];
                next_score -= dist[ans[pos1 - 1]][ans[pos1 + 1]];
                next_score += dist[ans[pos2 - 1]][target2];
                next_score += dist[target2][ans[pos2 + 1]];
                next_score -= dist[ans[pos2 - 1]][ans[pos2 + 1]];
            }
            int h = height[pos3], pos4, best_diff = 1e9;
            bool dir, prev;
            for (int pos = pos3; pos != -1; pos = height_next[pos]) {
                int diff1 = 0, diff2 = 0;
                if (pos == pos1 || pos == pos1 + 1 || pos == pos2 || pos == pos2 + 1) continue;
                if (pos == pos3) {
                    diff1 += dist[ans[pos - 1]][target1];
                    diff1 += dist[target1][target2];
                    diff1 += dist[target2][ans[pos]];
                    diff1 -= dist[ans[pos - 1]][ans[pos]];
                    diff2 += dist[ans[pos - 1]][target2];
                    diff2 += dist[target2][target1];
                    diff2 += dist[target1][ans[pos]];
                    diff2 -= dist[ans[pos - 1]][ans[pos]];
                } else {
                    diff1 += dist[ans[pos3 - 1]][target1];
                    diff1 += dist[target1][ans[pos3]];
                    diff1 -= dist[ans[pos3 - 1]][ans[pos3]];
                    diff1 += dist[ans[pos - 1]][target2];
                    diff1 += dist[target2][ans[pos]];
                    diff1 -= dist[ans[pos - 1]][ans[pos]];
                    diff2 += dist[ans[pos3 - 1]][target2];
                    diff2 += dist[target2][ans[pos3]];
                    diff2 -= dist[ans[pos3 - 1]][ans[pos3]];
                    diff2 += dist[ans[pos - 1]][target1];
                    diff2 += dist[target1][ans[pos]];
                    diff2 -= dist[ans[pos - 1]][ans[pos]];
                }
                if (diff1 < best_diff) {
                    best_diff = diff1;
                    pos4 = pos;
                    dir = true;
                    prev = false;
                }
                if (diff2 < best_diff) {
                    best_diff = diff2;
                    pos4 = pos;
                    dir = false;
                    prev = false;
                }
            }
            for (int pos = height_prev[pos3]; pos != -1; pos = height_prev[pos]) {
                int diff1 = 0, diff2 = 0;
                if (pos == pos1 || pos == pos1 + 1 || pos == pos2 || pos == pos2 + 1) continue;
                diff1 += dist[ans[pos - 1]][target1];
                diff1 += dist[target1][ans[pos]];
                diff1 -= dist[ans[pos - 1]][ans[pos]];
                diff1 += dist[ans[pos3 - 1]][target2];
                diff1 += dist[target2][ans[pos3]];
                diff1 -= dist[ans[pos3 - 1]][ans[pos3]];
                diff2 += dist[ans[pos - 1]][target2];
                diff2 += dist[target2][ans[pos]];
                diff2 -= dist[ans[pos - 1]][ans[pos]];
                diff2 += dist[ans[pos3 - 1]][target1];
                diff2 += dist[target1][ans[pos3]];
                diff2 -= dist[ans[pos3 - 1]][ans[pos3]];
                if (diff1 < best_diff) {
                    best_diff = diff1;
                    pos4 = pos;
                    dir = true;
                    prev = true;
                }
                if (diff2 < best_diff) {
                    best_diff = diff2;
                    pos4 = pos;
                    dir = false;
                    prev = true;
                }
            }
            next_score -= best_diff;
            if (sa.accept(current_score, next_score)) {
                current_score = next_score;
                if (prev) swap(pos3, pos4);
                ans.erase(ans.begin() + pos2);
                ans.erase(ans.begin() + pos1);
                if (pos3 >= pos2) pos3--;
                if (pos3 >= pos1) pos3--;
                if (pos4 >= pos2) pos4--;
                if (pos4 >= pos1) pos4--;
                if (dir) {
                    ans.insert(ans.begin() + pos4, target2);
                    ans.insert(ans.begin() + pos3, target1);
                } else {
                    ans.insert(ans.begin() + pos4, target1);
                    ans.insert(ans.begin() + pos3, target2);
                }
                calc(ans);
            }
        } else {
            int target1 = random::get(NUM), target2 = rev[target1], pos1 = pos[target1], pos2 = pos[target2];
            if (pos1 > pos2) {
                swap(target1, target2);
                swap(pos1, pos2);
            }
            int next_score = current_score;
            next_score += dist[ans[pos1 - 1]][target1];
            next_score += dist[target2][ans[pos2 + 1]];
            next_score -= dist[ans[pos1 - 1]][target2];
            next_score -= dist[target1][ans[pos2 + 1]];
            if (sa.accept(current_score, next_score)) {
                current_score = next_score;
                reverse(ans.begin() + pos1, ans.begin() + pos2 + 1);
                calc(ans);
            }
        }
        if (current_score > best_score) {
            updated = 0;
            best_score = current_score;
            best_ans = ans;
        }
    }
    
    output(best_ans);
    
    fprintf(stderr, "%d\n", best_score);
    
    return 0;
}
