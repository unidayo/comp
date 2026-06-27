#include <cstdio>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

const int N = 1000;
const int M = 50;

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
    constexpr static bool MAXIMIZE = false;
    constexpr static int LOG_SIZE = 0xFFFF;
    constexpr static int UPDATE_INTERVAL = 0xFF;
    constexpr static double TIME_LIMIT = 1.95;
    constexpr static double START_TEMP = 25;
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

int a[N + 1];
int b[N + 1];
int c[N + 1];
int d[N + 1];

void input() {
    for (int i = 1; i <= N; i++) scanf("%d %d %d %d", &a[i], &b[i], &c[i], &d[i]);
}

int dist(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

int calc_order_score(const vector<pair<int, pair<int, int>>>& order) {
    int sum = 0;
    for (int i = 0; i + 1 < order.size(); i++) sum += dist(order[i].second.first, order[i].second.second, order[i + 1].second.first, order[i + 1].second.second);
    return sum;
}

pair<vector<int>, vector<pair<int, int>>> calc_route() {
    vector<int> subset, out;
    for (int i = 0; i < M; i++) subset.push_back(i + 1);
    for (int i = M; i < N; i++) out.push_back(i + 1);
    
    vector<pair<int, pair<int, int>>> order;
    vector<pair<int, pair<int, int>>> pick, post;
    for (int p : subset) {
        pick.push_back(make_pair(a[p], make_pair(b[p], p)));
        post.push_back(make_pair(c[p], make_pair(d[p], p)));
    }
    sort(pick.begin(), pick.end());
    sort(post.begin(), post.end(), greater<pair<int, pair<int, int>>>());
    order.push_back(make_pair(0, make_pair(400, 400)));
    for (int i = 0; i < pick.size(); i++) {
        int p = pick[i].second.second;
        int x = pick[i].first;
        int y = pick[i].second.first;
        order.push_back(make_pair(p, make_pair(x, y)));
    }
    for (int i = 0; i < post.size(); i++) {
        int p = post[i].second.second;
        int x = post[i].first;
        int y = post[i].second.first;
        order.push_back(make_pair(-p, make_pair(x, y)));
    }
    order.push_back(make_pair(0, make_pair(400, 400)));
    
    int position[N * 2 + 2];
    for (int i = 0; i < order.size(); i++) position[order[i].first + N] = i;
    
    int current = calc_order_score(order);
    int best = current;
    vector<pair<int, pair<int, int>>> best_order = order;
    simulated_annealing sa;
    while (!sa.end()) {
        int type = random::get(10);
        if (type == 0) {
            int index1 = random::get(M);
            int index2 = random::get(N - M);
            int x = subset[index1];
            int y = out[index2];
            int pick = position[x + N];
            int post = position[-x + N];
            int next = current;
            next -= dist(order[pick - 1].second.first, order[pick - 1].second.second, order[pick].second.first, order[pick].second.second);
            next -= dist(order[pick].second.first, order[pick].second.second, order[pick + 1].second.first, order[pick + 1].second.second);
            if (pick + 1 != post) next -= dist(order[post - 1].second.first, order[post - 1].second.second, order[post].second.first, order[post].second.second);
            next -= dist(order[post].second.first, order[post].second.second, order[post + 1].second.first, order[post + 1].second.second);
            if (pick + 1 == post) {
                next += dist(order[pick - 1].second.first, order[pick - 1].second.second, order[post + 1].second.first, order[post + 1].second.second);
            } else {
                next += dist(order[pick - 1].second.first, order[pick - 1].second.second, order[pick + 1].second.first, order[pick + 1].second.second);
                next += dist(order[post - 1].second.first, order[post - 1].second.second, order[post + 1].second.first, order[post + 1].second.second);
            }
            pair<int, pair<int, int>> p1 = order[pick];
            pair<int, pair<int, int>> p2 = order[post];
            order.erase(order.begin() + post);
            order.erase(order.begin() + pick);
            static int left[M * 2];
            static int left_index[M * 2];
            static int right[M * 2];
            static int right_index[M * 2];
            for (int i = 1; i <= order.size() - 2; i++) {
                left[i] = 0;
                left[i] -= dist(order[i - 1].second.first, order[i - 1].second.second, order[i].second.first, order[i].second.second);
                left[i] += dist(order[i - 1].second.first, order[i - 1].second.second, a[y], b[y]);
                left[i] += dist(a[y], b[y], order[i].second.first, order[i].second.second);
                right[i] = 0;
                right[i] -= dist(order[i - 1].second.first, order[i - 1].second.second, order[i].second.first, order[i].second.second);
                right[i] += dist(order[i - 1].second.first, order[i - 1].second.second, c[y], d[y]);
                right[i] += dist(c[y], d[y], order[i].second.first, order[i].second.second);
            }
            left_index[1] = 1;
            for (int i = 2; i <= order.size() - 2; i++) {
                if (left[i] < left[i - 1]) {
                    left_index[i] = i;
                } else {
                    left[i] = left[i - 1];
                    left_index[i] = left_index[i - 1];
                }
            }
            right_index[order.size() - 2] = order.size() - 2;
            for (int i = order.size() - 3; i >= 1; i--) {
                if (right[i] < right[i + 1]) {
                    right_index[i] = i;
                } else {
                    right[i] = right[i + 1];
                    right_index[i] = right_index[i + 1];
                }
            }
            int best = 1e9, best_l, best_r;
            for (int i = 1; i <= order.size() - 4; i++) {
                if (left[i] + right[i + 2] < best) {
                    best = left[i] + right[i + 2];
                    best_l = left_index[i];
                    best_r = right_index[i + 2];
                }
            }
            next += best;
            if (sa.accept(current, next)) {
                current = next;
                swap(subset[index1], out[index2]);
                order.insert(order.begin() + best_r, make_pair(-y, make_pair(c[y], d[y])));
                order.insert(order.begin() + best_l, make_pair(y, make_pair(a[y], b[y])));
                for (int z = 1; z <= order.size() - 2; z++) position[order[z].first + N] = z;
            } else {
                order.insert(order.begin() + pick, p1);
                order.insert(order.begin() + post, p2);
            }
        } else if (type == 1) {
            int index1 = random::get(M);
            int index2 = random::get(N - M);
            int x = subset[index1];
            int y = out[index2];
            int pick = position[x + N];
            int post = position[-x + N];
            int next = current;
            next -= dist(order[pick - 1].second.first, order[pick - 1].second.second, order[pick].second.first, order[pick].second.second);
            next -= dist(order[pick].second.first, order[pick].second.second, order[pick + 1].second.first, order[pick + 1].second.second);
            if (pick + 1 != post) next -= dist(order[post - 1].second.first, order[post - 1].second.second, order[post].second.first, order[post].second.second);
            next -= dist(order[post].second.first, order[post].second.second, order[post + 1].second.first, order[post + 1].second.second);
            next += dist(order[pick - 1].second.first, order[pick - 1].second.second, a[y], b[y]);
            if (pick + 1 == post) {
                next += dist(a[y], b[y], c[y], d[y]);
            } else {
                next += dist(a[y], b[y], order[pick + 1].second.first, order[pick + 1].second.second);
                next += dist(order[post - 1].second.first, order[post - 1].second.second, c[y], d[y]);
            }
            next += dist(c[y], d[y], order[post + 1].second.first, order[post + 1].second.second);
            if (sa.accept(current, next)) {
                current = next;
                swap(subset[index1], out[index2]);
                order[pick] = make_pair(y, make_pair(a[y], b[y]));
                order[post] = make_pair(-y, make_pair(c[y], d[y]));
                position[y + N] = pick;
                position[-y + N] = post;
            }
        } else {
            int len = random::get(1, 30);
            int x = random::get(1, order.size() - 2 - len);
            int y = x + len;
            bool ok = true;
            for (int z = x; z < y; z++) {
                if (order[z].first > 0 && position[-order[z].first + N] <= y) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;
            int next = current;
            next -= dist(order[x - 1].second.first, order[x - 1].second.second, order[x].second.first, order[x].second.second);
            next -= dist(order[y].second.first, order[y].second.second, order[y + 1].second.first, order[y + 1].second.second);
            next += dist(order[x - 1].second.first, order[x - 1].second.second, order[y].second.first, order[y].second.second);
            next += dist(order[x].second.first, order[x].second.second, order[y + 1].second.first, order[y + 1].second.second);
            if (sa.accept(current, next)) {
                current = next;
                for (int l = x, r = y; l < r; l++, r--) swap(order[l], order[r]);
                for (int z = x; z <= y; z++) position[order[z].first + N] = z;
            }
        }
        if (current < best) {
            best = current;
            best_order = order;
        }
    }
    
    for (int x = 1; x <= best_order.size() - 2; x++) {
        int l = 1, r = best_order.size() - 2;
        if (best_order[x].first > 0) {
            r = position[-best_order[x].first + N] - 1;
        } else {
            l = position[-best_order[x].first + N] + 1;
        }
        int diff = 0;
        diff -= dist(best_order[x - 1].second.first, best_order[x - 1].second.second, best_order[x].second.first, best_order[x].second.second);
        diff -= dist(best_order[x].second.first, best_order[x].second.second, best_order[x + 1].second.first, best_order[x + 1].second.second);
        diff += dist(best_order[x - 1].second.first, best_order[x - 1].second.second, best_order[x + 1].second.first, best_order[x + 1].second.second);
        int best = 0, best_index = -1;
        for (int z = l; z <= r; z++) {
            if (z == x || z == x + 1) continue;
            int tmp = 0;
            tmp -= dist(best_order[z - 1].second.first, best_order[z - 1].second.second, best_order[z].second.first, best_order[z].second.second);
            tmp += dist(best_order[z - 1].second.first, best_order[z - 1].second.second, best_order[x].second.first, best_order[x].second.second);
            tmp += dist(best_order[x].second.first, best_order[x].second.second, best_order[z].second.first, best_order[z].second.second);
            if (diff + tmp < best) {
                best = diff + tmp;
                if (z < x) {
                    best_index = z;
                } else {
                    best_index = z - 1;
                }
            }
        }
        if (best < 0) {
            pair<int, pair<int, int>> p = best_order[x];
            best_order.erase(best_order.begin() + x);
            best_order.insert(best_order.begin() + best_index, p);
            for (int z = min(x, best_index); z <= max(x, best_index + 1); z++) position[best_order[z].first + N] = z;
        }
    }
    
    vector<int> best_subset;
    vector<pair<int, int>> route;
    for (const pair<int, pair<int, int>>& p : best_order) {
        if (p.first > 0) best_subset.push_back(p.first);
        pair<int, int> next = p.second;
        if (route.empty() || route.back() != next) route.push_back(next);
    }
    return make_pair(best_subset, route);
}

int calc_score(const vector<pair<int, int>>& route) {
    int t = 0;
    for (int i = 0; i + 1 < route.size(); i++) {
        t += dist(route[i].first, route[i].second, route[i + 1].first, route[i + 1].second);
    }
    return round(1e8 / (1000 + t));
}

int main() {
    input();
    
    pair<vector<int>, vector<pair<int, int>>> route = calc_route();
    
    printf("%d", M);
    for (int p : route.first) printf(" %d", p);
    printf("\n");
    
    printf("%d", route.second.size());
    for (const pair<int, int>& p : route.second) printf(" %d %d", p.first, p.second);
    printf("\n");
    
    fprintf(stderr, "%d\n", calc_score(route.second));
    
    return 0;
}
