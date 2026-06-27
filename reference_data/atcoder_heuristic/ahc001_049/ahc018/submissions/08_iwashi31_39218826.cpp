//
// ver 1.30
//

#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define rep(i, n) for (int i = 0; i < (n); i++)
#define REP(i, a, n) for (int i = (a); i <= (n); i++)
#define all(a) (a).begin(), (a).end()
#define mp(a, b) make_pair((a), (b))

using namespace std;

const int INF = 12345678;

const int RIGHT = 0;
const int UP = 1;
const int LEFT = 2;
const int DOWN = 3;
const int DX[8] = { 1, 0, -1, 0, 1, -1, -1, 1 };
const int DY[8] = { 0, -1, 0, 1, -1, -1, 1, 1 };

const int FIELD_SIZE = 200;
mt19937 mt_engine;

int field_size, source_num, house_num, base_cost;
int base_cost_pow;
ofstream logger;

vector<int> params;

unsigned int rand_xor() {
    static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    unsigned long t = (x ^ (x << 11u));
    x = y; y = z; z = w;
    return (w = (w ^ (w >> 19u)) ^ (t ^ (t >> 8u)));
}

class Timer {
private:
    static const int LAZY_MASK = 0xFF;

    chrono::system_clock::time_point start;
    int call_cnt = 0;
    int prev_time;
    bool is_lazy;

public:
    Timer(bool is_lazy = false) : is_lazy(is_lazy) {
        start = chrono::system_clock::now();
    }

    int get_ms() {
        if (!is_lazy || (call_cnt & LAZY_MASK) == 0) {
            auto end = chrono::system_clock::now();
            prev_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        }
        call_cnt++;
        return prev_time;
    }
};

class Point {
public:
    int x, y;

    Point() : x(-1), y(-1) {}
    Point(int x, int y) : x(x), y(y) {}

    void move(int direction, int distance = 1) {
        assert(direction >= 0 && direction < 8);
        x += DX[direction] * distance;
        y += DY[direction] * distance;
    }

    bool is_valid() {
        return x >= 0 && x < FIELD_SIZE && y >= 0 && y < FIELD_SIZE;
    }

    bool operator<(const Point& rhs) const {
        if (y != rhs.y) return y < rhs.y;
        return x < rhs.x;
    }

    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }

    friend ostream& operator<<(ostream& out, const Point& p) {
        out << '(' << p.x << ", " << p.y << ')';
        return out;
    }
};

class Source : public Point {
public:
};

class House : public Point {
public:
    bool is_completed = false;
};

vector<Source> orig_sources(source_num);
vector<House> orig_houses(house_num);

class Grid : public Point {
public:
    bool is_broken = false;
    bool has_water = false;
    House* house = nullptr;
    int accumulated_power = 0;
};

class Solver {
protected:
    enum class Result {
        NOT_BROKEN,
        BROKEN,
        FINISH,
        INVALID,
    };

    int total_cost = 0;
    vector<Source*> sources;
    vector<House*> houses;
    Grid grids[FIELD_SIZE][FIELD_SIZE];
    bool is_completed = false;

    void flow_water(const Point& pt) {
        grids[pt.y][pt.x].has_water = true;
        rep(dir, 4) {
            auto next = pt;
            next.move(dir);
            if (!next.is_valid()) continue;
            if (!grids[next.y][next.x].is_broken) continue;
            if (grids[next.y][next.x].has_water) continue;
            flow_water(next);
        }
    }

    bool connect(Point source, const Point& target, int power_step) {
        vector<int> dirs1, dirs2, dirs;
        rep(i, max(0, target.x - source.x)) dirs1.push_back(RIGHT);
        rep(i, max(0, source.x - target.x)) dirs1.push_back(LEFT);
        rep(i, max(0, target.y - source.y)) dirs2.push_back(DOWN);
        rep(i, max(0, source.y - target.y)) dirs2.push_back(UP);

        while (true) {
            bool moved = false;
            if (!dirs1.empty()) {
                Point next = source;
                next.move(dirs1.back());
                if (grids[next.y][next.x].is_broken) {
                    source = next;
                    dirs1.pop_back();
                    moved = true;
                }
            }
            if (!dirs2.empty()) {
                Point next = source;
                next.move(dirs2.back());
                if (grids[next.y][next.x].is_broken) {
                    source = next;
                    dirs2.pop_back();
                    moved = true;
                }
            }
            if (!moved) break;
        }

        if (dirs1.empty()) dirs = dirs2;
        else if (dirs2.empty()) dirs = dirs1;
        else {
            if (dirs1.size() > dirs2.size()) swap(dirs1, dirs2);
            double rate = (double) dirs2.size() / dirs1.size();
            while (!dirs1.empty() || !dirs2.empty()) {
                if (dirs1.empty() || (double) dirs2.size() / dirs1.size() >= rate) {
                    dirs.push_back(dirs2.back());
                    dirs2.pop_back();
                } else {
                    dirs.push_back(dirs1.back());
                    dirs1.pop_back();
                }
            }
        }

        Point now = source;
        int pp_power = grids[now.y][now.x].accumulated_power;
        int p_power = grids[now.y][now.x].accumulated_power;
        for (int dir: dirs) {
            now.move(dir);
            if (!grids[now.y][now.x].is_broken) {
                int next_power_sum = 0;
                int next_cnt = 0;
                rep(dir2, 4) {
                    auto next = now;
                    next.move(dir2);
                    if (!next.is_valid()) continue;
                    if (!grids[next.y][next.x].is_broken) continue;
                    next_power_sum += grids[next.y][next.x].accumulated_power;
                    next_cnt++;
                }
                int next_cost = next_power_sum / next_cnt;
                double attenuation_rate = 0.84;
                if (base_cost >= 64 && next_cost >= 500) attenuation_rate = 0.88;
                if (pp_power < p_power) attenuation_rate = 0.9;
                auto result = auto_excavate(now, power_step, next_cost * attenuation_rate);
                if (result == Result::FINISH) return true;
            }
            pp_power = p_power;
            p_power = grids[now.y][now.x].accumulated_power;
        }

        return false;
    }

    void connect_path(const vector<Point>& path, int power_step) {
        rep(i, path.size() - 1) {
            bool finished = connect(path[i], path[i + 1], power_step);
            if (finished) return;
        }
    }

    Result excavate(const Point& pt, int power) {
        if (power == 0) return Result::NOT_BROKEN;

        total_cost += base_cost + power;
        grids[pt.y][pt.x].accumulated_power += power;

        cout << pt.y << ' ' << pt.x << ' ' << power << endl;

        int r;
        cin >> r;
        if (r == -1) return Result::INVALID;
        if (r == 0) return Result::NOT_BROKEN;
        grids[pt.y][pt.x].is_broken = true;
        if (r == 1) {
            rep(dir, 4) {
                Point next = pt;
                next.move(dir);
                if (!next.is_valid()) continue;
                if (grids[next.y][next.x].has_water) {
                    flow_water(pt);
                    break;
                }
            }
            return Result::BROKEN;
        }
        if (r == 2) {
            is_completed = true;
            return Result::FINISH;
        }
        assert(false);
    }

    Result auto_excavate(Point& pt, int power_step, int initial_power = 0) {
        auto result = excavate(pt, initial_power);
        while (result == Result::NOT_BROKEN) {
            result = excavate(pt, power_step);
        }
        return result;
    }

    int calc_distance(const Point& p1, const Point& p2) const {
        return abs(p1.x - p2.x) + abs(p1.y - p2.y);
    }

    double calc_euclidean_distance(const Point& p1, const Point& p2) const {
        return hypot(abs(p1.x - p2.x), abs(p1.y - p2.y));
    }

public:
    Solver() {
        for (auto& source: orig_sources) sources.push_back(new Source(source));
        for (auto& house: orig_houses) {
            houses.push_back(new House(house));
            grids[house.y][house.x].house = houses.back();
        }
    }

    virtual void solve() = 0;
};

class ExploreSolver : public Solver {
protected:
    const int distance_step = 12;

    const int power_step_values[8] = { 6, 8, 10, 17, 25, 33, 58, 90 };
    const int source_power_step_values[8] = { 10, 12, 13, 22, 30, 33, 61, 94 };
    const int hard_power_step_values[8] = { 12, 26, 31, 38, 49, 82, 127, 210 };
    const int hard_threshold_rate[8] = {9, 8, 5, 4, 3, 2, 2, 1 };
    const int unbroken_penalty[8] = { 166, 167, 173, 169, 172, 168, 165, 235 };

    int power_step, hard_power_step;
    vector<House*> completed_houses, uncompleted_houses;
    vector<Point> checkpoints;
    vector<Point> next_checkpoints[FIELD_SIZE][FIELD_SIZE];
    vector<Point> next_next_checkpoints[FIELD_SIZE][FIELD_SIZE];

    int get_direction(const Point& p1, const Point& p2) {
        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        if (dx && dy) return -1;
        if (dx > 0) return RIGHT;
        if (dx < 0) return LEFT;
        if (dy > 0) return DOWN;
        if (dy < 0) return UP;
        return -1;
    }

    int calc_estimated_cost(const Point& pt) {
        if (grids[pt.y][pt.x].is_broken) return max(10, grids[pt.y][pt.x].accumulated_power - power_step/3);
        int value = grids[pt.y][pt.x].accumulated_power + (rand_xor() & 127);
        int extras = 0;
        int extra_cnt = 0;
        rep(dir, 4) {
            auto next = pt;
            next.move(dir, distance_step);
            if (!next.is_valid()) continue;
            extras += grids[next.y][next.x].accumulated_power;
            if (!grids[next.y][next.x].is_broken) extras += unbroken_penalty[base_cost_pow];
            extra_cnt++;
        }
        return value + extras / extra_cnt;
    }

    void generate_checkpoints() {
        vector<int> x_axis, y_axis;
        for (int i = 0;; i += distance_step) {
            if (i >= field_size) i = field_size-1;
            x_axis.push_back(i);
            y_axis.push_back(i);
            if (i == field_size-1) break;
        }
        for (auto y: y_axis) {
            for (auto x: x_axis) {
                checkpoints.push_back(Point(x, y));
            }
        }
        for (auto house: houses) {
            checkpoints.push_back(*house);
        }

        rep(y, field_size) {
            rep(x, field_size) {
                Point now(x, y);
                for (auto& cp: checkpoints) {
                    int d = calc_distance(now, cp);
                    if (d >= 1.75 * distance_step) continue;
                    next_checkpoints[y][x].push_back(cp);
                }
            }
        }

        for (auto& cp1: checkpoints) {
            for (auto& cp2: checkpoints) {
                if (cp1 == cp2) continue;
                int d = calc_distance(cp1, cp2);
                if (d <= 1.5 * distance_step || d >= 2.5 * distance_step) continue;
                next_next_checkpoints[cp1.y][cp1.x].push_back(cp2);
            }
        }
    }

    void excavate_sources() {
        for (auto& source: sources) {
            auto_excavate(*source, source_power_step_values[base_cost_pow]);
            grids[source->y][source->x].has_water = true;
        }
    }

    vector<Point> calc_best_path_from_source_to_house() {
        static long long min_cost[FIELD_SIZE][FIELD_SIZE];
        static Point prev_pt[FIELD_SIZE][FIELD_SIZE];
        rep(y, field_size) rep(x, field_size) min_cost[y][x] = INF;

        priority_queue<long long, vector<long long>, greater<>> q;
        rep(y, field_size) {
            rep(x, field_size) {
                if (!grids[y][x].has_water) continue;
                Point now(x, y);
                min_cost[y][x] = 0;
                for (auto& next: next_checkpoints[now.y][now.x]) {
                    int d = calc_distance(now, next);
                    long long next_dist = d * (calc_estimated_cost(now) + calc_estimated_cost(next) - 10) / 2;
                    if (next_dist < min_cost[next.y][next.x]) {
                        min_cost[next.y][next.x] = next_dist;
                        prev_pt[next.y][next.x] = now;
                        q.push((next_dist << 16) + (next.y << 8) + next.x);
                    }
                }
            }
        }

        Point* target = nullptr;
        while (!q.empty()) {
            auto info = q.top(); q.pop();
            Point now(info & 255, (info >> 8) & 255);
            long long now_dist = min_cost[now.y][now.x];
            if ((info >> 16) != now_dist) continue;
            if (grids[now.y][now.x].house != nullptr && !grids[now.y][now.x].has_water) {
                target = grids[now.y][now.x].house;
                break;
            }
            for (auto& next: next_checkpoints[now.y][now.x]) {
                int d = calc_distance(now, next);
                long long next_dist = now_dist + d * (calc_estimated_cost(now) + calc_estimated_cost(next) - 10) / 2;
                if (next_dist < min_cost[next.y][next.x]) {
                    min_cost[next.y][next.x] = next_dist;
                    prev_pt[next.y][next.x] = now;
                    q.push((next_dist << 16) + (next.y << 8) + next.x);
                }
            }
        }
        assert(target);

        vector<Point> path;
        Point now = *target;
        path.push_back(now);
        while (!grids[now.y][now.x].has_water) {
            now = prev_pt[now.y][now.x];
            path.push_back(now);
            assert(now.is_valid());
        }
        reverse(all(path));

        return path;
    }

    void prune_path(vector<Point>& path) {
        vector<int> pruned_ids;
        REP(i, 1, path.size()-2) {
            int dir1 = get_direction(path[i-1], path[i]);
            int dir2 = get_direction(path[i], path[i+1]);
            if (dir1 == -1 || dir2 == -1 || dir1 == dir2) continue;
            int cost1 = grids[path[i-1].y][path[i-1].x].accumulated_power;
            int cost2 = grids[path[i].y][path[i].x].accumulated_power;
            int cost3 = grids[path[i+1].y][path[i+1].x].accumulated_power;
            if (cost1 < 300 && cost2 >= 400 && cost3 < 300) {
                pruned_ids.push_back(i);
                i++;
            }
        }
        while (!pruned_ids.empty()) {
            path.erase(path.begin() + pruned_ids.back());
            pruned_ids.pop_back();
        }
    }

    void update_house_vectors() {
        for (auto it = uncompleted_houses.begin(); it != uncompleted_houses.end();) {
            auto house = *it;
            if (grids[house->y][house->x].has_water) {
                house->is_completed = true;
                completed_houses.push_back(house);
                it = uncompleted_houses.erase(it);
            } else {
                it++;
            }
        }
    }

public:
    ExploreSolver() : Solver() {
        uncompleted_houses = houses;
        power_step = power_step_values[base_cost_pow];
        hard_power_step = hard_power_step_values[base_cost_pow];

        generate_checkpoints();
    }

    void solve() override {
        excavate_sources();

        while (!is_completed) {
            auto path = calc_best_path_from_source_to_house();

            bool excavated = false;
            for (int i = 0; i < path.size(); i += 2) {
                auto& pt = path[i];
                if (!grids[pt.y][pt.x].is_broken) {
                    if (grids[pt.y][pt.x].accumulated_power >= hard_threshold_rate[base_cost_pow] * power_step) break;
                    auto result = excavate(pt, power_step);
                    if (result == Solver::Result::FINISH) return;
                    excavated = true;
                    break;
                }
            }

            if (!excavated) {
                for (int i = path.size()-2; i > 0; i -= 3) {
                    auto& pt = path[i];
                    if (!grids[pt.y][pt.x].is_broken) {
                        if (grids[pt.y][pt.x].accumulated_power <= 20) {
                            auto result = excavate(pt, power_step);
                            if (result == Solver::Result::FINISH) return;
                        } else {
                            auto result = excavate(pt, hard_power_step);
                            if (result == Solver::Result::FINISH) return;
                            if (base_cost <= 32 && !grids[pt.y][pt.x].is_broken) {
                                for (auto& next: next_next_checkpoints[pt.y][pt.x]) {
                                    if (pt.x == next.x || pt.y == next.y) continue;
                                    if (grids[next.y][next.x].is_broken) continue;
                                    if (grids[next.y][next.x].accumulated_power > 30) continue;
                                    auto result = excavate(next, 1.8 * power_step);
                                    if (result == Solver::Result::FINISH) return;
                                }
                            }
                        }
                        excavated = true;
                        break;
                    }
                }
            }

            if (!excavated) {
                for (int i = path.size()-2; i > 0; i--) {
                    auto& pt = path[i];
                    if (!grids[pt.y][pt.x].is_broken) {
                        if (grids[pt.y][pt.x].accumulated_power <= 20) {
                            auto result = excavate(pt, power_step);
                            if (result == Solver::Result::FINISH) return;
                        } else {
                            auto result = excavate(pt, hard_power_step);
                            if (result == Solver::Result::FINISH) return;
                            if (base_cost <= 32 && !grids[pt.y][pt.x].is_broken) {
                                for (auto& next: next_next_checkpoints[pt.y][pt.x]) {
                                    if (pt.x == next.x || pt.y == next.y) continue;
                                    if (grids[next.y][next.x].is_broken) continue;
                                    if (grids[next.y][next.x].accumulated_power > 30) continue;
                                    auto result = excavate(next, 1.8 * power_step);
                                    if (result == Solver::Result::FINISH) return;
                                }
                            }
                        }
                        excavated = true;
                        break;
                    }
                }
            }

            if (!excavated) {
                prune_path(path);
                connect_path(path, power_step);
            }

            update_house_vectors();
        }
    }
};

int main(int argc, char* argv[]) {
    logger = ofstream(argc >= 2 ? argv[1] : "log.txt");
    REP(i, 2, argc-1) {
        params.push_back(stoi(argv[i]));
    }

    cin >> field_size >> source_num >> house_num >> base_cost;
    orig_sources.resize(source_num);
    orig_houses.resize(house_num);
    rep(i, source_num) cin >> orig_sources[i].y >> orig_sources[i].x;
    rep(i, house_num) cin >> orig_houses[i].y >> orig_houses[i].x;

    int pw = 0;
    int bc = 1;
    while (bc < base_cost) {
        pw++;
        bc *= 2;
    }
    base_cost_pow = pw;

    Solver* solver = new ExploreSolver();
    solver->solve();
}
