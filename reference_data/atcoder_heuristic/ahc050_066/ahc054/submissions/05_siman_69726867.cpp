#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <vector>

using namespace std;
typedef long long ll;

double TIME_LIMIT = 1.9;

constexpr int UNDEFINED = -1;
constexpr int WALL = -1;
constexpr int TREE = 0;
constexpr int EMPTY = 1;
constexpr int MAX_N = 40;
constexpr int GRID_SIZE = MAX_N + 2;
constexpr int DZ[8] = {-GRID_SIZE, 1,
                       GRID_SIZE, -1,
                       -GRID_SIZE - 1, -GRID_SIZE + 1,
                       GRID_SIZE + 1, GRID_SIZE - 1};
constexpr int DY[8] = {-1, 1, 0, 0, -1, 1, 1, -1};
constexpr int DX[8] = {0, 0, -1, 1, 1, 1, -1, -1};
int BITS_SIZE = 5;

constexpr int LEFT = 2;
constexpr int RIGHT = 3;

ll g_visited[GRID_SIZE * GRID_SIZE];
ll g_updated[GRID_SIZE * GRID_SIZE];
ll g_checked[GRID_SIZE * GRID_SIZE][5];
ll g_vid;
ll g_uid;
ll g_cid;
ll g_noise;
int g_all_gid_count;
bool g_local_mode = false;
vector<int> g_tree_mask_list;

void reset_global_vars() {
    memset(g_updated, -1, sizeof(g_updated));
    memset(g_visited, -1, sizeof(g_visited));
    memset(g_checked, -1, sizeof(g_checked));
    g_uid = 0;
    g_vid = 0;
    g_cid = 0;
    g_noise = 0;
}

int calc_z(int y, int x) {
    return y * GRID_SIZE + x;
}

int calc_chebyshev_distance(int z1, int z2) {
    int y1 = z1 / GRID_SIZE;
    int x1 = z1 % GRID_SIZE;
    int y2 = z2 / GRID_SIZE;
    int x2 = z2 % GRID_SIZE;
    return max(abs(y1 - y2), abs(x1 - x2));
}

int calc_dist(int z1, int z2) {
    int y1 = z1 / GRID_SIZE;
    int x1 = z1 % GRID_SIZE;
    int y2 = z2 / GRID_SIZE;
    int x2 = z2 % GRID_SIZE;
    return abs(y1 - y2) + abs(x1 - x2);
}

class Xorshift {
   public:
    Xorshift(uint64_t seed = 88675123) {
        _x = 123456789;
        _y = 362436069;
        _z = 521288629;
        _w = seed;

        for (int i = 0; i < 100; ++i) {
            next();
        }
    }

    uint64_t next(uint64_t a, uint64_t b) {
        return a + (next() % (b - a + 1));
    }

    uint64_t next() {
        uint64_t t = _x ^ (_x << 11);
        _x = _y;
        _y = _z;
        _z = _w;
        _w = (_w ^ (_w >> 19)) ^ (t ^ (t >> 8));
        return _w;
    }

    double random_double() {
        return static_cast<double>(next()) / UINT64_MAX;
    }

    double random_double(double a, double b) {
        return a + (b - a) * random_double();
    }

   private:
    uint64_t _x, _y, _z, _w;
};

Xorshift g_rng;

class Timer {
   public:
    Timer() {
        begin();
    }

    inline void begin() {
        _start_at = chrono::steady_clock::now();
    }

    inline double get_time() {
        auto end_at = chrono::steady_clock::now();
        return chrono::duration_cast<chrono::nanoseconds>(end_at - _start_at)
                   .count() /
               1e9;
    }

    inline double get_time_ms() {
        auto end_at = chrono::steady_clock::now();
        return chrono::duration_cast<chrono::microseconds>(end_at - _start_at)
                   .count() /
               1e3;
    }

    inline double progress(double time_limit) {
        return get_time() / time_limit;
    }

   private:
    chrono::steady_clock::time_point _start_at;
};

Timer g_timer;

struct TurnInfo {
    int py, px;
    vector<int> new_revealed;
};

struct Area {
    int area_count1;
    int area_count2;
    int path_length;
    vector<int> all_walls;
    vector<int> walls;
    vector<int> extra_path_right;
    vector<int> extra_path_left;
    vector<int> extra_walls_right;
    vector<int> extra_walls_left;
    int area_ids[GRID_SIZE * GRID_SIZE];

    Area() {
        area_count1 = 0;
        area_count2 = 0;
        path_length = 0;
        all_walls.clear();
        walls.clear();
        memset(area_ids, -1, sizeof(area_ids));
    }

    int eval() {
        int value = 0;
        value += area_count1;
        value += area_count2;
        value += 2 * path_length;
        value += 2 * extra_path_right.size();
        value += 2 * extra_path_left.size();
        value -= abs(area_count1 - area_count2);

        return value;
    }
};

int N,
    Ty, Tx, Tz;
int Sz;
int g_grid[GRID_SIZE * GRID_SIZE];
int g_grid_origin[GRID_SIZE * GRID_SIZE];
vector<int> g_p_list;

bool is_inside(int z) {
    int y = z / GRID_SIZE;
    int x = z % GRID_SIZE;
    return 1 <= y && y <= N && 1 <= x && x <= N;
}

bool is_outside(int z) {
    int y = z / GRID_SIZE;
    int x = z % GRID_SIZE;
    return y < 0 || N + 1 < y || x < 0 || N + 1 < x;
}

struct SpiralPart {
    vector<int> dz_list;
    vector<int> template_dy;
    vector<int> template_dx;
};

vector<SpiralPart> g_spiral_part_list;

struct State {
    int turn;
    int py;
    int px;
    int max_py;
    int grid[GRID_SIZE * GRID_SIZE];
    bool revealed[GRID_SIZE * GRID_SIZE];
    bool wall_placed[GRID_SIZE * GRID_SIZE];
    bool has_spiral_trees;
    bool use_big_wall;
    bool success_build_wall;
    bool finished_build_walls;
    vector<int> extra_walls_right;
    vector<int> extra_walls_left;

    void update(const TurnInfo &info) {
        py = info.py;
        px = info.px;
        for (int z : info.new_revealed) {
            revealed[z] = true;
        }
    }

    vector<int> step() {
        vector<int> to_place;
        max_py = max(max_py, py);
        bool build_flag = max_py > N / 2;

        if (turn == 0) {
            int max_value = INT_MIN;
            Area best_area;
            vector<int> best_to_place;
            bool success_build_wall_status = false;
            bool has_spiral_trees_status = false;

            for (int i = 0; i < 20; ++i) {
                to_place.clear();
                memcpy(grid, g_grid_origin, sizeof(g_grid_origin));
                has_spiral_trees = false;
                vector<int> spiral_trees = generate_spiral_trees(Tz, 1);

                for (int ds = 2; ds <= 8 && spiral_trees.empty(); ++ds) {
                    spiral_trees = generate_spiral_trees(Tz, ds);
                }

                if (spiral_trees.empty()) {
                    vector<int> spiral_trees_failed;
                    for (int failed_limit = 1; failed_limit <= 4 && spiral_trees_failed.empty(); ++failed_limit) {
                        spiral_trees_failed = generate_spiral_trees(Tz, 1, failed_limit);
                        for (int ds = 2; ds <= 8 && spiral_trees_failed.empty(); ++ds) {
                            spiral_trees_failed = generate_spiral_trees(Tz, ds, failed_limit);
                        }
                    }
                    if (spiral_trees_failed.size() > 0) {
                        spiral_trees = spiral_trees_failed;
                    }
                }
                has_spiral_trees = (spiral_trees.size() > 0);

                for (int z : spiral_trees) {
                    if (grid[z] == EMPTY && not revealed[z]) {
                        to_place.push_back(z);
                        grid[z] = TREE;
                    }
                }

                if (use_big_wall) {
                    Area area = make_divided_areas();
                    bool can_place_walls = can_place_multi_tree(area.all_walls);
                    if (can_place_walls) {
                        success_build_wall = true;
                        for (int z : area.all_walls) {
                            if (grid[z] == EMPTY && not revealed[z]) {
                                to_place.push_back(z);
                                grid[z] = TREE;
                            }
                        }
                        int value = area.eval();
                        if (max_value < value) {
                            best_to_place = to_place;
                            success_build_wall_status = success_build_wall;
                            has_spiral_trees_status = has_spiral_trees;
                            best_area = area;
                            max_value = value;
                        }
                        break;
                    }
                }
                g_noise += 10;
            }

            memcpy(grid, g_grid_origin, sizeof(g_grid_origin));
            to_place = best_to_place;
            extra_walls_right = best_area.extra_walls_right;
            extra_walls_left = best_area.extra_walls_left;
            for (int z : to_place) {
                grid[z] = TREE;
            }
            success_build_wall = success_build_wall_status;
            has_spiral_trees = has_spiral_trees_status;
        }

        if (use_big_wall && success_build_wall && build_flag && not finished_build_walls) {
            finished_build_walls = true;
            if (px < N / 2) {
                for (int z : extra_walls_left) {
                    if (grid[z] == EMPTY && not revealed[z]) {
                        to_place.push_back(z);
                        grid[z] = TREE;
                    }
                }
            } else {
                for (int z : extra_walls_right) {
                    if (grid[z] == EMPTY && not revealed[z]) {
                        to_place.push_back(z);
                        grid[z] = TREE;
                    }
                }
            }
        }

        {
            vector<int> additional_trees = search_tree_positions();
            for (int z : additional_trees) {
                if (grid[z] == EMPTY && not revealed[z]) {
                    to_place.push_back(z);
                    grid[z] = TREE;
                }
            }
        }

        if (not success_build_wall || build_flag || not has_spiral_trees) {
            for (int z : to_place) {
                grid[z] = TREE;
            }
            vector<int> spiral_patch = check_can_spiral_patch(Tz);
            if (spiral_patch.size() > 0) {
                fprintf(stderr,
                        "Found spiral patch at turn %d! (count = %d)\n",
                        turn, (int)spiral_patch.size());
                has_spiral_trees = true;
            }
            for (int z : spiral_patch) {
                if (grid[z] == EMPTY && not revealed[z]) {
                    to_place.push_back(z);
                    grid[z] = TREE;
                }
            }
        }
        if (not success_build_wall || build_flag) {
            for (int z : to_place) {
                grid[z] = TREE;
            }

            vector<int> protect_trees = build_protect_trees();
            for (int z : protect_trees) {
                if (grid[z] == EMPTY && not revealed[z]) {
                    to_place.push_back(z);
                    grid[z] = TREE;
                }
            }
        }

        return to_place;
    }

    void output(vector<int> &to_place, bool simulation_mode = false) {
        if (simulation_mode) {
            return;
        }

        cout << to_place.size() << " ";
        for (int z : to_place) {
            int y = z / GRID_SIZE;
            int x = z % GRID_SIZE;
            cout << y - 1 << " " << x - 1 << " ";
            grid[z] = TREE;
        }
        cout << endl;
    }

    vector<int> search_tree_positions() {
        vector<int> res;
        int best_value = INT_MIN;

        for (int mask : g_tree_mask_list) {
            bool is_valid = true;
            vector<int> positions;
            for (int dir = 0; dir < 4 && is_valid; ++dir) {
                int bits = (mask >> (dir * BITS_SIZE)) & ((1 << BITS_SIZE) - 1);
                int tree_cnt = __builtin_popcount(bits);
                if (tree_cnt == 0) {
                    continue;
                }

                for (int i = 0; i < BITS_SIZE && is_valid; ++i) {
                    int ny = py + DY[dir] * (i + 1);
                    int nx = px + DX[dir] * (i + 1);
                    int nz = calc_z(ny, nx);
                    if (grid[nz] <= TREE) {
                        is_valid = false;
                        break;
                    }
                    if ((bits >> i & 1) == 0) {
                        continue;
                    }
                    is_valid &= not revealed[nz];
                    positions.push_back(nz);
                    break;
                }
            }
            if (not is_valid) {
                continue;
            }
            auto [found_flower, reached_count, unrevealed_count] = can_place_multi_tree_fast(positions);
            if (not found_flower) {
                continue;
            }

            auto [dir_kind, kind, revealed_count] =
                calc_revealed_cell_count(positions);
            int value = 0;
            int alpha = 111;
            int beta = 60;
            int gamma = 133;
            double delta = 0.7525106537730449;
            value -= alpha * revealed_count;
            value += beta * pow(dir_kind - kind, delta);
            value += gamma * unrevealed_count;

            if (has_spiral_trees) {
                if (revealed_count <= 0 && dir_kind <= 1) {
                    value -= 500;
                }
            } else {
                for (int z : positions) {
                    if (calc_dist(z, Tz) <= 1) {
                        value += 500;
                    }
                    if (calc_dist(z, Tz) == 2) {
                        value += 200;
                    }
                }
                int flower_block_count = calc_flower_block_count(positions);
                value += 15 * flower_block_count;
            }

            if (best_value < value) {
                best_value = value;
                res = positions;
            }
        }

        return res;
    }

    tuple<int, int, int> calc_revealed_cell_count(vector<int> &positions) {
        for (int z : positions) {
            assert(grid[z] == EMPTY);
            grid[z] = TREE;
        }

        int dir_kind = 0;
        int kind = 0;
        int cnt = 0;
        for (int dir = 0; dir < 4; ++dir) {
            int c = 0;
            int open_count = 0;
            for (int l = 1; l <= N; ++l) {
                int ny = py + DY[dir] * l;
                int nx = px + DX[dir] * l;
                int nz = calc_z(ny, nx);
                int f_dist = calc_dist(nz, calc_z(Ty, Tx));
                if (f_dist == 1 && grid[nz] == TREE) {
                    cnt -= 10;
                }
                if (grid[nz] <= TREE) {
                    break;
                }
                ++open_count;
                if (not revealed[nz]) {
                    ++cnt;
                    ++c;

                    if (f_dist <= 1) {
                        cnt += 100;
                    }
                }
            }
            if (c > 1 + 1 * not has_spiral_trees) {
                ++kind;
            }
            if (open_count > 0) {
                ++dir_kind;
            }
        }

        for (int z : positions) {
            grid[z] = EMPTY;
        }

        return make_tuple(dir_kind, kind, cnt);
    }

    int calc_flower_block_count(vector<int> &positions) {
        grid[Tz] = TREE;
        set<int> changed;
        for (int dir = 0; dir < 4; ++dir) {
            int nz = Tz + DZ[dir];
            if (grid[nz] == EMPTY && not revealed[nz]) {
                changed.insert(nz);
                grid[nz] = TREE;
            }
        }
        for (int z : positions) {
            grid[z] = TREE;
        }
        is_valid_grid();
        grid[Tz] = EMPTY;
        for (int z : positions) {
            grid[z] = EMPTY;
        }
        for (int z : changed) {
            grid[z] = EMPTY;
        }
        int cnt = 0;

        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (g_visited[z] == g_vid && not revealed[z]) {
                    ++cnt;
                }
            }
        }

        return cnt;
    }

    tuple<bool, int, int> can_place_multi_tree_fast(vector<int> &positions) {
        for (int z : positions) {
            assert(grid[z] == EMPTY);
            grid[z] = TREE;
        }
        auto [found_flower, reached_count, unrevealed_count] = is_valid_grid_eval();
        for (int z : positions) {
            grid[z] = EMPTY;
        }

        return {found_flower, reached_count, unrevealed_count};
    }

    bool can_place_multi_tree(vector<int> &positions, int target = -1, int diff_limit = 0) {
        int before_reached_count = 0;
        if (target == Tz) {
            target = -1;
        }
        if (target != -1) {
            is_valid_grid();
            for (int y = 1; y <= N; ++y) {
                for (int x = 1; x <= N; ++x) {
                    int z = calc_z(y, x);
                    if (g_visited[z] == g_vid) {
                        ++before_reached_count;
                    }
                }
            }
        }

        for (int z : positions) {
            assert(grid[z] == EMPTY);
            grid[z] = TREE;
        }
        bool res = is_valid_grid();
        if (target != -1) {
            res &= (g_visited[target] == g_vid);
            int after_reached_count = 0;
            for (int y = 1; y <= N; ++y) {
                for (int x = 1; x <= N; ++x) {
                    int z = calc_z(y, x);
                    if (g_visited[z] == g_vid) {
                        ++after_reached_count;
                    }
                }
            }
            res &= (after_reached_count + (int)positions.size() + diff_limit >=
                    before_reached_count);
        }
        for (int z : positions) {
            grid[z] = EMPTY;
        }

        return res;
    }

    bool can_place_tree(int z) {
        assert(grid[z] == EMPTY);
        grid[z] = TREE;
        bool res = is_valid_grid();
        grid[z] = EMPTY;

        return res;
    }

    bool is_valid_grid() {
        static int que[GRID_SIZE * GRID_SIZE];
        int front = 0, back = 0;

        que[back++] = calc_z(py, px);
        ++g_vid;
        bool found_flower = false;

        while (front < back) {
            int z = que[front++];

            g_visited[z] = g_vid;

            if (z == Tz) {
                found_flower = true;
            }

            for (int dir = 0; dir < 4; ++dir) {
                int nz = z + DZ[dir];
                if (g_visited[nz] == g_vid) {
                    continue;
                }
                if (grid[nz] != EMPTY) {
                    continue;
                }

                g_visited[nz] = g_vid;
                que[back++] = nz;
            }
        }

        return found_flower;
    }

    tuple<bool, int, int> is_valid_grid_eval() {
        static int fque[GRID_SIZE * GRID_SIZE];
        int front = 0, back = 0;

        fque[back++] = calc_z(py, px);
        ++g_vid;
        bool found_flower = false;
        int reached_count = 0;
        int unrevealed_count = 0;

        while (front < back) {
            int z = fque[front++];

            g_visited[z] = g_vid;
            ++reached_count;
            if (not revealed[z] && grid[z] == EMPTY) {
                ++unrevealed_count;
            }

            if (z == Tz) {
                found_flower = true;
            }

            for (int dir = 0; dir < 4; ++dir) {
                int nz = z + DZ[dir];
                if (g_visited[nz] == g_vid) {
                    continue;
                }
                if (grid[nz] != EMPTY) {
                    continue;
                }

                g_visited[nz] = g_vid;
                fque[back++] = nz;
            }
        }

        return {found_flower, reached_count, unrevealed_count};
    }

    vector<int> build_protect_trees() {
        vector<int> protect_trees;

        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (revealed[z]) {
                    continue;
                }
                if (grid[z] != EMPTY) {
                    continue;
                }
                int tree_count = 0;
                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (is_fixed_tree(nz)) {
                        ++tree_count;
                    }
                }
                if (tree_count < 2) {
                    continue;
                }
                if (g_checked[z][tree_count] == g_cid) {
                    continue;
                }
                g_checked[z][tree_count] = g_cid;
                vector<int> positions = check_can_spiral_patch(z);
                if (positions.size() > 0) {
                    g_checked[z][2] = g_cid;
                    g_checked[z][3] = g_cid;
                    g_checked[z][4] = g_cid;
                    return positions;
                }
            }
        }

        return protect_trees;
    }

    vector<int> generate_spiral_trees(int target, int drop_path_size, int failed_limit = 0) {
        auto [spiral_trees, min_tree_count1] = generate_spiral_trees_right(target, drop_path_size, failed_limit);
        auto [spiral_trees_left, min_tree_count2] = generate_spiral_trees_left(target, drop_path_size, failed_limit);
        auto [spiral_trees_right_left, min_tree_count3] = generate_spiral_trees_right_left_move(target, drop_path_size, failed_limit);
        auto [spiral_trees_left_right, min_tree_count4] = generate_spiral_trees_left_right_move(target, drop_path_size, failed_limit);

        min_tree_count1 += g_rng.next(0, g_noise);
        min_tree_count2 += g_rng.next(0, g_noise);
        min_tree_count3 += g_rng.next(0, g_noise);
        min_tree_count4 += g_rng.next(0, g_noise);

        if (spiral_trees.empty() ||
            (spiral_trees_left.size() > 0 &&
             min_tree_count1 > min_tree_count2)) {
            min_tree_count1 = min_tree_count2;
            spiral_trees = spiral_trees_left;
        }
        if (spiral_trees.empty() ||
            (spiral_trees_right_left.size() > 0 &&
             min_tree_count1 > min_tree_count3)) {
            min_tree_count1 = min_tree_count3;
            spiral_trees = spiral_trees_right_left;
        }
        if (spiral_trees.empty() ||
            (spiral_trees_left_right.size() > 0 &&
             min_tree_count1 > min_tree_count4)) {
            min_tree_count1 = min_tree_count4;
            spiral_trees = spiral_trees_left_right;
        }

        return spiral_trees;
    }

    pair<vector<int>, int> generate_spiral_trees_left_right_move(int target_z, int drop_path_size, int failed_limit = 0) {
        int ty = target_z / GRID_SIZE;
        int tx = target_z % GRID_SIZE;
        fprintf(stderr, "Generate spiral trees left =>\n");
        vector<pair<int, int>> spiral_template[8] = {
            // ..T..
            // ...T.
            // .TFT.
            // ..T..
            {{-2, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}},
            // .....
            // ..T..
            // .TF.T
            // ..TT.
            {{-1, 0}, {0, -1}, {0, 2}, {1, 0}, {1, 1}},
            // .....
            // ..T..
            // .TFT.
            // .T...
            // ..T..
            {{-1, 0}, {0, -1}, {0, 1}, {1, -1}, {2, 0}},
            // .....
            // .TT..
            // T.FT.
            // ..T..
            {{-1, -1}, {-1, 0}, {0, -2}, {0, 1}, {1, 0}},
            // ..T..
            // T..T.
            // .TFT.
            // ..T..
            {{-2, 0}, {-1, -2}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}},
            // ...T.
            // ..T..
            // .TF.T
            // ..TT.
            {{-2, 1}, {-1, 0}, {0, -1}, {0, 2}, {1, 0}, {1, 1}},
            // .....
            // ..T..
            // .TFT.
            // .T..T
            // ..T..
            {{-1, 0}, {0, -1}, {0, 1}, {1, -1}, {1, 2}, {2, 0}},
            // .....
            // .TT..
            // T.FT.
            // ..T..
            // .T...
            {{-1, -1}, {-1, 0}, {0, -2}, {0, 1}, {1, 0}, {2, -1}},
        };

        int min_tree_count = INT_MAX;
        vector<int> best_path;
        for (int ptn = 0; ptn < 8; ++ptn) {
            vector<int> positions;
            vector<int> guard_positions;
            int temp_grid[GRID_SIZE * GRID_SIZE];
            memcpy(temp_grid, g_grid_origin, sizeof(g_grid_origin));
            vector<pair<int, int>> &template_dy_dx = spiral_template[ptn];

            for (int i = 0; i < template_dy_dx.size(); ++i) {
                auto [dy, dx] = template_dy_dx[i];
                int ny = ty + dy;
                int nx = tx + dx;
                int nz = calc_z(ny, nx);
                if (is_outside(nz)) {
                    continue;
                }
                if (grid[nz] == WALL) {
                    continue;
                }

                if (calc_dist(nz, calc_z(ty, tx)) == 1 ||
                    calc_chebyshev_distance(nz, calc_z(ty, tx)) == 2) {
                    int tree_count = 0;
                    int empty_z = -1;
                    for (int dir = 0; dir < 4; ++dir) {
                        int nnz = nz + DZ[dir];
                        if (grid[nnz] == TREE) {
                            tree_count++;
                        } else if (grid[nnz] == EMPTY) {
                            empty_z = nnz;
                        }
                    }

                    if (calc_dist(nz, Tz) == 1 && tree_count == 3) {
                        for (int n_dir = 0; n_dir < 4; ++n_dir) {
                            int nnz = empty_z + DZ[n_dir];
                            if (grid[nnz] == TREE) {
                                guard_positions.push_back(nnz);
                            }
                        }
                    } else {
                        guard_positions.push_back(nz);
                    }
                }

                if (grid[nz] == EMPTY) {
                    positions.push_back(nz);
                }

                temp_grid[nz] = TREE;
            }

            int cur_dir = ptn % 4;
            int cz = calc_z(ty, tx);
            bool completed = false;
            int counter[GRID_SIZE * GRID_SIZE];
            memset(counter, 0, sizeof(counter));
            ++g_vid;
            vector<int> spiral_path;
            for (int i = 0; i < N * N && not completed; ++i) {
                if (g_visited[cz] != g_vid) {
                    g_visited[cz] = g_vid;
                    spiral_path.push_back(cz);
                    if (calc_dist(cz, Sz) <= 1) {
                        completed = false;
                        break;
                    }
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = cz + DZ[dir];
                        counter[nz]++;
                    }

                    bool ok = true;
                    int failed_count = 0;
                    for (int gz : guard_positions) {
                        ok &= (counter[gz] >= 2);
                        failed_count += (counter[gz] < 2);
                    }

                    completed = ok || failed_count <= failed_limit;
                }
                if (completed) {
                    break;
                }

                int right_dir = (cur_dir + 1) % 4;
                int rz = cz + DZ[right_dir];
                int left_dir = (cur_dir + 3) % 4;
                int lz = cz + DZ[left_dir];
                int sz = cz + DZ[cur_dir];

                // 左手方の要領で螺旋を描くように進む
                if (temp_grid[rz] == EMPTY) {
                    cz = rz;
                    cur_dir = right_dir;
                } else if (temp_grid[sz] == EMPTY) {
                    cz = sz;
                } else if (temp_grid[lz] == EMPTY) {
                    cz = lz;
                    cur_dir = left_dir;
                } else {
                    cur_dir = (cur_dir + 2) % 4;
                }
            }

            if (completed) {
                cerr << "ptn = " << ptn << ", tree_count = " << positions.size()
                     << ", path_count = " << spiral_path.size() << endl;
            }

            set<int> positions_set;

            if (spiral_path.size() > 0) {
                ++g_uid;
                for (int z : spiral_path) {
                    g_updated[z] = g_uid;
                }
                for (int idx = 0;
                     idx < (int)spiral_path.size() - drop_path_size; ++idx) {
                    int z = spiral_path[idx];
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = z + DZ[dir];
                        if (g_updated[nz] == g_uid) {
                            continue;
                        }
                        if (g_grid_origin[nz] == EMPTY) {
                            positions_set.insert(nz);
                        }
                    }
                }
            }

            vector<int> t_positions =
                vector<int>(positions_set.begin(), positions_set.end());
            bool can_place = can_place_multi_tree(t_positions, Sz, N);
            int reached_count = 0;
            for (int y = 1; y <= N; ++y) {
                for (int x = 1; x <= N; ++x) {
                    int z = calc_z(y, x);
                    if (g_visited[z] == g_vid) {
                        ++reached_count;
                    }
                }
            }
            int value = spiral_path.size() - reached_count;

            if (completed && min_tree_count > value && can_place) {
                min_tree_count = value;
                best_path = spiral_path;
            }
        }

        vector<int> res;
        set<int> positions_set;

        if (best_path.size() > 0) {
            ++g_uid;
            for (int z : best_path) {
                g_updated[z] = g_uid;
            }
            for (int i = 0; i < (int)best_path.size() - drop_path_size; ++i) {
                int z = best_path[i];
                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (g_updated[nz] == g_uid) {
                        continue;
                    }
                    if (g_grid_origin[nz] == EMPTY) {
                        positions_set.insert(nz);
                    }
                }
            }
        }

        return {vector<int>(positions_set.begin(), positions_set.end()), min_tree_count};
    }

    pair<vector<int>, int> generate_spiral_trees_left(int target_z, int drop_path_size, int failed_limit = 0) {
        int ty = target_z / GRID_SIZE;
        int tx = target_z % GRID_SIZE;
        fprintf(stderr, "Generate spiral trees left =>\n");
        vector<pair<int, int>> spiral_template[8] = {
            // ..T..
            // ...T.
            // .TFT.
            // ..T..
            {{-2, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}},
            // .....
            // ..T..
            // .TF.T
            // ..TT.
            {{-1, 0}, {0, -1}, {0, 2}, {1, 0}, {1, 1}},
            // .....
            // ..T..
            // .TFT.
            // .T...
            // ..T..
            {{-1, 0}, {0, -1}, {0, 1}, {1, -1}, {2, 0}},
            // .....
            // .TT..
            // T.FT.
            // ..T..
            {{-1, -1}, {-1, 0}, {0, -2}, {0, 1}, {1, 0}},
            // ..T..
            // T..T.
            // .TFT.
            // ..T..
            {{-2, 0}, {-1, -2}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}},
            // ...T.
            // ..T..
            // .TF.T
            // ..TT.
            {{-2, 1}, {-1, 0}, {0, -1}, {0, 2}, {1, 0}, {1, 1}},
            // .....
            // ..T..
            // .TFT.
            // .T..T
            // ..T..
            {{-1, 0}, {0, -1}, {0, 1}, {1, -1}, {1, 2}, {2, 0}},
            // .....
            // .TT..
            // T.FT.
            // ..T..
            // .T...
            {{-1, -1}, {-1, 0}, {0, -2}, {0, 1}, {1, 0}, {2, -1}},
        };

        int min_tree_count = INT_MAX;
        vector<int> best_path;
        for (int ptn = 0; ptn < 8; ++ptn) {
            vector<int> positions;
            vector<int> guard_positions;
            int temp_grid[GRID_SIZE * GRID_SIZE];
            memcpy(temp_grid, g_grid_origin, sizeof(g_grid_origin));
            vector<pair<int, int>> &template_dy_dx = spiral_template[ptn];

            for (int i = 0; i < template_dy_dx.size(); ++i) {
                auto [dy, dx] = template_dy_dx[i];
                int ny = ty + dy;
                int nx = tx + dx;
                int nz = calc_z(ny, nx);
                if (is_outside(nz)) {
                    continue;
                }
                if (grid[nz] == WALL) {
                    continue;
                }

                if (calc_dist(nz, calc_z(ty, tx)) == 1 ||
                    calc_chebyshev_distance(nz, calc_z(ty, tx)) == 2) {
                    int tree_count = 0;
                    int empty_z = -1;
                    for (int dir = 0; dir < 4; ++dir) {
                        int nnz = nz + DZ[dir];
                        if (grid[nnz] == TREE) {
                            tree_count++;
                        } else if (grid[nnz] == EMPTY) {
                            empty_z = nnz;
                        }
                    }

                    if (calc_dist(nz, Tz) == 1 && tree_count == 3) {
                        for (int n_dir = 0; n_dir < 4; ++n_dir) {
                            int nnz = empty_z + DZ[n_dir];
                            if (grid[nnz] == TREE) {
                                guard_positions.push_back(nnz);
                            }
                        }
                    } else {
                        guard_positions.push_back(nz);
                    }
                }

                if (grid[nz] == EMPTY) {
                    positions.push_back(nz);
                }

                temp_grid[nz] = TREE;
            }

            int cur_dir = ptn % 4;
            int cz = calc_z(ty, tx);
            bool completed = false;
            int counter[GRID_SIZE * GRID_SIZE];
            memset(counter, 0, sizeof(counter));
            ++g_vid;
            vector<int> spiral_path;
            for (int i = 0; i < N * N && not completed; ++i) {
                if (g_visited[cz] != g_vid) {
                    g_visited[cz] = g_vid;
                    spiral_path.push_back(cz);
                    if (calc_dist(cz, Sz) <= 1) {
                        completed = false;
                        break;
                    }
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = cz + DZ[dir];
                        counter[nz]++;
                    }

                    bool ok = true;
                    int failed_count = 0;
                    for (int gz : guard_positions) {
                        ok &= (counter[gz] >= 2);
                        failed_count += (counter[gz] < 2);
                    }

                    completed = ok || failed_count <= failed_limit;
                }
                if (completed) {
                    break;
                }

                int right_dir = (cur_dir + 1) % 4;
                int rz = cz + DZ[right_dir];
                int left_dir = (cur_dir + 3) % 4;
                int lz = cz + DZ[left_dir];
                int sz = cz + DZ[cur_dir];

                // 左手方の要領で螺旋を描くように進む
                if (temp_grid[lz] == EMPTY) {
                    cz = lz;
                    cur_dir = left_dir;
                } else if (temp_grid[sz] == EMPTY) {
                    cz = sz;
                } else if (temp_grid[rz] == EMPTY) {
                    cz = rz;
                    cur_dir = right_dir;
                } else {
                    cur_dir = (cur_dir + 2) % 4;
                }
            }

            if (completed) {
                cerr << "ptn = " << ptn << ", tree_count = " << positions.size()
                     << ", path_count = " << spiral_path.size() << endl;
            }

            set<int> positions_set;

            if (spiral_path.size() > 0) {
                ++g_uid;
                for (int z : spiral_path) {
                    g_updated[z] = g_uid;
                }
                for (int idx = 0;
                     idx < (int)spiral_path.size() - drop_path_size; ++idx) {
                    int z = spiral_path[idx];
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = z + DZ[dir];
                        if (g_updated[nz] == g_uid) {
                            continue;
                        }
                        if (g_grid_origin[nz] == EMPTY) {
                            positions_set.insert(nz);
                        }
                    }
                }
            }

            vector<int> t_positions =
                vector<int>(positions_set.begin(), positions_set.end());
            bool can_place = can_place_multi_tree(t_positions, Sz, N);
            int reached_count = 0;
            for (int y = 1; y <= N; ++y) {
                for (int x = 1; x <= N; ++x) {
                    int z = calc_z(y, x);
                    if (g_visited[z] == g_vid) {
                        ++reached_count;
                    }
                }
            }
            int value = spiral_path.size() - reached_count;

            if (completed && min_tree_count > value && can_place) {
                min_tree_count = value;
                best_path = spiral_path;
            }
        }

        vector<int> res;
        set<int> positions_set;

        if (best_path.size() > 0) {
            ++g_uid;
            for (int z : best_path) {
                g_updated[z] = g_uid;
            }
            for (int i = 0; i < (int)best_path.size() - drop_path_size; ++i) {
                int z = best_path[i];
                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (g_updated[nz] == g_uid) {
                        continue;
                    }
                    if (g_grid_origin[nz] == EMPTY) {
                        positions_set.insert(nz);
                    }
                }
            }
        }

        return {vector<int>(positions_set.begin(), positions_set.end()), min_tree_count};
    }

    pair<vector<int>, vector<int>> divide_areas(vector<int> walls) {
        int temp_grid[GRID_SIZE * GRID_SIZE];
        memcpy(temp_grid, grid, sizeof(grid));

        ++g_uid;
        {
            queue<int> que;
            que.push(Sz);
            while (not que.empty()) {
                int z = que.front();
                que.pop();

                if (g_updated[z] == g_uid) {
                    continue;
                }
                g_updated[z] = g_uid;

                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (temp_grid[nz] != EMPTY) {
                        continue;
                    }
                    que.push(nz);
                }
            }
        }

        for (int wz : walls) {
            temp_grid[wz] = WALL;
        }

        // fill wall area_id
        bool is_wall_area[GRID_SIZE * GRID_SIZE];
        memset(is_wall_area, false, sizeof(is_wall_area));
        {
            queue<int> que;
            que.push(Sz);
            ++g_vid;
            while (not que.empty()) {
                int z = que.front();
                que.pop();

                if (g_visited[z] == g_vid) {
                    continue;
                }
                g_visited[z] = g_vid;
                is_wall_area[z] = true;

                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (temp_grid[nz] != EMPTY) {
                        continue;
                    }
                    que.push(nz);
                }
            }
        }

        // collect inbound area positions
        vector<int> area_positions;
        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (temp_grid[z] != EMPTY) {
                    continue;
                }
                if (is_wall_area[z]) {
                    continue;
                }
                if (g_updated[z] != g_uid) {
                    continue;
                }
                area_positions.push_back(z);
            }
        }

        int L = area_positions.size();
        int min_diff = INT_MAX;
        int best_area_ids[GRID_SIZE * GRID_SIZE];
        memset(best_area_ids, -1, sizeof(best_area_ids));
        if (L < 2) {
            return make_pair(vector<int>(), vector<int>());
        }
        assert(L >= 2);

        for (int t = 0; t < 1000; ++t) {
            int i;
            int j;
            do {
                i = g_rng.next(0, L - 1);
                j = g_rng.next(0, L - 1);
            } while (i == j);

            int cz1 = area_positions[i];
            int cz2 = area_positions[j];
            int area_counter[2] = {0, 0};
            int area_ids[GRID_SIZE * GRID_SIZE];
            memset(area_ids, -1, sizeof(area_ids));
            queue<pair<int, int>> que;
            que.push({cz1, 0});
            que.push({cz2, 1});

            ++g_vid;
            int area_bonus = 0;
            while (not que.empty()) {
                auto [z, area_id] = que.front();
                que.pop();

                if (g_visited[z] == g_vid) {
                    continue;
                }
                g_visited[z] = g_vid;
                area_ids[z] = area_id;
                area_counter[area_id]++;
                if (area_id == 0) {
                    if (z % GRID_SIZE < N / 2) {
                        area_bonus += 1;
                    } else if (z % GRID_SIZE > N - N / 3) {
                        area_bonus -= 1;
                    }
                }
                if (area_id == 1) {
                    if (z % GRID_SIZE > N / 2) {
                        area_bonus += 1;
                    } else if (z % GRID_SIZE < N / 3) {
                        area_bonus -= 1;
                    }
                }

                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (temp_grid[nz] != EMPTY) {
                        continue;
                    }
                    if (g_visited[nz] == g_vid) {
                        continue;
                    }
                    que.push({nz, area_id});
                }
            }

            int area_diff = 20 * abs(area_counter[0] - area_counter[1]) - area_bonus;
            if (min_diff > area_diff && area_counter[0] >= area_counter[1]) {
                min_diff = area_diff;
                memcpy(best_area_ids, area_ids, sizeof(area_ids));
            }
        }

        vector<int> area1;
        vector<int> area2;
        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (best_area_ids[z] == 0) {
                    area1.push_back(z);
                } else if (best_area_ids[z] == 1) {
                    area2.push_back(z);
                }
            }
        }

        return make_pair(area1, area2);
    }

    vector<int> get_wall_area_positions(vector<int> &walls) {
        int temp_grid[GRID_SIZE * GRID_SIZE];
        memcpy(temp_grid, grid, sizeof(grid));
        for (int wz : walls) {
            temp_grid[wz] = WALL;
        }
        ++g_vid;
        vector<int> res;
        queue<int> que;
        que.push(Sz);
        while (not que.empty()) {
            int z = que.front();
            que.pop();

            if (g_visited[z] == g_vid) {
                continue;
            }
            g_visited[z] = g_vid;
            res.push_back(z);

            for (int dir = 0; dir < 4; ++dir) {
                int nz = z + DZ[dir];
                if (temp_grid[nz] != EMPTY) {
                    continue;
                }
                que.push(nz);
            }
        }

        return res;
    }

    Area make_divided_areas() {
        vector<int> walls = build_big_wall();

        {
            set<int> s_walls;
            int now_grid[GRID_SIZE * GRID_SIZE];
            memcpy(now_grid, grid, sizeof(grid));
            for (int z : walls) {
                s_walls.insert(z);
                now_grid[z] = TREE;
            }
            int cur_total_distance = calc_distance_all(Sz, now_grid, true);

            for (int wz : walls) {
                now_grid[wz] = EMPTY;
                int new_total_distance = calc_distance_all(Sz, now_grid, true);

                if (abs(cur_total_distance - new_total_distance) <= N * N * 0.05) {
                    s_walls.erase(wz);
                    cur_total_distance = new_total_distance;
                } else {
                    now_grid[wz] = TREE;
                }
            }
            walls = vector<int>(s_walls.begin(), s_walls.end());
        }

        auto [area1, area2] = divide_areas(walls);
        if (area1.empty() || area2.empty()) {
            fprintf(stderr, "[DEBUG] Failed to divide areas...\n");
            return Area();
        }
        int area_ids[GRID_SIZE * GRID_SIZE];
        memset(area_ids, -1, sizeof(area_ids));
        for (int z : area1) {
            area_ids[z] = 0;
        }
        for (int z : area2) {
            area_ids[z] = 1;
        }

        vector<int> middle_walls;
        int area1_count = area1.size();
        int area2_count = area2.size();
        fprintf(stderr, "area1_count = %d, area2_count = %d\n", area1_count,
                area2_count);
        int cur_grid[GRID_SIZE * GRID_SIZE];
        memcpy(cur_grid, grid, sizeof(grid));
        for (int z : walls) {
            cur_grid[z] = TREE;
        }

        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (area_ids[z] != 0) {
                    continue;
                }
                assert(grid[z] == EMPTY);
                bool is_border = false;
                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (area_ids[nz] == 1 && cur_grid[nz] == EMPTY) {
                        is_border = true;
                        break;
                    }
                }
                if (is_border) {
                    middle_walls.push_back(z);
                    cur_grid[z] = TREE;
                }
            }
        }

        set<int> res;
        for (int z : walls) {
            res.insert(z);
        }
        for (int z : middle_walls) {
            res.insert(z);
        }

        vector<int> area1_walls;
        vector<int> area2_walls;
        bool is_wall[GRID_SIZE * GRID_SIZE];
        memset(is_wall, false, sizeof(is_wall));
        for (int z : walls) {
            is_wall[z] = true;
        }

        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (not is_wall[z]) {
                    continue;
                }
                int empty_count = 0;
                int area_id = -1;
                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (area_ids[nz] == 0 || area_ids[nz] == 1) {
                        empty_count++;
                        area_id = area_ids[nz];
                    }
                }
                if (empty_count != 1) {
                    continue;
                }
                cur_grid[z] = EMPTY;
                fill_vid(Sz, cur_grid);
                cur_grid[z] = TREE;
                int area_count[2] = {0, 0};
                for (int z : area1) {
                    if (g_visited[z] == g_vid) {
                        area_count[0]++;
                    }
                }
                for (int z : area2) {
                    if (g_visited[z] == g_vid) {
                        area_count[1]++;
                    }
                }

                if (area_id == 0 && area_count[0] > 0.5 * area1_count) {
                    area1_walls.push_back(z);
                } else if (area_id == 1 && area_count[1] > 0.5 * area2_count) {
                    area2_walls.push_back(z);
                }
            }
        }
        if (area1_walls.empty() || area2_walls.empty()) {
            fprintf(stderr, "[DEBUG] Failed to find area walls...\n");
            return Area();
        }
        assert(area1_walls.size() > 0);
        assert(area2_walls.size() > 0);

        vector<int> wall_area_positions = get_wall_area_positions(walls);
        int min_dist = INT_MAX;
        int best_a1 = -1;
        int best_a2 = -1;
        int path_length = 0;
        for (int z1 : area1_walls) {
            for (int z2 : area2_walls) {
                int base_dist = calc_dist(z1, z2);
                int dist = 4 * base_dist;
                dist -= z1 / GRID_SIZE;
                dist -= z2 / GRID_SIZE;

                bool c1 = min_dist > dist;
                bool c2 = false;
                if (c1) {
                    cur_grid[z1] = EMPTY;
                    cur_grid[z2] = EMPTY;
                    int max_dist = -1;

                    for (int idx = wall_area_positions.size() - 1; idx >= 0 && max_dist < 2 * N; --idx) {
                        int z = wall_area_positions[idx];
                        if (cur_grid[z] != EMPTY) {
                            continue;
                        }
                        if (area_ids[z] != -1) {
                            continue;
                        }
                        cur_grid[z] = TREE;
                        int dist = calc_distance(z1, z2, cur_grid);
                        cur_grid[z] = EMPTY;

                        max_dist = max(max_dist, dist);
                    }
                    c2 = max_dist > 2 * N;
                    cur_grid[z1] = TREE;
                    cur_grid[z2] = TREE;
                }

                if (c1 && c2) {
                    min_dist = dist;
                    path_length = base_dist;
                    best_a1 = z1;
                    best_a2 = z2;
                }
            }
        }
        res.erase(best_a1);
        res.erase(best_a2);
        assert(best_a1 != -1);
        assert(best_a2 != -1);
        cur_grid[best_a1] = EMPTY;
        cur_grid[best_a2] = EMPTY;
        int best_z = -1;
        int max_dist = -1;

        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (cur_grid[z] != EMPTY) {
                    continue;
                }
                if (area_ids[z] != -1) {
                    continue;
                }
                cur_grid[z] = TREE;
                int dist = calc_distance(best_a1, best_a2, cur_grid);
                cur_grid[z] = EMPTY;

                if (max_dist < dist) {
                    max_dist = dist;
                    best_z = z;
                }
            }
        }
        assert(best_z != -1);
        res.insert(best_z);

        auto [extra_walls_right, extra_path_right] = add_extra_walls(res, area_ids, RIGHT);
        auto [extra_walls_left, extra_path_left] = add_extra_walls(res, area_ids, LEFT);

        Area area;
        area.path_length = path_length;
        area.walls = walls;
        area.all_walls = vector<int>(res.begin(), res.end());
        area.area_count1 = area1.size();
        area.area_count2 = area2.size();
        area.extra_walls_right = extra_walls_right;
        area.extra_walls_left = extra_walls_left;
        area.extra_path_right = extra_path_right;
        area.extra_path_left = extra_path_left;
        memcpy(area.area_ids, area_ids, sizeof(area.area_ids));

        return area;
    }

    pair<vector<int>, vector<int>> add_extra_walls(set<int> cur_walls, int *area_ids, int direction) {
        set<int> res;
        int copy_grid[GRID_SIZE * GRID_SIZE];
        memcpy(copy_grid, grid, sizeof(grid));

        for (int z : cur_walls) {
            grid[z] = TREE;
        }

        double alpha = 2.5;
        int beta = 6;
        double gamma = 0.02;

        int cz = Sz;
        int cur_dir = (direction == RIGHT) ? 1 : 3;
        int limit = 12 * N;
        vector<int> area_path;
        set<int> extra_walls;
        int extra_limit = N * alpha;
        ++g_uid;
        while ((int)area_path.size() < extra_limit && limit-- > 0) {
            int right_dir = (cur_dir + 1) % 4;
            int left_dir = (cur_dir + 3) % 4;
            int rz = cz + DZ[right_dir];
            int lz = cz + DZ[left_dir];
            int sz = cz + DZ[cur_dir];
            if (area_ids[cz] != -1 && g_updated[cz] != g_uid) {
                g_updated[cz] = g_uid;
                area_path.push_back(cz);
                if (cz / GRID_SIZE <= beta) {
                    break;
                }
            }

            if (direction == RIGHT) {
                if (grid[lz] == EMPTY) {
                    cz = lz;
                    cur_dir = left_dir;
                } else if (grid[sz] == EMPTY) {
                    cz = sz;
                } else {
                    cur_dir = (cur_dir + 1) % 4;
                }
            } else {
                if (grid[rz] == EMPTY) {
                    cz = rz;
                    cur_dir = right_dir;
                } else if (grid[sz] == EMPTY) {
                    cz = sz;
                } else {
                    cur_dir = (cur_dir + 3) % 4;
                }
            }
        }

        int cur_reached_count = 0;
        fill_vid(Sz, grid);
        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (g_visited[z] == g_vid) {
                    cur_reached_count++;
                }
            }
        }

        int dir_pattern[4] = {1, 3, 2, 0};
        bool ok = true;
        vector<int> extra_path;

        for (int i = 0; i < (int)area_path.size() && ok; ++i) {
            int z = area_path[i];
            for (int idx = 0; idx < 4; ++idx) {
                int dir = dir_pattern[idx];
                int nz = z + DZ[dir];
                if (grid[nz] != EMPTY) {
                    continue;
                }
                if (area_ids[nz] == -1) {
                    continue;
                }
                if (g_updated[nz] == g_uid) {
                    continue;
                }
                grid[nz] = TREE;
                bool can_place = is_valid_grid();
                int reached_count = 0;
                for (int y = 1; y <= N; ++y) {
                    for (int x = 1; x <= N; ++x) {
                        int tz = calc_z(y, x);
                        if (g_visited[tz] == g_vid) {
                            reached_count++;
                        }
                    }
                }

                grid[nz] = EMPTY;
                if (can_place && abs(reached_count - cur_reached_count) <= N * N * gamma) {
                    cur_reached_count--;
                    res.insert(nz);
                    grid[nz] = TREE;
                    extra_walls.insert(nz);
                } else {
                    ok = false;
                }
            }
            if (ok) {
                extra_path.push_back(z);
            }
        }

        int cur_total_dist = calc_distance_all(Sz, grid);

        for (int ez : extra_walls) {
            grid[ez] = EMPTY;
            int total_dist = calc_distance_all(Sz, grid);
            int dist = calc_distance(Sz, ez, grid);

            if (abs(total_dist - cur_total_dist) <= dist) {
                cur_total_dist = total_dist;
                res.erase(ez);
            } else {
                grid[ez] = TREE;
            }
        }

        memcpy(grid, copy_grid, sizeof(copy_grid));

        return {vector<int>(res.begin(), res.end()), extra_path};
    }

    void fill_vid(int from, int *cur_grid) {
        ++g_vid;
        queue<int> que;
        que.push(from);
        while (not que.empty()) {
            int z = que.front();
            que.pop();

            if (g_visited[z] == g_vid) {
                continue;
            }
            g_visited[z] = g_vid;

            for (int dir = 0; dir < 4; ++dir) {
                int nz = z + DZ[dir];
                if (cur_grid[nz] != EMPTY) {
                    continue;
                }
                if (g_visited[nz] == g_vid) {
                    continue;
                }
                que.push(nz);
            }
        }
    }

    int calc_distance_all(int from, int *cur_grid, bool count_mode = false) {
        queue<pair<int, int>> que;
        que.push({from, 0});
        ++g_vid;
        int total_distance = 0;

        while (not que.empty()) {
            auto [z, dist] = que.front();
            que.pop();

            if (g_visited[z] == g_vid) {
                continue;
            }
            g_visited[z] = g_vid;
            if (count_mode) {
                total_distance++;
            } else {
                total_distance += dist;
            }

            for (int dir = 0; dir < 4; ++dir) {
                int nz = z + DZ[dir];
                if (cur_grid[nz] != EMPTY) {
                    continue;
                }
                if (g_visited[nz] == g_vid) {
                    continue;
                }
                que.push({nz, dist + 1});
            }
        }

        return total_distance;
    }

    int calc_distance(int from, int to, int *cur_grid) {
        queue<pair<int, int>> que;
        que.push({from, 0});
        ++g_vid;

        while (not que.empty()) {
            auto [z, dist] = que.front();
            que.pop();

            if (g_visited[z] == g_vid) {
                continue;
            }
            g_visited[z] = g_vid;

            if (z == to) {
                return dist;
            }

            for (int dir = 0; dir < 4; ++dir) {
                int nz = z + DZ[dir];
                if (cur_grid[nz] != EMPTY) {
                    continue;
                }
                if (g_visited[nz] == g_vid) {
                    continue;
                }
                que.push({nz, dist + 1});
            }
        }

        return 0;
    }

    vector<int> build_big_wall() {
        int cur_dir = 1;
        int cz = Sz;
        ++g_uid;

        for (int i = 0; i < N * N; ++i) {
            int left_dir = (cur_dir + 3) % 4;
            int lz = cz + DZ[left_dir];
            int sz = cz + DZ[cur_dir];
            g_updated[cz] = g_uid;

            // 右手方の要領で壁に沿って進む
            if (grid[lz] == EMPTY) {
                cz = lz;
                cur_dir = left_dir;
            } else if (grid[sz] == EMPTY) {
                cz = sz;
            } else {
                cur_dir = (cur_dir + 1) % 4;
            }
        }

        set<int> wall_path;
        int temp_grid[GRID_SIZE * GRID_SIZE];
        memcpy(temp_grid, grid, sizeof(grid));
        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                if (g_updated[z] != g_uid) {
                    continue;
                }

                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (grid[nz] != EMPTY) {
                        continue;
                    }
                    grid[nz] = TREE;
                    bool can_place = is_valid_grid();
                    grid[nz] = EMPTY;

                    if (grid[nz] == EMPTY && g_updated[nz] != g_uid &&
                        can_place) {
                        wall_path.insert(nz);
                    }
                }
            }
        }
        memcpy(grid, temp_grid, sizeof(temp_grid));

        return vector<int>(wall_path.begin(), wall_path.end());
    }

    pair<vector<int>, int> generate_spiral_trees_right_left_move(int target_z, int drop_path_size, int failed_limit = 0) {
        int ty = target_z / GRID_SIZE;
        int tx = target_z % GRID_SIZE;
        fprintf(stderr, "Generate spiral trees right =>\n");
        vector<pair<int, int>> spiral_template[8] = {
            // ..T..
            // .T...
            // .TFT.
            // ..T..
            {{-2, 0}, {-1, -1}, {0, -1}, {0, 1}, {1, 0}},
            // .....
            // ..TT.
            // .TF.T
            // ..T..
            {{-1, 0}, {-1, 1}, {0, -1}, {0, 2}, {1, 0}},
            // .....
            // ..T..
            // .TFT.
            // ...T.
            // ..T..
            {{-1, 0}, {0, -1}, {0, 1}, {1, 1}, {2, 0}},
            // .....
            // ..T..
            // T.FT.
            // .TT..
            {{-1, 0}, {0, -2}, {0, 1}, {1, -1}, {1, 0}},
            // ..T..
            // .T..T
            // .TFT.
            // ..T..
            {{-2, 0}, {-1, -1}, {-1, 2}, {0, -1}, {0, 1}, {1, 0}},
            // .....
            // ..TT.
            // .TF.T
            // ..T..
            // ...T.
            {{-1, 0}, {-1, 1}, {0, -1}, {0, 2}, {1, 0}, {2, 1}},
            // .....
            // ..T..
            // .TFT.
            // T..T.
            // ..T..
            {{-1, 0}, {0, -1}, {0, 1}, {1, -2}, {1, 1}, {2, 0}},
            // .T...
            // ..T..
            // T.FT.
            // .TT..
            {{-2, -1}, {-1, 0}, {0, -2}, {0, 1}, {1, -1}, {1, 0}},
        };

        int min_tree_count = INT_MAX;
        vector<int> best_path;
        for (int ptn = 0; ptn < 8; ++ptn) {
            vector<int> positions;
            vector<int> guard_positions;
            vector<pair<int, int>> &template_dy_dx = spiral_template[ptn];
            int temp_grid[GRID_SIZE * GRID_SIZE];
            memcpy(temp_grid, g_grid_origin, sizeof(g_grid_origin));

            for (int i = 0; i < template_dy_dx.size(); ++i) {
                auto [dy, dx] = template_dy_dx[i];
                int ny = ty + dy;
                int nx = tx + dx;
                int nz = calc_z(ny, nx);
                if (is_outside(nz)) {
                    continue;
                }
                if (grid[nz] == WALL) {
                    continue;
                }

                if (calc_dist(nz, calc_z(ty, tx)) == 1 ||
                    calc_chebyshev_distance(nz, calc_z(ty, tx)) == 2) {
                    int tree_count = 0;
                    int empty_z = -1;
                    for (int dir = 0; dir < 4; ++dir) {
                        int nnz = nz + DZ[dir];
                        if (grid[nnz] == TREE) {
                            tree_count++;
                        } else if (grid[nnz] == EMPTY) {
                            empty_z = nnz;
                        }
                    }

                    if (calc_dist(nz, Tz) == 1 && tree_count == 3) {
                        for (int n_dir = 0; n_dir < 4; ++n_dir) {
                            int nnz = empty_z + DZ[n_dir];
                            if (grid[nnz] == TREE) {
                                guard_positions.push_back(nnz);
                            }
                        }
                    } else {
                        guard_positions.push_back(nz);
                    }
                }

                if (grid[nz] == EMPTY) {
                    positions.push_back(nz);
                }

                temp_grid[nz] = TREE;
            }

            int cur_dir = ptn % 4;
            int cz = calc_z(ty, tx);
            bool completed = false;
            int counter[GRID_SIZE * GRID_SIZE];
            memset(counter, 0, sizeof(counter));
            ++g_vid;
            vector<int> spiral_path;
            for (int i = 0; i < N * N && not completed; ++i) {
                if (g_visited[cz] != g_vid) {
                    g_visited[cz] = g_vid;
                    spiral_path.push_back(cz);
                    if (calc_dist(cz, Sz) <= 1) {
                        completed = false;
                        break;
                    }
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = cz + DZ[dir];
                        counter[nz]++;
                    }

                    bool ok = true;
                    int failed_count = 0;
                    for (int gz : guard_positions) {
                        ok &= (counter[gz] >= 2);
                        failed_count += (counter[gz] < 2);
                    }

                    completed = ok || (failed_count <= failed_limit);
                }
                if (completed) {
                    break;
                }

                int right_dir = (cur_dir + 1) % 4;
                int rz = cz + DZ[right_dir];
                int left_dir = (cur_dir + 3) % 4;
                int lz = cz + DZ[left_dir];
                int sz = cz + DZ[cur_dir];

                // 右手方の要領で螺旋を描くように進む
                if (temp_grid[lz] == EMPTY) {
                    cz = lz;
                    cur_dir = left_dir;
                } else if (temp_grid[sz] == EMPTY) {
                    cz = sz;
                } else if (temp_grid[rz] == EMPTY) {
                    cz = rz;
                    cur_dir = right_dir;
                } else {
                    cur_dir = (cur_dir + 2) % 4;
                }
            }

            if (completed) {
                cerr << "ptn = " << ptn << ", tree_count = " << positions.size()
                     << ", path_count = " << spiral_path.size() << endl;
            }

            set<int> positions_set;

            if ((int)spiral_path.size() > 0) {
                ++g_uid;
                for (int z : spiral_path) {
                    g_updated[z] = g_uid;
                }
                for (int i = 0; i < (int)spiral_path.size() - drop_path_size; ++i) {
                    int z = spiral_path[i];
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = z + DZ[dir];
                        if (g_updated[nz] == g_uid) {
                            continue;
                        }
                        if (g_grid_origin[nz] == EMPTY) {
                            positions_set.insert(nz);
                        }
                    }
                }
            }

            vector<int> t_positions =
                vector<int>(positions_set.begin(), positions_set.end());
            bool can_place = can_place_multi_tree(t_positions, Sz, N);
            int reached_count = 0;
            for (int y = 1; y <= N; ++y) {
                for (int x = 1; x <= N; ++x) {
                    int z = calc_z(y, x);
                    if (g_visited[z] == g_vid) {
                        ++reached_count;
                    }
                }
            }
            int value = spiral_path.size() - reached_count;

            if (completed && min_tree_count > value && can_place) {
                min_tree_count = value;
                best_path = spiral_path;
            }
        }

        vector<int> res;
        set<int> positions_set;

        if ((int)best_path.size() > 0) {
            ++g_uid;
            for (int z : best_path) {
                g_updated[z] = g_uid;
            }
            for (int i = 0; i < (int)best_path.size() - drop_path_size; ++i) {
                int z = best_path[i];
                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (g_updated[nz] == g_uid) {
                        continue;
                    }
                    if (g_grid_origin[nz] == EMPTY) {
                        positions_set.insert(nz);
                    }
                }
            }
        }

        return {vector<int>(positions_set.begin(), positions_set.end()), min_tree_count};
    }

    pair<vector<int>, int> generate_spiral_trees_right(int target_z, int drop_path_size, int failed_limit = 0) {
        int ty = target_z / GRID_SIZE;
        int tx = target_z % GRID_SIZE;
        fprintf(stderr, "Generate spiral trees right =>\n");
        vector<pair<int, int>> spiral_template[8] = {
            // ..T..
            // .T...
            // .TFT.
            // ..T..
            {{-2, 0}, {-1, -1}, {0, -1}, {0, 1}, {1, 0}},
            // .....
            // ..TT.
            // .TF.T
            // ..T..
            {{-1, 0}, {-1, 1}, {0, -1}, {0, 2}, {1, 0}},
            // .....
            // ..T..
            // .TFT.
            // ...T.
            // ..T..
            {{-1, 0}, {0, -1}, {0, 1}, {1, 1}, {2, 0}},
            // .....
            // ..T..
            // T.FT.
            // .TT..
            {{-1, 0}, {0, -2}, {0, 1}, {1, -1}, {1, 0}},
            // ..T..
            // .T..T
            // .TFT.
            // ..T..
            {{-2, 0}, {-1, -1}, {-1, 2}, {0, -1}, {0, 1}, {1, 0}},
            // .....
            // ..TT.
            // .TF.T
            // ..T..
            // ...T.
            {{-1, 0}, {-1, 1}, {0, -1}, {0, 2}, {1, 0}, {2, 1}},
            // .....
            // ..T..
            // .TFT.
            // T..T.
            // ..T..
            {{-1, 0}, {0, -1}, {0, 1}, {1, -2}, {1, 1}, {2, 0}},
            // .T...
            // ..T..
            // T.FT.
            // .TT..
            {{-2, -1}, {-1, 0}, {0, -2}, {0, 1}, {1, -1}, {1, 0}},
        };

        int min_tree_count = INT_MAX;
        vector<int> best_path;
        for (int ptn = 0; ptn < 8; ++ptn) {
            vector<int> positions;
            vector<int> guard_positions;
            vector<pair<int, int>> &template_dy_dx = spiral_template[ptn];
            int temp_grid[GRID_SIZE * GRID_SIZE];
            memcpy(temp_grid, g_grid_origin, sizeof(g_grid_origin));

            for (int i = 0; i < template_dy_dx.size(); ++i) {
                auto [dy, dx] = template_dy_dx[i];
                int ny = ty + dy;
                int nx = tx + dx;
                int nz = calc_z(ny, nx);
                if (is_outside(nz)) {
                    continue;
                }
                if (grid[nz] == WALL) {
                    continue;
                }

                if (calc_dist(nz, calc_z(ty, tx)) == 1 ||
                    calc_chebyshev_distance(nz, calc_z(ty, tx)) == 2) {
                    int tree_count = 0;
                    int empty_z = -1;
                    for (int dir = 0; dir < 4; ++dir) {
                        int nnz = nz + DZ[dir];
                        if (grid[nnz] == TREE) {
                            tree_count++;
                        } else if (grid[nnz] == EMPTY) {
                            empty_z = nnz;
                        }
                    }

                    if (calc_dist(nz, Tz) == 1 && tree_count == 3) {
                        for (int n_dir = 0; n_dir < 4; ++n_dir) {
                            int nnz = empty_z + DZ[n_dir];
                            if (grid[nnz] == TREE) {
                                guard_positions.push_back(nnz);
                            }
                        }
                    } else {
                        guard_positions.push_back(nz);
                    }
                }

                if (grid[nz] == EMPTY) {
                    positions.push_back(nz);
                }

                temp_grid[nz] = TREE;
            }

            int cur_dir = ptn % 4;
            int cz = calc_z(ty, tx);
            bool completed = false;
            int counter[GRID_SIZE * GRID_SIZE];
            memset(counter, 0, sizeof(counter));
            ++g_vid;
            vector<int> spiral_path;
            for (int i = 0; i < N * N && not completed; ++i) {
                if (g_visited[cz] != g_vid) {
                    g_visited[cz] = g_vid;
                    spiral_path.push_back(cz);
                    if (calc_dist(cz, Sz) <= 1) {
                        completed = false;
                        break;
                    }
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = cz + DZ[dir];
                        counter[nz]++;
                    }

                    bool ok = true;
                    int failed_count = 0;
                    for (int gz : guard_positions) {
                        ok &= (counter[gz] >= 2);
                        failed_count += (counter[gz] < 2);
                    }

                    completed = ok || failed_count <= failed_limit;
                }
                if (completed) {
                    break;
                }

                int right_dir = (cur_dir + 1) % 4;
                int rz = cz + DZ[right_dir];
                int left_dir = (cur_dir + 3) % 4;
                int lz = cz + DZ[left_dir];
                int sz = cz + DZ[cur_dir];

                // 右手方の要領で螺旋を描くように進む
                if (temp_grid[rz] == EMPTY) {
                    cz = rz;
                    cur_dir = right_dir;
                } else if (temp_grid[sz] == EMPTY) {
                    cz = sz;
                } else if (temp_grid[lz] == EMPTY) {
                    cz = lz;
                    cur_dir = left_dir;
                } else {
                    cur_dir = (cur_dir + 2) % 4;
                }
            }

            if (completed) {
                cerr << "ptn = " << ptn << ", tree_count = " << positions.size()
                     << ", path_count = " << spiral_path.size() << endl;
            }

            set<int> positions_set;

            if ((int)spiral_path.size() > 0) {
                ++g_uid;
                for (int z : spiral_path) {
                    g_updated[z] = g_uid;
                }
                for (int i = 0; i < (int)spiral_path.size() - drop_path_size; ++i) {
                    int z = spiral_path[i];
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = z + DZ[dir];
                        if (g_updated[nz] == g_uid) {
                            continue;
                        }
                        if (g_grid_origin[nz] == EMPTY) {
                            positions_set.insert(nz);
                        }
                    }
                }
            }

            vector<int> t_positions =
                vector<int>(positions_set.begin(), positions_set.end());
            bool can_place = can_place_multi_tree(t_positions, Sz, N);
            int reached_count = 0;
            for (int y = 1; y <= N; ++y) {
                for (int x = 1; x <= N; ++x) {
                    int z = calc_z(y, x);
                    if (g_visited[z] == g_vid) {
                        ++reached_count;
                    }
                }
            }
            int value = spiral_path.size() - reached_count;

            if (completed && min_tree_count > value && can_place) {
                min_tree_count = value;
                best_path = spiral_path;
            }
        }

        vector<int> res;
        set<int> positions_set;

        if ((int)best_path.size() > 0) {
            ++g_uid;
            for (int z : best_path) {
                g_updated[z] = g_uid;
            }
            for (int i = 0; i < (int)best_path.size() - drop_path_size; ++i) {
                int z = best_path[i];
                for (int dir = 0; dir < 4; ++dir) {
                    int nz = z + DZ[dir];
                    if (g_updated[nz] == g_uid) {
                        continue;
                    }
                    if (g_grid_origin[nz] == EMPTY) {
                        positions_set.insert(nz);
                    }
                }
            }
        }

        return {vector<int>(positions_set.begin(), positions_set.end()), min_tree_count};
    }

    bool is_fixed_tree(int z) {
        return is_outside(z) || grid[z] == WALL || (grid[z] == TREE && revealed[z]);
    }

    vector<int> check_can_spiral_patch(int target) {
        int ty = target / GRID_SIZE;
        int tx = target % GRID_SIZE;
        vector<int> res;
        int min_tree_count = INT_MAX;

        for (SpiralPart &part : g_spiral_part_list) {
            bool all_fixed = true;
            for (int dz : part.dz_list) {
                all_fixed &= is_fixed_tree(target + dz);
            }
            if (target == Tz && part.dz_list.size() == 4) {
                break;
            }

            if (all_fixed) {
                vector<int> positions;
                bool ok = true;

                for (int idx = 0; idx < (int)part.template_dy.size(); ++idx) {
                    int ny = ty + part.template_dy[idx];
                    int nx = tx + part.template_dx[idx];
                    int nz = calc_z(ny, nx);
                    if (not is_inside(nz)) {
                        continue;
                    }
                    if (grid[nz] == EMPTY && revealed[nz]) {
                        ok = false;
                        break;
                    }

                    if (grid[nz] == EMPTY && not revealed[nz]) {
                        positions.push_back(nz);
                    }
                }

                int tree_value = 5 * positions.size();
                for (int pz : positions) {
                    int not_revealed_tree_count = 0;
                    for (int dir = 0; dir < 4; ++dir) {
                        int nz = pz + DZ[dir];
                        if (grid[nz] == TREE && not revealed[nz]) {
                            not_revealed_tree_count++;
                        }
                    }
                    for (int dir = 4; dir < 8; ++dir) {
                        int nz = pz + DZ[dir];
                        if (grid[nz] == TREE && not revealed[nz]) {
                            tree_value -= 2;
                        }
                    }
                    if (not_revealed_tree_count >= 2) {
                        tree_value += 5;
                    }
                }

                if (ok && can_place_multi_tree(positions, target) && min_tree_count > tree_value) {
                    min_tree_count = tree_value;
                    res = positions;
                }
            }
        }

        return res;
    }
};

class LocalSimulator {
   public:
    int turn;
    int py;
    int px;
    int target;
    vector<int> new_revealed;
    int current_map[GRID_SIZE * GRID_SIZE];
    int distance[GRID_SIZE * GRID_SIZE];
    bool revealed[GRID_SIZE * GRID_SIZE];
    queue<int> next_p;

    void init(vector<int> p_list) {
        reset_global_vars();

        turn = 0;
        py = 1;
        px = N / 2 + 1;
        target = UNDEFINED;
        memset(current_map, WALL, sizeof(current_map));
        memset(revealed, false, sizeof(revealed));
        revealed[calc_z(py, px)] = true;
        new_revealed.clear();
        new_revealed.push_back(calc_z(py, px));

        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                current_map[z] = g_grid_origin[z];
            }
        }

        for (int i = 0; i < N * N - 1; ++i) {
            next_p.push(p_list[i]);
        }
    }

    int run(State state, vector<int> p_list, bool simulation_mode = false) {
        init(p_list);

        while (true) {
            TurnInfo info = read_turn_info();
            state.update(info);

            if (py == Ty && px == Tx) {
                break;
            }

            vector<int> to_place = state.step();
            state.output(to_place, simulation_mode);

            step(to_place);
            state.turn++;

            if (simulation_mode && state.has_spiral_trees && state.success_build_wall) {
                return 1000 * N;
            }
        }

        return turn;
    }

    void step(vector<int> &tree_positions) {
        for (int z : tree_positions) {
            int y = z / GRID_SIZE;
            int x = z % GRID_SIZE;
            assert(not revealed[z]);
            assert(current_map[z] == EMPTY);
            assert(y != Ty || x != Tx);

            current_map[z] = TREE;
        }

        new_revealed.clear();
        bool changed = false;
        int pz = calc_z(py, px);
        for (int dir = 0; dir < 4; ++dir) {
            int nz = pz + DZ[dir];
            while (current_map[nz] != WALL) {
                if (not revealed[nz]) {
                    revealed[nz] = true;
                    new_revealed.push_back(nz);

                    if (current_map[nz] == TREE) {
                        changed = true;
                    }
                }
                if (current_map[nz] == TREE) {
                    break;
                }

                nz += DZ[dir];
            }
        }
        if (changed) {
            int temp_target = target;
            target = UNDEFINED;
            change_target(temp_target);
        }
        if (revealed[Tz]) {
            change_target(Tz);
        }
        if (target != UNDEFINED && g_updated[pz] != g_uid) {
            target = UNDEFINED;
        }
        if (target == UNDEFINED || (target != Tz && revealed[target])) {
            change_target(pz);

            while (not next_p.empty()) {
                int pz = next_p.front();
                next_p.pop();
                if (revealed[pz]) {
                    continue;
                }
                if (g_updated[pz] != g_uid) {
                    assert(g_updated[pz] != g_uid);
                    continue;
                }
                change_target(pz);
                break;
            }
        }
        int min_dist = INT_MAX;
        int next_dir = -1;
        for (int dir = 0; dir < 4; ++dir) {
            int nz = calc_z(py + DY[dir], px + DX[dir]);
            if (g_updated[nz] == g_uid && min_dist > distance[nz]) {
                min_dist = distance[nz];
                next_dir = dir;
            }
        }
        assert(next_dir != -1);
        py += DY[next_dir];
        px += DX[next_dir];

        ++turn;
    }

    void change_target(int cur_target) {
        if (target == cur_target) {
            return;
        }
        target = cur_target;
        if (cur_target == UNDEFINED) {
            return;
        }
        ++g_uid;
        queue<pair<int, int>> que;
        que.push(make_pair(target, 0));
        g_updated[target] = g_uid;
        while (not que.empty()) {
            auto [z, dist] = que.front();
            que.pop();

            distance[z] = dist;

            for (int dir = 0; dir < 4; ++dir) {
                int nz = z + DZ[dir];
                if (g_updated[nz] == g_uid) {
                    continue;
                }
                if (current_map[nz] == WALL) {
                    continue;
                }
                if (revealed[nz] && current_map[nz] == TREE) {
                    continue;
                }

                g_updated[nz] = g_uid;
                que.push(make_pair(nz, dist + 1));
            }
        }
    }

    TurnInfo read_turn_info() {
        TurnInfo info;
        info.py = py;
        info.px = px;
        info.new_revealed = new_revealed;

        return info;
    }
};

class Solver {
   public:
    void run() {
        g_timer.begin();
        double start_at = g_timer.get_time();
        load_data();
        setup();

        show_grid();
        bool use_big_wall = local_check();

        if (g_local_mode) {
            local_simulation(g_p_list, use_big_wall);
        } else {
            start_game(use_big_wall);
        }
        double end_at = g_timer.get_time();
        double elapsed = end_at - start_at;
        fprintf(stderr, "time = %.6f\n", elapsed);
        if (elapsed > TIME_LIMIT) {
            fprintf(stderr, "[WARNING] Time Limit Exceeded\n");
        }
    }

   private:
    void setup_p_list() {
        vector<int> p_list;
        for (int y = 1; y <= N; ++y) {
            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                p_list.push_back(z);
            }
        }

        // シャッフル
        std::random_device seed_gen;
        std::mt19937 engine(seed_gen());
        std::shuffle(p_list.begin(), p_list.end(), engine);

        int tz_idx = -1;
        for (int i = 0; i < (int)p_list.size(); ++i) {
            if (p_list[i] == Tz) {
                tz_idx = i;
                break;
            }
        }
        swap(p_list[tz_idx], p_list[N * N / 2]);
        g_p_list = p_list;
    }

    bool local_check() {
        int score2 = local_simulation(g_p_list, true, true);
        return score2 > 3 * N * N * N * 0.02;
    }
    void start_game(bool use_big_wall) {
        State state = build_initial_state();
        state.use_big_wall = use_big_wall;
        ++g_cid;

        while (true) {
            TurnInfo info = read_turn_info();
            state.update(info);

            if (g_timer.get_time() > TIME_LIMIT) {
                cout << -1 << endl;
                fprintf(stderr, "[DEBUG] Force Stop, %.4f\n", g_timer.get_time());
                break;
            }

            if (info.py == Ty && info.px == Tx) {
                break;
            }

            vector<int> to_place = state.step();
            state.output(to_place);

            state.turn++;
        }
    }

    TurnInfo read_turn_info() {
        TurnInfo info;
        cin >> info.py >> info.px;
        ++info.py;
        ++info.px;

        int n;
        cin >> n;
        info.new_revealed.resize(n);
        for (int i = 0; i < n; ++i) {
            int y, x;
            cin >> y >> x;
            ++y;
            ++x;
            info.new_revealed[i] = calc_z(y, x);
        }

        return info;
    }

    State build_initial_state() {
        reset_global_vars();

        State state;
        state.turn = 0;
        state.py = Ty;
        state.px = Tx;
        state.max_py = 0;
        state.success_build_wall = false;
        state.finished_build_walls = false;
        state.extra_walls_right.clear();
        state.extra_walls_left.clear();
        memcpy(state.grid, g_grid_origin, sizeof(g_grid_origin));
        memset(state.revealed, 0, sizeof(state.revealed));
        memset(state.wall_placed, false, sizeof(state.wall_placed));
        return state;
    }

    void load_data() {
        fprintf(stderr, "Load data =>\n");
        memset(g_visited, -1, sizeof(g_visited));
        memset(g_updated, -1, sizeof(g_updated));
        memset(g_checked, -1, sizeof(g_checked));
        memset(g_grid, WALL, sizeof(g_grid));
        g_uid = 0;
        g_vid = 0;
        g_cid = 0;

        cin >> N >> Ty >> Tx;
        ++Ty;
        ++Tx;
        Tz = calc_z(Ty, Tx);
        fprintf(stderr, "N = %d\n", N);
        int tree_count = 0;
        Sz = calc_z(1, N / 2 + 1);

        for (int y = 1; y <= N; ++y) {
            string row;
            cin >> row;

            for (int x = 1; x <= N; ++x) {
                int z = calc_z(y, x);
                char c = row[x - 1];

                if (c == '.') {
                    g_grid[z] = EMPTY;
                } else {
                    g_grid[z] = TREE;
                    ++tree_count;
                }
            }
        }

        fprintf(stderr, "tree_rate = %.3f\n",
                (double)tree_count * 100.0 / (N * N));

        memcpy(g_grid_origin, g_grid, sizeof(g_grid));

        if (g_local_mode) {
            g_p_list.clear();
            for (int i = 0; i < N * N - 1; ++i) {
                int y, x;
                cin >> y >> x;
                ++y;
                ++x;
                int z = calc_z(y, x);
                g_p_list.push_back(z);
            }
        }
    }

    void setup() {
        fprintf(stderr, "Setup =>\n");
        setup_tree_mask_list();
        setup_spiral_parts();
        if (g_p_list.empty()) {
            setup_p_list();
        }
    }

    void setup_spiral_parts() {
        g_spiral_part_list.clear();

        {
            // ..T...
            // .T..T.
            // .#XT..
            // ..#...
            SpiralPart part;
            part.dz_list = {DZ[2], DZ[3]};
            part.template_dy = {-2, -1, -1, 0};
            part.template_dx = {0, -1, 2, 1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..T...
            // T..T..
            // .TX#..
            // ..#...
            SpiralPart part;
            part.dz_list = {DZ[1], DZ[2]};
            part.template_dy = {-2, -1, -1, 0};
            part.template_dx = {0, -2, 1, -1};
            g_spiral_part_list.push_back(part);
        }
        // pattern 2
        {
            // ......
            // ..#T..
            // .#X.T.
            // ..T...
            // ...T..
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[3]};
            part.template_dy = {-1, 0, 1, 2};
            part.template_dx = {1, 2, 0, 1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ....T..
            // ...T...
            // ..#X.T.
            // ...#T..
            SpiralPart part;
            part.dz_list = {DZ[2], DZ[3]};
            part.template_dy = {-2, -1, 0, 1};
            part.template_dx = {1, 0, 2, 1};
            g_spiral_part_list.push_back(part);
        }
        // pattern 3
        {
            // .....
            // ..#..
            // .TX#.
            // T..T.
            // ..T..
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[1]};
            part.template_dy = {0, 1, 1, 2};
            part.template_dx = {-1, -2, 1, 0};
            g_spiral_part_list.push_back(part);
        }
        {
            // ......
            // ..#...
            // .#XT..
            // .T..T.
            // ..T...
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[3]};
            part.template_dy = {0, 1, 1, 2};
            part.template_dx = {1, -1, 2, 0};
            g_spiral_part_list.push_back(part);
        }
        // pattern 4
        {
            // ..T...
            // ...#..
            // .T.X#.
            // ..TT..
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[1]};
            part.template_dy = {-2, 0, 1, 1};
            part.template_dx = {-1, -2, -1, 0};
            g_spiral_part_list.push_back(part);
        }
        {
            // .....
            // .T#..
            // T.X#.
            // ..T..
            // .T...
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[1]};
            part.template_dy = {-1, 0, 1, 2};
            part.template_dx = {-1, -2, 0, -1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..#..
            // ...T.
            // .TX#.
            // ..#..
            SpiralPart part;
            part.dz_list = {2 * DZ[0], DZ[1], DZ[2]};
            part.template_dy = {-1, 0};
            part.template_dx = {1, -1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..#..
            // .T...
            // .#XT.
            // ..#..
            SpiralPart part;
            part.dz_list = {2 * DZ[0], DZ[2], DZ[3]};
            part.template_dy = {-1, 0};
            part.template_dx = {-1, 1};
            g_spiral_part_list.push_back(part);
        }
        {
            // .....
            // ..#T.
            // .#X.#
            // ..T..
            // .....
            SpiralPart part;
            part.dz_list = {DZ[0], 2 * DZ[1], DZ[3]};
            part.template_dy = {-1, 1};
            part.template_dx = {1, 0};
            g_spiral_part_list.push_back(part);
        }
        {
            // .....
            // ..T..
            // .#X.#
            // ..#T.
            // .....
            SpiralPart part;
            part.dz_list = {2 * DZ[1], DZ[2], DZ[3]};
            part.template_dy = {-1, 1};
            part.template_dx = {0, 1};
            g_spiral_part_list.push_back(part);
        }
        {
            // .....
            // ..#..
            // .TX#.
            // ...T.
            // ..#..
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[1], 2 * DZ[2]};
            part.template_dy = {0, 1};
            part.template_dx = {-1, 1};
            g_spiral_part_list.push_back(part);
        }
        {
            // .....
            // ..#..
            // .#XT.
            // .T...
            // ..#..
            SpiralPart part;
            part.dz_list = {DZ[0], 2 * DZ[2], DZ[3]};
            part.template_dy = {0, 1};
            part.template_dx = {1, -1};
            g_spiral_part_list.push_back(part);
        }
        {
            // .....
            // .T#..
            // #.X#.
            // ..T..
            // .....
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[1], 2 * DZ[3]};
            part.template_dy = {-1, 1};
            part.template_dx = {-1, 0};
            g_spiral_part_list.push_back(part);
        }
        {
            // .....
            // ..T..
            // #.X#.
            // .T#..
            // .....
            SpiralPart part;
            part.dz_list = {DZ[1], DZ[2], 2 * DZ[3]};
            part.template_dy = {-1, 1};
            part.template_dx = {0, -1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..T...
            // .T..T.
            // .#XT..
            // .#.#..
            // ..#...
            SpiralPart part;
            part.dz_list = {DZ[2] + DZ[1], DZ[2] + DZ[3], 2 * DZ[2], DZ[3]};
            part.template_dy = {-2, -1, -1, 0};
            part.template_dx = {0, -1, 2, 1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..T...
            // T..T..
            // .TX#..
            // .#.#..
            // ..#...
            SpiralPart part;
            part.dz_list = {DZ[2] + DZ[1], DZ[2] + DZ[3], 2 * DZ[2], DZ[1]};
            part.template_dy = {-2, -1, -1, 0};
            part.template_dx = {0, -2, 1, -1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ......
            // ..##T.
            // .#.X.T.
            // ..#T...
            // ....T..
            SpiralPart part;
            part.dz_list = {DZ[0] + DZ[3], DZ[0], 2 * DZ[3], DZ[2] + DZ[3]};
            part.template_dy = {-1, 0, 1, 2};
            part.template_dx = {1, 2, 0, 1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..T...
            // .T....
            // .#X#..
            // ..#...
            SpiralPart part;
            part.dz_list = {DZ[1], DZ[2], DZ[3]};
            part.template_dy = {-2, -1};
            part.template_dx = {0, -1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..T...
            // ...T..
            // .#X#..
            // ..#...
            SpiralPart part;
            part.dz_list = {DZ[1], DZ[2], DZ[3]};
            part.template_dy = {-2, -1};
            part.template_dx = {0, 1};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..#..
            // .#X#.
            // ...T.
            // ..T..
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[1], DZ[3]};
            part.template_dy = {1, 2};
            part.template_dx = {1, 0};
            g_spiral_part_list.push_back(part);
        }
        {
            // ..#..
            // .#X#.
            // .T...
            // ..T..
            SpiralPart part;
            part.dz_list = {DZ[0], DZ[1], DZ[3]};
            part.template_dy = {1, 2};
            part.template_dx = {-1, 0};
            g_spiral_part_list.push_back(part);
        }
    }

    void setup_tree_mask_list() {
        g_tree_mask_list.clear();
        for (int mask = 0; mask < (1 << (4 * BITS_SIZE)); ++mask) {
            bool is_valid = true;
            for (int dir = 0; dir < 4 && is_valid; ++dir) {
                int bits = (mask >> (dir * BITS_SIZE)) & ((1 << BITS_SIZE) - 1);
                int tree_cnt = __builtin_popcount(bits);
                if (tree_cnt == 0) {
                    continue;
                }
                if (tree_cnt >= 2) {
                    is_valid = false;
                    break;
                }
            }
            if (is_valid) {
                g_tree_mask_list.push_back(mask);
            }
        }
    }

    int local_simulation(vector<int> p_list, bool use_big_wall = false, bool simulation_mode = false) {
        State state = build_initial_state();
        state.use_big_wall = use_big_wall;
        LocalSimulator simulator;
        int score = simulator.run(state, p_list, simulation_mode);

        return score;
    }

    void show_grid() {
        for (int y = 0; y <= N + 1; ++y) {
            for (int x = 0; x <= N + 1; ++x) {
                int z = calc_z(y, x);
                int v = g_grid[z];

                if (v == TREE) {
                    cerr << 'T';
                } else if (y == Ty && x == Tx) {
                    cerr << 'P';
                } else if (v == EMPTY) {
                    cerr << '.';
                } else if (v == WALL) {
                    cerr << '#';
                } else {
                    assert(false);
                }
            }
            cerr << endl;
        }
    }
};

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    if (getenv("LOCAL")) {
        g_local_mode = true;
    }

    Solver solver;
    solver.run();

    return 0;
}
