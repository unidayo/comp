#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <vector>
#define FOR(i, n, m) for (ll i = n; i < (int)m; i++)
#define REP(i, n) FOR(i, 0, n)
#define ALL(v) v.begin(), v.end()
#define pb push_back
using namespace std;
using ll = long long;
using P = pair<int, int>;
constexpr ll inf = 1000000000;

constexpr bool SELF_CONTAINED = false;

map<int, int> POWER = {
    {1, 1}, {2, 2}, {4, 4}, {8, 5}, {16, 8}, {32, 16}, {64, 28}, {128, 47},
};

map<int, int> POWER_EXPR = {
    {1, 1}, {2, 2}, {4, 4}, {8, 11}, {16, 18}, {32, 32}, {64, 44}, {128, 45},
};

map<int, double> DECR_RATIO = {
    {1, 1.325}, {2, 1.3},   {4, 1.26},  {8, 1.27},
    {16, 1.26}, {32, 1.25}, {64, 1.23}, {128, 1.15},
};

map<int, double> COEF_RATIO = {
    {1, 0.85}, {2, 1.0},   {4, 1.0},  {8, 1.0},
    {16, 1.0}, {32, 1.0}, {64, 1.25}, {128, 1.5}
};

struct UnionFind {
    vector<int> data;

    UnionFind(int sz) { data.assign(sz, -1); }

    bool unite(int x, int y) {
        x = find(x), y = find(y);
        if (x == y) return (false);
        if (data[x] > data[y]) swap(x, y);
        data[x] += data[y];
        data[y] = x;
        return (true);
    }

    int find(int k) {
        if (data[k] < 0) return (k);
        return (data[k] = find(data[k]));
    }

    bool same(int x, int y) { return find(x) == find(y); }

    int size(int k) { return (-data[find(k)]); }
};

bool between(int i, int l, int r) { return (l <= i and i <= r); }

// const
constexpr int MAX_N = 200, MAX_W = 4, MAX_K = 10, MIN_POWER = 1,
              MAX_POWER = 5000;
int N, W, K, C;
array<P, MAX_W> waters;
array<P, MAX_K> houses;
array<array<int, MAX_N>, MAX_N> X;

// state
array<array<bool, MAX_N>, MAX_N> broken;
array<array<int, MAX_N>, MAX_N> damage;
array<array<int, MAX_N>, MAX_N> estimation;
int additional = 0;
constexpr int MAX_ADDITIONAL = 1;

struct TotalPower {
    int total_power = 0;
    vector<pair<string, int>> check_points;

    void add(int power) { total_power += power; }

    void check(string name) { check_points.pb({name, total_power}); }

    void show() {
        check("total");
        REP(i, check_points.size()) {
            cerr << check_points[i].first << ": ";
            if (i) {
                cerr << check_points[i].second - check_points[i - 1].second
                     << ", ";
            }
            cerr << check_points[i].second;
            cerr << endl;
        }
    }
};

TotalPower total_power;

bool excavate(P p, int power) {
    assert(not broken[p.first][p.second]);
    assert(between(p.first, 0, N - 1) and between(p.second, 0, N - 1));
    assert(between(power, MIN_POWER, MAX_POWER));

    total_power.add(C + power);
    damage[p.first][p.second] += power;

    cout << p.first << " " << p.second << " " << power << endl;

    int verdict;

    if (SELF_CONTAINED) {
        verdict = (damage[p.first][p.second] >= X[p.first][p.second]);
    } else {
        cin >> verdict;
    }

    if (verdict == 0) {
        return false;
    } else if (verdict == 1) {
        broken[p.first][p.second] = true;
        return true;
    } else if (verdict == 2) {
        exit(0);
    } else {
        assert(false);
    }
}

void excavate_exponentially(P p, int initial_power) {
    assert(between(initial_power, MIN_POWER, MAX_POWER));

    int power = initial_power;

    while (not broken[p.first][p.second]) {
        excavate(p, power);
        power = min(power + max(C / 2, 1), MAX_POWER);
    }
}

P transfer_point(P p, P start, P goal) {
    assert(p != goal);

    if (p.first < goal.first) {
        return {p.first + 1, p.second};
    } else if (p.first > goal.first) {
        return {p.first - 1, p.second};
    } else if (p.second < goal.second) {
        return {p.first, p.second + 1};
    } else if (p.second > goal.second) {
        return {p.first, p.second - 1};
    } else {
        assert(false);
    }
}

void connect_two_points(P start, P goal,
                        const vector<vector<bool>>& necessary_cells) {
    P p = start, prev_p = start;

    while (true) {
        if (not necessary_cells[p.first][p.second]) {
            if (p == goal) break;
            p = transfer_point(p, start, goal);
            prev_p = p;
            continue;
        }
        if (not broken[p.first][p.second]) {
            excavate(p, max(estimation[p.first][p.second] -
                                damage[p.first][p.second],
                            1));
        }
        if (broken[p.first][p.second]) {
            damage[p.first][p.second] =
                max(1, int(damage[p.first][p.second] / DECR_RATIO[C]));
        }
        excavate_exponentially(p, POWER[C]);
        if (p == goal) break;
        p = transfer_point(p, start, goal);
        estimation[p.first][p.second] =
            int(0.15 * estimation[p.first][p.second] +
                0.85 * damage[prev_p.first][prev_p.second]);
        prev_p = p;
    }
}

int manhattan_distance(P p, P q) {
    return abs(p.first - q.first) + abs(p.second - q.second);
}

// ad-hoc solver
constexpr int BOX_WIDTH = 10;
constexpr int BOX_SIDE = (MAX_N - 1) / BOX_WIDTH + 1;
constexpr int BOX_NUM = BOX_SIDE * BOX_SIDE;

struct Box {
    int index;
    P repr;
    int r;
    int c;

    Box() {
        index = r = c = -1;
        repr = {-1, -1};
    }

    Box(int index, P repr) : index(index), repr(repr) {
        r = index / BOX_SIDE;
        c = index % BOX_SIDE;
    }

    bool is_open() { return broken[repr.first][repr.second]; }
};

struct Boxes {
    array<Box, BOX_NUM> boxes;

    Boxes() {
        REP(i, BOX_SIDE) REP(j, BOX_SIDE) {
            P repr = {-1, -1};
            REP(k, K) {
                P house = houses[k];
                if (house.first / BOX_WIDTH == i and
                    house.second / BOX_WIDTH == j) {
                    repr = house;
                    break;
                }
            }
            REP(k, W) {
                P water = waters[k];
                if (repr.first == -1 and water.first / BOX_WIDTH == i and
                    water.second / BOX_WIDTH == j) {
                    repr = water;
                    break;
                }
            }

            if (repr.first == -1) {
                repr = {min(BOX_WIDTH * i + BOX_WIDTH / 2, N - 1LL),
                        min(BOX_WIDTH * j + BOX_WIDTH / 2, N - 1LL)};
                if (i == 0) repr.first = 0;
                if (i == BOX_SIDE - 1) repr.first = N - 1;
                if (j == 0) repr.second = 0;
                if (j == BOX_SIDE - 1) repr.second = N - 1;
                        }
            int index = i * BOX_SIDE + j;
            boxes[index] = Box(index, repr);
        }
    }

    Box& operator[](int i) { return boxes[i]; }

    Box correspond_to(P p) {
        return boxes[(p.first / BOX_WIDTH) * BOX_SIDE + (p.second / BOX_WIDTH)];
    }

    Box at(int r, int c) {
        assert(between(r, 0, BOX_SIDE - 1) and between(c, 0, BOX_SIDE - 1));
        return boxes[r * BOX_SIDE + c];
    }

    vector<Box> enumerate_adjacent_boxes(Box box) {
        vector<Box> a_boxes;
        if (box.r > 0) a_boxes.pb(at(box.r - 1, box.c));
        if (box.r < BOX_SIDE - 1) a_boxes.pb(at(box.r + 1, box.c));
        if (box.c > 0) a_boxes.pb(at(box.r, box.c - 1));
        if (box.c < BOX_SIDE - 1) a_boxes.pb(at(box.r, box.c + 1));
        return a_boxes;
    }

    int size() { return BOX_NUM; }
};

array<array<int, BOX_NUM + 1>, (1 << (MAX_K + 1))> dp;

vector<P> dfs_construction(int bit, int i, const vector<vector<P>>& graph) {
    if (dp[bit][i] == 0) return {};
    assert(dp[bit][i] < inf);

    for (int sub_bit = bit; sub_bit > 0; sub_bit = ((sub_bit - 1) & bit)) {
        if (sub_bit == bit) continue;
        if (dp[sub_bit][i] + dp[bit ^ sub_bit][i] == dp[bit][i]) {
            auto edges_1 = dfs_construction(sub_bit, i, graph);
            auto edges_2 = dfs_construction(bit ^ sub_bit, i, graph);
            edges_1.insert(edges_1.end(), ALL(edges_2));
            return edges_1;
        }
    }

    for (auto [a_i, w] : graph[i]) {
        if (dp[bit][a_i] + w == dp[bit][i]) {
            auto edges = dfs_construction(bit, a_i, graph);
            edges.pb({min(i, a_i), max(i, a_i)});
            return edges;
        }
    }

    assert(false);
    return {};
}

vector<P> minimum_steiner_tree(const vector<vector<P>>& graph,
                               const vector<int>& terms) {
    int n = graph.size(), m = terms.size();

    REP(i, (1 << m)) REP(j, n) dp[i][j] = inf;

    REP(i, n) dp[0][i] = 0;
    REP(i, m) dp[(1 << i)][terms[i]] = 0;

    FOR(bit, 1, (1 << m)) {
        REP(i, n) {
            for (int sub_bit = bit; sub_bit > 0;
                 sub_bit = ((sub_bit - 1) & bit)) {
                if (sub_bit == bit) continue;
                dp[bit][i] =
                    min(dp[bit][i], dp[sub_bit][i] + dp[bit ^ sub_bit][i]);
            }
        }

        priority_queue<P, vector<P>, greater<P>> pq;

        REP(i, n) { pq.push({dp[bit][i], i}); }

        while (not pq.empty()) {
            auto [value, i] = pq.top();
            pq.pop();
            if (value != dp[bit][i]) continue;
            for (auto [a_i, w] : graph[i]) {
                if (dp[bit][a_i] > dp[bit][i] + w) {
                    dp[bit][a_i] = dp[bit][i] + w;
                    pq.push({dp[bit][a_i], a_i});
                }
            }
        }
    }

    auto edges = dfs_construction((1 << m) - 1, terms[0], graph);
    return edges;
}

bool box_is_housed(Box box, Boxes& boxes, UnionFind& uf) {
    REP(i, W) {
        Box house_box = boxes.correspond_to(houses[i]);
        if (uf.same(box.index, house_box.index)) return true;
    }

    return false;
}

bool box_is_watered(Box box, Boxes& boxes, UnionFind& uf) {
    REP(i, W) {
        Box water_box = boxes.correspond_to(waters[i]);
        if (uf.same(box.index, water_box.index)) return true;
    }

    return false;
}

bool all_houses_are_watered(Boxes& boxes, UnionFind& uf) {
    REP(i, K) {
        Box house_box = boxes.correspond_to(houses[i]);
        if (not box_is_watered(house_box, boxes, uf)) return false;
    }
    return true;
}

bool need_to_open(Box box, Boxes& boxes, UnionFind& uf) {
    vector<Box> a_boxes = boxes.enumerate_adjacent_boxes(box);

    REP(i, K) {
        if (box.index == boxes.correspond_to(houses[i]).index) {
            return true;
        }
    }

    for (Box a_box : a_boxes) {
        if (a_box.is_open() and not box_is_watered(a_box, boxes, uf)) {
            return true;
        }
    }

    return false;
}

int box_priority(Box box, Boxes& boxes, UnionFind& uf) {
    int min_dist = inf;

    REP(i, W) {
        min_dist = min(
            min_dist,
            manhattan_distance(box.repr, boxes.correspond_to(waters[i]).repr));
    }

    REP(i, boxes.size()) {
        Box box2 = boxes[i];
        if (box2.is_open() and box_is_watered(box2, boxes, uf)) {
            min_dist = min(min_dist, manhattan_distance(box.repr, box2.repr));
        }
    }

    float coef =
        11.5 + 0.95 * log2(max(damage[box.repr.first][box.repr.second], 2));
    coef *= COEF_RATIO[C];
    if (damage[box.repr.first][box.repr.second] <= 25) {
        return 0.825 *
               (damage[box.repr.first][box.repr.second] + coef * min_dist);
    }
    return damage[box.repr.first][box.repr.second] + coef * min_dist;
}

int next_explore_power(int power) {
    if (power == 0) return POWER_EXPR[C];
    // 50: 993.9273080929764
    return min(power + min(C, 50), MAX_POWER);
}

vector<P> enumerate_adjacents(P p) {
    vector<P> adjacents;
    int di[] = {-1, 1, 0, 0}, dj[] = {0, 0, -1, 1};
    REP(k, 4) {
        int ai = p.first + di[k], aj = p.second + dj[k];
        if (not between(ai, 0, N - 1) or not between(aj, 0, N - 1)) continue;
        adjacents.pb(P(ai, aj));
    }
    return adjacents;
}

int num_used_adjacents(P p, vector<vector<bool>>& used) {
    auto adjacents = enumerate_adjacents(p);
    int num = 0;
    for (auto a : adjacents) {
        if (used[a.first][a.second]) num++;
    }
    return num;
}

bool is_house(P p) {
    REP(i, K) {
        if (houses[i] == p) return true;
    }
    return false;
}

bool is_water(P p) {
    REP(i, W) {
        if (waters[i] == p) return true;
    }
    return false;
}

vector<vector<bool>> enumerate_necessary_cells(Boxes& boxes,
                                               const vector<P>& edges) {
    vector<vector<bool>> used(N, vector<bool>(N, false)),
        seen(N, vector<bool>(N, false));

    for (auto edge : edges) {
        if (edge.second == boxes.size()) continue;
        P start = boxes[edge.first].repr, goal = boxes[edge.second].repr;
        P p = start;
        while (true) {
            used[p.first][p.second] = true;
            if (p == goal) break;
            p = transfer_point(p, start, goal);
        }
    }
    queue<P> q;

    REP(i, N) REP(j, N) {
        if (num_used_adjacents(P(i, j), used) <= 1 and not is_house(P(i, j)) and
            not is_water(P(i, j))) {
            q.push(P(i, j));
            used[i][j] = false;
        }
    }

    while (not q.empty()) {
        P p = q.front();
        q.pop();
        auto adjacents = enumerate_adjacents(p);
        for (auto a : adjacents) {
            if (used[a.first][a.second] and num_used_adjacents(a, used) <= 1 and
                not is_house(a) and not is_water(a)) {
                used[a.first][a.second] = false;
                q.push(a);
            }
        }
    }

    return used;
}
void solve() {
    Boxes boxes;

    UnionFind uf(boxes.size() + 1);

    // queue<int> water_frontiers;
    // REP(i, K) { water_frontiers.push(boxes.correspond_to(waters[i]).index); }
    // while (not water_frontiers.empty()) {
    //     auto index = water_frontiers.front();
    //     water_frontiers.pop();
    //     Box box = boxes[index];
    //     if (box.is_open()) continue;
    //     excavate(box.repr, 50);
    //     if (box.is_open()) {
    //         vector<Box> a_boxes = boxes.enumerate_adjacent_boxes(box);
    //         for (Box a_box : a_boxes) {
    //             if (a_box.is_open()) {
    //                 uf.unite(box.index, a_box.index);
    //                 continue;
    //             }
    //             water_frontiers.push(a_box.index);
    //         }
    //     }
    // }

    priority_queue<P, vector<P>, greater<P>> frontiers;
    REP(i, K) {
        frontiers.push({box_priority(boxes.correspond_to(houses[i]), boxes, uf),
                        boxes.correspond_to(houses[i]).index});
    }
    map<int, int> box_to_explore_power;

    while (not frontiers.empty()) {
        auto [priority, index] = frontiers.top();
        Box box = boxes[index];
        frontiers.pop();

        if (box.is_open()) continue;
        if (not need_to_open(box, boxes, uf)) continue;

        int explore_power = next_explore_power(box_to_explore_power[box.index]);
        excavate(box.repr, explore_power);
        box_to_explore_power[box.index] = explore_power;

        if (not box.is_open()) {
            frontiers.push({box_priority(box, boxes, uf), box.index});
        } else {
            vector<Box> a_boxes = boxes.enumerate_adjacent_boxes(box);
            for (Box a_box : a_boxes) {
                if (a_box.is_open()) {
                    uf.unite(box.index, a_box.index);
                    continue;
                }

                frontiers.push({box_priority(a_box, boxes, uf), a_box.index});
            }
        }

        priority_queue<P, vector<P>, greater<P>> new_frontiers;

        while (not frontiers.empty()) {
            auto [_, i] = frontiers.top();
            frontiers.pop();
            if (boxes[i].is_open()) continue;
            if (not need_to_open(boxes[i], boxes, uf)) continue;
            new_frontiers.push(
                {box_priority(boxes[i], boxes, uf), boxes[i].index});
        }

        frontiers = new_frontiers;

        if (all_houses_are_watered(boxes, uf)) {
            additional++;
            if (additional == MAX_ADDITIONAL) break;
        }
    }

    REP(i, N) REP(j, N) {
        Box box = boxes.correspond_to({i, j});
        estimation[i][j] = damage[box.repr.first][box.repr.second];
    }

    total_power.check("explore");

    vector<vector<P>> graph(boxes.size() + 1);

    REP(i, boxes.size()) {
        Box box = boxes[i];
        if (not box.is_open()) continue;

        auto a_boxes = boxes.enumerate_adjacent_boxes(box);
        for (Box a_box : a_boxes) {
            if (not a_box.is_open()) continue;
            auto weight = damage[box.repr.first][box.repr.second] +
                          damage[a_box.repr.first][a_box.repr.second];
            graph[box.index].pb({a_box.index, weight});
        }
    }
    REP(i, W) {
        Box water_box = boxes.correspond_to(waters[i]);
        graph[water_box.index].pb({boxes.size(), 1});
        graph[boxes.size()].pb({water_box.index, 1});
    }

    vector<int> terms;
    REP(i, K) terms.pb(boxes.correspond_to(houses[i]).index);
    terms.pb(boxes.size());

    auto edges = minimum_steiner_tree(graph, terms);

    auto necessary_cells = enumerate_necessary_cells(boxes, edges);

    for (auto edge : edges) {
        if (edge.second == boxes.size()) continue;
        connect_two_points(boxes[edge.first].repr, boxes[edge.second].repr,
                           necessary_cells);
    }

    total_power.check("connection");
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    cin >> N >> W >> K >> C;

    if (SELF_CONTAINED) {
        REP(i, N) REP(j, N) cin >> X[i][j];
    }

    REP(i, W) cin >> waters[i].first >> waters[i].second;
    REP(i, K) cin >> houses[i].first >> houses[i].second;

    if(W == 1) {
        vector<P> candidates(K + 1);
        REP(i, K) candidates[i] = houses[i];
        candidates[K] = waters[0];
        int min_dist = inf, arg_min = -1;
        REP(i, K + 1) {
            int dist = manhattan_distance(candidates[i], P(N / 2, N / 2));
            if(dist < min_dist) {
                min_dist = dist;
                arg_min = i;
            }
        }
        waters[0] = candidates[arg_min];
        REP(i, K + 1) {
            if(i == arg_min) continue;
            if(i < arg_min) {
                houses[i] = candidates[i];
             } else {
                houses[i - 1] = candidates[i];
             }
        }
    }

    REP(i, N) REP(j, N) {
        broken[i][j] = false;
        damage[i][j] = 0;
        estimation[i][j] = -1;
    }

    solve();

    if (SELF_CONTAINED) {
        total_power.show();
    }
}